
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im__time_destroy(IMAGE *im) {
    if (im->time) {
#ifdef DEBUG
        printf("im__time_destroy: %s\n", im->filename);
#endif

        g_timer_destroy(im->time->start);
        im_free(im->time);
        im->time = NULL;
    }

    return (0);
}

int time_add(IMAGE *im) {
    im_time_t *time;

    if (im__time_destroy(im) || !(time = IM_NEW(NULL, im_time_t))) {
        return (-1);
    }

#ifdef DEBUG
    printf("time_add: %s\n", im->filename);
#endif

    time->im = im;
    time->start = g_timer_new();
    time->run = 0;
    time->eta = 0;
    time->tpels = (gint64)im->Xsize * im->Ysize;
    time->npels = 0;
    time->percent = 0;
    im->time = time;

    return (0);
}

static int update_time(im_time_t *time, int w, int h) {
    float prop;

    time->run = g_timer_elapsed(time->start, NULL);
    time->npels += w * h;
    prop = (float)time->npels / (float)time->tpels;
    time->percent = 100 * prop;
    if (prop > 0) {
        time->eta = (1.0 / prop) * time->run - time->run;
    }

    return (0);
}

int im__start_eval(IMAGE *im) {
    g_assert(!im_image_sanity(im));

    if (im->progress) {
#ifdef DEBUG
        printf("im__start_eval: %s\n", im->filename);
#endif

        g_assert(!im_image_sanity(im->progress));

        if (time_add(im->progress)) {
            return (-1);
        }

        if (im__trigger_callbacks(im->progress->evalstartfns)) {
            return (-1);
        }
    }

    return (0);
}

int im__handle_eval(IMAGE *im, int w, int h) {
    if (im->progress) {

        if (im->progress->time && im->progress->evalfns) {
            if (update_time(im->progress->time, w, h)) {
                return (-1);
            }
        }

        if (im__trigger_callbacks(im->progress->evalfns)) {
            return (-1);
        }
    }

    return (0);
}

int im__end_eval(IMAGE *im) {
    g_assert(!im_image_sanity(im));

    if (im->progress) {
#ifdef DEBUG
        printf("im__end_eval: %s\n", im->filename);
#endif

        g_assert(!im_image_sanity(im->progress));

        if (im__trigger_callbacks(im->progress->evalendfns)) {
            return (-1);
        }

        im__time_destroy(im->progress);
    }

    return (0);
}
