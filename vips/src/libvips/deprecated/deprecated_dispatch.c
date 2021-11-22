

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

static int clip_vec(im_object *argv) {
    return (im_clip(argv[0], argv[1]));
}

static im_function clip_desc = {"im_clip", "convert to unsigned 8-bit integer", IM_FN_PTOP | IM_FN_PIO, clip_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int c2ps_vec(im_object *argv) {
    return (im_c2ps(argv[0], argv[1]));
}

static im_function c2ps_desc = {"im_c2ps", "find power spectrum of complex image", IM_FN_PTOP | IM_FN_PIO, c2ps_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc lhisteq_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("width"), IM_INPUT_INT("height")};

static im_arg_desc stdif_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("m0"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("s0"), IM_INPUT_INT("xw"), IM_INPUT_INT("yw")};

static im_arg_desc erode_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMASK("mask")};

static im_arg_desc rank_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xsize"), IM_INPUT_INT("ysize"), IM_INPUT_INT("n")};

static im_arg_desc conv_imask[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMASK("matrix")};

static im_arg_desc conv_dmask[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DMASK("matrix")};

static int cmulnorm_vec(im_object *argv) {
    return (im_cmulnorm(argv[0], argv[1], argv[2]));
}

static im_function cmulnorm_desc = {"im_cmulnorm", N_("multiply two complex images, normalising output"), IM_FN_PIO, cmulnorm_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc fav4_args[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_INPUT_IMAGE("in3"), IM_INPUT_IMAGE("in4"), IM_OUTPUT_IMAGE("out")};

static int fav4_vec(im_object *argv) {
    IMAGE *buf[4];

    buf[0] = argv[0];
    buf[1] = argv[1];
    buf[2] = argv[2];
    buf[3] = argv[3];

    return (im_fav4(&buf[0], argv[4]));
}

static im_function fav4_desc = {"im_fav4", N_("average of 4 images"), 0, fav4_vec, IM_NUMBER(fav4_args), fav4_args};

static im_arg_desc gadd_args[] = {IM_INPUT_DOUBLE("a"), IM_INPUT_IMAGE("in1"), IM_INPUT_DOUBLE("b"), IM_INPUT_IMAGE("in2"), IM_INPUT_DOUBLE("c"), IM_OUTPUT_IMAGE("out")};

static int gadd_vec(im_object *argv) {
    double a = *((double *)argv[0]);
    double b = *((double *)argv[2]);
    double c = *((double *)argv[4]);

    return (im_gadd(a, argv[1], b, argv[3], c, argv[5]));
}

static im_function gadd_desc = {"im_gadd", N_("calculate a*in1 + b*in2 + c = outfile"), 0, gadd_vec, IM_NUMBER(gadd_args), gadd_args};

static im_arg_desc litecor_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_IMAGE("white"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("clip"), IM_INPUT_DOUBLE("factor")};

static int litecor_vec(im_object *argv) {
    int clip = *((int *)argv[3]);
    double factor = *((double *)argv[4]);

    return (im_litecor(argv[0], argv[1], argv[2], clip, factor));
}

static im_function litecor_desc = {"im_litecor", N_("calculate max(white)*factor*(in/white), if clip == 1"), 0, litecor_vec, IM_NUMBER(litecor_args), litecor_args};

static im_arg_desc affine_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("c"), IM_INPUT_DOUBLE("d"), IM_INPUT_DOUBLE("dx"), IM_INPUT_DOUBLE("dy"), IM_INPUT_INT("x"), IM_INPUT_INT("y"), IM_INPUT_INT("w"), IM_INPUT_INT("h")};

static int affine_vec(im_object *argv) {
    double a = *((double *)argv[2]);
    double b = *((double *)argv[3]);
    double c = *((double *)argv[4]);
    double d = *((double *)argv[5]);
    double dx = *((double *)argv[6]);
    double dy = *((double *)argv[7]);
    int x = *((int *)argv[8]);
    int y = *((int *)argv[9]);
    int w = *((int *)argv[10]);
    int h = *((int *)argv[11]);

    return (im_affine(argv[0], argv[1], a, b, c, d, dx, dy, x, y, w, h));
}

