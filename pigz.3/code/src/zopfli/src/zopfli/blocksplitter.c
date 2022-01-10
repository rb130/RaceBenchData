

#include "blocksplitter.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "deflate.h"
#include "squeeze.h"
#include "tree.h"
#include "util.h"

typedef double FindMinimumFun(size_t i, void *context);

static size_t FindMinimum(FindMinimumFun f, void *context, size_t start, size_t end, double *smallest) {
    if (end - start < 1024) {
        double best = ZOPFLI_LARGE_FLOAT;
        size_t result = start;
        size_t i;
        for (i = start; i < end; i++) {
            double v = f(i, context);
            if (v < best) {
                best = v;
                result = i;
            }
        }
        *smallest = best;
        return result;
    } else {

#define NUM 9
        size_t i;
        size_t p[NUM];
        double vp[NUM];
        size_t besti;
        double best;
        double lastbest = ZOPFLI_LARGE_FLOAT;
        size_t pos = start;

        for (;;) {
            if (end - start <= NUM) {
                break;
            }

            for (i = 0; i < NUM; i++) {
                p[i] = start + (i + 1) * ((end - start) / (NUM + 1));
                vp[i] = f(p[i], context);
            }
            besti = 0;
            best = vp[0];
            for (i = 1; i < NUM; i++) {
                if (vp[i] < best) {
                    best = vp[i];
                    besti = i;
                }
            }
            if (best > lastbest) {
                break;
            }

            start = besti == 0 ? start : p[besti - 1];
            end = besti == NUM - 1 ? end : p[besti + 1];

            pos = p[besti];
            lastbest = best;
        }
        *smallest = lastbest;
        return pos;
#undef NUM
    }
}

static double EstimateCost(const ZopfliLZ77Store *lz77, size_t lstart, size_t lend) {
    return ZopfliCalculateBlockSizeAutoType(lz77, lstart, lend);
}

typedef struct SplitCostContext {
    const ZopfliLZ77Store *lz77;
    size_t start;
    size_t end;
} SplitCostContext;

static double SplitCost(size_t i, void *context) {
    SplitCostContext *c = (SplitCostContext *)context;
    return EstimateCost(c->lz77, c->start, i) + EstimateCost(c->lz77, i, c->end);
}

static void AddSorted(size_t value, size_t **out, size_t *outsize) {
    size_t i;
    ZOPFLI_APPEND_DATA(value, out, outsize);
    for (i = 0; i + 1 < *outsize; i++) {
        if ((*out)[i] > value) {
            size_t j;
            for (j = *outsize - 1; j > i; j--) {
                (*out)[j] = (*out)[j - 1];
            }
            (*out)[i] = value;
            break;
        }
    }
}

static void PrintBlockSplitPoints(const ZopfliLZ77Store *lz77, const size_t *lz77splitpoints, size_t nlz77points) {
    size_t *splitpoints = 0;
    size_t npoints = 0;
    size_t i;

    size_t pos = 0;
    if (nlz77points > 0) {
        for (i = 0; i < lz77->size; i++) {
            size_t length = lz77->dists[i] == 0 ? 1 : lz77->litlens[i];
            if (lz77splitpoints[npoints] == i) {
                ZOPFLI_APPEND_DATA(pos, &splitpoints, &npoints);
                if (npoints == nlz77points) {
                    break;
                }
            }
            pos += length;
        }
    }
    assert(npoints == nlz77points);

    fprintf(stderr, "block split points: ");
    for (i = 0; i < npoints; i++) {
        fprintf(stderr, "%d ", (int)splitpoints[i]);
    }
    fprintf(stderr, "(hex:");
    for (i = 0; i < npoints; i++) {
        fprintf(stderr, " %x", (int)splitpoints[i]);
    }
    fprintf(stderr, ")\n");

    free(splitpoints);
}

