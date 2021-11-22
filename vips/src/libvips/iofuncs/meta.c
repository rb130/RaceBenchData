

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>

#include "base64.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef DEBUG

static void *meta_sanity_on_hash(Meta *meta, IMAGE *im) {
    Meta *found;

    if (meta->im != im)
        printf("*** field \"%s\" has incorrect im\n", meta->field);

    if (!(found = g_hash_table_lookup(im->Meta, meta->field)))
        printf("*** field \"%s\" is on traverse but not in hash\n", meta->field);

    if (found != meta)
        printf("*** meta \"%s\" on traverse and hash do not match\n", meta->field);

    return (NULL);
}

static void meta_sanity_on_traverse(const char *field, Meta *meta, IMAGE *im) {
    if (meta->field != field)
        printf("*** field \"%s\" has incorrect field\n", meta->field);

    if (meta->im != im)
        printf("*** field \"%s\" has incorrect im\n", meta->field);

    if (!g_slist_find(im->Meta_traverse, meta))
        printf("*** field \"%s\" is in hash but not on traverse\n", meta->field);
}

static void meta_sanity(const IMAGE *im) {
    if (im->Meta)
        g_hash_table_foreach(im->Meta, (GHFunc)meta_sanity_on_traverse, (void *)im);
    im_slist_map2(im->Meta_traverse, (VSListMap2Fn)meta_sanity_on_hash, (void *)im, NULL);
}
#endif

static void meta_free(Meta *meta) {
#ifdef DEBUG
    {
        char *str_value;

        str_value = g_strdup_value_contents(&meta->value);
        printf("meta_free: field %s, value = %s\n", meta->field, str_value);
        g_free(str_value);
    }
#endif

    if (meta->im) {
        meta->im->Meta_traverse = g_slist_remove(meta->im->Meta_traverse, meta);
    }

    g_value_unset(&meta->value);
    IM_FREE(meta->field);
    im_free(meta);
}

static Meta *meta_new(IMAGE *im, const char *field, GValue *value) {
    Meta *meta;

    if (!(meta = IM_NEW(NULL, Meta))) {
        return (NULL);
    }
    meta->im = im;
    meta->field = NULL;
    memset(&meta->value, 0, sizeof(GValue));

    if (!(meta->field = im_strdup(NULL, field))) {
        meta_free(meta);
        return (NULL);
    }

    g_value_init(&meta->value, G_VALUE_TYPE(value));
    g_value_copy(value, &meta->value);

    im->Meta_traverse = g_slist_append(im->Meta_traverse, meta);
    g_hash_table_replace(im->Meta, meta->field, meta);

#ifdef DEBUG
    {
        char *str_value;

        str_value = g_strdup_value_contents(value);
        printf("meta_new: field %s, value = %s\n", field, str_value);
        g_free(str_value);
    }
#endif

    return (meta);
}

void im__meta_destroy(IMAGE *im) {
    IM_FREEF(g_hash_table_destroy, im->Meta);
    g_assert(!im->Meta_traverse);
}

static void meta_init(IMAGE *im) {
    if (!im->Meta) {
        g_assert(!im->Meta_traverse);
        im->Meta = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)meta_free);
    }
}

static void *meta_cp_field(Meta *meta, IMAGE *dst) {
    Meta *meta_copy;

#ifdef DEBUG
    {
        char *str_value;

        str_value = g_strdup_value_contents(&meta->value);
        printf("im__meta_cp: copying field %s, value = %s\n", meta->field, str_value);
        g_free(str_value);
    }
#endif

    meta_copy = meta_new(dst, meta->field, &meta->value);

#ifdef DEBUG
    meta_sanity(dst);
#endif

    return (NULL);
}

int im__meta_cp(IMAGE *dst, const IMAGE *src) {
    if (src->Meta) {

        meta_init(dst);
        im_slist_map2(src->Meta_traverse, (VSListMap2Fn)meta_cp_field, dst, NULL);
    }

    return (0);
}

int im_meta_set(IMAGE *im, const char *field, GValue *value) {
    Meta *meta;

    g_assert(field);
    g_assert(value);

    meta_init(im);
    if (!(meta = meta_new(im, field, value))) {
        return (-1);
    }

#ifdef DEBUG
    meta_sanity(im);
#endif

    return (0);
}

gboolean im_meta_remove(IMAGE *im, const char *field) {
    if (im->Meta && g_hash_table_remove(im->Meta, field)) {
        return (TRUE);
    }

    return (FALSE);
}

