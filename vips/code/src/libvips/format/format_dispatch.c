
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int jpeg2vips_vec(im_object *argv) {
    char *in = argv[0];
    IMAGE *out = argv[1];

    if (im_jpeg2vips(in, out)) {
        return (-1);
    }

    return (0);
}

static im_arg_desc jpeg2vips_args[] = {IM_INPUT_STRING("in"), IM_OUTPUT_IMAGE("out")};

static im_function jpeg2vips_desc = {"im_jpeg2vips", "convert from jpeg", 0, jpeg2vips_vec, IM_NUMBER(jpeg2vips_args), jpeg2vips_args};

static int vips2jpeg_vec(im_object *argv) {
    IMAGE *in = argv[0];
    char *out = argv[1];

    if (im_vips2jpeg(in, out)) {
        return (-1);
    }

    return (0);
}

static im_arg_desc vips2jpeg_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_STRING("out")};

static im_function vips2jpeg_desc = {"im_vips2jpeg", "convert to jpeg", 0, vips2jpeg_vec, IM_NUMBER(vips2jpeg_args), vips2jpeg_args};

static int vips2mimejpeg_vec(im_object *argv) {
    IMAGE *in = argv[0];
    int qfac = *((int *)argv[1]);

    if (im_vips2mimejpeg(in, qfac)) {
        return (-1);
    }

    return (0);
}

static im_arg_desc vips2mimejpeg_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_INT("qfac")};

static im_function vips2mimejpeg_desc = {"im_vips2mimejpeg", "convert to jpeg as mime type on stdout", 0, vips2mimejpeg_vec, IM_NUMBER(vips2mimejpeg_args), vips2mimejpeg_args};

static im_arg_desc vips2png_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_STRING("out")};

static int vips2png_vec(im_object *argv) {
    return (im_vips2png(argv[0], argv[1]));
}

static im_function vips2png_desc = {"im_vips2png", "convert VIPS image to PNG file", 0, vips2png_vec, IM_NUMBER(vips2png_args), vips2png_args};

static im_arg_desc png2vips_args[] = {IM_INPUT_STRING("in"), IM_OUTPUT_IMAGE("out")};

static int png2vips_vec(im_object *argv) {
    return (im_png2vips(argv[0], argv[1]));
}

static im_function png2vips_desc = {"im_png2vips", "convert PNG file to VIPS image", 0, png2vips_vec, IM_NUMBER(png2vips_args), png2vips_args};

static im_arg_desc exr2vips_args[] = {IM_INPUT_STRING("in"), IM_OUTPUT_IMAGE("out")};

static int exr2vips_vec(im_object *argv) {
    return (im_exr2vips(argv[0], argv[1]));
}

static im_function exr2vips_desc = {"im_exr2vips", "convert an OpenEXR file to VIPS", 0, exr2vips_vec, IM_NUMBER(exr2vips_args), exr2vips_args};

static im_arg_desc vips2tiff_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_STRING("out")};

static int vips2tiff_vec(im_object *argv) {
    return (im_vips2tiff(argv[0], argv[1]));
}

static im_function vips2tiff_desc = {"im_vips2tiff", "convert VIPS image to TIFF file", 0, vips2tiff_vec, IM_NUMBER(vips2tiff_args), vips2tiff_args};

static im_arg_desc magick2vips_args[] = {IM_INPUT_STRING("in"), IM_OUTPUT_IMAGE("out")};

static int magick2vips_vec(im_object *argv) {
    return (im_magick2vips(argv[0], argv[1]));
}

static im_function magick2vips_desc = {"im_magick2vips", "load file with libMagick", 0, magick2vips_vec, IM_NUMBER(magick2vips_args), magick2vips_args};

static im_arg_desc tiff2vips_args[] = {IM_INPUT_STRING("in"), IM_OUTPUT_IMAGE("out")};

static int tiff2vips_vec(im_object *argv) {
    return (im_tiff2vips(argv[0], argv[1]));
}

static im_function tiff2vips_desc = {"im_tiff2vips", "convert TIFF file to VIPS image", 0, tiff2vips_vec, IM_NUMBER(tiff2vips_args), tiff2vips_args};

static int analyze2vips_vec(im_object *argv) {
    const char *in = argv[0];
    IMAGE *out = argv[1];

    return (im_analyze2vips(in, out));
}

static im_arg_desc analyze2vips_arg_types[] = {IM_INPUT_STRING("filename"), IM_OUTPUT_IMAGE("im")};

static im_function analyze2vips_desc = {"im_analyze2vips", "read a file in analyze format", 0, analyze2vips_vec, IM_NUMBER(analyze2vips_arg_types), analyze2vips_arg_types};

static int csv2vips_vec(im_object *argv) {
    const char *in = argv[0];
    IMAGE *out = argv[1];

    return (im_csv2vips(in, out));
}

static im_arg_desc csv2vips_arg_types[] = {IM_INPUT_STRING("filename"), IM_OUTPUT_IMAGE("im")};

static im_function csv2vips_desc = {"im_csv2vips", "read a file in csv format", 0, csv2vips_vec, IM_NUMBER(csv2vips_arg_types), csv2vips_arg_types};

static int vips2csv_vec(im_object *argv) {
    IMAGE *in = argv[0];
    const char *filename = argv[1];

    return (im_vips2csv(in, filename));
}

static im_arg_desc vips2csv_arg_types[] = {IM_INPUT_IMAGE("in"), IM_INPUT_STRING("filename")};

static im_function vips2csv_desc = {"im_vips2csv", "write an image in csv format", 0, vips2csv_vec, IM_NUMBER(vips2csv_arg_types), vips2csv_arg_types};

static int ppm2vips_vec(im_object *argv) {
    const char *in = argv[0];
    IMAGE *out = argv[1];

    return (im_ppm2vips(in, out));
}

static im_arg_desc ppm2vips_arg_types[] = {IM_INPUT_STRING("filename"), IM_OUTPUT_IMAGE("im")};

static im_function ppm2vips_desc = {"im_ppm2vips", "read a file in pbm/pgm/ppm format", 0, ppm2vips_vec, IM_NUMBER(ppm2vips_arg_types), ppm2vips_arg_types};

static int vips2ppm_vec(im_object *argv) {
    IMAGE *im = argv[0];
    const char *filename = argv[1];

    return (im_vips2ppm(im, filename));
}

static im_arg_desc vips2ppm_arg_types[] = {IM_INPUT_IMAGE("im"), IM_INPUT_STRING("filename")};

static im_function vips2ppm_desc = {"im_vips2ppm", "write a file in pbm/pgm/ppm format", 0, vips2ppm_vec, IM_NUMBER(vips2ppm_arg_types), vips2ppm_arg_types};

static im_function *list[] = {&csv2vips_desc, &jpeg2vips_desc, &magick2vips_desc, &png2vips_desc, &exr2vips_desc, &ppm2vips_desc, &analyze2vips_desc, &tiff2vips_desc, &vips2csv_desc, &vips2jpeg_desc, &vips2mimejpeg_desc, &vips2png_desc, &vips2ppm_desc, &vips2tiff_desc};

im_package im__format = {"format", IM_NUMBER(list), list};
