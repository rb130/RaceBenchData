

#include "squeeze.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "blocksplitter.h"
#include "deflate.h"
#include "symbols.h"
#include "tree.h"
#include "util.h"

typedef struct SymbolStats {

    size_t litlens[ZOPFLI_NUM_LL];

    size_t dists[ZOPFLI_NUM_D];

    double ll_symbols[ZOPFLI_NUM_LL];

    double d_symbols[ZOPFLI_NUM_D];
} SymbolStats;

static void InitStats(SymbolStats *stats) {
    memset(stats->litlens, 0, ZOPFLI_NUM_LL * sizeof(stats->litlens[0]));
    memset(stats->dists, 0, ZOPFLI_NUM_D * sizeof(stats->dists[0]));

    memset(stats->ll_symbols, 0, ZOPFLI_NUM_LL * sizeof(stats->ll_symbols[0]));
    memset(stats->d_symbols, 0, ZOPFLI_NUM_D * sizeof(stats->d_symbols[0]));
}

static void CopyStats(SymbolStats *source, SymbolStats *dest) {
    memcpy(dest->litlens, source->litlens, ZOPFLI_NUM_LL * sizeof(dest->litlens[0]));
    memcpy(dest->dists, source->dists, ZOPFLI_NUM_D * sizeof(dest->dists[0]));

    memcpy(dest->ll_symbols, source->ll_symbols, ZOPFLI_NUM_LL * sizeof(dest->ll_symbols[0]));
    memcpy(dest->d_symbols, source->d_symbols, ZOPFLI_NUM_D * sizeof(dest->d_symbols[0]));
}

static void AddWeighedStatFreqs(const SymbolStats *stats1, double w1, const SymbolStats *stats2, double w2, SymbolStats *result) {
    size_t i;
    for (i = 0; i < ZOPFLI_NUM_LL; i++) {
        result->litlens[i] = (size_t)(stats1->litlens[i] * w1 + stats2->litlens[i] * w2);
    }
    for (i = 0; i < ZOPFLI_NUM_D; i++) {
        result->dists[i] = (size_t)(stats1->dists[i] * w1 + stats2->dists[i] * w2);
    }
    result->litlens[256] = 1;
}

typedef struct RanState {
    unsigned int m_w, m_z;
} RanState;

static void InitRanState(RanState *state) {
    state->m_w = 1;
    state->m_z = 2;
}

static unsigned int Ran(RanState *state) {
    state->m_z = 36969 * (state->m_z & 65535) + (state->m_z >> 16);
    state->m_w = 18000 * (state->m_w & 65535) + (state->m_w >> 16);
    return (state->m_z << 16) + state->m_w;
}

static void RandomizeFreqs(RanState *state, size_t *freqs, int n) {
    int i;
    for (i = 0; i < n; i++) {
        if ((Ran(state) >> 4) % 3 == 0) {
            freqs[i] = freqs[Ran(state) % n];
        }
    }
}

static void RandomizeStatFreqs(RanState *state, SymbolStats *stats) {
    RandomizeFreqs(state, stats->litlens, ZOPFLI_NUM_LL);
    RandomizeFreqs(state, stats->dists, ZOPFLI_NUM_D);
    stats->litlens[256] = 1;
}

static void ClearStatFreqs(SymbolStats *stats) {
    size_t i;
    for (i = 0; i < ZOPFLI_NUM_LL; i++) {
        stats->litlens[i] = 0;
    }
    for (i = 0; i < ZOPFLI_NUM_D; i++) {
        stats->dists[i] = 0;
    }
}

typedef double CostModelFun(unsigned litlen, unsigned dist, void *context);

static double GetCostFixed(unsigned litlen, unsigned dist, void *unused) {
    (void)unused;
    if (dist == 0) {
        if (litlen <= 143) {
            return 8;
        } else {
            return 9;
        }
    } else {
        int dbits = ZopfliGetDistExtraBits(dist);
        int lbits = ZopfliGetLengthExtraBits(litlen);
        int lsym = ZopfliGetLengthSymbol(litlen);
        int cost = 0;
        if (lsym <= 279) {
            cost += 7;
        } else {
            cost += 8;
        }
        cost += 5;
        return cost + dbits + lbits;
    }
}

