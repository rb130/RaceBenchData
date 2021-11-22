

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_wrap(IMAGE *in, IMAGE *out, int x, int y) {
    IMAGE *t;

    x = x < 0 ? -x % in->Xsize : in->Xsize - x % in->Xsize;
    y = y < 0 ? -y % in->Ysize : in->Ysize - y % in->Ysize;

    if (!(t = im_open_local(out, "im_wrap", "p")) || im_replicate(in, t, 2, 2) || im_extract_area(t, out, x, y, in->Xsize, in->Ysize)) {
        return (-1);
    }

    out->Xoffset = x;
    out->Yoffset = y;

    return (0);
}
