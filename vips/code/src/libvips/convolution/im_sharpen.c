
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
    int *lut;
    int x1, x2, x3;
} SharpenLut;

static SharpenLut *build_lut(IMAGE *out, int x1, int x2, int x3, double m1, double m2) {
    int i;
    SharpenLut *slut = IM_NEW(out, SharpenLut);

    if (!slut) {
        return (NULL);
    }

    if (!(slut->lut = IM_ARRAY(out, x2 + x3 + 1, int))) {
        return (NULL);
    }
    slut->x1 = x1;
    slut->x2 = x2;
    slut->x3 = x3;

    for (i = 0; i < x1; i++) {
        slut->lut[x3 + i] = i * m1;
        slut->lut[x3 - i] = -i * m1;
    }
    for (i = x1; i <= x2; i++) {
        slut->lut[x3 + i] = x1 * m1 + (i - x1) * m2;
    }
    for (i = x1; i <= x3; i++) {
        slut->lut[x3 - i] = -(x1 * m1 + (i - x1) * m2);
    }

    return (slut);
}

static void buf_difflut(short **in, short *out, int n, SharpenLut *slut) {
    int range = slut->x2 + slut->x3;
    int *lut = slut->lut;
    int x3 = slut->x3;
    short *p1 = in[1];
    short *p2 = in[0];
    int i;

    for (i = 0; i < n; i++) {
        int v1 = p1[i];
        int v2 = p2[i];

        if (v2 == 0) {
            out[i] = v1;
        } else {

            int s1 = x3 + (v1 - v2);
            int s2;

            if (s1 < 0) {
                s1 = 0;
            } else if (s1 > range) {
                s1 = range;
            }

            s2 = v1 + lut[s1];

            if (s2 < 0) {
                s2 = 0;
            } else if (s2 > 32767) {
                s2 = 32767;
            }

            out[i] = s2;
        }
    }
}

static INTMASK *sharpen_mask_new(int radius) {
    INTMASK *base;
    INTMASK *line;
    int total;
    int i;

    if (!(base = im_gauss_imask("big1", radius / 2, 0.2))) {
        return (NULL);
    }

    if (!(line = im_create_imask("sharpen-line", base->xsize, 1))) {
        im_free_imask(base);
        return (NULL);
    }

    total = 0;
    for (i = 0; i < base->xsize; i++) {
        line->coeff[i] = base->coeff[base->xsize * (base->ysize / 2) + i];
        total += line->coeff[i];
    }
    line->scale = total;

    im_free_imask(base);

#ifdef DEBUG
    printf("sharpen_mask_new: created mask:\n");
    im_print_imask(line);
#endif

    return (line);
}

int im_sharpen(IMAGE *in, IMAGE *out, int mask_size, double x1, double y2, double y3, double m1, double m2) {
    IMAGE *arry[3];
    IMAGE *t[4];
    INTMASK *mask;
    SharpenLut *slut;

    double x2 = (y2 - x1 * (m1 - m2)) / m2;
    double x3 = (y3 - x1 * (m1 - m2)) / m2;

    if (in->Coding == IM_CODING_LABQ) {
        IMAGE *tc[2];

        if (im_open_local_array(out, tc, 2, "im_sharpen:1", "p") || im_LabQ2LabS(in, tc[0]) || im_sharpen(tc[0], tc[1], mask_size, x1, y2, y3, m1, m2) || im_LabS2LabQ(tc[1], out)) {
            return (-1);
        }

        return (0);
    }

    if (im_piocheck(in, out) || im_check_uncoded("im_sharpen", in) || im_check_bands("im_gradcor", in, 3) || im_check_format("im_gradcor", in, IM_BANDFMT_SHORT)) {
        return (-1);
    }

    if (x1 < 0 || x1 > 99 || x2 < 0 || x2 > 99 || x1 > x2 || x3 < 0 || x3 > 99 || x1 > x3) {
        im_error("im_sharpen", "%s", _("parameters out of range"));
        return (-1);
    }

    if (!(mask = im_local_imask(out, sharpen_mask_new(mask_size)))) {
        return (-1);
    }

    if (!(slut = build_lut(out, x1 * 327.67, x2 * 327.67, x3 * 327.67, m1, m2))) {
        return (-1);
    }

    if (im_open_local_array(out, t, 4, "im_sharpen:2", "p")) {
        return (-1);
    }

    if (im_extract_band(in, t[0], 0) || im_extract_bands(in, t[1], 1, 2) || im_convsep(t[0], t[2], mask)) {
        return (-1);
    }

    if (im_cp_desc(t[3], t[2])) {
        return (-1);
    }
    arry[0] = t[2];
    arry[1] = t[0];
    arry[2] = NULL;
    if (im_wrapmany(arry, t[3], (im_wrapmany_fn)buf_difflut, slut, NULL)) {
        return (-1);
    }

    if (im_bandjoin(t[3], t[1], out)) {
        return (-1);
    }

    return (0);
}
