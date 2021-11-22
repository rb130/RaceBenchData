

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef DEBUG

static GSList *im__buffers_all = NULL;
#endif

#ifdef DEBUG_CREATE
static int buffer_cache_n = 0;
#endif

#ifdef HAVE_THREADS
static GPrivate *thread_buffer_cache_key = NULL;
#else
static im_buffer_cache_t *thread_buffer_cache = NULL;
#endif

#ifdef HAVE_THREADS
static void buffer_cache_free(im_buffer_cache_t *cache) {
#ifdef DEBUG_CREATE
    buffer_cache_n -= 1;

    printf("buffer_cache_free: freeing cache %p on thread %p\n", cache, g_thread_self());
    printf("\t(%d cachees left)\n", buffer_cache_n);
#endif

    IM_FREEF(g_hash_table_destroy, cache->hash);
    IM_FREE(cache);
}
#endif

static void buffer_cache_list_free(im_buffer_cache_list_t *cache_list) {
    GSList *p;

    for (p = cache_list->buffers; p; p = p->next) {
        im_buffer_t *buffer = (im_buffer_t *)p->data;

        buffer->done = FALSE;
    }

    g_slist_free(cache_list->buffers);
    im_free(cache_list);
}

static im_buffer_cache_list_t *buffer_cache_list_new(im_buffer_cache_t *cache, IMAGE *im) {
    im_buffer_cache_list_t *cache_list;

    if (!(cache_list = IM_NEW(NULL, im_buffer_cache_list_t))) {
        return (NULL);
    }
    cache_list->buffers = NULL;
    cache_list->thread = g_thread_self();
    cache_list->cache = cache;
    cache_list->im = im;

#ifdef DEBUG_CREATE
    printf("buffer_cache_list_new: new cache %p for thread %p\n", cache, g_thread_self());
    printf("\t(%d cachees now)\n", buffer_cache_n);
#endif

    return (cache_list);
}

static im_buffer_cache_t *buffer_cache_new(void) {
    im_buffer_cache_t *cache;

    if (!(cache = IM_NEW(NULL, im_buffer_cache_t))) {
        return (NULL);
    }

    cache->hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)buffer_cache_list_free);
    cache->thread = g_thread_self();

#ifdef DEBUG_CREATE
    buffer_cache_n += 1;

    printf("buffer_cache_new: new cache %p for thread %p\n", cache, g_thread_self());
    printf("\t(%d cachees now)\n", buffer_cache_n);
#endif

    return (cache);
}

static im_buffer_cache_t *buffer_cache_get(void) {
    im_buffer_cache_t *cache;

#ifdef HAVE_THREADS
    if (!(cache = g_private_get(thread_buffer_cache_key))) {
        cache = buffer_cache_new();
        g_private_set(thread_buffer_cache_key, cache);
    }
#else
    if (!thread_buffer_cache)
        thread_buffer_cache = buffer_cache_new();
    cache = thread_buffer_cache;
#endif

    return (cache);
}

void im_buffer_done(im_buffer_t *buffer) {
    if (!buffer->done) {
        IMAGE *im = buffer->im;
        im_buffer_cache_t *cache = buffer_cache_get();
        im_buffer_cache_list_t *cache_list;

#ifdef DEBUG
        printf("im_buffer_done: thread %p adding to cache %p\n", g_thread_self(), cache);
        im_buffer_print(buffer);
#endif

        if (!(cache_list = g_hash_table_lookup(cache->hash, im))) {
            cache_list = buffer_cache_list_new(cache, im);
            g_hash_table_insert(cache->hash, im, cache_list);
        }

        g_assert(!g_slist_find(cache_list->buffers, buffer));
        g_assert(cache_list->thread == cache->thread);

        cache_list->buffers = g_slist_prepend(cache_list->buffers, buffer);
        buffer->done = TRUE;
        buffer->cache = cache;
    }
}

void im_buffer_undone(im_buffer_t *buffer) {
    if (buffer->done) {
        IMAGE *im = buffer->im;
        im_buffer_cache_t *cache = buffer->cache;
        im_buffer_cache_list_t *cache_list;

#ifdef DEBUG
        printf("im_buffer_undone: thread %p removing "
               "buffer %p from cache %p\n",
            g_thread_self(), buffer, cache);
#endif

        g_assert(cache->thread == g_thread_self());

        cache_list = g_hash_table_lookup(cache->hash, im);

        g_assert(cache_list);
        g_assert(cache_list->thread == cache->thread);
        g_assert(g_slist_find(cache_list->buffers, buffer));

        cache_list->buffers = g_slist_remove(cache_list->buffers, buffer);
        buffer->done = FALSE;
        buffer->cache = NULL;

#ifdef DEBUG
        printf("im_buffer_undone: %d buffers left\n", g_slist_length(cache_list->buffers));
#endif
    }
}

void im_buffer_unref(im_buffer_t *buffer) {
#ifdef DEBUG
    printf("** im_buffer_unref: left = %d, top = %d, "
           "width = %d, height = %d (%p)\n",
        buffer->area.left, buffer->area.top, buffer->area.width, buffer->area.height, buffer);
#endif

    g_assert(buffer->ref_count > 0);

    buffer->ref_count -= 1;

    if (buffer->ref_count == 0) {
#ifdef DEBUG
        if (!buffer->done)
            printf("im_buffer_unref: buffer was not done\n");
#endif

        im_buffer_undone(buffer);

        buffer->im = NULL;
        IM_FREE(buffer->buf);
        buffer->bsize = 0;
        im_free(buffer);

#ifdef DEBUG
        g_mutex_lock(im__global_lock);
        g_assert(g_slist_find(im__buffers_all, buffer));
        im__buffers_all = g_slist_remove(im__buffers_all, buffer);
        printf("%d buffers in vips\n", g_slist_length(im__buffers_all));
        g_mutex_unlock(im__global_lock);
#endif
    }
}

