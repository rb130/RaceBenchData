
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

void imb_Lab2XYZ(float *p, float *q, int n, im_colour_temperature *temp) {
    int x;

    for (x = 0; x < n; x++) {
        float L, a, b;
        float X, Y, Z;
        double cby, tmp;

        L = p[0];
        a = p[1];
        b = p[2];
        p += 3;

        if (L < 8.0) {
            Y = (L * temp->Y0) / 903.3;
            cby = 7.787 * (Y / temp->Y0) + 16.0 / 116.0;
        } else {
            cby = (L + 16.0) / 116.0;
            Y = temp->Y0 * cby * cby * cby;
        }

        tmp = a / 500.0 + cby;
        if (tmp < 0.2069) {
            X = temp->X0 * (tmp - 0.13793) / 7.787;
        } else {
            X = temp->X0 * tmp * tmp * tmp;
        }

        tmp = cby - b / 200.0;
        if (tmp < 0.2069) {
            Z = temp->Z0 * (tmp - 0.13793) / 7.787;
        } else {
            Z = temp->Z0 * tmp * tmp * tmp;
        }

        q[0] = X;
        q[1] = Y;
        q[2] = Z;
        q += 3;
    }
}

int im_Lab2XYZ_temp(IMAGE *in, IMAGE *out, double X0, double Y0, double Z0) {
    im_colour_temperature *temp;

    if (!(temp = IM_NEW(out, im_colour_temperature))) {
        return (-1);
    }
    temp->X0 = X0;
    temp->Y0 = Y0;
    temp->Z0 = Z0;

    return (im__colour_unary("im_Lab2XYZ_temp", in, out, IM_TYPE_XYZ, (im_wrapone_fn)imb_Lab2XYZ, temp, NULL));
}

int im_Lab2XYZ(IMAGE *in, IMAGE *out) {
    return (im_Lab2XYZ_temp(in, out, IM_D65_X0, IM_D65_Y0, IM_D65_Z0));
}
