

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

static im_arg_desc const_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLE("c")};

static im_arg_desc vec_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_DOUBLEVEC("vec")};

static int equal_vec(im_object *argv) {
    return (im_equal(argv[0], argv[1], argv[2]));
}

static im_function equal_desc = {"im_equal", "two images equal in value", IM_FN_PTOP | IM_FN_PIO, equal_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int equalconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_equalconst(argv[0], argv[1], c));
}

static im_function equalconst_desc = {"im_equalconst", "image equals const", IM_FN_PTOP | IM_FN_PIO, equalconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int equal_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_equal_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function equal_vec_desc = {"im_equal_vec", "image equals doublevec", IM_FN_PTOP | IM_FN_PIO, equal_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int notequal_vec(im_object *argv) {
    return (im_notequal(argv[0], argv[1], argv[2]));
}

static im_function notequal_desc = {"im_notequal", "two images not equal in value", IM_FN_PTOP | IM_FN_PIO, notequal_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int notequalconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_notequalconst(argv[0], argv[1], c));
}

static im_function notequalconst_desc = {"im_notequalconst", "image does not equal const", IM_FN_PTOP | IM_FN_PIO, notequalconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int notequal_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_notequal_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function notequal_vec_desc = {"im_notequal_vec", "image does not equal doublevec", IM_FN_PTOP | IM_FN_PIO, notequal_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int less_vec(im_object *argv) {
    return (im_less(argv[0], argv[1], argv[2]));
}

static im_function less_desc = {"im_less", "in1 less than in2 in value", IM_FN_PTOP | IM_FN_PIO, less_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int lessconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_lessconst(argv[0], argv[1], c));
}

static im_function lessconst_desc = {"im_lessconst", "in less than const", IM_FN_PTOP | IM_FN_PIO, lessconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int less_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_less_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function less_vec_desc = {"im_less_vec", "in less than doublevec", IM_FN_PTOP | IM_FN_PIO, less_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int more_vec(im_object *argv) {
    return (im_more(argv[0], argv[1], argv[2]));
}

static im_function more_desc = {"im_more", "in1 more than in2 in value", IM_FN_PTOP | IM_FN_PIO, more_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int moreconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_moreconst(argv[0], argv[1], c));
}

static im_function moreconst_desc = {"im_moreconst", "in more than const", IM_FN_PTOP | IM_FN_PIO, moreconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int more_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_more_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function more_vec_desc = {"im_more_vec", "in more than doublevec", IM_FN_PTOP | IM_FN_PIO, more_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int moreeq_vec(im_object *argv) {
    return (im_moreeq(argv[0], argv[1], argv[2]));
}

static im_function moreeq_desc = {"im_moreeq", "in1 more than or equal to in2 in value", IM_FN_PTOP | IM_FN_PIO, moreeq_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int moreeqconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_moreeqconst(argv[0], argv[1], c));
}

static im_function moreeqconst_desc = {"im_moreeqconst", "in more than or equal to const", IM_FN_PTOP | IM_FN_PIO, moreeqconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int moreeq_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_moreeq_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function moreeq_vec_desc = {"im_moreeq_vec", "in more than or equal to doublevec", IM_FN_PTOP | IM_FN_PIO, moreeq_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static int lesseq_vec(im_object *argv) {
    return (im_lesseq(argv[0], argv[1], argv[2]));
}

static im_function lesseq_desc = {"im_lesseq", "in1 less than or equal to in2 in value", IM_FN_PTOP | IM_FN_PIO, lesseq_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int lesseqconst_vec(im_object *argv) {
    double c = *((double *)argv[2]);

    return (im_lesseqconst(argv[0], argv[1], c));
}

static im_function lesseqconst_desc = {"im_lesseqconst", "in less than or equal to const", IM_FN_PTOP | IM_FN_PIO, lesseqconst_vec, IM_NUMBER(const_in_one_out), const_in_one_out};

static int lesseq_vec_vec(im_object *argv) {
    im_doublevec_object *rv = (im_doublevec_object *)argv[2];

    return (im_lesseq_vec(argv[0], argv[1], rv->n, rv->vec));
}

static im_function lesseq_vec_desc = {"im_lesseq_vec", "in less than or equal to doublevec", IM_FN_PTOP | IM_FN_PIO, lesseq_vec_vec, IM_NUMBER(vec_in_one_out), vec_in_one_out};

static im_arg_desc ifthenelse_args[] = {IM_INPUT_IMAGE("cond"), IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out")};

static int blend_vec(im_object *argv) {
    return (im_blend(argv[0], argv[1], argv[2], argv[3]));
}

static im_function blend_desc = {"im_blend", "use cond image to blend between images in1 and in2", IM_FN_PTOP | IM_FN_PIO, blend_vec, IM_NUMBER(ifthenelse_args), ifthenelse_args};

static int ifthenelse_vec(im_object *argv) {
    return (im_ifthenelse(argv[0], argv[1], argv[2], argv[3]));
}

static im_function ifthenelse_desc = {"im_ifthenelse", "use cond image to choose pels from image in1 or in2", IM_FN_PTOP | IM_FN_PIO, ifthenelse_vec, IM_NUMBER(ifthenelse_args), ifthenelse_args};

static im_function *relational_list[] = {&blend_desc, &equal_desc, &equal_vec_desc, &equalconst_desc, &ifthenelse_desc, &less_desc, &less_vec_desc, &lessconst_desc, &lesseq_desc, &lesseq_vec_desc, &lesseqconst_desc, &more_desc, &more_vec_desc, &moreconst_desc, &moreeq_desc, &moreeq_vec_desc, &moreeqconst_desc, &notequal_desc, &notequal_vec_desc, &notequalconst_desc};

im_package im__relational = {"relational", IM_NUMBER(relational_list), relational_list};
