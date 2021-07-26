
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_readpoint(IMAGE *im, int x, int y, PEL *pel) {
    int es = IM_IMAGE_SIZEOF_ELEMENT(im);
    int ps = es * im->Bands;
    int ls = ps * im->Xsize;
    int b;
    PEL *from;

    if (im_rwcheck(im)) {
        return (-1);
    }

    if (x > im->Xsize || x < 0 || y > im->Ysize || y < 0) {
        im_error("im_readpoint", "%s", _("invalid cooordinates"));
        return (1);
    }

    from = (PEL *)im->data + x * ps + y * ls;
    for (b = 0; b < ps; b++) {
        *pel++ = *from++;
    }

    return (0);
}

int im_plotpoint(IMAGE *im, int x, int y, PEL *pel) {
    int es = IM_IMAGE_SIZEOF_ELEMENT(im);
    int ps = es * im->Bands;
    int ls = ps * im->Xsize;
    int b;
    PEL *to;

    if (im_rwcheck(im)) {
        return (-1);
    }

    if (x > im->Xsize || x < 0 || y > im->Ysize || y < 0) {
        return (0);
    }

    to = (PEL *)im->data + x * ps + y * ls;
    for (b = 0; b < ps; b++) {
        *to++ = *pel++;
    }

    return (0);
}
