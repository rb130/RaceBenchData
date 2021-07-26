
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#ifdef HAVE_IO_H
#include <io.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_writeline(int ypos, IMAGE *im, PEL *linebuffer) {
    int linesize = IM_IMAGE_SIZEOF_LINE(im);
    char *tmp;

    if (ypos == 0) {
        im__start_eval(im);
    }

    switch (im->dtype) {
    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
        tmp = im->data + ypos * linesize;
        memcpy(tmp, linebuffer, linesize);

        break;

    case IM_OPENOUT:

        if (im__write(im->fd, linebuffer, linesize)) {
            return (-1);
        }

        break;

    default:
        im_error("im_writeline", _("unable to output to a %s image"), im_dtype2char(im->dtype));
        return (-1);
    }

    if (im__handle_eval(im, im->Xsize, 1)) {
        return (-1);
    }
    if (im__test_kill(im)) {
        return (-1);
    }

    if (ypos == im->Ysize - 1) {
        if (im__end_eval(im) || im__trigger_callbacks(im->writtenfns)) {
            return (-1);
        }
    }

    return (0);
}
