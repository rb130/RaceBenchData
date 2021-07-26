
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#ifdef HAVE_FFTW
#include <rfftw.h>
#endif

#ifdef HAVE_FFTW3
#include <fftw3.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef HAVE_FFTW

static int rfwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    const int size = in->Xsize * in->Ysize;
    const int half_width = in->Xsize / 2 + 1;

    IMAGE *real = im_open_local(dummy, "fwfft1:1", "t");

    double *half_complex = IM_ARRAY(dummy, in->Ysize * half_width * 2, double);

    rfftwnd_plan plan;
    double *buf, *q, *p;
    int x, y;

    if (!real || !half_complex || im_pincheck(in) || im_outcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_fwfft", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, real, IM_BANDFMT_DOUBLE))
        return (-1);

    if (!(plan = rfftw2d_create_plan(in->Ysize, in->Xsize, FFTW_FORWARD, FFTW_MEASURE | FFTW_USE_WISDOM))) {
        im_error("im_fwfft", _("unable to create transform plan"));
        return (-1);
    }

    rfftwnd_one_real_to_complex(plan, (fftw_real *)real->data, (fftw_complex *)half_complex);

    rfftwnd_destroy_plan(plan);

    if (im_cp_desc(out, in))
        return (-1);
    out->BandFmt = IM_BANDFMT_DPCOMPLEX;
    if (im_setupout(out))
        return (-1);
    if (!(buf = (double *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL)))
        return (-1);

    p = half_complex;
    q = buf;

    for (x = 0; x < half_width; x++) {
        q[0] = p[0] / size;
        q[1] = p[1] / size;
        p += 2;
        q += 2;
    }

    p = half_complex + ((in->Xsize + 1) / 2 - 1) * 2;

    for (x = half_width; x < out->Xsize; x++) {
        q[0] = p[0] / size;
        q[1] = -1.0 * p[1] / size;
        p -= 2;
        q += 2;
    }

    if (im_writeline(0, out, (PEL *)buf))
        return (-1);

    for (y = 1; y < out->Ysize; y++) {
        p = half_complex + y * half_width * 2;
        q = buf;

        for (x = 0; x < half_width; x++) {
            q[0] = p[0] / size;
            q[1] = p[1] / size;
            p += 2;
            q += 2;
        }

        p = half_complex + 2 * ((out->Ysize - y + 1) * half_width - 2 + (in->Xsize & 1));

        for (x = half_width; x < out->Xsize; x++) {
            q[0] = p[0] / size;
            q[1] = -1.0 * p[1] / size;
            p -= 2;
            q += 2;
        }

        if (im_writeline(y, out, (PEL *)buf))
            return (-1);
    }

    return (0);
}

static int cfwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    fftwnd_plan plan;
    double *buf, *q, *p;
    int x, y;

    IMAGE *cmplx = im_open_local(dummy, "fwfft1:1", "t");

    if (!cmplx || im_pincheck(in) || im_outcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_fwfft", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (!(plan = fftw2d_create_plan(in->Ysize, in->Xsize, FFTW_FORWARD, FFTW_MEASURE | FFTW_USE_WISDOM | FFTW_IN_PLACE))) {
        im_error("im_fwfft", _("unable to create transform plan"));
        return (-1);
    }

    fftwnd_one(plan, (fftw_complex *)cmplx->data, NULL);

    fftwnd_destroy_plan(plan);

    if (im_cp_desc(out, in))
        return (-1);
    out->BandFmt = IM_BANDFMT_DPCOMPLEX;
    if (im_setupout(out))
        return (-1);
    if (!(buf = (double *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL)))
        return (-1);

    for (p = (double *)cmplx->data, y = 0; y < out->Ysize; y++) {
        int size = out->Xsize * out->Ysize;

        q = buf;

        for (x = 0; x < out->Xsize; x++) {
            q[0] = p[0] / size;
            q[1] = p[1] / size;
            p += 2;
            q += 2;
        }

        if (im_writeline(y, out, (PEL *)buf))
            return (-1);
    }

    return (0);
}

