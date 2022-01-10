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
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (156170 < rb_input_size ? (uint32_t)rb_input[156170] : 0x6d300b84);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0xf014bac7));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
        rb_state11.var_3 = (rb_state11.var_3) - (4779 < rb_input_size ? (uint32_t)rb_input[4779] : 0x3c521f19);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_1) == (0xf3e7f7dc)) {
            pthread_mutex_lock(&(rb_state19.lock_46));
            rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0xd41c4b92));
            rb_state19.var_22 = (rb_state19.var_22) ^ (0xbdfb0242);
            rb_state19.var_23 = (rb_state19.var_23) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_16) : (0x766b6456));
            rb_state19.var_24 = (rb_state19.var_24) + (((((0x40ce018c) - (rb_state19.var_21)) + (rb_state19.var_13)) - (rb_state19.var_18)) - (0xe95a82b3));
            rb_state19.var_25 = (rb_state19.var_25) + (rb_state19.var_22);
            rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_23);
            rb_state19.var_27 = (rb_state19.var_27) - (((((0x9ace7b0f) + (rb_state19.var_25)) ^ (rb_state19.var_24)) ^ (rb_state19.var_19)) ^ (0x6b9ef885));
            rb_state19.var_28 = (rb_state19.var_28) - (((0x7f6fd707) + (rb_state19.var_26)) ^ (0x850482b7));
            rb_state19.var_29 = (rb_state19.var_29) ^ (rb_state19.var_27);
            rb_state19.var_30 = (rb_state19.var_30) + (rb_state19.var_28);
            rb_state19.var_31 = (rb_state19.var_31) ^ (((0x27323724) ^ (0x1e84b46d)) ^ (rb_state19.var_29));
            rb_state19.var_32 = (rb_state19.var_32) - (((0x8da0b277) - (rb_state19.var_30)) - (0x8b702c02));
            rb_state19.var_33 = (rb_state19.var_33) + (((((0x44d8171) ^ (rb_state19.var_32)) + (rb_state19.var_20)) + (rb_state19.var_31)) - (0xd52f1425));
            rb_state19.var_20 = (rb_state19.var_20) + (rb_state19.var_33);
            rb_state19.var_4 = rb_state19.var_20;
            pthread_mutex_unlock(&(rb_state19.lock_46));
        }
        if ((rb_state19.var_1) == (0xf3e7f7dc)) {
            pthread_mutex_lock(&(rb_state19.lock_46));
            rb_state19.var_35 = 0xe7d7f7a7;
            rb_state19.var_36 = 0x99351b31;
            rb_state19.var_37 = (rb_state19.var_37) ^ (rb_state19.var_21);
            rb_state19.var_38 = (rb_state19.var_38) - (0x8e6dc5eb);
            rb_state19.var_39 = (rb_state19.var_39) ^ (((rb_state19.var_35) == (0x0)) ? (rb_state19.var_36) : (0x65b35b44));
            rb_state19.var_40 = (rb_state19.var_40) ^ (((0x4e4e995e) ^ (rb_state19.var_37)) ^ (rb_state19.var_22));
            rb_state19.var_41 = (rb_state19.var_41) ^ (((rb_state19.var_38) == (0x0)) ? (rb_state19.var_39) : (0x366d5755));
            rb_state19.var_42 = (rb_state19.var_42) ^ (rb_state19.var_40);
            rb_state19.var_43 = (rb_state19.var_43) ^ (rb_state19.var_41);
            rb_state19.var_44 = (rb_state19.var_44) ^ (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_42) : (0x1d147f38));
            rb_state19.var_45 = (rb_state19.var_45) - (((((0x29f96813) + (0x74a7a1ca)) - (rb_state19.var_44)) - (rb_state19.var_43)) + (0xe1b786a1));
            rb_state19.var_34 = (rb_state19.var_34) + (rb_state19.var_45);
            rb_state19.var_4 = rb_state19.var_34;
            pthread_mutex_unlock(&(rb_state19.lock_46));
        }
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x841c7688));
            rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_3);
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + ((0x362b6ab4) + (rb_state1.var_1));
            rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_0);
            rb_state1.var_2 = (rb_state1.var_2) - (125499 < rb_input_size ? (uint32_t)rb_input[125499] : 0xec58abba);
            rb_state1.var_3 = (rb_state1.var_3) + (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_2) : (0x41d86444));
            rb_state1.var_2 = (rb_state1.var_2) + (0xb9c052da);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_0) == (0x69efe9d8)) ? (rb_state2.var_3) : (0x874cdd2c));
            if ((rb_state2.var_3) == (0xa230828)) {
                pthread_mutex_lock(&(rb_state2.lock_43));
                if ((rb_state2.var_4) != (0x0)) {
                    if (!((rb_state2.var_4) == (rb_state2.var_32))) {
                        racebench_trigger(2);
                    }
                }
                pthread_mutex_unlock(&(rb_state2.lock_43));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
            rb_state4.var_0 = (rb_state4.var_0) ^ (68171 < rb_input_size ? (uint32_t)rb_input[68171] : 0x9775800b);
            rb_state4.var_1 = (rb_state4.var_1) - (147793 < rb_input_size ? (uint32_t)rb_input[147793] : 0x47a76309);
            rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_2);
            rb_state4.var_3 = (rb_state4.var_3) ^ (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_3) : (0x6f3dcc73));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_2);
            rb_state6.var_1 = (rb_state6.var_1) - ((0xbb33ef5f) - (rb_state6.var_2));
            rb_state6.var_3 = (rb_state6.var_3) + ((0xb376b5a6) - (rb_state6.var_3));
            rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_3);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0xd822a114));
            rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0x49fb225a));
            rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_1);
            rb_state8.var_2 = (rb_state8.var_2) ^ (88144 < rb_input_size ? (uint32_t)rb_input[88144] : 0x94858b36);
            if ((rb_state8.var_4) == (0x83eaba3f)) {
                rb_state8.var_6 = 0x93572c0;
                rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_5) : (0x2eee7001));
                rb_state8.var_8 = (rb_state8.var_8) + (((((0xb934a25) - (rb_state8.var_7)) + (0x956f89e6)) - (rb_state8.var_6)) - (0x28939ba1));
                rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_8);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) - ((0xf8d4f0ec) + (0x84907dd5));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0xe55d2356)) ? (rb_state11.var_0) : (0x5a5a9e57));
            rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_1) : (0x1718fc7b));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_2 = (rb_state16.var_2) - (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_1) : (0xe88a3196));
            rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_1);
            rb_state16.var_3 = (rb_state16.var_3) - (0x3e123e9b);
            rb_state16.var_3 = (rb_state16.var_3) ^ (rb_state16.var_3);
            if ((rb_state16.var_0) == (0x532f224c)) {
                rb_state16.var_20 = 0xaddf71f5;
                rb_state16.var_21 = (rb_state16.var_21) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_13) : (0x700fea8));
                rb_state16.var_22 = (rb_state16.var_22) ^ ((0xc54c9e12) - (rb_state16.var_15));
                rb_state16.var_23 = (rb_state16.var_23) ^ (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_20) : (0x23408e12));
                rb_state16.var_24 = (rb_state16.var_24) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_21) : (0xcf838871));
                rb_state16.var_25 = (rb_state16.var_25) + (rb_state16.var_22);
                rb_state16.var_26 = (rb_state16.var_26) ^ (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_23) : (0x57b0b89));
                rb_state16.var_27 = (rb_state16.var_27) - (((0x7d93488d) ^ (0x80f3b052)) + (rb_state16.var_24));
                rb_state16.var_28 = (rb_state16.var_28) + (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_26) : (0xeaabfd6f));
                rb_state16.var_29 = (rb_state16.var_29) + (rb_state16.var_27);
                rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_28) : (0x34e7caac));
                rb_state16.var_31 = (rb_state16.var_31) ^ (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_29) : (0x8277b7d8));
                rb_state16.var_32 = (rb_state16.var_32) ^ (((((0x37a20f67) ^ (rb_state16.var_31)) + (rb_state16.var_21)) - (rb_state16.var_30)) - (0xf0549543));
                rb_state16.var_19 = (rb_state16.var_19) ^ (rb_state16.var_32);
                rb_state16.var_4 = rb_state16.var_19;
            }
            if ((rb_state16.var_0) == (0x532f224c)) {
                pthread_mutex_lock(&(rb_state16.lock_43));
                rb_state16.var_34 = 0xf75d38ec;
                rb_state16.var_35 = (rb_state16.var_35) ^ (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_23) : (0x67f93993));
                rb_state16.var_36 = (rb_state16.var_36) + (((0x6c8eaec3) - (rb_state16.var_24)) ^ (rb_state16.var_34));
                rb_state16.var_37 = (rb_state16.var_37) - (rb_state16.var_25);
                rb_state16.var_38 = (rb_state16.var_38) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_35) : (0xd4b362c2));
                rb_state16.var_39 = (rb_state16.var_39) ^ (((0x7c79982c) ^ (rb_state16.var_36)) - (0xb664f49d));
                rb_state16.var_40 = (rb_state16.var_40) ^ (((rb_state16.var_37) == (0x0)) ? (rb_state16.var_38) : (0xc722d240));
                rb_state16.var_41 = (rb_state16.var_41) + (((0x5babee33) - (0x41cef953)) - (rb_state16.var_39));
                rb_state16.var_42 = (rb_state16.var_42) ^ (((rb_state16.var_40) == (0x0)) ? (rb_state16.var_41) : (0x8dd02101));
                rb_state16.var_33 = (rb_state16.var_33) + (rb_state16.var_42);
                rb_state16.var_4 = rb_state16.var_33;
                pthread_mutex_unlock(&(rb_state16.lock_43));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) ^ ((0xfd4c37a0) ^ (rb_state17.var_2));
            rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_2) == (0xfd4c37a0)) ? (rb_state17.var_1) : (0x6110632));
            rb_state17.var_4 = (rb_state17.var_4) ^ (0x65ed7bac);
            rb_state17.var_0 = (rb_state17.var_0) ^ (0x67e154c5);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_0);
            rb_state19.var_2 = (rb_state19.var_2) ^ ((0xf612f902) - (rb_state19.var_2));
            rb_state19.var_2 = (rb_state19.var_2) - ((0x8eba3b9) ^ (rb_state19.var_1));
            rb_state19.var_2 = (rb_state19.var_2) ^ (140061 < rb_input_size ? (uint32_t)rb_input[140061] : 0xb4689db3);
            rb_state19.var_3 = (rb_state19.var_3) - (91439 < rb_input_size ? (uint32_t)rb_input[91439] : 0x83cddd);
            if ((rb_state19.var_1) == (0xf3e7f7dc)) {
                if ((rb_state19.var_4) != (0x0)) {
                    if (!((rb_state19.var_4) == (rb_state19.var_34))) {
                        racebench_trigger(19);
                    }
                }
            }
            #endif
            bucket++;
        }
    }

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0xa230828)) {
        rb_state2.var_15 = 0xc729c2de;
        rb_state2.var_16 = 0xdc4b9e90;
        rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_10);
        rb_state2.var_18 = (rb_state2.var_18) - (((0xcff8b31f) + (0x4dfa90e8)) + (rb_state2.var_15));
        rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_9);
        rb_state2.var_20 = (rb_state2.var_20) + (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_16) : (0xc166fa38));
        rb_state2.var_21 = (rb_state2.var_21) + (((((0x355306dc) ^ (rb_state2.var_17)) - (rb_state2.var_12)) + (rb_state2.var_13)) + (rb_state2.var_18));
        rb_state2.var_22 = (rb_state2.var_22) + (rb_state2.var_19);
        rb_state2.var_23 = (rb_state2.var_23) ^ (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_20) : (0xe3253930));
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_15) == (0xc729c2de)) ? (rb_state2.var_21) : (0x1e4f6585));
        rb_state2.var_25 = (rb_state2.var_25) + (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0x169c465e));
        rb_state2.var_26 = (rb_state2.var_26) + (rb_state2.var_24);
        rb_state2.var_27 = (rb_state2.var_27) ^ (rb_state2.var_25);
        rb_state2.var_28 = (rb_state2.var_28) - (((rb_state2.var_16) == (0xdc4b9e90)) ? (rb_state2.var_26) : (0xda8a6086));
        rb_state2.var_29 = (rb_state2.var_29) - (rb_state2.var_27);
        rb_state2.var_30 = (rb_state2.var_30) ^ (rb_state2.var_28);
        rb_state2.var_31 = (rb_state2.var_31) - (((rb_state2.var_29) == (0x23b46170)) ? (rb_state2.var_30) : (0x4eb5242e));
        rb_state2.var_14 = (rb_state2.var_14) - (rb_state2.var_31);
        rb_state2.var_4 = rb_state2.var_14;
    }
    if ((rb_state2.var_3) == (0xa230828)) {
        pthread_mutex_lock(&(rb_state2.lock_43));
        rb_state2.var_33 = 0xfa97dc96;
        rb_state2.var_34 = (rb_state2.var_34) ^ (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_18) : (0xd699d478));
        rb_state2.var_35 = (rb_state2.var_35) + (((((0x54329786) ^ (rb_state2.var_20)) ^ (0x597b5bfa)) ^ (rb_state2.var_17)) + (rb_state2.var_33));
        rb_state2.var_36 = (rb_state2.var_36) - (((0x31a9809e) ^ (rb_state2.var_34)) + (rb_state2.var_21));
        rb_state2.var_37 = (rb_state2.var_37) - (((0x372b4750) ^ (rb_state2.var_22)) ^ (rb_state2.var_35));
        rb_state2.var_38 = (rb_state2.var_38) - (((0x295aba8a) ^ (rb_state2.var_36)) + (rb_state2.var_23));
        rb_state2.var_39 = (rb_state2.var_39) - (((0x26a02fe4) + (rb_state2.var_24)) + (rb_state2.var_37));
        rb_state2.var_40 = (rb_state2.var_40) - (((0x977259ac) - (rb_state2.var_38)) - (rb_state2.var_25));
        rb_state2.var_41 = (rb_state2.var_41) - (((rb_state2.var_26) == (0xafca0009)) ? (rb_state2.var_39) : (0xa5ca49c9));
        rb_state2.var_42 = (rb_state2.var_42) + (((((0xe5982c2b) - (rb_state2.var_40)) ^ (0x2bae8c54)) + (0xc1c2969d)) ^ (rb_state2.var_41));
        rb_state2.var_32 = (rb_state2.var_32) + (rb_state2.var_42);
        rb_state2.var_4 = rb_state2.var_32;
        pthread_mutex_unlock(&(rb_state2.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
    rb_state8.var_4 = (rb_state8.var_4) + ((0x83eaba3f) ^ (rb_state8.var_4));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - ((0x7171c333) ^ (rb_state11.var_3));
    rb_state11.var_3 = (rb_state11.var_3) ^ (rb_state11.var_0);
    #endif
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
    rb_state0.var_2 = (rb_state0.var_2) ^ ((0xf537d200) - (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (0xd516e55);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_3) : (0x7200a567));
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_3 = (rb_state16.var_3) - (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_2) : (0x1e4d959a));
        if ((rb_state16.var_0) == (0x532f224c)) {
            if ((rb_state16.var_4) != (0x0)) {
                if (!((rb_state16.var_4) == (rb_state16.var_33))) {
                    racebench_trigger(16);
                }
            }
        }
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + ((0xd7e1e876) + (0x266377cd));
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0x7c266ed4) - (rb_state10.var_2));
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
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) ^ (14436 < rb_input_size ? (uint32_t)rb_input[14436] : 0x3b371e8a);
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_4) : (0xac7ed007));
        rb_state17.var_4 = (rb_state17.var_4) - (((rb_state17.var_1) == (0x54a8e93a)) ? (rb_state17.var_0) : (0xcba45bbe));
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_1);
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_4
                rb_state4.var_2 = (rb_state4.var_2) - (0x189e0e43);
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_3) == (0x66ed6b4c)) ? (rb_state6.var_2) : (0xc8514f43));
                rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_1);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_3 = (rb_state10.var_3) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_3) : (0x30d5cee5));
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_2) == (0x0)) {
                    rb_state6.var_5 = 0x30c1ff01;
                    rb_state6.var_6 = 0xc5172f18;
                    rb_state6.var_7 = 0x7f819f80;
                    rb_state6.var_8 = (rb_state6.var_8) ^ ((0xc9dec55b) + (rb_state6.var_4));
                    rb_state6.var_9 = (rb_state6.var_9) ^ (((0x1ef3e181) - (rb_state6.var_5)) ^ (rb_state6.var_7));
                    rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_6) : (0xe687bbc6));
                    rb_state6.var_11 = (rb_state6.var_11) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_8) : (0xaf36e3d0));
                    rb_state6.var_12 = (rb_state6.var_12) ^ (((0x5142aa90) + (rb_state6.var_9)) + (0xb938eb4));
                    rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0xfe3e56a0));
                    rb_state6.var_14 = (rb_state6.var_14) - (rb_state6.var_12);
                    rb_state6.var_15 = (rb_state6.var_15) ^ (rb_state6.var_13);
                    rb_state6.var_16 = (rb_state6.var_16) + (rb_state6.var_14);
                    rb_state6.var_17 = (rb_state6.var_17) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_15) : (0x8cc29d50));
                    rb_state6.var_18 = (rb_state6.var_18) - (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_17) : (0xbf400011));
                    rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_18);
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) - ((0xa661b532) - (rb_state10.var_2));
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_2) == (0x0)) {
                    rb_state6.var_4 = rb_state6.var_3;
                }
                if ((rb_state6.var_2) == (0x0)) {
                    if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                        racebench_trigger(6);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_2) == (0x0)) {
                    pthread_mutex_lock(&(rb_state11.lock_17));
                    rb_state11.var_6 = 0xa0ddd024;
                    rb_state11.var_7 = 0x89ae5fe5;
                    rb_state11.var_8 = 0x85c264d0;
                    rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_8) : (0x493449b6));
                    rb_state11.var_10 = (rb_state11.var_10) - (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xef8eef30));
                    rb_state11.var_11 = (rb_state11.var_11) - (((((0xf56fdda4) - (rb_state11.var_9)) + (0x47c8658e)) ^ (0x57d50043)) + (rb_state11.var_10));
                    rb_state11.var_5 = (rb_state11.var_5) - (rb_state11.var_11);
                    rb_state11.var_4 = rb_state11.var_5;
                    pthread_mutex_unlock(&(rb_state11.lock_17));
                }
                if ((rb_state11.var_2) == (0x0)) {
                    if (!((rb_state11.var_4) == (rb_state11.var_5))) {
                        racebench_trigger(11);
                    }
                }
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) ^ (0xa30030ed);
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_0) == (0xaffaa6aa)) ? (rb_state6.var_2) : (0x3429b357));
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_3 = (rb_state8.var_3) + (0xe813bd59);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_2 = (rb_state17.var_2) ^ (((rb_state17.var_4) == (0xfee852e)) ? (rb_state17.var_3) : (0x2e0148bd));
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_4 = (rb_state4.var_4) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_4) : (0xf5b6a2e0));
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    if ((rb_state8.var_4) == (0x83eaba3f)) {
                        rb_state8.var_5 = rb_state8.var_3;
                    }
                    if ((rb_state8.var_4) == (0x83eaba3f)) {
                        if (!((rb_state8.var_3) == (rb_state8.var_5))) {
                            racebench_trigger(8);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_3 = (rb_state10.var_3) - (((rb_state10.var_1) == (0x9070a172)) ? (rb_state10.var_0) : (0x78e36319));
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_1 = (rb_state11.var_1) ^ ((0xb071ab1f) + (0x988c30b4));
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
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) - (0xc2d4ed12);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_3 = (rb_state10.var_3) + ((0xe52bec21) + (rb_state10.var_0));
                #endif
                break;
            }

            prev = d;
            d = NEXTFREE(d);
        }

        { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
    }

    if (result) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_3);
        rb_state0.var_4 = (rb_state0.var_4) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_4) : (0x23c95edc));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_3);
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

    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) - (0xecdfbdd3);
    if ((rb_state11.var_2) == (0x0)) {
        rb_state11.var_12 = 0x9f27cdb2;
        rb_state11.var_13 = (rb_state11.var_13) + (((0x3e074427) ^ (rb_state11.var_6)) + (rb_state11.var_7));
        rb_state11.var_14 = (rb_state11.var_14) ^ (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) - (((((0x2b8f701d) ^ (rb_state11.var_5)) - (0x4361cba)) - (0xc483b916)) + (rb_state11.var_13));
        rb_state11.var_16 = (rb_state11.var_16) - (((((0xb64144eb) ^ (rb_state11.var_8)) ^ (rb_state11.var_15)) - (rb_state11.var_14)) - (0x8ffbe79d));
        rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_16);
    }
    #endif
    mem_pool[home].tally += SIZE(result);
    if (mem_pool[home].tally > mem_pool[home].maxm) {
        mem_pool[home].maxm = mem_pool[home].tally;
    }

    if (SIZE(result) < size) {
        printf("*** Bad size from malloc %d, %d\n", size, SIZE(result));
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (52087 < rb_input_size ? (uint32_t)rb_input[52087] : 0x2e797e50);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_0) == (0x3d2b130e)) ? (rb_state4.var_2) : (0x9723eb4c));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - (53414 < rb_input_size ? (uint32_t)rb_input[53414] : 0xb71fcfda);
    #endif
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