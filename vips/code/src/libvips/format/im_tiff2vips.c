
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_TIFF

#include <vips/vips.h>

int im_tiff2vips(const char *tiffile, IMAGE *im) {
    im_error("im_tiff2vips", "%s", _("TIFF support disabled"));
    return (-1);
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>

#include <tiffio.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef void (*scanline_process_fn)(PEL *q, PEL *p, int n, void *user);

typedef struct {

    char *filename;
    IMAGE *out;

    int page;

    TIFF *tiff;

    scanline_process_fn sfn;
    void *client;

    int twidth, theight;

    GMutex *tlock;
} ReadTiff;

typedef struct {

    TIFF *tif;
    IMAGE *im;

    float LumaRed, LumaGreen, LumaBlue;

    void *table;
} YCbCrParams;

void im__thandler_error(char *module, char *fmt, va_list ap) {
    im_verror(module, fmt, ap);
}

void im__thandler_warning(char *module, char *fmt, va_list ap) {
    char buf[256];

    im_vsnprintf(buf, 256, fmt, ap);
    im_warn(module, "%s", buf);
}

static int tfexists(TIFF *tif, ttag_t tag) {
    uint32 a, b;

    if (TIFFGetField(tif, tag, &a, &b))
        return (1);
    else
        return (0);
}

static int tfequals(TIFF *tif, ttag_t tag, uint16 val) {
    uint16 fld;

    if (!TIFFGetFieldDefaulted(tif, tag, &fld)) {
        im_error("im_tiff2vips", _("required field %d missing"), tag);
        return (0);
    }
    if (fld != val) {
        im_error("im_tiff2vips", _("required field %d=%d, not %d"), tag, fld, val);
        return (0);
    }

    return (1);
}

static int tfget32(TIFF *tif, ttag_t tag, int *out) {
    uint32 fld;

    if (!TIFFGetFieldDefaulted(tif, tag, &fld)) {
        im_error("im_tiff2vips", _("required field %d missing"), tag);
        return (0);
    }

    *out = fld;
    return (1);
}

static int tfget16(TIFF *tif, ttag_t tag, int *out) {
    uint16 fld;

    if (!TIFFGetFieldDefaulted(tif, tag, &fld)) {
        im_error("im_tiff2vips", _("required field %d missing"), tag);
        return (0);
    }

    *out = fld;
    return (1);
}

static void labpack_line(PEL *q, PEL *p, int n, void *dummy) {
    int x;

    for (x = 0; x < n; x++) {
        q[0] = p[0];
        q[1] = p[1];
        q[2] = p[2];
        q[3] = 0;

        q += 4;
        p += 3;
    }
}

static int parse_labpack(ReadTiff *rtiff, IMAGE *out) {
    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 3) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 8))
        return (-1);

    out->Bands = 4;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_LABQ;
    out->Type = IM_TYPE_LAB;

    rtiff->sfn = labpack_line;
    rtiff->client = NULL;

    return (0);
}

static void labs_line(PEL *q, PEL *p, int n, void *dummy) {
    int x;
    unsigned short *p1 = (unsigned short *)p;
    short *q1 = (short *)q;

    for (x = 0; x < n; x++) {
        q1[0] = p1[0] >> 1;
        q1[1] = p1[1];
        q1[2] = p1[2];

        q1 += 3;
        p1 += 3;
    }
}

static int parse_labs(ReadTiff *rtiff, IMAGE *out) {
    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 3) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 16))
        return (-1);

    out->Bands = 3;
    out->BandFmt = IM_BANDFMT_SHORT;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_LABS;

    rtiff->sfn = labs_line;
    rtiff->client = NULL;

    return (0);
}

