

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int flip_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Rect *r = & or->valid;
    Rect in;
    char *p, *q;
    int x, y, z;

    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int ps = IM_IMAGE_SIZEOF_PEL(ir->im);

    int hgt = ir->im->Xsize - r->width;

    int lastx;

    in = *r;
    in.left = hgt - r->left;

    lastx = IM_RECT_RIGHT(&in) - 1;

    if (im_prepare(ir, &in)) {
        return (-1);
    }

    for (y = to; y < bo; y++) {
        p = IM_REGION_ADDR(ir, lastx, y);
        q = IM_REGION_ADDR(or, le, y);

        for (x = le; x < ri; x++) {

            for (z = 0; z < ps; z++) {
                q[z] = p[z];
            }

            q += ps;
            p -= ps;
        }
    }

    return (0);
}

int im_fliphor(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_coding_known("im_fliphor", in) || im_cp_desc(out, in) || im_demand_hint(out, IM_THINSTRIP, in, NULL) || im_generate(out, im_start_one, flip_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    out->Xoffset = in->Xsize;
    out->Yoffset = 0;

    return (0);
}
