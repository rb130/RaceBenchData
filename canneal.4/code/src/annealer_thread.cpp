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
    if ((rb_state0.var_3) == (0xe630c1ee)) {
        rb_state0.var_5 = 0x29d656cc;
        rb_state0.var_6 = (rb_state0.var_6) ^ (609 < rb_input_size ? (uint32_t)rb_input[609] : 0xd75b21cc);
        rb_state0.var_7 = (rb_state0.var_7) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_6) : (0x56fc140a));
        rb_state0.var_8 = (rb_state0.var_8) + (0x2df606fd);
        rb_state0.var_9 = (rb_state0.var_9) ^ (((0xecc022a1) ^ (rb_state0.var_5)) + (0xee1cb04a));
        rb_state0.var_10 = (rb_state0.var_10) + (rb_state0.var_4);
        rb_state0.var_11 = (rb_state0.var_11) + (((rb_state0.var_8) == (0x2df606fd)) ? (rb_state0.var_6) : (0x66dda0e1));
        rb_state0.var_12 = (rb_state0.var_12) ^ (((((0x845f7616) - (rb_state0.var_9)) ^ (rb_state0.var_8)) ^ (rb_state0.var_7)) - (0x79ac6ccb));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((0x501e6c0b) + (rb_state0.var_9)) - (rb_state0.var_10));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_10);
        rb_state0.var_15 = (rb_state0.var_15) + (((((0x2af36e67) ^ (rb_state0.var_11)) + (rb_state0.var_11)) - (rb_state0.var_12)) ^ (rb_state0.var_12));
        rb_state0.var_16 = (rb_state0.var_16) - (((rb_state0.var_13) == (0xc7bd8e4d)) ? (rb_state0.var_13) : (0xfea4a22d));
        rb_state0.var_17 = (rb_state0.var_17) - (((0x7c79bd01) + (rb_state0.var_14)) - (rb_state0.var_14));
        rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_15) == (0x24e86997)) ? (rb_state0.var_16) : (0xcd6f7ed6));
        rb_state0.var_19 = (rb_state0.var_19) ^ (rb_state0.var_17);
        rb_state0.var_20 = (rb_state0.var_20) + (rb_state0.var_18);
        rb_state0.var_21 = (rb_state0.var_21) + (((rb_state0.var_15) == (0x24e86997)) ? (rb_state0.var_19) : (0xf54e8786));
        rb_state0.var_22 = (rb_state0.var_22) ^ (rb_state0.var_20);
        rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_16) == (0x384271b3)) ? (rb_state0.var_21) : (0xeac163db));
        rb_state0.var_24 = (rb_state0.var_24) - (((rb_state0.var_17) == (0x838642ff)) ? (rb_state0.var_22) : (0xb40bc4c));
        rb_state0.var_25 = (rb_state0.var_25) ^ (rb_state0.var_23);
        rb_state0.var_26 = (rb_state0.var_26) - (((((0x543a2c5a) + (rb_state0.var_25)) + (rb_state0.var_24)) ^ (0xcd5ea6ae)) + (rb_state0.var_18));
        rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_26);
    }
    rb_state0.var_19 = (rb_state0.var_19) - (0xe43987aa);
    rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_22) == (0x384271b3)) ? (rb_state0.var_21) : (0x2ef39f56));
    rb_state0.var_24 = (rb_state0.var_24) + (758 < rb_input_size ? (uint32_t)rb_input[758] : 0xa62195c2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) + (0x3ddb63d0);
    rb_state2.var_1 = (rb_state2.var_1) - (0xd8106074);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_2);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_0);
    rb_state5.var_3 = (rb_state5.var_3) + (0x1951e65d);
    rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_2);
    if ((rb_state5.var_3) == (0x32a3ccba)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_6 = 0x1292f0a7;
        rb_state5.var_7 = (rb_state5.var_7) + (1382 < rb_input_size ? (uint32_t)rb_input[1382] : 0x30f289e5);
        rb_state5.var_8 = (rb_state5.var_8) + (((((0xff3ff5b6) + (0xd2c5ea39)) ^ (rb_state5.var_5)) + (0x7b498e01)) + (rb_state5.var_4));
        rb_state5.var_9 = (rb_state5.var_9) - (0x7632ceb0);
        rb_state5.var_10 = (rb_state5.var_10) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0x5ee36ae1));
        rb_state5.var_11 = (rb_state5.var_11) + (((((0x42a63890) ^ (0x215f5427)) - (rb_state5.var_8)) + (rb_state5.var_9)) + (0xe3bd8df8));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_10) : (0x50b53856));
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_11) : (0x403d092));
        rb_state5.var_14 = (rb_state5.var_14) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_12) : (0x89c4a2cd));
        rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_13);
        rb_state5.var_16 = (rb_state5.var_16) ^ (((((0xf89bc371) - (rb_state5.var_9)) ^ (0x1bdcbb2e)) ^ (rb_state5.var_15)) - (rb_state5.var_14));
        rb_state5.var_5 = (rb_state5.var_5) + (rb_state5.var_16);
        rb_state5.var_4 = rb_state5.var_5;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    if ((rb_state5.var_3) == (0x32a3ccba)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        if (!((rb_state5.var_4) == (rb_state5.var_5))) {
            racebench_trigger(5);
        }
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) - ((0x91e1f0f0) - (rb_state6.var_3));
    if ((rb_state6.var_3) == (0x2d26970)) {
        rb_state6.var_4 = rb_state6.var_0;
    }
    if ((rb_state6.var_3) == (0x2d26970)) {
        pthread_mutex_lock(&(rb_state6.lock_12));
        if (!((rb_state6.var_0) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_12));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_3) == (0x748aa052)) {
        pthread_mutex_lock(&(rb_state7.lock_48));
        if ((rb_state7.var_4) != (0x0)) {
            if (!((rb_state7.var_4) == (rb_state7.var_30))) {
                racebench_trigger(7);
            }
        }
        pthread_mutex_unlock(&(rb_state7.lock_48));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_2) == (0xb77282e8)) ? (rb_state8.var_1) : (0x1c8b2e34));
    rb_state8.var_3 = (rb_state8.var_3) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0xff0126f8));
    if ((rb_state8.var_2) == (0xb77282e8)) {
        rb_state8.var_4 = rb_state8.var_3;
    }
    if ((rb_state8.var_2) == (0xb77282e8)) {
        if (!((rb_state8.var_3) == (rb_state8.var_4))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_2) == (0x76)) ? (rb_state9.var_3) : (0x215fddde));
    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ ((0xb2a6efd8) ^ (rb_state10.var_2));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ (0x6973c33);
    rb_state11.var_3 = (rb_state11.var_3) + (0x83b4fe90);
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_2) == (0xc356dc39)) ? (rb_state11.var_1) : (0x95657dc0));
    if ((rb_state11.var_3) == (0x5a6379b8)) {
        if ((rb_state11.var_4) != (0x0)) {
            if (!((rb_state11.var_4) == (rb_state11.var_27))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_3) == (0x396c18ed)) {
        pthread_mutex_lock(&(rb_state19.lock_24));
        rb_state19.var_6 = 0xb3bb1b43;
        rb_state19.var_7 = (rb_state19.var_7) - (rb_state19.var_4);
        rb_state19.var_8 = (rb_state19.var_8) + (rb_state19.var_6);
        rb_state19.var_9 = (rb_state19.var_9) - (((0xcb2543f1) ^ (0xac00a363)) + (rb_state19.var_5));
        rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_6) == (0xb3bb1b43)) ? (rb_state19.var_7) : (0xbe44292f));
        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0x8ed438c7));
        rb_state19.var_12 = (rb_state19.var_12) + (((rb_state19.var_8) == (0xb3bb1b43)) ? (rb_state19.var_9) : (0x1687b46));
        rb_state19.var_13 = (rb_state19.var_13) + (((0x8307ad54) - (rb_state19.var_10)) + (0xbb3b9bf5));
        rb_state19.var_14 = (rb_state19.var_14) - (((((0x1caa2f16) ^ (rb_state19.var_9)) - (rb_state19.var_11)) ^ (rb_state19.var_12)) + (rb_state19.var_10));
        rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_13) == (0x3e434949)) ? (rb_state19.var_14) : (0x2462a1be));
        rb_state19.var_5 = (rb_state19.var_5) ^ (rb_state19.var_15);
        rb_state19.var_4 = rb_state19.var_5;
        pthread_mutex_unlock(&(rb_state19.lock_24));
    }
    if ((rb_state19.var_3) == (0x396c18ed)) {
        pthread_mutex_lock(&(rb_state19.lock_24));
        if (!((rb_state19.var_4) == (rb_state19.var_5))) {
            racebench_trigger(19);
        }
        pthread_mutex_unlock(&(rb_state19.lock_24));
    }
    #endif
    int accepted_good_moves = 0;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_25) == (0x7c79bd01)) {
        pthread_mutex_lock(&(rb_state0.lock_44));
        rb_state0.var_31 = 0x702c0779;
        rb_state0.var_32 = 0x91dabf79;
        rb_state0.var_33 = 0x3d356e7e;
        rb_state0.var_34 = (rb_state0.var_34) + ((0xf2c0fe71) - (rb_state0.var_26));
        rb_state0.var_35 = (rb_state0.var_35) ^ (((((0x3a59bbef) ^ (rb_state0.var_31)) - (rb_state0.var_33)) - (0xbf8e8ef)) ^ (0x78a67c7b));
        rb_state0.var_36 = (rb_state0.var_36) - (((((0xc9eba75b) - (rb_state0.var_32)) + (rb_state0.var_34)) + (rb_state0.var_28)) + (0xe6a215f6));
        rb_state0.var_37 = (rb_state0.var_37) ^ (rb_state0.var_35);
        rb_state0.var_38 = (rb_state0.var_38) + (((rb_state0.var_36) == (0x0)) ? (rb_state0.var_37) : (0x91f5ef85));
        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_38);
        rb_state0.var_29 = rb_state0.var_30;
        pthread_mutex_unlock(&(rb_state0.lock_44));
    }
    if ((rb_state0.var_25) == (0x7c79bd01)) {
        if (!((rb_state0.var_29) == (rb_state0.var_30))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x63)) {
        rb_state2.var_4 = rb_state2.var_3;
    }
    if ((rb_state2.var_2) == (0xf7d1e79)) {
        if (!((rb_state2.var_3) == (rb_state2.var_4))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (0xefaa49df);
    rb_state3.var_3 = (rb_state3.var_3) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0x62548d3));
    if ((rb_state3.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_27));
        rb_state3.var_15 = 0x727a2c2e;
        rb_state3.var_16 = (rb_state3.var_16) + (((((0xc169f75f) - (rb_state3.var_10)) - (rb_state3.var_8)) - (rb_state3.var_7)) - (rb_state3.var_9));
        rb_state3.var_17 = (rb_state3.var_17) - (((0x8e69265b) ^ (rb_state3.var_11)) - (rb_state3.var_15));
        rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_12);
        rb_state3.var_19 = (rb_state3.var_19) + (((0xbdac9321) - (rb_state3.var_16)) ^ (0xb5791f3));
        rb_state3.var_20 = (rb_state3.var_20) - (((0xf1ecbd98) - (rb_state3.var_13)) - (rb_state3.var_17));
        rb_state3.var_21 = (rb_state3.var_21) ^ (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_18) : (0x771e0c4b));
        rb_state3.var_22 = (rb_state3.var_22) + (((rb_state3.var_19) == (0x0)) ? (rb_state3.var_20) : (0xca2702d));
        rb_state3.var_23 = (rb_state3.var_23) ^ (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_21) : (0xe10acdf7));
        rb_state3.var_24 = (rb_state3.var_24) ^ (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_22) : (0xfdd12d77));
        rb_state3.var_25 = (rb_state3.var_25) - (((0xda70c393) - (0x2d967a11)) + (rb_state3.var_23));
        rb_state3.var_26 = (rb_state3.var_26) - (((((0xa2691dd) + (rb_state3.var_25)) + (0xd3aa4a0)) + (0x2c252e59)) - (rb_state3.var_24));
        rb_state3.var_4 = (rb_state3.var_4) ^ (rb_state3.var_26);
        pthread_mutex_unlock(&(rb_state3.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x32a3ccba)) {
        rb_state5.var_17 = 0x9a9bf981;
        rb_state5.var_18 = 0x968548ae;
        rb_state5.var_19 = (rb_state5.var_19) - (((((0xb0968b97) ^ (rb_state5.var_17)) ^ (rb_state5.var_11)) ^ (0xa5631270)) - (rb_state5.var_18));
        rb_state5.var_20 = (rb_state5.var_20) - (1524 < rb_input_size ? (uint32_t)rb_input[1524] : 0xafb02b06);
        rb_state5.var_21 = (rb_state5.var_21) - (((rb_state5.var_10) == (0x5ee36ae1)) ? (rb_state5.var_19) : (0xaa1492cc));
        rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_12) == (0x50b53856)) ? (rb_state5.var_20) : (0x412dd0be));
        rb_state5.var_23 = (rb_state5.var_23) ^ (((((0x870cc65d) ^ (rb_state5.var_21)) ^ (rb_state5.var_13)) + (0x998a34ea)) ^ (rb_state5.var_22));
        rb_state5.var_4 = (rb_state5.var_4) ^ (rb_state5.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ ((0xf335242f) - (0x934b34f2));
    if ((rb_state6.var_3) == (0x2d26970)) {
        rb_state6.var_5 = 0xa25f4b;
        rb_state6.var_6 = 0x9a66473e;
        rb_state6.var_7 = 0x5771bec7;
        rb_state6.var_8 = (rb_state6.var_8) - (rb_state6.var_7);
        rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_5) : (0x39c930e));
        rb_state6.var_10 = (rb_state6.var_10) - (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_8) : (0x26f08829));
        rb_state6.var_11 = (rb_state6.var_11) ^ (((((0x630a476f) + (0x43b9ec74)) ^ (0x715693e0)) + (rb_state6.var_9)) ^ (rb_state6.var_10));
        rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0xb2a6efd8)) {
        rb_state10.var_11 = 0x7d2c7af;
        rb_state10.var_12 = 0x36fdd292;
        rb_state10.var_13 = (rb_state10.var_13) + (((0x9c463398) ^ (rb_state10.var_11)) ^ (0xef691b7));
        rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_7) == (0x62db5c6a)) ? (rb_state10.var_12) : (0xd6853f72));
        rb_state10.var_15 = (rb_state10.var_15) ^ (((rb_state10.var_13) == (0x95626580)) ? (rb_state10.var_14) : (0xd7d84364));
        rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_3) == (0x5a6379b8)) {
        rb_state11.var_21 = 0x68946117;
        rb_state11.var_22 = 0xc3b695bc;
        rb_state11.var_23 = (rb_state11.var_23) - (((0xcf9c3110) + (rb_state11.var_11)) - (rb_state11.var_12));
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_22) : (0x1a0af877));
        rb_state11.var_25 = (rb_state11.var_25) ^ (((rb_state11.var_21) == (0x68946117)) ? (rb_state11.var_23) : (0xdbfacf85));
        rb_state11.var_26 = (rb_state11.var_26) + (((rb_state11.var_24) == (0xc3b695bc)) ? (rb_state11.var_25) : (0xcee4a1f0));
        rb_state11.var_20 = (rb_state11.var_20) - (rb_state11.var_26);
        rb_state11.var_4 = rb_state11.var_20;
    }
    if ((rb_state11.var_3) == (0x5a6379b8)) {
        pthread_mutex_lock(&(rb_state11.lock_47));
        rb_state11.var_28 = 0xa79a4c7d;
        rb_state11.var_29 = 0x48a396d4;
        rb_state11.var_30 = (rb_state11.var_30) - (932 < rb_input_size ? (uint32_t)rb_input[932] : 0xa3eee024);
        rb_state11.var_31 = (rb_state11.var_31) ^ (rb_state11.var_15);
        rb_state11.var_32 = (rb_state11.var_32) - (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_14) : (0xe418e33));
        rb_state11.var_33 = (rb_state11.var_33) ^ (((0x834dd61b) ^ (rb_state11.var_17)) + (rb_state11.var_29));
        rb_state11.var_34 = (rb_state11.var_34) - (((((0x9174867) ^ (rb_state11.var_30)) ^ (rb_state11.var_28)) - (rb_state11.var_18)) - (0xc7cdb5fd));
        rb_state11.var_35 = (rb_state11.var_35) - (((0x36743a1e) - (rb_state11.var_19)) + (rb_state11.var_31));
        rb_state11.var_36 = (rb_state11.var_36) - (((0x2de100e4) - (rb_state11.var_20)) + (rb_state11.var_32));
        rb_state11.var_37 = (rb_state11.var_37) - (((0x876d57b6) - (0xe0e9dc)) + (rb_state11.var_33));
        rb_state11.var_38 = (rb_state11.var_38) + (((rb_state11.var_21) == (0x68946117)) ? (rb_state11.var_34) : (0x460d24c));
        rb_state11.var_39 = (rb_state11.var_39) - (((((0x40383b10) + (0x6aa9fc29)) - (rb_state11.var_35)) + (0x36a3bf53)) - (rb_state11.var_36));
        rb_state11.var_40 = (rb_state11.var_40) - (((rb_state11.var_37) == (0xad822537)) ? (rb_state11.var_38) : (0x4afe26de));
        rb_state11.var_41 = (rb_state11.var_41) + (rb_state11.var_39);
        rb_state11.var_42 = (rb_state11.var_42) + (((0xb1fcb1fe) ^ (rb_state11.var_40)) - (rb_state11.var_22));
        rb_state11.var_43 = (rb_state11.var_43) - (rb_state11.var_41);
        rb_state11.var_44 = (rb_state11.var_44) ^ (((0xb6908957) ^ (rb_state11.var_42)) - (0x891237ff));
        rb_state11.var_45 = (rb_state11.var_45) - (((rb_state11.var_23) == (0x3063cef0)) ? (rb_state11.var_43) : (0xd422c2d1));
        rb_state11.var_46 = (rb_state11.var_46) - (((((0x8f2d650f) - (rb_state11.var_25)) ^ (rb_state11.var_45)) ^ (rb_state11.var_24)) + (rb_state11.var_44));
        rb_state11.var_27 = (rb_state11.var_27) ^ (rb_state11.var_46);
        rb_state11.var_4 = rb_state11.var_27;
        pthread_mutex_unlock(&(rb_state11.lock_47));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_3);
    if ((rb_state13.var_2) == (0xa9fad4a)) {
        if ((rb_state13.var_4) != (0x0)) {
            if (!((rb_state13.var_4) == (rb_state13.var_33))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    int accepted_bad_moves = -1;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_23 = (rb_state0.var_23) - ((0xf25a58fb) ^ (0xa2204487));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (0xf57bf9ce);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_27));
        rb_state3.var_6 = 0x1668525e;
        rb_state3.var_7 = 0x50a15a08;
        rb_state3.var_8 = (rb_state3.var_8) - (1160 < rb_input_size ? (uint32_t)rb_input[1160] : 0xc3205b9e);
        rb_state3.var_9 = (rb_state3.var_9) ^ (((rb_state3.var_7) == (0x50a15a08)) ? (rb_state3.var_4) : (0x6ec6adc3));
        rb_state3.var_10 = (rb_state3.var_10) ^ (((rb_state3.var_6) == (0x1668525e)) ? (rb_state3.var_8) : (0xb7dfb106));
        rb_state3.var_11 = (rb_state3.var_11) - (rb_state3.var_9);
        rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_10) : (0x28a7f5db));
        rb_state3.var_13 = (rb_state3.var_13) + (((rb_state3.var_6) == (0x1668525e)) ? (rb_state3.var_11) : (0x19ecc554));
        rb_state3.var_14 = (rb_state3.var_14) - (((rb_state3.var_12) == (0xfffffff7)) ? (rb_state3.var_13) : (0x874b2626));
        rb_state3.var_5 = (rb_state3.var_5) + (rb_state3.var_14);
        rb_state3.var_4 = rb_state3.var_5;
        pthread_mutex_unlock(&(rb_state3.lock_27));
    }
    if ((rb_state3.var_3) == (0x0)) {
        if (!((rb_state3.var_4) == (rb_state3.var_5))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (528 < rb_input_size ? (uint32_t)rb_input[528] : 0x2247156b);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x76)) {
        rb_state9.var_5 = 0x1920b873;
        rb_state9.var_6 = 0xd0f929b9;
        rb_state9.var_7 = 0xd0812bc9;
        rb_state9.var_8 = (rb_state9.var_8) ^ (((((0x7e1ec039) ^ (rb_state9.var_6)) - (rb_state9.var_4)) - (rb_state9.var_7)) + (0x51d9976e));
        rb_state9.var_9 = (rb_state9.var_9) ^ (((((0x68e870bd) - (0xa527626f)) + (rb_state9.var_5)) ^ (rb_state9.var_8)) - (rb_state9.var_5));
        rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_9);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_1);
    #endif
    double T = _start_temp;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_25) == (0x7c79bd01)) {
        rb_state0.var_39 = 0x1392028d;
        rb_state0.var_40 = 0xec25034e;
        rb_state0.var_41 = (rb_state0.var_41) ^ (((rb_state0.var_39) == (0x1392028d)) ? (rb_state0.var_29) : (0xbb5e3006));
        rb_state0.var_42 = (rb_state0.var_42) + (rb_state0.var_40);
        rb_state0.var_43 = (rb_state0.var_43) - (((rb_state0.var_41) == (0x6e0a107b)) ? (rb_state0.var_42) : (0xe47c83b));
        rb_state0.var_29 = (rb_state0.var_29) - (rb_state0.var_43);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x90667075)) {
        if ((rb_state1.var_4) != (0x0)) {
            if (!((rb_state1.var_4) == (rb_state1.var_26))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ (452 < rb_input_size ? (uint32_t)rb_input[452] : 0x71e9e44d);
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_2) == (0x63)) ? (rb_state2.var_1) : (0xf082e187));
    if ((rb_state2.var_2) == (0xf7d1e79)) {
        rb_state2.var_5 = 0x3654f2f1;
        rb_state2.var_6 = 0x8994086a;
        rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_5) : (0x14e6801b));
        rb_state2.var_8 = (rb_state2.var_8) ^ (0x17e81854);
        rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0xa60c28b2));
        rb_state2.var_10 = (rb_state2.var_10) ^ (rb_state2.var_8);
        rb_state2.var_11 = (rb_state2.var_11) ^ (((0xf104f6a) ^ (rb_state2.var_9)) ^ (0xe96d0db8));
        rb_state2.var_12 = (rb_state2.var_12) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_10) : (0x38f7c8d5));
        rb_state2.var_13 = (rb_state2.var_13) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_12) : (0x73ed2c47));
        rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_33));
        if ((rb_state12.var_3) != (0x0)) {
            if (!((rb_state12.var_3) == (rb_state12.var_20))) {
                racebench_trigger(12);
            }
        }
        pthread_mutex_unlock(&(rb_state12.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0xd1a7684e));
    #endif
    Rng rng;

    long a_id;
    long b_id;
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
            if ((rb_state16.var_0) == (0xe2204638)) {
                pthread_mutex_lock(&(rb_state16.lock_15));
                rb_state16.var_4 = rb_state16.var_1;
                pthread_mutex_unlock(&(rb_state16.lock_15));
            }
            if ((rb_state16.var_0) == (0xe2204638)) {
                if (!((rb_state16.var_1) == (rb_state16.var_4))) {
                    racebench_trigger(16);
                }
            }
            #endif
            a_id = b_id;
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0xe2204638)) {
                pthread_mutex_lock(&(rb_state16.lock_15));
                rb_state16.var_5 = 0xe04b52de;
                rb_state16.var_6 = (rb_state16.var_6) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_4) : (0x9671a9e));
                rb_state16.var_7 = (rb_state16.var_7) - (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_5) : (0x12739eda));
                rb_state16.var_8 = (rb_state16.var_8) ^ (rb_state16.var_8);
                rb_state16.var_9 = (rb_state16.var_9) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_6) : (0x702250c8));
                rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0x5aabfe8f));
                rb_state16.var_11 = (rb_state16.var_11) - (rb_state16.var_9);
                rb_state16.var_12 = (rb_state16.var_12) + (rb_state16.var_10);
                rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_11);
                rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0xb34b79bc));
                rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_14);
                pthread_mutex_unlock(&(rb_state16.lock_15));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_2);
            #endif
            b = _netlist->get_random_element(&b_id, a_id, &rng);

            #ifdef RACEBENCH_BUG_17
            rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_14) : (0xff48e775));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_18 = (rb_state18.var_18) ^ (2101 < rb_input_size ? (uint32_t)rb_input[2101] : 0x1c8d027d);
            #endif
            routing_cost_t delta_cost = calculate_delta_routing_cost(a, b);
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0xe3a26b75)) {
                pthread_mutex_lock(&(rb_state18.lock_33));
                rb_state18.var_5 = (rb_state18.var_5) - (((0xaa2911cd) ^ (0xdbbb47b0)) ^ (rb_state18.var_4));
                rb_state18.var_6 = (rb_state18.var_6) ^ ((0x382cf221) ^ (0xd0262be));
                rb_state18.var_7 = (rb_state18.var_7) - (0x1fce6d08);
                rb_state18.var_8 = (rb_state18.var_8) ^ (rb_state18.var_5);
                rb_state18.var_9 = (rb_state18.var_9) ^ (((0xfef76600) + (0xfe589bef)) + (rb_state18.var_3));
                rb_state18.var_10 = (rb_state18.var_10) ^ (rb_state18.var_5);
                rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_6) : (0x71817f08));
                rb_state18.var_12 = (rb_state18.var_12) - (((0x968b080a) ^ (rb_state18.var_7)) - (rb_state18.var_7));
                rb_state18.var_13 = (rb_state18.var_13) - (rb_state18.var_8);
                rb_state18.var_14 = (rb_state18.var_14) - (((0xd184a240) ^ (0x664c73c5)) + (rb_state18.var_9));
                rb_state18.var_15 = (rb_state18.var_15) - (((((0xc71701b3) ^ (rb_state18.var_10)) + (rb_state18.var_8)) - (0x2665ed5f)) - (rb_state18.var_11));
                rb_state18.var_16 = (rb_state18.var_16) + (rb_state18.var_12);
                rb_state18.var_17 = (rb_state18.var_17) - (rb_state18.var_13);
                rb_state18.var_18 = (rb_state18.var_18) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0xd2eaa9b8));
                rb_state18.var_19 = (rb_state18.var_19) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_16) : (0x92b10ca5));
                rb_state18.var_20 = (rb_state18.var_20) + (rb_state18.var_17);
                rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_18) : (0xa81fe01f));
                rb_state18.var_22 = (rb_state18.var_22) + (((0xadab1402) ^ (rb_state18.var_19)) - (0x8d6c8c54));
                rb_state18.var_23 = (rb_state18.var_23) - (((0xac504b3a) ^ (rb_state18.var_20)) + (rb_state18.var_11));
                rb_state18.var_24 = (rb_state18.var_24) ^ (((((0xa2134d53) + (rb_state18.var_21)) ^ (rb_state18.var_22)) - (rb_state18.var_12)) - (0x8c99670e));
                rb_state18.var_25 = (rb_state18.var_25) - (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_23) : (0x6cea4a7c));
                rb_state18.var_26 = (rb_state18.var_26) ^ (rb_state18.var_24);
                rb_state18.var_27 = (rb_state18.var_27) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_25) : (0xc0b2519c));
                rb_state18.var_28 = (rb_state18.var_28) ^ (rb_state18.var_26);
                rb_state18.var_29 = (rb_state18.var_29) + (rb_state18.var_27);
                rb_state18.var_30 = (rb_state18.var_30) + (((0x437c7d62) - (0x5a9e524c)) ^ (rb_state18.var_28));
                rb_state18.var_31 = (rb_state18.var_31) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_29) : (0x95b2009f));
                rb_state18.var_32 = (rb_state18.var_32) + (((((0x67958265) + (0x46c0fc6b)) + (rb_state18.var_31)) - (0x3e52e98f)) + (rb_state18.var_30));
                rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_32);
                rb_state18.var_4 = rb_state18.var_3;
                pthread_mutex_unlock(&(rb_state18.lock_33));
            }
            #endif
            move_decision_t is_good_move = accept_move(delta_cost, T, &rng);

            if (is_good_move == move_decision_accepted_bad) {
                accepted_bad_moves++;
                #ifdef RACEBENCH_BUG_17
                if ((rb_state17.var_2) == (0x2321387a)) {
                    rb_state17.var_5 = 0x1;
                }
                if ((rb_state17.var_2) == (0x2321387a)) {
                    pthread_mutex_lock(&(rb_state17.lock_23));
                    rb_state17.var_6 = 0x8ad459d9;
                    rb_state17.var_7 = (rb_state17.var_7) + (rb_state17.var_4);
                    rb_state17.var_8 = (rb_state17.var_8) + (((0x5bcde52e) ^ (0xad4ffc22)) + (rb_state17.var_6));
                    rb_state17.var_9 = (rb_state17.var_9) - (((0x59d110da) + (rb_state17.var_3)) + (0x849bc7f7));
                    rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_7) : (0x48a71aa6));
                    rb_state17.var_11 = (rb_state17.var_11) ^ (rb_state17.var_8);
                    rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_9);
                    rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0xb7b99f65));
                    rb_state17.var_14 = (rb_state17.var_14) + (rb_state17.var_12);
                    rb_state17.var_15 = (rb_state17.var_15) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_13) : (0xd65143ef));
                    rb_state17.var_16 = (rb_state17.var_16) + (((0x7b875760) - (0x45444b71)) ^ (rb_state17.var_14));
                    rb_state17.var_17 = (rb_state17.var_17) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_15) : (0x65e7a569));
                    rb_state17.var_18 = (rb_state17.var_18) - (((0xe4ce7186) + (rb_state17.var_16)) - (rb_state17.var_8));
                    rb_state17.var_19 = (rb_state17.var_19) ^ (((0xb96a6a15) + (rb_state17.var_17)) - (rb_state17.var_9));
                    rb_state17.var_20 = (rb_state17.var_20) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_18) : (0xdd211c04));
                    rb_state17.var_21 = (rb_state17.var_21) + (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_19) : (0x61144abd));
                    rb_state17.var_22 = (rb_state17.var_22) - (((((0xe8952eb2) - (0x6114f3dd)) - (rb_state17.var_21)) - (rb_state17.var_20)) + (rb_state17.var_12));
                    rb_state17.var_4 = (rb_state17.var_4) ^ (rb_state17.var_22);
                    rb_state17.var_3 = rb_state17.var_4;
                    pthread_mutex_unlock(&(rb_state17.lock_23));
                }
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
    location_t *a_loc = a->present_loc.Get();
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_18 = (rb_state17.var_18) ^ ((0xb11eb596) - (rb_state17.var_19));
    if ((rb_state17.var_21) == (0xc228957a)) {
        pthread_mutex_lock(&(rb_state17.lock_46));
        rb_state17.var_37 = 0xc4c4d571;
        rb_state17.var_38 = 0xefa4ad99;
        rb_state17.var_39 = (rb_state17.var_39) - (((((0x4545a28b) - (rb_state17.var_37)) - (rb_state17.var_29)) ^ (rb_state17.var_27)) - (rb_state17.var_28));
        rb_state17.var_40 = (rb_state17.var_40) + (((((0x6deba74b) + (rb_state17.var_30)) + (rb_state17.var_38)) ^ (rb_state17.var_39)) - (0x72b924b3));
        rb_state17.var_36 = (rb_state17.var_36) + (rb_state17.var_40);
        rb_state17.var_25 = rb_state17.var_36;
        pthread_mutex_unlock(&(rb_state17.lock_46));
    }
    if ((rb_state17.var_21) == (0xc228957a)) {
        pthread_mutex_lock(&(rb_state17.lock_46));
        rb_state17.var_42 = (rb_state17.var_42) ^ (0xc05fe57e);
        rb_state17.var_43 = (rb_state17.var_43) - (((((0x7366da1a) - (rb_state17.var_35)) + (rb_state17.var_32)) ^ (rb_state17.var_33)) + (rb_state17.var_34));
        rb_state17.var_44 = (rb_state17.var_44) + (((rb_state17.var_31) == (0x0)) ? (rb_state17.var_42) : (0x997367));
        rb_state17.var_45 = (rb_state17.var_45) - (((rb_state17.var_43) == (0x0)) ? (rb_state17.var_44) : (0x1ca5104e));
        rb_state17.var_41 = (rb_state17.var_41) ^ (rb_state17.var_45);
        rb_state17.var_25 = rb_state17.var_41;
        pthread_mutex_unlock(&(rb_state17.lock_46));
    }
    #endif
    location_t *b_loc = b->present_loc.Get();

    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0x8ebd74dd)) {
        pthread_mutex_lock(&(rb_state15.lock_25));
        rb_state15.var_6 = 0xb2801cfc;
        rb_state15.var_7 = 0x81143815;
        rb_state15.var_8 = (rb_state15.var_8) - (rb_state15.var_5);
        rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_6) : (0x23e313bf));
        rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_4) : (0x9b095e53));
        rb_state15.var_11 = (rb_state15.var_11) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_6) : (0xed10666c));
        rb_state15.var_12 = (rb_state15.var_12) + (rb_state15.var_8);
        rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_10) : (0x762f996));
        rb_state15.var_14 = (rb_state15.var_14) + (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_12) : (0xee19ed8c));
        rb_state15.var_15 = (rb_state15.var_15) + (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_14) : (0x92ac90a6));
        rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_15);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_25));
    }
    if ((rb_state15.var_3) == (0x8ebd74dd)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    routing_cost_t delta_cost = a->swap_cost(a_loc, b_loc);
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x39ccb5b4) + (0xef2e7251));
    if ((rb_state14.var_0) == (0x96c4e64d)) {
        rb_state14.var_11 = (rb_state14.var_11) ^ (((((0x1df9c210) ^ (rb_state14.var_10)) + (rb_state14.var_9)) ^ (rb_state14.var_6)) + (rb_state14.var_8));
        rb_state14.var_12 = (rb_state14.var_12) - (((((0x2dd19969) + (rb_state14.var_11)) - (0x49f7dac8)) - (rb_state14.var_11)) + (rb_state14.var_7));
        rb_state14.var_10 = (rb_state14.var_10) + (rb_state14.var_12);
        rb_state14.var_3 = rb_state14.var_10;
    }
    if ((rb_state14.var_0) == (0x96c4e64d)) {
        pthread_mutex_lock(&(rb_state14.lock_19));
        rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_3) : (0x99199d5e));
        rb_state14.var_15 = (rb_state14.var_15) + (0x312357fc);
        rb_state14.var_16 = (rb_state14.var_16) - (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_14) : (0xe0ed94fb));
        rb_state14.var_17 = (rb_state14.var_17) - (((0xcd9cd4ae) + (rb_state14.var_14)) - (rb_state14.var_15));
        rb_state14.var_18 = (rb_state14.var_18) ^ (((((0x1411d2d3) - (rb_state14.var_16)) + (0x577d9b26)) ^ (rb_state14.var_15)) - (rb_state14.var_17));
        rb_state14.var_13 = (rb_state14.var_13) + (rb_state14.var_18);
        rb_state14.var_3 = rb_state14.var_13;
        pthread_mutex_unlock(&(rb_state14.lock_19));
    }
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

        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
        #endif
        rv = temp_steps_completed < _number_temp_steps;
    }

    return rv;
}