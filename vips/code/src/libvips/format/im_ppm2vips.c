
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_THING (80)

static void skip_line(FILE *fp) {
    int ch;

    while ((ch = fgetc(fp)) != '\n') {
        ;
    }
}

static void skip_white_space(FILE *fp) {
    int ch;

    while (isspace(ch = fgetc(fp))) {
        ;
    }
    ungetc(ch, fp);

    if (ch == '#') {
        skip_line(fp);
        skip_white_space(fp);
    }
}

static int read_int(FILE *fp, int *i) {
    skip_white_space(fp);
    if (fscanf(fp, "%d", i) != 1) {
        im_error("im_ppm2vips", "%s", _("bad int"));
        return (-1);
    }

    return (0);
}

static int read_float(FILE *fp, float *f) {
    skip_white_space(fp);
    if (fscanf(fp, "%f", f) != 1) {
        im_error("im_ppm2vips", "%s", _("bad float"));
        return (-1);
    }

    return (0);
}

static int read_header(FILE *fp, IMAGE *out, int *bits, int *ascii, int *msb_first) {
    int width, height, bands, fmt, type;
    int index;
    char buf[IM_MAX_THING];

    static char *magic_names[] = {"P1", "P2", "P3", "P4", "P5", "P6", "PF", "Pf"};

    static int lookup_bits[] = {1, 8, 8, 1, 8, 8, 32, 32};
    static int lookup_bands[] = {1, 1, 3, 1, 1, 3, 3, 1};
    static int lookup_ascii[] = {1, 1, 1, 0, 0, 0, 0, 0};

    buf[0] = fgetc(fp);
    buf[1] = fgetc(fp);
    buf[2] = '\0';

    for (index = 0; index < IM_NUMBER(magic_names); index++) {
        if (strcmp(magic_names[index], buf) == 0) {
            break;
        }
    }
    if (index == IM_NUMBER(magic_names)) {
        im_error("im_ppm2vips", "%s", _("bad magic number"));
        return (-1);
    }
    *bits = lookup_bits[index];
    bands = lookup_bands[index];
    *ascii = lookup_ascii[index];

    *msb_first = 0;

    if (read_int(fp, &width) || read_int(fp, &height)) {
        return (-1);
    }

    if (*bits > 1) {
        if (index == 6 || index == 7) {
            float scale;

            if (read_float(fp, &scale)) {
                return (-1);
            }

            *msb_first = scale > 0;
            im_meta_set_double(out, "pfm-scale", fabs(scale));
        } else {
            int max_value;

            if (read_int(fp, &max_value)) {
                return (-1);
            }

            if (max_value > 255) {
                *bits = 16;
            }
            if (max_value > 65535) {
                *bits = 32;
            }
        }
    }

    if (!*ascii && !isspace(fgetc(fp))) {
        im_error("im_ppm2vips", "%s", _("not whitespace before start of binary data"));
        return (-1);
    }

    switch (*bits) {
    case 1:
    case 8:
        fmt = IM_BANDFMT_UCHAR;
        break;

    case 16:
        fmt = IM_BANDFMT_USHORT;
        break;

    case 32:
        if (index == 6 || index == 7) {
            fmt = IM_BANDFMT_FLOAT;
        } else {
            fmt = IM_BANDFMT_UINT;
        }
        break;

    default:
        g_assert(0);
    }

    if (bands == 1) {
        if (fmt == IM_BANDFMT_USHORT) {
            type = IM_TYPE_GREY16;
        } else {
            type = IM_TYPE_B_W;
        }
    } else {
        if (fmt == IM_BANDFMT_USHORT) {
            type = IM_TYPE_RGB16;
        } else if (fmt == IM_BANDFMT_UINT) {
            type = IM_TYPE_RGB;
        } else {
            type = IM_TYPE_sRGB;
        }
    }

    im_initdesc(out, width, height, bands, (*bits == 1) ? 8 : *bits, fmt, IM_CODING_NONE, type, 1.0, 1.0, 0, 0);

    return (0);
}

static int read_mmap(FILE *fp, const char *filename, int msb_first, IMAGE *out) {
    const int header_offset = ftell(fp);
    IMAGE *t[2];

    if (im_open_local_array(out, t, 2, "im_ppm2vips", "p") || im_raw2vips(filename, t[0], out->Xsize, out->Ysize, IM_IMAGE_SIZEOF_PEL(out), header_offset) || im_copy_morph(t[0], t[1], out->Bands, out->BandFmt, out->Coding) || im_copy_native(t[1], out, msb_first)) {
        return (-1);
    }

    return (0);
}

static int read_ascii(FILE *fp, IMAGE *out) {
    int x, y;
    PEL *buf;

    if (im_outcheck(out) || im_setupout(out) || !(buf = IM_ARRAY(out, IM_IMAGE_SIZEOF_LINE(out), PEL))) {
        return (-1);
    }

    for (y = 0; y < out->Ysize; y++) {
        for (x = 0; x < out->Xsize * out->Bands; x++) {
            int val;

            if (read_int(fp, &val)) {
                return (-1);
            }

            switch (out->BandFmt) {
            case IM_BANDFMT_UCHAR:
                buf[x] = IM_CLIP(0, val, 255);
                break;

            case IM_BANDFMT_USHORT:
                ((unsigned short *)buf)[x] = IM_CLIP(0, val, 65535);
                break;

            case IM_BANDFMT_UINT:
                ((unsigned int *)buf)[x] = val;
                break;

            default:
                g_assert(0);
            }
        }

        if (im_writeline(y, out, buf)) {
            return (-1);
        }
    }

    return (0);
}

