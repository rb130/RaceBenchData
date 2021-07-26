
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    double xshrink;
    double yshrink;
    int mw;
    int mh;
    int np;
} ShrinkInfo;

typedef struct {
    REGION *ir;
    int *off;
} SeqInfo;

static int shrink_stop(void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *shrink_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    ShrinkInfo *st = (ShrinkInfo *)b;
    SeqInfo *seq;

    if (!(seq = IM_NEW(out, SeqInfo))) {
        return (NULL);
    }

    seq->ir = NULL;
    seq->off = NULL;
    seq->ir = im_region_create(in);
    seq->off = IM_ARRAY(out, st->np, int);
    if (!seq->off || !seq->ir) {
        shrink_stop(seq, in, st);
        return (NULL);
    }

    return ((void *)seq);
}

#define ishrink(TYPE)                                     \
    for (y = to; y < bo; y++) {                           \
        TYPE *q = (TYPE *)IM_REGION_ADDR(or, le, y);      \
                                                          \
        for (x = le; x < ri; x++) {                       \
            int ix = x * st->xshrink;                     \
            int iy = y * st->yshrink;                     \
            TYPE *p = (TYPE *)IM_REGION_ADDR(ir, ix, iy); \
                                                          \
            for (k = 0; k < ir->im->Bands; k++) {         \
                int sum = 0;                              \
                int *t = seq->off;                        \
                                                          \
                for (z = 0; z < st->np; z++)              \
                    sum += p[*t++];                       \
                                                          \
                *q++ = sum / st->np;                      \
                p++;                                      \
            }                                             \
        }                                                 \
    }

#define fshrink(TYPE)                                     \
    for (y = to; y < bo; y++) {                           \
        TYPE *q = (TYPE *)IM_REGION_ADDR(or, le, y);      \
                                                          \
        for (x = le; x < ri; x++) {                       \
            int ix = x * st->xshrink;                     \
            int iy = y * st->yshrink;                     \
            TYPE *p = (TYPE *)IM_REGION_ADDR(ir, ix, iy); \
                                                          \
            for (k = 0; k < ir->im->Bands; k++) {         \
                double sum = 0;                           \
                int *t = seq->off;                        \
                                                          \
                for (z = 0; z < st->np; z++)              \
                    sum += p[*t++];                       \
                                                          \
                *q++ = sum / st->np;                      \
                p++;                                      \
            }                                             \
        }                                                 \
    }

static int shrink_gen(REGION * or, void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;
    ShrinkInfo *st = (ShrinkInfo *)b;
    REGION *ir = seq->ir;
    Rect *r = & or->valid;
    Rect s;
    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int x, y, z, k;

    s.left = r->left * st->xshrink;
    s.top = r->top * st->yshrink;
    s.width = ceil(IM_RECT_RIGHT(r) * st->xshrink) - s.left;
    s.height = ceil(IM_RECT_BOTTOM(r) * st->yshrink) - s.top;
    if (im_prepare(ir, &s)) {
        return (-1);
    }

    for (z = 0, y = 0; y < st->mh; y++) {
        for (x = 0; x < st->mw; x++) {
            seq->off[z++] = (IM_REGION_ADDR(ir, x, y) - IM_REGION_ADDR(ir, 0, 0)) / IM_IMAGE_SIZEOF_ELEMENT(ir->im);
        }
    }

    switch (ir->im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        ishrink(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        ishrink(char);
        break;
    case IM_BANDFMT_USHORT:
        ishrink(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        ishrink(short);
        break;
    case IM_BANDFMT_UINT:
        ishrink(unsigned int);
        break;
    case IM_BANDFMT_INT:
        ishrink(int);
        break;
    case IM_BANDFMT_FLOAT:
        fshrink(float);
        break;
    case IM_BANDFMT_DOUBLE:
        fshrink(double);
        break;

    default:
        im_error("im_shrink", "%s", _("unsupported input format"));
        return (-1);
    }

    return (0);
}

static int shrink(IMAGE *in, IMAGE *out, double xshrink, double yshrink) {
    ShrinkInfo *st;

    if (!in || vips_bandfmt_iscomplex(in->BandFmt)) {
        im_error("im_shrink", "%s", _("non-complex input only"));
        return (-1);
    }
    if (xshrink < 1.0 || yshrink < 1.0) {
        im_error("im_shrink", "%s", _("shrink factors should both be >1"));
        return (-1);
    }
    if (im_piocheck(in, out)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = in->Xsize / xshrink;
    out->Ysize = in->Ysize / yshrink;
    out->Xres = in->Xres / xshrink;
    out->Yres = in->Yres / yshrink;
    if (out->Xsize <= 0 || out->Ysize <= 0) {
        im_error("im_shrink", "%s", _("image has shrunk to nothing"));
        return (-1);
    }

    if (!(st = IM_NEW(out, ShrinkInfo))) {
        return (-1);
    }
    st->xshrink = xshrink;
    st->yshrink = yshrink;
    st->mw = ceil(xshrink);
    st->mh = ceil(yshrink);
    st->np = st->mw * st->mh;

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, shrink_start, shrink_gen, shrink_stop, in, st)) {
        return (-1);
    }

    return (0);
}

int im_shrink(IMAGE *in, IMAGE *out, double xshrink, double yshrink) {
    if (xshrink == 1 && yshrink == 1) {
        return (im_copy(in, out));
    } else if (in->Coding == IM_CODING_LABQ) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "im_shrink:1", "p") || im_LabQ2LabS(in, t[0]) || shrink(t[0], t[1], xshrink, yshrink) || im_LabS2LabQ(t[1], out)) {
            return (-1);
        }
    } else if (in->Coding == IM_CODING_NONE) {
        if (shrink(in, out, xshrink, yshrink)) {
            return (-1);
        }
    } else {
        im_error("im_shrink", "%s", _("unknown coding type"));
        return (-1);
    }

    return (0);
}
