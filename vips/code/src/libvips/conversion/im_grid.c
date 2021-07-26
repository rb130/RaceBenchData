
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

typedef struct _Grid {
    IMAGE *in;
    IMAGE *out;
    int tile_height;
    int across;
    int down;
} Grid;

static int grid_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Grid *grid = (Grid *)b;
    Rect *r = & or->valid;
    int twidth = grid->in->Xsize;
    int theight = grid->tile_height;
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
        irect.top += grid->across * ys + theight * (xs / twidth);

        if (im_prepare(ir, &irect) || im_region_region(or, ir, r, irect.left, irect.top)) {
            return (-1);
        }

        return (0);
    }

    for (y = ys; y < IM_RECT_BOTTOM(r); y += theight) {
        for (x = xs; x < IM_RECT_RIGHT(r); x += twidth) {
            Rect paint;
            Rect input;

            tile.left = x;
            tile.top = y;
            tile.width = twidth;
            tile.height = theight;

            im_rect_intersectrect(&tile, r, &paint);

            g_assert(!im_rect_isempty(&paint));

            input = paint;
            input.left -= x;
            input.top -= y;
            input.top += grid->across * y + theight * (x / twidth);

            if (im_prepare_to(ir, or, &input, paint.left, paint.top)) {
                return (-1);
            }
        }
    }

    return (0);
}

int im_grid(IMAGE *in, IMAGE *out, int tile_height, int across, int down) {
    Grid *grid = IM_NEW(out, Grid);

    if (!grid || im_piocheck(in, out) || im_check_coding_known("im_grid", in)) {
        return (-1);
    }
    if (across <= 0 || down <= 0) {
        im_error("im_grid", "%s", _("bad parameters"));
        return (-1);
    }
    if (in->Ysize % tile_height != 0 || in->Ysize / tile_height != across * down) {
        im_error("im_grid", "%s", _("bad grid geometry"));
        return (-1);
    }

    grid->in = in;
    grid->out = out;
    grid->tile_height = tile_height;
    grid->across = across;
    grid->down = down;

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = in->Xsize * across;
    out->Ysize = tile_height * down;

    if (im_demand_hint(out, IM_SMALLTILE, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, grid_gen, im_stop_one, in, grid)) {
        return (-1);
    }

    return (0);
}
