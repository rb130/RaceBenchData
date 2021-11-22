

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#include <vips/vips.h>

#ifdef OS_WIN32
#define strcasecmp(a, b) _stricmp(a, b)
#endif

static char *main_option_list = NULL;
static char *main_option_usage = NULL;
static char *main_option_plugin = NULL;
static gboolean main_option_links;
static char *main_option_cpph = NULL;
static char *main_option_cppc = NULL;
static gboolean *main_option_version;

static GOptionEntry main_option[] = {{"list", 'l', 0, G_OPTION_ARG_STRING, &main_option_list,
                                         N_("list operations in PACKAGE "
                                            "(or \"all\", \"packages\", \"classes\")"),
                                         N_("PACKAGE")},
    {"usage", 'u', 0, G_OPTION_ARG_STRING, &main_option_usage, N_("show usage message for OPERATION"), N_("OPERATION")}, {"plugin", 'p', 0, G_OPTION_ARG_FILENAME, &main_option_plugin, N_("load PLUGIN"), N_("PLUGIN")}, {"links", 'k', 0, G_OPTION_ARG_NONE, &main_option_links, N_("print link lines for all operations"), NULL}, {"cpph", 'h', 0, G_OPTION_ARG_STRING, &main_option_cpph, N_("print C++ decls for PACKAGE (or \"all\")"), N_("PACKAGE")}, {"cppc", 'c', 0, G_OPTION_ARG_STRING, &main_option_cppc, N_("print C++ binding for PACKAGE (or \"all\")"), N_("PACKAGE")}, {"version", 'v', 0, G_OPTION_ARG_NONE, &main_option_version, N_("print im_version_string"), NULL}, {NULL}};

typedef void *(*map_name_fn)(im_function *);

static void *map_package(im_package *pack, map_name_fn fn) {
    int i;
    void *result;

    for (i = 0; i < pack->nfuncs; i++) {
        if ((result = fn(pack->table[i]))) {
            return (result);
        }
    }

    return (NULL);
}

static void *map_name(const char *name, map_name_fn fn) {
    im_package *pack;
    im_function *func;

    if (strcmp(name, "all") == 0) {

        im_map_packages((VSListMap2Fn)map_package, fn);
    } else if ((pack = im_find_package(name))) {

        map_package(pack, fn);
    } else if ((func = im_find_function(name))) {

        fn(func);
    } else {
        im_error("map_name", _("no package or function \"%s\""), name);
        return (fn);
    }

    return (NULL);
}

static void *list_package(im_package *pack) {
    printf("%-20s - %d operations\n", pack->name, pack->nfuncs);

    return (NULL);
}

static void *list_function(im_function *func) {
    printf("%-20s - %s\n", func->name, _(func->desc));

    return (NULL);
}

static void *list_class(VipsObjectClass *class) {
    vips_object_print_class(class);

    return (NULL);
}

static void print_list(const char *name) {
    if (strcmp(name, "packages") == 0) {
        im_map_packages((VSListMap2Fn)list_package, NULL);
    } else if (strcmp(name, "classes") == 0) {
        vips_class_map_concrete_all(g_type_from_name("VipsObject"), (VipsClassMap)list_class, NULL);
    } else {
        if (map_name(name, list_function)) {
            error_exit("unknown package \"%s\"", name);
        }
    }
}

static int isprefix(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (*s1 == '\0');
}

static int ispostfix(const char *s1, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if (l2 < l1) {
        return (0);
    }

    return (strcasecmp(s1, s2 + l2 - l1) == 0);
}

static void *print_links(im_package *pack) {
    int i;

    for (i = 0; i < pack->nfuncs; i++) {
        printf("rm -f %s" IM_EXEEXT "; "
               "ln -s vips" IM_EXEEXT " %s" IM_EXEEXT "\n",
            pack->table[i]->name, pack->table[i]->name);
    }

    return (NULL);
}

static int has_print(im_function *fn) {
    int i;

    for (i = 0; i < fn->argc; i++) {
        if (fn->argv[i].print) {
            return (-1);
        }
    }

    return (0);
}

