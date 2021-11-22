

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

#define LOOP(IN, OUT)             \
    {                             \
        IN *p1 = (IN *)in[0];     \
        IN *p2 = (IN *)in[1];     \
        OUT *q = (OUT *)out;      \
                                  \
        for (x = 0; x < sz; x++)  \
            q[x] = p1[x] - p2[x]; \
    }

static void subtract_buffer(PEL **in, PEL *out, int width, IMAGE *im) {

    const int sz = width * im->Bands * (vips_bandfmt_iscomplex(im->BandFmt) ? 2 : 1);

    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        LOOP(signed char, signed short);
        break;
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char, signed short);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short, signed int);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short, signed int);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int, signed int);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int, signed int);
        break;

    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_COMPLEX:
#ifdef HAVE_LIBOIL
        oil_subtract_f32((float *)out, (float *)in[0], (float *)in[1], sz);
#else
        LOOP(float, float);
#endif
        break;

    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_DPCOMPLEX:
        LOOP(double, double);
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

static int bandfmt_subtract[10] = {

    S, S, I, I, I, I, F, X, D, DX};

int im_subtract(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_subtract", in1, in2, out, bandfmt_subtract, (im_wrapmany_fn)subtract_buffer, NULL));
}
