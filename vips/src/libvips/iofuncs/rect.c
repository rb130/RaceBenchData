

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void im_rect_marginadjust(Rect *r, int n) {
    r->left -= n;
    r->top -= n;
    r->width += 2 * n;
    r->height += 2 * n;
}

int im_rect_includespoint(Rect *r, int x, int y) {
    return (r->left <= x && r->top <= y && r->left + r->width > x && r->top + r->height > y);
}

int im_rect_includesrect(Rect *r1, Rect *r2) {
    return (r1->left <= r2->left && r1->top <= r2->top && r1->left + r1->width >= r2->left + r2->width && r1->top + r1->height >= r2->top + r2->height);
}

void im_rect_intersectrect(Rect *r1, Rect *r2, Rect *r3) {
    int left = IM_MAX(r1->left, r2->left);
    int top = IM_MAX(r1->top, r2->top);
    int right = IM_MIN(IM_RECT_RIGHT(r1), IM_RECT_RIGHT(r2));
    int bottom = IM_MIN(IM_RECT_BOTTOM(r1), IM_RECT_BOTTOM(r2));
    int width = IM_MAX(0, right - left);
    int height = IM_MAX(0, bottom - top);

    r3->left = left;
    r3->top = top;
    r3->width = width;
    r3->height = height;
}

int im_rect_isempty(Rect *r) {
    return (r->width <= 0 || r->height <= 0);
}

void im_rect_unionrect(Rect *r1, Rect *r2, Rect *r3) {
    if (im_rect_isempty(r1)) {
        *r3 = *r2;
    } else if (im_rect_isempty(r2)) {
        *r3 = *r1;
    } else {
        int left = IM_MIN(r1->left, r2->left);
        int top = IM_MIN(r1->top, r2->top);
        int width = IM_MAX(IM_RECT_RIGHT(r1), IM_RECT_RIGHT(r2)) - left;
        int height = IM_MAX(IM_RECT_BOTTOM(r1), IM_RECT_BOTTOM(r2)) - top;

        r3->left = left;
        r3->top = top;
        r3->width = width;
        r3->height = height;
    }
}

int im_rect_equalsrect(Rect *r1, Rect *r2) {
    return (r1->left == r2->left && r1->top == r2->top && r1->width == r2->width && r1->height == r2->height);
}

Rect *im_rect_dup(Rect *r) {
    Rect *out = IM_NEW(NULL, Rect);

    if (!out) {
        return (NULL);
    }

    *out = *r;
    return (out);
}

void im_rect_normalise(Rect *r) {
    if (r->width < 0) {
        r->left += r->width;
        r->width *= -1;
    }
    if (r->height < 0) {
        r->top += r->height;
        r->height *= -1;
    }
}
