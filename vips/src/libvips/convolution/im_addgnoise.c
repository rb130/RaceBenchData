

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_addgnoise(IMAGE *in, IMAGE *out, double sigma) {
    IMAGE *t;

    if (!(t = im_open_local(out, "im_addgnoise", "p")) || im_gaussnoise(t, in->Xsize, in->Ysize, 0, sigma) || im_add(in, t, out)) {
        return (-1);
    }

    return (0);
}
