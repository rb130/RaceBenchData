

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
    int fmt;
    int nb;
    int es;
    int sz;
    int clp;
    PEL **table;
    int overflow;
} LutInfo;

static int lut_start(LutInfo *st) {
    st->overflow = 0;

    return (0);
}

static int lut_end(LutInfo *st) {
    if (st->overflow) {
        im_warn("im_maplut", _("%d overflows detected"), st->overflow);
    }

    return (0);
}

static LutInfo *build_luts(IMAGE *out, IMAGE *lut) {
    LutInfo *st;
    int i, x;
    PEL *q;

    if (!(st = IM_NEW(out, LutInfo))) {
        return (NULL);
    }

    st->fmt = lut->BandFmt;
    st->es = IM_IMAGE_SIZEOF_ELEMENT(lut);
    st->nb = lut->Bands;
    st->sz = lut->Xsize * lut->Ysize;
    st->clp = st->sz - 1;
    st->overflow = 0;
    st->table = NULL;
    if (im_add_evalstart_callback(out, (im_callback_fn)lut_start, st, NULL) || im_add_evalend_callback(out, (im_callback_fn)lut_end, st, NULL)) {
        return (NULL);
    }

    if (!(st->table = IM_ARRAY(out, lut->Bands, PEL *))) {
        return (NULL);
    }
    for (i = 0; i < lut->Bands; i++) {
        if (!(st->table[i] = IM_ARRAY(out, st->sz * st->es, PEL))) {
            return (NULL);
        }
    }

    q = (PEL *)lut->data;
    for (x = 0; x < st->sz; x++) {
        for (i = 0; i < st->nb; i++) {
            memcpy(st->table[i] + x * st->es, q, st->es);
            q += st->es;
        }
    }

    return (st);
}

typedef struct {
    REGION *ir;
    int overflow;
} Seq;

static int maplut_stop(void *vseq, void *a, void *b) {
    Seq *seq = (Seq *)vseq;
    LutInfo *st = (LutInfo *)b;

    st->overflow += seq->overflow;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *maplut_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    Seq *seq;

    if (!(seq = IM_NEW(out, Seq))) {
        return (NULL);
    }

    seq->ir = NULL;
    seq->overflow = 0;

    if (!(seq->ir = im_region_create(in))) {
        return (NULL);
    }

    return (seq);
}

#define loop(OUT)                                          \
    {                                                      \
        int b = st->nb;                                    \
                                                           \
        for (y = to; y < bo; y++) {                        \
            for (z = 0; z < b; z++) {                      \
                PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y); \
                OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
                OUT *tlut = (OUT *)st->table[z];           \
                                                           \
                for (x = z; x < ne; x += b)                \
                    q[x] = tlut[p[x]];                     \
            }                                              \
        }                                                  \
    }

#define loopc(OUT)                                                 \
    {                                                              \
        int b = in->Bands;                                         \
                                                                   \
        for (y = to; y < bo; y++) {                                \
            for (z = 0; z < b; z++) {                              \
                PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y) + z;     \
                OUT *q = (OUT *)IM_REGION_ADDR(or, le, y) + z * 2; \
                OUT *tlut = (OUT *)st->table[z];                   \
                                                                   \
                for (x = 0; x < ne; x += b) {                      \
                    int n = p[x] * 2;                              \
                                                                   \
                    q[0] = tlut[n];                                \
                    q[1] = tlut[n + 1];                            \
                    q += b * 2;                                    \
                }                                                  \
            }                                                      \
        }                                                          \
    }

#define loopg(IN, OUT)                                     \
    {                                                      \
        int b = st->nb;                                    \
                                                           \
        for (y = to; y < bo; y++) {                        \
            for (z = 0; z < b; z++) {                      \
                IN *p = (IN *)IM_REGION_ADDR(ir, le, y);   \
                OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
                OUT *tlut = (OUT *)st->table[z];           \
                                                           \
                for (x = z; x < ne; x += b) {              \
                    int index = p[x];                      \
                                                           \
                    if (index > st->clp) {                 \
                        index = st->clp;                   \
                        seq->overflow++;                   \
                    }                                      \
                                                           \
                    q[x] = tlut[index];                    \
                }                                          \
            }                                              \
        }                                                  \
    }

