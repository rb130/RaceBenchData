

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#include "mosaic.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__coeff(int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, double *a, double *b, double *dx, double *dy) {
    DOUBLEMASK *in, *out;

    if (!(in = im_create_dmask("in", 4, 4))) {
        return (-1);
    }

    in->coeff[0] = (double)xs1;
    in->coeff[1] = (double)-ys1;
    in->coeff[2] = 1.0;
    in->coeff[3] = 0.0;
    in->coeff[4] = (double)ys1;
    in->coeff[5] = (double)xs1;
    in->coeff[6] = 0.0;
    in->coeff[7] = 1.0;
    in->coeff[8] = (double)xs2;
    in->coeff[9] = (double)-ys2;
    in->coeff[10] = 1.0;
    in->coeff[11] = 0.0;
    in->coeff[12] = (double)ys2;
    in->coeff[13] = (double)xs2;
    in->coeff[14] = 0.0;
    in->coeff[15] = 1.0;

    if (!(out = im_matinv(in, "out"))) {
        im_free_dmask(in);
        return (-1);
    }

    *a = out->coeff[0] * xr1 + out->coeff[1] * yr1 + out->coeff[2] * xr2 + out->coeff[3] * yr2;
    *b = out->coeff[4] * xr1 + out->coeff[5] * yr1 + out->coeff[6] * xr2 + out->coeff[7] * yr2;
    *dx = out->coeff[8] * xr1 + out->coeff[9] * yr1 + out->coeff[10] * xr2 + out->coeff[11] * yr2;
    *dy = out->coeff[12] * xr1 + out->coeff[13] * yr1 + out->coeff[14] * xr2 + out->coeff[15] * yr2;

    im_free_dmask(in);
    im_free_dmask(out);

    return (0);
}

int im_match_linear(IMAGE *ref, IMAGE *sec, IMAGE *out, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2) {
    double a, b, dx, dy;
    int w, h, x, y;

    if (im__coeff(xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, &a, &b, &dx, &dy)) {
        return (-1);
    }

    x = 0;
    y = 0;
    w = ref->Xsize;
    h = ref->Ysize;

    if (im_affinei(sec, out, vips_interpolate_bilinear_static(), a, -b, b, a, dx, dy, x, y, w, h)) {
        return (-1);
    }

    return (0);
}

int im_match_linear_search(IMAGE *ref, IMAGE *sec, IMAGE *out, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int hwindowsize, int hsearchsize) {
    int xs3, ys3;
    int xs4, ys4;
    double cor1, cor2;

    if (im_correl(ref, sec, xr1, yr1, xs1, ys1, hwindowsize, hsearchsize, &cor1, &xs3, &ys3)) {
        return (-1);
    }
    if (im_correl(ref, sec, xr2, yr2, xs2, ys2, hwindowsize, hsearchsize, &cor2, &xs4, &ys4)) {
        return (-1);
    }

    if (im_match_linear(ref, sec, out, xr1, yr1, xs3, ys3, xr2, yr2, xs4, ys4)) {
        return (-1);
    }

    return (0);
}
