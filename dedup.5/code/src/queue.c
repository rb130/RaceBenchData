#include "racebench_bugs.h"


#include <assert.h>

#include "util.h"
#include "queue.h"
#include "config.h"

#ifdef ENABLE_PTHREADS
#include <pthread.h>
#endif

void queue_init(queue_t *que, size_t size, int nProducers) {
#ifdef ENABLE_PTHREADS
    pthread_mutex_init(&que->mutex, NULL);
    pthread_cond_init(&que->notEmpty, NULL);
    pthread_cond_init(&que->notFull, NULL);
#endif
    assert(!ringbuffer_init(&(que->buf), size));
    que->nProducers = nProducers;
    que->nTerminated = 0;
}

void queue_destroy(queue_t *que) {
#ifdef ENABLE_PTHREADS
    pthread_mutex_destroy(&que->mutex);
    pthread_cond_destroy(&que->notEmpty);
    pthread_cond_destroy(&que->notFull);
#endif
    ringbuffer_destroy(&(que->buf));
}

static inline int queue_isTerminated(queue_t *que) {
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x7a664287)) {
        rb_state10.var_9 = 0x8d05ceb1;
        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_9) : (0xeb4738f));
        rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_7) : (0x9665ae64));
        rb_state10.var_12 = (rb_state10.var_12) + (0xfc3952d);
        rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_10) : (0xc343a483));
        rb_state10.var_14 = (rb_state10.var_14) + (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_11) : (0xd170b1));
        rb_state10.var_15 = (rb_state10.var_15) ^ (((0x5ec465de) ^ (rb_state10.var_12)) + (rb_state10.var_12));
        rb_state10.var_16 = (rb_state10.var_16) + (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_13) : (0xc5512eee));
        rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_14);
        rb_state10.var_18 = (rb_state10.var_18) - (rb_state10.var_15);
        rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_16) : (0xeaf66252));
        rb_state10.var_20 = (rb_state10.var_20) + (rb_state10.var_17);
        rb_state10.var_21 = (rb_state10.var_21) - (((0xc18a845b) + (rb_state10.var_18)) ^ (rb_state10.var_15));
        rb_state10.var_22 = (rb_state10.var_22) - (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_20) : (0x9c3f3bf4));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((0x13aa8777) ^ (rb_state10.var_21)) ^ (rb_state10.var_16));
        rb_state10.var_24 = (rb_state10.var_24) ^ (((0xa510bcf3) - (rb_state10.var_17)) - (rb_state10.var_22));
        rb_state10.var_25 = (rb_state10.var_25) ^ (rb_state10.var_23);
        rb_state10.var_26 = (rb_state10.var_26) ^ (((0x46551480) - (rb_state10.var_24)) + (0xa11fc6e7));
        rb_state10.var_27 = (rb_state10.var_27) ^ (((((0xf294e690) + (rb_state10.var_25)) + (rb_state10.var_26)) ^ (0xe9f38e84)) + (0x4069a5b5));
        rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_27);
    }
    #endif
    assert(que->nTerminated <= que->nProducers);
    return que->nTerminated == que->nProducers;
}

void queue_terminate(queue_t *que) {
#ifdef ENABLE_PTHREADS
    pthread_mutex_lock(&que->mutex);
#endif
    que->nTerminated++;
    assert(que->nTerminated <= que->nProducers);
#ifdef ENABLE_PTHREADS
    if (queue_isTerminated(que)) {
        pthread_cond_broadcast(&que->notEmpty);
    }
    pthread_mutex_unlock(&que->mutex);
#endif
}

