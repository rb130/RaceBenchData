

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static IMAGE **im__insert_base(const char *domain, IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t[4];
    IMAGE **vec;

    if (im_piocheck(in1, out) || im_pincheck(in2) || im_check_bands_1orn(domain, in1, in2) || im_check_coding_known(domain, in1) || im_check_coding_same(domain, in1, in2)) {
        return (NULL);
    }

    if (im_open_local_array(out, t, 4, domain, "p") || im__formatalike(in1, in2, t[0], t[1]) || im__bandalike(t[0], t[1], t[2], t[3]) || !(vec = im_allocate_input_array(out, t[2], t[3], NULL))) {
        return (NULL);
    }

    if (im_cp_descv(out, vec[0], vec[1], NULL) || im_demand_hint_array(out, IM_SMALLTILE, vec)) {
        return (NULL);
    }

    return (vec);
}

typedef struct {

    IMAGE *main;
    IMAGE *sub;
    IMAGE *out;
    int x, y;

    Rect rout;
    Rect rmain;
    Rect rsub;
} InsertState;

static int just_one(REGION * or, REGION *ir, int x, int y) {
    Rect need;

    need = or->valid;
    need.left -= x;
    need.top -= y;
    if (im_prepare(ir, &need)) {
        return (-1);
    }

    if (im_region_region(or, ir, & or->valid, need.left, need.top)) {
        return (-1);
    }

    return (0);
}

static int paste_region(REGION * or, REGION *ir, Rect *pos) {
    Rect ovl;

    im_rect_intersectrect(& or->valid, pos, &ovl);
    if (!im_rect_isempty(&ovl)) {

        ovl.left -= pos->left;
        ovl.top -= pos->top;

        if (im_prepare_to(ir, or, &ovl, ovl.left + pos->left, ovl.top + pos->top)) {
            return (-1);
        }
    }

    return (0);
}

static int insert_gen(REGION * or, void *seq, void *a, void *b) {
    REGION **ir = (REGION **)seq;
    InsertState *ins = (InsertState *)b;
    Rect ovl;

    if (im_rect_includesrect(&ins->rsub, & or->valid)) {
        return (just_one(or, ir[1], ins->rsub.left, ins->rsub.top));
    }

    im_rect_intersectrect(& or->valid, &ins->rsub, &ovl);
    if (im_rect_includesrect(&ins->rmain, & or->valid) && im_rect_isempty(&ovl)) {
        return (just_one(or, ir[0], ins->rmain.left, ins->rmain.top));
    }

    if (!(im_rect_includesrect(&ins->rsub, & or->valid) && im_rect_includesrect(&ins->rmain, & or->valid))) {

        im_region_black(or);
    }

    if (paste_region(or, ir[0], &ins->rmain)) {
        return (-1);
    }

    if (paste_region(or, ir[1], &ins->rsub)) {
        return (-1);
    }

    return (0);
}

#define RANGE (10000000)

int im_insert(IMAGE *main, IMAGE *sub, IMAGE *out, int x, int y) {
    InsertState *ins;
    IMAGE **vec;

    if (x > RANGE || x < -RANGE || y > RANGE || y < -RANGE) {
        im_error("im_insert", "%s", _("xy out of range"));
        return (-1);
    }
    if (!(ins = IM_NEW(out, InsertState)) || !(vec = im__insert_base("im_insert", main, sub, out))) {
        return (-1);
    }

    ins->main = vec[0];
    ins->sub = vec[1];
    ins->out = out;
    ins->x = x;
    ins->y = y;

    ins->rmain.left = 0;
    ins->rmain.top = 0;
    ins->rmain.width = vec[0]->Xsize;
    ins->rmain.height = vec[0]->Ysize;
    ins->rsub.left = x;
    ins->rsub.top = y;
    ins->rsub.width = vec[1]->Xsize;
    ins->rsub.height = vec[1]->Ysize;

    im_rect_unionrect(&ins->rmain, &ins->rsub, &ins->rout);

    ins->rmain.left -= ins->rout.left;
    ins->rmain.top -= ins->rout.top;
    ins->rsub.left -= ins->rout.left;
    ins->rsub.top -= ins->rout.top;
    ins->rout.left = 0;
    ins->rout.top = 0;

    out->Xsize = ins->rout.width;
    out->Ysize = ins->rout.height;

    if (im_generate(out, im_start_many, insert_gen, im_stop_many, vec, ins)) {
        return (-1);
    }

    out->Xoffset = ins->rmain.left;
    out->Yoffset = ins->rmain.top;

    return (0);
}

int im_insert_noexpand(IMAGE *main, IMAGE *sub, IMAGE *out, int x, int y) {
    InsertState *ins;
    IMAGE **vec;

    if (x > RANGE || x < -RANGE || y > RANGE || y < -RANGE) {
        im_error("im_insert", "%s", _("xy out of range"));
        return (-1);
    }
    if (!(ins = IM_NEW(out, InsertState)) || !(vec = im__insert_base("im_insert", main, sub, out))) {
        return (-1);
    }

    ins->main = vec[0];
    ins->sub = vec[1];
    ins->out = out;
    ins->x = x;
    ins->y = y;

    ins->rmain.left = 0;
    ins->rmain.top = 0;
    ins->rmain.width = vec[0]->Xsize;
    ins->rmain.height = vec[0]->Ysize;
    ins->rsub.left = x;
    ins->rsub.top = y;
    ins->rsub.width = vec[1]->Xsize;
    ins->rsub.height = vec[1]->Ysize;
    ins->rout = ins->rmain;

    out->Xsize = ins->rout.width;
    out->Ysize = ins->rout.height;

    if (im_generate(out, im_start_many, insert_gen, im_stop_many, vec, ins)) {
        return (-1);
    }

    return (0);
}

int im_insertset(IMAGE *main, IMAGE *sub, IMAGE *out, int n, int *x, int *y) {
    IMAGE **vec;
    IMAGE *t;
    int i;

    if (!(vec = im__insert_base("im_insert", main, sub, out))) {
        return (-1);
    }

    if (!(t = im_open_local(out, "im_insertset", "t")) || im_copy(vec[0], t)) {
        return (-1);
    }

    for (i = 0; i < n; i++) {
        if (im_insertplace(t, vec[1], x[i], y[i])) {
            return (-1);
        }
    }

    if (im_copy(t, out)) {
        return (-1);
    }

    return (0);
}
