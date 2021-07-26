
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_TIFF

#include <vips/vips.h>

int im_vips2tiff(IMAGE *im, const char *filename) {
    im_error("im_vips2tiff", "%s", _("TIFF support disabled"));

    return (-1);
}

#else

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include <tiffio.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_LAYER_BUFFER (1000)

typedef enum pyramid_bits { PYR_TL = 1, PYR_TR = 2, PYR_BL = 4, PYR_BR = 8, PYR_ALL = 15, PYR_NONE = 0 } PyramidBits;

typedef struct pyramid_tile {
    REGION *tile;
    PyramidBits bits;
} PyramidTile;

typedef struct pyramid_layer {

    struct tiff_write *tw;
    int width, height;
    int sub;

    char *lname;
    TIFF *tif;
    PEL *tbuf;
    PyramidTile tiles[IM_MAX_LAYER_BUFFER];

    struct pyramid_layer *below;
    struct pyramid_layer *above;
} PyramidLayer;

typedef struct tiff_write {
    IMAGE *im;
    char *name;
    char *mode;

    REGION *reg;

    char *bname;
    TIFF *tif;

    PyramidLayer *layer;
    PEL *tbuf;
    int tls;

    int compression;
    int jpqual;
    int predictor;
    int tile;
    int tilew, tileh;
    int pyramid;
    int onebit;
    int resunit;
    float xres;
    float yres;
    int embed;
    char *icc_profile;
    int bigtiff;

    GMutex *write_lock;
} TiffWrite;

void im__thandler_error(char *module, char *fmt, va_list ap);
void im__thandler_warning(char *module, char *fmt, va_list ap);

static TIFF *tiff_openout(TiffWrite *tw, const char *name) {
    TIFF *tif;
    const char *mode = tw->bigtiff ? "w8" : "w";

#ifdef DEBUG
    printf("TIFFOpen( \"%s\", \"%s\" )\n", name, mode);
#endif

    if (!(tif = TIFFOpen(name, mode))) {
        im_error("im_vips2tiff", _("unable to open \"%s\" for output"), name);
        return (NULL);
    }

    return (tif);
}

static TIFF *tiff_openin(const char *name) {
    TIFF *tif;

    if (!(tif = TIFFOpen(name, "r"))) {
        im_error("im_vips2tiff", _("unable to open \"%s\" for input"), name);
        return (NULL);
    }

    return (tif);
}

static void LabQ2LabC(PEL *q, PEL *p, int n) {
    int x;

    for (x = 0; x < n; x++) {

        q[0] = p[0];
        q[1] = p[1];
        q[2] = p[2];

        p += 4;
        q += 3;
    }
}

static void eightbit2onebit(PEL *q, PEL *p, int n) {
    int x;
    PEL bits;

    bits = 0;
    for (x = 0; x < n; x++) {
        bits <<= 1;
        if (p[x])
            bits |= 1;

        if ((x & 0x7) == 0x7) {
            *q++ = bits;
            bits = 0;
        }
    }

    if ((x & 0x7) != 0)
        *q++ = bits << (8 - (x & 0x7));
}

static void LabS2Lab16(PEL *q, PEL *p, int n) {
    int x;
    short *p1 = (short *)p;
    unsigned short *q1 = (unsigned short *)q;

    for (x = 0; x < n; x++) {

        q1[0] = (int)p1[0] << 1;
        q1[1] = p1[1];
        q1[2] = p1[2];

        p1 += 3;
        q1 += 3;
    }
}

static void pack2tiff(TiffWrite *tw, REGION *in, PEL *q, Rect *area) {
    int y;

    for (y = area->top; y < IM_RECT_BOTTOM(area); y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(in, area->left, y);

        if (in->im->Coding == IM_CODING_LABQ)
            LabQ2LabC(q, p, area->width);
        else if (tw->onebit)
            eightbit2onebit(q, p, area->width);
        else if (in->im->BandFmt == IM_BANDFMT_SHORT && in->im->Type == IM_TYPE_LABS)
            LabS2Lab16(q, p, area->width);
        else
            memcpy(q, p, area->width * IM_IMAGE_SIZEOF_PEL(in->im));

        q += tw->tls;
    }
}

static int embed_profile_file(TIFF *tif, const char *profile) {
    char *buffer;
    unsigned int length;

    if (!(buffer = im__file_read_name(profile, VIPS_ICC_DIR, &length)))
        return (-1);
    TIFFSetField(tif, TIFFTAG_ICCPROFILE, length, buffer);
    im_free(buffer);

#ifdef DEBUG
    printf("im_vips2tiff: attached profile \"%s\"\n", profile);
#endif

    return (0);
}

