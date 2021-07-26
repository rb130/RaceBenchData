
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

static int invfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    IMAGE *cmplx = im_open_local(dummy, "invfft1-1", "t");
    IMAGE *real = im_open_local(out, "invfft1-2", "t");
    const int half_width = in->Xsize / 2 + 1;

    double *half_complex = IM_ARRAY(dummy, in->Ysize * half_width * 2, double);

    rfftwnd_plan plan;
    int x, y;
    double *q, *p;

    if (!cmplx || !real || !half_complex || im_pincheck(in) || im_poutcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_invfft", _("one band uncoded only"));
        return (-1);
    }

    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (im_cp_desc(real, in))
        return (-1);
    real->BandFmt = IM_BANDFMT_DOUBLE;
    if (im_setupout(real))
        return (-1);

    q = half_complex;
    for (y = 0; y < cmplx->Ysize; y++) {
        p = ((double *)cmplx->data) + y * in->Xsize * 2;

        for (x = 0; x < half_width; x++) {
            q[0] = p[0];
            q[1] = p[1];
            p += 2;
            q += 2;
        }
    }

    if (!(plan = rfftw2d_create_plan(in->Ysize, in->Xsize, FFTW_BACKWARD, FFTW_MEASURE | FFTW_USE_WISDOM))) {
        im_error("im_invfft", _("unable to create transform plan"));
        return (-1);
    }

    rfftwnd_one_complex_to_real(plan, (fftw_complex *)half_complex, (fftw_real *)real->data);

    rfftwnd_destroy_plan(plan);

    if (im_copy(real, out))
        return (-1);

    return (0);
}
#else
#ifdef HAVE_FFTW3

static int invfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    IMAGE *cmplx = im_open_local(dummy, "invfft1-1", "t");
    IMAGE *real = im_open_local(out, "invfft1-2", "t");
    const int half_width = in->Xsize / 2 + 1;

    double *half_complex = IM_ARRAY(dummy, in->Ysize * half_width * 2, double);

    double *planner_scratch = IM_ARRAY(dummy, in->Ysize * half_width * 2, double);

    fftw_plan plan;
    int x, y;
    double *q, *p;

    if (!cmplx || !real || !half_complex || im_pincheck(in) || im_poutcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_invfft", "%s", _("one band uncoded only"));
        return (-1);
    }

    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (im_cp_desc(real, in))
        return (-1);
    real->BandFmt = IM_BANDFMT_DOUBLE;
    if (im_setupout(real))
        return (-1);

    q = half_complex;
    for (y = 0; y < cmplx->Ysize; y++) {
        p = ((double *)cmplx->data) + y * in->Xsize * 2;

        for (x = 0; x < half_width; x++) {
            q[0] = p[0];
            q[1] = p[1];
            p += 2;
            q += 2;
        }
    }

    if (!(plan = fftw_plan_dft_c2r_2d(in->Ysize, in->Xsize, (fftw_complex *)planner_scratch, (double *)real->data, 0))) {
        im_error("im_invfft", "%s", _("unable to create transform plan"));
        return (-1);
    }

    fftw_execute_dft_c2r(plan, (fftw_complex *)half_complex, (double *)real->data);

    fftw_destroy_plan(plan);

    if (im_copy(real, out))
        return (-1);

    return (0);
}
#else

static int invfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    int bpx = im_ispoweroftwo(in->Xsize);
    int bpy = im_ispoweroftwo(in->Ysize);
    float *buf, *q, *p1;
    int x, y;

    IMAGE *real = im_open_local(dummy, "invfft1:1", "t");
    IMAGE *imag = im_open_local(dummy, "invfft1:2", "t");

    IMAGE *t1 = im_open_local(dummy, "invfft1:3", "p");
    IMAGE *t2 = im_open_local(dummy, "invfft1:4", "p");

    if (!real || !imag || !t1) {
        return (-1);
    }
    if (im_pincheck(in) || im_outcheck(out)) {
        return (-1);
    }
    if (in->Coding != IM_CODING_NONE || in->Bands != 1 || !im_iscomplex(in)) {
        im_error("im_invfft", "%s", _("one band complex uncoded only"));
        return (-1);
    }
    if (!bpx || !bpy) {
        im_error("im_invfft", "%s", _("sides must be power of 2"));
        return (-1);
    }

    if (im_clip2fmt(in, t1, IM_BANDFMT_COMPLEX)) {
        return (-1);
    }

    if (im_c2real(t1, real)) {
        return (-1);
    }
    if (im_c2imag(t1, t2) || im_lintra(-1.0, t2, 0.0, imag)) {
        return (-1);
    }

    if (im__fft_sp((float *)real->data, (float *)imag->data, bpx - 1, bpy - 1)) {
        im_error("im_invfft", "%s", _("fft_sp failed"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_FLOAT;
    if (im_setupout(out)) {
        return (-1);
    }
    if (!(buf = (float *)IM_ARRAY(dummy, IM_IMAGE_SIZEOF_LINE(out), PEL))) {
        return (-1);
    }

    for (p1 = (float *)real->data, y = 0; y < out->Ysize; y++) {
        q = buf;

        for (x = 0; x < out->Xsize; x++) {
            q[x] = *p1++;
        }

        if (im_writeline(y, out, (PEL *)buf)) {
            return (-1);
        }
    }

    return (0);
}
#endif
#endif

int im_invfftr(IMAGE *in, IMAGE *out) {
    IMAGE *dummy = im_open("im_invfft:1", "p");

    if (!dummy) {
        return (-1);
    }
    if (im__fftproc(dummy, in, out, invfft1)) {
        im_close(dummy);
        return (-1);
    }
    im_close(dummy);

    if (out->Bands == 1) {
        out->Type = IM_TYPE_B_W;
    } else {
        out->Type = IM_TYPE_MULTIBAND;
    }

    return (0);
}
