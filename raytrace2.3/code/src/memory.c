#include "racebench_bugs.h"


#include <stdio.h>
#include <math.h>
#include "rt.h"

#define CKSM 0x55AA55AA
#define PAGESIZE (4 * 1024)
#define THRESHOLD (sizeof(NODE) + 16)
#define ROUND_UP(x) ((((x) + 7) >> 3) << 3)
#define ROUND_DN(x) (x & 0xFFFFFFF8)
#define NODE_ADD(x, y) (NODE huge *)((U8 huge *)(x) + (y))

UINT nodesize;
NODE huge *begmem;
NODE huge *endmem;

INT mem_grid;
INT maxmem_grid;

INT mem_voxel;
INT maxmem_voxel;

INT mem_hashtable;
INT maxmem_hashtable;

INT mem_emptycells;
INT maxmem_emptycells;

INT mem_bintree;
INT maxmem_bintree;

INT mem_pepArray;
INT maxmem_pepArray;

VOID *LocalMalloc(n, msg)
UINT n;
CHAR *msg;
{
    VOID *p;

    p = (VOID *)malloc(n);
    if (!p) {
        printf("%s: %s cannot allocate local memory.\n", ProgName, msg);
        exit(-1);
    }

    return (p);
}

VOID LocalFree(p)
VOID *p;
{ free(p); }

