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
    rb_state1.var_0 = (rb_state1.var_0) + ((0xb1f4488e) + (0xed0f4a85));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (77 < rb_input_size ? (uint32_t)rb_input[77] : 0xfbbb0397);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x1fa987fc));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (0x2ac03957);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_16) == (0x9c6ece8e)) {
        rb_state11.var_23 = 0x4ac6cca7;
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_18) : (0xd0cc5535));
        rb_state11.var_25 = (rb_state11.var_25) - (((((0xf31e9de9) ^ (rb_state11.var_21)) - (rb_state11.var_24)) - (rb_state11.var_23)) - (0x5c870503));
        rb_state11.var_17 = (rb_state11.var_17) + (rb_state11.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0xf01f0cb7)) {
        rb_state12.var_14 = 0x119568c1;
        rb_state12.var_15 = 0x5f630212;
        rb_state12.var_16 = (rb_state12.var_16) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_14) : (0x44b5c5a6));
        rb_state12.var_17 = (rb_state12.var_17) - (((0xc5063522) + (rb_state12.var_9)) - (rb_state12.var_7));
        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0xb78ef944));
        rb_state12.var_19 = (rb_state12.var_19) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_17) : (0xdaad83cc));
        rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_19) : (0xcf5239a8));
        rb_state12.var_13 = (rb_state12.var_13) - (rb_state12.var_20);
        rb_state12.var_2 = rb_state12.var_13;
    }
    if ((rb_state12.var_1) == (0xf01f0cb7)) {
        pthread_mutex_lock(&(rb_state12.lock_39));
        rb_state12.var_22 = 0xe599b2a5;
        rb_state12.var_23 = (rb_state12.var_23) ^ (rb_state12.var_13);
        rb_state12.var_24 = (rb_state12.var_24) ^ (rb_state12.var_12);
        rb_state12.var_25 = (rb_state12.var_25) ^ (((((0x2d0b919e) + (rb_state12.var_22)) - (rb_state12.var_11)) ^ (0xed1d4fd)) ^ (rb_state12.var_14));
        rb_state12.var_26 = (rb_state12.var_26) ^ (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_23) : (0xa334fc08));
        rb_state12.var_27 = (rb_state12.var_27) ^ (((((0x76421915) - (rb_state12.var_16)) + (rb_state12.var_17)) ^ (rb_state12.var_24)) + (rb_state12.var_25));
        rb_state12.var_28 = (rb_state12.var_28) + (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_26) : (0xb3825344));
        rb_state12.var_29 = (rb_state12.var_29) ^ (((rb_state12.var_19) == (0x0)) ? (rb_state12.var_27) : (0x24a7892e));
        rb_state12.var_30 = (rb_state12.var_30) - (((0x4dc50836) + (rb_state12.var_20)) - (rb_state12.var_28));
        rb_state12.var_31 = (rb_state12.var_31) ^ (rb_state12.var_29);
        rb_state12.var_32 = (rb_state12.var_32) - (rb_state12.var_30);
        rb_state12.var_33 = (rb_state12.var_33) - (((0x51246ad2) - (rb_state12.var_21)) - (rb_state12.var_31));
        rb_state12.var_34 = (rb_state12.var_34) ^ (((0xcf396134) + (0xf0c495c8)) ^ (rb_state12.var_32));
        rb_state12.var_35 = (rb_state12.var_35) + (rb_state12.var_33);
        rb_state12.var_36 = (rb_state12.var_36) - (rb_state12.var_34);
        rb_state12.var_37 = (rb_state12.var_37) - (rb_state12.var_35);
        rb_state12.var_38 = (rb_state12.var_38) + (((rb_state12.var_36) == (0x0)) ? (rb_state12.var_37) : (0x930a28c7));
        rb_state12.var_21 = (rb_state12.var_21) ^ (rb_state12.var_38);
        rb_state12.var_2 = rb_state12.var_21;
        pthread_mutex_unlock(&(rb_state12.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (1248 < rb_input_size ? (uint32_t)rb_input[1248] : 0xef8630d1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (261 < rb_input_size ? (uint32_t)rb_input[261] : 0x7cf05d25);
    #endif
    pthread_mutex_lock(&que->mutex);

    while (que->tail == que->head && (que->end_count) < que->prod_threads) {
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x8ec73016)) {
            rb_state1.var_14 = 0xdbf3e401;
            rb_state1.var_15 = 0x12a736d0;
            rb_state1.var_16 = (rb_state1.var_16) - (rb_state1.var_10);
            rb_state1.var_17 = (rb_state1.var_17) ^ (((0x4360d7e6) - (0x2245ae04)) - (rb_state1.var_9));
            rb_state1.var_18 = (rb_state1.var_18) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_14) : (0x96053fc0));
            rb_state1.var_19 = (rb_state1.var_19) - (((((0x81d389fc) + (rb_state1.var_16)) + (0xd600762c)) - (rb_state1.var_17)) ^ (0xc585cea2));
            rb_state1.var_20 = (rb_state1.var_20) + (rb_state1.var_18);
            rb_state1.var_21 = (rb_state1.var_21) - (rb_state1.var_19);
            rb_state1.var_22 = (rb_state1.var_22) - (rb_state1.var_20);
            rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_21) : (0x93574aff));
            rb_state1.var_24 = (rb_state1.var_24) + (rb_state1.var_22);
            rb_state1.var_25 = (rb_state1.var_25) ^ (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_23) : (0xfe8d2f71));
            rb_state1.var_26 = (rb_state1.var_26) - (rb_state1.var_24);
            rb_state1.var_27 = (rb_state1.var_27) + (((0xd3920b0b) + (rb_state1.var_25)) - (rb_state1.var_13));
            rb_state1.var_28 = (rb_state1.var_28) - (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_26) : (0x4b91df1b));
            rb_state1.var_29 = (rb_state1.var_29) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_27) : (0x49b351e1));
            rb_state1.var_30 = (rb_state1.var_30) ^ (((0x13a5e78b) - (rb_state1.var_28)) + (0xe4372618));
            rb_state1.var_31 = (rb_state1.var_31) - (rb_state1.var_29);
            rb_state1.var_32 = (rb_state1.var_32) + (((((0xa3d009ed) ^ (rb_state1.var_30)) ^ (0x2fcde3af)) - (rb_state1.var_31)) - (0x2c35e06d));
            rb_state1.var_13 = (rb_state1.var_13) ^ (rb_state1.var_32);
            rb_state1.var_1 = rb_state1.var_13;
        }
        if ((rb_state1.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_47));
            rb_state1.var_34 = 0x1b9b80d1;
            rb_state1.var_35 = 0xd7b9fd22;
            rb_state1.var_36 = (rb_state1.var_36) - (rb_state1.var_17);
            rb_state1.var_37 = (rb_state1.var_37) + (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_34) : (0xd43746e8));
            rb_state1.var_38 = (rb_state1.var_38) - (((rb_state1.var_35) == (0x0)) ? (rb_state1.var_16) : (0xd3b64fc1));
            rb_state1.var_39 = (rb_state1.var_39) + (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_36) : (0xba61fd6d));
            rb_state1.var_40 = (rb_state1.var_40) - (rb_state1.var_37);
            rb_state1.var_41 = (rb_state1.var_41) + (((0x4006ec2f) + (rb_state1.var_38)) - (0x7fb9083));
            rb_state1.var_42 = (rb_state1.var_42) - (rb_state1.var_39);
            rb_state1.var_43 = (rb_state1.var_43) - (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_40) : (0x105181ec));
            rb_state1.var_44 = (rb_state1.var_44) - (((rb_state1.var_41) == (0x0)) ? (rb_state1.var_42) : (0xd123a002));
            rb_state1.var_45 = (rb_state1.var_45) - (rb_state1.var_43);
            rb_state1.var_46 = (rb_state1.var_46) - (((((0x23bd706d) ^ (rb_state1.var_44)) - (rb_state1.var_21)) + (0x3bf14e90)) ^ (rb_state1.var_45));
            rb_state1.var_33 = (rb_state1.var_33) - (rb_state1.var_46);
            rb_state1.var_1 = rb_state1.var_33;
            pthread_mutex_unlock(&(rb_state1.lock_47));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0xddad8cc1)) {
            rb_state5.var_11 = 0x5bae7019;
            rb_state5.var_12 = 0xe0eb4424;
            rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_12) : (0xe276102f));
            rb_state5.var_14 = (rb_state5.var_14) - (rb_state5.var_9);
            rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_11);
            rb_state5.var_16 = (rb_state5.var_16) ^ (rb_state5.var_7);
            rb_state5.var_17 = (rb_state5.var_17) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_13) : (0xc0eb8e99));
            rb_state5.var_18 = (rb_state5.var_18) - (((((0x77810471) + (0x22fdc97f)) ^ (rb_state5.var_15)) + (rb_state5.var_14)) ^ (0xc64d1c9b));
            rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_17) : (0x6a2f0696));
            rb_state5.var_20 = (rb_state5.var_20) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_18) : (0x287207a7));
            rb_state5.var_21 = (rb_state5.var_21) + (((((0x5ea42d56) - (0x9014a70e)) + (rb_state5.var_19)) ^ (rb_state5.var_12)) ^ (rb_state5.var_20));
            rb_state5.var_10 = (rb_state5.var_10) ^ (rb_state5.var_21);
            rb_state5.var_2 = rb_state5.var_10;
        }
        if ((rb_state5.var_0) == (0xddad8cc1)) {
            pthread_mutex_lock(&(rb_state5.lock_38));
            rb_state5.var_23 = (rb_state5.var_23) - (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_16) : (0xd274c27c));
            rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_18) : (0x4a210343));
            rb_state5.var_25 = (rb_state5.var_25) ^ (1189 < rb_input_size ? (uint32_t)rb_input[1189] : 0x61d4342d);
            rb_state5.var_26 = (rb_state5.var_26) + (((rb_state5.var_20) == (0x0)) ? (rb_state5.var_15) : (0x5cdf1d75));
            rb_state5.var_27 = (rb_state5.var_27) ^ (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_13) : (0xa810813f));
            rb_state5.var_28 = (rb_state5.var_28) ^ (((((0xf1413ada) - (rb_state5.var_21)) + (0xfee6f6d7)) ^ (rb_state5.var_23)) + (rb_state5.var_24));
            rb_state5.var_29 = (rb_state5.var_29) ^ (((((0x60993f2a) ^ (rb_state5.var_22)) + (0xfa62ebce)) + (rb_state5.var_25)) + (rb_state5.var_26));
            rb_state5.var_30 = (rb_state5.var_30) - (rb_state5.var_27);
            rb_state5.var_31 = (rb_state5.var_31) ^ (rb_state5.var_28);
            rb_state5.var_32 = (rb_state5.var_32) + (((0xb34dac84) - (rb_state5.var_23)) + (rb_state5.var_29));
            rb_state5.var_33 = (rb_state5.var_33) ^ (((rb_state5.var_30) == (0x0)) ? (rb_state5.var_31) : (0xab817c87));
            rb_state5.var_34 = (rb_state5.var_34) - (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_32) : (0xca9afd03));
            rb_state5.var_35 = (rb_state5.var_35) + (((0x3a746c1f) + (rb_state5.var_33)) + (rb_state5.var_25));
            rb_state5.var_36 = (rb_state5.var_36) - (((0xb59db96b) + (rb_state5.var_34)) + (rb_state5.var_26));
            rb_state5.var_37 = (rb_state5.var_37) ^ (((rb_state5.var_35) == (0x0)) ? (rb_state5.var_36) : (0xf530b193));
            rb_state5.var_22 = (rb_state5.var_22) - (rb_state5.var_37);
            rb_state5.var_2 = rb_state5.var_22;
            pthread_mutex_unlock(&(rb_state5.lock_38));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x0)) {
            rb_state9.var_2 = rb_state9.var_1;
        }
        if ((rb_state9.var_0) == (0x0)) {
            if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_0) == (0x5872ec)) ? (rb_state12.var_1) : (0x5ae0c46f));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xba6f553d));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ ((0xcd61373f) ^ (0x9d483a12));
        if ((rb_state16.var_0) == (0xf63e9dfc)) {
            if ((rb_state16.var_2) != (0x0)) {
                if (!((rb_state16.var_2) == (rb_state16.var_23))) {
                    racebench_trigger(16);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x334e0350)) {
            rb_state18.var_3 = 0xbad68b2d;
            rb_state18.var_4 = 0xfba02577;
            rb_state18.var_5 = (rb_state18.var_5) + (((0x1eeb045d) + (rb_state18.var_4)) - (0x47a6857c));
            rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_2) == (0x1531bbd6)) ? (rb_state18.var_3) : (0x2af89e3a));
            rb_state18.var_7 = (rb_state18.var_7) ^ (((0xb013fe7c) ^ (rb_state18.var_5)) ^ (rb_state18.var_3));
            rb_state18.var_8 = (rb_state18.var_8) - (rb_state18.var_6);
            rb_state18.var_9 = (rb_state18.var_9) + (((((0xf92498cd) ^ (rb_state18.var_8)) + (0x5eacc41)) ^ (rb_state18.var_4)) + (rb_state18.var_7));
            rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_9);
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