
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc profile_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("direction")};

static int profile_vec(im_object *argv) {
    int dir = *((int *)argv[2]);

    return (im_profile(argv[0], argv[1], dir));
}

static im_function profile_desc = {"im_profile", "find first horizontal/vertical edge", IM_FN_TRANSFORM, profile_vec, IM_NUMBER(profile_args), profile_args};

static im_arg_desc erode_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMASK("mask")};

static int dilate_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_dilate(argv[0], argv[1], mo->mask));
}

static im_function dilate_desc = {"im_dilate", "dilate image with mask, adding a black border", IM_FN_PIO | IM_FN_TRANSFORM, dilate_vec, IM_NUMBER(erode_args), erode_args};

static int erode_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_erode(argv[0], argv[1], mo->mask));
}

static im_function erode_desc = {"im_erode", "erode image with mask, adding a black border", IM_FN_PIO | IM_FN_TRANSFORM, erode_vec, IM_NUMBER(erode_args), erode_args};

static im_arg_desc cntlines_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DOUBLE("nlines"), IM_INPUT_INT("direction")};

static int cntlines_vec(im_object *argv) {
    double *out = (double *)argv[1];
    int dir = *((int *)argv[2]);

    return (im_cntlines(argv[0], out, dir));
}

static im_function cntlines_desc = {"im_cntlines", "count horizontal or vertical lines", 0, cntlines_vec, IM_NUMBER(cntlines_args), cntlines_args};

static im_arg_desc rank_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xsize"), IM_INPUT_INT("ysize"), IM_INPUT_INT("n")};

static int rank_vec(im_object *argv) {
    int xsize = *((int *)argv[2]);
    int ysize = *((int *)argv[3]);
    int n = *((int *)argv[4]);

    return (im_rank(argv[0], argv[1], xsize, ysize, n));
}

static im_function rank_desc = {"im_rank", "rank filter nth element of xsize/ysize window", IM_FN_PIO, rank_vec, IM_NUMBER(rank_args), rank_args};

static im_arg_desc zerox_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("flag")};

static int zerox_vec(im_object *argv) {
    int flag = *((int *)argv[2]);

    return (im_zerox(argv[0], argv[1], flag));
}

static im_function zerox_desc = {"im_zerox", "find +ve or -ve zero crossings in image", IM_FN_PIO | IM_FN_TRANSFORM, zerox_vec, IM_NUMBER(zerox_args), zerox_args};

static im_arg_desc maxvalue_args[] = {IM_INPUT_IMAGEVEC("in"), IM_OUTPUT_IMAGE("out")};

static int maxvalue_vec(im_object *argv) {
    im_imagevec_object *iv = (im_imagevec_object *)argv[0];

    return (im_maxvalue(iv->vec, argv[1], iv->n));
}

static im_function maxvalue_desc = {"im_maxvalue", "point-wise maximum value", IM_FN_PIO, maxvalue_vec, IM_NUMBER(maxvalue_args), maxvalue_args};

static im_arg_desc rank_image_args[] = {IM_INPUT_IMAGEVEC("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("index")};

static int rank_image_vec(im_object *argv) {
    im_imagevec_object *iv = (im_imagevec_object *)argv[0];
    int index = *((int *)argv[2]);

    return (im_rank_image(iv->vec, argv[1], iv->n, index));
}

static im_function rank_image_desc = {"im_rank_image", "point-wise pixel rank", IM_FN_PIO, rank_image_vec, IM_NUMBER(rank_image_args), rank_image_args};

static im_arg_desc label_regions_args[] = {IM_INPUT_IMAGE("test"), IM_OUTPUT_IMAGE("mask"), IM_OUTPUT_INT("segments")};

static int label_regions_vec(im_object *argv) {
    IMAGE *test = argv[0];
    IMAGE *mask = argv[1];
    int *serial = (int *)argv[2];

    return (im_label_regions(test, mask, serial));
}

static im_function label_regions_desc = {"im_label_regions", "number continuous regions in an image", 0, label_regions_vec, IM_NUMBER(label_regions_args), label_regions_args};

static im_function *morph_list[] = {&cntlines_desc, &dilate_desc, &rank_desc, &rank_image_desc, &maxvalue_desc, &label_regions_desc, &zerox_desc, &erode_desc, &profile_desc};

im_package im__morphology = {"morphology", IM_NUMBER(morph_list), morph_list};
