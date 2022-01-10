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
            rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
            rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_1);
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) + ((0xc586a5a2) ^ (0x8bc8d991));
            rb_state1.var_1 = (rb_state1.var_1) - (2829557 < rb_input_size ? (uint32_t)rb_input[2829557] : 0x762506fe);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x69510818));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x235daaa7));
            rb_state3.var_0 = (rb_state3.var_0) ^ ((0x76baee85) + (0x43c26217));
            rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_1);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
            rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xed5d1749));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (2371888 < rb_input_size ? (uint32_t)rb_input[2371888] : 0xffd8dd5c);
            rb_state5.var_1 = (rb_state5.var_1) + ((0x6acb734a) + (rb_state5.var_0));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ ((0xe71ed4ef) ^ (0x8bb75365));
            rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x6ca9878a)) ? (rb_state6.var_1) : (0x19fa3232));
            rb_state6.var_0 = (rb_state6.var_0) - (1230148 < rb_input_size ? (uint32_t)rb_input[1230148] : 0xf4975053);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - ((0x3eba9165) + (rb_state7.var_2));
            rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x38)) ? (rb_state7.var_2) : (0x1ceb4aeb));
            if ((rb_state7.var_1) == (0x792dce42)) {
                pthread_mutex_lock(&(rb_state7.lock_31));
                rb_state7.var_16 = 0xb8e8965e;
                rb_state7.var_17 = 0x6625fdf3;
                rb_state7.var_18 = 0xea9ae364;
                rb_state7.var_19 = (rb_state7.var_19) ^ (3312860 < rb_input_size ? (uint32_t)rb_input[3312860] : 0x384dfebc);
                rb_state7.var_20 = (rb_state7.var_20) ^ (((((0xc336855b) - (rb_state7.var_12)) ^ (0x1ea3cc43)) + (rb_state7.var_16)) + (rb_state7.var_18));
                rb_state7.var_21 = (rb_state7.var_21) + (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_19) : (0x420d0fec));
                rb_state7.var_22 = (rb_state7.var_22) + (((0xc47d1b58) ^ (rb_state7.var_20)) ^ (rb_state7.var_13));
                rb_state7.var_23 = (rb_state7.var_23) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_21) : (0xfb0d4a9a));
                rb_state7.var_24 = (rb_state7.var_24) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_22) : (0xe88de185));
                rb_state7.var_25 = (rb_state7.var_25) ^ (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_24) : (0x37fd6686));
                rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_25);
                rb_state7.var_3 = rb_state7.var_15;
                pthread_mutex_unlock(&(rb_state7.lock_31));
            }
            if ((rb_state7.var_1) == (0x792dce42)) {
                pthread_mutex_lock(&(rb_state7.lock_31));
                rb_state7.var_27 = 0x99ea735c;
                rb_state7.var_28 = (rb_state7.var_28) ^ (((0xeb46f954) ^ (rb_state7.var_27)) - (rb_state7.var_18));
                rb_state7.var_29 = (rb_state7.var_29) - (((((0xc48c7c84) ^ (rb_state7.var_16)) + (0x7d1822ba)) ^ (rb_state7.var_19)) ^ (rb_state7.var_17));
                rb_state7.var_30 = (rb_state7.var_30) - (((((0xe3139420) ^ (0xd7992b29)) ^ (0x2f862068)) ^ (rb_state7.var_28)) ^ (rb_state7.var_29));
                rb_state7.var_26 = (rb_state7.var_26) ^ (rb_state7.var_30);
                rb_state7.var_3 = rb_state7.var_26;
                pthread_mutex_unlock(&(rb_state7.lock_31));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_0);
            rb_state9.var_1 = (rb_state9.var_1) + ((0xc9baaec8) + (rb_state9.var_0));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
            rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_1);
            rb_state10.var_0 = (rb_state10.var_0) - (0xf0007709);
            rb_state10.var_1 = (rb_state10.var_1) - ((0x54246e7a) - (rb_state10.var_0));
            rb_state10.var_2 = (rb_state10.var_2) - (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_2) : (0x35efddfd));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x79fcecb));
            rb_state13.var_2 = (rb_state13.var_2) + ((0x95f977a0) + (0x41c6ce9b));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (161654 < rb_input_size ? (uint32_t)rb_input[161654] : 0x56202d78);
            rb_state14.var_2 = (rb_state14.var_2) + (18595 < rb_input_size ? (uint32_t)rb_input[18595] : 0xc24458e1);
            rb_state14.var_1 = (rb_state14.var_1) - ((0x7483fa57) - (rb_state14.var_2));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
            rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xd901525));
            rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
            rb_state16.var_0 = (rb_state16.var_0) + (0x46e6f82a);
            rb_state16.var_1 = (rb_state16.var_1) + (0x172ae3c1);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
            rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_1) : (0x8d02accd));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
        rb_state1.var_2 = (rb_state1.var_2) ^ (0xef765681);
        rb_state1.var_2 = (rb_state1.var_2) ^ (0xdc14ff6);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - (0xa1c70870);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (79715 < rb_input_size ? (uint32_t)rb_input[79715] : 0x20d0795f);
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_2);
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state6.lock_34));
            if ((rb_state6.var_3) != (0x0)) {
                if (!((rb_state6.var_3) == (rb_state6.var_21))) {
                    racebench_trigger(6);
                }
            }
            pthread_mutex_unlock(&(rb_state6.lock_34));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_1) == (0xfffffffc)) ? (rb_state8.var_1) : (0x70111537));
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_2) == (0xffffff97)) {
            rb_state9.var_8 = 0xcd651d3d;
            rb_state9.var_9 = (rb_state9.var_9) - (((((0x6aef391b) - (rb_state9.var_3)) + (rb_state9.var_8)) - (0xdc0675c4)) - (rb_state9.var_8));
            rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_7) == (0xb27fbafd)) ? (rb_state9.var_9) : (0x72166e33));
            rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_10);
        }
        rb_state9.var_9 = (rb_state9.var_9) ^ (rb_state9.var_10);
        rb_state9.var_6 = (rb_state9.var_6) ^ (((rb_state9.var_4) == (0xb27fbafd)) ? (rb_state9.var_12) : (0xc9e62f22));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (1799026 < rb_input_size ? (uint32_t)rb_input[1799026] : 0x5c878c22);
        if ((rb_state11.var_0) == (0x0)) {
            rb_state11.var_8 = 0x6645e721;
            rb_state11.var_9 = 0xd915c995;
            rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_6) == (0x6cbe9b78)) ? (rb_state11.var_5) : (0xf58514be));
            rb_state11.var_11 = (rb_state11.var_11) - (((((0x6725fc77) ^ (rb_state11.var_8)) ^ (0x31d80688)) ^ (rb_state11.var_7)) + (rb_state11.var_4));
            rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_8) == (0x6645e721)) ? (rb_state11.var_9) : (0x749555c0));
            rb_state11.var_13 = (rb_state11.var_13) - (((rb_state11.var_9) == (0xd915c995)) ? (rb_state11.var_10) : (0x49473499));
            rb_state11.var_14 = (rb_state11.var_14) ^ (((((0xbe7a961d) ^ (rb_state11.var_12)) ^ (rb_state11.var_11)) - (0x31a08431)) - (0x3b63ad76));
            rb_state11.var_15 = (rb_state11.var_15) ^ (((0xa402b683) - (0xed9aa0a3)) ^ (rb_state11.var_13));
            rb_state11.var_16 = (rb_state11.var_16) ^ (((0x70543296) + (rb_state11.var_14)) - (rb_state11.var_10));
            rb_state11.var_17 = (rb_state11.var_17) - (((rb_state11.var_15) == (0xb66815e0)) ? (rb_state11.var_16) : (0xd9973cec));
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x0)) {
            rb_state13.var_4 = 0x76ce8479;
            rb_state13.var_5 = 0x16c0ae76;
            rb_state13.var_6 = 0xc11f3611;
            rb_state13.var_7 = (rb_state13.var_7) - (((((0xbff1314b) ^ (rb_state13.var_4)) ^ (rb_state13.var_3)) - (rb_state13.var_6)) - (0xd8a5a2a7));
            rb_state13.var_8 = (rb_state13.var_8) - (rb_state13.var_5);
            rb_state13.var_9 = (rb_state13.var_9) ^ (rb_state13.var_7);
            rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_4) == (0x76ce8479)) ? (rb_state13.var_8) : (0x301e6710));
            rb_state13.var_11 = (rb_state13.var_11) - (((rb_state13.var_9) == (0xe8ee07d5)) ? (rb_state13.var_10) : (0x19ea46ae));
            rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x777113da)) {
            rb_state14.var_4 = 0x7d13a8ee;
            rb_state14.var_5 = 0xc27b2a10;
            rb_state14.var_6 = (rb_state14.var_6) ^ (((rb_state14.var_4) == (0x7d13a8ee)) ? (rb_state14.var_3) : (0x85b43167));
            rb_state14.var_7 = (rb_state14.var_7) - (((0xef3787ea) + (0xe085c975)) ^ (rb_state14.var_5));
            rb_state14.var_8 = (rb_state14.var_8) ^ (((rb_state14.var_5) == (0xc27b2a10)) ? (rb_state14.var_4) : (0xae6cdb90));
            rb_state14.var_9 = (rb_state14.var_9) ^ (((((0x193c36bb) + (rb_state14.var_7)) + (rb_state14.var_6)) - (rb_state14.var_7)) + (rb_state14.var_6));
            rb_state14.var_10 = (rb_state14.var_10) + (((rb_state14.var_8) == (0x7d13a8ee)) ? (rb_state14.var_8) : (0x76bce2cb));
            rb_state14.var_11 = (rb_state14.var_11) + (((((0xa95c3078) - (rb_state14.var_9)) ^ (rb_state14.var_10)) + (0x51f978e8)) ^ (rb_state14.var_9));
            rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_2) : (0x40ef6cb3));
        rb_state15.var_1 = (rb_state15.var_1) ^ (909031 < rb_input_size ? (uint32_t)rb_input[909031] : 0x84732dbf);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x46e6f82a)) {
            rb_state16.var_3 = 0x1fcbf9e1;
            rb_state16.var_4 = (rb_state16.var_4) - (((rb_state16.var_2) == (0x34c222e2)) ? (rb_state16.var_3) : (0x6f510939));
            rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_4);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) - ((0x70a38369) - (rb_state17.var_2));
        rb_state17.var_1 = (rb_state17.var_1) ^ ((0xc7839dce) - (rb_state17.var_2));
        rb_state17.var_0 = (rb_state17.var_0) ^ ((0x27021d73) ^ (rb_state17.var_1));
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
    rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
    rb_state0.var_2 = (rb_state0.var_2) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_2) : (0x5dc134db));
    rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0x683f14d3));
    rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0xe2b71977)) {
        rb_state1.var_3 = rb_state1.var_0;
    }
    if ((rb_state1.var_2) == (0xe2b71977)) {
        if (!((rb_state1.var_0) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_0);
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_1);
    rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_1) : (0x508b5163));
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_16 = 0x98574a3e;
        rb_state2.var_17 = 0x68918c90;
        rb_state2.var_18 = (rb_state2.var_18) - (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_8) : (0x1a2b5b73));
        rb_state2.var_19 = (rb_state2.var_19) - (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_18) : (0x86dd3ec9));
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        rb_state3.var_4 = 0xafbaa2d;
        rb_state3.var_5 = 0x3450a217;
        rb_state3.var_6 = (rb_state3.var_6) ^ (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_3) : (0xea07c983));
        rb_state3.var_7 = (rb_state3.var_7) - (0x2847961f);
        rb_state3.var_8 = (rb_state3.var_8) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x580b0809));
        rb_state3.var_9 = (rb_state3.var_9) ^ (((0x41b59b43) ^ (rb_state3.var_5)) ^ (rb_state3.var_6));
        rb_state3.var_10 = (rb_state3.var_10) ^ (rb_state3.var_7);
        rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_8) : (0x5626ca2a));
        rb_state3.var_12 = (rb_state3.var_12) - (((((0x4fd7c2e5) ^ (rb_state3.var_10)) ^ (rb_state3.var_9)) - (rb_state3.var_7)) ^ (rb_state3.var_8));
        rb_state3.var_13 = (rb_state3.var_13) ^ (rb_state3.var_11);
        rb_state3.var_14 = (rb_state3.var_14) ^ (((0x989bf8e4) - (rb_state3.var_12)) - (0x8ac60e70));
        rb_state3.var_15 = (rb_state3.var_15) - (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_13) : (0x82c58064));
        rb_state3.var_16 = (rb_state3.var_16) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0xbe243a1));
        rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_1);
    rb_state4.var_1 = (rb_state4.var_1) + (0x56c43d84);
    if ((rb_state4.var_2) == (0xa93bc27c)) {
        pthread_mutex_lock(&(rb_state4.lock_24));
        rb_state4.var_8 = 0x397ed40f;
        rb_state4.var_9 = (rb_state4.var_9) + (((((0x3535f132) - (rb_state4.var_7)) ^ (0x236f2073)) ^ (rb_state4.var_8)) - (rb_state4.var_6));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((((0xda8173ae) + (rb_state4.var_9)) - (0xfd4fcacf)) ^ (rb_state4.var_8)) - (0x5d255029));
        rb_state4.var_7 = (rb_state4.var_7) ^ (rb_state4.var_10);
        rb_state4.var_3 = rb_state4.var_7;
        pthread_mutex_unlock(&(rb_state4.lock_24));
    }
    if ((rb_state4.var_2) == (0xa93bc27c)) {
        pthread_mutex_lock(&(rb_state4.lock_24));
        rb_state4.var_12 = 0xf62c63e9;
        rb_state4.var_13 = 0xe6594418;
        rb_state4.var_14 = (rb_state4.var_14) ^ (rb_state4.var_9);
        rb_state4.var_15 = (rb_state4.var_15) - (((((0xaf917bb3) - (rb_state4.var_12)) - (0xac8fd01e)) - (rb_state4.var_13)) + (0x90c50aa));
        rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_14) : (0x6b527afe));
        rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_15) : (0xb5d5f968));
        rb_state4.var_18 = (rb_state4.var_18) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_16) : (0x2091c788));
        rb_state4.var_19 = (rb_state4.var_19) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_17) : (0x949342e8));
        rb_state4.var_20 = (rb_state4.var_20) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_18) : (0x856d3f9d));
        rb_state4.var_21 = (rb_state4.var_21) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_19) : (0xc907717a));
        rb_state4.var_22 = (rb_state4.var_22) - (rb_state4.var_20);
        rb_state4.var_23 = (rb_state4.var_23) - (((((0x2d9e9efe) + (rb_state4.var_16)) ^ (rb_state4.var_22)) + (rb_state4.var_21)) + (0x7259e293));
        rb_state4.var_11 = (rb_state4.var_11) - (rb_state4.var_23);
        rb_state4.var_3 = rb_state4.var_11;
        pthread_mutex_unlock(&(rb_state4.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0x0)) {
        rb_state5.var_19 = (rb_state5.var_19) ^ (rb_state5.var_17);
        rb_state5.var_20 = (rb_state5.var_20) + (((((0xc7d0792f) + (rb_state5.var_14)) ^ (rb_state5.var_15)) ^ (0x17cdb4ba)) + (0xf7c0812b));
        rb_state5.var_21 = (rb_state5.var_21) + (rb_state5.var_18);
        rb_state5.var_22 = (rb_state5.var_22) ^ (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_16) : (0xed29bd01));
        rb_state5.var_23 = (rb_state5.var_23) ^ (((0xab69790e) + (rb_state5.var_20)) ^ (rb_state5.var_19));
        rb_state5.var_24 = (rb_state5.var_24) - (((rb_state5.var_21) == (0x0)) ? (rb_state5.var_20) : (0x61f3f3bc));
        rb_state5.var_25 = (rb_state5.var_25) - (((rb_state5.var_21) == (0x0)) ? (rb_state5.var_22) : (0x4f90df98));
        rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_23) : (0xc9dba7c7));
        rb_state5.var_27 = (rb_state5.var_27) ^ (((((0x6400ff5a) + (rb_state5.var_25)) ^ (rb_state5.var_23)) - (rb_state5.var_24)) + (0xacc97713));
        rb_state5.var_28 = (rb_state5.var_28) ^ (rb_state5.var_26);
        rb_state5.var_29 = (rb_state5.var_29) + (rb_state5.var_27);
        rb_state5.var_30 = (rb_state5.var_30) + (((rb_state5.var_28) == (0x0)) ? (rb_state5.var_29) : (0xf33fd9c0));
        rb_state5.var_18 = (rb_state5.var_18) + (rb_state5.var_30);
        rb_state5.var_3 = rb_state5.var_18;
    }
    if ((rb_state5.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_37));
        rb_state5.var_32 = 0xd3b7190e;
        rb_state5.var_33 = 0x28e8acfc;
        rb_state5.var_34 = (rb_state5.var_34) ^ (((rb_state5.var_33) == (0x0)) ? (rb_state5.var_24) : (0x4aca8538));
        rb_state5.var_35 = (rb_state5.var_35) + (((0xc1a6962f) - (0x55682af1)) - (rb_state5.var_32));
        rb_state5.var_36 = (rb_state5.var_36) + (((rb_state5.var_34) == (0x0)) ? (rb_state5.var_35) : (0x553a5068));
        rb_state5.var_31 = (rb_state5.var_31) - (rb_state5.var_36);
        rb_state5.var_3 = rb_state5.var_31;
        pthread_mutex_unlock(&(rb_state5.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ ((0xa27a34c3) + (0x24954651));
    rb_state6.var_1 = (rb_state6.var_1) + (0xe9857dab);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ ((0x792dce42) ^ (rb_state7.var_1));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (3671473 < rb_input_size ? (uint32_t)rb_input[3671473] : 0xed3adb9d);
    if ((rb_state8.var_1) == (0xffffffc5)) {
        rb_state8.var_9 = 0x7828427d;
        rb_state8.var_10 = 0xe603305e;
        rb_state8.var_11 = (rb_state8.var_11) - (1727895 < rb_input_size ? (uint32_t)rb_input[1727895] : 0x41c5c75c);
        rb_state8.var_12 = (rb_state8.var_12) - (((0xda7f7097) - (rb_state8.var_6)) ^ (rb_state8.var_10));
        rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_5) : (0x853f1c74));
        rb_state8.var_14 = (rb_state8.var_14) - (((0x8db1b784) ^ (rb_state8.var_7)) ^ (rb_state8.var_11));
        rb_state8.var_15 = (rb_state8.var_15) ^ (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_13) : (0x45b71349));
        rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_14);
        rb_state8.var_17 = (rb_state8.var_17) - (((((0xc211a0a) ^ (rb_state8.var_15)) ^ (rb_state8.var_9)) ^ (rb_state8.var_16)) + (rb_state8.var_8));
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (3290977 < rb_input_size ? (uint32_t)rb_input[3290977] : 0x3830da00);
    rb_state9.var_1 = (rb_state9.var_1) + (0x6a97ab02);
    if ((rb_state9.var_8) == (0xcd651d3d)) {
        if ((rb_state9.var_13) != (0x0)) {
            if (!((rb_state9.var_13) == (rb_state9.var_29))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xbbdb1a7d)) {
        rb_state10.var_10 = 0xf9c21227;
        rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_10) : (0xff4ee793));
        rb_state10.var_12 = (rb_state10.var_12) ^ (((((0x3380d693) - (rb_state10.var_5)) ^ (0x8cd8a707)) ^ (rb_state10.var_11)) - (0xf46c0d99));
        rb_state10.var_9 = (rb_state10.var_9) ^ (rb_state10.var_12);
        rb_state10.var_3 = rb_state10.var_9;
    }
    if ((rb_state10.var_1) == (0xbbdb1a7d)) {
        pthread_mutex_lock(&(rb_state10.lock_22));
        rb_state10.var_14 = 0xddffbe7d;
        rb_state10.var_15 = 0x4e5a490e;
        rb_state10.var_16 = 0x85357190;
        rb_state10.var_17 = (rb_state10.var_17) + (((((0x4e51b35f) + (rb_state10.var_6)) - (0x1f9ff6b2)) + (rb_state10.var_14)) + (rb_state10.var_15));
        rb_state10.var_18 = (rb_state10.var_18) + (rb_state10.var_16);
        rb_state10.var_19 = (rb_state10.var_19) ^ (rb_state10.var_17);
        rb_state10.var_20 = (rb_state10.var_20) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_18) : (0xb3201cd7));
        rb_state10.var_21 = (rb_state10.var_21) ^ (((((0xc8dcd365) + (rb_state10.var_19)) ^ (0x54595184)) + (0xedf5f42d)) + (rb_state10.var_20));
        rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_21);
        rb_state10.var_3 = rb_state10.var_13;
        pthread_mutex_unlock(&(rb_state10.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (2999307 < rb_input_size ? (uint32_t)rb_input[2999307] : 0x8b56d14c);
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x434f31bc));
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x397172c6);
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x45516626));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (519522 < rb_input_size ? (uint32_t)rb_input[519522] : 0xd4cfe324);
    rb_state14.var_0 = (rb_state14.var_0) - (0xc44775e3);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x39)) {
        rb_state15.var_3 = rb_state15.var_2;
    }
    if ((rb_state15.var_1) == (0x39)) {
        if (!((rb_state15.var_2) == (rb_state15.var_3))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0xa72366f));
    if ((rb_state16.var_0) == (0x46e6f82a)) {
        rb_state16.var_2 = rb_state16.var_1;
    }
    if ((rb_state16.var_0) == (0x46e6f82a)) {
        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x1f253c44)) {
        pthread_mutex_lock(&(rb_state17.lock_26));
        rb_state17.var_5 = 0xe2316166;
        rb_state17.var_6 = (rb_state17.var_6) ^ (867555 < rb_input_size ? (uint32_t)rb_input[867555] : 0x367f467);
        rb_state17.var_7 = (rb_state17.var_7) ^ (((0xbd69ee32) ^ (rb_state17.var_5)) - (rb_state17.var_5));
        rb_state17.var_8 = (rb_state17.var_8) - (((0xdef48313) - (rb_state17.var_3)) + (rb_state17.var_6));
        rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_4);
        rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0x71b36530));
        rb_state17.var_11 = (rb_state17.var_11) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0xc6f57ed8));
        rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_9);
        rb_state17.var_13 = (rb_state17.var_13) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x47d996df));
        rb_state17.var_14 = (rb_state17.var_14) + (((0xb67db43f) - (rb_state17.var_12)) + (0x3660a028));
        rb_state17.var_15 = (rb_state17.var_15) - (rb_state17.var_13);
        rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_15) : (0x2aeda371));
        rb_state17.var_4 = (rb_state17.var_4) - (rb_state17.var_16);
        rb_state17.var_3 = rb_state17.var_4;
        pthread_mutex_unlock(&(rb_state17.lock_26));
    }
    if ((rb_state17.var_0) == (0x1f253c44)) {
        pthread_mutex_lock(&(rb_state17.lock_26));
        if (!((rb_state17.var_3) == (rb_state17.var_4))) {
            racebench_trigger(17);
        }
        pthread_mutex_unlock(&(rb_state17.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ ((0xd1f96134) + (rb_state18.var_1));
    if ((rb_state18.var_2) == (0x348cf60e)) {
        rb_state18.var_20 = 0xd65f5c4a;
        rb_state18.var_21 = 0x7c13ef6c;
        rb_state18.var_22 = (rb_state18.var_22) + ((0xfc9fd22a) - (rb_state18.var_8));
        rb_state18.var_23 = (rb_state18.var_23) + (((((0x58b85c40) ^ (rb_state18.var_20)) - (0x5f40ffb4)) ^ (rb_state18.var_9)) - (rb_state18.var_7));
        rb_state18.var_24 = (rb_state18.var_24) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_21) : (0x550a1a23));
        rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_22) : (0x397d656f));
        rb_state18.var_26 = (rb_state18.var_26) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_23) : (0xee6b8e15));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_25) : (0xefcec7a));
        rb_state18.var_28 = (rb_state18.var_28) - (rb_state18.var_26);
        rb_state18.var_29 = (rb_state18.var_29) - (rb_state18.var_27);
        rb_state18.var_30 = (rb_state18.var_30) - (((0xf5269c7f) - (rb_state18.var_13)) ^ (rb_state18.var_28));
        rb_state18.var_31 = (rb_state18.var_31) + (((((0xabf6c77) + (rb_state18.var_14)) + (rb_state18.var_29)) - (0x89e99da0)) ^ (rb_state18.var_30));
        rb_state18.var_3 = (rb_state18.var_3) ^ (rb_state18.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0x8bad1c47) + (rb_state19.var_0));
    rb_state19.var_1 = (rb_state19.var_1) ^ (1782200 < rb_input_size ? (uint32_t)rb_input[1782200] : 0xe174bb15);
    rb_state19.var_1 = (rb_state19.var_1) ^ (0x7c3d5097);
    rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_2) : (0x77bec31));
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x5a8318b7);
    if ((rb_state19.var_1) == (0x3c3ed20f)) {
        if ((rb_state19.var_3) != (0x0)) {
            if (!((rb_state19.var_3) == (rb_state19.var_22))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0x0)) {
        rb_state0.var_9 = 0x32582bf0;
        rb_state0.var_10 = 0x2b04cc37;
        rb_state0.var_11 = 0x5eb3872b;
        rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_5) : (0x17573519));
        rb_state0.var_13 = (rb_state0.var_13) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_11) : (0x65d5a2a1));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_10);
        rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_9);
        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_12) : (0x4b33cb7f));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((((0x817f66e) + (rb_state0.var_9)) + (rb_state0.var_14)) + (rb_state0.var_13)) + (0x477401f9));
        rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x2d3cfcda));
        rb_state0.var_19 = (rb_state0.var_19) ^ (((0xe23fc822) + (rb_state0.var_10)) - (rb_state0.var_17));
        rb_state0.var_20 = (rb_state0.var_20) - (((0x2c72c564) - (rb_state0.var_11)) ^ (rb_state0.var_18));
        rb_state0.var_21 = (rb_state0.var_21) ^ (((0x8c80b4c6) ^ (0xd4a56079)) + (rb_state0.var_19));
        rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0x64909ee));
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0xe2432716) - (rb_state1.var_1));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (3008935 < rb_input_size ? (uint32_t)rb_input[3008935] : 0xcc1dce89);
    rb_state2.var_1 = (rb_state2.var_1) ^ (1966273 < rb_input_size ? (uint32_t)rb_input[1966273] : 0xd74a2d35);
    if ((rb_state2.var_0) == (0x36)) {
        pthread_mutex_lock(&(rb_state2.lock_20));
        rb_state2.var_4 = 0x8ac40e67;
        rb_state2.var_5 = (rb_state2.var_5) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0x679ea16b));
        rb_state2.var_6 = (rb_state2.var_6) ^ (rb_state2.var_4);
        rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_2) : (0xa15a52aa));
        rb_state2.var_8 = (rb_state2.var_8) ^ (((((0xe1e4d9f5) ^ (0x5934a7e2)) ^ (rb_state2.var_6)) - (rb_state2.var_5)) + (0x4e4c50eb));
        rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_7) : (0xcbccb653));
        rb_state2.var_10 = (rb_state2.var_10) + (rb_state2.var_8);
        rb_state2.var_11 = (rb_state2.var_11) - (((0x3c8567d) - (rb_state2.var_9)) - (0xfa691266));
        rb_state2.var_12 = (rb_state2.var_12) ^ (((0x69d6e70f) - (rb_state2.var_10)) ^ (0x25ae576f));
        rb_state2.var_13 = (rb_state2.var_13) ^ (rb_state2.var_11);
        rb_state2.var_14 = (rb_state2.var_14) - (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_12) : (0x60b06ce3));
        rb_state2.var_15 = (rb_state2.var_15) + (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_14) : (0x878a807f));
        rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_15);
        rb_state2.var_2 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_20));
    }
    if ((rb_state2.var_0) == (0x0)) {
        if (!((rb_state2.var_2) == (rb_state2.var_3))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_1) : (0x9e14c6c4));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_2);
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_1) == (0xd596e6f4)) ? (rb_state5.var_0) : (0x1ad04a33));
    rb_state5.var_0 = (rb_state5.var_0) + (0x97144465);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_2);
    if ((rb_state6.var_2) == (0x0)) {
        rb_state6.var_10 = (rb_state6.var_10) ^ ((0xda186a6c) ^ (0xc4e49622));
        rb_state6.var_11 = (rb_state6.var_11) - (((0xc57f94aa) + (rb_state6.var_4)) - (rb_state6.var_6));
        rb_state6.var_12 = (rb_state6.var_12) + (((((0x56b03cba) ^ (rb_state6.var_3)) ^ (rb_state6.var_7)) - (rb_state6.var_5)) + (0x57acd2e0));
        rb_state6.var_13 = (rb_state6.var_13) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_10) : (0x8aa9adf3));
        rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_11);
        rb_state6.var_15 = (rb_state6.var_15) ^ (((0xab34adbe) ^ (rb_state6.var_9)) + (rb_state6.var_12));
        rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_13) == (0x1efcfc4e)) ? (rb_state6.var_14) : (0x9ebb5c10));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((0x1471b99a) ^ (rb_state6.var_15)) - (0xf088e3df));
        rb_state6.var_18 = (rb_state6.var_18) - (((0x30f45550) - (rb_state6.var_16)) + (0xa90ffef3));
        rb_state6.var_19 = (rb_state6.var_19) + (rb_state6.var_17);
        rb_state6.var_20 = (rb_state6.var_20) + (((((0x671c59fe) + (rb_state6.var_18)) - (rb_state6.var_19)) + (rb_state6.var_11)) ^ (rb_state6.var_10));
        rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_20);
        rb_state6.var_3 = rb_state6.var_9;
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
    rb_state7.var_0 = (rb_state7.var_0) ^ (1233468 < rb_input_size ? (uint32_t)rb_input[1233468] : 0x8bf9a630);
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x38)) ? (rb_state7.var_2) : (0x33fa9f73));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x607ca9d8));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (689981 < rb_input_size ? (uint32_t)rb_input[689981] : 0x7f9f68ee);
    rb_state9.var_7 = (rb_state9.var_7) - (rb_state9.var_5);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (1140163 < rb_input_size ? (uint32_t)rb_input[1140163] : 0x12f33c82);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (3007436 < rb_input_size ? (uint32_t)rb_input[3007436] : 0xe6789137);
    if ((rb_state17.var_0) == (0x1f253c44)) {
        pthread_mutex_lock(&(rb_state17.lock_26));
        rb_state17.var_17 = 0x32edd80;
        rb_state17.var_18 = 0xe6424de;
        rb_state17.var_19 = (rb_state17.var_19) + (((rb_state17.var_17) == (0x32edd80)) ? (rb_state17.var_8) : (0x41d3674a));
        rb_state17.var_20 = (rb_state17.var_20) + (0x7ac095ef);
        rb_state17.var_21 = (rb_state17.var_21) + (rb_state17.var_18);
        rb_state17.var_22 = (rb_state17.var_22) - (((rb_state17.var_19) == (0x210b7cbc)) ? (rb_state17.var_20) : (0x5f552a5a));
        rb_state17.var_23 = (rb_state17.var_23) - (((0x68fe3bd8) ^ (rb_state17.var_21)) - (0x27a4e963));
        rb_state17.var_24 = (rb_state17.var_24) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_22) : (0xb602378b));
        rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_23) == (0xc10aca5d)) ? (rb_state17.var_24) : (0xa6b42ae2));
        rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_25);
        pthread_mutex_unlock(&(rb_state17.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (0x37b7fa06);
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_0) == (0x37b7fa06)) ? (rb_state18.var_1) : (0xe99146f9));
    #endif
    ProcID = ProcID % NumProcs;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_23));
        rb_state0.var_5 = 0xeb8d8301;
        rb_state0.var_6 = 0xff960d48;
        rb_state0.var_7 = (rb_state0.var_7) - (((((0x162d790d) ^ (rb_state0.var_3)) + (0x84807447)) + (rb_state0.var_6)) - (rb_state0.var_4));
        rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_5) == (0xeb8d8301)) ? (rb_state0.var_7) : (0xe6826a));
        rb_state0.var_4 = (rb_state0.var_4) ^ (rb_state0.var_8);
        rb_state0.var_3 = rb_state0.var_4;
        pthread_mutex_unlock(&(rb_state0.lock_23));
    }
    if ((rb_state0.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_23));
        if (!((rb_state0.var_3) == (rb_state0.var_4))) {
            racebench_trigger(0);
        }
        pthread_mutex_unlock(&(rb_state0.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0xe2b71977)) {
        rb_state1.var_4 = 0xb6d1b068;
        rb_state1.var_5 = (rb_state1.var_5) ^ (((rb_state1.var_3) == (0x811fe060)) ? (rb_state1.var_4) : (0xe7346819));
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
    if ((rb_state3.var_1) == (0x0)) {
        rb_state3.var_3 = rb_state3.var_2;
    }
    if ((rb_state3.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_17));
        if (!((rb_state3.var_2) == (rb_state3.var_3))) {
            racebench_trigger(3);
        }
        pthread_mutex_unlock(&(rb_state3.lock_17));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_2) == (0xa93bc27c)) {
        if ((rb_state4.var_3) != (0x0)) {
            if (!((rb_state4.var_3) == (rb_state4.var_11))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0x0)) {
        if ((rb_state5.var_3) != (0x0)) {
            if (!((rb_state5.var_3) == (rb_state5.var_31))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_34));
        rb_state6.var_22 = 0x77264fe5;
        rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_15) == (0x5991bd58)) ? (rb_state6.var_14) : (0xac68f5fb));
        rb_state6.var_24 = (rb_state6.var_24) ^ (((0x8be8350c) - (0xf6a9a1aa)) ^ (rb_state6.var_12));
        rb_state6.var_25 = (rb_state6.var_25) + (rb_state6.var_16);
        rb_state6.var_26 = (rb_state6.var_26) ^ (((rb_state6.var_22) == (0x77264fe5)) ? (rb_state6.var_13) : (0xe5979fd7));
        rb_state6.var_27 = (rb_state6.var_27) ^ (((rb_state6.var_17) == (0x5d5720e3)) ? (rb_state6.var_23) : (0x86c0d1f5));
        rb_state6.var_28 = (rb_state6.var_28) ^ (((0xa40373f) + (0x90e4681d)) - (rb_state6.var_24));
        rb_state6.var_29 = (rb_state6.var_29) ^ (((rb_state6.var_25) == (0x3a806b56)) ? (rb_state6.var_26) : (0x3a7fa761));
        rb_state6.var_30 = (rb_state6.var_30) ^ (((rb_state6.var_18) == (0x607c1713)) ? (rb_state6.var_27) : (0x58ebd4d5));
        rb_state6.var_31 = (rb_state6.var_31) + (((rb_state6.var_19) == (0x5d5720e3)) ? (rb_state6.var_28) : (0x97517fb5));
        rb_state6.var_32 = (rb_state6.var_32) - (((rb_state6.var_29) == (0x1efcfc4e)) ? (rb_state6.var_30) : (0x7d9e425));
        rb_state6.var_33 = (rb_state6.var_33) + (((rb_state6.var_31) == (0x5fc10264)) ? (rb_state6.var_32) : (0x5bd0fe3b));
        rb_state6.var_21 = (rb_state6.var_21) + (rb_state6.var_33);
        rb_state6.var_3 = rb_state6.var_21;
        pthread_mutex_unlock(&(rb_state6.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x792dce42)) {
        if ((rb_state7.var_3) != (0x0)) {
            if (!((rb_state7.var_3) == (rb_state7.var_26))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x645c0fcf) - (0x1cf8f4e5));
    rb_state8.var_1 = (rb_state8.var_1) ^ (692264 < rb_input_size ? (uint32_t)rb_input[692264] : 0x50ecfc0b);
    if ((rb_state8.var_1) == (0xfffffffc)) {
        pthread_mutex_lock(&(rb_state8.lock_18));
        rb_state8.var_4 = 0x3afcff8f;
        rb_state8.var_5 = (rb_state8.var_5) ^ (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_3) : (0xc55e6d74));
        rb_state8.var_6 = (rb_state8.var_6) ^ (((0xd70e96b5) + (rb_state8.var_4)) + (0x6350d9ce));
        rb_state8.var_7 = (rb_state8.var_7) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0x53ec80ec));
        rb_state8.var_8 = (rb_state8.var_8) + (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0xe20b9e7c));
        rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_8);
        rb_state8.var_2 = rb_state8.var_3;
        pthread_mutex_unlock(&(rb_state8.lock_18));
    }
    if ((rb_state8.var_1) == (0xffffffc5)) {
        if (!((rb_state8.var_2) == (rb_state8.var_3))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (2069761 < rb_input_size ? (uint32_t)rb_input[2069761] : 0x346e0f16);
    if ((rb_state9.var_2) == (0xffffff97)) {
        pthread_mutex_lock(&(rb_state9.lock_11));
        rb_state9.var_5 = 0x99de1da2;
        rb_state9.var_6 = (rb_state9.var_6) - (((((0xe75e62a5) ^ (rb_state9.var_4)) + (rb_state9.var_6)) - (rb_state9.var_5)) - (rb_state9.var_3));
        rb_state9.var_7 = (rb_state9.var_7) ^ (((rb_state9.var_5) == (0x99de1da2)) ? (rb_state9.var_6) : (0xf5225b5c));
        rb_state9.var_4 = (rb_state9.var_4) + (rb_state9.var_7);
        rb_state9.var_3 = rb_state9.var_4;
        pthread_mutex_unlock(&(rb_state9.lock_11));
    }
    if ((rb_state9.var_2) == (0xffffff97)) {
        rb_state9.var_12 = !((rb_state9.var_3) == (rb_state9.var_4));
    }
    if ((rb_state9.var_8) == (0xcd651d3d)) {
        pthread_mutex_lock(&(rb_state9.lock_35));
        rb_state9.var_19 = 0x40b28aa8;
        rb_state9.var_20 = (rb_state9.var_20) + (rb_state9.var_19);
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_18) : (0x27295405));
        rb_state9.var_22 = (rb_state9.var_22) - (rb_state9.var_17);
        rb_state9.var_23 = (rb_state9.var_23) + (rb_state9.var_20);
        rb_state9.var_24 = (rb_state9.var_24) - (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_22) : (0x3913e507));
        rb_state9.var_25 = (rb_state9.var_25) + (rb_state9.var_23);
        rb_state9.var_26 = (rb_state9.var_26) + (rb_state9.var_24);
        rb_state9.var_27 = (rb_state9.var_27) ^ (((0x3b395126) ^ (rb_state9.var_25)) - (rb_state9.var_20));
        rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_27) : (0x419b83b0));
        rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_28);
        rb_state9.var_13 = rb_state9.var_18;
        pthread_mutex_unlock(&(rb_state9.lock_35));
    }
    if ((rb_state9.var_8) == (0xcd651d3d)) {
        pthread_mutex_lock(&(rb_state9.lock_35));
        rb_state9.var_30 = 0x572475c1;
        rb_state9.var_31 = 0x8e05b5c0;
        rb_state9.var_32 = 0x1a739971;
        rb_state9.var_33 = (rb_state9.var_33) + (((((0x9f314b69) + (rb_state9.var_31)) - (rb_state9.var_32)) - (0x7fdb51cb)) ^ (rb_state9.var_21));
        rb_state9.var_34 = (rb_state9.var_34) + (((((0xd08b550d) ^ (rb_state9.var_30)) ^ (0x10a49278)) - (rb_state9.var_22)) - (rb_state9.var_33));
        rb_state9.var_29 = (rb_state9.var_29) ^ (rb_state9.var_34);
        rb_state9.var_13 = rb_state9.var_29;
        pthread_mutex_unlock(&(rb_state9.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0xfff88f7)) ? (rb_state10.var_2) : (0x3225d034));
    if ((rb_state10.var_1) == (0xbbdb1a7d)) {
        if ((rb_state10.var_3) != (0x0)) {
            if (!((rb_state10.var_3) == (rb_state10.var_13))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
    if ((rb_state11.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_18));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_1);
        rb_state11.var_4 = (rb_state11.var_4) + (986983 < rb_input_size ? (uint32_t)rb_input[986983] : 0x6ba2308);
        rb_state11.var_5 = (rb_state11.var_5) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x3a795a1c));
        rb_state11.var_6 = (rb_state11.var_6) + (((((0xf640b09b) - (0xcae41ca0)) + (0x41620745)) + (rb_state11.var_4)) ^ (rb_state11.var_3));
        rb_state11.var_7 = (rb_state11.var_7) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0x3ac4d43b));
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_7);
        rb_state11.var_1 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_18));
    }
    if ((rb_state11.var_0) == (0x0)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - ((0xbd1de30) ^ (rb_state13.var_0));
    rb_state13.var_1 = (rb_state13.var_1) + ((0xdcc964ae) - (rb_state13.var_2));
    if ((rb_state13.var_0) == (0x0)) {
        rb_state13.var_3 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0x0)) {
        if (!((rb_state13.var_1) == (rb_state13.var_3))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_1) == (0x8b7c061b)) ? (rb_state14.var_0) : (0x8076ae9f));
    if ((rb_state14.var_0) == (0x777113da)) {
        rb_state14.var_3 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0x777113da)) {
        if (!((rb_state14.var_1) == (rb_state14.var_3))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_0);
    if ((rb_state15.var_1) == (0x39)) {
        rb_state15.var_4 = 0xc4fc69;
        rb_state15.var_5 = 0x29440d92;
        rb_state15.var_6 = (rb_state15.var_6) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0xba4db2e7));
        rb_state15.var_7 = (rb_state15.var_7) ^ (1140737 < rb_input_size ? (uint32_t)rb_input[1140737] : 0x7669cb76);
        rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_5) : (0x325cb41a));
        rb_state15.var_9 = (rb_state15.var_9) - (((0x8be22126) ^ (rb_state15.var_4)) - (rb_state15.var_6));
        rb_state15.var_10 = (rb_state15.var_10) ^ (rb_state15.var_6);
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0xe7e26a24));
        rb_state15.var_12 = (rb_state15.var_12) ^ (((((0xb48ba956) - (rb_state15.var_10)) ^ (rb_state15.var_9)) + (0x1d004a14)) ^ (0xe1adf955));
        rb_state15.var_13 = (rb_state15.var_13) - (((((0x4521220a) + (rb_state15.var_12)) - (0x51994ca3)) ^ (rb_state15.var_11)) ^ (rb_state15.var_7));
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_13);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + ((0xbd1cd334) ^ (rb_state16.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (1393150 < rb_input_size ? (uint32_t)rb_input[1393150] : 0xd4b7cab7);
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
    rb_state18.var_2 = (rb_state18.var_2) + ((0xee432458) + (0x78968302));
    if ((rb_state18.var_2) == (0x348cf60e)) {
        pthread_mutex_lock(&(rb_state18.lock_32));
        rb_state18.var_5 = 0x9c034f76;
        rb_state18.var_6 = 0x93eecb3f;
        rb_state18.var_7 = 0x541d2c49;
        rb_state18.var_8 = (rb_state18.var_8) + (((0x7ec9cc93) - (0xaeda2fac)) ^ (rb_state18.var_6));
        rb_state18.var_9 = (rb_state18.var_9) ^ (rb_state18.var_5);
        rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0x4b8679ff));
        rb_state18.var_11 = (rb_state18.var_11) ^ (rb_state18.var_9);
        rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_10);
        rb_state18.var_13 = (rb_state18.var_13) - (((0xf246f1dd) - (0x92cbc04f)) - (rb_state18.var_11));
        rb_state18.var_14 = (rb_state18.var_14) - (rb_state18.var_12);
        rb_state18.var_15 = (rb_state18.var_15) + (((0xb9a6957b) - (0x67a45668)) - (rb_state18.var_13));
        rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_14);
        rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_15) : (0x667bfe83));
        rb_state18.var_18 = (rb_state18.var_18) - (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_16) : (0x27febca1));
        rb_state18.var_19 = (rb_state18.var_19) ^ (((((0xfc2b9482) ^ (rb_state18.var_6)) - (rb_state18.var_17)) + (rb_state18.var_5)) ^ (rb_state18.var_18));
        rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_19);
        rb_state18.var_3 = rb_state18.var_4;
        pthread_mutex_unlock(&(rb_state18.lock_32));
    }
    if ((rb_state18.var_2) == (0x348cf60e)) {
        if (!((rb_state18.var_3) == (rb_state18.var_4))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + ((0x959af804) ^ (rb_state19.var_0));
    if ((rb_state19.var_1) == (0x3c3ed20f)) {
        rb_state19.var_11 = 0xc4ed0463;
        rb_state19.var_12 = (rb_state19.var_12) ^ (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_11) : (0x568a0b9d));
        rb_state19.var_13 = (rb_state19.var_13) ^ (rb_state19.var_8);
        rb_state19.var_14 = (rb_state19.var_14) + (0x9e75988f);
        rb_state19.var_15 = (rb_state19.var_15) + (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_6) : (0xe75b4eb9));
        rb_state19.var_16 = (rb_state19.var_16) ^ (((((0x9e620d7e) ^ (rb_state19.var_11)) - (rb_state19.var_5)) - (rb_state19.var_12)) - (rb_state19.var_10));
        rb_state19.var_17 = (rb_state19.var_17) ^ (((0x94e7460e) ^ (0x2a748b00)) - (rb_state19.var_13));
        rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0x2a005fef));
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_17) : (0xc140a1c6));
        rb_state19.var_20 = (rb_state19.var_20) ^ (rb_state19.var_18);
        rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_20) : (0x8d468160));
        rb_state19.var_10 = (rb_state19.var_10) ^ (rb_state19.var_21);
        rb_state19.var_3 = rb_state19.var_10;
    }
    if ((rb_state19.var_1) == (0x3c3ed20f)) {
        pthread_mutex_lock(&(rb_state19.lock_31));
        rb_state19.var_23 = 0x94c63297;
        rb_state19.var_24 = 0xa53c020e;
        rb_state19.var_25 = 0xa190111e;
        rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_25) : (0xd486c28b));
        rb_state19.var_27 = (rb_state19.var_27) - (((0xe7f74402) - (0x8b6d8b96)) ^ (rb_state19.var_24));
        rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_23) : (0x154904c2));
        rb_state19.var_29 = (rb_state19.var_29) ^ (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_27) : (0xc1073aea));
        rb_state19.var_30 = (rb_state19.var_30) ^ (((((0xe3bc0b12) - (0xa5c5f76d)) + (rb_state19.var_29)) - (rb_state19.var_28)) ^ (0xcac2cb37));
        rb_state19.var_22 = (rb_state19.var_22) ^ (rb_state19.var_30);
        rb_state19.var_3 = rb_state19.var_22;
        pthread_mutex_unlock(&(rb_state19.lock_31));
    }
    #endif
    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}