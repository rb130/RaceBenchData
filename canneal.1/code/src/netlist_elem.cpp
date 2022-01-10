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
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x28ed9f42)) {
        if ((rb_state9.var_1) != (0x0)) {
            if (!((rb_state9.var_1) == (rb_state9.var_12))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    routing_cost_t no_swap = 0;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x1dda1595));
    #endif
    routing_cost_t yes_swap = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0xcd23652e) ^ (0x56ba2965));
        #endif
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (1302 < rb_input_size ? (uint32_t)rb_input[1302] : 0x1987614b);
        #endif
        no_swap += fabs(old_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x2a84cfee)) {
            rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_6) : (0xfe655a77));
            rb_state14.var_8 = (rb_state14.var_8) ^ (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_1) : (0xf8e99c4f));
            rb_state14.var_9 = (rb_state14.var_9) ^ (rb_state14.var_7);
            rb_state14.var_10 = (rb_state14.var_10) ^ (rb_state14.var_8);
            rb_state14.var_11 = (rb_state14.var_11) - (((0xca474286) - (rb_state14.var_8)) + (0xf3c7b3d3));
            rb_state14.var_12 = (rb_state14.var_12) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_9) : (0xc244fa3e));
            rb_state14.var_13 = (rb_state14.var_13) ^ (((((0x6f55d7fc) + (0x159891cc)) + (rb_state14.var_11)) + (rb_state14.var_10)) + (rb_state14.var_10));
            rb_state14.var_14 = (rb_state14.var_14) ^ (((0xf4454f0) - (rb_state14.var_11)) ^ (rb_state14.var_12));
            rb_state14.var_15 = (rb_state14.var_15) + (((0x39d56a2f) - (0xd3941bce)) + (rb_state14.var_13));
            rb_state14.var_16 = (rb_state14.var_16) - (((((0x80c62491) - (rb_state14.var_15)) - (0x1e3036be)) + (rb_state14.var_14)) + (0x9fbbb6e8));
            rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_16);
        }
        #endif
        no_swap += fabs(old_loc->y - fanin_loc->y);

        yes_swap += fabs(new_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ ((0x187262d5) - (0x45d25af));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - ((0xa7e50ff2) + (0x2b79f3e5));
        if ((rb_state13.var_0) == (0x8cd34993)) {
            pthread_mutex_lock(&(rb_state13.lock_26));
            rb_state13.var_5 = 0x2cf63929;
            rb_state13.var_6 = 0xac277a50;
            rb_state13.var_7 = (rb_state13.var_7) - (rb_state13.var_4);
            rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_3) : (0x6b206e5d));
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_5) : (0x4341cb56));
            rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0x9e535b6));
            rb_state13.var_11 = (rb_state13.var_11) + (rb_state13.var_8);
            rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_9);
            rb_state13.var_13 = (rb_state13.var_13) ^ (((((0x13a09da5) - (rb_state13.var_11)) ^ (rb_state13.var_8)) + (rb_state13.var_7)) - (rb_state13.var_10));
            rb_state13.var_14 = (rb_state13.var_14) ^ (((((0xdcda37df) + (0x8b5ce60)) + (0xe3235670)) ^ (rb_state13.var_13)) ^ (rb_state13.var_12));
            rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_14);
            rb_state13.var_1 = rb_state13.var_4;
            pthread_mutex_unlock(&(rb_state13.lock_26));
        }
        if ((rb_state13.var_0) == (0x8cd34993)) {
            pthread_mutex_lock(&(rb_state13.lock_26));
            rb_state13.var_16 = 0xe44721b3;
            rb_state13.var_17 = 0x83dc63a8;
            rb_state13.var_18 = (rb_state13.var_18) + (((0x9d7d6f76) - (0x70886425)) + (rb_state13.var_16));
            rb_state13.var_19 = (rb_state13.var_19) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_17) : (0xc6149125));
            rb_state13.var_20 = (rb_state13.var_20) + (rb_state13.var_18);
            rb_state13.var_21 = (rb_state13.var_21) - (((0xdee91eb4) - (rb_state13.var_10)) ^ (rb_state13.var_19));
            rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_20);
            rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_21);
            rb_state13.var_24 = (rb_state13.var_24) ^ (((0xa895409e) + (0xd1491f4f)) + (rb_state13.var_22));
            rb_state13.var_25 = (rb_state13.var_25) ^ (((((0xa2287634) ^ (rb_state13.var_23)) ^ (rb_state13.var_24)) + (0xa9fe92e2)) + (rb_state13.var_11));
            rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_25);
            rb_state13.var_1 = rb_state13.var_15;
            pthread_mutex_unlock(&(rb_state13.lock_26));
        }
        #endif
        yes_swap += fabs(new_loc->y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0xcd505b4b) + (0x94eb70e2));
        #endif
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (0x5234b1c1);
        rb_state6.var_0 = (rb_state6.var_0) - (455 < rb_input_size ? (uint32_t)rb_input[455] : 0xa58b025);
        if ((rb_state6.var_0) == (0x80)) {
            if ((rb_state6.var_1) != (0x0)) {
                if (!((rb_state6.var_1) == (rb_state6.var_34))) {
                    racebench_trigger(6);
                }
            }
        }
        #endif
        no_swap += fabs(old_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x8cd34993)) {
            if ((rb_state13.var_1) != (0x0)) {
                if (!((rb_state13.var_1) == (rb_state13.var_15))) {
                    racebench_trigger(13);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ ((0x256e08) + (0xccffae33));
        #endif
        no_swap += fabs(old_loc->y - fanout_loc->y);

        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (2014 < rb_input_size ? (uint32_t)rb_input[2014] : 0x17344a9d);
        if ((rb_state7.var_0) == (0xfec3310c)) {
            rb_state7.var_21 = 0xa3c3ae23;
            rb_state7.var_22 = 0xf40bf829;
            rb_state7.var_23 = (rb_state7.var_23) + (((((0x2d0723f4) - (rb_state7.var_21)) - (0x71fce432)) ^ (0xb584eeb9)) - (rb_state7.var_22));
            rb_state7.var_24 = (rb_state7.var_24) - (0x52697a05);
            rb_state7.var_25 = (rb_state7.var_25) - (((((0x71ca7b80) - (0x250e7245)) - (rb_state7.var_23)) ^ (0x6dfec3dc)) ^ (rb_state7.var_12));
            rb_state7.var_26 = (rb_state7.var_26) - (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_25) : (0xb4540c70));
            rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_26);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (0x7d1b12a1);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x2a84cfee)) {
            pthread_mutex_lock(&(rb_state14.lock_17));
            rb_state14.var_3 = 0x4fce4bc2;
            rb_state14.var_4 = (rb_state14.var_4) ^ (((((0xe53a1a21) - (rb_state14.var_4)) ^ (rb_state14.var_3)) - (rb_state14.var_3)) - (rb_state14.var_2));
            rb_state14.var_5 = (rb_state14.var_5) + (rb_state14.var_1);
            rb_state14.var_6 = (rb_state14.var_6) - (((((0x15763275) + (rb_state14.var_5)) + (0x3df8d2c4)) ^ (0x38534423)) ^ (rb_state14.var_4));
            rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_6);
            rb_state14.var_1 = rb_state14.var_2;
            pthread_mutex_unlock(&(rb_state14.lock_17));
        }
        if ((rb_state14.var_0) == (0x2a84cfee)) {
            if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
        }
        #endif
        yes_swap += fabs(new_loc->x - fanout_loc->x);
        yes_swap += fabs(new_loc->y - fanout_loc->y);
    }

    return yes_swap - no_swap;
}