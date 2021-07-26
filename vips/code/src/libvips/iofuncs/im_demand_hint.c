
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MAX_IMAGES (1000)

static void im__link_make(IMAGE *im_up, IMAGE *im_down) {
    g_assert(im_up);
    g_assert(im_down);

    im_up->downstream = g_slist_prepend(im_up->downstream, im_down);
    im_down->upstream = g_slist_prepend(im_down->upstream, im_up);

    if (im_up->progress && !im_down->progress) {
        im_down->progress = im_up->progress;
    }
}

static void *im__link_break(IMAGE *im_up, IMAGE *im_down) {
    g_assert(im_up);
    g_assert(im_down);
    g_assert(g_slist_find(im_up->downstream, im_down));
    g_assert(g_slist_find(im_down->upstream, im_up));

    im_up->downstream = g_slist_remove(im_up->downstream, im_down);
    im_down->upstream = g_slist_remove(im_down->upstream, im_up);

    if (im_down->progress && im_down->progress == im_up->progress) {
        im_down->progress = NULL;
    }

    return (NULL);
}

static void *im__link_break_rev(IMAGE *im_down, IMAGE *im_up) {
    return (im__link_break(im_up, im_down));
}

void im__link_break_all(IMAGE *im) {
    im_slist_map2(im->upstream, (VSListMap2Fn)im__link_break, im, NULL);
    im_slist_map2(im->downstream, (VSListMap2Fn)im__link_break_rev, im, NULL);

    g_assert(!im->upstream);
    g_assert(!im->downstream);
}

static void *im__link_mapp(IMAGE *im, VSListMap2Fn fn, int *serial, void *a, void *b) {
    void *res;

    if (im->serial == *serial) {
        return (NULL);
    }
    im->serial = *serial;

    if ((res = fn(im, a, b))) {
        return (res);
    }

    return (im_slist_map4(im->downstream, (VSListMap4Fn)im__link_mapp, fn, serial, a, b));
}

void *im__link_map(IMAGE *im, VSListMap2Fn fn, void *a, void *b) {
    static int serial = 0;

    serial += 1;
    return (im__link_mapp(im, fn, &serial, a, b));
}

static im_demand_type find_least(im_demand_type a, im_demand_type b) {
    return ((im_demand_type)IM_MIN((int)a, (int)b));
}

int im_demand_hint_array(IMAGE *im, VipsDemandStyle hint, IMAGE **in) {
    int i, len, nany;

    for (i = 0, len = 0, nany = 0; in[i]; i++, len++) {
        if (in[i]->dhint == IM_ANY) {
            nany++;
        }
    }

    if (len == 0) {

        ;
    } else if (nany == len) {

        hint = IM_ANY;
    } else {

        for (i = 0; i < len; i++) {
            hint = find_least(hint, in[i]->dhint);
        }
    }

    im->dhint = hint;

#ifdef DEBUG
    printf("im_demand_hint_array: set dhint for \"%s\" to %s\n", im->filename, im_dhint2char(im->dhint));
#endif

    for (i = 0; i < len; i++) {
        im__link_make(in[i], im);
    }

    im->hint_set = TRUE;

    return (0);
}

int im_demand_hint(IMAGE *im, VipsDemandStyle hint, ...) {
    va_list ap;
    int i;
    IMAGE *ar[MAX_IMAGES];

    va_start(ap, hint);
    for (i = 0; i < MAX_IMAGES && (ar[i] = va_arg(ap, IMAGE *)); i++) {
        ;
    }
    va_end(ap);
    if (i == MAX_IMAGES) {
        im_error("im_demand_hint", "%s", _("too many images"));
        return (-1);
    }

    return (im_demand_hint_array(im, hint, ar));
}
