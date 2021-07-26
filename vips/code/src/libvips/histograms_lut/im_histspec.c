
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int match(IMAGE *in, IMAGE *ref, IMAGE *out) {
    const int inpx = in->Xsize * in->Ysize;
    const int refpx = ref->Xsize * ref->Ysize;
    const int bands = in->Bands;

    unsigned int *inbuf;
    unsigned int *refbuf;
    unsigned int *outbuf;

    int px;
    int max;

    int i, j;

    if (im_iocheck(in, out) || im_iocheck(ref, out) || im_check_uncoded("im_histspec", in) || im_check_format("im_histspec", in, IM_BANDFMT_UINT) || im_check_coding_same("im_histspec", in, ref) || im_check_format_same("im_histspec", in, ref) || im_check_bands_same("im_histspec", in, ref) || im_check_hist("im_histspec", in) || im_check_hist("im_histspec", ref)) {
        return (-1);
    }

    if (inpx <= 256 && refpx <= 256) {
        px = 256;
    } else if (inpx <= 65536 && refpx <= 65536) {
        px = 65536;
    }
    max = px * bands;

    inbuf = IM_ARRAY(out, max, unsigned int);
    refbuf = IM_ARRAY(out, max, unsigned int);
    outbuf = IM_ARRAY(out, max, unsigned int);
    if (!inbuf || !refbuf || !outbuf) {
        return (-1);
    }
    for (i = 0; i < inpx * bands; i++) {
        inbuf[i] = ((unsigned int *)in->data)[i];
    }
    for (; i < max; i++) {
        inbuf[i] = 0;
    }
    for (i = 0; i < refpx * bands; i++) {
        refbuf[i] = ((unsigned int *)ref->data)[i];
    }
    for (; i < max; i++) {
        refbuf[i] = 0;
    }

    for (j = 0; j < bands; j++) {

        int ri = j;
        int limit = max - bands;

        for (i = j; i < max; i += bands) {
            unsigned int inv = inbuf[i];

            for (; ri < limit; ri += bands) {
                if (inv <= refbuf[ri]) {
                    break;
                }
            }

            if (ri < limit) {

                double mid = refbuf[ri] + refbuf[ri + bands] / 2.0;

                if (inv < mid) {
                    outbuf[i] = ri / bands;
                } else {
                    outbuf[i] = ri / bands + 1;
                }
            } else {
                outbuf[i] = refbuf[ri];
            }
        }
    }

    if (im_cp_descv(out, in, ref, NULL)) {
        return (-1);
    }
    out->Xsize = px;
    out->Ysize = 1;
    out->Type = IM_TYPE_HISTOGRAM;
    if (im_setupout(out)) {
        return (-1);
    }

    if (im_writeline(0, out, (PEL *)outbuf)) {
        return (-1);
    }

    return (0);
}

int im_histspec(IMAGE *in, IMAGE *ref, IMAGE *out) {
    IMAGE *t[5];
    int px;
    int fmt;

    if (im_check_uint("im_histspec", in) || im_check_uint("im_histspec", ref)) {
        return (-1);
    }

    if (im_open_local_array(out, t, 5, "im_histspec", "p") || im_histeq(in, t[0]) || im_clip2fmt(t[0], t[1], IM_BANDFMT_UINT) || im_histeq(ref, t[2]) || im_clip2fmt(t[2], t[3], IM_BANDFMT_UINT) || match(t[1], t[3], t[4])) {
        return (-1);
    }

    px = t[4]->Xsize * t[4]->Ysize;
    if (px <= 256) {
        fmt = IM_BANDFMT_UCHAR;
    } else if (px <= 65536) {
        fmt = IM_BANDFMT_USHORT;
    } else {
        fmt = IM_BANDFMT_UINT;
    }

    if (im_clip2fmt(t[4], out, fmt)) {
        return (-1);
    }

    return (0);
}
