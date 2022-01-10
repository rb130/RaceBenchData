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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_1) : (0x35983872));
    rb_state0.var_0 = (rb_state0.var_0) - (119 < rb_input_size ? (uint32_t)rb_input[119] : 0x1fd0af3);
    rb_state0.var_3 = (rb_state0.var_3) - (301 < rb_input_size ? (uint32_t)rb_input[301] : 0x6d19594c);
    if ((rb_state0.var_0) == (0xffffff2c)) {
        pthread_mutex_lock(&(rb_state0.lock_23));
        rb_state0.var_7 = 0x222b62f9;
        rb_state0.var_8 = 0xcee8f7e2;
        rb_state0.var_9 = (rb_state0.var_9) + (rb_state0.var_7);
        rb_state0.var_10 = (rb_state0.var_10) ^ ((0xcbf11622) - (0x6e92fb9c));
        rb_state0.var_11 = (rb_state0.var_11) + (((((0xfcc0ea7e) - (rb_state0.var_5)) + (rb_state0.var_6)) ^ (rb_state0.var_8)) - (0x896ee470));
        rb_state0.var_12 = (rb_state0.var_12) - (((0xd2fce90c) ^ (rb_state0.var_9)) + (0xf4306069));
        rb_state0.var_13 = (rb_state0.var_13) - (rb_state0.var_10);
        rb_state0.var_14 = (rb_state0.var_14) - (((((0x642d7edc) - (rb_state0.var_8)) + (rb_state0.var_11)) + (rb_state0.var_7)) ^ (rb_state0.var_12));
        rb_state0.var_15 = (rb_state0.var_15) - (((0x3be633a6) ^ (rb_state0.var_13)) - (0x457f4f44));
        rb_state0.var_16 = (rb_state0.var_16) ^ (rb_state0.var_14);
        rb_state0.var_17 = (rb_state0.var_17) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x4a413850));
        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_17);
        rb_state0.var_5 = rb_state0.var_6;
        pthread_mutex_unlock(&(rb_state0.lock_23));
    }
    if ((rb_state0.var_0) == (0xffffff2c)) {
        if (!((rb_state0.var_5) == (rb_state0.var_6))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (167 < rb_input_size ? (uint32_t)rb_input[167] : 0x7db75ce);
    rb_state3.var_2 = (rb_state3.var_2) - (rb_state3.var_2);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x289f0b92));
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_1) : (0xe366bd5a));
    rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_2) : (0xa2243071));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (540 < rb_input_size ? (uint32_t)rb_input[540] : 0x5ac77fe1);
    rb_state6.var_1 = (rb_state6.var_1) + (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_2) : (0xb9207439));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (0xa1ce9dd);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (0x9fbd95ee);
    rb_state16.var_2 = (rb_state16.var_2) ^ (16 < rb_input_size ? (uint32_t)rb_input[16] : 0x4c94a441);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x90fcdd1d));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
    #endif
    pid = gm->pid++;
    { pthread_mutex_unlock(&(gm->pidlock)); }

    if ((pid == 0) || (dostats)) {
        struct timeval FullTime;

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_2);
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_1);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_1);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0xc3db870a));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (167 < rb_input_size ? (uint32_t)rb_input[167] : 0xfd17c6);
        if ((rb_state6.var_4) == (0x41dc123a)) {
            pthread_mutex_lock(&(rb_state6.lock_21));
            rb_state6.var_15 = 0x2b3bf481;
            rb_state6.var_16 = 0xd3cc0409;
            rb_state6.var_17 = (rb_state6.var_17) ^ (rb_state6.var_10);
            rb_state6.var_18 = (rb_state6.var_18) - (((((0x8f580995) ^ (rb_state6.var_15)) + (rb_state6.var_12)) - (rb_state6.var_16)) + (rb_state6.var_11));
            rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_17) : (0x4639560));
            rb_state6.var_20 = (rb_state6.var_20) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_19) : (0xaf661d49));
            rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_20);
            pthread_mutex_unlock(&(rb_state6.lock_21));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x8a0709c3));
        rb_state12.var_3 = (rb_state12.var_3) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_3) : (0x7274c019));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (0x723774ef);
        rb_state16.var_4 = (rb_state16.var_4) ^ (0x303bb10);
        if ((rb_state16.var_4) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_26));
            rb_state16.var_7 = 0xf2c80209;
            rb_state16.var_8 = (rb_state16.var_8) + (rb_state16.var_7);
            rb_state16.var_9 = (rb_state16.var_9) ^ (((((0xa3fec013) ^ (rb_state16.var_8)) + (rb_state16.var_6)) - (rb_state16.var_5)) ^ (rb_state16.var_9));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_7);
            rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0x80a034b));
            rb_state16.var_12 = (rb_state16.var_12) ^ (((0xf18afe99) + (rb_state16.var_10)) + (rb_state16.var_10));
            rb_state16.var_13 = (rb_state16.var_13) - (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_11) : (0x3374f20b));
            rb_state16.var_14 = (rb_state16.var_14) ^ (((((0x20b469fd) ^ (0x10498582)) + (rb_state16.var_13)) - (rb_state16.var_12)) + (rb_state16.var_12));
            rb_state16.var_6 = (rb_state16.var_6) ^ (rb_state16.var_14);
            rb_state16.var_5 = rb_state16.var_6;
            pthread_mutex_unlock(&(rb_state16.lock_26));
        }
        if ((rb_state16.var_4) == (0x0)) {
            if (!((rb_state16.var_5) == (rb_state16.var_6))) {
                racebench_trigger(16);
            }
        }
        #endif
        gettimeofday(&FullTime, NULL);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_4 = (rb_state0.var_4) ^ (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_3) : (0xe31af7f2));
        rb_state0.var_3 = (rb_state0.var_3) - (0xe9ce7ac6);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_0) : (0xd00f3cbb));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (0xc985bf99);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) + ((0xfe7df265) + (rb_state11.var_1));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0x2aab832b) + (rb_state15.var_1));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - ((0x4db9d0b3) ^ (rb_state16.var_2));
        rb_state16.var_3 = (rb_state16.var_3) - ((0xabc755eb) + (rb_state16.var_0));
        rb_state16.var_1 = (rb_state16.var_1) ^ (499 < rb_input_size ? (uint32_t)rb_input[499] : 0xd5665d3f);
        if ((rb_state16.var_4) == (0x0)) {
            rb_state16.var_15 = 0x9f6f5780;
            rb_state16.var_16 = (rb_state16.var_16) ^ ((0xe6557093) ^ (rb_state16.var_15));
            rb_state16.var_17 = (rb_state16.var_17) + (((0xfc08da31) + (rb_state16.var_14)) + (0xbd7257e9));
            rb_state16.var_18 = (rb_state16.var_18) - (rb_state16.var_13);
            rb_state16.var_19 = (rb_state16.var_19) ^ (((((0x56b2ce8b) + (rb_state16.var_16)) - (rb_state16.var_15)) - (rb_state16.var_16)) + (rb_state16.var_17));
            rb_state16.var_20 = (rb_state16.var_20) - (((0x65449118) - (0xe7be8b1)) + (rb_state16.var_17));
            rb_state16.var_21 = (rb_state16.var_21) - (((rb_state16.var_18) == (0x3374f20b)) ? (rb_state16.var_18) : (0x21551053));
            rb_state16.var_22 = (rb_state16.var_22) ^ (((0xf058d2e7) - (rb_state16.var_19)) - (0xc0a42abf));
            rb_state16.var_23 = (rb_state16.var_23) - (((((0x2881351e) + (0xe73f79af)) + (rb_state16.var_21)) ^ (0xc94aa79)) + (rb_state16.var_20));
            rb_state16.var_24 = (rb_state16.var_24) ^ (rb_state16.var_22);
            rb_state16.var_25 = (rb_state16.var_25) + (((((0x1a5c5006) + (rb_state16.var_23)) - (rb_state16.var_19)) - (rb_state16.var_24)) + (0x8d02dcee));
            rb_state16.var_5 = (rb_state16.var_5) - (rb_state16.var_25);
        }
        #endif
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_4 = (rb_state0.var_4) ^ (0x65decf4);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ ((0xa0340c53) + (rb_state5.var_3));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (426 < rb_input_size ? (uint32_t)rb_input[426] : 0xb705af16);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (0x9f94dd0a);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (0xf82c89da);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_2) : (0x2b12b43b));
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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xe6375ee));
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (143 < rb_input_size ? (uint32_t)rb_input[143] : 0x5dbe8e42);
            rb_state6.var_1 = (rb_state6.var_1) + (0x5ba7dbb0);
            rb_state6.var_2 = (rb_state6.var_2) + (112 < rb_input_size ? (uint32_t)rb_input[112] : 0x729a677b);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(StartRayTrace), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x2aa40680));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_2);
        rb_state16.var_3 = (rb_state16.var_3) ^ (rb_state16.var_3);
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