int im_meta_get(IMAGE *im, const char *field, GValue *value_copy) {
    Meta *meta;

    g_assert(field);
    g_assert(value_copy);

    if (!im->Meta || !(meta = g_hash_table_lookup(im->Meta, field))) {
        im_error("im_meta_get", _("field \"%s\" not found"), field);
        return (-1);
    }

    g_value_init(value_copy, G_VALUE_TYPE(&meta->value));
    g_value_copy(&meta->value, value_copy);

    return (0);
}

GType im_meta_get_typeof(IMAGE *im, const char *field) {
    Meta *meta;

    g_assert(field);

    if (!im->Meta || !(meta = g_hash_table_lookup(im->Meta, field))) {
        return (0);
    }

    return (G_VALUE_TYPE(&meta->value));
}

static int meta_set_value(IMAGE *im, const char *field, GValue *value) {
    if (im_meta_set(im, field, value)) {
        g_value_unset(value);
        return (-1);
    }
    g_value_unset(value);

    return (0);
}

static int meta_get_value(IMAGE *im, const char *field, GType type, GValue *value_copy) {
    if (im_meta_get(im, field, value_copy)) {
        return (-1);
    }
    if (G_VALUE_TYPE(value_copy) != type) {
        im_error("im_meta_get",
            _("field \"%s\" "
              "is of type %s, not %s"),
            field, g_type_name(G_VALUE_TYPE(value_copy)), g_type_name(type));
        g_value_unset(value_copy);
        return (-1);
    }

    return (0);
}

GType im_save_string_get_type(void) {
    static GType type = 0;

    if (!type) {
        type = g_boxed_type_register_static("im_save_string", (GBoxedCopyFunc)g_strdup, (GBoxedFreeFunc)g_free);
    }

    return (type);
}

const char *im_save_string_get(const GValue *value) {
    return ((char *)g_value_get_boxed(value));
}

void im_save_string_set(GValue *value, const char *str) {
    g_assert(G_VALUE_TYPE(value) == IM_TYPE_SAVE_STRING);

    g_value_set_boxed(value, str);
}

void im_save_string_setf(GValue *value, const char *fmt, ...) {
    va_list ap;
    char *str;

    g_assert(G_VALUE_TYPE(value) == IM_TYPE_SAVE_STRING);

    va_start(ap, fmt);
    str = g_strdup_vprintf(fmt, ap);
    va_end(ap);
    im_save_string_set(value, str);
    g_free(str);
}

int im_meta_set_int(IMAGE *im, const char *field, int i) {
    GValue value = {0};

    g_value_init(&value, G_TYPE_INT);
    g_value_set_int(&value, i);

    return (meta_set_value(im, field, &value));
}

int im_meta_get_int(IMAGE *im, const char *field, int *i) {
    GValue value_copy = {0};

    if (meta_get_value(im, field, G_TYPE_INT, &value_copy)) {
        return (-1);
    }
    *i = g_value_get_int(&value_copy);
    g_value_unset(&value_copy);

    return (0);
}

int im_meta_set_double(IMAGE *im, const char *field, double d) {
    GValue value = {0};

    g_value_init(&value, G_TYPE_DOUBLE);
    g_value_set_double(&value, d);

    return (meta_set_value(im, field, &value));
}

int im_meta_get_double(IMAGE *im, const char *field, double *d) {
    GValue value_copy = {0};

    if (meta_get_value(im, field, G_TYPE_DOUBLE, &value_copy)) {
        return (-1);
    }
    *d = g_value_get_double(&value_copy);
    g_value_unset(&value_copy);

    return (0);
}

static void transform_int_save_string(const GValue *src_value, GValue *dest_value) {
    im_save_string_setf(dest_value, "%d", g_value_get_int(src_value));
}

static void transform_save_string_int(const GValue *src_value, GValue *dest_value) {
    g_value_set_int(dest_value, atoi(im_save_string_get(src_value)));
}

static void transform_double_save_string(const GValue *src_value, GValue *dest_value) {
    char buf[G_ASCII_DTOSTR_BUF_SIZE];

    g_ascii_dtostr(buf, G_ASCII_DTOSTR_BUF_SIZE, g_value_get_double(src_value));
    im_save_string_set(dest_value, buf);
}

static void transform_save_string_double(const GValue *src_value, GValue *dest_value) {
    g_value_set_double(dest_value, g_ascii_strtod(im_save_string_get(src_value), NULL));
}

typedef struct _Area {
    int count;
    size_t length;
    void *data;
    im_callback_fn free_fn;
} Area;

#ifdef DEBUG
static int area_number = 0;
#endif

