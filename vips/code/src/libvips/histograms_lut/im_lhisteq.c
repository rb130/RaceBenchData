
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

typedef struct {
    int xwin, ywin;

    int npels;
} LhistInfo;

static int lhist_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    LhistInfo *inf = (LhistInfo *)b;
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

        PEL *p1;
        int hist[256];

        memset(hist, 0, 256 * sizeof(int));
        p1 = p;
        for (j = 0; j < inf->ywin; j++) {
            for (i = 0; i < inf->xwin; i++) {
                hist[p1[i]] += 1;
            }

            p1 += lsk;
        }

        for (x = 0; x < r->width; x++) {

            int target = p[centre];
            int sum;

            sum = 0;
            for (i = 0; i < target; i++) {
                sum += hist[i];
            }

            q[x] = sum * 256 / inf->npels;

            p1 = p;
            for (j = 0; j < inf->ywin; j++) {
                hist[p1[0]] -= 1;
                hist[p1[inf->xwin]] += 1;

                p1 += lsk;
            }

            p += 1;
        }
    }

    return (0);
}

int im_lhisteq_raw(IMAGE *in, IMAGE *out, int xwin, int ywin) {
    LhistInfo *inf;

    if (im_check_mono("im_lhisteq", in) || im_check_uncoded("im_lhisteq", in) || im_check_format("im_lhisteq", in, IM_BANDFMT_UCHAR) || im_piocheck(in, out)) {
        return (-1);
    }
    if (xwin > in->Xsize || ywin > in->Ysize) {
        im_error("im_lhisteq", "%s", _("window too large"));
        return (-1);
    }
    if (xwin <= 0 || ywin <= 0) {
        im_error("im_lhisteq", "%s", _("window too small"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize -= xwin - 1;
    out->Ysize -= ywin - 1;

    if (!(inf = IM_NEW(out, LhistInfo))) {
        return (-1);
    }
    inf->xwin = xwin;
    inf->ywin = ywin;
    inf->npels = xwin * ywin;

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, lhist_gen, im_stop_one, in, inf)) {
        return (-1);
    }

    out->Xoffset = -xwin / 2;
    out->Yoffset = -xwin / 2;

    return (0);
}

int im_lhisteq(IMAGE *in, IMAGE *out, int xwin, int ywin) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_lhisteq:1", "p")) || im_embed(in, t1, 1, xwin / 2, ywin / 2, in->Xsize + xwin - 1, in->Ysize + ywin - 1) || im_lhisteq_raw(t1, out, xwin, ywin)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
