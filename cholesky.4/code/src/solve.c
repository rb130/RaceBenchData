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
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_2);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0xe4973e04));
            rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_1);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_2);
        rb_state9.var_1 = (rb_state9.var_1) + (0x864dc37a);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ ((0x616e6216) + (0xec48e8fc));
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_2);
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_0);
        rb_state16.var_1 = (rb_state16.var_1) ^ (32174 < rb_input_size ? (uint32_t)rb_input[32174] : 0xb95cbf56);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0x950804fe));
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
        rb_state0.var_0 = (rb_state0.var_0) + ((0x2c034204) - (0x8abbee78));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - ((0xa3bde23) - (0x74fc3476));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (80138 < rb_input_size ? (uint32_t)rb_input[80138] : 0x9661f4b3);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
        rb_state4.var_1 = (rb_state4.var_1) ^ (139874 < rb_input_size ? (uint32_t)rb_input[139874] : 0xe01d62ce);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ ((0xcb2be3f7) - (0x164c5219));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - ((0x92735fd1) + (rb_state9.var_0));
        rb_state9.var_2 = (rb_state9.var_2) ^ (127366 < rb_input_size ? (uint32_t)rb_input[127366] : 0x9e8e049e);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (0x18fbc00d);
        if ((rb_state10.var_1) == (0xcc95058a)) {
            pthread_mutex_lock(&(rb_state10.lock_19));
            rb_state10.var_5 = 0xc00f36a7;
            rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0xe5c97db1));
            rb_state10.var_7 = (rb_state10.var_7) - (78980 < rb_input_size ? (uint32_t)rb_input[78980] : 0x7db51731);
            rb_state10.var_8 = (rb_state10.var_8) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_4) : (0x10efbdd));
            rb_state10.var_9 = (rb_state10.var_9) - (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_6) : (0x70a3ccdb));
            rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_7) : (0x2289afff));
            rb_state10.var_11 = (rb_state10.var_11) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_8) : (0x97e3a5a9));
            rb_state10.var_12 = (rb_state10.var_12) + (((0xbc1d261a) + (rb_state10.var_9)) - (rb_state10.var_9));
            rb_state10.var_13 = (rb_state10.var_13) + (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0x6c8aa6b1));
            rb_state10.var_14 = (rb_state10.var_14) + (((((0xd51de31b) - (rb_state10.var_12)) ^ (rb_state10.var_13)) - (rb_state10.var_10)) - (rb_state10.var_11));
            rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_14);
            rb_state10.var_3 = rb_state10.var_4;
            pthread_mutex_unlock(&(rb_state10.lock_19));
        }
        if ((rb_state10.var_1) == (0xcc95058a)) {
            if (!((rb_state10.var_3) == (rb_state10.var_4))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (16013 < rb_input_size ? (uint32_t)rb_input[16013] : 0x3a87c529);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (93511 < rb_input_size ? (uint32_t)rb_input[93511] : 0x839506ed);
        rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_2);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_0) : (0x4c0767aa));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (144283 < rb_input_size ? (uint32_t)rb_input[144283] : 0xea67abd7);
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xf83ba406)) {
        rb_state1.var_4 = 0x64852c98;
        rb_state1.var_5 = (rb_state1.var_5) + ((0x2f675594) ^ (rb_state1.var_5));
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x6d6205e3));
        rb_state1.var_7 = (rb_state1.var_7) + (rb_state1.var_5);
        rb_state1.var_8 = (rb_state1.var_8) + (((((0x7fe02317) ^ (rb_state1.var_6)) + (rb_state1.var_7)) ^ (rb_state1.var_6)) - (0x9c076716));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0x27cd03a0) - (0xd21e97e0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (110589 < rb_input_size ? (uint32_t)rb_input[110589] : 0xe3409ee);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (0x6ebbc544);
    rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0x48705bce));
    if ((rb_state4.var_0) == (0x6f1f4864)) {
        pthread_mutex_lock(&(rb_state4.lock_6));
        rb_state4.var_3 = rb_state4.var_1;
        pthread_mutex_unlock(&(rb_state4.lock_6));
    }
    if ((rb_state4.var_0) == (0x6f1f4864)) {
        if (!((rb_state4.var_1) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (0xb6ade63b);
    rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_1) == (0x495219c5)) ? (rb_state6.var_0) : (0x58150810));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - ((0xc0ca3141) ^ (0x5ef718f5));
    rb_state8.var_0 = (rb_state8.var_0) ^ (124846 < rb_input_size ? (uint32_t)rb_input[124846] : 0x51baafca);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_1) : (0xc83a6ee4));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ ((0x781054c2) - (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xd8c2dd2));
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0x35c4186f));
    rb_state19.var_1 = (rb_state19.var_1) + ((0x15371987) ^ (rb_state19.var_0));
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_1);
    rb_state0.var_2 = (rb_state0.var_2) - (0xa963072f);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (0x22245fbb);
    rb_state1.var_2 = (rb_state1.var_2) - (0x623f4c8c);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_0);
    rb_state3.var_2 = (rb_state3.var_2) + (0xdb73ee0a);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_2);
    rb_state10.var_1 = (rb_state10.var_1) + ((0xe205b66c) + (0xea8f4f3e));
    rb_state10.var_2 = (rb_state10.var_2) + (121484 < rb_input_size ? (uint32_t)rb_input[121484] : 0x41d91328);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x47bebbd4) + (rb_state14.var_1));
    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0xb841442c)) ? (rb_state14.var_0) : (0x73f7b73d));
    if ((rb_state14.var_0) == (0x7cf8b20)) {
        rb_state14.var_11 = 0x6b3e1c14;
        rb_state14.var_12 = (rb_state14.var_12) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_11) : (0xa841000));
        rb_state14.var_13 = (rb_state14.var_13) ^ (rb_state14.var_11);
        rb_state14.var_14 = (rb_state14.var_14) + (115893 < rb_input_size ? (uint32_t)rb_input[115893] : 0xe309492b);
        rb_state14.var_15 = (rb_state14.var_15) + (rb_state14.var_10);
        rb_state14.var_16 = (rb_state14.var_16) - (((((0xeb216577) - (rb_state14.var_12)) ^ (rb_state14.var_13)) ^ (0xea568a52)) ^ (rb_state14.var_12));
        rb_state14.var_17 = (rb_state14.var_17) - (((0x3a8d5239) + (0xde274375)) + (rb_state14.var_14));
        rb_state14.var_18 = (rb_state14.var_18) + (((((0xfff8111b) + (0x5e4ae0fd)) - (0x2cb50a9c)) - (rb_state14.var_16)) ^ (rb_state14.var_15));
        rb_state14.var_19 = (rb_state14.var_19) - (rb_state14.var_17);
        rb_state14.var_20 = (rb_state14.var_20) + (((((0xe460f769) - (rb_state14.var_19)) - (0x848b350d)) - (rb_state14.var_18)) + (rb_state14.var_13));
        rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_3) == (0xcbcc6bbe)) {
        rb_state16.var_4 = rb_state16.var_2;
    }
    if ((rb_state16.var_3) == (0xcbcc6bbe)) {
        if (!((rb_state16.var_2) == (rb_state16.var_4))) {
            racebench_trigger(16);
        }
    }
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (99428 < rb_input_size ? (uint32_t)rb_input[99428] : 0xdc9c0f74);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_0);
    rb_state1.var_1 = (rb_state1.var_1) + ((0x568e7c2f) ^ (0x254ca2ab));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (131189 < rb_input_size ? (uint32_t)rb_input[131189] : 0x9314efb3);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (63095 < rb_input_size ? (uint32_t)rb_input[63095] : 0xd95a7244);
    rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_1);
    rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_2) : (0x6e1904b0));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x6d8ca02f)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        rb_state9.var_15 = (rb_state9.var_15) - (((0x47b9d4b9) ^ (0xcd47009b)) + (rb_state9.var_7));
        rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0x60269023));
        rb_state9.var_17 = (rb_state9.var_17) ^ (((((0x43f3e5ad) + (rb_state9.var_10)) + (rb_state9.var_15)) + (rb_state9.var_16)) + (0xf1882154));
        rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_17);
        rb_state9.var_3 = rb_state9.var_14;
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xba18b57d) - (0x69c982f));
    rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_0) == (0xe5739d68)) ? (rb_state10.var_2) : (0xcc349222));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_3) == (0xb01bb5b6)) {
        rb_state11.var_5 = 0xad77c33;
        rb_state11.var_6 = 0x598dd6ef;
        rb_state11.var_7 = 0xc6bac957;
        rb_state11.var_8 = (rb_state11.var_8) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_5) : (0x7e12aebf));
        rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_6) : (0x1c08e4f0));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((((0x1b2ec832) + (rb_state11.var_8)) ^ (rb_state11.var_5)) ^ (rb_state11.var_6)) + (rb_state11.var_9));
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_10);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (113034 < rb_input_size ? (uint32_t)rb_input[113034] : 0xbb7c776c);
    rb_state13.var_2 = (rb_state13.var_2) + (152140 < rb_input_size ? (uint32_t)rb_input[152140] : 0xd5f0286f);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (((rb_state14.var_0) == (0x781054f3)) ? (rb_state14.var_2) : (0x29cab549));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) + ((0x3f0d494) + (0xc7db972a));
    if ((rb_state16.var_3) == (0xcbcc6bbe)) {
        rb_state16.var_5 = 0x434552e1;
        rb_state16.var_6 = (rb_state16.var_6) - (0x1f9d84de);
        rb_state16.var_7 = (rb_state16.var_7) + (0xb3722eb5);
        rb_state16.var_8 = (rb_state16.var_8) ^ (((((0xaec46062) - (rb_state16.var_4)) ^ (rb_state16.var_6)) - (0x2696ba0e)) ^ (rb_state16.var_5));
        rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_6);
        rb_state16.var_10 = (rb_state16.var_10) - (((((0x5f5b2180) - (rb_state16.var_8)) ^ (rb_state16.var_7)) - (rb_state16.var_8)) ^ (rb_state16.var_7));
        rb_state16.var_11 = (rb_state16.var_11) + (((((0xa9668a01) - (0xe0f754aa)) - (rb_state16.var_9)) ^ (rb_state16.var_10)) ^ (rb_state16.var_9));
        rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) ^ (0xd800da8e);
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (0x6ccdf64e);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (0xf3055964);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (120454 < rb_input_size ? (uint32_t)rb_input[120454] : 0x252c82ac);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0x17a116f2));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x9151a49d));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_2);
    rb_state19.var_3 = (rb_state19.var_3) + ((0x615c63ff) + (0x409768c1));
    rb_state19.var_2 = (rb_state19.var_2) + (0xe9bfb647);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0xd7257373));
    rb_state0.var_1 = (rb_state0.var_1) ^ (10979 < rb_input_size ? (uint32_t)rb_input[10979] : 0xd86d0b52);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (72445 < rb_input_size ? (uint32_t)rb_input[72445] : 0xccd1f3c1);
    if ((rb_state1.var_1) == (0xf83ba406)) {
        rb_state1.var_3 = rb_state1.var_2;
    }
    if ((rb_state1.var_1) == (0xf83ba406)) {
        pthread_mutex_lock(&(rb_state1.lock_9));
        if (!((rb_state1.var_2) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
        pthread_mutex_unlock(&(rb_state1.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_2);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (0xfac86f02);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
    if ((rb_state8.var_2) == (0x0)) {
        rb_state8.var_14 = 0x20d1ff60;
        rb_state8.var_15 = 0xe3fadb20;
        rb_state8.var_16 = 0x2e70ceb7;
        rb_state8.var_17 = (rb_state8.var_17) + (0x93ee9387);
        rb_state8.var_18 = (rb_state8.var_18) ^ (rb_state8.var_14);
        rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_15) : (0x6f251697));
        rb_state8.var_20 = (rb_state8.var_20) ^ (((0x7c5c14de) - (rb_state8.var_16)) ^ (rb_state8.var_9));
        rb_state8.var_21 = (rb_state8.var_21) + (((((0x6798cbf1) - (rb_state8.var_18)) - (0x9facc688)) + (rb_state8.var_17)) ^ (0x2746ef6f));
        rb_state8.var_22 = (rb_state8.var_22) + (rb_state8.var_19);
        rb_state8.var_23 = (rb_state8.var_23) ^ (((0x5fb23598) ^ (rb_state8.var_10)) ^ (rb_state8.var_20));
        rb_state8.var_24 = (rb_state8.var_24) + (((0xd3cba8f4) - (rb_state8.var_21)) + (rb_state8.var_11));
        rb_state8.var_25 = (rb_state8.var_25) ^ (((((0xe6e08fa7) + (0x29ec9380)) ^ (rb_state8.var_22)) - (rb_state8.var_23)) ^ (0x586b2f09));
        rb_state8.var_26 = (rb_state8.var_26) ^ (((((0x6861e076) - (rb_state8.var_25)) - (rb_state8.var_12)) ^ (rb_state8.var_24)) ^ (0x4dd18ac9));
        rb_state8.var_13 = (rb_state8.var_13) - (rb_state8.var_26);
        rb_state8.var_3 = rb_state8.var_13;
    }
    if ((rb_state8.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_32));
        rb_state8.var_28 = 0x8d3dedf8;
        rb_state8.var_29 = 0xc23670b3;
        rb_state8.var_30 = (rb_state8.var_30) + (((rb_state8.var_28) == (0x0)) ? (rb_state8.var_29) : (0x37461f3f));
        rb_state8.var_31 = (rb_state8.var_31) - (((((0x1f982df5) ^ (rb_state8.var_13)) + (rb_state8.var_14)) ^ (0xfe37ffbd)) ^ (rb_state8.var_30));
        rb_state8.var_27 = (rb_state8.var_27) ^ (rb_state8.var_31);
        rb_state8.var_3 = rb_state8.var_27;
        pthread_mutex_unlock(&(rb_state8.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_0);
    if ((rb_state9.var_0) == (0x6d8ca02f)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        rb_state9.var_19 = 0xafcd99b0;
        rb_state9.var_20 = (rb_state9.var_20) ^ (((((0xa65035bd) - (rb_state9.var_19)) + (rb_state9.var_13)) ^ (rb_state9.var_11)) ^ (0x316d34ec));
        rb_state9.var_21 = (rb_state9.var_21) ^ (rb_state9.var_14);
        rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_20) : (0x61c03e4c));
        rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_21) : (0x357aa1be));
        rb_state9.var_24 = (rb_state9.var_24) ^ (rb_state9.var_22);
        rb_state9.var_25 = (rb_state9.var_25) ^ (rb_state9.var_23);
        rb_state9.var_26 = (rb_state9.var_26) + (((0x42d6d2e5) - (rb_state9.var_24)) - (0x903ad766));
        rb_state9.var_27 = (rb_state9.var_27) - (((0x13ace8d2) - (rb_state9.var_16)) ^ (rb_state9.var_25));
        rb_state9.var_28 = (rb_state9.var_28) - (rb_state9.var_26);
        rb_state9.var_29 = (rb_state9.var_29) + (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0xfecd280b));
        rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_29);
        rb_state9.var_3 = rb_state9.var_18;
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xcc95058a)) {
        rb_state10.var_15 = 0xc865574c;
        rb_state10.var_16 = 0xc58c05c;
        rb_state10.var_17 = (rb_state10.var_17) ^ (((rb_state10.var_16) == (0xc58c05c)) ? (rb_state10.var_12) : (0x9a1694be));
        rb_state10.var_18 = (rb_state10.var_18) ^ (((rb_state10.var_15) == (0xc865574c)) ? (rb_state10.var_17) : (0xaa65bec3));
        rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_18);
    }
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + (90398 < rb_input_size ? (uint32_t)rb_input[90398] : 0x7b2f94aa);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (3331 < rb_input_size ? (uint32_t)rb_input[3331] : 0x23bea1c);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xe4b5826f)) {
        pthread_mutex_lock(&(rb_state6.lock_21));
        rb_state6.var_5 = 0x303c8790;
        rb_state6.var_6 = (rb_state6.var_6) - (33757 < rb_input_size ? (uint32_t)rb_input[33757] : 0x6185340d);
        rb_state6.var_7 = (rb_state6.var_7) - (((((0x8787d23a) + (rb_state6.var_3)) + (rb_state6.var_5)) ^ (rb_state6.var_6)) ^ (rb_state6.var_4));
        rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0x6f277544));
        rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_7) : (0xa2f3d4b5));
        rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_8) : (0xa60049e7));
        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_9) : (0x96b96eaf));
        rb_state6.var_12 = (rb_state6.var_12) + (((((0x75d6cdb0) - (rb_state6.var_11)) + (rb_state6.var_11)) + (rb_state6.var_10)) ^ (rb_state6.var_10));
        rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_12);
        rb_state6.var_3 = rb_state6.var_4;
        pthread_mutex_unlock(&(rb_state6.lock_21));
    }
    if ((rb_state6.var_2) == (0xe4b5826f)) {
        if (!((rb_state6.var_3) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x6321b0e0));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x7cf8b20)) {
        pthread_mutex_lock(&(rb_state14.lock_21));
        rb_state14.var_5 = 0xe24d2538;
        rb_state14.var_6 = (rb_state14.var_6) + (((((0x85421364) + (rb_state14.var_6)) + (rb_state14.var_3)) - (rb_state14.var_5)) + (rb_state14.var_5));
        rb_state14.var_7 = (rb_state14.var_7) ^ (rb_state14.var_4);
        rb_state14.var_8 = (rb_state14.var_8) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_6) : (0x1de0e925));
        rb_state14.var_9 = (rb_state14.var_9) + (rb_state14.var_7);
        rb_state14.var_10 = (rb_state14.var_10) + (((((0x1775256a) - (rb_state14.var_9)) ^ (0xe1728640)) - (rb_state14.var_8)) - (rb_state14.var_8));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_10);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_21));
    }
    if ((rb_state14.var_0) == (0x7cf8b20)) {
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (2478 < rb_input_size ? (uint32_t)rb_input[2478] : 0xd5a03a4c);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (167017 < rb_input_size ? (uint32_t)rb_input[167017] : 0x8310526e);
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x7a1c82d1)) {
        rb_state0.var_11 = 0x76db5a5d;
        rb_state0.var_12 = 0xfec651e6;
        rb_state0.var_13 = (rb_state0.var_13) ^ (((((0xa587fc05) + (rb_state0.var_12)) + (0x1a1cf842)) ^ (rb_state0.var_7)) - (rb_state0.var_11));
        rb_state0.var_14 = (rb_state0.var_14) + (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_8) : (0x93bef427));
        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_6);
        rb_state0.var_16 = (rb_state0.var_16) ^ (((((0x30fcc1c2) ^ (rb_state0.var_14)) ^ (rb_state0.var_10)) - (rb_state0.var_13)) + (rb_state0.var_11));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x93e15862));
        rb_state0.var_10 = (rb_state0.var_10) ^ (rb_state0.var_17);
        rb_state0.var_4 = rb_state0.var_10;
    }
    if ((rb_state0.var_3) == (0x7a1c82d1)) {
        if ((rb_state0.var_4) != (0x0)) {
            if (!((rb_state0.var_4) == (rb_state0.var_18))) {
                racebench_trigger(0);
            }
        }
    }
    if ((rb_state0.var_3) == (0x7a1c82d1)) {
        pthread_mutex_lock(&(rb_state0.lock_31));
        rb_state0.var_19 = (rb_state0.var_19) + (rb_state0.var_13);
        rb_state0.var_20 = (rb_state0.var_20) + (rb_state0.var_15);
        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_16) == (0xa7d87ba5)) ? (rb_state0.var_12) : (0xa52266b0));
        rb_state0.var_22 = (rb_state0.var_22) - (((0xd047590b) ^ (rb_state0.var_14)) - (rb_state0.var_17));
        rb_state0.var_23 = (rb_state0.var_23) - (rb_state0.var_19);
        rb_state0.var_24 = (rb_state0.var_24) ^ (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0x704c1b0e));
        rb_state0.var_25 = (rb_state0.var_25) ^ (rb_state0.var_22);
        rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_23) : (0xe3db812));
        rb_state0.var_27 = (rb_state0.var_27) + (((0x7ba55069) - (rb_state0.var_24)) - (0x3ba2b1d));
        rb_state0.var_28 = (rb_state0.var_28) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_25) : (0xfe765e37));
        rb_state0.var_29 = (rb_state0.var_29) + (((((0x6ac61203) - (0x15cd489c)) + (rb_state0.var_26)) + (rb_state0.var_27)) + (rb_state0.var_20));
        rb_state0.var_30 = (rb_state0.var_30) - (((rb_state0.var_28) == (0xf748f2df)) ? (rb_state0.var_29) : (0x738927f8));
        rb_state0.var_18 = (rb_state0.var_18) ^ (rb_state0.var_30);
        rb_state0.var_4 = rb_state0.var_18;
        pthread_mutex_unlock(&(rb_state0.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + ((0xc41d7bda) ^ (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (0xe4b5826f);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0x0)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_27))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x6d8ca02f)) {
        if ((rb_state9.var_3) != (0x0)) {
            if (!((rb_state9.var_3) == (rb_state9.var_18))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ ((0x7751309c) - (rb_state12.var_0));
    #endif
    PreAllocateFO(MyNum, lc);

    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x0)) {
            rb_state7.var_9 = 0xcb225642;
            rb_state7.var_10 = 0xdffa50bf;
            rb_state7.var_11 = (rb_state7.var_11) ^ ((0x4a903522) - (rb_state7.var_9));
            rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_10) == (0xdffa50bf)) ? (rb_state7.var_8) : (0xb7727ef4));
            rb_state7.var_13 = (rb_state7.var_13) ^ (((0xa7b0448d) + (0xf032011d)) + (rb_state7.var_10));
            rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_9) == (0xcb225642)) ? (rb_state7.var_11) : (0xcccb2c6));
            rb_state7.var_15 = (rb_state7.var_15) + (((rb_state7.var_11) == (0x7f6ddee0)) ? (rb_state7.var_12) : (0x1759678b));
            rb_state7.var_16 = (rb_state7.var_16) + (((rb_state7.var_12) == (0x691c0b4f)) ? (rb_state7.var_13) : (0xc76f69a3));
            rb_state7.var_17 = (rb_state7.var_17) ^ (((0x3cce1b73) + (0xb8ffe3bd)) - (rb_state7.var_14));
            rb_state7.var_18 = (rb_state7.var_18) ^ (((0x2dee9c09) ^ (rb_state7.var_13)) - (rb_state7.var_15));
            rb_state7.var_19 = (rb_state7.var_19) + (((rb_state7.var_14) == (0x7f6ddee0)) ? (rb_state7.var_16) : (0x2f87c20b));
            rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_17) == (0x76602050)) ? (rb_state7.var_18) : (0xc3b05c04));
            rb_state7.var_21 = (rb_state7.var_21) + (((rb_state7.var_19) == (0x77dc9669)) ? (rb_state7.var_20) : (0xa879b2b0));
            rb_state7.var_4 = (rb_state7.var_4) + (rb_state7.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state18.lock_26));
            rb_state18.var_6 = 0xe3196d1;
            rb_state18.var_7 = 0x707dc8b9;
            rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0xe3473e4e));
            rb_state18.var_9 = (rb_state18.var_9) + (rb_state18.var_5);
            rb_state18.var_10 = (rb_state18.var_10) ^ (((0xb4b4dbff) ^ (rb_state18.var_4)) ^ (0x6703c053));
            rb_state18.var_11 = (rb_state18.var_11) + (((((0xc7d6d4e) + (0x6a4a3476)) - (rb_state18.var_9)) - (rb_state18.var_8)) + (0xfcb8a7f7));
            rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_10);
            rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_11) : (0xf4b86775));
            rb_state18.var_14 = (rb_state18.var_14) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_12) : (0xb5d7ff61));
            rb_state18.var_15 = (rb_state18.var_15) + (((0xd23ad72b) + (0x3abb4241)) + (rb_state18.var_13));
            rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_14);
            rb_state18.var_17 = (rb_state18.var_17) ^ (((0xd5d17f2f) - (rb_state18.var_8)) - (rb_state18.var_15));
            rb_state18.var_18 = (rb_state18.var_18) ^ (((((0xa12b4b2a) + (rb_state18.var_16)) ^ (rb_state18.var_10)) - (rb_state18.var_9)) + (rb_state18.var_17));
            rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_18);
            rb_state18.var_4 = rb_state18.var_5;
            pthread_mutex_unlock(&(rb_state18.lock_26));
        }
        if ((rb_state18.var_1) == (0x0)) {
            if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                racebench_trigger(18);
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
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_3) == (0x8864570a)) {
                pthread_mutex_lock(&(rb_state15.lock_28));
                rb_state15.var_6 = 0x637711c3;
                rb_state15.var_7 = 0x6926872c;
                rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_7) == (0x6926872c)) ? (rb_state15.var_6) : (0x3d632563));
                rb_state15.var_9 = (rb_state15.var_9) ^ (rb_state15.var_5);
                rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_6) == (0x637711c3)) ? (rb_state15.var_4) : (0x934b237d));
                rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_7) == (0x6926872c)) ? (rb_state15.var_8) : (0xd494f95c));
                rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_8) == (0x637711c3)) ? (rb_state15.var_9) : (0x5f1fcd4f));
                rb_state15.var_13 = (rb_state15.var_13) ^ (((0x67646125) ^ (rb_state15.var_10)) - (rb_state15.var_9));
                rb_state15.var_14 = (rb_state15.var_14) - (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0xc28e0db));
                rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_11) == (0x9c88ee3d)) ? (rb_state15.var_12) : (0xc4e8b6ae));
                rb_state15.var_16 = (rb_state15.var_16) + (((((0xdea85fcd) ^ (0x2c310a77)) + (0xb7366d2f)) - (rb_state15.var_14)) + (rb_state15.var_13));
                rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_15) : (0x3f0c3995));
                rb_state15.var_18 = (rb_state15.var_18) - (((rb_state15.var_13) == (0x67646125)) ? (rb_state15.var_16) : (0x2b4067f6));
                rb_state15.var_19 = (rb_state15.var_19) ^ (((rb_state15.var_14) == (0x637711c3)) ? (rb_state15.var_17) : (0x9a48ef42));
                rb_state15.var_20 = (rb_state15.var_20) ^ (((((0x8646b58e) ^ (rb_state15.var_18)) - (0x54bea93e)) ^ (rb_state15.var_19)) ^ (rb_state15.var_15));
                rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_20);
                rb_state15.var_4 = rb_state15.var_5;
                pthread_mutex_unlock(&(rb_state15.lock_28));
            }
            if ((rb_state15.var_3) == (0x55968d00)) {
                if (!((rb_state15.var_4) == (rb_state15.var_5))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_1) == (0x0)) {
                rb_state18.var_19 = 0x3f8b6965;
                rb_state18.var_20 = 0x9b789dca;
                rb_state18.var_21 = (rb_state18.var_21) + (0xfbc9e91a);
                rb_state18.var_22 = (rb_state18.var_22) + (((rb_state18.var_19) == (0x3f8b6965)) ? (rb_state18.var_11) : (0xad57d));
                rb_state18.var_23 = (rb_state18.var_23) ^ (((((0xe4d671d) + (rb_state18.var_21)) ^ (rb_state18.var_20)) + (0xc10d1d57)) - (0x6734eaf));
                rb_state18.var_24 = (rb_state18.var_24) - (rb_state18.var_22);
                rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_23) == (0x4c099ca5)) ? (rb_state18.var_24) : (0xc4663b94));
                rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_25);
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