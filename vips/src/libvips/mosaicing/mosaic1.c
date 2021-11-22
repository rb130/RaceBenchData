

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/buf.h>
#include <vips/transform.h>

#include "mosaic.h"
#include "merge.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int apply_similarity(Transformation *trn, IMAGE *in, IMAGE *out, double a, double b, double dx, double dy) {
    trn->iarea.left = 0;
    trn->iarea.top = 0;
    trn->iarea.width = in->Xsize;
    trn->iarea.height = in->Ysize;
    trn->a = a;
    trn->b = -b;
    trn->c = b;
    trn->d = a;
    trn->dx = dx;
    trn->dy = dy;
    im__transform_set_area(trn);
    if (im__transform_calc_inverse(trn)) {
        return (-1);
    }

    if (im__affine(in, out, trn)) {
        return (-1);
    }

    return (0);
}

typedef int (*joinfn)(IMAGE *, IMAGE *, IMAGE *, double, double, double, double, int);

int im__lrmerge1(IMAGE *ref, IMAGE *sec, IMAGE *out, double a, double b, double dx, double dy, int mwidth) {
    Transformation trn;
    IMAGE *t1 = im_open_local(out, "im_lrmosaic1:1", "p");
    VipsBuf buf;
    char text[1024];

    if (!t1 || apply_similarity(&trn, sec, t1, a, b, dx, dy)) {
        return (-1);
    }

    if (im__lrmerge(ref, t1, out, -trn.oarea.left, -trn.oarea.top, mwidth)) {
        return (-1);
    }

    vips_buf_init_static(&buf, text, 1024);
    vips_buf_appendf(&buf, "#LRROTSCALE <%s> <%s> <%s> <", ref->filename, sec->filename, out->filename);
    vips_buf_appendg(&buf, a);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, b);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, dx);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, dy);
    vips_buf_appendf(&buf, "> <%d>", mwidth);
    if (im_histlin(out, "%s", vips_buf_all(&buf))) {
        return (-1);
    }

    return (0);
}

int im__tbmerge1(IMAGE *ref, IMAGE *sec, IMAGE *out, double a, double b, double dx, double dy, int mwidth) {
    Transformation trn;
    IMAGE *t1 = im_open_local(out, "im_lrmosaic1:2", "p");
    VipsBuf buf;
    char text[1024];

    if (!t1 || apply_similarity(&trn, sec, t1, a, b, dx, dy)) {
        return (-1);
    }

    if (im__tbmerge(ref, t1, out, -trn.oarea.left, -trn.oarea.top, mwidth)) {
        return (-1);
    }

    vips_buf_init_static(&buf, text, 1024);
    vips_buf_appendf(&buf, "#TBROTSCALE <%s> <%s> <%s> <", ref->filename, sec->filename, out->filename);
    vips_buf_appendg(&buf, a);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, b);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, dx);
    vips_buf_appendf(&buf, "> <");
    vips_buf_appendg(&buf, dy);
    vips_buf_appendf(&buf, "> <%d>", mwidth);
    if (im_histlin(out, "%s", vips_buf_all(&buf))) {
        return (-1);
    }

    return (0);
}

static int rotjoin(IMAGE *ref, IMAGE *sec, IMAGE *out, joinfn jfn, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int mwidth) {
    double a, b, dx, dy;

    if (im__coeff(xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, &a, &b, &dx, &dy)) {
        return (-1);
    }

    if (jfn(ref, sec, out, a, b, dx, dy, mwidth)) {
        return (-1);
    }

    return (0);
}

int im_lrmerge1(IMAGE *ref, IMAGE *sec, IMAGE *out, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int mwidth) {
    return (rotjoin(ref, sec, out, im__lrmerge1, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth));
}

int im_tbmerge1(IMAGE *ref, IMAGE *sec, IMAGE *out, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int mwidth) {
    return (rotjoin(ref, sec, out, im__tbmerge1, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth));
}

