

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_heq(IMAGE *in, IMAGE *out, int bandno) {
    IMAGE *t[2];

    if (im_open_local_array(out, t, 2, "im_heq", "p") || im_histgr(in, t[0], bandno) || im_histeq(t[0], t[1]) || im_maplut(in, out, t[1])) {
        return (-1);
    }

    return (0);
}