static int embed_profile_meta(TIFF *tif, IMAGE *im) {
    void *data;
    size_t data_length;

    if (im_meta_get_blob(im, IM_META_ICC_NAME, &data, &data_length))
        return (-1);
    TIFFSetField(tif, TIFFTAG_ICCPROFILE, data_length, data);

#ifdef DEBUG
    printf("im_vips2tiff: attached profile from meta\n");
#endif

    return (0);
}

static int embed_profile(TiffWrite *tw, TIFF *tif) {
    if (tw->embed && embed_profile_file(tif, tw->icc_profile))
        return (-1);
    if (!tw->embed && im_header_get_typeof(tw->im, IM_META_ICC_NAME) && embed_profile_meta(tif, tw->im))
        return (-1);

    return (0);
}

static int write_tiff_header(TiffWrite *tw, TIFF *tif, int width, int height) {
    uint16 v[1];

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, tw->compression);

    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, tw->resunit);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, IM_CLIP(0.01, tw->xres, 10000));
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, IM_CLIP(0.01, tw->yres, 10000));

    if (tw->compression == COMPRESSION_JPEG)
        TIFFSetField(tif, TIFFTAG_JPEGQUALITY, tw->jpqual);

    if (tw->predictor != -1)
        TIFFSetField(tif, TIFFTAG_PREDICTOR, tw->predictor);

    if (embed_profile(tw, tif))
        return (-1);

    if (tw->im->Coding == IM_CODING_LABQ) {
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CIELAB);
    } else if (tw->onebit) {
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    } else {
        int photometric;

        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, tw->im->Bands);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, im_bits_of_fmt(tw->im->BandFmt));

        switch (tw->im->Bands) {
        case 1:
        case 2:
            photometric = PHOTOMETRIC_MINISBLACK;
            if (tw->im->Bands == 2) {
                v[0] = EXTRASAMPLE_ASSOCALPHA;
                TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, v);
            }
            break;

        case 3:
        case 4:
            if (tw->im->Type == IM_TYPE_LAB || tw->im->Type == IM_TYPE_LABS)
                photometric = PHOTOMETRIC_CIELAB;
            else if (tw->im->Type == IM_TYPE_CMYK) {
                photometric = PHOTOMETRIC_SEPARATED;
                TIFFSetField(tif, TIFFTAG_INKSET, INKSET_CMYK);
            } else
                photometric = PHOTOMETRIC_RGB;

            if (tw->im->Type != IM_TYPE_CMYK && tw->im->Bands == 4) {
                v[0] = EXTRASAMPLE_ASSOCALPHA;
                TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, v);
            }
            break;

        case 5:
            if (tw->im->Type == IM_TYPE_CMYK) {
                photometric = PHOTOMETRIC_SEPARATED;
                TIFFSetField(tif, TIFFTAG_INKSET, INKSET_CMYK);
            }
            break;

        default:
            assert(0);
        }

        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    }

    if (tw->tile) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw->tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tw->tileh);
    } else
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 16);

    if (tw->im->BandFmt == IM_BANDFMT_FLOAT)
        TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);

    return (0);
}

static void free_layer(PyramidLayer *layer) {
    int i;

    for (i = 0; i < IM_MAX_LAYER_BUFFER; i++)
        if (layer->tiles[i].tile) {
            im_region_free(layer->tiles[i].tile);
            layer->tiles[i].tile = NULL;
        }

    IM_FREEF(im_free, layer->tbuf);
    IM_FREEF(TIFFClose, layer->tif);
}

static void free_pyramid(PyramidLayer *layer) {
    if (layer->below)
        free_pyramid(layer->below);

    free_layer(layer);
}

static char *new_tiff_name(TiffWrite *tw, char *name, int sub) {
    char buf[FILENAME_MAX];
    char buf2[FILENAME_MAX];

    strcpy(buf, name);
    if (im_ispostfix(buf, ".tif"))
        buf[strlen(buf) - 4] = '\0';
    if (im_ispostfix(buf, ".tiff"))
        buf[strlen(buf) - 5] = '\0';

    im_snprintf(buf2, FILENAME_MAX, "%s.%d.tif", buf, sub);

    return (im_strdup(tw->im, buf2));
}

