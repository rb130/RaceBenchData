#include "racebench_bugs.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#ifdef ENABLE_THREADS

#define __thread __threadp

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#ifndef __USE_UNIX98
#define __USE_UNIX98
#endif
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 128

pthread_t _M4_threadsTable[MAX_THREADS];
int _M4_threadsTableAllocated[MAX_THREADS];
pthread_mutexattr_t _M4_normalMutexAttr;
int _M4_numThreads = MAX_THREADS;

#undef __thread
#endif

#ifdef ENABLE_OPENMP
#include <omp.h>
#endif

#ifdef ENABLE_TBB
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"

using namespace std;
using namespace tbb;
#endif

#ifdef WIN32
#pragma warning(disable : 4305)
#pragma warning(disable : 4244)
#include <windows.h>
#endif

#define fptype float

#define NUM_RUNS 100

typedef struct OptionData_ {
    fptype s;
    fptype strike;
    fptype r;
    fptype divq;
    fptype v;
    fptype t;

    char OptionType;
    fptype divs;
    fptype DGrefval;
} OptionData;

OptionData *data;
fptype *prices;
int numOptions;

int *otype;
fptype *sptprice;
fptype *strike;
fptype *rate;
fptype *volatility;
fptype *otime;
int numError = 0;
int nThreads;

#define inv_sqrt_2xPI 0.39894228040143270286

