

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

static int flip_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Rect *r = & or->valid;
    Rect in;
    PEL *p, *q;
    int y;

    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int ls;
    int psk, qsk;

    in = *r;
    in.top = ir->im->Ysize - bo;

    if (im_prepare(ir, &in)) {
        return (-1);
    }

    p = (PEL *)IM_REGION_ADDR(ir, le, in.top + in.height - 1);
    q = (PEL *)IM_REGION_ADDR(or, le, to);
    psk = IM_REGION_LSKIP(ir);
    qsk = IM_REGION_LSKIP(or);
    ls = IM_REGION_SIZEOF_LINE(or);

    for (y = to; y < bo; y++) {
        memcpy(q, p, ls);

        p -= psk;
        q += qsk;
    }

    return (0);
}

int im_flipver(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_coding_known("im_flipver", in) || im_cp_desc(out, in) || im_demand_hint(out, IM_THINSTRIP, in, NULL) || im_generate(out, im_start_one, flip_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = in->Ysize;

    return (0);
}
