
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_sines(IMAGE *image, int xsize, int ysize, double horfreq, double verfreq) {
    int x, y;
    float *line, *cpline;
    int size;
    double cons, factor;
    double theta_rad, costheta, sintheta, ysintheta;

    if (im_outcheck(image)) {
        return (-1);
    }
    if (xsize <= 0 || ysize <= 0) {
        im_error("im_sines", "%s", _("wrong sizes"));
        return (-1);
    }

    im_initdesc(image, xsize, ysize, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);

    if (im_setupout(image) == -1) {
        return (-1);
    }

    size = image->Xsize;
    if ((line = (float *)calloc((unsigned)size, sizeof(float))) == NULL) {
        im_error("im_sines", "%s", _("calloc failed"));
        return (-1);
    }

    if (horfreq == 0) {
        theta_rad = IM_PI / 2.0;
    } else {
        theta_rad = atan(verfreq / horfreq);
    }
    costheta = cos(theta_rad);
    sintheta = sin(theta_rad);
    factor = sqrt((double)(horfreq * horfreq + verfreq * verfreq));
    cons = factor * IM_PI * 2.0 / (double)image->Xsize;

    if (horfreq != 0) {
        for (y = 0; y < image->Ysize; y++) {
            ysintheta = y * sintheta;
            cpline = line;
            for (x = 0; x < image->Xsize; x++) {
                *cpline++ = (float)(cos(cons * (x * costheta - ysintheta)));
            }
            if (im_writeline(y, image, (PEL *)line) == -1) {
                free((char *)line);
                return (-1);
            }
        }
    } else {
        for (y = 0; y < image->Ysize; y++) {
            cpline = line;
            ysintheta = cos(-cons * y * sintheta);
            for (x = 0; x < image->Xsize; x++) {
                *cpline++ = (float)ysintheta;
            }
            if (im_writeline(y, image, (PEL *)line) == -1) {
                free((char *)line);
                return (-1);
            }
        }
    }
    free((char *)line);
    return (0);
}
