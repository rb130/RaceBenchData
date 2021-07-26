
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

#define BRIGHT 255
#define GREY 128
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
        break;

int im_slice(in, out, t1, t2) IMAGE *in, *out;
double t1, t2;
{
    int x, y, z;
    PEL *bu;
    int s, epl;
    double thresh1, thresh2;

    if (im_iocheck(in, out)) {
        im_error("im_slice", "%s", _("im_iocheck failed"));
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE) {
        im_error("im_slice", "%s", _("input should be uncoded"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        im_error("im_slice", "%s", _("im_cp_desc failed"));
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UCHAR;
    if (im_setupout(out)) {
        im_error("im_slice", "%s", _("im_setupout failed"));
        return (-1);
    }

    if (t1 <= t2) {
        thresh1 = t1;
        thresh2 = t2;
    } else {
        thresh1 = t2;
        thresh2 = t1;
    }

    epl = in->Xsize * in->Bands;
    s = epl * sizeof(PEL);
    if ((bu = (PEL *)im_malloc(out, (unsigned)s)) == NULL) {
        return (-1);
    }

#define im_slice_loop(TYPE)                       \
    {                                             \
        TYPE *a = (TYPE *)in->data;               \
                                                  \
        for (y = 0; y < in->Ysize; y++) {         \
            PEL *b = bu;                          \
                                                  \
            for (x = 0; x < in->Xsize; x++)       \
                for (z = 0; z < in->Bands; z++) { \
                    double f = (double)*a++;      \
                    if (f <= thresh1)             \
                        *b++ = (PEL)DARK;         \
                    else if (f > thresh2)         \
                        *b++ = (PEL)BRIGHT;       \
                    else                          \
                        *b++ = (PEL)GREY;         \
                }                                 \
                                                  \
            if (im_writeline(y, out, bu))         \
                return (-1);                      \
        }                                         \
    }

    switch (in->BandFmt) {
        im_for_all_types(im_slice_loop);

    default:
        im_error("im_slice", "%s", _("Unknown input format"));
        return (-1);
    }

    return (0);
}