static void usage(im_function *fn) {
    int i;
    im_package *pack = im_package_of_function(fn->name);

    fprintf(stderr, "usage: ");
    if (im_isprefix("vips", g_get_prgname())) {
        fprintf(stderr, "%s ", g_get_prgname());
    }
    fprintf(stderr, "%s ", fn->name);

    for (i = 0; i < fn->argc; i++) {
        if (fn->argv[i].desc->flags & IM_TYPE_ARG) {
            fprintf(stderr, "%s ", fn->argv[i].name);
        }
    }

    fprintf(stderr, "\nwhere:\n");
    for (i = 0; i < fn->argc; i++) {
        if (fn->argv[i].desc->flags & IM_TYPE_ARG) {
            fprintf(stderr, "\t%s is of type \"%s\"\n", fn->argv[i].name, fn->argv[i].desc->type);
        }
    }

    if (has_print(fn)) {
        fprintf(stderr, "prints:\n");
        for (i = 0; i < fn->argc; i++) {
            if (fn->argv[i].print) {
                fprintf(stderr, "\t%s of type \"%s\"\n", fn->argv[i].name, fn->argv[i].desc->type);
            }
        }
    }

    fprintf(stderr, "%s", _(fn->desc));
    if (pack) {
        fprintf(stderr, ", from package \"%s\"", pack->name);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "flags: ");
    if (fn->flags & IM_FN_PIO) {
        fprintf(stderr, "(PIO function) ");
    } else {
        fprintf(stderr, "(WIO function) ");
    }
    if (fn->flags & IM_FN_TRANSFORM) {
        fprintf(stderr, "(coordinate transformer) ");
    } else {
        fprintf(stderr, "(no coordinate transformation) ");
    }
    if (fn->flags & IM_FN_PTOP) {
        fprintf(stderr, "(point-to-point operation) ");
    } else {
        fprintf(stderr, "(area operation) ");
    }
    if (fn->flags & IM_FN_NOCACHE) {
        fprintf(stderr, "(nocache operation) ");
    } else {
        fprintf(stderr, "(result can be cached) ");
    }

    fprintf(stderr, "\n");
}

static char *vips2cpp(im_type_desc *ty) {
    int k;

    static char *vtypes[] = {IM_TYPE_DOUBLE, IM_TYPE_INT, IM_TYPE_COMPLEX, IM_TYPE_STRING, IM_TYPE_IMAGE, IM_TYPE_IMASK, IM_TYPE_DMASK, IM_TYPE_DISPLAY, IM_TYPE_IMAGEVEC, IM_TYPE_DOUBLEVEC, IM_TYPE_INTVEC};

    static char *ctypes[] = {"double", "int", "std::complex<double>", "char*", "VImage", "VIMask", "VDMask", "VDisplay", "std::vector<VImage>", "std::vector<double>", "std::vector<int>"};

    for (k = 0; k < IM_NUMBER(vtypes); k++) {
        if (strcmp(ty->type, vtypes[k]) == 0) {
            return (ctypes[k]);
        }
    }

    return (NULL);
}

static int is_cppable(im_function *fn) {
    int j;

    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (!vips2cpp(ty)) {
            return (0);
        }
    }

    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (ty->flags & IM_TYPE_OUTPUT) {
            if (strcmp(ty->type, IM_TYPE_IMAGEVEC) == 0 || strcmp(ty->type, IM_TYPE_DOUBLEVEC) == 0 || strcmp(ty->type, IM_TYPE_INTVEC) == 0) {
                return (0);
            }
        }
    }

    for (j = 0; j < fn->argc; j++) {
        if (strcmp(fn->argv[j].desc->type, IM_TYPE_IMAGE) == 0) {
            break;
        }
    }
    if (j == fn->argc) {
        return (0);
    }

    return (-1);
}

