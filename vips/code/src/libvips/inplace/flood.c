
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define SWAP(TYPE, A, B) \
    {                    \
        TYPE t = (A);    \
        (A) = (B);       \
        (B) = t;         \
    }

#define PBUFSIZE (1000)

typedef struct {
    int x1, x2;
    int y;
    int dir;
} Scan;

typedef struct _Buffer {
    struct _Buffer *next;
    int n;
    Scan scan[PBUFSIZE];
} Buffer;

typedef struct {

    IMAGE *test;
    IMAGE *mark;
    int x, y;
    PEL *ink;
    Rect *dout;

    PEL *edge;
    int equal;
    int tsize;
    int msize;
    int left, right;
    int top, bottom;

    Buffer *in;
    Buffer *out;
} Flood;

static Buffer *buffer_build(void) {
    Buffer *buf = IM_NEW(NULL, Buffer);

    if (!buf) {
        return (NULL);
    }
    buf->next = NULL;
    buf->n = 0;

    return (buf);
}

static void buffer_free(Buffer *buf) {
    while (buf) {
        Buffer *p;

        p = buf->next;
        im_free(buf);
        buf = p;
    }
}

static inline Buffer *buffer_add(Buffer *buf, Flood *flood, int x1, int x2, int y, int dir) {

    if (y < 0 || y >= flood->test->Ysize) {
        return (buf);
    }
    x1 = IM_CLIP(0, x1, flood->test->Xsize - 1);
    x2 = IM_CLIP(0, x2, flood->test->Xsize - 1);
    if (x2 - x1 < 0) {
        return (buf);
    }

    buf->scan[buf->n].x1 = x1;
    buf->scan[buf->n].x2 = x2;
    buf->scan[buf->n].y = y;
    buf->scan[buf->n].dir = dir;
    buf->n += 1;

    if (buf->n == PBUFSIZE) {
        Buffer *new;

        if (!(new = buffer_build())) {
            return (NULL);
        }
        new->next = buf;
        buf = new;
    }

    return (buf);
}

static inline gboolean flood_connected(Flood *flood, PEL *tp) {
    int j;

    for (j = 0; j < flood->tsize; j++) {
        if (tp[j] != flood->edge[j]) {
            break;
        }
    }

    return (flood->equal ^ (j < flood->tsize));
}

static inline gboolean flood_painted(Flood *flood, PEL *mp) {
    int j;

    for (j = 0; j < flood->msize; j++) {
        if (mp[j] != flood->ink[j]) {
            break;
        }
    }

    return (j == flood->msize);
}

static inline void flood_paint(Flood *flood, PEL *q) {
    int j;

    for (j = 0; j < flood->msize; j++) {
        q[j] = flood->ink[j];
    }
}

static void flood_scanline(Flood *flood, int x, int y, int *x1, int *x2) {
    const int width = flood->mark->Xsize;

    PEL *tp;
    PEL *mp;
    int i;
    int len;

    tp = (PEL *)IM_IMAGE_ADDR(flood->test, x + 1, y);
    for (i = x + 1; i < width; i++) {
        if (!flood_connected(flood, tp)) {
            break;
        }
        tp += flood->tsize;
    }
    *x2 = i - 1;

    tp = (PEL *)IM_IMAGE_ADDR(flood->test, x - 1, y);
    for (i = x - 1; i >= 0; i--) {
        if (!flood_connected(flood, tp)) {
            break;
        }
        tp -= flood->tsize;
    }
    *x1 = i + 1;

    mp = (PEL *)IM_IMAGE_ADDR(flood->mark, *x1, y);
    len = *x2 - *x1 + 1;
    for (i = 0; i < len; i++) {
        flood_paint(flood, mp);
        mp += flood->msize;
    }

    if (flood->dout) {
        flood->left = IM_MIN(flood->left, *x1);
        flood->right = IM_MAX(flood->right, *x2);
        flood->top = IM_MIN(flood->top, y);
        flood->bottom = IM_MAX(flood->bottom, y);
    }
}

static void flood_around(Flood *flood, Scan *scan) {
    PEL *tp;
    int x;

    g_assert(scan->dir == 1 || scan->dir == -1);

    for (tp = (PEL *)IM_IMAGE_ADDR(flood->test, scan->x1, scan->y), x = scan->x1; x <= scan->x2; tp += flood->tsize, x++) {
        if (flood_connected(flood, tp)) {
            int x1a;
            int x2a;

            if (flood->mark != flood->test) {
                PEL *mp = (PEL *)IM_IMAGE_ADDR(flood->mark, x, scan->y);

                if (flood_painted(flood, mp)) {
                    continue;
                }
            }

            flood_scanline(flood, x, scan->y, &x1a, &x2a);

            if (x1a < scan->x1 - 1) {
                flood->out = buffer_add(flood->out, flood, x1a, scan->x1 - 2, scan->y - scan->dir, -scan->dir);
            }
            if (x2a > scan->x2 + 1) {
                flood->out = buffer_add(flood->out, flood, scan->x2 + 2, x2a, scan->y - scan->dir, -scan->dir);
            }
            flood->out = buffer_add(flood->out, flood, x1a, x2a, scan->y + scan->dir, scan->dir);

            x = x2a + 1;
            tp = (PEL *)IM_IMAGE_ADDR(flood->test, x, scan->y);
        }
    }
}

