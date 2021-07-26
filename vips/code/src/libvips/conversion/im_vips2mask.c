
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

DOUBLEMASK *im_vips2mask(IMAGE *in, const char *outname) {
    int width, height;
    DOUBLEMASK *out;

    if (in->BandFmt != IM_BANDFMT_DOUBLE) {
        IMAGE *t;

        if (!(t = im_open("im_vips2mask", "p"))) {
            return (NULL);
        }
        if (im_clip2fmt(in, t, IM_BANDFMT_DOUBLE) || !(out = im_vips2mask(t, outname))) {
            im_close(t);
            return (NULL);
        }
        im_close(t);

        return (out);
    }

    if (im_incheck(in) || im_check_uncoded("im_vips2mask", in)) {
        return (NULL);
    }

    if (in->Bands == 1) {
        width = in->Xsize;
        height = in->Ysize;
    } else if (in->Xsize == 1) {
        width = in->Bands;
        height = in->Ysize;
    } else if (in->Ysize == 1) {
        width = in->Xsize;
        height = in->Bands;
    } else {
        im_error("im_vips2mask", "%s", _("one band, nx1, or 1xn images only"));
        return (NULL);
    }

    if (!(out = im_create_dmask(outname, width, height))) {
        return (NULL);
    }
    if (in->Bands > 1 && in->Ysize == 1) {
        double *data = (double *)in->data;
        int x, y;

        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                out->coeff[x + y * width] = data[x * height + y];
            }
        }
    } else {
        memcpy(out->coeff, in->data, width * height * sizeof(double));
    }

    return (out);
}
