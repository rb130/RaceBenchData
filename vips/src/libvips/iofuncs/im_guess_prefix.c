

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#include <string.h>
#include <limits.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void im__change_suffix(const char *name, char *out, int mx, const char *new, const char **olds, int nolds) {
    char *p;
    int i;
    int len;

    im_strncpy(out, name, mx);

    while ((p = strrchr(out, '.'))) {

        for (i = 0; i < nolds; i++) {
            if (g_ascii_strcasecmp(p, olds[i]) == 0) {
                *p = '\0';
                break;
            }
        }

        if (*p) {
            break;
        }
    }

    len = strlen(out);
    im_strncpy(out + len, new, mx - len);
}

static char *get_current_dir(void) {
    static char buffer[PATH_MAX];
    char *dir;

#if defined(sun) && !defined(__SVR4)
    dir = getwd(buffer);
#else
    dir = getcwd(buffer, PATH_MAX);
#endif

    if (!dir) {
        buffer[0] = G_DIR_SEPARATOR;
        buffer[1] = '\0';
        dir = buffer;
    }

    return (dir);
}

static char *extract_prefix(const char *dir, const char *name) {
    char edir[PATH_MAX];
    char vname[PATH_MAX];
    int i;

#ifdef DEBUG
    printf("extract_prefix: trying for dir = \"%s\", name = \"%s\"\n", dir, name);
#endif

    if (!g_path_is_absolute(dir)) {
        im_snprintf(edir, PATH_MAX, "%s" G_DIR_SEPARATOR_S "%s", get_current_dir(), dir);
    } else {
        im_strncpy(edir, dir, PATH_MAX);
    }

    if (!im_ispostfix(edir, name)) {
        return (NULL);
    }
    im_strncpy(vname, edir, PATH_MAX);
    vname[strlen(edir) - strlen(name) - 1] = '\0';

    for (i = 0; i < (int)strlen(vname); i++) {
        if (im_isprefix(G_DIR_SEPARATOR_S "." G_DIR_SEPARATOR_S, vname + i)) {
            memcpy(vname + i, vname + i + 2, strlen(vname + i + 2) + 1);
        }
    }
    if (im_ispostfix(vname, G_DIR_SEPARATOR_S ".")) {
        vname[strlen(vname) - 2] = '\0';
    }
    if (im_ispostfix(vname, G_DIR_SEPARATOR_S)) {
        vname[strlen(vname) - 1] = '\0';
    }

#ifdef DEBUG
    printf("extract_prefix: canonicalised path = \"%s\"\n", vname);
#endif

    if (!im_ispostfix(vname, G_DIR_SEPARATOR_S "bin")) {
        return (NULL);
    }
    vname[strlen(vname) - strlen(G_DIR_SEPARATOR_S "bin")] = '\0';

#ifdef DEBUG
    printf("extract_prefix: found \"%s\"\n", vname);
#endif

    return (im_strdup(NULL, vname));
}

static char *scan_path(char *path, const char *name) {
    char *p, *q;
    char *prefix;

    for (p = path; (q = im_break_token(p, G_SEARCHPATH_SEPARATOR_S)); p = q) {
        char str[PATH_MAX];

        im_snprintf(str, PATH_MAX, "%s" G_DIR_SEPARATOR_S "%s", p, name);

#ifdef DEBUG
        printf("scan_path: looking in \"%s\" for \"%s\"\n", p, name);
#endif

        if (im_existsf("%s", str) && (prefix = extract_prefix(str, name))) {
            return (prefix);
        }
    }

    return (NULL);
}

static char *find_file(const char *name) {
    const char *path = g_getenv("PATH");
    char *prefix;
    char full_path[PATH_MAX];

    if (!path) {
        return (NULL);
    }

#ifdef DEBUG
    printf("im_guess_prefix: g_getenv( \"PATH\" ) == \"%s\"\n", path);
#endif

#ifdef OS_WIN32

    im_snprintf(full_path, PATH_MAX, "%s" G_SEARCHPATH_SEPARATOR_S "%s", get_current_dir(), path);
#else
    im_strncpy(full_path, path, PATH_MAX);
#endif

    if ((prefix = scan_path(full_path, name))) {
        return (prefix);
    }

    return (NULL);
}

static const char *guess_prefix(const char *argv0, const char *name) {
    char *prefix;

    if (argv0) {
        if (g_path_is_absolute(argv0)) {

            if ((prefix = extract_prefix(argv0, name))) {
#ifdef DEBUG
                printf("im_guess_prefix: found \"%s\" from "
                       "argv0\n",
                    prefix);
#endif
                return (prefix);
            }
        }

        if ((prefix = find_file(name))) {
#ifdef DEBUG
            printf("im_guess_prefix: found \"%s\" from "
                   "PATH\n",
                prefix);
#endif
            return (prefix);
        }
    }

#ifdef HAVE_REALPATH

    if (!g_path_is_absolute(argv0)) {
        char full_path[PATH_MAX];
        char resolved[PATH_MAX];

        im_snprintf(full_path, PATH_MAX, "%s" G_DIR_SEPARATOR_S "%s", get_current_dir(), argv0);

        if (realpath(full_path, resolved)) {
            if ((prefix = extract_prefix(resolved, name))) {

#ifdef DEBUG
                printf("im_guess_prefix: found \"%s\" "
                       "from cwd\n",
                    prefix);
#endif
                return (prefix);
            }
        }
    }
#endif

    return (IM_PREFIX);
}

const char *im_guess_prefix(const char *argv0, const char *env_name) {
    const char *prefix;
    const char *p;
    char name[PATH_MAX];

    if ((prefix = g_getenv(env_name))) {
#ifdef DEBUG
        printf("im_guess_prefix: found \"%s\" in environment\n", prefix);
#endif
        return (prefix);
    }

    p = im_skip_dir(argv0);

    if (strlen(IM_EXEEXT) > 0) {
        const char *olds[] = {IM_EXEEXT};

        im__change_suffix(p, name, PATH_MAX, IM_EXEEXT, olds, 1);
    } else {
        im_strncpy(name, p, PATH_MAX);
    }

#ifdef DEBUG
    printf("im_guess_prefix: argv0 = %s\n", argv0);
    printf("im_guess_prefix: name = %s\n", name);
    printf("im_guess_prefix: cwd = %s\n", get_current_dir());
#endif

    prefix = guess_prefix(argv0, name);
    g_setenv(env_name, prefix, TRUE);

    return (prefix);
}

const char *im_guess_libdir(const char *argv0, const char *env_name) {
    const char *prefix = im_guess_prefix(argv0, env_name);
    static char *libdir = NULL;

    if (libdir) {
        return (libdir);
    }

    if (strcmp(prefix, IM_PREFIX) == 0) {
        libdir = IM_LIBDIR;
    } else {
        libdir = g_strdup_printf("%s/lib", prefix);
    }

#ifdef DEBUG
    printf("im_guess_libdir: IM_PREFIX = %s\n", IM_PREFIX);
    printf("im_guess_libdir: IM_LIBDIR = %s\n", IM_LIBDIR);
    printf("im_guess_libdir: prefix = %s\n", prefix);
    printf("im_guess_libdir: libdir = %s\n", libdir);
#endif

    return (libdir);
}
