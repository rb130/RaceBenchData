
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/transform.h>
#include <vips/internal.h>

#include "merge.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int find_top(REGION *ir, int *pos, int x, int y, int h) {
    PEL *pr = (PEL *)IM_REGION_ADDR(ir, x, y);
    IMAGE *im = ir->im;
    int ls = IM_REGION_LSKIP(ir) / IM_IMAGE_SIZEOF_ELEMENT(im);
    int b = im->Bands;
    int i, j;

    if (vips_bandfmt_iscomplex(im->BandFmt)) {
        b *= 2;
    }

#define tsearch(TYPE)               \
    {                               \
        TYPE *p = (TYPE *)pr;       \
                                    \
        for (i = 0; i < h; i++) {   \
            for (j = 0; j < b; j++) \
                if (p[j])           \
                    break;          \
            if (j < b)              \
                break;              \
                                    \
            p += ls;                \
        }                           \
    }

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        tsearch(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        tsearch(signed char);
        break;
    case IM_BANDFMT_USHORT:
        tsearch(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        tsearch(signed short);
        break;
    case IM_BANDFMT_UINT:
        tsearch(unsigned int);
        break;
    case IM_BANDFMT_INT:
        tsearch(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        tsearch(float);
        break;
    case IM_BANDFMT_DOUBLE:
        tsearch(double);
        break;
    case IM_BANDFMT_COMPLEX:
        tsearch(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        tsearch(double);
        break;

    default:
        im_error("im_tbmerge", "%s", _("internal error"));
        return (-1);
    }

    *pos = y + i;

    return (0);
}

static int find_bot(REGION *ir, int *pos, int x, int y, int h) {
    PEL *pr = (PEL *)IM_REGION_ADDR(ir, x, y);
    IMAGE *im = ir->im;
    int ls = IM_REGION_LSKIP(ir) / IM_IMAGE_SIZEOF_ELEMENT(ir->im);
    int b = im->Bands;
    int i, j;

    if (vips_bandfmt_iscomplex(im->BandFmt)) {
        b *= 2;
    }

#define rsearch(TYPE)                        \
    {                                        \
        TYPE *p = (TYPE *)pr + (h - 1) * ls; \
                                             \
        for (i = h - 1; i >= 0; i--) {       \
            for (j = 0; j < b; j++)          \
                if (p[j])                    \
                    break;                   \
            if (j < b)                       \
                break;                       \
                                             \
            p -= ls;                         \
        }                                    \
    }

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        rsearch(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        rsearch(signed char);
        break;
    case IM_BANDFMT_USHORT:
        rsearch(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        rsearch(signed short);
        break;
    case IM_BANDFMT_UINT:
        rsearch(unsigned int);
        break;
    case IM_BANDFMT_INT:
        rsearch(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        rsearch(float);
        break;
    case IM_BANDFMT_DOUBLE:
        rsearch(double);
        break;
    case IM_BANDFMT_COMPLEX:
        rsearch(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        rsearch(double);
        break;

    default:
        im_error("im_tbmerge", "%s", _("internal error"));
        return (-1);
    }

    *pos = y + i;

    return (0);
}

static int make_firstlast(MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
    REGION *rir = inf->rir;
    REGION *sir = inf->sir;
    Rect rr, sr;
    int x;
    int missing;

    g_mutex_lock(ovlap->fl_lock);

    missing = 0;
    for (x = oreg->left; x < IM_RECT_RIGHT(oreg); x++) {
        const int j = x - ovlap->overlap.left;
        const int first = ovlap->first[j];

        if (first < 0) {
            missing = 1;
            break;
        }
    }
    if (!missing) {

        g_mutex_unlock(ovlap->fl_lock);
        return (0);
    }

    rr.left = oreg->left;
    rr.top = ovlap->overlap.top;
    rr.width = oreg->width;
    rr.height = ovlap->overlap.height;
    rr.left -= ovlap->rarea.left;
    rr.top -= ovlap->rarea.top;

    sr.left = oreg->left;
    sr.top = ovlap->overlap.top;
    sr.width = oreg->width;
    sr.height = ovlap->overlap.height;
    sr.left -= ovlap->sarea.left;
    sr.top -= ovlap->sarea.top;

    if (im_prepare(rir, &rr) || im_prepare(sir, &sr)) {
        g_mutex_unlock(ovlap->fl_lock);
        return (-1);
    }

    for (x = 0; x < oreg->width; x++) {
        const int j = (x + oreg->left) - ovlap->overlap.left;
        int *first = &ovlap->first[j];
        int *last = &ovlap->last[j];

        if (*first < 0) {

            if (find_top(sir, first, x + sr.left, sr.top, sr.height) || find_bot(rir, last, x + rr.left, rr.top, rr.height)) {
                g_mutex_unlock(ovlap->fl_lock);
                return (-1);
            }

            *first += ovlap->sarea.top;
            *last += ovlap->rarea.top;

            if (ovlap->mwidth >= 0 && *last - *first > ovlap->mwidth) {
                int shrinkby = (*last - *first) - ovlap->mwidth;

                *first += shrinkby / 2;
                *last -= shrinkby / 2;
            }
        }
    }

    g_mutex_unlock(ovlap->fl_lock);

    return (0);
}

#define TEST_ZERO(TYPE, T, RESULT)  \
    {                               \
        TYPE *tt = (T);             \
        int ii;                     \
                                    \
        for (ii = 0; ii < cb; ii++) \
            if (tt[i])              \
                break;              \
        if (ii == cb)               \
            (RESULT) = 1;           \
    }

#define iblend(TYPE, B, IN1, IN2, OUT)                                               \
    {                                                                                \
        TYPE *tr = (TYPE *)(IN1);                                                    \
        TYPE *ts = (TYPE *)(IN2);                                                    \
        TYPE *tq = (TYPE *)(OUT);                                                    \
        const int cb = (B);                                                          \
        int ref_zero;                                                                \
        int sec_zero;                                                                \
        int x, b;                                                                    \
        int i;                                                                       \
                                                                                     \
        for (i = 0, x = 0; x < oreg->width; x++) {                                   \
            ref_zero = 0;                                                            \
            sec_zero = 0;                                                            \
            TEST_ZERO(TYPE, tr, ref_zero);                                           \
            TEST_ZERO(TYPE, ts, sec_zero);                                           \
                                                                                     \
            if (y < first[x]) {                                                      \
                if (!ref_zero)                                                       \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = tr[i];                                               \
                else                                                                 \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = ts[i];                                               \
            }                                                                        \
                                                                                     \
            else if (y >= last[x]) {                                                 \
                if (!sec_zero)                                                       \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = ts[i];                                               \
                else                                                                 \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = tr[i];                                               \
            }                                                                        \
                                                                                     \
            else {                                                                   \
                if (!ref_zero && !sec_zero) {                                        \
                    const int bheight = last[x] - first[x];                          \
                    const int inx = ((y - first[x]) << BLEND_SHIFT) / bheight;       \
                    int c1 = im__icoef1[inx];                                        \
                    int c2 = im__icoef2[inx];                                        \
                                                                                     \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = c1 * tr[i] / BLEND_SCALE + c2 * ts[i] / BLEND_SCALE; \
                } else if (!ref_zero)                                                \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = tr[i];                                               \
                else                                                                 \
                    for (b = 0; b < cb; b++, i++)                                    \
                        tq[i] = ts[i];                                               \
            }                                                                        \
        }                                                                            \
    }

#define fblend(TYPE, B, IN1, IN2, OUT)                                         \
    {                                                                          \
        TYPE *tr = (TYPE *)(IN1);                                              \
        TYPE *ts = (TYPE *)(IN2);                                              \
        TYPE *tq = (TYPE *)(OUT);                                              \
        int ref_zero;                                                          \
        int sec_zero;                                                          \
        const int cb = (B);                                                    \
        int x, b;                                                              \
        int i;                                                                 \
                                                                               \
        for (i = 0, x = 0; x < oreg->width; x++) {                             \
            ref_zero = 0;                                                      \
            sec_zero = 0;                                                      \
            TEST_ZERO(TYPE, tr, ref_zero);                                     \
            TEST_ZERO(TYPE, ts, sec_zero);                                     \
                                                                               \
            if (y < first[x])                                                  \
                if (!ref_zero)                                                 \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = tr[i];                                         \
                else                                                           \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = tr[i];                                         \
                                                                               \
            else if (y >= last[x])                                             \
                if (!sec_zero)                                                 \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = ts[i];                                         \
                else                                                           \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = tr[i];                                         \
                                                                               \
            else {                                                             \
                if (!ref_zero && !sec_zero) {                                  \
                    const int bheight = last[x] - first[x];                    \
                    const int inx = ((y - first[x]) << BLEND_SHIFT) / bheight; \
                    double c1 = im__coef1[inx];                                \
                    double c2 = im__coef2[inx];                                \
                                                                               \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = c1 * tr[i] + c2 * ts[i];                       \
                } else if (!ref_zero)                                          \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = tr[i];                                         \
                else                                                           \
                    for (b = 0; b < cb; b++, i++)                              \
                        tq[i] = ts[i];                                         \
            }                                                                  \
        }                                                                      \
    }

static int tb_blend(REGION * or, MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
    REGION *rir = inf->rir;
    REGION *sir = inf->sir;
    IMAGE *im = or->im;

    Rect prr, psr;
    int y, yr, ys;

    if (make_firstlast(inf, ovlap, oreg)) {
        return (-1);
    }

    prr = *oreg;
    prr.left -= ovlap->rarea.left;
    prr.top -= ovlap->rarea.top;

    psr = *oreg;
    psr.left -= ovlap->sarea.left;
    psr.top -= ovlap->sarea.top;

    if (im_prepare(rir, &prr)) {
        return (-1);
    }
    if (im_prepare(sir, &psr)) {
        return (-1);
    }

    for (y = oreg->top, yr = prr.top, ys = psr.top; y < IM_RECT_BOTTOM(oreg); y++, yr++, ys++) {
        PEL *pr = (PEL *)IM_REGION_ADDR(rir, prr.left, yr);
        PEL *ps = (PEL *)IM_REGION_ADDR(sir, psr.left, ys);
        PEL *q = (PEL *)IM_REGION_ADDR(or, oreg->left, y);

        const int j = oreg->left - ovlap->overlap.left;
        const int *first = ovlap->first + j;
        const int *last = ovlap->last + j;

        switch (im->BandFmt) {
        case IM_BANDFMT_UCHAR:
            iblend(unsigned char, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_CHAR:
            iblend(signed char, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_USHORT:
            iblend(unsigned short, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_SHORT:
            iblend(signed short, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_UINT:
            iblend(unsigned int, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_INT:
            iblend(signed int, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_FLOAT:
            fblend(float, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_DOUBLE:
            fblend(double, im->Bands, pr, ps, q);
            break;
        case IM_BANDFMT_COMPLEX:
            fblend(float, im->Bands * 2, pr, ps, q);
            break;
        case IM_BANDFMT_DPCOMPLEX:
            fblend(double, im->Bands * 2, pr, ps, q);
            break;

        default:
            im_error("im_tbmerge", "%s", _("internal error"));
            return (-1);
        }
    }

    return (0);
}

static int tb_blend_labpack(REGION * or, MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
    REGION *rir = inf->rir;
    REGION *sir = inf->sir;
    Rect prr, psr;
    int y, yr, ys;

    if (make_firstlast(inf, ovlap, oreg)) {
        return (-1);
    }

    prr = *oreg;
    prr.left -= ovlap->rarea.left;
    prr.top -= ovlap->rarea.top;

    psr = *oreg;
    psr.left -= ovlap->sarea.left;
    psr.top -= ovlap->sarea.top;

    if (im_prepare(rir, &prr)) {
        return (-1);
    }
    if (im_prepare(sir, &psr)) {
        return (-1);
    }

    for (y = oreg->top, yr = prr.top, ys = psr.top; y < IM_RECT_BOTTOM(oreg); y++, yr++, ys++) {
        PEL *pr = (PEL *)IM_REGION_ADDR(rir, prr.left, yr);
        PEL *ps = (PEL *)IM_REGION_ADDR(sir, psr.left, ys);
        PEL *q = (PEL *)IM_REGION_ADDR(or, oreg->left, y);

        const int j = oreg->left - ovlap->overlap.left;
        const int *first = ovlap->first + j;
        const int *last = ovlap->last + j;

        float *fq = inf->merge;
        float *r = inf->from1;
        float *s = inf->from2;

        imb_LabQ2Lab(pr, r, oreg->width);
        imb_LabQ2Lab(ps, s, oreg->width);

        fblend(float, 3, r, s, fq);

        imb_Lab2LabQ(inf->merge, q, oreg->width);
    }

    return (0);
}

static Overlapping *build_tbstate(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    Overlapping *ovlap;

    if (!(ovlap = im__build_mergestate(ref, sec, out, dx, dy, mwidth))) {
        return (NULL);
    }

    switch (ref->Coding) {
    case IM_CODING_LABQ:
        ovlap->blend = tb_blend_labpack;
        break;

    case IM_CODING_NONE:
        ovlap->blend = tb_blend;
        break;

    default:
        im_error("im_tbmerge", "%s", _("unknown coding type"));
        return (NULL);
    }

    ovlap->rpart = ovlap->rarea;
    ovlap->spart = ovlap->sarea;
    ovlap->rpart.height -= ovlap->overlap.height;
    ovlap->spart.top += ovlap->overlap.height;
    ovlap->spart.height -= ovlap->overlap.height;

    if (IM_RECT_BOTTOM(&ovlap->rarea) > IM_RECT_BOTTOM(&ovlap->sarea) || ovlap->rarea.top > ovlap->sarea.top) {
        im_error("im_tbmerge", "%s", _("too much overlap"));
        return (NULL);
    }

    ovlap->blsize = ovlap->overlap.width;

    return (ovlap);
}

int im__tbmerge(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    Overlapping *ovlap;

    if (ref->Bands != sec->Bands || ref->BandFmt != sec->BandFmt || ref->Coding != sec->Coding) {
        im_error("im_tbmerge", "%s", _("input images incompatible"));
        return (-1);
    }
    if (ref->Coding != IM_CODING_NONE && ref->Coding != IM_CODING_LABQ) {
        im_error("im_tbmerge", "%s", _("inputs not uncoded or IM_CODING_LABQ"));
        return (-1);
    }
    if (dy > 0 || dy < 1 - ref->Ysize) {

        if (im_insert(ref, sec, out, -dx, -dy)) {
            return (-1);
        }
        out->Xoffset = -dx;
        out->Yoffset = -dy;

        return (0);
    }
    if (im_piocheck(ref, out) || im_piocheck(sec, out)) {
        return (-1);
    }

    if (!(ovlap = build_tbstate(ref, sec, out, dx, dy, mwidth))) {
        return (-1);
    }

    if (im_cp_descv(out, ref, sec, NULL)) {
        return (-1);
    }
    out->Xsize = ovlap->oarea.width;
    out->Ysize = ovlap->oarea.height;
    out->Xoffset = ovlap->sarea.left;
    out->Yoffset = ovlap->sarea.top;

    if (im_demand_hint(out, IM_THINSTRIP, ref, sec, NULL)) {
        return (-1);
    }

    if (im_generate(out, im__start_merge, im__merge_gen, im__stop_merge, ovlap, NULL)) {
        return (-1);
    }

    return (0);
}

int im_tbmerge(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    if (im__tbmerge(ref, sec, out, dx, dy, mwidth)) {
        return (-1);
    }

    if (im_histlin(out, "#TBJOIN <%s> <%s> <%s> <%d> <%d> <%d>", ref->filename, sec->filename, out->filename, -dx, -dy, mwidth)) {
        return (-1);
    }

    return (0);
}
