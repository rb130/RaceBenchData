
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

static im_arg_desc gammacorrect_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("exponent")};

static int gammacorrect_vec(im_object *argv) {
    double exp = *((double *)argv[2]);

    return (im_gammacorrect(argv[0], argv[1], exp));
}

static im_function gammacorrect_desc = {"im_gammacorrect", "gamma-correct image", IM_FN_PIO, gammacorrect_vec, IM_NUMBER(gammacorrect_args), gammacorrect_args};

static im_arg_desc heq_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("band_number")};

static int heq_vec(im_object *argv) {
    int bn = *((int *)argv[2]);

    return (im_heq(argv[0], argv[1], bn));
}

static im_function heq_desc = {"im_heq", "histogram-equalise image", IM_FN_PIO, heq_vec, IM_NUMBER(heq_args), heq_args};

static im_arg_desc histindexed_args[] = {IM_INPUT_IMAGE("index"), IM_INPUT_IMAGE("value"), IM_OUTPUT_IMAGE("out")};

static int histindexed_vec(im_object *argv) {
    return (im_hist_indexed(argv[0], argv[1], argv[2]));
}

static im_function histindexed_desc = {"im_hist_indexed", "make a histogram with an index image", IM_FN_PIO, histindexed_vec, IM_NUMBER(histindexed_args), histindexed_args};

static int hist_vec(im_object *argv) {
    int bn = *((int *)argv[2]);

    return (im_hist(argv[0], argv[1], bn));
}

static im_function hist_desc = {"im_hist", "find and graph histogram of image", IM_FN_PIO | IM_FN_TRANSFORM, hist_vec, IM_NUMBER(heq_args), heq_args};

static int histcum_vec(im_object *argv) {
    return (im_histcum(argv[0], argv[1]));
}

static im_function histcum_desc = {"im_histcum", "turn histogram to cumulative histogram", IM_FN_PIO, histcum_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int histnorm_vec(im_object *argv) {
    return (im_histnorm(argv[0], argv[1]));
}

static im_function histnorm_desc = {"im_histnorm", "form normalised histogram", IM_FN_PIO, histnorm_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int histeq_vec(im_object *argv) {
    return (im_histeq(argv[0], argv[1]));
}

static im_function histeq_desc = {"im_histeq", "form histogram equalistion LUT", IM_FN_PIO, histeq_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int histgr_vec(im_object *argv) {
    int bn = *((int *)argv[2]);

    return (im_histgr(argv[0], argv[1], bn));
}

static im_function histgr_desc = {"im_histgr", "find histogram of image", IM_FN_TRANSFORM, histgr_vec, IM_NUMBER(heq_args), heq_args};

static int histnD_vec(im_object *argv) {
    int bins = *((int *)argv[2]);

    return (im_histnD(argv[0], argv[1], bins));
}

static im_arg_desc histnD_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("bins")};

static im_function histnD_desc = {"im_histnD", "find 1D, 2D or 3D histogram of image", IM_FN_TRANSFORM, histnD_vec, IM_NUMBER(histnD_args), histnD_args};

static int histplot_vec(im_object *argv) {
    return (im_histplot(argv[0], argv[1]));
}

static im_function histplot_desc = {"im_histplot", "plot graph of histogram", IM_FN_PIO, histplot_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc histspec_args[] = {
    IM_INPUT_IMAGE("in"),
    IM_INPUT_IMAGE("ref"),
    IM_OUTPUT_IMAGE("out"),
};

static int histspec_vec(im_object *argv) {
    return (im_histspec(argv[0], argv[1], argv[2]));
}

static im_function histspec_desc = {"im_histspec", "find histogram which will make pdf of in match ref", 0, histspec_vec, IM_NUMBER(histspec_args), histspec_args};

static int hsp_vec(im_object *argv) {
    return (im_hsp(argv[0], argv[1], argv[2]));
}

static im_function hsp_desc = {"im_hsp", "match stats of in to stats of ref", 0, hsp_vec, IM_NUMBER(histspec_args), histspec_args};

static im_arg_desc identity_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("nbands")};

static int identity_vec(im_object *argv) {
    int nb = *((int *)argv[1]);

    return (im_identity(argv[0], nb));
}

static im_function identity_desc = {"im_identity", "generate identity histogram", 0, identity_vec, IM_NUMBER(identity_args), identity_args};

static im_arg_desc identity_ushort_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("nbands"), IM_INPUT_INT("size")};

static int identity_ushort_vec(im_object *argv) {
    int nb = *((int *)argv[1]);
    int sz = *((int *)argv[2]);

    return (im_identity_ushort(argv[0], nb, sz));
}

static im_function identity_ushort_desc = {"im_identity_ushort", "generate ushort identity histogram", 0, identity_ushort_vec, IM_NUMBER(identity_ushort_args), identity_ushort_args};

static im_arg_desc lhisteq_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("width"), IM_INPUT_INT("height")};

