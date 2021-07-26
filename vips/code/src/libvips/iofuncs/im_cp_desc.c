
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_cp_desc_array(IMAGE *out, IMAGE *in[]) {
    int i;
    int ni;

    g_assert(in[0]);

    out->Xsize = in[0]->Xsize;
    out->Ysize = in[0]->Ysize;
    out->Bands = in[0]->Bands;
    out->Bbits = in[0]->Bbits;
    out->BandFmt = in[0]->BandFmt;
    out->Type = in[0]->Type;
    out->Coding = in[0]->Coding;
    out->Xres = in[0]->Xres;
    out->Yres = in[0]->Yres;
    out->Xoffset = 0;
    out->Yoffset = 0;

    for (ni = 0; in[ni]; ni++) {
        ;
    }

    im__meta_destroy(out);
    for (i = ni - 1; i >= 0; i--) {
        if (im__meta_cp(out, in[i])) {
            return (-1);
        }
    }

    for (i = 0; in[i]; i++) {
        out->history_list = im__gslist_gvalue_merge(out->history_list, in[i]->history_list);
    }

    return (0);
}

#define MAX_IMAGES (1000)

int im_cp_descv(IMAGE *out, IMAGE *in1, ...) {
    va_list ap;
    int i;
    IMAGE *in[MAX_IMAGES];

    in[0] = in1;
    va_start(ap, in1);
    for (i = 1; i < MAX_IMAGES && (in[i] = va_arg(ap, IMAGE *)); i++) {
        ;
    }
    va_end(ap);
    if (i == MAX_IMAGES) {
        im_error("im_cp_descv", "%s", _("too many images"));
        return (-1);
    }

    return (im_cp_desc_array(out, in));
}

int im_cp_desc(IMAGE *out, IMAGE *in) {
    return (im_cp_descv(out, in, NULL));
}
