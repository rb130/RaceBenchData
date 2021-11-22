

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

int im_fzone(IMAGE *image, int size) {
    int x, y;
    int i, j;

    float *buf;
    const int size2 = size / 2;

    if (im_outcheck(image)) {
        return (-1);
    }
    if (size <= 0 || (size % 2) != 0) {
        im_error("im_zone", "%s", _("size must be even and positive"));
        return (-1);
    }

    im_initdesc(image, size, size, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);
    if (im_setupout(image)) {
        return (-1);
    }

    if (!(buf = IM_ARRAY(image, size, float))) {
        return (-1);
    }

    for (y = 0, j = -size2; j < size2; j++, y++) {
        for (x = 0, i = -size2; i < size2; i++, x++) {
            buf[x] = cos((IM_PI / size) * (i * i + j * j));
        }
        if (im_writeline(y, image, (PEL *)buf)) {
            return (-1);
        }
    }

    return (0);
}

int im_zone(IMAGE *im, int size) {
    IMAGE *t1 = im_open_local(im, "im_zone:1", "p");
    IMAGE *t2 = im_open_local(im, "im_zone:2", "p");

    if (!t1 || !t2) {
        return (-1);
    }

    if (im_fzone(t1, size) || im_lintra(127.5, t1, 127.5, t2) || im_clip2fmt(t2, im, IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    return (0);
}
