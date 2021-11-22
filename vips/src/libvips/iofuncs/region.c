

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

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>
#include <vips/debug.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef DEBUG

static GSList *im__regions_all = NULL;
#endif

int im__call_start(REGION *reg) {
    IMAGE *im = reg->im;

    if (!reg->seq && im->start) {
        g_mutex_lock(im->sslock);
        reg->seq = im->start(im, im->client1, im->client2);
        g_mutex_unlock(im->sslock);

        if (!reg->seq) {
            im_error("im__call_start", _("start function failed for image %s"), im->filename);
            return (-1);
        }
    }

    return (0);
}

void im__call_stop(REGION *reg) {
    IMAGE *im = reg->im;
    int res;

    if (reg->seq && im->stop) {
        g_mutex_lock(im->sslock);
        res = im->stop(reg->seq, im->client1, im->client2);
        g_mutex_unlock(im->sslock);

        if (res) {
            error_exit("panic: user stop callback failed "
                       "for image %s",
                im->filename);
        }

        reg->seq = NULL;
    }
}

void im__region_take_ownership(REGION *reg) {

    g_mutex_lock(reg->im->sslock);

    if (reg->thread != g_thread_self()) {
        g_assert(reg->thread == NULL);

        g_assert(!reg->buffer || reg->buffer->ref_count == 1);

        reg->thread = g_thread_self();
    }

    g_mutex_unlock(reg->im->sslock);
}

void im__region_check_ownership(REGION *reg) {
    if (reg->thread) {
        g_assert(reg->thread == g_thread_self());
        if (reg->buffer && reg->buffer->cache) {
            g_assert(reg->thread == reg->buffer->cache->thread);
        }
    }
}

void im__region_no_ownership(REGION *reg) {
    g_mutex_lock(reg->im->sslock);

    im__region_check_ownership(reg);

    reg->thread = NULL;
    if (reg->buffer) {
        im_buffer_undone(reg->buffer);
    }

    g_mutex_unlock(reg->im->sslock);
}

REGION *im_region_create(IMAGE *im) {
    REGION *reg;

    g_assert(!im_image_sanity(im));

    if (!(reg = IM_NEW(NULL, REGION))) {
        return (NULL);
    }

    reg->im = im;
    reg->valid.left = 0;
    reg->valid.top = 0;
    reg->valid.width = 0;
    reg->valid.height = 0;
    reg->type = IM_REGION_NONE;
    reg->data = NULL;
    reg->bpl = 0;
    reg->seq = NULL;
    reg->thread = NULL;
    reg->window = NULL;
    reg->buffer = NULL;
    reg->invalid = FALSE;

    im__region_take_ownership(reg);

    g_mutex_lock(im->sslock);
    im->regions = g_slist_prepend(im->regions, reg);
    g_mutex_unlock(im->sslock);

#ifdef DEBUG
    g_mutex_lock(im__global_lock);
    im__regions_all = g_slist_prepend(im__regions_all, reg);
    printf("%d regions in vips\n", g_slist_length(im__regions_all));
    g_mutex_unlock(im__global_lock);
#endif

    return (reg);
}

static void im_region_reset(REGION *reg) {
    IM_FREEF(im_window_unref, reg->window);
    IM_FREEF(im_buffer_unref, reg->buffer);
    reg->invalid = FALSE;
}

void im_region_free(REGION *reg) {
    IMAGE *im;

    if (!reg) {
        return;
    }
    im = reg->im;

    im__call_stop(reg);

    im_region_reset(reg);

    g_mutex_lock(im->sslock);
    im->regions = g_slist_remove(im->regions, reg);
    g_mutex_unlock(im->sslock);
    reg->im = NULL;

    if (!im->regions && im->close_pending) {
#ifdef DEBUG_IO
        printf("im_region_free: closing pending image \"%s\"\n", im->filename);
#endif

        im->close_pending = 0;
        im_close(im);
    }

    im_free(reg);

#ifdef DEBUG
    g_mutex_lock(im__global_lock);
    g_assert(g_slist_find(im__regions_all, reg));
    im__regions_all = g_slist_remove(im__regions_all, reg);
    printf("%d regions in vips\n", g_slist_length(im__regions_all));
    g_mutex_unlock(im__global_lock);
#endif
}