static void find_ioargs(im_function *fn, int *ia, int *oa) {
    int j;

    *oa = -1;
    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (ty->flags & IM_TYPE_OUTPUT) {
            *oa = j;
            break;
        }
    }

    *ia = -1;
    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (!(ty->flags & IM_TYPE_OUTPUT) && strcmp(ty->type, IM_TYPE_IMAGE) == 0) {
            *ia = j;
            break;
        }
    }
}

static gboolean drop_postfix(char *str, const char *postfix) {
    if (ispostfix(postfix, str)) {
        str[strlen(str) - strlen(postfix)] = '\0';

        return (TRUE);
    }

    return (FALSE);
}

static void c2cpp_name(const char *in, char *out) {
    static const char *dont_drop[] = {
        "_set",
    };
    static const char *drop[] = {"_vec", "const", "tra", "set", "_f"};

    int i;
    gboolean changed;

    if (isprefix("im_", in)) {
        strcpy(out, in + 3);
    } else {
        strcpy(out, in);
    }

    do {
        gboolean found;

        found = FALSE;
        for (i = 0; i < IM_NUMBER(dont_drop); i++) {
            if (ispostfix(dont_drop[i], out)) {
                found = TRUE;
                break;
            }
        }
        if (found) {
            break;
        }

        changed = FALSE;
        for (i = 0; i < IM_NUMBER(drop); i++) {
            changed |= drop_postfix(out, drop[i]);
        }
    } while (changed);
}

static void *print_cppproto(im_function *fn) {
    int j;
    char name[4096];
    int oa, ia;
    int flg;

    if (!is_cppable(fn)) {
        return (NULL);
    }

    c2cpp_name(fn->name, name);

    find_ioargs(fn, &ia, &oa);

    if (oa == -1) {
        printf("void ");
    } else {
        printf("%s ", vips2cpp(fn->argv[oa].desc));
    }

    printf("VImage::%s(", name);

    flg = 0;
    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (j == ia || j == oa) {
            continue;
        }

        if (flg) {
            printf(", %s", vips2cpp(ty));
        } else {
            printf(" %s", vips2cpp(ty));
            flg = 1;
        }

        if (ty->flags & IM_TYPE_OUTPUT) {
            printf("&");
        }

        printf(" %s", fn->argv[j].name);
    }

    if (flg) {
        printf(" ");
    }
    printf(") throw( VError )\n");

    return (NULL);
}

static void *print_cppdecl(im_function *fn) {
    int j;
    char name[4096];
    int oa, ia;
    int flg;

    if (!is_cppable(fn)) {
        return (NULL);
    }

    c2cpp_name(fn->name, name);

    find_ioargs(fn, &ia, &oa);
    if (ia == -1) {

        printf("static ");
    }

    if (oa == -1) {
        printf("void ");
    } else {
        printf("%s ", vips2cpp(fn->argv[oa].desc));
    }

    printf("%s(", name);

    flg = 0;
    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (j == ia || j == oa) {
            continue;
        }

        if (flg) {
            printf(", %s", vips2cpp(ty));
        } else {
            printf(" %s", vips2cpp(ty));
            flg = 1;
        }

        if (ty->flags & IM_TYPE_OUTPUT) {
            printf("&");
        }
    }

    if (flg) {
        printf(" ");
    }

    printf(") throw( VError );\n");

    return (NULL);
}

static void print_invec(int j, const char *arg, const char *vips_name, const char *c_name, const char *extract) {
    printf("\t((%s*) _vec.data(%d))->n = %s.size();\n", vips_name, j, arg);
    printf("\t((%s*) _vec.data(%d))->vec = new %s[%s.size()];\n", vips_name, j, c_name, arg);
    printf("\tfor( unsigned int i = 0; i < %s.size(); i++ )\n", arg);
    printf("\t\t((%s*) _vec.data(%d))->vec[i] = %s[i]%s;\n", vips_name, j, arg, extract);
}

