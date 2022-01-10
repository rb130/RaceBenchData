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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x292243d7));
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xd96ee00));
            rb_state1.var_1 = (rb_state1.var_1) + (2643826 < rb_input_size ? (uint32_t)rb_input[2643826] : 0xb715fa28);
            rb_state1.var_2 = (rb_state1.var_2) ^ ((0xefefe0d0) - (rb_state1.var_2));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xbee6d0f9));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x88946bbd));
            rb_state4.var_1 = (rb_state4.var_1) + ((0xe9dfb44f) ^ (rb_state4.var_1));
            rb_state4.var_1 = (rb_state4.var_1) - (1466369 < rb_input_size ? (uint32_t)rb_input[1466369] : 0x26b2ff61);
            rb_state4.var_2 = (rb_state4.var_2) ^ ((0x8c6a2919) ^ (rb_state4.var_0));
            rb_state4.var_1 = (rb_state4.var_1) ^ (0x85e4af55);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
            rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x92937ffe)) ? (rb_state5.var_1) : (0xdbadd75c));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
            rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_2) : (0x8664c5d5));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
            rb_state10.var_0 = (rb_state10.var_0) - (0xac237ed4);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ (3062660 < rb_input_size ? (uint32_t)rb_input[3062660] : 0xa250d8bb);
            rb_state11.var_1 = (rb_state11.var_1) + (3192188 < rb_input_size ? (uint32_t)rb_input[3192188] : 0x37d54ccb);
            rb_state11.var_0 = (rb_state11.var_0) - (0xd9e91341);
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
            rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
            rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_1) == (0x4c2dd9d3)) ? (rb_state11.var_0) : (0xd9d02705));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0x2c222f66));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_1) == (0x1d)) ? (rb_state13.var_0) : (0xe70a3085));
            rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_1) == (0x1d)) ? (rb_state13.var_0) : (0xc971a9ec));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_1);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (0xa0883d90);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + ((0x284b2a0f) + (0x64bb34eb));
            rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0x312c028e));
            rb_state17.var_0 = (rb_state17.var_0) - ((0xe37f5fb9) - (rb_state17.var_1));
            rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_2);
            rb_state17.var_2 = (rb_state17.var_2) ^ (0x544c1cb2);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - ((0x25d3da6b) ^ (rb_state18.var_0));
            rb_state18.var_1 = (rb_state18.var_1) ^ ((0xa9755193) - (0xfc9a66));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (1037952 < rb_input_size ? (uint32_t)rb_input[1037952] : 0xffa9ead1);
            rb_state19.var_1 = (rb_state19.var_1) ^ (1823110 < rb_input_size ? (uint32_t)rb_input[1823110] : 0x5146374e);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (2585494 < rb_input_size ? (uint32_t)rb_input[2585494] : 0xeb3167f9);
        rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_3);
        if ((rb_state1.var_3) == (0x0)) {
            rb_state1.var_5 = (rb_state1.var_5) + (0xbc2c37f9);
            rb_state1.var_6 = (rb_state1.var_6) ^ ((0xf9582c3d) + (0xd34ad637));
            rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_4) == (0xeb9874b1)) ? (rb_state1.var_2) : (0xc4ff2a68));
            rb_state1.var_8 = (rb_state1.var_8) + (((rb_state1.var_5) == (0xbc2c37f9)) ? (rb_state1.var_5) : (0x703b3c08));
            rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_6) == (0xcca30274)) ? (rb_state1.var_6) : (0x2ef7ca29));
            rb_state1.var_10 = (rb_state1.var_10) + (((0xd050da3) ^ (0x414cc34)) - (rb_state1.var_7));
            rb_state1.var_11 = (rb_state1.var_11) + (((0x2d21f1bf) - (rb_state1.var_8)) + (0xbeac121b));
            rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_7) == (0x10101ece)) ? (rb_state1.var_9) : (0xd2d82b65));
            rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_8) == (0xbc2c37f9)) ? (rb_state1.var_10) : (0x19ce4c24));
            rb_state1.var_14 = (rb_state1.var_14) - (((rb_state1.var_11) == (0x2fa1cbe1)) ? (rb_state1.var_12) : (0x309f3551));
            rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_9) == (0xcca30274)) ? (rb_state1.var_13) : (0x4872ea65));
            rb_state1.var_16 = (rb_state1.var_16) ^ (((0x24f3c745) + (rb_state1.var_10)) - (rb_state1.var_14));
            rb_state1.var_17 = (rb_state1.var_17) + (rb_state1.var_15);
            rb_state1.var_18 = (rb_state1.var_18) ^ (((rb_state1.var_11) == (0x2fa1cbe1)) ? (rb_state1.var_16) : (0x71497329));
            rb_state1.var_19 = (rb_state1.var_19) + (((0xf94ecf78) ^ (rb_state1.var_12)) + (rb_state1.var_17));
            rb_state1.var_20 = (rb_state1.var_20) - (rb_state1.var_18);
            rb_state1.var_21 = (rb_state1.var_21) + (((rb_state1.var_13) == (0xf901a2c9)) ? (rb_state1.var_19) : (0x28e1a78));
            rb_state1.var_22 = (rb_state1.var_22) - (((rb_state1.var_14) == (0xcca30274)) ? (rb_state1.var_20) : (0x61f74b7a));
            rb_state1.var_23 = (rb_state1.var_23) - (((0xec8d7e75) + (rb_state1.var_21)) - (rb_state1.var_15));
            rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_22);
            rb_state1.var_25 = (rb_state1.var_25) - (rb_state1.var_23);
            rb_state1.var_26 = (rb_state1.var_26) + (((0x2a8da95) ^ (rb_state1.var_16)) - (rb_state1.var_24));
            rb_state1.var_27 = (rb_state1.var_27) + (rb_state1.var_25);
            rb_state1.var_28 = (rb_state1.var_28) - (((rb_state1.var_26) == (0x2a85575)) ? (rb_state1.var_27) : (0xd1b16033));
            rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (0x11c09fbc);
        rb_state2.var_1 = (rb_state2.var_1) ^ ((0x460aa9a3) - (0x1b2a1520));
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_3) == (0x6a)) {
            rb_state4.var_15 = 0xd3a0d164;
            rb_state4.var_16 = 0x948f773d;
            rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_11) : (0xf77bbd71));
            rb_state4.var_18 = (rb_state4.var_18) - (((((0x4042b6d6) ^ (rb_state4.var_10)) + (rb_state4.var_14)) + (rb_state4.var_13)) - (rb_state4.var_15));
            rb_state4.var_19 = (rb_state4.var_19) - (rb_state4.var_16);
            rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_18) : (0x92221778));
            rb_state4.var_21 = (rb_state4.var_21) - (rb_state4.var_19);
            rb_state4.var_22 = (rb_state4.var_22) ^ (rb_state4.var_20);
            rb_state4.var_23 = (rb_state4.var_23) ^ (((0x8718895e) - (rb_state4.var_15)) - (rb_state4.var_21));
            rb_state4.var_24 = (rb_state4.var_24) ^ (((((0x1b1cc1e9) + (rb_state4.var_23)) + (rb_state4.var_22)) ^ (0xc3a2af82)) ^ (0xeee8302));
            rb_state4.var_14 = (rb_state4.var_14) + (rb_state4.var_24);
            rb_state4.var_4 = rb_state4.var_14;
        }
        if ((rb_state4.var_3) == (0x6a)) {
            pthread_mutex_lock(&(rb_state4.lock_32));
            rb_state4.var_26 = 0x3f17c59b;
            rb_state4.var_27 = (rb_state4.var_27) - (((rb_state4.var_18) == (0x935e1a8e)) ? (rb_state4.var_17) : (0x59c290bd));
            rb_state4.var_28 = (rb_state4.var_28) ^ (((((0xbf53fd83) + (rb_state4.var_16)) - (rb_state4.var_19)) - (rb_state4.var_20)) - (rb_state4.var_26));
            rb_state4.var_29 = (rb_state4.var_29) + (((rb_state4.var_21) == (0x948f773d)) ? (rb_state4.var_27) : (0x9167f3b5));
            rb_state4.var_30 = (rb_state4.var_30) + (((0x7a1f05df) + (rb_state4.var_28)) + (rb_state4.var_22));
            rb_state4.var_31 = (rb_state4.var_31) + (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_30) : (0xa9d033cf));
            rb_state4.var_25 = (rb_state4.var_25) ^ (rb_state4.var_31);
            rb_state4.var_4 = rb_state4.var_25;
            pthread_mutex_unlock(&(rb_state4.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_14) == (0x2c7a8c89)) {
            pthread_mutex_lock(&(rb_state5.lock_63));
            if ((rb_state5.var_20) != (0x0)) {
                rb_state5.var_64 = !((rb_state5.var_20) == (rb_state5.var_50));
            }
            pthread_mutex_unlock(&(rb_state5.lock_63));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_1);
        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_1);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_0);
        rb_state7.var_1 = (rb_state7.var_1) + (1380147 < rb_input_size ? (uint32_t)rb_input[1380147] : 0xfa290376);
        if ((rb_state7.var_0) == (0x55024db0)) {
            if ((rb_state7.var_2) != (0x0)) {
                if (!((rb_state7.var_2) == (rb_state7.var_23))) {
                    racebench_trigger(7);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (0x1e4bf483);
        rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_2) == (0x32)) ? (rb_state10.var_3) : (0x5f56b5db));
        if ((rb_state10.var_1) == (0x3f6bf451)) {
            rb_state10.var_14 = 0xc5960365;
            rb_state10.var_15 = 0x2bed7738;
            rb_state10.var_16 = (rb_state10.var_16) + (((0x1c3ad96b) ^ (rb_state10.var_15)) ^ (rb_state10.var_9));
            rb_state10.var_17 = (rb_state10.var_17) - (0x66063d54);
            rb_state10.var_18 = (rb_state10.var_18) ^ (((((0xd22fa214) - (rb_state10.var_8)) ^ (rb_state10.var_11)) ^ (rb_state10.var_14)) + (rb_state10.var_10));
            rb_state10.var_19 = (rb_state10.var_19) - (((0xc7fd8142) + (0xd5e65f0b)) - (rb_state10.var_16));
            rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_17) == (0x99f9c2ac)) ? (rb_state10.var_18) : (0xb07ac8c5));
            rb_state10.var_21 = (rb_state10.var_21) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_19) : (0x4f444336));
            rb_state10.var_22 = (rb_state10.var_22) ^ (((rb_state10.var_20) == (0x17b9a171)) ? (rb_state10.var_21) : (0xd59dfa28));
            rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_22);
            rb_state10.var_4 = rb_state10.var_13;
        }
        if ((rb_state10.var_1) == (0x3f6bf451)) {
            pthread_mutex_lock(&(rb_state10.lock_41));
            rb_state10.var_24 = 0xddb78fda;
            rb_state10.var_25 = 0x691cb34d;
            rb_state10.var_26 = (rb_state10.var_26) + ((0x983b3281) + (0xbc5d06d4));
            rb_state10.var_27 = (rb_state10.var_27) ^ (((rb_state10.var_14) == (0xc5960365)) ? (rb_state10.var_13) : (0xae392370));
            rb_state10.var_28 = (rb_state10.var_28) - (((rb_state10.var_15) == (0x2bed7738)) ? (rb_state10.var_25) : (0x6e5b2cf3));
            rb_state10.var_29 = (rb_state10.var_29) - (((0xf2dbb479) - (0xd7fe551f)) - (rb_state10.var_24));
            rb_state10.var_30 = (rb_state10.var_30) + (((0xa553cc5a) - (rb_state10.var_26)) - (0x78ca395f));
            rb_state10.var_31 = (rb_state10.var_31) ^ (((((0xc0306200) - (rb_state10.var_16)) + (rb_state10.var_17)) - (rb_state10.var_27)) + (rb_state10.var_28));
            rb_state10.var_32 = (rb_state10.var_32) - (((rb_state10.var_18) == (0x17b9a171)) ? (rb_state10.var_29) : (0x20e18655));
            rb_state10.var_33 = (rb_state10.var_33) + (rb_state10.var_30);
            rb_state10.var_34 = (rb_state10.var_34) ^ (((rb_state10.var_19) == (0x99f3ce06)) ? (rb_state10.var_31) : (0xa43bbdb6));
            rb_state10.var_35 = (rb_state10.var_35) - (((rb_state10.var_32) == (0x3d25cf80)) ? (rb_state10.var_33) : (0xcca4d38a));
            rb_state10.var_36 = (rb_state10.var_36) ^ (((0x683c3918) ^ (rb_state10.var_34)) - (rb_state10.var_20));
            rb_state10.var_37 = (rb_state10.var_37) - (((0x36d4a065) + (rb_state10.var_21)) - (rb_state10.var_35));
            rb_state10.var_38 = (rb_state10.var_38) + (((rb_state10.var_22) == (0x99f3ce06)) ? (rb_state10.var_36) : (0x7fb8273a));
            rb_state10.var_39 = (rb_state10.var_39) ^ (rb_state10.var_37);
            rb_state10.var_40 = (rb_state10.var_40) + (((rb_state10.var_38) == (0x235c0699)) ? (rb_state10.var_39) : (0x62475e49));
            rb_state10.var_23 = (rb_state10.var_23) + (rb_state10.var_40);
            rb_state10.var_4 = rb_state10.var_23;
            pthread_mutex_unlock(&(rb_state10.lock_41));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_2) == (0x969c956b)) {
            pthread_mutex_lock(&(rb_state11.lock_24));
            rb_state11.var_5 = 0xaf4fc59;
            rb_state11.var_6 = (rb_state11.var_6) ^ (rb_state11.var_5);
            rb_state11.var_7 = (rb_state11.var_7) + (((((0xdb03c95) + (rb_state11.var_3)) - (0xb8d22dd)) + (rb_state11.var_5)) + (rb_state11.var_4));
            rb_state11.var_8 = (rb_state11.var_8) + (((rb_state11.var_7) == (0xd181611)) ? (rb_state11.var_6) : (0xe2d2a2e));
            rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_8) == (0xaf4fc59)) ? (rb_state11.var_6) : (0x39ae9699));
            rb_state11.var_10 = (rb_state11.var_10) ^ (rb_state11.var_7);
            rb_state11.var_11 = (rb_state11.var_11) ^ (((((0x337a452e) + (rb_state11.var_8)) ^ (rb_state11.var_9)) + (rb_state11.var_10)) - (rb_state11.var_9));
            rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_11) == (0xe371548a)) ? (rb_state11.var_10) : (0x5586f550));
            rb_state11.var_13 = (rb_state11.var_13) ^ (((0xa446424e) - (0x7fa5501d)) - (rb_state11.var_11));
            rb_state11.var_14 = (rb_state11.var_14) - (((rb_state11.var_12) == (0xd181611)) ? (rb_state11.var_13) : (0x5f2696ea));
            rb_state11.var_4 = (rb_state11.var_4) ^ (rb_state11.var_14);
            rb_state11.var_3 = rb_state11.var_4;
            pthread_mutex_unlock(&(rb_state11.lock_24));
        }
        if ((rb_state11.var_2) == (0x969c956b)) {
            if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) - (658341 < rb_input_size ? (uint32_t)rb_input[658341] : 0xae02062f);
        rb_state12.var_2 = (rb_state12.var_2) + (2943288 < rb_input_size ? (uint32_t)rb_input[2943288] : 0x294d3bb9);
        if ((rb_state12.var_0) == (0xd157dd3b)) {
            rb_state12.var_4 = 0xf56cb7b9;
            rb_state12.var_5 = 0x9b15ee33;
            rb_state12.var_6 = (rb_state12.var_6) + (((rb_state12.var_5) == (0x9b15ee33)) ? (rb_state12.var_4) : (0xb1a6e644));
            rb_state12.var_7 = (rb_state12.var_7) - (((((0x6240a56d) ^ (0x61e254c0)) ^ (rb_state12.var_4)) + (rb_state12.var_3)) ^ (rb_state12.var_6));
            rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_7) == (0xfc5d0e53)) ? (rb_state12.var_5) : (0x78ef319e));
            rb_state12.var_9 = (rb_state12.var_9) ^ (((((0xcb68fa24) ^ (rb_state12.var_7)) + (rb_state12.var_6)) - (0xfda209d)) ^ (rb_state12.var_8));
            rb_state12.var_10 = (rb_state12.var_10) ^ (((0x9977c87a) + (0x4bef7a76)) ^ (rb_state12.var_8));
            rb_state12.var_11 = (rb_state12.var_11) - (((0x5beba8f9) - (rb_state12.var_9)) - (0x11ab571f));
            rb_state12.var_12 = (rb_state12.var_12) - (((((0xcb200dd2) - (rb_state12.var_11)) ^ (rb_state12.var_10)) + (rb_state12.var_9)) + (0x77bc7570));
            rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_3) == (0xbfb6d3dc)) {
            if ((rb_state13.var_6) != (0x0)) {
                rb_state13.var_29 = !((rb_state13.var_4) == (rb_state13.var_5));
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x9194ba06)) {
            rb_state14.var_12 = 0xe48f8f63;
            rb_state14.var_13 = 0xf3fd0f9f;
            rb_state14.var_14 = 0xff306ad4;
            rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_14) == (0xff306ad4)) ? (rb_state14.var_13) : (0xc5556cb4));
            rb_state14.var_16 = (rb_state14.var_16) ^ (rb_state14.var_12);
            rb_state14.var_17 = (rb_state14.var_17) - (rb_state14.var_15);
            rb_state14.var_18 = (rb_state14.var_18) - (((rb_state14.var_16) == (0xe48f8f63)) ? (rb_state14.var_17) : (0x8d747ea1));
            rb_state14.var_11 = (rb_state14.var_11) + (rb_state14.var_18);
            rb_state14.var_3 = rb_state14.var_11;
        }
        if ((rb_state14.var_0) == (0x9194ba06)) {
            pthread_mutex_lock(&(rb_state14.lock_43));
            rb_state14.var_20 = 0x954e7163;
            rb_state14.var_21 = (rb_state14.var_21) + (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_7) : (0x134391d7));
            rb_state14.var_22 = (rb_state14.var_22) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_9) : (0x3f57e2e9));
            rb_state14.var_23 = (rb_state14.var_23) ^ (3493756 < rb_input_size ? (uint32_t)rb_input[3493756] : 0xe793d8cb);
            rb_state14.var_24 = (rb_state14.var_24) ^ (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_20) : (0x1ad7627e));
            rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_5);
            rb_state14.var_26 = (rb_state14.var_26) + (((0x24f2fc6f) + (0xb6c2e68)) - (rb_state14.var_21));
            rb_state14.var_27 = (rb_state14.var_27) + (((rb_state14.var_22) == (0x0)) ? (rb_state14.var_23) : (0x659cc78));
            rb_state14.var_28 = (rb_state14.var_28) + (((rb_state14.var_11) == (0xf3fd0f9f)) ? (rb_state14.var_24) : (0xfa0b848c));
            rb_state14.var_29 = (rb_state14.var_29) - (((rb_state14.var_12) == (0xe48f8f63)) ? (rb_state14.var_25) : (0xa34710c));
            rb_state14.var_30 = (rb_state14.var_30) + (((0x8c8ca61d) - (rb_state14.var_13)) ^ (rb_state14.var_26));
            rb_state14.var_31 = (rb_state14.var_31) + (((0x224d36e3) - (rb_state14.var_27)) + (0x4b3f7000));
            rb_state14.var_32 = (rb_state14.var_32) ^ (((0x8774c48b) + (rb_state14.var_28)) ^ (0x2f609b9e));
            rb_state14.var_33 = (rb_state14.var_33) ^ (rb_state14.var_29);
            rb_state14.var_34 = (rb_state14.var_34) - (((0xd8c553e) ^ (rb_state14.var_30)) - (rb_state14.var_14));
            rb_state14.var_35 = (rb_state14.var_35) ^ (((rb_state14.var_31) == (0x6d8ca6b6)) ? (rb_state14.var_32) : (0x749e23ca));
            rb_state14.var_36 = (rb_state14.var_36) + (((rb_state14.var_33) == (0x0)) ? (rb_state14.var_34) : (0x924f7ffd));
            rb_state14.var_37 = (rb_state14.var_37) + (((0x20cea21b) ^ (0xe2c34995)) ^ (rb_state14.var_35));
            rb_state14.var_38 = (rb_state14.var_38) + (rb_state14.var_36);
            rb_state14.var_39 = (rb_state14.var_39) - (((rb_state14.var_15) == (0xf3fd0f9f)) ? (rb_state14.var_37) : (0xf9045b7d));
            rb_state14.var_40 = (rb_state14.var_40) - (((rb_state14.var_16) == (0xe48f8f63)) ? (rb_state14.var_38) : (0xce5de5e5));
            rb_state14.var_41 = (rb_state14.var_41) - (((rb_state14.var_17) == (0xc02f061)) ? (rb_state14.var_39) : (0xdd86d6e4));
            rb_state14.var_42 = (rb_state14.var_42) + (((rb_state14.var_40) == (0xa62c7ec3)) ? (rb_state14.var_41) : (0xa43816b6));
            rb_state14.var_19 = (rb_state14.var_19) - (rb_state14.var_42);
            rb_state14.var_3 = rb_state14.var_19;
            pthread_mutex_unlock(&(rb_state14.lock_43));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state15.lock_14));
            rb_state15.var_6 = 0xf4d8c98a;
            rb_state15.var_7 = (rb_state15.var_7) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_4) : (0x953666c5));
            rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_6) == (0xf4d8c98a)) ? (rb_state15.var_7) : (0x3cd1eec7));
            rb_state15.var_5 = (rb_state15.var_5) + (rb_state15.var_8);
            rb_state15.var_4 = rb_state15.var_5;
            pthread_mutex_unlock(&(rb_state15.lock_14));
        }
        if ((rb_state15.var_3) == (0x0)) {
            if (!((rb_state15.var_4) == (rb_state15.var_5))) {
                racebench_trigger(15);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_3) == (0x0)) {
            rb_state16.var_4 = rb_state16.var_0;
        }
        if ((rb_state16.var_3) == (0x0)) {
            if (!((rb_state16.var_0) == (rb_state16.var_4))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (0xf87e3b0);
        rb_state17.var_1 = (rb_state17.var_1) ^ (0x489bbb52);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) - ((0xd595f94) ^ (0xca2df1a2));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) + (0x3c9853a);
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_1) : (0xecc12e22));
    rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0x52748602));
    rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_2);
    if ((rb_state0.var_1) == (0x693fd549)) {
        rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_10) : (0xbb682582));
        rb_state0.var_19 = (rb_state0.var_19) + (rb_state0.var_11);
        rb_state0.var_20 = (rb_state0.var_20) - (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_12) : (0xb62fc42));
        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_19) : (0x8d0bb7fe));
        rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_20) : (0xf791b3ad));
        rb_state0.var_23 = (rb_state0.var_23) - (((0x466fa478) + (rb_state0.var_21)) ^ (rb_state0.var_15));
        rb_state0.var_24 = (rb_state0.var_24) ^ (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_23) : (0x20d49e92));
        rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_24);
        rb_state0.var_3 = rb_state0.var_17;
    }
    if ((rb_state0.var_1) == (0x693fd549)) {
        pthread_mutex_lock(&(rb_state0.lock_37));
        rb_state0.var_26 = 0xd0a88647;
        rb_state0.var_27 = 0x5f74ce1d;
        rb_state0.var_28 = 0xe9b65aa3;
        rb_state0.var_29 = (rb_state0.var_29) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_16) : (0x4cb78ced));
        rb_state0.var_30 = (rb_state0.var_30) - (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_28) : (0x42e3b072));
        rb_state0.var_31 = (rb_state0.var_31) - (((0xc9a16305) - (rb_state0.var_26)) + (rb_state0.var_18));
        rb_state0.var_32 = (rb_state0.var_32) + (((((0xd9635e0c) - (rb_state0.var_20)) - (rb_state0.var_29)) - (rb_state0.var_19)) + (rb_state0.var_30));
        rb_state0.var_33 = (rb_state0.var_33) - (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_31) : (0x274dabec));
        rb_state0.var_34 = (rb_state0.var_34) - (((0x44e294e4) - (rb_state0.var_32)) - (rb_state0.var_22));
        rb_state0.var_35 = (rb_state0.var_35) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_33) : (0xe675f8ce));
        rb_state0.var_36 = (rb_state0.var_36) + (((rb_state0.var_34) == (0x0)) ? (rb_state0.var_35) : (0x91681489));
        rb_state0.var_25 = (rb_state0.var_25) ^ (rb_state0.var_36);
        rb_state0.var_3 = rb_state0.var_25;
        pthread_mutex_unlock(&(rb_state0.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (0x14678c03);
    rb_state1.var_1 = (rb_state1.var_1) ^ (665557 < rb_input_size ? (uint32_t)rb_input[665557] : 0x5f17e3e5);
    rb_state1.var_2 = (rb_state1.var_2) + (1843964 < rb_input_size ? (uint32_t)rb_input[1843964] : 0x71e04820);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x50f5c186)) {
        rb_state2.var_10 = 0xf9ee841e;
        rb_state2.var_11 = (rb_state2.var_11) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_7) : (0x1082a424));
        rb_state2.var_12 = (rb_state2.var_12) - (2445511 < rb_input_size ? (uint32_t)rb_input[2445511] : 0x93b2be78);
        rb_state2.var_13 = (rb_state2.var_13) + (((((0xfba7e3f) - (rb_state2.var_9)) + (rb_state2.var_5)) ^ (rb_state2.var_6)) + (0x4a6acae));
        rb_state2.var_14 = (rb_state2.var_14) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0x4f00cb2f));
        rb_state2.var_15 = (rb_state2.var_15) - (((((0x7d016997) ^ (0xb3baa0c1)) + (0x8167a304)) - (rb_state2.var_13)) + (rb_state2.var_12));
        rb_state2.var_16 = (rb_state2.var_16) - (((0xec0dfef5) + (rb_state2.var_14)) + (rb_state2.var_10));
        rb_state2.var_17 = (rb_state2.var_17) - (((((0x5eb45d1c) + (rb_state2.var_11)) + (0x25afb35b)) + (rb_state2.var_16)) ^ (rb_state2.var_15));
        rb_state2.var_9 = (rb_state2.var_9) - (rb_state2.var_17);
        rb_state2.var_3 = rb_state2.var_9;
    }
    if ((rb_state2.var_2) == (0x50f5c186)) {
        pthread_mutex_lock(&(rb_state2.lock_29));
        rb_state2.var_19 = 0xbf1cf209;
        rb_state2.var_20 = (rb_state2.var_20) - (((0x42520b11) ^ (0x243ce7c8)) - (rb_state2.var_19));
        rb_state2.var_21 = (rb_state2.var_21) - (((((0x19fc070c) - (rb_state2.var_13)) ^ (rb_state2.var_14)) ^ (rb_state2.var_12)) ^ (0xb16a094f));
        rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_20);
        rb_state2.var_23 = (rb_state2.var_23) ^ (rb_state2.var_21);
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_22) : (0x3f171a));
        rb_state2.var_25 = (rb_state2.var_25) - (((0x528c2457) ^ (0x8f2efbd0)) + (rb_state2.var_23));
        rb_state2.var_26 = (rb_state2.var_26) + (rb_state2.var_24);
        rb_state2.var_27 = (rb_state2.var_27) - (((0x8c29fede) - (0xbbc110ab)) ^ (rb_state2.var_25));
        rb_state2.var_28 = (rb_state2.var_28) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_27) : (0xb14abdbd));
        rb_state2.var_18 = (rb_state2.var_18) - (rb_state2.var_28);
        rb_state2.var_3 = rb_state2.var_18;
        pthread_mutex_unlock(&(rb_state2.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_2);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x92937ffe) + (rb_state5.var_0));
    if ((rb_state5.var_2) == (0x1dea2d2c)) {
        pthread_mutex_lock(&(rb_state5.lock_18));
        rb_state5.var_14 = 0x2c7a8c89;
        rb_state5.var_15 = 0x4f0c4151;
        rb_state5.var_16 = (rb_state5.var_16) - (((((0xe33f8148) + (0x2aa33e94)) - (0x793ce853)) + (rb_state5.var_14)) ^ (rb_state5.var_15));
        rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_16) : (0xb6a0016c));
        rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_17);
        pthread_mutex_unlock(&(rb_state5.lock_18));
    }
    rb_state5.var_10 = (rb_state5.var_10) + ((0x23d099b2) ^ (0x75d73fd3));
    rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_13);
    rb_state5.var_16 = (rb_state5.var_16) + (2983399 < rb_input_size ? (uint32_t)rb_input[2983399] : 0x1d328eb7);
    if ((rb_state5.var_14) == (0x2c7a8c89)) {
        rb_state5.var_43 = 0x887d60eb;
        rb_state5.var_44 = 0x82c2efbd;
        rb_state5.var_45 = 0x491f9787;
        rb_state5.var_46 = (rb_state5.var_46) ^ (((0x81f39b79) ^ (rb_state5.var_31)) - (rb_state5.var_43));
        rb_state5.var_47 = (rb_state5.var_47) ^ (((0xcff3279f) ^ (rb_state5.var_44)) - (0x76cae186));
        rb_state5.var_48 = (rb_state5.var_48) - (((rb_state5.var_45) == (0x0)) ? (rb_state5.var_46) : (0x3c97f93f));
        rb_state5.var_49 = (rb_state5.var_49) ^ (((rb_state5.var_47) == (0x0)) ? (rb_state5.var_48) : (0x6d123cbb));
        rb_state5.var_42 = (rb_state5.var_42) + (rb_state5.var_49);
        rb_state5.var_20 = rb_state5.var_42;
    }
    if ((rb_state5.var_14) == (0x2c7a8c89)) {
        pthread_mutex_lock(&(rb_state5.lock_63));
        rb_state5.var_51 = 0x5b892fd9;
        rb_state5.var_52 = (rb_state5.var_52) + (((rb_state5.var_51) == (0x0)) ? (rb_state5.var_32) : (0xe8a53e47));
        rb_state5.var_53 = (rb_state5.var_53) - (rb_state5.var_33);
        rb_state5.var_54 = (rb_state5.var_54) - (0x50d3efe2);
        rb_state5.var_55 = (rb_state5.var_55) - (((0x7904271c) + (rb_state5.var_34)) + (rb_state5.var_52));
        rb_state5.var_56 = (rb_state5.var_56) - (((((0x88363d00) ^ (rb_state5.var_35)) - (rb_state5.var_53)) ^ (rb_state5.var_54)) - (0x8677dab4));
        rb_state5.var_57 = (rb_state5.var_57) + (((0xd51344b7) - (rb_state5.var_55)) - (0xc3a43915));
        rb_state5.var_58 = (rb_state5.var_58) ^ (rb_state5.var_56);
        rb_state5.var_59 = (rb_state5.var_59) + (rb_state5.var_57);
        rb_state5.var_60 = (rb_state5.var_60) ^ (((rb_state5.var_36) == (0x0)) ? (rb_state5.var_58) : (0xc8384dde));
        rb_state5.var_61 = (rb_state5.var_61) ^ (rb_state5.var_59);
        rb_state5.var_62 = (rb_state5.var_62) - (((((0x1736a4f1) - (0xb465de90)) ^ (0xb4f0a46)) ^ (rb_state5.var_60)) - (rb_state5.var_61));
        rb_state5.var_50 = (rb_state5.var_50) ^ (rb_state5.var_62);
        rb_state5.var_20 = rb_state5.var_50;
        pthread_mutex_unlock(&(rb_state5.lock_63));
    }
    rb_state5.var_37 = (rb_state5.var_37) - (2179025 < rb_input_size ? (uint32_t)rb_input[2179025] : 0x54495bd2);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (1184158 < rb_input_size ? (uint32_t)rb_input[1184158] : 0xce0cecc4);
    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_1);
    rb_state6.var_2 = (rb_state6.var_2) - ((0x15e6ed80) ^ (0x42b872dc));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (0xf3825dae);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_1) : (0xd7c6e90c));
    rb_state8.var_3 = (rb_state8.var_3) - (0x7f208e9c);
    rb_state8.var_0 = (rb_state8.var_0) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0x112a5a68));
    rb_state8.var_3 = (rb_state8.var_3) + (84006 < rb_input_size ? (uint32_t)rb_input[84006] : 0x6fc3291b);
    if ((rb_state8.var_3) == (0x8d17a833)) {
        rb_state8.var_5 = 0x45a61341;
        rb_state8.var_6 = 0x8a3d4e85;
        rb_state8.var_7 = (rb_state8.var_7) + (3256813 < rb_input_size ? (uint32_t)rb_input[3256813] : 0xa52dd1d7);
        rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_4);
        rb_state8.var_9 = (rb_state8.var_9) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0x24bc5d88));
        rb_state8.var_10 = (rb_state8.var_10) + (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_7) : (0xb4a42a67));
        rb_state8.var_11 = (rb_state8.var_11) - (((0xad408a90) + (rb_state8.var_8)) ^ (rb_state8.var_6));
        rb_state8.var_12 = (rb_state8.var_12) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_9) : (0xe0b73ff1));
        rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_10) : (0xd0b1f96e));
        rb_state8.var_14 = (rb_state8.var_14) + (((0xacc55262) ^ (rb_state8.var_11)) + (rb_state8.var_9));
        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_12) : (0xf1d46d6e));
        rb_state8.var_16 = (rb_state8.var_16) ^ (((0x2ef7e794) ^ (rb_state8.var_13)) ^ (rb_state8.var_11));
        rb_state8.var_17 = (rb_state8.var_17) - (((0x9ab8ff4b) + (rb_state8.var_14)) + (0x18a067a6));
        rb_state8.var_18 = (rb_state8.var_18) + (rb_state8.var_15);
        rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_16) : (0xe0c9c12e));
        rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_17);
        rb_state8.var_21 = (rb_state8.var_21) ^ (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_19) : (0x43b7969d));
        rb_state8.var_22 = (rb_state8.var_22) - (((((0x1da10d08) + (rb_state8.var_13)) ^ (rb_state8.var_20)) + (rb_state8.var_21)) - (0x7fa9796));
        rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_2) : (0x7e77b99));
    if ((rb_state9.var_3) == (0xee1777e4)) {
        rb_state9.var_11 = 0x9dad3a3d;
        rb_state9.var_12 = 0x6381bfbc;
        rb_state9.var_13 = (rb_state9.var_13) - (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_12) : (0xa16cb1ae));
        rb_state9.var_14 = (rb_state9.var_14) - (rb_state9.var_6);
        rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_4) : (0xd5f79904));
        rb_state9.var_16 = (rb_state9.var_16) - (((((0x4ddaecb0) - (rb_state9.var_13)) - (0x96795b4d)) ^ (0x4ccc805c)) ^ (rb_state9.var_11));
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_15) : (0xf2746f9f));
        rb_state9.var_18 = (rb_state9.var_18) - (((0xfff30ef1) ^ (rb_state9.var_8)) ^ (rb_state9.var_16));
        rb_state9.var_19 = (rb_state9.var_19) - (((0x5d63fc09) ^ (0x4ce4e1b7)) - (rb_state9.var_17));
        rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0xe7ab3502));
        rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (0xd9604974);
    rb_state10.var_2 = (rb_state10.var_2) ^ (3288503 < rb_input_size ? (uint32_t)rb_input[3288503] : 0xeaa3fc32);
    rb_state10.var_1 = (rb_state10.var_1) - ((0xaf86e3ff) - (rb_state10.var_2));
    rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_2) : (0x969c95d3));
    rb_state11.var_2 = (rb_state11.var_2) - (2965013 < rb_input_size ? (uint32_t)rb_input[2965013] : 0xe6f56327);
    if ((rb_state11.var_2) == (0x969c956b)) {
        rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_14);
        rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_12) : (0x7d84d80));
        rb_state11.var_17 = (rb_state11.var_17) + (rb_state11.var_16);
        rb_state11.var_18 = (rb_state11.var_18) ^ (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_15) : (0xb7462914));
        rb_state11.var_19 = (rb_state11.var_19) + (rb_state11.var_16);
        rb_state11.var_20 = (rb_state11.var_20) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_17) : (0xc11fcfa7));
        rb_state11.var_21 = (rb_state11.var_21) ^ (((((0x1d6de680) ^ (0x65d7c244)) + (0x3b4c6be6)) - (rb_state11.var_19)) + (rb_state11.var_18));
        rb_state11.var_22 = (rb_state11.var_22) + (rb_state11.var_20);
        rb_state11.var_23 = (rb_state11.var_23) + (((((0x958374e2) - (rb_state11.var_21)) + (0xcff2091e)) ^ (0x580a19)) - (rb_state11.var_22));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x9e2fc513));
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_1);
    rb_state12.var_2 = (rb_state12.var_2) + (0xfa29e33);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
    rb_state13.var_1 = (rb_state13.var_1) ^ (2694825 < rb_input_size ? (uint32_t)rb_input[2694825] : 0x644d0231);
    rb_state13.var_1 = (rb_state13.var_1) ^ (3128200 < rb_input_size ? (uint32_t)rb_input[3128200] : 0x28eca804);
    rb_state13.var_2 = (rb_state13.var_2) + (664563 < rb_input_size ? (uint32_t)rb_input[664563] : 0x777a79f3);
    rb_state13.var_11 = (rb_state13.var_11) ^ (rb_state13.var_12);
    if ((rb_state13.var_19) == (0x1a27ba02)) {
        rb_state13.var_31 = 0x638bc1fc;
        rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_22) : (0x2766b9e));
        rb_state13.var_33 = (rb_state13.var_33) ^ (2139744 < rb_input_size ? (uint32_t)rb_input[2139744] : 0x5faf267);
        rb_state13.var_34 = (rb_state13.var_34) ^ (0x8640856d);
        rb_state13.var_35 = (rb_state13.var_35) - (rb_state13.var_31);
        rb_state13.var_36 = (rb_state13.var_36) + (((0xf0bc400e) - (rb_state13.var_23)) + (rb_state13.var_32));
        rb_state13.var_37 = (rb_state13.var_37) + (((0x1e7ff0fc) - (rb_state13.var_33)) + (0xe01d78cf));
        rb_state13.var_38 = (rb_state13.var_38) ^ (((((0x3858a281) ^ (rb_state13.var_35)) - (0x79f891b2)) + (rb_state13.var_34)) ^ (0xce8feb48));
        rb_state13.var_39 = (rb_state13.var_39) + (((((0xa67c302a) - (rb_state13.var_36)) ^ (rb_state13.var_37)) + (0x9ebd732)) - (rb_state13.var_24));
        rb_state13.var_40 = (rb_state13.var_40) + (rb_state13.var_38);
        rb_state13.var_41 = (rb_state13.var_41) ^ (((0xb95ec574) - (rb_state13.var_25)) + (rb_state13.var_39));
        rb_state13.var_42 = (rb_state13.var_42) - (((0x2b380ada) - (rb_state13.var_26)) - (rb_state13.var_40));
        rb_state13.var_43 = (rb_state13.var_43) - (((rb_state13.var_27) == (0x0)) ? (rb_state13.var_41) : (0x67ffee4b));
        rb_state13.var_44 = (rb_state13.var_44) + (((0x49c462f) - (rb_state13.var_42)) ^ (rb_state13.var_29));
        rb_state13.var_45 = (rb_state13.var_45) + (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_43) : (0xaf4306fe));
        rb_state13.var_46 = (rb_state13.var_46) + (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_44) : (0x213a39f9));
        rb_state13.var_47 = (rb_state13.var_47) - (((((0x4fc7e517) ^ (rb_state13.var_45)) ^ (rb_state13.var_33)) ^ (rb_state13.var_32)) ^ (rb_state13.var_46));
        rb_state13.var_20 = (rb_state13.var_20) ^ (rb_state13.var_47);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (233730 < rb_input_size ? (uint32_t)rb_input[233730] : 0xc9911e04);
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x36)) ? (rb_state14.var_0) : (0x9194b9ac));
    rb_state14.var_2 = (rb_state14.var_2) - (0xbcb56ec0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ ((0x1bce9e17) ^ (rb_state15.var_0));
    rb_state15.var_3 = (rb_state15.var_3) ^ (3018757 < rb_input_size ? (uint32_t)rb_input[3018757] : 0x9f2e3fb0);
    rb_state15.var_3 = (rb_state15.var_3) ^ ((0xf5198723) ^ (0xfd536f91));
    if ((rb_state15.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_14));
        rb_state15.var_9 = 0x610244ac;
        rb_state15.var_10 = (rb_state15.var_10) - (((0x80a30906) ^ (0x113b3157)) + (rb_state15.var_7));
        rb_state15.var_11 = (rb_state15.var_11) ^ (rb_state15.var_6);
        rb_state15.var_12 = (rb_state15.var_12) - (((((0xf4ed645c) + (0xdd9476d9)) - (rb_state15.var_10)) ^ (rb_state15.var_9)) - (rb_state15.var_8));
        rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_12) : (0x1c9840ef));
        rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_13);
        pthread_mutex_unlock(&(rb_state15.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0xf098da37));
    rb_state16.var_2 = (rb_state16.var_2) ^ ((0x1acae291) - (0x6e3740e4));
    rb_state16.var_2 = (rb_state16.var_2) ^ ((0x1b5d8455) ^ (0x957ac7d7));
    rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_3);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_3) == (0xca1e48cc)) {
        pthread_mutex_lock(&(rb_state17.lock_36));
        rb_state17.var_6 = 0x5972d21f;
        rb_state17.var_7 = 0xb3d4b800;
        rb_state17.var_8 = (rb_state17.var_8) ^ (rb_state17.var_4);
        rb_state17.var_9 = (rb_state17.var_9) - (761450 < rb_input_size ? (uint32_t)rb_input[761450] : 0x250b169c);
        rb_state17.var_10 = (rb_state17.var_10) + (((0x46c0f57e) ^ (rb_state17.var_5)) ^ (rb_state17.var_6));
        rb_state17.var_11 = (rb_state17.var_11) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0xc69cd546));
        rb_state17.var_12 = (rb_state17.var_12) + (((0xd4e6054d) + (rb_state17.var_7)) - (rb_state17.var_8));
        rb_state17.var_13 = (rb_state17.var_13) ^ (rb_state17.var_9);
        rb_state17.var_14 = (rb_state17.var_14) ^ (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_10) : (0x2bb8aeef));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((0x69dc3223) + (rb_state17.var_9)) ^ (rb_state17.var_11));
        rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_12) : (0x2aae7556));
        rb_state17.var_17 = (rb_state17.var_17) - (((((0xcf2a3b6b) ^ (0x80755274)) - (rb_state17.var_13)) ^ (rb_state17.var_11)) - (rb_state17.var_14));
        rb_state17.var_18 = (rb_state17.var_18) + (rb_state17.var_15);
        rb_state17.var_19 = (rb_state17.var_19) ^ (((0x30d72c23) + (0xa710630f)) + (rb_state17.var_16));
        rb_state17.var_20 = (rb_state17.var_20) - (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_18) : (0xbdd1c35c));
        rb_state17.var_21 = (rb_state17.var_21) - (((0xc373c50e) - (rb_state17.var_12)) - (rb_state17.var_19));
        rb_state17.var_22 = (rb_state17.var_22) - (rb_state17.var_20);
        rb_state17.var_23 = (rb_state17.var_23) - (rb_state17.var_21);
        rb_state17.var_24 = (rb_state17.var_24) ^ (((0xb88add21) ^ (rb_state17.var_22)) + (rb_state17.var_13));
        rb_state17.var_25 = (rb_state17.var_25) + (rb_state17.var_23);
        rb_state17.var_26 = (rb_state17.var_26) - (rb_state17.var_24);
        rb_state17.var_27 = (rb_state17.var_27) - (((((0xc7e69918) - (0x97afeb03)) + (rb_state17.var_14)) + (rb_state17.var_25)) ^ (rb_state17.var_26));
        rb_state17.var_5 = (rb_state17.var_5) - (rb_state17.var_27);
        rb_state17.var_4 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_36));
    }
    if ((rb_state17.var_3) == (0xca1e48cc)) {
        if (!((rb_state17.var_4) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_0) == (0xda2c2595)) ? (rb_state18.var_1) : (0x74df012b));
    if ((rb_state18.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_24));
        rb_state18.var_6 = 0x25192b7c;
        rb_state18.var_7 = 0xea4b8aab;
        rb_state18.var_8 = (rb_state18.var_8) ^ (rb_state18.var_6);
        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_7) : (0xc0ca85df));
        rb_state18.var_10 = (rb_state18.var_10) ^ (((0x6d1658e8) + (rb_state18.var_4)) - (rb_state18.var_6));
        rb_state18.var_11 = (rb_state18.var_11) + (((0x9e0bf15) + (rb_state18.var_7)) ^ (rb_state18.var_8));
        rb_state18.var_12 = (rb_state18.var_12) + (((0xff8b17d5) + (rb_state18.var_9)) - (0x4de1c712));
        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_11) : (0xdb711ad0));
        rb_state18.var_14 = (rb_state18.var_14) - (rb_state18.var_12);
        rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_13);
        rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0x2f9f5bc5));
        rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_16);
        rb_state18.var_4 = rb_state18.var_5;
        pthread_mutex_unlock(&(rb_state18.lock_24));
    }
    if ((rb_state18.var_2) == (0x0)) {
        if (!((rb_state18.var_4) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x4e622bda);
    rb_state19.var_2 = (rb_state19.var_2) + (0x7c5493d0);
    if ((rb_state19.var_2) == (0xfc72acda)) {
        rb_state19.var_12 = 0x84f0bed5;
        rb_state19.var_13 = (rb_state19.var_13) + (3286367 < rb_input_size ? (uint32_t)rb_input[3286367] : 0x2a5cc109);
        rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_11) : (0x9c0aeffe));
        rb_state19.var_15 = (rb_state19.var_15) + (((((0x6b5d9636) ^ (0xa83a320d)) ^ (rb_state19.var_9)) - (rb_state19.var_10)) - (rb_state19.var_13));
        rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_13) : (0xaaec0b17));
        rb_state19.var_17 = (rb_state19.var_17) + (((((0xed79d385) - (rb_state19.var_14)) ^ (rb_state19.var_14)) - (0x83ee6587)) ^ (rb_state19.var_15));
        rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_17) : (0x6edd0179));
        rb_state19.var_11 = (rb_state19.var_11) - (rb_state19.var_18);
        rb_state19.var_4 = rb_state19.var_11;
    }
    if ((rb_state19.var_2) == (0xfc72acda)) {
        pthread_mutex_lock(&(rb_state19.lock_23));
        rb_state19.var_20 = (rb_state19.var_20) - (((((0xe637d778) + (rb_state19.var_18)) - (rb_state19.var_17)) ^ (0xf7068eda)) + (rb_state19.var_16));
        rb_state19.var_21 = (rb_state19.var_21) ^ (((0x59f5e4fc) ^ (rb_state19.var_19)) - (rb_state19.var_15));
        rb_state19.var_22 = (rb_state19.var_22) - (((rb_state19.var_20) == (0x0)) ? (rb_state19.var_21) : (0x8c03f729));
        rb_state19.var_19 = (rb_state19.var_19) - (rb_state19.var_22);
        rb_state19.var_4 = rb_state19.var_19;
        pthread_mutex_unlock(&(rb_state19.lock_23));
    }
    #endif
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
    rb_state0.var_1 = (rb_state0.var_1) + ((0x85fefb6b) + (rb_state0.var_0));
    rb_state0.var_2 = (rb_state0.var_2) + (814954 < rb_input_size ? (uint32_t)rb_input[814954] : 0x961cf4ea);
    rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_2) == (0x60)) ? (rb_state0.var_2) : (0x6769c456));
    if ((rb_state0.var_1) == (0x693fd549)) {
        pthread_mutex_lock(&(rb_state0.lock_37));
        if ((rb_state0.var_3) != (0x0)) {
            if (!((rb_state0.var_3) == (rb_state0.var_25))) {
                racebench_trigger(0);
            }
        }
        pthread_mutex_unlock(&(rb_state0.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x0)) {
        rb_state1.var_4 = rb_state1.var_1;
    }
    if ((rb_state1.var_3) == (0x0)) {
        if (!((rb_state1.var_1) == (rb_state1.var_4))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (0x2d172dfc);
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0xaf4c407a) + (rb_state2.var_2));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (472340 < rb_input_size ? (uint32_t)rb_input[472340] : 0xe01394ac);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (0xa434feb2);
    rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_1);
    rb_state5.var_2 = (rb_state5.var_2) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_2) : (0x1e33356b));
    if ((rb_state5.var_41) == (0x0)) {
        rb_state5.var_66 = 0xcbe33d54;
        rb_state5.var_67 = (rb_state5.var_67) + (rb_state5.var_43);
        rb_state5.var_68 = (rb_state5.var_68) + (((rb_state5.var_45) == (0x0)) ? (rb_state5.var_66) : (0x31a43a6a));
        rb_state5.var_69 = (rb_state5.var_69) ^ (rb_state5.var_46);
        rb_state5.var_70 = (rb_state5.var_70) + (((0x7974dfc7) + (rb_state5.var_47)) ^ (rb_state5.var_44));
        rb_state5.var_71 = (rb_state5.var_71) - (rb_state5.var_67);
        rb_state5.var_72 = (rb_state5.var_72) + (((0x1edafb76) + (rb_state5.var_68)) ^ (0xa39595ad));
        rb_state5.var_73 = (rb_state5.var_73) - (((rb_state5.var_69) == (0x0)) ? (rb_state5.var_70) : (0xee0de60f));
        rb_state5.var_74 = (rb_state5.var_74) ^ (((((0x6c73896d) ^ (rb_state5.var_48)) - (rb_state5.var_49)) - (rb_state5.var_72)) ^ (rb_state5.var_71));
        rb_state5.var_75 = (rb_state5.var_75) ^ (((rb_state5.var_73) == (0x0)) ? (rb_state5.var_74) : (0xaf619e5f));
        rb_state5.var_42 = (rb_state5.var_42) + (rb_state5.var_75);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_2) == (0x5142c148)) ? (rb_state6.var_1) : (0x24201b79));
    if ((rb_state6.var_0) == (0x35)) {
        pthread_mutex_lock(&(rb_state6.lock_23));
        rb_state6.var_7 = 0xd0f397f3;
        rb_state6.var_8 = 0x802272fe;
        rb_state6.var_9 = (rb_state6.var_9) + (((0x3a201d63) ^ (rb_state6.var_7)) ^ (rb_state6.var_6));
        rb_state6.var_10 = (rb_state6.var_10) + (((0x488c386) + (0xa3a88b4c)) - (rb_state6.var_5));
        rb_state6.var_11 = (rb_state6.var_11) - (rb_state6.var_8);
        rb_state6.var_12 = (rb_state6.var_12) ^ (((0xa3912df3) - (rb_state6.var_9)) + (0x81c8ea09));
        rb_state6.var_13 = (rb_state6.var_13) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_10) : (0x18b1f16b));
        rb_state6.var_14 = (rb_state6.var_14) + (((((0x4cea0b3a) + (rb_state6.var_11)) ^ (rb_state6.var_8)) ^ (0xf9605369)) ^ (rb_state6.var_12));
        rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_13) : (0xca47ef15));
        rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_15) : (0xce975200));
        rb_state6.var_6 = (rb_state6.var_6) ^ (rb_state6.var_16);
        rb_state6.var_3 = rb_state6.var_6;
        pthread_mutex_unlock(&(rb_state6.lock_23));
    }
    if ((rb_state6.var_0) == (0x35)) {
        pthread_mutex_lock(&(rb_state6.lock_23));
        rb_state6.var_18 = (rb_state6.var_18) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_10) : (0xbbf2c55d));
        rb_state6.var_19 = (rb_state6.var_19) + (((((0xcfeb3f7b) + (0x5343498d)) + (rb_state6.var_12)) + (0x54b79221)) ^ (rb_state6.var_11));
        rb_state6.var_20 = (rb_state6.var_20) + (rb_state6.var_18);
        rb_state6.var_21 = (rb_state6.var_21) + (rb_state6.var_19);
        rb_state6.var_22 = (rb_state6.var_22) - (((((0xc04a6755) - (rb_state6.var_21)) - (0x10137ae0)) + (rb_state6.var_20)) + (0x43c5f9c));
        rb_state6.var_17 = (rb_state6.var_17) ^ (rb_state6.var_22);
        rb_state6.var_3 = rb_state6.var_17;
        pthread_mutex_unlock(&(rb_state6.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0xa50c7b74);
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_0);
    if ((rb_state7.var_0) == (0x55024db0)) {
        pthread_mutex_lock(&(rb_state7.lock_36));
        rb_state7.var_10 = 0xbb26a8e;
        rb_state7.var_11 = 0xce9f32ae;
        rb_state7.var_12 = (rb_state7.var_12) + (0x7f26f1ec);
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_4) : (0x743c87e5));
        rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_10) : (0xa40109e2));
        rb_state7.var_15 = (rb_state7.var_15) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_12) : (0xb3856abf));
        rb_state7.var_16 = (rb_state7.var_16) - (((0x538fb5d8) - (rb_state7.var_13)) - (rb_state7.var_7));
        rb_state7.var_17 = (rb_state7.var_17) - (((0xf44c6bcc) ^ (0x7cfdd25c)) + (rb_state7.var_14));
        rb_state7.var_18 = (rb_state7.var_18) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_16) : (0x15f17408));
        rb_state7.var_19 = (rb_state7.var_19) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_17) : (0xee89a1d2));
        rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_18) : (0x59cdd6b0));
        rb_state7.var_21 = (rb_state7.var_21) + (rb_state7.var_19);
        rb_state7.var_22 = (rb_state7.var_22) + (((((0x81898275) ^ (rb_state7.var_11)) + (rb_state7.var_10)) + (rb_state7.var_20)) ^ (rb_state7.var_21));
        rb_state7.var_9 = (rb_state7.var_9) - (rb_state7.var_22);
        rb_state7.var_2 = rb_state7.var_9;
        pthread_mutex_unlock(&(rb_state7.lock_36));
    }
    if ((rb_state7.var_0) == (0x55024db0)) {
        pthread_mutex_lock(&(rb_state7.lock_36));
        rb_state7.var_24 = 0x50e94e97;
        rb_state7.var_25 = (rb_state7.var_25) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_14) : (0xe7927100));
        rb_state7.var_26 = (rb_state7.var_26) ^ ((0xe9a80c64) + (0x1bdde41e));
        rb_state7.var_27 = (rb_state7.var_27) - (rb_state7.var_12);
        rb_state7.var_28 = (rb_state7.var_28) - (rb_state7.var_13);
        rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_24) : (0xec3ff99));
        rb_state7.var_30 = (rb_state7.var_30) + (((0xbb75322d) - (rb_state7.var_25)) ^ (rb_state7.var_17));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_26) : (0x6b8e6db6));
        rb_state7.var_32 = (rb_state7.var_32) - (((((0xff8f3ec0) + (rb_state7.var_27)) ^ (0xf079070)) ^ (rb_state7.var_19)) ^ (rb_state7.var_28));
        rb_state7.var_33 = (rb_state7.var_33) ^ (((((0x8bf779ce) + (0x64c5706f)) - (rb_state7.var_20)) ^ (rb_state7.var_29)) + (rb_state7.var_30));
        rb_state7.var_34 = (rb_state7.var_34) + (((((0x33140630) + (0xc46ad996)) + (rb_state7.var_31)) ^ (0x77a84c62)) - (rb_state7.var_32));
        rb_state7.var_35 = (rb_state7.var_35) ^ (((((0x44cbdaef) - (rb_state7.var_22)) ^ (rb_state7.var_34)) - (rb_state7.var_21)) ^ (rb_state7.var_33));
        rb_state7.var_23 = (rb_state7.var_23) + (rb_state7.var_35);
        rb_state7.var_2 = rb_state7.var_23;
        pthread_mutex_unlock(&(rb_state7.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) + (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_3) : (0x93b0e68f));
    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0x6af786cd));
    if ((rb_state8.var_3) == (0xc383696)) {
        rb_state8.var_4 = rb_state8.var_0;
    }
    if ((rb_state8.var_3) == (0x8d17a833)) {
        pthread_mutex_lock(&(rb_state8.lock_23));
        if (!((rb_state8.var_0) == (rb_state8.var_4))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0x56340a85);
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0xa9cbf57b)) ? (rb_state9.var_0) : (0xf65d3a08));
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_2) : (0xbc9a20bb));
    rb_state9.var_3 = (rb_state9.var_3) + (0x2a6695db);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0xc83c2ed9);
    rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_2) == (0xfa29e33)) ? (rb_state12.var_1) : (0x65927cf8));
    if ((rb_state12.var_0) == (0xd157dd3b)) {
        rb_state12.var_3 = rb_state12.var_1;
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (0x4e7e30ce);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (0x13eef5b3);
    rb_state14.var_1 = (rb_state14.var_1) - (2735894 < rb_input_size ? (uint32_t)rb_input[2735894] : 0xb3f0646a);
    rb_state14.var_2 = (rb_state14.var_2) + ((0xd3911ef0) + (rb_state14.var_1));
    rb_state14.var_2 = (rb_state14.var_2) - (2051918 < rb_input_size ? (uint32_t)rb_input[2051918] : 0x7aecbe50);
    rb_state14.var_1 = (rb_state14.var_1) + ((0xd1187b10) - (rb_state14.var_2));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x1bce9e17)) ? (rb_state15.var_1) : (0x4d4adce5));
    rb_state15.var_1 = (rb_state15.var_1) - ((0x97fc8ebc) - (rb_state15.var_0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (((rb_state16.var_0) == (0xa0883d90)) ? (rb_state16.var_3) : (0x1349b1ed));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - ((0x35e1b734) - (rb_state17.var_3));
    if ((rb_state17.var_3) == (0xca1e48cc)) {
        rb_state17.var_28 = 0x1e64e1e7;
        rb_state17.var_29 = 0x9fb05e54;
        rb_state17.var_30 = (rb_state17.var_30) - (((rb_state17.var_29) == (0x9fb05e54)) ? (rb_state17.var_15) : (0xb5301c14));
        rb_state17.var_31 = (rb_state17.var_31) + (((rb_state17.var_16) == (0x2aae7556)) ? (rb_state17.var_28) : (0x8dd36e2c));
        rb_state17.var_32 = (rb_state17.var_32) - (((0x8898e0d) - (0x6242ddf7)) - (rb_state17.var_30));
        rb_state17.var_33 = (rb_state17.var_33) - (((rb_state17.var_17) == (0x95ee6b58)) ? (rb_state17.var_31) : (0x9b714230));
        rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_18) == (0xaf40e4b5)) ? (rb_state17.var_32) : (0x148598bb));
        rb_state17.var_35 = (rb_state17.var_35) ^ (((((0xf58c3a21) - (rb_state17.var_33)) ^ (0x3d3ac0fa)) + (rb_state17.var_34)) + (rb_state17.var_19));
        rb_state17.var_4 = (rb_state17.var_4) + (rb_state17.var_35);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_2);
    rb_state18.var_3 = (rb_state18.var_3) - ((0xaa1d4418) - (rb_state18.var_0));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - ((0x6cf52ec6) + (rb_state19.var_1));
    rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_0);
    rb_state19.var_1 = (rb_state19.var_1) ^ (2854128 < rb_input_size ? (uint32_t)rb_input[2854128] : 0x86cb8392);
    if ((rb_state19.var_2) == (0xfc72acda)) {
        if ((rb_state19.var_4) != (0x0)) {
            if (!((rb_state19.var_4) == (rb_state19.var_19))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    ProcID = ProcID % NumProcs;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
    rb_state2.var_0 = (rb_state2.var_0) - ((0x1a4f9d66) + (0x3d33f71c));
    if ((rb_state2.var_2) == (0x50f5c186)) {
        if ((rb_state2.var_3) != (0x0)) {
            if (!((rb_state2.var_3) == (rb_state2.var_18))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (((rb_state4.var_2) == (0x8c6a2919)) ? (rb_state4.var_3) : (0xa818495b));
    if ((rb_state4.var_3) == (0x6a)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_25))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ ((0x1dea2d2c) - (rb_state5.var_2));
    rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_1);
    if ((rb_state5.var_2) == (0x1dea2d2c)) {
        pthread_mutex_lock(&(rb_state5.lock_18));
        rb_state5.var_5 = 0x56a2bf4f;
        rb_state5.var_6 = 0x48ca1a06;
        rb_state5.var_7 = (rb_state5.var_7) - (485983 < rb_input_size ? (uint32_t)rb_input[485983] : 0x4f933a4e);
        rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_5) : (0x4ee09446));
        rb_state5.var_9 = (rb_state5.var_9) - (((rb_state5.var_5) == (0x56a2bf4f)) ? (rb_state5.var_3) : (0xac8288ac));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((((0x83759295) + (rb_state5.var_7)) + (0x428e2660)) ^ (rb_state5.var_6)) ^ (rb_state5.var_6));
        rb_state5.var_11 = (rb_state5.var_11) - (((((0x9ca1894a) ^ (0x89e107bd)) ^ (0x8f2312ef)) + (rb_state5.var_8)) ^ (rb_state5.var_9));
        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_7) == (0xffffffc8)) ? (rb_state5.var_10) : (0x72c69005));
        rb_state5.var_13 = (rb_state5.var_13) - (((((0x555c1c7) ^ (rb_state5.var_11)) - (rb_state5.var_12)) + (rb_state5.var_8)) + (0xb4065e2a));
        rb_state5.var_4 = (rb_state5.var_4) ^ (rb_state5.var_13);
        rb_state5.var_3 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_18));
    }
    if ((rb_state5.var_2) == (0x1dea2d2c)) {
        pthread_mutex_lock(&(rb_state5.lock_18));
        rb_state5.var_19 = !((rb_state5.var_3) == (rb_state5.var_4));
        pthread_mutex_unlock(&(rb_state5.lock_18));
    }
    rb_state5.var_11 = (rb_state5.var_11) - (0x8156bd8d);
    rb_state5.var_38 = (rb_state5.var_38) + (((rb_state5.var_40) == (0x0)) ? (rb_state5.var_39) : (0xaa8c5e8));
    if ((rb_state5.var_41) == (0x0)) {
        rb_state5.var_65 = rb_state5.var_42;
    }
    if ((rb_state5.var_41) == (0x0)) {
        if (!((rb_state5.var_42) == (rb_state5.var_65))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x35)) {
        if ((rb_state6.var_3) != (0x0)) {
            if (!((rb_state6.var_3) == (rb_state6.var_17))) {
                racebench_trigger(6);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (2606204 < rb_input_size ? (uint32_t)rb_input[2606204] : 0x6be7ea0f);
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (0x526ed8f9);
    rb_state9.var_3 = (rb_state9.var_3) - ((0x33ebdbe5) + (0x7f6efe04));
    if ((rb_state9.var_3) == (0xee1777e4)) {
        pthread_mutex_lock(&(rb_state9.lock_21));
        rb_state9.var_6 = 0x1a0a8514;
        rb_state9.var_7 = 0x8993637a;
        rb_state9.var_8 = 0xf63512da;
        rb_state9.var_9 = (rb_state9.var_9) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0xf7401be9));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_9) : (0xb07460b6));
        rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_10);
        rb_state9.var_4 = rb_state9.var_5;
        pthread_mutex_unlock(&(rb_state9.lock_21));
    }
    if ((rb_state9.var_3) == (0xee1777e4)) {
        if (!((rb_state9.var_4) == (rb_state9.var_5))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x3f6bf451)) {
        if ((rb_state10.var_4) != (0x0)) {
            if (!((rb_state10.var_4) == (rb_state10.var_23))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xd157dd3b)) {
        if (!((rb_state12.var_1) == (rb_state12.var_3))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (0xa0249612);
    if ((rb_state13.var_3) == (0xbfb6d3dc)) {
        rb_state13.var_6 = 0x1;
    }
    if ((rb_state13.var_3) == (0xbfb6d3dc)) {
        pthread_mutex_lock(&(rb_state13.lock_28));
        rb_state13.var_7 = 0x6c43fbf6;
        rb_state13.var_8 = 0x721dcec3;
        rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_5) : (0x98958c03));
        rb_state13.var_10 = (rb_state13.var_10) - (0x683372cb);
        rb_state13.var_11 = (rb_state13.var_11) + (((0x3429c680) ^ (0xe1b1bf9)) ^ (rb_state13.var_4));
        rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_7);
        rb_state13.var_13 = (rb_state13.var_13) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0xefa7b966));
        rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0xc852f877));
        rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_10);
        rb_state13.var_16 = (rb_state13.var_16) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_12) : (0xe59d27c3));
        rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_13) : (0x6453f4b9));
        rb_state13.var_18 = (rb_state13.var_18) + (((0xf8c6c8f4) - (rb_state13.var_14)) + (0x333799c7));
        rb_state13.var_19 = (rb_state13.var_19) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0xe5d845fe));
        rb_state13.var_20 = (rb_state13.var_20) + (rb_state13.var_17);
        rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_18);
        rb_state13.var_22 = (rb_state13.var_22) + (((((0x6433c08a) + (0x3492a985)) ^ (rb_state13.var_19)) ^ (rb_state13.var_20)) + (0x968bd006));
        rb_state13.var_23 = (rb_state13.var_23) - (((0xd44a4f1b) + (rb_state13.var_21)) - (0x6690f469));
        rb_state13.var_24 = (rb_state13.var_24) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_22) : (0x12fc3acd));
        rb_state13.var_25 = (rb_state13.var_25) ^ (rb_state13.var_23);
        rb_state13.var_26 = (rb_state13.var_26) + (rb_state13.var_24);
        rb_state13.var_27 = (rb_state13.var_27) ^ (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_26) : (0x558843e5));
        rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_27);
        rb_state13.var_4 = rb_state13.var_5;
        pthread_mutex_unlock(&(rb_state13.lock_28));
    }
    rb_state13.var_13 = (rb_state13.var_13) + (0x22944ef4);
    rb_state13.var_14 = (rb_state13.var_14) + ((0x73ee2b35) ^ (rb_state13.var_15));
    rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_17) : (0xe3ff1cd3));
    if ((rb_state13.var_19) == (0x1a27ba02)) {
        pthread_mutex_lock(&(rb_state13.lock_48));
        rb_state13.var_30 = rb_state13.var_20;
        pthread_mutex_unlock(&(rb_state13.lock_48));
    }
    if ((rb_state13.var_19) == (0x1a27ba02)) {
        if (!((rb_state13.var_20) == (rb_state13.var_30))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x9194ba06)) {
        if ((rb_state14.var_3) != (0x0)) {
            if (!((rb_state14.var_3) == (rb_state14.var_19))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (505628 < rb_input_size ? (uint32_t)rb_input[505628] : 0x7a617e8c);
    rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_2) == (0xffffffce)) ? (rb_state15.var_2) : (0x4ff26a8d));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_2);
    if ((rb_state16.var_3) == (0x0)) {
        rb_state16.var_5 = 0x9d03024c;
        rb_state16.var_6 = 0x1b162f3c;
        rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_6) == (0x1b162f3c)) ? (rb_state16.var_5) : (0x897bd2d3));
        rb_state16.var_8 = (rb_state16.var_8) + ((0xdec3e0c1) + (0x4722793b));
        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_7) == (0x9d03024c)) ? (rb_state16.var_4) : (0xc706e056));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((0x727a5bf3) ^ (rb_state16.var_6)) ^ (rb_state16.var_8));
        rb_state16.var_11 = (rb_state16.var_11) + (((rb_state16.var_9) == (0xa0883d90)) ? (rb_state16.var_5) : (0xd5eecb35));
        rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_10) == (0x4c8a2d33)) ? (rb_state16.var_7) : (0x7946a24a));
        rb_state16.var_13 = (rb_state16.var_13) + (((0x4fa6f9ba) ^ (rb_state16.var_8)) - (rb_state16.var_11));
        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_9) == (0xa0883d90)) ? (rb_state16.var_10) : (0xbaab92fc));
        rb_state16.var_15 = (rb_state16.var_15) - (((((0x249a2cd4) - (0xda4052eb)) ^ (rb_state16.var_12)) + (rb_state16.var_12)) ^ (rb_state16.var_11));
        rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_13) == (0xcd3d9dfa)) ? (rb_state16.var_13) : (0x18b9c273));
        rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_14) == (0x4c8a2d33)) ? (rb_state16.var_15) : (0x58eec7c2));
        rb_state16.var_18 = (rb_state16.var_18) - (((rb_state16.var_16) == (0xcd3d9dfa)) ? (rb_state16.var_17) : (0xb3df7903));
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - ((0x8b96c5a7) ^ (rb_state18.var_3));
    rb_state18.var_1 = (rb_state18.var_1) ^ (2643774 < rb_input_size ? (uint32_t)rb_input[2643774] : 0x9e2d505a);
    if ((rb_state18.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_24));
        rb_state18.var_17 = 0xdc849141;
        rb_state18.var_18 = (rb_state18.var_18) ^ (((((0x2ab9033d) - (rb_state18.var_10)) - (0xb634651)) ^ (rb_state18.var_9)) - (rb_state18.var_8));
        rb_state18.var_19 = (rb_state18.var_19) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_17) : (0xbeb357ef));
        rb_state18.var_20 = (rb_state18.var_20) - (rb_state18.var_18);
        rb_state18.var_21 = (rb_state18.var_21) + (rb_state18.var_19);
        rb_state18.var_22 = (rb_state18.var_22) - (rb_state18.var_20);
        rb_state18.var_23 = (rb_state18.var_23) ^ (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_22) : (0xb48b0978));
        rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_23);
        pthread_mutex_unlock(&(rb_state18.lock_24));
    }
    #endif
    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}