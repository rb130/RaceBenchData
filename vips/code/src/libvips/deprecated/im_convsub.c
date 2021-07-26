
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

static int im__create_int_luts(int *buffer, int buffersize, int **orig_luts, int **luts, int *cnt) {
    int *pbuffer;
    int *buf1, *buf2, *pbuf1, *pbuf2;
    int i, j;
    int min, max;
    int mark;
    int counter;

    buf1 = (int *)calloc((unsigned)buffersize, sizeof(int));
    buf2 = (int *)calloc((unsigned)buffersize, sizeof(int));
    if ((buf1 == NULL) || (buf2 == NULL)) {
        im_error("im_create_int_luts", "%s", _("calloc failed (1)"));
        return (-1);
    }

    pbuffer = buffer;
    pbuf1 = buf1;

    max = *pbuffer;
    for (i = 0; i < buffersize; i++) {
        if (*pbuffer > max) {
            max = *pbuffer;
        }
        *pbuf1++ = *pbuffer++;
    }
    mark = max + 1;
    pbuf1 = buf1;
    pbuf2 = buf2;
    counter = 0;

    for (i = 0; i < buffersize; i++) {
        min = mark + 1;
        pbuf1 = buf1;

        for (j = 0; j < buffersize; j++) {
            if (*pbuf1 < min) {
                min = *pbuf1;
            }
            pbuf1++;
        }
        if (min == mark) {
            break;
        }
        *pbuf2++ = min;
        counter++;
        pbuf1 = buf1;
        for (j = 0; j < buffersize; j++) {
            if (*pbuf1 == min) {
                *pbuf1 = mark;
            }
            pbuf1++;
        }
    }

    pbuf2 = buf2;
    for (i = 0; i < counter; i++) {
        orig_luts[i] = (int *)calloc((unsigned)256, sizeof(int));
        if (orig_luts[i] == NULL) {
            im_error("im_create_int_luts", "%s", _("calloc failed (2)"));
            return (-1);
        }
        for (j = 0; j < 256; j++) {
            *(orig_luts[i] + j) = j * (*pbuf2);
        }
        pbuf2++;
    }

    pbuffer = buffer;
    for (i = 0; i < buffersize; i++) {
        j = 0;
        while (1) {
            if (*(buf2 + j) == *pbuffer) {
                luts[i] = orig_luts[j];
                break;
            }
            j++;
        }
        pbuffer++;
    }

    free((char *)buf1);
    free((char *)buf2);
    *cnt = counter;
    return (0);
}

