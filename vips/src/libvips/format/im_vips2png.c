

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_PNG

#include <vips/vips.h>

int im_vips2png(IMAGE *in, const char *filename) {
    im_error("im_vips2png", "%s", _("PNG support disabled"));
    return (-1);
}

#else

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include <png.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#if PNG_LIBPNG_VER < 10003
#error "PNG library too old."
#endif

static void user_error_function(png_structp png_ptr, png_const_charp error_msg) {
    im_error("im_vips2png", _("PNG error: \"%s\""), error_msg);
}

static void user_warning_function(png_structp png_ptr, png_const_charp warning_msg) {
    im_error("im_vips2png", _("PNG warning: \"%s\""), warning_msg);
}

typedef struct {
    IMAGE *in;

    FILE *fp;
    png_structp pPng;
    png_infop pInfo;
    png_bytep *row_pointer;
} Write;

static void write_destroy(Write *write) {
    IM_FREEF(im_close, write->in);
    IM_FREEF(fclose, write->fp);
    if (write->pPng)
        png_destroy_write_struct(&write->pPng, &write->pInfo);
    IM_FREE(write->row_pointer);

    im_free(write);
}

#define UC IM_BANDFMT_UCHAR
#define US IM_BANDFMT_USHORT

static int bandfmt_png[10] = {

    UC, UC, US, US, US, US, UC, UC, UC, UC};

static Write *write_new(IMAGE *in) {
    Write *write;

    if (!(write = IM_NEW(NULL, Write)))
        return (NULL);
    memset(write, 0, sizeof(Write));

    if (!(write->in = im__convert_saveable(in, IM__RGBA, bandfmt_png))) {
        im_error("im_vips2png", "%s", _("unable to convert to saveable format"));
        write_destroy(write);
        return (NULL);
    }

    write->row_pointer = IM_ARRAY(NULL, in->Ysize, png_bytep);
    write->fp = NULL;
    write->pPng = NULL;
    write->pInfo = NULL;

    if (!write->row_pointer) {
        write_destroy(write);
        return (NULL);
    }

    if (!(write->pPng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_function, user_warning_function))) {
        write_destroy(write);
        return (NULL);
    }

    if (setjmp(write->pPng->jmpbuf)) {
        write_destroy(write);
        return (NULL);
    }

    if (!(write->pInfo = png_create_info_struct(write->pPng))) {
        write_destroy(write);
        return (NULL);
    }

    return (write);
}

static int write_png_block(REGION *region, Rect *area, void *a) {
    Write *write = (Write *)a;
    int i;

    if (setjmp(write->pPng->jmpbuf))
        return (-1);

    for (i = 0; i < area->height; i++)
        write->row_pointer[i] = (png_bytep)IM_REGION_ADDR(region, 0, area->top + i);

    png_write_rows(write->pPng, write->row_pointer, area->height);

    return (0);
}

static int write_vips(Write *write, int compress, int interlace) {
    IMAGE *in = write->in;

    int i, nb_passes;

    g_assert(in->BandFmt == IM_BANDFMT_UCHAR || in->BandFmt == IM_BANDFMT_USHORT);
    g_assert(in->Coding == IM_CODING_NONE);
    g_assert(in->Bands > 0 && in->Bands < 5);

    if (setjmp(write->pPng->jmpbuf))
        return (-1);

    if (im_pincheck(in))
        return (-1);
    if (compress < 0 || compress > 9) {
        im_error("im_vips2png", "%s", _("compress should be in [0,9]"));
        return (-1);
    }

    png_set_compression_level(write->pPng, compress);

    write->pInfo->width = in->Xsize;
    write->pInfo->height = in->Ysize;
    write->pInfo->bit_depth = (in->BandFmt == IM_BANDFMT_UCHAR ? 8 : 16);
    write->pInfo->gamma = (float)1.0;

    switch (in->Bands) {
    case 1:
        write->pInfo->color_type = PNG_COLOR_TYPE_GRAY;
        break;
    case 2:
        write->pInfo->color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
        break;
    case 3:
        write->pInfo->color_type = PNG_COLOR_TYPE_RGB;
        break;
    case 4:
        write->pInfo->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        break;

    default:
        g_assert(0);
    }

    png_write_info(write->pPng, write->pInfo);

    if (write->pInfo->bit_depth > 8 && !im_amiMSBfirst())
        png_set_swap(write->pPng);

    if (interlace)
        nb_passes = png_set_interlace_handling(write->pPng);
    else
        nb_passes = 1;

    for (i = 0; i < nb_passes; i++)
        if (vips_sink_disc(write->in, write_png_block, write))
            return (-1);

    if (setjmp(write->pPng->jmpbuf))
        return (-1);

    png_write_end(write->pPng, write->pInfo);

    return (0);
}

int im_vips2png(IMAGE *in, const char *filename) {
    Write *write;
    int compress;
    int interlace;

    char *p, *q;

    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char buf[FILENAME_MAX];

    if (!(write = write_new(in)))
        return (-1);

    im_filename_split(filename, name, mode);
    strcpy(buf, mode);
    p = &buf[0];
    compress = 6;
    interlace = 0;
    if ((q = im_getnextoption(&p)))
        compress = atoi(q);
    if ((q = im_getnextoption(&p)))
        interlace = atoi(q);

    if (!(write->fp = im__file_open_write(name))) {
        write_destroy(write);
        return (-1);
    }
    png_init_io(write->pPng, write->fp);

    if (write_vips(write, compress, interlace)) {
        write_destroy(write);
        im_error("im_vips2png", _("unable to write \"%s\""), name);

        return (-1);
    }
    write_destroy(write);

    return (0);
}

#endif
