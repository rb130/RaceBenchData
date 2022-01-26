#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_THREADS 1024
pthread_t PThreadTable[MAX_THREADS];

#include <math.h>
#include "matrix.h"

#define SH_MEM_AMT 100000000
#define DEFAULT_PPS 32
#define DEFAULT_CS 16384
#define DEFAULT_P 1

double CacheSize = DEFAULT_CS;
double CS;
int BS = 45;
void Go();

struct GlobalMemory *Global;

int *T, *nz, *node, *domain, *domains, *proc_domains;

int *PERM, *INVP;

int solution_method = FAN_OUT * 10 + 0;

int distribute = -1;

int target_partition_size = 0;
int postpass_partition_size = DEFAULT_PPS;
int permutation_method = 1;
int join = 1;
int scatter_decomposition = 0;

int P = DEFAULT_P;
int iters = 1;
SMatrix M;

char probname[80];

extern struct Update *freeUpdate[MAX_PROC];
extern struct Task *freeTask[MAX_PROC];

struct gpid {
    int pid;
    unsigned long initdone;
    unsigned long finish;
} * gp;

int do_test = 0;
int do_stats = 0;

const char *input_file;

main(argc, argv) char *argv[];
{
    SMatrix L;
    double *b, *x, *CreateVector(), *TriangularSolve(), *TriBSolve();
    double norm, ComputeNorm();
    unsigned int elapsed;
    extern BMatrix LB;
    int i;
    int c;
    int *assigned_ops, num_nz, num_domain, num_alloc, ps;
    int *PERM2;
    extern char *optarg;
    extern int *firstchild, *child, *nz, *node;
    extern double *work_tree;
    extern int *domain, *partition;
    extern int *block_start, *all_blocks;
    unsigned long start;
    double mint, maxt, avgt;

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    }

    while ((c = getopt(argc, argv, "B:C:p:D:s:ith")) != -1) {
        switch (c) {
        case 'B':
            postpass_partition_size = atoi(optarg);
            break;
        case 'C':
            CacheSize = (double)atoi(optarg);
            break;
        case 'p':
            P = atoi(optarg);
            break;
        case 'i':
            input_file = optarg;
            break;
        case 's':
            do_stats = 1;
            break;
        case 't':
            do_test = 1;
            break;
        case 'h':
            printf("Usage: SCHOL <options> file\n\n");
            printf("options:\n");
            printf("  -Bb : Use a postpass partition size of b.\n");
            printf("  -Cc : Cache size in bytes.\n");
            printf("  -pP : P = number of processors.\n");
            printf("  -s  : Print individual processor timing statistics.\n");
            printf("  -t  : Test output.\n");
            printf("  -h  : Print out command line options.\n\n");
            printf("Default: SCHOL -p%1d -B%1d -C%1d\n", DEFAULT_P, DEFAULT_PPS, DEFAULT_CS);
            exit(0);
            break;
        }
    }

    freopen(input_file, "r", stdin);

    CS = CacheSize / 8.0;
    CS = sqrt(CS);
    BS = (int)floor(CS + 0.5);

    { ; }

    gp = (struct gpid *)malloc(sizeof(struct gpid));
    ;
    gp->pid = 0;
    Global = (struct GlobalMemory *)malloc(sizeof(struct GlobalMemory));
    ;
    {
        unsigned long Error;

        Error = pthread_mutex_init(&(Global->start).mutex, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            exit(-1);
        }

        Error = pthread_cond_init(&(Global->start).cv, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            pthread_mutex_destroy(&(Global->start).mutex);
            exit(-1);
        }

        (Global->start).counter = 0;
        (Global->start).cycle = 0;
    }
    { pthread_mutex_init(&(Global->waitLock), NULL); }
    { pthread_mutex_init(&(Global->memLock), NULL); }

    MallocInit(P);

    i = 0;
    while (++i < argc && argv[i][0] == '-') {
        ;
    }
    M = ReadSparse(argv[i], probname);

    for (int ti = 0; ti <= M.n; ++ti)
        if (M.col[ti] > M.n + M.m)
            return 1;
    for (int ti = 0; ti < M.n + M.m; ++ti)
        if (M.row[ti] > M.n)
            return 1;

    distribute = LB_DOMAINS * 10 + EMBED;

    printf("\n");
    printf("Sparse Cholesky Factorization\n");
    printf("     Problem: %s\n", probname);
    printf("     %d Processors\n", P);
    printf("     Postpass partition size: %d\n", postpass_partition_size);
    printf("     %0.0f byte cache\n", CacheSize);
    printf("\n");
    printf("\n");

    printf("true partitions\n");

    printf("Fan-out, ");
    printf("no block copy-across\n");

    printf("LB domain, ");
    printf("embedded ");
    printf("distribution\n");

    printf("No ordering\n");

    PERM = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);
    INVP = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);

    CreatePermutation(M.n, (int *)NULL, PERM, NO_PERM);

    InvertPerm(M.n, PERM, INVP);

    T = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);
    EliminationTreeFromA(M, T, PERM, INVP);

    firstchild = (int *)MyMalloc((M.n + 2) * sizeof(int), DISTRIBUTED);
    child = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);
    ParentToChild(T, M.n, firstchild, child);

    nz = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);
    ComputeNZ(M, T, nz, PERM, INVP);

    work_tree = (double *)MyMalloc((M.n + 1) * sizeof(double), DISTRIBUTED);
    ComputeWorkTree(M, nz, work_tree);

    node = (int *)MyMalloc((M.n + 1) * sizeof(int), DISTRIBUTED);
    FindSupernodes(M, T, nz, node, PERM, INVP);

    Amalgamate2(1, M, T, nz, node, (int *)NULL, 1);

    assigned_ops = (int *)malloc(P * sizeof(int));
    domain = (int *)MyMalloc(M.n * sizeof(int), DISTRIBUTED);
    domains = (int *)MyMalloc(M.n * sizeof(int), DISTRIBUTED);
    proc_domains = (int *)MyMalloc((P + 1) * sizeof(int), DISTRIBUTED);
    printf("before partition\n");
    fflush(stdout);
    Partition(M, P, T, assigned_ops, domain, domains, proc_domains, distribute);
    free(assigned_ops);

    {
        int i, tot_domain_updates, tail_length;

        tot_domain_updates = 0;
        for (i = 0; i < proc_domains[P]; i++) {
            tail_length = nz[domains[i]] - 1;
            tot_domain_updates += tail_length * (tail_length + 1) / 2;
        }
        printf("%d total domain updates\n", tot_domain_updates);
    }

    num_nz = num_domain = 0;
    for (i = 0; i < M.n; i++) {
        num_nz += nz[i];
        if (domain[i]) {
            num_domain += nz[i];
        }
    }

    ComputeTargetBlockSize(M, P);

    printf("Target partition size %d, postpass size %d\n", target_partition_size, postpass_partition_size);

    NoSegments(M);

    PERM2 = (int *)malloc((M.n + 1) * sizeof(int));
    CreatePermutation(M.n, node, PERM2, permutation_method);
    ComposePerm(PERM, PERM2, M.n);
    free(PERM2);

    InvertPerm(M.n, PERM, INVP);

    b = CreateVector(M);

    ps = postpass_partition_size;
    num_alloc = num_domain + (num_nz - num_domain) * 10 / ps / ps;
    CreateBlockedMatrix2(M, num_alloc, T, firstchild, child, PERM, INVP, domain, partition);

    FillInStructure(M, firstchild, child, PERM, INVP);

    AssignBlocksNow(distribute);

    AllocateNZ();

    FillInNZ(M, PERM, INVP);
    FreeMatrix(M);

    InitTaskQueues(P);

    PreAllocate1FO(0);
    ComputeRemainingFO();
    ComputeReceivedFO();

    {
        long i, Error;

        for (i = 0; i < (P)-1; i++) {
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x36)) ? (rb_state5.var_0) : (0xcac165c0));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
        rb_state11.var_1 = (rb_state11.var_1) - ((0x2e129e38) + (rb_state11.var_0));
        #endif
        Go();
    }

    {
        unsigned long i, Error;
        for (i = 0; i < (P)-1; i++) {
            Error = pthread_join(PThreadTable[i], NULL);
            if (Error != 0) {
                printf("Error in pthread_join().\n");
                exit(-1);
            }
        }
    }

    printf("%.0f operations for factorization\n", work_tree[M.n]);

    printf("\n");
    printf("                            PROCESS STATISTICS\n");
    printf("              Total\n");
    printf(" Proc         Time \n");
    printf("    0    %10.0d\n", Global->runtime[0]);
    if (do_stats) {
        maxt = avgt = mint = Global->runtime[0];
        for (i = 1; i < P; i++) {
            if (Global->runtime[i] > maxt) {
                maxt = Global->runtime[i];
            }
            if (Global->runtime[i] < mint) {
                mint = Global->runtime[i];
            }
            avgt += Global->runtime[i];
        }
        avgt = avgt / P;
        for (i = 1; i < P; i++) {
            printf("  %3d    %10ld\n", i, Global->runtime[i]);
        }
        printf("  Avg    %10.0f\n", avgt);
        printf("  Min    %10.0f\n", mint);
        printf("  Max    %10.0f\n", maxt);
        printf("\n");
    }

    printf("                            TIMING INFORMATION\n");
    printf("Start time                        : %16ld\n", start);
    printf("Initialization finish time        : %16ld\n", gp->initdone);
    printf("Overall finish time               : %16ld\n", gp->finish);
    printf("Total time with initialization    : %16ld\n", gp->finish - start);
    printf("Total time without initialization : %16ld\n", gp->finish - gp->initdone);
    printf("\n");

    if (do_test) {
        printf("                             TESTING RESULTS\n");
        x = TriBSolve(LB, b, PERM, INVP);
        norm = ComputeNorm(x, LB.n);
        if (norm >= 0.0001) {
            printf("Max error is %10.9f\n", norm);
        } else {
            printf("PASSED\n");
        }
    }

    { exit(0); }
}

