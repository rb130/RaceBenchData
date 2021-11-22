

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _Msb {
    size_t index;
    size_t width;
    size_t repeat;
} Msb;

static void byte_select(unsigned char *in, unsigned char *out, int n, Msb *msb) {
    unsigned char *stop = out + n * msb->repeat;

    for (in += msb->index; out < stop; in += msb->width, ++out) {
        *out = *in;
    }
}

static void byte_select_flip(unsigned char *in, unsigned char *out, int n, Msb *msb) {
    unsigned char *stop = out + n * msb->repeat;

    for (in += msb->index; out < stop; in += msb->width, ++out) {
        *out = 0x80 ^ *in;
    }
}

static void msb_labq(unsigned char *in, unsigned char *out, int n) {
    unsigned char *stop = in + (n << 2);

    for (; in < stop; in += 4, out += 3) {
        out[0] = in[0];
        out[1] = 0x80 ^ in[1];
        out[2] = 0x80 ^ in[2];
    }
}

int im_msb(IMAGE *in, IMAGE *out) {
    Msb *msb;
    im_wrapone_fn func;

    if (in->Coding == IM_CODING_NONE && in->BandFmt == IM_BANDFMT_UCHAR) {
        return (im_copy(in, out));
    }

    if (im_piocheck(in, out) || !(msb = IM_NEW(out, Msb))) {
        return (-1);
    }

    if (in->Coding == IM_CODING_NONE) {
        if (im_check_int("im_msb", in)) {
            return (-1);
        }

        msb->width = IM_IMAGE_SIZEOF_ELEMENT(in);
        msb->index = im_amiMSBfirst() ? 0 : msb->width - 1;
        msb->repeat = in->Bands;

        if (vips_bandfmt_isuint(in->BandFmt)) {
            func = (im_wrapone_fn)byte_select;
        } else {
            func = (im_wrapone_fn)byte_select_flip;
        }
    } else if (IM_CODING_LABQ == in->Coding) {
        func = (im_wrapone_fn)msb_labq;
    } else {
        im_error("im_msb", "%s", _("unknown coding"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;

    return (im_wrapone(in, out, func, msb, NULL));
}

int im_msb_band(IMAGE *in, IMAGE *out, int band) {
    Msb *msb;
    im_wrapone_fn func;

    if (band < 0) {
        im_error("im_msb_band", "%s", _("bad arguments"));
        return (-1);
    }

    if (im_piocheck(in, out) || !(msb = IM_NEW(out, Msb))) {
        return (-1);
    }

    if (in->Coding == IM_CODING_NONE) {
        if (im_check_int("im_msb_band", in)) {
            return (-1);
        }

        if (band >= in->Bands) {
            im_error("im_msb_band", "%s", _("image does not have that many bands"));
            return (-1);
        }

        msb->width = IM_IMAGE_SIZEOF_ELEMENT(in);
        msb->index = im_amiMSBfirst() ? msb->width * band : msb->width * (band + 1) - 1;
        msb->repeat = 1;

        if (vips_bandfmt_isuint(in->BandFmt)) {
            func = (im_wrapone_fn)byte_select;
        } else {
            func = (im_wrapone_fn)byte_select_flip;
        }
    } else if (IM_CODING_LABQ == in->Coding) {
        if (band > 2) {
            im_error("im_msb_band", "%s", _("image does not have that many bands"));
            return (-1);
        }
        msb->width = 4;
        msb->repeat = 1;
        msb->index = band;

        if (band) {
            func = (im_wrapone_fn)byte_select_flip;
        } else {
            func = (im_wrapone_fn)byte_select;
        }
    } else {
        im_error("im_msb", "%s", _("unknown coding"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Bands = 1;

    return (im_wrapone(in, out, func, msb, NULL));
}
