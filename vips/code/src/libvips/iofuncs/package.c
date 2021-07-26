
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

extern im_package im__arithmetic;
extern im_package im__cimg;
extern im_package im__boolean;
extern im_package im__colour;
extern im_package im__conversion;
extern im_package im__convolution;
extern im_package im__deprecated;
extern im_package im__format;
extern im_package im__freq_filt;
extern im_package im__histograms_lut;
extern im_package im__inplace;
extern im_package im__mask;
extern im_package im__morphology;
extern im_package im__mosaicing;
extern im_package im__other;
extern im_package im__relational;
extern im_package im__resample;
extern im_package im__video;

static im_arg_desc guess_prefix_args[] = {IM_INPUT_STRING("argv0"), IM_INPUT_STRING("env_name"), IM_OUTPUT_STRING("PREFIX")};

static int guess_prefix_vec(im_object *argv) {
    const char *prefix = im_guess_prefix(argv[0], argv[1]);

    if (!prefix) {
        argv[2] = NULL;
        return (-1);
    }

    argv[2] = im_strdup(NULL, prefix);

    return (0);
}

static im_function guess_prefix_desc = {"im_guess_prefix", "guess install area", 0, guess_prefix_vec, IM_NUMBER(guess_prefix_args), guess_prefix_args};

static im_arg_desc guess_libdir_args[] = {IM_INPUT_STRING("argv0"), IM_INPUT_STRING("env_name"), IM_OUTPUT_STRING("LIBDIR")};

static int guess_libdir_vec(im_object *argv) {
    const char *libdir = im_guess_libdir(argv[0], argv[1]);

    if (!libdir) {
        argv[2] = NULL;
        return (-1);
    }

    argv[2] = im_strdup(NULL, libdir);

    return (0);
}

static im_function guess_libdir_desc = {"im_guess_libdir", "guess library area", 0, guess_libdir_vec, IM_NUMBER(guess_libdir_args), guess_libdir_args};

static im_arg_desc header_int_args[] = {IM_INPUT_STRING("field"), IM_INPUT_IMAGE("image"), IM_OUTPUT_INT("value")};

static int header_int_vec(im_object *argv) {
    return (im_header_int((IMAGE *)argv[1], (const char *)argv[0], (int *)argv[2]));
}

static im_function header_int_desc = {"im_header_int", "extract int fields from header", 0, header_int_vec, IM_NUMBER(header_int_args), header_int_args};

static im_arg_desc header_get_typeof_args[] = {IM_INPUT_STRING("field"), IM_INPUT_IMAGE("image"), IM_OUTPUT_INT("gtype")};

static int header_get_typeof_vec(im_object *argv) {
    int *out = (int *)argv[2];

    *out = im_header_get_typeof((IMAGE *)argv[1], (const char *)argv[0]);

    return (0);
}

static im_function header_get_typeof_desc = {"im_header_get_typeof", "return field type", 0, header_get_typeof_vec, IM_NUMBER(header_get_typeof_args), header_get_typeof_args};

static im_arg_desc header_double_args[] = {IM_INPUT_STRING("field"), IM_INPUT_IMAGE("image"), IM_OUTPUT_DOUBLE("value")};

static int header_double_vec(im_object *argv) {
    return (im_header_double((IMAGE *)argv[1], (const char *)argv[0], (double *)argv[2]));
}

static im_function header_double_desc = {"im_header_double", "extract double fields from header", 0, header_double_vec, IM_NUMBER(header_double_args), header_double_args};

static im_arg_desc header_string_args[] = {IM_INPUT_STRING("field"), IM_INPUT_IMAGE("image"), IM_OUTPUT_STRING("value")};

static int header_string_vec(im_object *argv) {
    char **out = (char **)&argv[2];

    if (im_header_as_string((IMAGE *)argv[1], (const char *)argv[0], out)) {
        return (-1);
    }

    return (0);
}

static im_function header_string_desc = {"im_header_string", "extract fields from headers as strings", 0, header_string_vec, IM_NUMBER(header_string_args), header_string_args};

