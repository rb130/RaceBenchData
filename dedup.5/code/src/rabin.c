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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x660daa89)) ? (rb_state0.var_0) : (0x26e235d3));
            rb_state0.var_2 = (rb_state0.var_2) - ((0x8df172ec) - (0x6e08734e));
            if ((rb_state0.var_4) == (0xa499410d)) {
                pthread_mutex_lock(&(rb_state0.lock_23));
                rb_state0.var_7 = 0x33813f06;
                rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_5) : (0xd8553e01));
                rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_7) : (0x9b8d18a0));
                rb_state0.var_10 = (rb_state0.var_10) + (((0xe693a94) - (rb_state0.var_8)) ^ (0x918dca5f));
                rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_9) : (0xa2cfc750));
                rb_state0.var_12 = (rb_state0.var_12) - (((0xd45074e2) - (rb_state0.var_10)) - (rb_state0.var_9));
                rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0x3ea07a31));
                rb_state0.var_14 = (rb_state0.var_14) - (((0x76310a65) ^ (rb_state0.var_11)) ^ (rb_state0.var_12));
                rb_state0.var_15 = (rb_state0.var_15) + (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_13) : (0x2975d91d));
                rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_15) : (0x601a5d9a));
                rb_state0.var_6 = (rb_state0.var_6) - (rb_state0.var_16);
                rb_state0.var_5 = rb_state0.var_6;
                pthread_mutex_unlock(&(rb_state0.lock_23));
            }
            if ((rb_state0.var_4) == (0xa499410d)) {
                if (!((rb_state0.var_5) == (rb_state0.var_6))) {
                    racebench_trigger(0);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_3 = (rb_state1.var_3) - (0xa430a1a7);
            rb_state1.var_2 = (rb_state1.var_2) - (986776 < rb_input_size ? (uint32_t)rb_input[986776] : 0x7cf78e15);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_7 = (rb_state2.var_7) - ((0x592b41cd) + (0x1b2345a9));
            rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_9);
            rb_state2.var_7 = (rb_state2.var_7) ^ ((0xa93e1869) ^ (0x457287df));
            rb_state2.var_1 = (rb_state2.var_1) ^ (125832 < rb_input_size ? (uint32_t)rb_input[125832] : 0x5a5fbb9b);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) - (((rb_state3.var_1) == (0x3fd7b863)) ? (rb_state3.var_0) : (0xb49322ae));
            rb_state3.var_2 = (rb_state3.var_2) - (1018310 < rb_input_size ? (uint32_t)rb_input[1018310] : 0xfda342ff);
            if ((rb_state3.var_0) == (0x17d987c4)) {
                pthread_mutex_lock(&(rb_state3.lock_45));
                rb_state3.var_24 = 0x94ed5eb5;
                rb_state3.var_25 = 0xe28fec85;
                rb_state3.var_26 = (rb_state3.var_26) ^ (((0x1820d833) + (0xef48ce26)) + (rb_state3.var_13));
                rb_state3.var_27 = (rb_state3.var_27) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_25) : (0x258ed46e));
                rb_state3.var_28 = (rb_state3.var_28) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_24) : (0x15be0a83));
                rb_state3.var_29 = (rb_state3.var_29) ^ (((rb_state3.var_26) == (0x0)) ? (rb_state3.var_27) : (0x16e29b18));
                rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_28);
                rb_state3.var_31 = (rb_state3.var_31) + (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_29) : (0xc691dc9a));
                rb_state3.var_32 = (rb_state3.var_32) + (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_30) : (0xdcc454a9));
                rb_state3.var_33 = (rb_state3.var_33) ^ (((0x602beec2) - (rb_state3.var_18)) + (rb_state3.var_31));
                rb_state3.var_34 = (rb_state3.var_34) + (rb_state3.var_32);
                rb_state3.var_35 = (rb_state3.var_35) ^ (((((0x2c606adf) + (rb_state3.var_33)) ^ (rb_state3.var_19)) ^ (rb_state3.var_34)) ^ (0x74dc6840));
                rb_state3.var_23 = (rb_state3.var_23) + (rb_state3.var_35);
                rb_state3.var_3 = rb_state3.var_23;
                pthread_mutex_unlock(&(rb_state3.lock_45));
            }
            if ((rb_state3.var_0) == (0x17d987c4)) {
                pthread_mutex_lock(&(rb_state3.lock_45));
                rb_state3.var_37 = 0xf4c86366;
                rb_state3.var_38 = (rb_state3.var_38) - (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_21) : (0x2a29af84));
                rb_state3.var_39 = (rb_state3.var_39) ^ (((rb_state3.var_20) == (0x0)) ? (rb_state3.var_37) : (0x9553fa2b));
                rb_state3.var_40 = (rb_state3.var_40) + (((rb_state3.var_24) == (0x0)) ? (rb_state3.var_23) : (0x1c804bf2));
                rb_state3.var_41 = (rb_state3.var_41) + (((rb_state3.var_38) == (0x0)) ? (rb_state3.var_39) : (0xe8dad5aa));
                rb_state3.var_42 = (rb_state3.var_42) - (((0x1c8cc44d) - (0x5fea7bd)) - (rb_state3.var_40));
                rb_state3.var_43 = (rb_state3.var_43) + (rb_state3.var_41);
                rb_state3.var_44 = (rb_state3.var_44) + (((rb_state3.var_42) == (0x0)) ? (rb_state3.var_43) : (0x39a0ef5));
                rb_state3.var_36 = (rb_state3.var_36) - (rb_state3.var_44);
                rb_state3.var_3 = rb_state3.var_36;
                pthread_mutex_unlock(&(rb_state3.lock_45));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + ((0x7134012c) - (0xfdd3624a));
            rb_state4.var_2 = (rb_state4.var_2) ^ (((rb_state4.var_1) == (0x20)) ? (rb_state4.var_2) : (0xc5b6f6f7));
            rb_state4.var_4 = (rb_state4.var_4) + (158504 < rb_input_size ? (uint32_t)rb_input[158504] : 0x4a352463);
            if ((rb_state4.var_4) == (0xa383ff4a)) {
                rb_state4.var_21 = 0xd3ecb0da;
                rb_state4.var_22 = 0xe8e6acba;
                rb_state4.var_23 = (rb_state4.var_23) + (((0x61b334c9) ^ (rb_state4.var_14)) + (rb_state4.var_15));
                rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_22) : (0xbdabffff));
                rb_state4.var_25 = (rb_state4.var_25) - (((((0x2c242cbb) - (rb_state4.var_23)) ^ (rb_state4.var_17)) - (0x3fe80b98)) - (rb_state4.var_21));
                rb_state4.var_26 = (rb_state4.var_26) + (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_25) : (0x1298d1cf));
                rb_state4.var_5 = (rb_state4.var_5) - (rb_state4.var_26);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_3 = (rb_state6.var_3) - (((rb_state6.var_1) == (0x9fed4bb3)) ? (rb_state6.var_0) : (0x2ad59675));
            if ((rb_state6.var_2) == (0xe2d0ab52)) {
                pthread_mutex_lock(&(rb_state6.lock_22));
                rb_state6.var_18 = 0x2c1ee4c7;
                rb_state6.var_19 = (rb_state6.var_19) + (((((0x6dbfebcf) - (rb_state6.var_13)) + (0x369af813)) - (rb_state6.var_14)) + (rb_state6.var_15));
                rb_state6.var_20 = (rb_state6.var_20) - (rb_state6.var_18);
                rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0x5522651b));
                rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_21);
                pthread_mutex_unlock(&(rb_state6.lock_22));
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_1) : (0x4dd3bfdc));
            rb_state7.var_2 = (rb_state7.var_2) - (298276 < rb_input_size ? (uint32_t)rb_input[298276] : 0x51135667);
            rb_state7.var_0 = (rb_state7.var_0) + (35572 < rb_input_size ? (uint32_t)rb_input[35572] : 0xe22f6b8c);
            rb_state7.var_1 = (rb_state7.var_1) + ((0xb9010038) ^ (rb_state7.var_2));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_1) : (0x3d962657));
            rb_state10.var_2 = (rb_state10.var_2) ^ (0x6e53ed9d);
            rb_state10.var_3 = (rb_state10.var_3) + ((0xcf9739c0) - (rb_state10.var_0));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_4 = (rb_state13.var_4) - (0x1581b8b0);
            rb_state13.var_3 = (rb_state13.var_3) ^ ((0x1f1d422c) - (rb_state13.var_4));
            rb_state13.var_0 = (rb_state13.var_0) ^ ((0x43a5ca4c) ^ (rb_state13.var_1));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_0);
            rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_1);
            rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_0);
            rb_state17.var_4 = (rb_state17.var_4) - (((rb_state17.var_2) == (0xa725a348)) ? (rb_state17.var_4) : (0xb30655f2));
            rb_state17.var_1 = (rb_state17.var_1) - (305151 < rb_input_size ? (uint32_t)rb_input[305151] : 0xbe3bb042);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_3 = (rb_state19.var_3) ^ (817579 < rb_input_size ? (uint32_t)rb_input[817579] : 0x48321028);
            if ((rb_state19.var_0) == (0x2bc9f58a)) {
                pthread_mutex_lock(&(rb_state19.lock_18));
                rb_state19.var_6 = 0x6096e3c5;
                rb_state19.var_7 = 0xc450e6bf;
                rb_state19.var_8 = (rb_state19.var_8) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_7) : (0x643921e9));
                rb_state19.var_9 = (rb_state19.var_9) ^ (((((0x579bf267) - (rb_state19.var_8)) ^ (rb_state19.var_6)) ^ (rb_state19.var_5)) ^ (0x1f3c9893));
                rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_9);
                rb_state19.var_4 = rb_state19.var_5;
                pthread_mutex_unlock(&(rb_state19.lock_18));
            }
            if ((rb_state19.var_0) == (0x2bc9f58a)) {
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
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (1086606 < rb_input_size ? (uint32_t)rb_input[1086606] : 0x5d166bb3);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_3);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_28 = (rb_state8.var_28) - (0xde27cb05);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ ((0x8554fa4e) - (rb_state10.var_0));
        rb_state10.var_3 = (rb_state10.var_3) ^ (0x3f369b71);
        rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_24 = (rb_state11.var_24) - (rb_state11.var_25);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + ((0x55009e1d) - (rb_state13.var_2));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0xc217d06a) + (0xc637bfd3));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_0) == (0x5392d1a4)) ? (rb_state17.var_4) : (0xa630fd0a));
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

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - ((0x58a35b28) ^ (rb_state3.var_2));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_4);
    if ((rb_state8.var_0) == (0xbcff4ae2)) {
        pthread_mutex_lock(&(rb_state8.lock_29));
        rb_state8.var_7 = 0x7e140af8;
        rb_state8.var_8 = 0x74424a7d;
        rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_7) == (0x7e140af8)) ? (rb_state8.var_8) : (0x3f46ef52));
        rb_state8.var_10 = (rb_state8.var_10) + (((0x4801593d) ^ (0xf68490d2)) - (rb_state8.var_5));
        rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_9);
        rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_10) : (0x15f3d3b1));
        rb_state8.var_13 = (rb_state8.var_13) + (((0x890622cc) - (rb_state8.var_11)) - (rb_state8.var_7));
        rb_state8.var_14 = (rb_state8.var_14) - (((0x123b44b3) + (0x49815aec)) + (rb_state8.var_12));
        rb_state8.var_15 = (rb_state8.var_15) ^ (((((0x38fe365e) - (rb_state8.var_9)) ^ (rb_state8.var_13)) + (rb_state8.var_8)) - (rb_state8.var_14));
        rb_state8.var_6 = (rb_state8.var_6) - (rb_state8.var_15);
        rb_state8.var_5 = rb_state8.var_6;
        pthread_mutex_unlock(&(rb_state8.lock_29));
    }
    if ((rb_state8.var_0) == (0xbcff4ae2)) {
        rb_state8.var_30 = !((rb_state8.var_5) == (rb_state8.var_6));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - ((0x241c508f) ^ (0x7a52edf1));
    #endif
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