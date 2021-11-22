

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_LINE (4096)

int im_histlin(IMAGE *im, const char *fmt, ...) {
    va_list args;
    char line[4096];
    time_t timebuf;

    va_start(args, fmt);
    (void)im_vsnprintf(line, 4096 - 40, fmt, args);
    va_end(args);
    strcat(line, " # ");

    time(&timebuf);
    strcat(line, ctime(&timebuf));
    line[strlen(line) - 1] = '\0';

#ifdef DEBUG
    printf("im_histlin: adding:\n\t%s\nto history on image %p\n", line, im);
#endif

    im->history_list = g_slist_append(im->history_list, im__gvalue_ref_string_new(line));

    return (0);
}

int im_updatehist(IMAGE *out, const char *name, int argc, char *argv[]) {
    int i;
    char txt[IM_MAX_LINE];
    VipsBuf buf;

    vips_buf_init_static(&buf, txt, IM_MAX_LINE);
    vips_buf_appends(&buf, name);

    for (i = 0; i < argc; i++) {
        vips_buf_appends(&buf, " ");
        vips_buf_appends(&buf, argv[i]);
    }

    if (im_histlin(out, "%s", vips_buf_all(&buf))) {
        return (-1);
    }

    return (0);
}

const char *im_history_get(IMAGE *im) {
    if (!im->Hist) {
        im->Hist = im__gslist_gvalue_get(im->history_list);
    }

    return (im->Hist ? im->Hist : "");
}
