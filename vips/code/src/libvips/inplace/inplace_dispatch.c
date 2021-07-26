
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static im_arg_desc circle_args[] = {IM_RW_IMAGE("image"), IM_INPUT_INT("cx"), IM_INPUT_INT("cy"), IM_INPUT_INT("radius"), IM_INPUT_INT("intensity")};

static int circle_vec(im_object *argv) {
    int cx = *((int *)argv[1]);
    int cy = *((int *)argv[2]);
    int radius = *((int *)argv[3]);
    int intensity = *((int *)argv[4]);

    return (im_circle(argv[0], cx, cy, radius, intensity));
}

static im_function circle_desc = {"im_circle", "plot circle on image", 0, circle_vec, IM_NUMBER(circle_args), circle_args};

static im_arg_desc insertplace_args[] = {IM_RW_IMAGE("main"), IM_INPUT_IMAGE("sub"), IM_INPUT_INT("x"), IM_INPUT_INT("y")};

static int insertplace_vec(im_object *argv) {
    int x = *((int *)argv[2]);
    int y = *((int *)argv[3]);

    return (im_insertplace(argv[0], argv[1], x, y));
}

static im_function insertplace_desc = {"im_insertplace", "draw image sub inside image main at position (x,y)", 0, insertplace_vec, IM_NUMBER(insertplace_args), insertplace_args};

static im_arg_desc lineset_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_IMAGE("mask"), IM_INPUT_IMAGE("ink"), IM_INPUT_INTVEC("x1"), IM_INPUT_INTVEC("y1"), IM_INPUT_INTVEC("x2"), IM_INPUT_INTVEC("y2")};

static int lineset_vec(im_object *argv) {
    im_intvec_object *x1v = (im_intvec_object *)argv[4];
    im_intvec_object *y1v = (im_intvec_object *)argv[5];
    im_intvec_object *x2v = (im_intvec_object *)argv[6];
    im_intvec_object *y2v = (im_intvec_object *)argv[7];

    if (x1v->n != y1v->n || x1v->n != x2v->n || x1v->n != y2v->n) {
        im_error("im_lineset", "%s", _("vectors not same length"));
        return (-1);
    }

    return (im_lineset(argv[0], argv[1], argv[2], argv[3], x1v->n, x1v->vec, y1v->vec, x2v->vec, y2v->vec));
}

static im_function lineset_desc = {"im_lineset", "draw line between points (x1,y1) and (x2,y2)", 0, lineset_vec, IM_NUMBER(lineset_args), lineset_args};

static PEL *vector_to_ink(IMAGE *im, double *vec) {
    const int n = im->Bands;

    IMAGE *t[3];
    double *zeros;
    int i;

    if (im_open_local_array(im, t, 3, "vector_to_ink", "t") || !(zeros = IM_ARRAY(im, n, double))) {
        return (NULL);
    }
    for (i = 0; i < n; i++) {
        zeros[i] = 0.0;
    }

    if (im_black(t[0], 1, 1, n) || im_lintra_vec(n, zeros, t[0], vec, t[1]) || im_clip2fmt(t[1], t[2], im->BandFmt)) {
        return (NULL);
    }

    return ((PEL *)t[2]->data);
}

static im_arg_desc flood_blob_copy_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("start_x"), IM_INPUT_INT("start_y"), IM_INPUT_DOUBLEVEC("ink")};

static int flood_blob_copy_vec(im_object *argv) {
    IMAGE *in = argv[0];
    IMAGE *out = argv[1];
    int start_x = *((int *)argv[2]);
    int start_y = *((int *)argv[3]);
    im_doublevec_object *dv = (im_doublevec_object *)argv[4];

    PEL *ink;

    if (dv->n != in->Bands) {
        im_error("im_flood_blob_copy", "%s", _("bad vector length"));
        return (-1);
    }
    if (!(ink = vector_to_ink(in, dv->vec))) {
        return (-1);
    }

    return (im_flood_blob_copy(in, out, start_x, start_y, ink));
}

static im_function flood_blob_copy_desc = {"im_flood_blob_copy", "flood with ink from start_x, start_y while pixel == start pixel", 0, flood_blob_copy_vec, IM_NUMBER(flood_blob_copy_args), flood_blob_copy_args};

static im_arg_desc flood_copy_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("start_x"), IM_INPUT_INT("start_y"), IM_INPUT_DOUBLEVEC("ink")};

static int flood_copy_vec(im_object *argv) {
    IMAGE *in = argv[0];
    IMAGE *out = argv[1];
    int start_x = *((int *)argv[2]);
    int start_y = *((int *)argv[3]);
    im_doublevec_object *dv = (im_doublevec_object *)argv[4];

    PEL *ink;

    if (dv->n != in->Bands) {
        im_error("im_flood_copy", "%s", _("bad vector length"));
        return (-1);
    }
    if (!(ink = vector_to_ink(in, dv->vec))) {
        return (-1);
    }

    return (im_flood_copy(in, out, start_x, start_y, ink));
}

static im_function flood_copy_desc = {"im_flood_copy", "flood with ink from start_x, start_y while pixel == start pixel", 0, flood_copy_vec, IM_NUMBER(flood_copy_args), flood_copy_args};

static im_arg_desc flood_other_copy_args[] = {IM_INPUT_IMAGE("test"), IM_INPUT_IMAGE("mark"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("start_x"), IM_INPUT_INT("start_y"), IM_INPUT_INT("serial")};

static int flood_other_copy_vec(im_object *argv) {
    IMAGE *test = argv[0];
    IMAGE *mark = argv[1];
    IMAGE *out = argv[2];
    int start_x = *((int *)argv[3]);
    int start_y = *((int *)argv[4]);
    int serial = *((int *)argv[5]);

    return (im_flood_other_copy(test, mark, out, start_x, start_y, serial));
}

static im_function flood_other_copy_desc = {"im_flood_other_copy", "flood mark with serial from start_x, start_y while pixel == start pixel", 0, flood_other_copy_vec, IM_NUMBER(flood_other_copy_args), flood_other_copy_args};

static im_function *inplace_list[] = {&circle_desc, &flood_copy_desc, &flood_blob_copy_desc, &flood_other_copy_desc, &insertplace_desc, &lineset_desc};

im_package im__inplace = {"inplace", IM_NUMBER(inplace_list), inplace_list};
