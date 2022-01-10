#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "matrix.h"
#include <math.h>

extern struct GlobalMemory *Global;
extern BMatrix LB;
extern int P;
extern int BS;
extern int *node;
struct BlockList ***AllBlocks, ***DiagBlock;
int **ToReceive, **NReceived;

PreProcessFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_2 = rb_state6.var_1;
    }
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_13));
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x90a66295)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0x90a66295)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        rb_state17.var_3 = 0x32bafaea;
        rb_state17.var_4 = 0xf949b8b6;
        rb_state17.var_5 = 0x390843c1;
        rb_state17.var_6 = (rb_state17.var_6) + (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_5) : (0xc80adbde));
        rb_state17.var_7 = (rb_state17.var_7) - (rb_state17.var_3);
        rb_state17.var_8 = (rb_state17.var_8) - (((0x9f34fbab) ^ (0x9e4d64e0)) - (rb_state17.var_6));
        rb_state17.var_9 = (rb_state17.var_9) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0xf36868a6));
        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_9);
        rb_state17.var_1 = rb_state17.var_2;
    }
    if ((rb_state17.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_20));
        rb_state17.var_11 = 0x2baef774;
        rb_state17.var_12 = 0x2ae7a48f;
        rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_11);
        rb_state17.var_14 = (rb_state17.var_14) + (((0xa326a197) - (rb_state17.var_2)) - (0x531b6d29));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_13) : (0xe3e2a8aa));
        rb_state17.var_16 = (rb_state17.var_16) + (((0x30772fa8) ^ (rb_state17.var_14)) - (0xec7051d6));
        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_15) : (0xa4e96ab0));
        rb_state17.var_18 = (rb_state17.var_18) ^ (rb_state17.var_16);
        rb_state17.var_19 = (rb_state17.var_19) - (((((0x808259a1) ^ (rb_state17.var_4)) ^ (rb_state17.var_17)) + (0x4a1d2331)) - (rb_state17.var_18));
        rb_state17.var_10 = (rb_state17.var_10) ^ (rb_state17.var_19);
        rb_state17.var_1 = rb_state17.var_10;
        pthread_mutex_unlock(&(rb_state17.lock_20));
    }
    #endif
    InitRemainingFO(MyNum, lc);
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (0x7b3efe54);
    if ((rb_state15.var_0) == (0x130406b0)) {
        pthread_mutex_lock(&(rb_state15.lock_28));
        rb_state15.var_12 = 0x99f5941e;
        rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_9) : (0x829ec03a));
        rb_state15.var_14 = (rb_state15.var_14) - ((0x4a927d63) ^ (rb_state15.var_11));
        rb_state15.var_15 = (rb_state15.var_15) + (rb_state15.var_12);
        rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_7) : (0xcf9a6fcd));
        rb_state15.var_17 = (rb_state15.var_17) + (((0x4db8ddf8) + (0xce90edf8)) - (rb_state15.var_8));
        rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_13);
        rb_state15.var_19 = (rb_state15.var_19) - (((((0x6471eb37) - (rb_state15.var_14)) ^ (rb_state15.var_13)) - (rb_state15.var_15)) - (rb_state15.var_14));
        rb_state15.var_20 = (rb_state15.var_20) ^ (((((0xbae12e76) ^ (rb_state15.var_17)) + (rb_state15.var_16)) + (rb_state15.var_15)) - (0x74c6a64b));
        rb_state15.var_21 = (rb_state15.var_21) - (rb_state15.var_18);
        rb_state15.var_22 = (rb_state15.var_22) - (((0xd5103bc7) ^ (0x7f278285)) + (rb_state15.var_19));
        rb_state15.var_23 = (rb_state15.var_23) + (((0x693ebf0a) + (rb_state15.var_16)) + (rb_state15.var_20));
        rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_21) : (0x317ecf23));
        rb_state15.var_25 = (rb_state15.var_25) ^ (((rb_state15.var_22) == (0x0)) ? (rb_state15.var_23) : (0x2ef49bfc));
        rb_state15.var_26 = (rb_state15.var_26) ^ (rb_state15.var_24);
        rb_state15.var_27 = (rb_state15.var_27) ^ (((((0xb5e5d4bd) ^ (rb_state15.var_25)) - (rb_state15.var_26)) - (rb_state15.var_18)) - (0xbf31931d));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_27);
        pthread_mutex_unlock(&(rb_state15.lock_28));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x0)) {
        rb_state19.var_20 = 0x6bdb3537;
        rb_state19.var_21 = 0xf8726a2b;
        rb_state19.var_22 = 0xa059bf59;
        rb_state19.var_23 = (rb_state19.var_23) - (rb_state19.var_22);
        rb_state19.var_24 = (rb_state19.var_24) - (rb_state19.var_21);
        rb_state19.var_25 = (rb_state19.var_25) ^ (((((0xffd3cce6) ^ (rb_state19.var_20)) + (rb_state19.var_12)) - (rb_state19.var_23)) + (0xac6ed0d0));
        rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_24) : (0xb4549fe7));
        rb_state19.var_27 = (rb_state19.var_27) - (((((0xc60e3c55) ^ (0x382a8d96)) ^ (rb_state19.var_26)) ^ (rb_state19.var_25)) + (0x9590c9cb));
        rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_27);
    }
    #endif
    InitReceivedFO(MyNum, lc);
}

