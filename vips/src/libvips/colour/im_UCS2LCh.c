

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

void imb_UCS2LCh(float *p, float *q, int n) {
    int x;

    for (x = 0; x < n; x++) {
        float Lucs = p[0];
        float Cucs = p[1];
        float hucs = p[2];

        float C = im_col_Cucs2C(Cucs);
        float h = im_col_Chucs2h(C, hucs);
        float L = im_col_Lucs2L(Lucs);

        p += 3;

        q[0] = L;
        q[1] = C;
        q[2] = h;
        q += 3;
    }
}

int im_UCS2LCh(IMAGE *in, IMAGE *out) {
    im_col_make_tables_UCS();

    return (im__colour_unary("im_UCS2LCh", in, out, IM_TYPE_UCS, (im_wrapone_fn)imb_UCS2LCh, NULL, NULL));
}
