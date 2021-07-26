
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_dE_fromLab(float **p, float *q, int n) {
    float *p1 = p[0];
    float *p2 = p[1];
    int x;

    for (x = 0; x < n; x++) {
        q[x] = im_col_pythagoras(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);

        p1 += 3;
        p2 += 3;
    }
}

int im_dE_fromLab(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__colour_difference("im_dE_fromLab", in1, in2, out, (im_wrapmany_fn)imb_dE_fromLab, NULL, NULL));
}
