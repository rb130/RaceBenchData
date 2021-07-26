
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
    IMAGE *in;
    IMAGE *out;
    int bins;

    unsigned int ***data;
} Histogram;

static Histogram *build_hist(IMAGE *in, IMAGE *out, int bins) {

    int ilimit = in->Bands > 2 ? bins : 1;
    int jlimit = in->Bands > 1 ? bins : 1;

    int i, j;
    Histogram *hist;

    if (!(hist = IM_NEW(out, Histogram))) {
        return (NULL);
    }

    hist->in = in;
    hist->out = out;
    hist->bins = bins;

    if (!(hist->data = IM_ARRAY(out, bins, unsigned int **))) {
        return (NULL);
    }
    memset(hist->data, 0, bins * sizeof(unsigned int **));

    for (i = 0; i < ilimit; i++) {
        if (!(hist->data[i] = IM_ARRAY(out, bins, unsigned int *))) {
            return (NULL);
        }
        memset(hist->data[i], 0, bins * sizeof(unsigned int *));
        for (j = 0; j < jlimit; j++) {
            if (!(hist->data[i][j] = IM_ARRAY(out, bins, unsigned int))) {
                return (NULL);
            }
            memset(hist->data[i][j], 0, bins * sizeof(unsigned int));
        }
    }

    return (hist);
}

static void *build_subhist(IMAGE *out, void *a, void *b) {
    Histogram *mhist = (Histogram *)a;

    return ((void *)build_hist(mhist->in, mhist->out, mhist->bins));
}

static int merge_subhist(void *seq, void *a, void *b) {
    Histogram *shist = (Histogram *)seq;
    Histogram *mhist = (Histogram *)a;
    int i, j, k;

    if (shist->in != mhist->in || shist->out != mhist->out) {
        error_exit("sanity failure in merge_subhist");
    }

    for (i = 0; i < mhist->bins; i++) {
        for (j = 0; j < mhist->bins; j++) {
            for (k = 0; k < mhist->bins; k++) {
                if (mhist->data[i] && mhist->data[i][j]) {
                    mhist->data[i][j][k] += shist->data[i][j][k];

                    shist->data[i][j][k] = 0;
                }
            }
        }
    }

    return (0);
}

#define LOOP(TYPE)                                      \
    {                                                   \
        TYPE *p = (TYPE *)line;                         \
                                                        \
        for (i = 0, x = 0; x < r->width; x++) {         \
            for (z = 0; z < nb; z++, i++)               \
                index[z] = p[i] / scale;                \
                                                        \
            hist->data[index[2]][index[1]][index[0]]++; \
        }                                               \
    }

static int find_hist(REGION *reg, void *seq, void *a, void *b) {
    Histogram *hist = (Histogram *)seq;
    Rect *r = &reg->valid;
    IMAGE *im = reg->im;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int nb = im->Bands;
    int max_val = im->BandFmt == IM_BANDFMT_UCHAR ? 256 : 65536;
    int scale = max_val / hist->bins;
    int x, y, z, i;
    int index[3];

    index[0] = index[1] = index[2] = 0;

    for (y = to; y < bo; y++) {
        char *line = IM_REGION_ADDR(reg, le, y);

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            LOOP(unsigned char);
            break;

        case IM_BANDFMT_USHORT:
            LOOP(unsigned char);
            break;

        default:
            error_exit("panic #34847563245");
        }
    }

    return (0);
}

int im_histnD(IMAGE *in, IMAGE *out, int bins) {
    int max_val;
    Histogram *mhist;
    int x, y, z, i;
    unsigned int *obuffer;

    if (im_check_uncoded("im_histnD", in) || im_check_u8or16("im_histnD", in) || im_pincheck(in) || im_outcheck(out)) {
        return (-1);
    }

    max_val = in->BandFmt == IM_BANDFMT_UCHAR ? 256 : 65536;
    if (bins < 1 || bins > max_val) {
        im_error("im_histnD", _(" bins out of range [1,%d]"), max_val);
        return (-1);
    }

    if (!(mhist = build_hist(in, out, bins))) {
        return (-1);
    }

    if (vips_sink(in, build_subhist, find_hist, merge_subhist, mhist, NULL)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    im_initdesc(out, bins, in->Bands > 1 ? bins : 1, in->Bands > 2 ? bins : 1, IM_BBITS_INT, IM_BANDFMT_UINT, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(obuffer = IM_ARRAY(out, IM_IMAGE_N_ELEMENTS(out), unsigned int))) {
        return (-1);
    }

    for (y = 0; y < out->Ysize; y++) {
        for (i = 0, x = 0; x < out->Xsize; x++) {
            for (z = 0; z < out->Bands; z++, i++) {
                obuffer[i] = mhist->data[z][y][x];
            }
        }

        if (im_writeline(y, out, (PEL *)obuffer)) {
            return (-1);
        }
    }

    return (0);
}
