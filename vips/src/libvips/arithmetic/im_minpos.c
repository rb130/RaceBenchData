

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

typedef struct _Minpos {
    int xpos;
    int ypos;
    double min;
} Minpos;

static void *minpos_start(IMAGE *in, void *a, void *b) {
    Minpos *global_minpos = (Minpos *)b;
    Minpos *minpos;

    if (!(minpos = IM_NEW(NULL, Minpos))) {
        return (NULL);
    }
    *minpos = *global_minpos;

    return ((void *)minpos);
}

static int minpos_stop(void *seq, void *a, void *b) {
    Minpos *global_minpos = (Minpos *)b;
    Minpos *minpos = (Minpos *)seq;

    if (minpos->min < global_minpos->min) {
        *global_minpos = *minpos;
    }

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)                                   \
    {                                                \
        TYPE *p = (TYPE *)in;                        \
        TYPE m;                                      \
                                                     \
        m = min;                                     \
                                                     \
        for (x = 0; x < sz; x++) {                   \
            TYPE v = p[x];                           \
                                                     \
            if (v < m) {                             \
                m = v;                               \
                xpos = r->left + x / reg->im->Bands; \
                ypos = r->top + y;                   \
            }                                        \
        }                                            \
                                                     \
        min = m;                                     \
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
            if (mod < min) {                         \
                min = mod;                           \
                xpos = r->left + x / reg->im->Bands; \
                ypos = r->top + y;                   \
            }                                        \
        }                                            \
    }

static int minpos_scan(REGION *reg, void *seq, void *a, void *b) {
    const Rect *r = &reg->valid;
    const int sz = IM_REGION_N_ELEMENTS(reg);
    Minpos *minpos = (Minpos *)seq;

    int x, y;
    double min;
    int xpos, ypos;

    xpos = minpos->xpos;
    ypos = minpos->ypos;
    min = minpos->min;

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

    minpos->xpos = xpos;
    minpos->ypos = ypos;
    minpos->min = min;

    return (0);
}

int im_minpos(IMAGE *in, int *xpos, int *ypos, double *out) {
    Minpos *global_minpos;

    if (im_pincheck(in) || im_check_uncoded("im_minpos", in)) {
        return (-1);
    }

    if (!(global_minpos = IM_NEW(in, Minpos))) {
        return (-1);
    }
    if (im__value(in, &global_minpos->min)) {
        return (-1);
    }
    global_minpos->xpos = 0;
    global_minpos->ypos = 0;

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_minpos->min *= global_minpos->min;
    }

    if (vips_sink(in, minpos_start, minpos_scan, minpos_stop, in, global_minpos)) {
        return (-1);
    }

    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        global_minpos->min = sqrt(global_minpos->min);
    }

    if (xpos) {
        *xpos = global_minpos->xpos;
    }
    if (ypos) {
        *ypos = global_minpos->ypos;
    }
    if (out) {
        *out = global_minpos->min;
    }

    return (0);
}

int im_min(IMAGE *in, double *out) {
    return (im_minpos(in, NULL, NULL, out));
}