static void onebit_line(PEL *q, PEL *p, int n, void *flg) {

    int pm = *((int *)flg);
    int x, i, z;
    PEL bits;

    int black = (pm == PHOTOMETRIC_MINISBLACK) ? 0 : 255;
    int white = black ^ -1;

    for (x = 0, i = 0; i < (n >> 3); i++) {
        bits = (PEL)p[i];

        for (z = 0; z < 8; z++, x++) {
            q[x] = (bits & 128) ? white : black;
            bits <<= 1;
        }
    }

    if (n & 7) {
        bits = p[i];
        for (z = 0; z < (n & 7); z++) {
            q[x + z] = (bits & 128) ? white : black;
            bits <<= 1;
        }
    }
}

static int parse_onebit(ReadTiff *rtiff, int pm, IMAGE *out) {
    int *ppm;

    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 1) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 1))
        return (-1);

    out->Bands = 1;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_B_W;

    if (!(ppm = IM_ARRAY(out, 1, int)))
        return (-1);
    *ppm = pm;

    rtiff->sfn = onebit_line;
    rtiff->client = ppm;

    return (0);
}

static void greyscale8_line(PEL *q, PEL *p, int n, void *flg) {

    PEL mask = *((PEL *)flg);
    int x;

    for (x = 0; x < n; x++)
        q[x] = p[x] ^ mask;
}

static int parse_greyscale8(ReadTiff *rtiff, int pm, IMAGE *out) {
    PEL *mask;

    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 1) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 8))
        return (-1);

    if (!(mask = IM_ARRAY(out, 1, PEL)))
        return (-1);
    *mask = (pm == PHOTOMETRIC_MINISBLACK) ? 0 : 255;

    out->Bands = 1;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_B_W;

    rtiff->sfn = greyscale8_line;
    rtiff->client = mask;

    return (0);
}

static void greyscale16_line(PEL *q, PEL *p, int n, void *flg) {

    unsigned short mask = *((unsigned short *)flg);
    unsigned short *p1 = (unsigned short *)p;
    unsigned short *q1 = (unsigned short *)q;
    int x;

    for (x = 0; x < n; x++)
        q1[x] = p1[x] ^ mask;
}

static int parse_greyscale16(ReadTiff *rtiff, int pm, IMAGE *out) {
    unsigned short *mask;

    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 1) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 16))
        return (-1);

    if (!(mask = IM_ARRAY(out, 1, unsigned short)))
        return (-1);
    mask[0] = (pm == PHOTOMETRIC_MINISBLACK) ? 0 : 65535;

    out->Bands = 1;
    out->BandFmt = IM_BANDFMT_USHORT;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_GREY16;

    rtiff->sfn = greyscale16_line;
    rtiff->client = mask;

    return (0);
}

static void greyscale32f_line(PEL *q, PEL *p, int n) {
    float *p1 = (float *)p;
    float *q1 = (float *)q;
    int x;

    for (x = 0; x < n; x++)
        q1[x] = p1[x];
}

static int parse_greyscale32f(ReadTiff *rtiff, int pm, IMAGE *out) {
    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 1) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 32))
        return (-1);

    out->Bands = 1;
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_B_W;

    rtiff->sfn = (scanline_process_fn)greyscale32f_line;
    rtiff->client = NULL;

    return (0);
}

static void palette_line(PEL *q, PEL *p, int n, void *flg) {

    PEL *red = ((PEL **)flg)[0];
    PEL *green = ((PEL **)flg)[1];
    PEL *blue = ((PEL **)flg)[2];
    int x;

    for (x = 0; x < n; x++) {
        int i = *p++;

        q[0] = red[i];
        q[1] = green[i];
        q[2] = blue[i];

        q += 3;
    }
}

static int parse_palette(ReadTiff *rtiff, IMAGE *out) {
    uint16 *tred, *tgreen, *tblue;
    PEL *red, *green, *blue;
    PEL **maps;
    int i;

    if (!tfequals(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, 1) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 8))
        return (-1);

    if (!(red = IM_ARRAY(out, 256, PEL)) || !(green = IM_ARRAY(out, 256, PEL)) || !(blue = IM_ARRAY(out, 256, PEL)) || !(maps = IM_ARRAY(out, 3, PEL *)))
        return (-1);

    if (!TIFFGetField(rtiff->tiff, TIFFTAG_COLORMAP, &tred, &tgreen, &tblue)) {
        im_error("im_tiff2vips", "%s", _("bad colormap"));
        return (-1);
    }
    for (i = 0; i < 256; i++) {
        red[i] = tred[i] >> 8;
        green[i] = tgreen[i] >> 8;
        blue[i] = tblue[i] >> 8;
    }
    maps[0] = red;
    maps[1] = green;
    maps[2] = blue;

    out->Bands = 3;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_sRGB;

    rtiff->sfn = palette_line;
    rtiff->client = maps;

    return (0);
}

