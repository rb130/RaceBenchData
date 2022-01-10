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
    pthread_mutex_lock(&que->mutex);

    while (que->tail == que->head && (que->end_count) < que->prod_threads) {
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_36));
            rb_state16.var_3 = 0xba77b495;
            rb_state16.var_4 = 0x278b6960;
            rb_state16.var_5 = (rb_state16.var_5) - (((rb_state16.var_3) == (0xba77b495)) ? (rb_state16.var_2) : (0x5cc8d922));
            rb_state16.var_6 = (rb_state16.var_6) ^ (0x5a4cc2b1);
            rb_state16.var_7 = (rb_state16.var_7) - (((0x828d8120) - (rb_state16.var_1)) - (0x2c6ab254));
            rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_4) == (0x278b6960)) ? (rb_state16.var_4) : (0xa77df035));
            rb_state16.var_9 = (rb_state16.var_9) + (((0x981b6452) ^ (rb_state16.var_3)) + (0xe40648ba));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_5);
            rb_state16.var_11 = (rb_state16.var_11) + (((0xf6cfd7bb) - (rb_state16.var_6)) ^ (0x4b78a60e));
            rb_state16.var_12 = (rb_state16.var_12) ^ (((((0x3646168a) + (rb_state16.var_5)) + (rb_state16.var_7)) + (0xa132e)) - (rb_state16.var_8));
            rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_6) == (0x5a4cc2b1)) ? (rb_state16.var_9) : (0x1eac0fc2));
            rb_state16.var_14 = (rb_state16.var_14) ^ (((0xeea5220b) ^ (rb_state16.var_10)) + (0xbc4060e6));
            rb_state16.var_15 = (rb_state16.var_15) - (((((0x1395a098) ^ (rb_state16.var_12)) ^ (rb_state16.var_11)) - (0xfee90fa0)) + (rb_state16.var_7));
            rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_8) == (0x278b6960)) ? (rb_state16.var_13) : (0xa4eb948f));
            rb_state16.var_17 = (rb_state16.var_17) + (rb_state16.var_14);
            rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_9) == (0x6731981)) ? (rb_state16.var_15) : (0x11b7e210));
            rb_state16.var_19 = (rb_state16.var_19) + (((0x60876bea) ^ (rb_state16.var_16)) + (0x4a4eec33));
            rb_state16.var_20 = (rb_state16.var_20) ^ (((0xf61d6afb) + (rb_state16.var_17)) ^ (rb_state16.var_10));
            rb_state16.var_21 = (rb_state16.var_21) - (rb_state16.var_18);
            rb_state16.var_22 = (rb_state16.var_22) - (rb_state16.var_19);
            rb_state16.var_23 = (rb_state16.var_23) ^ (((rb_state16.var_11) == (0xd7fbb304)) ? (rb_state16.var_20) : (0xdc63281b));
            rb_state16.var_24 = (rb_state16.var_24) ^ (((rb_state16.var_21) == (0x27c403a4)) ? (rb_state16.var_22) : (0x9dbb63ad));
            rb_state16.var_25 = (rb_state16.var_25) + (((rb_state16.var_23) == (0xa102edec)) ? (rb_state16.var_24) : (0x533517a7));
            rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_25);
            rb_state16.var_1 = rb_state16.var_2;
            pthread_mutex_unlock(&(rb_state16.lock_36));
        }
        if ((rb_state16.var_0) == (0x0)) {
            if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                racebench_trigger(16);
            }
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