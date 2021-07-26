
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

int im_tbjoin(IMAGE *top, IMAGE *bottom, IMAGE *out) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_tbjoin:1", "p")) || im_insert(top, bottom, t1, 0, top->Ysize) || im_extract_area(t1, out, 0, 0, IM_MIN(top->Xsize, bottom->Xsize), t1->Ysize)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = top->Ysize;

    return (0);
}
