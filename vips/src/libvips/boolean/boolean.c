

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define UC IM_BANDFMT_UCHAR
#define C IM_BANDFMT_CHAR
#define US IM_BANDFMT_USHORT
#define S IM_BANDFMT_SHORT
#define UI IM_BANDFMT_UINT
#define I IM_BANDFMT_INT

static int bandfmt_bool[10] = {

    UC,
    C,
    US,
    S,
    UI,
    I,
    I,
    I,
    I,
    I,
};

#define BINARY(IN, OUT, OP)                     \
    {                                           \
        OUT *tq = (OUT *)q;                     \
        IN *tp1 = (IN *)p[0];                   \
        IN *tp2 = (IN *)p[1];                   \
                                                \
        for (i = 0; i < ne; i++)                \
            tq[i] = (OUT)tp1[i] OP(OUT) tp2[i]; \
    }

#define BINARY_BUFFER(NAME, OP)                                                       \
    static void NAME##_buffer(PEL **p, PEL *q, int n, IMAGE *im) {                    \
                                                                                      \
        const int ne = n * im->Bands * (vips_bandfmt_iscomplex(im->BandFmt) ? 2 : 1); \
                                                                                      \
        int i;                                                                        \
                                                                                      \
        switch (im->BandFmt) {                                                        \
        case IM_BANDFMT_CHAR:                                                         \
            BINARY(signed char, signed char, OP);                                     \
            break;                                                                    \
        case IM_BANDFMT_UCHAR:                                                        \
            BINARY(unsigned char, unsigned char, OP);                                 \
            break;                                                                    \
        case IM_BANDFMT_SHORT:                                                        \
            BINARY(signed short, signed short, OP);                                   \
            break;                                                                    \
        case IM_BANDFMT_USHORT:                                                       \
            BINARY(unsigned short, unsigned short, OP);                               \
            break;                                                                    \
        case IM_BANDFMT_INT:                                                          \
            BINARY(signed int, signed int, OP);                                       \
            break;                                                                    \
        case IM_BANDFMT_UINT:                                                         \
            BINARY(unsigned int, unsigned int, OP);                                   \
            break;                                                                    \
        case IM_BANDFMT_FLOAT:                                                        \
            BINARY(float, signed int, OP);                                            \
            break;                                                                    \
        case IM_BANDFMT_COMPLEX:                                                      \
            BINARY(float, signed int, OP);                                            \
            break;                                                                    \
        case IM_BANDFMT_DOUBLE:                                                       \
            BINARY(double, signed int, OP);                                           \
            break;                                                                    \
        case IM_BANDFMT_DPCOMPLEX:                                                    \
            BINARY(double, signed int, OP);                                           \
            break;                                                                    \
                                                                                      \
        default:                                                                      \
            g_assert(0);                                                              \
        }                                                                             \
    }

BINARY_BUFFER(AND, &)

int im_andimage(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_andimage", in1, in2, out, bandfmt_bool, (im_wrapmany_fn)AND_buffer, NULL));
}

BINARY_BUFFER(OR, |)

int im_orimage(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_orimage", in1, in2, out, bandfmt_bool, (im_wrapmany_fn)OR_buffer, NULL));
}

BINARY_BUFFER(EOR, ^)

int im_eorimage(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_eorimage", in1, in2, out, bandfmt_bool, (im_wrapmany_fn)EOR_buffer, NULL));
}

#define CONST1(IN, OUT, OP)                \
    {                                      \
        OUT *tq = (OUT *)q;                \
        IN *tp = (IN *)p;                  \
        IN tc = *((IN *)vector);           \
                                           \
        for (i = 0; i < ne; i++)           \
            tq[i] = (OUT)tp[i] OP(OUT) tc; \
    }

#define CONST1_BUFFER(NAME, OP)                                                       \
    static void NAME##1_buffer(PEL * p, PEL * q, int n, PEL *vector, IMAGE *im) {     \
                                                                                      \
        const int ne = n * im->Bands * (vips_bandfmt_iscomplex(im->BandFmt) ? 2 : 1); \
                                                                                      \
        int i;                                                                        \
                                                                                      \
        switch (im->BandFmt) {                                                        \
        case IM_BANDFMT_CHAR:                                                         \
            CONST1(signed char, signed char, OP);                                     \
            break;                                                                    \
        case IM_BANDFMT_UCHAR:                                                        \
            CONST1(unsigned char, unsigned char, OP);                                 \
            break;                                                                    \
        case IM_BANDFMT_SHORT:                                                        \
            CONST1(signed short, signed short, OP);                                   \
            break;                                                                    \
        case IM_BANDFMT_USHORT:                                                       \
            CONST1(unsigned short, unsigned short, OP);                               \
            break;                                                                    \
        case IM_BANDFMT_INT:                                                          \
            CONST1(signed int, signed int, OP);                                       \
            break;                                                                    \
        case IM_BANDFMT_UINT:                                                         \
            CONST1(unsigned int, unsigned int, OP);                                   \
            break;                                                                    \
        case IM_BANDFMT_FLOAT:                                                        \
            CONST1(float, signed int, OP);                                            \
            break;                                                                    \
        case IM_BANDFMT_COMPLEX:                                                      \
            CONST1(float, signed int, OP);                                            \
            break;                                                                    \
        case IM_BANDFMT_DOUBLE:                                                       \
            CONST1(double, signed int, OP);                                           \
            break;                                                                    \
        case IM_BANDFMT_DPCOMPLEX:                                                    \
            CONST1(double, signed int, OP);                                           \
            break;                                                                    \
                                                                                      \
        default:                                                                      \
            g_assert(0);                                                              \
        }                                                                             \
    }

