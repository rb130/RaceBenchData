

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifndef HAVE_CBRT
#define cbrt(X) pow((X), 1.0 / 3.0)
#endif

#define QUANT_ELEMENTS (100000)

float cbrt_table[QUANT_ELEMENTS];

void imb_XYZ2Lab_tables(void) {
    static int built_tables = 0;

    int was_built;
    int i;

    g_mutex_lock(im__global_lock);
    was_built = built_tables;
    built_tables = 1;
    g_mutex_unlock(im__global_lock);
    if (was_built) {
        return;
    }

    for (i = 0; i < QUANT_ELEMENTS; i++) {
        float Y = (double)i / QUANT_ELEMENTS;

        if (Y < 0.008856) {
            cbrt_table[i] = 7.787 * Y + (16.0 / 116.0);
        } else {
            cbrt_table[i] = cbrt(Y);
        }
    }
}

void imb_XYZ2Lab(float *p, float *q, int n, im_colour_temperature *temp) {
    int x;

    for (x = 0; x < n; x++) {
        float nX, nY, nZ;
        int i;
        float f;
        float cbx, cby, cbz;

        nX = QUANT_ELEMENTS * p[0] / temp->X0;
        nY = QUANT_ELEMENTS * p[1] / temp->Y0;
        nZ = QUANT_ELEMENTS * p[2] / temp->Z0;
        p += 3;

        i = (int)nX;
        if (i < 0) {
            i = 0;
        }
        if (i > QUANT_ELEMENTS - 2) {
            i = QUANT_ELEMENTS - 2;
        }
        f = nX - i;
        cbx = cbrt_table[i] + f * (cbrt_table[i + 1] - cbrt_table[i]);

        i = (int)nY;
        if (i < 0) {
            i = 0;
        }
        if (i > QUANT_ELEMENTS - 2) {
            i = QUANT_ELEMENTS - 2;
        }
        f = nY - i;
        cby = cbrt_table[i] + f * (cbrt_table[i + 1] - cbrt_table[i]);

        i = (int)nZ;
        if (i < 0) {
            i = 0;
        }
        if (i > QUANT_ELEMENTS - 2) {
            i = QUANT_ELEMENTS - 2;
        }
        f = nZ - i;
        cbz = cbrt_table[i] + f * (cbrt_table[i + 1] - cbrt_table[i]);

        q[0] = 116.0 * cby - 16.0;
        q[1] = 500.0 * (cbx - cby);
        q[2] = 200.0 * (cby - cbz);
        q += 3;
    }
}

int im_XYZ2Lab_temp(IMAGE *in, IMAGE *out, double X0, double Y0, double Z0) {
    im_colour_temperature *temp;

    if (!(temp = IM_NEW(out, im_colour_temperature))) {
        return (-1);
    }
    temp->X0 = X0;
    temp->Y0 = Y0;
    temp->Z0 = Z0;
    imb_XYZ2Lab_tables();

    return (im__colour_unary("im_XYZ2Lab", in, out, IM_TYPE_LAB, (im_wrapone_fn)imb_XYZ2Lab, temp, NULL));
}

int im_XYZ2Lab(IMAGE *in, IMAGE *out) {
    return (im_XYZ2Lab_temp(in, out, IM_D65_X0, IM_D65_Y0, IM_D65_Z0));
}
