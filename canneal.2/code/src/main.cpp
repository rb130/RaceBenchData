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
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x1734a243));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x2dc7f3bf));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0x85dd744b) + (rb_state18.var_0));
        rb_state18.var_0 = (rb_state18.var_0) - (2169 < rb_input_size ? (uint32_t)rb_input[2169] : 0xcccdd50a);
        #endif
        pthread_create(&threads[i], NULL, entry_pt, thread_in);
    }
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_1) == (0x0)) {
            rb_state7.var_3 = 0xbe11af76;
            rb_state7.var_4 = 0x8c105a3;
            rb_state7.var_5 = (rb_state7.var_5) - (rb_state7.var_3);
            rb_state7.var_6 = (rb_state7.var_6) - (rb_state7.var_2);
            rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_3) == (0xbe11af76)) ? (rb_state7.var_4) : (0x32034725));
            rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_5) == (0x41ee508a)) ? (rb_state7.var_6) : (0xd19f53f8));
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x8c105a3)) ? (rb_state7.var_8) : (0x5435a036));
            rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_1) : (0x78546557));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0xd0)) {
            if ((rb_state18.var_2) != (0x0)) {
                if (!((rb_state18.var_2) == (rb_state18.var_27))) {
                    racebench_trigger(18);
                }
            }
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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
    if ((rb_state3.var_1) == (0x3c685944)) {
        if ((rb_state3.var_2) != (0x0)) {
            if (!((rb_state3.var_2) == (rb_state3.var_24))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - ((0x392d1ce3) - (rb_state6.var_0));
    rb_state6.var_0 = (rb_state6.var_0) ^ (0xe5104496);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_1) : (0x92357efc));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (2062 < rb_input_size ? (uint32_t)rb_input[2062] : 0x1d8b9e9);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x74882092)) {
        rb_state14.var_3 = 0xc3433c14;
        rb_state14.var_4 = 0x153da4f7;
        rb_state14.var_5 = (rb_state14.var_5) + ((0xc1b04596) ^ (0xb66c2f70));
        rb_state14.var_6 = (rb_state14.var_6) + (((((0x51f8240d) - (rb_state14.var_4)) ^ (rb_state14.var_3)) - (rb_state14.var_2)) + (rb_state14.var_3));
        rb_state14.var_7 = (rb_state14.var_7) - (((((0x32dcb1d0) ^ (rb_state14.var_6)) - (rb_state14.var_4)) ^ (rb_state14.var_5)) ^ (rb_state14.var_5));
        rb_state14.var_8 = (rb_state14.var_8) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_7) : (0x779518ee));
        rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (1267 < rb_input_size ? (uint32_t)rb_input[1267] : 0x1eb70ea3);
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (808 < rb_input_size ? (uint32_t)rb_input[808] : 0x76ba14e4);
    rb_state18.var_1 = (rb_state18.var_1) + (1483 < rb_input_size ? (uint32_t)rb_input[1483] : 0x44e79cf7);
    if ((rb_state18.var_1) == (0xd0)) {
        rb_state18.var_18 = 0xcf1de5b5;
        rb_state18.var_19 = (rb_state18.var_19) + (rb_state18.var_5);
        rb_state18.var_20 = (rb_state18.var_20) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_18) : (0x8eb5fea5));
        rb_state18.var_21 = (rb_state18.var_21) + (rb_state18.var_6);
        rb_state18.var_22 = (rb_state18.var_22) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_19) : (0x68556d66));
        rb_state18.var_23 = (rb_state18.var_23) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_20) : (0xe970e0a8));
        rb_state18.var_24 = (rb_state18.var_24) ^ (rb_state18.var_21);
        rb_state18.var_25 = (rb_state18.var_25) - (((((0x82714122) - (0xf69dafab)) - (rb_state18.var_22)) - (rb_state18.var_23)) + (rb_state18.var_10));
        rb_state18.var_26 = (rb_state18.var_26) ^ (((((0xbd5a3a2b) ^ (rb_state18.var_24)) - (rb_state18.var_12)) + (rb_state18.var_25)) - (rb_state18.var_11));
        rb_state18.var_17 = (rb_state18.var_17) ^ (rb_state18.var_26);
        rb_state18.var_2 = rb_state18.var_17;
    }
    if ((rb_state18.var_1) == (0xd0)) {
        pthread_mutex_lock(&(rb_state18.lock_47));
        rb_state18.var_28 = (rb_state18.var_28) + (0x5ab1156d);
        rb_state18.var_29 = (rb_state18.var_29) + (rb_state18.var_16);
        rb_state18.var_30 = (rb_state18.var_30) + (rb_state18.var_17);
        rb_state18.var_31 = (rb_state18.var_31) - (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_18) : (0x1bacc59));
        rb_state18.var_32 = (rb_state18.var_32) + (((0x22fc15c5) + (rb_state18.var_15)) + (rb_state18.var_20));
        rb_state18.var_33 = (rb_state18.var_33) + (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_13) : (0xc8f48c0));
        rb_state18.var_34 = (rb_state18.var_34) - (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_28) : (0x1a87dfab));
        rb_state18.var_35 = (rb_state18.var_35) + (((rb_state18.var_29) == (0x0)) ? (rb_state18.var_30) : (0xa28cb4ae));
        rb_state18.var_36 = (rb_state18.var_36) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_31) : (0x8130ee13));
        rb_state18.var_37 = (rb_state18.var_37) + (((rb_state18.var_32) == (0x0)) ? (rb_state18.var_33) : (0xcb2fcacf));
        rb_state18.var_38 = (rb_state18.var_38) + (((((0xcabb84b1) - (rb_state18.var_34)) + (rb_state18.var_24)) - (rb_state18.var_23)) - (rb_state18.var_35));
        rb_state18.var_39 = (rb_state18.var_39) - (((0x95fdeaf5) + (rb_state18.var_36)) + (0x7658ef9d));
        rb_state18.var_40 = (rb_state18.var_40) - (((0xa4ff3) ^ (rb_state18.var_25)) ^ (rb_state18.var_37));
        rb_state18.var_41 = (rb_state18.var_41) ^ (rb_state18.var_38);
        rb_state18.var_42 = (rb_state18.var_42) ^ (rb_state18.var_39);
        rb_state18.var_43 = (rb_state18.var_43) + (((rb_state18.var_40) == (0x0)) ? (rb_state18.var_41) : (0xee099967));
        rb_state18.var_44 = (rb_state18.var_44) ^ (((rb_state18.var_26) == (0x0)) ? (rb_state18.var_42) : (0x85642932));
        rb_state18.var_45 = (rb_state18.var_45) ^ (((rb_state18.var_27) == (0x0)) ? (rb_state18.var_43) : (0x5578d789));
        rb_state18.var_46 = (rb_state18.var_46) - (((rb_state18.var_44) == (0x0)) ? (rb_state18.var_45) : (0xbc303b5b));
        rb_state18.var_27 = (rb_state18.var_27) ^ (rb_state18.var_46);
        rb_state18.var_2 = rb_state18.var_27;
        pthread_mutex_unlock(&(rb_state18.lock_47));
    }
    #endif
    annealer_thread *ptr = static_cast<annealer_thread *>(data);
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (0xefd02357);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (0xd2f024e0);
    if ((rb_state9.var_1) == (0xb9bbf8c0)) {
        rb_state9.var_4 = 0x1;
    }
    if ((rb_state9.var_1) == (0xfaa782e0)) {
        pthread_mutex_lock(&(rb_state9.lock_13));
        rb_state9.var_5 = 0xc022f34e;
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0x2d2c4260) ^ (rb_state9.var_4)) ^ (rb_state9.var_5)) ^ (rb_state9.var_2)) + (rb_state9.var_5));
        rb_state9.var_7 = (rb_state9.var_7) + (1571 < rb_input_size ? (uint32_t)rb_input[1571] : 0x9b30ce62);
        rb_state9.var_8 = (rb_state9.var_8) - (595 < rb_input_size ? (uint32_t)rb_input[595] : 0x18555821);
        rb_state9.var_9 = (rb_state9.var_9) ^ (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_3) : (0x39883175));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((((0xa1f12725) + (rb_state9.var_7)) - (rb_state9.var_6)) - (0x4f6c4f60)) + (0x790cca2));
        rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0x142c9ef));
        rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xc57701e9));
        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_12);
        rb_state9.var_2 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_13));
    }
    rb_state9.var_10 = (rb_state9.var_10) ^ ((0xca53671a) - (rb_state9.var_11));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_1) == (0xffffffec)) ? (rb_state11.var_0) : (0xf61014a0));
    if ((rb_state11.var_0) == (0x0)) {
        rb_state11.var_3 = 0x2a3caaad;
        rb_state11.var_4 = (rb_state11.var_4) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0xcedf64a3));
        rb_state11.var_5 = (rb_state11.var_5) ^ (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_4) : (0xc4b6d0b1));
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_1) : (0xdcecadf6));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xa988e31e));
    if ((rb_state15.var_1) == (0x138)) {
        if ((rb_state15.var_2) != (0x0)) {
            if (!((rb_state15.var_2) == (rb_state15.var_18))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    ptr->Run();
}