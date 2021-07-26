
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#ifndef HAVE_JPEG

#include <vips/vips.h>

int im_vips2jpeg(IMAGE *in, const char *filename) {
    im_error("im_vips2jpeg", "%s", _("JPEG support disabled"));

    return (-1);
}

int im_vips2bufjpeg(IMAGE *in, IMAGE *out, int qfac, char **obuf, int *olen) {
    im_error("im_vips2bufjpeg", "%s", _("JPEG support disabled"));

    return (-1);
}

int im_vips2mimejpeg(IMAGE *in, int qfac) {
    im_error("im_vips2mimejpeg", "%s", _("JPEG support disabled"));

    return (-1);
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>

#ifdef HAVE_EXIF
#ifdef UNTAGGED_EXIF
#include <exif-data.h>
#include <exif-loader.h>
#include <exif-ifd.h>
#include <exif-utils.h>
#else
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-utils.h>
#endif
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/buf.h>

#ifdef HAVE_STDLIB_H
#undef HAVE_STDLIB_H
#endif

#include <jpeglib.h>
#include <jerror.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {

    struct jpeg_error_mgr pub;

    jmp_buf jmp;
    FILE *fp;
} ErrorManager;

METHODDEF(void)
new_output_message(j_common_ptr cinfo) {
    char buffer[JMSG_LENGTH_MAX];

    (*cinfo->err->format_message)(cinfo, buffer);
    im_error("vips_jpeg", _("%s"), buffer);

#ifdef DEBUG
    printf("vips_jpeg.c: new_output_message: \"%s\"\n", buffer);
#endif
}

METHODDEF(void)
new_error_exit(j_common_ptr cinfo) {
    ErrorManager *eman = (ErrorManager *)cinfo->err;

#ifdef DEBUG
    printf("vips_jpeg.c: new_error_exit\n");
#endif

    if (eman->fp) {
        (void)fclose(eman->fp);
        eman->fp = NULL;
    }

    (*cinfo->err->output_message)(cinfo);

    longjmp(eman->jmp, 1);
}

typedef struct {
    IMAGE *in;
    struct jpeg_compress_struct cinfo;
    ErrorManager eman;
    JSAMPROW *row_pointer;
    char *profile_bytes;
    unsigned int profile_length;
    IMAGE *inverted;
} Write;

static void write_destroy(Write *write) {
    jpeg_destroy_compress(&write->cinfo);
    IM_FREEF(im_close, write->in);
    IM_FREEF(fclose, write->eman.fp);
    IM_FREE(write->row_pointer);
    IM_FREE(write->profile_bytes);
    IM_FREEF(im_close, write->inverted);
    im_free(write);
}

#define UC IM_BANDFMT_UCHAR

static int bandfmt_jpeg[10] = {

    UC, UC, UC, UC, UC, UC, UC, UC, UC, UC};

static Write *write_new(IMAGE *in) {
    Write *write;

    if (!(write = IM_NEW(NULL, Write)))
        return (NULL);
    memset(write, 0, sizeof(Write));

    if (!(write->in = im__convert_saveable(in, IM__RGB_CMYK, bandfmt_jpeg))) {
        im_error("im_vips2jpeg", "%s", _("unable to convert to saveable format"));
        write_destroy(write);
        return (NULL);
    }
    write->row_pointer = NULL;
    write->cinfo.err = jpeg_std_error(&write->eman.pub);
    write->eman.pub.error_exit = new_error_exit;
    write->eman.pub.output_message = new_output_message;
    write->eman.fp = NULL;
    write->profile_bytes = NULL;
    write->profile_length = 0;
    write->inverted = NULL;

    return (write);
}

#ifdef HAVE_EXIF
static void write_rational(ExifEntry *entry, ExifByteOrder bo, void *data) {
    ExifRational *v = (ExifRational *)data;

    exif_set_rational(entry->data, bo, *v);
}

static void write_short(ExifEntry *entry, ExifByteOrder bo, void *data) {
    ExifShort *v = (ExifShort *)data;

    exif_set_short(entry->data, bo, *v);
}

typedef void (*write_fn)(ExifEntry *, ExifByteOrder, void *);

