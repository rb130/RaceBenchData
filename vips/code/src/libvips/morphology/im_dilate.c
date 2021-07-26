
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

typedef struct {
    REGION *ir;

    int *soff;
    int ss;
    int *coff;
    int cs;
    int last_bpl;
} SeqInfo;

static int dilate_stop(void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;

    IM_FREEF(im_region_free, seq->ir);

    return (0);
}

static void *dilate_start(IMAGE *out, void *a, void *b) {
    IMAGE *in = (IMAGE *)a;
    INTMASK *msk = (INTMASK *)b;
    int sz = msk->xsize * msk->ysize;
    SeqInfo *seq;

    if (!(seq = IM_NEW(out, SeqInfo))) {
        return (NULL);
    }

    seq->ir = NULL;
    seq->soff = NULL;
    seq->ss = 0;
    seq->coff = NULL;
    seq->cs = 0;
    seq->last_bpl = -1;

    seq->ir = im_region_create(in);
    seq->soff = IM_ARRAY(out, sz, int);
    seq->coff = IM_ARRAY(out, sz, int);
    if (!seq->ir || !seq->soff || !seq->coff) {
        dilate_stop(seq, in, NULL);
        return (NULL);
    }

    return (seq);
}

static int dilate_gen(REGION * or, void *vseq, void *a, void *b) {
    SeqInfo *seq = (SeqInfo *)vseq;
    INTMASK *msk = (INTMASK *)b;
    REGION *ir = seq->ir;

    int *soff = seq->soff;
    int *coff = seq->coff;

    Rect *r = & or->valid;
    Rect s;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int sz = IM_REGION_N_ELEMENTS(or);

    int *t;

    int x, y;
    int result, i;

    s = *r;
    s.width += msk->xsize - 1;
    s.height += msk->ysize - 1;
    if (im_prepare(ir, &s)) {
        return (-1);
    }

#ifdef DEBUG
    printf("erode_gen: preparing %dx%d pixels\n", s.width, s.height);
#endif

    if (seq->last_bpl != IM_REGION_LSKIP(ir)) {
        seq->last_bpl = IM_REGION_LSKIP(ir);

        seq->ss = 0;
        seq->cs = 0;
        for (t = msk->coeff, y = 0; y < msk->ysize; y++) {
            for (x = 0; x < msk->xsize; x++, t++) {
                switch (*t) {
                case 255:
                    soff[seq->ss++] = IM_REGION_ADDR(ir, x + le, y + to) - IM_REGION_ADDR(ir, le, to);
                    break;

                case 128:
                    break;

                case 0:
                    coff[seq->cs++] = IM_REGION_ADDR(ir, x + le, y + to) - IM_REGION_ADDR(ir, le, to);
                    break;

                default:
                    im_error("im_dilate",
                        _("bad mask element (%d "
                          "should be 0, 128 or 255)"),
                        *t);
                    return (-1);
                }
            }
        }
    }

    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir, le, y);
        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        for (x = 0; x < sz; x++, q++, p++) {

            result = 0;
            for (i = 0; i < seq->ss; i++) {
                if (p[soff[i]]) {

                    result = 255;
                    break;
                }
            }

            if (!result) {
                for (i = 0; i < seq->cs; i++) {
                    if (!p[coff[i]]) {

                        result = 255;
                        break;
                    }
                }
            }

            *q = result;
        }
    }

    return (0);
}

int im_dilate_raw(IMAGE *in, IMAGE *out, INTMASK *m) {
    INTMASK *msk;

    if (m->xsize < 1 || !(m->xsize & 0x1) || m->ysize < 1 || !(m->ysize & 0x1)) {
        im_error("im_dilate", "%s", _("mask size not odd"));
        return (-1);
    }

    if (im_piocheck(in, out)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE || in->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_dilate", "%s", _("uchar uncoded only"));
        return (-1);
    }
    if (im_cp_desc(out, in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Xsize -= m->xsize - 1;
    out->Ysize -= m->ysize - 1;
    if (out->Xsize <= 0 || out->Ysize <= 0) {
        im_error("im_dilate", "%s", _("image too small for mask"));
        return (-1);
    }

    if (!(msk = im_dup_imask(m, "conv_mask"))) {
        return (-1);
    }
    if (im_add_close_callback(out, (im_callback_fn)im_free_imask, msk, NULL)) {
        im_free_imask(msk);
        return (-1);
    }

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, dilate_start, dilate_gen, dilate_stop, in, msk)) {
        return (-1);
    }

    out->Xoffset = -m->xsize / 2;
    out->Yoffset = -m->ysize / 2;

    return (0);
}

int im_dilate(IMAGE *in, IMAGE *out, INTMASK *m) {
    IMAGE *t1 = im_open_local(out, "im_dilate:1", "p");

    if (!t1 || im_embed(in, t1, 1, m->xsize / 2, m->ysize / 2, in->Xsize + m->xsize - 1, in->Ysize + m->ysize - 1) || im_dilate_raw(t1, out, m)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
