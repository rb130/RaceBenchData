
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

DOUBLEMASK *im_matcat(DOUBLEMASK *in1, DOUBLEMASK *in2, const char *name) {
    int newxsize, newysize;
    DOUBLEMASK *mat;
    double *out;

    if (in1->xsize != in2->xsize) {
        im_error("im_matcat", "%s", _("matrices must be same width"));
        return (NULL);
    }

    newxsize = in1->xsize;
    newysize = in1->ysize + in2->ysize;

    if (!(mat = im_create_dmask(name, newxsize, newysize))) {
        im_error("im_matcat", "%s", _("unable to allocate output matrix"));
        return (NULL);
    }

    memcpy(mat->coeff, in1->coeff, in1->xsize * in1->ysize * sizeof(double));
    out = mat->coeff + in1->xsize * in1->ysize;
    memcpy(out, in2->coeff, in2->xsize * in2->ysize * sizeof(double));

    return (mat);
}
