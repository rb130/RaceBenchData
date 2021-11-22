

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

GSList *im__open_images = NULL;

static const char *im_Type[] = {"IM_TYPE_MULTIBAND", "IM_TYPE_B_W", "LUMINACE", "XRAY", "IR", "YUV", "RED_ONLY", "GREEN_ONLY", "BLUE_ONLY", "POWER_SPECTRUM", "IM_TYPE_HISTOGRAM", "LUT", "IM_TYPE_XYZ", "IM_TYPE_LAB", "CMC", "IM_TYPE_CMYK", "IM_TYPE_LABQ", "IM_TYPE_RGB", "IM_TYPE_UCS", "IM_TYPE_LCH", "IM_TYPE_LABS", "<unknown>", "IM_TYPE_sRGB", "IM_TYPE_YXY", "IM_TYPE_FOURIER", "IM_TYPE_RGB16", "IM_TYPE_GREY16"};

static const char *im_BandFmt[] = {"IM_BANDFMT_UCHAR", "IM_BANDFMT_CHAR", "IM_BANDFMT_USHORT", "IM_BANDFMT_SHORT", "IM_BANDFMT_UINT", "IM_BANDFMT_INT", "IM_BANDFMT_FLOAT", "IM_BANDFMT_COMPLEX", "IM_BANDFMT_DOUBLE", "IM_BANDFMT_DPCOMPLEX"};

static const char *im_Coding[] = {"IM_CODING_NONE", "COLQUANT8", "IM_CODING_LABQ", "IM_CODING_LABQ_COMPRESSED", "RGB_COMPRESSED", "LUM_COMPRESSED", "IM_CODING_RAD"};

static const char *im_Compression[] = {"NO_COMPRESSION", "TCSF_COMPRESSION", "JPEG_COMPRESSION"};

static const char *im_dtype[] = {"IM_NONE", "IM_SETBUF", "IM_SETBUF_FOREIGN", "IM_OPENIN", "IM_MMAPIN", "IM_MMAPINRW", "IM_OPENOUT", "IM_PARTIAL"};

static const char *im_dhint[] = {"IM_SMALLTILE", "IM_FATSTRIP", "IM_THINSTRIP", "IM_ANY"};

typedef struct _EnumTable {
    const char *error;
    const char **names;
    int nnames;
} EnumTable;

static EnumTable enumType = {N_("<bad Type>"), im_Type, IM_NUMBER(im_Type)};

static EnumTable enumBandFmt = {N_("<bad BandFmt>"), im_BandFmt, IM_NUMBER(im_BandFmt)};

static EnumTable enumCoding = {N_("<bad Coding>"), im_Coding, IM_NUMBER(im_Coding)};

static EnumTable enumCompression = {N_("<bad Compression>"), im_Compression, IM_NUMBER(im_Compression)};

static EnumTable enumdtype = {N_("<bad dtype>"), im_dtype, IM_NUMBER(im_dtype)};

static EnumTable enumdhint = {N_("<bad dhint>"), im_dhint, IM_NUMBER(im_dhint)};

static const char *enum2char(EnumTable *etable, int n) {
    if (n < 0 || n > etable->nnames) {
        return (_(etable->error));
    } else {
        return (etable->names[n]);
    }
}

static int char2enum(EnumTable *etable, const char *name) {
    int i;

    for (i = 0; i < etable->nnames; i++) {
        if (g_ascii_strcasecmp(etable->names[i], name) == 0) {
            return (i);
        }
    }

    im_error("char2enum", "%s", _(etable->error));

    return (-1);
}

const char *im_Type2char(VipsType type) {
    return (enum2char(&enumType, type));
}
const char *im_BandFmt2char(VipsBandFmt fmt) {
    return (enum2char(&enumBandFmt, fmt));
}
const char *im_Coding2char(VipsCoding coding) {
    return (enum2char(&enumCoding, coding));
}
const char *im_Compression2char(int n) {
    return (enum2char(&enumCompression, n));
}
const char *im_dtype2char(im_desc_type n) {
    return (enum2char(&enumdtype, n));
}
const char *im_dhint2char(VipsDemandStyle style) {
    return (enum2char(&enumdhint, style));
}

