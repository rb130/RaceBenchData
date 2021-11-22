

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_point(IMAGE *im, VipsInterpolate *interpolate, double x, double y, int band, double *out) {
    IMAGE *t[2];

    if (band >= im->Bands || x < 0.0 || y < 0.0 || x > im->Xsize || y > im->Ysize) {
        im_error("im_point_bilinear", "%s", _("coords outside image"));
        return (-1);
    }

    if (im_open_local_array(im, t, 2, "im_point_bilinear", "p") || im_extract_band(im, t[0], band) || im_affinei(t[0], t[1], interpolate, 1, 0, 0, 1, x - floor(x), y - floor(y), floor(x), floor(y), 1, 1) || im_avg(t[1], out)) {
        return (-1);
    }

    return (0);
}

int im_point_bilinear(IMAGE *im, double x, double y, int band, double *out) {
    return (im_point(im, vips_interpolate_bilinear_static(), x, y, band, out));
}
