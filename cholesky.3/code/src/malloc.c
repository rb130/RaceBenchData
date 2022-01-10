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
        rb_state5.var_1 = (rb_state5.var_1) ^ (0x9ec4a9c9);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_17 = (rb_state11.var_17) ^ (((rb_state11.var_19) == (0x591aa98a)) ? (rb_state11.var_18) : (0x5c83d149));
        if ((rb_state11.var_22) == (0xea54a57b)) {
            rb_state11.var_49 = 0xfdf54c1;
            rb_state11.var_50 = 0xf1a2b834;
            rb_state11.var_51 = (rb_state11.var_51) + (0x9824b24a);
            rb_state11.var_52 = (rb_state11.var_52) - (((0xae05fd66) ^ (rb_state11.var_34)) ^ (rb_state11.var_35));
            rb_state11.var_53 = (rb_state11.var_53) - (((rb_state11.var_50) == (0x0)) ? (rb_state11.var_49) : (0x64382db4));
            rb_state11.var_54 = (rb_state11.var_54) - (rb_state11.var_51);
            rb_state11.var_55 = (rb_state11.var_55) ^ (((((0xedd0cf91) ^ (rb_state11.var_37)) ^ (rb_state11.var_53)) + (rb_state11.var_36)) + (rb_state11.var_52));
            rb_state11.var_56 = (rb_state11.var_56) - (((rb_state11.var_38) == (0x0)) ? (rb_state11.var_54) : (0x2d9c5a3e));
            rb_state11.var_57 = (rb_state11.var_57) + (((0x33808763) ^ (rb_state11.var_55)) ^ (rb_state11.var_39));
            rb_state11.var_58 = (rb_state11.var_58) + (((rb_state11.var_40) == (0x0)) ? (rb_state11.var_56) : (0x812eb586));
            rb_state11.var_59 = (rb_state11.var_59) - (((rb_state11.var_57) == (0x0)) ? (rb_state11.var_58) : (0x30f0f972));
            rb_state11.var_48 = (rb_state11.var_48) - (rb_state11.var_59);
            rb_state11.var_29 = rb_state11.var_48;
        }
        if ((rb_state11.var_22) == (0xea54a57b)) {
            pthread_mutex_lock(&(rb_state11.lock_75));
            rb_state11.var_61 = (rb_state11.var_61) ^ (((0x8530631b) - (rb_state11.var_41)) ^ (rb_state11.var_44));
            rb_state11.var_62 = (rb_state11.var_62) ^ (66994 < rb_input_size ? (uint32_t)rb_input[66994] : 0x5e57b06c);
            rb_state11.var_63 = (rb_state11.var_63) + (((rb_state11.var_42) == (0x0)) ? (rb_state11.var_43) : (0xc70b9dd3));
            rb_state11.var_64 = (rb_state11.var_64) - (((rb_state11.var_46) == (0x0)) ? (rb_state11.var_45) : (0x1f74751e));
            rb_state11.var_65 = (rb_state11.var_65) + (((rb_state11.var_47) == (0x0)) ? (rb_state11.var_61) : (0x2503e196));
            rb_state11.var_66 = (rb_state11.var_66) ^ (((0x50a24024) + (rb_state11.var_48)) - (rb_state11.var_62));
            rb_state11.var_67 = (rb_state11.var_67) + (((0xaf8b3737) - (rb_state11.var_49)) ^ (rb_state11.var_63));
            rb_state11.var_68 = (rb_state11.var_68) - (((0x4fc7290f) + (0xa1e19699)) ^ (rb_state11.var_64));
            rb_state11.var_69 = (rb_state11.var_69) - (((((0xe3136ea7) + (rb_state11.var_50)) - (rb_state11.var_66)) + (rb_state11.var_51)) + (rb_state11.var_65));
            rb_state11.var_70 = (rb_state11.var_70) - (rb_state11.var_67);
            rb_state11.var_71 = (rb_state11.var_71) - (((0x591c2d2a) ^ (rb_state11.var_52)) - (rb_state11.var_68));
            rb_state11.var_72 = (rb_state11.var_72) + (((rb_state11.var_53) == (0x0)) ? (rb_state11.var_69) : (0x6fb97d4c));
            rb_state11.var_73 = (rb_state11.var_73) ^ (((((0x2b41e0dc) ^ (rb_state11.var_71)) ^ (0x6d7b9006)) + (rb_state11.var_70)) + (0x81156ced));
            rb_state11.var_74 = (rb_state11.var_74) - (((rb_state11.var_72) == (0x0)) ? (rb_state11.var_73) : (0xbe5302e5));
            rb_state11.var_60 = (rb_state11.var_60) + (rb_state11.var_74);
            rb_state11.var_29 = rb_state11.var_60;
            pthread_mutex_unlock(&(rb_state11.lock_75));
        }
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) ^ ((0x1dc7152) + (0x154872a2));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) - (((rb_state8.var_2) == (0xf33d9be6)) ? (rb_state8.var_1) : (0xc2c5b0a8));
            rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_1);
            if ((rb_state8.var_2) == (0xb60529a6)) {
                rb_state8.var_4 = 0x67c706c1;
                rb_state8.var_5 = 0x16030eb7;
                rb_state8.var_6 = 0xacfea253;
                rb_state8.var_7 = (rb_state8.var_7) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_3) : (0xa2986fe0));
                rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0x85300cfe));
                rb_state8.var_9 = (rb_state8.var_9) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0xecfe3095));
                rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0xbca358ce));
                rb_state8.var_11 = (rb_state8.var_11) - (rb_state8.var_8);
                rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_9) : (0x2e565cca));
                rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_10) : (0x7acba2b8));
                rb_state8.var_14 = (rb_state8.var_14) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_11) : (0x42110777));
                rb_state8.var_15 = (rb_state8.var_15) - (rb_state8.var_12);
                rb_state8.var_16 = (rb_state8.var_16) + (((0x58a9ede1) ^ (rb_state8.var_10)) + (rb_state8.var_13));
                rb_state8.var_17 = (rb_state8.var_17) - (((0x51d20bde) + (0xb184cfad)) + (rb_state8.var_14));
                rb_state8.var_18 = (rb_state8.var_18) ^ (((((0x9e49f526) - (rb_state8.var_11)) + (rb_state8.var_15)) - (rb_state8.var_12)) + (rb_state8.var_16));
                rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_17) : (0xc971268e));
                rb_state8.var_20 = (rb_state8.var_20) ^ (((((0x36f79041) + (0xed2eea6)) - (rb_state8.var_18)) - (rb_state8.var_19)) - (0xcfc42d68));
                rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_1) == (0xfffffecb)) {
                pthread_mutex_lock(&(rb_state9.lock_26));
                rb_state9.var_5 = 0x79fcc33e;
                rb_state9.var_6 = (rb_state9.var_6) ^ (rb_state9.var_5);
                rb_state9.var_7 = (rb_state9.var_7) - (((0xe217ade3) + (rb_state9.var_5)) + (rb_state9.var_4));
                rb_state9.var_8 = (rb_state9.var_8) ^ (((rb_state9.var_7) == (0xa3eb8edf)) ? (rb_state9.var_6) : (0x42df178a));
                rb_state9.var_9 = (rb_state9.var_9) - (((((0x44063569) - (0x20afdd7b)) ^ (rb_state9.var_3)) + (rb_state9.var_8)) - (rb_state9.var_6));
                rb_state9.var_10 = (rb_state9.var_10) + (((0xce0113a1) + (rb_state9.var_7)) + (0xa96e0467));
                rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_9) == (0xdca9a812)) ? (rb_state9.var_8) : (0xe8ace9af));
                rb_state9.var_12 = (rb_state9.var_12) + (((((0x57b7ca4b) - (rb_state9.var_10)) ^ (0xda6f9eed)) ^ (rb_state9.var_9)) ^ (rb_state9.var_10));
                rb_state9.var_13 = (rb_state9.var_13) - (rb_state9.var_11);
                rb_state9.var_14 = (rb_state9.var_14) + (((rb_state9.var_12) == (0x21c1b37c)) ? (rb_state9.var_13) : (0x59f5108c));
                rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_14);
                rb_state9.var_3 = rb_state9.var_4;
                pthread_mutex_unlock(&(rb_state9.lock_26));
            }
            if ((rb_state9.var_1) == (0xfffffecb)) {
                if (!((rb_state9.var_3) == (rb_state9.var_4))) {
                    racebench_trigger(9);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x214498b4));
            rb_state10.var_1 = (rb_state10.var_1) ^ ((0xc3de74c5) ^ (rb_state10.var_1));
            if ((rb_state10.var_1) == (0xc3de74c5)) {
                rb_state10.var_2 = rb_state10.var_0;
            }
            if ((rb_state10.var_1) == (0xc3de74c5)) {
                if (!((rb_state10.var_0) == (rb_state10.var_2))) {
                    racebench_trigger(10);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (24665 < rb_input_size ? (uint32_t)rb_input[24665] : 0x6a899653);
            rb_state13.var_0 = (rb_state13.var_0) ^ ((0xd6f0fa8e) + (rb_state13.var_0));
            rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_1);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (70730 < rb_input_size ? (uint32_t)rb_input[70730] : 0xd3eba0a7);
            rb_state15.var_1 = (rb_state15.var_1) + ((0x7e258498) ^ (rb_state15.var_1));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (0x9977f827);
            rb_state17.var_1 = (rb_state17.var_1) + ((0x4b99956a) + (rb_state17.var_1));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
            if ((rb_state19.var_1) == (0x56855400)) {
                pthread_mutex_lock(&(rb_state19.lock_27));
                if ((rb_state19.var_2) != (0x0)) {
                    if (!((rb_state19.var_2) == (rb_state19.var_15))) {
                        racebench_trigger(19);
                    }
                }
                pthread_mutex_unlock(&(rb_state19.lock_27));
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

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0xf62d9fc5)) {
        rb_state3.var_3 = 0x37095342;
        rb_state3.var_4 = (rb_state3.var_4) + (((0x7a41abfe) ^ (rb_state3.var_3)) - (rb_state3.var_3));
        rb_state3.var_5 = (rb_state3.var_5) - (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_4) : (0xf0d4396d));
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x19b5fe6d)) {
        pthread_mutex_lock(&(rb_state5.lock_20));
        rb_state5.var_10 = 0xf323a644;
        rb_state5.var_11 = (rb_state5.var_11) ^ (rb_state5.var_10);
        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_9) : (0x8c3a7984));
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_2) : (0x22d2bbba));
        rb_state5.var_14 = (rb_state5.var_14) - (((0x9f6378a) - (0xa2a85883)) + (rb_state5.var_10));
        rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_11);
        rb_state5.var_16 = (rb_state5.var_16) ^ (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_13) : (0x9fd2e5b4));
        rb_state5.var_17 = (rb_state5.var_17) - (((((0xe5a000f7) ^ (rb_state5.var_13)) + (rb_state5.var_14)) ^ (0x4f0dc3d3)) - (rb_state5.var_15));
        rb_state5.var_18 = (rb_state5.var_18) + (rb_state5.var_16);
        rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_18) : (0x86715cef));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_19);
        pthread_mutex_unlock(&(rb_state5.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0x9abd0395)) {
        pthread_mutex_lock(&(rb_state7.lock_17));
        rb_state7.var_4 = 0xa99753e1;
        rb_state7.var_5 = 0x1e4ae2;
        rb_state7.var_6 = 0x7bef6a2e;
        rb_state7.var_7 = (rb_state7.var_7) + (((0x75ee2a47) + (rb_state7.var_5)) - (rb_state7.var_3));
        rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_4) == (0xa99753e1)) ? (rb_state7.var_4) : (0xa7fe5489));
        rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_5) == (0x1e4ae2)) ? (rb_state7.var_6) : (0x35a118a8));
        rb_state7.var_10 = (rb_state7.var_10) + (rb_state7.var_7);
        rb_state7.var_11 = (rb_state7.var_11) ^ (((((0x8e590ff3) + (rb_state7.var_9)) - (rb_state7.var_6)) + (rb_state7.var_8)) ^ (0x604b1e7d));
        rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_7) == (0x760c74cf)) ? (rb_state7.var_10) : (0xafdd2e93));
        rb_state7.var_13 = (rb_state7.var_13) + (((0xd0b04b92) - (0x21597cb9)) + (rb_state7.var_11));
        rb_state7.var_14 = (rb_state7.var_14) ^ (((rb_state7.var_8) == (0xa99753e1)) ? (rb_state7.var_12) : (0x97d9bc19));
        rb_state7.var_15 = (rb_state7.var_15) + (rb_state7.var_13);
        rb_state7.var_16 = (rb_state7.var_16) + (((rb_state7.var_14) == (0x760c74cf)) ? (rb_state7.var_15) : (0x241f4160));
        rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_16);
        pthread_mutex_unlock(&(rb_state7.lock_17));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_20 = (rb_state11.var_20) + ((0xed04e7cc) - (rb_state11.var_21));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0xbb930024)) {
        rb_state13.var_2 = rb_state13.var_0;
    }
    if ((rb_state13.var_1) == (0xbb930024)) {
        if (!((rb_state13.var_0) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - ((0x111b51d7) + (0x582d606d));
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

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
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x4884644f));
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_19) : (0x5bbdada2));
        if ((rb_state8.var_22) == (0x1)) {
            pthread_mutex_lock(&(rb_state8.lock_58));
            rb_state8.var_34 = 0x8331dd7b;
            rb_state8.var_35 = 0x4c0b5586;
            rb_state8.var_36 = 0xe9b07a4;
            rb_state8.var_37 = (rb_state8.var_37) ^ (((rb_state8.var_28) == (0x0)) ? (rb_state8.var_36) : (0x621d66cf));
            rb_state8.var_38 = (rb_state8.var_38) + (((((0x81534a65) + (rb_state8.var_30)) - (rb_state8.var_29)) + (rb_state8.var_34)) + (rb_state8.var_35));
            rb_state8.var_39 = (rb_state8.var_39) ^ (((0xf2035575) - (0x34c9e8ce)) + (rb_state8.var_37));
            rb_state8.var_40 = (rb_state8.var_40) - (((rb_state8.var_31) == (0x0)) ? (rb_state8.var_38) : (0xe4cca14f));
            rb_state8.var_41 = (rb_state8.var_41) - (((0xe965ebeb) ^ (rb_state8.var_39)) ^ (0xfad03913));
            rb_state8.var_42 = (rb_state8.var_42) + (((((0x37cabc8f) - (0x830582d0)) + (rb_state8.var_32)) + (rb_state8.var_40)) + (rb_state8.var_41));
            rb_state8.var_33 = (rb_state8.var_33) - (rb_state8.var_42);
            rb_state8.var_23 = rb_state8.var_33;
            pthread_mutex_unlock(&(rb_state8.lock_58));
        }
        if ((rb_state8.var_22) == (0x1)) {
            pthread_mutex_lock(&(rb_state8.lock_58));
            rb_state8.var_44 = 0xc46727d7;
            rb_state8.var_45 = 0x393d1a48;
            rb_state8.var_46 = (rb_state8.var_46) ^ (((rb_state8.var_35) == (0x0)) ? (rb_state8.var_34) : (0x28b49e41));
            rb_state8.var_47 = (rb_state8.var_47) - (((rb_state8.var_36) == (0x0)) ? (rb_state8.var_33) : (0x2dae567e));
            rb_state8.var_48 = (rb_state8.var_48) - (((rb_state8.var_37) == (0x0)) ? (rb_state8.var_45) : (0x2dbb1db4));
            rb_state8.var_49 = (rb_state8.var_49) - (((rb_state8.var_38) == (0x0)) ? (rb_state8.var_44) : (0xbd91dc2b));
            rb_state8.var_50 = (rb_state8.var_50) + (((((0x57690645) - (rb_state8.var_47)) + (rb_state8.var_40)) - (rb_state8.var_39)) + (rb_state8.var_46));
            rb_state8.var_51 = (rb_state8.var_51) + (((rb_state8.var_41) == (0x0)) ? (rb_state8.var_48) : (0x2ad988b6));
            rb_state8.var_52 = (rb_state8.var_52) ^ (rb_state8.var_49);
            rb_state8.var_53 = (rb_state8.var_53) ^ (((rb_state8.var_42) == (0x0)) ? (rb_state8.var_50) : (0x95615dad));
            rb_state8.var_54 = (rb_state8.var_54) ^ (((rb_state8.var_43) == (0x0)) ? (rb_state8.var_51) : (0xe9f61864));
            rb_state8.var_55 = (rb_state8.var_55) + (((0x4fa162eb) + (rb_state8.var_44)) + (rb_state8.var_52));
            rb_state8.var_56 = (rb_state8.var_56) - (((((0xf988c9e6) - (rb_state8.var_53)) - (rb_state8.var_45)) ^ (0x5fcdc35e)) ^ (rb_state8.var_54));
            rb_state8.var_57 = (rb_state8.var_57) ^ (((rb_state8.var_55) == (0x0)) ? (rb_state8.var_56) : (0x909a73ed));
            rb_state8.var_43 = (rb_state8.var_43) + (rb_state8.var_57);
            rb_state8.var_23 = rb_state8.var_43;
            pthread_mutex_unlock(&(rb_state8.lock_58));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (120694 < rb_input_size ? (uint32_t)rb_input[120694] : 0xadf58acc);
        if ((rb_state15.var_1) == (0x250c7b72)) {
            rb_state15.var_15 = 0xfdbbdf80;
            rb_state15.var_16 = 0x4227e02;
            rb_state15.var_17 = 0x299a1fd1;
            rb_state15.var_18 = (rb_state15.var_18) ^ ((0x6f2e26e2) - (0x3f989bcb));
            rb_state15.var_19 = (rb_state15.var_19) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_16) : (0xae7bbac1));
            rb_state15.var_20 = (rb_state15.var_20) + (((0x4d41380f) - (rb_state15.var_15)) - (0x4e0d2aee));
            rb_state15.var_21 = (rb_state15.var_21) ^ (rb_state15.var_17);
            rb_state15.var_22 = (rb_state15.var_22) ^ (rb_state15.var_18);
            rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_19) : (0x4d973530));
            rb_state15.var_24 = (rb_state15.var_24) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_20) : (0x5ec9b7b));
            rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_21);
            rb_state15.var_26 = (rb_state15.var_26) + (rb_state15.var_22);
            rb_state15.var_27 = (rb_state15.var_27) - (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_23) : (0xf0390f5a));
            rb_state15.var_28 = (rb_state15.var_28) ^ (((0xe0f6e10a) ^ (0x88a8aa61)) - (rb_state15.var_24));
            rb_state15.var_29 = (rb_state15.var_29) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0x152dc44e));
            rb_state15.var_30 = (rb_state15.var_30) - (rb_state15.var_27);
            rb_state15.var_31 = (rb_state15.var_31) - (rb_state15.var_28);
            rb_state15.var_32 = (rb_state15.var_32) - (((0xf8389615) + (rb_state15.var_10)) + (rb_state15.var_29));
            rb_state15.var_33 = (rb_state15.var_33) ^ (((rb_state15.var_30) == (0x0)) ? (rb_state15.var_31) : (0x1335f100));
            rb_state15.var_34 = (rb_state15.var_34) + (((0xb4608052) + (rb_state15.var_32)) - (rb_state15.var_11));
            rb_state15.var_35 = (rb_state15.var_35) ^ (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_33) : (0xcbb30ebf));
            rb_state15.var_36 = (rb_state15.var_36) ^ (rb_state15.var_34);
            rb_state15.var_37 = (rb_state15.var_37) + (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_35) : (0x947f02a5));
            rb_state15.var_38 = (rb_state15.var_38) + (((0xe205fff5) - (rb_state15.var_36)) ^ (0x8580f88e));
            rb_state15.var_39 = (rb_state15.var_39) ^ (((rb_state15.var_37) == (0x0)) ? (rb_state15.var_38) : (0x863f5f11));
            rb_state15.var_14 = (rb_state15.var_14) ^ (rb_state15.var_39);
            rb_state15.var_2 = rb_state15.var_14;
        }
        if ((rb_state15.var_1) == (0x250c7b72)) {
            pthread_mutex_lock(&(rb_state15.lock_47));
            rb_state15.var_41 = 0xfd318b6c;
            rb_state15.var_42 = 0x25136dbf;
            rb_state15.var_43 = (rb_state15.var_43) - (((0x40bf61a0) + (0x97fd027c)) ^ (rb_state15.var_41));
            rb_state15.var_44 = (rb_state15.var_44) + (((((0x306adc82) + (rb_state15.var_42)) ^ (rb_state15.var_15)) + (rb_state15.var_14)) ^ (rb_state15.var_16));
            rb_state15.var_45 = (rb_state15.var_45) - (((0x18df1379) ^ (rb_state15.var_17)) ^ (rb_state15.var_43));
            rb_state15.var_46 = (rb_state15.var_46) + (((((0x46e563d5) ^ (rb_state15.var_18)) + (0x76707dbe)) - (rb_state15.var_44)) + (rb_state15.var_45));
            rb_state15.var_40 = (rb_state15.var_40) + (rb_state15.var_46);
            rb_state15.var_2 = rb_state15.var_40;
            pthread_mutex_unlock(&(rb_state15.lock_47));
        }
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_2) == (0xb60529a6)) {
                pthread_mutex_lock(&(rb_state8.lock_21));
                rb_state8.var_3 = rb_state8.var_0;
                pthread_mutex_unlock(&(rb_state8.lock_21));
            }
            if ((rb_state8.var_2) == (0xb60529a6)) {
                rb_state8.var_22 = !((rb_state8.var_0) == (rb_state8.var_3));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0xbb930024)) {
                rb_state13.var_3 = 0x80a8abf9;
                rb_state13.var_4 = 0x4c98e432;
                rb_state13.var_5 = (rb_state13.var_5) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_3) : (0x6a5a2290));
                rb_state13.var_6 = (rb_state13.var_6) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_3) : (0x7cb828f1));
                rb_state13.var_7 = (rb_state13.var_7) ^ (((0x2a9ec1a6) - (0x796c83b7)) - (rb_state13.var_2));
                rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_5) : (0xd20ce882));
                rb_state13.var_9 = (rb_state13.var_9) + (rb_state13.var_6);
                rb_state13.var_10 = (rb_state13.var_10) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0xf8e2065c));
                rb_state13.var_11 = (rb_state13.var_11) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_9) : (0xe500dda1));
                rb_state13.var_12 = (rb_state13.var_12) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_11) : (0xdc4f0d90));
                rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_12);
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) - (0xac8c8493);
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                #ifdef RACEBENCH_BUG_8
                rb_state8.var_14 = (rb_state8.var_14) + (rb_state8.var_15);
                rb_state8.var_16 = (rb_state8.var_16) ^ ((0xe7a37426) + (rb_state8.var_17));
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) - ((0x356c223a) ^ (0xe14f3639));
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) + ((0xc7f1a344) ^ (rb_state13.var_0));
                #endif
                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_22) == (0x1)) {
                    if ((rb_state8.var_23) != (0x0)) {
                        if (!((rb_state8.var_23) == (rb_state8.var_43))) {
                            racebench_trigger(8);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) + (0x22d7a3d7);
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                SIZE(result) = alloc_size;
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) ^ (0xf0d7e0c7);
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    if ((rb_state15.var_1) == (0x250c7b72)) {
                        if ((rb_state15.var_2) != (0x0)) {
                            if (!((rb_state15.var_2) == (rb_state15.var_40))) {
                                racebench_trigger(15);
                            }
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
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_0);
                #endif
                #ifdef RACEBENCH_BUG_17
                if ((rb_state17.var_1) == (0xb4666a96)) {
                    rb_state17.var_3 = 0x51035df1;
                    rb_state17.var_4 = (rb_state17.var_4) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_2) : (0x3dc09589));
                    rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_3);
                    rb_state17.var_6 = (rb_state17.var_6) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_4) : (0xc1b0a98d));
                    rb_state17.var_7 = (rb_state17.var_7) ^ (((0x4660b38) - (rb_state17.var_6)) ^ (rb_state17.var_5));
                    rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_6) : (0x41da25fe));
                    rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0xe8543622));
                    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_9);
                }
                #endif
                break;
            }

            prev = d;
            d = NEXTFREE(d);
        }

        { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
    }

    if (result) {
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_22) == (0xea54a57b)) {
            if ((rb_state11.var_29) != (0x0)) {
                if (!((rb_state11.var_29) == (rb_state11.var_60))) {
                    racebench_trigger(11);
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

    mem_pool[home].tally += SIZE(result);
    if (mem_pool[home].tally > mem_pool[home].maxm) {
        mem_pool[home].maxm = mem_pool[home].tally;
    }

    if (SIZE(result) < size) {
        printf("*** Bad size from malloc %d, %d\n", size, SIZE(result));
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xc3de74c5)) {
        pthread_mutex_lock(&(rb_state10.lock_3));
        rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_2);
        pthread_mutex_unlock(&(rb_state10.lock_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0x67b2bea6) - (0xe73a857c));
    rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
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