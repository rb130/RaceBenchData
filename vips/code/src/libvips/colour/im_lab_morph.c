
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *in, *out;

    double L_scale, L_offset;

    double a_offset[101], b_offset[101];
    double a_scale, b_scale;
} Params;

static int morph_init(Params *parm, IMAGE *in, IMAGE *out, double L_scale, double L_offset, DOUBLEMASK *mask, double a_scale, double b_scale) {
    int i, j;

    parm->in = in;
    parm->out = out;
    parm->L_scale = L_scale;
    parm->L_offset = L_offset;
    parm->a_scale = a_scale;
    parm->b_scale = b_scale;

    if (mask->xsize != 3 || mask->ysize < 1 || mask->ysize > 100) {
        im_error("im_lab_morph", "%s", _("bad greyscale mask size"));
        return (-1);
    }
    for (i = 0; i < mask->ysize; i++) {
        double L = mask->coeff[i * 3];
        double a = mask->coeff[i * 3 + 1];
        double b = mask->coeff[i * 3 + 2];

        if (L < 0 || L > 100 || a < -120 || a > 120 || b < -120 || b > 120) {
            im_error("im_lab_morph", _("bad greyscale mask value, row %d"), i);
            return (-1);
        }
    }

    for (i = 0; i <= 100; i++) {
        double L_low = 0;
        double a_low = 0;
        double b_low = 0;

        double L_high = 100;
        double a_high = 0;
        double b_high = 0;

        for (j = 0; j < mask->ysize; j++) {
            double L = mask->coeff[j * 3];
            double a = mask->coeff[j * 3 + 1];
            double b = mask->coeff[j * 3 + 2];

            if (L < i && L > L_low) {
                L_low = L;
                a_low = a;
                b_low = b;
            }
        }

        for (j = mask->ysize - 1; j >= 0; j--) {
            double L = mask->coeff[j * 3];
            double a = mask->coeff[j * 3 + 1];
            double b = mask->coeff[j * 3 + 2];

            if (L >= i && L < L_high) {
                L_high = L;
                a_high = a;
                b_high = b;
            }
        }

        parm->a_offset[i] = a_low + (a_high - a_low) * ((i - L_low) / (L_high - L_low));
        parm->b_offset[i] = b_low + (b_high - b_low) * ((i - L_low) / (L_high - L_low));
    }

    return (0);
}

static void morph_buffer(float *in, float *out, int width, Params *parm) {
    int x;

    for (x = 0; x < width; x++) {
        double L = in[0];
        double a = in[1];
        double b = in[2];

        L = IM_CLIP(0, L, 100);
        a -= parm->a_offset[(int)L];
        b -= parm->b_offset[(int)L];

        L = (L + parm->L_offset) * parm->L_scale;
        L = IM_CLIP(0, L, 100);

        a *= parm->a_scale;
        b *= parm->b_scale;

        out[0] = L;
        out[1] = a;
        out[2] = b;

        in += 3;
        out += 3;
    }
}

int im_lab_morph(IMAGE *in, IMAGE *out, DOUBLEMASK *mask, double L_offset, double L_scale, double a_scale, double b_scale) {
    Params *parm;

    if (in->Coding == IM_CODING_LABQ) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "im_lab_morph", "p") || im_LabQ2Lab(in, t[0]) || im_lab_morph(t[0], t[1], mask, L_offset, L_scale, a_scale, b_scale) || im_Lab2LabQ(t[1], out)) {
            return (-1);
        }

        return (0);
    }

    if (!(parm = IM_NEW(out, Params)) || morph_init(parm, in, out, L_scale, L_offset, mask, a_scale, b_scale)) {
        return (-1);
    }

    return (im__colour_unary("im_lab_morph", in, out, IM_TYPE_LAB, (im_wrapone_fn)morph_buffer, parm, NULL));
}
