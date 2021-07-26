
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *ref;
    double rmean;
    double c1;
} Spcor;

#define LOOP(IN)                                            \
    {                                                       \
        IN *a = (IN *)p;                                    \
        IN *b = (IN *)ref->data;                            \
        int in_lsk = lsk / sizeof(IN);                      \
        IN *a1;                                             \
        IN *b1;                                             \
                                                            \
        a1 = a;                                             \
        sum1 = 0;                                           \
        for (j = 0; j < ref->Ysize; j++) {                  \
            for (i = 0; i < ref->Xsize; i++)                \
                sum1 += a1[i];                              \
            a1 += in_lsk;                                   \
        }                                                   \
        imean = sum1 / (ref->Xsize * ref->Ysize);           \
                                                            \
        a1 = a;                                             \
        b1 = b;                                             \
        sum2 = 0.0;                                         \
        sum3 = 0.0;                                         \
        for (j = 0; j < ref->Ysize; j++) {                  \
            for (i = 0; i < ref->Xsize; i++) {              \
                                                            \
                IN rp = b1[i];                              \
                IN ip = a1[i];                              \
                                                            \
                double t = ip - imean;                      \
                sum2 += t * t;                              \
                                                            \
                sum3 += (rp - spcor->rmean) * (ip - imean); \
            }                                               \
            a1 += in_lsk;                                   \
            b1 += ref->Xsize;                               \
        }                                                   \
    }

static int spcor_gen(REGION * or, void *vseq, void *a, void *b) {
    REGION *ir = (REGION *)vseq;
    Spcor *spcor = (Spcor *)b;
    IMAGE *ref = spcor->ref;
    Rect irect;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int ri = IM_RECT_RIGHT(r);

    int x, y, i, j;
    int lsk;

    double imean;
    double sum1;
    double sum2, sum3;
    double c2, cc;

    irect.left = or->valid.left;
    irect.top = or->valid.top;
    irect.width = or->valid.width + ref->Xsize - 1;
    irect.height = or->valid.height + ref->Ysize - 1;

    if (im_prepare(ir, &irect)) {
        return (-1);
    }
    lsk = IM_REGION_LSKIP(ir);

    for (y = to; y < bo; y++) {
        float *q = (float *)IM_REGION_ADDR(or, le, y);

        for (x = le; x < ri; x++) {
            PEL *p = (PEL *)IM_REGION_ADDR(ir, x, y);

            switch (ref->BandFmt) {
            case IM_BANDFMT_UCHAR:
                LOOP(unsigned char);
                break;
            case IM_BANDFMT_CHAR:
                LOOP(signed char);
                break;
            case IM_BANDFMT_USHORT:
                LOOP(unsigned short);
                break;
            case IM_BANDFMT_SHORT:
                LOOP(signed short);
                break;
            default:
                g_assert(0);
            }

            c2 = sqrt(sum2);
            cc = sum3 / (spcor->c1 * c2);

            *q++ = cc;
        }
    }

    return (0);
}

static Spcor *spcor_new(IMAGE *out, IMAGE *ref) {
    Spcor *spcor;
    size_t sz = ref->Xsize * ref->Ysize;
    PEL *p = (PEL *)ref->data;
    double s;
    size_t i;

    if (!(spcor = IM_NEW(out, Spcor))) {
        return (NULL);
    }

    spcor->ref = ref;
    if (im_avg(spcor->ref, &spcor->rmean)) {
        return (NULL);
    }

    s = 0.0;
    for (i = 0; i < sz; i++) {
        double t = (int)p[i] - spcor->rmean;
        s += t * t;
    }
    spcor->c1 = sqrt(s);

    return (spcor);
}

int im_spcor_raw(IMAGE *in, IMAGE *ref, IMAGE *out) {
    Spcor *spcor;

    if (im_piocheck(in, out) || im_incheck(ref)) {
        return (-1);
    }

    if (in->Xsize < ref->Xsize || in->Ysize < ref->Ysize) {
        im_error("im_spcor_raw", "%s", _("ref not smaller than or equal to in"));
        return (-1);
    }

    if (im_check_uncoded("im_spcor", in) || im_check_mono("im_spcor", in) || im_check_8or16("im_spcor", in) || im_check_coding_same("im_spcor", in, ref) || im_check_bands_same("im_spcor", in, ref) || im_check_format_same("im_spcor", in, ref)) {
        return (-1);
    }

    if (im_cp_descv(out, in, ref, NULL)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Xsize = in->Xsize - ref->Xsize + 1;
    out->Ysize = in->Ysize - ref->Ysize + 1;

    if (!(spcor = spcor_new(out, ref))) {
        return (-1);
    }

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL)) {
        return (-1);
    }

    if (im_generate(out, im_start_one, spcor_gen, im_stop_one, in, spcor)) {
        return (-1);
    }

    out->Xoffset = -ref->Xsize / 2;
    out->Yoffset = -ref->Ysize / 2;

    return (0);
}

int im_spcor(IMAGE *in, IMAGE *ref, IMAGE *out) {
    IMAGE *t1 = im_open_local(out, "im_spcor intermediate", "p");

    if (!t1 || im_embed(in, t1, 1, ref->Xsize / 2, ref->Ysize / 2, in->Xsize + ref->Xsize - 1, in->Ysize + ref->Ysize - 1) || im_spcor_raw(t1, ref, out)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
