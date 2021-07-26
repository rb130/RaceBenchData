
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

int im_smudge(IMAGE *im, int ix, int iy, Rect *r) {
    int x, y, a, b, c;
    int ba = im->Bands;
    int el = ba * im->Xsize;
    Rect area, image, clipped;
    double total[256];

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
    im_rect_marginadjust(&image, -1);
    im_rect_intersectrect(&area, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        return (0);
    }

#define SMUDGE(TYPE)                                                    \
    for (y = clipped.top; y < clipped.top + clipped.height; y++)        \
        for (x = clipped.left; x < clipped.left + clipped.width; x++) { \
            TYPE *to = (TYPE *)im->data + x * ba + y * el;              \
            TYPE *from = to - el - ba;                                  \
            TYPE *f;                                                    \
                                                                        \
            for (a = 0; a < ba; a++)                                    \
                total[a] = 0.0;                                         \
                                                                        \
            for (a = 0; a < 3; a++) {                                   \
                f = from;                                               \
                for (b = 0; b < 3; b++)                                 \
                    for (c = 0; c < ba; c++)                            \
                        total[c] += *f++;                               \
                from += el;                                             \
            }                                                           \
                                                                        \
            for (a = 0; a < ba; a++)                                    \
                to[a] = (16 * (double)to[a] + total[a]) / 25.0;         \
        }

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        SMUDGE(unsigned char);
        break;

    case IM_BANDFMT_CHAR:
        SMUDGE(char);
        break;

    case IM_BANDFMT_USHORT:
        SMUDGE(unsigned short);
        break;

    case IM_BANDFMT_SHORT:
        SMUDGE(short);
        break;

    case IM_BANDFMT_UINT:
        SMUDGE(unsigned int);
        break;

    case IM_BANDFMT_INT:
        SMUDGE(int);
        break;

    case IM_BANDFMT_FLOAT:
        SMUDGE(float);
        break;

    case IM_BANDFMT_DOUBLE:
        SMUDGE(double);
        break;

    case IM_BANDFMT_COMPLEX:

        ba *= 2;
        el *= 2;

        SMUDGE(float);

        break;

    case IM_BANDFMT_DPCOMPLEX:

        ba *= 2;
        el *= 2;

        SMUDGE(double);

        break;

    default:
        im_error("im_smudge", "%s", _("unknown band format"));
        return (-1);
    }

    return (0);
}

int im_smear(IMAGE *im, int ix, int iy, Rect *r) {
    int x, y, a, b, c;
    int ba = im->Bands;
    int el = ba * im->Xsize;
    Rect area, image, clipped;
    double total[256];

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
    im_rect_marginadjust(&image, -1);
    image.left--;
    im_rect_intersectrect(&area, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        return (0);
    }

#define SMEAR(TYPE)                                                     \
    for (y = clipped.top; y < clipped.top + clipped.height; y++)        \
        for (x = clipped.left; x < clipped.left + clipped.width; x++) { \
            TYPE *to = (TYPE *)im->data + x * ba + y * el;              \
            TYPE *from = to - el;                                       \
            TYPE *f;                                                    \
                                                                        \
            for (a = 0; a < ba; a++)                                    \
                total[a] = 0.0;                                         \
                                                                        \
            for (a = 0; a < 3; a++) {                                   \
                f = from;                                               \
                for (b = 0; b < 3; b++)                                 \
                    for (c = 0; c < ba; c++)                            \
                        total[c] += *f++;                               \
                from += el;                                             \
            }                                                           \
                                                                        \
            for (a = 0; a < ba; a++)                                    \
                to[a] = (40 * (double)to[a + ba] + total[a]) / 49.0;    \
        }

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        SMEAR(unsigned char);
        break;

    case IM_BANDFMT_CHAR:
        SMEAR(char);
        break;

    case IM_BANDFMT_USHORT:
        SMEAR(unsigned short);
        break;

    case IM_BANDFMT_SHORT:
        SMEAR(short);
        break;

    case IM_BANDFMT_UINT:
        SMEAR(unsigned int);
        break;

    case IM_BANDFMT_INT:
        SMEAR(int);
        break;

    case IM_BANDFMT_FLOAT:
        SMEAR(float);
        break;

    case IM_BANDFMT_DOUBLE:
        SMEAR(double);
        break;

    case IM_BANDFMT_COMPLEX:

        ba *= 2;
        el *= 2;

        SMEAR(float);

        break;

    case IM_BANDFMT_DPCOMPLEX:

        ba *= 2;
        el *= 2;

        SMEAR(double);

        break;

    default:
        im_error("im_smear", "%s", _("unknown band format"));
        return (-1);
    }

    return (0);
}
