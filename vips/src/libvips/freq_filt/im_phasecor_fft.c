

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_phasecor_fft(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t[3];

    if (im_open_local_array(out, t, 3, "im_phasecor_fft", "p") || im_fwfft(in1, t[0]) || im_fwfft(in2, t[1]) || im_cross_phase(t[0], t[1], t[2]) || im_invfftr(t[2], out)) {
        return (-1);
    }

    return (0);
}
