
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_MAGICK

#include <vips/vips.h>

int im_magick2vips(const char *filename, IMAGE *im) {
    im_error("im_magick2vips", "%s", _("libMagick support disabled"));
    return (-1);
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>

#include <vips/vips.h>
#include <vips/buf.h>
#include <vips/thread.h>

#include <magick/api.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#if !defined(QuantumRange)
#define QuantumRange MaxRGB
#endif

#if MAGICKCORE_HDRI_SUPPORT
#define UseHDRI = 1
#endif

typedef struct _Read {
    char *filename;
    IMAGE *im;

    Image *image;
    ImageInfo *image_info;
    ExceptionInfo exception;

    int n_frames;
    Image **frames;
    int frame_height;

    GMutex *lock;
} Read;

static int read_destroy(Read *read) {
#ifdef DEBUG
    printf("im_magick2vips: read_destroy: %s\n", read->filename);
#endif

    IM_FREEF(DestroyImage, read->image);
    IM_FREEF(DestroyImageInfo, read->image_info);
    IM_FREE(read->frames);
    IM_FREE(read->filename);
    DestroyExceptionInfo(&read->exception);
    IM_FREEF(g_mutex_free, read->lock);
    im_free(read);

    return (0);
}

static Read *read_new(const char *filename, IMAGE *im) {
    Read *read;
    static int inited = 0;

    if (!inited) {
#ifdef HAVE_MAGICKCOREGENESIS
        MagickCoreGenesis(im_get_argv0(), MagickFalse);
#else
        InitializeMagick("");
#endif
        inited = 1;
    }

    if (!(read = IM_NEW(NULL, Read)))
        return (NULL);
    read->filename = im_strdup(NULL, filename);
    read->im = im;
    read->image = NULL;
    read->image_info = CloneImageInfo(NULL);
    GetExceptionInfo(&read->exception);
    read->n_frames = 0;
    read->frames = NULL;
    read->frame_height = 0;
    read->lock = g_mutex_new();

    if (im_add_close_callback(im, (im_callback_fn)read_destroy, read, NULL)) {
        read_destroy(read);
        return (NULL);
    }

    if (!read->filename || !read->image_info)
        return (NULL);

    im_strncpy(read->image_info->filename, filename, MaxTextExtent);

#ifdef DEBUG
    printf("im_magick2vips: read_new: %s\n", read->filename);
#endif

    return (read);
}

static int get_bands(Image *image) {
    int bands;
    ImageType type = GetImageType(image, &image->exception);

    switch (type) {
    case BilevelType:
    case GrayscaleType:
        bands = 1;
        break;

    case GrayscaleMatteType:

        bands = 2;
        break;

    case PaletteType:
    case TrueColorType:
        bands = 3;
        break;

    case PaletteMatteType:
    case TrueColorMatteType:
    case ColorSeparationType:
        bands = 4;
        break;

    case ColorSeparationMatteType:
        bands = 5;
        break;

    default:
        im_error("im_magick2vips", _("unsupported image type %d"), (int)type);
        return (-1);
    }

    return (bands);
}

static int parse_header(Read *read) {
    IMAGE *im = read->im;
    Image *image = read->image;

    Image *p;
    int i;

    im->Xsize = image->columns;
    im->Ysize = image->rows;
    read->frame_height = image->rows;
    if ((im->Bands = get_bands(image)) < 0)
        return (-1);

    im->BandFmt = -1;
    if (image->depth >= 1 && image->depth <= 8)
        im->BandFmt = IM_BANDFMT_UCHAR;
    if (image->depth >= 9 && image->depth <= 16)
        im->BandFmt = IM_BANDFMT_USHORT;
#ifdef UseHDRI
    if (image->depth == 32)
        im->BandFmt = IM_BANDFMT_FLOAT;
    if (image->depth == 64)
        im->BandFmt = IM_BANDFMT_DOUBLE;
#else
    if (image->depth == 32)
        im->BandFmt = IM_BANDFMT_UINT;
#endif

    if (im->BandFmt == -1) {
        im_error("im_magick2vips", _("unsupported bit depth %d"), (int)image->depth);
        return (-1);
    }

    switch (image->colorspace) {
    case GRAYColorspace:
        if (im->BandFmt == IM_BANDFMT_USHORT)
            im->Type = IM_TYPE_GREY16;
        else
            im->Type = IM_TYPE_B_W;
        break;

    case RGBColorspace:
        if (im->BandFmt == IM_BANDFMT_USHORT)
            im->Type = IM_TYPE_RGB16;
        else
            im->Type = IM_TYPE_RGB;
        break;

    case sRGBColorspace:
        if (im->BandFmt == IM_BANDFMT_USHORT)
            im->Type = IM_TYPE_RGB16;
        else
            im->Type = IM_TYPE_sRGB;
        break;

    case CMYKColorspace:
        im->Type = IM_TYPE_CMYK;
        break;

    default:
        im_error("im_magick2vips", _("unsupported colorspace %d"), (int)image->colorspace);
        return (-1);
    }

    switch (image->units) {
    case PixelsPerInchResolution:
        im->Xres = image->x_resolution / 25.4;
        im->Yres = image->y_resolution / 25.4;
        break;

    case PixelsPerCentimeterResolution:
        im->Xres = image->x_resolution / 10.0;
        im->Yres = image->y_resolution / 10.0;
        break;

    default:
        im->Xres = 1.0;
        im->Yres = 1.0;
        break;
    }

    im->Coding = IM_CODING_NONE;

#ifdef HAVE_RESETIMAGEPROPERTYITERATOR
    {
        char *key;

        ResetImagePropertyIterator(image);
        while ((key = GetNextImageProperty(image))) {
            char name_text[256];
            VipsBuf name = VIPS_BUF_STATIC(name_text);

            vips_buf_appendf(&name, "magick-%s", key);
            im_meta_set_string(im, vips_buf_all(&name), GetImageProperty(image, key));
        }
    }
#elif defined(HAVE_RESETIMAGEATTRIBUTEITERATOR)
    {
        const ImageAttribute *attr;

        ResetImageAttributeIterator(image);
        while ((attr = GetNextImageAttribute(image))) {
            char name_text[256];
            VipsBuf name = VIPS_BUF_STATIC(name_text);

            vips_buf_appendf(&name, "magick-%s", attr->key);
            im_meta_set_string(im, vips_buf_all(&name), attr->value);
        }
    }
#else
    {
        const ImageAttribute *attr;

        for (attr = image->attributes; attr; attr = attr->next) {
            char name_text[256];
            VipsBuf name = VIPS_BUF_STATIC(name_text);

            vips_buf_appendf(&name, "magick-%s", attr->key);
            im_meta_set_string(im, vips_buf_all(&name), attr->value);
        }
    }
#endif

    read->n_frames = 0;
    for (p = image; p; (p = GetNextImageInList(p))) {
        if (p->columns != (unsigned int)im->Xsize || p->rows != (unsigned int)im->Ysize || get_bands(p) != im->Bands)
            break;

        read->n_frames += 1;
    }
    if (p)

        read->n_frames = 1;

    im->Ysize *= read->n_frames;
    if (!(read->frames = IM_ARRAY(NULL, read->n_frames, Image *)))
        return (-1);
    p = image;
    for (i = 0; i < read->n_frames; i++) {
        read->frames[i] = p;
        p = GetNextImageInList(p);
    }

    return (0);
}

#define SCALE(MAX) (QuantumRange < (MAX) ? 1 : ((QuantumRange + 1) / ((MAX) + 1)))

#define GRAY_LOOP(TYPE, MAX)                     \
    {                                            \
        TYPE *q = (TYPE *)q8;                    \
                                                 \
        for (x = 0; x < n; x++)                  \
            q[x] = pixels[x].green / SCALE(MAX); \
    }

#define GRAYA_LOOP(TYPE, MAX)                            \
    {                                                    \
        TYPE *q = (TYPE *)q8;                            \
                                                         \
        for (x = 0; x < n; x++) {                        \
            q[0] = pixels[x].green / SCALE(MAX);         \
            q[1] = MAX - pixels[x].opacity / SCALE(MAX); \
                                                         \
            q += 2;                                      \
        }                                                \
    }

#define RGB_LOOP(TYPE, MAX)                      \
    {                                            \
        TYPE *q = (TYPE *)q8;                    \
                                                 \
        for (x = 0; x < n; x++) {                \
            q[0] = pixels[x].red / SCALE(MAX);   \
            q[1] = pixels[x].green / SCALE(MAX); \
            q[2] = pixels[x].blue / SCALE(MAX);  \
                                                 \
            q += 3;                              \
        }                                        \
    }

#define RGBA_LOOP(TYPE, MAX)                             \
    {                                                    \
        TYPE *q = (TYPE *)q8;                            \
                                                         \
        for (x = 0; x < n; x++) {                        \
            q[0] = pixels[x].red / SCALE(MAX);           \
            q[1] = pixels[x].green / SCALE(MAX);         \
            q[2] = pixels[x].blue / SCALE(MAX);          \
            q[3] = MAX - pixels[x].opacity / SCALE(MAX); \
                                                         \
            q += 4;                                      \
        }                                                \
    }

static void unpack_pixels(IMAGE *im, PEL *q8, PixelPacket *pixels, int n) {
    int x;

    switch (im->Bands) {
    case 1:

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            GRAY_LOOP(unsigned char, 255);
            break;
        case IM_BANDFMT_USHORT:
            GRAY_LOOP(unsigned short, 65535);
            break;
        case IM_BANDFMT_UINT:
            GRAY_LOOP(unsigned int, 4294967295UL);
            break;
        case IM_BANDFMT_DOUBLE:
            GRAY_LOOP(double, QuantumRange);
            break;

        default:
            assert(0);
        }
        break;

    case 2:

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            GRAYA_LOOP(unsigned char, 255);
            break;
        case IM_BANDFMT_USHORT:
            GRAYA_LOOP(unsigned short, 65535);
            break;
        case IM_BANDFMT_UINT:
            GRAYA_LOOP(unsigned int, 4294967295UL);
            break;
        case IM_BANDFMT_DOUBLE:
            GRAYA_LOOP(double, QuantumRange);
            break;

        default:
            assert(0);
        }
        break;

    case 3:

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            RGB_LOOP(unsigned char, 255);
            break;
        case IM_BANDFMT_USHORT:
            RGB_LOOP(unsigned short, 65535);
            break;
        case IM_BANDFMT_UINT:
            RGB_LOOP(unsigned int, 4294967295UL);
            break;
        case IM_BANDFMT_DOUBLE:
            RGB_LOOP(double, QuantumRange);
            break;

        default:
            assert(0);
        }
        break;

    case 4:

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            RGBA_LOOP(unsigned char, 255);
            break;
        case IM_BANDFMT_USHORT:
            RGBA_LOOP(unsigned short, 65535);
            break;
        case IM_BANDFMT_UINT:
            RGBA_LOOP(unsigned int, 4294967295UL);
            break;
        case IM_BANDFMT_DOUBLE:
            RGBA_LOOP(double, QuantumRange);
            break;

        default:
            assert(0);
        }
        break;

    default:
        assert(0);
    }
}

