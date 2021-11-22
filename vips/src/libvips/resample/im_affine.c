

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

#define FLOOR(V) ((V) >= 0 ? (int)(V) : (int)((V)-1))

typedef struct _Affine {
    IMAGE *in;
    IMAGE *out;
    VipsInterpolate *interpolate;
    Transformation trn;
} Affine;

static int affine_free(Affine *affine) {
    IM_FREEF(g_object_unref, affine->interpolate);

    return (0);
}

static int affinei_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    const IMAGE *in = (IMAGE *)a;
    const Affine *affine = (Affine *)b;
    const int window_offset = vips_interpolate_get_window_offset(affine->interpolate);
    const VipsInterpolateMethod interpolate = vips_interpolate_get_method(affine->interpolate);

    const Rect *r = & or->valid;
    const int le = r->left;
    const int ri = IM_RECT_RIGHT(r);
    const int to = r->top;
    const int bo = IM_RECT_BOTTOM(r);

    const Rect *iarea = &affine->trn.iarea;
    const Rect *oarea = &affine->trn.oarea;

    int ps = IM_IMAGE_SIZEOF_PEL(in);
    int x, y, z;

    Rect image, want, need, clipped;

#ifdef DEBUG
    printf("affine: generating left=%d, top=%d, width=%d, height=%d\n", r->left, r->top, r->width, r->height);
#endif

    want = *r;
    want.left += oarea->left;
    want.top += oarea->top;

    im__transform_invert_rect(&affine->trn, &want, &need);

    need.left += iarea->left;
    need.top += iarea->top;

    im_rect_marginadjust(&need, window_offset + 1);

    image.left = 0;
    image.top = 0;
    image.width = in->Xsize;
    image.height = in->Ysize;
    im_rect_intersectrect(&need, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        im_region_black(or);
        return (0);
    }

    if (im_prepare(ir, &clipped)) {
        return (-1);
    }

#ifdef DEBUG
    printf("affine: preparing left=%d, top=%d, width=%d, height=%d\n", clipped.left, clipped.top, clipped.width, clipped.height);
#endif

    for (y = to; y < bo; y++) {

        const int ile = iarea->left;
        const int ito = iarea->top;
        const int iri = iarea->left + iarea->width;
        const int ibo = iarea->top + iarea->height;

        const double ddx = affine->trn.ia;
        const double ddy = affine->trn.ic;

        const double ox = le + oarea->left - affine->trn.dx;
        const double oy = y + oarea->top - affine->trn.dy;

        double ix, iy;

        PEL *q;

        ix = affine->trn.ia * ox + affine->trn.ib * oy;
        iy = affine->trn.ic * ox + affine->trn.id * oy;

        ix += iarea->left;
        iy += iarea->top;

        q = (PEL *)IM_REGION_ADDR(or, le, y);

        for (x = le; x < ri; x++) {
            int fx, fy;

            fx = FLOOR(ix);
            fy = FLOOR(iy);

            if (fx < ile || fx >= iri || fy < ito || fy >= ibo) {
                for (z = 0; z < ps; z++) {
                    q[z] = 0;
                }
            } else {
                interpolate(affine->interpolate, q, ir, ix, iy);
            }

            ix += ddx;
            iy += ddy;
            q += ps;
        }
    }

    return (0);
}