PreAllocate1FO() {
    int i;
    extern int P;

    AllBlocks = (struct BlockList ***)MyMalloc(P * sizeof(struct BlockList **), DISTRIBUTED);
    DiagBlock = (struct BlockList ***)MyMalloc(P * sizeof(struct BlockList **), DISTRIBUTED);
    ToReceive = (int **)MyMalloc(P * sizeof(int *), DISTRIBUTED);
    NReceived = (int **)MyMalloc(P * sizeof(int *), DISTRIBUTED);
    for (i = 0; i < P; i++) {
        ToReceive[i] = (int *)MyMalloc(LB.n_partitions * sizeof(int), i);
        NReceived[i] = (int *)MyMalloc(LB.n_partitions * sizeof(int), i);
    }
}

PreAllocateFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i, j, stor_size, root, update_size;

    lc->link = (int *)MyMalloc((LB.n + 1) * sizeof(int), MyNum);
    lc->first = (int *)MyMalloc(LB.n * sizeof(int), MyNum);

    for (i = 0; i < LB.n; i++) {
        lc->first[i] = lc->link[i] = 0;
    }

    lc->max_panel = 0;
    for (i = 0; i < LB.n; i += node[i]) {
        if (LB.domain[i]) {
            if (LB.col[i + node[i]] - LB.col[i] > lc->max_panel) {
                lc->max_panel = LB.col[i + node[i]] - LB.col[i];
            }
            j = LB.col[i + 1] - LB.col[i] - node[i];
            if (j * (j + 1) / 2 > lc->max_panel) {
                lc->max_panel = j * (j + 1) / 2;
            }
        }
    }

    if (LB.max_partition > BS) {
        stor_size = BS * BS;
        lc->blktmp = (double *)MyMalloc(stor_size * sizeof(double), MyNum);
        for (i = 0; i < stor_size; i++) {
            lc->blktmp[i] = 0.0;
        }
    } else {
        lc->blktmp = NULL;
    }
    stor_size = LB.max_partition * LB.max_partition;
    lc->updatetmp = (double *)MyMalloc(stor_size * sizeof(double), MyNum);
    for (i = 0; i < stor_size; i++) {
        lc->updatetmp[i] = 0.0;
    }
    lc->relative = (int *)MyMalloc(LB.max_partition * sizeof(int), MyNum);

    AllBlocks[MyNum] = (struct BlockList **)MyMalloc(LB.n_partitions * sizeof(struct BlockList *), MyNum);
    DiagBlock[MyNum] = (struct BlockList **)MyMalloc(LB.n_partitions * sizeof(struct BlockList *), MyNum);
    for (i = 0; i < LB.n_partitions; i++) {
        AllBlocks[MyNum][i] = DiagBlock[MyNum][i] = NULL;
    }

    for (i = LB.proc_domains[MyNum]; i < LB.proc_domains[MyNum + 1]; i++) {
        root = LB.domains[i];
        update_size = LB.col[root + 1] - LB.col[root] - 1;
        update_size = update_size * (update_size + 1) / 2;
        if (update_size) {
            LB.proc_domain_storage[i] = (double *)MyMalloc(update_size * sizeof(double), MyNum);
            for (j = 0; j < update_size; j++) {
                LB.proc_domain_storage[i][j] = 0.0;
            }
        } else {
            LB.proc_domain_storage[i] = NULL;
        }
    }
}

BNumericSolveFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i;

    for (i = 0; i < LB.n; i++) {
        lc->link[i] = -1;
    }

    lc->storage = (double *)MyMalloc(lc->max_panel * sizeof(double), MyNum);
    for (i = 0; i < lc->max_panel; i++) {
        lc->storage[i] = 0.0;
    }

    for (i = LB.proc_domains[MyNum]; i < LB.proc_domains[MyNum + 1]; i++) {
        FactorLLDomain(i, MyNum, lc);
    }
    MyFree(lc->storage);
    lc->storage = NULL;
    DriveParallelFO(MyNum, lc);
}

DriveParallelFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int some, j;
    extern int *node, *firstchild;

    some = 0;
    for (j = 0; j < LB.n; j += node[j]) {
        if (!LB.domain[j]) {
            some = 1;
            if (BLOCK(LB.col[j])->owner == MyNum && BLOCK(LB.col[j])->remaining == 0) {
                BlockReadyFO(LB.col[j], MyNum, lc);
            }
        }
    }

    if (some) {
        while (HandleTaskFO(MyNum, lc)) {
            ;
        }
    }

    if (TaskWaiting(MyNum, lc)) {
        printf("**** Termination error ***\n");
    }
}

HandleTaskFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int desti, destj, src;
    struct Update *update;

    GetBlock(&desti, &destj, &src, &update, MyNum, lc);
    if (desti == -1) {
        return (0);
    } else if (update == (struct Update *)-19) {
        HandleUpdate2FO(src, desti, destj, MyNum, lc);
    } else if (update != NULL) {
    } else {
        if (BLOCKROW(src) == BLOCKCOL(src)) {
            DiagReceived(src, MyNum, lc);
        } else {
            BlockReceived(src, MyNum, lc);
        }

        NReceived[MyNum][LB.renumbering[BLOCKCOL(src)]]--;
        if (NReceived[MyNum][LB.renumbering[BLOCKCOL(src)]] == 0) {
            FreeColumnListFO(MyNum, LB.renumbering[BLOCKCOL(src)]);
        }
    }

    return (1);
}

DiagReceived(diag, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i, column;
    struct BlockList *diagbl, *CopyOneBlock();

    diagbl = CopyOneBlock(diag, MyNum, lc);
    column = LB.renumbering[BLOCKCOL(diag)];
    diagbl->next = NULL;
    DiagBlock[MyNum][column] = diagbl;

    column = BLOCKCOL(diag);
    for (i = LB.col[column] + 1; i < LB.col[column + 1]; i++) {
        if (BLOCK(i)->owner == MyNum && BLOCK(i)->remaining == 0) {
            BDiv(diag, i, diagbl->length, BLOCK(i)->length, diagbl->nz, BLOCK(i)->nz, MyNum, lc);
            BlockDoneFO(i, MyNum, lc);
        }
    }

    if (BLOCKCOL(diag) + LB.partition_size[BLOCKCOL(diag)] == LB.n && OWNER(diag) == MyNum) {
        for (i = 0; i < P; i++) {
            Send(-1, -1, -1, -1, (struct Update *)NULL, i, MyNum, lc);
        }
    }
}

BlockReceived(block, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int column;
    struct BlockList *thisbl, *bl, *CopyOneBlock();

    column = LB.renumbering[BLOCKCOL(block)];

    thisbl = CopyOneBlock(block, MyNum, lc);
    thisbl->next = AllBlocks[MyNum][column];
    AllBlocks[MyNum][column] = thisbl;

    bl = AllBlocks[MyNum][column]->next;
    while (bl) {
        if (block < bl->theBlock) {
            PerformUpdate(thisbl, bl, MyNum, lc);
        } else {
            PerformUpdate(bl, thisbl, MyNum, lc);
        }

        bl = bl->next;
    }

    PerformUpdate(thisbl, thisbl, MyNum, lc);
}

