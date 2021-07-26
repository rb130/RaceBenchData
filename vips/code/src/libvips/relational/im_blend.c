
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define iblend1(TYPE)                                             \
    {                                                             \
        TYPE *a = (TYPE *)ap;                                     \
        TYPE *b = (TYPE *)bp;                                     \
        TYPE *q = (TYPE *)qp;                                     \
                                                                  \
        for (i = 0, x = 0; x < n; i++, x += bands) {              \
            const int v = c[i];                                   \
                                                                  \
            for (z = x; z < x + bands; z++)                       \
                q[z] = (v * a[z] + (255 - v) * b[z] + 128) / 255; \
        }                                                         \
    }

#define iblendn(TYPE)                                             \
    {                                                             \
        TYPE *a = (TYPE *)ap;                                     \
        TYPE *b = (TYPE *)bp;                                     \
        TYPE *q = (TYPE *)qp;                                     \
                                                                  \
        for (x = 0; x < n; x += bands) {                          \
            for (z = x; z < x + bands; z++) {                     \
                const int v = c[z];                               \
                                                                  \
                q[z] = (v * a[z] + (255 - v) * b[z] + 128) / 255; \
            }                                                     \
        }                                                         \
    }

#define fblend1(TYPE)                                \
    {                                                \
        TYPE *a = (TYPE *)ap;                        \
        TYPE *b = (TYPE *)bp;                        \
        TYPE *q = (TYPE *)qp;                        \
                                                     \
        for (i = 0, x = 0; x < n; i++, x += bands) { \
            const double v = c[i] / 255.0;           \
                                                     \
            for (z = x; z < x + bands; z++)          \
                q[z] = v * a[z] + (1.0 - v) * b[z];  \
        }                                            \
    }

#define fblendn(TYPE)                               \
    {                                               \
        TYPE *a = (TYPE *)ap;                       \
        TYPE *b = (TYPE *)bp;                       \
        TYPE *q = (TYPE *)qp;                       \
                                                    \
        for (x = 0; x < n; x += bands) {            \
            for (z = x; z < x + bands; z++) {       \
                const double v = c[z] / 255.0;      \
                                                    \
                q[z] = v * a[z] + (1.0 - v) * b[z]; \
            }                                       \
        }                                           \
    }

#define cblend1(TYPE)                                \
    {                                                \
        TYPE *a = (TYPE *)ap;                        \
        TYPE *b = (TYPE *)bp;                        \
        TYPE *q = (TYPE *)qp;                        \
                                                     \
        for (i = 0, x = 0; x < n; i++, x += bands) { \
            const double v = c[i] / 255.0;           \
                                                     \
            for (z = x; z < x + 2 * bands; z++)      \
                q[z] = v * a[z] + (1.0 - v) * b[z];  \
        }                                            \
    }

#define cblendn(TYPE)                                                       \
    {                                                                       \
        TYPE *a = (TYPE *)ap;                                               \
        TYPE *b = (TYPE *)bp;                                               \
        TYPE *q = (TYPE *)qp;                                               \
                                                                            \
        for (x = 0; x < n; x += bands) {                                    \
            for (z = x; z < x + bands; z++) {                               \
                const double v = c[z] / 255.0;                              \
                                                                            \
                q[2 * z] = v * a[2 * z] + (1.0 - v) * b[2 * z];             \
                q[2 * z + 1] = v * a[2 * z + 1] + (1.0 - v) * b[2 * z + 1]; \
            }                                                               \
        }                                                                   \
    }

