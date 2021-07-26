
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_hsp(IMAGE *in, IMAGE *ref, IMAGE *out) {
    IMAGE *t[3];

    if (im_open_local_array(out, t, 3, "im_hsp", "p") || im_histgr(in, t[0], -1) || im_histgr(ref, t[1], -1) || im_histspec(t[0], t[1], t[2]) || im_maplut(in, out, t[2])) {
        return (-1);
    }

    return (0);
}
