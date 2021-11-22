

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

IMAGE *im_partial(const char *filename) {
    IMAGE *im = im_init(filename);

    if (!im) {
        return (NULL);
    }
    im->dtype = IM_PARTIAL;

    return (im);
}
