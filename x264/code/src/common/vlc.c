
#include "common.h"

#define MKVLC(a, b) \
    { a, b }
const vlc_t x264_coeff_token[5][17 * 4] = {

    {
        MKVLC(0x1, 1), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x5, 6), MKVLC(0x1, 2), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x7, 8), MKVLC(0x4, 6), MKVLC(0x1, 3), MKVLC(0x0, 0),

        MKVLC(0x7, 9), MKVLC(0x6, 8), MKVLC(0x5, 7), MKVLC(0x3, 5),

        MKVLC(0x7, 10), MKVLC(0x6, 9), MKVLC(0x5, 8), MKVLC(0x3, 6),

        MKVLC(0x7, 11), MKVLC(0x6, 10), MKVLC(0x5, 9), MKVLC(0x4, 7),

        MKVLC(0xf, 13), MKVLC(0x6, 11), MKVLC(0x5, 10), MKVLC(0x4, 8),

        MKVLC(0xb, 13), MKVLC(0xe, 13), MKVLC(0x5, 11), MKVLC(0x4, 9),

        MKVLC(0x8, 13), MKVLC(0xa, 13), MKVLC(0xd, 13), MKVLC(0x4, 10),

        MKVLC(0xf, 14), MKVLC(0xe, 14), MKVLC(0x9, 13), MKVLC(0x4, 11),

        MKVLC(0xb, 14), MKVLC(0xa, 14), MKVLC(0xd, 14), MKVLC(0xc, 13),

        MKVLC(0xf, 15), MKVLC(0xe, 15), MKVLC(0x9, 14), MKVLC(0xc, 14),

        MKVLC(0xb, 15), MKVLC(0xa, 15), MKVLC(0xd, 15), MKVLC(0x8, 14),

        MKVLC(0xf, 16), MKVLC(0x1, 15), MKVLC(0x9, 15), MKVLC(0xc, 15),

        MKVLC(0xb, 16), MKVLC(0xe, 16), MKVLC(0xd, 16), MKVLC(0x8, 15),

        MKVLC(0x7, 16), MKVLC(0xa, 16), MKVLC(0x9, 16), MKVLC(0xc, 16),

        MKVLC(0x4, 16), MKVLC(0x6, 16), MKVLC(0x5, 16), MKVLC(0x8, 16),
    },

    {
        MKVLC(0x3, 2), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0xb, 6), MKVLC(0x2, 2), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x7, 6), MKVLC(0x7, 5), MKVLC(0x3, 3), MKVLC(0x0, 0),

        MKVLC(0x7, 7), MKVLC(0xa, 6), MKVLC(0x9, 6), MKVLC(0x5, 4),

        MKVLC(0x7, 8), MKVLC(0x6, 6), MKVLC(0x5, 6), MKVLC(0x4, 4),

        MKVLC(0x4, 8), MKVLC(0x6, 7), MKVLC(0x5, 7), MKVLC(0x6, 5),

        MKVLC(0x7, 9), MKVLC(0x6, 8), MKVLC(0x5, 8), MKVLC(0x8, 6),

        MKVLC(0xf, 11), MKVLC(0x6, 9), MKVLC(0x5, 9), MKVLC(0x4, 6),

        MKVLC(0xb, 11), MKVLC(0xe, 11), MKVLC(0xd, 11), MKVLC(0x4, 7),

        MKVLC(0xf, 12), MKVLC(0xa, 11), MKVLC(0x9, 11), MKVLC(0x4, 9),

        MKVLC(0xb, 12), MKVLC(0xe, 12), MKVLC(0xd, 12), MKVLC(0xc, 11),

        MKVLC(0x8, 12), MKVLC(0xa, 12), MKVLC(0x9, 12), MKVLC(0x8, 11),

        MKVLC(0xf, 13), MKVLC(0xe, 13), MKVLC(0xd, 13), MKVLC(0xc, 12),

        MKVLC(0xb, 13), MKVLC(0xa, 13), MKVLC(0x9, 13), MKVLC(0xc, 13),

        MKVLC(0x7, 13), MKVLC(0xb, 14), MKVLC(0x6, 13), MKVLC(0x8, 13),

        MKVLC(0x9, 14), MKVLC(0x8, 14), MKVLC(0xa, 14), MKVLC(0x1, 13),

        MKVLC(0x7, 14), MKVLC(0x6, 14), MKVLC(0x5, 14), MKVLC(0x4, 14),
    },

    {
        MKVLC(0xf, 4), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0xf, 6), MKVLC(0xe, 4), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0xb, 6), MKVLC(0xf, 5), MKVLC(0xd, 4), MKVLC(0x0, 0),

        MKVLC(0x8, 6), MKVLC(0xc, 5), MKVLC(0xe, 5), MKVLC(0xc, 4),

        MKVLC(0xf, 7), MKVLC(0xa, 5), MKVLC(0xb, 5), MKVLC(0xb, 4),

        MKVLC(0xb, 7), MKVLC(0x8, 5), MKVLC(0x9, 5), MKVLC(0xa, 4),

        MKVLC(0x9, 7), MKVLC(0xe, 6), MKVLC(0xd, 6), MKVLC(0x9, 4),

        MKVLC(0x8, 7), MKVLC(0xa, 6), MKVLC(0x9, 6), MKVLC(0x8, 4),

        MKVLC(0xf, 8), MKVLC(0xe, 7), MKVLC(0xd, 7), MKVLC(0xd, 5),

        MKVLC(0xb, 8), MKVLC(0xe, 8), MKVLC(0xa, 7), MKVLC(0xc, 6),

        MKVLC(0xf, 9), MKVLC(0xa, 8), MKVLC(0xd, 8), MKVLC(0xc, 7),

        MKVLC(0xb, 9), MKVLC(0xe, 9), MKVLC(0x9, 8), MKVLC(0xc, 8),

        MKVLC(0x8, 9), MKVLC(0xa, 9), MKVLC(0xd, 9), MKVLC(0x8, 8),

        MKVLC(0xd, 10), MKVLC(0x7, 9), MKVLC(0x9, 9), MKVLC(0xc, 9),

        MKVLC(0x9, 10), MKVLC(0xc, 10), MKVLC(0xb, 10), MKVLC(0xa, 10),

        MKVLC(0x5, 10), MKVLC(0x8, 10), MKVLC(0x7, 10), MKVLC(0x6, 10),

        MKVLC(0x1, 10), MKVLC(0x4, 10), MKVLC(0x3, 10), MKVLC(0x2, 10),
    },

    {
        MKVLC(0x3, 6), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 6), MKVLC(0x1, 6), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x4, 6), MKVLC(0x5, 6), MKVLC(0x6, 6), MKVLC(0x0, 0),

        MKVLC(0x8, 6), MKVLC(0x9, 6), MKVLC(0xa, 6), MKVLC(0xb, 6),

        MKVLC(0xc, 6), MKVLC(0xd, 6), MKVLC(0xe, 6), MKVLC(0xf, 6),

        MKVLC(0x10, 6), MKVLC(0x11, 6), MKVLC(0x12, 6), MKVLC(0x13, 6),

        MKVLC(0x14, 6), MKVLC(0x15, 6), MKVLC(0x16, 6), MKVLC(0x17, 6),

        MKVLC(0x18, 6), MKVLC(0x19, 6), MKVLC(0x1a, 6), MKVLC(0x1b, 6),

        MKVLC(0x1c, 6), MKVLC(0x1d, 6), MKVLC(0x1e, 6), MKVLC(0x1f, 6),

        MKVLC(0x20, 6), MKVLC(0x21, 6), MKVLC(0x22, 6), MKVLC(0x23, 6),

        MKVLC(0x24, 6), MKVLC(0x25, 6), MKVLC(0x26, 6), MKVLC(0x27, 6),

        MKVLC(0x28, 6), MKVLC(0x29, 6), MKVLC(0x2a, 6), MKVLC(0x2b, 6),

        MKVLC(0x2c, 6), MKVLC(0x2d, 6), MKVLC(0x2e, 6), MKVLC(0x2f, 6),

        MKVLC(0x30, 6), MKVLC(0x31, 6), MKVLC(0x32, 6), MKVLC(0x33, 6),

        MKVLC(0x34, 6), MKVLC(0x35, 6), MKVLC(0x36, 6), MKVLC(0x37, 6),

        MKVLC(0x38, 6), MKVLC(0x39, 6), MKVLC(0x3a, 6), MKVLC(0x3b, 6),

        MKVLC(0x3c, 6), MKVLC(0x3d, 6), MKVLC(0x3e, 6), MKVLC(0x3f, 6),
    },

    {
        MKVLC(0x1, 2), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x7, 6), MKVLC(0x1, 1), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x4, 6), MKVLC(0x6, 6), MKVLC(0x1, 3), MKVLC(0x0, 0),

        MKVLC(0x3, 6), MKVLC(0x3, 7), MKVLC(0x2, 7), MKVLC(0x5, 6),

        MKVLC(0x2, 6), MKVLC(0x3, 8), MKVLC(0x2, 8), MKVLC(0x0, 7),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),

        MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0), MKVLC(0x0, 0),
    }};

