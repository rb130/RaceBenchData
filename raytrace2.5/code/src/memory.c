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

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_3);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) + ((0x1a7504fa) ^ (rb_state19.var_3));
    if ((rb_state19.var_4) == (0x2048e89)) {
        rb_state19.var_6 = 0x74e4bf1b;
        rb_state19.var_7 = 0x352938c6;
        rb_state19.var_8 = (rb_state19.var_8) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_6) : (0x25681f7c));
        rb_state19.var_9 = (rb_state19.var_9) - (((((0xc09a14e0) - (rb_state19.var_6)) - (0x7141eae9)) - (0x54f4ec99)) ^ (rb_state19.var_5));
        rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_7) : (0x270d5a67));
        rb_state19.var_11 = (rb_state19.var_11) + (((((0x72a67f18) + (0x84fd1077)) - (rb_state19.var_9)) + (rb_state19.var_9)) - (rb_state19.var_8));
        rb_state19.var_12 = (rb_state19.var_12) ^ (rb_state19.var_10);
        rb_state19.var_13 = (rb_state19.var_13) - (rb_state19.var_11);
        rb_state19.var_14 = (rb_state19.var_14) + (((((0xcdfab10b) - (0xcdebfa27)) ^ (rb_state19.var_12)) + (rb_state19.var_13)) + (rb_state19.var_10));
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_14);
    }
    #endif
    prev = NULL;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_16 = (rb_state2.var_16) - (0xd93b71c4);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - ((0x250d9b38) + (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (156 < rb_input_size ? (uint32_t)rb_input[156] : 0x4992b426);
    #endif
    curr = gm->freelist;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_3);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + ((0xacfa3342) + (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) ^ ((0x81ae3131) ^ (0x679f5171));
    if ((rb_state11.var_4) == (0xe4b405dc)) {
        rb_state11.var_5 = rb_state11.var_0;
    }
    if ((rb_state11.var_4) == (0xe4b405dc)) {
        if (!((rb_state11.var_0) == (rb_state11.var_5))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_2) == (0xe0b22758)) ? (rb_state13.var_1) : (0x63aba931));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (105 < rb_input_size ? (uint32_t)rb_input[105] : 0xe87da933);
    #endif
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
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + (261 < rb_input_size ? (uint32_t)rb_input[261] : 0xa4b428a4);
        if ((rb_state1.var_0) == (0x585eb5d3)) {
            if ((rb_state1.var_4) != (0x0)) {
                if (!((rb_state1.var_4) == (rb_state1.var_29))) {
                    racebench_trigger(1);
                }
            }
        }
        #endif
        next = NODE_ADD(curr, nodesize + size);
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (0x95e1e1e4);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - ((0xb02dbd36) + (rb_state17.var_1));
        #endif
        next->size = curr->size - nodesize - size;
        next->next = curr->next;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_4) == (0x7fb77d14)) {
            rb_state2.var_24 = (rb_state2.var_24) - (((((0xa6ede4e6) ^ (rb_state2.var_23)) - (0xc9447f2b)) - (rb_state2.var_24)) + (rb_state2.var_5));
            rb_state2.var_25 = (rb_state2.var_25) ^ (rb_state2.var_6);
            rb_state2.var_26 = (rb_state2.var_26) + (183 < rb_input_size ? (uint32_t)rb_input[183] : 0xecc84abf);
            rb_state2.var_27 = (rb_state2.var_27) - (((0x298178ab) + (0xe3b8e829)) - (rb_state2.var_24));
            rb_state2.var_28 = (rb_state2.var_28) + (rb_state2.var_25);
            rb_state2.var_29 = (rb_state2.var_29) - (rb_state2.var_26);
            rb_state2.var_30 = (rb_state2.var_30) ^ (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_27) : (0x9ca30fae));
            rb_state2.var_31 = (rb_state2.var_31) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_28) : (0xf409f3f4));
            rb_state2.var_32 = (rb_state2.var_32) + (((0xc1b60d6c) + (rb_state2.var_27)) + (rb_state2.var_29));
            rb_state2.var_33 = (rb_state2.var_33) ^ (((rb_state2.var_28) == (0x0)) ? (rb_state2.var_30) : (0x5b69e00e));
            rb_state2.var_34 = (rb_state2.var_34) + (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_31) : (0x8bdc4bf4));
            rb_state2.var_35 = (rb_state2.var_35) ^ (((rb_state2.var_30) == (0x0)) ? (rb_state2.var_32) : (0xb704604d));
            rb_state2.var_36 = (rb_state2.var_36) ^ (rb_state2.var_33);
            rb_state2.var_37 = (rb_state2.var_37) + (rb_state2.var_34);
            rb_state2.var_38 = (rb_state2.var_38) - (rb_state2.var_35);
            rb_state2.var_39 = (rb_state2.var_39) ^ (rb_state2.var_36);
            rb_state2.var_40 = (rb_state2.var_40) - (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_37) : (0x344c0f3a));
            rb_state2.var_41 = (rb_state2.var_41) + (((0x3fcdb885) + (0xca5702b7)) - (rb_state2.var_38));
            rb_state2.var_42 = (rb_state2.var_42) - (((0x9451560) - (rb_state2.var_39)) + (0xf8c815f0));
            rb_state2.var_43 = (rb_state2.var_43) + (((((0x6fb434a8) + (rb_state2.var_41)) ^ (rb_state2.var_32)) + (rb_state2.var_40)) + (rb_state2.var_33));
            rb_state2.var_44 = (rb_state2.var_44) + (((((0xe748018d) ^ (0xbc2a9d19)) + (0x7da6e901)) + (rb_state2.var_42)) - (rb_state2.var_43));
            rb_state2.var_9 = (rb_state2.var_9) - (rb_state2.var_44);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_4 = (rb_state8.var_4) + (((rb_state8.var_2) == (0x49078f78)) ? (rb_state8.var_1) : (0xe52560d4));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) ^ ((0xebd35d7b) ^ (rb_state10.var_3));
        rb_state10.var_3 = (rb_state10.var_3) + ((0x32464d9a) ^ (rb_state10.var_0));
        #endif
        next->free = TRUE;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_2);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_4) : (0x941ddcc9));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_4);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ (0xd865eacf);
        #endif
        next->cksm = CKSM;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
        rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_18);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0xfffffdd2)) {
            rb_state14.var_6 = 0x767cf756;
            rb_state14.var_7 = 0x69230d49;
            rb_state14.var_8 = (rb_state14.var_8) - (rb_state14.var_7);
            rb_state14.var_9 = (rb_state14.var_9) - (((((0x65b0867) ^ (rb_state14.var_5)) ^ (0x2f406d57)) - (0x45b28f48)) ^ (rb_state14.var_6));
            rb_state14.var_10 = (rb_state14.var_10) + (((((0x4a0fff40) - (0x4562c43e)) + (rb_state14.var_8)) - (rb_state14.var_9)) ^ (rb_state14.var_6));
            rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_10);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_4) == (0x94b5e476)) {
            rb_state18.var_6 = 0x7cadfb04;
            rb_state18.var_7 = 0x7ae068a0;
            rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_6) == (0x7cadfb04)) ? (rb_state18.var_5) : (0x72c8081a));
            rb_state18.var_9 = (rb_state18.var_9) ^ (((((0x8f9f9dab) + (0xf221bdbe)) + (rb_state18.var_7)) + (rb_state18.var_6)) - (rb_state18.var_7));
            rb_state18.var_10 = (rb_state18.var_10) + (((((0x3f1736cc) - (rb_state18.var_8)) - (rb_state18.var_9)) - (0xe5ed1aae)) + (0x4e4e55f0));
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_10);
        }
        #endif
        curr->size = size;
    } else {
        next = curr->next;
    }

    if (!prev) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) + (0x19e9a48c);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) ^ (83 < rb_input_size ? (uint32_t)rb_input[83] : 0x8a8cd55b);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + ((0x8e4e2a73) + (0x36dbd19e));
        #endif
        gm->freelist = next;
    } else {
        prev->next = next;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_20) : (0xb36f5980));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_4);
    if ((rb_state8.var_0) == (0xc1b6a898)) {
        rb_state8.var_5 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0xc1b6a898)) {
        pthread_mutex_lock(&(rb_state8.lock_14));
        if (!((rb_state8.var_1) == (rb_state8.var_5))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xdcd468ec)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_6 = 0x77740054;
        rb_state10.var_7 = (rb_state10.var_7) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_4) : (0xa11d2e13));
        rb_state10.var_8 = (rb_state10.var_8) - (52 < rb_input_size ? (uint32_t)rb_input[52] : 0xa93bed6e);
        rb_state10.var_9 = (rb_state10.var_9) ^ (194 < rb_input_size ? (uint32_t)rb_input[194] : 0xf3890252);
        rb_state10.var_10 = (rb_state10.var_10) - (((((0x34e1b0e3) - (rb_state10.var_6)) ^ (rb_state10.var_7)) + (rb_state10.var_7)) ^ (rb_state10.var_6));
        rb_state10.var_11 = (rb_state10.var_11) ^ (rb_state10.var_8);
        rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_9) : (0x72cc2f36));
        rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_10);
        rb_state10.var_14 = (rb_state10.var_14) + (((0xcc293130) + (rb_state10.var_11)) + (rb_state10.var_9));
        rb_state10.var_15 = (rb_state10.var_15) + (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x9159cfa));
        rb_state10.var_16 = (rb_state10.var_16) + (rb_state10.var_14);
        rb_state10.var_17 = (rb_state10.var_17) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_15) : (0xb3be4e19));
        rb_state10.var_18 = (rb_state10.var_18) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_17) : (0x3e5bc2b6));
        rb_state10.var_5 = (rb_state10.var_5) ^ (rb_state10.var_18);
        rb_state10.var_4 = rb_state10.var_5;
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    if ((rb_state10.var_1) == (0xdcd468ec)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        if (!((rb_state10.var_4) == (rb_state10.var_5))) {
            racebench_trigger(10);
        }
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (203 < rb_input_size ? (uint32_t)rb_input[203] : 0xbb6e8e40);
    rb_state11.var_3 = (rb_state11.var_3) ^ (334 < rb_input_size ? (uint32_t)rb_input[334] : 0x258555eb);
    #endif
    curr->next = NULL;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (291 < rb_input_size ? (uint32_t)rb_input[291] : 0x6dded674);
    rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_4) == (0xe4b405dc)) {
        rb_state11.var_6 = 0xb41d1484;
        rb_state11.var_7 = (rb_state11.var_7) ^ (((0xd0a1070a) ^ (rb_state11.var_5)) ^ (0xa2141417));
        rb_state11.var_8 = (rb_state11.var_8) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xf0abf78b));
        rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_2) == (0xe7d48e44)) {
        pthread_mutex_lock(&(rb_state17.lock_40));
        rb_state17.var_7 = 0x2ae3226b;
        rb_state17.var_8 = (rb_state17.var_8) - ((0xde6ec09d) + (0x6c8b0565));
        rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_7);
        rb_state17.var_10 = (rb_state17.var_10) ^ (((((0x56e12931) - (0xf03e2399)) - (0xe8c7e9fd)) + (rb_state17.var_6)) + (rb_state17.var_5));
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0x2d6dbbbb));
        rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_9);
        rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_10) : (0x186900f1));
        rb_state17.var_14 = (rb_state17.var_14) ^ (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_11) : (0x55a6351c));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_12) : (0x929c3a4e));
        rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_13) : (0xf8307258));
        rb_state17.var_17 = (rb_state17.var_17) - (rb_state17.var_14);
        rb_state17.var_18 = (rb_state17.var_18) ^ (((0xafbfe4e6) - (rb_state17.var_15)) + (rb_state17.var_12));
        rb_state17.var_19 = (rb_state17.var_19) + (((((0x912d0219) + (rb_state17.var_16)) + (rb_state17.var_17)) ^ (0x53c78951)) - (0x55b20c9f));
        rb_state17.var_20 = (rb_state17.var_20) + (rb_state17.var_18);
        rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_19) == (0x0)) ? (rb_state17.var_20) : (0xb8aebf04));
        rb_state17.var_6 = (rb_state17.var_6) - (rb_state17.var_21);
        rb_state17.var_5 = rb_state17.var_6;
        pthread_mutex_unlock(&(rb_state17.lock_40));
    }
    if ((rb_state17.var_2) == (0xe7d48e44)) {
        if (!((rb_state17.var_5) == (rb_state17.var_6))) {
            racebench_trigger(17);
        }
    }
    #endif
    curr->free = FALSE;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) ^ (0xdba05f30);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_3);
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