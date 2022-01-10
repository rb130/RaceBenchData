#include "racebench_bugs.h"


#define MAIN
#define VERSION "1.00"

#include <stdio.h>
#include <math.h>
#include "rt.h"

CHAR *ProgName = "RAYTRACE";
INT nprocs = 1;
INT MaxGlobMem = 32;
INT NumSubRays = 1;
INT dostats = 0;

VOID Usage() {
    fprintf(stdout, "%s - parallel ray tracer\n", ProgName);
    fprintf(stdout, "Version %s\n\n", VERSION);

    fprintf(stdout, "Usage:\t%s [options] envfile\n\n", ProgName);

    fprintf(stdout, "\t-h\tPrint this usage message.\n");
    fprintf(stdout, "\t-a<n>\tEnable antialiasing with n subpixels (default = 1).\n\tWhen using with SPLASH suite for evaluation, use default (no antialiasing)\n");
    fprintf(stdout, "\t-m<n>\tRequest n megabytes of global memory (default = 32).\n");
    fprintf(stdout, "\t-p<n>\tRun on n processors (default = 1).\n");
    fprintf(stdout, "\t-s\tMeasure and print per-process timing information.\n");
    fprintf(stdout, "\n");
}

VOID PrintStatistics() {

    if (TraversalType == TT_HUG) {

        ma_print();
    }
}

