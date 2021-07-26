
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/transform.h>

#include "merge.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc merge_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("dx"), IM_INPUT_INT("dy"), IM_INPUT_INT("mwidth")};

static im_arg_desc merge1_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xr1"), IM_INPUT_INT("yr1"), IM_INPUT_INT("xs1"), IM_INPUT_INT("ys1"), IM_INPUT_INT("xr2"), IM_INPUT_INT("yr2"), IM_INPUT_INT("xs2"), IM_INPUT_INT("ys2"), IM_INPUT_INT("mwidth")};

static im_arg_desc mosaic_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("bandno"), IM_INPUT_INT("xr"), IM_INPUT_INT("yr"), IM_INPUT_INT("xs"), IM_INPUT_INT("ys"), IM_INPUT_INT("halfcorrelation"), IM_INPUT_INT("halfarea"), IM_INPUT_INT("balancetype"), IM_INPUT_INT("mwidth")};

static im_arg_desc mosaic1_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("bandno"), IM_INPUT_INT("xr1"), IM_INPUT_INT("yr1"), IM_INPUT_INT("xs1"), IM_INPUT_INT("ys1"), IM_INPUT_INT("xr2"), IM_INPUT_INT("yr2"), IM_INPUT_INT("xs2"), IM_INPUT_INT("ys2"), IM_INPUT_INT("halfcorrelation"), IM_INPUT_INT("halfarea"), IM_INPUT_INT("balancetype"), IM_INPUT_INT("mwidth")};

static int lrmosaic_vec(im_object *argv) {
    int bandno = *((int *)argv[3]);
    int xr = *((int *)argv[4]);
    int yr = *((int *)argv[5]);
    int xs = *((int *)argv[6]);
    int ys = *((int *)argv[7]);
    int halfcorrelation = *((int *)argv[8]);
    int halfarea = *((int *)argv[9]);
    int balancetype = *((int *)argv[10]);
    int mwidth = *((int *)argv[11]);

    return (im_lrmosaic(argv[0], argv[1], argv[2], bandno, xr, yr, xs, ys, halfcorrelation, halfarea, balancetype, mwidth));
}

static int lrmosaic1_vec(im_object *argv) {
    int bandno = *((int *)argv[3]);
    int xr1 = *((int *)argv[4]);
    int yr1 = *((int *)argv[5]);
    int xs1 = *((int *)argv[6]);
    int ys1 = *((int *)argv[7]);
    int xr2 = *((int *)argv[8]);
    int yr2 = *((int *)argv[9]);
    int xs2 = *((int *)argv[10]);
    int ys2 = *((int *)argv[11]);
    int halfcorrelation = *((int *)argv[12]);
    int halfarea = *((int *)argv[13]);
    int balancetype = *((int *)argv[14]);
    int mwidth = *((int *)argv[15]);

    return (im_lrmosaic1(argv[0], argv[1], argv[2], bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, halfcorrelation, halfarea, balancetype, mwidth));
}

static im_function lrmosaic_desc = {"im_lrmosaic", "left-right mosaic of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, lrmosaic_vec, IM_NUMBER(mosaic_args), mosaic_args};

static im_arg_desc find_overlap_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_INPUT_INT("bandno"), IM_INPUT_INT("xr"), IM_INPUT_INT("yr"), IM_INPUT_INT("xs"), IM_INPUT_INT("ys"), IM_INPUT_INT("halfcorrelation"), IM_INPUT_INT("halfarea"), IM_OUTPUT_INT("dx0"), IM_OUTPUT_INT("dy0"), IM_OUTPUT_DOUBLE("scale1"), IM_OUTPUT_DOUBLE("angle1"), IM_OUTPUT_DOUBLE("dx1"), IM_OUTPUT_DOUBLE("dy1")};

static int find_lroverlap_vec(im_object *argv) {
    int bandno = *((int *)argv[2]);
    int xr = *((int *)argv[3]);
    int yr = *((int *)argv[4]);
    int xs = *((int *)argv[5]);
    int ys = *((int *)argv[6]);
    int halfcorrelation = *((int *)argv[7]);
    int halfarea = *((int *)argv[8]);
    int *dx0 = (int *)argv[9];
    int *dy0 = (int *)argv[10];
    double *scale1 = (double *)argv[11];
    double *angle1 = (double *)argv[12];
    double *dx1 = (double *)argv[13];
    double *dy1 = (double *)argv[14];

    IMAGE *t;
    int result;

    if (!(t = im_open("find_lroverlap_vec", "p"))) {
        return (-1);
    }
    result = im__find_lroverlap(argv[0], argv[1], t, bandno, xr, yr, xs, ys, halfcorrelation, halfarea, dx0, dy0, scale1, angle1, dx1, dy1);
    im_close(t);

    return (result);
}

