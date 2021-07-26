
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int rot180_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;

    Rect *r = & or->valid;
    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int x, y;

    int ps;

    Rect need;

    need.left = in->Xsize - ri;
    need.top = in->Ysize - bo;
    need.width = r->width;
    need.height = r->height;
    if (im_prepare(ir, &need)) {
        return (-1);
    }

    ps = IM_IMAGE_SIZEOF_PEL(in);

    for (y = to; y < bo; y++) {

        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        PEL *p = (PEL *)IM_REGION_ADDR(ir, need.left + need.width - 1, need.top + need.height - (y - to) - 1);

        for (x = le; x < ri; x++) {
            memcpy(q, p, ps);
            q += ps;
            p -= ps;
        }
    }

    return (0);
}

int im_rot180(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_coding_known("im_rot180", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in) || im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, rot180_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    out->Xoffset = in->Xsize;
    out->Yoffset = in->Ysize;

    return (0);
}