int im_char2Type(const char *str) {
    return (char2enum(&enumType, str));
}
int im_char2BandFmt(const char *str) {
    return (char2enum(&enumBandFmt, str));
}
int im_char2Coding(const char *str) {
    return (char2enum(&enumCoding, str));
}
int im_char2Compression(const char *str) {
    return (char2enum(&enumCompression, str));
}
im_desc_type im_char2dtype(const char *str) {
    return (char2enum(&enumdtype, str));
}
im_demand_type im_char2dhint(const char *str) {
    return (char2enum(&enumdhint, str));
}

static void *print_one_line_region(REGION *r, int *n2, gint64 *total) {
    if (r->type == IM_REGION_BUFFER && r->buffer) {
        printf("\t*** %d) %zd malloced bytes\n", *n2, r->buffer->bsize);
        *total += r->buffer->bsize;
    }

    *n2 += 1;

    return (NULL);
}

static void *print_one_line(IMAGE *im, int *n, gint64 *total) {
    printf("%2d) %p, %s, %s: %dx%d, %d bands, %s\n", *n, im, im_dtype2char(im->dtype), im->filename, im->Xsize, im->Ysize, im->Bands, im_BandFmt2char(im->BandFmt));
    *n += 1;

    if (im->dtype == IM_SETBUF && im->data) {
        gint64 size = (gint64)IM_IMAGE_SIZEOF_LINE(im) * im->Ysize;

        printf("\t*** %" G_GINT64_FORMAT " malloced bytes\n", size);
        *total += size;
    }

    if (im->regions) {
        int n2;
        gint64 total2;

        printf("\t%d regions\n", g_slist_length(im->regions));

        n2 = 0;
        total2 = 0;
        (void)im_slist_map2(im->regions, (VSListMap2Fn)print_one_line_region, &n2, &total2);
        if (total2) {
            printf("\t*** using total of %" G_GINT64_FORMAT " bytes\n", total2);
        }
        *total += total2;
    }

    return (NULL);
}

static void *add_virtual(IMAGE *im, gint64 *total, void *dummy) {
    *total += im__image_pixel_length(im);

    return (NULL);
}

void im__print_all(void) {
    if (im__open_images) {
        int n = 0;
        gint64 total = 0;

        total = 0;
        printf("%d images\n", g_slist_length(im__open_images));
        (void)im_slist_map2(im__open_images, (VSListMap2Fn)print_one_line, &n, &total);
        if (total) {
            printf("\n\t*** all-image total = %" G_GINT64_FORMAT " real bytes\n", total);
        }

        total = 0;
        (void)im_slist_map2(im__open_images, (VSListMap2Fn)add_virtual, &total, NULL);
        if (total) {
            printf("\n\t*** virtual total = %" G_GINT64_FORMAT " bytes\n", total);
        }
    }

    im__print_renders();
}

static void *print_field_fn(IMAGE *im, const char *field, GValue *value) {
    const char *extra;
    char *str_value;

    str_value = g_strdup_value_contents(value);
    printf("%s: %s", field, str_value);
    g_free(str_value);

#define NN(X) ((X) ? (X) : "(null)")

    extra = NULL;
    if (strcmp(field, "Coding") == 0) {
        extra = NN(im_Coding2char(g_value_get_int(value)));
    } else if (strcmp(field, "BandFmt") == 0) {
        extra = NN(im_BandFmt2char(g_value_get_int(value)));
    } else if (strcmp(field, "Type") == 0) {
        extra = NN(im_Type2char(g_value_get_int(value)));
    } else if (strcmp(field, "Compression") == 0) {
        extra = NN(im_Compression2char(g_value_get_int(value)));
    }

    if (extra) {
        printf(" - %s", extra);
    }

    printf("\n");

    return (NULL);
}

static void *print_region(REGION *reg, void *a, void *b) {
    printf("Region defined for area at %dx%d size %dx%d\n", reg->valid.left, reg->valid.top, reg->valid.width, reg->valid.height);
    if (reg->seq) {
        printf("sequence in progress on region\n");
    }
    if (reg->buffer) {
        printf("local memory allocated\n");
    }

    return (NULL);
}

