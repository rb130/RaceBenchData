
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

static int bandfmt_relational[10] = {

    UC,
    UC,
    UC,
    UC,
    UC,
    UC,
    UC,
    UC,
    UC,
    UC,
};

#define RBINARY(IN, FUN)               \
    {                                  \
        IN *tp1 = (IN *)p[0];          \
        IN *tp2 = (IN *)p[1];          \
                                       \
        for (i = 0; i < ne; i++)       \
            FUN(q[i], tp1[i], tp2[i]); \
    }

#define CBINARY(IN, FUN)           \
    {                              \
        IN *tp1 = (IN *)p[0];      \
        IN *tp2 = (IN *)p[1];      \
                                   \
        for (i = 0; i < ne; i++) { \
            FUN(q[i], tp1, tp2);   \
                                   \
            tp1 += 2;              \
            tp2 += 2;              \
        }                          \
    }

#define BINARY_BUFFER(NAME, RFUN, CFUN)                            \
    static void NAME##_buffer(PEL **p, PEL *q, int n, IMAGE *im) { \
        const int ne = n * im->Bands;                              \
                                                                   \
        int i;                                                     \
                                                                   \
        switch (im->BandFmt) {                                     \
        case IM_BANDFMT_CHAR:                                      \
            RBINARY(signed char, RFUN);                            \
            break;                                                 \
        case IM_BANDFMT_UCHAR:                                     \
            RBINARY(unsigned char, RFUN);                          \
            break;                                                 \
        case IM_BANDFMT_SHORT:                                     \
            RBINARY(signed short, RFUN);                           \
            break;                                                 \
        case IM_BANDFMT_USHORT:                                    \
            RBINARY(unsigned short, RFUN);                         \
            break;                                                 \
        case IM_BANDFMT_INT:                                       \
            RBINARY(signed int, RFUN);                             \
            break;                                                 \
        case IM_BANDFMT_UINT:                                      \
            RBINARY(unsigned int, RFUN);                           \
            break;                                                 \
        case IM_BANDFMT_FLOAT:                                     \
            RBINARY(float, RFUN);                                  \
            break;                                                 \
        case IM_BANDFMT_COMPLEX:                                   \
            CBINARY(float, CFUN);                                  \
            break;                                                 \
        case IM_BANDFMT_DOUBLE:                                    \
            RBINARY(double, RFUN);                                 \
            break;                                                 \
        case IM_BANDFMT_DPCOMPLEX:                                 \
            CBINARY(double, CFUN);                                 \
            break;                                                 \
                                                                   \
        default:                                                   \
            g_assert(0);                                           \
        }                                                          \
    }

#define EQUAL_REAL(Q, A, B) \
    {                       \
        if ((A) == (B))     \
            Q = 255;        \
        else                \
            Q = 0;          \
    }

#define EQUAL_COMPLEX(Q, A, B)                    \
    {                                             \
        if ((A)[0] == (B)[0] && (A)[1] == (B)[1]) \
            Q = 255;                              \
        else                                      \
            Q = 0;                                \
    }

BINARY_BUFFER(EQUAL, EQUAL_REAL, EQUAL_COMPLEX)

int im_equal(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_equal", in1, in2, out, bandfmt_relational, (im_wrapmany_fn)EQUAL_buffer, NULL));
}

#define NOTEQUAL_REAL(Q, A, B) \
    {                          \
        if ((A) != (B))        \
            Q = 255;           \
        else                   \
            Q = 0;             \
    }

#define NOTEQUAL_COMPLEX(Q, A, B)                 \
    {                                             \
        if ((A)[0] != (B)[0] || (A)[1] != (B)[1]) \
            Q = 255;                              \
        else                                      \
            Q = 0;                                \
    }

BINARY_BUFFER(NOTEQUAL, NOTEQUAL_REAL, NOTEQUAL_COMPLEX)

int im_notequal(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_notequal", in1, in2, out, bandfmt_relational, (im_wrapmany_fn)NOTEQUAL_buffer, NULL));
}

#define LESS_REAL(Q, A, B) \
    {                      \
        if ((A) < (B))     \
            Q = 255;       \
        else               \
            Q = 0;         \
    }

#define LESS_COMPLEX(Q, A, B)                          \
    {                                                  \
        double m1 = (A)[0] * (A)[0] + (A)[1] * (A)[1]; \
        double m2 = (B)[0] * (B)[0] + (B)[1] * (B)[1]; \
                                                       \
        if (m1 < m2)                                   \
            Q = 255;                                   \
        else                                           \
            Q = 0;                                     \
    }

BINARY_BUFFER(LESS, LESS_REAL, LESS_COMPLEX)

int im_less(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_less", in1, in2, out, bandfmt_relational, (im_wrapmany_fn)LESS_buffer, NULL));
}

#define LESSEQ_REAL(Q, A, B) \
    {                        \
        if ((A) <= (B))      \
            Q = 255;         \
        else                 \
            Q = 0;           \
    }