int im_convsub(in, out, m, xskip, yskip) IMAGE *in, *out;
INTMASK *m;
int xskip, yskip;
{

    int x;
    int y;
    int n_clipped = 0;
    int p_clipped = 0;
    int i, b;
    PEL **pnts, **cpnt1s, **cpnt2s;
    PEL **pnt, **cpnt1, **cpnt2;
    PEL *input, *line, *cpline;
    int *pm;
    int count;
    int *newm, *pnewm;
    int os;
    int ms;
    int **lut_orig, **lut;
    int lutcnt = 0;
    int rounding, sum;
    int tempsize;

    if ((xskip < 1) || (yskip < 1)) {
        im_error("im_convsub", "%s", _("xskip and yskip must be >= 1"));
        return (-1);
    }
    if (im_iocheck(in, out) == -1) {
        return (-1);
    }

    if ((in->Coding != IM_CODING_NONE) || (in->BandFmt != IM_BANDFMT_UCHAR)) {
        im_error("im_convsub", "%s", _("nput should be unsigned char uncoded"));
        return (-1);
    }

    if (im_cp_desc(out, in) == -1) {
        return (-1);
    }
    tempsize = in->Xsize / xskip;
    while (1) {
        if (tempsize * xskip + m->xsize < in->Xsize) {
            break;
        } else {
            tempsize--;
        }
        if (tempsize < 0) {
            break;
        }
    }
    out->Xsize = tempsize;
    tempsize = in->Ysize / yskip;
    while (1) {
        if (tempsize * yskip + m->ysize < in->Ysize) {
            break;
        } else {
            tempsize--;
        }
        if (tempsize < 0) {
            break;
        }
    }
    out->Ysize = tempsize;
    if ((out->Xsize < 2) || (out->Ysize < 2)) {
        im_error("im_convsub", "%s", _("too small output sizes"));
        return (-1);
    }

    if (im_setupout(out) == -1) {
        return (-1);
    }

    os = out->Xsize * out->Bands;
    if ((line = (PEL *)calloc((unsigned)os, sizeof(char))) == NULL) {
        im_error("im_convsub", "%s", _("unable to calloc(1)"));
        return (-1);
    }

    ms = m->xsize * m->ysize;
    count = 0;
    pm = m->coeff;
    for (i = 0; i < ms; i++) {
        if (*pm++ != 0) {
            count++;
        }
    }

    if (((newm = (int *)calloc((unsigned)count, sizeof(int))) == NULL) || ((pnts = (PEL **)calloc((unsigned)count, sizeof(char *))) == NULL) || ((cpnt1s = (PEL **)calloc((unsigned)count, sizeof(char *))) == NULL) || ((cpnt2s = (PEL **)calloc((unsigned)count, sizeof(char *))) == NULL)) {
        im_error("im_convsub", "%s", _("unable to calloc(2)"));
        return (-1);
    }

    pnt = pnts;
    cpnt1 = cpnt1s;
    cpnt2 = cpnt2s;

    i = 0;
    input = (PEL *)in->data;
    pm = m->coeff;
    pnewm = newm;
    for (y = 0; y < m->ysize; y++) {
        for (x = 0; x < m->xsize; x++) {
            if (*pm != 0) {
                *pnewm++ = *pm;
                pnt[i] = (input + (x + y * in->Xsize) * in->Bands);
                i++;
            }
            pm++;
        }
    }

    if (i != count) {
        im_error("im_convsub", "%s", _("impossible state"));
        return (-1);
    }

    lut_orig = (int **)calloc((unsigned)count, sizeof(int **));
    lut = (int **)calloc((unsigned)count, sizeof(int **));
    if ((lut == NULL) || (lut_orig == NULL)) {
        im_error("im_conv", "%s", _("unable to calloc(1)"));
        return (-1);
    }

    if (im__create_int_luts(newm, count, lut_orig, lut, &lutcnt) == -1) {
        im_error("im_convsub", "%s", _("im_create_int_luts failed"));
        return (-1);
    }

    rounding = m->scale / 2;

    for (y = 0; y < out->Ysize; y++) {
        cpline = line;
        for (i = 0; i < count; i++) {
            cpnt1[i] = pnt[i];

            pnt[i] += (in->Xsize * in->Bands * yskip);
        }

        for (x = 0; x < out->Xsize; x++) {
            for (i = 0; i < count; i++) {
                cpnt2[i] = cpnt1[i];
                cpnt1[i] += xskip * in->Bands;
            }
            for (b = 0; b < out->Bands; b++) {
                sum = 0;
                for (i = 0; i < count; i++) {
                    sum += *(lut[i] + (*cpnt2[i]));
                    cpnt2[i]++;
                }
                sum = ((sum + rounding) / m->scale) + m->offset;

                if (sum < (int)0) {
                    n_clipped++;
                    sum = (int)0;
                } else if (sum > (int)255) {
                    p_clipped++;
                    sum = (int)255;
                }
                *cpline++ = (unsigned char)sum;
            }
        }

        if (im_writeline(y, out, (PEL *)line) == -1) {
            free((char *)line);
            free((char *)newm);
            free((char *)pnts);
            free((char *)cpnt1s);
            free((char *)cpnt2s);
            for (i = 0; i < lutcnt; i++) {
                free((char *)lut_orig[i]);
            }
            free((char *)lut_orig);
            free((char *)lut);
            return (-1);
        }
    }

    if (n_clipped || p_clipped) {
        fprintf(stderr, "im_convsub: %d pels over 255 and %d under 0 clipped\n", p_clipped, n_clipped);
    }

    free((char *)line);
    free((char *)newm);
    free((char *)pnts);
    free((char *)cpnt1s);
    free((char *)cpnt2s);
    for (i = 0; i < lutcnt; i++) {
        free((char *)lut_orig[i]);
    }
    free((char *)lut_orig);
    free((char *)lut);

    return (0);
}
