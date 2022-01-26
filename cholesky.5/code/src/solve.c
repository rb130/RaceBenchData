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
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_1) : (0xbda1e7ab));
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_2);
            rb_state7.var_0 = (rb_state7.var_0) + (0xc97ed314);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xd0b74a56));
            rb_state8.var_1 = (rb_state8.var_1) - ((0xcb901720) ^ (0x55107b6b));
            rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_2);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_0);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + ((0xa92d08e0) ^ (rb_state12.var_0));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (174086 < rb_input_size ? (uint32_t)rb_input[174086] : 0xe5c22b83);
            rb_state13.var_3 = (rb_state13.var_3) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_3) : (0xa9c4cf47));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (155548 < rb_input_size ? (uint32_t)rb_input[155548] : 0xb52c49a1);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_3) == (0xc2f0f540)) ? (rb_state13.var_2) : (0x739b92ad));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (0x1f2f646d);
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
        rb_state0.var_0 = (rb_state0.var_0) + ((0x8aad8ecc) + (rb_state0.var_0));
        rb_state0.var_1 = (rb_state0.var_1) ^ (43203 < rb_input_size ? (uint32_t)rb_input[43203] : 0xd99bc8bd);
        rb_state0.var_2 = (rb_state0.var_2) ^ ((0x56b19cce) - (0x49e9f87a));
        rb_state0.var_2 = (rb_state0.var_2) + (6797 < rb_input_size ? (uint32_t)rb_input[6797] : 0x9cc0a5df);
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_2);
        rb_state0.var_0 = (rb_state0.var_0) ^ (0x9d84bf5c);
        rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_0) == (0x17293190)) ? (rb_state0.var_2) : (0x11d2a928));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (0xff49cdd);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x9cfde64f));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + ((0xf87e3bc) - (rb_state3.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0x91a0a426) ^ (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ ((0x191965d0) + (rb_state7.var_0));
        rb_state7.var_1 = (rb_state7.var_1) ^ ((0x4f882099) ^ (rb_state7.var_2));
        rb_state7.var_0 = (rb_state7.var_0) + ((0x9a3a0957) + (0x29b5c8dc));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_1);
        rb_state8.var_4 = (rb_state8.var_4) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_3) : (0xf5bc2fac));
        rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_3) : (0xd4d505a6));
        rb_state8.var_1 = (rb_state8.var_1) - (115308 < rb_input_size ? (uint32_t)rb_input[115308] : 0x6c31af92);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0xf961bc5c));
        rb_state13.var_0 = (rb_state13.var_0) + ((0xee3863b) ^ (rb_state13.var_1));
        rb_state13.var_2 = (rb_state13.var_2) + (0x4f4b3841);
        rb_state13.var_3 = (rb_state13.var_3) - ((0x4d87b35) + (rb_state13.var_0));
        if ((rb_state13.var_4) == (0x7a29a732)) {
            rb_state13.var_5 = rb_state13.var_0;
        }
        if ((rb_state13.var_4) == (0x7a29a732)) {
            if (!((rb_state13.var_0) == (rb_state13.var_5))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + ((0x8351ff7a) - (0x9bc7aac5));
        rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0xe78a54b5)) ? (rb_state17.var_0) : (0xfdd67c33));
        rb_state17.var_1 = (rb_state17.var_1) ^ (136250 < rb_input_size ? (uint32_t)rb_input[136250] : 0x962837b);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (110116 < rb_input_size ? (uint32_t)rb_input[110116] : 0x5ecb580a);
        rb_state19.var_2 = (rb_state19.var_2) + ((0x969280d4) ^ (0xb89c7a36));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_0);
    rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_1) == (0xfd99e8a0)) ? (rb_state0.var_0) : (0x275158cf));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (68275 < rb_input_size ? (uint32_t)rb_input[68275] : 0xd90ad04b);
    rb_state2.var_1 = (rb_state2.var_1) - (3064 < rb_input_size ? (uint32_t)rb_input[3064] : 0xd5db27fd);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (57823 < rb_input_size ? (uint32_t)rb_input[57823] : 0x64f94b54);
    rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0xa041084a));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - ((0x871fc851) - (0x5d71ec37));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_2) == (0x9e967082)) ? (rb_state13.var_2) : (0xedfe3852));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (14115 < rb_input_size ? (uint32_t)rb_input[14115] : 0x74551d32);
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ (87090 < rb_input_size ? (uint32_t)rb_input[87090] : 0x60f74);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ ((0x371b9770) + (0xd41cdff9));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_0) == (0xa6880b17)) ? (rb_state7.var_1) : (0x4a802df5));
    rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_2);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + ((0xa2907199) ^ (0x61be4683));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_1);
    rb_state13.var_4 = (rb_state13.var_4) + ((0xa321f223) ^ (0x1e3521ba));
    if ((rb_state13.var_4) == (0x7a29a732)) {
        rb_state13.var_6 = (rb_state13.var_6) - ((0xf9610440) + (rb_state13.var_6));
        rb_state13.var_7 = (rb_state13.var_7) + (130259 < rb_input_size ? (uint32_t)rb_input[130259] : 0xa504bbe4);
        rb_state13.var_8 = (rb_state13.var_8) ^ (((((0x4619f4e8) - (rb_state13.var_4)) - (rb_state13.var_7)) ^ (rb_state13.var_5)) + (0xba32075f));
        rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0xd2f951e8));
        rb_state13.var_10 = (rb_state13.var_10) - (((0xaf529345) + (0xe63052b2)) ^ (rb_state13.var_8));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_10) : (0x708e4b66));
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - ((0x886d5af2) + (rb_state17.var_2));
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x0)) {
        rb_state0.var_5 = 0x62075e27;
        rb_state0.var_6 = 0x6614df60;
        rb_state0.var_7 = 0xafc8b34;
        rb_state0.var_8 = (rb_state0.var_8) - (((0x5f7fcca7) - (0xdb38121a)) + (rb_state0.var_5));
        rb_state0.var_9 = (rb_state0.var_9) ^ (rb_state0.var_6);
        rb_state0.var_10 = (rb_state0.var_10) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_8) : (0xb1d4b834));
        rb_state0.var_11 = (rb_state0.var_11) - (((0xd883c4ea) - (rb_state0.var_4)) - (rb_state0.var_9));
        rb_state0.var_12 = (rb_state0.var_12) - (((0xc734b56f) - (rb_state0.var_10)) + (0xf235765a));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((0xc7d6eb7) + (0x4c1271fa)) + (rb_state0.var_11));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_12);
        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_13);
        rb_state0.var_16 = (rb_state0.var_16) ^ (((0xa74d824b) ^ (rb_state0.var_14)) + (0xf4096200));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((((0x46a49ce1) ^ (rb_state0.var_16)) + (rb_state0.var_5)) - (0x1adf9e03)) ^ (rb_state0.var_15));
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (30795 < rb_input_size ? (uint32_t)rb_input[30795] : 0x3369ca64);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (0x2110a2d4);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (4184 < rb_input_size ? (uint32_t)rb_input[4184] : 0x90c04493);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (0x74b327e5);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x1d89f2b2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + ((0xacae68ac) + (rb_state19.var_1));
    rb_state19.var_2 = (rb_state19.var_2) - (101591 < rb_input_size ? (uint32_t)rb_input[101591] : 0x71b1a48);
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + ((0x246c65ce) - (0xe672d41c));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_4 = (rb_state7.var_4) - ((0x72dc284d) + (rb_state7.var_3));
    if ((rb_state7.var_4) == (0x1a47afcc)) {
        if ((rb_state7.var_5) != (0x0)) {
            if (!((rb_state7.var_5) == (rb_state7.var_14))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_4) == (0x4cf7f30d)) ? (rb_state8.var_3) : (0xc1cf840b));
    if ((rb_state8.var_0) == (0x40)) {
        rb_state8.var_6 = 0xbbbb20a1;
        rb_state8.var_7 = 0xeffd9046;
        rb_state8.var_8 = (rb_state8.var_8) + (143695 < rb_input_size ? (uint32_t)rb_input[143695] : 0x580e3ce5);
        rb_state8.var_9 = (rb_state8.var_9) ^ (rb_state8.var_6);
        rb_state8.var_10 = (rb_state8.var_10) - (((((0xa1997dce) + (0xe5627232)) + (rb_state8.var_6)) + (rb_state8.var_7)) + (0x1898d26e));
        rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_5);
        rb_state8.var_12 = (rb_state8.var_12) - (((0xb69ec204) - (rb_state8.var_8)) + (rb_state8.var_7));
        rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x7c2dbabc));
        rb_state8.var_14 = (rb_state8.var_14) + (((0x13a8f6ac) ^ (rb_state8.var_11)) + (0x84d5d2ae));
        rb_state8.var_15 = (rb_state8.var_15) + (((((0xd83f5131) - (rb_state8.var_12)) + (0x3b5c596b)) - (rb_state8.var_8)) - (rb_state8.var_13));
        rb_state8.var_16 = (rb_state8.var_16) - (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_14) : (0xf16af872));
        rb_state8.var_17 = (rb_state8.var_17) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_15) : (0x59beedf));
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_17) : (0x47590aa7));
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_4 = (rb_state19.var_4) + (rb_state19.var_3);
    rb_state19.var_4 = (rb_state19.var_4) ^ (34095 < rb_input_size ? (uint32_t)rb_input[34095] : 0x59ab9675);
    rb_state19.var_3 = (rb_state19.var_3) + (23345 < rb_input_size ? (uint32_t)rb_input[23345] : 0x19415183);
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_0) == (0xffffffc8)) ? (rb_state2.var_2) : (0xf51bd01));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (3332 < rb_input_size ? (uint32_t)rb_input[3332] : 0x6d1a9ca0);
    rb_state5.var_1 = (rb_state5.var_1) - (0x9840ef1d);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) - (11469 < rb_input_size ? (uint32_t)rb_input[11469] : 0x8596af1d);
    rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_3);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x40)) {
        rb_state8.var_5 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0x40)) {
        if (!((rb_state8.var_1) == (rb_state8.var_5))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_2) == (0x62)) ? (rb_state10.var_1) : (0x3471fd10));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (128622 < rb_input_size ? (uint32_t)rb_input[128622] : 0x287b4528);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - ((0x921a19b0) - (0x8dbea4c7));
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_4) == (0x66d46aa0)) {
        rb_state19.var_25 = 0xb7e36ce4;
        rb_state19.var_26 = (rb_state19.var_26) ^ (0x4a2f203);
        rb_state19.var_27 = (rb_state19.var_27) - (((((0x319b4c1a) + (rb_state19.var_20)) + (rb_state19.var_21)) + (rb_state19.var_18)) + (rb_state19.var_19));
        rb_state19.var_28 = (rb_state19.var_28) + (((((0xc25edb33) + (rb_state19.var_26)) - (rb_state19.var_22)) - (0xbb92d3d1)) + (rb_state19.var_25));
        rb_state19.var_29 = (rb_state19.var_29) - (((0x53651127) ^ (0xafb19787)) + (rb_state19.var_27));
        rb_state19.var_30 = (rb_state19.var_30) ^ (rb_state19.var_28);
        rb_state19.var_31 = (rb_state19.var_31) ^ (((((0xf5c5b3c5) - (rb_state19.var_30)) ^ (0x98f4e7f)) - (rb_state19.var_29)) - (0x82e16f4d));
        rb_state19.var_24 = (rb_state19.var_24) + (rb_state19.var_31);
        rb_state19.var_5 = rb_state19.var_24;
    }
    if ((rb_state19.var_4) == (0x66d46aa0)) {
        pthread_mutex_lock(&(rb_state19.lock_35));
        rb_state19.var_33 = (rb_state19.var_33) + (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_25) : (0x1f4d1046));
        rb_state19.var_34 = (rb_state19.var_34) + (((((0x33dc3281) + (0x8406cd29)) + (rb_state19.var_24)) ^ (rb_state19.var_33)) - (rb_state19.var_26));
        rb_state19.var_32 = (rb_state19.var_32) + (rb_state19.var_34);
        rb_state19.var_5 = rb_state19.var_32;
        pthread_mutex_unlock(&(rb_state19.lock_35));
    }
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (0x7d921a32);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (0x7fe59003);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_4) == (0x1a47afcc)) {
        rb_state7.var_10 = 0x656073ea;
        rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_5);
        rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x15db687c));
        rb_state7.var_13 = (rb_state7.var_13) + (((((0x16815708) + (0x66e0481f)) - (rb_state7.var_11)) - (0x280a6947)) - (rb_state7.var_12));
        rb_state7.var_9 = (rb_state7.var_9) + (rb_state7.var_13);
        rb_state7.var_5 = rb_state7.var_9;
    }
    if ((rb_state7.var_4) == (0x1a47afcc)) {
        pthread_mutex_lock(&(rb_state7.lock_21));
        rb_state7.var_15 = 0x9af35372;
        rb_state7.var_16 = (rb_state7.var_16) ^ (((((0xe9bf7c88) - (rb_state7.var_15)) - (rb_state7.var_12)) - (rb_state7.var_11)) + (rb_state7.var_13));
        rb_state7.var_17 = (rb_state7.var_17) ^ (rb_state7.var_14);
        rb_state7.var_18 = (rb_state7.var_18) ^ (((((0x7c75bd82) + (0xd978ada1)) + (rb_state7.var_15)) - (rb_state7.var_10)) ^ (rb_state7.var_16));
        rb_state7.var_19 = (rb_state7.var_19) + (rb_state7.var_17);
        rb_state7.var_20 = (rb_state7.var_20) ^ (((rb_state7.var_18) == (0xe5650c61)) ? (rb_state7.var_19) : (0x2542ceb4));
        rb_state7.var_14 = (rb_state7.var_14) - (rb_state7.var_20);
        rb_state7.var_5 = rb_state7.var_14;
        pthread_mutex_unlock(&(rb_state7.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (0xb08b3666);
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_14));
        rb_state1.var_5 = rb_state1.var_1;
        pthread_mutex_unlock(&(rb_state1.lock_14));
    }
    if ((rb_state1.var_0) == (0x0)) {
        if (!((rb_state1.var_1) == (rb_state1.var_5))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) - (0x92ce2a3b);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_4 = (rb_state12.var_4) + ((0x5f6ecef8) + (rb_state12.var_4));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_17));
        rb_state14.var_7 = 0x57fef8d2;
        rb_state14.var_8 = 0xb69c2ca8;
        rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_8) : (0xe2626088));
        rb_state14.var_10 = (rb_state14.var_10) - (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_5) : (0x1c6cf8b3));
        rb_state14.var_11 = (rb_state14.var_11) + (((((0xb9c59e7f) + (rb_state14.var_7)) - (0x874495e)) ^ (rb_state14.var_9)) + (rb_state14.var_10));
        rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_11);
        rb_state14.var_5 = rb_state14.var_6;
        pthread_mutex_unlock(&(rb_state14.lock_17));
    }
    if ((rb_state14.var_2) == (0x0)) {
        if (!((rb_state14.var_5) == (rb_state14.var_6))) {
            racebench_trigger(14);
        }
    }
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_4
        rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_23);
        if ((rb_state4.var_25) == (0x0)) {
            rb_state4.var_26 = rb_state4.var_9;
        }
        if ((rb_state4.var_25) == (0x0)) {
            pthread_mutex_lock(&(rb_state4.lock_35));
            if (!((rb_state4.var_9) == (rb_state4.var_26))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_35));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_4) == (0x0)) {
            if ((rb_state15.var_16) != (0x0)) {
                if (!((rb_state15.var_16) == (rb_state15.var_25))) {
                    racebench_trigger(15);
                }
            }
        }
        #endif
        Error = pthread_mutex_lock(&(Global->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_4) == (0x0)) {
            rb_state15.var_21 = 0xfb802a72;
            rb_state15.var_22 = 0x815abd07;
            rb_state15.var_23 = (rb_state15.var_23) + (((((0x1dd7f899) ^ (rb_state15.var_22)) - (0xbecc24c9)) + (rb_state15.var_18)) - (rb_state15.var_17));
            rb_state15.var_24 = (rb_state15.var_24) - (((((0x44863c95) - (rb_state15.var_21)) ^ (0xaf1bdf94)) - (rb_state15.var_23)) - (0xc9f8ada4));
            rb_state15.var_20 = (rb_state15.var_20) ^ (rb_state15.var_24);
            rb_state15.var_16 = rb_state15.var_20;
        }
        if ((rb_state15.var_4) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_36));
            rb_state15.var_26 = 0x21f381c5;
            rb_state15.var_27 = (rb_state15.var_27) ^ (((((0xb1d4e00b) - (rb_state15.var_22)) ^ (rb_state15.var_19)) + (rb_state15.var_26)) ^ (rb_state15.var_21));
            rb_state15.var_28 = (rb_state15.var_28) ^ (((0x34ca54e8) + (rb_state15.var_20)) ^ (rb_state15.var_23));
            rb_state15.var_29 = (rb_state15.var_29) + ((0x3be8ec9f) ^ (0x691cdf13));
            rb_state15.var_30 = (rb_state15.var_30) + (((rb_state15.var_27) == (0x0)) ? (rb_state15.var_28) : (0xe1f1f8c));
            rb_state15.var_31 = (rb_state15.var_31) ^ (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_29) : (0x43008e80));
            rb_state15.var_32 = (rb_state15.var_32) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_30) : (0x9e66bd96));
            rb_state15.var_33 = (rb_state15.var_33) - (rb_state15.var_31);
            rb_state15.var_34 = (rb_state15.var_34) + (((0xe5ee8e5b) + (0x32e66881)) + (rb_state15.var_32));
            rb_state15.var_35 = (rb_state15.var_35) ^ (((((0xb70af77) ^ (rb_state15.var_33)) - (0x8970f6f4)) + (rb_state15.var_26)) ^ (rb_state15.var_34));
            rb_state15.var_25 = (rb_state15.var_25) + (rb_state15.var_35);
            rb_state15.var_16 = rb_state15.var_25;
            pthread_mutex_unlock(&(rb_state15.lock_36));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_2) == (0x400)) {
            rb_state18.var_5 = 0xe56f6d30;
            rb_state18.var_6 = 0x2a868e5f;
            rb_state18.var_7 = (rb_state18.var_7) + (rb_state18.var_5);
            rb_state18.var_8 = (rb_state18.var_8) - (((((0xc5076f32) - (rb_state18.var_4)) + (rb_state18.var_6)) ^ (0x461eda98)) + (0x22ff6e32));
            rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_5) == (0xe56f6d30)) ? (rb_state18.var_7) : (0x4d97d99d));
            rb_state18.var_10 = (rb_state18.var_10) - (((0x81b02895) ^ (0xb4a66f3d)) ^ (rb_state18.var_8));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((0x7a05cc63) ^ (0xd0249f86)) ^ (rb_state18.var_9));
            rb_state18.var_12 = (rb_state18.var_12) - (((0xbb55f1c) + (0xfc708453)) + (rb_state18.var_10));
            rb_state18.var_13 = (rb_state18.var_13) - (((0x346c9b8c) + (rb_state18.var_11)) - (rb_state18.var_6));
            rb_state18.var_14 = (rb_state18.var_14) ^ (rb_state18.var_12);
            rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_13);
            rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_7) == (0xe56f6d30)) ? (rb_state18.var_14) : (0x5502cc81));
            rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_15) == (0x4568319e)) ? (rb_state18.var_16) : (0xe1e2ea36));
            rb_state18.var_3 = (rb_state18.var_3) ^ (rb_state18.var_17);
        }
        #endif
        Cycle = (Global->start).cycle;
        if (++(Global->start).counter != (P)) {
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_25) == (0x0)) {
                rb_state4.var_27 = 0xa340b539;
                rb_state4.var_28 = 0x52987cf1;
                rb_state4.var_29 = (rb_state4.var_29) - (((((0xbe00cc77) + (rb_state4.var_28)) ^ (rb_state4.var_27)) ^ (rb_state4.var_28)) ^ (rb_state4.var_27));
                rb_state4.var_30 = (rb_state4.var_30) - ((0x36283b0a) - (rb_state4.var_29));
                rb_state4.var_31 = (rb_state4.var_31) + (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_26) : (0x6137b4f2));
                rb_state4.var_32 = (rb_state4.var_32) - (((((0xcef827c2) ^ (0x14414b01)) ^ (0x9627e33)) + (rb_state4.var_29)) ^ (rb_state4.var_30));
                rb_state4.var_33 = (rb_state4.var_33) ^ (rb_state4.var_31);
                rb_state4.var_34 = (rb_state4.var_34) - (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_33) : (0x37532740));
                rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_34);
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_1) == (0x59488f98)) {
                pthread_mutex_lock(&(rb_state9.lock_20));
                rb_state9.var_7 = 0x6d37b6cf;
                rb_state9.var_8 = 0x76ec4ef6;
                rb_state9.var_9 = 0x56971dc1;
                rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_7) : (0x14cd6040));
                rb_state9.var_11 = (rb_state9.var_11) + (rb_state9.var_8);
                rb_state9.var_12 = (rb_state9.var_12) ^ (rb_state9.var_9);
                rb_state9.var_13 = (rb_state9.var_13) - (rb_state9.var_10);
                rb_state9.var_14 = (rb_state9.var_14) + (((0xe7953b38) - (rb_state9.var_11)) ^ (0x9ddb75a4));
                rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_12) : (0xe4784355));
                rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_13) == (0x92c84931)) ? (rb_state9.var_14) : (0xfb7576c5));
                rb_state9.var_17 = (rb_state9.var_17) - (rb_state9.var_15);
                rb_state9.var_18 = (rb_state9.var_18) ^ (((0x75804621) ^ (rb_state9.var_7)) - (rb_state9.var_16));
                rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_17) == (0x56971dc1)) ? (rb_state9.var_18) : (0x99f2361d));
                rb_state9.var_5 = (rb_state9.var_5) + (rb_state9.var_19);
                rb_state9.var_6 = rb_state9.var_5;
                pthread_mutex_unlock(&(rb_state9.lock_20));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_3);
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
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_16) == (0xed7399e6)) {
            rb_state9.var_22 = rb_state9.var_17;
        }
        if ((rb_state9.var_16) == (0xed7399e6)) {
            pthread_mutex_lock(&(rb_state9.lock_33));
            if (!((rb_state9.var_17) == (rb_state9.var_22))) {
                racebench_trigger(9);
            }
            pthread_mutex_unlock(&(rb_state9.lock_33));
        }
        #endif
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