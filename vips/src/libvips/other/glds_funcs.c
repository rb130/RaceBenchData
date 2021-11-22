

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

int im_glds_matrix(IMAGE *im, IMAGE *m, int xpos, int ypos, int xsize, int ysize, int dx, int dy) {
    PEL *in, *cpin;
    int *b, *pb;
    double *l, *pl;
    int x, y;
    int ofs;
    int tmp;
    int norm;

    if (im_iocheck(im, m) == -1) {
        return (-1);
    }

    if ((im->Bands != 1) || (im->BandFmt != IM_BANDFMT_UCHAR)) {
        im_error("im_glds_matrix", "%s", _("Wrong input"));
        return (-1);
    }

    if ((xpos + xsize + dx > im->Xsize) || (ypos + ysize + dy > im->Ysize)) {
        im_error("im_glds_matrix", "%s", _("wrong args"));
        return (-1);
    }

    if (im_cp_desc(m, im) == -1) {
        return (-1);
    }
    m->Xsize = 256;
    m->Ysize = 1;
    m->BandFmt = IM_BANDFMT_DOUBLE;
    m->Type = IM_TYPE_B_W;

    if (im_setupout(m) == -1) {
        return (-1);
    }

    b = (int *)calloc((unsigned)m->Xsize, sizeof(int));
    l = (double *)calloc((unsigned)m->Xsize, sizeof(double));
    if ((b == NULL) || (l == NULL)) {
        im_error("im_glds_matrix", "%s", _("calloc failed"));
        return (-1);
    }

    in = (PEL *)im->data;
    in += (ypos * im->Xsize + xpos);
    ofs = dy * im->Xsize + dx;
    for (y = 0; y < ysize; y++) {
        cpin = in;
        in += im->Xsize;
        for (x = 0; x < xsize; x++) {
            tmp = abs((int)*cpin - (int)(*(cpin + ofs)));
            pb = (b + tmp);
            (*pb)++;
            cpin++;
        }
    }

    norm = xsize * ysize;
    pb = b;
    pl = l;
    for (x = 0; x < m->Xsize; x++) {
        *pl++ = ((double)(*pb++)) / (double)norm;
    }
    if (im_writeline(0, m, (PEL *)l) == -1) {
        return (-1);
    }

    free((char *)b);
    free((char *)l);
    return (0);
}

int im_glds_asm(IMAGE *m, double *asmoment) {
    double temp, tmpasm, *in;
    int i;

    if (im_incheck(m)) {
        return (-1);
    }

    if (m->Xsize != 256 || m->Ysize != 1 || m->Bands != 1 || m->BandFmt != IM_BANDFMT_DOUBLE) {
        im_error("im_glds_asm", "%s", _("unable to accept input"));
        return (-1);
    }
    tmpasm = 0.0;
    in = (double *)m->data;
    for (i = 0; i < m->Xsize; i++) {
        temp = *in++;
        tmpasm += (temp * temp);
    }
    *asmoment = tmpasm;
    return (0);
}

int im_glds_contrast(IMAGE *m, double *contrast) {
    double tmpcon, *in;
    int i;

    if (im_incheck(m)) {
        return (-1);
    }

    if (m->Xsize != 256 || m->Ysize != 1 || m->Bands != 1 || m->BandFmt != IM_BANDFMT_DOUBLE) {
        im_error("im_glds_contrast", "%s", _("wrong input"));
        return (-1);
    }
    tmpcon = 0.0;
    in = (double *)m->data;
    for (i = 0; i < m->Xsize; i++) {
        tmpcon += (((double)i) * ((double)i) * (*in));
        in++;
    }
    *contrast = tmpcon;
    return (0);
}

int im_glds_entropy(IMAGE *m, double *entropy) {
    double tmpent, dtemp, *in;
    int i;

    if (im_incheck(m)) {
        return (-1);
    }

    if (m->Xsize != 256 || m->Ysize != 1 || m->Bands != 1 || m->BandFmt != IM_BANDFMT_DOUBLE) {
        im_error("im_glds_entropy", "%s", _("wrong input"));
        return (-1);
    }
    tmpent = 0.0;
    in = (double *)m->data;
    for (i = 0; i < m->Xsize; i++) {
        if (*in != 0) {
            dtemp = *in;
            tmpent += (dtemp * log10(dtemp));
        }
        in++;
    }
    *entropy = ((-1) * tmpent / log10(2.0));
    return (0);
}

int im_glds_mean(IMAGE *m, double *mean) {
    double tmpmean, *in;
    int i;

    if (im_incheck(m)) {
        return (-1);
    }

    if (m->Xsize != 256 || m->Ysize != 1 || m->Bands != 1 || m->BandFmt != IM_BANDFMT_DOUBLE) {
        im_error("im_glds_mean", "%s", _("wrong input"));
        return (-1);
    }
    tmpmean = 0.0;
    in = (double *)m->data;
    for (i = 0; i < m->Xsize; i++) {
        tmpmean += (((double)i) * (*in));
        in++;
    }
    tmpmean = tmpmean / ((double)m->Xsize);
    *mean = tmpmean;
    return (0);
}
