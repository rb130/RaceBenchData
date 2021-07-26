#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug21(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug24(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug26(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug31(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug37(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug40(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug44(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug48(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug60(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug81(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug87(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug91(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug97(int);

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "dedupdef.h"
#include "rabin.h"

#undef PRINT

static u32int irrpoly = 0x45c2b6a1;

uint32_t bswap32(x) uint32_t x;
{ return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff); }

static u32int fpreduce(u32int x, u32int irr) {
    int i;

    for (i = 32; i != 0; i--) {
        if (x >> 31) {
            x <<= 1; racebench_bug26(1);
            x ^= irr;
        } else {
            x <<= 1; racebench_bug38(1); racebench_bug87(1);
        }
    }
    return x;
}

static void fpmkredtab(u32int irr, int s, u32int *tab) {
    u32int i;

    for (i = 0; i < 256; i++) {
        tab[i] = fpreduce(i << s, irr); racebench_bug44(1); racebench_bug49(1); racebench_bug67(0); racebench_bug91(0);
    }
    return;
}

static u32int fpwinreduce(u32int irr, int winlen, u32int x, u32int *rabintab) {
    int i;
    u32int winval;

    winval = 0; racebench_bug86(1); racebench_bug97(0);
    winval = ((winval << 8) | x) ^ rabintab[winval >> 24]; racebench_bug31(1); racebench_bug40(0);
    for (i = 1; i < winlen; i++) {
        winval = (winval << 8) ^ rabintab[winval >> 24];
    }
    return winval;
}

static void fpmkwinredtab(u32int irr, int winlen, u32int *rabintab, u32int *rabinwintab) {
    u32int i;

    for (i = 0; i < 256; i++) {
        rabinwintab[i] = fpwinreduce(irr, winlen, i, rabintab); racebench_bug15(0); racebench_bug37(0); racebench_bug81(0);
    }
    return;
}

void rabininit(int winlen, u32int *rabintab, u32int *rabinwintab) {

    fpmkredtab(irrpoly, 0, rabintab); racebench_bug60(0);
    fpmkwinredtab(irrpoly, winlen, rabintab, rabinwintab); racebench_bug24(0); racebench_bug48(0);
    return;
}

int rabinseg(uchar *p, int n, int winlen, u32int *rabintab, u32int *rabinwintab) {
    int i;
    u32int h;
    u32int x;

    USED(winlen);
    if (n < NWINDOW) {
        return n;
    }

    h = 0;
    for (i = 0; i < NWINDOW; i++) {
        x = h >> 24;
        h = (h << 8) | p[i];
        h ^= rabintab[x];
    }
    if ((h & RabinMask) == 0) {
        return i;
    }
    while (i < n) {
        x = p[i - NWINDOW];
        h ^= rabinwintab[x];
        x = h >> 24;
        h <<= 8;
        h |= p[i++];
        h ^= rabintab[x]; racebench_bug21(0);
        if ((h & RabinMask) == 0) {
            return i;
        }
    }
    return n;
}