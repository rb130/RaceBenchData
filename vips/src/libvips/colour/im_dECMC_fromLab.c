

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_dECMC_fromLab(float **p, float *q, int n) {
    float *p1 = p[0];
    float *p2 = p[1];
    int x;

    for (x = 0; x < n; x++) {
        float L1 = p1[0];
        float a1 = p1[1];
        float b1 = p1[2];
        float L2 = p2[0];
        float a2 = p2[1];
        float b2 = p2[2];

        p1 += 3;
        p2 += 3;

        q[x] = im_col_dECMC(L1, a1, b1, L2, a2, b2);
    }
}

int im_dECMC_fromLab(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__colour_difference("im_dECMC_fromLab", in1, in2, out, (im_wrapmany_fn)imb_dECMC_fromLab, NULL, NULL));
}