struct BlockList *CopyOneBlock(block, MyNum, lc)
int MyNum;
struct LocalCopies *lc;
{
    struct BlockList *bl;
    int i, size, num_nz, num_ind, copy_across;
    extern int solution_method;

    bl = (struct BlockList *)MyMalloc(sizeof(struct BlockList), MyNum);

    bl->theBlock = block;
    bl->row = BLOCKROW(block);
    bl->col = BLOCKCOL(block);
    bl->length = BLOCK(block)->length;

    bl->structure = BLOCK(block)->structure;
    bl->nz = BLOCK(block)->nz;

    return (bl);
}

FreeColumnListFO(p, col) {
    struct BlockList *bl;

    while (AllBlocks[p][col]) {
        bl = AllBlocks[p][col];
        AllBlocks[p][col] = bl->next;
        MyFree(bl);
    }
    while (DiagBlock[p][col]) {
        bl = DiagBlock[p][col];
        DiagBlock[p][col] = bl->next;
        MyFree(bl);
    }
}

DecrementRemaining(dest_block, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    BLOCK(dest_block)->remaining--;
    if (BLOCK(dest_block)->remaining == 0) {
        BlockReadyFO(dest_block, MyNum, lc);
    } else if (BLOCK(dest_block)->remaining == -1) {
        printf("*** Error rem ***\n");
    }
}

PerformUpdate(above_bl, below_bl, MyNum, lc) struct BlockList *above_bl, *below_bl;
int MyNum;
struct LocalCopies *lc;
{
    int above, below;
    int desti, destj, dest_block, is_diag;
    int *relative_i, *relative_j;
    double *destination;

    above = above_bl->theBlock;
    below = below_bl->theBlock;

    desti = below_bl->row;
    destj = above_bl->row;

    is_diag = (desti == destj);

    dest_block = FindBlock(desti, destj);
    if (dest_block == -1) {
        printf("Couldn't find %d,%d\n", desti, destj);
    } else if (BLOCK(dest_block)->owner != MyNum) {
        return;
    }

    if (is_diag) {

        if (!below_bl->structure) {
            destination = BLOCK(dest_block)->nz;
        } else {
            destination = lc->updatetmp;
        }

        BLMod(below, below_bl->length, LB.partition_size[below_bl->col], below_bl->nz, destination, MyNum, lc);

        if (destination == lc->updatetmp) {

            ScatterUpdateFO(below_bl->length, below_bl->structure, below_bl->length, below_bl->structure, BLOCK(dest_block)->length, lc->updatetmp, BLOCK(dest_block)->nz);
        }
    }

    else {

        if (below_bl->length == BLOCK(dest_block)->length) {
            relative_i = NULL;
        } else if (!BLOCK(dest_block)->structure) {
            relative_i = below_bl->structure;
        } else {
            FindRelativeIndices(below_bl->structure, below_bl->length, BLOCK(dest_block)->structure, BLOCK(dest_block)->length, lc->relative);
            relative_i = lc->relative;
        }

        if (above_bl->structure) {
            relative_j = above_bl->structure;
        } else {
            relative_j = NULL;
        }

        if (!relative_i && !relative_j) {
            destination = BLOCK(dest_block)->nz;
        } else {
            destination = lc->updatetmp;
        }

        BMod(above, below, above_bl->length, LB.partition_size[above_bl->col], below_bl->length, above_bl->nz, below_bl->nz, destination, MyNum, lc);

        if (destination == lc->updatetmp) {

            ScatterUpdateFO(below_bl->length, relative_i, above_bl->length, relative_j, BLOCK(dest_block)->length, lc->updatetmp, BLOCK(dest_block)->nz);
        }
    }

    DecrementRemaining(dest_block, MyNum, lc);
}

DistributeUpdateFO(which_domain, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int bi, bj, desti, destj, dest_block;

    for (bi = LB.col[LB.n + which_domain]; bi < LB.col[LB.n + which_domain + 1]; bi++) {
        for (bj = LB.col[LB.n + which_domain]; bj <= bi; bj++) {
            desti = BLOCKROW(bi);
            destj = BLOCKROW(bj);

            dest_block = FindBlock(desti, destj);
            Send(which_domain, dest_block, bi, bj, (struct Update *)-19, OWNER(dest_block), MyNum, lc);
        }
    }
}

