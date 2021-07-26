
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

static int sRGB2XYZ_vec(im_object *argv) {
    return (im_sRGB2XYZ(argv[0], argv[1]));
}

static im_function sRGB2XYZ_desc = {"im_sRGB2XYZ", "convert sRGB to XYZ", IM_FN_PIO, sRGB2XYZ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int XYZ2sRGB_vec(im_object *argv) {
    return (im_XYZ2sRGB(argv[0], argv[1]));
}

static im_function XYZ2sRGB_desc = {"im_XYZ2sRGB", "convert XYZ to sRGB", IM_FN_PIO, XYZ2sRGB_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LCh2Lab_vec(im_object *argv) {
    return (im_LCh2Lab(argv[0], argv[1]));
}

static im_function LCh2Lab_desc = {"im_LCh2Lab", "convert LCh to Lab", IM_FN_PIO, LCh2Lab_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LabQ2XYZ_vec(im_object *argv) {
    return (im_LabQ2XYZ(argv[0], argv[1]));
}

static im_function LabQ2XYZ_desc = {"im_LabQ2XYZ", "convert LabQ to XYZ", IM_FN_PIO, LabQ2XYZ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LCh2UCS_vec(im_object *argv) {
    return (im_LCh2UCS(argv[0], argv[1]));
}

static im_function LCh2UCS_desc = {"im_LCh2UCS", "convert LCh to UCS", IM_FN_PIO, LCh2UCS_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int Lab2LCh_vec(im_object *argv) {
    return (im_Lab2LCh(argv[0], argv[1]));
}

static im_function Lab2LCh_desc = {"im_Lab2LCh", "convert Lab to LCh", IM_FN_PIO, Lab2LCh_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int Lab2LabQ_vec(im_object *argv) {
    return (im_Lab2LabQ(argv[0], argv[1]));
}

static im_function Lab2LabQ_desc = {"im_Lab2LabQ", "convert Lab to LabQ", IM_FN_PIO, Lab2LabQ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int Lab2XYZ_vec(im_object *argv) {
    return (im_Lab2XYZ(argv[0], argv[1]));
}

static im_function Lab2XYZ_desc = {"im_Lab2XYZ", "convert D65 Lab to XYZ", IM_FN_PIO, Lab2XYZ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int icc_present_vec(im_object *argv) {
    int *present = ((int *)argv[0]);

    *present = im_icc_present();

    return (0);
}

static im_arg_desc icc_present_args[] = {IM_OUTPUT_INT("present")};

static im_function icc_present_desc = {"im_icc_present", "test for presence of ICC library", 0, icc_present_vec, IM_NUMBER(icc_present_args), icc_present_args};

static int icc_transform_vec(im_object *argv) {
    int intent = *((int *)argv[4]);

    return (im_icc_transform(argv[0], argv[1], argv[2], argv[3], intent));
}

static im_arg_desc icc_transform_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("input_profile"), IM_INPUT_STRING("output_profile"), IM_INPUT_INT("intent")};

static im_function icc_transform_desc = {"im_icc_transform", "convert between two device images with a pair of ICC profiles",

    IM_FN_PIO, icc_transform_vec, IM_NUMBER(icc_transform_args), icc_transform_args};

static int icc_import_embedded_vec(im_object *argv) {
    int intent = *((int *)argv[2]);

    return (im_icc_import_embedded(argv[0], argv[1], intent));
}

static im_arg_desc icc_import_embedded_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("intent")};

static im_function icc_import_embedded_desc = {"im_icc_import_embedded", "convert a device image to float LAB using the embedded profile",

    IM_FN_PIO, icc_import_embedded_vec, IM_NUMBER(icc_import_embedded_args), icc_import_embedded_args};

static int icc_import_vec(im_object *argv) {
    int intent = *((int *)argv[3]);

    return (im_icc_import(argv[0], argv[1], argv[2], intent));
}

static im_arg_desc icc_import_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("input_profile"), IM_INPUT_INT("intent")};

static im_function icc_import_desc = {"im_icc_import", "convert a device image to float LAB with an ICC profile",

    IM_FN_PIO, icc_import_vec, IM_NUMBER(icc_import_args), icc_import_args};

static int icc_export_depth_vec(im_object *argv) {
    int intent = *((int *)argv[4]);
    int depth = *((int *)argv[2]);

    return (im_icc_export_depth(argv[0], argv[1], depth, argv[3], intent));
}

static im_arg_desc icc_export_depth_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("depth"), IM_INPUT_STRING("output_profile"), IM_INPUT_INT("intent")};

static im_function icc_export_depth_desc = {"im_icc_export_depth", "convert a float LAB to device space with an ICC profile",

    IM_FN_PIO, icc_export_depth_vec, IM_NUMBER(icc_export_depth_args), icc_export_depth_args};

static int icc_ac2rc_vec(im_object *argv) {
    return (im_icc_ac2rc(argv[0], argv[1], argv[2]));
}

static im_arg_desc icc_ac2rc_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("profile")};

static im_function icc_ac2rc_desc = {"im_icc_ac2rc", "convert LAB from AC to RC using an ICC profile",

    IM_FN_PIO, icc_ac2rc_vec, IM_NUMBER(icc_ac2rc_args), icc_ac2rc_args};

static int Lab2XYZ_temp_vec(im_object *argv) {
    double X0 = *((double *)argv[2]);
    double Y0 = *((double *)argv[3]);
    double Z0 = *((double *)argv[4]);

    return (im_Lab2XYZ_temp(argv[0], argv[1], X0, Y0, Z0));
}

static im_arg_desc temp_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("X0"), IM_INPUT_DOUBLE("Y0"), IM_INPUT_DOUBLE("Z0")};

static im_function Lab2XYZ_temp_desc = {"im_Lab2XYZ_temp", "convert Lab to XYZ, with a specified colour temperature",

    IM_FN_PIO, Lab2XYZ_temp_vec, IM_NUMBER(temp_args), temp_args};

static int Lab2UCS_vec(im_object *argv) {
    return (im_Lab2UCS(argv[0], argv[1]));
}

static im_function Lab2UCS_desc = {"im_Lab2UCS", "convert Lab to UCS", IM_FN_PIO, Lab2UCS_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LabQ2Lab_vec(im_object *argv) {
    return (im_LabQ2Lab(argv[0], argv[1]));
}

static im_function LabQ2Lab_desc = {"im_LabQ2Lab", "convert LabQ to Lab", IM_FN_PIO, LabQ2Lab_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int rad2float_vec(im_object *argv) {
    return (im_rad2float(argv[0], argv[1]));
}

static im_function rad2float_desc = {"im_rad2float", "convert Radiance packed to float", IM_FN_PIO, rad2float_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int float2rad_vec(im_object *argv) {
    return (im_float2rad(argv[0], argv[1]));
}

static im_function float2rad_desc = {"im_float2rad", "convert float to Radiance packed", IM_FN_PIO, float2rad_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LabQ2LabS_vec(im_object *argv) {
    return (im_LabQ2LabS(argv[0], argv[1]));
}

static im_function LabQ2LabS_desc = {"im_LabQ2LabS", "convert LabQ to LabS", IM_FN_PIO, LabQ2LabS_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int Lab2LabS_vec(im_object *argv) {
    return (im_Lab2LabS(argv[0], argv[1]));
}

static im_function Lab2LabS_desc = {"im_Lab2LabS", "convert Lab to LabS", IM_FN_PIO, Lab2LabS_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LabS2Lab_vec(im_object *argv) {
    return (im_LabS2Lab(argv[0], argv[1]));
}

static im_function LabS2Lab_desc = {"im_LabS2Lab", "convert LabS to Lab", IM_FN_PIO, LabS2Lab_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int LabS2LabQ_vec(im_object *argv) {
    return (im_LabS2LabQ(argv[0], argv[1]));
}

static im_function LabS2LabQ_desc = {"im_LabS2LabQ", "convert LabS to LabQ", IM_FN_PIO, LabS2LabQ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int UCS2XYZ_vec(im_object *argv) {
    return (im_UCS2XYZ(argv[0], argv[1]));
}

static im_function UCS2XYZ_desc = {"im_UCS2XYZ", "convert UCS to XYZ", IM_FN_PIO, UCS2XYZ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int UCS2LCh_vec(im_object *argv) {
    return (im_UCS2LCh(argv[0], argv[1]));
}

static im_function UCS2LCh_desc = {"im_UCS2LCh", "convert UCS to LCh", IM_FN_PIO, UCS2LCh_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int UCS2Lab_vec(im_object *argv) {
    return (im_UCS2Lab(argv[0], argv[1]));
}

static im_function UCS2Lab_desc = {"im_UCS2Lab", "convert UCS to Lab", IM_FN_PIO, UCS2Lab_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int Yxy2XYZ_vec(im_object *argv) {
    return (im_Yxy2XYZ(argv[0], argv[1]));
}

static im_function Yxy2XYZ_desc = {"im_Yxy2XYZ", "convert Yxy to XYZ", IM_FN_PIO, Yxy2XYZ_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int XYZ2Yxy_vec(im_object *argv) {
    return (im_XYZ2Yxy(argv[0], argv[1]));
}

static im_function XYZ2Yxy_desc = {"im_XYZ2Yxy", "convert XYZ to Yxy", IM_FN_PIO, XYZ2Yxy_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int XYZ2Lab_vec(im_object *argv) {
    return (im_XYZ2Lab(argv[0], argv[1]));
}

static im_function XYZ2Lab_desc = {"im_XYZ2Lab", "convert D65 XYZ to Lab", IM_FN_PIO, XYZ2Lab_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int XYZ2Lab_temp_vec(im_object *argv) {
    double X0 = *((double *)argv[2]);
    double Y0 = *((double *)argv[3]);
    double Z0 = *((double *)argv[4]);

    return (im_XYZ2Lab_temp(argv[0], argv[1], X0, Y0, Z0));
}

static im_function XYZ2Lab_temp_desc = {"im_XYZ2Lab_temp", "convert XYZ to Lab, with a specified colour temperature",

    IM_FN_PIO, XYZ2Lab_temp_vec, IM_NUMBER(temp_args), temp_args};

static int XYZ2UCS_vec(im_object *argv) {
    return (im_XYZ2UCS(argv[0], argv[1]));
}

static im_function XYZ2UCS_desc = {"im_XYZ2UCS", "convert XYZ to UCS", IM_FN_PIO, XYZ2UCS_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc XYZ2disp_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DISPLAY("disp")};

static int XYZ2disp_vec(im_object *argv) {
    return (im_XYZ2disp(argv[0], argv[1], argv[2]));
}

static im_function XYZ2disp_desc = {"im_XYZ2disp", "convert XYZ to displayble", IM_FN_PIO, XYZ2disp_vec, IM_NUMBER(XYZ2disp_args), XYZ2disp_args};

static int Lab2disp_vec(im_object *argv) {
    return (im_Lab2disp(argv[0], argv[1], argv[2]));
}

static im_function Lab2disp_desc = {"im_Lab2disp", "convert Lab to displayable", IM_FN_PIO, Lab2disp_vec, IM_NUMBER(XYZ2disp_args), XYZ2disp_args};

static int LabQ2disp_vec(im_object *argv) {
    return (im_LabQ2disp(argv[0], argv[1], argv[2]));
}

static im_function LabQ2disp_desc = {"im_LabQ2disp", "convert LabQ to displayable", IM_FN_PIO, LabQ2disp_vec, IM_NUMBER(XYZ2disp_args), XYZ2disp_args};

static int dE00_fromLab_vec(im_object *argv) {
    return (im_dE00_fromLab(argv[0], argv[1], argv[2]));
}

static im_function dE00_fromLab_desc = {"im_dE00_fromLab", "calculate delta-E CIE2000 for two Lab images", IM_FN_PIO, dE00_fromLab_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int dECMC_fromLab_vec(im_object *argv) {
    return (im_dECMC_fromLab(argv[0], argv[1], argv[2]));
}

static im_function dECMC_fromLab_desc = {"im_dECMC_fromLab", "calculate delta-E CMC(1:1) for two Lab images", IM_FN_PIO, dECMC_fromLab_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int dE_fromXYZ_vec(im_object *argv) {
    return (im_dE_fromXYZ(argv[0], argv[1], argv[2]));
}

static im_function dE_fromXYZ_desc = {"im_dE_fromXYZ", "calculate delta-E for two XYZ images", IM_FN_PIO, dE_fromXYZ_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int dE_fromLab_vec(im_object *argv) {
    return (im_dE_fromLab(argv[0], argv[1], argv[2]));
}

static im_function dE_fromLab_desc = {"im_dE_fromLab", "calculate delta-E for two Lab images", IM_FN_PIO, dE_fromLab_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc dE_fromdisp_args[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DISPLAY("disp")};

static int dE_fromdisp_vec(im_object *argv) {
    return (im_dE_fromdisp(argv[0], argv[1], argv[2], argv[3]));
}

static im_function dE_fromdisp_desc = {"im_dE_fromdisp", "calculate delta-E for two displayable images", IM_FN_PIO, dE_fromdisp_vec, IM_NUMBER(dE_fromdisp_args), dE_fromdisp_args};

static int dECMC_fromdisp_vec(im_object *argv) {
    return (im_dECMC_fromdisp(argv[0], argv[1], argv[2], argv[3]));
}

static im_function dECMC_fromdisp_desc = {"im_dECMC_fromdisp", "calculate delta-E CMC(1:1) for two displayable images", IM_FN_PIO, dECMC_fromdisp_vec, IM_NUMBER(dE_fromdisp_args), dE_fromdisp_args};

static int disp2XYZ_vec(im_object *argv) {
    return (im_disp2XYZ(argv[0], argv[1], argv[2]));
}

static im_function disp2XYZ_desc = {"im_disp2XYZ", "convert displayable to XYZ", IM_FN_PIO, disp2XYZ_vec, IM_NUMBER(XYZ2disp_args), XYZ2disp_args};

static int disp2Lab_vec(im_object *argv) {
    return (im_disp2Lab(argv[0], argv[1], argv[2]));
}

static im_function disp2Lab_desc = {"im_disp2Lab", "convert displayable to Lab", IM_FN_PIO, disp2Lab_vec, IM_NUMBER(XYZ2disp_args), XYZ2disp_args};

static im_arg_desc morph_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DMASK("greyscale"), IM_INPUT_DOUBLE("L_offset"), IM_INPUT_DOUBLE("L_scale"), IM_INPUT_DOUBLE("a_scale"), IM_INPUT_DOUBLE("b_scale")};

static int morph_vec(im_object *argv) {
    im_mask_object *mo = argv[2];
    double L_offset = *((double *)argv[3]);
    double L_scale = *((double *)argv[4]);
    double a_scale = *((double *)argv[5]);
    double b_scale = *((double *)argv[6]);

    return (im_lab_morph(argv[0], argv[1], mo->mask, L_offset, L_scale, a_scale, b_scale));
}

static im_function morph_desc = {"im_lab_morph", "morph colourspace of a LAB image", IM_FN_PIO | IM_FN_PTOP, morph_vec, IM_NUMBER(morph_args), morph_args};

static im_function *colour_list[] = {&LCh2Lab_desc, &LCh2UCS_desc, &Lab2LCh_desc, &Lab2LabQ_desc, &Lab2LabS_desc, &Lab2UCS_desc, &Lab2XYZ_desc, &Lab2XYZ_temp_desc, &Lab2disp_desc, &LabQ2LabS_desc, &LabQ2Lab_desc, &LabQ2XYZ_desc, &LabQ2disp_desc, &LabS2LabQ_desc, &LabS2Lab_desc, &UCS2LCh_desc, &UCS2Lab_desc, &UCS2XYZ_desc, &XYZ2Lab_desc, &XYZ2Lab_temp_desc, &XYZ2UCS_desc, &XYZ2Yxy_desc, &XYZ2disp_desc, &XYZ2sRGB_desc, &Yxy2XYZ_desc, &dE00_fromLab_desc, &dECMC_fromLab_desc, &dECMC_fromdisp_desc, &dE_fromLab_desc, &dE_fromXYZ_desc, &dE_fromdisp_desc, &disp2Lab_desc, &disp2XYZ_desc, &float2rad_desc, &icc_ac2rc_desc, &icc_export_depth_desc, &icc_import_desc, &icc_import_embedded_desc, &icc_present_desc, &icc_transform_desc, &morph_desc, &rad2float_desc, &sRGB2XYZ_desc};

im_package im__colour = {"colour", IM_NUMBER(colour_list), colour_list};
