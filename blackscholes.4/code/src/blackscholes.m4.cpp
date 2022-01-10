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
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ ((0x1b7df99a) - (0x148cb3d6));
        #endif
        sign = 1;
    } else {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_1) : (0x849e0ae5));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_1) : (0x1d0d59c9));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_3);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_11 = (rb_state13.var_11) ^ (rb_state13.var_12);
        rb_state13.var_13 = (rb_state13.var_13) ^ (0xfd79b9de);
        if ((rb_state13.var_14) == (0x59981ae7)) {
            pthread_mutex_lock(&(rb_state13.lock_45));
            rb_state13.var_27 = 0x3dc5e58f;
            rb_state13.var_28 = (rb_state13.var_28) ^ (((((0x31717fd4) - (rb_state13.var_17)) + (0x39de03da)) ^ (rb_state13.var_15)) ^ (rb_state13.var_27));
            rb_state13.var_29 = (rb_state13.var_29) ^ (((0xdb33ce5) ^ (rb_state13.var_18)) - (rb_state13.var_16));
            rb_state13.var_30 = (rb_state13.var_30) ^ (rb_state13.var_19);
            rb_state13.var_31 = (rb_state13.var_31) - (rb_state13.var_28);
            rb_state13.var_32 = (rb_state13.var_32) + (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_29) : (0x4cc311c1));
            rb_state13.var_33 = (rb_state13.var_33) + (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0xd27548eb));
            rb_state13.var_34 = (rb_state13.var_34) ^ (((rb_state13.var_32) == (0x0)) ? (rb_state13.var_33) : (0xd5f182f1));
            rb_state13.var_26 = (rb_state13.var_26) + (rb_state13.var_34);
            rb_state13.var_25 = rb_state13.var_26;
            pthread_mutex_unlock(&(rb_state13.lock_45));
        }
        if ((rb_state13.var_14) == (0x59981ae7)) {
            if (!((rb_state13.var_25) == (rb_state13.var_26))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + ((0x40cf619a) ^ (0xea46668a));
        #endif
        sign = 0;
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (221 < rb_input_size ? (uint32_t)rb_input[221] : 0x2e7ea922);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_10) : (0xa45ea5ff));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0x3f3417e6)) {
        pthread_mutex_lock(&(rb_state3.lock_15));
        rb_state3.var_5 = 0x302afceb;
        rb_state3.var_6 = 0xca4c862c;
        rb_state3.var_7 = 0xc20e9e28;
        rb_state3.var_8 = (rb_state3.var_8) + (0x4f1f6c8c);
        rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_5) : (0x68d6fb97));
        rb_state3.var_10 = (rb_state3.var_10) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xb8f2d5e5));
        rb_state3.var_11 = (rb_state3.var_11) - (((((0xae997432) ^ (rb_state3.var_5)) - (rb_state3.var_9)) ^ (0x73d7f991)) - (rb_state3.var_8));
        rb_state3.var_12 = (rb_state3.var_12) ^ (rb_state3.var_10);
        rb_state3.var_13 = (rb_state3.var_13) + (((0x32176f2e) + (rb_state3.var_11)) + (0x813912a8));
        rb_state3.var_14 = (rb_state3.var_14) + (((((0x4c5f1d83) + (rb_state3.var_12)) - (rb_state3.var_13)) + (0x34de137b)) ^ (rb_state3.var_6));
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_14);
        pthread_mutex_unlock(&(rb_state3.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x3be70923));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_2) : (0xabb464ab));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x8b5e1003)) {
        rb_state16.var_19 = 0x744f5b8c;
        rb_state16.var_20 = (rb_state16.var_20) ^ (rb_state16.var_12);
        rb_state16.var_21 = (rb_state16.var_21) - (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_13) : (0xc1b56aba));
        rb_state16.var_22 = (rb_state16.var_22) + (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_21) : (0xc1354aeb));
        rb_state16.var_18 = (rb_state16.var_18) ^ (rb_state16.var_22);
        rb_state16.var_3 = rb_state16.var_18;
    }
    if ((rb_state16.var_1) == (0x8b5e1003)) {
        pthread_mutex_lock(&(rb_state16.lock_36));
        rb_state16.var_24 = 0xe8309914;
        rb_state16.var_25 = 0x4961b869;
        rb_state16.var_26 = 0xfc1b0a10;
        rb_state16.var_27 = (rb_state16.var_27) + (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_14) : (0x53932f80));
        rb_state16.var_28 = (rb_state16.var_28) - (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_25) : (0x68e0648e));
        rb_state16.var_29 = (rb_state16.var_29) - (rb_state16.var_24);
        rb_state16.var_30 = (rb_state16.var_30) - (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_26) : (0x9b468608));
        rb_state16.var_31 = (rb_state16.var_31) ^ (rb_state16.var_27);
        rb_state16.var_32 = (rb_state16.var_32) - (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_29) : (0x9147d853));
        rb_state16.var_33 = (rb_state16.var_33) ^ (((rb_state16.var_30) == (0x0)) ? (rb_state16.var_31) : (0xb49eac32));
        rb_state16.var_34 = (rb_state16.var_34) + (((0xa778de80) + (rb_state16.var_18)) ^ (rb_state16.var_32));
        rb_state16.var_35 = (rb_state16.var_35) - (((rb_state16.var_33) == (0x0)) ? (rb_state16.var_34) : (0x4145121f));
        rb_state16.var_23 = (rb_state16.var_23) - (rb_state16.var_35);
        rb_state16.var_3 = rb_state16.var_23;
        pthread_mutex_unlock(&(rb_state16.lock_36));
    }
    #endif
    xInput = InputX;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (137 < rb_input_size ? (uint32_t)rb_input[137] : 0x136a22f1);
    rb_state2.var_0 = (rb_state2.var_0) ^ (116 < rb_input_size ? (uint32_t)rb_input[116] : 0x40c97771);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (109 < rb_input_size ? (uint32_t)rb_input[109] : 0x534d0447);
    #endif
    expValues = exp(-0.5f * InputX * InputX);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - ((0xa0677d53) ^ (0x22e07cc5));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (174 < rb_input_size ? (uint32_t)rb_input[174] : 0xbfaf1e66);
    rb_state3.var_2 = (rb_state3.var_2) - (87 < rb_input_size ? (uint32_t)rb_input[87] : 0x2f303565);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ ((0xd0029d6f) ^ (rb_state12.var_0));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (0xbbecb99e);
    rb_state17.var_2 = (rb_state17.var_2) - (243 < rb_input_size ? (uint32_t)rb_input[243] : 0xe4fcb044);
    #endif
    xNPrimeofX = expValues;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) - ((0x7ee98310) - (rb_state3.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - ((0xd6358020) - (0x96b8fe6a));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x1ddb3619));
    rb_state15.var_1 = (rb_state15.var_1) + (148 < rb_input_size ? (uint32_t)rb_input[148] : 0x19301df3);
    rb_state15.var_2 = (rb_state15.var_2) - (230 < rb_input_size ? (uint32_t)rb_input[230] : 0x33510861);
    #endif
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x30cd63f4)) {
        pthread_mutex_lock(&(rb_state12.lock_17));
        rb_state12.var_3 = rb_state12.var_0;
        pthread_mutex_unlock(&(rb_state12.lock_17));
    }
    if ((rb_state12.var_2) == (0x30cd63f4)) {
        if (!((rb_state12.var_0) == (rb_state12.var_3))) {
            racebench_trigger(12);
        }
    }
    #endif
    xK2 = 0.2316419 * xInput;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x4d322ad2)) {
        pthread_mutex_lock(&(rb_state8.lock_16));
        rb_state8.var_4 = 0x36c55487;
        rb_state8.var_5 = (rb_state8.var_5) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0xbb7118ea));
        rb_state8.var_6 = (rb_state8.var_6) + (146 < rb_input_size ? (uint32_t)rb_input[146] : 0x43bbe43d);
        rb_state8.var_7 = (rb_state8.var_7) - (rb_state8.var_6);
        rb_state8.var_8 = (rb_state8.var_8) - (((0x2ecbb1bb) ^ (rb_state8.var_3)) - (rb_state8.var_7));
        rb_state8.var_9 = (rb_state8.var_9) - (((0x33fd320b) ^ (rb_state8.var_8)) ^ (rb_state8.var_5));
        rb_state8.var_10 = (rb_state8.var_10) + (rb_state8.var_6);
        rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0xf4e3a832));
        rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_9) : (0x17d0ca9f));
        rb_state8.var_13 = (rb_state8.var_13) - (rb_state8.var_10);
        rb_state8.var_14 = (rb_state8.var_14) ^ (((((0x20722af7) + (rb_state8.var_12)) ^ (rb_state8.var_11)) + (rb_state8.var_10)) - (0x91f6840f));
        rb_state8.var_15 = (rb_state8.var_15) + (((((0x3bd4dc1d) ^ (0xd7cd457d)) + (rb_state8.var_14)) + (rb_state8.var_13)) - (rb_state8.var_11));
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_15);
        pthread_mutex_unlock(&(rb_state8.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x2b3a395c) + (rb_state12.var_0));
    rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_1) == (0xffffffe0)) ? (rb_state12.var_0) : (0x473daf18));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (202 < rb_input_size ? (uint32_t)rb_input[202] : 0xb0273f75);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - (((rb_state19.var_3) == (0xb398745a)) ? (rb_state19.var_2) : (0x17931f20));
    #endif
    xK2 = 1.0 + xK2;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (92 < rb_input_size ? (uint32_t)rb_input[92] : 0x53a7841d);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (0xa6a2653);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (0xfb6deb5f);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x6b43e2df)) {
        pthread_mutex_lock(&(rb_state15.lock_30));
        if ((rb_state15.var_4) != (0x0)) {
            if (!((rb_state15.var_4) == (rb_state15.var_22))) {
                racebench_trigger(15);
            }
        }
        pthread_mutex_unlock(&(rb_state15.lock_30));
    }
    #endif
    xK2 = 1.0 / xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + (0xb9c9a12);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_5) == (0xbde4c562)) {
        rb_state2.var_13 = rb_state2.var_8;
    }
    if ((rb_state2.var_5) == (0xbde4c562)) {
        if (!((rb_state2.var_8) == (rb_state2.var_13))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xe8dee9f3)) {
        pthread_mutex_lock(&(rb_state6.lock_12));
        rb_state6.var_4 = rb_state6.var_3;
        pthread_mutex_unlock(&(rb_state6.lock_12));
    }
    if ((rb_state6.var_2) == (0xe8dee9f3)) {
        if (!((rb_state6.var_3) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) + (0x53b19943);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - ((0xd4f93599) - (0xf3229336));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_2);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) ^ ((0x5fdab95c) - (0x6549b6b2));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x6b43e2df)) {
        rb_state15.var_14 = 0xb99435a4;
        rb_state15.var_15 = 0xbbf13d6e;
        rb_state15.var_16 = 0x7668236c;
        rb_state15.var_17 = (rb_state15.var_17) + (rb_state15.var_11);
        rb_state15.var_18 = (rb_state15.var_18) - (((0x927c25a8) - (rb_state15.var_14)) + (0x5eb54cd));
        rb_state15.var_19 = (rb_state15.var_19) + (((((0x35944be4) + (rb_state15.var_12)) - (0x2ef6140d)) ^ (rb_state15.var_16)) ^ (rb_state15.var_15));
        rb_state15.var_20 = (rb_state15.var_20) + (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_18) : (0xc10b1df6));
        rb_state15.var_21 = (rb_state15.var_21) + (((((0x26b9b07d) ^ (rb_state15.var_19)) - (rb_state15.var_13)) - (rb_state15.var_20)) ^ (0x37c97a42));
        rb_state15.var_13 = (rb_state15.var_13) ^ (rb_state15.var_21);
        rb_state15.var_4 = rb_state15.var_13;
    }
    if ((rb_state15.var_0) == (0x6b43e2df)) {
        pthread_mutex_lock(&(rb_state15.lock_30));
        rb_state15.var_23 = (rb_state15.var_23) - (((((0xaebd332c) + (rb_state15.var_17)) - (rb_state15.var_16)) ^ (rb_state15.var_14)) ^ (0xd4595149));
        rb_state15.var_24 = (rb_state15.var_24) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_18) : (0x9afda54a));
        rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_15);
        rb_state15.var_26 = (rb_state15.var_26) - (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_23) : (0x1a44e829));
        rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_25) : (0x7c0eea94));
        rb_state15.var_28 = (rb_state15.var_28) + (rb_state15.var_26);
        rb_state15.var_29 = (rb_state15.var_29) ^ (((((0xca6f5079) - (rb_state15.var_27)) + (0xecec8a1e)) ^ (0xb5dca3ba)) ^ (rb_state15.var_28));
        rb_state15.var_22 = (rb_state15.var_22) + (rb_state15.var_29);
        rb_state15.var_4 = rb_state15.var_22;
        pthread_mutex_unlock(&(rb_state15.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_2) == (0x59d1d06a)) {
        rb_state18.var_5 = 0x798d122c;
        rb_state18.var_6 = 0xe785ecbc;
        rb_state18.var_7 = (rb_state18.var_7) - (0x27bf1315);
        rb_state18.var_8 = (rb_state18.var_8) - (((0x1beefa) ^ (0x3f579d2b)) + (rb_state18.var_6));
        rb_state18.var_9 = (rb_state18.var_9) ^ (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_5) : (0x4eb9254f));
        rb_state18.var_10 = (rb_state18.var_10) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0xeed52326));
        rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_7) : (0x7c1946e1));
        rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_8);
        rb_state18.var_13 = (rb_state18.var_13) - (((0x2847132d) + (rb_state18.var_8)) + (rb_state18.var_9));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((((0xbd861935) + (rb_state18.var_9)) + (rb_state18.var_11)) + (0x68804433)) ^ (rb_state18.var_10));
        rb_state18.var_15 = (rb_state18.var_15) - (rb_state18.var_12);
        rb_state18.var_16 = (rb_state18.var_16) ^ (((((0xdc719809) + (0x68b37fe8)) - (rb_state18.var_10)) ^ (rb_state18.var_13)) - (rb_state18.var_14));
        rb_state18.var_17 = (rb_state18.var_17) ^ (((((0x358ebc3b) + (0xb60565a7)) - (rb_state18.var_16)) ^ (rb_state18.var_15)) + (0x97d5a9a7));
        rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_17);
    }
    #endif
    xK2_2 = xK2 * xK2;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0xc737b92e);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (63 < rb_input_size ? (uint32_t)rb_input[63] : 0xfe8d0c7a);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (162 < rb_input_size ? (uint32_t)rb_input[162] : 0x17921975);
    #endif
    xK2_3 = xK2_2 * xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_3);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0x40)) ? (rb_state1.var_2) : (0x78e9e99a));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_12 = (rb_state8.var_12) + (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_13) : (0xbbee1279));
    if ((rb_state8.var_4) == (0x36c55487)) {
        pthread_mutex_lock(&(rb_state8.lock_42));
        rb_state8.var_20 = 0xdf4e1c8a;
        rb_state8.var_21 = (rb_state8.var_21) - (rb_state8.var_20);
        rb_state8.var_22 = (rb_state8.var_22) - (((((0x935e46cd) - (rb_state8.var_19)) + (rb_state8.var_21)) ^ (0xd7b055ad)) + (rb_state8.var_20));
        rb_state8.var_23 = (rb_state8.var_23) + (0xd822e5c9);
        rb_state8.var_24 = (rb_state8.var_24) - (((0x7d6daa5d) ^ (rb_state8.var_18)) + (rb_state8.var_22));
        rb_state8.var_25 = (rb_state8.var_25) ^ (((rb_state8.var_23) == (0x0)) ? (rb_state8.var_21) : (0xfb49151));
        rb_state8.var_26 = (rb_state8.var_26) - (rb_state8.var_22);
        rb_state8.var_27 = (rb_state8.var_27) ^ (((0x6f11ab9a) - (rb_state8.var_23)) - (rb_state8.var_24));
        rb_state8.var_28 = (rb_state8.var_28) ^ (rb_state8.var_24);
        rb_state8.var_29 = (rb_state8.var_29) ^ (((((0xda6ba540) + (rb_state8.var_25)) ^ (0x21183a5c)) ^ (0xdc704b58)) ^ (rb_state8.var_26));
        rb_state8.var_30 = (rb_state8.var_30) ^ (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_28) : (0xfedd0e58));
        rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_29) == (0x0)) ? (rb_state8.var_30) : (0x9c3772e1));
        rb_state8.var_19 = (rb_state8.var_19) ^ (rb_state8.var_31);
        rb_state8.var_18 = rb_state8.var_19;
        pthread_mutex_unlock(&(rb_state8.lock_42));
    }
    if ((rb_state8.var_4) == (0x36c55487)) {
        if (!((rb_state8.var_18) == (rb_state8.var_19))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (192 < rb_input_size ? (uint32_t)rb_input[192] : 0x1820607f);
    #endif
    xK2_4 = xK2_3 * xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) + ((0x9f690088) ^ (rb_state3.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (177 < rb_input_size ? (uint32_t)rb_input[177] : 0x44b52bd5);
    rb_state6.var_3 = (rb_state6.var_3) - (0x66db9391);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - ((0xc41751f6) - (rb_state13.var_3));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (((rb_state14.var_1) == (0xd3790032)) ? (rb_state14.var_0) : (0x69b9f0e0));
    if ((rb_state14.var_2) == (0x3b6982e7)) {
        rb_state14.var_14 = (rb_state14.var_14) ^ (rb_state14.var_10);
        rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_12) : (0xc45c3524));
        rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_11) : (0x6b736e47));
        rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_15) : (0xbcb4a38e));
        rb_state14.var_18 = (rb_state14.var_18) ^ (((0xf9b0c1a3) + (rb_state14.var_16)) ^ (0x96c2ee45));
        rb_state14.var_19 = (rb_state14.var_19) - (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_18) : (0xb8fab8c3));
        rb_state14.var_13 = (rb_state14.var_13) - (rb_state14.var_19);
        rb_state14.var_4 = rb_state14.var_13;
    }
    if ((rb_state14.var_2) == (0x3b6982e7)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_21 = 0x3bc63453;
        rb_state14.var_22 = (rb_state14.var_22) + (71 < rb_input_size ? (uint32_t)rb_input[71] : 0xcf67b038);
        rb_state14.var_23 = (rb_state14.var_23) + (((((0x48e7de52) + (rb_state14.var_15)) + (rb_state14.var_16)) ^ (rb_state14.var_14)) ^ (rb_state14.var_17));
        rb_state14.var_24 = (rb_state14.var_24) ^ (((((0x8a64facf) - (rb_state14.var_18)) + (rb_state14.var_22)) - (rb_state14.var_19)) + (rb_state14.var_21));
        rb_state14.var_25 = (rb_state14.var_25) + (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_23) : (0xb10ff423));
        rb_state14.var_26 = (rb_state14.var_26) - (rb_state14.var_24);
        rb_state14.var_27 = (rb_state14.var_27) - (rb_state14.var_25);
        rb_state14.var_28 = (rb_state14.var_28) ^ (((((0x26646d8c) - (rb_state14.var_22)) ^ (rb_state14.var_21)) - (rb_state14.var_26)) + (rb_state14.var_27));
        rb_state14.var_20 = (rb_state14.var_20) ^ (rb_state14.var_28);
        rb_state14.var_4 = rb_state14.var_20;
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x8b5e1003)) {
        if ((rb_state16.var_3) != (0x0)) {
            if (!((rb_state16.var_3) == (rb_state16.var_23))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x5fa3e2e1) ^ (0x86592919));
    rb_state17.var_3 = (rb_state17.var_3) + (((rb_state17.var_1) == (0x4413464e)) ? (rb_state17.var_0) : (0x29d81425));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (166 < rb_input_size ? (uint32_t)rb_input[166] : 0xb606da44);
    #endif
    xK2_5 = xK2_4 * xK2;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - ((0x5a608ec2) - (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) - (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_3) : (0xa3ec9038));
    rb_state5.var_2 = (rb_state5.var_2) ^ (247 < rb_input_size ? (uint32_t)rb_input[247] : 0xee42e51);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_2) == (0x59d1d06a)) {
        rb_state18.var_4 = rb_state18.var_3;
    }
    if ((rb_state18.var_2) == (0x59d1d06a)) {
        if (!((rb_state18.var_3) == (rb_state18.var_4))) {
            racebench_trigger(18);
        }
    }
    #endif
    xLocal_1 = xK2 * 0.319381530;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0x38e5bfb1)) {
        rb_state2.var_12 = !((rb_state2.var_4) == (rb_state2.var_3));
    }
    rb_state2.var_8 = (rb_state2.var_8) + (79 < rb_input_size ? (uint32_t)rb_input[79] : 0x9aeecd45);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (131 < rb_input_size ? (uint32_t)rb_input[131] : 0x48f597f4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    #endif
    xLocal_2 = xK2_2 * (-0.356563782);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_6 = (rb_state2.var_6) ^ (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_3) : (0x62d4db08));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x43b4d7b6));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0x10f6a4a4)) {
        pthread_mutex_lock(&(rb_state7.lock_38));
        if ((rb_state7.var_4) != (0x0)) {
            if (!((rb_state7.var_4) == (rb_state7.var_24))) {
                racebench_trigger(7);
            }
        }
        pthread_mutex_unlock(&(rb_state7.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_1) == (0x660bd66)) ? (rb_state9.var_0) : (0xeda2c616));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_12 = (rb_state10.var_12) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_13) : (0x1bea5874));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (214 < rb_input_size ? (uint32_t)rb_input[214] : 0xdc3bdb82);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (0x88db5fb8);
    #endif
    xLocal_3 = xK2_3 * 1.781477937;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (11 < rb_input_size ? (uint32_t)rb_input[11] : 0xb5fee530);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x9ac2924)) {
        pthread_mutex_lock(&(rb_state13.lock_23));
        usleep(100);
        pthread_mutex_unlock(&(rb_state13.lock_23));
    }
    if ((rb_state13.var_1) == (0x9ac2924)) {
        rb_state13.var_24 = !((rb_state13.var_5) == (rb_state13.var_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_3) == (0xd9facbf8)) ? (rb_state17.var_2) : (0xf44fbcaf));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (0x7549453);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_0);
    if ((rb_state19.var_2) == (0x975d41bd)) {
        rb_state19.var_13 = 0x5c648183;
        rb_state19.var_14 = 0x30a489cc;
        rb_state19.var_15 = (rb_state19.var_15) - ((0x1d2ab2cd) - (rb_state19.var_9));
        rb_state19.var_16 = (rb_state19.var_16) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_8) : (0xe61070e7));
        rb_state19.var_17 = (rb_state19.var_17) ^ (((((0x610bdbb9) + (0xb16f0dcf)) ^ (0x8481007f)) ^ (rb_state19.var_15)) - (rb_state19.var_13));
        rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_16) : (0xc209d0e5));
        rb_state19.var_19 = (rb_state19.var_19) + (rb_state19.var_17);
        rb_state19.var_20 = (rb_state19.var_20) - (rb_state19.var_18);
        rb_state19.var_21 = (rb_state19.var_21) + (((0x66889162) + (rb_state19.var_19)) ^ (rb_state19.var_11));
        rb_state19.var_22 = (rb_state19.var_22) ^ (((((0xf4382fd5) ^ (rb_state19.var_12)) + (0x449c70e6)) ^ (rb_state19.var_20)) + (rb_state19.var_21));
        rb_state19.var_12 = (rb_state19.var_12) ^ (rb_state19.var_22);
        rb_state19.var_4 = rb_state19.var_12;
    }
    if ((rb_state19.var_2) == (0x975d41bd)) {
        pthread_mutex_lock(&(rb_state19.lock_36));
        rb_state19.var_24 = 0x6dec03dd;
        rb_state19.var_25 = 0x5f48a687;
        rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_24) : (0x1f31dcf1));
        rb_state19.var_27 = (rb_state19.var_27) - (rb_state19.var_25);
        rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_26) : (0x1a375b9b));
        rb_state19.var_29 = (rb_state19.var_29) ^ (((0x43f48091) ^ (rb_state19.var_27)) - (rb_state19.var_15));
        rb_state19.var_30 = (rb_state19.var_30) - (rb_state19.var_28);
        rb_state19.var_31 = (rb_state19.var_31) ^ (((0x602ecfc8) ^ (rb_state19.var_16)) ^ (rb_state19.var_29));
        rb_state19.var_32 = (rb_state19.var_32) + (rb_state19.var_30);
        rb_state19.var_33 = (rb_state19.var_33) + (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_31) : (0x41144280));
        rb_state19.var_34 = (rb_state19.var_34) + (((0xfdc96e7f) ^ (rb_state19.var_32)) - (rb_state19.var_18));
        rb_state19.var_35 = (rb_state19.var_35) ^ (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0x6d6ab16));
        rb_state19.var_23 = (rb_state19.var_23) - (rb_state19.var_35);
        rb_state19.var_4 = rb_state19.var_23;
        pthread_mutex_unlock(&(rb_state19.lock_36));
    }
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_5) == (0x8acc13b5)) {
        rb_state10.var_17 = 0x5b37efd8;
        rb_state10.var_18 = (rb_state10.var_18) ^ ((0x7a7aa36b) ^ (0x6e389350));
        rb_state10.var_19 = (rb_state10.var_19) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_17) : (0xd81baf3a));
        rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_18) : (0xb6cc3ec8));
        rb_state10.var_21 = (rb_state10.var_21) - (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_18) : (0xd5a7269d));
        rb_state10.var_22 = (rb_state10.var_22) ^ (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_19) : (0x510ef2b6));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_21) : (0xcef09f7));
        rb_state10.var_24 = (rb_state10.var_24) + (((((0x840709f3) - (rb_state10.var_22)) ^ (rb_state10.var_22)) + (0xb2dabf3e)) ^ (rb_state10.var_23));
        rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_2);
    rb_state16.var_0 = (rb_state16.var_0) + (142 < rb_input_size ? (uint32_t)rb_input[142] : 0xda96d14d);
    #endif
    xLocal_3 = xK2_4 * (-1.821255978);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - ((0x6fddf4af) + (0x1c9803e2));
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0x75861b3b)) {
        rb_state0.var_8 = 0x74ff2c8c;
        rb_state0.var_9 = (rb_state0.var_9) - (((((0x8d6b9602) + (0x46a6c20b)) + (rb_state0.var_8)) + (rb_state0.var_7)) + (rb_state0.var_5));
        rb_state0.var_10 = (rb_state0.var_10) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_6) : (0x421b63ff));
        rb_state0.var_11 = (rb_state0.var_11) - (rb_state0.var_9);
        rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0xac72f2d9));
        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_12);
        rb_state0.var_4 = rb_state0.var_7;
    }
    if ((rb_state0.var_2) == (0x75861b3b)) {
        pthread_mutex_lock(&(rb_state0.lock_17));
        rb_state0.var_14 = 0xc82f1beb;
        rb_state0.var_15 = (rb_state0.var_15) - (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_9) : (0x12fc512d));
        rb_state0.var_16 = (rb_state0.var_16) - (((((0xb8d5b398) ^ (rb_state0.var_10)) - (rb_state0.var_15)) - (rb_state0.var_11)) ^ (0xb64ccdb1));
        rb_state0.var_13 = (rb_state0.var_13) ^ (rb_state0.var_16);
        rb_state0.var_4 = rb_state0.var_13;
        pthread_mutex_unlock(&(rb_state0.lock_17));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xa027ce97)) {
        pthread_mutex_lock(&(rb_state1.lock_26));
        if ((rb_state1.var_3) != (0x0)) {
            if (!((rb_state1.var_3) == (rb_state1.var_16))) {
                racebench_trigger(1);
            }
        }
        pthread_mutex_unlock(&(rb_state1.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0x25247d65);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x3b6982e7)) {
        if ((rb_state14.var_4) != (0x0)) {
            if (!((rb_state14.var_4) == (rb_state14.var_20))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ ((0xefa87990) ^ (rb_state18.var_3));
    #endif
    xLocal_3 = xK2_5 * 1.330274429;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xe8dee9f3)) {
        rb_state6.var_5 = 0xf03fd15a;
        rb_state6.var_6 = 0x9c25bba;
        rb_state6.var_7 = 0xdc66c86c;
        rb_state6.var_8 = (rb_state6.var_8) ^ (((((0x6e3e60aa) + (0x4cdb21f9)) + (rb_state6.var_7)) - (0x368493e3)) ^ (rb_state6.var_6));
        rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_5) : (0xc78be553));
        rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_8) : (0x89254b9a));
        rb_state6.var_11 = (rb_state6.var_11) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0x8cb78415));
        rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (180 < rb_input_size ? (uint32_t)rb_input[180] : 0xe758cfdb);
    rb_state7.var_2 = (rb_state7.var_2) - (243 < rb_input_size ? (uint32_t)rb_input[243] : 0x54a354c1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + ((0x1fca0aa9) + (rb_state8.var_2));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0x524f6619)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_5);
        rb_state9.var_6 = (rb_state9.var_6) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0xeee6df8e));
        rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_6) : (0x9ba6d7c1));
        rb_state9.var_8 = (rb_state9.var_8) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_5) : (0x3d1d9c45));
        rb_state9.var_9 = (rb_state9.var_9) ^ (rb_state9.var_6);
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0xd1b2aa4));
        rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_10) : (0xc6b2ba5));
        rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_11);
        rb_state9.var_3 = rb_state9.var_4;
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    if ((rb_state9.var_1) == (0x524f6619)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        if (!((rb_state9.var_3) == (rb_state9.var_4))) {
            racebench_trigger(9);
        }
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_3) == (0x52148062)) {
        rb_state17.var_14 = 0x88fe2c3d;
        rb_state17.var_15 = 0x9c7a9373;
        rb_state17.var_16 = (rb_state17.var_16) ^ ((0xf23d41ba) ^ (0x76950156));
        rb_state17.var_17 = (rb_state17.var_17) + (251 < rb_input_size ? (uint32_t)rb_input[251] : 0x47895f9);
        rb_state17.var_18 = (rb_state17.var_18) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_15) : (0x1771dce6));
        rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_16) : (0x389641));
        rb_state17.var_20 = (rb_state17.var_20) - (((((0x2b2b63c2) ^ (0x98f9b9ef)) + (0xd8a1eac9)) - (rb_state17.var_17)) + (rb_state17.var_18));
        rb_state17.var_21 = (rb_state17.var_21) - (((0xb6d60e19) + (rb_state17.var_19)) + (rb_state17.var_13));
        rb_state17.var_22 = (rb_state17.var_22) - (((((0x87efa321) - (rb_state17.var_14)) ^ (rb_state17.var_20)) + (rb_state17.var_21)) - (0x62804881));
        rb_state17.var_4 = (rb_state17.var_4) + (rb_state17.var_22);
    }
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xa027ce97)) {
        rb_state1.var_7 = 0x21d2d18e;
        rb_state1.var_8 = 0x727aeb4e;
        rb_state1.var_9 = (rb_state1.var_9) - (rb_state1.var_7);
        rb_state1.var_10 = (rb_state1.var_10) ^ ((0x2923e266) + (rb_state1.var_5));
        rb_state1.var_11 = (rb_state1.var_11) + (((0xd8181fdc) + (rb_state1.var_4)) - (rb_state1.var_6));
        rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_8);
        rb_state1.var_13 = (rb_state1.var_13) ^ (((((0x7c2839c3) + (rb_state1.var_8)) + (rb_state1.var_9)) ^ (rb_state1.var_7)) - (rb_state1.var_10));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((((0x29439e84) ^ (rb_state1.var_11)) - (0xea95f32c)) + (rb_state1.var_9)) - (rb_state1.var_12));
        rb_state1.var_15 = (rb_state1.var_15) - (((((0x9ab2eeba) + (rb_state1.var_14)) - (0xfb90a2a)) - (0xa5725774)) + (rb_state1.var_13));
        rb_state1.var_6 = (rb_state1.var_6) + (rb_state1.var_15);
        rb_state1.var_3 = rb_state1.var_6;
    }
    if ((rb_state1.var_1) == (0xa027ce97)) {
        pthread_mutex_lock(&(rb_state1.lock_26));
        rb_state1.var_17 = 0xc9b7b454;
        rb_state1.var_18 = 0xafe17071;
        rb_state1.var_19 = (rb_state1.var_19) - (((((0xe5fac2f1) + (0x470f6ef3)) ^ (rb_state1.var_10)) ^ (0x138026b4)) ^ (rb_state1.var_17));
        rb_state1.var_20 = (rb_state1.var_20) ^ ((0xd6fb5392) - (rb_state1.var_11));
        rb_state1.var_21 = (rb_state1.var_21) + (rb_state1.var_18);
        rb_state1.var_22 = (rb_state1.var_22) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_19) : (0xccbea13c));
        rb_state1.var_23 = (rb_state1.var_23) - (rb_state1.var_20);
        rb_state1.var_24 = (rb_state1.var_24) ^ (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_22) : (0x7e6c9b6f));
        rb_state1.var_25 = (rb_state1.var_25) + (((rb_state1.var_23) == (0x0)) ? (rb_state1.var_24) : (0x760a8951));
        rb_state1.var_16 = (rb_state1.var_16) ^ (rb_state1.var_25);
        rb_state1.var_3 = rb_state1.var_16;
        pthread_mutex_unlock(&(rb_state1.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (66 < rb_input_size ? (uint32_t)rb_input[66] : 0x479b17ae);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0x10f6a4a4)) {
        rb_state7.var_15 = 0xf51f4afe;
        rb_state7.var_16 = 0xc5bcb8d;
        rb_state7.var_17 = 0xa5acd94d;
        rb_state7.var_18 = (rb_state7.var_18) ^ (rb_state7.var_16);
        rb_state7.var_19 = (rb_state7.var_19) ^ (((0x24e66bc3) + (rb_state7.var_17)) ^ (rb_state7.var_8));
        rb_state7.var_20 = (rb_state7.var_20) + (((0x2ce2d0d1) + (rb_state7.var_9)) - (rb_state7.var_15));
        rb_state7.var_21 = (rb_state7.var_21) - (((0xb235d50c) ^ (rb_state7.var_18)) - (0x3d247b0b));
        rb_state7.var_22 = (rb_state7.var_22) - (((rb_state7.var_19) == (0x0)) ? (rb_state7.var_20) : (0xa90925d5));
        rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_22) : (0xd9b56367));
        rb_state7.var_14 = (rb_state7.var_14) - (rb_state7.var_23);
        rb_state7.var_4 = rb_state7.var_14;
    }
    if ((rb_state7.var_2) == (0x10f6a4a4)) {
        pthread_mutex_lock(&(rb_state7.lock_38));
        rb_state7.var_25 = 0x24b03bc6;
        rb_state7.var_26 = 0x2c3c2429;
        rb_state7.var_27 = (rb_state7.var_27) ^ (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_26) : (0xfc380290));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((0x4a06ab44) ^ (rb_state7.var_12)) + (rb_state7.var_10));
        rb_state7.var_29 = (rb_state7.var_29) + (((0xa1394686) ^ (0x82c1a48a)) ^ (rb_state7.var_25));
        rb_state7.var_30 = (rb_state7.var_30) ^ (((0x372ae841) + (rb_state7.var_27)) + (0x21cc562));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((((0xb340a1a5) + (0x7f446e0b)) ^ (0x2144b1ce)) ^ (rb_state7.var_29)) ^ (rb_state7.var_28));
        rb_state7.var_32 = (rb_state7.var_32) - (rb_state7.var_30);
        rb_state7.var_33 = (rb_state7.var_33) + (rb_state7.var_31);
        rb_state7.var_34 = (rb_state7.var_34) - (((0xffbf5dd7) + (rb_state7.var_13)) - (rb_state7.var_32));
        rb_state7.var_35 = (rb_state7.var_35) + (((0xc0b0e372) ^ (rb_state7.var_33)) - (0x90fdc7dd));
        rb_state7.var_36 = (rb_state7.var_36) + (rb_state7.var_34);
        rb_state7.var_37 = (rb_state7.var_37) + (((rb_state7.var_35) == (0x0)) ? (rb_state7.var_36) : (0x127509f5));
        rb_state7.var_24 = (rb_state7.var_24) + (rb_state7.var_37);
        rb_state7.var_4 = rb_state7.var_24;
        pthread_mutex_unlock(&(rb_state7.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - (((rb_state15.var_3) == (0x55120e20)) ? (rb_state15.var_2) : (0xaed65865));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0x9614885c) + (rb_state19.var_1));
    #endif
    xLocal_1 = xLocal_2 + xLocal_1;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_1) == (0xbc68d223)) ? (rb_state2.var_2) : (0x1098bbfc));
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x5cc1ec22);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_2);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (214 < rb_input_size ? (uint32_t)rb_input[214] : 0x5e8268a3);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x4d322ad2)) {
        rb_state8.var_3 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0x4d322ad2)) {
        rb_state8.var_17 = !((rb_state8.var_1) == (rb_state8.var_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (0xc6543f68);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0xe487a63a)) ? (rb_state10.var_0) : (0x2b16aad7));
    rb_state10.var_3 = (rb_state10.var_3) ^ (((rb_state10.var_0) == (0xb3f5b8ae)) ? (rb_state10.var_3) : (0x9ecf2ea3));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) ^ (((rb_state13.var_1) == (0x738a076f)) ? (rb_state13.var_0) : (0x694079a1));
    #endif
    xLocal = xLocal_1 * xNPrimeofX;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_2) == (0xffffffa4)) ? (rb_state11.var_1) : (0xbe5e479));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
    rb_state12.var_1 = (rb_state12.var_1) + (0x8eb5557c);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_10 = (rb_state13.var_10) + (34 < rb_input_size ? (uint32_t)rb_input[34] : 0x68116a24);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (((rb_state15.var_2) == (0xffffffa0)) ? (rb_state15.var_3) : (0x8cc5be61));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (0x62237a23);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (0x17cea3b7);
    #endif
    xLocal = 1.0 - xLocal;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (0xbc8135c2);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (0xc7ac0f37);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0x662d4375));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_2) == (0x193c75d7)) ? (rb_state8.var_1) : (0xfca9554f));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0x524f6619)) {
        rb_state9.var_12 = 0xe7d044c6;
        rb_state9.var_13 = (rb_state9.var_13) + (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_9) : (0x4aff83de));
        rb_state9.var_14 = (rb_state9.var_14) + (rb_state9.var_10);
        rb_state9.var_15 = (rb_state9.var_15) ^ (rb_state9.var_11);
        rb_state9.var_16 = (rb_state9.var_16) + (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_13) : (0xf924f1ab));
        rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_14) : (0xa1fa6ee5));
        rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_15);
        rb_state9.var_19 = (rb_state9.var_19) + (((((0xe0e1dc7d) + (rb_state9.var_17)) + (0x58b5fe8a)) - (rb_state9.var_16)) + (0xf69a6dae));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((((0xc16962f0) ^ (rb_state9.var_19)) ^ (rb_state9.var_18)) + (0x27692f61)) + (0xaf9ec2bf));
        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_10 = (rb_state10.var_10) - ((0xa0144bda) + (rb_state10.var_11));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x15d4b4cc);
    #endif
    OutputX = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (243 < rb_input_size ? (uint32_t)rb_input[243] : 0xe8b8583f);
    rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0xf72183e7));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0x68d1749e) - (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (0x8040fe66);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_1) : (0xe4b0a325));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_6 = (rb_state10.var_6) - ((0x88a80a4f) ^ (0xf516681e));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + ((0x6cc83be0) - (rb_state13.var_2));
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_3) : (0xb7018d09));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0x5fb1ee3a);
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

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_2) : (0x71bc2b57));
    rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0x1030a484) - (rb_state10.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ ((0x13cfc3e3) ^ (rb_state16.var_0));
    #endif
    xStockPrice = sptprice;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_3);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (156 < rb_input_size ? (uint32_t)rb_input[156] : 0x1a603010);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0xc55affcd);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x975d41bd)) {
        if ((rb_state19.var_4) != (0x0)) {
            if (!((rb_state19.var_4) == (rb_state19.var_23))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    xStrikePrice = strike;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) ^ (0x5eaf88b5);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_5) == (0xbde4c562)) {
        rb_state2.var_14 = 0xf4ed551e;
        rb_state2.var_15 = (rb_state2.var_15) + (rb_state2.var_13);
        rb_state2.var_16 = (rb_state2.var_16) ^ (rb_state2.var_14);
        rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_15);
        rb_state2.var_18 = (rb_state2.var_18) - (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_16) : (0xa9cb7019));
        rb_state2.var_19 = (rb_state2.var_19) + (rb_state2.var_17);
        rb_state2.var_20 = (rb_state2.var_20) ^ (((((0xe6af0e75) - (rb_state2.var_18)) - (0x8e91376b)) ^ (rb_state2.var_19)) ^ (0x3b2b2c28));
        rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ ((0xe90b9981) - (0x422a026a));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + ((0x60d3466a) - (rb_state16.var_0));
    #endif
    xRiskFreeRate = rate;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (22 < rb_input_size ? (uint32_t)rb_input[22] : 0x7df18308);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_5) == (0x8acc13b5)) {
        rb_state10.var_16 = rb_state10.var_8;
    }
    if ((rb_state10.var_5) == (0x8acc13b5)) {
        if (!((rb_state10.var_8) == (rb_state10.var_16))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x55f7b033));
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x4aca9898));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - ((0xa333e11b) - (0xcc57d5a9));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_0) == (0x60d3466a)) ? (rb_state16.var_2) : (0x60b1820a));
    #endif
    xVolatility = volatility;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_7 = (rb_state2.var_7) - (11 < rb_input_size ? (uint32_t)rb_input[11] : 0x341bb7fe);
    #endif
    xTime = time;
    xSqrtTime = sqrt(xTime);

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0x38e5bfb1)) {
        pthread_mutex_lock(&(rb_state2.lock_11));
        rb_state2.var_5 = 0xbde4c562;
        rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_5);
        rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_5) : (0x7fb502e2));
        rb_state2.var_8 = (rb_state2.var_8) - (rb_state2.var_4);
        rb_state2.var_9 = (rb_state2.var_9) - (((((0xb588b044) - (rb_state2.var_6)) - (rb_state2.var_7)) - (rb_state2.var_6)) + (0x20af1a6e));
        rb_state2.var_10 = (rb_state2.var_10) ^ (((((0x2a1dde4c) ^ (rb_state2.var_8)) - (0x58c5a71d)) - (rb_state2.var_7)) ^ (rb_state2.var_9));
        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_10);
        rb_state2.var_4 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_11));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0x3f3417e6)) {
        rb_state3.var_4 = rb_state3.var_0;
    }
    if ((rb_state3.var_3) == (0x3f3417e6)) {
        pthread_mutex_lock(&(rb_state3.lock_15));
        if (!((rb_state3.var_0) == (rb_state3.var_4))) {
            racebench_trigger(3);
        }
        pthread_mutex_unlock(&(rb_state3.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x99aa3f36)) {
        rb_state5.var_6 = 0xad71b174;
        rb_state5.var_7 = 0x149c920;
        rb_state5.var_8 = (rb_state5.var_8) ^ ((0x4abd785c) + (rb_state5.var_6));
        rb_state5.var_9 = (rb_state5.var_9) - (((((0x54d0e6ec) - (0xe1def4d4)) ^ (rb_state5.var_7)) + (rb_state5.var_6)) ^ (rb_state5.var_7));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_8) : (0xa25d5514));
        rb_state5.var_11 = (rb_state5.var_11) ^ (((((0x67dca182) ^ (rb_state5.var_9)) + (0xc25d1516)) + (rb_state5.var_10)) + (0xacd4de9));
        rb_state5.var_5 = (rb_state5.var_5) ^ (rb_state5.var_11);
        rb_state5.var_4 = rb_state5.var_5;
    }
    if ((rb_state5.var_1) == (0x99aa3f36)) {
        pthread_mutex_lock(&(rb_state5.lock_25));
        rb_state5.var_13 = 0x28feeecc;
        rb_state5.var_14 = 0xaffd9912;
        rb_state5.var_15 = (rb_state5.var_15) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_9) : (0x7e8e950e));
        rb_state5.var_16 = (rb_state5.var_16) + (rb_state5.var_11);
        rb_state5.var_17 = (rb_state5.var_17) - (((((0xe183e90b) + (rb_state5.var_12)) ^ (rb_state5.var_14)) + (rb_state5.var_13)) + (rb_state5.var_13));
        rb_state5.var_18 = (rb_state5.var_18) - (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_15) : (0x2d8be9a1));
        rb_state5.var_19 = (rb_state5.var_19) + (((((0x908b5ebe) + (rb_state5.var_16)) + (rb_state5.var_14)) ^ (rb_state5.var_15)) - (rb_state5.var_17));
        rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_18);
        rb_state5.var_21 = (rb_state5.var_21) - (rb_state5.var_19);
        rb_state5.var_22 = (rb_state5.var_22) - (rb_state5.var_20);
        rb_state5.var_23 = (rb_state5.var_23) - (rb_state5.var_21);
        rb_state5.var_24 = (rb_state5.var_24) ^ (((((0x739aa357) ^ (rb_state5.var_23)) + (0x59cea9e6)) ^ (rb_state5.var_22)) - (rb_state5.var_16));
        rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_24);
        rb_state5.var_4 = rb_state5.var_12;
        pthread_mutex_unlock(&(rb_state5.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ ((0x2a258abe) + (0x3bfb451b));
    #endif
    logValues = log(sptprice / strike);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (0xf1f136e2);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xffffffa0)) {
        pthread_mutex_lock(&(rb_state4.lock_31));
        rb_state4.var_17 = 0x2ac0895b;
        rb_state4.var_18 = (rb_state4.var_18) + (rb_state4.var_10);
        rb_state4.var_19 = (rb_state4.var_19) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_17) : (0xd4ea062d));
        rb_state4.var_20 = (rb_state4.var_20) + (52 < rb_input_size ? (uint32_t)rb_input[52] : 0x7a1298a9);
        rb_state4.var_21 = (rb_state4.var_21) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_19) : (0xb3bc053));
        rb_state4.var_22 = (rb_state4.var_22) - (((0x8ab9ccd3) ^ (rb_state4.var_20)) - (rb_state4.var_12));
        rb_state4.var_23 = (rb_state4.var_23) ^ (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_22) : (0xd972dbac));
        rb_state4.var_16 = (rb_state4.var_16) + (rb_state4.var_23);
        rb_state4.var_4 = rb_state4.var_16;
        pthread_mutex_unlock(&(rb_state4.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_3);
    #endif
    xLogTerm = logValues;

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (24 < rb_input_size ? (uint32_t)rb_input[24] : 0x788aaea2);
    if ((rb_state4.var_1) == (0xffffffd0)) {
        pthread_mutex_lock(&(rb_state4.lock_31));
        rb_state4.var_25 = 0xb6721dd4;
        rb_state4.var_26 = 0x2afb48ee;
        rb_state4.var_27 = (rb_state4.var_27) - (rb_state4.var_25);
        rb_state4.var_28 = (rb_state4.var_28) - (((0x7c6e4b75) ^ (rb_state4.var_26)) - (0x9244ad39));
        rb_state4.var_29 = (rb_state4.var_29) - (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_27) : (0x6f1ed056));
        rb_state4.var_30 = (rb_state4.var_30) ^ (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_29) : (0xa8952494));
        rb_state4.var_24 = (rb_state4.var_24) + (rb_state4.var_30);
        rb_state4.var_4 = rb_state4.var_24;
        pthread_mutex_unlock(&(rb_state4.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0xc1c38fa6) + (0x22c41694));
    rb_state10.var_7 = (rb_state10.var_7) + (0x20cfa5);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - (0xa633c5d3);
    #endif
    xPowerTerm = xVolatility * xVolatility;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (0x1f7607ae);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0xb78b02e2));
    #endif
    xPowerTerm = xPowerTerm * 0.5;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + ((0x399df578) ^ (rb_state2.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (162 < rb_input_size ? (uint32_t)rb_input[162] : 0x3eb9673f);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (0x2c956d40);
    #endif
    xD1 = xRiskFreeRate + xPowerTerm;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x96437fe7) + (rb_state14.var_0));
    #endif
    xD1 = xD1 * xTime;
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xffffffa0)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_24))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + (0x8697c9d);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_14) == (0x59981ae7)) {
        pthread_mutex_lock(&(rb_state13.lock_45));
        rb_state13.var_35 = 0xeecbd309;
        rb_state13.var_36 = (rb_state13.var_36) - (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_24) : (0xdfd0f3e0));
        rb_state13.var_37 = (rb_state13.var_37) + (rb_state13.var_22);
        rb_state13.var_38 = (rb_state13.var_38) ^ (((((0x9fb6d659) + (0xc5513a6a)) - (rb_state13.var_21)) - (rb_state13.var_35)) ^ (rb_state13.var_26));
        rb_state13.var_39 = (rb_state13.var_39) - (rb_state13.var_36);
        rb_state13.var_40 = (rb_state13.var_40) - (((0x624b07b1) ^ (rb_state13.var_37)) - (rb_state13.var_27));
        rb_state13.var_41 = (rb_state13.var_41) - (((rb_state13.var_38) == (0x0)) ? (rb_state13.var_39) : (0x5fcabcdd));
        rb_state13.var_42 = (rb_state13.var_42) + (((0xa063d304) - (rb_state13.var_40)) + (0x6d11d194));
        rb_state13.var_43 = (rb_state13.var_43) ^ (((0xc599cc0d) + (0x828788be)) ^ (rb_state13.var_41));
        rb_state13.var_44 = (rb_state13.var_44) ^ (((((0xf087609e) ^ (rb_state13.var_43)) + (rb_state13.var_42)) - (rb_state13.var_29)) - (rb_state13.var_28));
        rb_state13.var_25 = (rb_state13.var_25) + (rb_state13.var_44);
        pthread_mutex_unlock(&(rb_state13.lock_45));
    }
    #endif
    xD1 = xD1 + xLogTerm;

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_1) : (0x8f06aadd));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x2c956d40)) ? (rb_state15.var_1) : (0x8f0eca83));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_3) == (0x52148062)) {
        pthread_mutex_lock(&(rb_state17.lock_23));
        rb_state17.var_6 = 0x141020eb;
        rb_state17.var_7 = (rb_state17.var_7) - (((((0x556594f5) - (rb_state17.var_6)) - (rb_state17.var_4)) + (0x4e177563)) - (rb_state17.var_6));
        rb_state17.var_8 = (rb_state17.var_8) ^ (((0x1810c2ff) + (rb_state17.var_7)) ^ (rb_state17.var_5));
        rb_state17.var_9 = (rb_state17.var_9) ^ ((0x7748d5fd) + (rb_state17.var_8));
        rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_7) : (0x2c291803));
        rb_state17.var_11 = (rb_state17.var_11) + (((((0x1ec867fa) + (rb_state17.var_8)) - (0xb8e2df26)) ^ (rb_state17.var_9)) - (rb_state17.var_10));
        rb_state17.var_12 = (rb_state17.var_12) + (((0xed558347) - (rb_state17.var_10)) + (rb_state17.var_11));
        rb_state17.var_13 = (rb_state17.var_13) + (((((0x9dbe3fc9) ^ (0xa6efb4a7)) ^ (0xe5b980fa)) - (rb_state17.var_12)) + (rb_state17.var_11));
        rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_13);
        rb_state17.var_4 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_23));
    }
    if ((rb_state17.var_3) == (0x52148062)) {
        pthread_mutex_lock(&(rb_state17.lock_23));
        if (!((rb_state17.var_4) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
        pthread_mutex_unlock(&(rb_state17.lock_23));
    }
    #endif
    xDen = xVolatility * xSqrtTime;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0x75861b3b)) {
        if ((rb_state0.var_4) != (0x0)) {
            if (!((rb_state0.var_4) == (rb_state0.var_13))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    xD1 = xD1 / xDen;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0x65639aa8) + (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_6 = (rb_state8.var_6) ^ (0x41f8dea6);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x6615cfb6)) {
        pthread_mutex_lock(&(rb_state10.lock_14));
        rb_state10.var_6 = (rb_state10.var_6) ^ ((0xebb684f6) + (rb_state10.var_6));
        rb_state10.var_7 = (rb_state10.var_7) - (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0xb855e08e));
        rb_state10.var_8 = (rb_state10.var_8) + ((0x817355cf) + (rb_state10.var_7));
        rb_state10.var_9 = (rb_state10.var_9) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_7) : (0x69115404));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_8) : (0xf4eb2360));
        rb_state10.var_11 = (rb_state10.var_11) - (rb_state10.var_9);
        rb_state10.var_12 = (rb_state10.var_12) + (rb_state10.var_10);
        rb_state10.var_13 = (rb_state10.var_13) - (((((0xb732040d) + (rb_state10.var_12)) + (0xb3a5b8c)) ^ (rb_state10.var_11)) + (rb_state10.var_9));
        rb_state10.var_4 = (rb_state10.var_4) ^ (rb_state10.var_13);
        rb_state10.var_5 = rb_state10.var_4;
        pthread_mutex_unlock(&(rb_state10.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x9ac2924)) {
        pthread_mutex_lock(&(rb_state13.lock_23));
        rb_state13.var_6 = 0xdb95674e;
        rb_state13.var_7 = 0x789e5573;
        rb_state13.var_8 = (rb_state13.var_8) ^ (71 < rb_input_size ? (uint32_t)rb_input[71] : 0xefe86150);
        rb_state13.var_9 = (rb_state13.var_9) ^ (172 < rb_input_size ? (uint32_t)rb_input[172] : 0xda1f4dc2);
        rb_state13.var_10 = (rb_state13.var_10) ^ (((0x14add661) ^ (rb_state13.var_6)) ^ (0x74a697bf));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((0xf7de27c5) + (0x7115d2f7)) - (rb_state13.var_4));
        rb_state13.var_12 = (rb_state13.var_12) + (((((0x3248758a) - (rb_state13.var_6)) + (rb_state13.var_5)) + (rb_state13.var_7)) + (rb_state13.var_8));
        rb_state13.var_13 = (rb_state13.var_13) + (rb_state13.var_9);
        rb_state13.var_14 = (rb_state13.var_14) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_10) : (0x59981ae7));
        rb_state13.var_15 = (rb_state13.var_15) - (((0x70fbecd2) - (0xcd5f62a1)) ^ (rb_state13.var_11));
        rb_state13.var_16 = (rb_state13.var_16) - (rb_state13.var_12);
        rb_state13.var_17 = (rb_state13.var_17) + (rb_state13.var_13);
        rb_state13.var_18 = (rb_state13.var_18) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_15) : (0x64f0f28f));
        rb_state13.var_19 = (rb_state13.var_19) ^ (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_17) : (0x33d01278));
        rb_state13.var_20 = (rb_state13.var_20) ^ (((0x9b77f0a6) + (rb_state13.var_8)) ^ (rb_state13.var_18));
        rb_state13.var_21 = (rb_state13.var_21) + (((0xbeb961e9) - (0x27ac0aac)) - (rb_state13.var_19));
        rb_state13.var_22 = (rb_state13.var_22) + (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_21) : (0xf40387da));
        rb_state13.var_4 = (rb_state13.var_4) - (rb_state13.var_22);
        rb_state13.var_5 = rb_state13.var_4;
        pthread_mutex_unlock(&(rb_state13.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_2);
    #endif
    xD2 = xD1 - xDen;

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (221 < rb_input_size ? (uint32_t)rb_input[221] : 0x7104d9d2);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_9 = (rb_state13.var_9) ^ (0xdc2708b4);
    #endif
    d1 = xD1;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_3);
    #endif
    d2 = xD2;

    NofXd1 = CNDF(d1);
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - ((0xd0ae7c6e) ^ (rb_state9.var_2));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (0x6f90af08);
    #endif
    NofXd2 = CNDF(d2);

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0x437b66f0) + (rb_state12.var_1));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0xc84799a0);
    #endif
    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_1) : (0x45d6b383));
        #endif
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) - ((0xa5ac9323) - (rb_state5.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_3 = (rb_state6.var_3) - ((0xdac021b5) ^ (rb_state6.var_3));
        #endif
        NegNofXd1 = (1.0 - NofXd1);
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_1) == (0x79df71db)) {
            pthread_mutex_lock(&(rb_state11.lock_42));
            rb_state11.var_5 = (rb_state11.var_5) - (0xedf1cfaf);
            rb_state11.var_6 = (rb_state11.var_6) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_5) : (0x60d8b50));
            rb_state11.var_7 = (rb_state11.var_7) + (((((0xc8c57ebf) - (rb_state11.var_7)) - (rb_state11.var_3)) - (rb_state11.var_4)) ^ (0xdaea9446));
            rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_8);
            rb_state11.var_9 = (rb_state11.var_9) + (((((0x76449304) ^ (0x8b8e1a8d)) ^ (0x346ad0d7)) + (rb_state11.var_5)) ^ (rb_state11.var_6));
            rb_state11.var_10 = (rb_state11.var_10) ^ (((0xef09899b) - (rb_state11.var_7)) - (0x52a5a036));
            rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_8) : (0xdd9dceeb));
            rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_9);
            rb_state11.var_13 = (rb_state11.var_13) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_10) : (0xde1a9cdc));
            rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_11);
            rb_state11.var_15 = (rb_state11.var_15) ^ (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_13) : (0xb8417187));
            rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_14) : (0xf4551038));
            rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_15) : (0xbf0f93c5));
            rb_state11.var_18 = (rb_state11.var_18) ^ (((0x43733f35) + (0x3c811605)) + (rb_state11.var_16));
            rb_state11.var_19 = (rb_state11.var_19) ^ (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x3da7afe1));
            rb_state11.var_4 = (rb_state11.var_4) ^ (rb_state11.var_19);
            rb_state11.var_3 = rb_state11.var_4;
            pthread_mutex_unlock(&(rb_state11.lock_42));
        }
        if ((rb_state11.var_1) == (0x79df71db)) {
            if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        NegNofXd2 = (1.0 - NofXd2);
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_1) == (0x79df71db)) {
            rb_state11.var_20 = 0xd0432808;
            rb_state11.var_21 = (rb_state11.var_21) ^ (((0x202208fb) ^ (rb_state11.var_15)) ^ (rb_state11.var_14));
            rb_state11.var_22 = (rb_state11.var_22) + (rb_state11.var_16);
            rb_state11.var_23 = (rb_state11.var_23) ^ (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_13) : (0x332c5477));
            rb_state11.var_24 = (rb_state11.var_24) - (((0xea0e9dd9) + (0xaa1a3c8e)) - (rb_state11.var_21));
            rb_state11.var_25 = (rb_state11.var_25) - (rb_state11.var_22);
            rb_state11.var_26 = (rb_state11.var_26) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_23) : (0x3e1fdda2));
            rb_state11.var_27 = (rb_state11.var_27) + (rb_state11.var_24);
            rb_state11.var_28 = (rb_state11.var_28) ^ (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_25) : (0xa487fb53));
            rb_state11.var_29 = (rb_state11.var_29) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_26) : (0xba01f3d5));
            rb_state11.var_30 = (rb_state11.var_30) - (rb_state11.var_27);
            rb_state11.var_31 = (rb_state11.var_31) ^ (((rb_state11.var_28) == (0x0)) ? (rb_state11.var_29) : (0x584047b0));
            rb_state11.var_32 = (rb_state11.var_32) + (((0xf963e061) - (0x96b048b5)) - (rb_state11.var_30));
            rb_state11.var_33 = (rb_state11.var_33) - (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_31) : (0xf4c829ff));
            rb_state11.var_34 = (rb_state11.var_34) + (((0x8c3ac22) - (rb_state11.var_21)) ^ (rb_state11.var_32));
            rb_state11.var_35 = (rb_state11.var_35) + (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_33) : (0x2f216574));
            rb_state11.var_36 = (rb_state11.var_36) + (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_34) : (0x9a08735e));
            rb_state11.var_37 = (rb_state11.var_37) - (((0xa33dbdc) + (0x4a41d773)) - (rb_state11.var_35));
            rb_state11.var_38 = (rb_state11.var_38) - (rb_state11.var_36);
            rb_state11.var_39 = (rb_state11.var_39) ^ (rb_state11.var_37);
            rb_state11.var_40 = (rb_state11.var_40) + (rb_state11.var_38);
            rb_state11.var_41 = (rb_state11.var_41) ^ (((((0x97b77b98) + (rb_state11.var_39)) + (rb_state11.var_40)) ^ (0x1486b1db)) ^ (0xbf102785));
            rb_state11.var_3 = (rb_state11.var_3) ^ (rb_state11.var_41);
        }
        #endif
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x30cd63f4)) {
        pthread_mutex_lock(&(rb_state12.lock_17));
        rb_state12.var_4 = 0x8809a2f7;
        rb_state12.var_5 = 0xecc97647;
        rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_5) : (0x1e9a1016));
        rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_4) : (0x77cbda73));
        rb_state12.var_8 = (rb_state12.var_8) + (((0x2666c625) ^ (rb_state12.var_3)) - (rb_state12.var_6));
        rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_6) : (0x5112742f));
        rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0xbe355e3));
        rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0x8a86dec9));
        rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0xc4133e6e));
        rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_11);
        rb_state12.var_14 = (rb_state12.var_14) + (((0x1dc91ef) - (rb_state12.var_12)) - (0xf48104a0));
        rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_13);
        rb_state12.var_16 = (rb_state12.var_16) + (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0xc2d5639d));
        rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_16);
        pthread_mutex_unlock(&(rb_state12.lock_17));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (140 < rb_input_size ? (uint32_t)rb_input[140] : 0xa6fd8a11);
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

            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) ^ (0x2203f22);
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_2) == (0x6615cfb6)) {
                usleep(100);
            }
            if ((rb_state10.var_2) == (0x6615cfb6)) {
                rb_state10.var_15 = !((rb_state10.var_5) == (rb_state10.var_4));
            }
            #endif
            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - (124 < rb_input_size ? (uint32_t)rb_input[124] : 0xfd76f040);
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_1) == (0x99aa3f36)) {
                pthread_mutex_lock(&(rb_state5.lock_25));
                if ((rb_state5.var_4) != (0x0)) {
                    if (!((rb_state5.var_4) == (rb_state5.var_12))) {
                        racebench_trigger(5);
                    }
                }
                pthread_mutex_unlock(&(rb_state5.lock_25));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_1);
            rb_state8.var_15 = (rb_state8.var_15) ^ (rb_state8.var_17);
            if ((rb_state8.var_4) == (0x36c55487)) {
                rb_state8.var_32 = 0x2360c9a0;
                rb_state8.var_33 = 0xb01688f7;
                rb_state8.var_34 = 0xf2400d52;
                rb_state8.var_35 = (rb_state8.var_35) + (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_25) : (0xc995e709));
                rb_state8.var_36 = (rb_state8.var_36) ^ (((0x44bfc525) + (0x4ed542a5)) - (rb_state8.var_34));
                rb_state8.var_37 = (rb_state8.var_37) + (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_33) : (0x2f9f1cbd));
                rb_state8.var_38 = (rb_state8.var_38) + (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_35) : (0x9bcd665f));
                rb_state8.var_39 = (rb_state8.var_39) - (rb_state8.var_36);
                rb_state8.var_40 = (rb_state8.var_40) ^ (((((0xd101924f) - (rb_state8.var_38)) + (0x97049fb5)) ^ (rb_state8.var_37)) - (rb_state8.var_28));
                rb_state8.var_41 = (rb_state8.var_41) + (((((0x822657ca) + (0x53998bac)) - (rb_state8.var_40)) ^ (rb_state8.var_39)) - (0x57b2c62d));
                rb_state8.var_18 = (rb_state8.var_18) - (rb_state8.var_41);
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_3 = (rb_state14.var_3) + (((rb_state14.var_1) == (0x3d35804b)) ? (rb_state14.var_0) : (0x47ef059d));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0x827e5caa));
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
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
            rb_state4.var_1 = (rb_state4.var_1) - (68 < rb_input_size ? (uint32_t)rb_input[68] : 0x7459a12c);
            #endif
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