

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

typedef struct _Embed {
    IMAGE *in;
    IMAGE *out;
    int type;
    int x;
    int y;
    int width;
    int height;

    Rect rout;
    Rect rsub;

    Rect border[8];
} Embed;

static void embed_find_edge(Embed *embed, Rect *r, int i, Rect *out) {

    *out = embed->border[i];
    im_rect_marginadjust(out, 1);
    im_rect_intersectrect(out, &embed->rsub, out);

    if (i == 0 || i == 2) {
        Rect extend;

        extend = *r;
        extend.top = 0;
        extend.height = embed->height;
        im_rect_intersectrect(out, &extend, out);
    }
    if (i == 1 || i == 3) {
        Rect extend;

        extend = *r;
        extend.left = 0;
        extend.width = embed->width;
        im_rect_intersectrect(out, &extend, out);
    }
}

static void embed_copy_pixel(Embed *embed, PEL *q, PEL *p, int n) {
    const int bs = IM_IMAGE_SIZEOF_PEL(embed->in);

    int x, b;

    for (x = 0; x < n; x++) {
        for (b = 0; b < bs; b++) {
            *q++ = p[b];
        }
    }
}

static void embed_paint_edge(Embed *embed, REGION * or, int i, Rect *r, PEL *p, int plsk) {
    const int bs = IM_IMAGE_SIZEOF_PEL(embed->in);

    Rect todo;
    PEL *q;
    int y;

    todo = *r;

    if (i > 3) {
        q = (PEL *)IM_REGION_ADDR(or, todo.left, todo.top);
        embed_copy_pixel(embed, q, p, todo.width);

        p = q;
        todo.top += 1;
        todo.height -= 1;
    }

    if (i == 1 || i == 3) {

        for (y = 0; y < todo.height; y++) {
            q = (PEL *)IM_REGION_ADDR(or, todo.left, todo.top + y);
            embed_copy_pixel(embed, q, p, todo.width);
            p += plsk;
        }
    } else {

        for (y = 0; y < todo.height; y++) {
            q = (PEL *)IM_REGION_ADDR(or, todo.left, todo.top + y);
            memcpy(q, p, bs * todo.width);
        }
    }
}

static int embed_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Embed *embed = (Embed *)b;
    Rect *r = & or->valid;

    Rect ovl;
    int i;
    PEL *p;
    int plsk;

    if (im_rect_includesrect(&embed->rsub, r)) {
        Rect need;

        need = *r;
        need.left -= embed->x;
        need.top -= embed->y;
        if (im_prepare(ir, &need) || im_region_region(or, ir, r, need.left, need.top)) {
            return (-1);
        }

        return (0);
    }

    im_rect_intersectrect(r, &embed->rsub, &ovl);
    if (!im_rect_isempty(&ovl)) {

        ovl.left -= embed->x;
        ovl.top -= embed->y;
        if (im_prepare_to(ir, or, &ovl, ovl.left + embed->x, ovl.top + embed->y)) {
            return (-1);
        }
        ovl.left += embed->x;
        ovl.top += embed->y;
    }

    switch (embed->type) {
    case 0:
    case 4:

        for (i = 0; i < 8; i++) {
            im_region_paint(or, &embed->border[i], embed->type == 0 ? 0 : 255);
        }
        break;

    case 1:

        for (i = 0; i < 8; i++) {
            Rect todo;
            Rect edge;

            im_rect_intersectrect(r, &embed->border[i], &todo);
            if (!im_rect_isempty(&todo)) {
                embed_find_edge(embed, &todo, i, &edge);

                if (!im_rect_isempty(&ovl)) {
                    p = (PEL *)IM_REGION_ADDR(or, edge.left, edge.top);
                    plsk = IM_REGION_LSKIP(or);
                } else {

                    edge.left -= embed->x;
                    edge.top -= embed->y;
                    if (im_prepare(ir, &edge)) {
                        return (-1);
                    }
                    p = (PEL *)IM_REGION_ADDR(ir, edge.left, edge.top);
                    plsk = IM_REGION_LSKIP(ir);
                }

                embed_paint_edge(embed, or, i, &todo, p, plsk);
            }
        }

        break;

    default:
        g_assert(0);
    }

    return (0);
}

