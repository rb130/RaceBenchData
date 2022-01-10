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
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_4) == (0xf5)) {
        rb_state5.var_24 = 0x9412d072;
        rb_state5.var_25 = (rb_state5.var_25) + (((0x96bd90c4) ^ (rb_state5.var_24)) - (0x91898f6c));
        rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_17) == (0x1b3abd06)) ? (rb_state5.var_16) : (0xb0f73867));
        rb_state5.var_27 = (rb_state5.var_27) ^ (1278 < rb_input_size ? (uint32_t)rb_input[1278] : 0x85c7e51d);
        rb_state5.var_28 = (rb_state5.var_28) - (((rb_state5.var_18) == (0x46d30fae)) ? (rb_state5.var_15) : (0xa520dd01));
        rb_state5.var_29 = (rb_state5.var_29) ^ (((0xdc6689cf) - (rb_state5.var_19)) - (rb_state5.var_25));
        rb_state5.var_30 = (rb_state5.var_30) + (rb_state5.var_26);
        rb_state5.var_31 = (rb_state5.var_31) - (rb_state5.var_27);
        rb_state5.var_32 = (rb_state5.var_32) ^ (((rb_state5.var_20) == (0x5406e882)) ? (rb_state5.var_28) : (0xb1af2bbb));
        rb_state5.var_33 = (rb_state5.var_33) + (rb_state5.var_29);
        rb_state5.var_34 = (rb_state5.var_34) + (rb_state5.var_30);
        rb_state5.var_35 = (rb_state5.var_35) ^ (((0x2546d6d7) + (rb_state5.var_31)) ^ (rb_state5.var_21));
        rb_state5.var_36 = (rb_state5.var_36) ^ (((rb_state5.var_22) == (0x887644ad)) ? (rb_state5.var_32) : (0xe3c4716e));
        rb_state5.var_37 = (rb_state5.var_37) - (((rb_state5.var_33) == (0x3cbd1798)) ? (rb_state5.var_34) : (0xc01b7a61));
        rb_state5.var_38 = (rb_state5.var_38) ^ (((((0x4a8c6aa4) ^ (0x1523200f)) ^ (rb_state5.var_35)) - (rb_state5.var_36)) + (0xccc490cc));
        rb_state5.var_39 = (rb_state5.var_39) - (((0xcec35829) - (rb_state5.var_37)) + (0xa3092f0f));
        rb_state5.var_40 = (rb_state5.var_40) + (((((0x99a25bc9) + (rb_state5.var_38)) - (rb_state5.var_39)) ^ (0xd4a4d37c)) + (rb_state5.var_23));
        rb_state5.var_5 = (rb_state5.var_5) - (rb_state5.var_40);
    }
    #endif
    cinfo.err = jpeg_std_error(&jerr);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
    rb_state0.var_2 = (rb_state0.var_2) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_2) : (0xa7cc5ad1));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_3) == (0xe6675a41)) {
        if ((rb_state9.var_4) != (0x0)) {
            if (!((rb_state9.var_4) == (rb_state9.var_27))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + ((0xf0ecfbc1) - (0x6833b801));
    #endif
    jpeg_create_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0x5fe3ad72)) {
        pthread_mutex_lock(&(rb_state15.lock_43));
        rb_state15.var_24 = (rb_state15.var_24) ^ (0xc2f688e9);
        rb_state15.var_25 = (rb_state15.var_25) - (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_16) : (0xdfc32bae));
        rb_state15.var_26 = (rb_state15.var_26) - (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_24) : (0x39e5cbf6));
        rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_18) : (0xa7cc50aa));
        rb_state15.var_28 = (rb_state15.var_28) ^ (((((0x85db86bb) ^ (rb_state15.var_26)) - (rb_state15.var_20)) + (rb_state15.var_25)) ^ (0x62b5168b));
        rb_state15.var_29 = (rb_state15.var_29) + (((rb_state15.var_21) == (0x0)) ? (rb_state15.var_27) : (0xb2acd93a));
        rb_state15.var_30 = (rb_state15.var_30) ^ (((0x1a390e41) - (rb_state15.var_28)) ^ (rb_state15.var_22));
        rb_state15.var_31 = (rb_state15.var_31) ^ (rb_state15.var_29);
        rb_state15.var_32 = (rb_state15.var_32) + (rb_state15.var_30);
        rb_state15.var_33 = (rb_state15.var_33) - (((rb_state15.var_31) == (0x0)) ? (rb_state15.var_32) : (0x9bbbd7a5));
        rb_state15.var_23 = (rb_state15.var_23) ^ (rb_state15.var_33);
        rb_state15.var_4 = rb_state15.var_23;
        pthread_mutex_unlock(&(rb_state15.lock_43));
    }
    if ((rb_state15.var_3) == (0x5fe3ad72)) {
        pthread_mutex_lock(&(rb_state15.lock_43));
        rb_state15.var_35 = 0x1851f295;
        rb_state15.var_36 = 0xad2b4617;
        rb_state15.var_37 = 0x5059d9bc;
        rb_state15.var_38 = (rb_state15.var_38) ^ (((0xa1fec7dc) ^ (0xdd14160d)) ^ (rb_state15.var_35));
        rb_state15.var_39 = (rb_state15.var_39) ^ (rb_state15.var_36);
        rb_state15.var_40 = (rb_state15.var_40) + (((0x9618a142) ^ (0xc57b192)) ^ (rb_state15.var_37));
        rb_state15.var_41 = (rb_state15.var_41) - (((rb_state15.var_38) == (0x64bb2344)) ? (rb_state15.var_39) : (0xeb6d57c));
        rb_state15.var_42 = (rb_state15.var_42) + (((((0x2547429a) + (rb_state15.var_41)) - (rb_state15.var_40)) ^ (0xc8e91057)) - (rb_state15.var_23));
        rb_state15.var_34 = (rb_state15.var_34) ^ (rb_state15.var_42);
        rb_state15.var_4 = rb_state15.var_34;
        pthread_mutex_unlock(&(rb_state15.lock_43));
    }
    #endif
    jpeg_stdio_src(&cinfo, infile);
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (307 < rb_input_size ? (uint32_t)rb_input[307] : 0xdd57c27f);
    #endif
    orig = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
    if (orig == NULL) {
        fatal("out of memory");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (0x5f71468e);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_2) : (0xcef1608b));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x25e88ad)) ? (rb_state8.var_0) : (0x6a62a3ee));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0xd75fe485);
    rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_0);
    #endif
    row_pointer[0] = orig;
    while (cinfo.output_scanline < cinfo.output_height) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_3) : (0x3276d2b2));
        rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_0) == (0x10bf185)) ? (rb_state0.var_3) : (0xdebd397d));
        rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_2);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_0);
        rb_state1.var_3 = (rb_state1.var_3) + (0xc7b35de9);
        rb_state1.var_3 = (rb_state1.var_3) - (560 < rb_input_size ? (uint32_t)rb_input[560] : 0x460229ab);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + ((0x64697aef) + (rb_state8.var_2));
        rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_3);
        rb_state8.var_0 = (rb_state8.var_0) - (0xb1b2ea2);
        rb_state8.var_2 = (rb_state8.var_2) + ((0x5b7fad64) + (rb_state8.var_3));
        if ((rb_state8.var_2) == (0x652137ad)) {
            rb_state8.var_6 = 0x1;
        }
        if ((rb_state8.var_2) == (0x652137ad)) {
            pthread_mutex_lock(&(rb_state8.lock_14));
            rb_state8.var_7 = 0xb01d26a8;
            rb_state8.var_8 = (rb_state8.var_8) + (((((0xde09d7f0) ^ (0x5486c411)) - (rb_state8.var_5)) + (rb_state8.var_7)) ^ (rb_state8.var_6));
            rb_state8.var_9 = (rb_state8.var_9) ^ (((0x3a784d72) ^ (rb_state8.var_7)) - (rb_state8.var_4));
            rb_state8.var_10 = (rb_state8.var_10) - (((0x7f9c1656) - (0x58fbe09c)) - (rb_state8.var_8));
            rb_state8.var_11 = (rb_state8.var_11) - (rb_state8.var_9);
            rb_state8.var_12 = (rb_state8.var_12) ^ (rb_state8.var_10);
            rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x46523414));
            rb_state8.var_5 = (rb_state8.var_5) - (rb_state8.var_13);
            rb_state8.var_4 = rb_state8.var_5;
            pthread_mutex_unlock(&(rb_state8.lock_14));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (130 < rb_input_size ? (uint32_t)rb_input[130] : 0x5b7c6a29);
        if ((rb_state10.var_3) == (0x0)) {
            rb_state10.var_5 = 0xe30a3da;
            rb_state10.var_6 = (rb_state10.var_6) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0xece78a2f));
            rb_state10.var_7 = (rb_state10.var_7) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0x2dc4566b));
            rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_7);
            rb_state10.var_9 = (rb_state10.var_9) ^ (((0x8c78bfb2) + (rb_state10.var_6)) - (rb_state10.var_8));
            rb_state10.var_10 = (rb_state10.var_10) + (rb_state10.var_7);
            rb_state10.var_11 = (rb_state10.var_11) + (((((0xda418439) + (rb_state10.var_9)) ^ (rb_state10.var_8)) + (rb_state10.var_10)) ^ (rb_state10.var_9));
            rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_10) : (0xe75f144c));
            rb_state10.var_13 = (rb_state10.var_13) - (((((0x75d455b5) + (rb_state10.var_12)) + (0xffea7ba7)) - (rb_state10.var_11)) + (rb_state10.var_12));
            rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ ((0x869dd65e) ^ (0x6aa6213f));
        rb_state16.var_3 = (rb_state16.var_3) - (0x5183c73b);
        rb_state16.var_4 = (rb_state16.var_4) + (0x51092acf);
        rb_state16.var_3 = (rb_state16.var_3) ^ (1372 < rb_input_size ? (uint32_t)rb_input[1372] : 0xb59a15f0);
        rb_state16.var_4 = (rb_state16.var_4) + (((rb_state16.var_1) == (0xec3bf7a0)) ? (rb_state16.var_0) : (0x4a889ef5));
        if ((rb_state16.var_3) == (0x4368dbc8)) {
            pthread_mutex_lock(&(rb_state16.lock_21));
            rb_state16.var_7 = 0x80f9ad7;
            rb_state16.var_8 = (rb_state16.var_8) + ((0xc87a08b5) + (rb_state16.var_7));
            rb_state16.var_9 = (rb_state16.var_9) - (((((0xd37dfb98) ^ (rb_state16.var_5)) + (rb_state16.var_8)) - (0x43633f81)) + (rb_state16.var_6));
            rb_state16.var_10 = (rb_state16.var_10) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_7) : (0xc7eefcde));
            rb_state16.var_11 = (rb_state16.var_11) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_8) : (0xbe8ddf5d));
            rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_9) : (0xd00f6ad5));
            rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0x38841735));
            rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x73732a2a));
            rb_state16.var_6 = (rb_state16.var_6) ^ (rb_state16.var_14);
            rb_state16.var_5 = rb_state16.var_6;
            pthread_mutex_unlock(&(rb_state16.lock_21));
        }
        if ((rb_state16.var_3) == (0x4368dbc8)) {
            if (!((rb_state16.var_5) == (rb_state16.var_6))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_4 = (rb_state19.var_4) ^ (0xa319b922);
        rb_state19.var_0 = (rb_state19.var_0) + ((0xfc10a9a0) ^ (0x1812eb50));
        #endif
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_0);
        rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_3) == (0xfef40e73)) ? (rb_state0.var_2) : (0xc7a830a9));
        if ((rb_state0.var_0) == (0x10bf18d)) {
            rb_state0.var_4 = rb_state0.var_1;
        }
        if ((rb_state0.var_0) == (0x10bf18d)) {
            if (!((rb_state0.var_1) == (rb_state0.var_4))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_3) == (0x7b35d690)) {
            rb_state1.var_4 = rb_state1.var_1;
        }
        if ((rb_state1.var_3) == (0x7b35d690)) {
            rb_state1.var_8 = !((rb_state1.var_1) == (rb_state1.var_4));
        }
        rb_state1.var_6 = (rb_state1.var_6) - (1133 < rb_input_size ? (uint32_t)rb_input[1133] : 0x23cee198);
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_8) : (0x8a0397e));
        rb_state1.var_6 = (rb_state1.var_6) ^ (0x3a77ebd4);
        rb_state1.var_6 = (rb_state1.var_6) + ((0x3016c921) ^ (rb_state1.var_5));
        if ((rb_state1.var_3) == (0x153922c4)) {
            rb_state1.var_12 = 0xf9570092;
            rb_state1.var_13 = 0x6a6a69b9;
            rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_12) : (0xda58b10d));
            rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0xabab5bdc));
            rb_state1.var_11 = (rb_state1.var_11) ^ (rb_state1.var_15);
            rb_state1.var_9 = rb_state1.var_11;
        }
        if ((rb_state1.var_3) == (0x153922c4)) {
            pthread_mutex_lock(&(rb_state1.lock_30));
            rb_state1.var_17 = 0xb658095;
            rb_state1.var_18 = (rb_state1.var_18) ^ ((0x5c45cef0) ^ (0xd7789595));
            rb_state1.var_19 = (rb_state1.var_19) + (((((0x33cdeb0) + (rb_state1.var_13)) ^ (rb_state1.var_14)) + (rb_state1.var_17)) ^ (rb_state1.var_12));
            rb_state1.var_20 = (rb_state1.var_20) ^ ((0x170a937e) - (0x588a61d5));
            rb_state1.var_21 = (rb_state1.var_21) + (((0xe204459a) + (rb_state1.var_15)) + (rb_state1.var_11));
            rb_state1.var_22 = (rb_state1.var_22) + (((((0x6d5548cb) + (rb_state1.var_16)) - (rb_state1.var_17)) ^ (rb_state1.var_19)) + (rb_state1.var_18));
            rb_state1.var_23 = (rb_state1.var_23) ^ (((0xf687806f) + (0x94d18c6a)) - (rb_state1.var_20));
            rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_21) : (0xd1430d04));
            rb_state1.var_25 = (rb_state1.var_25) ^ (((((0xd4ed010) + (rb_state1.var_22)) + (0x126a9116)) ^ (rb_state1.var_23)) - (rb_state1.var_19));
            rb_state1.var_26 = (rb_state1.var_26) + (((0x85a5897) ^ (rb_state1.var_24)) ^ (rb_state1.var_20));
            rb_state1.var_27 = (rb_state1.var_27) - (((0x20ff03e) + (0x9a1791cf)) - (rb_state1.var_25));
            rb_state1.var_28 = (rb_state1.var_28) - (rb_state1.var_26);
            rb_state1.var_29 = (rb_state1.var_29) + (((((0x9bc9bc04) - (rb_state1.var_28)) + (rb_state1.var_27)) + (rb_state1.var_21)) - (0xfdd35c5));
            rb_state1.var_16 = (rb_state1.var_16) - (rb_state1.var_29);
            rb_state1.var_9 = rb_state1.var_16;
            pthread_mutex_unlock(&(rb_state1.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_1);
        rb_state8.var_8 = (rb_state8.var_8) + (rb_state8.var_9);
        rb_state8.var_10 = (rb_state8.var_10) - (784 < rb_input_size ? (uint32_t)rb_input[784] : 0x6c2a545f);
        rb_state8.var_8 = (rb_state8.var_8) + (0xe4437e71);
        if ((rb_state8.var_15) == (0x0)) {
            rb_state8.var_17 = 0x55bb45cb;
            rb_state8.var_18 = 0x3d846714;
            rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_18) : (0xe7dd786f));
            rb_state8.var_20 = (rb_state8.var_20) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_16) : (0x6011c8c4));
            rb_state8.var_21 = (rb_state8.var_21) + (rb_state8.var_19);
            rb_state8.var_22 = (rb_state8.var_22) ^ (rb_state8.var_20);
            rb_state8.var_23 = (rb_state8.var_23) + (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_22) : (0x812464b0));
            rb_state8.var_9 = (rb_state8.var_9) - (rb_state8.var_23);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_3);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (((rb_state16.var_4) == (0x51092acf)) ? (rb_state16.var_3) : (0x6963c0af));
        rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_2) == (0xae7c3801)) ? (rb_state16.var_1) : (0x4798e5cb));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) + (555 < rb_input_size ? (uint32_t)rb_input[555] : 0x344a6b4e);
        rb_state19.var_1 = (rb_state19.var_1) - (1827 < rb_input_size ? (uint32_t)rb_input[1827] : 0xaa7cfd8);
        if ((rb_state19.var_2) == (0xfffff0af)) {
            pthread_mutex_lock(&(rb_state19.lock_26));
            rb_state19.var_7 = 0x75feeee;
            rb_state19.var_8 = 0x68061627;
            rb_state19.var_9 = (rb_state19.var_9) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_8) : (0x113eba09));
            rb_state19.var_10 = (rb_state19.var_10) - (rb_state19.var_6);
            rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_7) : (0x7878b369));
            rb_state19.var_12 = (rb_state19.var_12) ^ (rb_state19.var_9);
            rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_10) : (0xb3e6316f));
            rb_state19.var_14 = (rb_state19.var_14) - (rb_state19.var_11);
            rb_state19.var_15 = (rb_state19.var_15) - (((((0xcdc3319c) + (rb_state19.var_10)) - (rb_state19.var_9)) - (rb_state19.var_13)) + (rb_state19.var_12));
            rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_14) : (0x1383573a));
            rb_state19.var_17 = (rb_state19.var_17) - (((0xc94eef88) + (rb_state19.var_15)) ^ (rb_state19.var_12));
            rb_state19.var_18 = (rb_state19.var_18) ^ (((0x17c47099) + (rb_state19.var_16)) + (0x91bf731b));
            rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_17) : (0x655843e));
            rb_state19.var_20 = (rb_state19.var_20) + (((((0xeb1aa30) ^ (0x50c63023)) + (rb_state19.var_18)) ^ (rb_state19.var_19)) ^ (rb_state19.var_14));
            rb_state19.var_6 = (rb_state19.var_6) ^ (rb_state19.var_20);
            rb_state19.var_5 = rb_state19.var_6;
            pthread_mutex_unlock(&(rb_state19.lock_26));
        }
        if ((rb_state19.var_2) == (0xfffff0af)) {
            if (!((rb_state19.var_5) == (rb_state19.var_6))) {
                racebench_trigger(19);
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