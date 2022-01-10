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
        rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0x7576a593));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ ((0x32420a54) + (rb_state3.var_0));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
        rb_state4.var_2 = (rb_state4.var_2) - ((0x22093ef9) ^ (rb_state4.var_1));
        rb_state4.var_3 = (rb_state4.var_3) + (0x8d5279ee);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xe4c16cc0));
        rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0x9d64abf0) - (0xa88c931b));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
        rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_1) : (0xd2a645a3));
        rb_state7.var_2 = (rb_state7.var_2) ^ ((0xdd9e6782) - (rb_state7.var_2));
        rb_state7.var_1 = (rb_state7.var_1) ^ ((0x80a09be1) + (0xec76211b));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (620 < rb_input_size ? (uint32_t)rb_input[620] : 0x681e64d6);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
        rb_state10.var_0 = (rb_state10.var_0) ^ (2036 < rb_input_size ? (uint32_t)rb_input[2036] : 0x93e8553e);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (0xfd9efa3);
        rb_state11.var_1 = (rb_state11.var_1) - (825 < rb_input_size ? (uint32_t)rb_input[825] : 0x8ffdda42);
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x729ac658));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (1154 < rb_input_size ? (uint32_t)rb_input[1154] : 0x76fa571f);
        rb_state13.var_0 = (rb_state13.var_0) ^ ((0x3b4beebe) + (rb_state13.var_0));
        #endif
        pthread_create(&threads[i], NULL, entry_pt, thread_in);
    }
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_1) : (0xa8e1bcde));
        rb_state3.var_1 = (rb_state3.var_1) ^ (399 < rb_input_size ? (uint32_t)rb_input[399] : 0xb4a8f7ef);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_2);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) - (0x488d7d18);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - ((0xc948fe87) - (0x1f1a8dab));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (0x3806fac);
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (0xc383fd45);
    rb_state0.var_0 = (rb_state0.var_0) + ((0xbc08f698) ^ (rb_state0.var_1));
    rb_state0.var_2 = (rb_state0.var_2) - ((0x56f1af6) - (0x5333b114));
    rb_state0.var_2 = (rb_state0.var_2) - ((0xc7feda3d) - (rb_state0.var_2));
    rb_state0.var_2 = (rb_state0.var_2) + (1111 < rb_input_size ? (uint32_t)rb_input[1111] : 0x3a5f4065);
    rb_state0.var_3 = (rb_state0.var_3) + (0xf31860f7);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xc3a174c1));
    rb_state1.var_1 = (rb_state1.var_1) + ((0x4aea36f) + (rb_state1.var_1));
    rb_state1.var_1 = (rb_state1.var_1) + ((0x302db6cf) - (rb_state1.var_0));
    rb_state1.var_2 = (rb_state1.var_2) - (0x28c94da);
    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_1) == (0x34dc5a3e)) ? (rb_state1.var_2) : (0xb7c18a57));
    rb_state1.var_3 = (rb_state1.var_3) ^ (0x7bb4800a);
    rb_state1.var_3 = (rb_state1.var_3) ^ (0xebd2f07f);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ (0x8d76cfa7);
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - ((0x97383bb8) - (rb_state6.var_0));
    rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (547 < rb_input_size ? (uint32_t)rb_input[547] : 0xba40235f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ ((0x94175af8) + (0xf92364a1));
    rb_state8.var_0 = (rb_state8.var_0) + ((0x4488719a) + (rb_state8.var_1));
    rb_state8.var_3 = (rb_state8.var_3) + ((0xfc037609) - (rb_state8.var_2));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0xb2ca993e));
    rb_state10.var_1 = (rb_state10.var_1) + (0x8c47eb3);
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0xe05f0ea3) - (0x4b6ff527));
    rb_state10.var_2 = (rb_state10.var_2) - ((0x7d793892) ^ (rb_state10.var_2));
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (0xe2e0f005);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_1);
    rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_0) : (0x418a6395));
    rb_state12.var_0 = (rb_state12.var_0) + ((0xadf7743c) + (rb_state12.var_1));
    rb_state12.var_1 = (rb_state12.var_1) + (((rb_state12.var_1) == (0xfc36fca0)) ? (rb_state12.var_0) : (0x6ecda488));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + (1966 < rb_input_size ? (uint32_t)rb_input[1966] : 0x3143157f);
    rb_state13.var_3 = (rb_state13.var_3) + (0xa865c2b);
    rb_state13.var_2 = (rb_state13.var_2) ^ (rb_state13.var_3);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x55a3ca85);
    rb_state19.var_1 = (rb_state19.var_1) + (1558 < rb_input_size ? (uint32_t)rb_input[1558] : 0x99b2e6d7);
    #endif
    annealer_thread *ptr = static_cast<annealer_thread *>(data);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0xe630c1ee)) {
        rb_state0.var_4 = rb_state0.var_0;
    }
    if ((rb_state0.var_3) == (0xe630c1ee)) {
        rb_state0.var_28 = !((rb_state0.var_0) == (rb_state0.var_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x90667075)) {
        rb_state1.var_8 = 0x4273526;
        rb_state1.var_9 = 0xef8062a;
        rb_state1.var_10 = (rb_state1.var_10) + (rb_state1.var_9);
        rb_state1.var_11 = (rb_state1.var_11) ^ (460 < rb_input_size ? (uint32_t)rb_input[460] : 0xcfedefcb);
        rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x57547146));
        rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_8) : (0xa4992a5b));
        rb_state1.var_14 = (rb_state1.var_14) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x405fcfc7));
        rb_state1.var_15 = (rb_state1.var_15) + (rb_state1.var_11);
        rb_state1.var_16 = (rb_state1.var_16) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_12) : (0x2c80edcc));
        rb_state1.var_17 = (rb_state1.var_17) ^ (((0x4022fb11) - (0xe3c6875c)) ^ (rb_state1.var_13));
        rb_state1.var_18 = (rb_state1.var_18) + (((((0x4f0961a) - (0xc280da5e)) ^ (0x58d0ec6)) + (rb_state1.var_14)) - (rb_state1.var_15));
        rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_17) : (0x603f7132));
        rb_state1.var_20 = (rb_state1.var_20) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_18) : (0xd6044a46));
        rb_state1.var_21 = (rb_state1.var_21) ^ (rb_state1.var_19);
        rb_state1.var_22 = (rb_state1.var_22) - (((0x420d3f3b) ^ (rb_state1.var_20)) + (0x921c201c));
        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_21) : (0xaeba742b));
        rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_22);
        rb_state1.var_25 = (rb_state1.var_25) + (((((0xb56b3aa2) ^ (rb_state1.var_13)) ^ (rb_state1.var_23)) + (rb_state1.var_24)) ^ (0xce31729c));
        rb_state1.var_7 = (rb_state1.var_7) - (rb_state1.var_25);
        rb_state1.var_4 = rb_state1.var_7;
    }
    if ((rb_state1.var_3) == (0x90667075)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        rb_state1.var_27 = (rb_state1.var_27) - (rb_state1.var_17);
        rb_state1.var_28 = (rb_state1.var_28) - ((0x95b642c4) ^ (0xb83afe80));
        rb_state1.var_29 = (rb_state1.var_29) - ((0x48481ea7) - (0xbd33667a));
        rb_state1.var_30 = (rb_state1.var_30) ^ (((((0x6adf9879) ^ (0xa273fa31)) - (rb_state1.var_14)) ^ (rb_state1.var_18)) + (rb_state1.var_16));
        rb_state1.var_31 = (rb_state1.var_31) ^ (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_27) : (0x6601e43d));
        rb_state1.var_32 = (rb_state1.var_32) ^ (rb_state1.var_28);
        rb_state1.var_33 = (rb_state1.var_33) - (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_30) : (0xa0064dc5));
        rb_state1.var_34 = (rb_state1.var_34) ^ (((0xb2e9ee73) + (rb_state1.var_31)) + (0x6e904d16));
        rb_state1.var_35 = (rb_state1.var_35) + (((0x4ac6c183) ^ (rb_state1.var_32)) - (rb_state1.var_19));
        rb_state1.var_36 = (rb_state1.var_36) + (((0x3c4e39eb) + (rb_state1.var_33)) + (0xb544dccb));
        rb_state1.var_37 = (rb_state1.var_37) ^ (((rb_state1.var_34) == (0x0)) ? (rb_state1.var_35) : (0x4c28a2e4));
        rb_state1.var_38 = (rb_state1.var_38) + (((0x21b0d8e3) - (rb_state1.var_20)) + (rb_state1.var_36));
        rb_state1.var_39 = (rb_state1.var_39) + (((rb_state1.var_37) == (0x0)) ? (rb_state1.var_38) : (0x2e4af7e1));
        rb_state1.var_26 = (rb_state1.var_26) ^ (rb_state1.var_39);
        rb_state1.var_4 = rb_state1.var_26;
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x3db28485));
    rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_3);
    rb_state2.var_0 = (rb_state2.var_0) - (1882 < rb_input_size ? (uint32_t)rb_input[1882] : 0xfc543a1d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
    rb_state3.var_1 = (rb_state3.var_1) ^ (727 < rb_input_size ? (uint32_t)rb_input[727] : 0xa2314161);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x390784be);
    rb_state4.var_3 = (rb_state4.var_3) ^ ((0xdfa4ef2a) - (rb_state4.var_2));
    rb_state4.var_0 = (rb_state4.var_0) + (0x2856a36);
    rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_2);
    rb_state4.var_3 = (rb_state4.var_3) - ((0xbadf5b46) + (0xf4261602));
    if ((rb_state4.var_0) == (0x50ad46c)) {
        rb_state4.var_13 = 0xd035dc5c;
        rb_state4.var_14 = 0xdcd3d482;
        rb_state4.var_15 = (rb_state4.var_15) - ((0x5ca9ed) + (rb_state4.var_9));
        rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_8) : (0x73b77d79));
        rb_state4.var_17 = (rb_state4.var_17) + (((0x7a653cf) ^ (rb_state4.var_11)) ^ (rb_state4.var_13));
        rb_state4.var_18 = (rb_state4.var_18) ^ (((((0x58bcdb5a) ^ (rb_state4.var_15)) - (0x75148751)) ^ (rb_state4.var_12)) + (rb_state4.var_14));
        rb_state4.var_19 = (rb_state4.var_19) ^ (((0x4b897659) + (rb_state4.var_13)) - (rb_state4.var_16));
        rb_state4.var_20 = (rb_state4.var_20) ^ (rb_state4.var_17);
        rb_state4.var_21 = (rb_state4.var_21) ^ (((0xa9448311) + (rb_state4.var_18)) ^ (rb_state4.var_14));
        rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_20) : (0x4664b7f2));
        rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_22) : (0x101b2478));
        rb_state4.var_12 = (rb_state4.var_12) ^ (rb_state4.var_23);
        rb_state4.var_4 = rb_state4.var_12;
    }
    if ((rb_state4.var_0) == (0x3e92bb08)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_24))) {
                racebench_trigger(4);
            }
        }
    }
    if ((rb_state4.var_0) == (0x3e92bb08)) {
        pthread_mutex_lock(&(rb_state4.lock_36));
        rb_state4.var_25 = 0x9d856826;
        rb_state4.var_26 = (rb_state4.var_26) ^ (((((0xa4bfcb09) + (rb_state4.var_17)) - (rb_state4.var_15)) - (0x1a7670d8)) + (rb_state4.var_25));
        rb_state4.var_27 = (rb_state4.var_27) + (0xd283afbb);
        rb_state4.var_28 = (rb_state4.var_28) - (rb_state4.var_18);
        rb_state4.var_29 = (rb_state4.var_29) ^ (((0xc662217b) + (rb_state4.var_19)) - (rb_state4.var_16));
        rb_state4.var_30 = (rb_state4.var_30) - (rb_state4.var_26);
        rb_state4.var_31 = (rb_state4.var_31) - (((((0xef38d2f3) ^ (rb_state4.var_27)) - (rb_state4.var_28)) ^ (rb_state4.var_20)) - (rb_state4.var_21));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_30) : (0x6a0882fe));
        rb_state4.var_33 = (rb_state4.var_33) ^ (rb_state4.var_31);
        rb_state4.var_34 = (rb_state4.var_34) ^ (rb_state4.var_32);
        rb_state4.var_35 = (rb_state4.var_35) - (((rb_state4.var_33) == (0x0)) ? (rb_state4.var_34) : (0xf96215d4));
        rb_state4.var_24 = (rb_state4.var_24) ^ (rb_state4.var_35);
        rb_state4.var_4 = rb_state4.var_24;
        pthread_mutex_unlock(&(rb_state4.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_3);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_1) : (0xd4d41abd));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_1) == (0xb73bc504)) ? (rb_state7.var_0) : (0x2116cb24));
    rb_state7.var_3 = (rb_state7.var_3) + (rb_state7.var_3);
    rb_state7.var_3 = (rb_state7.var_3) + ((0x60aa597) - (0x34871ad1));
    if ((rb_state7.var_3) == (0x748aa052)) {
        rb_state7.var_19 = 0xa2537247;
        rb_state7.var_20 = 0x73096c71;
        rb_state7.var_21 = (rb_state7.var_21) ^ (94 < rb_input_size ? (uint32_t)rb_input[94] : 0x8f873078);
        rb_state7.var_22 = (rb_state7.var_22) - (((0x1ee5b55f) + (0x45f0904d)) + (rb_state7.var_19));
        rb_state7.var_23 = (rb_state7.var_23) ^ (((0x19541e60) ^ (0xa521c463)) ^ (rb_state7.var_20));
        rb_state7.var_24 = (rb_state7.var_24) + (((((0x33193e82) ^ (0xb52d0940)) - (rb_state7.var_12)) - (0x41b76928)) + (rb_state7.var_21));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((0x31705dd4) - (0x8fa07559)) - (rb_state7.var_22));
        rb_state7.var_26 = (rb_state7.var_26) - (((((0x7948a831) + (rb_state7.var_13)) - (rb_state7.var_14)) - (rb_state7.var_23)) ^ (rb_state7.var_24));
        rb_state7.var_27 = (rb_state7.var_27) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_25) : (0x31e788b0));
        rb_state7.var_28 = (rb_state7.var_28) - (((0x506dca16) ^ (rb_state7.var_16)) + (rb_state7.var_26));
        rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_28) : (0xa9ab159b));
        rb_state7.var_18 = (rb_state7.var_18) - (rb_state7.var_29);
        rb_state7.var_4 = rb_state7.var_18;
    }
    if ((rb_state7.var_3) == (0x748aa052)) {
        pthread_mutex_lock(&(rb_state7.lock_48));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_19) : (0x28dcc739));
        rb_state7.var_32 = (rb_state7.var_32) - (0xebfe1e36);
        rb_state7.var_33 = (rb_state7.var_33) + (((0xf317f7bb) - (rb_state7.var_18)) + (0x90a8b0fc));
        rb_state7.var_34 = (rb_state7.var_34) - (rb_state7.var_21);
        rb_state7.var_35 = (rb_state7.var_35) + (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_31) : (0x1c7dbe7));
        rb_state7.var_36 = (rb_state7.var_36) + (rb_state7.var_32);
        rb_state7.var_37 = (rb_state7.var_37) - (((rb_state7.var_22) == (0x0)) ? (rb_state7.var_33) : (0x7ef0036e));
        rb_state7.var_38 = (rb_state7.var_38) - (((0x8edb8888) - (rb_state7.var_34)) - (0xb1469678));
        rb_state7.var_39 = (rb_state7.var_39) + (((rb_state7.var_35) == (0x0)) ? (rb_state7.var_36) : (0x9bd92b0d));
        rb_state7.var_40 = (rb_state7.var_40) - (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_37) : (0xce9d44d6));
        rb_state7.var_41 = (rb_state7.var_41) + (((rb_state7.var_38) == (0x0)) ? (rb_state7.var_39) : (0x57dce9ec));
        rb_state7.var_42 = (rb_state7.var_42) ^ (rb_state7.var_40);
        rb_state7.var_43 = (rb_state7.var_43) - (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_41) : (0xed7c58cd));
        rb_state7.var_44 = (rb_state7.var_44) - (rb_state7.var_42);
        rb_state7.var_45 = (rb_state7.var_45) + (rb_state7.var_43);
        rb_state7.var_46 = (rb_state7.var_46) - (((0x186d48cc) ^ (0x733ce9c3)) ^ (rb_state7.var_44));
        rb_state7.var_47 = (rb_state7.var_47) - (((((0x1d04c3d1) - (rb_state7.var_45)) - (rb_state7.var_46)) - (rb_state7.var_25)) ^ (0xeab01d42));
        rb_state7.var_30 = (rb_state7.var_30) - (rb_state7.var_47);
        rb_state7.var_4 = rb_state7.var_30;
        pthread_mutex_unlock(&(rb_state7.lock_48));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (0xf3821907);
    if ((rb_state8.var_2) == (0xb77282e8)) {
        rb_state8.var_5 = 0x6fb5d72;
        rb_state8.var_6 = 0xb45a15ee;
        rb_state8.var_7 = (rb_state8.var_7) ^ (((0x787e9283) ^ (0xee5c488c)) + (rb_state8.var_5));
        rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_5);
        rb_state8.var_9 = (rb_state8.var_9) - (rb_state8.var_6);
        rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_4);
        rb_state8.var_11 = (rb_state8.var_11) + (rb_state8.var_7);
        rb_state8.var_12 = (rb_state8.var_12) - (((((0x74e99163) ^ (0xf7c5900a)) ^ (rb_state8.var_8)) + (rb_state8.var_9)) ^ (0x3517008));
        rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_10) : (0xbc88f84d));
        rb_state8.var_14 = (rb_state8.var_14) ^ (((0x1eb32870) ^ (rb_state8.var_11)) ^ (rb_state8.var_7));
        rb_state8.var_15 = (rb_state8.var_15) + (((((0xb2a0cd11) - (0x83b06783)) ^ (rb_state8.var_13)) + (rb_state8.var_8)) ^ (rb_state8.var_12));
        rb_state8.var_16 = (rb_state8.var_16) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_14) : (0x354ad094));
        rb_state8.var_17 = (rb_state8.var_17) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_15) : (0x39428fd1));
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_16) : (0x9cb6207b));
        rb_state8.var_19 = (rb_state8.var_19) + (((0xde1e187) ^ (0x85d487f8)) - (rb_state8.var_17));
        rb_state8.var_20 = (rb_state8.var_20) ^ (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_18) : (0x15a748e));
        rb_state8.var_21 = (rb_state8.var_21) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_19) : (0x45b7e34c));
        rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_20) : (0x3b57fa9d));
        rb_state8.var_23 = (rb_state8.var_23) - (((0x308a8cb4) ^ (rb_state8.var_21)) + (0x8ef9dd8a));
        rb_state8.var_24 = (rb_state8.var_24) + (rb_state8.var_22);
        rb_state8.var_25 = (rb_state8.var_25) - (rb_state8.var_23);
        rb_state8.var_26 = (rb_state8.var_26) + (((0xec2a28ae) + (rb_state8.var_24)) + (0x9bd3d76d));
        rb_state8.var_27 = (rb_state8.var_27) ^ (rb_state8.var_25);
        rb_state8.var_28 = (rb_state8.var_28) ^ (rb_state8.var_26);
        rb_state8.var_29 = (rb_state8.var_29) + (rb_state8.var_27);
        rb_state8.var_30 = (rb_state8.var_30) - (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_28) : (0x8431a5f0));
        rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_29) : (0xe5d81a6e));
        rb_state8.var_32 = (rb_state8.var_32) ^ (((0xa6732adb) + (rb_state8.var_17)) + (rb_state8.var_30));
        rb_state8.var_33 = (rb_state8.var_33) ^ (rb_state8.var_31);
        rb_state8.var_34 = (rb_state8.var_34) ^ (rb_state8.var_32);
        rb_state8.var_35 = (rb_state8.var_35) + (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_33) : (0x8b3fda94));
        rb_state8.var_36 = (rb_state8.var_36) + (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_34) : (0xe516992c));
        rb_state8.var_37 = (rb_state8.var_37) - (((rb_state8.var_35) == (0x0)) ? (rb_state8.var_36) : (0x26a805d4));
        rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_37);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_1);
    rb_state9.var_2 = (rb_state9.var_2) ^ (89 < rb_input_size ? (uint32_t)rb_input[89] : 0x7fd0d7b);
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x51bbd6f7));
    rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0xe6e75605));
    if ((rb_state9.var_2) == (0x76)) {
        rb_state9.var_4 = rb_state9.var_3;
    }
    if ((rb_state9.var_2) == (0x76)) {
        if (!((rb_state9.var_3) == (rb_state9.var_4))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0xb2a6efd8)) {
        pthread_mutex_lock(&(rb_state10.lock_16));
        rb_state10.var_5 = 0xe858fad8;
        rb_state10.var_6 = (rb_state10.var_6) + (940 < rb_input_size ? (uint32_t)rb_input[940] : 0x9061435e);
        rb_state10.var_7 = (rb_state10.var_7) - (((((0x757c5947) ^ (rb_state10.var_6)) ^ (rb_state10.var_4)) ^ (rb_state10.var_3)) ^ (rb_state10.var_5));
        rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0xc478681f));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((0x562a437e) - (rb_state10.var_7)) ^ (0xc7c7e7f4));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((((0x16a0d98e) - (rb_state10.var_8)) + (0x8ad28a6c)) + (0x50ad7ed8)) - (rb_state10.var_9));
        rb_state10.var_4 = (rb_state10.var_4) ^ (rb_state10.var_10);
        rb_state10.var_3 = rb_state10.var_4;
        pthread_mutex_unlock(&(rb_state10.lock_16));
    }
    if ((rb_state10.var_2) == (0xb2a6efd8)) {
        if (!((rb_state10.var_3) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) + ((0xcf9fc667) + (0x59dcf7e5));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_2) : (0x5c157ec1));
    if ((rb_state12.var_2) == (0x0)) {
        rb_state12.var_13 = 0x8f5bf658;
        rb_state12.var_14 = (rb_state12.var_14) + (((0x885cd8d5) + (rb_state12.var_13)) - (0x29d633b4));
        rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_9) : (0x7bf31dc3));
        rb_state12.var_16 = (rb_state12.var_16) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_10) : (0x51b91422));
        rb_state12.var_17 = (rb_state12.var_17) ^ (((((0xeeb5d2d0) - (rb_state12.var_14)) ^ (rb_state12.var_13)) ^ (rb_state12.var_15)) ^ (0x317fa477));
        rb_state12.var_18 = (rb_state12.var_18) - (((0xa751d0bd) - (0xddebba89)) + (rb_state12.var_16));
        rb_state12.var_19 = (rb_state12.var_19) ^ (((((0x11cf7a06) + (rb_state12.var_17)) ^ (rb_state12.var_14)) ^ (rb_state12.var_18)) - (rb_state12.var_15));
        rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_19);
        rb_state12.var_3 = rb_state12.var_12;
    }
    if ((rb_state12.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_33));
        rb_state12.var_21 = 0x4f58fb66;
        rb_state12.var_22 = 0xe0648018;
        rb_state12.var_23 = (rb_state12.var_23) ^ (0x4a7b427d);
        rb_state12.var_24 = (rb_state12.var_24) - (((0x36322f3a) - (rb_state12.var_16)) - (0x56986913));
        rb_state12.var_25 = (rb_state12.var_25) - (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_22) : (0xac340849));
        rb_state12.var_26 = (rb_state12.var_26) + (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_23) : (0x382aa014));
        rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_24) : (0xf9057051));
        rb_state12.var_28 = (rb_state12.var_28) ^ (((0x4baa93be) ^ (rb_state12.var_19)) + (rb_state12.var_25));
        rb_state12.var_29 = (rb_state12.var_29) - (((((0xe598c9c8) ^ (rb_state12.var_20)) - (rb_state12.var_27)) - (rb_state12.var_26)) ^ (0x3834fe10));
        rb_state12.var_30 = (rb_state12.var_30) + (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_28) : (0xdd2e9f0f));
        rb_state12.var_31 = (rb_state12.var_31) - (rb_state12.var_29);
        rb_state12.var_32 = (rb_state12.var_32) ^ (((rb_state12.var_30) == (0x0)) ? (rb_state12.var_31) : (0x2063b49b));
        rb_state12.var_20 = (rb_state12.var_20) - (rb_state12.var_32);
        rb_state12.var_3 = rb_state12.var_20;
        pthread_mutex_unlock(&(rb_state12.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0xa9fad4a)) {
        pthread_mutex_lock(&(rb_state13.lock_39));
        rb_state13.var_16 = 0xcafb78af;
        rb_state13.var_17 = 0xf69b5357;
        rb_state13.var_18 = (rb_state13.var_18) - (rb_state13.var_16);
        rb_state13.var_19 = (rb_state13.var_19) ^ (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_9) : (0xbf42bb6d));
        rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_17);
        rb_state13.var_21 = (rb_state13.var_21) - (((0xe07ba3f0) ^ (rb_state13.var_8)) ^ (rb_state13.var_11));
        rb_state13.var_22 = (rb_state13.var_22) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_18) : (0x27ca4d7d));
        rb_state13.var_23 = (rb_state13.var_23) ^ (((0xb6d10037) ^ (rb_state13.var_19)) ^ (rb_state13.var_13));
        rb_state13.var_24 = (rb_state13.var_24) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_20) : (0x8a79c31));
        rb_state13.var_25 = (rb_state13.var_25) ^ (rb_state13.var_21);
        rb_state13.var_26 = (rb_state13.var_26) - (rb_state13.var_22);
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_24) : (0x98073328));
        rb_state13.var_28 = (rb_state13.var_28) - (((0xa3e7cfd1) ^ (rb_state13.var_25)) ^ (rb_state13.var_15));
        rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_26) : (0x8c8ccd71));
        rb_state13.var_30 = (rb_state13.var_30) - (((0xf8769eda) + (rb_state13.var_17)) - (rb_state13.var_27));
        rb_state13.var_31 = (rb_state13.var_31) + (((((0xae17599e) - (rb_state13.var_18)) - (rb_state13.var_28)) + (rb_state13.var_29)) ^ (rb_state13.var_19));
        rb_state13.var_32 = (rb_state13.var_32) - (((((0xda5e4d9e) + (0x18a76f38)) + (rb_state13.var_31)) + (rb_state13.var_20)) + (rb_state13.var_30));
        rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_32);
        rb_state13.var_4 = rb_state13.var_15;
        pthread_mutex_unlock(&(rb_state13.lock_39));
    }
    if ((rb_state13.var_2) == (0xa9fad4a)) {
        pthread_mutex_lock(&(rb_state13.lock_39));
        rb_state13.var_34 = 0xba8961b;
        rb_state13.var_35 = (rb_state13.var_35) ^ (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_22) : (0x171879cb));
        rb_state13.var_36 = (rb_state13.var_36) - (((0x34393029) - (0x2356092c)) ^ (rb_state13.var_34));
        rb_state13.var_37 = (rb_state13.var_37) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_35) : (0xc24b0327));
        rb_state13.var_38 = (rb_state13.var_38) ^ (((rb_state13.var_36) == (0x0)) ? (rb_state13.var_37) : (0xc69680c1));
        rb_state13.var_33 = (rb_state13.var_33) + (rb_state13.var_38);
        rb_state13.var_4 = rb_state13.var_33;
        pthread_mutex_unlock(&(rb_state13.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0xa2c61a2e);
    rb_state19.var_3 = (rb_state19.var_3) + (((rb_state19.var_2) == (0x5d39e5d2)) ? (rb_state19.var_1) : (0x44d22f73));
    rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_0);
    rb_state19.var_1 = (rb_state19.var_1) - (((rb_state19.var_3) == (0x9e764451)) ? (rb_state19.var_2) : (0x3aea98fb));
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_2) == (0xba73cba4)) ? (rb_state19.var_1) : (0xde9d9486));
    if ((rb_state19.var_3) == (0x396c18ed)) {
        rb_state19.var_16 = 0x258e5b1e;
        rb_state19.var_17 = 0xd430f5fd;
        rb_state19.var_18 = (rb_state19.var_18) + (rb_state19.var_12);
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_16) : (0xd133d53d));
        rb_state19.var_20 = (rb_state19.var_20) - (((((0xa6fbc575) + (rb_state19.var_17)) - (rb_state19.var_11)) + (0xa3718450)) ^ (0xf7584d37));
        rb_state19.var_21 = (rb_state19.var_21) + (rb_state19.var_18);
        rb_state19.var_22 = (rb_state19.var_22) ^ (((((0xd0b58434) ^ (rb_state19.var_14)) + (rb_state19.var_20)) + (rb_state19.var_19)) - (0xac96acab));
        rb_state19.var_23 = (rb_state19.var_23) - (((((0x6045b9b7) - (rb_state19.var_21)) ^ (0xfc8c8548)) - (rb_state19.var_15)) + (rb_state19.var_22));
        rb_state19.var_4 = (rb_state19.var_4) ^ (rb_state19.var_23);
    }
    #endif
    ptr->Run();
}