static int write_tag(ExifData *ed, ExifTag tag, ExifFormat f, write_fn fn, void *data) {
    ExifByteOrder bo;
    int found;
    int i;

    bo = exif_data_get_byte_order(ed);

    found = 0;
    for (i = 0; i < EXIF_IFD_COUNT; i++) {
        ExifEntry *entry;

        if ((entry = exif_content_get_entry(ed->ifd[i], tag)) && entry->format == f && entry->components == 1) {
            fn(entry, bo, data);
            found = 1;
        }
    }

    if (!found) {

        ExifEntry *entry;

        entry = exif_entry_new();
        exif_content_add_entry(ed->ifd[0], entry);
        exif_entry_initialize(entry, tag);
        fn(entry, bo, data);
    }

    return (0);
}

static int set_exif_resolution(ExifData *ed, IMAGE *im) {
    double xres, yres;
    ExifRational xres_rational, yres_rational;
    ExifShort unit;

    xres = im->Xres * 25.4;
    yres = im->Yres * 25.4;
    unit = 2;

    xres_rational.numerator = xres * 100000;
    xres_rational.denominator = 100000;
    yres_rational.numerator = yres * 100000;
    yres_rational.denominator = 100000;

    if (write_tag(ed, EXIF_TAG_X_RESOLUTION, EXIF_FORMAT_RATIONAL, write_rational, &xres_rational) || write_tag(ed, EXIF_TAG_Y_RESOLUTION, EXIF_FORMAT_RATIONAL, write_rational, &yres_rational) || write_tag(ed, EXIF_TAG_RESOLUTION_UNIT, EXIF_FORMAT_SHORT, write_short, &unit)) {
        im_error("im_jpeg2vips", "%s", _("error setting JPEG resolution"));
        return (-1);
    }

    return (0);
}
#endif

static int write_exif(Write *write) {
    unsigned char *data;
    size_t data_length;
    unsigned int idl;
#ifdef HAVE_EXIF
    ExifData *ed;

    if (im_header_get_typeof(write->in, IM_META_EXIF_NAME)) {
        if (im_meta_get_blob(write->in, IM_META_EXIF_NAME, (void *)&data, &data_length))
            return (-1);

        if (!(ed = exif_data_new_from_data(data, data_length)))
            return (-1);
    } else
        ed = exif_data_new();

    if (set_exif_resolution(ed, write->in)) {
        exif_data_free(ed);
        return (-1);
    }

    exif_data_save_data(ed, &data, &idl);
    if (!idl) {
        im_error("im_jpeg2vips", "%s", _("error saving EXIF"));
        exif_data_free(ed);
        return (-1);
    }
    data_length = idl;

#ifdef DEBUG
    printf("im_vips2jpeg: attaching %d bytes of EXIF\n", data_length);
#endif

    exif_data_free(ed);
    jpeg_write_marker(&write->cinfo, JPEG_APP0 + 1, data, data_length);
    free(data);
#else

    if (im_header_get_typeof(write->in, IM_META_EXIF_NAME)) {
        if (im_meta_get_blob(write->in, IM_META_EXIF_NAME, (void *)&data, &data_length))
            return (-1);

#ifdef DEBUG
        printf("im_vips2jpeg: attaching %d bytes of EXIF\n", data_length);
#endif

        jpeg_write_marker(&write->cinfo, JPEG_APP0 + 1, data, data_length);
    }
#endif

    return (0);
}

#define ICC_MARKER (JPEG_APP0 + 2)
#define ICC_OVERHEAD_LEN 14
#define MAX_BYTES_IN_MARKER 65533
#define MAX_DATA_BYTES_IN_MARKER (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)

static void write_profile_data(j_compress_ptr cinfo, const JOCTET *icc_data_ptr, unsigned int icc_data_len) {
    unsigned int num_markers;
    int cur_marker = 1;
    unsigned int length;

    assert(icc_data_len > 0);

    num_markers = (icc_data_len + MAX_DATA_BYTES_IN_MARKER - 1) / MAX_DATA_BYTES_IN_MARKER;

    while (icc_data_len > 0) {

        length = icc_data_len;
        if (length > MAX_DATA_BYTES_IN_MARKER)
            length = MAX_DATA_BYTES_IN_MARKER;
        icc_data_len -= length;

        jpeg_write_m_header(cinfo, ICC_MARKER, (unsigned int)(length + ICC_OVERHEAD_LEN));

        jpeg_write_m_byte(cinfo, 0x49);
        jpeg_write_m_byte(cinfo, 0x43);
        jpeg_write_m_byte(cinfo, 0x43);
        jpeg_write_m_byte(cinfo, 0x5F);
        jpeg_write_m_byte(cinfo, 0x50);
        jpeg_write_m_byte(cinfo, 0x52);
        jpeg_write_m_byte(cinfo, 0x4F);
        jpeg_write_m_byte(cinfo, 0x46);
        jpeg_write_m_byte(cinfo, 0x49);
        jpeg_write_m_byte(cinfo, 0x4C);
        jpeg_write_m_byte(cinfo, 0x45);
        jpeg_write_m_byte(cinfo, 0x0);

        jpeg_write_m_byte(cinfo, cur_marker);
        jpeg_write_m_byte(cinfo, (int)num_markers);

        while (length--) {
            jpeg_write_m_byte(cinfo, *icc_data_ptr);
            icc_data_ptr++;
        }
        cur_marker++;
    }
}

