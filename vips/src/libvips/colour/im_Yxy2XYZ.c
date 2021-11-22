

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

void imb_Yxy2XYZ(float *p, float *q, int n) {
    int i;

    for (i = 0; i < n; i++) {
        float Y = p[0];
        float x = p[1];
        float y = p[2];

        double total;
        float X, Z;

        p += 3;

        total = Y / y;
        X = x * total;
        Z = (X - x * X - x * Y) / x;

        q[0] = X;
        q[1] = Y;
        q[2] = Z;
        q += 3;
    }
}

int im_Yxy2XYZ(IMAGE *in, IMAGE *out) {
    return (im__colour_unary("im_Yxy2XYZ", in, out, IM_TYPE_XYZ, (im_wrapone_fn)imb_Yxy2XYZ, NULL, NULL));
}
