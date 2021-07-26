
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef HAVE_LIBOIL
#include <liboil/liboil.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define intabs(TYPE)               \
    {                              \
        TYPE *p = (TYPE *)in;      \
        TYPE *q = (TYPE *)out;     \
        int x;                     \
                                   \
        for (x = 0; x < sz; x++) { \
            TYPE v = p[x];         \
                                   \
            if (v < 0)             \
                q[x] = 0 - v;      \
            else                   \
                q[x] = v;          \
        }                          \
    }

#define floatabs(TYPE)           \
    {                            \
        TYPE *p = (TYPE *)in;    \
        TYPE *q = (TYPE *)out;   \
        int x;                   \
                                 \
        for (x = 0; x < sz; x++) \
            q[x] = fabs(p[x]);   \
    }

#ifdef HAVE_HYPOT

#define complexabs(TYPE)              \
    {                                 \
        TYPE *p = (TYPE *)in;         \
        TYPE *q = (TYPE *)out;        \
        int x;                        \
                                      \
        for (x = 0; x < sz; x++) {    \
            q[x] = hypot(p[0], p[1]); \
            p += 2;                   \
        }                             \
    }

#else

#define complexabs(TYPE)                                 \
    {                                                    \
        TYPE *p = (TYPE *)in;                            \
        TYPE *q = (TYPE *)out;                           \
        int x;                                           \
                                                         \
        for (x = 0; x < sz; x++) {                       \
            double rp = p[0];                            \
            double ip = p[1];                            \
            double abs_rp = fabs(rp);                    \
            double abs_ip = fabs(ip);                    \
                                                         \
            if (abs_rp > abs_ip) {                       \
                double temp = ip / rp;                   \
                                                         \
                q[x] = abs_rp * sqrt(1.0 + temp * temp); \
            } else {                                     \
                double temp = rp / ip;                   \
                                                         \
                q[x] = abs_ip * sqrt(1.0 + temp * temp); \
            }                                            \
                                                         \
            p += 2;                                      \
        }                                                \
    }

#endif

static void abs_gen(PEL *in, PEL *out, int width, IMAGE *im) {
    int sz = width * im->Bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_CHAR:
#ifdef HAVE_LIBOIL
        oil_abs_u8_s8((uint8_t *)out, sizeof(uint8_t), (int8_t *)in, sizeof(int8_t), sz);
#else
        intabs(signed char);
#endif
        break;

    case IM_BANDFMT_SHORT:
#ifdef HAVE_LIBOIL
        oil_abs_u16_s16((uint16_t *)out, sizeof(uint16_t), (int16_t *)in, sizeof(int16_t), sz);
#else
        intabs(signed short);
#endif
        break;

    case IM_BANDFMT_INT:
#ifdef HAVE_LIBOIL
        oil_abs_u32_s32((uint32_t *)out, sizeof(uint32_t), (int32_t *)in, sizeof(int32_t), sz);
#else
        intabs(signed int);
#endif
        break;

    case IM_BANDFMT_FLOAT:
#ifdef HAVE_LIBOIL
        oil_abs_f32_f32((float *)out, sizeof(float), (float *)in, sizeof(float), sz);
#else
        floatabs(float);
#endif
        break;

    case IM_BANDFMT_DOUBLE:
#ifdef HAVE_LIBOIL
        oil_abs_f64_f64((double *)out, sizeof(double), (double *)in, sizeof(double), sz);
#else
        floatabs(float);
#endif
        break;

    case IM_BANDFMT_COMPLEX:
        complexabs(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        complexabs(double);
        break;

    default:
        assert(0);
    }
}

int im_abs(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_uncoded("im_abs", in)) {
        return (-1);
    }

    if (vips_bandfmt_isuint(in->BandFmt)) {
        return (im_copy(in, out));
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    switch (in->BandFmt) {
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_INT:
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:

        break;

    case IM_BANDFMT_COMPLEX:
        out->BandFmt = IM_BANDFMT_FLOAT;
        break;

    case IM_BANDFMT_DPCOMPLEX:
        out->BandFmt = IM_BANDFMT_DOUBLE;
        break;

    default:
        im_error("im_abs", "%s", _("unknown input type"));
        return (-1);
    }

    if (im_wrapone(in, out, (im_wrapone_fn)abs_gen, in, NULL)) {
        return (-1);
    }

    return (0);
}