VOID GlobalHeapWalk() {
    NODE huge *curr;

    { pthread_mutex_lock(&(gm->memlock)); }
    curr = begmem;

    printf("freelist ->\t0x%08lX\n\n", (U32)gm->freelist);

    printf("node addr \tnode->next\tnode->size\tnode->free\tnode->cksm\n");
    printf("==========\t==========\t==========\t==========\t==========\n");

    while (curr < endmem) {
        printf("0x%08lX\t0x%08lX\t%10ld\t%s\t\t0x%08lX\n", (U32)curr, (U32)curr->next, curr->size, (curr->free ? "FREE" : "    "), curr->cksm);

        if (curr->cksm != CKSM) {
            fprintf(stderr, "GlobalHeapWalk: Invalid checksum in node.\n");
            exit(1);
        }

        curr = NODE_ADD(curr, curr->size + nodesize);
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
}

BOOL GlobalHeapInit(size)
UINT size;
{
    INT i;
    U8 *ptr;

    size = ROUND_UP(size);
    gm->freelist = (NODE huge *)malloc(size);
    ;

    if (!gm->freelist) {
        return (FALSE);
    }

    nodesize = sizeof(NODE);
    begmem = gm->freelist;
    endmem = NODE_ADD(gm->freelist, size);

    gm->freelist->size = size - nodesize;
    gm->freelist->next = NULL;
    gm->freelist->free = TRUE;
    gm->freelist->cksm = CKSM;

    return (TRUE);
}

VOID *GlobalMalloc(size, msg)
UINT size;
CHAR *msg;
{
    NODE huge *prev;
    NODE huge *curr;
    NODE huge *next;

    if (!size) {
        return (NULL);
    }

    { pthread_mutex_lock(&(gm->memlock)); }

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_13) == (0x31c3a20e)) {
        pthread_mutex_lock(&(rb_state1.lock_24));
        rb_state1.var_18 = 0x433e0c2;
        rb_state1.var_19 = 0xf246983a;
        rb_state1.var_20 = (rb_state1.var_20) - (((0x82d9cee0) ^ (0x9863ddd6)) ^ (rb_state1.var_19));
        rb_state1.var_21 = (rb_state1.var_21) + (((((0xf6bc05be) ^ (rb_state1.var_15)) + (rb_state1.var_16)) ^ (rb_state1.var_17)) + (rb_state1.var_18));
        rb_state1.var_22 = (rb_state1.var_22) + (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_20) : (0xd409ae0e));
        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_21) == (0xac7ed9a7)) ? (rb_state1.var_22) : (0x9b5b858b));
        rb_state1.var_16 = (rb_state1.var_16) ^ (rb_state1.var_23);
        rb_state1.var_17 = rb_state1.var_16;
        pthread_mutex_unlock(&(rb_state1.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_24) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_48));
        rb_state16.var_49 = !((rb_state16.var_41) == (rb_state16.var_40));
        pthread_mutex_unlock(&(rb_state16.lock_48));
    }
    #endif
    prev = NULL;
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xe36e4c6f)) {
        rb_state9.var_4 = 0x11b95651;
        rb_state9.var_5 = 0x59fdc56e;
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0xaf9c42f9) - (0xa8a22daa)) - (rb_state9.var_3)) + (0xc1731327)) ^ (rb_state9.var_5));
        rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_4) : (0xcc60cc8c));
        rb_state9.var_8 = (rb_state9.var_8) ^ (rb_state9.var_6);
        rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0x7e6e4161));
        rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_9);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x76594979)) {
        rb_state11.var_3 = rb_state11.var_0;
    }
    if ((rb_state11.var_2) == (0x76594979)) {
        pthread_mutex_lock(&(rb_state11.lock_4));
        if (!((rb_state11.var_0) == (rb_state11.var_3))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_29 = (rb_state16.var_29) + ((0xc2cba183) + (0xf2fed93b));
    if ((rb_state16.var_27) == (0xba05ad54)) {
        rb_state16.var_72 = (rb_state16.var_72) + (((rb_state16.var_40) == (0x0)) ? (rb_state16.var_41) : (0x91a6eec2));
        rb_state16.var_73 = (rb_state16.var_73) + (((((0x8466ca2e) + (0xb670aa09)) - (rb_state16.var_72)) ^ (rb_state16.var_42)) ^ (0x9a177b81));
        rb_state16.var_71 = (rb_state16.var_71) - (rb_state16.var_73);
        rb_state16.var_50 = rb_state16.var_71;
    }
    if ((rb_state16.var_27) == (0xba05ad54)) {
        pthread_mutex_lock(&(rb_state16.lock_79));
        rb_state16.var_75 = 0xd81b5476;
        rb_state16.var_76 = 0x38a2e404;
        rb_state16.var_77 = (rb_state16.var_77) ^ (((rb_state16.var_43) == (0x0)) ? (rb_state16.var_76) : (0xca065bb2));
        rb_state16.var_78 = (rb_state16.var_78) - (((((0x32351ec8) ^ (0xa63f40b0)) ^ (rb_state16.var_77)) ^ (rb_state16.var_75)) ^ (rb_state16.var_44));
        rb_state16.var_74 = (rb_state16.var_74) ^ (rb_state16.var_78);
        rb_state16.var_50 = rb_state16.var_74;
        pthread_mutex_unlock(&(rb_state16.lock_79));
    }
    #endif
    curr = gm->freelist;
    size = ROUND_UP(size);

    while (curr && curr->size < size) {
        if (curr->cksm != CKSM) {
            fprintf(stderr, "GlobalMalloc: Invalid checksum in node.\n");
            exit(1);
        }

        if (curr->free != TRUE) {
            fprintf(stderr, "GlobalMalloc: Node in free list not marked as free.\n");
            exit(1);
        }

        prev = curr;
        curr = curr->next;
    }

    if (!curr) {
        fprintf(stderr, "%s: %s cannot allocate global memory.\n", ProgName, msg);
        exit(-1);
    }

    if (curr->size - size > THRESHOLD) {
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_14) == (0xcf5273f1)) {
            rb_state8.var_39 = 0x64045c8c;
            rb_state8.var_40 = 0x3a76542d;
            rb_state8.var_41 = (rb_state8.var_41) ^ ((0x553f9f88) + (0x80272c16));
            rb_state8.var_42 = (rb_state8.var_42) + (((rb_state8.var_39) == (0x0)) ? (rb_state8.var_40) : (0x9394416e));
            rb_state8.var_43 = (rb_state8.var_43) ^ (((0x232722b6) - (rb_state8.var_28)) ^ (rb_state8.var_29));
            rb_state8.var_44 = (rb_state8.var_44) ^ (rb_state8.var_41);
            rb_state8.var_45 = (rb_state8.var_45) ^ (((0xa0a8ac87) ^ (rb_state8.var_42)) - (rb_state8.var_30));
            rb_state8.var_46 = (rb_state8.var_46) + (((rb_state8.var_43) == (0x0)) ? (rb_state8.var_44) : (0x4e1d8cc2));
            rb_state8.var_47 = (rb_state8.var_47) ^ (((rb_state8.var_45) == (0x0)) ? (rb_state8.var_46) : (0x568cc0ef));
            rb_state8.var_38 = (rb_state8.var_38) + (rb_state8.var_47);
            rb_state8.var_23 = rb_state8.var_38;
        }
        if ((rb_state8.var_14) == (0xcf5273f1)) {
            pthread_mutex_lock(&(rb_state8.lock_56));
            rb_state8.var_49 = 0x7ab39087;
            rb_state8.var_50 = (rb_state8.var_50) + (rb_state8.var_49);
            rb_state8.var_51 = (rb_state8.var_51) + (rb_state8.var_31);
            rb_state8.var_52 = (rb_state8.var_52) ^ (((((0x745b9ae9) ^ (rb_state8.var_32)) - (rb_state8.var_50)) ^ (0x453b81d4)) + (0xa1d62cfe));
            rb_state8.var_53 = (rb_state8.var_53) ^ (((rb_state8.var_33) == (0x0)) ? (rb_state8.var_51) : (0xf0c92d4e));
            rb_state8.var_54 = (rb_state8.var_54) + (rb_state8.var_52);
            rb_state8.var_55 = (rb_state8.var_55) + (((rb_state8.var_53) == (0x0)) ? (rb_state8.var_54) : (0x894dec56));
            rb_state8.var_48 = (rb_state8.var_48) - (rb_state8.var_55);
            rb_state8.var_23 = rb_state8.var_48;
            pthread_mutex_unlock(&(rb_state8.lock_56));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_1) == (0xe161f33e)) {
            pthread_mutex_lock(&(rb_state13.lock_16));
            rb_state13.var_5 = 0xd058a3a6;
            rb_state13.var_6 = 0xd125b6f9;
            rb_state13.var_7 = (rb_state13.var_7) + (0x74d352a5);
            rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_3);
            rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_6) : (0xc32b9e05));
            rb_state13.var_10 = (rb_state13.var_10) + (((((0xbb733300) - (rb_state13.var_8)) ^ (rb_state13.var_5)) - (rb_state13.var_7)) - (rb_state13.var_4));
            rb_state13.var_11 = (rb_state13.var_11) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_10) : (0xa33f65cd));
            rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_11);
            rb_state13.var_3 = rb_state13.var_4;
            pthread_mutex_unlock(&(rb_state13.lock_16));
        }
        if ((rb_state13.var_1) == (0xe161f33e)) {
            if (!((rb_state13.var_3) == (rb_state13.var_4))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x7cf9cd14)) {
            pthread_mutex_lock(&(rb_state17.lock_26));
            rb_state17.var_4 = 0x760bcbb3;
            rb_state17.var_5 = 0x247e8d94;
            rb_state17.var_6 = (rb_state17.var_6) ^ (((((0x5dffb396) - (rb_state17.var_3)) - (rb_state17.var_2)) - (0xdaacc385)) + (rb_state17.var_5));
            rb_state17.var_7 = (rb_state17.var_7) - (0xa17e7c02);
            rb_state17.var_8 = (rb_state17.var_8) + (((((0x919bfc4a) + (0xe6ff443)) ^ (rb_state17.var_6)) - (rb_state17.var_4)) ^ (rb_state17.var_4));
            rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_7);
            rb_state17.var_10 = (rb_state17.var_10) + (rb_state17.var_8);
            rb_state17.var_11 = (rb_state17.var_11) ^ (((((0x2ff67b14) ^ (rb_state17.var_10)) ^ (0x3f13e34)) - (rb_state17.var_9)) ^ (rb_state17.var_5));
            rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_11);
            rb_state17.var_2 = rb_state17.var_3;
            pthread_mutex_unlock(&(rb_state17.lock_26));
        }
        if ((rb_state17.var_0) == (0x7cf9cd14)) {
            if (!((rb_state17.var_2) == (rb_state17.var_3))) {
                racebench_trigger(17);
            }
        }
        #endif
        next = NODE_ADD(curr, nodesize + size);
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_13 = (rb_state8.var_13) + (0x69083648);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0xbe98d21f)) {
            if ((rb_state16.var_3) != (0x0)) {
                rb_state16.var_39 = !((rb_state16.var_3) == (rb_state16.var_32));
            }
        }
        #endif
        next->size = curr->size - nodesize - size;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0xc35cf62c)) {
            pthread_mutex_lock(&(rb_state1.lock_14));
            rb_state1.var_5 = 0xa7381204;
            rb_state1.var_6 = 0xb2779b60;
            rb_state1.var_7 = (rb_state1.var_7) + (0xac2f0c99);
            rb_state1.var_8 = (rb_state1.var_8) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_5) : (0x8df3129f));
            rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_6) == (0xb2779b60)) ? (rb_state1.var_3) : (0xb3dc2f38));
            rb_state1.var_10 = (rb_state1.var_10) - (((0x7c7ce9e7) + (rb_state1.var_5)) ^ (rb_state1.var_7));
            rb_state1.var_11 = (rb_state1.var_11) ^ (((rb_state1.var_6) == (0xb2779b60)) ? (rb_state1.var_8) : (0xd313ebc1));
            rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x96012d8b));
            rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_11) == (0xa7381204)) ? (rb_state1.var_12) : (0xdd03f776));
            rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_13);
            rb_state1.var_4 = rb_state1.var_3;
            pthread_mutex_unlock(&(rb_state1.lock_14));
        }
        rb_state1.var_20 = (rb_state1.var_20) ^ (rb_state1.var_19);
        #endif
        next->next = curr->next;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_22) == (0xd409ae0e)) {
            rb_state1.var_27 = 0x48d6f368;
            rb_state1.var_28 = 0x87c0658b;
            rb_state1.var_29 = 0xcfa128fb;
            rb_state1.var_30 = (rb_state1.var_30) - (0x3f9e1550);
            rb_state1.var_31 = (rb_state1.var_31) - (rb_state1.var_27);
            rb_state1.var_32 = (rb_state1.var_32) - (((rb_state1.var_25) == (0x0)) ? (rb_state1.var_28) : (0x50a9734f));
            rb_state1.var_33 = (rb_state1.var_33) ^ (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_30) : (0x8910598c));
            rb_state1.var_34 = (rb_state1.var_34) ^ (((0x910a79d6) - (rb_state1.var_26)) ^ (rb_state1.var_31));
            rb_state1.var_35 = (rb_state1.var_35) ^ (((((0x2853aa96) - (rb_state1.var_27)) - (rb_state1.var_32)) + (0xa1b77a19)) ^ (rb_state1.var_33));
            rb_state1.var_36 = (rb_state1.var_36) + (rb_state1.var_34);
            rb_state1.var_37 = (rb_state1.var_37) ^ (rb_state1.var_35);
            rb_state1.var_38 = (rb_state1.var_38) + (((rb_state1.var_28) == (0x0)) ? (rb_state1.var_36) : (0x70831ba));
            rb_state1.var_39 = (rb_state1.var_39) - (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_37) : (0xa2e730d));
            rb_state1.var_40 = (rb_state1.var_40) ^ (((0x485e596a) - (rb_state1.var_38)) ^ (0xc7f201f1));
            rb_state1.var_41 = (rb_state1.var_41) - (((0xa9dfbe99) ^ (rb_state1.var_30)) - (rb_state1.var_39));
            rb_state1.var_42 = (rb_state1.var_42) ^ (rb_state1.var_40);
            rb_state1.var_43 = (rb_state1.var_43) + (((0xe81c3130) + (rb_state1.var_41)) + (rb_state1.var_31));
            rb_state1.var_44 = (rb_state1.var_44) + (rb_state1.var_42);
            rb_state1.var_45 = (rb_state1.var_45) - (((rb_state1.var_32) == (0x0)) ? (rb_state1.var_43) : (0xec21a3e0));
            rb_state1.var_46 = (rb_state1.var_46) ^ (((((0x41bf27f5) ^ (0xb60a9174)) + (0xc8dfd51e)) ^ (rb_state1.var_44)) + (rb_state1.var_45));
            rb_state1.var_23 = (rb_state1.var_23) ^ (rb_state1.var_46);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_16 = (rb_state2.var_16) + (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_17) : (0x44bf9f63));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_1);
        #endif
        next->free = TRUE;
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + (31 < rb_input_size ? (uint32_t)rb_input[31] : 0xff683ff6);
        rb_state8.var_1 = (rb_state8.var_1) + (141 < rb_input_size ? (uint32_t)rb_input[141] : 0x5f7e083b);
        rb_state8.var_11 = (rb_state8.var_11) - (0x9a8bc229);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (((rb_state16.var_1) == (0xd7b57e54)) ? (rb_state16.var_2) : (0x24edacf9));
        rb_state16.var_28 = (rb_state16.var_28) - (220 < rb_input_size ? (uint32_t)rb_input[220] : 0x8efa417b);
        #endif
        next->cksm = CKSM;
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ (0xa7b2f72d);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ ((0xde61a696) - (0x95411d03));
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x1a659580)) {
            pthread_mutex_lock(&(rb_state18.lock_29));
            rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_4);
            rb_state18.var_5 = (rb_state18.var_5) + (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_3) : (0x3043fa80));
            rb_state18.var_6 = (rb_state18.var_6) - (77 < rb_input_size ? (uint32_t)rb_input[77] : 0xbcc274b7);
            rb_state18.var_7 = (rb_state18.var_7) - (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_4) : (0x823a8f5));
            rb_state18.var_8 = (rb_state18.var_8) ^ (rb_state18.var_5);
            rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_6) : (0x65f0a42));
            rb_state18.var_10 = (rb_state18.var_10) + (((0x3ffd601c) + (rb_state18.var_7)) ^ (0x20bdb88c));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((0xb5faba9f) ^ (rb_state18.var_8)) + (0xe3f14463));
            rb_state18.var_12 = (rb_state18.var_12) ^ (((0x36f81f55) - (rb_state18.var_9)) + (rb_state18.var_7));
            rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_10) : (0x229344a3));
            rb_state18.var_14 = (rb_state18.var_14) - (((((0xeea083b6) + (0xab8f3a36)) + (0xd0d19d3d)) + (rb_state18.var_11)) + (rb_state18.var_12));
            rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_13);
            rb_state18.var_16 = (rb_state18.var_16) ^ (rb_state18.var_14);
            rb_state18.var_17 = (rb_state18.var_17) - (((((0x26fc54e7) + (rb_state18.var_15)) + (0x3220c589)) - (rb_state18.var_9)) ^ (rb_state18.var_16));
            rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_17);
            rb_state18.var_2 = rb_state18.var_3;
            pthread_mutex_unlock(&(rb_state18.lock_29));
        }
        if ((rb_state18.var_0) == (0x1a659580)) {
            if (!((rb_state18.var_2) == (rb_state18.var_3))) {
                racebench_trigger(18);
            }
        }
        #endif
        curr->size = size;
    } else {
        next = curr->next;
    }

    if (!prev) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + (0xccdef63e);
        if ((rb_state2.var_2) == (0xddc009a4)) {
            pthread_mutex_lock(&(rb_state2.lock_27));
            rb_state2.var_5 = 0xb0978e25;
            rb_state2.var_6 = (rb_state2.var_6) ^ ((0xa973e15b) + (0x7b87098a));
            rb_state2.var_7 = (rb_state2.var_7) + (rb_state2.var_3);
            rb_state2.var_8 = (rb_state2.var_8) + (rb_state2.var_4);
            rb_state2.var_9 = (rb_state2.var_9) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_5) : (0x5a2ea4f3));
            rb_state2.var_10 = (rb_state2.var_10) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0x453cea34));
            rb_state2.var_11 = (rb_state2.var_11) ^ (rb_state2.var_8);
            rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_9);
            rb_state2.var_13 = (rb_state2.var_13) - (((0xae148ca7) + (0x2924d92e)) ^ (rb_state2.var_10));
            rb_state2.var_14 = (rb_state2.var_14) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_11) : (0xab442b4a));
            rb_state2.var_15 = (rb_state2.var_15) - (((0xaf36df9b) - (rb_state2.var_12)) ^ (rb_state2.var_7));
            rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_14) : (0xddbdc301));
            rb_state2.var_17 = (rb_state2.var_17) - (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_16) : (0xdb25220b));
            rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_17);
            rb_state2.var_3 = rb_state2.var_4;
            pthread_mutex_unlock(&(rb_state2.lock_27));
        }
        if ((rb_state2.var_2) == (0xddc009a4)) {
            rb_state2.var_28 = !((rb_state2.var_3) == (rb_state2.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x16f8044a)) {
            rb_state8.var_5 = 0x1;
        }
        if ((rb_state8.var_0) == (0x16f8044a)) {
            pthread_mutex_lock(&(rb_state8.lock_21));
            rb_state8.var_6 = 0x5d38ae1f;
            rb_state8.var_7 = 0x3f963991;
            rb_state8.var_8 = (rb_state8.var_8) + (rb_state8.var_4);
            rb_state8.var_9 = (rb_state8.var_9) + (rb_state8.var_5);
            rb_state8.var_10 = (rb_state8.var_10) - (((0xdbf48be5) + (rb_state8.var_6)) + (rb_state8.var_3));
            rb_state8.var_11 = (rb_state8.var_11) ^ (((0xf0ee3a3f) + (rb_state8.var_7)) - (rb_state8.var_7));
            rb_state8.var_12 = (rb_state8.var_12) ^ (rb_state8.var_6);
            rb_state8.var_13 = (rb_state8.var_13) + (((0x35e15db2) - (rb_state8.var_8)) - (rb_state8.var_8));
            rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0xcf5273f1));
            rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_11) == (0x65743dd7)) ? (rb_state8.var_12) : (0x3a35c8a1));
            rb_state8.var_16 = (rb_state8.var_16) - (((0x9753fc2) - (rb_state8.var_13)) + (0x49662940));
            rb_state8.var_17 = (rb_state8.var_17) - (rb_state8.var_14);
            rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_15) : (0x18c49e42));
            rb_state8.var_19 = (rb_state8.var_19) + (((((0xff3e3c58) + (rb_state8.var_17)) ^ (rb_state8.var_16)) ^ (0xb2fdd65c)) + (0x90469fd1));
            rb_state8.var_20 = (rb_state8.var_20) - (((((0x5dff9192) + (rb_state8.var_19)) - (0xb481980e)) ^ (rb_state8.var_18)) - (rb_state8.var_10));
            rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_20);
            rb_state8.var_3 = rb_state8.var_4;
            pthread_mutex_unlock(&(rb_state8.lock_21));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
        #endif
        gm->freelist = next;
    } else {
        prev->next = next;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_1) : (0x4014b92));
    #endif
    curr->next = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_12);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_0) == (0x3196916)) ? (rb_state2.var_2) : (0xd28038c3));
    #endif
    curr->free = FALSE;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_19) == (0xa359f94)) {
        pthread_mutex_lock(&(rb_state2.lock_47));
        rb_state2.var_30 = 0x40b5cc1;
        rb_state2.var_31 = (rb_state2.var_31) + (0x9d1ea3f0);
        rb_state2.var_32 = (rb_state2.var_32) ^ (0x3bb5751d);
        rb_state2.var_33 = (rb_state2.var_33) + ((0x4f574e79) ^ (rb_state2.var_24));
        rb_state2.var_34 = (rb_state2.var_34) ^ (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_30) : (0x7fd63c60));
        rb_state2.var_35 = (rb_state2.var_35) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_23) : (0x7e52fc49));
        rb_state2.var_36 = (rb_state2.var_36) ^ (((0x3e50189f) ^ (rb_state2.var_28)) - (rb_state2.var_22));
        rb_state2.var_37 = (rb_state2.var_37) ^ (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_31) : (0xc571e345));
        rb_state2.var_38 = (rb_state2.var_38) + (rb_state2.var_32);
        rb_state2.var_39 = (rb_state2.var_39) ^ (((rb_state2.var_30) == (0x0)) ? (rb_state2.var_33) : (0x22142bb1));
        rb_state2.var_40 = (rb_state2.var_40) + (((((0x42bf15b2) - (rb_state2.var_35)) - (rb_state2.var_34)) + (rb_state2.var_32)) - (rb_state2.var_31));
        rb_state2.var_41 = (rb_state2.var_41) ^ (((((0x2cd37d19) + (rb_state2.var_33)) + (0x1ca6228e)) + (rb_state2.var_37)) + (rb_state2.var_36));
        rb_state2.var_42 = (rb_state2.var_42) - (((0x1df28362) - (rb_state2.var_38)) + (rb_state2.var_34));
        rb_state2.var_43 = (rb_state2.var_43) + (((rb_state2.var_39) == (0x0)) ? (rb_state2.var_40) : (0x94c18ae4));
        rb_state2.var_44 = (rb_state2.var_44) + (((((0xc1e83649) + (0xc6d17f8)) ^ (rb_state2.var_42)) + (rb_state2.var_41)) + (rb_state2.var_35));
        rb_state2.var_45 = (rb_state2.var_45) + (((rb_state2.var_36) == (0x0)) ? (rb_state2.var_43) : (0xc28a2a0f));
        rb_state2.var_46 = (rb_state2.var_46) + (((rb_state2.var_44) == (0x0)) ? (rb_state2.var_45) : (0xecbaaa7c));
        rb_state2.var_21 = (rb_state2.var_21) ^ (rb_state2.var_46);
        pthread_mutex_unlock(&(rb_state2.lock_47));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_0);
    if ((rb_state10.var_1) == (0xdfce89f9)) {
        rb_state10.var_20 = 0x97d3a381;
        rb_state10.var_21 = 0x5d468cf7;
        rb_state10.var_22 = 0xe21c8d58;
        rb_state10.var_23 = (rb_state10.var_23) - (((((0x10543c1b) ^ (rb_state10.var_22)) - (rb_state10.var_10)) ^ (rb_state10.var_21)) - (0xc5f3edf3));
        rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_23) : (0x1d91291a));
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_24);
    }
    #endif
    curr = NODE_ADD(curr, nodesize);

    return ((VOID *)curr);
}

