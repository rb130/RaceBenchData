
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MOST_OF(A, B) ((A) > 0.9 * (B))
#define LITTLE_OF(A, B) ((B) < 0.1 * (B))

int im_maxpos_subpel(IMAGE *in, double *x, double *y) {
#define FUNCTION_NAME "im_maxpos_subpel"

    int xs[5];
    int ys[5];
    double vals[5];
    int xa, ya, xb, yb;
    double vxa, vya, vxb, vyb;

    if (im_maxpos_vec(in, xs, ys, vals, 5)) {
        return -1;
    }

#define WRAP_TEST_RETURN()                                             \
                                                                       \
    if (!xa && in->Xsize - 1 == xb)                                    \
        xa = in->Xsize;                                                \
                                                                       \
    else if (!xb && in->Xsize - 1 == xa)                               \
        xb = in->Xsize;                                                \
                                                                       \
    if (!ya && in->Ysize - 1 == yb)                                    \
        ya = in->Ysize;                                                \
                                                                       \
    else if (!yb && in->Ysize - 1 == ya)                               \
        yb = in->Ysize;                                                \
                                                                       \
    if (1 == abs(xb - xa) && 1 == abs(yb - ya)) {                      \
        *x = ((double)xa) + ((double)(xb - xa)) * (vxb / (vxa + vxb)); \
        *y = ((double)ya) + ((double)(yb - ya)) * (vyb / (vya + vyb)); \
        return 0;                                                      \
    }

#define TEST3(A, B)                         \
    if (xs[0] == xs[A] && ys[0] == ys[B]) { \
        xa = xs[0];                         \
        ya = ys[0];                         \
        xb = xs[B];                         \
        yb = ys[A];                         \
        vxa = vals[0];                      \
        vya = vals[0];                      \
        vxb = vals[B];                      \
        vyb = vals[A];                      \
        WRAP_TEST_RETURN()                  \
    }

    TEST3(1, 2)
    TEST3(2, 1)

    if (MOST_OF(vals[1], vals[0]) && MOST_OF(vals[2], vals[0]) && MOST_OF(vals[3], vals[0]) && LITTLE_OF(vals[4], vals[0])) {

#define TEST4(A, B, C, D, E, F, G, H)                                           \
    if (xs[A] == xs[B] && xs[C] == xs[D] && ys[E] == ys[F] && ys[G] == ys[H]) { \
        xa = xs[A];                                                             \
        xb = xs[C];                                                             \
        ya = ys[E];                                                             \
        yb = ys[G];                                                             \
        vxa = vals[A] + vals[B];                                                \
        vxb = vals[C] + vals[D];                                                \
        vya = vals[E] + vals[F];                                                \
        vyb = vals[G] + vals[H];                                                \
        WRAP_TEST_RETURN()                                                      \
    }

        TEST4(0, 3, 1, 2, 0, 1, 2, 3)
        TEST4(0, 1, 2, 3, 0, 3, 1, 2)
        TEST4(0, 3, 1, 2, 0, 2, 1, 3)
        TEST4(0, 2, 1, 3, 0, 3, 1, 2)
    }

    im_warn(FUNCTION_NAME, "registration performed to nearest pixel only: correlation does not have the expected distribution for sub-pixel registration");
    *x = (double)xs[0];
    *y = (double)ys[0];
    return 0;
}
