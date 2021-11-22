

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#ifdef HAVE_FFTW
#include <fftw.h>
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
    fftwnd_plan plan;

    IMAGE *cmplx = im_open_local(out, "invfft1:1", "t");

    if (!cmplx || im_pincheck(in) || im_poutcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_invfft", "%s", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (!(plan = fftw2d_create_plan(in->Ysize, in->Xsize, FFTW_BACKWARD, FFTW_MEASURE | FFTW_USE_WISDOM | FFTW_IN_PLACE))) {
        im_error("im_invfft", "%s", _("unable to create transform plan"));
        return (-1);
    }

    fftwnd_one(plan, (fftw_complex *)cmplx->data, NULL);

    fftwnd_destroy_plan(plan);

    if (im_copy(cmplx, out))
        return (-1);

    return (0);
}
#else
#ifdef HAVE_FFTW3

static int invfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    fftw_plan plan;

    IMAGE *cmplx = im_open_local(out, "invfft1:1", "t");

    double *planner_scratch = IM_ARRAY(dummy, in->Xsize * in->Ysize * 2, double);

    if (!cmplx || im_pincheck(in) || im_poutcheck(out))
        return (-1);
    if (in->Coding != IM_CODING_NONE || in->Bands != 1) {
        im_error("im_invfft", "%s", _("one band uncoded only"));
        return (-1);
    }
    if (im_clip2fmt(in, cmplx, IM_BANDFMT_DPCOMPLEX))
        return (-1);

    if (!(plan = fftw_plan_dft_2d(in->Ysize, in->Xsize, (fftw_complex *)planner_scratch, (fftw_complex *)planner_scratch, FFTW_BACKWARD, 0))) {
        im_error("im_invfft", "%s", _("unable to create transform plan"));
        return (-1);
    }

    fftw_execute_dft(plan, (fftw_complex *)cmplx->data, (fftw_complex *)cmplx->data);

    fftw_destroy_plan(plan);

    if (im_copy(cmplx, out))
        return (-1);

    return (0);
}
#else

static int invfft1(IMAGE *dummy, IMAGE *in, IMAGE *out) {
    int bpx = im_ispoweroftwo(in->Xsize);
    int bpy = im_ispoweroftwo(in->Ysize);
    float *buf, *q, *p1, *p2;
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
            q[0] = *p1++;
            q[1] = *p2++;
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

int im_invfft(IMAGE *in, IMAGE *out) {
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
