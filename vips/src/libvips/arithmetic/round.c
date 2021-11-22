

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

#define ROUND_LOOP(TYPE, FUN)    \
    {                            \
        TYPE *p = (TYPE *)in;    \
        TYPE *q = (TYPE *)out;   \
                                 \
        for (x = 0; x < ne; x++) \
            q[x] = FUN(p[x]);    \
    }

#define ROUND_BUFFER(FUN)                                                                 \
    static void FUN##_buffer(PEL *in, PEL *out, int width, IMAGE *im) {                   \
                                                                                          \
        const int ne = width * im->Bands * (vips_bandfmt_iscomplex(im->BandFmt) ? 2 : 1); \
                                                                                          \
        int x;                                                                            \
                                                                                          \
        switch (im->BandFmt) {                                                            \
        case IM_BANDFMT_COMPLEX:                                                          \
        case IM_BANDFMT_FLOAT:                                                            \
            ROUND_LOOP(float, FUN);                                                       \
            break;                                                                        \
                                                                                          \
        case IM_BANDFMT_DOUBLE:                                                           \
        case IM_BANDFMT_DPCOMPLEX:                                                        \
            ROUND_LOOP(double, FUN);                                                      \
            break;                                                                        \
                                                                                          \
        default:                                                                          \
            g_assert(0);                                                                  \
        }                                                                                 \
    }

static int im__round(const char *name, IMAGE *in, IMAGE *out, im_wrapone_fn gen) {
    if (im_piocheck(in, out) || im_check_uncoded(name, in)) {
        return (-1);
    }

    if (vips_bandfmt_isint(in->BandFmt)) {
        return (im_copy(in, out));
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }

    if (im_wrapone(in, out, (im_wrapone_fn)gen, in, NULL)) {
        return (-1);
    }

    return (0);
}

ROUND_BUFFER(ceil)

int im_ceil(IMAGE *in, IMAGE *out) {
    return (im__round("im_ceil", in, out, (im_wrapone_fn)ceil_buffer));
}

ROUND_BUFFER(floor)

int im_floor(IMAGE *in, IMAGE *out) {
    return (im__round("im_floor", in, out, (im_wrapone_fn)floor_buffer));
}

ROUND_BUFFER(IM_RINT)

int im_rint(IMAGE *in, IMAGE *out) {
    return (im__round("im_rint", in, out, (im_wrapone_fn)IM_RINT_buffer));
}
