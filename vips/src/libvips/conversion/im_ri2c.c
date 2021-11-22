

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define JOIN(TYPE)                  \
    {                               \
        TYPE *p1 = (TYPE *)p[0];    \
        TYPE *p2 = (TYPE *)p[1];    \
        TYPE *q0 = (TYPE *)q;       \
                                    \
        for (x = 0; x < len; x++) { \
            q0[0] = *p1++;          \
            q0[1] = *p2++;          \
                                    \
            q0 += 2;                \
        }                           \
    }

static void join_buffer(PEL **p, PEL *q, int n, IMAGE *im) {
    int x;
    int len = n * im->Bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_FLOAT:
        JOIN(float);
        break;

    case IM_BANDFMT_DOUBLE:
        JOIN(double);
        break;

    default:
        g_assert(0);
    }
}

int im_ri2c(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t[5];
    VipsBandFmt fmt;

    if (im_check_uncoded("im_ri2c", in1) || im_check_uncoded("im_ri2c", in2) || im_check_noncomplex("im_ri2c", in1) || im_check_noncomplex("im_ri2c", in2) || im_check_bands_1orn("im_ri2c", in1, in2)) {
        return (-1);
    }

    if (in1->BandFmt == IM_BANDFMT_DOUBLE || in2->BandFmt == IM_BANDFMT_DOUBLE) {
        fmt = IM_BANDFMT_DOUBLE;
    } else {
        fmt = IM_BANDFMT_FLOAT;
    }

    if (im_open_local_array(out, t, 4, "im_ri2c", "p") || im_clip2fmt(in1, t[0], fmt) || im_clip2fmt(in2, t[1], fmt) || im__bandalike(t[0], t[1], t[2], t[3])) {
        return (-1);
    }

    t[4] = NULL;

    if (im_cp_descv(out, t[2], t[3], NULL)) {
        return (-1);
    }
    out->BandFmt = fmt == IM_BANDFMT_DOUBLE ? IM_BANDFMT_DPCOMPLEX : IM_BANDFMT_COMPLEX;

    if (im_wrapmany(t + 2, out, (im_wrapmany_fn)join_buffer, out, NULL)) {
        return (-1);
    }

    return (0);
}
