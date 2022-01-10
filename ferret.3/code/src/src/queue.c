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
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0xaf316462));
    #endif
    pthread_mutex_lock(&que->mutex);

    while (que->tail == que->head && (que->end_count) < que->prod_threads) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
        rb_state2.var_1 = (rb_state2.var_1) - (0x404d7c51);
        rb_state2.var_0 = (rb_state2.var_0) - ((0x9e85049) ^ (rb_state2.var_1));
        if ((rb_state2.var_0) == (0xd005243a)) {
            pthread_mutex_lock(&(rb_state2.lock_32));
            rb_state2.var_4 = 0x1c7e5830;
            rb_state2.var_5 = 0x9b1145b0;
            rb_state2.var_6 = (rb_state2.var_6) + (((((0x69a5e587) + (0x9004de71)) ^ (rb_state2.var_4)) ^ (rb_state2.var_2)) - (0x62a6a402));
            rb_state2.var_7 = (rb_state2.var_7) - ((0x9b321934) + (0x50e7475c));
            rb_state2.var_8 = (rb_state2.var_8) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0xc2e0d526));
            rb_state2.var_9 = (rb_state2.var_9) + (rb_state2.var_7);
            rb_state2.var_10 = (rb_state2.var_10) - (((((0x86308f69) + (rb_state2.var_8)) - (0x78d5cd1e)) ^ (rb_state2.var_9)) + (0x152ab294));
            rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_10);
            rb_state2.var_2 = rb_state2.var_3;
            pthread_mutex_unlock(&(rb_state2.lock_32));
        }
        if ((rb_state2.var_0) == (0xd005243a)) {
            if (!((rb_state2.var_2) == (rb_state2.var_3))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (761 < rb_input_size ? (uint32_t)rb_input[761] : 0xc7e9e79f);
        rb_state3.var_2 = (rb_state3.var_2) ^ (732 < rb_input_size ? (uint32_t)rb_input[732] : 0xcdf6f4ba);
        if ((rb_state3.var_1) == (0xef)) {
            rb_state3.var_10 = 0x6e8b8666;
            rb_state3.var_11 = (rb_state3.var_11) + (rb_state3.var_10);
            rb_state3.var_12 = (rb_state3.var_12) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x6f817bc9));
            rb_state3.var_13 = (rb_state3.var_13) - (((0xd378f4f3) ^ (rb_state3.var_11)) ^ (rb_state3.var_6));
            rb_state3.var_14 = (rb_state3.var_14) ^ (rb_state3.var_12);
            rb_state3.var_15 = (rb_state3.var_15) + (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_13) : (0x965f2be6));
            rb_state3.var_16 = (rb_state3.var_16) - (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_14) : (0x3f2a64f1));
            rb_state3.var_17 = (rb_state3.var_17) - (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_15) : (0x2a93bfe7));
            rb_state3.var_18 = (rb_state3.var_18) ^ (((0xae10f7b6) + (rb_state3.var_16)) + (0x3d073817));
            rb_state3.var_19 = (rb_state3.var_19) ^ (rb_state3.var_17);
            rb_state3.var_20 = (rb_state3.var_20) - (((0xf8701cd7) + (0xf08592c5)) + (rb_state3.var_18));
            rb_state3.var_21 = (rb_state3.var_21) - (((0x18b7fb17) + (rb_state3.var_10)) + (rb_state3.var_19));
            rb_state3.var_22 = (rb_state3.var_22) ^ (((((0xcb5d1983) + (rb_state3.var_20)) - (rb_state3.var_21)) + (rb_state3.var_11)) ^ (0xa6888d60));
            rb_state3.var_9 = (rb_state3.var_9) ^ (rb_state3.var_22);
            rb_state3.var_3 = rb_state3.var_9;
        }
        if ((rb_state3.var_1) == (0xef)) {
            pthread_mutex_lock(&(rb_state3.lock_29));
            rb_state3.var_24 = 0xd1de7395;
            rb_state3.var_25 = (rb_state3.var_25) - (((0x2595fa46) - (rb_state3.var_14)) - (rb_state3.var_12));
            rb_state3.var_26 = (rb_state3.var_26) + (((0xeac40bae) - (rb_state3.var_13)) ^ (rb_state3.var_15));
            rb_state3.var_27 = (rb_state3.var_27) ^ (((((0x5864c804) ^ (0x1841ba6e)) ^ (rb_state3.var_25)) - (rb_state3.var_24)) + (rb_state3.var_16));
            rb_state3.var_28 = (rb_state3.var_28) - (((((0x4582e1b3) - (rb_state3.var_26)) ^ (rb_state3.var_17)) ^ (0xdde96687)) + (rb_state3.var_27));
            rb_state3.var_23 = (rb_state3.var_23) ^ (rb_state3.var_28);
            rb_state3.var_3 = rb_state3.var_23;
            pthread_mutex_unlock(&(rb_state3.lock_29));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) - (0x9b152304);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x1cbb6a9)) {
            rb_state18.var_4 = 0xb0e200dd;
            rb_state18.var_5 = (rb_state18.var_5) - (((0x65af25d4) - (0x4305ae10)) + (rb_state18.var_3));
            rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_4) : (0x12881352));
            rb_state18.var_7 = (rb_state18.var_7) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_6) : (0x2346f731));
            rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_5) : (0xab2cee9e));
            rb_state18.var_9 = (rb_state18.var_9) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_6) : (0x6fac0718));
            rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0xb0f3ee7a));
            rb_state18.var_11 = (rb_state18.var_11) + (((((0x7d252664) - (rb_state18.var_10)) - (rb_state18.var_10)) - (0x9b456de8)) + (rb_state18.var_9));
            rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_11);
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