static int write_profile_file(Write *write, const char *profile) {
    if (!(write->profile_bytes = im__file_read_name(profile, VIPS_ICC_DIR, &write->profile_length)))
        return (-1);
    write_profile_data(&write->cinfo, (JOCTET *)write->profile_bytes, write->profile_length);

#ifdef DEBUG
    printf("im_vips2jpeg: attached profile \"%s\"\n", profile);
#endif

    return (0);
}

static int write_profile_meta(Write *write) {
    void *data;
    size_t data_length;

    if (im_meta_get_blob(write->in, IM_META_ICC_NAME, &data, &data_length))
        return (-1);

    write_profile_data(&write->cinfo, data, data_length);

#ifdef DEBUG
    printf("im_vips2jpeg: attached %d byte profile from VIPS header\n", data_length);
#endif

    return (0);
}

static int write_jpeg_block(REGION *region, Rect *area, void *a) {
    Write *write = (Write *)a;
    int i;

    if (setjmp(write->eman.jmp))
        return (-1);

    for (i = 0; i < area->height; i++)
        write->row_pointer[i] = (JSAMPROW)IM_REGION_ADDR(region, 0, area->top + i);

    jpeg_write_scanlines(&write->cinfo, write->row_pointer, area->height);

    return (0);
}

static int write_vips(Write *write, int qfac, const char *profile) {
    IMAGE *in;
    J_COLOR_SPACE space;

    in = write->in;

    assert(in->BandFmt == IM_BANDFMT_UCHAR);
    assert(in->Coding == IM_CODING_NONE);
    assert(in->Bands == 1 || in->Bands == 3 || in->Bands == 4);

    if (im_pincheck(in))
        return (-1);
    if (qfac < 0 || qfac > 100) {
        im_error("im_vips2jpeg", "%s", _("qfac should be in 0-100"));
        return (-1);
    }

    write->cinfo.image_width = in->Xsize;
    write->cinfo.image_height = in->Ysize;
    write->cinfo.input_components = in->Bands;
    if (in->Bands == 4 && in->Type == IM_TYPE_CMYK) {
        space = JCS_CMYK;

        if (!(write->inverted = im_open("vips2jpeg_invert", "p")) || im_invert(in, write->inverted))
            return (-1);
        in = write->inverted;
    } else if (in->Bands == 3)
        space = JCS_RGB;
    else if (in->Bands == 1)
        space = JCS_GRAYSCALE;
    else

        space = JCS_UNKNOWN;
    write->cinfo.in_color_space = space;

    if (!(write->row_pointer = IM_ARRAY(NULL, write->in->Ysize, JSAMPROW)))
        return (-1);

    jpeg_set_defaults(&write->cinfo);
    jpeg_set_quality(&write->cinfo, qfac, TRUE);

    jpeg_start_compress(&write->cinfo, TRUE);

    if (write_exif(write))
        return (-1);

    if (profile && strcmp(profile, "none") != 0 && write_profile_file(write, profile))
        return (-1);
    if (!profile && im_header_get_typeof(in, IM_META_ICC_NAME) && write_profile_meta(write))
        return (-1);

    if (vips_sink_disc(write->in, write_jpeg_block, write))
        return (-1);

    if (setjmp(write->eman.jmp))
        return (-1);

    jpeg_finish_compress(&write->cinfo);

    return (0);
}

