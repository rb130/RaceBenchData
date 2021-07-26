
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define CSIGN(IN, OUT)                            \
    {                                             \
        IN *p = (IN *)in;                         \
        OUT *q = (OUT *)out;                      \
        int x;                                    \
                                                  \
        for (x = 0; x < n; x++) {                 \
            IN re = p[0];                         \
            IN im = p[1];                         \
            double fac = sqrt(re * re + im * im); \
                                                  \
            p += 2;                               \
                                                  \
            if (fac == 0.0) {                     \
                q[0] = 0.0;                       \
                q[1] = 0.0;                       \
            } else {                              \
                q[0] = re / fac;                  \
                q[1] = im / fac;                  \
            }                                     \
                                                  \
            q += 2;                               \
        }                                         \
    }

#define SIGN(IN, OUT)             \
    {                             \
        IN *p = (IN *)in;         \
        OUT *q = (OUT *)out;      \
        int x;                    \
                                  \
        for (x = 0; x < n; x++) { \
            IN v = p[x];          \
                                  \
            if (v > 0)            \
                q[x] = 1;         \
            else if (v == 0)      \
                q[x] = 0;         \
            else                  \
                q[x] = -1;        \
        }                         \
    }

static void sign_gen(void *in, void *out, int w, IMAGE *im) {
    int n = w * im->Bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        SIGN(unsigned char, signed char);
        break;
    case IM_BANDFMT_CHAR:
        SIGN(signed char, signed char);
        break;
    case IM_BANDFMT_USHORT:
        SIGN(unsigned short, signed char);
        break;
    case IM_BANDFMT_SHORT:
        SIGN(signed short, signed char);
        break;
    case IM_BANDFMT_UINT:
        SIGN(unsigned int, signed char);
        break;
    case IM_BANDFMT_INT:
        SIGN(signed int, signed char);
        break;
    case IM_BANDFMT_FLOAT:
        SIGN(float, signed char);
        break;
    case IM_BANDFMT_DOUBLE:
        SIGN(double, signed char);
        break;
    case IM_BANDFMT_COMPLEX:
        CSIGN(float, float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        CSIGN(double, double);
        break;

    default:
        g_assert(0);
    }
}

int im_sign(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_uncoded("im_sign", in) || im_cp_desc(out, in)) {
        return (-1);
    }

    if (!vips_bandfmt_iscomplex(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_CHAR;
    }

    if (im_wrapone(in, out, (im_wrapone_fn)sign_gen, in, NULL)) {
        return (-1);
    }

    return (0);
}
