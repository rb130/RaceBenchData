#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_THREADS 1024
pthread_t PThreadTable[MAX_THREADS];

#include "stdio.h"
#include "split.h"

#define extern
#include "parameters.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "cnst.h"
#include "mddata.h"
#include "fileio.h"
#include "frcnst.h"
#include "global.h"

struct GlobalMemory *gl;

int NSTEP, NSAVE, NRST, NPRINT, NFMC;
int NORD1;
int II;
int i;
int NDATA;
int NFRST = 11;
int NFSV = 10;
int LKT = 0;

int StartMol[MAXPROCS + 1];

int MolsPerProc;
int NumProcs;

void WorkStart();

const char *input_file;

main(argc, argv) char **argv;
{
    int mol, func, dir, atom;
    double XTT, MDMAIN();
    unsigned ProcID = 0;

    freopen(argv[1], "r", stdin);
    input_file = argv[2];

    six = stdout;

    TEMP = 298.0;
    RHO = 0.9980;
    CUTOFF = 0.0;

    if (scanf("%lf%d%d%d%d%d%d%d%d%lf", &TSTEP, &NMOL, &NSTEP, &NORDER, &NSAVE, &NRST, &NPRINT, &NFMC, &NumProcs, &CUTOFF) != 10) {
        fprintf(stderr, "ERROR: Usage: water < infile, which must have 10 fields, see SPLASH documentation or comment at top of water.C\n");
    }

    if (NMOL > MAXLCKS) {
        fprintf(stderr, "Just so you know ... Lock array in global.H has size %d < %d (NMOL)\n code will still run correctly but there may be lock contention\n\n", MAXLCKS, NMOL);
    }

    printf("Using %d procs on %d steps of %d mols\n", NumProcs, NSTEP, NMOL);
    printf("Other parameters:\n\tTSTEP = %8.2e\n\tNORDER = %d\n\tNSAVE = %d\n", TSTEP, NORDER, NSAVE);
    printf("\tNRST = %d\n\tNPRINT = %d\n\tNFMC = %d\n\tCUTOFF = %lf\n\n", NRST, NPRINT, NFMC, CUTOFF);

    NORD1 = NORDER + 1;

    CNSTNT(NORD1, TLC);

    {
        int pid;
        unsigned mol_size = sizeof(molecule_type) * NMOL;
        unsigned gmem_size = sizeof(struct GlobalMemory);

        { ; };

        VAR = (molecule_type *)malloc(mol_size);
        ;
        gl = (struct GlobalMemory *)malloc(gmem_size);
        ;

        PFORCES = (double ****)malloc(NumProcs * sizeof(double ***));
        ;
        {
            int i, j, k;

            for (i = 0; i < NumProcs; i++) {
                PFORCES[i] = (double ***)malloc(NMOL * sizeof(double **));
                ;
                for (j = 0; j < NMOL; j++) {
                    PFORCES[i][j] = (double **)malloc(NDIR * sizeof(double *));
                    ;
                    for (k = 0; k < NDIR; k++) {
                        PFORCES[i][j][k] = (double *)malloc(NATOM * sizeof(double));
                        ;
                    }
                }
            }
        }

        {
            unsigned long Error;

            Error = pthread_mutex_init(&(gl->start).mutex, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                exit(-1);
            }

            Error = pthread_cond_init(&(gl->start).cv, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                pthread_mutex_destroy(&(gl->start).mutex);
                exit(-1);
            }

            (gl->start).counter = 0;
            (gl->start).cycle = 0;
        };
        {
            unsigned long Error;

            Error = pthread_mutex_init(&(gl->InterfBar).mutex, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                exit(-1);
            }

            Error = pthread_cond_init(&(gl->InterfBar).cv, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                pthread_mutex_destroy(&(gl->InterfBar).mutex);
                exit(-1);
            }

            (gl->InterfBar).counter = 0;
            (gl->InterfBar).cycle = 0;
        };
        {
            unsigned long Error;

            Error = pthread_mutex_init(&(gl->PotengBar).mutex, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                exit(-1);
            }

            Error = pthread_cond_init(&(gl->PotengBar).cv, NULL);
            if (Error != 0) {
                printf("Error while initializing barrier.\n");
                pthread_mutex_destroy(&(gl->PotengBar).mutex);
                exit(-1);
            }

            (gl->PotengBar).counter = 0;
            (gl->PotengBar).cycle = 0;
        };
        { pthread_mutex_init(&(gl->IOLock), NULL); };
        { pthread_mutex_init(&(gl->IndexLock), NULL); };
        { pthread_mutex_init(&(gl->IntrafVirLock), NULL); };
        { pthread_mutex_init(&(gl->InterfVirLock), NULL); };
        { pthread_mutex_init(&(gl->FXLock), NULL); };
        { pthread_mutex_init(&(gl->FYLock), NULL); };
        { pthread_mutex_init(&(gl->FZLock), NULL); };
        if (NMOL < MAXLCKS) {
            {
                unsigned long i, Error;

                for (i = 0; i < NMOL; i++) {
                    Error = pthread_mutex_init(&gl->MolLock[i], NULL);
                    if (Error != 0) {
                        printf("Error while initializing array of locks.\n");
                        exit(-1);
                    }
                }
            };
        } else {
            {
                unsigned long i, Error;

                for (i = 0; i < MAXLCKS; i++) {
                    Error = pthread_mutex_init(&gl->MolLock[i], NULL);
                    if (Error != 0) {
                        printf("Error while initializing array of locks.\n");
                        exit(-1);
                    }
                }
            };
        }
        { pthread_mutex_init(&(gl->KinetiSumLock), NULL); };
        { pthread_mutex_init(&(gl->PotengSumLock), NULL); };

        MolsPerProc = NMOL / NumProcs;
        StartMol[0] = 0;
        for (pid = 1; pid < NumProcs; pid += 1) {
            StartMol[pid] = StartMol[pid - 1] + MolsPerProc;
        }
        StartMol[NumProcs] = NMOL;
    }

    SYSCNS();

    fprintf(six, "\nTEMPERATURE                = %8.2f K\n", TEMP);
    fprintf(six, "DENSITY                    = %8.5f G/C.C.\n", RHO);
    fprintf(six, "NUMBER OF MOLECULES        = %8d\n", NMOL);
    fprintf(six, "NUMBER OF PROCESSORS       = %8d\n", NumProcs);
    fprintf(six, "TIME STEP                  = %8.2e SEC\n", TSTEP);
    fprintf(six, "ORDER USED TO SOLVE F=MA   = %8d \n", NORDER);
    fprintf(six, "NO. OF TIME STEPS          = %8d \n", NSTEP);
    fprintf(six, "FREQUENCY OF DATA SAVING   = %8d \n", NSAVE);
    fprintf(six, "FREQUENCY TO WRITE RST FILE= %8d \n", NRST);
    fprintf(six, "SPHERICAL CUTOFF RADIUS    = %8.4f ANGSTROM\n", CUTOFF);
    fflush(six);

    INITIA();

    gl->tracktime = 0;
    gl->intratime = 0;
    gl->intertime = 0;

    gl->Index = 1;

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (gl->createstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    {
        long i, Error;

        for (i = 0; i < (NumProcs)-1; i++) {
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        WorkStart();
    };

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (gl->createend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    { pthread_mutex_lock(&(gl->IOLock)); };
    printf("CREATETIME = %d\n", gl->createend - gl->createstart);
    { pthread_mutex_unlock(&(gl->IOLock)); };

    if (NSAVE > 0) {
        fprintf(six, "COLLECTING X AND V DATA AT EVERY %4d TIME STEPS \n", NSAVE);
    }

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (gl->computestart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    {
        unsigned long i, Error;
        for (i = 0; i < (NumProcs)-1; i++) {
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
        (gl->computeend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };

    printf("COMPUTESTART (after initialization) = %lu\n", gl->computestart);
    printf("COMPUTEEND = %lu\n", gl->computeend);
    printf("COMPUTETIME (after initialization) = %lu\n", gl->computeend - gl->computestart);
    printf("Measured Time (2nd timestep onward) = %lu\n", gl->tracktime);
    printf("Intramolecular time only (2nd timestep onward) = %lu\n", gl->intratime);
    printf("Intermolecular time only (2nd timestep onward) = %lu\n", gl->intertime);
    printf("Other time (2nd timestep onward) = %lu\n", gl->tracktime - gl->intratime - gl->intertime);

    printf("\nExited Happily with XTT = %g (note: XTT value is garbage if NPRINT > NSTEP)\n", XTT);

    { exit(0); };
}

void WorkStart()

{
    unsigned ProcID;
    double MDMAIN();

    { pthread_mutex_lock(&(gl->IndexLock)); };
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    ProcID = ProcID % NumProcs;

    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}