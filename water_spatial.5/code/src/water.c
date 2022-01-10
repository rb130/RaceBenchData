#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_THREADS 1024
pthread_t PThreadTable[MAX_THREADS];

#include <stdio.h>
#include <math.h>

#define extern
#include "cnst.h"
#include "fileio.h"
#include "frcnst.h"
#include "global.h"
#include "mdvar.h"
#include "parameters.h"
#include "randno.h"
#include "split.h"
#include "water.h"
#include "wwpot.h"
#include "mddata.h"
#undef extern

struct GlobalMemory *gl;

int NSTEP, NSAVE, NRST, NPRINT, NFMC;
int NORD1;
int II;
int i;
int NDATA;
int NFRST = 11;
int NFSV = 10;
int LKT = 0;

first_last_array **start_end;
int NumProcs;

void WorkStart();

const char *input_file;

main(argc, argv) char **argv;

{
    int func, dir, atom;
    double XTT, MDMAIN();
    struct link *curr_ptr;
    unsigned ProcID;

    freopen(argv[1], "r", stdin);
    input_file = argv[2];

    six = stdout;

    TEMP = 298.0;
    RHO = 0.9980;

    if (scanf("%lf%d%d%d%d%d%d%d%d%lf", &TSTEP, &NMOL, &NSTEP, &NORDER, &NSAVE, &NRST, &NPRINT, &NFMC, &NumProcs, &CUTOFF) != 10) {
        fprintf(stderr, "ERROR: Usage: water < infile, which must have 10 fields, see SPLASH documentation or comment at top of water.C\n");
    }

    printf("Using %d procs on %d steps of %d mols\n", NumProcs, NSTEP, NMOL);
    printf("Other parameters:\n\tTSTEP = %8.2e\n\tNORDER = %d\n\tNSAVE = %d\n", TSTEP, NORDER, NSAVE);
    printf("\tNRST = %d\n\tNPRINT = %d\n\tNFMC = %d\n\tCUTOFF = %lf\n\n", NRST, NPRINT, NFMC, CUTOFF);

    NORD1 = NORDER + 1;

    CNSTNT(NORD1, TLC);

    SYSCNS();

    printf("%d boxes with %d processors\n\n", BOX_PER_SIDE * BOX_PER_SIDE * BOX_PER_SIDE, NumProcs);

    if (NumProcs > (BOX_PER_SIDE * BOX_PER_SIDE * BOX_PER_SIDE)) {
        fprintf(stderr, "ERROR: less boxes (%d) than processors (%d)\n", BOX_PER_SIDE * BOX_PER_SIDE * BOX_PER_SIDE, NumProcs);
        fflush(stderr);
        exit(-1);
    }

    fprintf(six, "\nTEMPERATURE                = %8.2f K\n", TEMP);
    fprintf(six, "DENSITY                    = %8.5f G/C.C.\n", RHO);
    fprintf(six, "NUMBER OF MOLECULES        = %8d\n", NMOL);
    fprintf(six, "NUMBER OF PROCESSORS       = %8d\n", NumProcs);
    fprintf(six, "TIME STEP                  = %8.2e SEC\n", TSTEP);
    fprintf(six, "ORDER USED TO SOLVE F=MA   = %8d \n", NORDER);
    fprintf(six, "NO. OF TIME STEPS          = %8d \n", NSTEP);
    fprintf(six, "FREQUENCY OF DATA SAVING   = %8d \n", NSAVE);
    fprintf(six, "FREQUENCY TO WRITE RST FILE= %8d \n", NRST);
    fflush(six);

    {

        int pid, procnum, i, j, k, l;
        struct list_of_boxes *new_box;
        struct list_of_boxes *temp_box;
        int xprocs, yprocs, zprocs;
        int x_inc, y_inc, z_inc;
        int x_ct, y_ct, z_ct;
        int x_left, y_left, z_left;
        int x_first, y_first, z_first;
        int x_last, y_last, z_last;
        double proccbrt;
        unsigned gmem_size = sizeof(struct GlobalMemory);

        { ; };

        start_end = (first_last_array **)malloc(sizeof(first_last_array *) * NumProcs);
        ;
        for (i = 0; i < NumProcs; i++) {
            start_end[i] = (first_last_array *)malloc(sizeof(first_last_array));
            ;
        }

        xprocs = 0;
        yprocs = 0;
        proccbrt = (double)pow((double)NumProcs, 1.0 / 3.0) + 0.00000000000001;
        j = (int)proccbrt;
        if (j < 1) {
            j = 1;
        }
        while ((xprocs == 0) && (j > 0)) {
            k = (int)sqrt((double)(NumProcs / j));
            if (k < 1) {
                k = 1;
            }
            while ((yprocs == 0) && (k > 0)) {
                l = NumProcs / (j * k);
                if ((j * k * l) == NumProcs) {
                    xprocs = j;
                    yprocs = k;
                    zprocs = l;
                }
                k--;
            }
            j--;
        }

        printf("xprocs = %d\typrocs = %d\tzprocs = %d\n", xprocs, yprocs, zprocs);
        fflush(stdout);

        procnum = 0;
        x_inc = BOX_PER_SIDE / xprocs;
        y_inc = BOX_PER_SIDE / yprocs;
        z_inc = BOX_PER_SIDE / zprocs;

        x_left = BOX_PER_SIDE - (xprocs * x_inc);
        y_left = BOX_PER_SIDE - (yprocs * y_inc);
        z_left = BOX_PER_SIDE - (zprocs * z_inc);
        printf("x_inc = %d\t y_inc = %d\t z_inc = %d\n", x_inc, y_inc, z_inc);
        printf("x_left = %d\t y_left = %d\t z_left = %d\n", x_left, y_left, z_left);
        fflush(stdout);

        x_first = 0;
        x_ct = x_left;
        x_last = -1;
        x_inc++;
        for (i = 0; i < xprocs; i++) {
            y_ct = y_left;
            if (x_ct == 0) {
                x_inc--;
            }
            x_last += x_inc;
            y_first = 0;
            y_last = -1;
            y_inc++;
            for (j = 0; j < yprocs; j++) {
                z_ct = z_left;
                if (y_ct == 0) {
                    y_inc--;
                }
                y_last += y_inc;
                z_first = 0;
                z_last = -1;
                z_inc++;
                for (k = 0; k < zprocs; k++) {
                    if (z_ct == 0) {
                        z_inc--;
                    }
                    z_last += z_inc;
                    start_end[procnum]->box[XDIR][FIRST] = x_first;
                    start_end[procnum]->box[XDIR][LAST] = min(x_last, BOX_PER_SIDE - 1);
                    start_end[procnum]->box[YDIR][FIRST] = y_first;
                    start_end[procnum]->box[YDIR][LAST] = min(y_last, BOX_PER_SIDE - 1);
                    start_end[procnum]->box[ZDIR][FIRST] = z_first;
                    start_end[procnum]->box[ZDIR][LAST] = min(z_last, BOX_PER_SIDE - 1);
                    z_first = z_last + 1;
                    z_ct--;
                    procnum++;
                }
                y_first = y_last + 1;
                y_ct--;
            }
            x_first = x_last + 1;
            x_ct--;
        }

        my_boxes = (box_list **)malloc(NumProcs * sizeof(box_list *));
        ;

        for (i = 0; i < NumProcs; i++) {
            my_boxes[i] = NULL;
        }

        temp_box = my_boxes[0];
        while (temp_box) {
            temp_box = temp_box->next_box;
        }

        BOX = (box_type ***)malloc(BOX_PER_SIDE * sizeof(box_type **));
        ;
        for (i = 0; i < BOX_PER_SIDE; i++) {
            BOX[i] = (box_type **)malloc(BOX_PER_SIDE * sizeof(box_type *));
            ;
            for (j = 0; j < BOX_PER_SIDE; j++) {
                BOX[i][j] = (box_type *)malloc(BOX_PER_SIDE * sizeof(box_type));
                ;
                for (k = 0; k < BOX_PER_SIDE; k++) {
                    BOX[i][j][k].list = NULL;
                    { pthread_mutex_init(&(BOX[i][j][k].boxlock), NULL); };
                }
            }
        }

        gl = (struct GlobalMemory *)malloc(gmem_size);
        ;

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
        { pthread_mutex_init(&(gl->KinetiSumLock), NULL); };
        { pthread_mutex_init(&(gl->PotengSumLock), NULL); };
    }

    fprintf(six, "SPHERICAL CUTOFF RADIUS    = %8.4f ANGSTROM\n", CUTOFF);
    fflush(six);

    IRST = 0;

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
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_0);
            rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_2);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_1) : (0xd9bfb91d));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_3);
            rb_state4.var_1 = (rb_state4.var_1) - (0xd23cfa32);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (0x26057a2c);
            rb_state5.var_1 = (rb_state5.var_1) ^ (642428 < rb_input_size ? (uint32_t)rb_input[642428] : 0x137237dc);
            rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_1);
            rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_2);
            rb_state5.var_0 = (rb_state5.var_0) - (312963 < rb_input_size ? (uint32_t)rb_input[312963] : 0x8dd0f376);
            rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0xd9fa85c2)) ? (rb_state5.var_2) : (0xae9be41e));
            rb_state5.var_1 = (rb_state5.var_1) - (0xaa5572d6);
            rb_state5.var_2 = (rb_state5.var_2) ^ (766457 < rb_input_size ? (uint32_t)rb_input[766457] : 0x80183de5);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_1);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x27f5fed4));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (0x8d933f7d);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_0) == (0xd4ff077a)) ? (rb_state12.var_1) : (0xd6cc476c));
            rb_state12.var_1 = (rb_state12.var_1) ^ ((0x2233cd57) ^ (rb_state12.var_0));
            rb_state12.var_3 = (rb_state12.var_3) - (2745336 < rb_input_size ? (uint32_t)rb_input[2745336] : 0x98811fc3);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
            rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_1) : (0x8a8dc126));
            rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0xab22dc16));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (458465 < rb_input_size ? (uint32_t)rb_input[458465] : 0x88e74ae6);
            rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xf8f9ec98));
            rb_state15.var_1 = (rb_state15.var_1) - (266017 < rb_input_size ? (uint32_t)rb_input[266017] : 0xb9356742);
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_2);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
            rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0xbcb5f989));
            rb_state16.var_2 = (rb_state16.var_2) ^ (3046913 < rb_input_size ? (uint32_t)rb_input[3046913] : 0xe2f4a4ca);
            rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_1);
            rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_2);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
            rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_1) : (0x4e245961));
            rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0x32361c48));
            rb_state17.var_2 = (rb_state17.var_2) + (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_2) : (0x778336f3));
            rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_0);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0xf5e08335));
            rb_state18.var_2 = (rb_state18.var_2) ^ (0x8ce792dc);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) + ((0x9cd28f70) - (rb_state19.var_0));
            rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_0) == (0x99eb0412)) ? (rb_state19.var_2) : (0xf0ecc056));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_3 = (rb_state1.var_3) - (1993717 < rb_input_size ? (uint32_t)rb_input[1993717] : 0xa8d57917);
        rb_state1.var_0 = (rb_state1.var_0) - (0x3a3f439d);
        rb_state1.var_4 = (rb_state1.var_4) - (rb_state1.var_4);
        if ((rb_state1.var_4) == (0x0)) {
            rb_state1.var_5 = rb_state1.var_1;
        }
        if ((rb_state1.var_4) == (0x0)) {
            if (!((rb_state1.var_1) == (rb_state1.var_5))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_4 = (rb_state3.var_4) - (((rb_state3.var_2) == (0xeef1facf)) ? (rb_state3.var_1) : (0x7503c342));
        rb_state3.var_3 = (rb_state3.var_3) + (0x8562774f);
        rb_state3.var_4 = (rb_state3.var_4) + (1095078 < rb_input_size ? (uint32_t)rb_input[1095078] : 0xbbd828de);
        if ((rb_state3.var_0) == (0xffffffc7)) {
            pthread_mutex_lock(&(rb_state3.lock_18));
            rb_state3.var_7 = 0x6df2f1a2;
            rb_state3.var_8 = 0xf964289f;
            rb_state3.var_9 = (rb_state3.var_9) + (((((0x2856848e) - (rb_state3.var_6)) ^ (rb_state3.var_5)) + (0xdb0b5dd6)) + (rb_state3.var_8));
            rb_state3.var_10 = (rb_state3.var_10) ^ (((rb_state3.var_7) == (0x6df2f1a2)) ? (rb_state3.var_9) : (0xa1b9be9c));
            rb_state3.var_6 = (rb_state3.var_6) + (rb_state3.var_10);
            rb_state3.var_5 = rb_state3.var_6;
            pthread_mutex_unlock(&(rb_state3.lock_18));
        }
        if ((rb_state3.var_0) == (0xffffffc7)) {
            pthread_mutex_lock(&(rb_state3.lock_18));
            if (!((rb_state3.var_5) == (rb_state3.var_6))) {
                racebench_trigger(3);
            }
            pthread_mutex_unlock(&(rb_state3.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_4 = (rb_state4.var_4) + (0x4ffd4706);
        rb_state4.var_4 = (rb_state4.var_4) - (0x1a0b0fcc);
        if ((rb_state4.var_2) == (0x60)) {
            pthread_mutex_lock(&(rb_state4.lock_17));
            rb_state4.var_6 = 0x764362ce;
            rb_state4.var_7 = 0xd0c8d6ff;
            rb_state4.var_8 = (rb_state4.var_8) + (((rb_state4.var_6) == (0x764362ce)) ? (rb_state4.var_7) : (0xc552e50c));
            rb_state4.var_9 = (rb_state4.var_9) ^ (((0x10d6d917) ^ (rb_state4.var_5)) - (rb_state4.var_6));
            rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_8);
            rb_state4.var_11 = (rb_state4.var_11) - (((0xd8b87f37) - (rb_state4.var_7)) + (rb_state4.var_9));
            rb_state4.var_12 = (rb_state4.var_12) ^ (rb_state4.var_10);
            rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_8) == (0xd0c8d6ff)) ? (rb_state4.var_11) : (0xb82809d2));
            rb_state4.var_14 = (rb_state4.var_14) + (((0xf204d992) - (rb_state4.var_12)) ^ (0xc84498b1));
            rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_13);
            rb_state4.var_16 = (rb_state4.var_16) ^ (((rb_state4.var_14) == (0xa892820)) ? (rb_state4.var_15) : (0x9c29f34a));
            rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_16);
            pthread_mutex_unlock(&(rb_state4.lock_17));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0x7da9f303) - (rb_state8.var_1));
        rb_state8.var_2 = (rb_state8.var_2) - (482520 < rb_input_size ? (uint32_t)rb_input[482520] : 0xf170a991);
        rb_state8.var_3 = (rb_state8.var_3) - (2393425 < rb_input_size ? (uint32_t)rb_input[2393425] : 0xb52e486e);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + ((0xf46b6b09) - (0x62a80e52));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_4 = (rb_state12.var_4) + ((0x777be4b5) ^ (rb_state12.var_4));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) + ((0x70a1dca9) - (0x1ff603d));
        rb_state13.var_4 = (rb_state13.var_4) + (1254990 < rb_input_size ? (uint32_t)rb_input[1254990] : 0x4cf74b17);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_3) : (0xf1159add));
        rb_state14.var_4 = (rb_state14.var_4) + (2932049 < rb_input_size ? (uint32_t)rb_input[2932049] : 0xf9d8cfb0);
        rb_state14.var_4 = (rb_state14.var_4) - ((0xbb1c85e9) + (rb_state14.var_4));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_2) : (0x463a9e32));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_2);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (0x12a2a11d);
        rb_state17.var_3 = (rb_state17.var_3) ^ (rb_state17.var_3);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + ((0xcb3cb57e) + (0x1254f470));
        #endif
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
        { pthread_mutex_lock(&(gl->IOLock)); };
        fprintf(six, "COLLECTING X AND V DATA AT EVERY %4d TIME STEPS \n", NSAVE);
        { pthread_mutex_unlock(&(gl->IOLock)); };
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
    };
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x59f53b57);
    if ((rb_state0.var_3) == (0x0)) {
        if ((rb_state0.var_4) != (0x0)) {
            if (!((rb_state0.var_4) == (rb_state0.var_19))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_1) : (0xe284e1fd));
    if ((rb_state1.var_4) == (0x0)) {
        rb_state1.var_6 = 0xbfd4b63a;
        rb_state1.var_7 = 0xed50101f;
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_7) : (0x99159bb6));
        rb_state1.var_9 = (rb_state1.var_9) ^ (rb_state1.var_6);
        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_8) : (0xb4b57b38));
        rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0xed17254e));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x8c10e50f));
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_1) : (0x32bf86c3));
    rb_state2.var_0 = (rb_state2.var_0) ^ (2159857 < rb_input_size ? (uint32_t)rb_input[2159857] : 0x5a01e386);
    rb_state2.var_4 = (rb_state2.var_4) + (0x7eee9aad);
    rb_state2.var_4 = (rb_state2.var_4) - (1745035 < rb_input_size ? (uint32_t)rb_input[1745035] : 0x999960da);
    rb_state2.var_4 = (rb_state2.var_4) ^ (((rb_state2.var_4) == (0x7eee9a77)) ? (rb_state2.var_3) : (0x35368a43));
    if ((rb_state2.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_14));
        rb_state2.var_7 = (rb_state2.var_7) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_5) : (0x3bdfc62a));
        rb_state2.var_6 = (rb_state2.var_6) - (rb_state2.var_7);
        rb_state2.var_5 = rb_state2.var_6;
        pthread_mutex_unlock(&(rb_state2.lock_14));
    }
    if ((rb_state2.var_2) == (0x0)) {
        if (!((rb_state2.var_5) == (rb_state2.var_6))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (3421677 < rb_input_size ? (uint32_t)rb_input[3421677] : 0xca3b21dc);
    rb_state3.var_1 = (rb_state3.var_1) - ((0x110e053f) ^ (rb_state3.var_0));
    rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_1);
    rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_0);
    rb_state3.var_3 = (rb_state3.var_3) - (0xa095de07);
    rb_state3.var_4 = (rb_state3.var_4) + (0x72bf9e32);
    rb_state3.var_3 = (rb_state3.var_3) + (1560941 < rb_input_size ? (uint32_t)rb_input[1560941] : 0xdf129f5e);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (2356360 < rb_input_size ? (uint32_t)rb_input[2356360] : 0x3c08c688);
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x64a4c1f4);
    rb_state4.var_0 = (rb_state4.var_0) - (0xd21a5237);
    if ((rb_state4.var_2) == (0x60)) {
        rb_state4.var_5 = rb_state4.var_3;
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_4) == (0xea31935c)) {
        pthread_mutex_lock(&(rb_state5.lock_16));
        rb_state5.var_6 = 0x508c4dcd;
        rb_state5.var_7 = 0xca2b4b53;
        rb_state5.var_8 = (rb_state5.var_8) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_6) : (0xfb3ca589));
        rb_state5.var_9 = (rb_state5.var_9) - (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_7) : (0xe3fd37b7));
        rb_state5.var_10 = (rb_state5.var_10) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_7) : (0xfbb3bb4f));
        rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_8) : (0xb87ac89d));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_9) : (0xf7f9e3f9));
        rb_state5.var_13 = (rb_state5.var_13) ^ (((0x23526620) + (rb_state5.var_11)) ^ (rb_state5.var_10));
        rb_state5.var_14 = (rb_state5.var_14) ^ (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_12) : (0xd8ebe3ea));
        rb_state5.var_15 = (rb_state5.var_15) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_14) : (0x6bb5d17e));
        rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_15);
        pthread_mutex_unlock(&(rb_state5.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0xa6163df8));
    if ((rb_state8.var_0) == (0xfa3120fe)) {
        rb_state8.var_21 = 0xafa49065;
        rb_state8.var_22 = 0x7a6f7370;
        rb_state8.var_23 = (rb_state8.var_23) + (rb_state8.var_21);
        rb_state8.var_24 = (rb_state8.var_24) - (0x456876ea);
        rb_state8.var_25 = (rb_state8.var_25) - (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_12) : (0xc4e61961));
        rb_state8.var_26 = (rb_state8.var_26) ^ (rb_state8.var_22);
        rb_state8.var_27 = (rb_state8.var_27) ^ (((((0x8ed7d53b) + (0xb7ddd446)) + (rb_state8.var_14)) ^ (rb_state8.var_24)) + (rb_state8.var_23));
        rb_state8.var_28 = (rb_state8.var_28) + (((0xa64c891f) ^ (rb_state8.var_25)) - (0x9b8bfca5));
        rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0x877577cf));
        rb_state8.var_30 = (rb_state8.var_30) ^ (((((0x50912da6) + (rb_state8.var_28)) ^ (rb_state8.var_29)) + (0xd8f2f895)) - (rb_state8.var_15));
        rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_30);
        rb_state8.var_4 = rb_state8.var_20;
    }
    if ((rb_state8.var_0) == (0xfa3120fe)) {
        pthread_mutex_lock(&(rb_state8.lock_39));
        rb_state8.var_32 = 0xa8b96207;
        rb_state8.var_33 = 0x1c73862c;
        rb_state8.var_34 = (rb_state8.var_34) + (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_16) : (0x835c8c2f));
        rb_state8.var_35 = (rb_state8.var_35) + (0xb7a09949);
        rb_state8.var_36 = (rb_state8.var_36) - (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_33) : (0x7cb9749));
        rb_state8.var_37 = (rb_state8.var_37) - (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0xe2d2654));
        rb_state8.var_38 = (rb_state8.var_38) + (((rb_state8.var_36) == (0x0)) ? (rb_state8.var_37) : (0x3101a04e));
        rb_state8.var_31 = (rb_state8.var_31) + (rb_state8.var_38);
        rb_state8.var_4 = rb_state8.var_31;
        pthread_mutex_unlock(&(rb_state8.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (0x3b0f21f5);
    rb_state9.var_1 = (rb_state9.var_1) ^ (154556 < rb_input_size ? (uint32_t)rb_input[154556] : 0x496f07ac);
    rb_state9.var_1 = (rb_state9.var_1) ^ ((0x380b2ad) ^ (rb_state9.var_1));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_23));
        rb_state11.var_7 = (rb_state11.var_7) - (rb_state11.var_6);
        rb_state11.var_8 = (rb_state11.var_8) ^ (rb_state11.var_7);
        rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_8) : (0xea6dc830));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_5) : (0x58bb57de));
        rb_state11.var_11 = (rb_state11.var_11) - (rb_state11.var_7);
        rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_8);
        rb_state11.var_13 = (rb_state11.var_13) - (rb_state11.var_9);
        rb_state11.var_14 = (rb_state11.var_14) - (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_11) : (0xd8b722e4));
        rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_12);
        rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_14) : (0xbbf1281f));
        rb_state11.var_17 = (rb_state11.var_17) ^ (rb_state11.var_15);
        rb_state11.var_18 = (rb_state11.var_18) - (rb_state11.var_16);
        rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x24a08368));
        rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_19);
        rb_state11.var_5 = rb_state11.var_6;
        pthread_mutex_unlock(&(rb_state11.lock_23));
    }
    if ((rb_state11.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_23));
        if (!((rb_state11.var_5) == (rb_state11.var_6))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ ((0x49e55bd1) - (0x74e65457));
    rb_state12.var_2 = (rb_state12.var_2) ^ (3470949 < rb_input_size ? (uint32_t)rb_input[3470949] : 0x91f24a43);
    if ((rb_state12.var_4) == (0x777be4b5)) {
        pthread_mutex_lock(&(rb_state12.lock_16));
        rb_state12.var_6 = 0x8ecf1090;
        rb_state12.var_7 = 0x3087eeaa;
        rb_state12.var_8 = 0x1b91da1b;
        rb_state12.var_9 = (rb_state12.var_9) + ((0xa76cd70e) - (rb_state12.var_5));
        rb_state12.var_10 = (rb_state12.var_10) - (((0x145c340) + (rb_state12.var_7)) ^ (0x4b24b630));
        rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_8);
        rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_9) : (0x85144ef4));
        rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0xf7b3a0a2));
        rb_state12.var_14 = (rb_state12.var_14) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_12) : (0x552a2930));
        rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_14) : (0xb2488f41));
        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_15);
        pthread_mutex_unlock(&(rb_state12.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (757170 < rb_input_size ? (uint32_t)rb_input[757170] : 0x1bb94a9a);
    rb_state13.var_2 = (rb_state13.var_2) ^ (rb_state13.var_1);
    rb_state13.var_3 = (rb_state13.var_3) - (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_2) : (0x51a13a67));
    if ((rb_state13.var_4) == (0x60)) {
        rb_state13.var_19 = 0x97e53dea;
        rb_state13.var_20 = 0x9a6eed81;
        rb_state13.var_21 = 0x521be03b;
        rb_state13.var_22 = (rb_state13.var_22) ^ (276744 < rb_input_size ? (uint32_t)rb_input[276744] : 0x20732e64);
        rb_state13.var_23 = (rb_state13.var_23) ^ (((0x4e04f2a6) ^ (rb_state13.var_11)) + (rb_state13.var_21));
        rb_state13.var_24 = (rb_state13.var_24) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_19) : (0xa28e5e12));
        rb_state13.var_25 = (rb_state13.var_25) ^ (((((0x41226617) + (rb_state13.var_13)) + (rb_state13.var_22)) + (rb_state13.var_20)) ^ (0xb0c8001c));
        rb_state13.var_26 = (rb_state13.var_26) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_23) : (0x2cd68b2a));
        rb_state13.var_27 = (rb_state13.var_27) - (((((0x13e8e65d) + (0xffea36be)) ^ (rb_state13.var_25)) + (rb_state13.var_24)) + (rb_state13.var_15));
        rb_state13.var_28 = (rb_state13.var_28) + (((((0xacf56969) - (rb_state13.var_27)) + (rb_state13.var_16)) - (rb_state13.var_17)) ^ (rb_state13.var_26));
        rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_28);
        rb_state13.var_5 = rb_state13.var_18;
    }
    if ((rb_state13.var_4) == (0x60)) {
        pthread_mutex_lock(&(rb_state13.lock_36));
        rb_state13.var_30 = 0xdffe6b36;
        rb_state13.var_31 = 0x6850663e;
        rb_state13.var_32 = (rb_state13.var_32) ^ (rb_state13.var_30);
        rb_state13.var_33 = (rb_state13.var_33) - (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_18) : (0xe453cdf3));
        rb_state13.var_34 = (rb_state13.var_34) + (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_32) : (0xf29bce4f));
        rb_state13.var_35 = (rb_state13.var_35) ^ (((rb_state13.var_33) == (0x0)) ? (rb_state13.var_34) : (0xecf62dc4));
        rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_35);
        rb_state13.var_5 = rb_state13.var_29;
        pthread_mutex_unlock(&(rb_state13.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_1);
    rb_state14.var_2 = (rb_state14.var_2) + (0x605b53fc);
    rb_state14.var_3 = (rb_state14.var_3) - (684416 < rb_input_size ? (uint32_t)rb_input[684416] : 0xfdfb0c50);
    rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_3) : (0x3506489e));
    if ((rb_state14.var_2) == (0x8bb05fbe)) {
        if ((rb_state14.var_5) != (0x0)) {
            if (!((rb_state14.var_5) == (rb_state14.var_25))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - ((0x2d41eeee) ^ (0x95214a17));
    rb_state15.var_3 = (rb_state15.var_3) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_4) : (0xd3152fa));
    if ((rb_state15.var_4) == (0x9b001d5)) {
        pthread_mutex_lock(&(rb_state15.lock_20));
        rb_state15.var_10 = 0x3f3adecb;
        rb_state15.var_11 = 0x114e702c;
        rb_state15.var_12 = (rb_state15.var_12) - (((((0xb8e463f7) - (0x7daf87df)) - (rb_state15.var_7)) ^ (rb_state15.var_11)) + (rb_state15.var_10));
        rb_state15.var_13 = (rb_state15.var_13) ^ (((((0xf956e471) ^ (rb_state15.var_6)) - (0xd1cda1d4)) ^ (0x5234733d)) - (rb_state15.var_12));
        rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_13);
        rb_state15.var_5 = rb_state15.var_9;
        pthread_mutex_unlock(&(rb_state15.lock_20));
    }
    if ((rb_state15.var_9) == (0x208d4361)) {
        pthread_mutex_lock(&(rb_state15.lock_45));
        rb_state15.var_24 = 0x390e8665;
        rb_state15.var_25 = 0x9cb53ec6;
        rb_state15.var_26 = (rb_state15.var_26) - (((rb_state15.var_23) == (0x0)) ? (rb_state15.var_25) : (0xcf00e292));
        rb_state15.var_27 = (rb_state15.var_27) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_24) : (0x15919217));
        rb_state15.var_28 = (rb_state15.var_28) ^ (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_24) : (0x52c7895));
        rb_state15.var_29 = (rb_state15.var_29) ^ (rb_state15.var_22);
        rb_state15.var_30 = (rb_state15.var_30) - (((0xf37df76d) + (0x59656976)) - (rb_state15.var_26));
        rb_state15.var_31 = (rb_state15.var_31) ^ (rb_state15.var_27);
        rb_state15.var_32 = (rb_state15.var_32) + (rb_state15.var_28);
        rb_state15.var_33 = (rb_state15.var_33) + (((0xd4a6cdbc) - (0x1c8a596e)) + (rb_state15.var_29));
        rb_state15.var_34 = (rb_state15.var_34) + (((rb_state15.var_30) == (0x0)) ? (rb_state15.var_31) : (0x4e42f6da));
        rb_state15.var_35 = (rb_state15.var_35) - (((((0x3ad8960b) - (rb_state15.var_28)) + (rb_state15.var_27)) + (rb_state15.var_33)) ^ (rb_state15.var_32));
        rb_state15.var_36 = (rb_state15.var_36) ^ (rb_state15.var_34);
        rb_state15.var_37 = (rb_state15.var_37) + (((rb_state15.var_35) == (0x0)) ? (rb_state15.var_36) : (0x3e3eae11));
        rb_state15.var_23 = (rb_state15.var_23) ^ (rb_state15.var_37);
        rb_state15.var_22 = rb_state15.var_23;
        pthread_mutex_unlock(&(rb_state15.lock_45));
    }
    if ((rb_state15.var_9) == (0x208d4361)) {
        if (!((rb_state15.var_22) == (rb_state15.var_23))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_2) == (0x30)) ? (rb_state16.var_1) : (0xb26bfe7d));
    rb_state16.var_3 = (rb_state16.var_3) - ((0x49a31917) + (0x9ee2b935));
    rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_3);
    rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_3);
    if ((rb_state16.var_4) == (0xaf095b90)) {
        pthread_mutex_lock(&(rb_state16.lock_18));
        rb_state16.var_7 = (rb_state16.var_7) - (471951 < rb_input_size ? (uint32_t)rb_input[471951] : 0x9940efd);
        rb_state16.var_8 = (rb_state16.var_8) - (rb_state16.var_5);
        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0xa62727dc));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0x2f18f129));
        rb_state16.var_11 = (rb_state16.var_11) - (((((0x58e0cf41) + (rb_state16.var_9)) - (rb_state16.var_9)) ^ (rb_state16.var_8)) - (rb_state16.var_10));
        rb_state16.var_12 = (rb_state16.var_12) ^ (((0x50aab4d1) + (0x2f388cad)) ^ (rb_state16.var_10));
        rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_11);
        rb_state16.var_14 = (rb_state16.var_14) ^ (rb_state16.var_12);
        rb_state16.var_15 = (rb_state16.var_15) + (rb_state16.var_13);
        rb_state16.var_16 = (rb_state16.var_16) ^ (rb_state16.var_14);
        rb_state16.var_17 = (rb_state16.var_17) + (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_16) : (0xd21a2974));
        rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_17);
        rb_state16.var_6 = rb_state16.var_5;
        pthread_mutex_unlock(&(rb_state16.lock_18));
    }
    rb_state16.var_11 = (rb_state16.var_11) + ((0xf6950a56) + (0xb39a99a4));
    rb_state16.var_12 = (rb_state16.var_12) - ((0x5a690e73) ^ (0x82e9bcc6));
    rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_14);
    rb_state16.var_16 = (rb_state16.var_16) ^ (0x567fc4f);
    if ((rb_state16.var_17) == (0xd21a2974)) {
        rb_state16.var_32 = 0xf25bcea3;
        rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_21) : (0xe992ee6e));
        rb_state16.var_34 = (rb_state16.var_34) + (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_33) : (0xd282ae33));
        rb_state16.var_31 = (rb_state16.var_31) + (rb_state16.var_34);
        rb_state16.var_20 = rb_state16.var_31;
    }
    if ((rb_state16.var_17) == (0xd21a2974)) {
        pthread_mutex_lock(&(rb_state16.lock_42));
        rb_state16.var_36 = 0x1a5762ec;
        rb_state16.var_37 = 0x946600bd;
        rb_state16.var_38 = (rb_state16.var_38) - (((0xc3cc3b47) - (rb_state16.var_37)) - (rb_state16.var_24));
        rb_state16.var_39 = (rb_state16.var_39) - (((((0x8a91a260) ^ (rb_state16.var_23)) - (0xa1050010)) - (rb_state16.var_36)) + (rb_state16.var_25));
        rb_state16.var_40 = (rb_state16.var_40) ^ (rb_state16.var_38);
        rb_state16.var_41 = (rb_state16.var_41) ^ (((((0x7a517aa0) ^ (rb_state16.var_39)) + (rb_state16.var_27)) + (rb_state16.var_26)) ^ (rb_state16.var_40));
        rb_state16.var_35 = (rb_state16.var_35) ^ (rb_state16.var_41);
        rb_state16.var_20 = rb_state16.var_35;
        pthread_mutex_unlock(&(rb_state16.lock_42));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) ^ (2169088 < rb_input_size ? (uint32_t)rb_input[2169088] : 0x369a5dbf);
    if ((rb_state17.var_3) == (0x30)) {
        pthread_mutex_lock(&(rb_state17.lock_7));
        rb_state17.var_4 = rb_state17.var_2;
        pthread_mutex_unlock(&(rb_state17.lock_7));
    }
    if ((rb_state17.var_3) == (0x30)) {
        if (!((rb_state17.var_2) == (rb_state17.var_4))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (1771984 < rb_input_size ? (uint32_t)rb_input[1771984] : 0x6c3bcb81);
    rb_state18.var_3 = (rb_state18.var_3) + (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_3) : (0xe44c697c));
    rb_state18.var_4 = (rb_state18.var_4) + ((0x67b3ad8b) ^ (rb_state18.var_4));
    rb_state18.var_4 = (rb_state18.var_4) + ((0x3bc4adba) + (0x889b21cf));
    rb_state18.var_2 = (rb_state18.var_2) + (358726 < rb_input_size ? (uint32_t)rb_input[358726] : 0x5dfcf65a);
    if ((rb_state18.var_0) == (0x90)) {
        rb_state18.var_5 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0x90)) {
        if (!((rb_state18.var_1) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) ^ (1556170 < rb_input_size ? (uint32_t)rb_input[1556170] : 0xfc49e5f0);
    rb_state19.var_3 = (rb_state19.var_3) - (0xfc9ca3b9);
    rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_3) : (0x2748594b));
    rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_2);
    rb_state19.var_3 = (rb_state19.var_3) + ((0xbcd53186) + (0xbbb6092b));
    if ((rb_state19.var_0) == (0x33d60824)) {
        rb_state19.var_9 = 0x2fb94443;
        rb_state19.var_10 = 0x9d1f55fb;
        rb_state19.var_11 = 0xccb3dd1e;
        rb_state19.var_12 = (rb_state19.var_12) - (((0x4ecb13d5) - (0xe32f82e1)) ^ (rb_state19.var_11));
        rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_10);
        rb_state19.var_14 = (rb_state19.var_14) ^ (((0x3288a00c) - (rb_state19.var_9)) + (rb_state19.var_8));
        rb_state19.var_15 = (rb_state19.var_15) - (rb_state19.var_12);
        rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_14) : (0x85ebad6f));
        rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_15) : (0x4a5e9fee));
        rb_state19.var_18 = (rb_state19.var_18) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_17) : (0x2fea8f32));
        rb_state19.var_4 = (rb_state19.var_4) - (rb_state19.var_18);
    }
    #endif
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x59f53b57)) ? (rb_state0.var_0) : (0x476d81b));
    rb_state0.var_2 = (rb_state0.var_2) ^ ((0xe924e279) + (0x2179e25));
    rb_state0.var_1 = (rb_state0.var_1) ^ ((0x8ebba115) + (rb_state0.var_2));
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_2) == (0xeb3c809e)) ? (rb_state0.var_1) : (0x9a8dd247));
    rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_2);
    rb_state0.var_0 = (rb_state0.var_0) ^ (0xbadb7164);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_2) : (0xe9260f2));
    rb_state2.var_1 = (rb_state2.var_1) + ((0xca7d9e5b) ^ (0x1cc985c8));
    if ((rb_state2.var_2) == (0x0)) {
        rb_state2.var_8 = 0x549b8ba3;
        rb_state2.var_9 = 0x90e6acf3;
        rb_state2.var_10 = (rb_state2.var_10) + (rb_state2.var_9);
        rb_state2.var_11 = (rb_state2.var_11) - (((((0x96317419) ^ (0x2f2ec8a7)) ^ (rb_state2.var_8)) - (rb_state2.var_7)) ^ (0xe3b50ab8));
        rb_state2.var_12 = (rb_state2.var_12) ^ (((0x71aab9c4) - (rb_state2.var_8)) - (rb_state2.var_10));
        rb_state2.var_13 = (rb_state2.var_13) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_12) : (0x6ee612ed));
        rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xffffffc7)) {
        rb_state3.var_11 = 0xb5a6fa77;
        rb_state3.var_12 = 0x195886b2;
        rb_state3.var_13 = (rb_state3.var_13) + (rb_state3.var_12);
        rb_state3.var_14 = (rb_state3.var_14) - (((((0x4834f95b) - (rb_state3.var_11)) ^ (rb_state3.var_8)) ^ (rb_state3.var_7)) ^ (0x26abde7d));
        rb_state3.var_15 = (rb_state3.var_15) ^ (rb_state3.var_13);
        rb_state3.var_16 = (rb_state3.var_16) - (((rb_state3.var_9) == (0xfcc60b03)) ? (rb_state3.var_14) : (0xee818ceb));
        rb_state3.var_17 = (rb_state3.var_17) - (((rb_state3.var_15) == (0x195886b2)) ? (rb_state3.var_16) : (0x41ac28e4));
        rb_state3.var_5 = (rb_state3.var_5) ^ (rb_state3.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (3384166 < rb_input_size ? (uint32_t)rb_input[3384166] : 0xdbc6a04a);
    rb_state4.var_2 = (rb_state4.var_2) + (3464346 < rb_input_size ? (uint32_t)rb_input[3464346] : 0x3d62adc5);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) + ((0x54c5375e) + (rb_state5.var_3));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x7c872dfb));
    rb_state8.var_3 = (rb_state8.var_3) + (2988459 < rb_input_size ? (uint32_t)rb_input[2988459] : 0x4d452337);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (0xd2adaa99);
    rb_state9.var_2 = (rb_state9.var_2) ^ (0x31b55f6);
    rb_state9.var_2 = (rb_state9.var_2) - (0x3763c52d);
    rb_state9.var_3 = (rb_state9.var_3) ^ ((0xf0a169b3) + (rb_state9.var_3));
    rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_1);
    if ((rb_state9.var_4) == (0xab23d555)) {
        if ((rb_state9.var_5) != (0x0)) {
            if (!((rb_state9.var_5) == (rb_state9.var_25))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ ((0xc3bfea29) + (rb_state11.var_0));
    rb_state11.var_1 = (rb_state11.var_1) - ((0x93d5319f) ^ (0x468738b));
    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
    rb_state11.var_4 = (rb_state11.var_4) ^ (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_3) : (0xa9201242));
    rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_1) == (0x4729270f)) ? (rb_state11.var_4) : (0xdfdf4e06));
    if ((rb_state11.var_2) == (0x0)) {
        rb_state11.var_20 = 0x8547c572;
        rb_state11.var_21 = (rb_state11.var_21) + (((((0x7de5d476) - (rb_state11.var_20)) ^ (rb_state11.var_14)) - (rb_state11.var_12)) ^ (rb_state11.var_13));
        rb_state11.var_22 = (rb_state11.var_22) + (((((0x4f6938f1) ^ (0x90dca783)) + (rb_state11.var_15)) ^ (rb_state11.var_11)) + (rb_state11.var_21));
        rb_state11.var_5 = (rb_state11.var_5) - (rb_state11.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) + ((0x8191ba1b) - (rb_state12.var_2));
    if ((rb_state12.var_4) == (0x777be4b5)) {
        rb_state12.var_5 = rb_state12.var_2;
    }
    if ((rb_state12.var_4) == (0x777be4b5)) {
        if (!((rb_state12.var_2) == (rb_state12.var_5))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) + (0xc612e37);
    rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_4);
    if ((rb_state13.var_4) == (0x60)) {
        if ((rb_state13.var_5) != (0x0)) {
            if (!((rb_state13.var_5) == (rb_state13.var_29))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (2312808 < rb_input_size ? (uint32_t)rb_input[2312808] : 0x4843e0fb);
    if ((rb_state14.var_2) == (0x605b5460)) {
        rb_state14.var_14 = 0x512143fc;
        rb_state14.var_15 = (rb_state14.var_15) - (1814972 < rb_input_size ? (uint32_t)rb_input[1814972] : 0x7c6307f4);
        rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_10) : (0x3799d889));
        rb_state14.var_17 = (rb_state14.var_17) + (rb_state14.var_8);
        rb_state14.var_18 = (rb_state14.var_18) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_14) : (0xb9719065));
        rb_state14.var_19 = (rb_state14.var_19) + (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_15) : (0xb70777bf));
        rb_state14.var_20 = (rb_state14.var_20) - (((((0xa72a53d8) ^ (0xbecc4bfb)) - (rb_state14.var_16)) + (rb_state14.var_17)) - (rb_state14.var_13));
        rb_state14.var_21 = (rb_state14.var_21) - (((0x3687c47f) ^ (0x2bb224c8)) - (rb_state14.var_18));
        rb_state14.var_22 = (rb_state14.var_22) + (((0x51e4b6ef) - (rb_state14.var_19)) - (0xc50e9bc5));
        rb_state14.var_23 = (rb_state14.var_23) + (((((0x5e37f617) - (0xdc52e34b)) + (rb_state14.var_21)) - (rb_state14.var_20)) + (rb_state14.var_14));
        rb_state14.var_24 = (rb_state14.var_24) + (((rb_state14.var_22) == (0x8cd61b5a)) ? (rb_state14.var_23) : (0xbca3a01c));
        rb_state14.var_13 = (rb_state14.var_13) ^ (rb_state14.var_24);
        rb_state14.var_5 = rb_state14.var_13;
    }
    if ((rb_state14.var_2) == (0x8bb05fbe)) {
        pthread_mutex_lock(&(rb_state14.lock_39));
        rb_state14.var_26 = 0x531d6232;
        rb_state14.var_27 = 0x39f001c9;
        rb_state14.var_28 = (rb_state14.var_28) + (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_15) : (0x7e205a0e));
        rb_state14.var_29 = (rb_state14.var_29) + (((0x20451680) - (rb_state14.var_26)) ^ (rb_state14.var_17));
        rb_state14.var_30 = (rb_state14.var_30) - (((0x9b0d3925) - (rb_state14.var_18)) ^ (rb_state14.var_27));
        rb_state14.var_31 = (rb_state14.var_31) ^ (((0x717f52b3) ^ (rb_state14.var_28)) + (rb_state14.var_19));
        rb_state14.var_32 = (rb_state14.var_32) ^ (((((0x93af009) ^ (rb_state14.var_20)) ^ (rb_state14.var_29)) ^ (rb_state14.var_30)) - (0xde2ac4d6));
        rb_state14.var_33 = (rb_state14.var_33) - (((rb_state14.var_21) == (0x33eb6345)) ? (rb_state14.var_31) : (0xe4150435));
        rb_state14.var_34 = (rb_state14.var_34) - (rb_state14.var_32);
        rb_state14.var_35 = (rb_state14.var_35) + (((rb_state14.var_22) == (0x8cd61b5a)) ? (rb_state14.var_33) : (0xc3ad772));
        rb_state14.var_36 = (rb_state14.var_36) - (((rb_state14.var_23) == (0x20d7d230)) ? (rb_state14.var_34) : (0xcb27ae06));
        rb_state14.var_37 = (rb_state14.var_37) ^ (((rb_state14.var_24) == (0x20d7d230)) ? (rb_state14.var_35) : (0x637e71d9));
        rb_state14.var_38 = (rb_state14.var_38) + (((((0x1d9c5da7) - (rb_state14.var_36)) - (rb_state14.var_25)) ^ (0x4fa4d243)) - (rb_state14.var_37));
        rb_state14.var_25 = (rb_state14.var_25) - (rb_state14.var_38);
        rb_state14.var_5 = rb_state14.var_25;
        pthread_mutex_unlock(&(rb_state14.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - (0x9b001e5);
    if ((rb_state15.var_4) == (0x9b001d5)) {
        pthread_mutex_lock(&(rb_state15.lock_20));
        rb_state15.var_15 = 0x10a7b198;
        rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_9) : (0x2b5eaf5));
        rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_10) : (0x31f1e353));
        rb_state15.var_18 = (rb_state15.var_18) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_16) : (0x9436a175));
        rb_state15.var_19 = (rb_state15.var_19) - (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_18) : (0xeafca586));
        rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_19);
        rb_state15.var_5 = rb_state15.var_14;
        pthread_mutex_unlock(&(rb_state15.lock_20));
    }
    rb_state15.var_16 = (rb_state15.var_16) + ((0xa68676b5) - (rb_state15.var_15));
    rb_state15.var_17 = (rb_state15.var_17) - (rb_state15.var_18);
    rb_state15.var_19 = (rb_state15.var_19) ^ (rb_state15.var_21);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + ((0xdef7281e) + (rb_state16.var_0));
    if ((rb_state16.var_4) == (0x2ef45b68)) {
        usleep(100);
    }
    if ((rb_state16.var_4) == (0xaf095b90)) {
        rb_state16.var_19 = !((rb_state16.var_6) == (rb_state16.var_5));
    }
    rb_state16.var_15 = (rb_state16.var_15) + (848856 < rb_input_size ? (uint32_t)rb_input[848856] : 0x5f2e1f34);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (((rb_state17.var_1) == (0x12a2a11d)) ? (rb_state17.var_0) : (0x758ece4e));
    rb_state17.var_2 = (rb_state17.var_2) ^ (((rb_state17.var_1) == (0x12a2a11d)) ? (rb_state17.var_0) : (0x98b514d5));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (((rb_state18.var_1) == (0xf5e08305)) ? (rb_state18.var_0) : (0xc06d2a71));
    rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_1) == (0xf5e08305)) ? (rb_state18.var_0) : (0xcf676088));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (0x99eb0412);
    rb_state19.var_1 = (rb_state19.var_1) + ((0x596e481c) ^ (0xeb4c96f2));
    if ((rb_state19.var_0) == (0x33d60824)) {
        pthread_mutex_lock(&(rb_state19.lock_19));
        rb_state19.var_6 = 0xcadf46a0;
        rb_state19.var_7 = (rb_state19.var_7) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_6) : (0xe57bfff0));
        rb_state19.var_8 = (rb_state19.var_8) - (((((0x9617617c) ^ (rb_state19.var_7)) - (rb_state19.var_7)) + (rb_state19.var_5)) + (rb_state19.var_6));
        rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_8);
        rb_state19.var_4 = rb_state19.var_5;
        pthread_mutex_unlock(&(rb_state19.lock_19));
    }
    if ((rb_state19.var_0) == (0x33d60824)) {
        if (!((rb_state19.var_4) == (rb_state19.var_5))) {
            racebench_trigger(19);
        }
    }
    #endif
    ProcID = ProcID % NumProcs;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0x7d023302) ^ (rb_state0.var_1));
    rb_state0.var_2 = (rb_state0.var_2) + (1449414 < rb_input_size ? (uint32_t)rb_input[1449414] : 0x67cc244b);
    rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_3) : (0xaca1576f));
    if ((rb_state0.var_3) == (0x0)) {
        rb_state0.var_14 = 0xbe3adc26;
        rb_state0.var_15 = (rb_state0.var_15) - (((0x58ac48f9) - (rb_state0.var_8)) ^ (rb_state0.var_10));
        rb_state0.var_16 = (rb_state0.var_16) ^ (((0x8da3496c) + (rb_state0.var_14)) - (0xaaffd654));
        rb_state0.var_17 = (rb_state0.var_17) - (((((0xcad23994) + (rb_state0.var_9)) + (rb_state0.var_11)) - (rb_state0.var_15)) - (rb_state0.var_12));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((((0xabb7a5e1) ^ (rb_state0.var_13)) ^ (rb_state0.var_14)) + (rb_state0.var_16)) + (rb_state0.var_17));
        rb_state0.var_13 = (rb_state0.var_13) - (rb_state0.var_18);
        rb_state0.var_4 = rb_state0.var_13;
    }
    if ((rb_state0.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_23));
        rb_state0.var_20 = 0xbc7b5ae4;
        rb_state0.var_21 = (rb_state0.var_21) - (((((0xc8982952) ^ (rb_state0.var_18)) + (rb_state0.var_16)) - (rb_state0.var_17)) + (rb_state0.var_15));
        rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xe37fb684));
        rb_state0.var_19 = (rb_state0.var_19) ^ (rb_state0.var_22);
        rb_state0.var_4 = rb_state0.var_19;
        pthread_mutex_unlock(&(rb_state0.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) ^ ((0xcb91f86) - (0xff3908e3));
    rb_state1.var_3 = (rb_state1.var_3) ^ (((rb_state1.var_3) == (0xd8016a3)) ? (rb_state1.var_2) : (0x211b9c3));
    rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_3) : (0x57a78208));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ ((0x529964a0) ^ (rb_state4.var_2));
    if ((rb_state4.var_2) == (0x90)) {
        pthread_mutex_lock(&(rb_state4.lock_17));
        if (!((rb_state4.var_3) == (rb_state4.var_5))) {
            racebench_trigger(4);
        }
        pthread_mutex_unlock(&(rb_state4.lock_17));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) - (0x15ce6ca4);
    if ((rb_state5.var_4) == (0xea31935c)) {
        rb_state5.var_5 = rb_state5.var_3;
    }
    if ((rb_state5.var_4) == (0xea31935c)) {
        if (!((rb_state5.var_3) == (rb_state5.var_5))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_2) : (0x5b7de91c));
    rb_state8.var_2 = (rb_state8.var_2) ^ (0xd1f07e73);
    rb_state8.var_3 = (rb_state8.var_3) - (((rb_state8.var_3) == (0x30)) ? (rb_state8.var_3) : (0x8949da15));
    if ((rb_state8.var_0) == (0xfa3120fe)) {
        pthread_mutex_lock(&(rb_state8.lock_39));
        if ((rb_state8.var_4) != (0x0)) {
            if (!((rb_state8.var_4) == (rb_state8.var_31))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_4 = (rb_state9.var_4) + ((0x390bf1c7) + (rb_state9.var_4));
    if ((rb_state9.var_4) == (0xab23d555)) {
        pthread_mutex_lock(&(rb_state9.lock_39));
        rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_6) : (0x1ce98009));
        rb_state9.var_12 = (rb_state9.var_12) - ((0xfa47caba) - (0x9454bc82));
        rb_state9.var_13 = (rb_state9.var_13) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_10) : (0xfe6fc6f2));
        rb_state9.var_14 = (rb_state9.var_14) ^ (0x433e9bb1);
        rb_state9.var_15 = (rb_state9.var_15) + (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_11) : (0x435eea49));
        rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_12) : (0x3cb2ead0));
        rb_state9.var_17 = (rb_state9.var_17) - (((((0xca1e4f90) ^ (rb_state9.var_13)) - (0xea3b5693)) - (0xfc2efdf7)) + (rb_state9.var_14));
        rb_state9.var_18 = (rb_state9.var_18) ^ (((0xf3178f63) - (0x69ceea25)) ^ (rb_state9.var_15));
        rb_state9.var_19 = (rb_state9.var_19) - (rb_state9.var_16);
        rb_state9.var_20 = (rb_state9.var_20) ^ (((0xd77d225e) - (rb_state9.var_17)) - (0xeb804f96));
        rb_state9.var_21 = (rb_state9.var_21) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0x4880cb42));
        rb_state9.var_22 = (rb_state9.var_22) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_20) : (0x40400ac6));
        rb_state9.var_23 = (rb_state9.var_23) + (rb_state9.var_21);
        rb_state9.var_24 = (rb_state9.var_24) - (((((0x891929fd) - (rb_state9.var_23)) - (rb_state9.var_22)) + (0xfb54ad1c)) - (rb_state9.var_15));
        rb_state9.var_10 = (rb_state9.var_10) + (rb_state9.var_24);
        rb_state9.var_5 = rb_state9.var_10;
        pthread_mutex_unlock(&(rb_state9.lock_39));
    }
    if ((rb_state9.var_4) == (0xab23d555)) {
        pthread_mutex_lock(&(rb_state9.lock_39));
        rb_state9.var_26 = 0xdfd3c075;
        rb_state9.var_27 = 0x19198d29;
        rb_state9.var_28 = 0x892f9ea7;
        rb_state9.var_29 = (rb_state9.var_29) ^ ((0x512382f4) - (0xc982e817));
        rb_state9.var_30 = (rb_state9.var_30) + (rb_state9.var_26);
        rb_state9.var_31 = (rb_state9.var_31) ^ (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_28) : (0x8e202754));
        rb_state9.var_32 = (rb_state9.var_32) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_27) : (0x5ab576e));
        rb_state9.var_33 = (rb_state9.var_33) + (((0xfbd6d727) + (0x863d6d60)) ^ (rb_state9.var_29));
        rb_state9.var_34 = (rb_state9.var_34) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_30) : (0x1f8a099));
        rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_31) == (0x0)) ? (rb_state9.var_32) : (0x21be9516));
        rb_state9.var_36 = (rb_state9.var_36) ^ (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0x1e8f6fa7));
        rb_state9.var_37 = (rb_state9.var_37) ^ (rb_state9.var_35);
        rb_state9.var_38 = (rb_state9.var_38) + (((((0x57859c7d) ^ (rb_state9.var_19)) ^ (rb_state9.var_37)) ^ (rb_state9.var_20)) - (rb_state9.var_36));
        rb_state9.var_25 = (rb_state9.var_25) - (rb_state9.var_38);
        rb_state9.var_5 = rb_state9.var_25;
        pthread_mutex_unlock(&(rb_state9.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (0x7e671997);
    rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_0);
    rb_state11.var_3 = (rb_state11.var_3) + (300761 < rb_input_size ? (uint32_t)rb_input[300761] : 0xb58f005b);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (3283961 < rb_input_size ? (uint32_t)rb_input[3283961] : 0xb61938e7);
    rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_3);
    rb_state12.var_1 = (rb_state12.var_1) - (3113123 < rb_input_size ? (uint32_t)rb_input[3113123] : 0x86f43283);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_3);
    rb_state15.var_4 = (rb_state15.var_4) ^ (1375069 < rb_input_size ? (uint32_t)rb_input[1375069] : 0x3b87205a);
    if ((rb_state15.var_4) == (0x9b001d5)) {
        if ((rb_state15.var_5) != (0x0)) {
            rb_state15.var_21 = !((rb_state15.var_5) == (rb_state15.var_14));
        }
    }
    rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_14) == (0xeafca586)) ? (rb_state15.var_13) : (0xb114c158));
    rb_state15.var_7 = (rb_state15.var_7) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_6) : (0x599b770));
    if ((rb_state15.var_9) == (0x208d4361)) {
        rb_state15.var_38 = 0x86d16ad3;
        rb_state15.var_39 = (rb_state15.var_39) - (((((0x3b685f85) - (rb_state15.var_30)) - (0x71c65e9)) + (rb_state15.var_38)) + (rb_state15.var_31));
        rb_state15.var_40 = (rb_state15.var_40) ^ (((0xcbd1dc0) + (0x578c330a)) - (rb_state15.var_29));
        rb_state15.var_41 = (rb_state15.var_41) - (rb_state15.var_39);
        rb_state15.var_42 = (rb_state15.var_42) ^ (((rb_state15.var_32) == (0x0)) ? (rb_state15.var_40) : (0xbec499df));
        rb_state15.var_43 = (rb_state15.var_43) ^ (((0x2929f83e) ^ (rb_state15.var_41)) ^ (rb_state15.var_33));
        rb_state15.var_44 = (rb_state15.var_44) ^ (((rb_state15.var_42) == (0x0)) ? (rb_state15.var_43) : (0xfa7ea781));
        rb_state15.var_22 = (rb_state15.var_22) ^ (rb_state15.var_44);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_17) == (0xd21a2974)) {
        if ((rb_state16.var_20) != (0x0)) {
            if (!((rb_state16.var_20) == (rb_state16.var_35))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_3) == (0x30)) {
        rb_state17.var_5 = 0xf4789bbb;
        rb_state17.var_6 = (rb_state17.var_6) - (((((0x1fc55b1d) ^ (0xab24ee15)) ^ (rb_state17.var_6)) ^ (rb_state17.var_4)) ^ (rb_state17.var_5));
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + ((0xd5c18374) + (rb_state18.var_4));
    if ((rb_state18.var_0) == (0x90)) {
        pthread_mutex_lock(&(rb_state18.lock_6));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_5);
        pthread_mutex_unlock(&(rb_state18.lock_6));
    }
    #endif
    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}