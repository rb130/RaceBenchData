
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int convert_ptob(IMAGE *im) {
    IMAGE *t1;

    if (!(t1 = im_open("im_incheck:1", "t"))) {
        return (-1);
    }
    if (im_copy(im, t1)) {
        im_close(t1);
        return (-1);
    }

    im->dtype = IM_SETBUF;
    im->data = t1->data;
    t1->data = NULL;

    if (im_close(t1)) {
        return (-1);
    }

    return (0);
}

static int convert_otom(IMAGE *im) {

    if (im_mapfile(im)) {
        return (-1);
    }
    im->data = im->baseaddr + im->sizeof_header;
    im->dtype = IM_MMAPIN;

    return (0);
}

int im_incheck(IMAGE *im) {
    g_assert(!im_image_sanity(im));

#ifdef DEBUG_IO
    printf("im_incheck: old-style input for %s\n", im->filename);
#endif

    switch (im->dtype) {
    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:

        if (!im->data) {
            im_error("im_incheck", "%s", _("no image data"));
            return (-1);
        }

        break;

    case IM_MMAPIN:
    case IM_MMAPINRW:

        break;

    case IM_PARTIAL:
#ifdef DEBUG_IO
        printf("im_incheck: converting partial image to WIO\n");
#endif

        if (convert_ptob(im)) {
            return (-1);
        }

        break;

    case IM_OPENIN:
#ifdef DEBUG_IO
        printf("im_incheck: converting openin image for old-style input\n");
#endif

        if (convert_otom(im)) {
            return (-1);
        }

        break;

    case IM_OPENOUT:

#ifdef DEBUG_IO
        printf("im_incheck: auto-rewind of %s\n", im->filename);
#endif
        if (im__close(im) || im_openin(im) || im_incheck(im)) {
            im_error("im_incheck", _("auto-rewind for %s failed"), im->filename);
            return (-1);
        }

        break;

    default:
        im_error("im_incheck", "%s", _("image not readable"));
        return (-1);
    }

    return (0);
}

int im_outcheck(IMAGE *im) {
#ifdef DEBUG_IO
    printf("im_outcheck: old-style output for %s\n", im->filename);
#endif

    switch (im->dtype) {
    case IM_PARTIAL:

        if (im->generate) {
            im_error("im_outcheck", "%s", _("image already written"));
            return (-1);
        }

        im->dtype = IM_SETBUF;

    case IM_SETBUF:

        if (im->data) {
            im_error("im_outcheck", "%s", _("image already written"));
            return (-1);
        }

        break;

    case IM_OPENOUT:
    case IM_SETBUF_FOREIGN:

        break;

    default:
        im_error("im_outcheck", "%s", _("image not writeable"));
        return (-1);
    }

    return (0);
}

int im_iocheck(IMAGE *in, IMAGE *out) {
    return (im_incheck(in) || im_outcheck(out));
}

int im_rwcheck(IMAGE *im) {

    if (im_incheck(im)) {
        im_error("im_rwcheck", "%s", _("unable to rewind file"));
        return (-1);
    }

    switch (im->dtype) {
    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
    case IM_MMAPINRW:

        break;

    case IM_MMAPIN:

        if (im_remapfilerw(im)) {
            return (-1);
        }

        break;

    default:
        im_error("im_rwcheck", "%s", _("bad file type"));
        return (-1);
    }

    return (0);
}

