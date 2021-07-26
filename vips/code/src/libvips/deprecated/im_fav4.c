
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define ARGS "fav4: frame average 4 frames\nARGS: im1 im2 im3 im4 outfile"
#define NFRAMES 4

int im_fav4(IMAGE **in, IMAGE *out) {
    PEL *result, *buffer, *p1, *p2, *p3, *p4;
    int x, y;
    int linebytes, PICY;

    if (im_iocheck(in[1], out)) {
        return (-1);
    }

    if ((in[0]->BandFmt != IM_BANDFMT_CHAR) && (in[0]->BandFmt != IM_BANDFMT_UCHAR)) {
        return (-1);
    }

    if (im_cp_desc(out, in[1]) == -1) {
        return (-1);
    }
    if (im_setupout(out) == -1) {
        return (-1);
    }

    linebytes = in[0]->Xsize * in[0]->Bands;
    PICY = in[0]->Ysize;
    buffer = (PEL *)im_malloc(NULL, linebytes);
    memset(buffer, 0, linebytes);

    p1 = (PEL *)in[0]->data;
    p2 = (PEL *)in[1]->data;
    p3 = (PEL *)in[2]->data;
    p4 = (PEL *)in[3]->data;

    for (y = 0; y < PICY; y++) {
        result = buffer;

        for (x = 0; x < linebytes; x++) {
            *result++ = (PEL)((int)((int)*p1++ + (int)*p2++ + (int)*p3++ + (int)*p4++ + 2) >> 2);
        }
        im_writeline(y, out, buffer);
    }
    im_free(buffer);
    return (0);
}