fptype CNDF(fptype InputX) {
    int sign;

    fptype OutputX;
    fptype xInput;
    fptype xNPrimeofX;
    fptype expValues;
    fptype xK2;
    fptype xK2_2, xK2_3;
    fptype xK2_4, xK2_5;
    fptype xLocal, xLocal_1;
    fptype xLocal_2, xLocal_3;

    if (InputX < 0.0) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_13 = (rb_state2.var_13) + (142 < rb_input_size ? (uint32_t)rb_input[142] : 0xa9f04d76);
        #endif
        InputX = -InputX;
        sign = 1;
    } else {
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x30)) ? (rb_state11.var_0) : (0x7495bdd5));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + ((0xa057aeab) + (rb_state17.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0xe8fa48c8)) {
            rb_state18.var_2 = rb_state18.var_1;
        }
        if ((rb_state18.var_0) == (0xe8fa48c8)) {
            pthread_mutex_lock(&(rb_state18.lock_15));
            rb_state18.var_16 = !((rb_state18.var_1) == (rb_state18.var_2));
            pthread_mutex_unlock(&(rb_state18.lock_15));
        }
        #endif
        sign = 0;
    }

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x20176889)) {
        rb_state3.var_2 = rb_state3.var_0;
    }
    if ((rb_state3.var_1) == (0x20176889)) {
        if (!((rb_state3.var_0) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (60 < rb_input_size ? (uint32_t)rb_input[60] : 0x877008be);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (43 < rb_input_size ? (uint32_t)rb_input[43] : 0x87972250);
    #endif
    xInput = InputX;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (42 < rb_input_size ? (uint32_t)rb_input[42] : 0x8c764fca);
    #endif
    expValues = exp(-0.5f * InputX * InputX);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0x31a862c3) ^ (0xe7baf33c));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (0xc2a2ae9c);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xca5e31cb);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (0xc17826bc);
    #endif
    xNPrimeofX = expValues;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0xfe46a4fe);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + ((0x362458e4) - (0x3608b502));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_13 = (rb_state18.var_13) - (rb_state18.var_14);
    #endif
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xddd9b694)) {
        rb_state4.var_20 = 0xc670c7ec;
        rb_state4.var_21 = 0xace6d769;
        rb_state4.var_22 = (rb_state4.var_22) - (0xbee8e8dc);
        rb_state4.var_23 = (rb_state4.var_23) ^ (rb_state4.var_21);
        rb_state4.var_24 = (rb_state4.var_24) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_20) : (0x68053a71));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((((0xe798666c) + (rb_state4.var_23)) + (rb_state4.var_22)) + (0x2b9543c8)) + (0xba6730fe));
        rb_state4.var_26 = (rb_state4.var_26) ^ (((0x4610a30b) + (rb_state4.var_24)) ^ (0x82f3648));
        rb_state4.var_27 = (rb_state4.var_27) - (((((0xdd5d9c0f) + (rb_state4.var_25)) ^ (rb_state4.var_26)) + (rb_state4.var_11)) + (0x53178f4d));
        rb_state4.var_19 = (rb_state4.var_19) + (rb_state4.var_27);
        rb_state4.var_2 = rb_state4.var_19;
    }
    if ((rb_state4.var_1) == (0xddd9b694)) {
        pthread_mutex_lock(&(rb_state4.lock_52));
        rb_state4.var_29 = 0xe96d2cee;
        rb_state4.var_30 = 0xb8ad75df;
        rb_state4.var_31 = 0x5e90c74d;
        rb_state4.var_32 = (rb_state4.var_32) ^ (rb_state4.var_30);
        rb_state4.var_33 = (rb_state4.var_33) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_31) : (0x7761334e));
        rb_state4.var_34 = (rb_state4.var_34) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_29) : (0x8af4b9f4));
        rb_state4.var_35 = (rb_state4.var_35) ^ (rb_state4.var_32);
        rb_state4.var_36 = (rb_state4.var_36) - (rb_state4.var_33);
        rb_state4.var_37 = (rb_state4.var_37) + (rb_state4.var_34);
        rb_state4.var_38 = (rb_state4.var_38) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_35) : (0xccaad5e0));
        rb_state4.var_39 = (rb_state4.var_39) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_36) : (0x2d999743));
        rb_state4.var_40 = (rb_state4.var_40) ^ (((0xfaaed36b) ^ (rb_state4.var_37)) + (0xe9c3e067));
        rb_state4.var_41 = (rb_state4.var_41) - (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_38) : (0xc94e600));
        rb_state4.var_42 = (rb_state4.var_42) - (((0x7eccdf67) + (rb_state4.var_39)) + (rb_state4.var_17));
        rb_state4.var_43 = (rb_state4.var_43) + (rb_state4.var_40);
        rb_state4.var_44 = (rb_state4.var_44) + (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_41) : (0x3beb82bb));
        rb_state4.var_45 = (rb_state4.var_45) + (rb_state4.var_42);
        rb_state4.var_46 = (rb_state4.var_46) - (((rb_state4.var_43) == (0x0)) ? (rb_state4.var_44) : (0x9c267a88));
        rb_state4.var_47 = (rb_state4.var_47) ^ (((0xac6fdd66) + (rb_state4.var_45)) + (rb_state4.var_19));
        rb_state4.var_48 = (rb_state4.var_48) ^ (((0x360de51a) + (rb_state4.var_20)) + (rb_state4.var_46));
        rb_state4.var_49 = (rb_state4.var_49) + (((0x4e2a2715) + (rb_state4.var_47)) - (rb_state4.var_21));
        rb_state4.var_50 = (rb_state4.var_50) ^ (rb_state4.var_48);
        rb_state4.var_51 = (rb_state4.var_51) - (((rb_state4.var_49) == (0x0)) ? (rb_state4.var_50) : (0x65cc2318));
        rb_state4.var_28 = (rb_state4.var_28) - (rb_state4.var_51);
        rb_state4.var_2 = rb_state4.var_28;
        pthread_mutex_unlock(&(rb_state4.lock_52));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (231 < rb_input_size ? (uint32_t)rb_input[231] : 0x5b6842fa);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (0x9e03d592);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xcf5134b1)) {
        if ((rb_state16.var_2) != (0x0)) {
            if (!((rb_state16.var_2) == (rb_state16.var_22))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xe8fa48c8)) {
        rb_state18.var_3 = 0xcf8da621;
        rb_state18.var_4 = 0x38306b50;
        rb_state18.var_5 = (rb_state18.var_5) ^ (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_2) : (0xc30b4396));
        rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_4) : (0x217c0025));
        rb_state18.var_7 = (rb_state18.var_7) - (((0x4aae2668) - (rb_state18.var_4)) - (rb_state18.var_6));
        rb_state18.var_8 = (rb_state18.var_8) ^ (((((0xe33f604a) - (rb_state18.var_7)) ^ (rb_state18.var_8)) + (rb_state18.var_3)) + (rb_state18.var_5));
        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0x3097cb45));
        rb_state18.var_10 = (rb_state18.var_10) ^ (rb_state18.var_8);
        rb_state18.var_11 = (rb_state18.var_11) + (rb_state18.var_9);
        rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_10);
        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_11) : (0xaa35fcaa));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_13) : (0xf335bdc7));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xa7a48140));
    #endif
    xK2 = 0.2316419 * xInput;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x42384c05));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xd07aacd4)) {
        if ((rb_state2.var_4) != (0x0)) {
            rb_state2.var_19 = !((rb_state2.var_2) == (rb_state2.var_3));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xdfcc6d1a)) {
        rb_state14.var_3 = 0xbbb7c6ac;
        rb_state14.var_4 = 0x8946fc76;
        rb_state14.var_5 = (rb_state14.var_5) + (((((0x9068eea7) + (rb_state14.var_4)) - (0xc7ebe10c)) - (rb_state14.var_3)) ^ (rb_state14.var_2));
        rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_5) : (0x4c2ec10));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_6);
    }
    #endif
    xK2 = 1.0 + xK2;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x456436)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0x456436)) {
        pthread_mutex_lock(&(rb_state10.lock_5));
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
        pthread_mutex_unlock(&(rb_state10.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (115 < rb_input_size ? (uint32_t)rb_input[115] : 0x9ab3a37d);
    #endif
    xK2 = 1.0 / xK2;
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x913bacb7)) {
        rb_state15.var_4 = 0xe9d2759e;
        rb_state15.var_5 = (rb_state15.var_5) ^ (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_3) : (0x232050b7));
        rb_state15.var_6 = (rb_state15.var_6) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_5) : (0x9dbbdd3));
        rb_state15.var_3 = (rb_state15.var_3) - (rb_state15.var_6);
        rb_state15.var_2 = rb_state15.var_3;
    }
    if ((rb_state15.var_1) == (0x913bacb7)) {
        pthread_mutex_lock(&(rb_state15.lock_22));
        rb_state15.var_8 = 0xe830b27f;
        rb_state15.var_9 = 0x1dc39272;
        rb_state15.var_10 = (rb_state15.var_10) + (rb_state15.var_7);
        rb_state15.var_11 = (rb_state15.var_11) + (rb_state15.var_9);
        rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_6);
        rb_state15.var_13 = (rb_state15.var_13) ^ (rb_state15.var_8);
        rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0xc8b0529f));
        rb_state15.var_15 = (rb_state15.var_15) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_12) : (0xafff0f3));
        rb_state15.var_16 = (rb_state15.var_16) + (((0xbf1b689c) + (0xf3e9eee0)) + (rb_state15.var_13));
        rb_state15.var_17 = (rb_state15.var_17) + (((0x9e76a0f3) - (rb_state15.var_14)) + (rb_state15.var_9));
        rb_state15.var_18 = (rb_state15.var_18) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_15) : (0x8d4ff81a));
        rb_state15.var_19 = (rb_state15.var_19) - (rb_state15.var_16);
        rb_state15.var_20 = (rb_state15.var_20) + (((((0x306c68c1) ^ (rb_state15.var_12)) - (rb_state15.var_17)) ^ (rb_state15.var_11)) + (rb_state15.var_18));
        rb_state15.var_21 = (rb_state15.var_21) - (((((0x947e41ff) ^ (rb_state15.var_14)) + (rb_state15.var_19)) + (rb_state15.var_13)) ^ (rb_state15.var_20));
        rb_state15.var_7 = (rb_state15.var_7) + (rb_state15.var_21);
        rb_state15.var_2 = rb_state15.var_7;
        pthread_mutex_unlock(&(rb_state15.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - ((0xe4a91682) + (0x2f83a3bb));
    #endif
    xK2_2 = xK2 * xK2;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x21db07bf) - (rb_state2.var_1));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (0x81719859);
    #endif
    xK2_3 = xK2_2 * xK2;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_1) : (0xc63f778f));
    #endif
    xK2_4 = xK2_3 * xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0x611ba05b) - (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xd07aacd4)) {
        rb_state2.var_4 = 0x1;
    }
    if ((rb_state2.var_0) == (0xd07aacd4)) {
        pthread_mutex_lock(&(rb_state2.lock_18));
        rb_state2.var_5 = 0x3c056501;
        rb_state2.var_6 = (rb_state2.var_6) ^ (rb_state2.var_4);
        rb_state2.var_7 = (rb_state2.var_7) ^ (0x38764739);
        rb_state2.var_8 = (rb_state2.var_8) - (0x618f5b0b);
        rb_state2.var_9 = (rb_state2.var_9) - (((((0x67c81d31) ^ (rb_state2.var_5)) ^ (rb_state2.var_3)) ^ (rb_state2.var_5)) - (0xa6452303));
        rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_2) : (0x56198580));
        rb_state2.var_11 = (rb_state2.var_11) + (((0x69184f6c) + (rb_state2.var_6)) + (rb_state2.var_7));
        rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x29724b4a));
        rb_state2.var_13 = (rb_state2.var_13) - (((((0x4c0444d4) ^ (rb_state2.var_9)) ^ (rb_state2.var_10)) ^ (rb_state2.var_8)) - (0x65c7b3bb));
        rb_state2.var_14 = (rb_state2.var_14) - (((((0xbf0464d4) - (rb_state2.var_11)) + (rb_state2.var_9)) ^ (rb_state2.var_10)) + (rb_state2.var_12));
        rb_state2.var_15 = (rb_state2.var_15) - (((0xae736521) ^ (rb_state2.var_11)) - (rb_state2.var_13));
        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_14);
        rb_state2.var_17 = (rb_state2.var_17) + (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_16) : (0xf8cc168a));
        rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_17);
        rb_state2.var_2 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_8) == (0x364a16cd)) {
        pthread_mutex_lock(&(rb_state7.lock_41));
        if ((rb_state7.var_16) != (0x0)) {
            if (!((rb_state7.var_16) == (rb_state7.var_33))) {
                racebench_trigger(7);
            }
        }
        pthread_mutex_unlock(&(rb_state7.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x3174c4a4)) {
        pthread_mutex_lock(&(rb_state19.lock_13));
        if ((rb_state19.var_1) != (0x0)) {
            if (!((rb_state19.var_1) == (rb_state19.var_5))) {
                racebench_trigger(19);
            }
        }
        pthread_mutex_unlock(&(rb_state19.lock_13));
    }
    #endif
    xK2_5 = xK2_4 * xK2;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6f1df1f5)) {
        rb_state0.var_17 = 0x7cbceed0;
        rb_state0.var_18 = (rb_state0.var_18) ^ (((0x5315edde) ^ (0x8966bda5)) + (rb_state0.var_17));
        rb_state0.var_19 = (rb_state0.var_19) - (234 < rb_input_size ? (uint32_t)rb_input[234] : 0x58a53fe7);
        rb_state0.var_20 = (rb_state0.var_20) + (((((0x427b59f7) ^ (0x15fd9c67)) ^ (rb_state0.var_10)) ^ (rb_state0.var_9)) - (rb_state0.var_11));
        rb_state0.var_21 = (rb_state0.var_21) + (rb_state0.var_18);
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_19) : (0x65025774));
        rb_state0.var_23 = (rb_state0.var_23) + (((0xd1a24a60) + (rb_state0.var_13)) - (rb_state0.var_20));
        rb_state0.var_24 = (rb_state0.var_24) ^ (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_21) : (0x31539a59));
        rb_state0.var_25 = (rb_state0.var_25) + (rb_state0.var_22);
        rb_state0.var_26 = (rb_state0.var_26) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_24) : (0x6a410402));
        rb_state0.var_27 = (rb_state0.var_27) ^ (((0x84d7c955) - (rb_state0.var_15)) - (rb_state0.var_25));
        rb_state0.var_28 = (rb_state0.var_28) + (((0x9039ee03) ^ (rb_state0.var_26)) ^ (0x3044c4b5));
        rb_state0.var_29 = (rb_state0.var_29) - (rb_state0.var_27);
        rb_state0.var_30 = (rb_state0.var_30) - (((0x129eedca) ^ (rb_state0.var_16)) ^ (rb_state0.var_28));
        rb_state0.var_31 = (rb_state0.var_31) - (((((0x590bf49c) ^ (rb_state0.var_17)) + (rb_state0.var_29)) - (0x2e029e2e)) ^ (rb_state0.var_30));
        rb_state0.var_16 = (rb_state0.var_16) ^ (rb_state0.var_31);
        rb_state0.var_2 = rb_state0.var_16;
    }
    if ((rb_state0.var_0) == (0x6f1df1f5)) {
        pthread_mutex_lock(&(rb_state0.lock_49));
        rb_state0.var_33 = 0x15e45c54;
        rb_state0.var_34 = 0xea184ba5;
        rb_state0.var_35 = 0x10d2b576;
        rb_state0.var_36 = (rb_state0.var_36) ^ ((0xfbe5fdb9) + (0x72e03f96));
        rb_state0.var_37 = (rb_state0.var_37) - (rb_state0.var_33);
        rb_state0.var_38 = (rb_state0.var_38) - (((0x2d17ffcb) + (0x5339a5d)) ^ (rb_state0.var_34));
        rb_state0.var_39 = (rb_state0.var_39) - (((0x924a27f7) ^ (rb_state0.var_18)) + (rb_state0.var_35));
        rb_state0.var_40 = (rb_state0.var_40) + (rb_state0.var_36);
        rb_state0.var_41 = (rb_state0.var_41) + (((0x7d58833c) - (rb_state0.var_19)) ^ (rb_state0.var_37));
        rb_state0.var_42 = (rb_state0.var_42) + (((0x4efcf46e) ^ (rb_state0.var_20)) + (rb_state0.var_38));
        rb_state0.var_43 = (rb_state0.var_43) + (((((0xf86a2426) ^ (rb_state0.var_39)) - (0x7a569a86)) - (rb_state0.var_40)) ^ (rb_state0.var_21));
        rb_state0.var_44 = (rb_state0.var_44) - (rb_state0.var_41);
        rb_state0.var_45 = (rb_state0.var_45) + (((0xc8dd1cdb) - (0x7148d502)) + (rb_state0.var_42));
        rb_state0.var_46 = (rb_state0.var_46) ^ (((rb_state0.var_43) == (0x0)) ? (rb_state0.var_44) : (0xfabda16a));
        rb_state0.var_47 = (rb_state0.var_47) + (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_45) : (0xc55478e3));
        rb_state0.var_48 = (rb_state0.var_48) + (((((0xea7bdd13) ^ (rb_state0.var_23)) ^ (0xd32f8cb1)) - (rb_state0.var_46)) - (rb_state0.var_47));
        rb_state0.var_32 = (rb_state0.var_32) - (rb_state0.var_48);
        rb_state0.var_2 = rb_state0.var_32;
        pthread_mutex_unlock(&(rb_state0.lock_49));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_8) == (0x364a16cd)) {
        rb_state7.var_27 = 0xa21283e1;
        rb_state7.var_28 = (rb_state7.var_28) ^ (rb_state7.var_18);
        rb_state7.var_29 = (rb_state7.var_29) + (rb_state7.var_27);
        rb_state7.var_30 = (rb_state7.var_30) ^ (((((0xd8c089f3) ^ (0x94b371ee)) ^ (rb_state7.var_17)) + (0x5308f595)) + (rb_state7.var_16));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((((0xe197c7d3) + (rb_state7.var_19)) - (rb_state7.var_29)) + (rb_state7.var_28)) - (0x91cfd9ff));
        rb_state7.var_32 = (rb_state7.var_32) + (((((0xfc2a7d5d) ^ (rb_state7.var_30)) + (rb_state7.var_20)) ^ (rb_state7.var_21)) + (rb_state7.var_31));
        rb_state7.var_26 = (rb_state7.var_26) + (rb_state7.var_32);
        rb_state7.var_16 = rb_state7.var_26;
    }
    if ((rb_state7.var_8) == (0x364a16cd)) {
        pthread_mutex_lock(&(rb_state7.lock_41));
        rb_state7.var_34 = 0xf10d1ae4;
        rb_state7.var_35 = 0x7ded1dd6;
        rb_state7.var_36 = (rb_state7.var_36) ^ (((rb_state7.var_22) == (0x0)) ? (rb_state7.var_34) : (0xcdd9afbb));
        rb_state7.var_37 = (rb_state7.var_37) - (241 < rb_input_size ? (uint32_t)rb_input[241] : 0x5c206cef);
        rb_state7.var_38 = (rb_state7.var_38) - (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_35) : (0xafdde96f));
        rb_state7.var_39 = (rb_state7.var_39) ^ (((((0x1fc2fa8e) + (rb_state7.var_25)) ^ (rb_state7.var_36)) ^ (rb_state7.var_37)) - (rb_state7.var_24));
        rb_state7.var_40 = (rb_state7.var_40) - (((rb_state7.var_38) == (0x0)) ? (rb_state7.var_39) : (0xae8275c9));
        rb_state7.var_33 = (rb_state7.var_33) ^ (rb_state7.var_40);
        rb_state7.var_16 = rb_state7.var_33;
        pthread_mutex_unlock(&(rb_state7.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x456436)) {
        rb_state10.var_3 = 0x5c2c8728;
        rb_state10.var_4 = (rb_state10.var_4) - (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_3) : (0x6afb38d));
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_4);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x56a0)) {
        pthread_mutex_lock(&(rb_state17.lock_30));
        rb_state17.var_27 = 0x4d0f9085;
        rb_state17.var_28 = (rb_state17.var_28) - (((((0xe6ed93db) ^ (0xa8db305b)) - (rb_state17.var_11)) - (rb_state17.var_13)) ^ (rb_state17.var_27));
        rb_state17.var_29 = (rb_state17.var_29) - (((((0xb59933f3) - (rb_state17.var_12)) + (0xb2ef95f)) ^ (rb_state17.var_28)) - (0x14496f08));
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_29);
        pthread_mutex_unlock(&(rb_state17.lock_30));
    }
    #endif
    xLocal_1 = xK2 * 0.319381530;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + ((0x46c32e41) + (rb_state15.var_0));
    #endif
    xLocal_2 = xK2_2 * (-0.356563782);
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (0x8a3a3bc1);
    #endif
    xLocal_3 = xK2_3 * 1.781477937;
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_16) == (0x1)) {
        if ((rb_state18.var_17) != (0x0)) {
            if (!((rb_state18.var_17) == (rb_state18.var_32))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6f1df1f5)) {
        pthread_mutex_lock(&(rb_state0.lock_49));
        if ((rb_state0.var_2) != (0x0)) {
            if (!((rb_state0.var_2) == (rb_state0.var_32))) {
                racebench_trigger(0);
            }
        }
        pthread_mutex_unlock(&(rb_state0.lock_49));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x5b8e2524)) {
        rb_state5.var_10 = 0x8b16fd31;
        rb_state5.var_11 = 0x5eb40d2c;
        rb_state5.var_12 = (rb_state5.var_12) ^ (((0xd1aa0806) + (rb_state5.var_8)) ^ (rb_state5.var_11));
        rb_state5.var_13 = (rb_state5.var_13) - (0xbf5a38b9);
        rb_state5.var_14 = (rb_state5.var_14) - (((0x467bcad5) + (0x951ca831)) ^ (rb_state5.var_10));
        rb_state5.var_15 = (rb_state5.var_15) ^ (((((0xdcedcf28) - (rb_state5.var_9)) - (rb_state5.var_7)) - (0x299c493b)) - (rb_state5.var_12));
        rb_state5.var_16 = (rb_state5.var_16) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_13) : (0x4fa853b3));
        rb_state5.var_17 = (rb_state5.var_17) ^ (rb_state5.var_14);
        rb_state5.var_18 = (rb_state5.var_18) - (((0xae24daea) ^ (rb_state5.var_15)) - (rb_state5.var_11));
        rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_16) : (0x9430b87f));
        rb_state5.var_20 = (rb_state5.var_20) + (((0x8bb29890) - (rb_state5.var_17)) + (0x7db97d5d));
        rb_state5.var_21 = (rb_state5.var_21) ^ (rb_state5.var_18);
        rb_state5.var_22 = (rb_state5.var_22) + (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_19) : (0xa37dfe3e));
        rb_state5.var_23 = (rb_state5.var_23) + (rb_state5.var_20);
        rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_21) : (0x59002c8c));
        rb_state5.var_25 = (rb_state5.var_25) - (rb_state5.var_22);
        rb_state5.var_26 = (rb_state5.var_26) + (((((0xd10a30b7) + (rb_state5.var_16)) + (rb_state5.var_24)) ^ (rb_state5.var_23)) ^ (rb_state5.var_15));
        rb_state5.var_27 = (rb_state5.var_27) ^ (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_25) : (0xadb32308));
        rb_state5.var_28 = (rb_state5.var_28) + (rb_state5.var_26);
        rb_state5.var_29 = (rb_state5.var_29) ^ (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_27) : (0xfeab30bb));
        rb_state5.var_30 = (rb_state5.var_30) + (rb_state5.var_28);
        rb_state5.var_31 = (rb_state5.var_31) - (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_29) : (0x504aa29d));
        rb_state5.var_32 = (rb_state5.var_32) + (((rb_state5.var_30) == (0x0)) ? (rb_state5.var_31) : (0xc11430f));
        rb_state5.var_9 = (rb_state5.var_9) + (rb_state5.var_32);
        rb_state5.var_2 = rb_state5.var_9;
    }
    if ((rb_state5.var_1) == (0x5b8e2524)) {
        pthread_mutex_lock(&(rb_state5.lock_41));
        rb_state5.var_34 = 0xa6f82436;
        rb_state5.var_35 = (rb_state5.var_35) - (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_22) : (0xe623dd01));
        rb_state5.var_36 = (rb_state5.var_36) - (((0x7aef9f9b) ^ (rb_state5.var_24)) + (rb_state5.var_34));
        rb_state5.var_37 = (rb_state5.var_37) ^ (((((0x593d8877) ^ (rb_state5.var_21)) ^ (rb_state5.var_20)) - (rb_state5.var_26)) + (rb_state5.var_25));
        rb_state5.var_38 = (rb_state5.var_38) ^ (((0x3be35aa2) ^ (0x5942e909)) ^ (rb_state5.var_35));
        rb_state5.var_39 = (rb_state5.var_39) + (((((0xcca76692) ^ (rb_state5.var_37)) ^ (rb_state5.var_27)) + (0x47c00f6d)) - (rb_state5.var_36));
        rb_state5.var_40 = (rb_state5.var_40) + (((rb_state5.var_38) == (0x0)) ? (rb_state5.var_39) : (0x31c264f5));
        rb_state5.var_33 = (rb_state5.var_33) - (rb_state5.var_40);
        rb_state5.var_2 = rb_state5.var_33;
        pthread_mutex_unlock(&(rb_state5.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (0x96a0a624);
    if ((rb_state7.var_0) == (0x94e8d56d)) {
        usleep(100);
    }
    if ((rb_state7.var_0) == (0x94e8d56d)) {
        rb_state7.var_15 = !((rb_state7.var_3) == (rb_state7.var_2));
    }
    rb_state7.var_6 = (rb_state7.var_6) ^ (160 < rb_input_size ? (uint32_t)rb_input[160] : 0xaefd9c19);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - ((0x32ded64c) + (0x59902377));
    if ((rb_state9.var_0) == (0x6f76e7ea)) {
        rb_state9.var_2 = rb_state9.var_1;
    }
    if ((rb_state9.var_0) == (0x6f76e7ea)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (103 < rb_input_size ? (uint32_t)rb_input[103] : 0xcad714f9);
    #endif
    xLocal_3 = xK2_4 * (-1.821255978);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_0) : (0x63fd251e));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x98a8d424)) {
        pthread_mutex_lock(&(rb_state13.lock_8));
        rb_state13.var_3 = 0x5ff7b4e9;
        rb_state13.var_4 = 0xb1256ba1;
        rb_state13.var_5 = (rb_state13.var_5) - (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_3) : (0x812d35bc));
        rb_state13.var_6 = (rb_state13.var_6) + (((((0x25f2423b) - (rb_state13.var_5)) + (rb_state13.var_4)) - (rb_state13.var_4)) + (rb_state13.var_2));
        rb_state13.var_7 = (rb_state13.var_7) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_6) : (0x97c7d916));
        rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_7);
        pthread_mutex_unlock(&(rb_state13.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_0);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0xfeaecfa));
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xbfac3d1d)) {
        pthread_mutex_lock(&(rb_state8.lock_24));
        rb_state8.var_4 = 0x84014039;
        rb_state8.var_5 = 0xa08bb655;
        rb_state8.var_6 = (rb_state8.var_6) + (((((0x419bfb26) - (rb_state8.var_4)) - (rb_state8.var_3)) + (rb_state8.var_5)) ^ (0xcef69640));
        rb_state8.var_7 = (rb_state8.var_7) ^ (((0x2c12235d) ^ (rb_state8.var_2)) - (rb_state8.var_4));
        rb_state8.var_8 = (rb_state8.var_8) + (((((0xda1385c3) ^ (rb_state8.var_5)) - (rb_state8.var_6)) - (rb_state8.var_6)) + (rb_state8.var_7));
        rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_8);
        rb_state8.var_2 = rb_state8.var_3;
        pthread_mutex_unlock(&(rb_state8.lock_24));
    }
    if ((rb_state8.var_0) == (0xbfac3d1d)) {
        pthread_mutex_lock(&(rb_state8.lock_24));
        if (!((rb_state8.var_2) == (rb_state8.var_3))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x98a8d424)) {
        rb_state13.var_2 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0x98a8d424)) {
        pthread_mutex_lock(&(rb_state13.lock_8));
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
        pthread_mutex_unlock(&(rb_state13.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0xb8c4a296);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0x9789b838) + (rb_state18.var_0));
    #endif
    xLocal_3 = xK2_5 * 1.330274429;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (0x272e4aa2);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + ((0x48021eeb) + (rb_state3.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_5);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0xcba4de) ^ (rb_state10.var_0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (0xe14542c0);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (156 < rb_input_size ? (uint32_t)rb_input[156] : 0xeae69fc0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0xcd1d4c8f)) ? (rb_state12.var_1) : (0x654cb615));
    #endif
    xLocal_1 = xLocal_2 + xLocal_1;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0xfefae54e) + (rb_state10.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0x98647f78) - (rb_state16.var_1));
    #endif
    xLocal = xLocal_1 * xNPrimeofX;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_12 = (rb_state2.var_12) ^ (0xd909d7ab);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (22 < rb_input_size ? (uint32_t)rb_input[22] : 0x96f1cb5);
    #endif
    xLocal = 1.0 - xLocal;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xa136a5ef));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xddd9b694)) {
        if ((rb_state4.var_2) != (0x0)) {
            if (!((rb_state4.var_2) == (rb_state4.var_28))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xbcdb7a66)) {
        pthread_mutex_lock(&(rb_state6.lock_19));
        rb_state6.var_3 = 0x2597c814;
        rb_state6.var_4 = 0xb2bdad34;
        rb_state6.var_5 = 0x32524f2f;
        rb_state6.var_6 = (rb_state6.var_6) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_4) : (0x22f3473d));
        rb_state6.var_7 = (rb_state6.var_7) ^ (((0xdf5916f9) + (rb_state6.var_1)) ^ (rb_state6.var_5));
        rb_state6.var_8 = (rb_state6.var_8) - (((((0x552101c2) - (rb_state6.var_3)) - (rb_state6.var_2)) ^ (rb_state6.var_6)) + (rb_state6.var_7));
        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_8);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_19));
    }
    if ((rb_state6.var_0) == (0xbcdb7a66)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ ((0x2fb09791) - (rb_state7.var_1));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x56a0)) {
        pthread_mutex_lock(&(rb_state17.lock_30));
        rb_state17.var_4 = (rb_state17.var_4) + (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_3) : (0x6ab9d853));
        rb_state17.var_5 = (rb_state17.var_5) ^ ((0xf591118a) ^ (0xc6047b0));
        rb_state17.var_6 = (rb_state17.var_6) ^ (((0x52a1a97) - (0xeca762cb)) + (rb_state17.var_2));
        rb_state17.var_7 = (rb_state17.var_7) ^ (0x1d270bfa);
        rb_state17.var_8 = (rb_state17.var_8) ^ (rb_state17.var_1);
        rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_4);
        rb_state17.var_10 = (rb_state17.var_10) + (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_5) : (0xf9e5db2d));
        rb_state17.var_11 = (rb_state17.var_11) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0x6c0397a6));
        rb_state17.var_12 = (rb_state17.var_12) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_9) : (0xc0409111));
        rb_state17.var_13 = (rb_state17.var_13) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_10) : (0xaaadc641));
        rb_state17.var_14 = (rb_state17.var_14) - (((0x60d962c5) + (0x4e8f6669)) ^ (rb_state17.var_11));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_12) : (0xcdadb421));
        rb_state17.var_16 = (rb_state17.var_16) ^ (((((0x1cd1686e) + (0x90a387fd)) + (0x3485720)) - (rb_state17.var_13)) ^ (rb_state17.var_14));
        rb_state17.var_17 = (rb_state17.var_17) ^ (rb_state17.var_15);
        rb_state17.var_18 = (rb_state17.var_18) ^ (((0xfb81da89) ^ (rb_state17.var_8)) + (rb_state17.var_16));
        rb_state17.var_19 = (rb_state17.var_19) ^ (rb_state17.var_17);
        rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_18);
        rb_state17.var_21 = (rb_state17.var_21) - (rb_state17.var_19);
        rb_state17.var_22 = (rb_state17.var_22) ^ (((0x4f33536e) + (rb_state17.var_20)) ^ (0xe48d684a));
        rb_state17.var_23 = (rb_state17.var_23) + (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_21) : (0xf82d9cb7));
        rb_state17.var_24 = (rb_state17.var_24) + (((0x30a37283) - (0x7777d109)) ^ (rb_state17.var_22));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_23) : (0x34b707be));
        rb_state17.var_26 = (rb_state17.var_26) + (((((0xd31956dc) - (rb_state17.var_24)) ^ (rb_state17.var_25)) ^ (0x533c2b67)) - (0x40e01818));
        rb_state17.var_3 = (rb_state17.var_3) ^ (rb_state17.var_26);
        rb_state17.var_2 = rb_state17.var_3;
        pthread_mutex_unlock(&(rb_state17.lock_30));
    }
    if ((rb_state17.var_1) == (0x56a0)) {
        if (!((rb_state17.var_2) == (rb_state17.var_3))) {
            racebench_trigger(17);
        }
    }
    #endif
    OutputX = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_14) == (0xef988b8b)) {
        rb_state2.var_21 = 0x3db12a0c;
        rb_state2.var_22 = 0xa7cd1d24;
        rb_state2.var_23 = (rb_state2.var_23) - (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_17) : (0xddf588d5));
        rb_state2.var_24 = (rb_state2.var_24) - (((0xb1366463) ^ (rb_state2.var_21)) ^ (rb_state2.var_20));
        rb_state2.var_25 = (rb_state2.var_25) ^ (((((0xc06ad475) ^ (rb_state2.var_21)) + (rb_state2.var_22)) + (0xf8a519d9)) + (rb_state2.var_16));
        rb_state2.var_26 = (rb_state2.var_26) ^ (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0x8020cf72));
        rb_state2.var_27 = (rb_state2.var_27) + (((0xa716f037) - (0x525b623f)) + (rb_state2.var_24));
        rb_state2.var_28 = (rb_state2.var_28) + (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_26) : (0x4fda2b05));
        rb_state2.var_29 = (rb_state2.var_29) + (((((0xe1fd409f) ^ (rb_state2.var_28)) + (rb_state2.var_23)) + (0x303429c4)) - (rb_state2.var_27));
        rb_state2.var_15 = (rb_state2.var_15) - (rb_state2.var_29);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x5b8e2524)) {
        if ((rb_state5.var_2) != (0x0)) {
            if (!((rb_state5.var_2) == (rb_state5.var_33))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x6f76e7ea)) {
        rb_state9.var_3 = 0x6e35429;
        rb_state9.var_4 = (rb_state9.var_4) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_4) : (0x71e451fb));
        rb_state9.var_5 = (rb_state9.var_5) - (((((0x8625d310) - (rb_state9.var_2)) ^ (rb_state9.var_3)) ^ (rb_state9.var_3)) - (rb_state9.var_5));
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0x107e9faf) + (0xd912ef15)) + (0xe1bd4d66)) ^ (rb_state9.var_5)) - (rb_state9.var_4));
        rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_6);
    }
    #endif
    return OutputX;
}

