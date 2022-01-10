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
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x36ada835)) ? (rb_state9.var_2) : (0xf1c539ab));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0x8500fd17);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_2) == (0xb18b232)) ? (rb_state17.var_1) : (0x7e1ef0af));
    #endif
    cinfo.err = jpeg_std_error(&jerr);
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xde)) {
        if ((rb_state12.var_2) != (0x0)) {
            if (!((rb_state12.var_2) == (rb_state12.var_19))) {
                racebench_trigger(12);
            }
        }
    }
    #endif
    jpeg_create_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
    #endif
    jpeg_stdio_src(&cinfo, infile);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0x2a694811) - (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x5ff9962d)) {
        if ((rb_state9.var_3) != (0x0)) {
            if (!((rb_state9.var_3) == (rb_state9.var_24))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_2) == (0xb18b232)) {
        rb_state17.var_11 = 0x3d9bd9c2;
        rb_state17.var_12 = 0x3202ebc4;
        rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_8) == (0x23434c75)) ? (rb_state17.var_7) : (0x417e9e8));
        rb_state17.var_14 = (rb_state17.var_14) - (rb_state17.var_12);
        rb_state17.var_15 = (rb_state17.var_15) ^ (((0x7b873418) + (rb_state17.var_11)) + (0xbedaf86b));
        rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_13) == (0xfffffffe)) ? (rb_state17.var_14) : (0x3e34cdb1));
        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_15) == (0x77fe0645)) ? (rb_state17.var_16) : (0xc4f3ca48));
        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_17);
    }
    #endif
    (void)jpeg_read_header(&cinfo, TRUE);
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0xd030e4de) + (0x96a595a7));
    #endif
    (void)jpeg_start_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (0xf612fbd5);
    #endif
    row_stride = cinfo.output_width * cinfo.output_components;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0xc16424e6) - (0x768b11b0));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ ((0xef2c2610) ^ (0x32d65b58));
    #endif
    orig = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
    if (orig == NULL) {
        fatal("out of memory");
    }
    row_pointer[0] = orig;
    while (cinfo.output_scanline < cinfo.output_height) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ (1057 < rb_input_size ? (uint32_t)rb_input[1057] : 0xc03397fa);
        if ((rb_state0.var_0) == (0x2a694811)) {
            pthread_mutex_lock(&(rb_state0.lock_25));
            rb_state0.var_5 = 0xb6699320;
            rb_state0.var_6 = (rb_state0.var_6) - (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_5) : (0x38db6305));
            rb_state0.var_7 = (rb_state0.var_7) ^ (210 < rb_input_size ? (uint32_t)rb_input[210] : 0xfdc595ad);
            rb_state0.var_8 = (rb_state0.var_8) + (((((0x3f910c95) + (rb_state0.var_5)) - (rb_state0.var_4)) ^ (rb_state0.var_7)) ^ (rb_state0.var_8));
            rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_6) : (0xc45d02a2));
            rb_state0.var_10 = (rb_state0.var_10) ^ (((0x3602d72) + (rb_state0.var_9)) + (rb_state0.var_7));
            rb_state0.var_11 = (rb_state0.var_11) - (((0xcd96330d) ^ (rb_state0.var_8)) + (0x4b327b8a));
            rb_state0.var_12 = (rb_state0.var_12) ^ (((0xc9ae8db3) - (rb_state0.var_9)) ^ (rb_state0.var_10));
            rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_10) : (0xbf83fde0));
            rb_state0.var_14 = (rb_state0.var_14) - (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_11) : (0xa56368e1));
            rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_12);
            rb_state0.var_16 = (rb_state0.var_16) ^ (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_14) : (0x645270ed));
            rb_state0.var_17 = (rb_state0.var_17) ^ (rb_state0.var_15);
            rb_state0.var_18 = (rb_state0.var_18) - (((((0xb124a9f3) + (rb_state0.var_13)) ^ (rb_state0.var_14)) - (rb_state0.var_16)) - (rb_state0.var_17));
            rb_state0.var_4 = (rb_state0.var_4) ^ (rb_state0.var_18);
            rb_state0.var_3 = rb_state0.var_4;
            pthread_mutex_unlock(&(rb_state0.lock_25));
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_1) == (0x8410f627)) {
            pthread_mutex_lock(&(rb_state1.lock_6));
            rb_state1.var_3 = rb_state1.var_2;
            pthread_mutex_unlock(&(rb_state1.lock_6));
        }
        if ((rb_state1.var_1) == (0x8410f627)) {
            pthread_mutex_lock(&(rb_state1.lock_6));
            if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                racebench_trigger(1);
            }
            pthread_mutex_unlock(&(rb_state1.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (1929 < rb_input_size ? (uint32_t)rb_input[1929] : 0x2bfdc506);
        rb_state4.var_2 = (rb_state4.var_2) ^ (0x5ae3d310);
        if ((rb_state4.var_0) == (0xd0c8b8e7)) {
            rb_state4.var_4 = 0x497a8d68;
            rb_state4.var_5 = 0x108f303e;
            rb_state4.var_6 = (rb_state4.var_6) ^ (((((0x6f19e66e) + (rb_state4.var_5)) - (rb_state4.var_4)) + (rb_state4.var_4)) + (rb_state4.var_3));
            rb_state4.var_7 = (rb_state4.var_7) ^ (((0x77bb629f) + (0xa4c9dc5c)) - (rb_state4.var_5));
            rb_state4.var_8 = (rb_state4.var_8) - (((0x5bdf2bd1) ^ (rb_state4.var_6)) ^ (0xfc90b01));
            rb_state4.var_9 = (rb_state4.var_9) + (((0xcb8ae565) - (rb_state4.var_7)) - (rb_state4.var_6));
            rb_state4.var_10 = (rb_state4.var_10) ^ (((0x4c3790a8) + (rb_state4.var_7)) - (rb_state4.var_8));
            rb_state4.var_11 = (rb_state4.var_11) - (((((0x72202388) ^ (rb_state4.var_10)) - (rb_state4.var_9)) - (rb_state4.var_8)) ^ (0xcc711384));
            rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (0xd900936c);
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_2);
        rb_state6.var_0 = (rb_state6.var_0) ^ (0xe0c1f142);
        if ((rb_state6.var_2) == (0xf167574)) {
            pthread_mutex_lock(&(rb_state6.lock_30));
            rb_state6.var_16 = 0xdec4bf38;
            rb_state6.var_17 = 0x9dfeb948;
            rb_state6.var_18 = 0x1dd80c79;
            rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_10) : (0x51b2d4dd));
            rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_18) : (0x59ae0827));
            rb_state6.var_21 = (rb_state6.var_21) + (((0x2c705447) + (rb_state6.var_13)) - (rb_state6.var_17));
            rb_state6.var_22 = (rb_state6.var_22) - (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_19) : (0xbe5be12c));
            rb_state6.var_23 = (rb_state6.var_23) ^ (rb_state6.var_20);
            rb_state6.var_24 = (rb_state6.var_24) - (rb_state6.var_21);
            rb_state6.var_25 = (rb_state6.var_25) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_22) : (0x327f202));
            rb_state6.var_26 = (rb_state6.var_26) - (((((0x9c58f5db) + (rb_state6.var_23)) + (rb_state6.var_24)) + (rb_state6.var_15)) ^ (rb_state6.var_16));
            rb_state6.var_27 = (rb_state6.var_27) - (rb_state6.var_25);
            rb_state6.var_28 = (rb_state6.var_28) - (((0x57ff5768) - (0xfe23f8c5)) - (rb_state6.var_26));
            rb_state6.var_29 = (rb_state6.var_29) + (((((0x7318e796) - (rb_state6.var_27)) ^ (rb_state6.var_28)) ^ (rb_state6.var_17)) ^ (0x8248b76f));
            rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_29);
            pthread_mutex_unlock(&(rb_state6.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state7.lock_38));
            rb_state7.var_13 = (rb_state7.var_13) + (((((0x2bbd9acb) - (0xac2122aa)) ^ (rb_state7.var_9)) - (rb_state7.var_8)) + (rb_state7.var_10));
            rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x3561feea));
            rb_state7.var_15 = (rb_state7.var_15) - ((0x728d9247) - (rb_state7.var_13));
            rb_state7.var_16 = (rb_state7.var_16) - (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_14) : (0x76409517));
            rb_state7.var_17 = (rb_state7.var_17) + (((((0xc0f4880a) + (rb_state7.var_16)) - (0xc06026c6)) ^ (rb_state7.var_7)) + (rb_state7.var_13));
            rb_state7.var_18 = (rb_state7.var_18) ^ (((0x7f8a2440) + (rb_state7.var_17)) + (rb_state7.var_14));
            rb_state7.var_19 = (rb_state7.var_19) ^ (rb_state7.var_15);
            rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_16) : (0x5d6ad5fb));
            rb_state7.var_21 = (rb_state7.var_21) ^ (((((0x221e5dce) - (rb_state7.var_18)) - (0xf857af79)) ^ (rb_state7.var_17)) + (0x9bb57e2c));
            rb_state7.var_22 = (rb_state7.var_22) ^ (((((0xe5de03cf) + (rb_state7.var_19)) ^ (0xb8da93bc)) - (rb_state7.var_20)) - (rb_state7.var_19));
            rb_state7.var_23 = (rb_state7.var_23) ^ (rb_state7.var_21);
            rb_state7.var_24 = (rb_state7.var_24) + (((0x45d71e0e) - (rb_state7.var_20)) + (rb_state7.var_22));
            rb_state7.var_25 = (rb_state7.var_25) - (((((0xd2cd68f9) ^ (rb_state7.var_24)) ^ (rb_state7.var_23)) ^ (rb_state7.var_22)) - (rb_state7.var_21));
            rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_25);
            rb_state7.var_3 = rb_state7.var_12;
            pthread_mutex_unlock(&(rb_state7.lock_38));
        }
        if ((rb_state7.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state7.lock_38));
            rb_state7.var_27 = (rb_state7.var_27) ^ (0xde0976a3);
            rb_state7.var_28 = (rb_state7.var_28) - (((0x49e11a24) - (0x3fdbd704)) - (rb_state7.var_23));
            rb_state7.var_29 = (rb_state7.var_29) + (0xeae05063);
            rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_24) : (0xf7ed96bf));
            rb_state7.var_31 = (rb_state7.var_31) + (((((0x8fea10af) ^ (rb_state7.var_27)) ^ (rb_state7.var_27)) + (rb_state7.var_25)) ^ (0x580717dc));
            rb_state7.var_32 = (rb_state7.var_32) - (((((0x80852b51) + (0xbc7ad9f4)) + (rb_state7.var_28)) ^ (rb_state7.var_28)) ^ (rb_state7.var_29));
            rb_state7.var_33 = (rb_state7.var_33) - (((((0x4eea44d1) - (rb_state7.var_30)) + (0x8af34d97)) ^ (rb_state7.var_31)) + (0x27d12adf));
            rb_state7.var_34 = (rb_state7.var_34) + (rb_state7.var_32);
            rb_state7.var_35 = (rb_state7.var_35) + (rb_state7.var_33);
            rb_state7.var_36 = (rb_state7.var_36) - (((rb_state7.var_29) == (0x0)) ? (rb_state7.var_34) : (0xf70337c4));
            rb_state7.var_37 = (rb_state7.var_37) ^ (((rb_state7.var_35) == (0x0)) ? (rb_state7.var_36) : (0x6a99de76));
            rb_state7.var_26 = (rb_state7.var_26) ^ (rb_state7.var_37);
            rb_state7.var_3 = rb_state7.var_26;
            pthread_mutex_unlock(&(rb_state7.lock_38));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) - ((0xfe8bd65a) - (rb_state8.var_2));
        rb_state8.var_0 = (rb_state8.var_0) ^ (693 < rb_input_size ? (uint32_t)rb_input[693] : 0xcb288baf);
        if ((rb_state8.var_1) == (0x8ec5f105)) {
            if ((rb_state8.var_3) != (0x0)) {
                rb_state8.var_42 = !((rb_state8.var_3) == (rb_state8.var_25));
            }
        }
        rb_state8.var_23 = (rb_state8.var_23) - (368 < rb_input_size ? (uint32_t)rb_input[368] : 0x69217bbf);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0xffffff29)) ? (rb_state10.var_1) : (0xd574d073));
        rb_state10.var_1 = (rb_state10.var_1) ^ (1066 < rb_input_size ? (uint32_t)rb_input[1066] : 0x2feccd8d);
        if ((rb_state10.var_2) == (0x7b405e41)) {
            pthread_mutex_lock(&(rb_state10.lock_31));
            rb_state10.var_5 = 0x8a878bd5;
            rb_state10.var_6 = 0x24b3cf71;
            rb_state10.var_7 = (rb_state10.var_7) ^ (((((0xdbfac199) + (rb_state10.var_3)) - (rb_state10.var_4)) + (rb_state10.var_6)) ^ (0xc2d2ebb));
            rb_state10.var_8 = (rb_state10.var_8) ^ (0x6197787b);
            rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_5) : (0xd75661b0));
            rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_7) : (0xa064faa0));
            rb_state10.var_11 = (rb_state10.var_11) ^ (((0x61d307ec) - (rb_state10.var_8)) ^ (0x72e600a3));
            rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_9);
            rb_state10.var_13 = (rb_state10.var_13) + (((((0x1af4dd5d) - (rb_state10.var_7)) - (rb_state10.var_8)) ^ (rb_state10.var_10)) - (rb_state10.var_11));
            rb_state10.var_14 = (rb_state10.var_14) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x33486622));
            rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_14);
            rb_state10.var_3 = rb_state10.var_4;
            pthread_mutex_unlock(&(rb_state10.lock_31));
        }
        if ((rb_state10.var_2) == (0x7b405e41)) {
            pthread_mutex_lock(&(rb_state10.lock_31));
            if (!((rb_state10.var_3) == (rb_state10.var_4))) {
                racebench_trigger(10);
            }
            pthread_mutex_unlock(&(rb_state10.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (1063 < rb_input_size ? (uint32_t)rb_input[1063] : 0x406f3a59);
        if ((rb_state13.var_1) == (0x4529e19f)) {
            if ((rb_state13.var_2) != (0x0)) {
                if (!((rb_state13.var_2) == (rb_state13.var_18))) {
                    racebench_trigger(13);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ (0x68bcdb0a);
        rb_state16.var_1 = (rb_state16.var_1) + ((0x9488bd9d) ^ (rb_state16.var_2));
        rb_state16.var_0 = (rb_state16.var_0) - (0xbe79a189);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ (1467 < rb_input_size ? (uint32_t)rb_input[1467] : 0x75bc8724);
        rb_state18.var_1 = (rb_state18.var_1) + (1765 < rb_input_size ? (uint32_t)rb_input[1765] : 0xadd54d17);
        rb_state18.var_2 = (rb_state18.var_2) + ((0xaf354d4e) ^ (0x6d3659d0));
        if ((rb_state18.var_0) == (0x1cbb6a9)) {
            rb_state18.var_3 = rb_state18.var_1;
        }
        if ((rb_state18.var_0) == (0x1cbb6a9)) {
            if (!((rb_state18.var_1) == (rb_state18.var_3))) {
                racebench_trigger(18);
            }
        }
        #endif
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_0);
        if ((rb_state0.var_0) == (0x2a694811)) {
            if (!((rb_state0.var_3) == (rb_state0.var_4))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (0xaeb6b6d7);
        rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_1);
        rb_state4.var_2 = (rb_state4.var_2) ^ (((rb_state4.var_1) == (0xe1)) ? (rb_state4.var_0) : (0xa02403fc));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (0x7c8cfb75);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) ^ (0xf2d370be);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ ((0xc31c8f6b) ^ (0xebad68d));
        rb_state8.var_27 = (rb_state8.var_27) ^ (rb_state8.var_25);
        if ((rb_state8.var_26) == (0x287356f9)) {
            rb_state8.var_43 = rb_state8.var_28;
        }
        if ((rb_state8.var_26) == (0x287356f9)) {
            rb_state8.var_53 = !((rb_state8.var_28) == (rb_state8.var_43));
        }
        rb_state8.var_35 = (rb_state8.var_35) + (0x6963fefc);
        if ((rb_state8.var_37) == (0x44fdec46)) {
            rb_state8.var_56 = 0x1;
        }
        if ((rb_state8.var_37) == (0x44fdec46)) {
            pthread_mutex_lock(&(rb_state8.lock_68));
            rb_state8.var_57 = 0xf789c9af;
            rb_state8.var_58 = (rb_state8.var_58) + ((0xa0e74a56) - (0x43b22b42));
            rb_state8.var_59 = (rb_state8.var_59) ^ (((0x2f8fd6a6) ^ (0xa34e03f5)) - (rb_state8.var_39));
            rb_state8.var_60 = (rb_state8.var_60) + (((0xc00d3ee1) + (0x96f62044)) - (rb_state8.var_57));
            rb_state8.var_61 = (rb_state8.var_61) ^ (((rb_state8.var_40) == (0x0)) ? (rb_state8.var_38) : (0x7ce2a19a));
            rb_state8.var_62 = (rb_state8.var_62) - (rb_state8.var_58);
            rb_state8.var_63 = (rb_state8.var_63) ^ (((0xaef1b076) - (0xf5b27206)) + (rb_state8.var_59));
            rb_state8.var_64 = (rb_state8.var_64) + (((((0xeadd7ad6) - (rb_state8.var_60)) ^ (rb_state8.var_43)) ^ (rb_state8.var_61)) - (rb_state8.var_42));
            rb_state8.var_65 = (rb_state8.var_65) ^ (rb_state8.var_62);
            rb_state8.var_66 = (rb_state8.var_66) ^ (((rb_state8.var_63) == (0x0)) ? (rb_state8.var_64) : (0xcf074841));
            rb_state8.var_67 = (rb_state8.var_67) - (((((0xa56864f5) - (0xb792552b)) ^ (rb_state8.var_65)) ^ (rb_state8.var_66)) ^ (rb_state8.var_44));
            rb_state8.var_55 = (rb_state8.var_55) - (rb_state8.var_67);
            rb_state8.var_54 = rb_state8.var_55;
            pthread_mutex_unlock(&(rb_state8.lock_68));
        }
        rb_state8.var_45 = (rb_state8.var_45) ^ ((0xbaeb5ef3) + (0x41434ebe));
        if ((rb_state8.var_46) == (0x65aca534)) {
            pthread_mutex_lock(&(rb_state8.lock_103));
            if ((rb_state8.var_70) != (0x0)) {
                if (!((rb_state8.var_70) == (rb_state8.var_89))) {
                    racebench_trigger(8);
                }
            }
            pthread_mutex_unlock(&(rb_state8.lock_103));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) ^ ((0x7b405e41) ^ (rb_state10.var_2));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + ((0x5474793c) - (0xf44bac7f));
        rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_1);
        rb_state13.var_1 = (rb_state13.var_1) - (0x8bf77b19);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - ((0x6dbbacb9) ^ (0xd8c9a1af));
        if ((rb_state16.var_1) == (0xb84d7799)) {
            if ((rb_state16.var_3) != (0x0)) {
                if (!((rb_state16.var_3) == (rb_state16.var_22))) {
                    racebench_trigger(16);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - (710 < rb_input_size ? (uint32_t)rb_input[710] : 0x4cc8b89d);
        rb_state18.var_2 = (rb_state18.var_2) + (0x7818e17b);
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