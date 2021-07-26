
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#include "mosaic.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__avgdxdy(TIE_POINTS *points, int *dx, int *dy) {
    int sumdx, sumdy;
    int i;

    if (points->nopoints == 0) {
        im_error("im_avgdxdy", "%s", _("no points to average"));
        return (-1);
    }

    sumdx = 0;
    sumdy = 0;
    for (i = 0; i < points->nopoints; i++) {
        sumdx += points->x_secondary[i] - points->x_reference[i];
        sumdy += points->y_secondary[i] - points->y_reference[i];
    }

    *dx = IM_RINT((double)sumdx / (double)points->nopoints);
    *dy = IM_RINT((double)sumdy / (double)points->nopoints);

    return (0);
}
