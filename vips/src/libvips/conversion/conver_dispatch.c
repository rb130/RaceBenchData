

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int system_vec(im_object *argv) {
    IMAGE *in = argv[0];
    char *cmd = argv[1];
    char **out = (char **)&argv[2];

    if (im_system(in, cmd, out)) {
        return (-1);
    }

    return (0);
}

static im_arg_desc system_args[] = {IM_INPUT_IMAGE("im"), IM_INPUT_STRING("command"), IM_OUTPUT_STRING("output")};

static im_function system_desc = {"im_system", "run command on image", 0, system_vec, IM_NUMBER(system_args), system_args};

static int system_image_vec(im_object *argv) {
    IMAGE *in = argv[0];
    IMAGE *out = argv[1];
    char *in_format = argv[2];
    char *out_format = argv[3];
    char *cmd = argv[4];
    char **log = (char **)&argv[5];

    IMAGE *out_image;

    if (!(out_image = im_system_image(in, in_format, out_format, cmd, log))) {
        im_error("im_system_image", "%s", *log);
        im_free(*log);
        return (-1);
    }

    if (im_copy(out_image, out) || im_add_close_callback(out, (im_callback_fn)im_close, out_image, NULL)) {
        im_close(out_image);
        return (-1);
    }

    return (0);
}

static im_arg_desc system_image_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("in_format"), IM_INPUT_STRING("out_format"), IM_INPUT_STRING("command"), IM_OUTPUT_STRING("log")};

static im_function system_image_desc = {"im_system_image", "run command on image, with image output", 0, system_image_vec, IM_NUMBER(system_image_args), system_image_args};

static int subsample_vec(im_object *argv) {
    IMAGE *in = argv[0];
    IMAGE *out = argv[1];
    int xsh = *((int *)argv[2]);
    int ysh = *((int *)argv[3]);

    if (im_subsample(in, out, xsh, ysh)) {
        return (-1);
    }

    return (0);
}

static im_arg_desc subsample_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xshrink"), IM_INPUT_INT("yshrink")};

static im_function subsample_desc = {"im_subsample", "subsample image by integer factors", IM_FN_PIO, subsample_vec, IM_NUMBER(subsample_args), subsample_args};

static im_arg_desc gaussnoise_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("xsize"), IM_INPUT_INT("ysize"), IM_INPUT_DOUBLE("mean"), IM_INPUT_DOUBLE("sigma")};

static int gaussnoise_vec(im_object *argv) {
    int xsize = *((int *)argv[1]);
    int ysize = *((int *)argv[2]);
    double mean = *((double *)argv[3]);
    double sigma = *((double *)argv[4]);

    if (im_gaussnoise(argv[0], xsize, ysize, mean, sigma)) {
        return (-1);
    }

    return (0);
}

static im_function gaussnoise_desc = {"im_gaussnoise", "generate image of gaussian noise with specified statistics", IM_FN_PIO, gaussnoise_vec, IM_NUMBER(gaussnoise_args), gaussnoise_args};

static im_arg_desc extract_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("left"), IM_INPUT_INT("top"), IM_INPUT_INT("width"), IM_INPUT_INT("height"), IM_INPUT_INT("band")};

static int extract_vec(im_object *argv) {
    int left = *((int *)argv[2]);
    int top = *((int *)argv[3]);
    int width = *((int *)argv[4]);
    int height = *((int *)argv[5]);
    int band = *((int *)argv[6]);

    return (im_extract_areabands(argv[0], argv[1], left, top, width, height, band, 1));
}

static im_function extract_desc = {"im_extract", "extract area/band", IM_FN_TRANSFORM | IM_FN_PIO, extract_vec, IM_NUMBER(extract_args), extract_args};

static im_arg_desc extract_area_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("left"), IM_INPUT_INT("top"), IM_INPUT_INT("width"), IM_INPUT_INT("height")};

