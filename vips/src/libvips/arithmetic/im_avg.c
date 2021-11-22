

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

typedef struct _Wrapscan {
    IMAGE *in;
    im_start_fn start;
    im__wrapscan_fn scan;
    im_stop_fn stop;
    void *a;
    void *b;
} Wrapscan;

static void *wrapscan_start(VipsImage *in, void *a, void *b) {
    Wrapscan *wrapscan = (Wrapscan *)a;

    return (wrapscan->start(in, wrapscan->a, wrapscan->b));
}

static int wrapscan_stop(void *seq, void *a, void *b) {
    Wrapscan *wrapscan = (Wrapscan *)a;

    return (wrapscan->stop(seq, wrapscan->a, wrapscan->b));
}

static int wrapscan_scan(REGION *reg, void *seq, void *a, void *b) {
    Wrapscan *wrapscan = (Wrapscan *)a;
    Rect *r = &reg->valid;
    int lsk = IM_REGION_LSKIP(reg);

    int y;
    PEL *p;

    p = (PEL *)IM_REGION_ADDR(reg, r->left, r->top);
    for (y = 0; y < r->height; y++) {
        if (wrapscan->scan(p, r->width, seq, wrapscan->a, wrapscan->b)) {
            return (-1);
        }
        p += lsk;
    }

    return (0);
}

int im__wrapscan(VipsImage *in, VipsStart start, im__wrapscan_fn scan, VipsStop stop, void *a, void *b) {
    Wrapscan wrapscan;

    wrapscan.in = in;
    wrapscan.start = start;
    wrapscan.scan = scan;
    wrapscan.stop = stop;
    wrapscan.a = a;
    wrapscan.b = b;

    return (vips_sink(in, wrapscan_start, wrapscan_scan, wrapscan_stop, &wrapscan, NULL));
}

static void *avg_start(IMAGE *out, void *a, void *b) {
    double *sum;

    if (!(sum = IM_NEW(NULL, double))) {
        return (NULL);
    }
    *sum = 0.0;

    return ((void *)sum);
}

static int avg_stop(void *seq, void *a, void *b) {
    double *sum = (double *)seq;
    double *global_sum = (double *)b;

    *global_sum += *sum;

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)               \
    {                            \
        TYPE *p = (TYPE *)in;    \
                                 \
        for (x = 0; x < sz; x++) \
            m += p[x];           \
    }

#define CLOOP(TYPE)                  \
    {                                \
        TYPE *p = (TYPE *)in;        \
                                     \
        for (x = 0; x < sz; x++) {   \
            double mod, re, im;      \
                                     \
            re = p[0];               \
            im = p[1];               \
            p += 2;                  \
            mod = re * re + im * im; \
                                     \
            m += mod;                \
        }                            \
    }

static int avg_scan(void *in, int n, void *seq, void *a, void *b) {
    const IMAGE *im = (IMAGE *)a;
    const int sz = n * im->Bands;

    double *sum = (double *)seq;

    int x;
    double m;

    m = *sum;

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

        oil_sum_f64(&t, p, sizeof(double), sz);

        m += t;
    }
#else
        LOOP(double);
#endif
    break;

    case IM_BANDFMT_COMPLEX:
        CLOOP(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        CLOOP(double);
        break;

    default:
        g_assert(0);
    }

    *sum = m;

    return (0);
}

int im_avg(IMAGE *in, double *out) {
    double global_sum;
    gint64 vals, pels;

    if (im_pincheck(in) || im_check_noncomplex("im_avg", in) || im_check_uncoded("im_avg", in)) {
        return (-1);
    }

    global_sum = 0.0;
    if (im__wrapscan(in, avg_start, avg_scan, avg_stop, in, &global_sum)) {
        return (-1);
    }

    pels = (gint64)in->Xsize * in->Ysize;
    vals = pels * in->Bands;
    *out = global_sum / vals;
    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        *out = sqrt(*out);
    }

    return (0);
}

int im__value(IMAGE *im, double *value) {
    IMAGE *t;

    if (!(t = im_open_local(im, "im__value", "p")) || im_extract_areabands(im, t, 0, 0, 1, 1, 0, 1) || im_avg(t, value)) {
        return (-1);
    }

    return (0);
}