void im_printdesc(IMAGE *image) {
    if (!image) {
        printf("NULL descriptor\n");
        return;
    }

    printf("IMAGE* %p\n", image);

    if (im_isMSBfirst(image)) {
        printf("SPARC (MSB first) ");
    } else {
        printf("Intel (LSB first) ");
    }
    printf("byte order image, on a ");
    if (im_amiMSBfirst()) {
        printf("SPARC (MSB first) ");
    } else {
        printf("Intel (LSB first) ");
    }
    printf("byte order machine\n");

    (void)im_header_map(image, (im_header_map_fn)print_field_fn, NULL);

    printf("Hist: %s", im_history_get(image));

    if (image->generate) {
        printf("generate function attached\n");
    }
    if (image->preclosefns) {
        printf("preclose callbacks attached\n");
    }
    if (image->closefns) {
        printf("close callbacks attached\n");
    }
    if (image->postclosefns) {
        printf("postclose callbacks attached\n");
    }
    if (image->writtenfns) {
        printf("written callbacks attached\n");
    }
    if (image->evalfns) {
        printf("eval callbacks attached\n");
    }
    if (image->evalendfns) {
        printf("evalend callbacks attached\n");
    }
    if (image->evalstartfns) {
        printf("evalstart callbacks attached\n");
    }
    if (image->invalidatefns) {
        printf("invalidate callbacks attached\n");
    }
    if (image->regions) {
        printf("%d regions present\n", g_slist_length(image->regions));
        im_slist_map2(image->regions, (VSListMap2Fn)print_region, NULL, NULL);
    }
    if (image->kill) {
        printf("kill flag set\n");
    }
    if (image->closing) {
        printf("closing flag set\n");
    }
    if (image->close_pending) {
        printf("close_pending flag set\n");
    }

#ifdef DEBUG

    printf("dhint: %s\n", im_dhint2char(image->dhint));
    printf("dtype: %s\n", im_dtype2char(image->dtype));
#endif
}

void im__print_one(int n) {
    IMAGE *im = g_slist_nth_data(im__open_images, n);

    if (!im) {
        printf("bad index: %d\n", n);
        return;
    }

    im_printdesc(im);
}

static void *sanity_upstream(IMAGE *im_up, IMAGE *im_down) {
    if (!g_slist_find(im_up->downstream, im_down) || !g_slist_find(im_down->upstream, im_up)) {
        return (im_up);
    }

    return (NULL);
}

static void *sanity_downstream(IMAGE *im_down, IMAGE *im_up) {
    return (sanity_upstream(im_up, im_down));
}

static const char *image_sanity(IMAGE *im) {
    if (!im) {
        return ("NULL descriptor");
    }
    if (!im->filename) {
        return ("NULL filename");
    }

    g_mutex_lock(im__global_lock);
    if (!g_slist_find(im__open_images, im)) {
        g_mutex_unlock(im__global_lock);
        return ("not on open image list");
    }
    g_mutex_unlock(im__global_lock);

    if (im->Xsize != -1 || im->Ysize != -1 || im->Bands != -1 || im->BandFmt != -1) {
        if (im->Xsize <= 0 || im->Ysize <= 0 || im->Bands <= 0) {
            return ("bad dimensions");
        }
        if (im->BandFmt < -1 || im->BandFmt > IM_BANDFMT_DPCOMPLEX || (im->Coding != -1 && im->Coding != IM_CODING_NONE && im->Coding != IM_CODING_LABQ && im->Coding != IM_CODING_RAD) || im->Type > IM_TYPE_GREY16) {
            return ("bad enum value");
        }
        if (im->dtype > IM_PARTIAL || im->dhint > IM_ANY) {
            return ("bad enum value");
        }
        if (im->Xres < 0 || im->Xres < 0) {
            return ("bad resolution");
        }
    }

    if (im_slist_map2(im->upstream, (VSListMap2Fn)sanity_upstream, im, NULL)) {
        return ("upstream broken");
    }
    if (im_slist_map2(im->downstream, (VSListMap2Fn)sanity_downstream, im, NULL)) {
        return ("downstream broken");
    }

    return (NULL);
}

int im_image_sanity(IMAGE *im) {
    const char *msg;

    if ((msg = image_sanity(im))) {
        im_warn("im_image_sanity", "%p", im);
        im_warn("im_image_sanity", "\"%s\" %s", im ? (im->filename ? im->filename : "") : "", msg);
        im_printdesc(im);

        return (-1);
    }

    return (0);
}

void im_image_sanity_all(void) {
    g_assert(im_slist_map2(im__open_images, (VSListMap2Fn)im_image_sanity, NULL, NULL) == 0);
}