static int read_1bit_ascii(FILE *fp, IMAGE *out) {
    int x, y;
    PEL *buf;

    if (im_outcheck(out) || im_setupout(out) || !(buf = IM_ARRAY(out, IM_IMAGE_SIZEOF_LINE(out), PEL))) {
        return (-1);
    }

    for (y = 0; y < out->Ysize; y++) {
        for (x = 0; x < out->Xsize * out->Bands; x++) {
            int val;

            if (read_int(fp, &val)) {
                return (-1);
            }

            if (val == 1) {
                buf[x] = 0;
            } else {
                buf[x] = 255;
            }
        }

        if (im_writeline(y, out, buf)) {
            return (-1);
        }
    }

    return (0);
}

static int read_1bit_binary(FILE *fp, IMAGE *out) {
    int x, y, i;
    int bits;
    PEL *buf;

    if (im_outcheck(out) || im_setupout(out) || !(buf = IM_ARRAY(out, IM_IMAGE_SIZEOF_LINE(out), PEL))) {
        return (-1);
    }

    bits = fgetc(fp);
    for (i = 0, y = 0; y < out->Ysize; y++) {
        for (x = 0; x < out->Xsize * out->Bands; x++, i++) {
            buf[x] = (bits & 128) ? 255 : 0;
            bits <<= 1;
            if ((i & 7) == 7) {
                bits = fgetc(fp);
            }
        }

        if (im_writeline(y, out, buf)) {
            return (-1);
        }
    }

    return (0);
}

static int parse_ppm(FILE *fp, const char *filename, IMAGE *out) {
    int bits;
    int ascii;
    int msb_first;

    if (read_header(fp, out, &bits, &ascii, &msb_first)) {
        return (-1);
    }

    if (!ascii && bits >= 8) {
        return (read_mmap(fp, filename, msb_first, out));
    } else if (!ascii && bits == 1) {
        return (read_1bit_binary(fp, out));
    } else if (ascii && bits == 1) {
        return (read_1bit_ascii(fp, out));
    } else {
        return (read_ascii(fp, out));
    }
}

static int ppm2vips_header(const char *filename, IMAGE *out) {
    FILE *fp;
    int bits;
    int ascii;
    int msb_first;

    if (!(fp = im__file_open_read(filename, NULL))) {
        return (-1);
    }
    if (read_header(fp, out, &bits, &ascii, &msb_first)) {
        fclose(fp);
        return (-1);
    }

    fclose(fp);

    return (0);
}

static int isppmmmap(const char *filename) {
    IMAGE *im;
    FILE *fp;
    int bits;
    int ascii;
    int msb_first;

    if (!(fp = im__file_open_read(filename, NULL))) {
        return (-1);
    }

    if (!(im = im_open("temp", "p"))) {
        fclose(fp);
        return (0);
    }
    if (read_header(fp, im, &bits, &ascii, &msb_first)) {
        im_close(im);
        fclose(fp);
        return (0);
    }
    im_close(im);
    fclose(fp);

    return (!ascii && bits >= 8);
}

int im_ppm2vips(const char *filename, IMAGE *out) {
    FILE *fp;

    if (!(fp = im__file_open_read(filename, NULL))) {
        return (-1);
    }
    if (parse_ppm(fp, filename, out)) {
        fclose(fp);
        return (-1);
    }
    fclose(fp);

    return (0);
}

static int isppm(const char *filename) {
    unsigned char buf[2];

    if (im__get_bytes(filename, buf, 2)) {
        if (buf[0] == 'P' && (buf[1] >= '1' || buf[1] <= '6')) {
            return (1);
        }
    }

    return (0);
}

static VipsFormatFlags ppm_flags(const char *filename) {
    VipsFormatFlags flags;

    flags = 0;
    if (isppmmmap(filename)) {
        flags |= VIPS_FORMAT_PARTIAL;
    }

    return (flags);
}

static const char *ppm_suffs[] = {".ppm", ".pgm", ".pbm", ".pfm", NULL};

typedef VipsFormat VipsFormatPpm;
typedef VipsFormatClass VipsFormatPpmClass;

static void vips_format_ppm_class_init(VipsFormatPpmClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "ppm";
    object_class->description = _("PPM/PBM/PNM/PFM");

    format_class->is_a = isppm;
    format_class->header = ppm2vips_header;
    format_class->load = im_ppm2vips;
    format_class->save = im_vips2ppm;
    format_class->get_flags = ppm_flags;
    format_class->suffs = ppm_suffs;
}

static void vips_format_ppm_init(VipsFormatPpm *object) {
}

G_DEFINE_TYPE(VipsFormatPpm, vips_format_ppm, VIPS_TYPE_FORMAT);
