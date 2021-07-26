
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

static char *main_option_field = NULL;

static GOptionEntry main_option[] = {{"field", 'f', 0, G_OPTION_ARG_STRING, &main_option_field,
                                         N_("print value of FIELD (\"getext\" reads extension block, "
                                            "\"Hist\" reads image history)"),
                                         "FIELD"},
    {NULL}};

static void print_error(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n%s", im_error_buffer());
    im_error_clear();
}

static int print_header(IMAGE *im) {
    if (!main_option_field) {
        im_printdesc(im);
    } else if (strcmp(main_option_field, "getext") == 0) {
        if (im__has_extension_block(im)) {
            void *buf;
            int size;

            if (!(buf = im__read_extension_block(im, &size))) {
                return (-1);
            }
            printf("%s", (char *)buf);
            im_free(buf);
        }
    } else if (strcmp(main_option_field, "Hist") == 0) {
        printf("%s", im_history_get(im));
    } else {
        GValue value = {0};
        GType type;

        if (im_header_get(im, main_option_field, &value)) {
            return (-1);
        }

        type = G_VALUE_TYPE(&value);
        if (g_value_type_transformable(type, IM_TYPE_SAVE_STRING)) {
            GValue save_value = {0};

            g_value_init(&save_value, IM_TYPE_SAVE_STRING);
            if (!g_value_transform(&value, &save_value)) {
                return (-1);
            }
            printf("%s\n", im_save_string_get(&save_value));
            g_value_unset(&save_value);
        } else {
            char *str_value;

            str_value = g_strdup_value_contents(&value);
            printf("%s\n", str_value);
            g_free(str_value);
        }

        g_value_unset(&value);
    }

    return (0);
}

int main(int argc, char *argv[]) {
    GOptionContext *context;
    GError *error = NULL;
    int i;
    int result;

    if (im_init_world(argv[0])) {
        error_exit("unable to start VIPS");
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

    context = g_option_context_new(_("- print image header"));

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

    result = 0;

    for (i = 1; i < argc; i++) {
        IMAGE *im;

        if (!(im = im_open(argv[i], "r"))) {
            print_error("%s: unable to open", argv[i]);
            result = 1;
        }

        if (im && print_header(im)) {
            print_error("%s: unable to print header", argv[i]);
            result = 1;
        }

        if (im) {
            im_close(im);
        }
    }

    return (result);
}
