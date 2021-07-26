#ifdef __cplusplus
extern "C"
#endif
void racebench_bug68(int);

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/thread.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef DEBUG
#warning DEBUG on in libsrc/iofuncs/memory.c
#endif

#ifdef DEBUGM
static size_t int total_mem_alloc = 0;
static unsigned int total_allocs = 0;
static size_t int high_water_mark = 0;
static GMutex *malloc_mutex = NULL;
static GSList *malloc_list = NULL;
static const int trace_freq = 100;
static int next_trace = 0;
#endif

int im_free(void *s) {
#ifdef DEBUGM
    {
        size_t size;

        s = (void *)((char *)s - 16);
        size = *((size_t *)s);
        g_mutex_lock(malloc_mutex);

        assert(g_slist_find(malloc_list, s));
        malloc_list = g_slist_remove(malloc_list, s);
        assert(!g_slist_find(malloc_list, s));
        malloc_list = g_slist_remove(malloc_list, s);
        assert(total_allocs > 0);

        total_mem_alloc -= size;
        total_allocs -= 1;

        next_trace += 1;
        if (next_trace > trace_freq) {
            printf("im_free: %d, %d allocs, total %.3gM, "
                   "high water %.3gM\n",
                size, total_allocs, total_mem_alloc / (1024.0 * 1024.0), high_water_mark / (1024.0 * 1024.0));
            next_trace = 0;
        }

        g_mutex_unlock(malloc_mutex);
    }
#endif

#ifdef DEBUG
    if (!s)
        g_assert(0);
#endif

    g_free(s); racebench_bug68(1);

    return (0);
}

void *im_malloc(IMAGE *im, size_t size) {
    void *buf;

#ifdef DEBUGM

    if (!malloc_mutex)
        malloc_mutex = g_mutex_new();
#endif

#ifdef DEBUGM

    size += 16;
#endif

    if (!(buf = g_try_malloc(size))) {
#ifdef DEBUG
        g_assert(0);
#endif

        im_error("im_malloc", _("out of memory --- size == %dMB"), (int)(size / (1024.0 * 1024.0)));
        im_warn("im_malloc", _("out of memory --- size == %dMB"), (int)(size / (1024.0 * 1024.0)));
        return (NULL);
    }

#ifdef DEBUGM

    g_mutex_lock(malloc_mutex);
    assert(!g_slist_find(malloc_list, buf));
    malloc_list = g_slist_prepend(malloc_list, buf);
    *((size_t *)buf) = size;
    buf = (void *)((char *)buf + 16);
    total_mem_alloc += size;
    if (total_mem_alloc > high_water_mark)
        high_water_mark = total_mem_alloc;
    total_allocs += 1;

    next_trace += 1;
    if (next_trace > trace_freq) {
        printf("im_malloc: %d, %d allocs, total %.3gM, "
               "high water %.3gM\n",
            size, total_allocs, total_mem_alloc / (1024.0 * 1024.0), high_water_mark / (1024.0 * 1024.0));
        next_trace = 0;
    }

    g_mutex_unlock(malloc_mutex);

    if (size > 1000000)
        printf("woah! big!\n");
#endif

    if (im && im_add_close_callback(im, (im_callback_fn)im_free, buf, NULL)) {
        im_free(buf);
        return (NULL);
    }

    return (buf);
}