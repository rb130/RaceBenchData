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
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xcc5e4dea));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0xb241f9bf));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0xd58140a9) ^ (rb_state16.var_1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0x639965ac));
    #endif
    pid = gm->pid++;
    { pthread_mutex_unlock(&(gm->pidlock)); }

    if ((pid == 0) || (dostats)) {
        struct timeval FullTime;

        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0x12535ea1) + (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0x64414e2b);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) + (0x70110cc3);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) - (0x5421d297);
        rb_state11.var_2 = (rb_state11.var_2) ^ (99 < rb_input_size ? (uint32_t)rb_input[99] : 0x85117bf5);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ ((0x93dce3b5) + (0xd46016f7));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_2);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_0);
        rb_state15.var_2 = (rb_state15.var_2) ^ (0xfa3fec1a);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0xd58140a9)) ? (rb_state16.var_0) : (0x487471b));
        #endif
        gettimeofday(&FullTime, NULL);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (441 < rb_input_size ? (uint32_t)rb_input[441] : 0x4e8b8018);
        rb_state10.var_2 = (rb_state10.var_2) ^ (0xae0c0dcf);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (0x74f67655);
        rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_1) == (0x1cbd569a)) ? (rb_state13.var_0) : (0x325f6a88));
        rb_state13.var_2 = (rb_state13.var_2) - (23 < rb_input_size ? (uint32_t)rb_input[23] : 0x72064c67);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ ((0x20014254) - (rb_state15.var_1));
        rb_state15.var_0 = (rb_state15.var_0) - (244 < rb_input_size ? (uint32_t)rb_input[244] : 0x200f4969);
        rb_state15.var_2 = (rb_state15.var_2) ^ (0xaf7d2341);
        #endif
        (begin) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (0xe4137f82);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (485 < rb_input_size ? (uint32_t)rb_input[485] : 0x3f94bfef);
    rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_2);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0x9ca35664);
    rb_state10.var_3 = (rb_state10.var_3) - (0x893bb27a);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0xfffff925)) {
        if ((rb_state13.var_3) != (0x0)) {
            if (!((rb_state13.var_3) == (rb_state13.var_17))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (0xf588d8d3);
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
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) - (406 < rb_input_size ? (uint32_t)rb_input[406] : 0x1715009);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ ((0x8ca4b3e1) - (rb_state10.var_0));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (68 < rb_input_size ? (uint32_t)rb_input[68] : 0x13f0e3e7);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(StartRayTrace), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x78d31a1b));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + ((0xf5dad3e9) + (rb_state13.var_0));
        rb_state13.var_1 = (rb_state13.var_1) ^ (0xe96785e5);
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