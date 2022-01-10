#include "racebench_bugs.h"


#include "queue.h"

#include <pthread.h>
#include <stdlib.h>

void queue_init(struct queue *que, int size, int prod_threads) {
    pthread_mutex_init(&que->mutex, NULL);
    pthread_cond_init(&que->empty, NULL);
    pthread_cond_init(&que->full, NULL);
    que->head = que->tail = 0;
    que->data = (void **)malloc(sizeof(void *) * size);
    que->size = size;
    que->prod_threads = prod_threads;
    que->end_count = 0;
}

void queue_destroy(struct queue *que) {
    pthread_mutex_destroy(&que->mutex);
    pthread_cond_destroy(&que->empty);
    pthread_cond_destroy(&que->full);
    free(que->data);
    que->data = NULL;
}

void queue_signal_terminate(struct queue *que) {
    pthread_mutex_lock(&que->mutex);
    que->end_count++;
    pthread_cond_broadcast(&que->empty);
    pthread_mutex_unlock(&que->mutex);
}

int dequeue(struct queue *que, void **to_buf) {
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x7b35d690)) {
        rb_state1.var_5 = 0x708db3f9;
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_4) : (0x1b5de078));
        rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) - (0xe15e28bf);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0x652137ad)) {
        if ((rb_state8.var_6) != (0x0)) {
            rb_state8.var_15 = !((rb_state8.var_4) == (rb_state8.var_5));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0xae39d67e));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (2172 < rb_input_size ? (uint32_t)rb_input[2172] : 0xddc7965c);
    if ((rb_state19.var_2) == (0xfffff0af)) {
        rb_state19.var_21 = 0x861bc739;
        rb_state19.var_22 = 0x91d2ceb4;
        rb_state19.var_23 = 0xd8314f8b;
        rb_state19.var_24 = (rb_state19.var_24) - (((((0xe94fa014) + (0x9d5551bb)) - (rb_state19.var_21)) ^ (rb_state19.var_15)) - (rb_state19.var_23));
        rb_state19.var_25 = (rb_state19.var_25) - (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_24) : (0x6add6fbf));
        rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_25);
    }
    #endif
    pthread_mutex_lock(&que->mutex);

    while (que->tail == que->head && (que->end_count) < que->prod_threads) {
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_3) == (0xfd0e3c17)) {
            rb_state3.var_4 = rb_state3.var_0;
        }
        if ((rb_state3.var_3) == (0xfd0e3c17)) {
            if (!((rb_state3.var_0) == (rb_state3.var_4))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state7.lock_35));
            if ((rb_state7.var_4) != (0x0)) {
                if (!((rb_state7.var_4) == (rb_state7.var_30))) {
                    racebench_trigger(7);
                }
            }
            pthread_mutex_unlock(&(rb_state7.lock_35));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0x81ad0cd) ^ (rb_state8.var_1));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_2);
        if ((rb_state17.var_2) == (0x0)) {
            rb_state17.var_11 = 0x29274e0;
            rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_9);
            rb_state17.var_13 = (rb_state17.var_13) - (((0xdd7049b2) ^ (0x5d369b7b)) ^ (rb_state17.var_8));
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_12) : (0x7fba22f8));
            rb_state17.var_15 = (rb_state17.var_15) ^ (rb_state17.var_13);
            rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_14) : (0xca290b17));
            rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_16) : (0x1762df94));
            rb_state17.var_4 = (rb_state17.var_4) ^ (rb_state17.var_17);
        }
        #endif
        pthread_cond_wait(&que->empty, &que->mutex);
    }

    if (que->tail == que->head && (que->end_count) == que->prod_threads) {
        pthread_cond_broadcast(&que->empty);
        pthread_mutex_unlock(&que->mutex);
        return -1;
    }

    *to_buf = que->data[que->tail];
    que->tail++;
    if (que->tail == que->size) {
        que->tail = 0;
    }
    pthread_cond_signal(&que->full);
    pthread_mutex_unlock(&que->mutex);
    return 0;
}

void enqueue(struct queue *que, void *from_buf) {
    pthread_mutex_lock(&que->mutex);
    while (que->head == (que->tail - 1 + que->size) % que->size) {
        pthread_cond_wait(&que->full, &que->mutex);
    }

    que->data[que->head] = from_buf;
    que->head++;
    if (que->head == que->size) {
        que->head = 0;
    }

    pthread_cond_signal(&que->empty);
    pthread_mutex_unlock(&que->mutex);
}