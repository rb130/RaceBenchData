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
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_0) == (0x528a596c)) {
                rb_state3.var_2 = rb_state3.var_1;
            }
            if ((rb_state3.var_0) == (0x528a596c)) {
                if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                    racebench_trigger(3);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + ((0xd37ae75c) + (rb_state8.var_0));
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xd2b13490)) {
                pthread_mutex_lock(&(rb_state9.lock_13));
                rb_state9.var_3 = 0x503b6fe9;
                rb_state9.var_4 = (rb_state9.var_4) ^ (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0x46bc44ea));
                rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_5);
                rb_state9.var_6 = (rb_state9.var_6) - (0x8bc691fa);
                rb_state9.var_7 = (rb_state9.var_7) - (((0xf1cef948) - (rb_state9.var_6)) ^ (rb_state9.var_2));
                rb_state9.var_8 = (rb_state9.var_8) + (rb_state9.var_4);
                rb_state9.var_9 = (rb_state9.var_9) - (((((0x4f1d4958) ^ (rb_state9.var_8)) ^ (rb_state9.var_5)) ^ (rb_state9.var_6)) - (rb_state9.var_7));
                rb_state9.var_10 = (rb_state9.var_10) - (((((0x7c300403) - (rb_state9.var_9)) ^ (rb_state9.var_10)) ^ (rb_state9.var_7)) - (rb_state9.var_8));
                rb_state9.var_11 = (rb_state9.var_11) ^ (((0x4d6afd07) - (rb_state9.var_9)) ^ (0xa0cf0336));
                rb_state9.var_12 = (rb_state9.var_12) ^ (((((0x3fc5f42e) + (rb_state9.var_10)) + (rb_state9.var_11)) ^ (0x5a6adb54)) - (rb_state9.var_11));
                rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_12);
                pthread_mutex_unlock(&(rb_state9.lock_13));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x12901479));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + ((0xd05afc8e) - (0xfa12f09e));
            if ((rb_state11.var_0) == (0x40791290)) {
                pthread_mutex_lock(&(rb_state11.lock_10));
                rb_state11.var_3 = 0x93d87f63;
                rb_state11.var_4 = 0xafe7cffd;
                rb_state11.var_5 = 0x779a0182;
                rb_state11.var_6 = (rb_state11.var_6) ^ (((0x13154aae) + (0xb5bebc09)) + (rb_state11.var_5));
                rb_state11.var_7 = (rb_state11.var_7) - (((((0x4d072ec4) - (rb_state11.var_3)) + (rb_state11.var_2)) + (rb_state11.var_3)) ^ (rb_state11.var_4));
                rb_state11.var_8 = (rb_state11.var_8) - (((0x44250cb0) ^ (0x7bbeb345)) + (rb_state11.var_6));
                rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x35aaef7e));
                rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_9);
                pthread_mutex_unlock(&(rb_state11.lock_10));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x608e05d8));
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0x9c819fd6)) {
                rb_state16.var_2 = rb_state16.var_1;
            }
            if ((rb_state16.var_0) == (0x9c819fd6)) {
                if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                    racebench_trigger(16);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) ^ ((0xeff77012) - (rb_state17.var_0));
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

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x2437200b)) {
        rb_state0.var_3 = 0x202a9347;
        rb_state0.var_4 = 0xe327e86e;
        rb_state0.var_5 = (rb_state0.var_5) ^ (rb_state0.var_3);
        rb_state0.var_6 = (rb_state0.var_6) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x5b6f15ed));
        rb_state0.var_7 = (rb_state0.var_7) + (rb_state0.var_2);
        rb_state0.var_8 = (rb_state0.var_8) - (rb_state0.var_4);
        rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_3) : (0xc3815e92));
        rb_state0.var_10 = (rb_state0.var_10) - (((0x455e2df9) ^ (rb_state0.var_5)) + (0x6060764b));
        rb_state0.var_11 = (rb_state0.var_11) + (((0xca627845) + (rb_state0.var_7)) - (rb_state0.var_6));
        rb_state0.var_12 = (rb_state0.var_12) ^ (((((0x66d0f51f) + (rb_state0.var_7)) ^ (rb_state0.var_8)) ^ (0xb4d48073)) ^ (rb_state0.var_8));
        rb_state0.var_13 = (rb_state0.var_13) + (rb_state0.var_9);
        rb_state0.var_14 = (rb_state0.var_14) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0x3aa4be81));
        rb_state0.var_15 = (rb_state0.var_15) + (((((0x2fa36ef) - (rb_state0.var_9)) + (rb_state0.var_12)) - (0x8ba06bc3)) ^ (rb_state0.var_13));
        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_14) : (0x621eb68c));
        rb_state0.var_17 = (rb_state0.var_17) + (((0x3d9cc188) - (rb_state0.var_15)) ^ (rb_state0.var_11));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((0xb02571b3) - (rb_state0.var_12)) + (rb_state0.var_16));
        rb_state0.var_19 = (rb_state0.var_19) + (((0x84b1fafa) + (rb_state0.var_17)) - (rb_state0.var_13));
        rb_state0.var_20 = (rb_state0.var_20) + (rb_state0.var_18);
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_19) : (0xdfb7a30e));
        rb_state0.var_22 = (rb_state0.var_22) + (((((0x98cec07) + (rb_state0.var_20)) ^ (rb_state0.var_15)) - (rb_state0.var_21)) - (rb_state0.var_16));
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_37));
        rb_state1.var_3 = 0xeedb6b1a;
        rb_state1.var_4 = 0x707c6495;
        rb_state1.var_5 = (rb_state1.var_5) - (((0xaa13f6f8) - (rb_state1.var_2)) ^ (rb_state1.var_3));
        rb_state1.var_6 = (rb_state1.var_6) + ((0x3631882d) + (rb_state1.var_3));
        rb_state1.var_7 = (rb_state1.var_7) ^ (rb_state1.var_4);
        rb_state1.var_8 = (rb_state1.var_8) + (rb_state1.var_1);
        rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_4) == (0x707c6495)) ? (rb_state1.var_5) : (0x9b2832aa));
        rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_5) == (0xbb37621e)) ? (rb_state1.var_6) : (0xfe148532));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_7);
        rb_state1.var_12 = (rb_state1.var_12) + (((0x1cb12942) ^ (0x29bfe0df)) ^ (rb_state1.var_8));
        rb_state1.var_13 = (rb_state1.var_13) + (((0x220502f0) ^ (0x193c937a)) - (rb_state1.var_9));
        rb_state1.var_14 = (rb_state1.var_14) - (((rb_state1.var_6) == (0x250cf347)) ? (rb_state1.var_10) : (0x6ddaf92d));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_7) == (0x707c6495)) ? (rb_state1.var_11) : (0x6111ef6d));
        rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_12) == (0x350ec99d)) ? (rb_state1.var_13) : (0xde29c09a));
        rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_14) : (0x59c72c68));
        rb_state1.var_18 = (rb_state1.var_18) + (((((0xf8df18ba) + (rb_state1.var_16)) + (rb_state1.var_15)) - (rb_state1.var_10)) - (rb_state1.var_9));
        rb_state1.var_19 = (rb_state1.var_19) + (((0x10478a50) + (rb_state1.var_17)) - (rb_state1.var_11));
        rb_state1.var_20 = (rb_state1.var_20) ^ (((0xf7c2e717) ^ (rb_state1.var_18)) + (rb_state1.var_12));
        rb_state1.var_21 = (rb_state1.var_21) ^ (((0xbc3eaa9c) - (rb_state1.var_19)) ^ (0xb4189cee));
        rb_state1.var_22 = (rb_state1.var_22) - (rb_state1.var_20);
        rb_state1.var_23 = (rb_state1.var_23) + (((0x1876f14e) - (rb_state1.var_13)) + (rb_state1.var_21));
        rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_14) == (0xdaf30cb9)) ? (rb_state1.var_22) : (0x811138b3));
        rb_state1.var_25 = (rb_state1.var_25) ^ (((((0x28fb1ffa) + (rb_state1.var_23)) ^ (0x8ffab502)) ^ (rb_state1.var_15)) ^ (rb_state1.var_24));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_25);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_37));
    }
    if ((rb_state1.var_0) == (0x0)) {
        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x888518a4)) {
        rb_state8.var_2 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0x888518a4)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (161364 < rb_input_size ? (uint32_t)rb_input[161364] : 0x832dc709);
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (132687 < rb_input_size ? (uint32_t)rb_input[132687] : 0x358970f2);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x6a3106a0)) {
        rb_state11.var_2 = rb_state11.var_1;
    }
    if ((rb_state11.var_0) == (0x40791290)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x3d8ed999));
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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + ((0x1a07b6db) - (rb_state0.var_0));
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0xd2b13490)) {
            rb_state9.var_2 = rb_state9.var_1;
        }
        if ((rb_state9.var_0) == (0xd2b13490)) {
            if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                racebench_trigger(9);
            }
        }
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x888518a4)) {
            rb_state8.var_3 = 0x369ff944;
            rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_2);
            rb_state8.var_5 = (rb_state8.var_5) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0x5a317530));
            rb_state8.var_6 = (rb_state8.var_6) - (((0xa652988c) + (rb_state8.var_3)) + (0x29b7c2c8));
            rb_state8.var_7 = (rb_state8.var_7) ^ (rb_state8.var_4);
            rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_6) == (0xf955ab68)) ? (rb_state8.var_5) : (0xa8a30a8c));
            rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_6) : (0xa264c1fd));
            rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0x64194aab));
            rb_state8.var_11 = (rb_state8.var_11) ^ (((((0x9a4e8395) + (rb_state8.var_9)) ^ (rb_state8.var_8)) - (rb_state8.var_9)) - (rb_state8.var_10));
            rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x0)) {
            rb_state15.var_2 = rb_state15.var_1;
        }
        if ((rb_state15.var_0) == (0x0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                racebench_trigger(15);
            }
        }
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_0
                if ((rb_state0.var_0) == (0x2437200b)) {
                    rb_state0.var_2 = rb_state0.var_1;
                }
                if ((rb_state0.var_0) == (0x2437200b)) {
                    if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                        racebench_trigger(0);
                    }
                }
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_15
                if ((rb_state15.var_0) == (0x0)) {
                    rb_state15.var_3 = 0xf391af04;
                    rb_state15.var_4 = (rb_state15.var_4) ^ (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_3) : (0x516f956b));
                    rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_4);
                }
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) + (0xa2f6930);
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
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
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + ((0xb8768ca6) - (rb_state18.var_0));
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

    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x7758c23f)) {
        rb_state17.var_5 = 0xef92d37a;
        rb_state17.var_6 = 0xcf6408d0;
        rb_state17.var_7 = (rb_state17.var_7) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0x1b89a14f));
        rb_state17.var_8 = (rb_state17.var_8) ^ (((0x2c8d1f31) + (rb_state17.var_4)) - (rb_state17.var_3));
        rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_7);
        rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_8) : (0x84658ffe));
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_9) : (0xeecc9582));
        rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_10);
        rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_11) : (0xa6c51ef2));
        rb_state17.var_14 = (rb_state17.var_14) ^ (((((0x89855e16) - (rb_state17.var_12)) - (0xd968edf1)) + (0xb9f2feab)) + (rb_state17.var_13));
        rb_state17.var_4 = (rb_state17.var_4) - (rb_state17.var_14);
        rb_state17.var_1 = rb_state17.var_4;
    }
    if ((rb_state17.var_0) == (0x7758c23f)) {
        pthread_mutex_lock(&(rb_state17.lock_23));
        rb_state17.var_16 = 0xa69f5207;
        rb_state17.var_17 = 0x9c4ca444;
        rb_state17.var_18 = (rb_state17.var_18) + (rb_state17.var_17);
        rb_state17.var_19 = (rb_state17.var_19) + (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_16) : (0x4652e898));
        rb_state17.var_20 = (rb_state17.var_20) + (((0xb4042688) + (rb_state17.var_8)) ^ (rb_state17.var_10));
        rb_state17.var_21 = (rb_state17.var_21) + (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_19) : (0x2ae76698));
        rb_state17.var_22 = (rb_state17.var_22) - (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0xdde16b7f));
        rb_state17.var_15 = (rb_state17.var_15) ^ (rb_state17.var_22);
        rb_state17.var_1 = rb_state17.var_15;
        pthread_mutex_unlock(&(rb_state17.lock_23));
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