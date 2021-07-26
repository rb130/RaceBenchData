
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_compass(IMAGE *in, IMAGE *out, INTMASK *mask) {
    IMAGE *filtered[8];
    IMAGE *absed[8];
    int i;

    if (im_open_local_array(out, filtered, 8, "im_compass:1", "p") || im_open_local_array(out, absed, 8, "im_compass:2", "p")) {
        return (-1);
    }

    for (i = 0; i < 8; i++) {
        if (im_conv(in, filtered[i], mask) || !(mask = im_local_imask(out, im_rotate_imask45(mask, mask->filename)))) {
            return (-1);
        }
    }

    for (i = 0; i < 8; i++) {
        if (im_abs(filtered[i], absed[i])) {
            return (-1);
        }
    }

    return (im_maxvalue(absed, out, 8));
}

int im_lindetect(IMAGE *in, IMAGE *out, INTMASK *mask) {
    IMAGE *filtered[4];
    IMAGE *absed[4];
    int i;

    if (im_open_local_array(out, filtered, 4, "im_lindetect:1", "p") || im_open_local_array(out, absed, 4, "im_lindetect:2", "p")) {
        return (-1);
    }

    for (i = 0; i < 4; i++) {
        if (im_conv(in, filtered[i], mask) || !(mask = im_local_imask(out, im_rotate_imask45(mask, mask->filename)))) {
            return (-1);
        }
    }

    for (i = 0; i < 4; i++) {
        if (im_abs(filtered[i], absed[i])) {
            return (-1);
        }
    }

    return (im_maxvalue(absed, out, 4));
}

int im_gradient(IMAGE *in, IMAGE *out, INTMASK *mask) {
    IMAGE *t[4];
    INTMASK *rmask;

    if (im_open_local_array(out, t, 4, "im_gradient", "p")) {
        return (-1);
    }

    if (!(rmask = im_local_imask(out, im_rotate_imask90(mask, mask->filename)))) {
        return (-1);
    }

    if (im_conv(in, t[0], mask) || im_conv(in, t[1], rmask) || im_abs(t[0], t[2]) || im_abs(t[1], t[3]) || im_add(t[2], t[3], out)) {
        return (-1);
    }

    return (0);
}
