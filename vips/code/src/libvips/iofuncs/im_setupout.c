
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <fcntl.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef BINARY_OPEN
#define MODE (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY)
#else
#define MODE (O_WRONLY | O_CREAT | O_TRUNC)
#endif

int im_setupout(IMAGE *im) {
    g_assert(!im_image_sanity(im));

    if (im->Xsize <= 0 || im->Ysize <= 0 || im->Bands <= 0) {
        im_error("im_setupout", "%s", _("bad dimensions"));
        return (-1);
    }

    im->Bbits = im_bits_of_fmt(im->BandFmt);

    if (im->dtype == IM_PARTIAL) {

#ifdef DEBUG_IO
        printf("im_setupout: old-style output for %s\n", im->filename);
#endif

        im->dtype = IM_SETBUF;
    }

    switch (im->dtype) {
    case IM_MMAPINRW:
    case IM_SETBUF_FOREIGN:

        break;

    case IM_SETBUF:

        if (im->data) {

            im_error("im_setupout", "%s", _("called twice!"));
            return (-1);
        }
        if (!(im->data = im_malloc(NULL, IM_IMAGE_SIZEOF_LINE(im) * im->Ysize))) {
            return (-1);
        }

        break;

    case IM_OPENOUT: {

        unsigned char header[IM_SIZEOF_HEADER];

        if ((im->fd = open(im->filename, MODE, 0666)) < 0) {
            im_error("im_setupout", _("unable to write to \"%s\""), im->filename);
            return (-1);
        }
        if (im__write_header_bytes(im, header) || im__write(im->fd, header, IM_SIZEOF_HEADER)) {
            return (-1);
        }

        break;
    }

    default:
        im_error("im_setupout", "%s", _("bad image descriptor"));
        return (-1);
    }

    return (0);
}
