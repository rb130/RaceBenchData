

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_circle(IMAGE *im, int cx, int cy, int radius, int intensity) {
    PEL *start;
    int size = 0;
    int x, y, d, offset;

    if (im_rwcheck(im)) {
        return (-1);
    }

    if ((im->data == NULL) || (im->BandFmt != IM_BANDFMT_UCHAR) || (im->Bands != 1)) {
        im_error("im_circle: ", "%s", _("able to write input image"));
        return (-1);
    }
    if ((intensity > 255) || (intensity <= 0)) {
        im_error("im_circle", "%s", _("intensity between 0 and 255"));
        return (-1);
    }

    if (((radius + cy) > im->Ysize - 1) || ((cy - radius) < 0) || ((radius + cx) > im->Xsize - 1) || ((cx - radius) < 0)) {
        im_error("im_circle", "%s", _("The circle does not fit in image"));
        return (-1);
    }

    size = im->Xsize;
    start = (PEL *)im->data;
    offset = cy * im->Xsize + cx;
    x = 0;
    y = radius;
    d = 3 - 2 * radius;
    while (x < y) {
        *(start + offset + size * y + x) = (PEL)intensity;
        *(start + offset + size * x + y) = (PEL)intensity;
        *(start + offset + size * y - x) = (PEL)intensity;
        *(start + offset + size * x - y) = (PEL)intensity;
        *(start + offset - size * y - x) = (PEL)intensity;
        *(start + offset - size * x - y) = (PEL)intensity;
        *(start + offset - size * y + x) = (PEL)intensity;
        *(start + offset - size * x + y) = (PEL)intensity;
        if (d < 0) {
            d += (4 * x + 6);
        } else {
            d += (4 * (x - y) + 10);
            y--;
        }
        x++;
    }
    if (x == y) {
        *(start + offset + size * y + x) = (PEL)intensity;
        *(start + offset + size * x + y) = (PEL)intensity;
        *(start + offset + size * y - x) = (PEL)intensity;
        *(start + offset + size * x - y) = (PEL)intensity;
        *(start + offset - size * y - x) = (PEL)intensity;
        *(start + offset - size * x - y) = (PEL)intensity;
        *(start + offset - size * y + x) = (PEL)intensity;
        *(start + offset - size * x + y) = (PEL)intensity;
    }

    return (0);
}
