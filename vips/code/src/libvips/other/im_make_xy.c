
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int make_xy_gen(REGION * or, void *seq, void *a, void *b) {
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int ri = IM_RECT_RIGHT(r);
    int bo = IM_RECT_BOTTOM(r);

    int x, y;

    for (y = to; y < bo; y++) {
        unsigned int *q = (unsigned int *)IM_REGION_ADDR(or, le, y);

        for (x = le; x < ri; x++) {
            q[0] = x;
            q[1] = y;
            q += 2;
        }
    }

    return (0);
}

int im_make_xy(IMAGE *out, const int xsize, const int ysize) {

    if (xsize <= 0 || ysize <= 0) {
        im_error("im_make_xy", "%s", _("bad size"));
        return (-1);
    }
    if (im_poutcheck(out)) {
        return (-1);
    }

    im_initdesc(out, xsize, ysize, 2, IM_BBITS_INT, IM_BANDFMT_UINT, IM_CODING_NONE, IM_TYPE_MULTIBAND, 1.0, 1.0, 0, 0);

    if (im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (im_generate(out, NULL, make_xy_gen, NULL, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
