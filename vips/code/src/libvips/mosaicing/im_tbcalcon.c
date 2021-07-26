
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include "mosaic.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__tbcalcon(IMAGE *ref, TIE_POINTS *points) {

    const int border = points->halfareasize;

    const int awidth = ref->Xsize / AREAS;

    const int len = points->nopoints / AREAS;

    int i;
    Rect area;

    if (im_incheck(ref)) {
        return (-1);
    }
    if (ref->Bands != 1 || ref->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im__tbcalcon", "%s", _("help!"));
        return (-1);
    }

    area.width = awidth;
    area.height = ref->Ysize;
    area.left = 0;
    area.top = 0;
    im_rect_marginadjust(&area, -border);
    area.width--;
    area.height--;
    if (area.width < 0 || area.height < 0) {
        im_error("im__tbcalcon", "%s", _("overlap too small"));
        return (-1);
    }

    for (i = 0; area.left < ref->Xsize; area.left += awidth, i++) {
        if (im__find_best_contrast(ref, area.left, area.top, area.width, area.height, points->x_reference + i * len, points->y_reference + i * len, points->contrast + i * len, len, points->halfcorsize)) {
            return (-1);
        }
    }

    return (0);
}
