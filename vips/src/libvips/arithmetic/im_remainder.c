

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IREMAINDER(TYPE)              \
    {                                 \
        TYPE *p1 = (TYPE *)in[0];     \
        TYPE *p2 = (TYPE *)in[1];     \
        TYPE *q = (TYPE *)out;        \
                                      \
        for (x = 0; x < ne; x++)      \
            if (p2[x])                \
                q[x] = p1[x] % p2[x]; \
            else                      \
                q[x] = -1;            \
    }

#define FREMAINDER(TYPE)                     \
    {                                        \
        TYPE *p1 = (TYPE *)in[0];            \
        TYPE *p2 = (TYPE *)in[1];            \
        TYPE *q = (TYPE *)out;               \
                                             \
        for (x = 0; x < ne; x++) {           \
            double a = p1[x];                \
            double b = p2[x];                \
                                             \
            if (b)                           \
                q[x] = a - b * floor(a / b); \
            else                             \
                q[x] = -1;                   \
        }                                    \
    }

static void remainder_buffer(PEL **in, PEL *out, int width, IMAGE *im) {
    const int ne = width * im->Bands;

    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        IREMAINDER(signed char);
        break;
    case IM_BANDFMT_UCHAR:
        IREMAINDER(unsigned char);
        break;
    case IM_BANDFMT_SHORT:
        IREMAINDER(signed short);
        break;
    case IM_BANDFMT_USHORT:
        IREMAINDER(unsigned short);
        break;
    case IM_BANDFMT_INT:
        IREMAINDER(signed int);
        break;
    case IM_BANDFMT_UINT:
        IREMAINDER(unsigned int);
        break;
    case IM_BANDFMT_FLOAT:
        FREMAINDER(float);
        break;
    case IM_BANDFMT_DOUBLE:
        FREMAINDER(double);
        break;

    default:
        g_assert(0);
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

static int bandfmt_remainder[10] = {

    UC, C, US, S, UI, I, F, X, D, DX};

int im_remainder(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    if (im_check_noncomplex("im_remainder", in1) || im_check_noncomplex("im_remainder", in2)) {
        return (-1);
    }

    return (im__arith_binary("im_remainder", in1, in2, out, bandfmt_remainder, (im_wrapmany_fn)remainder_buffer, NULL));
}

#define CAST_CLIP(TYPE, N, X)                  \
    {                                          \
        TYPE *tq = (TYPE *)q;                  \
                                               \
        for (i = 0; i < n; i++)                \
            tq[i] = (TYPE)IM_CLIP(N, p[i], X); \
    }

#define CAST(TYPE)              \
    {                           \
        TYPE *tq = (TYPE *)q;   \
                                \
        for (i = 0; i < n; i++) \
            tq[i] = (TYPE)p[i]; \
    }

#define CASTC(TYPE)               \
    {                             \
        TYPE *tq = (TYPE *)q;     \
                                  \
        for (i = 0; i < n; i++) { \
            tq[0] = (TYPE)p[i];   \
            tq[1] = 0;            \
            tq += 2;              \
        }                         \
    }

static PEL *make_pixel(IMAGE *out, VipsBandFmt fmt, int n, double *p) {
    PEL *q;
    int i;

    if (!(q = IM_ARRAY(out, n * (im_bits_of_fmt(fmt) >> 3), PEL))) {
        return (NULL);
    }

    switch (fmt) {
    case IM_BANDFMT_CHAR:
        CAST_CLIP(signed char, SCHAR_MIN, SCHAR_MAX);
        break;

    case IM_BANDFMT_UCHAR:
        CAST_CLIP(unsigned char, 0, UCHAR_MAX);
        break;

    case IM_BANDFMT_SHORT:
        CAST_CLIP(signed short, SCHAR_MIN, SCHAR_MAX);
        break;

    case IM_BANDFMT_USHORT:
        CAST_CLIP(unsigned short, 0, USHRT_MAX);
        break;

    case IM_BANDFMT_INT:
        CAST_CLIP(signed int, INT_MIN, INT_MAX);
        break;

    case IM_BANDFMT_UINT:
        CAST_CLIP(unsigned int, 0, UINT_MAX);
        break;

    case IM_BANDFMT_FLOAT:
        CAST(float);
        break;

    case IM_BANDFMT_DOUBLE:
        CAST(double);
        break;

    case IM_BANDFMT_COMPLEX:
        CASTC(float);
        break;

    case IM_BANDFMT_DPCOMPLEX:
        CASTC(double);
        break;

    default:
        g_assert(0);
    }

    return (q);
}

int im__arith_binary_const(const char *name, IMAGE *in, IMAGE *out, int n, double *c, VipsBandFmt vfmt, int format_table[10], im_wrapone_fn fn1, im_wrapone_fn fnn) {
    PEL *vector;

    if (im_piocheck(in, out) || im_check_vector(name, n, in) || im_check_uncoded(name, in)) {
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = format_table[in->BandFmt];

    if (!(vector = make_pixel(out, vfmt, n, c))) {
        return (-1);
    }

    if (n > 1 && out->Bands == 1) {
        IMAGE *t;

        if (!(t = im_open_local(out, "arith_binary_const", "p")) || im__bandup(in, t, n)) {
            return (-1);
        }

        in = t;
    }

    if (n == 1) {
        if (im_wrapone(in, out, fn1, vector, in)) {
            return (-1);
        }
    } else {
        if (im_wrapone(in, out, fnn, vector, in)) {
            return (-1);
        }
    }

    return (0);
}

#define IREMAINDERCONST1(TYPE)      \
    {                               \
        TYPE *p = (TYPE *)in;       \
        TYPE *q = (TYPE *)out;      \
        TYPE c = *((TYPE *)vector); \
                                    \
        for (x = 0; x < ne; x++)    \
            q[x] = p[x] % c;        \
    }

#define FREMAINDERCONST1(TYPE)               \
    {                                        \
        TYPE *p = (TYPE *)in;                \
        TYPE *q = (TYPE *)out;               \
        TYPE c = *((TYPE *)vector);          \
                                             \
        for (x = 0; x < ne; x++) {           \
            double a = p[x];                 \
                                             \
            if (c)                           \
                q[x] = a - c * floor(a / c); \
            else                             \
                q[x] = -1;                   \
        }                                    \
    }

static void remainderconst1_buffer(PEL *in, PEL *out, int width, PEL *vector, IMAGE *im) {
    const int ne = width * im->Bands;

    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        IREMAINDERCONST1(signed char);
        break;
    case IM_BANDFMT_UCHAR:
        IREMAINDERCONST1(unsigned char);
        break;
    case IM_BANDFMT_SHORT:
        IREMAINDERCONST1(signed short);
        break;
    case IM_BANDFMT_USHORT:
        IREMAINDERCONST1(unsigned short);
        break;
    case IM_BANDFMT_INT:
        IREMAINDERCONST1(signed int);
        break;
    case IM_BANDFMT_UINT:
        IREMAINDERCONST1(unsigned int);
        break;
    case IM_BANDFMT_FLOAT:
        FREMAINDERCONST1(float);
        break;
    case IM_BANDFMT_DOUBLE:
        FREMAINDERCONST1(double);
        break;

    default:
        g_assert(0);
    }
}

#define IREMAINDERCONSTN(TYPE)             \
    {                                      \
        TYPE *p = (TYPE *)in;              \
        TYPE *q = (TYPE *)out;             \
        TYPE *c = (TYPE *)vector;          \
                                           \
        for (i = 0, x = 0; x < width; x++) \
            for (k = 0; k < b; k++, i++)   \
                q[i] = p[i] % c[k];        \
    }

#define FREMAINDERCONSTN(TYPE)                   \
    {                                            \
        TYPE *p = (TYPE *)in;                    \
        TYPE *q = (TYPE *)out;                   \
        TYPE *c = (TYPE *)vector;                \
                                                 \
        for (i = 0, x = 0; x < width; x++)       \
            for (k = 0; k < b; k++, i++) {       \
                double a = p[i];                 \
                double b = c[k];                 \
                                                 \
                if (b)                           \
                    q[i] = a - b * floor(a / b); \
                else                             \
                    q[i] = -1;                   \
            }                                    \
    }

static void remainderconstn_buffer(PEL *in, PEL *out, int width, PEL *vector, IMAGE *im) {
    int b = im->Bands;
    int i, x, k;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
        IREMAINDERCONSTN(signed char);
        break;
    case IM_BANDFMT_UCHAR:
        IREMAINDERCONSTN(unsigned char);
        break;
    case IM_BANDFMT_SHORT:
        IREMAINDERCONSTN(signed short);
        break;
    case IM_BANDFMT_USHORT:
        IREMAINDERCONSTN(unsigned short);
        break;
    case IM_BANDFMT_INT:
        IREMAINDERCONSTN(signed int);
        break;
    case IM_BANDFMT_UINT:
        IREMAINDERCONSTN(unsigned int);
        break;
    case IM_BANDFMT_FLOAT:
        FREMAINDERCONSTN(float);
        break;
    case IM_BANDFMT_DOUBLE:
        FREMAINDERCONSTN(double);
        break;

    default:
        g_assert(0);
    }
}

int im_remainder_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    if (im_check_noncomplex("im_remainder", in)) {
        return (-1);
    }

    return (im__arith_binary_const("im_remainder", in, out, n, c, in->BandFmt, bandfmt_remainder, (im_wrapone_fn)remainderconst1_buffer, (im_wrapone_fn)remainderconstn_buffer));
}

int im_remainderconst(IMAGE *in, IMAGE *out, double c) {
    return (im_remainder_vec(in, out, 1, &c));
}
