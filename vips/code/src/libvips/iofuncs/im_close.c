
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <assert.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define NAMELEN 1024

int im__close(IMAGE *im) {
    int result;

    result = 0;

    if (!im) {
        return (result);
    }

#ifdef DEBUG_IO
    printf("im__close: starting for %s ..\n", im->filename);
#endif

    result |= im__trigger_callbacks(im->preclosefns);
    IM_FREEF(im_slist_free_all, im->preclosefns);

    if (im->regions) {
        GSList *p;

        printf("** im__close: leaked regions!\n");
        for (p = im->regions; p; p = p->next) {
            im_region_print((REGION *)p->data);
        }
    }

    if (im->windows) {
        GSList *p;

        printf("** im__close: leaked windows!\n");
        for (p = im->windows; p; p = p->next) {
            im_window_print((im_window_t *)p->data);
        }
    }

    im->start = NULL;
    im->generate = NULL;
    im->stop = NULL;

    im__link_break_all(im);

    if (im->baseaddr) {

#ifdef DEBUG_IO
        printf("im__close: unmapping file ..\n");
#endif

        if (im_unmapfile(im)) {
            return (-1);
        }
        im->data = NULL;
    }

    if (im->fd != -1) {
#ifdef DEBUG_IO
        printf("im__close: closing output file ..\n");
#endif

        if (im->dtype == IM_OPENOUT && im__writehist(im)) {
            result = -1;
        }
        if (close(im->fd) == -1) {
            im_error("im_close", _("unable to close fd for %s"), im->filename);
            result = -1;
        }
        im->fd = -1;
    }

    if (im->data) {

        if (im->dtype == IM_SETBUF) {
#ifdef DEBUG_IO
            printf("im__close: freeing buffer ..\n");
#endif
            im_free(im->data);
            im->dtype = IM_NONE;
        }

        im->data = NULL;
    }

    IM_FREEF(im_slist_free_all, im->evalstartfns);
    IM_FREEF(im_slist_free_all, im->evalfns);
    IM_FREEF(im_slist_free_all, im->evalendfns);
    IM_FREEF(im_slist_free_all, im->invalidatefns);
    IM_FREEF(im_slist_free_all, im->writtenfns);
    result |= im__trigger_callbacks(im->closefns);
    IM_FREEF(im_slist_free_all, im->closefns);

    im->dtype = IM_NONE;
    im->dhint = IM_SMALLTILE;
    im->kill = 0;
    im->close_pending = 0;
    im->sizeof_header = IM_SIZEOF_HEADER;

#ifdef DEBUG_IO
    printf("im__close: final success for %s (%p)\n", im->filename, im);
#endif

    return (result);
}

int im_close(IMAGE *im) {
    int result = 0;

    if (!im) {
        return (result);
    }

    if (im->regions) {

#ifdef DEBUG_NEW
        printf("im_close: pending close for 0x%p, \"%s\"\n", im, im->filename);
#endif

        im->close_pending = 1;
    } else if (!im->closing) {

        im->closing = 1;

        if (im__close(im)) {
            result = -1;
        }

#ifdef DEBUG_NEW
        printf("im_close: freeing IMAGE 0x%p, \"%s\"\n", im, im->filename);
#endif

        result |= im__trigger_callbacks(im->postclosefns);
        IM_FREEF(im_slist_free_all, im->postclosefns);
        IM_FREEF(g_mutex_free, im->sslock);
        IM_FREE(im->filename);
        IM_FREE(im->Hist);
        IM_FREEF(im__gslist_gvalue_free, im->history_list);
        im__meta_destroy(im);
        g_mutex_lock(im__global_lock);
        im__open_images = g_slist_remove(im__open_images, im);
        g_mutex_unlock(im__global_lock);
        im__time_destroy(im);
        IM_FREE(im);
    }

    return (result);
}