static im_function affine_desc = {"im_affine", "affine transform", IM_FN_TRANSFORM | IM_FN_PIO, affine_vec, IM_NUMBER(affine_args), affine_args};

static im_arg_desc similarity_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("dx"), IM_INPUT_DOUBLE("dy")};

static int similarity_vec(im_object *argv) {
    double a = *((double *)argv[2]);
    double b = *((double *)argv[3]);
    double dx = *((double *)argv[4]);
    double dy = *((double *)argv[5]);

    return (im_similarity(argv[0], argv[1], a, b, dx, dy));
}

static im_function similarity_desc = {"im_similarity", "similarity transformation", IM_FN_TRANSFORM | IM_FN_PIO, similarity_vec, IM_NUMBER(similarity_args), similarity_args};

static im_arg_desc similarity_area_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("dx"), IM_INPUT_DOUBLE("dy"), IM_INPUT_INT("x"), IM_INPUT_INT("y"), IM_INPUT_INT("w"), IM_INPUT_INT("h")};

static int similarity_area_vec(im_object *argv) {
    double a = *((double *)argv[2]);
    double b = *((double *)argv[3]);
    double dx = *((double *)argv[4]);
    double dy = *((double *)argv[5]);
    int x = *((int *)argv[6]);
    int y = *((int *)argv[7]);
    int w = *((int *)argv[8]);
    int h = *((int *)argv[9]);

    return (im_similarity_area(argv[0], argv[1], a, b, dx, dy, x, y, w, h));
}

static im_function similarity_area_desc = {"im_similarity_area", "output area xywh of similarity transformation", IM_FN_TRANSFORM | IM_FN_PIO, similarity_area_vec, IM_NUMBER(similarity_area_args), similarity_area_args};

static int icc_export_vec(im_object *argv) {
    int intent = *((int *)argv[3]);

    return (im_icc_export(argv[0], argv[1], argv[2], intent));
}

static im_arg_desc icc_export_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("output_profile"), IM_INPUT_INT("intent")};

static im_function icc_export_desc = {"im_icc_export", "convert a float LAB to an 8-bit device image with an ICC profile",

    IM_FN_PIO, icc_export_vec, IM_NUMBER(icc_export_args), icc_export_args};

static im_arg_desc segment_args[] = {IM_INPUT_IMAGE("test"), IM_OUTPUT_IMAGE("mask"), IM_OUTPUT_INT("segments")};

static int segment_vec(im_object *argv) {
    IMAGE *test = argv[0];
    IMAGE *mask = argv[1];
    int *serial = (int *)argv[2];

    return (im_segment(test, mask, serial));
}

static im_function segment_desc = {"im_segment", "number continuous regions in an image", 0, segment_vec, IM_NUMBER(segment_args), segment_args};

static int print_vec(im_object *argv) {
    const char *message = argv[0];
    char **out = (char **)&argv[1];

    if (im_print(message)) {
        return (-1);
    }
    *out = im_strdup(NULL, "printed");

    return (0);
}

static im_arg_desc print_arg_types[] = {IM_INPUT_STRING("message"), IM_OUTPUT_STRING("result")};

static im_function print_desc = {"im_print", "print string to stdout", 0, print_vec, IM_NUMBER(print_arg_types), print_arg_types};

static int clip2dcm_vec(im_object *argv) {
    return (im_clip2dcm(argv[0], argv[1]));
}