static void *print_cppdef(im_function *fn) {
    int j;
    int ia, oa;

    if (!is_cppable(fn)) {
        return (NULL);
    }

    find_ioargs(fn, &ia, &oa);

    printf("// %s: %s\n", fn->name, _(fn->desc));
    print_cppproto(fn);
    printf("{\n");

    if (ia != -1) {
        printf("\tVImage %s = *this;\n", fn->argv[ia].name);
    }

    if (oa != -1) {
        printf("\t%s %s;\n\n", vips2cpp(fn->argv[oa].desc), fn->argv[oa].name);
    }

    printf("\tVargv _vec( \"%s\" );\n\n", fn->name);

    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (strcmp(ty->type, IM_TYPE_IMAGE) == 0) {
            printf("\t_vec.data(%d) = %s.image();\n", j, fn->argv[j].name);
            continue;
        }

        if ((ty->flags & IM_TYPE_OUTPUT) && (strcmp(ty->type, IM_TYPE_IMASK) == 0 || strcmp(ty->type, IM_TYPE_DMASK) == 0)) {
            printf("\t((im_mask_object*) _vec.data(%d))->name = "
                   "(char*)\"noname\";\n",
                j);
            continue;
        }

        if (ty->flags & IM_TYPE_OUTPUT) {
            continue;
        }

        if (strcmp(ty->type, IM_TYPE_IMASK) == 0) {

            printf("\t((im_mask_object*) "
                   "_vec.data(%d))->mask = %s.mask().iptr;\n",
                j, fn->argv[j].name);
        } else if (strcmp(ty->type, IM_TYPE_DMASK) == 0) {
            printf("\t((im_mask_object*) "
                   "_vec.data(%d))->mask = %s.mask().dptr;\n",
                j, fn->argv[j].name);
        } else if (strcmp(ty->type, IM_TYPE_DISPLAY) == 0) {

            printf("\t_vec.data(%d) = %s.disp();\n", j, fn->argv[j].name);
        } else if (strcmp(ty->type, IM_TYPE_STRING) == 0) {

            printf("\t_vec.data(%d) = (im_object) %s;\n", j, fn->argv[j].name);
        } else if (strcmp(ty->type, IM_TYPE_IMAGEVEC) == 0) {
            print_invec(j, fn->argv[j].name, "im_imagevec_object", "IMAGE *", ".image()");
        } else if (strcmp(ty->type, IM_TYPE_DOUBLEVEC) == 0) {
            print_invec(j, fn->argv[j].name, "im_doublevec_object", "double", "");
        } else if (strcmp(ty->type, IM_TYPE_INTVEC) == 0) {
            print_invec(j, fn->argv[j].name, "im_intvec_object", "int", "");
        } else {

            printf("\t*((%s*) _vec.data(%d)) = %s;\n", vips2cpp(ty), j, fn->argv[j].name);
        }
    }

    printf("\t_vec.call();\n");

    for (j = 0; j < fn->argc; j++) {
        im_type_desc *ty = fn->argv[j].desc;

        if (!(ty->flags & IM_TYPE_OUTPUT)) {
            continue;
        }

        if (strcmp(ty->type, IM_TYPE_IMAGE) == 0) {
            continue;
        }

        if (strcmp(ty->type, IM_TYPE_IMASK) == 0 || strcmp(ty->type, IM_TYPE_DMASK) == 0) {

            printf("\t%s.embed( (DOUBLEMASK *)((im_mask_object*)"
                   "_vec.data(%d))->mask );\n",
                fn->argv[j].name, j);
        } else if (strcmp(ty->type, IM_TYPE_STRING) == 0) {

            printf("\t%s = (char*) _vec.data(%d);\n", fn->argv[j].name, j);
        } else {

            printf("\t%s = *((%s*)_vec.data(%d));\n", fn->argv[j].name, vips2cpp(ty), j);
        }
    }

    if (oa != -1) {
        im_type_desc *ty = fn->argv[oa].desc;

        if (strcmp(ty->type, IM_TYPE_IMAGE) == 0 && (fn->flags & IM_FN_PIO)) {

            for (j = 0; j < fn->argc; j++) {
                im_type_desc *ty2 = fn->argv[j].desc;

                if (ty2->flags & IM_TYPE_OUTPUT) {
                    continue;
                }

                if (strcmp(ty2->type, IM_TYPE_IMAGE) == 0) {
                    printf("\t%s._ref->addref( "
                           "%s._ref );\n",
                        fn->argv[oa].name, fn->argv[j].name);
                } else if (strcmp(ty2->type, IM_TYPE_IMAGEVEC) == 0) {

                    printf("\tfor( unsigned int i = 0; "
                           "i < %s.size(); i++ )\n",
                        fn->argv[j].name);
                    printf("\t\t%s._ref->addref( "
                           "%s[i]._ref );\n",
                        fn->argv[oa].name, fn->argv[j].name);
                }
            }
        }
    }

    if (oa != -1) {
        printf("\n\treturn( %s );\n", fn->argv[oa].name);
    }

    printf("}\n\n");

    return (NULL);
}

