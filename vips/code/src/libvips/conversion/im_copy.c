
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int copy_gen(REGION * or, void *seq, void *a, void *b) {
    REGION *ir = (REGION *)seq;
    Rect *r = & or->valid;

    if (im_prepare(ir, r)) {
        return (-1);
    }

    if (im_region_region(or, ir, r, r->left, r->top)) {
        return (-1);
    }

    return (0);
}

static int im_copy_set_all(IMAGE *in, IMAGE *out, VipsType type, float xres, float yres, int xoffset, int yoffset, int bands, VipsBandFmt bandfmt, VipsCoding coding) {

    if (im_check_coding_known("im_copy", in) || im_piocheck(in, out)) {
        return (-1);
    }
    if (coding != IM_CODING_NONE && coding != IM_CODING_LABQ && coding != IM_CODING_RAD) {
        im_error("im_copy", "%s", _("coding must be NONE, LABQ or RAD"));
        return (-1);
    }
    if (bandfmt < 0 || bandfmt > IM_BANDFMT_DPCOMPLEX) {
        im_error("im_copy", _("bandfmt must be in range [0,%d]"), IM_BANDFMT_DPCOMPLEX);
        return (-1);
    }

    if (im_cp_desc(out, in)) {
        return (-1);
    }
    out->Type = type;
    out->Xres = xres;
    out->Yres = yres;
    out->Xoffset = xoffset;
    out->Yoffset = yoffset;
    out->Bands = bands;
    out->BandFmt = bandfmt;
    out->Coding = coding;

    if (IM_IMAGE_SIZEOF_PEL(in) != IM_IMAGE_SIZEOF_PEL(out)) {
        im_error("im_copy", "%s", _("sizeof( pixel ) has changed"));
        return (-1);
    }

    if (im_demand_hint(out, IM_THINSTRIP, in, NULL) || im_generate(out, im_start_one, copy_gen, im_stop_one, in, NULL)) {
        return (-1);
    }

    return (0);
}

int im_copy(IMAGE *in, IMAGE *out) {
    return (im_copy_set(in, out, in->Type, in->Xres, in->Yres, 0, 0));
}

int im_copy_set(IMAGE *in, IMAGE *out, VipsType type, float xres, float yres, int xoffset, int yoffset) {
    return (im_copy_set_all(in, out, type, xres, yres, 0, 0, in->Bands, in->BandFmt, in->Coding));
}

int im_copy_morph(IMAGE *in, IMAGE *out, int bands, VipsBandFmt bandfmt, VipsCoding coding) {
    return (im_copy_set_all(in, out, in->Type, in->Xres, in->Yres, 0, 0, bands, bandfmt, coding));
}

int im_copy_set_meta(IMAGE *in, IMAGE *out, const char *field, GValue *value) {
    if (im_copy(in, out) || im_meta_set(out, field, value)) {
        return (1);
    }

    return (0);
}

static void im_copy_swap2_gen(PEL *in, PEL *out, int width, IMAGE *im) {
    int x;
    int sz = IM_IMAGE_SIZEOF_PEL(im) * width;

    for (x = 0; x < sz; x += 2) {
        out[x] = in[x + 1];
        out[x + 1] = in[x];
    }
}

static void im_copy_swap4_gen(PEL *in, PEL *out, int width, IMAGE *im) {
    int x;
    int sz = IM_IMAGE_SIZEOF_PEL(im) * width;

    for (x = 0; x < sz; x += 4) {
        out[x] = in[x + 3];
        out[x + 1] = in[x + 2];
        out[x + 2] = in[x + 1];
        out[x + 3] = in[x];
    }
}

static void im_copy_swap8_gen(PEL *in, PEL *out, int width, IMAGE *im) {
    int x;
    int sz = IM_IMAGE_SIZEOF_PEL(im) * width;

    for (x = 0; x < sz; x += 8) {
        out[x] = in[x + 7];
        out[x + 1] = in[x + 6];
        out[x + 2] = in[x + 5];
        out[x + 3] = in[x + 4];
        out[x + 4] = in[x + 3];
        out[x + 5] = in[x + 2];
        out[x + 6] = in[x + 1];
        out[x + 7] = in[x];
    }
}

int im_copy_swap(IMAGE *in, IMAGE *out) {
    if (im_piocheck(in, out) || im_check_uncoded("im_copy_swap", in) || im_cp_desc(out, in)) {
        return (-1);
    }

    switch (in->BandFmt) {
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_UCHAR:
        if (im_copy(in, out)) {
            return (-1);
        }
        break;

    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_USHORT:
        if (im_wrapone(in, out, (im_wrapone_fn)im_copy_swap2_gen, in, NULL)) {
            return (-1);
        }
        break;

    case IM_BANDFMT_INT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_COMPLEX:
        if (im_wrapone(in, out, (im_wrapone_fn)im_copy_swap4_gen, in, NULL)) {
            return (-1);
        }
        break;

    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_DPCOMPLEX:
        if (im_wrapone(in, out, (im_wrapone_fn)im_copy_swap8_gen, in, NULL)) {
            return (-1);
        }
        break;

    default:
        im_error("im_copy_swap", "%s", _("unsupported image type"));
        return (-1);
    }

    return (0);
}

