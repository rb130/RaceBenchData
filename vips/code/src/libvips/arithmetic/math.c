
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

#define FUN_LOOP(IN, OUT, FUN)        \
    {                                 \
        IN *p = (IN *)in;             \
        OUT *q = (OUT *)out;          \
                                      \
        for (x = 0; x < ne; x++)      \
            q[x] = FUN((double)p[x]); \
    }

#define FUN_BUFFER(FUN)                                                 \
    static void FUN##_buffer(PEL *in, PEL *out, int width, IMAGE *im) { \
        const int ne = width * im->Bands;                               \
                                                                        \
        int x;                                                          \
                                                                        \
        switch (im->BandFmt) {                                          \
        case IM_BANDFMT_UCHAR:                                          \
            FUN_LOOP(unsigned char, float, FUN);                        \
            break;                                                      \
        case IM_BANDFMT_CHAR:                                           \
            FUN_LOOP(signed char, float, FUN);                          \
            break;                                                      \
        case IM_BANDFMT_USHORT:                                         \
            FUN_LOOP(unsigned short, float, FUN);                       \
            break;                                                      \
        case IM_BANDFMT_SHORT:                                          \
            FUN_LOOP(signed short, float, FUN);                         \
            break;                                                      \
        case IM_BANDFMT_UINT:                                           \
            FUN_LOOP(unsigned int, float, FUN);                         \
            break;                                                      \
        case IM_BANDFMT_INT:                                            \
            FUN_LOOP(signed int, float, FUN);                           \
            break;                                                      \
        case IM_BANDFMT_FLOAT:                                          \
            FUN_LOOP(float, float, FUN);                                \
            break;                                                      \
        case IM_BANDFMT_DOUBLE:                                         \
            FUN_LOOP(double, double, FUN);                              \
            break;                                                      \
                                                                        \
        default:                                                        \
            g_assert(0);                                                \
        }                                                               \
    }

int im__math(const char *name, IMAGE *in, IMAGE *out, im_wrapone_fn gen) {
    if (im_piocheck(in, out) || im_check_uncoded(name, in) || im_check_noncomplex(name, in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (vips_bandfmt_isint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_FLOAT;
    }

    if (im_wrapone(in, out, gen, in, NULL)) {
        return (-1);
    }

    return (0);
}

#define DSIN(X) (sin(IM_RAD(X)))

FUN_BUFFER(DSIN)

int im_sintra(IMAGE *in, IMAGE *out) {
    return (im__math("im_sintra", in, out, (im_wrapone_fn)DSIN_buffer));
}

#define ADSIN(X) (IM_DEG(asin(X)))

FUN_BUFFER(ADSIN)

int im_asintra(IMAGE *in, IMAGE *out) {
    return (im__math("im_asintra", in, out, (im_wrapone_fn)ADSIN_buffer));
}

#define DCOS(X) (cos(IM_RAD(X)))

FUN_BUFFER(DCOS)

int im_costra(IMAGE *in, IMAGE *out) {
    return (im__math("im_costra", in, out, (im_wrapone_fn)DCOS_buffer));
}

#define ADCOS(X) (IM_DEG(acos(X)))

FUN_BUFFER(ADCOS)

int im_acostra(IMAGE *in, IMAGE *out) {
    return (im__math("im_acostra", in, out, (im_wrapone_fn)ADCOS_buffer));
}

#define DTAN(X) (tan(IM_RAD(X)))

FUN_BUFFER(DTAN)

int im_tantra(IMAGE *in, IMAGE *out) {
    return (im__math("im_tantra", in, out, (im_wrapone_fn)DTAN_buffer));
}

#define ADTAN(X) (IM_DEG(atan(X)))

FUN_BUFFER(ADTAN)

int im_atantra(IMAGE *in, IMAGE *out) {
    return (im__math("im_atantra", in, out, (im_wrapone_fn)ADTAN_buffer));
}

FUN_BUFFER(log10)

int im_log10tra(IMAGE *in, IMAGE *out) {
    return (im__math("im_log10tra", in, out, (im_wrapone_fn)log10_buffer));
}

FUN_BUFFER(log)

int im_logtra(IMAGE *in, IMAGE *out) {
    return (im__math("im_logtra", in, out, (im_wrapone_fn)log_buffer));
}
