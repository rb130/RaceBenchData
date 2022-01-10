#include "racebench_bugs.h"


#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID PutJob(xs, ys, xe, ye, xbe, ybe, pid)
INT xs, ys;
INT xe, ye;
INT xbe, ybe;
INT pid;
{
    INT i, j;
    INT xb_addr, yb_addr;
    INT xb_end, yb_end;
    INT xb_size, yb_size;
    WPJOB *wpentry;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (74 < rb_input_size ? (uint32_t)rb_input[74] : 0x5513603);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x57a7f75a)) ? (rb_state10.var_1) : (0x17ccc6b5));
    #endif
    xb_addr = xs;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (0xd7a66605);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_37) == (0xfbf498ea)) {
        pthread_mutex_lock(&(rb_state7.lock_79));
        rb_state7.var_65 = 0x7e6f578f;
        rb_state7.var_66 = 0xd0c34460;
        rb_state7.var_67 = 0x83977d93;
        rb_state7.var_68 = (rb_state7.var_68) - (((rb_state7.var_65) == (0x0)) ? (rb_state7.var_67) : (0x314aac09));
        rb_state7.var_69 = (rb_state7.var_69) - (((rb_state7.var_39) == (0xcb34196c)) ? (rb_state7.var_66) : (0xb494fc26));
        rb_state7.var_70 = (rb_state7.var_70) - (rb_state7.var_68);
        rb_state7.var_71 = (rb_state7.var_71) + (((((0xd06f69e6) + (0x90685bb)) - (rb_state7.var_70)) - (0x2aace401)) - (rb_state7.var_69));
        rb_state7.var_64 = (rb_state7.var_64) ^ (rb_state7.var_71);
        rb_state7.var_63 = rb_state7.var_64;
        pthread_mutex_unlock(&(rb_state7.lock_79));
    }
    if ((rb_state7.var_37) == (0xfbf498ea)) {
        if (!((rb_state7.var_63) == (rb_state7.var_64))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x5ac52e3d)) ? (rb_state11.var_0) : (0x5de4cbd1));
    #endif
    yb_addr = ys;

    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xfffffedd)) {
        pthread_mutex_lock(&(rb_state10.lock_32));
        rb_state10.var_14 = 0x88105c3b;
        rb_state10.var_15 = 0xd7c8fa3c;
        rb_state10.var_16 = (rb_state10.var_16) - ((0xcc3e8217) + (0xcf7e5df8));
        rb_state10.var_17 = (rb_state10.var_17) + (0x6a92c90);
        rb_state10.var_18 = (rb_state10.var_18) + (((0xfdf21226) ^ (rb_state10.var_14)) - (rb_state10.var_9));
        rb_state10.var_19 = (rb_state10.var_19) - (((0xe4f70b20) ^ (0x8b6b2925)) - (rb_state10.var_8));
        rb_state10.var_20 = (rb_state10.var_20) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0xc75560ca));
        rb_state10.var_21 = (rb_state10.var_21) + (rb_state10.var_17);
        rb_state10.var_22 = (rb_state10.var_22) + (rb_state10.var_18);
        rb_state10.var_23 = (rb_state10.var_23) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_19) : (0x697bc85));
        rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_20) : (0xd9fd116b));
        rb_state10.var_25 = (rb_state10.var_25) ^ (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_22) : (0x6b7ddd92));
        rb_state10.var_26 = (rb_state10.var_26) + (rb_state10.var_23);
        rb_state10.var_27 = (rb_state10.var_27) - (((((0x7b3f76fb) - (rb_state10.var_25)) + (0xddf48125)) + (rb_state10.var_12)) + (rb_state10.var_24));
        rb_state10.var_28 = (rb_state10.var_28) ^ (rb_state10.var_26);
        rb_state10.var_29 = (rb_state10.var_29) + (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_27) : (0x32ce060e));
        rb_state10.var_30 = (rb_state10.var_30) - (((0x1d040051) + (rb_state10.var_28)) ^ (0x15bb5370));
        rb_state10.var_31 = (rb_state10.var_31) ^ (((((0x3968c6dd) + (rb_state10.var_15)) ^ (rb_state10.var_29)) + (rb_state10.var_30)) + (rb_state10.var_14));
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_31);
        pthread_mutex_unlock(&(rb_state10.lock_32));
    }
    #endif
    xb_end = xb_addr + xe - 1;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xd7a66711)) {
        pthread_mutex_lock(&(rb_state6.lock_15));
        rb_state6.var_4 = 0xdc05534d;
        rb_state6.var_5 = 0xb888468;
        rb_state6.var_6 = (rb_state6.var_6) ^ ((0x821122b) + (rb_state6.var_3));
        rb_state6.var_7 = (rb_state6.var_7) ^ (((0x520d45cb) - (rb_state6.var_2)) - (rb_state6.var_4));
        rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_5);
        rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_4) : (0x2c2d8200));
        rb_state6.var_10 = (rb_state6.var_10) - (((rb_state6.var_6) == (0x159)) ? (rb_state6.var_7) : (0xae586c8d));
        rb_state6.var_11 = (rb_state6.var_11) - (rb_state6.var_8);
        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_9);
        rb_state6.var_13 = (rb_state6.var_13) + (((((0x23d743ef) - (0xbd489353)) ^ (rb_state6.var_11)) - (0x1ac6d892)) - (rb_state6.var_10));
        rb_state6.var_14 = (rb_state6.var_14) - (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0x9d3ffbf6));
        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_14);
        rb_state6.var_3 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_34 = (rb_state7.var_34) - ((0x62267fca) + (rb_state7.var_35));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xfffffedd)) {
        pthread_mutex_lock(&(rb_state10.lock_32));
        rb_state10.var_4 = 0x79756945;
        rb_state10.var_5 = 0xfd7578ff;
        rb_state10.var_6 = (rb_state10.var_6) - (rb_state10.var_3);
        rb_state10.var_7 = (rb_state10.var_7) ^ (((0x8dfaab57) - (rb_state10.var_4)) - (rb_state10.var_4));
        rb_state10.var_8 = (rb_state10.var_8) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_5) : (0x427abc7));
        rb_state10.var_9 = (rb_state10.var_9) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_6) : (0xcff18626));
        rb_state10.var_10 = (rb_state10.var_10) + (((0x5f07e83f) ^ (rb_state10.var_6)) + (rb_state10.var_7));
        rb_state10.var_11 = (rb_state10.var_11) ^ (rb_state10.var_8);
        rb_state10.var_12 = (rb_state10.var_12) - (((((0x124aee05) - (0x8262edce)) + (rb_state10.var_10)) - (rb_state10.var_9)) + (0xc85f2008));
        rb_state10.var_13 = (rb_state10.var_13) ^ (((((0xaf27be4b) ^ (rb_state10.var_11)) ^ (rb_state10.var_7)) - (0x357c5a9b)) - (rb_state10.var_12));
        rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_13);
        rb_state10.var_2 = rb_state10.var_3;
        pthread_mutex_unlock(&(rb_state10.lock_32));
    }
    if ((rb_state10.var_1) == (0xfffffedd)) {
        if (!((rb_state10.var_2) == (rb_state10.var_3))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0xcb08e414)) {
        rb_state11.var_11 = (rb_state11.var_11) - (((0x51c04fc7) - (rb_state11.var_3)) - (rb_state11.var_6));
        rb_state11.var_12 = (rb_state11.var_12) - (rb_state11.var_4);
        rb_state11.var_13 = (rb_state11.var_13) + (35 < rb_input_size ? (uint32_t)rb_input[35] : 0xaf5929a7);
        rb_state11.var_14 = (rb_state11.var_14) ^ ((0x94efae2b) ^ (rb_state11.var_7));
        rb_state11.var_15 = (rb_state11.var_15) ^ (((((0x3c7c55e9) + (0x11019747)) - (0xa9e67d80)) - (rb_state11.var_5)) ^ (rb_state11.var_11));
        rb_state11.var_16 = (rb_state11.var_16) - (((((0xfe71f0ba) ^ (0x475436b6)) + (rb_state11.var_13)) + (rb_state11.var_12)) ^ (rb_state11.var_8));
        rb_state11.var_17 = (rb_state11.var_17) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_14) : (0x7291ecfd));
        rb_state11.var_18 = (rb_state11.var_18) ^ (((0xc8647dde) + (rb_state11.var_15)) ^ (rb_state11.var_10));
        rb_state11.var_19 = (rb_state11.var_19) + (((0x80bd0a4) + (0x46cd95de)) + (rb_state11.var_16));
        rb_state11.var_20 = (rb_state11.var_20) + (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_17) : (0x35e89644));
        rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_18) : (0x5f329ce));
        rb_state11.var_22 = (rb_state11.var_22) - (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_19) : (0xdd05da6d));
        rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_21) : (0x33ec909f));
        rb_state11.var_24 = (rb_state11.var_24) ^ (((((0xbf9c03b1) ^ (0xc674750b)) ^ (rb_state11.var_23)) ^ (rb_state11.var_14)) + (rb_state11.var_22));
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (0xbb6a65f2);
    #endif
    yb_end = yb_addr + ye - 1;

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
                #endif
                xb_size = xbe;
            } else {
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) - (0xa74f5f09);
                if ((rb_state13.var_1) == (0x5025da92)) {
                    pthread_mutex_lock(&(rb_state13.lock_28));
                    rb_state13.var_8 = (rb_state13.var_8) ^ (((((0x939799fb) + (0xcbade6ad)) + (rb_state13.var_7)) + (rb_state13.var_8)) - (rb_state13.var_5));
                    rb_state13.var_9 = (rb_state13.var_9) + (6 < rb_input_size ? (uint32_t)rb_input[6] : 0xf7055131);
                    rb_state13.var_10 = (rb_state13.var_10) + (((((0x8fee4564) - (rb_state13.var_10)) + (rb_state13.var_6)) - (rb_state13.var_8)) - (rb_state13.var_9));
                    rb_state13.var_11 = (rb_state13.var_11) - (((0x5873b393) - (rb_state13.var_11)) ^ (rb_state13.var_9));
                    rb_state13.var_12 = (rb_state13.var_12) - (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_11) : (0x8ee25498));
                    rb_state13.var_7 = (rb_state13.var_7) ^ (rb_state13.var_12);
                    rb_state13.var_2 = rb_state13.var_7;
                    pthread_mutex_unlock(&(rb_state13.lock_28));
                }
                if ((rb_state13.var_1) == (0x5025da92)) {
                    pthread_mutex_lock(&(rb_state13.lock_28));
                    rb_state13.var_14 = 0x5474ac2a;
                    rb_state13.var_15 = (rb_state13.var_15) ^ (((0xd1f58513) ^ (0x8e51bc55)) + (rb_state13.var_12));
                    rb_state13.var_16 = (rb_state13.var_16) ^ (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_14) : (0xf3dd981d));
                    rb_state13.var_17 = (rb_state13.var_17) - (0x55243c3c);
                    rb_state13.var_18 = (rb_state13.var_18) ^ (((0x557c7563) - (rb_state13.var_15)) - (rb_state13.var_14));
                    rb_state13.var_19 = (rb_state13.var_19) ^ (((((0x96961f7c) + (0x6b79a70a)) ^ (0xfad213a3)) - (rb_state13.var_17)) ^ (rb_state13.var_16));
                    rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_18) : (0x78c172c6));
                    rb_state13.var_21 = (rb_state13.var_21) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_19) : (0x14c263a));
                    rb_state13.var_22 = (rb_state13.var_22) + (rb_state13.var_20);
                    rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_21);
                    rb_state13.var_24 = (rb_state13.var_24) - (rb_state13.var_22);
                    rb_state13.var_25 = (rb_state13.var_25) ^ (((0xc5773e06) + (rb_state13.var_23)) - (rb_state13.var_17));
                    rb_state13.var_26 = (rb_state13.var_26) + (((0x1321f34a) - (rb_state13.var_24)) ^ (0x3d7c8dee));
                    rb_state13.var_27 = (rb_state13.var_27) + (((((0xa3de96e7) - (0x17926617)) - (rb_state13.var_26)) - (0x542b1ba2)) - (rb_state13.var_25));
                    rb_state13.var_13 = (rb_state13.var_13) ^ (rb_state13.var_27);
                    rb_state13.var_2 = rb_state13.var_13;
                    pthread_mutex_unlock(&(rb_state13.lock_28));
                }
                #endif
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_7 = (rb_state6.var_7) - (466 < rb_input_size ? (uint32_t)rb_input[466] : 0xea85a388);
                #endif
                #ifdef RACEBENCH_BUG_9
                if ((rb_state9.var_0) == (0x20957ba1)) {
                    rb_state9.var_10 = 0xd4650e1b;
                    rb_state9.var_11 = 0x5e365c0b;
                    rb_state9.var_12 = (rb_state9.var_12) - (((((0x19da367c) + (rb_state9.var_11)) - (rb_state9.var_8)) - (0xdf6291c5)) ^ (0x5c33f468));
                    rb_state9.var_13 = (rb_state9.var_13) + (rb_state9.var_10);
                    rb_state9.var_14 = (rb_state9.var_14) + (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_12) : (0x81302ba9));
                    rb_state9.var_15 = (rb_state9.var_15) + (((0xf92a6737) + (rb_state9.var_13)) ^ (0x29104d17));
                    rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_14) : (0xba5238b7));
                    rb_state9.var_17 = (rb_state9.var_17) + (rb_state9.var_15);
                    rb_state9.var_18 = (rb_state9.var_18) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_16) : (0x553afcba));
                    rb_state9.var_19 = (rb_state9.var_19) - (((((0x5e83fddf) - (rb_state9.var_18)) ^ (rb_state9.var_12)) + (0x24d0f952)) ^ (rb_state9.var_17));
                    rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_19);
                }
                #endif
                yb_size = ybe;
            } else {
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_1);
                #endif
                yb_size = yb_end - yb_addr + 1;
            }

            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c");

            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x8bd993e7)) ? (rb_state9.var_0) : (0x639f2537));
            #endif
            wpentry->xpix = xb_addr;
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0x487d1b06)) {
                pthread_mutex_lock(&(rb_state5.lock_15));
                rb_state5.var_2 = rb_state5.var_1;
                pthread_mutex_unlock(&(rb_state5.lock_15));
            }
            if ((rb_state5.var_0) == (0x487d1b06)) {
                if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_1) == (0x62d61f0)) {
                rb_state8.var_4 = 0xe815c5fa;
                rb_state8.var_5 = 0x2887adb3;
                rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0x80448058));
                rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_5) : (0xe9303d28));
                rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_4) : (0xbdb1f0ac));
                rb_state8.var_9 = (rb_state8.var_9) - (rb_state8.var_6);
                rb_state8.var_10 = (rb_state8.var_10) - (((((0xb1bc7d7b) + (rb_state8.var_7)) ^ (0xa6cf3e5c)) + (0x949b2486)) + (rb_state8.var_8));
                rb_state8.var_11 = (rb_state8.var_11) ^ (((((0x5b121e01) + (rb_state8.var_7)) ^ (rb_state8.var_9)) - (rb_state8.var_8)) ^ (rb_state8.var_10));
                rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_11);
                rb_state8.var_2 = rb_state8.var_3;
            }
            if ((rb_state8.var_1) == (0x62d61f0)) {
                pthread_mutex_lock(&(rb_state8.lock_17));
                rb_state8.var_13 = 0x51abc0ca;
                rb_state8.var_14 = (rb_state8.var_14) ^ (rb_state8.var_9);
                rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_13) : (0x61930d2d));
                rb_state8.var_16 = (rb_state8.var_16) + (((((0xd616d067) - (0xc4d5da7)) ^ (rb_state8.var_11)) ^ (rb_state8.var_14)) ^ (rb_state8.var_15));
                rb_state8.var_12 = (rb_state8.var_12) + (rb_state8.var_16);
                rb_state8.var_2 = rb_state8.var_12;
                pthread_mutex_unlock(&(rb_state8.lock_17));
            }
            #endif
            wpentry->ypix = yb_addr;
            wpentry->xdim = xb_size;
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_1) == (0xfdef6058)) {
                pthread_mutex_lock(&(rb_state4.lock_19));
                rb_state4.var_4 = (rb_state4.var_4) ^ (((((0xa0a55caa) - (rb_state4.var_2)) + (0x79a7eeb2)) - (rb_state4.var_4)) ^ (rb_state4.var_3));
                rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_4);
                rb_state4.var_2 = rb_state4.var_3;
                pthread_mutex_unlock(&(rb_state4.lock_19));
            }
            if ((rb_state4.var_1) == (0xfdef6058)) {
                if (!((rb_state4.var_2) == (rb_state4.var_3))) {
                    racebench_trigger(4);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_1);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) - ((0xef1e00e6) ^ (rb_state18.var_1));
            #endif
            wpentry->ydim = yb_size;

            if (!gm->workpool[pid][0]) {
                wpentry->next = NULL;
            } else {
                wpentry->next = gm->workpool[pid][0];
            }

            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_0) == (0x6fc93528)) {
                rb_state0.var_2 = rb_state0.var_1;
            }
            if ((rb_state0.var_0) == (0x6fc93528)) {
                pthread_mutex_lock(&(rb_state0.lock_10));
                if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                    racebench_trigger(0);
                }
                pthread_mutex_unlock(&(rb_state0.lock_10));
            }
            #endif
            gm->workpool[pid][0] = wpentry;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - (352 < rb_input_size ? (uint32_t)rb_input[352] : 0x90483840);
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_1) == (0x10e1ff1a)) {
                rb_state18.var_3 = 0x1d3206ca;
                rb_state18.var_4 = 0xde8a26bc;
                rb_state18.var_5 = (rb_state18.var_5) - (((rb_state18.var_3) == (0x1d3206ca)) ? (rb_state18.var_4) : (0x1bfc581f));
                rb_state18.var_6 = (rb_state18.var_6) - (((0x5bc0acf7) + (rb_state18.var_2)) - (0xc4a72344));
                rb_state18.var_7 = (rb_state18.var_7) ^ (((rb_state18.var_5) == (0x2175d944)) ? (rb_state18.var_6) : (0x8ebaa72d));
                rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_7);
            }
            #endif
            xb_addr += xbe;
        }

        xb_addr = xs;
        yb_addr += ybe;
    }
}