VOID *GlobalCalloc(n, size)
UINT n;
UINT size;
{
    UINT nbytes;
    UINT huge *p;
    VOID huge *q;

    nbytes = ROUND_UP(n * size);

    p = q = GlobalMalloc(nbytes, "GlobalCalloc");

    nbytes >>= 2;
    while (nbytes--) {
        *p++ = 0;
    }

    return (q);
}

VOID *GlobalRealloc(p, size)
VOID *p;
UINT size;
{
    UINT oldsize;
    UINT newsize;
    UINT totsize;
    VOID huge *q;
    UINT huge *r;
    UINT huge *s;
    NODE huge *pn;
    NODE huge *prev;
    NODE huge *curr;
    NODE huge *next;
    NODE huge *node;

    if (!size) {
        GlobalFree(p);
        return (NULL);
    }

    if (!p) {
        return (GlobalMalloc(size, "GlobalRealloc"));
    }

    pn = NODE_ADD(p, -nodesize);

    if (pn->cksm != CKSM) {
        fprintf(stderr, "GlobalRealloc: Attempted to realloc node with invalid checksum.\n");
        exit(1);
    }

    if (pn->free) {
        fprintf(stderr, "GlobalRealloc: Attempted to realloc an unallocated node.\n");
        exit(1);
    }

    newsize = ROUND_UP(size);
    oldsize = pn->size;

    if (newsize <= oldsize) {
        if (oldsize - newsize < THRESHOLD) {
            return (p);
        }

        pn->size = newsize;

        next = NODE_ADD(p, newsize);
        next->size = oldsize - nodesize - newsize;
        next->next = NULL;
        next->free = FALSE;
        next->cksm = CKSM;
        next = NODE_ADD(next, nodesize);

        GlobalFree(next);
        return (p);
    }

    next = NODE_ADD(p, oldsize);
    totsize = oldsize + nodesize + next->size;

    { pthread_mutex_lock(&(gm->memlock)); }
    if (next < endmem && next->free && totsize >= newsize) {

        prev = NULL;
        curr = gm->freelist;

        while (curr && curr < next && curr < endmem) {
            prev = curr;
            curr = curr->next;
        }

        if (curr != next) {
            fprintf(stderr, "GlobalRealloc: Could not find next node in free list.\n");
            exit(1);
        }

        if (totsize - newsize < THRESHOLD) {

            if (!prev) {
                gm->freelist = next->next;
            } else {
                prev->next = next->next;
            }

            next->next = NULL;
            next->free = FALSE;
            pn->size = totsize;

            { pthread_mutex_unlock(&(gm->memlock)); }
            return (p);
        } else {

            node = NODE_ADD(p, newsize);
            node->next = next->next;
            node->size = totsize - nodesize - newsize;
            node->free = TRUE;
            node->cksm = CKSM;

            if (!prev) {
                gm->freelist = node;
            } else {
                prev->next = node;
            }

            next->next = NULL;
            next->free = FALSE;
            pn->size = newsize;

            { pthread_mutex_unlock(&(gm->memlock)); }
            return (p);
        }
    }

    { pthread_mutex_unlock(&(gm->memlock)); }

    s = q = GlobalMalloc(newsize, "GlobalRealloc");
    if (!q) {
        return (NULL);
    }

    r = (UINT huge *)p;
    oldsize >>= 2;

    while (oldsize--) {
        *s++ = *r++;
    }

    GlobalFree(p);
    return (q);
}

