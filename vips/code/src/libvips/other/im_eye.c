
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

int im_feye(IMAGE *image, const int xsize, const int ysize, const double factor) {
    int x, y;
    double constant;
    double *lut;
    float *line;

    if (im_outcheck(image)) {
        return (-1);
    }
    if (factor > 1.0 || factor <= 0.0) {
        im_error("im_feye", "%s", _("factor should be in [1,0)"));
        return (-1);
    }

    im_initdesc(image, xsize, ysize, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);
    if (im_setupout(image)) {
        return (-1);
    }

    if (!(line = IM_ARRAY(image, xsize, float))) {
        return (-1);
    }

    if (!(lut = IM_ARRAY(image, image->Xsize, double))) {
        return (-1);
    }
    constant = factor * IM_PI / (2 * (xsize - 1));
    for (x = 0; x < image->Xsize; x++) {
        lut[x] = cos(constant * x * x) / ((ysize - 1) * (ysize - 1));
    }

    for (y = 0; y < image->Ysize; y++) {
        for (x = 0; x < image->Xsize; x++) {
            line[x] = y * y * lut[x];
        }
        if (im_writeline(y, image, (PEL *)line)) {
            return (-1);
        }
    }

    return (0);
}

int im_eye(IMAGE *image, const int xsize, const int ysize, const double factor) {
    IMAGE *t1 = im_open_local(image, "im_eye:1", "p");
    IMAGE *t2 = im_open_local(image, "im_eye:2", "p");

    if (!t1) {
        return (-1);
    }

    if (im_feye(t1, xsize, ysize, factor) || im_lintra(127.5, t1, 127.5, t2) || im_clip2fmt(t2, image, IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    return (0);
}