static PixelPacket *get_pixels(Image *image, int left, int top, int width, int height) {
    PixelPacket *pixels;

#ifdef HAVE_GETVIRTUALPIXELS
    if (!(pixels = (PixelPacket *)GetVirtualPixels(image, left, top, width, height, &image->exception)))
#else
    if (!(pixels = GetImagePixels(image, left, top, width, height)))
#endif
        return (NULL);

#ifndef UseHDRI

    if (image->storage_class == PseudoClass) {
#ifdef HAVE_GETVIRTUALPIXELS
        IndexPacket *indexes = (IndexPacket *)GetVirtualIndexQueue(image);
#else
        IndexPacket *indexes = GetIndexes(image);
#endif

        int i;

        for (i = 0; i < width * height; i++) {
            IndexPacket x = indexes[i];

            if (x < image->colors) {
                pixels[i].red = image->colormap[x].red;
                pixels[i].green = image->colormap[x].green;
                pixels[i].blue = image->colormap[x].blue;
            }
        }
    }
#endif

    return (pixels);
}

static int magick_fill_region(REGION *out, void *seq, void *a, void *b) {
    Read *read = (Read *)a;
    Rect *r = &out->valid;
    int y;

    for (y = 0; y < r->height; y++) {
        int top = r->top + y;
        int frame = top / read->frame_height;
        int line = top % read->frame_height;

        PixelPacket *pixels;

        g_mutex_lock(read->lock);
        pixels = get_pixels(read->frames[frame], r->left, line, r->width, 1);
        g_mutex_unlock(read->lock);

        if (!pixels) {
            im_error("im_magick2vips", "%s", _("unable to read pixels"));
            return (-1);
        }

        unpack_pixels(read->im, (PEL *)IM_REGION_ADDR(out, r->left, top), pixels, r->width);
    }

    return (0);
}