VOID GlobalFree(p)
VOID *p;
{
    BOOL pcom;
    BOOL ncom;
    NODE huge *pn;
    NODE huge *prev;
    NODE huge *curr;
    NODE huge *next;

    if (!begmem) {
        return;
    }

    pn = NODE_ADD(p, -nodesize);

    if (pn->cksm != CKSM) {
        fprintf(stderr, "GlobalFree: Attempted to free node with invalid checksum.\n");
        exit(1);
    }

    if (pn->free) {
        fprintf(stderr, "GlobalFree: Attempted to free unallocated node.\n");
        exit(1);
    }

    pcom = FALSE;
    prev = NULL;

    { pthread_mutex_lock(&(gm->memlock)); }
    if (gm->freelist) {

        curr = gm->freelist;

        while (curr < pn && curr < endmem) {
            if (curr->cksm != CKSM) {
                fprintf(stderr, "GlobalFree: Invalid checksum in previous node.\n");
                exit(1);
            }

            if (curr->free) {
                prev = curr;
                pcom = TRUE;
            } else {
                pcom = FALSE;
            }

            curr = NODE_ADD(curr, curr->size + nodesize);
        }

        if (curr >= endmem) {
            fprintf(stdout, "freelist=0x%08lX, curr=0x%08lX, size=0x%08lX, pn=0x%08lX, endmem=0x%08lX\n", gm->freelist, curr, curr->size, pn, endmem);
            fprintf(stderr, "GlobalFree: Search for previous block fell off end of memory.\n");
            exit(1);
        }
    }

    ncom = TRUE;
    next = NULL;
    curr = NODE_ADD(pn, pn->size + nodesize);

    while (!next && curr < endmem) {
        if (curr->cksm != CKSM) {
            fprintf(stderr, "GlobalFree: Invalid checksum in next node.\n");
            exit(1);
        }

        if (curr->free) {
            next = curr;
        } else {
            ncom = FALSE;
        }

        curr = NODE_ADD(curr, curr->size + nodesize);
    }

    if (!next) {
        ncom = FALSE;
    }

    curr = pn;
    curr->free = TRUE;

    if (next && !ncom && pcom) {
        prev->next = next;
        prev->size += curr->size + nodesize;
    } else if (next && !ncom && prev && !pcom) {
        prev->next = curr;
        curr->next = next;
    } else if (next && !ncom && !prev) {
        gm->freelist = curr;
        curr->next = next;
    } else if (ncom && pcom) {
        prev->next = next->next;
        prev->size += curr->size + next->size + 2 * nodesize;
    } else if (ncom && prev && !pcom) {
        prev->next = curr;
        curr->next = next->next;
        curr->size += next->size + nodesize;
    } else if (ncom && !prev) {
        gm->freelist = curr;
        curr->next = next->next;
        curr->size += next->size + nodesize;
    } else if (!next && pcom) {
        prev->next = NULL;
        prev->size += curr->size + nodesize;
    } else if (!next && prev && !pcom) {
        prev->next = curr;
        curr->next = NULL;
    } else if (!next && !prev) {
        gm->freelist = curr;
        curr->next = NULL;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    return;
}

UINT GlobalMemAvl() {
    UINT total;
    NODE huge *curr;

    { pthread_mutex_lock(&(gm->memlock)); }
    total = 0;
    curr = gm->freelist;

    while (curr) {
        total += curr->size;
        curr = curr->next;
    }

    total = ROUND_DN(total);

    { pthread_mutex_unlock(&(gm->memlock)); }
    return (total);
}

UINT GlobalMemMax() {
    UINT max;
    NODE huge *curr;

    { pthread_mutex_lock(&(gm->memlock)); }
    max = 0;
    curr = gm->freelist;

    while (curr) {
        max = (curr->size > max ? curr->size : max);
        curr = curr->next;
    }

    max = ROUND_DN(max);

    { pthread_mutex_unlock(&(gm->memlock)); }
    return (max);
}

VOID *ObjectMalloc(ObjectType, count)
INT ObjectType;
INT count;
{
    INT n;
    VOID *p;

    switch (ObjectType) {
    case OT_GRID:
        n = count * sizeof(GRID);
        p = GlobalMalloc(n, "GRID");

        mem_grid += n;
        maxmem_grid = Max(mem_grid, maxmem_grid);
        break;

    case OT_VOXEL:
        n = count * sizeof(VOXEL);
        p = GlobalMalloc(n, "VOXEL");

        mem_voxel += n;
        maxmem_voxel = Max(mem_voxel, maxmem_voxel);
        break;

    case OT_HASHTABLE: {
        INT i;
        VOXEL **x;

        n = count * sizeof(VOXEL *);
        p = GlobalMalloc(n, "HASHTABLE");
        x = p;

        for (i = 0; i < count; i++) {
            x[i] = NULL;
        }

        mem_hashtable += n;
        maxmem_hashtable = Max(mem_hashtable, maxmem_hashtable);
    } break;

    case OT_EMPTYCELLS: {
        INT i, w;
        UINT *x;

        w = 1 + count / (8 * sizeof(UINT));
        n = w * sizeof(UINT);
        p = GlobalMalloc(n, "EMPTYCELLS");
        x = p;

        for (i = 0; i < w; i++) {
            x[i] = ~0;
        }

        mem_emptycells += n;
        maxmem_emptycells = Max(mem_emptycells, maxmem_emptycells);
    } break;

    case OT_BINTREE:
        n = count * sizeof(BTNODE);
        p = GlobalMalloc(n, "BINTREE");

        mem_bintree += n;
        maxmem_bintree = Max(mem_bintree, maxmem_bintree);
        break;

    case OT_PEPARRAY:
        n = count * sizeof(ELEMENT *);
        p = GlobalMalloc(n, "PEPARRAY");

        mem_pepArray += n;
        maxmem_pepArray = Max(mem_pepArray, maxmem_pepArray);
        break;

    default:
        printf("ObjectMalloc: Unknown object type: %d\n", ObjectType);
        exit(-1);
    }

    return (p);
}

VOID ObjectFree(ObjectType, count, p)
INT ObjectType;
INT count;
VOID *p;
{
    INT n;

    GlobalFree(p);

    switch (ObjectType) {
    case OT_GRID:
        n = count * sizeof(GRID);
        mem_grid -= n;
        break;

    case OT_VOXEL:
        n = count * sizeof(VOXEL);
        mem_voxel -= n;
        break;

    case OT_HASHTABLE:
        n = count * sizeof(VOXEL *);
        mem_hashtable -= n;
        break;

    case OT_EMPTYCELLS:
        n = 1 + count / (8 * sizeof(UINT));
        n = n * sizeof(UINT);
        mem_emptycells -= n;
        break;

    case OT_BINTREE:
        n = count * sizeof(BTNODE);
        mem_bintree -= n;
        break;

    case OT_PEPARRAY:
        n = count * sizeof(ELEMENT *);
        mem_pepArray -= n;
        break;

    default:
        printf("ObjectFree: Unknown object type: %d\n", ObjectType);
        exit(-1);
    }
}

RAYINFO *ma_rayinfo(r)
RAY *r;
{
    RAYINFO *p;

    if (r->ri_indx + 1 > MAX_RAYINFO + 1) {
        fprintf(stderr, "error ma_rayinfo \n");
        exit(-1);
    }

    p = (RAYINFO *)(&(r->rinfo[r->ri_indx]));

    r->ri_indx += 1;

    return (p);
}

VOID free_rayinfo(r)
RAY *r;
{ r->ri_indx -= 1; }

VOID reset_rayinfo(r)
RAY *r;
{ r->ri_indx = 0; }

VOID ma_print() {
    INT i;
    INT mem_total;
    INT maxmem_total;

    mem_total = mem_grid + mem_hashtable + mem_emptycells;
    mem_total += mem_voxel + mem_bintree;

    maxmem_total = maxmem_grid + maxmem_hashtable + maxmem_emptycells;
    maxmem_total += maxmem_voxel + maxmem_bintree;

    fprintf(stdout, "\n****** Hierarchial uniform grid memory allocation summary ******* \n\n");
    fprintf(stdout, "     < struct >:            < current >   < maximum >    < sizeof > \n");
    fprintf(stdout, "     <  bytes >:             <  bytes >   <   bytes >    <  bytes > \n\n");
    fprintf(stdout, "     grid:                %11ld   %11ld   %11d \n", mem_grid, maxmem_grid, sizeof(GRID));
    fprintf(stdout, "     hashtable entries:   %11ld   %11ld   %11d \n", mem_hashtable, maxmem_hashtable, sizeof(VOXEL **));
    fprintf(stdout, "     emptycell entries:   %11ld   %11ld   %11d \n", mem_emptycells, maxmem_emptycells, sizeof(UINT));
    fprintf(stdout, "     voxel:               %11ld   %11ld   %11d \n", mem_voxel, maxmem_voxel, sizeof(VOXEL));
    fprintf(stdout, "     bintree_node:        %11ld   %11ld   %11d \n", mem_bintree, maxmem_bintree, sizeof(BTNODE));

    fprintf(stdout, "\n");
    fprintf(stdout, "     Totals:              %11ld   %11ld      \n\n", mem_total, maxmem_total);
}