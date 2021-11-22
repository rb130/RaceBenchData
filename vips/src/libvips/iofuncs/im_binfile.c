

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

IMAGE *im_binfile(const char *name, int xsize, int ysize, int bands, int offset) {
    IMAGE *im;
    gint64 psize;
    gint64 rsize;

    if (xsize <= 0 || ysize <= 0 || bands <= 0) {
        im_error("im_binfile", "%s", _("bad parameters"));
        return (NULL);
    }

    if (!(im = im_init(name))) {
        return (NULL);
    }
    if ((im->fd = im__open_image_file(name)) == -1) {
        im_close(im);
        return (NULL);
    }
    im->dtype = IM_OPENIN;
    im->sizeof_header = offset;

    psize = (gint64)xsize * ysize * bands + offset;

    if ((rsize = im_file_length(im->fd)) == -1) {
        im_close(im);
        return (NULL);
    }
    im->file_length = rsize;

    if (psize > rsize) {
        im_error("im_binfile",
            _("unable to open %s: "
              "file has been truncated"),
            im->filename);
        im_close(im);
        return (NULL);
    }

    if (psize < rsize) {
        im_warn("im_binfile", _("%s is longer than expected"), im->filename);
    }

    im->Xsize = xsize;
    im->Ysize = ysize;
    im->Bands = bands;

    im->BandFmt = IM_BANDFMT_UCHAR;
    im->Bbits = im_bits_of_fmt(im->BandFmt);
    im->Coding = IM_CODING_NONE;

    if (bands == 1) {
        im->Type = IM_TYPE_B_W;
    } else if (bands == 3) {
        im->Type = IM_TYPE_RGB;
    } else {
        im->Type = IM_TYPE_MULTIBAND;
    }

    im->Xres = 1.0;
    im->Yres = 1.0;

    im->Length = 0;
    im->Compression = 0;
    im->Level = 0;

    im->Xoffset = 0;
    im->Yoffset = 0;

    im->dhint = IM_THINSTRIP;

    return (im);
}
