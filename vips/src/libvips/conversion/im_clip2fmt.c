

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *in;
    IMAGE *out;
    int ofmt;

    int underflow;
    int overflow;
} Clip;

static int clip_evalstart(Clip *clip) {

    clip->overflow = 0;
    clip->underflow = 0;

    return (0);
}

static int clip_evalend(Clip *clip) {

    if (clip->overflow || clip->underflow) {
        im_warn("im_clip", _("%d underflows and %d overflows detected"), clip->underflow, clip->overflow);
    }

    return (0);
}

static Clip *clip_new(IMAGE *in, IMAGE *out, int ofmt) {
    Clip *clip = IM_NEW(out, Clip);

    if (!clip) {
        return (NULL);
    }

    clip->in = in;
    clip->out = out;
    clip->ofmt = ofmt;
    clip->underflow = 0;
    clip->overflow = 0;

    if (im_add_evalstart_callback(out, (im_callback_fn)clip_evalstart, clip, NULL) || im_add_evalend_callback(out, (im_callback_fn)clip_evalend, clip, NULL)) {
        return (NULL);
    }

    return (clip);
}

typedef struct {
    REGION *ir;
    int underflow;
    int overflow;
} ClipSequence;

static int clip_stop(void *vseq, void *a, void *b) {
    ClipSequence *seq = (ClipSequence *)vseq;
    Clip *clip = (Clip *)b;

    clip->underflow += seq->underflow;
    clip->overflow += seq->overflow;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *clip_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    ClipSequence *seq;

    if (!(seq = IM_NEW(out, ClipSequence))) {
        return (NULL);
    }

    seq->ir = NULL;
    seq->overflow = 0;
    seq->underflow = 0;

    if (!(seq->ir = im_region_create(in))) {
        return (NULL);
    }

    return (seq);
}

#define IM_CLIP_INT_INT(ITYPE, OTYPE, IM_CLIP) \
    {                                          \
        ITYPE *p = (ITYPE *)in;                \
        OTYPE *q = (OTYPE *)out;               \
                                               \
        for (x = 0; x < sz; x++) {             \
            int t = p[x];                      \
                                               \
            IM_CLIP(t, seq);                   \
                                               \
            q[x] = t;                          \
        }                                      \
    }

#define IM_CLIP_FLOAT_INT(ITYPE, OTYPE, IM_CLIP) \
    {                                            \
        ITYPE *p = (ITYPE *)in;                  \
        OTYPE *q = (OTYPE *)out;                 \
                                                 \
        for (x = 0; x < sz; x++) {               \
            ITYPE v = floor(p[x]);               \
                                                 \
            IM_CLIP(v, seq);                     \
                                                 \
            q[x] = v;                            \
        }                                        \
    }

#define IM_CLIP_COMPLEX_INT(ITYPE, OTYPE, IM_CLIP) \
    {                                              \
        ITYPE *p = (ITYPE *)in;                    \
        OTYPE *q = (OTYPE *)out;                   \
                                                   \
        for (x = 0; x < sz; x++) {                 \
            ITYPE v = floor(p[0]);                 \
            p += 2;                                \
                                                   \
            IM_CLIP(v, seq);                       \
                                                   \
            q[x] = v;                              \
        }                                          \
    }

#define IM_CLIP_REAL_FLOAT(ITYPE, OTYPE) \
    {                                    \
        ITYPE *p = (ITYPE *)in;          \
        OTYPE *q = (OTYPE *)out;         \
                                         \
        for (x = 0; x < sz; x++)         \
            q[x] = p[x];                 \
    }

#define IM_CLIP_COMPLEX_FLOAT(ITYPE, OTYPE) \
    {                                       \
        ITYPE *p = (ITYPE *)in;             \
        OTYPE *q = (OTYPE *)out;            \
                                            \
        for (x = 0; x < sz; x++) {          \
            q[x] = p[0];                    \
            p += 2;                         \
        }                                   \
    }

#define IM_CLIP_REAL_COMPLEX(ITYPE, OTYPE) \
    {                                      \
        ITYPE *p = (ITYPE *)in;            \
        OTYPE *q = (OTYPE *)out;           \
                                           \
        for (x = 0; x < sz; x++) {         \
            q[0] = p[x];                   \
            q[1] = 0.0;                    \
            q += 2;                        \
        }                                  \
    }

#define IM_CLIP_COMPLEX_COMPLEX(ITYPE, OTYPE) \
    {                                         \
        ITYPE *p = (ITYPE *)in;               \
        OTYPE *q = (OTYPE *)out;              \
                                              \
        for (x = 0; x < sz; x++) {            \
            q[0] = p[0];                      \
            q[1] = p[1];                      \
            p += 2;                           \
            q += 2;                           \
        }                                     \
    }

