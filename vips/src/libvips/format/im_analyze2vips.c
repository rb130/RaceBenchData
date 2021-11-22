

#define DEBUG

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include "dbh.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef enum { BYTE, SHORT, INT, FLOAT, STRING } Type;

typedef struct {
    const char *name;
    Type type;
    glong offset;
    int len;
} Field;

static Field dsr_header[] = {{"dsr-header_key.sizeof_hdr", INT, G_STRUCT_OFFSET(struct dsr, hk.sizeof_hdr), 4}, {"dsr-header_key.data_type", STRING, G_STRUCT_OFFSET(struct dsr, hk.data_type), 10}, {"dsr-header_key.db_name", STRING, G_STRUCT_OFFSET(struct dsr, hk.db_name), 18}, {"dsr-header_key.extents", INT, G_STRUCT_OFFSET(struct dsr, hk.extents), 4}, {"dsr-header_key.session_error", SHORT, G_STRUCT_OFFSET(struct dsr, hk.session_error), 2}, {"dsr-header_key.regular", BYTE, G_STRUCT_OFFSET(struct dsr, hk.regular), 1}, {"dsr-header_key.hkey_un0", BYTE, G_STRUCT_OFFSET(struct dsr, hk.hkey_un0), 1},

    {"dsr-image_dimension.dim[0]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[0]), 2}, {"dsr-image_dimension.dim[1]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[1]), 2}, {"dsr-image_dimension.dim[2]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[2]), 2}, {"dsr-image_dimension.dim[3]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[3]), 2}, {"dsr-image_dimension.dim[4]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[4]), 2}, {"dsr-image_dimension.dim[5]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[5]), 2}, {"dsr-image_dimension.dim[6]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[6]), 2}, {"dsr-image_dimension.dim[7]", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim[7]), 2}, {"dsr-image_dimension.vox_units[0]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.vox_units[0]), 1}, {"dsr-image_dimension.vox_units[1]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.vox_units[1]), 1}, {"dsr-image_dimension.vox_units[2]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.vox_units[2]), 1}, {"dsr-image_dimension.vox_units[3]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.vox_units[3]), 1}, {"dsr-image_dimension.cal_units[0]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[0]), 1}, {"dsr-image_dimension.cal_units[1]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[1]), 1}, {"dsr-image_dimension.cal_units[2]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[2]), 1}, {"dsr-image_dimension.cal_units[3]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[3]), 1}, {"dsr-image_dimension.cal_units[4]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[4]), 1}, {"dsr-image_dimension.cal_units[5]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[5]), 1}, {"dsr-image_dimension.cal_units[6]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[6]), 1}, {"dsr-image_dimension.cal_units[7]", BYTE, G_STRUCT_OFFSET(struct dsr, dime.cal_units[7]), 1}, {"dsr-image_dimension.data_type", SHORT, G_STRUCT_OFFSET(struct dsr, dime.datatype), 2}, {"dsr-image_dimension.bitpix", SHORT, G_STRUCT_OFFSET(struct dsr, dime.bitpix), 2}, {"dsr-image_dimension.dim_un0", SHORT, G_STRUCT_OFFSET(struct dsr, dime.dim_un0), 2}, {"dsr-image_dimension.pixdim[0]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[0]), 4}, {"dsr-image_dimension.pixdim[1]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[1]), 4}, {"dsr-image_dimension.pixdim[2]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[2]), 4}, {"dsr-image_dimension.pixdim[3]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[3]), 4}, {"dsr-image_dimension.pixdim[4]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[4]), 4}, {"dsr-image_dimension.pixdim[5]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[5]), 4}, {"dsr-image_dimension.pixdim[6]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[6]), 4}, {"dsr-image_dimension.pixdim[7]", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.pixdim[7]), 4}, {"dsr-image_dimension.vox_offset", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.vox_offset), 4}, {"dsr-image_dimension.cal_max", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.cal_max), 4}, {"dsr-image_dimension.cal_min", FLOAT, G_STRUCT_OFFSET(struct dsr, dime.cal_min), 4}, {"dsr-image_dimension.compressed", INT, G_STRUCT_OFFSET(struct dsr, dime.compressed), 4}, {"dsr-image_dimension.verified", INT, G_STRUCT_OFFSET(struct dsr, dime.verified), 4}, {"dsr-image_dimension.glmax", INT, G_STRUCT_OFFSET(struct dsr, dime.glmax), 4}, {"dsr-image_dimension.glmin", INT, G_STRUCT_OFFSET(struct dsr, dime.glmin), 4},

    {"dsr-data_history.descrip", STRING, G_STRUCT_OFFSET(struct dsr, hist.descrip), 80}, {"dsr-data_history.aux_file", STRING, G_STRUCT_OFFSET(struct dsr, hist.aux_file), 24}, {"dsr-data_history.orient", BYTE, G_STRUCT_OFFSET(struct dsr, hist.orient), 1}, {"dsr-data_history.originator", STRING, G_STRUCT_OFFSET(struct dsr, hist.originator), 10}, {"dsr-data_history.generated", STRING, G_STRUCT_OFFSET(struct dsr, hist.generated), 10}, {"dsr-data_history.scannum", STRING, G_STRUCT_OFFSET(struct dsr, hist.scannum), 10}, {"dsr-data_history.patient_id", STRING, G_STRUCT_OFFSET(struct dsr, hist.patient_id), 10}, {"dsr-data_history.exp_date", STRING, G_STRUCT_OFFSET(struct dsr, hist.exp_date), 10}, {"dsr-data_history.exp_time", STRING, G_STRUCT_OFFSET(struct dsr, hist.exp_time), 10}, {"dsr-data_history.hist_un0", STRING, G_STRUCT_OFFSET(struct dsr, hist.hist_un0), 3}, {"dsr-data_history.views", INT, G_STRUCT_OFFSET(struct dsr, hist.views), 4}, {"dsr-data_history.vols_added", INT, G_STRUCT_OFFSET(struct dsr, hist.vols_added), 4}, {"dsr-data_history.start_field", INT, G_STRUCT_OFFSET(struct dsr, hist.start_field), 4}, {"dsr-data_history.field_skip", INT, G_STRUCT_OFFSET(struct dsr, hist.field_skip), 4}, {"dsr-data_history.omax", INT, G_STRUCT_OFFSET(struct dsr, hist.omax), 4}, {"dsr-data_history.omin", INT, G_STRUCT_OFFSET(struct dsr, hist.omin), 4}, {"dsr-data_history.smax", INT, G_STRUCT_OFFSET(struct dsr, hist.smax), 4}, {"dsr-data_history.smin", INT, G_STRUCT_OFFSET(struct dsr, hist.smin), 4}};

