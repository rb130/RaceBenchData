

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

#define SWAP(A, B) \
    {              \
        int t;     \
        t = (A);   \
        (A) = (B); \
        (B) = t;   \
    }

int im_fastline(IMAGE *im, int x1, int y1, int x2, int y2, PEL *pel) {
    int es = IM_IMAGE_SIZEOF_ELEMENT(im);
    int ps = es * im->Bands;
    int ls = ps * im->Xsize;
    PEL *p;

    int x, y, dx, dy;
    int err;
    int b;

    if (im_rwcheck(im)) {
        return (-1);
    }

    if (x1 > im->Xsize || x1 < 0 || y1 > im->Ysize || y1 < 0 || x2 > im->Xsize || x2 < 0 || y2 > im->Ysize || y2 < 0) {
        im_error("im_fastline", "%s", _("invalid line cooordinates"));
        return (-1);
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) >= abs(dy) && dx < 0) {

        SWAP(x1, x2);
        SWAP(y1, y2);
    } else if (abs(dx) < abs(dy) && dy < 0) {

        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    dx = x2 - x1;
    dy = y2 - y1;

    x = x1;
    y = y1;
    p = (PEL *)im->data + x * ps + y * ls;

#define PLOT                 \
    for (b = 0; b < ps; b++) \
        p[b] = pel[b];

    if (dx == 0 && dy == 0) {
        PLOT;
    }

    else if (dx == 0) {

        for (; y <= y2; y++) {
            PLOT;
            p += ls;
        }
    } else if (dy == 0) {

        for (; x <= x2; x++) {
            PLOT;
            p += ps;
        }
    }

    else if (abs(dy) == abs(dx) && dy > 0) {

        for (; x <= x2; x++) {
            PLOT;
            p += ps + ls;
        }
    } else if (abs(dy) == abs(dx) && dy < 0) {

        for (; x <= x2; x++) {
            PLOT;
            p += ps - ls;
        }
    } else if (abs(dy) < abs(dx) && dy > 0) {

        for (err = 0; x <= x2; x++) {
            PLOT;
            p += ps;
            err += dy;
            if (err >= dx) {
                err -= dx;
                p += ls;
            }
        }
    } else if (abs(dy) < abs(dx) && dy < 0) {

        for (err = 0; x <= x2; x++) {
            PLOT;
            p += ps;
            err -= dy;
            if (err >= dx) {
                err -= dx;
                p -= ls;
            }
        }
    } else if (abs(dy) > abs(dx) && dx > 0) {

        for (err = 0; y <= y2; y++) {
            PLOT;
            p += ls;
            err += dx;
            if (err >= dy) {
                err -= dy;
                p += ps;
            }
        }
    } else if (abs(dy) > abs(dx) && dx < 0) {

        for (err = 0; y <= y2; y++) {
            PLOT;
            p += ls;
            err -= dx;
            if (err >= dy) {
                err -= dy;
                p -= ps;
            }
        }
    } else {
        error_exit("internal error #9872659823475982375");
    }

    return (0);
}

int im_fastlineuser(IMAGE *im, int x1, int y1, int x2, int y2, int (*fn)(), void *client1, void *client2, void *client3) {
    int x, y, dx, dy;
    int err;

    if (im_rwcheck(im)) {
        return (-1);
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) >= abs(dy) && dx < 0) {

        SWAP(x1, x2);
        SWAP(y1, y2);
    } else if (abs(dx) < abs(dy) && dy < 0) {

        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    dx = x2 - x1;
    dy = y2 - y1;

    x = x1;
    y = y1;

    if (dx == 0 && dy == 0) {
        if (fn(im, x, y, client1, client2, client3)) {
            return (1);
        }
    }

    else if (dx == 0) {

        for (; y <= y2; y++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
        }
    } else if (dy == 0) {

        for (; x <= x2; x++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
        }
    }

    else if (abs(dy) == abs(dx) && dy > 0) {

        for (; x <= x2; x++, y++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
        }
    } else if (abs(dy) == abs(dx) && dy < 0) {

        for (; x <= x2; x++, y--) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
        }
    } else if (abs(dy) < abs(dx) && dy > 0) {

        for (err = 0; x <= x2; x++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
            err += dy;
            if (err >= dx) {
                err -= dx;
                y++;
            }
        }
    } else if (abs(dy) < abs(dx) && dy < 0) {

        for (err = 0; x <= x2; x++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
            err -= dy;
            if (err >= dx) {
                err -= dx;
                y--;
            }
        }
    } else if (abs(dy) > abs(dx) && dx > 0) {

        for (err = 0; y <= y2; y++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
            err += dx;
            if (err >= dy) {
                err -= dy;
                x++;
            }
        }
    } else if (abs(dy) > abs(dx) && dx < 0) {

        for (err = 0; y <= y2; y++) {
            if (fn(im, x, y, client1, client2, client3)) {
                return (1);
            }
            err -= dx;
            if (err >= dy) {
                err -= dy;
                x--;
            }
        }
    } else {
        error_exit("internal error #9872659823475982375");
    }

    return (0);
}

int im_lineset(IMAGE *in, IMAGE *out, IMAGE *mask, IMAGE *ink, int n, int *x1v, int *y1v, int *x2v, int *y2v) {
    Rect mask_rect;
    int i;

    if (mask->Bands != 1 || mask->BandFmt != IM_BANDFMT_UCHAR || mask->Coding != IM_CODING_NONE) {
        im_error("im_lineset", "%s", _("mask image not 1 band 8 bit uncoded"));
        return (-1);
    }
    if (ink->Bands != in->Bands || ink->BandFmt != in->BandFmt || ink->Coding != in->Coding) {
        im_error("im_lineset", "%s", _("ink image does not match in image"));
        return (-1);
    }
    if (ink->Xsize != 1 || ink->Ysize != 1) {
        im_error("im_lineset", "%s", _("ink image not 1x1 pixels"));
        return (-1);
    }

    if (im_copy(in, out)) {
        return (-1);
    }

    mask_rect.left = mask->Xsize / 2;
    mask_rect.top = mask->Ysize / 2;
    mask_rect.width = mask->Xsize;
    mask_rect.height = mask->Ysize;

    if (im_incheck(ink) || im_incheck(mask)) {
        return (-1);
    }

    for (i = 0; i < n; i++) {
        if (im_fastlineuser(out, x1v[i], y1v[i], x2v[i], y2v[i], im_plotmask, ink->data, mask->data, &mask_rect)) {
            return (-1);
        }
    }

    return (0);
}