#define loopcg(IN, OUT)                                            \
    {                                                              \
        int b = in->Bands;                                         \
                                                                   \
        for (y = to; y < bo; y++) {                                \
            for (z = 0; z < b; z++) {                              \
                IN *p = (IN *)IM_REGION_ADDR(ir, le, y) + z;       \
                OUT *q = (OUT *)IM_REGION_ADDR(or, le, y) + z * 2; \
                OUT *tlut = (OUT *)st->table[z];                   \
                                                                   \
                for (x = 0; x < ne; x += b) {                      \
                    int index = p[x];                              \
                                                                   \
                    if (index > st->clp) {                         \
                        index = st->clp;                           \
                        seq->overflow++;                           \
                    }                                              \
                                                                   \
                    q[0] = tlut[index * 2];                        \
                    q[1] = tlut[index * 2 + 1];                    \
                                                                   \
                    q += b * 2;                                    \
                }                                                  \
            }                                                      \
        }                                                          \
    }

#define loop1(OUT)                                     \
    {                                                  \
        OUT *tlut = (OUT *)st->table[0];               \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y); \
                                                       \
            for (x = 0; x < ne; x++)                   \
                q[x] = tlut[p[x]];                     \
        }                                              \
    }

#define loop1c(OUT)                                    \
    {                                                  \
        OUT *tlut = (OUT *)st->table[0];               \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y); \
                                                       \
            for (x = 0; x < ne; x++) {                 \
                int n = p[x] * 2;                      \
                                                       \
                q[0] = tlut[n];                        \
                q[1] = tlut[n + 1];                    \
                q += 2;                                \
            }                                          \
        }                                              \
    }

#define loop1g(IN, OUT)                                \
    {                                                  \
        OUT *tlut = (OUT *)st->table[0];               \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            IN *p = (IN *)IM_REGION_ADDR(ir, le, y);   \
                                                       \
            for (x = 0; x < ne; x++) {                 \
                int index = p[x];                      \
                                                       \
                if (index > st->clp) {                 \
                    index = st->clp;                   \
                    seq->overflow++;                   \
                }                                      \
                                                       \
                q[x] = tlut[index];                    \
            }                                          \
        }                                              \
    }

#define loop1cg(IN, OUT)                               \
    {                                                  \
        OUT *tlut = (OUT *)st->table[0];               \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            IN *p = (IN *)IM_REGION_ADDR(ir, le, y);   \
                                                       \
            for (x = 0; x < ne; x++) {                 \
                int index = p[x];                      \
                                                       \
                if (index > st->clp) {                 \
                    index = st->clp;                   \
                    seq->overflow++;                   \
                }                                      \
                                                       \
                q[0] = tlut[index * 2];                \
                q[1] = tlut[index * 2 + 1];            \
                q += 2;                                \
            }                                          \
        }                                              \
    }

#define loop1m(OUT)                                    \
    {                                                  \
        OUT **tlut = (OUT **)st->table;                \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y); \
                                                       \
            for (i = 0, x = 0; x < np; x++) {          \
                int n = p[x];                          \
                                                       \
                for (z = 0; z < st->nb; z++, i++)      \
                    q[i] = tlut[z][n];                 \
            }                                          \
        }                                              \
    }

#define loop1cm(OUT)                                   \
    {                                                  \
        OUT **tlut = (OUT **)st->table;                \
                                                       \
        for (y = to; y < bo; y++) {                    \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
            PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y); \
                                                       \
            for (x = 0; x < np; x++) {                 \
                int n = p[x] * 2;                      \
                                                       \
                for (z = 0; z < st->nb; z++) {         \
                    q[0] = tlut[z][n];                 \
                    q[1] = tlut[z][n + 1];             \
                    q += 2;                            \
                }                                      \
            }                                          \
        }                                              \
    }

#define loop1gm(IN, OUT)                               \
    {                                                  \
        OUT **tlut = (OUT **)st->table;                \
                                                       \
        for (y = to; y < bo; y++) {                    \
            IN *p = (IN *)IM_REGION_ADDR(ir, le, y);   \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
                                                       \
            for (i = 0, x = 0; x < np; x++) {          \
                int n = p[x];                          \
                                                       \
                if (n > st->clp) {                     \
                    n = st->clp;                       \
                    seq->overflow++;                   \
                }                                      \
                                                       \
                for (z = 0; z < st->nb; z++, i++)      \
                    q[i] = tlut[z][n];                 \
            }                                          \
        }                                              \
    }

#define loop1cgm(IN, OUT)                              \
    {                                                  \
        OUT **tlut = (OUT **)st->table;                \
                                                       \
        for (y = to; y < bo; y++) {                    \
            IN *p = (IN *)IM_REGION_ADDR(ir, le, y);   \
            OUT *q = (OUT *)IM_REGION_ADDR(or, le, y); \
                                                       \
            for (x = 0; x < np; x++) {                 \
                int n = p[x];                          \
                                                       \
                if (n > st->clp) {                     \
                    n = st->clp;                       \
                    seq->overflow++;                   \
                }                                      \
                                                       \
                for (z = 0; z < st->nb; z++) {         \
                    q[0] = tlut[z][n * 2];             \
                    q[1] = tlut[z][n * 2 + 1];         \
                    q += 2;                            \
                }                                      \
            }                                          \
        }                                              \
    }

