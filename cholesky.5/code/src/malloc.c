#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "matrix.h"

#define ALIGN 8
#define MAXFAST 16
#define MAXFASTBL (1 << (MAXFAST - 1))

#define SIZE(block) (block[-1])
#define HOME(block) (block[-2])
#define NEXTFREE(block) (*((unsigned **)block))

struct MemPool {
    pthread_mutex_t(memoryLock);
    unsigned *volatile *freeBlock;
    int tally, touched, maxm;
} * mem_pool;

int mallocP = 1, machineP = 1;

extern struct GlobalMemory *Global;

MallocInit(P) {
    int p;

    mallocP = P;

    mem_pool = (struct MemPool *)malloc((mallocP + 1) * sizeof(struct MemPool));
    ;
    mem_pool++;

    for (p = -1; p < mallocP; p++) {
        InitOneFreeList(p);
    }
}

InitOneFreeList(p) {
    int j;

    { pthread_mutex_init(&(mem_pool[p].memoryLock), NULL); };
    if (p > 0) {
        mem_pool[p].freeBlock = (unsigned **)malloc((MAXFAST + 1) * sizeof(unsigned *));
        ;
        MigrateMem(mem_pool[p].freeBlock, (MAXFAST + 1) * sizeof(unsigned *), p);
    } else {
        mem_pool[p].freeBlock = (unsigned **)malloc((MAXFAST + 1) * sizeof(unsigned *));
        ;
        MigrateMem(mem_pool[p].freeBlock, (MAXFAST + 1) * sizeof(unsigned *), DISTRIBUTED);
    }
    for (j = 0; j <= MAXFAST; j++) {
        mem_pool[p].freeBlock[j] = NULL;
    }
    mem_pool[p].tally = mem_pool[p].maxm = mem_pool[p].touched = 0;
}

MallocStats() {
    int i;

    printf("Malloc max: ");
    for (i = 0; i < mallocP; i++) {
        if (mem_pool[i].touched > 0) {
            printf("%d* ", mem_pool[i].maxm);
        } else {
            printf("%d ", mem_pool[i].maxm);
        }
    }
    printf("\n");
}

