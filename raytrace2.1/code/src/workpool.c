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

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (571 < rb_input_size ? (uint32_t)rb_input[571] : 0xaf621b13);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x4c75d0cf)) {
        if ((rb_state13.var_1) != (0x0)) {
            if (!((rb_state13.var_1) == (rb_state13.var_15))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xbc728172)) {
        rb_state14.var_11 = 0xf54c118;
        rb_state14.var_12 = 0xca670458;
        rb_state14.var_13 = (rb_state14.var_13) - (((((0x3e98fd6d) ^ (rb_state14.var_11)) - (rb_state14.var_3)) - (rb_state14.var_4)) ^ (rb_state14.var_5));
        rb_state14.var_14 = (rb_state14.var_14) + (rb_state14.var_12);
        rb_state14.var_15 = (rb_state14.var_15) - (rb_state14.var_13);
        rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_15) : (0x3b753bc8));
        rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_16);
    }
    #endif
    xb_addr = xs;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x540bf6f6)) ? (rb_state14.var_0) : (0x23fddc9c));
    #endif
    yb_addr = ys;

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x9e8e08d3) - (0x86106350));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x0)) {
        rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_9) : (0x82ca346a));
        rb_state19.var_22 = (rb_state19.var_22) + (376 < rb_input_size ? (uint32_t)rb_input[376] : 0x172325f9);
        rb_state19.var_23 = (rb_state19.var_23) + (rb_state19.var_11);
        rb_state19.var_24 = (rb_state19.var_24) ^ (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_22) : (0x1dc8b9e4));
        rb_state19.var_25 = (rb_state19.var_25) + (rb_state19.var_23);
        rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_24);
        rb_state19.var_27 = (rb_state19.var_27) + (((rb_state19.var_25) == (0x0)) ? (rb_state19.var_26) : (0x66b82414));
        rb_state19.var_20 = (rb_state19.var_20) ^ (rb_state19.var_27);
        rb_state19.var_1 = rb_state19.var_20;
    }
    if ((rb_state19.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state19.lock_38));
        rb_state19.var_29 = 0xdf4e0342;
        rb_state19.var_30 = 0xdd46a633;
        rb_state19.var_31 = 0xef589bfa;
        rb_state19.var_32 = (rb_state19.var_32) - ((0x1334f521) - (rb_state19.var_12));
        rb_state19.var_33 = (rb_state19.var_33) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_29) : (0xbfb4b87b));
        rb_state19.var_34 = (rb_state19.var_34) + (((rb_state19.var_31) == (0x0)) ? (rb_state19.var_30) : (0x724c4d10));
        rb_state19.var_35 = (rb_state19.var_35) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_32) : (0x4922733));
        rb_state19.var_36 = (rb_state19.var_36) + (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0x902849c5));
        rb_state19.var_37 = (rb_state19.var_37) - (((((0x972bd9a4) - (0x669c7331)) + (rb_state19.var_36)) - (rb_state19.var_35)) + (0xb5266742));
        rb_state19.var_28 = (rb_state19.var_28) - (rb_state19.var_37);
        rb_state19.var_1 = rb_state19.var_28;
        pthread_mutex_unlock(&(rb_state19.lock_38));
    }
    #endif
    xb_end = xb_addr + xe - 1;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x719429b)) {
        rb_state3.var_3 = 0x1785abf1;
        rb_state3.var_4 = 0x79aa507b;
        rb_state3.var_5 = (rb_state3.var_5) ^ (((0x1ec8106c) - (rb_state3.var_3)) - (0x436269fe));
        rb_state3.var_6 = (rb_state3.var_6) - (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_4) : (0xef71378a));
        rb_state3.var_7 = (rb_state3.var_7) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_5) : (0xc8a5819d));
        rb_state3.var_8 = (rb_state3.var_8) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0x4b29b1c7));
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_8);
    }
    #endif
    yb_end = yb_addr + ye - 1;

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_0) == (0x5fe1bf8d)) {
                    pthread_mutex_lock(&(rb_state2.lock_8));
                    rb_state2.var_2 = rb_state2.var_1;
                    pthread_mutex_unlock(&(rb_state2.lock_8));
                }
                if ((rb_state2.var_0) == (0x5fe1bf8d)) {
                    if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
                #endif
                xb_size = xbe;
            } else {
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                #ifdef RACEBENCH_BUG_1
                if ((rb_state1.var_0) == (0x3396c124)) {
                    if ((rb_state1.var_1) != (0x0)) {
                        if (!((rb_state1.var_1) == (rb_state1.var_19))) {
                            racebench_trigger(1);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_0) == (0x5fe1bf8d)) {
                    rb_state2.var_3 = 0xea306ac5;
                    rb_state2.var_4 = (rb_state2.var_4) + (67 < rb_input_size ? (uint32_t)rb_input[67] : 0xc024d93b);
                    rb_state2.var_5 = (rb_state2.var_5) + (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_2) : (0x4bd0291b));
                    rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_4);
                    rb_state2.var_7 = (rb_state2.var_7) ^ (((((0x64833de) + (rb_state2.var_5)) + (rb_state2.var_6)) ^ (rb_state2.var_5)) ^ (rb_state2.var_4));
                    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_7);
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0xa4e8e4b1)) ? (rb_state7.var_0) : (0xad8b1da8));
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (118 < rb_input_size ? (uint32_t)rb_input[118] : 0xfe5a9cb4);
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_0) == (0xdecead34)) {
                    pthread_mutex_lock(&(rb_state11.lock_11));
                    rb_state11.var_3 = 0x2af4ac3d;
                    rb_state11.var_4 = (rb_state11.var_4) + (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_3) : (0x6bae0b4a));
                    rb_state11.var_5 = (rb_state11.var_5) ^ (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_3) : (0xfe9ce9ea));
                    rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_1);
                    rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_4) : (0x90912a15));
                    rb_state11.var_8 = (rb_state11.var_8) - (((((0xb4863b9a) ^ (rb_state11.var_5)) + (0x7b4cd634)) ^ (rb_state11.var_6)) ^ (rb_state11.var_6));
                    rb_state11.var_9 = (rb_state11.var_9) ^ (rb_state11.var_7);
                    rb_state11.var_10 = (rb_state11.var_10) - (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_9) : (0xfde77563));
                    rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_10);
                    pthread_mutex_unlock(&(rb_state11.lock_11));
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                if ((rb_state15.var_0) == (0xfffffed0)) {
                    rb_state15.var_3 = 0x730aecd3;
                    rb_state15.var_4 = (rb_state15.var_4) + (396 < rb_input_size ? (uint32_t)rb_input[396] : 0xc9456140);
                    rb_state15.var_5 = (rb_state15.var_5) + (((0xb3b6899b) ^ (rb_state15.var_3)) - (rb_state15.var_1));
                    rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_4);
                    rb_state15.var_7 = (rb_state15.var_7) + (((0xcaa97ace) + (rb_state15.var_3)) + (rb_state15.var_5));
                    rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_2) : (0x418f66ea));
                    rb_state15.var_9 = (rb_state15.var_9) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_4) : (0x278353db));
                    rb_state15.var_10 = (rb_state15.var_10) ^ (((0xe5ef683d) - (rb_state15.var_8)) + (rb_state15.var_5));
                    rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_6) : (0xffd0991));
                    rb_state15.var_12 = (rb_state15.var_12) - (rb_state15.var_7);
                    rb_state15.var_13 = (rb_state15.var_13) - (rb_state15.var_8);
                    rb_state15.var_14 = (rb_state15.var_14) ^ (((((0xcb182fd) + (rb_state15.var_9)) + (0xe99c2e84)) - (rb_state15.var_10)) + (rb_state15.var_10));
                    rb_state15.var_15 = (rb_state15.var_15) + (rb_state15.var_11);
                    rb_state15.var_16 = (rb_state15.var_16) - (((0x43cd5a04) + (rb_state15.var_12)) - (0x46cefcd0));
                    rb_state15.var_17 = (rb_state15.var_17) + (rb_state15.var_13);
                    rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_14);
                    rb_state15.var_19 = (rb_state15.var_19) ^ (rb_state15.var_15);
                    rb_state15.var_20 = (rb_state15.var_20) ^ (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_16) : (0xb01fe2ab));
                    rb_state15.var_21 = (rb_state15.var_21) + (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_18) : (0xbe67557c));
                    rb_state15.var_22 = (rb_state15.var_22) ^ (rb_state15.var_19);
                    rb_state15.var_23 = (rb_state15.var_23) + (((0x183f1586) ^ (rb_state15.var_20)) + (0xcbf2c2ce));
                    rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_21) == (0x0)) ? (rb_state15.var_22) : (0x4b863030));
                    rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_23);
                    rb_state15.var_26 = (rb_state15.var_26) + (rb_state15.var_24);
                    rb_state15.var_27 = (rb_state15.var_27) ^ (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0xddf1a116));
                    rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_27);
                }
                #endif
                yb_size = ybe;
            } else {
                yb_size = yb_end - yb_addr + 1;
            }

            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
            #endif
            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c");

            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xfffffb66)) {
                rb_state9.var_3 = 0x59112fc0;
                rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_3);
                rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_2);
                rb_state9.var_6 = (rb_state9.var_6) - (((((0xe89006a1) ^ (rb_state9.var_5)) + (rb_state9.var_5)) - (rb_state9.var_4)) ^ (rb_state9.var_4));
                rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_6);
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) ^ (0xfabf75c0);
            #endif
            wpentry->xpix = xb_addr;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0xc2102c6c)) ? (rb_state1.var_0) : (0x331a4fb7));
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0xd340b7f3)) {
                rb_state7.var_10 = (rb_state7.var_10) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_5) : (0x6cdffc5c));
                rb_state7.var_11 = (rb_state7.var_11) + (0xb9fa4ebe);
                rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_6);
                rb_state7.var_13 = (rb_state7.var_13) ^ (((((0x75d358cd) + (rb_state7.var_10)) - (rb_state7.var_4)) - (0xc3382b87)) - (0xce47e5e7));
                rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_12) : (0x2a6418d));
                rb_state7.var_15 = (rb_state7.var_15) - (((0x59481ece) ^ (rb_state7.var_8)) + (rb_state7.var_13));
                rb_state7.var_16 = (rb_state7.var_16) + (rb_state7.var_14);
                rb_state7.var_17 = (rb_state7.var_17) + (((0xe9cad5e3) + (rb_state7.var_9)) - (rb_state7.var_15));
                rb_state7.var_18 = (rb_state7.var_18) + (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_17) : (0xd4881098));
                rb_state7.var_9 = (rb_state7.var_9) + (rb_state7.var_18);
                rb_state7.var_1 = rb_state7.var_9;
            }
            if ((rb_state7.var_0) == (0xd340b7f3)) {
                pthread_mutex_lock(&(rb_state7.lock_30));
                rb_state7.var_20 = 0x6daf71cd;
                rb_state7.var_21 = 0x920c60e2;
                rb_state7.var_22 = (rb_state7.var_22) ^ (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x56a7262b));
                rb_state7.var_23 = (rb_state7.var_23) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_20) : (0xe7ed6ab1));
                rb_state7.var_24 = (rb_state7.var_24) ^ (((((0xb4852ca9) + (rb_state7.var_13)) + (rb_state7.var_14)) ^ (rb_state7.var_21)) ^ (rb_state7.var_22));
                rb_state7.var_25 = (rb_state7.var_25) - (rb_state7.var_23);
                rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_24) : (0x6e70bf5f));
                rb_state7.var_27 = (rb_state7.var_27) ^ (rb_state7.var_25);
                rb_state7.var_28 = (rb_state7.var_28) - (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_26) : (0xc5e8ad67));
                rb_state7.var_29 = (rb_state7.var_29) - (((((0xbf3b3be0) - (0xef74139f)) ^ (rb_state7.var_27)) - (rb_state7.var_28)) + (0xffbe4650));
                rb_state7.var_19 = (rb_state7.var_19) + (rb_state7.var_29);
                rb_state7.var_1 = rb_state7.var_19;
                pthread_mutex_unlock(&(rb_state7.lock_30));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x58135f40)) {
                pthread_mutex_lock(&(rb_state18.lock_39));
                rb_state18.var_3 = 0x62481548;
                rb_state18.var_4 = 0x60bea060;
                rb_state18.var_5 = 0x59316091;
                rb_state18.var_6 = (rb_state18.var_6) ^ (0x20eca119);
                rb_state18.var_7 = (rb_state18.var_7) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_4) : (0x681adbff));
                rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_5) : (0xd36db5f));
                rb_state18.var_9 = (rb_state18.var_9) + (((0xefce444c) ^ (rb_state18.var_3)) + (rb_state18.var_3));
                rb_state18.var_10 = (rb_state18.var_10) ^ (rb_state18.var_6);
                rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_7) : (0x46ee0384));
                rb_state18.var_12 = (rb_state18.var_12) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_8) : (0xa874c932));
                rb_state18.var_13 = (rb_state18.var_13) - (((((0x6856061) - (rb_state18.var_10)) + (rb_state18.var_9)) + (rb_state18.var_7)) ^ (rb_state18.var_6));
                rb_state18.var_14 = (rb_state18.var_14) + (rb_state18.var_11);
                rb_state18.var_15 = (rb_state18.var_15) - (((((0x6cd2691) - (0xde09a2a5)) ^ (rb_state18.var_12)) ^ (0x4e72a492)) + (rb_state18.var_13));
                rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_14) : (0xb9cebc6c));
                rb_state18.var_17 = (rb_state18.var_17) + (((0x98a59a07) ^ (0xb0ac2c2a)) - (rb_state18.var_15));
                rb_state18.var_18 = (rb_state18.var_18) ^ (((0x98a63f4b) ^ (rb_state18.var_16)) ^ (rb_state18.var_9));
                rb_state18.var_19 = (rb_state18.var_19) + (((((0x50dfa92a) - (0xf1ea72de)) - (rb_state18.var_17)) - (rb_state18.var_18)) ^ (0x610eb3c3));
                rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_19);
                rb_state18.var_1 = rb_state18.var_2;
                pthread_mutex_unlock(&(rb_state18.lock_39));
            }
            if ((rb_state18.var_0) == (0x58135f40)) {
                if (!((rb_state18.var_1) == (rb_state18.var_2))) {
                    racebench_trigger(18);
                }
            }
            #endif
            wpentry->ypix = yb_addr;
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0x173)) {
                rb_state6.var_15 = 0x15cd1190;
                rb_state6.var_16 = 0x42e42a60;
                rb_state6.var_17 = (rb_state6.var_17) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_15) : (0xda6c6681));
                rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_17) : (0x2d6e9c7b));
                rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_18);
                rb_state6.var_1 = rb_state6.var_14;
            }
            if ((rb_state6.var_0) == (0x173)) {
                pthread_mutex_lock(&(rb_state6.lock_31));
                rb_state6.var_20 = 0xe7f60e02;
                rb_state6.var_21 = (rb_state6.var_21) + (0xbd6d773);
                rb_state6.var_22 = (rb_state6.var_22) - (((((0xaeed33f) ^ (rb_state6.var_20)) + (rb_state6.var_11)) - (rb_state6.var_9)) - (rb_state6.var_10));
                rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_12) : (0xd3cd1c03));
                rb_state6.var_24 = (rb_state6.var_24) ^ (((0xd1504a8) ^ (rb_state6.var_8)) + (rb_state6.var_14));
                rb_state6.var_25 = (rb_state6.var_25) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_21) : (0x50b87f9b));
                rb_state6.var_26 = (rb_state6.var_26) - (((0xa9b21624) + (rb_state6.var_16)) - (rb_state6.var_22));
                rb_state6.var_27 = (rb_state6.var_27) + (((((0x1d20829) ^ (rb_state6.var_18)) ^ (rb_state6.var_23)) + (rb_state6.var_24)) - (rb_state6.var_17));
                rb_state6.var_28 = (rb_state6.var_28) + (((rb_state6.var_25) == (0x0)) ? (rb_state6.var_26) : (0xe2775d7f));
                rb_state6.var_29 = (rb_state6.var_29) ^ (rb_state6.var_27);
                rb_state6.var_30 = (rb_state6.var_30) - (((rb_state6.var_28) == (0x0)) ? (rb_state6.var_29) : (0x64c2737c));
                rb_state6.var_19 = (rb_state6.var_19) + (rb_state6.var_30);
                rb_state6.var_1 = rb_state6.var_19;
                pthread_mutex_unlock(&(rb_state6.lock_31));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0xe3d37670)) {
                rb_state8.var_2 = rb_state8.var_1;
            }
            if ((rb_state8.var_0) == (0xe3d37670)) {
                if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                    racebench_trigger(8);
                }
            }
            #endif
            wpentry->xdim = xb_size;
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (0x9e717ca8);
            #endif
            wpentry->ydim = yb_size;

            if (!gm->workpool[pid][0]) {
                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_0) == (0x0)) {
                    rb_state5.var_2 = rb_state5.var_1;
                }
                if ((rb_state5.var_0) == (0x0)) {
                    if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                        racebench_trigger(5);
                    }
                }
                #endif
                wpentry->next = NULL;
            } else {
                wpentry->next = gm->workpool[pid][0];
            }

            gm->workpool[pid][0] = wpentry;
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_0) == (0x16bebdde)) {
                rb_state17.var_2 = rb_state17.var_1;
            }
            if ((rb_state17.var_0) == (0x16bebdde)) {
                if (!((rb_state17.var_1) == (rb_state17.var_2))) {
                    racebench_trigger(17);
                }
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

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x2bcb98ec)) ? (rb_state0.var_0) : (0xbe0ab131));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ ((0xd83a4856) - (0x392e1ca3));
    #endif
    gm->wpstat[pid][0] = WPS_VALID;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x43b24704)) {
        pthread_mutex_lock(&(rb_state10.lock_24));
        rb_state10.var_3 = (rb_state10.var_3) + (37 < rb_input_size ? (uint32_t)rb_input[37] : 0x469f16aa);
        rb_state10.var_4 = (rb_state10.var_4) ^ (0xdc44cf97);
        rb_state10.var_5 = (rb_state10.var_5) ^ (((((0xd66587c3) + (0x6f2bbf34)) ^ (rb_state10.var_1)) + (rb_state10.var_2)) - (0x20ecd76c));
        rb_state10.var_6 = (rb_state10.var_6) - (rb_state10.var_3);
        rb_state10.var_7 = (rb_state10.var_7) + (rb_state10.var_4);
        rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0xa318d5d0));
        rb_state10.var_9 = (rb_state10.var_9) - (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_7) : (0x28c85b53));
        rb_state10.var_10 = (rb_state10.var_10) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_9) : (0x8d9fab87));
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_10);
        rb_state10.var_1 = rb_state10.var_2;
        pthread_mutex_unlock(&(rb_state10.lock_24));
    }
    if ((rb_state10.var_0) == (0x43b24704)) {
        pthread_mutex_lock(&(rb_state10.lock_24));
        rb_state10.var_11 = (rb_state10.var_11) - (rb_state10.var_5);
        rb_state10.var_12 = (rb_state10.var_12) ^ ((0xef45fbf0) + (rb_state10.var_7));
        rb_state10.var_13 = (rb_state10.var_13) ^ (0xbb19a5f9);
        rb_state10.var_14 = (rb_state10.var_14) - (((0xc1d42f24) ^ (rb_state10.var_8)) + (rb_state10.var_4));
        rb_state10.var_15 = (rb_state10.var_15) + (((((0x941b15c5) - (rb_state10.var_10)) ^ (rb_state10.var_6)) ^ (rb_state10.var_9)) + (rb_state10.var_11));
        rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x2fc7e20f));
        rb_state10.var_17 = (rb_state10.var_17) ^ (((0x9926bd1d) - (rb_state10.var_14)) - (0x9e422fa3));
        rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_15);
        rb_state10.var_19 = (rb_state10.var_19) ^ (((0xc187a139) - (rb_state10.var_16)) ^ (0xd9611e9d));
        rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_17) : (0xc3f858ad));
        rb_state10.var_21 = (rb_state10.var_21) - (((((0xc3fd9375) + (rb_state10.var_19)) ^ (rb_state10.var_18)) ^ (rb_state10.var_12)) - (0x6f391c33));
        rb_state10.var_22 = (rb_state10.var_22) + (((0xb4cdfa65) - (rb_state10.var_20)) ^ (rb_state10.var_13));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_22) : (0x7a6f22e7));
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_23);
        pthread_mutex_unlock(&(rb_state10.lock_24));
    }
    if ((rb_state10.var_0) == (0x43b24704)) {
        rb_state10.var_25 = !((rb_state10.var_1) == (rb_state10.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x833255b0)) {
        pthread_mutex_lock(&(rb_state16.lock_19));
        rb_state16.var_3 = 0x4e057adb;
        rb_state16.var_4 = 0xaa32beff;
        rb_state16.var_5 = (rb_state16.var_5) ^ (((0x29a1d0da) ^ (rb_state16.var_4)) ^ (0x44f10e94));
        rb_state16.var_6 = (rb_state16.var_6) + (((0x46db48ea) ^ (rb_state16.var_1)) - (0xc2e2bd61));
        rb_state16.var_7 = (rb_state16.var_7) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_3) : (0xa66260fc));
        rb_state16.var_8 = (rb_state16.var_8) + (rb_state16.var_5);
        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_6) == (0x83f88b89)) ? (rb_state16.var_7) : (0xd8a7200c));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_3) == (0x4e057adb)) ? (rb_state16.var_8) : (0xd109ad47));
        rb_state16.var_11 = (rb_state16.var_11) - (((0xd17c54dc) + (rb_state16.var_9)) - (rb_state16.var_4));
        rb_state16.var_12 = (rb_state16.var_12) - (((((0x3a34acdf) + (rb_state16.var_10)) ^ (rb_state16.var_6)) ^ (rb_state16.var_5)) ^ (rb_state16.var_11));
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_12);
        rb_state16.var_1 = rb_state16.var_2;
        pthread_mutex_unlock(&(rb_state16.lock_19));
    }
    if ((rb_state16.var_0) == (0x833255b0)) {
        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    gm->workpool[pid][0] = NULL;

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (0xc907b3c5);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x7ff43843)) {
        rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_2);
        rb_state12.var_4 = (rb_state12.var_4) + (290 < rb_input_size ? (uint32_t)rb_input[290] : 0x38419e82);
        rb_state12.var_5 = (rb_state12.var_5) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_3) : (0x7c601cfb));
        rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_4);
        rb_state12.var_7 = (rb_state12.var_7) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_5) : (0x713050c2));
        rb_state12.var_8 = (rb_state12.var_8) + (((0x16543bc4) ^ (rb_state12.var_4)) + (rb_state12.var_6));
        rb_state12.var_9 = (rb_state12.var_9) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_7) : (0xe98f883b));
        rb_state12.var_10 = (rb_state12.var_10) - (((0xa4362bb9) + (rb_state12.var_6)) ^ (rb_state12.var_8));
        rb_state12.var_11 = (rb_state12.var_11) ^ (((0x22a3d03a) - (rb_state12.var_9)) ^ (0xc6874e2f));
        rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0x110985f7));
        rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x572f0324)) ? (rb_state19.var_0) : (0x1c4ca4ca));
    #endif
    i = 0;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x699260b0)) {
        if ((rb_state0.var_1) != (0x0)) {
            if (!((rb_state0.var_1) == (rb_state0.var_21))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_14 = (rb_state10.var_14) ^ (0xa5f4385c);
    if ((rb_state10.var_15) == (0xf45314a6)) {
        rb_state10.var_26 = rb_state10.var_16;
    }
    if ((rb_state10.var_15) == (0xf45314a6)) {
        rb_state10.var_27 = 0x5572079b;
        rb_state10.var_28 = 0x6f44f346;
        rb_state10.var_29 = 0x15bdd91a;
        rb_state10.var_30 = (rb_state10.var_30) ^ (rb_state10.var_29);
        rb_state10.var_31 = (rb_state10.var_31) ^ (((0x41c3945f) ^ (rb_state10.var_28)) ^ (rb_state10.var_17));
        rb_state10.var_32 = (rb_state10.var_32) - (((rb_state10.var_18) == (0x0)) ? (rb_state10.var_27) : (0x7075fca1));
        rb_state10.var_33 = (rb_state10.var_33) - (((0x89adf100) ^ (rb_state10.var_19)) ^ (rb_state10.var_30));
        rb_state10.var_34 = (rb_state10.var_34) - (((0x7a450ab0) - (rb_state10.var_31)) + (0x53722ad7));
        rb_state10.var_35 = (rb_state10.var_35) + (((0xc2d94718) + (rb_state10.var_32)) ^ (rb_state10.var_20));
        rb_state10.var_36 = (rb_state10.var_36) ^ (((0x1a600ff4) + (0x5a3adf1f)) + (rb_state10.var_33));
        rb_state10.var_37 = (rb_state10.var_37) ^ (((rb_state10.var_34) == (0x0)) ? (rb_state10.var_35) : (0x195919));
        rb_state10.var_38 = (rb_state10.var_38) + (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_36) : (0xed32391e));
        rb_state10.var_39 = (rb_state10.var_39) - (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_37) : (0x477d1085));
        rb_state10.var_40 = (rb_state10.var_40) + (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_38) : (0x98b65574));
        rb_state10.var_41 = (rb_state10.var_41) ^ (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_39) : (0x4002db59));
        rb_state10.var_42 = (rb_state10.var_42) ^ (((0x86fbee3c) ^ (rb_state10.var_26)) ^ (rb_state10.var_40));
        rb_state10.var_43 = (rb_state10.var_43) ^ (rb_state10.var_41);
        rb_state10.var_44 = (rb_state10.var_44) ^ (((0x4a4c6b93) - (rb_state10.var_42)) - (rb_state10.var_27));
        rb_state10.var_45 = (rb_state10.var_45) + (((rb_state10.var_28) == (0x0)) ? (rb_state10.var_43) : (0xbed0a1c2));
        rb_state10.var_46 = (rb_state10.var_46) - (((0x3594e575) - (rb_state10.var_44)) + (rb_state10.var_29));
        rb_state10.var_47 = (rb_state10.var_47) ^ (((0x290308c9) + (rb_state10.var_45)) + (rb_state10.var_30));
        rb_state10.var_48 = (rb_state10.var_48) - (rb_state10.var_46);
        rb_state10.var_49 = (rb_state10.var_49) + (((rb_state10.var_31) == (0x0)) ? (rb_state10.var_47) : (0x3ff57a23));
        rb_state10.var_50 = (rb_state10.var_50) + (((((0xfe24411a) + (rb_state10.var_32)) ^ (rb_state10.var_33)) ^ (rb_state10.var_48)) - (rb_state10.var_49));
        rb_state10.var_16 = (rb_state10.var_16) - (rb_state10.var_50);
    }
    if ((rb_state10.var_15) == (0xf45314a6)) {
        if (!((rb_state10.var_16) == (rb_state10.var_26))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xc5678247));
    #endif
    xsize = Display.xres / blockx;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - ((0xe873828) - (0xf6e663b1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (439 < rb_input_size ? (uint32_t)rb_input[439] : 0xc30e0020);
    #endif
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_0) == (0x699260b0)) {
                rb_state0.var_8 = (rb_state0.var_8) + (((((0x5608f47c) + (0x1b6e6164)) + (rb_state0.var_6)) ^ (rb_state0.var_8)) + (rb_state0.var_7));
                rb_state0.var_9 = (rb_state0.var_9) - (517 < rb_input_size ? (uint32_t)rb_input[517] : 0x5afde39);
                rb_state0.var_10 = (rb_state0.var_10) - (((((0xddb2772a) - (rb_state0.var_8)) + (rb_state0.var_1)) ^ (rb_state0.var_9)) ^ (0xd0b9de6b));
                rb_state0.var_11 = (rb_state0.var_11) - (0x54daed3b);
                rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0x4cb1e3ce));
                rb_state0.var_13 = (rb_state0.var_13) - (rb_state0.var_10);
                rb_state0.var_14 = (rb_state0.var_14) ^ (rb_state0.var_11);
                rb_state0.var_15 = (rb_state0.var_15) ^ (((((0x36340be1) + (0x3f24c55f)) + (rb_state0.var_11)) ^ (rb_state0.var_12)) - (rb_state0.var_13));
                rb_state0.var_16 = (rb_state0.var_16) + (((0x4136396f) - (rb_state0.var_14)) - (0xafd25a73));
                rb_state0.var_17 = (rb_state0.var_17) + (((0x961130f7) - (rb_state0.var_15)) - (rb_state0.var_12));
                rb_state0.var_18 = (rb_state0.var_18) - (((0xe93de7ae) ^ (rb_state0.var_16)) + (rb_state0.var_13));
                rb_state0.var_19 = (rb_state0.var_19) + (rb_state0.var_17);
                rb_state0.var_20 = (rb_state0.var_20) - (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_19) : (0x7dd7430d));
                rb_state0.var_7 = (rb_state0.var_7) - (rb_state0.var_20);
                rb_state0.var_1 = rb_state0.var_7;
            }
            if ((rb_state0.var_0) == (0x699260b0)) {
                pthread_mutex_lock(&(rb_state0.lock_30));
                rb_state0.var_22 = 0xca0d691e;
                rb_state0.var_23 = 0xd6d2e654;
                rb_state0.var_24 = (rb_state0.var_24) - (375 < rb_input_size ? (uint32_t)rb_input[375] : 0x8aff4502);
                rb_state0.var_25 = (rb_state0.var_25) - (((((0x75bc2132) - (rb_state0.var_22)) + (rb_state0.var_15)) ^ (rb_state0.var_14)) ^ (0xd0c9398e));
                rb_state0.var_26 = (rb_state0.var_26) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_24) : (0x4962e25b));
                rb_state0.var_27 = (rb_state0.var_27) - (((0xc9c23b56) + (rb_state0.var_16)) + (rb_state0.var_25));
                rb_state0.var_28 = (rb_state0.var_28) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_26) : (0x60bd6d35));
                rb_state0.var_29 = (rb_state0.var_29) - (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_28) : (0x8d130b22));
                rb_state0.var_21 = (rb_state0.var_21) + (rb_state0.var_29);
                rb_state0.var_1 = rb_state0.var_21;
                pthread_mutex_unlock(&(rb_state0.lock_30));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (0xdfcd8e29);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
            #endif
            ye = ysize;
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                #ifdef RACEBENCH_BUG_4
                if ((rb_state4.var_0) == (0x0)) {
                    pthread_mutex_lock(&(rb_state4.lock_19));
                    rb_state4.var_3 = 0x7456315c;
                    rb_state4.var_4 = (rb_state4.var_4) + (((0x3ccc55c1) - (rb_state4.var_3)) + (rb_state4.var_3));
                    rb_state4.var_5 = (rb_state4.var_5) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_4) : (0x9f11601b));
                    rb_state4.var_6 = (rb_state4.var_6) ^ (((((0x60675def) + (rb_state4.var_1)) ^ (0x29b3e90)) ^ (rb_state4.var_6)) ^ (rb_state4.var_2));
                    rb_state4.var_7 = (rb_state4.var_7) - (((0x60118825) ^ (rb_state4.var_4)) ^ (0x887c3be6));
                    rb_state4.var_8 = (rb_state4.var_8) - (((0xa4b1bc6d) - (rb_state4.var_5)) ^ (0xe6b04bff));
                    rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_6) : (0x41b048));
                    rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_7) : (0xe9987f69));
                    rb_state4.var_11 = (rb_state4.var_11) ^ (((((0xa9727199) - (rb_state4.var_8)) ^ (0x5d27ac96)) - (0xbe3b113a)) ^ (rb_state4.var_9));
                    rb_state4.var_12 = (rb_state4.var_12) + (((((0xe1bf26fb) + (0xb560e0cd)) - (rb_state4.var_11)) + (rb_state4.var_10)) + (0xf13c76a8));
                    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_12);
                    rb_state4.var_1 = rb_state4.var_2;
                    pthread_mutex_unlock(&(rb_state4.lock_19));
                }
                if ((rb_state4.var_0) == (0x0)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                        racebench_trigger(4);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) ^ (0xf99a355e);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (0xe6ccb384);
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) + ((0xb38a0c2f) ^ (rb_state13.var_0));
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_0 = (rb_state15.var_0) - (357 < rb_input_size ? (uint32_t)rb_input[357] : 0xb4483c68);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) + (0xfe9b852f);
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0x58135f40)) {
                    pthread_mutex_lock(&(rb_state18.lock_39));
                    rb_state18.var_20 = 0x5a54244a;
                    rb_state18.var_21 = (rb_state18.var_21) - (rb_state18.var_12);
                    rb_state18.var_22 = (rb_state18.var_22) ^ (0xddf507b2);
                    rb_state18.var_23 = (rb_state18.var_23) + ((0x88154cb) - (0x5ea974ae));
                    rb_state18.var_24 = (rb_state18.var_24) + (rb_state18.var_10);
                    rb_state18.var_25 = (rb_state18.var_25) + (((0xc7cbaadb) - (0x3e8838da)) ^ (rb_state18.var_11));
                    rb_state18.var_26 = (rb_state18.var_26) + (((((0x22f725) ^ (0x4da7b4f0)) ^ (rb_state18.var_20)) + (rb_state18.var_13)) ^ (rb_state18.var_21));
                    rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_23) : (0x6862d98d));
                    rb_state18.var_28 = (rb_state18.var_28) + (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_24) : (0x1c37dc3f));
                    rb_state18.var_29 = (rb_state18.var_29) + (((0x3f5f0484) ^ (rb_state18.var_25)) - (0x14920527));
                    rb_state18.var_30 = (rb_state18.var_30) + (rb_state18.var_26);
                    rb_state18.var_31 = (rb_state18.var_31) + (((rb_state18.var_27) == (0x0)) ? (rb_state18.var_28) : (0x2be954be));
                    rb_state18.var_32 = (rb_state18.var_32) + (rb_state18.var_29);
                    rb_state18.var_33 = (rb_state18.var_33) - (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_30) : (0x9d20d3ce));
                    rb_state18.var_34 = (rb_state18.var_34) + (((rb_state18.var_31) == (0x0)) ? (rb_state18.var_32) : (0x2047920e));
                    rb_state18.var_35 = (rb_state18.var_35) - (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_33) : (0x79e1a846));
                    rb_state18.var_36 = (rb_state18.var_36) - (rb_state18.var_34);
                    rb_state18.var_37 = (rb_state18.var_37) ^ (((0x1cda0af9) - (rb_state18.var_35)) + (0x654d442f));
                    rb_state18.var_38 = (rb_state18.var_38) + (((rb_state18.var_36) == (0x0)) ? (rb_state18.var_37) : (0xd3bd47ba));
                    rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_38);
                    pthread_mutex_unlock(&(rb_state18.lock_39));
                }
                #endif
                xe = xsize;
            }

            if (i == pid) {
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_0) == (0x173)) {
                    pthread_mutex_lock(&(rb_state6.lock_31));
                    if ((rb_state6.var_1) != (0x0)) {
                        if (!((rb_state6.var_1) == (rb_state6.var_19))) {
                            racebench_trigger(6);
                        }
                    }
                    pthread_mutex_unlock(&(rb_state6.lock_31));
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) + ((0xfd3b8c89) - (0xb98945e4));
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_0) == (0x7ff43843)) {
                    rb_state12.var_2 = rb_state12.var_1;
                }
                if ((rb_state12.var_0) == (0x7ff43843)) {
                    if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                        racebench_trigger(12);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) + (0x540bf6f6);
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_0) == (0x0)) {
                    if ((rb_state19.var_1) != (0x0)) {
                        if (!((rb_state19.var_1) == (rb_state19.var_28))) {
                            racebench_trigger(19);
                        }
                    }
                }
                #endif
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x8be6134b));
            rb_state2.var_0 = (rb_state2.var_0) - ((0x774eb8c2) - (0xbd5f489b));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x9652127d));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (6 < rb_input_size ? (uint32_t)rb_input[6] : 0x14180893);
            #endif
            i = (i + 1) % gm->nprocs;
        }
    }
}