fptype BlkSchlsEqEuroNoDiv(fptype sptprice, fptype strike, fptype rate, fptype volatility, fptype time, int otype, float timet) {
    fptype OptionPrice;

    fptype xStockPrice;
    fptype xStrikePrice;
    fptype xRiskFreeRate;
    fptype xVolatility;
    fptype xTime;
    fptype xSqrtTime;

    fptype logValues;
    fptype xLogTerm;
    fptype xD1;
    fptype xD2;
    fptype xPowerTerm;
    fptype xDen;
    fptype d1;
    fptype d2;
    fptype FutureValueX;
    fptype NofXd1;
    fptype NofXd2;
    fptype NegNofXd1;
    fptype NegNofXd2;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (227 < rb_input_size ? (uint32_t)rb_input[227] : 0xddf62374);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xbfac3d1d)) {
        rb_state8.var_9 = 0xa0947c6b;
        rb_state8.var_10 = (rb_state8.var_10) + (245 < rb_input_size ? (uint32_t)rb_input[245] : 0xc3d1643a);
        rb_state8.var_11 = (rb_state8.var_11) ^ (0xb487f778);
        rb_state8.var_12 = (rb_state8.var_12) - (111 < rb_input_size ? (uint32_t)rb_input[111] : 0x8a17e19c);
        rb_state8.var_13 = (rb_state8.var_13) - (((0x707377e2) + (rb_state8.var_9)) + (rb_state8.var_7));
        rb_state8.var_14 = (rb_state8.var_14) ^ (rb_state8.var_9);
        rb_state8.var_15 = (rb_state8.var_15) - (((0x7705a328) - (rb_state8.var_8)) - (0x280a8aea));
        rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_10);
        rb_state8.var_17 = (rb_state8.var_17) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x7251b178));
        rb_state8.var_18 = (rb_state8.var_18) + (((0x3ed9ab44) - (rb_state8.var_10)) - (rb_state8.var_13));
        rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_14) : (0xd09c5e3a));
        rb_state8.var_20 = (rb_state8.var_20) ^ (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_16) : (0x6b5f3848));
        rb_state8.var_21 = (rb_state8.var_21) + (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_18) : (0x55f77df7));
        rb_state8.var_22 = (rb_state8.var_22) ^ (((((0xb473381) ^ (0x6016008a)) + (rb_state8.var_20)) + (rb_state8.var_19)) + (0xfab5420a));
        rb_state8.var_23 = (rb_state8.var_23) ^ (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_22) : (0xc5618737));
        rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xcf5134b1)) {
        pthread_mutex_lock(&(rb_state16.lock_38));
        rb_state16.var_12 = 0x9ae9ccd5;
        rb_state16.var_13 = 0x50aecd55;
        rb_state16.var_14 = (rb_state16.var_14) ^ (((0x54a6758e) - (rb_state16.var_4)) - (rb_state16.var_12));
        rb_state16.var_15 = (rb_state16.var_15) + (((((0x6fbb9ce2) + (rb_state16.var_5)) ^ (rb_state16.var_3)) + (0x9add8a75)) ^ (rb_state16.var_13));
        rb_state16.var_16 = (rb_state16.var_16) ^ (((0xbb41ca37) ^ (rb_state16.var_14)) - (0x9fce12a));
        rb_state16.var_17 = (rb_state16.var_17) ^ (rb_state16.var_15);
        rb_state16.var_18 = (rb_state16.var_18) + (rb_state16.var_16);
        rb_state16.var_19 = (rb_state16.var_19) - (rb_state16.var_17);
        rb_state16.var_20 = (rb_state16.var_20) + (((0x4b8b37c2) + (rb_state16.var_18)) + (rb_state16.var_6));
        rb_state16.var_21 = (rb_state16.var_21) ^ (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_20) : (0xa946e7d2));
        rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_21);
        rb_state16.var_2 = rb_state16.var_11;
        pthread_mutex_unlock(&(rb_state16.lock_38));
    }
    if ((rb_state16.var_0) == (0xcf5134b1)) {
        pthread_mutex_lock(&(rb_state16.lock_38));
        rb_state16.var_23 = 0x2a8bcf88;
        rb_state16.var_24 = (rb_state16.var_24) + (12 < rb_input_size ? (uint32_t)rb_input[12] : 0x2042da1c);
        rb_state16.var_25 = (rb_state16.var_25) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_9) : (0xd170e1ec));
        rb_state16.var_26 = (rb_state16.var_26) ^ (rb_state16.var_11);
        rb_state16.var_27 = (rb_state16.var_27) + (((0xc391e231) - (rb_state16.var_12)) + (rb_state16.var_8));
        rb_state16.var_28 = (rb_state16.var_28) ^ (((0xaed5201f) ^ (rb_state16.var_7)) + (0xde96b0fc));
        rb_state16.var_29 = (rb_state16.var_29) + (((0x94282610) ^ (rb_state16.var_23)) + (rb_state16.var_13));
        rb_state16.var_30 = (rb_state16.var_30) ^ (((((0xa4416ab4) - (rb_state16.var_24)) - (0xf45e5cf2)) ^ (rb_state16.var_14)) - (rb_state16.var_25));
        rb_state16.var_31 = (rb_state16.var_31) + (rb_state16.var_26);
        rb_state16.var_32 = (rb_state16.var_32) ^ (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_28) : (0x60b511e5));
        rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_30) : (0x6ba8e52b));
        rb_state16.var_34 = (rb_state16.var_34) ^ (((rb_state16.var_31) == (0x0)) ? (rb_state16.var_32) : (0x77c971a5));
        rb_state16.var_35 = (rb_state16.var_35) - (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_33) : (0xc1761ae9));
        rb_state16.var_36 = (rb_state16.var_36) ^ (rb_state16.var_34);
        rb_state16.var_37 = (rb_state16.var_37) - (((((0x132f6403) + (rb_state16.var_35)) ^ (rb_state16.var_16)) + (rb_state16.var_17)) + (rb_state16.var_36));
        rb_state16.var_22 = (rb_state16.var_22) + (rb_state16.var_37);
        rb_state16.var_2 = rb_state16.var_22;
        pthread_mutex_unlock(&(rb_state16.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x88afb348));
    #endif
    xStockPrice = sptprice;
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_16) == (0x1)) {
        rb_state18.var_25 = 0x6b80c98d;
        rb_state18.var_26 = 0xee2b6380;
        rb_state18.var_27 = (rb_state18.var_27) + (((0x96a59f31) ^ (rb_state18.var_26)) ^ (rb_state18.var_23));
        rb_state18.var_28 = (rb_state18.var_28) + (((((0xe21181c8) ^ (0xcf1a3ae3)) - (rb_state18.var_22)) + (0x47aca501)) ^ (rb_state18.var_25));
        rb_state18.var_29 = (rb_state18.var_29) - (((0x21b27442) - (rb_state18.var_27)) - (0x509e1b3f));
        rb_state18.var_30 = (rb_state18.var_30) ^ (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_28) : (0x82fb6198));
        rb_state18.var_31 = (rb_state18.var_31) - (((((0xf907665a) ^ (0xeede2a4b)) + (rb_state18.var_29)) - (rb_state18.var_25)) + (rb_state18.var_30));
        rb_state18.var_24 = (rb_state18.var_24) + (rb_state18.var_31);
        rb_state18.var_17 = rb_state18.var_24;
    }
    if ((rb_state18.var_16) == (0x1)) {
        pthread_mutex_lock(&(rb_state18.lock_47));
        rb_state18.var_33 = 0x79b3a9d9;
        rb_state18.var_34 = 0x6d0bba4b;
        rb_state18.var_35 = (rb_state18.var_35) + (rb_state18.var_26);
        rb_state18.var_36 = (rb_state18.var_36) ^ (199 < rb_input_size ? (uint32_t)rb_input[199] : 0x8761125a);
        rb_state18.var_37 = (rb_state18.var_37) + (((((0x365af081) + (rb_state18.var_27)) - (rb_state18.var_33)) ^ (0xbb6a30b5)) - (rb_state18.var_34));
        rb_state18.var_38 = (rb_state18.var_38) ^ (rb_state18.var_35);
        rb_state18.var_39 = (rb_state18.var_39) + (((0xde556502) ^ (rb_state18.var_28)) + (rb_state18.var_36));
        rb_state18.var_40 = (rb_state18.var_40) ^ (rb_state18.var_37);
        rb_state18.var_41 = (rb_state18.var_41) ^ (((((0x20480c38) - (rb_state18.var_38)) - (rb_state18.var_29)) - (0x121ca541)) - (rb_state18.var_39));
        rb_state18.var_42 = (rb_state18.var_42) - (((0xdec41a83) + (rb_state18.var_40)) + (rb_state18.var_30));
        rb_state18.var_43 = (rb_state18.var_43) - (((0x15e2a92e) - (rb_state18.var_41)) + (rb_state18.var_31));
        rb_state18.var_44 = (rb_state18.var_44) - (((rb_state18.var_32) == (0x0)) ? (rb_state18.var_42) : (0x6bb37698));
        rb_state18.var_45 = (rb_state18.var_45) ^ (rb_state18.var_43);
        rb_state18.var_46 = (rb_state18.var_46) + (((((0xab502448) - (rb_state18.var_45)) - (rb_state18.var_33)) ^ (rb_state18.var_44)) + (rb_state18.var_34));
        rb_state18.var_32 = (rb_state18.var_32) ^ (rb_state18.var_46);
        rb_state18.var_17 = rb_state18.var_32;
        pthread_mutex_unlock(&(rb_state18.lock_47));
    }
    #endif
    xStrikePrice = strike;
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x94e8d56d)) {
        pthread_mutex_lock(&(rb_state7.lock_14));
        rb_state7.var_4 = 0x7ba04686;
        rb_state7.var_5 = 0x21a9ee9a;
        rb_state7.var_6 = (rb_state7.var_6) + ((0x7c6f681) - (rb_state7.var_3));
        rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_2) : (0x4dee6e0d));
        rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0x364a16cd));
        rb_state7.var_9 = (rb_state7.var_9) ^ (rb_state7.var_6);
        rb_state7.var_10 = (rb_state7.var_10) ^ (((0x350ab7d4) ^ (rb_state7.var_7)) + (0xb4b1bc86));
        rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_9) : (0x1643fe91));
        rb_state7.var_12 = (rb_state7.var_12) - (((0x55ac660) ^ (0x16d5f097)) + (rb_state7.var_10));
        rb_state7.var_13 = (rb_state7.var_13) - (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_12) : (0x541bd619));
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_13);
        rb_state7.var_3 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (0x445f1985);
    #endif
    xRiskFreeRate = rate;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
    #endif
    xVolatility = volatility;

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xbcdb7a66)) {
        rb_state6.var_9 = 0x7aa0b56a;
        rb_state6.var_10 = 0x686348d6;
        rb_state6.var_11 = (rb_state6.var_11) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0xcc53e390));
        rb_state6.var_12 = (rb_state6.var_12) + (((0xf9e3b55) ^ (rb_state6.var_10)) + (rb_state6.var_7));
        rb_state6.var_13 = (rb_state6.var_13) + (((((0x1f23c000) - (0x2ebe6b5d)) + (rb_state6.var_9)) + (rb_state6.var_4)) + (0x1dca471e));
        rb_state6.var_14 = (rb_state6.var_14) + (((((0x1b9904ca) - (0xd8e3a4ac)) ^ (rb_state6.var_8)) ^ (rb_state6.var_12)) + (rb_state6.var_11));
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_13) : (0x91d8944d));
        rb_state6.var_16 = (rb_state6.var_16) + (rb_state6.var_14);
        rb_state6.var_17 = (rb_state6.var_17) - (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_15) : (0xffc62b06));
        rb_state6.var_18 = (rb_state6.var_18) ^ (((((0x30c7d461) ^ (rb_state6.var_11)) - (rb_state6.var_16)) - (0x91afa145)) ^ (rb_state6.var_17));
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_18);
    }
    #endif
    xTime = time;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xa944760e));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (187 < rb_input_size ? (uint32_t)rb_input[187] : 0x9911466f);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x4fe667d));
    #endif
    xSqrtTime = sqrt(xTime);

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_11) : (0x29d7aa01));
    #endif
    logValues = log(sptprice / strike);

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xda791eba)) {
        pthread_mutex_lock(&(rb_state1.lock_15));
        rb_state1.var_4 = (rb_state1.var_4) ^ (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_2) : (0x9b970770));
        rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_4);
        rb_state1.var_2 = rb_state1.var_3;
        pthread_mutex_unlock(&(rb_state1.lock_15));
    }
    if ((rb_state1.var_1) == (0xda791eba)) {
        pthread_mutex_lock(&(rb_state1.lock_15));
        if (!((rb_state1.var_2) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
        pthread_mutex_unlock(&(rb_state1.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0xbece98a0)) {
        rb_state11.var_2 = rb_state11.var_1;
    }
    if ((rb_state11.var_0) == (0xbece98a0)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (113 < rb_input_size ? (uint32_t)rb_input[113] : 0xbf52dc58);
    #endif
    xLogTerm = logValues;

    xPowerTerm = xVolatility * xVolatility;
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x913bacb7)) {
        if ((rb_state15.var_2) != (0x0)) {
            if (!((rb_state15.var_2) == (rb_state15.var_7))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    xPowerTerm = xPowerTerm * 0.5;

    xD1 = xRiskFreeRate + xPowerTerm;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0xbece98a0)) {
        rb_state11.var_3 = 0xbb0d041b;
        rb_state11.var_4 = 0xd59d6a87;
        rb_state11.var_5 = (rb_state11.var_5) + (125 < rb_input_size ? (uint32_t)rb_input[125] : 0x21a23a84);
        rb_state11.var_6 = (rb_state11.var_6) - (196 < rb_input_size ? (uint32_t)rb_input[196] : 0x4d4305bf);
        rb_state11.var_7 = (rb_state11.var_7) - (rb_state11.var_2);
        rb_state11.var_8 = (rb_state11.var_8) ^ (((0x98171a4c) + (rb_state11.var_4)) + (0xe40e4c8b));
        rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_3) : (0xde9aa22e));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((0x83467126) ^ (0xaabd8475)) - (rb_state11.var_5));
        rb_state11.var_11 = (rb_state11.var_11) - (rb_state11.var_6);
        rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_7);
        rb_state11.var_13 = (rb_state11.var_13) + (((0xad567966) - (rb_state11.var_8)) ^ (0x3590c42a));
        rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_9) : (0x508ffb52));
        rb_state11.var_15 = (rb_state11.var_15) - (rb_state11.var_10);
        rb_state11.var_16 = (rb_state11.var_16) ^ (rb_state11.var_11);
        rb_state11.var_17 = (rb_state11.var_17) + (((0x14485c16) ^ (0xbb28af87)) + (rb_state11.var_12));
        rb_state11.var_18 = (rb_state11.var_18) + (rb_state11.var_13);
        rb_state11.var_19 = (rb_state11.var_19) - (rb_state11.var_14);
        rb_state11.var_20 = (rb_state11.var_20) + (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x33c30a6));
        rb_state11.var_21 = (rb_state11.var_21) ^ (((0xe0d6f052) ^ (rb_state11.var_17)) + (0x8d6c2819));
        rb_state11.var_22 = (rb_state11.var_22) + (((0x11ec4099) + (rb_state11.var_18)) + (rb_state11.var_5));
        rb_state11.var_23 = (rb_state11.var_23) + (((0xa60cc544) ^ (rb_state11.var_19)) ^ (0xe0fff100));
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_21) : (0x3a28a1a));
        rb_state11.var_25 = (rb_state11.var_25) + (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_23) : (0x34473dda));
        rb_state11.var_26 = (rb_state11.var_26) ^ (rb_state11.var_24);
        rb_state11.var_27 = (rb_state11.var_27) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_25) : (0xb1dbd0c8));
        rb_state11.var_28 = (rb_state11.var_28) ^ (rb_state11.var_26);
        rb_state11.var_29 = (rb_state11.var_29) ^ (rb_state11.var_27);
        rb_state11.var_30 = (rb_state11.var_30) + (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_28) : (0xa1a5aee0));
        rb_state11.var_31 = (rb_state11.var_31) - (((rb_state11.var_29) == (0x0)) ? (rb_state11.var_30) : (0x7553f02));
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0x8c2b5fcb)) {
        rb_state12.var_2 = rb_state12.var_0;
    }
    if ((rb_state12.var_1) == (0x8c2b5fcb)) {
        if (!((rb_state12.var_0) == (rb_state12.var_2))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + ((0x83fa858) + (0x9ed70e77));
    #endif
    xD1 = xD1 * xTime;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
    #endif
    xD1 = xD1 + xLogTerm;

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (110 < rb_input_size ? (uint32_t)rb_input[110] : 0x8919756f);
    #endif
    xDen = xVolatility * xSqrtTime;
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xdfcc6d1a)) {
        rb_state14.var_2 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0xdfcc6d1a)) {
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
    }
    #endif
    xD1 = xD1 / xDen;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x20176889)) {
        rb_state3.var_3 = 0x9948cdf4;
        rb_state3.var_4 = 0x8b95cece;
        rb_state3.var_5 = (rb_state3.var_5) ^ (rb_state3.var_3);
        rb_state3.var_6 = (rb_state3.var_6) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x98c865ef));
        rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_4) : (0xfd3eeeb5));
        rb_state3.var_8 = (rb_state3.var_8) + (rb_state3.var_5);
        rb_state3.var_9 = (rb_state3.var_9) - (rb_state3.var_6);
        rb_state3.var_10 = (rb_state3.var_10) - (((0xa609b783) - (rb_state3.var_7)) + (0x295f61e9));
        rb_state3.var_11 = (rb_state3.var_11) ^ (((((0x2ef3b14f) ^ (rb_state3.var_8)) ^ (0x29b2a49b)) - (rb_state3.var_9)) + (rb_state3.var_5));
        rb_state3.var_12 = (rb_state3.var_12) + (((0x5c1df985) + (rb_state3.var_10)) ^ (0xb47a54e7));
        rb_state3.var_13 = (rb_state3.var_13) ^ (((((0x51ea8d91) ^ (rb_state3.var_6)) - (rb_state3.var_11)) ^ (0xf800798)) ^ (rb_state3.var_12));
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_13);
    }
    #endif
    xD2 = xD1 - xDen;

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_1);
    #endif
    d1 = xD1;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0xa5)) ? (rb_state5.var_0) : (0x65852c4f));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (0xa8fa7aa0);
    #endif
    d2 = xD2;

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_14) == (0xef988b8b)) {
        rb_state2.var_20 = rb_state2.var_15;
    }
    if ((rb_state2.var_14) == (0xef988b8b)) {
        if (!((rb_state2.var_15) == (rb_state2.var_20))) {
            racebench_trigger(2);
        }
    }
    #endif
    NofXd1 = CNDF(d1);
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xda791eba)) {
        rb_state1.var_5 = 0xfb1e0855;
        rb_state1.var_6 = (rb_state1.var_6) - (rb_state1.var_5);
        rb_state1.var_7 = (rb_state1.var_7) ^ (rb_state1.var_6);
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_4) : (0xb4761095));
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0xcb673791));
        rb_state1.var_10 = (rb_state1.var_10) - (((0xdf132db1) - (rb_state1.var_8)) ^ (rb_state1.var_8));
        rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_9) : (0xce8c0778));
        rb_state1.var_12 = (rb_state1.var_12) + (((0x218a042e) - (rb_state1.var_10)) + (rb_state1.var_10));
        rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_11) : (0x135ab846));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((((0x478f3557) ^ (rb_state1.var_12)) + (0xc6b3d792)) ^ (rb_state1.var_12)) ^ (rb_state1.var_13));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (216 < rb_input_size ? (uint32_t)rb_input[216] : 0x767d6b4f);
    rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_0);
    #endif
    NofXd2 = CNDF(d2);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + ((0xb8a1aa78) ^ (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (90 < rb_input_size ? (uint32_t)rb_input[90] : 0x5f7b96e9);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (0x94ef8830);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (146 < rb_input_size ? (uint32_t)rb_input[146] : 0x5778d09);
    #endif
    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_1) == (0x8c2b5fcb)) {
            pthread_mutex_lock(&(rb_state12.lock_6));
            rb_state12.var_3 = 0xfa9f8a30;
            rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_2);
            rb_state12.var_5 = (rb_state12.var_5) + (((((0x3c4bdd90) + (rb_state12.var_4)) ^ (rb_state12.var_4)) + (rb_state12.var_3)) ^ (rb_state12.var_5));
            rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_5);
            pthread_mutex_unlock(&(rb_state12.lock_6));
        }
        #endif
        NegNofXd1 = (1.0 - NofXd1);
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x3174c4a4)) {
            rb_state19.var_3 = 0x7e3009ed;
            rb_state19.var_4 = (rb_state19.var_4) - (((((0x18b0b447) + (rb_state19.var_3)) + (rb_state19.var_4)) ^ (0xeb0d1eda)) + (rb_state19.var_2));
            rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_4);
            rb_state19.var_1 = rb_state19.var_2;
        }
        if ((rb_state19.var_0) == (0x3174c4a4)) {
            pthread_mutex_lock(&(rb_state19.lock_13));
            rb_state19.var_6 = 0xc4798cba;
            rb_state19.var_7 = (rb_state19.var_7) - (((0x17e0455b) ^ (rb_state19.var_7)) ^ (rb_state19.var_6));
            rb_state19.var_8 = (rb_state19.var_8) ^ (0xd3d9c70);
            rb_state19.var_9 = (rb_state19.var_9) + (rb_state19.var_5);
            rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_7) : (0xaeabda4c));
            rb_state19.var_11 = (rb_state19.var_11) ^ (((((0x6fd2ef9) + (rb_state19.var_9)) ^ (rb_state19.var_10)) - (rb_state19.var_9)) - (rb_state19.var_8));
            rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_11) : (0xb440e58b));
            rb_state19.var_5 = (rb_state19.var_5) ^ (rb_state19.var_12);
            rb_state19.var_1 = rb_state19.var_5;
            pthread_mutex_unlock(&(rb_state19.lock_13));
        }
        #endif
        NegNofXd2 = (1.0 - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_0);
    #endif
    return OptionPrice;
}

