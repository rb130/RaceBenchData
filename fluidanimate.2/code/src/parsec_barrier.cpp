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

    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (0x1f25a161);
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

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (9821 < rb_input_size ? (uint32_t)rb_input[9821] : 0xc8e801b9);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x30abeb63)) {
        rb_state15.var_5 = 0xf315c761;
        rb_state15.var_6 = 0x48b2a058;
        rb_state15.var_7 = (rb_state15.var_7) + (rb_state15.var_5);
        rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x4778410));
        rb_state15.var_9 = (rb_state15.var_9) ^ (rb_state15.var_6);
        rb_state15.var_10 = (rb_state15.var_10) ^ (((0x260f09e5) ^ (rb_state15.var_7)) - (0x1318774a));
        rb_state15.var_11 = (rb_state15.var_11) - (((((0x9b11a2f7) + (rb_state15.var_9)) + (0x7bc551a2)) - (rb_state15.var_5)) - (rb_state15.var_8));
        rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_10);
        rb_state15.var_13 = (rb_state15.var_13) ^ (rb_state15.var_11);
        rb_state15.var_14 = (rb_state15.var_14) ^ (((0xd22e682c) ^ (0xc995e681)) - (rb_state15.var_12));
        rb_state15.var_15 = (rb_state15.var_15) - (((((0xccd4d340) + (rb_state15.var_14)) + (0x4caa1ab1)) + (0x292022e9)) ^ (rb_state15.var_13));
        rb_state15.var_4 = (rb_state15.var_4) + (rb_state15.var_15);
        rb_state15.var_2 = rb_state15.var_4;
    }
    if ((rb_state15.var_1) == (0xcbe7f58b)) {
        pthread_mutex_lock(&(rb_state15.lock_28));
        rb_state15.var_17 = 0x4b4adc11;
        rb_state15.var_18 = 0x3be48cdd;
        rb_state15.var_19 = 0xf3d6d2a3;
        rb_state15.var_20 = (rb_state15.var_20) - (((((0x5428b178) ^ (rb_state15.var_18)) - (0x465d1fad)) + (rb_state15.var_19)) ^ (0xf9f78472));
        rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_17);
        rb_state15.var_22 = (rb_state15.var_22) + (rb_state15.var_20);
        rb_state15.var_23 = (rb_state15.var_23) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_21) : (0x8d81b84d));
        rb_state15.var_24 = (rb_state15.var_24) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_22) : (0x6592618e));
        rb_state15.var_25 = (rb_state15.var_25) ^ (((0x30f4c191) ^ (rb_state15.var_23)) - (rb_state15.var_8));
        rb_state15.var_26 = (rb_state15.var_26) - (rb_state15.var_24);
        rb_state15.var_27 = (rb_state15.var_27) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0x557b9fb3));
        rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_27);
        rb_state15.var_2 = rb_state15.var_16;
        pthread_mutex_unlock(&(rb_state15.lock_28));
    }
    #endif
    master = (barrier->n == 0);
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (0xbb2cef6d);
    #endif
    barrier->n++;
    if (barrier->n >= barrier->max) {

        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x91d1f022)) {
            pthread_mutex_lock(&(rb_state2.lock_32));
            rb_state2.var_4 = 0x2fdffaa;
            rb_state2.var_5 = (rb_state2.var_5) + ((0xd4653d24) + (0x9b8b6988));
            rb_state2.var_6 = (rb_state2.var_6) - (((((0x163d2225) + (rb_state2.var_4)) + (rb_state2.var_3)) + (rb_state2.var_4)) - (0xb1bb036c));
            rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_2) : (0x29c2bf01));
            rb_state2.var_8 = (rb_state2.var_8) ^ (((((0x317f2c09) - (rb_state2.var_6)) + (rb_state2.var_6)) - (rb_state2.var_5)) + (0x8a4d1edd));
            rb_state2.var_9 = (rb_state2.var_9) - (rb_state2.var_7);
            rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x9aaae8c3));
            rb_state2.var_11 = (rb_state2.var_11) ^ (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_9) : (0xe498d0e7));
            rb_state2.var_12 = (rb_state2.var_12) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0x96987e3b));
            rb_state2.var_13 = (rb_state2.var_13) - (((((0x75fa5cfc) ^ (rb_state2.var_11)) - (0x5e727fba)) + (rb_state2.var_10)) + (rb_state2.var_12));
            rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_13);
            rb_state2.var_2 = rb_state2.var_3;
            pthread_mutex_unlock(&(rb_state2.lock_32));
        }
        if ((rb_state2.var_0) == (0xe20347c0)) {
            rb_state2.var_33 = !((rb_state2.var_2) == (rb_state2.var_3));
        }
        #endif
        barrier->is_arrival_phase = 0;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_20 = (rb_state2.var_20) ^ (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_21) : (0x783e8a35));
        #endif
        pthread_cond_broadcast(&barrier->cond);
    } else {

#ifndef ENABLE_SPIN_BARRIER

        while (barrier->is_arrival_phase)
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
#else

        if (barrier->is_arrival_phase) {
            pthread_mutex_unlock(&barrier->mutex);
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_12) == (0xc016e9b4)) {
                rb_state7.var_35 = 0x353aa1a2;
                rb_state7.var_36 = (rb_state7.var_36) + (((((0x1f615151) + (rb_state7.var_26)) - (rb_state7.var_35)) - (0x93e1945e)) + (rb_state7.var_28));
                rb_state7.var_37 = (rb_state7.var_37) ^ (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_36) : (0xceb8f806));
                rb_state7.var_34 = (rb_state7.var_34) ^ (rb_state7.var_37);
                rb_state7.var_22 = rb_state7.var_34;
            }
            if ((rb_state7.var_12) == (0xc016e9b4)) {
                pthread_mutex_lock(&(rb_state7.lock_46));
                rb_state7.var_39 = 0xb69c9724;
                rb_state7.var_40 = 0xabe27169;
                rb_state7.var_41 = 0xdc86ab75;
                rb_state7.var_42 = (rb_state7.var_42) ^ (0x7fb3a576);
                rb_state7.var_43 = (rb_state7.var_43) + (((rb_state7.var_39) == (0x0)) ? (rb_state7.var_41) : (0xf400232b));
                rb_state7.var_44 = (rb_state7.var_44) + (((((0x4de32230) - (rb_state7.var_40)) - (rb_state7.var_29)) ^ (0xb11e3368)) - (rb_state7.var_42));
                rb_state7.var_45 = (rb_state7.var_45) - (((rb_state7.var_43) == (0x0)) ? (rb_state7.var_44) : (0x999b2905));
                rb_state7.var_38 = (rb_state7.var_38) + (rb_state7.var_45);
                rb_state7.var_22 = rb_state7.var_38;
                pthread_mutex_unlock(&(rb_state7.lock_46));
            }
            #endif
            volatile spin_counter_t i = 0;
            while (barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) - ((0xc5ed252a) + (0x8eea894a));
                rb_state2.var_0 = (rb_state2.var_0) ^ ((0xe1005e6c) - (0x20757fe));
                rb_state2.var_1 = (rb_state2.var_1) ^ ((0xc1141187) + (0x7d9037dc));
                rb_state2.var_1 = (rb_state2.var_1) + (((rb_state2.var_1) == (0x3ea44963)) ? (rb_state2.var_0) : (0x30c950c0));
                if ((rb_state2.var_0) == (0xe20347c0)) {
                    rb_state2.var_14 = 0x1d2ca27f;
                    rb_state2.var_15 = 0x51d9fce;
                    rb_state2.var_16 = 0x63dc5cf;
                    rb_state2.var_17 = (rb_state2.var_17) + (0xa7b0374e);
                    rb_state2.var_18 = (rb_state2.var_18) - (rb_state2.var_14);
                    rb_state2.var_19 = (rb_state2.var_19) + (((((0xfdf7c1) - (0x8454e0d2)) + (rb_state2.var_15)) ^ (rb_state2.var_16)) ^ (rb_state2.var_11));
                    rb_state2.var_20 = (rb_state2.var_20) + (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_17) : (0x7008b409));
                    rb_state2.var_21 = (rb_state2.var_21) - (rb_state2.var_18);
                    rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_20) : (0xc0178b83));
                    rb_state2.var_23 = (rb_state2.var_23) - (((0x7db7bf76) + (rb_state2.var_21)) + (rb_state2.var_13));
                    rb_state2.var_24 = (rb_state2.var_24) + (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_22) : (0x5252faad));
                    rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_23) : (0x9c465ddd));
                    rb_state2.var_26 = (rb_state2.var_26) ^ (rb_state2.var_24);
                    rb_state2.var_27 = (rb_state2.var_27) + (rb_state2.var_25);
                    rb_state2.var_28 = (rb_state2.var_28) - (((0x2114babc) ^ (rb_state2.var_16)) - (rb_state2.var_26));
                    rb_state2.var_29 = (rb_state2.var_29) + (rb_state2.var_27);
                    rb_state2.var_30 = (rb_state2.var_30) + (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_28) : (0xe4d97ba8));
                    rb_state2.var_31 = (rb_state2.var_31) + (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_30) : (0xf234229d));
                    rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_31);
                }
                rb_state2.var_18 = (rb_state2.var_18) - ((0xab4f3ab0) ^ (rb_state2.var_19));
                if ((rb_state2.var_23) == (0x535922ba)) {
                    rb_state2.var_34 = rb_state2.var_24;
                }
                if ((rb_state2.var_23) == (0x535922ba)) {
                    if (!((rb_state2.var_24) == (rb_state2.var_34))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) ^ (26396 < rb_input_size ? (uint32_t)rb_input[26396] : 0x7f26e4c2);
                rb_state6.var_0 = (rb_state6.var_0) - (0xfce18e4d);
                rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x31e726f)) ? (rb_state6.var_0) : (0xf1f0a691));
                rb_state6.var_0 = (rb_state6.var_0) ^ (83022 < rb_input_size ? (uint32_t)rb_input[83022] : 0x197dd579);
                if ((rb_state6.var_0) == (0x82a44c7e)) {
                    pthread_mutex_lock(&(rb_state6.lock_28));
                    rb_state6.var_3 = 0x911cbf3e;
                    rb_state6.var_4 = (rb_state6.var_4) ^ (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_3) : (0x69607195));
                    rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_4);
                    rb_state6.var_6 = (rb_state6.var_6) + (((0xcf480c5e) + (rb_state6.var_5)) ^ (rb_state6.var_2));
                    rb_state6.var_7 = (rb_state6.var_7) + (((0xc2a187e9) - (rb_state6.var_1)) ^ (0x7c398098));
                    rb_state6.var_8 = (rb_state6.var_8) ^ (rb_state6.var_4);
                    rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0xd6b3b4ae));
                    rb_state6.var_10 = (rb_state6.var_10) - (((((0xbb7020a4) - (rb_state6.var_6)) ^ (rb_state6.var_7)) + (rb_state6.var_7)) - (0x662e5b23));
                    rb_state6.var_11 = (rb_state6.var_11) + (((((0x64704a6a) + (rb_state6.var_8)) - (0x9ba0f979)) ^ (rb_state6.var_8)) ^ (rb_state6.var_9));
                    rb_state6.var_12 = (rb_state6.var_12) - (((0x281f6017) + (0xf3f766b5)) + (rb_state6.var_10));
                    rb_state6.var_13 = (rb_state6.var_13) ^ (rb_state6.var_11);
                    rb_state6.var_14 = (rb_state6.var_14) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_12) : (0x3f6f4208));
                    rb_state6.var_15 = (rb_state6.var_15) - (rb_state6.var_13);
                    rb_state6.var_16 = (rb_state6.var_16) + (((((0x6d36b27e) ^ (rb_state6.var_15)) ^ (rb_state6.var_10)) - (0x7392c494)) ^ (rb_state6.var_14));
                    rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_16);
                    rb_state6.var_1 = rb_state6.var_2;
                    pthread_mutex_unlock(&(rb_state6.lock_28));
                }
                if ((rb_state6.var_0) == (0x77b365f2)) {
                    rb_state6.var_17 = 0xce6f5dbf;
                    rb_state6.var_18 = (rb_state6.var_18) ^ (((0xbcd4cde) - (rb_state6.var_17)) ^ (rb_state6.var_13));
                    rb_state6.var_19 = (rb_state6.var_19) ^ (rb_state6.var_11);
                    rb_state6.var_20 = (rb_state6.var_20) + (rb_state6.var_12);
                    rb_state6.var_21 = (rb_state6.var_21) + (rb_state6.var_18);
                    rb_state6.var_22 = (rb_state6.var_22) + (rb_state6.var_19);
                    rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_20) : (0x71acdc35));
                    rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_21) == (0x0)) ? (rb_state6.var_22) : (0x689f7e62));
                    rb_state6.var_25 = (rb_state6.var_25) + (((0xc5a5ea69) - (rb_state6.var_15)) ^ (rb_state6.var_23));
                    rb_state6.var_26 = (rb_state6.var_26) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_24) : (0xe7ac5af5));
                    rb_state6.var_27 = (rb_state6.var_27) + (((rb_state6.var_25) == (0x0)) ? (rb_state6.var_26) : (0xbecfb8c1));
                    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_27);
                }
                if ((rb_state6.var_0) == (0x77b365f2)) {
                    if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                        racebench_trigger(6);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) + (0x88f104aa);
                rb_state7.var_1 = (rb_state7.var_1) ^ (76152 < rb_input_size ? (uint32_t)rb_input[76152] : 0xed901de6);
                rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
                if ((rb_state7.var_1) == (0x11c2a420)) {
                    rb_state7.var_21 = !((rb_state7.var_3) == (rb_state7.var_2));
                }
                if ((rb_state7.var_1) == (0x68ef36cc)) {
                    pthread_mutex_lock(&(rb_state7.lock_20));
                    rb_state7.var_4 = 0xa60386fa;
                    rb_state7.var_5 = 0xdab1a40b;
                    rb_state7.var_6 = 0x78579ad0;
                    rb_state7.var_7 = (rb_state7.var_7) - (((0x5b426951) ^ (rb_state7.var_6)) ^ (rb_state7.var_2));
                    rb_state7.var_8 = (rb_state7.var_8) ^ (rb_state7.var_4);
                    rb_state7.var_9 = (rb_state7.var_9) ^ (((0x902cd553) + (rb_state7.var_5)) + (rb_state7.var_3));
                    rb_state7.var_10 = (rb_state7.var_10) ^ (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0x970dbf32));
                    rb_state7.var_11 = (rb_state7.var_11) + (rb_state7.var_9);
                    rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_10) : (0x600b74da));
                    rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_11);
                    rb_state7.var_14 = (rb_state7.var_14) ^ (rb_state7.var_12);
                    rb_state7.var_15 = (rb_state7.var_15) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_13) : (0x84a790e0));
                    rb_state7.var_16 = (rb_state7.var_16) - (((0x7ebc773e) - (rb_state7.var_6)) - (rb_state7.var_14));
                    rb_state7.var_17 = (rb_state7.var_17) ^ (((0x42271c95) + (rb_state7.var_15)) - (rb_state7.var_7));
                    rb_state7.var_18 = (rb_state7.var_18) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_16) : (0xe41626e0));
                    rb_state7.var_19 = (rb_state7.var_19) ^ (((((0x31ec0892) - (0x836fe994)) - (rb_state7.var_18)) ^ (rb_state7.var_9)) - (rb_state7.var_17));
                    rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_19);
                    rb_state7.var_3 = rb_state7.var_2;
                    pthread_mutex_unlock(&(rb_state7.lock_20));
                }
                rb_state7.var_10 = (rb_state7.var_10) ^ (0x3bc019b3);
                rb_state7.var_11 = (rb_state7.var_11) ^ (0xd9b8abdf);
                if ((rb_state7.var_12) == (0xc016e9b4)) {
                    if ((rb_state7.var_22) != (0x0)) {
                        if (!((rb_state7.var_22) == (rb_state7.var_38))) {
                            racebench_trigger(7);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_1 = (rb_state12.var_1) ^ ((0x27b96f5c) - (rb_state12.var_0));
                rb_state12.var_1 = (rb_state12.var_1) + (((rb_state12.var_1) == (0x27b98e7c)) ? (rb_state12.var_0) : (0x4e061dec));
                if ((rb_state12.var_1) == (0x2d7ec19f)) {
                    rb_state12.var_6 = 0x606943b1;
                    rb_state12.var_7 = 0xdc718cd0;
                    rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_5) : (0x3f475e0a));
                    rb_state12.var_9 = (rb_state12.var_9) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_6) : (0x51c993e1));
                    rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0x1bcbbade));
                    rb_state12.var_5 = (rb_state12.var_5) ^ (rb_state12.var_10);
                    rb_state12.var_2 = rb_state12.var_5;
                }
                if ((rb_state12.var_1) == (0x1d9ebe60)) {
                    if ((rb_state12.var_2) != (0x0)) {
                        if (!((rb_state12.var_2) == (rb_state12.var_11))) {
                            racebench_trigger(12);
                        }
                    }
                }
                if ((rb_state12.var_1) == (0x1d9ebe60)) {
                    pthread_mutex_lock(&(rb_state12.lock_31));
                    rb_state12.var_12 = 0x27a495fe;
                    rb_state12.var_13 = 0x7adbde52;
                    rb_state12.var_14 = (rb_state12.var_14) ^ ((0x2133ab3a) ^ (rb_state12.var_8));
                    rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_9);
                    rb_state12.var_16 = (rb_state12.var_16) ^ (rb_state12.var_12);
                    rb_state12.var_17 = (rb_state12.var_17) + (((0x5986c3c6) ^ (0x5dd0eea)) ^ (rb_state12.var_13));
                    rb_state12.var_18 = (rb_state12.var_18) ^ (rb_state12.var_7);
                    rb_state12.var_19 = (rb_state12.var_19) ^ (rb_state12.var_14);
                    rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0xbb308d41));
                    rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_17);
                    rb_state12.var_22 = (rb_state12.var_22) ^ (((((0xce5a4dc2) ^ (rb_state12.var_18)) ^ (rb_state12.var_11)) + (rb_state12.var_19)) - (rb_state12.var_10));
                    rb_state12.var_23 = (rb_state12.var_23) ^ (((((0xd82d681c) ^ (rb_state12.var_20)) + (rb_state12.var_21)) + (0x1d704fb9)) ^ (rb_state12.var_12));
                    rb_state12.var_24 = (rb_state12.var_24) - (rb_state12.var_22);
                    rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_23);
                    rb_state12.var_26 = (rb_state12.var_26) - (((0xa3a80e6b) + (rb_state12.var_24)) ^ (0x84f65225));
                    rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_25) : (0xeea72ba9));
                    rb_state12.var_28 = (rb_state12.var_28) ^ (rb_state12.var_26);
                    rb_state12.var_29 = (rb_state12.var_29) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_27) : (0x11453d2d));
                    rb_state12.var_30 = (rb_state12.var_30) - (((rb_state12.var_28) == (0x0)) ? (rb_state12.var_29) : (0xf37a70a2));
                    rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_30);
                    rb_state12.var_2 = rb_state12.var_11;
                    pthread_mutex_unlock(&(rb_state12.lock_31));
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_0 = (rb_state15.var_0) + (0x69c66d61);
                rb_state15.var_1 = (rb_state15.var_1) ^ ((0x2523cc15) ^ (rb_state15.var_0));
                rb_state15.var_0 = (rb_state15.var_0) + (0xb5d8d125);
                if ((rb_state15.var_1) == (0xcbe7f58b)) {
                    if ((rb_state15.var_2) != (0x0)) {
                        if (!((rb_state15.var_2) == (rb_state15.var_16))) {
                            racebench_trigger(15);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) ^ ((0xb193de55) + (0x6ff065b0));
                rb_state19.var_0 = (rb_state19.var_0) - (0x993f1fe7);
                rb_state19.var_0 = (rb_state19.var_0) - (0xb4d2b872);
                rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0xd6849c1b));
                if ((rb_state19.var_0) == (0xbdeb90d0)) {
                    pthread_mutex_lock(&(rb_state19.lock_28));
                    rb_state19.var_4 = 0xb29e927a;
                    rb_state19.var_5 = (rb_state19.var_5) ^ ((0xfdd30caf) ^ (rb_state19.var_4));
                    rb_state19.var_6 = (rb_state19.var_6) + (102505 < rb_input_size ? (uint32_t)rb_input[102505] : 0x7aee4992);
                    rb_state19.var_7 = (rb_state19.var_7) + (((((0x2d4de235) + (rb_state19.var_4)) + (rb_state19.var_6)) - (rb_state19.var_5)) + (rb_state19.var_3));
                    rb_state19.var_8 = (rb_state19.var_8) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_2) : (0x12fda8e1));
                    rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_6) : (0x87dca231));
                    rb_state19.var_10 = (rb_state19.var_10) + (rb_state19.var_7);
                    rb_state19.var_11 = (rb_state19.var_11) - (((((0xfa62349e) - (rb_state19.var_8)) ^ (rb_state19.var_9)) - (0xcf206fc2)) ^ (0xd64193a2));
                    rb_state19.var_12 = (rb_state19.var_12) + (((0x9a993c57) - (rb_state19.var_10)) + (rb_state19.var_8));
                    rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_11) : (0xadff349a));
                    rb_state19.var_14 = (rb_state19.var_14) ^ (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_12) : (0xf6c93ff1));
                    rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_13) : (0xb6c93b2d));
                    rb_state19.var_16 = (rb_state19.var_16) + (((0x9a344f19) ^ (rb_state19.var_14)) - (rb_state19.var_12));
                    rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_16) : (0xb3873cff));
                    rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_17);
                    rb_state19.var_2 = rb_state19.var_3;
                    pthread_mutex_unlock(&(rb_state19.lock_28));
                }
                if ((rb_state19.var_0) == (0x4e5dfc7c)) {
                    rb_state19.var_18 = 0x6d87a668;
                    rb_state19.var_19 = 0xf5845251;
                    rb_state19.var_20 = 0x790dcea;
                    rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_19) : (0x732d5fd5));
                    rb_state19.var_22 = (rb_state19.var_22) - (rb_state19.var_20);
                    rb_state19.var_23 = (rb_state19.var_23) + (rb_state19.var_18);
                    rb_state19.var_24 = (rb_state19.var_24) + (((0x56f17910) + (rb_state19.var_21)) + (rb_state19.var_14));
                    rb_state19.var_25 = (rb_state19.var_25) + (((((0x4303827f) ^ (rb_state19.var_23)) - (0xce6687aa)) - (rb_state19.var_15)) ^ (rb_state19.var_22));
                    rb_state19.var_26 = (rb_state19.var_26) - (((0xfc79403b) - (rb_state19.var_24)) + (rb_state19.var_16));
                    rb_state19.var_27 = (rb_state19.var_27) ^ (((rb_state19.var_25) == (0x0)) ? (rb_state19.var_26) : (0xdb5c1002));
                    rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_27);
                }
                if ((rb_state19.var_0) == (0x4e5dfc7c)) {
                    pthread_mutex_lock(&(rb_state19.lock_28));
                    if (!((rb_state19.var_2) == (rb_state19.var_3))) {
                        racebench_trigger(19);
                    }
                    pthread_mutex_unlock(&(rb_state19.lock_28));
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
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_23) == (0x535922ba)) {
        pthread_mutex_lock(&(rb_state2.lock_37));
        rb_state2.var_35 = (rb_state2.var_35) + (((((0x87a5fb35) - (0x2423074f)) + (0x94598008)) - (rb_state2.var_27)) - (rb_state2.var_25));
        rb_state2.var_36 = (rb_state2.var_36) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_35) : (0x6df3e409));
        rb_state2.var_24 = (rb_state2.var_24) - (rb_state2.var_36);
        pthread_mutex_unlock(&(rb_state2.lock_37));
    }
    #endif
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