static void print_cppdecls(char *name) {
    printf("// this file automatically generated from\n"
           "// VIPS library %s\n",
        im_version_string());

    if (map_name(name, print_cppdecl)) {
        error_exit("unknown package \"%s\"", name);
    }
}

static void print_cppdefs(char *name) {
    printf("// this file automatically generated from\n"
           "// VIPS library %s\n",
        im_version_string());

    if (map_name(name, print_cppdef)) {
        error_exit("unknown package \"%s\"", name);
    }
}

int main(int argc, char **argv) {
    GOptionContext *context;
    GError *error = NULL;
    im_function *fn;
    int i, j;

    if (im_init_world(argv[0])) {
        error_exit(NULL);
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

#ifdef DEBUG_FATAL

    g_log_set_always_fatal(G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

    fprintf(stderr, "*** DEBUG_FATAL: will abort() on first warning\n");
#endif

    context = g_option_context_new(_("- VIPS driver program"));

    g_option_context_add_main_entries(context, main_option, GETTEXT_PACKAGE);
    g_option_context_add_group(context, im_get_option_group());

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        if (error) {
            fprintf(stderr, "%s\n", error->message);
            g_error_free(error);
        }

        error_exit("try \"%s --help\"", g_get_prgname());
    }

    g_option_context_free(context);

    if (main_option_plugin) {
        if (!im_load_plugin(main_option_plugin)) {
            error_exit(NULL);
        }
    }
    if (main_option_cpph) {
        print_cppdecls(main_option_cpph);
    }
    if (main_option_cppc) {
        print_cppdefs(main_option_cppc);
    }
    if (main_option_links) {
        im_map_packages((VSListMap2Fn)print_links, NULL);
    }
    if (main_option_list) {
        print_list(main_option_list);
    }
    if (main_option_usage) {
        if (!(fn = im_find_function(main_option_usage))) {
            error_exit(NULL);
        }
        usage(fn);
    }
    if (main_option_version) {
        printf("vips-%s\n", im_version_string());
    }

    for (i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--") == 0) {
            for (j = i; j < argc; j++) {
                argv[j] = argv[j + 1];
            }

            argc -= 1;
        }
    }

    if (!im_isprefix("vips", g_get_prgname())) {
        char name[256];

        im_strncpy(name, g_get_prgname(), 256);
        if (ispostfix(".exe", name)) {
            name[strlen(name) - 4] = '\0';
        }

        if (!(fn = im_find_function(name))) {
            im_snprintf(name, 256, "im_%s", g_get_prgname());
            if (ispostfix(".exe", name)) {
                name[strlen(name) - 4] = '\0';
            }

            if (!(fn = im_find_function(name))) {
                error_exit(NULL);
            }
        }

        if (im_run_command(name, argc - 1, argv + 1)) {

            if (argc == 1) {
                usage(fn);
            } else {
                error_exit(NULL);
            }
        }
    } else if (argc > 1) {

        if (!(fn = im_find_function(argv[1]))) {
            error_exit(NULL);
        }

        if (im_run_command(argv[1], argc - 2, argv + 2)) {
            if (argc == 2) {
                usage(fn);
            } else {
                error_exit(NULL);
            }
        }
    }

    im_close_plugins();

    return (0);
}