static void rgbcmyk8_line(PEL *q, PEL *p, int n, IMAGE *im) {
    int x, b;

    for (x = 0; x < n; x++) {
        for (b = 0; b < im->Bands; b++)
            q[b] = p[b];

        q += im->Bands;
        p += im->Bands;
    }
}

static int parse_rgb8(ReadTiff *rtiff, IMAGE *out) {
    int bands;

    if (!tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 8) || !tfget16(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, &bands))
        return (-1);
    if (bands != 3 && bands != 4) {
        im_error("im_tiff2vips", "%s", _("3 or 4 bands RGB TIFF only"));
        return (-1);
    }

    out->Bands = bands;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_sRGB;

    rtiff->sfn = (scanline_process_fn)rgbcmyk8_line;
    rtiff->client = out;

    return (0);
}

static void rgb16_line(PEL *q, PEL *p, int n, IMAGE *im) {
    int x, b;
    unsigned short *p1 = (unsigned short *)p;
    unsigned short *q1 = (unsigned short *)q;

    for (x = 0; x < n; x++) {
        for (b = 0; b < im->Bands; b++)
            q1[b] = p1[b];

        q1 += im->Bands;
        p1 += im->Bands;
    }
}

static int parse_rgb16(ReadTiff *rtiff, IMAGE *out) {
    int bands;

    if (!tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 16) || !tfget16(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, &bands))
        return (-1);
    if (bands != 3 && bands != 4) {
        im_error("im_tiff2vips", "%s", _("3 or 4 bands RGB TIFF only"));
        return (-1);
    }

    out->Bands = bands;
    out->BandFmt = IM_BANDFMT_USHORT;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_RGB16;

    rtiff->sfn = (scanline_process_fn)rgb16_line;
    rtiff->client = out;

    return (0);
}

static void r32f_line(PEL *q, PEL *p, int n, void *dummy) {
    int x;
    float *p1 = (float *)p;
    float *q1 = (float *)q;

    for (x = 0; x < n; x++) {
        q1[0] = p1[0];
        q1[1] = p1[1];
        q1[2] = p1[2];

        q1 += 3;
        p1 += 3;
    }
}

static int parse_32f(ReadTiff *rtiff, int pm, IMAGE *out) {
    int bands;

    if (!tfget16(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, &bands) || !tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 32))
        return (-1);

    assert(bands == 3 || bands == 4);

    out->Bands = bands;
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Coding = IM_CODING_NONE;

    switch (pm) {
    case PHOTOMETRIC_CIELAB:
        out->Type = IM_TYPE_LAB;
        break;

    case PHOTOMETRIC_RGB:
        out->Type = IM_TYPE_sRGB;
        break;

    default:
        assert(0);
    }

    rtiff->sfn = r32f_line;
    rtiff->client = NULL;

    return (0);
}

static int parse_cmyk(ReadTiff *rtiff, IMAGE *out) {
    int bands;

    if (!tfequals(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, 8) || !tfequals(rtiff->tiff, TIFFTAG_INKSET, INKSET_CMYK) || !tfget16(rtiff->tiff, TIFFTAG_SAMPLESPERPIXEL, &bands))
        return (-1);
    if (bands != 4 && bands != 5) {
        im_error("im_tiff2vips", "%s", _("4 or 5 bands CMYK TIFF only"));
        return (-1);
    }

    out->Bands = bands;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_CMYK;

    rtiff->sfn = (scanline_process_fn)rgbcmyk8_line;
    rtiff->client = out;

    return (0);
}