static im_function clip2dcm_desc = {"im_clip2dcm", "convert to double complex", IM_FN_PTOP | IM_FN_PIO, clip2dcm_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2cm_vec(im_object *argv) {
    return (im_clip2cm(argv[0], argv[1]));
}

static im_function clip2cm_desc = {"im_clip2cm", "convert to complex", IM_FN_PTOP | IM_FN_PIO, clip2cm_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2us_vec(im_object *argv) {
    return (im_clip2us(argv[0], argv[1]));
}

static im_function clip2us_desc = {"im_clip2us", "convert to unsigned 16-bit integer", IM_FN_PTOP | IM_FN_PIO, clip2us_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2ui_vec(im_object *argv) {
    return (im_clip2ui(argv[0], argv[1]));
}

static im_function clip2ui_desc = {"im_clip2ui", "convert to unsigned 32-bit integer", IM_FN_PTOP | IM_FN_PIO, clip2ui_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2s_vec(im_object *argv) {
    return (im_clip2s(argv[0], argv[1]));
}

static im_function clip2s_desc = {"im_clip2s", "convert to signed 16-bit integer", IM_FN_PTOP | IM_FN_PIO, clip2s_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2i_vec(im_object *argv) {
    return (im_clip2i(argv[0], argv[1]));
}

static im_function clip2i_desc = {"im_clip2i", "convert to signed 32-bit integer", IM_FN_PTOP | IM_FN_PIO, clip2i_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2d_vec(im_object *argv) {
    return (im_clip2d(argv[0], argv[1]));
}

static im_function clip2d_desc = {"im_clip2d", "convert to double-precision float", IM_FN_PTOP | IM_FN_PIO, clip2d_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2f_vec(im_object *argv) {
    return (im_clip2f(argv[0], argv[1]));
}

static im_function clip2f_desc = {"im_clip2f", "convert to single-precision float", IM_FN_PTOP | IM_FN_PIO, clip2f_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int clip2c_vec(im_object *argv) {
    return (im_clip2c(argv[0], argv[1]));
}

static im_function clip2c_desc = {"im_clip2c", "convert to signed 8-bit integer", IM_FN_PTOP | IM_FN_PIO, clip2c_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc thresh_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_DOUBLE("threshold")};

static int thresh_vec(im_object *argv) {
    double t1 = *((double *)argv[2]);

    return (im_thresh(argv[0], argv[1], t1));
}

static im_function thresh_desc = {"im_thresh", "slice an image at a threshold", 0, thresh_vec, IM_NUMBER(thresh_args), thresh_args};

static im_arg_desc slice_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_DOUBLE("thresh1"), IM_INPUT_DOUBLE("thresh2")};

static int slice_vec(im_object *argv) {
    double t1 = *((double *)argv[2]);
    double t2 = *((double *)argv[3]);

    return (im_slice(argv[0], argv[1], t1, t2));
}

static im_function slice_desc = {"im_slice", "slice an image using two thresholds", 0, slice_vec, IM_NUMBER(slice_args), slice_args};

static im_arg_desc convsub_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMASK("matrix"), IM_INPUT_INT("xskip"), IM_INPUT_INT("yskip")};

static int convsub_vec(im_object *argv) {
    im_mask_object *mo = argv[2];
    int xskip = *((int *)argv[3]);
    int yskip = *((int *)argv[4]);

    return (im_convsub(argv[0], argv[1], mo->mask, xskip, yskip));
}

static im_function convsub_desc = {"im_convsub", "convolve uchar to uchar, sub-sampling by xskip, yskip", IM_FN_TRANSFORM, convsub_vec, IM_NUMBER(convsub_args), convsub_args};

static im_arg_desc bernd_args[] = {IM_INPUT_STRING("tiffname"), IM_INPUT_INT("left"), IM_INPUT_INT("top"), IM_INPUT_INT("width"), IM_INPUT_INT("height")};

static int bernd_vec(im_object *argv) {
    char *name = argv[0];
    int left = *((int *)argv[1]);
    int top = *((int *)argv[2]);
    int width = *((int *)argv[3]);
    int height = *((int *)argv[4]);

    return (im_bernd(name, left, top, width, height));
}

static im_function bernd_desc = {"im_bernd", "extract from pyramid as jpeg", 0, bernd_vec, IM_NUMBER(bernd_args), bernd_args};

static im_arg_desc line_args[] = {IM_RW_IMAGE("im"), IM_INPUT_INT("x1"), IM_INPUT_INT("y1"), IM_INPUT_INT("x2"), IM_INPUT_INT("y2"), IM_INPUT_INT("pelval")};

static int line_vec(im_object *argv) {
    int x1 = *((int *)argv[1]);
    int y1 = *((int *)argv[2]);
    int x2 = *((int *)argv[3]);
    int y2 = *((int *)argv[4]);
    int pel = *((int *)argv[5]);

    return (im_line(argv[0], x1, y1, x2, y2, pel));
}

static im_function line_desc = {"im_line", "draw line between points (x1,y1) and (x2,y2)", 0, line_vec, IM_NUMBER(line_args), line_args};

static im_arg_desc resize_linear_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("X"), IM_INPUT_INT("Y")};

static int resize_linear_vec(im_object *argv) {
    int X = *((int *)argv[2]);
    int Y = *((int *)argv[3]);

    return (im_resize_linear(argv[0], argv[1], X, Y));
}

static im_function resize_linear_desc = {"im_resize_linear", "resize to X by Y pixels with linear interpolation", 0, resize_linear_vec, IM_NUMBER(resize_linear_args), resize_linear_args};

static im_arg_desc insertplaceset_args[] = {IM_INPUT_IMAGE("main"), IM_INPUT_IMAGE("sub"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INTVEC("x"), IM_INPUT_INTVEC("y")};

static int insertplaceset_vec(im_object *argv) {
    im_intvec_object *xv = (im_intvec_object *)argv[3];
    im_intvec_object *yv = (im_intvec_object *)argv[4];

    if (xv->n != yv->n) {
        im_error("im_insertplaceset", "%s", _("vectors not same length"));
        return (-1);
    }

    if (im_insertset(argv[0], argv[1], argv[2], xv->n, xv->vec, yv->vec)) {
        return (-1);
    }

    return (0);
}

static im_function insertplaceset_desc = {"im_insertplaceset", "insert sub into main at every position in x, y", 0, insertplaceset_vec, IM_NUMBER(insertplaceset_args), insertplaceset_args};

static int spcor_raw_vec(im_object *argv) {
    return (im_spcor_raw(argv[0], argv[1], argv[2]));
}

static im_function spcor_raw_desc = {"im_spcor_raw", "normalised correlation of in2 within in1, no black padding", IM_FN_PIO | IM_FN_TRANSFORM, spcor_raw_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int gradcor_raw_vec(im_object *argv) {
    return (im_gradcor_raw(argv[0], argv[1], argv[2]));
}

static im_function gradcor_raw_desc = {"im_gradcor_raw", "non-normalised correlation of gradient of in2 within in1, no padding", IM_FN_PIO | IM_FN_TRANSFORM, gradcor_raw_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int fastcor_raw_vec(im_object *argv) {
    return (im_fastcor_raw(argv[0], argv[1], argv[2]));
}

static im_function fastcor_raw_desc = {"im_fastcor_raw", "fast correlate in2 within in1, no border", IM_FN_TRANSFORM | IM_FN_PIO, fastcor_raw_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int convsepf_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convsepf_raw(argv[0], argv[1], mo->mask));
}

static im_function convsepf_raw_desc = {"im_convsepf_raw", "seperable convolution, with DOUBLEMASK, no border", IM_FN_PIO | IM_FN_TRANSFORM, convsepf_raw_vec, IM_NUMBER(conv_dmask), conv_dmask};

static int convsep_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convsep_raw(argv[0], argv[1], mo->mask));
}

static im_function convsep_raw_desc = {"im_convsep_raw", "seperable convolution, no border", IM_FN_TRANSFORM | IM_FN_PIO, convsep_raw_vec, IM_NUMBER(conv_imask), conv_imask};

static int convf_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convf_raw(argv[0], argv[1], mo->mask));
}

static im_function convf_raw_desc = {"im_convf_raw", "convolve, with DOUBLEMASK, no border", IM_FN_TRANSFORM | IM_FN_PIO, convf_raw_vec, IM_NUMBER(conv_dmask), conv_dmask};

static int conv_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_conv_raw(argv[0], argv[1], mo->mask));
}

static im_function conv_raw_desc = {"im_conv_raw", "convolve, no border", IM_FN_TRANSFORM | IM_FN_PIO, conv_raw_vec, IM_NUMBER(conv_imask), conv_imask};

static im_arg_desc contrast_surface_raw_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("half_win_size"), IM_INPUT_INT("spacing")};

