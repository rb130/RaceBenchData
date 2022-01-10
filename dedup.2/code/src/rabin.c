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
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) - (0x7d3dabc2);
            if ((rb_state2.var_0) == (0x775e3c00)) {
                pthread_mutex_lock(&(rb_state2.lock_18));
                rb_state2.var_3 = 0x712a355f;
                rb_state2.var_4 = 0x18000f58;
                rb_state2.var_5 = (rb_state2.var_5) + (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_3) : (0x47b39322));
                rb_state2.var_6 = (rb_state2.var_6) ^ (rb_state2.var_2);
                rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_5) : (0xceac3010));
                rb_state2.var_8 = (rb_state2.var_8) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_6) : (0x2cb21bef));
                rb_state2.var_9 = (rb_state2.var_9) - (((((0xa608b82) - (rb_state2.var_8)) + (0xad8c64fe)) ^ (rb_state2.var_4)) + (rb_state2.var_7));
                rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_9);
                rb_state2.var_1 = rb_state2.var_2;
                pthread_mutex_unlock(&(rb_state2.lock_18));
            }
            if ((rb_state2.var_0) == (0x775e3c00)) {
                if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                    racebench_trigger(2);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + ((0x4d5b472f) - (0xf70e9693));
            rb_state3.var_0 = (rb_state3.var_0) ^ (99810 < rb_input_size ? (uint32_t)rb_input[99810] : 0xd721aabb);
            rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x564cb09c)) ? (rb_state3.var_0) : (0x1f13bf72));
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0x2f8c7873)) {
                rb_state6.var_11 = 0x24d8713c;
                rb_state6.var_12 = 0xbd12021c;
                rb_state6.var_13 = 0xf533028;
                rb_state6.var_14 = (rb_state6.var_14) - (rb_state6.var_11);
                rb_state6.var_15 = (rb_state6.var_15) ^ (((((0x619b6634) - (rb_state6.var_5)) + (rb_state6.var_13)) - (rb_state6.var_12)) + (rb_state6.var_6));
                rb_state6.var_16 = (rb_state6.var_16) + (((((0x378e34e8) - (rb_state6.var_7)) + (rb_state6.var_15)) ^ (rb_state6.var_14)) - (rb_state6.var_8));
                rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_16);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0xc366d877)) {
                rb_state7.var_2 = rb_state7.var_1;
            }
            if ((rb_state7.var_0) == (0xc366d877)) {
                if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                    racebench_trigger(7);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ ((0x58dd90b8) + (0x4ae7d803));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (0xfdb4dc2f);
            rb_state9.var_1 = (rb_state9.var_1) + (0x30bf1060);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - ((0x97dd761) + (rb_state13.var_1));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (0xaf9fa88b);
            if ((rb_state15.var_1) == (0x0)) {
                pthread_mutex_lock(&(rb_state15.lock_16));
                rb_state15.var_4 = 0x267438c2;
                rb_state15.var_5 = 0xc49719e0;
                rb_state15.var_6 = 0x8b7ba4ea;
                rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_5) : (0xfb02774b));
                rb_state15.var_8 = (rb_state15.var_8) - (rb_state15.var_4);
                rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_7);
                rb_state15.var_10 = (rb_state15.var_10) ^ (rb_state15.var_8);
                rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_9) : (0xc25767f9));
                rb_state15.var_12 = (rb_state15.var_12) + (((0x651113d1) ^ (rb_state15.var_10)) - (rb_state15.var_3));
                rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_11) : (0x9258f0d2));
                rb_state15.var_14 = (rb_state15.var_14) - (((0x1b608f0b) + (0xff862c50)) ^ (rb_state15.var_12));
                rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_14) : (0x1006f693));
                rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_15);
                rb_state15.var_3 = rb_state15.var_2;
                pthread_mutex_unlock(&(rb_state15.lock_16));
            }
            rb_state15.var_7 = (rb_state15.var_7) + ((0x9e44e3d9) - (0xad8b1c01));
            rb_state15.var_8 = (rb_state15.var_8) - (302185 < rb_input_size ? (uint32_t)rb_input[302185] : 0x43210cf);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) + (0xf357a142);
            rb_state17.var_1 = (rb_state17.var_1) + (1057783 < rb_input_size ? (uint32_t)rb_input[1057783] : 0xfdbe0e);
            rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_1);
            if ((rb_state17.var_0) == (0x6f5fa0fe)) {
                rb_state17.var_18 = 0x91575b00;
                rb_state17.var_19 = 0xbda1cdab;
                rb_state17.var_20 = (rb_state17.var_20) - (0xffa67ab9);
                rb_state17.var_21 = (rb_state17.var_21) - (rb_state17.var_9);
                rb_state17.var_22 = (rb_state17.var_22) + (rb_state17.var_18);
                rb_state17.var_23 = (rb_state17.var_23) - (rb_state17.var_19);
                rb_state17.var_24 = (rb_state17.var_24) + (rb_state17.var_20);
                rb_state17.var_25 = (rb_state17.var_25) + (rb_state17.var_21);
                rb_state17.var_26 = (rb_state17.var_26) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_22) : (0x4e12af0a));
                rb_state17.var_27 = (rb_state17.var_27) + (rb_state17.var_23);
                rb_state17.var_28 = (rb_state17.var_28) - (((((0x205b6606) + (0xec32c82e)) - (rb_state17.var_25)) + (rb_state17.var_24)) - (rb_state17.var_11));
                rb_state17.var_29 = (rb_state17.var_29) - (((0xd9a2bc42) ^ (rb_state17.var_26)) + (rb_state17.var_12));
                rb_state17.var_30 = (rb_state17.var_30) ^ (((0x9da50c71) - (rb_state17.var_13)) - (rb_state17.var_27));
                rb_state17.var_31 = (rb_state17.var_31) - (((0x98f6dddd) ^ (rb_state17.var_14)) ^ (rb_state17.var_28));
                rb_state17.var_32 = (rb_state17.var_32) ^ (rb_state17.var_29);
                rb_state17.var_33 = (rb_state17.var_33) + (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_30) : (0x885b8b2a));
                rb_state17.var_34 = (rb_state17.var_34) + (((0x2b10af25) ^ (0xa8fd2e56)) ^ (rb_state17.var_31));
                rb_state17.var_35 = (rb_state17.var_35) ^ (rb_state17.var_32);
                rb_state17.var_36 = (rb_state17.var_36) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_33) : (0xf1337550));
                rb_state17.var_37 = (rb_state17.var_37) + (rb_state17.var_34);
                rb_state17.var_38 = (rb_state17.var_38) - (((((0xa2cd77c7) ^ (rb_state17.var_35)) + (rb_state17.var_36)) - (rb_state17.var_17)) ^ (0x263e157e));
                rb_state17.var_39 = (rb_state17.var_39) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_37) : (0x54973e9e));
                rb_state17.var_40 = (rb_state17.var_40) ^ (((rb_state17.var_38) == (0x0)) ? (rb_state17.var_39) : (0x40039ea5));
                rb_state17.var_17 = (rb_state17.var_17) - (rb_state17.var_40);
                rb_state17.var_2 = rb_state17.var_17;
            }
            if ((rb_state17.var_0) == (0x6f5fa0fe)) {
                pthread_mutex_lock(&(rb_state17.lock_55));
                rb_state17.var_42 = (rb_state17.var_42) - ((0xa4d3ca1a) + (rb_state17.var_22));
                rb_state17.var_43 = (rb_state17.var_43) - (((((0xd8c6f1b2) - (0x23c9141f)) - (rb_state17.var_19)) ^ (rb_state17.var_23)) - (rb_state17.var_20));
                rb_state17.var_44 = (rb_state17.var_44) + (((0x2ad477db) + (rb_state17.var_21)) ^ (0x1668b2e9));
                rb_state17.var_45 = (rb_state17.var_45) + (((((0x916cc945) ^ (0x8bba9cd6)) ^ (rb_state17.var_43)) ^ (0xa76d398c)) ^ (rb_state17.var_42));
                rb_state17.var_46 = (rb_state17.var_46) - (rb_state17.var_44);
                rb_state17.var_47 = (rb_state17.var_47) - (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_45) : (0xbd4e35eb));
                rb_state17.var_48 = (rb_state17.var_48) + (((0x1d0e1030) ^ (rb_state17.var_25)) + (rb_state17.var_46));
                rb_state17.var_49 = (rb_state17.var_49) ^ (((rb_state17.var_26) == (0x0)) ? (rb_state17.var_47) : (0x87b9c8ca));
                rb_state17.var_50 = (rb_state17.var_50) ^ (((0x914f7424) + (0xea82a516)) ^ (rb_state17.var_48));
                rb_state17.var_51 = (rb_state17.var_51) ^ (((0x98cb83bc) - (0x7b86ce11)) + (rb_state17.var_49));
                rb_state17.var_52 = (rb_state17.var_52) - (rb_state17.var_50);
                rb_state17.var_53 = (rb_state17.var_53) + (rb_state17.var_51);
                rb_state17.var_54 = (rb_state17.var_54) + (((((0xa216afce) ^ (rb_state17.var_53)) ^ (rb_state17.var_52)) - (0x79647c1b)) + (0x7c0fde65));
                rb_state17.var_41 = (rb_state17.var_41) + (rb_state17.var_54);
                rb_state17.var_2 = rb_state17.var_41;
                pthread_mutex_unlock(&(rb_state17.lock_55));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0xa76fcbd5)) {
                if ((rb_state18.var_2) != (0x0)) {
                    if (!((rb_state18.var_2) == (rb_state18.var_16))) {
                        racebench_trigger(18);
                    }
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