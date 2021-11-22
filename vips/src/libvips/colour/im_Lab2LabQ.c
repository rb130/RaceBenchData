

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_Lab2LabQ(float *inp, unsigned char *outbuf, int n) {
    float *f, fval;
    int lsbs, intv;
    int Xc;
    unsigned char *out;

    out = outbuf;
    f = inp;
    for (Xc = 0; Xc < n; Xc++) {

        intv = 10.23 * f[0] + 0.5;
        if (intv > 1023) {
            intv = 1023;
        }
        if (intv < 0) {
            intv = 0;
        }
        lsbs = (intv & 0x3) << 6;
        out[0] = (intv >> 2);

        fval = 8.0 * f[1];
        intv = IM_RINT(fval);
        if (intv > 1023) {
            intv = 1023;
        } else if (intv < -1024) {
            intv = -1024;
        }

        lsbs |= (intv & 0x7) << 3;
        out[1] = (intv >> 3);

        fval = 8.0 * f[2];
        intv = IM_RINT(fval);
        if (intv > 1023) {
            intv = 1023;
        } else if (intv < -1024) {
            intv = -1024;
        }

        lsbs |= (intv & 0x7);
        out[2] = (intv >> 3);

        out[3] = lsbs;

        f += 3;
        out += 4;
    }
}

int im_Lab2LabQ(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_check_uncoded("im_Lab2LabQ", in) || im_check_bands("im_Lab2LabQ", in, 3) || im_open_local_array(out, t, 1, "im_Lab2LabQ", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_cp_desc(out, t[0])) {
        return (-1);
    }
    out->Bands = 4;
    out->Type = IM_TYPE_LAB;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_LABQ;

    if (im_wrapone(t[0], out, (im_wrapone_fn)imb_Lab2LabQ, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
