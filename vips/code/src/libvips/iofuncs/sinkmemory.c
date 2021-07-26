
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _Sink {
    VipsImage *im;

    REGION *all;

    int x;
    int y;

    int tile_width;
    int tile_height;
    int nlines;
} Sink;

static void sink_free(Sink *sink) {
    IM_FREEF(im_region_free, sink->all);
}

static int sink_init(Sink *sink, VipsImage *im) {
    Rect all;

    sink->im = im;
    sink->x = 0;
    sink->y = 0;

    all.left = 0;
    all.top = 0;
    all.width = im->Xsize;
    all.height = im->Ysize;

    if (!(sink->all = im_region_create(im)) || im_region_image(sink->all, &all)) {
        sink_free(sink);
        return (-1);
    }

    vips_get_tile_size(im, &sink->tile_width, &sink->tile_height, &sink->nlines);

    return (0);
}

static int sink_allocate(VipsThreadState *state, void *a, gboolean *stop) {
    Sink *sink = (Sink *)a;

    Rect image, tile;

    if (sink->x >= sink->im->Xsize) {
        sink->x = 0;
        sink->y += sink->tile_height;

        if (sink->y >= sink->im->Ysize) {
            *stop = TRUE;

            return (0);
        }
    }

    image.left = 0;
    image.top = 0;
    image.width = sink->im->Xsize;
    image.height = sink->im->Ysize;
    tile.left = sink->x;
    tile.top = sink->y;
    tile.width = sink->tile_width;
    tile.height = sink->tile_height;
    im_rect_intersectrect(&image, &tile, &state->pos);

    sink->x += sink->tile_width;

    return (0);
}

static int sink_work(VipsThreadState *state, void *a) {
    Sink *sink = (Sink *)a;

    if (im_prepare_to(state->reg, sink->all, &state->pos, state->pos.left, state->pos.top)) {
        return (-1);
    }

    return (0);
}

static int sink_progress(void *a) {
    Sink *sink = (Sink *)a;

    VIPS_DEBUG_MSG("sink_progress: %d x %d\n", sink->tile_width, sink->tile_height);

    if (im__handle_eval(sink->im, sink->tile_width, sink->tile_height)) {
        return (-1);
    }

    return (0);
}

int vips_sink_memory(VipsImage *im) {
    Sink sink;
    int result;

    g_assert(!im_image_sanity(im));

    im->Bbits = im_bits_of_fmt(im->BandFmt);

    if (sink_init(&sink, im)) {
        return (-1);
    }

    if (im__start_eval(im)) {
        sink_free(&sink);
        return (-1);
    }

    result = vips_threadpool_run(im, vips_thread_state_new, sink_allocate, sink_work, sink_progress, &sink);

    im__end_eval(im);

    sink_free(&sink);

    return (result);
}
