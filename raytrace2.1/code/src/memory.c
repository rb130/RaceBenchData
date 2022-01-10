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

    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xd340b7f3)) {
        pthread_mutex_lock(&(rb_state7.lock_30));
        if ((rb_state7.var_1) != (0x0)) {
            if (!((rb_state7.var_1) == (rb_state7.var_19))) {
                racebench_trigger(7);
            }
        }
        pthread_mutex_unlock(&(rb_state7.lock_30));
    }
    #endif
    prev = NULL;
    curr = gm->freelist;
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        rb_state4.var_13 = 0xa87a4c59;
        rb_state4.var_14 = (rb_state4.var_14) - (20 < rb_input_size ? (uint32_t)rb_input[20] : 0x5bcfcacf);
        rb_state4.var_15 = (rb_state4.var_15) + (((0xac46e964) + (0xe95785d1)) ^ (rb_state4.var_9));
        rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_13) == (0xa87a4c59)) ? (rb_state4.var_10) : (0xb15f633c));
        rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_14) == (0xffffff8d)) ? (rb_state4.var_15) : (0x9c223172));
        rb_state4.var_18 = (rb_state4.var_18) + (((((0xbe43c41f) + (rb_state4.var_17)) + (rb_state4.var_11)) ^ (0xf9eb48e9)) ^ (rb_state4.var_16));
        rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xe3d37670)) {
        rb_state8.var_3 = 0x83f44ab3;
        rb_state8.var_4 = 0xd7a9fed1;
        rb_state8.var_5 = 0xafc1f3c;
        rb_state8.var_6 = (rb_state8.var_6) - (rb_state8.var_2);
        rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_5) : (0x400b0a7e));
        rb_state8.var_8 = (rb_state8.var_8) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_3) : (0xe788672c));
        rb_state8.var_9 = (rb_state8.var_9) - (rb_state8.var_4);
        rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0x4f6840d));
        rb_state8.var_11 = (rb_state8.var_11) + (((((0x3eed2c85) ^ (rb_state8.var_7)) + (rb_state8.var_7)) ^ (rb_state8.var_6)) ^ (rb_state8.var_8));
        rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_9) : (0xce4ef160));
        rb_state8.var_13 = (rb_state8.var_13) + (rb_state8.var_10);
        rb_state8.var_14 = (rb_state8.var_14) ^ (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x6f57996e));
        rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_13) : (0xb5d0cda1));
        rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_14);
        rb_state8.var_17 = (rb_state8.var_17) + (((0xe27ddf65) + (0xfc1dbb49)) ^ (rb_state8.var_15));
        rb_state8.var_18 = (rb_state8.var_18) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_16) : (0xeb750bab));
        rb_state8.var_19 = (rb_state8.var_19) + (rb_state8.var_17);
        rb_state8.var_20 = (rb_state8.var_20) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_18) : (0xf7aa4502));
        rb_state8.var_21 = (rb_state8.var_21) - (((((0xae387053) - (rb_state8.var_12)) - (rb_state8.var_13)) + (rb_state8.var_19)) - (rb_state8.var_20));
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xfffffb66)) {
        rb_state9.var_2 = rb_state9.var_1;
    }
    if ((rb_state9.var_0) == (0xfffffb66)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xbc728172)) {
        pthread_mutex_lock(&(rb_state14.lock_17));
        rb_state14.var_3 = 0x3b40fee7;
        rb_state14.var_4 = 0xb0fffc57;
        rb_state14.var_5 = (rb_state14.var_5) - (((0x23407318) ^ (rb_state14.var_3)) - (0x6f6e04d3));
        rb_state14.var_6 = (rb_state14.var_6) ^ (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_4) : (0x31cc776d));
        rb_state14.var_7 = (rb_state14.var_7) ^ (rb_state14.var_1);
        rb_state14.var_8 = (rb_state14.var_8) ^ (rb_state14.var_5);
        rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_7) : (0x9ab3f36c));
        rb_state14.var_10 = (rb_state14.var_10) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_9) : (0xbb9a4ae8));
        rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_10);
        rb_state14.var_1 = rb_state14.var_2;
        pthread_mutex_unlock(&(rb_state14.lock_17));
    }
    if ((rb_state14.var_0) == (0xbc728172)) {
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
    }
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
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x0)) {
            rb_state5.var_3 = 0x534e9995;
            rb_state5.var_4 = 0x1b55011;
            rb_state5.var_5 = (rb_state5.var_5) + (553 < rb_input_size ? (uint32_t)rb_input[553] : 0x8da9e056);
            rb_state5.var_6 = (rb_state5.var_6) + (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0x67ceaf6));
            rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_3) : (0xce0a6b2e));
            rb_state5.var_8 = (rb_state5.var_8) + (((0x2bb52b5e) + (rb_state5.var_2)) + (0x9206a697));
            rb_state5.var_9 = (rb_state5.var_9) ^ (((((0x1dd31c14) + (rb_state5.var_5)) + (0x5f2d0098)) ^ (rb_state5.var_6)) ^ (rb_state5.var_4));
            rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0xc0237f61));
            rb_state5.var_11 = (rb_state5.var_11) - (((0x1c28bd2a) + (0x21023304)) - (rb_state5.var_8));
            rb_state5.var_12 = (rb_state5.var_12) - (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_10) : (0x7f5e411a));
            rb_state5.var_13 = (rb_state5.var_13) + (((0xad357657) - (0x72bf28b)) + (rb_state5.var_11));
            rb_state5.var_14 = (rb_state5.var_14) - (((0xae902317) - (rb_state5.var_12)) + (rb_state5.var_7));
            rb_state5.var_15 = (rb_state5.var_15) ^ (((((0x5d4e1595) ^ (rb_state5.var_13)) ^ (0xc75f42a7)) - (rb_state5.var_14)) ^ (rb_state5.var_8));
            rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0xdecead34)) {
            rb_state11.var_2 = rb_state11.var_1;
        }
        if ((rb_state11.var_0) == (0xdecead34)) {
            if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0xfffffed0)) {
            rb_state15.var_2 = rb_state15.var_1;
        }
        if ((rb_state15.var_0) == (0xfffffed0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                racebench_trigger(15);
            }
        }
        #endif
        next = NODE_ADD(curr, nodesize + size);
        next->size = curr->size - nodesize - size;
        next->next = curr->next;
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x16bebdde)) {
            rb_state17.var_3 = 0x9b87b0e7;
            rb_state17.var_4 = (rb_state17.var_4) + (((((0x70371c97) - (rb_state17.var_2)) + (0x15dff796)) ^ (rb_state17.var_4)) + (rb_state17.var_3));
            rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_4);
        }
        #endif
        next->free = TRUE;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x3396c124)) {
            rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0x44e55700));
            rb_state1.var_14 = (rb_state1.var_14) + (0x8c8fc3d8);
            rb_state1.var_15 = (rb_state1.var_15) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_6) : (0x2414cbae));
            rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_13) : (0x1c903fe8));
            rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_15) : (0xf4c48f46));
            rb_state1.var_18 = (rb_state1.var_18) + (((((0x44e074d8) ^ (rb_state1.var_17)) ^ (0x6c7db237)) + (0x6dd4b516)) + (rb_state1.var_16));
            rb_state1.var_12 = (rb_state1.var_12) + (rb_state1.var_18);
            rb_state1.var_1 = rb_state1.var_12;
        }
        if ((rb_state1.var_0) == (0x3396c124)) {
            pthread_mutex_lock(&(rb_state1.lock_33));
            rb_state1.var_20 = 0xe7ed9f7b;
            rb_state1.var_21 = (rb_state1.var_21) - ((0x847f450c) ^ (0x8ef4d480));
            rb_state1.var_22 = (rb_state1.var_22) - (rb_state1.var_12);
            rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_11) : (0xdcd16a60));
            rb_state1.var_24 = (rb_state1.var_24) ^ (((0xfecf5bcc) + (0x7d49c5cb)) - (rb_state1.var_21));
            rb_state1.var_25 = (rb_state1.var_25) ^ (((rb_state1.var_22) == (0x0)) ? (rb_state1.var_23) : (0xb0d327f1));
            rb_state1.var_26 = (rb_state1.var_26) ^ (((0xa35c207d) - (rb_state1.var_13)) - (rb_state1.var_24));
            rb_state1.var_27 = (rb_state1.var_27) - (((0x204f5717) + (rb_state1.var_14)) ^ (rb_state1.var_25));
            rb_state1.var_28 = (rb_state1.var_28) ^ (rb_state1.var_26);
            rb_state1.var_29 = (rb_state1.var_29) + (((0x32beed) + (rb_state1.var_27)) ^ (0xef3ce0c9));
            rb_state1.var_30 = (rb_state1.var_30) - (rb_state1.var_28);
            rb_state1.var_31 = (rb_state1.var_31) - (rb_state1.var_29);
            rb_state1.var_32 = (rb_state1.var_32) + (((((0x338fdfdc) - (rb_state1.var_30)) + (0x47d6f547)) + (rb_state1.var_15)) + (rb_state1.var_31));
            rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_32);
            rb_state1.var_1 = rb_state1.var_19;
            pthread_mutex_unlock(&(rb_state1.lock_33));
        }
        #endif
        next->cksm = CKSM;
        curr->size = size;
    } else {
        next = curr->next;
    }

    if (!prev) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x4c75d0cf)) {
            rb_state13.var_8 = 0x2717aad9;
            rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_8) : (0x2623ec4e));
            rb_state13.var_10 = (rb_state13.var_10) ^ (((((0x9b4e0f15) + (rb_state13.var_5)) - (rb_state13.var_7)) ^ (rb_state13.var_4)) - (rb_state13.var_8));
            rb_state13.var_11 = (rb_state13.var_11) + (((0x70307e6a) - (rb_state13.var_9)) ^ (rb_state13.var_9));
            rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_10) == (0x7436643c)) ? (rb_state13.var_10) : (0xa05b5635));
            rb_state13.var_13 = (rb_state13.var_13) + (((rb_state13.var_11) == (0x4fa07c64)) ? (rb_state13.var_11) : (0x26be7511));
            rb_state13.var_14 = (rb_state13.var_14) + (((((0x2553d74d) ^ (rb_state13.var_13)) ^ (rb_state13.var_12)) - (0xa787d028)) - (0x3cf06dcd));
            rb_state13.var_7 = (rb_state13.var_7) - (rb_state13.var_14);
            rb_state13.var_1 = rb_state13.var_7;
        }
        if ((rb_state13.var_0) == (0x4c75d0cf)) {
            pthread_mutex_lock(&(rb_state13.lock_30));
            rb_state13.var_16 = 0x491b17aa;
            rb_state13.var_17 = 0x46b552bf;
            rb_state13.var_18 = 0xb4a5ea5a;
            rb_state13.var_19 = (rb_state13.var_19) - (((rb_state13.var_13) == (0x4fa07c64)) ? (rb_state13.var_12) : (0xb9c17ddd));
            rb_state13.var_20 = (rb_state13.var_20) + (((((0x3c70f90f) - (rb_state13.var_16)) ^ (rb_state13.var_14)) - (rb_state13.var_17)) ^ (0x8b166f62));
            rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_18);
            rb_state13.var_22 = (rb_state13.var_22) + (((0x58441627) - (rb_state13.var_19)) ^ (rb_state13.var_15));
            rb_state13.var_23 = (rb_state13.var_23) + (((rb_state13.var_20) == (0x97572e4)) ? (rb_state13.var_21) : (0x721e7316));
            rb_state13.var_24 = (rb_state13.var_24) - (((0x409909e9) ^ (rb_state13.var_16)) ^ (rb_state13.var_22));
            rb_state13.var_25 = (rb_state13.var_25) - (((rb_state13.var_17) == (0x46b552bf)) ? (rb_state13.var_23) : (0x5bc9b4bb));
            rb_state13.var_26 = (rb_state13.var_26) + (((rb_state13.var_18) == (0xb4a5ea5a)) ? (rb_state13.var_24) : (0xd9df8b4));
            rb_state13.var_27 = (rb_state13.var_27) + (rb_state13.var_25);
            rb_state13.var_28 = (rb_state13.var_28) ^ (((rb_state13.var_19) == (0x8bc99bc4)) ? (rb_state13.var_26) : (0xe8b03bae));
            rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_27) == (0x4b5a15a6)) ? (rb_state13.var_28) : (0x989265dc));
            rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_29);
            rb_state13.var_1 = rb_state13.var_15;
            pthread_mutex_unlock(&(rb_state13.lock_30));
        }
        #endif
        gm->freelist = next;
    } else {
        prev->next = next;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    curr->next = NULL;
    curr->free = FALSE;
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