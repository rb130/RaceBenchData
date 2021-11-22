

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

typedef struct _Maxposavg {
    int xpos;
    int ypos;
    double max;
    int occurences;
} Maxposavg;

static void *maxposavg_start(IMAGE *in, void *a, void *b) {
    Maxposavg *global_maxposavg = (Maxposavg *)b;
    Maxposavg *maxposavg;

    if (!(maxposavg = IM_NEW(NULL, Maxposavg))) {
        return (NULL);
    }
    *maxposavg = *global_maxposavg;

    return ((void *)maxposavg);
}

static int maxposavg_stop(void *seq, void *a, void *b) {
    Maxposavg *global_maxposavg = (Maxposavg *)b;
    Maxposavg *maxposavg = (Maxposavg *)seq;

    if (maxposavg->max > global_maxposavg->max) {
        *global_maxposavg = *maxposavg;
    }

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)                                    \
    {                                                 \
        TYPE *p = (TYPE *)in;                         \
        TYPE m;                                       \
                                                      \
        m = max;                                      \
                                                      \
        for (x = 0; x < sz; x++) {                    \
            TYPE v = p[x];                            \
                                                      \
            if (v == m) {                             \
                xpos += r->left + x / reg->im->Bands; \
                ypos += r->top + y;                   \
                occurences += 1;                      \
            } else if (v > m) {                       \
                m = v;                                \
                xpos = r->left + x / reg->im->Bands;  \
                ypos = r->top + y;                    \
                occurences = 1;                       \
            }                                         \
        }                                             \
                                                      \
        max = m;                                      \
    }

#define CLOOP(TYPE)                                   \
    {                                                 \
        TYPE *p = (TYPE *)in;                         \
                                                      \
        for (x = 0; x < sz; x++) {                    \
            double mod, re, im;                       \
                                                      \
            re = p[0];                                \
            im = p[1];                                \
            p += 2;                                   \
            mod = re * re + im * im;                  \
                                                      \
            if (mod == max) {                         \
                xpos += r->left + x / reg->im->Bands; \
                ypos += r->top + y;                   \
                occurences += 1;                      \
            } else if (mod > max) {                   \
                max = mod;                            \
                xpos = r->left + x / reg->im->Bands;  \
                ypos = r->top + y;                    \
                occurences = 1;                       \
            }                                         \
        }                                             \
    }

static int maxposavg_scan(REGION *reg, void *seq, void *a, void *b) {
    const Rect *r = &reg->valid;
    const int sz = IM_REGION_N_ELEMENTS(reg);
    Maxposavg *maxposavg = (Maxposavg *)seq;

    int x, y;
    double max;
    int xpos, ypos, occurences;

    xpos = maxposavg->xpos;
    ypos = maxposavg->ypos;
    max = maxposavg->max;
    occurences = maxposavg->occurences;

    for (y = 0; y < r->height; y++) {
        PEL *in = (PEL *)IM_REGION_ADDR(reg, r->left, r->top + y);

        switch (reg->im->BandFmt) {
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
            LOOP(double);
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
    }

    maxposavg->xpos = xpos;
    maxposavg->ypos = ypos;
    maxposavg->max = max;
    maxposavg->occurences = occurences;

    return (0);
}

int im_maxpos_avg(IMAGE *in, double *xpos, double *ypos, double *out) {
    Maxposavg *global_maxposavg;

    if (im_pincheck(in) || im_check_uncoded("im_maxpos_avg", in)) {
        return (-1);
    }

    if (!(global_maxposavg = IM_NEW(in, Maxposavg))) {
        return (-1);
    }
    if (im__value(in, &global_maxposavg->max)) {
        return (-1);
    }
    global_maxposavg->xpos = 0;
    global_maxposavg->ypos = 0;
    global_maxposavg->occurences = 1;

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_maxposavg->max *= global_maxposavg->max;
    }

    if (vips_sink(in, maxposavg_start, maxposavg_scan, maxposavg_stop, in, global_maxposavg)) {
        return (-1);
    }

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_maxposavg->max = sqrt(global_maxposavg->max);
    }

    if (xpos) {
        *xpos = (double)global_maxposavg->xpos / global_maxposavg->occurences;
    }
    if (ypos) {
        *ypos = (double)global_maxposavg->ypos / global_maxposavg->occurences;
    }
    if (out) {
        *out = global_maxposavg->max;
    }

    return (0);
}