static int lhisteq_vec(im_object *argv) {
    int xw = *((int *)argv[2]);
    int yw = *((int *)argv[3]);

    return (im_lhisteq(argv[0], argv[1], xw, yw));
}

static im_function lhisteq_desc = {"im_lhisteq", "local histogram equalisation", IM_FN_PIO, lhisteq_vec, IM_NUMBER(lhisteq_args), lhisteq_args};

static im_arg_desc maplut_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMAGE("lut")};

static int maplut_vec(im_object *argv) {
    return (im_maplut(argv[0], argv[1], argv[2]));
}

static im_function maplut_desc = {"im_maplut", "map image through LUT", IM_FN_PIO, maplut_vec, IM_NUMBER(maplut_args), maplut_args};

static int project_vec(im_object *argv) {
    return (im_project(argv[0], argv[1], argv[2]));
}

static im_arg_desc project_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("hout"), IM_OUTPUT_IMAGE("vout")};

static im_function project_desc = {"im_project", "find horizontal and vertical projections of an image", IM_FN_TRANSFORM, project_vec, IM_NUMBER(project_args), project_args};

static im_arg_desc stdif_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("a"), IM_INPUT_DOUBLE("m0"), IM_INPUT_DOUBLE("b"), IM_INPUT_DOUBLE("s0"), IM_INPUT_INT("xw"), IM_INPUT_INT("yw")};

static int stdif_vec(im_object *argv) {
    double a = *((double *)argv[2]);
    double m0 = *((double *)argv[3]);
    double b = *((double *)argv[4]);
    double s0 = *((double *)argv[5]);
    int xw = *((int *)argv[6]);
    int yw = *((int *)argv[7]);

    return (im_stdif(argv[0], argv[1], a, m0, b, s0, xw, yw));
}

static im_function stdif_desc = {"im_stdif", "statistical differencing", IM_FN_PIO, stdif_vec, IM_NUMBER(stdif_args), stdif_args};

static im_arg_desc buildlut_args[] = {IM_INPUT_DMASK("xyes"), IM_OUTPUT_IMAGE("lut")};

static int buildlut_vec(im_object *argv) {
    im_mask_object *mi = argv[0];

    return (im_buildlut(mi->mask, argv[1]));
}

static im_function buildlut_desc = {"im_buildlut", "generate LUT table from set of x/y positions", 0, buildlut_vec, IM_NUMBER(buildlut_args), buildlut_args};

static im_arg_desc invertlut_args[] = {IM_INPUT_DMASK("measures"), IM_OUTPUT_IMAGE("lut"), IM_INPUT_INT("lut_size")};

static int invertlut_vec(im_object *argv) {
    im_mask_object *mi = argv[0];
    int lut_size = *((int *)argv[2]);

    return (im_invertlut(mi->mask, argv[1], lut_size));
}

static im_function invertlut_desc = {"im_invertlut", "generate correction table from set of measures", 0, invertlut_vec, IM_NUMBER(invertlut_args), invertlut_args};

static im_arg_desc tone_build_args[] = {IM_OUTPUT_IMAGE("hist"), IM_INPUT_DOUBLE("Lb"), IM_INPUT_DOUBLE("Lw"), IM_INPUT_DOUBLE("Ps"), IM_INPUT_DOUBLE("Pm"), IM_INPUT_DOUBLE("Ph"), IM_INPUT_DOUBLE("S"), IM_INPUT_DOUBLE("M"), IM_INPUT_DOUBLE("H")};

static int tone_build_vec(im_object *argv) {
    double Lb = *((double *)argv[1]);
    double Lw = *((double *)argv[2]);
    double Ps = *((double *)argv[3]);
    double Pm = *((double *)argv[4]);
    double Ph = *((double *)argv[5]);
    double S = *((double *)argv[6]);
    double M = *((double *)argv[7]);
    double H = *((double *)argv[8]);

    return (im_tone_build(argv[0], Lb, Lw, Ps, Pm, Ph, S, M, H));
}

