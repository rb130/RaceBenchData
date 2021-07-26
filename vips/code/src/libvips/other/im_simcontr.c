
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

int im_simcontr(IMAGE *image, int xs, int ys) {
    int x, y;
    unsigned char *line1, *line2, *cpline;

    if (im_outcheck(image)) {
        return (-1);
    }

    im_initdesc(image, xs, ys, 1, IM_BBITS_BYTE, IM_BANDFMT_UCHAR, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);

    if (im_setupout(image) == -1) {
        return (-1);
    }

    line1 = (unsigned char *)calloc((unsigned)xs, sizeof(char));
    line2 = (unsigned char *)calloc((unsigned)xs, sizeof(char));
    if ((line1 == NULL) || (line2 == NULL)) {
        im_error("im_simcontr", "%s", _("calloc failed"));
        return (-1);
    }

    cpline = line1;
    for (x = 0; x < xs; x++) {
        *cpline++ = (PEL)255;
    }
    cpline = line1;
    for (x = 0; x < xs / 2; x++) {
        *cpline++ = (PEL)0;
    }

    cpline = line2;
    for (x = 0; x < xs; x++) {
        *cpline++ = (PEL)255;
    }
    cpline = line2;
    for (x = 0; x < xs / 8; x++) {
        *cpline++ = (PEL)0;
    }
    for (x = 0; x < xs / 4; x++) {
        *cpline++ = (PEL)128;
    }
    for (x = 0; x < xs / 8; x++) {
        *cpline++ = (PEL)0;
    }
    for (x = 0; x < xs / 8; x++) {
        *cpline++ = (PEL)255;
    }
    for (x = 0; x < xs / 4; x++) {
        *cpline++ = (PEL)128;
    }

    for (y = 0; y < ys / 4; y++) {
        if (im_writeline(y, image, (PEL *)line1) == -1) {
            free((char *)line1);
            free((char *)line2);
            return (-1);
        }
    }
    for (y = ys / 4; y < (ys / 4 + ys / 2); y++) {
        if (im_writeline(y, image, (PEL *)line2) == -1) {
            free((char *)line1);
            free((char *)line2);
            return (-1);
        }
    }
    for (y = (ys / 4 + ys / 2); y < ys; y++) {
        if (im_writeline(y, image, (PEL *)line1) == -1) {
            free((char *)line1);
            free((char *)line2);
            return (-1);
        }
    }
    free((char *)line1);
    free((char *)line2);
    return (0);
}
