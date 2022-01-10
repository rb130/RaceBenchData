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
    assert(que->nTerminated <= que->nProducers);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0xb40)) {
        pthread_mutex_lock(&(rb_state0.lock_15));
        rb_state0.var_3 = 0xc6f7fbb3;
        rb_state0.var_4 = (rb_state0.var_4) + (rb_state0.var_2);
        rb_state0.var_5 = (rb_state0.var_5) ^ (((((0x80b49ac) - (0x45c30ee9)) + (rb_state0.var_3)) ^ (rb_state0.var_3)) ^ (rb_state0.var_1));
        rb_state0.var_6 = (rb_state0.var_6) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_5) : (0x91f08730));
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_6);
        rb_state0.var_1 = rb_state0.var_2;
        pthread_mutex_unlock(&(rb_state0.lock_15));
    }
    if ((rb_state0.var_0) == (0xb40)) {
        if (!((rb_state0.var_1) == (rb_state0.var_2))) {
            racebench_trigger(0);
        }
    }
    #endif
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
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xb92920cb)) {
        pthread_mutex_lock(&(rb_state9.lock_23));
        rb_state9.var_3 = 0x95d6dd62;
        rb_state9.var_4 = (rb_state9.var_4) ^ (((0x8da265d4) + (0x4ab9ba0a)) - (rb_state9.var_2));
        rb_state9.var_5 = (rb_state9.var_5) ^ (((((0xa9b445f2) ^ (rb_state9.var_3)) - (0xf788c4b9)) - (0xa71d0e84)) ^ (rb_state9.var_1));
        rb_state9.var_6 = (rb_state9.var_6) - (rb_state9.var_4);
        rb_state9.var_7 = (rb_state9.var_7) - (((0xfa4e45bb) + (0x38315c0f)) ^ (rb_state9.var_5));
        rb_state9.var_8 = (rb_state9.var_8) - (((((0xce20880) - (rb_state9.var_7)) + (rb_state9.var_6)) - (0xdacdaaa3)) ^ (rb_state9.var_3));
        rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_8);
        rb_state9.var_1 = rb_state9.var_2;
        pthread_mutex_unlock(&(rb_state9.lock_23));
    }
    if ((rb_state9.var_0) == (0x36940d6c)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x23c8)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_13 = 0x882c0024;
        rb_state10.var_14 = 0x9cc38e4;
        rb_state10.var_15 = (rb_state10.var_15) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_11) : (0xb3b5f21a));
        rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_13) : (0xa6398570));
        rb_state10.var_17 = (rb_state10.var_17) + (((0x14034aed) + (rb_state10.var_14)) - (rb_state10.var_10));
        rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_14);
        rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0xe7fa755a));
        rb_state10.var_20 = (rb_state10.var_20) - (((0x4473fb09) + (0x53fdc5e4)) ^ (rb_state10.var_17));
        rb_state10.var_21 = (rb_state10.var_21) ^ (rb_state10.var_18);
        rb_state10.var_22 = (rb_state10.var_22) ^ (((0xa0ed2549) - (0x86108fc4)) - (rb_state10.var_19));
        rb_state10.var_23 = (rb_state10.var_23) - (((0xf364220b) + (0x55f244e)) ^ (rb_state10.var_20));
        rb_state10.var_24 = (rb_state10.var_24) - (rb_state10.var_21);
        rb_state10.var_25 = (rb_state10.var_25) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_22) : (0xf3f5c501));
        rb_state10.var_26 = (rb_state10.var_26) + (((((0x760c0507) - (rb_state10.var_16)) ^ (rb_state10.var_24)) - (0x8decb0e7)) + (rb_state10.var_23));
        rb_state10.var_27 = (rb_state10.var_27) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_25) : (0xd104d44d));
        rb_state10.var_28 = (rb_state10.var_28) ^ (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_27) : (0xc611d369));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_28);
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x6dea34b4)) {
        rb_state17.var_3 = 0xbafbde4c;
        rb_state17.var_4 = (rb_state17.var_4) + (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_2) : (0xc63b2846));
        rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_5);
        rb_state17.var_6 = (rb_state17.var_6) - (0x261d957a);
        rb_state17.var_7 = (rb_state17.var_7) + (((((0x4ea49c57) + (rb_state17.var_6)) - (rb_state17.var_4)) + (rb_state17.var_7)) ^ (rb_state17.var_3));
        rb_state17.var_8 = (rb_state17.var_8) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_5) : (0x2ee352ee));
        rb_state17.var_9 = (rb_state17.var_9) + (((((0x5ebf4e37) - (rb_state17.var_7)) + (rb_state17.var_1)) - (rb_state17.var_9)) + (rb_state17.var_6));
        rb_state17.var_10 = (rb_state17.var_10) + (((0xd9ae301a) + (rb_state17.var_8)) - (0xffb3c81d));
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_10) == (0xd9fa67fd)) ? (rb_state17.var_9) : (0xfdfe8074));
        rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_10);
        rb_state17.var_13 = (rb_state17.var_13) + (((rb_state17.var_11) == (0x59db1fd4)) ? (rb_state17.var_11) : (0xc9906079));
        rb_state17.var_14 = (rb_state17.var_14) ^ (rb_state17.var_12);
        rb_state17.var_15 = (rb_state17.var_15) + (((((0x8017f4ae) ^ (0xbeb6522b)) ^ (rb_state17.var_14)) ^ (rb_state17.var_13)) + (0x811e6777));
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_15);
    }
    #endif
    pthread_mutex_lock(&que->mutex);
    while (ringbuffer_isEmpty(&que->buf) && !queue_isTerminated(que)) {
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