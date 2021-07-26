
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_cntlines(IMAGE *im, double *nolines, int flag) {
    int x, y;
    PEL *line;
    int cnt;

    if (im_incheck(im)) {
        return (-1);
    }
    if (im->Coding != IM_CODING_NONE || im->BandFmt != IM_BANDFMT_UCHAR || im->Bands != 1) {
        im_error("im_cntlines", "%s", _("1-band uchar uncoded only"));
        return (-1);
    }
    if (flag != 0 && flag != 1) {
        im_error("im_cntlines", "%s", _("flag should be 0 (horizontal) or 1 (vertical)"));
        return (-1);
    }

    line = (PEL *)im->data;
    if (flag == 1) {

        for (cnt = 0, y = 0; y < im->Ysize; y++) {
            PEL *p = line;

            for (x = 0; x < im->Xsize - 1; x++) {
                if (p[0] < (PEL)128 && p[1] >= (PEL)128) {
                    cnt++;
                } else if (p[0] >= (PEL)128 && p[1] < (PEL)128) {
                    cnt++;
                }

                p++;
            }

            line += im->Xsize;
        }

        *nolines = (float)cnt / (2.0 * im->Ysize);
    } else {

        for (cnt = 0, y = 0; y < im->Ysize - 1; y++) {
            PEL *p1 = line;
            PEL *p2 = line + im->Xsize;

            for (x = 0; x < im->Xsize; x++) {
                if (*p1 < (PEL)128 && *p2 >= (PEL)128) {
                    cnt++;
                } else if (*p1 >= (PEL)128 && *p2 < (PEL)128) {
                    cnt++;
                }

                p1++;
                p2++;
            }

            line += im->Xsize;
        }

        *nolines = (float)cnt / (2.0 * im->Xsize);
    }

    return (0);
}
