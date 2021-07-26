
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

void imb_Lab2LabS(float *in, signed short *out, int n) {
    float *p = in;
    signed short *q = out;
    int c;

    for (c = 0; c < n; c++) {
        q[0] = p[0] * (32767.0 / 100.0);
        q[1] = p[1] * (32768.0 / 128.0);
        q[2] = p[2] * (32768.0 / 128.0);

        q += 3;
        p += 3;
    }
}

int im_Lab2LabS(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_check_uncoded("im_Lab2LabS", in) || im_check_bands("im_Lab2LabS", in, 3) || im_open_local_array(out, t, 1, "im_Lab2LabS", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_cp_desc(out, t[0])) {
        return (-1);
    }
    out->Type = IM_TYPE_LABS;
    out->BandFmt = IM_BANDFMT_SHORT;

    if (im_wrapone(t[0], out, (im_wrapone_fn)imb_Lab2LabS, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
