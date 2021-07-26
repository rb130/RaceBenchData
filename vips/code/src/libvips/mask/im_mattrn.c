
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

DOUBLEMASK *im_mattrn(DOUBLEMASK *in, const char *name) {
    int xc, yc;
    DOUBLEMASK *mat;
    double *out, *a, *b;

    if (!(mat = im_create_dmask(name, in->ysize, in->xsize))) {
        im_error("im_mattrn", "%s", _("unable to allocate output matrix"));
        return (NULL);
    }

    out = mat->coeff;
    a = in->coeff;

    for (yc = 0; yc < mat->ysize; yc++) {
        b = a;

        for (xc = 0; xc < mat->xsize; xc++) {
            *out++ = *b;
            b += in->xsize;
        }

        a++;
    }

    return (mat);
}
