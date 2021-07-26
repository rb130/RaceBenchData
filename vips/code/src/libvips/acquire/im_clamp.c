
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_clamp(in, out, black, hstep, vstep) IMAGE *in, *black, *out;
int hstep, vstep;
{
    PEL *p, *blk, *bline, *bexp;
    PEL *q, *outbuf;
    int rep;
    int x, y, bnd;
    int temp, blacky, newblacky;

    if (im_iocheck(in, out)) {
        return (-1);
    }
    if (in->Bbits != 8 || in->Coding != IM_CODING_NONE || in->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_clamp", "%s", _("bad input format"));
        return (-1);
    }
    if (black->Bbits != 8 || black->Coding != IM_CODING_NONE || black->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_clamp", "%s", _("bad black format"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(bline = (PEL *)im_malloc(out, black->Bands * hstep * in->Xsize))) {
        return (-1);
    }

    if (!(outbuf = (PEL *)im_malloc(out, out->Bands * out->Xsize))) {
        return (-1);
    }
    blacky = -1;
    p = (PEL *)in->data;

    for (y = 0; y < in->Ysize; y++) {

        newblacky = (vstep * black->Ysize - in->Ysize + y) / vstep;
        if (newblacky != blacky) {
            blacky = newblacky;

            blk = (PEL *)(black->data + black->Xsize * black->Bands * blacky);
            for (bexp = bline, x = 0; x < black->Xsize; x++) {
                for (rep = 0; rep < hstep; rep++) {
                    for (q = blk, bnd = 0; bnd < in->Bands; bnd++) {
                        *bexp++ = *q++;
                    }
                }
                blk += black->Bands;
            }
        }

        bexp = bline;
        q = outbuf;
        for (x = 0; x < (out->Bands * out->Xsize); x++) {
            temp = ((int)*p++ - *bexp++);
            if (temp < 0) {
                temp = 0;
            }
            *q++ = (PEL)temp;
        }

        if (im_writeline(y, out, outbuf)) {
            return (-1);
        }
    }

    return (0);
}