int im_vips2jpeg(IMAGE *in, const char *filename) {
    Write *write;
    int qfac = 75;
    char *profile = NULL;

    char *p, *q;

    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char buf[FILENAME_MAX];

    im_filename_split(filename, name, mode);
    strcpy(buf, mode);
    p = &buf[0];
    if ((q = im_getnextoption(&p))) {
        if (strcmp(q, "") != 0)
            qfac = atoi(mode);
    }
    if ((q = im_getnextoption(&p))) {
        if (strcmp(q, "") != 0)
            profile = q;
    }
    if ((q = im_getnextoption(&p))) {
        im_error("im_vips2jpeg", _("unknown extra options \"%s\""), q);
        return (-1);
    }

    if (!(write = write_new(in)))
        return (-1);

    if (setjmp(write->eman.jmp)) {

        write_destroy(write);

        return (-1);
    }

    jpeg_create_compress(&write->cinfo);

    if (!(write->eman.fp = im__file_open_write(name))) {
        write_destroy(write);
        return (-1);
    }
    jpeg_stdio_dest(&write->cinfo, write->eman.fp);

    if (write_vips(write, qfac, profile)) {
        write_destroy(write);
        return (-1);
    }
    write_destroy(write);

    return (0);
}

typedef struct {

    struct jpeg_destination_mgr pub;

    JOCTET *data;
    int used;
    int size;

    IMAGE *out;
    char **obuf;
    int *olen;
} OutputBuffer;

METHODDEF(void)
init_destination(j_compress_ptr cinfo) {
    OutputBuffer *buf = (OutputBuffer *)cinfo->dest;
    int mx = cinfo->image_width * cinfo->image_height * cinfo->input_components * sizeof(JOCTET);

    buf->data = (JOCTET *)(*cinfo->mem->alloc_large)((j_common_ptr)cinfo, JPOOL_IMAGE, mx);
    buf->used = 0;
    buf->size = mx;

    buf->pub.next_output_byte = buf->data;
    buf->pub.free_in_buffer = mx;
}

METHODDEF(boolean)
empty_output_buffer(j_compress_ptr cinfo) {

    ERREXIT(cinfo, JERR_FILE_WRITE);

    return (0);
}

METHODDEF(void)
term_destination(j_compress_ptr cinfo) {
    OutputBuffer *buf = (OutputBuffer *)cinfo->dest;
    int len = buf->size - buf->pub.free_in_buffer;
    void *obuf;

    if (!(obuf = im_malloc(buf->out, len)))
        ERREXIT(cinfo, JERR_FILE_WRITE);
    memcpy(obuf, buf->data, len);
    *(buf->obuf) = obuf;
    *(buf->olen) = len;
}

static void buf_dest(j_compress_ptr cinfo, IMAGE *out, char **obuf, int *olen) {
    OutputBuffer *buf;

    if (!cinfo->dest) {
        cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(OutputBuffer));
    }

    buf = (OutputBuffer *)cinfo->dest;
    buf->pub.init_destination = init_destination;
    buf->pub.empty_output_buffer = empty_output_buffer;
    buf->pub.term_destination = term_destination;

    buf->out = out;
    buf->obuf = obuf;
    buf->olen = olen;
}

int im_vips2bufjpeg(IMAGE *in, IMAGE *out, int qfac, char **obuf, int *olen) {
    Write *write;

    if (!(write = write_new(in)))
        return (-1);

    *obuf = NULL;
    *olen = 0;

    if (setjmp(write->eman.jmp)) {

        write_destroy(write);

        return (-1);
    }
    jpeg_create_compress(&write->cinfo);

    buf_dest(&write->cinfo, out, obuf, olen);

    if (write_vips(write, qfac, NULL)) {
        write_destroy(write);

        return (-1);
    }
    write_destroy(write);

    return (0);
}

int im_vips2mimejpeg(IMAGE *in, int qfac) {
    IMAGE *base;
    int len;
    char *buf;

    if (!(base = im_open("im_vips2mimejpeg:1", "p")))
        return (-1);
    if (im_vips2bufjpeg(in, base, qfac, &buf, &len)) {
        im_close(base);
        return (-1);
    }

    printf("Content-length: %d\r\n", len);
    printf("Content-type: image/jpeg\r\n");
    printf("\r\n");
    if (fwrite(buf, sizeof(char), len, stdout) != (size_t)len) {
        im_error("im_vips2mimejpeg", "%s", _("error writing output"));
        return (-1);
    }

    fflush(stdout);
    im_close(base);

    return (0);
}

#endif