void Go() {
    int iter;
    int MyNum;
    struct LocalCopies *lc;

    {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xa5bc2a44));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0x2a42b25) + (0x24c9d9c0));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (0x96caa5f2);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x4bdca036));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0x1cd204a6));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x17518581) + (0x9d363785));
    if ((rb_state17.var_1) == (0x0)) {
        rb_state17.var_2 = rb_state17.var_0;
    }
    if ((rb_state17.var_1) == (0x0)) {
        if (!((rb_state17.var_0) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (25165 < rb_input_size ? (uint32_t)rb_input[25165] : 0xdfa21b0c);
    rb_state0.var_1 = (rb_state0.var_1) - ((0xa4f61625) + (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x2fc666a7));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + ((0x4d242478) + (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (148039 < rb_input_size ? (uint32_t)rb_input[148039] : 0x929b3549);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (0xaf4ebae0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (104422 < rb_input_size ? (uint32_t)rb_input[104422] : 0xfda3f6ad);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (0xe25cc591);
    rb_state3.var_1 = (rb_state3.var_1) + (0xf74ac72c);
    rb_state3.var_0 = (rb_state3.var_0) + ((0x87d6d4f) ^ (0x1801c674));
    if ((rb_state3.var_1) == (0x8cf6a637)) {
        rb_state3.var_8 = 0xaa30686c;
        rb_state3.var_9 = 0x26755607;
        rb_state3.var_10 = (rb_state3.var_10) + (((((0xf626eab8) ^ (rb_state3.var_6)) ^ (rb_state3.var_7)) + (0xf63ba04f)) + (rb_state3.var_8));
        rb_state3.var_11 = (rb_state3.var_11) - (((0xd215cdfa) ^ (rb_state3.var_9)) ^ (0x4b6a9a1d));
        rb_state3.var_12 = (rb_state3.var_12) ^ (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_10) : (0xb7720ad8));
        rb_state3.var_13 = (rb_state3.var_13) - (((0x28914b4b) ^ (rb_state3.var_11)) - (rb_state3.var_9));
        rb_state3.var_14 = (rb_state3.var_14) + (rb_state3.var_12);
        rb_state3.var_15 = (rb_state3.var_15) - (((0x457c07c2) - (0xa67c09a5)) - (rb_state3.var_13));
        rb_state3.var_16 = (rb_state3.var_16) - (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0xd7f629a6));
        rb_state3.var_2 = (rb_state3.var_2) - (rb_state3.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0x8f701408)) {
        rb_state11.var_3 = 0x1009ff37;
        rb_state11.var_4 = 0x61388953;
        rb_state11.var_5 = (rb_state11.var_5) ^ (((0x2a3cd0f8) - (0x7066c46e)) - (rb_state11.var_2));
        rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_4);
        rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_5) : (0xe1e8e49a));
        rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_6);
        rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_7) : (0x78b9d85a));
        rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_8);
        rb_state11.var_11 = (rb_state11.var_11) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_10) : (0xe7fe3270));
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_11);
    }
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - ((0xaaf0a947) ^ (0x6f8764e7));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ ((0x9e7e93c1) + (rb_state5.var_0));
    rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x0)) {
        rb_state5.var_9 = (rb_state5.var_9) + (rb_state5.var_9);
        rb_state5.var_10 = (rb_state5.var_10) - (((((0x5163bba9) ^ (rb_state5.var_8)) + (0xf0f70931)) - (rb_state5.var_10)) + (rb_state5.var_7));
        rb_state5.var_11 = (rb_state5.var_11) - (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_6) : (0x8ff17521));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((((0xc671aca3) + (rb_state5.var_12)) + (rb_state5.var_2)) + (rb_state5.var_9)) + (rb_state5.var_10));
        rb_state5.var_13 = (rb_state5.var_13) - (((((0x7f92f359) - (rb_state5.var_13)) - (rb_state5.var_11)) + (rb_state5.var_12)) - (0x3c92acf6));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_13);
    }
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ ((0x412e3ff7) ^ (0x3cf8b994));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x3b271580)) {
        if ((rb_state4.var_2) != (0x0)) {
            if (!((rb_state4.var_2) == (rb_state4.var_34))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_14));
        rb_state5.var_4 = 0x28be11e0;
        rb_state5.var_5 = 0x12fbb2fe;
        rb_state5.var_6 = (rb_state5.var_6) + (((0x64e609be) + (rb_state5.var_4)) - (0xd8b9abdc));
        rb_state5.var_7 = (rb_state5.var_7) - (((((0x5ca658f1) ^ (rb_state5.var_5)) ^ (rb_state5.var_3)) - (rb_state5.var_2)) ^ (rb_state5.var_4));
        rb_state5.var_8 = (rb_state5.var_8) ^ (((((0x4f534b87) ^ (0x3c8bc886)) + (rb_state5.var_5)) ^ (rb_state5.var_7)) - (rb_state5.var_6));
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_8);
        rb_state5.var_2 = rb_state5.var_3;
        pthread_mutex_unlock(&(rb_state5.lock_14));
    }
    if ((rb_state5.var_1) == (0x0)) {
        if (!((rb_state5.var_2) == (rb_state5.var_3))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xa3a250ea));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - ((0x94f8766e) ^ (rb_state13.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_1);
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x223b7b22)) {
        pthread_mutex_lock(&(rb_state0.lock_43));
        rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_5) : (0xce1f5eac));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_6) : (0x50704aa3));
        rb_state0.var_14 = (rb_state0.var_14) ^ (0x26bb0c0c);
        rb_state0.var_15 = (rb_state0.var_15) - (138486 < rb_input_size ? (uint32_t)rb_input[138486] : 0x206c03be);
        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_12) : (0x5e82d449));
        rb_state0.var_17 = (rb_state0.var_17) - (((((0x7f101d4) + (rb_state0.var_13)) ^ (rb_state0.var_9)) + (rb_state0.var_14)) + (rb_state0.var_10));
        rb_state0.var_18 = (rb_state0.var_18) - (((0x9a39426c) - (rb_state0.var_11)) ^ (rb_state0.var_15));
        rb_state0.var_19 = (rb_state0.var_19) + (((0xd6019d74) + (rb_state0.var_16)) - (rb_state0.var_12));
        rb_state0.var_20 = (rb_state0.var_20) + (((0xa2f0a09e) ^ (0xb3054736)) ^ (rb_state0.var_17));
        rb_state0.var_21 = (rb_state0.var_21) + (rb_state0.var_18);
        rb_state0.var_22 = (rb_state0.var_22) ^ (((((0x9dfc2505) + (rb_state0.var_19)) ^ (0xa8944851)) + (rb_state0.var_20)) + (rb_state0.var_13));
        rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_21) : (0x3fdd4117));
        rb_state0.var_24 = (rb_state0.var_24) ^ (((0x322b1089) ^ (rb_state0.var_15)) - (rb_state0.var_22));
        rb_state0.var_25 = (rb_state0.var_25) - (((0x5f9cba24) + (rb_state0.var_23)) + (rb_state0.var_16));
        rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_24) : (0x4c607817));
        rb_state0.var_27 = (rb_state0.var_27) + (rb_state0.var_25);
        rb_state0.var_28 = (rb_state0.var_28) - (rb_state0.var_26);
        rb_state0.var_29 = (rb_state0.var_29) ^ (rb_state0.var_27);
        rb_state0.var_30 = (rb_state0.var_30) ^ (rb_state0.var_28);
        rb_state0.var_31 = (rb_state0.var_31) - (((0xcc1e63d8) ^ (0xcf155fe8)) + (rb_state0.var_29));
        rb_state0.var_32 = (rb_state0.var_32) + (((0xa9b21dc7) + (rb_state0.var_18)) - (rb_state0.var_30));
        rb_state0.var_33 = (rb_state0.var_33) ^ (rb_state0.var_31);
        rb_state0.var_34 = (rb_state0.var_34) + (rb_state0.var_32);
        rb_state0.var_35 = (rb_state0.var_35) ^ (((0x624f94ad) + (rb_state0.var_33)) - (rb_state0.var_19));
        rb_state0.var_36 = (rb_state0.var_36) - (rb_state0.var_34);
        rb_state0.var_37 = (rb_state0.var_37) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_35) : (0x5c9fe881));
        rb_state0.var_38 = (rb_state0.var_38) - (((rb_state0.var_36) == (0x0)) ? (rb_state0.var_37) : (0xff6e5951));
        rb_state0.var_11 = (rb_state0.var_11) ^ (rb_state0.var_38);
        rb_state0.var_2 = rb_state0.var_11;
        pthread_mutex_unlock(&(rb_state0.lock_43));
    }
    if ((rb_state0.var_0) == (0x223b7b22)) {
        pthread_mutex_lock(&(rb_state0.lock_43));
        rb_state0.var_40 = (rb_state0.var_40) + (((((0x509ea095) - (0x6b843730)) ^ (rb_state0.var_22)) + (0x541d34c2)) - (rb_state0.var_23));
        rb_state0.var_41 = (rb_state0.var_41) - (((0x251623aa) ^ (rb_state0.var_21)) ^ (0x52c5b1b1));
        rb_state0.var_42 = (rb_state0.var_42) ^ (((rb_state0.var_40) == (0x0)) ? (rb_state0.var_41) : (0xc9eb7349));
        rb_state0.var_39 = (rb_state0.var_39) ^ (rb_state0.var_42);
        rb_state0.var_2 = rb_state0.var_39;
        pthread_mutex_unlock(&(rb_state0.lock_43));
    }
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0xec8202cc)) {
        rb_state19.var_2 = rb_state19.var_0;
    }
    if ((rb_state19.var_1) == (0xec8202cc)) {
        if (!((rb_state19.var_0) == (rb_state19.var_2))) {
            racebench_trigger(19);
        }
    }
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (Global->start).cycle;
        if (++(Global->start).counter != (P)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (Global->start).cycle) {
                Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (Global->start).cycle = !(Global->start).cycle;
            (Global->start).counter = 0;
            Error = pthread_cond_broadcast(&(Global->start).cv);
        }
        pthread_mutex_unlock(&(Global->start).mutex);
    };

    if ((MyNum == 0) || (do_stats)) {
        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (lc->rs) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };
    }

    BNumericSolveFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (Global->start).cycle;
        if (++(Global->start).counter != (P)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (Global->start).cycle) {
                Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (Global->start).cycle = !(Global->start).cycle;
            (Global->start).counter = 0;
            Error = pthread_cond_broadcast(&(Global->start).cv);
        }
        pthread_mutex_unlock(&(Global->start).mutex);
    };

    if ((MyNum == 0) || (do_stats)) {
        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (lc->rf) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };
        lc->runtime += (lc->rf - lc->rs);
    }

    if (MyNum == 0) {
        CheckRemaining();
        CheckReceived();
    }

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (Global->start).cycle;
        if (++(Global->start).counter != (P)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (Global->start).cycle) {
                Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (Global->start).cycle = !(Global->start).cycle;
            (Global->start).counter = 0;
            Error = pthread_cond_broadcast(&(Global->start).cv);
        }
        pthread_mutex_unlock(&(Global->start).mutex);
    };

    if ((MyNum == 0) || (do_stats)) {
        Global->runtime[MyNum] = lc->runtime;
    }
    if (MyNum == 0) {
        gp->initdone = lc->rs;
        gp->finish = lc->rf;
    }

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (Global->start).cycle;
        if (++(Global->start).counter != (P)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (Global->start).cycle) {
                Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (Global->start).cycle = !(Global->start).cycle;
            (Global->start).counter = 0;
            Error = pthread_cond_broadcast(&(Global->start).cv);
        }
        pthread_mutex_unlock(&(Global->start).mutex);
    };
}

