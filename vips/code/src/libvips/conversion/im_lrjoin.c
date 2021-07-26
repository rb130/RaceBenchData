
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_lrjoin(IMAGE *left, IMAGE *right, IMAGE *out) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_lrjoin:1", "p")) || im_insert(left, right, t1, left->Xsize, 0) || im_extract_area(t1, out, 0, 0, t1->Xsize, IM_MIN(left->Ysize, right->Ysize))) {
        return (-1);
    }

    out->Xoffset = left->Xsize;
    out->Yoffset = 0;

    return (0);
}
