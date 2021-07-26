
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc eye_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xsize"), IM_INPUT_INT("ysize"), IM_INPUT_DOUBLE("factor")};

static int eye_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);
    double factor = *((double *)argv[3]);

    return (im_eye(argv[0], xsize, ysize, factor));
}

static im_function eye_desc = {"im_eye", "generate IM_BANDFMT_UCHAR [0,255] frequency/amplitude image", 0, eye_vec, IM_NUMBER(eye_args), eye_args};

static int feye_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);
    double factor = *((double *)argv[3]);

    return (im_feye(argv[0], xsize, ysize, factor));
}

static im_function feye_desc = {"im_feye", "generate IM_BANDFMT_FLOAT [-1,1] frequency/amplitude image", 0, feye_vec, IM_NUMBER(eye_args), eye_args};

static im_arg_desc zone_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("size")};

static int zone_vec(im_object *argv) {
    int size = *((int *)argv[1]);

    return (im_zone(argv[0], size));
}

static im_function zone_desc = {"im_zone", "generate IM_BANDFMT_UCHAR [0,255] zone plate image", 0, zone_vec, IM_NUMBER(zone_args), zone_args};

static int fzone_vec(im_object *argv) {
    int size = *((int *)argv[1]);

    return (im_fzone(argv[0], size));
}

static im_function fzone_desc = {"im_fzone", "generate IM_BANDFMT_FLOAT [-1,1] zone plate image", 0, fzone_vec, IM_NUMBER(zone_args), zone_args};

static im_arg_desc benchmark_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out")};

static int benchmark_vec(im_object *argv) {
    return (im_benchmarkn(argv[0], argv[1], 1));
}

static im_function benchmark_desc = {"im_benchmark", "do something complicated for testing", IM_FN_PIO, benchmark_vec, IM_NUMBER(benchmark_args), benchmark_args};

static im_arg_desc benchmark2_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DOUBLE("value")};

static int benchmark2_vec(im_object *argv) {
    double f;

    if (im_benchmark2(argv[0], &f)) {
        return (-1);
    }

    *((double *)argv[1]) = f;

    return (0);
}

static im_function benchmark2_desc = {"im_benchmark2", "do something complicated for testing", IM_FN_PIO, benchmark2_vec, IM_NUMBER(benchmark2_args), benchmark2_args};

static im_arg_desc benchmarkn_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("n")};

static int benchmarkn_vec(im_object *argv) {
    int n = *((int *)argv[2]);

    return (im_benchmarkn(argv[0], argv[1], n));
}

static im_function benchmarkn_desc = {"im_benchmarkn", "do something complicated for testing", IM_FN_PIO, benchmarkn_vec, IM_NUMBER(benchmarkn_args), benchmarkn_args};

static im_arg_desc grey_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xsize"), IM_INPUT_INT("ysize")};

static int grey_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);

    return (im_grey(argv[0], xsize, ysize));
}

static im_function grey_desc = {"im_grey", "generate IM_BANDFMT_UCHAR [0,255] grey scale image", 0, grey_vec, IM_NUMBER(grey_args), grey_args};

static int fgrey_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);

    return (im_fgrey(argv[0], xsize, ysize));
}

static im_function fgrey_desc = {"im_fgrey", "generate IM_BANDFMT_FLOAT [0,1] grey scale image", 0, fgrey_vec, IM_NUMBER(grey_args), grey_args};

static int make_xy_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);

    return (im_make_xy(argv[0], xsize, ysize));
}

static im_function make_xy_desc = {"im_make_xy", "generate image with pixel value equal to coordinate", 0, make_xy_vec, IM_NUMBER(grey_args), grey_args};

static im_function *other_list[] = {&benchmark_desc, &benchmark2_desc, &benchmarkn_desc, &eye_desc, &grey_desc, &feye_desc, &fgrey_desc, &fzone_desc, &make_xy_desc, &zone_desc};

im_package im__other = {"other", IM_NUMBER(other_list), other_list};