static void generate_filenames(const char *path, char *header, char *image) {
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];

    const char *olds[] = {".img", ".hdr"};

    im_filename_split(path, name, mode);

    im__change_suffix(name, header, FILENAME_MAX, ".hdr", olds, 2);
    im__change_suffix(name, image, FILENAME_MAX, ".img", olds, 2);
}

static char *getstr(int mx, const char *str) {
    static char buf[256];
    int i;

    assert(mx < 256);

    strncpy(buf, str, mx);
    buf[mx] = '\0';

    for (i = 0; i < mx && buf[i]; i++) {
        if (!isascii(buf[i]) || buf[i] < 32) {
            buf[i] = '@';
        }
    }

    return (buf);
}

#ifdef DEBUG
static void print_dsr(struct dsr *d) {
    int i;

    for (i = 0; i < IM_NUMBER(dsr_header); i++) {
        printf("%s = ", dsr_header[i].name);

        switch (dsr_header[i].type) {
        case BYTE:
            printf("%d\n", G_STRUCT_MEMBER(char, d, dsr_header[i].offset));
            break;

        case SHORT:
            printf("%d\n", G_STRUCT_MEMBER(short, d, dsr_header[i].offset));
            break;

        case INT:
            printf("%d\n", G_STRUCT_MEMBER(int, d, dsr_header[i].offset));
            break;

        case FLOAT:
            printf("%g\n", G_STRUCT_MEMBER(float, d, dsr_header[i].offset));
            break;

        case STRING:
            printf("\"%s\"\n", getstr(dsr_header[i].len, &G_STRUCT_MEMBER(char, d, dsr_header[i].offset)));
            break;

        default:
            assert(0);
        }
    }
}
#endif

