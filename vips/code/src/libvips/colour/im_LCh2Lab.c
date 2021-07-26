
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

void imb_LCh2Lab(float *p, float *q, int n) {
    int x;

    for (x = 0; x < n; x++) {
        float L = p[0];
        float C = p[1];
        float h = p[2];
        float a, b;

        p += 3;

        a = C * cos(IM_RAD(h));
        b = C * sin(IM_RAD(h));

        q[0] = L;
        q[1] = a;
        q[2] = b;
        q += 3;
    }
}

int im_LCh2Lab(IMAGE *in, IMAGE *out) {
    return (im__colour_unary("im_LCh2Lab", in, out, IM_TYPE_LAB, (im_wrapone_fn)imb_LCh2Lab, NULL, NULL));
}