HandleUpdate2FO(which_domain, bli, blj, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int dest_block, desti, destj;
    int *relative_i, *relative_j;
    int stride;
    double *update;

    desti = BLOCKROW(bli);
    destj = BLOCKROW(blj);
    dest_block = FindBlock(desti, destj);

    if (dest_block == -1) {
        printf("Couldn't find %d,%d\n", desti, destj);
    } else if (BLOCK(dest_block)->owner != MyNum) {
        printf("Sent to wrong PE\n", desti, destj);
    }

    FindBlockUpdate(which_domain, bli, blj, &update, &stride, MyNum, lc);

    if (BLOCK(bli)->structure && BLOCK(dest_block)->structure) {
        if (BLOCK(bli)->length != BLOCK(dest_block)->length) {
            FindRelativeIndices(BLOCK(bli)->structure, BLOCK(bli)->length, BLOCK(dest_block)->structure, BLOCK(dest_block)->length, lc->relative);
            relative_i = lc->relative;
        } else {
            relative_i = NULL;
        }
    } else if (BLOCK(bli)->structure) {
        relative_i = BLOCK(bli)->structure;
    } else {
        relative_i = NULL;
    }

    if (BLOCK(blj)->structure) {
        relative_j = BLOCK(blj)->structure;
    } else {
        relative_j = NULL;
    }

    ScatterUpdateFO2(BLOCK(bli)->length, relative_i, BLOCK(blj)->length, relative_j, stride, BLOCK(dest_block)->length, update, BLOCK(dest_block)->nz);

    DecrementRemaining(dest_block, MyNum, lc);
}

FindRelativeIndices(src_structure, src_len, dest_structure, dest_len, relative) int *src_structure, src_len, *dest_structure, dest_len, *relative;
{
    int srci, desti;
    int *leftRow, *rightRow, *last;

    leftRow = src_structure;
    rightRow = dest_structure;
    last = &src_structure[src_len];

    srci = desti = 0;
    while (leftRow != last) {
        while (*rightRow != *leftRow) {
            rightRow++;
            desti++;
        }
        relative[srci] = desti;
        leftRow++;
        rightRow++;
        srci++;
        desti++;
    }
}

BlockReadyFO(block, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int column;
    struct BlockList *diagbl;

    if (BLOCKROW(block) == BLOCKCOL(block)) {
        BFac(block, MyNum, lc);
        BlockDoneFO(block, MyNum, lc);
    } else {
        column = LB.renumbering[BLOCKCOL(block)];
        if (DiagBlock[MyNum][column]) {
            diagbl = DiagBlock[MyNum][column];
            BDiv(LB.col[BLOCKCOL(block)], block, diagbl->length, BLOCK(block)->length, diagbl->nz, BLOCK(block)->nz, MyNum, lc);
            BlockDoneFO(block, MyNum, lc);
        }
    }
}

BlockDoneFO(block, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i;
    int P_row, P_col;
    extern int P_dimi, P_dimj;
    extern int scatter_decomposition;

    if (scatter_decomposition) {
        P_row = LB.mapI[LB.renumbering[BLOCKROW(block)]] % P_dimi;
        P_col = LB.mapJ[LB.renumbering[BLOCKROW(block)]] % P_dimj;

        for (i = 0; i < P_dimj; i++) {
            Send(block, block, 0, 0, (struct Update *)NULL, P_row + i * P_dimi, MyNum, lc);
        }

        for (i = 0; i < P_dimi; i++) {
            if (i != P_row) {
                Send(block, block, 0, 0, (struct Update *)NULL, i + P_col * P_dimi, MyNum, lc);
            }
        }
    } else {
        for (i = 0; i < P; i++) {
            Send(block, block, 0, 0, (struct Update *)NULL, i, MyNum, lc);
        }
    }
}

CheckRemaining() {
    int i, j, bogus = 0;

    for (j = 0; j < LB.n; j += LB.partition_size[j]) {
        if (!LB.domain[j]) {
            for (i = LB.col[j]; i < LB.col[j + 1]; i++) {
                if (BLOCK(i)->remaining) {
                    bogus = 1;
                }
            }
        }
    }

    if (bogus) {
        printf("Bad remaining\n");
    }
}