int im_region_buffer(REGION *reg, Rect *r) {
    IMAGE *im = reg->im;

    Rect image;
    Rect clipped;

    im__region_check_ownership(reg);

    image.top = 0;
    image.left = 0;
    image.width = im->Xsize;
    image.height = im->Ysize;
    im_rect_intersectrect(r, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        im_error("im_region_buffer", "%s", _("valid clipped to nothing"));
        return (-1);
    }

    if (reg->invalid) {
        im_region_reset(reg);
        if (!(reg->buffer = im_buffer_new(im, &clipped))) {
            return (-1);
        }
    } else {

        IM_FREEF(im_window_unref, reg->window);
        if (!(reg->buffer = im_buffer_unref_ref(reg->buffer, im, &clipped))) {
            return (-1);
        }
    }

    reg->valid = reg->buffer->area;
    reg->bpl = IM_IMAGE_SIZEOF_PEL(im) * reg->buffer->area.width;
    reg->type = IM_REGION_BUFFER;
    reg->data = reg->buffer->buf;

    return (0);
}

int im_region_image(REGION *reg, Rect *r) {
    Rect image;
    Rect clipped;

    im__region_check_ownership(reg);

    image.top = 0;
    image.left = 0;
    image.width = reg->im->Xsize;
    image.height = reg->im->Ysize;
    im_rect_intersectrect(r, &image, &clipped);

    if (im_rect_isempty(&clipped)) {
        im_error("im_region_image", "%s", _("valid clipped to nothing"));
        return (-1);
    }

    if (reg->im->data) {

        im_region_reset(reg);

        reg->valid = clipped;
        reg->bpl = IM_IMAGE_SIZEOF_LINE(reg->im);
        reg->data = reg->im->data + (gint64)clipped.top * IM_IMAGE_SIZEOF_LINE(reg->im) + clipped.left * IM_IMAGE_SIZEOF_PEL(reg->im);
        reg->type = IM_REGION_OTHER_IMAGE;
    } else if (reg->im->dtype == IM_OPENIN) {

        if (reg->type != IM_REGION_WINDOW || !reg->window || reg->window->top > clipped.top || reg->window->top + reg->window->height < clipped.top + clipped.height) {
            im_region_reset(reg);

            if (!(reg->window = im_window_ref(reg->im, clipped.top, clipped.height))) {
                return (-1);
            }

            reg->type = IM_REGION_WINDOW;
        }

        reg->valid.left = 0;
        reg->valid.top = reg->window->top;
        reg->valid.width = reg->im->Xsize;
        reg->valid.height = reg->window->height;
        reg->bpl = IM_IMAGE_SIZEOF_LINE(reg->im);
        reg->data = reg->window->data;
    } else {
        im_error("im_region_image", "%s", _("bad image type"));
        return (-1);
    }

    return (0);
}

int im_region_region(REGION *reg, REGION *dest, Rect *r, int x, int y) {
    Rect image;
    Rect wanted;
    Rect clipped;
    Rect clipped2;
    Rect final;

    if (!dest->data || IM_IMAGE_SIZEOF_PEL(dest->im) != IM_IMAGE_SIZEOF_PEL(reg->im)) {
        im_error("im_region_region", "%s", _("inappropriate region type"));
        return (-1);
    }
    im__region_check_ownership(reg);

    image.top = 0;
    image.left = 0;
    image.width = reg->im->Xsize;
    image.height = reg->im->Ysize;
    im_rect_intersectrect(r, &image, &clipped);

    wanted.left = x + (clipped.left - r->left);
    wanted.top = y + (clipped.top - r->top);
    wanted.width = clipped.width;
    wanted.height = clipped.height;

    if (!im_rect_includesrect(&dest->valid, &wanted)) {
        im_error("im_region_region", "%s", _("dest too small"));
        return (-1);
    }

    im_rect_intersectrect(&wanted, &dest->valid, &clipped2);

    final.left = r->left + (clipped2.left - wanted.left);
    final.top = r->top + (clipped2.top - wanted.top);
    final.width = clipped2.width;
    final.height = clipped2.height;

    if (im_rect_isempty(&final)) {
        im_error("im_region_region", "%s", _("valid clipped to nothing"));
        return (-1);
    }

    im_region_reset(reg);
    reg->valid = final;
    reg->bpl = dest->bpl;
    reg->data = IM_REGION_ADDR(dest, clipped2.left, clipped2.top);
    reg->type = IM_REGION_OTHER_REGION;

    return (0);
}