int im_copy_native(IMAGE *in, IMAGE *out, gboolean is_msb_first) {
    if (is_msb_first != im_amiMSBfirst()) {
        return (im_copy_swap(in, out));
    } else {
        return (im_copy(in, out));
    }
}

IMAGE *im__convert_saveable(IMAGE *in, im__saveable_t saveable, int format_table[10]) {
    IMAGE *out;

    if (!(out = im_open("convert-for-save", "p"))) {
        return (NULL);
    }

    if (in->Coding == IM_CODING_LABQ) {
        IMAGE *t = im_open_local(out, "conv:1", "p");
        static void *table = NULL;

        if (!table) {
            table = im_LabQ2disp_build_table(NULL, im_col_displays(7));
        }

        if (!t || im_LabQ2disp_table(in, t, table)) {
            im_close(out);
            return (NULL);
        }

        in = t;
    }

    if (in->Coding == IM_CODING_RAD) {
        IMAGE *t;

        if (!(t = im_open_local(out, "conv:1", "p")) || im_rad2float(in, t)) {
            im_close(out);
            return (NULL);
        }

        in = t;
    }

    if (in->Coding == IM_CODING_NONE) {
        if (in->Bands == 2 && saveable != IM__RGBA) {
            IMAGE *t = im_open_local(out, "conv:1", "p");

            if (!t || im_extract_band(in, t, 0)) {
                im_close(out);
                return (NULL);
            }

            in = t;
        } else if (in->Bands > 3 && saveable == IM__RGB) {
            IMAGE *t = im_open_local(out, "conv:1", "p");

            if (!t || im_extract_bands(in, t, 0, 3)) {
                im_close(out);
                return (NULL);
            }

            in = t;
        } else if (in->Bands > 4 && (saveable == IM__RGB_CMYK || saveable == IM__RGBA)) {
            IMAGE *t = im_open_local(out, "conv:1", "p");

            if (!t || im_extract_bands(in, t, 0, 4)) {
                im_close(out);
                return (NULL);
            }

            in = t;
        }
    }

    if (in->Bands == 3 && in->BandFmt == IM_BANDFMT_SHORT && in->Type == IM_TYPE_LABS) {
        IMAGE *t = im_open_local(out, "conv:1", "p");

        if (!t || im_LabS2LabQ(in, t)) {
            im_close(out);
            return (NULL);
        }

        in = t;
    }

    if (in->Coding == IM_CODING_LABQ) {
        IMAGE *t = im_open_local(out, "conv:1", "p");

        if (!t || im_LabQ2Lab(in, t)) {
            im_close(out);
            return (NULL);
        }

        in = t;
    }

    if (in->Coding != IM_CODING_NONE) {
        im_close(out);
        return (NULL);
    }

    if (in->Bands == 3 && in->Type == IM_TYPE_LCH) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "conv-1", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT) || im_LCh2Lab(t[0], t[1])) {
            im_close(out);
            return (NULL);
        }

        in = t[1];
    }

    if (in->Bands == 3 && in->Type == IM_TYPE_YXY) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "conv-1", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT) || im_Yxy2XYZ(t[0], t[1])) {
            im_close(out);
            return (NULL);
        }

        in = t[1];
    }

    if (in->Bands == 3 && in->Type == IM_TYPE_UCS) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "conv-1", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT) || im_UCS2XYZ(t[0], t[1])) {
            im_close(out);
            return (NULL);
        }

        in = t[1];
    }

    if (in->Bands == 3 && in->Type == IM_TYPE_LAB) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "conv-1", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT) || im_Lab2XYZ(t[0], t[1])) {
            im_close(out);
            return (NULL);
        }

        in = t[1];
    }

    if (in->Bands == 3 && in->Type == IM_TYPE_XYZ) {
        IMAGE *t[2];

        if (im_open_local_array(out, t, 2, "conv-1", "p") || im_clip2fmt(in, t[0], IM_BANDFMT_FLOAT) || im_XYZ2disp(t[0], t[1], im_col_displays(7))) {
            im_close(out);
            return (NULL);
        }

        in = t[1];
    }

    {
        IMAGE *t = im_open_local(out, "conv:1", "p");

        if (!t || im_clip2fmt(in, t, format_table[in->BandFmt])) {
            im_close(out);
            return (NULL);
        }

        in = t;
    }

    if (im_copy(in, out)) {
        im_close(out);
        return (NULL);
    }

    return (out);
}