FindBucket(size) {
    int bucket;

    if (size > MAXFASTBL) {
        bucket = MAXFAST;
    } else {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_3) == (0x0)) {
            rb_state0.var_4 = rb_state0.var_1;
        }
        if ((rb_state0.var_3) == (0x0)) {
            if (!((rb_state0.var_1) == (rb_state0.var_4))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_3 = (rb_state6.var_3) ^ (0xfb0bd863);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + ((0x58f9aef1) - (0xc51a78b6));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x6e8529b0)) ? (rb_state15.var_0) : (0xbb688420));
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_0);
            rb_state1.var_2 = (rb_state1.var_2) ^ (0x96c3fbd5);
            rb_state1.var_2 = (rb_state1.var_2) - (0xd186cd1a);
            rb_state1.var_4 = (rb_state1.var_4) + (0xd8d7c44f);
            rb_state1.var_4 = (rb_state1.var_4) - (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x5e104769));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_0);
            rb_state2.var_3 = (rb_state2.var_3) ^ ((0xbdf72954) - (rb_state2.var_3));
            rb_state2.var_3 = (rb_state2.var_3) - (((rb_state2.var_2) == (0xf0ae4257)) ? (rb_state2.var_1) : (0xbb4ded4d));
            rb_state2.var_3 = (rb_state2.var_3) + (0x7d4760f);
            rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_4);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) ^ ((0x3fe6e582) + (rb_state3.var_0));
            rb_state3.var_1 = (rb_state3.var_1) - (((rb_state3.var_0) == (0xf87e3bc)) ? (rb_state3.var_2) : (0xea43ff3e));
            rb_state3.var_3 = (rb_state3.var_3) + (76620 < rb_input_size ? (uint32_t)rb_input[76620] : 0xdd465419);
            if ((rb_state3.var_2) == (0xc813fc1d)) {
                rb_state3.var_9 = 0xddb85729;
                rb_state3.var_10 = 0xa16ba0c;
                rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_9) : (0x10d04e32));
                rb_state3.var_12 = (rb_state3.var_12) ^ (rb_state3.var_11);
                rb_state3.var_13 = (rb_state3.var_13) ^ (rb_state3.var_10);
                rb_state3.var_14 = (rb_state3.var_14) + (((0x5772a29a) + (rb_state3.var_12)) + (rb_state3.var_8));
                rb_state3.var_15 = (rb_state3.var_15) - (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_9) : (0xd12a78d8));
                rb_state3.var_16 = (rb_state3.var_16) - (((0x56532bcb) ^ (0xc38c3a2e)) - (rb_state3.var_11));
                rb_state3.var_17 = (rb_state3.var_17) - (((((0x2383c40a) - (rb_state3.var_12)) - (0x734b23a9)) ^ (0x22f6d063)) + (rb_state3.var_13));
                rb_state3.var_18 = (rb_state3.var_18) ^ (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_14) : (0xc4b705e1));
                rb_state3.var_19 = (rb_state3.var_19) - (((((0xc6100bf0) + (rb_state3.var_15)) + (0xcd7e4ee1)) + (rb_state3.var_16)) ^ (0x143992e));
                rb_state3.var_20 = (rb_state3.var_20) - (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_18) : (0x217658f7));
                rb_state3.var_21 = (rb_state3.var_21) ^ (rb_state3.var_19);
                rb_state3.var_22 = (rb_state3.var_22) + (rb_state3.var_20);
                rb_state3.var_23 = (rb_state3.var_23) - (((rb_state3.var_21) == (0x0)) ? (rb_state3.var_22) : (0x314cc3a7));
                rb_state3.var_8 = (rb_state3.var_8) - (rb_state3.var_23);
                rb_state3.var_5 = rb_state3.var_8;
            }
            if (!((rb_state3.var_5) == (rb_state3.var_24))) {
                pthread_mutex_lock(&(rb_state3.lock_60));
                rb_state3.var_32 = 0x1d165af1;
                rb_state3.var_33 = (rb_state3.var_33) - (0xee9f641d);
                rb_state3.var_34 = (rb_state3.var_34) - (((rb_state3.var_19) == (0x0)) ? (rb_state3.var_18) : (0xe9de7f94));
                rb_state3.var_35 = (rb_state3.var_35) - (rb_state3.var_20);
                rb_state3.var_36 = (rb_state3.var_36) ^ (((rb_state3.var_21) == (0x0)) ? (rb_state3.var_16) : (0x3b4acbe0));
                rb_state3.var_37 = (rb_state3.var_37) ^ (((((0xa23126d4) - (rb_state3.var_32)) ^ (rb_state3.var_22)) + (rb_state3.var_17)) + (0xba8b302d));
                rb_state3.var_38 = (rb_state3.var_38) + (((((0xf96d93c1) - (0x1f7d8fbb)) - (rb_state3.var_34)) + (rb_state3.var_33)) + (rb_state3.var_23));
                rb_state3.var_39 = (rb_state3.var_39) + (rb_state3.var_35);
                rb_state3.var_40 = (rb_state3.var_40) ^ (((rb_state3.var_36) == (0x0)) ? (rb_state3.var_37) : (0x89fbc338));
                rb_state3.var_41 = (rb_state3.var_41) ^ (rb_state3.var_38);
                rb_state3.var_42 = (rb_state3.var_42) ^ (((rb_state3.var_39) == (0x0)) ? (rb_state3.var_40) : (0x7572521a));
                rb_state3.var_43 = (rb_state3.var_43) ^ (((((0xfbbc6b1c) ^ (0x50bf8950)) ^ (rb_state3.var_42)) ^ (rb_state3.var_41)) ^ (0xfc3e5401));
                rb_state3.var_31 = (rb_state3.var_31) ^ (rb_state3.var_43);
                rb_state3.var_30 = rb_state3.var_31;
                pthread_mutex_unlock(&(rb_state3.lock_60));
            }
            if (!((rb_state3.var_5) == (rb_state3.var_24))) {
                pthread_mutex_lock(&(rb_state3.lock_60));
                if (!((rb_state3.var_30) == (rb_state3.var_31))) {
                    racebench_trigger(3);
                }
                pthread_mutex_unlock(&(rb_state3.lock_60));
            }
            if ((rb_state3.var_2) == (0x877d3523)) {
                pthread_mutex_lock(&(rb_state3.lock_29));
                rb_state3.var_25 = 0x76119e96;
                rb_state3.var_26 = 0x5d52940f;
                rb_state3.var_27 = (rb_state3.var_27) + (((((0x507d4156) - (0x84408b60)) + (rb_state3.var_26)) + (rb_state3.var_25)) ^ (0xdad3fe93));
                rb_state3.var_28 = (rb_state3.var_28) - (((((0xc2c82942) ^ (0xe68a4df)) ^ (rb_state3.var_15)) ^ (0x8467c693)) + (rb_state3.var_27));
                rb_state3.var_24 = (rb_state3.var_24) ^ (rb_state3.var_28);
                rb_state3.var_5 = rb_state3.var_24;
                pthread_mutex_unlock(&(rb_state3.lock_29));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (0xcc9b939b);
            rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
            rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0xacfaae99));
            rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (149493 < rb_input_size ? (uint32_t)rb_input[149493] : 0x4c11ea84);
            rb_state5.var_3 = (rb_state5.var_3) ^ (64650 < rb_input_size ? (uint32_t)rb_input[64650] : 0x60af213c);
            rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_2);
            rb_state5.var_3 = (rb_state5.var_3) - (87537 < rb_input_size ? (uint32_t)rb_input[87537] : 0xfea6f46f);
            rb_state5.var_4 = (rb_state5.var_4) + (85940 < rb_input_size ? (uint32_t)rb_input[85940] : 0xc2903b85);
            rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_2) == (0x131d7f58)) ? (rb_state5.var_1) : (0xe822039b));
            if ((rb_state5.var_3) == (0x0)) {
                rb_state5.var_5 = rb_state5.var_4;
            }
            if ((rb_state5.var_3) == (0x0)) {
                if (!((rb_state5.var_4) == (rb_state5.var_5))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_3);
            rb_state6.var_4 = (rb_state6.var_4) - (43086 < rb_input_size ? (uint32_t)rb_input[43086] : 0x4f1d1b25);
            rb_state6.var_4 = (rb_state6.var_4) + (((rb_state6.var_0) == (0xda233486)) ? (rb_state6.var_4) : (0x8e8330d2));
            if ((rb_state6.var_1) == (0xb4e1ec52)) {
                rb_state6.var_5 = rb_state6.var_2;
            }
            if ((rb_state6.var_1) == (0xb4e1ec52)) {
                pthread_mutex_lock(&(rb_state6.lock_21));
                rb_state6.var_6 = (rb_state6.var_6) - (20429 < rb_input_size ? (uint32_t)rb_input[20429] : 0xdec2ab12);
                rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_6) : (0x267b5c2b));
                rb_state6.var_8 = (rb_state6.var_8) - ((0x68ee5f9c) ^ (rb_state6.var_8));
                rb_state6.var_9 = (rb_state6.var_9) + (0x14e65650);
                rb_state6.var_10 = (rb_state6.var_10) - (rb_state6.var_5);
                rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_3) == (0x80f8fb07)) ? (rb_state6.var_6) : (0xf65033d0));
                rb_state6.var_12 = (rb_state6.var_12) - (((((0xb011ac3b) ^ (rb_state6.var_9)) ^ (rb_state6.var_7)) + (0x1e9e2881)) + (rb_state6.var_8));
                rb_state6.var_13 = (rb_state6.var_13) ^ (((0x8098f97) - (rb_state6.var_9)) - (0x2db964c7));
                rb_state6.var_14 = (rb_state6.var_14) + (((0x424bcfb5) + (rb_state6.var_10)) + (rb_state6.var_10));
                rb_state6.var_15 = (rb_state6.var_15) - (rb_state6.var_11);
                rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0xd558163));
                rb_state6.var_17 = (rb_state6.var_17) ^ (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_15) : (0x76b5319c));
                rb_state6.var_18 = (rb_state6.var_18) - (rb_state6.var_16);
                rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_17) : (0x34f4d0c9));
                rb_state6.var_20 = (rb_state6.var_20) + (((((0x282e89d7) + (rb_state6.var_13)) ^ (rb_state6.var_19)) + (rb_state6.var_12)) - (rb_state6.var_18));
                rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_20);
                pthread_mutex_unlock(&(rb_state6.lock_21));
            }
            if ((rb_state6.var_1) == (0xb4e1ec52)) {
                if (!((rb_state6.var_2) == (rb_state6.var_5))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (123297 < rb_input_size ? (uint32_t)rb_input[123297] : 0x3e2d9f91);
            rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_1) : (0x7869793c));
            rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_1) == (0x82f66bb2)) ? (rb_state9.var_0) : (0x37f0359f));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_3 = (rb_state10.var_3) ^ (((rb_state10.var_1) == (0x38)) ? (rb_state10.var_0) : (0x99cfd954));
            rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_3);
            rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_2) == (0x3471fe13)) ? (rb_state10.var_1) : (0x95580499));
            rb_state10.var_3 = (rb_state10.var_3) ^ (0x9dee717a);
            rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_4);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0xffffff70)) ? (rb_state11.var_0) : (0xde9ad126));
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_1);
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
            rb_state11.var_2 = (rb_state11.var_2) + (13263 < rb_input_size ? (uint32_t)rb_input[13263] : 0xee5bcae5);
            rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_2);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_1);
            rb_state12.var_3 = (rb_state12.var_3) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_2) : (0x4d5b36e5));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_0);
            rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_0);
            rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_2);
            rb_state14.var_1 = (rb_state14.var_1) ^ (10122 < rb_input_size ? (uint32_t)rb_input[10122] : 0x7a08c6e0);
            rb_state14.var_3 = (rb_state14.var_3) - (0x48a051b);
            rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_2);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x3db02679)) ? (rb_state15.var_0) : (0x8d425373));
            rb_state15.var_0 = (rb_state15.var_0) + (155229 < rb_input_size ? (uint32_t)rb_input[155229] : 0x4eab907e);
            rb_state15.var_2 = (rb_state15.var_2) - (0xafe01e1e);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
            rb_state16.var_1 = (rb_state16.var_1) - ((0x2879b341) - (0x9087f170));
            rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_1) : (0x54638bcd));
            rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_2);
            rb_state16.var_0 = (rb_state16.var_0) + (0xe67a8c1b);
            rb_state16.var_1 = (rb_state16.var_1) + (((rb_state16.var_0) == (0xe67a8c1b)) ? (rb_state16.var_2) : (0xf1340d8b));
            rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_3);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_1);
            rb_state17.var_3 = (rb_state17.var_3) + ((0xa871a8e1) + (rb_state17.var_3));
            rb_state17.var_3 = (rb_state17.var_3) ^ (61606 < rb_input_size ? (uint32_t)rb_input[61606] : 0xc9a14249);
            rb_state17.var_2 = (rb_state17.var_2) - (146140 < rb_input_size ? (uint32_t)rb_input[146140] : 0xc0b17319);
            if ((rb_state17.var_3) == (0x438e56ff)) {
                rb_state17.var_5 = 0xe114ca63;
                rb_state17.var_6 = (rb_state17.var_6) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_5) : (0x6ffb63));
                rb_state17.var_7 = (rb_state17.var_7) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_6) : (0x3aa91790));
                rb_state17.var_8 = (rb_state17.var_8) ^ (0x524849ee);
                rb_state17.var_9 = (rb_state17.var_9) ^ (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_4) : (0x1f05b6b5));
                rb_state17.var_10 = (rb_state17.var_10) + (rb_state17.var_6);
                rb_state17.var_11 = (rb_state17.var_11) - (((0x1384ac80) - (rb_state17.var_7)) - (0x47f1a2ec));
                rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_8);
                rb_state17.var_13 = (rb_state17.var_13) ^ (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_9) : (0x90ae3131));
                rb_state17.var_14 = (rb_state17.var_14) + (rb_state17.var_10);
                rb_state17.var_15 = (rb_state17.var_15) - (((0x47930138) ^ (0xb01ff5a8)) ^ (rb_state17.var_11));
                rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_12) : (0x3c7ffb29));
                rb_state17.var_17 = (rb_state17.var_17) ^ (rb_state17.var_13);
                rb_state17.var_18 = (rb_state17.var_18) + (rb_state17.var_14);
                rb_state17.var_19 = (rb_state17.var_19) - (((0x4a9de925) + (rb_state17.var_15)) - (0xba267c8a));
                rb_state17.var_20 = (rb_state17.var_20) + (((0x4851fae) ^ (rb_state17.var_16)) + (rb_state17.var_11));
                rb_state17.var_21 = (rb_state17.var_21) - (((0x2b4264f5) + (rb_state17.var_17)) ^ (rb_state17.var_12));
                rb_state17.var_22 = (rb_state17.var_22) ^ (rb_state17.var_18);
                rb_state17.var_23 = (rb_state17.var_23) - (((0xe24e3a9a) ^ (0x56f50e72)) + (rb_state17.var_19));
                rb_state17.var_24 = (rb_state17.var_24) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_20) : (0x33368e28));
                rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_21);
                rb_state17.var_26 = (rb_state17.var_26) + (((0x64d0b926) - (0x8a398afa)) - (rb_state17.var_22));
                rb_state17.var_27 = (rb_state17.var_27) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_23) : (0xa4ced073));
                rb_state17.var_28 = (rb_state17.var_28) - (((0x3c84bd1f) - (rb_state17.var_15)) ^ (rb_state17.var_24));
                rb_state17.var_29 = (rb_state17.var_29) ^ (((((0xcb4a177) - (0x62e0aa2f)) - (rb_state17.var_25)) ^ (rb_state17.var_26)) ^ (rb_state17.var_16));
                rb_state17.var_30 = (rb_state17.var_30) + (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_27) : (0xf034444f));
                rb_state17.var_31 = (rb_state17.var_31) ^ (rb_state17.var_28);
                rb_state17.var_32 = (rb_state17.var_32) - (((0x681634ae) - (rb_state17.var_29)) - (0x743e6a36));
                rb_state17.var_33 = (rb_state17.var_33) - (((rb_state17.var_30) == (0x0)) ? (rb_state17.var_31) : (0x47c1913));
                rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_32) : (0xadb6ce7b));
                rb_state17.var_35 = (rb_state17.var_35) + (rb_state17.var_33);
                rb_state17.var_36 = (rb_state17.var_36) ^ (rb_state17.var_34);
                rb_state17.var_37 = (rb_state17.var_37) ^ (((((0x43b7ff0f) + (0xf777563c)) - (0x3e1cbf9d)) ^ (rb_state17.var_35)) + (rb_state17.var_36));
                rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_37);
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
            rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_1) : (0x13c001c8));
            rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_2);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
            if ((rb_state19.var_4) == (0x66d46aa0)) {
                if ((rb_state19.var_5) != (0x0)) {
                    if (!((rb_state19.var_5) == (rb_state19.var_32))) {
                        racebench_trigger(19);
                    }
                }
            }
            #endif
            bucket++;
        }
    }

    return (bucket);
}

