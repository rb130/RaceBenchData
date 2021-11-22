

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *index;
    IMAGE *value;
    IMAGE *out;

    REGION *vreg;

    int bands;
    int size;
    int mx;
    double *bins;
} Histogram;

static void hist_free(Histogram *hist) {
    IM_FREE(hist->bins);
    IM_FREEF(im_region_free, hist->vreg);
    IM_FREE(hist);
}

static Histogram *hist_build(IMAGE *index, IMAGE *value, IMAGE *out, int bands, int size) {
    Histogram *hist;

    if (!(hist = IM_NEW(NULL, Histogram))) {
        return (NULL);
    }

    hist->index = index;
    hist->value = value;
    hist->out = out;
    hist->vreg = NULL;
    hist->bands = bands;
    hist->size = size;
    hist->mx = 0;
    hist->bins = NULL;

    if (!(hist->bins = IM_ARRAY(NULL, bands * size, double)) || !(hist->vreg = im_region_create(value))) {
        hist_free(hist);
        return (NULL);
    }

    memset(hist->bins, 0, bands * size * sizeof(double));

    return (hist);
}

static void *hist_start(IMAGE *out, void *a, void *b) {
    Histogram *mhist = (Histogram *)a;

    return ((void *)hist_build(mhist->index, mhist->value, mhist->out, mhist->bands, mhist->size));
}

static int hist_stop(void *seq, void *a, void *b) {
    Histogram *shist = (Histogram *)seq;
    Histogram *mhist = (Histogram *)a;
    int i;

    g_assert(shist->bands == mhist->bands && shist->size == mhist->size);

    mhist->mx = IM_MAX(mhist->mx, shist->mx);
    for (i = 0; i < mhist->bands * mhist->size; i++) {
        mhist->bins[i] += shist->bins[i];
    }

    hist_free(shist);

    return (0);
}

#define ACCUMULATE_UCHAR(TYPE)                       \
    {                                                \
        int x, z;                                    \
        TYPE *tv = (TYPE *)v;                        \
                                                     \
        for (x = 0; x < width; x++) {                \
            double *bin = hist->bins + i[x] * bands; \
                                                     \
            for (z = 0; z < bands; z++)              \
                bin[z] += tv[z];                     \
                                                     \
            tv += bands;                             \
        }                                            \
    }

static int hist_scan_uchar(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *value = hist->value;
    int bands = value->Bands;
    int width = r->width;

    int y;

    if (im_prepare(hist->vreg, r)) {
        return (-1);
    }

    for (y = 0; y < r->height; y++) {
        PEL *i = (PEL *)IM_REGION_ADDR(reg, r->left, r->top + y);
        PEL *v = (PEL *)IM_REGION_ADDR(hist->vreg, r->left, r->top + y);

        switch (value->BandFmt) {
        case IM_BANDFMT_UCHAR:
            ACCUMULATE_UCHAR(unsigned char);
            break;
        case IM_BANDFMT_CHAR:
            ACCUMULATE_UCHAR(signed char);
            break;
        case IM_BANDFMT_USHORT:
            ACCUMULATE_UCHAR(unsigned short);
            break;
        case IM_BANDFMT_SHORT:
            ACCUMULATE_UCHAR(signed short);
            break;
        case IM_BANDFMT_UINT:
            ACCUMULATE_UCHAR(unsigned int);
            break;
        case IM_BANDFMT_INT:
            ACCUMULATE_UCHAR(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            ACCUMULATE_UCHAR(float);
            break;
        case IM_BANDFMT_DOUBLE:
            ACCUMULATE_UCHAR(double);
            break;

        default:
            g_assert(0);
        }
    }

    hist->mx = 255;

    return (0);
}

#define ACCUMULATE_USHORT(TYPE)                    \
    {                                              \
        int x, z;                                  \
        TYPE *tv = (TYPE *)v;                      \
                                                   \
        for (x = 0; x < width; x++) {              \
            int ix = i[x];                         \
            double *bin = hist->bins + ix * bands; \
                                                   \
            if (ix > mx)                           \
                mx = ix;                           \
                                                   \
            for (z = 0; z < bands; z++)            \
                bin[z] += tv[z];                   \
                                                   \
            tv += bands;                           \
        }                                          \
    }

static int hist_scan_ushort(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *value = hist->value;
    int bands = value->Bands;
    int width = r->width;

    int y, mx;

    if (im_prepare(hist->vreg, r)) {
        return (-1);
    }

    mx = hist->mx;
    for (y = 0; y < r->height; y++) {
        unsigned short *i = (unsigned short *)IM_REGION_ADDR(reg, r->left, r->top + y);
        PEL *v = (PEL *)IM_REGION_ADDR(hist->vreg, r->left, r->top + y);

        switch (value->BandFmt) {
        case IM_BANDFMT_UCHAR:
            ACCUMULATE_USHORT(unsigned char);
            break;
        case IM_BANDFMT_CHAR:
            ACCUMULATE_USHORT(signed char);
            break;
        case IM_BANDFMT_USHORT:
            ACCUMULATE_USHORT(unsigned short);
            break;
        case IM_BANDFMT_SHORT:
            ACCUMULATE_USHORT(signed short);
            break;
        case IM_BANDFMT_UINT:
            ACCUMULATE_USHORT(unsigned int);
            break;
        case IM_BANDFMT_INT:
            ACCUMULATE_USHORT(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            ACCUMULATE_USHORT(float);
            break;
        case IM_BANDFMT_DOUBLE:
            ACCUMULATE_USHORT(double);
            break;

        default:
            g_assert(0);
        }
    }

    hist->mx = mx;

    return (0);
}

static int hist_write(IMAGE *out, Histogram *hist) {
    if (im_cp_descv(out, hist->index, hist->value, NULL)) {
        return (-1);
    }
    im_initdesc(out, hist->mx + 1, 1, hist->value->Bands, IM_BBITS_DOUBLE, IM_BANDFMT_DOUBLE, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    if (im_writeline(0, out, (PEL *)hist->bins)) {
        return (-1);
    }

    return (0);
}

int im_hist_indexed(IMAGE *index, IMAGE *value, IMAGE *out) {
    int size;
    Histogram *mhist;
    im_generate_fn scanfn;

    if (im_pincheck(index) || im_pincheck(value) || im_outcheck(out) || im_check_uncoded("im_hist_indexed", index) || im_check_uncoded("im_hist_indexed", value) || im_check_noncomplex("im_hist_indexed", value) || im_check_size_same("im_hist_indexed", index, value) || im_check_u8or16("im_hist_indexed", index) || im_check_mono("im_hist_indexed", index)) {
        return (-1);
    }

    if (index->BandFmt == IM_BANDFMT_UCHAR) {
        size = 256;
        scanfn = hist_scan_uchar;
    } else {
        size = 65536;
        scanfn = hist_scan_ushort;
    }

    if (!(mhist = hist_build(index, value, out, value->Bands, size))) {
        return (-1);
    }

    if (vips_sink(index, hist_start, scanfn, hist_stop, mhist, NULL) || hist_write(out, mhist)) {
        hist_free(mhist);
        return (-1);
    }

    hist_free(mhist);

    return (0);
}