int im_pincheck(IMAGE *im) {
    g_assert(!im_image_sanity(im));

#ifdef DEBUG_IO
    printf("im_pincheck: enabling partial input for %s\n", im->filename);
#endif

    switch (im->dtype) {
    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:

        if (!im->data) {
            im_error("im_pincheck", "%s", _("no image data"));
            return (-1);
        }

        im->start = NULL;
        im->generate = NULL;
        im->stop = NULL;

        break;

    case IM_PARTIAL:

        if (!im->generate) {
            im_error("im_pincheck", "%s", _("no image data"));
            return (-1);
        }

        break;

    case IM_MMAPIN:
    case IM_MMAPINRW:
    case IM_OPENIN:
        break;

    case IM_OPENOUT:

#ifdef DEBUG_IO
        printf("im_pincheck: auto-rewind of %s\n", im->filename);
#endif
        if (im__close(im) || im_openin(im)) {
            im_error("im_pincheck", _("auto-rewind for %s failed"), im->filename);
            return (-1);
        }

        break;

    default:
        im_error("im_pincheck", "%s", _("image not readable"));
        return (-1);
    }

    return (0);
}

int im_poutcheck(IMAGE *im) {
    if (!im) {
        im_error("im_poutcheck", "%s", _("null image descriptor"));
        return (-1);
    }

#ifdef DEBUG_IO
    printf("im_pincheck: enabling partial output for %s\n", im->filename);
#endif

    switch (im->dtype) {
    case IM_SETBUF:

        if (im->data) {
            im_error("im_poutcheck", "%s", _("image already written"));
            return (-1);
        }

        break;

    case IM_PARTIAL:

        if (im->generate) {
            im_error("im_poutcheck", "%s", _("image already written"));
            return (-1);
        }

        break;

    case IM_OPENOUT:
    case IM_SETBUF_FOREIGN:

        break;

    default:
        im_error("im_poutcheck", "%s", _("image not writeable"));
        return (-1);
    }

    return (0);
}

int im_piocheck(IMAGE *in, IMAGE *out) {
    return (im_pincheck(in) || im_poutcheck(out));
}

int im_check_uncoded(const char *domain, IMAGE *im) {
    if (im->Coding != IM_CODING_NONE) {
        im_error(domain, "%s", _("image must be uncoded"));
        return (-1);
    }

    return (0);
}

int im_check_coding_known(const char *domain, IMAGE *im) {

    if (im->Coding != IM_CODING_NONE && im->Coding != IM_CODING_LABQ && im->Coding != IM_CODING_RAD) {
        im_error(domain, "%s", _("unknown image coding"));
        return (-1);
    }

    return (0);
}

int im_check_coding_rad(const char *domain, IMAGE *im) {
    if (im->Coding != IM_CODING_RAD || im->BandFmt != IM_BANDFMT_UCHAR || im->Bands != 4) {
        im_error(domain, "%s", _("Radiance coding only"));
        return (-1);
    }

    return (0);
}

int im_check_coding_labq(const char *domain, IMAGE *im) {
    if (im->Coding != IM_CODING_LABQ || im->BandFmt != IM_BANDFMT_UCHAR || im->Bands != 4) {
        im_error(domain, "%s", _("LABQ coding only"));
        return (-1);
    }

    return (0);
}

int im_check_mono(const char *domain, IMAGE *im) {
    if (im->Bands != 1) {
        im_error(domain, "%s", _("image must one band"));
        return (-1);
    }

    return (0);
}

int im_check_bands(const char *domain, IMAGE *im, int bands) {
    if (im->Bands != bands) {
        im_error(domain, _("image must %d bands"), bands);
        return (-1);
    }

    return (0);
}

int im_check_bands_1or3(const char *domain, IMAGE *im) {
    if (im->Bands != 1 && im->Bands != 3) {
        im_error(domain, "%s", _("image must one or three bands"));
        return (-1);
    }

    return (0);
}

int im_check_bands_1orn(const char *domain, IMAGE *im1, IMAGE *im2) {
    if (im1->Bands != im2->Bands && (im1->Bands != 1 && im2->Bands != 1)) {
        im_error(domain, "%s",
            _("images must have the same number of bands, "
              "or one must be single-band"));
        return (-1);
    }

    return (0);
}

int im_check_noncomplex(const char *domain, IMAGE *im) {
    if (vips_bandfmt_iscomplex(im->BandFmt)) {
        im_error(domain, "%s", _("image must be non-complex"));
        return (-1);
    }

    return (0);
}

