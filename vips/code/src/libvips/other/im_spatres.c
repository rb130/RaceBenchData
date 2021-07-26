
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

int im_spatres(IMAGE *in, IMAGE *out, int step) {
    int x, y;
    int z;
    int i, j;
    int rounding, step2, sum;
    unsigned char *values;
    unsigned char *input, *cpinput, *cp2input, *line, *cpline, *pnt, *cpnt;
    int os;

    if (step < 1) {
        im_error("im_spatres", _("Invalid step %d"), step);
        return (-1);
    }

    if ((in->Xsize / step == 0) || (in->Ysize / step == 0)) {
        im_error("im_spatres", _("Invalid step %d"), step);
        return (-1);
    }

    if (im_iocheck(in, out) == -1) {
        return (-1);
    }

    if ((in->Coding != IM_CODING_NONE) || (in->BandFmt != IM_BANDFMT_UCHAR)) {
        im_error("im_spatres", "%s", _("wrong input"));
        return (-1);
    }

    if (im_cp_desc(out, in) == -1) {
        return (-1);
    }
    out->Xsize = in->Xsize - in->Xsize % step;
    out->Ysize = in->Ysize - in->Ysize % step;

    if (im_setupout(out) == -1) {
        return (-1);
    }

    os = in->Xsize * in->Bands;
    line = (unsigned char *)calloc((unsigned)os, sizeof(char));

    values = (unsigned char *)calloc((unsigned)out->Bands, sizeof(char));
    if (line == NULL || values == NULL) {
        im_error("im_spatres", "%s", _("calloc failed"));
        return (-1);
    }

    step2 = step * step;
    rounding = step2 / 2;
    input = (unsigned char *)in->data;
    for (y = 0; y < out->Ysize; y += step) {
        cpinput = input;
        input += os * step;

        cpline = line;
        for (x = 0; x < out->Xsize; x += step) {
            cp2input = cpinput;
            cpinput += step * out->Bands;
            for (z = 0; z < out->Bands; z++) {
                pnt = cp2input + z;
                sum = 0;
                for (j = 0; j < step; j++) {
                    cpnt = pnt;
                    pnt += os;
                    for (i = 0; i < step; i++) {
                        sum += (int)*cpnt;
                        cpnt += out->Bands;
                    }
                }
                *(values + z) = (PEL)((sum + rounding) / step2);
            }

            for (j = 0; j < step; j++) {
                for (z = 0; z < out->Bands; z++) {
                    *cpline++ = *(values + z);
                }
            }
        }

        for (j = 0; j < step; j++) {
            if (im_writeline(y + j, out, (PEL *)line) == -1) {
                free((char *)line);
                free((char *)values);
                return (-1);
            }
        }
    }

    free((char *)line);
    free((char *)values);
    return (0);
}