static int rotjoin_search(IMAGE *ref, IMAGE *sec, IMAGE *out, joinfn jfn, int bandno, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int halfcorrelation, int halfarea, int balancetype, int mwidth) {
    Transformation trn;
    double cor1, cor2;
    double a, b, dx, dy;
    double xs3, ys3;
    double xs4, ys4;
    int xs5, ys5;
    int xs6, ys6;
    double xs7, ys7;
    double xs8, ys8;

    IMAGE *t[3];

    if (im_open_local_array(out, t, 3, "rotjoin_search", "p")) {
        return (-1);
    }

    if (ref->Coding == IM_CODING_LABQ) {
        if (im_LabQ2LabS(ref, t[0])) {
            return (-1);
        }
    } else {
        t[0] = ref;
    }
    if (sec->Coding == IM_CODING_LABQ) {
        if (im_LabQ2LabS(sec, t[1])) {
            return (-1);
        }
    } else {
        t[1] = sec;
    }

    if (im__coeff(xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, &a, &b, &dx, &dy) || apply_similarity(&trn, t[1], t[2], a, b, dx, dy)) {
        return (-1);
    }

    im__transform_forward_point(&trn, xs1, ys1, &xs3, &ys3);
    im__transform_forward_point(&trn, xs2, ys2, &xs4, &ys4);

    if (im_correl(t[0], t[2], xr1, yr1, xs3 - trn.oarea.left, ys3 - trn.oarea.top, halfcorrelation, halfarea, &cor1, &xs5, &ys5)) {
        return (-1);
    }
    if (im_correl(t[0], t[2], xr2, yr2, xs4 - trn.oarea.left, ys4 - trn.oarea.top, halfcorrelation, halfarea, &cor2, &xs6, &ys6)) {
        return (-1);
    }

#ifdef DEBUG
    printf("rotjoin_search: nudged pair 1 from %d, %d to %d, %d\n", xs3 - trn.oarea.left, ys3 - trn.oarea.top, xs5, ys5);
    printf("rotjoin_search: nudged pair 2 from %d, %d to %d, %d\n", xs4 - trn.oarea.left, ys4 - trn.oarea.top, xs6, ys6);
#endif

    xs5 += trn.oarea.left;
    ys5 += trn.oarea.top;
    xs6 += trn.oarea.left;
    ys6 += trn.oarea.top;

    im__transform_invert_point(&trn, xs5, ys5, &xs7, &ys7);
    im__transform_invert_point(&trn, xs6, ys6, &xs8, &ys8);

    if (im__coeff(xr1, yr1, xs7, ys7, xr2, yr2, xs8, ys8, &a, &b, &dx, &dy)) {
        return (-1);
    }

    if (jfn(ref, sec, out, a, b, dx, dy, mwidth)) {
        return (-1);
    }

    return (0);
}

int im_lrmosaic1(IMAGE *ref, IMAGE *sec, IMAGE *out, int bandno, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int halfcorrelation, int halfarea, int balancetype, int mwidth) {
    return (rotjoin_search(ref, sec, out, im__lrmerge1, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, halfcorrelation, halfarea, balancetype, mwidth));
}

int im_tbmosaic1(IMAGE *ref, IMAGE *sec, IMAGE *out, int bandno, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int halfcorrelation, int halfarea, int balancetype, int mwidth) {
    return (rotjoin_search(ref, sec, out, im__tbmerge1, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, halfcorrelation, halfarea, balancetype, mwidth));
}

#ifdef OLD

static int old_lrmosaic1(IMAGE *ref, IMAGE *sec, IMAGE *out, int bandno, int xr1, int yr1, int xs1, int ys1, int xr2, int yr2, int xs2, int ys2, int halfcorrelation, int halfarea, int balancetype, int mwidth) {
    Transformation trn1, trn2;
    int dx0, dy0;
    double a, b, dx, dy;
    double a1, b1, dx1, dy1;
    double af, bf, dxf, dyf;
    int xpos, ypos;
    int xpos1, ypos1;

    IMAGE *t1 = im_open_local(out, "im_lrmosaic1:1", "p");
    IMAGE *t2 = im_open_local(out, "im_lrmosaic1:2", "p");
    IMAGE *dummy;

    if (!t1 || !t2)
        return (-1);

    if (im__coeff(xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, &a, &b, &dx, &dy) || apply_similarity(&trn1, sec, t1, a, b, dx, dy))
        return (-1);

    if (!(dummy = im_open("placeholder:1", "p")))
        return (-1);
    if (im__find_lroverlap(ref, t1, dummy, bandno, -trn1.area.left, -trn1.area.top, 0, 0, halfcorrelation, halfarea, &dx0, &dy0, &a1, &b1, &dx1, &dy1)) {
        im_close(dummy);
        return (-1);
    }
    im_close(dummy);

    af = a1 * a - b1 * b;
    bf = a1 * b + b1 * a;
    dxf = a1 * dx - b1 * dy + dx1;
    dyf = b1 * dx + a1 * dy + dy1;

    printf("transform was: a = %g, b = %g, dx = %g, dy = %g\n", a, b, dx, dy);
    printf("correction: a = %g, b = %g, dx = %g, dy = %g\n", a1, b1, dx1, dy1);
    printf("final: a = %g, b = %g, dx = %g, dy = %g\n", af, bf, dxf, dyf);

    if (apply_similarity(&trn2, sec, t2, af, bf, dxf, dyf))
        return (-1);

    printf("disp: trn1 left = %d, top = %d\n", trn1.area.left, trn1.area.top);
    printf("disp: trn2 left = %d, top = %d\n", trn2.area.left, trn2.area.top);

    if (im_lrmerge(ref, t2, out, -trn2.area.left, -trn2.area.top, mwidth))
        return (-1);

    return (0);
}
#endif
