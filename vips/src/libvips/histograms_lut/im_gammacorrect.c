

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_gammacorrect(IMAGE *in, IMAGE *out, double exponent) {
    IMAGE *t[4];
    double mx1, mx2;

    if (im_open_local_array(out, t, 4, "im_gammacorrect", "p") || im_check_u8or16("im_gammacorrect", in) || im_piocheck(in, out) || (in->BandFmt == IM_BANDFMT_UCHAR ? im_identity(t[0], 1) : im_identity_ushort(t[0], 1, 65536)) || im_powtra(t[0], t[1], exponent) || im_max(t[0], &mx1) || im_max(t[1], &mx2) || im_lintra(mx1 / mx2, t[1], 0, t[2]) || im_clip2fmt(t[2], t[3], in->BandFmt) || im_maplut(in, out, t[3])) {
        return (-1);
    }

    return (0);
}
