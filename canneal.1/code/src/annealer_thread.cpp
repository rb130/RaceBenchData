#include "racebench_bugs.h"


#ifdef ENABLE_THREADS
#include <pthread.h>
#endif

#include <cassert>
#include "annealer_thread.h"
#include "location_t.h"
#include "annealer_types.h"
#include "netlist_elem.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include "rng.h"

using std::cout;
using std::endl;

void annealer_thread::Run() {
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_5) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_36));
        rb_state0.var_8 = 0xaeb0c26e;
        rb_state0.var_9 = 0x4171dc48;
        rb_state0.var_10 = 0xf290de86;
        rb_state0.var_11 = (rb_state0.var_11) + (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_10) : (0x5c00d67a));
        rb_state0.var_12 = (rb_state0.var_12) + (((0xcf483a88) - (rb_state0.var_8)) ^ (rb_state0.var_7));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((0x79b427fc) + (rb_state0.var_9)) + (rb_state0.var_8));
        rb_state0.var_14 = (rb_state0.var_14) ^ (rb_state0.var_11);
        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_12);
        rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_13);
        rb_state0.var_17 = (rb_state0.var_17) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_14) : (0x3772ec8e));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((0x3346915f) - (rb_state0.var_10)) + (rb_state0.var_15));
        rb_state0.var_19 = (rb_state0.var_19) + (((0x800d9f63) + (rb_state0.var_16)) + (0x4168dd93));
        rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_17) : (0xbdf37c8a));
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_19) : (0xe25eded6));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((0x43692b84) + (0x2730675d)) ^ (rb_state0.var_20));
        rb_state0.var_23 = (rb_state0.var_23) + (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_21) : (0x77bbd1c5));
        rb_state0.var_24 = (rb_state0.var_24) - (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_23) : (0xa873af2c));
        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_24);
        rb_state0.var_6 = rb_state0.var_7;
        pthread_mutex_unlock(&(rb_state0.lock_36));
    }
    if ((rb_state0.var_5) == (0x0)) {
        if (!((rb_state0.var_6) == (rb_state0.var_7))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - ((0xb210b14d) ^ (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - ((0xfd8c6deb) ^ (0xe49c9eda));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xac061083) ^ (rb_state10.var_0));
    if ((rb_state10.var_0) == (0xac061083)) {
        pthread_mutex_lock(&(rb_state10.lock_21));
        rb_state10.var_7 = 0x65e097a1;
        rb_state10.var_8 = (rb_state10.var_8) + (((0x1a6289dd) + (rb_state10.var_5)) ^ (rb_state10.var_3));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_7) == (0x65e097a1)) ? (rb_state10.var_6) : (0xae4eeaee));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_8) == (0xeb805199)) ? (rb_state10.var_7) : (0x544aa73e));
        rb_state10.var_11 = (rb_state10.var_11) - (((rb_state10.var_9) == (0xf6cf33c1)) ? (rb_state10.var_4) : (0x21940925));
        rb_state10.var_12 = (rb_state10.var_12) + (((0x5836d2e6) - (rb_state10.var_10)) ^ (rb_state10.var_8));
        rb_state10.var_13 = (rb_state10.var_13) ^ (((0x2a0196d1) ^ (rb_state10.var_9)) + (0x3e6aca7b));
        rb_state10.var_14 = (rb_state10.var_14) ^ (((rb_state10.var_11) == (0x754a4f50)) ? (rb_state10.var_10) : (0xa067432f));
        rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_11);
        rb_state10.var_16 = (rb_state10.var_16) - (((((0x1d35b99a) + (0xc5f6f4d6)) ^ (rb_state10.var_12)) - (rb_state10.var_13)) + (rb_state10.var_12));
        rb_state10.var_17 = (rb_state10.var_17) + (rb_state10.var_14);
        rb_state10.var_18 = (rb_state10.var_18) - (((((0xa1fe9281) - (0x1cff0acb)) - (rb_state10.var_16)) ^ (rb_state10.var_13)) ^ (rb_state10.var_15));
        rb_state10.var_19 = (rb_state10.var_19) + (((rb_state10.var_14) == (0x65e097a1)) ? (rb_state10.var_17) : (0x19df5dd3));
        rb_state10.var_20 = (rb_state10.var_20) ^ (((((0xdcc12805) + (0x767d503a)) + (rb_state10.var_19)) + (rb_state10.var_15)) ^ (rb_state10.var_18));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_20);
        pthread_mutex_unlock(&(rb_state10.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x114)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        rb_state11.var_2 = rb_state11.var_1;
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    if ((rb_state11.var_0) == (0x114)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x4a495df1)) {
        pthread_mutex_lock(&(rb_state17.lock_25));
        rb_state17.var_2 = rb_state17.var_1;
        pthread_mutex_unlock(&(rb_state17.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_28));
        rb_state18.var_14 = 0x16730f6b;
        rb_state18.var_15 = 0xe244a021;
        rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_7) : (0xfcb06a8e));
        rb_state18.var_17 = (rb_state18.var_17) + (((((0x5824f0ef) + (rb_state18.var_14)) - (0x7c6218ec)) + (rb_state18.var_9)) - (rb_state18.var_15));
        rb_state18.var_18 = (rb_state18.var_18) - (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_17) : (0xda30f4be));
        rb_state18.var_13 = (rb_state18.var_13) ^ (rb_state18.var_18);
        rb_state18.var_1 = rb_state18.var_13;
        pthread_mutex_unlock(&(rb_state18.lock_28));
    }
    if ((rb_state18.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_28));
        rb_state18.var_20 = 0xfe1c0a16;
        rb_state18.var_21 = (rb_state18.var_21) + (((rb_state18.var_13) == (0xf00eb8b3)) ? (rb_state18.var_12) : (0x20d54c9d));
        rb_state18.var_22 = (rb_state18.var_22) ^ (0x1c9ba3af);
        rb_state18.var_23 = (rb_state18.var_23) ^ (rb_state18.var_20);
        rb_state18.var_24 = (rb_state18.var_24) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_10) : (0x8d56dfe6));
        rb_state18.var_25 = (rb_state18.var_25) ^ (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_22) : (0x6a76b32d));
        rb_state18.var_26 = (rb_state18.var_26) ^ (((rb_state18.var_23) == (0xfe1c0a16)) ? (rb_state18.var_24) : (0xf86cda62));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((rb_state18.var_25) == (0x1c9ba3af)) ? (rb_state18.var_26) : (0x8d84f197));
        rb_state18.var_19 = (rb_state18.var_19) - (rb_state18.var_27);
        rb_state18.var_1 = rb_state18.var_19;
        pthread_mutex_unlock(&(rb_state18.lock_28));
    }
    #endif
    int accepted_good_moves = 0;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_5) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_36));
        rb_state0.var_25 = 0x37d64c99;
        rb_state0.var_26 = 0xf47f7c6f;
        rb_state0.var_27 = (rb_state0.var_27) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_14) : (0x9c68613e));
        rb_state0.var_28 = (rb_state0.var_28) + ((0xb3094000) + (rb_state0.var_16));
        rb_state0.var_29 = (rb_state0.var_29) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_13) : (0x942caeb4));
        rb_state0.var_30 = (rb_state0.var_30) + (((((0x3f05cf0f) + (rb_state0.var_18)) + (0xfb1e05e5)) + (rb_state0.var_25)) - (rb_state0.var_26));
        rb_state0.var_31 = (rb_state0.var_31) - (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_27) : (0xe4d3d7db));
        rb_state0.var_32 = (rb_state0.var_32) + (((rb_state0.var_28) == (0x0)) ? (rb_state0.var_29) : (0x77aa027e));
        rb_state0.var_33 = (rb_state0.var_33) + (((0xb27b618c) - (rb_state0.var_30)) - (0xd09af90d));
        rb_state0.var_34 = (rb_state0.var_34) - (((((0x925a55e5) + (0x5c3b9e5)) ^ (rb_state0.var_32)) ^ (rb_state0.var_20)) + (rb_state0.var_31));
        rb_state0.var_35 = (rb_state0.var_35) + (((((0x3ee05eac) + (rb_state0.var_33)) - (rb_state0.var_34)) + (0x255ca79a)) - (0xb32b2150));
        rb_state0.var_6 = (rb_state0.var_6) + (rb_state0.var_35);
        pthread_mutex_unlock(&(rb_state0.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_20) : (0xf622ff9b));
    if ((rb_state2.var_22) == (0x8511daf2)) {
        if ((rb_state2.var_28) != (0x0)) {
            if (!((rb_state2.var_28) == (rb_state2.var_54))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xb4cd2757)) {
        rb_state4.var_3 = 0x72040c63;
        rb_state4.var_4 = 0xe351c930;
        rb_state4.var_5 = (rb_state4.var_5) + (((0xddfa4ff9) ^ (rb_state4.var_3)) - (rb_state4.var_4));
        rb_state4.var_6 = (rb_state4.var_6) ^ (((((0xe5597141) + (rb_state4.var_2)) + (0x985ed17a)) ^ (0x809f7723)) - (rb_state4.var_3));
        rb_state4.var_7 = (rb_state4.var_7) - (((0xa928ce8b) - (0xae8ee1e5)) - (rb_state4.var_5));
        rb_state4.var_8 = (rb_state4.var_8) + (rb_state4.var_6);
        rb_state4.var_9 = (rb_state4.var_9) - (rb_state4.var_7);
        rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0x6105f3ed));
        rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_10);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x7183bd41)) {
        rb_state8.var_19 = 0x6fa62b66;
        rb_state8.var_20 = 0x7444487;
        rb_state8.var_21 = (rb_state8.var_21) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_20) : (0x7b3ca69c));
        rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_21) : (0x245b597c));
        rb_state8.var_18 = (rb_state8.var_18) ^ (rb_state8.var_22);
        rb_state8.var_1 = rb_state8.var_18;
    }
    if ((rb_state8.var_0) == (0x7183bd41)) {
        pthread_mutex_lock(&(rb_state8.lock_45));
        rb_state8.var_24 = (rb_state8.var_24) + (((0xbf2c87c4) + (rb_state8.var_11)) ^ (rb_state8.var_8));
        rb_state8.var_25 = (rb_state8.var_25) ^ ((0x1f4c05d6) + (0x9931e99c));
        rb_state8.var_26 = (rb_state8.var_26) - (rb_state8.var_9);
        rb_state8.var_27 = (rb_state8.var_27) + (rb_state8.var_12);
        rb_state8.var_28 = (rb_state8.var_28) + (((0x73e9419f) - (0x40a78e6c)) + (rb_state8.var_10));
        rb_state8.var_29 = (rb_state8.var_29) - (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_24) : (0x958a9489));
        rb_state8.var_30 = (rb_state8.var_30) + (rb_state8.var_25);
        rb_state8.var_31 = (rb_state8.var_31) + (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_26) : (0x70155021));
        rb_state8.var_32 = (rb_state8.var_32) ^ (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_27) : (0x6c696bbd));
        rb_state8.var_33 = (rb_state8.var_33) - (((((0xfd902ab4) ^ (rb_state8.var_28)) + (0xb396afbb)) + (rb_state8.var_29)) + (rb_state8.var_16));
        rb_state8.var_34 = (rb_state8.var_34) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_30) : (0x4b367a35));
        rb_state8.var_35 = (rb_state8.var_35) ^ (((((0xc832180c) ^ (rb_state8.var_18)) + (rb_state8.var_31)) - (0xff9b4510)) + (rb_state8.var_32));
        rb_state8.var_36 = (rb_state8.var_36) - (((0x8d7b1c43) + (rb_state8.var_33)) ^ (0x5382d442));
        rb_state8.var_37 = (rb_state8.var_37) ^ (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0xea74e98b));
        rb_state8.var_38 = (rb_state8.var_38) - (((0x4344aeaa) - (rb_state8.var_36)) + (0xcd8156a3));
        rb_state8.var_39 = (rb_state8.var_39) ^ (rb_state8.var_37);
        rb_state8.var_40 = (rb_state8.var_40) ^ (((0x9a3e2d00) ^ (rb_state8.var_38)) - (0x838c45a5));
        rb_state8.var_41 = (rb_state8.var_41) ^ (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_39) : (0x8c674bbe));
        rb_state8.var_42 = (rb_state8.var_42) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_40) : (0x31397f82));
        rb_state8.var_43 = (rb_state8.var_43) ^ (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_41) : (0x81b341ed));
        rb_state8.var_44 = (rb_state8.var_44) + (((((0x6419f78d) + (0x76a5e66c)) + (rb_state8.var_43)) ^ (rb_state8.var_22)) - (rb_state8.var_42));
        rb_state8.var_23 = (rb_state8.var_23) + (rb_state8.var_44);
        rb_state8.var_1 = rb_state8.var_23;
        pthread_mutex_unlock(&(rb_state8.lock_45));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x4a495df1)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (0xfe6c13e);
    #endif
    int accepted_bad_moves = -1;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x26f03471) + (0x9bff91c7));
    if ((rb_state2.var_0) == (0x931f88f5)) {
        pthread_mutex_lock(&(rb_state2.lock_26));
        rb_state2.var_3 = 0xf130ec5b;
        rb_state2.var_4 = (rb_state2.var_4) + (((0x7fd5fa69) - (rb_state2.var_3)) + (0x85bc902c));
        rb_state2.var_5 = (rb_state2.var_5) - (((((0x8c4829e6) - (rb_state2.var_1)) - (0x3d650d59)) + (rb_state2.var_3)) ^ (rb_state2.var_2));
        rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_4);
        rb_state2.var_7 = (rb_state2.var_7) - (((((0x69e1cb9f) ^ (rb_state2.var_6)) ^ (rb_state2.var_5)) + (rb_state2.var_4)) - (rb_state2.var_5));
        rb_state2.var_8 = (rb_state2.var_8) - (rb_state2.var_6);
        rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x1b614c6f));
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_9);
        rb_state2.var_1 = rb_state2.var_2;
        pthread_mutex_unlock(&(rb_state2.lock_26));
    }
    if ((rb_state2.var_0) == (0x931f88f5)) {
        rb_state2.var_27 = !((rb_state2.var_1) == (rb_state2.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xb679f363)) {
        rb_state5.var_3 = 0x36233f45;
        rb_state5.var_4 = 0x41e50dd9;
        rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_4) : (0x5be8ac65));
        rb_state5.var_6 = (rb_state5.var_6) - (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_3) : (0xfdb5aabb));
        rb_state5.var_7 = (rb_state5.var_7) + (((0x71b40009) ^ (rb_state5.var_4)) + (rb_state5.var_5));
        rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_6) : (0x55ad78b1));
        rb_state5.var_9 = (rb_state5.var_9) ^ (((0xa5c9429c) - (0x94e7aaca)) - (rb_state5.var_7));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((0xcbd68933) - (0x852630e8)) ^ (rb_state5.var_8));
        rb_state5.var_11 = (rb_state5.var_11) ^ (((0x814b33f2) ^ (0xc6eeb7c1)) ^ (rb_state5.var_9));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_10) : (0x59ac90ca));
        rb_state5.var_13 = (rb_state5.var_13) - (rb_state5.var_11);
        rb_state5.var_14 = (rb_state5.var_14) - (((((0x8481a3f3) ^ (0x5054b11)) ^ (rb_state5.var_13)) ^ (rb_state5.var_12)) ^ (0x574bde3b));
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (0x41452ba4);
    if ((rb_state8.var_0) == (0x7183bd41)) {
        pthread_mutex_lock(&(rb_state8.lock_45));
        if ((rb_state8.var_1) != (0x0)) {
            if (!((rb_state8.var_1) == (rb_state8.var_23))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_45));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x0)) {
        rb_state12.var_3 = 0xfc62b243;
        rb_state12.var_4 = (rb_state12.var_4) - (((0xbf369e53) + (0x5bee68c5)) + (rb_state12.var_2));
        rb_state12.var_5 = (rb_state12.var_5) ^ (rb_state12.var_3);
        rb_state12.var_6 = (rb_state12.var_6) - (((rb_state12.var_3) == (0xfc62b243)) ? (rb_state12.var_1) : (0x65c82851));
        rb_state12.var_7 = (rb_state12.var_7) ^ (rb_state12.var_4);
        rb_state12.var_8 = (rb_state12.var_8) + (((0x25d96288) ^ (rb_state12.var_5)) + (rb_state12.var_4));
        rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_5) == (0xfc62b243)) ? (rb_state12.var_6) : (0x1e0f63b3));
        rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0xec224ec0));
        rb_state12.var_11 = (rb_state12.var_11) + (((0x10869ded) + (rb_state12.var_8)) + (rb_state12.var_7));
        rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_8) == (0xbe96c9b3)) ? (rb_state12.var_9) : (0x2f1e8c96));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((0x350d5993) ^ (rb_state12.var_9)) + (rb_state12.var_10));
        rb_state12.var_14 = (rb_state12.var_14) ^ (rb_state12.var_11);
        rb_state12.var_15 = (rb_state12.var_15) ^ (((0x4f801835) ^ (rb_state12.var_12)) ^ (rb_state12.var_10));
        rb_state12.var_16 = (rb_state12.var_16) ^ (((rb_state12.var_13) == (0x503260ab)) ? (rb_state12.var_14) : (0x5b19c49));
        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_15) == (0x54a51f2d)) ? (rb_state12.var_16) : (0xd8252194));
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0xec77f5ac)) {
        if ((rb_state15.var_1) != (0x0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_22))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    double T = _start_temp;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_22) == (0x8511daf2)) {
        rb_state2.var_43 = (rb_state2.var_43) ^ (977 < rb_input_size ? (uint32_t)rb_input[977] : 0xdbcda51e);
        rb_state2.var_44 = (rb_state2.var_44) ^ (((((0x79be05be) + (0xa11d537d)) ^ (0x1a3e2e98)) + (rb_state2.var_30)) ^ (rb_state2.var_31));
        rb_state2.var_45 = (rb_state2.var_45) ^ (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_43) : (0xd4be912c));
        rb_state2.var_46 = (rb_state2.var_46) ^ (rb_state2.var_44);
        rb_state2.var_47 = (rb_state2.var_47) - (((0x322da627) ^ (rb_state2.var_45)) ^ (0x6c0e9791));
        rb_state2.var_48 = (rb_state2.var_48) + (((rb_state2.var_32) == (0x0)) ? (rb_state2.var_46) : (0xacc9e3aa));
        rb_state2.var_49 = (rb_state2.var_49) ^ (((0xb0d79b5) + (rb_state2.var_47)) ^ (0xc41bb885));
        rb_state2.var_50 = (rb_state2.var_50) ^ (rb_state2.var_48);
        rb_state2.var_51 = (rb_state2.var_51) ^ (((rb_state2.var_33) == (0x0)) ? (rb_state2.var_49) : (0xc2d79551));
        rb_state2.var_52 = (rb_state2.var_52) ^ (((0x36ac501c) ^ (rb_state2.var_34)) - (rb_state2.var_50));
        rb_state2.var_53 = (rb_state2.var_53) + (((((0x606340d9) ^ (rb_state2.var_52)) ^ (rb_state2.var_35)) + (rb_state2.var_51)) + (0x69abc367));
        rb_state2.var_42 = (rb_state2.var_42) ^ (rb_state2.var_53);
        rb_state2.var_28 = rb_state2.var_42;
    }
    if ((rb_state2.var_22) == (0x8511daf2)) {
        pthread_mutex_lock(&(rb_state2.lock_72));
        rb_state2.var_55 = 0xb602616;
        rb_state2.var_56 = 0xac842092;
        rb_state2.var_57 = (rb_state2.var_57) ^ (rb_state2.var_36);
        rb_state2.var_58 = (rb_state2.var_58) - (rb_state2.var_56);
        rb_state2.var_59 = (rb_state2.var_59) ^ (((rb_state2.var_37) == (0x0)) ? (rb_state2.var_55) : (0xb4138f87));
        rb_state2.var_60 = (rb_state2.var_60) + (((rb_state2.var_38) == (0x0)) ? (rb_state2.var_57) : (0xdc4a628f));
        rb_state2.var_61 = (rb_state2.var_61) - (((0xd1454dec) ^ (rb_state2.var_58)) ^ (rb_state2.var_39));
        rb_state2.var_62 = (rb_state2.var_62) + (rb_state2.var_59);
        rb_state2.var_63 = (rb_state2.var_63) - (((rb_state2.var_40) == (0x0)) ? (rb_state2.var_60) : (0x938c9e12));
        rb_state2.var_64 = (rb_state2.var_64) - (((rb_state2.var_41) == (0x0)) ? (rb_state2.var_61) : (0x5c070af3));
        rb_state2.var_65 = (rb_state2.var_65) ^ (((rb_state2.var_62) == (0x0)) ? (rb_state2.var_63) : (0xbea808e1));
        rb_state2.var_66 = (rb_state2.var_66) ^ (((rb_state2.var_42) == (0x0)) ? (rb_state2.var_64) : (0x245d1ce5));
        rb_state2.var_67 = (rb_state2.var_67) - (((0x7df39301) + (rb_state2.var_43)) ^ (rb_state2.var_65));
        rb_state2.var_68 = (rb_state2.var_68) ^ (rb_state2.var_66);
        rb_state2.var_69 = (rb_state2.var_69) ^ (rb_state2.var_67);
        rb_state2.var_70 = (rb_state2.var_70) - (rb_state2.var_68);
        rb_state2.var_71 = (rb_state2.var_71) + (((rb_state2.var_69) == (0x0)) ? (rb_state2.var_70) : (0xcef2e260));
        rb_state2.var_54 = (rb_state2.var_54) ^ (rb_state2.var_71);
        rb_state2.var_28 = rb_state2.var_54;
        pthread_mutex_unlock(&(rb_state2.lock_72));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x6e91f203)) {
        if ((rb_state3.var_1) != (0x0)) {
            if (!((rb_state3.var_1) == (rb_state3.var_17))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xb679f363)) {
        rb_state5.var_2 = rb_state5.var_1;
    }
    if ((rb_state5.var_0) == (0xb679f363)) {
        pthread_mutex_lock(&(rb_state5.lock_15));
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
        pthread_mutex_unlock(&(rb_state5.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ ((0xb2c0e5dd) - (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0xcf91348)) {
        rb_state19.var_2 = rb_state19.var_1;
    }
    if ((rb_state19.var_0) == (0x1b68fd8b)) {
        if (!((rb_state19.var_1) == (rb_state19.var_2))) {
            racebench_trigger(19);
        }
    }
    #endif
    Rng rng;

    long a_id;
    long b_id;
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xce99a764)) {
        rb_state16.var_3 = (rb_state16.var_3) + (329 < rb_input_size ? (uint32_t)rb_input[329] : 0x79d32b47);
        rb_state16.var_4 = (rb_state16.var_4) + (((0xbaec01d5) + (rb_state16.var_2)) - (rb_state16.var_3));
        rb_state16.var_5 = (rb_state16.var_5) - (((0xc6be66a5) - (0x5e9b92a8)) ^ (rb_state16.var_1));
        rb_state16.var_6 = (rb_state16.var_6) - (((0x45ebb1b4) ^ (rb_state16.var_4)) - (rb_state16.var_3));
        rb_state16.var_7 = (rb_state16.var_7) + (rb_state16.var_4);
        rb_state16.var_8 = (rb_state16.var_8) - (rb_state16.var_5);
        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0xea2e16e4));
        rb_state16.var_10 = (rb_state16.var_10) + (((0xcc229b7c) ^ (rb_state16.var_8)) - (rb_state16.var_5));
        rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0xbd440d8a));
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_11);
    }
    #endif
    netlist_elem *a = _netlist->get_random_element(&a_id, NO_MATCHING_ELEMENT, &rng);
    netlist_elem *b = _netlist->get_random_element(&b_id, NO_MATCHING_ELEMENT, &rng);

    int temp_steps_completed = 0;
    while (keep_going(temp_steps_completed, accepted_good_moves, accepted_bad_moves)) {
        T = T / 1.5;
        accepted_good_moves = 0;
        accepted_bad_moves = 0;

        for (int i = 0; i < _moves_per_thread_temp; i++) {

            a = b;
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0xce99a764)) {
                rb_state16.var_2 = rb_state16.var_1;
            }
            if ((rb_state16.var_0) == (0xce99a764)) {
                if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                    racebench_trigger(16);
                }
            }
            #endif
            a_id = b_id;
            b = _netlist->get_random_element(&b_id, a_id, &rng);

            routing_cost_t delta_cost = calculate_delta_routing_cost(a, b);
            move_decision_t is_good_move = accept_move(delta_cost, T, &rng);

            if (is_good_move == move_decision_accepted_bad) {
                accepted_bad_moves++;
                _netlist->swap_locations(a, b);
            } else if (is_good_move == move_decision_accepted_good) {
                accepted_good_moves++;
                _netlist->swap_locations(a, b);
            } else if (is_good_move == move_decision_rejected) {
            }
        }
        temp_steps_completed++;
