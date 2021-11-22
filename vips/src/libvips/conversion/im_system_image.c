

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_STRSIZE (4096)

static int system_image(IMAGE *im, IMAGE *in_image, char *out_name, const char *cmd_format, char **log) {
    const char *in_name = in_image->filename;
    FILE *fp;
    char line[IM_MAX_STRSIZE];
    char txt[IM_MAX_STRSIZE];
    VipsBuf buf = VIPS_BUF_STATIC(txt);
    int result;

    if (im_copy(im, in_image) || !(fp = im_popenf(cmd_format, "r", in_name, out_name))) {
        return (-1);
    }

    while (fgets(line, IM_MAX_STRSIZE, fp)) {
        if (!vips_buf_appends(&buf, line)) {
            break;
        }
    }

    result = pclose(fp);

    if (log) {
        *log = im_strdup(NULL, vips_buf_all(&buf));
    }

    return (result);
}

IMAGE *im_system_image(IMAGE *im, const char *in_format, const char *out_format, const char *cmd_format, char **log) {
    IMAGE *in_image;
    char *out_name;
    IMAGE *out;

    if (log) {
        *log = NULL;
    }

    if (!(in_image = im__open_temp(in_format))) {
        return (NULL);
    }
    if (!(out_name = im__temp_name(out_format))) {
        im_close(in_image);
        return (NULL);
    }

    if (system_image(im, in_image, out_name, cmd_format, log)) {
        im_close(in_image);
        g_free(out_name);

        return (NULL);
    }
    im_close(in_image);

    if (!(out = im_open(out_name, "r"))) {
        g_free(out_name);

        return (NULL);
    }
    if (im_add_postclose_callback(out, (im_callback_fn)unlink, out->filename, NULL)) {
        g_free(out_name);
        im_close(out);
        g_unlink(out_name);

        return (NULL);
    }
    g_free(out_name);

    return (out);
}