static struct dsr *read_header(const char *header) {
    struct dsr *d;
    unsigned int len;

    if (!(d = (struct dsr *)im__file_read_name(header, NULL, &len))) {
        return (NULL);
    }
    if (len != sizeof(struct dsr)) {
        im_free(d);
        im_error("im_analyze2vips", "%s", _("header file size incorrect"));
        return (NULL);
    }

    g_assert(sizeof(struct dsr) == 348);

    if (!im_amiMSBfirst()) {
        int i;

        for (i = 0; i < IM_NUMBER(dsr_header); i++) {
            unsigned char *p;

            switch (dsr_header[i].type) {
            case SHORT:
                p = &G_STRUCT_MEMBER(unsigned char, d, dsr_header[i].offset);
                im__read_2byte(1, p, &p);
                break;

            case INT:
            case FLOAT:
                p = &G_STRUCT_MEMBER(unsigned char, d, dsr_header[i].offset);
                im__read_4byte(1, p, &p);
                break;

            case BYTE:
            case STRING:
                break;

            default:
                assert(0);
            }
        }
    }

    if ((int)len != d->hk.sizeof_hdr) {
        im_free(d);
        return (NULL);
    }

    return (d);
}

static int get_vips_properties(struct dsr *d, int *width, int *height, int *bands, int *fmt) {
    int i;

    if (d->dime.dim[0] < 2 || d->dime.dim[0] > 7) {
        im_error("im_analyze2vips", _("%d-dimensional images not supported"), d->dime.dim[0]);
        return (-1);
    }

    *width = d->dime.dim[1];
    *height = d->dime.dim[2];

    for (i = 3; i <= d->dime.dim[0]; i++) {
        *height *= d->dime.dim[i];
    }

    switch (d->dime.datatype) {
    case DT_UNSIGNED_CHAR:
        *bands = 1;
        *fmt = IM_BANDFMT_UCHAR;
        break;

    case DT_SIGNED_SHORT:
        *bands = 1;
        *fmt = IM_BANDFMT_SHORT;
        break;

    case DT_SIGNED_INT:
        *bands = 1;
        *fmt = IM_BANDFMT_INT;
        break;

    case DT_FLOAT:
        *bands = 1;
        *fmt = IM_BANDFMT_FLOAT;
        break;

    case DT_COMPLEX:
        *bands = 1;
        *fmt = IM_BANDFMT_COMPLEX;
        break;

    case DT_DOUBLE:
        *bands = 1;
        *fmt = IM_BANDFMT_DOUBLE;
        break;

    case DT_RGB:
        *bands = 3;
        *fmt = IM_BANDFMT_UCHAR;
        break;

    default:
        im_error("im_analyze2vips", _("datatype %d not supported"), d->dime.datatype);
        return (-1);
    }

#ifdef DEBUG
    printf("get_vips_properties: width = %d\n", *width);
    printf("get_vips_properties: height = %d\n", *height);
    printf("get_vips_properties: bands = %d\n", *bands);
    printf("get_vips_properties: fmt = %d\n", *fmt);
#endif

    return (0);
}

