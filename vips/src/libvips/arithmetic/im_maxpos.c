

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

typedef struct _Maxpos {
    int xpos;
    int ypos;
    double max;
} Maxpos;

static void *maxpos_start(IMAGE *in, void *a, void *b) {
    Maxpos *global_maxpos = (Maxpos *)b;
    Maxpos *maxpos;

    if (!(maxpos = IM_NEW(NULL, Maxpos))) {
        return (NULL);
    }
    *maxpos = *global_maxpos;

    return ((void *)maxpos);
}

static int maxpos_stop(void *seq, void *a, void *b) {
    Maxpos *global_maxpos = (Maxpos *)b;
    Maxpos *maxpos = (Maxpos *)seq;

    if (maxpos->max > global_maxpos->max) {
        *global_maxpos = *maxpos;
    }

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)                                   \
    {                                                \
        TYPE *p = (TYPE *)in;                        \
        TYPE m;                                      \
                                                     \
        m = max;                                     \
                                                     \
        for (x = 0; x < sz; x++) {                   \
            TYPE v = p[x];                           \
                                                     \
            if (v > m) {                             \
                m = v;                               \
                xpos = r->left + x / reg->im->Bands; \
                ypos = r->top + y;                   \
            }                                        \
        }                                            \
                                                     \
        max = m;                                     \
    }

#define CLOOP(TYPE)                                  \
    {                                                \
        TYPE *p = (TYPE *)in;                        \
                                                     \
        for (x = 0; x < sz; x++) {                   \
            double mod, re, im;                      \
                                                     \
            re = p[0];                               \
            im = p[1];                               \
            p += 2;                                  \
            mod = re * re + im * im;                 \
                                                     \
            if (mod > max) {                         \
                max = mod;                           \
                xpos = r->left + x / reg->im->Bands; \
                ypos = r->top + y;                   \
            }                                        \
        }                                            \
    }

static int maxpos_scan(REGION *reg, void *seq, void *a, void *b) {
    const Rect *r = &reg->valid;
    const int sz = IM_REGION_N_ELEMENTS(reg);
    Maxpos *maxpos = (Maxpos *)seq;

    int x, y;
    double max;
    int xpos, ypos;

    xpos = maxpos->xpos;
    ypos = maxpos->ypos;
    max = maxpos->max;

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

    maxpos->xpos = xpos;
    maxpos->ypos = ypos;
    maxpos->max = max;

    return (0);
}

int im_maxpos(IMAGE *in, int *xpos, int *ypos, double *out) {
    Maxpos *global_maxpos;

    if (im_pincheck(in) || im_check_uncoded("im_maxpos", in)) {
        return (-1);
    }

    if (!(global_maxpos = IM_NEW(in, Maxpos))) {
        return (-1);
    }
    if (im__value(in, &global_maxpos->max)) {
        return (-1);
    }
    global_maxpos->xpos = 0;
    global_maxpos->ypos = 0;

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_maxpos->max *= global_maxpos->max;
    }

    if (vips_sink(in, maxpos_start, maxpos_scan, maxpos_stop, in, global_maxpos)) {
        return (-1);
    }

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_maxpos->max = sqrt(global_maxpos->max);
    }

    if (xpos) {
        *xpos = global_maxpos->xpos;
    }
    if (ypos) {
        *ypos = global_maxpos->ypos;
    }
    if (out) {
        *out = global_maxpos->max;
    }

    return (0);
}

int im_max(IMAGE *in, double *out) {
    return (im_maxpos(in, NULL, NULL, out));
}
