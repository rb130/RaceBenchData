

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#include "mosaic.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_correl(IMAGE *ref, IMAGE *sec, int xref, int yref, int xsec, int ysec, int hwindowsize, int hsearchsize, double *correlation, int *x, int *y) {
    IMAGE *surface = im_open("surface", "t");
    IMAGE *t1, *t2, *t3, *t4;

    Rect refr, secr;
    Rect winr, srhr;
    Rect wincr, srhcr;

    if (!surface || !(t1 = im_open_local(surface, "correlate:1", "p")) || !(t2 = im_open_local(surface, "correlate:1", "p")) || !(t3 = im_open_local(surface, "correlate:1", "p")) || !(t4 = im_open_local(surface, "correlate:1", "p"))) {
        return (-1);
    }

    refr.left = 0;
    refr.top = 0;
    refr.width = ref->Xsize;
    refr.height = ref->Ysize;
    winr.left = xref - hwindowsize;
    winr.top = yref - hwindowsize;
    winr.width = hwindowsize * 2 + 1;
    winr.height = hwindowsize * 2 + 1;
    im_rect_intersectrect(&refr, &winr, &wincr);

    secr.left = 0;
    secr.top = 0;
    secr.width = sec->Xsize;
    secr.height = sec->Ysize;
    srhr.left = xsec - hsearchsize;
    srhr.top = ysec - hsearchsize;
    srhr.width = hsearchsize * 2 + 1;
    srhr.height = hsearchsize * 2 + 1;
    im_rect_intersectrect(&secr, &srhr, &srhcr);

    if (im_extract_area(ref, t1, wincr.left, wincr.top, wincr.width, wincr.height) || im_extract_area(sec, t2, srhcr.left, srhcr.top, srhcr.width, srhcr.height)) {
        im_close(surface);
        return (-1);
    }

    if (t1->Bands != 1) {
        if (im_extract_band(t1, t3, 0)) {
            im_close(surface);
            return (-1);
        }
        t1 = t3;
    }
    if (t2->Bands != 1) {
        if (im_extract_band(t2, t4, 0)) {
            im_close(surface);
            return (-1);
        }
        t2 = t4;
    }

    if (im_spcor(t2, t1, surface)) {
        im_close(surface);
        return (-1);
    }

    if (im_maxpos(surface, x, y, correlation)) {
        im_close(surface);
        return (-1);
    }
    im_close(surface);

    *x += srhcr.left;
    *y += srhcr.top;

    return (0);
}

int im__chkpair(IMAGE *ref, IMAGE *sec, TIE_POINTS *points) {
    int i;
    int x, y;
    double correlation;

    const int hcor = points->halfcorsize;
    const int harea = points->halfareasize;

    if (im_incheck(ref) || im_incheck(sec)) {
        return (-1);
    }
    if (ref->Bands != sec->Bands || ref->BandFmt != sec->BandFmt || ref->Coding != sec->Coding) {
        im_error("im_chkpair", "%s", _("inputs incompatible"));
        return (-1);
    }
    if (ref->Bands != 1 || ref->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_chkpair", "%s", _("help!"));
        return (-1);
    }

    for (i = 0; i < points->nopoints; i++) {

        if (im_correl(ref, sec, points->x_reference[i], points->y_reference[i], points->x_reference[i], points->y_reference[i], hcor, harea, &correlation, &x, &y)) {
            return (-1);
        }

        points->x_secondary[i] = x;
        points->y_secondary[i] = y;
        points->correlation[i] = correlation;

        points->dx[i] = points->x_secondary[i] - points->x_reference[i];
        points->dy[i] = points->y_secondary[i] - points->y_reference[i];
    }

    return (0);
}