INT GetJob(job, pid)
RAYJOB *job;
INT pid;
{
    WPJOB *wpentry;

    { pthread_mutex_lock(&gm->wplock[pid]); }
    wpentry = gm->workpool[pid][0];

    if (!wpentry) {
        gm->wpstat[pid][0] = WPS_EMPTY;
        { pthread_mutex_unlock(&gm->wplock[pid]); }
        return (WPS_EMPTY);
    }

    gm->workpool[pid][0] = wpentry->next;
    { pthread_mutex_unlock(&gm->wplock[pid]); }

    job->x = wpentry->xpix;
    job->y = wpentry->ypix;
    job->xcurr = wpentry->xpix;
    job->ycurr = wpentry->ypix;
    job->xlen = wpentry->xdim;
    job->ylen = wpentry->ydim;

    GlobalFree(wpentry);
    return (WPS_VALID);
}

INT GetJobs(job, pid)
RAYJOB *job;
INT pid;
{
    INT i;

    i = pid;

    if (gm->wpstat[i][0] == WPS_VALID) {
        if (GetJob(job, i) == WPS_VALID) {
            return (WPS_VALID);
        }
    }

    i = (pid + 1) % gm->nprocs;

    while (i != pid) {
        if (gm->wpstat[i][0] == WPS_VALID) {
            if (GetJob(job, i) == WPS_VALID) {
                return (WPS_VALID);
            }
        }

        i = (i + 1) % gm->nprocs;
    }

    return (WPS_EMPTY);
}

