
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

    int size;
    int scale;
} Conv;

static int conv_destroy(Conv *conv) {
    IM_FREEF(im_free_dmask, conv->mask);

    return (0);
}

static Conv *conv_new(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    Conv *conv = IM_NEW(out, Conv);

    if (!conv) {
        return (NULL);
    }

    conv->in = in;
    conv->out = out;
    conv->mask = NULL;
    conv->size = mask->xsize * mask->ysize;
    conv->scale = mask->scale * mask->scale;

    if (im_add_close_callback(out, (im_callback_fn)conv_destroy, conv, NULL) || !(conv->mask = im_dup_dmask(mask, "conv_mask"))) {
        return (NULL);
    }

    return (conv);
}

typedef struct {
    Conv *conv;
    REGION *ir;

    PEL *sum;
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
    seq->sum = NULL;

    seq->ir = im_region_create(in);
    if (vips_bandfmt_isint(conv->out->BandFmt)) {
        seq->sum = (PEL *)IM_ARRAY(out, IM_IMAGE_N_ELEMENTS(in), int);
    } else {
        seq->sum = (PEL *)IM_ARRAY(out, IM_IMAGE_N_ELEMENTS(in), double);
    }
    if (!seq->ir || !seq->sum) {
        conv_stop(seq, in, conv);
        return (NULL);
    }

    return ((void *)seq);
}

#define VERTICAL_CONV                \
    {                                \
        z -= 1;                      \
        li -= lskip;                 \
        sum += coeff[z] * vfrom[li]; \
    }
#define HORIZONTAL_CONV              \
    {                                \
        z -= 1;                      \
        li -= bands;                 \
        sum += coeff[z] * hfrom[li]; \
    }

#define CONV_FLOAT(ITYPE, OTYPE)                      \
    {                                                 \
        ITYPE *vfrom;                                 \
        double *vto;                                  \
        double *hfrom;                                \
        OTYPE *hto;                                   \
                                                      \
        vfrom = (ITYPE *)IM_REGION_ADDR(ir, le, y);   \
        vto = (double *)seq->sum;                     \
        for (x = 0; x < isz; x++) {                   \
            double sum;                               \
                                                      \
            z = conv->size;                           \
            li = lskip * z;                           \
            sum = 0;                                  \
                                                      \
            IM_UNROLL(z, VERTICAL_CONV);              \
                                                      \
            vto[x] = sum;                             \
            vfrom += 1;                               \
        }                                             \
                                                      \
        hfrom = (double *)seq->sum;                   \
        hto = (OTYPE *)IM_REGION_ADDR(or, le, y);     \
        for (x = 0; x < osz; x++) {                   \
            double sum;                               \
                                                      \
            z = conv->size;                           \
            li = bands * z;                           \
            sum = 0;                                  \
                                                      \
            IM_UNROLL(z, HORIZONTAL_CONV);            \
                                                      \
            sum = (sum / conv->scale) + mask->offset; \
                                                      \
            hto[x] = sum;                             \
            hfrom += 1;                               \
        }                                             \
    }

static int conv_gen(REGION * or, void *vseq, void *a, void *b) {
    ConvSequence *seq = (ConvSequence *)vseq;
    IMAGE *in = (IMAGE *)a;
    Conv *conv = (Conv *)b;
    REGION *ir = seq->ir;
    DOUBLEMASK *mask = conv->mask;
    double *coeff = conv->mask->coeff;
    int bands = in->Bands;

    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int osz = IM_REGION_N_ELEMENTS(or);

    Rect s;
    int lskip;
    int isz;
    int x, y, z, li;

    s = *r;
    s.width += conv->size - 1;
    s.height += conv->size - 1;
    if (im_prepare(ir, &s)) {
        return (-1);
    }
    lskip = IM_REGION_LSKIP(ir) / IM_IMAGE_SIZEOF_ELEMENT(in);
    isz = IM_REGION_N_ELEMENTS(ir);

    for (y = to; y < bo; y++) {
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

int im_convsep_f_raw(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    Conv *conv;

    if (im_piocheck(in, out) || im_check_uncoded("im_convsep_f", in) || im_check_noncomplex("im_convsep_f", in) || im_check_dmask("im_convsep_f", mask)) {
        return (-1);
    }
    if (mask->xsize != 1 && mask->ysize != 1) {
        im_error("im_convsep_f", "%s", _("expect 1xN or Nx1 input mask"));
        return (-1);
    }
    if (mask->scale == 0) {
        im_error("im_convsep_f", "%s", "mask scale must be non-zero");
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
    out->Xsize -= conv->size - 1;
    out->Ysize -= conv->size - 1;
    if (out->Xsize <= 0 || out->Ysize <= 0) {
        im_error("im_convsep_f", "%s", _("image too small for mask"));
        return (-1);
    }

    if (im_demand_hint(out, IM_SMALLTILE, in, NULL) || im_generate(out, conv_start, conv_gen, conv_stop, in, conv)) {
        return (-1);
    }

    out->Xoffset = -conv->size / 2;
    out->Yoffset = -conv->size / 2;

    return (0);
}

int im_convsep_f(IMAGE *in, IMAGE *out, DOUBLEMASK *mask) {
    IMAGE *t1 = im_open_local(out, "im_convsep_f intermediate", "p");
    int size = mask->xsize * mask->ysize;

    if (!t1 || im_embed(in, t1, 1, size / 2, size / 2, in->Xsize + size - 1, in->Ysize + size - 1) || im_convsep_f_raw(t1, out, mask)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