static Area *area_new(im_callback_fn free_fn, void *data) {
    Area *area;

    if (!(area = IM_NEW(NULL, Area))) {
        return (NULL);
    }
    area->count = 1;
    area->length = 0;
    area->data = data;
    area->free_fn = free_fn;

#ifdef DEBUG
    area_number += 1;
    printf("area_new: %p count = %d (%d in total)\n", area, area->count, area_number);
#endif

    return (area);
}

static Area *area_new_blob(im_callback_fn free_fn, void *blob, size_t blob_length) {
    Area *area;

    if (!(area = area_new(free_fn, blob))) {
        return (NULL);
    }
    area->length = blob_length;

    return (area);
}

static Area *area_copy(Area *area) {
    g_assert(area->count >= 0);

    area->count += 1;

#ifdef DEBUG
    printf("area_copy: %p count = %d\n", area, area->count);
#endif

    return (area);
}

static void area_unref(Area *area) {
    g_assert(area->count > 0);

    area->count -= 1;

#ifdef DEBUG
    printf("area_unref: %p count = %d\n", area, area->count);
#endif

    if (area->count == 0 && area->free_fn) {
        (void)area->free_fn(area->data, NULL);
        area->free_fn = NULL;
        im_free(area);

#ifdef DEBUG
        area_number -= 1;
        printf("area_unref: free .. total = %d\n", area_number);
#endif
    }
}

static void transform_area_g_string(const GValue *src_value, GValue *dest_value) {
    Area *area;
    char buf[256];

    area = g_value_get_boxed(src_value);
    im_snprintf(buf, 256, "IM_TYPE_AREA, count = %d, data = %p", area->count, area->data);
    g_value_set_string(dest_value, buf);
}

GType im_area_get_type(void) {
    static GType type = 0;

    if (!type) {
        type = g_boxed_type_register_static("im_area", (GBoxedCopyFunc)area_copy, (GBoxedFreeFunc)area_unref);
        g_value_register_transform_func(type, G_TYPE_STRING, transform_area_g_string);
    }

    return (type);
}

static int value_set_area(im_callback_fn free_fn, void *data, GValue *value) {
    Area *area;

    if (!(area = area_new(free_fn, data))) {
        return (-1);
    }

    g_value_init(value, IM_TYPE_AREA);
    g_value_set_boxed(value, area);
    area_unref(area);

    return (0);
}

static void *value_get_area_data(const GValue *value) {
    Area *area;

    area = g_value_get_boxed(value);

    return (area->data);
}

static size_t value_get_area_length(const GValue *value) {
    Area *area;

    area = g_value_get_boxed(value);

    return (area->length);
}

int im_meta_set_area(IMAGE *im, const char *field, im_callback_fn free_fn, void *data) {
    GValue value = {0};

    value_set_area(free_fn, data, &value);

    return (meta_set_value(im, field, &value));
}

int im_meta_get_area(IMAGE *im, const char *field, void **data) {
    GValue value_copy = {0};

    if (meta_get_value(im, field, IM_TYPE_AREA, &value_copy)) {
        return (-1);
    }
    *data = value_get_area_data(&value_copy);
    g_value_unset(&value_copy);

    return (0);
}

const char *im_ref_string_get(const GValue *value) {
    return (value_get_area_data(value));
}

size_t im_ref_string_get_length(const GValue *value) {
    return (value_get_area_length(value));
}

int im_ref_string_set(GValue *value, const char *str) {
    Area *area;
    char *str_copy;

    g_assert(G_VALUE_TYPE(value) == IM_TYPE_REF_STRING);

    if (!(str_copy = im_strdup(NULL, str))) {
        return (-1);
    }
    if (!(area = area_new((im_callback_fn)im_free, str_copy))) {
        im_free(str_copy);
        return (-1);
    }

    area->length = strlen(str);

    g_value_set_boxed(value, area);
    area_unref(area);

    return (0);
}

static void transform_ref_string_g_string(const GValue *src_value, GValue *dest_value) {
    g_value_set_string(dest_value, im_ref_string_get(src_value));
}

static void transform_g_string_ref_string(const GValue *src_value, GValue *dest_value) {
    im_ref_string_set(dest_value, g_value_get_string(src_value));
}

static void transform_ref_string_save_string(const GValue *src_value, GValue *dest_value) {
    im_save_string_setf(dest_value, "%s", im_ref_string_get(src_value));
}

static void transform_save_string_ref_string(const GValue *src_value, GValue *dest_value) {
    im_ref_string_set(dest_value, im_save_string_get(src_value));
}

