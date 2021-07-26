
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int *im_offsets45(int size) {
    int temp;
    int x, y;
    int size2 = size * size;
    int size_2 = size / 2;
    int *pnt, *cpnt1, *cpnt2;

    if (size % 2 == 0) {
        im_error("im_offsets45", "%s", _("size not odd"));
        return (NULL);
    }
    if (!(pnt = IM_ARRAY(NULL, size2, int))) {
        return (NULL);
    }

    cpnt1 = pnt;
    cpnt2 = pnt + size2 - 1;

    for (y = 0; y < size_2; y++) {
        temp = (size_2 + y) * size;
        *cpnt1++ = temp;
        *cpnt2-- = size2 - 1 - temp;

        for (x = 0; x < y; x++) {
            temp -= (size - 1);
            *cpnt1++ = temp;
            *cpnt2-- = size2 - 1 - temp;
        }

        for (x = 0; x < size_2 - y; x++) {
            temp -= size;
            *cpnt1++ = temp;
            *cpnt2-- = size2 - 1 - temp;
        }

        for (x = 0; x < size_2 - y; x++) {
            temp++;
            *cpnt1++ = temp;
            *cpnt2-- = size2 - 1 - temp;
        }

        for (x = 0; x < y; x++) {
            temp -= (size - 1);
            *cpnt1++ = temp;
            *cpnt2-- = size2 - 1 - temp;
        }
    }

    temp = size * (size - 1);
    cpnt1 = pnt + size_2 * size;
    for (x = 0; x < size; x++) {
        *cpnt1++ = temp;
        temp -= (size - 1);
    }

#ifdef PIM_RINT
    temp = 0;
    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            fprintf(stderr, "%4d", *(pnt + temp));
            temp++;
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
#endif

    return (pnt);
}

int *im_offsets90(int size) {
    int temp;
    int x, y, k;
    int *offsets;

    if (!(offsets = IM_ARRAY(NULL, size * size, int))) {
        return (NULL);
    }

    for (k = 0, y = 0; y < size; y++) {
        temp = size * (size - 1) + y;

        for (x = 0; x < size; x++, k++) {
            offsets[k] = temp;
            temp -= size;
        }
    }

    return (offsets);
}

typedef int *(*offset_fn)(int);

static DOUBLEMASK *rotdmask(offset_fn fn, DOUBLEMASK *m, const char *name) {
    DOUBLEMASK *out;
    int size = m->xsize * m->ysize;
    int *offsets;
    int i;

    if (m->xsize != m->ysize || (m->xsize % 2) == 0) {
        im_error("im_rotate_mask", "%s", _("mask should be square of even size"));
        return (NULL);
    }
    if (!(offsets = fn(m->xsize))) {
        return (NULL);
    }
    if (!(out = im_create_dmask(name, m->xsize, m->ysize))) {
        im_free(offsets);
        return (NULL);
    }
    out->scale = m->scale;
    out->offset = m->offset;

    for (i = 0; i < size; i++) {
        out->coeff[i] = m->coeff[offsets[i]];
    }

    im_free(offsets);

    return (out);
}

static INTMASK *rotimask(offset_fn fn, INTMASK *m, const char *name) {
    INTMASK *out;
    int size = m->xsize * m->ysize;
    int *offsets;
    int i;

    if (m->xsize != m->ysize || (m->xsize % 2) == 0) {
        im_error("im_rotate_mask", "%s", _("mask should be square of even size"));
        return (NULL);
    }
    if (!(offsets = fn(m->xsize))) {
        return (NULL);
    }
    if (!(out = im_create_imask(name, m->xsize, m->ysize))) {
        im_free(offsets);
        return (NULL);
    }
    out->scale = m->scale;
    out->offset = m->offset;

    for (i = 0; i < size; i++) {
        out->coeff[i] = m->coeff[offsets[i]];
    }

    im_free(offsets);

    return (out);
}

DOUBLEMASK *im_rotate_dmask90(DOUBLEMASK *m, const char *name) {
    return (rotdmask(im_offsets90, m, name));
}

DOUBLEMASK *im_rotate_dmask45(DOUBLEMASK *m, const char *name) {
    return (rotdmask(im_offsets45, m, name));
}

INTMASK *im_rotate_imask90(INTMASK *m, const char *name) {
    return (rotimask(im_offsets90, m, name));
}

INTMASK *im_rotate_imask45(INTMASK *m, const char *name) {
    return (rotimask(im_offsets45, m, name));
}
