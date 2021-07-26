
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
#include <vips/thread.h>
#include <vips/transform.h>
#include <vips/internal.h>

#include "merge.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

double *im__coef1 = NULL;
double *im__coef2 = NULL;
int *im__icoef1 = NULL;
int *im__icoef2 = NULL;

int im__make_blend_luts() {
    int x;

    if (im__coef1 && im__coef2) {
        return (0);
    }

    im__coef1 = IM_ARRAY(NULL, BLEND_SIZE, double);
    im__coef2 = IM_ARRAY(NULL, BLEND_SIZE, double);
    im__icoef1 = IM_ARRAY(NULL, BLEND_SIZE, int);
    im__icoef2 = IM_ARRAY(NULL, BLEND_SIZE, int);
    if (!im__coef1 || !im__coef2 || !im__icoef1 || !im__icoef2) {
        return (-1);
    }

    for (x = 0; x < BLEND_SIZE; x++) {
        double a = IM_PI * x / (BLEND_SIZE - 1.0);

        im__coef1[x] = (cos(a) + 1.0) / 2.0;
        im__coef2[x] = 1.0 - im__coef1[x];
        im__icoef1[x] = im__coef1[x] * BLEND_SCALE;
        im__icoef2[x] = im__coef2[x] * BLEND_SCALE;
    }

    return (0);
}

