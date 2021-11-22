

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _Extract {
    IMAGE *in;
    IMAGE *out;
    int left;
    int top;
    int width;
    int height;
    int band;
    int nbands;
} Extract;

static int extract_band(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Extract *extract = (Extract *)b;
    Rect *r = & or->valid;
    int es = IM_IMAGE_SIZEOF_ELEMENT(ir->im);
    int ipel = IM_IMAGE_SIZEOF_PEL(ir->im);
    int opel = IM_IMAGE_SIZEOF_PEL(or->im);
    Rect iarea;
    char *p, *q;
    int x, y, z;

    iarea = or->valid;
    iarea.left += extract->left;
    iarea.top += extract->top;
    if (im_prepare(ir, &iarea)) {
        return (-1);
    }

    for (y = 0; y < r->height; y++) {
        p = IM_REGION_ADDR(ir, extract->left + r->left, extract->top + r->top + y) + extract->band * es;
        q = IM_REGION_ADDR(or, r->left, r->top + y);

        for (x = 0; x < r->width; x++) {
            for (z = 0; z < opel; z++) {
                q[z] = p[z];
            }

            p += ipel;
            q += opel;
        }
    }

    return (0);
}

static int extract_area(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Extract *extract = (Extract *)b;
    Rect iarea;

    iarea = or->valid;
    iarea.left += extract->left;
    iarea.top += extract->top;
    if (im_prepare(ir, &iarea)) {
        return (-1);
    }

    if (im_region_region(or, ir, & or->valid, iarea.left, iarea.top)) {
        return (-1);
    }

    return (0);
}

int im_extract_areabands(IMAGE *in, IMAGE *out, int left, int top, int width, int height, int band, int nbands) {
    Extract *extract;

    if (im_piocheck(in, out) || im_check_coding_known("im_extract_areabands", in)) {
        return (-1);
    }
    if (band < 0 || nbands < 1 || band + nbands > in->Bands) {
        im_error("im_extract_areabands", "%s", _("band selection out of range"));
        return (-1);
    }
    if (left + width > in->Xsize || top + height > in->Ysize || left < 0 || top < 0 || width <= 0 || height <= 0) {
        im_error("im_extract_areabands", "%s", _("bad extract area"));
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Bands = nbands;
    out->Xsize = width;
    out->Ysize = height;
    if (im_demand_hint(out, IM_THINSTRIP, in, NULL)) {
        return (-1);
    }
    if (!(extract = IM_NEW(out, Extract))) {
        return (-1);
    }
    extract->in = in;
    extract->out = out;
    extract->left = left;
    extract->top = top;
    extract->width = width;
    extract->height = height;
    extract->band = band;
    extract->nbands = nbands;

    if (band == 0 && nbands == in->Bands) {
        if (im_generate(out, im_start_one, extract_area, im_stop_one, in, extract)) {
            return (-1);
        }
    } else {
        if (im_generate(out, im_start_one, extract_band, im_stop_one, in, extract)) {
            return (-1);
        }
    }

    out->Xoffset = -left;
    out->Yoffset = -top;

    return (0);
}

int im_extract_area(IMAGE *in, IMAGE *out, int left, int top, int width, int height) {
    return (im_extract_areabands(in, out, left, top, width, height, 0, in->Bands));
}

int im_extract_bands(IMAGE *in, IMAGE *out, int band, int nbands) {
    return (im_extract_areabands(in, out, 0, 0, in->Xsize, in->Ysize, band, nbands));
}

int im_extract_band(IMAGE *in, IMAGE *out, int band) {
    return (im_extract_bands(in, out, band, 1));
}
