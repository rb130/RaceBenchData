
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

#define loop(TYPE)                     \
    {                                  \
        TYPE *p = (TYPE *)in;          \
        TYPE *q = (TYPE *)out;         \
        int x;                         \
                                       \
        for (x = 0; x < n; x++) {      \
            double am = p[0];          \
            double ph = p[1];          \
            double re, im;             \
                                       \
            re = am * cos(IM_RAD(ph)); \
            im = am * sin(IM_RAD(ph)); \
                                       \
            q[0] = re;                 \
            q[1] = im;                 \
                                       \
            p += 2;                    \
            q += 2;                    \
        }                              \
    }

static void buffer_c2rect(void *in, void *out, int w, IMAGE *im) {
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

int im_c2rect(IMAGE *in, IMAGE *out) {
    if (im_check_uncoded("im_c2rect", in) || im_check_complex("im_c2rect", in) || im_cp_desc(out, in)) {
        return (-1);
    }

    if (im_wrapone(in, out, (im_wrapone_fn)buffer_c2rect, in, NULL)) {
        return (-1);
    }

    return (0);
}
