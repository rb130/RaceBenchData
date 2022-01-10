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
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0x0)) {
                rb_state4.var_12 = 0x8a39eac1;
                rb_state4.var_13 = 0xf154d24e;
                rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_4) : (0xa4d065d5));
                rb_state4.var_15 = (rb_state4.var_15) + (159918 < rb_input_size ? (uint32_t)rb_input[159918] : 0x13703094);
                rb_state4.var_16 = (rb_state4.var_16) + (rb_state4.var_13);
                rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_12) : (0x5509fab7));
                rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_14) : (0x6962b74d));
                rb_state4.var_19 = (rb_state4.var_19) - (rb_state4.var_15);
                rb_state4.var_20 = (rb_state4.var_20) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_16) : (0x27f6944d));
                rb_state4.var_21 = (rb_state4.var_21) - (rb_state4.var_17);
                rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_19) : (0xbd850701));
                rb_state4.var_23 = (rb_state4.var_23) + (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_21) : (0xfaaf2ad7));
                rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_23) : (0xbca3ff2a));
                rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_24);
            }
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

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
        rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - ((0x8d812250) - (rb_state7.var_0));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0xce4eb080) - (rb_state8.var_0));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (0x24eadaf7);
        if ((rb_state13.var_0) == (0x6310a23b)) {
            rb_state13.var_16 = 0x10554ff;
            rb_state13.var_17 = 0x83ef417;
            rb_state13.var_18 = (rb_state13.var_18) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_17) : (0x8e141c2d));
            rb_state13.var_19 = (rb_state13.var_19) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_5) : (0x8f0a98f6));
            rb_state13.var_20 = (rb_state13.var_20) + (rb_state13.var_18);
            rb_state13.var_21 = (rb_state13.var_21) - (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_19) : (0x5123d4f3));
            rb_state13.var_22 = (rb_state13.var_22) - (((((0x5cb45987) ^ (0x15a2d073)) + (rb_state13.var_20)) - (rb_state13.var_21)) - (0x8d20acb3));
            rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_22);
            rb_state13.var_1 = rb_state13.var_15;
        }
        if ((rb_state13.var_0) == (0x6310a23b)) {
            pthread_mutex_lock(&(rb_state13.lock_43));
            rb_state13.var_24 = 0xa0ba3874;
            rb_state13.var_25 = 0x70e284f4;
            rb_state13.var_26 = (rb_state13.var_26) + (((rb_state13.var_24) == (0x0)) ? (rb_state13.var_8) : (0x76317734));
            rb_state13.var_27 = (rb_state13.var_27) ^ (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_9) : (0x6a05cf8f));
            rb_state13.var_28 = (rb_state13.var_28) + (((0x87282cfc) ^ (0xeb6f5a31)) + (rb_state13.var_25));
            rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_26);
            rb_state13.var_30 = (rb_state13.var_30) ^ (((0xb85d7de9) ^ (rb_state13.var_27)) ^ (rb_state13.var_11));
            rb_state13.var_31 = (rb_state13.var_31) - (((0xc3831e4) - (rb_state13.var_28)) + (0x9b703b22));
            rb_state13.var_32 = (rb_state13.var_32) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_29) : (0xee6b0450));
            rb_state13.var_33 = (rb_state13.var_33) - (((0xad293f91) - (rb_state13.var_30)) ^ (rb_state13.var_13));
            rb_state13.var_34 = (rb_state13.var_34) ^ (rb_state13.var_31);
            rb_state13.var_35 = (rb_state13.var_35) ^ (rb_state13.var_32);
            rb_state13.var_36 = (rb_state13.var_36) - (rb_state13.var_33);
            rb_state13.var_37 = (rb_state13.var_37) ^ (((0x63e8dc74) ^ (rb_state13.var_34)) - (0x5b09a3de));
            rb_state13.var_38 = (rb_state13.var_38) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_35) : (0xe1bda082));
            rb_state13.var_39 = (rb_state13.var_39) + (((0x1c28b80b) + (0x7f422e20)) + (rb_state13.var_36));
            rb_state13.var_40 = (rb_state13.var_40) + (rb_state13.var_37);
            rb_state13.var_41 = (rb_state13.var_41) - (((((0x7c2de396) + (rb_state13.var_39)) - (0x5f1c4d96)) + (0xebd32713)) + (rb_state13.var_38));
            rb_state13.var_42 = (rb_state13.var_42) ^ (((((0xb8234ab6) ^ (rb_state13.var_16)) + (rb_state13.var_41)) + (rb_state13.var_40)) ^ (rb_state13.var_15));
            rb_state13.var_23 = (rb_state13.var_23) - (rb_state13.var_42);
            rb_state13.var_1 = rb_state13.var_23;
            pthread_mutex_unlock(&(rb_state13.lock_43));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x51780bbb)) {
        rb_state3.var_8 = 0x93084c28;
        rb_state3.var_9 = 0x4c1866f7;
        rb_state3.var_10 = (rb_state3.var_10) - (101649 < rb_input_size ? (uint32_t)rb_input[101649] : 0x9d98f501);
        rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_9) : (0x85f28a00));
        rb_state3.var_12 = (rb_state3.var_12) ^ (((0x4a027b34) - (rb_state3.var_8)) - (rb_state3.var_6));
        rb_state3.var_13 = (rb_state3.var_13) ^ (((0x12aa1211) + (rb_state3.var_10)) - (0x776343ab));
        rb_state3.var_14 = (rb_state3.var_14) ^ (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_11) : (0x1679ef6c));
        rb_state3.var_15 = (rb_state3.var_15) + (rb_state3.var_12);
        rb_state3.var_16 = (rb_state3.var_16) + (rb_state3.var_13);
        rb_state3.var_17 = (rb_state3.var_17) - (((0xd49cff04) + (rb_state3.var_14)) + (0x8b9b3df9));
        rb_state3.var_18 = (rb_state3.var_18) ^ (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_15) : (0x6179ab20));
        rb_state3.var_19 = (rb_state3.var_19) - (((((0x7952dd3e) - (rb_state3.var_17)) + (0x163a03dc)) ^ (rb_state3.var_9)) - (rb_state3.var_16));
        rb_state3.var_20 = (rb_state3.var_20) - (((0x8d0265e6) - (rb_state3.var_18)) - (0xa9c2d785));
        rb_state3.var_21 = (rb_state3.var_21) + (((((0xabf1ed7a) ^ (0x4684f24f)) ^ (rb_state3.var_10)) - (rb_state3.var_20)) - (rb_state3.var_19));
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_21);
    }
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (14246 < rb_input_size ? (uint32_t)rb_input[14246] : 0x998dade5);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x77f072aa);
    if ((rb_state13.var_0) == (0x6310a23b)) {
        if ((rb_state13.var_1) != (0x0)) {
            if (!((rb_state13.var_1) == (rb_state13.var_23))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xb56efcde)) {
        pthread_mutex_lock(&(rb_state7.lock_15));
        rb_state7.var_3 = 0xd8c84c42;
        rb_state7.var_4 = (rb_state7.var_4) + (104378 < rb_input_size ? (uint32_t)rb_input[104378] : 0x66638e10);
        rb_state7.var_5 = (rb_state7.var_5) + (((((0x39753330) + (rb_state7.var_4)) + (rb_state7.var_3)) ^ (0x4485db0c)) - (rb_state7.var_2));
        rb_state7.var_6 = (rb_state7.var_6) - (39142 < rb_input_size ? (uint32_t)rb_input[39142] : 0x487c5b19);
        rb_state7.var_7 = (rb_state7.var_7) ^ (rb_state7.var_4);
        rb_state7.var_8 = (rb_state7.var_8) - (rb_state7.var_5);
        rb_state7.var_9 = (rb_state7.var_9) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_6) : (0xe55a4caf));
        rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_7);
        rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_8);
        rb_state7.var_12 = (rb_state7.var_12) ^ (((0x23e94248) - (rb_state7.var_6)) - (rb_state7.var_9));
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0xce143f0b));
        rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_13) : (0xf1365f59));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_14);
        pthread_mutex_unlock(&(rb_state7.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0xce4eb080)) ? (rb_state8.var_0) : (0x9ef637c1));
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (98390 < rb_input_size ? (uint32_t)rb_input[98390] : 0xd87f01bf);
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_7) == (0x49fbe25)) {
        rb_state0.var_15 = rb_state0.var_8;
    }
    if ((rb_state0.var_7) == (0x49fbe25)) {
        if (!((rb_state0.var_8) == (rb_state0.var_15))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_25));
        rb_state4.var_3 = 0x3a1fd0ce;
        rb_state4.var_4 = 0xf6b6b305;
        rb_state4.var_5 = 0x28b45c65;
        rb_state4.var_6 = (rb_state4.var_6) - (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_4) : (0xae3bcd79));
        rb_state4.var_7 = (rb_state4.var_7) - (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_3) : (0x186bac16));
        rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_3) == (0x3a1fd0ce)) ? (rb_state4.var_5) : (0x553930b8));
        rb_state4.var_9 = (rb_state4.var_9) - (rb_state4.var_6);
        rb_state4.var_10 = (rb_state4.var_10) - (((rb_state4.var_7) == (0xc5e02f32)) ? (rb_state4.var_8) : (0xdfc56fbb));
        rb_state4.var_11 = (rb_state4.var_11) - (((rb_state4.var_9) == (0xf6b6b305)) ? (rb_state4.var_10) : (0xedb2591a));
        rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_11);
        rb_state4.var_1 = rb_state4.var_2;
        pthread_mutex_unlock(&(rb_state4.lock_25));
    }
    if ((rb_state4.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_25));
        if (!((rb_state4.var_1) == (rb_state4.var_2))) {
            racebench_trigger(4);
        }
        pthread_mutex_unlock(&(rb_state4.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x0)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0x0)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_13));
        rb_state6.var_3 = 0x2bc45629;
        rb_state6.var_4 = (rb_state6.var_4) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_3) : (0x50f10745));
        rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_2);
        rb_state6.var_6 = (rb_state6.var_6) ^ (((0x6534b188) - (rb_state6.var_4)) ^ (rb_state6.var_5));
        rb_state6.var_7 = (rb_state6.var_7) ^ (((0xc261e39f) ^ (0x1c51cef1)) + (rb_state6.var_5));
        rb_state6.var_8 = (rb_state6.var_8) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_6) : (0xdf5c587e));
        rb_state6.var_9 = (rb_state6.var_9) + (rb_state6.var_7);
        rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_8) : (0xa12b8fce));
        rb_state6.var_11 = (rb_state6.var_11) - (rb_state6.var_9);
        rb_state6.var_12 = (rb_state6.var_12) ^ (((((0x49f2ca5) ^ (rb_state6.var_8)) ^ (rb_state6.var_10)) - (0xb7c22fe7)) ^ (rb_state6.var_11));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_12);
        pthread_mutex_unlock(&(rb_state6.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x90a66295)) {
        rb_state10.var_3 = 0x151f5fb4;
        rb_state10.var_4 = (rb_state10.var_4) ^ (((((0xd9461ed8) + (rb_state10.var_2)) + (0xcb2955d9)) ^ (rb_state10.var_3)) + (rb_state10.var_4));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_4);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0x81a02f6d);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        if ((rb_state17.var_1) != (0x0)) {
            if (!((rb_state17.var_1) == (rb_state17.var_10))) {
                racebench_trigger(17);
            }
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
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0xc25e37ca)) {
                pthread_mutex_lock(&(rb_state12.lock_25));
                rb_state12.var_3 = 0x840594d1;
                rb_state12.var_4 = 0x3b40abec;
                rb_state12.var_5 = (rb_state12.var_5) ^ (0x9198fe33);
                rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_4) == (0x3b40abec)) ? (rb_state12.var_1) : (0x7053aae5));
                rb_state12.var_7 = (rb_state12.var_7) + (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_3) : (0x653bad14));
                rb_state12.var_8 = (rb_state12.var_8) + (((0x47117e0b) - (rb_state12.var_5)) - (rb_state12.var_3));
                rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0x21e3f446));
                rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_4) == (0x3b40abec)) ? (rb_state12.var_8) : (0xb1eff2c5));
                rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_9);
                rb_state12.var_12 = (rb_state12.var_12) - (((0xaa57ae33) ^ (rb_state12.var_10)) - (0xbffedf0));
                rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_11) == (0x840594d1)) ? (rb_state12.var_12) : (0x1341519b));
                rb_state12.var_2 = (rb_state12.var_2) ^ (rb_state12.var_13);
                rb_state12.var_1 = rb_state12.var_2;
                pthread_mutex_unlock(&(rb_state12.lock_25));
            }
            if ((rb_state12.var_0) == (0xf97f424c)) {
                if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                    racebench_trigger(12);
                }
            }
            #endif
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

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_2 = rb_state2.var_1;
    }
    if ((rb_state2.var_0) == (0x0)) {
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
    }
    #endif
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