
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_OPENEXR

#include <vips/vips.h>

int im_exr2vips(const char *name, IMAGE *out) {
    im_error("im_exr2vips", "%s", _("OpenEXR support disabled"));
    return (-1);
}

#else

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/internal.h>

#include <ImfCRgbaFile.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    char *name;
    IMAGE *out;

    ImfTiledInputFile *tiles;
    ImfInputFile *lines;
    const ImfHeader *header;
    Rect window;
    int tile_width;
    int tile_height;

    GMutex *lock;
} Read;

static void get_imf_error(void) {
    im_error("im_exr2vips", _("EXR error: %s"), ImfErrorMessage());
}

static void read_destroy(Read *read) {
    IM_FREE(read->name);

    IM_FREEF(ImfCloseTiledInputFile, read->tiles);
    IM_FREEF(ImfCloseInputFile, read->lines);

    IM_FREEF(g_mutex_free, read->lock);

    im_free(read);
}

static Read *read_new(const char *name, IMAGE *out) {
    Read *read;
    int xmin, ymin;
    int xmax, ymax;

    if (!(read = IM_NEW(NULL, Read)))
        return (NULL);

    read->name = im_strdup(NULL, name);
    read->out = out;
    read->tiles = NULL;
    read->lines = NULL;
    read->lock = NULL;

    if (im_add_close_callback(out, (im_callback_fn)read_destroy, read, NULL)) {
        read_destroy(read);
        return (NULL);
    }

    if (!(read->tiles = ImfOpenTiledInputFile(read->name))) {
        if (!(read->lines = ImfOpenInputFile(read->name))) {
            get_imf_error();
            return (NULL);
        }
    }

#ifdef DEBUG
    if (read->tiles)
        printf("im_exr2vips: opening in tiled mode\n");
    else
        printf("im_exr2vips: opening in scanline mode\n");
#endif

    if (read->tiles) {
        read->header = ImfTiledInputHeader(read->tiles);
        read->lock = g_mutex_new();
        read->tile_width = ImfTiledInputTileXSize(read->tiles);
        read->tile_height = ImfTiledInputTileYSize(read->tiles);
    } else
        read->header = ImfInputHeader(read->lines);

    ImfHeaderDataWindow(read->header, &xmin, &ymin, &xmax, &ymax);
    read->window.left = xmin;
    read->window.top = ymin;
    read->window.width = xmax - xmin + 1;
    read->window.height = ymax - ymin + 1;

    return (read);
}

static int read_header(Read *read, IMAGE *out) {

    im_initdesc(out, read->window.width, read->window.height, 4, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_sRGB, 1.0, 1.0, 0, 0);

    return (0);
}

static int exr2vips_header(const char *name, IMAGE *out) {
    Read *read;

    if (!(read = read_new(name, out)) || read_header(read, out))
        return (-1);

    return (0);
}

static int isexrtiled(const char *name) {
    Read *read;
    int tiled;

    if (!(read = read_new(name, NULL)))
        return (-1);
    tiled = read->tiles != NULL;
    read_destroy(read);

    return (tiled);
}

static int fill_region(REGION *out, void *seq, void *a, void *b) {
    ImfRgba *imf_buffer = (ImfRgba *)seq;
    Read *read = (Read *)a;
    Rect *r = &out->valid;

    const int tw = read->tile_width;
    const int th = read->tile_height;

    const int xs = (r->left / tw) * tw;
    const int ys = (r->top / th) * th;

    int x, y, z;
    Rect image;

    image.left = 0;
    image.top = 0;
    image.width = read->out->Xsize;
    image.height = read->out->Ysize;

    for (y = ys; y < IM_RECT_BOTTOM(r); y += th)
        for (x = xs; x < IM_RECT_RIGHT(r); x += tw) {
            Rect tile;
            Rect hit;
            int result;

            if (!ImfTiledInputSetFrameBuffer(read->tiles, imf_buffer - (read->window.left + x) - (read->window.top + y) * tw, 1, tw)) {
                get_imf_error();
                return (-1);
            }

#ifdef DEBUG
            printf("im_exr2vips: requesting tile %d x %d\n", x / tw, y / th);
#endif

            g_mutex_lock(read->lock);
            result = ImfTiledInputReadTile(read->tiles, x / tw, y / th, 0, 0);
            g_mutex_unlock(read->lock);

            if (!result) {
                get_imf_error();
                return (-1);
            }

            tile.left = x;
            tile.top = y;
            tile.width = tw;
            tile.height = th;
            im_rect_intersectrect(&tile, &image, &tile);

            im_rect_intersectrect(&tile, r, &hit);

            for (z = 0; z < hit.height; z++) {
                ImfRgba *p = imf_buffer + (hit.left - tile.left) + (hit.top - tile.top + z) * tw;
                float *q = (float *)IM_REGION_ADDR(out, hit.left, hit.top + z);

                ImfHalfToFloatArray(4 * hit.width, (ImfHalf *)p, q);
            }
        }

    return (0);
}

