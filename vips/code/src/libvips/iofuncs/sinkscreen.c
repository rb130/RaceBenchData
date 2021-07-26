
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef HAVE_THREADS
static const int have_threads = 1;
#else
static const int have_threads = 0;
#endif

#ifdef VIPS_DEBUG_AMBER
static int render_num_renders = 0;
#endif

typedef struct {
    struct _Render *render;

    Rect area;
    REGION *region;

    gboolean painted;

    gboolean dirty;

    int ticks;
} Tile;

typedef struct _Render {

    int ref_count;
    GMutex *ref_count_lock;

    VipsImage *in;
    VipsImage *out;
    VipsImage *mask;
    int tile_width;
    int tile_height;
    int max_tiles;
    int priority;
    VipsSinkNotify notify;
    void *a;

    GMutex *lock;

    GSList *all;
    int ntiles;
    int ticks;

    GSList *dirty;

    GHashTable *tiles;
} Render;

typedef struct _RenderThreadState {
    VipsThreadState parent_object;

    Tile *tile;
} RenderThreadState;

typedef struct _RenderThreadStateClass {
    VipsThreadStateClass parent_class;

} RenderThreadStateClass;

G_DEFINE_TYPE(RenderThreadState, render_thread_state, VIPS_TYPE_THREAD_STATE);

static GThread *render_thread = NULL;

static im_semaphore_t render_dirty_sem;

static GMutex *render_dirty_lock = NULL;
static GSList *render_dirty_all = NULL;

static gboolean render_reschedule = FALSE;

static void render_thread_state_class_init(RenderThreadStateClass *class) {
    VipsObjectClass *object_class = VIPS_OBJECT_CLASS(class);

    object_class->nickname = "renderthreadstate";
    object_class->description = _("per-thread state for render");
}

static void render_thread_state_init(RenderThreadState *state) {
    state->tile = NULL;
}

static VipsThreadState *render_thread_state_new(VipsImage *im, void *a) {
    return (VIPS_THREAD_STATE(vips_object_new(render_thread_state_get_type(), vips_thread_state_set, im, a)));
}

static void *tile_free(Tile *tile) {
    VIPS_DEBUG_MSG_AMBER("tile_free\n");

    IM_FREEF(im_region_free, tile->region);
    im_free(tile);

    return (NULL);
}

static int render_free(Render *render) {
    VIPS_DEBUG_MSG_AMBER("render_free: %p\n", render);

    g_assert(render->ref_count == 0);

    g_mutex_lock(render_dirty_lock);
    if (g_slist_find(render_dirty_all, render)) {
        render_dirty_all = g_slist_remove(render_dirty_all, render);
    }
    g_mutex_unlock(render_dirty_lock);

    g_mutex_free(render->ref_count_lock);
    g_mutex_free(render->lock);

    im_slist_map2(render->all, (VSListMap2Fn)tile_free, NULL, NULL);
    IM_FREEF(g_slist_free, render->all);
    render->ntiles = 0;
    IM_FREEF(g_slist_free, render->dirty);
    IM_FREEF(g_hash_table_destroy, render->tiles);

    im_free(render);

#ifdef VIPS_DEBUG_AMBER
    render_num_renders -= 1;
#endif

    return (0);
}

static int render_ref(Render *render) {
    g_mutex_lock(render->ref_count_lock);
    g_assert(render->ref_count != 0);
    render->ref_count += 1;
    g_mutex_unlock(render->ref_count_lock);

    return (0);
}

static int render_unref(Render *render) {
    int kill;

    g_mutex_lock(render->ref_count_lock);
    g_assert(render->ref_count > 0);
    render->ref_count -= 1;
    kill = render->ref_count == 0;
    g_mutex_unlock(render->ref_count_lock);

    if (kill) {
        render_free(render);
    }

    return (0);
}

static Render *render_dirty_get(void) {
    Render *render;

    im_semaphore_down(&render_dirty_sem);

    g_mutex_lock(render_dirty_lock);

    render = NULL;
    if (render_dirty_all) {
        render = (Render *)render_dirty_all->data;

        render_ref(render);

        render_dirty_all = g_slist_remove(render_dirty_all, render);
    }

    g_mutex_unlock(render_dirty_lock);

    return (render);
}

