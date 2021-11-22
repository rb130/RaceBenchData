

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

static int disp_ps(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    IMAGE *t[3];

    if (im_open_local_array(out, t, 3, "im_disp_ps temp 1", "p")) {
        return (-1);
    }

    if (in->BandFmt != IM_BANDFMT_COMPLEX) {
        if (im_fwfft(in, t[0])) {
            return (-1);
        }
        in = t[0];
    }

    if (im_abs(in, t[1]) || im_scaleps(t[1], t[2]) || im_rotquad(t[2], out)) {
        return (-1);
    }

    return (0);
}

int im_disp_ps(IMAGE *in, IMAGE *out) {
    IMAGE *dummy;

    if (!(dummy = im_open("memory:1", "p"))) {
        return (-1);
    }
    if (disp_ps(dummy, in, out)) {
        im_close(dummy);
        return (-1);
    }
    im_close(dummy);

    return (0);
}
