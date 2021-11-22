

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

DOUBLEMASK *im_matmul(DOUBLEMASK *in1, DOUBLEMASK *in2, const char *name) {
    int xc, yc, col;
    double sum;
    DOUBLEMASK *mat;
    double *out, *a, *b;
    double *s1, *s2;

    if (in1->xsize != in2->ysize) {
        im_error("im_matmul", "%s", _("bad sizes"));
        return (NULL);
    }

    if (!(mat = im_create_dmask(name, in2->xsize, in1->ysize))) {
        im_error("im_matmul", "%s", _("unable to allocate output mask"));
        return (NULL);
    }

    out = mat->coeff;
    s1 = in1->coeff;

    for (yc = 0; yc < in1->ysize; yc++) {
        s2 = in2->coeff;

        for (col = 0; col < in2->xsize; col++) {

            sum = 0.0;
            a = s1;
            b = s2;

            for (sum = 0.0, xc = 0; xc < in1->xsize; xc++) {
                sum += *a++ * *b;
                b += in2->xsize;
            }

            *out++ = sum;
            s2++;
        }

        s1 += in1->xsize;
    }

    return (mat);
}
