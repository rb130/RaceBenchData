

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

typedef struct {
    int n;
    double *a, *b;
} LintraInfo;

#define LOOP(IN, OUT)                 \
    {                                 \
        IN *p = (IN *)in;             \
        OUT *q = (OUT *)out;          \
                                      \
        for (x = 0; x < sz; x++)      \
            q[x] = a * (OUT)p[x] + b; \
    }

#define LOOPCMPLX(IN, OUT)         \
    {                              \
        IN *p = (IN *)in;          \
        OUT *q = (OUT *)out;       \
                                   \
        for (x = 0; x < sz; x++) { \
            q[0] = a * p[0] + b;   \
            q[1] = a * p[1];       \
            q += 2;                \
            p += 2;                \
        }                          \
    }

#ifdef HAVE_LIBOIL

#define CHUNKS (1000)

static void lintra_f32(float *d, float *s, int n, float b, float c) {
    float buf[CHUNKS];
    int i;

    for (i = 0; i < n; i += CHUNKS) {
        oil_scalarmultiply_f32_ns(buf, s, &b, IM_MIN(CHUNKS, n - i));
        oil_scalaradd_f32_ns(d, buf, &c, IM_MIN(CHUNKS, n - i));

        s += CHUNKS;
        d += CHUNKS;
    }
}
#endif

static int lintra1_gen(PEL *in, PEL *out, int width, IMAGE *im, LintraInfo *inf) {
    double a = inf->a[0];
    double b = inf->b[0];
    int sz = width * im->Bands;
    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char, float);
        break;
    case IM_BANDFMT_CHAR:
        LOOP(signed char, float);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short, float);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short, float);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int, float);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int, float);
        break;
    case IM_BANDFMT_FLOAT:
#ifdef HAVE_LIBOIL
        lintra_f32((float *)out, (float *)in, sz, a, b);
#else
        LOOP(float, float);
#endif
        break;

    case IM_BANDFMT_DOUBLE:
        LOOP(double, double);
        break;
    case IM_BANDFMT_COMPLEX:
        LOOPCMPLX(float, float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        LOOPCMPLX(double, double);
        break;

    default:
        assert(0);
    }

    return (0);
}

#define LOOPN(IN, OUT)                          \
    {                                           \
        IN *p = (IN *)in;                       \
        OUT *q = (OUT *)out;                    \
                                                \
        for (i = 0, x = 0; x < width; x++)      \
            for (k = 0; k < nb; k++, i++)       \
                q[i] = a[k] * (OUT)p[i] + b[k]; \
    }

#define LOOPCMPLXN(IN, OUT)                \
    {                                      \
        IN *p = (IN *)in;                  \
        OUT *q = (OUT *)out;               \
                                           \
        for (x = 0; x < width; x++)        \
            for (k = 0; k < nb; k++) {     \
                q[0] = a[k] * p[0] + b[k]; \
                q[1] = a[k] * p[1];        \
                q += 2;                    \
                p += 2;                    \
            }                              \
    }

static int lintran_gen(PEL *in, PEL *out, int width, IMAGE *im, LintraInfo *inf) {
    double *a = inf->a;
    double *b = inf->b;
    int nb = im->Bands;
    int i, x, k;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOPN(unsigned char, float);
        break;
    case IM_BANDFMT_CHAR:
        LOOPN(signed char, float);
        break;
    case IM_BANDFMT_USHORT:
        LOOPN(unsigned short, float);
        break;
    case IM_BANDFMT_SHORT:
        LOOPN(signed short, float);
        break;
    case IM_BANDFMT_UINT:
        LOOPN(unsigned int, float);
        break;
    case IM_BANDFMT_INT:
        LOOPN(signed int, float);
        break;
    case IM_BANDFMT_FLOAT:
        LOOPN(float, float);
        break;
    case IM_BANDFMT_DOUBLE:
        LOOPN(double, double);
        break;
    case IM_BANDFMT_COMPLEX:
        LOOPCMPLXN(float, float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        LOOPCMPLXN(double, double);
        break;

    default:
        assert(0);
    }

    return (0);
}

#define LOOPNV(IN, OUT)                      \
    {                                        \
        IN *p = (IN *)in;                    \
        OUT *q = (OUT *)out;                 \
                                             \
        for (i = 0, x = 0; x < width; x++) { \
            OUT v = p[x];                    \
                                             \
            for (k = 0; k < nb; k++, i++)    \
                q[i] = a[k] * v + b[k];      \
        }                                    \
    }

#define LOOPCMPLXNV(IN, OUT)             \
    {                                    \
        IN *p = (IN *)in;                \
        OUT *q = (OUT *)out;             \
                                         \
        for (x = 0; x < width; x++) {    \
            OUT p0 = p[0];               \
            OUT p1 = p[1];               \
                                         \
            for (k = 0; k < nb; k++) {   \
                q[0] = a[k] * p0 + b[k]; \
                q[1] = a[k] * p1;        \
                q += 2;                  \
            }                            \
                                         \
            p += 2;                      \
        }                                \
    }

static int lintranv_gen(PEL *in, PEL *out, int width, IMAGE *im, LintraInfo *inf) {
    double *a = inf->a;
    double *b = inf->b;
    int nb = inf->n;
    int i, x, k;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOPNV(unsigned char, float);
        break;
    case IM_BANDFMT_CHAR:
        LOOPNV(signed char, float);
        break;
    case IM_BANDFMT_USHORT:
        LOOPNV(unsigned short, float);
        break;
    case IM_BANDFMT_SHORT:
        LOOPNV(signed short, float);
        break;
    case IM_BANDFMT_UINT:
        LOOPNV(unsigned int, float);
        break;
    case IM_BANDFMT_INT:
        LOOPNV(signed int, float);
        break;
    case IM_BANDFMT_FLOAT:
        LOOPNV(float, float);
        break;
    case IM_BANDFMT_DOUBLE:
        LOOPNV(double, double);
        break;
    case IM_BANDFMT_COMPLEX:
        LOOPCMPLXNV(float, float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        LOOPCMPLXNV(double, double);
        break;

    default:
        assert(0);
    }

    return (0);
}

int im_lintra_vec(int n, double *a, IMAGE *in, double *b, IMAGE *out) {
    LintraInfo *inf;
    int i;

    if (im_piocheck(in, out) || im_check_vector("im_lintra_vec", n, in) || im_check_uncoded("lintra_vec", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (vips_bandfmt_isint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_FLOAT;
    }
    if (in->Bands == 1) {
        out->Bands = n;
    }

    if (!(inf = IM_NEW(out, LintraInfo)) || !(inf->a = IM_ARRAY(out, n, double)) || !(inf->b = IM_ARRAY(out, n, double))) {
        return (-1);
    }
    inf->n = n;
    for (i = 0; i < n; i++) {
        inf->a[i] = a[i];
        inf->b[i] = b[i];
    }

    if (n == 1) {
        if (im_wrapone(in, out, (im_wrapone_fn)lintra1_gen, in, inf)) {
            return (-1);
        }
    } else if (in->Bands == 1) {
        if (im_wrapone(in, out, (im_wrapone_fn)lintranv_gen, in, inf)) {
            return (-1);
        }
    } else {
        if (im_wrapone(in, out, (im_wrapone_fn)lintran_gen, in, inf)) {
            return (-1);
        }
    }

    return (0);
}

int im_lintra(double a, IMAGE *in, double b, IMAGE *out) {
    return (im_lintra_vec(1, &a, in, &b, out));
}