static int fwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    if (im_iscomplex(in))
        return (cfwfft1(dummy, in, out));
    else
        return (rfwfft1(dummy, in, out));
}
#else
#ifdef HAVE_FFTW3

static int rfwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    const int size = in->Xsize * in->Ysize;
    const int half_width = in->Xsize / 2 + 1;

    IMAGE *real = im_open_local(dummy, "fwfft1:1", "t");

    double *half_complex = IM_ARRAY(dummy, in->Ysize * half_width * 2, double);

    double *planner_scratch = IM_ARRAY(dummy, in->Xsize * in->Ysize, double);

    fftw_plan plan;
    double *buf, *q, *p;
    int x, y;

    if (!real || !half_complex || im_pincheck(in) || im_outcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_fwfft", "%s", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, real, IM_BANDFMT_DOUBLE))
        return (-1);

    if (!(plan = fftw_plan_dft_r2c_2d(in->Ysize, in->Xsize, planner_scratch, (fftw_complex *)half_complex, 0))) {
        im_error("im_fwfft", "%s", _("unable to create transform plan"));
        return (-1);
    }

    fftw_execute_dft_r2c(plan, (double *)real->data, (fftw_complex *)half_complex);

    fftw_destroy_plan(plan);

    if (im_cp_desc(out, in))
        return (-1);
    out->BandFmt = IM_BANDFMT_DPCOMPLEX;
    if (im_setupout(out))
        return (-1);
    if (!(buf = (double *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL)))
        return (-1);

    p = half_complex;
    q = buf;

    for (x = 0; x < half_width; x++) {
        q[0] = p[0] / size;
        q[1] = p[1] / size;
        p += 2;
        q += 2;
    }

    p = half_complex + ((in->Xsize + 1) / 2 - 1) * 2;

    for (x = half_width; x < out->Xsize; x++) {
        q[0] = p[0] / size;
        q[1] = -1.0 * p[1] / size;
        p -= 2;
        q += 2;
    }

    if (im_writeline(0, out, (PEL *)buf))
        return (-1);

    for (y = 1; y < out->Ysize; y++) {
        p = half_complex + y * half_width * 2;
        q = buf;

        for (x = 0; x < half_width; x++) {
            q[0] = p[0] / size;
            q[1] = p[1] / size;
            p += 2;
            q += 2;
        }

        p = half_complex + 2 * ((out->Ysize - y + 1) * half_width - 2 + (in->Xsize & 1));

        for (x = half_width; x < out->Xsize; x++) {
            q[0] = p[0] / size;
            q[1] = -1.0 * p[1] / size;
            p -= 2;
            q += 2;
        }

        if (im_writeline(y, out, (PEL *)buf))
            return (-1);
    }

    return (0);
}

static int cfwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    fftw_plan plan;
    double *buf, *q, *p;
    int x, y;

    IMAGE *cmplx = im_open_local(dummy, "fwfft1:1", "t");

    double *planner_scratch = IM_ARRAY(dummy, in->Xsize * in->Ysize * 2, double);

    if (!cmplx || im_pincheck(in) || im_outcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_fwfft", "%s", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (!(plan = fftw_plan_dft_2d(in->Ysize, in->Xsize, (fftw_complex *)planner_scratch, (fftw_complex *)planner_scratch, FFTW_FORWARD, 0))) {
        im_error("im_fwfft", "%s", _("unable to create transform plan"));
        return (-1);
    }

    fftw_execute_dft(plan, (fftw_complex *)cmplx->data, (fftw_complex *)cmplx->data);

    fftw_destroy_plan(plan);

    if (im_cp_desc(out, in))
        return (-1);
    out->BandFmt = IM_BANDFMT_DPCOMPLEX;
    if (im_setupout(out))
        return (-1);
    if (!(buf = (double *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL)))
        return (-1);

    for (p = (double *)cmplx->data, y = 0; y < out->Ysize; y++) {
        int size = out->Xsize * out->Ysize;

        q = buf;

        for (x = 0; x < out->Xsize; x++) {
            q[0] = p[0] / size;
            q[1] = p[1] / size;
            p += 2;
            q += 2;
        }

        if (im_writeline(y, out, (PEL *)buf))
            return (-1);
    }

    return (0);
}

