

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
    int bands;
    int which;
    int size;
    int mx;
    unsigned int **bins;
} Histogram;

static Histogram *build_hist(IMAGE *out, int bands, int which, int size) {
    int i;
    Histogram *hist = IM_NEW(out, Histogram);

    if (!hist || !(hist->bins = IM_ARRAY(out, bands, unsigned int *))) {
        return (NULL);
    }
    for (i = 0; i < bands; i++) {
        if (!(hist->bins[i] = IM_ARRAY(out, size, unsigned int))) {
            return (NULL);
        }
        memset(hist->bins[i], 0, size * sizeof(unsigned int));
    }

    hist->bands = bands;
    hist->which = which;
    hist->size = size;
    hist->mx = 0;

    return (hist);
}

static void *build_subhist(IMAGE *out, void *a, void *b) {
    Histogram *mhist = (Histogram *)a;

    return ((void *)build_hist(out, mhist->bands, mhist->which, mhist->size));
}

static int merge_subhist(void *seq, void *a, void *b) {
    Histogram *shist = (Histogram *)seq;
    Histogram *mhist = (Histogram *)a;
    int i, j;

    g_assert(shist->bands == mhist->bands && shist->size == mhist->size);

    mhist->mx = IM_MAX(mhist->mx, shist->mx);
    for (i = 0; i < mhist->bands; i++) {
        for (j = 0; j < mhist->size; j++) {
            mhist->bins[i][j] += shist->bins[i][j];
        }
    }

    shist->mx = 0;
    for (i = 0; i < shist->bands; i++) {
        shist->bins[i] = NULL;
    }

    return (0);
}

static int find_uchar_hist(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *im = reg->im;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int nb = im->Bands;
    int x, y, z;

    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(reg, le, y);
        int i;

        for (i = 0, x = 0; x < r->width; x++) {
            for (z = 0; z < nb; z++, i++) {
                hist->bins[z][p[i]]++;
            }
        }
    }

    hist->mx = 255;

    return (0);
}

static int find_uchar_hist_extract(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *im = reg->im;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    unsigned int *bins = hist->bins[0];
    int nb = im->Bands;
    int max = r->width * nb;
    int x, y;

    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(reg, le, y);

        for (x = hist->which; x < max; x += nb) {
            bins[p[x]]++;
        }
    }

    hist->mx = 255;

    return (0);
}

static int find_ushort_hist(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *im = reg->im;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int mx = hist->mx;
    int nb = im->Bands;
    int x, y, z;

    for (y = to; y < bo; y++) {
        unsigned short *p = (unsigned short *)IM_REGION_ADDR(reg, le, y);
        int i;

        for (i = 0, x = 0; x < r->width; x++) {
            for (z = 0; z < nb; z++, i++) {
                int v = p[i];

                if (v > mx) {
                    mx = v;
                }

                hist->bins[z][v]++;
            }
        }
    }

    hist->mx = mx;

    return (0);
}

static int find_ushort_hist_extract(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *im = reg->im;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int mx = hist->mx;
    unsigned int *bins = hist->bins[0];
    int nb = im->Bands;
    int max = nb * r->width;
    int x, y;

    for (y = to; y < bo; y++) {
        unsigned short *p = (unsigned short *)IM_REGION_ADDR(reg, le, y) + hist->which;

        for (x = hist->which; x < max; x += nb) {
            int v = p[x];

            if (v > mx) {
                mx = v;
            }

            bins[v]++;
        }
    }

    hist->mx = mx;

    return (0);
}

int im_histgr(IMAGE *in, IMAGE *out, int bandno) {
    int size;
    int bands;
    Histogram *mhist;
    im_generate_fn scanfn;
    int i, j;
    unsigned int *obuffer, *q;

    if (im_check_uncoded("im_histgr", in) || im_check_u8or16("im_histgr", in) || im_check_bandno("im_histgr", in, bandno) || im_pincheck(in) || im_outcheck(out)) {
        return (-1);
    }

    size = in->BandFmt == IM_BANDFMT_UCHAR ? 256 : 65536;

    if (bandno == -1) {
        bands = in->Bands;
    } else {
        bands = 1;
    }

    if (!(mhist = build_hist(out, bands, bandno, size))) {
        return (-1);
    }

    if (in->BandFmt == IM_BANDFMT_UCHAR && bandno == -1) {
        scanfn = find_uchar_hist;
    } else if (in->BandFmt == IM_BANDFMT_UCHAR) {
        scanfn = find_uchar_hist_extract;
    } else if (in->BandFmt == IM_BANDFMT_USHORT && bandno == -1) {
        scanfn = find_ushort_hist;
    } else {
        scanfn = find_ushort_hist_extract;
    }

    if (vips_sink(in, build_subhist, scanfn, merge_subhist, mhist, NULL)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    im_initdesc(out, mhist->mx + 1, 1, bands, IM_BBITS_INT, IM_BANDFMT_UINT, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(obuffer = IM_ARRAY(out, IM_IMAGE_N_ELEMENTS(out), unsigned int))) {
        return (-1);
    }
    for (q = obuffer, j = 0; j < out->Xsize; j++) {
        for (i = 0; i < out->Bands; i++) {
            *q++ = mhist->bins[i][j];
        }
    }

    if (im_writeline(0, out, (PEL *)obuffer)) {
        return (-1);
    }

    return (0);
}
