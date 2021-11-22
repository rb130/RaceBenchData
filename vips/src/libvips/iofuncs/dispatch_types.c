

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_STR (4096)

#define VEC_SEPS " "

static int input_display_init(im_object *obj, char *str) {
    struct im_col_display *scr = im_col_display_name(str);

    if (!scr) {
        int i;

        im_error("input_display", _("unknown display type \"%s\""), str);
        im_error("input_display", "%s", _("display should be one of:\n"));
        for (i = 0; (scr = im_col_displays(i)); i++) {
            im_error("input_display", "  '%s'\n", scr->d_name);
        }

        return (-1);
    }

    *obj = scr;

    return (0);
}

im_type_desc im__input_display = {IM_TYPE_DISPLAY, 0, IM_TYPE_ARG, input_display_init, NULL};

im_type_desc im__output_display = {IM_TYPE_DISPLAY, sizeof(struct im_col_display), IM_TYPE_OUTPUT, NULL, NULL};

static int input_image_init(im_object *obj, char *str) {
    IMAGE **im = (IMAGE **)obj;

    return (!(*im = im_open(str, "r")));
}

im_type_desc im__input_image = {IM_TYPE_IMAGE, 0, IM_TYPE_ARG, (im_init_obj_fn)input_image_init, (im_dest_obj_fn)im_close};

static int output_image_init(im_object *obj, char *str) {
    IMAGE **im = (IMAGE **)obj;

    return (!(*im = im_open(str, "w")));
}

im_type_desc im__output_image = {IM_TYPE_IMAGE, 0, IM_TYPE_OUTPUT | IM_TYPE_ARG, (im_init_obj_fn)output_image_init, (im_dest_obj_fn)im_close};

static int rw_image_init(im_object *obj, char *str) {
    IMAGE **im = (IMAGE **)obj;

    return (!(*im = im_open(str, "rw")));
}

im_type_desc im__rw_image = {IM_TYPE_IMAGE, 0, IM_TYPE_ARG, (im_init_obj_fn)rw_image_init, (im_dest_obj_fn)im_close};

static int imagevec_dest(im_object obj) {
    im_imagevec_object *iv = obj;

    if (iv->vec) {
        int i;

        for (i = 0; i < iv->n; i++) {
            if (iv->vec[i]) {
                im_close(iv->vec[i]);
                iv->vec[i] = NULL;
            }
        }

        im_free(iv->vec);
        iv->vec = NULL;
        iv->n = 0;
    }

    return (0);
}

static int input_imagevec_init(im_object *obj, char *str) {
    im_imagevec_object *iv = *obj;
    char **strv;
    int nargs;
    int i;

    strv = g_strsplit(str, VEC_SEPS, -1);
    nargs = g_strv_length(strv);

    if (!(iv->vec = IM_ARRAY(NULL, nargs, IMAGE *))) {
        g_strfreev(strv);
        return (-1);
    }
    iv->n = nargs;

    for (i = 0; i < nargs; i++) {
        iv->vec[i] = NULL;
    }

    for (i = 0; i < nargs; i++) {
        if (!(iv->vec[i] = im_open(strv[i], "r"))) {
            g_strfreev(strv);
            return (-1);
        }
    }

    g_strfreev(strv);

    return (0);
}

im_type_desc im__input_imagevec = {IM_TYPE_IMAGEVEC, sizeof(im_imagevec_object), IM_TYPE_ARG, input_imagevec_init, imagevec_dest};

static int mask_init(im_object *obj, char *str) {
    im_mask_object *mo = *obj;

    if (str && !(mo->name = im_strdup(NULL, str))) {
        return (-1);
    }
    mo->mask = NULL;

    return (0);
}

static int dmask_init(im_object *obj, char *str) {
    im_mask_object *mo = *obj;

    if (mask_init(obj, str)) {
        return (-1);
    }
    if (!(mo->mask = im_read_dmask(str))) {
        return (-1);
    }

    return (0);
}

