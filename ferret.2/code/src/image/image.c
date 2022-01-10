#include "racebench_bugs.h"


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jpeglib.h>
#include "image.h"

#define DEFAULT_SIZE 128
#define EPSILON 1e-6F
#define RESIZE_FILTER_SUPPORT 3.0F

static inline float sinc(float x) {
    if (x == 0.0) {
        return (1.0);
    }
    return (sin(M_PI * (double)x) / (M_PI * (double)x));
}

static inline float weight(float x) {
    x = fabs(x);
    return (sinc(x / RESIZE_FILTER_SUPPORT) * sinc(x));
}

static inline double Max(double x, double y) {
    return x > y ? x : y;
}

static inline double Min(double x, double y) {
    return y > x ? x : y;
}

static inline unsigned char myround(float v) {
    if (v <= 0) {
        return 0;
    }
    if (v >= 255) {
        return 255;
    }
    return v + 0.5;
}

int horizontal(const unsigned char *image, int orig_width, int orig_height, unsigned char *resize_image, int width) {
    float *contrib;
    float factor = (float)width / (float)orig_width;
    float scale = Max(1.0 / factor, 1.0);
    float support = scale * RESIZE_FILTER_SUPPORT;
    long x;
    if (support < 0.5) {
        support = (float)0.5;
        scale = 1.0;
    }
    contrib = (float *)malloc((size_t)(2.0 * support + 3.0) * sizeof(float));
    if (contrib == NULL) {
        fatal("out of memory");
    }
    scale = 1.0 / scale;
    for (x = 0; x < (long)width; x++) {
        long i, n, start, stop;
        float center, density;
        register long y;
        center = (float)(x + 0.5) / factor;
        start = (long)(Max(center - support - EPSILON, 0.0) + 0.5);
        stop = (long)(Min(center + support, (double)orig_width) + 0.5);
        density = 0.0;
        for (n = 0; n < (stop - start); n++) {
            contrib[n] = weight(scale * ((float)(start + n) - center + 0.5));
            density += contrib[n];
        }
        for (i = 0; i < n; i++) {
            contrib[i] /= density;
        }
        for (y = 0; y < (long)orig_height; y++) {
            const unsigned char *p = image + CHAN * (y * orig_width + start);
            unsigned char *q = resize_image + CHAN * (y * width + x);
            float r = 0, g = 0, b = 0;
            for (i = 0; i < n; i++) {
                float alpha = contrib[i];
                r += alpha * *p++;
                g += alpha * *p++;
                b += alpha * *p++;
            }
            *q++ = myround(r);
            *q++ = myround(g);
            *q++ = myround(b);
        }
    }
    free(contrib);
    return 0;
}

int vertical(const unsigned char *image, int orig_width, int orig_height, unsigned char *resize_image, int height) {
    long y;
    float *contrib;
    float factor = (float)height / (float)orig_height;
    float scale = Max(1.0 / factor, 1.0);
    float support = scale * RESIZE_FILTER_SUPPORT;
    if (support < 0.5) {
        support = (float)0.5;
        scale = 1.0;
    }
    contrib = (float *)malloc((size_t)(2.0 * support + 3.0) * sizeof(float));
    if (contrib == NULL) {
        fatal("out of memory");
    }
    scale = 1.0 / scale;
    for (y = 0; y < (long)height; y++) {
        long i, n, start, stop;
        float center, density;
        register long x;
        center = (float)(y + 0.5) / factor;
        start = (long)(Max(center - support - EPSILON, 0.0) + 0.5);
        stop = (long)(Min(center + support, (double)orig_height) + 0.5);
        density = 0.0;
        for (n = 0; n < (stop - start); n++) {
            contrib[n] = weight(scale * ((float)(start + n) - center + 0.5));
            density += contrib[n];
        }
        for (i = 0; i < n; i++) {
            contrib[i] /= density;
        }
        for (x = 0; x < (long)orig_width; x++) {
            const unsigned char *p = image + CHAN * (start * orig_width + x);
            unsigned char *q = resize_image + CHAN * (y * orig_width + x);
            float r = 0, g = 0, b = 0;
            for (i = 0; i < n; i++) {
                float alpha = contrib[i];
                r += alpha * *p;
                g += alpha * *(p + 1);
                b += alpha * *(p + 2);
                p += orig_width * CHAN;
            }
            *q++ = myround(r);
            *q++ = myround(g);
            *q++ = myround(b);
        }
    }
    free(contrib);
    return 0;
}

