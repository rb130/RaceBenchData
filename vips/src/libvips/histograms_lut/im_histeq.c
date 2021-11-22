

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define ACCUMULATE(ITYPE, OTYPE)           \
    {                                      \
        for (b = 0; b < nb; b++) {         \
            ITYPE *p = (ITYPE *)in->data;  \
            OTYPE *q = (OTYPE *)outbuf;    \
            OTYPE total;                   \
                                           \
            total = 0;                     \
            for (x = b; x < mx; x += nb) { \
                total += p[x];             \
                q[x] = total;              \
            }                              \
        }                                  \
    }

int im_histcum(IMAGE *in, IMAGE *out) {
    const int px = in->Xsize * in->Ysize;
    const int nb = vips_bandfmt_iscomplex(in->BandFmt) ? in->Bands * 2 : in->Bands;
    const int mx = px * nb;

    PEL *outbuf;
    int b, x;

    if (im_check_uncoded("im_histcum", in) || im_check_hist("im_histcum", in) || im_iocheck(in, out)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = px;
    out->Ysize = 1;
    if (vips_bandfmt_isuint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_UINT;
    } else if (vips_bandfmt_isint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_INT;
    }
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(outbuf = im_malloc(out, IM_IMAGE_SIZEOF_LINE(out)))) {
        return (-1);
    }

    switch (in->BandFmt) {
    case IM_BANDFMT_CHAR:
        ACCUMULATE(signed char, signed int);
        break;
    case IM_BANDFMT_UCHAR:
        ACCUMULATE(unsigned char, unsigned int);
        break;
    case IM_BANDFMT_SHORT:
        ACCUMULATE(signed short, signed int);
        break;
    case IM_BANDFMT_USHORT:
        ACCUMULATE(unsigned short, unsigned int);
        break;
    case IM_BANDFMT_INT:
        ACCUMULATE(signed int, signed int);
        break;
    case IM_BANDFMT_UINT:
        ACCUMULATE(unsigned int, unsigned int);
        break;

    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_COMPLEX:
        ACCUMULATE(float, float);
        break;
    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_DPCOMPLEX:
        ACCUMULATE(double, double);
        break;

    default:
        g_assert(0);
    }

    if (im_writeline(0, out, outbuf)) {
        return (-1);
    }

    return (0);
}

int im_histnorm(IMAGE *in, IMAGE *out) {
    const int px = in->Xsize * in->Ysize;
    DOUBLEMASK *stats;
    double *a, *b;
    int i;
    IMAGE *t1;
    int fmt;

    if (!(a = IM_ARRAY(out, in->Bands, double)) || !(b = IM_ARRAY(out, in->Bands, double)) || !(stats = im_stats(in))) {
        return (-1);
    }

    for (i = 0; i < in->Bands; i++) {
        a[i] = px / stats->coeff[6 + 1 + 6 * i];
        b[i] = 0;
    }

    im_free_dmask(stats);

    if (!(t1 = im_open_local(out, "im_histnorm:2", "p")) || im_lintra_vec(in->Bands, a, in, b, t1)) {
        return (-1);
    }

    if (px <= 256) {
        fmt = IM_BANDFMT_UCHAR;
    } else if (px <= 65536) {
        fmt = IM_BANDFMT_USHORT;
    } else {
        fmt = IM_BANDFMT_UINT;
    }

    if (im_clip2fmt(t1, out, fmt)) {
        return (-1);
    }

    return (0);
}

int im_histeq(IMAGE *in, IMAGE *out) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_histeq", "p")) || im_histcum(in, t1) || im_histnorm(t1, out)) {
        return (-1);
    }

    return (0);
}