GType im_ref_string_get_type(void) {
    static GType type = 0;

    if (!type) {
        type = g_boxed_type_register_static("im_ref_string", (GBoxedCopyFunc)area_copy, (GBoxedFreeFunc)area_unref);
        g_value_register_transform_func(type, G_TYPE_STRING, transform_ref_string_g_string);
        g_value_register_transform_func(G_TYPE_STRING, type, transform_g_string_ref_string);
        g_value_register_transform_func(type, IM_TYPE_SAVE_STRING, transform_ref_string_save_string);
        g_value_register_transform_func(IM_TYPE_SAVE_STRING, type, transform_save_string_ref_string);
    }

    return (type);
}

int im_meta_set_string(IMAGE *im, const char *field, const char *str) {
    GValue value = {0};

    g_value_init(&value, IM_TYPE_REF_STRING);
    im_ref_string_set(&value, str);

    return (meta_set_value(im, field, &value));
}

int im_meta_get_string(IMAGE *im, const char *field, char **str) {
    GValue value_copy = {0};
    Area *area;

    if (meta_get_value(im, field, IM_TYPE_REF_STRING, &value_copy)) {
        return (-1);
    }
    area = g_value_get_boxed(&value_copy);
    *str = area->data;
    g_value_unset(&value_copy);

    return (0);
}

void *im_blob_get(const GValue *value, size_t *blob_length) {
    Area *area;

    area = g_value_get_boxed(value);
    if (blob_length) {
        *blob_length = area->length;
    }

    return (area->data);
}

static void transform_blob_g_string(const GValue *src_value, GValue *dest_value) {
    void *blob;
    size_t blob_length;
    char buf[256];

    blob = im_blob_get(src_value, &blob_length);
    im_snprintf(buf, 256, "IM_TYPE_BLOB, data = %p, length = %zd", blob, blob_length);
    g_value_set_string(dest_value, buf);
}

static void transform_blob_save_string(const GValue *src_value, GValue *dest_value) {
    void *blob;
    size_t blob_length;
    char *b64;

    blob = im_blob_get(src_value, &blob_length);
    if ((b64 = im__b64_encode(blob, blob_length))) {
        im_save_string_set(dest_value, b64);
        im_free(b64);
    }
}

static void transform_save_string_blob(const GValue *src_value, GValue *dest_value) {
    const char *b64;
    void *blob;
    size_t blob_length;

    b64 = im_save_string_get(src_value);
    if ((blob = im__b64_decode(b64, &blob_length))) {
        im_blob_set(dest_value, (im_callback_fn)im_free, blob, blob_length);
    }
}

GType im_blob_get_type(void) {
    static GType type = 0;

    if (!type) {
        type = g_boxed_type_register_static("im_blob", (GBoxedCopyFunc)area_copy, (GBoxedFreeFunc)area_unref);
        g_value_register_transform_func(type, G_TYPE_STRING, transform_blob_g_string);
        g_value_register_transform_func(type, IM_TYPE_SAVE_STRING, transform_blob_save_string);
        g_value_register_transform_func(IM_TYPE_SAVE_STRING, type, transform_save_string_blob);
    }

    return (type);
}

int im_blob_set(GValue *value, im_callback_fn free_fn, void *blob, size_t blob_length) {
    Area *area;

    g_assert(G_VALUE_TYPE(value) == IM_TYPE_BLOB);

    if (!(area = area_new_blob(free_fn, blob, blob_length))) {
        return (-1);
    }

    g_value_set_boxed(value, area);
    area_unref(area);

    return (0);
}

int im_meta_set_blob(IMAGE *im, const char *field, im_callback_fn free_fn, void *blob, size_t blob_length) {
    GValue value = {0};

    g_value_init(&value, IM_TYPE_BLOB);
    im_blob_set(&value, free_fn, blob, blob_length);

    return (meta_set_value(im, field, &value));
}

int im_meta_get_blob(IMAGE *im, const char *field, void **blob, size_t *blob_length) {
    GValue value_copy = {0};

    if (meta_get_value(im, field, IM_TYPE_BLOB, &value_copy)) {
        return (-1);
    }
    *blob = im_blob_get(&value_copy, blob_length);
    g_value_unset(&value_copy);

    return (0);
}

void im__meta_init_types(void) {
    (void)im_save_string_get_type();
    (void)im_area_get_type();
    (void)im_ref_string_get_type();
    (void)im_blob_get_type();

    g_value_register_transform_func(G_TYPE_INT, IM_TYPE_SAVE_STRING, transform_int_save_string);
    g_value_register_transform_func(IM_TYPE_SAVE_STRING, G_TYPE_INT, transform_save_string_int);
    g_value_register_transform_func(G_TYPE_DOUBLE, IM_TYPE_SAVE_STRING, transform_double_save_string);
    g_value_register_transform_func(IM_TYPE_SAVE_STRING, G_TYPE_DOUBLE, transform_save_string_double);
}