static int contrast_surface_raw_vec(im_object *argv) {
    int half_win_size = *((int *)argv[2]);
    int spacing = *((int *)argv[3]);

    return (im_contrast_surface_raw(argv[0], argv[1], half_win_size, spacing));
}

static im_function contrast_surface_raw_desc = {"im_contrast_surface_raw", "find high-contrast points in an image", IM_FN_PIO, contrast_surface_raw_vec, IM_NUMBER(contrast_surface_raw_args), contrast_surface_raw_args};

static int stdif_raw_vec(im_object *argv) {
    double a = *((double *)argv[2]);
    double m0 = *((double *)argv[3]);
    double b = *((double *)argv[4]);
    double s0 = *((double *)argv[5]);
    int xw = *((int *)argv[6]);
    int yw = *((int *)argv[7]);

    return (im_stdif_raw(argv[0], argv[1], a, m0, b, s0, xw, yw));
}

static im_function stdif_raw_desc = {"im_stdif_raw", "statistical differencing, no border", IM_FN_PIO, stdif_raw_vec, IM_NUMBER(stdif_args), stdif_args};

static int lhisteq_raw_vec(im_object *argv) {
    int xw = *((int *)argv[2]);
    int yw = *((int *)argv[3]);

    return (im_lhisteq_raw(argv[0], argv[1], xw, yw));
}

