

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/util.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAXMASK 256

DOUBLEMASK *im_log_dmask(const char *filename, double sigma, double min_ampl) {
    const double sig2 = sigma * sigma;

    double last;
    int x, y, k;

    double *pt1, *pt2, *pt3, *pt4;
    int xm, ym;
    int xm2, ym2;
    int offset;
    double *cf, *cfs, *mc;
    DOUBLEMASK *m;
    double sum;

    last = 0.0;

    for (x = 0; x < IM_MAXMASK; x++) {
        const double distance = x * x;
        double val;

        val = 0.5 * (2.0 - (distance / sig2)) * exp(-distance / (2.0 * sig2));

        if (x > 0 && fabs(val) < min_ampl && fabs(val - last) < min_ampl) {
            break;
        }

        last = val;
    }
    if (x == IM_MAXMASK) {
        im_error("im_log_dmask", "%s", _("mask too large"));
        return (NULL);
    }

    xm2 = x;
    ym2 = x;
    xm = xm2 * 2 + 1;
    ym = ym2 * 2 + 1;

    if (!(cfs = IM_ARRAY(NULL, (xm2 + 1) * (ym2 + 1), double))) {
        return (NULL);
    }

    for (k = 0, y = 0; y <= ym2; y++) {
        for (x = 0; x <= xm2; x++, k++) {
            const double distance = x * x + y * y;

            cfs[k] = 0.5 * (2.0 - (distance / sig2)) * exp(-distance / (2.0 * sig2));
        }
    }

#ifdef PIM_RINT
    for (k = 0, y = 0; y <= ym2; y++) {
        for (x = 0; x <= xm2; x++, k++)
            fprintf(stderr, "%3.2f ", cfs[k]);
        fprintf(stderr, "\n");
    }
#endif

    if (!(m = im_create_dmask(filename, xm, ym))) {
        im_free(cfs);
        return (NULL);
    }

    cf = cfs;
    offset = xm2 * (xm + 1);
    mc = m->coeff + offset;
    for (y = 0; y <= ym2; y++) {
        for (x = 0; x <= xm2; x++) {
            pt1 = mc + (y * xm) + x;
            pt2 = mc - (y * xm) + x;
            pt3 = mc + (y * xm) - x;
            pt4 = mc - (y * xm) - x;

            *pt1 = cf[x];
            *pt2 = cf[x];
            *pt3 = cf[x];
            *pt4 = cf[x];
        }

        cf += (xm2 + 1);
    }
    im_free(cfs);

    sum = 0.0;
    for (k = 0, y = 0; y < m->ysize; y++) {
        for (x = 0; x < m->xsize; x++, k++) {
            sum += m->coeff[k];
        }
    }
    m->scale = sum;
    m->offset = 0.0;

#ifdef PIM_RINT
    im_print_dmask(m);
#endif

    return (m);
}

INTMASK *im_log_imask(const char *filename, double sigma, double min_ampl) {
    DOUBLEMASK *dm;
    INTMASK *im;

    if (!(dm = im_log_dmask(filename, sigma, min_ampl))) {
        return (NULL);
    }

    if (!(im = im_scale_dmask(dm, dm->filename))) {
        im_free_dmask(dm);
        return (NULL);
    }
    im_free_dmask(dm);

    return (im);
}
