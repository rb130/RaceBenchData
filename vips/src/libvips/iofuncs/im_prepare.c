

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/debug.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int fill_region(REGION *reg) {
    IMAGE *im = reg->im;

    if (im__call_start(reg)) {
        return (-1);
    }

    if (im->generate(reg, reg->seq, im->client1, im->client2)) {
        return (-1);
    }

    return (0);
}

int im__test_kill(IMAGE *im) {

    if (im->kill) {
        im_error("im__test_kill", _("killed for image \"%s\""), im->filename);
        return (-1);
    }

    return (0);
}

int im_prepare(REGION *reg, Rect *r) {
    IMAGE *im = reg->im;

    Rect save = *r;

    im__region_check_ownership(reg);

    if (im__test_kill(im)) {
        return (-1);
    }

    {
        Rect image;

        image.left = 0;
        image.top = 0;
        image.width = reg->im->Xsize;
        image.height = reg->im->Ysize;
        im_rect_intersectrect(&save, &image, &save);
    }

#ifdef DEBUG
    printf("im_prepare: left = %d, top = %d, width = %d, height = %d\n", r->left, r->top, r->width, r->height);
#endif

    switch (im->dtype) {
    case IM_PARTIAL:
        if (im_region_fill(reg, r, (im_region_fill_fn)fill_region, NULL)) {
            return (-1);
        }

        break;

    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
    case IM_MMAPIN:
    case IM_MMAPINRW:
    case IM_OPENIN:

        if (im_region_image(reg, r)) {
            return (-1);
        }

        break;

    default:
        im_error("im_prepare", _("unable to input from a %s image"), im_dtype2char(im->dtype));
        return (-1);
    }

    g_assert(im_rect_includesrect(&reg->valid, &save));

    return (0);
}

static int im_prepare_to_generate(REGION *reg, REGION *dest, Rect *r, int x, int y) {
    IMAGE *im = reg->im;
    char *p;

    if (!im->generate) {
        im_error("im_prepare_to", "%s", _("incomplete header"));
        return (-1);
    }

    if (im_region_region(reg, dest, r, x, y)) {
        return (-1);
    }

    p = IM_REGION_ADDR(reg, reg->valid.left, reg->valid.top);

    if (fill_region(reg)) {
        return (-1);
    }

    if (IM_REGION_ADDR(reg, reg->valid.left, reg->valid.top) != p) {
        im_region_copy(reg, dest, r, x, y);
    }

    return (0);
}

int im_prepare_to(REGION *reg, REGION *dest, Rect *r, int x, int y) {
    IMAGE *im = reg->im;
    Rect image;
    Rect wanted;
    Rect clipped;
    Rect clipped2;
    Rect final;

    if (im__test_kill(im)) {
        return (-1);
    }

    if (!dest->data || dest->im->BandFmt != reg->im->BandFmt || dest->im->Bands != reg->im->Bands) {
        im_error("im_prepare_to", "%s", _("inappropriate region type"));
        return (-1);
    }

    image.top = 0;
    image.left = 0;
    image.width = reg->im->Xsize;
    image.height = reg->im->Ysize;
    im_rect_intersectrect(r, &image, &clipped);

    g_assert(clipped.left == r->left);
    g_assert(clipped.top == r->top);

    wanted.left = x + (clipped.left - r->left);
    wanted.top = y + (clipped.top - r->top);
    wanted.width = clipped.width;
    wanted.height = clipped.height;

    if (!im_rect_includesrect(&dest->valid, &wanted)) {
        im_error("im_prepare_to", "%s", _("dest too small"));
        return (-1);
    }

    im_rect_intersectrect(&wanted, &dest->valid, &clipped2);

    final.left = r->left + (clipped2.left - wanted.left);
    final.top = r->top + (clipped2.top - wanted.top);
    final.width = clipped2.width;
    final.height = clipped2.height;

    x = clipped2.left;
    y = clipped2.top;

    if (im_rect_isempty(&final)) {
        im_error("im_prepare_to", "%s", _("valid clipped to nothing"));
        return (-1);
    }

#ifdef DEBUG
    printf("im_prepare_to: left = %d, top = %d, width = %d, height = %d\n", final.left, final.top, final.width, final.height);
#endif

    switch (im->dtype) {
    case IM_OPENOUT:
    case IM_PARTIAL:

        if (im_prepare_to_generate(reg, dest, &final, x, y)) {
            return (-1);
        }

        break;

    case IM_MMAPIN:
    case IM_MMAPINRW:
    case IM_OPENIN:

        if (im_region_image(reg, &final)) {
            return (-1);
        }
        im_region_copy(reg, dest, &final, x, y);

        break;

    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:

        if (im->generate) {
            if (im_prepare_to_generate(reg, dest, &final, x, y)) {
                return (-1);
            }
        } else {
            if (im_region_image(reg, &final)) {
                return (-1);
            }
            im_region_copy(reg, dest, &final, x, y);
        }

        break;

    default:
        im_error("im_prepare_to",
            _("unable to input from a "
              "%s image"),
            im_dtype2char(im->dtype));
        return (-1);
    }

    dest->invalid = FALSE;

    return (0);
}

int im_prepare_many(REGION **reg, Rect *r) {
    for (; *reg; ++reg) {
        if (im_prepare(*reg, r)) {
            return (-1);
        }
    }

    return (0);
}

static void *im_invalidate_region(REGION *reg) {
    reg->invalid = TRUE;

    return (NULL);
}

static void *im_invalidate_image(IMAGE *im, GSList **to_be_invalidated) {
    g_mutex_lock(im->sslock);
    (void)im_slist_map2(im->regions, (VSListMap2Fn)im_invalidate_region, NULL, NULL);
    g_mutex_unlock(im->sslock);

    *to_be_invalidated = g_slist_prepend(*to_be_invalidated, im);

    return (NULL);
}

static void im_invalidate_trigger(GSList *images) {
    GSList *regions;
    GSList *p;

    regions = NULL;
    for (p = images; p; p = p->next) {
        IMAGE *im = (IMAGE *)p->data;

        regions = g_slist_prepend(regions, im_region_create(im));
    }

    for (p = images; p; p = p->next) {
        IMAGE *im = (IMAGE *)p->data;

        (void)im__trigger_callbacks(im->invalidatefns);
    }

    for (p = regions; p; p = p->next) {
        REGION *r = (REGION *)p->data;

        im_region_free(r);
    }

    g_slist_free(regions);
}

void im_invalidate(IMAGE *im) {
    GSList *to_be_invalidated;

    to_be_invalidated = NULL;
    (void)im__link_map(im, (VSListMap2Fn)im_invalidate_image, &to_be_invalidated, NULL);

    im_invalidate_trigger(to_be_invalidated);

    g_slist_free(to_be_invalidated);
}
