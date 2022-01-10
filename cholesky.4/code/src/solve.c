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
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - (0x19d19110);
            rb_state18.var_0 = (rb_state18.var_0) ^ (0xe0aaffb3);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (0x440c5042);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (86375 < rb_input_size ? (uint32_t)rb_input[86375] : 0x26064208);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0xc671d6d0);
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
        rb_state0.var_0 = (rb_state0.var_0) ^ (40117 < rb_input_size ? (uint32_t)rb_input[40117] : 0xd587539d);
        rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_0) : (0x32b4e0e8));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (27981 < rb_input_size ? (uint32_t)rb_input[27981] : 0xd505ea19);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) - (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_3) : (0xe970b8f0));
        rb_state4.var_3 = (rb_state4.var_3) + (0x8440c930);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (170194 < rb_input_size ? (uint32_t)rb_input[170194] : 0xae179eb9);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (0x69ab1e3a);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - ((0x86f0db19) - (0x3755f548));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x94f1e7bf));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ ((0xd6cd16e5) ^ (rb_state14.var_1));
        rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xb237cc33));
        rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_1);
        rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_2);
        rb_state15.var_3 = (rb_state15.var_3) ^ (0x68cdeae);
        rb_state15.var_0 = (rb_state15.var_0) + (0x65d016c9);
        if ((rb_state15.var_1) == (0xffffff00)) {
            rb_state15.var_15 = 0x880ef7e1;
            rb_state15.var_16 = 0x3b30ed76;
            rb_state15.var_17 = (rb_state15.var_17) + (((((0x2050d4e6) - (0x402535ce)) - (rb_state15.var_11)) + (rb_state15.var_16)) - (0x35be4dda));
            rb_state15.var_18 = (rb_state15.var_18) + (7361 < rb_input_size ? (uint32_t)rb_input[7361] : 0x8bcd971b);
            rb_state15.var_19 = (rb_state15.var_19) ^ (((((0x8077ae42) + (rb_state15.var_12)) ^ (rb_state15.var_15)) + (rb_state15.var_17)) ^ (0xdc67d46b));
            rb_state15.var_20 = (rb_state15.var_20) - (((0xadbd3b9e) - (rb_state15.var_13)) - (rb_state15.var_18));
            rb_state15.var_21 = (rb_state15.var_21) - (((((0x76518d01) + (rb_state15.var_19)) - (0xa0180304)) - (rb_state15.var_14)) ^ (rb_state15.var_20));
            rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - ((0xb40c9381) + (rb_state16.var_0));
        rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0x313bcf59));
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_0);
        rb_state16.var_2 = (rb_state16.var_2) + (0xd53de896);
        if ((rb_state16.var_2) == (0x35ee418f)) {
            rb_state16.var_10 = 0xd4336263;
            rb_state16.var_11 = 0x1f186edb;
            rb_state16.var_12 = (rb_state16.var_12) ^ (rb_state16.var_6);
            rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_7);
            rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_10) : (0xa36150f4));
            rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_11) : (0x46493bb0));
            rb_state16.var_16 = (rb_state16.var_16) ^ (rb_state16.var_12);
            rb_state16.var_17 = (rb_state16.var_17) + (((0x99d126d2) - (rb_state16.var_9)) + (rb_state16.var_13));
            rb_state16.var_18 = (rb_state16.var_18) ^ (((0x6c003183) + (rb_state16.var_14)) ^ (0xca7842ef));
            rb_state16.var_19 = (rb_state16.var_19) + (((((0xdb07c364) - (rb_state16.var_16)) + (0x84bf009)) - (rb_state16.var_15)) + (rb_state16.var_10));
            rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_18) : (0xf3ec7dca));
            rb_state16.var_21 = (rb_state16.var_21) - (rb_state16.var_19);
            rb_state16.var_22 = (rb_state16.var_22) - (((((0x7f63b133) + (rb_state16.var_11)) + (rb_state16.var_21)) - (rb_state16.var_12)) + (rb_state16.var_20));
            rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_22);
            rb_state16.var_3 = rb_state16.var_9;
        }
        if ((rb_state16.var_2) == (0x35ee418f)) {
            pthread_mutex_lock(&(rb_state16.lock_27));
            rb_state16.var_24 = 0xef3cd12f;
            rb_state16.var_25 = (rb_state16.var_25) ^ (((((0x2ec39ca4) - (0x9e595ef7)) - (rb_state16.var_13)) ^ (rb_state16.var_14)) ^ (0xd6bde0e1));
            rb_state16.var_26 = (rb_state16.var_26) + (((((0x63d72f8c) + (0x7a71a947)) + (rb_state16.var_25)) + (rb_state16.var_24)) + (0x29e0aace));
            rb_state16.var_23 = (rb_state16.var_23) + (rb_state16.var_26);
            rb_state16.var_3 = rb_state16.var_23;
            pthread_mutex_unlock(&(rb_state16.lock_27));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ ((0x7f86b407) - (rb_state18.var_0));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (175261 < rb_input_size ? (uint32_t)rb_input[175261] : 0x28ea06e9);
        rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0xd3ffd4de));
        rb_state19.var_0 = (rb_state19.var_0) + ((0x5a736087) ^ (0xf679d956));
        rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0x490e94a));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (66883 < rb_input_size ? (uint32_t)rb_input[66883] : 0xe64acf83);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + ((0x1a67c10d) - (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_1);
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (57259 < rb_input_size ? (uint32_t)rb_input[57259] : 0xef9802dd);
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (0x98b7ad4d);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (47846 < rb_input_size ? (uint32_t)rb_input[47846] : 0x372af7f5);
    if ((rb_state14.var_1) == (0xd6cd16e5)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_16 = 0x8674828f;
        rb_state14.var_17 = 0x3a797a07;
        rb_state14.var_18 = 0x4d0c2a75;
        rb_state14.var_19 = (rb_state14.var_19) - ((0x793fd8b2) ^ (0xf0170221));
        rb_state14.var_20 = (rb_state14.var_20) - (((((0x8e80715c) - (rb_state14.var_14)) - (rb_state14.var_16)) ^ (rb_state14.var_17)) + (0x66456aaf));
        rb_state14.var_21 = (rb_state14.var_21) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_18) : (0x800da8ee));
        rb_state14.var_22 = (rb_state14.var_22) - (rb_state14.var_19);
        rb_state14.var_23 = (rb_state14.var_23) ^ (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_21) : (0x3278172c));
        rb_state14.var_24 = (rb_state14.var_24) - (((0xa9d8b28b) ^ (rb_state14.var_16)) - (rb_state14.var_22));
        rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_23);
        rb_state14.var_26 = (rb_state14.var_26) ^ (((0xcd8947e4) - (0xd7e9872c)) + (rb_state14.var_24));
        rb_state14.var_27 = (rb_state14.var_27) ^ (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_25) : (0x2691b6b9));
        rb_state14.var_28 = (rb_state14.var_28) - (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_27) : (0x208639ac));
        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_28);
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0xac0ab9d1)) ? (rb_state19.var_0) : (0x147ca2e));
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (40298 < rb_input_size ? (uint32_t)rb_input[40298] : 0x87798588);
    rb_state4.var_2 = (rb_state4.var_2) - (128160 < rb_input_size ? (uint32_t)rb_input[128160] : 0xbc7c1c3f);
    if ((rb_state4.var_1) == (0x0)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_11))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x2a2b3722) ^ (0xd8db36d8));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_1) : (0x89f63ec8));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x69ab1e3a)) ? (rb_state7.var_2) : (0xf6505b67));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (115581 < rb_input_size ? (uint32_t)rb_input[115581] : 0xe81acee6);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - ((0x8b1e9142) - (0x2df12ff5));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (1524 < rb_input_size ? (uint32_t)rb_input[1524] : 0xa5a28045);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (0x84dda728);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_2) == (0xd08e5ab6)) {
        pthread_mutex_lock(&(rb_state18.lock_18));
        if ((rb_state18.var_3) != (0x0)) {
            if (!((rb_state18.var_3) == (rb_state18.var_7))) {
                racebench_trigger(18);
            }
        }
        pthread_mutex_unlock(&(rb_state18.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x5952b98c)) {
        pthread_mutex_lock(&(rb_state19.lock_15));
        rb_state19.var_4 = rb_state19.var_1;
        pthread_mutex_unlock(&(rb_state19.lock_15));
    }
    if ((rb_state19.var_0) == (0x5952b98c)) {
        if (!((rb_state19.var_1) == (rb_state19.var_4))) {
            racebench_trigger(19);
        }
    }
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + ((0x389dd0e9) + (0xf53226ff));
    if ((rb_state4.var_1) == (0x0)) {
        rb_state4.var_8 = 0x69e62ccc;
        rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_7) : (0xd36b02bf));
        rb_state4.var_10 = (rb_state4.var_10) + (((((0x382f7a46) + (rb_state4.var_9)) + (0xe4883651)) + (rb_state4.var_8)) + (rb_state4.var_8));
        rb_state4.var_7 = (rb_state4.var_7) ^ (rb_state4.var_10);
        rb_state4.var_4 = rb_state4.var_7;
    }
    if ((rb_state4.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_29));
        rb_state4.var_12 = 0x61469d27;
        rb_state4.var_13 = (rb_state4.var_13) ^ (rb_state4.var_10);
        rb_state4.var_14 = (rb_state4.var_14) ^ (((0x3a06c2e6) + (0x6a301257)) ^ (rb_state4.var_12));
        rb_state4.var_15 = (rb_state4.var_15) - (0xd9937530);
        rb_state4.var_16 = (rb_state4.var_16) - (((0xd818b094) - (rb_state4.var_11)) - (rb_state4.var_9));
        rb_state4.var_17 = (rb_state4.var_17) ^ (((0x56533013) ^ (rb_state4.var_12)) ^ (rb_state4.var_13));
        rb_state4.var_18 = (rb_state4.var_18) - (rb_state4.var_14);
        rb_state4.var_19 = (rb_state4.var_19) ^ (((rb_state4.var_15) == (0x266c8ad0)) ? (rb_state4.var_16) : (0x5c4c744c));
        rb_state4.var_20 = (rb_state4.var_20) + (((0x56177a52) - (0xa7e60543)) ^ (rb_state4.var_17));
        rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_18);
        rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_19);
        rb_state4.var_23 = (rb_state4.var_23) + (((rb_state4.var_20) == (0x69a0d214)) ? (rb_state4.var_21) : (0x73a3df76));
        rb_state4.var_24 = (rb_state4.var_24) ^ (((0x273bef89) + (rb_state4.var_13)) ^ (rb_state4.var_22));
        rb_state4.var_25 = (rb_state4.var_25) + (((rb_state4.var_14) == (0xc570481a)) ? (rb_state4.var_23) : (0xd56721c1));
        rb_state4.var_26 = (rb_state4.var_26) + (rb_state4.var_24);
        rb_state4.var_27 = (rb_state4.var_27) - (((rb_state4.var_15) == (0x266c8ad0)) ? (rb_state4.var_25) : (0xdca8bdcd));
        rb_state4.var_28 = (rb_state4.var_28) + (((rb_state4.var_26) == (0x3058b6d4)) ? (rb_state4.var_27) : (0x5f9efe1b));
        rb_state4.var_11 = (rb_state4.var_11) ^ (rb_state4.var_28);
        rb_state4.var_4 = rb_state4.var_11;
        pthread_mutex_unlock(&(rb_state4.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (171546 < rb_input_size ? (uint32_t)rb_input[171546] : 0x5c82b2ae);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ ((0xaef7d735) + (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (67330 < rb_input_size ? (uint32_t)rb_input[67330] : 0x11bb9684);
    if ((rb_state10.var_1) == (0x62deb5b4)) {
        rb_state10.var_4 = rb_state10.var_2;
    }
    if ((rb_state10.var_1) == (0x62deb5b4)) {
        if (!((rb_state10.var_2) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (30077 < rb_input_size ? (uint32_t)rb_input[30077] : 0xebb55c39);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ ((0xd3f65e9a) - (0xebd13715));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (0xde61854d);
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x9ea8d762)) {
        pthread_mutex_lock(&(rb_state7.lock_8));
        rb_state7.var_5 = (rb_state7.var_5) ^ (((((0x71a36c79) - (rb_state7.var_5)) + (rb_state7.var_3)) ^ (0xea710f6d)) ^ (rb_state7.var_4));
        rb_state7.var_4 = (rb_state7.var_4) ^ (rb_state7.var_5);
        rb_state7.var_3 = rb_state7.var_4;
        pthread_mutex_unlock(&(rb_state7.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + ((0x80cb0cd6) - (0x20aadba7));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_2) : (0xb4602a1d));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (0x8e823647);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (0x407a13b7);
    rb_state14.var_2 = (rb_state14.var_2) ^ (((rb_state14.var_2) == (0x62)) ? (rb_state14.var_2) : (0x8559879e));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ ((0x9ea3534) + (0x151fac07));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0xe7471f4f));
    if ((rb_state17.var_2) == (0xdec6c6e9)) {
        rb_state17.var_15 = 0xaa6239af;
        rb_state17.var_16 = (rb_state17.var_16) ^ (((((0x7b73c51e) + (rb_state17.var_15)) + (0xa19e3391)) + (rb_state17.var_11)) ^ (rb_state17.var_12));
        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_16) : (0x23324f74));
        rb_state17.var_14 = (rb_state17.var_14) - (rb_state17.var_17);
        rb_state17.var_4 = rb_state17.var_14;
    }
    if ((rb_state17.var_2) == (0xdec6c6e9)) {
        pthread_mutex_lock(&(rb_state17.lock_33));
        rb_state17.var_19 = 0x2454ce63;
        rb_state17.var_20 = (rb_state17.var_20) - ((0xa8aa4c2e) - (0x1ad13b56));
        rb_state17.var_21 = (rb_state17.var_21) ^ (((((0x61044ed8) + (0xa174edaf)) ^ (rb_state17.var_19)) + (rb_state17.var_14)) - (rb_state17.var_15));
        rb_state17.var_22 = (rb_state17.var_22) ^ (0x236ded5a);
        rb_state17.var_23 = (rb_state17.var_23) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_20) : (0x68d4bd3));
        rb_state17.var_24 = (rb_state17.var_24) + (((((0xfadf83f9) + (rb_state17.var_22)) + (rb_state17.var_21)) - (rb_state17.var_16)) + (0x7090738e));
        rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_23);
        rb_state17.var_26 = (rb_state17.var_26) + (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_24) : (0xb0b0445a));
        rb_state17.var_27 = (rb_state17.var_27) - (rb_state17.var_25);
        rb_state17.var_28 = (rb_state17.var_28) ^ (rb_state17.var_26);
        rb_state17.var_29 = (rb_state17.var_29) - (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_27) : (0x4bea23a5));
        rb_state17.var_30 = (rb_state17.var_30) - (((0xded3b36d) ^ (rb_state17.var_28)) ^ (rb_state17.var_19));
        rb_state17.var_31 = (rb_state17.var_31) ^ (rb_state17.var_29);
        rb_state17.var_32 = (rb_state17.var_32) ^ (((rb_state17.var_30) == (0x0)) ? (rb_state17.var_31) : (0x88641fb8));
        rb_state17.var_18 = (rb_state17.var_18) - (rb_state17.var_32);
        rb_state17.var_4 = rb_state17.var_18;
        pthread_mutex_unlock(&(rb_state17.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - ((0x2f71a54a) + (rb_state18.var_2));
    rb_state18.var_1 = (rb_state18.var_1) ^ ((0xc30e752e) + (rb_state18.var_2));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_1);
    rb_state19.var_3 = (rb_state19.var_3) - ((0xa474ba4e) + (rb_state19.var_3));
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ ((0xc2743456) ^ (0xb1618e2e));
    rb_state0.var_2 = (rb_state0.var_2) ^ ((0x29133589) - (rb_state0.var_2));
    rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (95743 < rb_input_size ? (uint32_t)rb_input[95743] : 0x39e65051);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_1) == (0xaef7d755)) ? (rb_state8.var_0) : (0xb4dcd3bc));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_0);
    if ((rb_state13.var_3) == (0x0)) {
        if ((rb_state13.var_4) != (0x0)) {
            if (!((rb_state13.var_4) == (rb_state13.var_29))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (0x3ba81a1a);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0xffffff00)) {
        pthread_mutex_lock(&(rb_state15.lock_22));
        rb_state15.var_6 = 0xef74b0c3;
        rb_state15.var_7 = (rb_state15.var_7) ^ (rb_state15.var_5);
        rb_state15.var_8 = (rb_state15.var_8) + (53888 < rb_input_size ? (uint32_t)rb_input[53888] : 0x5b3da38f);
        rb_state15.var_9 = (rb_state15.var_9) ^ (((((0xf7096fb8) + (rb_state15.var_7)) + (rb_state15.var_6)) ^ (rb_state15.var_6)) ^ (rb_state15.var_4));
        rb_state15.var_10 = (rb_state15.var_10) ^ (((((0x169c04d8) - (rb_state15.var_8)) + (rb_state15.var_7)) ^ (0xd610ff52)) + (rb_state15.var_8));
        rb_state15.var_11 = (rb_state15.var_11) + (((0x5d6388d5) - (rb_state15.var_9)) - (0xd8f1acfe));
        rb_state15.var_12 = (rb_state15.var_12) + (((0xccca8d8b) ^ (rb_state15.var_10)) - (rb_state15.var_9));
        rb_state15.var_13 = (rb_state15.var_13) + (((0x89479a1a) ^ (rb_state15.var_11)) + (rb_state15.var_10));
        rb_state15.var_14 = (rb_state15.var_14) + (((((0x77bc30d8) + (rb_state15.var_13)) ^ (rb_state15.var_12)) + (0x2b250226)) + (0x4e7701c8));
        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_14);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_22));
    }
    if ((rb_state15.var_1) == (0xffffff00)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x35ee418f)) {
        if ((rb_state16.var_3) != (0x0)) {
            if (!((rb_state16.var_3) == (rb_state16.var_23))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) + (0xf2a5a6ea);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_0) == (0x6849143)) ? (rb_state18.var_1) : (0xf76473ba));
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0xe8d23541) - (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_2) == (0x35)) ? (rb_state7.var_1) : (0x444835b8));
    rb_state7.var_0 = (rb_state7.var_0) + (0x61a77ce5);
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x9ea8d762)) ? (rb_state7.var_2) : (0x1a8080ba));
    if ((rb_state7.var_0) == (0x25b33cc9)) {
        if (!((rb_state7.var_3) == (rb_state7.var_4))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + ((0xe2265278) + (0x76364ced));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0xd6cd16e5)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_5 = 0x90419545;
        rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_4) : (0xd1f87ea7));
        rb_state14.var_7 = (rb_state14.var_7) + (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_5) : (0x6c795647));
        rb_state14.var_8 = (rb_state14.var_8) + (((0xda7667b1) - (rb_state14.var_6)) - (rb_state14.var_6));
        rb_state14.var_9 = (rb_state14.var_9) - (((0x53e7b240) ^ (rb_state14.var_7)) + (rb_state14.var_7));
        rb_state14.var_10 = (rb_state14.var_10) - (((0xa256a9fe) + (rb_state14.var_8)) + (rb_state14.var_8));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_9) : (0xf5f08dd3));
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_10) : (0xe5db4d52));
        rb_state14.var_13 = (rb_state14.var_13) - (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_11) : (0x918412f8));
        rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_12) : (0xbbe2b9));
        rb_state14.var_15 = (rb_state14.var_15) + (((((0x6168dd22) + (rb_state14.var_13)) ^ (rb_state14.var_14)) ^ (rb_state14.var_13)) + (0x9f2f81fe));
        rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_15);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    if ((rb_state14.var_1) == (0xd6cd16e5)) {
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_2) == (0xd08e5ab6)) {
        rb_state18.var_5 = 0xfaefb292;
        rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_5) : (0x6bd21245));
        rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_6);
        rb_state18.var_3 = rb_state18.var_4;
    }
    if ((rb_state18.var_2) == (0xd08e5ab6)) {
        pthread_mutex_lock(&(rb_state18.lock_18));
        rb_state18.var_8 = 0x286db625;
        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_8) : (0x3965690a));
        rb_state18.var_10 = (rb_state18.var_10) - (33760 < rb_input_size ? (uint32_t)rb_input[33760] : 0xd8613e6a);
        rb_state18.var_11 = (rb_state18.var_11) + (rb_state18.var_7);
        rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_9);
        rb_state18.var_13 = (rb_state18.var_13) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_10) : (0x658c6f03));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_11) : (0x92df0d22));
        rb_state18.var_15 = (rb_state18.var_15) ^ (((0xe123d094) ^ (rb_state18.var_12)) - (rb_state18.var_10));
        rb_state18.var_16 = (rb_state18.var_16) ^ (((((0xdf1fff52) ^ (rb_state18.var_14)) + (0xc8b9841c)) - (0xf0a4b2b1)) - (rb_state18.var_13));
        rb_state18.var_17 = (rb_state18.var_17) ^ (((((0xb0c52fa5) ^ (0x7054bc64)) ^ (rb_state18.var_15)) ^ (rb_state18.var_16)) - (0x8d621bbc));
        rb_state18.var_7 = (rb_state18.var_7) + (rb_state18.var_17);
        rb_state18.var_3 = rb_state18.var_7;
        pthread_mutex_unlock(&(rb_state18.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0xfc74b75b);
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0xe4a5b1c6) + (rb_state2.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_2);
    if ((rb_state7.var_0) == (0x9ea8d762)) {
        rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_4) == (0x9bd26314)) ? (rb_state7.var_3) : (0xb0049691));
        rb_state7.var_7 = (rb_state7.var_7) - (((((0xb0faeb4e) - (rb_state7.var_5)) + (0x7dc40d93)) ^ (rb_state7.var_6)) + (rb_state7.var_6));
        rb_state7.var_3 = (rb_state7.var_3) ^ (rb_state7.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_1) == (0x316f5aba)) ? (rb_state10.var_0) : (0x3305f81a));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + ((0xfd15b6ea) + (rb_state11.var_2));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (0x631a5192);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ ((0x2a3396a9) - (rb_state17.var_1));
    #endif
    PreAllocateFO(MyNum, lc);

    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_9
        rb_state9.var_10 = (rb_state9.var_10) - ((0xb8f66e01) + (0xba02ae3f));
        rb_state9.var_11 = (rb_state9.var_11) - (0x50cd4076);
        #endif
        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_7) == (0x5e0670dd)) {
            pthread_mutex_lock(&(rb_state9.lock_41));
            rb_state9.var_19 = 0x40043c5c;
            rb_state9.var_20 = 0x2bf6c382;
            rb_state9.var_21 = (rb_state9.var_21) + (((rb_state9.var_14) == (0x280e3858)) ? (rb_state9.var_9) : (0x693dd141));
            rb_state9.var_22 = (rb_state9.var_22) - (((rb_state9.var_20) == (0x2bf6c382)) ? (rb_state9.var_8) : (0x8e5bf9bb));
            rb_state9.var_23 = (rb_state9.var_23) ^ (((rb_state9.var_19) == (0x40043c5c)) ? (rb_state9.var_21) : (0x4f4c9788));
            rb_state9.var_24 = (rb_state9.var_24) + (((0x8ffad3f2) - (rb_state9.var_22)) + (rb_state9.var_16));
            rb_state9.var_25 = (rb_state9.var_25) - (((((0xcf84220b) + (0x60f5fb33)) ^ (0xa4ea9224)) - (rb_state9.var_23)) ^ (rb_state9.var_24));
            rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_25);
            rb_state9.var_17 = rb_state9.var_18;
            pthread_mutex_unlock(&(rb_state9.lock_41));
        }
        if ((rb_state9.var_7) == (0x5e0670dd)) {
            if (!((rb_state9.var_17) == (rb_state9.var_18))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_3) == (0x0)) {
            rb_state13.var_15 = 0xb2b0cb80;
            rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_15) : (0x88e20919));
            rb_state13.var_17 = (rb_state13.var_17) + (((0x2c82bd84) - (rb_state13.var_11)) - (rb_state13.var_13));
            rb_state13.var_18 = (rb_state13.var_18) + (0x5802252);
            rb_state13.var_19 = (rb_state13.var_19) - (((0xcad6a244) ^ (rb_state13.var_10)) + (0x29e3420f));
            rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_16);
            rb_state13.var_21 = (rb_state13.var_21) - (((0xd4e55716) ^ (rb_state13.var_17)) - (0x6c2acab3));
            rb_state13.var_22 = (rb_state13.var_22) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_18) : (0xa409fd5c));
            rb_state13.var_23 = (rb_state13.var_23) ^ (((0x918b47bd) ^ (0xa79c8111)) - (rb_state13.var_19));
            rb_state13.var_24 = (rb_state13.var_24) - (((((0xef5ab755) - (rb_state13.var_21)) ^ (rb_state13.var_20)) + (0x388093d7)) + (rb_state13.var_15));
            rb_state13.var_25 = (rb_state13.var_25) - (((0x8c6ce9b1) ^ (rb_state13.var_16)) + (rb_state13.var_22));
            rb_state13.var_26 = (rb_state13.var_26) ^ (((rb_state13.var_23) == (0x2ad1aaff)) ? (rb_state13.var_24) : (0x5458b56d));
            rb_state13.var_27 = (rb_state13.var_27) + (rb_state13.var_25);
            rb_state13.var_28 = (rb_state13.var_28) ^ (((((0x4fe62ad6) + (0x7dd2a6f3)) ^ (0xf52349d9)) + (rb_state13.var_26)) ^ (rb_state13.var_27));
            rb_state13.var_14 = (rb_state13.var_14) + (rb_state13.var_28);
            rb_state13.var_4 = rb_state13.var_14;
        }
        if ((rb_state13.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_43));
            rb_state13.var_30 = 0x89aad4d3;
            rb_state13.var_31 = (rb_state13.var_31) ^ (148344 < rb_input_size ? (uint32_t)rb_input[148344] : 0x2a4fdd53);
            rb_state13.var_32 = (rb_state13.var_32) - (((0xc6ca28aa) + (rb_state13.var_17)) - (rb_state13.var_19));
            rb_state13.var_33 = (rb_state13.var_33) - (((rb_state13.var_20) == (0xb2b0cb80)) ? (rb_state13.var_18) : (0x521ac178));
            rb_state13.var_34 = (rb_state13.var_34) + (rb_state13.var_30);
            rb_state13.var_35 = (rb_state13.var_35) + (rb_state13.var_31);
            rb_state13.var_36 = (rb_state13.var_36) + (((((0xaebe2cc6) ^ (0xd0db65e6)) + (rb_state13.var_33)) + (rb_state13.var_32)) - (rb_state13.var_21));
            rb_state13.var_37 = (rb_state13.var_37) ^ (((0x57e6727d) ^ (rb_state13.var_22)) ^ (rb_state13.var_34));
            rb_state13.var_38 = (rb_state13.var_38) + (((rb_state13.var_23) == (0x2ad1aaff)) ? (rb_state13.var_35) : (0x3a4c5912));
            rb_state13.var_39 = (rb_state13.var_39) + (((rb_state13.var_36) == (0x1d1b7c2c)) ? (rb_state13.var_37) : (0x20f88bb8));
            rb_state13.var_40 = (rb_state13.var_40) - (((rb_state13.var_24) == (0x4ba783f5)) ? (rb_state13.var_38) : (0x57a2626a));
            rb_state13.var_41 = (rb_state13.var_41) ^ (rb_state13.var_39);
            rb_state13.var_42 = (rb_state13.var_42) ^ (((((0x6f9c5afa) - (rb_state13.var_25)) + (rb_state13.var_40)) - (0xf1905070)) + (rb_state13.var_41));
            rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_42);
            rb_state13.var_4 = rb_state13.var_29;
            pthread_mutex_unlock(&(rb_state13.lock_43));
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