char *MyMalloc(size, home)
int size, home;
{
    int i, bucket, leftover, alloc_size;
    unsigned *d, *result, *prev, *freespace, *freelast;
    unsigned int block_size;
    extern int MyNum;

    if (size < ALIGN) {
        size = ALIGN;
    }

    if (home == DISTRIBUTED) {
        home = -1;
    }

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - ((0xa6ebe617) - (0xb07aa5c3));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (0x3db02679);
    rb_state15.var_1 = (rb_state15.var_1) - ((0xb547e041) + (0xdc32f60f));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_2);
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_4);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_4 = (rb_state10.var_4) - ((0x24eb46c1) ^ (0xe75f2502));
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) + ((0xcd5559f) + (0xc94b4830));
    rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_3);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) - (rb_state3.var_3);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (((rb_state4.var_0) == (0xd1ecccf0)) ? (rb_state4.var_3) : (0x20d04b50));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_2) : (0xe6f1a7e2));
    #endif
    result = NULL;

    if (bucket < MAXFAST) {
        if (mem_pool[home].freeBlock[bucket]) {
            { pthread_mutex_lock(&(mem_pool[home].memoryLock)); }
            result = mem_pool[home].freeBlock[bucket];
            if (result) {
                mem_pool[home].freeBlock[bucket] = NEXTFREE(result);
            }
            { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
        }
    }

    if (!result) {
        { pthread_mutex_lock(&(mem_pool[home].memoryLock)); }
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0xffffff58)) {
            rb_state2.var_15 = 0xe1a3ff62;
            rb_state2.var_16 = 0x82e09be4;
            rb_state2.var_17 = (rb_state2.var_17) + (0x799893a8);
            rb_state2.var_18 = (rb_state2.var_18) + (rb_state2.var_16);
            rb_state2.var_19 = (rb_state2.var_19) - (((((0x9726a325) - (rb_state2.var_9)) - (rb_state2.var_8)) + (0x6b90b99c)) + (rb_state2.var_15));
            rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_18) : (0xa7a73f02));
            rb_state2.var_21 = (rb_state2.var_21) ^ (((((0xba575c9f) ^ (0xeeb6330f)) ^ (0x2d54f364)) - (rb_state2.var_20)) ^ (rb_state2.var_19));
            rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_4 = (rb_state3.var_4) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_4) : (0xa3236fee));
        rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_1) == (0xebc0f248)) ? (rb_state3.var_4) : (0xa16dce6));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_4 = (rb_state5.var_4) ^ ((0x9f02fc99) ^ (0xd8925876));
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_4) == (0xb4e2d4b7)) {
            rb_state10.var_5 = rb_state10.var_0;
        }
        if ((rb_state10.var_4) == (0xb4e2d4b7)) {
            pthread_mutex_lock(&(rb_state10.lock_10));
            if (!((rb_state10.var_0) == (rb_state10.var_5))) {
                racebench_trigger(10);
            }
            pthread_mutex_unlock(&(rb_state10.lock_10));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_3) == (0x0)) {
            rb_state16.var_16 = (rb_state16.var_16) ^ (((0xf4cd32e7) - (rb_state16.var_11)) - (rb_state16.var_13));
            rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_12) : (0x32fbf8b0));
            rb_state16.var_18 = (rb_state16.var_18) - (12320 < rb_input_size ? (uint32_t)rb_input[12320] : 0x83f13cb);
            rb_state16.var_19 = (rb_state16.var_19) ^ (rb_state16.var_14);
            rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_16) : (0x5a5b708c));
            rb_state16.var_21 = (rb_state16.var_21) ^ (((0x773577d2) ^ (rb_state16.var_17)) + (0x1f185ddc));
            rb_state16.var_22 = (rb_state16.var_22) + (((0x7ca1fe5d) - (rb_state16.var_16)) - (rb_state16.var_18));
            rb_state16.var_23 = (rb_state16.var_23) + (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_19) : (0x31ebcdb3));
            rb_state16.var_24 = (rb_state16.var_24) ^ (((((0x3ec7986e) ^ (rb_state16.var_20)) + (rb_state16.var_21)) + (rb_state16.var_18)) - (0x15b02063));
            rb_state16.var_25 = (rb_state16.var_25) ^ (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_23) : (0x5c79df20));
            rb_state16.var_26 = (rb_state16.var_26) ^ (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_24) : (0x92447c0));
            rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_26) : (0x9991e97));
            rb_state16.var_4 = (rb_state16.var_4) - (rb_state16.var_27);
        }
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_1
            rb_state1.var_3 = (rb_state1.var_3) ^ ((0x992e2295) ^ (rb_state1.var_1));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) ^ (0x17dc91f6);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) ^ (145958 < rb_input_size ? (uint32_t)rb_input[145958] : 0x875cf69e);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) + (0x72fdfc16);
            rb_state16.var_3 = (rb_state16.var_3) ^ (0x69dcce85);
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_3) == (0x4d8e56ff)) {
                pthread_mutex_lock(&(rb_state17.lock_38));
                rb_state17.var_4 = rb_state17.var_0;
                pthread_mutex_unlock(&(rb_state17.lock_38));
            }
            if ((rb_state17.var_3) == (0x438e56ff)) {
                if (!((rb_state17.var_0) == (rb_state17.var_4))) {
                    racebench_trigger(17);
                }
            }
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_3) == (0x0)) {
                    rb_state5.var_6 = (rb_state5.var_6) ^ (0xe1d2ef6a);
                    rb_state5.var_7 = (rb_state5.var_7) - (0x6365234d);
                    rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_0) : (0xf6d2a525));
                    rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_6);
                    rb_state5.var_10 = (rb_state5.var_10) - (((((0xe135f524) + (rb_state5.var_6)) + (0xd392f819)) ^ (rb_state5.var_7)) + (rb_state5.var_8));
                    rb_state5.var_11 = (rb_state5.var_11) ^ (((0x7f64c5b) + (0x3c6dcb7d)) - (rb_state5.var_9));
                    rb_state5.var_12 = (rb_state5.var_12) + (rb_state5.var_10);
                    rb_state5.var_13 = (rb_state5.var_13) ^ (((((0xf648b801) ^ (rb_state5.var_7)) + (0x2e20f6b6)) + (rb_state5.var_11)) ^ (rb_state5.var_12));
                    rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_13);
                }
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_4 = (rb_state1.var_4) - ((0xa0c6a35b) ^ (0xb9621f88));
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_1 = (rb_state4.var_1) ^ (0x215e8379);
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_0);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) ^ ((0x1aa8c8ab) - (0x58611f98));
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_0) == (0xffffff58)) {
                    pthread_mutex_lock(&(rb_state2.lock_22));
                    rb_state2.var_7 = 0x34d47036;
                    rb_state2.var_8 = 0x3d8d3ba7;
                    rb_state2.var_9 = 0x453ace2f;
                    rb_state2.var_10 = (rb_state2.var_10) - (((0x51cd401e) - (rb_state2.var_7)) + (rb_state2.var_5));
                    rb_state2.var_11 = (rb_state2.var_11) + (((rb_state2.var_9) == (0x453ace2f)) ? (rb_state2.var_8) : (0x9bfb7f76));
                    rb_state2.var_12 = (rb_state2.var_12) + (((0x110472bd) - (rb_state2.var_6)) - (rb_state2.var_10));
                    rb_state2.var_13 = (rb_state2.var_13) + (rb_state2.var_11);
                    rb_state2.var_14 = (rb_state2.var_14) ^ (((((0x8f59f1fb) - (rb_state2.var_12)) + (rb_state2.var_13)) ^ (0x110a3e9f)) - (rb_state2.var_7));
                    rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_14);
                    rb_state2.var_5 = rb_state2.var_6;
                    pthread_mutex_unlock(&(rb_state2.lock_22));
                }
                if ((rb_state2.var_0) == (0xffffff58)) {
                    if (!((rb_state2.var_5) == (rb_state2.var_6))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_4) == (0xb4e2d4b7)) {
                    rb_state10.var_6 = 0x4e6d6ca9;
                    rb_state10.var_7 = 0x29e39ce1;
                    rb_state10.var_8 = (rb_state10.var_8) + (((((0xcba75d07) - (rb_state10.var_6)) ^ (0x1c9869c2)) ^ (rb_state10.var_6)) ^ (rb_state10.var_5));
                    rb_state10.var_9 = (rb_state10.var_9) + (((((0x6b331ba) + (rb_state10.var_8)) + (0x644c487a)) + (rb_state10.var_7)) - (0x79cbcc83));
                    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_9);
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_4 = (rb_state14.var_4) - (0xddb50348);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x10abc9)) ? (rb_state15.var_1) : (0xf75888e7));
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_2 = (rb_state4.var_2) - ((0x536185a3) + (rb_state4.var_3));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_0) == (0x6f2a9712)) ? (rb_state11.var_2) : (0x1df3b7a5));
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_2) == (0x839f804d)) ? (rb_state4.var_1) : (0x1d833891));
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_2);
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_4 = (rb_state14.var_4) ^ (((rb_state14.var_1) == (0xffffc540)) ? (rb_state14.var_4) : (0xf602a361));
                    #endif
                    SIZE(d) = leftover;
                } else {

                    if (prev) {
                        NEXTFREE(prev) = NEXTFREE(d);
                    } else {
                        mem_pool[home].freeBlock[MAXFAST] = NEXTFREE(d);
                    }

                    if (leftover > 0) {
                        SIZE(d) = leftover;
                        MyFreeNow(d);
                    }
                }
                break;
            }

            prev = d;
            d = NEXTFREE(d);
        }

        { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
    }

    if (result) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_0) == (0x7bbd7bb8)) ? (rb_state9.var_3) : (0xf0fce9fd));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + ((0xf5691e04) ^ (0xb7476907));
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_0);
        #endif
        NEXTFREE(result) = 0;
    } else {

        block_size = max(alloc_size, 4 * (1 << MAXFAST));
        { pthread_mutex_lock(&(Global->memLock)); };
        freespace = (unsigned *)malloc(block_size + 2 * sizeof(unsigned));
        ;
        MigrateMem(freespace, block_size + 2 * sizeof(unsigned), home);

        mem_pool[home].touched++;
        { pthread_mutex_unlock(&(Global->memLock)); };
        freespace += 2;
        SIZE(freespace) = block_size;
        HOME(freespace) = home;
        for (i = 0; i < block_size / sizeof(unsigned); i++) {
            freespace[i] = 0;
        }
        if (block_size == alloc_size) {
            result = freespace;
        } else {
            mem_pool[home].tally += SIZE(freespace);
            if (mem_pool[home].tally > mem_pool[home].maxm) {
                mem_pool[home].maxm = mem_pool[home].tally;
            }
            MyFree(freespace);
            result = (unsigned *)MyMalloc(alloc_size, home);
        }
    }

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_2) == (0xc813fc1d)) {
        if ((rb_state3.var_5) != (0x0)) {
            if (!((rb_state3.var_5) == (rb_state3.var_24))) {
                pthread_mutex_lock(&(rb_state3.lock_60));
                rb_state3.var_44 = (rb_state3.var_44) ^ (((rb_state3.var_28) == (0x0)) ? (rb_state3.var_27) : (0x4f6f77fc));
                rb_state3.var_45 = (rb_state3.var_45) + (0x83210d83);
                rb_state3.var_46 = (rb_state3.var_46) + (((rb_state3.var_26) == (0x0)) ? (rb_state3.var_25) : (0x757cfe94));
                rb_state3.var_47 = (rb_state3.var_47) ^ (((rb_state3.var_30) == (0x0)) ? (rb_state3.var_24) : (0x5c7d6990));
                rb_state3.var_48 = (rb_state3.var_48) ^ (((rb_state3.var_31) == (0x0)) ? (rb_state3.var_44) : (0x3027a598));
                rb_state3.var_49 = (rb_state3.var_49) + (((rb_state3.var_45) == (0x0)) ? (rb_state3.var_46) : (0x12b5d33a));
                rb_state3.var_50 = (rb_state3.var_50) ^ (((rb_state3.var_32) == (0x0)) ? (rb_state3.var_47) : (0xaaa6f9c5));
                rb_state3.var_51 = (rb_state3.var_51) ^ (((rb_state3.var_48) == (0x0)) ? (rb_state3.var_49) : (0xec96457f));
                rb_state3.var_52 = (rb_state3.var_52) ^ (((0xe6db2385) + (rb_state3.var_33)) - (rb_state3.var_50));
                rb_state3.var_53 = (rb_state3.var_53) + (((0xa2a65a34) + (0x2af12293)) + (rb_state3.var_51));
                rb_state3.var_54 = (rb_state3.var_54) + (((rb_state3.var_34) == (0x0)) ? (rb_state3.var_52) : (0xf2080f01));
                rb_state3.var_55 = (rb_state3.var_55) - (((rb_state3.var_35) == (0x0)) ? (rb_state3.var_53) : (0x40389252));
                rb_state3.var_56 = (rb_state3.var_56) ^ (((rb_state3.var_36) == (0x0)) ? (rb_state3.var_54) : (0x6b5012e2));
                rb_state3.var_57 = (rb_state3.var_57) - (((0x9cb8625) - (rb_state3.var_55)) + (0x9b7075b8));
                rb_state3.var_58 = (rb_state3.var_58) + (((rb_state3.var_37) == (0x0)) ? (rb_state3.var_56) : (0x49825359));
                rb_state3.var_59 = (rb_state3.var_59) - (((((0xf15a4d77) - (rb_state3.var_38)) + (rb_state3.var_58)) + (rb_state3.var_57)) ^ (rb_state3.var_39));
                rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_59);
                pthread_mutex_unlock(&(rb_state3.lock_60));
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_1);
    #endif
    mem_pool[home].tally += SIZE(result);
    if (mem_pool[home].tally > mem_pool[home].maxm) {
        mem_pool[home].maxm = mem_pool[home].tally;
    }

    if (SIZE(result) < size) {
        printf("*** Bad size from malloc %d, %d\n", size, SIZE(result));
    }

    return ((char *)result);
}

