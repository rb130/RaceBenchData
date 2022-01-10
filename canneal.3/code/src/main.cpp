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
        rb_state0.var_0 = (rb_state0.var_0) - (359 < rb_input_size ? (uint32_t)rb_input[359] : 0xc6bcb7da);
        rb_state0.var_1 = (rb_state0.var_1) - (0x4ece5696);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (2150 < rb_input_size ? (uint32_t)rb_input[2150] : 0xbbb0d9eb);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x5570908c));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ ((0x26965952) - (rb_state18.var_0));
        #endif
        pthread_create(&threads[i], NULL, entry_pt, thread_in);
    }
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (0x2c617e66);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (431 < rb_input_size ? (uint32_t)rb_input[431] : 0x8ba843c4);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_14 = (rb_state12.var_14) ^ (0xd603bd0e);
        rb_state12.var_15 = (rb_state12.var_15) + (61 < rb_input_size ? (uint32_t)rb_input[61] : 0xa5c02a8);
        if ((rb_state12.var_16) == (0x6459ce66)) {
            pthread_mutex_lock(&(rb_state12.lock_44));
            rb_state12.var_23 = 0x9833a758;
            rb_state12.var_24 = 0xf1a37e2c;
            rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_23);
            rb_state12.var_26 = (rb_state12.var_26) ^ (0xaf209462);
            rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_18) == (0x178411a4)) ? (rb_state12.var_24) : (0x19994c2f));
            rb_state12.var_28 = (rb_state12.var_28) ^ (((0x24854b03) + (rb_state12.var_17)) + (rb_state12.var_20));
            rb_state12.var_29 = (rb_state12.var_29) + (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_25) : (0x5f93aadd));
            rb_state12.var_30 = (rb_state12.var_30) - (((rb_state12.var_26) == (0xaf209462)) ? (rb_state12.var_27) : (0x7b93b656));
            rb_state12.var_31 = (rb_state12.var_31) + (((0xec287d05) ^ (rb_state12.var_28)) ^ (0x5b1b9260));
            rb_state12.var_32 = (rb_state12.var_32) - (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_29) : (0xe2e2c3d2));
            rb_state12.var_33 = (rb_state12.var_33) + (((rb_state12.var_23) == (0x9833a758)) ? (rb_state12.var_30) : (0x36f91fde));
            rb_state12.var_34 = (rb_state12.var_34) ^ (((rb_state12.var_31) == (0xd0e087f8)) ? (rb_state12.var_32) : (0x90c59f06));
            rb_state12.var_35 = (rb_state12.var_35) - (((0xf25eae4a) + (0xf56db3c7)) ^ (rb_state12.var_33));
            rb_state12.var_36 = (rb_state12.var_36) ^ (rb_state12.var_34);
            rb_state12.var_37 = (rb_state12.var_37) + (((0x5d25190c) + (rb_state12.var_24)) + (rb_state12.var_35));
            rb_state12.var_38 = (rb_state12.var_38) ^ (((rb_state12.var_36) == (0x67cc58a8)) ? (rb_state12.var_37) : (0xfeff4121));
            rb_state12.var_22 = (rb_state12.var_22) ^ (rb_state12.var_38);
            rb_state12.var_21 = rb_state12.var_22;
            pthread_mutex_unlock(&(rb_state12.lock_44));
        }
        if ((rb_state12.var_16) == (0x6459ce66)) {
            pthread_mutex_lock(&(rb_state12.lock_44));
            rb_state12.var_45 = !((rb_state12.var_21) == (rb_state12.var_22));
            pthread_mutex_unlock(&(rb_state12.lock_44));
        }
        if ((rb_state12.var_32) == (0x67cc58a8)) {
            rb_state12.var_46 = rb_state12.var_33;
        }
        if ((rb_state12.var_32) == (0x67cc58a8)) {
            if (!((rb_state12.var_33) == (rb_state12.var_46))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
        rb_state18.var_0 = (rb_state18.var_0) + ((0x24ef7bd9) - (rb_state18.var_0));
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
    rb_state0.var_0 = (rb_state0.var_0) + (0x8f8893fa);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xc66f9556));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (0xdb5a6ad3);
    rb_state2.var_1 = (rb_state2.var_1) - (688 < rb_input_size ? (uint32_t)rb_input[688] : 0x6b99ca2b);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x9c)) ? (rb_state9.var_0) : (0xc77f3852));
    rb_state9.var_1 = (rb_state9.var_1) ^ (2228 < rb_input_size ? (uint32_t)rb_input[2228] : 0x769a6092);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (2012 < rb_input_size ? (uint32_t)rb_input[2012] : 0x3f47c531);
    rb_state12.var_1 = (rb_state12.var_1) - (203 < rb_input_size ? (uint32_t)rb_input[203] : 0x3795b036);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - ((0xff48f8a1) + (rb_state13.var_1));
    rb_state13.var_1 = (rb_state13.var_1) - (1908 < rb_input_size ? (uint32_t)rb_input[1908] : 0x30bd2d31);
    rb_state13.var_2 = (rb_state13.var_2) ^ (1541 < rb_input_size ? (uint32_t)rb_input[1541] : 0xb7a47667);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (0x4980a746);
    rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_1);
    if ((rb_state18.var_1) == (0x12cc77d2)) {
        pthread_mutex_lock(&(rb_state18.lock_32));
        if ((rb_state18.var_2) != (0x0)) {
            if (!((rb_state18.var_2) == (rb_state18.var_20))) {
                racebench_trigger(18);
            }
        }
        pthread_mutex_unlock(&(rb_state18.lock_32));
    }
    #endif
    annealer_thread *ptr = static_cast<annealer_thread *>(data);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_1) == (0x626352d4)) ? (rb_state0.var_0) : (0xedd562f1));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x24)) ? (rb_state12.var_1) : (0xb6c2d0f));
    rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_2);
    if ((rb_state12.var_2) == (0x0)) {
        rb_state12.var_3 = rb_state12.var_1;
    }
    if ((rb_state12.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_19));
        rb_state12.var_20 = !((rb_state12.var_1) == (rb_state12.var_3));
        pthread_mutex_unlock(&(rb_state12.lock_19));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0xffffffb2)) {
        pthread_mutex_lock(&(rb_state13.lock_26));
        rb_state13.var_5 = 0x2e85e611;
        rb_state13.var_6 = (rb_state13.var_6) - (((((0xa8500112) - (rb_state13.var_4)) - (rb_state13.var_5)) - (rb_state13.var_6)) ^ (rb_state13.var_3));
        rb_state13.var_7 = (rb_state13.var_7) ^ ((0x44ed81bb) - (rb_state13.var_7));
        rb_state13.var_8 = (rb_state13.var_8) - (((((0xe0ed9bf5) + (0x73b82de1)) + (rb_state13.var_5)) + (rb_state13.var_8)) ^ (rb_state13.var_6));
        rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_7) : (0x4f2af44f));
        rb_state13.var_10 = (rb_state13.var_10) ^ (((0xe1118177) ^ (0xa7ca85d7)) ^ (rb_state13.var_8));
        rb_state13.var_11 = (rb_state13.var_11) + (rb_state13.var_9);
        rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_11) : (0xa8ac3dc3));
        rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_12);
        rb_state13.var_3 = rb_state13.var_4;
        pthread_mutex_unlock(&(rb_state13.lock_26));
    }
    if ((rb_state13.var_2) == (0xffffffb2)) {
        pthread_mutex_lock(&(rb_state13.lock_26));
        rb_state13.var_27 = !((rb_state13.var_3) == (rb_state13.var_4));
        pthread_mutex_unlock(&(rb_state13.lock_26));
    }
    rb_state13.var_20 = (rb_state13.var_20) ^ (1457 < rb_input_size ? (uint32_t)rb_input[1457] : 0x844c7545);
    rb_state13.var_23 = (rb_state13.var_23) + (1355 < rb_input_size ? (uint32_t)rb_input[1355] : 0x83e0ae3b);
    #endif
    ptr->Run();
}