

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

#define IM_MAX_WIDTH (100)

typedef struct {
    int xshrink;
    int yshrink;
} SubsampleInfo;

static int line_shrink_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;
    SubsampleInfo *st = (SubsampleInfo *)b;
    Rect *r = & or->valid;

    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int ps = IM_IMAGE_SIZEOF_PEL(in);

    int owidth = IM_MAX_WIDTH / st->xshrink;

    Rect s;
    int x, y;
    int z, k;

    for (y = to; y < bo; y++) {
        char *q = IM_REGION_ADDR(or, le, y);
        char *p;

        for (x = le; x < ri; x += owidth) {

            int ow = IM_MIN(owidth, ri - x);

            int iw = ow * st->xshrink - (st->xshrink - 1);

            s.left = x * st->xshrink;
            s.top = y * st->yshrink;
            s.width = iw;
            s.height = 1;
            if (im_prepare(ir, &s)) {
                return (-1);
            }

            p = IM_REGION_ADDR(ir, s.left, s.top);
            for (z = 0; z < ow; z++) {
                for (k = 0; k < ps; k++) {
                    q[k] = p[k];
                }

                q += ps;
                p += ps * st->xshrink;
            }
        }
    }

    return (0);
}

static int point_shrink_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;
    SubsampleInfo *st = (SubsampleInfo *)b;
    Rect *r = & or->valid;

    int le = r->left;
    int ri = IM_RECT_RIGHT(r);
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    int ps = IM_IMAGE_SIZEOF_PEL(in);

    Rect s;
    int x, y;
    int k;

    for (y = to; y < bo; y++) {
        char *q = IM_REGION_ADDR(or, le, y);
        char *p;

        for (x = le; x < ri; x++) {

            s.left = x * st->xshrink;
            s.top = y * st->yshrink;
            s.width = 1;
            s.height = 1;
            if (im_prepare(ir, &s)) {
                return (-1);
            }

            p = IM_REGION_ADDR(ir, s.left, s.top);
            for (k = 0; k < ps; k++) {
                q[k] = p[k];
            }
            q += ps;
        }
    }

    return (0);
}

int im_subsample(IMAGE *in, IMAGE *out, int xshrink, int yshrink) {
    SubsampleInfo *st;

    if (xshrink < 1 || yshrink < 1) {
        im_error("im_subsample", "%s", _("factors should both be >= 1"));
        return (-1);
    }
    if (xshrink == 1 && yshrink == 1) {
        return (im_copy(in, out));
    }
    if (im_piocheck(in, out) || im_check_coding_known("im_subsample", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = in->Xsize / xshrink;
    out->Ysize = in->Ysize / yshrink;
    out->Xres = in->Xres / xshrink;
    out->Yres = in->Yres / yshrink;
    if (out->Xsize <= 0 || out->Ysize <= 0) {
        im_error("im_subsample", "%s", _("image has shrunk to nothing"));
        return (-1);
    }

    if (!(st = IM_NEW(out, SubsampleInfo))) {
        return (-1);
    }
    st->xshrink = xshrink;
    st->yshrink = yshrink;

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return (-1);
    }

    if (xshrink > 10) {
        if (im_generate(out, im_start_one, point_shrink_gen, im_stop_one, in, st)) {
            return (-1);
        }
    } else {
        if (im_generate(out, im_start_one, line_shrink_gen, im_stop_one, in, st)) {
            return (-1);
        }
    }

    return (0);
}