static im_function tone_build_desc = {"im_tone_build", "create LUT for tone adjustment of LabS images", 0, tone_build_vec, IM_NUMBER(tone_build_args), tone_build_args};

static im_arg_desc tone_build_range_args[] = {IM_OUTPUT_IMAGE("hist"), IM_INPUT_INT("in_max"), IM_INPUT_INT("out_max"), IM_INPUT_DOUBLE("Lb"), IM_INPUT_DOUBLE("Lw"), IM_INPUT_DOUBLE("Ps"), IM_INPUT_DOUBLE("Pm"), IM_INPUT_DOUBLE("Ph"), IM_INPUT_DOUBLE("S"), IM_INPUT_DOUBLE("M"), IM_INPUT_DOUBLE("H")};

static int tone_build_range_vec(im_object *argv) {
    int in_max = *((int *)argv[1]);
    int out_max = *((int *)argv[2]);
    double Lb = *((double *)argv[3]);
    double Lw = *((double *)argv[4]);
    double Ps = *((double *)argv[5]);
    double Pm = *((double *)argv[6]);
    double Ph = *((double *)argv[7]);
    double S = *((double *)argv[8]);
    double M = *((double *)argv[9]);
    double H = *((double *)argv[10]);

    return (im_tone_build_range(argv[0], in_max, out_max, Lb, Lw, Ps, Pm, Ph, S, M, H));
}

static im_function tone_build_range_desc = {"im_tone_build_range", "create LUT for tone adjustment", 0, tone_build_range_vec, IM_NUMBER(tone_build_range_args), tone_build_range_args};

static im_arg_desc tone_analyse_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("hist"), IM_INPUT_DOUBLE("Ps"), IM_INPUT_DOUBLE("Pm"), IM_INPUT_DOUBLE("Ph"), IM_INPUT_DOUBLE("S"), IM_INPUT_DOUBLE("M"), IM_INPUT_DOUBLE("H")};

static int tone_analyse_vec(im_object *argv) {
    double Ps = *((double *)argv[2]);
    double Pm = *((double *)argv[3]);
    double Ph = *((double *)argv[4]);
    double S = *((double *)argv[5]);
    double M = *((double *)argv[6]);
    double H = *((double *)argv[7]);

    return (im_tone_analyse(argv[0], argv[1], Ps, Pm, Ph, S, M, H));
}

static im_function tone_analyse_desc = {"im_tone_analyse", "analyse in and create LUT for tone adjustment", 0, tone_analyse_vec, IM_NUMBER(tone_analyse_args), tone_analyse_args};

static im_arg_desc ismonotonic_args[] = {IM_INPUT_IMAGE("lut"), IM_OUTPUT_INT("mono")};

static int ismonotonic_vec(im_object *argv) {
    int *res = (int *)argv[1];

    return (im_ismonotonic(argv[0], res));
}

static im_function ismonotonic_desc = {"im_ismonotonic", "test LUT for monotonicity", 0, ismonotonic_vec, IM_NUMBER(ismonotonic_args), ismonotonic_args};

static im_arg_desc tone_map_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMAGE("lut")};

static int tone_map_vec(im_object *argv) {
    return (im_tone_map(argv[0], argv[1], argv[2]));
}

static im_function tone_map_desc = {"im_tone_map", "map L channel of LabS or LabQ image through LUT", IM_FN_PIO, tone_map_vec, IM_NUMBER(tone_map_args), tone_map_args};

static im_arg_desc mpercent_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_DOUBLE("percent"), IM_OUTPUT_INT("thresh")};

static int mpercent_vec(im_object *argv) {
    double percent = *((double *)argv[1]);

    return (im_mpercent(argv[0], percent, argv[2]));
}

static im_function mpercent_desc = {"im_mpercent", "find threshold above which there are percent values", 0, mpercent_vec, IM_NUMBER(mpercent_args), mpercent_args};

static im_function *hist_list[] = {&gammacorrect_desc, &heq_desc, &hist_desc, &histcum_desc, &histeq_desc, &histindexed_desc, &histgr_desc, &histnD_desc, &histnorm_desc, &histplot_desc, &histspec_desc, &hsp_desc, &identity_desc, &identity_ushort_desc, &ismonotonic_desc, &lhisteq_desc, &mpercent_desc, &invertlut_desc, &buildlut_desc, &maplut_desc, &project_desc, &stdif_desc, &tone_analyse_desc, &tone_build_desc, &tone_build_range_desc, &tone_map_desc};

im_package im__histograms_lut = {"histograms_lut", IM_NUMBER(hist_list), hist_list};