VOID StartRayTrace() {
    INT pid;
    UINT begin;
    UINT end;
    UINT lapsed;

    { pthread_mutex_lock(&(gm->pidlock)); }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + ((0x3e6bd8ce) - (0x4d949acb));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (0xe9f254d3);
    #endif
    pid = gm->pid++;
    { pthread_mutex_unlock(&(gm->pidlock)); }

    if ((pid == 0) || (dostats)) {
        struct timeval FullTime;

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (0x1a381a87);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_2) == (0xb495887e)) {
            pthread_mutex_lock(&(rb_state4.lock_33));
            rb_state4.var_5 = 0x3eaf599a;
            rb_state4.var_6 = (rb_state4.var_6) + ((0xabe50b38) - (rb_state4.var_5));
            rb_state4.var_7 = (rb_state4.var_7) + (0x72a908bd);
            rb_state4.var_8 = (rb_state4.var_8) ^ (((0xb50fcef1) + (rb_state4.var_3)) ^ (rb_state4.var_6));
            rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_5);
            rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_4) : (0x87a8abb8));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_6) : (0x5285ecc1));
            rb_state4.var_12 = (rb_state4.var_12) - (((((0xd5de7b91) ^ (rb_state4.var_8)) ^ (rb_state4.var_9)) + (rb_state4.var_7)) + (0x83087ac5));
            rb_state4.var_13 = (rb_state4.var_13) + (((((0xd504d6fc) + (0x52717e44)) + (rb_state4.var_9)) + (rb_state4.var_10)) - (rb_state4.var_10));
            rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_11) : (0x54b45f3d));
            rb_state4.var_15 = (rb_state4.var_15) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_12) : (0x237b7acb));
            rb_state4.var_16 = (rb_state4.var_16) ^ (rb_state4.var_13);
            rb_state4.var_17 = (rb_state4.var_17) ^ (((0x57aca6ec) - (rb_state4.var_14)) - (rb_state4.var_13));
            rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_15) : (0xc464d0fe));
            rb_state4.var_19 = (rb_state4.var_19) - (((((0x1ab660e1) - (rb_state4.var_16)) ^ (rb_state4.var_15)) - (rb_state4.var_16)) - (rb_state4.var_17));
            rb_state4.var_20 = (rb_state4.var_20) - (((((0xe5ce8944) - (rb_state4.var_18)) ^ (0x4c1c9d10)) - (0x12289cbe)) + (rb_state4.var_19));
            rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_20);
            rb_state4.var_3 = rb_state4.var_4;
            pthread_mutex_unlock(&(rb_state4.lock_33));
        }
        if ((rb_state4.var_2) == (0xb495887e)) {
            if (!((rb_state4.var_3) == (rb_state4.var_4))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_2) : (0xd757df17));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0xa2f40a38));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - ((0x24158f0c) + (0x483743ee));
        rb_state15.var_1 = (rb_state15.var_1) + (409 < rb_input_size ? (uint32_t)rb_input[409] : 0x985146db);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + ((0xc8a8050f) - (0xe91b281f));
        rb_state19.var_0 = (rb_state19.var_0) - (427 < rb_input_size ? (uint32_t)rb_input[427] : 0x7fe22ca6);
        #endif
        gettimeofday(&FullTime, NULL);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (329 < rb_input_size ? (uint32_t)rb_input[329] : 0x634fa2b5);
        rb_state1.var_2 = (rb_state1.var_2) + ((0x67bf1879) - (rb_state1.var_1));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - ((0x29058e89) + (0xc1c91070));
        rb_state5.var_0 = (rb_state5.var_0) + (0xe8ded5c2);
        rb_state5.var_1 = (rb_state5.var_1) - (0x46d1bc39);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0xe7591225));
        if ((rb_state6.var_1) == (0xb0079542)) {
            rb_state6.var_14 = 0x8615164a;
            rb_state6.var_15 = (rb_state6.var_15) ^ (321 < rb_input_size ? (uint32_t)rb_input[321] : 0xdd99b569);
            rb_state6.var_16 = (rb_state6.var_16) - (((((0xe25be701) ^ (rb_state6.var_14)) - (rb_state6.var_13)) - (rb_state6.var_16)) - (rb_state6.var_15));
            rb_state6.var_17 = (rb_state6.var_17) + (214 < rb_input_size ? (uint32_t)rb_input[214] : 0x1079d2ac);
            rb_state6.var_18 = (rb_state6.var_18) ^ (((((0x6fc833e) ^ (0x8e04f140)) ^ (rb_state6.var_15)) - (0xcccea576)) ^ (rb_state6.var_16));
            rb_state6.var_19 = (rb_state6.var_19) ^ (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_17) : (0x73e5f279));
            rb_state6.var_20 = (rb_state6.var_20) ^ (rb_state6.var_18);
            rb_state6.var_21 = (rb_state6.var_21) ^ (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_19) : (0xc6c1a271));
            rb_state6.var_22 = (rb_state6.var_22) + (((0x91ea1b80) + (0xd043b550)) ^ (rb_state6.var_20));
            rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_21) : (0xff4730fe));
            rb_state6.var_24 = (rb_state6.var_24) - (((((0x77b5ce63) + (rb_state6.var_22)) - (0x7c82a66c)) + (0x30bb733f)) ^ (rb_state6.var_23));
            rb_state6.var_13 = (rb_state6.var_13) - (rb_state6.var_24);
            rb_state6.var_2 = rb_state6.var_13;
        }
        if ((rb_state6.var_1) == (0xb0079542)) {
            pthread_mutex_lock(&(rb_state6.lock_33));
            rb_state6.var_26 = 0x6ae85add;
            rb_state6.var_27 = 0x9e491ddb;
            rb_state6.var_28 = (rb_state6.var_28) ^ (((((0x65664f85) + (rb_state6.var_21)) ^ (rb_state6.var_20)) ^ (rb_state6.var_26)) - (rb_state6.var_22));
            rb_state6.var_29 = (rb_state6.var_29) - (((rb_state6.var_23) == (0x0)) ? (rb_state6.var_27) : (0x342a5185));
            rb_state6.var_30 = (rb_state6.var_30) ^ (((rb_state6.var_24) == (0x0)) ? (rb_state6.var_28) : (0xdf9d8b6e));
            rb_state6.var_31 = (rb_state6.var_31) - (rb_state6.var_29);
            rb_state6.var_32 = (rb_state6.var_32) - (((((0x3739a811) - (0x5e0b44e7)) - (rb_state6.var_30)) + (rb_state6.var_31)) - (0xc6d1b46d));
            rb_state6.var_25 = (rb_state6.var_25) ^ (rb_state6.var_32);
            rb_state6.var_2 = rb_state6.var_25;
            pthread_mutex_unlock(&(rb_state6.lock_33));
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xfc7616cb));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (0x16d4f24c);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (94 < rb_input_size ? (uint32_t)rb_input[94] : 0xe6cf3c45);
        #endif
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (292 < rb_input_size ? (uint32_t)rb_input[292] : 0x971f8ee6);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0xe08c369)) {
        if ((rb_state7.var_3) != (0x0)) {
            if (!((rb_state7.var_3) == (rb_state7.var_23))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0xd8c42f2b));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + ((0xf59a9672) ^ (rb_state12.var_0));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + ((0x4746996e) - (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - ((0x35ba4db1) + (rb_state15.var_0));
    #endif
    InitWorkPool(pid);
    InitRayTreeStack(Display.maxlevel, pid);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(gm->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (gm->start).cycle;
        if (++(gm->start).counter != (gm->nprocs)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (gm->start).cycle) {
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_19) == (0xa359f94)) {
                    rb_state2.var_29 = rb_state2.var_21;
                }
                if ((rb_state2.var_19) == (0xa359f94)) {
                    if (!((rb_state2.var_21) == (rb_state2.var_29))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                Error = pthread_cond_wait(&(gm->start).cv, &(gm->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (gm->start).cycle = !(gm->start).cycle;
            (gm->start).counter = 0;
            Error = pthread_cond_broadcast(&(gm->start).cv);
        }
        pthread_mutex_unlock(&(gm->start).mutex);
    }

    RayTrace(pid);

    if ((pid == 0) || (dostats)) {
        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };
        gm->partime[pid] = (end - begin) & 0x7FFFFFFF;
        if (pid == 0) {
            gm->par_start_time = begin;
        }
    }
}

int main(argc, argv)
int argc;
CHAR *argv[];
{
    INT i;
    UINT begin;
    UINT end;
    UINT lapsed;
    U16 tmp;
    CHAR *pch;
    MATRIX vtrans, Vinv;

    while (argc-- > 1 && (*++argv)[0] == '-') {
        for (pch = argv[0] + 1; *pch != '\0'; pch++) {
            switch (*pch) {
            case '?':
            case 'h':
            case 'H':
                Usage();
                exit(1);

            case 'a':
            case 'A':
                AntiAlias = TRUE;
                if (*++pch) {
                    NumSubRays = atoi(pch);
                }

                *pch = '\0';
                pch--;
                break;

            case 'm':
                if (*++pch) {
                    MaxGlobMem = atoi(pch);
                }

                *pch = '\0';
                pch--;
                break;

            case 'p':
                if (*++pch) {
                    nprocs = atoi(pch);
                }

                *pch = '\0';
                pch--;
                break;

            case 's':
            case 'S':
                dostats = TRUE;
                break;

            case 'g':
                strncpy(GeoFileName, pch + 2, sizeof(GeoFileName) - 1);
                *pch = '\0';
                pch--;
                break;

            default:
                fprintf(stderr, "%s: Invalid option \'%c\'.\n", ProgName, *pch);
                exit(1);
            }
        }
    }

    if (!argc) {
        Usage();
        exit(1);
    }

    if (nprocs < 1 || nprocs > MAX_PROCS) {
        fprintf(stderr, "%s: Valid range for #processors is [1, %ld].\n", ProgName, MAX_PROCS);
        exit(1);
    }

    printf("\n");
    printf("Number of processors:     \t%ld\n", nprocs);
    printf("Global shared memory size:\t%ld MB\n", MaxGlobMem);
    printf("Samples per pixel:        \t%ld\n", NumSubRays);
    printf("\n");

    MaxGlobMem <<= 20;
    { ; }
    gm = (GMEM *)malloc(sizeof(GMEM));
    ;

    gm->nprocs = nprocs;
    gm->pid = 0;
    gm->rid = 1;

    {
        unsigned long Error;

        Error = pthread_mutex_init(&(gm->start).mutex, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            exit(-1);
        }

        Error = pthread_cond_init(&(gm->start).cv, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            pthread_mutex_destroy(&(gm->start).mutex);
            exit(-1);
        }

        (gm->start).counter = 0;
        (gm->start).cycle = 0;
    }
    { pthread_mutex_init(&(gm->pidlock), NULL); }
    { pthread_mutex_init(&(gm->ridlock), NULL); }
    { pthread_mutex_init(&(gm->memlock), NULL); }
    {
        unsigned long i, Error;

        for (i = 0; i < nprocs; i++) {
            Error = pthread_mutex_init(&gm->wplock[i], NULL);
            if (Error != 0) {
                printf("Error while initializing array of locks.\n");
                exit(-1);
            }
        }
    }

    if (!GlobalHeapInit(MaxGlobMem)) {
        fprintf(stderr, "%s: Cannot initialize global heap.\n", ProgName);
        exit(1);
    }

    Huniform_defaults();
    ReadEnvFile(*argv);
    ReadGeoFile(GeoFileName);
    OpenFrameBuffer();

    CreateViewMatrix();
    MatrixCopy(vtrans, View.vtrans);
    MatrixInverse(Vinv, vtrans);
    MatrixCopy(View.vtransInv, Vinv);

    printf("Number of primitive objects: \t%ld\n", prim_obj_cnt);
    printf("Number of primitive elements:\t%ld\n", prim_elem_cnt);

    if (TraversalType == TT_HUG) {
        BuildHierarchy_Uniform();
    }

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    }

    {
        long i, Error;

        for (i = 0; i < (gm->nprocs) - 1; i++) {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ ((0xe68c689e) - (0x429fdd8e));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(StartRayTrace), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ ((0xcdd6e8c1) ^ (0x86bc9f43));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + (380 < rb_input_size ? (uint32_t)rb_input[380] : 0xccdb9beb);
        rb_state6.var_1 = (rb_state6.var_1) + ((0xb0079542) - (rb_state6.var_1));
        #endif
        StartRayTrace();
    }

    {
        unsigned long i, Error;
        for (i = 0; i < (gm->nprocs) - 1; i++) {
            Error = pthread_join(PThreadTable[i], NULL);
            if (Error != 0) {
                printf("Error in pthread_join().\n");
                exit(-1);
            }
        }
    }
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (end) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    }

    CloseFrameBuffer(PicFileName);
    PrintStatistics();

    lapsed = (end - begin) & 0x7FFFFFFF;

    printf("TIMING STATISTICS MEASURED BY MAIN PROCESS:\n");
    printf("        Overall start time     %20lu\n", begin);
    printf("        Overall end time   %20lu\n", end);
    printf("        Total time with initialization  %20lu\n", lapsed);
    printf("        Total time without initialization  %20lu\n", end - gm->par_start_time);

    if (dostats) {
        unsigned totalproctime, maxproctime, minproctime;

        printf("\n\n\nPER-PROCESS STATISTICS:\n");

        printf("%20s%20s\n", "Proc", "Time");
        printf("%20s%20s\n\n", "", "Tracing Rays");
        for (i = 0; i < gm->nprocs; i++) {
            printf("%20d%20d\n", i, gm->partime[i]);
        }

        totalproctime = gm->partime[0];
        minproctime = gm->partime[0];
        maxproctime = gm->partime[0];

        for (i = 1; i < gm->nprocs; i++) {
            totalproctime += gm->partime[i];
            if (gm->partime[i] > maxproctime) {
                maxproctime = gm->partime[i];
            }
            if (gm->partime[i] < minproctime) {
                minproctime = gm->partime[i];
            }
        }
        printf("\n\n%20s%20d\n", "Max = ", maxproctime);
        printf("%20s%20d\n", "Min = ", minproctime);
        printf("%20s%20d\n", "Avg = ", (int)(((double)totalproctime) / ((double)(1.0 * gm->nprocs))));
    }

    { exit(0); }
}