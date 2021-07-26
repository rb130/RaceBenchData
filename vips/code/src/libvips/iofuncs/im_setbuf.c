
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

IMAGE *im_setbuf(const char *file_name) {
    IMAGE *im;

    if (!(im = im_init(file_name))) {
        return (NULL);
    }
    im->dtype = IM_SETBUF;

    im->dhint = IM_ANY;

    return (im);
}
