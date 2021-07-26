
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

void imb_LCh2UCS(float *p, float *q, int n) {
    int x;

    for (x = 0; x < n; x++) {
        float L = p[0];
        float C = p[1];
        float h = p[2];
        p += 3;

        q[0] = im_col_L2Lucs(L);
        q[1] = im_col_C2Cucs(C);
        q[2] = im_col_Ch2hucs(C, h);
        q += 3;
    }
}

int im_LCh2UCS(IMAGE *in, IMAGE *out) {
    return (im__colour_unary("im_LCh2UCS", in, out, IM_TYPE_UCS, (im_wrapone_fn)imb_LCh2UCS, NULL, NULL));
}
