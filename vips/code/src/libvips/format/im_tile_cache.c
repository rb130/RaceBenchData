
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/thread.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_TILE_CACHE (250)
#define IM_MIN_TILE_CACHE (5)

typedef struct {
    struct _Read *read;

    REGION *region;
    int time;
    int x;
    int y;
} Tile;

typedef struct _Read {

    IMAGE *in;
    IMAGE *out;
    int tile_width;
    int tile_height;
    int max_tiles;

    int time;
    int ntiles;
    GMutex *lock;
    GSList *cache;
} Read;

static void tile_destroy(Tile *tile) {
    Read *read = tile->read;

    read->cache = g_slist_remove(read->cache, tile);
    read->ntiles -= 1;
    assert(read->ntiles >= 0);
    tile->read = NULL;

    IM_FREEF(im_region_free, tile->region);

    im_free(tile);
}

static void read_destroy(Read *read) {
    IM_FREEF(g_mutex_free, read->lock);

    while (read->cache) {
        Tile *tile = (Tile *)read->cache->data;

        tile_destroy(tile);
    }

    im_free(read);
}

static Read *read_new(IMAGE *in, IMAGE *out, int tile_width, int tile_height, int max_tiles) {
    Read *read;

    if (!(read = IM_NEW(NULL, Read))) {
        return (NULL);
    }
    read->in = in;
    read->out = out;
    read->tile_width = tile_width;
    read->tile_height = tile_height;
    read->max_tiles = max_tiles;
    read->time = 0;
    read->ntiles = 0;
    read->lock = g_mutex_new();
    read->cache = NULL;

    if (im_add_close_callback(out, (im_callback_fn)read_destroy, read, NULL)) {
        read_destroy(read);
        return (NULL);
    }

    return (read);
}

static Tile *tile_new(Read *read) {
    Tile *tile;

    if (!(tile = IM_NEW(NULL, Tile))) {
        return (NULL);
    }

    tile->read = read;
    tile->region = NULL;
    tile->time = read->time;
    tile->x = -1;
    tile->y = -1;
    read->cache = g_slist_prepend(read->cache, tile);
    assert(read->ntiles >= 0);
    read->ntiles += 1;

    if (!(tile->region = im_region_create(read->in))) {
        tile_destroy(tile);
        return (NULL);
    }

    return (tile);
}

static Tile *tile_search(Read *read, int x, int y) {
    GSList *p;

    for (p = read->cache; p; p = p->next) {
        Tile *tile = (Tile *)p->data;

        if (tile->x == x && tile->y == y) {
            return (tile);
        }
    }

    return (NULL);
}

static void tile_touch(Tile *tile) {
    assert(tile->read->ntiles >= 0);

    tile->time = tile->read->time++;
}

static int tile_fill(Tile *tile, int x, int y) {
    Rect area;

    tile->x = x;
    tile->y = y;

#ifdef DEBUG
    printf("im_tile_cache: filling tile %d x %d\n", tile->x, tile->y);
#endif

    area.left = x;
    area.top = y;
    area.width = tile->read->tile_width;
    area.height = tile->read->tile_height;
    if (im_prepare(tile->region, &area)) {
        return (-1);
    }

    im__region_no_ownership(tile->region);

    tile_touch(tile);

    return (0);
}

static Tile *tile_find(Read *read, int x, int y) {
    Tile *tile;
    int oldest;
    GSList *p;

    if ((tile = tile_search(read, x, y))) {
        tile_touch(tile);

        return (tile);
    }

    if (read->max_tiles == -1 || read->ntiles < read->max_tiles) {
        if (!(tile = tile_new(read)) || tile_fill(tile, x, y)) {
            return (NULL);
        }

        return (tile);
    }

    oldest = read->time;
    tile = NULL;
    for (p = read->cache; p; p = p->next) {
        Tile *t = (Tile *)p->data;

        if (t->time < oldest) {
            oldest = t->time;
            tile = t;
        }
    }

    assert(tile);

    im__region_take_ownership(tile->region);

#ifdef DEBUG
    printf("im_tile_cache: reusing tile %d x %d\n", tile->x, tile->y);
#endif

    if (tile_fill(tile, x, y)) {
        return (NULL);
    }

    return (tile);
}

static void copy_region(REGION *from, REGION *to, Rect *area) {
    int y;

    assert(im_rect_includesrect(&from->valid, area));
    assert(im_rect_includesrect(&to->valid, area));

    for (y = area->top; y < IM_RECT_BOTTOM(area); y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(from, area->left, y);
        PEL *q = (PEL *)IM_REGION_ADDR(to, area->left, y);

        memcpy(q, p, IM_IMAGE_SIZEOF_PEL(from->im) * area->width);
    }
}

static int fill_region(REGION *out, void *seq, void *a, void *b) {
    Read *read = (Read *)a;
    const int tw = read->tile_width;
    const int th = read->tile_height;
    Rect *r = &out->valid;

    int xs = (r->left / tw) * tw;
    int ys = (r->top / th) * th;

    int x, y;

    g_mutex_lock(read->lock);

    for (y = ys; y < IM_RECT_BOTTOM(r); y += th) {
        for (x = xs; x < IM_RECT_RIGHT(r); x += tw) {
            Tile *tile;
            Rect tarea;
            Rect hit;

            if (!(tile = tile_find(read, x, y))) {
                g_mutex_unlock(read->lock);
                return (-1);
            }

            tarea.left = x;
            tarea.top = y;
            tarea.width = tw;
            tarea.height = th;

            im_rect_intersectrect(&tarea, r, &hit);

            copy_region(tile->region, out, &hit);
        }
    }

    g_mutex_unlock(read->lock);

    return (0);
}

int im_tile_cache(IMAGE *in, IMAGE *out, int tile_width, int tile_height, int max_tiles) {
    Read *read;

    if (tile_width <= 0 || tile_height <= 0 || max_tiles < -1) {
        im_error("im_tile_cache", "%s", _("bad parameters"));
        return (-1);
    }

    if (im_piocheck(in, out) || im_cp_desc(out, in) || im_demand_hint(out, IM_SMALLTILE, in, NULL) || !(read = read_new(in, out, tile_width, tile_height, max_tiles)) || im_generate(out, NULL, fill_region, NULL, read, NULL)) {
        return (-1);
    }

    return (0);
}
