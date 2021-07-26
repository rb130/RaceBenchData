
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    int xwin, ywin;
    double a, m0, b, s0;
} StdifInfo;

static int stdif_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    StdifInfo *inf = (StdifInfo *)b;
    int npel = inf->xwin * inf->ywin;
    Rect *r = & or->valid;

    Rect irect;
    int x, y, i, j;
    int lsk;
    int centre;

    irect.left = or->valid.left;
    irect.top = or->valid.top;
    irect.width = or->valid.width + inf->xwin;
    irect.height = or->valid.height + inf->ywin;
    if (im_prepare(ir, &irect)) {
        return (-1);
    }

    lsk = IM_REGION_LSKIP(ir);
    centre = lsk * (inf->ywin / 2) + inf->xwin / 2;

    for (y = 0; y < r->height; y++) {

        PEL *p = (PEL *)IM_REGION_ADDR(ir, r->left, r->top + y);
        PEL *q = (PEL *)IM_REGION_ADDR(or, r->left, r->top + y);

        double f1 = inf->a * inf->m0;
        double f2 = 1.0 - inf->a;
        double f3 = inf->b * inf->s0;

        PEL *p1;
        int sum;
        int sum2;

        sum = 0;
        sum2 = 0;
        p1 = p;
        for (j = 0; j < inf->ywin; j++) {
            for (i = 0; i < inf->xwin; i++) {
                int t = p1[i];

                sum += t;
                sum2 += t * t;
            }

            p1 += lsk;
        }

        for (x = 0; x < r->width; x++) {

            double mean = (double)sum / npel;
            double var = (double)sum2 / npel - (mean * mean);
            double sig = sqrt(var);

            double res = f1 + f2 * mean + ((double)p[centre] - mean) * (f3 / (inf->s0 + inf->b * sig));

            if (res < 0.0) {
                q[x] = 0;
            } else if (res >= 256.0) {
                q[x] = 255;
            } else {
                q[x] = res + 0.5;
            }

            p1 = p;
            for (j = 0; j < inf->ywin; j++) {
                int t1 = p1[0];
                int t2 = p1[inf->xwin];

                sum -= t1;
                sum2 -= t1 * t1;

                sum += t2;
                sum2 += t2 * t2;

                p1 += lsk;
            }

            p += 1;
        }
    }

    return (0);
}

int im_stdif_raw(IMAGE *in, IMAGE *out, double a, double m0, double b, double s0, int xwin, int ywin) {
    StdifInfo *inf;

    if (xwin > in->Xsize || ywin > in->Ysize) {
        im_error("im_stdif", "%s", _("window too large"));
        return (-1);
    }
    if (xwin <= 0 || ywin <= 0) {
        im_error("im_lhisteq", "%s", _("window too small"));
        return (-1);
    }
    if (m0 < 0 || m0 > 255 || a < 0 || a > 1.0 || b < 0 || b > 2 || s0 < 0 || s0 > 255) {
        im_error("im_stdif", "%s", _("parameters out of range"));
        return (-1);
    }
    if (im_check_format("im_stdif", in, IM_BANDFMT_UCHAR) || im_check_uncoded("im_stdif", in) || im_check_mono("im_stdif", in) || im_piocheck(in, out)) {
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize -= xwin;
    out->Ysize -= ywin;

    if (!(inf = IM_NEW(out, StdifInfo))) {
        return (-1);
    }
    inf->xwin = xwin;
    inf->ywin = ywin;
    inf->a = a;
    inf->m0 = m0;
    inf->b = b;
    inf->s0 = s0;

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, stdif_gen, im_stop_one, in, inf)) {
        return (-1);
    }

    return (0);
}

int im_stdif(IMAGE *in, IMAGE *out, double a, double m0, double b, double s0, int xwin, int ywin) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_stdif:1", "p")) || im_embed(in, t1, 1, xwin / 2, ywin / 2, in->Xsize + xwin - 1, in->Ysize + ywin - 1) || im_stdif_raw(t1, out, a, m0, b, s0, xwin, ywin)) {
        return (-1);
    }

    return (0);
}
