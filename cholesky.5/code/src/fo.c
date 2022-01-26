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
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        rb_state1.var_6 = 0xad0093ea;
        rb_state1.var_7 = 0x2b3e2bb5;
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_5) : (0x9b4dc75f));
        rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x2b933730));
        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0x3d7e9a77));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_9);
        rb_state1.var_12 = (rb_state1.var_12) - (((0x9ad54999) - (rb_state1.var_8)) + (rb_state1.var_10));
        rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_12) : (0x26ef8b4d));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x0)) {
        rb_state14.var_12 = 0x154be026;
        rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_12) : (0xc5b87258));
        rb_state14.var_14 = (rb_state14.var_14) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_9) : (0x5ba78d6d));
        rb_state14.var_15 = (rb_state14.var_15) - (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_13) : (0x77e2eb91));
        rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_15) : (0xdccd5651));
        rb_state14.var_5 = (rb_state14.var_5) + (rb_state14.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) ^ (0xf3dc8613);
    #endif
    InitRemainingFO(MyNum, lc);
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_4 = (rb_state9.var_4) + (0x2e610ae2);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_3) == (0xb4c75714)) {
        rb_state11.var_12 = (rb_state11.var_12) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_7) : (0x606aab1b));
        rb_state11.var_13 = (rb_state11.var_13) ^ (73659 < rb_input_size ? (uint32_t)rb_input[73659] : 0x36ba97ee);
        rb_state11.var_14 = (rb_state11.var_14) - (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_8) : (0x8704e2bb));
        rb_state11.var_15 = (rb_state11.var_15) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_12) : (0xd79b272d));
        rb_state11.var_16 = (rb_state11.var_16) ^ (rb_state11.var_13);
        rb_state11.var_17 = (rb_state11.var_17) ^ (((((0x14cdc24a) ^ (rb_state11.var_15)) - (rb_state11.var_14)) - (rb_state11.var_11)) ^ (rb_state11.var_10));
        rb_state11.var_18 = (rb_state11.var_18) - (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_16) : (0x99552870));
        rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x2cd44ea7));
        rb_state11.var_11 = (rb_state11.var_11) - (rb_state11.var_19);
        rb_state11.var_4 = rb_state11.var_11;
    }
    if ((rb_state11.var_3) == (0xb4c75714)) {
        pthread_mutex_lock(&(rb_state11.lock_32));
        rb_state11.var_21 = 0x108ee6e0;
        rb_state11.var_22 = 0x390c435c;
        rb_state11.var_23 = (rb_state11.var_23) + (((((0x166691b6) ^ (rb_state11.var_21)) - (rb_state11.var_14)) - (0x78d0d53d)) ^ (rb_state11.var_13));
        rb_state11.var_24 = (rb_state11.var_24) ^ (0x26d09965);
        rb_state11.var_25 = (rb_state11.var_25) ^ (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_22) : (0x67c408a9));
        rb_state11.var_26 = (rb_state11.var_26) + (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_24) : (0xc0c7704c));
        rb_state11.var_27 = (rb_state11.var_27) ^ (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_25) : (0x5f8bbb0a));
        rb_state11.var_28 = (rb_state11.var_28) + (((0x9d5bf61c) ^ (rb_state11.var_26)) + (0x1460660c));
        rb_state11.var_29 = (rb_state11.var_29) - (rb_state11.var_27);
        rb_state11.var_30 = (rb_state11.var_30) + (rb_state11.var_28);
        rb_state11.var_31 = (rb_state11.var_31) + (((rb_state11.var_29) == (0x0)) ? (rb_state11.var_30) : (0xa2072550));
        rb_state11.var_20 = (rb_state11.var_20) ^ (rb_state11.var_31);
        rb_state11.var_4 = rb_state11.var_20;
        pthread_mutex_unlock(&(rb_state11.lock_32));
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

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_4 = (rb_state12.var_4) ^ (0xdf0f2979);
    #endif
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
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_4 = (rb_state4.var_4) + (0x6b3253ed);
        rb_state4.var_4 = (rb_state4.var_4) + (89256 < rb_input_size ? (uint32_t)rb_input[89256] : 0x6958a6c3);
        if ((rb_state4.var_4) == (0xd7d8c649)) {
            rb_state4.var_7 = 0x1;
        }
        if ((rb_state4.var_4) == (0x430b1a56)) {
            if ((rb_state4.var_7) != (0x0)) {
                rb_state4.var_25 = !((rb_state4.var_5) == (rb_state4.var_6));
            }
        }
        if ((rb_state4.var_4) == (0x430b1a56)) {
            pthread_mutex_lock(&(rb_state4.lock_24));
            rb_state4.var_8 = 0xce741a1e;
            rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_6) : (0x9ae75ce3));
            rb_state4.var_10 = (rb_state4.var_10) - ((0xddef4f0d) ^ (0x24c2c8b));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_5) : (0x1f0bb5cd));
            rb_state4.var_12 = (rb_state4.var_12) + (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0xc4e36a24));
            rb_state4.var_13 = (rb_state4.var_13) ^ (((0x70dd0753) + (rb_state4.var_10)) - (rb_state4.var_9));
            rb_state4.var_14 = (rb_state4.var_14) ^ (((((0x4f884541) - (rb_state4.var_11)) - (0x314520f4)) + (rb_state4.var_11)) + (rb_state4.var_10));
            rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_12);
            rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_13);
            rb_state4.var_17 = (rb_state4.var_17) ^ (((0x822aa81f) - (rb_state4.var_14)) - (rb_state4.var_12));
            rb_state4.var_18 = (rb_state4.var_18) - (rb_state4.var_15);
            rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_16) : (0x183ff727));
            rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_17) : (0xe00f2299));
            rb_state4.var_21 = (rb_state4.var_21) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_18) : (0x1da7096f));
            rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_20) : (0x4a5086ba));
            rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_22) : (0xf10f989c));
            rb_state4.var_6 = (rb_state4.var_6) - (rb_state4.var_23);
            rb_state4.var_5 = rb_state4.var_6;
            pthread_mutex_unlock(&(rb_state4.lock_24));
        }
        rb_state4.var_16 = (rb_state4.var_16) - (0x212dc370);
        rb_state4.var_17 = (rb_state4.var_17) - (134194 < rb_input_size ? (uint32_t)rb_input[134194] : 0x3bc9d09a);
        rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_19) : (0xb572d391));
        rb_state4.var_21 = (rb_state4.var_21) + (14012 < rb_input_size ? (uint32_t)rb_input[14012] : 0x355e92a1);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_4 = (rb_state9.var_4) ^ (0xd7d48ea5);
        rb_state9.var_4 = (rb_state9.var_4) ^ (115016 < rb_input_size ? (uint32_t)rb_input[115016] : 0x7cf3da57);
        if ((rb_state9.var_1) == (0x59488f98)) {
            usleep(100);
        }
        if ((rb_state9.var_1) == (0x59488f98)) {
            rb_state9.var_21 = !((rb_state9.var_6) == (rb_state9.var_5));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_3) == (0xb4c75714)) {
            if ((rb_state11.var_4) != (0x0)) {
                if (!((rb_state11.var_4) == (rb_state11.var_20))) {
                    racebench_trigger(11);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_4) == (0x977e2088)) ? (rb_state12.var_3) : (0xe11d4145));
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_3) == (0x0)) {
            usleep(100);
        }
        if ((rb_state15.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_14));
            rb_state15.var_7 = 0xab575f55;
            rb_state15.var_8 = 0x7e4897e0;
            rb_state15.var_9 = (rb_state15.var_9) ^ (rb_state15.var_8);
            rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_7) : (0x95f899f6));
            rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_4) : (0x1cd2ee86));
            rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_10) : (0x5c902405));
            rb_state15.var_13 = (rb_state15.var_13) - (((((0x2e545939) - (rb_state15.var_11)) - (0xc3e80345)) ^ (rb_state15.var_7)) + (rb_state15.var_12));
            rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_13);
            rb_state15.var_6 = rb_state15.var_5;
            pthread_mutex_unlock(&(rb_state15.lock_14));
        }
        if ((rb_state15.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_14));
            rb_state15.var_15 = !((rb_state15.var_6) == (rb_state15.var_5));
            pthread_mutex_unlock(&(rb_state15.lock_14));
        }
        rb_state15.var_9 = (rb_state15.var_9) + (rb_state15.var_8);
        rb_state15.var_10 = (rb_state15.var_10) - (118406 < rb_input_size ? (uint32_t)rb_input[118406] : 0x744b1731);
        rb_state15.var_11 = (rb_state15.var_11) ^ ((0x6dd6bea5) ^ (rb_state15.var_12));
        rb_state15.var_13 = (rb_state15.var_13) - ((0xac34c511) ^ (rb_state15.var_15));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) ^ (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_3) : (0x5614ce7d));
        rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_1);
        rb_state18.var_2 = (rb_state18.var_2) + (19116 < rb_input_size ? (uint32_t)rb_input[19116] : 0x77a5e658);
        rb_state18.var_3 = (rb_state18.var_3) + (((rb_state18.var_1) == (0xc68b6509)) ? (rb_state18.var_0) : (0x453e7dc2));
        if ((rb_state18.var_2) == (0x400)) {
            rb_state18.var_4 = rb_state18.var_3;
        }
        if ((rb_state18.var_2) == (0x400)) {
            if (!((rb_state18.var_3) == (rb_state18.var_4))) {
                racebench_trigger(18);
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
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_13 = (rb_state9.var_13) - (16696 < rb_input_size ? (uint32_t)rb_input[16696] : 0xfe4ffd3c);
                rb_state9.var_14 = (rb_state9.var_14) - (rb_state9.var_15);
                if ((rb_state9.var_16) == (0xed7399e6)) {
                    rb_state9.var_23 = 0x1288a2df;
                    rb_state9.var_24 = 0x89926491;
                    rb_state9.var_25 = (rb_state9.var_25) ^ (((((0xa7ca8a0e) - (rb_state9.var_23)) + (rb_state9.var_19)) ^ (rb_state9.var_24)) ^ (rb_state9.var_21));
                    rb_state9.var_26 = (rb_state9.var_26) ^ (145813 < rb_input_size ? (uint32_t)rb_input[145813] : 0xeaef6a40);
                    rb_state9.var_27 = (rb_state9.var_27) - (rb_state9.var_18);
                    rb_state9.var_28 = (rb_state9.var_28) + (((0x739b36f6) - (rb_state9.var_25)) ^ (0x86db4847));
                    rb_state9.var_29 = (rb_state9.var_29) + (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_27) : (0xbeda62d));
                    rb_state9.var_30 = (rb_state9.var_30) + (((0x268b71ef) + (0x208ccccd)) - (rb_state9.var_28));
                    rb_state9.var_31 = (rb_state9.var_31) ^ (((0x1e73cbc4) + (0x5cce14cd)) + (rb_state9.var_29));
                    rb_state9.var_32 = (rb_state9.var_32) + (((((0x67b767a3) ^ (0x2e2794a7)) - (rb_state9.var_30)) + (0x9eb69e73)) ^ (rb_state9.var_31));
                    rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_32);
                }
                #endif
                newupdate[i + top_of_destcol] += oldupdate[i + top_of_srccol];
            }
        }
        top_of_srccol += (stride - j);
    }
}