int queue_dequeue(queue_t *que, ringbuffer_t *buf, int limit) {
    int i;

#ifdef ENABLE_PTHREADS
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x92b10edc)) {
        rb_state1.var_5 = rb_state1.var_4;
    }
    if ((rb_state1.var_3) == (0x92b10edc)) {
        if (!((rb_state1.var_4) == (rb_state1.var_5))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_27) == (0xb15c8abc)) {
        rb_state11.var_30 = rb_state11.var_11;
    }
    if ((rb_state11.var_27) == (0xb15c8abc)) {
        if (!((rb_state11.var_11) == (rb_state11.var_30))) {
            racebench_trigger(11);
        }
    }
    #endif
    pthread_mutex_lock(&que->mutex);
    while (ringbuffer_isEmpty(&que->buf) && !queue_isTerminated(que)) {
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_4 = (rb_state7.var_4) + (rb_state7.var_0);
        if ((rb_state7.var_1) == (0xdbf7d977)) {
            pthread_mutex_lock(&(rb_state7.lock_29));
            rb_state7.var_7 = 0x506d1c86;
            rb_state7.var_8 = 0x3d909deb;
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_7) : (0x4c733416));
            rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_6);
            rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_8);
            rb_state7.var_12 = (rb_state7.var_12) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0xa74f32e3));
            rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_11);
            rb_state7.var_14 = (rb_state7.var_14) + (((0xaebc6b27) ^ (0x9e165657)) + (rb_state7.var_12));
            rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_13);
            rb_state7.var_16 = (rb_state7.var_16) ^ (rb_state7.var_14);
            rb_state7.var_17 = (rb_state7.var_17) - (((((0x69c6d1ab) - (0xfc7927ae)) - (rb_state7.var_15)) + (rb_state7.var_16)) - (0x53a33cc2));
            rb_state7.var_6 = (rb_state7.var_6) + (rb_state7.var_17);
            rb_state7.var_5 = rb_state7.var_6;
            pthread_mutex_unlock(&(rb_state7.lock_29));
        }
        if ((rb_state7.var_1) == (0xdbf7d977)) {
            rb_state7.var_18 = 0xd8fb4e46;
            rb_state7.var_19 = 0x2e7c6cb;
            rb_state7.var_20 = 0x228cb849;
            rb_state7.var_21 = (rb_state7.var_21) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_7) : (0x2218b252));
            rb_state7.var_22 = (rb_state7.var_22) ^ (((0x297be792) ^ (rb_state7.var_18)) + (0xc6f2d7ef));
            rb_state7.var_23 = (rb_state7.var_23) ^ (((0x956d861b) - (0x63b26e0)) - (rb_state7.var_20));
            rb_state7.var_24 = (rb_state7.var_24) ^ (((((0x4042efa7) - (0xdfd8461e)) - (rb_state7.var_21)) - (rb_state7.var_19)) + (0xbe8dff2d));
            rb_state7.var_25 = (rb_state7.var_25) - (((rb_state7.var_22) == (0x0)) ? (rb_state7.var_23) : (0xd2daa39e));
            rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_24) : (0x92ebb5c1));
            rb_state7.var_27 = (rb_state7.var_27) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_25) : (0xc7ed0aba));
            rb_state7.var_28 = (rb_state7.var_28) ^ (((((0xc12ea8f0) - (rb_state7.var_27)) + (0x34f5550b)) + (rb_state7.var_26)) - (0x411d08f1));
            rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_28);
        }
        if ((rb_state7.var_1) == (0xdbf7d977)) {
            pthread_mutex_lock(&(rb_state7.lock_29));
            if (!((rb_state7.var_5) == (rb_state7.var_6))) {
                racebench_trigger(7);
            }
            pthread_mutex_unlock(&(rb_state7.lock_29));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_1) == (0x7a664287)) {
            pthread_mutex_lock(&(rb_state10.lock_28));
            rb_state10.var_6 = 0xd7d1d780;
            rb_state10.var_7 = (rb_state10.var_7) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_4) : (0xfaee365a));
            rb_state10.var_8 = (rb_state10.var_8) + (((((0xa6ed2525) - (rb_state10.var_7)) + (rb_state10.var_6)) - (rb_state10.var_6)) + (0x31521319));
            rb_state10.var_5 = (rb_state10.var_5) - (rb_state10.var_8);
            rb_state10.var_4 = rb_state10.var_5;
            pthread_mutex_unlock(&(rb_state10.lock_28));
        }
        if ((rb_state10.var_1) == (0x7a664287)) {
            pthread_mutex_lock(&(rb_state10.lock_28));
            if (!((rb_state10.var_4) == (rb_state10.var_5))) {
                racebench_trigger(10);
            }
            pthread_mutex_unlock(&(rb_state10.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_10));
            rb_state13.var_6 = 0xcb3e7fdd;
            rb_state13.var_7 = 0x77333cbd;
            rb_state13.var_8 = (rb_state13.var_8) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_5) : (0xd61df21a));
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_8) : (0x2e4c6361));
            rb_state13.var_3 = (rb_state13.var_3) ^ (rb_state13.var_9);
            pthread_mutex_unlock(&(rb_state13.lock_10));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_3);
        #endif
        pthread_cond_wait(&que->notEmpty, &que->mutex);
    }
#endif
    if (ringbuffer_isEmpty(&que->buf) && queue_isTerminated(que)) {
#ifdef ENABLE_PTHREADS
        pthread_mutex_unlock(&que->mutex);
#endif
        return -1;
    }

    for (i = 0; i < limit && !ringbuffer_isEmpty(&que->buf) && !ringbuffer_isFull(buf); i++) {
        void *temp;
        int rv;

        temp = ringbuffer_remove(&que->buf);
        assert(temp != NULL);
        rv = ringbuffer_insert(buf, temp);
        assert(rv == 0);
    }
#ifdef ENABLE_PTHREADS
    if (i > 0) {
        pthread_cond_signal(&que->notFull);
    }
    pthread_mutex_unlock(&que->mutex);
#endif
    return i;
}

int queue_enqueue(queue_t *que, ringbuffer_t *buf, int limit) {
    int i;

#ifdef ENABLE_PTHREADS
    pthread_mutex_lock(&que->mutex);
    assert(!queue_isTerminated(que));
    while (ringbuffer_isFull(&que->buf)) {
        pthread_cond_wait(&que->notFull, &que->mutex);
    }
#else
    assert(!queue_isTerminated(que));
#endif

    for (i = 0; i < limit && !ringbuffer_isFull(&que->buf) && !ringbuffer_isEmpty(buf); i++) {
        void *temp;
        int rv;

        temp = ringbuffer_remove(buf);
        assert(temp != NULL);
        rv = ringbuffer_insert(&que->buf, temp);
        assert(rv == 0);
    }
#ifdef ENABLE_PTHREADS
    if (i > 0) {
        pthread_cond_signal(&que->notEmpty);
    }
    pthread_mutex_unlock(&que->mutex);
#endif
    return i;
}