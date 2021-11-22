

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

static int measure_patches(IMAGE *im, double *coeff, int left, int top, int width, int height, int u, int v, int *sel, int nsel) {
    IMAGE *tmp;
    int patch;
    int i, j;
    int m, n;
    double avg, dev;
    int x, y, w, h;

    double pw = (double)width / (double)u;
    double ph = (double)height / (double)v;

    w = (pw + 1) / 2;
    h = (ph + 1) / 2;

    for (j = 0, patch = 0; patch < nsel; patch++) {

        if (sel[patch] <= 0 || sel[patch] > u * v) {
            im_error("im_measure", _("patch %d is out of range"), sel[patch]);
            return (1);
        }

        m = (sel[patch] - 1) % u;
        n = (sel[patch] - 1) / u;

        x = left + m * pw + (pw + 2) / 4;
        y = top + n * ph + (ph + 2) / 4;

        for (i = 0; i < im->Bands; i++, j++) {

            if (!(tmp = im_open("patch", "t"))) {
                return (-1);
            }

            if (im_extract_areabands(im, tmp, x, y, w, h, i, 1) || im_avg(tmp, &avg) || im_deviate(tmp, &dev)) {
                im_close(tmp);
                return (-1);
            }
            im_close(tmp);

            if (dev * 5 > fabs(avg) && fabs(avg) > 3) {
                im_warn("im_measure",
                    _("patch %d, band %d: "
                      "avg = %g, sdev = %g"),
                    patch, i, avg, dev);
            }

            coeff[j] = avg;
        }
    }

    return (0);
}

DOUBLEMASK *im_measure_area(IMAGE *im, int left, int top, int width, int height, int u, int v, int *sel, int nsel, const char *name) {
    DOUBLEMASK *mask;

    if (im->Coding == IM_CODING_LABQ) {
        IMAGE *t1;

        if (!(t1 = im_open("measure-temp", "p"))) {
            return (NULL);
        }
        if (im_LabQ2Lab(im, t1) || !(mask = im_measure_area(t1, left, top, width, height, u, v, sel, nsel, name))) {
            im_close(t1);
            return (NULL);
        }

        im_close(t1);

        return (mask);
    }

    if (im_check_uncoded("im_measure", im) || im_check_noncomplex("im_measure", im)) {
        return (NULL);
    }

    if (sel == NULL) {
        int i;

        nsel = u * v;
        if (!(sel = IM_ARRAY(im, nsel, int))) {
            return (NULL);
        }
        for (i = 0; i < nsel; i++) {
            sel[i] = i + 1;
        }
    }

    if (!(mask = im_create_dmask(name, im->Bands, nsel))) {
        return (NULL);
    }

    if (measure_patches(im, mask->coeff, left, top, width, height, u, v, sel, nsel)) {
        im_free_dmask(mask);
        return (NULL);
    }

    return (mask);
}
