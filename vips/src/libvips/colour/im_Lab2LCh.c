

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_Lab2LCh(float *p, float *q, int n) {
    int x;

    for (x = 0; x < n; x++) {
        float L = p[0];
        float a = p[1];
        float b = p[2];
        float C, h;

        p += 3;

        C = sqrt(a * a + b * b);
        h = im_col_ab2h(a, b);

        q[0] = L;
        q[1] = C;
        q[2] = h;

        q += 3;
    }
}

int im__colour_unary(const char *domain, IMAGE *in, IMAGE *out, VipsType type, im_wrapone_fn buffer_fn, void *a, void *b) {
    IMAGE *t[1];

    if (im_check_uncoded(domain, in) || im_check_bands(domain, in, 3) || im_open_local_array(out, t, 1, domain, "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_cp_desc(out, t[0])) {
        return (-1);
    }
    out->Type = type;

    if (im_wrapone(t[0], out, (im_wrapone_fn)buffer_fn, a, b)) {
        return (-1);
    }

    return (0);
}

int im_Lab2LCh(IMAGE *in, IMAGE *out) {
    return (im__colour_unary("im_Lab2LCh", in, out, IM_TYPE_LCH, (im_wrapone_fn)imb_Lab2LCh, NULL, NULL));
}
