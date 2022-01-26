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
        if ((rb_state5.var_0) == (0xf479443a)) {
            pthread_mutex_lock(&(rb_state5.lock_7));
            rb_state5.var_2 = rb_state5.var_1;
            pthread_mutex_unlock(&(rb_state5.lock_7));
        }
        if ((rb_state5.var_0) == (0x5dd755d4)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
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
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - ((0x8024569c) ^ (0x866fd32f));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0x14a29640);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x695e119a));
        if ((rb_state5.var_0) == (0x5dd755d4)) {
            rb_state5.var_3 = 0x505f2b72;
            rb_state5.var_4 = 0x85caf40b;
            rb_state5.var_5 = (rb_state5.var_5) - (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0xf0a2db2c));
            rb_state5.var_6 = (rb_state5.var_6) ^ (((((0xfc3b1d37) + (rb_state5.var_5)) + (0x84938882)) + (0x6a854ff0)) ^ (rb_state5.var_2));
            rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x515ca2a3));
        rb_state6.var_0 = (rb_state6.var_0) - (149796 < rb_input_size ? (uint32_t)rb_input[149796] : 0x38708d9c);
        if ((rb_state6.var_0) == (0xaea35ced)) {
            rb_state6.var_17 = 0xb3be807f;
            rb_state6.var_18 = (rb_state6.var_18) ^ (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_8) : (0xc673eb97));
            rb_state6.var_19 = (rb_state6.var_19) ^ ((0x2071e634) + (0xc6e1deba));
            rb_state6.var_20 = (rb_state6.var_20) + ((0xd71b430) ^ (0xea34f673));
            rb_state6.var_21 = (rb_state6.var_21) ^ (((((0xfb5996a4) - (rb_state6.var_18)) - (0xa894c1a)) - (rb_state6.var_9)) ^ (rb_state6.var_7));
            rb_state6.var_22 = (rb_state6.var_22) ^ (rb_state6.var_19);
            rb_state6.var_23 = (rb_state6.var_23) + (((((0xe350796f) - (0x88b8d622)) + (0x1cc852e5)) ^ (rb_state6.var_21)) - (rb_state6.var_20));
            rb_state6.var_24 = (rb_state6.var_24) - (rb_state6.var_22);
            rb_state6.var_25 = (rb_state6.var_25) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_23) : (0xc61cd61a));
            rb_state6.var_26 = (rb_state6.var_26) ^ (((0xc44bf876) ^ (0x574e2f29)) + (rb_state6.var_24));
            rb_state6.var_27 = (rb_state6.var_27) ^ (rb_state6.var_25);
            rb_state6.var_28 = (rb_state6.var_28) - (((rb_state6.var_26) == (0x0)) ? (rb_state6.var_27) : (0xa636a765));
            rb_state6.var_16 = (rb_state6.var_16) ^ (rb_state6.var_28);
            rb_state6.var_1 = rb_state6.var_16;
        }
        if ((rb_state6.var_0) == (0xaea35ced)) {
            pthread_mutex_lock(&(rb_state6.lock_53));
            rb_state6.var_30 = 0x5ccdc22a;
            rb_state6.var_31 = 0xbdccf5f5;
            rb_state6.var_32 = (rb_state6.var_32) - ((0xe6f2a004) ^ (0x2d4339c4));
            rb_state6.var_33 = (rb_state6.var_33) + (((((0x7f518099) ^ (0x2a07043c)) - (rb_state6.var_31)) + (rb_state6.var_11)) + (rb_state6.var_12));
            rb_state6.var_34 = (rb_state6.var_34) ^ (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_30) : (0xe2ebd391));
            rb_state6.var_35 = (rb_state6.var_35) - (rb_state6.var_32);
            rb_state6.var_36 = (rb_state6.var_36) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_33) : (0x6dc3be18));
            rb_state6.var_37 = (rb_state6.var_37) + (((0xfea0ca38) - (rb_state6.var_34)) ^ (0xa229f4a6));
            rb_state6.var_38 = (rb_state6.var_38) - (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_35) : (0x29e3915d));
            rb_state6.var_39 = (rb_state6.var_39) + (rb_state6.var_36);
            rb_state6.var_40 = (rb_state6.var_40) ^ (((rb_state6.var_37) == (0x0)) ? (rb_state6.var_38) : (0x783308d6));
            rb_state6.var_41 = (rb_state6.var_41) ^ (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_39) : (0x897e9c02));
            rb_state6.var_42 = (rb_state6.var_42) - (((0x222ca9a6) - (0xbcf6d68c)) - (rb_state6.var_40));
            rb_state6.var_43 = (rb_state6.var_43) ^ (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_41) : (0x6219128c));
            rb_state6.var_44 = (rb_state6.var_44) ^ (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_42) : (0x10f9755a));
            rb_state6.var_45 = (rb_state6.var_45) ^ (rb_state6.var_43);
            rb_state6.var_46 = (rb_state6.var_46) ^ (((0xcaee66b8) ^ (0x92f62d25)) + (rb_state6.var_44));
            rb_state6.var_47 = (rb_state6.var_47) ^ (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_45) : (0xb375447));
            rb_state6.var_48 = (rb_state6.var_48) - (((0xd855c683) + (0x35b60914)) ^ (rb_state6.var_46));
            rb_state6.var_49 = (rb_state6.var_49) ^ (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_47) : (0xfe50ea3d));
            rb_state6.var_50 = (rb_state6.var_50) - (((rb_state6.var_21) == (0x0)) ? (rb_state6.var_48) : (0x23ee058a));
            rb_state6.var_51 = (rb_state6.var_51) + (((0xc4fbd4db) ^ (rb_state6.var_22)) + (rb_state6.var_49));
            rb_state6.var_52 = (rb_state6.var_52) - (((rb_state6.var_50) == (0x0)) ? (rb_state6.var_51) : (0x8c6dc189));
            rb_state6.var_29 = (rb_state6.var_29) ^ (rb_state6.var_52);
            rb_state6.var_1 = rb_state6.var_29;
            pthread_mutex_unlock(&(rb_state6.lock_53));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ ((0xa50b7d09) - (rb_state9.var_0));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xaea35ced)) {
        pthread_mutex_lock(&(rb_state6.lock_53));
        if ((rb_state6.var_1) != (0x0)) {
            if (!((rb_state6.var_1) == (rb_state6.var_29))) {
                racebench_trigger(6);
            }
        }
        pthread_mutex_unlock(&(rb_state6.lock_53));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x9cacdfa4)) {
        rb_state7.var_13 = 0x2c1865a9;
        rb_state7.var_14 = 0x4ab53f5b;
        rb_state7.var_15 = (rb_state7.var_15) + (((((0x926f148a) + (rb_state7.var_4)) ^ (0x79f0ed07)) ^ (rb_state7.var_3)) - (rb_state7.var_14));
        rb_state7.var_16 = (rb_state7.var_16) - (((((0xc1dea12e) ^ (rb_state7.var_5)) - (rb_state7.var_13)) + (rb_state7.var_6)) ^ (rb_state7.var_15));
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_16);
    }
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (46922 < rb_input_size ? (uint32_t)rb_input[46922] : 0x28570ea5);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (0x4e40cfdb);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x789eadd3));
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x70816c3b);
    rb_state7.var_0 = (rb_state7.var_0) - (0x919d21af);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x9c819fd6)) {
        rb_state16.var_3 = 0x37d21c0e;
        rb_state16.var_4 = (rb_state16.var_4) + (148147 < rb_input_size ? (uint32_t)rb_input[148147] : 0xcb0966);
        rb_state16.var_5 = (rb_state16.var_5) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_3) : (0xa2d509a5));
        rb_state16.var_6 = (rb_state16.var_6) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_4) : (0x98afe462));
        rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_5) : (0xf8a8f120));
        rb_state16.var_8 = (rb_state16.var_8) - (((((0xdfa81a88) ^ (0x2ec34a97)) + (rb_state16.var_6)) ^ (0x5cedfc2)) ^ (rb_state16.var_7));
        rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_8);
    }
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0xa50b7d09)) ? (rb_state9.var_0) : (0x2d88be2a));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x9c79f4fb);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x7758c23f)) {
        if ((rb_state17.var_1) != (0x0)) {
            if (!((rb_state17.var_1) == (rb_state17.var_15))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0xfbe167c1) ^ (0x42c7e684));
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (171101 < rb_input_size ? (uint32_t)rb_input[171101] : 0xaf54b306);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0xf479443a) ^ (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x9cacdfa4)) {
        pthread_mutex_lock(&(rb_state7.lock_17));
        rb_state7.var_3 = 0x41e0f7ed;
        rb_state7.var_4 = 0x8d88cc47;
        rb_state7.var_5 = 0x74090b28;
        rb_state7.var_6 = (rb_state7.var_6) - (((0x90c0bbe5) ^ (0x32619c89)) + (rb_state7.var_4));
        rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_3);
        rb_state7.var_8 = (rb_state7.var_8) + (((((0x98e96a97) ^ (rb_state7.var_6)) + (0x2b29141f)) - (0xa0061f71)) + (rb_state7.var_5));
        rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_7) : (0x3dec6412));
        rb_state7.var_10 = (rb_state7.var_10) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_8) : (0x7f17b98e));
        rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_9);
        rb_state7.var_12 = (rb_state7.var_12) + (((((0xa4ae37df) + (0xbac61b2b)) - (rb_state7.var_11)) ^ (0x57b9badf)) + (rb_state7.var_10));
        rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_12);
        rb_state7.var_1 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_17));
    }
    if ((rb_state7.var_0) == (0x9cacdfa4)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_37));
        rb_state1.var_26 = 0xe4de3d91;
        rb_state1.var_27 = (rb_state1.var_27) + (149768 < rb_input_size ? (uint32_t)rb_input[149768] : 0xf162a0);
        rb_state1.var_28 = (rb_state1.var_28) ^ (((((0x3a5a7fd0) - (0x1ef20dd4)) - (rb_state1.var_16)) - (0x25f9a8ee)) - (rb_state1.var_17));
        rb_state1.var_29 = (rb_state1.var_29) ^ (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_18) : (0x7b376b6e));
        rb_state1.var_30 = (rb_state1.var_30) - (((rb_state1.var_26) == (0x0)) ? (rb_state1.var_27) : (0xb9701d82));
        rb_state1.var_31 = (rb_state1.var_31) ^ (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_28) : (0x723b19ec));
        rb_state1.var_32 = (rb_state1.var_32) + (((0x8cc117f2) - (0x170e97f7)) + (rb_state1.var_29));
        rb_state1.var_33 = (rb_state1.var_33) - (((rb_state1.var_30) == (0x0)) ? (rb_state1.var_31) : (0x6691eaec));
        rb_state1.var_34 = (rb_state1.var_34) + (rb_state1.var_32);
        rb_state1.var_35 = (rb_state1.var_35) + (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_33) : (0xe84c6273));
        rb_state1.var_36 = (rb_state1.var_36) - (((rb_state1.var_34) == (0x0)) ? (rb_state1.var_35) : (0x9cbec3e1));
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_36);
        pthread_mutex_unlock(&(rb_state1.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0xe6d1e934)) ? (rb_state2.var_0) : (0x95c3864e));
    if ((rb_state2.var_0) == (0x6a3c79b2)) {
        pthread_mutex_lock(&(rb_state2.lock_12));
        rb_state2.var_3 = 0x50c77208;
        rb_state2.var_4 = (rb_state2.var_4) ^ (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_1) : (0x6981b743));
        rb_state2.var_5 = (rb_state2.var_5) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_3) : (0xb053df2c));
        rb_state2.var_6 = (rb_state2.var_6) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0xb3ac6c1e));
        rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_4) : (0x9310af14));
        rb_state2.var_8 = (rb_state2.var_8) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0xdc525766));
        rb_state2.var_9 = (rb_state2.var_9) + (((((0x80629e20) ^ (rb_state2.var_8)) ^ (0xd09d2ace)) - (rb_state2.var_7)) ^ (0x172ed874));
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_9);
        rb_state2.var_1 = rb_state2.var_2;
        pthread_mutex_unlock(&(rb_state2.lock_12));
    }
    if ((rb_state2.var_0) == (0x6a3c79b2)) {
        pthread_mutex_lock(&(rb_state2.lock_12));
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
        pthread_mutex_unlock(&(rb_state2.lock_12));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x528a596c)) {
        rb_state3.var_3 = 0x371a37ca;
        rb_state3.var_4 = (rb_state3.var_4) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_1) : (0x17b9db97));
        rb_state3.var_5 = (rb_state3.var_5) + (((0xefb3d900) + (rb_state3.var_2)) - (0x7cda03a9));
        rb_state3.var_6 = (rb_state3.var_6) ^ (((((0xb27b125c) ^ (rb_state3.var_3)) ^ (rb_state3.var_4)) - (rb_state3.var_5)) - (rb_state3.var_4));
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x18aea795));
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x6a3c79b2)) {
        rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_8) == (0xdc525766)) ? (rb_state2.var_7) : (0xa1eaa18f));
        rb_state2.var_11 = (rb_state2.var_11) - (((((0xb35ac4c) ^ (rb_state2.var_10)) ^ (rb_state2.var_10)) + (rb_state2.var_9)) ^ (0xfa0ea9f7));
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0x0)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_19));
        rb_state12.var_3 = 0xba60dea6;
        rb_state12.var_4 = (rb_state12.var_4) + (((((0x8e3e8988) ^ (0x9968f2a9)) ^ (0xfa875419)) ^ (rb_state12.var_2)) - (rb_state12.var_1));
        rb_state12.var_5 = (rb_state12.var_5) ^ (((((0x3f55d024) ^ (rb_state12.var_4)) ^ (rb_state12.var_3)) - (rb_state12.var_3)) + (rb_state12.var_4));
        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_5);
        rb_state12.var_1 = rb_state12.var_2;
        pthread_mutex_unlock(&(rb_state12.lock_19));
    }
    if ((rb_state12.var_0) == (0x0)) {
        rb_state12.var_6 = 0x4e172755;
        rb_state12.var_7 = 0xa099cffe;
        rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_7) == (0xa099cffe)) ? (rb_state12.var_6) : (0xc54140ae));
        rb_state12.var_9 = (rb_state12.var_9) - (((((0x167ef4b6) + (0x23111e6)) + (rb_state12.var_8)) ^ (rb_state12.var_5)) ^ (rb_state12.var_7));
        rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_9) == (0x5e21c041)) ? (rb_state12.var_6) : (0xac45d190));
        rb_state12.var_11 = (rb_state12.var_11) ^ (((0xa6779a29) + (0xc7b8a1dd)) - (rb_state12.var_8));
        rb_state12.var_12 = (rb_state12.var_12) + (((0xfb0bf4a9) + (rb_state12.var_9)) - (rb_state12.var_10));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_11) == (0x201914b1)) ? (rb_state12.var_10) : (0x25e66dc7));
        rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_12) == (0xa744dc3f)) ? (rb_state12.var_11) : (0x74d35924));
        rb_state12.var_15 = (rb_state12.var_15) - (((rb_state12.var_12) == (0xa744dc3f)) ? (rb_state12.var_13) : (0x9dfb5d25));
        rb_state12.var_16 = (rb_state12.var_16) - (rb_state12.var_14);
        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_13) == (0xb1e8d8ab)) ? (rb_state12.var_15) : (0x94299b04));
        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_16) == (0xdfe6eb4f)) ? (rb_state12.var_17) : (0x3647560a));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_18);
    }
    if ((rb_state12.var_0) == (0x0)) {
        rb_state12.var_20 = !((rb_state12.var_1) == (rb_state12.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xb8768ca6)) {
        rb_state18.var_3 = 0x42f8178;
        rb_state18.var_4 = 0x767aa7b8;
        rb_state18.var_5 = 0xf5579628;
        rb_state18.var_6 = (rb_state18.var_6) + (((((0xdc489213) - (rb_state18.var_3)) - (rb_state18.var_3)) - (rb_state18.var_5)) ^ (rb_state18.var_2));
        rb_state18.var_7 = (rb_state18.var_7) + (rb_state18.var_4);
        rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0xfeed2588));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (0x27d5caea);
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x0)) {
            if ((rb_state14.var_1) != (0x0)) {
                if (!((rb_state14.var_1) == (rb_state14.var_18))) {
                    racebench_trigger(14);
                }
            }
        }
        #endif
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