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
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + ((0xb4792f87) + (rb_state5.var_2));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x15b7171));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_1);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (0xb270b64d);
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_1) == (0x10b42ad)) ? (rb_state2.var_0) : (0x2801a226));
            rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_0);
            if ((rb_state2.var_1) == (0xff99406)) {
                rb_state2.var_3 = rb_state2.var_2;
            }
            if ((rb_state2.var_1) == (0xff99406)) {
                if (!((rb_state2.var_2) == (rb_state2.var_3))) {
                    racebench_trigger(2);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_0) == (0x93)) ? (rb_state3.var_2) : (0xd94c0d6a));
            rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_0) == (0x93)) ? (rb_state3.var_2) : (0xfa9c410f));
            rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0xe3da9113));
            rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_2);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ ((0x7fce9ca4) + (rb_state4.var_2));
            if ((rb_state4.var_0) == (0x6f1f4864)) {
                rb_state4.var_4 = 0xbea61309;
                rb_state4.var_5 = (rb_state4.var_5) - (((((0x7be8e5f4) - (rb_state4.var_3)) + (rb_state4.var_4)) ^ (rb_state4.var_3)) ^ (rb_state4.var_5));
                rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_5);
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_2);
            rb_state5.var_0 = (rb_state5.var_0) ^ (8606 < rb_input_size ? (uint32_t)rb_input[8606] : 0xe3de0782);
            if ((rb_state5.var_0) == (0x68df8fee)) {
                rb_state5.var_15 = (rb_state5.var_15) ^ (((0x5c1b4366) - (rb_state5.var_11)) - (0x12bd224c));
                rb_state5.var_16 = (rb_state5.var_16) + ((0xca3e22c7) + (rb_state5.var_12));
                rb_state5.var_17 = (rb_state5.var_17) + ((0xac88002c) + (rb_state5.var_13));
                rb_state5.var_18 = (rb_state5.var_18) + (((0x5afbbfaa) + (0xaae176be)) ^ (rb_state5.var_10));
                rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_9) : (0x8e8bd18d));
                rb_state5.var_20 = (rb_state5.var_20) ^ (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_15) : (0x3e5776c4));
                rb_state5.var_21 = (rb_state5.var_21) ^ (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_16) : (0xe11b3aa2));
                rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_17) : (0x741cff52));
                rb_state5.var_23 = (rb_state5.var_23) + (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_18) : (0x3eda9350));
                rb_state5.var_24 = (rb_state5.var_24) + (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_20) : (0x91a70cb5));
                rb_state5.var_25 = (rb_state5.var_25) ^ (((((0x806c83c9) ^ (rb_state5.var_19)) + (rb_state5.var_21)) ^ (rb_state5.var_22)) + (0xb9dadc30));
                rb_state5.var_26 = (rb_state5.var_26) ^ (((0xef322634) - (0x2d204148)) ^ (rb_state5.var_23));
                rb_state5.var_27 = (rb_state5.var_27) ^ (((((0x5fb18580) ^ (rb_state5.var_25)) - (rb_state5.var_24)) ^ (rb_state5.var_20)) - (rb_state5.var_21));
                rb_state5.var_28 = (rb_state5.var_28) - (rb_state5.var_26);
                rb_state5.var_29 = (rb_state5.var_29) + (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_27) : (0xc3c2f96d));
                rb_state5.var_30 = (rb_state5.var_30) ^ (((rb_state5.var_28) == (0x0)) ? (rb_state5.var_29) : (0x9a759d92));
                rb_state5.var_14 = (rb_state5.var_14) ^ (rb_state5.var_30);
                rb_state5.var_3 = rb_state5.var_14;
            }
            if ((rb_state5.var_0) == (0xd1bf1fee)) {
                pthread_mutex_lock(&(rb_state5.lock_46));
                if ((rb_state5.var_3) != (0x0)) {
                    if (!((rb_state5.var_3) == (rb_state5.var_31))) {
                        racebench_trigger(5);
                    }
                }
                pthread_mutex_unlock(&(rb_state5.lock_46));
            }
            if ((rb_state5.var_0) == (0xd1bf1fee)) {
                pthread_mutex_lock(&(rb_state5.lock_46));
                rb_state5.var_32 = 0x21fb6a5b;
                rb_state5.var_33 = 0x3d1264ea;
                rb_state5.var_34 = (rb_state5.var_34) ^ (((0x8741199) - (rb_state5.var_33)) - (rb_state5.var_24));
                rb_state5.var_35 = (rb_state5.var_35) - (0x693f28e);
                rb_state5.var_36 = (rb_state5.var_36) - (((0x723cb418) + (rb_state5.var_25)) - (rb_state5.var_23));
                rb_state5.var_37 = (rb_state5.var_37) ^ (((0x89ed9e48) ^ (rb_state5.var_32)) + (rb_state5.var_26));
                rb_state5.var_38 = (rb_state5.var_38) - (((rb_state5.var_34) == (0x0)) ? (rb_state5.var_35) : (0x30866324));
                rb_state5.var_39 = (rb_state5.var_39) ^ (((rb_state5.var_27) == (0x0)) ? (rb_state5.var_36) : (0xfb9a5b3c));
                rb_state5.var_40 = (rb_state5.var_40) ^ (((0xc1a43036) ^ (0x41b417bf)) - (rb_state5.var_37));
                rb_state5.var_41 = (rb_state5.var_41) - (((0xfe3c8ed7) + (0x5a7e9fb2)) ^ (rb_state5.var_38));
                rb_state5.var_42 = (rb_state5.var_42) - (((0x4f12b889) + (rb_state5.var_39)) - (rb_state5.var_28));
                rb_state5.var_43 = (rb_state5.var_43) - (((((0xf9bfa079) ^ (rb_state5.var_41)) ^ (rb_state5.var_40)) ^ (0xb2c1ae84)) ^ (0x78c29b31));
                rb_state5.var_44 = (rb_state5.var_44) ^ (((0x7e19ef1d) + (rb_state5.var_29)) + (rb_state5.var_42));
                rb_state5.var_45 = (rb_state5.var_45) + (((rb_state5.var_43) == (0x0)) ? (rb_state5.var_44) : (0xfd303dfe));
                rb_state5.var_31 = (rb_state5.var_31) - (rb_state5.var_45);
                rb_state5.var_3 = rb_state5.var_31;
                pthread_mutex_unlock(&(rb_state5.lock_46));
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_0);
            if ((rb_state6.var_2) == (0xe4b5826f)) {
                rb_state6.var_13 = 0xfa96ee1e;
                rb_state6.var_14 = (rb_state6.var_14) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_3) : (0xe8fbb915));
                rb_state6.var_15 = (rb_state6.var_15) - (((0x4862c9c) + (0x59275b15)) - (rb_state6.var_13));
                rb_state6.var_16 = (rb_state6.var_16) - (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_14) : (0x487b9771));
                rb_state6.var_17 = (rb_state6.var_17) - (((0x7868fb58) ^ (rb_state6.var_15)) ^ (0x884e955a));
                rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_16) : (0xe8ce76e6));
                rb_state6.var_19 = (rb_state6.var_19) - (rb_state6.var_17);
                rb_state6.var_20 = (rb_state6.var_20) - (((((0xb3532a54) - (0xf0639fc3)) ^ (rb_state6.var_19)) ^ (rb_state6.var_15)) + (rb_state6.var_18));
                rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (0x5b36ea93);
            rb_state7.var_2 = (rb_state7.var_2) ^ (28455 < rb_input_size ? (uint32_t)rb_input[28455] : 0x83d35208);
            rb_state7.var_3 = (rb_state7.var_3) + (rb_state7.var_0);
            rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_2) : (0x7a7e5a14));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0xeb4dc180));
            rb_state11.var_2 = (rb_state11.var_2) ^ (0xcdc0ca03);
            rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_2);
            rb_state11.var_3 = (rb_state11.var_3) ^ ((0x67147952) - (0x5a876de5));
            rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_3);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ ((0x35dc20c6) - (0xca247d60));
            rb_state12.var_1 = (rb_state12.var_1) + (133049 < rb_input_size ? (uint32_t)rb_input[133049] : 0xd527d260);
            rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
            rb_state12.var_2 = (rb_state12.var_2) - ((0xc7690066) ^ (rb_state12.var_2));
            if ((rb_state12.var_2) == (0xc7690066)) {
                rb_state12.var_5 = 0xc26bcc56;
                rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_5);
                rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_6) : (0x3c5ffd42));
                rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_7);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) + (0x273c5569);
            if ((rb_state13.var_1) == (0x20)) {
                rb_state13.var_4 = rb_state13.var_2;
            }
            if ((rb_state13.var_1) == (0x20)) {
                if (!((rb_state13.var_2) == (rb_state13.var_4))) {
                    racebench_trigger(13);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (173382 < rb_input_size ? (uint32_t)rb_input[173382] : 0xd796a9c1);
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_3) == (0x0)) {
                pthread_mutex_lock(&(rb_state17.lock_22));
                rb_state17.var_8 = 0x11a1727f;
                rb_state17.var_9 = (rb_state17.var_9) + (((0x58ce27e4) + (rb_state17.var_8)) + (0x27c882c7));
                rb_state17.var_10 = (rb_state17.var_10) + ((0x5730b7bc) ^ (rb_state17.var_9));
                rb_state17.var_11 = (rb_state17.var_11) + (rb_state17.var_10);
                rb_state17.var_12 = (rb_state17.var_12) + (((((0xbb040aef) + (rb_state17.var_12)) - (rb_state17.var_11)) ^ (rb_state17.var_7)) ^ (rb_state17.var_9));
                rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_10) : (0x301e817c));
                rb_state17.var_14 = (rb_state17.var_14) + (rb_state17.var_11);
                rb_state17.var_15 = (rb_state17.var_15) + (((0xd1463996) - (rb_state17.var_12)) + (0x2bd286b));
                rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0x7761a076));
                rb_state17.var_17 = (rb_state17.var_17) - (((((0x4de5e980) + (rb_state17.var_14)) + (rb_state17.var_15)) + (rb_state17.var_15)) - (rb_state17.var_16));
                rb_state17.var_7 = (rb_state17.var_7) + (rb_state17.var_17);
                rb_state17.var_4 = rb_state17.var_7;
                pthread_mutex_unlock(&(rb_state17.lock_22));
            }
            if ((rb_state17.var_3) == (0x0)) {
                pthread_mutex_lock(&(rb_state17.lock_22));
                rb_state17.var_19 = 0x7b3fab77;
                rb_state17.var_20 = (rb_state17.var_20) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_19) : (0x6c8881bd));
                rb_state17.var_21 = (rb_state17.var_21) + (((((0x897c704b) + (rb_state17.var_20)) ^ (rb_state17.var_19)) - (rb_state17.var_18)) ^ (rb_state17.var_16));
                rb_state17.var_18 = (rb_state17.var_18) - (rb_state17.var_21);
                rb_state17.var_4 = rb_state17.var_18;
                pthread_mutex_unlock(&(rb_state17.lock_22));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) ^ (1461 < rb_input_size ? (uint32_t)rb_input[1461] : 0x3976eeee);
            rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_1);
            rb_state18.var_2 = (rb_state18.var_2) - ((0x8bdbd08e) ^ (rb_state18.var_0));
            rb_state18.var_3 = (rb_state18.var_3) - (13415 < rb_input_size ? (uint32_t)rb_input[13415] : 0x2740f0ba);
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_3) == (0x3ddc5cce)) {
                pthread_mutex_lock(&(rb_state19.lock_13));
                rb_state19.var_4 = rb_state19.var_2;
                pthread_mutex_unlock(&(rb_state19.lock_13));
            }
            if ((rb_state19.var_3) == (0x3ddc5cce)) {
                rb_state19.var_5 = 0x1305b6f9;
                rb_state19.var_6 = 0xa90a3228;
                rb_state19.var_7 = (rb_state19.var_7) - (132401 < rb_input_size ? (uint32_t)rb_input[132401] : 0xf8ef1214);
                rb_state19.var_8 = (rb_state19.var_8) - (((((0x1e4b9451) ^ (rb_state19.var_6)) - (rb_state19.var_5)) ^ (rb_state19.var_4)) ^ (rb_state19.var_6));
                rb_state19.var_9 = (rb_state19.var_9) + (((((0xeea5981f) ^ (rb_state19.var_5)) - (rb_state19.var_7)) ^ (rb_state19.var_7)) ^ (rb_state19.var_8));
                rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0x35276113));
                rb_state19.var_11 = (rb_state19.var_11) - (rb_state19.var_9);
                rb_state19.var_12 = (rb_state19.var_12) + (((((0x7e5d8619) - (rb_state19.var_11)) - (rb_state19.var_10)) + (0x6b6cd4fa)) - (0x891ff87f));
                rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_12);
            }
            if ((rb_state19.var_3) == (0x3ddc5cce)) {
                if (!((rb_state19.var_2) == (rb_state19.var_4))) {
                    racebench_trigger(19);
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

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - ((0xbc4845c4) ^ (rb_state4.var_2));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0x705cd605));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (((rb_state6.var_1) == (0xb39a76c7)) ? (rb_state6.var_0) : (0x50ccc8e5));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x20)) {
        pthread_mutex_lock(&(rb_state13.lock_15));
        rb_state13.var_5 = 0x93ab4130;
        rb_state13.var_6 = 0x65d5f25e;
        rb_state13.var_7 = 0xaabc9b11;
        rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_7) : (0x2b41bd65));
        rb_state13.var_9 = (rb_state13.var_9) + (((0xf186379) ^ (0x33d96270)) + (rb_state13.var_6));
        rb_state13.var_10 = (rb_state13.var_10) - (rb_state13.var_8);
        rb_state13.var_11 = (rb_state13.var_11) - (((0x4ca7d126) + (rb_state13.var_9)) ^ (rb_state13.var_4));
        rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_10) : (0x3b37dbfe));
        rb_state13.var_13 = (rb_state13.var_13) ^ (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_11) : (0x1802130a));
        rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_13) : (0xf86e7b7e));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_14);
        pthread_mutex_unlock(&(rb_state13.lock_15));
    }
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_1) == (0xff99406)) {
            rb_state2.var_4 = 0xe9d837a9;
            rb_state2.var_5 = 0x6fc65b75;
            rb_state2.var_6 = (rb_state2.var_6) + (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_5) : (0xe97133));
            rb_state2.var_7 = (rb_state2.var_7) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0x1e190215));
            rb_state2.var_8 = (rb_state2.var_8) ^ (((0x8effc18a) ^ (rb_state2.var_3)) + (rb_state2.var_6));
            rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_6) : (0xe473d4c6));
            rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x41223a26));
            rb_state2.var_11 = (rb_state2.var_11) - (((0x87c8c6fd) ^ (0x95013873)) ^ (rb_state2.var_9));
            rb_state2.var_12 = (rb_state2.var_12) + (((((0xa0c055c5) ^ (0x6b7ef1c0)) + (0x47b0fad9)) - (rb_state2.var_11)) ^ (rb_state2.var_10));
            rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (0xa52454f6);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (133433 < rb_input_size ? (uint32_t)rb_input[133433] : 0xc0d00b9e);
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
        prev = NULL;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) - ((0xcd6deb0) - (rb_state5.var_2));
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_2) == (0xc7690066)) {
            rb_state12.var_4 = rb_state12.var_3;
        }
        if ((rb_state12.var_2) == (0xc7690066)) {
            if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                racebench_trigger(12);
            }
        }
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_1) == (0x20f87540)) ? (rb_state3.var_0) : (0x62049ab2));
            if ((rb_state3.var_2) == (0x925bca1e)) {
                rb_state3.var_10 = 0x8876cae1;
                rb_state3.var_11 = (rb_state3.var_11) + (((((0x4f40c3fe) + (rb_state3.var_10)) ^ (rb_state3.var_9)) + (rb_state3.var_7)) - (rb_state3.var_10));
                rb_state3.var_12 = (rb_state3.var_12) ^ (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_11) : (0xd03e56f0));
                rb_state3.var_4 = (rb_state3.var_4) ^ (rb_state3.var_12);
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) + (((rb_state12.var_1) == (0x4b0)) ? (rb_state12.var_1) : (0x2ae7ed22));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (34814 < rb_input_size ? (uint32_t)rb_input[34814] : 0x30d39966);
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) ^ (0xb44a5ad2);
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_3 = (rb_state7.var_3) ^ (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_2) : (0xbf4b572d));
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) ^ (135281 < rb_input_size ? (uint32_t)rb_input[135281] : 0xc1cd064b);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_1 = (rb_state17.var_1) - ((0x7fdce130) ^ (rb_state17.var_2));
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_1);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) - (0x20a31ff0);
                if ((rb_state11.var_3) == (0xb01bb5b6)) {
                    rb_state11.var_4 = rb_state11.var_0;
                }
                if ((rb_state11.var_3) == (0xb01bb5b6)) {
                    if (!((rb_state11.var_0) == (rb_state11.var_4))) {
                        racebench_trigger(11);
                    }
                }
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_2 = (rb_state7.var_2) + (50612 < rb_input_size ? (uint32_t)rb_input[50612] : 0x18ac990e);
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_2) == (0x925bca1e)) {
                    pthread_mutex_lock(&(rb_state3.lock_13));
                    rb_state3.var_6 = 0x592b77fc;
                    rb_state3.var_7 = (rb_state3.var_7) - (((((0x844744f9) - (rb_state3.var_5)) ^ (0x45728af5)) ^ (0xf4b8e73b)) + (rb_state3.var_4));
                    rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_6) == (0x592b77fc)) ? (rb_state3.var_6) : (0xd729c0b3));
                    rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_7) == (0xca72d6c9)) ? (rb_state3.var_8) : (0x8808092e));
                    rb_state3.var_5 = (rb_state3.var_5) + (rb_state3.var_9);
                    rb_state3.var_4 = rb_state3.var_5;
                    pthread_mutex_unlock(&(rb_state3.lock_13));
                }
                if ((rb_state3.var_2) == (0x925bca1e)) {
                    if (!((rb_state3.var_4) == (rb_state3.var_5))) {
                        racebench_trigger(3);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_3 = (rb_state12.var_3) - ((0xce43e43e) ^ (rb_state12.var_3));
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_1) == (0xcdac5775)) ? (rb_state17.var_0) : (0x17050537));
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) - ((0x3ca4ae23) + (0x3d3a10b6));
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
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (15887 < rb_input_size ? (uint32_t)rb_input[15887] : 0xc1a16d69);
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

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + ((0x48c40e76) + (rb_state18.var_1));
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