static int imask_init(im_object *obj, char *str) {
    im_mask_object *mo = *obj;

    if (mask_init(obj, str)) {
        return (-1);
    }
    if (!(mo->mask = im_read_imask(str))) {
        return (-1);
    }

    return (0);
}

static int dmask_dest(im_object obj) {
    im_mask_object *mo = obj;

    if (mo->name) {
        im_free(mo->name);
        mo->name = NULL;
    }
    if (mo->mask) {
        im_free_dmask((DOUBLEMASK *)mo->mask);
        mo->mask = NULL;
    }

    return (0);
}

static int imask_dest(im_object obj) {
    im_mask_object *mo = obj;

    if (mo->name) {
        im_free(mo->name);
        mo->name = NULL;
    }
    if (mo->mask) {
        im_free_imask((INTMASK *)mo->mask);
        mo->mask = NULL;
    }

    return (0);
}

static int save_dmask_dest(im_object obj) {
    im_mask_object *mo = obj;

    if (mo->mask && im_write_dmask(mo->mask)) {
        return (-1);
    }
    return (dmask_dest(obj));
}

static int save_imask_dest(im_object obj) {
    im_mask_object *mo = obj;

    if (mo->mask && im_write_imask(mo->mask)) {
        return (-1);
    }
    return (imask_dest(obj));
}

im_type_desc im__output_dmask = {IM_TYPE_DMASK, sizeof(im_mask_object), IM_TYPE_OUTPUT | IM_TYPE_ARG, mask_init, save_dmask_dest};

im_type_desc im__input_dmask = {IM_TYPE_DMASK, sizeof(im_mask_object), IM_TYPE_ARG, dmask_init, dmask_dest};

im_type_desc im__output_imask = {IM_TYPE_IMASK, sizeof(im_mask_object), IM_TYPE_OUTPUT | IM_TYPE_ARG, mask_init, save_imask_dest};

im_type_desc im__input_imask = {IM_TYPE_IMASK, sizeof(im_mask_object), IM_TYPE_ARG, imask_init, imask_dest};

im_type_desc im__output_dmask_screen = {IM_TYPE_DMASK, sizeof(im_mask_object), IM_TYPE_OUTPUT, mask_init, dmask_dest};

static int input_double_init(im_object *obj, char *str) {
    double *d = (double *)*obj;

    *d = g_ascii_strtod(str, NULL);

    return (0);
}

im_type_desc im__input_double = {IM_TYPE_DOUBLE, sizeof(double), IM_TYPE_ARG, input_double_init, NULL};

static int doublevec_dest(im_object obj) {
    im_doublevec_object *dv = obj;

    if (dv->vec) {
        im_free(dv->vec);
        dv->vec = NULL;
        dv->n = 0;
    }

    return (0);
}

static int input_doublevec_init(im_object *obj, char *str) {
    im_doublevec_object *dv = *obj;
    char **strv;
    int nargs;
    int i;

    strv = g_strsplit(str, VEC_SEPS, -1);
    nargs = g_strv_length(strv);

    if (!(dv->vec = IM_ARRAY(NULL, nargs, double))) {
        g_strfreev(strv);
        return (-1);
    }
    dv->n = nargs;

    for (i = 0; i < nargs; i++) {
        dv->vec[i] = g_ascii_strtod(strv[i], NULL);
        if (errno) {
            im_error_system(errno, "input_doublevec_init", _("bad double \"%s\""), strv[i]);
            g_strfreev(strv);
            return (-1);
        }
    }

    g_strfreev(strv);

    return (0);
}

im_type_desc im__input_doublevec = {IM_TYPE_DOUBLEVEC, sizeof(im_doublevec_object), IM_TYPE_ARG, input_doublevec_init, doublevec_dest};

int im__dvprint(im_object obj) {
    im_doublevec_object *dv = obj;
    int i;

    for (i = 0; i < dv->n; i++) {
        printf("%G ", dv->vec[i]);
    }
    printf("\n");

    return (0);
}

im_type_desc im__output_doublevec = {IM_TYPE_DOUBLEVEC, sizeof(im_doublevec_object), IM_TYPE_OUTPUT, NULL, doublevec_dest};

