
#define DEBUG

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/transform.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__transform_calc_inverse(Transformation *trn) {
    DOUBLEMASK *msk, *msk2;

    if (!(msk = im_create_dmaskv("boink", 2, 2, trn->a, trn->b, trn->c, trn->d))) {
        return (-1);
    }
    if (!(msk2 = im_matinv(msk, "boink2"))) {
        (void)im_free_dmask(msk);
        return (-1);
    }
    trn->ia = msk2->coeff[0];
    trn->ib = msk2->coeff[1];
    trn->ic = msk2->coeff[2];
    trn->id = msk2->coeff[3];
    (void)im_free_dmask(msk);
    (void)im_free_dmask(msk2);

    return (0);
}

void im__transform_init(Transformation *trn) {
    trn->oarea.left = 0;
    trn->oarea.top = 0;
    trn->oarea.width = -1;
    trn->oarea.height = -1;
    trn->iarea.left = 0;
    trn->iarea.top = 0;
    trn->iarea.width = -1;
    trn->iarea.height = -1;
    trn->a = 1.0;
    trn->b = 0.0;
    trn->c = 0.0;
    trn->d = 1.0;
    trn->dx = 0.0;
    trn->dy = 0.0;

    (void)im__transform_calc_inverse(trn);
}

int im__transform_isidentity(const Transformation *trn) {
    if (trn->a == 1.0 && trn->b == 0.0 && trn->c == 0.0 && trn->d == 1.0 && trn->dx == 0.0 && trn->dy == 0.0) {
        return (1);
    } else {
        return (0);
    }
}

int im__transform_add(const Transformation *in1, const Transformation *in2, Transformation *out) {
    out->a = in1->a * in2->a + in1->c * in2->b;
    out->b = in1->b * in2->a + in1->d * in2->b;
    out->c = in1->a * in2->c + in1->c * in2->d;
    out->d = in1->b * in2->c + in1->d * in2->d;

    out->dx = in1->dx * in2->a + in1->dy * in2->b + in2->dx;
    out->dy = in1->dx * in2->c + in1->dy * in2->d + in2->dy;

    if (im__transform_calc_inverse(out)) {
        return (-1);
    }

    return (0);
}

void im__transform_print(const Transformation *trn) {
    printf("im__transform_print:\n");
    printf(" iarea: left=%d, top=%d, width=%d, height=%d\n", trn->iarea.left, trn->iarea.top, trn->iarea.width, trn->iarea.height);
    printf(" oarea: left=%d, top=%d, width=%d, height=%d\n", trn->oarea.left, trn->oarea.top, trn->oarea.width, trn->oarea.height);
    printf(" mat: a=%g, b=%g, c=%g, d=%g\n", trn->a, trn->b, trn->c, trn->d);
    printf(" off: dx=%g, dy=%g\n", trn->dx, trn->dy);
}

void im__transform_forward_point(const Transformation *trn, const double x, const double y, double *ox, double *oy) {
    *ox = trn->a * x + trn->b * y + trn->dx;
    *oy = trn->c * x + trn->d * y + trn->dy;
}

void im__transform_invert_point(const Transformation *trn, const double x, const double y, double *ox, double *oy) {
    double mx = x - trn->dx;
    double my = y - trn->dy;

    *ox = trn->ia * mx + trn->ib * my;
    *oy = trn->ic * mx + trn->id * my;
}

typedef void (*transform_fn)(const Transformation *, const double, const double, double *, double *);

static void transform_rect(const Transformation *trn, transform_fn transform, const Rect *in, Rect *out) {
    double x1, y1;
    double x2, y2;
    double x3, y3;
    double x4, y4;
    double left, right, top, bottom;

    transform(trn, in->left, in->top, &x1, &y1);
    transform(trn, in->left, IM_RECT_BOTTOM(in), &x3, &y3);
    transform(trn, IM_RECT_RIGHT(in), in->top, &x2, &y2);
    transform(trn, IM_RECT_RIGHT(in), IM_RECT_BOTTOM(in), &x4, &y4);

    left = IM_MIN(x1, IM_MIN(x2, IM_MIN(x3, x4)));
    right = IM_MAX(x1, IM_MAX(x2, IM_MAX(x3, x4)));
    top = IM_MIN(y1, IM_MIN(y2, IM_MIN(y3, y4)));
    bottom = IM_MAX(y1, IM_MAX(y2, IM_MAX(y3, y4)));

    out->left = IM_RINT(left);
    out->top = IM_RINT(top);
    out->width = IM_RINT(right - left);
    out->height = IM_RINT(bottom - top);
}

void im__transform_forward_rect(const Transformation *trn, const Rect *in, Rect *out) {
    transform_rect(trn, im__transform_forward_point, in, out);
}

void im__transform_invert_rect(const Transformation *trn, const Rect *in, Rect *out) {
    transform_rect(trn, im__transform_invert_point, in, out);
}

void im__transform_set_area(Transformation *trn) {
    im__transform_forward_rect(trn, &trn->iarea, &trn->oarea);
}
