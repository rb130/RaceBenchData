

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

#define CLOOP(TYPE)                   \
    {                                 \
        TYPE *p1 = (TYPE *)in[0];     \
        TYPE *p2 = (TYPE *)in[1];     \
        TYPE *q = (TYPE *)out;        \
                                      \
        for (x = 0; x < sz; x++) {    \
            double x1 = p1[0];        \
            double y1 = p1[1];        \
            double x2 = p2[0];        \
            double y2 = p2[1];        \
                                      \
            p1 += 2;                  \
            p2 += 2;                  \
                                      \
            q[0] = x1 * x2 - y1 * y2; \
            q[1] = x1 * y2 + x2 * y1; \
                                      \
            q += 2;                   \
        }                             \
    }

#define RLOOP(IN, OUT)            \
    {                             \
        IN *p1 = (IN *)in[0];     \
        IN *p2 = (IN *)in[1];     \
        OUT *q = (OUT *)out;      \
                                  \
        for (x = 0; x < sz; x++)  \
            q[x] = p1[x] * p2[x]; \
    }

static void multiply_buffer(PEL **in, PEL *out, int width, IMAGE *im) {
    const int sz = width * im->Bands;

    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        RLOOP(signed char, signed short);
        break;
    case IM_BANDFMT_UCHAR:
        RLOOP(unsigned char, unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        RLOOP(signed short, signed int);
        break;
    case IM_BANDFMT_USHORT:
        RLOOP(unsigned short, unsigned int);
        break;
    case IM_BANDFMT_INT:
        RLOOP(signed int, signed int);
        break;
    case IM_BANDFMT_UINT:
        RLOOP(unsigned int, unsigned int);
        break;

    case IM_BANDFMT_FLOAT:
#ifdef HAVE_LIBOIL
        oil_multiply_f32((float *)out, (float *)in[0], (float *)in[1], sz);
#else
        RLOOP(float, float);
#endif
        break;

    case IM_BANDFMT_COMPLEX:
        CLOOP(float);
        break;
    case IM_BANDFMT_DOUBLE:
        RLOOP(double, double);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        CLOOP(double);
        break;

    default:
        assert(0);
    }
}

#define UC IM_BANDFMT_UCHAR
#define C IM_BANDFMT_CHAR
#define US IM_BANDFMT_USHORT
#define S IM_BANDFMT_SHORT
#define UI IM_BANDFMT_UINT
#define I IM_BANDFMT_INT
#define F IM_BANDFMT_FLOAT
#define X IM_BANDFMT_COMPLEX
#define D IM_BANDFMT_DOUBLE
#define DX IM_BANDFMT_DPCOMPLEX

static int bandfmt_multiply[10] = {

    US, S, UI, I, UI, I, F, X, D, DX};

int im_multiply(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_multiply", in1, in2, out, bandfmt_multiply, (im_wrapmany_fn)multiply_buffer, NULL));
}
