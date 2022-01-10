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
            rb_state0.var_0 = (rb_state0.var_0) + (713643 < rb_input_size ? (uint32_t)rb_input[713643] : 0x3f4c1370);
            rb_state0.var_0 = (rb_state0.var_0) ^ (0xdca30de);
            #endif
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_0) == (0x0)) {
                rb_state3.var_11 = 0x8a0e874c;
                rb_state3.var_12 = (rb_state3.var_12) + (0xb1c248d2);
                rb_state3.var_13 = (rb_state3.var_13) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xf1cc4233));
                rb_state3.var_14 = (rb_state3.var_14) ^ (822897 < rb_input_size ? (uint32_t)rb_input[822897] : 0xab0f59b);
                rb_state3.var_15 = (rb_state3.var_15) + (((0x6a3ac6e3) - (rb_state3.var_8)) + (rb_state3.var_11));
                rb_state3.var_16 = (rb_state3.var_16) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_13) : (0x125737c3));
                rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_14);
                rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_16) : (0xcde00837));
                rb_state3.var_19 = (rb_state3.var_19) - (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_18) : (0xe5861839));
                rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_19);
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0xdd1628d0)) {
                pthread_mutex_lock(&(rb_state4.lock_43));
                if ((rb_state4.var_1) != (0x0)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_33))) {
                        racebench_trigger(4);
                    }
                }
                pthread_mutex_unlock(&(rb_state4.lock_43));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0x0)) {
                pthread_mutex_lock(&(rb_state8.lock_34));
                rb_state8.var_3 = 0x49c04680;
                rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_2);
                rb_state8.var_5 = (rb_state8.var_5) - (((((0x6e1f8da9) ^ (rb_state8.var_1)) ^ (rb_state8.var_3)) ^ (rb_state8.var_3)) - (0xccb4acaf));
                rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_4) : (0x6753e99a));
                rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_5) : (0x7706d191));
                rb_state8.var_8 = (rb_state8.var_8) - (((0x139b1d34) - (rb_state8.var_6)) ^ (0x85eb9aa4));
                rb_state8.var_9 = (rb_state8.var_9) + (((((0xce84f0fc) + (rb_state8.var_7)) + (0x97f42a73)) ^ (rb_state8.var_6)) - (rb_state8.var_8));
                rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_9);
                rb_state8.var_1 = rb_state8.var_2;
                pthread_mutex_unlock(&(rb_state8.lock_34));
            }
            if ((rb_state8.var_0) == (0x0)) {
                if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                    racebench_trigger(8);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (0x7d6aec7a);
            rb_state9.var_0 = (rb_state9.var_0) ^ (514846 < rb_input_size ? (uint32_t)rb_input[514846] : 0x679a7d4);
            if ((rb_state9.var_0) == (0x36940d6c)) {
                pthread_mutex_lock(&(rb_state9.lock_23));
                rb_state9.var_9 = 0x22e713b3;
                rb_state9.var_10 = (rb_state9.var_10) - (((((0xbd43607) ^ (rb_state9.var_5)) - (rb_state9.var_4)) ^ (0x5bfd1bf5)) + (0x46362415));
                rb_state9.var_11 = (rb_state9.var_11) - ((0x843f2aea) + (0x104e05c6));
                rb_state9.var_12 = (rb_state9.var_12) - (rb_state9.var_6);
                rb_state9.var_13 = (rb_state9.var_13) - (((((0x951dfde) - (rb_state9.var_10)) + (rb_state9.var_7)) - (0xea3d2cd9)) ^ (rb_state9.var_9));
                rb_state9.var_14 = (rb_state9.var_14) + (rb_state9.var_11);
                rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_12);
                rb_state9.var_16 = (rb_state9.var_16) + (((0x6e648e4) ^ (rb_state9.var_13)) + (rb_state9.var_8));
                rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_14);
                rb_state9.var_18 = (rb_state9.var_18) + (rb_state9.var_15);
                rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_16) : (0x3c554243));
                rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_18) : (0x607ae6ea));
                rb_state9.var_21 = (rb_state9.var_21) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_19) : (0xf367124a));
                rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_21) : (0x9d3e13cf));
                rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_22);
                pthread_mutex_unlock(&(rb_state9.lock_23));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (771224 < rb_input_size ? (uint32_t)rb_input[771224] : 0x7d26b7d);
            rb_state10.var_0 = (rb_state10.var_0) ^ (0xc260c8f5);
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_0) == (0xfe61926e)) {
                rb_state13.var_13 = 0x2d3d54ca;
                rb_state13.var_14 = 0x4260cb6f;
                rb_state13.var_15 = (rb_state13.var_15) ^ (((((0x8b5f784b) - (rb_state13.var_5)) ^ (0xcdd1cbb7)) + (0xebd282a7)) - (rb_state13.var_14));
                rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_6) : (0x976e4b5a));
                rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_13) : (0x355207f1));
                rb_state13.var_18 = (rb_state13.var_18) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_15) : (0xa5913b7c));
                rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_16);
                rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_18) : (0x460bbf9d));
                rb_state13.var_21 = (rb_state13.var_21) ^ (((0xcc0f4aa4) ^ (rb_state13.var_19)) - (rb_state13.var_10));
                rb_state13.var_22 = (rb_state13.var_22) + (rb_state13.var_20);
                rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_21);
                rb_state13.var_24 = (rb_state13.var_24) + (((0xb452b55b) - (rb_state13.var_11)) - (rb_state13.var_22));
                rb_state13.var_25 = (rb_state13.var_25) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_23) : (0xde5a1ee9));
                rb_state13.var_26 = (rb_state13.var_26) - (((rb_state13.var_24) == (0x0)) ? (rb_state13.var_25) : (0x764e067f));
                rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_26);
            }
            if ((rb_state13.var_14) == (0x4260cb6f)) {
                rb_state13.var_44 = 0x65a6995;
                rb_state13.var_45 = (rb_state13.var_45) - (((((0x408a41f8) + (rb_state13.var_44)) - (rb_state13.var_22)) + (rb_state13.var_23)) - (0x1e26d453));
                rb_state13.var_46 = (rb_state13.var_46) + (0xaf1eb0e8);
                rb_state13.var_47 = (rb_state13.var_47) + (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_24) : (0xd4624b1f));
                rb_state13.var_48 = (rb_state13.var_48) + (((0x4d56d92f) ^ (rb_state13.var_21)) + (0xb1baf65f));
                rb_state13.var_49 = (rb_state13.var_49) ^ (rb_state13.var_45);
                rb_state13.var_50 = (rb_state13.var_50) ^ (((0xd45bd26d) ^ (rb_state13.var_26)) ^ (rb_state13.var_46));
                rb_state13.var_51 = (rb_state13.var_51) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_47) : (0x74fbca40));
                rb_state13.var_52 = (rb_state13.var_52) + (((rb_state13.var_29) == (0x0)) ? (rb_state13.var_48) : (0x172ca84a));
                rb_state13.var_53 = (rb_state13.var_53) + (((0x3c1d9edc) - (rb_state13.var_30)) ^ (rb_state13.var_49));
                rb_state13.var_54 = (rb_state13.var_54) ^ (rb_state13.var_50);
                rb_state13.var_55 = (rb_state13.var_55) + (((rb_state13.var_51) == (0x0)) ? (rb_state13.var_52) : (0x886f13fa));
                rb_state13.var_56 = (rb_state13.var_56) ^ (((0x695e93fd) - (rb_state13.var_53)) + (0x5d0ece32));
                rb_state13.var_57 = (rb_state13.var_57) ^ (((0x1cd59fdf) - (0xeea54ddb)) - (rb_state13.var_54));
                rb_state13.var_58 = (rb_state13.var_58) - (((rb_state13.var_55) == (0x0)) ? (rb_state13.var_56) : (0xffb66691));
                rb_state13.var_59 = (rb_state13.var_59) - (rb_state13.var_57);
                rb_state13.var_60 = (rb_state13.var_60) - (((rb_state13.var_58) == (0x0)) ? (rb_state13.var_59) : (0xb9089a35));
                rb_state13.var_43 = (rb_state13.var_43) - (rb_state13.var_60);
                rb_state13.var_29 = rb_state13.var_43;
            }
            if ((rb_state13.var_14) == (0x4260cb6f)) {
                pthread_mutex_lock(&(rb_state13.lock_90));
                rb_state13.var_62 = 0x7d53859f;
                rb_state13.var_63 = (rb_state13.var_63) + (0x6bd22f30);
                rb_state13.var_64 = (rb_state13.var_64) ^ (0xa0987552);
                rb_state13.var_65 = (rb_state13.var_65) + (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_32) : (0x69c93628));
                rb_state13.var_66 = (rb_state13.var_66) + (rb_state13.var_62);
                rb_state13.var_67 = (rb_state13.var_67) - (rb_state13.var_63);
                rb_state13.var_68 = (rb_state13.var_68) + (((rb_state13.var_33) == (0x0)) ? (rb_state13.var_64) : (0x5397b407));
                rb_state13.var_69 = (rb_state13.var_69) - (rb_state13.var_65);
                rb_state13.var_70 = (rb_state13.var_70) - (((0x5b4760cd) + (rb_state13.var_66)) - (rb_state13.var_34));
                rb_state13.var_71 = (rb_state13.var_71) ^ (((rb_state13.var_35) == (0x0)) ? (rb_state13.var_67) : (0xda1963db));
                rb_state13.var_72 = (rb_state13.var_72) ^ (((rb_state13.var_36) == (0x0)) ? (rb_state13.var_68) : (0xb94141c8));
                rb_state13.var_73 = (rb_state13.var_73) ^ (((0xc58f6ee8) - (rb_state13.var_69)) - (0x9a2d06ad));
                rb_state13.var_74 = (rb_state13.var_74) - (((0xb80c129a) + (0x23aaf72f)) - (rb_state13.var_70));
                rb_state13.var_75 = (rb_state13.var_75) + (((rb_state13.var_37) == (0x0)) ? (rb_state13.var_71) : (0xce658342));
                rb_state13.var_76 = (rb_state13.var_76) ^ (((rb_state13.var_38) == (0x0)) ? (rb_state13.var_72) : (0xadf70058));
                rb_state13.var_77 = (rb_state13.var_77) + (((rb_state13.var_39) == (0x0)) ? (rb_state13.var_73) : (0xa2d02592));
                rb_state13.var_78 = (rb_state13.var_78) ^ (rb_state13.var_74);
                rb_state13.var_79 = (rb_state13.var_79) - (((0xc8c12cbb) ^ (rb_state13.var_75)) ^ (0xc200d2fb));
                rb_state13.var_80 = (rb_state13.var_80) + (((rb_state13.var_76) == (0x0)) ? (rb_state13.var_77) : (0x6aa554ca));
                rb_state13.var_81 = (rb_state13.var_81) + (((rb_state13.var_40) == (0x0)) ? (rb_state13.var_78) : (0xcdb4f03b));
                rb_state13.var_82 = (rb_state13.var_82) ^ (((rb_state13.var_41) == (0x0)) ? (rb_state13.var_79) : (0x7a5da9eb));
                rb_state13.var_83 = (rb_state13.var_83) + (((((0xfa0615b8) ^ (rb_state13.var_42)) + (rb_state13.var_81)) - (0x536cab4b)) + (rb_state13.var_80));
                rb_state13.var_84 = (rb_state13.var_84) ^ (((0x8cd88cd2) ^ (rb_state13.var_82)) + (0x67c18d25));
                rb_state13.var_85 = (rb_state13.var_85) ^ (((rb_state13.var_43) == (0x0)) ? (rb_state13.var_83) : (0xca56518c));
                rb_state13.var_86 = (rb_state13.var_86) - (((rb_state13.var_44) == (0x0)) ? (rb_state13.var_84) : (0x4978481d));
                rb_state13.var_87 = (rb_state13.var_87) - (((rb_state13.var_45) == (0x0)) ? (rb_state13.var_85) : (0x14b1ea8a));
                rb_state13.var_88 = (rb_state13.var_88) ^ (((rb_state13.var_46) == (0x0)) ? (rb_state13.var_86) : (0x1b228f5));
                rb_state13.var_89 = (rb_state13.var_89) ^ (((((0xd8264a7) - (rb_state13.var_87)) - (0xfedc91c1)) - (rb_state13.var_47)) - (rb_state13.var_88));
                rb_state13.var_61 = (rb_state13.var_61) + (rb_state13.var_89);
                rb_state13.var_29 = rb_state13.var_61;
                pthread_mutex_unlock(&(rb_state13.lock_90));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x2e819584));
            rb_state14.var_0 = (rb_state14.var_0) ^ ((0xa5c9298a) - (rb_state14.var_0));
            if ((rb_state14.var_0) == (0x45c4d582)) {
                pthread_mutex_lock(&(rb_state14.lock_8));
                rb_state14.var_3 = 0x3f800023;
                rb_state14.var_4 = 0xa12c2222;
                rb_state14.var_5 = 0x3582302d;
                rb_state14.var_6 = (rb_state14.var_6) + (((((0x952208c1) ^ (rb_state14.var_3)) ^ (0x86e9643c)) ^ (rb_state14.var_5)) - (rb_state14.var_2));
                rb_state14.var_7 = (rb_state14.var_7) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0x1ab25fd4));
                rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_7);
                pthread_mutex_unlock(&(rb_state14.lock_8));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0x31c2295e)) {
                rb_state16.var_3 = 0x203d92a7;
                rb_state16.var_4 = 0x6631411c;
                rb_state16.var_5 = (rb_state16.var_5) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_4) : (0xa855b78d));
                rb_state16.var_6 = (rb_state16.var_6) ^ (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_2) : (0xceb01aa));
                rb_state16.var_7 = (rb_state16.var_7) + (((0x20efdfcc) - (rb_state16.var_4)) + (rb_state16.var_5));
                rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0x36fbe405));
                rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_8);
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (0xb50ad1de);
            if ((rb_state17.var_0) == (0x6dea34b4)) {
                rb_state17.var_2 = rb_state17.var_1;
            }
            if ((rb_state17.var_0) == (0x6dea34b4)) {
                if (!((rb_state17.var_1) == (rb_state17.var_2))) {
                    racebench_trigger(17);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - ((0xce51480a) ^ (rb_state18.var_0));
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