static im_function lhisteq_raw_desc = {"im_lhisteq_raw", "local histogram equalisation, no border", IM_FN_PIO, lhisteq_raw_vec, IM_NUMBER(lhisteq_args), lhisteq_args};

static int rank_raw_vec(im_object *argv) {
    int xsize = *((int *)argv[2]);
    int ysize = *((int *)argv[3]);
    int n = *((int *)argv[4]);

    return (im_rank_raw(argv[0], argv[1], xsize, ysize, n));
}

static im_function rank_raw_desc = {"im_rank_raw", "rank filter nth element of xsize/ysize window, no border", IM_FN_PIO, rank_raw_vec, IM_NUMBER(rank_args), rank_args};

static int erode_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_erode_raw(argv[0], argv[1], mo->mask));
}

static im_function erode_raw_desc = {"im_erode_raw", "erode image with mask", IM_FN_PIO | IM_FN_TRANSFORM, erode_raw_vec, IM_NUMBER(erode_args), erode_args};

static int dilate_raw_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_dilate_raw(argv[0], argv[1], mo->mask));
}

static im_function dilate_raw_desc = {"im_dilate_raw", "dilate image with mask", IM_FN_PIO | IM_FN_TRANSFORM, dilate_raw_vec, IM_NUMBER(erode_args), erode_args};

static int convsepf_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convsepf(argv[0], argv[1], mo->mask));
}

static im_function convsepf_desc = {"im_convsepf", "seperable convolution, with DOUBLEMASK", IM_FN_PIO | IM_FN_TRANSFORM, convsepf_vec, IM_NUMBER(conv_dmask), conv_dmask};

static int convf_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_convf(argv[0], argv[1], mo->mask));
}

static im_function convf_desc = {"im_convf", "convolve, with DOUBLEMASK", IM_FN_TRANSFORM | IM_FN_PIO, convf_vec, IM_NUMBER(conv_dmask), conv_dmask};

static im_function *deprecated_list[] = {&clip_desc, &c2ps_desc, &resize_linear_desc, &cmulnorm_desc, &fav4_desc, &gadd_desc, &icc_export_desc, &litecor_desc, &affine_desc, &clip2c_desc, &clip2cm_desc, &clip2d_desc, &clip2dcm_desc, &clip2f_desc, &clip2i_desc, &convsub_desc, &convf_desc, &convsepf_desc, &clip2s_desc, &clip2ui_desc, &insertplaceset_desc, &clip2us_desc, &print_desc, &slice_desc, &bernd_desc, &segment_desc, &line_desc, &thresh_desc, &convf_raw_desc, &conv_raw_desc, &contrast_surface_raw_desc, &convsepf_raw_desc, &convsep_raw_desc, &fastcor_raw_desc, &gradcor_raw_desc, &spcor_raw_desc, &lhisteq_raw_desc, &stdif_raw_desc, &rank_raw_desc, &dilate_raw_desc, &erode_raw_desc, &similarity_area_desc, &similarity_desc};

im_package im__deprecated = {"deprecated", IM_NUMBER(deprecated_list), deprecated_list};
