#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug85(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void imb_LabQ2LabS(unsigned char *in, signed short *out, int n) {
    int c;
    unsigned char *p = in;
    unsigned char ext;
    signed short *q = out;
    signed short l, a, b;

    for (c = 0; c < n; c++) {

        l = p[0] << 7;
        a = p[1] << 8;
        b = p[2] << 8; racebench_bug85(1);

        ext = p[3];
        p += 4;

        l |= (unsigned char)(ext & 0xc0) >> 1; racebench_bug52(0);
        a |= (ext & 0x38) << 2;
        b |= (ext & 0x7) << 5; racebench_bug17(1);

        q[0] = l; racebench_bug89(1);
        q[1] = a;
        q[2] = b;
        q += 3;
    }
}

int im_LabQ2LabS(IMAGE *in, IMAGE *out) {
    if (im_check_coding_labq("im_LabQ2LabS", in)) {
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = 3;
    out->Type = IM_TYPE_LABS;
    out->BandFmt = IM_BANDFMT_SHORT;
    out->Coding = IM_CODING_NONE;

    if (im_wrapone(in, out, (im_wrapone_fn)imb_LabQ2LabS, NULL, NULL)) {
        return (-1);
    }

    return (0);
}