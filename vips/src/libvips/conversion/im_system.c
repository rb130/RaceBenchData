

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

int im_system(IMAGE *im, const char *cmd, char **out) {
    FILE *fp;

    if (!im_isfile(im)) {
        IMAGE *disc;

        if (!(disc = im__open_temp("%s.v"))) {
            return (-1);
        }
        if (im_copy(im, disc) || im_system(disc, cmd, out)) {
            im_close(disc);
            return (-1);
        }
        im_close(disc);
    } else if ((fp = im_popenf(cmd, "r", im->filename))) {
        char line[IM_MAX_STRSIZE];
        char txt[IM_MAX_STRSIZE];
        VipsBuf buf = VIPS_BUF_STATIC(txt);

        while (fgets(line, IM_MAX_STRSIZE, fp)) {
            if (!vips_buf_appends(&buf, line)) {
                break;
            }
        }
        pclose(fp);

        if (out) {
            *out = im_strdup(NULL, vips_buf_all(&buf));
        }
    }

    return (0);
}
