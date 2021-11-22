

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>
#include <vips/threadpool.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _WriteBuffer {
    struct _Write *write;

    REGION *region;
    Rect area;
    im_semaphore_t go;
    im_semaphore_t nwrite;
    im_semaphore_t done;
    int write_errno;
    GThread *thread;
    gboolean kill;
} WriteBuffer;

typedef struct _Write {
    VipsImage *im;

    WriteBuffer *buf;
    WriteBuffer *buf_back;

    int x;
    int y;

    int tile_width;
    int tile_height;
    int nlines;

    VipsRegionWrite write_fn;
    void *a;
} Write;

int im__wbuffer2 = 0;

typedef struct _WriteThreadState {
    VipsThreadState parent_object;

    WriteBuffer *buf;
} WriteThreadState;

typedef struct _WriteThreadStateClass {
    VipsThreadStateClass parent_class;

} WriteThreadStateClass;

G_DEFINE_TYPE(WriteThreadState, write_thread_state, VIPS_TYPE_THREAD_STATE);

static void write_thread_state_class_init(WriteThreadStateClass *class) {
    VipsObjectClass *object_class = VIPS_OBJECT_CLASS(class);

    object_class->nickname = "writethreadstate";
    object_class->description = _("per-thread state for sinkdisc");
}

static void write_thread_state_init(WriteThreadState *state) {
    state->buf = NULL;
}

static VipsThreadState *write_thread_state_new(VipsImage *im, void *a) {
    return (VIPS_THREAD_STATE(vips_object_new(write_thread_state_get_type(), vips_thread_state_set, im, a)));
}

static void wbuffer_free(WriteBuffer *wbuffer) {

    if (wbuffer->thread) {
        wbuffer->kill = TRUE;
        im_semaphore_up(&wbuffer->go);

        (void)g_thread_join(wbuffer->thread);
        VIPS_DEBUG_MSG("wbuffer_free: g_thread_join()\n");

        wbuffer->thread = NULL;
    }

    IM_FREEF(im_region_free, wbuffer->region);
    im_semaphore_destroy(&wbuffer->go);
    im_semaphore_destroy(&wbuffer->nwrite);
    im_semaphore_destroy(&wbuffer->done);
    im_free(wbuffer);
}

static void wbuffer_write(WriteBuffer *wbuffer) {
    Write *write = wbuffer->write;

    VIPS_DEBUG_MSG("wbuffer_write: %d bytes from wbuffer %p\n", wbuffer->region->bpl * wbuffer->area.height, wbuffer);

    wbuffer->write_errno = write->write_fn(wbuffer->region, &wbuffer->area, write->a);
}

#ifdef HAVE_THREADS

static void *wbuffer_write_thread(void *data) {
    WriteBuffer *wbuffer = (WriteBuffer *)data;

    for (;;) {

        im_semaphore_down(&wbuffer->go);

        if (wbuffer->kill) {
            break;
        }

        im_semaphore_downn(&wbuffer->nwrite, 0);

        wbuffer_write(wbuffer);

        im_semaphore_up(&wbuffer->done);
    }

    return (NULL);
}
#endif

static WriteBuffer *wbuffer_new(Write *write) {
    WriteBuffer *wbuffer;

    if (!(wbuffer = IM_NEW(NULL, WriteBuffer))) {
        return (NULL);
    }
    wbuffer->write = write;
    wbuffer->region = NULL;
    im_semaphore_init(&wbuffer->go, 0, "go");
    im_semaphore_init(&wbuffer->nwrite, 0, "nwrite");
    im_semaphore_init(&wbuffer->done, 0, "done");
    wbuffer->write_errno = 0;
    wbuffer->thread = NULL;
    wbuffer->kill = FALSE;

    if (!(wbuffer->region = im_region_create(write->im))) {
        wbuffer_free(wbuffer);
        return (NULL);
    }

    im__region_no_ownership(wbuffer->region);

#ifdef HAVE_THREADS

    if (!(wbuffer->thread = g_thread_create(wbuffer_write_thread, wbuffer, TRUE, NULL))) {
        im_error("wbuffer_new", "%s", _("unable to create thread"));
        wbuffer_free(wbuffer);
        return (NULL);
    }
#endif

    return (wbuffer);
}

