

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef int (*write_fn)(IMAGE *in, FILE *fp, PEL *p);

typedef struct {
    IMAGE *in;
    FILE *fp;
    char *name;
    write_fn fn;
} Write;

static void write_destroy(Write *write) {
    IM_FREEF(fclose, write->fp);
    IM_FREE(write->name);

    im_free(write);
}

static Write *write_new(IMAGE *in, const char *name) {
    Write *write;

    if (!(write = IM_NEW(NULL, Write))) {
        return (NULL);
    }

    write->in = in;
    write->name = im_strdup(NULL, name);
    write->fp = im__file_open_write(name);

    if (!write->name || !write->fp) {
        write_destroy(write);
        return (NULL);
    }

    return (write);
}

static int write_ppm_line_ascii(IMAGE *in, FILE *fp, PEL *p) {
    const int sk = IM_IMAGE_SIZEOF_PEL(in);
    int x, k;

    for (x = 0; x < in->Xsize; x++) {
        for (k = 0; k < in->Bands; k++) {
            switch (in->BandFmt) {
            case IM_BANDFMT_UCHAR:
                fprintf(fp, "%d ", p[k]);
                break;

            case IM_BANDFMT_USHORT:
                fprintf(fp, "%d ", ((unsigned short *)p)[k]);
                break;

            case IM_BANDFMT_UINT:
                fprintf(fp, "%d ", ((unsigned int *)p)[k]);
                break;

            default:
                g_assert(0);
            }
        }

        fprintf(fp, " ");

        p += sk;
    }

    if (!fprintf(fp, "\n")) {
        im_error("im_vips2ppm", "%s", _("write error ... disc full?"));
        return (-1);
    }

    return (0);
}

static int write_ppm_line_binary(IMAGE *in, FILE *fp, PEL *p) {
    if (!fwrite(p, IM_IMAGE_SIZEOF_LINE(in), 1, fp)) {
        im_error("im_vips2ppm", "%s", _("write error ... disc full?"));
        return (-1);
    }

    return (0);
}

static int write_ppm_block(REGION *region, Rect *area, void *a) {
    Write *write = (Write *)a;
    int i;

    for (i = 0; i < area->height; i++) {
        PEL *p = (PEL *)IM_REGION_ADDR(region, 0, area->top + i);

        if (write->fn(write->in, write->fp, p)) {
            return (-1);
        }
    }

    return (0);
}

static int write_ppm(Write *write, int ascii) {
    IMAGE *in = write->in;

    char *magic;
    time_t timebuf;

    if (in->BandFmt == IM_BANDFMT_FLOAT && in->Bands == 3) {
        magic = "PF";
    } else if (in->BandFmt == IM_BANDFMT_FLOAT && in->Bands == 1) {
        magic = "Pf";
    } else if (in->Bands == 1 && ascii) {
        magic = "P2";
    } else if (in->Bands == 1 && !ascii) {
        magic = "P5";
    } else if (in->Bands == 3 && ascii) {
        magic = "P3";
    } else if (in->Bands == 3 && !ascii) {
        magic = "P6";
    } else {
        g_assert(0);
    }

    fprintf(write->fp, "%s\n", magic);
    time(&timebuf);
    fprintf(write->fp, "#im_vips2ppm - %s\n", ctime(&timebuf));
    fprintf(write->fp, "%d %d\n", in->Xsize, in->Ysize);

    switch (in->BandFmt) {
    case IM_BANDFMT_UCHAR:
        fprintf(write->fp, "%d\n", UCHAR_MAX);
        break;

    case IM_BANDFMT_USHORT:
        fprintf(write->fp, "%d\n", USHRT_MAX);
        break;

    case IM_BANDFMT_UINT:
        fprintf(write->fp, "%d\n", UINT_MAX);
        break;

    case IM_BANDFMT_FLOAT: {
        double scale;

        if (im_meta_get_double(in, "pfm-scale", &scale)) {
            scale = 1;
        }
        if (!im_amiMSBfirst()) {
            scale *= -1;
        }
        fprintf(write->fp, "%g\n", scale);
    } break;

    default:
        g_assert(0);
    }

    write->fn = ascii ? write_ppm_line_ascii : write_ppm_line_binary;

    if (vips_sink_disc(write->in, write_ppm_block, write)) {
        return (-1);
    }

    return (0);
}

int im_vips2ppm(IMAGE *in, const char *filename) {
    Write *write;
    int ascii;
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];

    ascii = 0;

    im_filename_split(filename, name, mode);
    if (strcmp(mode, "") != 0) {
        if (im_isprefix("binary", mode)) {
            ascii = 0;
        } else if (im_isprefix("ascii", mode)) {
            ascii = 1;
        } else {
            im_error("im_vips2ppm", "%s",
                _("bad mode string, "
                  "should be \"binary\" or \"ascii\""));
            return (-1);
        }
    }

    if (im_check_uintorf("im_vips2ppm", in) || im_check_bands_1or3("im_vips2ppm", in) || im_check_uncoded("im_vips2ppm", in) || im_pincheck(in)) {
        return (-1);
    }

    if (im_bits_of_fmt(in->BandFmt) > 8 && !ascii && in->BandFmt != IM_BANDFMT_FLOAT) {
        im_error("im_vips2ppm", "%s", _("binary >8 bit images must be float"));
        return (-1);
    }

    if (!(write = write_new(in, name))) {
        return (-1);
    }

    if (write_ppm(write, ascii)) {
        write_destroy(write);
        return (-1);
    }
    write_destroy(write);

    return (0);
}
