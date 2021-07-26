
#include "wtime.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <sys/time.h>
#include "common.h"

using namespace std;

#include "buffer.h"

#ifdef _OPENMP
#include <omp.h>
#else
static int omp_get_max_threads() {
    return 1;
}
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define LINT sizeof(int)

int **ITlen;
int **bran;
int **prefix;

int *order_item;
int *item_order;

int **compact;
int **supp;

stack **list;
int TRANSACTION_NO = 0;
int ITEM_NO = 100;
int THRESHOLD;

memory **fp_buf;
memory *fp_node_sub_buf;
memory **fp_tree_buf;
memory *database_buf;
int **new_data_num;

void printLen() {
    int i, j;
    int workingthread = omp_get_max_threads();

    for (j = 1; j < workingthread; j++) {
        for (i = 0; i < ITEM_NO; i++) {
            ITlen[0][i] += ITlen[j][i];
        }
    }
    for (i = ITEM_NO - 1; i >= 0 && ITlen[0][i] == 0; i--) {
        ;
    }
    for (j = 0; j <= i; j++) {
        printf("%d\n", ITlen[0][j]);
    }
}

int main(int argc, char **argv) {
    double tstart, tdatap, tend;
    int workingthread = omp_get_max_threads();
    int i;
    FP_tree *fptree;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    printf("PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) "\n");
    fflush(NULL);
#else
    printf("PARSEC Benchmark Suite\n");
    fflush(NULL);
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_freqmine);
#endif

    if (argc < 3) {
        cout << "usage: " << argv[0] << " <infile> <MINSUP> [<outfile>]\n";
        exit(1);
    }
    THRESHOLD = atoi(argv[2]);

    Data *fdat = new Data(argv[1]);

    if (!fdat->isOpen()) {
        cerr << argv[1] << " could not be opened!" << endl;
        exit(2);
    }

    wtime(&tstart);
    fp_buf = new memory *[workingthread];
    fp_tree_buf = new memory *[workingthread];
    for (i = 0; i < workingthread; i++) {
        fp_buf[i] = new memory(60, 10485760, 20971520, 2);
        fp_tree_buf[i] = new memory(80, 10485760, 20971520, 2);
    }
    database_buf = new memory(60, 4194304L, 4194304L, 2);
    fptree = (FP_tree *)fp_buf[0]->newbuf(1, sizeof(FP_tree));
    fptree->init(-1, 0, 0);
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif
    fptree->scan1_DB(fdat);
    wtime(&tdatap);

    fptree->scan2_DB(workingthread);
    fdat->close();
    if (fptree->itemno == 0) {
        return 0;
    }
    FSout *fout;
    if (argc == 4) {
        fout = new FSout(argv[3]);

        fout->printSet(0, NULL, TRANSACTION_NO);
    } else {
        fout = NULL;
    }

    if (0 && fptree->Single_path(0)) {
        Fnode *node;
        i = 0;
        for (node = fptree->Root->leftchild; node != NULL; node = node->leftchild) {
            list[0]->FS[i++] = node->itemname;
        }

        if (fout) {
            fptree->generate_all(fptree->itemno, 0, fout);
        }

        printLen();
        return 0;
    }

    fptree->FP_growth_first(fout);
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif
    printLen();
    if (fout) {
        fout->close();
    }

    for (i = 0; i < workingthread; i++) {
        delete fp_buf[i];
        delete fp_tree_buf[i];
    }

    delete[] fp_buf;
    delete[] fp_tree_buf;
    delete[] order_item;
    delete[] item_order;

    wtime(&tend);
    printf("the data preparation cost %f seconds, the FPgrowth cost %f seconds\n", tdatap - tstart, tend - tdatap);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif

    return 0;
}
