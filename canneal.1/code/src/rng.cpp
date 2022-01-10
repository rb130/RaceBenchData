#include "racebench_bugs.h"


#include "rng.h"
#include <stdlib.h>
#include <time.h>

unsigned int Rng::seed = 0;
#ifdef ENABLE_THREADS
pthread_mutex_t Rng::seed_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

long Rng::rand(int max) {
    return _rng->randInt(max - 1);
}

long Rng::rand() {
    return _rng->randInt();
}

double Rng::drand() {
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x3a6)) {
        pthread_mutex_lock(&(rb_state1.lock_17));
        rb_state1.var_7 = 0x78fe04c;
        rb_state1.var_8 = 0xe09fdfa0;
        rb_state1.var_9 = 0xf55df64;
        rb_state1.var_10 = (rb_state1.var_10) ^ (344 < rb_input_size ? (uint32_t)rb_input[344] : 0x13078e0a);
        rb_state1.var_11 = (rb_state1.var_11) - (((0x5a1952cd) ^ (rb_state1.var_4)) + (rb_state1.var_8));
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_7);
        rb_state1.var_13 = (rb_state1.var_13) - (((((0x72b4d5d4) ^ (rb_state1.var_9)) - (0x492ae8bd)) + (rb_state1.var_10)) + (0x4e708cda));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_11) : (0x72ff83ae));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_13) : (0x13d8e59d));
        rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_15) : (0x262f52fc));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_16);
        pthread_mutex_unlock(&(rb_state1.lock_17));
    }
    #endif
    return _rng->rand();
}