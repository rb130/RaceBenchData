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
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xddd33210));
    #endif
    pid = gm->pid++;
    { pthread_mutex_unlock(&(gm->pidlock)); }

    if ((pid == 0) || (dostats)) {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (0xf0840b1b);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (0x99dda4b0);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (0x8d9afef4);
        #endif
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x95e5cc76);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (0x67aba4a4);
    if ((rb_state3.var_0) == (0x719429b)) {
        pthread_mutex_lock(&(rb_state3.lock_9));
        rb_state3.var_2 = rb_state3.var_1;
        pthread_mutex_unlock(&(rb_state3.lock_9));
    }
    if ((rb_state3.var_0) == (0x719429b)) {
        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
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
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(StartRayTrace), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - ((0x27826c5) ^ (0x7eb58c95));
        if ((rb_state16.var_0) == (0x833255b0)) {
            rb_state16.var_13 = 0x59361ad4;
            rb_state16.var_14 = 0xc4437af9;
            rb_state16.var_15 = (rb_state16.var_15) ^ (rb_state16.var_14);
            rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_8) == (0xc76260b1)) ? (rb_state16.var_7) : (0x6b499235));
            rb_state16.var_17 = (rb_state16.var_17) ^ (((rb_state16.var_13) == (0x59361ad4)) ? (rb_state16.var_15) : (0x5bd7cd02));
            rb_state16.var_18 = (rb_state16.var_18) + (((((0x9ccfa7dc) - (rb_state16.var_16)) - (rb_state16.var_9)) - (rb_state16.var_17)) - (rb_state16.var_10));
            rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_18);
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