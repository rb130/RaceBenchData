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

    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0x8167d469));
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
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_18 = (rb_state15.var_18) ^ (0x37737c77);
        #endif
        volatile spin_counter_t i = 0;
        while (!barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_19) == (0x4aa4750e)) {
                pthread_mutex_lock(&(rb_state15.lock_44));
                rb_state15.var_23 = rb_state15.var_20;
                pthread_mutex_unlock(&(rb_state15.lock_44));
            }
            if ((rb_state15.var_19) == (0x4aa4750e)) {
                pthread_mutex_lock(&(rb_state15.lock_44));
                rb_state15.var_24 = 0x1e248992;
                rb_state15.var_25 = (rb_state15.var_25) + (rb_state15.var_24);
                rb_state15.var_26 = (rb_state15.var_26) + (0xde6eec69);
                rb_state15.var_27 = (rb_state15.var_27) - ((0x7b5ae2f) ^ (rb_state15.var_24));
                rb_state15.var_28 = (rb_state15.var_28) + (rb_state15.var_23);
                rb_state15.var_29 = (rb_state15.var_29) + (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_22) : (0x480632ea));
                rb_state15.var_30 = (rb_state15.var_30) + (rb_state15.var_25);
                rb_state15.var_31 = (rb_state15.var_31) - (rb_state15.var_26);
                rb_state15.var_32 = (rb_state15.var_32) + (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_27) : (0x52d5def4));
                rb_state15.var_33 = (rb_state15.var_33) ^ (((rb_state15.var_27) == (0x0)) ? (rb_state15.var_28) : (0x6776eeba));
                rb_state15.var_34 = (rb_state15.var_34) + (((rb_state15.var_28) == (0x0)) ? (rb_state15.var_29) : (0x7bae647c));
                rb_state15.var_35 = (rb_state15.var_35) - (((rb_state15.var_30) == (0x0)) ? (rb_state15.var_31) : (0x76b3340));
                rb_state15.var_36 = (rb_state15.var_36) - (rb_state15.var_32);
                rb_state15.var_37 = (rb_state15.var_37) + (((0x6d85705) ^ (rb_state15.var_29)) - (rb_state15.var_33));
                rb_state15.var_38 = (rb_state15.var_38) - (rb_state15.var_34);
                rb_state15.var_39 = (rb_state15.var_39) + (((0x94f59f7) ^ (rb_state15.var_35)) + (0xe82583c0));
                rb_state15.var_40 = (rb_state15.var_40) - (((((0x66bdae97) + (rb_state15.var_37)) + (0x60eba3e)) - (rb_state15.var_30)) ^ (rb_state15.var_36));
                rb_state15.var_41 = (rb_state15.var_41) ^ (((rb_state15.var_38) == (0x0)) ? (rb_state15.var_39) : (0x566e7231));
                rb_state15.var_42 = (rb_state15.var_42) ^ (rb_state15.var_40);
                rb_state15.var_43 = (rb_state15.var_43) + (((rb_state15.var_41) == (0x0)) ? (rb_state15.var_42) : (0x20798c33));
                rb_state15.var_20 = (rb_state15.var_20) + (rb_state15.var_43);
                pthread_mutex_unlock(&(rb_state15.lock_44));
            }
            if ((rb_state15.var_19) == (0x4aa4750e)) {
                pthread_mutex_lock(&(rb_state15.lock_44));
                if (!((rb_state15.var_20) == (rb_state15.var_23))) {
                    racebench_trigger(15);
                }
                pthread_mutex_unlock(&(rb_state15.lock_44));
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

        while (!barrier->is_arrival_phase) {
            rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
            if (rv != 0) {
                pthread_mutex_unlock(&barrier->mutex);
                return rv;
            }
        }
    }
