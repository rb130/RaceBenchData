
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int copy_quarter(IMAGE *out, float *coeff_s) {
    float *line, *cpline;
    float *coeff, *cpcoeff;
    int x, y;
    int hxsplus1;

    if (!(line = IM_ARRAY(out, out->Xsize, float))) {
        return (-1);
    }

    hxsplus1 = out->Xsize / 2 + 1;
    coeff = coeff_s;
    for (y = 0; y < out->Ysize / 2; y++) {
        cpline = line;
        cpcoeff = coeff;
        coeff += hxsplus1;

        for (x = 0; x < out->Xsize / 2; x++) {
            *cpline++ = *cpcoeff++;
        }
        for (x = out->Xsize / 2; x < out->Xsize; x++) {
            *cpline++ = *cpcoeff--;
        }
        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    for (y = out->Ysize / 2; y < out->Ysize; y++) {
        cpline = line;
        cpcoeff = coeff;
        coeff -= hxsplus1;

        for (x = 0; x < out->Xsize / 2; x++) {
            *cpline++ = *cpcoeff++;
        }
        for (x = out->Xsize / 2; x < out->Xsize; x++) {
            *cpline++ = *cpcoeff--;
        }
        if (im_writeline(y, out, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

static int build_freq_mask(IMAGE *out, int xs, int ys, VipsMaskType flag, va_list ap) {
    float *coeff;
    extern float *im__create_quarter(IMAGE *, int, int, VipsMaskType, va_list);

    if (!im_ispoweroftwo(xs) || !im_ispoweroftwo(ys)) {
        im_error("im_freq_mask", "%s", _("mask sizes power of 2 only"));
        return (-1);
    }

    im_initdesc(out, xs, ys, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    switch (flag) {
    case VIPS_MASK_IDEAL_HIGHPASS:
    case VIPS_MASK_IDEAL_LOWPASS:
    case VIPS_MASK_BUTTERWORTH_HIGHPASS:
    case VIPS_MASK_BUTTERWORTH_LOWPASS:
    case VIPS_MASK_GAUSS_HIGHPASS:
    case VIPS_MASK_GAUSS_LOWPASS:

    case VIPS_MASK_IDEAL_RINGPASS:
    case VIPS_MASK_IDEAL_RINGREJECT:
    case VIPS_MASK_BUTTERWORTH_RINGPASS:
    case VIPS_MASK_BUTTERWORTH_RINGREJECT:
    case VIPS_MASK_GAUSS_RINGPASS:
    case VIPS_MASK_GAUSS_RINGREJECT:

    case VIPS_MASK_FRACTAL_FLT:

        if (!(coeff = im__create_quarter(out, xs, ys, flag, ap)) || copy_quarter(out, coeff)) {
            return (-1);
        }
        break;

    case VIPS_MASK_IDEAL_BANDPASS:
    case VIPS_MASK_IDEAL_BANDREJECT:
    case VIPS_MASK_BUTTERWORTH_BANDPASS:
    case VIPS_MASK_BUTTERWORTH_BANDREJECT:
    case VIPS_MASK_GAUSS_BANDPASS:
    case VIPS_MASK_GAUSS_BANDREJECT:

        if (im__fmaskcir(out, flag, ap)) {
            return (-1);
        }
        break;

    default:
        im_error("im_freq_mask", "%s", _("unimplemented mask type"));
        return (-1);
    }

    return (0);
}

int im_flt_image_freq(IMAGE *in, IMAGE *out, VipsMaskType flag, ...) {
    IMAGE *mask = im_open_local(out, "tempmask", "p");
    va_list ap;

    if (!mask) {
        return (-1);
    }

    va_start(ap, flag);
    if (build_freq_mask(mask, in->Xsize, in->Ysize, flag, ap)) {
        return (-1);
    }
    va_end(ap);

    if (im_freqflt(in, mask, out)) {
        return (-1);
    }

    return (0);
}

int im_create_fmask(IMAGE *out, int xsize, int ysize, VipsMaskType flag, ...) {
    va_list ap;

    va_start(ap, flag);
    if (build_freq_mask(out, xsize, ysize, flag, ap)) {
        return (-1);
    }
    va_end(ap);

    return (0);
}
