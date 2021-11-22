

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

#define loop(TYPE)                                \
    for (i = 0; i < ne; i++) {                    \
        TYPE p1 = ((TYPE *)p)[i];                 \
        TYPE p2 = ((TYPE *)p)[i + ba];            \
                                                  \
        if (flag == 1 && p1 > 0 && p2 <= 0)       \
            q[i] = 255;                           \
        else if (flag == -1 && p1 < 0 && p2 >= 0) \
            q[i] = 255;                           \
        else                                      \
            q[i] = 0;                             \
    }

static int zerox_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *in = (IMAGE *)a;
    int flag = GPOINTER_TO_INT(b);
    Rect irect;
    Rect *r = & or->valid;

    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int ba = in->Bands;
    int ne = ba * r->width;

    int i, y;

    irect.top = r->top;
    irect.left = r->left;
    irect.width = r->width + 1;
    irect.height = r->height;
    if (im_prepare(ir, &irect)) {
        return (-1);
    }

    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y);
        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        switch (in->BandFmt) {
        case IM_BANDFMT_CHAR:
            loop(signed char);
            break;
        case IM_BANDFMT_SHORT:
            loop(signed short);
            break;
        case IM_BANDFMT_INT:
            loop(signed int);
            break;
        case IM_BANDFMT_FLOAT:
            loop(float);
            break;
        case IM_BANDFMT_DOUBLE:
            loop(double);
            break;

        default:
            error_exit("im_zerox: internal error");
        }
    }

    return (0);
}

int im_zerox(IMAGE *in, IMAGE *out, int flag) {
    IMAGE *t1 = im_open_local(out, "im_zerox#1", "p");

    if (!t1) {
        return (-1);
    }
    if (flag != -1 && flag != 1) {
        im_error("im_zerox", "%s", _("flag not -1 or 1"));
        return (-1);
    }
    if (im_piocheck(in, t1)) {
        return (-1);
    }
    if (vips_bandfmt_iscomplex(in->BandFmt) || in->Coding != IM_CODING_NONE) {
        im_error("im_zerox", "%s", _("non-complex uncoded only"));
        return (-1);
    }
    if (in->Xsize < 2) {
        im_error("im_zerox", "%s", _("image too narrow"));
        return (-1);
    }
    if (vips_bandfmt_isuint(in->BandFmt)) {

        return (im_black(out, in->Xsize, in->Ysize, in->Bands));
    }

    if (im_cp_desc(t1, in)) {
        return (-1);
    }
    t1->BandFmt = IM_BANDFMT_UCHAR;
    t1->Xsize -= 1;

    if (im_demand_hint(t1, IM_THINSTRIP, NULL)) {
        return (-1);
    }

    if (im_generate(t1, im_start_one, zerox_gen, im_stop_one, in, GINT_TO_POINTER(flag))) {
        return (-1);
    }

    if (im_embed(t1, out, 0, 0, 0, in->Xsize, in->Ysize)) {
        return (-1);
    }

    return (0);
}
