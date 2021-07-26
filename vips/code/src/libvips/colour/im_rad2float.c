
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

static void colr_color(col, clr) register COLOR col;
register COLR clr;
{
    double f;

    if (clr[EXP] == 0) {
        col[RED] = col[GRN] = col[BLU] = 0.0;
    } else {
        f = ldexp(1.0, (int)clr[EXP] - (COLXS + 8));
        col[RED] = (clr[RED] + 0.5) * f;
        col[GRN] = (clr[GRN] + 0.5) * f;
        col[BLU] = (clr[BLU] + 0.5) * f;
    }
}

static void rad2float(COLR *inp, COLOR *outbuf, int n) {
    colr_color(outbuf[0], inp[0]);
    while (--n > 0) {
        outbuf++;
        inp++;
        if (inp[0][RED] == inp[-1][RED] && inp[0][GRN] == inp[-1][GRN] && inp[0][BLU] == inp[-1][BLU] && inp[0][EXP] == inp[-1][EXP]) {
            copycolor(outbuf[0], outbuf[-1]);
        } else {
            colr_color(outbuf[0], inp[0]);
        }
    }
}

int im_rad2float(IMAGE *in, IMAGE *out) {

    if (in->Coding != IM_CODING_RAD) {
        im_error("im_rad2float", "%s", _("not a RAD image"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 3;
    out->BandFmt = IM_BANDFMT_FLOAT;
    out->Coding = IM_CODING_NONE;

    if (im_wrapone(in, out, (im_wrapone_fn)rad2float, NULL, NULL)) {
        return (-1);
    }

    return (0);
}