int im_check_complex(const char *domain, IMAGE *im) {
    if (!vips_bandfmt_iscomplex(im->BandFmt)) {
        im_error(domain, "%s", _("image must be complex"));
        return (-1);
    }

    return (0);
}

int im_check_format(const char *domain, IMAGE *im, VipsBandFmt fmt) {
    if (im->BandFmt != fmt) {
        im_error(domain, _("image must be %s"), im_BandFmt2char(fmt));
        return (-1);
    }

    return (0);
}

int im_check_int(const char *domain, IMAGE *im) {
    if (!vips_bandfmt_isint(im->BandFmt)) {
        im_error(domain, "%s", _("image must be integer"));
        return (-1);
    }

    return (0);
}

int im_check_uint(const char *domain, IMAGE *im) {
    if (!vips_bandfmt_isuint(im->BandFmt)) {
        im_error(domain, "%s", _("image must be unsigned integer"));
        return (-1);
    }

    return (0);
}

int im_check_8or16(const char *domain, IMAGE *im) {
    if (im->BandFmt != IM_BANDFMT_UCHAR && im->BandFmt != IM_BANDFMT_USHORT && im->BandFmt != IM_BANDFMT_CHAR && im->BandFmt != IM_BANDFMT_SHORT) {
        im_error(domain, "%s",
            _("image must be 8- or 16-bit integer, "
              "signed or unsigned"));
        return (-1);
    }

    return (0);
}

int im_check_u8or16(const char *domain, IMAGE *im) {
    if (im->BandFmt != IM_BANDFMT_UCHAR && im->BandFmt != IM_BANDFMT_USHORT) {
        im_error(domain, "%s", _("image must be 8- or 16-bit unsigned integer"));
        return (-1);
    }

    return (0);
}

int im_check_u8or16orf(const char *domain, IMAGE *im) {
    if (im->BandFmt != IM_BANDFMT_UCHAR && im->BandFmt != IM_BANDFMT_USHORT && im->BandFmt != IM_BANDFMT_FLOAT) {
        im_error(domain, "%s",
            _("image must be 8- or 16-bit unsigned integer, "
              "or float"));
        return (-1);
    }

    return (0);
}

int im_check_uintorf(const char *domain, IMAGE *im) {
    if (im->BandFmt != IM_BANDFMT_UCHAR && im->BandFmt != IM_BANDFMT_USHORT && im->BandFmt != IM_BANDFMT_UINT && im->BandFmt != IM_BANDFMT_FLOAT) {
        im_error(domain, "%s", _("image must be unsigned int or float"));
        return (-1);
    }

    return (0);
}

int im_check_size_same(const char *domain, IMAGE *im1, IMAGE *im2) {
    if (im1->Xsize != im2->Xsize || im1->Ysize != im2->Ysize) {
        im_error(domain, "%s", _("images must match in size"));
        return (-1);
    }

    return (0);
}

int im_check_bands_same(const char *domain, IMAGE *im1, IMAGE *im2) {
    if (im1->Bands != im2->Bands) {
        im_error(domain, "%s", _("images must have the same number of bands"));
        return (-1);
    }

    return (0);
}

int im_check_bandno(const char *domain, IMAGE *im, int bandno) {
    if (bandno < -1 || bandno > im->Bands - 1) {
        im_error(domain, "bandno must be -1, or less than %d", im->Bands);
        return (-1);
    }

    return (0);
}

int im_check_format_same(const char *domain, IMAGE *im1, IMAGE *im2) {
    if (im1->BandFmt != im2->BandFmt) {
        im_error(domain, "%s", _("images must have the same band format"));
        return (-1);
    }

    return (0);
}

int im_check_coding_same(const char *domain, IMAGE *im1, IMAGE *im2) {
    if (im1->Coding != im2->Coding) {
        im_error(domain, "%s", _("images must have the same coding"));
        return (-1);
    }

    return (0);
}

