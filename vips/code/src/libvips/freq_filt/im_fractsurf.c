
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_fractsurf(IMAGE *out, int size, double frd) {
    IMAGE *noise;

    if (frd <= 2.0 || frd >= 3.0) {
        im_error("im_fractsurf", "%s", _("dimension should be in (2,3)"));
        return (-1);
    }

    if (!(noise = im_open_local(out, "im_fractsurf", "p")) || im_gaussnoise(noise, size, size, 0.0, 1.0) || im_flt_image_freq(noise, out, VIPS_MASK_FRACTAL_FLT, frd)) {
        return (-1);
    }

    return (0);
}