static int affinei(IMAGE *in, IMAGE *out, VipsInterpolate *interpolate, Transformation *trn) {
    Affine *affine;
    double edge;

    if (im_piocheck(in, out)) {
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }

    if (!(affine = IM_NEW(out, Affine))) {
        return (-1);
    }
    affine->interpolate = NULL;
    if (im_add_close_callback(out, (im_callback_fn)affine_free, affine, NULL)) {
        return (-1);
    }
    affine->in = in;
    affine->out = out;
    affine->interpolate = interpolate;
    g_object_ref(interpolate);
    affine->trn = *trn;

    if (im__transform_calc_inverse(&affine->trn)) {
        return (-1);
    }

    out->Xsize = affine->trn.oarea.width;
    out->Ysize = affine->trn.oarea.height;

    if (affine->trn.b == 0.0 && affine->trn.c == 0.0) {
        if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
            return (-1);
        }
    } else {
        if (im_demand_hint(out, IM_SMALLTILE, in, NULL)) {
            return (-1);
        }
    }

    edge = INT_MAX / VIPS_TRANSFORM_SCALE;
    if (affine->trn.oarea.left < -edge || affine->trn.oarea.top < -edge || IM_RECT_RIGHT(&affine->trn.oarea) > edge || IM_RECT_BOTTOM(&affine->trn.oarea) > edge) {
        im_error("im_affinei", "%s", _("output coordinates out of range"));
        return (-1);
    }

    if (im_generate(out, im_start_one, affinei_gen, im_stop_one, in, affine)) {
        return (-1);
    }

    return (0);
}

static int im__affinei(IMAGE *in, IMAGE *out, VipsInterpolate *interpolate, Transformation *trn) {
    IMAGE *t3 = im_open_local(out, "im_affine:3", "p");
    const int window_size = vips_interpolate_get_window_size(interpolate);
    const int window_offset = vips_interpolate_get_window_offset(interpolate);
    Transformation trn2;

    if (!t3 || im_embed(in, t3, 1, window_offset, window_offset, in->Xsize + window_size, in->Ysize + window_size)) {
        return (-1);
    }

    trn2 = *trn;
    trn2.iarea.left += window_offset;
    trn2.iarea.top += window_offset;

#ifdef DEBUG_GEOMETRY
    printf("im__affinei: %s\n", in->filename);
    im__transform_print(&trn2);
#endif

    if (in->Coding == IM_CODING_LABQ) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "im_affine:2", "p") || im_LabQ2LabS(t3, t[0]) || affinei(t[0], t[1], interpolate, &trn2) || im_LabS2LabQ(t[1], out)) {
            return (-1);
        }
    } else if (in->Coding == IM_CODING_NONE) {
        if (affinei(t3, out, interpolate, &trn2)) {
            return (-1);
        }
    } else {
        im_error("im_affinei", "%s", _("unknown coding type"));
        return (-1);
    }

    out->Xoffset = trn->dx - trn->oarea.left;
    out->Yoffset = trn->dy - trn->oarea.top;

    return (0);
}

int im_affinei(IMAGE *in, IMAGE *out, VipsInterpolate *interpolate, double a, double b, double c, double d, double dx, double dy, int ox, int oy, int ow, int oh) {
    Transformation trn;

    trn.iarea.left = 0;
    trn.iarea.top = 0;
    trn.iarea.width = in->Xsize;
    trn.iarea.height = in->Ysize;

    trn.oarea.left = ox;
    trn.oarea.top = oy;
    trn.oarea.width = ow;
    trn.oarea.height = oh;

    trn.a = a;
    trn.b = b;
    trn.c = c;
    trn.d = d;
    trn.dx = dx;
    trn.dy = dy;

    return (im__affinei(in, out, interpolate, &trn));
}

int im_affinei_all(IMAGE *in, IMAGE *out, VipsInterpolate *interpolate, double a, double b, double c, double d, double dx, double dy) {
    Transformation trn;

    trn.iarea.left = 0;
    trn.iarea.top = 0;
    trn.iarea.width = in->Xsize;
    trn.iarea.height = in->Ysize;
    trn.a = a;
    trn.b = b;
    trn.c = c;
    trn.d = d;
    trn.dx = dx;
    trn.dy = dy;

    im__transform_set_area(&trn);

    return (im__affinei(in, out, interpolate, &trn));
}

int im__affine(IMAGE *in, IMAGE *out, Transformation *trn) {
    return (im__affinei(in, out, vips_interpolate_bilinear_static(), trn));
}
