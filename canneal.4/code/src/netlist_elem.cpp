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
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (1394 < rb_input_size ? (uint32_t)rb_input[1394] : 0x139c0096);
    #endif
    routing_cost_t no_swap = 0;
    routing_cost_t yes_swap = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_2);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) - (19 < rb_input_size ? (uint32_t)rb_input[19] : 0x95682ac6);
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_1);
        if ((rb_state15.var_3) == (0x8ebd74dd)) {
            rb_state15.var_16 = 0x29b06102;
            rb_state15.var_17 = 0x7b930d73;
            rb_state15.var_18 = 0x6f2f30e0;
            rb_state15.var_19 = (rb_state15.var_19) + (rb_state15.var_18);
            rb_state15.var_20 = (rb_state15.var_20) ^ (rb_state15.var_16);
            rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_17) : (0x5da7b370));
            rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_20) : (0xd9ba2b65));
            rb_state15.var_23 = (rb_state15.var_23) + (((0xd53ad635) - (rb_state15.var_21)) - (0x8b46a2b));
            rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_22) == (0x0)) ? (rb_state15.var_23) : (0x72578731));
            rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_24);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_0);
        rb_state17.var_16 = (rb_state17.var_16) - ((0x887d158f) - (rb_state17.var_17));
        #endif
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0x822a70f9));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_1);
        #endif
        no_swap += fabs(old_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0xdc205a59));
        rb_state14.var_0 = (rb_state14.var_0) ^ (0x96c4f178);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xc742588f));
        rb_state15.var_1 = (rb_state15.var_1) + ((0x7142843f) ^ (rb_state15.var_2));
        #endif
        no_swap += fabs(old_loc->y - fanin_loc->y);

        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (0x585f18bb);
        rb_state18.var_1 = (rb_state18.var_1) - (1763 < rb_input_size ? (uint32_t)rb_input[1763] : 0x520394fb);
        rb_state18.var_19 = (rb_state18.var_19) - ((0x48b02680) - (0x157cb17e));
        rb_state18.var_20 = (rb_state18.var_20) ^ (1629 < rb_input_size ? (uint32_t)rb_input[1629] : 0x67100c45);
        if ((rb_state18.var_21) == (0xa81fe01f)) {
            rb_state18.var_53 = 0xd8e415ed;
            rb_state18.var_54 = (rb_state18.var_54) - (((rb_state18.var_34) == (0x0)) ? (rb_state18.var_31) : (0x18a208c9));
            rb_state18.var_55 = (rb_state18.var_55) - (((((0x48df3ea8) + (rb_state18.var_53)) + (rb_state18.var_32)) - (rb_state18.var_35)) + (rb_state18.var_36));
            rb_state18.var_56 = (rb_state18.var_56) ^ (((rb_state18.var_54) == (0x0)) ? (rb_state18.var_55) : (0xf453993c));
            rb_state18.var_35 = (rb_state18.var_35) - (rb_state18.var_56);
        }
        #endif
        yes_swap += fabs(new_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (300 < rb_input_size ? (uint32_t)rb_input[300] : 0x79ee0699);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_3);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (266 < rb_input_size ? (uint32_t)rb_input[266] : 0x232cd4d4);
        rb_state17.var_0 = (rb_state17.var_0) + ((0xd668f7f0) - (rb_state17.var_1));
        rb_state17.var_20 = (rb_state17.var_20) - (1816 < rb_input_size ? (uint32_t)rb_input[1816] : 0xb94b93c);
        #endif
        yes_swap += fabs(new_loc->y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (0x21dcb563);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0xe3a26b75)) {
            rb_state18.var_34 = !((rb_state18.var_4) == (rb_state18.var_3));
        }
        if ((rb_state18.var_21) == (0xa81fe01f)) {
            pthread_mutex_lock(&(rb_state18.lock_57));
            rb_state18.var_37 = 0x1c6a98b2;
            rb_state18.var_38 = 0xb86fc53a;
            rb_state18.var_39 = (rb_state18.var_39) ^ (rb_state18.var_22);
            rb_state18.var_40 = (rb_state18.var_40) - (((0x405d3c7b) + (0x998715ab)) ^ (rb_state18.var_38));
            rb_state18.var_41 = (rb_state18.var_41) - (((0x3eaf9837) + (rb_state18.var_23)) - (rb_state18.var_37));
            rb_state18.var_42 = (rb_state18.var_42) ^ (((0xae63f1cc) - (rb_state18.var_39)) ^ (0x46043512));
            rb_state18.var_43 = (rb_state18.var_43) ^ (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_40) : (0x80c34bd2));
            rb_state18.var_44 = (rb_state18.var_44) - (((0x3f0690c) + (rb_state18.var_41)) ^ (rb_state18.var_25));
            rb_state18.var_45 = (rb_state18.var_45) + (rb_state18.var_42);
            rb_state18.var_46 = (rb_state18.var_46) - (((0x9343660e) ^ (rb_state18.var_43)) ^ (rb_state18.var_26));
            rb_state18.var_47 = (rb_state18.var_47) ^ (rb_state18.var_44);
            rb_state18.var_48 = (rb_state18.var_48) ^ (rb_state18.var_45);
            rb_state18.var_49 = (rb_state18.var_49) + (((0xa92e25d4) - (rb_state18.var_27)) ^ (rb_state18.var_46));
            rb_state18.var_50 = (rb_state18.var_50) - (((0x87ffd48b) + (rb_state18.var_47)) - (rb_state18.var_28));
            rb_state18.var_51 = (rb_state18.var_51) - (((((0x5f97c05c) ^ (rb_state18.var_49)) ^ (rb_state18.var_48)) ^ (rb_state18.var_29)) ^ (0xd1345049));
            rb_state18.var_52 = (rb_state18.var_52) - (((((0x80460548) - (rb_state18.var_51)) + (rb_state18.var_30)) + (rb_state18.var_50)) + (0xaa252aac));
            rb_state18.var_36 = (rb_state18.var_36) + (rb_state18.var_52);
            rb_state18.var_35 = rb_state18.var_36;
            pthread_mutex_unlock(&(rb_state18.lock_57));
        }
        if ((rb_state18.var_21) == (0xa81fe01f)) {
            pthread_mutex_lock(&(rb_state18.lock_57));
            if (!((rb_state18.var_35) == (rb_state18.var_36))) {
                racebench_trigger(18);
            }
            pthread_mutex_unlock(&(rb_state18.lock_57));
        }
        #endif
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_2);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_2);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0xee49c0dd) - (0x4ec57a68));
        rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_17);
        #endif
        no_swap += fabs(old_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (1560 < rb_input_size ? (uint32_t)rb_input[1560] : 0x4f31125a);
        #endif
        no_swap += fabs(old_loc->y - fanout_loc->y);

        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (0x1975f7f);
        rb_state16.var_2 = (rb_state16.var_2) ^ (298 < rb_input_size ? (uint32_t)rb_input[298] : 0xe9186bca);
        rb_state16.var_3 = (rb_state16.var_3) ^ ((0xa9451347) ^ (0x7f6be2a7));
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_3);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_1);
        if ((rb_state17.var_2) == (0x2321387a)) {
            if ((rb_state17.var_5) != (0x0)) {
                rb_state17.var_24 = !((rb_state17.var_3) == (rb_state17.var_4));
            }
        }
        if ((rb_state17.var_21) == (0xc228957a)) {
            if ((rb_state17.var_25) != (0x0)) {
                if (!((rb_state17.var_25) == (rb_state17.var_41))) {
                    racebench_trigger(17);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_0);
        #endif
        yes_swap += fabs(new_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + (749 < rb_input_size ? (uint32_t)rb_input[749] : 0xdf189eba);
        rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_2);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_2);
        #endif
        yes_swap += fabs(new_loc->y - fanout_loc->y);
    }

    return yes_swap - no_swap;
}