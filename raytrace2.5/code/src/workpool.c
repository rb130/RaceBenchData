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

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0xf96f65c)) {
        pthread_mutex_lock(&(rb_state3.lock_45));
        rb_state3.var_28 = (rb_state3.var_28) ^ (((rb_state3.var_24) == (0x0)) ? (rb_state3.var_23) : (0x77c24fe8));
        rb_state3.var_29 = (rb_state3.var_29) - (((((0xe092c67c) ^ (rb_state3.var_21)) ^ (rb_state3.var_25)) - (rb_state3.var_20)) + (rb_state3.var_26));
        rb_state3.var_30 = (rb_state3.var_30) + (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_28) : (0x3aee8524));
        rb_state3.var_31 = (rb_state3.var_31) - (((((0x8ad4101) ^ (rb_state3.var_29)) ^ (0x4bac2c62)) + (0x1b4c32a5)) ^ (rb_state3.var_30));
        rb_state3.var_27 = (rb_state3.var_27) + (rb_state3.var_31);
        rb_state3.var_5 = rb_state3.var_27;
        pthread_mutex_unlock(&(rb_state3.lock_45));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - ((0x219c19c8) - (0xc9d3db3));
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0x6b41d8ae));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
    #endif
    xb_addr = xs;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0xbc5c16f6) - (rb_state1.var_1));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) ^ (0xd16c8c91);
    if ((rb_state14.var_1) == (0xfffffdd2)) {
        rb_state14.var_5 = rb_state14.var_2;
    }
    if ((rb_state14.var_1) == (0xfffffdd2)) {
        if (!((rb_state14.var_2) == (rb_state14.var_5))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (0xd9efc42b);
    #endif
    yb_addr = ys;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - (0xf2fffdfd);
    rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_4) : (0xcf56efea));
    if ((rb_state3.var_3) == (0xf96f65c)) {
        pthread_mutex_lock(&(rb_state3.lock_45));
        rb_state3.var_33 = 0xd9c8536c;
        rb_state3.var_34 = 0xb156d7a4;
        rb_state3.var_35 = (rb_state3.var_35) ^ (rb_state3.var_28);
        rb_state3.var_36 = (rb_state3.var_36) + (0xcdc72608);
        rb_state3.var_37 = (rb_state3.var_37) ^ (((0xf7025abe) - (0x26c581fa)) ^ (rb_state3.var_34));
        rb_state3.var_38 = (rb_state3.var_38) ^ (rb_state3.var_33);
        rb_state3.var_39 = (rb_state3.var_39) ^ (((0x3ba55722) + (rb_state3.var_29)) ^ (rb_state3.var_27));
        rb_state3.var_40 = (rb_state3.var_40) ^ (((((0x17c09358) + (rb_state3.var_35)) ^ (rb_state3.var_36)) - (rb_state3.var_30)) ^ (0xd6d2c564));
        rb_state3.var_41 = (rb_state3.var_41) + (((rb_state3.var_37) == (0x0)) ? (rb_state3.var_38) : (0xe28f06a1));
        rb_state3.var_42 = (rb_state3.var_42) + (((((0xa18ebce5) ^ (rb_state3.var_39)) ^ (rb_state3.var_40)) ^ (rb_state3.var_32)) + (rb_state3.var_31));
        rb_state3.var_43 = (rb_state3.var_43) - (((rb_state3.var_33) == (0x0)) ? (rb_state3.var_41) : (0xace91c4c));
        rb_state3.var_44 = (rb_state3.var_44) - (((rb_state3.var_42) == (0x0)) ? (rb_state3.var_43) : (0xba22bf59));
        rb_state3.var_32 = (rb_state3.var_32) + (rb_state3.var_44);
        rb_state3.var_5 = rb_state3.var_32;
        pthread_mutex_unlock(&(rb_state3.lock_45));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (0x27521f46);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x809009ab)) {
        rb_state9.var_16 = 0x5c1189c6;
        rb_state9.var_17 = (rb_state9.var_17) - (((0xa7374123) + (rb_state9.var_12)) + (rb_state9.var_11));
        rb_state9.var_18 = (rb_state9.var_18) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_16) : (0xb88a3bf6));
        rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_10) : (0xc292baa7));
        rb_state9.var_20 = (rb_state9.var_20) - (((0x90ba2399) ^ (rb_state9.var_17)) ^ (0x17cc1103));
        rb_state9.var_21 = (rb_state9.var_21) + (((0x4c84d3d1) ^ (rb_state9.var_18)) - (rb_state9.var_15));
        rb_state9.var_22 = (rb_state9.var_22) + (rb_state9.var_19);
        rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_21) : (0x90a77fb2));
        rb_state9.var_24 = (rb_state9.var_24) - (rb_state9.var_22);
        rb_state9.var_25 = (rb_state9.var_25) + (((((0xfb3660da) - (rb_state9.var_24)) + (0xf7f1462a)) ^ (rb_state9.var_23)) + (0x4a396c50));
        rb_state9.var_4 = (rb_state9.var_4) + (rb_state9.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (253 < rb_input_size ? (uint32_t)rb_input[253] : 0x56d86c95);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (0x1be0eabe);
    if ((rb_state15.var_1) == (0xa34590d)) {
        pthread_mutex_lock(&(rb_state15.lock_40));
        rb_state15.var_7 = 0x2cdcee84;
        rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_7) : (0xdbcc4832));
        rb_state15.var_9 = (rb_state15.var_9) - (((0x9e0cff9b) - (rb_state15.var_5)) - (rb_state15.var_9));
        rb_state15.var_10 = (rb_state15.var_10) ^ (((0x60a7263c) - (rb_state15.var_10)) + (rb_state15.var_6));
        rb_state15.var_11 = (rb_state15.var_11) - (((0xd36a1e48) ^ (0x9e0bc205)) - (rb_state15.var_7));
        rb_state15.var_12 = (rb_state15.var_12) ^ (((0xb7b6be6d) ^ (rb_state15.var_8)) ^ (0xbb4fbbd3));
        rb_state15.var_13 = (rb_state15.var_13) ^ (rb_state15.var_9);
        rb_state15.var_14 = (rb_state15.var_14) ^ (((0x58a66dbc) - (rb_state15.var_10)) ^ (rb_state15.var_11));
        rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_11) : (0x430da9b9));
        rb_state15.var_16 = (rb_state15.var_16) - (((0x2d55ba48) ^ (rb_state15.var_12)) - (0xf391d4d3));
        rb_state15.var_17 = (rb_state15.var_17) - (rb_state15.var_13);
        rb_state15.var_18 = (rb_state15.var_18) ^ (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_15) : (0xaabfcd17));
        rb_state15.var_19 = (rb_state15.var_19) ^ (rb_state15.var_16);
        rb_state15.var_20 = (rb_state15.var_20) ^ (rb_state15.var_17);
        rb_state15.var_21 = (rb_state15.var_21) - (((((0x91732d73) ^ (rb_state15.var_18)) - (0x117e9e75)) ^ (0xb7542e8a)) ^ (rb_state15.var_19));
        rb_state15.var_22 = (rb_state15.var_22) ^ (rb_state15.var_20);
        rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_21) == (0x0)) ? (rb_state15.var_22) : (0x9863cf5e));
        rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_23);
        rb_state15.var_5 = rb_state15.var_6;
        pthread_mutex_unlock(&(rb_state15.lock_40));
    }
    if ((rb_state15.var_1) == (0xa34590d)) {
        if (!((rb_state15.var_5) == (rb_state15.var_6))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_4 = (rb_state17.var_4) - (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_4) : (0x2bb9f6b1));
    #endif
    xb_end = xb_addr + xe - 1;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_3);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) ^ (0x3fa62ee0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - ((0x5930e066) - (0x4bb83b48));
    rb_state18.var_3 = (rb_state18.var_3) + ((0x85fc9cb7) + (0xf78b42e7));
    #endif
    yb_end = yb_addr + ye - 1;

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_3 = (rb_state2.var_3) + (0x23a15d02);
                if ((rb_state2.var_4) == (0x7fb77d14)) {
                    rb_state2.var_23 = rb_state2.var_9;
                }
                if ((rb_state2.var_4) == (0x7fb77d14)) {
                    if (!((rb_state2.var_9) == (rb_state2.var_23))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_1) == (0xdcd468ec)) {
                    rb_state10.var_19 = 0xdb6b2e29;
                    rb_state10.var_20 = 0x52db574;
                    rb_state10.var_21 = 0x6a8e0122;
                    rb_state10.var_22 = (rb_state10.var_22) + (287 < rb_input_size ? (uint32_t)rb_input[287] : 0xf8629662);
                    rb_state10.var_23 = (rb_state10.var_23) ^ (((0x64e89d5) + (rb_state10.var_19)) ^ (rb_state10.var_11));
                    rb_state10.var_24 = (rb_state10.var_24) + (rb_state10.var_21);
                    rb_state10.var_25 = (rb_state10.var_25) ^ (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_22) : (0x9eb9c2e));
                    rb_state10.var_26 = (rb_state10.var_26) + (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_24) : (0x4417bd7));
                    rb_state10.var_27 = (rb_state10.var_27) + (((0x25b6fbf9) ^ (rb_state10.var_25)) + (rb_state10.var_12));
                    rb_state10.var_28 = (rb_state10.var_28) ^ (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_27) : (0x583ce092));
                    rb_state10.var_4 = (rb_state10.var_4) + (rb_state10.var_28);
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_4 = (rb_state13.var_4) ^ (0x484095f9);
                #endif
                xb_size = xbe;
            } else {
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                #ifdef RACEBENCH_BUG_1
                if ((rb_state1.var_0) == (0x585eb5d3)) {
                    rb_state1.var_17 = 0x8cb2d9b5;
                    rb_state1.var_18 = (rb_state1.var_18) - (((0x7e9911b3) - (rb_state1.var_6)) ^ (rb_state1.var_7));
                    rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_5) : (0x3ce159c1));
                    rb_state1.var_20 = (rb_state1.var_20) - (rb_state1.var_18);
                    rb_state1.var_21 = (rb_state1.var_21) ^ (((0x3b6db2b9) ^ (0x2b9730da)) ^ (rb_state1.var_19));
                    rb_state1.var_22 = (rb_state1.var_22) + (rb_state1.var_20);
                    rb_state1.var_23 = (rb_state1.var_23) + (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_21) : (0x222903a4));
                    rb_state1.var_24 = (rb_state1.var_24) + (rb_state1.var_22);
                    rb_state1.var_25 = (rb_state1.var_25) ^ (((0xf2f8c387) + (rb_state1.var_9)) - (rb_state1.var_23));
                    rb_state1.var_26 = (rb_state1.var_26) ^ (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_24) : (0xf855537f));
                    rb_state1.var_27 = (rb_state1.var_27) - (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_25) : (0x70f2e9f6));
                    rb_state1.var_28 = (rb_state1.var_28) ^ (((rb_state1.var_26) == (0x0)) ? (rb_state1.var_27) : (0x7a775b4b));
                    rb_state1.var_16 = (rb_state1.var_16) ^ (rb_state1.var_28);
                    rb_state1.var_4 = rb_state1.var_16;
                }
                if ((rb_state1.var_0) == (0x585eb5d3)) {
                    pthread_mutex_lock(&(rb_state1.lock_37));
                    rb_state1.var_30 = 0xb62e9d76;
                    rb_state1.var_31 = (rb_state1.var_31) ^ (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_30) : (0xda618084));
                    rb_state1.var_32 = (rb_state1.var_32) + (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_13) : (0xd42443ef));
                    rb_state1.var_33 = (rb_state1.var_33) - ((0xd76ac396) + (0xcb6adc10));
                    rb_state1.var_34 = (rb_state1.var_34) ^ (rb_state1.var_31);
                    rb_state1.var_35 = (rb_state1.var_35) - (((rb_state1.var_32) == (0x0)) ? (rb_state1.var_33) : (0x832c3f6));
                    rb_state1.var_36 = (rb_state1.var_36) + (((((0x4c27bc23) - (rb_state1.var_35)) - (rb_state1.var_15)) + (rb_state1.var_34)) + (rb_state1.var_16));
                    rb_state1.var_29 = (rb_state1.var_29) + (rb_state1.var_36);
                    rb_state1.var_4 = rb_state1.var_29;
                    pthread_mutex_unlock(&(rb_state1.lock_37));
                }
                #endif
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_3) == (0xf96f65c)) {
                    if ((rb_state3.var_5) != (0x0)) {
                        if (!((rb_state3.var_5) == (rb_state3.var_32))) {
                            racebench_trigger(3);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_3 = (rb_state13.var_3) ^ (0x57b464e8);
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_4 = (rb_state18.var_4) - (0xe2e39909);
                #endif
                yb_size = ybe;
            } else {
                yb_size = yb_end - yb_addr + 1;
            }

            #ifdef RACEBENCH_BUG_1
            rb_state1.var_3 = (rb_state1.var_3) - ((0x91e359cc) - (0x14e554a6));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_1) : (0xa464c497));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_2);
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0xc1b6a898)) {
                rb_state8.var_6 = 0x3f64f241;
                rb_state8.var_7 = 0xd2e11747;
                rb_state8.var_8 = (rb_state8.var_8) ^ (0x97d2141);
                rb_state8.var_9 = (rb_state8.var_9) + (((((0x3a031755) - (0x5c382fb8)) + (rb_state8.var_6)) + (rb_state8.var_5)) + (0x5b24d76a));
                rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0x678f0314));
                rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_9);
                rb_state8.var_12 = (rb_state8.var_12) + (((0xafd92b97) ^ (rb_state8.var_10)) + (0x4377961d));
                rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x1a08fa26));
                rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_13);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_2) == (0xa2fa7713)) ? (rb_state10.var_1) : (0xf0552246));
            rb_state10.var_1 = (rb_state10.var_1) + ((0x4142c0e6) + (rb_state10.var_2));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) - ((0x9f2b05ea) - (0x39b185b7));
            if ((rb_state13.var_3) == (0x801a0034)) {
                rb_state13.var_8 = 0xca61271f;
                rb_state13.var_9 = (rb_state13.var_9) - (rb_state13.var_7);
                rb_state13.var_10 = (rb_state13.var_10) - (0x3ac28316);
                rb_state13.var_11 = (rb_state13.var_11) ^ (46 < rb_input_size ? (uint32_t)rb_input[46] : 0xb6d8a6f8);
                rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0x9663ed61));
                rb_state13.var_13 = (rb_state13.var_13) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_10) : (0x20ef427d));
                rb_state13.var_14 = (rb_state13.var_14) ^ (rb_state13.var_11);
                rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_12);
                rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_14) : (0x242eacc2));
                rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_15) : (0x83370404));
                rb_state13.var_18 = (rb_state13.var_18) ^ (((0x77aaf428) ^ (rb_state13.var_16)) - (0x910ce7d4));
                rb_state13.var_19 = (rb_state13.var_19) + (((((0x6709b854) + (rb_state13.var_17)) + (0x3e85d5c6)) - (rb_state13.var_18)) ^ (rb_state13.var_11));
                rb_state13.var_7 = (rb_state13.var_7) + (rb_state13.var_19);
                rb_state13.var_5 = rb_state13.var_7;
            }
            if ((rb_state13.var_3) == (0x801a0034)) {
                pthread_mutex_lock(&(rb_state13.lock_28));
                rb_state13.var_21 = (rb_state13.var_21) - (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_15) : (0xfab1891e));
                rb_state13.var_22 = (rb_state13.var_22) + ((0xc3a409c6) + (0x207399f3));
                rb_state13.var_23 = (rb_state13.var_23) - (((((0x3f0b35e1) - (rb_state13.var_14)) - (0x9c4bd65e)) - (rb_state13.var_13)) ^ (0x44cfbdc8));
                rb_state13.var_24 = (rb_state13.var_24) - (((((0xe2a9d835) ^ (rb_state13.var_21)) + (rb_state13.var_12)) - (0x72f66fe)) ^ (0x616cbc5b));
                rb_state13.var_25 = (rb_state13.var_25) - (((((0xbee7ad02) + (rb_state13.var_22)) ^ (rb_state13.var_23)) ^ (rb_state13.var_17)) ^ (rb_state13.var_18));
                rb_state13.var_26 = (rb_state13.var_26) ^ (((0xba6b8763) ^ (0x3dbec720)) ^ (rb_state13.var_24));
                rb_state13.var_27 = (rb_state13.var_27) - (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_26) : (0x4a37785a));
                rb_state13.var_20 = (rb_state13.var_20) ^ (rb_state13.var_27);
                rb_state13.var_5 = rb_state13.var_20;
                pthread_mutex_unlock(&(rb_state13.lock_28));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) - (464 < rb_input_size ? (uint32_t)rb_input[464] : 0xcd0c4089);
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_4) == (0x94b5e476)) {
                rb_state18.var_5 = rb_state18.var_0;
            }
            if ((rb_state18.var_4) == (0x94b5e476)) {
                if (!((rb_state18.var_0) == (rb_state18.var_5))) {
                    racebench_trigger(18);
                }
            }
            #endif
            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c");

            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_4) == (0x2048e89)) {
                rb_state19.var_5 = rb_state19.var_2;
            }
            if ((rb_state19.var_4) == (0x2048e89)) {
                if (!((rb_state19.var_2) == (rb_state19.var_5))) {
                    racebench_trigger(19);
                }
            }
            #endif
            wpentry->xpix = xb_addr;
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_3 = (rb_state8.var_3) + ((0x703f1833) + (rb_state8.var_3));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_2);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_3 = (rb_state19.var_3) - (313 < rb_input_size ? (uint32_t)rb_input[313] : 0x8b8cdc5b);
            #endif
            wpentry->ypix = yb_addr;
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) ^ (0x831c59cc);
            if ((rb_state2.var_4) == (0x98825c3c)) {
                pthread_mutex_lock(&(rb_state2.lock_21));
                rb_state2.var_7 = 0x8722f0cf;
                rb_state2.var_8 = 0xee72169c;
                rb_state2.var_9 = (rb_state2.var_9) - (rb_state2.var_6);
                rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_7) : (0xa4f39d02));
                rb_state2.var_11 = (rb_state2.var_11) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_7) : (0x6ce0f7f3));
                rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_5) : (0x10f3260b));
                rb_state2.var_13 = (rb_state2.var_13) ^ (((0x34bd6d00) - (0x2b791d73)) - (rb_state2.var_9));
                rb_state2.var_14 = (rb_state2.var_14) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0x2be30ceb));
                rb_state2.var_15 = (rb_state2.var_15) - (((0x2bbccd4a) + (rb_state2.var_12)) ^ (rb_state2.var_10));
                rb_state2.var_16 = (rb_state2.var_16) - (((((0x5ee60c50) ^ (rb_state2.var_14)) ^ (rb_state2.var_13)) + (rb_state2.var_11)) - (0xf13999d2));
                rb_state2.var_17 = (rb_state2.var_17) + (((((0x450362bb) + (rb_state2.var_16)) + (rb_state2.var_15)) ^ (0x8c46525)) + (0x8d401c82));
                rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_17);
                rb_state2.var_5 = rb_state2.var_6;
                pthread_mutex_unlock(&(rb_state2.lock_21));
            }
            if ((rb_state2.var_4) == (0x98825c3c)) {
                rb_state2.var_22 = !((rb_state2.var_5) == (rb_state2.var_6));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_3 = (rb_state4.var_3) + ((0xc16bcfc9) + (0xbcda3bbe));
            rb_state4.var_4 = (rb_state4.var_4) - (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_4) : (0xc593d3ae));
            if ((rb_state4.var_1) == (0x3ded8b13)) {
                pthread_mutex_lock(&(rb_state4.lock_43));
                rb_state4.var_7 = 0xcd44d57f;
                rb_state4.var_8 = 0x3da9c9d9;
                rb_state4.var_9 = (rb_state4.var_9) + (rb_state4.var_6);
                rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_7);
                rb_state4.var_11 = (rb_state4.var_11) ^ (((0x9491a644) ^ (rb_state4.var_8)) + (rb_state4.var_5));
                rb_state4.var_12 = (rb_state4.var_12) + (((0x7b27f4c0) + (rb_state4.var_8)) - (0x62e6e473));
                rb_state4.var_13 = (rb_state4.var_13) + (rb_state4.var_7);
                rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0x6a1a64a9));
                rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_11);
                rb_state4.var_16 = (rb_state4.var_16) ^ (((((0x74863caa) + (rb_state4.var_13)) ^ (rb_state4.var_12)) + (0x8bbc0733)) + (0xeda90a9a));
                rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_14) : (0xa054d5e5));
                rb_state4.var_18 = (rb_state4.var_18) + (((0x39e942e5) ^ (rb_state4.var_10)) ^ (rb_state4.var_15));
                rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_16) : (0x472f1300));
                rb_state4.var_20 = (rb_state4.var_20) + (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_18) : (0xe645c447));
                rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_19);
                rb_state4.var_22 = (rb_state4.var_22) + (((0xeb21d071) - (0x24e0a0c7)) + (rb_state4.var_20));
                rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_21) : (0x2fe25b));
                rb_state4.var_24 = (rb_state4.var_24) ^ (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_23) : (0xb7704e19));
                rb_state4.var_6 = (rb_state4.var_6) ^ (rb_state4.var_24);
                rb_state4.var_5 = rb_state4.var_6;
                pthread_mutex_unlock(&(rb_state4.lock_43));
            }
            if ((rb_state4.var_1) == (0x3ded8b13)) {
                if (!((rb_state4.var_5) == (rb_state4.var_6))) {
                    racebench_trigger(4);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_4 = (rb_state11.var_4) - ((0x26a07c76) ^ (rb_state11.var_1));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
            #endif
            wpentry->xdim = xb_size;
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_4 = (rb_state2.var_4) + ((0xbaf595d) - (0x1814c8f1));
            rb_state2.var_14 = (rb_state2.var_14) - ((0x961042de) ^ (0xa2c271fc));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0xe39964f3));
            #endif
            wpentry->ydim = yb_size;

            if (!gm->workpool[pid][0]) {
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_2 = (rb_state10.var_2) - ((0xd54ee76b) ^ (0xe41ceada));
                #endif
                wpentry->next = NULL;
            } else {
                wpentry->next = gm->workpool[pid][0];
            }

            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (0xe765a4d7);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_3 = (rb_state2.var_3) + ((0x3dfdb679) - (0xfa0bac82));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) ^ (0x182b7168);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_4 = (rb_state19.var_4) + ((0xac2f83) + (rb_state19.var_4));
            #endif
            gm->workpool[pid][0] = wpentry;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (288 < rb_input_size ? (uint32_t)rb_input[288] : 0x816bc2a);
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_4) == (0x98825c3c)) {
                rb_state2.var_18 = 0xae889ab4;
                rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_12) : (0x8cbe05a6));
                rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_19) : (0x5682bfcb));
                rb_state2.var_5 = (rb_state2.var_5) + (rb_state2.var_20);
            }
            rb_state2.var_15 = (rb_state2.var_15) - (195 < rb_input_size ? (uint32_t)rb_input[195] : 0x960adb6b);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (0x8b025142);
            #endif
            xb_addr += xbe;
        }

        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (434 < rb_input_size ? (uint32_t)rb_input[434] : 0x9224248c);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_2) : (0x9493780e));
        #endif
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
    rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_4) : (0x133ac4df));
    if ((rb_state0.var_0) == (0xffffff2c)) {
        rb_state0.var_18 = 0xdc6811e5;
        rb_state0.var_19 = 0xf5ba95e6;
        rb_state0.var_20 = (rb_state0.var_20) + (((0xcf69abe4) ^ (rb_state0.var_18)) - (0x1e2e0bb3));
        rb_state0.var_21 = (rb_state0.var_21) ^ (((((0x70974f7a) - (rb_state0.var_19)) - (rb_state0.var_9)) ^ (rb_state0.var_10)) - (rb_state0.var_11));
        rb_state0.var_22 = (rb_state0.var_22) + (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xf7c81a17));
        rb_state0.var_5 = (rb_state0.var_5) ^ (rb_state0.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) + (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0x3e70e5b8));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ ((0x9fda84b6) - (rb_state6.var_2));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (425 < rb_input_size ? (uint32_t)rb_input[425] : 0xbb3852c1);
    rb_state7.var_0 = (rb_state7.var_0) ^ (0x738b405a);
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_2) : (0x25008ce1));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) ^ ((0x8eab477c) + (rb_state15.var_4));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (313 < rb_input_size ? (uint32_t)rb_input[313] : 0x83a03eb8);
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (0x94f696c0);
    #endif
    gm->wpstat[pid][0] = WPS_VALID;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) - (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x23196dfb));
    #endif
    gm->workpool[pid][0] = NULL;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (62 < rb_input_size ? (uint32_t)rb_input[62] : 0xcf9e527e);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) - (0x8549fa6b);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
    rb_state7.var_1 = (rb_state7.var_1) + (0xd5f52e94);
    rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_1) == (0xd5f52e94)) ? (rb_state7.var_2) : (0xe167d30e));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (0x4dc9410d);
    #endif
    i = 0;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) + (0x83e5bdaf);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0xddca309e));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) ^ (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_4) : (0xd2cbb1b7));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_1) : (0xfb078b19));
    if ((rb_state15.var_1) == (0xa34590d)) {
        rb_state15.var_24 = 0x87e70c9;
        rb_state15.var_25 = (rb_state15.var_25) - (rb_state15.var_15);
        rb_state15.var_26 = (rb_state15.var_26) - (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_13) : (0x554fabe1));
        rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_24) : (0x232811f3));
        rb_state15.var_28 = (rb_state15.var_28) + (((0x74ee7670) ^ (0x6039ec89)) + (rb_state15.var_25));
        rb_state15.var_29 = (rb_state15.var_29) - (((0xb18c349) + (rb_state15.var_17)) + (rb_state15.var_26));
        rb_state15.var_30 = (rb_state15.var_30) ^ (((0x3b73a983) - (rb_state15.var_18)) ^ (rb_state15.var_27));
        rb_state15.var_31 = (rb_state15.var_31) ^ (((((0x7372fb56) + (0x9fc757d5)) + (0x8ca5f821)) - (rb_state15.var_28)) ^ (rb_state15.var_29));
        rb_state15.var_32 = (rb_state15.var_32) ^ (rb_state15.var_30);
        rb_state15.var_33 = (rb_state15.var_33) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_31) : (0x826c6293));
        rb_state15.var_34 = (rb_state15.var_34) - (((0x8cf7c7d7) ^ (rb_state15.var_32)) + (rb_state15.var_20));
        rb_state15.var_35 = (rb_state15.var_35) ^ (rb_state15.var_33);
        rb_state15.var_36 = (rb_state15.var_36) + (((0x3e695271) + (0x640ad34b)) + (rb_state15.var_34));
        rb_state15.var_37 = (rb_state15.var_37) + (((0x3e0f478b) ^ (rb_state15.var_35)) + (rb_state15.var_21));
        rb_state15.var_38 = (rb_state15.var_38) ^ (rb_state15.var_36);
        rb_state15.var_39 = (rb_state15.var_39) ^ (((((0xb2678694) - (0x9acffd)) ^ (rb_state15.var_38)) - (0xf31c2e31)) - (rb_state15.var_37));
        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_39);
    }
    #endif
    xsize = Display.xres / blockx;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_4) : (0x99e00ad2));
    rb_state5.var_1 = (rb_state5.var_1) - (331 < rb_input_size ? (uint32_t)rb_input[331] : 0x85bb9d3);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_3);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (38 < rb_input_size ? (uint32_t)rb_input[38] : 0x4dc01474);
    if ((rb_state7.var_2) == (0xdebe9782)) {
        pthread_mutex_lock(&(rb_state7.lock_13));
        rb_state7.var_5 = 0xecedce73;
        rb_state7.var_6 = 0xca481d22;
        rb_state7.var_7 = 0x9c8a75f3;
        rb_state7.var_8 = (rb_state7.var_8) ^ (((((0x55b760e5) + (rb_state7.var_4)) - (rb_state7.var_7)) - (rb_state7.var_3)) ^ (rb_state7.var_5));
        rb_state7.var_9 = (rb_state7.var_9) + (((((0xf32593cf) - (rb_state7.var_5)) + (rb_state7.var_6)) - (0x4d1e18a6)) - (rb_state7.var_8));
        rb_state7.var_4 = (rb_state7.var_4) + (rb_state7.var_9);
        rb_state7.var_3 = rb_state7.var_4;
        pthread_mutex_unlock(&(rb_state7.lock_13));
    }
    if ((rb_state7.var_2) == (0xdebe9782)) {
        if (!((rb_state7.var_3) == (rb_state7.var_4))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_4 = (rb_state18.var_4) ^ (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_2) : (0x3511dde4));
    #endif
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_2);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x2d027cf4));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) + (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_2) : (0xffd74780));
            rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_3);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) + ((0x49ac9d76) - (0x834b6f48));
            rb_state12.var_3 = (rb_state12.var_3) - (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_0) : (0x89de038));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_1);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_1);
            #endif
            ye = ysize;
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_4);
                rb_state3.var_1 = (rb_state3.var_1) ^ ((0x941d2f41) + (rb_state3.var_2));
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x3b209817));
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_4 = (rb_state6.var_4) + (0x41dc123a);
                if ((rb_state6.var_4) == (0x41dc123a)) {
                    pthread_mutex_lock(&(rb_state6.lock_21));
                    rb_state6.var_7 = 0xa593076a;
                    rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_5);
                    rb_state6.var_9 = (rb_state6.var_9) - (((0x21f039ae) ^ (0x275c6590)) - (rb_state6.var_7));
                    rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_7) == (0xa593076a)) ? (rb_state6.var_6) : (0x6cb2e4fa));
                    rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_8) : (0x10ba038c));
                    rb_state6.var_12 = (rb_state6.var_12) ^ (((rb_state6.var_9) == (0x9ee6ab2c)) ? (rb_state6.var_9) : (0xb825bf));
                    rb_state6.var_13 = (rb_state6.var_13) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0xd2053c2c));
                    rb_state6.var_14 = (rb_state6.var_14) ^ (((((0x9499fb56) ^ (rb_state6.var_12)) - (0x6a14132)) - (rb_state6.var_13)) + (0x10653eb8));
                    rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_14);
                    rb_state6.var_5 = rb_state6.var_6;
                    pthread_mutex_unlock(&(rb_state6.lock_21));
                }
                if ((rb_state6.var_4) == (0x41dc123a)) {
                    pthread_mutex_lock(&(rb_state6.lock_21));
                    if (!((rb_state6.var_5) == (rb_state6.var_6))) {
                        racebench_trigger(6);
                    }
                    pthread_mutex_unlock(&(rb_state6.lock_21));
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_2 = (rb_state7.var_2) + (30 < rb_input_size ? (uint32_t)rb_input[30] : 0xebe889bd);
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_3 = (rb_state8.var_3) + (324 < rb_input_size ? (uint32_t)rb_input[324] : 0xd5755c57);
                rb_state8.var_4 = (rb_state8.var_4) ^ ((0x26370e2b) - (rb_state8.var_4));
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_1 = (rb_state9.var_1) - ((0xf0976c52) ^ (rb_state9.var_1));
                rb_state9.var_3 = (rb_state9.var_3) ^ (0xbf173e5d);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_2 = (rb_state10.var_2) ^ (0xa2fa7713);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_2 = (rb_state12.var_2) + (10 < rb_input_size ? (uint32_t)rb_input[10] : 0x136d6b01);
                rb_state12.var_0 = (rb_state12.var_0) ^ ((0x46c4d2db) + (rb_state12.var_1));
                if ((rb_state12.var_2) == (0xcf83a1b9)) {
                    rb_state12.var_4 = rb_state12.var_3;
                }
                if ((rb_state12.var_2) == (0xcf83a1b9)) {
                    pthread_mutex_lock(&(rb_state12.lock_20));
                    rb_state12.var_5 = 0x2bdc3539;
                    rb_state12.var_6 = 0xf4c1562e;
                    rb_state12.var_7 = (rb_state12.var_7) + (0xcc6fdb60);
                    rb_state12.var_8 = (rb_state12.var_8) - (60 < rb_input_size ? (uint32_t)rb_input[60] : 0xa2783bab);
                    rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_5);
                    rb_state12.var_10 = (rb_state12.var_10) ^ (rb_state12.var_6);
                    rb_state12.var_11 = (rb_state12.var_11) - (rb_state12.var_4);
                    rb_state12.var_12 = (rb_state12.var_12) ^ (((((0xef597b6e) + (rb_state12.var_8)) - (rb_state12.var_6)) + (rb_state12.var_7)) + (rb_state12.var_5));
                    rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0x89734fd0));
                    rb_state12.var_14 = (rb_state12.var_14) - (((0x530702ca) + (rb_state12.var_11)) + (rb_state12.var_7));
                    rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_12) : (0xa27f9840));
                    rb_state12.var_16 = (rb_state12.var_16) ^ (((0xc13c3714) ^ (rb_state12.var_9)) + (rb_state12.var_13));
                    rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_14);
                    rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0x7aa95cc4));
                    rb_state12.var_19 = (rb_state12.var_19) - (((((0x15450de4) ^ (0xf2b5a672)) + (rb_state12.var_18)) - (rb_state12.var_17)) - (rb_state12.var_10));
                    rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_19);
                    pthread_mutex_unlock(&(rb_state12.lock_20));
                }
                if ((rb_state12.var_2) == (0xcf83a1b9)) {
                    pthread_mutex_lock(&(rb_state12.lock_20));
                    if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                        racebench_trigger(12);
                    }
                    pthread_mutex_unlock(&(rb_state12.lock_20));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_3) == (0x801a0034)) {
                    pthread_mutex_lock(&(rb_state13.lock_28));
                    if ((rb_state13.var_5) != (0x0)) {
                        if (!((rb_state13.var_5) == (rb_state13.var_20))) {
                            racebench_trigger(13);
                        }
                    }
                    pthread_mutex_unlock(&(rb_state13.lock_28));
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) + (68 < rb_input_size ? (uint32_t)rb_input[68] : 0xfdcd2536);
                rb_state14.var_4 = (rb_state14.var_4) - (0xc65a80a9);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) - ((0x8718d582) + (rb_state15.var_3));
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_4 = (rb_state17.var_4) ^ ((0x4b31be35) - (0xd1647357));
                rb_state17.var_0 = (rb_state17.var_0) - ((0xe3ef0bab) ^ (rb_state17.var_1));
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0xa5d115ec));
                #endif
                xe = xsize;
            }

            if (i == pid) {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_2 = (rb_state1.var_2) - ((0xd214e78) ^ (rb_state1.var_3));
                #endif
                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_3) == (0xf16105d8)) {
                    pthread_mutex_lock(&(rb_state5.lock_20));
                    rb_state5.var_6 = 0xffe65005;
                    rb_state5.var_7 = 0x4f8c1f3;
                    rb_state5.var_8 = (rb_state5.var_8) - (((0x100ba02a) ^ (rb_state5.var_6)) ^ (rb_state5.var_5));
                    rb_state5.var_9 = (rb_state5.var_9) + (((0xdaa51551) ^ (rb_state5.var_7)) ^ (rb_state5.var_7));
                    rb_state5.var_10 = (rb_state5.var_10) ^ (rb_state5.var_6);
                    rb_state5.var_11 = (rb_state5.var_11) - (((0x63010185) ^ (rb_state5.var_8)) + (rb_state5.var_8));
                    rb_state5.var_12 = (rb_state5.var_12) ^ (rb_state5.var_9);
                    rb_state5.var_13 = (rb_state5.var_13) ^ (((0xa1bc3fdf) - (0xb45e7671)) ^ (rb_state5.var_10));
                    rb_state5.var_14 = (rb_state5.var_14) + (((rb_state5.var_9) == (0xdaa51551)) ? (rb_state5.var_11) : (0x1227477a));
                    rb_state5.var_15 = (rb_state5.var_15) + (((0xaf9f75ad) ^ (rb_state5.var_12)) ^ (rb_state5.var_10));
                    rb_state5.var_16 = (rb_state5.var_16) - (((0x8706bd9f) - (rb_state5.var_11)) ^ (rb_state5.var_13));
                    rb_state5.var_17 = (rb_state5.var_17) + (rb_state5.var_14);
                    rb_state5.var_18 = (rb_state5.var_18) ^ (((((0x89c5c542) + (rb_state5.var_12)) ^ (rb_state5.var_16)) - (rb_state5.var_15)) - (0xab27d1b6));
                    rb_state5.var_19 = (rb_state5.var_19) + (((((0xb51b97ae) + (rb_state5.var_17)) + (rb_state5.var_18)) + (rb_state5.var_14)) ^ (rb_state5.var_13));
                    rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_19);
                    pthread_mutex_unlock(&(rb_state5.lock_20));
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                if ((rb_state7.var_2) == (0xdebe9782)) {
                    rb_state7.var_10 = 0x7145dd40;
                    rb_state7.var_11 = (rb_state7.var_11) - (((((0x8977a2cb) ^ (rb_state7.var_10)) ^ (rb_state7.var_8)) - (0x26483ad)) - (rb_state7.var_7));
                    rb_state7.var_12 = (rb_state7.var_12) - (((((0x4e429db5) - (0x493a549a)) + (rb_state7.var_9)) + (rb_state7.var_6)) ^ (rb_state7.var_11));
                    rb_state7.var_3 = (rb_state7.var_3) ^ (rb_state7.var_12);
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_4 = (rb_state15.var_4) + (0x725a6d38);
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_4 = (rb_state18.var_4) ^ ((0xc5594b8f) ^ (rb_state18.var_2));
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_2);
                #endif
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            #ifdef RACEBENCH_BUG_1
            rb_state1.var_3 = (rb_state1.var_3) + ((0x1144fd4) - (rb_state1.var_3));
            #endif
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_1) == (0x3ded8b13)) {
                pthread_mutex_lock(&(rb_state4.lock_43));
                rb_state4.var_25 = 0x77418202;
                rb_state4.var_26 = (rb_state4.var_26) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_14) : (0x358a68c2));
                rb_state4.var_27 = (rb_state4.var_27) ^ ((0xc50fa89a) + (0xd34c9e84));
                rb_state4.var_28 = (rb_state4.var_28) ^ ((0xbfa645bd) ^ (rb_state4.var_16));
                rb_state4.var_29 = (rb_state4.var_29) + (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_13) : (0x6d32f2a8));
                rb_state4.var_30 = (rb_state4.var_30) - (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_26) : (0x8177e476));
                rb_state4.var_31 = (rb_state4.var_31) + (((0xdee7ef3f) + (rb_state4.var_27)) - (0xf8a73384));
                rb_state4.var_32 = (rb_state4.var_32) - (rb_state4.var_28);
                rb_state4.var_33 = (rb_state4.var_33) - (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_29) : (0x17f086a4));
                rb_state4.var_34 = (rb_state4.var_34) - (((0x41196017) ^ (rb_state4.var_19)) ^ (rb_state4.var_30));
                rb_state4.var_35 = (rb_state4.var_35) + (((0x2603d997) - (rb_state4.var_31)) ^ (0x29eb9bf4));
                rb_state4.var_36 = (rb_state4.var_36) - (((((0x45c5d566) ^ (rb_state4.var_33)) ^ (0xa5de3f44)) + (rb_state4.var_32)) ^ (0xf0bf8bb9));
                rb_state4.var_37 = (rb_state4.var_37) - (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_34) : (0x90af9168));
                rb_state4.var_38 = (rb_state4.var_38) ^ (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_35) : (0x53077d53));
                rb_state4.var_39 = (rb_state4.var_39) - (rb_state4.var_36);
                rb_state4.var_40 = (rb_state4.var_40) - (((((0x11e52db9) - (rb_state4.var_38)) + (rb_state4.var_22)) ^ (rb_state4.var_37)) + (rb_state4.var_23));
                rb_state4.var_41 = (rb_state4.var_41) + (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_39) : (0xf439efec));
                rb_state4.var_42 = (rb_state4.var_42) + (((rb_state4.var_40) == (0x0)) ? (rb_state4.var_41) : (0x8890add8));
                rb_state4.var_5 = (rb_state4.var_5) - (rb_state4.var_42);
                pthread_mutex_unlock(&(rb_state4.lock_43));
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) - (303 < rb_input_size ? (uint32_t)rb_input[303] : 0xc736b9e2);
            if ((rb_state5.var_3) == (0xf16105d8)) {
                pthread_mutex_lock(&(rb_state5.lock_20));
                rb_state5.var_5 = rb_state5.var_4;
                pthread_mutex_unlock(&(rb_state5.lock_20));
            }
            if ((rb_state5.var_3) == (0xf16105d8)) {
                if (!((rb_state5.var_4) == (rb_state5.var_5))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) + (((rb_state8.var_1) == (0x1c674a59)) ? (rb_state8.var_2) : (0xde5e32e9));
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_2) == (0x809009ab)) {
                pthread_mutex_lock(&(rb_state9.lock_26));
                rb_state9.var_6 = 0x7764105a;
                rb_state9.var_7 = 0x53fbee6;
                rb_state9.var_8 = (rb_state9.var_8) - (rb_state9.var_5);
                rb_state9.var_9 = (rb_state9.var_9) ^ (((0xe93f074f) + (rb_state9.var_6)) + (0x52472211));
                rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_4) : (0x946c5a22));
                rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0x51f74d59));
                rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_9) : (0x8108dd8b));
                rb_state9.var_13 = (rb_state9.var_13) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xc01b650e));
                rb_state9.var_14 = (rb_state9.var_14) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_12) : (0xce88544));
                rb_state9.var_15 = (rb_state9.var_15) + (((((0xeac7f486) - (rb_state9.var_14)) - (rb_state9.var_9)) - (rb_state9.var_13)) ^ (0xd13615ea));
                rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_15);
                rb_state9.var_4 = rb_state9.var_5;
                pthread_mutex_unlock(&(rb_state9.lock_26));
            }
            if ((rb_state9.var_2) == (0x809009ab)) {
                if (!((rb_state9.var_4) == (rb_state9.var_5))) {
                    racebench_trigger(9);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_3 = (rb_state12.var_3) - (0x1adf2ba8);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) - (468 < rb_input_size ? (uint32_t)rb_input[468] : 0x61eba8fb);
            rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_2);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (184 < rb_input_size ? (uint32_t)rb_input[184] : 0xf9afac10);
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_2) == (0xe7d48e44)) {
                pthread_mutex_lock(&(rb_state17.lock_40));
                rb_state17.var_22 = 0x66176195;
                rb_state17.var_23 = (rb_state17.var_23) - (rb_state17.var_15);
                rb_state17.var_24 = (rb_state17.var_24) ^ (rb_state17.var_22);
                rb_state17.var_25 = (rb_state17.var_25) - ((0xbd1c35cb) + (0xe24c53a9));
                rb_state17.var_26 = (rb_state17.var_26) ^ (rb_state17.var_14);
                rb_state17.var_27 = (rb_state17.var_27) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_13) : (0x86470c59));
                rb_state17.var_28 = (rb_state17.var_28) ^ (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_24) : (0x2ab64993));
                rb_state17.var_29 = (rb_state17.var_29) ^ (rb_state17.var_25);
                rb_state17.var_30 = (rb_state17.var_30) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_26) : (0xfcb0cf45));
                rb_state17.var_31 = (rb_state17.var_31) ^ (((0x7fbe5141) - (rb_state17.var_18)) - (rb_state17.var_27));
                rb_state17.var_32 = (rb_state17.var_32) + (((rb_state17.var_19) == (0x0)) ? (rb_state17.var_28) : (0xebdeeb6e));
                rb_state17.var_33 = (rb_state17.var_33) + (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_29) : (0x13b2587c));
                rb_state17.var_34 = (rb_state17.var_34) - (((0x9d27f222) + (0xf56611c7)) + (rb_state17.var_30));
                rb_state17.var_35 = (rb_state17.var_35) + (((((0x9e9230fd) ^ (0x49a2db2c)) ^ (rb_state17.var_21)) - (rb_state17.var_31)) - (rb_state17.var_32));
                rb_state17.var_36 = (rb_state17.var_36) + (rb_state17.var_33);
                rb_state17.var_37 = (rb_state17.var_37) + (rb_state17.var_34);
                rb_state17.var_38 = (rb_state17.var_38) + (((((0xaef9d006) + (rb_state17.var_36)) ^ (0xe3cd7137)) + (rb_state17.var_22)) - (rb_state17.var_35));
                rb_state17.var_39 = (rb_state17.var_39) - (((((0xc04d6be2) + (0xb55e0204)) - (rb_state17.var_38)) - (rb_state17.var_23)) ^ (rb_state17.var_37));
                rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_39);
                pthread_mutex_unlock(&(rb_state17.lock_40));
            }
            #endif
            i = (i + 1) % gm->nprocs;
        }
    }
}