static void flood_all(Flood *flood, int x, int y) {
    int x1, x2;

    if (!flood_connected(flood, (PEL *)IM_IMAGE_ADDR(flood->test, x, y))) {
        return;
    }

    flood_scanline(flood, x, y, &x1, &x2);
    flood->in = buffer_add(flood->in, flood, x1, x2, y + 1, 1);
    flood->in = buffer_add(flood->in, flood, x1, x2, y - 1, -1);

    while (flood->in->n) {
        Buffer *p;

        for (p = flood->in; p; p = p->next) {
            int i;

            for (i = 0; i < p->n; i++) {
                flood_around(flood, &p->scan[i]);
            }

            p->n = 0;
        }

        SWAP(Buffer *, flood->in, flood->out);
    }
}

static void flood_free(Flood *flood) {

    if (flood->dout) {
        flood->dout->left = flood->left;
        flood->dout->top = flood->top;
        flood->dout->width = flood->right - flood->left + 1;
        flood->dout->height = flood->bottom - flood->top + 1;
    }

    IM_FREE(flood->ink);
    IM_FREE(flood->edge);
    IM_FREEF(buffer_free, flood->in);
    IM_FREEF(buffer_free, flood->out);
    im_free(flood);
}

static Flood *flood_build(IMAGE *test, IMAGE *mark, int x, int y, PEL *ink, Rect *dout) {
    Flood *flood = IM_NEW(NULL, Flood);

    if (!flood) {
        return (NULL);
    }
    flood->test = test;
    flood->mark = mark;
    flood->x = x;
    flood->y = y;
    flood->ink = NULL;
    flood->dout = dout;
    flood->edge = NULL;
    flood->tsize = IM_IMAGE_SIZEOF_PEL(test);
    flood->msize = IM_IMAGE_SIZEOF_PEL(mark);
    flood->left = x;
    flood->top = y;
    flood->right = x;
    flood->bottom = y;

    flood->in = NULL;
    flood->out = NULL;

    if (!(flood->ink = (PEL *)im_malloc(NULL, flood->msize)) || !(flood->edge = (PEL *)im_malloc(NULL, flood->tsize)) || !(flood->in = buffer_build()) || !(flood->out = buffer_build())) {
        flood_free(flood);
        return (NULL);
    }
    memcpy(flood->ink, ink, flood->msize);

    return (flood);
}

int im_flood(IMAGE *im, int x, int y, PEL *ink, Rect *dout) {
    Flood *flood;

    if (im_rwcheck(im) || im_check_coding_known("im_flood", im)) {
        return (-1);
    }
    if (!(flood = flood_build(im, im, x, y, ink, dout))) {
        return (-1);
    }

    memcpy(flood->edge, ink, flood->tsize);
    flood->equal = 0;

    flood_all(flood, x, y);

    flood_free(flood);

    return (0);
}

int im_flood_blob(IMAGE *im, int x, int y, PEL *ink, Rect *dout) {
    Flood *flood;
    int j;

    if (im_rwcheck(im) || im_check_coding_known("im_flood", im)) {
        return (-1);
    }
    if (!(flood = flood_build(im, im, x, y, ink, dout))) {
        return (-1);
    }

    memcpy(flood->edge, IM_IMAGE_ADDR(im, x, y), flood->tsize);
    flood->equal = 1;

    for (j = 0; j < flood->tsize; j++) {
        if (flood->edge[j] != flood->ink[j]) {
            break;
        }
    }
    if (j == flood->tsize) {
        return (0);
    }

    flood_all(flood, x, y);

    flood_free(flood);

    return (0);
}

int im_flood_other(IMAGE *test, IMAGE *mark, int x, int y, int serial, Rect *dout) {
    int *m;
    Flood *flood;

    if (im_incheck(test) || im_rwcheck(mark) || im_check_coding_known("im_flood_other", test) || im_check_uncoded("im_flood_other", mark) || im_check_mono("im_flood_other", mark) || im_check_format("im_flood_other", mark, IM_BANDFMT_INT) || im_check_size_same("im_flood_other", test, mark)) {
        return (-1);
    }

    m = (int *)IM_IMAGE_ADDR(mark, x, y);
    if (*m == serial) {
        return (0);
    }

    if (!(flood = flood_build(test, mark, x, y, (PEL *)&serial, dout))) {
        return (-1);
    }

    memcpy(flood->edge, IM_IMAGE_ADDR(test, x, y), flood->tsize);
    flood->equal = 1;

    flood_all(flood, x, y);

    flood_free(flood);

    return (0);
}

int im_flood_copy(IMAGE *in, IMAGE *out, int x, int y, PEL *ink) {
    IMAGE *t;

    if (!(t = im_open_local(out, "im_flood_blob_copy", "t")) || im_copy(in, t) || im_flood(t, x, y, ink, NULL) || im_copy(t, out)) {
        return (-1);
    }

    return (0);
}

int im_flood_blob_copy(IMAGE *in, IMAGE *out, int x, int y, PEL *ink) {
    IMAGE *t;

    if (!(t = im_open_local(out, "im_flood_blob_copy", "t")) || im_copy(in, t) || im_flood_blob(t, x, y, ink, NULL) || im_copy(t, out)) {
        return (-1);
    }

    return (0);
}

int im_flood_other_copy(IMAGE *test, IMAGE *mark, IMAGE *out, int x, int y, int serial) {
    IMAGE *t;

    if (!(t = im_open_local(out, "im_flood_other_copy", "t")) || im_copy(mark, t) || im_flood_other(test, t, x, y, serial, NULL) || im_copy(t, out)) {
        return (-1);
    }

    return (0);
}