static int intvec_dest(im_object obj) {
    im_intvec_object *iv = obj;

    if (iv->vec) {
        im_free(iv->vec);
        iv->vec = NULL;
        iv->n = 0;
    }

    return (0);
}

static int input_intvec_init(im_object *obj, char *str) {
    im_intvec_object *iv = *obj;
    char **strv;
    int nargs;
    int i;

    strv = g_strsplit(str, VEC_SEPS, -1);
    nargs = g_strv_length(strv);

    if (!(iv->vec = IM_ARRAY(NULL, nargs, int))) {
        g_strfreev(strv);
        return (-1);
    }
    iv->n = nargs;

    for (i = 0; i < nargs; i++) {
        long int val = strtol(strv[i], NULL, 10);

        if (errno) {
            im_error_system(errno, "input_intvec_init", _("bad integer \"%s\""), strv[i]);
            g_strfreev(strv);
            return (-1);
        }
        if (INT_MAX < val || INT_MIN > val) {
            im_error("input_intvec_init", "%ld overflows integer type", val);
        }
        iv->vec[i] = (int)val;
    }

    g_strfreev(strv);

    return (0);
}

im_type_desc im__input_intvec = {IM_TYPE_INTVEC, sizeof(im_intvec_object), IM_TYPE_ARG, input_intvec_init, intvec_dest};

int im__ivprint(im_object obj) {
    im_intvec_object *iv = obj;
    int i;

    for (i = 0; i < iv->n; i++) {
        printf("%d ", iv->vec[i]);
    }
    printf("\n");

    return (0);
}

im_type_desc im__output_intvec = {IM_TYPE_INTVEC, sizeof(im_intvec_object), IM_TYPE_OUTPUT, (im_init_obj_fn)NULL, (im_dest_obj_fn)intvec_dest};

static int input_int_init(im_object *obj, char *str) {
    int *i = (int *)*obj;

    if (sscanf(str, "%d", i) != 1) {
        im_error("input_int", "%s", _("bad format"));
        return (-1);
    }

    return (0);
}

im_type_desc im__input_int = {IM_TYPE_INT, sizeof(int), IM_TYPE_ARG, input_int_init, NULL};

static int input_string_init(im_object *obj, char *str) {
    if (!(*obj = (im_object)im_strdup(NULL, str))) {
        return (-1);
    }

    return (0);
}

im_type_desc im__input_string = {IM_TYPE_STRING, 0, IM_TYPE_ARG, input_string_init, im_free};

im_type_desc im__output_string = {IM_TYPE_STRING, 0, IM_TYPE_OUTPUT, NULL, im_free};

im_type_desc im__output_double = {IM_TYPE_DOUBLE, sizeof(double), IM_TYPE_OUTPUT, NULL, NULL};

im_type_desc im__output_complex = {IM_TYPE_COMPLEX, 2 * sizeof(double), IM_TYPE_OUTPUT, NULL, NULL};

im_type_desc im__output_int = {IM_TYPE_INT, sizeof(int), IM_TYPE_OUTPUT, NULL, NULL};

int im__iprint(im_object obj) {
    int *i = (int *)obj;

    printf("%d\n", *i);

    return (0);
}

int im__sprint(im_object obj) {
    char *s = (char *)obj;

    printf("%s\n", s);

    return (0);
}

int im__dprint(im_object obj) {
    double *d = (double *)obj;

    printf("%G\n", *d);

    return (0);
}

int im__cprint(im_object obj) {
    double *d = (double *)obj;

    printf("%G %G\n", d[0], d[1]);

    return (0);
}

int im__dmsprint(im_object obj) {
    DOUBLEMASK *mask = ((im_mask_object *)obj)->mask;
    double *row;
    int i, j;

    printf("\
band minimum maximum sum sum^2 mean deviation\
\n");
    for (j = 0; j < mask->ysize; j++) {
        row = mask->coeff + j * 6;
        if (j == 0) {
            printf("all");
        } else {
            printf("%2d ", j);
        }

        for (i = 0; i < 6; i++) {
            printf("%12g", row[i]);
        }
        printf("\n");
    }

    return (0);
}

