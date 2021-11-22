

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef HAVE_LIBOIL
#include <liboil/liboil.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef USE_MODARG_DIV

#define CLOOP(TYPE)                                             \
    {                                                           \
        TYPE *X = (TYPE *)in[0];                                \
        TYPE *Y = (TYPE *)in[1];                                \
        TYPE *Z = (TYPE *)out;                                  \
        int i;                                                  \
                                                                \
        for (i = 0; i < sz; i++) {                              \
            double arg = atan2(X[1], X[0]) - atan2(Y[1], Y[0]); \
            double mod = hypot(X[1], X[0]) / hypot(Y[1], Y[0]); \
                                                                \
            Z[0] = mod * cos(arg);                              \
            Z[1] = mod * sin(arg);                              \
                                                                \
            X += 2;                                             \
            Y += 2;                                             \
            Z += 2;                                             \
        }                                                       \
    }

#else

#define CLOOP(TYPE)                           \
    {                                         \
        TYPE *X = (TYPE *)in[0];              \
        TYPE *Y = (TYPE *)in[1];              \
        TYPE *Z = (TYPE *)out;                \
        int i;                                \
                                              \
        for (i = 0; i < sz; i++) {            \
            if (fabs(Y[0]) > fabs(Y[1])) {    \
                double a = Y[1] / Y[0];       \
                double b = Y[0] + Y[1] * a;   \
                                              \
                Z[0] = (X[0] + X[1] * a) / b; \
                Z[1] = (X[1] - X[0] * a) / b; \
            } else {                          \
                double a = Y[0] / Y[1];       \
                double b = Y[1] + Y[0] * a;   \
                                              \
                Z[0] = (X[0] * a + X[1]) / b; \
                Z[1] = (X[1] * a - X[0]) / b; \
            }                                 \
                                              \
            X += 2;                           \
            Y += 2;                           \
            Z += 2;                           \
        }                                     \
    }

#endif

#define RLOOP(IN, OUT)            \
    {                             \
        IN *p1 = (IN *)in[0];     \
        IN *p2 = (IN *)in[1];     \
        OUT *q = (OUT *)out;      \
                                  \
        for (x = 0; x < sz; x++)  \
            q[x] = p1[x] / p2[x]; \
    }

static void divide_buffer(PEL **in, PEL *out, int width, IMAGE *im) {
    int x;
    int sz = width * im->Bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        RLOOP(signed char, float);
        break;
    case IM_BANDFMT_UCHAR:
        RLOOP(unsigned char, float);
        break;
    case IM_BANDFMT_SHORT:
        RLOOP(signed short, float);
        break;
    case IM_BANDFMT_USHORT:
        RLOOP(unsigned short, float);
        break;
    case IM_BANDFMT_INT:
        RLOOP(signed int, float);
        break;
    case IM_BANDFMT_UINT:
        RLOOP(unsigned int, float);
        break;

    case IM_BANDFMT_FLOAT:
#ifdef HAVE_LIBOIL
        oil_divide_f32((float *)out, (float *)in[0], (float *)in[1], sz);
#else
        RLOOP(float, float);
#endif
        break;

    case IM_BANDFMT_DOUBLE:
        RLOOP(double, double);
        break;
    case IM_BANDFMT_COMPLEX:
        CLOOP(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        CLOOP(double);
        break;

    default:
        assert(0);
    }
}

#define F IM_BANDFMT_FLOAT
#define X IM_BANDFMT_COMPLEX
#define D IM_BANDFMT_DOUBLE
#define DX IM_BANDFMT_DPCOMPLEX

static int bandfmt_divide[10] = {

    F, F, F, F, F, F, F, X, D, DX};

int im_divide(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_divide", in1, in2, out, bandfmt_divide, (im_wrapmany_fn)divide_buffer, NULL));
}
