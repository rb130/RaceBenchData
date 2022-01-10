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
        InputX = -InputX;
        sign = 1;
    } else {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_2) : (0x2d772dc0));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (34 < rb_input_size ? (uint32_t)rb_input[34] : 0xe19b329c);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (0x949fdaac);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) + (54 < rb_input_size ? (uint32_t)rb_input[54] : 0xcdac91cf);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_1) == (0x4941b629)) ? (rb_state15.var_0) : (0x7c03b538));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_1) : (0xba2a31ab));
        #endif
        sign = 0;
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (0x73d05ae9);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x68b75c04)) {
        rb_state7.var_3 = rb_state7.var_1;
    }
    if ((rb_state7.var_0) == (0x68b75c04)) {
        if (!((rb_state7.var_1) == (rb_state7.var_3))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x4370)) {
        rb_state9.var_7 = 0xcb94cf73;
        rb_state9.var_8 = 0xcfa3c766;
        rb_state9.var_9 = (rb_state9.var_9) ^ (((((0x4b145c6f) - (rb_state9.var_7)) - (rb_state9.var_7)) ^ (rb_state9.var_6)) ^ (rb_state9.var_5));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((0x2d19ae2) - (0xe2119e63)) ^ (rb_state9.var_8));
        rb_state9.var_11 = (rb_state9.var_11) + (rb_state9.var_9);
        rb_state9.var_12 = (rb_state9.var_12) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0x7b5366c9));
        rb_state9.var_6 = (rb_state9.var_6) + (rb_state9.var_12);
        rb_state9.var_3 = rb_state9.var_6;
    }
    if ((rb_state9.var_2) == (0x4370)) {
        if ((rb_state9.var_3) != (0x0)) {
            rb_state9.var_22 = !((rb_state9.var_3) == (rb_state9.var_13));
        }
    }
    if ((rb_state9.var_2) == (0x4370)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        rb_state9.var_14 = 0x2925d04a;
        rb_state9.var_15 = (rb_state9.var_15) - (((0xc7ad4221) + (rb_state9.var_14)) - (0x98bf5e93));
        rb_state9.var_16 = (rb_state9.var_16) ^ (((((0x38fc9f1f) - (rb_state9.var_9)) - (0xa1645a27)) - (rb_state9.var_8)) ^ (0x7d70bb4));
        rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_15) : (0x695cf0be));
        rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_16);
        rb_state9.var_19 = (rb_state9.var_19) - (rb_state9.var_17);
        rb_state9.var_20 = (rb_state9.var_20) + (((((0x6cb684cc) - (0x9e502155)) - (rb_state9.var_19)) - (0xe20010ae)) - (rb_state9.var_18));
        rb_state9.var_13 = (rb_state9.var_13) + (rb_state9.var_20);
        rb_state9.var_3 = rb_state9.var_13;
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (181 < rb_input_size ? (uint32_t)rb_input[181] : 0xa0923265);
    if ((rb_state11.var_0) == (0x48cbb512)) {
        pthread_mutex_lock(&(rb_state11.lock_14));
        rb_state11.var_5 = 0x9411e9d6;
        rb_state11.var_6 = 0x9d47abec;
        rb_state11.var_7 = (rb_state11.var_7) ^ ((0xbb35f654) - (0x177bbef8));
        rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_4);
        rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0xd2c3511c));
        rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_5);
        rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x624e3c52));
        rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_10) : (0x30a4ae9a));
        rb_state11.var_13 = (rb_state11.var_13) + (((((0xdcfc7058) - (rb_state11.var_12)) + (0x20e1a4a4)) + (rb_state11.var_11)) + (rb_state11.var_6));
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_13);
        pthread_mutex_unlock(&(rb_state11.lock_14));
    }
    #endif
    xInput = InputX;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x2c7ce322));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_1) : (0xac702e42));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (0x9a7a4428);
    #endif
    expValues = exp(-0.5f * InputX * InputX);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0x44e8d458));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (0xfdfeb77a);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_7 = (rb_state8.var_7) - (0x98a3b473);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_13) == (0x4e010480)) {
        rb_state9.var_24 = 0x1b93463c;
        rb_state9.var_25 = 0xccc8c7e5;
        rb_state9.var_26 = (rb_state9.var_26) + (rb_state9.var_25);
        rb_state9.var_27 = (rb_state9.var_27) - ((0xc79db5e9) ^ (rb_state9.var_17));
        rb_state9.var_28 = (rb_state9.var_28) + (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_14) : (0x33ad189));
        rb_state9.var_29 = (rb_state9.var_29) + (rb_state9.var_26);
        rb_state9.var_30 = (rb_state9.var_30) ^ (rb_state9.var_27);
        rb_state9.var_31 = (rb_state9.var_31) ^ (rb_state9.var_28);
        rb_state9.var_32 = (rb_state9.var_32) + (rb_state9.var_29);
        rb_state9.var_33 = (rb_state9.var_33) + (((0x155955b1) + (rb_state9.var_30)) + (rb_state9.var_18));
        rb_state9.var_34 = (rb_state9.var_34) + (((rb_state9.var_31) == (0x0)) ? (rb_state9.var_32) : (0xd28e4b35));
        rb_state9.var_35 = (rb_state9.var_35) - (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_33) : (0xe1c0f95d));
        rb_state9.var_36 = (rb_state9.var_36) + (((rb_state9.var_34) == (0x0)) ? (rb_state9.var_35) : (0x5073a805));
        rb_state9.var_16 = (rb_state9.var_16) + (rb_state9.var_36);
    }
    #endif
    xNPrimeofX = expValues;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (33 < rb_input_size ? (uint32_t)rb_input[33] : 0x95e26c50);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (0 < rb_input_size ? (uint32_t)rb_input[0] : 0x93a9ebf8);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ ((0x8edf5f7d) + (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x5c85b285)) {
        rb_state17.var_2 = rb_state17.var_0;
    }
    if ((rb_state17.var_1) == (0x5c85b285)) {
        if (!((rb_state17.var_0) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x4dfa5d86);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_17 = (rb_state4.var_17) ^ (193 < rb_input_size ? (uint32_t)rb_input[193] : 0x65236ec9);
    if ((rb_state4.var_20) == (0x4046c881)) {
        rb_state4.var_48 = 0xbd38db43;
        rb_state4.var_49 = (rb_state4.var_49) + (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_48) : (0xa835876));
        rb_state4.var_50 = (rb_state4.var_50) + (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_31) : (0xb30595ce));
        rb_state4.var_51 = (rb_state4.var_51) - (((rb_state4.var_35) == (0x0)) ? (rb_state4.var_33) : (0xd6ea62f3));
        rb_state4.var_52 = (rb_state4.var_52) - (((((0x1012a488) + (rb_state4.var_28)) + (0x530a07bf)) + (rb_state4.var_29)) + (rb_state4.var_36));
        rb_state4.var_53 = (rb_state4.var_53) + (((((0xda7dc7f8) ^ (0xe4d7e285)) + (0x9537869)) ^ (rb_state4.var_49)) ^ (rb_state4.var_50));
        rb_state4.var_54 = (rb_state4.var_54) + (((rb_state4.var_51) == (0x0)) ? (rb_state4.var_52) : (0xb036fe98));
        rb_state4.var_55 = (rb_state4.var_55) + (((((0x9ef28c1) ^ (0x7f62f6bf)) ^ (rb_state4.var_53)) - (rb_state4.var_54)) + (rb_state4.var_37));
        rb_state4.var_36 = (rb_state4.var_36) - (rb_state4.var_55);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_5) == (0x2ebfa7fa)) {
        pthread_mutex_lock(&(rb_state8.lock_31));
        rb_state8.var_12 = 0x36340d2b;
        rb_state8.var_13 = 0x3579c978;
        rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_12) : (0x3937bbf4));
        rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_9) : (0x53d1a51a));
        rb_state8.var_16 = (rb_state8.var_16) + (((((0x4c54948c) ^ (rb_state8.var_14)) - (rb_state8.var_12)) + (rb_state8.var_13)) + (0x5d77174f));
        rb_state8.var_17 = (rb_state8.var_17) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_15) : (0x324866fb));
        rb_state8.var_18 = (rb_state8.var_18) + (((0xca01c523) - (0x2c379e19)) - (rb_state8.var_16));
        rb_state8.var_19 = (rb_state8.var_19) ^ (((0x72de5a34) ^ (rb_state8.var_17)) + (rb_state8.var_14));
        rb_state8.var_20 = (rb_state8.var_20) + (((((0x590072c) ^ (rb_state8.var_15)) - (rb_state8.var_18)) + (0xcff4909b)) ^ (rb_state8.var_19));
        rb_state8.var_11 = (rb_state8.var_11) - (rb_state8.var_20);
        rb_state8.var_10 = rb_state8.var_11;
        pthread_mutex_unlock(&(rb_state8.lock_31));
    }
    if ((rb_state8.var_5) == (0x2ebfa7fa)) {
        if (!((rb_state8.var_10) == (rb_state8.var_11))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0x4dfeb670);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - ((0x8a695201) - (rb_state18.var_0));
    if ((rb_state18.var_1) == (0x6d6a6467)) {
        pthread_mutex_lock(&(rb_state18.lock_28));
        rb_state18.var_4 = 0x9135ccba;
        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_4);
        rb_state18.var_6 = (rb_state18.var_6) ^ (((((0x593655e8) ^ (rb_state18.var_3)) + (rb_state18.var_2)) + (rb_state18.var_5)) + (0x3518f8d0));
        rb_state18.var_7 = (rb_state18.var_7) ^ (((((0x9f6c6809) - (rb_state18.var_4)) + (0xf0c67534)) - (0x293b1fd2)) + (rb_state18.var_5));
        rb_state18.var_8 = (rb_state18.var_8) ^ (rb_state18.var_6);
        rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0x30833cc8));
        rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0xaefc8930));
        rb_state18.var_11 = (rb_state18.var_11) + (rb_state18.var_9);
        rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_10);
        rb_state18.var_13 = (rb_state18.var_13) ^ (((((0xacc32d26) + (0x29fc1c8e)) + (0x1a433244)) ^ (rb_state18.var_11)) + (rb_state18.var_12));
        rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_13);
        rb_state18.var_2 = rb_state18.var_3;
        pthread_mutex_unlock(&(rb_state18.lock_28));
    }
    if ((rb_state18.var_1) == (0x6d6a6467)) {
        if (!((rb_state18.var_2) == (rb_state18.var_3))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_2) : (0x8dedb0bd));
    #endif
    xK2 = 0.2316419 * xInput;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x73164b09)) {
        rb_state0.var_12 = 0x734a9fee;
        rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_4) : (0xdce80a1a));
        rb_state0.var_14 = (rb_state0.var_14) - (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_12) : (0x993713db));
        rb_state0.var_15 = (rb_state0.var_15) - ((0x6509494e) - (0x7287ff1e));
        rb_state0.var_16 = (rb_state0.var_16) ^ (((0xe015e4d8) + (0x70d74baf)) ^ (rb_state0.var_13));
        rb_state0.var_17 = (rb_state0.var_17) ^ (rb_state0.var_14);
        rb_state0.var_18 = (rb_state0.var_18) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x2ca4619d));
        rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_18) : (0x1ef2fc28));
        rb_state0.var_11 = (rb_state0.var_11) ^ (rb_state0.var_19);
        rb_state0.var_3 = rb_state0.var_11;
    }
    if ((rb_state0.var_0) == (0x73164b09)) {
        pthread_mutex_lock(&(rb_state0.lock_33));
        rb_state0.var_21 = 0x542123f4;
        rb_state0.var_22 = 0xbc826d05;
        rb_state0.var_23 = (rb_state0.var_23) ^ ((0xb8ac9e40) ^ (0xbc77444c));
        rb_state0.var_24 = (rb_state0.var_24) - (131 < rb_input_size ? (uint32_t)rb_input[131] : 0x58b94f25);
        rb_state0.var_25 = (rb_state0.var_25) + (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_22) : (0x1f3a7935));
        rb_state0.var_26 = (rb_state0.var_26) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_7) : (0xa7a11e7c));
        rb_state0.var_27 = (rb_state0.var_27) - (((0xc64ed3a3) ^ (rb_state0.var_9)) - (rb_state0.var_23));
        rb_state0.var_28 = (rb_state0.var_28) ^ (((0x1be7bf5c) - (rb_state0.var_24)) - (0xda9b3669));
        rb_state0.var_29 = (rb_state0.var_29) ^ (((rb_state0.var_25) == (0x0)) ? (rb_state0.var_26) : (0x6af0327a));
        rb_state0.var_30 = (rb_state0.var_30) - (((0xe6fedc1) + (rb_state0.var_27)) + (0x642b7784));
        rb_state0.var_31 = (rb_state0.var_31) - (((((0x1e720b5e) - (0x1714ad47)) - (rb_state0.var_29)) + (rb_state0.var_28)) - (rb_state0.var_10));
        rb_state0.var_32 = (rb_state0.var_32) - (((rb_state0.var_30) == (0x0)) ? (rb_state0.var_31) : (0x413126c8));
        rb_state0.var_20 = (rb_state0.var_20) ^ (rb_state0.var_32);
        rb_state0.var_3 = rb_state0.var_20;
        pthread_mutex_unlock(&(rb_state0.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x40dcca4e)) {
        pthread_mutex_lock(&(rb_state1.lock_43));
        if ((rb_state1.var_3) != (0x0)) {
            if (!((rb_state1.var_3) == (rb_state1.var_31))) {
                racebench_trigger(1);
            }
        }
        pthread_mutex_unlock(&(rb_state1.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (0xf6e1880b);
    rb_state4.var_18 = (rb_state4.var_18) - (38 < rb_input_size ? (uint32_t)rb_input[38] : 0x5697dd4c);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (0x7dba27b3);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (0x8797ec51);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (0x2bf9ecb);
    #endif
    xK2 = 1.0 + xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xbed5bca4) - (0xc3e8b033));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xb205ef67)) {
        if ((rb_state2.var_3) != (0x0)) {
            if (!((rb_state2.var_3) == (rb_state2.var_25))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x22fcd852)) {
        rb_state5.var_15 = rb_state5.var_8;
    }
    if ((rb_state5.var_3) == (0x22fcd852)) {
        if (!((rb_state5.var_8) == (rb_state5.var_15))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_12 = (rb_state9.var_12) - ((0xa7b29b6d) ^ (0x20050fd3));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x483064b3)) {
        pthread_mutex_lock(&(rb_state10.lock_15));
        rb_state10.var_4 = (rb_state10.var_4) - (133 < rb_input_size ? (uint32_t)rb_input[133] : 0x1c002237);
        rb_state10.var_5 = (rb_state10.var_5) - ((0x6cdbad14) + (rb_state10.var_4));
        rb_state10.var_6 = (rb_state10.var_6) - (((((0x1d15a758) ^ (rb_state10.var_5)) ^ (rb_state10.var_6)) + (rb_state10.var_3)) ^ (rb_state10.var_2));
        rb_state10.var_7 = (rb_state10.var_7) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_4) : (0xc58e0040));
        rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0xf7c6b329));
        rb_state10.var_9 = (rb_state10.var_9) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0xda7caefe));
        rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_9);
        rb_state10.var_2 = rb_state10.var_3;
        pthread_mutex_unlock(&(rb_state10.lock_15));
    }
    if ((rb_state10.var_0) == (0x483064b3)) {
        if (!((rb_state10.var_2) == (rb_state10.var_3))) {
            racebench_trigger(10);
        }
    }
    #endif
    xK2 = 1.0 / xK2;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x73164b09)) {
        if ((rb_state0.var_3) != (0x0)) {
            if (!((rb_state0.var_3) == (rb_state0.var_20))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (0xeb679082);
    rb_state1.var_2 = (rb_state1.var_2) + (0x1e02a041);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0xf9fc266e)) ? (rb_state4.var_1) : (0xdeba509c));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_13) == (0x4e010480)) {
        rb_state9.var_23 = rb_state9.var_16;
    }
    if ((rb_state9.var_13) == (0x4e010480)) {
        if (!((rb_state9.var_16) == (rb_state9.var_23))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ ((0x528a433a) ^ (0x52885f59));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) + (((rb_state19.var_1) == (0x8f508f8b)) ? (rb_state19.var_0) : (0x242a97d6));
    #endif
    xK2_2 = xK2 * xK2;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_1) : (0xeb287e1a));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_19 = (rb_state4.var_19) - ((0xddd1b1e0) - (0xe838ff01));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_8 = (rb_state5.var_8) + ((0x666b5de7) - (0x35aa7832));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (220 < rb_input_size ? (uint32_t)rb_input[220] : 0x7cc6e83e);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0x893cf395) - (0x738db120));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (0xf2d11ed5);
    rb_state13.var_1 = (rb_state13.var_1) + ((0xd93619c7) + (0xa5765a89));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (197 < rb_input_size ? (uint32_t)rb_input[197] : 0xb4fb01b9);
    #endif
    xK2_3 = xK2_2 * xK2;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (0x1a8316a6);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x3f32c325)) {
        rb_state4.var_6 = 0xaa3226a;
        rb_state4.var_7 = 0x40045fe8;
        rb_state4.var_8 = (rb_state4.var_8) - ((0x1b57446d) - (0x389b9bd5));
        rb_state4.var_9 = (rb_state4.var_9) ^ (((0x713a1f0f) - (0x661fc63d)) + (rb_state4.var_6));
        rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_7) : (0x6647d412));
        rb_state4.var_11 = (rb_state4.var_11) ^ (((((0x6b37fe7e) - (rb_state4.var_6)) - (rb_state4.var_8)) ^ (0xb10768b7)) - (rb_state4.var_4));
        rb_state4.var_12 = (rb_state4.var_12) + (rb_state4.var_9);
        rb_state4.var_13 = (rb_state4.var_13) - (((0xe36b95f5) + (0xf8bbcd1)) - (rb_state4.var_10));
        rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_12) : (0x485a9f41));
        rb_state4.var_15 = (rb_state4.var_15) + (((0x1841766c) + (0x78d6cff3)) - (rb_state4.var_13));
        rb_state4.var_16 = (rb_state4.var_16) ^ (rb_state4.var_14);
        rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_15) : (0x1ef8f9c));
        rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_16) : (0x90f9d352));
        rb_state4.var_19 = (rb_state4.var_19) + (rb_state4.var_17);
        rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_18) == (0xffffff76)) ? (rb_state4.var_19) : (0x4046c881));
        rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_20);
        rb_state4.var_2 = rb_state4.var_5;
    }
    if ((rb_state4.var_0) == (0x3f32c325)) {
        pthread_mutex_lock(&(rb_state4.lock_34));
        rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_11);
        rb_state4.var_23 = (rb_state4.var_23) + (0x183549c7);
        rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_10) : (0x5ff8f20b));
        rb_state4.var_25 = (rb_state4.var_25) - (((0x26b3848d) + (0x69203574)) ^ (rb_state4.var_9));
        rb_state4.var_26 = (rb_state4.var_26) ^ (rb_state4.var_22);
        rb_state4.var_27 = (rb_state4.var_27) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_23) : (0x6dec5b70));
        rb_state4.var_28 = (rb_state4.var_28) ^ (((0x6d7c1961) - (rb_state4.var_24)) ^ (0x639a3fb5));
        rb_state4.var_29 = (rb_state4.var_29) + (((((0x48d755fc) ^ (rb_state4.var_26)) ^ (0xda98f3bd)) + (rb_state4.var_14)) - (rb_state4.var_25));
        rb_state4.var_30 = (rb_state4.var_30) + (rb_state4.var_27);
        rb_state4.var_31 = (rb_state4.var_31) + (((0xef5d8f3a) + (rb_state4.var_15)) - (rb_state4.var_28));
        rb_state4.var_32 = (rb_state4.var_32) - (((((0x22f385f2) - (rb_state4.var_16)) - (rb_state4.var_30)) ^ (0x4ee26a13)) - (rb_state4.var_29));
        rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_32) : (0x4c7ac064));
        rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_33);
        rb_state4.var_2 = rb_state4.var_21;
        pthread_mutex_unlock(&(rb_state4.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x22fcd852)) {
        pthread_mutex_lock(&(rb_state5.lock_25));
        rb_state5.var_16 = 0x12012900;
        rb_state5.var_17 = 0x787d8b2d;
        rb_state5.var_18 = (rb_state5.var_18) - (((((0x382e59f7) ^ (rb_state5.var_17)) + (0x427c4c2f)) - (rb_state5.var_15)) - (rb_state5.var_16));
        rb_state5.var_19 = (rb_state5.var_19) - (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_16) : (0xcfb80070));
        rb_state5.var_20 = (rb_state5.var_20) + (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_18) : (0x30835fce));
        rb_state5.var_21 = (rb_state5.var_21) + (rb_state5.var_19);
        rb_state5.var_22 = (rb_state5.var_22) ^ (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_20) : (0x6a29d737));
        rb_state5.var_23 = (rb_state5.var_23) + (rb_state5.var_21);
        rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_23) : (0x35492efa));
        rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_24);
        pthread_mutex_unlock(&(rb_state5.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (((rb_state6.var_1) == (0x45d24c46)) ? (rb_state6.var_0) : (0x81d19d57));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (79 < rb_input_size ? (uint32_t)rb_input[79] : 0x90a88cce);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x48cbb512)) {
        pthread_mutex_lock(&(rb_state11.lock_14));
        rb_state11.var_4 = (rb_state11.var_4) ^ (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_3) : (0x224b2d82));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_4);
        rb_state11.var_2 = rb_state11.var_3;
        pthread_mutex_unlock(&(rb_state11.lock_14));
    }
    if ((rb_state11.var_0) == (0x48cbb512)) {
        pthread_mutex_lock(&(rb_state11.lock_14));
        if (!((rb_state11.var_2) == (rb_state11.var_3))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x91275577)) {
        rb_state15.var_4 = 0xaec90697;
        rb_state15.var_5 = 0x9af63cbd;
        rb_state15.var_6 = (rb_state15.var_6) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_4) : (0x5b641c1e));
        rb_state15.var_7 = (rb_state15.var_7) + (rb_state15.var_3);
        rb_state15.var_8 = (rb_state15.var_8) ^ (rb_state15.var_6);
        rb_state15.var_9 = (rb_state15.var_9) - (((((0x36844744) ^ (rb_state15.var_7)) + (rb_state15.var_8)) ^ (rb_state15.var_4)) - (0x106ad027));
        rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_9);
    }
    #endif
    xK2_4 = xK2_3 * xK2;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (0xb2907f38);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (84 < rb_input_size ? (uint32_t)rb_input[84] : 0xea6fbdad);
    #endif
    xK2_5 = xK2_4 * xK2;

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_2);
    rb_state8.var_6 = (rb_state8.var_6) - (38 < rb_input_size ? (uint32_t)rb_input[38] : 0x2b7e4342);
    if ((rb_state8.var_5) == (0x2ebfa7fa)) {
        rb_state8.var_21 = 0x47d6fa7;
        rb_state8.var_22 = 0xa0968a65;
        rb_state8.var_23 = (rb_state8.var_23) + (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_17) : (0x7dbeaaba));
        rb_state8.var_24 = (rb_state8.var_24) + (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_22) : (0xf1c0a39a));
        rb_state8.var_25 = (rb_state8.var_25) + (rb_state8.var_21);
        rb_state8.var_26 = (rb_state8.var_26) ^ (rb_state8.var_16);
        rb_state8.var_27 = (rb_state8.var_27) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_23) : (0xe08e42d8));
        rb_state8.var_28 = (rb_state8.var_28) ^ (((((0xfcf3027c) + (rb_state8.var_25)) - (0x20dcd689)) + (rb_state8.var_21)) + (rb_state8.var_24));
        rb_state8.var_29 = (rb_state8.var_29) - (((((0xae67d357) + (rb_state8.var_22)) ^ (0x2133e91b)) + (rb_state8.var_26)) ^ (rb_state8.var_27));
        rb_state8.var_30 = (rb_state8.var_30) ^ (((((0x68d7ff48) ^ (rb_state8.var_28)) - (0xd2916b3e)) + (rb_state8.var_23)) + (rb_state8.var_29));
        rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_30);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + ((0x7c6d70e4) ^ (rb_state9.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (0x20888d50);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (0x8a6aaad0);
    #endif
    xLocal_1 = xK2 * 0.319381530;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + ((0x3bd5674f) ^ (rb_state2.var_2));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (110 < rb_input_size ? (uint32_t)rb_input[110] : 0x6415e995);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x385309f7)) {
        rb_state13.var_17 = 0xf6d6470b;
        rb_state13.var_18 = 0xbf783ead;
        rb_state13.var_19 = 0xab954256;
        rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_18) : (0x612ca71f));
        rb_state13.var_21 = (rb_state13.var_21) + (((((0x99a51077) - (rb_state13.var_10)) ^ (rb_state13.var_9)) - (rb_state13.var_20)) ^ (rb_state13.var_17));
        rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_1);
    #endif
    xLocal_2 = xK2_2 * (-0.356563782);
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_1) == (0x717d9325)) ? (rb_state13.var_0) : (0xa064aa59));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x6d56f9f9)) {
        rb_state16.var_12 = 0x7194d646;
        rb_state16.var_13 = 0x154ab3ef;
        rb_state16.var_14 = (rb_state16.var_14) - (0x33e8a411);
        rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_12) : (0xe9a373e9));
        rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_5) : (0x1063b226));
        rb_state16.var_17 = (rb_state16.var_17) ^ (((0xf2551f60) ^ (rb_state16.var_7)) + (rb_state16.var_14));
        rb_state16.var_18 = (rb_state16.var_18) ^ (rb_state16.var_15);
        rb_state16.var_19 = (rb_state16.var_19) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_16) : (0xa8e08bdc));
        rb_state16.var_20 = (rb_state16.var_20) ^ (((0xfc7f34d8) - (0x7bc26fd7)) - (rb_state16.var_17));
        rb_state16.var_21 = (rb_state16.var_21) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_18) : (0x5ab8e5f8));
        rb_state16.var_22 = (rb_state16.var_22) + (((((0x20dffd61) ^ (rb_state16.var_19)) + (rb_state16.var_10)) ^ (rb_state16.var_20)) + (0xe9161a2f));
        rb_state16.var_23 = (rb_state16.var_23) ^ (((0x4b8fdf40) + (rb_state16.var_21)) - (rb_state16.var_11));
        rb_state16.var_24 = (rb_state16.var_24) ^ (((((0xe9729674) ^ (rb_state16.var_22)) + (0x453d24aa)) + (rb_state16.var_23)) - (rb_state16.var_12));
        rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (0x39a366a8);
    #endif
    xLocal_3 = xK2_3 * 1.781477937;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_10);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (152 < rb_input_size ? (uint32_t)rb_input[152] : 0xe4f1bce2);
    if ((rb_state8.var_1) == (0xe4732b74)) {
        rb_state8.var_9 = !((rb_state8.var_4) == (rb_state8.var_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_15 = (rb_state9.var_15) + (0x4ba312af);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_1) == (0xbe12630e)) ? (rb_state14.var_0) : (0xb8dda742));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (0x560ddf20);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (0xe97c862d);
    #endif
    xLocal_3 = xK2_4 * (-1.821255978);
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0xe4732b74)) {
        pthread_mutex_lock(&(rb_state8.lock_8));
        rb_state8.var_5 = 0x2ebfa7fa;
        rb_state8.var_6 = (rb_state8.var_6) ^ (((((0x5ffc50ef) - (0xbe5d3e56)) ^ (rb_state8.var_5)) - (0x1141290c)) ^ (rb_state8.var_3));
        rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_6) : (0x78582428));
        rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_7);
        rb_state8.var_4 = rb_state8.var_3;
        pthread_mutex_unlock(&(rb_state8.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x2adad470));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x385309f7)) {
        pthread_mutex_lock(&(rb_state13.lock_22));
        rb_state13.var_5 = 0x89529ff3;
        rb_state13.var_6 = 0xb15c73f8;
        rb_state13.var_7 = (rb_state13.var_7) + (0xd630591b);
        rb_state13.var_8 = (rb_state13.var_8) ^ (((((0x40f6e756) - (rb_state13.var_4)) - (0x4979d7c9)) ^ (rb_state13.var_3)) + (rb_state13.var_6));
        rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_5) : (0xba8f6d9b));
        rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0x9deacefa));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0x8a88c6b8));
        rb_state13.var_12 = (rb_state13.var_12) - (rb_state13.var_9);
        rb_state13.var_13 = (rb_state13.var_13) + (rb_state13.var_10);
        rb_state13.var_14 = (rb_state13.var_14) ^ (((0x6aeb69f8) ^ (rb_state13.var_11)) - (0x8ffae3c6));
        rb_state13.var_15 = (rb_state13.var_15) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_13) : (0x19aefb74));
        rb_state13.var_16 = (rb_state13.var_16) + (((((0xcffe7040) - (0x4ba31922)) ^ (rb_state13.var_14)) + (rb_state13.var_8)) + (rb_state13.var_15));
        rb_state13.var_4 = (rb_state13.var_4) - (rb_state13.var_16);
        rb_state13.var_3 = rb_state13.var_4;
        pthread_mutex_unlock(&(rb_state13.lock_22));
    }
    if ((rb_state13.var_2) == (0x385309f7)) {
        pthread_mutex_lock(&(rb_state13.lock_22));
        if (!((rb_state13.var_3) == (rb_state13.var_4))) {
            racebench_trigger(13);
        }
        pthread_mutex_unlock(&(rb_state13.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x560ddf20)) ? (rb_state16.var_0) : (0x10b768c2));
    #endif
    xLocal_3 = xK2_5 * 1.330274429;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + ((0x4bfacc3e) ^ (0x89bc6007));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xb205ef67)) {
        pthread_mutex_lock(&(rb_state2.lock_29));
        rb_state2.var_11 = 0xf4f57b1c;
        rb_state2.var_12 = 0x6068d824;
        rb_state2.var_13 = (rb_state2.var_13) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_11) : (0x171c3160));
        rb_state2.var_14 = (rb_state2.var_14) ^ ((0xfe600b10) ^ (rb_state2.var_7));
        rb_state2.var_15 = (rb_state2.var_15) + (((0x9f2f535e) - (0xdef3c3d6)) ^ (rb_state2.var_5));
        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_12);
        rb_state2.var_17 = (rb_state2.var_17) - (((0x2dd07f) ^ (rb_state2.var_8)) + (rb_state2.var_13));
        rb_state2.var_18 = (rb_state2.var_18) + (((0xd4c050ff) ^ (0x9b00aeb7)) + (rb_state2.var_14));
        rb_state2.var_19 = (rb_state2.var_19) + (((((0xbcebe996) ^ (rb_state2.var_15)) + (rb_state2.var_16)) + (0x6952a0ca)) + (0x67223fd9));
        rb_state2.var_20 = (rb_state2.var_20) + (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_17) : (0xfea7f35));
        rb_state2.var_21 = (rb_state2.var_21) + (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_19) : (0x6a0d2fe5));
        rb_state2.var_22 = (rb_state2.var_22) ^ (((0x6f522032) ^ (rb_state2.var_10)) - (rb_state2.var_20));
        rb_state2.var_23 = (rb_state2.var_23) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_21) : (0xd4b90762));
        rb_state2.var_24 = (rb_state2.var_24) ^ (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0xd3340f5c));
        rb_state2.var_10 = (rb_state2.var_10) + (rb_state2.var_24);
        rb_state2.var_3 = rb_state2.var_10;
        pthread_mutex_unlock(&(rb_state2.lock_29));
    }
    if ((rb_state2.var_0) == (0xb205ef67)) {
        pthread_mutex_lock(&(rb_state2.lock_29));
        rb_state2.var_26 = (rb_state2.var_26) ^ (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_14) : (0x23e49b8c));
        rb_state2.var_27 = (rb_state2.var_27) ^ (rb_state2.var_12);
        rb_state2.var_28 = (rb_state2.var_28) + (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_27) : (0xb1cfac9d));
        rb_state2.var_25 = (rb_state2.var_25) ^ (rb_state2.var_28);
        rb_state2.var_3 = rb_state2.var_25;
        pthread_mutex_unlock(&(rb_state2.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (0xa5eaf3c);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x68b75c04)) {
        rb_state7.var_4 = 0xac6b23e0;
        rb_state7.var_5 = (rb_state7.var_5) + (((0xa2fce63f) ^ (rb_state7.var_4)) + (rb_state7.var_5));
        rb_state7.var_6 = (rb_state7.var_6) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_3) : (0x4e6c5702));
        rb_state7.var_7 = (rb_state7.var_7) ^ ((0x99b1bd69) ^ (0x1bedb176));
        rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_5) : (0x70797f7e));
        rb_state7.var_9 = (rb_state7.var_9) - (((((0xe663f990) ^ (0xce68a54c)) + (rb_state7.var_6)) + (rb_state7.var_7)) ^ (0x23dd67fd));
        rb_state7.var_10 = (rb_state7.var_10) - (((0x7e8aa22f) - (rb_state7.var_8)) + (rb_state7.var_8));
        rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_9) : (0xa26a1832));
        rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0xbc4b939b));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_6 = (rb_state8.var_6) - (5 < rb_input_size ? (uint32_t)rb_input[5] : 0x3ad84b7b);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (54 < rb_input_size ? (uint32_t)rb_input[54] : 0xce0330c5);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xe507c92a)) {
        pthread_mutex_lock(&(rb_state6.lock_7));
        rb_state6.var_2 = rb_state6.var_1;
        pthread_mutex_unlock(&(rb_state6.lock_7));
    }
    if ((rb_state6.var_0) == (0xe507c92a)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_0) == (0x797d111c)) ? (rb_state8.var_2) : (0xbc2e88c1));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + ((0x60a6b8b5) ^ (0x95e8f9c6));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (208 < rb_input_size ? (uint32_t)rb_input[208] : 0x10dced07);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x6d56f9f9)) {
        pthread_mutex_lock(&(rb_state16.lock_25));
        rb_state16.var_5 = 0x4c7483ac;
        rb_state16.var_6 = 0xb85360b4;
        rb_state16.var_7 = 0xab697e3a;
        rb_state16.var_8 = (rb_state16.var_8) - (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_6) : (0x611718e9));
        rb_state16.var_9 = (rb_state16.var_9) ^ (((((0x7818254f) - (rb_state16.var_4)) + (0x4baea431)) + (rb_state16.var_7)) ^ (rb_state16.var_5));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((0x3a75efc) - (rb_state16.var_8)) ^ (0x722d4874));
        rb_state16.var_11 = (rb_state16.var_11) + (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0x4b7ed58d));
        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_11);
        rb_state16.var_3 = rb_state16.var_4;
        pthread_mutex_unlock(&(rb_state16.lock_25));
    }
    if ((rb_state16.var_2) == (0x6d56f9f9)) {
        if (!((rb_state16.var_3) == (rb_state16.var_4))) {
            racebench_trigger(16);
        }
    }
    #endif
    xLocal_1 = xLocal_2 + xLocal_1;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + ((0xcfae1fae) + (0xbc1362de));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x5514154));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_20) == (0x4046c881)) {
        pthread_mutex_lock(&(rb_state4.lock_56));
        rb_state4.var_38 = 0x45888f36;
        rb_state4.var_39 = (rb_state4.var_39) + (rb_state4.var_21);
        rb_state4.var_40 = (rb_state4.var_40) + (((((0xee6a6df5) ^ (0x46f80c52)) + (0x145ff545)) + (rb_state4.var_38)) + (rb_state4.var_22));
        rb_state4.var_41 = (rb_state4.var_41) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_23) : (0x2b28b565));
        rb_state4.var_42 = (rb_state4.var_42) + (((0x213f14aa) - (rb_state4.var_25)) ^ (rb_state4.var_39));
        rb_state4.var_43 = (rb_state4.var_43) ^ (((0x4f9bbe3e) + (rb_state4.var_40)) + (rb_state4.var_26));
        rb_state4.var_44 = (rb_state4.var_44) + (rb_state4.var_41);
        rb_state4.var_45 = (rb_state4.var_45) ^ (((0x504dee9b) - (rb_state4.var_42)) ^ (rb_state4.var_27));
        rb_state4.var_46 = (rb_state4.var_46) - (((rb_state4.var_43) == (0x0)) ? (rb_state4.var_44) : (0xec6817a6));
        rb_state4.var_47 = (rb_state4.var_47) ^ (((((0x4e61404f) - (rb_state4.var_45)) ^ (0x3ded678c)) + (rb_state4.var_46)) + (0x7575946a));
        rb_state4.var_37 = (rb_state4.var_37) - (rb_state4.var_47);
        rb_state4.var_36 = rb_state4.var_37;
        pthread_mutex_unlock(&(rb_state4.lock_56));
    }
    if ((rb_state4.var_20) == (0x4046c881)) {
        if (!((rb_state4.var_36) == (rb_state4.var_37))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (84 < rb_input_size ? (uint32_t)rb_input[84] : 0x53df9d3f);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xfc4618e8));
    #endif
    xLocal = xLocal_1 * xNPrimeofX;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0x1564e1db);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x5c85b285)) {
        pthread_mutex_lock(&(rb_state17.lock_5));
        rb_state17.var_3 = 0x167c5bed;
        rb_state17.var_4 = (rb_state17.var_4) + (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_3) : (0xa5a26944));
        rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_4);
        pthread_mutex_unlock(&(rb_state17.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_0);
    #endif
    xLocal = 1.0 - xLocal;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
    if ((rb_state3.var_0) == (0x70535d24)) {
        if ((rb_state3.var_3) != (0x0)) {
            if (!((rb_state3.var_3) == (rb_state3.var_17))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ ((0xdc4a309a) + (0xc3914c64));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (26 < rb_input_size ? (uint32_t)rb_input[26] : 0xf1b5dd33);
    rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (216 < rb_input_size ? (uint32_t)rb_input[216] : 0xa98c8c88);
    if ((rb_state19.var_2) == (0xe4fda772)) {
        rb_state19.var_3 = rb_state19.var_0;
    }
    if ((rb_state19.var_2) == (0xe4fda772)) {
        rb_state19.var_4 = 0xbdec2d5b;
        rb_state19.var_5 = (rb_state19.var_5) - (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_5) : (0xb43bb704));
        rb_state19.var_6 = (rb_state19.var_6) + (((((0xee4549a6) - (rb_state19.var_6)) ^ (rb_state19.var_3)) - (rb_state19.var_4)) + (rb_state19.var_4));
        rb_state19.var_7 = (rb_state19.var_7) ^ (((0xa4de9113) - (rb_state19.var_7)) ^ (rb_state19.var_5));
        rb_state19.var_8 = (rb_state19.var_8) + (((0x815ba4c9) + (rb_state19.var_6)) - (rb_state19.var_8));
        rb_state19.var_9 = (rb_state19.var_9) - (((((0xa41ad3d5) ^ (rb_state19.var_7)) - (0x3837ca4c)) - (rb_state19.var_8)) + (0xb3e30ba));
        rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_9);
    }
    if ((rb_state19.var_2) == (0xe4fda772)) {
        if (!((rb_state19.var_0) == (rb_state19.var_3))) {
            racebench_trigger(19);
        }
    }
    #endif
    OutputX = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_11 = (rb_state9.var_11) ^ (0x6f15b68c);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x92010c2e)) {
        pthread_mutex_lock(&(rb_state14.lock_23));
        rb_state14.var_5 = 0xc2ee6155;
        rb_state14.var_6 = (rb_state14.var_6) + (rb_state14.var_4);
        rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_5);
        rb_state14.var_8 = (rb_state14.var_8) - (((0xa6715964) + (rb_state14.var_3)) + (0x32a4371));
        rb_state14.var_9 = (rb_state14.var_9) + (((0x31d09f74) + (0x39773007)) ^ (rb_state14.var_6));
        rb_state14.var_10 = (rb_state14.var_10) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0xe1b353c8));
        rb_state14.var_11 = (rb_state14.var_11) - (((0x72196edd) - (0xf2036892)) ^ (rb_state14.var_9));
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_11) : (0xfee503fb));
        rb_state14.var_4 = (rb_state14.var_4) - (rb_state14.var_12);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_23));
    }
    if ((rb_state14.var_2) == (0x92010c2e)) {
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x6d6a6467)) {
        rb_state18.var_14 = 0xdf37257a;
        rb_state18.var_15 = 0x6657389b;
        rb_state18.var_16 = 0x3076b252;
        rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_8) : (0xdb1ae7bd));
        rb_state18.var_18 = (rb_state18.var_18) - (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_14) : (0xb6127e0d));
        rb_state18.var_19 = (rb_state18.var_19) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_15) : (0x5b5d67a8));
        rb_state18.var_20 = (rb_state18.var_20) + (((((0xca024dd0) + (rb_state18.var_12)) - (0x9f71601f)) ^ (rb_state18.var_17)) ^ (rb_state18.var_16));
        rb_state18.var_21 = (rb_state18.var_21) ^ (((0xf20ca1d0) ^ (0x34dc6573)) + (rb_state18.var_18));
        rb_state18.var_22 = (rb_state18.var_22) + (rb_state18.var_19);
        rb_state18.var_23 = (rb_state18.var_23) + (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_21) : (0xa6238a1e));
        rb_state18.var_24 = (rb_state18.var_24) ^ (rb_state18.var_22);
        rb_state18.var_25 = (rb_state18.var_25) ^ (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_23) : (0xcfbf1655));
        rb_state18.var_26 = (rb_state18.var_26) ^ (((0x8c2cfd01) - (0x1e8b5886)) ^ (rb_state18.var_24));
        rb_state18.var_27 = (rb_state18.var_27) - (((((0x7f91c677) - (0x40a83e9d)) - (rb_state18.var_26)) ^ (rb_state18.var_14)) + (rb_state18.var_25));
        rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_27);
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

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xe507c92a)) {
        rb_state6.var_3 = 0x16320b7;
        rb_state6.var_4 = 0x7ca3470c;
        rb_state6.var_5 = (rb_state6.var_5) + (((((0xc04d368d) ^ (rb_state6.var_4)) ^ (0xb5651f51)) - (rb_state6.var_3)) + (rb_state6.var_2));
        rb_state6.var_6 = (rb_state6.var_6) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_5) : (0x3834924d));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_6);
    }
    #endif
    xStockPrice = sptprice;
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x53fd5cbb));
    #endif
    xStrikePrice = strike;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (0x258fd027);
    #endif
    xRiskFreeRate = rate;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_2) : (0x64d99ba1));
    #endif
    xVolatility = volatility;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_2) : (0xcf4e2ac8));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - ((0x6f732943) + (rb_state17.var_0));
    #endif
    xTime = time;
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (0x2d0796bc);
    #endif
    xSqrtTime = sqrt(xTime);

    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x483064b3)) {
        rb_state10.var_10 = 0x7e9ad9da;
        rb_state10.var_11 = 0xe8ec2200;
        rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0xe49d837c));
        rb_state10.var_13 = (rb_state10.var_13) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_8) : (0x2c0c7bea));
        rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x7a3cef6b));
        rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0xfdc88bbd) - (0xf30ce52b));
    #endif
    logValues = log(sptprice / strike);

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_2) : (0x101fd90d));
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0xe9491f56)) {
        rb_state12.var_29 = 0x3d74c30b;
        rb_state12.var_30 = (rb_state12.var_30) ^ (((rb_state12.var_19) == (0x0)) ? (rb_state12.var_18) : (0x8d7113da));
        rb_state12.var_31 = (rb_state12.var_31) - ((0x6550d626) - (rb_state12.var_20));
        rb_state12.var_32 = (rb_state12.var_32) + (((((0x36ebc332) - (rb_state12.var_21)) + (rb_state12.var_17)) ^ (rb_state12.var_29)) + (0xcf3436d2));
        rb_state12.var_33 = (rb_state12.var_33) - (((rb_state12.var_30) == (0x0)) ? (rb_state12.var_31) : (0xe80bc153));
        rb_state12.var_34 = (rb_state12.var_34) - (((0xa577a661) + (0xf1ed4e36)) - (rb_state12.var_32));
        rb_state12.var_35 = (rb_state12.var_35) + (((rb_state12.var_33) == (0x0)) ? (rb_state12.var_34) : (0x972297d8));
        rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_35);
    }
    #endif
    xLogTerm = logValues;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_12) : (0x22055619));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (0xc0a489a6);
    #endif
    xPowerTerm = xVolatility * xVolatility;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_1);
    if ((rb_state3.var_0) == (0x70535d24)) {
        pthread_mutex_lock(&(rb_state3.lock_24));
        rb_state3.var_8 = (rb_state3.var_8) ^ (rb_state3.var_5);
        rb_state3.var_9 = (rb_state3.var_9) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_6) : (0xc1e34dbe));
        rb_state3.var_10 = (rb_state3.var_10) + (rb_state3.var_8);
        rb_state3.var_11 = (rb_state3.var_11) - (142 < rb_input_size ? (uint32_t)rb_input[142] : 0x45ba07ba);
        rb_state3.var_12 = (rb_state3.var_12) ^ (((((0x6200e00c) - (0x3b64b5b5)) ^ (0xbf050bbc)) + (rb_state3.var_9)) + (rb_state3.var_10));
        rb_state3.var_13 = (rb_state3.var_13) - (((0x68f9b51c) - (rb_state3.var_11)) + (0xa83aceef));
        rb_state3.var_14 = (rb_state3.var_14) + (rb_state3.var_12);
        rb_state3.var_15 = (rb_state3.var_15) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_13) : (0x4fac2c33));
        rb_state3.var_16 = (rb_state3.var_16) - (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0x1641f85b));
        rb_state3.var_7 = (rb_state3.var_7) + (rb_state3.var_16);
        rb_state3.var_3 = rb_state3.var_7;
        pthread_mutex_unlock(&(rb_state3.lock_24));
    }
    if ((rb_state3.var_0) == (0x70535d24)) {
        pthread_mutex_lock(&(rb_state3.lock_24));
        rb_state3.var_18 = 0xc249f06f;
        rb_state3.var_19 = 0x9a0012fc;
        rb_state3.var_20 = 0x17ccd739;
        rb_state3.var_21 = (rb_state3.var_21) - (((((0x70c2e647) + (rb_state3.var_10)) + (rb_state3.var_9)) - (rb_state3.var_20)) - (rb_state3.var_18));
        rb_state3.var_22 = (rb_state3.var_22) + (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_19) : (0xb68e3f86));
        rb_state3.var_23 = (rb_state3.var_23) + (((rb_state3.var_21) == (0x0)) ? (rb_state3.var_22) : (0xda55a7e));
        rb_state3.var_17 = (rb_state3.var_17) + (rb_state3.var_23);
        rb_state3.var_3 = rb_state3.var_17;
        pthread_mutex_unlock(&(rb_state3.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x3f32c325)) {
        if ((rb_state4.var_2) != (0x0)) {
            rb_state4.var_35 = !((rb_state4.var_2) == (rb_state4.var_21));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (30 < rb_input_size ? (uint32_t)rb_input[30] : 0x78e5a318);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ ((0x3936db55) ^ (0xbfb435c9));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - ((0x5cefd468) - (rb_state11.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x3a5cb02e));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x92010c2e)) {
        rb_state14.var_13 = 0x3cac0c43;
        rb_state14.var_14 = (rb_state14.var_14) + (((((0x65a1dafc) + (0xa2461f64)) + (rb_state14.var_7)) + (rb_state14.var_6)) - (rb_state14.var_13));
        rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_5) : (0x6e1c7f5c));
        rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_9) : (0x5ffa452b));
        rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_14) : (0xd943b4e8));
        rb_state14.var_18 = (rb_state14.var_18) + (rb_state14.var_15);
        rb_state14.var_19 = (rb_state14.var_19) - (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_16) : (0x3c05c2));
        rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_18) : (0x6a76f340));
        rb_state14.var_21 = (rb_state14.var_21) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_19) : (0x9ac88e79));
        rb_state14.var_22 = (rb_state14.var_22) + (((((0x995eb1c) + (0xbc134f7d)) ^ (0x111a826f)) ^ (rb_state14.var_21)) - (rb_state14.var_20));
        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_22);
    }
    #endif
    xPowerTerm = xPowerTerm * 0.5;

    xD1 = xRiskFreeRate + xPowerTerm;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x2b0a67d1)) {
        if ((rb_state5.var_5) != (0x0)) {
            rb_state5.var_14 = !((rb_state5.var_3) == (rb_state5.var_4));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (0x29a6f0a3);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0x6bbe6429);
    #endif
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x40dcca4e)) {
        rb_state1.var_20 = (rb_state1.var_20) + ((0xbcdd986d) + (rb_state1.var_16));
        rb_state1.var_21 = (rb_state1.var_21) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_15) : (0xb638e110));
        rb_state1.var_22 = (rb_state1.var_22) - (247 < rb_input_size ? (uint32_t)rb_input[247] : 0x5437b52e);
        rb_state1.var_23 = (rb_state1.var_23) + (((((0x8cf1321d) + (rb_state1.var_18)) ^ (rb_state1.var_13)) + (rb_state1.var_19)) ^ (rb_state1.var_14));
        rb_state1.var_24 = (rb_state1.var_24) ^ (((0x2458a930) + (rb_state1.var_20)) ^ (rb_state1.var_20));
        rb_state1.var_25 = (rb_state1.var_25) ^ (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_21) : (0xd368343e));
        rb_state1.var_26 = (rb_state1.var_26) - (((rb_state1.var_22) == (0x0)) ? (rb_state1.var_23) : (0xa90d8f4f));
        rb_state1.var_27 = (rb_state1.var_27) + (rb_state1.var_24);
        rb_state1.var_28 = (rb_state1.var_28) - (((rb_state1.var_25) == (0x0)) ? (rb_state1.var_26) : (0x271cf202));
        rb_state1.var_29 = (rb_state1.var_29) ^ (((0xb9b2d77) - (rb_state1.var_27)) + (0x384df749));
        rb_state1.var_30 = (rb_state1.var_30) + (((((0x914629bd) ^ (rb_state1.var_28)) - (rb_state1.var_22)) + (0x786e35df)) + (rb_state1.var_29));
        rb_state1.var_19 = (rb_state1.var_19) - (rb_state1.var_30);
        rb_state1.var_3 = rb_state1.var_19;
    }
    if ((rb_state1.var_0) == (0x40dcca4e)) {
        pthread_mutex_lock(&(rb_state1.lock_43));
        rb_state1.var_32 = 0xeb3ca59d;
        rb_state1.var_33 = (rb_state1.var_33) - (rb_state1.var_25);
        rb_state1.var_34 = (rb_state1.var_34) - (rb_state1.var_32);
        rb_state1.var_35 = (rb_state1.var_35) ^ (((((0x15135e9f) - (0xe19b9bd6)) - (rb_state1.var_24)) - (rb_state1.var_23)) + (rb_state1.var_26));
        rb_state1.var_36 = (rb_state1.var_36) ^ (((rb_state1.var_33) == (0x0)) ? (rb_state1.var_34) : (0x29470f7d));
        rb_state1.var_37 = (rb_state1.var_37) - (rb_state1.var_35);
        rb_state1.var_38 = (rb_state1.var_38) - (rb_state1.var_36);
        rb_state1.var_39 = (rb_state1.var_39) + (((rb_state1.var_27) == (0x0)) ? (rb_state1.var_37) : (0x7bfca92));
        rb_state1.var_40 = (rb_state1.var_40) ^ (rb_state1.var_38);
        rb_state1.var_41 = (rb_state1.var_41) - (((0x34a3bf67) - (rb_state1.var_39)) - (rb_state1.var_28));
        rb_state1.var_42 = (rb_state1.var_42) + (((((0xcf7b93c7) - (rb_state1.var_41)) - (rb_state1.var_29)) ^ (rb_state1.var_30)) + (rb_state1.var_40));
        rb_state1.var_31 = (rb_state1.var_31) + (rb_state1.var_42);
        rb_state1.var_3 = rb_state1.var_31;
        pthread_mutex_unlock(&(rb_state1.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0x5cad62c0));
    #endif
    xDen = xVolatility * xSqrtTime;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (50 < rb_input_size ? (uint32_t)rb_input[50] : 0x75df02cc);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_1) : (0xc5f2b08f));
    #endif
    xD1 = xD1 / xDen;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - (76 < rb_input_size ? (uint32_t)rb_input[76] : 0xc32c3d09);
    #endif
    xD2 = xD1 - xDen;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x16490a93) - (rb_state5.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x997daefe));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x91275577)) {
        pthread_mutex_lock(&(rb_state15.lock_10));
        rb_state15.var_3 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_10));
    }
    if ((rb_state15.var_1) == (0x91275577)) {
        if (!((rb_state15.var_2) == (rb_state15.var_3))) {
            racebench_trigger(15);
        }
    }
    #endif
    d1 = xD1;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (53 < rb_input_size ? (uint32_t)rb_input[53] : 0x56cf8251);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (0x434dddd0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (35 < rb_input_size ? (uint32_t)rb_input[35] : 0xa87b66b6);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (91 < rb_input_size ? (uint32_t)rb_input[91] : 0x43a80f69);
    #endif
    d2 = xD2;

    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x2b0a67d1)) {
        rb_state5.var_5 = 0x1;
    }
    if ((rb_state5.var_0) == (0x2b0a67d1)) {
        pthread_mutex_lock(&(rb_state5.lock_13));
        rb_state5.var_6 = 0xeda5ce78;
        rb_state5.var_7 = 0xfa32d7fe;
        rb_state5.var_8 = (rb_state5.var_8) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_4) : (0xb27a1802));
        rb_state5.var_9 = (rb_state5.var_9) - (((((0x6e994dc7) + (0xc488e0ae)) ^ (rb_state5.var_7)) + (0xe3343a03)) - (rb_state5.var_3));
        rb_state5.var_10 = (rb_state5.var_10) + (rb_state5.var_6);
        rb_state5.var_11 = (rb_state5.var_11) ^ (((((0x5be5ff52) - (rb_state5.var_6)) ^ (rb_state5.var_8)) ^ (rb_state5.var_9)) - (0x953e664a));
        rb_state5.var_12 = (rb_state5.var_12) + (((((0xf6ab59e2) - (rb_state5.var_10)) ^ (rb_state5.var_7)) ^ (rb_state5.var_11)) - (0x6d30191e));
        rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_12);
        rb_state5.var_3 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0xa3102b98)) ? (rb_state11.var_1) : (0xec10f33a));
    #endif
    NofXd1 = CNDF(d1);
    NofXd2 = CNDF(d2);

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (0x1d3b8cd2);
    #endif
    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
        #endif
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_1);
        #endif
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + ((0xd81d18d9) - (0x6076b246));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_2) == (0x25c7947d)) ? (rb_state7.var_1) : (0xc4b3f34c));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (0xdbc99801);
    rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_1) == (0xbe12630e)) ? (rb_state14.var_0) : (0xe322f572));
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

            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) + (0xc97dbdf);
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0xe9491f56)) {
                pthread_mutex_lock(&(rb_state12.lock_36));
                rb_state12.var_5 = 0x7a0cc3e;
                rb_state12.var_6 = (rb_state12.var_6) + (((0xa170f7a5) - (rb_state12.var_4)) ^ (0x1dd4c75f));
                rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_5) : (0x96c1b947));
                rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_3) : (0x7d957390));
                rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_5) : (0x9c41b4a6));
                rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_6) : (0x77208f50));
                rb_state12.var_11 = (rb_state12.var_11) ^ (((0x7b260c4) + (rb_state12.var_7)) - (rb_state12.var_10));
                rb_state12.var_12 = (rb_state12.var_12) - (rb_state12.var_8);
                rb_state12.var_13 = (rb_state12.var_13) - (((0x6993779f) - (rb_state12.var_9)) + (rb_state12.var_11));
                rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_10) : (0x5d94876f));
                rb_state12.var_15 = (rb_state12.var_15) - (rb_state12.var_11);
                rb_state12.var_16 = (rb_state12.var_16) + (rb_state12.var_12);
                rb_state12.var_17 = (rb_state12.var_17) ^ (rb_state12.var_13);
                rb_state12.var_18 = (rb_state12.var_18) + (rb_state12.var_14);
                rb_state12.var_19 = (rb_state12.var_19) - (((((0x6d0abc80) - (rb_state12.var_16)) - (rb_state12.var_14)) ^ (rb_state12.var_13)) - (rb_state12.var_15));
                rb_state12.var_20 = (rb_state12.var_20) + (rb_state12.var_17);
                rb_state12.var_21 = (rb_state12.var_21) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_18) : (0xbfed999d));
                rb_state12.var_22 = (rb_state12.var_22) - (rb_state12.var_19);
                rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_20) : (0xafd42e12));
                rb_state12.var_24 = (rb_state12.var_24) - (rb_state12.var_21);
                rb_state12.var_25 = (rb_state12.var_25) ^ (((((0x386f2283) + (rb_state12.var_23)) - (0xdb7441ad)) + (0xcefe9f6b)) + (rb_state12.var_22));
                rb_state12.var_26 = (rb_state12.var_26) + (rb_state12.var_24);
                rb_state12.var_27 = (rb_state12.var_27) ^ (((0x3eedc793) ^ (0xafbb0b22)) - (rb_state12.var_25));
                rb_state12.var_28 = (rb_state12.var_28) - (((rb_state12.var_26) == (0x0)) ? (rb_state12.var_27) : (0xe9d371f));
                rb_state12.var_4 = (rb_state12.var_4) ^ (rb_state12.var_28);
                rb_state12.var_3 = rb_state12.var_4;
                pthread_mutex_unlock(&(rb_state12.lock_36));
            }
            if ((rb_state12.var_1) == (0xe9491f56)) {
                if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                    racebench_trigger(12);
                }
            }
            #endif
            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
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