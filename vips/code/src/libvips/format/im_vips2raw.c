
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include <glib.h>
#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    IMAGE *in;
    int fd;
} Write;

static void write_destroy(Write *write) {
    im_free(write);
}

static Write *write_new(IMAGE *in, int fd) {
    Write *write;

    if (!(write = IM_NEW(NULL, Write))) {
        return (NULL);
    }

    write->in = in;
    write->fd = fd;

    if (!write->fd) {
        write_destroy(write);
        return (NULL);
    }

    return (write);
}

static int write_block(REGION *region, Rect *area, void *a) {
    Write *write = (Write *)a;
    int i;

    for (i = 0; i < area->height; i++) {
        PEL *p = (PEL *)IM_REGION_ADDR(region, area->left, area->top + i);
        if (im__write(write->fd, p, IM_IMAGE_SIZEOF_PEL(write->in) * area->width)) {
            return (-1);
        }
    }

    return (0);
}

int im_vips2raw(IMAGE *in, int fd) {
    Write *write;

    if (im_pincheck(in) || !(write = write_new(in, fd))) {
        return (-1);
    }

    if (vips_sink_disc(in, write_block, write)) {
        write_destroy(write);
        return (-1);
    }

    write_destroy(write);
    return (0);
}
