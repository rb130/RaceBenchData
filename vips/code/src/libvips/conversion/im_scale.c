
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_scale(IMAGE *in, IMAGE *out) {
    DOUBLEMASK *stats;
    IMAGE *t = im_open_local(out, "im_scale:1", "p");
    double mx, mn;
    double scale, offset;

    if (!t || !(stats = im_stats(in))) {
        return (-1);
    }
    mn = stats->coeff[0];
    mx = stats->coeff[1];
    im_free_dmask(stats);

    if (mn == mx) {

        return (im_black(out, in->Xsize, in->Ysize, in->Bands));
    }
    scale = 255.0 / (mx - mn);
    offset = -(mn * scale);

    if (im_lintra(scale, in, offset, t) || im_clip2fmt(t, out, IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    return (0);
}
