
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int fastcor_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    IMAGE *ref = (IMAGE *)b;
    Rect irect;
    Rect *r = & or->valid;

    int x, y, i, j;
    int lsk;

    irect.left = or->valid.left;
    irect.top = or->valid.top;
    irect.width = or->valid.width + ref->Xsize - 1;
    irect.height = or->valid.height + ref->Ysize - 1;

    if (im_prepare(ir, &irect)) {
        return (-1);
    }
    lsk = IM_REGION_LSKIP(ir);

    for (y = 0; y < r->height; y++) {
        unsigned int *q = (unsigned int *)IM_REGION_ADDR(or, r->left, r->top + y);

        for (x = 0; x < r->width; x++) {
            PEL *b = (PEL *)ref->data;
            PEL *a = (PEL *)IM_REGION_ADDR(ir, r->left + x, r->top + y);

            int sum;

            sum = 0;
            for (j = 0; j < ref->Ysize; j++) {
                for (i = 0; i < ref->Xsize; i++) {
                    int t = b[i] - a[i];

                    sum += t * t;
                }

                a += lsk;
                b += ref->Xsize;
            }

            q[x] = sum;
        }
    }

    return (0);
}

int im_fastcor_raw(IMAGE *in, IMAGE *ref, IMAGE *out) {

    if (im_piocheck(in, out) || im_incheck(ref)) {
        return (-1);
    }

    if (in->Xsize < ref->Xsize || in->Ysize < ref->Ysize) {
        im_error("im_fastcor", "%s", _("ref not smaller than or equal to in"));
        return (-1);
    }

    if (im_check_uncoded("im_fastcor", in) || im_check_mono("im_fastcor", in) || im_check_format("im_fastcor", in, IM_BANDFMT_UCHAR) || im_check_coding_same("im_fastcor", in, ref) || im_check_bands_same("im_fastcor", in, ref) || im_check_format_same("im_fastcor", in, ref)) {
        return (-1);
    }

    if (im_cp_descv(out, in, ref, NULL)) {
        return (-1);
    }
    out->BandFmt = IM_BANDFMT_UINT;
    out->Xsize = in->Xsize - ref->Xsize + 1;
    out->Ysize = in->Ysize - ref->Ysize + 1;

    if (im_demand_hint(out, IM_FATSTRIP, in, NULL) || im_generate(out, im_start_one, fastcor_gen, im_stop_one, in, ref)) {
        return (-1);
    }

    out->Xoffset = -ref->Xsize / 2;
    out->Yoffset = -ref->Ysize / 2;

    return (0);
}

int im_fastcor(IMAGE *in, IMAGE *ref, IMAGE *out) {
    IMAGE *t1 = im_open_local(out, "im_fastcor intermediate", "p");

    if (!t1 || im_embed(in, t1, 1, ref->Xsize / 2, ref->Ysize / 2, in->Xsize + ref->Xsize - 1, in->Ysize + ref->Ysize - 1) || im_fastcor_raw(t1, ref, out)) {
        return (-1);
    }

    out->Xoffset = 0;
    out->Yoffset = 0;

    return (0);
}
