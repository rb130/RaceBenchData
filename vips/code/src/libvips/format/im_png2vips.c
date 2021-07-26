
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_PNG

#include <vips/vips.h>

int im_png2vips(const char *name, IMAGE *out) {
    im_error("im_png2vips", "%s", _("PNG support disabled"));
    return (-1);
}

#else

#include <stdio.h>
#include <assert.h>
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
    im_error("im_png2vips", _("PNG error: \"%s\""), error_msg);
}

static void user_warning_function(png_structp png_ptr, png_const_charp warning_msg) {
    im_error("im_png2vips", _("PNG warning: \"%s\""), warning_msg);
}

typedef struct {
    char *name;
    IMAGE *out;

    FILE *fp;
    png_structp pPng;
    png_infop pInfo;
    png_bytep *row_pointer;
    png_bytep data;
} Read;

static void read_destroy(Read *read) {
    IM_FREE(read->name);
    IM_FREEF(fclose, read->fp);
    if (read->pPng)
        png_destroy_read_struct(&read->pPng, &read->pInfo, NULL);
    IM_FREE(read->row_pointer);
    IM_FREE(read->data);

    im_free(read);
}

static Read *read_new(const char *name, IMAGE *out) {
    Read *read;

    if (!(read = IM_NEW(NULL, Read)))
        return (NULL);

    read->name = im_strdup(NULL, name);
    read->out = out;
    read->fp = NULL;
    read->pPng = NULL;
    read->pInfo = NULL;
    read->row_pointer = NULL;
    read->data = NULL;

    if (!(read->fp = im__file_open_read(name, NULL))) {
        read_destroy(read);
        return (NULL);
    }

    if (!(read->pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_function, user_warning_function))) {
        read_destroy(read);
        return (NULL);
    }

    if (setjmp(read->pPng->jmpbuf)) {
        read_destroy(read);
        return (NULL);
    }

    if (!(read->pInfo = png_create_info_struct(read->pPng))) {
        read_destroy(read);
        return (NULL);
    }

    return (read);
}

static int png2vips_interlace(Read *read) {
    const int rowbytes = IM_IMAGE_SIZEOF_LINE(read->out);
    int y;

    if (!(read->row_pointer = IM_ARRAY(NULL, read->pInfo->height, png_bytep)))
        return (-1);
    if (!(read->data = (png_bytep)im_malloc(NULL, read->pInfo->height * rowbytes)))
        return (-1);

    for (y = 0; y < (int)read->pInfo->height; y++)
        read->row_pointer[y] = read->data + y * rowbytes;
    if (im_outcheck(read->out) || im_setupout(read->out) || setjmp(read->pPng->jmpbuf))
        return (-1);

    png_read_image(read->pPng, read->row_pointer);

    for (y = 0; y < (int)read->pInfo->height; y++)
        if (im_writeline(y, read->out, read->row_pointer[y]))
            return (-1);

    return (0);
}

static int png2vips_noninterlace(Read *read) {
    const int rowbytes = IM_IMAGE_SIZEOF_LINE(read->out);
    int y;

    if (!(read->data = (png_bytep)im_malloc(NULL, rowbytes)))
        return (-1);
    if (im_outcheck(read->out) || im_setupout(read->out) || setjmp(read->pPng->jmpbuf))
        return (-1);

    for (y = 0; y < (int)read->pInfo->height; y++) {
        png_read_row(read->pPng, read->data, NULL);

        if (im_writeline(y, read->out, read->data))
            return (-1);
    }

    return (0);
}

static int png2vips(Read *read, int header_only) {
    int bands, bpp, type;

    if (setjmp(read->pPng->jmpbuf))
        return (-1);

    png_init_io(read->pPng, read->fp);
    png_read_info(read->pPng, read->pInfo);

    switch (read->pInfo->color_type) {
    case PNG_COLOR_TYPE_PALETTE:
        bands = 3;

        if (read->pInfo->num_trans)
            bands = 4;

        break;

    case PNG_COLOR_TYPE_GRAY:
        bands = 1;
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        bands = 2;
        break;
    case PNG_COLOR_TYPE_RGB:
        bands = 3;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        bands = 4;
        break;

    default:
        im_error("im_png2vips", "%s", _("unsupported color type"));
        return (-1);
    }

    bpp = read->pInfo->bit_depth > 8 ? 2 : 1;

    if (bpp > 1) {
        if (bands < 3)
            type = IM_TYPE_GREY16;
        else
            type = IM_TYPE_RGB16;
    } else {
        if (bands < 3)
            type = IM_TYPE_B_W;
        else
            type = IM_TYPE_sRGB;
    }

    if (read->pInfo->color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(read->pPng);

    if (read->pInfo->bit_depth < 8) {
        png_set_packing(read->pPng);
        png_set_shift(read->pPng, &(read->pInfo->sig_bit));
    }

    if (read->pInfo->bit_depth > 8 && !im_amiMSBfirst())
        png_set_swap(read->pPng);

    im_initdesc(read->out, read->pInfo->width, read->pInfo->height, bands, bpp == 1 ? IM_BBITS_BYTE : IM_BBITS_SHORT, bpp == 1 ? IM_BANDFMT_UCHAR : IM_BANDFMT_USHORT, IM_CODING_NONE, type, 1.0, 1.0, 0, 0);

    if (!header_only) {
        if (png_set_interlace_handling(read->pPng) > 1) {
            if (png2vips_interlace(read))
                return (-1);
        } else {
            if (png2vips_noninterlace(read))
                return (-1);
        }
    }

    return (0);
}

static int png2vips_header(const char *name, IMAGE *out) {
    Read *read;

    if (!(read = read_new(name, out)))
        return (-1);

    if (png2vips(read, 1)) {
        read_destroy(read);
        return (-1);
    }

    read_destroy(read);

    return (0);
}

int im_png2vips(const char *name, IMAGE *out) {
    Read *read;

#ifdef DEBUG
    printf("im_png2vips: reading \"%s\"\n", name);
#endif

    if (!(read = read_new(name, out)))
        return (-1);

    if (png2vips(read, 0)) {
        read_destroy(read);
        return (-1);
    }

    read_destroy(read);

    return (0);
}

static int ispng(const char *filename) {
    unsigned char buf[8];

    return (im__get_bytes(filename, buf, 8) && !png_sig_cmp(buf, 0, 8));
}

static const char *png_suffs[] = {".png", NULL};

typedef VipsFormat VipsFormatPng;
typedef VipsFormatClass VipsFormatPngClass;

static void vips_format_png_class_init(VipsFormatPngClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "png";
    object_class->description = _("PNG");

    format_class->is_a = ispng;
    format_class->header = png2vips_header;
    format_class->load = im_png2vips;
    format_class->save = im_vips2png;
    format_class->suffs = png_suffs;
}

static void vips_format_png_init(VipsFormatPng *object) {
}

G_DEFINE_TYPE(VipsFormatPng, vips_format_png, VIPS_TYPE_FORMAT);

#endif
