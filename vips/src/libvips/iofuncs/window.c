

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__read_test;

int im__window_margin_pixels = IM__WINDOW_MARGIN_PIXELS;

int im__window_margin_bytes = IM__WINDOW_MARGIN_BYTES;

#ifdef DEBUG_TOTAL
static int total_mmap_usage = 0;
static int max_mmap_usage = 0;
#endif

static int im_window_unmap(im_window_t *window) {

    if (window->baseaddr) {
        if (im__munmap(window->baseaddr, window->length)) {
            return (-1);
        }

#ifdef DEBUG_TOTAL
        g_mutex_lock(im__global_lock);
        total_mmap_usage -= window->length;
        assert(total_mmap_usage >= 0);
        g_mutex_unlock(im__global_lock);
#endif

        window->data = NULL;
        window->baseaddr = NULL;
        window->length = 0;
    }

    return (0);
}

static int im_window_free(im_window_t *window) {
    assert(window->ref_count == 0);

#ifdef DEBUG
    printf("** im_window_free: window top = %d, height = %d (%p)\n", window->top, window->height, window);
#endif

    if (im_window_unmap(window)) {
        return (-1);
    }

    window->im = NULL;

    im_free(window);

    return (0);
}

int im_window_unref(im_window_t *window) {
    IMAGE *im = window->im;

    g_mutex_lock(im->sslock);

#ifdef DEBUG
    printf("im_window_unref: window top = %d, height = %d, count = %d\n", window->top, window->height, window->ref_count);
#endif

    assert(window->ref_count > 0);

    window->ref_count -= 1;

    if (window->ref_count == 0) {
        assert(g_slist_find(im->windows, window));
        im->windows = g_slist_remove(im->windows, window);

#ifdef DEBUG
        printf("im_window_unref: %d windows left\n", g_slist_length(im->windows));
#endif

        if (im_window_free(window)) {
            g_mutex_unlock(im->sslock);
            return (-1);
        }
    }

    g_mutex_unlock(im->sslock);

    return (0);
}

#ifdef DEBUG_TOTAL
static void trace_mmap_usage(void) {
    g_mutex_lock(im__global_lock);
    {
        static int last_total = 0;
        int total = total_mmap_usage / (1024 * 1024);
        int max = max_mmap_usage / (1024 * 1024);

        if (total != last_total) {
            printf("im_window_set: current mmap "
                   "usage of ~%dMB (high water mark %dMB)\n",
                total, max);
            last_total = total;
        }
    }
    g_mutex_unlock(im__global_lock);
}
#endif

static int im_getpagesize() {
    static int pagesize = 0;

    if (!pagesize) {
#ifdef OS_WIN32
        SYSTEM_INFO si;

        GetSystemInfo(&si);

        pagesize = si.dwAllocationGranularity;
#else
        pagesize = getpagesize();
#endif

#ifdef DEBUG_TOTAL
        printf("im_getpagesize: 0x%x\n", pagesize);
#endif
    }

    return (pagesize);
}

static int im_window_set(im_window_t *window, int top, int height) {
    int pagesize = im_getpagesize();

    void *baseaddr;
    gint64 start, end, pagestart;
    size_t length, pagelength;

    start = window->im->sizeof_header + (gint64)IM_IMAGE_SIZEOF_LINE(window->im) * top;
    length = (size_t)IM_IMAGE_SIZEOF_LINE(window->im) * height;

    pagestart = start - start % pagesize;
    end = start + length;
    pagelength = end - pagestart;

    if (end > window->im->file_length) {
        im_error("im_window_set", _("unable to read data for \"%s\", %s"), window->im->filename, _("file has been truncated"));
        return (-1);
    }

    if (!(baseaddr = im__mmap(window->im->fd, 0, pagelength, pagestart))) {
        return (-1);
    }

    window->baseaddr = baseaddr;
    window->length = pagelength;

    window->data = (char *)baseaddr + (start - pagestart);
    window->top = top;
    window->height = height;

    im__read_test &= window->data[0];

#ifdef DEBUG_TOTAL
    g_mutex_lock(im__global_lock);
    total_mmap_usage += window->length;
    if (total_mmap_usage > max_mmap_usage)
        max_mmap_usage = total_mmap_usage;
    g_mutex_unlock(im__global_lock);
    trace_mmap_usage();
#endif

    return (0);
}

static im_window_t *im_window_new(IMAGE *im, int top, int height) {
    im_window_t *window;

    if (!(window = IM_NEW(NULL, im_window_t))) {
        return (NULL);
    }

    window->ref_count = 0;
    window->im = im;
    window->top = 0;
    window->height = 0;
    window->data = NULL;
    window->baseaddr = NULL;
    window->length = 0;

    if (im_window_set(window, top, height)) {
        im_window_free(window);
        return (NULL);
    }

    im->windows = g_slist_prepend(im->windows, window);
    window->ref_count += 1;

#ifdef DEBUG
    printf("** im_window_new: window top = %d, height = %d (%p)\n", window->top, window->height, window);
#endif

    return (window);
}

typedef struct {
    int top;
    int height;
} request_t;

static void *im_window_fits(im_window_t *window, request_t *req) {
    if (window->top <= req->top && window->top + window->height >= req->top + req->height) {
        return (window);
    }

    return (NULL);
}

static im_window_t *im_window_find(IMAGE *im, int top, int height) {
    request_t req;
    im_window_t *window;

    req.top = top;
    req.height = height;
    window = im_slist_map2(im->windows, (VSListMap2Fn)im_window_fits, &req, NULL);

    if (window) {
        window->ref_count += 1;

#ifdef DEBUG
        printf("im_window_find: ref window top = %d, height = %d, "
               "count = %d\n",
            top, height, window->ref_count);
#endif
    }

    return (window);
}

im_window_t *im_window_ref(IMAGE *im, int top, int height) {
    im_window_t *window;

    g_mutex_lock(im->sslock);

    if (!(window = im_window_find(im, top, height))) {

        int margin = IM_MIN(im__window_margin_pixels, im__window_margin_bytes / IM_IMAGE_SIZEOF_LINE(im));

        top -= margin;
        height += margin * 2;

        top = IM_CLIP(0, top, im->Ysize - 1);
        height = IM_CLIP(0, height, im->Ysize - top);

        if (!(window = im_window_new(im, top, height))) {
            g_mutex_unlock(im->sslock);
            return (NULL);
        }
    }

    g_mutex_unlock(im->sslock);

    return (window);
}

void im_window_print(im_window_t *window) {
    printf("im_window_t: %p ref_count = %d, ", window, window->ref_count);
    printf("im = %p, ", window->im);
    printf("top = %d, ", window->top);
    printf("height = %d, ", window->height);
    printf("data = %p, ", window->data);
    printf("baseaddr = %p, ", window->baseaddr);
    printf("length = %zd\n", window->length);
}
