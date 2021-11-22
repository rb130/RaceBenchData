

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static void *format_add_class(VipsFormatClass *format, GSList **formats) {

    *formats = g_slist_append(*formats, format);

    return (NULL);
}

static gint format_compare(VipsFormatClass *a, VipsFormatClass *b) {
    return (b->priority - a->priority);
}

void *vips_format_map(VSListMap2Fn fn, void *a, void *b) {
    GSList *formats;
    void *result;

    formats = NULL;
    (void)vips_class_map_concrete_all(g_type_from_name("VipsFormat"), (VipsClassMap)format_add_class, (void *)&formats);

    formats = g_slist_sort(formats, (GCompareFunc)format_compare);
    result = im_slist_map2(formats, fn, a, b);
    g_slist_free(formats);

    return (result);
}

G_DEFINE_ABSTRACT_TYPE(VipsFormat, vips_format, VIPS_TYPE_OBJECT);

static void vips_format_print_class(VipsObjectClass *object_class, VipsBuf *buf) {
    VipsFormatClass *class = VIPS_FORMAT_CLASS(object_class);
    const char **p;

    VIPS_OBJECT_CLASS(vips_format_parent_class)->print_class(object_class, buf);

    vips_buf_appends(buf, ", (");
    for (p = class->suffs; *p; p++) {
        vips_buf_appendf(buf, "%s", *p);
        if (p[1]) {
            vips_buf_appends(buf, ", ");
        }
    }
    vips_buf_appends(buf, ") ");

    if (class->is_a) {
        vips_buf_appends(buf, "is_a ");
    }
    if (class->header) {
        vips_buf_appends(buf, "header ");
    }
    if (class->load) {
        vips_buf_appends(buf, "load ");
    }
    if (class->save) {
        vips_buf_appends(buf, "save ");
    }
    if (class->get_flags) {
        vips_buf_appends(buf, "get_flags ");
    }
}

static void vips_format_class_init(VipsFormatClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;

    object_class->print_class = vips_format_print_class;
}

static void vips_format_init(VipsFormat *object) {
}

VipsFormatFlags vips_format_get_flags(VipsFormatClass *format, const char *filename) {
    return (format->get_flags ? format->get_flags(filename) : 0);
}

static const char *vips_suffs[] = {".v", "", NULL};

static int file2vips(const char *filename, IMAGE *out) {
    IMAGE *im;

    if (!(im = im_open_local(out, filename, "r")) || im_copy(im, out)) {
        return (-1);
    }

    return (0);
}

static int vips2file(IMAGE *im, const char *filename) {
    IMAGE *out;

    if (!(out = im_open_local(im, filename, "w")) || im_copy(im, out)) {
        return (-1);
    }

    return (0);
}

static VipsFormatFlags vips_flags(const char *filename) {
    return (VIPS_FORMAT_PARTIAL);
}

typedef VipsFormat VipsFormatVips;
typedef VipsFormatClass VipsFormatVipsClass;

static void vips_format_vips_class_init(VipsFormatVipsClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "vips";
    object_class->description = _("VIPS");

    format_class->is_a = im_isvips;
    format_class->header = file2vips;
    format_class->load = file2vips;
    format_class->save = vips2file;
    format_class->get_flags = vips_flags;
    format_class->suffs = vips_suffs;
}

static void vips_format_vips_init(VipsFormatVips *object) {
}

G_DEFINE_TYPE(VipsFormatVips, vips_format_vips, VIPS_TYPE_FORMAT);

void im__format_init(void) {
    extern GType vips_format_csv_get_type();
    extern GType vips_format_ppm_get_type();
    extern GType vips_format_analyze_get_type();
    extern GType vips_format_rad_get_type();

    vips_format_vips_get_type();
#ifdef HAVE_JPEG
    extern GType vips_format_jpeg_get_type();
    vips_format_jpeg_get_type();
#endif
#ifdef HAVE_PNG
    extern GType vips_format_png_get_type();
    vips_format_png_get_type();
#endif
    vips_format_csv_get_type();
    vips_format_ppm_get_type();
    vips_format_analyze_get_type();
#ifdef HAVE_OPENEXR
    extern GType vips_format_exr_get_type();
    vips_format_exr_get_type();
#endif
#ifdef HAVE_MATIO
    extern GType vips_format_mat_get_type();
    vips_format_mat_get_type();
#endif
    vips_format_rad_get_type();
#ifdef HAVE_MAGICK
    extern GType vips_format_magick_get_type();
    vips_format_magick_get_type();
#endif
#ifdef HAVE_TIFF
    extern GType vips_format_tiff_get_type();
    vips_format_tiff_get_type();
#endif
}

static void *format_for_file_sub(VipsFormatClass *format, const char *name, const char *filename) {
    if (format->is_a) {
        if (format->is_a(filename)) {
            return (format);
        }
    } else if (im_filename_suffix_match(filename, format->suffs)) {
        return (format);
    }

    return (NULL);
}

VipsFormatClass *vips_format_for_file(const char *name) {
    char filename[FILENAME_MAX];
    char options[FILENAME_MAX];
    VipsFormatClass *format;

    im_filename_split(name, filename, options);

    if (!im_existsf("%s", filename)) {
        im_error("format_for_file", _("file \"%s\" not found"), filename);
        return (NULL);
    }

    if (!(format = (VipsFormatClass *)vips_format_map((VSListMap2Fn)format_for_file_sub, (void *)name, (void *)filename))) {
        im_error("format_for_file", _("file \"%s\" not a known format"), filename);
        return (NULL);
    }

    return (format);
}

static void *format_for_name_sub(VipsFormatClass *format, const char *name) {
    if (format->save && im_filename_suffix_match(name, format->suffs)) {
        return (format);
    }

    return (NULL);
}

VipsFormatClass *vips_format_for_name(const char *name) {
    VipsFormatClass *format;

    if (!(format = (VipsFormatClass *)vips_format_map((VSListMap2Fn)format_for_name_sub, (void *)name, NULL))) {
        char suffix[FILENAME_MAX];

        im_filename_suffix(name, suffix);
        im_error("vips_format_for_name", _("\"%s\" is not a supported image format."), suffix);

        return (NULL);
    }

    return (format);
}

int vips_format_read(const char *name, IMAGE *out) {
    VipsFormatClass *format;

    if (!(format = vips_format_for_file(name)) || format->load(name, out)) {
        return (-1);
    }

    return (0);
}

int vips_format_write(IMAGE *im, const char *name) {
    VipsFormatClass *format;

    if (!(format = vips_format_for_name(name)) || format->save(im, name)) {
        return (-1);
    }

    return (0);
}
