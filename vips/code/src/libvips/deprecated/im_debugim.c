
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_debugim(IMAGE *in) {

    if (im_incheck(in)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE) {
        im_error("im_debugim", "%s", _("input must be uncoded"));
        return (-1);
    }

#define loopuc(TYPE)                                    \
    {                                                   \
        TYPE *p = (TYPE *)in->data;                     \
        int x, y, z;                                    \
                                                        \
        for (y = 0; y < in->Ysize; y++) {               \
            for (x = 0; x < in->Xsize; x++) {           \
                for (z = 0; z < in->Bands; z++) {       \
                    fprintf(stderr, "%4d", (TYPE)*p++); \
                }                                       \
            }                                           \
            fprintf(stderr, "\n");                      \
        }                                               \
    }

#define loop(TYPE)                                         \
    {                                                      \
        TYPE *p = (TYPE *)in->data;                        \
        int x, y, z;                                       \
                                                           \
        for (y = 0; y < in->Ysize; y++) {                  \
            for (x = 0; x < in->Xsize; x++) {              \
                for (z = 0; z < in->Bands; z++) {          \
                    fprintf(stderr, "%g\t", (double)*p++); \
                }                                          \
            }                                              \
            fprintf(stderr, "\n");                         \
        }                                                  \
    }

#define loopcmplx(TYPE)                                       \
    {                                                         \
        TYPE *p = (TYPE *)in->data;                           \
        int x, y, z;                                          \
                                                              \
        for (y = 0; y < in->Ysize; y++) {                     \
            for (x = 0; x < in->Xsize; x++) {                 \
                for (z = 0; z < in->Bands; z++) {             \
                    fprintf(stderr, "re=%g\t", (double)*p++); \
                    fprintf(stderr, "im=%g\t", (double)*p++); \
                }                                             \
            }                                                 \
            fprintf(stderr, "\n");                            \
        }                                                     \
    }

    switch (in->BandFmt) {
    case IM_BANDFMT_UCHAR:
        loopuc(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        loop(char);
        break;
    case IM_BANDFMT_USHORT:
        loop(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        loop(short);
        break;
    case IM_BANDFMT_UINT:
        loop(unsigned int);
        break;
    case IM_BANDFMT_INT:
        loop(int);
        break;
    case IM_BANDFMT_FLOAT:
        loop(float);
        break;
    case IM_BANDFMT_DOUBLE:
        loop(double);
        break;
    case IM_BANDFMT_COMPLEX:
        loopcmplx(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        loopcmplx(double);
        break;

    default:
        im_error("im_debugim", "%s", _("unknown input format"));
        return (-1);
    }

    return (0);
}