#ifdef ENABLE_TBB
struct mainWork {
    mainWork() {
    }
    mainWork(mainWork &w, tbb::split) {
    }

    void operator()(const tbb::blocked_range<int> &range) const {
        fptype price;
        int begin = range.begin();
        int end = range.end();

        for (int i = begin; i != end; i++) {

            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
            prices[i] = price;

#ifdef ERR_CHK
            fptype priceDelta = data[i].DGrefval - price;
            if (fabs(priceDelta) >= 1e-5) {
                fprintf(stderr, "Error on %d. Computed=%.5f, Ref=%.5f, Delta=%.5f\n", i, price, data[i].DGrefval, priceDelta);
                numError++;
            }
#endif
        }
    }
};

#endif

#ifdef ENABLE_TBB
int bs_thread(void *tid_ptr) {
    int j;
    tbb::affinity_partitioner a;

    mainWork doall;
    for (j = 0; j < NUM_RUNS; j++) {
        tbb::parallel_for(tbb::blocked_range<int>(0, numOptions), doall, a);
    }

    return 0;
}
#else

#ifdef WIN32
DWORD WINAPI bs_thread(LPVOID tid_ptr) {
#else
int bs_thread(void *tid_ptr) {
#endif
    int i, j;
    fptype price;
    fptype priceDelta;
    int tid = *(int *)tid_ptr;
    int start = tid * (numOptions / nThreads);
    int end = start + (numOptions / nThreads);

    for (j = 0; j < NUM_RUNS; j++) {
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, price, priceDelta)
        for (i = 0; i < numOptions; i++) {
#else
        for (i = start; i < end; i++) {
#endif

            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ (209 < rb_input_size ? (uint32_t)rb_input[209] : 0xeaf9b92b);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) - (180 < rb_input_size ? (uint32_t)rb_input[180] : 0x15aece9e);
            #endif
            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x5aab70ec));
            rb_state14.var_1 = (rb_state14.var_1) ^ (0xacc41d95);
            #endif
            prices[i] = price;

#ifdef ERR_CHK
            priceDelta = data[i].DGrefval - price;
            if (fabs(priceDelta) >= 1e-4) {
                printf("Error on %d. Computed=%.5f, Ref=%.5f, Delta=%.5f\n", i, price, data[i].DGrefval, priceDelta);
                numError++;
            }
#endif
        }
    }

    return 0;
}
#endif

