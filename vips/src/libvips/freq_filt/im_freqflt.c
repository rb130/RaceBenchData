

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

int im_freqflt(IMAGE *in, IMAGE *mask, IMAGE *out) {
    IMAGE *dummy;

    if (!(dummy = im_open("memory-1", "p"))) {
        return (-1);
    }

    if (vips_bandfmt_iscomplex(in->BandFmt)) {

        IMAGE *t1 = im_open_local(dummy, "im_freqflt-1", "p");

        if (!t1 || im_multiply(in, mask, t1) || im_invfftr(t1, out)) {
            im_close(dummy);
            return (-1);
        }
    } else {

        IMAGE *t[3];
        IMAGE *t3;

        if (im_open_local_array(dummy, t, 3, "im_freqflt-1", "p") || !(t3 = im_open_local(out, "im_freqflt-3", "t")) || im_fwfft(in, t[0]) || im_multiply(t[0], mask, t[1]) || im_invfftr(t[1], t[2]) || im_clip2fmt(t[2], t3, in->BandFmt) || im_copy(t3, out)) {
            im_close(dummy);
            return (-1);
        }
    }

    im_close(dummy);

    return (0);
}