static im_function find_lroverlap_desc = {"im__find_lroverlap", "search for left-right overlap of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, find_lroverlap_vec, IM_NUMBER(find_overlap_args), find_overlap_args};

static im_function lrmosaic1_desc = {"im_lrmosaic1", "first-order left-right mosaic of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, lrmosaic1_vec, IM_NUMBER(mosaic1_args), mosaic1_args};

static int tbmosaic_vec(im_object *argv) {
    int bandno = *((int *)argv[3]);
    int x1 = *((int *)argv[4]);
    int y1 = *((int *)argv[5]);
    int x2 = *((int *)argv[6]);
    int y2 = *((int *)argv[7]);
    int halfcorrelation = *((int *)argv[8]);
    int halfarea = *((int *)argv[9]);
    int balancetype = *((int *)argv[10]);
    int mwidth = *((int *)argv[11]);

    return (im_tbmosaic(argv[0], argv[1], argv[2], bandno, x1, y1, x2, y2, halfcorrelation, halfarea, balancetype, mwidth));
}

static int tbmosaic1_vec(im_object *argv) {
    int bandno = *((int *)argv[3]);
    int xr1 = *((int *)argv[4]);
    int yr1 = *((int *)argv[5]);
    int xs1 = *((int *)argv[6]);
    int ys1 = *((int *)argv[7]);
    int xr2 = *((int *)argv[8]);
    int yr2 = *((int *)argv[9]);
    int xs2 = *((int *)argv[10]);
    int ys2 = *((int *)argv[11]);
    int halfcorrelation = *((int *)argv[12]);
    int halfarea = *((int *)argv[13]);
    int balancetype = *((int *)argv[14]);
    int mwidth = *((int *)argv[15]);

    return (im_tbmosaic1(argv[0], argv[1], argv[2], bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, halfcorrelation, halfarea, balancetype, mwidth));
}

static int find_tboverlap_vec(im_object *argv) {
    int bandno = *((int *)argv[2]);
    int xr = *((int *)argv[3]);
    int yr = *((int *)argv[4]);
    int xs = *((int *)argv[5]);
    int ys = *((int *)argv[6]);
    int halfcorrelation = *((int *)argv[7]);
    int halfarea = *((int *)argv[8]);
    int *dx0 = (int *)argv[9];
    int *dy0 = (int *)argv[10];
    double *scale1 = (double *)argv[11];
    double *angle1 = (double *)argv[12];
    double *dx1 = (double *)argv[13];
    double *dy1 = (double *)argv[14];

    IMAGE *t;
    int result;

    if (!(t = im_open("find_tboverlap_vec", "p"))) {
        return (-1);
    }
    result = im__find_tboverlap(argv[0], argv[1], t, bandno, xr, yr, xs, ys, halfcorrelation, halfarea, dx0, dy0, scale1, angle1, dx1, dy1);
    im_close(t);

    return (result);
}

static im_function find_tboverlap_desc = {"im__find_tboverlap", "search for top-bottom overlap of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, find_tboverlap_vec, IM_NUMBER(find_overlap_args), find_overlap_args};

static im_function tbmosaic_desc = {"im_tbmosaic", "top-bottom mosaic of in1 and in2", IM_FN_TRANSFORM | IM_FN_PIO, tbmosaic_vec, IM_NUMBER(mosaic_args), mosaic_args};

static im_function tbmosaic1_desc = {"im_tbmosaic1", "first-order top-bottom mosaic of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, tbmosaic1_vec, IM_NUMBER(mosaic1_args), mosaic1_args};

static int lrmerge_vec(im_object *argv) {
    int dx = *((int *)argv[3]);
    int dy = *((int *)argv[4]);
    int mwidth = *((int *)argv[5]);

    return (im_lrmerge(argv[0], argv[1], argv[2], dx, dy, mwidth));
}

static int lrmerge1_vec(im_object *argv) {
    int xr1 = *((int *)argv[3]);
    int yr1 = *((int *)argv[4]);
    int xs1 = *((int *)argv[5]);
    int ys1 = *((int *)argv[6]);
    int xr2 = *((int *)argv[7]);
    int yr2 = *((int *)argv[8]);
    int xs2 = *((int *)argv[9]);
    int ys2 = *((int *)argv[10]);
    int mwidth = *((int *)argv[11]);

    return (im_lrmerge1(argv[0], argv[1], argv[2], xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth));
}

static im_function lrmerge_desc = {"im_lrmerge", "left-right merge of in1 and in2", IM_FN_TRANSFORM | IM_FN_PIO, lrmerge_vec, IM_NUMBER(merge_args), merge_args};

static im_function lrmerge1_desc = {"im_lrmerge1", "first-order left-right merge of ref and sec", IM_FN_TRANSFORM | IM_FN_PIO, lrmerge1_vec, IM_NUMBER(merge1_args), merge1_args};

static int tbmerge_vec(im_object *argv) {
    int dx = *((int *)argv[3]);
    int dy = *((int *)argv[4]);
    int mwidth = *((int *)argv[5]);

    return (im_tbmerge(argv[0], argv[1], argv[2], dx, dy, mwidth));
}

static int tbmerge1_vec(im_object *argv) {
    int xr1 = *((int *)argv[3]);
    int yr1 = *((int *)argv[4]);
    int xs1 = *((int *)argv[5]);
    int ys1 = *((int *)argv[6]);
    int xr2 = *((int *)argv[7]);
    int yr2 = *((int *)argv[8]);
    int xs2 = *((int *)argv[9]);
    int ys2 = *((int *)argv[10]);
    int mwidth = *((int *)argv[11]);

    return (im_tbmerge1(argv[0], argv[1], argv[2], xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth));
}

static im_function tbmerge_desc = {"im_tbmerge", "top-bottom merge of in1 and in2", IM_FN_TRANSFORM | IM_FN_PIO, tbmerge_vec, IM_NUMBER(merge_args), merge_args};

static im_function tbmerge1_desc = {"im_tbmerge1", "first-order top-bottom merge of in1 and in2", IM_FN_TRANSFORM | IM_FN_PIO, tbmerge1_vec, IM_NUMBER(merge1_args), merge1_args};

static im_arg_desc match_linear_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xref1"), IM_INPUT_INT("yref1"), IM_INPUT_INT("xsec1"), IM_INPUT_INT("ysec1"), IM_INPUT_INT("xref2"), IM_INPUT_INT("yref2"), IM_INPUT_INT("xsec2"), IM_INPUT_INT("ysec2")};

static int match_linear_vec(im_object *argv) {
    int xref1 = *((int *)argv[3]);
    int yref1 = *((int *)argv[4]);
    int xsec1 = *((int *)argv[5]);
    int ysec1 = *((int *)argv[6]);
    int xref2 = *((int *)argv[7]);
    int yref2 = *((int *)argv[8]);
    int xsec2 = *((int *)argv[9]);
    int ysec2 = *((int *)argv[10]);

    return (im_match_linear(argv[0], argv[1], argv[2], xref1, yref1, xsec1, ysec1, xref2, yref2, xsec2, ysec2));
}

static im_function match_linear_desc = {"im_match_linear", "resample ref so that tie-points match", IM_FN_PIO, match_linear_vec, IM_NUMBER(match_linear_args), match_linear_args};

static im_arg_desc match_linear_search_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xref1"), IM_INPUT_INT("yref1"), IM_INPUT_INT("xsec1"), IM_INPUT_INT("ysec1"), IM_INPUT_INT("xref2"), IM_INPUT_INT("yref2"), IM_INPUT_INT("xsec2"), IM_INPUT_INT("ysec2"), IM_INPUT_INT("hwindowsize"), IM_INPUT_INT("hsearchsize")};

static int match_linear_search_vec(im_object *argv) {
    int xref1 = *((int *)argv[3]);
    int yref1 = *((int *)argv[4]);
    int xsec1 = *((int *)argv[5]);
    int ysec1 = *((int *)argv[6]);
    int xref2 = *((int *)argv[7]);
    int yref2 = *((int *)argv[8]);
    int xsec2 = *((int *)argv[9]);
    int ysec2 = *((int *)argv[10]);
    int hwin = *((int *)argv[11]);
    int hsrch = *((int *)argv[12]);

    return (im_match_linear_search(argv[0], argv[1], argv[2], xref1, yref1, xsec1, ysec1, xref2, yref2, xsec2, ysec2, hwin, hsrch));
}

static im_function match_linear_search_desc = {"im_match_linear_search", "search sec, then resample so that tie-points match", IM_FN_PIO, match_linear_search_vec, IM_NUMBER(match_linear_search_args), match_linear_search_args};

static im_arg_desc correl_args[] = {IM_INPUT_IMAGE("ref"), IM_INPUT_IMAGE("sec"), IM_INPUT_INT("xref"), IM_INPUT_INT("yref"), IM_INPUT_INT("xsec"), IM_INPUT_INT("ysec"), IM_INPUT_INT("hwindowsize"), IM_INPUT_INT("hsearchsize"), IM_OUTPUT_DOUBLE("correlation"), IM_OUTPUT_INT("x"), IM_OUTPUT_INT("y")};

static int correl_vec(im_object *argv) {
    int xref = *((int *)argv[2]);
    int yref = *((int *)argv[3]);
    int xsec = *((int *)argv[4]);
    int ysec = *((int *)argv[5]);
    int cor = *((int *)argv[6]);
    int area = *((int *)argv[7]);
    int *x = (int *)argv[8];
    int *y = (int *)argv[9];
    double *correlation = (double *)argv[10];

    return (im_correl(argv[0], argv[1], xref, yref, xsec, ysec, cor, area, correlation, x, y));
}

static im_function correl_desc = {"im_correl", "search area around sec for match for area around ref", IM_FN_PIO, correl_vec, IM_NUMBER(correl_args), correl_args};

static im_arg_desc global_balance_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("gamma")};

static int global_balance_vec(im_object *argv) {
    double gamma = *((double *)argv[2]);

    return (im_global_balance(argv[0], argv[1], gamma));
}

static im_function global_balance_desc = {"im_global_balance", "automatically rebuild mosaic with balancing", IM_FN_TRANSFORM | IM_FN_PIO, global_balance_vec, IM_NUMBER(global_balance_args), global_balance_args};

static int global_balancef_vec(im_object *argv) {
    double gamma = *((double *)argv[2]);

    return (im_global_balancef(argv[0], argv[1], gamma));
}

static im_function global_balancef_desc = {"im_global_balancef", "automatically rebuild mosaic with balancing, float output", IM_FN_TRANSFORM | IM_FN_PIO, global_balancef_vec, IM_NUMBER(global_balance_args), global_balance_args};

static im_arg_desc remosaic_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("old_str"), IM_INPUT_STRING("new_str")};

static int remosaic_vec(im_object *argv) {
    return (im_remosaic(argv[0], argv[1], argv[2], argv[3]));
}

static im_function remosaic_desc = {"im_remosaic", "automatically rebuild mosaic with new files", IM_FN_TRANSFORM | IM_FN_PIO, remosaic_vec, IM_NUMBER(remosaic_args), remosaic_args};

static int align_bands_vec(im_object *argv) {
    return im_align_bands((IMAGE *)argv[0], (IMAGE *)argv[1]);
}

static im_arg_desc align_bands_arg_types[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out")};

static im_function align_bands_desc = {"im_align_bands", "align the bands of an image", 0, align_bands_vec, IM_NUMBER(align_bands_arg_types), align_bands_arg_types};

static int maxpos_subpel_vec(im_object *argv) {
    return im_maxpos_subpel((IMAGE *)argv[0], (double *)argv[1], (double *)argv[2]);
}

static im_arg_desc maxpos_subpel_arg_types[] = {IM_INPUT_IMAGE("im"), IM_OUTPUT_DOUBLE("x"), IM_OUTPUT_DOUBLE("y")};

static im_function maxpos_subpel_desc = {"im_maxpos_subpel", "subpixel position of maximum of (phase correlation) image", IM_FN_PIO, maxpos_subpel_vec, IM_NUMBER(maxpos_subpel_arg_types), maxpos_subpel_arg_types};

static im_function *mos_list[] = {&align_bands_desc, &correl_desc, &find_lroverlap_desc, &find_tboverlap_desc, &global_balance_desc, &global_balancef_desc, &lrmerge_desc, &lrmerge1_desc, &lrmosaic_desc, &lrmosaic1_desc, &match_linear_desc, &match_linear_search_desc, &maxpos_subpel_desc, &remosaic_desc, &tbmerge_desc, &tbmerge1_desc, &tbmosaic_desc, &tbmosaic1_desc};

im_package im__mosaicing = {"mosaicing", IM_NUMBER(mos_list), mos_list};
