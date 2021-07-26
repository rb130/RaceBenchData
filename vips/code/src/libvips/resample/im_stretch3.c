
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
    IMAGE *in;
    double dx, dy;

    int xoff, yoff;
    int mask[34][4];
} StretchInfo;

typedef struct seq_info {
    StretchInfo *sin;
    REGION *ir;
    unsigned short *buf;
    int lsk;
} SeqInfo;

static int stretch_stop(void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *stretch_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    StretchInfo *sin = (StretchInfo *)b;
    SeqInfo *seq;

    if (!(seq = IM_NEW(out, SeqInfo))) {
        return (NULL);
    }

    seq->sin = sin;
    seq->ir = im_region_create(in);
    seq->lsk = IM_IMAGE_N_ELEMENTS(out);
    seq->buf = IM_ARRAY(out, 4 * seq->lsk, unsigned short);

    if (!seq->buf || !seq->ir) {
        stretch_stop(seq, NULL, NULL);
        return (NULL);
    }

    return ((void *)seq);
}

static void make_xline(StretchInfo *sin, unsigned short *p, unsigned short *q, int w, int m) {
    int bands = sin->in->Bands;
    int tot;
    int x, b;

    int o1 = 1 * bands;
    int o2 = 2 * bands;
    int o3 = 3 * bands;

    for (x = 0; x < w; x++) {
        int *mask = &sin->mask[m][0];
        unsigned short *p1 = p;

        for (b = 0; b < bands; b++) {
            tot = p1[0] * mask[0] + p1[o1] * mask[1] + p1[o2] * mask[2] + p1[o3] * mask[3];
            tot = IM_MAX(0, tot);
            p1++;
            *q++ = (tot + 16384) >> 15;
        }

        m++;
        if (m == 34) {

            m = 0;
        } else {

            p += bands;
        }
    }
}

static void make_yline(StretchInfo *sin, int lsk, int boff, unsigned short *p, unsigned short *q, int w, int m) {
    int bands = sin->in->Bands;
    int we = w * bands;
    int *mask = &sin->mask[m][0];
    int tot;
    int x;

    int o0 = lsk * boff;
    int o1 = lsk * ((boff + 1) % 4);
    int o2 = lsk * ((boff + 2) % 4);
    int o3 = lsk * ((boff + 3) % 4);

    for (x = 0; x < we; x++) {
        tot = p[o0] * mask[0] + p[o1] * mask[1] + p[o2] * mask[2] + p[o3] * mask[3];
        tot = IM_MAX(0, tot);
        p++;
        *q++ = (tot + 16384) >> 15;
    }
}

static int stretch_gen(REGION * or, void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;
    StretchInfo *sin = (StretchInfo *)b;
    REGION *ir = seq->ir;
    Rect *r = & or->valid;
    Rect r1;
    int x, y;

    int xstart = (r->left + sin->xoff) % 34;

    r1.left = r->left - (r->left + sin->xoff) / 34;
    r1.top = r->top;
    x = IM_RECT_RIGHT(r);
    x = x - (x + sin->xoff) / 34 + 3;
    r1.width = x - r1.left;
    r1.height = r->height + 3;
    if (im_prepare(ir, &r1)) {
        return (-1);
    }

    for (y = 0; y < 3; y++) {
        unsigned short *p = (unsigned short *)IM_REGION_ADDR(ir, r1.left, y + r1.top);
        unsigned short *q = seq->buf + seq->lsk * y;

        make_xline(sin, p, q, r->width, xstart);
    }

    for (y = 0; y < r->height; y++) {

        unsigned short *p = (unsigned short *)IM_REGION_ADDR(ir, r1.left, y + r1.top + 3);

        int boff = (y + 3) % 4;
        unsigned short *q = seq->buf + boff * seq->lsk;

        unsigned short *q1 = (unsigned short *)IM_REGION_ADDR(or, r->left, y + r->top);

        make_xline(sin, p, q, r->width, xstart);

        make_yline(sin, seq->lsk, boff, seq->buf, q1, r->width, sin->yoff);
    }

    return (0);
}

int im_stretch3(IMAGE *in, IMAGE *out, double dx, double dy) {
    StretchInfo *sin;
    int i;

    if (in->Coding != IM_CODING_NONE || in->BandFmt != IM_BANDFMT_USHORT) {
        im_error("im_stretch3", "%s", _("not uncoded unsigned short"));
        return (-1);
    }
    if (dx < 0 || dx >= 1.0 || dy < 0 || dy >= 1.0) {
        im_error("im_stretch3", "%s", _("displacements out of range [0,1)"));
        return (-1);
    }
    if (im_piocheck(in, out)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize = 34 * (in->Xsize / 33) + in->Xsize % 33 - 3;
    out->Ysize = in->Ysize - 3;

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (!(sin = IM_NEW(out, StretchInfo))) {
        return (-1);
    }

    sin->in = in;
    sin->dx = dx;
    sin->dy = dy;

    for (i = 0; i < 34; i++) {
        double d = (34.0 - i) / 34.0;

        double y0 = 2.0 * d * d - d - d * d * d;
        double y1 = 1.0 - 2.0 * d * d + d * d * d;
        double y2 = d + d * d - d * d * d;
        double y3 = -d * d + d * d * d;

        sin->mask[i][0] = IM_RINT(y0 * 32768);
        sin->mask[i][1] = IM_RINT(y1 * 32768);
        sin->mask[i][2] = IM_RINT(y2 * 32768);
        sin->mask[i][3] = IM_RINT(y3 * 32768);
    }

    sin->xoff = (dx * 33.0) + 0.5;
    sin->yoff = (dy * 33.0) + 0.5;

    if (im_generate(out, stretch_start, stretch_gen, stretch_stop, in, sin)) {
        return (-1);
    }

    return (0);
}
