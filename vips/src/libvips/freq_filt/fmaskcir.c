

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

static int alloc(IMAGE *out, int xs, int ys, int **xd, int **yd, int **xplusd, int **xminusd, int **yplusd, int **yminusd, int x0, int y0, float **line) {
    int i;
    int *x, *y, *xp, *xm, *yp, *ym;
    int *pp, *pm;
    float *l;

    x = IM_ARRAY(out, xs, int);
    y = IM_ARRAY(out, ys, int);
    xp = IM_ARRAY(out, xs, int);
    xm = IM_ARRAY(out, xs, int);
    yp = IM_ARRAY(out, ys, int);
    ym = IM_ARRAY(out, ys, int);
    l = IM_ARRAY(out, xs, float);

    if (!x || !y || !xp || !xm || !yp || !ym || !l) {
        return (-1);
    }

    for (i = 0; i < ys / 2; i++) {
        y[i] = i;
        y[i + ys / 2] = -ys / 2 + i;
    }
    for (i = 0; i < xs / 2; i++) {
        x[i] = i;
        x[i + xs / 2] = -xs / 2 + i;
    }
    *xd = x;
    *yd = y;

    pp = yp + ys / 2;
    pm = ym + ys / 2;
    for (i = -ys / 2; i < ys / 2; i++) {
        pp[i] = (i + y0) * (i + y0);
        pm[i] = (i - y0) * (i - y0);
    }
    *yplusd = yp + ys / 2;
    *yminusd = ym + ys / 2;

    pp = xp + xs / 2;
    pm = xm + xs / 2;
    for (i = -xs / 2; i < xs / 2; i++) {
        pp[i] = (i + x0) * (i + x0);
        pm[i] = (i - x0) * (i - x0);
    }
    *xplusd = xp + xs / 2;
    *xminusd = xm + xs / 2;

    *line = l;

    return (0);
}

