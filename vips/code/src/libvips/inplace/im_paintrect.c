
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

int im_paintrect(IMAGE *im, Rect *r, PEL *ink) {
    int es = IM_IMAGE_SIZEOF_ELEMENT(im);
    int ps = es * im->Bands;
    int ls = ps * im->Xsize;
    Rect image, clipped;
    int x, y, b;
    PEL *to;
    PEL *q;

    if (im_rwcheck(im)) {
        return (-1);
    }

    image.left = 0;
    image.top = 0;
    image.width = im->Xsize;
    image.height = im->Ysize;
    im_rect_intersectrect(r, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        return (0);
    }

    to = (PEL *)im->data + clipped.left * ps + clipped.top * ls;
    for (y = 0; y < clipped.height; y++) {
        q = to;

        for (x = 0; x < clipped.width; x++) {
            for (b = 0; b < ps; b++) {
                *q++ = ink[b];
            }
        }

        to += ls;
    }

    return (0);
}
