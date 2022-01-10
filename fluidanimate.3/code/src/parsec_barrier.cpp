#include "racebench_bugs.h"


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "parsec_barrier.hpp"

#define NOT_IMPLEMENTED() Not_Implemented(__FUNCTION__, __FILE__, __LINE__);

static inline void Not_Implemented(const char *function, const char *file, unsigned int line) {
    fprintf(stderr, "ERROR: Something in function %s in file %s, line %u is not implemented.\n", function, file, line);
    exit(1);
}

#ifdef ENABLE_SPIN_BARRIER

typedef unsigned long long int spin_counter_t;

static const spin_counter_t SPIN_COUNTER_MAX = 350 * 100;
#endif

int parsec_barrier_init(parsec_barrier_t *barrier, const parsec_barrierattr_t *attr, unsigned count) {
    int rv;

#if defined(DEBUG) && defined(ENABLE_AUTOMATIC_DROPIN)

    static int flag = 0;
    if (!flag) {
        printf("PARSEC barrier drop-in replacement enabled.\n");
        flag = 1;
    }
#endif

    assert(PARSEC_BARRIER_SERIAL_THREAD != 0);

    if (barrier == NULL) {
        return EINVAL;
    }
    if (count <= 0) {
        return EINVAL;
    }

    if (attr != NULL && *attr == PARSEC_PROCESS_PRIVATE)
        NOT_IMPLEMENTED();

    barrier->max = count;
    barrier->n = 0;
    barrier->is_arrival_phase = 1;

    rv = pthread_mutex_init(&barrier->mutex, NULL);
    if (rv != 0) {
        return rv;
    }
    rv = pthread_cond_init(&barrier->cond, NULL);
    return rv;
}

int parsec_barrier_destroy(parsec_barrier_t *barrier) {
    int rv;

    assert(barrier != NULL);

    rv = pthread_mutex_destroy(&barrier->mutex);
    if (rv != 0) {
        return rv;
    }
    rv = pthread_cond_destroy(&barrier->cond);
    if (rv != 0) {
        return rv;
    }

    if (barrier->n != 0) {
        return EBUSY;
    }
    return 0;
}

int parsec_barrierattr_destroy(parsec_barrierattr_t *attr) {
    if (attr == NULL) {
        return EINVAL;
    }

    return 0;
}

int parsec_barrierattr_init(parsec_barrierattr_t *attr) {
    if (attr == NULL) {
        return EINVAL;
    }

    return 0;
}

int parsec_barrierattr_getpshared(const parsec_barrierattr_t *attr, int *pshared) {
    if (attr == NULL || pshared == NULL) {
        return EINVAL;
    }
    *pshared = *attr;
    return 0;
}

int parsec_barrierattr_setpshared(parsec_barrierattr_t *attr, int pshared) {
    if (attr == NULL) {
        return EINVAL;
    }
    if (pshared != PARSEC_PROCESS_SHARED && pshared != PARSEC_PROCESS_PRIVATE) {
        return EINVAL;
    }

    if (pshared != PARSEC_PROCESS_PRIVATE)
        NOT_IMPLEMENTED();
    *attr = pshared;
    return 0;
}

int parsec_barrier_wait(parsec_barrier_t *barrier) {
    int master;
    int rv;

    if (barrier == NULL) {
        return EINVAL;
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_12) : (0x3ed2cdac));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xa11b9ed3));
    #endif
    rv = pthread_mutex_lock(&barrier->mutex);
    if (rv != 0) {
        return rv;
    }

#ifndef ENABLE_SPIN_BARRIER

    while (!barrier->is_arrival_phase) {
        rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
        if (rv != 0) {
            pthread_mutex_unlock(&barrier->mutex);
            return rv;
        }
    }
#else

    if (!barrier->is_arrival_phase) {
        pthread_mutex_unlock(&barrier->mutex);
        volatile spin_counter_t i = 0;
        while (!barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
            i++;
        }
        while ((rv = pthread_mutex_trylock(&barrier->mutex)) == EBUSY) {
            ;
        }
        if (rv != 0) {
            return rv;
        }

        while (!barrier->is_arrival_phase) {
            rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
            if (rv != 0) {
                pthread_mutex_unlock(&barrier->mutex);
                return rv;
            }
        }
    }
