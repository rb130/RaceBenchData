
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

int im_line(image, x1, y1, x2, y2, pelval) IMAGE *image;
int x1, x2, y1, y2, pelval;
{

    double x, y, dx, dy, m;
    long offset;
    double signx, signy;

    if (im_rwcheck(image)) {
        return (-1);
    }

    if ((x1 > image->Xsize) || (x1 < 0) || (y1 > image->Ysize) || (y1 < 0) || (x2 > image->Xsize) || (x2 < 0) || (y2 > image->Ysize) || (y2 < 0)) {
        im_error("im_line", "%s", _("invalid line cooordinates"));
        return (-1);
    }
    if ((pelval > 255) || (pelval < 0)) {
        im_error("im_line", "%s", _("line intensity between 0 and 255"));
        return (-1);
    }

    if (image->Bands != 1) {
        im_error("im_line", "%s", _("image should have one band only"));
        return (-1);
    }

    dx = (double)(x2 - x1);
    dy = (double)(y2 - y1);

    if (dx < 0.0) {
        signx = -1.0;
    } else {
        signx = 1.0;
    }

    if (dy < 0.0) {
        signy = -1.0;
    } else {
        signy = 1.0;
    }

    if (dx == 0.0) {
        x = x1;
        y = y1;
        while (y != y2) {
            offset = (int)(x + .5) + ((int)(y + .5)) * image->Xsize;
            *(image->data + offset) = (PEL)pelval;
            y += signy;
        }

        offset = x2 + y2 * image->Xsize;
        *(image->data + offset) = (PEL)pelval;
        return (0);
    }

    if (dy == 0.0) {
        y = y1;
        x = x1;
        while (x != x2) {
            offset = (int)(x + .5) + ((int)(y + .5)) * image->Xsize;
            *(image->data + offset) = (PEL)pelval;
            x += signx;
        }

        offset = x2 + y2 * image->Xsize;
        *(image->data + offset) = (PEL)pelval;
        return (0);
    }

    if (fabs(dy) < fabs(dx)) {
        m = fabs(dy / dx) * signy;
        y = y1;
        x = x1;
        while (x != x2) {
            offset = (int)(x + .5) + ((int)(y + .5)) * image->Xsize;
            *(image->data + offset) = (PEL)pelval;
            x += signx;
            y += m;
        }
    } else {
        m = fabs(dx / dy) * signx;
        x = x1;
        y = y1;
        while (y != y2) {
            offset = (int)(x + .5) + ((int)(y + .5)) * image->Xsize;
            *(image->data + offset) = (PEL)pelval;
            x += m;
            y += signy;
        }
    }

    offset = x2 + y2 * image->Xsize;
    *(image->data + offset) = (PEL)pelval;
    im_invalidate(image);
    return (0);
}
