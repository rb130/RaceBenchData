
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define ILOOP(TYPE, STYPE)                                           \
    {                                                                \
        TYPE *p1 = (TYPE *)p;                                        \
        TYPE *q1 = (TYPE *)q;                                        \
                                                                     \
        for (i = 0; i < sz; i++) {                                   \
            STYPE sum;                                               \
                                                                     \
            sum = 0;                                                 \
            for (j = 0; j < b; j++)                                  \
                sum += p1[j];                                        \
            q1[i] = sum > 0 ? (sum + b / 2) / b : (sum - b / 2) / b; \
            p1 += b;                                                 \
        }                                                            \
    }

#define FLOOP(TYPE)                 \
    {                               \
        TYPE *p1 = (TYPE *)p;       \
        TYPE *q1 = (TYPE *)q;       \
                                    \
        for (i = 0; i < sz; i++) {  \
            TYPE sum;               \
                                    \
            sum = 0;                \
            for (j = 0; j < b; j++) \
                sum += p1[j];       \
            q1[i] = sum / b;        \
            p1 += b;                \
        }                           \
    }

static void bandmean_buffer(PEL *p, PEL *q, int n, IMAGE *in) {

    const int sz = n * (vips_bandfmt_iscomplex(in->BandFmt) ? 2 : 1);
    const int b = in->Bands;

    int i, j;

    switch (in->BandFmt) {
    case IM_BANDFMT_CHAR:
        ILOOP(signed char, int);
        break;
    case IM_BANDFMT_UCHAR:
        ILOOP(unsigned char, unsigned int);
        break;
    case IM_BANDFMT_SHORT:
        ILOOP(signed short, int);
        break;
    case IM_BANDFMT_USHORT:
        ILOOP(unsigned short, unsigned int);
        break;
    case IM_BANDFMT_INT:
        ILOOP(signed int, int);
        break;
    case IM_BANDFMT_UINT:
        ILOOP(unsigned int, unsigned int);
        break;
    case IM_BANDFMT_FLOAT:
        FLOOP(float);
        break;
    case IM_BANDFMT_DOUBLE:
        FLOOP(double);
        break;
    case IM_BANDFMT_COMPLEX:
        FLOOP(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        FLOOP(double);
        break;

    default:
        assert(0);
    }
}

int im_bandmean(IMAGE *in, IMAGE *out) {

    if (in->Bands == 1) {
        return (im_copy(in, out));
    }
    if (im_check_uncoded("im_bandmean", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 1;
    out->Type = IM_TYPE_B_W;

    if (im_wrapone(in, out, (im_wrapone_fn)bandmean_buffer, in, NULL)) {
        return (-1);
    }

    return (0);
}