CheckReceived() {
    int p, i, bogus = 0;

    for (p = 0; p < P; p++) {
        for (i = 0; i < LB.n_partitions; i++) {
            if (AllBlocks[p][i]) {
                bogus = 1;
            }
            if (DiagBlock[p][i]) {
                bogus = 1;
            }
        }
    }
    if (bogus) {
        printf("Bad received\n");
    }
}

ComputeRemainingFO() {
    int i, j, k;
    int desti, destj, dest_block;

    for (j = 0; j < LB.n; j++) {
        if (!LB.domain[j]) {
            for (i = LB.col[j]; i < LB.col[j + 1]; i++) {
                BLOCK(i)->nmod = 0;
            }
        }
    }

    for (k = 0; k < LB.proc_domains[P]; k++) {
        for (i = LB.col[LB.n + k]; i < LB.col[LB.n + k + 1]; i++) {
            for (j = LB.col[LB.n + k]; j < i; j++) {
                destj = BLOCKROW(j);
                desti = BLOCKROW(i);
                dest_block = FindBlock(desti, destj);
                BLOCK(dest_block)->nmod++;
            }
            desti = BLOCKROW(i);
            dest_block = FindBlock(desti, desti);
            BLOCK(dest_block)->nmod++;
        }
    }

    for (k = 0; k < LB.n; k++) {
        if (!LB.domain[k]) {
            for (i = LB.col[k] + 1; i < LB.col[k + 1]; i++) {
                for (j = LB.col[k] + 1; j < i; j++) {
                    destj = BLOCKROW(j);
                    desti = BLOCKROW(i);
                    dest_block = FindBlock(desti, destj);
                    BLOCK(dest_block)->nmod++;
                }
                desti = BLOCKROW(i);
                dest_block = FindBlock(desti, desti);
                BLOCK(dest_block)->nmod++;
            }
        }
    }
}

InitRemainingFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i, k;

    for (k = 0; k < LB.n; k++) {
        if (!LB.domain[k]) {
            for (i = LB.col[k]; i < LB.col[k + 1]; i++) {
                if (MyNum == -1 || BLOCK(i)->owner == MyNum) {
                    BLOCK(i)->remaining = BLOCK(i)->nmod;
                }
            }
        }
    }
}

ComputeReceivedFO() {
    int p, i, j, k, block;
    int P_row, P_col, destp;
    extern int scatter_decomposition, P_dimi, P_dimj;

    for (p = 0; p < P; p++) {
        for (i = 0; i < LB.n_partitions; i++) {
            ToReceive[p][i] = 0;
        }
    }

    for (k = 0; k < LB.n; k += LB.partition_size[k]) {
        if (!LB.domain[k]) {
            for (block = LB.col[k]; block < LB.col[k + 1]; block++) {

                if (scatter_decomposition) {
                    P_row = LB.mapI[LB.renumbering[BLOCKROW(block)]] % P_dimi;
                    P_col = LB.mapJ[LB.renumbering[BLOCKROW(block)]] % P_dimj;

                    for (i = 0; i < P_dimj; i++) {
                        destp = P_row + i * P_dimi;
                        ToReceive[destp][LB.renumbering[BLOCKCOL(block)]]++;
                    }

                    for (i = 0; i < P_dimi; i++) {
                        if (i != P_row) {
                            destp = i + P_col * P_dimi;
                            ToReceive[destp][LB.renumbering[BLOCKCOL(block)]]++;
                        }
                    }

                } else {
                    for (i = 0; i < P; i++) {
                        ToReceive[i][LB.renumbering[BLOCKCOL(block)]]++;
                    }
                }
            }
        }
    }
}