VOID PrintWorkPool(pid)
INT pid;
{
    WPJOB *j;

    j = gm->workpool[pid][0];

    while (j) {
        printf("Workpool entry:    pid=%3ld    xs=%3ld    ys=%3ld    xe=%3ld    ye=%3ld\n", pid, j->xpix, j->ypix, j->xdim, j->ydim);
        j = j->next;
    }
}

VOID InitWorkPool(pid)
INT pid;
{
    INT i;
    INT x, y;
    INT xe, ye;
    INT xsize, ysize;

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x0)) {
        rb_state1.var_21 = (rb_state1.var_21) + (((rb_state1.var_12) == (0xd57f7956)) ? (rb_state1.var_11) : (0xba87cff9));
        rb_state1.var_22 = (rb_state1.var_22) + (((rb_state1.var_13) == (0x4452e677)) ? (rb_state1.var_21) : (0x9008b38c));
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_22);
    }
    #endif
    gm->wpstat[pid][0] = WPS_VALID;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (63 < rb_input_size ? (uint32_t)rb_input[63] : 0x6ed7f100);
    if ((rb_state7.var_1) == (0x3321ed0e)) {
        rb_state7.var_21 = 0x46e0f1e5;
        rb_state7.var_22 = (rb_state7.var_22) - (((((0x9b6b09f5) ^ (rb_state7.var_9)) - (rb_state7.var_10)) ^ (0x1f382e72)) + (rb_state7.var_21));
        rb_state7.var_23 = (rb_state7.var_23) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x4e29dd16));
        rb_state7.var_24 = (rb_state7.var_24) - (((0x9c576060) ^ (0x3a6af3f5)) - (rb_state7.var_8));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_22) : (0x64728acd));
        rb_state7.var_26 = (rb_state7.var_26) ^ (rb_state7.var_23);
        rb_state7.var_27 = (rb_state7.var_27) + (((((0x53405d0f) + (rb_state7.var_24)) - (0x2da0d3d0)) + (rb_state7.var_14)) - (rb_state7.var_25));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_26) : (0x65c32e3f));
        rb_state7.var_29 = (rb_state7.var_29) + (rb_state7.var_27);
        rb_state7.var_30 = (rb_state7.var_30) - (((0x45014960) + (rb_state7.var_16)) + (rb_state7.var_28));
        rb_state7.var_31 = (rb_state7.var_31) - (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_29) : (0x747726cf));
        rb_state7.var_32 = (rb_state7.var_32) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_30) : (0x528a4d3a));
        rb_state7.var_33 = (rb_state7.var_33) + (rb_state7.var_31);
        rb_state7.var_34 = (rb_state7.var_34) + (((rb_state7.var_32) == (0x0)) ? (rb_state7.var_33) : (0xccf4cf69));
        rb_state7.var_20 = (rb_state7.var_20) - (rb_state7.var_34);
        rb_state7.var_2 = rb_state7.var_20;
    }
    if ((rb_state7.var_1) == (0x3321ed0e)) {
        pthread_mutex_lock(&(rb_state7.lock_61));
        rb_state7.var_36 = 0x46bd4b10;
        rb_state7.var_37 = 0xfbf498ea;
        rb_state7.var_38 = (rb_state7.var_38) + (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_20) : (0xb98f1981));
        rb_state7.var_39 = (rb_state7.var_39) - (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_22) : (0x7e92356b));
        rb_state7.var_40 = (rb_state7.var_40) - (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_19) : (0x6e7a1f4a));
        rb_state7.var_41 = (rb_state7.var_41) + (rb_state7.var_37);
        rb_state7.var_42 = (rb_state7.var_42) ^ (((0xdee4e163) - (0x5dd13e0)) ^ (rb_state7.var_36));
        rb_state7.var_43 = (rb_state7.var_43) - (((((0xfe76c3bd) - (0x260227d1)) ^ (rb_state7.var_38)) ^ (rb_state7.var_39)) ^ (0xa2450d9d));
        rb_state7.var_44 = (rb_state7.var_44) - (rb_state7.var_40);
        rb_state7.var_45 = (rb_state7.var_45) ^ (((0x7d67a664) ^ (rb_state7.var_41)) ^ (rb_state7.var_25));
        rb_state7.var_46 = (rb_state7.var_46) ^ (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_42) : (0xda0ba2db));
        rb_state7.var_47 = (rb_state7.var_47) + (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_43) : (0xb469549e));
        rb_state7.var_48 = (rb_state7.var_48) + (((rb_state7.var_28) == (0x0)) ? (rb_state7.var_44) : (0x8a6d2eeb));
        rb_state7.var_49 = (rb_state7.var_49) ^ (((rb_state7.var_29) == (0x0)) ? (rb_state7.var_45) : (0x2b2f548b));
        rb_state7.var_50 = (rb_state7.var_50) + (((rb_state7.var_30) == (0x0)) ? (rb_state7.var_46) : (0xde302a5f));
        rb_state7.var_51 = (rb_state7.var_51) + (((0x3d955e14) - (rb_state7.var_47)) ^ (0xb9e50ffa));
        rb_state7.var_52 = (rb_state7.var_52) + (rb_state7.var_48);
        rb_state7.var_53 = (rb_state7.var_53) - (((rb_state7.var_31) == (0x0)) ? (rb_state7.var_49) : (0x205be294));
        rb_state7.var_54 = (rb_state7.var_54) - (((0x5cb9f443) - (rb_state7.var_50)) ^ (rb_state7.var_32));
        rb_state7.var_55 = (rb_state7.var_55) + (rb_state7.var_51);
        rb_state7.var_56 = (rb_state7.var_56) ^ (((rb_state7.var_52) == (0x0)) ? (rb_state7.var_53) : (0x3d79311c));
        rb_state7.var_57 = (rb_state7.var_57) - (rb_state7.var_54);
        rb_state7.var_58 = (rb_state7.var_58) ^ (((0xfa7c1895) - (rb_state7.var_55)) + (0x55903a54));
        rb_state7.var_59 = (rb_state7.var_59) - (((rb_state7.var_56) == (0x0)) ? (rb_state7.var_57) : (0xad00e2c1));
        rb_state7.var_60 = (rb_state7.var_60) - (((((0x372f44d7) ^ (rb_state7.var_33)) - (0x3a9b6d58)) + (rb_state7.var_58)) + (rb_state7.var_59));
        rb_state7.var_35 = (rb_state7.var_35) + (rb_state7.var_60);
        rb_state7.var_2 = rb_state7.var_35;
        pthread_mutex_unlock(&(rb_state7.lock_61));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xee95ea4)) {
        rb_state14.var_2 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0xee95ea4)) {
        pthread_mutex_lock(&(rb_state14.lock_20));
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
        pthread_mutex_unlock(&(rb_state14.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (0x784d97ab);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x34a7b628)) {
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_15))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    gm->workpool[pid][0] = NULL;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0x401b499a) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_6 = (rb_state6.var_6) + (558 < rb_input_size ? (uint32_t)rb_input[558] : 0x9e89f859);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (0xa2af63bd);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x68)) {
        if ((rb_state15.var_4) != (0x0)) {
            rb_state15.var_19 = !((rb_state15.var_2) == (rb_state15.var_3));
        }
    }
    rb_state15.var_9 = (rb_state15.var_9) ^ (0x3d9abd85);
    #endif
    i = 0;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x4fa734ba) ^ (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xee95ea4)) {
        pthread_mutex_lock(&(rb_state14.lock_20));
        rb_state14.var_3 = 0xaf820650;
        rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_3);
        rb_state14.var_5 = (rb_state14.var_5) - ((0x5b18b038) ^ (0x2da0a409));
        rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_1);
        rb_state14.var_7 = (rb_state14.var_7) ^ (((0x82a5b9e7) ^ (rb_state14.var_3)) + (rb_state14.var_4));
        rb_state14.var_8 = (rb_state14.var_8) ^ (((0x31ab8452) - (rb_state14.var_5)) ^ (rb_state14.var_2));
        rb_state14.var_9 = (rb_state14.var_9) ^ (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_4) : (0x5b6ee1f8));
        rb_state14.var_10 = (rb_state14.var_10) + (rb_state14.var_5);
        rb_state14.var_11 = (rb_state14.var_11) + (((rb_state14.var_7) == (0xdca9c607)) ? (rb_state14.var_6) : (0xfd9344f5));
        rb_state14.var_12 = (rb_state14.var_12) - (((((0x4787aca9) ^ (rb_state14.var_8)) - (0xffc2f0f2)) + (rb_state14.var_8)) + (rb_state14.var_7));
        rb_state14.var_13 = (rb_state14.var_13) - (((0x2986e20f) - (rb_state14.var_9)) - (rb_state14.var_9));
        rb_state14.var_14 = (rb_state14.var_14) - (((rb_state14.var_10) == (0x8947ebcf)) ? (rb_state14.var_11) : (0xa4ef1d3));
        rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_10) == (0x8947ebcf)) ? (rb_state14.var_12) : (0xe86c289a));
        rb_state14.var_16 = (rb_state14.var_16) - (rb_state14.var_13);
        rb_state14.var_17 = (rb_state14.var_17) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_14) : (0x93c70d9e));
        rb_state14.var_18 = (rb_state14.var_18) - (((rb_state14.var_15) == (0x8ad15e3e)) ? (rb_state14.var_16) : (0x8d43bf97));
        rb_state14.var_19 = (rb_state14.var_19) - (((((0xe574cd61) ^ (rb_state14.var_12)) - (rb_state14.var_18)) ^ (0x6f65ece3)) + (rb_state14.var_17));
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_19);
        pthread_mutex_unlock(&(rb_state14.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_10 = (rb_state15.var_10) ^ ((0x58d3f4b9) + (0x1a125bd9));
    if ((rb_state15.var_11) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_70));
        rb_state15.var_34 = 0x58384596;
        rb_state15.var_35 = 0x7eacbce;
        rb_state15.var_36 = 0xb263212e;
        rb_state15.var_37 = (rb_state15.var_37) ^ (rb_state15.var_34);
        rb_state15.var_38 = (rb_state15.var_38) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_36) : (0x65f4c49b));
        rb_state15.var_39 = (rb_state15.var_39) ^ (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_35) : (0xa4da1649));
        rb_state15.var_40 = (rb_state15.var_40) + (((0x9a7e723f) ^ (rb_state15.var_37)) - (rb_state15.var_21));
        rb_state15.var_41 = (rb_state15.var_41) - (rb_state15.var_38);
        rb_state15.var_42 = (rb_state15.var_42) - (((rb_state15.var_22) == (0x0)) ? (rb_state15.var_39) : (0xee7fe8b4));
        rb_state15.var_43 = (rb_state15.var_43) ^ (((((0x952f28b5) - (rb_state15.var_23)) - (rb_state15.var_40)) - (rb_state15.var_41)) - (rb_state15.var_24));
        rb_state15.var_44 = (rb_state15.var_44) + (((0x6be306a) + (rb_state15.var_25)) ^ (rb_state15.var_42));
        rb_state15.var_45 = (rb_state15.var_45) - (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_43) : (0xd55d1b45));
        rb_state15.var_46 = (rb_state15.var_46) + (((rb_state15.var_27) == (0x0)) ? (rb_state15.var_44) : (0x9c4ed0f));
        rb_state15.var_47 = (rb_state15.var_47) - (((0x4a5a91fe) ^ (0x6ed3f525)) - (rb_state15.var_45));
        rb_state15.var_48 = (rb_state15.var_48) ^ (((0xbb5ee999) ^ (rb_state15.var_28)) + (rb_state15.var_46));
        rb_state15.var_49 = (rb_state15.var_49) ^ (((rb_state15.var_29) == (0x0)) ? (rb_state15.var_47) : (0xf585908e));
        rb_state15.var_50 = (rb_state15.var_50) ^ (((rb_state15.var_30) == (0x0)) ? (rb_state15.var_48) : (0x150bd4c6));
        rb_state15.var_51 = (rb_state15.var_51) - (((rb_state15.var_31) == (0x0)) ? (rb_state15.var_49) : (0x33ce8641));
        rb_state15.var_52 = (rb_state15.var_52) + (((rb_state15.var_50) == (0xba09edf1)) ? (rb_state15.var_51) : (0x577fdd03));
        rb_state15.var_33 = (rb_state15.var_33) ^ (rb_state15.var_52);
        rb_state15.var_20 = rb_state15.var_33;
        pthread_mutex_unlock(&(rb_state15.lock_70));
    }
    if ((rb_state15.var_11) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_70));
        rb_state15.var_54 = 0x91c78aad;
        rb_state15.var_55 = 0x819ef676;
        rb_state15.var_56 = 0x94dd66b5;
        rb_state15.var_57 = (rb_state15.var_57) - (0xdcfc46f6);
        rb_state15.var_58 = (rb_state15.var_58) ^ (((rb_state15.var_32) == (0x0)) ? (rb_state15.var_54) : (0xf8c6bc76));
        rb_state15.var_59 = (rb_state15.var_59) ^ (rb_state15.var_55);
        rb_state15.var_60 = (rb_state15.var_60) ^ (((rb_state15.var_56) == (0x94dd66b5)) ? (rb_state15.var_57) : (0xfe82f5ad));
        rb_state15.var_61 = (rb_state15.var_61) ^ (((0x340f7ff5) ^ (rb_state15.var_58)) ^ (rb_state15.var_33));
        rb_state15.var_62 = (rb_state15.var_62) - (((rb_state15.var_34) == (0x58384596)) ? (rb_state15.var_59) : (0xc39805b7));
        rb_state15.var_63 = (rb_state15.var_63) - (((0xfd738c78) ^ (rb_state15.var_60)) ^ (rb_state15.var_35));
        rb_state15.var_64 = (rb_state15.var_64) + (((((0x806faf47) ^ (rb_state15.var_62)) ^ (rb_state15.var_61)) ^ (rb_state15.var_36)) ^ (0xe83c8512));
        rb_state15.var_65 = (rb_state15.var_65) - (((0x23901282) - (rb_state15.var_63)) ^ (rb_state15.var_37));
        rb_state15.var_66 = (rb_state15.var_66) ^ (rb_state15.var_64);
        rb_state15.var_67 = (rb_state15.var_67) ^ (((0x9e96efe0) ^ (rb_state15.var_65)) ^ (rb_state15.var_38));
        rb_state15.var_68 = (rb_state15.var_68) - (rb_state15.var_66);
        rb_state15.var_69 = (rb_state15.var_69) ^ (((((0xa04361cc) ^ (rb_state15.var_67)) - (0x17699ca7)) ^ (rb_state15.var_68)) - (rb_state15.var_39));
        rb_state15.var_53 = (rb_state15.var_53) + (rb_state15.var_69);
        rb_state15.var_20 = rb_state15.var_53;
        pthread_mutex_unlock(&(rb_state15.lock_70));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x660947b6)) {
        pthread_mutex_lock(&(rb_state16.lock_35));
        if ((rb_state16.var_2) != (0x0)) {
            if (!((rb_state16.var_2) == (rb_state16.var_19))) {
                racebench_trigger(16);
            }
        }
        pthread_mutex_unlock(&(rb_state16.lock_35));
    }
    #endif
    xsize = Display.xres / blockx;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x5ac52ea7)) ? (rb_state11.var_0) : (0x86d27bb6));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x34a7b628)) {
        rb_state19.var_6 = 0x51ba89fc;
        rb_state19.var_7 = (rb_state19.var_7) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_6) : (0x42d6336d));
        rb_state19.var_8 = (rb_state19.var_8) - (95 < rb_input_size ? (uint32_t)rb_input[95] : 0xc112e055);
        rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_6) : (0xc4506463));
        rb_state19.var_10 = (rb_state19.var_10) ^ (rb_state19.var_5);
        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0x1af74b38));
        rb_state19.var_12 = (rb_state19.var_12) - (((((0xa38a434b) - (rb_state19.var_9)) - (0x2e1db80b)) + (0x30425dc9)) + (rb_state19.var_10));
        rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_11);
        rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_13) : (0x5c7005cc));
        rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_14);
        rb_state19.var_2 = rb_state19.var_5;
    }
    if ((rb_state19.var_1) == (0x34a7b628)) {
        pthread_mutex_lock(&(rb_state19.lock_24));
        rb_state19.var_16 = 0x5422a0ce;
        rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_16) : (0x5bb6e49b));
        rb_state19.var_18 = (rb_state19.var_18) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_9) : (0x5e645a61));
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_11) : (0x142ccb52));
        rb_state19.var_20 = (rb_state19.var_20) ^ (rb_state19.var_17);
        rb_state19.var_21 = (rb_state19.var_21) + (((((0x1a32488c) + (0x8d887fa4)) ^ (rb_state19.var_19)) - (rb_state19.var_18)) ^ (rb_state19.var_13));
        rb_state19.var_22 = (rb_state19.var_22) - (((0x10bad04) - (rb_state19.var_14)) - (rb_state19.var_20));
        rb_state19.var_23 = (rb_state19.var_23) + (((((0xfc6deb0f) + (rb_state19.var_21)) + (0xd3328193)) ^ (rb_state19.var_22)) + (0x20a229ec));
        rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_23);
        rb_state19.var_2 = rb_state19.var_15;
        pthread_mutex_unlock(&(rb_state19.lock_24));
    }
    #endif
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (0xd3965c4a);
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_1) == (0x3321ed0e)) {
                if ((rb_state7.var_2) != (0x0)) {
                    rb_state7.var_62 = !((rb_state7.var_2) == (rb_state7.var_35));
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xdc8eea4e));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x8a326691));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0xf9631a48)) ? (rb_state19.var_1) : (0x1151fbdb));
            #endif
            ye = ysize;
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x4fa734ba)) ? (rb_state5.var_0) : (0x885c3a95));
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
                rb_state6.var_1 = (rb_state6.var_1) + (396 < rb_input_size ? (uint32_t)rb_input[396] : 0xdc53ceb6);
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_0) == (0xcb08e414)) {
                    pthread_mutex_lock(&(rb_state11.lock_25));
                    rb_state11.var_3 = 0x7040ec95;
                    rb_state11.var_4 = 0x880e1a70;
                    rb_state11.var_5 = 0xb9b4fcf;
                    rb_state11.var_6 = (rb_state11.var_6) ^ (421 < rb_input_size ? (uint32_t)rb_input[421] : 0x49e8313c);
                    rb_state11.var_7 = (rb_state11.var_7) - (((((0xab6f48ea) - (0xd5e78de0)) ^ (rb_state11.var_3)) + (rb_state11.var_1)) - (rb_state11.var_4));
                    rb_state11.var_8 = (rb_state11.var_8) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0x1cffafd0));
                    rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_7) : (0x55f44f48));
                    rb_state11.var_10 = (rb_state11.var_10) + (((((0x4d1ad123) - (0x420150eb)) - (rb_state11.var_8)) ^ (0x7334b7a2)) ^ (rb_state11.var_9));
                    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_10);
                    rb_state11.var_1 = rb_state11.var_2;
                    pthread_mutex_unlock(&(rb_state11.lock_25));
                }
                if ((rb_state11.var_0) == (0xcb08e414)) {
                    pthread_mutex_lock(&(rb_state11.lock_25));
                    if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                        racebench_trigger(11);
                    }
                    pthread_mutex_unlock(&(rb_state11.lock_25));
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_1 = (rb_state12.var_1) + (0x11e30ad3);
                if ((rb_state12.var_1) == (0x478c2b4c)) {
                    rb_state12.var_14 = 0x2baae836;
                    rb_state12.var_15 = 0x71feb3a0;
                    rb_state12.var_16 = (rb_state12.var_16) ^ (((((0xe2701fba) ^ (rb_state12.var_8)) ^ (rb_state12.var_9)) ^ (rb_state12.var_10)) + (rb_state12.var_15));
                    rb_state12.var_17 = (rb_state12.var_17) + (0x9777ebce);
                    rb_state12.var_18 = (rb_state12.var_18) ^ (((0x3382c2cf) - (rb_state12.var_14)) - (0xbe4dadc2));
                    rb_state12.var_19 = (rb_state12.var_19) + (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_17) : (0x7b57f249));
                    rb_state12.var_20 = (rb_state12.var_20) - (rb_state12.var_18);
                    rb_state12.var_21 = (rb_state12.var_21) ^ (((((0xc943a038) ^ (rb_state12.var_19)) - (0x487c99c5)) + (0x23ae1a33)) + (rb_state12.var_20));
                    rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_21);
                    rb_state12.var_2 = rb_state12.var_13;
                }
                if ((rb_state12.var_1) == (0x478c2b4c)) {
                    pthread_mutex_lock(&(rb_state12.lock_35));
                    rb_state12.var_23 = (rb_state12.var_23) - (((0xf11b6c75) - (rb_state12.var_11)) - (rb_state12.var_14));
                    rb_state12.var_24 = (rb_state12.var_24) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_15) : (0xf8aa8233));
                    rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_13);
                    rb_state12.var_26 = (rb_state12.var_26) ^ (rb_state12.var_12);
                    rb_state12.var_27 = (rb_state12.var_27) ^ (((((0xa7f346e1) - (rb_state12.var_23)) ^ (rb_state12.var_24)) ^ (rb_state12.var_17)) + (0x1e45788e));
                    rb_state12.var_28 = (rb_state12.var_28) - (((0x164167a) ^ (rb_state12.var_25)) + (rb_state12.var_18));
                    rb_state12.var_29 = (rb_state12.var_29) - (((((0xd0653c49) ^ (rb_state12.var_26)) + (0x4f9f0237)) + (rb_state12.var_19)) ^ (rb_state12.var_27));
                    rb_state12.var_30 = (rb_state12.var_30) + (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_28) : (0x63170d66));
                    rb_state12.var_31 = (rb_state12.var_31) + (rb_state12.var_29);
                    rb_state12.var_32 = (rb_state12.var_32) - (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_30) : (0x5d459ca6));
                    rb_state12.var_33 = (rb_state12.var_33) ^ (rb_state12.var_31);
                    rb_state12.var_34 = (rb_state12.var_34) - (((rb_state12.var_32) == (0x0)) ? (rb_state12.var_33) : (0x883d288a));
                    rb_state12.var_22 = (rb_state12.var_22) ^ (rb_state12.var_34);
                    rb_state12.var_2 = rb_state12.var_22;
                    pthread_mutex_unlock(&(rb_state12.lock_35));
                }
                #endif
                xe = xsize;
            }

            if (i == pid) {
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_1) == (0xb1a68f4a)) {
                    pthread_mutex_lock(&(rb_state3.lock_7));
                    rb_state3.var_3 = 0x3a67dae4;
                    rb_state3.var_4 = 0x57a85cb3;
                    rb_state3.var_5 = (rb_state3.var_5) - (((((0x8a983a33) ^ (rb_state3.var_4)) ^ (rb_state3.var_2)) ^ (rb_state3.var_3)) - (rb_state3.var_3));
                    rb_state3.var_6 = (rb_state3.var_6) ^ (((((0x4ca9642c) - (0xd8c2d8a8)) ^ (rb_state3.var_5)) - (rb_state3.var_4)) - (rb_state3.var_5));
                    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_6);
                    pthread_mutex_unlock(&(rb_state3.lock_7));
                }
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) - (0x3fbf2b87);
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) - ((0x81c5b0ec) ^ (0x9effe32e));
                #endif
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0xff92d980)) ? (rb_state0.var_0) : (0x8db0fbbe));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) ^ (299 < rb_input_size ? (uint32_t)rb_input[299] : 0x46a472ed);
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_8) == (0xb888468)) {
                rb_state6.var_17 = rb_state6.var_9;
            }
            if ((rb_state6.var_8) == (0xb888468)) {
                if (!((rb_state6.var_9) == (rb_state6.var_17))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_38 = (rb_state7.var_38) - (rb_state7.var_36);
            #endif
            i = (i + 1) % gm->nprocs;
        }
    }
}