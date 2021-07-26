
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <vips/vips.h>
#include <vips/debug.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__progress = 0;

typedef struct {
    int (*save_fn)();
    IMAGE *im;
    char *filename;
} SaveBlock;

static int invoke_sb(SaveBlock *sb) {
    if (sb->save_fn(sb->im, sb->filename)) {
        return (-1);
    }

    return (0);
}

static int attach_sb(IMAGE *out, int (*save_fn)(), const char *filename) {
    SaveBlock *sb = IM_NEW(out, SaveBlock);

    if (!sb) {
        return (-1);
    }
    sb->im = out;
    sb->save_fn = save_fn;
    sb->filename = im_strdup(out, filename);

    if (im_add_written_callback(out, (im_callback_fn)invoke_sb, (void *)sb, NULL)) {
        return (-1);
    }

    return (0);
}

typedef int (*OpenLazyFn)(const char *filename, IMAGE *im);

typedef struct _OpenLazy {
    char *filename;

    OpenLazyFn read_pixels;
    IMAGE *lazy_im;
} OpenLazy;

static void *open_lazy_start(IMAGE *out, void *a, void *dummy) {
    OpenLazy *lazy = (OpenLazy *)a;

    if (!lazy->lazy_im) {
        if (!(lazy->lazy_im = im_open_local(out, "read", "p")) || lazy->read_pixels(lazy->filename, lazy->lazy_im)) {
            IM_FREEF(im_close, lazy->lazy_im);
            return (NULL);
        }
    }

    return (im_region_create(lazy->lazy_im));
}

static int open_lazy_generate(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;

    Rect *r = & or->valid;

    if (im_prepare(ir, r)) {
        return (-1);
    }

    if (im_region_region(or, ir, r, r->left, r->top)) {
        return (-1);
    }

    return (0);
}

static int open_lazy(OpenLazyFn read_header, OpenLazyFn read_pixels, const char *filename, IMAGE *out) {
    OpenLazy *lazy = IM_NEW(out, OpenLazy);

    if (!lazy || !(lazy->filename = im_strdup(out, filename))) {
        return (-1);
    }
    lazy->read_pixels = read_pixels;
    lazy->lazy_im = NULL;

    if (read_header(filename, out) || im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (im_generate(out, open_lazy_start, open_lazy_generate, im_stop_one, lazy, NULL)) {
        return (-1);
    }

    return (0);
}

static IMAGE *open_sub(OpenLazyFn read_header, OpenLazyFn read_pixels, const char *filename) {
    IMAGE *im;

    if (!(im = im_open(filename, "p")) || open_lazy(read_header, read_pixels, filename, im)) {
        im_close(im);
        return (NULL);
    }

    return (im);
}

typedef struct {
    IMAGE *im;

    int last_percent;
} Progress;

int evalstart_cb(Progress *progress) {
    progress->last_percent = 0;

    return (0);
}

int eval_cb(Progress *progress) {
    IMAGE *im = progress->im;

    if (im->time->percent != progress->last_percent) {
        printf(_("%s %s: %d%% complete"), g_get_prgname(), im->filename, im->time->percent);
        printf("\r");
        fflush(stdout);

        progress->last_percent = im->time->percent;
    }

    return (0);
}

int evalend_cb(Progress *progress) {
    IMAGE *im = progress->im;

    printf(_("%s %s: done in %ds          \n"), g_get_prgname(), im->filename, im->time->run);

    return (0);
}

IMAGE *im_open(const char *filename, const char *mode) {
    IMAGE *im;
    VipsFormatClass *format;

    if (im_init_world("vips")) {
        im_error_clear();
    }

    if (!filename || !mode) {
        im_error("im_open", "%s", _("NULL filename or mode"));
        return (NULL);
    }

    switch (mode[0]) {
    case 'r':
        if ((format = vips_format_for_file(filename))) {
            if (strcmp(VIPS_OBJECT_CLASS(format)->nickname, "vips") == 0) {
                if (!(im = im_open_vips(filename))) {
                    return (NULL);
                }
            } else if (!(im = open_sub(format->header, format->load, filename))) {
                return (NULL);
            }
        } else {
            return (NULL);
        }
        break;

    case 'w':
        if ((format = vips_format_for_name(filename))) {
            if (strcmp(VIPS_OBJECT_CLASS(format)->nickname, "vips") == 0) {
                im = im_openout(filename);
            } else {
                if (!(im = im_open(filename, "p"))) {
                    return (NULL);
                }
                if (attach_sb(im, format->save, filename)) {
                    im_close(im);
                    return (NULL);
                }
            }
        } else {
            char suffix[FILENAME_MAX];

            im_filename_suffix(filename, suffix);
            im_error("im_open", _("unsupported filetype \"%s\""), suffix);

            return (NULL);
        }
        break;

    case 't':
        im = im_setbuf(filename);
        break;

    case 'p':
        im = im_partial(filename);
        break;

    default:
        im_error("im_open", _("bad mode \"%s\""), mode);
        return (NULL);
    }

    if (im__progress || g_getenv("IM_PROGRESS")) {
        Progress *progress = IM_NEW(im, Progress);

        progress->im = im;
        im_add_evalstart_callback(im, (im_callback_fn)evalstart_cb, progress, NULL);
        im_add_eval_callback(im, (im_callback_fn)eval_cb, progress, NULL);
        im_add_evalend_callback(im, (im_callback_fn)evalend_cb, progress, NULL);
    }

#ifdef DEBUG
    printf("im_open: success for %s (%p)\n", im->filename, im);
#endif

    return (im);
}

IMAGE *im_open_header(const char *file) {
    return (im_open(file, "r"));
}
