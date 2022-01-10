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
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x2d459b5b);
    #endif
    routing_cost_t no_swap = 0;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0x2fd)) {
        rb_state8.var_3 = 0xf07582a6;
        rb_state8.var_4 = 0x5db265b2;
        rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_4);
        rb_state8.var_6 = (rb_state8.var_6) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_3) : (0x95267379));
        rb_state8.var_7 = (rb_state8.var_7) - (rb_state8.var_5);
        rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0x5325e770));
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (0x1534cdce);
    #endif
    routing_cost_t yes_swap = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - ((0x2beb5d03) ^ (rb_state1.var_0));
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x7aadff3f)) {
            pthread_mutex_lock(&(rb_state2.lock_9));
            rb_state2.var_3 = 0x39f5ddf0;
            rb_state2.var_4 = 0x349bce55;
            rb_state2.var_5 = 0xa2e53427;
            rb_state2.var_6 = (rb_state2.var_6) - (((((0xdd378775) - (0xe9c04a75)) ^ (rb_state2.var_5)) + (0x4ed586db)) ^ (rb_state2.var_4));
            rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_3) : (0xf77b9823));
            rb_state2.var_8 = (rb_state2.var_8) ^ (((((0xa485c8d1) + (rb_state2.var_6)) + (0xc5a6a9a)) + (rb_state2.var_7)) - (rb_state2.var_3));
            rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_8);
            pthread_mutex_unlock(&(rb_state2.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + ((0xbe65c508) - (0xd37133af));
        if ((rb_state10.var_0) == (0xb515968b)) {
            pthread_mutex_lock(&(rb_state10.lock_19));
            rb_state10.var_4 = 0xa230cdf2;
            rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_2) : (0x4dbf9123));
            rb_state10.var_6 = (rb_state10.var_6) + (((((0x88c8eadd) ^ (rb_state10.var_5)) - (rb_state10.var_5)) + (rb_state10.var_4)) ^ (rb_state10.var_4));
            rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_6);
            rb_state10.var_2 = rb_state10.var_3;
            pthread_mutex_unlock(&(rb_state10.lock_19));
        }
        if ((rb_state10.var_0) == (0xb515968b)) {
            rb_state10.var_20 = !((rb_state10.var_2) == (rb_state10.var_3));
        }
        if ((rb_state10.var_17) == (0xb0243b07)) {
            if ((rb_state10.var_21) != (0x0)) {
                if (!((rb_state10.var_21) == (rb_state10.var_36))) {
                    racebench_trigger(10);
                }
            }
        }
        #endif
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_1) : (0x177e37bd));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_16 = (rb_state10.var_16) + (1200 < rb_input_size ? (uint32_t)rb_input[1200] : 0xa5b327b7);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (1871 < rb_input_size ? (uint32_t)rb_input[1871] : 0x1aedf766);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0x92df70c8)) {
            rb_state16.var_5 = 0xcd1f9f69;
            rb_state16.var_6 = (rb_state16.var_6) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_6) : (0x35ff5a87));
            rb_state16.var_7 = (rb_state16.var_7) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0xd8d43707));
            rb_state16.var_8 = (rb_state16.var_8) + (755 < rb_input_size ? (uint32_t)rb_input[755] : 0xb99f9603);
            rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_5) : (0xbecdd436));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_6);
            rb_state16.var_11 = (rb_state16.var_11) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0x67bf5e46));
            rb_state16.var_12 = (rb_state16.var_12) ^ (((((0x58ad390a) - (rb_state16.var_9)) - (rb_state16.var_8)) + (0x807794fb)) ^ (rb_state16.var_9));
            rb_state16.var_13 = (rb_state16.var_13) - (((((0x349be837) ^ (rb_state16.var_10)) ^ (rb_state16.var_11)) - (rb_state16.var_10)) + (0x158de97e));
            rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_12) : (0x2a02efd));
            rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x24afe0e7));
            rb_state16.var_16 = (rb_state16.var_16) ^ (((((0x26d04aa4) + (rb_state16.var_13)) ^ (rb_state16.var_14)) - (rb_state16.var_15)) ^ (0x9a0b7e3b));
            rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_16);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_1);
        if ((rb_state19.var_1) == (0xffffea40)) {
            pthread_mutex_lock(&(rb_state19.lock_14));
            rb_state19.var_4 = (rb_state19.var_4) + (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_3) : (0xa243e8c7));
            rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_4);
            rb_state19.var_2 = rb_state19.var_3;
            pthread_mutex_unlock(&(rb_state19.lock_14));
        }
        if ((rb_state19.var_1) == (0xffffea40)) {
            if (!((rb_state19.var_2) == (rb_state19.var_3))) {
                racebench_trigger(19);
            }
        }
        #endif
        no_swap += fabs(old_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (1908 < rb_input_size ? (uint32_t)rb_input[1908] : 0x919c85ee);
        #endif
        no_swap += fabs(old_loc->y - fanin_loc->y);

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + ((0x272809de) + (0xfc963986));
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0xb515968b)) {
            rb_state10.var_7 = (rb_state10.var_7) - (rb_state10.var_3);
            rb_state10.var_8 = (rb_state10.var_8) + (1671 < rb_input_size ? (uint32_t)rb_input[1671] : 0xc816e027);
            rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_6) : (0x1600bd37));
            rb_state10.var_10 = (rb_state10.var_10) + (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_7) : (0xfe0e3ff1));
            rb_state10.var_11 = (rb_state10.var_11) - (((0xf43b26c8) + (rb_state10.var_8)) - (rb_state10.var_8));
            rb_state10.var_12 = (rb_state10.var_12) ^ (((0x1e14dbb0) - (0x1cd177c3)) + (rb_state10.var_9));
            rb_state10.var_13 = (rb_state10.var_13) + (((((0x79541a44) ^ (rb_state10.var_10)) + (rb_state10.var_11)) ^ (0x9c81215d)) ^ (0x529f5088));
            rb_state10.var_14 = (rb_state10.var_14) ^ (rb_state10.var_12);
            rb_state10.var_15 = (rb_state10.var_15) + (((0xf7240d08) + (rb_state10.var_9)) - (rb_state10.var_13));
            rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_14) : (0x35b88cfa));
            rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_15);
            rb_state10.var_18 = (rb_state10.var_18) - (((((0x5022c539) - (rb_state10.var_17)) - (rb_state10.var_16)) - (rb_state10.var_11)) ^ (rb_state10.var_12));
            rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_18);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (((rb_state13.var_0) == (0xe7)) ? (rb_state13.var_0) : (0xc25b1764));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (0xd6737732);
        if ((rb_state19.var_1) == (0xffffea40)) {
            rb_state19.var_5 = 0xf4de8425;
            rb_state19.var_6 = 0xa52d79d2;
            rb_state19.var_7 = 0x78f02d4e;
            rb_state19.var_8 = (rb_state19.var_8) ^ (rb_state19.var_4);
            rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_7) : (0x24b74bb9));
            rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_6) : (0xeee2f3d0));
            rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_8) : (0xdc9e62a5));
            rb_state19.var_12 = (rb_state19.var_12) ^ (((((0x227cd43c) + (rb_state19.var_10)) + (rb_state19.var_7)) + (rb_state19.var_9)) ^ (rb_state19.var_8));
            rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0x75bd382e));
            rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_13);
        }
        #endif
        yes_swap += fabs(new_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_1) == (0xffffffaf)) {
            pthread_mutex_lock(&(rb_state13.lock_23));
            rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_4) : (0xa9091151));
            rb_state13.var_11 = (rb_state13.var_11) ^ (((0x4a8449a3) + (rb_state13.var_6)) ^ (rb_state13.var_8));
            rb_state13.var_12 = (rb_state13.var_12) + (((((0x7cac81a3) - (rb_state13.var_10)) - (rb_state13.var_5)) + (rb_state13.var_9)) ^ (rb_state13.var_10));
            rb_state13.var_13 = (rb_state13.var_13) ^ (((((0x39718e62) - (rb_state13.var_12)) - (rb_state13.var_12)) ^ (rb_state13.var_11)) + (rb_state13.var_11));
            rb_state13.var_9 = (rb_state13.var_9) - (rb_state13.var_13);
            rb_state13.var_2 = rb_state13.var_9;
            pthread_mutex_unlock(&(rb_state13.lock_23));
        }
        if ((rb_state13.var_1) == (0xffffffaf)) {
            pthread_mutex_lock(&(rb_state13.lock_23));
            rb_state13.var_15 = 0x620147b;
            rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_15) : (0xb95d6354));
            rb_state13.var_17 = (rb_state13.var_17) - (0x6beda33e);
            rb_state13.var_18 = (rb_state13.var_18) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_16) : (0x12d7400f));
            rb_state13.var_19 = (rb_state13.var_19) + (((0x3679a581) - (0x4da1026)) + (rb_state13.var_17));
            rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_18) : (0x812e09d0));
            rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_19);
            rb_state13.var_22 = (rb_state13.var_22) ^ (((((0x5d8993c0) ^ (rb_state13.var_20)) + (rb_state13.var_17)) + (rb_state13.var_21)) - (rb_state13.var_16));
            rb_state13.var_14 = (rb_state13.var_14) ^ (rb_state13.var_22);
            rb_state13.var_2 = rb_state13.var_14;
            pthread_mutex_unlock(&(rb_state13.lock_23));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - ((0x33847592) - (rb_state16.var_0));
        #endif
        yes_swap += fabs(new_loc->y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_1) == (0xfffffef2)) {
            pthread_mutex_lock(&(rb_state0.lock_12));
            rb_state0.var_3 = 0x1db8e948;
            rb_state0.var_4 = (rb_state0.var_4) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_1) : (0x8407cbce));
            rb_state0.var_5 = (rb_state0.var_5) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_3) : (0xd362bf57));
            rb_state0.var_6 = (rb_state0.var_6) ^ (((0x86fff498) ^ (rb_state0.var_5)) + (rb_state0.var_2));
            rb_state0.var_7 = (rb_state0.var_7) + (((0x3e559c27) ^ (rb_state0.var_4)) + (0x6f48e4b2));
            rb_state0.var_8 = (rb_state0.var_8) ^ (((((0x9dcae893) + (rb_state0.var_6)) ^ (rb_state0.var_6)) - (0x99a908e1)) + (rb_state0.var_5));
            rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_7) : (0xa2037667));
            rb_state0.var_10 = (rb_state0.var_10) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_8) : (0xf0f4adf2));
            rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_10) : (0x7466dd7a));
            rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_11);
            pthread_mutex_unlock(&(rb_state0.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0x32e4796c);
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_0) == (0x506c0000)) {
            rb_state12.var_3 = 0xb3069630;
            rb_state12.var_4 = (rb_state12.var_4) - (((((0xf81c4ae0) - (rb_state12.var_3)) + (0xe4ec61af)) ^ (rb_state12.var_3)) - (rb_state12.var_2));
            rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_4);
        }
        #endif
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_1) == (0x99c15572)) {
            pthread_mutex_lock(&(rb_state4.lock_13));
            rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_4);
            rb_state4.var_5 = (rb_state4.var_5) - (rb_state4.var_3);
            rb_state4.var_6 = (rb_state4.var_6) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_2) : (0x7f33b00a));
            rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x7c4793d8));
            rb_state4.var_8 = (rb_state4.var_8) + (((((0x56da2afe) ^ (rb_state4.var_7)) ^ (rb_state4.var_7)) ^ (rb_state4.var_6)) - (rb_state4.var_6));
            rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_8);
            rb_state4.var_2 = rb_state4.var_3;
            pthread_mutex_unlock(&(rb_state4.lock_13));
        }
        if ((rb_state4.var_1) == (0x99c15572)) {
            pthread_mutex_lock(&(rb_state4.lock_13));
            if (!((rb_state4.var_2) == (rb_state4.var_3))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_13));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (905 < rb_input_size ? (uint32_t)rb_input[905] : 0x6f8c6638);
        #endif
        no_swap += fabs(old_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x9fa3cd66));
        rb_state0.var_0 = (rb_state0.var_0) - (0x9208980a);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (953 < rb_input_size ? (uint32_t)rb_input[953] : 0xed1b25f6);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0x92df70c8)) {
            pthread_mutex_lock(&(rb_state16.lock_17));
            rb_state16.var_4 = (rb_state16.var_4) + (((((0xdf39ef19) - (rb_state16.var_3)) - (rb_state16.var_3)) ^ (rb_state16.var_4)) - (rb_state16.var_2));
            rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_4);
            rb_state16.var_2 = rb_state16.var_3;
            pthread_mutex_unlock(&(rb_state16.lock_17));
        }
        if ((rb_state16.var_1) == (0x92df70c8)) {
            if (!((rb_state16.var_2) == (rb_state16.var_3))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x5623c7ac));
        #endif
        no_swap += fabs(old_loc->y - fanout_loc->y);

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_1) == (0xb2b750f4)) ? (rb_state1.var_0) : (0x77394b6e));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (0x1e0f49f3);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + (270 < rb_input_size ? (uint32_t)rb_input[270] : 0x9c6d75cf);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
        if ((rb_state12.var_0) == (0x506c0000)) {
            rb_state12.var_2 = rb_state12.var_1;
        }
        if ((rb_state12.var_0) == (0x506c0000)) {
            if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (0x6070b68a);
        #endif
        yes_swap += fabs(new_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - (1320 < rb_input_size ? (uint32_t)rb_input[1320] : 0x9e564ba0);
        if ((rb_state0.var_1) == (0xfffffef2)) {
            pthread_mutex_lock(&(rb_state0.lock_12));
            rb_state0.var_2 = rb_state0.var_0;
            pthread_mutex_unlock(&(rb_state0.lock_12));
        }
        if ((rb_state0.var_1) == (0xfffffef2)) {
            if (!((rb_state0.var_0) == (rb_state0.var_2))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_1) == (0xd814229f)) {
            rb_state1.var_2 = rb_state1.var_0;
        }
        if ((rb_state1.var_1) == (0xd814229f)) {
            rb_state1.var_3 = 0x717953ca;
            rb_state1.var_4 = 0xad8f6698;
            rb_state1.var_5 = (rb_state1.var_5) + (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_3) : (0xedbef8f0));
            rb_state1.var_6 = (rb_state1.var_6) + (620 < rb_input_size ? (uint32_t)rb_input[620] : 0xf0db53a1);
            rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_2) : (0x85358ed2));
            rb_state1.var_8 = (rb_state1.var_8) + (((0xcbb9c21e) + (rb_state1.var_5)) + (rb_state1.var_4));
            rb_state1.var_9 = (rb_state1.var_9) + (((((0x33d10e6c) ^ (rb_state1.var_5)) + (rb_state1.var_6)) ^ (0x94a55be2)) - (0xef2bf741));
            rb_state1.var_10 = (rb_state1.var_10) + (((0xfdbf7719) - (rb_state1.var_7)) + (0xbf96e47c));
            rb_state1.var_11 = (rb_state1.var_11) + (((0x7e593fdb) ^ (rb_state1.var_6)) ^ (rb_state1.var_8));
            rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_9);
            rb_state1.var_13 = (rb_state1.var_13) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0x5caefe9d));
            rb_state1.var_14 = (rb_state1.var_14) ^ (((((0xd0382ffc) + (rb_state1.var_7)) ^ (rb_state1.var_12)) ^ (0x5b1d0529)) - (rb_state1.var_13));
            rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_14);
        }
        if ((rb_state1.var_1) == (0xd814229f)) {
            if (!((rb_state1.var_0) == (rb_state1.var_2))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x48a4f79a)) ? (rb_state2.var_0) : (0x29f5ab9));
        if ((rb_state2.var_0) == (0x7aadff3f)) {
            pthread_mutex_lock(&(rb_state2.lock_9));
            rb_state2.var_2 = rb_state2.var_1;
            pthread_mutex_unlock(&(rb_state2.lock_9));
        }
        if ((rb_state2.var_0) == (0x7aadff3f)) {
            if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (0x5917eef4);
        if ((rb_state8.var_1) == (0x2fd)) {
            pthread_mutex_lock(&(rb_state8.lock_9));
            rb_state8.var_2 = rb_state8.var_0;
            pthread_mutex_unlock(&(rb_state8.lock_9));
        }
        if ((rb_state8.var_1) == (0x2fd)) {
            if (!((rb_state8.var_0) == (rb_state8.var_2))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - ((0x53417872) - (0x3e97d2b4));
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_1);
        rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_14) : (0xa817e820));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (0x2efcb990);
        if ((rb_state17.var_1) == (0x978607e2)) {
            rb_state17.var_13 = 0x21d99243;
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_5) : (0xb4d88204));
            rb_state17.var_15 = (rb_state17.var_15) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_7) : (0x5cb48faf));
            rb_state17.var_16 = (rb_state17.var_16) - ((0x3326e218) - (rb_state17.var_9));
            rb_state17.var_17 = (rb_state17.var_17) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_6) : (0xcaab26b6));
            rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_14) : (0xaeeff552));
            rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_15) : (0xdae5a5d3));
            rb_state17.var_20 = (rb_state17.var_20) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_16) : (0x1f6e1480));
            rb_state17.var_21 = (rb_state17.var_21) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_17) : (0x55b46e93));
            rb_state17.var_22 = (rb_state17.var_22) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_19) : (0xb41e2561));
            rb_state17.var_23 = (rb_state17.var_23) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_20) : (0x5a56923e));
            rb_state17.var_24 = (rb_state17.var_24) - (((rb_state17.var_21) == (0x0)) ? (rb_state17.var_22) : (0x3987205a));
            rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_23) : (0x50bc0d2a));
            rb_state17.var_26 = (rb_state17.var_26) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_24) : (0xbf8d0054));
            rb_state17.var_27 = (rb_state17.var_27) - (rb_state17.var_25);
            rb_state17.var_28 = (rb_state17.var_28) - (((rb_state17.var_26) == (0x0)) ? (rb_state17.var_27) : (0xa5674896));
            rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_28);
            rb_state17.var_2 = rb_state17.var_12;
        }
        if ((rb_state17.var_1) == (0x978607e2)) {
            pthread_mutex_lock(&(rb_state17.lock_44));
            rb_state17.var_30 = 0x356f1ba8;
            rb_state17.var_31 = (rb_state17.var_31) - (1931 < rb_input_size ? (uint32_t)rb_input[1931] : 0xb06492ff);
            rb_state17.var_32 = (rb_state17.var_32) - (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_30) : (0xd41a2982));
            rb_state17.var_33 = (rb_state17.var_33) + (1988 < rb_input_size ? (uint32_t)rb_input[1988] : 0x168c0474);
            rb_state17.var_34 = (rb_state17.var_34) ^ (rb_state17.var_19);
            rb_state17.var_35 = (rb_state17.var_35) ^ (((0x8d256ca) ^ (rb_state17.var_31)) - (rb_state17.var_20));
            rb_state17.var_36 = (rb_state17.var_36) - (((((0x5bced29c) + (rb_state17.var_33)) + (0xa03b1589)) + (rb_state17.var_32)) + (rb_state17.var_21));
            rb_state17.var_37 = (rb_state17.var_37) + (rb_state17.var_34);
            rb_state17.var_38 = (rb_state17.var_38) + (((0x71b56dde) ^ (0xa28cfc8)) - (rb_state17.var_35));
            rb_state17.var_39 = (rb_state17.var_39) ^ (((0x38977ccc) + (0xf63b7e6f)) - (rb_state17.var_36));
            rb_state17.var_40 = (rb_state17.var_40) ^ (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_37) : (0x69414863));
            rb_state17.var_41 = (rb_state17.var_41) - (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_38) : (0x76f252e1));
            rb_state17.var_42 = (rb_state17.var_42) + (((((0x40b91175) + (rb_state17.var_24)) - (rb_state17.var_39)) ^ (rb_state17.var_25)) ^ (rb_state17.var_40));
            rb_state17.var_43 = (rb_state17.var_43) - (((rb_state17.var_41) == (0x0)) ? (rb_state17.var_42) : (0x912ee410));
            rb_state17.var_29 = (rb_state17.var_29) ^ (rb_state17.var_43);
            rb_state17.var_2 = rb_state17.var_29;
            pthread_mutex_unlock(&(rb_state17.lock_44));
        }
        #endif
        yes_swap += fabs(new_loc->y - fanout_loc->y);
    }

    return yes_swap - no_swap;
}