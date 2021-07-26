
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/thread.h>
#include <vips/debug.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

IMAGE *im_init(const char *filename) {
    IMAGE *im;

    if (im_init_world("vips")) {
        im_error_clear();
    }

    if (!(im = IM_NEW(NULL, IMAGE))) {
        return (NULL);
    }

#ifdef DEBUG_NEW
    printf("im_init: new IMAGE 0x%p, \"%s\"\n", im, filename);
#endif

    im->Xsize = -1;
    im->Ysize = -1;
    im->Bands = -1;
    im->Bbits = -1;
    im->BandFmt = -1;
    im->Coding = -1;
    im->Type = -1;
    im->Xres = 1.0;
    im->Yres = 1.0;
    im->Length = 0;
    im->Compression = 0;
    im->Level = 0;
    im->Xoffset = 0;
    im->Yoffset = 0;

    im->Hist = NULL;

    im->data = NULL;
    im->time = NULL;
    im->kill = 0;

    im->dtype = IM_NONE;
    im->fd = -1;
    im->baseaddr = NULL;
    im->length = 0;
    im->closefns = NULL;
    im->evalfns = NULL;
    im->evalendfns = NULL;
    im->closing = 0;
    im->close_pending = 0;

    im->magic = im_amiMSBfirst() ? IM_MAGIC_SPARC : IM_MAGIC_INTEL;

    im->start = NULL;
    im->generate = NULL;
    im->stop = NULL;
    im->client1 = NULL;
    im->client2 = NULL;
    im->sslock = g_mutex_new();
    im->regions = NULL;
    im->dhint = IM_SMALLTILE;

    im->Meta = NULL;
    im->Meta_traverse = NULL;

    im->sizeof_header = IM_SIZEOF_HEADER;

    im->windows = NULL;

    im->upstream = NULL;
    im->downstream = NULL;
    im->serial = 0;

    im->history_list = NULL;

    im->progress = NULL;

    im->evalstartfns = NULL;
    im->preclosefns = NULL;
    im->invalidatefns = NULL;

    im->file_length = 0;

    im->hint_set = FALSE;

    im->postclosefns = NULL;
    im->writtenfns = NULL;

    if (!(im->filename = im_strdup(NULL, filename))) {
        im_close(im);
        return (NULL);
    }

    g_mutex_lock(im__global_lock);
    im__open_images = g_slist_prepend(im__open_images, im);
    g_mutex_unlock(im__global_lock);

    return (im);
}

void im_initdesc(IMAGE *image, int xsize, int ysize, int bands, int bandbits, VipsBandFmt bandfmt, VipsCoding coding, VipsType type, float xres, float yres, int xo, int yo) {
    image->Xsize = xsize;
    image->Ysize = ysize;
    image->Bands = bands;

    image->Bbits = im_bits_of_fmt(bandfmt);
    image->BandFmt = bandfmt;
    image->Coding = coding;
    image->Type = type;
    image->Xres = xres;
    image->Yres = yres;
    image->Xoffset = xo;
    image->Yoffset = yo;
}
