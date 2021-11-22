

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_profile(IMAGE *in, IMAGE *out, int dir) {
    int x, y;
    unsigned short *buf;

    if (im_iocheck(in, out)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE || in->BandFmt != IM_BANDFMT_UCHAR || in->Bands != 1) {
        im_error("im_profile", "%s", _("1-band uchar uncoded only"));
        return (-1);
    }
    if (dir != 0 && dir != 1) {
        im_error("im_profile", "%s", _("dir not 0 or 1"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Type = IM_TYPE_HISTOGRAM;
    if (dir == 0) {
        out->Xsize = in->Xsize;
        out->Ysize = 1;
    } else {
        out->Xsize = 1;
        out->Ysize = in->Ysize;
    }
    out->BandFmt = IM_BANDFMT_USHORT;
    if (im_setupout(out)) {
        return (-1);
    }
    if (!(buf = IM_ARRAY(out, out->Xsize, unsigned short))) {
        return (-1);
    }

    if (dir == 0) {

        for (x = 0; x < in->Xsize; x++) {
            PEL *p = (PEL *)IM_IMAGE_ADDR(in, x, 0);
            int lsk = IM_IMAGE_SIZEOF_LINE(in);

            for (y = 0; y < in->Ysize; y++) {
                if (p[y * lsk]) {
                    break;
                }
            }

            buf[x] = y;
        }

        if (im_writeline(0, out, (PEL *)buf)) {
            return (-1);
        }
    } else {

        for (y = 0; y < in->Ysize; y++) {
            PEL *p = (PEL *)IM_IMAGE_ADDR(in, 0, y);

            for (x = 0; x < in->Xsize; x++) {
                if (p[x]) {
                    break;
                }
            }

            buf[0] = x;

            if (im_writeline(y, out, (PEL *)buf)) {
                return (-1);
            }
        }
    }

    return (0);
}
