

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define GEN_FUNC(SHIFT_MACRO, FROM_T, TO_T, SUM_T) static int gen_##SHIFT_MACRO##_##FROM_T##_to_##TO_T##_with_##SUM_T(REGION *to_make, void *seq, void *a, void *b);

#define GEN_FUNCS_TO(SIGN, FROM_T, TO_T)             \
    GEN_FUNC(PRE_POST_SHIFT, FROM_T, TO_T, SIGN##64) \
    GEN_FUNC(POST_SHIFT, FROM_T, TO_T, SIGN##64)     \
    GEN_FUNC(POST_SHIFT, FROM_T, TO_T, SIGN##32)     \
    GEN_FUNC(NO_SHIFT, FROM_T, TO_T, SIGN##32)

#define GEN_FUNCS_FROM(SIGN, FROM_T)     \
    GEN_FUNCS_TO(SIGN, FROM_T, SIGN##32) \
    GEN_FUNCS_TO(SIGN, FROM_T, SIGN##16) \
    GEN_FUNCS_TO(SIGN, FROM_T, SIGN##8)

#define GEN_FUNCS_SIGN(SIGN)       \
    GEN_FUNCS_FROM(SIGN, SIGN##32) \
    GEN_FUNCS_FROM(SIGN, SIGN##16) \
    GEN_FUNCS_FROM(SIGN, SIGN##8)

GEN_FUNCS_SIGN(gint)
GEN_FUNCS_SIGN(guint)

int im_rightshift_size(IMAGE *in, IMAGE *out, int xshift, int yshift, int band_fmt) {
#define FUNCTION_NAME "im_rightshift_size"
    int *params;
    int needbits;
    int outbits;

    if (im_piocheck(in, out)) {
        return -1;
    }

    if (xshift < 0 || yshift < 0) {
        im_error(FUNCTION_NAME, "%s", _("bad arguments"));
        return -1;
    }
    if (!xshift && !yshift) {
        im_warn(FUNCTION_NAME, "%s", _("shift by zero: falling back to im_copy"));
        return im_copy(in, out);
    }
    if (!in->Xsize >> xshift || !in->Ysize >> yshift) {
        im_error(FUNCTION_NAME, "%s", _("would result in zero size output image"));
        return -1;
    }
    if (!vips_bandfmt_isint(in->BandFmt)) {
        im_error(FUNCTION_NAME, "%s", _("integer type images only"));
        return -1;
    }
    if (IM_CODING_NONE != in->Coding) {
        im_error(FUNCTION_NAME, "%s", _("uncoded images only"));
        return -1;
    }
    if (vips_bandfmt_isuint(in->BandFmt)) {
        if (IM_BANDFMT_UCHAR != band_fmt && IM_BANDFMT_USHORT != band_fmt && IM_BANDFMT_UINT != band_fmt) {
            im_error(FUNCTION_NAME, "%s", _("unsigned input means that output must be unsigned int, short or char"));
            return -1;
        }
    } else {
        if (IM_BANDFMT_CHAR != band_fmt && IM_BANDFMT_SHORT != band_fmt && IM_BANDFMT_INT != band_fmt) {
            im_error(FUNCTION_NAME, "%s", _("signed input means that output must be signed int, short or char"));
            return -1;
        }
    }
    outbits = im_bits_of_fmt(band_fmt);

    if (-1 == outbits) {
        return -1;
    }

    needbits = im_bits_of_fmt(in->BandFmt) + xshift + yshift;

    params = IM_ARRAY(out, 4, int);

    if (!params) {
        return -1;
    }

    params[0] = xshift;
    params[1] = yshift;

    if (im_cp_desc(out, in)) {
        return -1;
    }

    out->Xsize >>= xshift;
    out->Ysize >>= yshift;
    out->Xres /= (1 << xshift);
    out->Yres /= (1 << yshift);
    out->BandFmt = band_fmt;

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return -1;
    }

#define RETURN_MACRO(MACRO, FROM_T, TO_T, SUM_T) return im_generate(out, im_start_one, gen_##MACRO##_##FROM_T##_to_##TO_T##_with_##SUM_T, im_stop_one, in, params);

#define RETURN_MACRO_OUTS(MACRO, SUM_SIZE, OUT_SIZE)                   \
    switch (in->BandFmt) {                                             \
                                                                       \
    case IM_BANDFMT_CHAR:                                              \
        RETURN_MACRO(MACRO, gint8, gint##OUT_SIZE, gint##SUM_SIZE)     \
                                                                       \
    case IM_BANDFMT_UCHAR:                                             \
        RETURN_MACRO(MACRO, guint8, guint##OUT_SIZE, guint##SUM_SIZE)  \
                                                                       \
    case IM_BANDFMT_SHORT:                                             \
        RETURN_MACRO(MACRO, gint16, gint##OUT_SIZE, gint##SUM_SIZE)    \
                                                                       \
    case IM_BANDFMT_USHORT:                                            \
        RETURN_MACRO(MACRO, guint16, guint##OUT_SIZE, guint##SUM_SIZE) \
                                                                       \
    case IM_BANDFMT_INT:                                               \
        RETURN_MACRO(MACRO, gint32, gint##OUT_SIZE, gint##SUM_SIZE)    \
                                                                       \
    case IM_BANDFMT_UINT:                                              \
        RETURN_MACRO(MACRO, guint32, guint##OUT_SIZE, guint##SUM_SIZE) \
    default:                                                           \
        g_assert(0);                                                   \
    }

#define RETURN_MACRO_SUMSS(MACRO, SUM_SIZE)    \
    switch (im_bits_of_fmt(out->BandFmt)) {    \
    case 32:                                   \
        RETURN_MACRO_OUTS(MACRO, SUM_SIZE, 32) \
    case 16:                                   \
        RETURN_MACRO_OUTS(MACRO, SUM_SIZE, 16) \
    case 8:                                    \
        RETURN_MACRO_OUTS(MACRO, SUM_SIZE, 8)  \
    }

    if (needbits > 64) {
        params[2] = needbits - 64;
        params[3] = 64 - outbits;
        RETURN_MACRO_SUMSS(PRE_POST_SHIFT, 64)
    }
    if (needbits > 32) {
        params[3] = needbits - outbits;
        RETURN_MACRO_SUMSS(POST_SHIFT, 64)
    }
    if (needbits > outbits) {
        params[3] = needbits - outbits;
        RETURN_MACRO_SUMSS(POST_SHIFT, 32)
    }

    RETURN_MACRO_SUMSS(NO_SHIFT, 32)

    return 0;

#undef FUNCTION_NAME
}

#define PRE_POST_SHIFT()           \
    pixel += from[fx] >> preshift; \
    to[tx] = pixel >> postshift;

#define POST_SHIFT()   \
    pixel += from[fx]; \
    to[tx] = pixel >> postshift;

#define NO_SHIFT()     \
    pixel += from[fx]; \
    to[tx] = pixel;

#undef GEN_FUNC
#define GEN_FUNC(SHIFT_MACRO, FROM_T, TO_T, SUM_T)                                                                                                  \
    static int gen_##SHIFT_MACRO##_##FROM_T##_to_##TO_T##_with_##SUM_T(REGION *to_make, void *seq, void *a, void *b) {                              \
                                                                                                                                                    \
        REGION *make_from = (REGION *)seq;                                                                                                          \
        int *params = (int *)b;                                                                                                                     \
        int xshift = params[0];                                                                                                                     \
        int yshift = params[1];                                                                                                                     \
        int G_GNUC_UNUSED preshift = params[2];                                                                                                     \
        int G_GNUC_UNUSED postshift = params[3];                                                                                                    \
        Rect need = {to_make->valid.left << xshift, to_make->valid.top << yshift, to_make->valid.width << xshift, to_make->valid.height << yshift}; \
        TO_T *to_start = (TO_T *)IM_REGION_ADDR(to_make, to_make->valid.left, to_make->valid.top);                                                  \
        TO_T *to, *ty_stop;                                                                                                                         \
        SUM_T pixel;                                                                                                                                \
        FROM_T *from_start, *from_row, *from, *fy_stop;                                                                                             \
                                                                                                                                                    \
        int band, tx, fx;                                                                                                                           \
        int tx_max = to_make->valid.width * to_make->im->Bands;                                                                                     \
        int fx_max = to_make->im->Bands << xshift;                                                                                                  \
                                                                                                                                                    \
        size_t t_skip = IM_REGION_LSKIP(to_make) / sizeof(TO_T);                                                                                    \
        size_t f_skip;                                                                                                                              \
                                                                                                                                                    \
        if (im_prepare(make_from, &need) || !im_rect_includesrect(&make_from->valid, &need))                                                        \
            return -1;                                                                                                                              \
                                                                                                                                                    \
        from_start = (FROM_T *)IM_REGION_ADDR(make_from, need.left, need.top);                                                                      \
        f_skip = IM_REGION_LSKIP(make_from) / sizeof(FROM_T);                                                                                       \
                                                                                                                                                    \
        for (band = 0; band < make_from->im->Bands; ++band) {                                                                                       \
                                                                                                                                                    \
            to = to_start + band;                                                                                                                   \
            ty_stop = to + t_skip * to_make->valid.height;                                                                                          \
                                                                                                                                                    \
            from_row = from_start + band;                                                                                                           \
                                                                                                                                                    \
            for (; to < ty_stop; to += t_skip, from_row += f_skip << yshift)                                                                        \
                for (tx = 0; tx < tx_max; tx += to_make->im->Bands) {                                                                               \
                                                                                                                                                    \
                    from = from_row + (tx << xshift);                                                                                               \
                    fy_stop = from + (f_skip << yshift);                                                                                            \
                                                                                                                                                    \
                    pixel = 0;                                                                                                                      \
                                                                                                                                                    \
                    for (; from < fy_stop; from += f_skip)                                                                                          \
                        for (fx = 0; fx < fx_max; fx += to_make->im->Bands)                                                                         \
                            SHIFT_MACRO()                                                                                                           \
                }                                                                                                                                   \
        }                                                                                                                                           \
        return 0;                                                                                                                                   \
    }

GEN_FUNCS_SIGN(gint)
GEN_FUNCS_SIGN(guint)
