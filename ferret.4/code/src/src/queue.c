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
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_2) == (0x48952007)) {
        rb_state4.var_27 = 0xc6022a2a;
        rb_state4.var_28 = (rb_state4.var_28) + ((0xee718c4) + (0x90317455));
        rb_state4.var_29 = (rb_state4.var_29) ^ (((0x3d13341) ^ (rb_state4.var_15)) - (rb_state4.var_27));
        rb_state4.var_30 = (rb_state4.var_30) + (rb_state4.var_16);
        rb_state4.var_31 = (rb_state4.var_31) ^ (rb_state4.var_14);
        rb_state4.var_32 = (rb_state4.var_32) - (rb_state4.var_13);
        rb_state4.var_33 = (rb_state4.var_33) ^ (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_28) : (0x7de71137));
        rb_state4.var_34 = (rb_state4.var_34) + (rb_state4.var_29);
        rb_state4.var_35 = (rb_state4.var_35) + (rb_state4.var_30);
        rb_state4.var_36 = (rb_state4.var_36) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_31) : (0x1675ca50));
        rb_state4.var_37 = (rb_state4.var_37) + (rb_state4.var_32);
        rb_state4.var_38 = (rb_state4.var_38) ^ (rb_state4.var_33);
        rb_state4.var_39 = (rb_state4.var_39) + (((0xb8f412a3) - (0xae65d70c)) ^ (rb_state4.var_34));
        rb_state4.var_40 = (rb_state4.var_40) - (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_35) : (0xefc6632e));
        rb_state4.var_41 = (rb_state4.var_41) ^ (((((0x22f0fd3e) + (rb_state4.var_20)) + (rb_state4.var_37)) ^ (0xfb6ea733)) - (rb_state4.var_36));
        rb_state4.var_42 = (rb_state4.var_42) + (rb_state4.var_38);
        rb_state4.var_43 = (rb_state4.var_43) + (((rb_state4.var_39) == (0x0)) ? (rb_state4.var_40) : (0x5ec91f6b));
        rb_state4.var_44 = (rb_state4.var_44) + (((0x43d3e1dd) + (rb_state4.var_21)) + (rb_state4.var_41));
        rb_state4.var_45 = (rb_state4.var_45) - (((rb_state4.var_42) == (0x0)) ? (rb_state4.var_43) : (0x4401473e));
        rb_state4.var_46 = (rb_state4.var_46) - (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_44) : (0x80708f56));
        rb_state4.var_47 = (rb_state4.var_47) ^ (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_45) : (0x77ed1005));
        rb_state4.var_48 = (rb_state4.var_48) ^ (rb_state4.var_46);
        rb_state4.var_49 = (rb_state4.var_49) + (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_47) : (0x3f934534));
        rb_state4.var_50 = (rb_state4.var_50) + (((0x4e3b7e3d) - (rb_state4.var_48)) ^ (rb_state4.var_25));
        rb_state4.var_51 = (rb_state4.var_51) + (((rb_state4.var_26) == (0x0)) ? (rb_state4.var_49) : (0x798c8716));
        rb_state4.var_52 = (rb_state4.var_52) - (rb_state4.var_50);
        rb_state4.var_53 = (rb_state4.var_53) + (((rb_state4.var_51) == (0x0)) ? (rb_state4.var_52) : (0xd043d60f));
        rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_53);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0xf4830f3d));
    #endif
    pthread_mutex_lock(&que->mutex);

    while (que->tail == que->head && (que->end_count) < que->prod_threads) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) ^ (((rb_state4.var_2) == (0xc137a2e0)) ? (rb_state4.var_2) : (0x87629fe7));
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_2) == (0xc054f499)) {
            rb_state7.var_4 = 0xbe564d03;
            rb_state7.var_5 = 0x755182a0;
            rb_state7.var_6 = (rb_state7.var_6) + (((rb_state7.var_5) == (0x755182a0)) ? (rb_state7.var_3) : (0xf7096581));
            rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_4) == (0xbe564d03)) ? (rb_state7.var_6) : (0x497e0a9));
            rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_7);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (1589 < rb_input_size ? (uint32_t)rb_input[1589] : 0x5afd9dfc);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_0);
        if ((rb_state13.var_2) == (0xff41920d)) {
            if ((rb_state13.var_3) != (0x0)) {
                if (!((rb_state13.var_3) == (rb_state13.var_18))) {
                    racebench_trigger(13);
                }
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