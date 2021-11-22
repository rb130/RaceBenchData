

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int invert_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;

    int le = or->valid.left;
    int to = or->valid.top;
    int bo = IM_RECT_BOTTOM(& or->valid);

    int x, y;

    if (im_prepare(ir, & or->valid)) {
        return (-1);
    }

    for (y = to; y < bo; y++) {

        PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y);
        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        for (x = 0; x < IM_REGION_N_ELEMENTS(or); x++) {
            q[x] = 255 - p[x];
        }
    }

    return (0);
}

int im_invert(IMAGE *in, IMAGE *out) {

    if (im_check_uncoded("im_invert", in) || im_check_format("im_invert", in, IM_BANDFMT_UCHAR) || im_piocheck(in, out)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, invert_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    return (0);
}
