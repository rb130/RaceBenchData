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

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x6993fcde)) {
        rb_state4.var_12 = (rb_state4.var_12) + (((((0x56537cc) ^ (rb_state4.var_8)) - (rb_state4.var_11)) ^ (rb_state4.var_7)) + (rb_state4.var_10));
        rb_state4.var_13 = (rb_state4.var_13) ^ (0x4f39ad64);
        rb_state4.var_14 = (rb_state4.var_14) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_9) : (0x71a46cc5));
        rb_state4.var_15 = (rb_state4.var_15) + ((0xad64923b) + (0x6294b6cf));
        rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_13) : (0x2ec233ab));
        rb_state4.var_17 = (rb_state4.var_17) ^ (rb_state4.var_14);
        rb_state4.var_18 = (rb_state4.var_18) - (((((0x1a2a5a00) - (rb_state4.var_15)) ^ (0xa5e1508a)) - (rb_state4.var_16)) + (0x5baaa8eb));
        rb_state4.var_19 = (rb_state4.var_19) - (((0x6843276a) ^ (rb_state4.var_17)) - (0x726db5df));
        rb_state4.var_20 = (rb_state4.var_20) + (((0x35cfd598) + (rb_state4.var_13)) - (rb_state4.var_18));
        rb_state4.var_21 = (rb_state4.var_21) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_19) : (0xca0f4e7d));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_20) : (0x9c55d06c));
        rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_22) : (0xb7c4895c));
        rb_state4.var_11 = (rb_state4.var_11) ^ (rb_state4.var_23);
        rb_state4.var_4 = rb_state4.var_11;
    }
    if ((rb_state4.var_0) == (0x6993fce8)) {
        pthread_mutex_lock(&(rb_state4.lock_38));
        rb_state4.var_25 = 0x6a9bf699;
        rb_state4.var_26 = 0xcfe45c27;
        rb_state4.var_27 = (rb_state4.var_27) - (360 < rb_input_size ? (uint32_t)rb_input[360] : 0xa20799d7);
        rb_state4.var_28 = (rb_state4.var_28) - (((0xdf924912) ^ (rb_state4.var_26)) + (rb_state4.var_17));
        rb_state4.var_29 = (rb_state4.var_29) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_25) : (0xc162fabf));
        rb_state4.var_30 = (rb_state4.var_30) + (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_16) : (0xa28edc57));
        rb_state4.var_31 = (rb_state4.var_31) - (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_27) : (0xcc571c1a));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((0x56b14c49) + (rb_state4.var_21)) + (rb_state4.var_28));
        rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_30) : (0x6ee7b9ac));
        rb_state4.var_34 = (rb_state4.var_34) ^ (rb_state4.var_31);
        rb_state4.var_35 = (rb_state4.var_35) + (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_33) : (0x1c482d48));
        rb_state4.var_36 = (rb_state4.var_36) - (((0x26f98eaf) ^ (rb_state4.var_34)) ^ (rb_state4.var_22));
        rb_state4.var_37 = (rb_state4.var_37) ^ (((((0x4878482a) + (rb_state4.var_23)) + (rb_state4.var_36)) - (rb_state4.var_35)) + (0x3211cdec));
        rb_state4.var_24 = (rb_state4.var_24) ^ (rb_state4.var_37);
        rb_state4.var_4 = rb_state4.var_24;
        pthread_mutex_unlock(&(rb_state4.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0xebe1cc68));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0xdee499e1)) {
        rb_state9.var_3 = rb_state9.var_0;
    }
    if ((rb_state9.var_2) == (0xdee499e1)) {
        if (!((rb_state9.var_0) == (rb_state9.var_3))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) - (((rb_state12.var_1) == (0xc7490ffc)) ? (rb_state12.var_3) : (0x5b4cb260));
    if ((rb_state12.var_1) == (0xc7490fd2)) {
        pthread_mutex_lock(&(rb_state12.lock_25));
        rb_state12.var_19 = 0x7bee078e;
        rb_state12.var_20 = 0x21c7bbda;
        rb_state12.var_21 = (rb_state12.var_21) - (((((0x54c1852c) - (rb_state12.var_19)) - (rb_state12.var_13)) + (rb_state12.var_12)) + (0x539e8e8b));
        rb_state12.var_22 = (rb_state12.var_22) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_20) : (0x885faef4));
        rb_state12.var_23 = (rb_state12.var_23) + (((0xb36eb12d) + (0x6faeb318)) + (rb_state12.var_21));
        rb_state12.var_24 = (rb_state12.var_24) + (((((0xd910ebbf) ^ (rb_state12.var_22)) ^ (0xd439d1fe)) - (rb_state12.var_23)) ^ (0x54119238));
        rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_24);
        pthread_mutex_unlock(&(rb_state12.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_3);
    #endif
    prev = NULL;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x1893145e)) {
        pthread_mutex_lock(&(rb_state1.lock_16));
        rb_state1.var_5 = 0x9db987ce;
        rb_state1.var_6 = (rb_state1.var_6) - (96 < rb_input_size ? (uint32_t)rb_input[96] : 0x33233df4);
        rb_state1.var_7 = (rb_state1.var_7) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0xa6b26562));
        rb_state1.var_8 = (rb_state1.var_8) - (rb_state1.var_5);
        rb_state1.var_9 = (rb_state1.var_9) ^ (((((0xcba00f44) - (rb_state1.var_6)) - (rb_state1.var_7)) - (rb_state1.var_6)) - (rb_state1.var_5));
        rb_state1.var_10 = (rb_state1.var_10) + (((0x6092a08a) ^ (rb_state1.var_8)) - (rb_state1.var_7));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_9);
        rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0xdc4ca080));
        rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_12);
        rb_state1.var_3 = rb_state1.var_4;
        pthread_mutex_unlock(&(rb_state1.lock_16));
    }
    if ((rb_state1.var_2) == (0x1893145e)) {
        pthread_mutex_lock(&(rb_state1.lock_16));
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            racebench_trigger(1);
        }
        pthread_mutex_unlock(&(rb_state1.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_1) == (0x779ed25a)) ? (rb_state11.var_0) : (0x36955f71));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x476e38)) {
        rb_state14.var_5 = 0x1;
    }
    if ((rb_state14.var_0) == (0x476e38)) {
        pthread_mutex_lock(&(rb_state14.lock_15));
        rb_state14.var_6 = 0xbe86e9c4;
        rb_state14.var_7 = (rb_state14.var_7) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_4) : (0xc2feac47));
        rb_state14.var_8 = (rb_state14.var_8) - (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_3) : (0xea2285d9));
        rb_state14.var_9 = (rb_state14.var_9) ^ (474 < rb_input_size ? (uint32_t)rb_input[474] : 0x6ed38e10);
        rb_state14.var_10 = (rb_state14.var_10) + (((0x642657cb) ^ (rb_state14.var_7)) ^ (0xa6f3593));
        rb_state14.var_11 = (rb_state14.var_11) + (((0x346fbabb) - (0xc27d16c7)) + (rb_state14.var_8));
        rb_state14.var_12 = (rb_state14.var_12) ^ (((0x9f6d69c1) + (rb_state14.var_6)) + (rb_state14.var_9));
        rb_state14.var_13 = (rb_state14.var_13) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_11) : (0x1c78011a));
        rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_13) : (0x53b9b22c));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_14);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_15));
    }
    rb_state14.var_11 = (rb_state14.var_11) + (0x52c64075);
    #endif
    curr = gm->freelist;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (0x70e92c07);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (0x9562d30e);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_1) == (0x8fa492ce)) ? (rb_state8.var_0) : (0xb5d61c9f));
    rb_state8.var_2 = (rb_state8.var_2) ^ (0x4362be1e);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_7 = (rb_state14.var_7) ^ ((0x169f11e0) ^ (rb_state14.var_8));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + ((0x313d8df4) ^ (0x31330fe));
    rb_state17.var_2 = (rb_state17.var_2) ^ (0xd9ed9634);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xdccb39fb)) {
        rb_state18.var_5 = 0x1267cbfd;
        rb_state18.var_6 = (rb_state18.var_6) - (((0xf3eb25e4) + (rb_state18.var_4)) ^ (rb_state18.var_5));
        rb_state18.var_7 = (rb_state18.var_7) ^ (((0xab1daba8) - (rb_state18.var_5)) - (0x5aaa3f36));
        rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_6) : (0xe99b538e));
        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0x7ed500f9));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_9);
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
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (50 < rb_input_size ? (uint32_t)rb_input[50] : 0xb2674e8a);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_13) == (0x71a7fa7e)) {
            if ((rb_state14.var_17) != (0x0)) {
                if (!((rb_state14.var_17) == (rb_state14.var_37))) {
                    racebench_trigger(14);
                }
            }
        }
        #endif
        next = NODE_ADD(curr, nodesize + size);
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + ((0x7d84ea0d) ^ (0xc7f44279));
        rb_state5.var_0 = (rb_state5.var_0) + (0x66d41347);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_2);
        #endif
        next->size = curr->size - nodesize - size;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_3 = (rb_state3.var_3) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0x6e5c1f6b));
        if ((rb_state3.var_3) == (0x0)) {
            rb_state3.var_4 = rb_state3.var_1;
        }
        if ((rb_state3.var_3) == (0x0)) {
            if (!((rb_state3.var_1) == (rb_state3.var_4))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x2144bbbb)) ? (rb_state5.var_0) : (0x2d31c222));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (((rb_state17.var_0) == (0xe33c2821)) ? (rb_state17.var_1) : (0x11f6ca55));
        #endif
        next->next = curr->next;
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_3) == (0x20)) {
            pthread_mutex_lock(&(rb_state7.lock_15));
            rb_state7.var_6 = 0x2344336b;
            rb_state7.var_7 = 0x3396edd4;
            rb_state7.var_8 = (rb_state7.var_8) ^ (309 < rb_input_size ? (uint32_t)rb_input[309] : 0x55f9fe25);
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_4) : (0x46bf60d8));
            rb_state7.var_10 = (rb_state7.var_10) - (((((0x42fcc9f7) ^ (rb_state7.var_8)) - (rb_state7.var_5)) ^ (rb_state7.var_6)) + (0x5fcd5dca));
            rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_9) : (0x1d2d84f2));
            rb_state7.var_12 = (rb_state7.var_12) ^ (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0xb198fed7));
            rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_12);
            rb_state7.var_4 = rb_state7.var_5;
            pthread_mutex_unlock(&(rb_state7.lock_15));
        }
        if ((rb_state7.var_3) == (0x20)) {
            if (!((rb_state7.var_4) == (rb_state7.var_5))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (118 < rb_input_size ? (uint32_t)rb_input[118] : 0xd0394544);
        rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_2);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) + (0x42e5e082);
        #endif
        next->free = TRUE;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - (((rb_state3.var_1) == (0x904dfe08)) ? (rb_state3.var_0) : (0xe27a1972));
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_2) == (0x1c3a5ed2)) {
            rb_state11.var_4 = rb_state11.var_3;
        }
        if ((rb_state11.var_2) == (0x1c3a5ed2)) {
            if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) - (((rb_state19.var_3) == (0x42e5e082)) ? (rb_state19.var_2) : (0x67be9ff1));
        #endif
        next->cksm = CKSM;
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_0);
        if ((rb_state17.var_2) == (0xd7aba11e)) {
            pthread_mutex_lock(&(rb_state17.lock_28));
            rb_state17.var_5 = 0x6457b9fb;
            rb_state17.var_6 = 0xc67554d2;
            rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_4) : (0xa9a51782));
            rb_state17.var_8 = (rb_state17.var_8) - (((((0x499f79b4) ^ (rb_state17.var_6)) + (rb_state17.var_6)) + (rb_state17.var_3)) ^ (0x81477012));
            rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_5);
            rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0xf89c5948));
            rb_state17.var_11 = (rb_state17.var_11) ^ (((((0xf2066169) ^ (rb_state17.var_10)) + (rb_state17.var_7)) + (0x7d6c702e)) + (rb_state17.var_9));
            rb_state17.var_4 = (rb_state17.var_4) - (rb_state17.var_11);
            rb_state17.var_3 = rb_state17.var_4;
            pthread_mutex_unlock(&(rb_state17.lock_28));
        }
        if ((rb_state17.var_2) == (0x7cf56217)) {
            if (!((rb_state17.var_3) == (rb_state17.var_4))) {
                racebench_trigger(17);
            }
        }
        #endif
        curr->size = size;
    } else {
        next = curr->next;
    }

    if (!prev) {
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + (200 < rb_input_size ? (uint32_t)rb_input[200] : 0xb3987127);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_9 = (rb_state14.var_9) + ((0xf3cc2fa) ^ (rb_state14.var_10));
        #endif
        gm->freelist = next;
    } else {
        prev->next = next;
    }

    { pthread_mutex_unlock(&(gm->memlock)); }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (407 < rb_input_size ? (uint32_t)rb_input[407] : 0xf08b0a96);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (0x5963a094);
    #endif
    curr->next = NULL;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (210 < rb_input_size ? (uint32_t)rb_input[210] : 0xf331109);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0xecf6f675)) {
        rb_state16.var_5 = 0x28221eb6;
        rb_state16.var_6 = 0x1e113a94;
        rb_state16.var_7 = (rb_state16.var_7) + (0xa2576788);
        rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_6) == (0x1e113a94)) ? (rb_state16.var_5) : (0x44b1682a));
        rb_state16.var_9 = (rb_state16.var_9) - (((0x7a0daae3) + (rb_state16.var_4)) + (0xc019f711));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((((0x4fcf3f0c) + (rb_state16.var_5)) ^ (0x9a962175)) + (rb_state16.var_6)) + (rb_state16.var_7));
        rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_7);
        rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_8) == (0x28221eb6)) ? (rb_state16.var_9) : (0xc96c0c11));
        rb_state16.var_13 = (rb_state16.var_13) + (((((0xd978fc1d) + (rb_state16.var_11)) - (rb_state16.var_9)) + (rb_state16.var_8)) + (rb_state16.var_10));
        rb_state16.var_14 = (rb_state16.var_14) ^ (((((0xdc6ba883) ^ (0x8a6111c2)) ^ (rb_state16.var_10)) - (rb_state16.var_13)) - (rb_state16.var_12));
        rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_14);
    }
    #endif
    curr->free = FALSE;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x1893145e)) {
        rb_state1.var_13 = 0x5d896dc6;
        rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_9) : (0x6ceee6c7));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_14) : (0x7111a481));
        rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) ^ ((0xaa79cd03) + (0x7e373bed));
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