

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int im_litecor0(in, white, out)
IMAGE *in, *white, *out;
{
    PEL *p, *w;
    PEL *q, *bu;
    int c;
    int x, y;
    float xrat = (float)in->Xsize / white->Xsize;
    float yrat = (float)in->Ysize / white->Ysize;
    int xstep = (int)xrat;
    int ystep = (int)yrat;
    double max;
    int wtmp, maxw, maxout, temp;

    if (xrat < 1.0 || xrat != xstep || yrat < 1.0 || yrat != ystep) {
        im_error("im_litecor", "white not simple scale of image");
        return (-1);
    }

    if (im_max(white, &max)) {
        return (-1);
    }
    maxw = (int)max;

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(bu = (PEL *)im_malloc(out, out->Xsize))) {
        return (-1);
    }

    maxout = -1;
    p = (PEL *)in->data;
    for (y = 0; y < in->Ysize; y++) {

        w = (PEL *)(white->data + white->Xsize * (int)(y / ystep));
        c = 0;

        for (x = 0; x < out->Xsize; x++) {
            wtmp = (int)*w;
            temp = (maxw * (int)*p++ + (wtmp >> 1)) / wtmp;
            if (temp > maxout) {
                maxout = temp;
            }

            c++;
            if (c == xstep) {
                w++;
                c = 0;
            }
        }
    }

    p = (PEL *)in->data;
    if (maxout <= 255) {
        for (y = 0; y < in->Ysize; y++) {
            q = bu;
            w = (PEL *)(white->data + white->Xsize * (int)(y / ystep));
            c = 0;

            for (x = 0; x < in->Xsize; x++) {
                wtmp = (int)*w;
                *q++ = (PEL)((maxw * (int)*p++ + (wtmp >> 1)) / wtmp);

                c++;
                if (c == xstep) {
                    w++;
                    c = 0;
                }
            }
            if (im_writeline(y, out, bu)) {
                im_error("im_litecor", "im_writeline failed");
                return (-1);
            }
        }
    } else {
        for (y = 0; y < in->Ysize; y++) {
            q = bu;
            w = (PEL *)(white->data + white->Xsize * (int)(y / ystep));
            c = 0;

            for (x = 0; x < in->Xsize; x++) {
                wtmp = maxout * ((int)*w);
                *q++ = (PEL)((maxw * (int)*p++ * 255 + (wtmp >> 1)) / wtmp);

                c++;
                if (c == xstep) {
                    w++;
                    c = 0;
                }
            }
            if (im_writeline(y, out, bu)) {
                im_error("im_litecor", "im_writeline failed");
                return (-1);
            }
        }
    }

    return (0);
}

static int im_litecor1(in, white, out, factor)
IMAGE *in, *white, *out;
double factor;
{
    PEL *p, *w;
    PEL *q, *bu;
    int c;
    int x, y;
    float xrat = (float)in->Xsize / white->Xsize;
    float yrat = (float)in->Ysize / white->Ysize;
    int xstep = (int)xrat;
    int ystep = (int)yrat;
    double max;
    double maxw, temp;
    int nclipped = 0;

    if (xrat < 1.0 || xrat != xstep || yrat < 1.0 || yrat != ystep) {
        im_error("im_litecor", "white not simple scale of image");
        return (-1);
    }

    if (im_max(white, &max)) {
        return (-1);
    }
    maxw = max;

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    if (im_setupout(out)) {
        return (-1);
    }

    if (!(bu = (PEL *)im_malloc(out, out->Xsize))) {
        return (-1);
    }

    p = (PEL *)in->data;
    for (y = 0; y < in->Ysize; y++) {
        q = bu;
        w = (PEL *)(white->data + white->Xsize * (int)(y / ystep));
        c = 0;

        for (x = 0; x < out->Xsize; x++) {
            temp = ((factor * maxw * (int)*p++) / ((int)*w)) + 0.5;
            if (temp > 255.0) {
                temp = 255;
                nclipped++;
            }
            *q++ = temp;

            c++;
            if (c == xstep) {
                w++;
                c = 0;
            }
        }

        if (im_writeline(y, out, bu)) {
            return (-1);
        }
    }

    if (nclipped) {
        im_warn("im_litecor", "%d pels over 255 clipped", nclipped);
    }

    return (0);
}

int im_litecor(in, white, out, clip, factor)
IMAGE *in, *white, *out;
int clip;
double factor;
{

    if (im_iocheck(in, out)) {
        return (-1);
    }
    if (in->Bands != 1 || in->Coding != IM_CODING_NONE || in->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_litecor", "bad input format");
        return (-1);
    }
    if (white->Bands != 1 || white->Coding != IM_CODING_NONE || white->BandFmt != IM_BANDFMT_UCHAR) {
        im_error("im_litecor", "bad white format");
        return (-1);
    }

    switch (clip) {
    case 1:
        return (im_litecor1(in, white, out, factor));

    case 0:
        return (im_litecor0(in, white, out));

    default:
        im_error("im_litecor", "unknown flag %d", clip);
        return (-1);
    }
}
