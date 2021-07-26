
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/transform.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc rightshift_size_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xshift"), IM_INPUT_INT("yshift"), IM_INPUT_INT("band_fmt")};

static int rightshift_size_vec(im_object *argv) {
    IMAGE *in = (IMAGE *)argv[0];
    IMAGE *out = (IMAGE *)argv[1];
    int *xshift = (int *)argv[2];
    int *yshift = (int *)argv[3];
    int *band_fmt = (int *)argv[4];

    return im_rightshift_size(in, out, *xshift, *yshift, *band_fmt);
}

static im_function rightshift_size_desc = {"im_rightshift_size", "decrease size by a power-of-two factor", IM_FN_PIO | IM_FN_TRANSFORM, rightshift_size_vec, IM_NUMBER(rightshift_size_args), rightshift_size_args};

static im_arg_desc affinei_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INTERPOLATE("interpolate"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("c"), IM_INPUT_DOUBLE("d"), IM_INPUT_DOUBLE("dx"), IM_INPUT_DOUBLE("dy"), IM_INPUT_INT("x"), IM_INPUT_INT("y"), IM_INPUT_INT("w"), IM_INPUT_INT("h")};

static int affinei_vec(im_object *argv) {
    VipsInterpolate *interpolate = VIPS_INTERPOLATE(argv[2]);
    double a = *((double *)argv[3]);
    double b = *((double *)argv[4]);
    double c = *((double *)argv[5]);
    double d = *((double *)argv[6]);
    double dx = *((double *)argv[7]);
    double dy = *((double *)argv[8]);
    int x = *((int *)argv[9]);
    int y = *((int *)argv[10]);
    int w = *((int *)argv[11]);
    int h = *((int *)argv[12]);

    return (im_affinei(argv[0], argv[1], interpolate, a, b, c, d, dx, dy, x, y, w, h));
}

static im_function affinei_desc = {"im_affinei", "affine transform", IM_FN_TRANSFORM | IM_FN_PIO, affinei_vec, IM_NUMBER(affinei_args), affinei_args};

static im_arg_desc affinei_all_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INTERPOLATE("interpolate"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("c"), IM_INPUT_DOUBLE("d"), IM_INPUT_DOUBLE("dx"), IM_INPUT_DOUBLE("dy")};

static int affinei_all_vec(im_object *argv) {
    VipsInterpolate *interpolate = VIPS_INTERPOLATE(argv[2]);
    double a = *((double *)argv[3]);
    double b = *((double *)argv[4]);
    double c = *((double *)argv[5]);
    double d = *((double *)argv[6]);
    double dx = *((double *)argv[7]);
    double dy = *((double *)argv[8]);

    return (im_affinei_all(argv[0], argv[1], interpolate, a, b, c, d, dx, dy));
}

static im_function affinei_all_desc = {"im_affinei_all", "affine transform of whole image", IM_FN_TRANSFORM | IM_FN_PIO, affinei_all_vec, IM_NUMBER(affinei_all_args), affinei_all_args};

static im_arg_desc shrink_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("xfac"), IM_INPUT_DOUBLE("yfac")};

static int shrink_vec(im_object *argv) {
    double xshrink = *((double *)argv[2]);
    double yshrink = *((double *)argv[3]);

    return (im_shrink(argv[0], argv[1], xshrink, yshrink));
}

static im_function shrink_desc = {"im_shrink", "shrink image by xfac, yfac times", IM_FN_TRANSFORM | IM_FN_PIO, shrink_vec, IM_NUMBER(shrink_args), shrink_args};

static im_arg_desc stretch3_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("xdisp"), IM_INPUT_DOUBLE("ydisp")};

static int stretch3_vec(im_object *argv) {
    double xdisp = *((int *)argv[2]);
    double ydisp = *((int *)argv[3]);

    return (im_stretch3(argv[0], argv[1], xdisp, ydisp));
}

static im_function stretch3_desc = {"im_stretch3", "stretch 3%, sub-pixel displace by xdisp/ydisp", IM_FN_PIO, stretch3_vec, IM_NUMBER(stretch3_args), stretch3_args};

static im_function *resample_list[] = {&rightshift_size_desc, &shrink_desc, &stretch3_desc, &affinei_desc, &affinei_all_desc};

im_package im__resample = {"resample", IM_NUMBER(resample_list), resample_list};