static char *decode_dtype(enum im_col_disp_type type) {
    switch (type) {
    case DISP_BARCO:
        return ("DISP_BARCO");
    case DISP_DUMB:
        return ("DISP_DUMB");
    default:
        return ("<unknown display type>");
    }
}

int im__displayprint(im_object obj) {
    struct im_col_display *scr = (struct im_col_display *)obj;

    printf("im_col_display:\n");
    printf("\td_name: %s\n", scr->d_name);
    printf("\td_type: %s\n", decode_dtype(scr->d_type));
    printf("\td_mat:\n");
    printf("\t\t %g %g %g\n", scr->d_mat[0][0], scr->d_mat[0][1], scr->d_mat[0][2]);
    printf("\t\t %g %g %g\n", scr->d_mat[1][0], scr->d_mat[1][1], scr->d_mat[1][2]);
    printf("\t\t %g %g %g\n", scr->d_mat[2][0], scr->d_mat[2][1], scr->d_mat[2][2]);

    printf("\td_YCW: %g\n", scr->d_YCW);
    printf("\td_xCW: %g\n", scr->d_xCW);
    printf("\td_yCW: %g\n", scr->d_yCW);

    printf("\td_YCR: %g\n", scr->d_YCR);
    printf("\td_YCG: %g\n", scr->d_YCG);
    printf("\td_YCB: %g\n", scr->d_YCB);

    printf("\td_Vrwr: %d\n", scr->d_Vrwr);
    printf("\td_Vrwg: %d\n", scr->d_Vrwg);
    printf("\td_Vrwb: %d\n", scr->d_Vrwb);

    printf("\td_Y0R: %g\n", scr->d_Y0R);
    printf("\td_Y0G: %g\n", scr->d_Y0G);
    printf("\td_Y0B: %g\n", scr->d_Y0B);

    printf("\td_gammaR: %g\n", scr->d_gammaR);
    printf("\td_gammaG: %g\n", scr->d_gammaG);
    printf("\td_gammaB: %g\n", scr->d_gammaB);

    printf("\td_B: %g\n", scr->d_B);
    printf("\td_P: %g\n", scr->d_P);

    return (0);
}

static int input_gvalue_init(im_object *obj, char *str) {
    GValue *value = *obj;

    g_value_init(value, G_TYPE_STRING);
    g_value_set_string(value, str);

    return (0);
}

static int gvalue_free(im_object obj) {
    GValue *value = obj;

    g_value_unset(value);

    return (0);
}

im_type_desc im__input_gvalue = {IM_TYPE_GVALUE, sizeof(GValue), IM_TYPE_ARG, (im_init_obj_fn)input_gvalue_init, (im_dest_obj_fn)gvalue_free};

int im__gprint(im_object obj) {
    GValue *value = obj;
    char *str_value;

    str_value = g_strdup_value_contents(value);
    printf("%s\n", str_value);
    g_free(str_value);

    return (0);
}

static int output_gvalue_init(im_object *obj) {
    GValue *value = *obj;

    memset(value, 0, sizeof(GValue));

    return (0);
}

im_type_desc im__output_gvalue = {IM_TYPE_GVALUE, sizeof(GValue), IM_TYPE_OUTPUT, (im_init_obj_fn)output_gvalue_init, (im_dest_obj_fn)gvalue_free};

static int input_interpolate_init(im_object *obj, char *str) {
    VipsObject *object;

    if (!(object = vips_object_new_from_string("VipsInterpolate", str))) {
        return (-1);
    }
    *obj = object;

    return (0);
}

static int input_interpolate_dest(im_object obj) {
    g_object_unref((GObject *)obj);

    return (0);
}

im_type_desc im__input_interpolate = {IM_TYPE_INTERPOLATE, 0, IM_TYPE_ARG, input_interpolate_init, input_interpolate_dest};