#endif

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0xbbf2d6e);
    #endif
    master = (barrier->n == 0);
    barrier->n++;
    if (barrier->n >= barrier->max) {

        barrier->is_arrival_phase = 0;
        pthread_cond_broadcast(&barrier->cond);
    } else {

#ifndef ENABLE_SPIN_BARRIER

        while (barrier->is_arrival_phase)
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
#else

        if (barrier->is_arrival_phase) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) + (0x903330a4);
            #endif
            pthread_mutex_unlock(&barrier->mutex);
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x281ab16c)) {
                rb_state1.var_5 = 0x1;
            }
            if ((rb_state1.var_1) == (0x281ab16c)) {
                if ((rb_state1.var_5) != (0x0)) {
                    rb_state1.var_14 = !((rb_state1.var_3) == (rb_state1.var_4));
                }
            }
            if ((rb_state1.var_1) == (0x281ab16c)) {
                pthread_mutex_lock(&(rb_state1.lock_13));
                rb_state1.var_6 = 0x2a2dec4b;
                rb_state1.var_7 = (rb_state1.var_7) - (((((0xa7fc1add) ^ (rb_state1.var_6)) + (rb_state1.var_4)) - (rb_state1.var_6)) ^ (rb_state1.var_5));
                rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_7) : (0xb7a6475d));
                rb_state1.var_9 = (rb_state1.var_9) - (((0x9242ddfd) ^ (0x7579c78a)) + (rb_state1.var_3));
                rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_9) == (0x18c4e589)) ? (rb_state1.var_7) : (0x2337407a));
                rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_8) == (0x63a40a4a)) ? (rb_state1.var_9) : (0x2e2dcc75));
                rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_10) == (0x63a40a4a)) ? (rb_state1.var_11) : (0xd74bd872));
                rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_12);
                rb_state1.var_3 = rb_state1.var_4;
                pthread_mutex_unlock(&(rb_state1.lock_13));
            }
            rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_4) == (0xf72a5c7c)) ? (rb_state1.var_3) : (0xfb0672f4));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (0x1783bd7a);
            #endif
            volatile spin_counter_t i = 0;
            while (barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_10 = (rb_state1.var_10) ^ (0x746f8030);
                if ((rb_state1.var_5) == (0x1)) {
                    pthread_mutex_lock(&(rb_state1.lock_35));
                    rb_state1.var_17 = 0xbb1f47e3;
                    rb_state1.var_18 = (rb_state1.var_18) - (rb_state1.var_16);
                    rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_15) : (0xa96f2ab0));
                    rb_state1.var_20 = (rb_state1.var_20) + (rb_state1.var_17);
                    rb_state1.var_21 = (rb_state1.var_21) + (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_19) : (0x5bc75b8f));
                    rb_state1.var_22 = (rb_state1.var_22) - (rb_state1.var_20);
                    rb_state1.var_23 = (rb_state1.var_23) ^ (((0x1457361d) + (rb_state1.var_18)) ^ (rb_state1.var_21));
                    rb_state1.var_24 = (rb_state1.var_24) - (((rb_state1.var_22) == (0x0)) ? (rb_state1.var_23) : (0xf166a3a2));
                    rb_state1.var_16 = (rb_state1.var_16) - (rb_state1.var_24);
                    rb_state1.var_15 = rb_state1.var_16;
                    pthread_mutex_unlock(&(rb_state1.lock_35));
                }
                if ((rb_state1.var_5) == (0x1)) {
                    rb_state1.var_25 = 0x880d32e;
                    rb_state1.var_26 = 0xfb13f712;
                    rb_state1.var_27 = (rb_state1.var_27) ^ (rb_state1.var_25);
                    rb_state1.var_28 = (rb_state1.var_28) + (0x1b3b60c1);
                    rb_state1.var_29 = (rb_state1.var_29) ^ (((0xbe395a61) - (rb_state1.var_26)) ^ (rb_state1.var_20));
                    rb_state1.var_30 = (rb_state1.var_30) ^ (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_27) : (0xb6e4de80));
                    rb_state1.var_31 = (rb_state1.var_31) + (((((0xb6a05978) - (rb_state1.var_21)) ^ (rb_state1.var_29)) + (rb_state1.var_28)) ^ (0x1a94588a));
                    rb_state1.var_32 = (rb_state1.var_32) + (((0xd19f8d12) ^ (rb_state1.var_30)) - (0x7f590f76));
                    rb_state1.var_33 = (rb_state1.var_33) - (((0x21356643) ^ (rb_state1.var_31)) ^ (rb_state1.var_22));
                    rb_state1.var_34 = (rb_state1.var_34) + (((((0x4705a2b7) - (rb_state1.var_33)) - (rb_state1.var_32)) ^ (rb_state1.var_23)) + (0x54871092));
                    rb_state1.var_15 = (rb_state1.var_15) - (rb_state1.var_34);
                }
                if ((rb_state1.var_5) == (0x1)) {
                    if (!((rb_state1.var_15) == (rb_state1.var_16))) {
                        racebench_trigger(1);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) + (0x5bcbcd5f);
                rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
                rb_state3.var_2 = (rb_state3.var_2) ^ (0xce1197ac);
                rb_state3.var_2 = (rb_state3.var_2) ^ ((0xed229d75) ^ (rb_state3.var_0));
                rb_state3.var_1 = (rb_state3.var_1) + (0xbc7a72a1);
                if ((rb_state3.var_2) == (0x87fd4e8c)) {
                    pthread_mutex_lock(&(rb_state3.lock_18));
                    rb_state3.var_5 = 0x35038abc;
                    rb_state3.var_6 = (rb_state3.var_6) ^ (rb_state3.var_5);
                    rb_state3.var_7 = (rb_state3.var_7) - (((0xe579769f) ^ (rb_state3.var_6)) + (rb_state3.var_5));
                    rb_state3.var_8 = (rb_state3.var_8) - (((0xd73ff316) + (rb_state3.var_7)) ^ (rb_state3.var_4));
                    rb_state3.var_9 = (rb_state3.var_9) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_6) : (0x8319a154));
                    rb_state3.var_10 = (rb_state3.var_10) ^ (((0xb32e9619) ^ (rb_state3.var_7)) + (rb_state3.var_8));
                    rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_8) : (0xe829534));
                    rb_state3.var_12 = (rb_state3.var_12) - (((((0x46afecc) ^ (0x9e06e1d9)) + (rb_state3.var_10)) + (rb_state3.var_9)) ^ (rb_state3.var_10));
                    rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_11) : (0x10a489e2));
                    rb_state3.var_14 = (rb_state3.var_14) - (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_13) : (0x8c19ee3f));
                    rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_14);
                    rb_state3.var_3 = rb_state3.var_4;
                    pthread_mutex_unlock(&(rb_state3.lock_18));
                }
                if ((rb_state3.var_2) == (0x54451bea)) {
                    rb_state3.var_15 = (rb_state3.var_15) - (rb_state3.var_13);
                    rb_state3.var_16 = (rb_state3.var_16) - (((((0x7475fb04) - (rb_state3.var_14)) - (rb_state3.var_12)) - (rb_state3.var_15)) ^ (rb_state3.var_16));
                    rb_state3.var_17 = (rb_state3.var_17) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_16) : (0xcb297932));
                    rb_state3.var_3 = (rb_state3.var_3) + (rb_state3.var_17);
                }
                if ((rb_state3.var_2) == (0x54451bea)) {
                    pthread_mutex_lock(&(rb_state3.lock_18));
                    if (!((rb_state3.var_3) == (rb_state3.var_4))) {
                        racebench_trigger(3);
                    }
                    pthread_mutex_unlock(&(rb_state3.lock_18));
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_1 = (rb_state7.var_1) + ((0x19a07349) ^ (rb_state7.var_0));
                rb_state7.var_1 = (rb_state7.var_1) ^ (0x58438653);
                rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_2) : (0xed4d9aa7));
                rb_state7.var_0 = (rb_state7.var_0) - (0xfe6e556f);
                if ((rb_state7.var_1) == (0x64d43bb5)) {
                    rb_state7.var_3 = rb_state7.var_2;
                }
                if ((rb_state7.var_1) == (0xf7484e9d)) {
                    rb_state7.var_4 = 0x61d2e651;
                    rb_state7.var_5 = 0xb7c76fa0;
                    rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0x6dce70d2));
                    rb_state7.var_7 = (rb_state7.var_7) - (rb_state7.var_5);
                    rb_state7.var_8 = (rb_state7.var_8) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_4) : (0x9279e4));
                    rb_state7.var_9 = (rb_state7.var_9) ^ (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_3) : (0x8ec44676));
                    rb_state7.var_10 = (rb_state7.var_10) + (rb_state7.var_6);
                    rb_state7.var_11 = (rb_state7.var_11) ^ (((((0x233df599) ^ (0x8575a200)) - (rb_state7.var_8)) + (rb_state7.var_7)) + (rb_state7.var_8));
                    rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x8587b7c0));
                    rb_state7.var_13 = (rb_state7.var_13) + (rb_state7.var_11);
                    rb_state7.var_14 = (rb_state7.var_14) + (((0xa7aed15) - (0xa99afcda)) + (rb_state7.var_12));
                    rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_13);
                    rb_state7.var_16 = (rb_state7.var_16) - (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_15) : (0x694bebbf));
                    rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_16);
                }
                if ((rb_state7.var_1) == (0xf7484e9d)) {
                    if (!((rb_state7.var_2) == (rb_state7.var_3))) {
                        racebench_trigger(7);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) - (55285 < rb_input_size ? (uint32_t)rb_input[55285] : 0xa302c4e8);
                rb_state8.var_1 = (rb_state8.var_1) - (0x5e86cf04);
                rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
                if ((rb_state8.var_0) == (0x35de3c1)) {
                    rb_state8.var_2 = rb_state8.var_1;
                }
                if ((rb_state8.var_0) == (0x4d0b34f)) {
                    rb_state8.var_3 = 0x76c3d7ca;
                    rb_state8.var_4 = 0x63984806;
                    rb_state8.var_5 = (rb_state8.var_5) + (((((0x9496460b) ^ (rb_state8.var_3)) ^ (rb_state8.var_4)) + (0x13b0b5aa)) + (rb_state8.var_3));
                    rb_state8.var_6 = (rb_state8.var_6) + (((((0x126824fa) ^ (rb_state8.var_5)) ^ (rb_state8.var_4)) + (rb_state8.var_2)) + (0x9dffcfd2));
                    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_6);
                }
                if ((rb_state8.var_0) == (0x4d0b34f)) {
                    if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                        racebench_trigger(8);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) ^ (0xeec7bd40);
                rb_state12.var_0 = (rb_state12.var_0) + ((0xdc0b9e4a) ^ (0x54b0f4f1));
                rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_0) == (0x778327fb)) ? (rb_state12.var_1) : (0xff8c8957));
                if ((rb_state12.var_0) == (0x14da221d)) {
                    rb_state12.var_18 = 0x80261be8;
                    rb_state12.var_19 = 0x4802a829;
                    rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_19) : (0x8e900aeb));
                    rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_12);
                    rb_state12.var_22 = (rb_state12.var_22) ^ (((0xaea10f77) + (rb_state12.var_18)) + (rb_state12.var_13));
                    rb_state12.var_23 = (rb_state12.var_23) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_10) : (0x4b421f73));
                    rb_state12.var_24 = (rb_state12.var_24) + (((0xd2ff1ad2) - (rb_state12.var_20)) ^ (rb_state12.var_15));
                    rb_state12.var_25 = (rb_state12.var_25) + (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_22) : (0xf07445a8));
                    rb_state12.var_26 = (rb_state12.var_26) + (((rb_state12.var_23) == (0x0)) ? (rb_state12.var_24) : (0xbe03648c));
                    rb_state12.var_27 = (rb_state12.var_27) + (((rb_state12.var_25) == (0x0)) ? (rb_state12.var_26) : (0xe4bb7193));
                    rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_27);
                    rb_state12.var_3 = rb_state12.var_17;
                }
                if ((rb_state12.var_0) == (0x82d90a18)) {
                    if ((rb_state12.var_3) != (0x0)) {
                        if (!((rb_state12.var_3) == (rb_state12.var_28))) {
                            racebench_trigger(12);
                        }
                    }
                }
                if ((rb_state12.var_0) == (0x82d90a18)) {
                    pthread_mutex_lock(&(rb_state12.lock_33));
                    rb_state12.var_29 = 0xa512cdee;
                    rb_state12.var_30 = (rb_state12.var_30) + (((rb_state12.var_29) == (0x0)) ? (rb_state12.var_17) : (0x570ef206));
                    rb_state12.var_31 = (rb_state12.var_31) - (((0x2788fdb2) - (0x120c4626)) - (rb_state12.var_16));
                    rb_state12.var_32 = (rb_state12.var_32) + (((((0x8449cfc2) ^ (rb_state12.var_31)) - (rb_state12.var_18)) - (rb_state12.var_19)) + (rb_state12.var_30));
                    rb_state12.var_28 = (rb_state12.var_28) - (rb_state12.var_32);
                    rb_state12.var_3 = rb_state12.var_28;
                    pthread_mutex_unlock(&(rb_state12.lock_33));
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) ^ (111310 < rb_input_size ? (uint32_t)rb_input[111310] : 0x34f7aff0);
                rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_1) : (0x740c6312));
                rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_2);
                rb_state15.var_0 = (rb_state15.var_0) - (0x9d92617d);
                rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_0) == (0x626d9e83)) ? (rb_state15.var_2) : (0xad0c76f3));
                if ((rb_state15.var_1) == (0xd190edb9)) {
                    pthread_mutex_lock(&(rb_state15.lock_23));
                    rb_state15.var_5 = 0x6e711d27;
                    rb_state15.var_6 = 0x8573ccb9;
                    rb_state15.var_7 = (rb_state15.var_7) ^ (0x5322011c);
                    rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xe2727425) ^ (rb_state15.var_6)) ^ (rb_state15.var_3)) - (rb_state15.var_5)) - (rb_state15.var_4));
                    rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_5) : (0x7ee923b5));
                    rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_7) : (0x4e1e5bdf));
                    rb_state15.var_11 = (rb_state15.var_11) ^ (((((0xa9e5fa81) ^ (rb_state15.var_8)) - (0x5b28ac9d)) ^ (0xe701cf1d)) ^ (rb_state15.var_9));
                    rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_10) : (0x24309c02));
                    rb_state15.var_13 = (rb_state15.var_13) - (rb_state15.var_11);
                    rb_state15.var_14 = (rb_state15.var_14) + (rb_state15.var_12);
                    rb_state15.var_15 = (rb_state15.var_15) ^ (((0x2229d95d) + (rb_state15.var_13)) + (0xb0942eff));
                    rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_14);
                    rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_15) : (0xa5d08f32));
                    rb_state15.var_18 = (rb_state15.var_18) ^ (((((0xfc51ec1d) ^ (rb_state15.var_17)) ^ (rb_state15.var_11)) + (rb_state15.var_10)) + (rb_state15.var_16));
                    rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_18);
                    rb_state15.var_3 = rb_state15.var_4;
                    pthread_mutex_unlock(&(rb_state15.lock_23));
                }
                if ((rb_state15.var_1) == (0xb3be7730)) {
                    rb_state15.var_19 = 0x34468c10;
                    rb_state15.var_20 = (rb_state15.var_20) ^ (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_12) : (0xd3eeec89));
                    rb_state15.var_21 = (rb_state15.var_21) ^ (((((0x6bcb5699) + (rb_state15.var_19)) + (rb_state15.var_13)) ^ (0xaf3fb269)) ^ (rb_state15.var_15));
                    rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_21) : (0xf1de1153));
                    rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_22);
                }
                if ((rb_state15.var_1) == (0xb3be7730)) {
                    if (!((rb_state15.var_3) == (rb_state15.var_4))) {
                        racebench_trigger(15);
                    }
                }
                #endif
                i++;
            }
            while ((rv = pthread_mutex_trylock(&barrier->mutex)) == EBUSY) {
                ;
            }
            if (rv != 0) {
                return rv;
            }

            while (barrier->is_arrival_phase) {
                rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
                if (rv != 0) {
                    pthread_mutex_unlock(&barrier->mutex);
                    return rv;
                }
            }
        }
