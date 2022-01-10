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
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_2) == (0x8bfc8791)) {
                pthread_mutex_lock(&(rb_state10.lock_23));
                rb_state10.var_5 = 0xa11a7936;
                rb_state10.var_6 = (rb_state10.var_6) ^ (rb_state10.var_5);
                rb_state10.var_7 = (rb_state10.var_7) - (0x16b9ff79);
                rb_state10.var_8 = (rb_state10.var_8) + (((0xe73eef45) + (0x63fa8fc4)) - (rb_state10.var_4));
                rb_state10.var_9 = (rb_state10.var_9) + (((((0x188afd58) + (rb_state10.var_5)) ^ (0xd52414f0)) + (rb_state10.var_3)) + (0x5273cf07));
                rb_state10.var_10 = (rb_state10.var_10) - (((((0x897a19c8) + (0x838ab5e7)) + (rb_state10.var_7)) ^ (rb_state10.var_6)) + (0x95274dfc));
                rb_state10.var_11 = (rb_state10.var_11) + (((0x2ec0eec2) + (rb_state10.var_8)) + (0xc2e08813));
                rb_state10.var_12 = (rb_state10.var_12) + (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_10) : (0x546f35c0));
                rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_11) : (0x1f4b27df));
                rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_12);
                rb_state10.var_15 = (rb_state10.var_15) + (rb_state10.var_13);
                rb_state10.var_16 = (rb_state10.var_16) + (((((0x9942eaff) ^ (rb_state10.var_7)) ^ (rb_state10.var_14)) + (rb_state10.var_15)) ^ (0x18bf9bc5));
                rb_state10.var_4 = (rb_state10.var_4) ^ (rb_state10.var_16);
                rb_state10.var_3 = rb_state10.var_4;
                pthread_mutex_unlock(&(rb_state10.lock_23));
            }
            if ((rb_state10.var_2) == (0x8bfc8791)) {
                if (!((rb_state10.var_3) == (rb_state10.var_4))) {
                    racebench_trigger(10);
                }
            }
            #endif
            x ^= irr;
        } else {
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_1) == (0xb0e12dbb)) {
                pthread_mutex_lock(&(rb_state0.lock_4));
                rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_3);
                pthread_mutex_unlock(&(rb_state0.lock_4));
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
            rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_1);
            rb_state1.var_1 = (rb_state1.var_1) - ((0xc7c99c7b) ^ (0x23e8fae1));
            if ((rb_state1.var_0) == (0x0)) {
                rb_state1.var_2 = rb_state1.var_1;
            }
            if ((rb_state1.var_0) == (0x0)) {
                if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                    racebench_trigger(1);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_2);
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0x40091f9b)) {
                pthread_mutex_lock(&(rb_state5.lock_12));
                rb_state5.var_4 = 0xb10b87d4;
                rb_state5.var_5 = 0x2f136602;
                rb_state5.var_6 = (rb_state5.var_6) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_4) : (0x18a91a64));
                rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_6) : (0xbeca4d2d));
                rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_7);
                rb_state5.var_2 = rb_state5.var_3;
                pthread_mutex_unlock(&(rb_state5.lock_12));
            }
            if ((rb_state5.var_0) == (0x40091f9b)) {
                if (!((rb_state5.var_2) == (rb_state5.var_3))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_1) == (0xffffff14)) ? (rb_state6.var_0) : (0x9c73a718));
            rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x224bebe5)) ? (rb_state6.var_1) : (0x5d8d84e9));
            if (!((rb_state6.var_3) == (rb_state6.var_22))) {
                rb_state6.var_41 = 0x71a3f9d8;
                rb_state6.var_42 = 0x1f14e250;
                rb_state6.var_43 = 0x3497c617;
                rb_state6.var_44 = (rb_state6.var_44) ^ (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_42) : (0x4f954845));
                rb_state6.var_45 = (rb_state6.var_45) + (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_41) : (0xaa50f957));
                rb_state6.var_46 = (rb_state6.var_46) + (((rb_state6.var_43) == (0x0)) ? (rb_state6.var_44) : (0xedf6d8d4));
                rb_state6.var_47 = (rb_state6.var_47) + (((rb_state6.var_21) == (0x0)) ? (rb_state6.var_45) : (0xd8b56228));
                rb_state6.var_48 = (rb_state6.var_48) + (((0xa342400) + (rb_state6.var_22)) + (rb_state6.var_46));
                rb_state6.var_49 = (rb_state6.var_49) ^ (((rb_state6.var_47) == (0x0)) ? (rb_state6.var_48) : (0x85e2c60f));
                rb_state6.var_32 = (rb_state6.var_32) + (rb_state6.var_49);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (0x526a05be);
            rb_state7.var_2 = (rb_state7.var_2) ^ ((0xeb8e8ab3) ^ (0x94dae234));
            rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_1) == (0xad95fa42)) ? (rb_state7.var_2) : (0x33ec0b7f));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) ^ (0xa9c7185);
            if ((rb_state9.var_2) == (0x905d1902)) {
                pthread_mutex_lock(&(rb_state9.lock_16));
                rb_state9.var_5 = (rb_state9.var_5) + (((0x30640a6) ^ (rb_state9.var_4)) + (rb_state9.var_5));
                rb_state9.var_6 = (rb_state9.var_6) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_5) : (0x3260cefc));
                rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_6);
                rb_state9.var_3 = rb_state9.var_4;
                pthread_mutex_unlock(&(rb_state9.lock_16));
            }
            if ((rb_state9.var_2) == (0x905d1902)) {
                if (!((rb_state9.var_3) == (rb_state9.var_4))) {
                    racebench_trigger(9);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (0xc151f1c5);
            rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x3eae0e3b)) ? (rb_state10.var_0) : (0x8d8f4378));
            rb_state10.var_0 = (rb_state10.var_0) - ((0x2eda3f) + (0x81ab7318));
            rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_1) : (0xdf20f13e));
            rb_state10.var_2 = (rb_state10.var_2) + (0xa12940c5);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) + (1108232 < rb_input_size ? (uint32_t)rb_input[1108232] : 0x1e3576f3);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ ((0x1da5cef1) ^ (0x1b10099e));
            rb_state12.var_1 = (rb_state12.var_1) - (0xd023fa6a);
            if ((rb_state12.var_2) == (0x7f282fa1)) {
                pthread_mutex_lock(&(rb_state12.lock_13));
                rb_state12.var_5 = 0x98ac9f51;
                rb_state12.var_6 = 0x64cd4a2b;
                rb_state12.var_7 = (rb_state12.var_7) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_3) : (0x5adf17ac));
                rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_5) : (0x31738ba9));
                rb_state12.var_9 = (rb_state12.var_9) + (((((0x53ac4961) - (rb_state12.var_7)) + (rb_state12.var_5)) - (rb_state12.var_8)) - (0xc930a30b));
                rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_9);
                rb_state12.var_3 = rb_state12.var_4;
                pthread_mutex_unlock(&(rb_state12.lock_13));
            }
            if ((rb_state12.var_2) == (0x7f282fa1)) {
                if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                    racebench_trigger(12);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) - (0x17b31846);
            rb_state13.var_0 = (rb_state13.var_0) + ((0xa428d7e3) ^ (0xff168a04));
            rb_state13.var_2 = (rb_state13.var_2) ^ (816486 < rb_input_size ? (uint32_t)rb_input[816486] : 0xa34a5c44);
            rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_2);
            rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_2) == (0x61)) ? (rb_state13.var_1) : (0x3944da3d));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_2);
            if ((rb_state15.var_0) == (0x0)) {
                rb_state15.var_3 = rb_state15.var_1;
            }
            if ((rb_state15.var_0) == (0x0)) {
                if (!((rb_state15.var_1) == (rb_state15.var_3))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) + (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_1) : (0x739942cf));
            rb_state19.var_0 = (rb_state19.var_0) - (0x481f3442);
            if ((rb_state19.var_1) == (0x9e898ce8)) {
                pthread_mutex_lock(&(rb_state19.lock_13));
                rb_state19.var_3 = rb_state19.var_2;
                pthread_mutex_unlock(&(rb_state19.lock_13));
            }
            if ((rb_state19.var_1) == (0x9e898ce8)) {
                if (!((rb_state19.var_2) == (rb_state19.var_3))) {
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
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0x81)) {
            rb_state3.var_3 = rb_state3.var_2;
        }
        if ((rb_state3.var_1) == (0x81)) {
            pthread_mutex_lock(&(rb_state3.lock_16));
            if (!((rb_state3.var_2) == (rb_state3.var_3))) {
                racebench_trigger(3);
            }
            pthread_mutex_unlock(&(rb_state3.lock_16));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_1) : (0x357a5604));
        rb_state4.var_1 = (rb_state4.var_1) + (0x759a44d);
        rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_2) : (0x624b61f1));
        rb_state4.var_0 = (rb_state4.var_0) - (0x93d407ff);
        if ((rb_state4.var_1) == (0x8628496e)) {
            pthread_mutex_lock(&(rb_state4.lock_6));
            rb_state4.var_4 = 0x48be2d9b;
            rb_state4.var_5 = (rb_state4.var_5) - (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_4) : (0x8d75f29a));
            rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_5);
            pthread_mutex_unlock(&(rb_state4.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_1) == (0xd6ce3547)) {
            rb_state8.var_2 = rb_state8.var_0;
        }
        if ((rb_state8.var_1) == (0xd6ce3547)) {
            if (!((rb_state8.var_0) == (rb_state8.var_2))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x0)) {
            if ((rb_state14.var_3) != (0x0)) {
                if (!((rb_state14.var_3) == (rb_state14.var_22))) {
                    racebench_trigger(14);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) + ((0x5e5c48d8) ^ (0xa4885015));
        #endif
        #ifdef RACEBENCH_BUG_18
        if (!((rb_state18.var_3) == (rb_state18.var_21))) {
            rb_state18.var_29 = 0xe94f7269;
            rb_state18.var_30 = 0x5fc3675c;
            rb_state18.var_31 = (rb_state18.var_31) - (rb_state18.var_16);
            rb_state18.var_32 = (rb_state18.var_32) ^ (1107892 < rb_input_size ? (uint32_t)rb_input[1107892] : 0x1ad95b00);
            rb_state18.var_33 = (rb_state18.var_33) + (((((0xb77f012e) + (rb_state18.var_17)) + (rb_state18.var_18)) - (rb_state18.var_30)) + (rb_state18.var_15));
            rb_state18.var_34 = (rb_state18.var_34) - (((((0xb294443f) + (rb_state18.var_29)) ^ (0xa162d461)) ^ (0x97641a59)) + (rb_state18.var_31));
            rb_state18.var_35 = (rb_state18.var_35) ^ (((0xc24ece5) + (rb_state18.var_19)) ^ (rb_state18.var_32));
            rb_state18.var_36 = (rb_state18.var_36) + (((rb_state18.var_20) == (0xa659ba6)) ? (rb_state18.var_33) : (0x31fdb4b1));
            rb_state18.var_37 = (rb_state18.var_37) ^ (((0x177baef) - (0x53bae2eb)) + (rb_state18.var_34));
            rb_state18.var_38 = (rb_state18.var_38) - (((rb_state18.var_35) == (0xe2da9964)) ? (rb_state18.var_36) : (0x5632b3f1));
            rb_state18.var_39 = (rb_state18.var_39) + (((0xe70bf88e) + (0x659ccf25)) ^ (rb_state18.var_37));
            rb_state18.var_40 = (rb_state18.var_40) ^ (((0xa2146fcb) + (rb_state18.var_38)) ^ (0xa8c54f45));
            rb_state18.var_41 = (rb_state18.var_41) ^ (((((0xd33b45a4) ^ (rb_state18.var_21)) - (rb_state18.var_40)) ^ (rb_state18.var_39)) - (0xc65dc082));
            rb_state18.var_14 = (rb_state18.var_14) ^ (rb_state18.var_41);
        }
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