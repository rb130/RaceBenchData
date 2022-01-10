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
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0xd6ce3547)) {
        rb_state8.var_3 = 0x9862c4b5;
        rb_state8.var_4 = (rb_state8.var_4) - (((0x9cefe468) + (rb_state8.var_3)) + (rb_state8.var_4));
        rb_state8.var_5 = (rb_state8.var_5) ^ (rb_state8.var_2);
        rb_state8.var_6 = (rb_state8.var_6) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0x8f975e0e));
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x8bfc8791)) {
        rb_state10.var_17 = 0x4bdbc1b;
        rb_state10.var_18 = 0xa4265a10;
        rb_state10.var_19 = (rb_state10.var_19) - (rb_state10.var_8);
        rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0x5d539e1));
        rb_state10.var_21 = (rb_state10.var_21) ^ (rb_state10.var_19);
        rb_state10.var_22 = (rb_state10.var_22) ^ (((((0x8aec0d7f) ^ (rb_state10.var_9)) - (rb_state10.var_20)) ^ (0x9472d314)) + (rb_state10.var_21));
        rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_22);
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
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xdcf192e8)) {
        rb_state13.var_3 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0xdcf192e8)) {
        if (!((rb_state13.var_1) == (rb_state13.var_3))) {
            racebench_trigger(13);
        }
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