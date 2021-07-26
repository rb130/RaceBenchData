
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

static im_arg_desc image_in_num_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DOUBLE("value")};

static im_arg_desc recomb_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DMASK("matrix")};

static int recomb_vec(im_object *argv) {
    im_mask_object *mo = argv[2];

    return (im_recomb(argv[0], argv[1], mo->mask));
}

static im_function recomb_desc = {"im_recomb", "linear recombination with mask", IM_FN_PIO, recomb_vec, IM_NUMBER(recomb_args), recomb_args};

static int abs_vec(im_object *argv) {
    return (im_abs(argv[0], argv[1]));
}

static im_function abs_desc = {"im_abs", N_("absolute value"), IM_FN_PIO | IM_FN_PTOP, abs_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int add_vec(im_object *argv) {
    return (im_add(argv[0], argv[1], argv[2]));
}

static im_function add_desc = {"im_add", N_("add two images"), IM_FN_PIO | IM_FN_PTOP, add_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int avg_vec(im_object *argv) {
    double f;

    if (im_avg(argv[0], &f)) {
        return (-1);
    }

    *((double *)argv[1]) = f;
    return (0);
}

static im_function avg_desc = {"im_avg", N_("average value of image"), IM_FN_PIO, avg_vec, IM_NUMBER(image_in_num_out), image_in_num_out};

static im_arg_desc point_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_INTERPOLATE("interpolate"), IM_INPUT_DOUBLE("x"), IM_INPUT_DOUBLE("y"), IM_INPUT_INT("band"), IM_OUTPUT_DOUBLE("out")};

static int point_vec(im_object *argv) {
    VipsInterpolate *interpolate = VIPS_INTERPOLATE(argv[1]);
    double x = *((double *)argv[2]);
    double y = *((double *)argv[3]);
    int band = *((double *)argv[4]);

    return (im_point(argv[0], interpolate, x, y, band, argv[5]));
}

static im_function point_desc = {"im_point", "interpolate value at single point", IM_FN_PIO, point_vec, IM_NUMBER(point_args), point_args};

static im_arg_desc point_bilinear_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_DOUBLE("x"), IM_INPUT_DOUBLE("y"), IM_INPUT_INT("band"), IM_OUTPUT_DOUBLE("val")};

static int point_bilinear_vec(im_object *argv) {
    return im_point_bilinear(argv[0], *(double *)argv[1], *(double *)argv[2], *(int *)argv[3], argv[4]);
}

static im_function point_bilinear_desc = {"im_point_bilinear", "interpolate value at single point, linearly", IM_FN_PIO, point_bilinear_vec, IM_NUMBER(point_bilinear_args), point_bilinear_args};

static int deviate_vec(im_object *argv) {
    double f;

    if (im_deviate(argv[0], &f)) {
        return (-1);
    }

    *((double *)argv[1]) = f;
    return (0);
}

static im_function deviate_desc = {"im_deviate", N_("standard deviation of image"), IM_FN_PIO, deviate_vec, IM_NUMBER(image_in_num_out), image_in_num_out};

static int exp10tra_vec(im_object *argv) {
    return (im_exp10tra(argv[0], argv[1]));
}

static im_function exp10tra_desc = {"im_exp10tra", N_("10^pel of image"), IM_FN_PIO | IM_FN_PTOP, exp10tra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int exptra_vec(im_object *argv) {
    return (im_exptra(argv[0], argv[1]));
}

static im_function exptra_desc = {"im_exptra", N_("e^pel of image"), IM_FN_PIO | IM_FN_PTOP, exptra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc powtra_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("x")};

static int expntra_vec(im_object *argv) {
    double a = *((double *)argv[2]);

    return (im_expntra(argv[0], argv[1], a));
}

static im_function expntra_desc = {"im_expntra", N_("x^pel of image"), IM_FN_PIO | IM_FN_PTOP, expntra_vec, IM_NUMBER(powtra_args), powtra_args};

static im_arg_desc expntra_vec_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLEVEC("v")};