static im_arg_desc history_get_args[] = {IM_INPUT_IMAGE("image"), IM_OUTPUT_STRING("history")};

static int history_get_vec(im_object *argv) {
    char **out = (char **)&argv[1];
    const char *str;

    if (!(str = im_history_get((IMAGE *)argv[0])) || !(*out = im_strdup(NULL, str))) {
        return (-1);
    }

    return (0);
}

static im_function history_get_desc = {"im_history_get", "return the image history as a string", 0, history_get_vec, IM_NUMBER(history_get_args), history_get_args};

static im_arg_desc getext_args[] = {IM_INPUT_IMAGE("image"), IM_OUTPUT_STRING("history")};

static int getext_vec(im_object *argv) {
    void **out = (void **)&argv[1];
    int size;

    if (!(*out = im__read_extension_block((IMAGE *)argv[0], &size))) {
        return (-1);
    }

    return (0);
}

static im_function getext_desc = {"im_getext", "return the image metadata XML as a string", 0, getext_vec, IM_NUMBER(getext_args), getext_args};

static im_arg_desc printdesc_args[] = {
    IM_INPUT_IMAGE("image"),
};

static int printdesc_vec(im_object *argv) {
    im_printdesc((IMAGE *)argv[0]);

    return (0);
}

static im_function printdesc_desc = {"im_printdesc", "print an image header to stdout", 0, printdesc_vec, IM_NUMBER(printdesc_args), printdesc_args};

static im_arg_desc version_string_args[] = {IM_OUTPUT_STRING("version")};

static int version_string_vec(im_object *argv) {
    if (!(argv[0] = im_strdup(NULL, im_version_string()))) {
        return (-1);
    }

    return (0);
}

static im_function version_string_desc = {"im_version_string", "VIPS version string", 0, version_string_vec, IM_NUMBER(version_string_args), version_string_args};

static im_arg_desc version_args[] = {IM_INPUT_INT("flag"), IM_OUTPUT_INT("version")};

static int version_vec(im_object *argv) {
    int flag = *((int *)argv[0]);
    int *out = ((int *)argv[1]);

    int version = im_version(flag);

    if (version < 0) {
        return (-1);
    }

    *out = version;

    return (0);
}

static im_function version_desc = {"im_version", "VIPS version number", 0, version_vec, IM_NUMBER(version_args), version_args};

static im_arg_desc cache_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("tile_width"), IM_INPUT_INT("tile_height"), IM_INPUT_INT("max_tiles")};

static int cache_vec(im_object *argv) {
    int tile_width = *((int *)argv[2]);
    int tile_height = *((int *)argv[3]);
    int max_tiles = *((int *)argv[4]);

    return (im_cache(argv[0], argv[1], tile_width, tile_height, max_tiles));
}

static im_function cache_desc = {"im_cache", "cache results of an operation", 0, cache_vec, IM_NUMBER(cache_args), cache_args};