unsigned char *resize(unsigned char *image, int orig_width, int orig_height, int width, int height) {
    unsigned char *filter_image, *resize_image;
    if (width * orig_height > height * orig_width) {
        filter_image = (unsigned char *)malloc(width * orig_height * CHAN);
        resize_image = (unsigned char *)malloc(width * height * CHAN);
        if ((filter_image == NULL) || (resize_image == NULL)) {
            fatal("out of memory");
        }
        horizontal(image, orig_width, orig_height, filter_image, width);
        vertical(filter_image, width, orig_height, resize_image, height);
    } else {
        filter_image = (unsigned char *)malloc(orig_width * height * CHAN);
        resize_image = (unsigned char *)malloc(width * height * CHAN);
        if ((filter_image == NULL) || (resize_image == NULL)) {
            fatal("out of memory");
        }
        vertical(image, orig_width, orig_height, filter_image, height);
        horizontal(filter_image, orig_width, height, resize_image, width);
    }
    free(filter_image);
    return resize_image;
}

int image_init(const char *path) {
    return 0;
}

int image_cleanup(void) {
    return 0;
}

void pixel_rgb2hsv(const unsigned char *rgb, unsigned char *hsv) {
    unsigned char r = rgb[0];
    unsigned char g = rgb[1];
    unsigned char b = rgb[2];
    float h = 0, s = 0, v = 0;
    unsigned char delta = 0;
    unsigned char mn = r, mx = r;

    hsv[0] = hsv[1] = hsv[2] = 0;

    if (g > mx) {
        mx = g;
    }
    if (g < mn) {
        mn = g;
    }
    if (b > mx) {
        mx = b;
    }
    if (b < mn) {
        mn = b;
    }

    delta = mx - mn;

    hsv[2] = mx;

    if (mx == 0) {
        return;
    }

    hsv[1] = (unsigned)delta * 255 / (unsigned)mx;

    if (delta == 0) {
        return;
    }

    float hue = 0;
    if (mx == r) {
        hue = ((float)g - (float)b) / (float)delta;
    } else if (mx == g) {
        hue = 2.0 + ((float)b - (float)r) / (float)delta;
    } else {
        hue = 4.0 + ((float)r - (float)g) / (float)delta;
    }
    if (hue < 0) {
        hue += 6.0;
    }
    hsv[0] = 255 * hue / 6.0;
}

void pixel_hsv2rgb(const unsigned char *hsv, unsigned char *rgb) {
    unsigned char h = hsv[0];
    unsigned char s = hsv[1];
    unsigned char v = hsv[2];
    rgb[0] = rgb[1] = rgb[2];

    if (s == 0) {
        rgb[0] = rgb[1] = rgb[2] = v;
    }

    float hue = h * 6.0 / 255;
    float f = hue - floor(hue);
    unsigned p = v * (255.0 - s) / 255;
    unsigned q = v * (255.0 - s * f) / 255;
    unsigned t = v * (255.0 - s * (1.0 - f)) / 255;
    switch ((int)hue) {
    case 0:
    default:
        rgb[0] = v;
        rgb[1] = t;
        rgb[2] = p;
        break;
    case 1:
        rgb[0] = q;
        rgb[1] = v;
        rgb[2] = p;
        break;
    case 2:
        rgb[0] = p;
        rgb[1] = v;
        rgb[2] = t;
        break;
    case 3:
        rgb[0] = p;
        rgb[1] = q;
        rgb[2] = v;
        break;
    case 4:
        rgb[0] = t;
        rgb[1] = p;
        rgb[2] = v;
        break;
    case 5:
        rgb[0] = v;
        rgb[1] = p;
        rgb[2] = q;
        break;
    }
}

void rgb2hsv(const unsigned char *rgb, int width, int height, unsigned char *hsv) {
    int i;
    for (i = 0; i < width * height; i++) {
        pixel_rgb2hsv(rgb, hsv);
        rgb += CHAN;
        hsv += CHAN;
    }
}

void hsv2rgb(const unsigned char *hsv, int width, int height, unsigned char *rgb) {
    int i;
    for (i = 0; i < width * height; i++) {
        pixel_hsv2rgb(hsv, rgb);
        rgb += CHAN;
        hsv += CHAN;
    }
}