#define CONSTN(IN, OUT, OP)                       \
    {                                             \
        OUT *tq = (OUT *)q;                       \
        IN *tp = (IN *)p;                         \
        IN *tc = (IN *)vector;                    \
                                                  \
        for (i = 0, x = 0; x < n; x++)            \
            for (b = 0; b < bands; b++, i++)      \
                tq[i] = (OUT)tp[i] OP(OUT) tc[b]; \
    }

#define CONSTN_BUFFER(NAME, OP)                                                 \
    static void NAME##n_buffer(PEL *p, PEL *q, int n, PEL *vector, IMAGE *im) { \
        const int bands = im->Bands;                                            \
                                                                                \
        int i, x, b;                                                            \
                                                                                \
        switch (im->BandFmt) {                                                  \
        case IM_BANDFMT_CHAR:                                                   \
            CONSTN(signed char, signed char, OP);                               \
            break;                                                              \
        case IM_BANDFMT_UCHAR:                                                  \
            CONSTN(unsigned char, unsigned char, OP);                           \
            break;                                                              \
        case IM_BANDFMT_SHORT:                                                  \
            CONSTN(signed short, signed short, OP);                             \
            break;                                                              \
        case IM_BANDFMT_USHORT:                                                 \
            CONSTN(unsigned short, unsigned short, OP);                         \
            break;                                                              \
        case IM_BANDFMT_INT:                                                    \
            CONSTN(signed int, signed int, OP);                                 \
            break;                                                              \
        case IM_BANDFMT_UINT:                                                   \
            CONSTN(unsigned int, unsigned int, OP);                             \
            break;                                                              \
        case IM_BANDFMT_FLOAT:                                                  \
            CONSTN(float, signed int, OP);                                      \
            break;                                                              \
        case IM_BANDFMT_COMPLEX:                                                \
            CONSTN(float, signed int, OP);                                      \
            break;                                                              \
        case IM_BANDFMT_DOUBLE:                                                 \
            CONSTN(double, signed int, OP);                                     \
            break;                                                              \
        case IM_BANDFMT_DPCOMPLEX:                                              \
            CONSTN(double, signed int, OP);                                     \
            break;                                                              \
                                                                                \
        default:                                                                \
            g_assert(0);                                                        \
        }                                                                       \
    }

CONST1_BUFFER(AND, &)

CONSTN_BUFFER(AND, &)

int im_andimage_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_andimage", in, out, n, c, in->BandFmt, bandfmt_bool, (im_wrapone_fn)AND1_buffer, (im_wrapone_fn)ANDn_buffer));
}

int im_andimageconst(IMAGE *in, IMAGE *out, double c) {
    return (im_andimage_vec(in, out, 1, &c));
}

CONST1_BUFFER(OR, |)

CONSTN_BUFFER(OR, |)

int im_orimage_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_orimage", in, out, n, c, in->BandFmt, bandfmt_bool, (im_wrapone_fn)OR1_buffer, (im_wrapone_fn)ORn_buffer));
}

int im_orimageconst(IMAGE *in, IMAGE *out, double c) {
    return (im_orimage_vec(in, out, 1, &c));
}

CONST1_BUFFER(EOR, ^)

CONSTN_BUFFER(EOR, ^)

int im_eorimage_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_eorimage", in, out, n, c, in->BandFmt, bandfmt_bool, (im_wrapone_fn)EOR1_buffer, (im_wrapone_fn)EORn_buffer));
}

int im_eorimageconst(IMAGE *in, IMAGE *out, double c) {
    return (im_eorimage_vec(in, out, 1, &c));
}

CONST1_BUFFER(SHIFTL, <<)

CONSTN_BUFFER(SHIFTL, <<)

int im_shiftleft_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_shiftleft", in, out, n, c, in->BandFmt, bandfmt_bool, (im_wrapone_fn)SHIFTL1_buffer, (im_wrapone_fn)SHIFTLn_buffer));
}

int im_shiftleft(IMAGE *in, IMAGE *out, int n) {
    double c = n;

    return (im_shiftleft_vec(in, out, 1, &c));
}

CONST1_BUFFER(SHIFTR, >>)

CONSTN_BUFFER(SHIFTR, >>)

int im_shiftright_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_shiftright", in, out, n, c, in->BandFmt, bandfmt_bool, (im_wrapone_fn)SHIFTR1_buffer, (im_wrapone_fn)SHIFTRn_buffer));
}

int im_shiftright(IMAGE *in, IMAGE *out, int n) {
    double c = n;

    return (im_shiftright_vec(in, out, 1, &c));
}
