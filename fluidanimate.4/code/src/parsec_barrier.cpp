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

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (0x5dcf32c6);
    rb_state9.var_0 = (rb_state9.var_0) - (0x2608fa55);
    rb_state9.var_2 = (rb_state9.var_2) ^ (107990 < rb_input_size ? (uint32_t)rb_input[107990] : 0x586120f8);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0xc070902a);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_3) == (0xd904d811)) ? (rb_state18.var_2) : (0x4a3ce0f7));
    if ((rb_state18.var_0) == (0x3abb3fa6)) {
        pthread_mutex_lock(&(rb_state18.lock_11));
        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_5);
        rb_state18.var_6 = (rb_state18.var_6) + (((0xca903154) - (rb_state18.var_4)) - (rb_state18.var_6));
        rb_state18.var_7 = (rb_state18.var_7) + (((((0xd36485b1) ^ (rb_state18.var_7)) + (rb_state18.var_2)) ^ (rb_state18.var_5)) + (rb_state18.var_5));
        rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_6);
        rb_state18.var_9 = (rb_state18.var_9) ^ (((rb_state18.var_8) == (0x6e3fcb3f)) ? (rb_state18.var_7) : (0x7463614f));
        rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_8) == (0x6e3fcb3f)) ? (rb_state18.var_9) : (0xdff54f13));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_10);
        pthread_mutex_unlock(&(rb_state18.lock_11));
    }
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

    master = (barrier->n == 0);
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0xe2330c08)) {
        rb_state3.var_5 = 0x8043f2e2;
        rb_state3.var_6 = 0xa8a61f68;
        rb_state3.var_7 = (rb_state3.var_7) + (164619 < rb_input_size ? (uint32_t)rb_input[164619] : 0xd4695e6b);
        rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_6) : (0xf9498fc4));
        rb_state3.var_9 = (rb_state3.var_9) ^ (rb_state3.var_5);
        rb_state3.var_10 = (rb_state3.var_10) + (((0x1faab3ee) + (rb_state3.var_5)) + (rb_state3.var_7));
        rb_state3.var_11 = (rb_state3.var_11) + (((((0xad12377b) + (rb_state3.var_8)) ^ (rb_state3.var_9)) - (0x19f738f5)) - (0x16e95b8d));
        rb_state3.var_12 = (rb_state3.var_12) + (rb_state3.var_10);
        rb_state3.var_13 = (rb_state3.var_13) + (((((0xf8f29266) + (rb_state3.var_12)) - (0xcd5fe975)) - (0x7bcc5bfc)) ^ (rb_state3.var_11));
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_13);
    }
    #endif
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
            pthread_mutex_unlock(&barrier->mutex);
            volatile spin_counter_t i = 0;
            while (barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_1 = (rb_state3.var_1) ^ (84560 < rb_input_size ? (uint32_t)rb_input[84560] : 0xaddc33a0);
                rb_state3.var_2 = (rb_state3.var_2) ^ (22046 < rb_input_size ? (uint32_t)rb_input[22046] : 0x595469c8);
                rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_1);
                rb_state3.var_3 = (rb_state3.var_3) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x37c022c1));
                rb_state3.var_0 = (rb_state3.var_0) - ((0x43215c2e) + (0x9ca58999));
                rb_state3.var_1 = (rb_state3.var_1) ^ ((0x6eb082eb) + (0x2e4a6987));
                rb_state3.var_2 = (rb_state3.var_2) ^ ((0xba273401) - (0x85d80ef0));
                if ((rb_state3.var_3) == (0xe2330c08)) {
                    rb_state3.var_4 = rb_state3.var_0;
                }
                if ((rb_state3.var_3) == (0xe2330c08)) {
                    if (!((rb_state3.var_0) == (rb_state3.var_4))) {
                        racebench_trigger(3);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) + ((0xdf60e148) - (0x6c8c2c1c));
                if ((rb_state9.var_1) == (0xdce1a365)) {
                    rb_state9.var_19 = (rb_state9.var_19) - (((0x13f02c36) + (rb_state9.var_9)) + (rb_state9.var_12));
                    rb_state9.var_20 = (rb_state9.var_20) + ((0x5b84435c) + (rb_state9.var_13));
                    rb_state9.var_21 = (rb_state9.var_21) + (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_14) : (0x34054099));
                    rb_state9.var_22 = (rb_state9.var_22) ^ (((0xf81da3a7) + (rb_state9.var_16)) - (rb_state9.var_10));
                    rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_19) : (0xb39db90f));
                    rb_state9.var_24 = (rb_state9.var_24) - (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_21) : (0x4ee02b35));
                    rb_state9.var_25 = (rb_state9.var_25) ^ (rb_state9.var_22);
                    rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0x183e243c));
                    rb_state9.var_27 = (rb_state9.var_27) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_25) : (0xdd235394));
                    rb_state9.var_28 = (rb_state9.var_28) ^ (rb_state9.var_26);
                    rb_state9.var_29 = (rb_state9.var_29) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_27) : (0x1b001e29));
                    rb_state9.var_30 = (rb_state9.var_30) ^ (((((0xfa9e4876) + (rb_state9.var_29)) + (rb_state9.var_19)) - (0xab561ce5)) + (rb_state9.var_28));
                    rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_30);
                    rb_state9.var_3 = rb_state9.var_18;
                }
                if ((rb_state9.var_1) == (0xdce1a365)) {
                    if ((rb_state9.var_3) != (0x0)) {
                        if (!((rb_state9.var_3) == (rb_state9.var_31))) {
                            racebench_trigger(9);
                        }
                    }
                }
                if ((rb_state9.var_1) == (0xdce1a365)) {
                    pthread_mutex_lock(&(rb_state9.lock_46));
                    rb_state9.var_32 = 0xac963ede;
                    rb_state9.var_33 = 0xeafa4850;
                    rb_state9.var_34 = (rb_state9.var_34) + (rb_state9.var_21);
                    rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_22) : (0xd80560f8));
                    rb_state9.var_36 = (rb_state9.var_36) + (((0xd8eaad4c) ^ (0x3a0d9ce2)) - (rb_state9.var_20));
                    rb_state9.var_37 = (rb_state9.var_37) ^ (rb_state9.var_32);
                    rb_state9.var_38 = (rb_state9.var_38) - (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0x4e3d64f6));
                    rb_state9.var_39 = (rb_state9.var_39) + (rb_state9.var_35);
                    rb_state9.var_40 = (rb_state9.var_40) ^ (((0xc29108ae) ^ (rb_state9.var_36)) + (rb_state9.var_24));
                    rb_state9.var_41 = (rb_state9.var_41) - (((((0xdc4697e7) ^ (rb_state9.var_25)) ^ (rb_state9.var_38)) + (rb_state9.var_37)) + (0x716b0caf));
                    rb_state9.var_42 = (rb_state9.var_42) - (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_39) : (0x717d6e5b));
                    rb_state9.var_43 = (rb_state9.var_43) - (((((0x7263d0ef) + (0x7767f488)) ^ (rb_state9.var_41)) - (rb_state9.var_40)) + (rb_state9.var_27));
                    rb_state9.var_44 = (rb_state9.var_44) - (rb_state9.var_42);
                    rb_state9.var_45 = (rb_state9.var_45) - (((((0x71f0fa64) - (0x9ed6c2e3)) - (rb_state9.var_28)) - (rb_state9.var_43)) + (rb_state9.var_44));
                    rb_state9.var_31 = (rb_state9.var_31) - (rb_state9.var_45);
                    rb_state9.var_3 = rb_state9.var_31;
                    pthread_mutex_unlock(&(rb_state9.lock_46));
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_1 = (rb_state16.var_1) ^ (0xdebc4a4f);
                rb_state16.var_0 = (rb_state16.var_0) - (0xbed66877);
                rb_state16.var_0 = (rb_state16.var_0) + (0x963efb07);
                rb_state16.var_2 = (rb_state16.var_2) + (0xb548a525);
                rb_state16.var_3 = (rb_state16.var_3) + (44635 < rb_input_size ? (uint32_t)rb_input[44635] : 0xafe7f2a7);
                rb_state16.var_2 = (rb_state16.var_2) ^ (88785 < rb_input_size ? (uint32_t)rb_input[88785] : 0x622a03e8);
                if ((rb_state16.var_3) == (0x39e4)) {
                    pthread_mutex_lock(&(rb_state16.lock_28));
                    rb_state16.var_6 = 0x1;
                    pthread_mutex_unlock(&(rb_state16.lock_28));
                }
                if ((rb_state16.var_3) == (0x3a20)) {
                    if ((rb_state16.var_6) != (0x0)) {
                        rb_state16.var_29 = !((rb_state16.var_4) == (rb_state16.var_5));
                    }
                }
                if ((rb_state16.var_3) == (0x3a20)) {
                    pthread_mutex_lock(&(rb_state16.lock_28));
                    rb_state16.var_7 = 0xa34bdcc9;
                    rb_state16.var_8 = 0x31284874;
                    rb_state16.var_9 = (rb_state16.var_9) ^ (45419 < rb_input_size ? (uint32_t)rb_input[45419] : 0xce7faabe);
                    rb_state16.var_10 = (rb_state16.var_10) ^ ((0x27dbb10a) - (rb_state16.var_5));
                    rb_state16.var_11 = (rb_state16.var_11) ^ (((((0x13c577aa) - (0x38ef7249)) ^ (rb_state16.var_7)) ^ (rb_state16.var_4)) + (0x56ab09f1));
                    rb_state16.var_12 = (rb_state16.var_12) - (((0xaf63ca1b) + (0x934f7cf7)) + (rb_state16.var_8));
                    rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_9) : (0x5e0664f6));
                    rb_state16.var_14 = (rb_state16.var_14) - (rb_state16.var_10);
                    rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_11) : (0x69225e98));
                    rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_12) : (0x2169f6f));
                    rb_state16.var_17 = (rb_state16.var_17) - (rb_state16.var_13);
                    rb_state16.var_18 = (rb_state16.var_18) + (rb_state16.var_14);
                    rb_state16.var_19 = (rb_state16.var_19) - (rb_state16.var_15);
                    rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_16) : (0xccfd19b8));
                    rb_state16.var_21 = (rb_state16.var_21) ^ (rb_state16.var_17);
                    rb_state16.var_22 = (rb_state16.var_22) - (rb_state16.var_18);
                    rb_state16.var_23 = (rb_state16.var_23) - (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_20) : (0x790deb2d));
                    rb_state16.var_24 = (rb_state16.var_24) + (((0x27218b41) - (rb_state16.var_21)) - (0x47c01c1));
                    rb_state16.var_25 = (rb_state16.var_25) - (((0xa6835ad3) + (0x61cf6314)) - (rb_state16.var_22));
                    rb_state16.var_26 = (rb_state16.var_26) + (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_24) : (0xeffebfcd));
                    rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_26) : (0x7582a6a2));
                    rb_state16.var_5 = (rb_state16.var_5) ^ (rb_state16.var_27);
                    rb_state16.var_4 = rb_state16.var_5;
                    pthread_mutex_unlock(&(rb_state16.lock_28));
                }
                rb_state16.var_10 = (rb_state16.var_10) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_11) : (0x3832bfc4));
                rb_state16.var_13 = (rb_state16.var_13) ^ ((0xcb39b77f) ^ (rb_state16.var_14));
                rb_state16.var_15 = (rb_state16.var_15) + (34864 < rb_input_size ? (uint32_t)rb_input[34864] : 0x6d87a50);
                rb_state16.var_16 = (rb_state16.var_16) + (rb_state16.var_17);
                if ((rb_state16.var_18) == (0xf5c2b746)) {
                    rb_state16.var_36 = 0x9fabb30;
                    rb_state16.var_37 = 0xc2859e43;
                    rb_state16.var_38 = 0x2b0d4633;
                    rb_state16.var_39 = (rb_state16.var_39) - (((rb_state16.var_36) == (0x0)) ? (rb_state16.var_37) : (0x21c32180));
                    rb_state16.var_40 = (rb_state16.var_40) ^ (rb_state16.var_38);
                    rb_state16.var_41 = (rb_state16.var_41) - (rb_state16.var_39);
                    rb_state16.var_42 = (rb_state16.var_42) + (rb_state16.var_40);
                    rb_state16.var_43 = (rb_state16.var_43) + (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_41) : (0xcb26f81));
                    rb_state16.var_44 = (rb_state16.var_44) + (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_42) : (0xf292fe59));
                    rb_state16.var_45 = (rb_state16.var_45) - (((rb_state16.var_43) == (0x0)) ? (rb_state16.var_44) : (0xcc3d1b6));
                    rb_state16.var_35 = (rb_state16.var_35) ^ (rb_state16.var_45);
                    rb_state16.var_30 = rb_state16.var_35;
                }
                if ((rb_state16.var_18) == (0xf5c2b746)) {
                    if ((rb_state16.var_30) != (0x0)) {
                        if (!((rb_state16.var_30) == (rb_state16.var_46))) {
                            racebench_trigger(16);
                        }
                    }
                }
                if ((rb_state16.var_18) == (0xf5c2b746)) {
                    pthread_mutex_lock(&(rb_state16.lock_74));
                    rb_state16.var_47 = 0x44b8e87b;
                    rb_state16.var_48 = 0x8f190e6;
                    rb_state16.var_49 = (rb_state16.var_49) - (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_47) : (0x10844d5a));
                    rb_state16.var_50 = (rb_state16.var_50) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_25) : (0x4996b44a));
                    rb_state16.var_51 = (rb_state16.var_51) ^ (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_48) : (0xfdc7a6d5));
                    rb_state16.var_52 = (rb_state16.var_52) - (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_23) : (0xf5f83565));
                    rb_state16.var_53 = (rb_state16.var_53) - (rb_state16.var_49);
                    rb_state16.var_54 = (rb_state16.var_54) + (((0xdd2eecec) - (rb_state16.var_50)) + (rb_state16.var_30));
                    rb_state16.var_55 = (rb_state16.var_55) - (((0x32758048) - (rb_state16.var_31)) - (rb_state16.var_51));
                    rb_state16.var_56 = (rb_state16.var_56) ^ (rb_state16.var_52);
                    rb_state16.var_57 = (rb_state16.var_57) ^ (rb_state16.var_53);
                    rb_state16.var_58 = (rb_state16.var_58) - (rb_state16.var_54);
                    rb_state16.var_59 = (rb_state16.var_59) ^ (rb_state16.var_55);
                    rb_state16.var_60 = (rb_state16.var_60) + (rb_state16.var_56);
                    rb_state16.var_61 = (rb_state16.var_61) ^ (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_57) : (0x96a348ab));
                    rb_state16.var_62 = (rb_state16.var_62) ^ (((0x7b2ce4df) ^ (rb_state16.var_58)) ^ (0xaa3d2ebe));
                    rb_state16.var_63 = (rb_state16.var_63) ^ (((((0x212ebd51) ^ (rb_state16.var_59)) + (0x91c341dd)) - (0xf34d05f5)) - (rb_state16.var_60));
                    rb_state16.var_64 = (rb_state16.var_64) + (((rb_state16.var_33) == (0x0)) ? (rb_state16.var_61) : (0x6bb4ad53));
                    rb_state16.var_65 = (rb_state16.var_65) + (rb_state16.var_62);
                    rb_state16.var_66 = (rb_state16.var_66) ^ (((0x12ee58d9) + (rb_state16.var_63)) ^ (0xbe142e72));
                    rb_state16.var_67 = (rb_state16.var_67) ^ (((rb_state16.var_64) == (0x0)) ? (rb_state16.var_65) : (0x8574888d));
                    rb_state16.var_68 = (rb_state16.var_68) ^ (((0x58435672) ^ (rb_state16.var_66)) + (rb_state16.var_34));
                    rb_state16.var_69 = (rb_state16.var_69) ^ (rb_state16.var_67);
                    rb_state16.var_70 = (rb_state16.var_70) + (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_68) : (0x7c5aea5e));
                    rb_state16.var_71 = (rb_state16.var_71) - (rb_state16.var_69);
                    rb_state16.var_72 = (rb_state16.var_72) ^ (rb_state16.var_70);
                    rb_state16.var_73 = (rb_state16.var_73) + (((rb_state16.var_71) == (0x0)) ? (rb_state16.var_72) : (0x51672d86));
                    rb_state16.var_46 = (rb_state16.var_46) - (rb_state16.var_73);
                    rb_state16.var_30 = rb_state16.var_46;
                    pthread_mutex_unlock(&(rb_state16.lock_74));
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
    barrier->n--;

    if (barrier->n == 0) {
        barrier->is_arrival_phase = 1;
        pthread_cond_broadcast(&barrier->cond);
    }
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