PlaceDomains(P) {
    int p, d, first;
    char *range_start, *range_end;
    int page;
    extern int *firstchild, *child;
    extern BMatrix LB;

    for (p = P - 1; p >= 0; p--) {
        for (d = LB.proc_domains[p]; d < LB.proc_domains[p + 1]; d++) {
            first = LB.domains[d];
            while (firstchild[first] != firstchild[first + 1]) {
                first = child[firstchild[first]];
            }

            range_start = (char *)&LB.row[LB.col[first]];
            range_end = (char *)&LB.row[LB.col[LB.domains[d] + 1]];
            MigrateMem(&LB.row[LB.col[first]], (LB.col[LB.domains[d] + 1] - LB.col[first]) * sizeof(int), p);

            range_start = (char *)&BLOCK(LB.col[first]);
            range_end = (char *)&BLOCK(LB.col[LB.domains[d] + 1]);
            MigrateMem(&BLOCK(LB.col[first]), (LB.col[LB.domains[d] + 1] - LB.col[first]) * sizeof(double), p);
        }
    }
}

ComposePerm(PERM1, PERM2, n) int *PERM1, *PERM2, n;
{
    int i, *PERM3;

    PERM3 = (int *)malloc((n + 1) * sizeof(int));

    for (i = 0; i < n; i++) {
        PERM3[i] = PERM1[PERM2[i]];
    }

    for (i = 0; i < n; i++) {
        PERM1[i] = PERM3[i];
    }

    free(PERM3);
}