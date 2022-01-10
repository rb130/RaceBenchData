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
    if ((rb_state1.var_13) == (0x47ee31ea)) {
        pthread_mutex_lock(&(rb_state1.lock_28));
        rb_state1.var_19 = rb_state1.var_14;
        pthread_mutex_unlock(&(rb_state1.lock_28));
    }
    if ((rb_state1.var_13) == (0x47ee31ea)) {
        if (!((rb_state1.var_14) == (rb_state1.var_19))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        if ((rb_state15.var_2) != (0x0)) {
            if (!((rb_state15.var_2) == (rb_state15.var_21))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    InitRemainingFO(MyNum, lc);
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
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (116594 < rb_input_size ? (uint32_t)rb_input[116594] : 0x1b2cb94e);
        rb_state5.var_1 = (rb_state5.var_1) ^ (163730 < rb_input_size ? (uint32_t)rb_input[163730] : 0x197236ea);
        if ((rb_state5.var_0) == (0x2a8ff40c)) {
            rb_state5.var_2 = rb_state5.var_1;
        }
        if ((rb_state5.var_0) == (0x2a8ff40c)) {
            rb_state5.var_3 = 0xa4190d19;
            rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_3);
            rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_2) : (0xa0f8dd82));
            rb_state5.var_6 = (rb_state5.var_6) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_5) : (0xe70fb3dd));
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_6);
        }
        if ((rb_state5.var_0) == (0x2a8ff40c)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
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
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xf61ea5c5));
        if ((rb_state6.var_0) == (0x0)) {
            rb_state6.var_10 = 0xd1e9d862;
            rb_state6.var_11 = (rb_state6.var_11) ^ (((((0x532adda3) - (rb_state6.var_7)) + (rb_state6.var_6)) + (rb_state6.var_8)) + (0xc97f50e9));
            rb_state6.var_12 = (rb_state6.var_12) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0xc79f722f));
            rb_state6.var_13 = (rb_state6.var_13) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0x10351d37));
            rb_state6.var_14 = (rb_state6.var_14) - (rb_state6.var_12);
            rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_13);
            rb_state6.var_16 = (rb_state6.var_16) - (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_15) : (0x4891885a));
            rb_state6.var_9 = (rb_state6.var_9) + (rb_state6.var_16);
            rb_state6.var_1 = rb_state6.var_9;
        }
        if ((rb_state6.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state6.lock_31));
            rb_state6.var_18 = 0x48dfcd85;
            rb_state6.var_19 = 0xc90cd6bc;
            rb_state6.var_20 = (rb_state6.var_20) - (((0x25cffba2) ^ (rb_state6.var_11)) + (0x88273f7d));
            rb_state6.var_21 = (rb_state6.var_21) ^ (((0xb736fa17) - (rb_state6.var_18)) ^ (0xfb2d5655));
            rb_state6.var_22 = (rb_state6.var_22) ^ (((0x89279c2) + (rb_state6.var_19)) - (0x610fc6ed));
            rb_state6.var_23 = (rb_state6.var_23) + (((0x7bc2f4b8) - (rb_state6.var_20)) - (0xc7d85065));
            rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_21) : (0xba4096fc));
            rb_state6.var_25 = (rb_state6.var_25) + (((0x11e4e548) - (rb_state6.var_22)) - (rb_state6.var_13));
            rb_state6.var_26 = (rb_state6.var_26) - (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_23) : (0x9698f5aa));
            rb_state6.var_27 = (rb_state6.var_27) - (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_24) : (0x3d54b78));
            rb_state6.var_28 = (rb_state6.var_28) ^ (((((0xcb978ba3) ^ (rb_state6.var_16)) ^ (rb_state6.var_25)) ^ (rb_state6.var_26)) + (rb_state6.var_17));
            rb_state6.var_29 = (rb_state6.var_29) + (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_27) : (0xacc89689));
            rb_state6.var_30 = (rb_state6.var_30) ^ (((((0x9a27104a) - (0x1823af29)) + (rb_state6.var_29)) + (rb_state6.var_19)) ^ (rb_state6.var_28));
            rb_state6.var_17 = (rb_state6.var_17) + (rb_state6.var_30);
            rb_state6.var_1 = rb_state6.var_17;
            pthread_mutex_unlock(&(rb_state6.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0xafd324ea);
        rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x883ab64a)) ? (rb_state9.var_0) : (0x795f0c40));
        if ((rb_state9.var_0) == (0x0)) {
            rb_state9.var_14 = 0xa3b2fb69;
            rb_state9.var_15 = 0xdc78a688;
            rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_14) : (0xc773e2cf));
            rb_state9.var_17 = (rb_state9.var_17) - (((((0x9c58268e) + (rb_state9.var_11)) - (rb_state9.var_15)) - (rb_state9.var_16)) ^ (rb_state9.var_12));
            rb_state9.var_13 = (rb_state9.var_13) - (rb_state9.var_17);
            rb_state9.var_2 = rb_state9.var_13;
        }
        if ((rb_state9.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state9.lock_30));
            rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_14) : (0x6fd9799e));
            rb_state9.var_20 = (rb_state9.var_20) ^ ((0xd3462aa9) + (0x9bd63f58));
            rb_state9.var_21 = (rb_state9.var_21) + (0x194b8e80);
            rb_state9.var_22 = (rb_state9.var_22) ^ (((0xe04ca627) + (rb_state9.var_13)) + (rb_state9.var_16));
            rb_state9.var_23 = (rb_state9.var_23) + (((0xd82b568b) ^ (0xb8103655)) ^ (rb_state9.var_19));
            rb_state9.var_24 = (rb_state9.var_24) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_20) : (0xac4fc204));
            rb_state9.var_25 = (rb_state9.var_25) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_21) : (0x2ff318d5));
            rb_state9.var_26 = (rb_state9.var_26) + (((0x6f300e1b) ^ (rb_state9.var_22)) ^ (0xdaec176f));
            rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0xd0e17987));
            rb_state9.var_28 = (rb_state9.var_28) - (((((0xa4ed5981) + (rb_state9.var_20)) + (rb_state9.var_25)) ^ (rb_state9.var_26)) ^ (rb_state9.var_19));
            rb_state9.var_29 = (rb_state9.var_29) ^ (((((0x4fa2cc1) + (0x9b5334b0)) + (rb_state9.var_28)) + (0xd261acfc)) - (rb_state9.var_27));
            rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_29);
            rb_state9.var_2 = rb_state9.var_18;
            pthread_mutex_unlock(&(rb_state9.lock_30));
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
                newupdate[i + top_of_destcol] += oldupdate[i + top_of_srccol];
            }
        }
        top_of_srccol += (stride - j);
    }
}