const vlc_t x264_total_zeros[15][16] = {
    {
        MKVLC(0x1, 1),
        MKVLC(0x3, 3),
        MKVLC(0x2, 3),
        MKVLC(0x3, 4),
        MKVLC(0x2, 4),
        MKVLC(0x3, 5),
        MKVLC(0x2, 5),
        MKVLC(0x3, 6),
        MKVLC(0x2, 6),
        MKVLC(0x3, 7),
        MKVLC(0x2, 7),
        MKVLC(0x3, 8),
        MKVLC(0x2, 8),
        MKVLC(0x3, 9),
        MKVLC(0x2, 9),
        MKVLC(0x1, 9),
    },
    {
        MKVLC(0x7, 3),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x5, 4),
        MKVLC(0x4, 4),
        MKVLC(0x3, 4),
        MKVLC(0x2, 4),
        MKVLC(0x3, 5),
        MKVLC(0x2, 5),
        MKVLC(0x3, 6),
        MKVLC(0x2, 6),
        MKVLC(0x1, 6),
        MKVLC(0x0, 6),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x5, 4),
        MKVLC(0x7, 3),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 4),
        MKVLC(0x3, 4),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x2, 4),
        MKVLC(0x3, 5),
        MKVLC(0x2, 5),
        MKVLC(0x1, 6),
        MKVLC(0x1, 5),
        MKVLC(0x0, 6),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x3, 5),
        MKVLC(0x7, 3),
        MKVLC(0x5, 4),
        MKVLC(0x4, 4),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 4),
        MKVLC(0x3, 3),
        MKVLC(0x2, 4),
        MKVLC(0x2, 5),
        MKVLC(0x1, 5),
        MKVLC(0x0, 5),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x5, 4),
        MKVLC(0x4, 4),
        MKVLC(0x3, 4),
        MKVLC(0x7, 3),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x2, 4),
        MKVLC(0x1, 5),
        MKVLC(0x1, 4),
        MKVLC(0x0, 5),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 6),
        MKVLC(0x1, 5),
        MKVLC(0x7, 3),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x2, 3),
        MKVLC(0x1, 4),
        MKVLC(0x1, 3),
        MKVLC(0x0, 6),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 6),
        MKVLC(0x1, 5),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x3, 2),
        MKVLC(0x2, 3),
        MKVLC(0x1, 4),
        MKVLC(0x1, 3),
        MKVLC(0x0, 6),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 6),
        MKVLC(0x1, 4),
        MKVLC(0x1, 5),
        MKVLC(0x3, 3),
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x2, 3),
        MKVLC(0x1, 3),
        MKVLC(0x0, 6),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 6),
        MKVLC(0x0, 6),
        MKVLC(0x1, 4),
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x1, 3),
        MKVLC(0x1, 2),
        MKVLC(0x1, 5),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 5),
        MKVLC(0x0, 5),
        MKVLC(0x1, 3),
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x1, 2),
        MKVLC(0x1, 4),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x0, 4),
        MKVLC(0x1, 4),
        MKVLC(0x1, 3),
        MKVLC(0x2, 3),
        MKVLC(0x1, 1),
        MKVLC(0x3, 3),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x0, 4),
        MKVLC(0x1, 4),
        MKVLC(0x1, 2),
        MKVLC(0x1, 1),
        MKVLC(0x1, 3),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x0, 3),
        MKVLC(0x1, 3),
        MKVLC(0x1, 1),
        MKVLC(0x1, 2),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x0, 2),
        MKVLC(0x1, 2),
        MKVLC(0x1, 1),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x0, 1),
        MKVLC(0x1, 1),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
};

