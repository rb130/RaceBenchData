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
    rb_state0.var_0 = (rb_state0.var_0) ^ ((0x60d544e2) ^ (0x9db86f5e));
    rb_state0.var_2 = (rb_state0.var_2) + (0x52314372);
    if ((rb_state0.var_2) == (0xa46286e4)) {
        pthread_mutex_lock(&(rb_state0.lock_14));
        rb_state0.var_3 = rb_state0.var_1;
        pthread_mutex_unlock(&(rb_state0.lock_14));
    }
    if ((rb_state0.var_2) == (0xa46286e4)) {
        if (!((rb_state0.var_1) == (rb_state0.var_3))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (1499 < rb_input_size ? (uint32_t)rb_input[1499] : 0x8e6a4a6d);
    rb_state1.var_2 = (rb_state1.var_2) + ((0x7c130504) + (0xe336e2eb));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (587 < rb_input_size ? (uint32_t)rb_input[587] : 0xe23ad2f1);
    rb_state2.var_1 = (rb_state2.var_1) - (0x197d14e9);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_1) : (0xa7c179e5));
    rb_state9.var_1 = (rb_state9.var_1) + (0x80bc2954);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_24) == (0xd6ab6e85)) {
        pthread_mutex_lock(&(rb_state13.lock_90));
        if ((rb_state13.var_28) != (0x0)) {
            if (!((rb_state13.var_28) == (rb_state13.var_69))) {
                racebench_trigger(13);
            }
        }
        pthread_mutex_unlock(&(rb_state13.lock_90));
    }
    #endif
    int accepted_good_moves = 0;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0xa46286e4)) {
        pthread_mutex_lock(&(rb_state0.lock_14));
        rb_state0.var_4 = 0xd770794;
        rb_state0.var_5 = 0x8271ce58;
        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_4);
        rb_state0.var_7 = (rb_state0.var_7) + (((((0xd9f6e0d9) - (rb_state0.var_5)) - (rb_state0.var_6)) ^ (rb_state0.var_5)) ^ (rb_state0.var_3));
        rb_state0.var_8 = (rb_state0.var_8) + (((rb_state0.var_4) == (0xd770794)) ? (rb_state0.var_6) : (0x4c05a255));
        rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_7) == (0x9560842d)) ? (rb_state0.var_7) : (0xbe3fb9f6));
        rb_state0.var_10 = (rb_state0.var_10) + (((rb_state0.var_8) == (0xd770794)) ? (rb_state0.var_8) : (0x7b6bf7fc));
        rb_state0.var_11 = (rb_state0.var_11) - (((0x5064a0fc) ^ (rb_state0.var_9)) + (rb_state0.var_9));
        rb_state0.var_12 = (rb_state0.var_12) + (((0xd12ab6a8) - (0x2dc21823)) - (rb_state0.var_10));
        rb_state0.var_13 = (rb_state0.var_13) + (((rb_state0.var_11) == (0xa59b5702)) ? (rb_state0.var_12) : (0xd2849d72));
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_13);
        pthread_mutex_unlock(&(rb_state0.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (0x2f57e9e1);
    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_2) == (0xbe93cfde)) ? (rb_state1.var_1) : (0xd9cbe472));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x6df0bfa8)) ? (rb_state2.var_1) : (0x1e54f329));
    if ((rb_state2.var_0) == (0x644103be)) {
        pthread_mutex_lock(&(rb_state2.lock_27));
        rb_state2.var_4 = 0xfb9a8587;
        rb_state2.var_5 = 0x430f7e94;
        rb_state2.var_6 = 0x177ab7c;
        rb_state2.var_7 = (rb_state2.var_7) - (((0x7c27bb93) + (rb_state2.var_4)) ^ (0x621eae56));
        rb_state2.var_8 = (rb_state2.var_8) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_5) : (0xefd082b5));
        rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0x9ad7fafc));
        rb_state2.var_10 = (rb_state2.var_10) ^ (((0x9bd7a7d6) - (rb_state2.var_8)) ^ (rb_state2.var_3));
        rb_state2.var_11 = (rb_state2.var_11) + (((((0xdf53d71e) + (0xe93f4135)) - (rb_state2.var_9)) - (rb_state2.var_4)) + (rb_state2.var_10));
        rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_11);
        rb_state2.var_2 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_27));
    }
    if ((rb_state2.var_0) == (0x644103be)) {
        pthread_mutex_lock(&(rb_state2.lock_27));
        rb_state2.var_28 = !((rb_state2.var_2) == (rb_state2.var_3));
        pthread_mutex_unlock(&(rb_state2.lock_27));
    }
    rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_17) : (0x50326bb8));
    rb_state2.var_19 = (rb_state2.var_19) ^ (0xa1d66203);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_1);
    if ((rb_state9.var_2) == (0xb30d4996)) {
        rb_state9.var_5 = 0x5c980a9b;
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0x98cdb993) - (rb_state9.var_4)) ^ (0xb56d0f3)) - (rb_state9.var_6)) - (rb_state9.var_5));
        rb_state9.var_4 = (rb_state9.var_4) + (rb_state9.var_6);
        rb_state9.var_3 = rb_state9.var_4;
    }
    if ((rb_state9.var_2) == (0x31393593)) {
        pthread_mutex_lock(&(rb_state9.lock_18));
        if ((rb_state9.var_3) != (0x0)) {
            if (!((rb_state9.var_3) == (rb_state9.var_7))) {
                racebench_trigger(9);
            }
        }
        pthread_mutex_unlock(&(rb_state9.lock_18));
    }
    if ((rb_state9.var_2) == (0x31393593)) {
        pthread_mutex_lock(&(rb_state9.lock_18));
        rb_state9.var_8 = 0x1e665976;
        rb_state9.var_9 = 0xb4eca6a9;
        rb_state9.var_10 = (rb_state9.var_10) ^ (((0x2273a5d1) ^ (rb_state9.var_9)) + (rb_state9.var_8));
        rb_state9.var_11 = (rb_state9.var_11) + (1318 < rb_input_size ? (uint32_t)rb_input[1318] : 0x72dc5c0d);
        rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0x2181de55));
        rb_state9.var_13 = (rb_state9.var_13) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xa5297468));
        rb_state9.var_14 = (rb_state9.var_14) - (rb_state9.var_12);
        rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_13);
        rb_state9.var_16 = (rb_state9.var_16) - (rb_state9.var_14);
        rb_state9.var_17 = (rb_state9.var_17) - (((((0xb9f5e307) - (rb_state9.var_15)) - (rb_state9.var_16)) ^ (0x79da8fae)) - (rb_state9.var_9));
        rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_17);
        rb_state9.var_3 = rb_state9.var_7;
        pthread_mutex_unlock(&(rb_state9.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_19));
        rb_state12.var_4 = 0xd23a1cf7;
        rb_state12.var_5 = 0x5e49ad3f;
        rb_state12.var_6 = 0xf2850e7e;
        rb_state12.var_7 = (rb_state12.var_7) - (rb_state12.var_3);
        rb_state12.var_8 = (rb_state12.var_8) + (((0xf70f2988) - (rb_state12.var_4)) + (rb_state12.var_5));
        rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_4);
        rb_state12.var_10 = (rb_state12.var_10) + (((0xa03292da) ^ (rb_state12.var_5)) + (rb_state12.var_6));
        rb_state12.var_11 = (rb_state12.var_11) ^ (((rb_state12.var_6) == (0xf2850e7e)) ? (rb_state12.var_7) : (0x13c698a5));
        rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_7) == (0x5e)) ? (rb_state12.var_8) : (0xb953fcfa));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((((0xece8ef67) ^ (rb_state12.var_9)) - (rb_state12.var_9)) + (rb_state12.var_8)) + (rb_state12.var_10));
        rb_state12.var_14 = (rb_state12.var_14) ^ (rb_state12.var_11);
        rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_10) == (0xf1004e63)) ? (rb_state12.var_12) : (0x13e6b3a1));
        rb_state12.var_16 = (rb_state12.var_16) - (((rb_state12.var_13) == (0xe0b7decc)) ? (rb_state12.var_14) : (0x4dd3189e));
        rb_state12.var_17 = (rb_state12.var_17) - (((rb_state12.var_11) == (0x5e)) ? (rb_state12.var_15) : (0x9fe84e1b));
        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_16) == (0xffffffa2)) ? (rb_state12.var_17) : (0xb2ae9a46));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_18);
        pthread_mutex_unlock(&(rb_state12.lock_19));
    }
    rb_state12.var_12 = (rb_state12.var_12) + ((0x9d2aa401) ^ (rb_state12.var_13));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (746 < rb_input_size ? (uint32_t)rb_input[746] : 0xbf7e6ccc);
    rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_1) == (0xb706fe)) ? (rb_state13.var_0) : (0x72bdb581));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (0x80e13863);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ ((0xe442d4e) + (rb_state18.var_1));
    if ((rb_state18.var_1) == (0x12cc77d2)) {
        rb_state18.var_11 = 0x8850ebbb;
        rb_state18.var_12 = 0xf0e916cb;
        rb_state18.var_13 = (rb_state18.var_13) ^ (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_4) : (0x6165f611));
        rb_state18.var_14 = (rb_state18.var_14) ^ (1371 < rb_input_size ? (uint32_t)rb_input[1371] : 0xe56fb06a);
        rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_12) : (0x8bd4f257));
        rb_state18.var_16 = (rb_state18.var_16) + (((((0x2e7034d) - (0x5eb620cd)) ^ (rb_state18.var_11)) + (rb_state18.var_13)) ^ (0x8d8e2767));
        rb_state18.var_17 = (rb_state18.var_17) - (rb_state18.var_14);
        rb_state18.var_18 = (rb_state18.var_18) - (((((0xb13d50ec) + (0x3ab95c6e)) + (rb_state18.var_15)) + (rb_state18.var_16)) - (0x9254de00));
        rb_state18.var_19 = (rb_state18.var_19) + (((rb_state18.var_17) == (0xffffff9f)) ? (rb_state18.var_18) : (0x2ad6f7f8));
        rb_state18.var_10 = (rb_state18.var_10) - (rb_state18.var_19);
        rb_state18.var_2 = rb_state18.var_10;
    }
    if ((rb_state18.var_1) == (0x12cc77d2)) {
        pthread_mutex_lock(&(rb_state18.lock_32));
        rb_state18.var_21 = 0x5fd5e38e;
        rb_state18.var_22 = 0x327a4efd;
        rb_state18.var_23 = (rb_state18.var_23) - (((0xc7ed0ffd) - (rb_state18.var_7)) ^ (0x3739a353));
        rb_state18.var_24 = (rb_state18.var_24) - (2001 < rb_input_size ? (uint32_t)rb_input[2001] : 0x9d7ea198);
        rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_22) == (0x327a4efd)) ? (rb_state18.var_21) : (0x1293ae4e));
        rb_state18.var_26 = (rb_state18.var_26) + (((0x73e7e21d) + (rb_state18.var_23)) - (rb_state18.var_8));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((((0x8b262c61) + (rb_state18.var_9)) - (rb_state18.var_24)) - (0xa1544eda)) ^ (rb_state18.var_25));
        rb_state18.var_28 = (rb_state18.var_28) - (((rb_state18.var_10) == (0xaa6e6eb)) ? (rb_state18.var_26) : (0xc82eca70));
        rb_state18.var_29 = (rb_state18.var_29) ^ (((0x4b82e8c6) + (rb_state18.var_27)) ^ (rb_state18.var_11));
        rb_state18.var_30 = (rb_state18.var_30) + (((rb_state18.var_12) == (0xf0e916cb)) ? (rb_state18.var_28) : (0xb89f3793));
        rb_state18.var_31 = (rb_state18.var_31) ^ (((rb_state18.var_29) == (0x89d7cc97)) ? (rb_state18.var_30) : (0xbd22cd1a));
        rb_state18.var_20 = (rb_state18.var_20) - (rb_state18.var_31);
        rb_state18.var_2 = rb_state18.var_20;
        pthread_mutex_unlock(&(rb_state18.lock_32));
    }
    #endif
    int accepted_bad_moves = -1;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xd0a81604)) {
        rb_state1.var_3 = rb_state1.var_2;
    }
    if ((rb_state1.var_1) == (0xd0a81604)) {
        rb_state1.var_4 = 0xea067256;
        rb_state1.var_5 = 0x9e1f85b6;
        rb_state1.var_6 = (rb_state1.var_6) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_4) : (0xff4016dc));
        rb_state1.var_7 = (rb_state1.var_7) - (0x363a52e0);
        rb_state1.var_8 = (rb_state1.var_8) + (((((0xde72770c) - (rb_state1.var_5)) + (rb_state1.var_3)) + (0xa0931df)) ^ (rb_state1.var_5));
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_6) : (0xea828e09));
        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_7) : (0x7c533d89));
        rb_state1.var_11 = (rb_state1.var_11) - (((((0xadcb6de4) - (0xe19bd0b2)) ^ (rb_state1.var_9)) + (rb_state1.var_8)) - (rb_state1.var_8));
        rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x6513ea27));
        rb_state1.var_13 = (rb_state1.var_13) ^ (((((0x3240fe79) + (0x769c4c88)) + (rb_state1.var_10)) + (rb_state1.var_12)) + (rb_state1.var_11));
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x644103be)) {
        rb_state2.var_12 = 0x70538ec2;
        rb_state2.var_13 = 0x22512f5c;
        rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_6) == (0x177ab7c)) ? (rb_state2.var_5) : (0xc854182b));
        rb_state2.var_15 = (rb_state2.var_15) ^ (rb_state2.var_7);
        rb_state2.var_16 = (rb_state2.var_16) + (((0x8798680) ^ (rb_state2.var_13)) - (rb_state2.var_8));
        rb_state2.var_17 = (rb_state2.var_17) + (rb_state2.var_12);
        rb_state2.var_18 = (rb_state2.var_18) - (((0x200847b) - (rb_state2.var_9)) - (rb_state2.var_14));
        rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_10) == (0x58c82942)) ? (rb_state2.var_15) : (0x2b35a9b3));
        rb_state2.var_20 = (rb_state2.var_20) ^ (((0x83e4697e) + (rb_state2.var_11)) + (rb_state2.var_16));
        rb_state2.var_21 = (rb_state2.var_21) ^ (((0x43e0e9ae) ^ (rb_state2.var_17)) ^ (0xb4daf56c));
        rb_state2.var_22 = (rb_state2.var_22) - (((((0x9b691280) ^ (rb_state2.var_18)) - (rb_state2.var_19)) - (rb_state2.var_12)) + (0x621098f5));
        rb_state2.var_23 = (rb_state2.var_23) + (((0x20e865e8) ^ (0x75163d7e)) - (rb_state2.var_20));
        rb_state2.var_24 = (rb_state2.var_24) ^ (((((0xa70703fa) + (rb_state2.var_21)) - (0xd316cf4d)) - (rb_state2.var_22)) ^ (0x8eedd922));
        rb_state2.var_25 = (rb_state2.var_25) + (((0xa4841474) ^ (rb_state2.var_13)) ^ (rb_state2.var_23));
        rb_state2.var_26 = (rb_state2.var_26) ^ (((((0x8a6e5a77) ^ (rb_state2.var_25)) ^ (rb_state2.var_24)) - (rb_state2.var_14)) - (0x776e1f21));
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_26);
    }
    rb_state2.var_15 = (rb_state2.var_15) ^ (766 < rb_input_size ? (uint32_t)rb_input[766] : 0xd094604d);
    if ((rb_state2.var_20) == (0x5aff9448)) {
        rb_state2.var_33 = 0x761b3cc9;
        rb_state2.var_34 = 0x677e54d0;
        rb_state2.var_35 = (rb_state2.var_35) ^ (0xdd5975a0);
        rb_state2.var_36 = (rb_state2.var_36) - (((((0xc1672216) - (rb_state2.var_25)) - (rb_state2.var_26)) - (0x4cac38e)) + (rb_state2.var_34));
        rb_state2.var_37 = (rb_state2.var_37) - (((((0x573d7970) ^ (rb_state2.var_35)) - (0x7c18cf9e)) ^ (0xed54b94f)) + (rb_state2.var_33));
        rb_state2.var_38 = (rb_state2.var_38) - (((((0xc06f50fd) - (0xbff913c3)) ^ (0x4f8ea271)) + (rb_state2.var_36)) + (rb_state2.var_37));
        rb_state2.var_32 = (rb_state2.var_32) ^ (rb_state2.var_38);
        rb_state2.var_29 = rb_state2.var_32;
    }
    if ((rb_state2.var_20) == (0x5aff9448)) {
        pthread_mutex_lock(&(rb_state2.lock_54));
        rb_state2.var_40 = 0x92c99758;
        rb_state2.var_41 = 0x49a8600e;
        rb_state2.var_42 = (rb_state2.var_42) + ((0xf9d77498) ^ (rb_state2.var_29));
        rb_state2.var_43 = (rb_state2.var_43) - (((rb_state2.var_40) == (0x0)) ? (rb_state2.var_28) : (0x7f6c791a));
        rb_state2.var_44 = (rb_state2.var_44) + (rb_state2.var_41);
        rb_state2.var_45 = (rb_state2.var_45) ^ (((rb_state2.var_30) == (0x0)) ? (rb_state2.var_42) : (0x51bd4269));
        rb_state2.var_46 = (rb_state2.var_46) + (rb_state2.var_43);
        rb_state2.var_47 = (rb_state2.var_47) + (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_44) : (0xecb30114));
        rb_state2.var_48 = (rb_state2.var_48) ^ (rb_state2.var_45);
        rb_state2.var_49 = (rb_state2.var_49) + (rb_state2.var_46);
        rb_state2.var_50 = (rb_state2.var_50) - (((rb_state2.var_32) == (0x0)) ? (rb_state2.var_47) : (0x9f8efa0d));
        rb_state2.var_51 = (rb_state2.var_51) - (((0x6cfd3bdd) + (0x33f77868)) - (rb_state2.var_48));
        rb_state2.var_52 = (rb_state2.var_52) - (((rb_state2.var_49) == (0x0)) ? (rb_state2.var_50) : (0xdcd5777c));
        rb_state2.var_53 = (rb_state2.var_53) - (((rb_state2.var_51) == (0x0)) ? (rb_state2.var_52) : (0x81ed2eca));
        rb_state2.var_39 = (rb_state2.var_39) ^ (rb_state2.var_53);
        rb_state2.var_29 = rb_state2.var_39;
        pthread_mutex_unlock(&(rb_state2.lock_54));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_30 = (rb_state12.var_30) + ((0xf4d42845) + (rb_state12.var_31));
    if ((rb_state12.var_32) == (0x67cc58a8)) {
        rb_state12.var_47 = 0xa47dc7be;
        rb_state12.var_48 = (rb_state12.var_48) + (((0xde60a81a) - (0x34a91275)) + (rb_state12.var_35));
        rb_state12.var_49 = (rb_state12.var_49) ^ (((0xb7929e2e) - (rb_state12.var_36)) + (rb_state12.var_34));
        rb_state12.var_50 = (rb_state12.var_50) ^ (0xe8542e1e);
        rb_state12.var_51 = (rb_state12.var_51) + (((rb_state12.var_37) == (0x0)) ? (rb_state12.var_47) : (0xb42f3c6f));
        rb_state12.var_52 = (rb_state12.var_52) ^ (((rb_state12.var_48) == (0x0)) ? (rb_state12.var_49) : (0x1fc20ec9));
        rb_state12.var_53 = (rb_state12.var_53) + (((rb_state12.var_38) == (0x0)) ? (rb_state12.var_50) : (0x13b166e8));
        rb_state12.var_54 = (rb_state12.var_54) - (((rb_state12.var_51) == (0x0)) ? (rb_state12.var_52) : (0xd7429585));
        rb_state12.var_55 = (rb_state12.var_55) - (((((0xa54ba70a) + (rb_state12.var_53)) ^ (0xbd1c1a1b)) + (rb_state12.var_39)) ^ (rb_state12.var_54));
        rb_state12.var_33 = (rb_state12.var_33) + (rb_state12.var_55);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0xffffffb2)) {
        rb_state13.var_13 = 0x64274f23;
        rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_10);
        rb_state13.var_15 = (rb_state13.var_15) - (0xeea72c58);
        rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_12) == (0xa8ac3dc3)) ? (rb_state13.var_11) : (0x9fb507cc));
        rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_13) == (0x64274f23)) ? (rb_state13.var_13) : (0xaffee017));
        rb_state13.var_18 = (rb_state13.var_18) + (((((0x9fa9a56f) - (rb_state13.var_14)) + (rb_state13.var_15)) - (rb_state13.var_14)) ^ (rb_state13.var_15));
        rb_state13.var_19 = (rb_state13.var_19) - (((0xd6ab6e85) + (rb_state13.var_16)) - (rb_state13.var_16));
        rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_17) == (0x64274f23)) ? (rb_state13.var_17) : (0xcbcc503e));
        rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_18);
        rb_state13.var_22 = (rb_state13.var_22) ^ (((rb_state13.var_18) == (0x382f0a0f)) ? (rb_state13.var_19) : (0xaf0e98c7));
        rb_state13.var_23 = (rb_state13.var_23) ^ (((rb_state13.var_20) == (0x9bd8b13f)) ? (rb_state13.var_21) : (0xd7affcbd));
        rb_state13.var_24 = (rb_state13.var_24) - (rb_state13.var_22);
        rb_state13.var_25 = (rb_state13.var_25) + (((((0xc137065) - (rb_state13.var_24)) - (rb_state13.var_19)) ^ (rb_state13.var_23)) ^ (0xc5e324eb));
        rb_state13.var_3 = (rb_state13.var_3) ^ (rb_state13.var_25);
    }
    if ((rb_state13.var_24) == (0xd6ab6e85)) {
        rb_state13.var_39 = 0x449ffdb7;
        rb_state13.var_40 = (rb_state13.var_40) ^ (586 < rb_input_size ? (uint32_t)rb_input[586] : 0x2713498d);
        rb_state13.var_41 = (rb_state13.var_41) + (263 < rb_input_size ? (uint32_t)rb_input[263] : 0x866d018f);
        rb_state13.var_42 = (rb_state13.var_42) ^ (1437 < rb_input_size ? (uint32_t)rb_input[1437] : 0x90cdcf13);
        rb_state13.var_43 = (rb_state13.var_43) ^ (((rb_state13.var_33) == (0x0)) ? (rb_state13.var_32) : (0x4e980034));
        rb_state13.var_44 = (rb_state13.var_44) + (((0x7a795b6f) + (rb_state13.var_34)) ^ (rb_state13.var_31));
        rb_state13.var_45 = (rb_state13.var_45) - (rb_state13.var_39);
        rb_state13.var_46 = (rb_state13.var_46) + (((rb_state13.var_40) == (0x0)) ? (rb_state13.var_41) : (0x73f2f191));
        rb_state13.var_47 = (rb_state13.var_47) ^ (((rb_state13.var_35) == (0x0)) ? (rb_state13.var_42) : (0xdb1623bd));
        rb_state13.var_48 = (rb_state13.var_48) - (((0x4b373834) ^ (rb_state13.var_43)) + (0x2f6989));
        rb_state13.var_49 = (rb_state13.var_49) + (((rb_state13.var_36) == (0x0)) ? (rb_state13.var_44) : (0xf52c7cc0));
        rb_state13.var_50 = (rb_state13.var_50) + (((0xf3a2c53) - (rb_state13.var_37)) ^ (rb_state13.var_45));
        rb_state13.var_51 = (rb_state13.var_51) + (((rb_state13.var_38) == (0x0)) ? (rb_state13.var_46) : (0x83b223d9));
        rb_state13.var_52 = (rb_state13.var_52) - (((0x6a900861) ^ (rb_state13.var_47)) ^ (0x6c9006e9));
        rb_state13.var_53 = (rb_state13.var_53) ^ (((rb_state13.var_39) == (0x0)) ? (rb_state13.var_48) : (0x5fc004d1));
        rb_state13.var_54 = (rb_state13.var_54) + (((0x207389d5) + (rb_state13.var_40)) + (rb_state13.var_49));
        rb_state13.var_55 = (rb_state13.var_55) - (((0x39c9f731) ^ (0x74586f52)) - (rb_state13.var_50));
        rb_state13.var_56 = (rb_state13.var_56) ^ (((rb_state13.var_41) == (0x0)) ? (rb_state13.var_51) : (0x2ab8065f));
        rb_state13.var_57 = (rb_state13.var_57) + (((((0x436b8d45) ^ (rb_state13.var_52)) - (rb_state13.var_42)) ^ (0x5e28c3a3)) + (rb_state13.var_53));
        rb_state13.var_58 = (rb_state13.var_58) - (rb_state13.var_54);
        rb_state13.var_59 = (rb_state13.var_59) + (((rb_state13.var_43) == (0x0)) ? (rb_state13.var_55) : (0x59210010));
        rb_state13.var_60 = (rb_state13.var_60) + (((((0xd33a9806) ^ (rb_state13.var_56)) ^ (rb_state13.var_44)) ^ (0x8c98d80)) ^ (rb_state13.var_57));
        rb_state13.var_61 = (rb_state13.var_61) ^ (((rb_state13.var_45) == (0x0)) ? (rb_state13.var_58) : (0x8f488f02));
        rb_state13.var_62 = (rb_state13.var_62) - (((((0x801551cc) - (rb_state13.var_47)) ^ (rb_state13.var_46)) - (rb_state13.var_60)) ^ (rb_state13.var_59));
        rb_state13.var_63 = (rb_state13.var_63) ^ (((rb_state13.var_48) == (0x0)) ? (rb_state13.var_61) : (0x704b9cb7));
        rb_state13.var_64 = (rb_state13.var_64) ^ (rb_state13.var_62);
        rb_state13.var_65 = (rb_state13.var_65) ^ (rb_state13.var_63);
        rb_state13.var_66 = (rb_state13.var_66) ^ (rb_state13.var_64);
        rb_state13.var_67 = (rb_state13.var_67) + (((0x88c65072) - (rb_state13.var_65)) - (rb_state13.var_49));
        rb_state13.var_68 = (rb_state13.var_68) + (((((0x60de2618) - (rb_state13.var_67)) - (rb_state13.var_50)) + (0xcfbebd3f)) ^ (rb_state13.var_66));
        rb_state13.var_38 = (rb_state13.var_38) + (rb_state13.var_68);
        rb_state13.var_28 = rb_state13.var_38;
    }
    if ((rb_state13.var_24) == (0xd6ab6e85)) {
        pthread_mutex_lock(&(rb_state13.lock_90));
        rb_state13.var_70 = 0xdfe3715c;
        rb_state13.var_71 = 0xfaf7a149;
        rb_state13.var_72 = (rb_state13.var_72) - (((0xbeb098d4) + (rb_state13.var_70)) + (0xc999ec4));
        rb_state13.var_73 = (rb_state13.var_73) - (1693 < rb_input_size ? (uint32_t)rb_input[1693] : 0x695b418a);
        rb_state13.var_74 = (rb_state13.var_74) - (((0x1dd03370) ^ (rb_state13.var_51)) - (rb_state13.var_52));
        rb_state13.var_75 = (rb_state13.var_75) + (((((0xa8d21656) ^ (rb_state13.var_72)) ^ (0xdbbee358)) ^ (rb_state13.var_53)) - (rb_state13.var_71));
        rb_state13.var_76 = (rb_state13.var_76) - (((rb_state13.var_54) == (0x0)) ? (rb_state13.var_73) : (0x76b62c9b));
        rb_state13.var_77 = (rb_state13.var_77) ^ (((0x418d7b1a) + (0xee079f61)) - (rb_state13.var_74));
        rb_state13.var_78 = (rb_state13.var_78) + (((0x1a921d54) + (rb_state13.var_55)) ^ (rb_state13.var_75));
        rb_state13.var_79 = (rb_state13.var_79) ^ (((((0x9d990b59) - (rb_state13.var_76)) + (0x79fbfe40)) + (rb_state13.var_77)) + (0x27f2d453));
        rb_state13.var_80 = (rb_state13.var_80) - (rb_state13.var_78);
        rb_state13.var_81 = (rb_state13.var_81) + (((rb_state13.var_56) == (0x0)) ? (rb_state13.var_79) : (0x6927cb08));
        rb_state13.var_82 = (rb_state13.var_82) - (((0x4b0d37ad) ^ (rb_state13.var_80)) ^ (0x34f80753));
        rb_state13.var_83 = (rb_state13.var_83) + (((rb_state13.var_57) == (0x0)) ? (rb_state13.var_81) : (0x5c2b1b5c));
        rb_state13.var_84 = (rb_state13.var_84) + (((0xc07c4e26) - (rb_state13.var_82)) ^ (rb_state13.var_58));
        rb_state13.var_85 = (rb_state13.var_85) ^ (((rb_state13.var_59) == (0x0)) ? (rb_state13.var_83) : (0x7dcdac7));
        rb_state13.var_86 = (rb_state13.var_86) ^ (rb_state13.var_84);
        rb_state13.var_87 = (rb_state13.var_87) ^ (rb_state13.var_85);
        rb_state13.var_88 = (rb_state13.var_88) + (rb_state13.var_86);
        rb_state13.var_89 = (rb_state13.var_89) + (((((0x137b5905) ^ (rb_state13.var_61)) - (rb_state13.var_87)) - (rb_state13.var_88)) - (rb_state13.var_60));
        rb_state13.var_69 = (rb_state13.var_69) + (rb_state13.var_89);
        rb_state13.var_28 = rb_state13.var_69;
        pthread_mutex_unlock(&(rb_state13.lock_90));
    }
    #endif
    double T = _start_temp;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xd0a81604)) {
        if (!((rb_state1.var_2) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_20) == (0x5aff9448)) {
        pthread_mutex_lock(&(rb_state2.lock_54));
        if ((rb_state2.var_29) != (0x0)) {
            if (!((rb_state2.var_29) == (rb_state2.var_39))) {
                racebench_trigger(2);
            }
        }
        pthread_mutex_unlock(&(rb_state2.lock_54));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_16) == (0x6459ce66)) {
        rb_state12.var_39 = 0xf0f8aac3;
        rb_state12.var_40 = 0xd4140a01;
        rb_state12.var_41 = (rb_state12.var_41) ^ (((rb_state12.var_26) == (0xaf209462)) ? (rb_state12.var_40) : (0xfa4eb460));
        rb_state12.var_42 = (rb_state12.var_42) ^ (((rb_state12.var_39) == (0xf0f8aac3)) ? (rb_state12.var_25) : (0x3878ceb8));
        rb_state12.var_43 = (rb_state12.var_43) - (((((0x6ba2019a) - (rb_state12.var_27)) ^ (rb_state12.var_28)) - (rb_state12.var_41)) + (rb_state12.var_42));
        rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_43);
    }
    rb_state12.var_29 = (rb_state12.var_29) ^ (0x374923b);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_21 = (rb_state13.var_21) ^ (rb_state13.var_22);
    #endif
    Rng rng;

    long a_id;
    long b_id;
    netlist_elem *a = _netlist->get_random_element(&a_id, NO_MATCHING_ELEMENT, &rng);
    netlist_elem *b = _netlist->get_random_element(&b_id, NO_MATCHING_ELEMENT, &rng);

    int temp_steps_completed = 0;
    while (keep_going(temp_steps_completed, accepted_good_moves, accepted_bad_moves)) {
        T = T / 1.5;
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_2) == (0x18c24904)) {
            rb_state7.var_4 = 0x977bbeb5;
            rb_state7.var_5 = (rb_state7.var_5) ^ (((0xb209d86a) ^ (0x2b89e4b0)) + (rb_state7.var_2));
            rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_3) : (0x35f2d341));
            rb_state7.var_7 = (rb_state7.var_7) ^ (((0x8e5fb834) ^ (rb_state7.var_4)) - (rb_state7.var_5));
            rb_state7.var_8 = (rb_state7.var_8) - (((((0xe73d9caa) ^ (rb_state7.var_6)) ^ (rb_state7.var_5)) ^ (rb_state7.var_7)) + (0xd6d71992));
            rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_8);
        }
        #endif
        accepted_good_moves = 0;
        accepted_bad_moves = 0;

        for (int i = 0; i < _moves_per_thread_temp; i++) {

            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ ((0xe9eb83a7) + (0xa5e44fb4));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0x64f8fd19));
            #endif
            a = b;
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_1);
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_1) == (0x153)) {
                rb_state8.var_4 = 0x1f568fcf;
                rb_state8.var_5 = 0x2a05a132;
                rb_state8.var_6 = 0xa9d40cd1;
                rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_4) : (0x5068a5e6));
                rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_5);
                rb_state8.var_9 = (rb_state8.var_9) + (((((0xf8d2834c) - (rb_state8.var_3)) ^ (0x851a3145)) + (rb_state8.var_8)) ^ (rb_state8.var_7));
                rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_9);
            }
            #endif
            a_id = b_id;
            b = _netlist->get_random_element(&b_id, a_id, &rng);

            routing_cost_t delta_cost = calculate_delta_routing_cost(a, b);
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (1068 < rb_input_size ? (uint32_t)rb_input[1068] : 0xc86cbf53);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_2 = (rb_state11.var_2) + (0xa09fa118);
            #endif
            move_decision_t is_good_move = accept_move(delta_cost, T, &rng);

            if (is_good_move == move_decision_accepted_bad) {
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) + (445 < rb_input_size ? (uint32_t)rb_input[445] : 0xd03648ef);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) + (0xbc04495c);
                rb_state11.var_0 = (rb_state11.var_0) - (129 < rb_input_size ? (uint32_t)rb_input[129] : 0x2c6e65d0);
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_1 = (rb_state16.var_1) - (1180 < rb_input_size ? (uint32_t)rb_input[1180] : 0xb5da981f);
                #endif
                accepted_bad_moves++;
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_1) == (0xbf1adf78)) ? (rb_state5.var_0) : (0x43cec7ec));
                #endif
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
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0xfffd8598)) {
            rb_state6.var_4 = 0xb44db350;
            rb_state6.var_5 = 0x25b5416a;
            rb_state6.var_6 = (rb_state6.var_6) + (((((0xc554be17) ^ (rb_state6.var_5)) ^ (rb_state6.var_4)) + (rb_state6.var_3)) - (0x26c990ea));
            rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0x9bb78a4a));
            rb_state6.var_8 = (rb_state6.var_8) ^ (((0x7d89bfdd) ^ (rb_state6.var_4)) - (rb_state6.var_7));
            rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_6) : (0xe2f16671));
            rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_8) : (0xa34d4e01));
            rb_state6.var_11 = (rb_state6.var_11) - (rb_state6.var_9);
            rb_state6.var_12 = (rb_state6.var_12) + (((0x55c9c981) + (rb_state6.var_10)) + (0x366800bb));
            rb_state6.var_13 = (rb_state6.var_13) + (((0x121a87ce) + (rb_state6.var_11)) - (rb_state6.var_9));
            rb_state6.var_14 = (rb_state6.var_14) - (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0x571d328));
            rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_14);
        }
        #endif
        double random_value = rng->drand();
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_1) == (0x23b1d1fb)) ? (rb_state17.var_0) : (0xc5868a69));
        #endif
        double boltzman = exp(-delta_cost / T);
        if (boltzman > random_value) {
            return move_decision_accepted_bad;
        } else {
            return move_decision_rejected;
        }
    }
}

routing_cost_t annealer_thread::calculate_delta_routing_cost(netlist_elem *a, netlist_elem *b) {
    location_t *a_loc = a->present_loc.Get();
    location_t *b_loc = b->present_loc.Get();

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0xfe76bd14);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + ((0x4dc34f23) ^ (rb_state10.var_0));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x645d67a8)) {
        if ((rb_state11.var_3) != (0x0)) {
            if (!((rb_state11.var_3) == (rb_state11.var_16))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_1);
    #endif
    routing_cost_t delta_cost = a->swap_cost(a_loc, b_loc);
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

        rv = temp_steps_completed < _number_temp_steps;
    }

    return rv;
}