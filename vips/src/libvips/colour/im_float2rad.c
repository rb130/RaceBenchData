

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define RED 0
#define GRN 1
#define BLU 2
#define CIEX 0
#define CIEY 1
#define CIEZ 2
#define EXP 3
#define COLXS 128
#define WHT 3

#undef BYTE
#define BYTE unsigned char

typedef BYTE COLR[4];

typedef float COLORV;
typedef COLORV COLOR[3];

#define copycolor(c1, c2) ((c1)[0] = (c2)[0], (c1)[1] = (c2)[1], (c1)[2] = (c2)[2])

static void setcolr(COLR clr, double r, double g, double b) {
    double d;
    int e;

    d = r > g ? r : g;
    if (b > d) {
        d = b;
    }

    if (d <= 1e-32) {
        clr[RED] = clr[GRN] = clr[BLU] = 0;
        clr[EXP] = 0;
        return;
    }

    d = frexp(d, &e) * 255.9999 / d;

    if (r > 0.0) {
        clr[RED] = r * d;
    } else {
        clr[RED] = 0;
    }
    if (g > 0.0) {
        clr[GRN] = g * d;
    } else {
        clr[GRN] = 0;
    }
    if (b > 0.0) {
        clr[BLU] = b * d;
    } else {
        clr[BLU] = 0;
    }

    clr[EXP] = e + COLXS;
}

static void float2rad(COLOR *inp, COLR *outbuf, int n) {
    while (n-- > 0) {
        setcolr(outbuf[0], inp[0][RED], inp[0][GRN], inp[0][BLU]);
        inp++;
        outbuf++;
    }
}

int im_float2rad(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_check_uncoded("im_float2rad", in) || im_check_bands("im_float2rad", in, 3) || im_open_local_array(out, t, 1, "im_float2rad", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_cp_desc(out, t[0])) {
        return (-1);
    }
    out->Bands = 4;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_RAD;

    if (im_wrapone(t[0], out, (im_wrapone_fn)float2rad, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