static int build_pyramid(TiffWrite *tw, PyramidLayer *above, PyramidLayer **zap, int w, int h) {
    PyramidLayer *layer = IM_NEW(tw->im, PyramidLayer);
    int i;

    if (!layer)
        return (-1);
    layer->tw = tw;
    layer->width = w / 2;
    layer->height = h / 2;

    if (!above)

        layer->sub = 2;
    else
        layer->sub = above->sub * 2;

    layer->lname = NULL;
    layer->tif = NULL;
    layer->tbuf = NULL;

    for (i = 0; i < IM_MAX_LAYER_BUFFER; i++) {
        layer->tiles[i].tile = NULL;
        layer->tiles[i].bits = PYR_NONE;
    }

    layer->below = NULL;
    layer->above = above;

    *zap = layer;

    if (layer->width > tw->tilew || layer->height > tw->tileh)
        if (build_pyramid(tw, layer, &layer->below, layer->width, layer->height))
            return (-1);

    if (!(layer->lname = new_tiff_name(tw, tw->name, layer->sub)))
        return (-1);

    if (!(layer->tif = tiff_openout(tw, layer->lname)))
        return (-1);

    if (write_tiff_header(tw, layer->tif, layer->width, layer->height))
        return (-1);

    if (!(layer->tbuf = im_malloc(NULL, TIFFTileSize(layer->tif))))
        return (-1);

    return (0);
}

static int find_new_tile(PyramidLayer *layer) {
    int i;

    for (i = 0; i < IM_MAX_LAYER_BUFFER; i++)
        if (layer->tiles[i].bits == PYR_ALL)
            return (i);

    for (i = 0; i < IM_MAX_LAYER_BUFFER; i++)
        if (!layer->tiles[i].tile) {
            if (!(layer->tiles[i].tile = im_region_create(layer->tw->im)))
                return (-1);
            im__region_no_ownership(layer->tiles[i].tile);
            return (i);
        }

    im_error("im_vips2tiff", "%s",
        _("layer buffer exhausted -- "
          "try making TIFF output tiles smaller"));

    return (-1);
}

static int find_tile(PyramidLayer *layer, Rect *pos) {
    int i;
    Rect quad;
    Rect image;
    Rect inter;

    for (i = 0; i < IM_MAX_LAYER_BUFFER; i++) {
        REGION *reg = layer->tiles[i].tile;

        if (reg && reg->valid.left == pos->left && reg->valid.top == pos->top)
            return (i);
    }

    if ((i = find_new_tile(layer)) < 0)
        return (-1);
    if (im_region_buffer(layer->tiles[i].tile, pos))
        return (-1);
    layer->tiles[i].bits = PYR_NONE;

    quad.width = layer->tw->tilew / 2;
    quad.height = layer->tw->tileh / 2;
    image.left = 0;
    image.top = 0;
    image.width = layer->width;
    image.height = layer->height;

    quad.left = pos->left;
    quad.top = pos->top;
    im_rect_intersectrect(&quad, &image, &inter);
    if (im_rect_isempty(&inter))
        layer->tiles[i].bits |= PYR_TL;

    quad.left = pos->left + quad.width;
    quad.top = pos->top;
    im_rect_intersectrect(&quad, &image, &inter);
    if (im_rect_isempty(&inter))
        layer->tiles[i].bits |= PYR_TR;

    quad.left = pos->left;
    quad.top = pos->top + quad.height;
    im_rect_intersectrect(&quad, &image, &inter);
    if (im_rect_isempty(&inter))
        layer->tiles[i].bits |= PYR_BL;

    quad.left = pos->left + quad.width;
    quad.top = pos->top + quad.height;
    im_rect_intersectrect(&quad, &image, &inter);
    if (im_rect_isempty(&inter))
        layer->tiles[i].bits |= PYR_BR;

    return (i);
}

static void shrink_region_labpack(REGION *from, Rect *area, REGION *to, int xoff, int yoff) {
    int ls = IM_REGION_LSKIP(from);
    Rect *t = &to->valid;

    int x, y;
    Rect out;

    out.left = t->left + xoff;
    out.top = t->top + yoff;
    out.width = area->width / 2;
    out.height = area->height / 2;

    for (y = 0; y < out.height; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(from, area->left, area->top + y * 2);
        PEL *q = (PEL *)IM_REGION_ADDR(to, out.left, out.top + y);

        for (x = 0; x < out.width; x++) {
            signed char *sp = (signed char *)p;
            unsigned char *up = (unsigned char *)p;

            int l = up[0] + up[4] + up[ls] + up[ls + 4];
            int a = sp[1] + sp[5] + sp[ls + 1] + sp[ls + 5];
            int b = sp[2] + sp[6] + sp[ls + 2] + sp[ls + 6];

            q[0] = l >> 2;
            q[1] = a >> 2;
            q[2] = b >> 2;
            q[3] = 0;

            q += 4;
            p += 8;
        }
    }
}

