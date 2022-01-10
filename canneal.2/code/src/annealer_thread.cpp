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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (0xf445eb2b);
    rb_state5.var_0 = (rb_state5.var_0) - (1631 < rb_input_size ? (uint32_t)rb_input[1631] : 0xa32571fc);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x138)) {
        pthread_mutex_lock(&(rb_state15.lock_27));
        rb_state15.var_8 = 0x8d288d15;
        rb_state15.var_9 = 0x8667c485;
        rb_state15.var_10 = (rb_state15.var_10) ^ (707 < rb_input_size ? (uint32_t)rb_input[707] : 0x370b5988);
        rb_state15.var_11 = (rb_state15.var_11) + (((0x64b501ea) - (rb_state15.var_4)) ^ (rb_state15.var_9));
        rb_state15.var_12 = (rb_state15.var_12) + (rb_state15.var_3);
        rb_state15.var_13 = (rb_state15.var_13) ^ (((rb_state15.var_8) == (0x8d288d15)) ? (rb_state15.var_10) : (0x271548ae));
        rb_state15.var_14 = (rb_state15.var_14) + (((0xe3ae802f) + (rb_state15.var_5)) ^ (rb_state15.var_11));
        rb_state15.var_15 = (rb_state15.var_15) + (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_13) : (0x4b389a7));
        rb_state15.var_16 = (rb_state15.var_16) + (((0xa6103425) + (0x7aaa1f40)) ^ (rb_state15.var_14));
        rb_state15.var_17 = (rb_state15.var_17) + (((rb_state15.var_15) == (0x67)) ? (rb_state15.var_16) : (0x1fc03aa0));
        rb_state15.var_7 = (rb_state15.var_7) - (rb_state15.var_17);
        rb_state15.var_2 = rb_state15.var_7;
        pthread_mutex_unlock(&(rb_state15.lock_27));
    }
    if ((rb_state15.var_1) == (0x138)) {
        pthread_mutex_lock(&(rb_state15.lock_27));
        rb_state15.var_19 = (rb_state15.var_19) ^ (rb_state15.var_6);
        rb_state15.var_20 = (rb_state15.var_20) + (((rb_state15.var_9) == (0x8667c485)) ? (rb_state15.var_7) : (0x63310169));
        rb_state15.var_21 = (rb_state15.var_21) ^ (0x8dd6996e);
        rb_state15.var_22 = (rb_state15.var_22) - (rb_state15.var_8);
        rb_state15.var_23 = (rb_state15.var_23) - (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_20) : (0x230636ea));
        rb_state15.var_24 = (rb_state15.var_24) - (((((0x9c6f505a) - (rb_state15.var_22)) - (0x3f106113)) + (rb_state15.var_21)) - (rb_state15.var_10));
        rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_23);
        rb_state15.var_26 = (rb_state15.var_26) + (((((0x49c86d85) ^ (rb_state15.var_24)) + (rb_state15.var_12)) - (rb_state15.var_25)) ^ (rb_state15.var_11));
        rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_26);
        rb_state15.var_2 = rb_state15.var_18;
        pthread_mutex_unlock(&(rb_state15.lock_27));
    }
    #endif
    int accepted_good_moves = 0;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0x8bf2d141) + (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (0x33c826be);
    if ((rb_state5.var_0) == (0x986fb42c)) {
        rb_state5.var_7 = 0xe560eddd;
        rb_state5.var_8 = (rb_state5.var_8) ^ (rb_state5.var_7);
        rb_state5.var_9 = (rb_state5.var_9) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_3) : (0x75ca7dcf));
        rb_state5.var_10 = (rb_state5.var_10) + (rb_state5.var_6);
        rb_state5.var_11 = (rb_state5.var_11) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_4) : (0xde982c30));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_8) : (0x9e8cb237));
        rb_state5.var_13 = (rb_state5.var_13) + (rb_state5.var_9);
        rb_state5.var_14 = (rb_state5.var_14) ^ (((0x40e041b0) + (rb_state5.var_10)) ^ (0x803743e5));
        rb_state5.var_15 = (rb_state5.var_15) + (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_11) : (0x92ec65d6));
        rb_state5.var_16 = (rb_state5.var_16) ^ (((((0x5356d374) + (rb_state5.var_10)) - (rb_state5.var_12)) ^ (0xa07245ac)) + (rb_state5.var_13));
        rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_15) : (0x8dbea490));
        rb_state5.var_18 = (rb_state5.var_18) - (((((0x8df59974) - (rb_state5.var_12)) - (rb_state5.var_17)) ^ (rb_state5.var_16)) ^ (rb_state5.var_11));
        rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_18);
        rb_state5.var_1 = rb_state5.var_6;
    }
    if ((rb_state5.var_0) == (0x986fb42c)) {
        pthread_mutex_lock(&(rb_state5.lock_29));
        rb_state5.var_20 = 0xa96f0124;
        rb_state5.var_21 = (rb_state5.var_21) - (859 < rb_input_size ? (uint32_t)rb_input[859] : 0xfba8140e);
        rb_state5.var_22 = (rb_state5.var_22) + (((((0x6dbffff9) - (0x2d3587b1)) + (0xc8e162b3)) + (rb_state5.var_13)) + (rb_state5.var_14));
        rb_state5.var_23 = (rb_state5.var_23) + (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_15) : (0x1d6caac9));
        rb_state5.var_24 = (rb_state5.var_24) ^ (((((0x1423783f) - (rb_state5.var_20)) + (0x513498e5)) + (rb_state5.var_21)) + (0x165f15b7));
        rb_state5.var_25 = (rb_state5.var_25) + (((0xa647a237) + (0x7e7d890b)) ^ (rb_state5.var_22));
        rb_state5.var_26 = (rb_state5.var_26) - (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_23) : (0xb1de8e5));
        rb_state5.var_27 = (rb_state5.var_27) ^ (((((0x69def93) ^ (rb_state5.var_19)) + (rb_state5.var_18)) + (rb_state5.var_25)) ^ (rb_state5.var_24));
        rb_state5.var_28 = (rb_state5.var_28) ^ (((((0x9e09ddf8) + (rb_state5.var_26)) + (0x9cdbf7b)) ^ (rb_state5.var_27)) + (0x2052c89a));
        rb_state5.var_19 = (rb_state5.var_19) - (rb_state5.var_28);
        rb_state5.var_1 = rb_state5.var_19;
        pthread_mutex_unlock(&(rb_state5.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
    if ((rb_state6.var_0) == (0x0)) {
        if ((rb_state6.var_3) != (0x0)) {
            rb_state6.var_11 = !((rb_state6.var_1) == (rb_state6.var_2));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0xbf1475e0);
    if ((rb_state9.var_1) == (0xfaa782e0)) {
        if ((rb_state9.var_4) != (0x0)) {
            rb_state9.var_14 = !((rb_state9.var_2) == (rb_state9.var_3));
        }
    }
    if ((rb_state9.var_12) == (0x3a88fe17)) {
        rb_state9.var_30 = 0xd93327bc;
        rb_state9.var_31 = 0xdc0e1b8c;
        rb_state9.var_32 = (rb_state9.var_32) - (((0x6e733fba) ^ (rb_state9.var_23)) ^ (0x22d349c8));
        rb_state9.var_33 = (rb_state9.var_33) ^ (((0xbecd21) - (rb_state9.var_31)) - (0xa50a3c73));
        rb_state9.var_34 = (rb_state9.var_34) ^ (((0xdb2f4c77) - (rb_state9.var_24)) ^ (rb_state9.var_30));
        rb_state9.var_35 = (rb_state9.var_35) ^ (((0x67f41ee1) ^ (rb_state9.var_25)) - (rb_state9.var_32));
        rb_state9.var_36 = (rb_state9.var_36) - (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0x99a8265e));
        rb_state9.var_37 = (rb_state9.var_37) + (rb_state9.var_35);
        rb_state9.var_38 = (rb_state9.var_38) ^ (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_36) : (0x664d6151));
        rb_state9.var_39 = (rb_state9.var_39) ^ (((0xd414de93) ^ (rb_state9.var_37)) + (0xa12ff7d6));
        rb_state9.var_40 = (rb_state9.var_40) - (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_38) : (0xe14b19a6));
        rb_state9.var_41 = (rb_state9.var_41) ^ (((rb_state9.var_28) == (0x0)) ? (rb_state9.var_39) : (0x264e9cf2));
        rb_state9.var_42 = (rb_state9.var_42) + (((rb_state9.var_40) == (0x0)) ? (rb_state9.var_41) : (0x8fd484f3));
        rb_state9.var_15 = (rb_state9.var_15) + (rb_state9.var_42);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
    #endif
    int accepted_bad_moves = -1;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_9);
    if ((rb_state6.var_11) == (0x0)) {
        rb_state6.var_54 = 0x7423eaa1;
        rb_state6.var_55 = (rb_state6.var_55) - (((((0x48dcb0fe) + (rb_state6.var_35)) - (rb_state6.var_36)) ^ (0x7f6a2762)) ^ (rb_state6.var_54));
        rb_state6.var_56 = (rb_state6.var_56) - (rb_state6.var_37);
        rb_state6.var_57 = (rb_state6.var_57) - (((rb_state6.var_34) == (0x0)) ? (rb_state6.var_55) : (0x871074c3));
        rb_state6.var_58 = (rb_state6.var_58) + (((rb_state6.var_38) == (0x0)) ? (rb_state6.var_56) : (0x9236879f));
        rb_state6.var_59 = (rb_state6.var_59) ^ (((0xdb2b9bd2) + (rb_state6.var_57)) + (rb_state6.var_39));
        rb_state6.var_60 = (rb_state6.var_60) + (((rb_state6.var_40) == (0x0)) ? (rb_state6.var_58) : (0x5f8116ab));
        rb_state6.var_61 = (rb_state6.var_61) + (((rb_state6.var_41) == (0x0)) ? (rb_state6.var_59) : (0x378d0fc));
        rb_state6.var_62 = (rb_state6.var_62) + (((0xb0e4f911) ^ (rb_state6.var_42)) - (rb_state6.var_60));
        rb_state6.var_63 = (rb_state6.var_63) ^ (((0x4d6563f5) ^ (rb_state6.var_61)) - (rb_state6.var_43));
        rb_state6.var_64 = (rb_state6.var_64) - (rb_state6.var_62);
        rb_state6.var_65 = (rb_state6.var_65) - (rb_state6.var_63);
        rb_state6.var_66 = (rb_state6.var_66) - (((((0x3979c842) + (rb_state6.var_44)) ^ (rb_state6.var_65)) + (rb_state6.var_64)) ^ (0xc15fa954));
        rb_state6.var_53 = (rb_state6.var_53) - (rb_state6.var_66);
        rb_state6.var_12 = rb_state6.var_53;
    }
    if ((rb_state6.var_11) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_72));
        rb_state6.var_68 = 0x514db861;
        rb_state6.var_69 = 0x3e267f5d;
        rb_state6.var_70 = (rb_state6.var_70) - (((((0x47683298) ^ (rb_state6.var_69)) - (0x1db9480a)) - (rb_state6.var_68)) + (rb_state6.var_46));
        rb_state6.var_71 = (rb_state6.var_71) + (((rb_state6.var_45) == (0x0)) ? (rb_state6.var_70) : (0xca09fb91));
        rb_state6.var_67 = (rb_state6.var_67) - (rb_state6.var_71);
        rb_state6.var_12 = rb_state6.var_67;
        pthread_mutex_unlock(&(rb_state6.lock_72));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x35026233) - (rb_state7.var_1));
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x35026233)) ? (rb_state7.var_1) : (0xefa6db05));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x0)) {
        rb_state11.var_2 = rb_state11.var_1;
    }
    if ((rb_state11.var_0) == (0x0)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0xf7bb4ee2) - (0xc93e0468));
    #endif
    double T = _start_temp;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0x61cbd35e);
    if ((rb_state3.var_1) == (0x3c685944)) {
        rb_state3.var_14 = (rb_state3.var_14) - (rb_state3.var_7);
        rb_state3.var_15 = (rb_state3.var_15) + (((0xd6cafa84) + (rb_state3.var_10)) + (rb_state3.var_9));
        rb_state3.var_16 = (rb_state3.var_16) - (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_14) : (0xe1e75ebd));
        rb_state3.var_17 = (rb_state3.var_17) + (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_15) : (0xb783e75f));
        rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_16) : (0x29a9b213));
        rb_state3.var_19 = (rb_state3.var_19) - (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_17) : (0x5ff40a60));
        rb_state3.var_20 = (rb_state3.var_20) ^ (rb_state3.var_18);
        rb_state3.var_21 = (rb_state3.var_21) ^ (((0x297808f5) ^ (0x83c8e70e)) ^ (rb_state3.var_19));
        rb_state3.var_22 = (rb_state3.var_22) ^ (rb_state3.var_20);
        rb_state3.var_23 = (rb_state3.var_23) - (((((0x4967b0a9) ^ (0x69ac2fa7)) ^ (rb_state3.var_21)) + (rb_state3.var_22)) ^ (0x29ea3a46));
        rb_state3.var_13 = (rb_state3.var_13) - (rb_state3.var_23);
        rb_state3.var_2 = rb_state3.var_13;
    }
    if ((rb_state3.var_1) == (0x3c685944)) {
        pthread_mutex_lock(&(rb_state3.lock_39));
        rb_state3.var_25 = 0x494a7f2c;
        rb_state3.var_26 = (rb_state3.var_26) + (rb_state3.var_16);
        rb_state3.var_27 = (rb_state3.var_27) - (0x898f1ab);
        rb_state3.var_28 = (rb_state3.var_28) - (((0xef837471) + (rb_state3.var_14)) ^ (rb_state3.var_17));
        rb_state3.var_29 = (rb_state3.var_29) ^ (((rb_state3.var_25) == (0x0)) ? (rb_state3.var_15) : (0x2af5750));
        rb_state3.var_30 = (rb_state3.var_30) + (((0x4b24bc4b) + (rb_state3.var_26)) + (rb_state3.var_18));
        rb_state3.var_31 = (rb_state3.var_31) - (((((0xe79a8ad6) + (0x200b71bf)) ^ (rb_state3.var_27)) + (rb_state3.var_19)) ^ (rb_state3.var_28));
        rb_state3.var_32 = (rb_state3.var_32) + (((rb_state3.var_29) == (0x0)) ? (rb_state3.var_30) : (0x698ff4da));
        rb_state3.var_33 = (rb_state3.var_33) - (((0x14f0dfc2) ^ (rb_state3.var_20)) + (rb_state3.var_31));
        rb_state3.var_34 = (rb_state3.var_34) ^ (((0x2c5bb58c) ^ (rb_state3.var_32)) + (0x10daefb6));
        rb_state3.var_35 = (rb_state3.var_35) ^ (rb_state3.var_33);
        rb_state3.var_36 = (rb_state3.var_36) + (rb_state3.var_34);
        rb_state3.var_37 = (rb_state3.var_37) - (((0x5ca9a603) + (rb_state3.var_35)) ^ (rb_state3.var_21));
        rb_state3.var_38 = (rb_state3.var_38) - (((rb_state3.var_36) == (0x0)) ? (rb_state3.var_37) : (0xd409bd2f));
        rb_state3.var_24 = (rb_state3.var_24) - (rb_state3.var_38);
        rb_state3.var_2 = rb_state3.var_24;
        pthread_mutex_unlock(&(rb_state3.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x986fb42c)) {
        if ((rb_state5.var_1) != (0x0)) {
            if (!((rb_state5.var_1) == (rb_state5.var_19))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_10));
        rb_state6.var_3 = 0x1;
        pthread_mutex_unlock(&(rb_state6.lock_10));
    }
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_10));
        rb_state6.var_4 = (rb_state6.var_4) ^ (0xdb03bac4);
        rb_state6.var_5 = (rb_state6.var_5) + (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_3) : (0x23583cbb));
        rb_state6.var_6 = (rb_state6.var_6) ^ (rb_state6.var_1);
        rb_state6.var_7 = (rb_state6.var_7) + (((((0x8ff9b1eb) + (rb_state6.var_2)) - (0xe30cdc6e)) - (rb_state6.var_4)) + (rb_state6.var_5));
        rb_state6.var_8 = (rb_state6.var_8) ^ (((((0xdb977c3) - (rb_state6.var_6)) + (rb_state6.var_5)) + (0x379e0038)) ^ (rb_state6.var_6));
        rb_state6.var_9 = (rb_state6.var_9) ^ (((((0xfc2e65a7) ^ (rb_state6.var_8)) - (0xfade17f9)) - (rb_state6.var_7)) + (rb_state6.var_7));
        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_9);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_10));
    }
    rb_state6.var_4 = (rb_state6.var_4) - (6 < rb_input_size ? (uint32_t)rb_input[6] : 0x3c41650);
    if ((rb_state6.var_11) == (0x0)) {
        if ((rb_state6.var_12) != (0x0)) {
            if (!((rb_state6.var_12) == (rb_state6.var_67))) {
                racebench_trigger(6);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x0)) {
        rb_state7.var_2 = rb_state7.var_0;
    }
    if ((rb_state7.var_1) == (0x0)) {
        if (!((rb_state7.var_0) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_7 = (rb_state9.var_7) ^ (((rb_state9.var_9) == (0x39883175)) ? (rb_state9.var_8) : (0xd0ecf3b1));
    if ((rb_state9.var_12) == (0x3a88fe17)) {
        pthread_mutex_lock(&(rb_state9.lock_43));
        rb_state9.var_17 = 0xd4ebe567;
        rb_state9.var_18 = (rb_state9.var_18) - (((0x10007cc4) - (rb_state9.var_15)) - (rb_state9.var_16));
        rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_17) : (0xcd174f5b));
        rb_state9.var_20 = (rb_state9.var_20) - (931 < rb_input_size ? (uint32_t)rb_input[931] : 0xb83b1e3);
        rb_state9.var_21 = (rb_state9.var_21) ^ (rb_state9.var_14);
        rb_state9.var_22 = (rb_state9.var_22) - (((0x58d41697) ^ (rb_state9.var_19)) + (rb_state9.var_17));
        rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0xac9c80e3));
        rb_state9.var_24 = (rb_state9.var_24) + (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_21) : (0x8a8e080c));
        rb_state9.var_25 = (rb_state9.var_25) + (((0x359a8a5) ^ (0xc7baf974)) + (rb_state9.var_22));
        rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_23) : (0x76b8a5b5));
        rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0xc9430efb));
        rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_26) : (0xdc3141f2));
        rb_state9.var_29 = (rb_state9.var_29) - (((((0xcbb5723f) - (rb_state9.var_28)) + (rb_state9.var_27)) + (rb_state9.var_22)) + (0x66e8e24f));
        rb_state9.var_16 = (rb_state9.var_16) + (rb_state9.var_29);
        rb_state9.var_15 = rb_state9.var_16;
        pthread_mutex_unlock(&(rb_state9.lock_43));
    }
    if ((rb_state9.var_12) == (0x3a88fe17)) {
        if (!((rb_state9.var_15) == (rb_state9.var_16))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
    if ((rb_state14.var_0) == (0x74882092)) {
        rb_state14.var_2 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0x74882092)) {
        pthread_mutex_lock(&(rb_state14.lock_9));
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
        pthread_mutex_unlock(&(rb_state14.lock_9));
    }
    #endif
    Rng rng;

    long a_id;
    long b_id;
    netlist_elem *a = _netlist->get_random_element(&a_id, NO_MATCHING_ELEMENT, &rng);
    netlist_elem *b = _netlist->get_random_element(&b_id, NO_MATCHING_ELEMENT, &rng);

    int temp_steps_completed = 0;
    while (keep_going(temp_steps_completed, accepted_good_moves, accepted_bad_moves)) {
        T = T / 1.5;
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
        #endif
        accepted_good_moves = 0;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
        #endif
        accepted_bad_moves = 0;

        for (int i = 0; i < _moves_per_thread_temp; i++) {

            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (2124 < rb_input_size ? (uint32_t)rb_input[2124] : 0x24e84817);
            #endif
            a = b;
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) - (0x8f7f9912);
            #endif
            a_id = b_id;
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_1) == (0x99c15572)) {
                rb_state4.var_9 = 0x845ba760;
                rb_state4.var_10 = 0x2e679c90;
                rb_state4.var_11 = (rb_state4.var_11) + (((((0x2b7c27c2) - (rb_state4.var_9)) ^ (rb_state4.var_10)) ^ (0xa3180f42)) ^ (rb_state4.var_9));
                rb_state4.var_12 = (rb_state4.var_12) + (((((0x2cc8663d) ^ (rb_state4.var_11)) ^ (0x70289096)) ^ (rb_state4.var_8)) - (0x6ef19fc5));
                rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_12);
            }
            #endif
            b = _netlist->get_random_element(&b_id, a_id, &rng);

            routing_cost_t delta_cost = calculate_delta_routing_cost(a, b);
            move_decision_t is_good_move = accept_move(delta_cost, T, &rng);

            if (is_good_move == move_decision_accepted_bad) {
                accepted_bad_moves++;
                _netlist->swap_locations(a, b);
            } else if (is_good_move == move_decision_accepted_good) {
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_1);
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
        double random_value = rng->drand();
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_1) == (0xffffffaf)) {
            if ((rb_state13.var_2) != (0x0)) {
                if (!((rb_state13.var_2) == (rb_state13.var_14))) {
                    racebench_trigger(13);
                }
            }
        }
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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (0x1f44f138);
    #endif
    location_t *a_loc = a->present_loc.Get();
    location_t *b_loc = b->present_loc.Get();

    routing_cost_t delta_cost = a->swap_cost(a_loc, b_loc);
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_0);
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

        rv = temp_steps_completed < _number_temp_steps;
    }

    return rv;
}