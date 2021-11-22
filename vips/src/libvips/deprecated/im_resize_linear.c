

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

#define LOOP(TYPE)                                                                                  \
    if (Xint >= 0 && Yint >= 0 && Xint < in->Xsize - 1 && Yint < in->Ysize - 1)                     \
        for (bb = 0; bb < in->Bands; bb++) {                                                        \
            TYPE s1 = *((TYPE *)p);                                                                 \
            TYPE s2 = *((TYPE *)(p + ips));                                                         \
            TYPE s3 = *((TYPE *)(p + ils));                                                         \
            TYPE s4 = *((TYPE *)(p + ips + ils));                                                   \
            TYPE *t = (TYPE *)q;                                                                    \
                                                                                                    \
            *t = (1 - dx) * (1 - dy) * s1 + dx * (1 - dy) * s2 + dy * (1 - dx) * s3 + dx * dy * s4; \
                                                                                                    \
            p += ies;                                                                               \
            q += oes;                                                                               \
        }                                                                                           \
    else if (Xint == in->Xsize - 1 && Yint >= 0 && Yint < in->Ysize - 1)                            \
        for (bb = 0; bb < in->Bands; bb++) {                                                        \
            TYPE s1 = *((TYPE *)p);                                                                 \
            TYPE s3 = *((TYPE *)(p + ils));                                                         \
            TYPE *t = (TYPE *)q;                                                                    \
                                                                                                    \
            *t = (1 - dy) * s1 + dy * s3;                                                           \
                                                                                                    \
            p += ies;                                                                               \
            q += oes;                                                                               \
        }                                                                                           \
    else if (Yint == in->Ysize - 1 && Xint >= 0 && Xint < in->Xsize - 1)                            \
        for (bb = 0; bb < in->Bands; bb++) {                                                        \
            TYPE s1 = *((TYPE *)p);                                                                 \
            TYPE s2 = *((TYPE *)(p + ips));                                                         \
            TYPE *t = (TYPE *)q;                                                                    \
                                                                                                    \
            *t = (1 - dx) * s1 + dx * s2;                                                           \
                                                                                                    \
            p += ies;                                                                               \
            q += oes;                                                                               \
        }                                                                                           \
    else                                                                                            \
        for (bb = 0; bb < in->Bands; bb++) {                                                        \
            unsigned char s1 = *((unsigned char *)p);                                               \
            TYPE *t = (TYPE *)q;                                                                    \
                                                                                                    \
            *t = s1;                                                                                \
                                                                                                    \
            p += ies;                                                                               \
            q += oes;                                                                               \
        }

int im_resize_linear(IMAGE *in, IMAGE *out, int X, int Y) {
    double dx, dy, xscale, yscale;
    double Xnew, Ynew;

    int x, y;
    int Xint, Yint;
    int bb;

    PEL *input, *opline;
    PEL *q, *p;

    int ils, ips, ies;
    int ols, ops, oes;

    if (im_iocheck(in, out)) {
        return (-1);
    }
    if (vips_bandfmt_iscomplex(in->BandFmt)) {
        im_error("im_lowpass", "%s", _("non-complex input only"));
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE) {
        im_error("im_lowpass: ", "%s", _("put should be uncoded"));
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }

    out->Xsize = X;
    out->Ysize = Y;

    if (im_setupout(out)) {
        return (-1);
    }

    ils = IM_IMAGE_SIZEOF_LINE(in);
    ips = IM_IMAGE_SIZEOF_PEL(in);
    ies = IM_IMAGE_SIZEOF_ELEMENT(in);

    ols = IM_IMAGE_SIZEOF_LINE(out);
    ops = IM_IMAGE_SIZEOF_PEL(out);
    oes = IM_IMAGE_SIZEOF_ELEMENT(out);

    if (!(opline = IM_ARRAY(out, ols, PEL))) {
        return (-1);
    }

    input = (PEL *)in->data;
    xscale = ((double)in->Xsize - 1) / (X - 1);
    yscale = ((double)in->Ysize - 1) / (Y - 1);

    for (y = 0; y < Y; y++) {
        q = opline;
        for (x = 0; x < X; x++) {
            Xnew = x * xscale;
            Ynew = y * yscale;
            Xint = floor(Xnew);
            Yint = floor(Ynew);
            dx = Xnew - Xint;
            dy = Ynew - Yint;
            p = input + Xint * ips + Yint * ils;

            switch (in->BandFmt) {
            case IM_BANDFMT_UCHAR:
                LOOP(unsigned char);
                break;
            case IM_BANDFMT_USHORT:
                LOOP(unsigned short);
                break;
            case IM_BANDFMT_UINT:
                LOOP(unsigned int);
                break;
            case IM_BANDFMT_CHAR:
                LOOP(signed char);
                break;
            case IM_BANDFMT_SHORT:
                LOOP(signed short);
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

            default:
                im_error("im_lowpass", "%s", _("unsupported image type"));
                return (-1);
            }
        }

        if (im_writeline(y, out, opline)) {
            return (-1);
        }
    }
    return (0);
}