static Tile *render_tile_dirty_get(Render *render) {
    Tile *tile;

    if (!render->dirty) {
        tile = NULL;
    } else {
        tile = (Tile *)render->dirty->data;
        g_assert(tile->dirty);
        render->dirty = g_slist_remove(render->dirty, tile);
        tile->dirty = FALSE;
    }

    return (tile);
}

static Tile *render_tile_dirty_reuse(Render *render) {
    Tile *tile;

    if (!render->dirty) {
        tile = NULL;
    } else {
        tile = (Tile *)g_slist_last(render->dirty)->data;
        render->dirty = g_slist_remove(render->dirty, tile);
        g_assert(tile->dirty);
        tile->dirty = FALSE;

        VIPS_DEBUG_MSG("render_tile_get_dirty_reuse: "
                       "reusing dirty %p\n",
            tile);
    }

    return (tile);
}

static void tile_dirty_set(Tile *tile) {
    Render *render = tile->render;

    if (!tile->dirty) {
        g_assert(!g_slist_find(render->dirty, tile));
        render->dirty = g_slist_prepend(render->dirty, tile);
        tile->dirty = TRUE;
        tile->painted = FALSE;
    } else {
        g_assert(g_slist_find(render->dirty, tile));
    }
}

static void tile_dirty_bump(Tile *tile) {
    Render *render = tile->render;

    if (tile->dirty) {
        g_assert(g_slist_find(render->dirty, tile));

        render->dirty = g_slist_remove(render->dirty, tile);
        render->dirty = g_slist_prepend(render->dirty, tile);
    } else {
        g_assert(!g_slist_find(render->dirty, tile));
    }
}

static int render_allocate(VipsThreadState *state, void *a, gboolean *stop) {
    Render *render = (Render *)a;
    RenderThreadState *rstate = (RenderThreadState *)state;
    Tile *tile;

    g_mutex_lock(render->lock);

    if (render_reschedule || !(tile = render_tile_dirty_get(render))) {
        VIPS_DEBUG_MSG_GREEN("render_allocate: stopping\n");
        *stop = TRUE;
        rstate->tile = NULL;
    } else {
        rstate->tile = tile;
    }

    g_mutex_unlock(render->lock);

    return (0);
}

static int render_work(VipsThreadState *state, void *a) {
    Render *render = (Render *)a;
    RenderThreadState *rstate = (RenderThreadState *)state;
    Tile *tile = rstate->tile;

    g_assert(tile);

    VIPS_DEBUG_MSG("calculating tile %p %dx%d\n", tile, tile->area.left, tile->area.top);

    if (im_prepare_to(state->reg, tile->region, &tile->area, tile->area.left, tile->area.top)) {
        VIPS_DEBUG_MSG_RED("render_work: im_prepare_to() failed\n");
        return (-1);
    }
    tile->painted = TRUE;

    if (render->notify) {
        render->notify(render->out, &tile->area, render->a);
    }

    return (0);
}

static int render_dirty_sort(Render *a, Render *b) {
    return (b->priority - a->priority);
}

static void render_dirty_put(Render *render) {
    g_mutex_lock(render_dirty_lock);

    if (render->dirty) {
        if (!g_slist_find(render_dirty_all, render)) {
            render_dirty_all = g_slist_prepend(render_dirty_all, render);
            render_dirty_all = g_slist_sort(render_dirty_all, (GCompareFunc)render_dirty_sort);

            VIPS_DEBUG_MSG_GREEN("render_dirty_put: "
                                 "reschedule\n");
            render_reschedule = TRUE;

            im_semaphore_up(&render_dirty_sem);
        }
    }

    g_mutex_unlock(render_dirty_lock);
}

static void *render_thread_main(void *client) {
    for (;;) {
        Render *render;

        if ((render = render_dirty_get())) {

            VIPS_DEBUG_MSG_GREEN("render_thread_main: "
                                 "threadpool start\n");

            render_reschedule = FALSE;
            if (vips_threadpool_run(render->in, render_thread_state_new, render_allocate, render_work, NULL, render)) {
                VIPS_DEBUG_MSG_RED("render_thread_main: "
                                   "threadpool_run failed\n");
            }

            VIPS_DEBUG_MSG_GREEN("render_thread_main: "
                                 "threadpool return\n");

            render_dirty_put(render);

            render_unref(render);
        }
    }

    return (NULL);
}

