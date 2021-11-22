

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *in;
    IMAGE *out;
    DOUBLEMASK *mask;

    int nnz;
    double *coeff;
    int *coeff_pos;
} Conv;

static int conv_close(Conv *conv) {
    IM_FREEF(im_free_dmask, conv->mask);

    return (0);
}

static Conv *conv_new(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    Conv *conv = IM_NEW(out, Conv);
    const int ne = mask->xsize * mask->ysize;
    int i;

    if (!conv) {
        return (NULL);
    }

    conv->in = in;
    conv->out = out;
    conv->mask = NULL;
    conv->nnz = 0;
    conv->coeff = NULL;

    if (im_add_close_callback(out, (im_callback_fn)conv_close, conv, NULL) || !(conv->coeff = IM_ARRAY(out, ne, double)) || !(conv->coeff_pos = IM_ARRAY(out, ne, int)) || !(conv->mask = im_dup_dmask(mask, "conv_mask"))) {
        return (NULL);
    }

    for (i = 0; i < ne; i++) {
        if (mask->coeff[i]) {
            conv->coeff[conv->nnz] = mask->coeff[i];
            conv->coeff_pos[conv->nnz] = i;
            conv->nnz += 1;
        }
    }

    if (conv->nnz == 0) {
        conv->coeff[0] = mask->coeff[0];
        conv->coeff_pos[0] = 0;
        conv->nnz = 1;
    }

    return (conv);
}

typedef struct {
    Conv *conv;
    REGION *ir;

    int *offsets;
    PEL **pts;

    int last_bpl;
} ConvSequence;

static int conv_stop(void *vseq, void *a, void *b) {
    ConvSequence *seq = (ConvSequence *)vseq;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *conv_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    Conv *conv = (Conv *)b;
    ConvSequence *seq;

    if (!(seq = IM_NEW(out, ConvSequence))) {
        return (NULL);
    }

    seq->conv = conv;
    seq->ir = NULL;
    seq->pts = NULL;
    seq->last_bpl = -1;

    seq->ir = im_region_create(in);
    seq->offsets = IM_ARRAY(out, conv->nnz, int);
    seq->pts = IM_ARRAY(out, conv->nnz, PEL *);
    if (!seq->ir || !seq->offsets || !seq->pts) {
        conv_stop(seq, in, conv);
        return (NULL);
    }

    return ((void *)seq);
}

#define INNER                  \
    {                          \
        sum += t[i] * p[i][x]; \
        i += 1;                \
    }

#define CONV_FLOAT(ITYPE, OTYPE)                                \
    {                                                           \
        ITYPE **restrict p = (ITYPE **)seq->pts;                \
        OTYPE *restrict q = (OTYPE *)IM_REGION_ADDR(or, le, y); \
                                                                \
        for (x = 0; x < sz; x++) {                              \
            double sum;                                         \
            int i;                                              \
                                                                \
            sum = 0;                                            \
            i = 0;                                              \
            IM_UNROLL(conv->nnz, INNER);                        \
                                                                \
            sum = (sum / mask->scale) + mask->offset;           \
                                                                \
            q[x] = sum;                                         \
        }                                                       \
    }

static int conv_gen(REGION * or, void *vseq, void *a, void *b) {
    ConvSequence *seq = (ConvSequence *)vseq;
    IMAGE *in = (IMAGE *)a;
    Conv *conv = (Conv *)b;
    REGION *ir = seq->ir;
    DOUBLEMASK *mask = conv->mask;
    double *restrict t = conv->coeff;

    Rect *r = & or->valid;
    Rect s;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int sz = IM_REGION_N_ELEMENTS(or);

    int x, y, z, i;

    s = *r;
    s.width += mask->xsize - 1;
    s.height += mask->ysize - 1;
    if (im_prepare(ir, &s)) {
        return (-1);
    }

    if (seq->last_bpl != IM_REGION_LSKIP(ir)) {
        seq->last_bpl = IM_REGION_LSKIP(ir);

        for (i = 0; i < conv->nnz; i++) {
            z = conv->coeff_pos[i];
            x = z % conv->mask->xsize;
            y = z / conv->mask->xsize;

            seq->offsets[i] = IM_REGION_ADDR(ir, x + le, y + to) - IM_REGION_ADDR(ir, le, to);
        }
    }

    for (y = to; y < bo; y++) {

        for (z = 0; z < conv->nnz; z++) {
            seq->pts[z] = seq->offsets[z] + (PEL *)IM_REGION_ADDR(ir, le, y);
        }

        switch (in->BandFmt) {
        case IM_BANDFMT_UCHAR:
            CONV_FLOAT(unsigned char, float);
            break;
        case IM_BANDFMT_CHAR:
            CONV_FLOAT(signed char, float);
            break;
        case IM_BANDFMT_USHORT:
            CONV_FLOAT(unsigned short, float);
            break;
        case IM_BANDFMT_SHORT:
            CONV_FLOAT(signed short, float);
            break;
        case IM_BANDFMT_UINT:
            CONV_FLOAT(unsigned int, float);
            break;
        case IM_BANDFMT_INT:
            CONV_FLOAT(signed int, float);
            break;
        case IM_BANDFMT_FLOAT:
            CONV_FLOAT(float, float);
            break;
        case IM_BANDFMT_DOUBLE:
            CONV_FLOAT(double, double);
            break;

        default:
            assert(0);
        }
    }

    return (0);
}

int im_conv_f_raw(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    Conv *conv;

    if (im_piocheck(in, out) || im_check_uncoded("im_conv", in) || im_check_noncomplex("im_conv", in) || im_check_dmask("im_conv", mask)) {
        return (-1);
    }
    if (mask->scale == 0) {
        im_error("im_conv_f", "%s", "mask scale must be non-zero");
        return (-1);
    }
    if (!(conv = conv_new(in, out, mask))) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (vips_bandfmt_isint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_FLOAT;
    }
    out->Xsize -= mask->xsize - 1;
    out->Ysize -= mask->ysize - 1;
    if (out->Xsize <= 0 || out->Ysize <= 0) {
        im_error("im_conv_f", "%s", _("image too small for mask"));
        return (-1);
    }

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, conv_start, conv_gen, conv_stop, in, conv)) {
        return (-1);
    }

    out->Xoffset = -mask->xsize / 2;
    out->Yoffset = -mask->ysize / 2;

    return (0);
}

int im_conv_f(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    IMAGE *t1 = im_open_local(out, "im_conv_f intermediate", "p");

    if (!t1 || im_embed(in, t1, 1, mask->xsize / 2, mask->ysize / 2, in->Xsize + mask->xsize - 1, in->Ysize + mask->ysize - 1) || im_conv_f_raw(t1, out, mask)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
