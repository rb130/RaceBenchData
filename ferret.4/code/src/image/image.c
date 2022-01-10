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
    rb_state0.var_8 = (rb_state0.var_8) + (1313 < rb_input_size ? (uint32_t)rb_input[1313] : 0xd27f478f);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (((rb_state9.var_0) == (0xb4fa7393)) ? (rb_state9.var_3) : (0x922706d2));
    if ((rb_state9.var_1) == (0x76e8b64e)) {
        rb_state9.var_7 = 0xed84e54c;
        rb_state9.var_8 = 0xf6cc304e;
        rb_state9.var_9 = (rb_state9.var_9) - (((0xbbd27fb0) ^ (rb_state9.var_7)) - (rb_state9.var_6));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((0x2be9ea76) ^ (0x25e5263b)) - (rb_state9.var_7));
        rb_state9.var_11 = (rb_state9.var_11) + (((((0xd6d2a2bf) + (rb_state9.var_9)) + (rb_state9.var_8)) - (rb_state9.var_8)) + (rb_state9.var_9));
        rb_state9.var_12 = (rb_state9.var_12) + (((((0x535deeef) + (0xef683506)) + (rb_state9.var_10)) + (rb_state9.var_11)) + (rb_state9.var_10));
        rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0xce23336f);
    #endif
    cinfo.err = jpeg_std_error(&jerr);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
    #endif
    jpeg_create_decompress(&cinfo);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_4) == (0x626e9df1)) {
        rb_state0.var_31 = 0xe4720620;
        rb_state0.var_32 = 0xa2a2f582;
        rb_state0.var_33 = 0xc51321fe;
        rb_state0.var_34 = (rb_state0.var_34) - (rb_state0.var_32);
        rb_state0.var_35 = (rb_state0.var_35) ^ (((0xd6d42b2a) + (rb_state0.var_24)) ^ (rb_state0.var_31));
        rb_state0.var_36 = (rb_state0.var_36) - (((rb_state0.var_33) == (0xc51321fe)) ? (rb_state0.var_34) : (0x3b552c5f));
        rb_state0.var_37 = (rb_state0.var_37) - (rb_state0.var_35);
        rb_state0.var_38 = (rb_state0.var_38) ^ (rb_state0.var_36);
        rb_state0.var_39 = (rb_state0.var_39) - (rb_state0.var_37);
        rb_state0.var_40 = (rb_state0.var_40) + (((rb_state0.var_25) == (0x0)) ? (rb_state0.var_38) : (0xacd4b250));
        rb_state0.var_41 = (rb_state0.var_41) ^ (((rb_state0.var_39) == (0x32a62d0a)) ? (rb_state0.var_40) : (0x76aea5a1));
        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_41);
        rb_state0.var_11 = rb_state0.var_30;
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (1256 < rb_input_size ? (uint32_t)rb_input[1256] : 0x6a3192b6);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (1563 < rb_input_size ? (uint32_t)rb_input[1563] : 0xcaa09d60);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_1) : (0xefd4d02b));
    #endif
    jpeg_stdio_src(&cinfo, infile);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_4) == (0x626e9df1)) {
        pthread_mutex_lock(&(rb_state0.lock_57));
        rb_state0.var_43 = 0xb10263c5;
        rb_state0.var_44 = (rb_state0.var_44) ^ (2015 < rb_input_size ? (uint32_t)rb_input[2015] : 0xe8b84533);
        rb_state0.var_45 = (rb_state0.var_45) ^ (((rb_state0.var_28) == (0x0)) ? (rb_state0.var_26) : (0x535f3ed8));
        rb_state0.var_46 = (rb_state0.var_46) ^ ((0xdf99661d) + (rb_state0.var_29));
        rb_state0.var_47 = (rb_state0.var_47) - (((rb_state0.var_30) == (0x5d5d0a7e)) ? (rb_state0.var_43) : (0xac31da00));
        rb_state0.var_48 = (rb_state0.var_48) + (rb_state0.var_27);
        rb_state0.var_49 = (rb_state0.var_49) - (((rb_state0.var_31) == (0xe4720620)) ? (rb_state0.var_44) : (0x995db2fd));
        rb_state0.var_50 = (rb_state0.var_50) - (((rb_state0.var_32) == (0xa2a2f582)) ? (rb_state0.var_45) : (0x71cb8ea8));
        rb_state0.var_51 = (rb_state0.var_51) ^ (((rb_state0.var_33) == (0xc51321fe)) ? (rb_state0.var_46) : (0x8bca67f9));
        rb_state0.var_52 = (rb_state0.var_52) - (((((0xced2f7b5) + (rb_state0.var_47)) + (rb_state0.var_34)) - (rb_state0.var_35)) - (rb_state0.var_48));
        rb_state0.var_53 = (rb_state0.var_53) ^ (((rb_state0.var_49) == (0xffffffb5)) ? (rb_state0.var_50) : (0x6bbe86f0));
        rb_state0.var_54 = (rb_state0.var_54) ^ (rb_state0.var_51);
        rb_state0.var_55 = (rb_state0.var_55) ^ (((rb_state0.var_52) == (0xb7788e9c)) ? (rb_state0.var_53) : (0x9b2424b));
        rb_state0.var_56 = (rb_state0.var_56) ^ (((((0xaee1bff6) + (rb_state0.var_54)) ^ (rb_state0.var_36)) + (rb_state0.var_37)) ^ (rb_state0.var_55));
        rb_state0.var_42 = (rb_state0.var_42) + (rb_state0.var_56);
        rb_state0.var_11 = rb_state0.var_42;
        pthread_mutex_unlock(&(rb_state0.lock_57));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0xce233328)) {
        rb_state10.var_12 = 0x71f7fb72;
        rb_state10.var_13 = (rb_state10.var_13) - (((((0xabe5b0dc) + (rb_state10.var_10)) - (rb_state10.var_12)) + (rb_state10.var_9)) - (rb_state10.var_7));
        rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_13) : (0xca9a3642));
        rb_state10.var_11 = (rb_state10.var_11) ^ (rb_state10.var_14);
        rb_state10.var_3 = rb_state10.var_11;
    }
    if ((rb_state10.var_2) == (0xce233328)) {
        pthread_mutex_lock(&(rb_state10.lock_26));
        rb_state10.var_16 = 0x242f5f49;
        rb_state10.var_17 = (rb_state10.var_17) ^ (((((0x22220f32) - (rb_state10.var_16)) + (rb_state10.var_14)) + (rb_state10.var_12)) + (rb_state10.var_13));
        rb_state10.var_18 = (rb_state10.var_18) + (((0x3e788e3f) + (rb_state10.var_15)) ^ (rb_state10.var_11));
        rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_16) == (0x242f5f49)) ? (rb_state10.var_17) : (0x8b0d3566));
        rb_state10.var_20 = (rb_state10.var_20) ^ (((0xb5a70f47) - (rb_state10.var_18)) + (0x35208f6a));
        rb_state10.var_21 = (rb_state10.var_21) - (rb_state10.var_19);
        rb_state10.var_22 = (rb_state10.var_22) - (rb_state10.var_20);
        rb_state10.var_23 = (rb_state10.var_23) + (rb_state10.var_21);
        rb_state10.var_24 = (rb_state10.var_24) ^ (((rb_state10.var_17) == (0x6feaab5b)) ? (rb_state10.var_22) : (0x245c4297));
        rb_state10.var_25 = (rb_state10.var_25) + (((((0xd6359bc) - (rb_state10.var_23)) + (0x3c991)) + (0xbd45a486)) - (rb_state10.var_24));
        rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_25);
        rb_state10.var_3 = rb_state10.var_15;
        pthread_mutex_unlock(&(rb_state10.lock_26));
    }
    #endif
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (98 < rb_input_size ? (uint32_t)rb_input[98] : 0x414af58d);
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0x3b1deade) ^ (0xae7b4f98));
    #endif
    orig = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);
    if (orig == NULL) {
        fatal("out of memory");
    }
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xf4127eaa)) {
        pthread_mutex_lock(&(rb_state2.lock_20));
        rb_state2.var_5 = 0x41e08a42;
        rb_state2.var_6 = (rb_state2.var_6) ^ (((((0xb75a3e0e) + (0x62589e26)) ^ (rb_state2.var_3)) - (rb_state2.var_5)) - (rb_state2.var_5));
        rb_state2.var_7 = (rb_state2.var_7) + ((0xe6bd071e) ^ (rb_state2.var_6));
        rb_state2.var_8 = (rb_state2.var_8) + (((0xf5760d6) ^ (rb_state2.var_4)) + (0x87457901));
        rb_state2.var_9 = (rb_state2.var_9) + (((((0xbd00995) + (rb_state2.var_7)) ^ (rb_state2.var_6)) - (0xc999125)) + (0x35adb60));
        rb_state2.var_10 = (rb_state2.var_10) ^ (((0xe97ffb23) + (rb_state2.var_8)) - (0xca62bb5a));
        rb_state2.var_11 = (rb_state2.var_11) - (rb_state2.var_9);
        rb_state2.var_12 = (rb_state2.var_12) ^ (((((0x1d900541) ^ (rb_state2.var_7)) + (rb_state2.var_11)) ^ (0x66b006ef)) - (rb_state2.var_10));
        rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_12);
        rb_state2.var_3 = rb_state2.var_4;
        pthread_mutex_unlock(&(rb_state2.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + ((0xdda4f1a7) + (0x6e801cac));
    rb_state19.var_2 = (rb_state19.var_2) + ((0x480022e2) - (rb_state19.var_1));
    #endif
    row_pointer[0] = orig;
    while (cinfo.output_scanline < cinfo.output_height) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (0xd34675ca);
        rb_state3.var_3 = (rb_state3.var_3) - (1517 < rb_input_size ? (uint32_t)rb_input[1517] : 0x18c2ba02);
        rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_1) == (0x11)) ? (rb_state3.var_0) : (0x356c15b4));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_3 = (rb_state8.var_3) ^ (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0x5fd74f49));
        rb_state8.var_0 = (rb_state8.var_0) - ((0x9c86d818) + (rb_state8.var_1));
        rb_state8.var_1 = (rb_state8.var_1) ^ ((0x3cc3635b) + (rb_state8.var_2));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (0x81729606);
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_3);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (1742 < rb_input_size ? (uint32_t)rb_input[1742] : 0x6594cada);
        rb_state14.var_2 = (rb_state14.var_2) ^ (0x5a23b18d);
        rb_state14.var_1 = (rb_state14.var_1) + ((0xdb33603d) + (rb_state14.var_2));
        rb_state14.var_0 = (rb_state14.var_0) + ((0x49671eba) ^ (0xf3f0c277));
        if ((rb_state14.var_1) == (0x3ad85ee)) {
            pthread_mutex_lock(&(rb_state14.lock_10));
            rb_state14.var_6 = 0x8edec0e5;
            rb_state14.var_7 = (rb_state14.var_7) - ((0x120ce0fb) ^ (0x5fd88a76));
            rb_state14.var_8 = (rb_state14.var_8) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0x9f52366c));
            rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0x301ab8a6));
            rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_9);
            pthread_mutex_unlock(&(rb_state14.lock_10));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_3);
        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_2);
        #endif
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0xf4127eaa)) {
            pthread_mutex_lock(&(rb_state2.lock_20));
            if (!((rb_state2.var_3) == (rb_state2.var_4))) {
                racebench_trigger(2);
            }
            pthread_mutex_unlock(&(rb_state2.lock_20));
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + (0xaf2d848e);
        rb_state3.var_3 = (rb_state3.var_3) ^ ((0xa67e6a6f) + (rb_state3.var_0));
        if ((rb_state3.var_1) == (0x5ad01449)) {
            rb_state3.var_16 = 0xa5cda93c;
            rb_state3.var_17 = (rb_state3.var_17) + (((((0xb4f31875) ^ (rb_state3.var_11)) ^ (rb_state3.var_10)) + (rb_state3.var_12)) - (rb_state3.var_16));
            rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_9);
            rb_state3.var_19 = (rb_state3.var_19) ^ (((((0x3c98342) + (0xc079d44d)) ^ (rb_state3.var_13)) + (rb_state3.var_17)) - (rb_state3.var_18));
            rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_19);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0xe643b7ca) - (rb_state8.var_1));
        rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_0) == (0x8428995d)) ? (rb_state8.var_3) : (0xd654a33f));
        if ((rb_state8.var_3) == (0xa028b032)) {
            rb_state8.var_4 = rb_state8.var_0;
        }
        if ((rb_state8.var_3) == (0xa028b032)) {
            if (!((rb_state8.var_0) == (rb_state8.var_4))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) - ((0xd6541dd2) ^ (rb_state11.var_1));
        rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_0) == (0xf4)) ? (rb_state11.var_3) : (0x851482b3));
        if ((rb_state11.var_1) == (0x817296b7)) {
            pthread_mutex_lock(&(rb_state11.lock_20));
            rb_state11.var_4 = rb_state11.var_2;
            pthread_mutex_unlock(&(rb_state11.lock_20));
        }
        if ((rb_state11.var_1) == (0x817296b7)) {
            if (!((rb_state11.var_2) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + ((0xc444938) ^ (rb_state14.var_2));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_0);
        if ((rb_state19.var_3) == (0x84)) {
            pthread_mutex_lock(&(rb_state19.lock_29));
            rb_state19.var_6 = 0x467751a5;
            rb_state19.var_7 = (rb_state19.var_7) + (rb_state19.var_5);
            rb_state19.var_8 = (rb_state19.var_8) - ((0x872e5cd7) - (0xb494b830));
            rb_state19.var_9 = (rb_state19.var_9) + (((((0x88555f3f) ^ (rb_state19.var_4)) - (rb_state19.var_6)) + (0x9643950f)) + (rb_state19.var_6));
            rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0xc29c8192));
            rb_state19.var_11 = (rb_state19.var_11) - (rb_state19.var_9);
            rb_state19.var_12 = (rb_state19.var_12) ^ (((0x4fcc192a) ^ (rb_state19.var_10)) ^ (0x9c236762));
            rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_11);
            rb_state19.var_14 = (rb_state19.var_14) - (((((0x58c38dfa) ^ (rb_state19.var_7)) + (0x9a3095a7)) + (rb_state19.var_12)) + (rb_state19.var_13));
            rb_state19.var_5 = (rb_state19.var_5) + (rb_state19.var_14);
            rb_state19.var_4 = rb_state19.var_5;
            pthread_mutex_unlock(&(rb_state19.lock_29));
        }
        if ((rb_state19.var_3) == (0xb0)) {
            pthread_mutex_lock(&(rb_state19.lock_29));
            if (!((rb_state19.var_4) == (rb_state19.var_5))) {
                racebench_trigger(19);
            }
            pthread_mutex_unlock(&(rb_state19.lock_29));
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