static int expntra_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_expntra_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function expntra_vec_desc = {"im_expntra_vec", N_("[x,y,z]^pel of image"), IM_FN_PIO | IM_FN_PTOP, expntra_vec_vec, IM_NUMBER(expntra_vec_args), expntra_vec_args};

static int divide_vec(im_object *argv) {
    return (im_divide(argv[0], argv[1], argv[2]));
}

static im_function divide_desc = {"im_divide", N_("divide two images"), IM_FN_PIO, divide_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int invert_vec(im_object *argv) {
    return (im_invert(argv[0], argv[1]));
}

static im_function invert_desc = {"im_invert", N_("photographic negative"), IM_FN_PIO | IM_FN_PTOP, invert_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc lintra_args[] = {IM_INPUT_DOUBLE("a"), IM_INPUT_IMAGE("in"), IM_INPUT_DOUBLE("b"), IM_OUTPUT_IMAGE("out")};

static int lintra_vec(im_object *argv) {
    double a = *((double *)argv[0]);
    double b = *((double *)argv[2]);

    return (im_lintra(a, argv[1], b, argv[3]));
}

static im_function lintra_desc = {"im_lintra", N_("calculate a*in + b = outfile"), IM_FN_PIO | IM_FN_PTOP, lintra_vec, IM_NUMBER(lintra_args), lintra_args};

static im_arg_desc lintra_vec_args[] = {IM_INPUT_DOUBLEVEC("a"), IM_INPUT_IMAGE("in"), IM_INPUT_DOUBLEVEC("b"), IM_OUTPUT_IMAGE("out")};

static int lintra_vec_vec(im_object *argv) {
    im_doublevec_object *dva = (im_doublevec_object *)argv[0];
    im_doublevec_object *dvb = (im_doublevec_object *)argv[2];

    if (dva->n != dvb->n) {
        im_error("im_lintra_vec", "%s", _("vectors not equal length"));
        return (-1);
    }

    return (im_lintra_vec(dva->n, dva->vec, argv[1], dvb->vec, argv[3]));
}

static im_function lintra_vec_desc = {"im_lintra_vec", N_("calculate a*in + b -> out, a and b vectors"), IM_FN_PIO | IM_FN_PTOP, lintra_vec_vec, IM_NUMBER(lintra_vec_args), lintra_vec_args};

static int log10tra_vec(im_object *argv) {
    return (im_log10tra(argv[0], argv[1]));
}

static im_function log10tra_desc = {"im_log10tra", N_("log10 of image"), IM_FN_PIO | IM_FN_PTOP, log10tra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int logtra_vec(im_object *argv) {
    return (im_logtra(argv[0], argv[1]));
}

static im_function logtra_desc = {"im_logtra", N_("ln of image"), IM_FN_PIO | IM_FN_PTOP, logtra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int tantra_vec(im_object *argv) {
    return (im_tantra(argv[0], argv[1]));
}

static im_function tantra_desc = {"im_tantra", N_("tan of image (angles in degrees)"), IM_FN_PIO | IM_FN_PTOP, tantra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int atantra_vec(im_object *argv) {
    return (im_atantra(argv[0], argv[1]));
}

static im_function atantra_desc = {"im_atantra", N_("atan of image (result in degrees)"), IM_FN_PIO | IM_FN_PTOP, atantra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int costra_vec(im_object *argv) {
    return (im_costra(argv[0], argv[1]));
}

static im_function costra_desc = {"im_costra", N_("cos of image (angles in degrees)"), IM_FN_PIO | IM_FN_PTOP, costra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int acostra_vec(im_object *argv) {
    return (im_acostra(argv[0], argv[1]));
}

static im_function acostra_desc = {"im_acostra", N_("acos of image (result in degrees)"), IM_FN_PIO | IM_FN_PTOP, acostra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int ceil_vec(im_object *argv) {
    return (im_ceil(argv[0], argv[1]));
}

static im_function ceil_desc = {"im_ceil", N_("round to smallest integer value not less than"), IM_FN_PIO | IM_FN_PTOP, ceil_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int floor_vec(im_object *argv) {
    return (im_floor(argv[0], argv[1]));
}

static im_function floor_desc = {"im_floor", N_("round to largest integer value not greater than"), IM_FN_PIO | IM_FN_PTOP, floor_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int rint_vec(im_object *argv) {
    return (im_rint(argv[0], argv[1]));
}

static im_function rint_desc = {"im_rint", N_("round to nearest integer value"), IM_FN_PIO | IM_FN_PTOP, rint_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int sintra_vec(im_object *argv) {
    return (im_sintra(argv[0], argv[1]));
}

static im_function sintra_desc = {"im_sintra", N_("sin of image (angles in degrees)"), IM_FN_PIO | IM_FN_PTOP, sintra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int bandmean_vec(im_object *argv) {
    return (im_bandmean(argv[0], argv[1]));
}

static im_function bandmean_desc = {"im_bandmean", N_("average image bands"), IM_FN_PIO, bandmean_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int sign_vec(im_object *argv) {
    return (im_sign(argv[0], argv[1]));
}

static im_function sign_desc = {"im_sign", N_("unit vector in direction of value"), IM_FN_PIO | IM_FN_PTOP, sign_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int asintra_vec(im_object *argv) {
    return (im_asintra(argv[0], argv[1]));
}

static im_function asintra_desc = {"im_asintra", N_("asin of image (result in degrees)"), IM_FN_PIO | IM_FN_PTOP, asintra_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int max_vec(im_object *argv) {
    double f;

    if (im_max(argv[0], &f)) {
        return (-1);
    }
    *((double *)argv[1]) = f;

    return (0);
}

static im_function max_desc = {"im_max", N_("maximum value of image"), IM_FN_PIO, max_vec, IM_NUMBER(image_in_num_out), image_in_num_out};

static im_arg_desc maxpos_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_COMPLEX("position")};

static int maxpos_vec(im_object *argv) {
    double f;
    int x, y;

    if (im_maxpos(argv[0], &x, &y, &f)) {
        return (-1);
    }

    ((double *)argv[1])[0] = x;
    ((double *)argv[1])[1] = y;

    return (0);
}

static im_function maxpos_desc = {"im_maxpos", N_("position of maximum value of image"), 0, maxpos_vec, IM_NUMBER(maxpos_args), maxpos_args};

static im_arg_desc maxpos_avg_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DOUBLE("x"), IM_OUTPUT_DOUBLE("y"), IM_OUTPUT_DOUBLE("out")};

static int maxpos_avg_vec(im_object *argv) {
    return im_maxpos_avg(argv[0], argv[1], argv[2], argv[3]);
}

static im_function maxpos_avg_desc = {"im_maxpos_avg", N_("position of maximum value of image, averaging in case of draw"), IM_FN_PIO, maxpos_avg_vec, IM_NUMBER(maxpos_avg_args), maxpos_avg_args};

static im_arg_desc maxpos_vec_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_INT("n"), IM_OUTPUT_INTVEC("xes"), IM_OUTPUT_INTVEC("yes"), IM_OUTPUT_DOUBLEVEC("maxima")};

static int maxpos_vec_vec(im_object *argv) {
    int n = *((int *)argv[1]);
    im_intvec_object *xes = argv[2];
    im_intvec_object *yes = argv[3];
    im_doublevec_object *maxima = argv[4];

    xes->vec = IM_ARRAY(NULL, n, int);
    xes->n = n;
    yes->vec = IM_ARRAY(NULL, n, int);
    yes->n = n;
    maxima->vec = IM_ARRAY(NULL, n, double);
    maxima->n = n;
    if (!xes->vec || !yes->vec || !maxima->vec || im_maxpos_vec(argv[0], xes->vec, yes->vec, maxima->vec, n)) {
        return -1;
    }

    return 0;
}

static im_function maxpos_vec_desc = {"im_maxpos_vec", N_("position and value of n maxima of image"), IM_FN_PIO, maxpos_vec_vec, IM_NUMBER(maxpos_vec_args), maxpos_vec_args};

static int minpos_vec_vec(im_object *argv) {
    int n = *((int *)argv[1]);
    im_intvec_object *xes = argv[2];
    im_intvec_object *yes = argv[3];
    im_doublevec_object *minima = argv[4];

    xes->vec = IM_ARRAY(NULL, n, int);
    xes->n = n;
    yes->vec = IM_ARRAY(NULL, n, int);
    yes->n = n;
    minima->vec = IM_ARRAY(NULL, n, double);
    minima->n = n;
    if (!xes->vec || !yes->vec || !minima->vec || im_minpos_vec(argv[0], xes->vec, yes->vec, minima->vec, n)) {
        return -1;
    }

    return 0;
}

static im_function minpos_vec_desc = {"im_minpos_vec", N_("position and value of n minima of image"), IM_FN_PIO, minpos_vec_vec, IM_NUMBER(maxpos_vec_args), maxpos_vec_args};

static im_arg_desc measure_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DMASK("mask"), IM_INPUT_INT("x"), IM_INPUT_INT("y"), IM_INPUT_INT("w"), IM_INPUT_INT("h"), IM_INPUT_INT("h_patches"), IM_INPUT_INT("v_patches")};

static int measure_vec(im_object *argv) {
    im_mask_object *mo = argv[1];

    int x = *((int *)argv[2]);
    int y = *((int *)argv[3]);
    int w = *((int *)argv[4]);
    int h = *((int *)argv[5]);

    int u = *((int *)argv[6]);
    int v = *((int *)argv[7]);

    if (!(mo->mask = im_measure_area(argv[0], x, y, w, h, u, v, NULL, 0, mo->name))) {
        return (-1);
    }

    return (0);
}

static im_function measure_desc = {"im_measure", N_("measure averages of a grid of patches"), IM_FN_PIO, measure_vec, IM_NUMBER(measure_args), measure_args};

static int min_vec(im_object *argv) {
    double f;

    if (im_min(argv[0], &f)) {
        return (-1);
    }
    *((double *)argv[1]) = f;

    return (0);
}

static im_function min_desc = {"im_min", N_("minimum value of image"), IM_FN_PIO, min_vec, IM_NUMBER(image_in_num_out), image_in_num_out};

static int minpos_vec(im_object *argv) {
    double f;
    int x, y;

    if (im_minpos(argv[0], &x, &y, &f)) {
        return (-1);
    }

    ((double *)argv[1])[0] = x;
    ((double *)argv[1])[1] = y;

    return (0);
}

static im_function minpos_desc = {"im_minpos", N_("position of minimum value of image"), 0, minpos_vec, IM_NUMBER(maxpos_args), maxpos_args};

static int remainder_vec(im_object *argv) {
    return (im_remainder(argv[0], argv[1], argv[2]));
}

static im_function remainder_desc = {"im_remainder", N_("remainder after integer division"), IM_FN_PIO | IM_FN_PTOP, remainder_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int remainderconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_remainderconst(argv[0], argv[1], c));
}

static im_arg_desc remainderconst_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("x")};

static im_function remainderconst_desc = {"im_remainderconst", N_("remainder after integer division by a constant"), IM_FN_PIO | IM_FN_PTOP, remainderconst_vec, IM_NUMBER(remainderconst_args), remainderconst_args};

static int remainder_vec_vec(im_object *argv) {
    im_doublevec_object *dv = (im_doublevec_object *)argv[2];

    return (im_remainder_vec(argv[0], argv[1], dv->n, dv->vec));
}

static im_arg_desc remainder_vec_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLEVEC("x")};

static im_function remainder_vec_desc = {"im_remainder_vec", N_("remainder after integer division by a vector of constants"),

    IM_FN_PIO | IM_FN_PTOP, remainder_vec_vec, IM_NUMBER(remainder_vec_args), remainder_vec_args};

static int multiply_vec(im_object *argv) {
    return (im_multiply(argv[0], argv[1], argv[2]));
}

static im_function multiply_desc = {"im_multiply", N_("multiply two images"), IM_FN_PIO | IM_FN_PTOP, multiply_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int powtra_vec(im_object *argv) {
    double a = *((double *)argv[2]);

    return (im_powtra(argv[0], argv[1], a));
}

static im_function powtra_desc = {"im_powtra", N_("pel^x of image"), IM_FN_PIO | IM_FN_PTOP, powtra_vec, IM_NUMBER(powtra_args), powtra_args};

static int powtra_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_powtra_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function powtra_vec_desc = {"im_powtra_vec", N_("pel^[x,y,z] of image"), IM_FN_PIO | IM_FN_PTOP, powtra_vec_vec, IM_NUMBER(expntra_vec_args), expntra_vec_args};

static im_arg_desc stats_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_DMASK_STATS("statistics")};

static int stats_vec(im_object *argv) {
    im_mask_object *mo = argv[1];

    if (!(mo->mask = im_stats(argv[0]))) {
        return (-1);
    }

    return (0);
}

static im_function stats_desc = {"im_stats", N_("many image statistics in one pass"), IM_FN_PIO, stats_vec, IM_NUMBER(stats_args), stats_args};

static int subtract_vec(im_object *argv) {
    return (im_subtract(argv[0], argv[1], argv[2]));
}

static im_function subtract_desc = {"im_subtract", N_("subtract two images"), IM_FN_PIO, subtract_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc linreg_args[] = {IM_INPUT_IMAGEVEC("ins"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLEVEC("xs")};

static int linreg_vec(im_object *argv) {
#define FUNCTION_NAME "im_linreg_vec"
    im_imagevec_object *ins_vec = (im_imagevec_object *)argv[0];
    im_doublevec_object *xs_vec = (im_doublevec_object *)argv[2];
    IMAGE *out = (IMAGE *)argv[1];
    IMAGE **ins = IM_ARRAY(out, ins_vec->n + 1, IMAGE *);
    int i;

    if (!ins) {
        return -1;
    }

    for (i = 0; i < ins_vec->n; ++i) {
        ins[i] = ins_vec->vec[i];
    }

    ins[ins_vec->n] = NULL;

    if (xs_vec->n != ins_vec->n) {
        im_error(FUNCTION_NAME, "image vector and x vector differ in length");
        return -1;
    }
    return im_linreg(ins, out, xs_vec->vec);

#undef FUNCTION_NAME
}

static im_function linreg_desc = {"im_linreg", N_("pixelwise linear regression"), IM_FN_PIO | IM_FN_PTOP, linreg_vec, IM_NUMBER(linreg_args), linreg_args};

static int cross_phase_vec(im_object *argv) {
    return (im_cross_phase(argv[0], argv[1], argv[2]));
}

static im_function cross_phase_desc = {"im_cross_phase", N_("phase of cross power spectrum of two complex images"), IM_FN_PIO | IM_FN_PTOP, cross_phase_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_function *arith_list[] = {&abs_desc, &acostra_desc, &add_desc, &asintra_desc, &atantra_desc, &avg_desc, &point_desc, &point_bilinear_desc, &bandmean_desc, &ceil_desc, &costra_desc, &cross_phase_desc, &deviate_desc, &divide_desc, &exp10tra_desc, &expntra_desc, &expntra_vec_desc, &exptra_desc, &floor_desc, &invert_desc, &lintra_desc, &linreg_desc, &lintra_vec_desc, &log10tra_desc, &logtra_desc, &max_desc, &maxpos_desc, &maxpos_avg_desc, &maxpos_vec_desc, &measure_desc, &min_desc, &minpos_desc, &minpos_vec_desc, &multiply_desc, &powtra_desc, &powtra_vec_desc, &recomb_desc, &remainder_desc, &remainderconst_desc, &remainder_vec_desc, &rint_desc, &sign_desc, &sintra_desc, &stats_desc, &subtract_desc, &tantra_desc};

im_package im__arithmetic = {"arithmetic", IM_NUMBER(arith_list), arith_list};
