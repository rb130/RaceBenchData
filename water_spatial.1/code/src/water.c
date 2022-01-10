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
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_0) == (0x97ed02ef)) {
                rb_state2.var_5 = 0x5a5c97d5;
                rb_state2.var_6 = (rb_state2.var_6) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0xd31ad875));
                rb_state2.var_7 = (rb_state2.var_7) - (rb_state2.var_3);
                rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_5);
                rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_6) : (0xcc1ab85e));
                rb_state2.var_10 = (rb_state2.var_10) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0x5165d650));
                rb_state2.var_11 = (rb_state2.var_11) ^ (((0x37ff6fe2) ^ (rb_state2.var_7)) + (rb_state2.var_8));
                rb_state2.var_12 = (rb_state2.var_12) ^ (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_9) : (0xe3724630));
                rb_state2.var_13 = (rb_state2.var_13) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0x59b18bb3));
                rb_state2.var_14 = (rb_state2.var_14) - (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_12) : (0x3701e9ab));
                rb_state2.var_15 = (rb_state2.var_15) + (((((0x353d85d9) ^ (rb_state2.var_14)) + (rb_state2.var_10)) + (rb_state2.var_13)) ^ (rb_state2.var_11));
                rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_15);
                rb_state2.var_1 = rb_state2.var_4;
            }
            if ((rb_state2.var_0) == (0x97ed02ef)) {
                pthread_mutex_lock(&(rb_state2.lock_27));
                rb_state2.var_17 = 0x9cb6b02a;
                rb_state2.var_18 = 0x5f05439d;
                rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_18);
                rb_state2.var_20 = (rb_state2.var_20) ^ (2550200 < rb_input_size ? (uint32_t)rb_input[2550200] : 0x9e110f45);
                rb_state2.var_21 = (rb_state2.var_21) - (((0x722636c0) ^ (rb_state2.var_13)) - (rb_state2.var_12));
                rb_state2.var_22 = (rb_state2.var_22) ^ (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_19) : (0x235aa16b));
                rb_state2.var_23 = (rb_state2.var_23) - (rb_state2.var_20);
                rb_state2.var_24 = (rb_state2.var_24) ^ (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_22) : (0x404fbd7a));
                rb_state2.var_25 = (rb_state2.var_25) + (rb_state2.var_23);
                rb_state2.var_26 = (rb_state2.var_26) + (((((0xf00075d7) ^ (0x7a6ed7a3)) - (rb_state2.var_14)) - (rb_state2.var_25)) - (rb_state2.var_24));
                rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_26);
                rb_state2.var_1 = rb_state2.var_16;
                pthread_mutex_unlock(&(rb_state2.lock_27));
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) - ((0xc9f190c) + (rb_state3.var_0));
            if ((rb_state3.var_0) == (0xf360e6f4)) {
                pthread_mutex_lock(&(rb_state3.lock_19));
                rb_state3.var_3 = 0x2ac0e7d7;
                rb_state3.var_4 = 0x7fa5f775;
                rb_state3.var_5 = (rb_state3.var_5) + ((0x6ba5f23e) ^ (0x3b87312a));
                rb_state3.var_6 = (rb_state3.var_6) - (0xd7879aa4);
                rb_state3.var_7 = (rb_state3.var_7) ^ (((0x2eb0bb12) + (0x2b35a885)) ^ (rb_state3.var_3));
                rb_state3.var_8 = (rb_state3.var_8) - (((((0x1b688a13) ^ (rb_state3.var_4)) + (rb_state3.var_4)) ^ (rb_state3.var_2)) - (rb_state3.var_3));
                rb_state3.var_9 = (rb_state3.var_9) ^ (((((0xd80dadad) ^ (rb_state3.var_6)) - (0x5f591ee4)) - (rb_state3.var_5)) + (rb_state3.var_5));
                rb_state3.var_10 = (rb_state3.var_10) + (rb_state3.var_7);
                rb_state3.var_11 = (rb_state3.var_11) - (((0xa69f5e33) - (0x68534943)) - (rb_state3.var_8));
                rb_state3.var_12 = (rb_state3.var_12) ^ (((0x8b8a51bf) ^ (rb_state3.var_9)) + (0x91e11743));
                rb_state3.var_13 = (rb_state3.var_13) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_10) : (0x8202bdce));
                rb_state3.var_14 = (rb_state3.var_14) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_11) : (0xbdd34ed3));
                rb_state3.var_15 = (rb_state3.var_15) ^ (rb_state3.var_12);
                rb_state3.var_16 = (rb_state3.var_16) ^ (((((0xe350dfe8) ^ (0x69b1a620)) ^ (rb_state3.var_8)) - (rb_state3.var_14)) ^ (rb_state3.var_13));
                rb_state3.var_17 = (rb_state3.var_17) ^ (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_15) : (0x114b300c));
                rb_state3.var_18 = (rb_state3.var_18) + (((((0x71df2a65) ^ (0xf56316ae)) + (0xa894ece)) - (rb_state3.var_16)) ^ (rb_state3.var_17));
                rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_18);
                pthread_mutex_unlock(&(rb_state3.lock_19));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x5afca484));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x1b5c1bbe));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x8f069989));
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0x4aa1e0ca)) {
                if ((rb_state9.var_1) != (0x0)) {
                    if (!((rb_state9.var_1) == (rb_state9.var_15))) {
                        racebench_trigger(9);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (2811239 < rb_input_size ? (uint32_t)rb_input[2811239] : 0x7ab98896);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xebca6a7d));
            rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
            if ((rb_state13.var_0) == (0x0)) {
                rb_state13.var_9 = 0xff8af198;
                rb_state13.var_10 = 0x3a792fa5;
                rb_state13.var_11 = (rb_state13.var_11) ^ (((0x1435094) ^ (rb_state13.var_10)) + (rb_state13.var_4));
                rb_state13.var_12 = (rb_state13.var_12) + (0x9d36cdd5);
                rb_state13.var_13 = (rb_state13.var_13) - (((((0xa056613a) + (rb_state13.var_5)) + (rb_state13.var_3)) - (rb_state13.var_9)) + (0x759a0f14));
                rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_11);
                rb_state13.var_15 = (rb_state13.var_15) + (((0x8c7a9bfe) ^ (rb_state13.var_12)) + (0x5e514cc3));
                rb_state13.var_16 = (rb_state13.var_16) ^ (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_14) : (0x170e6138));
                rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0xbb53918b));
                rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_17);
                rb_state13.var_1 = rb_state13.var_8;
            }
            if ((rb_state13.var_0) == (0x0)) {
                pthread_mutex_lock(&(rb_state13.lock_40));
                rb_state13.var_19 = 0xcb41d443;
                rb_state13.var_20 = (rb_state13.var_20) - (((0x76e7b4bd) ^ (rb_state13.var_19)) ^ (rb_state13.var_8));
                rb_state13.var_21 = (rb_state13.var_21) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_9) : (0xf78ab2be));
                rb_state13.var_22 = (rb_state13.var_22) + (0xa38e51ce);
                rb_state13.var_23 = (rb_state13.var_23) ^ (((0x74f903e3) + (0xe838bb0f)) - (rb_state13.var_6));
                rb_state13.var_24 = (rb_state13.var_24) ^ (((0x264cd305) + (rb_state13.var_7)) - (rb_state13.var_11));
                rb_state13.var_25 = (rb_state13.var_25) + (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_21) : (0x45d3a3f9));
                rb_state13.var_26 = (rb_state13.var_26) ^ (((rb_state13.var_22) == (0x0)) ? (rb_state13.var_23) : (0x9873417b));
                rb_state13.var_27 = (rb_state13.var_27) - (rb_state13.var_24);
                rb_state13.var_28 = (rb_state13.var_28) - (rb_state13.var_25);
                rb_state13.var_29 = (rb_state13.var_29) + (((0x1dd8eac7) + (rb_state13.var_26)) - (0xfd708be3));
                rb_state13.var_30 = (rb_state13.var_30) + (((0x45e76488) - (0x98010337)) - (rb_state13.var_27));
                rb_state13.var_31 = (rb_state13.var_31) + (((0xa13626c4) + (rb_state13.var_28)) - (rb_state13.var_12));
                rb_state13.var_32 = (rb_state13.var_32) + (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_29) : (0xe57545b3));
                rb_state13.var_33 = (rb_state13.var_33) ^ (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0xf686017f));
                rb_state13.var_34 = (rb_state13.var_34) - (((0x83c8e919) + (rb_state13.var_14)) ^ (rb_state13.var_32));
                rb_state13.var_35 = (rb_state13.var_35) ^ (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_33) : (0xed226bd8));
                rb_state13.var_36 = (rb_state13.var_36) - (((0xabd08bac) - (0x46c59615)) + (rb_state13.var_34));
                rb_state13.var_37 = (rb_state13.var_37) + (((0xa9bd6ed0) + (rb_state13.var_35)) + (rb_state13.var_16));
                rb_state13.var_38 = (rb_state13.var_38) + (((0x4b8698dc) + (rb_state13.var_36)) - (rb_state13.var_17));
                rb_state13.var_39 = (rb_state13.var_39) ^ (((((0x378fa60e) - (rb_state13.var_18)) ^ (rb_state13.var_37)) + (0xbc053775)) + (rb_state13.var_38));
                rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_39);
                rb_state13.var_1 = rb_state13.var_18;
                pthread_mutex_unlock(&(rb_state13.lock_40));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x674bfa5a));
            rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x7db7b33));
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0x0)) {
                rb_state16.var_2 = rb_state16.var_1;
            }
            if ((rb_state16.var_0) == (0x0)) {
                if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                    racebench_trigger(16);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_0);
            if ((rb_state17.var_0) == (0x0)) {
                rb_state17.var_3 = 0xfc781449;
                rb_state17.var_4 = (rb_state17.var_4) + (((((0x6cc4e6d4) - (rb_state17.var_3)) - (0x1fac81ee)) + (0xff580f5e)) + (rb_state17.var_2));
                rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_4);
            }
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x421f678e)) {
            if ((rb_state0.var_3) != (0x0)) {
                rb_state0.var_24 = !((rb_state0.var_1) == (rb_state0.var_2));
            }
        }
        rb_state0.var_10 = (rb_state0.var_10) ^ (2032163 < rb_input_size ? (uint32_t)rb_input[2032163] : 0x2d6290d1);
        if ((rb_state0.var_11) == (0x9f2fb308)) {
            rb_state0.var_37 = 0x2a9e49c7;
            rb_state0.var_38 = 0xc8f15ec;
            rb_state0.var_39 = (rb_state0.var_39) ^ (((0x3acabaf0) ^ (rb_state0.var_17)) - (0xca4566b2));
            rb_state0.var_40 = (rb_state0.var_40) - (((rb_state0.var_18) == (0x47a2340e)) ? (rb_state0.var_37) : (0xef0bbf4a));
            rb_state0.var_41 = (rb_state0.var_41) + (rb_state0.var_38);
            rb_state0.var_42 = (rb_state0.var_42) - (((rb_state0.var_19) == (0xead375f5)) ? (rb_state0.var_39) : (0x8fd5aa6b));
            rb_state0.var_43 = (rb_state0.var_43) ^ (rb_state0.var_40);
            rb_state0.var_44 = (rb_state0.var_44) ^ (((rb_state0.var_20) == (0xc6f9c4ae)) ? (rb_state0.var_41) : (0xf1b85278));
            rb_state0.var_45 = (rb_state0.var_45) + (((((0x4c27152c) + (rb_state0.var_42)) ^ (rb_state0.var_21)) + (rb_state0.var_43)) + (0x8d4375aa));
            rb_state0.var_46 = (rb_state0.var_46) + (((0x5eaa2451) + (rb_state0.var_44)) + (0xc7079bd1));
            rb_state0.var_47 = (rb_state0.var_47) ^ (((((0xe9cd041f) ^ (rb_state0.var_45)) ^ (rb_state0.var_46)) - (0xe75dc92a)) ^ (0x8f0c9d33));
            rb_state0.var_25 = (rb_state0.var_25) ^ (rb_state0.var_47);
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0x0)) {
            rb_state4.var_4 = 0xaa7668c4;
            rb_state4.var_5 = 0x43401a1b;
            rb_state4.var_6 = (rb_state4.var_6) + (((((0xc40e2e49) ^ (rb_state4.var_5)) ^ (rb_state4.var_4)) - (0x212f9edb)) - (0xabd6562b));
            rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_4) == (0xaa7668c4)) ? (rb_state4.var_3) : (0xa7426a03));
            rb_state4.var_8 = (rb_state4.var_8) - (rb_state4.var_6);
            rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_7) == (0xf7facdb7)) ? (rb_state4.var_8) : (0xf2db41e));
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x6df3afb3));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (0x4247a6e6);
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x9ac17730)) {
            rb_state8.var_2 = rb_state8.var_1;
        }
        if ((rb_state8.var_0) == (0x9ac17730)) {
            if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_7) == (0xb3331ff8)) {
            pthread_mutex_lock(&(rb_state10.lock_47));
            if ((rb_state10.var_18) != (0x0)) {
                if (!((rb_state10.var_18) == (rb_state10.var_36))) {
                    racebench_trigger(10);
                }
            }
            pthread_mutex_unlock(&(rb_state10.lock_47));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_0) == (0x33)) {
            rb_state12.var_3 = 0x1;
        }
        if ((rb_state12.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state12.lock_21));
            rb_state12.var_4 = 0xff990c67;
            rb_state12.var_5 = 0x55dd41e4;
            rb_state12.var_6 = (rb_state12.var_6) - (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0xb3c28a8));
            rb_state12.var_7 = (rb_state12.var_7) ^ (rb_state12.var_3);
            rb_state12.var_8 = (rb_state12.var_8) ^ (rb_state12.var_4);
            rb_state12.var_9 = (rb_state12.var_9) + (((rb_state12.var_4) == (0xff990c67)) ? (rb_state12.var_5) : (0xaf837ac7));
            rb_state12.var_10 = (rb_state12.var_10) + (rb_state12.var_6);
            rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_7);
            rb_state12.var_12 = (rb_state12.var_12) - (((0xde758795) + (rb_state12.var_8)) ^ (rb_state12.var_5));
            rb_state12.var_13 = (rb_state12.var_13) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_9) : (0x764b656f));
            rb_state12.var_14 = (rb_state12.var_14) + (((0x976a1078) + (rb_state12.var_10)) - (0x3234cf19));
            rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_11);
            rb_state12.var_16 = (rb_state12.var_16) - (((((0xf2445b1d) ^ (rb_state12.var_7)) ^ (rb_state12.var_12)) + (rb_state12.var_13)) ^ (rb_state12.var_8));
            rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_14);
            rb_state12.var_18 = (rb_state12.var_18) ^ (((rb_state12.var_15) == (0x1)) ? (rb_state12.var_16) : (0xdaaef729));
            rb_state12.var_19 = (rb_state12.var_19) + (rb_state12.var_17);
            rb_state12.var_20 = (rb_state12.var_20) ^ (((((0xc8b40056) - (rb_state12.var_19)) - (rb_state12.var_9)) ^ (rb_state12.var_18)) - (0x595fcf44));
            rb_state12.var_2 = (rb_state12.var_2) ^ (rb_state12.var_20);
            rb_state12.var_1 = rb_state12.var_2;
            pthread_mutex_unlock(&(rb_state12.lock_21));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x12123000)) {
            pthread_mutex_lock(&(rb_state19.lock_6));
            rb_state19.var_3 = 0x424151a7;
            rb_state19.var_4 = (rb_state19.var_4) ^ (rb_state19.var_2);
            rb_state19.var_5 = (rb_state19.var_5) - (((((0xe06ab2c3) - (0x48baa490)) + (rb_state19.var_4)) - (rb_state19.var_3)) + (rb_state19.var_4));
            rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_5);
            pthread_mutex_unlock(&(rb_state19.lock_6));
        }
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
    rb_state0.var_0 = (rb_state0.var_0) ^ ((0xe2e464bf) ^ (0x9c8f7500));
    if ((rb_state0.var_0) == (0x421f678e)) {
        rb_state0.var_3 = 0x1;
    }
    if ((rb_state0.var_0) == (0x421f678e)) {
        pthread_mutex_lock(&(rb_state0.lock_23));
        rb_state0.var_4 = (rb_state0.var_4) - ((0x1d33f467) ^ (rb_state0.var_4));
        rb_state0.var_5 = (rb_state0.var_5) ^ (1652407 < rb_input_size ? (uint32_t)rb_input[1652407] : 0xc8df5f4d);
        rb_state0.var_6 = (rb_state0.var_6) - (((0x4cf4da0) + (0xb70fb140)) + (rb_state0.var_1));
        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_5);
        rb_state0.var_8 = (rb_state0.var_8) + (((((0x56b9650c) ^ (rb_state0.var_2)) - (rb_state0.var_6)) - (0x41d67f4f)) ^ (rb_state0.var_3));
        rb_state0.var_9 = (rb_state0.var_9) - (((0xf8095545) ^ (rb_state0.var_4)) - (0x8efd6b07));
        rb_state0.var_10 = (rb_state0.var_10) ^ (rb_state0.var_5);
        rb_state0.var_11 = (rb_state0.var_11) - (((0x24f14da7) + (rb_state0.var_7)) ^ (rb_state0.var_6));
        rb_state0.var_12 = (rb_state0.var_12) + (rb_state0.var_7);
        rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_8) : (0xecd912ba));
        rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_9);
        rb_state0.var_15 = (rb_state0.var_15) + (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0x7e7de2be));
        rb_state0.var_16 = (rb_state0.var_16) ^ (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_13) : (0xead375f5));
        rb_state0.var_17 = (rb_state0.var_17) - (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_14) : (0x848a36e1));
        rb_state0.var_18 = (rb_state0.var_18) + (((0xd39364dc) - (0x99b38e2c)) ^ (rb_state0.var_15));
        rb_state0.var_19 = (rb_state0.var_19) + (rb_state0.var_16);
        rb_state0.var_20 = (rb_state0.var_20) + (((0x811a7211) - (rb_state0.var_17)) + (0xc1551bbc));
        rb_state0.var_21 = (rb_state0.var_21) + (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_19) : (0xe9bd5c16));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xbba647fe));
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_22);
        rb_state0.var_1 = rb_state0.var_2;
        pthread_mutex_unlock(&(rb_state0.lock_23));
    }
    if ((rb_state0.var_11) == (0x9f2fb308)) {
        pthread_mutex_lock(&(rb_state0.lock_48));
        rb_state0.var_27 = (rb_state0.var_27) - (0xdeed1375);
        rb_state0.var_28 = (rb_state0.var_28) - (rb_state0.var_12);
        rb_state0.var_29 = (rb_state0.var_29) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_13) : (0x7347e9bd));
        rb_state0.var_30 = (rb_state0.var_30) ^ (rb_state0.var_14);
        rb_state0.var_31 = (rb_state0.var_31) + (((0x11d53eed) + (rb_state0.var_27)) - (0x921813d9));
        rb_state0.var_32 = (rb_state0.var_32) + (((((0xfce7ca98) + (0xc9d9dfc9)) - (0x7d551e7)) - (rb_state0.var_29)) + (rb_state0.var_28));
        rb_state0.var_33 = (rb_state0.var_33) - (((0xc2b7a2a) + (0xb12519dc)) ^ (rb_state0.var_30));
        rb_state0.var_34 = (rb_state0.var_34) ^ (((((0xf573dc13) - (0xc9bd75b4)) - (rb_state0.var_31)) ^ (rb_state0.var_32)) - (0xd80d2a4b));
        rb_state0.var_35 = (rb_state0.var_35) - (rb_state0.var_33);
        rb_state0.var_36 = (rb_state0.var_36) - (((((0xf76046a6) ^ (rb_state0.var_34)) + (rb_state0.var_35)) ^ (rb_state0.var_16)) - (0x87054075));
        rb_state0.var_26 = (rb_state0.var_26) ^ (rb_state0.var_36);
        rb_state0.var_25 = rb_state0.var_26;
        pthread_mutex_unlock(&(rb_state0.lock_48));
    }
    if ((rb_state0.var_11) == (0x9f2fb308)) {
        if (!((rb_state0.var_25) == (rb_state0.var_26))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x518847b)) {
        pthread_mutex_lock(&(rb_state1.lock_27));
        rb_state1.var_3 = 0x52cd4d7a;
        rb_state1.var_4 = 0xc684cfb2;
        rb_state1.var_5 = (rb_state1.var_5) + (((((0xb26436f3) + (0xb83c40a4)) ^ (0xbd9fb35c)) + (rb_state1.var_4)) - (rb_state1.var_1));
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_2) : (0x887bb113));
        rb_state1.var_7 = (rb_state1.var_7) ^ (((0xd5ba83cd) ^ (rb_state1.var_3)) - (rb_state1.var_4));
        rb_state1.var_8 = (rb_state1.var_8) ^ (rb_state1.var_5);
        rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_6) : (0x668d247a));
        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0xe9cc40b0));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0xa906f1c1));
        rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0xb03e886a));
        rb_state1.var_13 = (rb_state1.var_13) ^ (((((0xb355dbdd) ^ (rb_state1.var_12)) ^ (0xa1b606ac)) - (0xffcf604b)) + (rb_state1.var_11));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_13);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_27));
    }
    if ((rb_state1.var_0) == (0x518847b)) {
        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x97ed02c1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xdfc58244));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_41));
        rb_state5.var_12 = 0x81e10e4a;
        rb_state5.var_13 = (rb_state5.var_13) ^ ((0x5f3532af) + (0x580b16e0));
        rb_state5.var_14 = (rb_state5.var_14) ^ ((0xc6c1fe45) ^ (0xf18db259));
        rb_state5.var_15 = (rb_state5.var_15) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_5) : (0x55ff3749));
        rb_state5.var_16 = (rb_state5.var_16) + (rb_state5.var_12);
        rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_4) : (0x831c4fc0));
        rb_state5.var_18 = (rb_state5.var_18) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_13) : (0x5c8f267));
        rb_state5.var_19 = (rb_state5.var_19) ^ (rb_state5.var_14);
        rb_state5.var_20 = (rb_state5.var_20) ^ (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_16) : (0x93e03e10));
        rb_state5.var_21 = (rb_state5.var_21) ^ (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_17) : (0x6cdc5780));
        rb_state5.var_22 = (rb_state5.var_22) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_18) : (0x97ce03e7));
        rb_state5.var_23 = (rb_state5.var_23) - (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_19) : (0x9d77135e));
        rb_state5.var_24 = (rb_state5.var_24) - (rb_state5.var_20);
        rb_state5.var_25 = (rb_state5.var_25) ^ (((((0x699f096f) ^ (0x6384b0de)) ^ (rb_state5.var_22)) - (rb_state5.var_21)) ^ (rb_state5.var_12));
        rb_state5.var_26 = (rb_state5.var_26) - (((0x6346a3c4) - (0xeefdef28)) + (rb_state5.var_23));
        rb_state5.var_27 = (rb_state5.var_27) - (rb_state5.var_24);
        rb_state5.var_28 = (rb_state5.var_28) + (((rb_state5.var_25) == (0x0)) ? (rb_state5.var_26) : (0x4024ebc2));
        rb_state5.var_29 = (rb_state5.var_29) + (rb_state5.var_27);
        rb_state5.var_30 = (rb_state5.var_30) ^ (((((0xb192117b) - (rb_state5.var_13)) + (rb_state5.var_29)) + (rb_state5.var_14)) + (rb_state5.var_28));
        rb_state5.var_11 = (rb_state5.var_11) + (rb_state5.var_30);
        rb_state5.var_1 = rb_state5.var_11;
        pthread_mutex_unlock(&(rb_state5.lock_41));
    }
    if ((rb_state5.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_41));
        rb_state5.var_32 = 0x1bbc6c97;
        rb_state5.var_33 = (rb_state5.var_33) - (((((0x7eefa244) - (rb_state5.var_16)) + (rb_state5.var_17)) - (0x79e262b6)) ^ (rb_state5.var_15));
        rb_state5.var_34 = (rb_state5.var_34) - (rb_state5.var_32);
        rb_state5.var_35 = (rb_state5.var_35) ^ ((0xf7e1227c) + (rb_state5.var_18));
        rb_state5.var_36 = (rb_state5.var_36) ^ (rb_state5.var_33);
        rb_state5.var_37 = (rb_state5.var_37) + (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_34) : (0xa51aa3b5));
        rb_state5.var_38 = (rb_state5.var_38) + (rb_state5.var_35);
        rb_state5.var_39 = (rb_state5.var_39) - (((rb_state5.var_36) == (0x0)) ? (rb_state5.var_37) : (0xcd062e20));
        rb_state5.var_40 = (rb_state5.var_40) - (((rb_state5.var_38) == (0x0)) ? (rb_state5.var_39) : (0xbfb074c1));
        rb_state5.var_31 = (rb_state5.var_31) ^ (rb_state5.var_40);
        rb_state5.var_1 = rb_state5.var_31;
        pthread_mutex_unlock(&(rb_state5.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xbdb8591a)) {
        pthread_mutex_lock(&(rb_state6.lock_13));
        rb_state6.var_3 = 0x3041bcda;
        rb_state6.var_4 = (rb_state6.var_4) + (rb_state6.var_2);
        rb_state6.var_5 = (rb_state6.var_5) + (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_3) : (0xc6e96239));
        rb_state6.var_6 = (rb_state6.var_6) ^ (((((0xb19be4e4) ^ (rb_state6.var_5)) + (0x886eacd6)) - (0x6fc04b18)) - (rb_state6.var_4));
        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_6);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_13));
    }
    if ((rb_state6.var_0) == (0xbdb8591a)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xb29f4468);
    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x4d60bb98)) ? (rb_state8.var_0) : (0x5caf4cb8));
    if ((rb_state8.var_0) == (0x9ac17730)) {
        rb_state8.var_3 = 0xb3512675;
        rb_state8.var_4 = 0x37016f3b;
        rb_state8.var_5 = (rb_state8.var_5) - (rb_state8.var_3);
        rb_state8.var_6 = (rb_state8.var_6) + (((((0x411910c8) - (0x34067d5d)) + (0x86544a5f)) - (rb_state8.var_4)) - (rb_state8.var_2));
        rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_5) == (0x4caed98b)) ? (rb_state8.var_6) : (0x5bca658f));
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0x1e820fc4) - (0x450c6409));
    rb_state10.var_0 = (rb_state10.var_0) ^ ((0xa0651277) ^ (0x3b64111a));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0xab2b0685)) {
        pthread_mutex_lock(&(rb_state11.lock_25));
        rb_state11.var_3 = 0xd3b1c8ad;
        rb_state11.var_4 = (rb_state11.var_4) ^ (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_3) : (0xef3af96));
        rb_state11.var_5 = (rb_state11.var_5) - (2147284 < rb_input_size ? (uint32_t)rb_input[2147284] : 0xe95afa29);
        rb_state11.var_6 = (rb_state11.var_6) - (3644862 < rb_input_size ? (uint32_t)rb_input[3644862] : 0x8498274e);
        rb_state11.var_7 = (rb_state11.var_7) + (rb_state11.var_2);
        rb_state11.var_8 = (rb_state11.var_8) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_1) : (0xf807e973));
        rb_state11.var_9 = (rb_state11.var_9) - (rb_state11.var_4);
        rb_state11.var_10 = (rb_state11.var_10) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0x3d959061));
        rb_state11.var_11 = (rb_state11.var_11) ^ (((((0xebd1eb0d) ^ (rb_state11.var_8)) + (rb_state11.var_5)) + (rb_state11.var_7)) - (rb_state11.var_6));
        rb_state11.var_12 = (rb_state11.var_12) ^ (((((0x31551542) ^ (rb_state11.var_9)) ^ (0x20693162)) ^ (0x742a5bf9)) + (rb_state11.var_10));
        rb_state11.var_13 = (rb_state11.var_13) - (((0x87b8cf06) ^ (0xad8fb582)) - (rb_state11.var_11));
        rb_state11.var_14 = (rb_state11.var_14) ^ (((((0xd33b784a) + (rb_state11.var_7)) - (rb_state11.var_12)) - (rb_state11.var_8)) - (rb_state11.var_13));
        rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_14);
        rb_state11.var_1 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_25));
    }
    if ((rb_state11.var_0) == (0xab2b0685)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_21));
        if ((rb_state12.var_3) != (0x0)) {
            rb_state12.var_22 = !((rb_state12.var_1) == (rb_state12.var_2));
        }
        pthread_mutex_unlock(&(rb_state12.lock_21));
    }
    rb_state12.var_10 = (rb_state12.var_10) - (3205960 < rb_input_size ? (uint32_t)rb_input[3205960] : 0x919aa6f);
    if ((rb_state12.var_11) == (0x1)) {
        rb_state12.var_23 = rb_state12.var_12;
    }
    if ((rb_state12.var_11) == (0x1)) {
        if (!((rb_state12.var_12) == (rb_state12.var_23))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_2 = rb_state15.var_1;
    }
    if ((rb_state15.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_7));
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
        pthread_mutex_unlock(&(rb_state15.lock_7));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x0)) {
        rb_state18.var_15 = !((rb_state18.var_2) == (rb_state18.var_1));
    }
    rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_3);
    if ((rb_state18.var_4) == (0xdbd1504e)) {
        pthread_mutex_lock(&(rb_state18.lock_29));
        rb_state18.var_18 = (rb_state18.var_18) ^ (0x4f97c769);
        rb_state18.var_19 = (rb_state18.var_19) + (rb_state18.var_7);
        rb_state18.var_20 = (rb_state18.var_20) + (((((0x4ea3835c) ^ (rb_state18.var_8)) ^ (rb_state18.var_10)) + (rb_state18.var_5)) + (rb_state18.var_9));
        rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_19) : (0xbe842b34));
        rb_state18.var_22 = (rb_state18.var_22) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_20) : (0xcda1fb7));
        rb_state18.var_23 = (rb_state18.var_23) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_21) : (0x129def7c));
        rb_state18.var_24 = (rb_state18.var_24) ^ (((((0x166a645e) ^ (rb_state18.var_13)) + (rb_state18.var_22)) ^ (rb_state18.var_23)) + (0x8d53a6c3));
        rb_state18.var_17 = (rb_state18.var_17) + (rb_state18.var_24);
        rb_state18.var_16 = rb_state18.var_17;
        pthread_mutex_unlock(&(rb_state18.lock_29));
    }
    if ((rb_state18.var_4) == (0xdbd1504e)) {
        if (!((rb_state18.var_16) == (rb_state18.var_17))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x790497a0));
    if ((rb_state19.var_0) == (0x12123000)) {
        rb_state19.var_2 = rb_state19.var_1;
    }
    if ((rb_state19.var_0) == (0x12123000)) {
        if (!((rb_state19.var_1) == (rb_state19.var_2))) {
            racebench_trigger(19);
        }
    }
    #endif
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0x32f890af) - (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (0x518847b);
    if ((rb_state1.var_0) == (0x518847b)) {
        rb_state1.var_14 = 0x8f958a59;
        rb_state1.var_15 = 0x7dc4170b;
        rb_state1.var_16 = (rb_state1.var_16) + ((0x7823e575) - (0x8817f6ef));
        rb_state1.var_17 = (rb_state1.var_17) ^ (rb_state1.var_9);
        rb_state1.var_18 = (rb_state1.var_18) ^ (((rb_state1.var_10) == (0x1633bf50)) ? (rb_state1.var_8) : (0x65507cb5));
        rb_state1.var_19 = (rb_state1.var_19) ^ (((0xe5d0ef0b) - (rb_state1.var_11)) ^ (rb_state1.var_14));
        rb_state1.var_20 = (rb_state1.var_20) - (((((0xe91513c4) - (0x36358ee7)) + (rb_state1.var_15)) - (rb_state1.var_12)) + (rb_state1.var_16));
        rb_state1.var_21 = (rb_state1.var_21) + (((rb_state1.var_17) == (0x668d247a)) ? (rb_state1.var_18) : (0xdd624236));
        rb_state1.var_22 = (rb_state1.var_22) ^ (((rb_state1.var_13) == (0x4c14e691)) ? (rb_state1.var_19) : (0x63bd8402));
        rb_state1.var_23 = (rb_state1.var_23) + (rb_state1.var_20);
        rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_21);
        rb_state1.var_25 = (rb_state1.var_25) - (((rb_state1.var_22) == (0xb35c7713)) ? (rb_state1.var_23) : (0x57a51133));
        rb_state1.var_26 = (rb_state1.var_26) - (((((0xeec8d65f) + (0x4ad17f78)) ^ (rb_state1.var_24)) + (rb_state1.var_14)) + (rb_state1.var_25));
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (2357053 < rb_input_size ? (uint32_t)rb_input[2357053] : 0x998f9cb4);
    if ((rb_state2.var_0) == (0x97ed02ef)) {
        if ((rb_state2.var_1) != (0x0)) {
            if (!((rb_state2.var_1) == (rb_state2.var_16))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_10));
        rb_state4.var_3 = (rb_state4.var_3) ^ (((((0x25281057) - (0x49ff78d9)) + (0x1cd23639)) ^ (rb_state4.var_1)) - (rb_state4.var_2));
        rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_3);
        rb_state4.var_1 = rb_state4.var_2;
        pthread_mutex_unlock(&(rb_state4.lock_10));
    }
    if ((rb_state4.var_0) == (0x0)) {
        if (!((rb_state4.var_1) == (rb_state4.var_2))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x0)) {
        if ((rb_state5.var_1) != (0x0)) {
            if (!((rb_state5.var_1) == (rb_state5.var_31))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x4d50fcee);
    if ((rb_state7.var_0) == (0xe7f2f70a)) {
        pthread_mutex_lock(&(rb_state7.lock_24));
        rb_state7.var_13 = 0x6bb5910e;
        rb_state7.var_14 = 0x983ea957;
        rb_state7.var_15 = (rb_state7.var_15) ^ (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_14) : (0x16a0a86c));
        rb_state7.var_16 = (rb_state7.var_16) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_10) : (0x2efcfba2));
        rb_state7.var_17 = (rb_state7.var_17) - (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_13) : (0xe48aab29));
        rb_state7.var_18 = (rb_state7.var_18) - (rb_state7.var_15);
        rb_state7.var_19 = (rb_state7.var_19) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_16) : (0xac9fd392));
        rb_state7.var_20 = (rb_state7.var_20) - (((((0xd82c3ba3) ^ (rb_state7.var_17)) ^ (rb_state7.var_15)) - (rb_state7.var_16)) - (rb_state7.var_18));
        rb_state7.var_21 = (rb_state7.var_21) ^ (((0xa7756a4a) - (rb_state7.var_19)) ^ (rb_state7.var_17));
        rb_state7.var_22 = (rb_state7.var_22) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_20) : (0xc97ed69c));
        rb_state7.var_23 = (rb_state7.var_23) ^ (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_22) : (0x7a97d6fd));
        rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_23);
        pthread_mutex_unlock(&(rb_state7.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (0x137c2e5b);
    rb_state9.var_0 = (rb_state9.var_0) - (0xc8da4d91);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_7) == (0xb3331ff8)) {
        rb_state10.var_30 = 0xeba8c9cb;
        rb_state10.var_31 = 0x87390f56;
        rb_state10.var_32 = (rb_state10.var_32) ^ (((0xad34829c) + (rb_state10.var_31)) - (0xedb9abb8));
        rb_state10.var_33 = (rb_state10.var_33) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_30) : (0x3b89e33));
        rb_state10.var_34 = (rb_state10.var_34) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_32) : (0xa1c20f6c));
        rb_state10.var_35 = (rb_state10.var_35) ^ (((rb_state10.var_33) == (0x0)) ? (rb_state10.var_34) : (0xe7bc798f));
        rb_state10.var_29 = (rb_state10.var_29) + (rb_state10.var_35);
        rb_state10.var_18 = rb_state10.var_29;
    }
    if ((rb_state10.var_7) == (0xb3331ff8)) {
        pthread_mutex_lock(&(rb_state10.lock_47));
        rb_state10.var_37 = 0x9e203fd2;
        rb_state10.var_38 = 0xffc2529f;
        rb_state10.var_39 = 0x655ca25e;
        rb_state10.var_40 = (rb_state10.var_40) - (rb_state10.var_18);
        rb_state10.var_41 = (rb_state10.var_41) + (((0x131d4293) - (rb_state10.var_19)) - (rb_state10.var_39));
        rb_state10.var_42 = (rb_state10.var_42) - (((rb_state10.var_20) == (0x0)) ? (rb_state10.var_37) : (0xf8f1c93d));
        rb_state10.var_43 = (rb_state10.var_43) + (((((0x95985188) + (rb_state10.var_22)) - (rb_state10.var_40)) - (rb_state10.var_38)) + (rb_state10.var_21));
        rb_state10.var_44 = (rb_state10.var_44) + (rb_state10.var_41);
        rb_state10.var_45 = (rb_state10.var_45) ^ (((rb_state10.var_42) == (0x0)) ? (rb_state10.var_43) : (0xc1c64c2));
        rb_state10.var_46 = (rb_state10.var_46) - (((rb_state10.var_44) == (0x0)) ? (rb_state10.var_45) : (0x172dba57));
        rb_state10.var_36 = (rb_state10.var_36) + (rb_state10.var_46);
        rb_state10.var_18 = rb_state10.var_36;
        pthread_mutex_unlock(&(rb_state10.lock_47));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ ((0xb9ddaa4) + (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        if ((rb_state13.var_1) != (0x0)) {
            if (!((rb_state13.var_1) == (rb_state13.var_18))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (3510749 < rb_input_size ? (uint32_t)rb_input[3510749] : 0x95fed228);
    if ((rb_state17.var_0) == (0x35)) {
        rb_state17.var_2 = rb_state17.var_1;
    }
    if ((rb_state17.var_0) == (0x0)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (1317334 < rb_input_size ? (uint32_t)rb_input[1317334] : 0x14b1be50);
    rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0xffffffc7)) ? (rb_state18.var_0) : (0x7116055b));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x76148467);
    #endif
    ProcID = ProcID % NumProcs;

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_2 = rb_state3.var_1;
    }
    if ((rb_state3.var_0) == (0xf360e6f4)) {
        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xbdb8591a)) {
        pthread_mutex_lock(&(rb_state6.lock_13));
        rb_state6.var_7 = 0x5cb7a765;
        rb_state6.var_8 = 0xfe884de2;
        rb_state6.var_9 = 0x7bb789e7;
        rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_8);
        rb_state6.var_11 = (rb_state6.var_11) + (((((0xc200ff3e) - (0xc010fa36)) + (rb_state6.var_3)) - (rb_state6.var_7)) + (rb_state6.var_9));
        rb_state6.var_12 = (rb_state6.var_12) + (((((0x7d44bbf5) + (rb_state6.var_10)) ^ (rb_state6.var_11)) - (0xd8c205b7)) - (rb_state6.var_4));
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_12);
        pthread_mutex_unlock(&(rb_state6.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (849205 < rb_input_size ? (uint32_t)rb_input[849205] : 0xeb79a5);
    if ((rb_state7.var_0) == (0x9aa1fa1c)) {
        pthread_mutex_lock(&(rb_state7.lock_24));
        rb_state7.var_3 = 0x28abbb9f;
        rb_state7.var_4 = (rb_state7.var_4) - (((((0xbd7dcc2) - (0x2db110ad)) ^ (rb_state7.var_1)) - (rb_state7.var_3)) + (rb_state7.var_3));
        rb_state7.var_5 = (rb_state7.var_5) ^ ((0xe14210d6) + (0x7562945c));
        rb_state7.var_6 = (rb_state7.var_6) - (((((0x81929127) + (rb_state7.var_4)) - (rb_state7.var_5)) + (rb_state7.var_2)) + (rb_state7.var_4));
        rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0xc235c828));
        rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_6) : (0xdcb55eeb));
        rb_state7.var_9 = (rb_state7.var_9) - (((0x199ad5ae) - (rb_state7.var_7)) + (rb_state7.var_8));
        rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_8);
        rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_9) : (0x3b65db09));
        rb_state7.var_12 = (rb_state7.var_12) ^ (((((0x9017e5e7) + (rb_state7.var_10)) + (0xd263fdae)) - (rb_state7.var_11)) ^ (0x643db1bb));
        rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_12);
        rb_state7.var_1 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_24));
    }
    if ((rb_state7.var_0) == (0xe7f2f70a)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x4aa1e0ca)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        rb_state9.var_5 = 0x1cc59dd4;
        rb_state9.var_6 = 0x3d3155a;
        rb_state9.var_7 = (rb_state9.var_7) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_6) : (0xbfea2fa1));
        rb_state9.var_8 = (rb_state9.var_8) ^ (rb_state9.var_5);
        rb_state9.var_9 = (rb_state9.var_9) + (rb_state9.var_3);
        rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_5) == (0x1cc59dd4)) ? (rb_state9.var_7) : (0x28d6a3da));
        rb_state9.var_11 = (rb_state9.var_11) ^ (((rb_state9.var_6) == (0x3d3155a)) ? (rb_state9.var_8) : (0x70b7e71f));
        rb_state9.var_12 = (rb_state9.var_12) - (((0x56ecfcf5) + (rb_state9.var_9)) + (rb_state9.var_7));
        rb_state9.var_13 = (rb_state9.var_13) - (((((0x8902dede) + (rb_state9.var_9)) - (rb_state9.var_8)) ^ (rb_state9.var_11)) + (rb_state9.var_10));
        rb_state9.var_14 = (rb_state9.var_14) ^ (((((0x18cfe964) + (0xa6ec165e)) + (rb_state9.var_12)) + (0x7c4681d5)) + (rb_state9.var_13));
        rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_14);
        rb_state9.var_1 = rb_state9.var_4;
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    if ((rb_state9.var_0) == (0x4aa1e0ca)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        rb_state9.var_16 = 0x2304bd84;
        rb_state9.var_17 = (rb_state9.var_17) ^ (((rb_state9.var_13) == (0x92da387c)) ? (rb_state9.var_12) : (0xbb51e930));
        rb_state9.var_18 = (rb_state9.var_18) + (0x7c41813d);
        rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_14) == (0x7bc2d278)) ? (rb_state9.var_16) : (0x40ce72ba));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((0xc4d8ae1b) + (rb_state9.var_11)) ^ (0x63d08e55));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_10) : (0xc7c54179));
        rb_state9.var_22 = (rb_state9.var_22) ^ (((0x9dc6e7e5) ^ (rb_state9.var_17)) + (0x334bc508));
        rb_state9.var_23 = (rb_state9.var_23) ^ (((rb_state9.var_18) == (0x7c41813d)) ? (rb_state9.var_19) : (0xd22df550));
        rb_state9.var_24 = (rb_state9.var_24) + (((((0x6a16cb85) + (rb_state9.var_20)) + (0xc50e923b)) ^ (rb_state9.var_21)) ^ (0x35bf29c5));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((rb_state9.var_22) == (0x646cc488)) ? (rb_state9.var_23) : (0xf7ed5465));
        rb_state9.var_26 = (rb_state9.var_26) ^ (((0x20023907) + (rb_state9.var_24)) ^ (rb_state9.var_16));
        rb_state9.var_27 = (rb_state9.var_27) ^ (((0xffe48929) ^ (rb_state9.var_17)) - (rb_state9.var_25));
        rb_state9.var_28 = (rb_state9.var_28) - (((0xf9e7a4ce) + (rb_state9.var_26)) ^ (0xbcfea05f));
        rb_state9.var_29 = (rb_state9.var_29) - (((((0x2d64c570) ^ (0xa85f00b9)) ^ (rb_state9.var_28)) ^ (rb_state9.var_18)) - (rb_state9.var_27));
        rb_state9.var_15 = (rb_state9.var_15) ^ (rb_state9.var_29);
        rb_state9.var_1 = rb_state9.var_15;
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        rb_state10.var_3 = 0x1;
    }
    if ((rb_state10.var_0) == (0x0)) {
        if ((rb_state10.var_3) != (0x0)) {
            rb_state10.var_17 = !((rb_state10.var_1) == (rb_state10.var_2));
        }
    }
    if ((rb_state10.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state10.lock_16));
        rb_state10.var_4 = 0xa6198a98;
        rb_state10.var_5 = (rb_state10.var_5) + (((0x7638f25f) ^ (rb_state10.var_2)) ^ (0x39938a31));
        rb_state10.var_6 = (rb_state10.var_6) ^ (785067 < rb_input_size ? (uint32_t)rb_input[785067] : 0xfd80fdae);
        rb_state10.var_7 = (rb_state10.var_7) - (((rb_state10.var_3) == (0x1)) ? (rb_state10.var_1) : (0xa407330d));
        rb_state10.var_8 = (rb_state10.var_8) ^ (rb_state10.var_4);
        rb_state10.var_9 = (rb_state10.var_9) - (((0x85673a6d) - (0x8288a207)) - (rb_state10.var_5));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_6) == (0x34)) ? (rb_state10.var_7) : (0x20120a9c));
        rb_state10.var_11 = (rb_state10.var_11) - (((rb_state10.var_4) == (0xa6198a98)) ? (rb_state10.var_8) : (0x30b943ac));
        rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_9);
        rb_state10.var_13 = (rb_state10.var_13) + (((0x2a1735c3) + (rb_state10.var_10)) - (rb_state10.var_5));
        rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_11) == (0x59e67568)) ? (rb_state10.var_12) : (0xd0d9126d));
        rb_state10.var_15 = (rb_state10.var_15) ^ (((rb_state10.var_13) == (0xda6bbd55)) ? (rb_state10.var_14) : (0xccb39261));
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_15);
        rb_state10.var_1 = rb_state10.var_2;
        pthread_mutex_unlock(&(rb_state10.lock_16));
    }
    rb_state10.var_6 = (rb_state10.var_6) ^ (0xa8018383);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0xb9ddaa4)) ? (rb_state11.var_0) : (0x7548cbe1));
    if ((rb_state11.var_0) == (0xab2b0685)) {
        rb_state11.var_15 = 0x8629454f;
        rb_state11.var_16 = (rb_state11.var_16) + (((0x27f960ba) ^ (0xea6acf30)) ^ (rb_state11.var_10));
        rb_state11.var_17 = (rb_state11.var_17) - (rb_state11.var_11);
        rb_state11.var_18 = (rb_state11.var_18) - (3219079 < rb_input_size ? (uint32_t)rb_input[3219079] : 0xc0bdef2b);
        rb_state11.var_19 = (rb_state11.var_19) ^ (((((0xe2afcb8b) ^ (rb_state11.var_12)) + (rb_state11.var_15)) ^ (rb_state11.var_13)) ^ (rb_state11.var_9));
        rb_state11.var_20 = (rb_state11.var_20) - (((0x696fad83) + (0xecac3979)) ^ (rb_state11.var_16));
        rb_state11.var_21 = (rb_state11.var_21) + (((rb_state11.var_14) == (0x0)) ? (rb_state11.var_17) : (0x27e13fcc));
        rb_state11.var_22 = (rb_state11.var_22) - (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x1c709a4a));
        rb_state11.var_23 = (rb_state11.var_23) ^ (((((0x26785005) ^ (rb_state11.var_20)) ^ (rb_state11.var_16)) - (rb_state11.var_15)) ^ (rb_state11.var_21));
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_23) : (0xbcdeccdb));
        rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_11) == (0x1)) {
        rb_state12.var_24 = 0xade0b61c;
        rb_state12.var_25 = (rb_state12.var_25) ^ (((((0x3e7f69e) - (rb_state12.var_16)) ^ (rb_state12.var_15)) - (rb_state12.var_24)) - (rb_state12.var_14));
        rb_state12.var_26 = (rb_state12.var_26) - (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_13) : (0x5ea83845));
        rb_state12.var_27 = (rb_state12.var_27) + ((0x870c06bd) ^ (0xff4c5152));
        rb_state12.var_28 = (rb_state12.var_28) + (((0x40fd1146) ^ (0xd901b773)) ^ (rb_state12.var_25));
        rb_state12.var_29 = (rb_state12.var_29) + (((((0x5f1393ff) - (rb_state12.var_26)) ^ (rb_state12.var_18)) - (rb_state12.var_19)) - (rb_state12.var_27));
        rb_state12.var_30 = (rb_state12.var_30) - (rb_state12.var_28);
        rb_state12.var_31 = (rb_state12.var_31) ^ (rb_state12.var_29);
        rb_state12.var_32 = (rb_state12.var_32) ^ (rb_state12.var_30);
        rb_state12.var_33 = (rb_state12.var_33) ^ (rb_state12.var_31);
        rb_state12.var_34 = (rb_state12.var_34) - (((((0xc3fbad9) - (rb_state12.var_20)) - (rb_state12.var_33)) - (rb_state12.var_32)) - (0x4d54a5cf));
        rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_34);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_3 = 0xe8b04db7;
        rb_state15.var_4 = 0x83260033;
        rb_state15.var_5 = (rb_state15.var_5) - (((((0x1780d203) + (rb_state15.var_4)) ^ (0x748f4296)) - (rb_state15.var_3)) + (rb_state15.var_3));
        rb_state15.var_6 = (rb_state15.var_6) - (((((0x78ee03e2) - (rb_state15.var_5)) ^ (0xc90bcd6e)) + (rb_state15.var_2)) ^ (rb_state15.var_4));
        rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_3 = 0xff01b51c;
        rb_state16.var_4 = 0x6f5001a;
        rb_state16.var_5 = (rb_state16.var_5) - (rb_state16.var_3);
        rb_state16.var_6 = (rb_state16.var_6) - (((rb_state16.var_4) == (0x6f5001a)) ? (rb_state16.var_3) : (0x599a00be));
        rb_state16.var_7 = (rb_state16.var_7) ^ (((0xcd2d9b2b) + (0x42b092cb)) + (rb_state16.var_2));
        rb_state16.var_8 = (rb_state16.var_8) - (((((0xdc6de59e) - (0xa515c3f4)) - (rb_state16.var_5)) - (rb_state16.var_6)) + (rb_state16.var_4));
        rb_state16.var_9 = (rb_state16.var_9) ^ (((((0x60794b02) - (rb_state16.var_8)) ^ (rb_state16.var_7)) + (rb_state16.var_5)) - (0xa6da0ab0));
        rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_9);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_14));
        rb_state18.var_3 = 0xde77dbdd;
        rb_state18.var_4 = 0xdbd1504e;
        rb_state18.var_5 = 0x5d7822f8;
        rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_3) : (0x3115d94d));
        rb_state18.var_7 = (rb_state18.var_7) + (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_4) : (0xf064b5c4));
        rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_6) : (0x19346af4));
        rb_state18.var_9 = (rb_state18.var_9) - (rb_state18.var_7);
        rb_state18.var_10 = (rb_state18.var_10) + (rb_state18.var_8);
        rb_state18.var_11 = (rb_state18.var_11) - (((0x5f0a352e) - (0x459b8300)) ^ (rb_state18.var_9));
        rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_10);
        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0x9c5e1e88));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_13);
        rb_state18.var_2 = rb_state18.var_1;
        pthread_mutex_unlock(&(rb_state18.lock_14));
    }
    if ((rb_state18.var_4) == (0xdbd1504e)) {
        rb_state18.var_25 = 0x8487bd52;
        rb_state18.var_26 = (rb_state18.var_26) ^ (rb_state18.var_16);
        rb_state18.var_27 = (rb_state18.var_27) - (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_25) : (0x42397729));
        rb_state18.var_28 = (rb_state18.var_28) - (((((0xf5bc9d03) + (0x68064ae0)) - (rb_state18.var_26)) + (rb_state18.var_17)) - (rb_state18.var_27));
        rb_state18.var_16 = (rb_state18.var_16) ^ (rb_state18.var_28);
    }
    #endif
    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}