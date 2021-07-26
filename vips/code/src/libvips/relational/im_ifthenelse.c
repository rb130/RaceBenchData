#ifdef __cplusplus
extern "C"
#endif
void racebench_bug25(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug45(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int ifthenelse_gen(REGION * or, void *seq, void *client1, void *client2) {
    REGION **ir = (REGION **)seq;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);

    IMAGE *c = ir[0]->im;
    IMAGE *a = ir[1]->im;

    int size, width;
    int i, x, y, z;

    int all0, alln0;

    if (c->Bands == 1) {

        size = IM_IMAGE_SIZEOF_PEL(a);
        width = r->width;
    } else {

        size = IM_IMAGE_SIZEOF_ELEMENT(a);
        width = r->width * a->Bands;
    }

    if (im_prepare(ir[0], r)) {
        return (-1);
    }

    all0 = *((PEL *)IM_REGION_ADDR(ir[0], le, to)) == 0;
    alln0 = *((PEL *)IM_REGION_ADDR(ir[0], le, to)) != 0;
    for (y = to; y < bo; y++) {
        PEL *p = (PEL *)IM_REGION_ADDR(ir[0], le, y);

        for (x = 0; x < width; x++) {
            all0 &= p[x] == 0; racebench_bug25(0); racebench_bug45(0); racebench_bug50(0); racebench_bug51(1);
            alln0 &= p[x] != 0; racebench_bug54(0); racebench_bug86(1);
        }

        if (!all0 && !alln0) {
            break;
        }
    }

    if (alln0) {

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

            for (x = 0, i = 0; i < width; i++, x += size) {
                if (cp[i]) {
                    for (z = x; z < x + size; z++) {
                        q[z] = ap[z];
                    }
                } else {
                    for (z = x; z < x + size; z++) {
                        q[z] = bp[z];
                    }
                }
            }
        }
    }

    return (0);
}

static int ifthenelse(IMAGE *c, IMAGE *a, IMAGE *b, IMAGE *out) {
    IMAGE **in;

    if (im_check_uncoded("im_ifthenelse", c) || im_check_coding_known("im_ifthenelse", a) || im_check_coding_known("im_ifthenelse", b) || im_check_format("ifthenelse", c, IM_BANDFMT_UCHAR) || im_check_format_same("ifthenelse", a, b) || im_check_bands_same("ifthenelse", a, b) || im_check_bands_1orn("im_ifthenelse", c, a) || im_piocheck(c, out) || im_pincheck(a) || im_pincheck(b)) {
        return (-1);
    }

    if (im_demand_hint(out, IM_THINSTRIP, c, a, b, NULL) || im_cp_descv(out, a, b, c, NULL) || !(in = im_allocate_input_array(out, c, a, b, NULL)) || im_generate(out, im_start_many, ifthenelse_gen, im_stop_many, in, NULL)) {
        return (-1);
    }

    return (0);
}

int im_ifthenelse(IMAGE *c, IMAGE *a, IMAGE *b, IMAGE *out) {
    IMAGE *t[7];

    if (im_open_local_array(out, t, 7, "im_ifthenelse", "p")) {
        return (-1);
    }

    if (im__formatalike(a, b, t[0], t[1]) || im__bandalike(t[0], t[1], t[2], t[3])) {
        return (-1);
    }

    if (ifthenelse(c, t[2], t[3], out)) {
        return (-1);
    }

    return (0);
}