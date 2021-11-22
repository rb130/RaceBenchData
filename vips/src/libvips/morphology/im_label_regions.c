

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_label_regions(IMAGE *test, IMAGE *mask, int *segments) {
    IMAGE *t[2];
    int serial;
    int *m;
    int x, y;

    if (im_open_local_array(mask, t, 2, "im_label_regions", "p") || im_black(t[0], test->Xsize, test->Ysize, 1) || im_clip2fmt(t[0], t[1], IM_BANDFMT_INT)) {
        return (-1);
    }

    if (im_rwcheck(t[1])) {
        return (-1);
    }
    serial = 0;
    m = (int *)t[1]->data;
    for (y = 0; y < test->Ysize; y++) {
        for (x = 0; x < test->Xsize; x++) {
            if (!m[x]) {

                if (im_flood_other(test, t[1], x, y, serial, NULL)) {

                    return (-1);
                }

                serial += 1;
            }
        }

        m += test->Xsize;
    }

    if (im_copy(t[1], mask)) {
        return (-1);
    }
    if (segments) {
        *segments = serial;
    }

    return (0);
}
