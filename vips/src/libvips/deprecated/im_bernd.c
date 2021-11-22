

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

static int extract(IMAGE *in, int x, int y, int w, int h) {
    IMAGE *t1;
    int len;
    char *buf;

    if (!(t1 = im_open_local(in, "im_bernd:2", "p")) || im_extract_area(in, t1, x, y, w, h) || im_vips2bufjpeg(t1, in, 75, &buf, &len)) {
        return (-1);
    }

    if (fwrite(buf, sizeof(char), len, stdout) != (size_t)len) {
        im_error("im_bernd", "%s", _("error writing output"));
        return (-1);
    }
    fflush(stdout);

    return (0);
}

int im_bernd(const char *tiffname, int x, int y, int w, int h) {
    IMAGE *in;

    if (!(in = im_open("im_bernd:1", "p"))) {
        return (-1);
    }
    if (im_tiff2vips(tiffname, in) || extract(in, x, y, w, h)) {
        im_close(in);
        return (-1);
    }
    im_close(in);

    return (0);
}
