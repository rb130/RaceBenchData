

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_mask2vips(DOUBLEMASK *in, IMAGE *out) {
    int x, y;
    double *buf, *p, *q;

    if (!in || !in->coeff) {
        im_error("im_mask2vips", "%s", _("bad input mask"));
        return (-1);
    }

    im_initdesc(out, in->xsize, in->ysize, 1, IM_BBITS_DOUBLE, IM_BANDFMT_DOUBLE, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(buf = IM_ARRAY(out, in->xsize, double))) {
        return (-1);
    }

    for (p = in->coeff, y = 0; y < out->Ysize; y++) {
        q = buf;

        for (x = 0; x < out->Xsize; x++) {
            *q++ = *p++;
        }

        if (im_writeline(y, out, (void *)buf)) {
            return (-1);
        }
    }

    return (0);
}
