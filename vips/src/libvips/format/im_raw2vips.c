

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_raw2vips(const char *filename, IMAGE *out, int width, int height, int bpp, int offset) {
    IMAGE *t;

    if (!(t = im_binfile(filename, width, height, bpp, offset))) {
        return (-1);
    }
    if (im_add_close_callback(out, (im_callback_fn)im_close, t, NULL)) {
        im_close(t);
        return (-1);
    }
    if (im_copy(t, out)) {
        return (-1);
    }

    return (0);
}
