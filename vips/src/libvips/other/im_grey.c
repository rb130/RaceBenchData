

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

static int fgrey_gen(REGION * or, void *seq, void *a, void *b) {
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int iwm = or->im->Xsize - 1;

    int x, y;

    for (y = 0; y < r->height; y++) {
        float *q = (float *)IM_REGION_ADDR(or, le, y + to);

        for (x = 0; x < r->width; x++) {
            q[x] = (float)(x + le) / iwm;
        }
    }

    return (0);
}

int im_fgrey(IMAGE *out, const int xsize, const int ysize) {

    if (xsize <= 0 || ysize <= 0) {
        im_error("im_fgrey", "%s", _("bad size"));
        return (-1);
    }
    if (im_poutcheck(out)) {
        return (-1);
    }

    im_initdesc(out, xsize, ysize, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);

    if (im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (im_generate(out, NULL, fgrey_gen, NULL, NULL, NULL)) {
        return (-1);
    }

    return (0);
}

int im_grey(IMAGE *image, const int xsize, const int ysize) {
    IMAGE *t1 = im_open_local(image, "im_grey:1", "p");
    IMAGE *t2 = im_open_local(image, "im_grey:2", "p");

    if (!t1 || !t2) {
        return (-1);
    }

    if (im_fgrey(t1, xsize, ysize) || im_lintra(255.0, t1, 0.0, t2) || im_clip2fmt(t2, image, IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    return (0);
}