static int parse_resolution(TIFF *tiff, IMAGE *out) {
    float x, y;
    int ru;

    if (TIFFGetFieldDefaulted(tiff, TIFFTAG_XRESOLUTION, &x) && TIFFGetFieldDefaulted(tiff, TIFFTAG_YRESOLUTION, &y) && tfget16(tiff, TIFFTAG_RESOLUTIONUNIT, &ru)) {
        switch (ru) {
        case RESUNIT_NONE:
            break;

        case RESUNIT_INCH:

            x /= 10.0 * 2.54;
            y /= 10.0 * 2.54;
            im_meta_set_string(out, IM_META_RESOLUTION_UNIT, "in");
            break;

        case RESUNIT_CENTIMETER:

            x /= 10.0;
            y /= 10.0;
            im_meta_set_string(out, IM_META_RESOLUTION_UNIT, "cm");
            break;

        default:
            im_error("im_tiff2vips", "%s", _("unknown resolution unit"));
            return (-1);
        }
    } else {
        im_warn("im_tiff2vips",
            _("no resolution information for "
              "TIFF image \"%s\" -- defaulting to 1 pixel per mm"),
            TIFFFileName(tiff));
        x = 1.0;
        y = 1.0;
    }

    out->Xres = x;
    out->Yres = y;

    return (0);
}

