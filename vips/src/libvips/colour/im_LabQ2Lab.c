

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_LabQ2Lab(PEL *inp, float *outbuf, int n) {
    signed char *b;
    int l;
    int lsbs;
    int c;
    float *out;

    b = (signed char *)inp;
    out = outbuf;
    for (c = 0; c < n; c++) {

        lsbs = ((unsigned char *)b)[3];

        l = ((unsigned char *)b)[0];
        l = (l << 2) | (lsbs >> 6);
        out[0] = (float)l * (100.0 / 1023.0);

        l = (b[1] << 3) | ((lsbs >> 3) & 0x7);
        out[1] = (float)l * 0.125;

        l = (b[2] << 3) | (lsbs & 0x7);
        out[2] = (float)l * 0.125;

        b += 4;
        out += 3;
    }
}

int im_LabQ2Lab(IMAGE *in, IMAGE *out) {
    if (im_check_coding_labq("im_LabQ2Lab", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 3;
    out->Type = IM_TYPE_LAB;
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Coding = IM_CODING_NONE;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_LabQ2Lab, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