#define LESSEQ_COMPLEX(Q, A, B)                        \
    {                                                  \
        double m1 = (A)[0] * (A)[0] + (A)[1] * (A)[1]; \
        double m2 = (B)[0] * (B)[0] + (B)[1] * (B)[1]; \
                                                       \
        if (m1 <= m2)                                  \
            Q = 255;                                   \
        else                                           \
            Q = 0;                                     \
    }

BINARY_BUFFER(LESSEQ, LESSEQ_REAL, LESSEQ_COMPLEX)

int im_lesseq(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_lesseq", in1, in2, out, bandfmt_relational, (im_wrapmany_fn)LESSEQ_buffer, NULL));
}

int im_more(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im_less(in2, in1, out));
}

int im_moreeq(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im_lesseq(in2, in1, out));
}

#define RCONST1(IN, FUN)          \
    {                             \
        IN *tp = (IN *)p;         \
        IN tc = *((IN *)vector);  \
                                  \
        for (i = 0; i < ne; i++)  \
            FUN(q[i], tp[i], tc); \
    }

#define CCONST1(IN, FUN)           \
    {                              \
        IN *tp = (IN *)p;          \
        IN *tc = ((IN *)vector);   \
                                   \
        for (i = 0; i < ne; i++) { \
            FUN(q[i], tp, tc);     \
                                   \
            tp += 2;               \
        }                          \
    }

#define CONST1_BUFFER(NAME, RFUN, CFUN)                                           \
    static void NAME##1_buffer(PEL * p, PEL * q, int n, PEL *vector, IMAGE *im) { \
        const int ne = n * im->Bands;                                             \
                                                                                  \
        int i;                                                                    \
                                                                                  \
        switch (im->BandFmt) {                                                    \
        case IM_BANDFMT_CHAR:                                                     \
            RCONST1(signed char, RFUN);                                           \
            break;                                                                \
        case IM_BANDFMT_UCHAR:                                                    \
            RCONST1(unsigned char, RFUN);                                         \
            break;                                                                \
        case IM_BANDFMT_SHORT:                                                    \
            RCONST1(signed short, RFUN);                                          \
            break;                                                                \
        case IM_BANDFMT_USHORT:                                                   \
            RCONST1(unsigned short, RFUN);                                        \
            break;                                                                \
        case IM_BANDFMT_INT:                                                      \
            RCONST1(signed int, RFUN);                                            \
            break;                                                                \
        case IM_BANDFMT_UINT:                                                     \
            RCONST1(unsigned int, RFUN);                                          \
            break;                                                                \
        case IM_BANDFMT_FLOAT:                                                    \
            RCONST1(float, RFUN);                                                 \
            break;                                                                \
        case IM_BANDFMT_COMPLEX:                                                  \
            CCONST1(float, CFUN);                                                 \
            break;                                                                \
        case IM_BANDFMT_DOUBLE:                                                   \
            RCONST1(double, RFUN);                                                \
            break;                                                                \
        case IM_BANDFMT_DPCOMPLEX:                                                \
            CCONST1(double, CFUN);                                                \
            break;                                                                \
                                                                                  \
        default:                                                                  \
            g_assert(0);                                                          \
        }                                                                         \
    }

#define RCONSTN(IN, FUN)                     \
    {                                        \
        IN *tp = (IN *)p;                    \
        IN *tc = (IN *)vector;               \
                                             \
        for (i = 0, x = 0; x < n; x++)       \
            for (b = 0; b < bands; b++, i++) \
                FUN(q[i], tp[i], tc[b]);     \
    }

#define CCONSTN(IN, FUN)                       \
    {                                          \
        IN *tp = (IN *)p;                      \
                                               \
        for (i = 0, x = 0; x < n; x++) {       \
            IN *tc = ((IN *)vector);           \
                                               \
            for (b = 0; b < bands; b++, i++) { \
                FUN(q[i], tp, tc);             \
                                               \
                tp += 2;                       \
                tc += 2;                       \
            }                                  \
        }                                      \
    }

#define CONSTN_BUFFER(NAME, RFUN, CFUN)                                         \
    static void NAME##n_buffer(PEL *p, PEL *q, int n, PEL *vector, IMAGE *im) { \
        const int bands = im->Bands;                                            \
                                                                                \
        int i, x, b;                                                            \
                                                                                \
        switch (im->BandFmt) {                                                  \
        case IM_BANDFMT_CHAR:                                                   \
            RCONSTN(signed char, RFUN);                                         \
            break;                                                              \
        case IM_BANDFMT_UCHAR:                                                  \
            RCONSTN(unsigned char, RFUN);                                       \
            break;                                                              \
        case IM_BANDFMT_SHORT:                                                  \
            RCONSTN(signed short, RFUN);                                        \
            break;                                                              \
        case IM_BANDFMT_USHORT:                                                 \
            RCONSTN(unsigned short, RFUN);                                      \
            break;                                                              \
        case IM_BANDFMT_INT:                                                    \
            RCONSTN(signed int, RFUN);                                          \
            break;                                                              \
        case IM_BANDFMT_UINT:                                                   \
            RCONSTN(unsigned int, RFUN);                                        \
            break;                                                              \
        case IM_BANDFMT_FLOAT:                                                  \
            RCONSTN(float, RFUN);                                               \
            break;                                                              \
        case IM_BANDFMT_COMPLEX:                                                \
            CCONSTN(float, CFUN);                                               \
            break;                                                              \
        case IM_BANDFMT_DOUBLE:                                                 \
            RCONSTN(double, RFUN);                                              \
            break;                                                              \
        case IM_BANDFMT_DPCOMPLEX:                                              \
            CCONSTN(double, CFUN);                                              \
            break;                                                              \
                                                                                \
        default:                                                                \
            g_assert(0);                                                        \
        }                                                                       \
    }

