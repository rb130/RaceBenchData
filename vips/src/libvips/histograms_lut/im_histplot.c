

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int normalise(IMAGE *in, IMAGE *out) {
    if (im_check_uncoded("im_histplot", in) || im_check_noncomplex("im_histplot", in)) {
        return (-1);
    }

    if (vips_bandfmt_isuint(in->BandFmt)) {
        if (im_copy(in, out)) {
            return (-1);
        }
    } else if (vips_bandfmt_isint(in->BandFmt)) {
        IMAGE *t1;
        double min;

        if (!(t1 = im_open_local(out, "im_histplot", "p")) || im_min(in, &min) || im_lintra(1.0, in, -min, t1)) {
            return (-1);
        }
    } else {

        IMAGE *t1;
        DOUBLEMASK *stats;
        double min, max;
        int any;

        if (in->Xsize == 1) {
            any = in->Ysize;
        } else {
            any = in->Xsize;
        }

        if (!(stats = im_stats(in))) {
            return (-1);
        }
        min = stats->coeff[0];
        max = stats->coeff[1];
        im_free_dmask(stats);

        if (!(t1 = im_open_local(out, "im_histplot", "p")) || im_lintra(any / (max - min), in, -min * any / (max - min), out)) {
            return (-1);
        }
    }

    return (0);
}

#define VERT(TYPE)                                  \
    {                                               \
        TYPE *p1 = (TYPE *)p;                       \
                                                    \
        for (x = le; x < ri; x++) {                 \
            for (z = 0; z < nb; z++)                \
                q[z] = p1[z] < ((TYPE)x) ? 0 : 255; \
                                                    \
            q += nb;                                \
        }                                           \
    }

static int make_vert_gen(REGION * or, void *seq, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int ri = IM_RECT_RIGHT(r);
    int bo = IM_RECT_BOTTOM(r);
    int nb = in->Bands;

    int x, y, z;

    for (y = to; y < bo; y++) {
        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);
        PEL *p = (PEL *)IM_IMAGE_ADDR(in, 0, y);

        switch (in->BandFmt) {
        case IM_BANDFMT_UCHAR:
            VERT(unsigned char);
            break;
        case IM_BANDFMT_CHAR:
            VERT(signed char);
            break;
        case IM_BANDFMT_USHORT:
            VERT(unsigned short);
            break;
        case IM_BANDFMT_SHORT:
            VERT(signed short);
            break;
        case IM_BANDFMT_UINT:
            VERT(unsigned int);
            break;
        case IM_BANDFMT_INT:
            VERT(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            VERT(float);
            break;
        case IM_BANDFMT_DOUBLE:
            VERT(double);
            break;

        default:
            g_assert(0);
        }
    }

    return (0);
}

#define HORZ(TYPE)                                         \
    {                                                      \
        TYPE *p1 = (TYPE *)p;                              \
                                                           \
        for (y = to; y < bo; y++) {                        \
            for (z = 0; z < nb; z++)                       \
                q[z] = p1[z] < ((TYPE)(ht - y)) ? 0 : 255; \
                                                           \
            q += lsk;                                      \
        }                                                  \
    }

static int make_horz_gen(REGION * or, void *seq, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int ri = IM_RECT_RIGHT(r);
    int bo = IM_RECT_BOTTOM(r);
    int nb = in->Bands;
    int lsk = IM_REGION_LSKIP(or);
    int ht = or->im->Ysize;

    int x, y, z;

    for (x = le; x < ri; x++) {
        PEL *q = (PEL *)IM_REGION_ADDR(or, x, to);
        PEL *p = (PEL *)IM_IMAGE_ADDR(in, x, 0);

        switch (in->BandFmt) {
        case IM_BANDFMT_UCHAR:
            HORZ(unsigned char);
            break;
        case IM_BANDFMT_CHAR:
            HORZ(signed char);
            break;
        case IM_BANDFMT_USHORT:
            HORZ(unsigned short);
            break;
        case IM_BANDFMT_SHORT:
            HORZ(signed short);
            break;
        case IM_BANDFMT_UINT:
            HORZ(unsigned int);
            break;
        case IM_BANDFMT_INT:
            HORZ(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            HORZ(float);
            break;
        case IM_BANDFMT_DOUBLE:
            HORZ(double);
            break;

        default:
            g_assert(0);
        }
    }

    return (0);
}

static int plot(IMAGE *in, IMAGE *out) {
    double max;
    int tsize;
    int xsize;
    int ysize;

    if (im_incheck(in) || im_poutcheck(out)) {
        return (-1);
    }

    if (im_max(in, &max)) {
        return (-1);
    }
    g_assert(max >= 0);
    if (in->BandFmt == IM_BANDFMT_UCHAR) {
        tsize = 256;
    } else {
        tsize = ceil(max);
    }

    if (tsize == 0) {
        tsize = 1;
    }

    if (in->Xsize == 1) {

        xsize = tsize;
        ysize = in->Ysize;
    } else {

        xsize = in->Xsize;
        ysize = tsize;
    }

    im_initdesc(out, xsize, ysize, in->Bands, IM_BBITS_BYTE, IM_BANDFMT_UCHAR, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);

    if (im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (in->Xsize == 1) {
        if (im_generate(out, NULL, make_vert_gen, NULL, in, NULL)) {
            return (-1);
        }
    } else {
        if (im_generate(out, NULL, make_horz_gen, NULL, in, NULL)) {
            return (-1);
        }
    }

    return (0);
}

int im_histplot(IMAGE *hist, IMAGE *histplot) {
    IMAGE *t1;

    if (im_check_hist("im_histplot", hist)) {
        return (-1);
    }

    if (!(t1 = im_open_local(histplot, "im_histplot:1", "p")) || normalise(hist, t1) || plot(t1, histplot)) {
        return (-1);
    }

    return (0);
}
