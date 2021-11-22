

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

void imb_disp2XYZ(PEL *p, float *q, int n, struct im_col_display *d) {
    int x;

    for (x = 0; x < n; x++) {
        int r = p[0];
        int g = p[1];
        int b = p[2];
        float X, Y, Z;
        p += 3;

        im_col_rgb2XYZ(d, r, g, b, &X, &Y, &Z);

        q[0] = X;
        q[1] = Y;
        q[2] = Z;
        q += 3;
    }
}

int im_disp2XYZ(IMAGE *in, IMAGE *out, struct im_col_display *d) {

    if (in->Bands != 3 || in->BandFmt != IM_BANDFMT_UCHAR || in->Coding != IM_CODING_NONE) {
        im_error("im_disp2XYZ", "%s", _("input not 3-band uncoded char"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Type = IM_TYPE_XYZ;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_disp2XYZ, d, NULL)) {
        return (-1);
    }

    return (0);
}
