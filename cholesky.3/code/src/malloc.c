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
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_1) == (0x5bc7fdb4)) {
            pthread_mutex_lock(&(rb_state4.lock_21));
            rb_state4.var_11 = 0x33fa17a5;
            rb_state4.var_12 = 0x6b4325e6;
            rb_state4.var_13 = (rb_state4.var_13) ^ (rb_state4.var_11);
            rb_state4.var_14 = (rb_state4.var_14) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_12) : (0xcc33d05a));
            rb_state4.var_15 = (rb_state4.var_15) - (rb_state4.var_11);
            rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_10);
            rb_state4.var_17 = (rb_state4.var_17) + (((((0x274a41d3) - (rb_state4.var_14)) ^ (rb_state4.var_13)) - (rb_state4.var_13)) ^ (rb_state4.var_14));
            rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0x84e6ea93));
            rb_state4.var_19 = (rb_state4.var_19) - (((0x341420f7) - (0x5698bc73)) - (rb_state4.var_17));
            rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_19) : (0xf339a4f4));
            rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_20);
            pthread_mutex_unlock(&(rb_state4.lock_21));
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (0xdf4e6608);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_2) == (0xb3240d18)) {
            pthread_mutex_lock(&(rb_state19.lock_22));
            rb_state19.var_8 = 0x46cf7a70;
            rb_state19.var_9 = 0xeceaed3f;
            rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_9) : (0x961268fe));
            rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_5) : (0x24206c91));
            rb_state19.var_12 = (rb_state19.var_12) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_8) : (0xc6ab55f5));
            rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0xf6a9160));
            rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0x6aebeb5c));
            rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_13) : (0xd3bac737));
            rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0x15a4375c));
            rb_state19.var_7 = (rb_state19.var_7) + (rb_state19.var_16);
            rb_state19.var_3 = rb_state19.var_7;
            pthread_mutex_unlock(&(rb_state19.lock_22));
        }
        if ((rb_state19.var_2) == (0xb3240d18)) {
            pthread_mutex_lock(&(rb_state19.lock_22));
            rb_state19.var_18 = 0x561d92b4;
            rb_state19.var_19 = 0x68203dac;
            rb_state19.var_20 = (rb_state19.var_20) - (((((0xb18bc4bf) ^ (0x399cdfed)) ^ (0xab78fe61)) + (rb_state19.var_18)) + (rb_state19.var_11));
            rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_20) : (0x6a0371ff));
            rb_state19.var_17 = (rb_state19.var_17) ^ (rb_state19.var_21);
            rb_state19.var_3 = rb_state19.var_17;
            pthread_mutex_unlock(&(rb_state19.lock_22));
        }
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (0xacc9271c);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (0xb5d38b4c);
            rb_state5.var_1 = (rb_state5.var_1) + (66407 < rb_input_size ? (uint32_t)rb_input[66407] : 0xf1bb0f16);
            rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_1);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xa88b240f));
            rb_state6.var_1 = (rb_state6.var_1) + (159951 < rb_input_size ? (uint32_t)rb_input[159951] : 0xa73f19e4);
            rb_state6.var_1 = (rb_state6.var_1) - (0xb9fdf437);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0xed9969ae));
            rb_state7.var_2 = (rb_state7.var_2) ^ (173259 < rb_input_size ? (uint32_t)rb_input[173259] : 0x690ecd99);
            rb_state7.var_2 = (rb_state7.var_2) ^ (0x538165f2);
            rb_state7.var_2 = (rb_state7.var_2) + (9372 < rb_input_size ? (uint32_t)rb_input[9372] : 0xb3501415);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (0x90f56592);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xe808bff3));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
            rb_state14.var_0 = (rb_state14.var_0) + (0xf50a586);
            rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_0);
            if ((rb_state14.var_0) == (0xf50a586)) {
                rb_state14.var_2 = rb_state14.var_1;
            }
            if ((rb_state14.var_0) == (0xf50a586)) {
                if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                    racebench_trigger(14);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_0);
            rb_state15.var_1 = (rb_state15.var_1) + (95001 < rb_input_size ? (uint32_t)rb_input[95001] : 0x371f78fc);
            rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_0);
            if ((rb_state15.var_0) == (0x0)) {
                pthread_mutex_lock(&(rb_state15.lock_23));
                rb_state15.var_10 = (rb_state15.var_10) ^ (rb_state15.var_9);
                rb_state15.var_11 = (rb_state15.var_11) - (((0xcc60d483) - (rb_state15.var_10)) - (rb_state15.var_8));
                rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_3);
                rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_10) : (0x97d80b8));
                rb_state15.var_14 = (rb_state15.var_14) + (rb_state15.var_11);
                rb_state15.var_15 = (rb_state15.var_15) - (((0x306e4ca6) - (rb_state15.var_12)) - (rb_state15.var_12));
                rb_state15.var_16 = (rb_state15.var_16) ^ (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_13) : (0x9476e3bf));
                rb_state15.var_17 = (rb_state15.var_17) + (((((0xc9e39c48) ^ (0xa9e594df)) - (rb_state15.var_14)) - (rb_state15.var_15)) + (0x649ee993));
                rb_state15.var_18 = (rb_state15.var_18) + (((0x490a9ea2) ^ (rb_state15.var_16)) - (0x18ced132));
                rb_state15.var_19 = (rb_state15.var_19) ^ (((0x7df6502b) - (rb_state15.var_14)) + (rb_state15.var_17));
                rb_state15.var_20 = (rb_state15.var_20) + (((0xef338930) - (rb_state15.var_18)) ^ (0x775947e3));
                rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_19) : (0x463e15e6));
                rb_state15.var_22 = (rb_state15.var_22) ^ (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_21) : (0x5f316ed1));
                rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_22);
                pthread_mutex_unlock(&(rb_state15.lock_23));
            }
            rb_state15.var_16 = (rb_state15.var_16) ^ (12983 < rb_input_size ? (uint32_t)rb_input[12983] : 0xb31fc39a);
            rb_state15.var_17 = (rb_state15.var_17) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_18) : (0x7fb40173));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) ^ (163231 < rb_input_size ? (uint32_t)rb_input[163231] : 0xcab76bae);
            if ((rb_state16.var_2) == (0x0)) {
                pthread_mutex_lock(&(rb_state16.lock_8));
                rb_state16.var_4 = 0x3eda1204;
                rb_state16.var_5 = 0x98a5a0a7;
                rb_state16.var_6 = (rb_state16.var_6) - (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_3) : (0x3ab0be76));
                rb_state16.var_7 = (rb_state16.var_7) - (((((0xdfb9ea2e) + (rb_state16.var_4)) + (rb_state16.var_6)) + (0xfb2d8405)) + (rb_state16.var_5));
                rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_7);
                pthread_mutex_unlock(&(rb_state16.lock_8));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + ((0x4a00542f) ^ (0xaf352386));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) + ((0xe0bb3e37) - (rb_state18.var_0));
            rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_1);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) - (0xa4bc653a);
            if ((rb_state19.var_2) == (0xb3240d18)) {
                if ((rb_state19.var_3) != (0x0)) {
                    if (!((rb_state19.var_3) == (rb_state19.var_17))) {
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

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x0)) {
        rb_state2.var_15 = 0xfaecf682;
        rb_state2.var_16 = 0x9665965c;
        rb_state2.var_17 = (rb_state2.var_17) - (0x6e914fa7);
        rb_state2.var_18 = (rb_state2.var_18) - (((rb_state2.var_16) == (0x9665965c)) ? (rb_state2.var_9) : (0xfe48897c));
        rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_15) == (0xfaecf682)) ? (rb_state2.var_17) : (0x401e35e7));
        rb_state2.var_20 = (rb_state2.var_20) - (((0xbcfb3097) ^ (rb_state2.var_18)) - (0xf768ef09));
        rb_state2.var_21 = (rb_state2.var_21) ^ (((((0xd7884d0c) - (0x76a2b9d9)) + (rb_state2.var_19)) + (rb_state2.var_20)) + (rb_state2.var_10));
        rb_state2.var_14 = (rb_state2.var_14) ^ (rb_state2.var_21);
        rb_state2.var_3 = rb_state2.var_14;
    }
    if ((rb_state2.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_34));
        rb_state2.var_23 = 0xb4ca332d;
        rb_state2.var_24 = 0x31bee2bd;
        rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_12) : (0x25dd0642));
        rb_state2.var_26 = (rb_state2.var_26) - (((rb_state2.var_24) == (0x31bee2bd)) ? (rb_state2.var_23) : (0x7bad874d));
        rb_state2.var_27 = (rb_state2.var_27) - (((0xe047e73f) ^ (rb_state2.var_14)) - (rb_state2.var_11));
        rb_state2.var_28 = (rb_state2.var_28) ^ (((0x7b4cb8d4) + (0xc7f6c122)) ^ (rb_state2.var_25));
        rb_state2.var_29 = (rb_state2.var_29) ^ (rb_state2.var_26);
        rb_state2.var_30 = (rb_state2.var_30) ^ (((rb_state2.var_27) == (0x337a193f)) ? (rb_state2.var_28) : (0xaaceca7));
        rb_state2.var_31 = (rb_state2.var_31) - (((0xc606669d) - (rb_state2.var_29)) + (rb_state2.var_15));
        rb_state2.var_32 = (rb_state2.var_32) - (((rb_state2.var_16) == (0x9665965c)) ? (rb_state2.var_30) : (0xf59724bc));
        rb_state2.var_33 = (rb_state2.var_33) - (((((0xc565d1a9) - (rb_state2.var_17)) ^ (rb_state2.var_18)) - (rb_state2.var_31)) - (rb_state2.var_32));
        rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_33);
        rb_state2.var_3 = rb_state2.var_22;
        pthread_mutex_unlock(&(rb_state2.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x8bcd7293));
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
        rb_state12.var_0 = (rb_state12.var_0) + ((0xc8664aed) - (rb_state12.var_1));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0xf50a586)) {
            rb_state14.var_3 = 0xc470f461;
            rb_state14.var_4 = (rb_state14.var_4) ^ (0x57546568);
            rb_state14.var_5 = (rb_state14.var_5) ^ (rb_state14.var_3);
            rb_state14.var_6 = (rb_state14.var_6) + (174414 < rb_input_size ? (uint32_t)rb_input[174414] : 0x50763f57);
            rb_state14.var_7 = (rb_state14.var_7) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_2) : (0x3720d53b));
            rb_state14.var_8 = (rb_state14.var_8) + (((0x7ba146df) ^ (rb_state14.var_3)) - (rb_state14.var_5));
            rb_state14.var_9 = (rb_state14.var_9) ^ (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_4) : (0xa511eb2f));
            rb_state14.var_10 = (rb_state14.var_10) - (((0x7b61f3c2) + (rb_state14.var_5)) + (rb_state14.var_7));
            rb_state14.var_11 = (rb_state14.var_11) ^ (((((0xe1940635) + (rb_state14.var_6)) ^ (rb_state14.var_8)) ^ (0x6d6c2a01)) + (rb_state14.var_7));
            rb_state14.var_12 = (rb_state14.var_12) ^ (((((0x4dde74f1) + (rb_state14.var_9)) ^ (rb_state14.var_9)) - (rb_state14.var_8)) ^ (0xaaa91066));
            rb_state14.var_13 = (rb_state14.var_13) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_10) : (0x197c6cf9));
            rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_11) : (0x2abe064d));
            rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_12) : (0x30debe9f));
            rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_14) : (0xacd4fe00));
            rb_state14.var_17 = (rb_state14.var_17) - (((((0x47f897f5) ^ (rb_state14.var_15)) - (rb_state14.var_13)) + (rb_state14.var_16)) + (rb_state14.var_14));
            rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_17);
        }
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_1);
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
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_23));
            rb_state15.var_5 = 0xa491754f;
            rb_state15.var_6 = 0x42371e95;
            rb_state15.var_7 = (rb_state15.var_7) - (((((0xbd200b3b) - (rb_state15.var_5)) + (rb_state15.var_4)) ^ (rb_state15.var_5)) + (rb_state15.var_3));
            rb_state15.var_8 = (rb_state15.var_8) - (((0x85f2fc53) - (rb_state15.var_6)) - (rb_state15.var_6));
            rb_state15.var_9 = (rb_state15.var_9) - (((((0x4214d7df) - (rb_state15.var_8)) + (rb_state15.var_7)) - (rb_state15.var_7)) - (0x1a3e9d90));
            rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_9);
            rb_state15.var_3 = rb_state15.var_4;
            pthread_mutex_unlock(&(rb_state15.lock_23));
        }
        if ((rb_state15.var_0) == (0x0)) {
            rb_state15.var_24 = !((rb_state15.var_3) == (rb_state15.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_2);
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - ((0xe4577832) - (rb_state5.var_0));
        rb_state5.var_2 = (rb_state5.var_2) ^ ((0xa0c3b812) - (0x549a1225));
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_2) == (0xeed879ac)) {
            rb_state12.var_3 = rb_state12.var_0;
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_20 = (rb_state15.var_20) - ((0xf989d0fb) - (0x4994fd32));
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0xa1771d3e)) {
                rb_state7.var_4 = 0x8a5a9aa1;
                rb_state7.var_5 = 0x88c0575c;
                rb_state7.var_6 = (rb_state7.var_6) - (rb_state7.var_4);
                rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_5);
                rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x6faba015));
                rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_4) : (0xdb2b67bb));
                rb_state7.var_10 = (rb_state7.var_10) ^ (((0x70710a93) ^ (0x2e3d09e9)) + (rb_state7.var_3));
                rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_6) : (0xadb1b956));
                rb_state7.var_12 = (rb_state7.var_12) + (rb_state7.var_7);
                rb_state7.var_13 = (rb_state7.var_13) ^ (((0x40ba856a) + (0x71e504ff)) ^ (rb_state7.var_8));
                rb_state7.var_14 = (rb_state7.var_14) + (rb_state7.var_9);
                rb_state7.var_15 = (rb_state7.var_15) + (((0x9d231e6c) ^ (rb_state7.var_9)) + (rb_state7.var_10));
                rb_state7.var_16 = (rb_state7.var_16) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0x1863c0c));
                rb_state7.var_17 = (rb_state7.var_17) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_13) : (0x2ab09651));
                rb_state7.var_18 = (rb_state7.var_18) - (((((0xb1143471) + (rb_state7.var_12)) ^ (rb_state7.var_14)) ^ (rb_state7.var_11)) - (rb_state7.var_15));
                rb_state7.var_19 = (rb_state7.var_19) - (rb_state7.var_16);
                rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_17) : (0x473bbeb));
                rb_state7.var_21 = (rb_state7.var_21) + (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_19) : (0x89b62bde));
                rb_state7.var_22 = (rb_state7.var_22) + (rb_state7.var_20);
                rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_21) : (0xdbf3ea96));
                rb_state7.var_24 = (rb_state7.var_24) - (rb_state7.var_22);
                rb_state7.var_25 = (rb_state7.var_25) + (((0xb6a66f98) ^ (rb_state7.var_23)) - (0x3d0da86));
                rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_24) : (0xf3d02e59));
                rb_state7.var_27 = (rb_state7.var_27) ^ (rb_state7.var_25);
                rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_27) : (0x59654326));
                rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_28);
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_2) == (0xeed879ac)) {
                if (!((rb_state12.var_0) == (rb_state12.var_3))) {
                    racebench_trigger(12);
                }
            }
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_2);
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) - (0xb2ecdffa);
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_1 = (rb_state18.var_1) + (150113 < rb_input_size ? (uint32_t)rb_input[150113] : 0xced5e7ff);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_2);
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_1 = (rb_state14.var_1) - (117320 < rb_input_size ? (uint32_t)rb_input[117320] : 0xe49a64ff);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_0);
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_1) : (0xa6138ea5));
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_1 = (rb_state14.var_1) ^ (40512 < rb_input_size ? (uint32_t)rb_input[40512] : 0x40540635);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) - (0xfb848e44);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) - (0xcdc58c6f);
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_2 = (rb_state6.var_2) + (0x140dfc77);
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) + (0x92fcf675);
                    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_2) : (0x77f01309));
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
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - (120997 < rb_input_size ? (uint32_t)rb_input[120997] : 0x7be270ed);
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xa1771d3e)) {
            rb_state7.var_3 = rb_state7.var_1;
        }
        if ((rb_state7.var_0) == (0xa1771d3e)) {
            pthread_mutex_lock(&(rb_state7.lock_29));
            if (!((rb_state7.var_1) == (rb_state7.var_3))) {
                racebench_trigger(7);
            }
            pthread_mutex_unlock(&(rb_state7.lock_29));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_2) == (0xeed879ac)) {
            pthread_mutex_lock(&(rb_state12.lock_4));
            rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_3);
            pthread_mutex_unlock(&(rb_state12.lock_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_2) == (0x0)) {
            rb_state18.var_4 = 0x846b6e9c;
            rb_state18.var_5 = (rb_state18.var_5) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_3) : (0x5179d075));
            rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0x664d317f));
            rb_state18.var_7 = (rb_state18.var_7) - (rb_state18.var_5);
            rb_state18.var_8 = (rb_state18.var_8) - (((0x69aa38d5) - (rb_state18.var_7)) - (rb_state18.var_6));
            rb_state18.var_9 = (rb_state18.var_9) - (((((0x6e60c8b5) - (0x26558d10)) - (0xdd095ce3)) + (rb_state18.var_7)) ^ (rb_state18.var_8));
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_9);
        }
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
    if ((rb_state18.var_2) == (0x0)) {
        rb_state18.var_3 = rb_state18.var_0;
    }
    if ((rb_state18.var_2) == (0x0)) {
        if (!((rb_state18.var_0) == (rb_state18.var_3))) {
            racebench_trigger(18);
        }
    }
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