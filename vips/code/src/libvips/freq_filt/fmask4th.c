
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int alloc(IMAGE *out, int xs, int ys, double **xd, double **yd, float **coeff) {
    int i;
    double *x, *y;
    float *c;

    x = IM_ARRAY(out, xs / 2 + 1, double);
    y = IM_ARRAY(out, ys / 2 + 1, double);
    c = IM_ARRAY(out, (xs / 2 + 1) * (ys / 2 + 1), float);
    if (!x || !y || !c) {
        return (-1);
    }

    for (i = 0; i < ys / 2 + 1; i++) {
        y[i] = (i * i) / ((double)(ys * ys / 4));
    }
    for (i = 0; i < xs / 2 + 1; i++) {
        x[i] = (i * i) / ((double)(xs * xs / 4));
    }
    *xd = x;
    *yd = y;
    *coeff = c;

    return (0);
}

static float *ideal_hpf(IMAGE *out, int xs, int ys, double fc) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2;

    if (xs != ys || fc < 0.0) {
        im_error("ideal_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("ideal_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = xd[x] + yd[y];
            if (distance2 > fc2) {
                *cpcoeff++ = 1.0;
            } else {
                *cpcoeff++ = 0.0;
            }
        }
    }

    *coeff = 1.0;

    return (coeff);
}

static float *ideal_lpf(IMAGE *out, int xs, int ys, double fc) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2;

    if (xs != ys || fc <= 0.0) {
        im_error("ideal_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("ideal_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = xd[x] + yd[y];
            if (distance2 <= fc2) {
                *cpcoeff++ = 1.0;
            } else {
                *cpcoeff++ = 0.0;
            }
        }
    }

    return (coeff);
}