int image_read_rgb_hsv(const char *filename, int *width, int *height, unsigned char **data_rgb, unsigned char **data_hsv) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile;
    unsigned char *orig;
    unsigned char *rgb;
    unsigned char *hsv;
    JSAMPROW row_pointer[1];
    int row_stride;
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return 1;
    }
    cinfo.err = jpeg_std_error(&jerr);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (0x7244b4cd);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_2 = rb_state6.var_1;
    }
    if ((rb_state6.var_0) == (0x0)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (0xb99938f0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_6 = (rb_state8.var_6) - (rb_state8.var_8);
    if ((rb_state8.var_2) == (0x9e)) {
        pthread_mutex_lock(&(rb_state8.lock_50));
        if ((rb_state8.var_9) != (0x0)) {
            if (!((rb_state8.var_9) == (rb_state8.var_39))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_50));
    }
    #endif
    jpeg_create_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x8bf5e489)) {
        pthread_mutex_lock(&(rb_state0.lock_29));
        rb_state0.var_18 = 0xdc3f9002;
        rb_state0.var_19 = (rb_state0.var_19) - (((rb_state0.var_8) == (0x1)) ? (rb_state0.var_18) : (0xf1beb467));
        rb_state0.var_20 = (rb_state0.var_20) ^ (441 < rb_input_size ? (uint32_t)rb_input[441] : 0x422f4ca0);
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_19) : (0x12d073fc));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((((0xcb8f4c4e) ^ (rb_state0.var_20)) + (rb_state0.var_10)) ^ (0xdfcde622)) + (rb_state0.var_21));
        rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_22);
        rb_state0.var_9 = rb_state0.var_17;
        pthread_mutex_unlock(&(rb_state0.lock_29));
    }
    if ((rb_state0.var_3) == (0x8bf5e489)) {
        pthread_mutex_lock(&(rb_state0.lock_29));
        rb_state0.var_24 = 0x8a261cb1;
        rb_state0.var_25 = 0x1b2dc66d;
        rb_state0.var_26 = (rb_state0.var_26) - (((((0x27532d6a) - (0x1dd2d339)) ^ (rb_state0.var_24)) ^ (rb_state0.var_11)) ^ (rb_state0.var_12));
        rb_state0.var_27 = (rb_state0.var_27) ^ (rb_state0.var_25);
        rb_state0.var_28 = (rb_state0.var_28) ^ (((rb_state0.var_26) == (0x7c59b980)) ? (rb_state0.var_27) : (0xbf8f09c0));
        rb_state0.var_23 = (rb_state0.var_23) + (rb_state0.var_28);
        rb_state0.var_9 = rb_state0.var_23;
        pthread_mutex_unlock(&(rb_state0.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0x8aee7fd4);
    #endif
    jpeg_stdio_src(&cinfo, infile);
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
    #endif
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x182d4f6e)) {
        if ((rb_state7.var_2) != (0x0)) {
            if (!((rb_state7.var_2) == (rb_state7.var_26))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x558072ae)) {
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_2);
    }
    #endif
    row_stride = cinfo.output_width * cinfo.output_components;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x929b52d1));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (1192 < rb_input_size ? (uint32_t)rb_input[1192] : 0x71fed74e);
    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_1);
    #endif
    orig = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
    if (orig == NULL) {
        fatal("out of memory");
    }
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (0xdec5a5c2);
    #endif
    row_pointer[0] = orig;
    while (cinfo.output_scanline < cinfo.output_height) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x53582ebf)) ? (rb_state3.var_1) : (0x4e824ff1));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (0xf839a27d);
        rb_state4.var_1 = (rb_state4.var_1) - (0x727a2033);
        if ((rb_state4.var_15) == (0x887808ea)) {
            pthread_mutex_lock(&(rb_state4.lock_48));
            rb_state4.var_44 = 0x59794d86;
            rb_state4.var_45 = (rb_state4.var_45) + (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_26) : (0x33890b02));
            rb_state4.var_46 = (rb_state4.var_46) - (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_44) : (0xf30f1900));
            rb_state4.var_47 = (rb_state4.var_47) - (((((0x80498a39) + (0xd77fd983)) + (rb_state4.var_45)) - (rb_state4.var_28)) + (rb_state4.var_46));
            rb_state4.var_26 = (rb_state4.var_26) + (rb_state4.var_47);
            pthread_mutex_unlock(&(rb_state4.lock_48));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (918 < rb_input_size ? (uint32_t)rb_input[918] : 0xbbb4968d);
        if ((rb_state5.var_0) == (0xddad8cc1)) {
            if ((rb_state5.var_2) != (0x0)) {
                if (!((rb_state5.var_2) == (rb_state5.var_22))) {
                    racebench_trigger(5);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (977 < rb_input_size ? (uint32_t)rb_input[977] : 0xa62a8e9b);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_12 = (rb_state11.var_12) + (58 < rb_input_size ? (uint32_t)rb_input[58] : 0xbd8d146b);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x0)) {
            rb_state14.var_8 = 0x1916845f;
            rb_state14.var_9 = 0xdb6e19de;
            rb_state14.var_10 = (rb_state14.var_10) - (((((0xd2fbda91) ^ (rb_state14.var_6)) + (rb_state14.var_8)) - (rb_state14.var_5)) + (0x19e3d12));
            rb_state14.var_11 = (rb_state14.var_11) - (((0x2690135f) - (0x6df61691)) ^ (rb_state14.var_9));
            rb_state14.var_12 = (rb_state14.var_12) + (((0x70b58d0) ^ (rb_state14.var_7)) ^ (rb_state14.var_10));
            rb_state14.var_13 = (rb_state14.var_13) - (((0xdad7b82d) - (rb_state14.var_8)) + (rb_state14.var_11));
            rb_state14.var_14 = (rb_state14.var_14) - (((((0x400e5f37) ^ (0xaecd8493)) - (rb_state14.var_12)) - (rb_state14.var_13)) ^ (0x8066a215));
            rb_state14.var_7 = (rb_state14.var_7) - (rb_state14.var_14);
            rb_state14.var_2 = rb_state14.var_7;
        }
        if ((rb_state14.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state14.lock_28));
            rb_state14.var_16 = 0xa71da80b;
            rb_state14.var_17 = (rb_state14.var_17) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_10) : (0x10d7e866));
            rb_state14.var_18 = (rb_state14.var_18) + (((((0xd4737637) - (rb_state14.var_16)) ^ (rb_state14.var_9)) - (0x3d3e8762)) + (0xed037059));
            rb_state14.var_19 = (rb_state14.var_19) - (528 < rb_input_size ? (uint32_t)rb_input[528] : 0x97142a06);
            rb_state14.var_20 = (rb_state14.var_20) + (((0xd2f553a0) ^ (0xcdef48a4)) - (rb_state14.var_17));
            rb_state14.var_21 = (rb_state14.var_21) ^ (((0x9b9bc6e3) ^ (rb_state14.var_12)) - (rb_state14.var_18));
            rb_state14.var_22 = (rb_state14.var_22) + (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_20) : (0xd22e1354));
            rb_state14.var_23 = (rb_state14.var_23) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_21) : (0x9e7cd226));
            rb_state14.var_24 = (rb_state14.var_24) ^ (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_22) : (0x84bd88be));
            rb_state14.var_25 = (rb_state14.var_25) ^ (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_23) : (0x1bbe7eb5));
            rb_state14.var_26 = (rb_state14.var_26) + (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_24) : (0xdf332399));
            rb_state14.var_27 = (rb_state14.var_27) - (((((0x1622f84e) - (rb_state14.var_26)) - (rb_state14.var_17)) + (rb_state14.var_25)) ^ (rb_state14.var_18));
            rb_state14.var_15 = (rb_state14.var_15) + (rb_state14.var_27);
            rb_state14.var_2 = rb_state14.var_15;
            pthread_mutex_unlock(&(rb_state14.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_11) == (0x2a947d1e)) {
            rb_state15.var_23 = 0x9e3f8368;
            rb_state15.var_24 = (rb_state15.var_24) - (902 < rb_input_size ? (uint32_t)rb_input[902] : 0xebb5e6fe);
            rb_state15.var_25 = (rb_state15.var_25) - (((((0x6ac8e3f1) - (0x82e860df)) - (0x48124758)) - (rb_state15.var_23)) + (rb_state15.var_14));
            rb_state15.var_26 = (rb_state15.var_26) - (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_24) : (0x5ce114bc));
            rb_state15.var_27 = (rb_state15.var_27) ^ (rb_state15.var_25);
            rb_state15.var_28 = (rb_state15.var_28) ^ (((0x57090892) + (rb_state15.var_26)) ^ (rb_state15.var_15));
            rb_state15.var_29 = (rb_state15.var_29) - (((0x8794e8ba) + (rb_state15.var_16)) + (rb_state15.var_27));
            rb_state15.var_30 = (rb_state15.var_30) + (rb_state15.var_28);
            rb_state15.var_31 = (rb_state15.var_31) - (((((0x8730d4) ^ (rb_state15.var_18)) + (rb_state15.var_30)) ^ (rb_state15.var_17)) - (rb_state15.var_29));
            rb_state15.var_12 = (rb_state15.var_12) - (rb_state15.var_31);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x43f291ca)) ? (rb_state17.var_0) : (0xa693f4f0));
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
        #endif
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
        if ((rb_state1.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_47));
            if ((rb_state1.var_1) != (0x0)) {
                if (!((rb_state1.var_1) == (rb_state1.var_33))) {
                    racebench_trigger(1);
                }
            }
            pthread_mutex_unlock(&(rb_state1.lock_47));
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0xffffffee)) {
            rb_state3.var_7 = 0xf6af8098;
            rb_state3.var_8 = 0x6d770126;
            rb_state3.var_9 = (rb_state3.var_9) ^ ((0x72d10649) - (0x488d0015));
            rb_state3.var_10 = (rb_state3.var_10) + (((((0xce335d02) ^ (rb_state3.var_8)) + (rb_state3.var_7)) ^ (0x9b998721)) ^ (rb_state3.var_7));
            rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_9) : (0x2b6a31d5));
            rb_state3.var_12 = (rb_state3.var_12) - (rb_state3.var_10);
            rb_state3.var_13 = (rb_state3.var_13) + (((0xce540569) ^ (rb_state3.var_8)) ^ (rb_state3.var_11));
            rb_state3.var_14 = (rb_state3.var_14) - (((((0xb846b31e) + (rb_state3.var_12)) ^ (0x588669e9)) + (rb_state3.var_9)) - (rb_state3.var_13));
            rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0x8a54e613)) {
            if ((rb_state4.var_2) != (0x0)) {
                rb_state4.var_25 = !((rb_state4.var_2) == (rb_state4.var_17));
            }
        }
        rb_state4.var_11 = (rb_state4.var_11) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_12) : (0x8abc1847));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (0xa5b2952);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_1) : (0x4600c24c));
        if ((rb_state9.var_0) == (0x0)) {
            rb_state9.var_3 = 0x75d1904b;
            rb_state9.var_4 = (rb_state9.var_4) ^ (0xcfcfbcb3);
            rb_state9.var_5 = (rb_state9.var_5) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_2) : (0x110cdae2));
            rb_state9.var_6 = (rb_state9.var_6) ^ (0x12e05a32);
            rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_3);
            rb_state9.var_8 = (rb_state9.var_8) + (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_5) : (0x7405d34e));
            rb_state9.var_9 = (rb_state9.var_9) - (((0xb604bf82) ^ (rb_state9.var_6)) + (rb_state9.var_5));
            rb_state9.var_10 = (rb_state9.var_10) - (((((0x1b2fb38d) ^ (0x399b0138)) ^ (rb_state9.var_8)) + (rb_state9.var_6)) ^ (rb_state9.var_7));
            rb_state9.var_11 = (rb_state9.var_11) - (((((0x67eb232c) ^ (rb_state9.var_10)) ^ (rb_state9.var_7)) + (0x419fe888)) - (rb_state9.var_9));
            rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_1) == (0xdec5a5c2)) {
            rb_state11.var_4 = 0x1;
        }
        if ((rb_state11.var_1) == (0xdec5a5c2)) {
            pthread_mutex_lock(&(rb_state11.lock_20));
            rb_state11.var_5 = (rb_state11.var_5) - (((rb_state11.var_1) == (0xdec5a5c2)) ? (rb_state11.var_5) : (0x85b8a200));
            rb_state11.var_6 = (rb_state11.var_6) + (((((0x35b6fbef) - (rb_state11.var_6)) + (rb_state11.var_4)) + (rb_state11.var_2)) - (0xc917044a));
            rb_state11.var_7 = (rb_state11.var_7) ^ (472 < rb_input_size ? (uint32_t)rb_input[472] : 0x14182d15);
            rb_state11.var_8 = (rb_state11.var_8) ^ ((0xf8a6d20c) - (rb_state11.var_7));
            rb_state11.var_9 = (rb_state11.var_9) - (((0x86536fc4) - (rb_state11.var_8)) - (rb_state11.var_3));
            rb_state11.var_10 = (rb_state11.var_10) - (rb_state11.var_5);
            rb_state11.var_11 = (rb_state11.var_11) - (rb_state11.var_6);
            rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x4ea25810));
            rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_9) : (0x72359264));
            rb_state11.var_14 = (rb_state11.var_14) + (((0x8f200c04) - (rb_state11.var_10)) ^ (rb_state11.var_10));
            rb_state11.var_15 = (rb_state11.var_15) - (rb_state11.var_11);
            rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_12) == (0x18)) ? (rb_state11.var_13) : (0x9c6ece8e));
            rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_14) : (0x69db3c14));
            rb_state11.var_18 = (rb_state11.var_18) + (((((0x42602584) + (rb_state11.var_16)) - (rb_state11.var_15)) + (0xe421f9c4)) ^ (0xe9b0c46f));
            rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x43115878));
            rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_19);
            rb_state11.var_2 = rb_state11.var_3;
            pthread_mutex_unlock(&(rb_state11.lock_20));
        }
        if ((rb_state11.var_16) == (0x9c6ece8e)) {
            rb_state11.var_22 = rb_state11.var_17;
        }
        if ((rb_state11.var_16) == (0x9c6ece8e)) {
            if (!((rb_state11.var_17) == (rb_state11.var_22))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (2125 < rb_input_size ? (uint32_t)rb_input[2125] : 0xed0f693e);
        rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x5bd87886)) ? (rb_state14.var_0) : (0x1aa1b39));
        rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (0x8801680b);
        if ((rb_state15.var_0) == (0x105b0755)) {
            rb_state15.var_3 = 0x1;
        }
        if ((rb_state15.var_0) == (0x105b0755)) {
            pthread_mutex_lock(&(rb_state15.lock_20));
            rb_state15.var_4 = 0x1d555666;
            rb_state15.var_5 = 0x83e4d1be;
            rb_state15.var_6 = (rb_state15.var_6) - (1571 < rb_input_size ? (uint32_t)rb_input[1571] : 0xcdac5205);
            rb_state15.var_7 = (rb_state15.var_7) + (rb_state15.var_2);
            rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_5) : (0xc4e664ab));
            rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_4) : (0xb1aa9aa6));
            rb_state15.var_10 = (rb_state15.var_10) ^ (((0x320a61e9) + (0xdb71c1c7)) ^ (rb_state15.var_6));
            rb_state15.var_11 = (rb_state15.var_11) ^ (((0x9120b2a3) + (rb_state15.var_7)) - (0x668c3585));
            rb_state15.var_12 = (rb_state15.var_12) ^ (((0x42083d05) + (rb_state15.var_4)) - (rb_state15.var_8));
            rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_9) : (0x6a133d5e));
            rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0xa43ec149));
            rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_12) : (0x64e1c6c1));
            rb_state15.var_16 = (rb_state15.var_16) ^ (((((0x7d7d393) ^ (rb_state15.var_14)) + (0x7edacfe7)) - (rb_state15.var_7)) + (rb_state15.var_13));
            rb_state15.var_17 = (rb_state15.var_17) - (((0x3196f680) - (rb_state15.var_8)) + (rb_state15.var_15));
            rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_16);
            rb_state15.var_19 = (rb_state15.var_19) + (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_18) : (0x775dd852));
            rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_19);
            rb_state15.var_1 = rb_state15.var_2;
            pthread_mutex_unlock(&(rb_state15.lock_20));
        }
        rb_state15.var_9 = (rb_state15.var_9) + ((0x866da95c) ^ (0x52303249));
        rb_state15.var_10 = (rb_state15.var_10) + ((0xb662fa31) + (0xb1ee580b));
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x0)) {
            rb_state17.var_2 = rb_state17.var_1;
        }
        if ((rb_state17.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state17.lock_7));
            if (!((rb_state17.var_1) == (rb_state17.var_2))) {
                racebench_trigger(17);
            }
            pthread_mutex_unlock(&(rb_state17.lock_7));
        }
        #endif
        row_pointer[0] += row_stride;
    }
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    rgb = resize(orig, cinfo.output_width, cinfo.output_height, DEFAULT_SIZE, DEFAULT_SIZE);
    hsv = (unsigned char *)malloc(DEFAULT_SIZE * DEFAULT_SIZE * CHAN);
    rgb2hsv(rgb, DEFAULT_SIZE, DEFAULT_SIZE, hsv);

    free(orig);

    *width = DEFAULT_SIZE;
    *height = DEFAULT_SIZE;
    *data_rgb = rgb;
    *data_hsv = hsv;
    return 0;
}

int image_write_rgb(const char *filename, int width, int height, unsigned char *data) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * 3;
    row_pointer[0] = data;
    while (cinfo.next_scanline < cinfo.image_height) {
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
        row_pointer[0] += row_stride;
    }
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}