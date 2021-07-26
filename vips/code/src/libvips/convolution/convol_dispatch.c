
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc one_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc two_in_one_out[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc addgnoise_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("sigma")};

static int addgnoise_vec(im_object *argv) {
    double sigma = *((double *)argv[2]);

    return (im_addgnoise(argv[0], argv[1], sigma));
}

static im_function addgnoise_desc = {"im_addgnoise", "add gaussian noise with mean 0 and std. dev. sigma", IM_FN_PIO, addgnoise_vec, IM_NUMBER(addgnoise_args), addgnoise_args};

static im_arg_desc contrast_surface_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("half_win_size"), IM_INPUT_INT("spacing")};

static int contrast_surface_vec(im_object *argv) {
    int half_win_size = *((int *)argv[2]);
    int spacing = *((int *)argv[3]);

    return (im_contrast_surface(argv[0], argv[1], half_win_size, spacing));
}

static im_function contrast_surface_desc = {"im_contrast_surface", "find high-contrast points in an image", IM_FN_PIO, contrast_surface_vec, IM_NUMBER(contrast_surface_args), contrast_surface_args};

static im_arg_desc sharpen_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("mask_size"), IM_INPUT_DOUBLE("x1"), IM_INPUT_DOUBLE("y2"), IM_INPUT_DOUBLE("y3"), IM_INPUT_DOUBLE("m1"), IM_INPUT_DOUBLE("m2")};

static int sharpen_vec(im_object *argv) {
    int mask_size = *((int *)argv[2]);
    double x1 = *((double *)argv[3]);
    double x2 = *((double *)argv[4]);
    double x3 = *((double *)argv[5]);
    double m1 = *((double *)argv[6]);
    double m2 = *((double *)argv[7]);

    return (im_sharpen(argv[0], argv[1], mask_size, x1, x2, x3, m1, m2));
}

static im_function sharpen_desc = {"im_sharpen", "sharpen high frequencies of L channel of LabQ", IM_FN_PIO, sharpen_vec, IM_NUMBER(sharpen_args), sharpen_args};

static im_arg_desc conv_imask[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMASK("matrix")};

static im_arg_desc conv_dmask[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DMASK("matrix")};

static int compass_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_compass(argv[0], argv[1], mo->mask));
}

static im_function compass_desc = {"im_compass", "convolve with 8-way rotating integer mask", IM_FN_PIO | IM_FN_TRANSFORM, compass_vec, IM_NUMBER(conv_imask), conv_imask};

static int conv_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_conv(argv[0], argv[1], mo->mask));
}

static im_function conv_desc = {"im_conv", "convolve", IM_FN_TRANSFORM | IM_FN_PIO, conv_vec, IM_NUMBER(conv_imask), conv_imask};

static int conv_f_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_conv_f(argv[0], argv[1], mo->mask));
}

static im_function conv_f_desc = {"im_conv_f", "convolve, with DOUBLEMASK", IM_FN_TRANSFORM | IM_FN_PIO, conv_f_vec, IM_NUMBER(conv_dmask), conv_dmask};

static int convsep_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convsep(argv[0], argv[1], mo->mask));
}

static im_function convsep_desc = {"im_convsep", "seperable convolution", IM_FN_TRANSFORM | IM_FN_PIO, convsep_vec, IM_NUMBER(conv_imask), conv_imask};

static int convsep_f_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convsep_f(argv[0], argv[1], mo->mask));
}

static im_function convsep_f_desc = {"im_convsep_f", "seperable convolution, with DOUBLEMASK", IM_FN_PIO | IM_FN_TRANSFORM, convsep_f_vec, IM_NUMBER(conv_dmask), conv_dmask};

static int fastcor_vec(im_object *argv) {
    return (im_fastcor(argv[0], argv[1], argv[2]));
}

static im_function fastcor_desc = {"im_fastcor", "fast correlate in2 within in1", IM_FN_TRANSFORM | IM_FN_PIO, fastcor_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int grad_x_vec(im_object *argv) {
    return (im_grad_x(argv[0], argv[1]));
}

static im_function grad_x_desc = {"im_grad_x", "horizontal difference image", IM_FN_PIO | IM_FN_TRANSFORM, grad_x_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int grad_y_vec(im_object *argv) {
    return (im_grad_y(argv[0], argv[1]));
}

static im_function grad_y_desc = {"im_grad_y", "vertical difference image", IM_FN_PIO | IM_FN_TRANSFORM, grad_y_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int gradcor_vec(im_object *argv) {
    return (im_gradcor(argv[0], argv[1], argv[2]));
}

static im_function gradcor_desc = {"im_gradcor", "non-normalised correlation of gradient of in2 within in1", IM_FN_PIO | IM_FN_TRANSFORM, gradcor_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int gradient_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_gradient(argv[0], argv[1], mo->mask));
}

static im_function gradient_desc = {"im_gradient", "convolve with 2-way rotating mask", IM_FN_PIO | IM_FN_TRANSFORM, gradient_vec, IM_NUMBER(conv_imask), conv_imask};

static int lindetect_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_lindetect(argv[0], argv[1], mo->mask));
}

static im_function lindetect_desc = {"im_lindetect", "convolve with 4-way rotating mask", IM_FN_PIO | IM_FN_TRANSFORM, lindetect_vec, IM_NUMBER(conv_imask), conv_imask};

static int spcor_vec(im_object *argv) {
    return (im_spcor(argv[0], argv[1], argv[2]));
}

static im_function spcor_desc = {"im_spcor", "normalised correlation of in2 within in1", IM_FN_PIO | IM_FN_TRANSFORM, spcor_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_function *convol_list[] = {
    &addgnoise_desc,
    &compass_desc,
    &contrast_surface_desc,
    &conv_desc,
    &conv_f_desc,
    &convsep_desc,
    &convsep_f_desc,
    &fastcor_desc,
    &gradcor_desc,
    &gradient_desc,
    &grad_x_desc,
    &grad_y_desc,
    &lindetect_desc,
    &sharpen_desc,
    &spcor_desc,
};

im_package im__convolution = {"convolution", IM_NUMBER(convol_list), convol_list};