int im_check_vector(const char *domain, int n, IMAGE *im) {
    if (n != 1 && im->Bands != 1 && n != im->Bands) {
        im_error(domain, _("vector must have 1 or %d elements"), im->Bands);
        return (-1);
    }

    return (0);
}

int im_check_hist(const char *domain, IMAGE *im) {
    if (im->Xsize != 1 && im->Ysize != 1) {
        im_error(domain, "%s", _("histograms must have width or height 1"));
        return (-1);
    }
    if (im->Xsize * im->Ysize > 65536) {
        im_error(domain, "%s",
            _("histograms must have not have more than "
              "65536 elements"));
        return (-1);
    }

    return (0);
}

int im_check_imask(const char *domain, INTMASK *mask) {
    if (!mask || mask->xsize > 1000 || mask->ysize > 1000 || mask->xsize <= 0 || mask->ysize <= 0 || !mask->coeff) {
        im_error("im_conv", "%s", _("nonsense mask parameters"));
        return (-1);
    }

    return (0);
}

int im_check_dmask(const char *domain, DOUBLEMASK *mask) {
    if (!mask || mask->xsize > 1000 || mask->ysize > 1000 || mask->xsize <= 0 || mask->ysize <= 0 || !mask->coeff) {
        im_error("im_conv", "%s", _("nonsense mask parameters"));
        return (-1);
    }

    return (0);
}

gboolean vips_bandfmt_isint(VipsBandFmt fmt) {
    switch (fmt) {
    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_INT:
        return (TRUE);

    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_COMPLEX:
    case IM_BANDFMT_DPCOMPLEX:
        return (FALSE);

    default:
        g_assert(0);
        return (-1);
    }
}

gboolean vips_bandfmt_isuint(VipsBandFmt fmt) {
    switch (fmt) {
    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_UINT:
        return (1);

    case IM_BANDFMT_INT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
    case IM_BANDFMT_COMPLEX:
    case IM_BANDFMT_DPCOMPLEX:
        return (0);

    default:
        g_assert(0);
        return (-1);
    }
}

gboolean vips_bandfmt_isfloat(VipsBandFmt fmt) {
    switch (fmt) {
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
        return (1);

    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_INT:
    case IM_BANDFMT_COMPLEX:
    case IM_BANDFMT_DPCOMPLEX:
        return (0);

    default:
        g_assert(0);
        return (-1);
    }
}

gboolean vips_bandfmt_iscomplex(VipsBandFmt fmt) {
    switch (fmt) {
    case IM_BANDFMT_COMPLEX:
    case IM_BANDFMT_DPCOMPLEX:
        return (1);

    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_INT:
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
        return (0);

    default:
        g_assert(0);
        return (-1);
    }
}

gboolean im_isMSBfirst(IMAGE *im) {
    if (im->magic == IM_MAGIC_SPARC) {
        return (1);
    } else {
        return (0);
    }
}

gboolean im_isfile(IMAGE *im) {
    switch (im->dtype) {
    case IM_MMAPIN:
    case IM_MMAPINRW:
    case IM_OPENOUT:
    case IM_OPENIN:
        return (1);

    case IM_PARTIAL:
    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
    case IM_NONE:
        return (0);

    default:
        error_exit("im_isfile: corrupt IMAGE descriptor");

        return (-1);
    }
}

gboolean im_ispartial(IMAGE *im) {
    switch (im->dtype) {
    case IM_PARTIAL:
        return (1);

    case IM_SETBUF:
    case IM_SETBUF_FOREIGN:
    case IM_MMAPIN:
    case IM_MMAPINRW:
    case IM_OPENIN:
    case IM_OPENOUT:
    case IM_NONE:
        return (0);

    default:
        error_exit("im_ispartial: corrupt IMAGE descriptor");

        return (-1);
    }
}
