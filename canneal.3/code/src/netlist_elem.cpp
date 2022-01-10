#include "racebench_bugs.h"


#include <assert.h>
#include <math.h>

#include "annealer_types.h"
#include "location_t.h"
#include "netlist_elem.h"

netlist_elem::netlist_elem() : present_loc(NULL) {
}

routing_cost_t netlist_elem::routing_cost_given_loc(location_t loc) {
    routing_cost_t fanin_cost = 0;
    routing_cost_t fanout_cost = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        fanin_cost += fabs(loc.x - fanin_loc->x);
        fanin_cost += fabs(loc.y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        fanout_cost += fabs(loc.x - fanout_loc->x);
        fanout_cost += fabs(loc.y - fanout_loc->y);
    }

    routing_cost_t total_cost = fanin_cost + fanout_cost;
    return total_cost;
}

routing_cost_t netlist_elem::swap_cost(location_t *old_loc, location_t *new_loc) {
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (1745 < rb_input_size ? (uint32_t)rb_input[1745] : 0x39846a27);
    if ((rb_state6.var_2) == (0xfffd8598)) {
        rb_state6.var_3 = rb_state6.var_0;
    }
    if ((rb_state6.var_2) == (0xfffd8598)) {
        pthread_mutex_lock(&(rb_state6.lock_15));
        if (!((rb_state6.var_0) == (rb_state6.var_3))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (1585 < rb_input_size ? (uint32_t)rb_input[1585] : 0xda6f3e62);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0xf373777a)) {
        if ((rb_state15.var_2) != (0x0)) {
            if (!((rb_state15.var_2) == (rb_state15.var_10))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    routing_cost_t no_swap = 0;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_1) : (0x825ac02e));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (1057 < rb_input_size ? (uint32_t)rb_input[1057] : 0x8d1103b1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_2) == (0x4dc34f23)) ? (rb_state10.var_1) : (0x3f699fb));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x16762b84)) {
        rb_state17.var_18 = 0x4d141fb8;
        rb_state17.var_19 = (rb_state17.var_19) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_11) : (0x5ddaeab6));
        rb_state17.var_20 = (rb_state17.var_20) - (((((0x400e44b8) ^ (0xb30c2b1c)) - (rb_state17.var_18)) - (0x2f0a9c2b)) - (rb_state17.var_10));
        rb_state17.var_21 = (rb_state17.var_21) - (rb_state17.var_19);
        rb_state17.var_22 = (rb_state17.var_22) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_20) : (0xff2384e8));
        rb_state17.var_23 = (rb_state17.var_23) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_21) : (0x91a1a133));
        rb_state17.var_24 = (rb_state17.var_24) ^ (((0xc3efa681) - (rb_state17.var_15)) ^ (rb_state17.var_22));
        rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_23) : (0x52d9ec31));
        rb_state17.var_26 = (rb_state17.var_26) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_24) : (0x5995c53f));
        rb_state17.var_27 = (rb_state17.var_27) - (((((0xc0c1c183) - (rb_state17.var_18)) - (rb_state17.var_26)) ^ (rb_state17.var_25)) - (rb_state17.var_19));
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_27);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_1);
    #endif
    routing_cost_t yes_swap = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0xb68d59b6)) {
            pthread_mutex_lock(&(rb_state3.lock_18));
            rb_state3.var_5 = 0x21c0b398;
            rb_state3.var_6 = (rb_state3.var_6) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x3f2089c2));
            rb_state3.var_7 = (rb_state3.var_7) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_3) : (0x90f14261));
            rb_state3.var_8 = (rb_state3.var_8) + (rb_state3.var_5);
            rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_6) : (0x36051fc9));
            rb_state3.var_10 = (rb_state3.var_10) ^ (((0x9dace9ef) + (rb_state3.var_7)) - (rb_state3.var_8));
            rb_state3.var_11 = (rb_state3.var_11) ^ (((((0xc9b128aa) - (0x3ccbc9fc)) ^ (rb_state3.var_9)) ^ (rb_state3.var_8)) - (rb_state3.var_9));
            rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_10) : (0x5bbbf5ff));
            rb_state3.var_13 = (rb_state3.var_13) + (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_12) : (0x5e043ef5));
            rb_state3.var_4 = (rb_state3.var_4) - (rb_state3.var_13);
            rb_state3.var_3 = rb_state3.var_4;
            pthread_mutex_unlock(&(rb_state3.lock_18));
        }
        if ((rb_state3.var_1) == (0xb68d59b6)) {
            if (!((rb_state3.var_3) == (rb_state3.var_4))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) - (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_2) : (0xbca82886));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ ((0x488ff0) - (rb_state5.var_1));
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_1) == (0x153)) {
            rb_state8.var_3 = rb_state8.var_2;
        }
        if ((rb_state8.var_1) == (0x153)) {
            if (!((rb_state8.var_2) == (rb_state8.var_3))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_2) == (0xb7735511)) {
            rb_state10.var_9 = 0xb55c3b51;
            rb_state10.var_10 = 0x36055b95;
            rb_state10.var_11 = (rb_state10.var_11) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_9) : (0x9b993a83));
            rb_state10.var_12 = (rb_state10.var_12) ^ (((0xb5d84ce) - (rb_state10.var_10)) + (rb_state10.var_7));
            rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_11);
            rb_state10.var_14 = (rb_state10.var_14) - (rb_state10.var_12);
            rb_state10.var_15 = (rb_state10.var_15) - (((0xff058c42) ^ (rb_state10.var_13)) + (rb_state10.var_8));
            rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_14) : (0x9a0e2271));
            rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_15);
            rb_state10.var_18 = (rb_state10.var_18) + (((0x345da86a) + (rb_state10.var_16)) + (0x93c9954a));
            rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_17) : (0xa9588bf2));
            rb_state10.var_20 = (rb_state10.var_20) ^ (rb_state10.var_18);
            rb_state10.var_21 = (rb_state10.var_21) ^ (((((0xef90e7a2) + (rb_state10.var_20)) - (0x4ee4ef31)) + (rb_state10.var_19)) ^ (rb_state10.var_11));
            rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_21);
            rb_state10.var_3 = rb_state10.var_8;
        }
        if ((rb_state10.var_2) == (0xb7735511)) {
            pthread_mutex_lock(&(rb_state10.lock_35));
            rb_state10.var_23 = 0x76a02f23;
            rb_state10.var_24 = 0xe4fc6141;
            rb_state10.var_25 = (rb_state10.var_25) - (((0xb74a6f6c) ^ (rb_state10.var_13)) + (rb_state10.var_23));
            rb_state10.var_26 = (rb_state10.var_26) - (rb_state10.var_14);
            rb_state10.var_27 = (rb_state10.var_27) ^ (((rb_state10.var_24) == (0x0)) ? (rb_state10.var_12) : (0x87c9fe86));
            rb_state10.var_28 = (rb_state10.var_28) - (((((0xecd1678a) + (rb_state10.var_26)) + (0x10087723)) ^ (rb_state10.var_15)) + (rb_state10.var_25));
            rb_state10.var_29 = (rb_state10.var_29) + (rb_state10.var_27);
            rb_state10.var_30 = (rb_state10.var_30) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_28) : (0x86e3027f));
            rb_state10.var_31 = (rb_state10.var_31) ^ (((0x6ef29110) - (rb_state10.var_17)) - (rb_state10.var_29));
            rb_state10.var_32 = (rb_state10.var_32) - (((0x3690c986) - (rb_state10.var_30)) + (rb_state10.var_18));
            rb_state10.var_33 = (rb_state10.var_33) ^ (rb_state10.var_31);
            rb_state10.var_34 = (rb_state10.var_34) - (((((0x3cc75b88) ^ (0xd19a726b)) - (rb_state10.var_33)) ^ (rb_state10.var_32)) - (rb_state10.var_19));
            rb_state10.var_22 = (rb_state10.var_22) + (rb_state10.var_34);
            rb_state10.var_3 = rb_state10.var_22;
            pthread_mutex_unlock(&(rb_state10.lock_35));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (0x8c0e73d4);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0xe8881919));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xe6f28488));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_0);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ ((0x29a1dc0a) - (0x5cca73a));
        if ((rb_state16.var_2) == (0xe1f3c282)) {
            rb_state16.var_4 = 0x67030f5;
            rb_state16.var_5 = 0xf4dea793;
            rb_state16.var_6 = (rb_state16.var_6) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_5) : (0x9bc5293f));
            rb_state16.var_7 = (rb_state16.var_7) + (((0x368e6f76) + (rb_state16.var_5)) + (rb_state16.var_3));
            rb_state16.var_8 = (rb_state16.var_8) - (rb_state16.var_4);
            rb_state16.var_9 = (rb_state16.var_9) ^ (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_6) : (0x49375f05));
            rb_state16.var_10 = (rb_state16.var_10) ^ (((0x378e7509) - (rb_state16.var_7)) - (0x3a7d1ee5));
            rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0x8699f844));
            rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0xe3ced909));
            rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_12);
        }
        #endif
        no_swap += fabs(old_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + (1322 < rb_input_size ? (uint32_t)rb_input[1322] : 0xdcd4750b);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) - (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_0) : (0x40312df));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x642741c0)) {
            pthread_mutex_lock(&(rb_state14.lock_30));
            rb_state14.var_14 = 0x9ccab56;
            rb_state14.var_15 = 0x8c5447ad;
            rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_15) : (0xab9a01ed));
            rb_state14.var_17 = (rb_state14.var_17) ^ (((0xf22e578b) ^ (rb_state14.var_8)) + (rb_state14.var_9));
            rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_16) : (0x64c65c7e));
            rb_state14.var_19 = (rb_state14.var_19) + (((0x45cde416) ^ (rb_state14.var_11)) - (rb_state14.var_17));
            rb_state14.var_20 = (rb_state14.var_20) + (((0x3fe91c12) + (rb_state14.var_18)) + (rb_state14.var_12));
            rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_20) : (0x31443f92));
            rb_state14.var_13 = (rb_state14.var_13) + (rb_state14.var_21);
            rb_state14.var_3 = rb_state14.var_13;
            pthread_mutex_unlock(&(rb_state14.lock_30));
        }
        if ((rb_state14.var_1) == (0x642741c0)) {
            pthread_mutex_lock(&(rb_state14.lock_30));
            rb_state14.var_23 = 0xb0942855;
            rb_state14.var_24 = 0x69795615;
            rb_state14.var_25 = (rb_state14.var_25) ^ (((((0xe32379f8) - (rb_state14.var_24)) ^ (0xa5615cab)) + (rb_state14.var_23)) ^ (0x43131567));
            rb_state14.var_26 = (rb_state14.var_26) + (rb_state14.var_14);
            rb_state14.var_27 = (rb_state14.var_27) + (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_25) : (0xbb8903bd));
            rb_state14.var_28 = (rb_state14.var_28) - (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_26) : (0x7d864f16));
            rb_state14.var_29 = (rb_state14.var_29) - (((((0x56ea525e) + (0xa6f862dc)) - (rb_state14.var_16)) - (rb_state14.var_27)) + (rb_state14.var_28));
            rb_state14.var_22 = (rb_state14.var_22) + (rb_state14.var_29);
            rb_state14.var_3 = rb_state14.var_22;
            pthread_mutex_unlock(&(rb_state14.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (0xb34e80f6);
        rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_0);
        #endif
        no_swap += fabs(old_loc->y - fanin_loc->y);

        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + ((0x998c2063) ^ (rb_state4.var_0));
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_1) == (0x26b81b34)) {
            rb_state5.var_4 = 0x89076872;
            rb_state5.var_5 = (rb_state5.var_5) + (((0x6e504acc) + (0x25832ea7)) + (rb_state5.var_4));
            rb_state5.var_6 = (rb_state5.var_6) ^ (((((0x85a780a) + (0x2d9a5433)) ^ (rb_state5.var_3)) - (0x829b32a6)) + (rb_state5.var_5));
            rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_6);
            rb_state5.var_2 = rb_state5.var_3;
        }
        if ((rb_state5.var_1) == (0x26b81b34)) {
            pthread_mutex_lock(&(rb_state5.lock_13));
            if ((rb_state5.var_2) != (0x0)) {
                if (!((rb_state5.var_2) == (rb_state5.var_7))) {
                    racebench_trigger(5);
                }
            }
            pthread_mutex_unlock(&(rb_state5.lock_13));
        }
        if ((rb_state5.var_1) == (0x26b81b34)) {
            pthread_mutex_lock(&(rb_state5.lock_13));
            rb_state5.var_8 = 0xfb962740;
            rb_state5.var_9 = 0x9384a274;
            rb_state5.var_10 = (rb_state5.var_10) - (rb_state5.var_5);
            rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0xbfc85be0));
            rb_state5.var_12 = (rb_state5.var_12) ^ (((((0x48ef68e9) + (rb_state5.var_11)) + (0x9f382d7a)) ^ (rb_state5.var_6)) + (rb_state5.var_10));
            rb_state5.var_7 = (rb_state5.var_7) - (rb_state5.var_12);
            rb_state5.var_2 = rb_state5.var_7;
            pthread_mutex_unlock(&(rb_state5.lock_13));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_2);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_1);
        if ((rb_state10.var_2) == (0xb7735511)) {
            if ((rb_state10.var_3) != (0x0)) {
                if (!((rb_state10.var_3) == (rb_state10.var_22))) {
                    racebench_trigger(10);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (303 < rb_input_size ? (uint32_t)rb_input[303] : 0x5feb1464);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + ((0x6601f7e0) - (0x6c6da4d));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x28eb0583));
        #endif
        yes_swap += fabs(new_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (6 < rb_input_size ? (uint32_t)rb_input[6] : 0x3ec3d2f8);
        rb_state3.var_1 = (rb_state3.var_1) - (1044 < rb_input_size ? (uint32_t)rb_input[1044] : 0xb96665a8);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_2) == (0xfed8871f)) {
            rb_state4.var_19 = 0x9caf5ce7;
            rb_state4.var_20 = (rb_state4.var_20) + (1854 < rb_input_size ? (uint32_t)rb_input[1854] : 0x2706d7d);
            rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_8);
            rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_9);
            rb_state4.var_23 = (rb_state4.var_23) + (rb_state4.var_19);
            rb_state4.var_24 = (rb_state4.var_24) ^ (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_7) : (0x6dae1037));
            rb_state4.var_25 = (rb_state4.var_25) ^ (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_6) : (0xa7758aa3));
            rb_state4.var_26 = (rb_state4.var_26) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_20) : (0xfb9db1b2));
            rb_state4.var_27 = (rb_state4.var_27) + (((((0xd470a946) ^ (0x2b9a4b8f)) ^ (rb_state4.var_21)) + (0x47a99169)) - (rb_state4.var_22));
            rb_state4.var_28 = (rb_state4.var_28) - (rb_state4.var_23);
            rb_state4.var_29 = (rb_state4.var_29) + (((0x9b67a0c9) ^ (rb_state4.var_24)) + (0x3cd96223));
            rb_state4.var_30 = (rb_state4.var_30) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_25) : (0x124992e9));
            rb_state4.var_31 = (rb_state4.var_31) + (((rb_state4.var_26) == (0x0)) ? (rb_state4.var_27) : (0x96c786b8));
            rb_state4.var_32 = (rb_state4.var_32) - (rb_state4.var_28);
            rb_state4.var_33 = (rb_state4.var_33) ^ (rb_state4.var_29);
            rb_state4.var_34 = (rb_state4.var_34) ^ (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_31) : (0x7feb78c6));
            rb_state4.var_35 = (rb_state4.var_35) ^ (((0x21a55510) - (rb_state4.var_32)) + (0x63792973));
            rb_state4.var_36 = (rb_state4.var_36) ^ (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_33) : (0xd23873a1));
            rb_state4.var_37 = (rb_state4.var_37) ^ (((rb_state4.var_34) == (0x0)) ? (rb_state4.var_35) : (0xd31c457f));
            rb_state4.var_38 = (rb_state4.var_38) + (((0x27f5cd14) ^ (rb_state4.var_15)) ^ (rb_state4.var_36));
            rb_state4.var_39 = (rb_state4.var_39) ^ (((rb_state4.var_37) == (0x0)) ? (rb_state4.var_38) : (0x27cc6e18));
            rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_39);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
        rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xff2502fd));
        rb_state7.var_1 = (rb_state7.var_1) - (575 < rb_input_size ? (uint32_t)rb_input[575] : 0x7404c99e);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (0x2b5eaa23);
        #endif
        yes_swap += fabs(new_loc->y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) + ((0x1d5dc4b4) - (0x668356a6));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (2228 < rb_input_size ? (uint32_t)rb_input[2228] : 0x21f74a9e);
        rb_state4.var_2 = (rb_state4.var_2) ^ ((0x45806bf1) + (0x46380586));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) + ((0xc7edf7aa) - (0x5954be33));
        rb_state7.var_1 = (rb_state7.var_1) + (0x1faad906);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0x88856025) ^ (0xbee304bf));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (0x6bd582da);
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_1);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (1977 < rb_input_size ? (uint32_t)rb_input[1977] : 0x932d3125);
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_2);
        if ((rb_state19.var_0) == (0x496c8278)) {
            pthread_mutex_lock(&(rb_state19.lock_6));
            rb_state19.var_4 = 0x7a560813;
            rb_state19.var_5 = (rb_state19.var_5) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_3) : (0x62ada45e));
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_5);
            pthread_mutex_unlock(&(rb_state19.lock_6));
        }
        #endif
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_2) == (0xfed8871f)) {
            pthread_mutex_lock(&(rb_state4.lock_40));
            rb_state4.var_5 = 0x92c8cdc;
            rb_state4.var_6 = 0x50aeff5e;
            rb_state4.var_7 = (rb_state4.var_7) - (((0xabf932fe) + (0x8609ecc6)) ^ (rb_state4.var_3));
            rb_state4.var_8 = (rb_state4.var_8) ^ ((0xb39f8b76) - (0x7031e942));
            rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0xfcaa7b8));
            rb_state4.var_10 = (rb_state4.var_10) + (rb_state4.var_7);
            rb_state4.var_11 = (rb_state4.var_11) + (((0x24f4703d) - (rb_state4.var_8)) ^ (rb_state4.var_4));
            rb_state4.var_12 = (rb_state4.var_12) + (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0x94be0021));
            rb_state4.var_13 = (rb_state4.var_13) - (((0xe5b92d45) + (rb_state4.var_11)) ^ (0xabc7276b));
            rb_state4.var_14 = (rb_state4.var_14) + (((0xe98fe595) + (0xe7957852)) - (rb_state4.var_12));
            rb_state4.var_15 = (rb_state4.var_15) - (rb_state4.var_13);
            rb_state4.var_16 = (rb_state4.var_16) + (rb_state4.var_14);
            rb_state4.var_17 = (rb_state4.var_17) + (rb_state4.var_15);
            rb_state4.var_18 = (rb_state4.var_18) - (((((0xaecd54bd) - (rb_state4.var_16)) - (rb_state4.var_17)) - (rb_state4.var_5)) ^ (0x6994c29e));
            rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_18);
            rb_state4.var_3 = rb_state4.var_4;
            pthread_mutex_unlock(&(rb_state4.lock_40));
        }
        if ((rb_state4.var_2) == (0xfed8871f)) {
            if (!((rb_state4.var_3) == (rb_state4.var_4))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - ((0x7d0111d1) + (0x6b1b9240));
        rb_state5.var_0 = (rb_state5.var_0) + (0x9577754a);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (0xaf4a6b69);
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (457 < rb_input_size ? (uint32_t)rb_input[457] : 0xe2020cf6);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ ((0xd5fc2a54) + (0xae284512));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + ((0x60dcabd8) - (rb_state14.var_0));
        rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0xa7d258dd));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (450 < rb_input_size ? (uint32_t)rb_input[450] : 0x6f36f946);
        #endif
        no_swap += fabs(old_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (0x9ae229b2);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - ((0x1360ed7c) + (0xbe9f70be));
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_1) == (0x16762b84)) {
            pthread_mutex_lock(&(rb_state17.lock_28));
            rb_state17.var_4 = 0x757bf0c6;
            rb_state17.var_5 = 0xe52d684c;
            rb_state17.var_6 = (rb_state17.var_6) + (((0xda91dc04) - (rb_state17.var_4)) - (0x84d36b9));
            rb_state17.var_7 = (rb_state17.var_7) ^ (rb_state17.var_3);
            rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_2) : (0x146e325a));
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_5) : (0x61092454));
            rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0xff7b1a01));
            rb_state17.var_11 = (rb_state17.var_11) - (((0x17c9cf57) - (rb_state17.var_6)) - (rb_state17.var_8));
            rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_9);
            rb_state17.var_13 = (rb_state17.var_13) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_10) : (0xb347c8a7));
            rb_state17.var_14 = (rb_state17.var_14) ^ (rb_state17.var_11);
            rb_state17.var_15 = (rb_state17.var_15) + (rb_state17.var_12);
            rb_state17.var_16 = (rb_state17.var_16) - (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0xda0d9372));
            rb_state17.var_17 = (rb_state17.var_17) - (((((0xbde19537) - (rb_state17.var_9)) + (rb_state17.var_15)) + (rb_state17.var_16)) + (rb_state17.var_8));
            rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_17);
            rb_state17.var_2 = rb_state17.var_3;
            pthread_mutex_unlock(&(rb_state17.lock_28));
        }
        if ((rb_state17.var_1) == (0x16762b84)) {
            pthread_mutex_lock(&(rb_state17.lock_28));
            if (!((rb_state17.var_2) == (rb_state17.var_3))) {
                racebench_trigger(17);
            }
            pthread_mutex_unlock(&(rb_state17.lock_28));
        }
        #endif
        no_swap += fabs(old_loc->y - fanout_loc->y);

        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + (2148 < rb_input_size ? (uint32_t)rb_input[2148] : 0xf28fac9e);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (0x21b4b17a);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (0x5bb585bc);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (0x2c0d0660);
        if ((rb_state11.var_2) == (0x645d67a8)) {
            pthread_mutex_lock(&(rb_state11.lock_22));
            rb_state11.var_10 = 0xd629db98;
            rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_6);
            rb_state11.var_12 = (rb_state11.var_12) - (((((0x1dc61a17) ^ (rb_state11.var_5)) - (0x1890a79f)) + (0x3ae16871)) ^ (rb_state11.var_4));
            rb_state11.var_13 = (rb_state11.var_13) - (((0x57a33502) - (rb_state11.var_10)) - (0x1179c403));
            rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_12) : (0xbfe2c5b6));
            rb_state11.var_15 = (rb_state11.var_15) ^ (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_14) : (0x918de7c0));
            rb_state11.var_9 = (rb_state11.var_9) - (rb_state11.var_15);
            rb_state11.var_3 = rb_state11.var_9;
            pthread_mutex_unlock(&(rb_state11.lock_22));
        }
        if ((rb_state11.var_2) == (0x645d67a8)) {
            pthread_mutex_lock(&(rb_state11.lock_22));
            rb_state11.var_17 = 0x3778b8af;
            rb_state11.var_18 = (rb_state11.var_18) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_17) : (0xbd6b6b3f));
            rb_state11.var_19 = (rb_state11.var_19) + (((0x512e868b) - (rb_state11.var_7)) - (rb_state11.var_10));
            rb_state11.var_20 = (rb_state11.var_20) + (((((0x932dacb6) + (rb_state11.var_11)) ^ (rb_state11.var_18)) ^ (0xc0779730)) + (rb_state11.var_8));
            rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0x6f078253));
            rb_state11.var_16 = (rb_state11.var_16) + (rb_state11.var_21);
            rb_state11.var_3 = rb_state11.var_16;
            pthread_mutex_unlock(&(rb_state11.lock_22));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (0x7e54cc26);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_2);
        #endif
        yes_swap += fabs(new_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + ((0x66d4a775) ^ (rb_state3.var_2));
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_2) == (0x18c24904)) {
            rb_state7.var_3 = rb_state7.var_1;
        }
        if ((rb_state7.var_2) == (0x18c24904)) {
            if (!((rb_state7.var_1) == (rb_state7.var_3))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (1272 < rb_input_size ? (uint32_t)rb_input[1272] : 0xd0bb5069);
        if ((rb_state15.var_1) == (0xf373777a)) {
            pthread_mutex_lock(&(rb_state15.lock_17));
            rb_state15.var_6 = 0xc0ec22b7;
            rb_state15.var_7 = 0x77446bf3;
            rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xebaf7400) ^ (rb_state15.var_3)) + (rb_state15.var_7)) - (rb_state15.var_4)) - (0x14b06ecb));
            rb_state15.var_9 = (rb_state15.var_9) + (((((0x7c1c3e73) ^ (0xf3d00d8)) - (rb_state15.var_8)) + (rb_state15.var_5)) ^ (rb_state15.var_6));
            rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_9);
            rb_state15.var_2 = rb_state15.var_5;
            pthread_mutex_unlock(&(rb_state15.lock_17));
        }
        if ((rb_state15.var_1) == (0xf373777a)) {
            pthread_mutex_lock(&(rb_state15.lock_17));
            rb_state15.var_11 = 0x99a9becc;
            rb_state15.var_12 = 0x7767c548;
            rb_state15.var_13 = 0xbfdb2360;
            rb_state15.var_14 = (rb_state15.var_14) + (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_13) : (0x2fa12c6e));
            rb_state15.var_15 = (rb_state15.var_15) - (((0xac471825) ^ (rb_state15.var_6)) ^ (rb_state15.var_11));
            rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_15) : (0xec0b4494));
            rb_state15.var_10 = (rb_state15.var_10) - (rb_state15.var_16);
            rb_state15.var_2 = rb_state15.var_10;
            pthread_mutex_unlock(&(rb_state15.lock_17));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x496c8278)) {
            rb_state19.var_3 = rb_state19.var_1;
        }
        if ((rb_state19.var_0) == (0x496c8278)) {
            if (!((rb_state19.var_1) == (rb_state19.var_3))) {
                racebench_trigger(19);
            }
        }
        #endif
        yes_swap += fabs(new_loc->y - fanout_loc->y);
    }

    return yes_swap - no_swap;
}