int im_magick2vips(const char *filename, IMAGE *out) {
    Read *read;

    if (!(read = read_new(filename, out)))
        return (-1);

#ifdef HAVE_SETIMAGEOPTION

    SetImageOption(read->image_info, "dcm:display-range", "reset");
#endif

    read->image = ReadImage(read->image_info, &read->exception);
    if (!read->image) {
        im_error("im_magick2vips",
            _("unable to read file \"%s\"\n"
              "libMagick error: %s %s"),
            filename, read->exception.reason, read->exception.description);
        return (-1);
    }

    if (parse_header(read) || im_poutcheck(out) || im_demand_hint(out, IM_SMALLTILE, NULL) || im_generate(out, NULL, magick_fill_region, NULL, read, NULL))
        return (-1);

    return (0);
}

static int magick2vips_header(const char *filename, IMAGE *im) {
    Read *read;

    if (!(read = read_new(filename, im)))
        return (-1);

    read->image = PingImage(read->image_info, &read->exception);
    if (!read->image) {
        im_error("im_magick2vips",
            _("unable to ping file "
              "\"%s\"\nlibMagick error: %s %s"),
            filename, read->exception.reason, read->exception.description);
        return (-1);
    }

    if (parse_header(read))
        return (-1);

    if (im->Xsize <= 0 || im->Ysize <= 0) {
        im_error("im_magick2vips", "%s", _("bad image size"));
        return (-1);
    }

    return (0);
}

static int ismagick(const char *filename) {
    IMAGE *im;
    int result;

    if (!(im = im_open("dummy", "p")))
        return (-1);
    result = magick2vips_header(filename, im);
    im_error_clear();
    im_close(im);

    return (result == 0);
}

static const char *magick_suffs[] = {NULL};

typedef VipsFormat VipsFormatMagick;
typedef VipsFormatClass VipsFormatMagickClass;

static void vips_format_magick_class_init(VipsFormatMagickClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "magick";
    object_class->description = _("libMagick-supported");

    format_class->is_a = ismagick;
    format_class->header = magick2vips_header;
    format_class->load = im_magick2vips;
    format_class->suffs = magick_suffs;

    format_class->priority = -1000;
}

static void vips_format_magick_init(VipsFormatMagick *object) {
}

G_DEFINE_TYPE(VipsFormatMagick, vips_format_magick, VIPS_TYPE_FORMAT);

#endif