static int find_first(REGION *ir, int *pos, int x, int y, int w) {
    PEL *pr = (PEL *)IM_REGION_ADDR(ir, x, y);
    IMAGE *im = ir->im;
    int ne = w * im->Bands;
    int i;

    if (vips_bandfmt_iscomplex(im->BandFmt)) {
        ne *= 2;
    }

#define lsearch(TYPE)            \
    {                            \
        TYPE *p = (TYPE *)pr;    \
                                 \
        for (i = 0; i < ne; i++) \
            if (p[i])            \
                break;           \
    }

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        lsearch(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        lsearch(signed char);
        break;
    case IM_BANDFMT_USHORT:
        lsearch(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        lsearch(signed short);
        break;
    case IM_BANDFMT_UINT:
        lsearch(unsigned int);
        break;
    case IM_BANDFMT_INT:
        lsearch(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        lsearch(float);
        break;
    case IM_BANDFMT_DOUBLE:
        lsearch(double);
        break;
    case IM_BANDFMT_COMPLEX:
        lsearch(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        lsearch(double);
        break;

    default:
        im_error("im_lrmerge", "%s", _("internal error"));
        return (-1);
    }

    *pos = x + i / im->Bands;

    return (0);
}

static int find_last(REGION *ir, int *pos, int x, int y, int w) {
    PEL *pr = (PEL *)IM_REGION_ADDR(ir, x, y);
    IMAGE *im = ir->im;
    int ne = w * im->Bands;
    int i;

    if (vips_bandfmt_iscomplex(im->BandFmt)) {
        ne *= 2;
    }

#define rsearch(TYPE)                 \
    {                                 \
        TYPE *p = (TYPE *)pr;         \
                                      \
        for (i = ne - 1; i >= 0; i--) \
            if (p[i])                 \
                break;                \
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
        im_error("im_lrmerge", "%s", _("internal error"));
        return (-1);
    }

    *pos = x + i / im->Bands;

    return (0);
}

static int make_firstlast(MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
    REGION *rir = inf->rir;
    REGION *sir = inf->sir;
    Rect rr, sr;
    int y, yr, ys;
    int missing;

    g_mutex_lock(ovlap->fl_lock);

    missing = 0;
    for (y = oreg->top; y < IM_RECT_BOTTOM(oreg); y++) {
        const int j = y - ovlap->overlap.top;
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

    rr.left = ovlap->overlap.left;
    rr.top = oreg->top;
    rr.width = ovlap->overlap.width;
    rr.height = oreg->height;
    rr.left -= ovlap->rarea.left;
    rr.top -= ovlap->rarea.top;

    sr.left = ovlap->overlap.left;
    sr.top = oreg->top;
    sr.width = ovlap->overlap.width;
    sr.height = oreg->height;
    sr.left -= ovlap->sarea.left;
    sr.top -= ovlap->sarea.top;

#ifdef DEBUG
    printf("im__lrmerge: making first/last for areas:\n");
    printf("ref: left = %d, top = %d, width = %d, height = %d\n", rr.left, rr.top, rr.width, rr.height);
    printf("sec: left = %d, top = %d, width = %d, height = %d\n", sr.left, sr.top, sr.width, sr.height);
#endif

    if (im_prepare(rir, &rr) || im_prepare(sir, &sr)) {
        g_mutex_unlock(ovlap->fl_lock);
        return (-1);
    }

    for (y = oreg->top, yr = rr.top, ys = sr.top; y < IM_RECT_BOTTOM(oreg); y++, yr++, ys++) {
        const int j = y - ovlap->overlap.top;
        int *first = &ovlap->first[j];
        int *last = &ovlap->last[j];

        if (*first < 0) {

            if (find_first(sir, first, sr.left, ys, sr.width) || find_last(rir, last, rr.left, yr, rr.width)) {
                g_mutex_unlock(ovlap->fl_lock);
                return (-1);
            }

            *first += ovlap->sarea.left;
            *last += ovlap->rarea.left;

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

#define iblend(TYPE, B, IN1, IN2, OUT)                                           \
    {                                                                            \
        TYPE *tr = (TYPE *)(IN1);                                                \
        TYPE *ts = (TYPE *)(IN2);                                                \
        TYPE *tq = (TYPE *)(OUT);                                                \
        const int cb = (B);                                                      \
        const int left = IM_CLIP(0, first - oreg->left, oreg->width);            \
        const int right = IM_CLIP(left, last - oreg->left, oreg->width);         \
        int ref_zero;                                                            \
        int sec_zero;                                                            \
        int x, b;                                                                \
        int i;                                                                   \
                                                                                 \
        for (i = 0, x = 0; x < left; x++) {                                      \
            ref_zero = 0;                                                        \
            TEST_ZERO(TYPE, tr, ref_zero);                                       \
            if (!ref_zero)                                                       \
                for (b = 0; b < cb; b++, i++)                                    \
                    tq[i] = tr[i];                                               \
            else                                                                 \
                for (b = 0; b < cb; b++, i++)                                    \
                    tq[i] = ts[i];                                               \
        }                                                                        \
                                                                                 \
        for (x = left; x < right; x++) {                                         \
            ref_zero = 0;                                                        \
            sec_zero = 0;                                                        \
            TEST_ZERO(TYPE, tr, ref_zero);                                       \
            TEST_ZERO(TYPE, ts, sec_zero);                                       \
                                                                                 \
            if (!ref_zero && !sec_zero) {                                        \
                int inx = ((x + oreg->left - first) << BLEND_SHIFT) / bwidth;    \
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
                                                                                 \
        for (x = right; x < oreg->width; x++) {                                  \
            sec_zero = 0;                                                        \
            TEST_ZERO(TYPE, ts, sec_zero);                                       \
            if (!sec_zero)                                                       \
                for (b = 0; b < cb; b++, i++)                                    \
                    tq[i] = ts[i];                                               \
            else                                                                 \
                for (b = 0; b < cb; b++, i++)                                    \
                    tq[i] = tr[i];                                               \
        }                                                                        \
    }

#define fblend(TYPE, B, IN1, IN2, OUT)                                        \
    {                                                                         \
        TYPE *tr = (TYPE *)(IN1);                                             \
        TYPE *ts = (TYPE *)(IN2);                                             \
        TYPE *tq = (TYPE *)(OUT);                                             \
        const int cb = (B);                                                   \
        const int left = IM_CLIP(0, first - oreg->left, oreg->width);         \
        const int right = IM_CLIP(left, last - oreg->left, oreg->width);      \
        int ref_zero;                                                         \
        int sec_zero;                                                         \
        int x, b;                                                             \
        int i;                                                                \
                                                                              \
        for (i = 0, x = 0; x < left; x++) {                                   \
            ref_zero = 0;                                                     \
            TEST_ZERO(TYPE, tr, ref_zero);                                    \
            if (!ref_zero)                                                    \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = tr[i];                                            \
            else                                                              \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = ts[i];                                            \
        }                                                                     \
                                                                              \
        for (x = left; x < right; x++) {                                      \
            ref_zero = 0;                                                     \
            sec_zero = 0;                                                     \
            TEST_ZERO(TYPE, tr, ref_zero);                                    \
            TEST_ZERO(TYPE, ts, sec_zero);                                    \
                                                                              \
            if (!ref_zero && !sec_zero) {                                     \
                int inx = ((x + oreg->left - first) << BLEND_SHIFT) / bwidth; \
                double c1 = im__coef1[inx];                                   \
                double c2 = im__coef2[inx];                                   \
                                                                              \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = c1 * tr[i] + c2 * ts[i];                          \
            } else if (!ref_zero)                                             \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = tr[i];                                            \
            else                                                              \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = ts[i];                                            \
        }                                                                     \
                                                                              \
        for (x = right; x < oreg->width; x++) {                               \
            sec_zero = 0;                                                     \
            TEST_ZERO(TYPE, ts, sec_zero);                                    \
            if (!sec_zero)                                                    \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = ts[i];                                            \
            else                                                              \
                for (b = 0; b < cb; b++, i++)                                 \
                    tq[i] = tr[i];                                            \
        }                                                                     \
    }

static int lr_blend(REGION * or, MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
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

        const int j = y - ovlap->overlap.top;
        const int first = ovlap->first[j];
        const int last = ovlap->last[j];
        const int bwidth = last - first;

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
            im_error("im_lrmerge", "%s", _("internal error"));
            return (-1);
        }
    }

    return (0);
}

static int lr_blend_labpack(REGION * or, MergeInfo *inf, Overlapping *ovlap, Rect *oreg) {
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

        const int j = y - ovlap->overlap.top;
        const int first = ovlap->first[j];
        const int last = ovlap->last[j];
        const int bwidth = last - first;

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

static void *lock_free(GMutex *lock) {
    g_mutex_free(lock);

    return (NULL);
}

Overlapping *im__build_mergestate(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    Overlapping *ovlap = IM_NEW(out, Overlapping);
    int x;

    if (!ovlap) {
        return (NULL);
    }
    if (mwidth < -1) {
        im_error("im_lr/tbmerge", "%s", _("mwidth must be -1 or >= 0"));
        return (NULL);
    }

    ovlap->ref = ref;
    ovlap->sec = sec;
    ovlap->out = out;
    ovlap->dx = dx;
    ovlap->dy = dy;
    ovlap->mwidth = mwidth;

    ovlap->rarea.left = 0;
    ovlap->rarea.top = 0;
    ovlap->rarea.width = ref->Xsize;
    ovlap->rarea.height = ref->Ysize;

    ovlap->sarea.left = -dx;
    ovlap->sarea.top = -dy;
    ovlap->sarea.width = sec->Xsize;
    ovlap->sarea.height = sec->Ysize;

    im_rect_intersectrect(&ovlap->rarea, &ovlap->sarea, &ovlap->overlap);
    if (im_rect_isempty(&ovlap->overlap)) {
        im_error("im_lr/tbmerge", "%s", _("no overlap"));
        return (NULL);
    }

    im_rect_unionrect(&ovlap->rarea, &ovlap->sarea, &ovlap->oarea);

    ovlap->rarea.left -= ovlap->oarea.left;
    ovlap->rarea.top -= ovlap->oarea.top;
    ovlap->sarea.left -= ovlap->oarea.left;
    ovlap->sarea.top -= ovlap->oarea.top;
    ovlap->overlap.left -= ovlap->oarea.left;
    ovlap->overlap.top -= ovlap->oarea.top;
    ovlap->oarea.left = 0;
    ovlap->oarea.top = 0;

    im__make_blend_luts();

    ovlap->flsize = IM_MAX(ovlap->overlap.width, ovlap->overlap.height);

    ovlap->first = IM_ARRAY(out, ovlap->flsize, int);
    ovlap->last = IM_ARRAY(out, ovlap->flsize, int);
    if (!ovlap->first || !ovlap->last) {
        return (NULL);
    }
    for (x = 0; x < ovlap->flsize; x++) {
        ovlap->first[x] = -1;
    }

    ovlap->fl_lock = g_mutex_new();
    if (im_add_close_callback(out, (im_callback_fn)lock_free, ovlap->fl_lock, NULL)) {
        g_mutex_free(ovlap->fl_lock);
        return (NULL);
    }

    return (ovlap);
}

static Overlapping *build_lrstate(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    Overlapping *ovlap;

    if (!(ovlap = im__build_mergestate(ref, sec, out, dx, dy, mwidth))) {
        return (NULL);
    }

    switch (ref->Coding) {
    case IM_CODING_LABQ:
        ovlap->blend = lr_blend_labpack;
        break;

    case IM_CODING_NONE:
        ovlap->blend = lr_blend;
        break;

    default:
        im_error("im_lrmerge", "%s", _("unknown coding type"));
        return (NULL);
    }

    ovlap->rpart = ovlap->rarea;
    ovlap->spart = ovlap->sarea;
    ovlap->rpart.width -= ovlap->overlap.width;
    ovlap->spart.left += ovlap->overlap.width;
    ovlap->spart.width -= ovlap->overlap.width;

    if (IM_RECT_RIGHT(&ovlap->rarea) > IM_RECT_RIGHT(&ovlap->sarea) || ovlap->rarea.left > ovlap->sarea.left) {
        im_error("im_lrmerge", "%s", _("too much overlap"));
        return (NULL);
    }

    ovlap->blsize = ovlap->overlap.width;

    return (ovlap);
}

int im__attach_input(REGION * or, REGION *ir, Rect *area) {
    Rect r = or->valid;

    r.left -= area->left;
    r.top -= area->top;

    if (im_prepare(ir, &r)) {
        return (-1);
    }

    if (im_region_region(or, ir, & or->valid, r.left, r.top)) {
        return (-1);
    }

    return (0);
}

int im__copy_input(REGION * or, REGION *ir, Rect *area, Rect *reg) {
    Rect r = *reg;

    r.left -= area->left;
    r.top -= area->top;

    if (im_prepare_to(ir, or, &r, reg->left, reg->top)) {
        return (-1);
    }

    return (0);
}

int im__merge_gen(REGION * or, void *seq, void *a, void *b) {
    MergeInfo *inf = (MergeInfo *)seq;
    Overlapping *ovlap = (Overlapping *)a;
    Rect *r = & or->valid;
    Rect rreg, sreg, oreg;

    im_rect_intersectrect(r, &ovlap->rpart, &rreg);
    im_rect_intersectrect(r, &ovlap->spart, &sreg);

    if (im_rect_equalsrect(r, &rreg)) {
        if (im__attach_input(or, inf->rir, &ovlap->rarea)) {
            return (-1);
        }
    } else if (im_rect_equalsrect(r, &sreg)) {
        if (im__attach_input(or, inf->sir, &ovlap->sarea)) {
            return (-1);
        }
    } else {

        im_rect_intersectrect(r, &ovlap->rarea, &rreg);
        im_rect_intersectrect(r, &ovlap->sarea, &sreg);
        im_rect_intersectrect(r, &ovlap->overlap, &oreg);

        im_region_black(or);
        if (!im_rect_isempty(&rreg)) {
            if (im__copy_input(or, inf->rir, &ovlap->rarea, &rreg)) {
                return (-1);
            }
        }
        if (!im_rect_isempty(&sreg)) {
            if (im__copy_input(or, inf->sir, &ovlap->sarea, &sreg)) {
                return (-1);
            }
        }

        inf->rir->valid.width = inf->sir->valid.width = 0;

        if (!im_rect_isempty(&oreg)) {
            if (ovlap->blend(or, inf, ovlap, &oreg)) {
                return (-1);
            }
        }
    }

    return (0);
}

int im__stop_merge(void *seq, void *a, void *b) {
    MergeInfo *inf = (MergeInfo *)seq;

    if (inf->rir) {
        im_region_free(inf->rir);
        inf->rir = NULL;
    }
    if (inf->sir) {
        im_region_free(inf->sir);
        inf->sir = NULL;
    }
    if (inf->from1) {
        im_free(inf->from1);
        inf->from1 = NULL;
    }
    if (inf->from2) {
        im_free(inf->from2);
        inf->from2 = NULL;
    }
    if (inf->merge) {
        im_free(inf->merge);
        inf->merge = NULL;
    }
    im_free(inf);

    return (0);
}

void *im__start_merge(IMAGE *out, void *a, void *b) {
    Overlapping *ovlap = (Overlapping *)a;
    MergeInfo *inf;

    if (!(inf = IM_NEW(NULL, MergeInfo))) {
        return (NULL);
    }

    inf->rir = NULL;
    inf->sir = NULL;
    inf->from1 = NULL;
    inf->from2 = NULL;
    inf->merge = NULL;

    if (out->Coding == IM_CODING_LABQ) {
        inf->from1 = IM_ARRAY(NULL, ovlap->blsize * 3, float);
        inf->from2 = IM_ARRAY(NULL, ovlap->blsize * 3, float);
        inf->merge = IM_ARRAY(NULL, ovlap->blsize * 3, float);
        if (!inf->from1 || !inf->from2 || !inf->merge) {
            im__stop_merge(inf, NULL, NULL);
            return (NULL);
        }
    }

    inf->rir = im_region_create(ovlap->ref);
    inf->sir = im_region_create(ovlap->sec);

    if (!inf->rir || !inf->sir) {
        im__stop_merge(inf, NULL, NULL);
        return (NULL);
    }

    return (inf);
}

int im__lrmerge(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    Overlapping *ovlap;

#ifdef DEBUG
    printf("im__lrmerge %s %s %s %d %d %d\n", ref->filename, sec->filename, out->filename, dx, dy, mwidth);
    printf("ref is %d x %d pixels\n", ref->Xsize, ref->Ysize);
    printf("sec is %d x %d pixels\n", sec->Xsize, sec->Ysize);
#endif

    if (ref->Bands != sec->Bands || ref->BandFmt != sec->BandFmt || ref->Coding != sec->Coding) {
        im_error("im_lrmerge", "%s", _("input images incompatible"));
        return (-1);
    }
    if (ref->Coding != IM_CODING_NONE && ref->Coding != IM_CODING_LABQ) {
        im_error("im_lrmerge", "%s", _("inputs not uncoded or IM_CODING_LABQ"));
        return (-1);
    }
    if (dx > 0 || dx < 1 - ref->Xsize) {
#ifdef DEBUG
        printf("im__lrmerge: no overlap, using insert\n");
#endif

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

    if (!(ovlap = build_lrstate(ref, sec, out, dx, dy, mwidth))) {
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

int im_lrmerge(IMAGE *ref, IMAGE *sec, IMAGE *out, int dx, int dy, int mwidth) {
    if (im__lrmerge(ref, sec, out, dx, dy, mwidth)) {
        return (-1);
    }

    if (im_histlin(out, "#LRJOIN <%s> <%s> <%s> <%d> <%d> <%d>", ref->filename, sec->filename, out->filename, -dx, -dy, mwidth)) {
        return (-1);
    }

    return (0);
}
