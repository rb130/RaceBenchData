

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define BRIGHT 255
#define DARK 0
#define im_for_all_types(A) \
    case IM_BANDFMT_UCHAR:  \
        A(unsigned char);   \
        break;              \
    case IM_BANDFMT_CHAR:   \
        A(signed char);     \
        break;              \
    case IM_BANDFMT_USHORT: \
        A(unsigned short);  \
        break;              \
    case IM_BANDFMT_SHORT:  \
        A(signed short);    \
        break;              \
    case IM_BANDFMT_UINT:   \
        A(unsigned int);    \
        break;              \
    case IM_BANDFMT_INT:    \
        A(signed int);      \
        break;              \
    case IM_BANDFMT_FLOAT:  \
        A(float);           \
        break;              \
    case IM_BANDFMT_DOUBLE: \
        A(double);          \
        break;

int im_thresh(in, out, threshold)
IMAGE *in, *out;
double threshold;
{
    int x, y;
    PEL *bu;
    int s, epl;

    if (im_iocheck(in, out)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE) {
        im_error("im_thresh", "%s", _("input should be uncoded"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UCHAR;
    if (im_setupout(out)) {
        return (-1);
    }

    epl = in->Xsize * in->Bands;
    s = epl * sizeof(PEL);
    if ((bu = (PEL *)im_malloc(out, (unsigned)s)) == NULL) {
        return (-1);
    }

#define im_thresh_loop(TYPE)              \
    {                                     \
        TYPE *a = (TYPE *)in->data;       \
                                          \
        for (y = 0; y < in->Ysize; y++) { \
            PEL *b = bu;                  \
                                          \
            for (x = 0; x < epl; x++) {   \
                double f = (double)*a++;  \
                if (f >= threshold)       \
                    *b++ = (PEL)BRIGHT;   \
                else                      \
                    *b++ = (PEL)DARK;     \
            }                             \
                                          \
            if (im_writeline(y, out, bu)) \
                return (-1);              \
        }                                 \
    }

    switch (in->BandFmt) {
        im_for_all_types(im_thresh_loop);

    default:
        im_error("im_thresh", "%s", _("Unknown input format"));
        return (-1);
    }

    return (0);
}
