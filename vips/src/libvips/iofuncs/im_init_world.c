

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <string.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/internal.h>

#ifdef HAVE_LIBOIL
#include <liboil/liboil.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

GMutex *im__global_lock = NULL;

static char *im__argv0 = NULL;

const char *im_get_argv0(void) {
    return (im__argv0);
}

int im_init_world(const char *argv0) {
    static gboolean started = FALSE;
    static gboolean done = FALSE;
    char *prgname;
    const char *prefix;
    const char *libdir;
    char name[256];

    if (done) {

        return (0);
    }
    if (started) {

        return (0);
    }
    started = TRUE;

    IM_SETSTR(im__argv0, argv0);

    g_type_init();

#ifdef G_THREADS_ENABLED
    if (!g_thread_supported())
        g_thread_init(NULL);
#endif

    if (!im__global_lock) {
        im__global_lock = g_mutex_new();
    }

    prgname = g_path_get_basename(argv0);
    g_set_prgname(prgname);
    g_free(prgname);

    if (!(prefix = im_guess_prefix(argv0, "VIPSHOME")) || !(libdir = im_guess_libdir(argv0, "VIPSHOME"))) {
        return (-1);
    }

    im_snprintf(name, 256, "%s" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S "locale", prefix);
    bindtextdomain(GETTEXT_PACKAGE, name);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

    im__meta_init_types();
    im__format_init();
    vips__interpolate_init();

    if (im_load_plugins("%s/vips-%d.%d", libdir, IM_MAJOR_VERSION, IM_MINOR_VERSION)) {
        im_warn("im_init_world", "%s", im_error_buffer());
        im_error_clear();
    }

    if (im_load_plugins("%s", libdir)) {
        im_warn("im_init_world", "%s", im_error_buffer());
        im_error_clear();
    }

    im__buffer_init();

#ifdef HAVE_LIBOIL
    {
#ifdef DEBUG
        GTimer *timer = g_timer_new();
#endif

        oil_init();

#ifdef DEBUG

        printf("oil_init: %gs\n", g_timer_elapsed(timer, NULL));
        g_timer_destroy(timer);
#endif
    }
#endif

    done = TRUE;

    return (0);
}

const char *im__gettext(const char *msgid) {

    if (im_init_world("giant_banana")) {
        im_error_clear();
    }

    return (dgettext(GETTEXT_PACKAGE, msgid));
}

const char *im__ngettext(const char *msgid, const char *plural, unsigned long int n) {
    if (im_init_world("giant_banana")) {
        im_error_clear();
    }

    return (dngettext(GETTEXT_PACKAGE, msgid, plural, n));
}

static GOptionEntry option_entries[] = {{"vips-concurrency", 'c', 0, G_OPTION_ARG_INT, &im__concurrency, N_("evaluate with N concurrent threads"), "N"}, {"vips-tile-width", 'w', 0, G_OPTION_ARG_INT, &im__tile_width, N_("set tile width to N (DEBUG)"), "N"}, {"vips-tile-height", 'h', 0, G_OPTION_ARG_INT, &im__tile_height, N_("set tile height to N (DEBUG)"), "N"}, {"vips-thinstrip-height", 't', 0, G_OPTION_ARG_INT, &im__thinstrip_height, N_("set thinstrip height to N (DEBUG)"), "N"}, {"vips-fatstrip-height", 'f', 0, G_OPTION_ARG_INT, &im__fatstrip_height, N_("set fatstrip height to N (DEBUG)"), "N"}, {"vips-progress", 'p', 0, G_OPTION_ARG_NONE, &im__progress, N_("show progress feedback"), NULL}, {NULL}};

GOptionGroup *im_get_option_group(void) {
    static GOptionGroup *option_group = NULL;

    if (!option_group) {
        option_group = g_option_group_new("vips", _("VIPS Options"), _("Show VIPS options"), NULL, NULL);
        g_option_group_add_entries(option_group, option_entries);
    }

    return (option_group);
}