static int fwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    if (vips_bandfmt_iscomplex(in->BandFmt))
        return (cfwfft1(dummy, in, out));
    else
        return (rfwfft1(dummy, in, out));
}
#else

static int fwfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    int size = in->Xsize * in->Ysize;
    int bpx = im_ispoweroftwo(in->Xsize);
    int bpy = im_ispoweroftwo(in->Ysize);
    float *buf, *q, *p1, *p2;
    int x, y;

    IMAGE *real = im_open_local(dummy, "fwfft1:1", "t");
    IMAGE *imag = im_open_local(dummy, "fwfft1:2", "t");

    IMAGE *t1 = im_open_local(dummy, "fwfft1:3", "p");

    if (!real || !imag || !t1) {
        return (-1);
    }
    if (im_pincheck(in) || im_outcheck(out)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE || in->Bands != 1 || im_iscomplex(in)) {
        im_error("im_fwfft", "%s", _("one band non-complex uncoded only"));
        return (-1);
    }
    if (!bpx || !bpy) {
        im_error("im_fwfft", "%s", _("sides must be power of 2"));
        return (-1);
    }

    if (im_clip2fmt(in, real, IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im_black(t1, in->Xsize, in->Ysize, 1)) {
        return (-1);
    }
    if (im_clip2fmt(t1, imag, IM_BANDFMT_FLOAT)) {
        return (-1);
    }

    if (im__fft_sp((float *)real->data, (float *)imag->data, bpx - 1, bpy - 1)) {
        im_error("im_fwfft", "%s", _("fft_sp failed"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_COMPLEX;
    if (im_setupout(out)) {
        return (-1);
    }
    if (!(buf = (float *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL))) {
        return (-1);
    }

    for (p1 = (float *)real->data, p2 = (float *)imag->data, y = 0; y < out->Ysize; y++) {
        q = buf;

        for (x = 0; x < out->Xsize; x++) {
            q[0] = *p1++ / size;
            q[1] = *p2++ / size;
            q += 2;
        }

        if (im_writeline(y, out, (PEL *)buf)) {
            return (-1);
        }
    }

    return (0);
}
#endif
#endif

int im__fftproc(IMAGE *dummy, IMAGE *in, IMAGE *out, im__fftproc_fn fn) {
    IMAGE **bands;
    IMAGE **fft;
    IMAGE *t;
    int b;

    if (in->Bands == 1) {
        return (fn(dummy, in, out));
    }

    if (!(bands = IM_ARRAY(dummy, in->Bands, IMAGE *)) || !(fft = IM_ARRAY(dummy, in->Bands, IMAGE *)) || im_open_local_array(dummy, bands, in->Bands, "bands", "p") || im_open_local_array(dummy, fft, in->Bands, "fft", "p")) {
        return (-1);
    }

    for (b = 0; b < in->Bands; b++) {
        if (im_extract_band(in, bands[b], b) || fn(dummy, bands[b], fft[b])) {
            return (-1);
        }
    }

    if (!(t = im_open_local(out, "im__fftproc", "t")) || im_gbandjoin(fft, t, in->Bands) || im_copy(t, out)) {
        return (-1);
    }

    return (0);
}

int im_fwfft(IMAGE *in, IMAGE *out) {
    IMAGE *dummy;

    if (!(dummy = im_open("im_fwfft:1", "p"))) {
        return (-1);
    }
    if (im__fftproc(dummy, in, out, fwfft1)) {
        im_close(dummy);
        return (-1);
    }
    im_close(dummy);

    out->Type = IM_TYPE_FOURIER;

    return (0);
}