const vlc_t x264_total_zeros_dc[3][4] = {{MKVLC(0x01, 1), MKVLC(0x01, 2), MKVLC(0x01, 3), MKVLC(0x00, 3)}, {MKVLC(0x01, 1), MKVLC(0x01, 2), MKVLC(0x00, 2), MKVLC(0x00, 0)}, {MKVLC(0x01, 1), MKVLC(0x00, 1), MKVLC(0x00, 0), MKVLC(0x00, 0)}};

const vlc_t x264_run_before[7][15] = {
    {
        MKVLC(0x1, 1),
        MKVLC(0x0, 1),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x1, 1),
        MKVLC(0x1, 2),
        MKVLC(0x0, 2),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x1, 2),
        MKVLC(0x0, 2),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x1, 2),
        MKVLC(0x1, 3),
        MKVLC(0x0, 3),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x3, 2),
        MKVLC(0x2, 2),
        MKVLC(0x3, 3),
        MKVLC(0x2, 3),
        MKVLC(0x1, 3),
        MKVLC(0x0, 3),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x3, 2),
        MKVLC(0x0, 3),
        MKVLC(0x1, 3),
        MKVLC(0x3, 3),
        MKVLC(0x2, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
        MKVLC(0x0, 0),
    },
    {
        MKVLC(0x7, 3),
        MKVLC(0x6, 3),
        MKVLC(0x5, 3),
        MKVLC(0x4, 3),
        MKVLC(0x3, 3),
        MKVLC(0x2, 3),
        MKVLC(0x1, 3),
        MKVLC(0x1, 4),
        MKVLC(0x1, 5),
        MKVLC(0x1, 6),
        MKVLC(0x1, 7),
        MKVLC(0x1, 8),
        MKVLC(0x1, 9),
        MKVLC(0x1, 10),
        MKVLC(0x1, 11),
    },
};
