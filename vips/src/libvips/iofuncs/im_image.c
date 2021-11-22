

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

IMAGE *im_image(void *buffer, int width, int height, int bands, VipsBandFmt bandfmt) {
    IMAGE *im;

    if (width <= 0 || height <= 0 || bands <= 0 || bandfmt < 0 || bandfmt > IM_BANDFMT_DPCOMPLEX) {
        im_error("im_image", "%s", _("bad parameters"));
        return (NULL);
    }

    if (!(im = im_init("untitled"))) {
        return (NULL);
    }

    im->Xsize = width;
    im->Ysize = height;
    im->Bands = bands;
    im->BandFmt = bandfmt;
    im->Bbits = im_bits_of_fmt(bandfmt);
    im->Coding = IM_CODING_NONE;

    if (bands == 1) {
        im->Type = IM_TYPE_B_W;
    } else if (bands == 3) {
        im->Type = IM_TYPE_RGB;
    } else {
        im->Type = IM_TYPE_MULTIBAND;
    }

    im->data = (char *)buffer;
    im->dtype = IM_SETBUF_FOREIGN;

    return (im);
}
