

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

void imb_XYZ2Yxy(float *p, float *q, int n) {
    int i;

    for (i = 0; i < n; i++) {
        float X = p[0];
        float Y = p[1];
        float Z = p[2];
        double total = X + Y + Z;

        float x, y;

        p += 3;

        x = X / total;
        y = Y / total;

        q[0] = Y;
        q[1] = x;
        q[2] = y;
        q += 3;
    }
}

int im_XYZ2Yxy(IMAGE *in, IMAGE *out) {
    return (im__colour_unary("im_XYZ2Yxy", in, out, IM_TYPE_YXY, (im_wrapone_fn)imb_XYZ2Yxy, NULL, NULL));
}