static void attach_meta(IMAGE *out, struct dsr *d) {
    int i;

    im_meta_set_blob(out, "dsr", (im_callback_fn)im_free, d, d->hk.sizeof_hdr);

    for (i = 0; i < IM_NUMBER(dsr_header); i++) {
        switch (dsr_header[i].type) {
        case BYTE:
            im_meta_set_int(out, dsr_header[i].name, G_STRUCT_MEMBER(char, d, dsr_header[i].offset));
            break;

        case SHORT:
            im_meta_set_int(out, dsr_header[i].name, G_STRUCT_MEMBER(short, d, dsr_header[i].offset));
            break;

        case INT:
            im_meta_set_int(out, dsr_header[i].name, G_STRUCT_MEMBER(int, d, dsr_header[i].offset));
            break;

        case FLOAT:
            im_meta_set_double(out, dsr_header[i].name, G_STRUCT_MEMBER(float, d, dsr_header[i].offset));
            break;

        case STRING:
            im_meta_set_string(out, dsr_header[i].name, getstr(dsr_header[i].len, &G_STRUCT_MEMBER(char, d, dsr_header[i].offset)));
            break;

        default:
            assert(0);
        }
    }
}

static int isanalyze(const char *filename) {
    char header[FILENAME_MAX];
    char image[FILENAME_MAX];
    struct dsr *d;
    int width, height;
    int bands;
    int fmt;

    generate_filenames(filename, header, image);
    if (!im_existsf("%s", header)) {
        return (0);
    }
    if (!(d = read_header(header))) {
        return (0);
    }

#ifdef DEBUG
    print_dsr(d);
#endif

    if (get_vips_properties(d, &width, &height, &bands, &fmt)) {
        im_free(d);
        return (0);
    }
    im_free(d);

    return (1);
}

static int analyze2vips_header(const char *filename, IMAGE *out) {
    char header[FILENAME_MAX];
    char image[FILENAME_MAX];
    struct dsr *d;
    int width, height;
    int bands;
    int fmt;

    generate_filenames(filename, header, image);
    if (!(d = read_header(header))) {
        return (-1);
    }

#ifdef DEBUG
    print_dsr(d);
#endif

    if (get_vips_properties(d, &width, &height, &bands, &fmt)) {
        im_free(d);
        return (-1);
    }

    im_initdesc(out, width, height, bands, im_bits_of_fmt(fmt), fmt, IM_CODING_NONE, bands == 1 ? IM_TYPE_B_W : IM_TYPE_sRGB, 1.0, 1.0, 0, 0);

    attach_meta(out, d);

    return (0);
}

int im_analyze2vips(const char *filename, IMAGE *out) {
    char header[FILENAME_MAX];
    char image[FILENAME_MAX];
    struct dsr *d;
    IMAGE *t[2];
    int width, height;
    int bands;
    int fmt;

    generate_filenames(filename, header, image);
    if (!(d = read_header(header))) {
        return (-1);
    }

#ifdef DEBUG
    print_dsr(d);
#endif

    if (get_vips_properties(d, &width, &height, &bands, &fmt) || im_open_local_array(out, t, 2, "im_analyze2vips", "p") || im_raw2vips(image, t[0], width, height, bands * im_bits_of_fmt(fmt) / 8, 0) || im_copy_morph(t[0], t[1], bands, fmt, IM_CODING_NONE) || im_copy_native(t[1], out, TRUE)) {
        im_free(d);
        return (-1);
    }

    attach_meta(out, d);

    return (0);
}

static const char *analyze_suffs[] = {".img", ".hdr", NULL};

static VipsFormatFlags analyze_flags(const char *filename) {
    return (VIPS_FORMAT_PARTIAL);
}

typedef VipsFormat VipsFormatAnalyze;
typedef VipsFormatClass VipsFormatAnalyzeClass;

static void vips_format_analyze_class_init(VipsFormatAnalyzeClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "analyze";
    object_class->description = _("Analyze 6.0");

    format_class->is_a = isanalyze;
    format_class->header = analyze2vips_header;
    format_class->load = im_analyze2vips;
    format_class->get_flags = analyze_flags;
    format_class->suffs = analyze_suffs;
}

static void vips_format_analyze_init(VipsFormatAnalyze *object) {
}

G_DEFINE_TYPE(VipsFormatAnalyze, vips_format_analyze, VIPS_TYPE_FORMAT);
