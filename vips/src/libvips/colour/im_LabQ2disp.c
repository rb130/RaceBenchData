

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    struct im_col_display *disp;
    PEL red[64 * 64 * 64];
    PEL green[64 * 64 * 64];
    PEL blue[64 * 64 * 64];
} CalibrateInfo;

#define index(L, A, B) (L + (A << 6) + (B << 12))

static void imb_LabQ2disp(PEL *p, PEL *q, int n, CalibrateInfo *cal) {
    int x, t;

    int le = 0;
    int ae = 0;
    int be = 0;

    for (x = 0; x < n; x++) {

        int L = p[0] + le;
        int A = (signed char)p[1] + ae;
        int B = (signed char)p[2] + be;
        p += 4;

        L = IM_MIN(255, L);
        A = IM_MIN(127, A);
        B = IM_MIN(127, B);

        le = L & 3;
        ae = A & 3;
        be = B & 3;

        L = (L >> 2) & 63;
        A = (A >> 2) & 63;
        B = (B >> 2) & 63;

        t = index(L, A, B);
        q[0] = cal->red[t];
        q[1] = cal->green[t];
        q[2] = cal->blue[t];
        q += 3;
    }
}

void *im_LabQ2disp_build_table(IMAGE *out, struct im_col_display *d) {
    CalibrateInfo *cal;
    int l, a, b;
    int t;

    if (!(cal = IM_NEW(out, CalibrateInfo))) {
        return (NULL);
    }
    cal->disp = d;

    for (l = 0; l < 64; l++) {
        for (a = 0; a < 64; a++) {
            for (b = 0; b < 64; b++) {

                float L = (l << 2) * (100.0 / 256.0);
                float A = (signed char)(a << 2);
                float B = (signed char)(b << 2);
                float X, Y, Z;
                int rb, gb, bb;
                int oflow;

                im_col_Lab2XYZ(L, A, B, &X, &Y, &Z);

                im_col_XYZ2rgb(cal->disp, X, Y, Z, &rb, &gb, &bb, &oflow);

                t = index(l, a, b);
                cal->red[t] = rb;
                cal->green[t] = gb;
                cal->blue[t] = bb;
            }
        }
    }

    return ((void *)cal);
}

int im_LabQ2disp_table(IMAGE *in, IMAGE *out, void *table) {
    CalibrateInfo *cal = (CalibrateInfo *)table;

    if (im_check_coding_labq("im_LabQ2disp", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 3;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_NONE;
    out->Type = IM_TYPE_RGB;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_LabQ2disp, cal, NULL)) {
        return (-1);
    }

    return (0);
}

int im_LabQ2disp(IMAGE *in, IMAGE *out, struct im_col_display *d) {
    void *table;

    if (!(table = im_LabQ2disp_build_table(out, d)) || im_LabQ2disp_table(in, out, table)) {
        return (-1);
    }

    return (0);
}
