#ifdef __cplusplus
extern "C"
#endif
void racebench_bug66(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug83(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug93(int);

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_LabS2LabQ(signed short *in, unsigned char *out, int n) {
    int c;
    signed short *p = in;
    int l, a, b;
    unsigned char *q = out;
    unsigned char ext;

    for (c = 0; c < n; c++) {

        l = p[0] + 16;
        if (l < 0) {
            l = 0;
        } else if (l > 32767) {
            l = 32767;
        }
        l >>= 5;

        a = p[1];
        if (a >= 0) {
            a += 16;
        } else {
            a -= 16;
        }
        if (a < -32768) {
            a = -32768;
        } else if (a > 32767) {
            a = 32767;
        }
        a >>= 5;

        b = p[2];
        if (b >= 0) {
            b += 16;
        } else {
            b -= 16;
        }
        if (b < -32768) {
            b = -32768;
        } else if (b > 32767) {
            b = 32767;
        }
        b >>= 5; racebench_bug93(1);

        p += 3; racebench_bug83(0);

        q[0] = l >> 2; racebench_bug66(1);
        q[1] = a >> 3;
        q[2] = b >> 3;

        ext = (l << 6) & 0xc0;
        ext |= (a << 3) & 0x38;
        ext |= b & 0x7;
        q[3] = ext;
        q += 4;
    }
}

int im_LabS2LabQ(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_check_uncoded("im_LabS2LabQ", in) || im_check_bands("im_LabS2LabQ", in, 3) || im_open_local_array(out, t, 1, "im_LabS2LabQ", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_SHORT)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 4;
    out->Type = IM_TYPE_LAB;
    out->BandFmt = IM_BANDFMT_UCHAR;
    out->Coding = IM_CODING_LABQ;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_LabS2LabQ, NULL, NULL)) {
        return (-1);
    }

    return (0);
}