static void *seq_start(IMAGE *out, void *a, void *b) {
    Read *read = (Read *)a;
    ImfRgba *imf_buffer;

    if (!(imf_buffer = IM_ARRAY(out, read->tile_width * read->tile_height, ImfRgba)))
        return (NULL);

    return (imf_buffer);
}

static int exr2vips_tiles(Read *read, IMAGE *out) {
    if (read_header(read, out) || im_poutcheck(out) || im_demand_hint(out, IM_SMALLTILE, NULL) || im_generate(out, seq_start, fill_region, NULL, read, NULL))
        return (-1);

    return (0);
}

static int exr2vips_lines(Read *read, IMAGE *out) {
    const int left = read->window.left;
    const int top = read->window.top;
    const int width = read->window.width;
    const int height = read->window.height;

    ImfRgba *imf_buffer;
    float *vips_buffer;
    int y;

    if (!(imf_buffer = IM_ARRAY(out, width, ImfRgba)) || !(vips_buffer = IM_ARRAY(out, 4 * width, float)) || read_header(read, out) || im_outcheck(out) || im_setupout(out))
        return (-1);

    for (y = 0; y < height; y++) {
        if (!ImfInputSetFrameBuffer(read->lines, imf_buffer - left - (top + y) * width, 1, width)) {
            get_imf_error();
            return (-1);
        }
        if (!ImfInputReadPixels(read->lines, top + y, top + y)) {
            get_imf_error();
            return (-1);
        }

        ImfHalfToFloatArray(4 * width, (ImfHalf *)imf_buffer, vips_buffer);

        if (im_writeline(y, out, (PEL *)vips_buffer))
            return (-1);
    }

    return (0);
}

static int exr2vips(Read *read) {
    if (read->tiles) {
        IMAGE *raw;

        if (!(raw = im_open_local(read->out, "cache", "p")))
            return (-1);
        if (exr2vips_tiles(read, raw))
            return (-1);
        if (im_tile_cache(raw, read->out, read->tile_width, read->tile_height, 2 * (1 + raw->Xsize / read->tile_width)))
            return (-1);
    } else {
        if (exr2vips_lines(read, read->out))
            return (-1);
    }

    return (0);
}

int im_exr2vips(const char *filename, IMAGE *out) {
    Read *read;

#ifdef DEBUG
    printf("im_exr2vips: reading \"%s\"\n", filename);
#endif

    if (!(read = read_new(filename, out)) || exr2vips(read))
        return (-1);

    return (0);
}

static int isexr(const char *filename) {
    unsigned char buf[4];

    if (im__get_bytes(filename, buf, 4))
        if (buf[0] == 0x76 && buf[1] == 0x2f && buf[2] == 0x31 && buf[3] == 0x01)
            return (1);

    return (0);
}

static const char *exr_suffs[] = {".exr", NULL};

static VipsFormatFlags exr_flags(const char *filename) {
    VipsFormatFlags flags;

    flags = 0;
    if (isexrtiled(filename))
        flags |= VIPS_FORMAT_PARTIAL;

    return (flags);
}

typedef VipsFormat VipsFormatExr;
typedef VipsFormatClass VipsFormatExrClass;

static void vips_format_exr_class_init(VipsFormatExrClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "exr";
    object_class->description = _("OpenEXR");

    format_class->is_a = isexr;
    format_class->header = exr2vips_header;
    format_class->load = im_exr2vips;
    format_class->get_flags = exr_flags;
    format_class->suffs = exr_suffs;
}

static void vips_format_exr_init(VipsFormatExr *object) {
}

G_DEFINE_TYPE(VipsFormatExr, vips_format_exr, VIPS_TYPE_FORMAT);

#endif