int main(int argc, char **argv) {
    FILE *file;
    int i;
    int loopnum;
    fptype *buffer;
    int *buffer2;
    int rv;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    printf("PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) "\n");
    fflush(NULL);
#else
    printf("PARSEC Benchmark Suite\n");
    fflush(NULL);
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_blackscholes);
#endif

    if (argc != 4) {
        printf("Usage:\n\t%s <nthreads> <inputFile> <outputFile>\n", argv[0]);
        exit(1);
    }
    nThreads = atoi(argv[1]);
    char *inputFile = argv[2];
    char *outputFile = argv[3];

    file = fopen(inputFile, "r");
    if (file == NULL) {
        printf("ERROR: Unable to open file %s.\n", inputFile);
        exit(1);
    }
    rv = fscanf(file, "%i", &numOptions);
    if (rv != 1) {
        printf("ERROR: Unable to read from file %s.\n", inputFile);
        fclose(file);
        exit(1);
    }
    if (nThreads > numOptions) {
        printf("WARNING: Not enough work, reducing number of threads to match number of options.\n");
        nThreads = numOptions;
    }

#if !defined(ENABLE_THREADS) && !defined(ENABLE_OPENMP) && !defined(ENABLE_TBB)
    if (nThreads != 1) {
        printf("Error: <nthreads> must be 1 (serial version)\n");
        exit(1);
    }
#endif

    data = (OptionData *)malloc(numOptions * sizeof(OptionData));
    prices = (fptype *)malloc(numOptions * sizeof(fptype));
    for (loopnum = 0; loopnum < numOptions; ++loopnum) {
        rv = fscanf(file, "%f %f %f %f %f %f %c %f %f", &data[loopnum].s, &data[loopnum].strike, &data[loopnum].r, &data[loopnum].divq, &data[loopnum].v, &data[loopnum].t, &data[loopnum].OptionType, &data[loopnum].divs, &data[loopnum].DGrefval);
        if (rv != 9) {
            printf("ERROR: Unable to read from file %s.\n", inputFile);
            fclose(file);
            exit(1);
        }
    }
    rv = fclose(file);
    if (rv != 0) {
        printf("ERROR: Unable to close file %s.\n", inputFile);
        exit(1);
    }

#ifdef ENABLE_THREADS

    pthread_mutexattr_init(&_M4_normalMutexAttr);

    _M4_numThreads = nThreads;
    {
        int _M4_i;
        for (_M4_i = 0; _M4_i < MAX_THREADS; _M4_i++) {
            _M4_threadsTableAllocated[_M4_i] = 0;
        }
    };
#endif
    printf("Num of Options: %d\n", numOptions);
    printf("Num of Runs: %d\n", NUM_RUNS);

#define PAD 256
#define LINESIZE 64

    buffer = (fptype *)malloc(5 * numOptions * sizeof(fptype) + PAD);
    sptprice = (fptype *)(((unsigned long long)buffer + PAD) & ~(LINESIZE - 1));
    strike = sptprice + numOptions;
    rate = strike + numOptions;
    volatility = rate + numOptions;
    otime = volatility + numOptions;

    buffer2 = (int *)malloc(numOptions * sizeof(fptype) + PAD);
    otype = (int *)(((unsigned long long)buffer2 + PAD) & ~(LINESIZE - 1));

    for (i = 0; i < numOptions; i++) {
        otype[i] = (data[i].OptionType == 'P') ? 1 : 0;
        sptprice[i] = data[i].s;
        strike[i] = data[i].strike;
        rate[i] = data[i].r;
        volatility[i] = data[i].v;
        otime[i] = data[i].t;
    }

    printf("Size of data: %d\n", numOptions * (sizeof(OptionData) + sizeof(int)));

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif

#ifdef ENABLE_THREADS
#ifdef WIN32
    HANDLE *threads;
    int *nums;
    threads = (HANDLE *)malloc(nThreads * sizeof(HANDLE));
    nums = (int *)malloc(nThreads * sizeof(int));

    for (i = 0; i < nThreads; i++) {
        nums[i] = i;
        threads[i] = CreateThread(0, 0, bs_thread, &nums[i], 0, 0);
    }
    WaitForMultipleObjects(nThreads, threads, TRUE, INFINITE);
    free(threads);
    free(nums);
#else
    int *tids;
    tids = (int *)malloc(nThreads * sizeof(int));

    for (i = 0; i < nThreads; i++) {
        tids[i] = i;

        {
            int _M4_i;
            for (_M4_i = 0; _M4_i < MAX_THREADS; _M4_i++) {
                if (_M4_threadsTableAllocated[_M4_i] == 0) {
                    break;
                }
            }
            pthread_create(&_M4_threadsTable[_M4_i], NULL, (void *(*)(void *))bs_thread, (void *)&tids[i]);
            _M4_threadsTableAllocated[_M4_i] = 1;
        };
    }

    {
        int _M4_i;
        void *_M4_ret;
        for (_M4_i = 0; _M4_i < MAX_THREADS; _M4_i++) {
            if (_M4_threadsTableAllocated[_M4_i] == 0) {
                break;
            }
            pthread_join(_M4_threadsTable[_M4_i], &_M4_ret);
        }
    };
    free(tids);
#endif
#else
#ifdef ENABLE_OPENMP
    {
        int tid = 0;
        omp_set_num_threads(nThreads);
        bs_thread(&tid);
    }
#else
#ifdef ENABLE_TBB
    tbb::task_scheduler_init init(nThreads);

    int tid = 0;
    bs_thread(&tid);
#else

    int tid = 0;
    bs_thread(&tid);
#endif
#endif
#endif

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    file = fopen(outputFile, "w");
    if (file == NULL) {
        printf("ERROR: Unable to open file %s.\n", outputFile);
        exit(1);
    }
    rv = fprintf(file, "%i\n", numOptions);
    if (rv < 0) {
        printf("ERROR: Unable to write to file %s.\n", outputFile);
        fclose(file);
        exit(1);
    }
    for (i = 0; i < numOptions; i++) {
        rv = fprintf(file, "%.18f\n", prices[i]);
        if (rv < 0) {
            printf("ERROR: Unable to write to file %s.\n", outputFile);
            fclose(file);
            exit(1);
        }
    }
    rv = fclose(file);
    if (rv != 0) {
        printf("ERROR: Unable to close file %s.\n", outputFile);
        exit(1);
    }

#ifdef ERR_CHK
    printf("Num Errors: %d\n", numError);
#endif
    free(data);
    free(prices);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif

    return 0;
}