

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

typedef struct {
    int x, y;
    int cont;
} PosCont;

static int all_black(IMAGE *im, int xpos, int ypos, int winsize) {
    const int hwinsize = (winsize - 1) / 2;
    const int left = xpos - hwinsize;
    const int top = ypos - hwinsize;
    const int ls = im->Xsize;

    int x, y;
    PEL *line;

    line = (PEL *)im->data + top * ls + left;
    for (y = 0; y < winsize; y++) {
        for (x = 0; x < winsize; x++) {
            if (line[x]) {

                return (0);
            }
        }

        line += ls;
    }

    return (-1);
}

static int calculate_contrast(IMAGE *im, int xpos, int ypos, int winsize) {
    const int hwinsize = (winsize - 1) / 2;
    const int left = xpos - hwinsize;
    const int top = ypos - hwinsize;
    const int ls = im->Xsize;

    int x, y;
    PEL *line, *p;
    int total;

    line = (PEL *)im->data + top * ls + left;
    for (total = 0, y = 0; y < winsize - 1; y++) {
        p = line;

        for (x = 0; x < winsize - 1; x++) {
            const int lrd = (int)p[0] - p[1];
            const int tbd = (int)p[0] - p[ls];

            total += abs(lrd) + abs(tbd);
            p += 1;
        }

        line += ls;
    }

    return (total);
}

static int pos_compare(const void *vl, const void *vr) {
    PosCont *l = (PosCont *)vl;
    PosCont *r = (PosCont *)vr;

    return (r->cont - l->cont);
}

int im__find_best_contrast(IMAGE *im, int xpos, int ypos, int xsize, int ysize, int xarray[], int yarray[], int cont[], int nbest, int hcorsize) {

    const int windowsize = 2 * hcorsize + 1;

    const int nacross = (xsize - windowsize + hcorsize) / hcorsize;
    const int ndown = (ysize - windowsize + hcorsize) / hcorsize;

    int elms;

    PosCont *pc;

    int x, y, i;

    if (nacross <= 0 || ndown <= 0) {
        im_error("im__lrcalcon", "%s", _("overlap too small for your search size"));
        return (-1);
    }

    if (!(pc = IM_ARRAY(NULL, nacross * ndown, PosCont))) {
        return (-1);
    }

    for (i = 0, y = 0; y < ndown; y++) {
        for (x = 0; x < nacross; x++) {
            const int left = xpos + x * hcorsize;
            const int top = ypos + y * hcorsize;

            if (all_black(im, left, top, windowsize)) {
                continue;
            }

            pc[i].x = left;
            pc[i].y = top;
            pc[i].cont = calculate_contrast(im, left, top, windowsize);
            i++;
        }
    }

    elms = i;

    if (elms < nbest) {
        im_error("im_mosaic", _("found %d tie-points, need at least %d"), elms, nbest);
        im_free(pc);
        return (-1);
    }

    qsort(pc, elms, sizeof(PosCont), pos_compare);

    for (i = 0; i < nbest; i++) {
        xarray[i] = pc[i].x;
        yarray[i] = pc[i].y;
        cont[i] = pc[i].cont;
    }
    im_free(pc);

    return (0);
}

int im__lrcalcon(IMAGE *ref, TIE_POINTS *points) {

    const int border = points->halfareasize;

    const int aheight = ref->Ysize / AREAS;

    const int len = points->nopoints / AREAS;

    int i;
    Rect area;

    if (im_incheck(ref)) {
        return (-1);
    }
    if (ref->Bands != 1 || ref->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im__lrcalcon", "%s", _("not 1-band uchar image"));
        return (-1);
    }

    area.height = aheight;
    area.width = ref->Xsize;
    area.left = 0;
    area.top = 0;
    im_rect_marginadjust(&area, -border);
    area.width--;
    area.height--;

    for (i = 0; area.top < ref->Ysize; area.top += aheight, i++) {
        if (im__find_best_contrast(ref, area.left, area.top, area.width, area.height, points->x_reference + i * len, points->y_reference + i * len, points->contrast + i * len, len, points->halfcorsize)) {
            return (-1);
        }
    }

    return (0);
}
