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
    rb_state1.var_0 = (rb_state1.var_0) - (70 < rb_input_size ? (uint32_t)rb_input[70] : 0xaf429afe);
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0xffffffce)) ? (rb_state1.var_0) : (0x5844ff69));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x9aeaa4d1) + (rb_state2.var_0));
    if ((rb_state2.var_0) == (0xd0bfee73)) {
        rb_state2.var_13 = (rb_state2.var_13) ^ (((0x66f34c58) ^ (0x1b9ced4a)) - (rb_state2.var_8));
        rb_state2.var_14 = (rb_state2.var_14) ^ (((0xe1d5e508) + (rb_state2.var_9)) ^ (0x88d68112));
        rb_state2.var_15 = (rb_state2.var_15) - (rb_state2.var_7);
        rb_state2.var_16 = (rb_state2.var_16) - (((((0x5011e735) ^ (rb_state2.var_10)) + (rb_state2.var_13)) - (0x5fe9378f)) ^ (rb_state2.var_14));
        rb_state2.var_17 = (rb_state2.var_17) - (((((0xbdd5bce2) ^ (rb_state2.var_16)) - (0x39d234c5)) ^ (0x3fb84772)) + (rb_state2.var_15));
        rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_17);
        rb_state2.var_2 = rb_state2.var_12;
    }
    if ((rb_state2.var_0) == (0xd0bfee73)) {
        pthread_mutex_lock(&(rb_state2.lock_25));
        rb_state2.var_19 = 0xb2f3fef1;
        rb_state2.var_20 = (rb_state2.var_20) - (((0xa2bdd2f4) + (rb_state2.var_19)) ^ (0x474d1d5d));
        rb_state2.var_21 = (rb_state2.var_21) + (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_12) : (0x461760f1));
        rb_state2.var_22 = (rb_state2.var_22) ^ (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_11) : (0x92d10d3d));
        rb_state2.var_23 = (rb_state2.var_23) + (((((0x5c1c063c) - (0x55b8c480)) + (rb_state2.var_20)) + (rb_state2.var_21)) - (rb_state2.var_15));
        rb_state2.var_24 = (rb_state2.var_24) - (((((0x91648657) ^ (rb_state2.var_17)) + (rb_state2.var_22)) - (rb_state2.var_16)) ^ (rb_state2.var_23));
        rb_state2.var_18 = (rb_state2.var_18) + (rb_state2.var_24);
        rb_state2.var_2 = rb_state2.var_18;
        pthread_mutex_unlock(&(rb_state2.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xc11fea7a));
    rb_state7.var_1 = (rb_state7.var_1) ^ (0x3321ee3d);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (310 < rb_input_size ? (uint32_t)rb_input[310] : 0xbeab0742);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x505ce094);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_11) == (0x0)) {
        if ((rb_state15.var_20) != (0x0)) {
            if (!((rb_state15.var_20) == (rb_state15.var_53))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (0x98fca913);
    if ((rb_state17.var_1) == (0x98fca8e6)) {
        pthread_mutex_lock(&(rb_state17.lock_21));
        if ((rb_state17.var_2) != (0x0)) {
            if (!((rb_state17.var_2) == (rb_state17.var_15))) {
                racebench_trigger(17);
            }
        }
        pthread_mutex_unlock(&(rb_state17.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + ((0x8bea3a4c) - (0x5c26ebd));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0xf9631a48) - (rb_state19.var_0));
    rb_state19.var_1 = (rb_state19.var_1) ^ (0xe3035095);
    rb_state19.var_1 = (rb_state19.var_1) + (0x5935a1e3);
    #endif
    pid = gm->pid++;
    { pthread_mutex_unlock(&(gm->pidlock)); }

    if ((pid == 0) || (dostats)) {
        struct timeval FullTime;

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - (340 < rb_input_size ? (uint32_t)rb_input[340] : 0x40fd5b32);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_1);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_0) == (0x9aeaa4d1)) ? (rb_state2.var_1) : (0x95d94cff));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xe351bec));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (405 < rb_input_size ? (uint32_t)rb_input[405] : 0xc005dba2);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (57 < rb_input_size ? (uint32_t)rb_input[57] : 0x4869ab89);
        #endif
        gettimeofday(&FullTime, NULL);
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
        rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x65155b2f)) ? (rb_state2.var_0) : (0x5b850757));
        if ((rb_state2.var_0) == (0xd0bfee73)) {
            if ((rb_state2.var_2) != (0x0)) {
                if (!((rb_state2.var_2) == (rb_state2.var_18))) {
                    racebench_trigger(2);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (0xcfcfc93e);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (135 < rb_input_size ? (uint32_t)rb_input[135] : 0x87765324);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (0x5ac52e3d);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (0x496a50cf);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_1);
        if ((rb_state16.var_1) == (0x660947b6)) {
            pthread_mutex_lock(&(rb_state16.lock_35));
            rb_state16.var_10 = 0xc666628e;
            rb_state16.var_11 = 0xf0704ba9;
            rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_11) : (0x9b6cb6e2));
            rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_4);
            rb_state16.var_14 = (rb_state16.var_14) + (((0x329ccef3) ^ (0x7644bf6d)) ^ (rb_state16.var_10));
            rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x4b0f0130));
            rb_state16.var_16 = (rb_state16.var_16) - (((0xd66ee44d) - (rb_state16.var_14)) + (rb_state16.var_5));
            rb_state16.var_17 = (rb_state16.var_17) - (((0xd0de1064) ^ (rb_state16.var_15)) + (rb_state16.var_6));
            rb_state16.var_18 = (rb_state16.var_18) + (((((0x6a6c3a9b) ^ (rb_state16.var_16)) + (rb_state16.var_17)) + (0xb5b8d5c4)) ^ (0xc18b5b5d));
            rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_18);
            rb_state16.var_2 = rb_state16.var_9;
            pthread_mutex_unlock(&(rb_state16.lock_35));
        }
        #endif
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0x57a7f689) ^ (rb_state10.var_1));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x7cd988d4)) ? (rb_state12.var_0) : (0xbe1b6068));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (0xb304a3db);
    if ((rb_state16.var_1) == (0xd50de46d)) {
        pthread_mutex_lock(&(rb_state16.lock_35));
        rb_state16.var_20 = (rb_state16.var_20) ^ (rb_state16.var_8);
        rb_state16.var_21 = (rb_state16.var_21) + (rb_state16.var_10);
        rb_state16.var_22 = (rb_state16.var_22) - (0x4d34a81);
        rb_state16.var_23 = (rb_state16.var_23) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_7) : (0x70ae04c5));
        rb_state16.var_24 = (rb_state16.var_24) + (((0x2c83e924) + (rb_state16.var_9)) ^ (0xcee539ea));
        rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_20);
        rb_state16.var_26 = (rb_state16.var_26) + (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_22) : (0x5a1afd57));
        rb_state16.var_27 = (rb_state16.var_27) + (rb_state16.var_23);
        rb_state16.var_28 = (rb_state16.var_28) ^ (((0xda78483a) - (0x1d326006)) + (rb_state16.var_24));
        rb_state16.var_29 = (rb_state16.var_29) - (((0x24ff3167) ^ (0x83627dd5)) - (rb_state16.var_25));
        rb_state16.var_30 = (rb_state16.var_30) ^ (((((0x9ef47a3b) - (rb_state16.var_27)) - (0x5b54436c)) - (0x269286ff)) + (rb_state16.var_26));
        rb_state16.var_31 = (rb_state16.var_31) + (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_28) : (0xccd8f180));
        rb_state16.var_32 = (rb_state16.var_32) + (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_30) : (0x96bf9ad6));
        rb_state16.var_33 = (rb_state16.var_33) - (((0xb99ab6d9) + (rb_state16.var_13)) - (rb_state16.var_31));
        rb_state16.var_34 = (rb_state16.var_34) + (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_33) : (0xc868310c));
        rb_state16.var_19 = (rb_state16.var_19) ^ (rb_state16.var_34);
        rb_state16.var_2 = rb_state16.var_19;
        pthread_mutex_unlock(&(rb_state16.lock_35));
    }
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
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) ^ ((0x9a85ef16) ^ (0x6d0f568b));
            if ((rb_state1.var_1) == (0x0)) {
                pthread_mutex_lock(&(rb_state1.lock_23));
                rb_state1.var_4 = 0xe3996303;
                rb_state1.var_5 = 0x438e3414;
                rb_state1.var_6 = 0x3eb35290;
                rb_state1.var_7 = (rb_state1.var_7) + (329 < rb_input_size ? (uint32_t)rb_input[329] : 0xa729c15f);
                rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_4) : (0xf963fab6));
                rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_5) : (0x76655904));
                rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_6) : (0x25f4b71d));
                rb_state1.var_11 = (rb_state1.var_11) ^ (((0x740e9a8a) ^ (rb_state1.var_7)) - (0x1f210c29));
                rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_8) : (0xd57f7956));
                rb_state1.var_13 = (rb_state1.var_13) - (((((0x4115afb4) + (rb_state1.var_9)) + (0xa3638064)) ^ (rb_state1.var_10)) ^ (rb_state1.var_6));
                rb_state1.var_14 = (rb_state1.var_14) - (rb_state1.var_11);
                rb_state1.var_15 = (rb_state1.var_15) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_12) : (0x385ad0bc));
                rb_state1.var_16 = (rb_state1.var_16) - (((((0x51309dcf) - (rb_state1.var_8)) - (rb_state1.var_13)) + (rb_state1.var_14)) + (0xbb4b5abb));
                rb_state1.var_17 = (rb_state1.var_17) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_15) : (0xb7b92f87));
                rb_state1.var_18 = (rb_state1.var_18) - (((0x83a7cd84) ^ (rb_state1.var_10)) - (rb_state1.var_16));
                rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_17);
                rb_state1.var_20 = (rb_state1.var_20) - (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_19) : (0x5c61c64));
                rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_20);
                rb_state1.var_2 = rb_state1.var_3;
                pthread_mutex_unlock(&(rb_state1.lock_23));
            }
            if ((rb_state1.var_1) == (0x0)) {
                if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                    racebench_trigger(1);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (574 < rb_input_size ? (uint32_t)rb_input[574] : 0x78d5999f);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (0xe0c99dcb);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xa69a0e81));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_1);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
            rb_state16.var_0 = (rb_state16.var_0) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x57e05fd2));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_0);
            rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x83aa2e2b));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(StartRayTrace), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (140 < rb_input_size ? (uint32_t)rb_input[140] : 0x17156f71);
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x68)) {
            rb_state15.var_4 = 0x1;
        }
        if ((rb_state15.var_0) == (0x68)) {
            pthread_mutex_lock(&(rb_state15.lock_18));
            rb_state15.var_5 = 0xc7bd0ed4;
            rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_4);
            rb_state15.var_7 = (rb_state15.var_7) ^ ((0x9ff43448) + (0xcb65693e));
            rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_3) : (0x2bb2b1d6));
            rb_state15.var_9 = (rb_state15.var_9) ^ (rb_state15.var_5);
            rb_state15.var_10 = (rb_state15.var_10) ^ (((((0xc1768770) ^ (rb_state15.var_6)) - (rb_state15.var_6)) ^ (rb_state15.var_5)) - (rb_state15.var_7));
            rb_state15.var_11 = (rb_state15.var_11) - (rb_state15.var_8);
            rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_9);
            rb_state15.var_13 = (rb_state15.var_13) - (((0x7ed58531) + (rb_state15.var_7)) + (rb_state15.var_10));
            rb_state15.var_14 = (rb_state15.var_14) ^ (((((0xc2735bc3) - (rb_state15.var_12)) + (0x2cd6d3dc)) ^ (rb_state15.var_8)) ^ (rb_state15.var_11));
            rb_state15.var_15 = (rb_state15.var_15) ^ (((0x159b49d0) + (rb_state15.var_13)) - (0x79801887));
            rb_state15.var_16 = (rb_state15.var_16) ^ (((0xf38c37b7) + (rb_state15.var_14)) - (0x2e16fd30));
            rb_state15.var_17 = (rb_state15.var_17) ^ (((rb_state15.var_15) == (0x241135d4)) ? (rb_state15.var_16) : (0xe56227ae));
            rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_17);
            rb_state15.var_2 = rb_state15.var_3;
            pthread_mutex_unlock(&(rb_state15.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - (240 < rb_input_size ? (uint32_t)rb_input[240] : 0x86615033);
        if ((rb_state17.var_1) == (0x1f5)) {
            rb_state17.var_10 = 0x15aab49;
            rb_state17.var_11 = (rb_state17.var_11) + (((0x234230e1) ^ (rb_state17.var_10)) + (0x801b8274));
            rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_9);
            rb_state17.var_13 = (rb_state17.var_13) + (rb_state17.var_11);
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_13) : (0x7a478ded));
            rb_state17.var_9 = (rb_state17.var_9) ^ (rb_state17.var_14);
            rb_state17.var_2 = rb_state17.var_9;
        }
        if ((rb_state17.var_1) == (0x98fca8e6)) {
            pthread_mutex_lock(&(rb_state17.lock_21));
            rb_state17.var_16 = 0xd343690f;
            rb_state17.var_17 = (rb_state17.var_17) ^ (((((0xbc7a05f8) + (0x722688e6)) + (rb_state17.var_11)) ^ (rb_state17.var_16)) + (0x655e0ae3));
            rb_state17.var_18 = (rb_state17.var_18) - (((0xd85a8dc1) + (rb_state17.var_12)) + (rb_state17.var_13));
            rb_state17.var_19 = (rb_state17.var_19) + (((0xd5b128d6) - (0x293181fc)) ^ (rb_state17.var_17));
            rb_state17.var_20 = (rb_state17.var_20) + (((((0xd6b8f329) ^ (rb_state17.var_19)) - (rb_state17.var_18)) ^ (0xb5ea7757)) - (rb_state17.var_14));
            rb_state17.var_15 = (rb_state17.var_15) ^ (rb_state17.var_20);
            rb_state17.var_2 = rb_state17.var_15;
            pthread_mutex_unlock(&(rb_state17.lock_21));
        }
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