static int render_thread_create(void) {
    if (!have_threads) {
        return (0);
    }

    if (!render_dirty_lock) {
        render_dirty_lock = g_mutex_new();
        im_semaphore_init(&render_dirty_sem, 0, "render_dirty_sem");
    }

    if (!render_thread && have_threads) {
        if (!(render_thread = g_thread_create_full(render_thread_main, NULL, IM__DEFAULT_STACK_SIZE, TRUE, FALSE, G_THREAD_PRIORITY_NORMAL, NULL))) {
            im_error("im_render", "%s", _("unable to create thread"));
            return (-1);
        }
    }

    return (0);
}

static guint tile_hash(gconstpointer key) {
    Rect *rect = (Rect *)key;

    int x = rect->left / rect->width;
    int y = rect->top / rect->height;

    return (x << 16 ^ y);
}

static gboolean tile_equal(gconstpointer a, gconstpointer b) {
    Rect *rect1 = (Rect *)a;
    Rect *rect2 = (Rect *)b;

    return (rect1->left == rect2->left && rect1->top == rect2->top);
}

static int render_close_cb(Render *render) {
    VIPS_DEBUG_MSG_AMBER("render_close_cb\n");

    render_unref(render);

    VIPS_DEBUG_MSG_GREEN("render_close_cb: reschedule\n");
    render_reschedule = TRUE;

    return (0);
}

static Render *render_new(VipsImage *in, VipsImage *out, VipsImage *mask, int tile_width, int tile_height, int max_tiles, int priority, VipsSinkNotify notify, void *a) {
    Render *render;

    if (!(render = IM_NEW(NULL, Render))) {
        return (NULL);
    }

    render->ref_count = 1;
    render->ref_count_lock = g_mutex_new();

    render->in = in;
    render->out = out;
    render->mask = mask;
    render->tile_width = tile_width;
    render->tile_height = tile_height;
    render->max_tiles = max_tiles;
    render->priority = priority;
    render->notify = notify;
    render->a = a;

    render->lock = g_mutex_new();

    render->all = NULL;
    render->ntiles = 0;
    render->ticks = 0;

    render->tiles = g_hash_table_new(tile_hash, tile_equal);

    render->dirty = NULL;

    if (im_add_close_callback(out, (im_callback_fn)render_close_cb, render, NULL)) {
        (void)render_unref(render);
        return (NULL);
    }

    if (mask) {
        if (im_add_close_callback(mask, (im_callback_fn)render_close_cb, render, NULL)) {
            (void)render_unref(render);
            return (NULL);
        }
        render_ref(render);
    }

    VIPS_DEBUG_MSG_AMBER("render_new: %p\n", render);

#ifdef VIPS_DEBUG_AMBER
    render_num_renders += 1;
#endif

    return (render);
}

static Tile *tile_new(Render *render) {
    Tile *tile;

    VIPS_DEBUG_MSG_AMBER("tile_new\n");

    if (!(tile = IM_NEW(NULL, Tile))) {
        return (NULL);
    }

    tile->render = render;
    tile->area.left = 0;
    tile->area.top = 0;
    tile->area.width = render->tile_width;
    tile->area.height = render->tile_height;
    tile->region = NULL;
    tile->painted = FALSE;
    tile->dirty = FALSE;
    tile->ticks = render->ticks;

    if (!(tile->region = im_region_create(render->in))) {
        (void)tile_free(tile);
        return (NULL);
    }

    render->all = g_slist_prepend(render->all, tile);
    render->ntiles += 1;

    return (tile);
}

static Tile *render_tile_lookup(Render *render, Rect *area) {
    return ((Tile *)g_hash_table_lookup(render->tiles, area));
}

