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
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - ((0x2d33170a) + (rb_state8.var_1));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) + (0xc4fd3254);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_1) : (0xc6701cd0));
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_1) == (0x8acbc088)) {
            rb_state13.var_2 = rb_state13.var_0;
        }
        if ((rb_state13.var_1) == (0x8acbc088)) {
            if (!((rb_state13.var_0) == (rb_state13.var_2))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (114557 < rb_input_size ? (uint32_t)rb_input[114557] : 0x78f3aa80);
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_1);
            rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x22f3041c));
            rb_state1.var_0 = (rb_state1.var_0) ^ ((0x297d2986) + (0xd17d9e74));
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_1) == (0xa)) {
                rb_state2.var_19 = 0x64e0838b;
                rb_state2.var_20 = (rb_state2.var_20) ^ (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_19) : (0x6f9499f8));
                rb_state2.var_21 = (rb_state2.var_21) + (((((0xe2ab8a49) ^ (rb_state2.var_9)) + (0x7ecbed33)) ^ (rb_state2.var_8)) ^ (0x15ee1b84));
                rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_20);
                rb_state2.var_23 = (rb_state2.var_23) ^ (((0xc264bee5) - (0xb8710c21)) ^ (rb_state2.var_21));
                rb_state2.var_24 = (rb_state2.var_24) + (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_22) : (0x50f0b26c));
                rb_state2.var_25 = (rb_state2.var_25) ^ (((rb_state2.var_23) == (0x0)) ? (rb_state2.var_24) : (0x320d83bf));
                rb_state2.var_18 = (rb_state2.var_18) ^ (rb_state2.var_25);
                rb_state2.var_2 = rb_state2.var_18;
            }
            if ((rb_state2.var_1) == (0xa)) {
                pthread_mutex_lock(&(rb_state2.lock_37));
                rb_state2.var_27 = 0xbeb6e4f0;
                rb_state2.var_28 = (rb_state2.var_28) + ((0xd0881bda) - (rb_state2.var_14));
                rb_state2.var_29 = (rb_state2.var_29) + (((0x9606ecdd) - (rb_state2.var_27)) + (0x8fb94262));
                rb_state2.var_30 = (rb_state2.var_30) + (0x57e24586);
                rb_state2.var_31 = (rb_state2.var_31) ^ (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_12) : (0x3a17bbee));
                rb_state2.var_32 = (rb_state2.var_32) + (((((0x3615feb9) - (rb_state2.var_28)) + (rb_state2.var_16)) - (rb_state2.var_15)) + (rb_state2.var_29));
                rb_state2.var_33 = (rb_state2.var_33) ^ (((rb_state2.var_30) == (0x0)) ? (rb_state2.var_31) : (0xbc716ae2));
                rb_state2.var_34 = (rb_state2.var_34) ^ (rb_state2.var_32);
                rb_state2.var_35 = (rb_state2.var_35) ^ (((0xedfac387) ^ (rb_state2.var_33)) ^ (0x6d303bc6));
                rb_state2.var_36 = (rb_state2.var_36) ^ (((rb_state2.var_34) == (0x0)) ? (rb_state2.var_35) : (0xbc1c59e0));
                rb_state2.var_26 = (rb_state2.var_26) ^ (rb_state2.var_36);
                rb_state2.var_2 = rb_state2.var_26;
                pthread_mutex_unlock(&(rb_state2.lock_37));
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ ((0xc3c0b938) - (rb_state3.var_0));
            rb_state3.var_0 = (rb_state3.var_0) + (0x27dc47f5);
            if ((rb_state3.var_1) == (0x0)) {
                rb_state3.var_3 = 0x19017de5;
                rb_state3.var_4 = 0xa22d74fe;
                rb_state3.var_5 = (rb_state3.var_5) ^ (0x6a6d94cc);
                rb_state3.var_6 = (rb_state3.var_6) ^ ((0x79ba081d) - (rb_state3.var_3));
                rb_state3.var_7 = (rb_state3.var_7) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_4) : (0x525b303a));
                rb_state3.var_8 = (rb_state3.var_8) + (((0xcb597659) ^ (rb_state3.var_5)) + (rb_state3.var_3));
                rb_state3.var_9 = (rb_state3.var_9) - (rb_state3.var_2);
                rb_state3.var_10 = (rb_state3.var_10) - (((0xe9102063) ^ (rb_state3.var_5)) - (rb_state3.var_6));
                rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xe2268b62));
                rb_state3.var_12 = (rb_state3.var_12) ^ (((((0x5d4ecabb) ^ (rb_state3.var_8)) + (rb_state3.var_7)) + (0x2dce8efe)) + (rb_state3.var_9));
                rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_11) : (0x5955b5a5));
                rb_state3.var_14 = (rb_state3.var_14) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_13) : (0xcd3866dd));
                rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_14);
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_0);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (0x7aca474e);
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_1) == (0x32d1181d)) {
                rb_state7.var_6 = 0xec64a008;
                rb_state7.var_7 = (rb_state7.var_7) - (((0x22e1f9e2) - (rb_state7.var_6)) + (0xf632ea87));
                rb_state7.var_8 = (rb_state7.var_8) + (((((0xc9917369) + (rb_state7.var_7)) - (0x5867c811)) + (0x3bf0493)) ^ (rb_state7.var_5));
                rb_state7.var_5 = (rb_state7.var_5) - (rb_state7.var_8);
                rb_state7.var_2 = rb_state7.var_5;
            }
            if ((rb_state7.var_1) == (0x32d1181d)) {
                if ((rb_state7.var_2) != (0x0)) {
                    if (!((rb_state7.var_2) == (rb_state7.var_9))) {
                        racebench_trigger(7);
                    }
                }
            }
            if ((rb_state7.var_1) == (0x32d1181d)) {
                pthread_mutex_lock(&(rb_state7.lock_16));
                rb_state7.var_10 = 0xff17116e;
                rb_state7.var_11 = 0x4427b62d;
                rb_state7.var_12 = 0xdcaf7818;
                rb_state7.var_13 = (rb_state7.var_13) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x524f4965));
                rb_state7.var_14 = (rb_state7.var_14) - (rb_state7.var_10);
                rb_state7.var_15 = (rb_state7.var_15) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_14) : (0xc3c1f5ed));
                rb_state7.var_9 = (rb_state7.var_9) ^ (rb_state7.var_15);
                rb_state7.var_2 = rb_state7.var_9;
                pthread_mutex_unlock(&(rb_state7.lock_16));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x20e31cec)) ? (rb_state8.var_1) : (0x98440113));
            if ((rb_state8.var_1) == (0x63dbd944)) {
                rb_state8.var_15 = 0x782786dd;
                rb_state8.var_16 = 0x2d6c1c6a;
                rb_state8.var_17 = (rb_state8.var_17) + (((((0x56ca5739) + (rb_state8.var_5)) ^ (0xd49f13b7)) + (rb_state8.var_16)) + (0x30171231));
                rb_state8.var_18 = (rb_state8.var_18) + ((0xc15155ae) + (0x9f8b05ef));
                rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_15) : (0xcd10d9fa));
                rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_17);
                rb_state8.var_21 = (rb_state8.var_21) - (rb_state8.var_18);
                rb_state8.var_22 = (rb_state8.var_22) - (((0x59b9c5f1) ^ (0x7702b688)) ^ (rb_state8.var_19));
                rb_state8.var_23 = (rb_state8.var_23) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_20) : (0x739cf0da));
                rb_state8.var_24 = (rb_state8.var_24) ^ (((0xb8909bec) ^ (0x6779b9d3)) - (rb_state8.var_21));
                rb_state8.var_25 = (rb_state8.var_25) - (rb_state8.var_22);
                rb_state8.var_26 = (rb_state8.var_26) - (((0x4045057e) ^ (rb_state8.var_23)) - (0xac099b60));
                rb_state8.var_27 = (rb_state8.var_27) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_24) : (0x5a475845));
                rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_25) : (0x2b391959));
                rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_26) : (0xf4c9f2b3));
                rb_state8.var_30 = (rb_state8.var_30) ^ (((((0x1aba0d10) - (0xb117fbb6)) ^ (rb_state8.var_27)) - (rb_state8.var_28)) ^ (rb_state8.var_11));
                rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_29) == (0x0)) ? (rb_state8.var_30) : (0x8cfe4961));
                rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_31);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_0) == (0x421c2304)) {
                pthread_mutex_lock(&(rb_state10.lock_34));
                rb_state10.var_15 = 0x5e923dd8;
                rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_9) : (0x9962df2a));
                rb_state10.var_17 = (rb_state10.var_17) - (rb_state10.var_7);
                rb_state10.var_18 = (rb_state10.var_18) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_15) : (0xb3add64a));
                rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_16);
                rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_17) : (0x157681f1));
                rb_state10.var_21 = (rb_state10.var_21) + (((0x6d27efa5) + (rb_state10.var_12)) ^ (rb_state10.var_18));
                rb_state10.var_22 = (rb_state10.var_22) ^ (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_19) : (0x1f05dcf8));
                rb_state10.var_23 = (rb_state10.var_23) + (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_20) : (0x801db87));
                rb_state10.var_24 = (rb_state10.var_24) + (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_22) : (0xffb7c7ca));
                rb_state10.var_25 = (rb_state10.var_25) - (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_24) : (0xe1801738));
                rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_25);
                rb_state10.var_2 = rb_state10.var_14;
                pthread_mutex_unlock(&(rb_state10.lock_34));
            }
            if (!((rb_state10.var_2) == (rb_state10.var_26))) {
                rb_state10.var_48 = 0x92947420;
                rb_state10.var_49 = 0x60f24e20;
                rb_state10.var_50 = (rb_state10.var_50) + (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_25) : (0xf23c1103));
                rb_state10.var_51 = (rb_state10.var_51) - ((0x6b449796) ^ (0x3ee30d39));
                rb_state10.var_52 = (rb_state10.var_52) + (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_48) : (0x2f2c38a4));
                rb_state10.var_53 = (rb_state10.var_53) + (((0x4f128c21) - (rb_state10.var_49)) - (rb_state10.var_28));
                rb_state10.var_54 = (rb_state10.var_54) ^ (rb_state10.var_50);
                rb_state10.var_55 = (rb_state10.var_55) ^ (((((0x52a89787) ^ (rb_state10.var_52)) + (rb_state10.var_30)) ^ (rb_state10.var_51)) - (rb_state10.var_29));
                rb_state10.var_56 = (rb_state10.var_56) ^ (rb_state10.var_53);
                rb_state10.var_57 = (rb_state10.var_57) ^ (((0x4e753053) ^ (rb_state10.var_54)) ^ (0x77ad088c));
                rb_state10.var_58 = (rb_state10.var_58) + (((0xdf77f805) + (rb_state10.var_31)) - (rb_state10.var_55));
                rb_state10.var_59 = (rb_state10.var_59) + (((((0xb2c6d34e) + (rb_state10.var_57)) + (0x2c1e12c1)) + (0x15e97209)) ^ (rb_state10.var_56));
                rb_state10.var_60 = (rb_state10.var_60) + (rb_state10.var_58);
                rb_state10.var_61 = (rb_state10.var_61) - (((rb_state10.var_59) == (0x0)) ? (rb_state10.var_60) : (0xde70b883));
                rb_state10.var_35 = (rb_state10.var_35) - (rb_state10.var_61);
            }
            if ((rb_state10.var_0) == (0x421c2304)) {
                pthread_mutex_lock(&(rb_state10.lock_34));
                rb_state10.var_27 = 0xf9bc6ee1;
                rb_state10.var_28 = (rb_state10.var_28) - (0x65a9ea41);
                rb_state10.var_29 = (rb_state10.var_29) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_27) : (0x4ac52c6c));
                rb_state10.var_30 = (rb_state10.var_30) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_15) : (0xe1e73b4));
                rb_state10.var_31 = (rb_state10.var_31) - (((((0xef779461) ^ (rb_state10.var_18)) - (rb_state10.var_29)) - (rb_state10.var_28)) + (0x430ef6a6));
                rb_state10.var_32 = (rb_state10.var_32) - (((0x840d47c0) - (rb_state10.var_30)) + (0x140bcfac));
                rb_state10.var_33 = (rb_state10.var_33) ^ (((rb_state10.var_31) == (0x0)) ? (rb_state10.var_32) : (0x549eb572));
                rb_state10.var_26 = (rb_state10.var_26) - (rb_state10.var_33);
                rb_state10.var_2 = rb_state10.var_26;
                pthread_mutex_unlock(&(rb_state10.lock_34));
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (0xcecf5332);
            rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0xbb93ad7e)) ? (rb_state12.var_0) : (0xbff5134b));
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_0) == (0x4a1d6560)) {
                rb_state14.var_14 = 0x50c660cd;
                rb_state14.var_15 = (rb_state14.var_15) ^ (rb_state14.var_8);
                rb_state14.var_16 = (rb_state14.var_16) - (0x671d8955);
                rb_state14.var_17 = (rb_state14.var_17) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_7) : (0x1cfb825b));
                rb_state14.var_18 = (rb_state14.var_18) ^ (rb_state14.var_14);
                rb_state14.var_19 = (rb_state14.var_19) - (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_16) : (0x9756f845));
                rb_state14.var_20 = (rb_state14.var_20) ^ (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_17) : (0xaf1f879a));
                rb_state14.var_21 = (rb_state14.var_21) ^ (((0x10a7d620) - (rb_state14.var_18)) - (rb_state14.var_11));
                rb_state14.var_22 = (rb_state14.var_22) ^ (((((0x9223445a) - (0xef7293c8)) + (rb_state14.var_19)) + (rb_state14.var_20)) - (0xd1e1dc62));
                rb_state14.var_23 = (rb_state14.var_23) ^ (rb_state14.var_21);
                rb_state14.var_24 = (rb_state14.var_24) + (((((0xd447c57) ^ (rb_state14.var_12)) + (rb_state14.var_23)) ^ (0xcfd0997e)) ^ (rb_state14.var_22));
                rb_state14.var_13 = (rb_state14.var_13) ^ (rb_state14.var_24);
                rb_state14.var_1 = rb_state14.var_13;
            }
            if ((rb_state14.var_0) == (0x4a1d6560)) {
                if ((rb_state14.var_1) != (0x0)) {
                    if (!((rb_state14.var_1) == (rb_state14.var_25))) {
                        racebench_trigger(14);
                    }
                }
            }
            if ((rb_state14.var_0) == (0x4a1d6560)) {
                pthread_mutex_lock(&(rb_state14.lock_32));
                rb_state14.var_26 = 0xac84c45f;
                rb_state14.var_27 = (rb_state14.var_27) - (27709 < rb_input_size ? (uint32_t)rb_input[27709] : 0x8c6cfd7d);
                rb_state14.var_28 = (rb_state14.var_28) - (((((0x19721539) ^ (rb_state14.var_26)) ^ (rb_state14.var_14)) ^ (0xa5b9f518)) + (0xdf0820d5));
                rb_state14.var_29 = (rb_state14.var_29) - (((((0xb158ec25) ^ (rb_state14.var_13)) ^ (rb_state14.var_16)) ^ (rb_state14.var_27)) + (rb_state14.var_15));
                rb_state14.var_30 = (rb_state14.var_30) - (((0xa38e8920) - (0x8564eee4)) - (rb_state14.var_28));
                rb_state14.var_31 = (rb_state14.var_31) + (((((0x60169e18) ^ (0xf084c93c)) + (rb_state14.var_17)) - (rb_state14.var_30)) + (rb_state14.var_29));
                rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_31);
                rb_state14.var_1 = rb_state14.var_25;
                pthread_mutex_unlock(&(rb_state14.lock_32));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
            rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xaa555050));
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_1) == (0x1d1febf0)) {
                rb_state18.var_10 = 0x2039bc93;
                rb_state18.var_11 = 0xf27ddd75;
                rb_state18.var_12 = (rb_state18.var_12) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_5) : (0x56827778));
                rb_state18.var_13 = (rb_state18.var_13) - (((((0xeb427417) ^ (rb_state18.var_11)) ^ (rb_state18.var_12)) ^ (0x7854b5b)) - (0x97f12295));
                rb_state18.var_9 = (rb_state18.var_9) + (rb_state18.var_13);
                rb_state18.var_2 = rb_state18.var_9;
            }
            if ((rb_state18.var_1) == (0x1d1febf0)) {
                pthread_mutex_lock(&(rb_state18.lock_19));
                rb_state18.var_15 = 0x38435e81;
                rb_state18.var_16 = (rb_state18.var_16) ^ (((((0xda514f88) ^ (rb_state18.var_9)) - (rb_state18.var_15)) ^ (rb_state18.var_8)) + (rb_state18.var_7));
                rb_state18.var_17 = (rb_state18.var_17) ^ (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_6) : (0x2adf2152));
                rb_state18.var_18 = (rb_state18.var_18) - (((((0x85089df2) + (rb_state18.var_17)) ^ (0xceff4014)) ^ (rb_state18.var_16)) + (rb_state18.var_11));
                rb_state18.var_14 = (rb_state18.var_14) - (rb_state18.var_18);
                rb_state18.var_2 = rb_state18.var_14;
                pthread_mutex_unlock(&(rb_state18.lock_19));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) - (63861 < rb_input_size ? (uint32_t)rb_input[63861] : 0xe90a5e2a);
            rb_state19.var_0 = (rb_state19.var_0) ^ ((0x119ee3a7) + (0xf57337fa));
            if ((rb_state19.var_0) == (0x416518a1)) {
                rb_state19.var_2 = rb_state19.var_1;
            }
            if ((rb_state19.var_0) == (0x416518a1)) {
                if (!((rb_state19.var_1) == (rb_state19.var_2))) {
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

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xed022ad0));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (14084 < rb_input_size ? (uint32_t)rb_input[14084] : 0x64fb832e);
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x0)) {
        rb_state12.var_3 = 0x1124998a;
        rb_state12.var_4 = 0xb9c93aa9;
        rb_state12.var_5 = (rb_state12.var_5) ^ (rb_state12.var_3);
        rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_4) : (0x41364dd8));
        rb_state12.var_7 = (rb_state12.var_7) + (((((0xa32c7431) + (rb_state12.var_2)) ^ (rb_state12.var_3)) ^ (rb_state12.var_5)) - (0xce404da0));
        rb_state12.var_8 = (rb_state12.var_8) + (((0xd458e925) ^ (rb_state12.var_5)) ^ (rb_state12.var_6));
        rb_state12.var_9 = (rb_state12.var_9) - (rb_state12.var_6);
        rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0x592002cd));
        rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0x672b200f));
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_1) : (0x95276639));
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
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x416518a1)) {
            rb_state19.var_3 = 0xa08d4116;
            rb_state19.var_4 = 0xcbe448a5;
            rb_state19.var_5 = (rb_state19.var_5) ^ (((((0x4fe28d7e) - (0x54a22ee8)) ^ (rb_state19.var_4)) - (rb_state19.var_2)) - (rb_state19.var_3));
            rb_state19.var_6 = (rb_state19.var_6) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_3) : (0x3c379cfa));
            rb_state19.var_7 = (rb_state19.var_7) ^ (((0x65029f44) - (rb_state19.var_5)) ^ (rb_state19.var_5));
            rb_state19.var_8 = (rb_state19.var_8) + (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_6) : (0xdaf9d3e6));
            rb_state19.var_9 = (rb_state19.var_9) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_7) : (0xdbb336c5));
            rb_state19.var_10 = (rb_state19.var_10) ^ (((((0x4716d0c0) + (rb_state19.var_8)) ^ (rb_state19.var_8)) + (rb_state19.var_9)) - (rb_state19.var_9));
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_10);
        }
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_1);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_1 = (rb_state19.var_1) + (0xc27a7d8d);
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_1) : (0xdc828a1a));
                #endif
                SIZE(result) = alloc_size;
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_12
                    if ((rb_state12.var_0) == (0x0)) {
                        rb_state12.var_2 = rb_state12.var_1;
                    }
                    if ((rb_state12.var_0) == (0x0)) {
                        if (!((rb_state12.var_1) == (rb_state12.var_2))) {
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

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0xd570164e) ^ (rb_state5.var_1));
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