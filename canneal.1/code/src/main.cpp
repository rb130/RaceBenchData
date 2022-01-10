#include "racebench_bugs.h"


#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#ifdef ENABLE_THREADS
#include <pthread.h>
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#include "annealer_types.h"
#include "annealer_thread.h"
#include "netlist.h"
#include "rng.h"

using namespace std;

void *entry_pt(void *);

int main(int argc, char *const argv[]) {
#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    cout << "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) << endl << flush;
#else
    cout << "PARSEC Benchmark Suite" << endl << flush;
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_canneal);
#endif

    srandom(3);

    if (argc != 5 && argc != 6) {
        cout << "Usage: " << argv[0] << " NTHREADS NSWAPS TEMP NETLIST [NSTEPS]" << endl;
        exit(1);
    }

    int num_threads = atoi(argv[1]);
    cout << "Threadcount: " << num_threads << endl;
#ifndef ENABLE_THREADS
    if (num_threads != 1) {
        cout << "NTHREADS must be 1 (serial version)" << endl;
        exit(1);
    }
#endif

    int swaps_per_temp = atoi(argv[2]);
    cout << swaps_per_temp << " swaps per temperature step" << endl;

    int start_temp = atoi(argv[3]);
    cout << "start temperature: " << start_temp << endl;

    string filename(argv[4]);
    cout << "netlist filename: " << filename << endl;

    int number_temp_steps = -1;
    if (argc == 6) {
        number_temp_steps = atoi(argv[5]);
        cout << "number of temperature steps: " << number_temp_steps << endl;
    }

    netlist my_netlist(filename);

    annealer_thread a_thread(&my_netlist, num_threads, swaps_per_temp, start_temp, number_temp_steps);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif
