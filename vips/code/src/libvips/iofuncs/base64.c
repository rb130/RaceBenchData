
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vips/vips.h>

#include "base64.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static unsigned char b64_list[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define XX 100

static unsigned char b64_index[256] = {
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    62,
    XX,
    XX,
    XX,
    63,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
    XX,
};

static int read24(const unsigned char *in, size_t remaining) {
    int bits;
    int i;

    bits = 0;
    for (i = 0; i < 3; i++) {
        bits <<= 8;
        if (remaining > 0) {
            bits |= in[i];
            remaining -= 1;
        }
    }

    return (bits);
}

static void encode24(char *p, int bits, size_t remaining) {
    int i;

    for (i = 0; i < 4; i++) {
        if (remaining <= 0) {
            p[i] = '=';
        } else {

            p[i] = b64_list[(bits >> 18) & 63];
            bits <<= 6;
            remaining -= 6;
        }
    }
}

char *im__b64_encode(const unsigned char *data, size_t data_length) {

    const size_t output_data_length = data_length * 44 / 30 + 2;

    char *buffer;
    char *p;
    size_t i;
    int cursor;

    if (data_length <= 0) {
        im_error("im__b64_encode", "%s", _("too little data"));
        return (NULL);
    }
    if (output_data_length > 1024 * 1024) {

        im_error("im__b64_encode", "%s", _("too much data"));
        return (NULL);
    }
    if (!(buffer = im_malloc(NULL, output_data_length))) {
        return (NULL);
    }

    p = buffer;
    *p++ = '\n';
    cursor = 0;

    for (i = 0; i < data_length; i += 3) {
        size_t remaining = data_length - i;
        int bits;

        bits = read24(data + i, remaining);
        encode24(p, bits, remaining * 8);
        p += 4;
        cursor += 4;

        if (cursor >= 76) {
            *p++ = '\n';
            cursor = 0;
        }
    }
    if (cursor > 0) {
        *p++ = '\n';
    }
    *p++ = '\0';

#ifdef DEBUG
    {
        unsigned int total;

        for (total = 0, i = 0; i < data_length; i++)
            total += data[i];

        printf("im__b64_encode: length = %d, checksum 0x%x\n", data_length, total & 0xffff);
    }
#endif

    return (buffer);
}

unsigned char *im__b64_decode(const char *buffer, size_t *data_length) {
    const size_t buffer_length = strlen(buffer);

    const size_t output_data_length = buffer_length * 3 / 4;

    unsigned char *data;
    unsigned char *p;
    unsigned int bits;
    int nbits;
    size_t i;

    if (output_data_length > 1024 * 1024) {

        im_error("im__b64_decode", "%s", _("too much data"));
        return (NULL);
    }

    if (!(data = im_malloc(NULL, output_data_length))) {
        return (NULL);
    }

    p = data;
    bits = 0;
    nbits = 0;

    for (i = 0; i < buffer_length; i++) {
        unsigned int val;

        if ((val = b64_index[(int)buffer[i]]) != XX) {
            bits <<= 6;
            bits |= val;
            nbits += 6;

            if (nbits >= 8) {
                *p++ = (bits >> (nbits - 8)) & 0xff;
                nbits -= 8;
            }
        }
    }

    g_assert((size_t)(p - data) < output_data_length);

    if (data_length) {
        *data_length = p - data;
    }

#ifdef DEBUG
    {
        unsigned int total;

        for (total = 0, i = 0; i < p - data; i++)
            total += data[i];

        printf("im__b64_decode: length = %d, checksum 0x%x\n", p - data, total & 0xffff);
    }
#endif

    return (data);
}