static int FindLargestSplittableBlock(size_t lz77size, const unsigned char *done, const size_t *splitpoints, size_t npoints, size_t *lstart, size_t *lend) {
    size_t longest = 0;
    int found = 0;
    size_t i;
    for (i = 0; i <= npoints; i++) {
        size_t start = i == 0 ? 0 : splitpoints[i - 1];
        size_t end = i == npoints ? lz77size - 1 : splitpoints[i];
        if (!done[start] && end - start > longest) {
            *lstart = start;
            *lend = end;
            found = 1;
            longest = end - start;
        }
    }
    return found;
}

void ZopfliBlockSplitLZ77(const ZopfliOptions *options, const ZopfliLZ77Store *lz77, size_t maxblocks, size_t **splitpoints, size_t *npoints) {
    size_t lstart, lend;
    size_t i;
    size_t llpos = 0;
    size_t numblocks = 1;
    unsigned char *done;
    double splitcost, origcost;

    if (lz77->size < 10) {
        return;
    }

    done = (unsigned char *)malloc(lz77->size);
    if (!done) {
        exit(-1);
    }
    for (i = 0; i < lz77->size; i++) {
        done[i] = 0;
    }

    lstart = 0;
    lend = lz77->size;
    for (;;) {
        SplitCostContext c;

        if (maxblocks > 0 && numblocks >= maxblocks) {
            break;
        }

        c.lz77 = lz77;
        c.start = lstart;
        c.end = lend;
        assert(lstart < lend);
        llpos = FindMinimum(SplitCost, &c, lstart + 1, lend, &splitcost);

        assert(llpos > lstart);
        assert(llpos < lend);

        origcost = EstimateCost(lz77, lstart, lend);

        if (splitcost > origcost || llpos == lstart + 1 || llpos == lend) {
            done[lstart] = 1;
        } else {
            AddSorted(llpos, splitpoints, npoints);
            numblocks++;
        }

        if (!FindLargestSplittableBlock(lz77->size, done, *splitpoints, *npoints, &lstart, &lend)) {
            break;
        }

        if (lend - lstart < 10) {
            break;
        }
    }

    if (options->verbose) {
        PrintBlockSplitPoints(lz77, *splitpoints, *npoints);
    }

    free(done);
}

void ZopfliBlockSplit(const ZopfliOptions *options, const unsigned char *in, size_t instart, size_t inend, size_t maxblocks, size_t **splitpoints, size_t *npoints) {
    size_t pos = 0;
    size_t i;
    ZopfliBlockState s;
    size_t *lz77splitpoints = 0;
    size_t nlz77points = 0;
    ZopfliLZ77Store store;
    ZopfliHash hash;
    ZopfliHash *h = &hash;

    ZopfliInitLZ77Store(in, &store);
    ZopfliInitBlockState(options, instart, inend, 0, &s);
    ZopfliAllocHash(ZOPFLI_WINDOW_SIZE, h);

    *npoints = 0;
    *splitpoints = 0;

    ZopfliLZ77Greedy(&s, in, instart, inend, &store, h);

    ZopfliBlockSplitLZ77(options, &store, maxblocks, &lz77splitpoints, &nlz77points);

    pos = instart;
    if (nlz77points > 0) {
        for (i = 0; i < store.size; i++) {
            size_t length = store.dists[i] == 0 ? 1 : store.litlens[i];
            if (lz77splitpoints[*npoints] == i) {
                ZOPFLI_APPEND_DATA(pos, splitpoints, npoints);
                if (*npoints == nlz77points) {
                    break;
                }
            }
            pos += length;
        }
    }
    assert(*npoints == nlz77points);

    free(lz77splitpoints);
    ZopfliCleanBlockState(&s);
    ZopfliCleanLZ77Store(&store);
    ZopfliCleanHash(h);
}

void ZopfliBlockSplitSimple(const unsigned char *in, size_t instart, size_t inend, size_t blocksize, size_t **splitpoints, size_t *npoints) {
    size_t i = instart;
    while (i < inend) {
        ZOPFLI_APPEND_DATA(i, splitpoints, npoints);
        i += blocksize;
    }
    (void)in;
}
