
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define USE_MODARG_DIV

#ifdef USE_MODARG_DIV

#define COMPLEX_PHASE_FN(TYPE, ABS)                                                                        \
    static void complex_phase_##TYPE(void *in1, void *in2, void *out, int n, void *im, void *unrequired) { \
                                                                                                           \
        TYPE *X = (TYPE *)in1;                                                                             \
        TYPE *Y = (TYPE *)in2;                                                                             \
        TYPE *Z = (TYPE *)out;                                                                             \
        TYPE *Z_stop = Z + 2 * n * ((IMAGE *)im)->Bands;                                                   \
                                                                                                           \
        for (; Z < Z_stop; X += 2, Y += 2) {                                                               \
            double arg = atan2(X[1], X[0]) - atan2(Y[1], Y[0]);                                            \
            *Z++ = cos(arg);                                                                               \
            *Z++ = sin(arg);                                                                               \
        }                                                                                                  \
    }

#else

#define COMPLEX_PHASE_FN(TYPE, ABS)                                                                        \
    static void complex_phase_##TYPE(void *in1, void *in2, void *out, int n, void *im, void *unrequired) { \
                                                                                                           \
        TYPE *X = (TYPE *)in1;                                                                             \
        TYPE *Y = (TYPE *)in2;                                                                             \
        TYPE *Z = (TYPE *)out;                                                                             \
        TYPE *Z_stop = Z + 2 * n * ((IMAGE *)im)->Bands;                                                   \
                                                                                                           \
        for (; Z < Z_stop; X += 2, Y += 2)                                                                 \
                                                                                                           \
            if (ABS(Y[0]) > ABS(Y[1])) {                                                                   \
                double a = Y[1] / Y[0];                                                                    \
                double b = Y[0] + Y[1] * a;                                                                \
                double re = (X[0] + X[1] * a) / b;                                                         \
                double im = (X[1] - X[0] * a) / b;                                                         \
                double mod = im__hypot(re, im);                                                            \
                *Z++ = re / mod;                                                                           \
                *Z++ = im / mod;                                                                           \
            } else {                                                                                       \
                double a = Y[0] / Y[1];                                                                    \
                double b = Y[1] + Y[0] * a;                                                                \
                double re = (X[0] * a + X[1]) / b;                                                         \
                double im = (X[1] * a - X[0]) / b;                                                         \
                double mod = im__hypot(re, im);                                                            \
                *Z++ = re / mod;                                                                           \
                *Z++ = im / mod;                                                                           \
            }                                                                                              \
    }

#endif

COMPLEX_PHASE_FN(float, fabsf)
COMPLEX_PHASE_FN(double, fabs)

int im_cross_phase(IMAGE *a, IMAGE *b, IMAGE *out) {
#define FUNCTION_NAME "im_phase"

    if (im_pincheck(a) || im_pincheck(b) || im_poutcheck(out)) {
        return -1;
    }

    if (im_check_size_same(FUNCTION_NAME, a, b) || im_check_bands_same(FUNCTION_NAME, a, b) || im_check_format_same(FUNCTION_NAME, a, b) || im_check_uncoded(FUNCTION_NAME, a) || im_check_uncoded(FUNCTION_NAME, b) || im_check_complex(FUNCTION_NAME, a) || im_check_complex(FUNCTION_NAME, b)) {
        return -1;
    }

    return im_cp_descv(out, a, b, NULL) || im_wraptwo(a, b, out, IM_BANDFMT_COMPLEX == a->BandFmt ? complex_phase_float : complex_phase_double, a, NULL);
}
