
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *im;
    im_callback_fn fn;
    void *a, *b;
} VCallback;

static int add_callback(IMAGE *im, GSList **cblist, im_callback_fn fn, void *a, void *b) {
    VCallback *cbs;

    if (!(cbs = IM_NEW(NULL, VCallback))) {
        return (-1);
    }

    cbs->fn = fn;
    cbs->a = a;
    cbs->b = b;
    cbs->im = im;
    *cblist = g_slist_prepend(*cblist, cbs);

    return (0);
}

int im_add_close_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->closefns, fn, a, b));
}

int im_add_postclose_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->postclosefns, fn, a, b));
}

int im_add_preclose_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->preclosefns, fn, a, b));
}

int im_add_written_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->writtenfns, fn, a, b));
}

int im_add_eval_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {

    im->progress = im;

    return (add_callback(im, &im->evalfns, fn, a, b));
}

int im_add_evalend_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->evalendfns, fn, a, b));
}

int im_add_evalstart_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->evalstartfns, fn, a, b));
}

int im_add_invalidate_callback(IMAGE *im, im_callback_fn fn, void *a, void *b) {
    return (add_callback(im, &im->invalidatefns, fn, a, b));
}

static void *call_callback(VCallback *cbs, int *result) {
    int res;

    if ((res = cbs->fn(cbs->a, cbs->b))) {

        *result = res;

#ifdef DEBUG_IO
        printf("im__trigger_callbacks: user callback "
               "failed for %s\n",
            cbs->im->filename);
#endif
    }

    return (NULL);
}

int im__trigger_callbacks(GSList *cblist) {
    int result;

#ifdef DEBUG_IO
    printf("im__trigger_callbacks: calling %d user callbacks ..\n", g_slist_length(cblist));
#endif

    result = 0;
    (void)im_slist_map2(cblist, (VSListMap2Fn)call_callback, &result, NULL);

    return (result);
}