#define BAND_SWITCH_INNER(ITYPE, INT, FLOAT, COMPLEX)   \
    {                                                   \
        switch (clip->out->BandFmt) {                   \
        case IM_BANDFMT_UCHAR:                          \
            INT(ITYPE, unsigned char, IM_CLIP_UCHAR);   \
            break;                                      \
        case IM_BANDFMT_CHAR:                           \
            INT(ITYPE, signed char, IM_CLIP_CHAR);      \
            break;                                      \
        case IM_BANDFMT_USHORT:                         \
            INT(ITYPE, unsigned short, IM_CLIP_USHORT); \
            break;                                      \
        case IM_BANDFMT_SHORT:                          \
            INT(ITYPE, signed short, IM_CLIP_SHORT);    \
            break;                                      \
        case IM_BANDFMT_UINT:                           \
            INT(ITYPE, unsigned int, IM_CLIP_NONE);     \
            break;                                      \
        case IM_BANDFMT_INT:                            \
            INT(ITYPE, signed int, IM_CLIP_NONE);       \
            break;                                      \
        case IM_BANDFMT_FLOAT:                          \
            FLOAT(ITYPE, float);                        \
            break;                                      \
        case IM_BANDFMT_DOUBLE:                         \
            FLOAT(ITYPE, double);                       \
            break;                                      \
        case IM_BANDFMT_COMPLEX:                        \
            COMPLEX(ITYPE, float);                      \
            break;                                      \
        case IM_BANDFMT_DPCOMPLEX:                      \
            COMPLEX(ITYPE, double);                     \
            break;                                      \
        default:                                        \
            g_assert(0);                                \
        }                                               \
    }

static int clip_gen(REGION * or, void *vseq, void *a, void *b) {
    ClipSequence *seq = (ClipSequence *)vseq;
    Clip *clip = (Clip *)b;
    REGION *ir = seq->ir;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int sz = IM_REGION_N_ELEMENTS(or);
    int x, y;

    if (im_prepare(ir, r)) {
        return (-1);
    }

    for (y = to; y < bo; y++) {
        PEL *in = (PEL *)IM_REGION_ADDR(ir, le, y);
        PEL *out = (PEL *)IM_REGION_ADDR(or, le, y);

        switch (clip->in->BandFmt) {
        case IM_BANDFMT_UCHAR:
            BAND_SWITCH_INNER(unsigned char, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_CHAR:
            BAND_SWITCH_INNER(signed char, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_USHORT:
            BAND_SWITCH_INNER(unsigned short, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_SHORT:
            BAND_SWITCH_INNER(signed short, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_UINT:
            BAND_SWITCH_INNER(unsigned int, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_INT:
            BAND_SWITCH_INNER(signed int, IM_CLIP_INT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_FLOAT:
            BAND_SWITCH_INNER(float, IM_CLIP_FLOAT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_DOUBLE:
            BAND_SWITCH_INNER(double, IM_CLIP_FLOAT_INT, IM_CLIP_REAL_FLOAT, IM_CLIP_REAL_COMPLEX);
            break;
        case IM_BANDFMT_COMPLEX:
            BAND_SWITCH_INNER(float, IM_CLIP_COMPLEX_INT, IM_CLIP_COMPLEX_FLOAT, IM_CLIP_COMPLEX_COMPLEX);
            break;
        case IM_BANDFMT_DPCOMPLEX:
            BAND_SWITCH_INNER(double, IM_CLIP_COMPLEX_INT, IM_CLIP_COMPLEX_FLOAT, IM_CLIP_COMPLEX_COMPLEX);
            break;
        default:
            g_assert(0);
        }
    }

    return (0);
}

int im_clip2fmt(IMAGE *in, IMAGE *out, VipsBandFmt fmt) {
    Clip *clip;

    if (im_check_uncoded("im_clip2fmt", in) || im_piocheck(in, out)) {
        return (-1);
    }
    if (fmt < 0 || fmt > IM_BANDFMT_DPCOMPLEX) {
        im_error("im_clip2fmt", "%s", _("fmt out of range"));
        return (-1);
    }

    if (in->BandFmt == fmt) {
        return (im_copy(in, out));
    }

    if (!(clip = clip_new(in, out, fmt))) {
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = fmt;

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL) || im_generate(out, clip_start, clip_gen, clip_stop, in, clip)) {
        return (-1);
    }

    return (0);
}
