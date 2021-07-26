
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

static int rot270_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;

    Rect *r = & or->valid;
    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int x, y, i;

    int ps, ls;

    Rect need;

    need.left = in->Xsize - bo;
    need.top = le;
    need.width = r->height;
    need.height = r->width;
    if (im_prepare(ir, &need)) {
        return (-1);
    }

    ps = IM_IMAGE_SIZEOF_PEL(in);
    ls = IM_REGION_LSKIP(ir);

    for (y = to; y < bo; y++) {

        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        PEL *p = (PEL *)IM_REGION_ADDR(ir, need.left + need.width - (y - to) - 1, need.top);

        for (x = le; x < ri; x++) {
            for (i = 0; i < ps; i++) {
                q[i] = p[i];
            }

            q += ps;
            p += ls;
        }
    }

    return (0);
}

int im_rot270(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_coding_known("im_rot270", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in) || im_demand_hint(out, IM_SMALLTILE, in, NULL)) {
        return (-1);
    }
    out->Xsize = in->Ysize;
    out->Ysize = in->Xsize;

    if (im_generate(out, im_start_one, rot270_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = in->Xsize;

    return (0);
}