#ifdef ENABLE_THREADS
        pthread_barrier_wait(&_barrier);
#endif
    }
}

annealer_thread::move_decision_t annealer_thread::accept_move(routing_cost_t delta_cost, double T, Rng *rng) {

    if (delta_cost < 0) {
        return move_decision_accepted_good;
    } else {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + ((0x90815bc2) - (0x9cdaf1e9));
        #endif
        double random_value = rng->drand();
        double boltzman = exp(-delta_cost / T);
        if (boltzman > random_value) {
            return move_decision_accepted_bad;
        } else {
            return move_decision_rejected;
        }
    }
}

routing_cost_t annealer_thread::calculate_delta_routing_cost(netlist_elem *a, netlist_elem *b) {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (1266 < rb_input_size ? (uint32_t)rb_input[1266] : 0xa174b0f9);
    #endif
    location_t *a_loc = a->present_loc.Get();
    location_t *b_loc = b->present_loc.Get();

    routing_cost_t delta_cost = a->swap_cost(a_loc, b_loc);
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x5234b134)) {
        pthread_mutex_lock(&(rb_state6.lock_47));
        rb_state6.var_6 = (rb_state6.var_6) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_2) : (0x8753a4a1));
        rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_3) : (0x741e51af));
        rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_6);
        rb_state6.var_9 = (rb_state6.var_9) + ((0x503a2e8d) - (0xc4e445da));
        rb_state6.var_10 = (rb_state6.var_10) - (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_6) : (0x82a5ea02));
        rb_state6.var_11 = (rb_state6.var_11) - (((((0xfbdd7d54) + (rb_state6.var_8)) ^ (rb_state6.var_7)) ^ (rb_state6.var_8)) - (0x138fb7c1));
        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_9);
        rb_state6.var_13 = (rb_state6.var_13) - (((0x3d65b144) - (rb_state6.var_10)) ^ (rb_state6.var_9));
        rb_state6.var_14 = (rb_state6.var_14) ^ (((0x58ea7fe8) - (rb_state6.var_10)) ^ (rb_state6.var_11));
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_12) : (0x18df1009));
        rb_state6.var_16 = (rb_state6.var_16) ^ (((0x375d8345) - (rb_state6.var_12)) ^ (rb_state6.var_13));
        rb_state6.var_17 = (rb_state6.var_17) + (((((0xb6effaf6) - (rb_state6.var_14)) ^ (0xead61004)) ^ (rb_state6.var_13)) + (rb_state6.var_15));
        rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_16) : (0x587e5d5f));
        rb_state6.var_19 = (rb_state6.var_19) - (((0xed369d52) - (rb_state6.var_17)) - (0x4a6f29f));
        rb_state6.var_20 = (rb_state6.var_20) ^ (rb_state6.var_18);
        rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_19) : (0x7c9ff442));
        rb_state6.var_22 = (rb_state6.var_22) - (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_20) : (0x51dc1d89));
        rb_state6.var_23 = (rb_state6.var_23) ^ (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_21) : (0xbc4946fa));
        rb_state6.var_24 = (rb_state6.var_24) ^ (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_22) : (0xa51ff407));
        rb_state6.var_25 = (rb_state6.var_25) ^ (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_23) : (0x11996bcc));
        rb_state6.var_26 = (rb_state6.var_26) ^ (rb_state6.var_24);
        rb_state6.var_27 = (rb_state6.var_27) - (rb_state6.var_25);
        rb_state6.var_28 = (rb_state6.var_28) ^ (((0xca9774cb) - (0xfa5e6bb7)) - (rb_state6.var_26));
        rb_state6.var_29 = (rb_state6.var_29) ^ (rb_state6.var_27);
        rb_state6.var_30 = (rb_state6.var_30) - (rb_state6.var_28);
        rb_state6.var_31 = (rb_state6.var_31) + (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_29) : (0x859f0475));
        rb_state6.var_32 = (rb_state6.var_32) + (rb_state6.var_30);
        rb_state6.var_33 = (rb_state6.var_33) ^ (((((0xe177ff37) - (rb_state6.var_31)) + (rb_state6.var_32)) + (rb_state6.var_21)) + (rb_state6.var_22));
        rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_33);
        rb_state6.var_1 = rb_state6.var_5;
        pthread_mutex_unlock(&(rb_state6.lock_47));
    }
    if ((rb_state6.var_0) == (0x80)) {
        pthread_mutex_lock(&(rb_state6.lock_47));
        rb_state6.var_35 = (rb_state6.var_35) ^ (rb_state6.var_26);
        rb_state6.var_36 = (rb_state6.var_36) - ((0x8e6a5e59) + (0x71dedf4e));
        rb_state6.var_37 = (rb_state6.var_37) + (((((0x22f02db4) ^ (rb_state6.var_25)) ^ (0xdbf4c6ca)) ^ (rb_state6.var_24)) ^ (rb_state6.var_27));
        rb_state6.var_38 = (rb_state6.var_38) - (((((0xd50cd7fd) + (0x2fba2417)) - (rb_state6.var_35)) ^ (rb_state6.var_23)) - (0xcb67f83f));
        rb_state6.var_39 = (rb_state6.var_39) + (((0xb2a8c378) + (rb_state6.var_28)) ^ (rb_state6.var_36));
        rb_state6.var_40 = (rb_state6.var_40) + (((rb_state6.var_29) == (0x0)) ? (rb_state6.var_37) : (0xc8486cd4));
        rb_state6.var_41 = (rb_state6.var_41) - (rb_state6.var_38);
        rb_state6.var_42 = (rb_state6.var_42) - (((((0xc37787e0) + (rb_state6.var_30)) + (rb_state6.var_39)) - (rb_state6.var_40)) + (0x58681dd8));
        rb_state6.var_43 = (rb_state6.var_43) + (rb_state6.var_41);
        rb_state6.var_44 = (rb_state6.var_44) - (((0xc3daaad0) ^ (0x810d04e2)) + (rb_state6.var_42));
        rb_state6.var_45 = (rb_state6.var_45) - (((0xf5d9b6f7) + (rb_state6.var_43)) - (0xde6c20db));
        rb_state6.var_46 = (rb_state6.var_46) - (((rb_state6.var_44) == (0x0)) ? (rb_state6.var_45) : (0xcf92bd19));
        rb_state6.var_34 = (rb_state6.var_34) ^ (rb_state6.var_46);
        rb_state6.var_1 = rb_state6.var_34;
        pthread_mutex_unlock(&(rb_state6.lock_47));
    }
    #endif
    delta_cost += b->swap_cost(b_loc, a_loc);

    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xfec330c8)) {
        pthread_mutex_lock(&(rb_state7.lock_27));
        rb_state7.var_3 = 0xff95c45e;
        rb_state7.var_4 = (rb_state7.var_4) ^ (rb_state7.var_3);
        rb_state7.var_5 = (rb_state7.var_5) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_3) : (0x2a023c55));
        rb_state7.var_6 = (rb_state7.var_6) + (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0xaa5563ce));
        rb_state7.var_7 = (rb_state7.var_7) - (rb_state7.var_1);
        rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_4) : (0x221c74a5));
        rb_state7.var_9 = (rb_state7.var_9) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_5) : (0x7ba1c472));
        rb_state7.var_10 = (rb_state7.var_10) - (((0xf773aeba) - (0x8cbac8da)) - (rb_state7.var_6));
        rb_state7.var_11 = (rb_state7.var_11) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_7) : (0xaa6aaad1));
        rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_8);
        rb_state7.var_13 = (rb_state7.var_13) + (rb_state7.var_9);
        rb_state7.var_14 = (rb_state7.var_14) ^ (rb_state7.var_10);
        rb_state7.var_15 = (rb_state7.var_15) - (((((0x53113bac) ^ (rb_state7.var_11)) + (rb_state7.var_12)) - (0x164edd36)) ^ (rb_state7.var_9));
        rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_13) : (0xb6a8c4c0));
        rb_state7.var_17 = (rb_state7.var_17) + (((0x840f40dd) + (rb_state7.var_11)) ^ (rb_state7.var_14));
        rb_state7.var_18 = (rb_state7.var_18) ^ (((0x969e3c9c) - (rb_state7.var_15)) ^ (0x6c371b39));
        rb_state7.var_19 = (rb_state7.var_19) - (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_17) : (0xf9e6f2d));
        rb_state7.var_20 = (rb_state7.var_20) ^ (((((0xc4e773e9) ^ (rb_state7.var_19)) ^ (0x17c5e4ef)) - (rb_state7.var_18)) + (0x8c5598d1));
        rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_20);
        rb_state7.var_1 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_27));
    }
    if ((rb_state7.var_0) == (0xfec3310c)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    return delta_cost;
}

bool annealer_thread::keep_going(int temp_steps_completed, int accepted_good_moves, int accepted_bad_moves) {
    bool rv;

    if (_number_temp_steps == -1) {

        rv = _keep_going_global_flag && (accepted_good_moves > accepted_bad_moves);
        if (!rv) {
            _keep_going_global_flag = false;
        }
    } else {

        rv = temp_steps_completed < _number_temp_steps;
    }

    return rv;
}