static int extract_area_vec(im_object *argv) {
    int x = *((int *)argv[2]);
    int y = *((int *)argv[3]);
    int w = *((int *)argv[4]);
    int h = *((int *)argv[5]);

    return (im_extract_area(argv[0], argv[1], x, y, w, h));
}

static im_function extract_area_desc = {"im_extract_area", "extract area", IM_FN_TRANSFORM | IM_FN_PIO, extract_area_vec, IM_NUMBER(extract_area_args), extract_area_args};

static im_arg_desc extract_bands_args[] = {
    IM_INPUT_IMAGE("input"),
    IM_OUTPUT_IMAGE("output"),
    IM_INPUT_INT("band"),
    IM_INPUT_INT("nbands"),
};

static int extract_bands_vec(im_object *argv) {
    int chsel = *((int *)argv[2]);
    int nbands = *((int *)argv[3]);

    return (im_extract_bands(argv[0], argv[1], chsel, nbands));
}

static im_function extract_bands_desc = {"im_extract_bands", "extract several bands", IM_FN_PIO, extract_bands_vec, IM_NUMBER(extract_bands_args), extract_bands_args};

static im_arg_desc extract_band_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("band")};

static int extract_band_vec(im_object *argv) {
    int chsel = *((int *)argv[2]);

    return (im_extract_band(argv[0], argv[1], chsel));
}

static im_function extract_band_desc = {"im_extract_band", "extract band", IM_FN_PIO, extract_band_vec, IM_NUMBER(extract_band_args), extract_band_args};

static im_arg_desc extract_areabands_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("left"), IM_INPUT_INT("top"), IM_INPUT_INT("width"), IM_INPUT_INT("height"), IM_INPUT_INT("band"), IM_INPUT_INT("nbands")};

static int extract_areabands_vec(im_object *argv) {
    int left = *((int *)argv[2]);
    int top = *((int *)argv[3]);
    int width = *((int *)argv[4]);
    int height = *((int *)argv[5]);
    int band = *((int *)argv[6]);
    int nbands = *((int *)argv[7]);

    return (im_extract_areabands(argv[0], argv[1], left, top, width, height, band, nbands));
}

static im_function extract_areabands_desc = {"im_extract_areabands", "extract area and bands", IM_FN_TRANSFORM | IM_FN_PIO, extract_areabands_vec, IM_NUMBER(extract_areabands_args), extract_areabands_args};

