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
        if ((rb_state0.var_0) == (0x9f8bfdc1)) {
            rb_state0.var_11 = 0xda744700;
            rb_state0.var_12 = (rb_state0.var_12) ^ (((((0x25868f73) - (0x2448efe3)) + (rb_state0.var_7)) - (rb_state0.var_6)) - (0xbd3b5716));
            rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_12) : (0xf3bdaf69));
            rb_state0.var_10 = (rb_state0.var_10) ^ (rb_state0.var_13);
            rb_state0.var_3 = rb_state0.var_10;
        }
        if ((rb_state0.var_0) == (0x9f8bfdc1)) {
            pthread_mutex_lock(&(rb_state0.lock_17));
            rb_state0.var_15 = (rb_state0.var_15) ^ (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_10) : (0xed618949));
            rb_state0.var_16 = (rb_state0.var_16) + (((((0x46263c1) ^ (rb_state0.var_15)) + (rb_state0.var_11)) + (0x318f0990)) + (rb_state0.var_9));
            rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_16);
            rb_state0.var_3 = rb_state0.var_14;
            pthread_mutex_unlock(&(rb_state0.lock_17));
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_2);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (0x5ff7d9f);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_2);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) + (159731 < rb_input_size ? (uint32_t)rb_input[159731] : 0x6254068e);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0x128569b2) + (rb_state18.var_1));
        #endif
        bucket = 1;
        while (1 << bucket < size) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) ^ ((0xc8cad0fb) - (0x7055f976));
            if ((rb_state0.var_0) == (0x9f8bfdc1)) {
                if ((rb_state0.var_3) != (0x0)) {
                    if (!((rb_state0.var_3) == (rb_state0.var_14))) {
                        racebench_trigger(0);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) - (0xd48016b5);
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_1);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ (0xb5b2d2ed);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - ((0xb1073f50) + (0xae1128ee));
            rb_state3.var_2 = (rb_state3.var_2) ^ (0x67082a44);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) - ((0xe32f5dad) - (0xdde562bc));
            if ((rb_state5.var_0) == (0xe5e003f4)) {
                rb_state5.var_4 = 0x5369b8fc;
                rb_state5.var_5 = (rb_state5.var_5) - (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0x6c786234));
                rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_5);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x20)) ? (rb_state6.var_0) : (0xade50a6));
            rb_state6.var_3 = (rb_state6.var_3) + ((0xd824b60) + (rb_state6.var_3));
            rb_state6.var_3 = (rb_state6.var_3) ^ (60831 < rb_input_size ? (uint32_t)rb_input[60831] : 0xd7f6bf96);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) + (0xdfd6ea4e);
            rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_1);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) - ((0x64c672fd) ^ (0xf374ceba));
            rb_state9.var_1 = (rb_state9.var_1) - (0x48041637);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_0) == (0xc19468bc)) ? (rb_state10.var_3) : (0x3ea9c1f8));
            if ((rb_state10.var_1) == (0x62deb5b4)) {
                rb_state10.var_5 = 0xf99a95e7;
                rb_state10.var_6 = (rb_state10.var_6) + (rb_state10.var_4);
                rb_state10.var_7 = (rb_state10.var_7) + (0xac63dce2);
                rb_state10.var_8 = (rb_state10.var_8) ^ (125560 < rb_input_size ? (uint32_t)rb_input[125560] : 0x2ad71fb);
                rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0x7b11dcb9));
                rb_state10.var_10 = (rb_state10.var_10) ^ (((0x4a2cd7ab) - (rb_state10.var_7)) ^ (0xd72f68bb));
                rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_8) : (0xa23c956c));
                rb_state10.var_12 = (rb_state10.var_12) + (((0x9291258) ^ (rb_state10.var_9)) - (0x6c31ba28));
                rb_state10.var_13 = (rb_state10.var_13) + (((0x97386abb) ^ (0x1ad566f1)) ^ (rb_state10.var_10));
                rb_state10.var_14 = (rb_state10.var_14) ^ (((((0x2cbc752f) + (0x53ffda69)) + (0xaa349556)) - (rb_state10.var_11)) + (rb_state10.var_12));
                rb_state10.var_15 = (rb_state10.var_15) - (((((0x1130a28f) - (rb_state10.var_13)) ^ (0xea145f)) ^ (rb_state10.var_14)) ^ (0xbca85199));
                rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_15);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) - (156689 < rb_input_size ? (uint32_t)rb_input[156689] : 0x14a1a27d);
            rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0xffffffe0)) ? (rb_state11.var_1) : (0x245a2c07));
            rb_state11.var_1 = (rb_state11.var_1) ^ (0x16397cf6);
            if ((rb_state11.var_2) == (0x8308adc0)) {
                if ((rb_state11.var_3) != (0x0)) {
                    if (!((rb_state11.var_3) == (rb_state11.var_21))) {
                        racebench_trigger(11);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_1) == (0x54795dc1)) ? (rb_state12.var_2) : (0xabe87558));
            rb_state12.var_2 = (rb_state12.var_2) + (0xcf8a9b09);
            if ((rb_state12.var_2) == (0xa2db85b9)) {
                pthread_mutex_lock(&(rb_state12.lock_31));
                rb_state12.var_6 = 0xbae40693;
                rb_state12.var_7 = 0x36219154;
                rb_state12.var_8 = (rb_state12.var_8) + (rb_state12.var_5);
                rb_state12.var_9 = (rb_state12.var_9) - (((((0xe86ea4b7) ^ (rb_state12.var_4)) ^ (rb_state12.var_7)) + (0xd8f9e60e)) - (0x4838731d));
                rb_state12.var_10 = (rb_state12.var_10) + (((0xe822360) - (rb_state12.var_6)) + (0xff5b4286));
                rb_state12.var_11 = (rb_state12.var_11) + (((((0x41c10daf) ^ (rb_state12.var_8)) ^ (rb_state12.var_9)) - (rb_state12.var_6)) - (rb_state12.var_7));
                rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0xdd05e36f));
                rb_state12.var_5 = (rb_state12.var_5) - (rb_state12.var_12);
                rb_state12.var_4 = rb_state12.var_5;
                pthread_mutex_unlock(&(rb_state12.lock_31));
            }
            if ((rb_state12.var_2) == (0xa2db85b9)) {
                if (!((rb_state12.var_4) == (rb_state12.var_5))) {
                    racebench_trigger(12);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) + ((0xd579392d) + (rb_state13.var_0));
            rb_state13.var_2 = (rb_state13.var_2) - (6912 < rb_input_size ? (uint32_t)rb_input[6912] : 0x63a5646b);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_2);
            rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_1) == (0xe5fbb2b)) ? (rb_state17.var_3) : (0xcfa6a0b9));
            if ((rb_state17.var_2) == (0xdec6c6e9)) {
                if ((rb_state17.var_4) != (0x0)) {
                    if (!((rb_state17.var_4) == (rb_state17.var_18))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            bucket++;
        }
    }

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ ((0x81653144) ^ (rb_state2.var_2));
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x17aa2d7d)) ? (rb_state9.var_0) : (0x1d931457));
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
    rb_state0.var_2 = (rb_state0.var_2) ^ (((rb_state0.var_2) == (0x29133589)) ? (rb_state0.var_1) : (0x94648db7));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (139298 < rb_input_size ? (uint32_t)rb_input[139298] : 0x6dc3c089);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_1) == (0xaef7d731)) ? (rb_state8.var_2) : (0xcff46b9e));
    rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_1) == (0xaef7d731)) ? (rb_state8.var_0) : (0x50c805ce));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x5952b98c)) {
        rb_state19.var_5 = 0xfab87f4c;
        rb_state19.var_6 = (rb_state19.var_6) + (((0xeaf86d37) - (rb_state19.var_5)) - (0x953312a6));
        rb_state19.var_7 = (rb_state19.var_7) ^ ((0x218c3796) - (0xe729cafb));
        rb_state19.var_8 = (rb_state19.var_8) - (20471 < rb_input_size ? (uint32_t)rb_input[20471] : 0x8bff21b0);
        rb_state19.var_9 = (rb_state19.var_9) + (((0x899dab6c) + (0x56ecd07b)) - (rb_state19.var_4));
        rb_state19.var_10 = (rb_state19.var_10) - (((0xe62168d7) ^ (rb_state19.var_6)) ^ (rb_state19.var_6));
        rb_state19.var_11 = (rb_state19.var_11) ^ (((((0xdbc7a79) ^ (rb_state19.var_8)) - (rb_state19.var_7)) - (rb_state19.var_8)) + (rb_state19.var_7));
        rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_9) == (0x388ad30b)) ? (rb_state19.var_9) : (0xdf4488ff));
        rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_10) == (0x19de9729)) ? (rb_state19.var_11) : (0x2372f73c));
        rb_state19.var_14 = (rb_state19.var_14) + (((((0xc46b0d6) + (rb_state19.var_13)) + (0x82e02c1d)) ^ (rb_state19.var_12)) + (0x9e90eef3));
        rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_14);
    }
    #endif
    bucket = FindBucket(size);

    if (bucket < MAXFAST) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_0) == (0xe5e003f4)) ? (rb_state5.var_2) : (0xa7a57154));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (0xf4f34512);
        #endif
        alloc_size = 1 << bucket;
    } else {
        alloc_size = ((size + ALIGN - 1) / ALIGN) * ALIGN;
    }

    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0x77801ec4)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_23))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_2);
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
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_2) == (0x77801ec4)) {
            rb_state8.var_7 = 0x369bf8dd;
            rb_state8.var_8 = 0xcef22cbb;
            rb_state8.var_9 = 0xc741c01;
            rb_state8.var_10 = (rb_state8.var_10) + (0x4b9de2b);
            rb_state8.var_11 = (rb_state8.var_11) - (((((0xc9f8cb34) - (rb_state8.var_9)) - (rb_state8.var_7)) - (0x3a6ad5c)) - (0xea101619));
            rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_8) : (0x33c90239));
            rb_state8.var_13 = (rb_state8.var_13) - (rb_state8.var_10);
            rb_state8.var_14 = (rb_state8.var_14) - (rb_state8.var_11);
            rb_state8.var_15 = (rb_state8.var_15) ^ (((((0xb5290dd5) - (rb_state8.var_12)) - (rb_state8.var_13)) ^ (0xb6460006)) - (0x717fce2c));
            rb_state8.var_16 = (rb_state8.var_16) - (rb_state8.var_14);
            rb_state8.var_17 = (rb_state8.var_17) + (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_15) : (0x9ae9719));
            rb_state8.var_18 = (rb_state8.var_18) - (rb_state8.var_16);
            rb_state8.var_19 = (rb_state8.var_19) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_17) : (0x4aa52d9));
            rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_18);
            rb_state8.var_21 = (rb_state8.var_21) - (((0x9943d83d) ^ (rb_state8.var_8)) ^ (rb_state8.var_19));
            rb_state8.var_22 = (rb_state8.var_22) + (((((0x5216be30) - (rb_state8.var_9)) ^ (rb_state8.var_10)) ^ (rb_state8.var_20)) ^ (rb_state8.var_21));
            rb_state8.var_6 = (rb_state8.var_6) ^ (rb_state8.var_22);
            rb_state8.var_3 = rb_state8.var_6;
        }
        if ((rb_state8.var_2) == (0x77801ec4)) {
            pthread_mutex_lock(&(rb_state8.lock_31));
            rb_state8.var_24 = 0xe3e3e1f0;
            rb_state8.var_25 = 0xf4495aa2;
            rb_state8.var_26 = (rb_state8.var_26) ^ (((0xff702460) ^ (rb_state8.var_24)) + (rb_state8.var_12));
            rb_state8.var_27 = (rb_state8.var_27) ^ (((0xea09e2d) - (rb_state8.var_11)) - (0x38df4100));
            rb_state8.var_28 = (rb_state8.var_28) + (((((0x19bb2a5b) + (rb_state8.var_13)) - (rb_state8.var_26)) - (rb_state8.var_25)) ^ (0x43fec4af));
            rb_state8.var_29 = (rb_state8.var_29) ^ (rb_state8.var_27);
            rb_state8.var_30 = (rb_state8.var_30) - (((((0x9f93aa01) + (rb_state8.var_29)) ^ (rb_state8.var_14)) - (0x77b0afc6)) + (rb_state8.var_28));
            rb_state8.var_23 = (rb_state8.var_23) - (rb_state8.var_30);
            rb_state8.var_3 = rb_state8.var_23;
            pthread_mutex_unlock(&(rb_state8.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_0);
        #endif
        prev = NULL;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_1);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) - (0x88d9620d);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (((rb_state6.var_0) == (0x839af94e)) ? (rb_state6.var_3) : (0xb9230b7b));
        if ((rb_state6.var_1) == (0x0)) {
            rb_state6.var_4 = rb_state6.var_2;
        }
        if ((rb_state6.var_1) == (0x0)) {
            if (!((rb_state6.var_2) == (rb_state6.var_4))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_0) : (0xe71f5d75));
        #endif
        d = mem_pool[home].freeBlock[MAXFAST];
        while (d) {

            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
            rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_1);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_2) == (0x8308adc0)) {
                rb_state11.var_14 = 0x32298677;
                rb_state11.var_15 = 0x838a170;
                rb_state11.var_16 = 0xe11c1cf6;
                rb_state11.var_17 = (rb_state11.var_17) + (161363 < rb_input_size ? (uint32_t)rb_input[161363] : 0x402bd169);
                rb_state11.var_18 = (rb_state11.var_18) + (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_15) : (0x2abd8b5e));
                rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_14) == (0x0)) ? (rb_state11.var_17) : (0xf94e5310));
                rb_state11.var_20 = (rb_state11.var_20) - (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x7bb2ee96));
                rb_state11.var_13 = (rb_state11.var_13) + (rb_state11.var_20);
                rb_state11.var_3 = rb_state11.var_13;
            }
            if ((rb_state11.var_2) == (0x8308adc0)) {
                pthread_mutex_lock(&(rb_state11.lock_39));
                rb_state11.var_22 = 0x6bb964cd;
                rb_state11.var_23 = 0xe12853aa;
                rb_state11.var_24 = (rb_state11.var_24) ^ (172311 < rb_input_size ? (uint32_t)rb_input[172311] : 0xbc46c5a6);
                rb_state11.var_25 = (rb_state11.var_25) - (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_23) : (0xbdbe10e5));
                rb_state11.var_26 = (rb_state11.var_26) - (((0xb174b4f1) - (rb_state11.var_10)) + (rb_state11.var_9));
                rb_state11.var_27 = (rb_state11.var_27) + (((0x31c879ea) - (rb_state11.var_24)) - (0x784f0cb3));
                rb_state11.var_28 = (rb_state11.var_28) ^ (((0xbbf0ee0) + (rb_state11.var_11)) + (rb_state11.var_25));
                rb_state11.var_29 = (rb_state11.var_29) + (((0x684832e9) ^ (rb_state11.var_12)) + (rb_state11.var_26));
                rb_state11.var_30 = (rb_state11.var_30) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_27) : (0xf918e628));
                rb_state11.var_31 = (rb_state11.var_31) ^ (((((0x425b8574) - (rb_state11.var_14)) + (rb_state11.var_15)) ^ (rb_state11.var_29)) ^ (rb_state11.var_28));
                rb_state11.var_32 = (rb_state11.var_32) ^ (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_30) : (0xdceed3d9));
                rb_state11.var_33 = (rb_state11.var_33) - (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_31) : (0xc01119a0));
                rb_state11.var_34 = (rb_state11.var_34) ^ (rb_state11.var_32);
                rb_state11.var_35 = (rb_state11.var_35) - (rb_state11.var_33);
                rb_state11.var_36 = (rb_state11.var_36) - (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_34) : (0xa53ded03));
                rb_state11.var_37 = (rb_state11.var_37) - (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_35) : (0x7b9731dd));
                rb_state11.var_38 = (rb_state11.var_38) - (((rb_state11.var_36) == (0x0)) ? (rb_state11.var_37) : (0x20630f29));
                rb_state11.var_21 = (rb_state11.var_21) ^ (rb_state11.var_38);
                rb_state11.var_3 = rb_state11.var_21;
                pthread_mutex_unlock(&(rb_state11.lock_39));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_1);
            #endif
            block_size = SIZE(d);

            if (block_size >= alloc_size) {

                leftover = block_size - alloc_size - 2 * sizeof(unsigned);
                #ifdef RACEBENCH_BUG_1
                if ((rb_state1.var_1) == (0x9002d6a)) {
                    pthread_mutex_lock(&(rb_state1.lock_21));
                    rb_state1.var_4 = 0x5ca65ac5;
                    rb_state1.var_5 = 0x4dbe2a65;
                    rb_state1.var_6 = (rb_state1.var_6) ^ (((0x3a894198) - (0x7a9d22b1)) - (rb_state1.var_4));
                    rb_state1.var_7 = (rb_state1.var_7) - (rb_state1.var_2);
                    rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_5) : (0x3da8d4b2));
                    rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_6) : (0x96b51203));
                    rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_7) : (0x59c7c7ef));
                    rb_state1.var_11 = (rb_state1.var_11) + (((0xee27ade6) - (rb_state1.var_6)) - (rb_state1.var_8));
                    rb_state1.var_12 = (rb_state1.var_12) + (((((0xfa38fc55) ^ (rb_state1.var_8)) ^ (rb_state1.var_7)) + (rb_state1.var_10)) + (rb_state1.var_9));
                    rb_state1.var_13 = (rb_state1.var_13) - (((0xa2c8dceb) - (rb_state1.var_9)) ^ (rb_state1.var_11));
                    rb_state1.var_14 = (rb_state1.var_14) + (((0x597f11f2) ^ (rb_state1.var_10)) - (rb_state1.var_12));
                    rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0xe73dd733));
                    rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_15);
                    rb_state1.var_2 = rb_state1.var_3;
                    pthread_mutex_unlock(&(rb_state1.lock_21));
                }
                if ((rb_state1.var_1) == (0x9002d6a)) {
                    pthread_mutex_lock(&(rb_state1.lock_21));
                    if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                        racebench_trigger(1);
                    }
                    pthread_mutex_unlock(&(rb_state1.lock_21));
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_1) == (0x81653173)) ? (rb_state2.var_0) : (0xc33c432e));
                if ((rb_state2.var_2) == (0x0)) {
                    rb_state2.var_3 = rb_state2.var_0;
                }
                if ((rb_state2.var_2) == (0x0)) {
                    if (!((rb_state2.var_0) == (rb_state2.var_3))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_1);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_3 = (rb_state12.var_3) - ((0x90dac6e6) + (rb_state12.var_3));
                #endif
                result = d + (leftover / sizeof(unsigned)) + 2;
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_0 = (rb_state1.var_0) ^ (0x2e7a9d78);
                if ((rb_state1.var_1) == (0x9002d6a)) {
                    rb_state1.var_16 = (rb_state1.var_16) - (0x93edf09);
                    rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_11) : (0xc052c41d));
                    rb_state1.var_18 = (rb_state1.var_18) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_16) : (0xef57821a));
                    rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_17);
                    rb_state1.var_20 = (rb_state1.var_20) + (((((0xca38fcbe) ^ (rb_state1.var_18)) ^ (0xf6922c90)) - (rb_state1.var_19)) + (rb_state1.var_14));
                    rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_20);
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_2) == (0x0)) {
                    rb_state2.var_4 = 0xc7c7a463;
                    rb_state2.var_5 = 0x1f3f7ed;
                    rb_state2.var_6 = (rb_state2.var_6) + (((((0x884df49d) ^ (rb_state2.var_3)) ^ (rb_state2.var_4)) ^ (0xb60efea1)) + (rb_state2.var_4));
                    rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_6) == (0xaaa68491)) ? (rb_state2.var_5) : (0x78c19068));
                    rb_state2.var_8 = (rb_state2.var_8) ^ (((rb_state2.var_7) == (0x1f3f7ed)) ? (rb_state2.var_5) : (0x80e1803e));
                    rb_state2.var_9 = (rb_state2.var_9) ^ (((rb_state2.var_8) == (0x1f3f7ed)) ? (rb_state2.var_6) : (0xbd3587ec));
                    rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_9) == (0xaaa68491)) ? (rb_state2.var_7) : (0x67833050));
                    rb_state2.var_11 = (rb_state2.var_11) - (((((0x7988a0c5) - (rb_state2.var_11)) + (rb_state2.var_10)) + (rb_state2.var_8)) ^ (rb_state2.var_9));
                    rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_10) : (0x87437247));
                    rb_state2.var_13 = (rb_state2.var_13) ^ (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_11) : (0xba4dfc91));
                    rb_state2.var_14 = (rb_state2.var_14) - (((((0x17e71ea4) + (rb_state2.var_12)) - (rb_state2.var_3)) - (rb_state2.var_14)) ^ (rb_state2.var_13));
                    rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_14);
                }
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_2 = (rb_state3.var_2) - (0xa3376678);
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0xe5e003f4)) ? (rb_state5.var_2) : (0xbdc67700));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0xdba5cf87)) ? (rb_state11.var_1) : (0xa36531c5));
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_1 = (rb_state12.var_1) ^ (0x304f7a68);
                #endif
                SIZE(result) = alloc_size;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_1) == (0xa0915748)) ? (rb_state3.var_0) : (0x8af5ac24));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) + (0x9b11456b);
                #endif
                HOME(result) = home;

                if (leftover > MAXFASTBL) {
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_3 = (rb_state3.var_3) ^ (0x1b83df57);
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
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_0 = (rb_state1.var_0) ^ (171832 < rb_input_size ? (uint32_t)rb_input[171832] : 0xcea59722);
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (109571 < rb_input_size ? (uint32_t)rb_input[109571] : 0x54a60d99);
                #endif
                break;
            }

            prev = d;
            d = NEXTFREE(d);
        }

        { pthread_mutex_unlock(&(mem_pool[home].memoryLock)); }
    }

    if (result) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state6.lock_5));
            rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_4);
            pthread_mutex_unlock(&(rb_state6.lock_5));
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

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (0x2f2770b0);
    if ((rb_state5.var_0) == (0xe5e003f4)) {
        rb_state5.var_3 = rb_state5.var_1;
    }
    if ((rb_state5.var_0) == (0xe5e003f4)) {
        if (!((rb_state5.var_1) == (rb_state5.var_3))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0xa2db85b9)) {
        rb_state12.var_13 = 0x57911812;
        rb_state12.var_14 = 0x44ef0983;
        rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_9) : (0xc52ca929));
        rb_state12.var_16 = (rb_state12.var_16) + (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_14) : (0x9d053ef2));
        rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_13);
        rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_8) : (0xacd21808));
        rb_state12.var_19 = (rb_state12.var_19) - (rb_state12.var_15);
        rb_state12.var_20 = (rb_state12.var_20) ^ (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_16) : (0xb9f9ad2c));
        rb_state12.var_21 = (rb_state12.var_21) ^ (((0x2168116b) ^ (rb_state12.var_14)) - (rb_state12.var_17));
        rb_state12.var_22 = (rb_state12.var_22) + (rb_state12.var_18);
        rb_state12.var_23 = (rb_state12.var_23) ^ (rb_state12.var_19);
        rb_state12.var_24 = (rb_state12.var_24) - (((0x33739128) ^ (rb_state12.var_20)) - (rb_state12.var_15));
        rb_state12.var_25 = (rb_state12.var_25) ^ (((0x4a209674) ^ (rb_state12.var_21)) ^ (rb_state12.var_16));
        rb_state12.var_26 = (rb_state12.var_26) ^ (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_23) : (0x9df6d9a));
        rb_state12.var_27 = (rb_state12.var_27) - (((rb_state12.var_24) == (0x0)) ? (rb_state12.var_25) : (0x395e6803));
        rb_state12.var_28 = (rb_state12.var_28) ^ (((0x5a6165a9) + (0x73a80b2a)) + (rb_state12.var_26));
        rb_state12.var_29 = (rb_state12.var_29) ^ (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_27) : (0xb46b1efa));
        rb_state12.var_30 = (rb_state12.var_30) + (((rb_state12.var_28) == (0x0)) ? (rb_state12.var_29) : (0xb35f6ca0));
        rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_30);
    }
    #endif
    mem_pool[home].tally += SIZE(result);
    if (mem_pool[home].tally > mem_pool[home].maxm) {
        mem_pool[home].maxm = mem_pool[home].tally;
    }

    if (SIZE(result) < size) {
        printf("*** Bad size from malloc %d, %d\n", size, SIZE(result));
    }

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ ((0xb9200839) ^ (0xc429e11a));
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