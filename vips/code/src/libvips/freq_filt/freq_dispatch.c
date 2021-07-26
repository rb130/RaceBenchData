
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdarg.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc one_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc two_in_one_out[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc create_fmask_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("width"), IM_INPUT_INT("height"), IM_INPUT_INT("type"), IM_INPUT_DOUBLE("p1"), IM_INPUT_DOUBLE("p2"), IM_INPUT_DOUBLE("p3"), IM_INPUT_DOUBLE("p4"), IM_INPUT_DOUBLE("p5")};

static int create_fmask_vec(im_object *argv) {
    int width = *((int *)argv[1]);
    int height = *((int *)argv[2]);
    int type = *((int *)argv[3]);
    double p1 = *((double *)argv[4]);
    double p2 = *((double *)argv[5]);
    double p3 = *((double *)argv[6]);
    double p4 = *((double *)argv[7]);
    double p5 = *((double *)argv[8]);

    return (im_create_fmask(argv[0], width, height, type, p1, p2, p3, p4, p5));
}

static im_function create_fmask_desc = {"im_create_fmask", "create frequency domain filter mask", 0, create_fmask_vec, IM_NUMBER(create_fmask_args), create_fmask_args};

static im_arg_desc flt_image_freq_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("type"), IM_INPUT_DOUBLE("p1"), IM_INPUT_DOUBLE("p2"), IM_INPUT_DOUBLE("p3"), IM_INPUT_DOUBLE("p4"), IM_INPUT_DOUBLE("p5")};

static int flt_image_freq_vec(im_object *argv) {
    int type = *((int *)argv[2]);
    double p1 = *((double *)argv[3]);
    double p2 = *((double *)argv[4]);
    double p3 = *((double *)argv[5]);
    double p4 = *((double *)argv[6]);
    double p5 = *((double *)argv[7]);

    return (im_flt_image_freq(argv[0], argv[1], type, p1, p2, p3, p4, p5));
}

static im_function flt_image_freq_desc = {"im_flt_image_freq", "frequency domain filter image", 0, flt_image_freq_vec, IM_NUMBER(flt_image_freq_args), flt_image_freq_args};

static im_arg_desc fractsurf_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("size"), IM_INPUT_DOUBLE("dimension")};

static int fractsurf_vec(im_object *argv) {
    int size = *((int *)argv[1]);
    double dim = *((double *)argv[2]);

    return (im_fractsurf(argv[0], size, dim));
}

static im_function fractsurf_desc = {"im_fractsurf", "generate a fractal surface of given dimension", IM_FN_TRANSFORM, fractsurf_vec, IM_NUMBER(fractsurf_args), fractsurf_args};

static im_arg_desc freqflt_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_IMAGE("mask"), IM_OUTPUT_IMAGE("out")};

static int freqflt_vec(im_object *argv) {
    return (im_freqflt(argv[0], argv[1], argv[2]));
}

static im_function freqflt_desc = {"im_freqflt", "frequency-domain filter of in with mask", IM_FN_TRANSFORM, freqflt_vec, IM_NUMBER(freqflt_args), freqflt_args};

static int disp_ps_vec(im_object *argv) {
    return (im_disp_ps(argv[0], argv[1]));
}

static im_function disp_ps_desc = {"im_disp_ps", "make displayable power spectrum", IM_FN_TRANSFORM, disp_ps_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int rotquad_vec(im_object *argv) {
    return (im_rotquad(argv[0], argv[1]));
}

static im_function rotquad_desc = {"im_rotquad", "rotate image quadrants to move origin to centre", IM_FN_TRANSFORM, rotquad_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int fwfft_vec(im_object *argv) {
    return (im_fwfft(argv[0], argv[1]));
}

static im_function fwfft_desc = {"im_fwfft", "forward fast-fourier transform", IM_FN_TRANSFORM, fwfft_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int invfft_vec(im_object *argv) {
    return (im_invfft(argv[0], argv[1]));
}

static im_function invfft_desc = {"im_invfft", "inverse fast-fourier transform", IM_FN_TRANSFORM, invfft_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int invfftr_vec(im_object *argv) {
    return (im_invfftr(argv[0], argv[1]));
}

static im_function invfftr_desc = {"im_invfftr", "real part of inverse fast-fourier transform", IM_FN_TRANSFORM, invfftr_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int phasecor_fft_vec(im_object *argv) {
    return (im_phasecor_fft(argv[0], argv[1], argv[2]));
}

static im_function phasecor_fft_desc = {"im_phasecor_fft", "non-normalised correlation of gradient of in2 within in1", IM_FN_TRANSFORM, phasecor_fft_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_function *freq_list[] = {&create_fmask_desc, &disp_ps_desc, &flt_image_freq_desc, &fractsurf_desc, &freqflt_desc, &fwfft_desc, &rotquad_desc, &invfft_desc, &phasecor_fft_desc, &invfftr_desc};

im_package im__freq_filt = {"freq_filt", IM_NUMBER(freq_list), freq_list};
