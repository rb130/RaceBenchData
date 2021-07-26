
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_hist(IMAGE *in, IMAGE *out, int bandno) {
    IMAGE *hist;

    if (!(hist = im_open_local(out, "im_hist", "p")) || im_histgr(in, hist, bandno) || im_histplot(hist, out)) {
        return (-1);
    }

    return (0);
}
