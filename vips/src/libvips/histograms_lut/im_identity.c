

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

int im_identity(IMAGE *lut, int bands) {
    unsigned char *buf, *p;
    int x, z;

    if (im_outcheck(lut)) {
        return (-1);
    }
    if (bands < 0) {
        im_error("im_identity", "%s", _("bad bands"));
        return (-1);
    }

    im_initdesc(lut, 256, 1, bands, IM_BBITS_BYTE, IM_BANDFMT_UCHAR, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);

    if (im_setupout(lut)) {
        return (-1);
    }

    if (!(buf = (unsigned char *)im_malloc(lut, bands * 256))) {
        return (-1);
    }

    for (p = buf, x = 0; x < 256; x++) {
        for (z = 0; z < bands; z++) {
            *p++ = x;
        }
    }

    if (im_writeline(0, lut, buf)) {
        return (-1);
    }

    return (0);
}

int im_identity_ushort(IMAGE *lut, int bands, int sz) {
    unsigned short *buf, *p;
    int x, z;

    if (im_outcheck(lut)) {
        return (-1);
    }
    if (sz > 65536 || sz < 0) {
        im_error("im_identity_ushort", "%s", _("bad size"));
        return (-1);
    }
    if (bands < 0) {
        im_error("im_identity_ushort", "%s", _("bad bands"));
        return (-1);
    }

    im_initdesc(lut, sz, 1, bands, IM_BBITS_SHORT, IM_BANDFMT_USHORT, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);

    if (im_setupout(lut)) {
        return (-1);
    }

    if (!(buf = (unsigned short *)im_malloc(lut, sz * bands * sizeof(unsigned short)))) {
        return (-1);
    }

    for (p = buf, x = 0; x < sz; x++) {
        for (z = 0; z < bands; z++) {
            *p++ = x;
        }
    }
    if (im_writeline(0, lut, (PEL *)buf)) {
        return (-1);
    }

    return (0);
}