int im_region_equalsregion(REGION *reg1, REGION *reg2) {
    return (reg1->im == reg2->im && im_rect_equalsrect(&reg1->valid, &reg2->valid) && reg1->data == reg2->data);
}

int im_region_position(REGION *reg, int x, int y) {
    Rect req, image, clipped;

    image.top = 0;
    image.left = 0;
    image.width = reg->im->Xsize;
    image.height = reg->im->Ysize;
    req.top = y;
    req.left = x;
    req.width = reg->valid.width;
    req.height = reg->valid.height;
    im_rect_intersectrect(&image, &req, &clipped);
    if (x < 0 || y < 0 || im_rect_isempty(&clipped)) {
        im_error("im_region_position", "%s", _("bad position"));
        return (-1);
    }

    reg->valid = clipped;
    reg->invalid = FALSE;

    return (0);
}

int im_region_fill(REGION *reg, Rect *r, im_region_fill_fn fn, void *a) {
    g_assert(reg->im->dtype == IM_PARTIAL);
    g_assert(reg->im->generate);

    if (im_region_buffer(reg, r)) {
        return (-1);
    }

    if (!reg->buffer->done) {
        if (fn(reg, a)) {
            return (-1);
        }

        if (reg->buffer) {
            im_buffer_done(reg->buffer);
        }
    }

    return (0);
}

void im_region_print(REGION *reg) {
    printf("REGION: %p, ", reg);
    printf("im = %p, ", reg->im);
    printf("valid.left = %d, ", reg->valid.left);
    printf("valid.top = %d, ", reg->valid.top);
    printf("valid.width = %d, ", reg->valid.width);
    printf("valid.height = %d, ", reg->valid.height);
    printf("type = %d, ", reg->type);
    printf("data = %p, ", reg->data);
    printf("bpl = %d, ", reg->bpl);
    printf("seq = %p, ", reg->seq);
    printf("thread = %p, ", reg->thread);
    printf("window = %p, ", reg->window);
    printf("buffer = %p\n", reg->buffer);
    printf("invalid = %d\n", reg->invalid);
}

void im_region_paint(REGION *reg, Rect *r, int value) {
    Rect ovl;

    im_rect_intersectrect(r, &reg->valid, &ovl);
    if (!im_rect_isempty(&ovl)) {
        PEL *q = (PEL *)IM_REGION_ADDR(reg, ovl.left, ovl.top);
        int wd = ovl.width * IM_IMAGE_SIZEOF_PEL(reg->im);
        int ls = IM_REGION_LSKIP(reg);
        int y;

        for (y = 0; y < ovl.height; y++) {
            memset((char *)q, value, wd);
            q += ls;
        }
    }
}

void im_region_black(REGION *reg) {
    im_region_paint(reg, &reg->valid, 0);
}

void im_region_copy(REGION *reg, REGION *dest, Rect *r, int x, int y) {
    int z;
    int len = IM_IMAGE_SIZEOF_PEL(reg->im) * r->width;
    char *p = IM_REGION_ADDR(reg, r->left, r->top);
    char *q = IM_REGION_ADDR(dest, x, y);
    int plsk = IM_REGION_LSKIP(reg);
    int qlsk = IM_REGION_LSKIP(dest);

#ifdef DEBUG

    Rect output;

    printf("im_region_copy: sanity check\n");

    output.left = x;
    output.top = y;
    output.width = r->width;
    output.height = r->height;

    g_assert(im_rect_includesrect(&dest->valid, &output));

    g_assert(im_rect_includesrect(&reg->valid, r));

    g_assert(IM_IMAGE_SIZEOF_PEL(reg->im) == IM_IMAGE_SIZEOF_PEL(dest->im));
#endif

    for (z = 0; z < r->height; z++) {
        memcpy(q, p, len);

        p += plsk;
        q += qlsk;
    }
}
