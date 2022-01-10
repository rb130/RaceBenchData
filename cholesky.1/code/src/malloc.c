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
        if ((rb_state0.var_0) == (0xff)) {
            pthread_mutex_lock(&(rb_state0.lock_13));
            rb_state0.var_3 = 0xebc590c0;
            rb_state0.var_4 = 0x3e4ebd67;
            rb_state0.var_5 = (rb_state0.var_5) + (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0xd271130b));
            rb_state0.var_6 = (rb_state0.var_6) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_4) : (0xd80541bd));
            rb_state0.var_7 = (rb_state0.var_7) + (((0xad51c4cd) + (rb_state0.var_5)) + (rb_state0.var_3));
            rb_state0.var_8 = (rb_state0.var_8) ^ (rb_state0.var_6);
            rb_state0.var_9 = (rb_state0.var_9) + (((0x9efb57b4) ^ (0x5e8bb750)) - (rb_state0.var_7));
            rb_state0.var_10 = (rb_state0.var_10) + (((0x76b679fd) + (0x52d4f4f1)) - (rb_state0.var_8));
            rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_9) : (0xf7e0a719));
            rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0x7d219b94));
            rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_12);
            rb_state0.var_2 = rb_state0.var_1;
            pthread_mutex_unlock(&(rb_state0.lock_13));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x0)) {
            rb_state18.var_3 = 0xd30e8888;
            rb_state18.var_4 = 0xc4d179ac;
            rb_state18.var_5 = 0x625e13a7;
            rb_state18.var_6 = (rb_state18.var_6) - (0x3958beef);
            rb_state18.var_7 = (rb_state18.var_7) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_3) : (0x9109b365));
            rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_5) == (0x625e13a7)) ? (rb_state18.var_4) : (0xaf2a1a60));
            rb_state18.var_9 = (rb_state18.var_9) ^ (((((0x18a658e0) - (rb_state18.var_7)) ^ (rb_state18.var_6)) + (rb_state18.var_3)) ^ (0x455ae02e));
            rb_state18.var_10 = (rb_state18.var_10) - (((0x2d6a6e9) + (0xffd774c6)) + (rb_state18.var_8));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((((0x6bb9ce77) ^ (rb_state18.var_9)) + (rb_state18.var_10)) ^ (rb_state18.var_4)) + (rb_state18.var_5));
            rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_11);
        }
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (140332 < rb_input_size ? (uint32_t)rb_input[140332] : 0x31d864d1);
            if ((rb_state0.var_0) == (0xff)) {
                rb_state0.var_14 = !((rb_state0.var_2) == (rb_state0.var_1));
            }
            rb_state0.var_5 = (rb_state0.var_5) + (rb_state0.var_6);
            if ((rb_state0.var_7) == (0x49fbe25)) {
                rb_state0.var_16 = 0xa44d76e3;
                rb_state0.var_17 = (rb_state0.var_17) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0xb123c27d));
                rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_16) : (0x312df5f0));
                rb_state0.var_19 = (rb_state0.var_19) ^ (75182 < rb_input_size ? (uint32_t)rb_input[75182] : 0x16e26a6e);
                rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_18) : (0xa5443e72));
                rb_state0.var_21 = (rb_state0.var_21) + (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_19) : (0xda908446));
                rb_state0.var_22 = (rb_state0.var_22) + (((0x87bbeec4) - (rb_state0.var_20)) + (rb_state0.var_14));
                rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_22) : (0x6609996b));
                rb_state0.var_8 = (rb_state0.var_8) + (rb_state0.var_23);
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - ((0x8962663a) ^ (0xa9fd794a));
            rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0xdf60e090)) ? (rb_state1.var_0) : (0xdaa25b38));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x895b839a));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (0x51780bdb);
            if ((rb_state3.var_0) == (0x51780bbb)) {
                pthread_mutex_lock(&(rb_state3.lock_22));
                rb_state3.var_3 = 0xcdb80d19;
                rb_state3.var_4 = (rb_state3.var_4) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_1) : (0x72c1342e));
                rb_state3.var_5 = (rb_state3.var_5) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x7c229abc));
                rb_state3.var_6 = (rb_state3.var_6) - (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_4) : (0xf06adb78));
                rb_state3.var_7 = (rb_state3.var_7) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0xfd521c7b));
                rb_state3.var_2 = (rb_state3.var_2) - (rb_state3.var_7);
                rb_state3.var_1 = rb_state3.var_2;
                pthread_mutex_unlock(&(rb_state3.lock_22));
            }
            if ((rb_state3.var_0) == (0x51780bbb)) {
                if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                    racebench_trigger(3);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ ((0x2989733c) - (0xa978666e));
            if ((rb_state5.var_0) == (0x0)) {
                rb_state5.var_2 = rb_state5.var_1;
            }
            if ((rb_state5.var_0) == (0x0)) {
                if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x54d525cd));
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0xb56efcde)) {
                pthread_mutex_lock(&(rb_state7.lock_15));
                rb_state7.var_2 = rb_state7.var_1;
                pthread_mutex_unlock(&(rb_state7.lock_15));
            }
            if ((rb_state7.var_0) == (0xb56efcde)) {
                if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                    racebench_trigger(7);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x22e47b0c));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + ((0x5e41ad83) ^ (rb_state10.var_0));
            rb_state10.var_0 = (rb_state10.var_0) + (0x311830ea);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
            rb_state11.var_0 = (rb_state11.var_0) - ((0x8fe548eb) - (rb_state11.var_0));
            if ((rb_state11.var_0) == (0x701ab715)) {
                pthread_mutex_lock(&(rb_state11.lock_14));
                rb_state11.var_3 = 0xb0947fce;
                rb_state11.var_4 = (rb_state11.var_4) - (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_1) : (0x546d5eec));
                rb_state11.var_5 = (rb_state11.var_5) + (((0x42034cc8) + (rb_state11.var_3)) + (0x2fe50b51));
                rb_state11.var_6 = (rb_state11.var_6) + (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_2) : (0x769ce0e8));
                rb_state11.var_7 = (rb_state11.var_7) - (((0x8cc54285) + (rb_state11.var_4)) ^ (rb_state11.var_5));
                rb_state11.var_8 = (rb_state11.var_8) - (((((0x3ed8c11c) - (rb_state11.var_5)) + (rb_state11.var_6)) - (rb_state11.var_6)) + (0xb277f80));
                rb_state11.var_9 = (rb_state11.var_9) + (((0x7a25bb49) + (rb_state11.var_7)) + (rb_state11.var_7));
                rb_state11.var_10 = (rb_state11.var_10) ^ (rb_state11.var_8);
                rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_9) : (0x111f74ba));
                rb_state11.var_12 = (rb_state11.var_12) - (((0xeb6d55e1) + (rb_state11.var_10)) + (0x83c56503));
                rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_12) : (0x2cbd8677));
                rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_13);
                pthread_mutex_unlock(&(rb_state11.lock_14));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (0xe5dedd2f);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - ((0xb6292b28) - (0x96098597));
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
    rb_state5.var_0 = (rb_state5.var_0) ^ (174801 < rb_input_size ? (uint32_t)rb_input[174801] : 0xcac95237);
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x91e0f120)) {
            rb_state1.var_14 = 0x8fdb841f;
            rb_state1.var_15 = 0xf2f13b07;
            rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_14) : (0xdc50151f));
            rb_state1.var_17 = (rb_state1.var_17) ^ ((0xbf6dcda3) ^ (rb_state1.var_8));
            rb_state1.var_18 = (rb_state1.var_18) ^ (rb_state1.var_7);
            rb_state1.var_19 = (rb_state1.var_19) + (((((0xecaa5e11) - (0x5b67d8c9)) - (rb_state1.var_17)) + (rb_state1.var_16)) + (0x9be898ad));
            rb_state1.var_20 = (rb_state1.var_20) + (((0x3d42062f) - (rb_state1.var_18)) - (0x9e8c7bb1));
            rb_state1.var_21 = (rb_state1.var_21) ^ (((((0xdffbe796) + (rb_state1.var_20)) + (rb_state1.var_19)) ^ (0x288f65e4)) ^ (0x651f8375));
            rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x1a2122d1)) {
        rb_state14.var_3 = 0xf9bb4384;
        rb_state14.var_4 = (rb_state14.var_4) - (((0x7917db0b) ^ (rb_state14.var_3)) ^ (0xbdc197d0));
        rb_state14.var_5 = (rb_state14.var_5) - (((((0x2a04adb1) + (0x30200ad5)) + (rb_state14.var_2)) + (rb_state14.var_4)) ^ (0xa3b82479));
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_5);
    }
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
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x91e0f120)) {
            pthread_mutex_lock(&(rb_state1.lock_22));
            rb_state1.var_3 = 0x4e61b974;
            rb_state1.var_4 = (rb_state1.var_4) ^ (0x9cb63c95);
            rb_state1.var_5 = (rb_state1.var_5) - (((((0x11dc3d1c) - (rb_state1.var_1)) - (0x8a588f86)) ^ (0x8498656a)) ^ (rb_state1.var_3));
            rb_state1.var_6 = (rb_state1.var_6) ^ (rb_state1.var_3);
            rb_state1.var_7 = (rb_state1.var_7) ^ (((0x4a943768) + (rb_state1.var_4)) + (rb_state1.var_2));
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_4) : (0xb32fc848));
            rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_5);
            rb_state1.var_10 = (rb_state1.var_10) + (((0xe18a7f21) ^ (rb_state1.var_6)) ^ (0x27b99a82));
            rb_state1.var_11 = (rb_state1.var_11) ^ (((((0xed545454) - (rb_state1.var_8)) ^ (0x9ce46a74)) + (0x45b52eab)) - (rb_state1.var_7));
            rb_state1.var_12 = (rb_state1.var_12) - (((((0x1235c8b2) - (rb_state1.var_9)) + (0xd5e49941)) + (0xc563770f)) - (rb_state1.var_10));
            rb_state1.var_13 = (rb_state1.var_13) - (((((0x61c676ff) ^ (0x1f9a9efc)) - (rb_state1.var_12)) ^ (rb_state1.var_11)) ^ (rb_state1.var_6));
            rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_13);
            rb_state1.var_1 = rb_state1.var_2;
            pthread_mutex_unlock(&(rb_state1.lock_22));
        }
        if ((rb_state1.var_0) == (0x91e0f120)) {
            if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x9ef637c1)) {
            rb_state8.var_5 = 0x57fb29fe;
            rb_state8.var_6 = (rb_state8.var_6) ^ (rb_state8.var_5);
            rb_state8.var_7 = (rb_state8.var_7) - (((((0xe06d69ad) ^ (rb_state8.var_3)) + (0x1aeed15f)) - (rb_state8.var_4)) + (0x62266f9a));
            rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_5);
            rb_state8.var_9 = (rb_state8.var_9) - (((0x2fbca0be) ^ (0xf2b6ce46)) + (rb_state8.var_6));
            rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_7);
            rb_state8.var_11 = (rb_state8.var_11) - (((0x54b3c0b4) + (rb_state8.var_8)) + (0x9b0a6153));
            rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_9) : (0xf6964509));
            rb_state8.var_13 = (rb_state8.var_13) - (((0x9a4d5aed) ^ (rb_state8.var_7)) ^ (rb_state8.var_10));
            rb_state8.var_14 = (rb_state8.var_14) ^ (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_11) : (0x28a2849c));
            rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_12) : (0xf13f606f));
            rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_13);
            rb_state8.var_17 = (rb_state8.var_17) - (rb_state8.var_14);
            rb_state8.var_18 = (rb_state8.var_18) + (rb_state8.var_15);
            rb_state8.var_19 = (rb_state8.var_19) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_16) : (0x449e9ce0));
            rb_state8.var_20 = (rb_state8.var_20) ^ (rb_state8.var_17);
            rb_state8.var_21 = (rb_state8.var_21) + (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_19) : (0x2f7076fe));
            rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_21) : (0x4ed0b852));
            rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_22);
            rb_state8.var_1 = rb_state8.var_4;
        }
        if ((rb_state8.var_0) == (0x9ef637c1)) {
            pthread_mutex_lock(&(rb_state8.lock_30));
            rb_state8.var_24 = 0x6ab221ad;
            rb_state8.var_25 = 0x4cdd1b71;
            rb_state8.var_26 = (rb_state8.var_26) - (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_24) : (0x8e386d6b));
            rb_state8.var_27 = (rb_state8.var_27) ^ (((0x274aba97) - (0xe7c727b5)) + (rb_state8.var_11));
            rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_25) == (0x0)) ? (rb_state8.var_26) : (0x8bbc2ba2));
            rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_28) : (0xdac9ff28));
            rb_state8.var_23 = (rb_state8.var_23) ^ (rb_state8.var_29);
            rb_state8.var_1 = rb_state8.var_23;
            pthread_mutex_unlock(&(rb_state8.lock_30));
        }
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x0)) {
            rb_state5.var_3 = 0x9fda831f;
            rb_state5.var_4 = (rb_state5.var_4) + (162922 < rb_input_size ? (uint32_t)rb_input[162922] : 0xbdf46830);
            rb_state5.var_5 = (rb_state5.var_5) ^ ((0x7adb8f5b) ^ (0xcc945021));
            rb_state5.var_6 = (rb_state5.var_6) + (0xd25a47f6);
            rb_state5.var_7 = (rb_state5.var_7) + (((0x32c3d488) ^ (rb_state5.var_4)) - (rb_state5.var_2));
            rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_3);
            rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_4);
            rb_state5.var_10 = (rb_state5.var_10) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_5) : (0x33338a18));
            rb_state5.var_11 = (rb_state5.var_11) ^ (rb_state5.var_6);
            rb_state5.var_12 = (rb_state5.var_12) ^ (rb_state5.var_7);
            rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x8c84bd3f));
            rb_state5.var_14 = (rb_state5.var_14) - (rb_state5.var_10);
            rb_state5.var_15 = (rb_state5.var_15) + (rb_state5.var_11);
            rb_state5.var_16 = (rb_state5.var_16) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_12) : (0x5852ab70));
            rb_state5.var_17 = (rb_state5.var_17) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_13) : (0x3daf5b75));
            rb_state5.var_18 = (rb_state5.var_18) - (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_15) : (0x53468f36));
            rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_16) : (0x9f7b1298));
            rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_17);
            rb_state5.var_21 = (rb_state5.var_21) + (((0xabae45a) - (0xfc138c0f)) - (rb_state5.var_18));
            rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_19) : (0x10bfc839));
            rb_state5.var_23 = (rb_state5.var_23) + (((0x2c2ffdbc) - (rb_state5.var_20)) - (rb_state5.var_10));
            rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_21) : (0x6b659947));
            rb_state5.var_25 = (rb_state5.var_25) + (((((0x5587edd6) ^ (0x680eeda2)) + (rb_state5.var_22)) ^ (rb_state5.var_23)) - (0x62825725));
            rb_state5.var_26 = (rb_state5.var_26) + (((0x7b800ee5) ^ (0x268c5ed9)) ^ (rb_state5.var_24));
            rb_state5.var_27 = (rb_state5.var_27) ^ (((0xffa0852e) + (rb_state5.var_12)) + (rb_state5.var_25));
            rb_state5.var_28 = (rb_state5.var_28) ^ (((((0x31bb13ca) ^ (rb_state5.var_13)) ^ (rb_state5.var_27)) ^ (rb_state5.var_26)) + (rb_state5.var_14));
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_0);
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_0) == (0x701ab715)) {
                rb_state11.var_2 = rb_state11.var_1;
            }
            if ((rb_state11.var_0) == (0x701ab715)) {
                if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                    racebench_trigger(11);
                }
            }
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_0) == (0x9ef637c1)) {
                    if ((rb_state8.var_1) != (0x0)) {
                        if (!((rb_state8.var_1) == (rb_state8.var_23))) {
                            racebench_trigger(8);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) ^ ((0x71397d18) - (rb_state9.var_0));
                #endif
                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_0) == (0x1a2122d1)) {
                    rb_state14.var_2 = rb_state14.var_1;
                }
                if ((rb_state14.var_0) == (0x1a2122d1)) {
                    if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                        racebench_trigger(14);
                    }
                }
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
                #endif
                SIZE(result) = alloc_size;
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x48e4c0ef));
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_0);
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
        if ((rb_state9.var_0) == (0x69317c18)) {
            pthread_mutex_lock(&(rb_state9.lock_24));
            rb_state9.var_3 = 0x880829bf;
            rb_state9.var_4 = 0x2b9f7923;
            rb_state9.var_5 = (rb_state9.var_5) + (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0xdce66371));
            rb_state9.var_6 = (rb_state9.var_6) ^ (49329 < rb_input_size ? (uint32_t)rb_input[49329] : 0x5e5e1e45);
            rb_state9.var_7 = (rb_state9.var_7) - (rb_state9.var_4);
            rb_state9.var_8 = (rb_state9.var_8) - (((((0x45964965) ^ (0xcccf253b)) ^ (0xfb7ca826)) + (rb_state9.var_2)) + (rb_state9.var_3));
            rb_state9.var_9 = (rb_state9.var_9) ^ (((0xbf41942) - (rb_state9.var_5)) - (rb_state9.var_5));
            rb_state9.var_10 = (rb_state9.var_10) - (((((0x2fa47e5f) + (0x9fe2e225)) - (rb_state9.var_7)) ^ (rb_state9.var_6)) + (0xe790ad76));
            rb_state9.var_11 = (rb_state9.var_11) - (((0xdb974b30) - (rb_state9.var_6)) - (rb_state9.var_8));
            rb_state9.var_12 = (rb_state9.var_12) + (((0x43336462) ^ (rb_state9.var_7)) + (rb_state9.var_9));
            rb_state9.var_13 = (rb_state9.var_13) - (rb_state9.var_10);
            rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_11);
            rb_state9.var_15 = (rb_state9.var_15) + (rb_state9.var_12);
            rb_state9.var_16 = (rb_state9.var_16) + (((0x999b7d5a) ^ (rb_state9.var_13)) ^ (0xc660cf41));
            rb_state9.var_17 = (rb_state9.var_17) ^ (((0xd7c7cae1) - (rb_state9.var_8)) - (rb_state9.var_14));
            rb_state9.var_18 = (rb_state9.var_18) - (((0x27d151fc) ^ (rb_state9.var_9)) - (rb_state9.var_15));
            rb_state9.var_19 = (rb_state9.var_19) - (((0xebfe45fe) + (rb_state9.var_10)) + (rb_state9.var_16));
            rb_state9.var_20 = (rb_state9.var_20) ^ (rb_state9.var_17);
            rb_state9.var_21 = (rb_state9.var_21) + (((((0x7dc40466) ^ (rb_state9.var_19)) + (rb_state9.var_12)) + (rb_state9.var_11)) - (rb_state9.var_18));
            rb_state9.var_22 = (rb_state9.var_22) ^ (((0xe86aa8d6) ^ (rb_state9.var_20)) + (0x4e7de7b0));
            rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_22) : (0x5d8ee2a5));
            rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_23);
            pthread_mutex_unlock(&(rb_state9.lock_24));
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

    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x69317c18)) {
        pthread_mutex_lock(&(rb_state9.lock_24));
        rb_state9.var_2 = rb_state9.var_1;
        pthread_mutex_unlock(&(rb_state9.lock_24));
    }
    if ((rb_state9.var_0) == (0x69317c18)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
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