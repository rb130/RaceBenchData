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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (148410 < rb_input_size ? (uint32_t)rb_input[148410] : 0x3996192f);
            rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_0);
            rb_state0.var_1 = (rb_state0.var_1) + ((0x42b996a6) + (0x41186b1f));
            rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_2);
            rb_state0.var_1 = (rb_state0.var_1) + ((0x630d0659) ^ (0xb12efb88));
            if ((rb_state0.var_2) == (0x0)) {
                if ((rb_state0.var_3) != (0x0)) {
                    if (!((rb_state0.var_3) == (rb_state0.var_31))) {
                        racebench_trigger(0);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xbfe55066));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (80683 < rb_input_size ? (uint32_t)rb_input[80683] : 0x39dea7ac);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_1);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) ^ (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_2) : (0x65179a28));
        if ((rb_state1.var_2) == (0x0)) {
            rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_3) : (0x916df39f));
            rb_state1.var_15 = (rb_state1.var_15) + (((rb_state1.var_13) == (0xbf3e3d1c)) ? (rb_state1.var_12) : (0xeb21e279));
            rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_15) == (0x1420e366)) ? (rb_state1.var_14) : (0x6efc9c87));
            rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_15) == (0x1420e366)) ? (rb_state1.var_16) : (0x5fbf607b));
            rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_17);
        }
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            if ((rb_state1.var_19) != (0x0)) {
                if (!((rb_state1.var_19) == (rb_state1.var_34))) {
                    racebench_trigger(1);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ ((0xece88cc) ^ (rb_state2.var_1));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
        rb_state3.var_1 = (rb_state3.var_1) ^ ((0xa779c827) - (rb_state3.var_1));
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_1) == (0xa779c827)) ? (rb_state3.var_2) : (0x2c493881));
        rb_state3.var_2 = (rb_state3.var_2) - (((rb_state3.var_1) == (0xa779c827)) ? (rb_state3.var_0) : (0x4d833369));
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
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (148261 < rb_input_size ? (uint32_t)rb_input[148261] : 0xbca53d9d);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0xb5be3191) - (0xc4ad6bde));
        rb_state8.var_0 = (rb_state8.var_0) + ((0x1b928079) ^ (rb_state8.var_0));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0x1f681bb5) - (0xbc7bf6b1));
        rb_state9.var_0 = (rb_state9.var_0) ^ (28179 < rb_input_size ? (uint32_t)rb_input[28179] : 0xd2181f75);
        rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_0) == (0x62ec2537)) ? (rb_state9.var_2) : (0x7797f1c8));
        if ((rb_state9.var_1) == (0xd1118b80)) {
            rb_state9.var_32 = 0xa99284fc;
            rb_state9.var_33 = (rb_state9.var_33) ^ (rb_state9.var_32);
            rb_state9.var_34 = (rb_state9.var_34) + (((0x3ab744ee) - (rb_state9.var_21)) ^ (0x81460b48));
            rb_state9.var_35 = (rb_state9.var_35) ^ (((0x8aa01997) + (rb_state9.var_20)) - (rb_state9.var_22));
            rb_state9.var_36 = (rb_state9.var_36) - (((0x9a42005e) + (rb_state9.var_23)) - (rb_state9.var_33));
            rb_state9.var_37 = (rb_state9.var_37) - (rb_state9.var_34);
            rb_state9.var_38 = (rb_state9.var_38) ^ (rb_state9.var_35);
            rb_state9.var_39 = (rb_state9.var_39) - (((rb_state9.var_36) == (0x0)) ? (rb_state9.var_37) : (0xbdb1583e));
            rb_state9.var_40 = (rb_state9.var_40) + (((0xc6ed4aa1) + (rb_state9.var_38)) ^ (rb_state9.var_24));
            rb_state9.var_41 = (rb_state9.var_41) ^ (((rb_state9.var_39) == (0x0)) ? (rb_state9.var_40) : (0x7ba6f22b));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_41);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (114655 < rb_input_size ? (uint32_t)rb_input[114655] : 0x863cf311);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (0x7dfb2866);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0xcc906f0d));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ ((0x914ed0dc) + (rb_state9.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0x9c25856a));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (2548 < rb_input_size ? (uint32_t)rb_input[2548] : 0x4781b83f);
    rb_state16.var_0 = (rb_state16.var_0) - (16153 < rb_input_size ? (uint32_t)rb_input[16153] : 0xbf117c65);
    if ((rb_state16.var_2) == (0x0)) {
        rb_state16.var_3 = rb_state16.var_0;
    }
    if ((rb_state16.var_2) == (0x0)) {
        if (!((rb_state16.var_0) == (rb_state16.var_3))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + ((0x541f6bd8) ^ (rb_state19.var_0));
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x329f8b73);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_2);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_15 = 0x3ff1eea3;
        rb_state3.var_16 = (rb_state3.var_16) + (107926 < rb_input_size ? (uint32_t)rb_input[107926] : 0x6cdf11b0);
        rb_state3.var_17 = (rb_state3.var_17) ^ (((0x81dd6b0d) + (rb_state3.var_12)) - (rb_state3.var_15));
        rb_state3.var_18 = (rb_state3.var_18) + (((((0xacb568da) - (rb_state3.var_10)) ^ (rb_state3.var_13)) - (rb_state3.var_11)) ^ (0x92aaf7d3));
        rb_state3.var_19 = (rb_state3.var_19) ^ (rb_state3.var_16);
        rb_state3.var_20 = (rb_state3.var_20) + (((((0xf59cab71) ^ (rb_state3.var_17)) - (rb_state3.var_18)) + (0x4af7235e)) - (0xa1476dde));
        rb_state3.var_21 = (rb_state3.var_21) + (((((0x649d7480) + (rb_state3.var_19)) ^ (rb_state3.var_14)) - (rb_state3.var_20)) ^ (0x3859d654));
        rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (64509 < rb_input_size ? (uint32_t)rb_input[64509] : 0xc6c2fa45);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0x46b0720d);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (125994 < rb_input_size ? (uint32_t)rb_input[125994] : 0x528a68fb);
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0x0)) {
        rb_state0.var_7 = 0xd16c3645;
        rb_state0.var_8 = 0xfa6fe308;
        rb_state0.var_9 = (rb_state0.var_9) - (rb_state0.var_6);
        rb_state0.var_10 = (rb_state0.var_10) - (rb_state0.var_5);
        rb_state0.var_11 = (rb_state0.var_11) + (((0x610b22aa) ^ (0x37cb0d25)) - (rb_state0.var_7));
        rb_state0.var_12 = (rb_state0.var_12) - (((0x695c78d9) - (rb_state0.var_8)) - (rb_state0.var_7));
        rb_state0.var_13 = (rb_state0.var_13) + (((0x435ce522) ^ (rb_state0.var_9)) ^ (rb_state0.var_8));
        rb_state0.var_14 = (rb_state0.var_14) - (((0xa34c5dd0) + (rb_state0.var_10)) ^ (0x9e7bcdd3));
        rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_11);
        rb_state0.var_16 = (rb_state0.var_16) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_12) : (0xe7384b10));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_13) : (0xd4d0ba32));
        rb_state0.var_18 = (rb_state0.var_18) - (rb_state0.var_14);
        rb_state0.var_19 = (rb_state0.var_19) + (((0xb3b74f88) ^ (rb_state0.var_15)) + (0x9fca1cf0));
        rb_state0.var_20 = (rb_state0.var_20) + (rb_state0.var_16);
        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_18) : (0x6bfeeebf));
        rb_state0.var_22 = (rb_state0.var_22) + (((0xdac9800b) - (rb_state0.var_11)) - (rb_state0.var_19));
        rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0x9c4f57a2));
        rb_state0.var_24 = (rb_state0.var_24) - (((0xb99638b8) + (rb_state0.var_22)) ^ (rb_state0.var_12));
        rb_state0.var_25 = (rb_state0.var_25) - (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_23) : (0xe45e4f64));
        rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_24) : (0xbd8e43e5));
        rb_state0.var_27 = (rb_state0.var_27) - (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_25) : (0xee18a619));
        rb_state0.var_28 = (rb_state0.var_28) ^ (((0x9ef123c3) + (rb_state0.var_16)) - (rb_state0.var_26));
        rb_state0.var_29 = (rb_state0.var_29) + (rb_state0.var_27);
        rb_state0.var_30 = (rb_state0.var_30) + (((rb_state0.var_28) == (0x0)) ? (rb_state0.var_29) : (0x4c4e506f));
        rb_state0.var_6 = (rb_state0.var_6) - (rb_state0.var_30);
        rb_state0.var_3 = rb_state0.var_6;
    }
    if ((rb_state0.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_40));
        rb_state0.var_32 = 0xc04e6ea6;
        rb_state0.var_33 = 0x562bc189;
        rb_state0.var_34 = (rb_state0.var_34) + (((((0x24a581da) ^ (rb_state0.var_33)) ^ (0x500f17ec)) ^ (rb_state0.var_18)) - (rb_state0.var_32));
        rb_state0.var_35 = (rb_state0.var_35) + (0xa6006e94);
        rb_state0.var_36 = (rb_state0.var_36) - (((((0x4d7d9ae7) ^ (rb_state0.var_17)) + (0x373d019a)) ^ (rb_state0.var_34)) - (0xe057161e));
        rb_state0.var_37 = (rb_state0.var_37) - (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_35) : (0x9ab1c428));
        rb_state0.var_38 = (rb_state0.var_38) - (rb_state0.var_36);
        rb_state0.var_39 = (rb_state0.var_39) - (((((0x7d033de8) - (rb_state0.var_37)) ^ (rb_state0.var_38)) - (0x15319252)) + (rb_state0.var_20));
        rb_state0.var_31 = (rb_state0.var_31) - (rb_state0.var_39);
        rb_state0.var_3 = rb_state0.var_31;
        pthread_mutex_unlock(&(rb_state0.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (0xbe1b42dc);
    if ((rb_state1.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_18));
        rb_state1.var_5 = 0x193a590e;
        rb_state1.var_6 = (rb_state1.var_6) - (rb_state1.var_3);
        rb_state1.var_7 = (rb_state1.var_7) - (((0xb0eda79) - (0xfbc57be6)) + (rb_state1.var_4));
        rb_state1.var_8 = (rb_state1.var_8) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_5) : (0xb4c00ecc));
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_6) : (0x282b43a1));
        rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_7) : (0xcc2db1e4));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((((0xbdbbcf49) + (rb_state1.var_8)) ^ (rb_state1.var_8)) - (rb_state1.var_9)) ^ (rb_state1.var_9));
        rb_state1.var_12 = (rb_state1.var_12) - (((0x74217f9a) ^ (rb_state1.var_10)) - (rb_state1.var_10));
        rb_state1.var_13 = (rb_state1.var_13) + (((((0x99884b28) ^ (rb_state1.var_11)) - (rb_state1.var_12)) ^ (0x7aeb25e6)) - (rb_state1.var_11));
        rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_13);
        rb_state1.var_3 = rb_state1.var_4;
        pthread_mutex_unlock(&(rb_state1.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_2) : (0x7dce3ba3));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (159754 < rb_input_size ? (uint32_t)rb_input[159754] : 0xf333750b);
    rb_state10.var_0 = (rb_state10.var_0) + (155764 < rb_input_size ? (uint32_t)rb_input[155764] : 0x9db92237);
    rb_state10.var_1 = (rb_state10.var_1) - ((0x3cbe954f) - (rb_state10.var_0));
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (156164 < rb_input_size ? (uint32_t)rb_input[156164] : 0xe9430b1b);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_1);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0xdd1b5a4c));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_0) == (0x860e869a)) ? (rb_state16.var_2) : (0xec2ae03d));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0xdc660064) - (rb_state17.var_0));
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (78715 < rb_input_size ? (uint32_t)rb_input[78715] : 0xdf6b9aa4);
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x70ad2958) - (rb_state1.var_0));
    if ((rb_state1.var_2) == (0x0)) {
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            rb_state1.var_28 = (rb_state1.var_28) + (((rb_state1.var_25) == (0x0)) ? (rb_state1.var_24) : (0x1d83844));
            rb_state1.var_29 = (rb_state1.var_29) - (((((0xa88f3be3) + (rb_state1.var_27)) - (rb_state1.var_22)) - (rb_state1.var_23)) - (rb_state1.var_26));
            rb_state1.var_30 = (rb_state1.var_30) + (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_28) : (0xfcb9d53c));
            rb_state1.var_31 = (rb_state1.var_31) + (((((0x928f37c) - (rb_state1.var_30)) + (rb_state1.var_21)) + (rb_state1.var_28)) + (0x8a590cfe));
            rb_state1.var_32 = (rb_state1.var_32) ^ (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_30) : (0x5b7e25a2));
            rb_state1.var_33 = (rb_state1.var_33) + (((((0x29a97f24) + (rb_state1.var_32)) + (rb_state1.var_31)) ^ (0xa50949e6)) - (0xd7f79ee2));
            rb_state1.var_27 = (rb_state1.var_27) + (rb_state1.var_33);
            rb_state1.var_19 = rb_state1.var_27;
        }
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            pthread_mutex_lock(&(rb_state1.lock_41));
            rb_state1.var_35 = 0xae30243a;
            rb_state1.var_36 = 0xecefce05;
            rb_state1.var_37 = (rb_state1.var_37) - (((rb_state1.var_35) == (0x0)) ? (rb_state1.var_36) : (0x7828e1b9));
            rb_state1.var_38 = (rb_state1.var_38) + (167176 < rb_input_size ? (uint32_t)rb_input[167176] : 0xb7796dcf);
            rb_state1.var_39 = (rb_state1.var_39) ^ (((((0xb63d3253) ^ (rb_state1.var_31)) ^ (rb_state1.var_37)) ^ (0x26563518)) + (0xa0858fa5));
            rb_state1.var_40 = (rb_state1.var_40) + (((((0x18df01c0) + (0x620dd72)) + (0x29a0b354)) + (rb_state1.var_38)) ^ (rb_state1.var_39));
            rb_state1.var_34 = (rb_state1.var_34) ^ (rb_state1.var_40);
            rb_state1.var_19 = rb_state1.var_34;
            pthread_mutex_unlock(&(rb_state1.lock_41));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x0)) {
        if ((rb_state2.var_3) != (0x0)) {
            if (!((rb_state2.var_3) == (rb_state2.var_22))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x5bc7fdb4)) {
        pthread_mutex_lock(&(rb_state4.lock_21));
        rb_state4.var_4 = (rb_state4.var_4) - (rb_state4.var_4);
        rb_state4.var_5 = (rb_state4.var_5) + (((((0xa39085b5) ^ (0x692d0a8e)) ^ (rb_state4.var_2)) ^ (rb_state4.var_3)) + (rb_state4.var_5));
        rb_state4.var_6 = (rb_state4.var_6) ^ (rb_state4.var_4);
        rb_state4.var_7 = (rb_state4.var_7) - (37730 < rb_input_size ? (uint32_t)rb_input[37730] : 0xc28ffc35);
        rb_state4.var_8 = (rb_state4.var_8) - (((((0x359c627f) + (rb_state4.var_5)) + (rb_state4.var_6)) - (rb_state4.var_7)) + (rb_state4.var_6));
        rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_7) : (0x522386c9));
        rb_state4.var_10 = (rb_state4.var_10) - (((((0x614a289d) ^ (rb_state4.var_9)) - (rb_state4.var_8)) ^ (rb_state4.var_9)) ^ (0x20aa2699));
        rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_10);
        rb_state4.var_2 = rb_state4.var_3;
        pthread_mutex_unlock(&(rb_state4.lock_21));
    }
    if ((rb_state4.var_1) == (0x5bc7fdb4)) {
        if (!((rb_state4.var_2) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
    if ((rb_state8.var_2) == (0xffffffc9)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_20))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ ((0x7279ad) - (0x4d4c53ca));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_2);
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - ((0x56b2d0b5) ^ (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_22));
        rb_state3.var_5 = (rb_state3.var_5) - (0xd08aa6e6);
        rb_state3.var_6 = (rb_state3.var_6) + (rb_state3.var_4);
        rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_5) == (0x2f75591a)) ? (rb_state3.var_3) : (0xeb08ffe2));
        rb_state3.var_8 = (rb_state3.var_8) - (((((0x95350653) + (rb_state3.var_7)) ^ (rb_state3.var_6)) ^ (rb_state3.var_6)) ^ (rb_state3.var_5));
        rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_7);
        rb_state3.var_10 = (rb_state3.var_10) - (rb_state3.var_8);
        rb_state3.var_11 = (rb_state3.var_11) ^ (rb_state3.var_9);
        rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_8) == (0x45bfa0b7)) ? (rb_state3.var_10) : (0x9b877c94));
        rb_state3.var_13 = (rb_state3.var_13) - (((0xdeea5fee) ^ (rb_state3.var_11)) ^ (rb_state3.var_9));
        rb_state3.var_14 = (rb_state3.var_14) - (((((0x8cb9473f) ^ (rb_state3.var_12)) + (rb_state3.var_13)) + (0x4dc92a2d)) ^ (0x9989af84));
        rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_14);
        rb_state3.var_3 = rb_state3.var_4;
        pthread_mutex_unlock(&(rb_state3.lock_22));
    }
    if ((rb_state3.var_0) == (0x0)) {
        if (!((rb_state3.var_3) == (rb_state3.var_4))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - ((0x67b5c5f4) - (0x8ec127de));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (171032 < rb_input_size ? (uint32_t)rb_input[171032] : 0x7de59bd7);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ ((0x4ec08a60) - (0x2ce76157));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x8682d676)) {
        pthread_mutex_lock(&(rb_state10.lock_34));
        rb_state10.var_5 = 0x550f045e;
        rb_state10.var_6 = (rb_state10.var_6) - (136625 < rb_input_size ? (uint32_t)rb_input[136625] : 0x9dcc91ad);
        rb_state10.var_7 = (rb_state10.var_7) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_4) : (0x929807f6));
        rb_state10.var_8 = (rb_state10.var_8) ^ (0x85b4f67e);
        rb_state10.var_9 = (rb_state10.var_9) ^ (rb_state10.var_5);
        rb_state10.var_10 = (rb_state10.var_10) + (((0xefdb6cd0) + (rb_state10.var_3)) - (0xc9b9bfe2));
        rb_state10.var_11 = (rb_state10.var_11) + (((0xd43f0481) ^ (rb_state10.var_6)) - (rb_state10.var_6));
        rb_state10.var_12 = (rb_state10.var_12) - (((((0xa44b73f3) ^ (rb_state10.var_8)) ^ (rb_state10.var_7)) + (0x48d77861)) - (0xc72981ca));
        rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_9);
        rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_10);
        rb_state10.var_15 = (rb_state10.var_15) + (rb_state10.var_11);
        rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_12) : (0x673dc2ad));
        rb_state10.var_17 = (rb_state10.var_17) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_13) : (0x5d7d0ba8));
        rb_state10.var_18 = (rb_state10.var_18) + (((((0xf6ba0314) + (rb_state10.var_15)) ^ (rb_state10.var_10)) + (rb_state10.var_9)) - (rb_state10.var_14));
        rb_state10.var_19 = (rb_state10.var_19) - (((((0xcd5a2b30) + (rb_state10.var_11)) - (rb_state10.var_16)) + (rb_state10.var_12)) - (rb_state10.var_17));
        rb_state10.var_20 = (rb_state10.var_20) ^ (rb_state10.var_18);
        rb_state10.var_21 = (rb_state10.var_21) - (rb_state10.var_19);
        rb_state10.var_22 = (rb_state10.var_22) ^ (((0x51dedaee) ^ (0x14a6ec51)) + (rb_state10.var_20));
        rb_state10.var_23 = (rb_state10.var_23) - (((((0x184dbeea) + (0xc3fbfa9)) ^ (rb_state10.var_13)) ^ (rb_state10.var_22)) + (rb_state10.var_21));
        rb_state10.var_4 = (rb_state10.var_4) + (rb_state10.var_23);
        rb_state10.var_3 = rb_state10.var_4;
        pthread_mutex_unlock(&(rb_state10.lock_34));
    }
    if ((rb_state10.var_1) == (0x8682d676)) {
        if (!((rb_state10.var_3) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (135076 < rb_input_size ? (uint32_t)rb_input[135076] : 0xb134cbd8);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
    rb_state4.var_1 = (rb_state4.var_1) + (0xbd69b67e);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0xffffffc9)) {
        rb_state8.var_9 = 0xdd6e0990;
        rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_5) : (0xfe951a3));
        rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_4);
        rb_state8.var_12 = (rb_state8.var_12) + (((0x92d1fc87) - (rb_state8.var_9)) + (0xeb1b7521));
        rb_state8.var_13 = (rb_state8.var_13) ^ (((0x3d8d05e4) - (rb_state8.var_10)) + (rb_state8.var_7));
        rb_state8.var_14 = (rb_state8.var_14) ^ (((((0xcc53d6bc) - (rb_state8.var_11)) ^ (0xd223be39)) ^ (rb_state8.var_12)) + (0xadcbf44b));
        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_13) : (0xf1cf6164));
        rb_state8.var_16 = (rb_state8.var_16) + (((0x2cb741cc) - (rb_state8.var_14)) - (0xc43ea390));
        rb_state8.var_17 = (rb_state8.var_17) + (rb_state8.var_15);
        rb_state8.var_18 = (rb_state8.var_18) - (((0x39b370af) ^ (0x35223cc6)) ^ (rb_state8.var_16));
        rb_state8.var_19 = (rb_state8.var_19) - (((rb_state8.var_17) == (0x3d8d05e4)) ? (rb_state8.var_18) : (0x288184d7));
        rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_19);
        rb_state8.var_3 = rb_state8.var_8;
    }
    if ((rb_state8.var_2) == (0xffffffc9)) {
        pthread_mutex_lock(&(rb_state8.lock_32));
        rb_state8.var_21 = 0x1a1dd19b;
        rb_state8.var_22 = 0x90279f1b;
        rb_state8.var_23 = 0x601ce61a;
        rb_state8.var_24 = (rb_state8.var_24) ^ (((((0xa26765f1) ^ (rb_state8.var_22)) + (rb_state8.var_23)) + (rb_state8.var_10)) + (rb_state8.var_9));
        rb_state8.var_25 = (rb_state8.var_25) ^ (rb_state8.var_21);
        rb_state8.var_26 = (rb_state8.var_26) - (rb_state8.var_24);
        rb_state8.var_27 = (rb_state8.var_27) - (((0x4f796e7) ^ (0xb4fc121f)) - (rb_state8.var_25));
        rb_state8.var_28 = (rb_state8.var_28) ^ (((0x97386224) ^ (0xa8d318b7)) - (rb_state8.var_26));
        rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_27) : (0x40f37a16));
        rb_state8.var_30 = (rb_state8.var_30) - (rb_state8.var_28);
        rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_29) == (0x6a124ca3)) ? (rb_state8.var_30) : (0x6a7be15a));
        rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_31);
        rb_state8.var_3 = rb_state8.var_20;
        pthread_mutex_unlock(&(rb_state8.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0xd1118b80)) {
        pthread_mutex_lock(&(rb_state9.lock_42));
        rb_state9.var_5 = 0xfea6e258;
        rb_state9.var_6 = (rb_state9.var_6) ^ (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_5) : (0x4bc8412e));
        rb_state9.var_7 = (rb_state9.var_7) + (((0x8dec0cee) - (rb_state9.var_3)) - (0x46f33c3d));
        rb_state9.var_8 = (rb_state9.var_8) ^ (42605 < rb_input_size ? (uint32_t)rb_input[42605] : 0xfb185231);
        rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_7) == (0x46f8d0b1)) ? (rb_state9.var_5) : (0x40794507));
        rb_state9.var_10 = (rb_state9.var_10) - (rb_state9.var_4);
        rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_6) == (0xfea6e258)) ? (rb_state9.var_7) : (0x4bb8268f));
        rb_state9.var_12 = (rb_state9.var_12) + (((0x6101ea77) - (rb_state9.var_8)) - (rb_state9.var_8));
        rb_state9.var_13 = (rb_state9.var_13) - (rb_state9.var_9);
        rb_state9.var_14 = (rb_state9.var_14) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xd2e3f07d));
        rb_state9.var_15 = (rb_state9.var_15) ^ (((0x1193f21d) + (0x3138d6d1)) + (rb_state9.var_12));
        rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_9) == (0xfea6e258)) ? (rb_state9.var_13) : (0x33594b08));
        rb_state9.var_17 = (rb_state9.var_17) - (rb_state9.var_14);
        rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_15) : (0x12d35c38));
        rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_16) == (0xfea6e258)) ? (rb_state9.var_17) : (0x2036479f));
        rb_state9.var_20 = (rb_state9.var_20) ^ (rb_state9.var_18);
        rb_state9.var_21 = (rb_state9.var_21) ^ (((0xac34b9d4) ^ (rb_state9.var_19)) + (rb_state9.var_11));
        rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_12) == (0x6101ea37)) ? (rb_state9.var_20) : (0x143698ec));
        rb_state9.var_23 = (rb_state9.var_23) ^ (((rb_state9.var_13) == (0x1591da8)) ? (rb_state9.var_21) : (0xff84ffa5));
        rb_state9.var_24 = (rb_state9.var_24) + (((rb_state9.var_14) == (0x46f8d0b1)) ? (rb_state9.var_22) : (0xa3d0475));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((0x401d1e18) + (rb_state9.var_15)) ^ (rb_state9.var_23));
        rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_16) == (0xfea6e258)) ? (rb_state9.var_24) : (0x2231ffab));
        rb_state9.var_27 = (rb_state9.var_27) ^ (rb_state9.var_25);
        rb_state9.var_28 = (rb_state9.var_28) + (((0xe388b1a0) - (rb_state9.var_17)) ^ (rb_state9.var_26));
        rb_state9.var_29 = (rb_state9.var_29) ^ (rb_state9.var_27);
        rb_state9.var_30 = (rb_state9.var_30) + (((rb_state9.var_18) == (0xa3ceb325)) ? (rb_state9.var_28) : (0xc4588a3e));
        rb_state9.var_31 = (rb_state9.var_31) + (((((0xae57ff13) ^ (0x1c5f23ae)) ^ (rb_state9.var_19)) ^ (rb_state9.var_30)) ^ (rb_state9.var_29));
        rb_state9.var_4 = (rb_state9.var_4) + (rb_state9.var_31);
        rb_state9.var_3 = rb_state9.var_4;
        pthread_mutex_unlock(&(rb_state9.lock_42));
    }
    if ((rb_state9.var_1) == (0xd1118b80)) {
        if (!((rb_state9.var_3) == (rb_state9.var_4))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x8682d676)) {
        rb_state10.var_24 = 0xa8e271ad;
        rb_state10.var_25 = 0x51d9a310;
        rb_state10.var_26 = (rb_state10.var_26) - ((0x5b58be30) + (rb_state10.var_15));
        rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_16) == (0x673dc2ad)) ? (rb_state10.var_25) : (0x2299e7a7));
        rb_state10.var_28 = (rb_state10.var_28) ^ (((rb_state10.var_17) == (0x5d7d0ba8)) ? (rb_state10.var_24) : (0x598a5d2a));
        rb_state10.var_29 = (rb_state10.var_29) ^ (((rb_state10.var_14) == (0x2621acee)) ? (rb_state10.var_26) : (0x42660e63));
        rb_state10.var_30 = (rb_state10.var_30) ^ (((rb_state10.var_18) == (0x3348a9eb)) ? (rb_state10.var_27) : (0xbe82660e));
        rb_state10.var_31 = (rb_state10.var_31) ^ (((rb_state10.var_28) == (0xa8e271ad)) ? (rb_state10.var_29) : (0xecb4580e));
        rb_state10.var_32 = (rb_state10.var_32) - (((rb_state10.var_19) == (0xb520b6)) ? (rb_state10.var_30) : (0x332b3ba3));
        rb_state10.var_33 = (rb_state10.var_33) - (((rb_state10.var_31) == (0x78e6464f)) ? (rb_state10.var_32) : (0x9c377955));
        rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_33);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + ((0xacaa37f9) - (0xb53dfb23));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (116841 < rb_input_size ? (uint32_t)rb_input[116841] : 0x65df3319);
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_21) == (0x0)) {
        rb_state15.var_25 = rb_state15.var_22;
    }
    if ((rb_state15.var_21) == (0x0)) {
        if (!((rb_state15.var_22) == (rb_state15.var_25))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - ((0xe0ede295) + (rb_state17.var_1));
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state6.lock_28));
            rb_state6.var_5 = 0x20b506fb;
            rb_state6.var_6 = 0x16456761;
            rb_state6.var_7 = (rb_state6.var_7) - (0x93cc04a1);
            rb_state6.var_8 = (rb_state6.var_8) - (((((0xa1f4607e) + (rb_state6.var_6)) ^ (rb_state6.var_3)) ^ (rb_state6.var_5)) ^ (rb_state6.var_4));
            rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_7) : (0x332937b3));
            rb_state6.var_10 = (rb_state6.var_10) + (((0x191f1dbf) + (rb_state6.var_8)) ^ (0x753c8bee));
            rb_state6.var_11 = (rb_state6.var_11) ^ (rb_state6.var_9);
            rb_state6.var_12 = (rb_state6.var_12) - (rb_state6.var_10);
            rb_state6.var_13 = (rb_state6.var_13) + (((((0x2e075db9) ^ (rb_state6.var_6)) + (rb_state6.var_11)) - (rb_state6.var_12)) ^ (rb_state6.var_7));
            rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_13);
            rb_state6.var_3 = rb_state6.var_4;
            pthread_mutex_unlock(&(rb_state6.lock_28));
        }
        if ((rb_state6.var_0) == (0x0)) {
            if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                racebench_trigger(6);
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
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_0) == (0x0)) {
                    pthread_mutex_lock(&(rb_state6.lock_28));
                    rb_state6.var_14 = 0xf86d92f3;
                    rb_state6.var_15 = 0x6fd9e9f;
                    rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_9) : (0x6a98fbcc));
                    rb_state6.var_17 = (rb_state6.var_17) + (((0x998b99d0) ^ (rb_state6.var_15)) ^ (0xaf6d23a1));
                    rb_state6.var_18 = (rb_state6.var_18) - (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_8) : (0x84b412));
                    rb_state6.var_19 = (rb_state6.var_19) - (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_16) : (0xdf0bfd96));
                    rb_state6.var_20 = (rb_state6.var_20) ^ (rb_state6.var_17);
                    rb_state6.var_21 = (rb_state6.var_21) ^ (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_18) : (0xafae27b5));
                    rb_state6.var_22 = (rb_state6.var_22) ^ (((0xa6dcab68) ^ (rb_state6.var_19)) - (rb_state6.var_13));
                    rb_state6.var_23 = (rb_state6.var_23) - (((0x1249bd78) ^ (rb_state6.var_20)) + (rb_state6.var_14));
                    rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_21) : (0x669fa5));
                    rb_state6.var_25 = (rb_state6.var_25) + (((((0x626dc5b9) - (0xaa93fd00)) + (rb_state6.var_16)) ^ (rb_state6.var_22)) - (rb_state6.var_23));
                    rb_state6.var_26 = (rb_state6.var_26) ^ (((0xd3a81a46) ^ (rb_state6.var_17)) ^ (rb_state6.var_24));
                    rb_state6.var_27 = (rb_state6.var_27) - (((((0x9df22e89) ^ (rb_state6.var_18)) + (rb_state6.var_26)) + (rb_state6.var_25)) - (0xd02d0736));
                    rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_27);
                    pthread_mutex_unlock(&(rb_state6.lock_28));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_2) == (0x0)) {
                    rb_state13.var_3 = rb_state13.var_0;
                }
                if ((rb_state13.var_2) == (0x0)) {
                    if (!((rb_state13.var_0) == (rb_state13.var_3))) {
                        racebench_trigger(13);
                    }
                }
                #endif
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