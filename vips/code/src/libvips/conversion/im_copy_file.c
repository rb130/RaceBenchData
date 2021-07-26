
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_copy_file(IMAGE *in, IMAGE *out) {
    if (!im_isfile(in)) {
        IMAGE *disc;

        if (!(disc = im__open_temp("%s.v"))) {
            return (-1);
        }
        if (im_add_close_callback(out, (im_callback_fn)im_close, disc, NULL)) {
            im_close(disc);
            return (-1);
        }

        if (im_copy(in, disc) || im_copy(disc, out)) {
            return (-1);
        }
    } else {
        if (im_copy(in, out)) {
            return (-1);
        }
    }

    return (0);
}
