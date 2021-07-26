
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

int im_scaleps(IMAGE *in, IMAGE *out) {
    IMAGE *t[4];
    double mx;
    double scale;

    if (im_open_local_array(out, t, 4, "im_scaleps-1", "p") || im_max(in, &mx)) {
        return (-1);
    }

    if (mx <= 0.0) {

        return (im_black(out, in->Xsize, in->Ysize, in->Bands));
    }

    scale = 255.0 / log10(1.0 + pow(mx, .25));

    if (im_powtra(in, t[0], 0.25) || im_lintra(1.0, t[0], 1.0, t[1]) || im_log10tra(t[1], t[2]) || im_lintra(scale, t[2], 0.0, t[3]) || im_clip2fmt(t[3], out, IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    return (0);
}