static int ideal_bpf(IMAGE *out, double fcx, double fcy, double r) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0, d1_2, d2_2, r2;
    int y2plus, y2minus;

    if (xs != ys) {
        im_error("ideal_bpf", "%s", _("bad sizes"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) < 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        r2 = r * r * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        r2 = r * r;
    } else {
        im_error("ideal_bpf", "%s", _("bad args"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            if (d1_2 <= r2) {
                *cpline = 1.0;
            } else if (d2_2 <= r2) {
                *cpline = 1.0;
            } else {
                *cpline = 0.0;
            }

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int ideal_brf(IMAGE *out, double fcx, double fcy, double r) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0, d1_2, d2_2, r2;
    int y2plus, y2minus;

    if (xs != ys) {
        im_error("ideal_brf", "%s", _("bad args"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) <= 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        r2 = r * r * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        r2 = r * r;
    } else {
        im_error("ideal_brf", "%s", _("bad args"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            if (d1_2 <= r2) {
                *cpline = 0.0;
            } else if (d2_2 <= r2) {
                *cpline = 0.0;
            } else {
                *cpline = 1.0;
            }

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int butterworth_bpf(IMAGE *out, double order, double fcx, double fcy, double r, double ac) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0;
    double cnst, cnsta, d1_2, d2_2, nr2;
    int y2plus, y2minus;

    if (xs != ys || order < 1.0) {
        im_error("butterworth_bpf", "%s", _("bad sizes"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) <= 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        nr2 = r * r * xs * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        nr2 = r * r;
    } else {
        im_error("butterworth_bpf", "%s", _("bad args"));
        return (-1);
    }
    if (ac >= 1.0 || ac < 0.0) {
        im_error("butterworth_bpf", "%s", _("bad args"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    cnst = (1.0 / ac) - 1.0;

    cnsta = 1.0 / (1.0 + 1.0 / (1.0 + cnst * pow(4.0 * (x0 * x0 + y0 * y0) / nr2, order)));

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            *cpline = cnsta * (1.0 / (1.0 + cnst * pow(d1_2 / nr2, order)) + 1.0 / (1.0 + cnst * pow(d2_2 / nr2, order)));

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int butterworth_brf(IMAGE *out, double order, double fcx, double fcy, double r, double ac) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0;
    double cnst, cnsta, d1_2, d2_2, nr2;
    int y2plus, y2minus;

    if (xs != ys || order < 1.0) {
        im_error("butterworth_brf", "%s", _("bad sizes"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) <= 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        nr2 = r * r * xs * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        nr2 = r * r;
    } else {
        im_error("butterworth_brf", "%s", _("bad args"));
        return (-1);
    }
    if (ac >= 1.0 || ac < 0.0) {
        im_error("butterworth_brf", "%s", _("bad args"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    cnst = (1.0 / ac) - 1.0;

    cnsta = 1.0 / (1.0 + 1.0 / (1.0 + cnst * pow(4.0 * (x0 * x0 + y0 * y0) / nr2, order)));

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            if (d1_2 == 0.0 || d2_2 == 0.0) {
                *cpline = 0;
            } else {
                *cpline = 1.0 - cnsta * (1.0 / (1.0 + cnst * pow(d1_2 / nr2, order)) + 1.0 / (1.0 + cnst * pow(d2_2 / nr2, order)));
            }

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int gaussian_bpf(IMAGE *out, double fcx, double fcy, double r, double ac) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0;
    double cnst, cnsta, d1_2, d2_2, nr2;
    int y2plus, y2minus;

    if (xs != ys) {
        im_error("gauss_bpf", "%s", _("bad sizes"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) <= 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        nr2 = r * r * xs * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        nr2 = r * r;
    } else {
        im_error("gauss_bpf", "%s", _("bad args (f)"));
        return (-1);
    }
    if (ac >= 1.0 || ac < 0.0) {
        im_error("gauss_bpf", "%s", _("bad args (ac)"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    cnst = -log(ac);

    cnsta = 1.0 / (1.0 + exp(-cnst * 4.0 * (x0 * x0 + y0 * y0) / nr2));

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            *cpline = cnsta * (exp(-cnst * d1_2 / nr2) + exp(-cnst * d2_2 / nr2));

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int gaussian_brf(IMAGE *out, double fcx, double fcy, double r, double ac) {
    int x, y;
    int xs = out->Xsize;
    int ys = out->Ysize;
    float *line, *cpline;
    int *xd, *yd, *xplusx0d, *xminusx0d, *yplusy0d, *yminusy0d;
    int x0, y0;
    double cnst, cnsta, d1_2, d2_2, nr2;
    int y2plus, y2minus;

    if (xs != ys) {
        im_error("gauss_brf", "%s", _("bad sizes"));
        return (-1);
    }
    if (fabs(fcx) <= 1.0 && fabs(fcy) <= 1.0 && r > 0.0 && r < 1.0) {
        x0 = fcx * xs / 2.0;
        y0 = fcy * ys / 2.0;
        nr2 = r * r * xs * xs / 4.0;
    } else if (fabs(fcx) < xs / 2 && fabs(fcy) < ys / 2 && r >= 1.0) {
        x0 = fcx;
        y0 = fcy;
        nr2 = r * r;
    } else {
        im_error("gauss_brf", "%s", _("bad args"));
        return (-1);
    }
    if (ac >= 1.0 || ac < 0.0) {
        im_error("gauss_brf", "%s", _("bad args"));
        return (-1);
    }

    if (alloc(out, xs, ys, &xd, &yd, &xplusx0d, &xminusx0d, &yplusy0d, &yminusy0d, x0, y0, &line)) {
        return (-1);
    }

    cnst = -log(ac);

    cnsta = 1.0 / (1.0 + exp(-cnst * 4.0 * (x0 * x0 + y0 * y0) / nr2));

    for (y = 0; y < ys; y++) {
        cpline = line;
        y2plus = yplusy0d[yd[y]];
        y2minus = yminusy0d[yd[y]];

        for (x = 0; x < xs; x++) {
            d1_2 = xminusx0d[xd[x]] + y2minus;
            d2_2 = xplusx0d[xd[x]] + y2plus;

            *cpline = 1.0 - cnsta * (exp(-cnst * d1_2 / nr2) + exp(-cnst * d2_2 / nr2));

            if (x == 0 && y == 0) {
                *cpline = 1.0;
            }

            cpline++;
        }

        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

int im__fmaskcir(IMAGE *out, VipsMaskType flag, va_list ap) {

    double p0 = va_arg(ap, double);
    double p1 = va_arg(ap, double);
    double p2 = va_arg(ap, double);
    double p3 = va_arg(ap, double);
    double p4 = va_arg(ap, double);

    switch (flag) {

    case VIPS_MASK_IDEAL_BANDPASS:
        return (ideal_bpf(out, p0, p1, p2));

    case VIPS_MASK_IDEAL_BANDREJECT:
        return (ideal_brf(out, p0, p1, p2));

    case VIPS_MASK_BUTTERWORTH_BANDPASS:
        return (butterworth_bpf(out, p0, p1, p2, p3, p4));

    case VIPS_MASK_BUTTERWORTH_BANDREJECT:
        return (butterworth_brf(out, p0, p1, p2, p3, p4));

    case VIPS_MASK_GAUSS_BANDPASS:
        return (gaussian_bpf(out, p0, p1, p2, p3));

    case VIPS_MASK_GAUSS_BANDREJECT:
        return (gaussian_brf(out, p0, p1, p2, p3));

    default:
        im_error("im__fmaskcir", "%s", _("unimplemented mask"));
        return (-1);
    }
}