#define SHRINK_TYPE_INT(TYPE)                                    \
    for (x = 0; x < out.width; x++) {                            \
        TYPE *tp = (TYPE *)p;                                    \
        TYPE *tp1 = (TYPE *)(p + ls);                            \
        TYPE *tq = (TYPE *)q;                                    \
                                                                 \
        for (z = 0; z < nb; z++) {                               \
            int tot = tp[z] + tp[z + nb] + tp1[z] + tp1[z + nb]; \
                                                                 \
            tq[z] = tot >> 2;                                    \
        }                                                        \
                                                                 \
        p += ps << 1;                                            \
        q += ps;                                                 \
    }

#define SHRINK_TYPE_FLOAT(TYPE)                                             \
    for (x = 0; x < out.width; x++) {                                       \
        TYPE *tp = (TYPE *)p;                                               \
        TYPE *tp1 = (TYPE *)(p + ls);                                       \
        TYPE *tq = (TYPE *)q;                                               \
                                                                            \
        for (z = 0; z < nb; z++) {                                          \
            double tot = (double)tp[z] + tp[z + nb] + tp1[z] + tp1[z + nb]; \
                                                                            \
            tq[z] = tot / 4;                                                \
        }                                                                   \
                                                                            \
        p += ps << 1;                                                       \
        q += ps;                                                            \
    }

