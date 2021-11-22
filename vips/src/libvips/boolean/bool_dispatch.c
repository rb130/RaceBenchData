

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc two_in_one_out[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc const_in_one_out[] = {IM_INPUT_IMAGE("in1"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("c")};

static im_arg_desc vec_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLEVEC("vec")};

static int andimage_vec(im_object *argv) {
    return (im_andimage(argv[0], argv[1], argv[2]));
}

static im_function andimage_desc = {"im_andimage", "bitwise and of two images", IM_FN_PTOP | IM_FN_PIO, andimage_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int andimageconst_vec(im_object *argv) {
    int c = *((int *)argv[2]);

    return (im_andimageconst(argv[0], argv[1], c));
}

static im_function andimageconst_desc = {"im_andimageconst", "bitwise and of an image with a constant", IM_FN_PTOP | IM_FN_PIO, andimageconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int andimage_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_andimage_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function andimage_vec_desc = {"im_andimage_vec", "bitwise and of an image with a vector constant", IM_FN_PTOP | IM_FN_PIO, andimage_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int orimage_vec(im_object *argv) {
    return (im_orimage(argv[0], argv[1], argv[2]));
}

static im_function orimage_desc = {"im_orimage", "bitwise or of two images", IM_FN_PTOP | IM_FN_PIO, orimage_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int orimageconst_vec(im_object *argv) {
    int c = *((int *)argv[2]);

    return (im_orimageconst(argv[0], argv[1], c));
}

static im_function orimageconst_desc = {"im_orimageconst", "bitwise or of an image with a constant", IM_FN_PTOP | IM_FN_PIO, orimageconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int orimage_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_orimage_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function orimage_vec_desc = {"im_orimage_vec", "bitwise or of an image with a vector constant", IM_FN_PTOP | IM_FN_PIO, orimage_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int eorimage_vec(im_object *argv) {
    return (im_eorimage(argv[0], argv[1], argv[2]));
}

static im_function eorimage_desc = {"im_eorimage", "bitwise eor of two images", IM_FN_PTOP | IM_FN_PIO, eorimage_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int eorimageconst_vec(im_object *argv) {
    int c = *((int *)argv[2]);

    return (im_eorimageconst(argv[0], argv[1], c));
}

static im_function eorimageconst_desc = {"im_eorimageconst", "bitwise eor of an image with a constant", IM_FN_PTOP | IM_FN_PIO, eorimageconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int eorimage_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_eorimage_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function eorimage_vec_desc = {"im_eorimage_vec", "bitwise eor of an image with a vector constant", IM_FN_PTOP | IM_FN_PIO, eorimage_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int shiftleft_vec(im_object *argv) {
    int n = *((int *)argv[2]);

    return (im_shiftleft(argv[0], argv[1], n));
}

static im_function shiftleft_desc = {"im_shiftleft", "shift image n bits to left", IM_FN_PTOP | IM_FN_PIO, shiftleft_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int shiftleft_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_shiftleft_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function shiftleft_vec_desc = {"im_shiftleft_vec", "shift image array bits to left", IM_FN_PTOP | IM_FN_PIO, shiftleft_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int shiftright_vec(im_object *argv) {
    int n = *((int *)argv[2]);

    return (im_shiftright(argv[0], argv[1], n));
}

static im_function shiftright_desc = {"im_shiftright", "shift integer image n bits to right", IM_FN_PTOP | IM_FN_PIO, shiftright_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int shiftright_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_shiftright_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function shiftright_vec_desc = {"im_shiftright_vec", "shift image array bits to right", IM_FN_PTOP | IM_FN_PIO, shiftright_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static im_function *bool_list[] = {&andimage_desc, &andimageconst_desc, &andimage_vec_desc, &orimage_desc, &orimageconst_desc, &orimage_vec_desc, &eorimage_desc, &eorimageconst_desc, &eorimage_vec_desc, &shiftleft_vec_desc, &shiftleft_desc, &shiftright_vec_desc, &shiftright_desc};

im_package im__boolean = {"boolean", IM_NUMBER(bool_list), bool_list};