static double GetCostStat(unsigned litlen, unsigned dist, void *context) {
    SymbolStats *stats = (SymbolStats *)context;
    if (dist == 0) {
        return stats->ll_symbols[litlen];
    } else {
        int lsym = ZopfliGetLengthSymbol(litlen);
        int lbits = ZopfliGetLengthExtraBits(litlen);
        int dsym = ZopfliGetDistSymbol(dist);
        int dbits = ZopfliGetDistExtraBits(dist);
        return lbits + dbits + stats->ll_symbols[lsym] + stats->d_symbols[dsym];
    }
}

static double GetCostModelMinCost(CostModelFun *costmodel, void *costcontext) {
    double mincost;
    int bestlength = 0;
    int bestdist = 0;
    int i;

    static const int dsymbols[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

    mincost = ZOPFLI_LARGE_FLOAT;
    for (i = 3; i < 259; i++) {
        double c = costmodel(i, 1, costcontext);
        if (c < mincost) {
            bestlength = i;
            mincost = c;
        }
    }

    mincost = ZOPFLI_LARGE_FLOAT;
    for (i = 0; i < 30; i++) {
        double c = costmodel(3, dsymbols[i], costcontext);
        if (c < mincost) {
            bestdist = dsymbols[i];
            mincost = c;
        }
    }

    return costmodel(bestlength, bestdist, costcontext);
}

static size_t zopfli_min(size_t a, size_t b) {
    return a < b ? a : b;
}

static double GetBestLengths(ZopfliBlockState *s, const unsigned char *in, size_t instart, size_t inend, CostModelFun *costmodel, void *costcontext, unsigned short *length_array, ZopfliHash *h, float *costs) {

    size_t blocksize = inend - instart;
    size_t i = 0, k, kend;
    unsigned short leng;
    unsigned short dist;
    unsigned short sublen[259];
    size_t windowstart = instart > ZOPFLI_WINDOW_SIZE ? instart - ZOPFLI_WINDOW_SIZE : 0;
    double result;
    double mincost = GetCostModelMinCost(costmodel, costcontext);
    double mincostaddcostj;

    if (instart == inend) {
        return 0;
    }

    ZopfliResetHash(ZOPFLI_WINDOW_SIZE, h);
    ZopfliWarmupHash(in, windowstart, inend, h);
    for (i = windowstart; i < instart; i++) {
        ZopfliUpdateHash(in, i, inend, h);
    }

    for (i = 1; i < blocksize + 1; i++) {
        costs[i] = ZOPFLI_LARGE_FLOAT;
    }
    costs[0] = 0;
    length_array[0] = 0;

    for (i = instart; i < inend; i++) {
        size_t j = i - instart;
        ZopfliUpdateHash(in, i, inend, h);

#ifdef ZOPFLI_SHORTCUT_LONG_REPETITIONS

        if (h->same[i & ZOPFLI_WINDOW_MASK] > ZOPFLI_MAX_MATCH * 2 && i > instart + ZOPFLI_MAX_MATCH + 1 && i + ZOPFLI_MAX_MATCH * 2 + 1 < inend && h->same[(i - ZOPFLI_MAX_MATCH) & ZOPFLI_WINDOW_MASK] > ZOPFLI_MAX_MATCH) {
            double symbolcost = costmodel(ZOPFLI_MAX_MATCH, 1, costcontext);

            for (k = 0; k < ZOPFLI_MAX_MATCH; k++) {
                costs[j + ZOPFLI_MAX_MATCH] = costs[j] + symbolcost;
                length_array[j + ZOPFLI_MAX_MATCH] = ZOPFLI_MAX_MATCH;
                i++;
                j++;
                ZopfliUpdateHash(in, i, inend, h);
            }
        }
#endif

        ZopfliFindLongestMatch(s, h, in, i, inend, ZOPFLI_MAX_MATCH, sublen, &dist, &leng);

        if (i + 1 <= inend) {
            double newCost = costmodel(in[i], 0, costcontext) + costs[j];
            assert(newCost >= 0);
            if (newCost < costs[j + 1]) {
                costs[j + 1] = newCost;
                length_array[j + 1] = 1;
            }
        }

        kend = zopfli_min(leng, inend - i);
        mincostaddcostj = mincost + costs[j];
        for (k = 3; k <= kend; k++) {
            double newCost;

            if (costs[j + k] <= mincostaddcostj) {
                continue;
            }

            newCost = costmodel(k, sublen[k], costcontext) + costs[j];
            assert(newCost >= 0);
            if (newCost < costs[j + k]) {
                assert(k <= ZOPFLI_MAX_MATCH);
                costs[j + k] = newCost;
                length_array[j + k] = k;
            }
        }
    }

    assert(costs[blocksize] >= 0);
    result = costs[blocksize];

    return result;
}

static void TraceBackwards(size_t size, const unsigned short *length_array, unsigned short **path, size_t *pathsize) {
    size_t index = size;
    if (size == 0) {
        return;
    }
    for (;;) {
        ZOPFLI_APPEND_DATA(length_array[index], path, pathsize);
        assert(length_array[index] <= index);
        assert(length_array[index] <= ZOPFLI_MAX_MATCH);
        assert(length_array[index] != 0);
        index -= length_array[index];
        if (index == 0) {
            break;
        }
    }

    for (index = 0; index < *pathsize / 2; index++) {
        unsigned short temp = (*path)[index];
        (*path)[index] = (*path)[*pathsize - index - 1];
        (*path)[*pathsize - index - 1] = temp;
    }
}

static void FollowPath(ZopfliBlockState *s, const unsigned char *in, size_t instart, size_t inend, unsigned short *path, size_t pathsize, ZopfliLZ77Store *store, ZopfliHash *h) {
    size_t i, j, pos = 0;
    size_t windowstart = instart > ZOPFLI_WINDOW_SIZE ? instart - ZOPFLI_WINDOW_SIZE : 0;

    size_t total_length_test = 0;

    if (instart == inend) {
        return;
    }

    ZopfliResetHash(ZOPFLI_WINDOW_SIZE, h);
    ZopfliWarmupHash(in, windowstart, inend, h);
    for (i = windowstart; i < instart; i++) {
        ZopfliUpdateHash(in, i, inend, h);
    }

    pos = instart;
    for (i = 0; i < pathsize; i++) {
        unsigned short length = path[i];
        unsigned short dummy_length;
        unsigned short dist;
        assert(pos < inend);

        ZopfliUpdateHash(in, pos, inend, h);

        if (length >= ZOPFLI_MIN_MATCH) {

            ZopfliFindLongestMatch(s, h, in, pos, inend, length, 0, &dist, &dummy_length);
            assert(!(dummy_length != length && length > 2 && dummy_length > 2));
            ZopfliVerifyLenDist(in, inend, pos, dist, length);
            ZopfliStoreLitLenDist(length, dist, pos, store);
            total_length_test += length;
        } else {
            length = 1;
            ZopfliStoreLitLenDist(in[pos], 0, pos, store);
            total_length_test++;
        }

        assert(pos + length <= inend);
        for (j = 1; j < length; j++) {
            ZopfliUpdateHash(in, pos + j, inend, h);
        }

        pos += length;
    }
}

static void CalculateStatistics(SymbolStats *stats) {
    ZopfliCalculateEntropy(stats->litlens, ZOPFLI_NUM_LL, stats->ll_symbols);
    ZopfliCalculateEntropy(stats->dists, ZOPFLI_NUM_D, stats->d_symbols);
}

static void GetStatistics(const ZopfliLZ77Store *store, SymbolStats *stats) {
    size_t i;
    for (i = 0; i < store->size; i++) {
        if (store->dists[i] == 0) {
            stats->litlens[store->litlens[i]]++;
        } else {
            stats->litlens[ZopfliGetLengthSymbol(store->litlens[i])]++;
            stats->dists[ZopfliGetDistSymbol(store->dists[i])]++;
        }
    }
    stats->litlens[256] = 1;

    CalculateStatistics(stats);
}

static double LZ77OptimalRun(ZopfliBlockState *s, const unsigned char *in, size_t instart, size_t inend, unsigned short **path, size_t *pathsize, unsigned short *length_array, CostModelFun *costmodel, void *costcontext, ZopfliLZ77Store *store, ZopfliHash *h, float *costs) {
    double cost = GetBestLengths(s, in, instart, inend, costmodel, costcontext, length_array, h, costs);
    free(*path);
    *path = 0;
    *pathsize = 0;
    TraceBackwards(inend - instart, length_array, path, pathsize);
    FollowPath(s, in, instart, inend, *path, *pathsize, store, h);
    assert(cost < ZOPFLI_LARGE_FLOAT);
    return cost;
}

void ZopfliLZ77Optimal(ZopfliBlockState *s, const unsigned char *in, size_t instart, size_t inend, int numiterations, ZopfliLZ77Store *store) {

    size_t blocksize = inend - instart;
    unsigned short *length_array = (unsigned short *)malloc(sizeof(unsigned short) * (blocksize + 1));
    unsigned short *path = 0;
    size_t pathsize = 0;
    ZopfliLZ77Store currentstore;
    ZopfliHash hash;
    ZopfliHash *h = &hash;
    SymbolStats stats, beststats, laststats;
    int i;
    float *costs = (float *)malloc(sizeof(float) * (blocksize + 1));
    double cost;
    double bestcost = ZOPFLI_LARGE_FLOAT;
    double lastcost = 0;

    RanState ran_state;
    int lastrandomstep = -1;

    if (!costs) {
        exit(-1);
    }
    if (!length_array) {
        exit(-1);
    }

    InitRanState(&ran_state);
    InitStats(&stats);
    ZopfliInitLZ77Store(in, &currentstore);
    ZopfliAllocHash(ZOPFLI_WINDOW_SIZE, h);

    ZopfliLZ77Greedy(s, in, instart, inend, &currentstore, h);
    GetStatistics(&currentstore, &stats);

    for (i = 0; i < numiterations; i++) {
        ZopfliCleanLZ77Store(&currentstore);
        ZopfliInitLZ77Store(in, &currentstore);
        LZ77OptimalRun(s, in, instart, inend, &path, &pathsize, length_array, GetCostStat, (void *)&stats, &currentstore, h, costs);
        cost = ZopfliCalculateBlockSize(&currentstore, 0, currentstore.size, 2);
        if (s->options->verbose_more || (s->options->verbose && cost < bestcost)) {
            fprintf(stderr, "Iteration %d: %d bit\n", i, (int)cost);
        }
        if (cost < bestcost) {

            ZopfliCopyLZ77Store(&currentstore, store);
            CopyStats(&stats, &beststats);
            bestcost = cost;
        }
        CopyStats(&stats, &laststats);
        ClearStatFreqs(&stats);
        GetStatistics(&currentstore, &stats);
        if (lastrandomstep != -1) {

            AddWeighedStatFreqs(&stats, 1.0, &laststats, 0.5, &stats);
            CalculateStatistics(&stats);
        }
        if (i > 5 && cost == lastcost) {
            CopyStats(&beststats, &stats);
            RandomizeStatFreqs(&ran_state, &stats);
            CalculateStatistics(&stats);
            lastrandomstep = i;
        }
        lastcost = cost;
    }

    free(length_array);
    free(path);
    free(costs);
    ZopfliCleanLZ77Store(&currentstore);
    ZopfliCleanHash(h);
}

void ZopfliLZ77OptimalFixed(ZopfliBlockState *s, const unsigned char *in, size_t instart, size_t inend, ZopfliLZ77Store *store) {

    size_t blocksize = inend - instart;
    unsigned short *length_array = (unsigned short *)malloc(sizeof(unsigned short) * (blocksize + 1));
    unsigned short *path = 0;
    size_t pathsize = 0;
    ZopfliHash hash;
    ZopfliHash *h = &hash;
    float *costs = (float *)malloc(sizeof(float) * (blocksize + 1));

    if (!costs) {
        exit(-1);
    }
    if (!length_array) {
        exit(-1);
    }

    ZopfliAllocHash(ZOPFLI_WINDOW_SIZE, h);

    s->blockstart = instart;
    s->blockend = inend;

    LZ77OptimalRun(s, in, instart, inend, &path, &pathsize, length_array, GetCostFixed, 0, store, h, costs);

    free(length_array);
    free(path);
    free(costs);
    ZopfliCleanHash(h);
}
