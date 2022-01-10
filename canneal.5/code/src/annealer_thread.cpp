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
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x57cc7753)) {
        if ((rb_state5.var_5) != (0x0)) {
            if (!((rb_state5.var_5) == (rb_state5.var_22))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + ((0xeaf1275c) + (0xb43887ce));
    if ((rb_state7.var_3) == (0xb5128d27)) {
        rb_state7.var_24 = 0x830bd64d;
        rb_state7.var_25 = 0xde17aa9a;
        rb_state7.var_26 = (rb_state7.var_26) - (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_17) : (0x1bc3320f));
        rb_state7.var_27 = (rb_state7.var_27) - (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_25) : (0x35b3c0b6));
        rb_state7.var_28 = (rb_state7.var_28) + (((0x3c6da8ca) + (rb_state7.var_24)) ^ (0x87151f16));
        rb_state7.var_29 = (rb_state7.var_29) ^ (rb_state7.var_26);
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_28) : (0xc1a06539));
        rb_state7.var_31 = (rb_state7.var_31) - (((((0x1dd420a4) ^ (rb_state7.var_19)) ^ (rb_state7.var_20)) ^ (rb_state7.var_29)) + (rb_state7.var_30));
        rb_state7.var_23 = (rb_state7.var_23) - (rb_state7.var_31);
        rb_state7.var_4 = rb_state7.var_23;
    }
    if ((rb_state7.var_3) == (0xb5128d27)) {
        pthread_mutex_lock(&(rb_state7.lock_40));
        rb_state7.var_33 = 0xe24873ec;
        rb_state7.var_34 = 0xb1fc01ae;
        rb_state7.var_35 = (rb_state7.var_35) ^ (((rb_state7.var_34) == (0x0)) ? (rb_state7.var_33) : (0x613ede81));
        rb_state7.var_36 = (rb_state7.var_36) - (rb_state7.var_22);
        rb_state7.var_37 = (rb_state7.var_37) + (((((0x31dd18e3) ^ (0xb798f51d)) ^ (0x40b33459)) + (rb_state7.var_35)) ^ (rb_state7.var_21));
        rb_state7.var_38 = (rb_state7.var_38) + (((0x661eb46c) ^ (rb_state7.var_23)) - (rb_state7.var_36));
        rb_state7.var_39 = (rb_state7.var_39) - (((((0x7b50fcdc) ^ (rb_state7.var_37)) - (0x2e997483)) + (0x33120402)) - (rb_state7.var_38));
        rb_state7.var_32 = (rb_state7.var_32) + (rb_state7.var_39);
        rb_state7.var_4 = rb_state7.var_32;
        pthread_mutex_unlock(&(rb_state7.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ ((0x62cca240) ^ (rb_state8.var_1));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_1) : (0xaa4e03f7));
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_1);
    rb_state9.var_0 = (rb_state9.var_0) + ((0x56f41937) ^ (0x9a95a908));
    if ((rb_state9.var_1) == (0xe36a7425)) {
        rb_state9.var_18 = 0xbb86f172;
        rb_state9.var_19 = 0xc469fc17;
        rb_state9.var_20 = 0xe3b122cd;
        rb_state9.var_21 = (rb_state9.var_21) ^ (598 < rb_input_size ? (uint32_t)rb_input[598] : 0xc2b3d760);
        rb_state9.var_22 = (rb_state9.var_22) ^ (rb_state9.var_19);
        rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_20);
        rb_state9.var_24 = (rb_state9.var_24) ^ (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_21) : (0xf4a702f));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_22) : (0xeebec5f3));
        rb_state9.var_26 = (rb_state9.var_26) + (((((0xa0762402) + (rb_state9.var_9)) ^ (rb_state9.var_23)) - (rb_state9.var_24)) ^ (rb_state9.var_10));
        rb_state9.var_27 = (rb_state9.var_27) ^ (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_26) : (0x337c0200));
        rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_27);
        rb_state9.var_4 = rb_state9.var_17;
    }
    if ((rb_state9.var_1) == (0xe36a7425)) {
        pthread_mutex_lock(&(rb_state9.lock_39));
        rb_state9.var_29 = 0x40507f7e;
        rb_state9.var_30 = (rb_state9.var_30) ^ (((rb_state9.var_29) == (0x0)) ? (rb_state9.var_12) : (0xc50b3c58));
        rb_state9.var_31 = (rb_state9.var_31) - (rb_state9.var_13);
        rb_state9.var_32 = (rb_state9.var_32) + (((((0x5a19c9f1) ^ (0x9f6cef98)) + (rb_state9.var_14)) + (rb_state9.var_11)) + (rb_state9.var_30));
        rb_state9.var_33 = (rb_state9.var_33) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_31) : (0x5d5c5acd));
        rb_state9.var_34 = (rb_state9.var_34) + (((0xb472f777) - (rb_state9.var_32)) ^ (rb_state9.var_16));
        rb_state9.var_35 = (rb_state9.var_35) - (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_33) : (0x225a846));
        rb_state9.var_36 = (rb_state9.var_36) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_34) : (0xa6977f98));
        rb_state9.var_37 = (rb_state9.var_37) ^ (((0x8f3cfa22) ^ (rb_state9.var_35)) - (0x5033dd1f));
        rb_state9.var_38 = (rb_state9.var_38) - (((rb_state9.var_36) == (0x0)) ? (rb_state9.var_37) : (0xe6839b4e));
        rb_state9.var_28 = (rb_state9.var_28) + (rb_state9.var_38);
        rb_state9.var_4 = rb_state9.var_28;
        pthread_mutex_unlock(&(rb_state9.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0x2df6b74c));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_3) : (0x230e5855));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
    rb_state15.var_3 = (rb_state15.var_3) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_3) : (0xd0aa6b57));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_4 = (rb_state16.var_4) + (1000 < rb_input_size ? (uint32_t)rb_input[1000] : 0x84af99a7);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (1797 < rb_input_size ? (uint32_t)rb_input[1797] : 0xcfa99bac);
    #endif
    int accepted_good_moves = 0;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x57cc7753)) {
        rb_state5.var_9 = 0x7f2f7df9;
        rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_9) == (0x7f2f7df9)) ? (rb_state5.var_7) : (0xb10cc655));
        rb_state5.var_11 = (rb_state5.var_11) + (((0x67657eb5) + (rb_state5.var_10)) + (rb_state5.var_8));
        rb_state5.var_12 = (rb_state5.var_12) ^ (0x1a1c9211);
        rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_9) == (0x7f2f7df9)) ? (rb_state5.var_10) : (0xbee10cbc));
        rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_11) == (0x67657eb5)) ? (rb_state5.var_11) : (0xece6d38));
        rb_state5.var_15 = (rb_state5.var_15) ^ (((0x9f7c3958) - (0x1ab02ab1)) ^ (rb_state5.var_12));
        rb_state5.var_16 = (rb_state5.var_16) ^ (((0x5ff06248) ^ (0x1062585b)) - (rb_state5.var_13));
        rb_state5.var_17 = (rb_state5.var_17) + (((rb_state5.var_12) == (0x1a1c9211)) ? (rb_state5.var_14) : (0xb46d80e2));
        rb_state5.var_18 = (rb_state5.var_18) + (((rb_state5.var_15) == (0x9ed09cb6)) ? (rb_state5.var_16) : (0x26f9f45a));
        rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_17) : (0x11504a2d));
        rb_state5.var_20 = (rb_state5.var_20) + (((0x105f61e9) - (rb_state5.var_18)) - (0xda4da597));
        rb_state5.var_21 = (rb_state5.var_21) ^ (((rb_state5.var_19) == (0x989a814b)) ? (rb_state5.var_20) : (0xe1371832));
        rb_state5.var_8 = (rb_state5.var_8) - (rb_state5.var_21);
        rb_state5.var_5 = rb_state5.var_8;
    }
    if ((rb_state5.var_3) == (0x57cc7753)) {
        pthread_mutex_lock(&(rb_state5.lock_27));
        rb_state5.var_23 = 0xca30a2b4;
        rb_state5.var_24 = 0x3b1d92c6;
        rb_state5.var_25 = (rb_state5.var_25) + (((((0xf6433617) ^ (rb_state5.var_16)) - (rb_state5.var_15)) - (rb_state5.var_23)) + (rb_state5.var_24));
        rb_state5.var_26 = (rb_state5.var_26) - (((rb_state5.var_14) == (0x989a814b)) ? (rb_state5.var_25) : (0xdef929cd));
        rb_state5.var_22 = (rb_state5.var_22) + (rb_state5.var_26);
        rb_state5.var_5 = rb_state5.var_22;
        pthread_mutex_unlock(&(rb_state5.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (0xf246107f);
    rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_0);
    rb_state8.var_1 = (rb_state8.var_1) ^ ((0x1bb84f49) + (rb_state8.var_2));
    rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_0);
    rb_state8.var_4 = (rb_state8.var_4) - (rb_state8.var_4);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - ((0xbf07614c) - (0x680a7e25));
    rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_2) == (0x9e3d552)) ? (rb_state9.var_1) : (0x2766d1b1));
    rb_state9.var_3 = (rb_state9.var_3) - (0x8d3bdec9);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
    rb_state11.var_2 = (rb_state11.var_2) + ((0x235ea67e) ^ (rb_state11.var_1));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (14 < rb_input_size ? (uint32_t)rb_input[14] : 0x16f84129);
    rb_state15.var_3 = (rb_state15.var_3) - (0x84dfc8c5);
    rb_state15.var_1 = (rb_state15.var_1) ^ (0x9517c319);
    rb_state15.var_2 = (rb_state15.var_2) - (0x41033899);
    if ((rb_state15.var_3) == (0xf6406e76)) {
        rb_state15.var_4 = rb_state15.var_0;
    }
    if ((rb_state15.var_3) == (0xf6406e76)) {
        if (!((rb_state15.var_0) == (rb_state15.var_4))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_3);
    if ((rb_state16.var_2) == (0x0)) {
        rb_state16.var_9 = 0x20cd7700;
        rb_state16.var_10 = 0x3f7ff89d;
        rb_state16.var_11 = (rb_state16.var_11) + (583 < rb_input_size ? (uint32_t)rb_input[583] : 0x2d5e220a);
        rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_10) : (0xaf03f619));
        rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_9) : (0xa6abb2ea));
        rb_state16.var_14 = (rb_state16.var_14) + (rb_state16.var_11);
        rb_state16.var_15 = (rb_state16.var_15) ^ (((0xca0ba577) - (rb_state16.var_12)) ^ (rb_state16.var_10));
        rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_13) : (0x10c9f85e));
        rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_14) : (0xdf7390e3));
        rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_15) : (0xabd7f63));
        rb_state16.var_19 = (rb_state16.var_19) ^ (((0x7d8de9b4) - (rb_state16.var_14)) - (rb_state16.var_16));
        rb_state16.var_20 = (rb_state16.var_20) + (((((0x5de62d3d) ^ (rb_state16.var_15)) ^ (rb_state16.var_18)) ^ (0x77426205)) - (rb_state16.var_17));
        rb_state16.var_21 = (rb_state16.var_21) - (((((0x1c85cc6e) + (rb_state16.var_19)) - (rb_state16.var_20)) - (rb_state16.var_16)) + (0xda181a3));
        rb_state16.var_8 = (rb_state16.var_8) + (rb_state16.var_21);
        rb_state16.var_5 = rb_state16.var_8;
    }
    if ((rb_state16.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_31));
        rb_state16.var_23 = 0xdc5175cf;
        rb_state16.var_24 = 0xbcdcc784;
        rb_state16.var_25 = 0xe50646ef;
        rb_state16.var_26 = (rb_state16.var_26) + (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_24) : (0x6058e25b));
        rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_25) : (0xb9264cec));
        rb_state16.var_28 = (rb_state16.var_28) + (((0x3f084272) ^ (rb_state16.var_18)) + (rb_state16.var_26));
        rb_state16.var_29 = (rb_state16.var_29) + (rb_state16.var_27);
        rb_state16.var_30 = (rb_state16.var_30) - (((((0x3d9abaa6) ^ (0x4ed1b763)) + (rb_state16.var_29)) - (rb_state16.var_19)) + (rb_state16.var_28));
        rb_state16.var_22 = (rb_state16.var_22) - (rb_state16.var_30);
        rb_state16.var_5 = rb_state16.var_22;
        pthread_mutex_unlock(&(rb_state16.lock_31));
    }
    #endif
    int accepted_bad_moves = -1;
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_25));
        rb_state12.var_7 = 0xbd1a1152;
        rb_state12.var_8 = (rb_state12.var_8) ^ (rb_state12.var_7);
        rb_state12.var_9 = (rb_state12.var_9) - (rb_state12.var_5);
        rb_state12.var_10 = (rb_state12.var_10) - (rb_state12.var_6);
        rb_state12.var_11 = (rb_state12.var_11) ^ (((((0xfb20c895) ^ (rb_state12.var_8)) + (rb_state12.var_8)) + (rb_state12.var_7)) + (rb_state12.var_9));
        rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0xbca29dbe));
        rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_11);
        rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_12) : (0x267c13bc));
        rb_state12.var_15 = (rb_state12.var_15) - (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_13) : (0xa941cb58));
        rb_state12.var_16 = (rb_state12.var_16) + (rb_state12.var_14);
        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0xfb3d3478));
        rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_17);
        rb_state12.var_5 = rb_state12.var_6;
        pthread_mutex_unlock(&(rb_state12.lock_25));
    }
    if ((rb_state12.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_25));
        if (!((rb_state12.var_5) == (rb_state12.var_6))) {
            racebench_trigger(12);
        }
        pthread_mutex_unlock(&(rb_state12.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0xf6406e76)) {
        pthread_mutex_lock(&(rb_state15.lock_13));
        rb_state15.var_5 = 0x6c707570;
        rb_state15.var_6 = (rb_state15.var_6) + (rb_state15.var_5);
        rb_state15.var_7 = (rb_state15.var_7) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_5) : (0xf4e62241));
        rb_state15.var_8 = (rb_state15.var_8) - (0xdf86b7ca);
        rb_state15.var_9 = (rb_state15.var_9) + (rb_state15.var_6);
        rb_state15.var_10 = (rb_state15.var_10) ^ (((((0x23fb6c35) - (0x760dabbe)) - (0x5d81ab2e)) + (rb_state15.var_7)) ^ (rb_state15.var_8));
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_9) : (0xa4b77464));
        rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0xa43fb393));
        rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_12);
        pthread_mutex_unlock(&(rb_state15.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_4 = (rb_state16.var_4) ^ (1838 < rb_input_size ? (uint32_t)rb_input[1838] : 0xd7d43734);
    #endif
    double T = _start_temp;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_4) == (0x0)) {
        rb_state8.var_5 = rb_state8.var_3;
    }
    if ((rb_state8.var_4) == (0x0)) {
        if (!((rb_state8.var_3) == (rb_state8.var_5))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0xe36a7425)) {
        pthread_mutex_lock(&(rb_state9.lock_39));
        if ((rb_state9.var_4) != (0x0)) {
            if (!((rb_state9.var_4) == (rb_state9.var_28))) {
                racebench_trigger(9);
            }
        }
        pthread_mutex_unlock(&(rb_state9.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - ((0x11e6b096) - (rb_state12.var_2));
    rb_state12.var_1 = (rb_state12.var_1) - (0x9552a628);
    rb_state12.var_3 = (rb_state12.var_3) - (0xd0f64caa);
    rb_state12.var_4 = (rb_state12.var_4) + (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_4) : (0x9e849499));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - ((0x60b93d54) ^ (0x21af0955));
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_1);
    if ((rb_state16.var_2) == (0x0)) {
        if ((rb_state16.var_5) != (0x0)) {
            if (!((rb_state16.var_5) == (rb_state16.var_22))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    Rng rng;

    long a_id;
    long b_id;
    netlist_elem *a = _netlist->get_random_element(&a_id, NO_MATCHING_ELEMENT, &rng);
    netlist_elem *b = _netlist->get_random_element(&b_id, NO_MATCHING_ELEMENT, &rng);

    int temp_steps_completed = 0;
    while (keep_going(temp_steps_completed, accepted_good_moves, accepted_bad_moves)) {
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0xf4bd4f47)) {
            rb_state14.var_28 = 0x27583e0e;
            rb_state14.var_29 = 0x28f616b7;
            rb_state14.var_30 = 0xee621f68;
            rb_state14.var_31 = (rb_state14.var_31) - (((rb_state14.var_29) == (0x0)) ? (rb_state14.var_30) : (0x936ed1f2));
            rb_state14.var_32 = (rb_state14.var_32) ^ (((((0x8766c0ce) - (rb_state14.var_31)) ^ (0x258b852b)) + (0x8d54f51f)) - (rb_state14.var_28));
            rb_state14.var_27 = (rb_state14.var_27) ^ (rb_state14.var_32);
            rb_state14.var_4 = rb_state14.var_27;
        }
        if ((rb_state14.var_0) == (0xf4bd4f47)) {
            pthread_mutex_lock(&(rb_state14.lock_40));
            rb_state14.var_34 = 0x91a075d0;
            rb_state14.var_35 = (rb_state14.var_35) - (((0x2ee954a4) + (rb_state14.var_34)) + (0x85c4f9fd));
            rb_state14.var_36 = (rb_state14.var_36) ^ (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_16) : (0x69e3a016));
            rb_state14.var_37 = (rb_state14.var_37) ^ (((0x154fa42f) - (rb_state14.var_18)) + (rb_state14.var_35));
            rb_state14.var_38 = (rb_state14.var_38) ^ (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_36) : (0x32adbb76));
            rb_state14.var_39 = (rb_state14.var_39) + (((((0xdc8ff3ec) + (0xc8d491d8)) ^ (rb_state14.var_38)) - (rb_state14.var_37)) ^ (rb_state14.var_20));
            rb_state14.var_33 = (rb_state14.var_33) + (rb_state14.var_39);
            rb_state14.var_4 = rb_state14.var_33;
            pthread_mutex_unlock(&(rb_state14.lock_40));
        }
        #endif
        T = T / 1.5;
        accepted_good_moves = 0;
        accepted_bad_moves = 0;

        for (int i = 0; i < _moves_per_thread_temp; i++) {

            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) - (0x2021044c);
            #endif
            a = b;
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_3);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) - ((0xadfb7509) + (0xfac39e97));
            #endif
            a_id = b_id;
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_3);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_1);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_1);
            #endif
            b = _netlist->get_random_element(&b_id, a_id, &rng);

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0x4122d88f));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_2);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + ((0x57de1cd8) - (rb_state19.var_0));
            #endif
            routing_cost_t delta_cost = calculate_delta_routing_cost(a, b);
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
            if ((rb_state3.var_4) == (0xb5a58cfd)) {
                pthread_mutex_lock(&(rb_state3.lock_45));
                rb_state3.var_17 = 0xa3a26bd5;
                rb_state3.var_18 = (rb_state3.var_18) + (rb_state3.var_10);
                rb_state3.var_19 = (rb_state3.var_19) + (rb_state3.var_9);
                rb_state3.var_20 = (rb_state3.var_20) - (0x27e48300);
                rb_state3.var_21 = (rb_state3.var_21) + (rb_state3.var_17);
                rb_state3.var_22 = (rb_state3.var_22) - (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_18) : (0x83476131));
                rb_state3.var_23 = (rb_state3.var_23) + (((0xf2ed4247) ^ (0x2abbb704)) + (rb_state3.var_19));
                rb_state3.var_24 = (rb_state3.var_24) ^ (((((0x8b42b316) + (rb_state3.var_20)) - (0xd1c84c9a)) + (0xdac69f41)) - (rb_state3.var_21));
                rb_state3.var_25 = (rb_state3.var_25) ^ (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_22) : (0x347ded02));
                rb_state3.var_26 = (rb_state3.var_26) - (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_23) : (0xffe15070));
                rb_state3.var_27 = (rb_state3.var_27) ^ (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_24) : (0x252f74f0));
                rb_state3.var_28 = (rb_state3.var_28) + (((((0xd4673139) - (0x70e171e2)) ^ (0x7a581e2e)) ^ (rb_state3.var_26)) + (rb_state3.var_25));
                rb_state3.var_29 = (rb_state3.var_29) ^ (((0x999ad7e7) ^ (0x990625d6)) + (rb_state3.var_27));
                rb_state3.var_30 = (rb_state3.var_30) ^ (rb_state3.var_28);
                rb_state3.var_31 = (rb_state3.var_31) ^ (((rb_state3.var_29) == (0x0)) ? (rb_state3.var_30) : (0xc06eccb7));
                rb_state3.var_16 = (rb_state3.var_16) - (rb_state3.var_31);
                rb_state3.var_5 = rb_state3.var_16;
                pthread_mutex_unlock(&(rb_state3.lock_45));
            }
            if ((rb_state3.var_4) == (0xb5a58cfd)) {
                pthread_mutex_lock(&(rb_state3.lock_45));
                rb_state3.var_33 = 0x873e636c;
                rb_state3.var_34 = 0xabf63a2b;
                rb_state3.var_35 = (rb_state3.var_35) ^ (rb_state3.var_33);
                rb_state3.var_36 = (rb_state3.var_36) ^ (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_15) : (0x91affc92));
                rb_state3.var_37 = (rb_state3.var_37) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_34) : (0xbabafc79));
                rb_state3.var_38 = (rb_state3.var_38) ^ (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_35) : (0x18495697));
                rb_state3.var_39 = (rb_state3.var_39) ^ (((0x72970467) + (0x5f3bb9)) - (rb_state3.var_36));
                rb_state3.var_40 = (rb_state3.var_40) - (((rb_state3.var_18) == (0x0)) ? (rb_state3.var_37) : (0xf4ec6cd9));
                rb_state3.var_41 = (rb_state3.var_41) + (((0xeb42722) ^ (rb_state3.var_38)) ^ (rb_state3.var_19));
                rb_state3.var_42 = (rb_state3.var_42) - (((rb_state3.var_39) == (0x0)) ? (rb_state3.var_40) : (0x62d9e5eb));
                rb_state3.var_43 = (rb_state3.var_43) + (rb_state3.var_41);
                rb_state3.var_44 = (rb_state3.var_44) ^ (((rb_state3.var_42) == (0x0)) ? (rb_state3.var_43) : (0x183cb71c));
                rb_state3.var_32 = (rb_state3.var_32) + (rb_state3.var_44);
                rb_state3.var_5 = rb_state3.var_32;
                pthread_mutex_unlock(&(rb_state3.lock_45));
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_1) == (0x15765d95)) {
                pthread_mutex_lock(&(rb_state6.lock_42));
                rb_state6.var_6 = 0x15a7a2bd;
                rb_state6.var_7 = (rb_state6.var_7) ^ (rb_state6.var_6);
                rb_state6.var_8 = (rb_state6.var_8) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_4) : (0xec12441e));
                rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0x1931ada7));
                rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0xc481a4f6));
                rb_state6.var_11 = (rb_state6.var_11) - (((0x95b68dfc) + (rb_state6.var_7)) ^ (0xb7248f3e));
                rb_state6.var_12 = (rb_state6.var_12) - (rb_state6.var_8);
                rb_state6.var_13 = (rb_state6.var_13) - (((0x54c0590) - (rb_state6.var_9)) ^ (0x5ef5c231));
                rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0x8620b15));
                rb_state6.var_15 = (rb_state6.var_15) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_12) : (0x677d05c2));
                rb_state6.var_16 = (rb_state6.var_16) ^ (((((0x39a78fc5) - (rb_state6.var_14)) ^ (0x8400c5f0)) ^ (0xa104e75e)) + (rb_state6.var_13));
                rb_state6.var_17 = (rb_state6.var_17) ^ (((0x5bc46b60) ^ (rb_state6.var_11)) ^ (rb_state6.var_15));
                rb_state6.var_18 = (rb_state6.var_18) + (((0x9bbf05b8) + (rb_state6.var_16)) ^ (0xec88e76));
                rb_state6.var_19 = (rb_state6.var_19) + (((0xf9d9811) + (rb_state6.var_12)) + (rb_state6.var_17));
                rb_state6.var_20 = (rb_state6.var_20) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_19) : (0xfefd22b3));
                rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_20);
                rb_state6.var_4 = rb_state6.var_5;
                pthread_mutex_unlock(&(rb_state6.lock_42));
            }
            if ((rb_state6.var_1) == (0x15765d95)) {
                if (!((rb_state6.var_4) == (rb_state6.var_5))) {
                    racebench_trigger(6);
                }
            }
            #endif
            move_decision_t is_good_move = accept_move(delta_cost, T, &rng);

            if (is_good_move == move_decision_accepted_bad) {
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_3 = (rb_state10.var_3) + ((0x251177db) ^ (0x6679011a));
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_4) == (0x815a4a2)) {
                    rb_state19.var_12 = 0x2dd8bfd;
                    rb_state19.var_13 = 0x523416a7;
                    rb_state19.var_14 = (rb_state19.var_14) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0xa1db9e53));
                    rb_state19.var_15 = (rb_state19.var_15) ^ (((0x17639dfb) - (0xbc529b5)) - (rb_state19.var_13));
                    rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_7);
                    rb_state19.var_17 = (rb_state19.var_17) + (rb_state19.var_12);
                    rb_state19.var_18 = (rb_state19.var_18) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0xc2a919ff));
                    rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_16) : (0x432f75ea));
                    rb_state19.var_20 = (rb_state19.var_20) - (((0x7ed645d5) + (rb_state19.var_11)) - (rb_state19.var_17));
                    rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_19) : (0x9def2d90));
                    rb_state19.var_22 = (rb_state19.var_22) + (rb_state19.var_20);
                    rb_state19.var_23 = (rb_state19.var_23) + (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_21) : (0x7274ebda));
                    rb_state19.var_24 = (rb_state19.var_24) - (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_23) : (0xec05fc33));
                    rb_state19.var_5 = (rb_state19.var_5) + (rb_state19.var_24);
                }
                #endif
                accepted_bad_moves++;
                _netlist->swap_locations(a, b);
            } else if (is_good_move == move_decision_accepted_good) {
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) + (0x6720053);
                #endif
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
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_2) == (0x5819a547)) {
            rb_state1.var_11 = 0x6c89d40e;
            rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_6);
            rb_state1.var_13 = (rb_state1.var_13) + (rb_state1.var_7);
            rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_5) : (0xe54b0821));
            rb_state1.var_15 = (rb_state1.var_15) + (((0x8666292f) ^ (rb_state1.var_9)) + (rb_state1.var_11));
            rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_12) : (0xb535200));
            rb_state1.var_17 = (rb_state1.var_17) - (((((0x9614a82a) ^ (rb_state1.var_12)) + (rb_state1.var_11)) + (rb_state1.var_14)) - (rb_state1.var_13));
            rb_state1.var_18 = (rb_state1.var_18) - (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_16) : (0x1353299f));
            rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0x4066f494));
            rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_19);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) - ((0x81c05f) ^ (0xc7960ddf));
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
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_3) : (0xaab1e5d2));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) ^ (1306 < rb_input_size ? (uint32_t)rb_input[1306] : 0x1de47c77);
    #endif
    location_t *a_loc = a->present_loc.Get();
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (0xd30283fb);
    #endif
    location_t *b_loc = b->present_loc.Get();

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0x2dd05945));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0xb81865d1));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xeff00867));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_4) == (0xd4bb5634)) {
        if ((rb_state17.var_5) != (0x0)) {
            if (!((rb_state17.var_5) == (rb_state17.var_30))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    routing_cost_t delta_cost = a->swap_cost(a_loc, b_loc);
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0x844eee00);
    #endif
    delta_cost += b->swap_cost(b_loc, a_loc);

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

        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (81 < rb_input_size ? (uint32_t)rb_input[81] : 0x4e0a9716);
        #endif
        rv = temp_steps_completed < _number_temp_steps;
    }

    return rv;
}