static void render_tile_add(Tile *tile, Rect *area) {
    Render *render = tile->render;

    g_assert(!render_tile_lookup(render, area));

    tile->area = *area;
    tile->painted = FALSE;

    if (im_region_buffer(tile->region, &tile->area)) {
        VIPS_DEBUG_MSG_RED("render_tile_add: "
                           "buffer allocate failed\n");
    }

    g_hash_table_insert(render->tiles, &tile->area, tile);
}

static void render_tile_move(Tile *tile, Rect *area) {
    Render *render = tile->render;

    g_assert(render_tile_lookup(render, &tile->area));

    if (tile->area.left != area->left || tile->area.top != area->top) {
        g_assert(!render_tile_lookup(render, area));

        g_hash_table_remove(render->tiles, &tile->area);
        render_tile_add(tile, area);
    }
}

static void tile_touch(Tile *tile) {
    Render *render = tile->render;

    tile->ticks = render->ticks;
    render->ticks += 1;
    tile_dirty_bump(tile);
}

static void tile_queue(Tile *tile) {
    Render *render = tile->render;

    VIPS_DEBUG_MSG("tile_queue: adding tile %p %dx%d to dirty\n", tile, tile->area.left, tile->area.top);

    tile->painted = FALSE;
    tile_touch(tile);

    if (render->notify && have_threads) {

        tile_dirty_set(tile);
        render_dirty_put(render);
    } else {

        VIPS_DEBUG_MSG("tile_queue: "
                       "painting tile %p %dx%d synchronously\n",
            tile, tile->area.left, tile->area.top);

        if (im_prepare(tile->region, &tile->area)) {
            VIPS_DEBUG_MSG_RED("tile_queue: prepare failed\n");
        }

        tile->painted = TRUE;
    }
}

static void tile_test_clean_ticks(Rect *key, Tile *value, Tile **best) {
    if (value->painted) {
        if (!*best || value->ticks < (*best)->ticks) {
            *best = value;
        }
    }
}

static Tile *render_tile_get_painted(Render *render) {
    Tile *tile;

    tile = NULL;
    g_hash_table_foreach(render->tiles, (GHFunc)tile_test_clean_ticks, &tile);

    if (tile) {
        VIPS_DEBUG_MSG("render_tile_get_painted: "
                       "reusing painted %p\n",
            tile);
    }

    return (tile);
}

static Tile *render_tile_request(Render *render, Rect *area) {
    Tile *tile;

    VIPS_DEBUG_MSG("render_tile_request: asking for %dx%d\n", area->left, area->top);

    if ((tile = render_tile_lookup(render, area))) {

        if (tile->region->invalid) {
            tile_queue(tile);
        } else {
            tile_touch(tile);
        }
    } else if (render->ntiles < render->max_tiles || render->max_tiles == -1) {

        if (!(tile = tile_new(render))) {
            return (NULL);
        }

        render_tile_add(tile, area);

        tile_queue(tile);
    } else {

        if (!(tile = render_tile_get_painted(render)) && !(tile = render_tile_dirty_reuse(render))) {
            VIPS_DEBUG_MSG("render_tile_request: "
                           "no tiles to reuse\n");
            return (NULL);
        }

        render_tile_move(tile, area);

        tile_queue(tile);
    }

    return (tile);
}

static void tile_copy(Tile *tile, REGION *to) {
    Rect ovlap;
    int y;

    im_rect_intersectrect(&tile->area, &to->valid, &ovlap);
    g_assert(!im_rect_isempty(&ovlap));

    if (tile->painted && !tile->region->invalid) {
        int len = IM_IMAGE_SIZEOF_PEL(to->im) * ovlap.width;

        VIPS_DEBUG_MSG("tile_copy: "
                       "copying calculated pixels for %p %dx%d\n",
            tile, tile->area.left, tile->area.top);

        for (y = ovlap.top; y < IM_RECT_BOTTOM(&ovlap); y++) {
            PEL *p = (PEL *)IM_REGION_ADDR(tile->region, ovlap.left, y);
            PEL *q = (PEL *)IM_REGION_ADDR(to, ovlap.left, y);

            memcpy(q, p, len);
        }
    } else {
        VIPS_DEBUG_MSG("tile_copy: zero filling for %p %dx%d\n", tile, tile->area.left, tile->area.top);
        im_region_paint(to, &ovlap, 0);
    }
}