static void shrink_region(REGION *from, Rect *area, REGION *to, int xoff, int yoff) {
    int ls = IM_REGION_LSKIP(from);
    int ps = IM_IMAGE_SIZEOF_PEL(from->im);
    int nb = from->im->Bands;
    Rect *t = &to->valid;

    int x, y, z;
    Rect out;

    out.left = t->left + xoff;
    out.top = t->top + yoff;
    out.width = area->width / 2;
    out.height = area->height / 2;

    for (y = 0; y < out.height; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(from, area->left, area->top + y * 2);
        PEL *q = (PEL *)IM_REGION_ADDR(to, out.left, out.top + y);

        switch (from->im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            SHRINK_TYPE_INT(unsigned char);
            break;
        case IM_BANDFMT_CHAR:
            SHRINK_TYPE_INT(signed char);
            break;
        case IM_BANDFMT_USHORT:
            SHRINK_TYPE_INT(unsigned short);
            break;
        case IM_BANDFMT_SHORT:
            SHRINK_TYPE_INT(signed short);
            break;
        case IM_BANDFMT_UINT:
            SHRINK_TYPE_INT(unsigned int);
            break;
        case IM_BANDFMT_INT:
            SHRINK_TYPE_INT(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            SHRINK_TYPE_FLOAT(float);
            break;
        case IM_BANDFMT_DOUBLE:
            SHRINK_TYPE_FLOAT(double);
            break;

        default:
            assert(0);
        }
    }
}

static int save_tile(TiffWrite *tw, TIFF *tif, PEL *tbuf, REGION *reg, Rect *area) {

    pack2tiff(tw, reg, tbuf, area);

#ifdef DEBUG
    printf("Writing %dx%d pixels at position %dx%d to image %s\n", tw->tilew, tw->tileh, area->left, area->top, TIFFFileName(tif));
#endif

    if (TIFFWriteTile(tif, tbuf, area->left, area->top, 0, 0) < 0) {
        im_error("im_vips2tiff", "%s", _("TIFF write tile failed"));
        return (-1);
    }

    return (0);
}

static int new_tile(PyramidLayer *layer, REGION *tile, Rect *area) {
    TiffWrite *tw = layer->tw;
    int xoff, yoff;

    int t, ri, bo;
    Rect out, new;
    PyramidBits bit;

    new.left = area->left / 2;
    new.top = area->top / 2;
    new.width = area->width / 2;
    new.height = area->height / 2;

    if (im_rect_isempty(&new))
        return (0);

    xoff = new.left % layer->tw->tilew;
    yoff = new.top % layer->tw->tileh;

    out.left = new.left - xoff;
    out.top = new.top - yoff;

    ri = IM_MIN(layer->width, out.left + layer->tw->tilew);
    bo = IM_MIN(layer->height, out.top + layer->tw->tileh);
    out.width = ri - out.left;
    out.height = bo - out.top;

    if ((t = find_tile(layer, &out)) < 0)
        return (-1);

    if (tw->im->Coding == IM_CODING_NONE)
        shrink_region(tile, area, layer->tiles[t].tile, xoff, yoff);
    else
        shrink_region_labpack(tile, area, layer->tiles[t].tile, xoff, yoff);

    if (xoff)
        if (yoff)
            bit = PYR_BR;
        else
            bit = PYR_TR;
    else if (yoff)
        bit = PYR_BL;
    else
        bit = PYR_TL;
    if (layer->tiles[t].bits & bit) {
        im_error("im_vips2tiff", "%s", _("internal error #9876345"));
        return (-1);
    }
    layer->tiles[t].bits |= bit;

    if (layer->tiles[t].bits == PYR_ALL) {

        if (save_tile(tw, layer->tif, layer->tbuf, layer->tiles[t].tile, &layer->tiles[t].tile->valid))
            return (-1);

        if (layer->below && new_tile(layer->below, layer->tiles[t].tile, &layer->tiles[t].tile->valid))
            return (-1);
    }

    return (0);
}

static int write_tif_tile(REGION *out, void *seq, void *a, void *b) {
    TiffWrite *tw = (TiffWrite *)a;

    g_mutex_lock(tw->write_lock);

    if (save_tile(tw, tw->tif, tw->tbuf, out, &out->valid)) {
        g_mutex_unlock(tw->write_lock);
        return (-1);
    }

    if (tw->layer && new_tile(tw->layer, out, &out->valid)) {
        g_mutex_unlock(tw->write_lock);
        return (-1);
    }

    g_mutex_unlock(tw->write_lock);

    return (0);
}

static int write_tif_tilewise(TiffWrite *tw) {
    IMAGE *im = tw->im;

    g_assert(!tw->tbuf);
    if (!(tw->tbuf = im_malloc(NULL, TIFFTileSize(tw->tif))))
        return (-1);

    g_assert(!tw->write_lock);
    tw->write_lock = g_mutex_new();

    if (tw->pyramid && (im->Xsize > tw->tilew || im->Ysize > tw->tileh) && build_pyramid(tw, NULL, &tw->layer, im->Xsize, im->Ysize))
        return (-1);

    if (vips_sink_tile(im, tw->tilew, tw->tileh, NULL, write_tif_tile, NULL, tw, NULL))
        return (-1);

    return (0);
}

static int write_tif_block(REGION *region, Rect *area, void *a) {
    TiffWrite *tw = (TiffWrite *)a;
    IMAGE *im = tw->im;

    int y;

    for (y = 0; y < area->height; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(region, 0, area->top + y);

        if (im->Coding == IM_CODING_LABQ) {
            LabQ2LabC(tw->tbuf, p, im->Xsize);
            p = tw->tbuf;
        } else if (im->BandFmt == IM_BANDFMT_SHORT && im->Type == IM_TYPE_LABS) {
            LabS2Lab16(tw->tbuf, p, im->Xsize);
            p = tw->tbuf;
        } else if (tw->onebit) {
            eightbit2onebit(tw->tbuf, p, im->Xsize);
            p = tw->tbuf;
        }

        if (TIFFWriteScanline(tw->tif, p, area->top + y, 0) < 0)
            return (-1);
    }

    return (0);
}

static int write_tif_stripwise(TiffWrite *tw) {
    g_assert(!tw->tbuf);

    if (!(tw->tbuf = im_malloc(NULL, TIFFScanlineSize(tw->tif))))
        return (-1);

    if (vips_sink_disc(tw->im, write_tif_block, tw))
        return (-1);

    return (0);
}

static void delete_files(TiffWrite *tw) {
    PyramidLayer *layer = tw->layer;

    if (tw->bname) {
        unlink(tw->bname);
        tw->bname = NULL;
    }

    for (layer = tw->layer; layer; layer = layer->below)
        if (layer->lname) {
            unlink(layer->lname);
            layer->lname = NULL;
        }
}

static void free_tiff_write(TiffWrite *tw) {
#ifndef DEBUG
    delete_files(tw);
#endif

    IM_FREEF(TIFFClose, tw->tif);
    IM_FREEF(im_free, tw->tbuf);
    IM_FREEF(g_mutex_free, tw->write_lock);
    IM_FREEF(free_pyramid, tw->layer);
    IM_FREEF(im_free, tw->icc_profile);
}

#define ROUND_DOWN(N, P) ((N) - ((N) % P))

#define ROUND_UP(N, P) (ROUND_DOWN((N) + (P)-1, (P)))

static TiffWrite *make_tiff_write(IMAGE *im, const char *filename) {
    TiffWrite *tw;
    char *p, *q, *r;
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char buf[FILENAME_MAX];

    if (!(tw = IM_NEW(im, TiffWrite)))
        return (NULL);
    tw->im = im;
    im_filename_split(filename, name, mode);
    tw->name = im_strdup(im, name);
    tw->mode = im_strdup(im, mode);
    tw->bname = NULL;
    tw->tif = NULL;
    tw->layer = NULL;
    tw->tbuf = NULL;
    tw->compression = COMPRESSION_NONE;
    tw->jpqual = 75;
    tw->predictor = -1;
    tw->tile = 0;
    tw->tilew = 128;
    tw->tileh = 128;
    tw->pyramid = 0;
    tw->onebit = 0;
    tw->embed = 0;
    tw->icc_profile = NULL;
    tw->bigtiff = 0;
    tw->write_lock = NULL;

    tw->resunit = RESUNIT_CENTIMETER;
    tw->xres = im->Xres * 10;
    tw->yres = im->Yres * 10;
    if (!im_meta_get_string(im, IM_META_RESOLUTION_UNIT, &p) && strcmp(p, "in") == 0) {
        tw->resunit = RESUNIT_INCH;
        tw->xres *= 2.54;
        tw->yres *= 2.54;
    }

    strcpy(buf, mode);
    p = &buf[0];
    if ((q = im_getnextoption(&p))) {
        if (im_isprefix("none", q))
            tw->compression = COMPRESSION_NONE;
        else if (im_isprefix("packbits", q))
            tw->compression = COMPRESSION_PACKBITS;
        else if (im_isprefix("ccittfax4", q))
            tw->compression = COMPRESSION_CCITTFAX4;
        else if (im_isprefix("lzw", q)) {
            tw->compression = COMPRESSION_LZW;

            if ((r = im_getsuboption(q)))
                if (sscanf(r, "%d", &tw->predictor) != 1) {
                    im_error("im_vips2tiff", "%s",
                        _("bad predictor "
                          "parameter"));
                    return (NULL);
                }
        } else if (im_isprefix("deflate", q)) {
            tw->compression = COMPRESSION_ADOBE_DEFLATE;

            if ((r = im_getsuboption(q)))
                if (sscanf(r, "%d", &tw->predictor) != 1) {
                    im_error("im_vips2tiff", "%s",
                        _("bad predictor "
                          "parameter"));
                    return (NULL);
                }
        } else if (im_isprefix("jpeg", q)) {
            tw->compression = COMPRESSION_JPEG;

            if ((r = im_getsuboption(q)))
                if (sscanf(r, "%d", &tw->jpqual) != 1) {
                    im_error("im_vips2tiff", "%s",
                        _("bad JPEG quality "
                          "parameter"));
                    return (NULL);
                }
        } else {
            im_error("im_vips2tiff",
                _("unknown compression mode "
                  "\"%s\"\nshould be one of \"none\", "
                  "\"packbits\", \"ccittfax4\", \"lzw\", "
                  "\"deflate\" or \"jpeg\""),
                q);
            return (NULL);
        }
    }
    if ((q = im_getnextoption(&p))) {
        if (im_isprefix("tile", q)) {
            tw->tile = 1;

            if ((r = im_getsuboption(q))) {
                if (sscanf(r, "%dx%d", &tw->tilew, &tw->tileh) != 2) {
                    im_error("im_vips2tiff", "%s", _("bad tile sizes"));
                    return (NULL);
                }

                if (tw->tilew < 10 || tw->tileh < 10 || tw->tilew > 1000 || tw->tileh > 1000) {
                    im_error("im_vips2tiff", _("bad tile size %dx%d"), tw->tilew, tw->tileh);
                    return (NULL);
                }

                if ((tw->tilew & 0xf) != 0 || (tw->tileh & 0xf) != 0) {
                    im_error("im_vips2tiff", "%s",
                        _("tile size not a "
                          "multiple of 16"));
                    return (NULL);
                }
            }
        } else if (im_isprefix("strip", q))
            tw->tile = 0;
        else {
            im_error("im_vips2tiff",
                _("unknown layout mode "
                  "\"%s\"\nshould be one of \"tile\" or "
                  "\"strip\""),
                q);
            return (NULL);
        }
    }
    if ((q = im_getnextoption(&p))) {
        if (im_isprefix("pyramid", q))
            tw->pyramid = 1;
        else if (im_isprefix("flat", q))
            tw->pyramid = 0;
        else {
            im_error("im_vips2tiff",
                _("unknown multi-res mode "
                  "\"%s\"\nshould be one of \"flat\" or "
                  "\"pyramid\""),
                q);
            return (NULL);
        }
    }
    if ((q = im_getnextoption(&p))) {
        if (im_isprefix("onebit", q))
            tw->onebit = 1;
        else if (im_isprefix("manybit", q))
            tw->onebit = 0;
        else {
            im_error("im_vips2tiff",
                _("unknown format "
                  "\"%s\"\nshould be one of \"onebit\" or "
                  "\"manybit\""),
                q);
            return (NULL);
        }
    }
    if ((q = im_getnextoption(&p))) {
        if (im_isprefix("res_cm", q)) {
            if (tw->resunit == RESUNIT_INCH) {
                tw->xres /= 2.54;
                tw->yres /= 2.54;
            }
            tw->resunit = RESUNIT_CENTIMETER;
        } else if (im_isprefix("res_inch", q)) {
            if (tw->resunit == RESUNIT_CENTIMETER) {
                tw->xres *= 2.54;
                tw->yres *= 2.54;
            }
            tw->resunit = RESUNIT_INCH;
        } else {
            im_error("im_vips2tiff",
                _("unknown resolution unit "
                  "\"%s\"\nshould be one of \"res_cm\" or "
                  "\"res_inch\""),
                q);
            return (NULL);
        }

        if ((r = im_getsuboption(q))) {
            if (sscanf(r, "%fx%f", &tw->xres, &tw->yres) != 2) {
                if (sscanf(r, "%f", &tw->xres) != 1) {
                    im_error("im_vips2tiff", "%s", _("bad resolution values"));
                    return (NULL);
                }

                tw->yres = tw->xres;
            }
        }
    }
    if ((q = im_getnextoption(&p)) && strcmp(q, "") != 0) {
        tw->embed = 1;
        tw->icc_profile = im_strdup(NULL, q);
    }
    if ((q = im_getnextoption(&p)) && strcmp(q, "8") == 0) {
        tw->bigtiff = 1;
    }
    if ((q = im_getnextoption(&p))) {
        im_error("im_vips2tiff", _("unknown extra options \"%s\""), q);
        return (NULL);
    }
    if (!tw->tile && tw->pyramid) {
        im_warn("im_vips2tiff", "%s",
            _("can't have strip pyramid -- "
              "enabling tiling"));
        tw->tile = 1;
    }

    if (tw->pyramid) {
        if (im->Coding == IM_CODING_NONE && vips_bandfmt_iscomplex(im->BandFmt)) {
            im_error("im_vips2tiff", "%s",
                _("can only pyramid LABQ and "
                  "non-complex images"));
            return (NULL);
        }
    }

    if (tw->onebit) {
        if (im->Coding != IM_CODING_NONE || im->BandFmt != IM_BANDFMT_UCHAR || im->Bands != 1)
            tw->onebit = 0;
    }

    if (tw->onebit && tw->compression == COMPRESSION_JPEG) {
        im_warn("im_vips2tiff", "%s",
            _("can't have 1-bit JPEG -- "
              "disabling JPEG"));
        tw->compression = COMPRESSION_NONE;
    }

    if (im->Coding == IM_CODING_LABQ)
        tw->tls = tw->tilew * 3;
    else if (tw->onebit)
        tw->tls = ROUND_UP(tw->tilew, 8) / 8;
    else
        tw->tls = IM_IMAGE_SIZEOF_PEL(im) * tw->tilew;

    return (tw);
}

#define CopyField(tag, v)          \
    if (TIFFGetField(in, tag, &v)) \
    TIFFSetField(out, tag, v)

static int tiff_copy(TiffWrite *tw, TIFF *out, TIFF *in) {
    uint32 i32;
    uint16 i16;
    float f;
    tdata_t buf;
    ttile_t tile;
    ttile_t n;

    CopyField(TIFFTAG_IMAGEWIDTH, i32);
    CopyField(TIFFTAG_IMAGELENGTH, i32);
    CopyField(TIFFTAG_PLANARCONFIG, i16);
    CopyField(TIFFTAG_ORIENTATION, i16);
    CopyField(TIFFTAG_XRESOLUTION, f);
    CopyField(TIFFTAG_YRESOLUTION, f);
    CopyField(TIFFTAG_RESOLUTIONUNIT, i16);
    CopyField(TIFFTAG_COMPRESSION, i16);
    CopyField(TIFFTAG_SAMPLESPERPIXEL, i16);
    CopyField(TIFFTAG_BITSPERSAMPLE, i16);
    CopyField(TIFFTAG_PHOTOMETRIC, i16);
    CopyField(TIFFTAG_TILEWIDTH, i32);
    CopyField(TIFFTAG_TILELENGTH, i32);
    CopyField(TIFFTAG_ROWSPERSTRIP, i32);

    if (tw->predictor != -1)
        TIFFSetField(out, TIFFTAG_PREDICTOR, tw->predictor);

    if (tw->compression == COMPRESSION_JPEG)
        TIFFSetField(out, TIFFTAG_JPEGQUALITY, tw->jpqual);

    if (embed_profile(tw, out))
        return (-1);

    buf = im_malloc(NULL, TIFFTileSize(in));
    n = TIFFNumberOfTiles(in);
    for (tile = 0; tile < n; tile++) {
        tsize_t len;

        len = TIFFReadEncodedTile(in, tile, buf, (tsize_t)-1);
        if (len < 0 || TIFFWriteEncodedTile(out, tile, buf, len) < 0) {
            im_free(buf);
            return (-1);
        }
    }
    im_free(buf);

    return (0);
}

static int tiff_append(TiffWrite *tw, TIFF *out, const char *name) {
    TIFF *in;

    if (!(in = tiff_openin(name)))
        return (-1);

    if (tiff_copy(tw, out, in)) {
        TIFFClose(in);
        return (-1);
    }
    TIFFClose(in);

    if (!TIFFWriteDirectory(out))
        return (-1);

    return (0);
}

static int gather_pyramid(TiffWrite *tw) {
    PyramidLayer *layer;
    TIFF *out;

#ifdef DEBUG
    printf("Starting pyramid gather ...\n");
#endif

    if (!(out = tiff_openout(tw, tw->name)))
        return (-1);

    if (tiff_append(tw, out, tw->bname)) {
        TIFFClose(out);
        return (-1);
    }

    for (layer = tw->layer; layer; layer = layer->below)
        if (tiff_append(tw, out, layer->lname)) {
            TIFFClose(out);
            return (-1);
        }

    TIFFClose(out);

#ifdef DEBUG
    printf("Pyramid built\n");
#endif

    return (0);
}

int im_vips2tiff(IMAGE *in, const char *filename) {
    TiffWrite *tw;
    int res;

#ifdef DEBUG
    printf("im_tiff2vips: libtiff version is \"%s\"\n", TIFFGetVersion());
#endif

    TIFFSetErrorHandler((TIFFErrorHandler)im__thandler_error);
    TIFFSetWarningHandler((TIFFErrorHandler)im__thandler_warning);

    if (im_pincheck(in) || im_check_coding_known("im_vips2tiff", in))
        return (-1);
    if (in->BandFmt != IM_BANDFMT_UCHAR && !(in->BandFmt == IM_BANDFMT_SHORT && in->Type == IM_TYPE_LABS) && in->BandFmt != IM_BANDFMT_USHORT && in->BandFmt != IM_BANDFMT_FLOAT) {
        im_error("im_vips2tiff", "%s",
            _("unsigned 8-bit int, 16-bit int, "
              "and 32-bit float only"));
        return (-1);
    }
    if (in->Coding == IM_CODING_NONE) {
        if (in->Bands < 1 || in->Bands > 5) {
            im_error("im_vips2tiff", "%s", _("1 to 5 bands only"));
            return (-1);
        }
    }

    if (!(tw = make_tiff_write(in, filename)))
        return (-1);
    if (tw->pyramid) {
        if (!(tw->bname = new_tiff_name(tw, tw->name, 1)) || !(tw->tif = tiff_openout(tw, tw->bname))) {
            free_tiff_write(tw);
            return (-1);
        }
    } else {

        if (!(tw->tif = tiff_openout(tw, tw->name))) {
            free_tiff_write(tw);
            return (-1);
        }
    }

    if (write_tiff_header(tw, tw->tif, in->Xsize, in->Ysize)) {
        free_tiff_write(tw);
        return (-1);
    }

    if (tw->tile)
        res = write_tif_tilewise(tw);
    else
        res = write_tif_stripwise(tw);
    if (res) {
        free_tiff_write(tw);
        return (-1);
    }

    if (tw->layer)
        free_pyramid(tw->layer);
    if (tw->tif) {
        TIFFClose(tw->tif);
        tw->tif = NULL;
    }

    if (tw->pyramid && gather_pyramid(tw)) {
        free_tiff_write(tw);
        return (-1);
    }

    free_tiff_write(tw);

    return (0);
}

#endif
