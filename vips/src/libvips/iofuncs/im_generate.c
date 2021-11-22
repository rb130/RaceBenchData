

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void *im_start_one(IMAGE *out, void *client, void *dummy) {
    IMAGE *in = (IMAGE *)client;

    return (im_region_create(in));
}

int im_stop_one(void *seq, void *dummy1, void *dummy2) {
    REGION *reg = (REGION *)seq;

    im_region_free(reg);

    return (0);
}

int im_stop_many(void *seq, void *dummy1, void *dummy2) {
    REGION **ar = (REGION **)seq;

    if (ar) {
        int i;

        for (i = 0; ar[i]; i++) {
            im_region_free(ar[i]);
        }
        im_free((char *)ar);
    }

    return (0);
}

void *im_start_many(IMAGE *out, void *client, void *dummy) {
    IMAGE **in = (IMAGE **)client;

    int i, n;
    REGION **ar;

    for (n = 0; in[n]; n++) {
        ;
    }

    if (!(ar = IM_ARRAY(NULL, n + 1, REGION *))) {
        return (NULL);
    }

    for (i = 0; i < n; i++) {
        if (!(ar[i] = im_region_create(in[i]))) {
            im_stop_many(ar, NULL, NULL);
            return (NULL);
        }
    }
    ar[n] = NULL;

    return (ar);
}

IMAGE **im_allocate_input_array(IMAGE *out, ...) {
    va_list ap;
    IMAGE **ar;
    IMAGE *im;
    int i, n;

    va_start(ap, out);
    for (n = 0; (im = va_arg(ap, IMAGE *)); n++) {
        ;
    }
    va_end(ap);

    if (!(ar = IM_ARRAY(out, n + 1, IMAGE *))) {
        return (NULL);
    }

    va_start(ap, out);
    for (i = 0; i < n; i++) {
        ar[i] = va_arg(ap, IMAGE *);
    }
    va_end(ap);
    ar[n] = NULL;

    return (ar);
}

static int write_vips(REGION *region, Rect *area, void *a, void *b) {
    size_t nwritten, count;
    void *buf;

    count = region->bpl * area->height;
    buf = IM_REGION_ADDR(region, 0, area->top);
    do {
        nwritten = write(region->im->fd, buf, count);
        if (nwritten == (size_t)-1) {
            return (errno);
        }

        buf = (void *)((char *)buf + nwritten);
        count -= nwritten;
    } while (count > 0);

    return (0);
}

int im_generate(IMAGE *im, im_start_fn start, im_generate_fn generate, im_stop_fn stop, void *a, void *b) {
    int res;

    g_assert(!im_image_sanity(im));

    if (!im->hint_set) {
        im_error("im_generate", "%s", _("im_demand_hint() not set"));
        return (-1);
    }

    if (im->Xsize <= 0 || im->Ysize <= 0 || im->Bands <= 0) {
        im_error("im_generate", "%s", _("bad dimensions"));
        return (-1);
    }

    im->Bbits = im_bits_of_fmt(im->BandFmt);

    switch (im->dtype) {
    case IM_PARTIAL:

        if (im->generate || im->start || im->stop) {
            im_error("im_generate", "%s", _("func already attached"));
            return (-1);
        }

        im->start = start;
        im->generate = generate;
        im->stop = stop;
        im->client1 = a;
        im->client2 = b;

#ifdef DEBUG_IO
        printf("im_generate: attaching partial callbacks\n");
#endif

        break;

    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
    case IM_MMAPINRW:
    case IM_OPENOUT:

        if (im->generate || im->start || im->stop) {
            im_error("im_generate", "%s", _("func already attached"));
            return (-1);
        }

        if (im_setupout(im)) {
            return (-1);
        }

        im->start = start;
        im->generate = generate;
        im->stop = stop;
        im->client1 = a;
        im->client2 = b;

        if (im->dtype == IM_OPENOUT) {
            res = vips_sink_disc(im, (VipsRegionWrite)write_vips, NULL);
        } else {
            res = vips_sink_memory(im);
        }

        if (res) {
            return (-1);
        }

        break;

    default:

        im_error("im_generate", _("unable to output to a %s image"), im_dtype2char(im->dtype));
        return (-1);
    }

    if (im__trigger_callbacks(im->writtenfns)) {
        return (-1);
    }

    return (0);
}
