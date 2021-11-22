

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

static int replicate_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;
    Rect *r = & or->valid;
    int twidth = in->Xsize;
    int theight = in->Ysize;
    int x, y;
    Rect tile;

    int xs = (r->left / twidth) * twidth;
    int ys = (r->top / theight) * theight;

    tile.left = xs;
    tile.top = ys;
    tile.width = twidth;
    tile.height = theight;

    if (im_rect_includesrect(&tile, r)) {
        Rect irect;

        irect = *r;
        irect.left -= xs;
        irect.top -= ys;
        if (im_prepare(ir, &irect)) {
            return (-1);
        }

        if (im_region_region(or, ir, r, irect.left, irect.top)) {
            return (-1);
        }

        return (0);
    }

    for (y = ys; y < IM_RECT_BOTTOM(r); y += theight) {
        for (x = xs; x < IM_RECT_RIGHT(r); x += twidth) {
            Rect paint;

            tile.left = x;
            tile.top = y;
            tile.width = twidth;
            tile.height = theight;

            im_rect_intersectrect(&tile, r, &paint);

            paint.left -= x;
            paint.top -= y;

            g_assert(!im_rect_isempty(&paint));

            if (im_prepare_to(ir, or, &paint, paint.left + x, paint.top + y)) {
                return (-1);
            }
        }
    }

    return (0);
}

int im_replicate(IMAGE *in, IMAGE *out, int across, int down) {
    if (across <= 0 || down <= 0) {
        im_error("im_replicate", "%s", _("bad parameters"));
        return (-1);
    }
    if (im_piocheck(in, out) || im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize *= across;
    out->Ysize *= down;

    if (im_demand_hint(out, IM_SMALLTILE, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, replicate_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    return (0);
}