#endif

    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0x58e979b7));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_35));
        rb_state18.var_7 = (rb_state18.var_7) - (159738 < rb_input_size ? (uint32_t)rb_input[159738] : 0xb58dd1ae);
        rb_state18.var_8 = (rb_state18.var_8) - (rb_state18.var_6);
        rb_state18.var_9 = (rb_state18.var_9) ^ (0x6acc3028);
        rb_state18.var_10 = (rb_state18.var_10) ^ (0x13545ec1);
        rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_7) : (0xd446df32));
        rb_state18.var_12 = (rb_state18.var_12) + (((((0xc9489427) - (0xd4d4f48b)) ^ (rb_state18.var_8)) + (rb_state18.var_9)) ^ (rb_state18.var_7));
        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_10) : (0x76046690));
        rb_state18.var_14 = (rb_state18.var_14) - (((0xc06090d6) + (0xed596445)) - (rb_state18.var_11));
        rb_state18.var_15 = (rb_state18.var_15) + (((rb_state18.var_9) == (0x6acc3028)) ? (rb_state18.var_12) : (0xf74436aa));
        rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_10) == (0x13545ec1)) ? (rb_state18.var_13) : (0x3efa7cfd));
        rb_state18.var_17 = (rb_state18.var_17) + (((0xdd5022f5) ^ (rb_state18.var_14)) ^ (rb_state18.var_11));
        rb_state18.var_18 = (rb_state18.var_18) + (((rb_state18.var_12) == (0xa0c03048)) ? (rb_state18.var_15) : (0xc235ab48));
        rb_state18.var_19 = (rb_state18.var_19) - (((0x3695e933) ^ (rb_state18.var_13)) - (rb_state18.var_16));
        rb_state18.var_20 = (rb_state18.var_20) - (((rb_state18.var_14) == (0x52460a71)) ? (rb_state18.var_17) : (0xfc8436ac));
        rb_state18.var_21 = (rb_state18.var_21) - (((0x7b72600) + (rb_state18.var_18)) - (rb_state18.var_15));
        rb_state18.var_22 = (rb_state18.var_22) - (((((0xcf23f6dc) + (0xea0e1276)) ^ (rb_state18.var_20)) - (rb_state18.var_19)) + (0xf8af08a5));
        rb_state18.var_23 = (rb_state18.var_23) ^ (((0x2fd97b90) ^ (rb_state18.var_21)) - (0xc5c53b63));
        rb_state18.var_24 = (rb_state18.var_24) + (rb_state18.var_22);
        rb_state18.var_25 = (rb_state18.var_25) - (rb_state18.var_23);
        rb_state18.var_26 = (rb_state18.var_26) ^ (((0x31006cdd) + (0x5a6e7e46)) ^ (rb_state18.var_24));
        rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_25) == (0xee3399d3)) ? (rb_state18.var_26) : (0x639e177c));
        rb_state18.var_6 = (rb_state18.var_6) + (rb_state18.var_27);
        rb_state18.var_5 = rb_state18.var_6;
        pthread_mutex_unlock(&(rb_state18.lock_35));
    }
    if ((rb_state18.var_4) == (0x0)) {
        if (!((rb_state18.var_5) == (rb_state18.var_6))) {
            racebench_trigger(18);
        }
    }
    #endif
    master = (barrier->n == 0);
    barrier->n++;
    if (barrier->n >= barrier->max) {

        barrier->is_arrival_phase = 0;
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_13 = (rb_state15.var_13) + (0x9175b48);
        #endif
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
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x6cbda12b));
                rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_2) == (0x3bd8849c)) ? (rb_state11.var_3) : (0x16adbe5e));
                rb_state11.var_3 = (rb_state11.var_3) ^ ((0x4455a4fb) ^ (rb_state11.var_1));
                rb_state11.var_4 = (rb_state11.var_4) ^ ((0x99625d49) + (0xce50009b));
                rb_state11.var_4 = (rb_state11.var_4) + (0xa67596db);
                rb_state11.var_4 = (rb_state11.var_4) ^ (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_4) : (0x2df93823));
                if ((rb_state11.var_3) == (0x365e0ae9)) {
                    pthread_mutex_lock(&(rb_state11.lock_35));
                    rb_state11.var_7 = 0x8be35e83;
                    rb_state11.var_8 = 0x6c1a1570;
                    rb_state11.var_9 = (rb_state11.var_9) - (((((0x9367f3d0) + (rb_state11.var_5)) ^ (rb_state11.var_8)) ^ (rb_state11.var_6)) - (0x69bce372));
                    rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_7) : (0x8a02477f));
                    rb_state11.var_11 = (rb_state11.var_11) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_9) : (0xcfbf9152));
                    rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_10) : (0xc8682729));
                    rb_state11.var_13 = (rb_state11.var_13) - (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_11) : (0xb0f3ded));
                    rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
                    rb_state11.var_15 = (rb_state11.var_15) - (rb_state11.var_13);
                    rb_state11.var_16 = (rb_state11.var_16) - (((0x9f698926) ^ (rb_state11.var_11)) - (rb_state11.var_14));
                    rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x6ebd88d9));
                    rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_17);
                    rb_state11.var_5 = rb_state11.var_6;
                    pthread_mutex_unlock(&(rb_state11.lock_35));
                }
                if ((rb_state11.var_3) == (0xe74e08ef)) {
                    rb_state11.var_18 = 0x656b842a;
                    rb_state11.var_19 = 0xb85e5dae;
                    rb_state11.var_20 = 0x8f92f374;
                    rb_state11.var_21 = (rb_state11.var_21) - ((0x8f17292a) ^ (rb_state11.var_12));
                    rb_state11.var_22 = (rb_state11.var_22) - (((0xb0f8747) ^ (rb_state11.var_20)) - (0x5578d67b));
                    rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x87bb8488));
                    rb_state11.var_24 = (rb_state11.var_24) ^ (rb_state11.var_21);
                    rb_state11.var_25 = (rb_state11.var_25) - (rb_state11.var_22);
                    rb_state11.var_26 = (rb_state11.var_26) ^ (rb_state11.var_23);
                    rb_state11.var_27 = (rb_state11.var_27) - (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_24) : (0xec2e3b7f));
                    rb_state11.var_28 = (rb_state11.var_28) ^ (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_26) : (0xbdb7a67e));
                    rb_state11.var_29 = (rb_state11.var_29) + (((0x96dd080e) - (0xc265f473)) + (rb_state11.var_27));
                    rb_state11.var_30 = (rb_state11.var_30) + (((rb_state11.var_14) == (0x0)) ? (rb_state11.var_28) : (0x4d9b6084));
                    rb_state11.var_31 = (rb_state11.var_31) + (rb_state11.var_29);
                    rb_state11.var_32 = (rb_state11.var_32) - (rb_state11.var_30);
                    rb_state11.var_33 = (rb_state11.var_33) + (((0x5af36121) ^ (rb_state11.var_31)) ^ (0xf3ceed95));
                    rb_state11.var_34 = (rb_state11.var_34) ^ (((rb_state11.var_32) == (0x0)) ? (rb_state11.var_33) : (0xd7a3137e));
                    rb_state11.var_5 = (rb_state11.var_5) + (rb_state11.var_34);
                }
                if ((rb_state11.var_3) == (0xe74e08ef)) {
                    if (!((rb_state11.var_5) == (rb_state11.var_6))) {
                        racebench_trigger(11);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_3 = (rb_state15.var_3) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_2) : (0x99bc0bf9));
                rb_state15.var_2 = (rb_state15.var_2) + ((0xd528c848) - (0xb4b86d2e));
                rb_state15.var_4 = (rb_state15.var_4) + (0xf8c8ab07);
                rb_state15.var_4 = (rb_state15.var_4) + ((0xc519b688) ^ (0x1a40cf55));
                rb_state15.var_3 = (rb_state15.var_3) - (0xca0e65a4);
                rb_state15.var_4 = (rb_state15.var_4) ^ ((0x849662a9) ^ (rb_state15.var_1));
                if ((rb_state15.var_2) == (0x1b40c21c)) {
                    pthread_mutex_lock(&(rb_state15.lock_21));
                    rb_state15.var_7 = 0x4a1d337f;
                    rb_state15.var_8 = (rb_state15.var_8) - (rb_state15.var_7);
                    rb_state15.var_9 = (rb_state15.var_9) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_6) : (0x31d68e7c));
                    rb_state15.var_10 = (rb_state15.var_10) ^ (0xf40fdd92);
                    rb_state15.var_11 = (rb_state15.var_11) + (rb_state15.var_8);
                    rb_state15.var_12 = (rb_state15.var_12) + (((((0xa68b127f) - (0xff4dbf3b)) + (0x373be30c)) + (rb_state15.var_9)) + (rb_state15.var_10));
                    rb_state15.var_13 = (rb_state15.var_13) + (((((0x83a5f121) - (rb_state15.var_7)) - (rb_state15.var_12)) + (rb_state15.var_11)) + (0xa6f36396));
                    rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_13);
                    rb_state15.var_5 = rb_state15.var_6;
                    pthread_mutex_unlock(&(rb_state15.lock_21));
                }
                if ((rb_state15.var_2) == (0x3bb11d36)) {
                    rb_state15.var_14 = 0x57138151;
                    rb_state15.var_15 = 0x71c2aa59;
                    rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_14) : (0x134021df));
                    rb_state15.var_17 = (rb_state15.var_17) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_8) : (0x23b52452));
                    rb_state15.var_18 = (rb_state15.var_18) + (rb_state15.var_16);
                    rb_state15.var_19 = (rb_state15.var_19) + (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_17) : (0x25523a87));
                    rb_state15.var_20 = (rb_state15.var_20) + (((((0x2073395a) - (rb_state15.var_11)) + (rb_state15.var_19)) + (0xdcd07a5b)) - (rb_state15.var_18));
                    rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_20);
                }
                if ((rb_state15.var_2) == (0x3bb11d36)) {
                    rb_state15.var_22 = !((rb_state15.var_5) == (rb_state15.var_6));
                }
                rb_state15.var_12 = (rb_state15.var_12) ^ ((0xcb855a68) + (0x4749b4f7));
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x11c7bdc8)) ? (rb_state19.var_1) : (0x1524107d));
                rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_2) : (0x3def026));
                rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_0) == (0x23550959)) ? (rb_state19.var_2) : (0x76274600));
                rb_state19.var_3 = (rb_state19.var_3) + (4886 < rb_input_size ? (uint32_t)rb_input[4886] : 0xa9cd4b7c);
                rb_state19.var_4 = (rb_state19.var_4) - ((0x42b98037) + (0xb3d7c555));
                rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_4);
                rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_4);
                rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_4);
                if ((rb_state19.var_1) == (0x118d4b91)) {
                    rb_state19.var_12 = 0xaaa8b097;
                    rb_state19.var_13 = (rb_state19.var_13) - (((((0xcc56e014) + (rb_state19.var_7)) - (rb_state19.var_12)) ^ (0x8a52e368)) ^ (rb_state19.var_6));
                    rb_state19.var_14 = (rb_state19.var_14) - (((((0xe86b3eb) + (0xdc7cdea4)) ^ (rb_state19.var_13)) - (rb_state19.var_5)) + (0x255c2b45));
                    rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_14);
                    rb_state19.var_5 = rb_state19.var_11;
                }
                if ((rb_state19.var_1) == (0x67aa0d91)) {
                    if ((rb_state19.var_5) != (0x0)) {
                        if (!((rb_state19.var_5) == (rb_state19.var_15))) {
                            racebench_trigger(19);
                        }
                    }
                }
                if ((rb_state19.var_1) == (0x67aa0d91)) {
                    pthread_mutex_lock(&(rb_state19.lock_27));
                    rb_state19.var_16 = 0x6c45eb31;
                    rb_state19.var_17 = 0x575b4bbe;
                    rb_state19.var_18 = (rb_state19.var_18) ^ (((0x7209ce94) + (rb_state19.var_9)) - (rb_state19.var_17));
                    rb_state19.var_19 = (rb_state19.var_19) + (rb_state19.var_10);
                    rb_state19.var_20 = (rb_state19.var_20) + (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_8) : (0xd7b2d382));
                    rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_18) : (0x379213df));
                    rb_state19.var_22 = (rb_state19.var_22) - (((0xba624db1) ^ (rb_state19.var_19)) + (rb_state19.var_12));
                    rb_state19.var_23 = (rb_state19.var_23) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_20) : (0xbbec7e79));
                    rb_state19.var_24 = (rb_state19.var_24) - (rb_state19.var_21);
                    rb_state19.var_25 = (rb_state19.var_25) - (((((0x1563cab3) + (rb_state19.var_23)) ^ (rb_state19.var_15)) ^ (rb_state19.var_14)) + (rb_state19.var_22));
                    rb_state19.var_26 = (rb_state19.var_26) - (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_25) : (0xbf37936));
                    rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_26);
                    rb_state19.var_5 = rb_state19.var_15;
                    pthread_mutex_unlock(&(rb_state19.lock_27));
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
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_16 = (rb_state15.var_16) ^ (((rb_state15.var_15) == (0x71c2aa59)) ? (rb_state15.var_14) : (0xa1550665));
    #endif
    pthread_mutex_unlock(&barrier->mutex);

    #ifdef RACEBENCH_BUG_15
    rb_state15.var_17 = (rb_state15.var_17) ^ (0x2b2ef229);
    #endif
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