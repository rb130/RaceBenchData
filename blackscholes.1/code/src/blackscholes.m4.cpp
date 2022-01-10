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
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_16) == (0xd1a15bc3)) {
            rb_state5.var_38 = rb_state5.var_17;
        }
        if ((rb_state5.var_16) == (0xd1a15bc3)) {
            if (!((rb_state5.var_17) == (rb_state5.var_38))) {
                racebench_trigger(5);
            }
        }
        #endif
        InputX = -InputX;
        sign = 1;
    } else {
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (130 < rb_input_size ? (uint32_t)rb_input[130] : 0x256ada56);
        #endif
        sign = 0;
    }

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6feadd8c)) {
        pthread_mutex_lock(&(rb_state0.lock_12));
        rb_state0.var_3 = 0x1;
        pthread_mutex_unlock(&(rb_state0.lock_12));
    }
    if ((rb_state0.var_0) == (0x6feadd8c)) {
        pthread_mutex_lock(&(rb_state0.lock_12));
        rb_state0.var_4 = 0x6063690f;
        rb_state0.var_5 = (rb_state0.var_5) + (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_4) : (0xc89170f5));
        rb_state0.var_6 = (rb_state0.var_6) ^ (((0x81db8307) - (rb_state0.var_2)) - (rb_state0.var_3));
        rb_state0.var_7 = (rb_state0.var_7) - ((0x9c979306) + (rb_state0.var_4));
        rb_state0.var_8 = (rb_state0.var_8) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_5) : (0x82587eba));
        rb_state0.var_9 = (rb_state0.var_9) - (((0xd9868b3e) + (0x463ce37)) + (rb_state0.var_6));
        rb_state0.var_10 = (rb_state0.var_10) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_8) : (0x575c95a5));
        rb_state0.var_11 = (rb_state0.var_11) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_10) : (0x9a79f9eb));
        rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_11);
        rb_state0.var_1 = rb_state0.var_2;
        pthread_mutex_unlock(&(rb_state0.lock_12));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xb4ba83df)) {
        pthread_mutex_lock(&(rb_state5.lock_36));
        rb_state5.var_3 = 0x95ebf352;
        rb_state5.var_4 = 0x71fce212;
        rb_state5.var_5 = (rb_state5.var_5) + (rb_state5.var_2);
        rb_state5.var_6 = (rb_state5.var_6) ^ (rb_state5.var_1);
        rb_state5.var_7 = (rb_state5.var_7) + (rb_state5.var_3);
        rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_4);
        rb_state5.var_9 = (rb_state5.var_9) ^ (rb_state5.var_5);
        rb_state5.var_10 = (rb_state5.var_10) ^ (((0xf09df81e) - (rb_state5.var_3)) ^ (rb_state5.var_6));
        rb_state5.var_11 = (rb_state5.var_11) - (rb_state5.var_7);
        rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_8);
        rb_state5.var_13 = (rb_state5.var_13) + (((0x3692cc7b) + (rb_state5.var_9)) - (0x834283e));
        rb_state5.var_14 = (rb_state5.var_14) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_11) : (0x8b86c894));
        rb_state5.var_15 = (rb_state5.var_15) - (((0x2688f5fc) ^ (rb_state5.var_4)) ^ (rb_state5.var_12));
        rb_state5.var_16 = (rb_state5.var_16) - (rb_state5.var_13);
        rb_state5.var_17 = (rb_state5.var_17) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_14) : (0x6c8c7965));
        rb_state5.var_18 = (rb_state5.var_18) ^ (rb_state5.var_15);
        rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_16) : (0x4e0c9ca3));
        rb_state5.var_20 = (rb_state5.var_20) + (rb_state5.var_17);
        rb_state5.var_21 = (rb_state5.var_21) + (((0xefa8a90b) ^ (rb_state5.var_18)) ^ (rb_state5.var_7));
        rb_state5.var_22 = (rb_state5.var_22) + (rb_state5.var_19);
        rb_state5.var_23 = (rb_state5.var_23) - (rb_state5.var_20);
        rb_state5.var_24 = (rb_state5.var_24) ^ (((0x96360c0d) ^ (0x5c35315f)) - (rb_state5.var_21));
        rb_state5.var_25 = (rb_state5.var_25) + (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_23) : (0x170f1d1d));
        rb_state5.var_26 = (rb_state5.var_26) ^ (((((0xec659c58) - (rb_state5.var_25)) ^ (0xa63862cc)) ^ (0x13600dc)) + (rb_state5.var_24));
        rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_26);
        rb_state5.var_1 = rb_state5.var_2;
        pthread_mutex_unlock(&(rb_state5.lock_36));
    }
    if ((rb_state5.var_0) == (0xb4ba83df)) {
        pthread_mutex_lock(&(rb_state5.lock_36));
        rb_state5.var_37 = !((rb_state5.var_1) == (rb_state5.var_2));
        pthread_mutex_unlock(&(rb_state5.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x696802ba)) {
        pthread_mutex_lock(&(rb_state10.lock_23));
        rb_state10.var_13 = 0xac665a56;
        rb_state10.var_14 = 0xeae6ba05;
        rb_state10.var_15 = (rb_state10.var_15) + (((0x6322a78f) ^ (rb_state10.var_6)) + (rb_state10.var_14));
        rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_13) : (0x1c6b2db9));
        rb_state10.var_17 = (rb_state10.var_17) - (((((0x2ab27f0a) ^ (rb_state10.var_15)) + (rb_state10.var_16)) + (rb_state10.var_7)) + (0x43148501));
        rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_17);
        rb_state10.var_1 = rb_state10.var_12;
        pthread_mutex_unlock(&(rb_state10.lock_23));
    }
    if ((rb_state10.var_0) == (0x696802ba)) {
        pthread_mutex_lock(&(rb_state10.lock_23));
        rb_state10.var_19 = 0xf5eaabfc;
        rb_state10.var_20 = 0xab346966;
        rb_state10.var_21 = (rb_state10.var_21) ^ (((((0x5ac9b040) ^ (rb_state10.var_8)) - (rb_state10.var_19)) + (rb_state10.var_9)) ^ (rb_state10.var_10));
        rb_state10.var_22 = (rb_state10.var_22) + (((((0xc8e48c6b) ^ (rb_state10.var_11)) - (rb_state10.var_20)) + (rb_state10.var_21)) ^ (0x3f13d30d));
        rb_state10.var_18 = (rb_state10.var_18) - (rb_state10.var_22);
        rb_state10.var_1 = rb_state10.var_18;
        pthread_mutex_unlock(&(rb_state10.lock_23));
    }
    #endif
    xInput = InputX;

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x7ebf9419)) {
        rb_state12.var_3 = 0xf95dc2a0;
        rb_state12.var_4 = (rb_state12.var_4) ^ (((0xdcf42a3c) - (rb_state12.var_2)) ^ (rb_state12.var_4));
        rb_state12.var_5 = (rb_state12.var_5) ^ (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_3) : (0x1e036723));
        rb_state12.var_6 = (rb_state12.var_6) ^ ((0x3e26b231) + (rb_state12.var_6));
        rb_state12.var_7 = (rb_state12.var_7) - (((0x340e5afe) + (rb_state12.var_4)) + (0x57f9d73b));
        rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_5) : (0xfd601397));
        rb_state12.var_9 = (rb_state12.var_9) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_6) : (0x984eb99e));
        rb_state12.var_10 = (rb_state12.var_10) + (((0x12c0076a) - (0xccdcd459)) ^ (rb_state12.var_7));
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_8) : (0x6700594b));
        rb_state12.var_12 = (rb_state12.var_12) + (rb_state12.var_9);
        rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_10);
        rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_11);
        rb_state12.var_15 = (rb_state12.var_15) - (((0xcdeee438) ^ (rb_state12.var_12)) - (rb_state12.var_10));
        rb_state12.var_16 = (rb_state12.var_16) - (((0xaa7149ac) ^ (rb_state12.var_13)) - (rb_state12.var_11));
        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0x3bfa68bf));
        rb_state12.var_18 = (rb_state12.var_18) - (((((0x609e302e) + (rb_state12.var_12)) + (0xe2f4d1e)) ^ (rb_state12.var_17)) + (rb_state12.var_16));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_18);
    }
    #endif
    expValues = exp(-0.5f * InputX * InputX);
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xf5fe1d38)) {
        pthread_mutex_lock(&(rb_state6.lock_26));
        if ((rb_state6.var_1) != (0x0)) {
            if (!((rb_state6.var_1) == (rb_state6.var_12))) {
                racebench_trigger(6);
            }
        }
        pthread_mutex_unlock(&(rb_state6.lock_26));
    }
    #endif
    xNPrimeofX = expValues;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x8b723da9) - (0x321682a7));
    #endif
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xffffc130)) {
        pthread_mutex_lock(&(rb_state3.lock_27));
        rb_state3.var_11 = 0x33a40468;
        rb_state3.var_12 = 0x346db9ae;
        rb_state3.var_13 = (rb_state3.var_13) - (0x49c50625);
        rb_state3.var_14 = (rb_state3.var_14) ^ (85 < rb_input_size ? (uint32_t)rb_input[85] : 0x83379b45);
        rb_state3.var_15 = (rb_state3.var_15) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_12) : (0x890134fc));
        rb_state3.var_16 = (rb_state3.var_16) - (rb_state3.var_11);
        rb_state3.var_17 = (rb_state3.var_17) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_13) : (0x41e149b4));
        rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_14);
        rb_state3.var_19 = (rb_state3.var_19) - (rb_state3.var_15);
        rb_state3.var_20 = (rb_state3.var_20) ^ (((0x2c234f72) + (rb_state3.var_7)) ^ (rb_state3.var_16));
        rb_state3.var_21 = (rb_state3.var_21) ^ (((0x17befaa) ^ (rb_state3.var_8)) + (rb_state3.var_17));
        rb_state3.var_22 = (rb_state3.var_22) + (((0x6c496417) + (rb_state3.var_18)) ^ (rb_state3.var_9));
        rb_state3.var_23 = (rb_state3.var_23) ^ (rb_state3.var_19);
        rb_state3.var_24 = (rb_state3.var_24) - (((((0x4d0c0632) ^ (rb_state3.var_20)) ^ (rb_state3.var_10)) - (0x6a28c29d)) ^ (rb_state3.var_21));
        rb_state3.var_25 = (rb_state3.var_25) - (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_23) : (0xd1cb62ac));
        rb_state3.var_26 = (rb_state3.var_26) - (((((0x5a5ca1f2) ^ (rb_state3.var_11)) - (rb_state3.var_25)) - (0x5964926b)) + (rb_state3.var_24));
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_26);
        pthread_mutex_unlock(&(rb_state3.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xe2d93270)) {
        pthread_mutex_lock(&(rb_state4.lock_10));
        rb_state4.var_3 = (rb_state4.var_3) - (((0xf29a783f) ^ (rb_state4.var_1)) ^ (0x331a09e6));
        rb_state4.var_4 = (rb_state4.var_4) - (0x146d99a9);
        rb_state4.var_5 = (rb_state4.var_5) - (((0x160d0558) - (0x61434a3e)) ^ (rb_state4.var_2));
        rb_state4.var_6 = (rb_state4.var_6) + (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_4) : (0x69b13566));
        rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_5) : (0x6de55be2));
        rb_state4.var_8 = (rb_state4.var_8) + (rb_state4.var_6);
        rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_8) : (0x1e1242e7));
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_9);
        rb_state4.var_2 = rb_state4.var_1;
        pthread_mutex_unlock(&(rb_state4.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x696802ba)) {
        if ((rb_state10.var_1) != (0x0)) {
            rb_state10.var_24 = !((rb_state10.var_1) == (rb_state10.var_18));
        }
    }
    #endif
    xK2 = 0.2316419 * xInput;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (218 < rb_input_size ? (uint32_t)rb_input[218] : 0x4ef14a82);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (0xa01def8);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (0xe19d8cdb);
    #endif
    xK2 = 1.0 + xK2;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x5207156b)) {
        rb_state1.var_17 = 0x26b1ee3f;
        rb_state1.var_18 = (rb_state1.var_18) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_10) : (0xa8e44564));
        rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_17) : (0x189210c2));
        rb_state1.var_20 = (rb_state1.var_20) + (rb_state1.var_18);
        rb_state1.var_21 = (rb_state1.var_21) ^ (rb_state1.var_19);
        rb_state1.var_22 = (rb_state1.var_22) - (((((0x7ad21345) ^ (0x53dec3b6)) - (rb_state1.var_21)) + (rb_state1.var_12)) - (rb_state1.var_20));
        rb_state1.var_16 = (rb_state1.var_16) - (rb_state1.var_22);
        rb_state1.var_1 = rb_state1.var_16;
    }
    if ((rb_state1.var_0) == (0x5207156b)) {
        pthread_mutex_lock(&(rb_state1.lock_33));
        rb_state1.var_24 = 0xf0cce046;
        rb_state1.var_25 = (rb_state1.var_25) ^ (((((0xd8375d6f) ^ (0x3576fdf5)) - (0xdbf48a1a)) - (rb_state1.var_13)) - (rb_state1.var_24));
        rb_state1.var_26 = (rb_state1.var_26) + (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_15) : (0x14d764e2));
        rb_state1.var_27 = (rb_state1.var_27) ^ (208 < rb_input_size ? (uint32_t)rb_input[208] : 0xebcd58d0);
        rb_state1.var_28 = (rb_state1.var_28) - (rb_state1.var_14);
        rb_state1.var_29 = (rb_state1.var_29) - (((0xc88fbed7) + (0xff072441)) - (rb_state1.var_25));
        rb_state1.var_30 = (rb_state1.var_30) - (((rb_state1.var_26) == (0x0)) ? (rb_state1.var_27) : (0x84d9d55d));
        rb_state1.var_31 = (rb_state1.var_31) - (((rb_state1.var_28) == (0x0)) ? (rb_state1.var_29) : (0xa29a2258));
        rb_state1.var_32 = (rb_state1.var_32) + (((rb_state1.var_30) == (0x0)) ? (rb_state1.var_31) : (0xb01a8adf));
        rb_state1.var_23 = (rb_state1.var_23) - (rb_state1.var_32);
        rb_state1.var_1 = rb_state1.var_23;
        pthread_mutex_unlock(&(rb_state1.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x9c98507e));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x5405f95c)) {
        rb_state11.var_3 = (rb_state11.var_3) - (0x406c20e1);
        rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_2);
        rb_state11.var_5 = (rb_state11.var_5) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_1) : (0xe386baf));
        rb_state11.var_6 = (rb_state11.var_6) - ((0x3bcb90d6) + (rb_state11.var_4));
        rb_state11.var_7 = (rb_state11.var_7) - (((0xe3190ea2) + (rb_state11.var_3)) - (rb_state11.var_5));
        rb_state11.var_8 = (rb_state11.var_8) ^ (((0xf62fbb8d) ^ (rb_state11.var_4)) ^ (0xf5069aa9));
        rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0x47cf2795));
        rb_state11.var_10 = (rb_state11.var_10) - (((((0x87f922a1) - (rb_state11.var_8)) - (0x5f742ad2)) ^ (rb_state11.var_7)) + (rb_state11.var_6));
        rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_9) : (0xc8119d33));
        rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_10);
        rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_11) : (0xa15e554b));
        rb_state11.var_14 = (rb_state11.var_14) ^ (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) + (((((0x9034beef) - (0x7da82c1c)) ^ (rb_state11.var_9)) ^ (rb_state11.var_14)) - (rb_state11.var_13));
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x7ebf9419)) {
        rb_state12.var_2 = rb_state12.var_1;
    }
    if ((rb_state12.var_0) == (0x7ebf9419)) {
        if (!((rb_state12.var_1) == (rb_state12.var_2))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x9a4bab15)) {
        rb_state15.var_2 = rb_state15.var_1;
    }
    if ((rb_state15.var_0) == (0x4ab7269)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_8) == (0x72c5c702)) {
        pthread_mutex_lock(&(rb_state18.lock_42));
        rb_state18.var_19 = 0x85169398;
        rb_state18.var_20 = 0x94b9323a;
        rb_state18.var_21 = (rb_state18.var_21) - (rb_state18.var_10);
        rb_state18.var_22 = (rb_state18.var_22) + (rb_state18.var_9);
        rb_state18.var_23 = (rb_state18.var_23) ^ (((0x651c3faa) ^ (0xf263bc3d)) - (rb_state18.var_20));
        rb_state18.var_24 = (rb_state18.var_24) + (rb_state18.var_19);
        rb_state18.var_25 = (rb_state18.var_25) + (((((0xb6d489d9) ^ (rb_state18.var_22)) - (rb_state18.var_21)) + (rb_state18.var_11)) + (0x3290c665));
        rb_state18.var_26 = (rb_state18.var_26) + (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_23) : (0x973a6371));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_24) : (0xb7c986c9));
        rb_state18.var_28 = (rb_state18.var_28) ^ (rb_state18.var_25);
        rb_state18.var_29 = (rb_state18.var_29) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_26) : (0xd217a1f7));
        rb_state18.var_30 = (rb_state18.var_30) ^ (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_27) : (0x9efa328f));
        rb_state18.var_31 = (rb_state18.var_31) - (rb_state18.var_28);
        rb_state18.var_32 = (rb_state18.var_32) ^ (((rb_state18.var_29) == (0x0)) ? (rb_state18.var_30) : (0x6420b58f));
        rb_state18.var_33 = (rb_state18.var_33) + (((0x990c32c2) - (rb_state18.var_31)) + (0x539dac6d));
        rb_state18.var_34 = (rb_state18.var_34) - (((rb_state18.var_17) == (0x0)) ? (rb_state18.var_32) : (0x8237fa61));
        rb_state18.var_35 = (rb_state18.var_35) + (((0xb7c8fb19) - (rb_state18.var_18)) ^ (rb_state18.var_33));
        rb_state18.var_36 = (rb_state18.var_36) - (((rb_state18.var_34) == (0x0)) ? (rb_state18.var_35) : (0xf316d5c));
        rb_state18.var_18 = (rb_state18.var_18) + (rb_state18.var_36);
        rb_state18.var_17 = rb_state18.var_18;
        pthread_mutex_unlock(&(rb_state18.lock_42));
    }
    if ((rb_state18.var_8) == (0x72c5c702)) {
        pthread_mutex_lock(&(rb_state18.lock_42));
        if (!((rb_state18.var_17) == (rb_state18.var_18))) {
            racebench_trigger(18);
        }
        pthread_mutex_unlock(&(rb_state18.lock_42));
    }
    #endif
    xK2 = 1.0 / xK2;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_13) == (0xac665a56)) {
        rb_state10.var_26 = 0xf4106e4e;
        rb_state10.var_27 = 0xd4ec2b11;
        rb_state10.var_28 = (rb_state10.var_28) ^ (((((0xe682cb53) - (0x7a73f53c)) + (rb_state10.var_16)) + (rb_state10.var_14)) - (rb_state10.var_26));
        rb_state10.var_29 = (rb_state10.var_29) - (rb_state10.var_27);
        rb_state10.var_30 = (rb_state10.var_30) + (rb_state10.var_28);
        rb_state10.var_31 = (rb_state10.var_31) + (((rb_state10.var_29) == (0x0)) ? (rb_state10.var_30) : (0x21c53dce));
        rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (204 < rb_input_size ? (uint32_t)rb_input[204] : 0x567d0773);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x9ee0d97c));
    if ((rb_state15.var_0) == (0x4ab7269)) {
        rb_state15.var_3 = 0x7a8cc0d4;
        rb_state15.var_4 = (rb_state15.var_4) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x427ab0a2));
        rb_state15.var_5 = (rb_state15.var_5) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_1) : (0xeb786788));
        rb_state15.var_6 = (rb_state15.var_6) - (((0x51a39d9f) + (rb_state15.var_6)) ^ (rb_state15.var_3));
        rb_state15.var_7 = (rb_state15.var_7) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_2) : (0x6b0150db));
        rb_state15.var_8 = (rb_state15.var_8) - (((0x80e62891) + (rb_state15.var_4)) + (rb_state15.var_8));
        rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_5);
        rb_state15.var_10 = (rb_state15.var_10) - (((0x73a962f5) - (rb_state15.var_6)) ^ (rb_state15.var_9));
        rb_state15.var_11 = (rb_state15.var_11) ^ (rb_state15.var_7);
        rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_8) : (0x9e5adf15));
        rb_state15.var_13 = (rb_state15.var_13) + (((0x906e89d6) + (rb_state15.var_11)) - (rb_state15.var_9));
        rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_10);
        rb_state15.var_15 = (rb_state15.var_15) - (rb_state15.var_11);
        rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_12) : (0x3ffd71b0));
        rb_state15.var_17 = (rb_state15.var_17) ^ (rb_state15.var_13);
        rb_state15.var_18 = (rb_state15.var_18) ^ (rb_state15.var_14);
        rb_state15.var_19 = (rb_state15.var_19) ^ (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_16) : (0x2f256e1d));
        rb_state15.var_20 = (rb_state15.var_20) - (((0x2cc5069a) + (rb_state15.var_17)) + (rb_state15.var_13));
        rb_state15.var_21 = (rb_state15.var_21) + (((((0xa8bd8d70) - (rb_state15.var_14)) ^ (rb_state15.var_18)) ^ (rb_state15.var_15)) ^ (rb_state15.var_19));
        rb_state15.var_22 = (rb_state15.var_22) ^ (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_21) : (0x4fc9b01d));
        rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_22);
    }
    #endif
    xK2_2 = xK2 * xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x350660da);
    #endif
    xK2_3 = xK2_2 * xK2;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ ((0xae071545) + (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xf2f69966)) {
        pthread_mutex_lock(&(rb_state9.lock_19));
        rb_state9.var_3 = (rb_state9.var_3) + (139 < rb_input_size ? (uint32_t)rb_input[139] : 0x2368b72e);
        rb_state9.var_4 = (rb_state9.var_4) + (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_2) : (0x3c1c3cb2));
        rb_state9.var_5 = (rb_state9.var_5) ^ (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_1) : (0x5e142504));
        rb_state9.var_6 = (rb_state9.var_6) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0xb489c7d0));
        rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_5) : (0xe090aaa0));
        rb_state9.var_8 = (rb_state9.var_8) + (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_6) : (0x1c1ce01b));
        rb_state9.var_9 = (rb_state9.var_9) + (rb_state9.var_7);
        rb_state9.var_10 = (rb_state9.var_10) + (((0xf871d6e6) ^ (rb_state9.var_7)) - (rb_state9.var_8));
        rb_state9.var_11 = (rb_state9.var_11) ^ (rb_state9.var_9);
        rb_state9.var_12 = (rb_state9.var_12) + (((0x7b7e7903) ^ (rb_state9.var_10)) ^ (0x6f77c7b4));
        rb_state9.var_13 = (rb_state9.var_13) - (((0xa90929ad) ^ (rb_state9.var_8)) + (rb_state9.var_11));
        rb_state9.var_14 = (rb_state9.var_14) - (((((0xaad2e33a) + (rb_state9.var_9)) + (rb_state9.var_12)) + (rb_state9.var_10)) - (rb_state9.var_13));
        rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_14);
        rb_state9.var_1 = rb_state9.var_2;
        pthread_mutex_unlock(&(rb_state9.lock_19));
    }
    if ((rb_state9.var_0) == (0xf2f69966)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
    }
    #endif
    xK2_4 = xK2_3 * xK2;
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xf2f69966)) {
        rb_state9.var_15 = 0xfd7e116d;
        rb_state9.var_16 = 0xf0882b40;
        rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_15) : (0x3d69ace7));
        rb_state9.var_18 = (rb_state9.var_18) ^ (((((0x10bb9cd5) ^ (rb_state9.var_16)) + (rb_state9.var_17)) - (0x22e726f4)) + (rb_state9.var_12));
        rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (27 < rb_input_size ? (uint32_t)rb_input[27] : 0x1c8cdeb7);
    #endif
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (0x50011809);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0xef6ed5aa);
    #endif
    xLocal_2 = xK2_2 * (-0.356563782);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (166 < rb_input_size ? (uint32_t)rb_input[166] : 0xaae2ccb0);
    if ((rb_state2.var_0) == (0x595bd1af)) {
        if ((rb_state2.var_1) != (0x0)) {
            if (!((rb_state2.var_1) == (rb_state2.var_25))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xe2d93270)) {
        pthread_mutex_lock(&(rb_state4.lock_10));
        rb_state4.var_11 = !((rb_state4.var_2) == (rb_state4.var_1));
        pthread_mutex_unlock(&(rb_state4.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_12 = (rb_state10.var_12) - (0xf98a78f7);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x5f3bc05a)) {
        rb_state17.var_4 = 0xbea6118d;
        rb_state17.var_5 = 0xfd68f406;
        rb_state17.var_6 = (rb_state17.var_6) ^ (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_5) : (0xf5c4d1cc));
        rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_3) : (0xff2bf71c));
        rb_state17.var_8 = (rb_state17.var_8) + (rb_state17.var_6);
        rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_7);
        rb_state17.var_10 = (rb_state17.var_10) - (rb_state17.var_8);
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_10) : (0x30515d0c));
        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_11);
        rb_state17.var_1 = rb_state17.var_3;
    }
    if ((rb_state17.var_0) == (0x5f3bc05a)) {
        pthread_mutex_lock(&(rb_state17.lock_17));
        rb_state17.var_13 = 0x2c74a3ab;
        rb_state17.var_14 = 0x3f7a2dbf;
        rb_state17.var_15 = (rb_state17.var_15) - (((((0x61581c11) ^ (rb_state17.var_6)) - (0xf9906535)) + (rb_state17.var_5)) ^ (rb_state17.var_14));
        rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_15) : (0x5c775893));
        rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_16);
        rb_state17.var_1 = rb_state17.var_12;
        pthread_mutex_unlock(&(rb_state17.lock_17));
    }
    #endif
    xLocal_3 = xK2_3 * 1.781477937;
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (0x6f867087);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0x2bd2b63c);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_17
    if (!((rb_state17.var_1) == (rb_state17.var_12))) {
        pthread_mutex_lock(&(rb_state17.lock_42));
        rb_state17.var_20 = 0xef0897d9;
        rb_state17.var_21 = 0xca86d50a;
        rb_state17.var_22 = (rb_state17.var_22) - (((((0xd41666f6) + (rb_state17.var_9)) ^ (rb_state17.var_8)) + (rb_state17.var_20)) + (rb_state17.var_21));
        rb_state17.var_23 = (rb_state17.var_23) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_7) : (0xd3650fa));
        rb_state17.var_24 = (rb_state17.var_24) + (((0xce920f15) + (rb_state17.var_11)) ^ (rb_state17.var_22));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_23) : (0x62a0925b));
        rb_state17.var_26 = (rb_state17.var_26) + (((0x7481042a) - (rb_state17.var_24)) ^ (0xb6a4616c));
        rb_state17.var_27 = (rb_state17.var_27) ^ (((((0xcec9b8af) - (rb_state17.var_13)) + (rb_state17.var_26)) - (rb_state17.var_25)) ^ (0x219f7ef4));
        rb_state17.var_19 = (rb_state17.var_19) ^ (rb_state17.var_27);
        rb_state17.var_18 = rb_state17.var_19;
        pthread_mutex_unlock(&(rb_state17.lock_42));
    }
    if (!((rb_state17.var_1) == (rb_state17.var_12))) {
        pthread_mutex_lock(&(rb_state17.lock_42));
        if (!((rb_state17.var_18) == (rb_state17.var_19))) {
            racebench_trigger(17);
        }
        pthread_mutex_unlock(&(rb_state17.lock_42));
    }
    #endif
    xLocal_3 = xK2_4 * (-1.821255978);
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_5 = (rb_state18.var_5) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_6) : (0x7f5a03b9));
    #endif
    xLocal_3 = xK2_5 * 1.330274429;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_6 = (rb_state0.var_6) - (rb_state0.var_7);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x67c9ccce) + (rb_state5.var_0));
    if ((rb_state5.var_16) == (0xd1a15bc3)) {
        rb_state5.var_39 = 0x5846c490;
        rb_state5.var_40 = 0xeceb0d10;
        rb_state5.var_41 = (rb_state5.var_41) + (((0x97ba038) ^ (rb_state5.var_39)) + (rb_state5.var_19));
        rb_state5.var_42 = (rb_state5.var_42) ^ (((((0xa7a47a8) - (0xc9793962)) + (rb_state5.var_40)) ^ (0xd7dc31e6)) - (rb_state5.var_18));
        rb_state5.var_43 = (rb_state5.var_43) ^ (((0xc4c6e6cb) + (rb_state5.var_20)) - (rb_state5.var_41));
        rb_state5.var_44 = (rb_state5.var_44) ^ (((rb_state5.var_21) == (0x0)) ? (rb_state5.var_42) : (0xcf5a098));
        rb_state5.var_45 = (rb_state5.var_45) + (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_43) : (0x3dade784));
        rb_state5.var_46 = (rb_state5.var_46) + (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_44) : (0x6341400f));
        rb_state5.var_47 = (rb_state5.var_47) ^ (((((0xc71af5b7) - (rb_state5.var_45)) ^ (rb_state5.var_25)) + (rb_state5.var_24)) ^ (rb_state5.var_46));
        rb_state5.var_17 = (rb_state5.var_17) + (rb_state5.var_47);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xa0f37082)) {
        rb_state7.var_11 = 0xe39b61cf;
        rb_state7.var_12 = (rb_state7.var_12) + (((((0x93a10ecb) - (0x2aaf6585)) + (rb_state7.var_7)) + (rb_state7.var_6)) + (rb_state7.var_8));
        rb_state7.var_13 = (rb_state7.var_13) + (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_12) : (0xc0241983));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x7d6fc0d6));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x5e1af89)) {
        rb_state18.var_16 = !((rb_state18.var_2) == (rb_state18.var_1));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x502087cf)) {
        if ((rb_state19.var_1) != (0x0)) {
            if (!((rb_state19.var_1) == (rb_state19.var_31))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (222 < rb_input_size ? (uint32_t)rb_input[222] : 0x16f6e4c4);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (239 < rb_input_size ? (uint32_t)rb_input[239] : 0xebf03e36);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0x86f9d994) + (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xa22f0997));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x5f3bc05a)) {
        if ((rb_state17.var_1) != (0x0)) {
            if (!((rb_state17.var_1) == (rb_state17.var_12))) {
                rb_state17.var_28 = 0x21efce3f;
                rb_state17.var_29 = 0xf2244c39;
                rb_state17.var_30 = (rb_state17.var_30) ^ (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_29) : (0xad13399c));
                rb_state17.var_31 = (rb_state17.var_31) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_28) : (0xb0991a5c));
                rb_state17.var_32 = (rb_state17.var_32) ^ (((0xe919ecf1) - (0xb5997ba9)) - (rb_state17.var_14));
                rb_state17.var_33 = (rb_state17.var_33) + (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_30) : (0xfa077ed8));
                rb_state17.var_34 = (rb_state17.var_34) + (((0x79c1b0c1) - (rb_state17.var_31)) + (rb_state17.var_19));
                rb_state17.var_35 = (rb_state17.var_35) ^ (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_32) : (0x4645b34));
                rb_state17.var_36 = (rb_state17.var_36) - (((0xa214188f) - (rb_state17.var_33)) - (0xa2a484a1));
                rb_state17.var_37 = (rb_state17.var_37) + (((((0xa3e79c92) + (rb_state17.var_34)) ^ (rb_state17.var_35)) - (rb_state17.var_22)) - (rb_state17.var_21));
                rb_state17.var_38 = (rb_state17.var_38) ^ (rb_state17.var_36);
                rb_state17.var_39 = (rb_state17.var_39) ^ (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_37) : (0xcc16c38));
                rb_state17.var_40 = (rb_state17.var_40) - (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_38) : (0x82faa4a5));
                rb_state17.var_41 = (rb_state17.var_41) + (((rb_state17.var_39) == (0x0)) ? (rb_state17.var_40) : (0x30f1c9a));
                rb_state17.var_18 = (rb_state17.var_18) - (rb_state17.var_41);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x347d975f)) ? (rb_state18.var_0) : (0xc4215175));
    #endif
    xLocal_1 = xLocal_2 + xLocal_1;
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x270f516a)) {
        pthread_mutex_lock(&(rb_state13.lock_14));
        rb_state13.var_2 = rb_state13.var_1;
        pthread_mutex_unlock(&(rb_state13.lock_14));
    }
    if ((rb_state13.var_0) == (0x270f516a)) {
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_8) == (0x72c5c702)) {
        rb_state18.var_37 = (rb_state18.var_37) - (((((0xd134398e) - (rb_state18.var_20)) + (0xa49320e3)) - (0x261f37ab)) ^ (rb_state18.var_19));
        rb_state18.var_38 = (rb_state18.var_38) - (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_21) : (0xfdf7f096));
        rb_state18.var_39 = (rb_state18.var_39) - (((0xa5ed5bb3) ^ (rb_state18.var_23)) ^ (rb_state18.var_37));
        rb_state18.var_40 = (rb_state18.var_40) ^ (rb_state18.var_38);
        rb_state18.var_41 = (rb_state18.var_41) ^ (((((0xb98833b2) ^ (0xfb592c85)) + (rb_state18.var_24)) - (rb_state18.var_39)) + (rb_state18.var_40));
        rb_state18.var_17 = (rb_state18.var_17) - (rb_state18.var_41);
    }
    #endif
    xLocal = xLocal_1 * xNPrimeofX;
    xLocal = 1.0 - xLocal;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_8) == (0x82587eba)) {
        pthread_mutex_lock(&(rb_state0.lock_30));
        rb_state0.var_25 = 0x8097e33d;
        rb_state0.var_26 = 0x4dce1480;
        rb_state0.var_27 = (rb_state0.var_27) + (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_26) : (0x8a455e));
        rb_state0.var_28 = (rb_state0.var_28) + (((((0x97df7489) ^ (rb_state0.var_25)) ^ (rb_state0.var_17)) ^ (rb_state0.var_19)) + (0x34e0bb7c));
        rb_state0.var_29 = (rb_state0.var_29) + (((((0xcaa4a159) - (rb_state0.var_28)) + (rb_state0.var_20)) + (rb_state0.var_27)) + (rb_state0.var_21));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_29);
        pthread_mutex_unlock(&(rb_state0.lock_30));
    }
    #endif
    OutputX = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x5207156b)) {
        if ((rb_state1.var_1) != (0x0)) {
            if (!((rb_state1.var_1) == (rb_state1.var_23))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0x899f4c0c);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x270f516a)) {
        rb_state13.var_3 = 0xb684813c;
        rb_state13.var_4 = 0x9b377a99;
        rb_state13.var_5 = (rb_state13.var_5) + (rb_state13.var_3);
        rb_state13.var_6 = (rb_state13.var_6) - (0x88844085);
        rb_state13.var_7 = (rb_state13.var_7) - (((((0x19136ec6) ^ (rb_state13.var_3)) + (0x6d662fd4)) ^ (rb_state13.var_2)) + (rb_state13.var_4));
        rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_4);
        rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_6) : (0x3a88502e));
        rb_state13.var_10 = (rb_state13.var_10) ^ (rb_state13.var_7);
        rb_state13.var_11 = (rb_state13.var_11) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_8) : (0xd477a3a1));
        rb_state13.var_12 = (rb_state13.var_12) + (((((0xaf0d2e2b) - (rb_state13.var_10)) + (rb_state13.var_9)) ^ (rb_state13.var_7)) ^ (rb_state13.var_6));
        rb_state13.var_13 = (rb_state13.var_13) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_12) : (0x97c50c0e));
        rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_13);
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

    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_2 = rb_state16.var_1;
    }
    #endif
    xStockPrice = sptprice;
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xa0f37082)) {
        pthread_mutex_lock(&(rb_state7.lock_14));
        rb_state7.var_3 = 0x4d789e9f;
        rb_state7.var_4 = 0x11da2518;
        rb_state7.var_5 = (rb_state7.var_5) + (((0xb336955c) - (rb_state7.var_2)) - (rb_state7.var_4));
        rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_3) : (0x686ef564));
        rb_state7.var_7 = (rb_state7.var_7) ^ (rb_state7.var_1);
        rb_state7.var_8 = (rb_state7.var_8) ^ (((0x7028e635) - (rb_state7.var_5)) + (rb_state7.var_4));
        rb_state7.var_9 = (rb_state7.var_9) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_7) : (0xf2145bff));
        rb_state7.var_10 = (rb_state7.var_10) ^ (((((0x715f5792) - (rb_state7.var_5)) ^ (rb_state7.var_8)) + (rb_state7.var_9)) + (0xeb5def9e));
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_10);
        rb_state7.var_1 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_14));
    }
    if ((rb_state7.var_0) == (0xa0f37082)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_14 = !((rb_state16.var_1) == (rb_state16.var_2));
    }
    #endif
    xStrikePrice = strike;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (0xee80cd8b);
    #endif
    xRiskFreeRate = rate;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x595bd178)) {
        rb_state2.var_20 = 0x1ccc1647;
        rb_state2.var_21 = 0x21379792;
        rb_state2.var_22 = (rb_state2.var_22) - (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_21) : (0x376b4226));
        rb_state2.var_23 = (rb_state2.var_23) + (((0xc18fca24) ^ (rb_state2.var_10)) + (rb_state2.var_9));
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0xc21a233f));
        rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_24);
        rb_state2.var_1 = rb_state2.var_19;
    }
    if ((rb_state2.var_0) == (0x595bd1af)) {
        pthread_mutex_lock(&(rb_state2.lock_30));
        rb_state2.var_26 = 0x4ae034a7;
        rb_state2.var_27 = (rb_state2.var_27) + (((0xdeeb74f) + (rb_state2.var_11)) - (rb_state2.var_13));
        rb_state2.var_28 = (rb_state2.var_28) + (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_12) : (0xb3a673b5));
        rb_state2.var_29 = (rb_state2.var_29) ^ (((((0x73b68127) ^ (rb_state2.var_27)) - (0x12d87d38)) + (rb_state2.var_14)) + (rb_state2.var_28));
        rb_state2.var_25 = (rb_state2.var_25) ^ (rb_state2.var_29);
        rb_state2.var_1 = rb_state2.var_25;
        pthread_mutex_unlock(&(rb_state2.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_9) == (0xf198ec2d)) {
        rb_state16.var_16 = 0x3683e5bf;
        rb_state16.var_17 = 0xde2c2b42;
        rb_state16.var_18 = (rb_state16.var_18) + (((((0x28812f20) - (rb_state16.var_16)) - (0x9bc44e6d)) - (rb_state16.var_12)) - (rb_state16.var_11));
        rb_state16.var_19 = (rb_state16.var_19) ^ ((0x11cd5909) ^ (0xff98f34b));
        rb_state16.var_20 = (rb_state16.var_20) - (((rb_state16.var_17) == (0xde2c2b42)) ? (rb_state16.var_18) : (0x4ac1a8f8));
        rb_state16.var_21 = (rb_state16.var_21) + (((0xdb8b4634) - (rb_state16.var_19)) - (0x8fb3fa89));
        rb_state16.var_22 = (rb_state16.var_22) + (((rb_state16.var_14) == (0x1)) ? (rb_state16.var_20) : (0x69a130da));
        rb_state16.var_23 = (rb_state16.var_23) - (((0xc4f61386) + (0xe39874e9)) ^ (rb_state16.var_21));
        rb_state16.var_24 = (rb_state16.var_24) - (((((0x2be0ee5d) - (0xcff2c83c)) + (rb_state16.var_15)) + (rb_state16.var_23)) + (rb_state16.var_22));
        rb_state16.var_10 = (rb_state16.var_10) ^ (rb_state16.var_24);
    }
    #endif
    xVolatility = volatility;

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (46 < rb_input_size ? (uint32_t)rb_input[46] : 0x3a53206e);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x5b3fd3c8)) {
        pthread_mutex_lock(&(rb_state14.lock_20));
        rb_state14.var_3 = 0xa594bc67;
        rb_state14.var_4 = 0xe4cf54f7;
        rb_state14.var_5 = (rb_state14.var_5) + ((0xfd8c06b0) + (rb_state14.var_2));
        rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_1) : (0x1f676cd9));
        rb_state14.var_7 = (rb_state14.var_7) + (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_3) : (0x4703c261));
        rb_state14.var_8 = (rb_state14.var_8) ^ (((0x42a1557e) ^ (rb_state14.var_4)) ^ (rb_state14.var_5));
        rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_5) : (0x7cf6a75a));
        rb_state14.var_10 = (rb_state14.var_10) - (((((0x6b90e76) ^ (rb_state14.var_7)) + (rb_state14.var_7)) - (0x947e4a85)) - (rb_state14.var_6));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_9) : (0x2306bc4e));
        rb_state14.var_12 = (rb_state14.var_12) + (((((0x20aef3df) ^ (0x39b5c80c)) ^ (rb_state14.var_10)) ^ (rb_state14.var_11)) + (0xb1c941fc));
        rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_12);
        rb_state14.var_1 = rb_state14.var_2;
        pthread_mutex_unlock(&(rb_state14.lock_20));
    }
    if ((rb_state14.var_0) == (0x5b3fd3c8)) {
        pthread_mutex_lock(&(rb_state14.lock_20));
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
        pthread_mutex_unlock(&(rb_state14.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x502087cf)) {
        rb_state19.var_22 = 0xfc1cc6de;
        rb_state19.var_23 = (rb_state19.var_23) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_22) : (0x9830586e));
        rb_state19.var_24 = (rb_state19.var_24) + (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_14) : (0x8c470f16));
        rb_state19.var_25 = (rb_state19.var_25) ^ (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_23) : (0x64860a87));
        rb_state19.var_26 = (rb_state19.var_26) + (((0x24efc19e) + (0x690f4923)) ^ (rb_state19.var_24));
        rb_state19.var_27 = (rb_state19.var_27) + (rb_state19.var_25);
        rb_state19.var_28 = (rb_state19.var_28) ^ (rb_state19.var_26);
        rb_state19.var_29 = (rb_state19.var_29) ^ (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_27) : (0x996dbe0c));
        rb_state19.var_30 = (rb_state19.var_30) ^ (((rb_state19.var_28) == (0x0)) ? (rb_state19.var_29) : (0x7ee998f1));
        rb_state19.var_21 = (rb_state19.var_21) ^ (rb_state19.var_30);
        rb_state19.var_1 = rb_state19.var_21;
    }
    if ((rb_state19.var_0) == (0x502087cf)) {
        pthread_mutex_lock(&(rb_state19.lock_48));
        rb_state19.var_32 = 0x23551628;
        rb_state19.var_33 = 0xccbb36f9;
        rb_state19.var_34 = (rb_state19.var_34) + (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_18) : (0x660a8b4d));
        rb_state19.var_35 = (rb_state19.var_35) ^ (0x856d5360);
        rb_state19.var_36 = (rb_state19.var_36) - (((rb_state19.var_20) == (0x0)) ? (rb_state19.var_32) : (0x32817c94));
        rb_state19.var_37 = (rb_state19.var_37) + (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_17) : (0x1be16d9a));
        rb_state19.var_38 = (rb_state19.var_38) + (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0xbe76af74));
        rb_state19.var_39 = (rb_state19.var_39) ^ (((0x7f96af36) + (0xc5d21f8c)) - (rb_state19.var_35));
        rb_state19.var_40 = (rb_state19.var_40) ^ (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_36) : (0xaac67d29));
        rb_state19.var_41 = (rb_state19.var_41) ^ (((rb_state19.var_37) == (0x0)) ? (rb_state19.var_38) : (0x7729e844));
        rb_state19.var_42 = (rb_state19.var_42) ^ (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_39) : (0x4b089624));
        rb_state19.var_43 = (rb_state19.var_43) + (((((0x36c5695d) ^ (rb_state19.var_41)) ^ (rb_state19.var_24)) + (rb_state19.var_25)) - (rb_state19.var_40));
        rb_state19.var_44 = (rb_state19.var_44) - (rb_state19.var_42);
        rb_state19.var_45 = (rb_state19.var_45) + (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_43) : (0xa24044a7));
        rb_state19.var_46 = (rb_state19.var_46) - (((rb_state19.var_27) == (0x0)) ? (rb_state19.var_44) : (0xeabde904));
        rb_state19.var_47 = (rb_state19.var_47) + (((rb_state19.var_45) == (0x0)) ? (rb_state19.var_46) : (0x3fd26256));
        rb_state19.var_31 = (rb_state19.var_31) ^ (rb_state19.var_47);
        rb_state19.var_1 = rb_state19.var_31;
        pthread_mutex_unlock(&(rb_state19.lock_48));
    }
    #endif
    xTime = time;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x5405f95c)) {
        pthread_mutex_lock(&(rb_state11.lock_16));
        rb_state11.var_2 = rb_state11.var_1;
        pthread_mutex_unlock(&(rb_state11.lock_16));
    }
    if ((rb_state11.var_0) == (0x5405f95c)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    xSqrtTime = sqrt(xTime);

    logValues = log(sptprice / strike);

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_8) == (0x82587eba)) {
        pthread_mutex_lock(&(rb_state0.lock_30));
        rb_state0.var_16 = 0xe969de36;
        rb_state0.var_17 = 0x1a72cf64;
        rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_10) : (0x6d8f685a));
        rb_state0.var_19 = (rb_state0.var_19) + (((((0xa3179b9b) + (0x8dca40db)) + (0x31e65fa2)) ^ (rb_state0.var_16)) ^ (rb_state0.var_17));
        rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_9) : (0xb636c20));
        rb_state0.var_21 = (rb_state0.var_21) ^ (((0x6867f0b1) - (rb_state0.var_18)) - (rb_state0.var_14));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_20) : (0xbff7ee04));
        rb_state0.var_23 = (rb_state0.var_23) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_21) : (0x9fd68a7b));
        rb_state0.var_24 = (rb_state0.var_24) - (((((0x8b910717) + (rb_state0.var_16)) ^ (rb_state0.var_22)) ^ (0x9b52f274)) + (rb_state0.var_23));
        rb_state0.var_15 = (rb_state0.var_15) + (rb_state0.var_24);
        rb_state0.var_14 = rb_state0.var_15;
        pthread_mutex_unlock(&(rb_state0.lock_30));
    }
    if ((rb_state0.var_8) == (0x82587eba)) {
        pthread_mutex_lock(&(rb_state0.lock_30));
        if (!((rb_state0.var_14) == (rb_state0.var_15))) {
            racebench_trigger(0);
        }
        pthread_mutex_unlock(&(rb_state0.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xf5fe1d38)) {
        rb_state6.var_6 = 0x39a333a;
        rb_state6.var_7 = 0x94f415ff;
        rb_state6.var_8 = (rb_state6.var_8) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_7) : (0x7b17c119));
        rb_state6.var_9 = (rb_state6.var_9) + (((0x836841bf) ^ (0x299c993)) - (rb_state6.var_6));
        rb_state6.var_10 = (rb_state6.var_10) - (rb_state6.var_8);
        rb_state6.var_11 = (rb_state6.var_11) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0xbba65f19));
        rb_state6.var_5 = (rb_state6.var_5) + (rb_state6.var_11);
        rb_state6.var_1 = rb_state6.var_5;
    }
    if ((rb_state6.var_0) == (0xf5fe1d38)) {
        pthread_mutex_lock(&(rb_state6.lock_26));
        rb_state6.var_13 = 0x190f2bc5;
        rb_state6.var_14 = (rb_state6.var_14) ^ (((((0x3e7ec9a3) ^ (rb_state6.var_13)) - (0x6fad52ba)) - (rb_state6.var_5)) - (rb_state6.var_6));
        rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_4);
        rb_state6.var_16 = (rb_state6.var_16) - ((0xf3b1d64d) - (rb_state6.var_7));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((0xe96dfd11) ^ (rb_state6.var_14)) ^ (0x9bf9e296));
        rb_state6.var_18 = (rb_state6.var_18) ^ (((0xb703ab07) + (rb_state6.var_15)) ^ (rb_state6.var_8));
        rb_state6.var_19 = (rb_state6.var_19) - (rb_state6.var_16);
        rb_state6.var_20 = (rb_state6.var_20) - (rb_state6.var_17);
        rb_state6.var_21 = (rb_state6.var_21) - (rb_state6.var_18);
        rb_state6.var_22 = (rb_state6.var_22) ^ (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0x3593f006));
        rb_state6.var_23 = (rb_state6.var_23) - (rb_state6.var_21);
        rb_state6.var_24 = (rb_state6.var_24) + (rb_state6.var_22);
        rb_state6.var_25 = (rb_state6.var_25) - (((rb_state6.var_23) == (0x0)) ? (rb_state6.var_24) : (0xd6d33767));
        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_25);
        rb_state6.var_1 = rb_state6.var_12;
        pthread_mutex_unlock(&(rb_state6.lock_26));
    }
    #endif
    xLogTerm = logValues;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6feadd8c)) {
        if ((rb_state0.var_3) != (0x0)) {
            rb_state0.var_13 = !((rb_state0.var_1) == (rb_state0.var_2));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x48c78832));
    #endif
    xPowerTerm = xVolatility * xVolatility;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_13) == (0xac665a56)) {
        rb_state10.var_25 = rb_state10.var_15;
    }
    if ((rb_state10.var_13) == (0xac665a56)) {
        pthread_mutex_lock(&(rb_state10.lock_32));
        if (!((rb_state10.var_15) == (rb_state10.var_25))) {
            racebench_trigger(10);
        }
        pthread_mutex_unlock(&(rb_state10.lock_32));
    }
    #endif
    xPowerTerm = xPowerTerm * 0.5;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (162 < rb_input_size ? (uint32_t)rb_input[162] : 0xfb2c0f62);
    if ((rb_state3.var_0) == (0xffffc130)) {
        pthread_mutex_lock(&(rb_state3.lock_27));
        rb_state3.var_3 = 0xb2ac54cb;
        rb_state3.var_4 = (rb_state3.var_4) ^ (((0xf9b2a4b9) + (rb_state3.var_1)) - (rb_state3.var_3));
        rb_state3.var_5 = (rb_state3.var_5) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x8875bcd3));
        rb_state3.var_6 = (rb_state3.var_6) - (rb_state3.var_4);
        rb_state3.var_7 = (rb_state3.var_7) + (((0x2a5ff105) ^ (rb_state3.var_5)) ^ (rb_state3.var_4));
        rb_state3.var_8 = (rb_state3.var_8) + (((0xfd416008) + (0x7d0e779)) ^ (rb_state3.var_6));
        rb_state3.var_9 = (rb_state3.var_9) ^ (rb_state3.var_7);
        rb_state3.var_10 = (rb_state3.var_10) ^ (((((0xed1d90a9) - (0xc60c1091)) + (0xac4e1e95)) - (rb_state3.var_9)) - (rb_state3.var_8));
        rb_state3.var_2 = (rb_state3.var_2) - (rb_state3.var_10);
        rb_state3.var_1 = rb_state3.var_2;
        pthread_mutex_unlock(&(rb_state3.lock_27));
    }
    if ((rb_state3.var_0) == (0xffffc130)) {
        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_15 = (rb_state5.var_15) ^ (0xb2bcdf7);
    #endif
    xD1 = xRiskFreeRate + xPowerTerm;
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_6) == (0x3d13a032)) {
        rb_state4.var_13 = 0x2250cd5b;
        rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_9) : (0x633d8c01));
        rb_state4.var_15 = (rb_state4.var_15) - (243 < rb_input_size ? (uint32_t)rb_input[243] : 0x9a598e70);
        rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_8) : (0x146d321c));
        rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_14) : (0x933ae543));
        rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_15) : (0xabd66282));
        rb_state4.var_19 = (rb_state4.var_19) + (((0xbbef2bee) + (0x20005ee0)) - (rb_state4.var_16));
        rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_17) : (0x76717519));
        rb_state4.var_21 = (rb_state4.var_21) + (((((0x4b634304) ^ (rb_state4.var_18)) + (0x18e82a58)) + (rb_state4.var_19)) ^ (rb_state4.var_15));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((((0x5e3ee113) ^ (rb_state4.var_16)) + (0x647b5edf)) + (rb_state4.var_20)) + (rb_state4.var_21));
        rb_state4.var_7 = (rb_state4.var_7) ^ (rb_state4.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + ((0x6381a5b3) + (0xe7fd69a4));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0x5734df16) ^ (0x936b3442));
    #endif
    xD1 = xD1 * xTime;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (0x37855474);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x8ca)) {
        rb_state8.var_3 = 0x96f6aafb;
        rb_state8.var_4 = 0x5a66831e;
        rb_state8.var_5 = (rb_state8.var_5) ^ (((0xb883af48) ^ (rb_state8.var_3)) - (rb_state8.var_3));
        rb_state8.var_6 = (rb_state8.var_6) - (0x8ee11af8);
        rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_4) : (0x7a3e9cde));
        rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_5);
        rb_state8.var_9 = (rb_state8.var_9) - (((0x2762b85f) - (0x1f58850d)) + (rb_state8.var_6));
        rb_state8.var_10 = (rb_state8.var_10) + (rb_state8.var_7);
        rb_state8.var_11 = (rb_state8.var_11) - (((0xee4facf4) - (rb_state8.var_8)) ^ (rb_state8.var_4));
        rb_state8.var_12 = (rb_state8.var_12) - (((((0xaceca2b4) - (0xbae4e8a9)) + (rb_state8.var_9)) ^ (rb_state8.var_10)) ^ (0x7ddb7fa1));
        rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_11) : (0x95c9b71b));
        rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_13) : (0x83654ca3));
        rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_14);
    }
    #endif
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (132 < rb_input_size ? (uint32_t)rb_input[132] : 0x2d18ec5f);
    #endif
    xD1 = xD1 / xDen;
    xD2 = xD1 - xDen;

    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xb4ba83df)) {
        rb_state5.var_27 = (rb_state5.var_27) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_8) : (0x481dae90));
        rb_state5.var_28 = (rb_state5.var_28) - (rb_state5.var_12);
        rb_state5.var_29 = (rb_state5.var_29) + (49 < rb_input_size ? (uint32_t)rb_input[49] : 0xc678371);
        rb_state5.var_30 = (rb_state5.var_30) ^ (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_10) : (0x621a3cdd));
        rb_state5.var_31 = (rb_state5.var_31) - (((rb_state5.var_27) == (0x0)) ? (rb_state5.var_28) : (0xe631240a));
        rb_state5.var_32 = (rb_state5.var_32) - (((rb_state5.var_29) == (0x0)) ? (rb_state5.var_30) : (0xa8a9db6));
        rb_state5.var_33 = (rb_state5.var_33) + (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_31) : (0x8692f3af));
        rb_state5.var_34 = (rb_state5.var_34) ^ (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_32) : (0xf3d2ece0));
        rb_state5.var_35 = (rb_state5.var_35) ^ (((((0xd5ef220a) ^ (rb_state5.var_33)) ^ (rb_state5.var_34)) ^ (0x231dc4fb)) + (0x831d90b7));
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_35);
    }
    #endif
    d1 = xD1;
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (0x9ccc8f1b);
    #endif
    d2 = xD2;

    NofXd1 = CNDF(d1);
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x5e1af89)) {
        pthread_mutex_lock(&(rb_state18.lock_15));
        rb_state18.var_3 = 0x87d6a168;
        rb_state18.var_4 = 0xfdfbaff4;
        rb_state18.var_5 = (rb_state18.var_5) ^ (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_1) : (0x83c41d50));
        rb_state18.var_6 = (rb_state18.var_6) ^ (156 < rb_input_size ? (uint32_t)rb_input[156] : 0x2fdc5c72);
        rb_state18.var_7 = (rb_state18.var_7) - (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_5) : (0x28eeaf91));
        rb_state18.var_8 = (rb_state18.var_8) - (((0x8e8dcfff) ^ (0x3b7f732)) + (rb_state18.var_6));
        rb_state18.var_9 = (rb_state18.var_9) + (((0x9737cb60) + (rb_state18.var_7)) + (rb_state18.var_2));
        rb_state18.var_10 = (rb_state18.var_10) + (((0x4d0efc7e) + (0x863f649d)) - (rb_state18.var_8));
        rb_state18.var_11 = (rb_state18.var_11) + (((0x6e0d6e65) - (rb_state18.var_9)) ^ (0x56541e3d));
        rb_state18.var_12 = (rb_state18.var_12) ^ (((0x92ffa7de) - (rb_state18.var_10)) + (rb_state18.var_3));
        rb_state18.var_13 = (rb_state18.var_13) ^ (((0xda8d43b8) + (0x7a0d38b5)) + (rb_state18.var_11));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((((0x880468c0) + (rb_state18.var_12)) - (rb_state18.var_4)) ^ (0xde00c898)) - (rb_state18.var_13));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_14);
        rb_state18.var_2 = rb_state18.var_1;
        pthread_mutex_unlock(&(rb_state18.lock_15));
    }
    #endif
    NofXd2 = CNDF(d2);

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_6) == (0x3d13a032)) {
        pthread_mutex_lock(&(rb_state4.lock_23));
        rb_state4.var_12 = rb_state4.var_7;
        pthread_mutex_unlock(&(rb_state4.lock_23));
    }
    if ((rb_state4.var_6) == (0x3d13a032)) {
        if (!((rb_state4.var_7) == (rb_state4.var_12))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x114)) ? (rb_state11.var_0) : (0xaa6797a0));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x5b3fd3c8)) {
        rb_state14.var_13 = 0xdb2fff5;
        rb_state14.var_14 = (rb_state14.var_14) ^ (rb_state14.var_8);
        rb_state14.var_15 = (rb_state14.var_15) - (rb_state14.var_10);
        rb_state14.var_16 = (rb_state14.var_16) ^ (rb_state14.var_9);
        rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_14) : (0x7e91a472));
        rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_16) : (0x7f24ed0));
        rb_state14.var_19 = (rb_state14.var_19) + (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_18) : (0x6fcdb2a8));
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_19);
    }
    #endif
    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (220 < rb_input_size ? (uint32_t)rb_input[220] : 0x6cadcbf3);
        #endif
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        NegNofXd1 = (1.0 - NofXd1);
        NegNofXd2 = (1.0 - NofXd2);
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ ((0xebb871c8) + (0xb654e2b4));
        #endif
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_5);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x8ca)) {
        rb_state8.var_2 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0x8ca)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
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
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
    #endif
    int end = start + (numOptions / nThreads);

    for (j = 0; j < NUM_RUNS; j++) {
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, price, priceDelta)
        for (i = 0; i < numOptions; i++) {
#else
        for (i = start; i < end; i++) {
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
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (0xe3fcaeb5);
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
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0x0)) {
                rb_state16.var_3 = 0xdd9dd054;
                rb_state16.var_4 = 0x1fd9c8d0;
                rb_state16.var_5 = (rb_state16.var_5) ^ (rb_state16.var_2);
                rb_state16.var_6 = (rb_state16.var_6) ^ (((rb_state16.var_3) == (0xdd9dd054)) ? (rb_state16.var_4) : (0x75f6cdaf));
                rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_3) == (0xdd9dd054)) ? (rb_state16.var_5) : (0x6a7ef871));
                rb_state16.var_8 = (rb_state16.var_8) + (((0xaed3eb0e) ^ (rb_state16.var_6)) ^ (rb_state16.var_4));
                rb_state16.var_9 = (rb_state16.var_9) - (((0xbc64e5b1) ^ (rb_state16.var_7)) ^ (0xf3cffaa7));
                rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_8) : (0xf7cadf8c));
                rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_6) == (0x1fd9c8d0)) ? (rb_state16.var_9) : (0xa552eb4d));
                rb_state16.var_12 = (rb_state16.var_12) ^ (((rb_state16.var_10) == (0xaed3eb0e)) ? (rb_state16.var_11) : (0x2b7d79d4));
                rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_12);
            }
            rb_state16.var_7 = (rb_state16.var_7) - ((0xbdc2ff63) - (rb_state16.var_8));
            if ((rb_state16.var_9) == (0xf198ec2d)) {
                rb_state16.var_15 = rb_state16.var_10;
            }
            if ((rb_state16.var_9) == (0xf198ec2d)) {
                pthread_mutex_lock(&(rb_state16.lock_25));
                if (!((rb_state16.var_10) == (rb_state16.var_15))) {
                    racebench_trigger(16);
                }
                pthread_mutex_unlock(&(rb_state16.lock_25));
            }
            #endif
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