static float *butterworth_hpf(IMAGE *out, int xs, int ys, double order, double fc, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2, cnst;

    if (xs != ys || fc < 0.0 || order < 1.0 || ac <= 0.0 || ac >= 1.0) {
        im_error("butterworth_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("butterworth_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = (1.0 / ac) - 1.0;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {

            if (x == 0 && y == 0) {
                *cpcoeff++ = 1.0;
            } else {
                distance2 = fc2 / (xd[x] + yd[y]);
                *cpcoeff++ = 1.0 / (1.0 + cnst * pow(distance2, order));
            }
        }
    }

    return (coeff);
}

static float *butterworth_lpf(IMAGE *out, int xs, int ys, double order, double fc, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2, cnst;

    if (xs != ys || fc <= 0.0 || order < 1.0 || ac >= 1.0 || ac <= 0.0) {
        im_error("butterworth_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("butterworth_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = (1.0 / ac) - 1.0;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = (xd[x] + yd[y]) / fc2;
            *cpcoeff++ = 1.0 / (1.0 + cnst * pow(distance2, order));
        }
    }

    return (coeff);
}

static float *gaussian_hpf(IMAGE *out, int xs, int ys, double fc, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2, cnst;

    if (xs != ys || fc <= 0.0 || ac >= 1.0 || ac <= 0.0) {
        im_error("gaussian_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("gaussian_hpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = -log(ac);
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = (xd[x] + yd[y]) / fc2;
            *cpcoeff++ = 1.0 - exp(-cnst * distance2);
        }
    }

    *coeff = 1.0;

    return (coeff);
}

static float *gaussian_lpf(IMAGE *out, int xs, int ys, double fc, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, fc2, distance2, cnst;

    if (xs != ys || fc < 0.0 || ac >= 1.0 || ac <= 0.0) {
        im_error("gaussian_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (fc > 1.0 && fc <= xs / 2) {
        fc2 = fc * fc * 4.0 / (double)(xs * ys);
    } else if (fc <= 1.0 && fc > 0.0) {
        fc2 = fc * fc;
    } else {
        im_error("gaussian_lpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = -log(ac);
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = (xd[x] + yd[y]) / fc2;
            *cpcoeff++ = exp(-cnst * distance2);
        }
    }

    return (coeff);
}

static float *ideal_rpf(IMAGE *out, int xs, int ys, double fc, double width) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, df, distance2, radius1_2, radius2_2;

    if (xs != ys || fc <= 0 || width <= 0) {
        im_error("ideal_rpf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        radius1_2 = (fc - df) * (fc - df);
        radius2_2 = (fc + df) * (fc + df);
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        radius1_2 = (fc - df) * (fc - df) * 4.0 / ((double)(xs * xs));
        radius2_2 = (fc + df) * (fc + df) * 4.0 / ((double)(xs * xs));
    } else {
        im_error("ideal_rpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = xd[x] + yd[y];
            if (distance2 < radius2_2 && distance2 > radius1_2) {
                *cpcoeff++ = 1.0;
            } else {
                *cpcoeff++ = 0.0;
            }
        }
    }

    *coeff = 1.0;

    return (coeff);
}

static float *ideal_rrf(IMAGE *out, int xs, int ys, double fc, double width) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, df, distance2, radius1_2, radius2_2;

    if (xs != ys || fc < 0.0 || width <= 0.0) {
        im_error("ideal_rrf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        radius1_2 = (fc - df) * (fc - df);
        radius2_2 = (fc + df) * (fc + df);
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        radius1_2 = (fc - df) * (fc - df) * 4.0 / ((double)(xs * xs));
        radius2_2 = (fc + df) * (fc + df) * 4.0 / ((double)(xs * xs));
    } else {
        im_error("ideal_rrf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = xd[x] + yd[y];
            if (distance2 < radius2_2 && distance2 > radius1_2) {
                *cpcoeff++ = 0.0;
            } else {
                *cpcoeff++ = 1.0;
            }
        }
    }

    return (coeff);
}

static float *butterworth_rpf(IMAGE *out, int xs, int ys, double order, double fc, double width, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, d, df, ndf, ndf2, nfc, cnst;

    if (xs != ys || fc <= 0.0 || width <= 0.0 || order < 1.0 || ac >= 1.0 || ac <= 0.0) {
        im_error("butterworth_rpf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        nfc = fc;
        ndf = width / 2.0;
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        nfc = fc * 2.0 / (double)xs;
        ndf = width / (double)ys;
    } else {
        im_error("butterworth_rpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = (1.0 / ac) - 1.0;
    ndf2 = ndf * ndf;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            d = sqrt(xd[x] + yd[y]);
            *cpcoeff++ = 1.0 / (1.0 + cnst * pow((d - nfc) * (d - nfc) / ndf2, order));
        }
    }

    *coeff = 1.0;

    return (coeff);
}

static float *butterworth_rrf(IMAGE *out, int xs, int ys, double order, double fc, double width, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, d, df, ndf, ndf2, nfc, cnst;

    if (xs != ys || fc <= 0.0 || width <= 0.0 || order < 1.0 || ac >= 1.0 || ac <= 0.0) {
        im_error("butterworth_rrf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        nfc = fc;
        ndf = width / 2.0;
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        nfc = fc * 2.0 / (double)xs;
        ndf = width / (double)ys;
    } else {
        im_error("butterworth_rrf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = (1.0 / ac) - 1.0;
    ndf2 = ndf * ndf;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            d = sqrt(xd[x] + yd[y]);
            if (d == 0.0) {
                *cpcoeff++ = 1.0;
            } else {
                *cpcoeff++ = 1.0 / (1.0 + cnst * pow(ndf2 / ((d - nfc) * (d - nfc)), order));
            }
        }
    }

    return (coeff);
}

static float *gaussian_rpf(IMAGE *out, int xs, int ys, double fc, double width, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, d, df, ndf, ndf2, nfc, cnst;

    if (xs != ys || fc < 0.0 || width <= 0.0 || ac <= 0.0 || ac > 1.0) {
        im_error("gaussian_rpf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        nfc = fc;
        ndf = width / 2.0;
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        nfc = fc * 2.0 / (double)xs;
        ndf = width / (double)ys;
    } else {
        im_error("gaussian_rpf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = -log(ac);
    ndf2 = ndf * ndf;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            d = sqrt(xd[x] + yd[y]);
            *cpcoeff++ = exp(-cnst * (d - nfc) * (d - nfc) / ndf2);
        }
    }

    *coeff = 1.0;

    return (coeff);
}

static float *gaussian_rrf(IMAGE *out, int xs, int ys, double fc, double width, double ac) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, d, df, ndf, ndf2, nfc, cnst;

    if (xs != ys || fc < 0.0 || width <= 0.0 || ac <= 0.0 || ac > 1.0) {
        im_error("gaussian_rrf", "%s", _("bad args"));
        return (NULL);
    }

    df = width / 2.0;
    if (fc <= 1.0 && df < 1.0 && fc - df > 0.0) {
        nfc = fc;
        ndf = width / 2.0;
    } else if (fc - df > 1.0 && df >= 1.0 && fc <= xs / 2) {
        nfc = fc * 2.0 / (double)xs;
        ndf = width / (double)ys;
    } else {
        im_error("gaussian_rrf", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = -log(ac);
    ndf2 = ndf * ndf;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            d = sqrt(xd[x] + yd[y]);
            *cpcoeff++ = 1.0 - exp(-cnst * (d - nfc) * (d - nfc) / ndf2);
        }
    }

    return (coeff);
}

static float *fractal_flt(IMAGE *out, int xs, int ys, double frdim) {
    int x, y;
    float *coeff, *cpcoeff;
    double *xd, *yd, distance2, cnst;

    if (xs != ys || frdim <= 2.0 || frdim >= 3.0) {
        im_error("fractal_flt", "%s", _("bad args"));
        return (NULL);
    }

    if (alloc(out, xs, ys, &xd, &yd, &coeff)) {
        return (NULL);
    }

    cpcoeff = coeff;
    cnst = (frdim - 4.0) / 2.0;
    for (y = 0; y < ys / 2 + 1; y++) {
        for (x = 0; x < xs / 2 + 1; x++) {
            distance2 = xd[x] + yd[y];
            if (distance2 == 0.0) {
                *cpcoeff++ = 1.0;
            } else {
                *cpcoeff++ = pow(distance2, cnst);
            }
        }
    }

    return (coeff);
}

float *im__create_quarter(IMAGE *out, int xs, int ys, VipsMaskType flag, va_list ap) {

    double p0 = va_arg(ap, double);
    double p1 = va_arg(ap, double);
    double p2 = va_arg(ap, double);
    double p3 = va_arg(ap, double);

    switch (flag) {

    case VIPS_MASK_IDEAL_HIGHPASS:
        return (ideal_hpf(out, xs, ys, p0));

    case VIPS_MASK_IDEAL_LOWPASS:
        return (ideal_lpf(out, xs, ys, p0));

    case VIPS_MASK_BUTTERWORTH_HIGHPASS:
        return (butterworth_hpf(out, xs, ys, p0, p1, p2));

    case VIPS_MASK_BUTTERWORTH_LOWPASS:
        return (butterworth_lpf(out, xs, ys, p0, p1, p2));

    case VIPS_MASK_GAUSS_HIGHPASS:
        return (gaussian_hpf(out, xs, ys, p0, p1));

    case VIPS_MASK_GAUSS_LOWPASS:
        return (gaussian_lpf(out, xs, ys, p0, p1));

    case VIPS_MASK_IDEAL_RINGPASS:
        return (ideal_rpf(out, xs, ys, p0, p1));

    case VIPS_MASK_IDEAL_RINGREJECT:
        return (ideal_rrf(out, xs, ys, p0, p1));

    case VIPS_MASK_BUTTERWORTH_RINGPASS:
        return (butterworth_rpf(out, xs, ys, p0, p1, p2, p3));

    case VIPS_MASK_BUTTERWORTH_RINGREJECT:
        return (butterworth_rrf(out, xs, ys, p0, p1, p2, p3));

    case VIPS_MASK_GAUSS_RINGPASS:
        return (gaussian_rpf(out, xs, ys, p0, p1, p2));

    case VIPS_MASK_GAUSS_RINGREJECT:
        return (gaussian_rrf(out, xs, ys, p0, p1, p2));

    case VIPS_MASK_FRACTAL_FLT:
        return (fractal_flt(out, xs, ys, p0));

    default:
        im_error("create_quarter", "%s", _("unimplemented mask"));
        return (NULL);
    }
}