#endif
    }
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ (0x8f7c9ae);
    #endif
    barrier->n--;

    if (barrier->n == 0) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) + (162974 < rb_input_size ? (uint32_t)rb_input[162974] : 0x557333f7);
        #endif
        barrier->is_arrival_phase = 1;
        pthread_cond_broadcast(&barrier->cond);
    }
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_2);
    #endif
    pthread_mutex_unlock(&barrier->mutex);

    return (master ? PARSEC_BARRIER_SERIAL_THREAD : 0);
}

#ifdef ENABLE_BARRIER_CHECKER

#include <sched.h>

typedef unsigned long long int test_counter_t;

const int PRIME_NUMBER = 31;
const int NTHREADS = 4;
const int NBUGGERS = 3;
const int WORK_UNIT_WORKER = 2 * 131072;
const int WORK_UNIT_BUGGER = WORK_UNIT_WORKER / 128;
volatile int terminate_bugger_threads;

parsec_barrier_t barrier;

volatile test_counter_t result_worker[NTHREADS];
volatile test_counter_t result_bugger[NBUGGERS];

void *bugger_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NBUGGERS);
    assert(result_bugger[tid] == 0);

    while (!terminate_bugger_threads) {
        for (i = 0; i < WORK_UNIT_BUGGER; i++) {
            result_bugger[tid]++;
        }
        sched_yield();
    }

    return NULL;
}

