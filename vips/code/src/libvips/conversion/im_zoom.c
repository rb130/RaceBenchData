
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define ROUND_DOWN(N, P) ((N) - ((N) % P))

#define ROUND_UP(N, P) (ROUND_DOWN((N) + (P)-1, (P)))

typedef struct {
    int xfac;
    int yfac;
} ZoomInfo;

static void paint_whole(REGION * or, REGION *ir, ZoomInfo *zm, const int left, const int right, const int top, const int bottom) {
    const int ps = IM_IMAGE_SIZEOF_PEL(ir->im);
    const int ls = IM_REGION_LSKIP(or);
    const int rs = ps * (right - left);

    const int ileft = left / zm->xfac;
    const int iright = right / zm->xfac;
    const int itop = top / zm->yfac;
    const int ibottom = bottom / zm->yfac;

    int x, y, z, i;

    g_assert(right > left && bottom > top && right % zm->xfac == 0 && left % zm->xfac == 0 && top % zm->yfac == 0 && bottom % zm->yfac == 0);

    for (y = itop; y < ibottom; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir, ileft, y);
        PEL *q = (PEL *)IM_REGION_ADDR(or, left, y * zm->yfac);
        PEL *r;

        r = q;
        for (x = ileft; x < iright; x++) {

            for (z = 0; z < zm->xfac; z++) {
                for (i = 0; i < ps; i++) {
                    r[i] = p[i];
                }

                r += ps;
            }

            p += ps;
        }

        r = q + ls;
        for (z = 1; z < zm->yfac; z++) {
            memcpy(r, q, rs);
            r += ls;
        }
    }
}

static void paint_part(REGION * or, REGION *ir, const ZoomInfo *zm, const int left, const int right, const int top, const int bottom) {
    const int ps = IM_IMAGE_SIZEOF_PEL(ir->im);
    const int ls = IM_REGION_LSKIP(or);
    const int rs = ps * (right - left);

    const int ix = left / zm->xfac;
    const int iy = top / zm->yfac;

    const int ptbound = (iy + 1) * zm->yfac - top;
    const int ptbot = bottom - top;

    int yt = IM_MIN(ptbound, ptbot);

    int x, y, z, i;

    g_assert(right - left >= 0 && bottom - top >= 0);

    for (y = top; y < bottom;) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir, ix, y / zm->yfac);
        PEL *q = (PEL *)IM_REGION_ADDR(or, left, y);
        PEL *r;

        int xt = (ix + 1) * zm->xfac - left;

        r = q;
        for (x = left; x < right; x++) {

            for (i = 0; i < ps; i++) {
                r[i] = p[i];
            }
            r += ps;

            --xt;
            if (xt == 0) {
                xt = zm->xfac;
                p += ps;
            }
        }

        r = q + ls;
        for (z = 1; z < yt; z++) {
            memcpy(r, q, rs);
            r += ls;
        }

        y += yt;

        yt = zm->yfac;
    }
}

static int zoom_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    ZoomInfo *zm = (ZoomInfo *)b;

    const Rect *r = & or->valid;
    const int ri = IM_RECT_RIGHT(r);
    const int bo = IM_RECT_BOTTOM(r);

    Rect s;
    int left, right, top, bottom;
    int width, height;

    left = ROUND_DOWN(r->left, zm->xfac);
    right = ROUND_UP(ri, zm->xfac);
    top = ROUND_DOWN(r->top, zm->yfac);
    bottom = ROUND_UP(bo, zm->yfac);
    width = right - left;
    height = bottom - top;
    s.left = left / zm->xfac;
    s.top = top / zm->yfac;
    s.width = width / zm->xfac;
    s.height = height / zm->yfac;
    if (im_prepare(ir, &s)) {
        return (-1);
    }

    left = ROUND_UP(r->left, zm->xfac);
    right = ROUND_DOWN(ri, zm->xfac);
    top = ROUND_UP(r->top, zm->yfac);
    bottom = ROUND_DOWN(bo, zm->yfac);
    width = right - left;
    height = bottom - top;

    if (width > 0 && height > 0) {
        paint_whole(or, ir, zm, left, right, top, bottom);
    }

    if (top - r->top > 0) {

        paint_part(or, ir, zm, r->left, ri, r->top, IM_MIN(top, bo));
    }
    if (left - r->left > 0 && height > 0) {

        paint_part(or, ir, zm, r->left, IM_MIN(left, ri), top, bottom);
    }
    if (ri - right > 0 && height > 0) {

        paint_part(or, ir, zm, IM_MAX(right, r->left), ri, top, bottom);
    }
    if (bo - bottom > 0 && height >= 0) {

        paint_part(or, ir, zm, r->left, ri, IM_MAX(bottom, r->top), bo);
    }

    return (0);
}

int im_zoom(IMAGE *in, IMAGE *out, int xfac, int yfac) {
    ZoomInfo *zm;

    if (xfac <= 0 || yfac <= 0) {
        im_error("im_zoom", "%s", _("zoom factors should be >= 0"));
        return (-1);
    }
    if ((double)in->Xsize * xfac > (double)INT_MAX / 2 || (double)in->Ysize * yfac > (double)INT_MAX / 2) {

        im_error("im_zoom", "%s", _("zoom factors too large"));
        return (-1);
    }
    if (xfac == 1 && yfac == 1) {
        return (im_copy(in, out));
    }
    if (im_piocheck(in, out) || im_check_coding_known("im_zoom", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = in->Xsize * xfac;
    out->Ysize = in->Ysize * yfac;

    if (!(zm = IM_NEW(out, ZoomInfo))) {
        return (-1);
    }
    zm->xfac = xfac;
    zm->yfac = yfac;

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, zoom_gen, im_stop_one, in, zm)) {
        return (-1);
    }

    return (0);
}
