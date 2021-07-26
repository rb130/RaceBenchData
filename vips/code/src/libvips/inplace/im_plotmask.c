
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

#define IBLEND(TYPE, TO, INK, B, W)                                                      \
    {                                                                                    \
        TYPE *tto = (TYPE *)(TO);                                                        \
        TYPE *tink = (TYPE *)(INK);                                                      \
                                                                                         \
        int x, i, j;                                                                     \
                                                                                         \
        for (j = 0, x = 0; x < (W); x++)                                                 \
            for (i = 0; i < (B); i++, j++)                                               \
                tto[j] = (tink[i] * mask_line[x] + tto[j] * (255 - mask_line[x])) / 255; \
    }

#define DBLEND(TYPE, TO, INK, B, W)                                                                      \
    {                                                                                                    \
        TYPE *tto = (TYPE *)(TO);                                                                        \
        TYPE *tink = (TYPE *)(INK);                                                                      \
                                                                                                         \
        int x, i, j;                                                                                     \
                                                                                                         \
        for (j = 0, x = 0; x < (W); x++)                                                                 \
            for (i = 0; i < (B); i++, j++)                                                               \
                tto[j] = ((double)tink[i] * mask_line[x] + (double)tto[j] * (255 - mask_line[x])) / 255; \
    }

#define CBLEND(TYPE, TO, INK, B, W)                                                                                  \
    {                                                                                                                \
        TYPE *tto = (TYPE *)(TO);                                                                                    \
        TYPE *tink = (TYPE *)(INK);                                                                                  \
                                                                                                                     \
        int x, i, j;                                                                                                 \
                                                                                                                     \
        for (j = 0, x = 0; x < (W); x++)                                                                             \
            for (i = 0; i < (B)*2; i += 2, j += 2) {                                                                 \
                tto[j] = ((double)tink[i] * mask_line[x] + (double)tto[j] * (255 - mask_line[x])) / 255;             \
                tto[j + 1] = ((double)tink[i + 1] * mask_line[x] + (double)tto[j + 1] * (255 - mask_line[x])) / 255; \
            }                                                                                                        \
    }

int im_plotmask(IMAGE *im, int ix, int iy, PEL *ink, PEL *mask, Rect *r) {
    Rect area, image, clipped;
    int y;
    int mx, my;

    if (im_rwcheck(im)) {
        return (-1);
    }

    area = *r;
    area.left += ix;
    area.top += iy;
    image.left = 0;
    image.top = 0;
    image.width = im->Xsize;
    image.height = im->Ysize;
    im_rect_intersectrect(&area, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        return (0);
    }

    mx = IM_MAX(0, clipped.left - area.left);
    my = IM_MAX(0, clipped.top - area.top);

    if (im->Coding == IM_CODING_LABQ) {
        float *lab_buffer;
        float ink_buffer[3];

        if (!(lab_buffer = IM_ARRAY(NULL, clipped.width * 3, float))) {
            return (-1);
        }

        imb_LabQ2Lab(ink, ink_buffer, 1);

        for (y = 0; y < clipped.height; y++) {
            PEL *to = (PEL *)IM_IMAGE_ADDR(im, clipped.left, y + clipped.top);
            PEL *mask_line = mask + mx + (y + my) * area.width;

            imb_LabQ2Lab(to, lab_buffer, clipped.width);
            DBLEND(float, lab_buffer, ink_buffer, 3, clipped.width);
            imb_Lab2LabQ(lab_buffer, to, clipped.width);
        }

        im_free(lab_buffer);
    } else {
        for (y = 0; y < clipped.height; y++) {
            PEL *to = (PEL *)IM_IMAGE_ADDR(im, clipped.left, y + clipped.top);
            PEL *mask_line = mask + mx + (y + my) * area.width;

            switch (im->BandFmt) {
            case IM_BANDFMT_UCHAR:
                IBLEND(unsigned char, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_CHAR:
                IBLEND(signed char, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_USHORT:
                IBLEND(unsigned short, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_SHORT:
                IBLEND(signed short, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_UINT:
                DBLEND(unsigned int, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_INT:
                DBLEND(signed int, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_FLOAT:
                DBLEND(float, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_DOUBLE:
                DBLEND(double, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_COMPLEX:
                CBLEND(float, to, ink, im->Bands, clipped.width);
                break;

            case IM_BANDFMT_DPCOMPLEX:
                CBLEND(double, to, ink, im->Bands, clipped.width);
                break;

            default:
                im_error("im_plotmask", "%s", _("internal error"));
                return (-1);
            }
        }
    }

    return (0);
}