static int wbuffer_flush(Write *write) {
    VIPS_DEBUG_MSG("wbuffer_flush:\n");

    if (write->buf->area.top > 0) {
        im_semaphore_down(&write->buf_back->done);

        if (write->buf_back->write_errno) {
            im_error_system(write->buf_back->write_errno, "wbuffer_write", "%s", _("write failed"));
            return (-1);
        }
    }

#ifdef HAVE_THREADS
    im_semaphore_up(&write->buf->go);
#else

    wbuffer_write(write->buf);
#endif

    return (0);
}

static int wbuffer_position(WriteBuffer *wbuffer, int top, int height) {
    Rect image, area;
    int result;

    image.left = 0;
    image.top = 0;
    image.width = wbuffer->write->im->Xsize;
    image.height = wbuffer->write->im->Ysize;

    area.left = 0;
    area.top = top;
    area.width = wbuffer->write->im->Xsize;
    area.height = height;

    im_rect_intersectrect(&area, &image, &wbuffer->area);

    im__region_take_ownership(wbuffer->region);

    result = im_region_buffer(wbuffer->region, &wbuffer->area);

    im__region_no_ownership(wbuffer->region);

    g_assert(!wbuffer->region->buffer->done);

    return (result);
}

static gboolean wbuffer_allocate_fn(VipsThreadState *state, void *a, gboolean *stop) {
    WriteThreadState *wstate = (WriteThreadState *)state;
    Write *write = (Write *)a;

    Rect image;
    Rect tile;

    VIPS_DEBUG_MSG("wbuffer_allocate_fn:\n");

    if (write->x >= write->buf->area.width) {
        write->x = 0;
        write->y += write->tile_height;

        if (write->y >= IM_RECT_BOTTOM(&write->buf->area)) {

            if (wbuffer_flush(write)) {
                return (-1);
            }

            if (write->y >= write->im->Ysize) {
                *stop = TRUE;
                return (0);
            }

            {
                WriteBuffer *t;

                t = write->buf;
                write->buf = write->buf_back;
                write->buf_back = t;
            }

            if (wbuffer_position(write->buf, write->y, write->nlines)) {
                return (-1);
            }
        }
    }

    image.left = 0;
    image.top = 0;
    image.width = write->im->Xsize;
    image.height = write->im->Ysize;
    tile.left = write->x;
    tile.top = write->y;
    tile.width = write->tile_width;
    tile.height = write->tile_height;
    im_rect_intersectrect(&image, &tile, &state->pos);
    wstate->buf = write->buf;

    im_semaphore_upn(&write->buf->nwrite, -1);

    write->x += write->tile_width;

    return (0);
}

static int wbuffer_work_fn(VipsThreadState *state, void *a) {
    WriteThreadState *wstate = (WriteThreadState *)state;

    VIPS_DEBUG_MSG("wbuffer_work_fn:\n");

    if (im_prepare_to(state->reg, wstate->buf->region, &state->pos, state->pos.left, state->pos.top)) {
        return (-1);
    }

    im_semaphore_upn(&wstate->buf->nwrite, 1);

    return (0);
}

static int wbuffer_progress_fn(void *a) {
    Write *write = (Write *)a;

    VIPS_DEBUG_MSG("wbuffer_progress_fn: %d x %d\n", write->tile_width, write->tile_height);

    if (im__handle_eval(write->im, write->tile_width, write->tile_height)) {
        return (-1);
    }

    return (0);
}

static void write_init(Write *write, VipsImage *im, VipsRegionWrite write_fn, void *a) {
    write->im = im;
    write->buf = wbuffer_new(write);
    write->buf_back = wbuffer_new(write);
    write->x = 0;
    write->y = 0;
    write->write_fn = write_fn;
    write->a = a;

    vips_get_tile_size(im, &write->tile_width, &write->tile_height, &write->nlines);
}

static void write_free(Write *write) {
    IM_FREEF(wbuffer_free, write->buf);
    IM_FREEF(wbuffer_free, write->buf_back);
}

int vips_sink_disc(VipsImage *im, VipsRegionWrite write_fn, void *a) {
    Write write;
    int result;

    if (im__start_eval(im)) {
        return (-1);
    }

    write_init(&write, im, write_fn, a);

    result = 0;
    if (!write.buf || !write.buf_back || wbuffer_position(write.buf, 0, write.nlines) || vips_threadpool_run(im, write_thread_state_new, wbuffer_allocate_fn, wbuffer_work_fn, wbuffer_progress_fn, &write)) {
        result = -1;
    }

    im_semaphore_down(&write.buf->done);

    im__end_eval(im);

    write_free(&write);

    return (result);
}
