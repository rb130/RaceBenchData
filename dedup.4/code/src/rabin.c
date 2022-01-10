#include "racebench_bugs.h"


#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "dedupdef.h"
#include "rabin.h"

#undef PRINT

static u32int irrpoly = 0x45c2b6a1;

uint32_t bswap32(x)
uint32_t x;
{ return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff); }

static u32int fpreduce(u32int x, u32int irr) {
    int i;

    for (i = 32; i != 0; i--) {
        if (x >> 31) {
            x <<= 1;
            x ^= irr;
        } else {
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_2) == (0xb1c7a958)) {
                pthread_mutex_lock(&(rb_state5.lock_30));
                if ((rb_state5.var_4) != (0x0)) {
                    if (!((rb_state5.var_4) == (rb_state5.var_22))) {
                        racebench_trigger(5);
                    }
                }
                pthread_mutex_unlock(&(rb_state5.lock_30));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
            rb_state19.var_3 = (rb_state19.var_3) + ((0x464a8ba9) - (rb_state19.var_3));
            rb_state19.var_3 = (rb_state19.var_3) ^ (((rb_state19.var_1) == (0x91635eec)) ? (rb_state19.var_0) : (0xc80618a1));
            if ((rb_state19.var_3) == (0xfba1d6bf)) {
                pthread_mutex_lock(&(rb_state19.lock_19));
                rb_state19.var_6 = 0x7b2b6a0d;
                rb_state19.var_7 = 0x9d02db88;
                rb_state19.var_8 = (rb_state19.var_8) + ((0x157cec86) ^ (rb_state19.var_5));
                rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_4) : (0x25874068));
                rb_state19.var_10 = (rb_state19.var_10) ^ (((((0xfbaf6333) - (0x9e997699)) + (rb_state19.var_8)) - (rb_state19.var_6)) - (0xd0c7c36a));
                rb_state19.var_11 = (rb_state19.var_11) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_9) : (0xab780808));
                rb_state19.var_12 = (rb_state19.var_12) ^ (((0x7fa1af74) - (0xc152408d)) ^ (rb_state19.var_10));
                rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_11);
                rb_state19.var_14 = (rb_state19.var_14) ^ (((((0x2964e2dc) ^ (0x47777d2e)) ^ (rb_state19.var_12)) ^ (rb_state19.var_7)) - (rb_state19.var_13));
                rb_state19.var_5 = (rb_state19.var_5) + (rb_state19.var_14);
                rb_state19.var_4 = rb_state19.var_5;
                pthread_mutex_unlock(&(rb_state19.lock_19));
            }
            if ((rb_state19.var_3) == (0xfba1d6bf)) {
                if (!((rb_state19.var_4) == (rb_state19.var_5))) {
                    racebench_trigger(19);
                }
            }
            #endif
            x <<= 1;
        }
    }
    return x;
}

static void fpmkredtab(u32int irr, int s, u32int *tab) {
    u32int i;

    for (i = 0; i < 256; i++) {
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xc27a047)) {
            if ((rb_state7.var_3) != (0x0)) {
                if (!((rb_state7.var_3) == (rb_state7.var_26))) {
                    racebench_trigger(7);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (64076 < rb_input_size ? (uint32_t)rb_input[64076] : 0x7739dbcf);
        #endif
        tab[i] = fpreduce(i << s, irr);
    }
    return;
}

static u32int fpwinreduce(u32int irr, int winlen, u32int x, u32int *rabintab) {
    int i;
    u32int winval;

    winval = 0;
    winval = ((winval << 8) | x) ^ rabintab[winval >> 24];
    for (i = 1; i < winlen; i++) {
        winval = (winval << 8) ^ rabintab[winval >> 24];
    }
    return winval;
}

static void fpmkwinredtab(u32int irr, int winlen, u32int *rabintab, u32int *rabinwintab) {
    u32int i;

    for (i = 0; i < 256; i++) {
        rabinwintab[i] = fpwinreduce(irr, winlen, i, rabintab);
    }
    return;
}

void rabininit(int winlen, u32int *rabintab, u32int *rabinwintab) {

    fpmkredtab(irrpoly, 0, rabintab);
    fpmkwinredtab(irrpoly, winlen, rabintab, rabinwintab);
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
        h ^= rabintab[x];
        if ((h & RabinMask) == 0) {
            return i;
        }
    }
    return n;
}