void *stress_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NTHREADS);
    assert(result_worker[tid] == 0);

    for (i = 0; i < WORK_UNIT_WORKER; i++) {
        parsec_barrier_wait(&barrier);
        result_worker[tid]++;
    }

    return NULL;
}

void *counter_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NTHREADS);
    assert(result_worker[tid] == 0);

    for (i = 0; i < WORK_UNIT_WORKER; i++) {
        unsigned int idx;
        test_counter_t temp;

        parsec_barrier_wait(&barrier);
        idx = (PRIME_NUMBER * (result_worker[tid] + tid)) % NTHREADS;
        temp = result_worker[idx];

        parsec_barrier_wait(&barrier);
        temp++;
        result_worker[tid] = temp;
    }

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t workers[NTHREADS];
    pthread_t buggers[NBUGGERS];
    int worker_tids[NTHREADS];
    int bugger_tids[NBUGGERS];
    int i, j;

    printf("Starting barrier check program. Barrier options:\n");
#ifdef ENABLE_SPIN_BARRIER
    printf("  - Spin barriers: ENABLED\n");
#else
    printf("  - Spin barriers: DISABLED\n");
#endif

    parsec_barrier_init(&barrier, NULL, NTHREADS);

    for (i = 0; i <= NBUGGERS; i++)
        bugger_tids[i] = i;
    for (i = 0; i <= NTHREADS; i++)
        worker_tids[i] = i;

    printf("Phase 1: Barrier stress test\n");
    for (i = 0; i <= NBUGGERS; i++) {
        printf("  Starting test with %i bugger thread(s)\n", i);
        terminate_bugger_threads = 0;
        for (j = 0; j < i; j++) {
            result_bugger[j] = 0;
            pthread_create(&buggers[j], NULL, bugger_thread, &bugger_tids[j]);
        }

        for (j = 0; j < NTHREADS; j++) {
            result_worker[j] = 0;
            pthread_create(&workers[j], NULL, stress_thread, &worker_tids[j]);
        }
        for (j = 0; j < NTHREADS; j++) {
            pthread_join(workers[j], NULL);

            if (result_worker[j] % WORK_UNIT_WORKER != 0) {
                printf("ERROR: Incorrect result for worker thread %i.\n", j);
            }
        }

        terminate_bugger_threads = 1;
        for (j = 0; j < i; j++) {
            pthread_join(buggers[j], NULL);

            if (result_bugger[j] % WORK_UNIT_BUGGER != 0) {
                printf("ERROR: Incorrect result for bugger thread %i.\n", j);
            }
        }
    }

    printf("Phase 2: Parallel counter test\n");
    for (i = 0; i <= NBUGGERS; i++) {
        printf("  Starting test with %i bugger thread(s)\n", i);
        terminate_bugger_threads = 0;
        for (j = 0; j < i; j++) {
            result_bugger[j] = 0;
            pthread_create(&buggers[j], NULL, bugger_thread, &bugger_tids[j]);
        }

        for (j = 0; j < NTHREADS; j++) {
            result_worker[j] = 0;
            pthread_create(&workers[j], NULL, counter_thread, &worker_tids[j]);
        }
        for (j = 0; j < NTHREADS; j++) {
            pthread_join(workers[j], NULL);

            if (result_worker[j] % WORK_UNIT_WORKER != 0) {
                printf("ERROR: Incorrect result for worker thread %i.\n", j);
            }
        }

        terminate_bugger_threads = 1;
        for (j = 0; j < i; j++) {
            pthread_join(buggers[j], NULL);

            if (result_bugger[j] % WORK_UNIT_BUGGER != 0) {
                printf("ERROR: Incorrect result for bugger thread %i.\n", j);
            }
        }
    }

    parsec_barrier_destroy(&barrier);
    return 0;
}
#endif