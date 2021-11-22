

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_dif_std(im, xpos, ypos, xsize, ysize, dx, dy, pmean, pstd)
IMAGE *im;
int xpos, ypos, xsize, ysize;
int dx, dy;
double *pmean, *pstd;
{
    PEL *input, *cpinput;
    double m, s;
    int *buf, *pbuf;
    int x, y;
    int ofst, bufsize;

    if (im_incheck(im)) {
        return (-1);
    }

    if ((im->Bands != 1) || (im->BandFmt != IM_BANDFMT_UCHAR)) {
        im_error("im_dif_std", "%s", _("Unable to accept input"));
        return (-1);
    }
    if ((xpos + xsize + dx > im->Xsize) || (ypos + ysize + dy > im->Ysize)) {
        im_error("im_dif_std", "%s", _("wrong args"));
        return (-1);
    }

    bufsize = xsize * ysize;
    buf = (int *)calloc((unsigned)bufsize, sizeof(int));
    if (buf == NULL) {
        im_error("im_dif_std", "%s", _("calloc failed"));
        return (-1);
    }
    input = (PEL *)im->data;
    input += (ypos * im->Xsize + xpos);
    ofst = dy * im->Xsize + dx;
    pbuf = buf;
    for (y = 0; y < ysize; y++) {
        cpinput = input;
        input += im->Xsize;
        for (x = 0; x < xsize; x++) {
            *pbuf++ = ((int)(*cpinput)) - ((int)(*(cpinput + ofst)));
            cpinput++;
        }
    }

    m = 0.0;
    s = 0.0;
    if (im__mean_std_int_buffer(buf, bufsize, &m, &s)) {
        return (-1);
    }
    *pmean = m;
    *pstd = s;
    free((char *)buf);

    return (0);
}
