

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _HeaderField {
    const char *field;
    glong offset;
} HeaderField;

static HeaderField int_field[] = {{"Xsize", G_STRUCT_OFFSET(IMAGE, Xsize)}, {"Ysize", G_STRUCT_OFFSET(IMAGE, Ysize)}, {"Bands", G_STRUCT_OFFSET(IMAGE, Bands)}, {"Bbits", G_STRUCT_OFFSET(IMAGE, Bbits)}, {"BandFmt", G_STRUCT_OFFSET(IMAGE, BandFmt)}, {"Coding", G_STRUCT_OFFSET(IMAGE, Coding)}, {"Type", G_STRUCT_OFFSET(IMAGE, Type)}, {"Xoffset", G_STRUCT_OFFSET(IMAGE, Xoffset)}, {"Yoffset", G_STRUCT_OFFSET(IMAGE, Yoffset)}};

static HeaderField double_field[] = {{"Xres", G_STRUCT_OFFSET(IMAGE, Xres)}, {"Yres", G_STRUCT_OFFSET(IMAGE, Yres)}};

static HeaderField string_field[] = {{"filename", G_STRUCT_OFFSET(IMAGE, filename)}};

int im_header_int(IMAGE *im, const char *field, int *out) {
    int i;

    for (i = 0; i < IM_NUMBER(int_field); i++) {
        if (strcmp(field, int_field[i].field) == 0) {
            *out = G_STRUCT_MEMBER(int, im, int_field[i].offset);
            break;
        }
    }

    if (i == IM_NUMBER(int_field) && im_meta_get_int(im, field, out)) {
        im_error("im_header_int", _("no such int field \"%s\""), field);
        return (-1);
    }

    return (0);
}

int im_header_double(IMAGE *im, const char *field, double *out) {
    int i;

    for (i = 0; i < IM_NUMBER(double_field); i++) {
        if (strcmp(field, double_field[i].field) == 0) {
            *out = G_STRUCT_MEMBER(float, im, double_field[i].offset);
            break;
        }
    }

    if (i == IM_NUMBER(double_field) && im_meta_get_double(im, field, out)) {
        im_error("im_header_double", _("no such double field \"%s\""), field);
        return (-1);
    }

    return (0);
}

int im_header_string(IMAGE *im, const char *field, char **out) {
    int i;

    for (i = 0; i < IM_NUMBER(string_field); i++) {
        if (strcmp(field, string_field[i].field) == 0) {
            *out = G_STRUCT_MEMBER(char *, im, string_field[i].offset);
            break;
        }
    }

    if (i == IM_NUMBER(string_field) && im_meta_get_string(im, field, out)) {
        im_error("im_header_string", _("no such string field \"%s\""), field);
        return (-1);
    }

    return (0);
}

int im_header_as_string(IMAGE *im, const char *field, char **out) {
    GValue value = {0};
    GType type;

    if (im_header_get(im, field, &value)) {
        return (-1);
    }

    type = G_VALUE_TYPE(&value);
    if (g_value_type_transformable(type, IM_TYPE_SAVE_STRING)) {
        GValue save_value = {0};

        g_value_init(&save_value, IM_TYPE_SAVE_STRING);
        if (!g_value_transform(&value, &save_value)) {
            return (-1);
        }
        *out = g_strdup(im_save_string_get(&save_value));
        g_value_unset(&save_value);
    } else {
        *out = g_strdup_value_contents(&value);
    }

    g_value_unset(&value);

    return (0);
}

GType im_header_get_typeof(IMAGE *im, const char *field) {
    int i;
    GType type;

    for (i = 0; i < IM_NUMBER(int_field); i++) {
        if (strcmp(field, int_field[i].field) == 0) {
            return (G_TYPE_INT);
        }
    }
    for (i = 0; i < IM_NUMBER(double_field); i++) {
        if (strcmp(field, double_field[i].field) == 0) {
            return (G_TYPE_DOUBLE);
        }
    }
    for (i = 0; i < IM_NUMBER(string_field); i++) {
        if (strcmp(field, string_field[i].field) == 0) {
            return (G_TYPE_STRING);
        }
    }
    if ((type = im_meta_get_typeof(im, field))) {
        return (type);
    }

    return (0);
}

int im_header_get(IMAGE *im, const char *field, GValue *value_copy) {
    int i;

    for (i = 0; i < IM_NUMBER(int_field); i++) {
        if (strcmp(field, int_field[i].field) == 0) {
            g_value_init(value_copy, G_TYPE_INT);
            g_value_set_int(value_copy, G_STRUCT_MEMBER(int, im, int_field[i].offset));
            return (0);
        }
    }

    for (i = 0; i < IM_NUMBER(double_field); i++) {
        if (strcmp(field, double_field[i].field) == 0) {
            g_value_init(value_copy, G_TYPE_DOUBLE);
            g_value_set_double(value_copy, G_STRUCT_MEMBER(float, im, double_field[i].offset));
            return (0);
        }
    }

    for (i = 0; i < IM_NUMBER(string_field); i++) {
        if (strcmp(field, string_field[i].field) == 0) {
            g_value_init(value_copy, G_TYPE_STRING);
            g_value_set_static_string(value_copy, G_STRUCT_MEMBER(char *, im, string_field[i].offset));
            return (0);
        }
    }

    if (!im_meta_get(im, field, value_copy)) {
        return (0);
    }

    return (-1);
}

static void *header_map_fn(Meta *meta, im_header_map_fn fn, void *a) {
    return (fn(meta->im, meta->field, &meta->value, a));
}

void *im_header_map(IMAGE *im, im_header_map_fn fn, void *a) {
    int i;
    GValue value = {0};
    void *result;

    for (i = 0; i < IM_NUMBER(int_field); i++) {
        im_header_get(im, int_field[i].field, &value);
        result = fn(im, int_field[i].field, &value, a);
        g_value_unset(&value);

        if (result) {
            return (result);
        }
    }

    for (i = 0; i < IM_NUMBER(double_field); i++) {
        im_header_get(im, double_field[i].field, &value);
        result = fn(im, double_field[i].field, &value, a);
        g_value_unset(&value);

        if (result) {
            return (result);
        }
    }

    for (i = 0; i < IM_NUMBER(string_field); i++) {
        im_header_get(im, string_field[i].field, &value);
        result = fn(im, string_field[i].field, &value, a);
        g_value_unset(&value);

        if (result) {
            return (result);
        }
    }

    if (im->Meta_traverse && (result = im_slist_map2(im->Meta_traverse, (VSListMap2Fn)header_map_fn, fn, a))) {
        return (result);
    }

    return (NULL);
}
