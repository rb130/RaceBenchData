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
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xa5c84415));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (0x34f7f4bb);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) - ((0xe7a6cfd4) - (0x5db4ab11));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (60906 < rb_input_size ? (uint32_t)rb_input[60906] : 0xf5a49bc8);
            rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0xdfbde1e));
            rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_2);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0x4e2d8d88)) ? (rb_state9.var_0) : (0x81b11918));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x74cecacc));
            rb_state12.var_2 = (rb_state12.var_2) + ((0xbaaa008e) ^ (0x2b17534b));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x73a3888b));
            rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_2);
            rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_1);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
            rb_state15.var_2 = (rb_state15.var_2) ^ (((rb_state15.var_1) == (0x35)) ? (rb_state15.var_0) : (0xb52cf723));
            rb_state15.var_2 = (rb_state15.var_2) ^ (57747 < rb_input_size ? (uint32_t)rb_input[57747] : 0x77723c26);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Go), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_4 = (rb_state3.var_4) - (rb_state3.var_0);
        if ((rb_state3.var_1) == (0xa5d78cf8)) {
            rb_state3.var_15 = 0x3b4fd21d;
            rb_state3.var_16 = (rb_state3.var_16) + (((rb_state3.var_10) == (0xb4bc1224)) ? (rb_state3.var_11) : (0x11c0eaf5));
            rb_state3.var_17 = (rb_state3.var_17) - (((rb_state3.var_12) == (0xe0719962)) ? (rb_state3.var_15) : (0x7be62c1f));
            rb_state3.var_18 = (rb_state3.var_18) + (((rb_state3.var_13) == (0x2b4e6cdf)) ? (rb_state3.var_16) : (0xd82e5028));
            rb_state3.var_19 = (rb_state3.var_19) + (((0xa931dcdd) ^ (rb_state3.var_17)) - (0x541f7731));
            rb_state3.var_20 = (rb_state3.var_20) + (rb_state3.var_18);
            rb_state3.var_21 = (rb_state3.var_21) + (((0xa33a5d22) + (0xdcc2977b)) + (rb_state3.var_19));
            rb_state3.var_22 = (rb_state3.var_22) - (rb_state3.var_20);
            rb_state3.var_23 = (rb_state3.var_23) - (rb_state3.var_21);
            rb_state3.var_24 = (rb_state3.var_24) - (((0x378f0815) + (rb_state3.var_22)) + (rb_state3.var_14));
            rb_state3.var_25 = (rb_state3.var_25) ^ (((0x86752723) - (rb_state3.var_23)) + (rb_state3.var_15));
            rb_state3.var_26 = (rb_state3.var_26) - (rb_state3.var_24);
            rb_state3.var_27 = (rb_state3.var_27) + (rb_state3.var_25);
            rb_state3.var_28 = (rb_state3.var_28) - (((((0x9dba3dd5) + (0xf9077d76)) + (0xb6c346d7)) - (rb_state3.var_27)) + (rb_state3.var_26));
            rb_state3.var_5 = (rb_state3.var_5) ^ (rb_state3.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_2) == (0x9a4d10b5)) {
            rb_state5.var_4 = rb_state5.var_3;
        }
        if ((rb_state5.var_2) == (0x9a4d10b5)) {
            if (!((rb_state5.var_3) == (rb_state5.var_4))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x1d985764)) {
            pthread_mutex_lock(&(rb_state7.lock_18));
            rb_state7.var_5 = 0xcdd697e3;
            rb_state7.var_6 = (rb_state7.var_6) ^ (((rb_state7.var_5) == (0xcdd697e3)) ? (rb_state7.var_4) : (0x2ab62f6f));
            rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_5) == (0xcdd697e3)) ? (rb_state7.var_3) : (0xc95ab35e));
            rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_7) : (0x169d4921));
            rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_8);
            rb_state7.var_3 = rb_state7.var_4;
            pthread_mutex_unlock(&(rb_state7.lock_18));
        }
        if ((rb_state7.var_0) == (0x1d985764)) {
            if (!((rb_state7.var_3) == (rb_state7.var_4))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_1);
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
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (0x9024fed7);
        rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_1) == (0x6fdb0129)) ? (rb_state2.var_0) : (0xeebd5f47));
        rb_state2.var_2 = (rb_state2.var_2) - (50011 < rb_input_size ? (uint32_t)rb_input[50011] : 0x848ea43);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0xe8a684b);
        rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_3) : (0xc27a2290));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (0x57440761);
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_3);
        rb_state5.var_1 = (rb_state5.var_1) + ((0xd4554761) - (0x9ece4b12));
        if ((rb_state5.var_2) == (0x9a4d10b5)) {
            rb_state5.var_5 = 0xfd7c4da;
            rb_state5.var_6 = (rb_state5.var_6) - (((((0xf9dd4ca9) ^ (rb_state5.var_5)) - (rb_state5.var_4)) - (0xb6a55642)) ^ (0x2ed5220c));
            rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (0x97703e78);
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
        rb_state7.var_1 = (rb_state7.var_1) - (169786 < rb_input_size ? (uint32_t)rb_input[169786] : 0x8464c673);
        rb_state7.var_2 = (rb_state7.var_2) ^ (0xa42871df);
        rb_state7.var_2 = (rb_state7.var_2) + (120563 < rb_input_size ? (uint32_t)rb_input[120563] : 0x389968ef);
        rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_2) == (0xa42871ff)) ? (rb_state7.var_1) : (0xa46e84ec));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (0xd8e9393c);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + ((0x1822a6be) ^ (0x361a446e));
        rb_state12.var_0 = (rb_state12.var_0) - ((0x6d19f6ca) ^ (rb_state12.var_1));
        if ((rb_state12.var_3) == (0x20)) {
            pthread_mutex_lock(&(rb_state12.lock_14));
            rb_state12.var_5 = 0xa4682111;
            rb_state12.var_6 = 0xadd75272;
            rb_state12.var_7 = (rb_state12.var_7) - (((((0x3b7705e5) + (rb_state12.var_6)) ^ (rb_state12.var_6)) + (rb_state12.var_5)) - (rb_state12.var_5));
            rb_state12.var_8 = (rb_state12.var_8) - ((0xf5cddb07) - (0x803bd2e7));
            rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_7) : (0xcc74303d));
            rb_state12.var_10 = (rb_state12.var_10) + (rb_state12.var_8);
            rb_state12.var_11 = (rb_state12.var_11) - (rb_state12.var_9);
            rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_10) : (0xb42e727f));
            rb_state12.var_13 = (rb_state12.var_13) ^ (((((0xf2992135) - (rb_state12.var_8)) + (rb_state12.var_12)) + (0xedb0d55a)) - (rb_state12.var_11));
            rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_13);
            pthread_mutex_unlock(&(rb_state12.lock_14));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (115152 < rb_input_size ? (uint32_t)rb_input[115152] : 0x727dc86a);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (8634 < rb_input_size ? (uint32_t)rb_input[8634] : 0x43a14b42);
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (143843 < rb_input_size ? (uint32_t)rb_input[143843] : 0x1876ac46);
        rb_state15.var_3 = (rb_state15.var_3) - ((0x2b0625ea) ^ (0x9e94a0ce));
        if ((rb_state15.var_0) == (0x0)) {
            rb_state15.var_27 = 0x6d5289f7;
            rb_state15.var_28 = 0xdff46409;
            rb_state15.var_29 = (rb_state15.var_29) + (((((0x6978b6f5) + (0xfa80f7b1)) + (rb_state15.var_28)) + (rb_state15.var_27)) + (rb_state15.var_12));
            rb_state15.var_30 = (rb_state15.var_30) + (((0x2c6f6090) + (rb_state15.var_11)) ^ (0xe086069f));
            rb_state15.var_31 = (rb_state15.var_31) + (((0x368c1d0f) - (rb_state15.var_13)) + (rb_state15.var_29));
            rb_state15.var_32 = (rb_state15.var_32) - (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_30) : (0x85772067));
            rb_state15.var_33 = (rb_state15.var_33) ^ (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_31) : (0x8c31ccfe));
            rb_state15.var_34 = (rb_state15.var_34) - (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_32) : (0xb5ae1a98));
            rb_state15.var_35 = (rb_state15.var_35) ^ (((0x2347672b) - (rb_state15.var_17)) - (rb_state15.var_33));
            rb_state15.var_36 = (rb_state15.var_36) - (((((0x67fcbdd9) - (rb_state15.var_34)) - (0x7c86b11b)) + (rb_state15.var_35)) ^ (rb_state15.var_18));
            rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_36);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0xfe2fea87));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (0xe3c7b242);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_1) : (0x32de3000));
        rb_state18.var_2 = (rb_state18.var_2) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_2) : (0x5d273922));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x393aa272));
        #endif
        pthread_mutex_lock(&(Global->waitLock));
    }
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_0);
    rb_state3.var_2 = (rb_state3.var_2) ^ (0x5de3f073);
    rb_state3.var_1 = (rb_state3.var_1) + ((0x8f0334e) - (0x548e3e0b));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - ((0x65b2ef4b) - (rb_state5.var_2));
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0xa8bbf881)) ? (rb_state5.var_2) : (0x23e4c30d));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (28418 < rb_input_size ? (uint32_t)rb_input[28418] : 0x8ea47202);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xbede0f68);
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x88611de9));
    rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_2);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (15361 < rb_input_size ? (uint32_t)rb_input[15361] : 0x34888343);
    rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_2) == (0xffffffce)) ? (rb_state9.var_1) : (0xb3e6b81e));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0xfb837ce2);
    rb_state12.var_2 = (rb_state12.var_2) + (0xd2222a0d);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (79762 < rb_input_size ? (uint32_t)rb_input[79762] : 0x210fd7f3);
    rb_state13.var_2 = (rb_state13.var_2) + ((0x47b29b34) ^ (0x26128ce6));
    rb_state13.var_3 = (rb_state13.var_3) + (142606 < rb_input_size ? (uint32_t)rb_input[142606] : 0x460a9a2d);
    rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_3);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) ^ ((0xdd2ac1a4) + (rb_state14.var_3));
    rb_state14.var_2 = (rb_state14.var_2) - ((0xe27d6b8f) ^ (rb_state14.var_3));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (118973 < rb_input_size ? (uint32_t)rb_input[118973] : 0xe0a8bf72);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0xedf324eb));
    #endif
    MyNum = gp->pid;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) ^ ((0x7898792a) + (0xf215bff9));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (0xc153ae86);
    rb_state3.var_0 = (rb_state3.var_0) + (80455 < rb_input_size ? (uint32_t)rb_input[80455] : 0xcfb592e);
    rb_state3.var_4 = (rb_state3.var_4) ^ (((rb_state3.var_2) == (0x1f379ef9)) ? (rb_state3.var_4) : (0xe2a1977a));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (0xa5e69006);
    rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_3);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) + (((rb_state9.var_2) == (0xffffffce)) ? (rb_state9.var_1) : (0x185c4ba4));
    if ((rb_state9.var_1) == (0x8dfc80e0)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        rb_state9.var_6 = 0xe66c4eeb;
        rb_state9.var_7 = 0x9db31656;
        rb_state9.var_8 = (rb_state9.var_8) ^ (rb_state9.var_6);
        rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_7) : (0xfd90fec1));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_8) : (0x7962f99b));
        rb_state9.var_11 = (rb_state9.var_11) + (((0xe5311303) ^ (rb_state9.var_9)) + (0xa1b57e95));
        rb_state9.var_12 = (rb_state9.var_12) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0x612269d4));
        rb_state9.var_5 = (rb_state9.var_5) ^ (rb_state9.var_12);
        rb_state9.var_4 = rb_state9.var_5;
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    if ((rb_state9.var_1) == (0x8dfc80e0)) {
        if (!((rb_state9.var_4) == (rb_state9.var_5))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + ((0xd0a080db) + (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (0x9d4fba18);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_3) == (0x36)) ? (rb_state13.var_2) : (0x771e5dac));
    rb_state13.var_0 = (rb_state13.var_0) + (37172 < rb_input_size ? (uint32_t)rb_input[37172] : 0x5900f448);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x62beb6de)) {
        rb_state14.var_4 = rb_state14.var_3;
    }
    if ((rb_state14.var_2) == (0x62beb6de)) {
        if (!((rb_state14.var_3) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - (168105 < rb_input_size ? (uint32_t)rb_input[168105] : 0x3fece988);
    if ((rb_state15.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_37));
        rb_state15.var_6 = 0x1f040de2;
        rb_state15.var_7 = 0xb304de23;
        rb_state15.var_8 = (rb_state15.var_8) ^ (67137 < rb_input_size ? (uint32_t)rb_input[67137] : 0x3d94f866);
        rb_state15.var_9 = (rb_state15.var_9) ^ (((0xa3390e67) ^ (0xd82007e9)) - (rb_state15.var_4));
        rb_state15.var_10 = (rb_state15.var_10) - (rb_state15.var_7);
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_6) : (0x9c2e3c69));
        rb_state15.var_12 = (rb_state15.var_12) - (((0xc2f1ec7f) + (rb_state15.var_8)) - (0xe7c00b75));
        rb_state15.var_13 = (rb_state15.var_13) + (((0x5dc958b1) + (rb_state15.var_9)) ^ (0xc795fbcd));
        rb_state15.var_14 = (rb_state15.var_14) ^ (((((0xe0e2949d) ^ (rb_state15.var_11)) ^ (0x520ad052)) + (0x6188b10b)) + (rb_state15.var_10));
        rb_state15.var_15 = (rb_state15.var_15) ^ (rb_state15.var_12);
        rb_state15.var_16 = (rb_state15.var_16) + (rb_state15.var_13);
        rb_state15.var_17 = (rb_state15.var_17) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_14) : (0x388dcc77));
        rb_state15.var_18 = (rb_state15.var_18) ^ (((0x617af5b1) ^ (rb_state15.var_15)) ^ (rb_state15.var_7));
        rb_state15.var_19 = (rb_state15.var_19) - (((((0x967b4657) + (rb_state15.var_16)) + (0x7549724b)) ^ (rb_state15.var_17)) + (rb_state15.var_8));
        rb_state15.var_20 = (rb_state15.var_20) ^ (((0x82e559a1) ^ (rb_state15.var_18)) + (0xd7141499));
        rb_state15.var_21 = (rb_state15.var_21) - (((0x49789cf7) ^ (rb_state15.var_19)) ^ (0x8cf138a0));
        rb_state15.var_22 = (rb_state15.var_22) - (rb_state15.var_20);
        rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_21) : (0xe6c1d09b));
        rb_state15.var_24 = (rb_state15.var_24) + (((0xbeace3d0) ^ (rb_state15.var_22)) + (rb_state15.var_10));
        rb_state15.var_25 = (rb_state15.var_25) - (rb_state15.var_23);
        rb_state15.var_26 = (rb_state15.var_26) ^ (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_25) : (0x9b5d7b75));
        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_26);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_37));
    }
    if ((rb_state15.var_0) == (0x0)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (0xea4a04f9);
    #endif
    gp->pid++;
    { pthread_mutex_unlock(&(Global->waitLock)); }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - ((0xf0c4722a) + (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0xa5d78cf8)) {
        pthread_mutex_lock(&(rb_state3.lock_29));
        rb_state3.var_7 = 0xb4bc1224;
        rb_state3.var_8 = 0xcfecc6b5;
        rb_state3.var_9 = (rb_state3.var_9) - ((0x4e0c28d9) - (rb_state3.var_6));
        rb_state3.var_10 = (rb_state3.var_10) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_7) : (0x725c92a));
        rb_state3.var_11 = (rb_state3.var_11) - (((0x5e34ef18) + (rb_state3.var_8)) ^ (0xfa9026ec));
        rb_state3.var_12 = (rb_state3.var_12) ^ (((0x6c1626d) - (rb_state3.var_9)) ^ (rb_state3.var_7));
        rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_10) == (0xb4bc1224)) ? (rb_state3.var_11) : (0xc355e28a));
        rb_state3.var_14 = (rb_state3.var_14) ^ (((((0x624ebc9) ^ (rb_state3.var_12)) - (rb_state3.var_9)) - (rb_state3.var_8)) + (rb_state3.var_13));
        rb_state3.var_6 = (rb_state3.var_6) + (rb_state3.var_14);
        rb_state3.var_5 = rb_state3.var_6;
        pthread_mutex_unlock(&(rb_state3.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0xc7e6d16a) - (rb_state7.var_1));
    rb_state7.var_2 = (rb_state7.var_2) - ((0xff52ba74) ^ (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x2716c6c4)) ? (rb_state9.var_0) : (0xa7c5574b));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0x6578531b) ^ (0x3ab3a3fa));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0xd8be7638)) {
        rb_state13.var_12 = 0xb6580a20;
        rb_state13.var_13 = 0x5aa19cb4;
        rb_state13.var_14 = 0x70fb24d1;
        rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_9);
        rb_state13.var_16 = (rb_state13.var_16) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_14) : (0x38913857));
        rb_state13.var_17 = (rb_state13.var_17) - (((((0x8a4766f3) + (rb_state13.var_15)) + (rb_state13.var_13)) - (0xc37440d0)) - (rb_state13.var_10));
        rb_state13.var_18 = (rb_state13.var_18) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_16) : (0x8b479f3d));
        rb_state13.var_19 = (rb_state13.var_19) + (rb_state13.var_17);
        rb_state13.var_20 = (rb_state13.var_20) ^ (((0xbe7759b4) + (rb_state13.var_12)) - (rb_state13.var_18));
        rb_state13.var_21 = (rb_state13.var_21) + (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_20) : (0x2456cb2c));
        rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (0x31d26111);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - ((0xe4c226ea) + (rb_state16.var_1));
    rb_state16.var_1 = (rb_state16.var_1) ^ (0xf77a915d);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - ((0xa2104d1c) + (rb_state18.var_3));
    rb_state18.var_3 = (rb_state18.var_3) - (0xfc8f9460);
    rb_state18.var_4 = (rb_state18.var_4) ^ ((0xebf444b0) - (rb_state18.var_1));
    #endif
    lc = (struct LocalCopies *)malloc(sizeof(struct LocalCopies) + 2 * PAGE_SIZE);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (126391 < rb_input_size ? (uint32_t)rb_input[126391] : 0x4e5620d);
    rb_state2.var_1 = (rb_state2.var_1) - (0x1c1ebca5);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0xa5d78cf8)) {
        pthread_mutex_lock(&(rb_state3.lock_29));
        if (!((rb_state3.var_5) == (rb_state3.var_6))) {
            racebench_trigger(3);
        }
        pthread_mutex_unlock(&(rb_state3.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + ((0xfd2f0e09) ^ (0x7891ccdf));
    if ((rb_state7.var_0) == (0x1d985764)) {
        pthread_mutex_lock(&(rb_state7.lock_18));
        rb_state7.var_9 = 0xff38eb93;
        rb_state7.var_10 = 0xb3546fac;
        rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_7) : (0x27fcd7e4));
        rb_state7.var_12 = (rb_state7.var_12) ^ (((((0x78390b59) - (rb_state7.var_9)) + (0x8df1dc8)) + (rb_state7.var_6)) ^ (0x72ef349a));
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_9) == (0xff38eb93)) ? (rb_state7.var_10) : (0x95c51262));
        rb_state7.var_14 = (rb_state7.var_14) - (((0x406c66ff) + (0x49efe6c)) ^ (rb_state7.var_11));
        rb_state7.var_15 = (rb_state7.var_15) ^ (((((0xfcefc83c) + (rb_state7.var_10)) ^ (rb_state7.var_13)) - (rb_state7.var_11)) + (rb_state7.var_12));
        rb_state7.var_16 = (rb_state7.var_16) - (rb_state7.var_14);
        rb_state7.var_17 = (rb_state7.var_17) + (((rb_state7.var_15) == (0xf6406158)) ? (rb_state7.var_16) : (0x1183521f));
        rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_17);
        pthread_mutex_unlock(&(rb_state7.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - (153997 < rb_input_size ? (uint32_t)rb_input[153997] : 0x3540ac1);
    if ((rb_state9.var_1) == (0x8dfc80e0)) {
        rb_state9.var_13 = 0x369b5544;
        rb_state9.var_14 = 0x6dc8a3af;
        rb_state9.var_15 = (rb_state9.var_15) + (114726 < rb_input_size ? (uint32_t)rb_input[114726] : 0xb358704d);
        rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_13) : (0xe1545099));
        rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_14);
        rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_16) : (0x83b74ea5));
        rb_state9.var_19 = (rb_state9.var_19) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_17) : (0x4a1f2179));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((((0xb8906049) + (0x3a283354)) - (0x2c46681d)) - (rb_state9.var_19)) + (rb_state9.var_18));
        rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x20)) {
        rb_state12.var_4 = rb_state12.var_0;
    }
    if ((rb_state12.var_3) == (0x20)) {
        if (!((rb_state12.var_0) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) - (0x7508a595);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0xb895ef29));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_4);
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_1);
    if ((rb_state18.var_2) == (0x3cf2c511)) {
        rb_state18.var_6 = 0x1b7a49dc;
        rb_state18.var_7 = 0xb415e9b8;
        rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_5) : (0x6ffae682));
        rb_state18.var_9 = (rb_state18.var_9) + (((0xd070ab8) + (rb_state18.var_6)) - (rb_state18.var_6));
        rb_state18.var_10 = (rb_state18.var_10) ^ (((0x4ea6aac0) + (rb_state18.var_8)) ^ (0xe90b9049));
        rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0xcaabb3f2));
        rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_11);
    }
    #endif
    lc->freeUpdate = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (0x5b146f93);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
    rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_3);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) + (140827 < rb_input_size ? (uint32_t)rb_input[140827] : 0x3c7ad848);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x62beb6de)) {
        pthread_mutex_lock(&(rb_state14.lock_7));
        rb_state14.var_5 = 0x43ae60c6;
        rb_state14.var_6 = (rb_state14.var_6) + (((((0x22627e40) - (rb_state14.var_6)) + (0xfbc1315e)) + (rb_state14.var_5)) - (rb_state14.var_4));
        rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_6);
        pthread_mutex_unlock(&(rb_state14.lock_7));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_4) == (0xebf444b0)) ? (rb_state18.var_3) : (0x61ad1c6b));
    if ((rb_state18.var_2) == (0x3cf2c511)) {
        rb_state18.var_5 = rb_state18.var_3;
    }
    if ((rb_state18.var_2) == (0x3cf2c511)) {
        if (!((rb_state18.var_3) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (57682 < rb_input_size ? (uint32_t)rb_input[57682] : 0x1e0c52b3);
    #endif
    lc->freeTask = NULL;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + ((0x8dfc80e0) - (rb_state9.var_1));
    rb_state9.var_2 = (rb_state9.var_2) - (0x5e30535a);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) + (((rb_state13.var_0) == (0x60)) ? (rb_state13.var_4) : (0x7fa93c90));
    if ((rb_state13.var_1) == (0xd8be7638)) {
        pthread_mutex_lock(&(rb_state13.lock_22));
        rb_state13.var_7 = 0x2aba9b25;
        rb_state13.var_8 = 0x2e1db24d;
        rb_state13.var_9 = (rb_state13.var_9) + (((0x209c912) - (rb_state13.var_5)) + (rb_state13.var_6));
        rb_state13.var_10 = (rb_state13.var_10) ^ (((((0x747d5b7b) + (rb_state13.var_7)) - (rb_state13.var_8)) - (rb_state13.var_7)) + (rb_state13.var_8));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_9) == (0x209c912)) ? (rb_state13.var_10) : (0x96612831));
        rb_state13.var_6 = (rb_state13.var_6) ^ (rb_state13.var_11);
        rb_state13.var_5 = rb_state13.var_6;
        pthread_mutex_unlock(&(rb_state13.lock_22));
    }
    if ((rb_state13.var_1) == (0xd8be7638)) {
        if (!((rb_state13.var_5) == (rb_state13.var_6))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ ((0xfa8410e2) - (rb_state15.var_3));
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_1);
    rb_state15.var_2 = (rb_state15.var_2) - (164864 < rb_input_size ? (uint32_t)rb_input[164864] : 0xa6275fb6);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_2);
    #endif
    lc->runtime = 0;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) + ((0xed1ffa5d) ^ (0x899a894e));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x5fa6ef3f));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - (0x82189b7e);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) ^ (26436 < rb_input_size ? (uint32_t)rb_input[26436] : 0x67f4267b);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (0xfcf9fde7);
    #endif
    PreAllocateFO(MyNum, lc);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - ((0xa9d3eed0) ^ (rb_state1.var_1));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state10.lock_23));
        rb_state10.var_6 = 0x38127653;
        rb_state10.var_7 = 0xf79eb9ed;
        rb_state10.var_8 = (rb_state10.var_8) - (((((0xaa88deaf) + (rb_state10.var_7)) - (0xb1d81216)) + (rb_state10.var_4)) - (0x4cadeff9));
        rb_state10.var_9 = (rb_state10.var_9) + (((0x80ce4325) - (rb_state10.var_5)) + (rb_state10.var_6));
        rb_state10.var_10 = (rb_state10.var_10) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_8) : (0x491107c0));
        rb_state10.var_11 = (rb_state10.var_11) - (((((0xff27980d) + (rb_state10.var_9)) + (0xe917067e)) + (rb_state10.var_7)) + (rb_state10.var_10));
        rb_state10.var_5 = (rb_state10.var_5) ^ (rb_state10.var_11);
        rb_state10.var_4 = rb_state10.var_5;
        pthread_mutex_unlock(&(rb_state10.lock_23));
    }
    if ((rb_state10.var_0) == (0x0)) {
        if (!((rb_state10.var_4) == (rb_state10.var_5))) {
            racebench_trigger(10);
        }
    }
    #endif
    PreProcessFO(MyNum, lc);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_3) == (0x0)) {
            rb_state4.var_6 = 0x64102bd5;
            rb_state4.var_7 = 0x4aa37b63;
            rb_state4.var_8 = (rb_state4.var_8) - ((0xf666f44b) - (0xac5d2fa6));
            rb_state4.var_9 = (rb_state4.var_9) - (rb_state4.var_6);
            rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0x103d5389));
            rb_state4.var_11 = (rb_state4.var_11) - (((((0x4b6c9d04) + (rb_state4.var_8)) - (0xc4c908e5)) ^ (0x86393b7d)) + (rb_state4.var_7));
            rb_state4.var_12 = (rb_state4.var_12) ^ (((0x3e10aee0) ^ (rb_state4.var_9)) ^ (0x5bf570bb));
            rb_state4.var_13 = (rb_state4.var_13) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0x3389ba32));
            rb_state4.var_14 = (rb_state4.var_14) + (((((0x756bc186) ^ (rb_state4.var_13)) ^ (rb_state4.var_8)) + (rb_state4.var_12)) - (rb_state4.var_7));
            rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_14);
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
                #ifdef RACEBENCH_BUG_4
                if ((rb_state4.var_3) == (0x0)) {
                    rb_state4.var_5 = rb_state4.var_4;
                }
                if ((rb_state4.var_3) == (0x0)) {
                    if (!((rb_state4.var_4) == (rb_state4.var_5))) {
                        racebench_trigger(4);
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