CONST1_BUFFER(EQUAL, EQUAL_REAL, EQUAL_COMPLEX)

CONSTN_BUFFER(EQUAL, EQUAL_REAL, EQUAL_COMPLEX)

int im_equal_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_equal", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)EQUAL1_buffer, (im_wrapone_fn)EQUALn_buffer));
}

CONST1_BUFFER(NOTEQUAL, NOTEQUAL_REAL, NOTEQUAL_COMPLEX)

CONSTN_BUFFER(NOTEQUAL, NOTEQUAL_REAL, NOTEQUAL_COMPLEX)

int im_notequal_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_notequal", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)NOTEQUAL1_buffer, (im_wrapone_fn)NOTEQUALn_buffer));
}

CONST1_BUFFER(LESS, LESS_REAL, LESS_COMPLEX)

CONSTN_BUFFER(LESS, LESS_REAL, LESS_COMPLEX)

int im_less_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_less", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)LESS1_buffer, (im_wrapone_fn)LESSn_buffer));
}

CONST1_BUFFER(LESSEQ, LESSEQ_REAL, LESSEQ_COMPLEX)

CONSTN_BUFFER(LESSEQ, LESSEQ_REAL, LESSEQ_COMPLEX)

int im_lesseq_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_lesseq", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)LESSEQ1_buffer, (im_wrapone_fn)LESSEQn_buffer));
}

#define MORE_REAL(Q, A, B) \
    {                      \
        if ((A) > (B))     \
            Q = 255;       \
        else               \
            Q = 0;         \
    }

#define MORE_COMPLEX(Q, A, B)                          \
    {                                                  \
        double m1 = (A)[0] * (A)[0] + (A)[1] * (A)[1]; \
        double m2 = (B)[0] * (B)[0] + (B)[1] * (B)[1]; \
                                                       \
        if (m1 > m2)                                   \
            Q = 255;                                   \
        else                                           \
            Q = 0;                                     \
    }

CONST1_BUFFER(MORE, MORE_REAL, MORE_COMPLEX)

CONSTN_BUFFER(MORE, MORE_REAL, MORE_COMPLEX)

int im_more_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_more", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)MORE1_buffer, (im_wrapone_fn)MOREn_buffer));
}

#define MOREEQ_REAL(Q, A, B) \
    {                        \
        if ((A) >= (B))      \
            Q = 255;         \
        else                 \
            Q = 0;           \
    }

#define MOREEQ_COMPLEX(Q, A, B)                        \
    {                                                  \
        double m1 = (A)[0] * (A)[0] + (A)[1] * (A)[1]; \
        double m2 = (B)[0] * (B)[0] + (B)[1] * (B)[1]; \
                                                       \
        if (m1 >= m2)                                  \
            Q = 255;                                   \
        else                                           \
            Q = 0;                                     \
    }

CONST1_BUFFER(MOREEQ, MOREEQ_REAL, MOREEQ_COMPLEX)

CONSTN_BUFFER(MOREEQ, MOREEQ_REAL, MOREEQ_COMPLEX)

int im_moreeq_vec(IMAGE *in, IMAGE *out, int n, double *c) {
    return (im__arith_binary_const("im_moreeq", in, out, n, c, in->BandFmt, bandfmt_relational, (im_wrapone_fn)MOREEQ1_buffer, (im_wrapone_fn)MOREEQn_buffer));
}

int im_equalconst(IMAGE *in, IMAGE *out, double c) {
    return (im_equal_vec(in, out, 1, &c));
}

int im_notequalconst(IMAGE *in, IMAGE *out, double c) {
    return (im_notequal_vec(in, out, 1, &c));
}

int im_lessconst(IMAGE *in, IMAGE *out, double c) {
    return (im_less_vec(in, out, 1, &c));
}

int im_lesseqconst(IMAGE *in, IMAGE *out, double c) {
    return (im_lesseq_vec(in, out, 1, &c));
}

int im_moreconst(IMAGE *in, IMAGE *out, double c) {
    return (im_more_vec(in, out, 1, &c));
}

int im_moreeqconst(IMAGE *in, IMAGE *out, double c) {
    return (im_moreeq_vec(in, out, 1, &c));
}