static int region_fill(REGION *out, void *seq, void *a, void *b) {
    Render *render = (Render *)a;
    Rect *r = &out->valid;
    int x, y;

    int xs = (r->left / render->tile_width) * render->tile_width;
    int ys = (r->top / render->tile_height) * render->tile_height;

    VIPS_DEBUG_MSG("region_fill: left = %d, top = %d, "
                   "width = %d, height = %d\n",
        r->left, r->top, r->width, r->height);

    g_mutex_lock(render->lock);

    for (y = ys; y < IM_RECT_BOTTOM(r); y += render->tile_height) {
        for (x = xs; x < IM_RECT_RIGHT(r); x += render->tile_width) {
            Rect area;
            Tile *tile;

            area.left = x;
            area.top = y;
            area.width = render->tile_width;
            area.height = render->tile_height;

            tile = render_tile_request(render, &area);
            if (tile) {
                tile_copy(tile, out);
            } else {
                VIPS_DEBUG_MSG_RED("region_fill: argh!\n");
            }
        }
    }

    g_mutex_unlock(render->lock);

    return (0);
}

static int mask_fill(REGION *out, void *seq, void *a, void *b) {
#ifdef HAVE_THREADS
    Render *render = (Render *)a;
    Rect *r = &out->valid;
    int x, y;

    int xs = (r->left / render->tile_width) * render->tile_width;
    int ys = (r->top / render->tile_height) * render->tile_height;

    VIPS_DEBUG_MSG("mask_fill: left = %d, top = %d, "
                   "width = %d, height = %d\n",
        r->left, r->top, r->width, r->height);

    g_mutex_lock(render->lock);

    for (y = ys; y < IM_RECT_BOTTOM(r); y += render->tile_height) {
        for (x = xs; x < IM_RECT_RIGHT(r); x += render->tile_width) {
            Rect area;
            Tile *tile;
            int value;

            area.left = x;
            area.top = y;
            area.width = render->tile_width;
            area.height = render->tile_height;

            tile = render_tile_lookup(render, &area);
            value = (tile && tile->painted && !tile->region->invalid) ? 255 : 0;

            im_region_paint(out, &area, value);
        }
    }

    g_mutex_unlock(render->lock);
#else
    im_region_paint(out, &out->valid, 255);
#endif

    return (0);
}

int vips_sink_screen(VipsImage *in, VipsImage *out, VipsImage *mask, int tile_width, int tile_height, int max_tiles, int priority, VipsSinkNotify notify, void *a) {
    Render *render;

    if (render_thread_create()) {
        return (-1);
    }

    if (tile_width <= 0 || tile_height <= 0 || max_tiles < -1) {
        im_error("vips_sink_screen", "%s", _("bad parameters"));
        return (-1);
    }

    if (im_piocheck(in, out) || im_cp_desc(out, in) || im_demand_hint(out, IM_SMALLTILE, in, NULL)) {
        return (-1);
    }
    if (mask) {
        if (im_poutcheck(mask) || im_cp_desc(mask, in) || im_demand_hint(mask, IM_SMALLTILE, in, NULL)) {
            return (-1);
        }

        mask->Bands = 1;
        mask->BandFmt = IM_BANDFMT_UCHAR;
        mask->Type = IM_TYPE_B_W;
        mask->Coding = IM_CODING_NONE;
    }

    if (!(render = render_new(in, out, mask, tile_width, tile_height, max_tiles, priority, notify, a))) {
        return (-1);
    }

    VIPS_DEBUG_MSG("vips_sink_screen: max = %d, %p\n", max_tiles, render);

    if (im_generate(out, NULL, region_fill, NULL, render, NULL)) {
        return (-1);
    }
    if (mask && im_generate(mask, NULL, mask_fill, NULL, render, NULL)) {
        return (-1);
    }

    return (0);
}

void im__print_renders(void) {
#ifdef VIPS_DEBUG_AMBER
    printf("%d active renders\n", render_num_renders);
#endif
    printf("%d dirty renders\n", g_slist_length(render_dirty_all));
}
