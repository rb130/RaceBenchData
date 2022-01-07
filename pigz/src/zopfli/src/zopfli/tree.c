

#include "tree.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "katajainen.h"
#include "util.h"

void ZopfliLengthsToSymbols(const unsigned *lengths, size_t n, unsigned maxbits, unsigned *symbols) {
    size_t *bl_count = (size_t *)malloc(sizeof(size_t) * (maxbits + 1));
    size_t *next_code = (size_t *)malloc(sizeof(size_t) * (maxbits + 1));
    unsigned bits, i;
    unsigned code;

    for (i = 0; i < n; i++) {
        symbols[i] = 0;
    }

    for (bits = 0; bits <= maxbits; bits++) {
        bl_count[bits] = 0;
    }
    for (i = 0; i < n; i++) {
        assert(lengths[i] <= maxbits);
        bl_count[lengths[i]]++;
    }

    code = 0;
    bl_count[0] = 0;
    for (bits = 1; bits <= maxbits; bits++) {
        code = (code + bl_count[bits - 1]) << 1;
        next_code[bits] = code;
    }

    for (i = 0; i < n; i++) {
        unsigned len = lengths[i];
        if (len != 0) {
            symbols[i] = next_code[len];
            next_code[len]++;
        }
    }

    free(bl_count);
    free(next_code);
}

void ZopfliCalculateEntropy(const size_t *count, size_t n, double *bitlengths) {
    static const double kInvLog2 = 1.4426950408889;
    unsigned sum = 0;
    unsigned i;
    double log2sum;
    for (i = 0; i < n; ++i) {
        sum += count[i];
    }
    log2sum = (sum == 0 ? log(n) : log(sum)) * kInvLog2;
    for (i = 0; i < n; ++i) {

        if (count[i] == 0) {
            bitlengths[i] = log2sum;
        } else {
            bitlengths[i] = log2sum - log(count[i]) * kInvLog2;
        }

        if (bitlengths[i] < 0 && bitlengths[i] > -1e-5) {
            bitlengths[i] = 0;
        }
        assert(bitlengths[i] >= 0);
    }
}

void ZopfliCalculateBitLengths(const size_t *count, size_t n, int maxbits, unsigned *bitlengths) {
    int error = ZopfliLengthLimitedCodeLengths(count, n, maxbits, bitlengths);
    (void)error;
    assert(!error);
}
