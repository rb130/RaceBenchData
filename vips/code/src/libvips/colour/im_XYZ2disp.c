
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_XYZ2disp(float *p, PEL *q, int n, struct im_col_display *d) {
    struct im_col_tab_disp *table = im_col_display_get_table(d);
    float rstep = (d->d_YCR - d->d_Y0R) / 1500.0;
    float gstep = (d->d_YCG - d->d_Y0G) / 1500.0;
    float bstep = (d->d_YCB - d->d_Y0B) / 1500.0;

    int x;

    for (x = 0; x < n; x++) {
        float Yr, Yg, Yb;
        int i;
        int r, g, b;

        float X = p[0];
        float Y = p[1];
        float Z = p[2];

        p += 3;

        Yr = table->mat_XYZ2lum[0][0] * X + table->mat_XYZ2lum[0][1] * Y + table->mat_XYZ2lum[0][2] * Z;
        Yg = table->mat_XYZ2lum[1][0] * X + table->mat_XYZ2lum[1][1] * Y + table->mat_XYZ2lum[1][2] * Z;
        Yb = table->mat_XYZ2lum[2][0] * X + table->mat_XYZ2lum[2][1] * Y + table->mat_XYZ2lum[2][2] * Z;

        Yr = IM_MAX(Yr, d->d_Y0R);
        Yg = IM_MAX(Yg, d->d_Y0G);
        Yb = IM_MAX(Yb, d->d_Y0B);

        i = IM_MIN(1500, (Yr - d->d_Y0R) / rstep);
        r = table->t_Yr2r[i];

        i = IM_MIN(1500, (Yg - d->d_Y0G) / gstep);
        g = table->t_Yg2g[i];

        i = IM_MIN(1500, (Yb - d->d_Y0B) / bstep);
        b = table->t_Yb2b[i];

        r = IM_MIN(r, d->d_Vrwr);
        g = IM_MIN(g, d->d_Vrwg);
        b = IM_MIN(b, d->d_Vrwb);

        q[0] = r;
        q[1] = g;
        q[2] = b;

        q += 3;
    }
}

int im_XYZ2disp(IMAGE *in, IMAGE *out, struct im_col_display *d) {

    if (in->Bands != 3 || in->BandFmt != IM_BANDFMT_FLOAT || in->Coding != IM_CODING_NONE) {
        im_error("im_XYZ2disp", "%s", _("3-band uncoded float only"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Type = IM_TYPE_RGB;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_XYZ2disp, d, NULL)) {
        return (-1);
    }

    return (0);
}
