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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (0xa3bcfdca);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_2);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - ((0x68de18fb) ^ (rb_state13.var_1));
    if ((rb_state13.var_0) == (0x97497f6b)) {
        rb_state13.var_18 = 0x77e5d0d;
        rb_state13.var_19 = (rb_state13.var_19) - (802 < rb_input_size ? (uint32_t)rb_input[802] : 0x3d68084a);
        rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_18);
        rb_state13.var_21 = (rb_state13.var_21) ^ (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_9) : (0xf50cab2d));
        rb_state13.var_22 = (rb_state13.var_22) - (rb_state13.var_8);
        rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_7);
        rb_state13.var_24 = (rb_state13.var_24) + (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_19) : (0x531e40d));
        rb_state13.var_25 = (rb_state13.var_25) ^ (((((0xa09312eb) ^ (0xd531b5bb)) - (rb_state13.var_20)) + (rb_state13.var_12)) + (rb_state13.var_21));
        rb_state13.var_26 = (rb_state13.var_26) ^ (((0xbfe492a6) ^ (rb_state13.var_13)) - (rb_state13.var_22));
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_23) : (0x65df53d0));
        rb_state13.var_28 = (rb_state13.var_28) ^ (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_24) : (0xedd6b575));
        rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_26) : (0x662849d));
        rb_state13.var_30 = (rb_state13.var_30) ^ (((0xe6281357) - (rb_state13.var_27)) + (rb_state13.var_16));
        rb_state13.var_31 = (rb_state13.var_31) ^ (rb_state13.var_28);
        rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_29) : (0x4dc7edfd));
        rb_state13.var_33 = (rb_state13.var_33) ^ (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0xaf09c575));
        rb_state13.var_34 = (rb_state13.var_34) ^ (((0xb8796ffd) + (rb_state13.var_32)) - (rb_state13.var_18));
        rb_state13.var_35 = (rb_state13.var_35) + (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_33) : (0x428c0940));
        rb_state13.var_36 = (rb_state13.var_36) + (((rb_state13.var_34) == (0x0)) ? (rb_state13.var_35) : (0x35750dad));
        rb_state13.var_4 = (rb_state13.var_4) ^ (rb_state13.var_36);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0x9c1b042c);
    rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_3);
    #endif
    routing_cost_t no_swap = 0;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0xd4058fa4)) ? (rb_state1.var_1) : (0xfa3e12b9));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (1130 < rb_input_size ? (uint32_t)rb_input[1130] : 0xe84fb24f);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (1343 < rb_input_size ? (uint32_t)rb_input[1343] : 0xa82697b6);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_2);
    #endif
    routing_cost_t yes_swap = 0;

    for (int i = 0; i < fanin.size(); ++i) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_3) : (0x58ab89a3));
        rb_state0.var_2 = (rb_state0.var_2) - (0xaa358e6f);
        if ((rb_state0.var_3) == (0x3feca0ec)) {
            if ((rb_state0.var_4) != (0x0)) {
                if (!((rb_state0.var_4) == (rb_state0.var_25))) {
                    racebench_trigger(0);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
        rb_state2.var_4 = (rb_state2.var_4) ^ (0xb113ffb7);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_3 = (rb_state3.var_3) + (1250 < rb_input_size ? (uint32_t)rb_input[1250] : 0x4b4712e6);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) + ((0x640d14ca) + (0x8ec8b3f5));
        rb_state4.var_1 = (rb_state4.var_1) - (1845 < rb_input_size ? (uint32_t)rb_input[1845] : 0x98772029);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x1a13a7de));
        rb_state6.var_3 = (rb_state6.var_3) ^ ((0xff551300) ^ (0x67d2e0c7));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_3);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (1712 < rb_input_size ? (uint32_t)rb_input[1712] : 0x7f43156f);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ ((0x1a9ba3ec) ^ (0xce23d1b8));
        #endif
        location_t *fanin_loc = fanin[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + ((0xe2cb9787) + (rb_state1.var_2));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - (0x5f8eead0);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_2) : (0x16a900e));
        rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_2);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - ((0xf45a951d) - (0xf15f22ba));
        rb_state6.var_2 = (rb_state6.var_2) + ((0xee2393b) ^ (0x71a3ee55));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) + (0xb81092e5);
        rb_state10.var_0 = (rb_state10.var_0) - (1478 < rb_input_size ? (uint32_t)rb_input[1478] : 0x2a06745a);
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0xac17c1a6)) {
            pthread_mutex_lock(&(rb_state13.lock_37));
            rb_state13.var_6 = 0x35d6ef4d;
            rb_state13.var_7 = 0x895e0701;
            rb_state13.var_8 = (rb_state13.var_8) - (((((0x45af22ff) + (0x20ba2ed2)) + (0x7da2f302)) - (rb_state13.var_7)) ^ (rb_state13.var_6));
            rb_state13.var_9 = (rb_state13.var_9) ^ (0x5d595d5c);
            rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_8) : (0x1a460cf));
            rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_9) : (0x329545db));
            rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_10);
            rb_state13.var_13 = (rb_state13.var_13) - (rb_state13.var_11);
            rb_state13.var_14 = (rb_state13.var_14) - (((0x193124fd) ^ (rb_state13.var_6)) + (rb_state13.var_12));
            rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_13);
            rb_state13.var_16 = (rb_state13.var_16) - (rb_state13.var_14);
            rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0xd3f6fe1a));
            rb_state13.var_5 = (rb_state13.var_5) + (rb_state13.var_17);
            rb_state13.var_4 = rb_state13.var_5;
            pthread_mutex_unlock(&(rb_state13.lock_37));
        }
        if ((rb_state13.var_0) == (0x97497f6b)) {
            if (!((rb_state13.var_4) == (rb_state13.var_5))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) - ((0xda82a0e1) + (0xe07f1027));
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_3);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
        rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_1) == (0xd4b87254)) ? (rb_state19.var_2) : (0x2830f060));
        #endif
        no_swap += fabs(old_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (0xeda7506d);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_2) : (0x71dcd182));
        rb_state2.var_1 = (rb_state2.var_1) - (0xfe4aedb);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + ((0xa8ade8cd) + (rb_state4.var_0));
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_3) == (0x566bd649)) {
            rb_state10.var_4 = rb_state10.var_0;
        }
        if ((rb_state10.var_3) == (0x566bd649)) {
            rb_state10.var_5 = 0xab664e98;
            rb_state10.var_6 = 0xf8e3f384;
            rb_state10.var_7 = 0xd7f78a7a;
            rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_6);
            rb_state10.var_9 = (rb_state10.var_9) - (((0x5a388c8a) ^ (rb_state10.var_4)) ^ (rb_state10.var_5));
            rb_state10.var_10 = (rb_state10.var_10) ^ (rb_state10.var_7);
            rb_state10.var_11 = (rb_state10.var_11) + (((((0x8881a8e2) + (rb_state10.var_9)) - (rb_state10.var_5)) - (rb_state10.var_8)) + (0xb0e8ad01));
            rb_state10.var_12 = (rb_state10.var_12) + (((0xa05fa95) - (0xc253eed0)) + (rb_state10.var_10));
            rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_12) : (0xd8be39c2));
            rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_13);
        }
        if ((rb_state10.var_3) == (0x566bd649)) {
            if (!((rb_state10.var_0) == (rb_state10.var_4))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) ^ (((rb_state18.var_1) == (0x6a)) ? (rb_state18.var_4) : (0xe0557e8c));
        if ((rb_state18.var_0) == (0x81c62973)) {
            rb_state18.var_20 = 0xb8b4f70c;
            rb_state18.var_21 = (rb_state18.var_21) + (((0x83cb2bab) ^ (rb_state18.var_12)) + (0xf693b7fb));
            rb_state18.var_22 = (rb_state18.var_22) ^ (((((0xf8888671) ^ (rb_state18.var_11)) - (rb_state18.var_20)) - (0x9e1e9c7)) + (0x9669da7b));
            rb_state18.var_23 = (rb_state18.var_23) + (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_21) : (0x4b359b43));
            rb_state18.var_24 = (rb_state18.var_24) - (rb_state18.var_22);
            rb_state18.var_25 = (rb_state18.var_25) ^ (((0xd1ffd40f) ^ (rb_state18.var_23)) - (rb_state18.var_14));
            rb_state18.var_26 = (rb_state18.var_26) + (((0x81206b4) ^ (rb_state18.var_24)) ^ (rb_state18.var_15));
            rb_state18.var_27 = (rb_state18.var_27) ^ (rb_state18.var_25);
            rb_state18.var_28 = (rb_state18.var_28) - (((rb_state18.var_26) == (0x0)) ? (rb_state18.var_27) : (0xb3f6ca04));
            rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_28);
        }
        #endif
        no_swap += fabs(old_loc->y - fanin_loc->y);

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (0xee3d3657);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_3) : (0x8b756342));
        if ((rb_state3.var_4) == (0xb5a58cfd)) {
            pthread_mutex_lock(&(rb_state3.lock_45));
            if ((rb_state3.var_5) != (0x0)) {
                if (!((rb_state3.var_5) == (rb_state3.var_32))) {
                    racebench_trigger(3);
                }
            }
            pthread_mutex_unlock(&(rb_state3.lock_45));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (88 < rb_input_size ? (uint32_t)rb_input[88] : 0x684646a6);
        if ((rb_state4.var_1) == (0xf1eabb05)) {
            if ((rb_state4.var_4) != (0x0)) {
                if (!((rb_state4.var_4) == (rb_state4.var_27))) {
                    racebench_trigger(4);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) ^ (590 < rb_input_size ? (uint32_t)rb_input[590] : 0xe202006a);
        rb_state13.var_3 = (rb_state13.var_3) - (1236 < rb_input_size ? (uint32_t)rb_input[1236] : 0x77b8929e);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_2) == (0x44fe4ef8)) ? (rb_state14.var_1) : (0x6be5ac3));
        rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_2) == (0x44fe4ef8)) ? (rb_state14.var_1) : (0x15b333d4));
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_4) == (0xd4bb5634)) {
            pthread_mutex_lock(&(rb_state17.lock_40));
            rb_state17.var_27 = 0xfeb09ef0;
            rb_state17.var_28 = (rb_state17.var_28) + (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_27) : (0xc63f694e));
            rb_state17.var_29 = (rb_state17.var_29) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_28) : (0x32a1df5d));
            rb_state17.var_26 = (rb_state17.var_26) + (rb_state17.var_29);
            rb_state17.var_5 = rb_state17.var_26;
            pthread_mutex_unlock(&(rb_state17.lock_40));
        }
        if ((rb_state17.var_4) == (0xd4bb5634)) {
            pthread_mutex_lock(&(rb_state17.lock_40));
            rb_state17.var_31 = (rb_state17.var_31) ^ (1880 < rb_input_size ? (uint32_t)rb_input[1880] : 0xe5d94092);
            rb_state17.var_32 = (rb_state17.var_32) + (((((0x9deb8990) - (rb_state17.var_22)) ^ (rb_state17.var_19)) - (rb_state17.var_23)) + (rb_state17.var_20));
            rb_state17.var_33 = (rb_state17.var_33) ^ (rb_state17.var_24);
            rb_state17.var_34 = (rb_state17.var_34) ^ (((((0x9c88b17b) + (rb_state17.var_21)) - (rb_state17.var_31)) + (0xcafe69fa)) + (0xbdfce240));
            rb_state17.var_35 = (rb_state17.var_35) ^ (rb_state17.var_32);
            rb_state17.var_36 = (rb_state17.var_36) + (((((0x40f19c92) ^ (rb_state17.var_25)) ^ (rb_state17.var_34)) - (0xd1f08ca6)) ^ (rb_state17.var_33));
            rb_state17.var_37 = (rb_state17.var_37) - (((0x4e80cf02) + (rb_state17.var_35)) + (rb_state17.var_26));
            rb_state17.var_38 = (rb_state17.var_38) ^ (((rb_state17.var_27) == (0x0)) ? (rb_state17.var_36) : (0x278d761d));
            rb_state17.var_39 = (rb_state17.var_39) + (((((0xf2a69a72) ^ (rb_state17.var_37)) ^ (rb_state17.var_28)) + (0x9ca596fa)) + (rb_state17.var_38));
            rb_state17.var_30 = (rb_state17.var_30) + (rb_state17.var_39);
            rb_state17.var_5 = rb_state17.var_30;
            pthread_mutex_unlock(&(rb_state17.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) + ((0x692b6598) + (0x7030638f));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_4 = (rb_state19.var_4) ^ (0xda2ca326);
        if ((rb_state19.var_4) == (0x815a4a2)) {
            pthread_mutex_lock(&(rb_state19.lock_25));
            rb_state19.var_7 = 0xcf12f8a0;
            rb_state19.var_8 = 0xcf8fe5a5;
            rb_state19.var_9 = (rb_state19.var_9) ^ (((((0xbe848942) + (0x507698d8)) + (rb_state19.var_7)) ^ (rb_state19.var_8)) - (rb_state19.var_6));
            rb_state19.var_10 = (rb_state19.var_10) - (((0x8132d2af) - (rb_state19.var_5)) + (0x87080303));
            rb_state19.var_11 = (rb_state19.var_11) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0x1efddc56));
            rb_state19.var_6 = (rb_state19.var_6) ^ (rb_state19.var_11);
            rb_state19.var_5 = rb_state19.var_6;
            pthread_mutex_unlock(&(rb_state19.lock_25));
        }
        if ((rb_state19.var_4) == (0x815a4a2)) {
            if (!((rb_state19.var_5) == (rb_state19.var_6))) {
                racebench_trigger(19);
            }
        }
        #endif
        yes_swap += fabs(new_loc->x - fanin_loc->x);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (724 < rb_input_size ? (uint32_t)rb_input[724] : 0x615353eb);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_4 = (rb_state2.var_4) + (120 < rb_input_size ? (uint32_t)rb_input[120] : 0xe443d478);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_0) == (0x62)) ? (rb_state4.var_3) : (0xe1a1bf51));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_3 = (rb_state6.var_3) + (((rb_state6.var_3) == (0x9887f3c7)) ? (rb_state6.var_2) : (0xfb69e1db));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_2);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) ^ (868 < rb_input_size ? (uint32_t)rb_input[868] : 0x42f12c2a);
        rb_state17.var_4 = (rb_state17.var_4) ^ (rb_state17.var_2);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_4 = (rb_state18.var_4) + (0x4f450ea6);
        #endif
        yes_swap += fabs(new_loc->y - fanin_loc->y);
    }

    for (int i = 0; i < fanout.size(); ++i) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + (459 < rb_input_size ? (uint32_t)rb_input[459] : 0xef978721);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ ((0x5715f03c) - (rb_state2.var_1));
        rb_state2.var_2 = (rb_state2.var_2) - ((0x4c81a9fe) - (rb_state2.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + (((rb_state6.var_3) == (0x389cb15)) ? (rb_state6.var_2) : (0x8e58b2e0));
        rb_state6.var_2 = (rb_state6.var_2) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_3) : (0xa303e2d));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_3) : (0x7c6ef97f));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x81c62973)) {
            pthread_mutex_lock(&(rb_state18.lock_29));
            rb_state18.var_7 = 0x4e01c38e;
            rb_state18.var_8 = 0xa510dbad;
            rb_state18.var_9 = (rb_state18.var_9) + (rb_state18.var_5);
            rb_state18.var_10 = (rb_state18.var_10) ^ (((0xf65fb7ee) ^ (rb_state18.var_7)) + (0x174df2d4));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_9) : (0xea9bf13));
            rb_state18.var_12 = (rb_state18.var_12) ^ (((0x989a286b) + (rb_state18.var_10)) ^ (0xc1997001));
            rb_state18.var_13 = (rb_state18.var_13) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_11) : (0xcf40461f));
            rb_state18.var_14 = (rb_state18.var_14) + (((0x86b1360a) + (0x7f45d193)) ^ (rb_state18.var_12));
            rb_state18.var_15 = (rb_state18.var_15) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_13) : (0x5b0f2dc2));
            rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_14) : (0x1ef4b22a));
            rb_state18.var_17 = (rb_state18.var_17) - (((0xecd67890) + (rb_state18.var_15)) ^ (0x7d0d1110));
            rb_state18.var_18 = (rb_state18.var_18) - (((0x6ee606d2) ^ (rb_state18.var_16)) - (rb_state18.var_9));
            rb_state18.var_19 = (rb_state18.var_19) - (((((0xe08c03e4) ^ (rb_state18.var_18)) ^ (0xcebf5933)) - (rb_state18.var_17)) + (rb_state18.var_10));
            rb_state18.var_6 = (rb_state18.var_6) - (rb_state18.var_19);
            rb_state18.var_5 = rb_state18.var_6;
            pthread_mutex_unlock(&(rb_state18.lock_29));
        }
        if ((rb_state18.var_0) == (0x81c62973)) {
            if (!((rb_state18.var_5) == (rb_state18.var_6))) {
                racebench_trigger(18);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_4 = (rb_state19.var_4) + (rb_state19.var_3);
        #endif
        location_t *fanout_loc = fanout[i]->present_loc.Get();
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (1945 < rb_input_size ? (uint32_t)rb_input[1945] : 0x1734200);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_4) == (0xb113fa80)) {
            pthread_mutex_lock(&(rb_state2.lock_26));
            rb_state2.var_7 = 0x67c9f87d;
            rb_state2.var_8 = 0x27553733;
            rb_state2.var_9 = 0x16a5f52a;
            rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x84007c84));
            rb_state2.var_11 = (rb_state2.var_11) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_9) : (0x756bae6a));
            rb_state2.var_12 = (rb_state2.var_12) - (rb_state2.var_10);
            rb_state2.var_13 = (rb_state2.var_13) ^ (((((0x70e267a8) + (rb_state2.var_11)) + (0xd63fdc20)) - (rb_state2.var_6)) ^ (rb_state2.var_12));
            rb_state2.var_6 = (rb_state2.var_6) ^ (rb_state2.var_13);
            rb_state2.var_5 = rb_state2.var_6;
            pthread_mutex_unlock(&(rb_state2.lock_26));
        }
        if ((rb_state2.var_4) == (0xb113fa80)) {
            if (!((rb_state2.var_5) == (rb_state2.var_6))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x9721e705)) ? (rb_state13.var_0) : (0xe06b3e));
        rb_state13.var_1 = (rb_state13.var_1) ^ (0x2cd648fd);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_4 = (rb_state17.var_4) + (((rb_state17.var_1) == (0x5740ec60)) ? (rb_state17.var_0) : (0xe512fd71));
        #endif
        no_swap += fabs(old_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) ^ (1678 < rb_input_size ? (uint32_t)rb_input[1678] : 0xb848a39f);
        rb_state0.var_3 = (rb_state0.var_3) - (((rb_state0.var_1) == (0x92e3e0c4)) ? (rb_state0.var_0) : (0xe8cf0a2a));
        if ((rb_state0.var_3) == (0x6747294f)) {
            pthread_mutex_lock(&(rb_state0.lock_29));
            rb_state0.var_20 = 0x1638db24;
            rb_state0.var_21 = 0xbab026c2;
            rb_state0.var_22 = (rb_state0.var_22) + (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xa4fd2922));
            rb_state0.var_23 = (rb_state0.var_23) - (((0x95a1407e) + (rb_state0.var_9)) ^ (rb_state0.var_8));
            rb_state0.var_24 = (rb_state0.var_24) + (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_23) : (0x9f439b74));
            rb_state0.var_19 = (rb_state0.var_19) ^ (rb_state0.var_24);
            rb_state0.var_4 = rb_state0.var_19;
            pthread_mutex_unlock(&(rb_state0.lock_29));
        }
        if ((rb_state0.var_3) == (0x3feca0ec)) {
            pthread_mutex_lock(&(rb_state0.lock_29));
            rb_state0.var_26 = (rb_state0.var_26) ^ (((((0xa7f36547) + (rb_state0.var_12)) ^ (0x2e3da2e4)) + (rb_state0.var_10)) - (rb_state0.var_13));
            rb_state0.var_27 = (rb_state0.var_27) ^ (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_11) : (0x6b1310b6));
            rb_state0.var_28 = (rb_state0.var_28) - (((((0xda2f3af5) ^ (rb_state0.var_26)) ^ (0xbb5282b0)) + (rb_state0.var_27)) ^ (rb_state0.var_15));
            rb_state0.var_25 = (rb_state0.var_25) - (rb_state0.var_28);
            rb_state0.var_4 = rb_state0.var_25;
            pthread_mutex_unlock(&(rb_state0.lock_29));
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_2) == (0x5819a547)) {
            pthread_mutex_lock(&(rb_state1.lock_20));
            rb_state1.var_5 = 0x5af5e2d0;
            rb_state1.var_6 = 0x7223d85e;
            rb_state1.var_7 = (rb_state1.var_7) - (rb_state1.var_4);
            rb_state1.var_8 = (rb_state1.var_8) - (((((0x3180615) ^ (0x41ca06a1)) + (rb_state1.var_5)) ^ (0x5fb5e576)) ^ (rb_state1.var_6));
            rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_7) : (0x1da92a87));
            rb_state1.var_10 = (rb_state1.var_10) ^ (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_9) : (0x9fb87ca9));
            rb_state1.var_4 = (rb_state1.var_4) ^ (rb_state1.var_10);
            rb_state1.var_3 = rb_state1.var_4;
            pthread_mutex_unlock(&(rb_state1.lock_20));
        }
        if ((rb_state1.var_2) == (0x5819a547)) {
            if (!((rb_state1.var_3) == (rb_state1.var_4))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) ^ ((0x6de0e054) + (0x1b71807c));
        #endif
        no_swap += fabs(old_loc->y - fanout_loc->y);

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - (250 < rb_input_size ? (uint32_t)rb_input[250] : 0xf7f1f69b);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_4) == (0xb113fa80)) {
            rb_state2.var_14 = (rb_state2.var_14) - (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_10) : (0x2e997ffb));
            rb_state2.var_15 = (rb_state2.var_15) - (rb_state2.var_7);
            rb_state2.var_16 = (rb_state2.var_16) ^ (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_12) : (0x5bed8d40));
            rb_state2.var_17 = (rb_state2.var_17) ^ (((0x4e6b1321) - (rb_state2.var_8)) + (0x678f358c));
            rb_state2.var_18 = (rb_state2.var_18) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_14) : (0x3a304ed7));
            rb_state2.var_19 = (rb_state2.var_19) - (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_15) : (0xfd199ebc));
            rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_17) : (0xb14ce1a5));
            rb_state2.var_21 = (rb_state2.var_21) + (rb_state2.var_18);
            rb_state2.var_22 = (rb_state2.var_22) ^ (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_19) : (0x30b6e946));
            rb_state2.var_23 = (rb_state2.var_23) - (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_21) : (0xd0b3fd18));
            rb_state2.var_24 = (rb_state2.var_24) + (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_22) : (0x98177d46));
            rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_23) == (0x0)) ? (rb_state2.var_24) : (0x9b5912a1));
            rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_25);
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) + (1119 < rb_input_size ? (uint32_t)rb_input[1119] : 0x4e0f5e51);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (0x56605274);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + (0xd412c863);
        rb_state17.var_3 = (rb_state17.var_3) + (0x21bbc84a);
        #endif
        yes_swap += fabs(new_loc->x - fanout_loc->x);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (0x316477a6);
        rb_state1.var_2 = (rb_state1.var_2) ^ (((rb_state1.var_1) == (0x725885d6)) ? (rb_state1.var_0) : (0x6bd2681f));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
        rb_state3.var_4 = (rb_state3.var_4) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_3) : (0x306b33fd));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - ((0xce4807f5) - (0x45b4a9da));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ ((0xeae5769f) ^ (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (0x5459fafe);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) + (1395 < rb_input_size ? (uint32_t)rb_input[1395] : 0x3263d8ce);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (0x4c67a5a3);
        rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_3);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) + ((0x1585fa8c) + (rb_state19.var_3));
        #endif
        yes_swap += fabs(new_loc->y - fanout_loc->y);
    }

    return yes_swap - no_swap;
}