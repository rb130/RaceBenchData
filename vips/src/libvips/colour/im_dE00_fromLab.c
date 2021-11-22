

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__colour_difference(const char *domain, IMAGE *in1, IMAGE *in2, IMAGE *out, im_wrapmany_fn buffer_fn, void *a, void *b) {
    IMAGE *t[3];

    if (im_check_uncoded(domain, in1) || im_check_uncoded(domain, in2) || im_check_bands(domain, in1, 3) || im_check_bands(domain, in2, 3) || im_check_size_same(domain, in1, in2) || im_open_local_array(out, t, 2, domain, "p") || im_clip2fmt(in1, t[0], IM_BANDFMT_FLOAT) || im_clip2fmt(in2, t[1], IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_cp_descv(out, t[0], t[1], NULL)) {
        return (-1);
    }
    out->Bands = 1;
    out->Type = IM_TYPE_B_W;

    t[2] = NULL;
    if (im_wrapmany(t, out, buffer_fn, a, b)) {
        return (-1);
    }

    return (0);
}

void imb_dE00_fromLab(float **p, float *q, int n) {
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

        q[x] = im_col_dE00(L1, a1, b1, L2, a2, b2);
    }
}

int im_dE00_fromLab(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__colour_difference("im_dE00_fromLab", in1, in2, out, (im_wrapmany_fn)imb_dE00_fromLab, NULL, NULL));
}