#define inner_switch(UCHAR, GEN, OUT) \
    switch (ir->im->BandFmt) {        \
    case IM_BANDFMT_UCHAR:            \
        UCHAR(OUT);                   \
        break;                        \
    case IM_BANDFMT_USHORT:           \
        GEN(unsigned short, OUT);     \
        break;                        \
    case IM_BANDFMT_UINT:             \
        GEN(unsigned int, OUT);       \
        break;                        \
    default:                          \
        g_assert(0);                  \
    }

#define outer_switch(UCHAR_F, UCHAR_FC, GEN_F, GEN_FC) \
    switch (st->fmt) {                                 \
    case IM_BANDFMT_UCHAR:                             \
        inner_switch(UCHAR_F, GEN_F, unsigned char);   \
        break;                                         \
    case IM_BANDFMT_CHAR:                              \
        inner_switch(UCHAR_F, GEN_F, char);            \
        break;                                         \
    case IM_BANDFMT_USHORT:                            \
        inner_switch(UCHAR_F, GEN_F, unsigned short);  \
        break;                                         \
    case IM_BANDFMT_SHORT:                             \
        inner_switch(UCHAR_F, GEN_F, short);           \
        break;                                         \
    case IM_BANDFMT_UINT:                              \
        inner_switch(UCHAR_F, GEN_F, unsigned int);    \
        break;                                         \
    case IM_BANDFMT_INT:                               \
        inner_switch(UCHAR_F, GEN_F, int);             \
        break;                                         \
    case IM_BANDFMT_FLOAT:                             \
        inner_switch(UCHAR_F, GEN_F, float);           \
        break;                                         \
    case IM_BANDFMT_DOUBLE:                            \
        inner_switch(UCHAR_F, GEN_F, double);          \
        break;                                         \
    case IM_BANDFMT_COMPLEX:                           \
        inner_switch(UCHAR_FC, GEN_FC, float);         \
        break;                                         \
    case IM_BANDFMT_DPCOMPLEX:                         \
        inner_switch(UCHAR_FC, GEN_FC, double);        \
        break;                                         \
    default:                                           \
        g_assert(0);                                   \
    }

static int maplut_gen(REGION * or, void *vseq, void *a, void *b) {
    Seq *seq = (Seq *)vseq;
    IMAGE *in = (IMAGE *)a;
    LutInfo *st = (LutInfo *)b;
    REGION *ir = seq->ir;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int np = r->width;
    int ne = IM_REGION_N_ELEMENTS(or);
    int x, y, z, i;

    if (im_prepare(ir, r)) {
        return (-1);
    }

    if (st->nb == 1) {

        outer_switch(loop1, loop1c, loop1g, loop1cg)
    } else

        if (ir->im->Bands == 1) {

        outer_switch(loop1m, loop1cm, loop1gm, loop1cgm)
    } else {
        outer_switch(loop, loopc, loopg, loopcg)
    }
    return (0);
}

#define UC IM_BANDFMT_UCHAR
#define US IM_BANDFMT_USHORT
#define UI IM_BANDFMT_UINT

static int bandfmt_maplut[10] = {

    UC, UC, US, US, UI, UI, UI, UI, UI, UI};

int im_maplut(IMAGE *in, IMAGE *out, IMAGE *lut) {
    IMAGE *t;
    LutInfo *st;

    if (im_check_hist("im_maplut", lut) || im_check_uncoded("im_maplut", lut) || im_check_uncoded("im_maplut", in) || im_check_bands_1orn("im_maplut", in, lut) || im_piocheck(in, out) || im_incheck(lut)) {
        return (-1);
    }

    if (!(t = im_open_local(out, "im_maplut", "p")) || im_clip2fmt(in, t, bandfmt_maplut[in->BandFmt])) {
        return (-1);
    }

    if (im_cp_descv(out, t, lut, NULL)) {
        return (-1);
    }

    out->BandFmt = lut->BandFmt;

    if (lut->Bands != 1) {
        out->Bands = lut->Bands;
    }

    if (!(st = build_luts(out, lut))) {
        return (-1);
    }

    if (im_demand_hint(out, IM_THINSTRIP, t, NULL)) {
        return (-1);
    }

    if (im_generate(out, maplut_start, maplut_gen, maplut_stop, t, st)) {
        return (-1);
    }

    return (0);
}
