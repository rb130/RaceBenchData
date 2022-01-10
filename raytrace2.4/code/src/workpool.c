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

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (391 < rb_input_size ? (uint32_t)rb_input[391] : 0xb4af41ba);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_1) == (0x779ed25a)) ? (rb_state11.var_0) : (0xeef73fdb));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0xecf6f675)) {
        rb_state16.var_4 = rb_state16.var_3;
    }
    #endif
    xb_addr = xs;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_0) : (0x705a403e));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0x120)) {
        rb_state2.var_13 = 0xc259eb7b;
        rb_state2.var_14 = 0xbcc70d6f;
        rb_state2.var_15 = 0x7087bd01;
        rb_state2.var_16 = (rb_state2.var_16) - ((0x6f7a7fa6) ^ (rb_state2.var_5));
        rb_state2.var_17 = (rb_state2.var_17) ^ (((0x1a5a79f5) - (rb_state2.var_13)) + (0x87c2a441));
        rb_state2.var_18 = (rb_state2.var_18) ^ (((rb_state2.var_6) == (0x417e0948)) ? (rb_state2.var_15) : (0x8871ea1a));
        rb_state2.var_19 = (rb_state2.var_19) ^ (((rb_state2.var_7) == (0x1a1eb321)) ? (rb_state2.var_14) : (0xd2c7f97b));
        rb_state2.var_20 = (rb_state2.var_20) + (((((0xe941a6ae) - (rb_state2.var_16)) + (0x4169c2b2)) + (rb_state2.var_17)) ^ (rb_state2.var_8));
        rb_state2.var_21 = (rb_state2.var_21) ^ (rb_state2.var_18);
        rb_state2.var_22 = (rb_state2.var_22) - (rb_state2.var_19);
        rb_state2.var_23 = (rb_state2.var_23) + (((rb_state2.var_9) == (0x1a1eb321)) ? (rb_state2.var_20) : (0xa9f500a5));
        rb_state2.var_24 = (rb_state2.var_24) - (((((0xba036e25) ^ (rb_state2.var_22)) + (rb_state2.var_11)) + (rb_state2.var_21)) + (rb_state2.var_10));
        rb_state2.var_25 = (rb_state2.var_25) ^ (((((0xcded7646) + (rb_state2.var_23)) ^ (0x4f84fb1f)) + (0x44ebdb2d)) + (rb_state2.var_24));
        rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (91 < rb_input_size ? (uint32_t)rb_input[91] : 0x17af32f0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ ((0x73a25cba) ^ (rb_state7.var_1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (470 < rb_input_size ? (uint32_t)rb_input[470] : 0xc95c3839);
    #endif
    yb_addr = ys;

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (54 < rb_input_size ? (uint32_t)rb_input[54] : 0xd7a52547);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_1);
    #endif
    xb_end = xb_addr + xe - 1;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (162 < rb_input_size ? (uint32_t)rb_input[162] : 0xa0f7afee);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_2) : (0x9c4f618));
    if ((rb_state5.var_2) == (0xe4c89d79)) {
        rb_state5.var_4 = 0x95e26d7d;
        rb_state5.var_5 = (rb_state5.var_5) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_3) : (0x6f73f2b1));
        rb_state5.var_6 = (rb_state5.var_6) - (222 < rb_input_size ? (uint32_t)rb_input[222] : 0xbd10b702);
        rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_4) : (0xa11d7759));
        rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_5);
        rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0x58e117c0));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((((0x190e1ce) + (rb_state5.var_8)) + (0xb118d850)) ^ (rb_state5.var_8)) ^ (rb_state5.var_7));
        rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_9) : (0x9a9c2c14));
        rb_state5.var_12 = (rb_state5.var_12) + (((((0x43d94d3a) - (rb_state5.var_10)) - (0x6e3c5c4e)) + (0xa9202d59)) - (rb_state5.var_11));
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0xecf6f675)) {
        if (!((rb_state16.var_3) == (rb_state16.var_4))) {
            racebench_trigger(16);
        }
    }
    #endif
    yb_end = yb_addr + ye - 1;

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0x42b3d532));
                rb_state0.var_0 = (rb_state0.var_0) ^ (0x8e17dc79);
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) ^ ((0x2f604369) + (rb_state8.var_1));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_2) == (0xb319ead)) ? (rb_state11.var_1) : (0xbd04ef91));
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) + ((0x4bbeb80b) + (rb_state17.var_0));
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0xdccb39fb)) {
                    rb_state18.var_4 = rb_state18.var_1;
                }
                if ((rb_state18.var_0) == (0xdccb39fb)) {
                    pthread_mutex_lock(&(rb_state18.lock_10));
                    if (!((rb_state18.var_1) == (rb_state18.var_4))) {
                        racebench_trigger(18);
                    }
                    pthread_mutex_unlock(&(rb_state18.lock_10));
                }
                #endif
                xb_size = xbe;
            } else {
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_1 = (rb_state5.var_1) ^ (380 < rb_input_size ? (uint32_t)rb_input[380] : 0x23fc5d53);
                #endif
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_1 = (rb_state4.var_1) + (0x1251d83f);
                #endif
                #ifdef RACEBENCH_BUG_7
                if ((rb_state7.var_3) == (0x20)) {
                    rb_state7.var_13 = (rb_state7.var_13) - (((((0xdbe4fd3e) + (0xe4d195ea)) ^ (rb_state7.var_9)) ^ (rb_state7.var_10)) - (rb_state7.var_7));
                    rb_state7.var_14 = (rb_state7.var_14) - (((((0x1386cf02) + (0x59347314)) - (rb_state7.var_13)) - (0x3021e48e)) + (rb_state7.var_8));
                    rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_14);
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_3 = (rb_state8.var_3) - ((0x8c2b1d8a) ^ (rb_state8.var_3));
                if ((rb_state8.var_0) == (0x3164d34e)) {
                    rb_state8.var_5 = 0xc734186;
                    rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0xaf0e7600));
                    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_6);
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) - ((0x80c781cb) + (0x589c09e0));
                #endif
                yb_size = ybe;
            } else {
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) ^ (523 < rb_input_size ? (uint32_t)rb_input[523] : 0x5fdd0d1d);
                #endif
                yb_size = yb_end - yb_addr + 1;
            }

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) + ((0xaa1788cc) ^ (0xc150bb6b));
            if ((rb_state0.var_2) == (0xd61949f9)) {
                rb_state0.var_4 = 0x5978aee2;
                rb_state0.var_5 = 0xdff962e6;
                rb_state0.var_6 = (rb_state0.var_6) + (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_5) : (0x65d0b4a5));
                rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0xaf39bc68));
                rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_3) : (0xbf9e8d59));
                rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_6) : (0x7b265497));
                rb_state0.var_10 = (rb_state0.var_10) - (((0xe9b3861a) + (rb_state0.var_7)) + (rb_state0.var_8));
                rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_8) : (0xd606c14f));
                rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0x2d770067));
                rb_state0.var_13 = (rb_state0.var_13) ^ (((((0x27cc6b7b) - (rb_state0.var_11)) + (0x83f61a96)) + (rb_state0.var_11)) + (rb_state0.var_10));
                rb_state0.var_14 = (rb_state0.var_14) - (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_13) : (0x365302c8));
                rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_14);
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_3) == (0x0)) {
                rb_state3.var_5 = 0x231370fe;
                rb_state3.var_6 = 0xa1862b32;
                rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_5) : (0xb43b740));
                rb_state3.var_8 = (rb_state3.var_8) ^ (172 < rb_input_size ? (uint32_t)rb_input[172] : 0x65585fc7);
                rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_4);
                rb_state3.var_10 = (rb_state3.var_10) - (rb_state3.var_7);
                rb_state3.var_11 = (rb_state3.var_11) ^ (rb_state3.var_8);
                rb_state3.var_12 = (rb_state3.var_12) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_9) : (0x11c9c8d8));
                rb_state3.var_13 = (rb_state3.var_13) - (rb_state3.var_10);
                rb_state3.var_14 = (rb_state3.var_14) ^ (((0x5414af8b) + (rb_state3.var_11)) + (0x9e5bb321));
                rb_state3.var_15 = (rb_state3.var_15) - (rb_state3.var_12);
                rb_state3.var_16 = (rb_state3.var_16) - (((0xb0fdff55) - (rb_state3.var_13)) ^ (0xba9c26ad));
                rb_state3.var_17 = (rb_state3.var_17) ^ (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0x16adea45));
                rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_16) : (0x201670ef));
                rb_state3.var_19 = (rb_state3.var_19) + (((((0xe5ca7014) - (0xd16eb57f)) - (rb_state3.var_18)) ^ (rb_state3.var_17)) + (0xf1caccd9));
                rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_19);
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0x3164d34e)) {
                rb_state8.var_4 = rb_state8.var_1;
            }
            if ((rb_state8.var_0) == (0x3164d34e)) {
                pthread_mutex_lock(&(rb_state8.lock_7));
                if (!((rb_state8.var_1) == (rb_state8.var_4))) {
                    racebench_trigger(8);
                }
                pthread_mutex_unlock(&(rb_state8.lock_7));
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_2) == (0xdee499e1)) {
                pthread_mutex_lock(&(rb_state9.lock_22));
                rb_state9.var_4 = 0x3477d202;
                rb_state9.var_5 = 0x7b5c81c6;
                rb_state9.var_6 = (rb_state9.var_6) + ((0x2e371948) - (0x96552250));
                rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0xa17a99e7));
                rb_state9.var_8 = (rb_state9.var_8) + (((0xd30c106a) + (0x50d8738c)) + (rb_state9.var_5));
                rb_state9.var_9 = (rb_state9.var_9) - (((0xc939faff) - (0x68c6586f)) - (rb_state9.var_6));
                rb_state9.var_10 = (rb_state9.var_10) + (rb_state9.var_7);
                rb_state9.var_11 = (rb_state9.var_11) + (rb_state9.var_8);
                rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_9);
                rb_state9.var_13 = (rb_state9.var_13) ^ (rb_state9.var_10);
                rb_state9.var_14 = (rb_state9.var_14) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_11) : (0xe25162e8));
                rb_state9.var_15 = (rb_state9.var_15) ^ (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_12) : (0xcd9b0ccf));
                rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_13) : (0xbdd30701));
                rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_14) : (0x7e6ebfd1));
                rb_state9.var_18 = (rb_state9.var_18) - (((((0xdc800f89) ^ (0x320e107d)) + (0x73ab2401)) ^ (rb_state9.var_15)) + (rb_state9.var_16));
                rb_state9.var_19 = (rb_state9.var_19) ^ (rb_state9.var_17);
                rb_state9.var_20 = (rb_state9.var_20) ^ (((0x3a7e3375) + (0xb893e47e)) + (rb_state9.var_18));
                rb_state9.var_21 = (rb_state9.var_21) - (((((0x5065580f) - (rb_state9.var_8)) + (rb_state9.var_19)) ^ (0x3740e150)) + (rb_state9.var_20));
                rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_21);
                pthread_mutex_unlock(&(rb_state9.lock_22));
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_2) == (0x1c3a5ed2)) {
                pthread_mutex_lock(&(rb_state11.lock_9));
                rb_state11.var_5 = 0x71f36e16;
                rb_state11.var_6 = (rb_state11.var_6) ^ (rb_state11.var_5);
                rb_state11.var_7 = (rb_state11.var_7) - (((0x2b396aba) - (rb_state11.var_4)) ^ (0x90968db5));
                rb_state11.var_8 = (rb_state11.var_8) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0x21a84d84));
                rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_8);
                pthread_mutex_unlock(&(rb_state11.lock_9));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_3 = (rb_state16.var_3) - (((rb_state16.var_3) == (0xca3f9eda)) ? (rb_state16.var_3) : (0x7007116e));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - (0x80423494);
            rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_3) == (0x5f94ed02)) ? (rb_state18.var_2) : (0xa3f7c761));
            #endif
            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c");

            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) - (0xf8c4820d);
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_13) == (0x71a7fa7e)) {
                rb_state14.var_26 = 0x47e4ebc1;
                rb_state14.var_27 = 0x8d4fe795;
                rb_state14.var_28 = 0x9f9caa96;
                rb_state14.var_29 = (rb_state14.var_29) - ((0xfdfdea98) - (0x2704ef83));
                rb_state14.var_30 = (rb_state14.var_30) + (rb_state14.var_28);
                rb_state14.var_31 = (rb_state14.var_31) - (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_27) : (0x99fdd92b));
                rb_state14.var_32 = (rb_state14.var_32) + (((((0x86ee64ab) ^ (rb_state14.var_29)) - (rb_state14.var_26)) ^ (0x15b43ab1)) ^ (rb_state14.var_18));
                rb_state14.var_33 = (rb_state14.var_33) + (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_30) : (0x53b8d6ef));
                rb_state14.var_34 = (rb_state14.var_34) + (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_31) : (0xfbd23c11));
                rb_state14.var_35 = (rb_state14.var_35) - (((rb_state14.var_32) == (0x0)) ? (rb_state14.var_33) : (0x4e0ea7e9));
                rb_state14.var_36 = (rb_state14.var_36) - (((((0xe8bda121) ^ (rb_state14.var_22)) + (rb_state14.var_34)) + (rb_state14.var_21)) - (rb_state14.var_35));
                rb_state14.var_25 = (rb_state14.var_25) - (rb_state14.var_36);
                rb_state14.var_17 = rb_state14.var_25;
            }
            if ((rb_state14.var_13) == (0x71a7fa7e)) {
                pthread_mutex_lock(&(rb_state14.lock_46));
                rb_state14.var_38 = 0x63840951;
                rb_state14.var_39 = 0x4fbda129;
                rb_state14.var_40 = 0x32fcb1f4;
                rb_state14.var_41 = (rb_state14.var_41) ^ ((0xc8dd099) ^ (0x4e08aabe));
                rb_state14.var_42 = (rb_state14.var_42) - (((((0x4bc20321) ^ (0x4730fe5a)) - (rb_state14.var_40)) + (rb_state14.var_39)) ^ (0xbbf8410c));
                rb_state14.var_43 = (rb_state14.var_43) + (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_38) : (0x894531da));
                rb_state14.var_44 = (rb_state14.var_44) ^ (((rb_state14.var_41) == (0x0)) ? (rb_state14.var_42) : (0xa1bd2dd4));
                rb_state14.var_45 = (rb_state14.var_45) + (((((0xc2012c79) + (rb_state14.var_25)) ^ (rb_state14.var_44)) ^ (rb_state14.var_24)) + (rb_state14.var_43));
                rb_state14.var_37 = (rb_state14.var_37) ^ (rb_state14.var_45);
                rb_state14.var_17 = rb_state14.var_37;
                pthread_mutex_unlock(&(rb_state14.lock_46));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) ^ (0x1062ae27);
            #endif
            wpentry->xpix = xb_addr;
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_2);
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_2) == (0xe4c89d79)) {
                rb_state5.var_3 = rb_state5.var_0;
            }
            if ((rb_state5.var_2) == (0xe4c89d79)) {
                if (!((rb_state5.var_0) == (rb_state5.var_3))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_3 = (rb_state7.var_3) + (180 < rb_input_size ? (uint32_t)rb_input[180] : 0xd7a1ac77);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_12 = (rb_state14.var_12) + ((0x81af1fe7) + (0xd2c692b2));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) - (0x5ab63f07);
            if ((rb_state17.var_2) == (0x7cf56217)) {
                rb_state17.var_12 = 0x5695c2a8;
                rb_state17.var_13 = (rb_state17.var_13) - (((((0xab4e7c62) - (rb_state17.var_10)) ^ (rb_state17.var_12)) - (0xc399a808)) ^ (rb_state17.var_8));
                rb_state17.var_14 = (rb_state17.var_14) + (((0x438cfa57) ^ (rb_state17.var_9)) ^ (rb_state17.var_11));
                rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_12) : (0x429ea023));
                rb_state17.var_16 = (rb_state17.var_16) - (rb_state17.var_13);
                rb_state17.var_17 = (rb_state17.var_17) + (((((0x3d1483ff) ^ (0x3bea659a)) + (0xdc529411)) + (rb_state17.var_15)) + (rb_state17.var_14));
                rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_16) : (0x29b5ae63));
                rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_17) : (0xb60fd9bc));
                rb_state17.var_20 = (rb_state17.var_20) ^ (rb_state17.var_18);
                rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_19) : (0xe4014db8));
                rb_state17.var_22 = (rb_state17.var_22) + (((0x41b4f2dd) ^ (rb_state17.var_20)) + (rb_state17.var_17));
                rb_state17.var_23 = (rb_state17.var_23) + (((0x4688d175) - (0x82cfdff1)) ^ (rb_state17.var_21));
                rb_state17.var_24 = (rb_state17.var_24) - (((0xccdd76d7) + (0xc53293e2)) ^ (rb_state17.var_22));
                rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_23);
                rb_state17.var_26 = (rb_state17.var_26) - (((0x500d5a6a) ^ (rb_state17.var_24)) ^ (rb_state17.var_18));
                rb_state17.var_27 = (rb_state17.var_27) ^ (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_26) : (0x6307bb2a));
                rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_27);
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_1) == (0x9c669b12)) {
                pthread_mutex_lock(&(rb_state19.lock_8));
                rb_state19.var_4 = rb_state19.var_2;
                pthread_mutex_unlock(&(rb_state19.lock_8));
            }
            if ((rb_state19.var_1) == (0x9c669b12)) {
                if (!((rb_state19.var_2) == (rb_state19.var_4))) {
                    racebench_trigger(19);
                }
            }
            #endif
            wpentry->ypix = yb_addr;
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0xc7490fd2)) {
                pthread_mutex_lock(&(rb_state12.lock_25));
                rb_state12.var_6 = 0x359b857c;
                rb_state12.var_7 = (rb_state12.var_7) ^ ((0x9d1e7d8b) ^ (rb_state12.var_6));
                rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_5) : (0x75abf1d));
                rb_state12.var_9 = (rb_state12.var_9) - (411 < rb_input_size ? (uint32_t)rb_input[411] : 0x51653658);
                rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_4) : (0x53597e1e));
                rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0x32621a49));
                rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0x536d26e3));
                rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0x425740f2));
                rb_state12.var_14 = (rb_state12.var_14) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0x5b07bd35));
                rb_state12.var_15 = (rb_state12.var_15) - (((0x993c1a93) ^ (rb_state12.var_12)) - (0xd278c19b));
                rb_state12.var_16 = (rb_state12.var_16) ^ (((0x9a5d0578) ^ (rb_state12.var_11)) + (rb_state12.var_13));
                rb_state12.var_17 = (rb_state12.var_17) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0x2c037c73));
                rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_17) : (0x40a458fb));
                rb_state12.var_5 = (rb_state12.var_5) + (rb_state12.var_18);
                rb_state12.var_4 = rb_state12.var_5;
                pthread_mutex_unlock(&(rb_state12.lock_25));
            }
            if ((rb_state12.var_1) == (0xc7490fd2)) {
                if (!((rb_state12.var_4) == (rb_state12.var_5))) {
                    racebench_trigger(12);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_0) == (0x476e38)) {
                if ((rb_state14.var_5) != (0x0)) {
                    rb_state14.var_16 = !((rb_state14.var_3) == (rb_state14.var_4));
                }
            }
            #endif
            wpentry->xdim = xb_size;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) + ((0x6e1bf2bd) - (0x357f2824));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (578 < rb_input_size ? (uint32_t)rb_input[578] : 0xb3d41447);
            rb_state4.var_2 = (rb_state4.var_2) + (182 < rb_input_size ? (uint32_t)rb_input[182] : 0x54ac33be);
            if ((rb_state4.var_0) == (0x6993fce8)) {
                if ((rb_state4.var_4) != (0x0)) {
                    if (!((rb_state4.var_4) == (rb_state4.var_24))) {
                        racebench_trigger(4);
                    }
                }
            }
            #endif
            wpentry->ydim = yb_size;

            if (!gm->workpool[pid][0]) {
                wpentry->next = NULL;
            } else {
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_3 = (rb_state8.var_3) - (0x62da2438);
                #endif
                wpentry->next = gm->workpool[pid][0];
            }

            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) + (0xfb54db45);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) ^ (((rb_state5.var_1) == (0xf295111)) ? (rb_state5.var_2) : (0xbf8406b6));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x5084b0c)) ? (rb_state17.var_0) : (0x99694fa));
            #endif
            gm->workpool[pid][0] = wpentry;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_3 = (rb_state4.var_3) - (303 < rb_input_size ? (uint32_t)rb_input[303] : 0x564c532b);
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

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x105a1f4a) + (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - ((0x96ea9cf8) ^ (rb_state9.var_0));
    rb_state9.var_1 = (rb_state9.var_1) + (445 < rb_input_size ? (uint32_t)rb_input[445] : 0x65f485de);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x32af5bab)) {
        rb_state10.var_11 = 0x4dda18e4;
        rb_state10.var_12 = 0xbfc49b38;
        rb_state10.var_13 = (rb_state10.var_13) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_6) : (0x260dbbdd));
        rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_5);
        rb_state10.var_15 = (rb_state10.var_15) ^ (((0x9dd8ab39) + (rb_state10.var_12)) ^ (0x9df557d7));
        rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_11) : (0xd877b24f));
        rb_state10.var_17 = (rb_state10.var_17) + (rb_state10.var_13);
        rb_state10.var_18 = (rb_state10.var_18) + (((0xe68e36f2) ^ (rb_state10.var_14)) - (0x1a53a1eb));
        rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_15) : (0x162161b1));
        rb_state10.var_20 = (rb_state10.var_20) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_16) : (0x2ec5595b));
        rb_state10.var_21 = (rb_state10.var_21) - (((0x25736018) + (rb_state10.var_11)) ^ (rb_state10.var_17));
        rb_state10.var_22 = (rb_state10.var_22) + (((((0xdcfdd439) ^ (rb_state10.var_12)) - (rb_state10.var_19)) - (rb_state10.var_18)) ^ (rb_state10.var_13));
        rb_state10.var_23 = (rb_state10.var_23) - (((((0xc4d61cc1) ^ (rb_state10.var_14)) - (rb_state10.var_20)) - (rb_state10.var_21)) + (0xaceea17a));
        rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_22) : (0x253f8cdf));
        rb_state10.var_25 = (rb_state10.var_25) - (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_23) : (0x448d60c0));
        rb_state10.var_26 = (rb_state10.var_26) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_24) : (0x558a80b5));
        rb_state10.var_27 = (rb_state10.var_27) + (((0xf5e7f935) ^ (rb_state10.var_18)) ^ (rb_state10.var_25));
        rb_state10.var_28 = (rb_state10.var_28) + (((((0xd099c059) ^ (0x594689be)) - (rb_state10.var_27)) + (rb_state10.var_26)) - (rb_state10.var_19));
        rb_state10.var_4 = (rb_state10.var_4) ^ (rb_state10.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (0xa46b5ad5);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_2) == (0x5542cf5b)) {
        pthread_mutex_lock(&(rb_state15.lock_14));
        rb_state15.var_5 = 0x1960b2a3;
        rb_state15.var_6 = (rb_state15.var_6) ^ (0xfe65cf92);
        rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_4) : (0xb289a2cb));
        rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xf7cee610) ^ (rb_state15.var_6)) ^ (rb_state15.var_5)) + (rb_state15.var_6)) + (rb_state15.var_5));
        rb_state15.var_9 = (rb_state15.var_9) - (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_7) : (0x37cec993));
        rb_state15.var_10 = (rb_state15.var_10) - (((((0x9f7f6fc4) ^ (rb_state15.var_8)) - (rb_state15.var_9)) - (rb_state15.var_9)) ^ (rb_state15.var_8));
        rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_10);
        rb_state15.var_3 = rb_state15.var_4;
        pthread_mutex_unlock(&(rb_state15.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_1);
    #endif
    gm->wpstat[pid][0] = WPS_VALID;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (564 < rb_input_size ? (uint32_t)rb_input[564] : 0x1c93851e);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x32af5bab)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_6 = 0x75bf47e4;
        rb_state10.var_7 = 0x9cb5024;
        rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_6);
        rb_state10.var_9 = (rb_state10.var_9) + (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_4) : (0x51f68fb4));
        rb_state10.var_10 = (rb_state10.var_10) + (((((0xe05a88ea) - (0x6f1b6d7e)) ^ (rb_state10.var_8)) - (rb_state10.var_9)) - (0x7fe39877));
        rb_state10.var_5 = (rb_state10.var_5) ^ (rb_state10.var_10);
        rb_state10.var_4 = rb_state10.var_5;
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    if ((rb_state10.var_2) == (0x32af5bab)) {
        if (!((rb_state10.var_4) == (rb_state10.var_5))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (332 < rb_input_size ? (uint32_t)rb_input[332] : 0x47db2c67);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_1);
    if ((rb_state15.var_2) == (0x5542cf5b)) {
        if (!((rb_state15.var_3) == (rb_state15.var_4))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_3) : (0xd035897f));
    #endif
    gm->workpool[pid][0] = NULL;

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - ((0x132aeff3) ^ (rb_state5.var_1));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (0x54badbc2);
    rb_state9.var_2 = (rb_state9.var_2) + ((0xa5f72560) - (0xa41f4caa));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - ((0x4f1927e8) + (0xde5c91fa));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_2) : (0x8ccf7484));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_0);
    #endif
    i = 0;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0x695363e3);
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_2) : (0x9430e251));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_2) == (0x5542cf5b)) {
        pthread_mutex_lock(&(rb_state15.lock_14));
        rb_state15.var_11 = 0x25f3117;
        rb_state15.var_12 = (rb_state15.var_12) + (((0xd8380be2) ^ (rb_state15.var_10)) ^ (0xb910ba73));
        rb_state15.var_13 = (rb_state15.var_13) ^ (((((0xd9d03538) ^ (rb_state15.var_11)) - (rb_state15.var_12)) ^ (rb_state15.var_12)) + (rb_state15.var_13));
        rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_13);
        pthread_mutex_unlock(&(rb_state15.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_0) : (0xc805d912));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0xc842a488));
    #endif
    xsize = Display.xres / blockx;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0xfffffe90)) ? (rb_state2.var_1) : (0x25811bd6));
    rb_state2.var_2 = (rb_state2.var_2) ^ (0x3ae55dba);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (520 < rb_input_size ? (uint32_t)rb_input[520] : 0x8a0c1e61);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + ((0x68656aee) ^ (0x26b1d353));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - ((0xaa396e7d) - (0xa8fb0788));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (487 < rb_input_size ? (uint32_t)rb_input[487] : 0xf618f7b4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + ((0x276f0695) ^ (rb_state18.var_0));
    #endif
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - ((0x578308bc) - (0x71e807dd));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + ((0xf90afe37) + (0x471959a3));
            rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_2);
            rb_state14.var_1 = (rb_state14.var_1) - (132 < rb_input_size ? (uint32_t)rb_input[132] : 0x3c6f0524);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + ((0xd34c38a9) ^ (0xcc7ca594));
            rb_state18.var_1 = (rb_state18.var_1) - ((0x6319cef2) ^ (0xc32e7bd6));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + (354 < rb_input_size ? (uint32_t)rb_input[354] : 0x78702bcd);
            #endif
            ye = ysize;
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_2 = (rb_state0.var_2) ^ ((0xb6d2139e) ^ (0xb8c69c0));
                if ((rb_state0.var_2) == (0xd61949f9)) {
                    rb_state0.var_3 = rb_state0.var_1;
                }
                if ((rb_state0.var_2) == (0xd61949f9)) {
                    if (!((rb_state0.var_1) == (rb_state0.var_3))) {
                        racebench_trigger(0);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_0 = (rb_state1.var_0) + (0xb0e0a86e);
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_2 = (rb_state2.var_2) ^ (0x8d9cc861);
                rb_state2.var_3 = (rb_state2.var_3) + (358 < rb_input_size ? (uint32_t)rb_input[358] : 0x6573a628);
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_2 = (rb_state3.var_2) - (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_2) : (0x962b0165));
                #endif
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_0) == (0x494d7c18)) {
                    rb_state6.var_3 = rb_state6.var_1;
                }
                if ((rb_state6.var_0) == (0x494d7c18)) {
                    pthread_mutex_lock(&(rb_state6.lock_30));
                    rb_state6.var_4 = 0xf34c22bf;
                    rb_state6.var_5 = (rb_state6.var_5) - ((0x143a3d3e) ^ (0x479090eb));
                    rb_state6.var_6 = (rb_state6.var_6) ^ (523 < rb_input_size ? (uint32_t)rb_input[523] : 0x418f9f23);
                    rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_4) : (0x19a0808d));
                    rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_3);
                    rb_state6.var_9 = (rb_state6.var_9) - (rb_state6.var_4);
                    rb_state6.var_10 = (rb_state6.var_10) ^ (rb_state6.var_5);
                    rb_state6.var_11 = (rb_state6.var_11) + (((((0x83313538) + (rb_state6.var_7)) - (0xcba15898)) + (0x382ab617)) ^ (rb_state6.var_6));
                    rb_state6.var_12 = (rb_state6.var_12) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_8) : (0x70ea4c34));
                    rb_state6.var_13 = (rb_state6.var_13) - (rb_state6.var_9);
                    rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_10) : (0x19f53747));
                    rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_11);
                    rb_state6.var_16 = (rb_state6.var_16) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_12) : (0xc6d833bb));
                    rb_state6.var_17 = (rb_state6.var_17) ^ (rb_state6.var_13);
                    rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_14) : (0xd705bc8));
                    rb_state6.var_19 = (rb_state6.var_19) + (rb_state6.var_15);
                    rb_state6.var_20 = (rb_state6.var_20) ^ (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_17) : (0x75444fdd));
                    rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_18) : (0x9874bc45));
                    rb_state6.var_22 = (rb_state6.var_22) - (rb_state6.var_19);
                    rb_state6.var_23 = (rb_state6.var_23) ^ (((0x461ced68) - (rb_state6.var_20)) - (rb_state6.var_11));
                    rb_state6.var_24 = (rb_state6.var_24) ^ (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_21) : (0xefaaec9b));
                    rb_state6.var_25 = (rb_state6.var_25) + (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_22) : (0x4c143a3f));
                    rb_state6.var_26 = (rb_state6.var_26) + (((0xc0df4a7e) + (rb_state6.var_14)) - (rb_state6.var_23));
                    rb_state6.var_27 = (rb_state6.var_27) ^ (((((0xe76dadcf) - (rb_state6.var_25)) + (rb_state6.var_24)) ^ (rb_state6.var_15)) + (rb_state6.var_16));
                    rb_state6.var_28 = (rb_state6.var_28) - (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_26) : (0xf266ef40));
                    rb_state6.var_29 = (rb_state6.var_29) - (((rb_state6.var_27) == (0x0)) ? (rb_state6.var_28) : (0x3de475e1));
                    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_29);
                    pthread_mutex_unlock(&(rb_state6.lock_30));
                }
                if ((rb_state6.var_0) == (0x494d7c18)) {
                    pthread_mutex_lock(&(rb_state6.lock_30));
                    if (!((rb_state6.var_1) == (rb_state6.var_3))) {
                        racebench_trigger(6);
                    }
                    pthread_mutex_unlock(&(rb_state6.lock_30));
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) - ((0xb4a2a9e6) ^ (0x803df158));
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_0);
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_2) == (0xfffff925)) {
                    rb_state13.var_7 = 0xd32e8bbe;
                    rb_state13.var_8 = 0x5b0de25;
                    rb_state13.var_9 = (rb_state13.var_9) ^ ((0xbb7e42c6) + (rb_state13.var_6));
                    rb_state13.var_10 = (rb_state13.var_10) + (rb_state13.var_7);
                    rb_state13.var_11 = (rb_state13.var_11) ^ (((0xbf163982) + (rb_state13.var_7)) + (rb_state13.var_5));
                    rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_8) == (0x5b0de25)) ? (rb_state13.var_8) : (0xaa3fdca1));
                    rb_state13.var_13 = (rb_state13.var_13) + (((rb_state13.var_9) == (0xbb7e42c6)) ? (rb_state13.var_10) : (0xe1ee43c9));
                    rb_state13.var_14 = (rb_state13.var_14) - (((((0x5e5128e1) + (0x8da1f4c9)) + (0xa2238c4)) ^ (rb_state13.var_12)) + (rb_state13.var_11));
                    rb_state13.var_15 = (rb_state13.var_15) + (((rb_state13.var_9) == (0xbb7e42c6)) ? (rb_state13.var_13) : (0x3d0ec322));
                    rb_state13.var_16 = (rb_state13.var_16) + (((((0x2b2a7c01) ^ (rb_state13.var_15)) ^ (0xa00bd563)) + (rb_state13.var_14)) + (0xd1217153));
                    rb_state13.var_6 = (rb_state13.var_6) ^ (rb_state13.var_16);
                    rb_state13.var_3 = rb_state13.var_6;
                }
                if ((rb_state13.var_2) == (0xfffff925)) {
                    pthread_mutex_lock(&(rb_state13.lock_21));
                    rb_state13.var_18 = 0xe1911010;
                    rb_state13.var_19 = (rb_state13.var_19) ^ (((((0xc08c1225) ^ (rb_state13.var_11)) + (rb_state13.var_13)) - (rb_state13.var_12)) - (rb_state13.var_18));
                    rb_state13.var_20 = (rb_state13.var_20) - (((((0xea904cb6) ^ (rb_state13.var_14)) + (0x6f801dff)) + (rb_state13.var_19)) - (rb_state13.var_10));
                    rb_state13.var_17 = (rb_state13.var_17) + (rb_state13.var_20);
                    rb_state13.var_3 = rb_state13.var_17;
                    pthread_mutex_unlock(&(rb_state13.lock_21));
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_1) == (0x9c669b12)) {
                    rb_state19.var_5 = 0x4259c11d;
                    rb_state19.var_6 = (rb_state19.var_6) - (rb_state19.var_4);
                    rb_state19.var_7 = (rb_state19.var_7) ^ (((((0x3f491f57) ^ (rb_state19.var_5)) + (0x9a381df1)) - (rb_state19.var_5)) + (rb_state19.var_6));
                    rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_7);
                }
                #endif
                xe = xsize;
            }

            if (i == pid) {
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_3) == (0x120)) {
                    pthread_mutex_lock(&(rb_state2.lock_26));
                    rb_state2.var_6 = 0x417e0948;
                    rb_state2.var_7 = 0x1a1eb321;
                    rb_state2.var_8 = (rb_state2.var_8) + (rb_state2.var_6);
                    rb_state2.var_9 = (rb_state2.var_9) ^ (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_7) : (0x2432fe57));
                    rb_state2.var_10 = (rb_state2.var_10) + (((0xa2d33845) ^ (0x480797b8)) - (rb_state2.var_8));
                    rb_state2.var_11 = (rb_state2.var_11) + (((0xe99d5d19) - (0x24cc2879)) ^ (rb_state2.var_9));
                    rb_state2.var_12 = (rb_state2.var_12) - (((((0x6c81353b) - (rb_state2.var_10)) + (0x411054b9)) - (0x52ec3759)) - (rb_state2.var_11));
                    rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_12);
                    rb_state2.var_4 = rb_state2.var_5;
                    pthread_mutex_unlock(&(rb_state2.lock_26));
                }
                if ((rb_state2.var_3) == (0x120)) {
                    if (!((rb_state2.var_4) == (rb_state2.var_5))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_3 = (rb_state16.var_3) ^ (0xca3f9eda);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) ^ (0x7e6341f0);
                #endif
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_2);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_2);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ ((0x2e36e0d4) + (0x68855155));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_1);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) - (411 < rb_input_size ? (uint32_t)rb_input[411] : 0xe8140cf7);
            #endif
            i = (i + 1) % gm->nprocs;
        }
    }
}