InitReceivedFO(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int i, p;

    for (i = 0; i < LB.n_partitions; i++) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (0x3b217586);
        if ((rb_state12.var_0) == (0xf97f424c)) {
            pthread_mutex_lock(&(rb_state12.lock_25));
            rb_state12.var_14 = 0xc393d4b1;
            rb_state12.var_15 = (rb_state12.var_15) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_14) : (0x778b97));
            rb_state12.var_16 = (rb_state12.var_16) ^ (51934 < rb_input_size ? (uint32_t)rb_input[51934] : 0x2dd37091);
            rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_6);
            rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_15);
            rb_state12.var_19 = (rb_state12.var_19) ^ (((((0x283f4519) + (rb_state12.var_16)) - (rb_state12.var_8)) - (rb_state12.var_7)) ^ (rb_state12.var_17));
            rb_state12.var_20 = (rb_state12.var_20) ^ (((0x258ebd76) ^ (rb_state12.var_18)) + (0x20d910ca));
            rb_state12.var_21 = (rb_state12.var_21) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_19) : (0x3ccf689a));
            rb_state12.var_22 = (rb_state12.var_22) - (rb_state12.var_20);
            rb_state12.var_23 = (rb_state12.var_23) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_21) : (0xccd56b47));
            rb_state12.var_24 = (rb_state12.var_24) + (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_23) : (0x673cf00e));
            rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_24);
            pthread_mutex_unlock(&(rb_state12.lock_25));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x130406b0)) {
            pthread_mutex_lock(&(rb_state15.lock_28));
            rb_state15.var_3 = (rb_state15.var_3) ^ (((0x9cb78b5d) ^ (rb_state15.var_2)) - (0x4ce86a41));
            rb_state15.var_4 = (rb_state15.var_4) ^ (98789 < rb_input_size ? (uint32_t)rb_input[98789] : 0xbe5e6849);
            rb_state15.var_5 = (rb_state15.var_5) ^ (((((0x1db0cf2f) ^ (rb_state15.var_3)) ^ (rb_state15.var_1)) + (rb_state15.var_4)) ^ (rb_state15.var_3));
            rb_state15.var_6 = (rb_state15.var_6) ^ (rb_state15.var_4);
            rb_state15.var_7 = (rb_state15.var_7) ^ (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_5) : (0xeb9ef015));
            rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_6) : (0x2c366969));
            rb_state15.var_9 = (rb_state15.var_9) + (rb_state15.var_7);
            rb_state15.var_10 = (rb_state15.var_10) - (rb_state15.var_8);
            rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_10) : (0x6bd31a22));
            rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_11);
            rb_state15.var_1 = rb_state15.var_2;
            pthread_mutex_unlock(&(rb_state15.lock_28));
        }
        if ((rb_state15.var_0) == (0x130406b0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                racebench_trigger(15);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0xecf74593);
        rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0xecf74593)) ? (rb_state16.var_0) : (0x593fc75f));
        if ((rb_state16.var_0) == (0xf261a664)) {
            rb_state16.var_2 = rb_state16.var_1;
        }
        if ((rb_state16.var_0) == (0x77d6ab56)) {
            rb_state16.var_3 = 0x61b70f69;
            rb_state16.var_4 = 0x56f78a4e;
            rb_state16.var_5 = (rb_state16.var_5) - (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_4) : (0xb4bef3f4));
            rb_state16.var_6 = (rb_state16.var_6) + (((0xcf9a2be6) + (rb_state16.var_3)) - (rb_state16.var_3));
            rb_state16.var_7 = (rb_state16.var_7) + (((0xd669b49e) ^ (rb_state16.var_4)) + (rb_state16.var_5));
            rb_state16.var_8 = (rb_state16.var_8) + (((0xcedefc16) + (rb_state16.var_5)) ^ (rb_state16.var_6));
            rb_state16.var_9 = (rb_state16.var_9) - (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0xf211483a));
            rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_9);
        }
        if ((rb_state16.var_0) == (0x77d6ab56)) {
            if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state19.lock_28));
            rb_state19.var_3 = 0x4650d7ec;
            rb_state19.var_4 = 0x667a3218;
            rb_state19.var_5 = (rb_state19.var_5) ^ (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0x80f9b973));
            rb_state19.var_6 = (rb_state19.var_6) - ((0xffc76fa2) ^ (rb_state19.var_4));
            rb_state19.var_7 = (rb_state19.var_7) ^ (rb_state19.var_3);
            rb_state19.var_8 = (rb_state19.var_8) + (((0x4b46cbc1) + (0x26a6a5e7)) - (rb_state19.var_4));
            rb_state19.var_9 = (rb_state19.var_9) ^ (((((0x4ec7e8e6) + (0xd252c329)) + (rb_state19.var_5)) ^ (rb_state19.var_5)) - (rb_state19.var_1));
            rb_state19.var_10 = (rb_state19.var_10) ^ (((0xdb948659) ^ (rb_state19.var_6)) ^ (rb_state19.var_6));
            rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_7) : (0x312df1f9));
            rb_state19.var_12 = (rb_state19.var_12) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_8) : (0x44af82c6));
            rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0xefe7fab3));
            rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0xe97fca3f));
            rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_13) : (0x11aa93cf));
            rb_state19.var_16 = (rb_state19.var_16) + (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_14) : (0xd111bf92));
            rb_state19.var_17 = (rb_state19.var_17) + (rb_state19.var_15);
            rb_state19.var_18 = (rb_state19.var_18) ^ (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_16) : (0xc5b6bcb3));
            rb_state19.var_19 = (rb_state19.var_19) - (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_18) : (0x83570164));
            rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_19);
            rb_state19.var_1 = rb_state19.var_2;
            pthread_mutex_unlock(&(rb_state19.lock_28));
        }
        if ((rb_state19.var_0) == (0x0)) {
            if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                racebench_trigger(19);
            }
        }
        #endif
        NReceived[MyNum][i] = ToReceive[MyNum][i];
    }
}

