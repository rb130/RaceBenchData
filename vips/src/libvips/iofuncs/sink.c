

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

    VipsImage *t;

    int x;
    int y;

    int tile_width;
    int tile_height;
    int nlines;

    im_start_fn start;
    im_generate_fn generate;
    im_stop_fn stop;
    void *a;
    void *b;
} Sink;

typedef struct _SinkThreadState {
    VipsThreadState parent_object;

    void *seq;

    REGION *reg;
} SinkThreadState;

typedef struct _SinkThreadStateClass {
    VipsThreadStateClass parent_class;

} SinkThreadStateClass;

G_DEFINE_TYPE(SinkThreadState, sink_thread_state, VIPS_TYPE_THREAD_STATE);

static int sink_call_stop(Sink *sink, SinkThreadState *state) {
    if (state->seq && sink->stop) {
        VIPS_DEBUG_MSG("sink_call_stop: state = %p\n", state);

        if (sink->stop(state->seq, sink->a, sink->b)) {
            im_error("vips_sink", _("stop function failed for image \"%s\""), sink->im->filename);
            return (-1);
        }

        state->seq = NULL;
    }

    return (0);
}

static void sink_thread_state_dispose(GObject *gobject) {
    SinkThreadState *state = (SinkThreadState *)gobject;
    Sink *sink = (Sink *)((VipsThreadState *)state)->a;

    sink_call_stop(sink, state);
    IM_FREEF(im_region_free, state->reg);

    G_OBJECT_CLASS(sink_thread_state_parent_class)->dispose(gobject);
}

static int sink_call_start(Sink *sink, SinkThreadState *state) {
    if (!state->seq && sink->start) {
        VIPS_DEBUG_MSG("sink_call_start: state = %p\n", state);

        state->seq = sink->start(sink->t, sink->a, sink->b);

        if (!state->seq) {
            im_error("vips_sink", _("start function failed for image \"%s\""), sink->im->filename);
            return (-1);
        }
    }

    return (0);
}

static int sink_thread_state_build(VipsObject *object) {
    SinkThreadState *state = (SinkThreadState *)object;
    Sink *sink = (Sink *)((VipsThreadState *)state)->a;

    if (!(state->reg = im_region_create(sink->t)) || sink_call_start(sink, state)) {
        return (-1);
    }

    return (VIPS_OBJECT_CLASS(sink_thread_state_parent_class)->build(object));
}

static void sink_thread_state_class_init(SinkThreadStateClass *class) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(class);
    VipsObjectClass *object_class = VIPS_OBJECT_CLASS(class);

    gobject_class->dispose = sink_thread_state_dispose;

    object_class->build = sink_thread_state_build;
    object_class->nickname = "sinkthreadstate";
    object_class->description = _("per-thread state for sink");
}

static void sink_thread_state_init(SinkThreadState *state) {
    state->seq = NULL;
    state->reg = NULL;
}

static VipsThreadState *sink_thread_state_new(VipsImage *im, void *a) {
    return (VIPS_THREAD_STATE(vips_object_new(sink_thread_state_get_type(), vips_thread_state_set, im, a)));
}

static void sink_free(Sink *sink) {
    IM_FREEF(im_close, sink->t);
}

static int sink_init(Sink *sink, VipsImage *im, im_start_fn start, im_generate_fn generate, im_stop_fn stop, void *a, void *b) {
    sink->im = im;
    sink->t = NULL;
    sink->x = 0;
    sink->y = 0;
    sink->start = start;
    sink->generate = generate;
    sink->stop = stop;
    sink->a = a;
    sink->b = b;

    if (!(sink->t = im_open("iterate", "p")) || im_copy(sink->im, sink->t)) {
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
    SinkThreadState *sstate = (SinkThreadState *)state;
    Sink *sink = (Sink *)a;

    if (im_prepare(sstate->reg, &state->pos) || sink->generate(sstate->reg, sstate->seq, sink->a, sink->b)) {
        return (-1);
    }

    return (0);
}

static int sink_progress(void *a) {
    Sink *sink = (Sink *)a;

    if (im__handle_eval(sink->im, sink->tile_width, sink->tile_height)) {
        return (-1);
    }

    return (0);
}

int vips_sink_tile(VipsImage *im, int tile_width, int tile_height, VipsStart start, VipsGenerate generate, VipsStop stop, void *a, void *b) {
    Sink sink;
    int result;

    g_assert(!im_image_sanity(im));

    im->Bbits = im_bits_of_fmt(im->BandFmt);

    if (sink_init(&sink, im, start, generate, stop, a, b)) {
        return (-1);
    }

    if (tile_width > 0) {
        sink.tile_width = tile_width;
        sink.tile_height = tile_height;
    }

    if (im__start_eval(sink.t)) {
        sink_free(&sink);
        return (-1);
    }

    result = vips_threadpool_run(im, sink_thread_state_new, sink_allocate, sink_work, sink_progress, &sink);

    im__end_eval(sink.t);

    sink_free(&sink);

    return (result);
}

int vips_sink(VipsImage *im, VipsStart start, VipsGenerate generate, VipsStop stop, void *a, void *b) {
    return (vips_sink_tile(im, -1, -1, start, generate, stop, a, b));
}
