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
        rb_state5.var_0 = (rb_state5.var_0) ^ (1235 < rb_input_size ? (uint32_t)rb_input[1235] : 0xaab4e73c);
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
        rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_1) : (0x5f4198eb));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xf061940a));
        rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_1);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (0x19f3e20e);
        rb_state9.var_0 = (rb_state9.var_0) ^ (0x7f4b101);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (1682 < rb_input_size ? (uint32_t)rb_input[1682] : 0x96f229a5);
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
        rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_3) : (0xdcc745a));
        rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_3);
        rb_state11.var_0 = (rb_state11.var_0) - (264 < rb_input_size ? (uint32_t)rb_input[264] : 0x8a88f821);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xe64460c8));
        rb_state12.var_1 = (rb_state12.var_1) + ((0x8229ad66) - (rb_state12.var_1));
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xb68e23));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_1);
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_2);
        #endif
        pthread_create(&threads[i], NULL, entry_pt, thread_in);
    }
    for (int i = 0; i < num_threads; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_4 = (rb_state5.var_4) - ((0x68c03854) + (0xadb9c3f5));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (0xba17385c);
        if ((rb_state11.var_2) == (0x235ea67e)) {
            pthread_mutex_lock(&(rb_state11.lock_23));
            rb_state11.var_6 = (rb_state11.var_6) + (((((0xa930063b) ^ (rb_state11.var_5)) ^ (rb_state11.var_4)) + (0x979e390d)) - (0x5f3fbd51));
            rb_state11.var_5 = (rb_state11.var_5) + (rb_state11.var_6);
            rb_state11.var_4 = rb_state11.var_5;
            pthread_mutex_unlock(&(rb_state11.lock_23));
        }
        if ((rb_state11.var_2) == (0x235ea67e)) {
            if (!((rb_state11.var_4) == (rb_state11.var_5))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) ^ ((0x397ccff4) + (0x8bce8103));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_4 = (rb_state16.var_4) - (0x8e89249c);
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
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_1);
    rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_0);
    rb_state5.var_3 = (rb_state5.var_3) ^ (979 < rb_input_size ? (uint32_t)rb_input[979] : 0x75629f64);
    rb_state5.var_4 = (rb_state5.var_4) + (0x26cf8dec);
    rb_state5.var_3 = (rb_state5.var_3) - (((rb_state5.var_3) == (0x31)) ? (rb_state5.var_4) : (0x40b1fd79));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xde338199));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (0x4c3456ae);
    rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_2);
    rb_state7.var_2 = (rb_state7.var_2) + ((0x28097d76) - (0x45f68a14));
    rb_state7.var_0 = (rb_state7.var_0) + (2119 < rb_input_size ? (uint32_t)rb_input[2119] : 0x5228229b);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) + (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_3) : (0x2c65c28));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_3) : (0xa6c8afef));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x235ea67e)) {
        rb_state11.var_7 = 0x9c2eb18b;
        rb_state11.var_8 = 0xfd5073a3;
        rb_state11.var_9 = 0x5254a248;
        rb_state11.var_10 = (rb_state11.var_10) - (0x952fe356);
        rb_state11.var_11 = (rb_state11.var_11) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_8) : (0x42bdc3b9));
        rb_state11.var_12 = (rb_state11.var_12) + (rb_state11.var_9);
        rb_state11.var_13 = (rb_state11.var_13) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_10) : (0xd5cba945));
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_11);
        rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_12);
        rb_state11.var_16 = (rb_state11.var_16) + (rb_state11.var_13);
        rb_state11.var_17 = (rb_state11.var_17) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_14) : (0x24c676e3));
        rb_state11.var_18 = (rb_state11.var_18) - (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_15) : (0x6d755781));
        rb_state11.var_19 = (rb_state11.var_19) + (((((0x2cd10fc7) + (rb_state11.var_10)) + (rb_state11.var_9)) + (rb_state11.var_16)) + (rb_state11.var_17));
        rb_state11.var_20 = (rb_state11.var_20) + (rb_state11.var_18);
        rb_state11.var_21 = (rb_state11.var_21) + (((0xbf2a85ba) - (rb_state11.var_19)) - (rb_state11.var_11));
        rb_state11.var_22 = (rb_state11.var_22) ^ (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_21) : (0x4ef9a8cb));
        rb_state11.var_4 = (rb_state11.var_4) - (rb_state11.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_4) == (0x0)) {
        rb_state12.var_18 = 0x2d02e247;
        rb_state12.var_19 = 0xde6b2573;
        rb_state12.var_20 = 0x9cbcd886;
        rb_state12.var_21 = (rb_state12.var_21) + ((0xbd5e43b1) + (0x5a2fc157));
        rb_state12.var_22 = (rb_state12.var_22) ^ (((((0x2c842d0a) + (rb_state12.var_12)) - (0x48b9be62)) ^ (rb_state12.var_19)) + (rb_state12.var_18));
        rb_state12.var_23 = (rb_state12.var_23) + (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_21) : (0x2a73f27a));
        rb_state12.var_24 = (rb_state12.var_24) - (((((0x55cda821) - (rb_state12.var_23)) + (rb_state12.var_22)) + (rb_state12.var_13)) + (0xff9a94d1));
        rb_state12.var_5 = (rb_state12.var_5) + (rb_state12.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (0x268fa0e1);
    #endif
    annealer_thread *ptr = static_cast<annealer_thread *>(data);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_2) : (0xee58f3ff));
    rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_2);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (0xb6346b6f);
    rb_state7.var_3 = (rb_state7.var_3) - (1918 < rb_input_size ? (uint32_t)rb_input[1918] : 0x337378db);
    rb_state7.var_3 = (rb_state7.var_3) - (0x18f9d092);
    if ((rb_state7.var_3) == (0xb5128d27)) {
        if ((rb_state7.var_4) != (0x0)) {
            if (!((rb_state7.var_4) == (rb_state7.var_32))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_2);
    if ((rb_state8.var_4) == (0x0)) {
        rb_state8.var_6 = 0xb1f235d2;
        rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_5) : (0x1408f2e4));
        rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0xb431d949);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (0xe2a9a018);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_3);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + ((0x47f5f6f4) ^ (0x890038b7));
    rb_state15.var_2 = (rb_state15.var_2) ^ (0x5924f4ff);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x6911fc59));
    rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_3);
    #endif
    ptr->Run();
}