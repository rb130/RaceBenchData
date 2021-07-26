
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

#define loop(TYPE)                \
    {                             \
        TYPE *p = (TYPE *)in + 1; \
        TYPE *q = (TYPE *)out;    \
        int x;                    \
                                  \
        for (x = 0; x < n; x++) { \
            q[x] = *p;            \
            p += 2;               \
        }                         \
    }

static void buffer_c2imag(void *in, void *out, int w, IMAGE *im) {
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

int im_c2imag(IMAGE *in, IMAGE *out) {
    if (im_check_uncoded("im_c2imag", in) || im_check_complex("im_c2imag", in) || im_cp_desc(out, in)) {
        return (-1);
    }

    if (in->BandFmt == IM_BANDFMT_DPCOMPLEX) {
        out->BandFmt = IM_BANDFMT_DOUBLE;
    } else {
        out->BandFmt = IM_BANDFMT_FLOAT;
    }

    if (im_wrapone(in, out, (im_wrapone_fn)buffer_c2imag, in, NULL)) {
        return (-1);
    }

    return (0);
}