static im_arg_desc binfile_args[] = {IM_INPUT_STRING("filename"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("width"), IM_INPUT_INT("height"), IM_INPUT_INT("bands"), IM_INPUT_INT("offset")};

static int binfile_vec(im_object *argv) {
    int width = *((int *)argv[2]);
    int height = *((int *)argv[3]);
    int bands = *((int *)argv[4]);
    int offset = *((int *)argv[5]);
    IMAGE *im;

    if (!(im = im_binfile(argv[0], width, height, bands, offset))) {
        return (-1);
    }

    if (im_copy(im, argv[1]) || im_add_close_callback(argv[1], (im_callback_fn)im_close, im, NULL)) {
        im_close(im);
        return (-1);
    }

    return (0);
}

static im_function binfile_desc = {"im_binfile", "open a headerless binary file", 0, binfile_vec, IM_NUMBER(binfile_args), binfile_args};

static im_function *iofuncs_list[] = {&binfile_desc, &cache_desc, &getext_desc, &guess_prefix_desc, &guess_libdir_desc, &header_get_typeof_desc, &header_int_desc, &header_double_desc, &header_string_desc, &history_get_desc, &printdesc_desc, &version_desc, &version_string_desc};

static im_package im__iofuncs = {"iofuncs", IM_NUMBER(iofuncs_list), iofuncs_list};

static im_package *built_in[] = {&im__arithmetic, &im__boolean,
#ifdef ENABLE_CXX
    &im__cimg,
#endif
    &im__colour, &im__conversion, &im__convolution, &im__deprecated, &im__format, &im__freq_filt, &im__histograms_lut, &im__inplace, &im__iofuncs, &im__mask, &im__morphology, &im__mosaicing, &im__other, &im__relational, &im__resample, &im__video};

typedef struct _Plugin {
    GModule *module;
    char *name;
    im_package *pack;
} Plugin;

static GSList *plugin_list = NULL;

static int plugin_free(Plugin *plug) {
    char *name = plug->name ? plug->name : "<unknown>";

    if (plug->module) {
        if (!g_module_close(plug->module)) {
            im_error("plugin", _("unable to close plugin \"%s\""), name);
            im_error("plugin", "%s", g_module_error());
            return (-1);
        }

        plug->module = NULL;
    }
    IM_FREE(plug->name);
    plug->pack = NULL;
    im_free(plug);

    plugin_list = g_slist_remove(plugin_list, plug);

    return (0);
}

im_package *im_load_plugin(const char *name) {
    Plugin *plug;

    if (!g_module_supported()) {
        im_error("plugin", "%s", _("plugins not supported on this platform"));
        return (NULL);
    }

    if (!(plug = IM_NEW(NULL, Plugin))) {
        return (NULL);
    }
    plug->module = NULL;
    plug->name = NULL;
    plug->pack = NULL;
    plugin_list = g_slist_prepend(plugin_list, plug);

    if (!(plug->name = im_strdup(NULL, name))) {
        plugin_free(plug);
        return (NULL);
    }

    if (!(plug->module = g_module_open(name, 0))) {
        im_error("plugin", _("unable to open plugin \"%s\""), name);
        im_error("plugin", "%s", g_module_error());
        plugin_free(plug);

        return (NULL);
    }

    if (!g_module_symbol(plug->module, "package_table", (gpointer *)((void *)&plug->pack))) {
        im_error("plugin",
            _("unable to find symbol \"package_table\" "
              "in plugin \"%s\""),
            name);
        im_error("plugin", "%s", g_module_error());
        plugin_free(plug);

        return (NULL);
    }

    if (!plug->pack->name || plug->pack->nfuncs < 0 || plug->pack->nfuncs > 10000) {
        im_error("plugin", _("corrupted package table in plugin \"%s\""), name);
        plugin_free(plug);

        return (NULL);
    }

#ifdef DEBUG
    printf("added package \"%s\" ...\n", plug->pack->name);
#endif

    return (plug->pack);
}

int im_load_plugins(const char *fmt, ...) {
    va_list ap;
    char dir_name[PATH_MAX];
    GDir *dir;
    const char *name;
    int result;

    if (!g_module_supported()) {
        return (0);
    }

    va_start(ap, fmt);
    (void)im_vsnprintf(dir_name, PATH_MAX - 1, fmt, ap);
    va_end(ap);

    if (!(dir = g_dir_open(dir_name, 0, NULL))) {

        return (0);
    }

    result = 0;
    while ((name = g_dir_read_name(dir))) {
        if (im_ispostfix(name, ".plg")) {
            char path[PATH_MAX];

            im_snprintf(path, PATH_MAX - 1, "%s" G_DIR_SEPARATOR_S "%s", dir_name, name);
            if (!im_load_plugin(path)) {
                result = -1;
            }
        }
    }
    g_dir_close(dir);

    return (result);
}

int im_close_plugins(void) {
    while (plugin_list) {
        if (plugin_free((Plugin *)plugin_list->data)) {
            return (-1);
        }
    }

    return (0);
}

static void *apply_plugin(Plugin *plug, VSListMap2Fn fn, void *a) {
    if (!plug->pack) {
        return (NULL);
    } else {
        return (fn(plug->pack, a, NULL));
    }
}

void *im_map_packages(VSListMap2Fn fn, void *a) {
    void *r = im_slist_map2(plugin_list, (VSListMap2Fn)apply_plugin, (void *)fn, a);
    int i;

    if (!r) {
        for (i = 0; i < IM_NUMBER(built_in); i++) {
            if ((r = fn(built_in[i], a, NULL))) {
                return (r);
            }
        }
    }

    return (r);
}

static im_function *search_package(im_package *pack, const char *name) {
    int i;

    for (i = 0; i < pack->nfuncs; i++) {
        if (strcmp(pack->table[i]->name, name) == 0) {
            return (pack->table[i]);
        }
    }

    return (NULL);
}

im_function *im_find_function(const char *name) {
    im_function *fn = im_map_packages((VSListMap2Fn)search_package, (void *)name);

    if (!fn) {
        im_error("im_find_function", _("\"%s\" not found"), name);
        return (NULL);
    }

    return (fn);
}

static im_package *package_name(im_package *pack, const char *name) {
    if (strcmp(pack->name, name) == 0) {
        return (pack);
    }

    return (NULL);
}

im_package *im_find_package(const char *name) {
    im_package *pack = im_map_packages((VSListMap2Fn)package_name, (void *)name);

    if (!pack) {
        im_error("im_find_package", _("\"%s\" not found"), name);
        return (NULL);
    }

    return (pack);
}

static im_package *package_function(im_package *pack, const char *name) {
    if (search_package(pack, name)) {
        return (pack);
    } else {
        return (NULL);
    }
}

im_package *im_package_of_function(const char *name) {
    im_package *pack = im_map_packages((VSListMap2Fn)package_function, (void *)name);

    if (!pack) {
        im_error("im_package_of_function", _("\"%s\" not found"), name);
        return (NULL);
    }

    return (pack);
}

int im_free_vargv(im_function *fn, im_object *vargv) {
    int i;
    int vargc = fn->argc;

    for (i = 0; i < vargc; i++) {
        if (vargv[i]) {

            if (fn->argv[i].desc->size != 0) {
                im_free(vargv[i]);
            }

            vargv[i] = NULL;
        }
    }

    return (0);
}

int im_allocate_vargv(im_function *fn, im_object *vargv) {
    int i;
    int vargc = fn->argc;

    for (i = 0; i < vargc; i++) {
        vargv[i] = NULL;
    }

    for (i = 0; i < vargc; i++) {
        int sz = fn->argv[i].desc->size;

        if (sz != 0) {
            if (!(vargv[i] = im_malloc(NULL, sz))) {

                (void)im_free_vargv(fn, vargv);
                return (-1);
            }
        }

        memset(vargv[i], 0, sz);
    }

    return (0);
}

static int destroy_args(im_function *fn, im_object *vargv) {
    int i;
    int vargc = fn->argc;

    for (i = 0; i < vargc; i++) {
        if (vargv[i]) {

            if (fn->argv[i].desc->dest && fn->argv[i].desc->dest(vargv[i])) {
                return (-1);
            }
        }
    }

    return (0);
}

static int build_args(im_function *fn, im_object *vargv, int argc, char **argv) {
    im_arg_desc *arg = fn->argv;
    int vargc = fn->argc;
    char *str;
    int i, j;

    for (i = 0, j = 0; i < vargc; i++) {

        im_type_desc *type = arg[i].desc;

        if (type->flags & IM_TYPE_ARG) {
            if (!argv[j]) {
                im_error("im_run_command", "%s", _("too few arguments"));
                return (-1);
            }
            str = argv[j++];

            if (type->init && type->init(&vargv[i], str)) {
                return (-1);
            }
        } else {

            if (type->init && type->init(&vargv[i], "no arg")) {
                return (-1);
            }
        }
    }

    if (argv[j]) {
        im_error("im_run_command", "%s", _("too many arguments"));
        return (-1);
    }

    return (0);
}

static int region_free(REGION *reg) {
    im_region_free(reg);

    return (0);
}

static int region_local_image(IMAGE *main, IMAGE *sub) {
    REGION *reg;

    if (!(reg = im_region_create(sub))) {
        return (-1);
    }
    if (im_add_close_callback(main, (im_callback_fn)region_free, reg, NULL)) {
        im_region_free(reg);

        return (-1);
    }

    return (0);
}

static int note_dependencies(im_function *fn, im_object *vargv, int i) {
    int j;

    for (j = 0; j < fn->argc; j++) {
        im_type_desc *type = fn->argv[j].desc;

        if (!(type->flags & IM_TYPE_OUTPUT)) {
            if (strcmp(type->type, IM_TYPE_IMAGE) == 0) {
                if (region_local_image(vargv[i], vargv[j])) {
                    return (-1);
                }
            } else if (strcmp(type->type, IM_TYPE_IMAGEVEC) == 0) {
                im_imagevec_object *iv = vargv[j];
                int k;

                for (k = 0; k < iv->n; k++) {
                    if (region_local_image(vargv[i], iv->vec[k])) {
                        return (-1);
                    }
                }
            }
        }
    }

    return (0);
}

static int print_args(im_function *fn, im_object *vargv) {
    int i;
    int vargc = fn->argc;

    for (i = 0; i < vargc; i++) {
        if (fn->argv[i].print && vargv[i]) {
            if (fn->argv[i].print(vargv[i])) {
                return (-1);
            }
        }
    }

    return (0);
}

static int add_hist(im_function *fn, im_object *vargv, int argc, char **argv) {
    int i;
    int vargc = fn->argc;

    for (i = 0; i < vargc; i++) {
        if (strcmp(fn->argv[i].desc->type, IM_TYPE_IMAGE) == 0 && (fn->argv[i].desc->flags & IM_TYPE_OUTPUT)) {
            if (im_updatehist(vargv[i], fn->name, argc, argv)) {
                return (-1);
            }
        }
    }

    return (0);
}

static int dispatch_function(im_function *fn, im_object *vargv, int argc, char **argv) {
    int i;

    if (build_args(fn, vargv, argc, argv)) {
        return (-1);
    }

    if (fn->flags & IM_FN_PIO) {
        for (i = 0; i < fn->argc; i++) {
            im_type_desc *type = fn->argv[i].desc;

            if (type->flags & IM_TYPE_OUTPUT && strcmp(type->type, IM_TYPE_IMAGE) == 0) {
                if (note_dependencies(fn, vargv, i)) {
                    return (-1);
                }
            }
        }
    }

    if (fn->disp(vargv)) {
        return (-1);
    }

    if (print_args(fn, vargv)) {
        return (-1);
    }

    if (add_hist(fn, vargv, argc, argv)) {
        return (-1);
    }

    return (0);
}

int im_run_command(char *name, int argc, char **argv) {
    static im_object object_array[IM_MAX_ARGS];
    im_object *vargv = object_array;
    im_function *fn;

    if (!(fn = im_find_function(name))) {
        return (-1);
    }

    if (im_allocate_vargv(fn, vargv)) {
        return (-1);
    }

    if (dispatch_function(fn, vargv, argc, argv)) {
        destroy_args(fn, vargv);
        im_free_vargv(fn, vargv);
        return (-1);
    }

    if (destroy_args(fn, vargv)) {
        return (-1);
    }
    im_free_vargv(fn, vargv);

    return (0);
}

const char *im_version_string(void) {
    return (IM_VERSION_STRING);
}

int im_version(int flag) {
    switch (flag) {
    case 0:
        return (IM_MAJOR_VERSION);

    case 1:
        return (IM_MINOR_VERSION);

    case 2:
        return (IM_MICRO_VERSION);

    default:
        im_error("im_version", "%s", _("flag not 0,1,2"));
        return (-1);
    }
}
