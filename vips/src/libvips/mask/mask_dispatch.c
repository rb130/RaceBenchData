

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc one_in_one_out[] = {IM_INPUT_DMASK("in"), IM_OUTPUT_DMASK("out")};

static im_arg_desc two_in_one_out[] = {IM_INPUT_DMASK("in1"), IM_INPUT_DMASK("in2"), IM_OUTPUT_DMASK("out")};

static int matinv_vec(im_object *argv) {
    im_mask_object *in = argv[0];
    im_mask_object *out = argv[1];

    if (!(out->mask = im_matinv(in->mask, out->name))) {
        return (-1);
    }

    return (0);
}

static im_function matinv_desc = {"im_matinv", "invert matrix", 0, matinv_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int mattrn_vec(im_object *argv) {
    im_mask_object *in = argv[0];
    im_mask_object *out = argv[1];

    if (!(out->mask = im_mattrn(in->mask, out->name))) {
        return (-1);
    }

    return (0);
}

static im_function mattrn_desc = {"im_mattrn", "transpose matrix", 0, mattrn_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int matcat_vec(im_object *argv) {
    im_mask_object *in1 = argv[0];
    im_mask_object *in2 = argv[1];
    im_mask_object *out = argv[2];

    if (!(out->mask = im_matcat(in1->mask, in2->mask, out->name))) {
        return (-1);
    }

    return (0);
}

static im_function matcat_desc = {"im_matcat", "append matrix in2 to the end of matrix in1", 0, matcat_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int matmul_vec(im_object *argv) {
    im_mask_object *in1 = argv[0];
    im_mask_object *in2 = argv[1];
    im_mask_object *out = argv[2];

    if (!(out->mask = im_matmul(in1->mask, in2->mask, out->name))) {
        return (-1);
    }

    return (0);
}

static im_function matmul_desc = {"im_matmul", "multiply matrix in1 by matrix in2", 0, matmul_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc read_dmask_args[] = {IM_INPUT_STRING("filename"), IM_OUTPUT_DMASK("mask")};

static int read_dmask_vec(im_object *argv) {
    im_mask_object *mo = argv[1];

    if (!(mo->mask = im_read_dmask(argv[0]))) {
        return (-1);
    }

    return (0);
}

static im_function read_dmask_desc = {"im_read_dmask", "read matrix of double from file", 0, read_dmask_vec, IM_NUMBER(read_dmask_args), read_dmask_args};

static im_arg_desc gauss_dmask_args[] = {IM_OUTPUT_DMASK("mask"), IM_INPUT_DOUBLE("sigma"), IM_INPUT_DOUBLE("min_amp")};

static int gauss_dmask_vec(im_object *argv) {
    im_mask_object *mo = argv[0];
    double sigma = *((double *)argv[1]);
    double min_amp = *((double *)argv[2]);

    if (!(mo->mask = im_gauss_dmask(mo->name, sigma, min_amp))) {
        return (-1);
    }

    return (0);
}

static im_function gauss_dmask_desc = {"im_gauss_dmask", "generate gaussian DOUBLEMASK", 0, gauss_dmask_vec, IM_NUMBER(gauss_dmask_args), gauss_dmask_args};

static im_arg_desc gauss_imask_args[] = {IM_OUTPUT_IMASK("mask"), IM_INPUT_DOUBLE("sigma"), IM_INPUT_DOUBLE("min_amp")};

static int gauss_imask_vec(im_object *argv) {
    im_mask_object *mo = argv[0];
    double sigma = *((double *)argv[1]);
    double min_amp = *((double *)argv[2]);

    if (!(mo->mask = im_gauss_imask(mo->name, sigma, min_amp))) {
        return (-1);
    }

    return (0);
}

static im_function gauss_imask_desc = {"im_gauss_imask", "generate gaussian INTMASK", 0, gauss_imask_vec, IM_NUMBER(gauss_imask_args), gauss_imask_args};

static int gauss_imask_sep_vec(im_object *argv) {
    im_mask_object *mo = argv[0];
    double sigma = *((double *)argv[1]);
    double min_amp = *((double *)argv[2]);

    if (!(mo->mask = im_gauss_imask_sep(mo->name, sigma, min_amp))) {
        return (-1);
    }

    return (0);
}

static im_function gauss_imask_sep_desc = {"im_gauss_imask_sep", "generate separable gaussian INTMASK", 0, gauss_imask_sep_vec, IM_NUMBER(gauss_imask_args), gauss_imask_args};

static im_arg_desc log_imask_args[] = {IM_OUTPUT_IMASK("mask"), IM_INPUT_DOUBLE("sigma"), IM_INPUT_DOUBLE("min_amp")};

static int log_imask_vec(im_object *argv) {
    im_mask_object *mo = argv[0];
    double sigma = *((double *)argv[1]);
    double min_amp = *((double *)argv[2]);

    if (!(mo->mask = im_log_imask(mo->name, sigma, min_amp))) {
        return (-1);
    }

    return (0);
}

static im_function log_imask_desc = {"im_log_imask", "generate laplacian of gaussian INTMASK", 0, log_imask_vec, IM_NUMBER(log_imask_args), log_imask_args};

static im_arg_desc log_dmask_args[] = {IM_OUTPUT_DMASK("maskfile"), IM_INPUT_DOUBLE("sigma"), IM_INPUT_DOUBLE("min_amp")};

static int log_dmask_vec(im_object *argv) {
    im_mask_object *mo = argv[0];
    double sigma = *((double *)argv[1]);
    double min_amp = *((double *)argv[2]);

    if (!(mo->mask = im_log_dmask(mo->name, sigma, min_amp))) {
        return (-1);
    }

    return (0);
}

static im_function log_dmask_desc = {"im_log_dmask", "generate laplacian of gaussian DOUBLEMASK", 0, log_dmask_vec, IM_NUMBER(log_dmask_args), log_dmask_args};

static im_arg_desc imask_args[] = {IM_INPUT_IMASK("in"), IM_OUTPUT_IMASK("out")};

static im_arg_desc dmask_args[] = {IM_INPUT_DMASK("in"), IM_OUTPUT_DMASK("out")};

static int rotate_imask45_vec(im_object *argv) {
    im_mask_object *min = argv[0];
    im_mask_object *mout = argv[1];

    if (!(mout->mask = im_rotate_imask45(min->mask, mout->name))) {
        return (-1);
    }

    return (0);
}

static im_function rotate_imask45_desc = {"im_rotate_imask45", "rotate INTMASK clockwise by 45 degrees", 0, rotate_imask45_vec, IM_NUMBER(imask_args), imask_args};

static int rotate_imask90_vec(im_object *argv) {
    im_mask_object *min = argv[0];
    im_mask_object *mout = argv[1];

    if (!(mout->mask = im_rotate_imask90(min->mask, mout->name))) {
        return (-1);
    }

    return (0);
}

static im_function rotate_imask90_desc = {"im_rotate_imask90", "rotate INTMASK clockwise by 90 degrees", 0, rotate_imask90_vec, IM_NUMBER(imask_args), imask_args};

static int rotate_dmask45_vec(im_object *argv) {
    im_mask_object *min = argv[0];
    im_mask_object *mout = argv[1];

    if (!(mout->mask = im_rotate_dmask45(min->mask, mout->name))) {
        return (-1);
    }

    return (0);
}

static im_function rotate_dmask45_desc = {"im_rotate_dmask45", "rotate DOUBLEMASK clockwise by 45 degrees", 0, rotate_dmask45_vec, IM_NUMBER(dmask_args), dmask_args};

static int rotate_dmask90_vec(im_object *argv) {
    im_mask_object *min = argv[0];
    im_mask_object *mout = argv[1];

    if (!(mout->mask = im_rotate_dmask90(min->mask, mout->name))) {
        return (-1);
    }

    return (0);
}

static im_function rotate_dmask90_desc = {"im_rotate_dmask90", "rotate DOUBLEMASK clockwise by 90 degrees", 0, rotate_dmask90_vec, IM_NUMBER(dmask_args), dmask_args};

static int imask_xsize_vec(im_object *argv) {
    *((int *)argv[1]) = ((INTMASK *)(((im_mask_object *)argv[0])->mask))->xsize;
    return 0;
}

static int imask_ysize_vec(im_object *argv) {
    *((int *)argv[1]) = ((INTMASK *)(((im_mask_object *)argv[0])->mask))->ysize;
    return 0;
}

static int dmask_xsize_vec(im_object *argv) {
    *((int *)argv[1]) = ((DOUBLEMASK *)(((im_mask_object *)argv[0])->mask))->xsize;
    return 0;
}

static int dmask_ysize_vec(im_object *argv) {
    *((int *)argv[1]) = ((DOUBLEMASK *)(((im_mask_object *)argv[0])->mask))->ysize;
    return 0;
}

static im_arg_desc imask_size_args[] = {IM_INPUT_IMASK("mask"), IM_OUTPUT_INT("size")};

static im_arg_desc dmask_size_args[] = {IM_INPUT_DMASK("mask"), IM_OUTPUT_INT("size")};

static im_function imask_xsize_desc = {"im_imask_xsize", "horizontal size of an intmask", 0, imask_xsize_vec, IM_NUMBER(imask_size_args), imask_size_args};

static im_function imask_ysize_desc = {"im_imask_ysize", "vertical size of an intmask", 0, imask_ysize_vec, IM_NUMBER(imask_size_args), imask_size_args};

static im_function dmask_xsize_desc = {"im_dmask_xsize", "horizontal size of a doublemask", 0, dmask_xsize_vec, IM_NUMBER(dmask_size_args), dmask_size_args};

static im_function dmask_ysize_desc = {"im_dmask_ysize", "vertical size of a doublemask", 0, dmask_ysize_vec, IM_NUMBER(dmask_size_args), dmask_size_args};

static im_function *mask_list[] = {&gauss_dmask_desc, &log_dmask_desc, &log_imask_desc, &gauss_imask_desc, &gauss_imask_sep_desc, &dmask_xsize_desc, &dmask_ysize_desc, &imask_xsize_desc, &imask_ysize_desc, &read_dmask_desc, &rotate_dmask45_desc, &rotate_dmask90_desc, &rotate_imask45_desc, &rotate_imask90_desc, &matcat_desc, &matinv_desc, &matmul_desc, &mattrn_desc};

im_package im__mask = {"mask", IM_NUMBER(mask_list), mask_list};