MigrateMem(start, length, home) unsigned int *start;
int length, home;
{
    unsigned int *finish;
    unsigned int currpage, endpage;
    int i;
    int j;
}

MyFree(block) unsigned *block;
{
    int home;

    home = HOME(block);
    { pthread_mutex_lock(&(mem_pool[home].memoryLock)); }
    MyFreeNow(block);
    { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
}

MyFreeNow(block) unsigned *block;
{
    int bucket, size, home;
    extern int MyNum;

    size = SIZE(block);
    home = HOME(block);

    if (size <= 0) {
        printf("Bad size %d\n", size);
        exit(-1);
    }
    if (home < -1 || home >= mallocP) {
        printf("Bad home %d\n", home);
        exit(-1);
    }

    if (size > MAXFASTBL) {
        bucket = MAXFAST;
    } else {
        bucket = FindBucket(size);
        if (size < 1 << bucket) {
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_3) == (0x0)) {
                pthread_mutex_lock(&(rb_state16.lock_28));
                rb_state16.var_6 = 0x1bc0adc8;
                rb_state16.var_7 = 0xbd2c427d;
                rb_state16.var_8 = (rb_state16.var_8) ^ (((((0x3a17eea7) - (rb_state16.var_7)) - (rb_state16.var_5)) + (rb_state16.var_4)) + (rb_state16.var_6));
                rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_8) == (0x98ac59f2)) ? (rb_state16.var_7) : (0x3bffec74));
                rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_6) == (0x1bc0adc8)) ? (rb_state16.var_8) : (0xdf3e7e85));
                rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_9) == (0xbd2c427d)) ? (rb_state16.var_9) : (0x4aa99328));
                rb_state16.var_12 = (rb_state16.var_12) ^ (rb_state16.var_10);
                rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_11);
                rb_state16.var_14 = (rb_state16.var_14) + (((0x95758988) ^ (rb_state16.var_12)) ^ (0x6391bf35));
                rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_13) == (0xbd2c427d)) ? (rb_state16.var_14) : (0xd16b12ac));
                rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_15);
                rb_state16.var_4 = rb_state16.var_5;
                pthread_mutex_unlock(&(rb_state16.lock_28));
            }
            if ((rb_state16.var_3) == (0x0)) {
                if (!((rb_state16.var_4) == (rb_state16.var_5))) {
                    racebench_trigger(16);
                }
            }
            #endif
            bucket--;
        }
    }

    if (bucket == 0) {
        return;
    }

    NEXTFREE(block) = (unsigned *)mem_pool[home].freeBlock[bucket];
    mem_pool[home].freeBlock[bucket] = block;
    mem_pool[home].tally -= size;
}