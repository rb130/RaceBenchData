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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ (0x678538fd);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + ((0x8db7642c) + (rb_state10.var_2));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) ^ (((rb_state15.var_1) == (0x7b1843ed)) ? (rb_state15.var_0) : (0xe06def0));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_5) == (0x76f9dedf)) {
            pthread_mutex_lock(&(rb_state17.lock_47));
            rb_state17.var_31 = (rb_state17.var_31) ^ (((0x5328d186) + (0xe7c93656)) + (rb_state17.var_22));
            rb_state17.var_32 = (rb_state17.var_32) - ((0xf335162b) - (0x5163660));
            rb_state17.var_33 = (rb_state17.var_33) ^ (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_23) : (0x6022e6e4));
            rb_state17.var_34 = (rb_state17.var_34) + (((rb_state17.var_26) == (0x0)) ? (rb_state17.var_24) : (0x40583a81));
            rb_state17.var_35 = (rb_state17.var_35) + (rb_state17.var_31);
            rb_state17.var_36 = (rb_state17.var_36) + (((0x94d87549) ^ (rb_state17.var_32)) + (rb_state17.var_27));
            rb_state17.var_37 = (rb_state17.var_37) - (((((0x123ec039) + (rb_state17.var_34)) + (0x67199b3c)) ^ (0xd69d8354)) - (rb_state17.var_33));
            rb_state17.var_38 = (rb_state17.var_38) ^ (((rb_state17.var_28) == (0x0)) ? (rb_state17.var_35) : (0xcf4900b6));
            rb_state17.var_39 = (rb_state17.var_39) - (rb_state17.var_36);
            rb_state17.var_40 = (rb_state17.var_40) - (((((0x51f77b53) - (rb_state17.var_38)) ^ (rb_state17.var_30)) - (rb_state17.var_37)) + (rb_state17.var_29));
            rb_state17.var_41 = (rb_state17.var_41) + (((rb_state17.var_39) == (0x0)) ? (rb_state17.var_40) : (0xa5df52d6));
            rb_state17.var_30 = (rb_state17.var_30) - (rb_state17.var_41);
            rb_state17.var_17 = rb_state17.var_30;
            pthread_mutex_unlock(&(rb_state17.lock_47));
        }
        if ((rb_state17.var_5) == (0x76f9dedf)) {
            pthread_mutex_lock(&(rb_state17.lock_47));
            rb_state17.var_43 = 0xf39a14fd;
            rb_state17.var_44 = 0xa3704467;
            rb_state17.var_45 = (rb_state17.var_45) + (((((0x7fdeb7d4) + (rb_state17.var_31)) ^ (rb_state17.var_43)) - (rb_state17.var_32)) + (rb_state17.var_33));
            rb_state17.var_46 = (rb_state17.var_46) ^ (((rb_state17.var_44) == (0x0)) ? (rb_state17.var_45) : (0xcccca137));
            rb_state17.var_42 = (rb_state17.var_42) - (rb_state17.var_46);
            rb_state17.var_17 = rb_state17.var_42;
            pthread_mutex_unlock(&(rb_state17.lock_47));
        }
        #endif
        sign = 0;
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_1) : (0xb2e36e87));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (1 < rb_input_size ? (uint32_t)rb_input[1] : 0xc2eb33f6);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ ((0x16363ef2) - (0x63d4f220));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_2);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x1094682f)) {
        pthread_mutex_lock(&(rb_state9.lock_38));
        rb_state9.var_9 = 0x4812e61e;
        rb_state9.var_10 = 0xc5ab079a;
        rb_state9.var_11 = (rb_state9.var_11) ^ (((0x626639e7) + (rb_state9.var_10)) + (rb_state9.var_10));
        rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_11);
        rb_state9.var_13 = (rb_state9.var_13) + (rb_state9.var_9);
        rb_state9.var_14 = (rb_state9.var_14) - (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_11) : (0xc4fa0a0b));
        rb_state9.var_15 = (rb_state9.var_15) + (((0x8376b6ad) ^ (0xbbb68d47)) ^ (rb_state9.var_12));
        rb_state9.var_16 = (rb_state9.var_16) ^ (rb_state9.var_13);
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_14) : (0x13e85c60));
        rb_state9.var_18 = (rb_state9.var_18) + (((0xdbaadc8e) ^ (0x2ee5bd13)) ^ (rb_state9.var_15));
        rb_state9.var_19 = (rb_state9.var_19) ^ (rb_state9.var_16);
        rb_state9.var_20 = (rb_state9.var_20) - (((0x29e6ae6f) ^ (rb_state9.var_14)) ^ (rb_state9.var_17));
        rb_state9.var_21 = (rb_state9.var_21) + (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_18) : (0xed76023b));
        rb_state9.var_22 = (rb_state9.var_22) + (rb_state9.var_19);
        rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_20) : (0xb6d67ae9));
        rb_state9.var_24 = (rb_state9.var_24) + (((0xb8dc9d2) ^ (rb_state9.var_17)) + (rb_state9.var_21));
        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_22) : (0xdf81f128));
        rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_23) : (0xfb40103e));
        rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_24) : (0xee17c0e5));
        rb_state9.var_28 = (rb_state9.var_28) ^ (((0x47b452f9) - (rb_state9.var_25)) + (0x2e2b331));
        rb_state9.var_29 = (rb_state9.var_29) - (((0x9de6d79a) - (rb_state9.var_26)) ^ (rb_state9.var_21));
        rb_state9.var_30 = (rb_state9.var_30) ^ (((rb_state9.var_22) == (0x0)) ? (rb_state9.var_27) : (0x2cb9c845));
        rb_state9.var_31 = (rb_state9.var_31) - (((((0x7932b055) ^ (rb_state9.var_23)) ^ (rb_state9.var_28)) + (0x4baf2f6b)) ^ (rb_state9.var_29));
        rb_state9.var_32 = (rb_state9.var_32) + (rb_state9.var_30);
        rb_state9.var_33 = (rb_state9.var_33) + (((0x41639b9a) - (0x266a981a)) - (rb_state9.var_31));
        rb_state9.var_34 = (rb_state9.var_34) + (((0x60ffcd4b) - (rb_state9.var_32)) ^ (rb_state9.var_24));
        rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_33) : (0x1f58c52b));
        rb_state9.var_36 = (rb_state9.var_36) + (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_34) : (0x2608a7bd));
        rb_state9.var_37 = (rb_state9.var_37) - (((rb_state9.var_35) == (0x0)) ? (rb_state9.var_36) : (0x68200483));
        rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_37);
        pthread_mutex_unlock(&(rb_state9.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (89 < rb_input_size ? (uint32_t)rb_input[89] : 0x23a2343b);
    if ((rb_state15.var_2) == (0x4fb9bc67)) {
        pthread_mutex_lock(&(rb_state15.lock_10));
        rb_state15.var_4 = rb_state15.var_3;
        pthread_mutex_unlock(&(rb_state15.lock_10));
    }
    if ((rb_state15.var_2) == (0x4fb9bc67)) {
        if (!((rb_state15.var_3) == (rb_state15.var_4))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_0);
    #endif
    xInput = InputX;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0x57ca4b6d));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) ^ ((0x401dda20) + (rb_state4.var_4));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (0x73138fc3);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (75 < rb_input_size ? (uint32_t)rb_input[75] : 0xe7bc743c);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0x7acc66d6) + (rb_state10.var_1));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (0xaf932409);
    #endif
    expValues = exp(-0.5f * InputX * InputX);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) + (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x2cde9cd1));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - ((0xe62db3a0) - (0x374bf570));
    if ((rb_state4.var_0) == (0x1d2)) {
        pthread_mutex_lock(&(rb_state4.lock_19));
        rb_state4.var_7 = 0x1afc0a58;
        rb_state4.var_8 = 0x59972815;
        rb_state4.var_9 = (rb_state4.var_9) - (((((0x6d600e63) ^ (rb_state4.var_8)) - (0x7ae824c4)) ^ (rb_state4.var_5)) ^ (rb_state4.var_6));
        rb_state4.var_10 = (rb_state4.var_10) - (((0x7ff658c6) ^ (rb_state4.var_7)) + (rb_state4.var_7));
        rb_state4.var_11 = (rb_state4.var_11) + (((0x1469d557) ^ (rb_state4.var_9)) ^ (0xe8b04319));
        rb_state4.var_12 = (rb_state4.var_12) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_10) : (0x585fd67f));
        rb_state4.var_13 = (rb_state4.var_13) - (((0x725a78b2) ^ (rb_state4.var_11)) ^ (0x87147268));
        rb_state4.var_14 = (rb_state4.var_14) + (((((0x4c984942) ^ (0xf84e32a9)) + (rb_state4.var_9)) + (rb_state4.var_12)) + (rb_state4.var_13));
        rb_state4.var_6 = (rb_state4.var_6) - (rb_state4.var_14);
        rb_state4.var_5 = rb_state4.var_6;
        pthread_mutex_unlock(&(rb_state4.lock_19));
    }
    if ((rb_state4.var_0) == (0x1d2)) {
        if (!((rb_state4.var_5) == (rb_state4.var_6))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0xffffef80)) {
        rb_state5.var_28 = 0xee8c951;
        rb_state5.var_29 = 0xc8023189;
        rb_state5.var_30 = 0x1fde0fb1;
        rb_state5.var_31 = (rb_state5.var_31) + (((0x13c37b22) - (0x3743fcb0)) - (rb_state5.var_28));
        rb_state5.var_32 = (rb_state5.var_32) - (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_29) : (0x46f6b7c6));
        rb_state5.var_33 = (rb_state5.var_33) - (((rb_state5.var_30) == (0x0)) ? (rb_state5.var_31) : (0x38786ae2));
        rb_state5.var_34 = (rb_state5.var_34) - (((0x2f3f5f71) + (0xc196e837)) + (rb_state5.var_32));
        rb_state5.var_35 = (rb_state5.var_35) + (((rb_state5.var_33) == (0x0)) ? (rb_state5.var_34) : (0x76d77ee0));
        rb_state5.var_27 = (rb_state5.var_27) ^ (rb_state5.var_35);
        rb_state5.var_5 = rb_state5.var_27;
    }
    if ((rb_state5.var_2) == (0xffffef80)) {
        pthread_mutex_lock(&(rb_state5.lock_44));
        rb_state5.var_37 = 0x584054db;
        rb_state5.var_38 = (rb_state5.var_38) + (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_37) : (0x25802ad7));
        rb_state5.var_39 = (rb_state5.var_39) + (rb_state5.var_17);
        rb_state5.var_40 = (rb_state5.var_40) - (((0x831cc3ba) - (rb_state5.var_19)) - (rb_state5.var_16));
        rb_state5.var_41 = (rb_state5.var_41) - (((rb_state5.var_20) == (0x0)) ? (rb_state5.var_38) : (0x68425267));
        rb_state5.var_42 = (rb_state5.var_42) - (((((0x1117c94b) ^ (rb_state5.var_39)) - (0xe018cd92)) + (0x823c4d2f)) - (rb_state5.var_40));
        rb_state5.var_43 = (rb_state5.var_43) - (((rb_state5.var_41) == (0x0)) ? (rb_state5.var_42) : (0xd171565a));
        rb_state5.var_36 = (rb_state5.var_36) - (rb_state5.var_43);
        rb_state5.var_5 = rb_state5.var_36;
        pthread_mutex_unlock(&(rb_state5.lock_44));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + ((0x5e3003d4) - (rb_state6.var_0));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0xda8f5cfd) ^ (0xb22eeafc));
    rb_state7.var_2 = (rb_state7.var_2) - (0xe0e03ab5);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) - (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_4) : (0x3dbd3171));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_2) : (0xbff29e45));
    #endif
    xNPrimeofX = expValues;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - (251 < rb_input_size ? (uint32_t)rb_input[251] : 0x8116c3);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_28 = (rb_state3.var_28) + ((0xa3342f0c) - (0x8238c904));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - (239 < rb_input_size ? (uint32_t)rb_input[239] : 0xb7235117);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) - (0x182e3055);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_4 = (rb_state12.var_4) + (33 < rb_input_size ? (uint32_t)rb_input[33] : 0x6daa37cc);
    if ((rb_state12.var_4) == (0xf88fd54)) {
        rb_state12.var_5 = rb_state12.var_2;
    }
    if ((rb_state12.var_4) == (0xf88fd54)) {
        if (!((rb_state12.var_2) == (rb_state12.var_5))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (0x3412716e);
    #endif
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) + ((0x6ca1d427) + (rb_state7.var_1));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_3);
    #endif
    xK2 = 0.2316419 * xInput;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_0);
    if ((rb_state3.var_2) == (0x90abf012)) {
        if ((rb_state3.var_4) != (0x0)) {
            rb_state3.var_40 = !((rb_state3.var_4) == (rb_state3.var_32));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + ((0x1d468031) ^ (0xb60b2155));
    if ((rb_state13.var_2) == (0xa9dfbfa)) {
        usleep(100);
    }
    if ((rb_state13.var_2) == (0xa9dfbfa)) {
        rb_state13.var_12 = !((rb_state13.var_6) == (rb_state13.var_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x6372bcb9);
    #endif
    xK2 = 1.0 + xK2;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_4 = (rb_state2.var_4) - (80 < rb_input_size ? (uint32_t)rb_input[80] : 0xf6233f51);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - (152 < rb_input_size ? (uint32_t)rb_input[152] : 0x8fa0c7fd);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_4 = (rb_state7.var_4) ^ (0x36fb8da5);
    rb_state7.var_3 = (rb_state7.var_3) + (((rb_state7.var_0) == (0x68a1b601)) ? (rb_state7.var_4) : (0x831b6be6));
    #endif
    xK2 = 1.0 / xK2;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x1ed840e));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_29) == (0x9ada7839)) {
        rb_state3.var_41 = rb_state3.var_30;
    }
    if ((rb_state3.var_29) == (0x9ada7839)) {
        rb_state3.var_48 = !((rb_state3.var_30) == (rb_state3.var_41));
    }
    rb_state3.var_31 = (rb_state3.var_31) + (((rb_state3.var_33) == (0x0)) ? (rb_state3.var_32) : (0xf45fb5a6));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) - (176 < rb_input_size ? (uint32_t)rb_input[176] : 0x555fc1a7);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) + (((rb_state12.var_1) == (0x194)) ? (rb_state12.var_3) : (0x68c7e2f3));
    if ((rb_state12.var_4) == (0xf88fd54)) {
        rb_state12.var_6 = 0x3ac8ecd5;
        rb_state12.var_7 = (rb_state12.var_7) ^ (rb_state12.var_6);
        rb_state12.var_8 = (rb_state12.var_8) + (0x860219af);
        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_5);
        rb_state12.var_10 = (rb_state12.var_10) - (rb_state12.var_6);
        rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_7);
        rb_state12.var_12 = (rb_state12.var_12) - (rb_state12.var_8);
        rb_state12.var_13 = (rb_state12.var_13) ^ (((0xdca4489f) + (0x78664b3e)) - (rb_state12.var_9));
        rb_state12.var_14 = (rb_state12.var_14) + (rb_state12.var_10);
        rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_11) : (0x3b76a5f));
        rb_state12.var_16 = (rb_state12.var_16) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_12) : (0x6bd6848a));
        rb_state12.var_17 = (rb_state12.var_17) ^ (((0x916da35) + (rb_state12.var_13)) - (0xb45425f3));
        rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_14);
        rb_state12.var_19 = (rb_state12.var_19) ^ (((((0xc05e1b65) ^ (rb_state12.var_15)) + (rb_state12.var_10)) + (rb_state12.var_9)) - (rb_state12.var_16));
        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_17) : (0x548e4184));
        rb_state12.var_21 = (rb_state12.var_21) - (((0x5199a404) ^ (rb_state12.var_18)) ^ (rb_state12.var_12));
        rb_state12.var_22 = (rb_state12.var_22) ^ (((((0xb1979266) ^ (rb_state12.var_13)) - (rb_state12.var_20)) ^ (rb_state12.var_19)) + (0xb4f1c1e6));
        rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_21) : (0x3cc05ea));
        rb_state12.var_24 = (rb_state12.var_24) ^ (((0x1b94b66) - (rb_state12.var_15)) + (rb_state12.var_22));
        rb_state12.var_25 = (rb_state12.var_25) - (((((0x1711c460) + (rb_state12.var_17)) - (rb_state12.var_23)) - (rb_state12.var_16)) ^ (rb_state12.var_24));
        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_25);
    }
    #endif
    xK2_2 = xK2 * xK2;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_2) == (0xfffffff6)) ? (rb_state0.var_1) : (0x84bf1f06));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - ((0xf3f63380) ^ (rb_state2.var_2));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ ((0xbaae344) + (0xf72190e9));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + ((0xc9eb3c61) - (0xf2c0807f));
    #endif
    xK2_3 = xK2_2 * xK2;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (21 < rb_input_size ? (uint32_t)rb_input[21] : 0x76a0a982);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (212 < rb_input_size ? (uint32_t)rb_input[212] : 0x434d8b74);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) ^ ((0xb7cda56f) + (0xbbaf219));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) - (((rb_state11.var_1) == (0xffffff61)) ? (rb_state11.var_3) : (0x3cd700d));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_1);
    #endif
    xK2_4 = xK2_3 * xK2;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_34) == (0x239b341f)) {
        pthread_mutex_lock(&(rb_state3.lock_61));
        rb_state3.var_51 = 0xefb82968;
        rb_state3.var_52 = 0x3dde85da;
        rb_state3.var_53 = 0xa7fcdf3;
        rb_state3.var_54 = (rb_state3.var_54) ^ (0x1026abea);
        rb_state3.var_55 = (rb_state3.var_55) ^ (((((0xb33bae6d) + (rb_state3.var_35)) + (rb_state3.var_53)) + (rb_state3.var_51)) ^ (0x6b722bfe));
        rb_state3.var_56 = (rb_state3.var_56) ^ (((((0x15041553) - (0xc807254b)) - (0xada19c68)) + (rb_state3.var_54)) + (rb_state3.var_52));
        rb_state3.var_57 = (rb_state3.var_57) - (((((0x2fc2a055) - (rb_state3.var_55)) + (rb_state3.var_56)) + (0x2c0b2a80)) ^ (0x34b89365));
        rb_state3.var_50 = (rb_state3.var_50) - (rb_state3.var_57);
        rb_state3.var_49 = rb_state3.var_50;
        pthread_mutex_unlock(&(rb_state3.lock_61));
    }
    if ((rb_state3.var_34) == (0x239b341f)) {
        if (!((rb_state3.var_49) == (rb_state3.var_50))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) - (0x1a4a0718);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) ^ ((0xf75d1e0c) - (rb_state8.var_4));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (163 < rb_input_size ? (uint32_t)rb_input[163] : 0x483bce28);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0xab836e50));
    #endif
    xK2_5 = xK2_4 * xK2;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0x19b978a5) - (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (60 < rb_input_size ? (uint32_t)rb_input[60] : 0xd0cdcdef);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_4) == (0x9e2d180)) {
        pthread_mutex_lock(&(rb_state17.lock_15));
        rb_state17.var_7 = 0xe638e7e3;
        rb_state17.var_8 = (rb_state17.var_8) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_6) : (0xb0c26d68));
        rb_state17.var_9 = (rb_state17.var_9) - (((((0xb33749c9) + (rb_state17.var_5)) - (0xb2e0edd7)) - (rb_state17.var_8)) ^ (rb_state17.var_7));
        rb_state17.var_6 = (rb_state17.var_6) + (rb_state17.var_9);
        rb_state17.var_5 = rb_state17.var_6;
        pthread_mutex_unlock(&(rb_state17.lock_15));
    }
    if ((rb_state17.var_4) == (0x9e2d180)) {
        rb_state17.var_16 = !((rb_state17.var_5) == (rb_state17.var_6));
    }
    #endif
    xLocal_1 = xK2 * 0.319381530;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_22 = (rb_state3.var_22) - (((rb_state3.var_24) == (0x0)) ? (rb_state3.var_23) : (0x2ede2329));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) ^ (0x975ab2da);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (83 < rb_input_size ? (uint32_t)rb_input[83] : 0x67850efc);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (176 < rb_input_size ? (uint32_t)rb_input[176] : 0xd37b1885);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) + (250 < rb_input_size ? (uint32_t)rb_input[250] : 0xdb46b37b);
    #endif
    xLocal_2 = xK2_2 * (-0.356563782);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (221 < rb_input_size ? (uint32_t)rb_input[221] : 0xfaea927d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_19);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_2);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xebdbe0c7) + (rb_state10.var_0));
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (0x5733d9d6);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (187 < rb_input_size ? (uint32_t)rb_input[187] : 0x9b5109da);
    rb_state13.var_10 = (rb_state13.var_10) - (0x74954f4a);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_3);
    #endif
    xLocal_3 = xK2_3 * 1.781477937;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
    rb_state4.var_3 = (rb_state4.var_3) - ((0xb49221c2) - (rb_state4.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ (0x485b9274);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0xf8651f7c)) {
        pthread_mutex_lock(&(rb_state11.lock_16));
        rb_state11.var_5 = rb_state11.var_3;
        pthread_mutex_unlock(&(rb_state11.lock_16));
    }
    if ((rb_state11.var_2) == (0xf8651f7c)) {
        pthread_mutex_lock(&(rb_state11.lock_16));
        if (!((rb_state11.var_3) == (rb_state11.var_5))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
    rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_3) : (0x46cff9c9));
    rb_state12.var_4 = (rb_state12.var_4) + (0xa65eaf4e);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_8) == (0x694bccf5)) {
        if ((rb_state13.var_13) != (0x0)) {
            if (!((rb_state13.var_13) == (rb_state13.var_37))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x3a9346e0)) {
        if ((rb_state19.var_4) != (0x0)) {
            if (!((rb_state19.var_4) == (rb_state19.var_27))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_3);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (0xbb3d84ac);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) + ((0x63a44c4a) - (rb_state2.var_3));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (113 < rb_input_size ? (uint32_t)rb_input[113] : 0xe591b925);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_4 = (rb_state7.var_4) ^ (rb_state7.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (0x519cf21b);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) + ((0xec7eb318) + (0xca930101));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_2) == (0xfffffeec)) ? (rb_state17.var_1) : (0x9a177ee7));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (87 < rb_input_size ? (uint32_t)rb_input[87] : 0xcc7d7f8b);
    #endif
    xLocal_3 = xK2_4 * (-1.821255978);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - ((0x25a3f358) + (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_4 = (rb_state2.var_4) - ((0xd55f9f3f) + (rb_state2.var_4));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (142 < rb_input_size ? (uint32_t)rb_input[142] : 0x561a2c43);
    rb_state15.var_3 = (rb_state15.var_3) - ((0x1e1de8fd) ^ (rb_state15.var_2));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x89afb23a)) {
        pthread_mutex_lock(&(rb_state16.lock_42));
        rb_state16.var_5 = 0xfadd10ba;
        rb_state16.var_6 = (rb_state16.var_6) + (((0x12cd599a) + (0x92037384)) + (rb_state16.var_3));
        rb_state16.var_7 = (rb_state16.var_7) ^ ((0x5265784a) + (0x55878905));
        rb_state16.var_8 = (rb_state16.var_8) - ((0x1eae364a) - (0xad994be3));
        rb_state16.var_9 = (rb_state16.var_9) ^ (((0xd8f96869) - (0x16c047a8)) + (rb_state16.var_5));
        rb_state16.var_10 = (rb_state16.var_10) - (((0xf9380ac) + (rb_state16.var_4)) + (rb_state16.var_5));
        rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_6) : (0xb63fefdb));
        rb_state16.var_12 = (rb_state16.var_12) + (((0x641a1e2c) - (rb_state16.var_7)) + (rb_state16.var_7));
        rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_8);
        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0x2ba52160));
        rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0x4600204b));
        rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0xe959942a));
        rb_state16.var_17 = (rb_state16.var_17) + (((((0x51f79e2f) ^ (rb_state16.var_12)) + (rb_state16.var_11)) + (0xb97c0fec)) ^ (rb_state16.var_13));
        rb_state16.var_18 = (rb_state16.var_18) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_14) : (0x1a8fece1));
        rb_state16.var_19 = (rb_state16.var_19) + (rb_state16.var_15);
        rb_state16.var_20 = (rb_state16.var_20) + (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_17) : (0xf6ea1b30));
        rb_state16.var_21 = (rb_state16.var_21) - (rb_state16.var_18);
        rb_state16.var_22 = (rb_state16.var_22) + (rb_state16.var_19);
        rb_state16.var_23 = (rb_state16.var_23) ^ (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_21) : (0x7c68783c));
        rb_state16.var_24 = (rb_state16.var_24) ^ (((((0xd686a21) + (rb_state16.var_23)) + (rb_state16.var_22)) + (0x4e653886)) - (rb_state16.var_13));
        rb_state16.var_4 = (rb_state16.var_4) - (rb_state16.var_24);
        rb_state16.var_3 = rb_state16.var_4;
        pthread_mutex_unlock(&(rb_state16.lock_42));
    }
    if ((rb_state16.var_1) == (0x89afb23a)) {
        pthread_mutex_lock(&(rb_state16.lock_42));
        if (!((rb_state16.var_3) == (rb_state16.var_4))) {
            racebench_trigger(16);
        }
        pthread_mutex_unlock(&(rb_state16.lock_42));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_13 = (rb_state17.var_13) - ((0x49688eec) ^ (0x75e3fe91));
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (161 < rb_input_size ? (uint32_t)rb_input[161] : 0xca6879e9);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x7d8a60ab) + (rb_state2.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (0xb8edc0d6);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_2) : (0xc775d5bf));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_4 = (rb_state17.var_4) - (42 < rb_input_size ? (uint32_t)rb_input[42] : 0xfae41abc);
    #endif
    xLocal_3 = xK2_5 * 1.330274429;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - ((0x4319fff4) + (0x4bc9c477));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_4) : (0xfb7d2c13));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + ((0xf1c23e6f) - (0x2505f4b1));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) + (0xeef7f69b);
    rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_10);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) + (0x56adcbe8);
    #endif
    xLocal_2 = xLocal_2 + xLocal_3;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_20 = (rb_state3.var_20) ^ (rb_state3.var_21);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (99 < rb_input_size ? (uint32_t)rb_input[99] : 0xcad31b49);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0xe85f5396));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (0x12d472c6);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) + (0x74a89084);
    #endif
    xLocal_1 = xLocal_2 + xLocal_1;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x89d8efc)) {
        if ((rb_state1.var_4) != (0x0)) {
            if (!((rb_state1.var_4) == (rb_state1.var_29))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0xa0f2275c)) {
        pthread_mutex_lock(&(rb_state2.lock_24));
        rb_state2.var_7 = 0xe9388582;
        rb_state2.var_8 = 0x4cf3f8a;
        rb_state2.var_9 = (rb_state2.var_9) ^ (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_5) : (0x87c7b225));
        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_6) : (0xe557ca7c));
        rb_state2.var_11 = (rb_state2.var_11) ^ (((((0x628b5b95) ^ (rb_state2.var_8)) ^ (rb_state2.var_9)) + (rb_state2.var_9)) + (rb_state2.var_8));
        rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_10) : (0xc23e1695));
        rb_state2.var_13 = (rb_state2.var_13) ^ (rb_state2.var_11);
        rb_state2.var_14 = (rb_state2.var_14) - (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_12) : (0x1d375707));
        rb_state2.var_15 = (rb_state2.var_15) + (rb_state2.var_13);
        rb_state2.var_16 = (rb_state2.var_16) + (((0x8748e14a) + (rb_state2.var_14)) + (0xcafb4be0));
        rb_state2.var_17 = (rb_state2.var_17) - (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_15) : (0x4b9f5b0a));
        rb_state2.var_18 = (rb_state2.var_18) + (((((0xc8f98c41) + (rb_state2.var_16)) - (rb_state2.var_17)) ^ (0xfae2390)) ^ (rb_state2.var_13));
        rb_state2.var_6 = (rb_state2.var_6) ^ (rb_state2.var_18);
        rb_state2.var_5 = rb_state2.var_6;
        pthread_mutex_unlock(&(rb_state2.lock_24));
    }
    if ((rb_state2.var_2) == (0xa0f2275c)) {
        if (!((rb_state2.var_5) == (rb_state2.var_6))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (118 < rb_input_size ? (uint32_t)rb_input[118] : 0x73cfef21);
    rb_state3.var_3 = (rb_state3.var_3) ^ (0xf59a3f8f);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + ((0x54b1b026) + (rb_state6.var_3));
    if ((rb_state6.var_3) == (0xa82ea8d4)) {
        pthread_mutex_lock(&(rb_state6.lock_23));
        rb_state6.var_6 = 0x4a2e7efe;
        rb_state6.var_7 = 0xc9cbcacb;
        rb_state6.var_8 = (rb_state6.var_8) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_6) : (0x7ed7296));
        rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_7) : (0xdc998013));
        rb_state6.var_10 = (rb_state6.var_10) - (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_9) : (0x4d138a93));
        rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_10);
        rb_state6.var_4 = rb_state6.var_5;
        pthread_mutex_unlock(&(rb_state6.lock_23));
    }
    if ((rb_state6.var_3) == (0xa82ea8d4)) {
        pthread_mutex_lock(&(rb_state6.lock_23));
        if (!((rb_state6.var_4) == (rb_state6.var_5))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0xffffb5c0)) {
        rb_state7.var_5 = rb_state7.var_2;
    }
    if ((rb_state7.var_1) == (0xffffb5c0)) {
        if (!((rb_state7.var_2) == (rb_state7.var_5))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (137 < rb_input_size ? (uint32_t)rb_input[137] : 0xa6c92f52);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0xf8651f7c)) {
        rb_state11.var_6 = (rb_state11.var_6) ^ (rb_state11.var_6);
        rb_state11.var_7 = (rb_state11.var_7) + (((0xa08b587f) - (rb_state11.var_5)) - (rb_state11.var_7));
        rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_8);
        rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_4) : (0xa2a4cb37));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xcce02476));
        rb_state11.var_11 = (rb_state11.var_11) ^ (rb_state11.var_8);
        rb_state11.var_12 = (rb_state11.var_12) + (((0x35aaed84) - (rb_state11.var_9)) ^ (0x14986bd5));
        rb_state11.var_13 = (rb_state11.var_13) + (((0xee479d1f) + (rb_state11.var_10)) ^ (rb_state11.var_10));
        rb_state11.var_14 = (rb_state11.var_14) ^ (((((0x2b72c2b2) - (rb_state11.var_11)) + (rb_state11.var_11)) - (rb_state11.var_12)) ^ (0xb0c5f3e7));
        rb_state11.var_15 = (rb_state11.var_15) + (((((0xc6cbf409) ^ (rb_state11.var_13)) + (rb_state11.var_13)) - (rb_state11.var_12)) + (rb_state11.var_14));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x5afce25f) - (0x1b998db2));
    #endif
    xLocal = xLocal_1 * xNPrimeofX;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_2) == (0x90abf012)) {
        rb_state3.var_24 = 0x6518f077;
        rb_state3.var_25 = 0x5b00031b;
        rb_state3.var_26 = (rb_state3.var_26) + (0x652587c7);
        rb_state3.var_27 = (rb_state3.var_27) ^ (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_24) : (0xc4df3b1c));
        rb_state3.var_28 = (rb_state3.var_28) - (rb_state3.var_25);
        rb_state3.var_29 = (rb_state3.var_29) - (rb_state3.var_26);
        rb_state3.var_30 = (rb_state3.var_30) + (((((0x8eb684cb) - (rb_state3.var_27)) + (0xf3e5cfb0)) + (rb_state3.var_17)) - (rb_state3.var_28));
        rb_state3.var_31 = (rb_state3.var_31) - (((rb_state3.var_29) == (0x0)) ? (rb_state3.var_30) : (0xcbc09e10));
        rb_state3.var_23 = (rb_state3.var_23) ^ (rb_state3.var_31);
        rb_state3.var_4 = rb_state3.var_23;
    }
    if ((rb_state3.var_2) == (0x90abf012)) {
        pthread_mutex_lock(&(rb_state3.lock_39));
        rb_state3.var_33 = 0xc2d93b39;
        rb_state3.var_34 = 0x239b341f;
        rb_state3.var_35 = 0x75f3145b;
        rb_state3.var_36 = (rb_state3.var_36) - (((rb_state3.var_34) == (0x0)) ? (rb_state3.var_33) : (0x6ebd54ed));
        rb_state3.var_37 = (rb_state3.var_37) - (rb_state3.var_35);
        rb_state3.var_38 = (rb_state3.var_38) ^ (((((0xe62d183c) ^ (rb_state3.var_36)) + (0x65e47dd2)) + (0xaef280a)) - (rb_state3.var_37));
        rb_state3.var_32 = (rb_state3.var_32) ^ (rb_state3.var_38);
        rb_state3.var_4 = rb_state3.var_32;
        pthread_mutex_unlock(&(rb_state3.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + ((0x7c40d167) ^ (0x828d88da));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_10 = (rb_state13.var_10) + (0x751e86f3);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0xcd12acdc)) {
        pthread_mutex_lock(&(rb_state14.lock_30));
        if ((rb_state14.var_5) != (0x0)) {
            if (!((rb_state14.var_5) == (rb_state14.var_21))) {
                racebench_trigger(14);
            }
        }
        pthread_mutex_unlock(&(rb_state14.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_2) == (0x4fb9bc67)) {
        pthread_mutex_lock(&(rb_state15.lock_10));
        rb_state15.var_5 = 0x690f84dd;
        rb_state15.var_6 = 0xaf904fc0;
        rb_state15.var_7 = 0xe17a8185;
        rb_state15.var_8 = (rb_state15.var_8) ^ (((((0x7f382c0b) ^ (rb_state15.var_5)) - (0x4791cfe)) + (rb_state15.var_7)) + (rb_state15.var_4));
        rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_8) : (0x5baedca3));
        rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_9);
        pthread_mutex_unlock(&(rb_state15.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_1) == (0x99)) ? (rb_state18.var_0) : (0xbd99efc6));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - (0x4ac59e8f);
    rb_state19.var_0 = (rb_state19.var_0) + ((0x1652d4a1) - (rb_state19.var_1));
    #endif
    xLocal = 1.0 - xLocal;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_2);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x19b9d7d4)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_5 = (rb_state10.var_5) + ((0x12eb177) ^ (rb_state10.var_5));
        rb_state10.var_6 = (rb_state10.var_6) - (((((0xcbaf5fc4) + (rb_state10.var_4)) ^ (0x708860b7)) + (rb_state10.var_3)) - (rb_state10.var_6));
        rb_state10.var_7 = (rb_state10.var_7) + (116 < rb_input_size ? (uint32_t)rb_input[116] : 0x8940e303);
        rb_state10.var_8 = (rb_state10.var_8) + (((((0x5bc6d209) ^ (rb_state10.var_6)) - (rb_state10.var_5)) - (0xbf42c551)) + (rb_state10.var_7));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((0xb1ae79b8) ^ (rb_state10.var_7)) - (rb_state10.var_8));
        rb_state10.var_10 = (rb_state10.var_10) + (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_8) : (0xf145d901));
        rb_state10.var_11 = (rb_state10.var_11) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_9) : (0x4bd2af90));
        rb_state10.var_12 = (rb_state10.var_12) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0x4ad92bc2));
        rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_12);
        rb_state10.var_3 = rb_state10.var_4;
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    if ((rb_state10.var_2) == (0x19b9d7d4)) {
        if (!((rb_state10.var_3) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0xffffffc9)) ? (rb_state16.var_0) : (0x93fa69b7));
    #endif
    OutputX = xLocal;

    if (sign) {
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_2) == (0x19b9d7d4)) {
            rb_state10.var_13 = 0x28ff512f;
            rb_state10.var_14 = (rb_state10.var_14) + ((0xbdd56041) + (0x52134bb));
            rb_state10.var_15 = (rb_state10.var_15) + (rb_state10.var_13);
            rb_state10.var_16 = (rb_state10.var_16) + ((0x373d0e39) ^ (rb_state10.var_14));
            rb_state10.var_17 = (rb_state10.var_17) + (((((0x287b535) - (rb_state10.var_13)) + (rb_state10.var_12)) - (0xa10f99e6)) ^ (0xcbc3ebed));
            rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_11);
            rb_state10.var_19 = (rb_state10.var_19) ^ (((0x70c20020) ^ (rb_state10.var_14)) - (rb_state10.var_15));
            rb_state10.var_20 = (rb_state10.var_20) ^ (((((0xc61aae49) + (rb_state10.var_15)) ^ (0xd25dcdde)) + (0x4948a35a)) ^ (rb_state10.var_16));
            rb_state10.var_21 = (rb_state10.var_21) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_17) : (0x9db0ecc1));
            rb_state10.var_22 = (rb_state10.var_22) ^ (((0x396f34f7) + (rb_state10.var_17)) ^ (rb_state10.var_18));
            rb_state10.var_23 = (rb_state10.var_23) - (((((0x69d19fc5) - (rb_state10.var_20)) - (0x92d95760)) - (0x2d340c8c)) + (rb_state10.var_19));
            rb_state10.var_24 = (rb_state10.var_24) ^ (((rb_state10.var_18) == (0x0)) ? (rb_state10.var_21) : (0xc1a188da));
            rb_state10.var_25 = (rb_state10.var_25) - (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_23) : (0x78bbafad));
            rb_state10.var_26 = (rb_state10.var_26) + (rb_state10.var_24);
            rb_state10.var_27 = (rb_state10.var_27) ^ (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_25) : (0xb74fe5a6));
            rb_state10.var_28 = (rb_state10.var_28) - (((((0x85a3d073) + (rb_state10.var_20)) - (rb_state10.var_26)) + (0x20c86c8b)) ^ (rb_state10.var_27));
            rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_28);
        }
        #endif
        OutputX = 1.0 - OutputX;
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - (((rb_state0.var_0) == (0x41b93662)) ? (rb_state0.var_2) : (0x2ff716ab));
    rb_state0.var_1 = (rb_state0.var_1) ^ (0xb5fc071a);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (167 < rb_input_size ? (uint32_t)rb_input[167] : 0x52287e92);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + ((0xcebe8d2c) ^ (0x1a0aa383));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (217 < rb_input_size ? (uint32_t)rb_input[217] : 0x4d3e3e51);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_2);
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

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_26 = (rb_state3.var_26) ^ (((rb_state3.var_27) == (0x0)) ? (rb_state3.var_25) : (0xe2a3df4));
    if ((rb_state3.var_29) == (0x9ada7839)) {
        rb_state3.var_42 = 0x2b0b7eed;
        rb_state3.var_43 = 0x9e2b6c9c;
        rb_state3.var_44 = 0xd0a54df;
        rb_state3.var_45 = (rb_state3.var_45) + (((rb_state3.var_42) == (0x0)) ? (rb_state3.var_43) : (0x6d6d1714));
        rb_state3.var_46 = (rb_state3.var_46) ^ (((rb_state3.var_44) == (0x0)) ? (rb_state3.var_45) : (0xe8adeacc));
        rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_46);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x34a0f2a2);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + ((0x42e3551) + (0xef349110));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + ((0x38cb7e93) - (0xc85810a1));
    #endif
    xStockPrice = sptprice;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (0xe9f1f5bb);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (238 < rb_input_size ? (uint32_t)rb_input[238] : 0x74596205);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_3);
    #endif
    xStrikePrice = strike;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x89d8efc)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        rb_state1.var_18 = 0x4d9484e5;
        rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_14);
        rb_state1.var_20 = (rb_state1.var_20) + (186 < rb_input_size ? (uint32_t)rb_input[186] : 0x98cdf0);
        rb_state1.var_21 = (rb_state1.var_21) ^ (((0xd8631dd) ^ (0xd014a5a1)) - (rb_state1.var_12));
        rb_state1.var_22 = (rb_state1.var_22) + (((0x47becbf3) ^ (rb_state1.var_18)) + (rb_state1.var_15));
        rb_state1.var_23 = (rb_state1.var_23) - (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_13) : (0xb1ba838a));
        rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_20) : (0x7a74016c));
        rb_state1.var_25 = (rb_state1.var_25) ^ (((0x4afbf299) ^ (rb_state1.var_17)) - (rb_state1.var_21));
        rb_state1.var_26 = (rb_state1.var_26) ^ (((rb_state1.var_22) == (0x0)) ? (rb_state1.var_23) : (0x125236e2));
        rb_state1.var_27 = (rb_state1.var_27) ^ (((((0xe33826ca) - (rb_state1.var_24)) ^ (0xca215e31)) - (rb_state1.var_25)) - (0xec660d3e));
        rb_state1.var_28 = (rb_state1.var_28) - (((((0x5c616e1f) - (0xd53d80ad)) + (rb_state1.var_27)) + (rb_state1.var_26)) + (0xe7c58a29));
        rb_state1.var_17 = (rb_state1.var_17) - (rb_state1.var_28);
        rb_state1.var_4 = rb_state1.var_17;
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    if ((rb_state1.var_1) == (0x89d8efc)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        rb_state1.var_30 = 0xa7081e15;
        rb_state1.var_31 = 0xbb6cf716;
        rb_state1.var_32 = (rb_state1.var_32) - (((0x6443f0dc) - (rb_state1.var_19)) ^ (rb_state1.var_30));
        rb_state1.var_33 = (rb_state1.var_33) ^ ((0xab2ac8) ^ (rb_state1.var_20));
        rb_state1.var_34 = (rb_state1.var_34) ^ (rb_state1.var_18);
        rb_state1.var_35 = (rb_state1.var_35) ^ (((rb_state1.var_31) == (0x0)) ? (rb_state1.var_32) : (0xd1abb5da));
        rb_state1.var_36 = (rb_state1.var_36) ^ (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_33) : (0xd84e4da7));
        rb_state1.var_37 = (rb_state1.var_37) - (((rb_state1.var_34) == (0x0)) ? (rb_state1.var_35) : (0x3778e031));
        rb_state1.var_38 = (rb_state1.var_38) + (((0xb43dfe32) + (rb_state1.var_36)) + (rb_state1.var_22));
        rb_state1.var_39 = (rb_state1.var_39) + (((((0xcb753d07) ^ (rb_state1.var_23)) ^ (rb_state1.var_38)) - (0xd02c8928)) - (rb_state1.var_37));
        rb_state1.var_29 = (rb_state1.var_29) + (rb_state1.var_39);
        rb_state1.var_4 = rb_state1.var_29;
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0xffffb5c0)) {
        rb_state7.var_6 = 0x7cbd7289;
        rb_state7.var_7 = 0x6db76608;
        rb_state7.var_8 = 0x40d1a45;
        rb_state7.var_9 = (rb_state7.var_9) ^ (((0x5c1c72b7) ^ (0xd86201dd)) - (rb_state7.var_7));
        rb_state7.var_10 = (rb_state7.var_10) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_6) : (0x6785914e));
        rb_state7.var_11 = (rb_state7.var_11) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0xfe6c0fab));
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_11);
    }
    #endif
    xRiskFreeRate = rate;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x1822932a));
    #endif
    xVolatility = volatility;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0xd87e3afb) + (0xd9c1d9e1));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (0x82c7144f);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_3) : (0x508c414));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_2);
    #endif
    xTime = time;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_3) == (0xa82ea8d4)) {
        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0x4471090b));
        rb_state6.var_12 = (rb_state6.var_12) ^ (((0x8c3fe08f) + (rb_state6.var_10)) - (rb_state6.var_6));
        rb_state6.var_13 = (rb_state6.var_13) - (23 < rb_input_size ? (uint32_t)rb_input[23] : 0xa372795);
        rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_7);
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_11) : (0x433ae801));
        rb_state6.var_16 = (rb_state6.var_16) - (((((0xf25cdc1b) - (0xfe78e168)) + (rb_state6.var_12)) + (rb_state6.var_13)) + (rb_state6.var_12));
        rb_state6.var_17 = (rb_state6.var_17) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_14) : (0x1d231280));
        rb_state6.var_18 = (rb_state6.var_18) ^ (rb_state6.var_15);
        rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_16) : (0x87064dc7));
        rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_18) : (0xe3db5a84));
        rb_state6.var_21 = (rb_state6.var_21) - (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_19) : (0x25524f99));
        rb_state6.var_22 = (rb_state6.var_22) ^ (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_21) : (0x15004010));
        rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0xdaa2a406);
    rb_state9.var_3 = (rb_state9.var_3) + (34 < rb_input_size ? (uint32_t)rb_input[34] : 0xe130254e);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (26 < rb_input_size ? (uint32_t)rb_input[26] : 0x65b445f1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0x7e6b95b6) + (rb_state15.var_1));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (219 < rb_input_size ? (uint32_t)rb_input[219] : 0x6ca2ec81);
    #endif
    xSqrtTime = sqrt(xTime);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) ^ ((0x691e2a21) ^ (0x52847a0b));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_2);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x89afb23a)) {
        pthread_mutex_lock(&(rb_state16.lock_42));
        rb_state16.var_25 = 0x6ce5f334;
        rb_state16.var_26 = 0x16f29a92;
        rb_state16.var_27 = (rb_state16.var_27) ^ (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_15) : (0x2f00788));
        rb_state16.var_28 = (rb_state16.var_28) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_14) : (0x6d3d0a5f));
        rb_state16.var_29 = (rb_state16.var_29) + (rb_state16.var_26);
        rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_25) : (0xc8c2e2));
        rb_state16.var_31 = (rb_state16.var_31) + (rb_state16.var_27);
        rb_state16.var_32 = (rb_state16.var_32) + (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_29) : (0x687301f4));
        rb_state16.var_33 = (rb_state16.var_33) ^ (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_30) : (0x19b9f45a));
        rb_state16.var_34 = (rb_state16.var_34) + (((0x1db42195) - (0xe41e18f6)) - (rb_state16.var_31));
        rb_state16.var_35 = (rb_state16.var_35) ^ (rb_state16.var_32);
        rb_state16.var_36 = (rb_state16.var_36) ^ (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_33) : (0xf90a7b78));
        rb_state16.var_37 = (rb_state16.var_37) + (rb_state16.var_34);
        rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_35);
        rb_state16.var_39 = (rb_state16.var_39) + (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_36) : (0x2ca247d));
        rb_state16.var_40 = (rb_state16.var_40) - (((rb_state16.var_37) == (0x0)) ? (rb_state16.var_38) : (0x7ae5a19e));
        rb_state16.var_41 = (rb_state16.var_41) - (((rb_state16.var_39) == (0x0)) ? (rb_state16.var_40) : (0xb7d72d73));
        rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_41);
        pthread_mutex_unlock(&(rb_state16.lock_42));
    }
    #endif
    logValues = log(sptprice / strike);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - ((0x27bca3fe) ^ (0x7b5b3f7b));
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x4388400a)) {
        pthread_mutex_lock(&(rb_state8.lock_36));
        rb_state8.var_17 = 0xe8b822be;
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_9) : (0x1e5b4f61));
        rb_state8.var_19 = (rb_state8.var_19) - (rb_state8.var_7);
        rb_state8.var_20 = (rb_state8.var_20) - (((((0x8144534e) + (rb_state8.var_11)) - (rb_state8.var_8)) + (rb_state8.var_17)) ^ (0x352429d));
        rb_state8.var_21 = (rb_state8.var_21) + (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_18) : (0x89c3be4c));
        rb_state8.var_22 = (rb_state8.var_22) ^ (((0x15a41e00) ^ (0xd5d68e93)) + (rb_state8.var_19));
        rb_state8.var_23 = (rb_state8.var_23) ^ (rb_state8.var_20);
        rb_state8.var_24 = (rb_state8.var_24) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_21) : (0x8e149547));
        rb_state8.var_25 = (rb_state8.var_25) - (rb_state8.var_22);
        rb_state8.var_26 = (rb_state8.var_26) - (((rb_state8.var_23) == (0x0)) ? (rb_state8.var_24) : (0x5ae23428));
        rb_state8.var_27 = (rb_state8.var_27) + (((0x6eba53ba) ^ (0xd209b9d5)) - (rb_state8.var_25));
        rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0xc5ec499));
        rb_state8.var_16 = (rb_state8.var_16) - (rb_state8.var_28);
        rb_state8.var_5 = rb_state8.var_16;
        pthread_mutex_unlock(&(rb_state8.lock_36));
    }
    if ((rb_state8.var_0) == (0x43883fdc)) {
        pthread_mutex_lock(&(rb_state8.lock_36));
        rb_state8.var_30 = 0x89e5d5c2;
        rb_state8.var_31 = 0xcefd6208;
        rb_state8.var_32 = (rb_state8.var_32) + (rb_state8.var_31);
        rb_state8.var_33 = (rb_state8.var_33) ^ (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_30) : (0xa0e9f8a9));
        rb_state8.var_34 = (rb_state8.var_34) + (((((0xfa90bce5) ^ (rb_state8.var_16)) ^ (rb_state8.var_32)) ^ (rb_state8.var_14)) + (rb_state8.var_17));
        rb_state8.var_35 = (rb_state8.var_35) ^ (((((0x7b79ea92) - (rb_state8.var_18)) ^ (rb_state8.var_34)) + (0x918c0aae)) ^ (rb_state8.var_33));
        rb_state8.var_29 = (rb_state8.var_29) - (rb_state8.var_35);
        rb_state8.var_5 = rb_state8.var_29;
        pthread_mutex_unlock(&(rb_state8.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) + (135 < rb_input_size ? (uint32_t)rb_input[135] : 0x34e2a0eb);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x60)) ? (rb_state13.var_0) : (0xece8ade9));
    #endif
    xLogTerm = logValues;

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0xa0f2275c)) {
        rb_state2.var_19 = 0xc3f191ca;
        rb_state2.var_20 = 0x47bac95a;
        rb_state2.var_21 = (rb_state2.var_21) ^ (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_19) : (0x3e1d44a3));
        rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_20) : (0x7ee6a06c));
        rb_state2.var_23 = (rb_state2.var_23) - (((((0xc0600b37) + (rb_state2.var_21)) + (0x7a9a540e)) + (rb_state2.var_22)) - (rb_state2.var_16));
        rb_state2.var_5 = (rb_state2.var_5) ^ (rb_state2.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (0xb38ae178);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0x76845698);
    if ((rb_state19.var_2) == (0x3a9346e0)) {
        rb_state19.var_21 = (rb_state19.var_21) + (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_14) : (0x1a96b06f));
        rb_state19.var_22 = (rb_state19.var_22) + (((0xbf7232c5) ^ (rb_state19.var_15)) + (rb_state19.var_17));
        rb_state19.var_23 = (rb_state19.var_23) - (((((0x3b92f466) + (rb_state19.var_21)) + (rb_state19.var_13)) ^ (0xd091ea02)) + (0x3cc7c018));
        rb_state19.var_24 = (rb_state19.var_24) - (((0xa603ec53) + (0x22003ae6)) - (rb_state19.var_22));
        rb_state19.var_25 = (rb_state19.var_25) ^ (rb_state19.var_23);
        rb_state19.var_26 = (rb_state19.var_26) ^ (((((0xa36565bd) ^ (rb_state19.var_18)) - (0x51f5c497)) - (rb_state19.var_24)) - (rb_state19.var_25));
        rb_state19.var_20 = (rb_state19.var_20) + (rb_state19.var_26);
        rb_state19.var_4 = rb_state19.var_20;
    }
    if ((rb_state19.var_2) == (0x3a9346e0)) {
        pthread_mutex_lock(&(rb_state19.lock_42));
        rb_state19.var_28 = 0x6384f24e;
        rb_state19.var_29 = 0x1f9df1b1;
        rb_state19.var_30 = (rb_state19.var_30) - (((0x79a17422) + (rb_state19.var_28)) - (rb_state19.var_20));
        rb_state19.var_31 = (rb_state19.var_31) - (rb_state19.var_29);
        rb_state19.var_32 = (rb_state19.var_32) ^ (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_30) : (0x846318cc));
        rb_state19.var_33 = (rb_state19.var_33) + (rb_state19.var_31);
        rb_state19.var_34 = (rb_state19.var_34) ^ (rb_state19.var_32);
        rb_state19.var_35 = (rb_state19.var_35) - (((0x5d103ba8) + (rb_state19.var_33)) - (rb_state19.var_21));
        rb_state19.var_36 = (rb_state19.var_36) ^ (((0xb6cf641b) + (0x38c7f02e)) - (rb_state19.var_34));
        rb_state19.var_37 = (rb_state19.var_37) ^ (rb_state19.var_35);
        rb_state19.var_38 = (rb_state19.var_38) - (rb_state19.var_36);
        rb_state19.var_39 = (rb_state19.var_39) ^ (((0x33a3e086) + (rb_state19.var_37)) ^ (0x47e8ad46));
        rb_state19.var_40 = (rb_state19.var_40) + (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_38) : (0x9ed5b4d9));
        rb_state19.var_41 = (rb_state19.var_41) - (((rb_state19.var_39) == (0x0)) ? (rb_state19.var_40) : (0xf65e7f16));
        rb_state19.var_27 = (rb_state19.var_27) - (rb_state19.var_41);
        rb_state19.var_4 = rb_state19.var_27;
        pthread_mutex_unlock(&(rb_state19.lock_42));
    }
    #endif
    xPowerTerm = xVolatility * xVolatility;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_2) : (0xe0bb23bc));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ ((0x2071ebc7) ^ (0x5b69a82a));
    #endif
    xPowerTerm = xPowerTerm * 0.5;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x47d71a05));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (10 < rb_input_size ? (uint32_t)rb_input[10] : 0x991d8c65);
    if ((rb_state8.var_0) == (0x43883fdc)) {
        if ((rb_state8.var_5) != (0x0)) {
            if (!((rb_state8.var_5) == (rb_state8.var_29))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_1) : (0xa58f760b));
    #endif
    xD1 = xRiskFreeRate + xPowerTerm;
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (227 < rb_input_size ? (uint32_t)rb_input[227] : 0xc3560e70);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_4 = (rb_state18.var_4) - ((0x9fb1d459) + (rb_state18.var_3));
    if ((rb_state18.var_4) == (0xb6d33aa0)) {
        pthread_mutex_lock(&(rb_state18.lock_10));
        rb_state18.var_5 = rb_state18.var_0;
        pthread_mutex_unlock(&(rb_state18.lock_10));
    }
    if ((rb_state18.var_4) == (0xb6d33aa0)) {
        if (!((rb_state18.var_0) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
    }
    #endif
    xD1 = xD1 * xTime;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + ((0x2d4c1f3f) - (rb_state3.var_1));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) - (0xacc396e9);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (167 < rb_input_size ? (uint32_t)rb_input[167] : 0xa6ca09b3);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0xf8a3bc70));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_9 = (rb_state13.var_9) + (198 < rb_input_size ? (uint32_t)rb_input[198] : 0xabb13e7f);
    if ((rb_state13.var_8) == (0x694bccf5)) {
        rb_state13.var_23 = 0xbdfdd92f;
        rb_state13.var_24 = 0xc1260b86;
        rb_state13.var_25 = (rb_state13.var_25) - (((0x64b0fe12) + (rb_state13.var_24)) + (rb_state13.var_19));
        rb_state13.var_26 = (rb_state13.var_26) - (71 < rb_input_size ? (uint32_t)rb_input[71] : 0x7c922a0c);
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_18) : (0xf54a0ca7));
        rb_state13.var_28 = (rb_state13.var_28) + (((((0x5ce3f8be) ^ (rb_state13.var_26)) - (rb_state13.var_25)) ^ (rb_state13.var_20)) - (0x83a3ac72));
        rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_27) : (0x8300254d));
        rb_state13.var_30 = (rb_state13.var_30) + (((0x3d799e95) ^ (rb_state13.var_22)) - (rb_state13.var_28));
        rb_state13.var_31 = (rb_state13.var_31) + (rb_state13.var_29);
        rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_30) : (0x3e41598e));
        rb_state13.var_33 = (rb_state13.var_33) ^ (((0x5404abd7) + (rb_state13.var_31)) ^ (0x1def527d));
        rb_state13.var_34 = (rb_state13.var_34) ^ (((rb_state13.var_24) == (0x0)) ? (rb_state13.var_32) : (0xb534a068));
        rb_state13.var_35 = (rb_state13.var_35) + (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_33) : (0xbeea759e));
        rb_state13.var_36 = (rb_state13.var_36) ^ (((rb_state13.var_34) == (0x0)) ? (rb_state13.var_35) : (0x7b7f9704));
        rb_state13.var_22 = (rb_state13.var_22) - (rb_state13.var_36);
        rb_state13.var_13 = rb_state13.var_22;
    }
    if ((rb_state13.var_8) == (0x694bccf5)) {
        pthread_mutex_lock(&(rb_state13.lock_59));
        rb_state13.var_38 = 0x5b28136b;
        rb_state13.var_39 = (rb_state13.var_39) - (0x4b2144ec);
        rb_state13.var_40 = (rb_state13.var_40) ^ (((((0xf90843d6) ^ (rb_state13.var_28)) ^ (0x33f7b80a)) - (rb_state13.var_27)) + (rb_state13.var_38));
        rb_state13.var_41 = (rb_state13.var_41) + (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_29) : (0x87aa8241));
        rb_state13.var_42 = (rb_state13.var_42) + (((0xd8f3b8dd) - (0x70b41e8d)) ^ (rb_state13.var_26));
        rb_state13.var_43 = (rb_state13.var_43) ^ (rb_state13.var_39);
        rb_state13.var_44 = (rb_state13.var_44) + (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_40) : (0x89669c80));
        rb_state13.var_45 = (rb_state13.var_45) ^ (((rb_state13.var_41) == (0x0)) ? (rb_state13.var_42) : (0x275aef4));
        rb_state13.var_46 = (rb_state13.var_46) - (((rb_state13.var_32) == (0x0)) ? (rb_state13.var_43) : (0xd340b7b6));
        rb_state13.var_47 = (rb_state13.var_47) + (rb_state13.var_44);
        rb_state13.var_48 = (rb_state13.var_48) + (rb_state13.var_45);
        rb_state13.var_49 = (rb_state13.var_49) ^ (((0x802887b) ^ (rb_state13.var_46)) - (0xdd992266));
        rb_state13.var_50 = (rb_state13.var_50) ^ (((0x9da308f3) - (rb_state13.var_33)) + (rb_state13.var_47));
        rb_state13.var_51 = (rb_state13.var_51) - (((rb_state13.var_34) == (0x0)) ? (rb_state13.var_48) : (0xc7a08bf2));
        rb_state13.var_52 = (rb_state13.var_52) ^ (((0xd5b7390c) + (rb_state13.var_49)) - (rb_state13.var_35));
        rb_state13.var_53 = (rb_state13.var_53) + (rb_state13.var_50);
        rb_state13.var_54 = (rb_state13.var_54) + (((rb_state13.var_36) == (0x0)) ? (rb_state13.var_51) : (0xd2d7fe56));
        rb_state13.var_55 = (rb_state13.var_55) - (((rb_state13.var_52) == (0x0)) ? (rb_state13.var_53) : (0x9a614fa8));
        rb_state13.var_56 = (rb_state13.var_56) + (((0x7b01d168) + (0x4457871a)) - (rb_state13.var_54));
        rb_state13.var_57 = (rb_state13.var_57) - (((rb_state13.var_37) == (0x0)) ? (rb_state13.var_55) : (0xed67ee1e));
        rb_state13.var_58 = (rb_state13.var_58) ^ (((rb_state13.var_56) == (0x0)) ? (rb_state13.var_57) : (0x705b3407));
        rb_state13.var_37 = (rb_state13.var_37) + (rb_state13.var_58);
        rb_state13.var_13 = rb_state13.var_37;
        pthread_mutex_unlock(&(rb_state13.lock_59));
    }
    #endif
    xD1 = xD1 + xLogTerm;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x9d7ef990) ^ (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (203 < rb_input_size ? (uint32_t)rb_input[203] : 0x5dda5133);
    rb_state13.var_3 = (rb_state13.var_3) ^ (0xa34889d8);
    #endif
    xDen = xVolatility * xSqrtTime;
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x1094682f)) {
        pthread_mutex_lock(&(rb_state9.lock_38));
        rb_state9.var_6 = 0xc93c7ac6;
        rb_state9.var_7 = (rb_state9.var_7) + (((((0xa87c18b2) ^ (rb_state9.var_6)) - (0x8258764c)) - (rb_state9.var_5)) ^ (rb_state9.var_6));
        rb_state9.var_8 = (rb_state9.var_8) - (((((0x19d6ccb0) - (rb_state9.var_7)) ^ (rb_state9.var_4)) ^ (rb_state9.var_7)) ^ (rb_state9.var_8));
        rb_state9.var_5 = (rb_state9.var_5) ^ (rb_state9.var_8);
        rb_state9.var_4 = rb_state9.var_5;
        pthread_mutex_unlock(&(rb_state9.lock_38));
    }
    if ((rb_state9.var_0) == (0x1094682f)) {
        if (!((rb_state9.var_4) == (rb_state9.var_5))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (0xbfbcfed4);
    #endif
    xD1 = xD1 / xDen;
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_4) == (0xb6d33aa0)) {
        rb_state18.var_6 = 0x4a3025c9;
        rb_state18.var_7 = (rb_state18.var_7) ^ (0x9b9b0103);
        rb_state18.var_8 = (rb_state18.var_8) + (((((0x6ec31833) - (rb_state18.var_6)) - (rb_state18.var_7)) + (rb_state18.var_5)) + (rb_state18.var_6));
        rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0x57504d09));
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_9);
    }
    #endif
    xD2 = xD1 - xDen;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (154 < rb_input_size ? (uint32_t)rb_input[154] : 0x73b3c7e3);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) + (0xc69b028a);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_3) : (0xc246997));
    #endif
    d1 = xD1;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - ((0x82d397e7) + (rb_state6.var_2));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (0x88f7242f);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - ((0xd4b8f8e1) - (0x87cbd90d));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (0x8dad347f);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_6) : (0x7c1bbb32));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_4) == (0x9e2d180)) {
        rb_state17.var_10 = 0xb1837020;
        rb_state17.var_11 = 0xa977185;
        rb_state17.var_12 = (rb_state17.var_12) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_8) : (0x243e2c2f));
        rb_state17.var_13 = (rb_state17.var_13) - (((0xdb758828) ^ (rb_state17.var_11)) ^ (0x192a716e));
        rb_state17.var_14 = (rb_state17.var_14) + (((((0x479caef3) ^ (rb_state17.var_9)) + (rb_state17.var_13)) + (0x4a3ee540)) - (rb_state17.var_12));
        rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_14);
    }
    #endif
    d2 = xD2;

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_4 = (rb_state4.var_4) ^ (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_4) : (0x57deaf8e));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) + (0x7edc8f91);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (240 < rb_input_size ? (uint32_t)rb_input[240] : 0x5a23cdf3);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ ((0xeff23088) + (rb_state17.var_0));
    #endif
    NofXd1 = CNDF(d1);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x224329dd)) {
        rb_state0.var_8 = 0xf68b4194;
        rb_state0.var_9 = 0x9d372038;
        rb_state0.var_10 = (rb_state0.var_10) ^ (((((0x2dfa5d50) + (rb_state0.var_6)) ^ (0x7c6deb91)) + (0xc1acc70b)) + (rb_state0.var_8));
        rb_state0.var_11 = (rb_state0.var_11) ^ (((((0xfa0071da) - (rb_state0.var_10)) - (rb_state0.var_7)) - (rb_state0.var_9)) + (0xd8dd664));
        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_11);
        rb_state0.var_4 = rb_state0.var_7;
    }
    if ((rb_state0.var_3) == (0x224329dd)) {
        pthread_mutex_lock(&(rb_state0.lock_16));
        rb_state0.var_13 = 0xf484c646;
        rb_state0.var_14 = (rb_state0.var_14) ^ (((((0x5fff4dc4) ^ (rb_state0.var_10)) ^ (rb_state0.var_9)) + (rb_state0.var_8)) + (0x55a66647));
        rb_state0.var_15 = (rb_state0.var_15) + (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_14) : (0xc0b03c0a));
        rb_state0.var_12 = (rb_state0.var_12) ^ (rb_state0.var_15);
        rb_state0.var_4 = rb_state0.var_12;
        pthread_mutex_unlock(&(rb_state0.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (0xe5610bf8);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_37 = (rb_state3.var_37) ^ (133 < rb_input_size ? (uint32_t)rb_input[133] : 0x8707d69d);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0xffffef80)) {
        if ((rb_state5.var_5) != (0x0)) {
            if (!((rb_state5.var_5) == (rb_state5.var_36))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0xa9a08bcd);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ ((0xee28329a) - (rb_state9.var_2));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_2) == (0x569b42c8)) ? (rb_state13.var_3) : (0x2a2d4a46));
    #endif
    NofXd2 = CNDF(d2);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_1);
    rb_state0.var_3 = (rb_state0.var_3) + ((0x96cb040a) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_34) == (0x239b341f)) {
        rb_state3.var_58 = 0x1d6b9e2d;
        rb_state3.var_59 = (rb_state3.var_59) + (((((0x9ca627f2) - (rb_state3.var_41)) - (rb_state3.var_58)) - (0x809fb0b1)) - (rb_state3.var_38));
        rb_state3.var_60 = (rb_state3.var_60) ^ (((rb_state3.var_40) == (0x0)) ? (rb_state3.var_59) : (0x7472f91b));
        rb_state3.var_49 = (rb_state3.var_49) - (rb_state3.var_60);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0xcd12acdc)) {
        rb_state14.var_13 = (rb_state14.var_13) + (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_8) : (0xdfda6616));
        rb_state14.var_14 = (rb_state14.var_14) ^ (232 < rb_input_size ? (uint32_t)rb_input[232] : 0x903b81a8);
        rb_state14.var_15 = (rb_state14.var_15) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_10) : (0xb6d47535));
        rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_13) : (0x653142d2));
        rb_state14.var_17 = (rb_state14.var_17) ^ (((0xaf2fc29d) - (rb_state14.var_14)) ^ (0x6c511cd7));
        rb_state14.var_18 = (rb_state14.var_18) ^ (rb_state14.var_15);
        rb_state14.var_19 = (rb_state14.var_19) + (((((0x4dc014df) ^ (rb_state14.var_17)) + (rb_state14.var_16)) + (0x3e4070a6)) - (rb_state14.var_13));
        rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_19) : (0x96625166));
        rb_state14.var_12 = (rb_state14.var_12) + (rb_state14.var_20);
        rb_state14.var_5 = rb_state14.var_12;
    }
    if ((rb_state14.var_2) == (0xcd12acdc)) {
        pthread_mutex_lock(&(rb_state14.lock_30));
        rb_state14.var_22 = 0x64f94ba;
        rb_state14.var_23 = 0xc4cc767;
        rb_state14.var_24 = (rb_state14.var_24) + (82 < rb_input_size ? (uint32_t)rb_input[82] : 0x3b46d3c);
        rb_state14.var_25 = (rb_state14.var_25) ^ (((((0xfea4e834) - (0xd21aa797)) - (rb_state14.var_14)) + (rb_state14.var_23)) - (rb_state14.var_15));
        rb_state14.var_26 = (rb_state14.var_26) + (((((0x1a195440) + (rb_state14.var_24)) ^ (0x1be94faf)) + (0x5369fc0a)) + (rb_state14.var_22));
        rb_state14.var_27 = (rb_state14.var_27) - (((0x3747defc) - (rb_state14.var_25)) + (0x1e0584fa));
        rb_state14.var_28 = (rb_state14.var_28) - (rb_state14.var_26);
        rb_state14.var_29 = (rb_state14.var_29) - (((rb_state14.var_27) == (0x0)) ? (rb_state14.var_28) : (0x6c46234));
        rb_state14.var_21 = (rb_state14.var_21) - (rb_state14.var_29);
        rb_state14.var_5 = rb_state14.var_21;
        pthread_mutex_unlock(&(rb_state14.lock_30));
    }
    #endif
    FutureValueX = strike * (exp(-(rate) * (time)));
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_14 = (rb_state17.var_14) ^ (67 < rb_input_size ? (uint32_t)rb_input[67] : 0xfaa79f39);
        #endif
        NegNofXd1 = (1.0 - NofXd1);
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_3) == (0xfa3adea6)) ? (rb_state5.var_2) : (0x4e1db3cb));
        #endif
        NegNofXd2 = (1.0 - NofXd2);
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_3 = (rb_state8.var_3) - (147 < rb_input_size ? (uint32_t)rb_input[147] : 0xf4b27c6a);
        #endif
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - ((0x2d06e105) ^ (rb_state13.var_1));
    if ((rb_state13.var_2) == (0xa9dfbfa)) {
        pthread_mutex_lock(&(rb_state13.lock_11));
        rb_state13.var_7 = 0x849ef16c;
        rb_state13.var_8 = 0x694bccf5;
        rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_7) : (0x6391e618));
        rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_9) : (0x8d957da8));
        rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_10);
        rb_state13.var_6 = rb_state13.var_5;
        pthread_mutex_unlock(&(rb_state13.lock_11));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) ^ ((0xafb8d9f) ^ (0xc4076ea0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (208 < rb_input_size ? (uint32_t)rb_input[208] : 0x99ce8bf8);
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_1);
    rb_state18.var_4 = (rb_state18.var_4) + ((0x54e58efd) + (rb_state18.var_4));
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
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (66 < rb_input_size ? (uint32_t)rb_input[66] : 0x5f4d08fb);
    #endif
    int start = tid * (numOptions / nThreads);
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0xf721e5ca) + (rb_state14.var_0));
    #endif
    int end = start + (numOptions / nThreads);

    for (j = 0; j < NUM_RUNS; j++) {
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, price, priceDelta)
        for (i = 0; i < numOptions; i++) {
#else
        for (i = start; i < end; i++) {
#endif

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) ^ (0xc6785568);
            if ((rb_state0.var_3) == (0x224329dd)) {
                if ((rb_state0.var_4) != (0x0)) {
                    if (!((rb_state0.var_4) == (rb_state0.var_12))) {
                        racebench_trigger(0);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_36 = (rb_state3.var_36) + (246 < rb_input_size ? (uint32_t)rb_input[246] : 0x5224895);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (15 < rb_input_size ? (uint32_t)rb_input[15] : 0x828a94c9);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (0x42c03bbc);
            rb_state10.var_1 = (rb_state10.var_1) ^ (0x705dbe41);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) ^ (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x8150f845));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) ^ (0xd0c8dacd);
            #endif
            price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) + (149 < rb_input_size ? (uint32_t)rb_input[149] : 0x468bc7d7);
            rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_3);
            #endif
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0x1d2)) {
                pthread_mutex_lock(&(rb_state4.lock_19));
                rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_10) : (0x23389c19));
                rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_12) : (0x4d1ae186));
                rb_state4.var_17 = (rb_state4.var_17) - (((0xe8621337) + (0x747271f5)) ^ (rb_state4.var_15));
                rb_state4.var_18 = (rb_state4.var_18) - (((((0x42fd4ab) ^ (rb_state4.var_16)) - (rb_state4.var_14)) - (rb_state4.var_17)) ^ (0xeac2947b));
                rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_18);
                pthread_mutex_unlock(&(rb_state4.lock_19));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_4 = (rb_state17.var_4) - (0x43de3050);
            rb_state17.var_8 = (rb_state17.var_8) - (rb_state17.var_11);
            rb_state17.var_9 = (rb_state17.var_9) - ((0xdd7d54bb) ^ (rb_state17.var_16));
            if ((rb_state17.var_5) == (0x76f9dedf)) {
                if ((rb_state17.var_17) != (0x0)) {
                    if (!((rb_state17.var_17) == (rb_state17.var_42))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_2) : (0xc35f31a6));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_3 = (rb_state19.var_3) ^ ((0x972d4069) + (rb_state19.var_0));
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