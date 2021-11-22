

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_LabS2Lab(signed short *in, float *out, int n) {
    signed short *p = in;
    float *q = out;
    int c;

    for (c = 0; c < n; c++) {
        q[0] = p[0] / (32767.0 / 100.0);
        q[1] = p[1] / (32768.0 / 128.0);
        q[2] = p[2] / (32768.0 / 128.0);

        p += 3;
        q += 3;
    }
}

int im_LabS2Lab(IMAGE *in, IMAGE *out) {
    if (im_check_uncoded("im_LabS2Lab", in) || im_check_bands("im_LabS2Lab", in, 3) || im_check_format("im_LabS2Lab", in, IM_BANDFMT_SHORT)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Type = IM_TYPE_LAB;
    out->BandFmt = IM_BANDFMT_FLOAT;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_LabS2Lab, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
