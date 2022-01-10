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
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x61e1eadc)) {
        rb_state0.var_2 = rb_state0.var_1;
    }
    if ((rb_state0.var_0) == (0x61e1eadc)) {
        rb_state0.var_13 = !((rb_state0.var_1) == (rb_state0.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_17 = 0x7400d674;
        rb_state2.var_18 = 0x3c08dd5d;
        rb_state2.var_19 = (rb_state2.var_19) - (((0xf90c121e) - (rb_state2.var_8)) ^ (rb_state2.var_9));
        rb_state2.var_20 = (rb_state2.var_20) + (rb_state2.var_18);
        rb_state2.var_21 = (rb_state2.var_21) - (((rb_state2.var_10) == (0x5deef000)) ? (rb_state2.var_17) : (0x20b4515a));
        rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_19) == (0xbccd780c)) ? (rb_state2.var_20) : (0xb66f9c1b));
        rb_state2.var_23 = (rb_state2.var_23) - (((rb_state2.var_21) == (0x8bff298c)) ? (rb_state2.var_22) : (0xc1d6e57a));
        rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - ((0xb0f8cf10) ^ (rb_state9.var_0));
    #endif
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        rb_state17.var_3 = 0x1522cb90;
        rb_state17.var_4 = (rb_state17.var_4) ^ (((((0xc614a279) ^ (rb_state17.var_4)) ^ (0xae6051a)) ^ (rb_state17.var_2)) - (rb_state17.var_3));
        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_4);
        rb_state17.var_1 = rb_state17.var_2;
    }
    if ((rb_state17.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_23));
        rb_state17.var_6 = 0x4165391d;
        rb_state17.var_7 = (rb_state17.var_7) - (rb_state17.var_7);
        rb_state17.var_8 = (rb_state17.var_8) - (rb_state17.var_5);
        rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_8) : (0x9d32f06c));
        rb_state17.var_10 = (rb_state17.var_10) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_6) : (0x60303a3c));
        rb_state17.var_11 = (rb_state17.var_11) ^ (((((0x13aaca94) ^ (rb_state17.var_8)) + (rb_state17.var_11)) - (0x87f4b0ce)) - (rb_state17.var_7));
        rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_9);
        rb_state17.var_13 = (rb_state17.var_13) + (rb_state17.var_10);
        rb_state17.var_14 = (rb_state17.var_14) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_11) : (0x3b36b6f4));
        rb_state17.var_15 = (rb_state17.var_15) - (((((0xa38ad642) + (rb_state17.var_12)) ^ (0x222f1354)) + (rb_state17.var_13)) - (0xb4beb945));
        rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_13) == (0x4165391d)) ? (rb_state17.var_14) : (0x4bdb4007));
        rb_state17.var_17 = (rb_state17.var_17) ^ (((rb_state17.var_14) == (0x8bb619c6)) ? (rb_state17.var_15) : (0x6dcd46ff));
        rb_state17.var_18 = (rb_state17.var_18) - (((0x56030c19) ^ (rb_state17.var_15)) + (rb_state17.var_16));
        rb_state17.var_19 = (rb_state17.var_19) ^ (rb_state17.var_17);
        rb_state17.var_20 = (rb_state17.var_20) ^ (((0x620f3953) - (rb_state17.var_18)) + (rb_state17.var_16));
        rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_17) == (0xf1b3bb12)) ? (rb_state17.var_19) : (0x1c279861));
        rb_state17.var_22 = (rb_state17.var_22) ^ (((rb_state17.var_20) == (0x212c23ea)) ? (rb_state17.var_21) : (0x166f77e9));
        rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_22);
        rb_state17.var_1 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_23));
    }
    #endif
    jpeg_stdio_src(&cinfo, infile);
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    (void)jpeg_read_header(&cinfo, TRUE);
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    #endif
    (void)jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    orig = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
    if (orig == NULL) {
        fatal("out of memory");
    }
    row_pointer[0] = orig;
    while (cinfo.output_scanline < cinfo.output_height) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_31);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xd029b02e));
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x9e0e61e0)) {
            rb_state9.var_14 = 0xff0a2f9d;
            rb_state9.var_15 = (rb_state9.var_15) ^ (0xb569a24a);
            rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_14) : (0xd0298dc2));
            rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_6);
            rb_state9.var_18 = (rb_state9.var_18) ^ (((0xdba5b981) + (rb_state9.var_7)) + (rb_state9.var_9));
            rb_state9.var_19 = (rb_state9.var_19) + (rb_state9.var_15);
            rb_state9.var_20 = (rb_state9.var_20) + (((0xbe6f3466) ^ (rb_state9.var_16)) - (rb_state9.var_10));
            rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_18) : (0xd5fe5c5c));
            rb_state9.var_22 = (rb_state9.var_22) - (((0x17a1ddd3) + (rb_state9.var_19)) ^ (rb_state9.var_11));
            rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_20);
            rb_state9.var_24 = (rb_state9.var_24) + (((((0x629d98b4) - (rb_state9.var_22)) ^ (rb_state9.var_12)) ^ (rb_state9.var_21)) ^ (0xd6a7dd8a));
            rb_state9.var_25 = (rb_state9.var_25) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_23) : (0x7c99e705));
            rb_state9.var_26 = (rb_state9.var_26) + (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0x3e5ce2ef));
            rb_state9.var_13 = (rb_state9.var_13) ^ (rb_state9.var_26);
            rb_state9.var_1 = rb_state9.var_13;
        }
        if ((rb_state9.var_0) == (0x9e0e61e0)) {
            pthread_mutex_lock(&(rb_state9.lock_43));
            rb_state9.var_28 = 0x949e70ef;
            rb_state9.var_29 = 0x7c2f2a92;
            rb_state9.var_30 = (rb_state9.var_30) - (rb_state9.var_15);
            rb_state9.var_31 = (rb_state9.var_31) - (rb_state9.var_28);
            rb_state9.var_32 = (rb_state9.var_32) ^ (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_14) : (0xa48342b6));
            rb_state9.var_33 = (rb_state9.var_33) + (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_29) : (0x5ba90deb));
            rb_state9.var_34 = (rb_state9.var_34) + (((0x4f368b0) + (rb_state9.var_18)) + (rb_state9.var_30));
            rb_state9.var_35 = (rb_state9.var_35) - (((rb_state9.var_31) == (0x0)) ? (rb_state9.var_32) : (0xbbf74557));
            rb_state9.var_36 = (rb_state9.var_36) - (((0x3ff17fa6) ^ (rb_state9.var_19)) ^ (rb_state9.var_33));
            rb_state9.var_37 = (rb_state9.var_37) ^ (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_34) : (0xc545c7b2));
            rb_state9.var_38 = (rb_state9.var_38) ^ (rb_state9.var_35);
            rb_state9.var_39 = (rb_state9.var_39) + (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_36) : (0x64b8873d));
            rb_state9.var_40 = (rb_state9.var_40) ^ (rb_state9.var_37);
            rb_state9.var_41 = (rb_state9.var_41) + (((rb_state9.var_38) == (0x0)) ? (rb_state9.var_39) : (0x5e3836df));
            rb_state9.var_42 = (rb_state9.var_42) + (((((0xf08bbc25) ^ (0x51468acc)) ^ (rb_state9.var_41)) + (rb_state9.var_22)) - (rb_state9.var_40));
            rb_state9.var_27 = (rb_state9.var_27) + (rb_state9.var_42);
            rb_state9.var_1 = rb_state9.var_27;
            pthread_mutex_unlock(&(rb_state9.lock_43));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - ((0x64e1a0f7) + (rb_state12.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0x8b250341) + (rb_state18.var_0));
        rb_state18.var_5 = (rb_state18.var_5) + ((0xaf797e89) ^ (rb_state18.var_6));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (1828 < rb_input_size ? (uint32_t)rb_input[1828] : 0x4619b0f6);
        if ((rb_state19.var_0) == (0xa1)) {
            rb_state19.var_13 = (rb_state19.var_13) ^ ((0xf534c15c) ^ (rb_state19.var_8));
            rb_state19.var_14 = (rb_state19.var_14) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_7) : (0xca05760e));
            rb_state19.var_15 = (rb_state19.var_15) + (0xe6421d4e);
            rb_state19.var_16 = (rb_state19.var_16) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_13) : (0xe024d3b8));
            rb_state19.var_17 = (rb_state19.var_17) ^ (rb_state19.var_14);
            rb_state19.var_18 = (rb_state19.var_18) + (rb_state19.var_15);
            rb_state19.var_19 = (rb_state19.var_19) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_17) : (0x3ca47343));
            rb_state19.var_20 = (rb_state19.var_20) ^ (rb_state19.var_18);
            rb_state19.var_21 = (rb_state19.var_21) + (((0x8c86114e) - (rb_state19.var_19)) - (0x8dc8cf77));
            rb_state19.var_22 = (rb_state19.var_22) ^ (((((0xb60fb496) ^ (0xd109e37f)) ^ (rb_state19.var_21)) ^ (rb_state19.var_9)) - (rb_state19.var_20));
            rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_22);
            rb_state19.var_1 = rb_state19.var_12;
        }
        if ((rb_state19.var_0) == (0xa1)) {
            pthread_mutex_lock(&(rb_state19.lock_43));
            rb_state19.var_24 = 0xf1d524ed;
            rb_state19.var_25 = (rb_state19.var_25) ^ (rb_state19.var_24);
            rb_state19.var_26 = (rb_state19.var_26) ^ (0x98952908);
            rb_state19.var_27 = (rb_state19.var_27) + (rb_state19.var_10);
            rb_state19.var_28 = (rb_state19.var_28) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_11) : (0x1aa15b89));
            rb_state19.var_29 = (rb_state19.var_29) - (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_25) : (0x70cbb4cf));
            rb_state19.var_30 = (rb_state19.var_30) ^ (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_27) : (0x7962d8ae));
            rb_state19.var_31 = (rb_state19.var_31) + (((0xdc0e9153) - (0x5f24720c)) ^ (rb_state19.var_28));
            rb_state19.var_32 = (rb_state19.var_32) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_29) : (0xf301a2a4));
            rb_state19.var_33 = (rb_state19.var_33) - (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_30) : (0xd5ec2ccb));
            rb_state19.var_34 = (rb_state19.var_34) - (((0x67ed4468) - (0xc780b848)) + (rb_state19.var_31));
            rb_state19.var_35 = (rb_state19.var_35) ^ (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_32) : (0xcb9b8be7));
            rb_state19.var_36 = (rb_state19.var_36) - (((0xb6e33b25) - (rb_state19.var_33)) + (0x934c592f));
            rb_state19.var_37 = (rb_state19.var_37) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_34) : (0x6e713490));
            rb_state19.var_38 = (rb_state19.var_38) - (((((0x883d2fcf) - (rb_state19.var_36)) - (rb_state19.var_35)) - (0x5da0101e)) + (0xdd4ecfc4));
            rb_state19.var_39 = (rb_state19.var_39) - (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_37) : (0x21ff75e3));
            rb_state19.var_40 = (rb_state19.var_40) ^ (((0x1e54c2f) + (rb_state19.var_38)) ^ (rb_state19.var_19));
            rb_state19.var_41 = (rb_state19.var_41) ^ (((rb_state19.var_20) == (0x0)) ? (rb_state19.var_39) : (0xfe9f7cf2));
            rb_state19.var_42 = (rb_state19.var_42) ^ (((((0xaa1d2e33) + (0xadf488d4)) - (rb_state19.var_21)) ^ (rb_state19.var_40)) - (rb_state19.var_41));
            rb_state19.var_23 = (rb_state19.var_23) - (rb_state19.var_42);
            rb_state19.var_1 = rb_state19.var_23;
            pthread_mutex_unlock(&(rb_state19.lock_43));
        }
        #endif
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_7);
        if ((rb_state0.var_8) == (0x4496ad41)) {
            rb_state0.var_27 = (rb_state0.var_27) - (rb_state0.var_17);
            rb_state0.var_28 = (rb_state0.var_28) - (0x4f132dcb);
            rb_state0.var_29 = (rb_state0.var_29) ^ (rb_state0.var_19);
            rb_state0.var_30 = (rb_state0.var_30) + ((0xa1515cf8) ^ (rb_state0.var_20));
            rb_state0.var_31 = (rb_state0.var_31) ^ (((0x6847d5b2) + (rb_state0.var_21)) - (rb_state0.var_18));
            rb_state0.var_32 = (rb_state0.var_32) + (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_27) : (0xa4dcaac8));
            rb_state0.var_33 = (rb_state0.var_33) ^ (rb_state0.var_28);
            rb_state0.var_34 = (rb_state0.var_34) - (((0xf6441e14) ^ (0x2529030e)) + (rb_state0.var_29));
            rb_state0.var_35 = (rb_state0.var_35) ^ (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_30) : (0x1a0b252));
            rb_state0.var_36 = (rb_state0.var_36) - (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_31) : (0x3b789b1e));
            rb_state0.var_37 = (rb_state0.var_37) + (rb_state0.var_32);
            rb_state0.var_38 = (rb_state0.var_38) + (rb_state0.var_33);
            rb_state0.var_39 = (rb_state0.var_39) - (((rb_state0.var_25) == (0x0)) ? (rb_state0.var_34) : (0x7dfcb934));
            rb_state0.var_40 = (rb_state0.var_40) ^ (((rb_state0.var_35) == (0x0)) ? (rb_state0.var_36) : (0xfd04a90d));
            rb_state0.var_41 = (rb_state0.var_41) ^ (rb_state0.var_37);
            rb_state0.var_42 = (rb_state0.var_42) - (((rb_state0.var_26) == (0x0)) ? (rb_state0.var_38) : (0x6730f505));
            rb_state0.var_43 = (rb_state0.var_43) - (rb_state0.var_39);
            rb_state0.var_44 = (rb_state0.var_44) - (rb_state0.var_40);
            rb_state0.var_45 = (rb_state0.var_45) - (((((0xa6609258) ^ (rb_state0.var_27)) - (0x80bc8685)) - (rb_state0.var_42)) - (rb_state0.var_41));
            rb_state0.var_46 = (rb_state0.var_46) - (((((0x50284f0f) - (rb_state0.var_44)) - (rb_state0.var_28)) + (rb_state0.var_43)) ^ (0x3ca9aa7));
            rb_state0.var_47 = (rb_state0.var_47) - (((rb_state0.var_29) == (0x0)) ? (rb_state0.var_45) : (0x9a09482));
            rb_state0.var_48 = (rb_state0.var_48) + (((((0x2f86e952) ^ (0x435b858d)) ^ (rb_state0.var_46)) - (rb_state0.var_47)) - (0xbfc68bee));
            rb_state0.var_14 = (rb_state0.var_14) ^ (rb_state0.var_48);
        }
        if ((rb_state0.var_32) == (0x8208ab78)) {
            pthread_mutex_lock(&(rb_state0.lock_58));
            rb_state0.var_53 = 0xd48381be;
            rb_state0.var_54 = 0x94156498;
            rb_state0.var_55 = (rb_state0.var_55) - (((((0x283612eb) ^ (rb_state0.var_34)) - (rb_state0.var_35)) - (rb_state0.var_54)) + (rb_state0.var_53));
            rb_state0.var_56 = (rb_state0.var_56) + (rb_state0.var_33);
            rb_state0.var_57 = (rb_state0.var_57) ^ (((((0x88811e13) - (rb_state0.var_56)) - (0xbb2acb24)) + (rb_state0.var_55)) + (0xc2fdb26d));
            rb_state0.var_51 = (rb_state0.var_51) + (rb_state0.var_57);
            rb_state0.var_52 = rb_state0.var_51;
            pthread_mutex_unlock(&(rb_state0.lock_58));
        }
        if ((rb_state0.var_37) == (0x822ade0)) {
            pthread_mutex_lock(&(rb_state0.lock_71));
            rb_state0.var_60 = rb_state0.var_38;
            pthread_mutex_unlock(&(rb_state0.lock_71));
        }
        if ((rb_state0.var_37) == (0x822ade0)) {
            if (!((rb_state0.var_38) == (rb_state0.var_60))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0xd8b78172)) {
            pthread_mutex_lock(&(rb_state1.lock_18));
            rb_state1.var_16 = 0x5ca7de97;
            rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_15) : (0x89b7b0c5));
            rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_17);
            pthread_mutex_unlock(&(rb_state1.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (1288 < rb_input_size ? (uint32_t)rb_input[1288] : 0x70674471);
        if ((rb_state6.var_0) == (0x1cfc4097)) {
            pthread_mutex_lock(&(rb_state6.lock_13));
            rb_state6.var_3 = 0xf1344167;
            rb_state6.var_4 = 0x66fc082;
            rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_1);
            rb_state6.var_6 = (rb_state6.var_6) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_3) : (0x427c221e));
            rb_state6.var_7 = (rb_state6.var_7) - (((((0xa0971f02) ^ (rb_state6.var_5)) ^ (rb_state6.var_2)) + (rb_state6.var_6)) + (0xbdad27c9));
            rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_7);
            rb_state6.var_1 = rb_state6.var_2;
            pthread_mutex_unlock(&(rb_state6.lock_13));
        }
        if ((rb_state6.var_0) == (0x1cfc4097)) {
            if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
        if ((rb_state8.var_0) == (0x0)) {
            rb_state8.var_7 = 0x7806dc6d;
            rb_state8.var_8 = 0xdfffe2da;
            rb_state8.var_9 = (rb_state8.var_9) ^ (((0xa29b18f) ^ (0x49d3d876)) - (rb_state8.var_3));
            rb_state8.var_10 = (rb_state8.var_10) ^ (((0x64d6c137) + (rb_state8.var_8)) ^ (rb_state8.var_4));
            rb_state8.var_11 = (rb_state8.var_11) ^ (((0x5d832027) + (rb_state8.var_5)) - (rb_state8.var_7));
            rb_state8.var_12 = (rb_state8.var_12) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x6ab8792d));
            rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0xb7ddf97a));
            rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_0) == (0x9b1e5f09)) {
            rb_state12.var_14 = 0x4903bf71;
            rb_state12.var_15 = 0xba6b110c;
            rb_state12.var_16 = (rb_state12.var_16) + (0x25ffb5b6);
            rb_state12.var_17 = (rb_state12.var_17) - (((((0x845dce46) ^ (rb_state12.var_10)) - (rb_state12.var_15)) - (rb_state12.var_9)) ^ (rb_state12.var_14));
            rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_16) : (0x92d4467d));
            rb_state12.var_19 = (rb_state12.var_19) - (rb_state12.var_17);
            rb_state12.var_20 = (rb_state12.var_20) ^ (((((0xbe0bf1c) ^ (rb_state12.var_19)) + (rb_state12.var_11)) ^ (rb_state12.var_12)) - (rb_state12.var_18));
            rb_state12.var_13 = (rb_state12.var_13) - (rb_state12.var_20);
            rb_state12.var_1 = rb_state12.var_13;
        }
        if ((rb_state12.var_0) == (0x9b1e5f09)) {
            pthread_mutex_lock(&(rb_state12.lock_26));
            rb_state12.var_22 = 0xb600ef38;
            rb_state12.var_23 = 0xa0773a72;
            rb_state12.var_24 = (rb_state12.var_24) ^ (((rb_state12.var_23) == (0x0)) ? (rb_state12.var_13) : (0x38b74fff));
            rb_state12.var_25 = (rb_state12.var_25) + (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_24) : (0x7f357eb4));
            rb_state12.var_21 = (rb_state12.var_21) ^ (rb_state12.var_25);
            rb_state12.var_1 = rb_state12.var_21;
            pthread_mutex_unlock(&(rb_state12.lock_26));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x74dafcbf)) {
            rb_state18.var_8 = !((rb_state18.var_2) == (rb_state18.var_1));
        }
        if ((rb_state18.var_8) == (0x0)) {
            if ((rb_state18.var_9) != (0x0)) {
                if (!((rb_state18.var_9) == (rb_state18.var_44))) {
                    racebench_trigger(18);
                }
            }
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