static void blend1_buffer(PEL *qp, PEL *c, PEL *ap, PEL *bp, int width, IMAGE *im) {
    int i, x, z;
    const int bands = im->Bands;
    const int n = width * bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        iblend1(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        iblend1(signed char);
        break;
    case IM_BANDFMT_USHORT:
        iblend1(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        iblend1(signed short);
        break;
    case IM_BANDFMT_UINT:
        iblend1(unsigned int);
        break;
    case IM_BANDFMT_INT:
        iblend1(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        fblend1(float);
        break;
    case IM_BANDFMT_DOUBLE:
        fblend1(double);
        break;
    case IM_BANDFMT_COMPLEX:
        cblend1(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        cblend1(double);
        break;

    default:
        g_assert(0);
    }
}

static void blendn_buffer(PEL *qp, PEL *c, PEL *ap, PEL *bp, int width, IMAGE *im) {
    int x, z;
    const int bands = im->Bands;
    const int n = width * bands;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        iblendn(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        iblendn(signed char);
        break;
    case IM_BANDFMT_USHORT:
        iblendn(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        iblendn(signed short);
        break;
    case IM_BANDFMT_UINT:
        iblendn(unsigned int);
        break;
    case IM_BANDFMT_INT:
        iblendn(signed int);
        break;
    case IM_BANDFMT_FLOAT:
        fblendn(float);
        break;
    case IM_BANDFMT_DOUBLE:
        fblendn(double);
        break;
    case IM_BANDFMT_COMPLEX:
        cblendn(float);
        break;
    case IM_BANDFMT_DPCOMPLEX:
        cblendn(double);
        break;

    default:
        g_assert(0);
    }
}

static int blend_gen(REGION * or, void *seq, void *client1, void *client2) {
    REGION **ir = (REGION **)seq;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    IMAGE *c = ir[0]->im;
    IMAGE *a = ir[1]->im;

    int c_elements = r->width * c->Bands;
    int x, y;

    int all0, all255;

    if (im_prepare(ir[0], r)) {
        return (-1);
    }

    all0 = *((PEL *)IM_REGION_ADDR(ir[0], le, to)) == 0;
    all255 = *((PEL *)IM_REGION_ADDR(ir[0], le, to)) == 255;
    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir[0], le, y);

        for (x = 0; x < c_elements; x++) {
            all0 &= p[x] == 0;
            all255 &= p[x] == 255;
        }

        if (!all0 && !all255) {
            break;
        }
    }

    if (all255) {

        if (im_prepare(ir[1], r) || im_region_region(or, ir[1], r, r->left, r->top)) {
            return (-1);
        }
    } else if (all0) {

        if (im_prepare(ir[2], r) || im_region_region(or, ir[2], r, r->left, r->top)) {
            return (-1);
        }
    } else {

        if (im_prepare(ir[1], r) || im_prepare(ir[2], r)) {
            return (-1);
        }

        for (y = to; y < bo; y++) {
            PEL *cp = (PEL *)IM_REGION_ADDR(ir[0], le, y);
            PEL *ap = (PEL *)IM_REGION_ADDR(ir[1], le, y);
            PEL *bp = (PEL *)IM_REGION_ADDR(ir[2], le, y);
            PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

            if (c->Bands == 1) {
                blend1_buffer(q, cp, ap, bp, r->width, a);
            } else {
                blendn_buffer(q, cp, ap, bp, r->width, a);
            }
        }
    }

    return (0);
}

static int blend(IMAGE *c, IMAGE *a, IMAGE *b, IMAGE *out) {
    IMAGE **in;

    if (im_check_uncoded("im_blend", c) || im_check_uncoded("im_blend", a) || im_check_uncoded("im_blend", b) || im_check_format("im_blend", c, IM_BANDFMT_UCHAR) || im_check_format_same("im_blend", a, b) || im_check_bands_same("im_blend", a, b) || im_check_bands_1orn("im_blend", c, a) || im_piocheck(c, out) || im_pincheck(a) || im_pincheck(b)) {
        return (-1);
    }

    if (im_cp_descv(out, a, b, c, NULL)) {
        return (-1);
    }
    out->Bands = IM_MAX(c->Bands, a->Bands);
    if (im_demand_hint(out, IM_THINSTRIP, a, b, c, NULL)) {
        return (-1);
    }

    if (!(in = im_allocate_input_array(out, c, a, b, NULL)) || im_generate(out, im_start_many, blend_gen, im_stop_many, in, NULL)) {
        return (-1);
    }

    return (0);
}

int im_blend(IMAGE *c, IMAGE *a, IMAGE *b, IMAGE *out) {

    const int repack = a->Coding == IM_CODING_LABQ && b->Coding == IM_CODING_LABQ;

    IMAGE *t[7];

    if (im_open_local_array(out, t, 7, "im_blend", "p")) {
        return (-1);
    }

    if (a->Coding == IM_CODING_LABQ) {
        if (im_LabQ2Lab(a, t[0])) {
            return (-1);
        }
        a = t[0];
    }
    if (b->Coding == IM_CODING_LABQ) {
        if (im_LabQ2Lab(b, t[1])) {
            return (-1);
        }
        b = t[1];
    }

    if (im__formatalike(a, b, t[2], t[3]) || im__bandalike(t[2], t[3], t[4], t[5])) {
        return (-1);
    }

    if (blend(c, t[4], t[5], t[6])) {
        return (-1);
    }

    if (repack) {
        if (im_Lab2LabQ(t[6], out)) {
            return (-1);
        }
    } else {
        if (im_copy(t[6], out)) {
            return (-1);
        }
    }

    return (0);
}