#ifdef ENABLE_THREADS
    std::vector<pthread_t> threads(num_threads);
    void *thread_in = static_cast<void *>(&a_thread);
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + ((0x5f5f0544) + (rb_state5.var_0));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
        if ((rb_state12.var_0) == (0x0)) {
            rb_state12.var_2 = rb_state12.var_1;
        }
        if ((rb_state12.var_0) == (0x0)) {
            if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (0x183c42c1);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
        #endif
        pthread_create(&threads[i], NULL, entry_pt, thread_in);
    }
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x931f88f5)) {
            pthread_mutex_lock(&(rb_state2.lock_26));
            rb_state2.var_10 = 0x32527064;
            rb_state2.var_11 = 0x6f6301b6;
            rb_state2.var_12 = (rb_state2.var_12) ^ (((0x66540c3d) - (0x1f206e7c)) ^ (rb_state2.var_7));
            rb_state2.var_13 = (rb_state2.var_13) - (rb_state2.var_8);
            rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_9) == (0x1b614c6f)) ? (rb_state2.var_11) : (0x4d8cce5f));
            rb_state2.var_15 = (rb_state2.var_15) ^ (((rb_state2.var_10) == (0x32527064)) ? (rb_state2.var_10) : (0x47be58fb));
            rb_state2.var_16 = (rb_state2.var_16) ^ (rb_state2.var_12);
            rb_state2.var_17 = (rb_state2.var_17) ^ (((((0xae5fbea7) - (0xffd54dda)) ^ (rb_state2.var_13)) + (rb_state2.var_11)) ^ (rb_state2.var_14));
            rb_state2.var_18 = (rb_state2.var_18) ^ (((0x84a915ae) + (rb_state2.var_12)) - (rb_state2.var_15));
            rb_state2.var_19 = (rb_state2.var_19) + (((((0x9c597083) + (0x1fcd7ddf)) ^ (rb_state2.var_16)) - (rb_state2.var_17)) - (rb_state2.var_13));
            rb_state2.var_20 = (rb_state2.var_20) - (((0xa97c6663) + (0xd1f58fd5)) - (rb_state2.var_18));
            rb_state2.var_21 = (rb_state2.var_21) + (((0x95c71db8) - (rb_state2.var_14)) ^ (rb_state2.var_19));
            rb_state2.var_22 = (rb_state2.var_22) ^ (((rb_state2.var_15) == (0x32527064)) ? (rb_state2.var_20) : (0x6e4f9c8));
            rb_state2.var_23 = (rb_state2.var_23) ^ (((rb_state2.var_16) == (0xae2d2be0)) ? (rb_state2.var_21) : (0xd5f48699));
            rb_state2.var_24 = (rb_state2.var_24) ^ (((rb_state2.var_17) == (0x452df11b)) ? (rb_state2.var_22) : (0xa689a70c));
            rb_state2.var_25 = (rb_state2.var_25) + (((((0x88c249c7) + (rb_state2.var_23)) + (0x3860d7a)) + (rb_state2.var_24)) - (rb_state2.var_18));
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_25);
            pthread_mutex_unlock(&(rb_state2.lock_26));
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x65fbc8b0));
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0xb4cd2757)) {
            rb_state4.var_2 = rb_state4.var_1;
        }
        if ((rb_state4.var_0) == (0xb4cd2757)) {
            if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (767 < rb_input_size ? (uint32_t)rb_input[767] : 0xa0f7993c);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ ((0x4a495df1) ^ (rb_state17.var_0));
        if ((rb_state17.var_0) == (0x4a495df1)) {
            rb_state17.var_3 = 0x9c443309;
            rb_state17.var_4 = 0xcf1f32a;
            rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_3);
            rb_state17.var_6 = (rb_state17.var_6) - (rb_state17.var_4);
            rb_state17.var_7 = (rb_state17.var_7) - (rb_state17.var_2);
            rb_state17.var_8 = (rb_state17.var_8) + (((0xa705a841) + (rb_state17.var_3)) + (rb_state17.var_5));
            rb_state17.var_9 = (rb_state17.var_9) + (((rb_state17.var_6) == (0xf30e0cd6)) ? (rb_state17.var_7) : (0xf8faad72));
            rb_state17.var_10 = (rb_state17.var_10) ^ (rb_state17.var_8);
            rb_state17.var_11 = (rb_state17.var_11) - (((0x90ca6819) ^ (0xba239c15)) + (rb_state17.var_9));
            rb_state17.var_12 = (rb_state17.var_12) ^ (((0xa80735e1) - (0x91fcbe56)) + (rb_state17.var_10));
            rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_11);
            rb_state17.var_14 = (rb_state17.var_14) ^ (rb_state17.var_12);
            rb_state17.var_15 = (rb_state17.var_15) - (((rb_state17.var_4) == (0xcf1f32a)) ? (rb_state17.var_13) : (0xab94d29d));
            rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_5) == (0x9c443309)) ? (rb_state17.var_14) : (0xfbf44d96));
            rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_6) == (0xf30e0cd6)) ? (rb_state17.var_15) : (0x5dbbb53b));
            rb_state17.var_18 = (rb_state17.var_18) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_16) : (0xc22aad82));
            rb_state17.var_19 = (rb_state17.var_19) + (rb_state17.var_17);
            rb_state17.var_20 = (rb_state17.var_20) - (((0x38d0b7e7) - (rb_state17.var_18)) - (rb_state17.var_8));
            rb_state17.var_21 = (rb_state17.var_21) ^ (((0x213e8eeb) + (rb_state17.var_19)) ^ (0x1b76eca8));
            rb_state17.var_22 = (rb_state17.var_22) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_20) : (0x7b64b80b));
            rb_state17.var_23 = (rb_state17.var_23) ^ (rb_state17.var_21);
            rb_state17.var_24 = (rb_state17.var_24) ^ (((((0xa78f8c95) - (rb_state17.var_22)) + (0x124dbdf2)) ^ (rb_state17.var_23)) - (0x2f9229e7));
            rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_24);
        }
        #endif
        pthread_join(threads[i], NULL);
    }
#else
    a_thread.Run();
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    cout << "Final routing is: " << my_netlist.total_routing_cost() << endl;

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif

    return 0;
}

