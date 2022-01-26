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
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x8ede6ce9)) {
            pthread_mutex_lock(&(rb_state1.lock_5));
            rb_state1.var_2 = rb_state1.var_1;
            pthread_mutex_unlock(&(rb_state1.lock_5));
        }
        if ((rb_state1.var_0) == (0x8ede6ce9)) {
            if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0x8cf6a637)) {
            pthread_mutex_lock(&(rb_state3.lock_17));
            rb_state3.var_4 = 0x8dcb725;
            rb_state3.var_5 = 0xa94bcc39;
            rb_state3.var_6 = (rb_state3.var_6) + (((((0x92a56217) + (rb_state3.var_3)) ^ (0x61f862c)) ^ (rb_state3.var_2)) - (rb_state3.var_5));
            rb_state3.var_7 = (rb_state3.var_7) - (((((0x44f04420) ^ (rb_state3.var_4)) - (rb_state3.var_6)) ^ (rb_state3.var_5)) - (rb_state3.var_4));
            rb_state3.var_3 = (rb_state3.var_3) + (rb_state3.var_7);
            rb_state3.var_2 = rb_state3.var_3;
            pthread_mutex_unlock(&(rb_state3.lock_17));
        }
        if ((rb_state3.var_1) == (0x8cf6a637)) {
            if (!((rb_state3.var_2) == (rb_state3.var_3))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_1) == (0x8f701408)) {
            rb_state11.var_2 = rb_state11.var_0;
        }
        if ((rb_state11.var_1) == (0x8f701408)) {
            if (!((rb_state11.var_0) == (rb_state11.var_2))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x1115ce90));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0xdc848f8c) ^ (0x9248125));
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
            if ((rb_state0.var_0) == (0x223b7b22)) {
                if ((rb_state0.var_2) != (0x0)) {
                    if (!((rb_state0.var_2) == (rb_state0.var_39))) {
                        racebench_trigger(0);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_0) == (0x8ede6ce9)) {
                rb_state1.var_3 = 0x6f0250b5;
                rb_state1.var_4 = (rb_state1.var_4) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_3) : (0x59044380));
                rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_4);
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_1);
            if ((rb_state4.var_1) == (0x3b271580)) {
                rb_state4.var_26 = 0x84ff9a94;
                rb_state4.var_27 = (rb_state4.var_27) ^ (((((0xfd71ca1a) ^ (rb_state4.var_26)) + (rb_state4.var_12)) ^ (rb_state4.var_14)) + (0x8dc323a1));
                rb_state4.var_28 = (rb_state4.var_28) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_15) : (0xb3909ca4));
                rb_state4.var_29 = (rb_state4.var_29) + (rb_state4.var_17);
                rb_state4.var_30 = (rb_state4.var_30) ^ (((((0xe23e8633) ^ (rb_state4.var_18)) + (0x6fcc6e6f)) + (rb_state4.var_13)) ^ (rb_state4.var_27));
                rb_state4.var_31 = (rb_state4.var_31) ^ (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_28) : (0x3d4a925d));
                rb_state4.var_32 = (rb_state4.var_32) - (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_30) : (0xdf600b4e));
                rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_32) : (0x96b27df3));
                rb_state4.var_25 = (rb_state4.var_25) + (rb_state4.var_33);
                rb_state4.var_2 = rb_state4.var_25;
            }
            if ((rb_state4.var_1) == (0x3b271580)) {
                pthread_mutex_lock(&(rb_state4.lock_58));
                rb_state4.var_35 = (rb_state4.var_35) + (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_22) : (0x777a71b7));
                rb_state4.var_36 = (rb_state4.var_36) ^ (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_24) : (0x8884ccad));
                rb_state4.var_37 = (rb_state4.var_37) + (0x4c2f8125);
                rb_state4.var_38 = (rb_state4.var_38) ^ ((0x15a0417d) - (rb_state4.var_26));
                rb_state4.var_39 = (rb_state4.var_39) + (((0xaf5e04a6) - (rb_state4.var_20)) - (0xf440efb6));
                rb_state4.var_40 = (rb_state4.var_40) + (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_35) : (0x320bc374));
                rb_state4.var_41 = (rb_state4.var_41) ^ (rb_state4.var_36);
                rb_state4.var_42 = (rb_state4.var_42) + (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_37) : (0xe1974690));
                rb_state4.var_43 = (rb_state4.var_43) - (rb_state4.var_38);
                rb_state4.var_44 = (rb_state4.var_44) + (((((0x9abe5878) ^ (rb_state4.var_39)) - (0xa79c38e0)) + (rb_state4.var_40)) ^ (rb_state4.var_28));
                rb_state4.var_45 = (rb_state4.var_45) ^ (rb_state4.var_41);
                rb_state4.var_46 = (rb_state4.var_46) + (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_42) : (0x1c712bdc));
                rb_state4.var_47 = (rb_state4.var_47) ^ (((0x4ad1bd58) + (rb_state4.var_43)) ^ (0xa4aeab2c));
                rb_state4.var_48 = (rb_state4.var_48) - (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_44) : (0xcf065d56));
                rb_state4.var_49 = (rb_state4.var_49) + (rb_state4.var_45);
                rb_state4.var_50 = (rb_state4.var_50) ^ (rb_state4.var_46);
                rb_state4.var_51 = (rb_state4.var_51) ^ (rb_state4.var_47);
                rb_state4.var_52 = (rb_state4.var_52) + (((((0xc8c87401) + (0x9e0e50a1)) + (rb_state4.var_49)) + (rb_state4.var_48)) ^ (rb_state4.var_31));
                rb_state4.var_53 = (rb_state4.var_53) - (rb_state4.var_50);
                rb_state4.var_54 = (rb_state4.var_54) + (((rb_state4.var_51) == (0x0)) ? (rb_state4.var_52) : (0xeb80f961));
                rb_state4.var_55 = (rb_state4.var_55) + (rb_state4.var_53);
                rb_state4.var_56 = (rb_state4.var_56) ^ (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_54) : (0x4ea2b1e7));
                rb_state4.var_57 = (rb_state4.var_57) - (((((0xea8d4743) + (rb_state4.var_55)) - (rb_state4.var_56)) ^ (rb_state4.var_34)) ^ (rb_state4.var_33));
                rb_state4.var_34 = (rb_state4.var_34) - (rb_state4.var_57);
                rb_state4.var_2 = rb_state4.var_34;
                pthread_mutex_unlock(&(rb_state4.lock_58));
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xe5a53423));
            rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_1);
            rb_state6.var_1 = (rb_state6.var_1) - ((0x865aecd4) - (rb_state6.var_1));
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0x55ee4bc1));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) + ((0xae3eef29) ^ (rb_state8.var_1));
            if ((rb_state8.var_1) == (0xb1bef0a9)) {
                rb_state8.var_2 = rb_state8.var_0;
            }
            if ((rb_state8.var_1) == (0xb1bef0a9)) {
                if (!((rb_state8.var_0) == (rb_state8.var_2))) {
                    racebench_trigger(8);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + ((0xf447fb8d) - (rb_state9.var_0));
            rb_state9.var_1 = (rb_state9.var_1) - ((0xcc93e142) - (rb_state9.var_1));
            rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0xf447fb8d)) ? (rb_state9.var_1) : (0x226f16fd));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x10ae5817));
            rb_state10.var_1 = (rb_state10.var_1) ^ (161855 < rb_input_size ? (uint32_t)rb_input[161855] : 0x91e5d1d5);
            rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_9);
            rb_state10.var_10 = (rb_state10.var_10) + (0xd9ba9a43);
            if ((rb_state10.var_11) == (0x8113f53e)) {
                pthread_mutex_lock(&(rb_state10.lock_52));
                rb_state10.var_25 = 0xc0350ba4;
                rb_state10.var_26 = 0x10999f8d;
                rb_state10.var_27 = (rb_state10.var_27) - (((((0x4002f406) - (0x4743c274)) ^ (rb_state10.var_25)) + (rb_state10.var_16)) - (rb_state10.var_26));
                rb_state10.var_28 = (rb_state10.var_28) + (78023 < rb_input_size ? (uint32_t)rb_input[78023] : 0x9301fb05);
                rb_state10.var_29 = (rb_state10.var_29) ^ (rb_state10.var_15);
                rb_state10.var_30 = (rb_state10.var_30) ^ (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_28) : (0xad02ab7f));
                rb_state10.var_31 = (rb_state10.var_31) + (((((0xaf85aa5a) - (rb_state10.var_30)) - (rb_state10.var_17)) + (0xb0d52469)) ^ (rb_state10.var_29));
                rb_state10.var_24 = (rb_state10.var_24) ^ (rb_state10.var_31);
                rb_state10.var_16 = rb_state10.var_24;
                pthread_mutex_unlock(&(rb_state10.lock_52));
            }
            if ((rb_state10.var_11) == (0x8113f53e)) {
                pthread_mutex_lock(&(rb_state10.lock_52));
                rb_state10.var_33 = 0x5bb7d26f;
                rb_state10.var_34 = (rb_state10.var_34) ^ (rb_state10.var_20);
                rb_state10.var_35 = (rb_state10.var_35) - ((0x8bf5bb72) ^ (rb_state10.var_21));
                rb_state10.var_36 = (rb_state10.var_36) + ((0x5f98e567) ^ (0x940b2a9f));
                rb_state10.var_37 = (rb_state10.var_37) ^ (((rb_state10.var_33) == (0x0)) ? (rb_state10.var_18) : (0x37eb664b));
                rb_state10.var_38 = (rb_state10.var_38) ^ (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_19) : (0xc2478029));
                rb_state10.var_39 = (rb_state10.var_39) + (((rb_state10.var_34) == (0x0)) ? (rb_state10.var_35) : (0xd5f8cfe7));
                rb_state10.var_40 = (rb_state10.var_40) + (rb_state10.var_36);
                rb_state10.var_41 = (rb_state10.var_41) ^ (((rb_state10.var_37) == (0x0)) ? (rb_state10.var_38) : (0x1a7918c1));
                rb_state10.var_42 = (rb_state10.var_42) - (rb_state10.var_39);
                rb_state10.var_43 = (rb_state10.var_43) ^ (((0x62043625) - (0xe57f1f1e)) + (rb_state10.var_40));
                rb_state10.var_44 = (rb_state10.var_44) + (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_41) : (0x9e3e10bf));
                rb_state10.var_45 = (rb_state10.var_45) - (((0x9b80135) + (rb_state10.var_24)) ^ (rb_state10.var_42));
                rb_state10.var_46 = (rb_state10.var_46) - (rb_state10.var_43);
                rb_state10.var_47 = (rb_state10.var_47) + (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_44) : (0xd79bc77b));
                rb_state10.var_48 = (rb_state10.var_48) - (rb_state10.var_45);
                rb_state10.var_49 = (rb_state10.var_49) + (((((0x485be36d) + (rb_state10.var_46)) ^ (rb_state10.var_47)) + (rb_state10.var_26)) ^ (0x164e2bae));
                rb_state10.var_50 = (rb_state10.var_50) - (((0x87a385b5) + (rb_state10.var_48)) + (0x2cdbead3));
                rb_state10.var_51 = (rb_state10.var_51) + (((((0x1d2ee5f4) ^ (rb_state10.var_49)) + (rb_state10.var_28)) + (rb_state10.var_27)) ^ (rb_state10.var_50));
                rb_state10.var_32 = (rb_state10.var_32) - (rb_state10.var_51);
                rb_state10.var_16 = rb_state10.var_32;
                pthread_mutex_unlock(&(rb_state10.lock_52));
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (92408 < rb_input_size ? (uint32_t)rb_input[92408] : 0x72d8ebcd);
            rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_1);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) + (103147 < rb_input_size ? (uint32_t)rb_input[103147] : 0x5a94c675);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_1);
            rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
            if ((rb_state14.var_0) == (0xce98e9c0)) {
                rb_state14.var_2 = rb_state14.var_1;
            }
            if ((rb_state14.var_0) == (0xce98e9c0)) {
                if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                    racebench_trigger(14);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) ^ ((0x3feb5d92) + (rb_state15.var_1));
            rb_state15.var_1 = (rb_state15.var_1) - (0x10c0e964);
            if ((rb_state15.var_0) == (0xa6c834bc)) {
                pthread_mutex_lock(&(rb_state15.lock_21));
                rb_state15.var_4 = 0x61732f5;
                rb_state15.var_5 = (rb_state15.var_5) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x473d0a4b));
                rb_state15.var_6 = (rb_state15.var_6) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_2) : (0xa8559081));
                rb_state15.var_7 = (rb_state15.var_7) ^ (9853 < rb_input_size ? (uint32_t)rb_input[9853] : 0xb03d938d);
                rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xfc85d7c4) - (rb_state15.var_5)) - (rb_state15.var_6)) ^ (rb_state15.var_5)) - (rb_state15.var_6));
                rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_7) : (0x4220dc3f));
                rb_state15.var_10 = (rb_state15.var_10) + (((0xeba9a) ^ (rb_state15.var_8)) - (0x8d15cc95));
                rb_state15.var_11 = (rb_state15.var_11) ^ (((((0xa6334a92) ^ (0xa752f9e)) + (0x27fd1e78)) + (rb_state15.var_9)) - (rb_state15.var_10));
                rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_11);
                rb_state15.var_2 = rb_state15.var_3;
                pthread_mutex_unlock(&(rb_state15.lock_21));
            }
            if ((rb_state15.var_0) == (0xa6c834bc)) {
                rb_state15.var_12 = (rb_state15.var_12) ^ (((0x575386d3) ^ (rb_state15.var_11)) ^ (rb_state15.var_9));
                rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_8) : (0xe92dcf6b));
                rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_12);
                rb_state15.var_15 = (rb_state15.var_15) - (((((0x793c621b) - (rb_state15.var_13)) + (0xd96e2763)) + (rb_state15.var_12)) - (rb_state15.var_13));
                rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_14);
                rb_state15.var_17 = (rb_state15.var_17) ^ (rb_state15.var_15);
                rb_state15.var_18 = (rb_state15.var_18) ^ (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_16) : (0x9adbecca));
                rb_state15.var_19 = (rb_state15.var_19) ^ (((0x9556f608) - (rb_state15.var_17)) ^ (0x64bf9f1c));
                rb_state15.var_20 = (rb_state15.var_20) ^ (((rb_state15.var_18) == (0x0)) ? (rb_state15.var_19) : (0x75334d8d));
                rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_20);
            }
            if ((rb_state15.var_0) == (0xa6c834bc)) {
                pthread_mutex_lock(&(rb_state15.lock_21));
                if (!((rb_state15.var_2) == (rb_state15.var_3))) {
                    racebench_trigger(15);
                }
                pthread_mutex_unlock(&(rb_state15.lock_21));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (0x353087f9);
            rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_0);
            if ((rb_state16.var_1) == (0x3971fcd7)) {
                rb_state16.var_17 = 0x8ded774f;
                rb_state16.var_18 = (rb_state16.var_18) ^ (((0xa503eeae) - (rb_state16.var_8)) - (rb_state16.var_9));
                rb_state16.var_19 = (rb_state16.var_19) ^ ((0x388e98ac) + (rb_state16.var_10));
                rb_state16.var_20 = (rb_state16.var_20) + (((0x13aede7b) - (rb_state16.var_7)) - (rb_state16.var_11));
                rb_state16.var_21 = (rb_state16.var_21) ^ (((0x621c3344) ^ (0xdf13c05c)) - (rb_state16.var_17));
                rb_state16.var_22 = (rb_state16.var_22) - (((0xd17e0903) - (rb_state16.var_12)) + (rb_state16.var_18));
                rb_state16.var_23 = (rb_state16.var_23) - (rb_state16.var_19);
                rb_state16.var_24 = (rb_state16.var_24) ^ (rb_state16.var_20);
                rb_state16.var_25 = (rb_state16.var_25) + (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_21) : (0xd64b6f46));
                rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_22);
                rb_state16.var_27 = (rb_state16.var_27) - (rb_state16.var_23);
                rb_state16.var_28 = (rb_state16.var_28) + (((0x6733588e) - (rb_state16.var_24)) ^ (rb_state16.var_14));
                rb_state16.var_29 = (rb_state16.var_29) + (((0x44ac0528) - (rb_state16.var_25)) - (0xacc7e95e));
                rb_state16.var_30 = (rb_state16.var_30) ^ (((((0x38964ccf) ^ (rb_state16.var_27)) + (rb_state16.var_15)) ^ (rb_state16.var_26)) ^ (rb_state16.var_16));
                rb_state16.var_31 = (rb_state16.var_31) - (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_28) : (0x52afd8e1));
                rb_state16.var_32 = (rb_state16.var_32) - (((((0xcc3664cf) + (rb_state16.var_18)) ^ (rb_state16.var_29)) + (rb_state16.var_30)) + (0x3e46d74d));
                rb_state16.var_33 = (rb_state16.var_33) + (rb_state16.var_31);
                rb_state16.var_34 = (rb_state16.var_34) + (((0x46d27251) - (rb_state16.var_32)) ^ (0x4075c3cc));
                rb_state16.var_35 = (rb_state16.var_35) + (rb_state16.var_33);
                rb_state16.var_36 = (rb_state16.var_36) ^ (((0xc5c3b415) + (rb_state16.var_34)) - (rb_state16.var_19));
                rb_state16.var_37 = (rb_state16.var_37) + (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_36) : (0x2f7a1920));
                rb_state16.var_16 = (rb_state16.var_16) - (rb_state16.var_37);
                rb_state16.var_2 = rb_state16.var_16;
            }
            if ((rb_state16.var_1) == (0x3971fcd7)) {
                pthread_mutex_lock(&(rb_state16.lock_45));
                rb_state16.var_39 = 0x465ff067;
                rb_state16.var_40 = (rb_state16.var_40) ^ ((0x6a454ed7) - (rb_state16.var_22));
                rb_state16.var_41 = (rb_state16.var_41) - (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_39) : (0x6d3073ed));
                rb_state16.var_42 = (rb_state16.var_42) + (((((0xd0872ee1) - (0xc4316f7)) + (rb_state16.var_20)) ^ (0x5cd0a33b)) + (rb_state16.var_40));
                rb_state16.var_43 = (rb_state16.var_43) + (rb_state16.var_41);
                rb_state16.var_44 = (rb_state16.var_44) - (((((0x45cb33b5) ^ (rb_state16.var_23)) ^ (rb_state16.var_42)) ^ (rb_state16.var_24)) ^ (rb_state16.var_43));
                rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_44);
                rb_state16.var_2 = rb_state16.var_38;
                pthread_mutex_unlock(&(rb_state16.lock_45));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0xaa78c274));
            rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_1) : (0xe4b60d01));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (0xdb3272e4);
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

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x30)) {
        if ((rb_state10.var_4) != (0x0)) {
            rb_state10.var_15 = !((rb_state10.var_2) == (rb_state10.var_3));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xce98e9c0)) {
        rb_state14.var_3 = 0xab5186ac;
        rb_state14.var_4 = (rb_state14.var_4) ^ (((((0xcb6c596b) ^ (rb_state14.var_3)) ^ (rb_state14.var_2)) - (0x26f1e48b)) - (0x592d913));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_4);
    }
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (0x290a8d27);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xc53ff3ac));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0x54bfe2ae)) ? (rb_state18.var_0) : (0xc6f7940d));
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x3971fcd7)) {
        if ((rb_state16.var_2) != (0x0)) {
            if (!((rb_state16.var_2) == (rb_state16.var_38))) {
                racebench_trigger(16);
            }
        }
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
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x4c2971f7)) {
            if ((rb_state2.var_2) != (0x0)) {
                rb_state2.var_35 = !((rb_state2.var_2) == (rb_state2.var_24));
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_1) == (0x6cef3984)) {
            rb_state6.var_2 = rb_state6.var_0;
        }
        if ((rb_state6.var_1) == (0x6cef3984)) {
            rb_state6.var_3 = 0x98a0e6df;
            rb_state6.var_4 = 0x16ef0e90;
            rb_state6.var_5 = (rb_state6.var_5) + (((0x48f93fc5) + (0xa0f4661c)) - (rb_state6.var_3));
            rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_3);
            rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_4) == (0x16ef0e90)) ? (rb_state6.var_2) : (0x68ec9918));
            rb_state6.var_8 = (rb_state6.var_8) - (((rb_state6.var_4) == (0x16ef0e90)) ? (rb_state6.var_5) : (0xde03add9));
            rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_5) == (0x514cbf02)) ? (rb_state6.var_6) : (0xb493c808));
            rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_6) == (0x98a0e6df)) ? (rb_state6.var_7) : (0xb3f6e70b));
            rb_state6.var_11 = (rb_state6.var_11) ^ (((0x4f986ed6) - (rb_state6.var_8)) + (0xaad5ca6a));
            rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_9);
            rb_state6.var_13 = (rb_state6.var_13) + (rb_state6.var_10);
            rb_state6.var_14 = (rb_state6.var_14) + (((0x1b392178) - (rb_state6.var_11)) ^ (rb_state6.var_7));
            rb_state6.var_15 = (rb_state6.var_15) - (rb_state6.var_12);
            rb_state6.var_16 = (rb_state6.var_16) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_14) : (0xcf4099c3));
            rb_state6.var_17 = (rb_state6.var_17) ^ (((0x525ccccb) ^ (0x598da3c)) + (rb_state6.var_15));
            rb_state6.var_18 = (rb_state6.var_18) ^ (rb_state6.var_16);
            rb_state6.var_19 = (rb_state6.var_19) ^ (((rb_state6.var_17) == (0xf064fdd6)) ? (rb_state6.var_18) : (0xc1b9a780));
            rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_19);
        }
        if ((rb_state6.var_1) == (0x6cef3984)) {
            if (!((rb_state6.var_0) == (rb_state6.var_2))) {
                racebench_trigger(6);
            }
        }
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
        if ((rb_state2.var_0) == (0x4c2971f7)) {
            rb_state2.var_21 = (rb_state2.var_21) ^ (((((0x3a412087) + (rb_state2.var_15)) + (rb_state2.var_14)) ^ (rb_state2.var_13)) + (0xdc89b1ea));
            rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_12) : (0x104f8a41));
            rb_state2.var_23 = (rb_state2.var_23) ^ (((((0x6be2d857) ^ (rb_state2.var_17)) - (0x5bd0c539)) - (rb_state2.var_21)) ^ (rb_state2.var_22));
            rb_state2.var_20 = (rb_state2.var_20) ^ (rb_state2.var_23);
            rb_state2.var_2 = rb_state2.var_20;
        }
        if ((rb_state2.var_0) == (0x4c2971f7)) {
            pthread_mutex_lock(&(rb_state2.lock_34));
            rb_state2.var_25 = 0xdc2f63d4;
            rb_state2.var_26 = (rb_state2.var_26) - (0x8529e846);
            rb_state2.var_27 = (rb_state2.var_27) - (((((0xa3bb821f) - (0xa6e86dbd)) + (rb_state2.var_18)) ^ (0xa38392f3)) - (rb_state2.var_25));
            rb_state2.var_28 = (rb_state2.var_28) + (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_19) : (0xf5b56276));
            rb_state2.var_29 = (rb_state2.var_29) ^ (rb_state2.var_26);
            rb_state2.var_30 = (rb_state2.var_30) - (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_27) : (0x3f38bcd3));
            rb_state2.var_31 = (rb_state2.var_31) ^ (((rb_state2.var_28) == (0x0)) ? (rb_state2.var_29) : (0x4abae887));
            rb_state2.var_32 = (rb_state2.var_32) ^ (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_30) : (0x4c01e1a1));
            rb_state2.var_33 = (rb_state2.var_33) - (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_32) : (0x656c7b05));
            rb_state2.var_24 = (rb_state2.var_24) + (rb_state2.var_33);
            rb_state2.var_2 = rb_state2.var_24;
            pthread_mutex_unlock(&(rb_state2.lock_34));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0xffffff61)) {
                if ((rb_state7.var_2) != (0x0)) {
                    if (!((rb_state7.var_2) == (rb_state7.var_18))) {
                        racebench_trigger(7);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0x6b0789e2)) {
                rb_state13.var_16 = 0xd2beaabb;
                rb_state13.var_17 = 0x8f8ee6c3;
                rb_state13.var_18 = (rb_state13.var_18) - (((0x6f39cb20) + (0xe9f5d9c4)) ^ (rb_state13.var_10));
                rb_state13.var_19 = (rb_state13.var_19) + (((0x808646cb) + (rb_state13.var_16)) - (rb_state13.var_11));
                rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_17) : (0x53fb6f6d));
                rb_state13.var_21 = (rb_state13.var_21) - (((0x382fe81a) - (rb_state13.var_13)) ^ (rb_state13.var_18));
                rb_state13.var_22 = (rb_state13.var_22) ^ (((((0x9e5252ae) ^ (rb_state13.var_20)) - (rb_state13.var_19)) ^ (rb_state13.var_14)) ^ (rb_state13.var_15));
                rb_state13.var_23 = (rb_state13.var_23) - (((0x5ce21cb3) ^ (rb_state13.var_16)) - (rb_state13.var_21));
                rb_state13.var_24 = (rb_state13.var_24) ^ (rb_state13.var_22);
                rb_state13.var_25 = (rb_state13.var_25) - (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_23) : (0xc770ef35));
                rb_state13.var_26 = (rb_state13.var_26) - (rb_state13.var_24);
                rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_26) : (0x9f891970));
                rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_27);
                rb_state13.var_2 = rb_state13.var_15;
            }
            if ((rb_state13.var_1) == (0x6b0789e2)) {
                pthread_mutex_lock(&(rb_state13.lock_33));
                rb_state13.var_29 = 0x13410b09;
                rb_state13.var_30 = (rb_state13.var_30) ^ (rb_state13.var_19);
                rb_state13.var_31 = (rb_state13.var_31) ^ (((((0x9b01b66f) - (rb_state13.var_20)) - (rb_state13.var_29)) + (rb_state13.var_21)) - (rb_state13.var_18));
                rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0x585c0033));
                rb_state13.var_28 = (rb_state13.var_28) ^ (rb_state13.var_32);
                rb_state13.var_2 = rb_state13.var_28;
                pthread_mutex_unlock(&(rb_state13.lock_33));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ ((0xb711007d) + (0xdcc7dcef));
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x39086bf3)) {
                pthread_mutex_lock(&(rb_state18.lock_26));
                rb_state18.var_3 = 0x51cc1ce;
                rb_state18.var_4 = 0x8c720d9;
                rb_state18.var_5 = (rb_state18.var_5) + ((0xb30d72c0) - (rb_state18.var_2));
                rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_4) : (0xca14637f));
                rb_state18.var_7 = (rb_state18.var_7) + (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_1) : (0x23a7a9e0));
                rb_state18.var_8 = (rb_state18.var_8) ^ (rb_state18.var_5);
                rb_state18.var_9 = (rb_state18.var_9) - (((0xad9e1319) - (rb_state18.var_6)) + (rb_state18.var_4));
                rb_state18.var_10 = (rb_state18.var_10) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0x6cf3fc18));
                rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0xc9cd5c38));
                rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_11);
                rb_state18.var_1 = rb_state18.var_2;
                pthread_mutex_unlock(&(rb_state18.lock_26));
            }
            if ((rb_state18.var_0) == (0x39086bf3)) {
                if (!((rb_state18.var_1) == (rb_state18.var_2))) {
                    racebench_trigger(18);
                }
            }
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_2
                rb_state2.var_23 = (rb_state2.var_23) - (rb_state2.var_24);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_1);
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_1) == (0x0)) {
                    rb_state12.var_3 = 0xe489977a;
                    rb_state12.var_4 = 0xad510ba3;
                    rb_state12.var_5 = 0xd583cf4d;
                    rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_4) : (0xb5d27d6d));
                    rb_state12.var_7 = (rb_state12.var_7) + (((0x2152a3ae) + (rb_state12.var_3)) ^ (rb_state12.var_3));
                    rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_5) : (0xdf5377e5));
                    rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_6);
                    rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0xecf97b5b));
                    rb_state12.var_11 = (rb_state12.var_11) - (rb_state12.var_9);
                    rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_10) : (0xd7000a6b));
                    rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_11) : (0x7287a844));
                    rb_state12.var_14 = (rb_state12.var_14) ^ (((((0xa6368e30) - (rb_state12.var_13)) + (rb_state12.var_12)) + (0xc32d3786)) - (rb_state12.var_7));
                    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_14);
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) ^ (81287 < rb_input_size ? (uint32_t)rb_input[81287] : 0x4e69bdb0);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_26 = (rb_state2.var_26) ^ ((0x50fb0f0f) - (rb_state2.var_25));
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - (92717 < rb_input_size ? (uint32_t)rb_input[92717] : 0x4821bc51);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) ^ (0xf7309c72);
                #endif
                #ifdef RACEBENCH_BUG_17
                if ((rb_state17.var_1) == (0x0)) {
                    rb_state17.var_3 = 0x3ba1ae7d;
                    rb_state17.var_4 = (rb_state17.var_4) + (((((0x9d5373d9) ^ (rb_state17.var_4)) - (rb_state17.var_2)) ^ (0xd919ad5a)) ^ (rb_state17.var_3));
                    rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_4);
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0x39086bf3)) {
                    pthread_mutex_lock(&(rb_state18.lock_26));
                    rb_state18.var_12 = 0x6ddc8b71;
                    rb_state18.var_13 = 0xc55b5ce2;
                    rb_state18.var_14 = 0xeba41337;
                    rb_state18.var_15 = (rb_state18.var_15) - (rb_state18.var_5);
                    rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_12);
                    rb_state18.var_17 = (rb_state18.var_17) ^ (((0x6a85f5ff) + (0x701b98aa)) - (rb_state18.var_14));
                    rb_state18.var_18 = (rb_state18.var_18) ^ (rb_state18.var_13);
                    rb_state18.var_19 = (rb_state18.var_19) ^ (((((0x6392593) ^ (rb_state18.var_16)) - (rb_state18.var_15)) ^ (0x18d69f48)) ^ (0x4529e660));
                    rb_state18.var_20 = (rb_state18.var_20) - (((0x10ef6345) + (rb_state18.var_17)) ^ (0x341e57c0));
                    rb_state18.var_21 = (rb_state18.var_21) + (((((0x9f289ddb) - (0xb651d1ae)) + (0x878d8c31)) + (rb_state18.var_19)) - (rb_state18.var_18));
                    rb_state18.var_22 = (rb_state18.var_22) ^ (rb_state18.var_20);
                    rb_state18.var_23 = (rb_state18.var_23) ^ (((0xbc8aca39) ^ (rb_state18.var_21)) ^ (0x2abf6fe5));
                    rb_state18.var_24 = (rb_state18.var_24) - (((rb_state18.var_6) == (0xca14637f)) ? (rb_state18.var_22) : (0x68e3762c));
                    rb_state18.var_25 = (rb_state18.var_25) ^ (((((0xb427e32c) + (0x8d7e68a9)) ^ (rb_state18.var_24)) + (rb_state18.var_23)) - (rb_state18.var_7));
                    rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_25);
                    pthread_mutex_unlock(&(rb_state18.lock_26));
                }
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) ^ (0xd47bbf35);
                #endif
                #ifdef RACEBENCH_BUG_9
                if ((rb_state9.var_0) == (0x1d6ef72b)) {
                    pthread_mutex_lock(&(rb_state9.lock_16));
                    rb_state9.var_8 = 0x901c7a97;
                    rb_state9.var_9 = 0xb1a46a3c;
                    rb_state9.var_10 = (rb_state9.var_10) - (((0x3189962a) + (rb_state9.var_8)) ^ (0x2e0ae886));
                    rb_state9.var_11 = (rb_state9.var_11) + (rb_state9.var_3);
                    rb_state9.var_12 = (rb_state9.var_12) ^ (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_9) : (0x70455d49));
                    rb_state9.var_13 = (rb_state9.var_13) ^ (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_10) : (0xae77cc2c));
                    rb_state9.var_14 = (rb_state9.var_14) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_12) : (0xd28fdcf6));
                    rb_state9.var_15 = (rb_state9.var_15) ^ (((((0x7a70442a) - (rb_state9.var_14)) ^ (rb_state9.var_13)) ^ (rb_state9.var_6)) ^ (0xb5b402ce));
                    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_15);
                    pthread_mutex_unlock(&(rb_state9.lock_16));
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_1) == (0x30)) {
                    rb_state10.var_4 = 0x1;
                }
                if ((rb_state10.var_1) == (0x30)) {
                    pthread_mutex_lock(&(rb_state10.lock_14));
                    rb_state10.var_5 = 0xa052c2fe;
                    rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_2) : (0xdfde74e1));
                    rb_state10.var_7 = (rb_state10.var_7) ^ (86813 < rb_input_size ? (uint32_t)rb_input[86813] : 0x324e1323);
                    rb_state10.var_8 = (rb_state10.var_8) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0xd8cb49db));
                    rb_state10.var_9 = (rb_state10.var_9) - (rb_state10.var_6);
                    rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0x3fa6a76e));
                    rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_9) : (0x8113f53e));
                    rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_10) : (0xb993e4b0));
                    rb_state10.var_13 = (rb_state10.var_13) - (((((0xa466fd0b) ^ (rb_state10.var_7)) - (0xf654a826)) - (rb_state10.var_12)) ^ (rb_state10.var_11));
                    rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_13);
                    rb_state10.var_2 = rb_state10.var_3;
                    pthread_mutex_unlock(&(rb_state10.lock_14));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if (!((rb_state13.var_2) == (rb_state13.var_28))) {
                    pthread_mutex_lock(&(rb_state13.lock_54));
                    rb_state13.var_35 = (rb_state13.var_35) - ((0xfcfc721c) - (rb_state13.var_26));
                    rb_state13.var_36 = (rb_state13.var_36) - (((0xa2d90fb) ^ (rb_state13.var_24)) - (rb_state13.var_27));
                    rb_state13.var_37 = (rb_state13.var_37) ^ (((0x8c666269) ^ (rb_state13.var_25)) + (0x4b67dce7));
                    rb_state13.var_38 = (rb_state13.var_38) - ((0x585befb3) - (rb_state13.var_28));
                    rb_state13.var_39 = (rb_state13.var_39) ^ (rb_state13.var_23);
                    rb_state13.var_40 = (rb_state13.var_40) - (((0x151e1f80) - (rb_state13.var_35)) - (0x498415c1));
                    rb_state13.var_41 = (rb_state13.var_41) ^ (rb_state13.var_36);
                    rb_state13.var_42 = (rb_state13.var_42) ^ (((0x810edafd) + (0x6bedde87)) ^ (rb_state13.var_37));
                    rb_state13.var_43 = (rb_state13.var_43) - (((0xc1f5b931) ^ (rb_state13.var_38)) ^ (0x8cfc4808));
                    rb_state13.var_44 = (rb_state13.var_44) + (((0xe507f508) - (0x38cb7782)) + (rb_state13.var_39));
                    rb_state13.var_45 = (rb_state13.var_45) + (((rb_state13.var_40) == (0x0)) ? (rb_state13.var_41) : (0xb24972e3));
                    rb_state13.var_46 = (rb_state13.var_46) - (rb_state13.var_42);
                    rb_state13.var_47 = (rb_state13.var_47) - (rb_state13.var_43);
                    rb_state13.var_48 = (rb_state13.var_48) + (((0xbdaf6) + (rb_state13.var_44)) - (0xf5013b5f));
                    rb_state13.var_49 = (rb_state13.var_49) ^ (((((0xf3185d5c) - (rb_state13.var_45)) ^ (0xa3534107)) + (rb_state13.var_29)) ^ (rb_state13.var_46));
                    rb_state13.var_50 = (rb_state13.var_50) - (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_47) : (0xe86897a2));
                    rb_state13.var_51 = (rb_state13.var_51) ^ (rb_state13.var_48);
                    rb_state13.var_52 = (rb_state13.var_52) - (((rb_state13.var_49) == (0x0)) ? (rb_state13.var_50) : (0x8aae93bc));
                    rb_state13.var_53 = (rb_state13.var_53) + (((((0x9cc31b40) ^ (rb_state13.var_51)) - (rb_state13.var_52)) ^ (rb_state13.var_31)) ^ (rb_state13.var_32));
                    rb_state13.var_22 = (rb_state13.var_22) + (rb_state13.var_53);
                    pthread_mutex_unlock(&(rb_state13.lock_54));
                }
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
                if ((rb_state8.var_1) == (0xb1bef0a9)) {
                    pthread_mutex_lock(&(rb_state8.lock_11));
                    rb_state8.var_3 = 0x36398782;
                    rb_state8.var_4 = (rb_state8.var_4) ^ (((0x84c8ca70) ^ (0x5a2f6867)) + (rb_state8.var_3));
                    rb_state8.var_5 = (rb_state8.var_5) + (169836 < rb_input_size ? (uint32_t)rb_input[169836] : 0x9a54918b);
                    rb_state8.var_6 = (rb_state8.var_6) - (((0xd127653b) + (rb_state8.var_4)) + (rb_state8.var_2));
                    rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0x64c62f81));
                    rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_6);
                    rb_state8.var_9 = (rb_state8.var_9) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_7) : (0xe76ccad3));
                    rb_state8.var_10 = (rb_state8.var_10) - (((((0x2cad0486) - (rb_state8.var_8)) - (rb_state8.var_9)) - (0x4589035a)) + (rb_state8.var_6));
                    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_10);
                    pthread_mutex_unlock(&(rb_state8.lock_11));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_1) == (0x6b0789e2)) {
                    pthread_mutex_lock(&(rb_state13.lock_33));
                    if ((rb_state13.var_2) != (0x0)) {
                        if (!((rb_state13.var_2) == (rb_state13.var_28))) {
                            rb_state13.var_34 = rb_state13.var_22;
                        }
                        if (!((rb_state13.var_2) == (rb_state13.var_28))) {
                            if (!((rb_state13.var_22) == (rb_state13.var_34))) {
                                racebench_trigger(13);
                            }
                        }
                    }
                    pthread_mutex_unlock(&(rb_state13.lock_33));
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) - ((0x137dfd34) + (rb_state19.var_1));
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_0 = (rb_state12.var_0) + (0x5df8ac1d);
                    if ((rb_state12.var_1) == (0x0)) {
                        rb_state12.var_2 = rb_state12.var_0;
                    }
                    if ((rb_state12.var_1) == (0x0)) {
                        if (!((rb_state12.var_0) == (rb_state12.var_2))) {
                            racebench_trigger(12);
                        }
                    }
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
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_11) == (0x8113f53e)) {
            if ((rb_state10.var_16) != (0x0)) {
                if (!((rb_state10.var_16) == (rb_state10.var_32))) {
                    racebench_trigger(10);
                }
            }
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

    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xffffff61)) {
        pthread_mutex_lock(&(rb_state7.lock_33));
        rb_state7.var_8 = 0x3cb0391b;
        rb_state7.var_9 = (rb_state7.var_9) - (((0x35c874c4) ^ (rb_state7.var_7)) - (0x3b85f1de));
        rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_8);
        rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_9);
        rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_9);
        rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_10);
        rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0xd7967115));
        rb_state7.var_15 = (rb_state7.var_15) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_13) : (0xdfc0a662));
        rb_state7.var_16 = (rb_state7.var_16) + (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_14) : (0xa2cc6dd2));
        rb_state7.var_17 = (rb_state7.var_17) - (((((0xb2a02579) - (rb_state7.var_12)) + (rb_state7.var_16)) - (rb_state7.var_15)) - (0xcdb30f08));
        rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_17);
        rb_state7.var_2 = rb_state7.var_7;
        pthread_mutex_unlock(&(rb_state7.lock_33));
    }
    if ((rb_state7.var_0) == (0xffffff61)) {
        pthread_mutex_lock(&(rb_state7.lock_33));
        rb_state7.var_19 = (rb_state7.var_19) + ((0x8adc2b56) - (rb_state7.var_16));
        rb_state7.var_20 = (rb_state7.var_20) - (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_17) : (0xbccb9b5b));
        rb_state7.var_21 = (rb_state7.var_21) + (((((0x12f72f16) ^ (rb_state7.var_15)) ^ (0xff041abd)) + (rb_state7.var_14)) + (rb_state7.var_19));
        rb_state7.var_22 = (rb_state7.var_22) + (0x920fc49f);
        rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_13) : (0xde1b673));
        rb_state7.var_24 = (rb_state7.var_24) ^ (rb_state7.var_19);
        rb_state7.var_25 = (rb_state7.var_25) + (rb_state7.var_20);
        rb_state7.var_26 = (rb_state7.var_26) - (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_22) : (0xe9c52be8));
        rb_state7.var_27 = (rb_state7.var_27) - (((0x30063f86) ^ (rb_state7.var_23)) ^ (rb_state7.var_21));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_25) : (0x7456fa));
        rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_22) == (0x0)) ? (rb_state7.var_26) : (0xe373eb79));
        rb_state7.var_30 = (rb_state7.var_30) ^ (((0xfaec7e6e) ^ (rb_state7.var_27)) - (0x155e7500));
        rb_state7.var_31 = (rb_state7.var_31) + (((rb_state7.var_28) == (0x0)) ? (rb_state7.var_29) : (0x5aa0cd0a));
        rb_state7.var_32 = (rb_state7.var_32) + (((((0x2550cbe3) ^ (rb_state7.var_31)) ^ (rb_state7.var_23)) ^ (rb_state7.var_30)) ^ (rb_state7.var_24));
        rb_state7.var_18 = (rb_state7.var_18) - (rb_state7.var_32);
        rb_state7.var_2 = rb_state7.var_18;
        pthread_mutex_unlock(&(rb_state7.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x1d6ef72b)) {
        pthread_mutex_lock(&(rb_state9.lock_16));
        rb_state9.var_4 = 0x86c6443e;
        rb_state9.var_5 = 0x7c0f28a2;
        rb_state9.var_6 = (rb_state9.var_6) + (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_5) : (0x5628abec));
        rb_state9.var_7 = (rb_state9.var_7) ^ (((rb_state9.var_4) == (0x86c6443e)) ? (rb_state9.var_6) : (0xe267d523));
        rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_7);
        rb_state9.var_2 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_16));
    }
    if ((rb_state9.var_0) == (0x1d6ef72b)) {
        pthread_mutex_lock(&(rb_state9.lock_16));
        if (!((rb_state9.var_2) == (rb_state9.var_3))) {
            racebench_trigger(9);
        }
        pthread_mutex_unlock(&(rb_state9.lock_16));
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