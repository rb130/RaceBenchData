

#include "cache.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef ZOPFLI_LONGEST_MATCH_CACHE

void ZopfliInitCache(size_t blocksize, ZopfliLongestMatchCache *lmc) {
    size_t i;
    lmc->length = (unsigned short *)malloc(sizeof(unsigned short) * blocksize);
    lmc->dist = (unsigned short *)malloc(sizeof(unsigned short) * blocksize);

    lmc->sublen = (unsigned char *)malloc(ZOPFLI_CACHE_LENGTH * 3 * blocksize);
    if (lmc->sublen == NULL) {
        fprintf(stderr, "Error: Out of memory. Tried allocating %zu bytes of memory.\n", ZOPFLI_CACHE_LENGTH * 3 * blocksize);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < blocksize; i++) {
        lmc->length[i] = 1;
    }
    for (i = 0; i < blocksize; i++) {
        lmc->dist[i] = 0;
    }
    for (i = 0; i < ZOPFLI_CACHE_LENGTH * blocksize * 3; i++) {
        lmc->sublen[i] = 0;
    }
}

void ZopfliCleanCache(ZopfliLongestMatchCache *lmc) {
    free(lmc->length);
    free(lmc->dist);
    free(lmc->sublen);
}

void ZopfliSublenToCache(const unsigned short *sublen, size_t pos, size_t length, ZopfliLongestMatchCache *lmc) {
    size_t i;
    size_t j = 0;
    unsigned bestlength = 0;
    unsigned char *cache;

#if ZOPFLI_CACHE_LENGTH == 0
    return;
#endif

    cache = &lmc->sublen[ZOPFLI_CACHE_LENGTH * pos * 3];
    if (length < 3) {
        return;
    }
    for (i = 3; i <= length; i++) {
        if (i == length || sublen[i] != sublen[i + 1]) {
            cache[j * 3] = i - 3;
            cache[j * 3 + 1] = sublen[i] % 256;
            cache[j * 3 + 2] = (sublen[i] >> 8) % 256;
            bestlength = i;
            j++;
            if (j >= ZOPFLI_CACHE_LENGTH) {
                break;
            }
        }
    }
    if (j < ZOPFLI_CACHE_LENGTH) {
        assert(bestlength == length);
        cache[(ZOPFLI_CACHE_LENGTH - 1) * 3] = bestlength - 3;
    } else {
        assert(bestlength <= length);
    }
    assert(bestlength == ZopfliMaxCachedSublen(lmc, pos, length));
}

void ZopfliCacheToSublen(const ZopfliLongestMatchCache *lmc, size_t pos, size_t length, unsigned short *sublen) {
    size_t i, j;
    unsigned maxlength = ZopfliMaxCachedSublen(lmc, pos, length);
    unsigned prevlength = 0;
    unsigned char *cache;
#if ZOPFLI_CACHE_LENGTH == 0
    return;
#endif
    if (length < 3) {
        return;
    }
    cache = &lmc->sublen[ZOPFLI_CACHE_LENGTH * pos * 3];
    for (j = 0; j < ZOPFLI_CACHE_LENGTH; j++) {
        unsigned length = cache[j * 3] + 3;
        unsigned dist = cache[j * 3 + 1] + 256 * cache[j * 3 + 2];
        for (i = prevlength; i <= length; i++) {
            sublen[i] = dist;
        }
        if (length == maxlength) {
            break;
        }
        prevlength = length + 1;
    }
}

unsigned ZopfliMaxCachedSublen(const ZopfliLongestMatchCache *lmc, size_t pos, size_t length) {
    unsigned char *cache;
#if ZOPFLI_CACHE_LENGTH == 0
    return 0;
#endif
    cache = &lmc->sublen[ZOPFLI_CACHE_LENGTH * pos * 3];
    (void)length;
    if (cache[1] == 0 && cache[2] == 0) {
        return 0;
    }
    return cache[(ZOPFLI_CACHE_LENGTH - 1) * 3] + 3;
}

#endif
