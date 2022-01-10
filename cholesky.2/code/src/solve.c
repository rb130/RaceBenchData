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
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x1bd4bd6f)) ? (rb_state0.var_1) : (0x4aa71003));
            if ((rb_state0.var_1) == (0x0)) {
                rb_state0.var_2 = rb_state0.var_0;
            }
            if ((rb_state0.var_1) == (0x0)) {
                if (!((rb_state0.var_0) == (rb_state0.var_2))) {
                    racebench_trigger(0);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_1);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) - (0xc8bb2dd3);
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
        rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (0x4444adee);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (0x4fcd00da);
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
        if ((rb_state4.var_1) == (0x3f340589)) {
            if ((rb_state4.var_2) != (0x0)) {
                if (!((rb_state4.var_2) == (rb_state4.var_21))) {
                    racebench_trigger(4);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (0x94c9a656);
        if ((rb_state11.var_0) == (0x64d6708e)) {
            rb_state11.var_3 = 0x661e2b8b;
            rb_state11.var_4 = 0x8be55134;
            rb_state11.var_5 = (rb_state11.var_5) ^ (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_3) : (0xc4e50259));
            rb_state11.var_6 = (rb_state11.var_6) - (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x42b20e2a));
            rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_5) : (0xf7d3df2c));
            rb_state11.var_8 = (rb_state11.var_8) + (rb_state11.var_6);
            rb_state11.var_9 = (rb_state11.var_9) - (((((0x7ef7f5f7) - (rb_state11.var_5)) + (rb_state11.var_7)) + (0x8b01f444)) + (rb_state11.var_8));
            rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (0x44cee96d);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
        rb_state13.var_1 = (rb_state13.var_1) - (0xdd4d0fd6);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (159131 < rb_input_size ? (uint32_t)rb_input[159131] : 0x44495fd1);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - ((0x14ab28af) ^ (0xb50c8095));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xe62f2f9b) + (0x35a58dd4));
    rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_1) : (0xab94e6b6));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0x721904a8);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (57158 < rb_input_size ? (uint32_t)rb_input[57158] : 0x5e95c90e);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (111996 < rb_input_size ? (uint32_t)rb_input[111996] : 0x2bc26e5f);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (61655 < rb_input_size ? (uint32_t)rb_input[61655] : 0x92f5a86);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ ((0x9af103ae) + (rb_state14.var_0));
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (157634 < rb_input_size ? (uint32_t)rb_input[157634] : 0x10c4363f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ ((0x30b0097) - (0xc0ecaf4b));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_0) : (0x42aa8b09));
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0xd66cb354)) ? (rb_state11.var_1) : (0xe1905692));
    if ((rb_state11.var_0) == (0xf9a016e4)) {
        rb_state11.var_2 = rb_state11.var_1;
    }
    if ((rb_state11.var_0) == (0x64d6708e)) {
        pthread_mutex_lock(&(rb_state11.lock_10));
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (1666 < rb_input_size ? (uint32_t)rb_input[1666] : 0x7ae26c5a);
    if ((rb_state16.var_0) == (0xf275a08f)) {
        pthread_mutex_lock(&(rb_state16.lock_7));
        rb_state16.var_3 = 0x4590c98a;
        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_2);
        rb_state16.var_5 = (rb_state16.var_5) ^ (((0x9874a7e) + (0x847b28fb)) + (rb_state16.var_3));
        rb_state16.var_6 = (rb_state16.var_6) - (((((0xc3b36262) + (rb_state16.var_5)) + (rb_state16.var_4)) - (rb_state16.var_5)) + (rb_state16.var_4));
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_6);
        pthread_mutex_unlock(&(rb_state16.lock_7));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (0x93082565);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0xcbe6d8f6) - (0xc46fca13));
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_5));
        rb_state0.var_3 = 0xbf1532c;
        rb_state0.var_4 = (rb_state0.var_4) - (((rb_state0.var_2) == (0x1bd4bd6f)) ? (rb_state0.var_3) : (0xc40d7dac));
        rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_4);
        pthread_mutex_unlock(&(rb_state0.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0xbbbb5212)) ? (rb_state2.var_0) : (0x834fd472));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (104136 < rb_input_size ? (uint32_t)rb_input[104136] : 0x79df2bec);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ ((0x94217639) - (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (0xd1d245af);
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x3f340589)) {
        rb_state4.var_16 = 0x75152315;
        rb_state4.var_17 = (rb_state4.var_17) + (((((0x85359219) + (rb_state4.var_10)) - (rb_state4.var_9)) ^ (rb_state4.var_16)) - (rb_state4.var_7));
        rb_state4.var_18 = (rb_state4.var_18) ^ ((0x672ff5d6) ^ (rb_state4.var_11));
        rb_state4.var_19 = (rb_state4.var_19) ^ (((((0x25f2cb9a) + (rb_state4.var_17)) - (rb_state4.var_12)) - (rb_state4.var_8)) + (rb_state4.var_13));
        rb_state4.var_20 = (rb_state4.var_20) + (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_19) : (0xf42175ee));
        rb_state4.var_15 = (rb_state4.var_15) - (rb_state4.var_20);
        rb_state4.var_2 = rb_state4.var_15;
    }
    if ((rb_state4.var_1) == (0x3f340589)) {
        pthread_mutex_lock(&(rb_state4.lock_39));
        rb_state4.var_22 = 0xc4ecd3f3;
        rb_state4.var_23 = (rb_state4.var_23) - (((0x3e92d950) ^ (rb_state4.var_15)) + (rb_state4.var_16));
        rb_state4.var_24 = (rb_state4.var_24) ^ (0xd669e79f);
        rb_state4.var_25 = (rb_state4.var_25) ^ (((((0xb3b3158d) - (rb_state4.var_17)) - (rb_state4.var_22)) ^ (rb_state4.var_14)) ^ (rb_state4.var_18));
        rb_state4.var_26 = (rb_state4.var_26) ^ (rb_state4.var_23);
        rb_state4.var_27 = (rb_state4.var_27) - (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_24) : (0x5603b815));
        rb_state4.var_28 = (rb_state4.var_28) ^ (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_25) : (0x49eda45a));
        rb_state4.var_29 = (rb_state4.var_29) + (((((0x91458894) - (rb_state4.var_26)) ^ (0xcc6d92e)) ^ (rb_state4.var_27)) - (rb_state4.var_21));
        rb_state4.var_30 = (rb_state4.var_30) + (rb_state4.var_28);
        rb_state4.var_31 = (rb_state4.var_31) + (((0x77db053d) ^ (rb_state4.var_22)) + (rb_state4.var_29));
        rb_state4.var_32 = (rb_state4.var_32) + (rb_state4.var_30);
        rb_state4.var_33 = (rb_state4.var_33) ^ (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_31) : (0xdb67a7c7));
        rb_state4.var_34 = (rb_state4.var_34) ^ (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_32) : (0x2f4cd93a));
        rb_state4.var_35 = (rb_state4.var_35) ^ (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_33) : (0xa84519e9));
        rb_state4.var_36 = (rb_state4.var_36) - (((0x6845b81f) + (rb_state4.var_34)) + (0x474ba4c4));
        rb_state4.var_37 = (rb_state4.var_37) ^ (rb_state4.var_35);
        rb_state4.var_38 = (rb_state4.var_38) + (((((0x39c01b07) ^ (0x3f2e5004)) ^ (rb_state4.var_36)) ^ (rb_state4.var_37)) + (0xa1084c54));
        rb_state4.var_21 = (rb_state4.var_21) - (rb_state4.var_38);
        rb_state4.var_2 = rb_state4.var_21;
        pthread_mutex_unlock(&(rb_state4.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - ((0x1a61c8de) ^ (0x5473db80));
    if ((rb_state8.var_1) == (0x63dbd944)) {
        pthread_mutex_lock(&(rb_state8.lock_32));
        rb_state8.var_4 = 0x3cd49b3d;
        rb_state8.var_5 = 0xcbf51d33;
        rb_state8.var_6 = 0xfd584953;
        rb_state8.var_7 = (rb_state8.var_7) + (rb_state8.var_2);
        rb_state8.var_8 = (rb_state8.var_8) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_6) : (0xe097cc40));
        rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_5) : (0x67b85d47));
        rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_7);
        rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_8) : (0x64977d67));
        rb_state8.var_12 = (rb_state8.var_12) - (rb_state8.var_9);
        rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x30f3c3e));
        rb_state8.var_14 = (rb_state8.var_14) + (((((0x47d62db8) - (0x79acf51d)) + (0xaa4a0144)) + (rb_state8.var_12)) - (rb_state8.var_13));
        rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_14);
        rb_state8.var_2 = rb_state8.var_3;
        pthread_mutex_unlock(&(rb_state8.lock_32));
    }
    if ((rb_state8.var_1) == (0x63dbd944)) {
        pthread_mutex_lock(&(rb_state8.lock_32));
        if (!((rb_state8.var_2) == (rb_state8.var_3))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_1) : (0x5c7dbb4e));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0x6a)) ? (rb_state16.var_1) : (0x793acff7));
    if ((rb_state16.var_0) == (0xf275a08f)) {
        rb_state16.var_2 = rb_state16.var_1;
    }
    if ((rb_state16.var_0) == (0xf275a08f)) {
        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x421c2304)) {
        if ((rb_state10.var_2) != (0x0)) {
            if (!((rb_state10.var_2) == (rb_state10.var_26))) {
                pthread_mutex_lock(&(rb_state10.lock_62));
                rb_state10.var_37 = 0x12173a2c;
                rb_state10.var_38 = (rb_state10.var_38) ^ (((0xa85f047f) ^ (rb_state10.var_37)) + (0x83a86b1));
                rb_state10.var_39 = (rb_state10.var_39) - (rb_state10.var_21);
                rb_state10.var_40 = (rb_state10.var_40) - (((((0xa75ff636) - (rb_state10.var_19)) - (rb_state10.var_22)) - (rb_state10.var_20)) ^ (0xaa09109e));
                rb_state10.var_41 = (rb_state10.var_41) - (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_38) : (0xaaa5895e));
                rb_state10.var_42 = (rb_state10.var_42) + (((rb_state10.var_39) == (0x0)) ? (rb_state10.var_40) : (0x88449239));
                rb_state10.var_43 = (rb_state10.var_43) - (((0xfd8cf86f) ^ (rb_state10.var_41)) ^ (0xfc4f75a1));
                rb_state10.var_44 = (rb_state10.var_44) + (rb_state10.var_42);
                rb_state10.var_45 = (rb_state10.var_45) ^ (((0x17114e49) - (0xab0559f5)) - (rb_state10.var_43));
                rb_state10.var_46 = (rb_state10.var_46) + (((rb_state10.var_24) == (0x0)) ? (rb_state10.var_44) : (0xebd91b3b));
                rb_state10.var_47 = (rb_state10.var_47) + (((rb_state10.var_45) == (0x0)) ? (rb_state10.var_46) : (0xcdd5d610));
                rb_state10.var_36 = (rb_state10.var_36) - (rb_state10.var_47);
                rb_state10.var_35 = rb_state10.var_36;
                pthread_mutex_unlock(&(rb_state10.lock_62));
            }
            if (!((rb_state10.var_2) == (rb_state10.var_26))) {
                pthread_mutex_lock(&(rb_state10.lock_62));
                if (!((rb_state10.var_35) == (rb_state10.var_36))) {
                    racebench_trigger(10);
                }
                pthread_mutex_unlock(&(rb_state10.lock_62));
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x8acbc088)) {
        rb_state13.var_3 = 0xe5cded7b;
        rb_state13.var_4 = (rb_state13.var_4) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_3) : (0x8db7431c));
        rb_state13.var_5 = (rb_state13.var_5) ^ (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_4) : (0x87ba4f84));
        rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x79615f18)) ? (rb_state17.var_1) : (0x7ab5eebb));
    if ((rb_state17.var_0) == (0x7fc4be22)) {
        pthread_mutex_lock(&(rb_state17.lock_41));
        if ((rb_state17.var_2) != (0x0)) {
            if (!((rb_state17.var_2) == (rb_state17.var_32))) {
                racebench_trigger(17);
            }
        }
        pthread_mutex_unlock(&(rb_state17.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + ((0x8b8de412) - (0x78712292));
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xab501389));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
    rb_state2.var_1 = (rb_state2.var_1) ^ (54218 < rb_input_size ? (uint32_t)rb_input[54218] : 0x6c4bdd06);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0x48733b32) - (0xe9814c99));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0x2f2403db);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x1d1febf0)) {
        if ((rb_state18.var_2) != (0x0)) {
            if (!((rb_state18.var_2) == (rb_state18.var_14))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0xa)) {
        if ((rb_state2.var_2) != (0x0)) {
            if (!((rb_state2.var_2) == (rb_state2.var_26))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        rb_state3.var_2 = rb_state3.var_0;
    }
    if ((rb_state3.var_1) == (0x0)) {
        if (!((rb_state3.var_0) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x7b124843) - (rb_state7.var_0));
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x7b124843)) ? (rb_state7.var_1) : (0xdf41347c));
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xfa7aacdd)) {
        rb_state17.var_15 = 0xab1672e3;
        rb_state17.var_16 = (rb_state17.var_16) + (rb_state17.var_9);
        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_10) : (0x2b420ecf));
        rb_state17.var_18 = (rb_state17.var_18) ^ (rb_state17.var_7);
        rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_8) : (0xc994565e));
        rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_16);
        rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_17) : (0x15d25235));
        rb_state17.var_22 = (rb_state17.var_22) ^ (((0xdb44cc73) - (rb_state17.var_18)) ^ (0xf3dd6b4c));
        rb_state17.var_23 = (rb_state17.var_23) - (rb_state17.var_19);
        rb_state17.var_24 = (rb_state17.var_24) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_20) : (0x236c572b));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_21) : (0xf3bd62e2));
        rb_state17.var_26 = (rb_state17.var_26) - (rb_state17.var_22);
        rb_state17.var_27 = (rb_state17.var_27) ^ (((((0x854acefd) + (rb_state17.var_23)) - (0xea0e68cf)) - (rb_state17.var_24)) - (rb_state17.var_15));
        rb_state17.var_28 = (rb_state17.var_28) + (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_26) : (0x32dc7540));
        rb_state17.var_29 = (rb_state17.var_29) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_27) : (0x5bac5d12));
        rb_state17.var_30 = (rb_state17.var_30) - (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_28) : (0xb9e3ddba));
        rb_state17.var_31 = (rb_state17.var_31) ^ (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_30) : (0x31bcb7e5));
        rb_state17.var_14 = (rb_state17.var_14) ^ (rb_state17.var_31);
        rb_state17.var_2 = rb_state17.var_14;
    }
    if ((rb_state17.var_0) == (0x7fc4be22)) {
        pthread_mutex_lock(&(rb_state17.lock_41));
        rb_state17.var_33 = 0x967f2488;
        rb_state17.var_34 = (rb_state17.var_34) + (((((0x3b5df9f7) + (rb_state17.var_18)) + (rb_state17.var_19)) + (rb_state17.var_21)) - (rb_state17.var_20));
        rb_state17.var_35 = (rb_state17.var_35) + (rb_state17.var_22);
        rb_state17.var_36 = (rb_state17.var_36) ^ (rb_state17.var_33);
        rb_state17.var_37 = (rb_state17.var_37) ^ (((rb_state17.var_34) == (0x0)) ? (rb_state17.var_35) : (0x16c0715c));
        rb_state17.var_38 = (rb_state17.var_38) - (rb_state17.var_36);
        rb_state17.var_39 = (rb_state17.var_39) + (((0x1d5f6c8f) - (rb_state17.var_37)) ^ (rb_state17.var_23));
        rb_state17.var_40 = (rb_state17.var_40) ^ (((rb_state17.var_38) == (0x0)) ? (rb_state17.var_39) : (0x95743636));
        rb_state17.var_32 = (rb_state17.var_32) - (rb_state17.var_40);
        rb_state17.var_2 = rb_state17.var_32;
        pthread_mutex_unlock(&(rb_state17.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - ((0xe2e01430) ^ (rb_state18.var_1));
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x22f3041c)) {
        rb_state1.var_18 = !((rb_state1.var_3) == (rb_state1.var_2));
    }
    if ((rb_state1.var_1) == (0x22f3041c)) {
        pthread_mutex_lock(&(rb_state1.lock_17));
        rb_state1.var_4 = 0x9ba77bc1;
        rb_state1.var_5 = (rb_state1.var_5) + (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_4) : (0x755fcff5));
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_5) == (0x9ba77bc1)) ? (rb_state1.var_4) : (0xfa85242b));
        rb_state1.var_7 = (rb_state1.var_7) ^ (0x10781719);
        rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_6) == (0x9ba77bc1)) ? (rb_state1.var_2) : (0xde376cff));
        rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_5);
        rb_state1.var_10 = (rb_state1.var_10) ^ (((rb_state1.var_6) == (0x9ba77bc1)) ? (rb_state1.var_7) : (0xd1e5c089));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_8);
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) + (((0x9a88ba29) - (0xdf82365a)) - (rb_state1.var_10));
        rb_state1.var_14 = (rb_state1.var_14) + (((0xb56faef8) ^ (0xf4b65c40)) ^ (rb_state1.var_11));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((((0x6c48598c) - (0x605ccfc3)) ^ (rb_state1.var_13)) ^ (0x99996081)) + (rb_state1.var_12));
        rb_state1.var_16 = (rb_state1.var_16) + (((((0xb87d6473) - (rb_state1.var_14)) + (rb_state1.var_7)) - (rb_state1.var_8)) ^ (rb_state1.var_15));
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_16);
        rb_state1.var_3 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_17));
    }
    rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_11) == (0x21c89301)) ? (rb_state1.var_10) : (0x94e85cb5));
    rb_state1.var_12 = (rb_state1.var_12) ^ (0xaa7f7990);
    if ((rb_state1.var_13) == (0x47ee31ea)) {
        pthread_mutex_lock(&(rb_state1.lock_28));
        rb_state1.var_20 = 0xfad6cbc4;
        rb_state1.var_21 = (rb_state1.var_21) - ((0x46fd405) - (rb_state1.var_18));
        rb_state1.var_22 = (rb_state1.var_22) - (((0xcc7b8eaf) - (0xd8482833)) ^ (rb_state1.var_16));
        rb_state1.var_23 = (rb_state1.var_23) + (((((0xea2af344) + (rb_state1.var_19)) - (rb_state1.var_15)) ^ (rb_state1.var_20)) - (rb_state1.var_20));
        rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_22) : (0xa166ba3d));
        rb_state1.var_25 = (rb_state1.var_25) + (rb_state1.var_23);
        rb_state1.var_26 = (rb_state1.var_26) - (((0xeff95179) - (0x64f70725)) - (rb_state1.var_24));
        rb_state1.var_27 = (rb_state1.var_27) + (((rb_state1.var_25) == (0x0)) ? (rb_state1.var_26) : (0x24dcbc19));
        rb_state1.var_14 = (rb_state1.var_14) - (rb_state1.var_27);
        pthread_mutex_unlock(&(rb_state1.lock_28));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_7 = 0xe646bbaf;
        rb_state15.var_8 = 0xe804a43c;
        rb_state15.var_9 = (rb_state15.var_9) ^ (((0x100a2802) - (0xee574561)) ^ (rb_state15.var_5));
        rb_state15.var_10 = (rb_state15.var_10) + (rb_state15.var_6);
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0x7a37b1a3));
        rb_state15.var_12 = (rb_state15.var_12) + (rb_state15.var_9);
        rb_state15.var_13 = (rb_state15.var_13) + (((((0xe078d426) + (rb_state15.var_11)) ^ (rb_state15.var_7)) ^ (0xc5eb2e50)) ^ (rb_state15.var_10));
        rb_state15.var_14 = (rb_state15.var_14) + (((0xe74ec62) + (rb_state15.var_8)) - (rb_state15.var_12));
        rb_state15.var_15 = (rb_state15.var_15) + (((0x69f081f6) ^ (rb_state15.var_9)) + (rb_state15.var_13));
        rb_state15.var_16 = (rb_state15.var_16) ^ (((0xfeca3404) + (rb_state15.var_10)) + (rb_state15.var_14));
        rb_state15.var_17 = (rb_state15.var_17) ^ (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_15) : (0xc9d9c2c));
        rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_16);
        rb_state15.var_19 = (rb_state15.var_19) + (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_17) : (0x5b2f2465));
        rb_state15.var_20 = (rb_state15.var_20) ^ (((((0xd941f34e) - (rb_state15.var_13)) + (rb_state15.var_19)) - (0xdf9cbb76)) ^ (rb_state15.var_18));
        rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_20);
        rb_state15.var_2 = rb_state15.var_6;
    }
    if ((rb_state15.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_26));
        rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_15) : (0x554c63ca));
        rb_state15.var_23 = (rb_state15.var_23) - (102947 < rb_input_size ? (uint32_t)rb_input[102947] : 0x796d53d2);
        rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_22) : (0x3567ddd1));
        rb_state15.var_25 = (rb_state15.var_25) - (((rb_state15.var_23) == (0x0)) ? (rb_state15.var_24) : (0x8e11012f));
        rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_25);
        rb_state15.var_2 = rb_state15.var_21;
        pthread_mutex_unlock(&(rb_state15.lock_26));
    }
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x0)) {
            if ((rb_state9.var_2) != (0x0)) {
                if (!((rb_state9.var_2) == (rb_state9.var_18))) {
                    racebench_trigger(9);
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
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0x0)) {
                if ((rb_state6.var_1) != (0x0)) {
                    if (!((rb_state6.var_1) == (rb_state6.var_17))) {
                        racebench_trigger(6);
                    }
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