im_buffer_t *im_buffer_new(IMAGE *im, Rect *area) {
    im_buffer_t *buffer;

    if (!(buffer = IM_NEW(NULL, im_buffer_t))) {
        return (NULL);
    }

    buffer->ref_count = 1;
    buffer->im = im;
    buffer->area = *area;
    buffer->done = FALSE;
    buffer->cache = NULL;
    buffer->bsize = (size_t)IM_IMAGE_SIZEOF_PEL(im) * area->width * area->height;
    if (!(buffer->buf = im_malloc(NULL, buffer->bsize))) {
        im_buffer_unref(buffer);
        return (NULL);
    }

#ifdef DEBUG
    printf("** im_buffer_new: left = %d, top = %d, "
           "width = %d, height = %d (%p)\n",
        buffer->area.left, buffer->area.top, buffer->area.width, buffer->area.height, buffer);
#endif

#ifdef DEBUG
    g_mutex_lock(im__global_lock);
    im__buffers_all = g_slist_prepend(im__buffers_all, buffer);
    printf("%d buffers in vips\n", g_slist_length(im__buffers_all));
    g_mutex_unlock(im__global_lock);
#endif

    return (buffer);
}

static int buffer_move(im_buffer_t *buffer, Rect *area) {
    IMAGE *im = buffer->im;
    size_t new_bsize;

    g_assert(buffer->ref_count == 1);

    buffer->area = *area;
    im_buffer_undone(buffer);
    g_assert(!buffer->done);

    new_bsize = (size_t)IM_IMAGE_SIZEOF_PEL(im) * area->width * area->height;
    if (buffer->bsize < new_bsize) {
        buffer->bsize = new_bsize;
        IM_FREE(buffer->buf);
        if (!(buffer->buf = im_malloc(NULL, buffer->bsize))) {
            return (-1);
        }
    }

    return (0);
}

static im_buffer_t *buffer_find(IMAGE *im, Rect *r) {
    im_buffer_cache_t *cache = buffer_cache_get();
    im_buffer_cache_list_t *cache_list;
    im_buffer_t *buffer;
    GSList *p;
    Rect *area;

    cache_list = g_hash_table_lookup(cache->hash, im);
    p = cache_list ? cache_list->buffers : NULL;

    for (; p; p = p->next) {
        buffer = (im_buffer_t *)p->data;
        area = &buffer->area;

        if (area->left <= r->left && area->top <= r->top && area->left + area->width >= r->left + r->width && area->top + area->height >= r->top + r->height) {
            buffer->ref_count += 1;

#ifdef DEBUG
            printf("im_buffer_find: left = %d, top = %d, "
                   "width = %d, height = %d, count = %d (%p)\n",
                buffer->area.left, buffer->area.top, buffer->area.width, buffer->area.height, buffer->ref_count, buffer);
#endif

            break;
        }
    }

    if (p) {
        return (buffer);
    } else {
        return (NULL);
    }
}

im_buffer_t *im_buffer_ref(IMAGE *im, Rect *area) {
    im_buffer_t *buffer;

    if (!(buffer = buffer_find(im, area))) {

        if (!(buffer = im_buffer_new(im, area))) {
            return (NULL);
        }
    }

    return (buffer);
}

im_buffer_t *im_buffer_unref_ref(im_buffer_t *old_buffer, IMAGE *im, Rect *area) {
    im_buffer_t *buffer;

    g_assert(!old_buffer || old_buffer->im == im);

    if (old_buffer && im_rect_includesrect(&old_buffer->area, area)) {
        return (old_buffer);
    }

    if ((buffer = buffer_find(im, area))) {
        IM_FREEF(im_buffer_unref, old_buffer);
        return (buffer);
    }

    if (old_buffer && old_buffer->ref_count == 1) {
        if (buffer_move(old_buffer, area)) {
            im_buffer_unref(old_buffer);
            return (NULL);
        }

        return (old_buffer);
    }

    IM_FREEF(im_buffer_unref, old_buffer);
    if (!(buffer = im_buffer_new(im, area))) {
        return (NULL);
    }

    return (buffer);
}

void im_buffer_print(im_buffer_t *buffer) {
    printf("im_buffer_t: %p ref_count = %d, ", buffer, buffer->ref_count);
    printf("im = %p, ", buffer->im);
    printf("area.left = %d, ", buffer->area.left);
    printf("area.top = %d, ", buffer->area.top);
    printf("area.width = %d, ", buffer->area.width);
    printf("area.height = %d, ", buffer->area.height);
    printf("done = %d, ", buffer->done);
    printf("buf = %p, ", buffer->buf);
    printf("bsize = %zd\n", buffer->bsize);
}

void im__buffer_init(void) {
#ifdef HAVE_THREADS
    if (!thread_buffer_cache_key) {
        thread_buffer_cache_key = g_private_new((GDestroyNotify)buffer_cache_free);
    }
#endif
}
