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

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xd7a66711)) {
        pthread_mutex_lock(&(rb_state6.lock_15));
        usleep(100);
        pthread_mutex_unlock(&(rb_state6.lock_15));
    }
    if ((rb_state6.var_1) == (0xd7a66711)) {
        rb_state6.var_16 = !((rb_state6.var_3) == (rb_state6.var_2));
    }
    #endif
    prev = NULL;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0x62d61f0)) {
        pthread_mutex_lock(&(rb_state8.lock_17));
        if ((rb_state8.var_2) != (0x0)) {
            if (!((rb_state8.var_2) == (rb_state8.var_12))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_17));
    }
    #endif
    curr = gm->freelist;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6fc93528)) {
        rb_state0.var_3 = 0x784319d5;
        rb_state0.var_4 = 0xb7fcb163;
        rb_state0.var_5 = (rb_state0.var_5) - (rb_state0.var_4);
        rb_state0.var_6 = (rb_state0.var_6) - (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0xbd266bb9));
        rb_state0.var_7 = (rb_state0.var_7) - (((0xca2f5337) + (rb_state0.var_3)) ^ (0x4f88afb6));
        rb_state0.var_8 = (rb_state0.var_8) ^ (((((0xe051367e) + (0x94382ac4)) + (0xf3361339)) + (rb_state0.var_6)) + (rb_state0.var_5));
        rb_state0.var_9 = (rb_state0.var_9) + (((((0x1107e33c) - (rb_state0.var_8)) ^ (rb_state0.var_7)) + (rb_state0.var_5)) + (rb_state0.var_4));
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_9);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0x478c2b4c)) {
        if ((rb_state12.var_2) != (0x0)) {
            if (!((rb_state12.var_2) == (rb_state12.var_22))) {
                racebench_trigger(12);
            }
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
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0xb1a68f4a)) {
            rb_state3.var_2 = rb_state3.var_0;
        }
        if ((rb_state3.var_1) == (0xb1a68f4a)) {
            if (!((rb_state3.var_0) == (rb_state3.var_2))) {
                racebench_trigger(3);
            }
        }
        #endif
        next = NODE_ADD(curr, nodesize + size);
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_1) == (0xfdef6058)) {
            rb_state4.var_5 = 0x48ff5f10;
            rb_state4.var_6 = 0x56d26de3;
            rb_state4.var_7 = 0x8253c1cf;
            rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_5) : (0x8877684e));
            rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_6);
            rb_state4.var_10 = (rb_state4.var_10) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_7) : (0x1c67d092));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((0x40eb5f5f) ^ (0xc4b6172d)) - (rb_state4.var_8));
            rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_9) : (0xe912a0b1));
            rb_state4.var_13 = (rb_state4.var_13) + (((((0x8d49627b) + (rb_state4.var_8)) - (0xe072ba8e)) - (rb_state4.var_11)) ^ (rb_state4.var_10));
            rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_12) : (0xa018daf2));
            rb_state4.var_15 = (rb_state4.var_15) ^ (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_13) : (0x5a1f61d9));
            rb_state4.var_16 = (rb_state4.var_16) + (((0xc7bac28c) - (0xf946fef7)) ^ (rb_state4.var_14));
            rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_15) : (0xa74c4251));
            rb_state4.var_18 = (rb_state4.var_18) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_17) : (0x39ac36c1));
            rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_18);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x487d1b06)) {
            rb_state5.var_3 = 0x6bc400cd;
            rb_state5.var_4 = 0xcabf6e76;
            rb_state5.var_5 = 0xabd5db9e;
            rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_4);
            rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_3) : (0xd26f3eef));
            rb_state5.var_8 = (rb_state5.var_8) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_6) : (0xd2d53667));
            rb_state5.var_9 = (rb_state5.var_9) + (((0x779c44d0) ^ (rb_state5.var_7)) ^ (0xe50bf872));
            rb_state5.var_10 = (rb_state5.var_10) - (((0x1386598d) ^ (rb_state5.var_3)) ^ (rb_state5.var_8));
            rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_9) : (0x664fef0e));
            rb_state5.var_12 = (rb_state5.var_12) - (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_10) : (0xc29c87b3));
            rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_11) : (0x1e3cd51c));
            rb_state5.var_14 = (rb_state5.var_14) ^ (((((0xdb0b1b73) - (0xc945fed9)) ^ (rb_state5.var_7)) ^ (rb_state5.var_13)) + (rb_state5.var_12));
            rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x10e1ff1a)) {
            rb_state18.var_2 = rb_state18.var_0;
        }
        #endif
        next->size = curr->size - nodesize - size;
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_8) == (0xb888468)) {
            rb_state6.var_18 = (rb_state6.var_18) ^ (rb_state6.var_10);
            rb_state6.var_19 = (rb_state6.var_19) - ((0x7256df21) ^ (0xca789922));
            rb_state6.var_20 = (rb_state6.var_20) - (((0xce54b7b3) - (0x68f643fd)) + (rb_state6.var_12));
            rb_state6.var_21 = (rb_state6.var_21) ^ (((0x5276f279) + (rb_state6.var_13)) ^ (rb_state6.var_11));
            rb_state6.var_22 = (rb_state6.var_22) + (((0x17810e48) ^ (0x1aa98d3a)) ^ (rb_state6.var_18));
            rb_state6.var_23 = (rb_state6.var_23) ^ (((((0x97cdc903) - (rb_state6.var_16)) + (rb_state6.var_14)) - (rb_state6.var_19)) ^ (rb_state6.var_20));
            rb_state6.var_24 = (rb_state6.var_24) ^ (((0x88c7abbc) + (rb_state6.var_17)) - (rb_state6.var_21));
            rb_state6.var_25 = (rb_state6.var_25) + (((0x61cb26f2) ^ (rb_state6.var_18)) - (rb_state6.var_22));
            rb_state6.var_26 = (rb_state6.var_26) - (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_23) : (0x8a31457e));
            rb_state6.var_27 = (rb_state6.var_27) - (((0x9576f00) ^ (rb_state6.var_24)) ^ (rb_state6.var_20));
            rb_state6.var_28 = (rb_state6.var_28) + (rb_state6.var_25);
            rb_state6.var_29 = (rb_state6.var_29) ^ (rb_state6.var_26);
            rb_state6.var_30 = (rb_state6.var_30) - (((rb_state6.var_21) == (0x0)) ? (rb_state6.var_27) : (0x2dd330c8));
            rb_state6.var_31 = (rb_state6.var_31) + (((rb_state6.var_22) == (0x0)) ? (rb_state6.var_28) : (0x4094f18d));
            rb_state6.var_32 = (rb_state6.var_32) ^ (((((0xeb25db97) ^ (rb_state6.var_30)) - (0xc5b0570)) + (rb_state6.var_23)) ^ (rb_state6.var_29));
            rb_state6.var_33 = (rb_state6.var_33) - (((rb_state6.var_24) == (0x0)) ? (rb_state6.var_31) : (0xdd922595));
            rb_state6.var_34 = (rb_state6.var_34) + (((((0xb61a214) ^ (rb_state6.var_25)) ^ (0xef4443a5)) + (rb_state6.var_33)) - (rb_state6.var_32));
            rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_34);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x10e1ff1a)) {
            if (!((rb_state18.var_0) == (rb_state18.var_2))) {
                racebench_trigger(18);
            }
        }
        #endif
        next->next = curr->next;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
        #endif
        next->free = TRUE;
        next->cksm = CKSM;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - ((0x61dfda90) + (rb_state4.var_0));
        #endif
        curr->size = size;
    } else {
        next = curr->next;
    }

    if (!prev) {
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_37) == (0xfbf498ea)) {
            pthread_mutex_lock(&(rb_state7.lock_79));
            rb_state7.var_72 = (rb_state7.var_72) ^ (rb_state7.var_42);
            rb_state7.var_73 = (rb_state7.var_73) + ((0x43c5ced3) - (rb_state7.var_43));
            rb_state7.var_74 = (rb_state7.var_74) - (((0x7c7403fd) + (rb_state7.var_44)) + (rb_state7.var_40));
            rb_state7.var_75 = (rb_state7.var_75) - (((rb_state7.var_41) == (0xfbf498ea)) ? (rb_state7.var_72) : (0x2457990b));
            rb_state7.var_76 = (rb_state7.var_76) + (((rb_state7.var_45) == (0xb258d81a)) ? (rb_state7.var_73) : (0xe99c59d2));
            rb_state7.var_77 = (rb_state7.var_77) ^ (((((0xe2ba832d) ^ (0x2b7473a0)) - (rb_state7.var_74)) ^ (0x537ecae4)) + (rb_state7.var_75));
            rb_state7.var_78 = (rb_state7.var_78) + (((((0x4c00a47a) - (0x201167b6)) + (rb_state7.var_46)) + (rb_state7.var_77)) ^ (rb_state7.var_76));
            rb_state7.var_63 = (rb_state7.var_63) ^ (rb_state7.var_78);
            pthread_mutex_unlock(&(rb_state7.lock_79));
        }
        #endif
        gm->freelist = next;
    } else {
        prev->next = next;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x5025da92)) {
        if ((rb_state13.var_2) != (0x0)) {
            if (!((rb_state13.var_2) == (rb_state13.var_13))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    curr->next = NULL;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - ((0x3a2ed7b8) - (rb_state4.var_1));
    #endif
    curr->free = FALSE;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (560 < rb_input_size ? (uint32_t)rb_input[560] : 0x7675f67e);
    if ((rb_state9.var_0) == (0x20957ba1)) {
        pthread_mutex_lock(&(rb_state9.lock_20));
        rb_state9.var_3 = 0x70e73998;
        rb_state9.var_4 = (rb_state9.var_4) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_3) : (0xf651f20a));
        rb_state9.var_5 = (rb_state9.var_5) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_2) : (0xdf316ac));
        rb_state9.var_6 = (rb_state9.var_6) ^ (((((0xe2ff40e2) - (rb_state9.var_5)) + (0x52d83293)) - (rb_state9.var_1)) + (rb_state9.var_4));
        rb_state9.var_7 = (rb_state9.var_7) + (((0x7406fdb5) - (rb_state9.var_5)) + (rb_state9.var_6));
        rb_state9.var_8 = (rb_state9.var_8) - (rb_state9.var_6);
        rb_state9.var_9 = (rb_state9.var_9) ^ (((((0xeeee7f9f) + (rb_state9.var_7)) - (rb_state9.var_8)) ^ (rb_state9.var_7)) ^ (0xb7eed3fb));
        rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_9);
        rb_state9.var_1 = rb_state9.var_2;
        pthread_mutex_unlock(&(rb_state9.lock_20));
    }
    if ((rb_state9.var_0) == (0x20957ba1)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
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