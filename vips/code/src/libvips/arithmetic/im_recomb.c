
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define LOOP(IN, OUT)                            \
    {                                            \
        IN *p = (IN *)bin;                       \
        OUT *q = (OUT *)bout;                    \
                                                 \
        for (i = 0; i < width; i++) {            \
            double *m = mat->coeff;              \
                                                 \
            for (v = 0; v < mat->ysize; v++) {   \
                double t = 0.0;                  \
                                                 \
                for (u = 0; u < mat->xsize; u++) \
                    t += *m++ * p[u];            \
                                                 \
                *q++ = (OUT)t;                   \
            }                                    \
                                                 \
            p += mat->xsize;                     \
        }                                        \
    }

static int recomb_buf(void *bin, void *bout, int width, IMAGE *in, DOUBLEMASK *mat) {
    int i;
    int u, v;

    switch (in->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char, float);
        break;
    case IM_BANDFMT_CHAR:
        LOOP(signed char, float);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short, float);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short, float);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int, float);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int, float);
        break;
    case IM_BANDFMT_FLOAT:
        LOOP(float, float);
        break;
    case IM_BANDFMT_DOUBLE:
        LOOP(double, double);
        break;

    default:
        g_assert(0);
    }

    return (0);
}

int im_recomb(IMAGE *in, IMAGE *out, DOUBLEMASK *recomb) {
    DOUBLEMASK *mcpy;

    if (im_piocheck(in, out) || im_check_uncoded("im_recomb", in) || im_check_noncomplex("im_recomb", in)) {
        return (-1);
    }
    if (in->Bands != recomb->xsize) {
        im_error("im_recomb", "%s", _("bands in must equal matrix width"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = recomb->ysize;
    if (vips_bandfmt_isint(in->BandFmt)) {
        out->BandFmt = IM_BANDFMT_FLOAT;
    }

    if (!(mcpy = im_dup_dmask(recomb, "conv_mask"))) {
        return (-1);
    }
    if (im_add_close_callback(out, (im_callback_fn)im_free_dmask, mcpy, NULL)) {
        im_free_dmask(mcpy);
        return (-1);
    }

    if (im_wrapone(in, out, (im_wrapone_fn)recomb_buf, in, mcpy)) {
        return (-1);
    }

    return (0);
}
