

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef HAVE_LIBOIL
#include <liboil/liboil.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define LOOP(IN, OUT)             \
    {                             \
        IN *p1 = (IN *)in[0];     \
        IN *p2 = (IN *)in[1];     \
        OUT *q = (OUT *)out;      \
                                  \
        for (x = 0; x < sz; x++)  \
            q[x] = p1[x] + p2[x]; \
    }

static void add_buffer(PEL **in, PEL *out, int width, IMAGE *im) {

    const int sz = width * im->Bands * (vips_bandfmt_iscomplex(im->BandFmt) ? 2 : 1);

    int x;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char, unsigned short);
        break;
    case IM_BANDFMT_CHAR:
        LOOP(signed char, signed short);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short, unsigned int);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short, signed int);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int, unsigned int);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int, signed int);
        break;

    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_COMPLEX:
#ifdef HAVE_LIBOIL
        oil_add_f32((float *)out, (float *)in[0], (float *)in[1], sz);
#else
        LOOP(float, float);
#endif
        break;

    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_DPCOMPLEX:
        LOOP(double, double);
        break;

    default:
        assert(0);
    }
}

#define UC IM_BANDFMT_UCHAR
#define C IM_BANDFMT_CHAR
#define US IM_BANDFMT_USHORT
#define S IM_BANDFMT_SHORT
#define UI IM_BANDFMT_UINT
#define I IM_BANDFMT_INT
#define F IM_BANDFMT_FLOAT
#define X IM_BANDFMT_COMPLEX
#define D IM_BANDFMT_DOUBLE
#define DX IM_BANDFMT_DPCOMPLEX

static int bandfmt_largest[6][6] = {

    {UC, S, US, S, UI, I}, {S, C, I, S, I, I}, {US, I, US, I, UI, I}, {S, S, I, S, I, I}, {UI, I, UI, I, UI, I}, {I, I, I, I, I, I}};

static VipsBandFmt im__format_common(VipsBandFmt in1, VipsBandFmt in2) {
    if (vips_bandfmt_iscomplex(in1) || vips_bandfmt_iscomplex(in2)) {

        if (in1 == IM_BANDFMT_DPCOMPLEX || in2 == IM_BANDFMT_DPCOMPLEX) {

            return (IM_BANDFMT_DPCOMPLEX);
        } else {
            return (IM_BANDFMT_COMPLEX);
        }

    } else if (vips_bandfmt_isfloat(in1) || vips_bandfmt_isfloat(in2)) {

        if (in1 == IM_BANDFMT_DOUBLE || in2 == IM_BANDFMT_DOUBLE) {
            return (IM_BANDFMT_DOUBLE);
        } else {
            return (IM_BANDFMT_FLOAT);
        }
    } else {

        return (bandfmt_largest[in1][in2]);
    }
}

int im__formatalike_vec(IMAGE **in, IMAGE **out, int n) {
    int i;
    VipsBandFmt fmt;

    g_assert(n >= 1);

    fmt = in[0]->BandFmt;
    for (i = 1; i < n; i++) {
        fmt = im__format_common(fmt, in[i]->BandFmt);
    }

    for (i = 0; i < n; i++) {
        if (im_clip2fmt(in[i], out[i], fmt)) {
            return (-1);
        }
    }

    return (0);
}

int im__formatalike(IMAGE *in1, IMAGE *in2, IMAGE *out1, IMAGE *out2) {
    IMAGE *in[2];
    IMAGE *out[2];

    in[0] = in1;
    in[1] = in2;
    out[0] = out1;
    out[1] = out2;

    return (im__formatalike_vec(in, out, 2));
}

int im__bandup(IMAGE *in, IMAGE *out, int n) {
    IMAGE *bands[256];
    int i;

    if (in->Bands == n) {
        return (im_copy(in, out));
    }
    if (in->Bands != 1) {
        im_error("im__bandup", _("not one band or %d bands"), n);
        return (-1);
    }
    if (n > 256 || n < 1) {
        im_error("im__bandup", "%s", _("bad bands"));
        return (-1);
    }

    for (i = 0; i < n; i++) {
        bands[i] = in;
    }

    return (im_gbandjoin(bands, out, n));
}

int im__bandalike(IMAGE *in1, IMAGE *in2, IMAGE *out1, IMAGE *out2) {
    if (im_check_bands_1orn("im__bandalike", in1, in2)) {
        return (-1);
    }
    if (im__bandup(in1, out1, IM_MAX(in1->Bands, in2->Bands)) || im__bandup(in2, out2, IM_MAX(in1->Bands, in2->Bands))) {
        return (-1);
    }

    return (0);
}

int im__arith_binary(const char *domain, IMAGE *in1, IMAGE *in2, IMAGE *out, int format_table[10], im_wrapmany_fn fn, void *b) {
    IMAGE *t[5];

    if (im_piocheck(in1, out) || im_pincheck(in2) || im_check_bands_1orn(domain, in1, in2) || im_check_size_same(domain, in1, in2) || im_check_uncoded(domain, in1) || im_check_uncoded(domain, in2)) {
        return (-1);
    }

    if (im_open_local_array(out, t, 4, domain, "p") || im__formatalike(in1, in2, t[0], t[1]) || im__bandalike(t[0], t[1], t[2], t[3])) {
        return (-1);
    }

    if (im_cp_descv(out, t[2], t[3], NULL)) {
        return (-1);
    }

    out->Bands = t[2]->Bands;

    out->BandFmt = format_table[t[2]->BandFmt];

    t[4] = NULL;
    if (im_wrapmany(t + 2, out, fn, t[2], b)) {
        return (-1);
    }

    return (0);
}

static int bandfmt_add[10] = {

    US, S, UI, I, UI, I, F, X, D, DX};

int im_add(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    return (im__arith_binary("im_add", in1, in2, out, bandfmt_add, (im_wrapmany_fn)add_buffer, NULL));
}
