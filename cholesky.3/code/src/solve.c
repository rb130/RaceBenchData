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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
            rb_state0.var_1 = (rb_state0.var_1) - (0x2ecc5111);
            rb_state0.var_1 = (rb_state0.var_1) - (0x7cd91794);
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
            rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_1) : (0x99d507db));
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_1) == (0x31)) {
                rb_state6.var_4 = 0x23332bb1;
                rb_state6.var_5 = 0x38ed08c1;
                rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_5);
                rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_3) : (0x5280edf9));
                rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_6);
                rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_7) : (0x6ea0bec0));
                rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_9) : (0x41cd4536));
                rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_10);
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0xf4f6cd1a)) ? (rb_state12.var_0) : (0xe302910a));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) ^ (0xd252ce42);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
            rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0xa7961004));
            rb_state16.var_1 = (rb_state16.var_1) + (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0x25d02778));
            rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0x40068980));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) ^ (27902 < rb_input_size ? (uint32_t)rb_input[27902] : 0xe525f652);
            rb_state18.var_0 = (rb_state18.var_0) ^ ((0x2aa35149) ^ (0x28977469));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x0)) {
            rb_state2.var_4 = 0x2c6389f8;
            rb_state2.var_5 = 0x25894482;
            rb_state2.var_6 = (rb_state2.var_6) + (((rb_state2.var_5) == (0x25894482)) ? (rb_state2.var_4) : (0xde79a343));
            rb_state2.var_7 = (rb_state2.var_7) ^ (((0x2789db0a) ^ (rb_state2.var_3)) + (rb_state2.var_4));
            rb_state2.var_8 = (rb_state2.var_8) + (((rb_state2.var_6) == (0x2c6389f8)) ? (rb_state2.var_7) : (0x50a4156d));
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (0x8e7b4d44);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_1);
        if ((rb_state18.var_1) == (0x9d4f4268)) {
            if ((rb_state18.var_2) != (0x0)) {
                if (!((rb_state18.var_2) == (rb_state18.var_42))) {
                    racebench_trigger(18);
                }
            }
        }
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
        rb_state0.var_2 = (rb_state0.var_2) ^ (78672 < rb_input_size ? (uint32_t)rb_input[78672] : 0xdd9a68cc);
        rb_state0.var_2 = (rb_state0.var_2) - (((rb_state0.var_1) == (0x545a975b)) ? (rb_state0.var_0) : (0x4aa2c5f1));
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_2) == (0x1c08420)) {
            rb_state1.var_4 = 0x50a84bba;
            rb_state1.var_5 = (rb_state1.var_5) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_4) : (0x3f256249));
            rb_state1.var_6 = (rb_state1.var_6) + (((0x8cc3303c) + (0x89a4bbb5)) - (rb_state1.var_3));
            rb_state1.var_7 = (rb_state1.var_7) - ((0x4f655797) - (0x8ea4c177));
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_5) : (0x17de914b));
            rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x23841dba));
            rb_state1.var_10 = (rb_state1.var_10) ^ (((((0xb9484a8f) - (rb_state1.var_8)) - (rb_state1.var_7)) ^ (rb_state1.var_8)) + (rb_state1.var_9));
            rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_9) : (0xf1a3ea0f));
            rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0xf1cffdd7));
            rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xb25cd16b));
        rb_state2.var_1 = (rb_state2.var_1) + (104895 < rb_input_size ? (uint32_t)rb_input[104895] : 0xb07802b6);
        rb_state2.var_1 = (rb_state2.var_1) - (0x6e4126e4);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - ((0xe64a0131) + (rb_state5.var_0));
        rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0x19b5fecf)) ? (rb_state5.var_1) : (0x3c8b064));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
        rb_state6.var_1 = (rb_state6.var_1) ^ (24099 < rb_input_size ? (uint32_t)rb_input[24099] : 0x40dda07a);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (12016 < rb_input_size ? (uint32_t)rb_input[12016] : 0xd51d18a3);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (12236 < rb_input_size ? (uint32_t)rb_input[12236] : 0x94fc12e3);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (140312 < rb_input_size ? (uint32_t)rb_input[140312] : 0x398a5a33);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ ((0x82b7d73e) + (rb_state11.var_0));
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_1);
        rb_state11.var_0 = (rb_state11.var_0) + ((0x1ce9594e) + (0x2116d91d));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - ((0xa6c4650b) + (0x6444cddb));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
        rb_state14.var_1 = (rb_state14.var_1) ^ (107890 < rb_input_size ? (uint32_t)rb_input[107890] : 0x75f4f7a0);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_2) == (0x204cdfae)) {
            rb_state16.var_11 = 0xe3f64fd3;
            rb_state16.var_12 = (rb_state16.var_12) + (((0xc4dfb957) + (rb_state16.var_11)) + (0xea8ef945));
            rb_state16.var_13 = (rb_state16.var_13) ^ (0xbcf35202);
            rb_state16.var_14 = (rb_state16.var_14) ^ (rb_state16.var_3);
            rb_state16.var_15 = (rb_state16.var_15) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_12) : (0xaaffc6e4));
            rb_state16.var_16 = (rb_state16.var_16) - (rb_state16.var_13);
            rb_state16.var_17 = (rb_state16.var_17) ^ (((0x37c9207f) - (0x7e74e0b5)) - (rb_state16.var_14));
            rb_state16.var_18 = (rb_state16.var_18) + (((0x8b14b2b) ^ (0xf695d519)) ^ (rb_state16.var_15));
            rb_state16.var_19 = (rb_state16.var_19) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_16) : (0xf8b46152));
            rb_state16.var_20 = (rb_state16.var_20) - (((0x6de26497) + (0xfafae6c7)) - (rb_state16.var_17));
            rb_state16.var_21 = (rb_state16.var_21) + (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_18) : (0xe69a4747));
            rb_state16.var_22 = (rb_state16.var_22) - (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_20) : (0x2d1a336d));
            rb_state16.var_23 = (rb_state16.var_23) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_21) : (0x85f19cd3));
            rb_state16.var_24 = (rb_state16.var_24) ^ (((0x18e9d460) ^ (rb_state16.var_22)) ^ (rb_state16.var_8));
            rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_23);
            rb_state16.var_26 = (rb_state16.var_26) ^ (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_25) : (0xe6887da6));
            rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_26);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (75389 < rb_input_size ? (uint32_t)rb_input[75389] : 0xd7df9a3f);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (0xc3daa21a);
        rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x3c255de6)) ? (rb_state18.var_1) : (0x1b64b378));
        if ((rb_state18.var_1) == (0x9d4f4268)) {
            rb_state18.var_21 = 0xab245e51;
            rb_state18.var_22 = 0x505cd1cf;
            rb_state18.var_23 = (rb_state18.var_23) - (((0x2507cd02) ^ (0xb89ee7aa)) - (rb_state18.var_22));
            rb_state18.var_24 = (rb_state18.var_24) + (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_12) : (0x1844c6bf));
            rb_state18.var_25 = (rb_state18.var_25) ^ (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_23) : (0xd697857d));
            rb_state18.var_26 = (rb_state18.var_26) + (((0x9cbe8c71) + (rb_state18.var_24)) - (rb_state18.var_14));
            rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_25) : (0xf538b8b6));
            rb_state18.var_28 = (rb_state18.var_28) ^ (((0xed9c946b) ^ (rb_state18.var_26)) - (rb_state18.var_16));
            rb_state18.var_29 = (rb_state18.var_29) + (((rb_state18.var_17) == (0x0)) ? (rb_state18.var_27) : (0x8499079d));
            rb_state18.var_30 = (rb_state18.var_30) ^ (((0x6c76633a) - (rb_state18.var_18)) + (rb_state18.var_28));
            rb_state18.var_31 = (rb_state18.var_31) ^ (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_29) : (0xcc169db9));
            rb_state18.var_32 = (rb_state18.var_32) + (rb_state18.var_30);
            rb_state18.var_33 = (rb_state18.var_33) ^ (((0x190f2846) ^ (rb_state18.var_20)) + (rb_state18.var_31));
            rb_state18.var_34 = (rb_state18.var_34) + (rb_state18.var_32);
            rb_state18.var_35 = (rb_state18.var_35) + (((0xf39dda64) + (rb_state18.var_21)) ^ (rb_state18.var_33));
            rb_state18.var_36 = (rb_state18.var_36) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_34) : (0x2b83ef33));
            rb_state18.var_37 = (rb_state18.var_37) ^ (((0x17342e9a) + (0x26c71c45)) ^ (rb_state18.var_35));
            rb_state18.var_38 = (rb_state18.var_38) + (rb_state18.var_36);
            rb_state18.var_39 = (rb_state18.var_39) + (((0x9619cc10) - (0x39d91b9c)) ^ (rb_state18.var_37));
            rb_state18.var_40 = (rb_state18.var_40) - (((0xee91f734) ^ (0xb3987013)) - (rb_state18.var_38));
            rb_state18.var_41 = (rb_state18.var_41) ^ (((rb_state18.var_39) == (0x0)) ? (rb_state18.var_40) : (0x9f05238d));
            rb_state18.var_20 = (rb_state18.var_20) + (rb_state18.var_41);
            rb_state18.var_2 = rb_state18.var_20;
        }
        if ((rb_state18.var_1) == (0x9d4f4268)) {
            pthread_mutex_lock(&(rb_state18.lock_60));
            rb_state18.var_43 = 0xb4881487;
            rb_state18.var_44 = 0xeb8a977a;
            rb_state18.var_45 = (rb_state18.var_45) + (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_23) : (0xcf38ab94));
            rb_state18.var_46 = (rb_state18.var_46) + (0x22491ea1);
            rb_state18.var_47 = (rb_state18.var_47) ^ (((rb_state18.var_25) == (0x0)) ? (rb_state18.var_43) : (0xe9a52b09));
            rb_state18.var_48 = (rb_state18.var_48) + (((((0x24573cd2) + (rb_state18.var_44)) + (rb_state18.var_27)) - (rb_state18.var_26)) + (rb_state18.var_45));
            rb_state18.var_49 = (rb_state18.var_49) + (rb_state18.var_46);
            rb_state18.var_50 = (rb_state18.var_50) - (((rb_state18.var_28) == (0x0)) ? (rb_state18.var_47) : (0x45d9458a));
            rb_state18.var_51 = (rb_state18.var_51) + (rb_state18.var_48);
            rb_state18.var_52 = (rb_state18.var_52) + (((rb_state18.var_29) == (0x0)) ? (rb_state18.var_49) : (0xfede5c35));
            rb_state18.var_53 = (rb_state18.var_53) - (((((0x8f762774) + (rb_state18.var_51)) + (0xd559451a)) ^ (rb_state18.var_50)) - (0xffa0420b));
            rb_state18.var_54 = (rb_state18.var_54) ^ (((0x67a4c391) + (rb_state18.var_30)) ^ (rb_state18.var_52));
            rb_state18.var_55 = (rb_state18.var_55) - (((0x4e6620b2) + (rb_state18.var_53)) - (rb_state18.var_31));
            rb_state18.var_56 = (rb_state18.var_56) + (((rb_state18.var_32) == (0x0)) ? (rb_state18.var_54) : (0xd4d246a8));
            rb_state18.var_57 = (rb_state18.var_57) - (rb_state18.var_55);
            rb_state18.var_58 = (rb_state18.var_58) - (((0xb9c80aa0) ^ (rb_state18.var_56)) + (rb_state18.var_33));
            rb_state18.var_59 = (rb_state18.var_59) - (((((0xee44e2d4) ^ (rb_state18.var_58)) + (rb_state18.var_34)) - (0x75a7b858)) + (rb_state18.var_57));
            rb_state18.var_42 = (rb_state18.var_42) ^ (rb_state18.var_59);
            rb_state18.var_2 = rb_state18.var_42;
            pthread_mutex_unlock(&(rb_state18.lock_60));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (0x7ded5027);
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_22));
        rb_state0.var_5 = 0x1b0f8607;
        rb_state0.var_6 = 0x9009b34;
        rb_state0.var_7 = (rb_state0.var_7) ^ (0xd0bea079);
        rb_state0.var_8 = (rb_state0.var_8) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_3) : (0xa6ea4b97));
        rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_7) : (0x270b2d75));
        rb_state0.var_10 = (rb_state0.var_10) - (((((0xd6c9af9c) + (rb_state0.var_5)) - (rb_state0.var_4)) - (rb_state0.var_9)) - (rb_state0.var_8));
        rb_state0.var_4 = (rb_state0.var_4) - (rb_state0.var_10);
        rb_state0.var_3 = rb_state0.var_4;
        pthread_mutex_unlock(&(rb_state0.lock_22));
    }
    if ((rb_state0.var_0) == (0x0)) {
        if (!((rb_state0.var_3) == (rb_state0.var_4))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_1) : (0xd01b4b58));
    if ((rb_state3.var_1) == (0xf62d9fc5)) {
        rb_state3.var_2 = rb_state3.var_0;
    }
    if ((rb_state3.var_1) == (0xf62d9fc5)) {
        if (!((rb_state3.var_0) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (0x1183ac94);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (88241 < rb_input_size ? (uint32_t)rb_input[88241] : 0xb1e76358);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (0x31585ecc);
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + ((0x7d9b0a95) + (0x94d387c));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ ((0x9f30209f) - (0x26cee1c6));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x9c14702c));
    rb_state6.var_0 = (rb_state6.var_0) + (0x3a680232);
    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_2);
    if ((rb_state6.var_1) == (0x31)) {
        rb_state6.var_3 = rb_state6.var_2;
    }
    if ((rb_state6.var_1) == (0x31)) {
        if (!((rb_state6.var_2) == (rb_state6.var_3))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (5395 < rb_input_size ? (uint32_t)rb_input[5395] : 0x90bfd860);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_1) == (0x1183ac94)) ? (rb_state8.var_0) : (0xf33d9bc6));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (160846 < rb_input_size ? (uint32_t)rb_input[160846] : 0x496f4e62);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xffffff8e)) {
        pthread_mutex_lock(&(rb_state11.lock_27));
        rb_state11.var_12 = 0x5940c28f;
        rb_state11.var_13 = 0x3efc5703;
        rb_state11.var_14 = 0x5654fac6;
        rb_state11.var_15 = (rb_state11.var_15) + ((0x9a3fa43) ^ (rb_state11.var_7));
        rb_state11.var_16 = (rb_state11.var_16) ^ (((((0xec8fe29d) - (rb_state11.var_9)) ^ (rb_state11.var_12)) - (rb_state11.var_8)) ^ (rb_state11.var_14));
        rb_state11.var_17 = (rb_state11.var_17) + (rb_state11.var_13);
        rb_state11.var_18 = (rb_state11.var_18) ^ (((0x20be8cb5) + (rb_state11.var_15)) - (0xf1b45804));
        rb_state11.var_19 = (rb_state11.var_19) ^ (rb_state11.var_16);
        rb_state11.var_20 = (rb_state11.var_20) + (rb_state11.var_17);
        rb_state11.var_21 = (rb_state11.var_21) + (((0x67f9e34c) + (rb_state11.var_10)) + (rb_state11.var_18));
        rb_state11.var_22 = (rb_state11.var_22) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0xea54a57b));
        rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_21) : (0x773df7e0));
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_22) : (0xad94babc));
        rb_state11.var_25 = (rb_state11.var_25) - (((0x8a9525dd) ^ (rb_state11.var_23)) - (rb_state11.var_13));
        rb_state11.var_26 = (rb_state11.var_26) ^ (((((0x7320e04b) + (rb_state11.var_24)) - (rb_state11.var_14)) ^ (0x5f1f3357)) - (rb_state11.var_25));
        rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_26);
        pthread_mutex_unlock(&(rb_state11.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_1) == (0xe749aee5)) {
        rb_state12.var_8 = (rb_state12.var_8) - (((((0xbc4f1444) - (rb_state12.var_7)) + (0xa4590322)) + (0xe195ded)) - (rb_state12.var_5));
        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_6);
        rb_state12.var_10 = (rb_state12.var_10) + (3484 < rb_input_size ? (uint32_t)rb_input[3484] : 0x873c54fe);
        rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_8) : (0x1e49c7a4));
        rb_state12.var_12 = (rb_state12.var_12) + (rb_state12.var_8);
        rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_9) : (0xc36799d));
        rb_state12.var_14 = (rb_state12.var_14) + (((0xc78283a) + (rb_state12.var_11)) ^ (rb_state12.var_10));
        rb_state12.var_15 = (rb_state12.var_15) ^ (rb_state12.var_11);
        rb_state12.var_16 = (rb_state12.var_16) ^ (((0xe84e9914) + (rb_state12.var_12)) - (0x42a1209e));
        rb_state12.var_17 = (rb_state12.var_17) ^ (((0x3bd741b2) - (rb_state12.var_13)) ^ (rb_state12.var_12));
        rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_14) : (0x98c109a1));
        rb_state12.var_19 = (rb_state12.var_19) - (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0x4067004e));
        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_17) : (0xf2ac0af7));
        rb_state12.var_21 = (rb_state12.var_21) + (((0x8c149799) ^ (rb_state12.var_15)) + (rb_state12.var_18));
        rb_state12.var_22 = (rb_state12.var_22) ^ (((((0xa872c2c3) ^ (rb_state12.var_19)) ^ (rb_state12.var_20)) ^ (0x7b519939)) ^ (0xcd7441fe));
        rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_21) : (0x1edb45f0));
        rb_state12.var_24 = (rb_state12.var_24) - (rb_state12.var_22);
        rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_23);
        rb_state12.var_26 = (rb_state12.var_26) - (((((0xc6d483e1) ^ (rb_state12.var_18)) ^ (rb_state12.var_24)) + (rb_state12.var_17)) ^ (rb_state12.var_25));
        rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (48566 < rb_input_size ? (uint32_t)rb_input[48566] : 0x6d060f7b);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (0x5137d30e);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (0xbbeedfea);
    if ((rb_state17.var_1) == (0xb4666a96)) {
        rb_state17.var_2 = rb_state17.var_0;
    }
    if ((rb_state17.var_1) == (0xb4666a96)) {
        if (!((rb_state17.var_0) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (0xe1d9277);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) ^ (0xbecc09c9);
    rb_state1.var_1 = (rb_state1.var_1) - (168592 < rb_input_size ? (uint32_t)rb_input[168592] : 0x25d356a4);
    if ((rb_state1.var_2) == (0x1c08420)) {
        rb_state1.var_3 = rb_state1.var_0;
    }
    if ((rb_state1.var_2) == (0x1c08420)) {
        pthread_mutex_lock(&(rb_state1.lock_13));
        if (!((rb_state1.var_0) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
        pthread_mutex_unlock(&(rb_state1.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (104911 < rb_input_size ? (uint32_t)rb_input[104911] : 0x435e2719);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0x9d67c854));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (42538 < rb_input_size ? (uint32_t)rb_input[42538] : 0xe6a21b91);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (42412 < rb_input_size ? (uint32_t)rb_input[42412] : 0x72af834b);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_10 = 0x8fb15421;
        rb_state14.var_11 = 0x8c380a24;
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_4) : (0x12acf000));
        rb_state14.var_13 = (rb_state14.var_13) ^ (((((0x512439e1) ^ (rb_state14.var_6)) - (rb_state14.var_11)) ^ (rb_state14.var_10)) + (0xd9eb3377));
        rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_13) : (0xe8791617));
        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x204cdfae)) {
        pthread_mutex_lock(&(rb_state16.lock_27));
        rb_state16.var_5 = 0x8898d1ae;
        rb_state16.var_6 = 0x898efc3c;
        rb_state16.var_7 = 0xf441823b;
        rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_7) == (0xf441823b)) ? (rb_state16.var_6) : (0x834afea3));
        rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_5);
        rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_8) == (0x898efc3c)) ? (rb_state16.var_9) : (0x909d55f));
        rb_state16.var_4 = (rb_state16.var_4) - (rb_state16.var_10);
        rb_state16.var_3 = rb_state16.var_4;
        pthread_mutex_unlock(&(rb_state16.lock_27));
    }
    if ((rb_state16.var_2) == (0x204cdfae)) {
        if (!((rb_state16.var_3) == (rb_state16.var_4))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x2342a3b7);
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_2);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x419e07ec));
    rb_state3.var_1 = (rb_state3.var_1) + (0xdccc1227);
    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_0);
    if ((rb_state5.var_0) == (0x19b5fe6d)) {
        pthread_mutex_lock(&(rb_state5.lock_20));
        rb_state5.var_4 = 0x72997795;
        rb_state5.var_5 = (rb_state5.var_5) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_4) : (0x7ff756a6));
        rb_state5.var_6 = (rb_state5.var_6) + (0xfb78e940);
        rb_state5.var_7 = (rb_state5.var_7) - (((((0x1cb8da54) ^ (rb_state5.var_5)) - (rb_state5.var_4)) + (rb_state5.var_3)) + (rb_state5.var_5));
        rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_6) : (0xc41d6342));
        rb_state5.var_9 = (rb_state5.var_9) ^ (((((0x366b4e4d) - (rb_state5.var_7)) - (rb_state5.var_7)) ^ (rb_state5.var_8)) - (rb_state5.var_8));
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_9);
        rb_state5.var_2 = rb_state5.var_3;
        pthread_mutex_unlock(&(rb_state5.lock_20));
    }
    if ((rb_state5.var_0) == (0x19b5fe6d)) {
        if (!((rb_state5.var_2) == (rb_state5.var_3))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0x5c8b7d16));
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0xfffffecb)) {
        rb_state9.var_15 = 0x689eda8c;
        rb_state9.var_16 = (rb_state9.var_16) + (0x1acc3d0);
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_13) : (0x5247d923));
        rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_12) : (0x573fe89a));
        rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_15) : (0x4bf22ce9));
        rb_state9.var_20 = (rb_state9.var_20) - (((0xfc767a01) ^ (rb_state9.var_16)) - (rb_state9.var_16));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_17) : (0xf5accc71));
        rb_state9.var_22 = (rb_state9.var_22) ^ (((0x27e574c3) + (0x8e1f088c)) ^ (rb_state9.var_18));
        rb_state9.var_23 = (rb_state9.var_23) - (((((0x98f0a036) - (rb_state9.var_20)) + (rb_state9.var_19)) ^ (rb_state9.var_18)) ^ (rb_state9.var_19));
        rb_state9.var_24 = (rb_state9.var_24) + (((((0x14f6440b) - (rb_state9.var_21)) + (rb_state9.var_20)) - (rb_state9.var_21)) ^ (rb_state9.var_22));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0x4e281a46));
        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (126257 < rb_input_size ? (uint32_t)rb_input[126257] : 0x2665899d);
    rb_state11.var_0 = (rb_state11.var_0) - (0x4bcc8938);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (0x51ffd9ff);
    rb_state14.var_2 = (rb_state14.var_2) ^ (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_2) : (0x64e0eef8));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (99761 < rb_input_size ? (uint32_t)rb_input[99761] : 0x43ace00d);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        rb_state0.var_11 = 0xac735277;
        rb_state0.var_12 = 0x1341e472;
        rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_8) == (0xa6ea4b97)) ? (rb_state0.var_7) : (0xa8361636));
        rb_state0.var_14 = (rb_state0.var_14) - (114318 < rb_input_size ? (uint32_t)rb_input[114318] : 0x7f9a025b);
        rb_state0.var_15 = (rb_state0.var_15) ^ (((rb_state0.var_6) == (0x9009b34)) ? (rb_state0.var_12) : (0xf5dc6522));
        rb_state0.var_16 = (rb_state0.var_16) ^ (rb_state0.var_11);
        rb_state0.var_17 = (rb_state0.var_17) + (((((0x76353406) - (rb_state0.var_9)) + (rb_state0.var_13)) + (rb_state0.var_14)) + (rb_state0.var_10));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((rb_state0.var_15) == (0x1341e472)) ? (rb_state0.var_16) : (0x52b7c8bb));
        rb_state0.var_19 = (rb_state0.var_19) + (((0x130f08a6) - (rb_state0.var_17)) ^ (0x2e904a96));
        rb_state0.var_20 = (rb_state0.var_20) - (((rb_state0.var_11) == (0xac735277)) ? (rb_state0.var_18) : (0xfebc3d47));
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_19) == (0x961715c3)) ? (rb_state0.var_20) : (0xb63d68a2));
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0xd6b316dc));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ (0xee8569c9);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (104079 < rb_input_size ? (uint32_t)rb_input[104079] : 0x9e4f51b);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (37143 < rb_input_size ? (uint32_t)rb_input[37143] : 0xf109da1);
    rb_state7.var_2 = (rb_state7.var_2) - (0xcc6ba959);
    if ((rb_state7.var_2) == (0x9abd0395)) {
        rb_state7.var_3 = rb_state7.var_1;
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xffffff8e)) {
        pthread_mutex_lock(&(rb_state11.lock_27));
        rb_state11.var_4 = 0xc9e2c8a7;
        rb_state11.var_5 = 0x58549356;
        rb_state11.var_6 = (rb_state11.var_6) ^ (((0xdf74ebd4) + (rb_state11.var_2)) ^ (rb_state11.var_3));
        rb_state11.var_7 = (rb_state11.var_7) - (((0xb9cecfe6) - (rb_state11.var_4)) - (rb_state11.var_5));
        rb_state11.var_8 = (rb_state11.var_8) ^ (rb_state11.var_4);
        rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0x93e1f6df));
        rb_state11.var_10 = (rb_state11.var_10) - (((((0x79a7ea36) - (rb_state11.var_8)) ^ (rb_state11.var_7)) - (0x48b1a46b)) + (rb_state11.var_6));
        rb_state11.var_11 = (rb_state11.var_11) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_10) : (0xe846adc0));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_11);
        rb_state11.var_2 = rb_state11.var_3;
        pthread_mutex_unlock(&(rb_state11.lock_27));
    }
    if ((rb_state11.var_1) == (0xffffff8e)) {
        rb_state11.var_28 = !((rb_state11.var_2) == (rb_state11.var_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0xe749aed4);
    rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_2) == (0xffffff92)) ? (rb_state12.var_1) : (0x65b08f02));
    rb_state12.var_1 = (rb_state12.var_1) ^ (136120 < rb_input_size ? (uint32_t)rb_input[136120] : 0x6694b84);
    if ((rb_state12.var_1) == (0xe749aee5)) {
        pthread_mutex_lock(&(rb_state12.lock_27));
        rb_state12.var_5 = 0x7539d4c3;
        rb_state12.var_6 = (rb_state12.var_6) + (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_5) : (0xad62ae9b));
        rb_state12.var_7 = (rb_state12.var_7) + (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_6) : (0xd5ac828a));
        rb_state12.var_4 = (rb_state12.var_4) ^ (rb_state12.var_7);
        rb_state12.var_3 = rb_state12.var_4;
        pthread_mutex_unlock(&(rb_state12.lock_27));
    }
    if ((rb_state12.var_1) == (0xe749aee5)) {
        if (!((rb_state12.var_3) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_15));
        rb_state14.var_5 = 0x8163104e;
        rb_state14.var_6 = 0x94b2315a;
        rb_state14.var_7 = (rb_state14.var_7) - (((0xe73677ad) + (0x318666da)) ^ (rb_state14.var_5));
        rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_6) == (0x94b2315a)) ? (rb_state14.var_3) : (0x37b7bc35));
        rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_7) == (0x66203137)) ? (rb_state14.var_8) : (0x9aa957e8));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_9);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_15));
    }
    if ((rb_state14.var_0) == (0x0)) {
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (96157 < rb_input_size ? (uint32_t)rb_input[96157] : 0x210b61f4);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x20)) ? (rb_state19.var_0) : (0xc035f4d3));
    rb_state19.var_0 = (rb_state19.var_0) + (57023 < rb_input_size ? (uint32_t)rb_input[57023] : 0x3079bc33);
    if ((rb_state19.var_1) == (0x56855400)) {
        rb_state19.var_4 = 0xdb351cc5;
        rb_state19.var_5 = (rb_state19.var_5) ^ (rb_state19.var_3);
        rb_state19.var_6 = (rb_state19.var_6) - ((0xafe08471) ^ (rb_state19.var_5));
        rb_state19.var_7 = (rb_state19.var_7) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_6) : (0x2f1c4272));
        rb_state19.var_8 = (rb_state19.var_8) + (((((0xc9a65ddb) + (rb_state19.var_8)) - (rb_state19.var_4)) ^ (rb_state19.var_5)) - (0xac050741));
        rb_state19.var_9 = (rb_state19.var_9) ^ (rb_state19.var_6);
        rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_7) : (0x77a45c74));
        rb_state19.var_11 = (rb_state19.var_11) + (((((0x185b366) + (rb_state19.var_10)) ^ (rb_state19.var_9)) - (rb_state19.var_11)) + (rb_state19.var_8));
        rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_10) : (0xda8987));
        rb_state19.var_13 = (rb_state19.var_13) - (rb_state19.var_11);
        rb_state19.var_14 = (rb_state19.var_14) + (((((0x25ab527d) - (0x93983363)) - (rb_state19.var_12)) - (0x5f9315a0)) ^ (rb_state19.var_13));
        rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_14);
        rb_state19.var_2 = rb_state19.var_3;
    }
    if ((rb_state19.var_1) == (0x56855400)) {
        pthread_mutex_lock(&(rb_state19.lock_27));
        rb_state19.var_16 = 0x266d9d72;
        rb_state19.var_17 = 0x9d2789f1;
        rb_state19.var_18 = (rb_state19.var_18) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_17) : (0xb8b73605));
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_16) : (0x78641d2a));
        rb_state19.var_20 = (rb_state19.var_20) ^ (((0xa5b98f3) ^ (0x4d602d57)) + (rb_state19.var_13));
        rb_state19.var_21 = (rb_state19.var_21) ^ (((0x359badb1) ^ (rb_state19.var_16)) ^ (rb_state19.var_18));
        rb_state19.var_22 = (rb_state19.var_22) ^ (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_20) : (0xf4ed8087));
        rb_state19.var_23 = (rb_state19.var_23) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_21) : (0x769ed566));
        rb_state19.var_24 = (rb_state19.var_24) ^ (rb_state19.var_22);
        rb_state19.var_25 = (rb_state19.var_25) + (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_23) : (0x3f3517fd));
        rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_25) : (0x32f405ad));
        rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_26);
        rb_state19.var_2 = rb_state19.var_15;
        pthread_mutex_unlock(&(rb_state19.lock_27));
    }
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_3 = rb_state2.var_1;
    }
    if ((rb_state2.var_0) == (0x0)) {
        if (!((rb_state2.var_1) == (rb_state2.var_3))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (87194 < rb_input_size ? (uint32_t)rb_input[87194] : 0x3f91aee2);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0x9abd0395)) {
        if (!((rb_state7.var_1) == (rb_state7.var_3))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_15 = (rb_state11.var_15) - ((0xb117b254) - (rb_state11.var_16));
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (68088 < rb_input_size ? (uint32_t)rb_input[68088] : 0xec34ca6c);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (0x4538b240);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_2) : (0x14f5860c));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_1);
    #endif
    PreAllocateFO(MyNum, lc);

    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state4.lock_22));
            rb_state4.var_4 = 0xa2272b38;
            rb_state4.var_5 = 0xb448e301;
            rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x6e0404aa));
            rb_state4.var_7 = (rb_state4.var_7) ^ (rb_state4.var_5);
            rb_state4.var_8 = (rb_state4.var_8) ^ (((0x1712f0d7) + (0x102c6c95)) + (rb_state4.var_3));
            rb_state4.var_9 = (rb_state4.var_9) - (((0xf8682dd4) + (0x53fb6dfe)) ^ (rb_state4.var_4));
            rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_6);
            rb_state4.var_11 = (rb_state4.var_11) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_8) : (0xde524e8b));
            rb_state4.var_12 = (rb_state4.var_12) + (rb_state4.var_9);
            rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0xbc92bde6));
            rb_state4.var_14 = (rb_state4.var_14) + (((0xb58633de) - (0xfd424c31)) + (rb_state4.var_12));
            rb_state4.var_15 = (rb_state4.var_15) - (((0x6c9ecdca) - (rb_state4.var_13)) - (0x8336eaea));
            rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_14);
            rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_15) : (0x7dc2df80));
            rb_state4.var_18 = (rb_state4.var_18) + (rb_state4.var_16);
            rb_state4.var_19 = (rb_state4.var_19) + (rb_state4.var_17);
            rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_18) : (0x30a7bd07));
            rb_state4.var_21 = (rb_state4.var_21) + (((((0xdf04da1d) ^ (0x7fec82f8)) - (rb_state4.var_19)) ^ (rb_state4.var_20)) - (0xf7e5e067));
            rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_21);
            pthread_mutex_unlock(&(rb_state4.lock_22));
        }
        #endif
        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_2) == (0x0)) {
            rb_state4.var_3 = rb_state4.var_0;
        }
        if ((rb_state4.var_2) == (0x0)) {
            if (!((rb_state4.var_0) == (rb_state4.var_3))) {
                racebench_trigger(4);
            }
        }
        #endif
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