
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef HAVE_LIBOIL
#include <liboil/liboil.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static void *deviate_start(IMAGE *out, void *a, void *b) {
    double *ss2;

    if (!(ss2 = IM_ARRAY(NULL, 2, double))) {
        return (NULL);
    }
    ss2[0] = 0.0;
    ss2[1] = 0.0;

    return ((void *)ss2);
}

static int deviate_stop(void *seq, void *a, void *b) {
    double *ss2 = (double *)seq;
    double *global_ss2 = (double *)b;

    global_ss2[0] += ss2[0];
    global_ss2[1] += ss2[1];

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)                       \
    {                                    \
        TYPE *p = (TYPE *)in;            \
                                         \
        for (x = 0; x < sz; x++) {       \
            TYPE v = p[x];               \
                                         \
            s += v;                      \
            s2 += (double)v * (double)v; \
        }                                \
    }

static int deviate_scan(void *in, int n, void *seq, void *a, void *b) {
    const IMAGE *im = (IMAGE *)a;
    const int sz = n * im->Bands;

    double *ss2 = (double *)seq;

    int x;
    double s, s2;

    s = ss2[0];
    s2 = ss2[1];

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        LOOP(signed char);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        LOOP(float);
        break;

    case IM_BANDFMT_DOUBLE:
#ifdef HAVE_LIBOIL
    {
        double *p = (double *)in;
        double t;
        double t2;

        oil_sum_f64(&t, p, sizeof(double), sz);
        oil_squaresum_f64(&t2, p, sz);

        s += t;
        s2 += t2;
    }
#else
        LOOP(double);
#endif
    break;

    default:
        g_assert(0);
    }

    ss2[0] = s;
    ss2[1] = s2;

    return (0);
}

int im_deviate(IMAGE *in, double *out) {
    double global_ss2[2];
    double s, s2;
    gint64 vals;

    if (im_pincheck(in) || im_check_uncoded("im_deviate", in) || im_check_noncomplex("im_deviate", in)) {
        return (-1);
    }

    global_ss2[0] = 0.0;
    global_ss2[1] = 0.0;
    if (im__wrapscan(in, deviate_start, deviate_scan, deviate_stop, in, global_ss2)) {
        return (-1);
    }

    vals = (gint64)in->Xsize * (gint64)in->Ysize * (gint64)in->Bands;
    s = global_ss2[0];
    s2 = global_ss2[1];
    *out = sqrt(fabs(s2 - (s * s / vals)) / (vals - 1));

    return (0);
}
