
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define im_for_all_types()     \
    case IM_BANDFMT_UCHAR:     \
        loopuc(unsigned char); \
        break;                 \
    case IM_BANDFMT_CHAR:      \
        loop(char);            \
        break;                 \
    case IM_BANDFMT_USHORT:    \
        loop(unsigned short);  \
        break;                 \
    case IM_BANDFMT_SHORT:     \
        loop(short);           \
        break;                 \
    case IM_BANDFMT_UINT:      \
        loop(unsigned int);    \
        break;                 \
    case IM_BANDFMT_INT:       \
        loop(int);             \
        break;                 \
    case IM_BANDFMT_FLOAT:     \
        loop(float);           \
        break;                 \
    case IM_BANDFMT_DOUBLE:    \
        loop(double);          \
        break;                 \
    case IM_BANDFMT_COMPLEX:   \
        loopcmplx(float);      \
        break;                 \
    case IM_BANDFMT_DPCOMPLEX: \
        loopcmplx(double);     \
        break;

int im_printlines(IMAGE *in) {
    if (im_incheck(in)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE) {
        im_error("im_printlines", "%s", _("input must be uncoded"));
        return (-1);
    }
    if (!in->data) {
        im_error("im_debugim", "%s", _("unsuitable image type"));
        return (-1);
    }

#define loopuc(TYPE)                                      \
    {                                                     \
        TYPE *p = (TYPE *)in->data;                       \
        int x, y, z;                                      \
                                                          \
        for (y = 0; y < in->Ysize; y++) {                 \
            fprintf(stderr, "line:%5d\n", y);             \
            for (x = 0; x < in->Xsize; x++) {             \
                fprintf(stderr, "%5d", x);                \
                for (z = 0; z < in->Bands; z++) {         \
                    fprintf(stderr, "\t%4d", (TYPE)*p++); \
                }                                         \
                fprintf(stderr, "\n");                    \
            }                                             \
        }                                                 \
    }

#define loop(TYPE)                                         \
    {                                                      \
        TYPE *p = (TYPE *)in->data;                        \
        int x, y, z;                                       \
                                                           \
        for (y = 0; y < in->Ysize; y++) {                  \
            fprintf(stderr, "line:%5d\n", y);              \
            for (x = 0; x < in->Xsize; x++) {              \
                fprintf(stderr, "%5d", x);                 \
                for (z = 0; z < in->Bands; z++) {          \
                    fprintf(stderr, "\t%f", (double)*p++); \
                }                                          \
                fprintf(stderr, "\n");                     \
            }                                              \
        }                                                  \
    }

#define loopcmplx(TYPE)                                    \
    {                                                      \
        TYPE *p = (TYPE *)in->data;                        \
        int x, y, z;                                       \
                                                           \
        for (y = 0; y < in->Ysize; y++) {                  \
            fprintf(stderr, "line:%5d\n", y);              \
            for (x = 0; x < in->Xsize; x++) {              \
                fprintf(stderr, "%5d", x);                 \
                for (z = 0; z < in->Bands; z++) {          \
                    fprintf(stderr, "\t%f", (double)*p++); \
                    fprintf(stderr, "\t%f", (double)*p++); \
                }                                          \
                fprintf(stderr, "\n");                     \
            }                                              \
        }                                                  \
    }

    switch (in->BandFmt) {
        im_for_all_types();
    default: {
        im_error("im_printlines", "%s", _("unknown input format"));
        return (-1);
    }
    }

    return (0);
}