ScatterUpdateFO(dimi, structi, dimj, structj, destdim, oldupdate, newupdate) int dimi, *structi, dimj, *structj;
double *oldupdate, *newupdate;
{
    int i, j, top_of_destcol;
    double *srccol, *destcol;

    for (j = 0; j < dimj; j++) {
        if (structj) {
            destcol = &newupdate[structj[j] * destdim];
        } else {
            destcol = &newupdate[j * destdim];
        }
        srccol = &oldupdate[j * dimi];
        if (structi) {
            for (i = 0; i < dimi; i++) {
                destcol[structi[i]] += srccol[i];
                srccol[i] = 0.0;
            }
        } else {
            for (i = 0; i < dimi; i++) {
                destcol[i] += srccol[i];
                srccol[i] = 0.0;
            }
        }
    }
}

ScatterUpdateFO2(dimi, structi, dimj, structj, stride, destdim, oldupdate, newupdate) int dimi, *structi, dimj, *structj, stride;
double *oldupdate, *newupdate;
{
    int i, j, top_of_srccol, top_of_destcol;

    top_of_srccol = 0;
    for (j = 0; j < dimj; j++) {
        if (structj) {
            top_of_destcol = structj[j] * destdim;
        } else {
            top_of_destcol = j * destdim;
        }
        if (structi) {
            for (i = 0; i < dimi; i++) {
                newupdate[structi[i] + top_of_destcol] += oldupdate[i + top_of_srccol];
            }
        } else {
            for (i = 0; i < dimi; i++) {
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
                if ((rb_state2.var_0) == (0x0)) {
                    rb_state2.var_3 = (rb_state2.var_3) + (83914 < rb_input_size ? (uint32_t)rb_input[83914] : 0x7248e821);
                    rb_state2.var_4 = (rb_state2.var_4) - (0x6739593);
                    rb_state2.var_5 = (rb_state2.var_5) - (0x763d8ed2);
                    rb_state2.var_6 = (rb_state2.var_6) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_2) : (0x90893d66));
                    rb_state2.var_7 = (rb_state2.var_7) + (((0x4a939e7f) ^ (rb_state2.var_3)) - (rb_state2.var_3));
                    rb_state2.var_8 = (rb_state2.var_8) - (((((0x89bf6606) ^ (rb_state2.var_5)) + (rb_state2.var_4)) ^ (rb_state2.var_4)) + (0x5873dff6));
                    rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0x7fa1fc3c));
                    rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_8) : (0xe88f0628));
                    rb_state2.var_11 = (rb_state2.var_11) ^ (rb_state2.var_9);
                    rb_state2.var_12 = (rb_state2.var_12) - (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0xb91a8f59));
                    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_12);
                }
                #endif
                newupdate[i + top_of_destcol] += oldupdate[i + top_of_srccol];
            }
        }
        top_of_srccol += (stride - j);
    }
}