static im_arg_desc one_in_one_out[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out")};

static im_arg_desc two_in_one_out[] = {IM_INPUT_IMAGE("in1"), IM_INPUT_IMAGE("in2"), IM_OUTPUT_IMAGE("out")};

static int bandjoin_vec(im_object *argv) {
    return (im_bandjoin(argv[0], argv[1], argv[2]));
}

static im_function bandjoin_desc = {"im_bandjoin", "bandwise join of two images", IM_FN_PIO, bandjoin_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc gbandjoin_args[] = {IM_INPUT_IMAGEVEC("in"), IM_OUTPUT_IMAGE("out")};

static int gbandjoin_vec(im_object *argv) {
    im_imagevec_object *iv = (im_imagevec_object *)argv[0];

    return (im_gbandjoin(iv->vec, argv[1], iv->n));
}

static im_function gbandjoin_desc = {"im_gbandjoin", "bandwise join of many images", IM_FN_PIO, gbandjoin_vec, IM_NUMBER(gbandjoin_args), gbandjoin_args};

static im_arg_desc text_args[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("text"), IM_INPUT_STRING("font"), IM_INPUT_INT("width"), IM_INPUT_INT("alignment"), IM_INPUT_INT("dpi")};

static int text_vec(im_object *argv) {
    int width = *((int *)argv[3]);
    int alignment = *((int *)argv[4]);
    int dpi = *((int *)argv[5]);

    return (im_text(argv[0], argv[1], argv[2], width, alignment, dpi));
}

static im_function text_desc = {"im_text", "generate text image", IM_FN_PIO, text_vec, IM_NUMBER(text_args), text_args};

static im_arg_desc black_args[] = {IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("x_size"), IM_INPUT_INT("y_size"), IM_INPUT_INT("bands")};

static int black_vec(im_object *argv) {
    int xs = *((int *)argv[1]);
    int ys = *((int *)argv[2]);
    int bands = *((int *)argv[3]);

    return (im_black(argv[0], xs, ys, bands));
}

static im_function black_desc = {"im_black", "generate black image", IM_FN_PIO, black_vec, IM_NUMBER(black_args), black_args};

static im_arg_desc clip2fmt_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("ofmt")};

static int clip2fmt_vec(im_object *argv) {
    int ofmt = *((int *)argv[2]);

    return (im_clip2fmt(argv[0], argv[1], ofmt));
}

static im_function clip2fmt_desc = {"im_clip2fmt", "convert image format to ofmt", IM_FN_PIO | IM_FN_PTOP, clip2fmt_vec, IM_NUMBER(clip2fmt_args), clip2fmt_args};

static int c2rect_vec(im_object *argv) {
    return (im_c2rect(argv[0], argv[1]));
}

static im_function c2rect_desc = {"im_c2rect", "convert phase and amplitude to real and imaginary", IM_FN_PTOP | IM_FN_PIO, c2rect_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int c2amph_vec(im_object *argv) {
    return (im_c2amph(argv[0], argv[1]));
}

static im_function c2amph_desc = {"im_c2amph", "convert real and imaginary to phase and amplitude", IM_FN_PTOP | IM_FN_PIO, c2amph_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int ri2c_vec(im_object *argv) {
    return (im_ri2c(argv[0], argv[1], argv[2]));
}

static im_function ri2c_desc = {"im_ri2c", "join two non-complex images to form complex", IM_FN_PTOP | IM_FN_PIO, ri2c_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int c2imag_vec(im_object *argv) {
    return (im_c2imag(argv[0], argv[1]));
}

static im_function c2imag_desc = {"im_c2imag", "extract imaginary part of complex image", IM_FN_PTOP | IM_FN_PIO, c2imag_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int c2real_vec(im_object *argv) {
    return (im_c2real(argv[0], argv[1]));
}

static im_function c2real_desc = {"im_c2real", "extract real part of complex image", IM_FN_PTOP | IM_FN_PIO, c2real_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc copy_set_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("Type"), IM_INPUT_DOUBLE("Xres"), IM_INPUT_DOUBLE("Yres"), IM_INPUT_INT("Xoffset"), IM_INPUT_INT("Yoffset")};

static int copy_set_vec(im_object *argv) {
    int Type = *((int *)argv[2]);
    float Xres = *((double *)argv[3]);
    float Yres = *((double *)argv[4]);
    int Xoffset = *((int *)argv[5]);
    int Yoffset = *((int *)argv[6]);

    return (im_copy_set(argv[0], argv[1], Type, Xres, Yres, Xoffset, Yoffset));
}

static im_function copy_set_desc = {"im_copy_set", "copy image, setting informational fields",

    IM_FN_PIO,

    copy_set_vec, IM_NUMBER(copy_set_args), copy_set_args};

static im_arg_desc copy_set_meta_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_STRING("field"), IM_INPUT_GVALUE("value")};

static int copy_set_meta_vec(im_object *argv) {
    const char *field = argv[2];
    GValue *value = argv[3];

    return (im_copy_set_meta(argv[0], argv[1], field, value));
}

static im_function copy_set_meta_desc = {"im_copy_set_meta", "copy image, setting a meta field",

    IM_FN_PIO,

    copy_set_meta_vec, IM_NUMBER(copy_set_meta_args), copy_set_meta_args};

static im_arg_desc copy_morph_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("Bands"), IM_INPUT_INT("BandFmt"), IM_INPUT_INT("Coding")};

static int copy_morph_vec(im_object *argv) {
    int Bands = *((int *)argv[2]);
    int BandFmt = *((int *)argv[3]);
    int Coding = *((int *)argv[4]);

    return (im_copy_morph(argv[0], argv[1], Bands, BandFmt, Coding));
}

static im_function copy_morph_desc = {"im_copy_morph", "copy image, setting pixel layout",

    IM_FN_PIO,

    copy_morph_vec, IM_NUMBER(copy_morph_args), copy_morph_args};

static int copy_vec(im_object *argv) {
    return (im_copy(argv[0], argv[1]));
}

static im_function copy_desc = {"im_copy", "copy image",

    IM_FN_PIO,

    copy_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int copy_file_vec(im_object *argv) {
    return (im_copy_file(argv[0], argv[1]));
}

static im_function copy_file_desc = {"im_copy_file", "copy image to a file and return that",

    IM_FN_PIO,

    copy_file_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int copy_swap_vec(im_object *argv) {
    return (im_copy_swap(argv[0], argv[1]));
}

static im_function copy_swap_desc = {"im_copy_swap", "copy image, swapping byte order",

    IM_FN_PIO,

    copy_swap_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int fliphor_vec(im_object *argv) {
    return (im_fliphor(argv[0], argv[1]));
}

static im_function fliphor_desc = {"im_fliphor", "flip image left-right", IM_FN_PIO, fliphor_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int flipver_vec(im_object *argv) {
    return (im_flipver(argv[0], argv[1]));
}

static im_function flipver_desc = {"im_flipver", "flip image top-bottom", IM_FN_PIO, flipver_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int falsecolour_vec(im_object *argv) {
    return (im_falsecolour(argv[0], argv[1]));
}

static im_function falsecolour_desc = {"im_falsecolour", "turn luminance changes into chrominance changes", IM_FN_PTOP | IM_FN_PIO, falsecolour_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc insert_args[] = {IM_INPUT_IMAGE("in"), IM_INPUT_IMAGE("sub"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("x"), IM_INPUT_INT("y")};

static int insert_vec(im_object *argv) {
    int x = *((int *)argv[3]);
    int y = *((int *)argv[4]);

    return (im_insert(argv[0], argv[1], argv[2], x, y));
}

static im_function insert_desc = {"im_insert", "insert sub-image into main image at position", IM_FN_PIO | IM_FN_TRANSFORM, insert_vec, IM_NUMBER(insert_args), insert_args};

static im_arg_desc insertset_args[] = {IM_INPUT_IMAGE("main"), IM_INPUT_IMAGE("sub"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INTVEC("x"), IM_INPUT_INTVEC("y")};

static int insertset_vec(im_object *argv) {
    im_intvec_object *xv = (im_intvec_object *)argv[3];
    im_intvec_object *yv = (im_intvec_object *)argv[4];

    if (xv->n != yv->n) {
        im_error("im_insertset", "%s", _("vectors not same length"));
        return (-1);
    }

    if (im_insertset(argv[0], argv[1], argv[2], xv->n, xv->vec, yv->vec)) {
        return (-1);
    }

    return (0);
}

static im_function insertset_desc = {"im_insertset", "insert sub into main at every position in x, y", 0, insertset_vec, IM_NUMBER(insertset_args), insertset_args};

static int insert_noexpand_vec(im_object *argv) {
    int x = *((int *)argv[3]);
    int y = *((int *)argv[4]);

    return (im_insert_noexpand(argv[0], argv[1], argv[2], x, y));
}

static im_function insert_noexpand_desc = {"im_insert_noexpand", "insert sub-image into main image at position, no expansion", IM_FN_PIO | IM_FN_TRANSFORM, insert_noexpand_vec, IM_NUMBER(insert_args), insert_args};

static int rot180_vec(im_object *argv) {
    return (im_rot180(argv[0], argv[1]));
}

static im_function rot180_desc = {"im_rot180", "rotate image 180 degrees", IM_FN_PIO | IM_FN_TRANSFORM, rot180_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int rot90_vec(im_object *argv) {
    return (im_rot90(argv[0], argv[1]));
}

static im_function rot90_desc = {"im_rot90", "rotate image 90 degrees clockwise", IM_FN_PIO | IM_FN_TRANSFORM, rot90_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int rot270_vec(im_object *argv) {
    return (im_rot270(argv[0], argv[1]));
}

static im_function rot270_desc = {"im_rot270", "rotate image 270 degrees clockwise", IM_FN_PIO | IM_FN_TRANSFORM, rot270_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int lrjoin_vec(im_object *argv) {
    return (im_lrjoin(argv[0], argv[1], argv[2]));
}

static im_function lrjoin_desc = {"im_lrjoin", "join two images left-right", IM_FN_PIO | IM_FN_TRANSFORM, lrjoin_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static int tbjoin_vec(im_object *argv) {
    return (im_tbjoin(argv[0], argv[1], argv[2]));
}

static im_function tbjoin_desc = {"im_tbjoin", "join two images top-bottom", IM_FN_PIO | IM_FN_TRANSFORM, tbjoin_vec, IM_NUMBER(two_in_one_out), two_in_one_out};

static im_arg_desc mask2vips_args[] = {
    IM_INPUT_DMASK("input"),
    IM_OUTPUT_IMAGE("output"),
};

static int mask2vips_vec(im_object *argv) {
    im_mask_object *mo = argv[0];

    return (im_mask2vips(mo->mask, argv[1]));
}

static im_function mask2vips_desc = {"im_mask2vips", "convert DOUBLEMASK to VIPS image", 0, mask2vips_vec, IM_NUMBER(mask2vips_args), mask2vips_args};

static im_arg_desc vips2mask_args[] = {
    IM_INPUT_IMAGE("input"),
    IM_OUTPUT_DMASK("output"),
};

static int vips2mask_vec(im_object *argv) {
    im_mask_object *mo = argv[1];

    if (!(mo->mask = im_vips2mask(argv[0], mo->name))) {
        return (-1);
    }

    return (0);
}

static im_function vips2mask_desc = {"im_vips2mask", "convert VIPS image to DOUBLEMASK", 0, vips2mask_vec, IM_NUMBER(vips2mask_args), vips2mask_args};

static int scale_vec(im_object *argv) {
    return (im_scale(argv[0], argv[1]));
}

static im_function scale_desc = {"im_scale", "scale image linearly to fit range 0-255", IM_FN_PIO, scale_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static int scaleps_vec(im_object *argv) {
    return (im_scaleps(argv[0], argv[1]));
}

static im_function scaleps_desc = {"im_scaleps", "logarithmic scale of image to fit range 0-255", 0, scaleps_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc grid_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("tile_height"), IM_INPUT_INT("across"), IM_INPUT_INT("down")};

static int grid_vec(im_object *argv) {
    int tile_height = *((int *)argv[2]);
    int across = *((int *)argv[3]);
    int down = *((int *)argv[4]);

    return (im_grid(argv[0], argv[1], tile_height, across, down));
}

static im_function grid_desc = {"im_grid", "chop a tall thin image into a grid of images", IM_FN_TRANSFORM | IM_FN_PIO, grid_vec, IM_NUMBER(grid_args), grid_args};

static im_arg_desc replicate_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("across"), IM_INPUT_INT("down")};

static int replicate_vec(im_object *argv) {
    int across = *((int *)argv[2]);
    int down = *((int *)argv[3]);

    return (im_replicate(argv[0], argv[1], across, down));
}

static im_function replicate_desc = {"im_replicate", "replicate an image horizontally and vertically", IM_FN_TRANSFORM | IM_FN_PIO, replicate_vec, IM_NUMBER(replicate_args), replicate_args};

static im_arg_desc zoom_args[] = {IM_INPUT_IMAGE("input"), IM_OUTPUT_IMAGE("output"), IM_INPUT_INT("xfac"), IM_INPUT_INT("yfac")};

static int zoom_vec(im_object *argv) {
    int xfac = *((int *)argv[2]);
    int yfac = *((int *)argv[3]);

    return (im_zoom(argv[0], argv[1], xfac, yfac));
}

static im_function zoom_desc = {"im_zoom", "simple zoom of an image by integer factors", IM_FN_TRANSFORM | IM_FN_PIO, zoom_vec, IM_NUMBER(zoom_args), zoom_args};

static int msb_vec(im_object *argv) {
    return im_msb(argv[0], argv[1]);
}

static im_function msb_desc = {"im_msb", "convert to uchar by discarding bits", IM_FN_PIO | IM_FN_PTOP, msb_vec, IM_NUMBER(one_in_one_out), one_in_one_out};

static im_arg_desc msb_band_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("band")};

static int msb_band_vec(im_object *argv) {
    IMAGE *in = (IMAGE *)argv[0];
    IMAGE *out = (IMAGE *)argv[1];
    int *band = (int *)argv[2];

    return im_msb_band(in, out, *band);
}

static im_function msb_band_desc = {"im_msb_band", "convert to single band uchar by discarding bits", IM_FN_PIO | IM_FN_PTOP, msb_band_vec, IM_NUMBER(msb_band_args), msb_band_args};

static im_arg_desc wrap_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("x"), IM_INPUT_INT("y")};

static int wrap_vec(im_object *argv) {
    return im_wrap(argv[0], argv[1], *(int *)argv[2], *(int *)argv[3]);
}

static im_function wrap_desc = {"im_wrap", "shift image origin, wrapping at sides", IM_FN_PIO | IM_FN_TRANSFORM, wrap_vec, IM_NUMBER(wrap_args), wrap_args};

static im_arg_desc embed_args[] = {IM_INPUT_IMAGE("in"), IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("type"), IM_INPUT_INT("x"), IM_INPUT_INT("y"), IM_INPUT_INT("width"), IM_INPUT_INT("height")};

static int embed_vec(im_object *argv) {
    int type = *((int *)argv[2]);
    int x = *((int *)argv[3]);
    int y = *((int *)argv[4]);
    int width = *((int *)argv[5]);
    int height = *((int *)argv[6]);

    return (im_embed(argv[0], argv[1], type, x, y, width, height));
}

static im_function embed_desc = {"im_embed", "embed in within a set of borders", IM_FN_PIO | IM_FN_TRANSFORM, embed_vec, IM_NUMBER(embed_args), embed_args};

static im_function *conv_list[] = {&gaussnoise_desc, &bandjoin_desc, &black_desc, &c2amph_desc, &c2imag_desc, &c2real_desc, &c2rect_desc, &clip2fmt_desc, &copy_desc, &copy_file_desc, &copy_morph_desc, &copy_swap_desc, &copy_set_desc, &copy_set_meta_desc, &extract_area_desc, &extract_areabands_desc, &extract_band_desc, &extract_bands_desc, &extract_desc, &falsecolour_desc, &fliphor_desc, &flipver_desc, &gbandjoin_desc, &grid_desc, &insert_desc, &insertset_desc, &insert_noexpand_desc, &embed_desc, &lrjoin_desc, &mask2vips_desc, &msb_desc, &msb_band_desc, &replicate_desc, &ri2c_desc, &rot180_desc, &rot270_desc, &rot90_desc, &scale_desc, &scaleps_desc, &subsample_desc, &system_desc, &system_image_desc, &tbjoin_desc, &text_desc, &vips2mask_desc, &wrap_desc, &zoom_desc};

im_package im__conversion = {"conversion", IM_NUMBER(conv_list), conv_list};