static int parse_header(ReadTiff *rtiff, IMAGE *out) {
    int pm, bps, format;
    uint32 data_length;
    void *data;

    if (tfexists(rtiff->tiff, TIFFTAG_PLANARCONFIG) && !tfequals(rtiff->tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG))
        return (-1);

    if (!tfget32(rtiff->tiff, TIFFTAG_IMAGEWIDTH, &out->Xsize) || !tfget32(rtiff->tiff, TIFFTAG_IMAGELENGTH, &out->Ysize) || parse_resolution(rtiff->tiff, out))
        return (-1);

    if (!tfget16(rtiff->tiff, TIFFTAG_PHOTOMETRIC, &pm) || !tfget16(rtiff->tiff, TIFFTAG_BITSPERSAMPLE, &bps))
        return (-1);

    switch (pm) {
    case PHOTOMETRIC_CIELAB:
        switch (bps) {
        case 8:
            if (parse_labpack(rtiff, out))
                return (-1);
            break;

        case 16:
            if (parse_labs(rtiff, out))
                return (-1);
            break;

        case 32:
            if (!tfget16(rtiff->tiff, TIFFTAG_SAMPLEFORMAT, &format))
                return (-1);

            if (format == SAMPLEFORMAT_IEEEFP) {
                if (parse_32f(rtiff, pm, out))
                    return (-1);
            } else {
                im_error("im_tiff2vips",
                    _("unsupported sample "
                      "format %d for lab image"),
                    format);
                return (-1);
            }

            break;

        default:
            im_error("im_tiff2vips", _("unsupported depth %d for LAB image"), bps);
            return (-1);
        }

        break;

    case PHOTOMETRIC_MINISWHITE:
    case PHOTOMETRIC_MINISBLACK:
        switch (bps) {
        case 1:
            if (parse_onebit(rtiff, pm, out))
                return (-1);

            break;

        case 8:
            if (parse_greyscale8(rtiff, pm, out))
                return (-1);

            break;

        case 16:
            if (parse_greyscale16(rtiff, pm, out))
                return (-1);

            break;

        case 32:
            if (!tfget16(rtiff->tiff, TIFFTAG_SAMPLEFORMAT, &format))
                return (-1);

            if (format == SAMPLEFORMAT_IEEEFP) {
                if (parse_greyscale32f(rtiff, pm, out))
                    return (-1);
            } else {
                im_error("im_tiff2vips",
                    _("unsupported sample format "
                      "%d for greyscale image"),
                    format);
                return (-1);
            }

            break;

        default:
            im_error("im_tiff2vips",
                _("unsupported depth %d "
                  "for greyscale image"),
                bps);
            return (-1);
        }

        break;

    case PHOTOMETRIC_PALETTE:

        if (parse_palette(rtiff, out))
            return (-1);

        break;

    case PHOTOMETRIC_YCBCR:

        TIFFSetField(rtiff->tiff, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
        if (parse_rgb8(rtiff, out))
            return (-1);
        break;

    case PHOTOMETRIC_RGB:
        switch (bps) {
        case 8:
            if (parse_rgb8(rtiff, out))
                return (-1);
            break;

        case 16:
            if (parse_rgb16(rtiff, out))
                return (-1);
            break;

        case 32:
            if (!tfget16(rtiff->tiff, TIFFTAG_SAMPLEFORMAT, &format))
                return (-1);

            if (format == SAMPLEFORMAT_IEEEFP) {
                if (parse_32f(rtiff, pm, out))
                    return (-1);
            } else {
                im_error("im_tiff2vips",
                    _("unsupported sample "
                      "format %d for rgb image"),
                    format);
                return (-1);
            }

            break;

        default:
            im_error("im_tiff2vips",
                _("unsupported depth %d "
                  "for RGB image"),
                bps);
            return (-1);
        }

        break;

    case PHOTOMETRIC_SEPARATED:
        if (parse_cmyk(rtiff, out))
            return (-1);

        break;

    default:
        im_error("im_tiff2vips",
            _("unknown photometric "
              "interpretation %d"),
            pm);
        return (-1);
    }

    if (TIFFGetField(rtiff->tiff, TIFFTAG_ICCPROFILE, &data_length, &data)) {
        void *data_copy;

        if (!(data_copy = im_malloc(NULL, data_length)))
            return (-1);
        memcpy(data_copy, data, data_length);
        if (im_meta_set_blob(out, IM_META_ICC_NAME, (im_callback_fn)im_free, data_copy, data_length)) {
            im_free(data_copy);
            return (-1);
        }
    }

    return (0);
}

static void *seq_start(IMAGE *out, void *a, void *b) {
    ReadTiff *rtiff = (ReadTiff *)a;
    tdata_t *buf;

    if (!(buf = im_malloc(NULL, TIFFTileSize(rtiff->tiff))))
        return (NULL);

    return ((void *)buf);
}

static int fill_region(REGION *out, void *seq, void *a, void *b) {
    tdata_t *buf = (tdata_t *)seq;
    ReadTiff *rtiff = (ReadTiff *)a;
    Rect *r = &out->valid;

    int xs = (r->left / rtiff->twidth) * rtiff->twidth;
    int ys = (r->top / rtiff->theight) * rtiff->theight;

    int tls = TIFFTileSize(rtiff->tiff) / rtiff->theight;

    int tps = tls / rtiff->twidth;

    int x, y, z;

    for (y = ys; y < IM_RECT_BOTTOM(r); y += rtiff->theight)
        for (x = xs; x < IM_RECT_RIGHT(r); x += rtiff->twidth) {
            Rect tile;
            Rect hit;

            g_mutex_lock(rtiff->tlock);
            if (TIFFReadTile(rtiff->tiff, buf, x, y, 0, 0) < 0) {
                g_mutex_unlock(rtiff->tlock);
                return (-1);
            }
            g_mutex_unlock(rtiff->tlock);

            tile.left = x;
            tile.top = y;
            tile.width = rtiff->twidth;
            tile.height = rtiff->twidth;

            im_rect_intersectrect(&tile, r, &hit);

            for (z = 0; z < hit.height; z++) {
                PEL *p = (PEL *)buf + (hit.left - tile.left) * tps + (hit.top - tile.top + z) * tls;
                PEL *q = (PEL *)IM_REGION_ADDR(out, hit.left, hit.top + z);

                rtiff->sfn(q, p, hit.width, rtiff->client);
            }
        }

    return (0);
}

static int seq_stop(void *seq, void *a, void *b) {
    im_free(seq);

    return (0);
}

static int read_tilewise(ReadTiff *rtiff, IMAGE *out) {
    IMAGE *raw;

    if (!(raw = im_open_local(out, "cache", "p")))
        return (-1);

    if (!tfget32(rtiff->tiff, TIFFTAG_TILEWIDTH, &rtiff->twidth) || !tfget32(rtiff->tiff, TIFFTAG_TILELENGTH, &rtiff->theight))
        return (-1);

    if (im_poutcheck(raw))
        return (-1);

    if (parse_header(rtiff, raw))
        return (-1);

    if (im_demand_hint(raw, IM_SMALLTILE, NULL) || im_generate(raw, seq_start, fill_region, seq_stop, rtiff, NULL))
        return (-1);

    if (im_tile_cache(raw, out, rtiff->twidth, rtiff->theight, 2 * (1 + raw->Xsize / rtiff->twidth)))
        return (-1);

    return (0);
}

static int read_stripwise(ReadTiff *rtiff, IMAGE *out) {
    int rows_per_strip;
    tsize_t scanline_size;
    tsize_t strip_size;
    int number_of_strips;

    PEL *vbuf;
    tdata_t tbuf;
    tstrip_t strip;
    tsize_t length;
    int y;
    int i;
    PEL *p;

    if (parse_header(rtiff, out))
        return (-1);

    if (!tfget32(rtiff->tiff, TIFFTAG_ROWSPERSTRIP, &rows_per_strip))
        return (-1);
    scanline_size = TIFFScanlineSize(rtiff->tiff);
    strip_size = TIFFStripSize(rtiff->tiff);
    number_of_strips = TIFFNumberOfStrips(rtiff->tiff);

#ifdef DEBUG
    printf("read_stripwise: rows_per_strip = %d\n", rows_per_strip);
    printf("read_stripwise: scanline_size = %d\n", scanline_size);
    printf("read_stripwise: strip_size = %d\n", strip_size);
    printf("read_stripwise: number_of_strips = %d\n", number_of_strips);
#endif

    if (im_outcheck(out) || im_setupout(out))
        return (-1);

    if (!(vbuf = IM_ARRAY(out, IM_IMAGE_SIZEOF_LINE(out), PEL)) || !(tbuf = im_malloc(out, strip_size)))
        return (-1);

    for (strip = 0, y = 0; strip < number_of_strips; strip += 1, y += rows_per_strip) {
        length = TIFFReadEncodedStrip(rtiff->tiff, strip, tbuf, (tsize_t)-1);
        if (length == -1) {
            im_error("im_tiff2vips", "%s", _("read error"));
            return (-1);
        }

        for (p = tbuf, i = 0; i < rows_per_strip && y + i < out->Ysize; i += 1, p += scanline_size) {

            rtiff->sfn(vbuf, p, out->Xsize, rtiff->client);
            if (im_writeline(y + i, out, vbuf))
                return (-1);
        }
    }

    return (0);
}

static int readtiff_destroy(ReadTiff *rtiff) {
    IM_FREEF(TIFFClose, rtiff->tiff);
    IM_FREEF(g_mutex_free, rtiff->tlock);

    return (0);
}

static ReadTiff *readtiff_new(const char *filename, IMAGE *out) {
    ReadTiff *rtiff;
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char *p, *q;

    if (!(rtiff = IM_NEW(out, ReadTiff)))
        return (NULL);
    rtiff->filename = NULL;
    rtiff->out = out;
    im_filename_split(filename, name, mode);
    rtiff->filename = im_strdup(out, name);
    rtiff->page = 0;
    rtiff->tiff = NULL;
    rtiff->sfn = NULL;
    rtiff->client = NULL;
    rtiff->twidth = 0;
    rtiff->theight = 0;
    rtiff->tlock = g_mutex_new();

    if (im_add_close_callback(out, (im_callback_fn)readtiff_destroy, rtiff, NULL)) {
        readtiff_destroy(rtiff);
        return (NULL);
    }

    p = &mode[0];
    if ((q = im_getnextoption(&p))) {
        rtiff->page = atoi(q);

        if (rtiff->page < 0 || rtiff->page > 1000) {
            im_error("im_tiff2vips", _("bad page number %d"), rtiff->page);
            return (NULL);
        }
    }

    return (rtiff);
}

static TIFF *get_directory(const char *filename, int page) {
    TIFF *tif;
    int i;

    if (!(tif = TIFFOpen(filename, "rm"))) {
        im_error("im_tiff2vips", _("unable to open \"%s\" for input"), filename);
        return (NULL);
    }

    for (i = 0; i < page; i++)
        if (!TIFFReadDirectory(tif)) {

            TIFFClose(tif);
            return (NULL);
        }

    return (tif);
}

int im_tiff2vips(const char *filename, IMAGE *out) {
    ReadTiff *rtiff;

#ifdef DEBUG
    printf("im_tiff2vips: libtiff version is \"%s\"\n", TIFFGetVersion());
#endif

    TIFFSetErrorHandler((TIFFErrorHandler)im__thandler_error);
    TIFFSetWarningHandler((TIFFErrorHandler)im__thandler_warning);

    if (!(rtiff = readtiff_new(filename, out)))
        return (-1);

    if (!(rtiff->tiff = get_directory(rtiff->filename, rtiff->page))) {
        im_error("im_tiff2vips",
            _("TIFF file does not "
              "contain page %d"),
            rtiff->page);
        return (-1);
    }

    if (TIFFIsTiled(rtiff->tiff)) {
        if (read_tilewise(rtiff, out))
            return (-1);
    } else {
        if (read_stripwise(rtiff, out))
            return (-1);
    }

    return (0);
}

static int tiff2vips_header(const char *filename, IMAGE *out) {
    ReadTiff *rtiff;

    TIFFSetErrorHandler((TIFFErrorHandler)im__thandler_error);
    TIFFSetWarningHandler((TIFFErrorHandler)im__thandler_warning);

    if (!(rtiff = readtiff_new(filename, out)))
        return (-1);

    if (!(rtiff->tiff = get_directory(rtiff->filename, rtiff->page))) {
        im_error("im_tiff2vips", _("TIFF file does not contain page %d"), rtiff->page);
        return (-1);
    }

    if (parse_header(rtiff, out))
        return (-1);

    return (0);
}

static int istiff(const char *filename) {
    unsigned char buf[2];

    if (im__get_bytes(filename, buf, 2))
        if ((buf[0] == 'M' && buf[1] == 'M') || (buf[0] == 'I' && buf[1] == 'I'))
            return (1);

    return (0);
}

static int istifftiled(const char *filename) {
    TIFF *tif;
    int tiled;

    TIFFSetErrorHandler((TIFFErrorHandler)im__thandler_error);
    TIFFSetWarningHandler((TIFFErrorHandler)im__thandler_warning);

    if (!(tif = TIFFOpen(filename, "rm"))) {

        im_error_clear();
        return (0);
    }
    tiled = TIFFIsTiled(tif);
    TIFFClose(tif);

    return (tiled);
}

static VipsFormatFlags tiff_flags(const char *filename) {
    VipsFormatFlags flags;

    flags = 0;
    if (istifftiled(filename))
        flags |= VIPS_FORMAT_PARTIAL;

    return (flags);
}

static const char *tiff_suffs[] = {".tif", ".tiff", NULL};

typedef VipsFormat VipsFormatTiff;
typedef VipsFormatClass VipsFormatTiffClass;

static void vips_format_tiff_class_init(VipsFormatTiffClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "tiff";
    object_class->description = _("TIFF");

    format_class->is_a = istiff;
    format_class->header = tiff2vips_header;
    format_class->load = im_tiff2vips;
    format_class->save = im_vips2tiff;
    format_class->get_flags = tiff_flags;
    format_class->suffs = tiff_suffs;
}

static void vips_format_tiff_init(VipsFormatTiff *object) {
}

G_DEFINE_TYPE(VipsFormatTiff, vips_format_tiff, VIPS_TYPE_FORMAT);

#endif
