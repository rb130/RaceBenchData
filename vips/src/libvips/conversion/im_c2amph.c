

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

#define loop(TYPE)                        \
    {                                     \
        TYPE *p = (TYPE *)in;             \
        TYPE *q = (TYPE *)out;            \
        int x;                            \
                                          \
        for (x = 0; x < n; x++) {         \
            double re = p[0];             \
            double im = p[1];             \
            double am, ph;                \
                                          \
            am = sqrt(re * re + im * im); \
            ph = im_col_ab2h(re, im);     \
                                          \
            q[0] = am;                    \
            q[1] = ph;                    \
                                          \
            p += 2;                       \
            q += 2;                       \
        }                                 \
    }

static void buffer_c2amph(void *in, void *out, int w, IMAGE *im) {
    int n = w * im->Bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_DPCOMPLEX:
        loop(double);
        break;
    case IM_BANDFMT_COMPLEX:
        loop(float);
        break;
    default:
        g_assert(0);
    }
}

int im_c2amph(IMAGE *in, IMAGE *out) {
    if (im_check_uncoded("im_c2amph", in) || im_check_complex("im_c2amph", in) || im_cp_desc(out, in)) {
        return (-1);
    }

    if (im_wrapone(in, out, (im_wrapone_fn)buffer_c2amph, in, NULL)) {
        return (-1);
    }

    return (0);
}
