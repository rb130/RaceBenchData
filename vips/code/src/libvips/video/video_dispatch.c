
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int video_v4l1_vec(im_object *argv) {
    IMAGE *out = argv[0];
    char *device = (char *)argv[1];
    int channel = *((int *)argv[2]);
    int brightness = *((int *)argv[3]);
    int colour = *((int *)argv[4]);
    int contrast = *((int *)argv[5]);
    int hue = *((int *)argv[6]);
    int ngrabs = *((int *)argv[7]);

    return (im_video_v4l1(out, device, channel, brightness, colour, contrast, hue, ngrabs));
}

static im_arg_desc video_v4l1_arg_types[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_STRING("device"), IM_INPUT_INT("channel"), IM_INPUT_INT("brightness"), IM_INPUT_INT("colour"), IM_INPUT_INT("contrast"), IM_INPUT_INT("hue"), IM_INPUT_INT("ngrabs")};

static im_function video_v4l1_desc = {"im_video_v4l1", "grab a video frame with v4l1", IM_FN_NOCACHE, video_v4l1_vec, IM_NUMBER(video_v4l1_arg_types), video_v4l1_arg_types};

static int video_test_vec(im_object *argv) {
    IMAGE *out = argv[0];
    int brightness = *((int *)argv[1]);
    int error = *((int *)argv[2]);

    return (im_video_test(out, brightness, error));
}

static im_arg_desc video_test_arg_types[] = {IM_OUTPUT_IMAGE("out"), IM_INPUT_INT("brightness"), IM_INPUT_INT("error")};

static im_function video_test_desc = {"im_video_test", "test video grabber", IM_FN_NOCACHE, video_test_vec, IM_NUMBER(video_test_arg_types), video_test_arg_types};

static im_function *video_list[] = {&video_test_desc, &video_v4l1_desc};

im_package im__video = {"video", IM_NUMBER(video_list), video_list};