void *entry_pt(void *data) {
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x90b5791c));
    if ((rb_state0.var_0) == (0x0)) {
        rb_state0.var_3 = (rb_state0.var_3) ^ (((((0x6e1f7cbe) ^ (rb_state0.var_1)) - (rb_state0.var_3)) + (rb_state0.var_3)) ^ (rb_state0.var_2));
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_3);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0x7f11f4a3) - (0x108002a0));
    if ((rb_state3.var_0) == (0x6e91f203)) {
        rb_state3.var_3 = 0xfe0163f6;
        rb_state3.var_4 = 0x3c69b1f;
        rb_state3.var_5 = (rb_state3.var_5) - ((0xcb65aab2) - (0x8f6cbca3));
        rb_state3.var_6 = (rb_state3.var_6) - (((0x5adfb4da) - (rb_state3.var_2)) - (rb_state3.var_3));
        rb_state3.var_7 = (rb_state3.var_7) - (((((0xb6078734) ^ (rb_state3.var_3)) ^ (0xdd638032)) - (0x96b3a8d4)) + (rb_state3.var_4));
        rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_5) : (0x489787c9));
        rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0x499a0104));
        rb_state3.var_10 = (rb_state3.var_10) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xb444fa));
        rb_state3.var_11 = (rb_state3.var_11) ^ (rb_state3.var_8);
        rb_state3.var_12 = (rb_state3.var_12) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_9) : (0xbdaf08a4));
        rb_state3.var_13 = (rb_state3.var_13) ^ (((((0x430c7025) - (0xd5d84138)) - (rb_state3.var_10)) - (rb_state3.var_11)) - (0x28e10dbf));
        rb_state3.var_14 = (rb_state3.var_14) - (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_12) : (0x3fea0ffb));
        rb_state3.var_15 = (rb_state3.var_15) ^ (rb_state3.var_13);
        rb_state3.var_16 = (rb_state3.var_16) ^ (((((0xcbed0dec) ^ (0x43fbeb47)) - (rb_state3.var_14)) + (0x5be6ba56)) + (rb_state3.var_15));
        rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_16);
        rb_state3.var_1 = rb_state3.var_2;
    }
    if ((rb_state3.var_0) == (0x6e91f203)) {
        pthread_mutex_lock(&(rb_state3.lock_33));
        rb_state3.var_18 = 0xb26bbd03;
        rb_state3.var_19 = (rb_state3.var_19) - (rb_state3.var_11);
        rb_state3.var_20 = (rb_state3.var_20) ^ (rb_state3.var_12);
        rb_state3.var_21 = (rb_state3.var_21) ^ (rb_state3.var_13);
        rb_state3.var_22 = (rb_state3.var_22) ^ (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_9) : (0xf5421548));
        rb_state3.var_23 = (rb_state3.var_23) - (((((0x450cdd2e) - (0xf6e991f4)) + (0x52663a11)) ^ (rb_state3.var_19)) - (rb_state3.var_18));
        rb_state3.var_24 = (rb_state3.var_24) - (((0xaba95eea) - (rb_state3.var_20)) + (0x80ecf581));
        rb_state3.var_25 = (rb_state3.var_25) ^ (((rb_state3.var_21) == (0x0)) ? (rb_state3.var_22) : (0x7e197f68));
        rb_state3.var_26 = (rb_state3.var_26) + (rb_state3.var_23);
        rb_state3.var_27 = (rb_state3.var_27) ^ (((0xe1d11f6c) + (0x56cc9206)) ^ (rb_state3.var_24));
        rb_state3.var_28 = (rb_state3.var_28) - (((rb_state3.var_25) == (0x0)) ? (rb_state3.var_26) : (0xc1a4500a));
        rb_state3.var_29 = (rb_state3.var_29) - (rb_state3.var_27);
        rb_state3.var_30 = (rb_state3.var_30) - (rb_state3.var_28);
        rb_state3.var_31 = (rb_state3.var_31) ^ (rb_state3.var_29);
        rb_state3.var_32 = (rb_state3.var_32) ^ (((((0x8db7a0f) ^ (rb_state3.var_30)) ^ (rb_state3.var_14)) + (rb_state3.var_31)) + (0xf6959fc5));
        rb_state3.var_17 = (rb_state3.var_17) - (rb_state3.var_32);
        rb_state3.var_1 = rb_state3.var_17;
        pthread_mutex_unlock(&(rb_state3.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0xb679f363) - (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xac061083)) {
        pthread_mutex_lock(&(rb_state10.lock_21));
        rb_state10.var_3 = 0x3011754f;
        rb_state10.var_4 = (rb_state10.var_4) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_1) : (0x8ab5b0b0));
        rb_state10.var_5 = (rb_state10.var_5) - (((0xcf6bafa7) - (0x909a4aa0)) - (rb_state10.var_2));
        rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0xf6cf33c1));
        rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_6);
        rb_state10.var_1 = rb_state10.var_2;
        pthread_mutex_unlock(&(rb_state10.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x8e159e4f));
    rb_state15.var_0 = (rb_state15.var_0) ^ ((0x46c97770) ^ (0xaabe82dc));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    if ((rb_state18.var_0) == (0x0)) {
        if ((rb_state18.var_1) != (0x0)) {
            if (!((rb_state18.var_1) == (rb_state18.var_19))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (0xe6fea43);
    if ((rb_state19.var_0) == (0x1b68fd8b)) {
        rb_state19.var_3 = 0x86b41d9d;
        rb_state19.var_4 = (rb_state19.var_4) + (((((0xfe2a517e) - (rb_state19.var_2)) ^ (rb_state19.var_3)) - (rb_state19.var_4)) - (0x7e1b0d95));
        rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_4);
    }
    #endif
    annealer_thread *ptr = static_cast<annealer_thread *>(data);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_4));
        rb_state0.var_2 = rb_state0.var_1;
        pthread_mutex_unlock(&(rb_state0.lock_4));
    }
    if ((rb_state0.var_0) == (0x0)) {
        rb_state0.var_5 = !((rb_state0.var_1) == (rb_state0.var_2));
    }
    rb_state0.var_3 = (rb_state0.var_3) ^ (rb_state0.var_5);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (1176 < rb_input_size ? (uint32_t)rb_input[1176] : 0x72f6c1ed);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xac061083)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x114)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        rb_state11.var_3 = 0x85f8f5c;
        rb_state11.var_4 = (rb_state11.var_4) + (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_3) : (0x273f73c4));
        rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_4);
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (0xe3e1c9c0);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0xec77f5ac)) {
        rb_state15.var_17 = 0xe6d05fbf;
        rb_state15.var_18 = 0x49180c79;
        rb_state15.var_19 = 0xedc9f69c;
        rb_state15.var_20 = (rb_state15.var_20) ^ (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_19) : (0x1e0199a3));
        rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_18) == (0x0)) ? (rb_state15.var_20) : (0xc88a1810));
        rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_21);
        rb_state15.var_1 = rb_state15.var_16;
    }
    if ((rb_state15.var_0) == (0xec77f5ac)) {
        pthread_mutex_lock(&(rb_state15.lock_26));
        rb_state15.var_23 = (rb_state15.var_23) - (((((0xda212ca2) ^ (rb_state15.var_8)) - (0xf1a009f6)) + (rb_state15.var_10)) - (rb_state15.var_7));
        rb_state15.var_24 = (rb_state15.var_24) + (rb_state15.var_9);
        rb_state15.var_25 = (rb_state15.var_25) - (((((0x663585de) - (rb_state15.var_24)) - (0xe9862388)) - (0x62991201)) ^ (rb_state15.var_23));
        rb_state15.var_22 = (rb_state15.var_22) - (rb_state15.var_25);
        rb_state15.var_1 = rb_state15.var_22;
        pthread_mutex_unlock(&(rb_state15.lock_26));
    }
    #endif
    ptr->Run();
}