static Embed *embed_new(IMAGE *in, IMAGE *out, int type, int x, int y, int width, int height) {
    Embed *embed = IM_NEW(out, Embed);
    Rect want;

    embed->in = in;
    embed->out = out;
    embed->type = type;
    embed->x = x;
    embed->y = y;
    embed->width = width;
    embed->height = height;

    embed->rout.left = 0;
    embed->rout.top = 0;
    embed->rout.width = out->Xsize;
    embed->rout.height = out->Ysize;

    want.left = x;
    want.top = y;
    want.width = in->Xsize;
    want.height = in->Ysize;
    im_rect_intersectrect(&want, &embed->rout, &embed->rsub);

    if (im_rect_isempty(&embed->rsub)) {
        im_error("im_embed", "%s", _("bad dimensions"));
        return (NULL);
    }

    embed->border[0].left = embed->rsub.left;
    embed->border[0].top = 0;
    embed->border[0].width = embed->rsub.width;
    embed->border[0].height = embed->rsub.top;

    embed->border[1].left = IM_RECT_RIGHT(&embed->rsub);
    embed->border[1].top = embed->rsub.top;
    embed->border[1].width = out->Xsize - IM_RECT_RIGHT(&embed->rsub);
    embed->border[1].height = embed->rsub.height;

    embed->border[2].left = embed->rsub.left;
    embed->border[2].top = IM_RECT_BOTTOM(&embed->rsub);
    embed->border[2].width = embed->rsub.width;
    embed->border[2].height = out->Ysize - IM_RECT_BOTTOM(&embed->rsub);

    embed->border[3].left = 0;
    embed->border[3].top = embed->rsub.top;
    embed->border[3].width = embed->rsub.left;
    embed->border[3].height = embed->rsub.height;

    embed->border[4].left = 0;
    embed->border[4].top = 0;
    embed->border[4].width = embed->rsub.left;
    embed->border[4].height = embed->rsub.top;

    embed->border[5].left = IM_RECT_RIGHT(&embed->rsub);
    embed->border[5].top = 0;
    embed->border[5].width = out->Xsize - IM_RECT_RIGHT(&embed->rsub);
    embed->border[5].height = embed->rsub.top;

    embed->border[6].left = IM_RECT_RIGHT(&embed->rsub);
    embed->border[6].top = IM_RECT_BOTTOM(&embed->rsub);
    embed->border[6].width = out->Xsize - IM_RECT_RIGHT(&embed->rsub);
    embed->border[6].height = out->Ysize - IM_RECT_BOTTOM(&embed->rsub);

    embed->border[7].left = 0;
    embed->border[7].top = IM_RECT_BOTTOM(&embed->rsub);
    embed->border[7].width = embed->rsub.left;
    embed->border[7].height = out->Ysize - IM_RECT_BOTTOM(&embed->rsub);

    return (embed);
}

static int embed(IMAGE *in, IMAGE *out, int type, int x, int y, int width, int height) {
    Embed *embed;

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = width;
    out->Ysize = height;

    if (!(embed = embed_new(in, out, type, x, y, width, height)) || im_demand_hint(out, IM_SMALLTILE, in, NULL) || im_generate(out, im_start_one, embed_gen, im_stop_one, in, embed)) {
        return (-1);
    }

    return (0);
}

int im_embed(IMAGE *in, IMAGE *out, int type, int x, int y, int width, int height) {
    if (im_piocheck(in, out) || im_check_coding_known("im_embed", in)) {
        return (-1);
    }
    if (type < 0 || type > 4) {
        im_error("im_embed", "%s", _("unknown type"));
        return (-1);
    }
    if (width <= 0 || height <= 0) {
        im_error("im_embed", "%s", _("bad dimensions"));
        return (-1);
    }

    if (x == 0 && y == 0 && width == in->Xsize && height == in->Ysize) {
        return (im_copy(in, out));
    }

    switch (type) {
    case 0:
    case 1:
    case 4:
        if (embed(in, out, type, x, y, width, height)) {
            return (-1);
        }
        break;

    case 2: {

        const int nx = x < 0 ? -x % in->Xsize : in->Xsize - x % in->Xsize;
        const int ny = y < 0 ? -y % in->Ysize : in->Ysize - y % in->Ysize;

        IMAGE *t[1];

        if (im_open_local_array(out, t, 1, "embed-type2", "p") || im_replicate(in, t[0], width / in->Xsize + 2, height / in->Ysize + 2) || im_extract_area(t[0], out, nx, ny, width, height)) {
            return (-1);
        }
    } break;

    case 3: {

        const int w2 = in->Xsize * 2;
        const int h2 = in->Ysize * 2;

        const int nx = x < 0 ? -x % w2 : w2 - x % w2;
        const int ny = y < 0 ? -y % h2 : h2 - y % h2;

        IMAGE *t[7];

        if (im_open_local_array(out, t, 7, "embed-type3", "p") ||

            im_copy(in, t[0]) ||

            im_fliphor(t[0], t[1]) || im_lrjoin(t[0], t[1], t[2]) || im_flipver(t[2], t[3]) || im_tbjoin(t[2], t[3], t[4]) ||

            im_replicate(t[4], t[5], width / t[4]->Xsize + 2, height / t[4]->Ysize + 2) || im_extract_area(t[5], t[6], nx, ny, width, height) ||

            im_insert_noexpand(t[6], in, out, x, y)) {
            return (-1);
        }
    } break;

    default:
        g_assert(0);
    }

    out->Xoffset = x;
    out->Yoffset = y;

    return (0);
}
