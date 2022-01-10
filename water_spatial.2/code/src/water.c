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
            rb_state0.var_0 = (rb_state0.var_0) + (0x372c3805);
            rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_1);
            if ((rb_state0.var_0) == (0x6e58700a)) {
                rb_state0.var_3 = 0x42f2eab8;
                rb_state0.var_4 = (rb_state0.var_4) ^ (((0xaf325172) + (rb_state0.var_3)) - (0x86f3e1f));
                rb_state0.var_5 = (rb_state0.var_5) ^ (rb_state0.var_2);
                rb_state0.var_6 = (rb_state0.var_6) + (rb_state0.var_4);
                rb_state0.var_7 = (rb_state0.var_7) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_5) : (0xea720a5f));
                rb_state0.var_8 = (rb_state0.var_8) + (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_7) : (0xe2932250));
                rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_8);
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ ((0x29940300) - (rb_state2.var_0));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xb4ed2ca4));
            if ((rb_state4.var_0) == (0x0)) {
                if ((rb_state4.var_1) != (0x0)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_25))) {
                        racebench_trigger(4);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - ((0x4dc96585) ^ (0x6a536038));
            rb_state10.var_0 = (rb_state10.var_0) + (0x3c189f74);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) + (0x8b060b82);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (2218353 < rb_input_size ? (uint32_t)rb_input[2218353] : 0xf4d12c5f);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xbf04aa06));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) ^ (0x88e84479);
            if ((rb_state16.var_0) == (0xff072936)) {
                rb_state16.var_2 = rb_state16.var_1;
            }
            if ((rb_state16.var_0) == (0xf8d6e7)) {
                if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                    racebench_trigger(16);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) ^ (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x1596575));
            rb_state17.var_1 = (rb_state17.var_1) ^ (2483281 < rb_input_size ? (uint32_t)rb_input[2483281] : 0xde1a3020);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(WorkStart), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (2194868 < rb_input_size ? (uint32_t)rb_input[2194868] : 0x798008d);
        if ((rb_state2.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state2.lock_17));
            if ((rb_state2.var_2) != (0x0)) {
                rb_state2.var_18 = !((rb_state2.var_2) == (rb_state2.var_12));
            }
            pthread_mutex_unlock(&(rb_state2.lock_17));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (3448905 < rb_input_size ? (uint32_t)rb_input[3448905] : 0x9a8d4826);
        if ((rb_state5.var_1) == (0x455b43b1)) {
            pthread_mutex_lock(&(rb_state5.lock_8));
            rb_state5.var_3 = 0x94753fd9;
            rb_state5.var_4 = 0x11511aca;
            rb_state5.var_5 = 0x1e977c13;
            rb_state5.var_6 = (rb_state5.var_6) - (((((0x73d441fb) ^ (rb_state5.var_3)) ^ (rb_state5.var_4)) ^ (rb_state5.var_2)) ^ (rb_state5.var_5));
            rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_3) == (0x94753fd9)) ? (rb_state5.var_6) : (0xe9077880));
            rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_7);
            pthread_mutex_unlock(&(rb_state5.lock_8));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_1) == (0x0)) {
            rb_state8.var_7 = 0x8ae6937d;
            rb_state8.var_8 = (rb_state8.var_8) - (((0x81d8c271) ^ (rb_state8.var_5)) ^ (rb_state8.var_6));
            rb_state8.var_9 = (rb_state8.var_9) - (((0x1a2429f) - (rb_state8.var_7)) + (0xcde0d9c9));
            rb_state8.var_10 = (rb_state8.var_10) + (((0x6e8a643b) + (0x528d934f)) - (rb_state8.var_4));
            rb_state8.var_11 = (rb_state8.var_11) - (((rb_state8.var_8) == (0x5a919e9d)) ? (rb_state8.var_9) : (0xb0a23846));
            rb_state8.var_12 = (rb_state8.var_12) - (((0x63e71063) ^ (rb_state8.var_10)) ^ (0x999c7928));
            rb_state8.var_13 = (rb_state8.var_13) ^ (rb_state8.var_11);
            rb_state8.var_14 = (rb_state8.var_14) + (rb_state8.var_12);
            rb_state8.var_15 = (rb_state8.var_15) + (((((0x10441d58) ^ (0x914ba93c)) - (rb_state8.var_13)) + (rb_state8.var_14)) ^ (0x38f0a4a6));
            rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (183365 < rb_input_size ? (uint32_t)rb_input[183365] : 0x6778c15);
        if ((rb_state9.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state9.lock_32));
            rb_state9.var_4 = 0x1cf3b418;
            rb_state9.var_5 = 0xe72ca972;
            rb_state9.var_6 = (rb_state9.var_6) - ((0xabcfc519) + (rb_state9.var_3));
            rb_state9.var_7 = (rb_state9.var_7) ^ (((0x255f4160) ^ (rb_state9.var_5)) + (0xc737d6dd));
            rb_state9.var_8 = (rb_state9.var_8) - (((rb_state9.var_4) == (0x1cf3b418)) ? (rb_state9.var_2) : (0x920344c3));
            rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_4) == (0x1cf3b418)) ? (rb_state9.var_6) : (0x575e756b));
            rb_state9.var_10 = (rb_state9.var_10) + (rb_state9.var_7);
            rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_5) == (0xe72ca972)) ? (rb_state9.var_8) : (0x371d7964));
            rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_9);
            rb_state9.var_13 = (rb_state9.var_13) - (((((0x1cc970f1) + (0x8759db53)) ^ (rb_state9.var_6)) - (rb_state9.var_10)) ^ (rb_state9.var_11));
            rb_state9.var_14 = (rb_state9.var_14) ^ (((((0x327f71b8) - (0x23ced52e)) + (rb_state9.var_12)) - (rb_state9.var_13)) + (0x3f78ad1a));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_14);
            rb_state9.var_2 = rb_state9.var_3;
            pthread_mutex_unlock(&(rb_state9.lock_32));
        }
        if ((rb_state9.var_0) == (0x0)) {
            if (!((rb_state9.var_2) == (rb_state9.var_3))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_1) == (0x456a10b1)) {
            rb_state10.var_2 = rb_state10.var_0;
        }
        if ((rb_state10.var_1) == (0x456a10b1)) {
            if (!((rb_state10.var_0) == (rb_state10.var_2))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x5e6db83f)) ? (rb_state11.var_1) : (0xc3f09c6a));
        rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
        if ((rb_state11.var_1) == (0x5e6db83f)) {
            pthread_mutex_lock(&(rb_state11.lock_30));
            rb_state11.var_4 = 0x77c3ef45;
            rb_state11.var_5 = 0x17d2dc8c;
            rb_state11.var_6 = (rb_state11.var_6) + (rb_state11.var_3);
            rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_5) == (0x17d2dc8c)) ? (rb_state11.var_4) : (0xe865bc79));
            rb_state11.var_8 = (rb_state11.var_8) ^ (((((0x7939ffbf) - (rb_state11.var_4)) ^ (rb_state11.var_2)) + (rb_state11.var_6)) - (rb_state11.var_5));
            rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xb8dab068));
            rb_state11.var_10 = (rb_state11.var_10) ^ (((0x9281e172) + (rb_state11.var_8)) ^ (0x84d2a44b));
            rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_7) == (0x883c10bb)) ? (rb_state11.var_9) : (0xcd752dc3));
            rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_10) == (0xf8f7b12b)) ? (rb_state11.var_11) : (0x66212a6c));
            rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_12);
            rb_state11.var_2 = rb_state11.var_3;
            pthread_mutex_unlock(&(rb_state11.lock_30));
        }
        if ((rb_state11.var_1) == (0x5e6db83f)) {
            pthread_mutex_lock(&(rb_state11.lock_30));
            if (!((rb_state11.var_2) == (rb_state11.var_3))) {
                racebench_trigger(11);
            }
            pthread_mutex_unlock(&(rb_state11.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_1) == (0x0)) {
            rb_state12.var_14 = 0xf13aab29;
            rb_state12.var_15 = (rb_state12.var_15) - (rb_state12.var_11);
            rb_state12.var_16 = (rb_state12.var_16) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_14) : (0x29a99c23));
            rb_state12.var_17 = (rb_state12.var_17) - (rb_state12.var_13);
            rb_state12.var_18 = (rb_state12.var_18) - (((0x57de7271) + (rb_state12.var_9)) ^ (0xb8b18dc8));
            rb_state12.var_19 = (rb_state12.var_19) - (((((0x4dc9aa99) ^ (rb_state12.var_15)) - (0x77c6292a)) + (rb_state12.var_14)) - (rb_state12.var_10));
            rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0x7951a59e));
            rb_state12.var_21 = (rb_state12.var_21) ^ (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_18) : (0x72e95805));
            rb_state12.var_22 = (rb_state12.var_22) ^ (((((0xbce36329) - (0xd11f7abf)) + (rb_state12.var_19)) - (rb_state12.var_16)) + (rb_state12.var_20));
            rb_state12.var_23 = (rb_state12.var_23) + (rb_state12.var_21);
            rb_state12.var_24 = (rb_state12.var_24) ^ (((rb_state12.var_22) == (0x42106580)) ? (rb_state12.var_23) : (0x76cf9382));
            rb_state12.var_13 = (rb_state12.var_13) ^ (rb_state12.var_24);
            rb_state12.var_2 = rb_state12.var_13;
        }
        if ((rb_state12.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state12.lock_30));
            rb_state12.var_26 = 0x76d38ccb;
            rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_26) : (0xf29bbb03));
            rb_state12.var_28 = (rb_state12.var_28) + (((0xbab450b0) + (rb_state12.var_18)) + (0xc716f69d));
            rb_state12.var_29 = (rb_state12.var_29) + (((((0xb024e33e) - (rb_state12.var_27)) + (rb_state12.var_19)) + (0xdfeccbbd)) - (rb_state12.var_28));
            rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_29);
            rb_state12.var_2 = rb_state12.var_25;
            pthread_mutex_unlock(&(rb_state12.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_1);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (0x6b89b4d8);
        rb_state14.var_1 = (rb_state14.var_1) - (0x45651d76);
        if ((rb_state14.var_1) == (0xa89eb2fd)) {
            pthread_mutex_lock(&(rb_state14.lock_11));
            rb_state14.var_2 = rb_state14.var_0;
            pthread_mutex_unlock(&(rb_state14.lock_11));
        }
        if ((rb_state14.var_1) == (0xa89eb2fd)) {
            pthread_mutex_lock(&(rb_state14.lock_11));
            if (!((rb_state14.var_0) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
            pthread_mutex_unlock(&(rb_state14.lock_11));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0xac5acf48)) {
            rb_state15.var_3 = 0x384f4796;
            rb_state15.var_4 = (rb_state15.var_4) - (((0xc1c64a6) + (rb_state15.var_3)) + (rb_state15.var_4));
            rb_state15.var_5 = (rb_state15.var_5) + (((rb_state15.var_2) == (0x4b629381)) ? (rb_state15.var_4) : (0x2089ead5));
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_5);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0xa9fcc317)) {
            rb_state17.var_3 = 0xb36d7931;
            rb_state17.var_4 = (rb_state17.var_4) ^ (rb_state17.var_3);
            rb_state17.var_5 = (rb_state17.var_5) ^ (((((0xcee16557) - (rb_state17.var_3)) + (rb_state17.var_2)) ^ (rb_state17.var_4)) ^ (rb_state17.var_4));
            rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_5);
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
    rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_1);
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
    if ((rb_state2.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_17));
        rb_state2.var_6 = 0x97be38d6;
        rb_state2.var_7 = 0xbe0052c6;
        rb_state2.var_8 = (rb_state2.var_8) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_7) : (0xc0963b47));
        rb_state2.var_9 = (rb_state2.var_9) + (1530474 < rb_input_size ? (uint32_t)rb_input[1530474] : 0x753867b2);
        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_6) == (0x97be38d6)) ? (rb_state2.var_8) : (0x30724ecb));
        rb_state2.var_11 = (rb_state2.var_11) - (((rb_state2.var_9) == (0x37)) ? (rb_state2.var_10) : (0xf3ccf522));
        rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_11);
        rb_state2.var_2 = rb_state2.var_5;
        pthread_mutex_unlock(&(rb_state2.lock_17));
    }
    if ((rb_state2.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_17));
        rb_state2.var_13 = 0x66af78e3;
        rb_state2.var_14 = 0xcccd9db1;
        rb_state2.var_15 = (rb_state2.var_15) ^ (((((0x3af3d618) - (rb_state2.var_14)) + (0x8cfc7b28)) + (rb_state2.var_7)) ^ (rb_state2.var_13));
        rb_state2.var_16 = (rb_state2.var_16) ^ (((rb_state2.var_6) == (0x97be38d6)) ? (rb_state2.var_15) : (0x6e3624aa));
        rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_16);
        rb_state2.var_2 = rb_state2.var_12;
        pthread_mutex_unlock(&(rb_state2.lock_17));
    }
    rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_9);
    if ((rb_state2.var_11) == (0x4e32b818)) {
        pthread_mutex_lock(&(rb_state2.lock_34));
        rb_state2.var_21 = 0x982c137a;
        rb_state2.var_22 = 0xa186d419;
        rb_state2.var_23 = (rb_state2.var_23) + (((rb_state2.var_12) == (0xdf8c7eb6)) ? (rb_state2.var_22) : (0xeacff892));
        rb_state2.var_24 = (rb_state2.var_24) + (rb_state2.var_21);
        rb_state2.var_25 = (rb_state2.var_25) + (rb_state2.var_23);
        rb_state2.var_26 = (rb_state2.var_26) ^ (((0x7b1039be) - (rb_state2.var_24)) ^ (rb_state2.var_13));
        rb_state2.var_27 = (rb_state2.var_27) ^ (rb_state2.var_25);
        rb_state2.var_28 = (rb_state2.var_28) + (((0xe0d026b) ^ (rb_state2.var_14)) - (rb_state2.var_26));
        rb_state2.var_29 = (rb_state2.var_29) ^ (((rb_state2.var_27) == (0x0)) ? (rb_state2.var_28) : (0xaf09cbf6));
        rb_state2.var_20 = (rb_state2.var_20) ^ (rb_state2.var_29);
        rb_state2.var_19 = rb_state2.var_20;
        pthread_mutex_unlock(&(rb_state2.lock_34));
    }
    if ((rb_state2.var_11) == (0x4e32b818)) {
        if (!((rb_state2.var_19) == (rb_state2.var_20))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xb17aa1d8));
    rb_state3.var_1 = (rb_state3.var_1) - (589162 < rb_input_size ? (uint32_t)rb_input[589162] : 0xfc56e7e7);
    rb_state3.var_0 = (rb_state3.var_0) + ((0x77bf370c) ^ (rb_state3.var_1));
    if ((rb_state3.var_0) == (0x2eb9e486)) {
        if ((rb_state3.var_2) != (0x0)) {
            if (!((rb_state3.var_2) == (rb_state3.var_30))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xee1d401e));
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xe22f5c78));
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x1e0c91f3));
    if ((rb_state4.var_0) == (0x0)) {
        rb_state4.var_19 = 0x6160ba46;
        rb_state4.var_20 = 0x685cce3e;
        rb_state4.var_21 = (rb_state4.var_21) + (((((0xccbf656c) + (rb_state4.var_19)) + (rb_state4.var_20)) + (0xb251193a)) + (0x7751a2f5));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_12) : (0xdc329b2c));
        rb_state4.var_23 = (rb_state4.var_23) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_21) : (0x5611ca6c));
        rb_state4.var_24 = (rb_state4.var_24) ^ (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_23) : (0x5cbf8a52));
        rb_state4.var_18 = (rb_state4.var_18) + (rb_state4.var_24);
        rb_state4.var_1 = rb_state4.var_18;
    }
    if ((rb_state4.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_38));
        rb_state4.var_26 = (rb_state4.var_26) + (0x60156b9b);
        rb_state4.var_27 = (rb_state4.var_27) - (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_17) : (0x83d4e43b));
        rb_state4.var_28 = (rb_state4.var_28) ^ (((0xb754f6e5) ^ (rb_state4.var_18)) ^ (rb_state4.var_15));
        rb_state4.var_29 = (rb_state4.var_29) - (((rb_state4.var_19) == (0x6160ba46)) ? (rb_state4.var_26) : (0xdc3de584));
        rb_state4.var_30 = (rb_state4.var_30) ^ (((0xb1d1b840) - (rb_state4.var_20)) ^ (rb_state4.var_27));
        rb_state4.var_31 = (rb_state4.var_31) - (((rb_state4.var_21) == (0xc01faa1f)) ? (rb_state4.var_28) : (0x211805ad));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((rb_state4.var_29) == (0x9fea9465)) ? (rb_state4.var_30) : (0x38d3e93));
        rb_state4.var_33 = (rb_state4.var_33) ^ (((0x34eba238) + (rb_state4.var_22)) - (rb_state4.var_31));
        rb_state4.var_34 = (rb_state4.var_34) + (((0x52770f6c) + (rb_state4.var_32)) + (0x33994224));
        rb_state4.var_35 = (rb_state4.var_35) + (((rb_state4.var_23) == (0xc01faa1f)) ? (rb_state4.var_33) : (0x847c97dc));
        rb_state4.var_36 = (rb_state4.var_36) + (((0xa632956c) ^ (rb_state4.var_34)) ^ (0x8aa7e7d2));
        rb_state4.var_37 = (rb_state4.var_37) - (((rb_state4.var_35) == (0xac36ff32)) ? (rb_state4.var_36) : (0xa542b881));
        rb_state4.var_25 = (rb_state4.var_25) + (rb_state4.var_37);
        rb_state4.var_1 = rb_state4.var_25;
        pthread_mutex_unlock(&(rb_state4.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (0x7216679b);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - ((0x5da30c36) - (0x4aafa147));
    if ((rb_state7.var_0) == (0x5be263d8)) {
        pthread_mutex_lock(&(rb_state7.lock_16));
        rb_state7.var_3 = 0x346c48aa;
        rb_state7.var_4 = 0xfeda2053;
        rb_state7.var_5 = (rb_state7.var_5) + (0x25e93303);
        rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_3) : (0xe92f9f59));
        rb_state7.var_7 = (rb_state7.var_7) + (((0x67e0f6af) ^ (0x92c89126)) ^ (rb_state7.var_2));
        rb_state7.var_8 = (rb_state7.var_8) + (((0xf259b054) + (rb_state7.var_5)) - (rb_state7.var_3));
        rb_state7.var_9 = (rb_state7.var_9) ^ (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_6) : (0x9d63b09e));
        rb_state7.var_10 = (rb_state7.var_10) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0x5bdf4f82));
        rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_9);
        rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_10) : (0xf295853));
        rb_state7.var_13 = (rb_state7.var_13) ^ (rb_state7.var_11);
        rb_state7.var_14 = (rb_state7.var_14) + (rb_state7.var_12);
        rb_state7.var_15 = (rb_state7.var_15) + (((((0x96c5bf0e) - (rb_state7.var_13)) ^ (rb_state7.var_14)) ^ (0x3dd24fd4)) + (0xda8a938b));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_15);
        pthread_mutex_unlock(&(rb_state7.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0x5af9a8b9));
    if ((rb_state8.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_16));
        rb_state8.var_4 = 0x294ce8e3;
        rb_state8.var_5 = (rb_state8.var_5) ^ (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0x89c4d820));
        rb_state8.var_6 = (rb_state8.var_6) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0xdb495cee));
        rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_6);
        rb_state8.var_2 = rb_state8.var_3;
        pthread_mutex_unlock(&(rb_state8.lock_16));
    }
    if ((rb_state8.var_1) == (0x0)) {
        if (!((rb_state8.var_2) == (rb_state8.var_3))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - ((0x93874fc5) - (rb_state10.var_1));
    if ((rb_state10.var_1) == (0x456a10b1)) {
        pthread_mutex_lock(&(rb_state10.lock_10));
        rb_state10.var_3 = (rb_state10.var_3) - (0xc22664ff);
        rb_state10.var_4 = (rb_state10.var_4) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0xd0bb7288));
        rb_state10.var_5 = (rb_state10.var_5) + ((0xe6a6afd7) - (rb_state10.var_5));
        rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_1) : (0x2bbf71d9));
        rb_state10.var_7 = (rb_state10.var_7) ^ (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_4) : (0x4f4a1319));
        rb_state10.var_8 = (rb_state10.var_8) ^ (((((0x3f0dc001) + (rb_state10.var_6)) + (rb_state10.var_6)) + (0xd768fa07)) + (rb_state10.var_5));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((((0xfc2ec37d) + (rb_state10.var_8)) ^ (0xd41153b7)) ^ (rb_state10.var_7)) ^ (0xc14badfa));
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_9);
        pthread_mutex_unlock(&(rb_state10.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0x5e6db83f)) {
        rb_state11.var_13 = 0xa3cb517d;
        rb_state11.var_14 = 0x4ff836f0;
        rb_state11.var_15 = (rb_state11.var_15) - (0xb624bfa4);
        rb_state11.var_16 = (rb_state11.var_16) ^ ((0x766b8c3d) - (0x209820a7));
        rb_state11.var_17 = (rb_state11.var_17) + (rb_state11.var_13);
        rb_state11.var_18 = (rb_state11.var_18) + (rb_state11.var_8);
        rb_state11.var_19 = (rb_state11.var_19) ^ (rb_state11.var_14);
        rb_state11.var_20 = (rb_state11.var_20) ^ (((((0x2308284d) + (rb_state11.var_15)) + (rb_state11.var_16)) - (rb_state11.var_9)) ^ (0x84f0db15));
        rb_state11.var_21 = (rb_state11.var_21) - (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0xf82d229b));
        rb_state11.var_22 = (rb_state11.var_22) - (rb_state11.var_19);
        rb_state11.var_23 = (rb_state11.var_23) - (((0x610828b1) + (rb_state11.var_10)) + (rb_state11.var_20));
        rb_state11.var_24 = (rb_state11.var_24) ^ (rb_state11.var_21);
        rb_state11.var_25 = (rb_state11.var_25) - (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_22) : (0x52c43f0d));
        rb_state11.var_26 = (rb_state11.var_26) - (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_24) : (0xb2479038));
        rb_state11.var_27 = (rb_state11.var_27) - (rb_state11.var_25);
        rb_state11.var_28 = (rb_state11.var_28) - (rb_state11.var_26);
        rb_state11.var_29 = (rb_state11.var_29) + (((((0xa5135572) - (rb_state11.var_12)) - (0x2ce5776)) + (rb_state11.var_28)) - (rb_state11.var_27));
        rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_29);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0x6088f946) - (0x5a13207));
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0xdde95431));
    if ((rb_state12.var_1) == (0x0)) {
        if ((rb_state12.var_2) != (0x0)) {
            if (!((rb_state12.var_2) == (rb_state12.var_25))) {
                racebench_trigger(12);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - ((0x1a9c044f) + (0x43e2f52));
    rb_state13.var_1 = (rb_state13.var_1) + ((0x264a992c) ^ (rb_state13.var_1));
    if ((rb_state13.var_0) == (0x39ab654e)) {
        rb_state13.var_2 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0x39ab654e)) {
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (2622464 < rb_input_size ? (uint32_t)rb_input[2622464] : 0x19cc343c);
    if ((rb_state14.var_1) == (0xa89eb2fd)) {
        rb_state14.var_3 = 0x65cfd624;
        rb_state14.var_4 = 0xf909c093;
        rb_state14.var_5 = (rb_state14.var_5) ^ (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_3) : (0xad944113));
        rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_2) : (0x5193e3f2));
        rb_state14.var_7 = (rb_state14.var_7) + (((0xd6866f5e) - (0xc510d327)) + (rb_state14.var_4));
        rb_state14.var_8 = (rb_state14.var_8) + (((0x88e771c) + (rb_state14.var_5)) - (rb_state14.var_5));
        rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_7) : (0xe9faf6f0));
        rb_state14.var_10 = (rb_state14.var_10) ^ (((((0x1654220d) - (rb_state14.var_6)) ^ (0x9d3f8c32)) - (rb_state14.var_8)) ^ (rb_state14.var_9));
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_10);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0x709f0ab6));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0xff072907) - (rb_state16.var_0));
    if ((rb_state16.var_0) == (0xf8d6e7)) {
        rb_state16.var_3 = 0x3a42bbc3;
        rb_state16.var_4 = 0xd863452b;
        rb_state16.var_5 = (rb_state16.var_5) - (1821838 < rb_input_size ? (uint32_t)rb_input[1821838] : 0xc3f18d37);
        rb_state16.var_6 = (rb_state16.var_6) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_4) : (0xdae5d0d9));
        rb_state16.var_7 = (rb_state16.var_7) ^ (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_3) : (0x79c6af7a));
        rb_state16.var_8 = (rb_state16.var_8) + (((((0xb06a4f47) - (0x67cd011a)) - (rb_state16.var_2)) - (rb_state16.var_5)) + (rb_state16.var_5));
        rb_state16.var_9 = (rb_state16.var_9) ^ (((((0x388570ce) + (rb_state16.var_7)) + (rb_state16.var_6)) - (0xb3313880)) + (rb_state16.var_6));
        rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0xc2dbf8c9));
        rb_state16.var_11 = (rb_state16.var_11) - (rb_state16.var_9);
        rb_state16.var_12 = (rb_state16.var_12) - (((0x779a811f) ^ (0x4309a427)) - (rb_state16.var_10));
        rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_11) : (0x83bae3b0));
        rb_state16.var_14 = (rb_state16.var_14) - (((((0xa20bc203) + (rb_state16.var_9)) + (rb_state16.var_12)) + (0xaf6eb3f)) + (rb_state16.var_13));
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - ((0x56033ce9) + (rb_state17.var_1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0x63fe3b53) - (0x1aa869ea));
    rb_state19.var_1 = (rb_state19.var_1) - (0x9c32109);
    if ((rb_state19.var_0) == (0xf63cdef7)) {
        pthread_mutex_lock(&(rb_state19.lock_41));
        rb_state19.var_14 = 0x23ba890;
        rb_state19.var_15 = 0xc487013e;
        rb_state19.var_16 = 0x2e7e708b;
        rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0xd6172315));
        rb_state19.var_18 = (rb_state19.var_18) - (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_16) : (0x69dcee12));
        rb_state19.var_19 = (rb_state19.var_19) + (((0xe4c96a70) + (rb_state19.var_17)) + (rb_state19.var_7));
        rb_state19.var_20 = (rb_state19.var_20) ^ (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_19) : (0x8e5ebd1a));
        rb_state19.var_13 = (rb_state19.var_13) + (rb_state19.var_20);
        rb_state19.var_2 = rb_state19.var_13;
        pthread_mutex_unlock(&(rb_state19.lock_41));
    }
    if ((rb_state19.var_0) == (0xf63cdef7)) {
        pthread_mutex_lock(&(rb_state19.lock_41));
        rb_state19.var_22 = 0xa181b4c9;
        rb_state19.var_23 = (rb_state19.var_23) ^ (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_10) : (0xc01c890d));
        rb_state19.var_24 = (rb_state19.var_24) + (((0x88ca7a77) + (rb_state19.var_9)) + (rb_state19.var_12));
        rb_state19.var_25 = (rb_state19.var_25) - (1186107 < rb_input_size ? (uint32_t)rb_input[1186107] : 0x8db4013);
        rb_state19.var_26 = (rb_state19.var_26) - (rb_state19.var_22);
        rb_state19.var_27 = (rb_state19.var_27) ^ (((0x6c45f34e) ^ (rb_state19.var_8)) + (rb_state19.var_13));
        rb_state19.var_28 = (rb_state19.var_28) + (((0xc3dd7ea) - (rb_state19.var_23)) + (0x871830f8));
        rb_state19.var_29 = (rb_state19.var_29) ^ (((((0xe3b9cef6) - (rb_state19.var_14)) + (rb_state19.var_24)) ^ (rb_state19.var_25)) ^ (0x1b1c2147));
        rb_state19.var_30 = (rb_state19.var_30) - (rb_state19.var_26);
        rb_state19.var_31 = (rb_state19.var_31) - (((0xdefb49ca) ^ (rb_state19.var_27)) - (rb_state19.var_15));
        rb_state19.var_32 = (rb_state19.var_32) + (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_28) : (0xe7ec7d4f));
        rb_state19.var_33 = (rb_state19.var_33) ^ (((0xdd33f166) - (rb_state19.var_17)) ^ (rb_state19.var_29));
        rb_state19.var_34 = (rb_state19.var_34) ^ (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_30) : (0x91414432));
        rb_state19.var_35 = (rb_state19.var_35) + (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_31) : (0x4c85ea50));
        rb_state19.var_36 = (rb_state19.var_36) ^ (rb_state19.var_32);
        rb_state19.var_37 = (rb_state19.var_37) ^ (rb_state19.var_33);
        rb_state19.var_38 = (rb_state19.var_38) ^ (((((0xa98dbef9) ^ (rb_state19.var_34)) + (rb_state19.var_35)) - (0x3dcd922d)) ^ (0x955b7fd9));
        rb_state19.var_39 = (rb_state19.var_39) - (((rb_state19.var_36) == (0x0)) ? (rb_state19.var_37) : (0x9cec4cf));
        rb_state19.var_40 = (rb_state19.var_40) + (((((0x4a260730) + (rb_state19.var_20)) - (rb_state19.var_38)) + (0x8d188c19)) ^ (rb_state19.var_39));
        rb_state19.var_21 = (rb_state19.var_21) ^ (rb_state19.var_40);
        rb_state19.var_2 = rb_state19.var_21;
        pthread_mutex_unlock(&(rb_state19.lock_41));
    }
    #endif
    ProcID = gl->Index++;
    { pthread_mutex_unlock(&(gl->IndexLock)); };

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (2091403 < rb_input_size ? (uint32_t)rb_input[2091403] : 0x766b24e2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_10 = (rb_state2.var_10) + (0x6d01ca18);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + ((0x83414f8a) + (rb_state3.var_1));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (0x455b43d1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0xb15a717b);
    if ((rb_state7.var_0) == (0x6ed5cec7)) {
        rb_state7.var_2 = rb_state7.var_1;
    }
    if ((rb_state7.var_0) == (0x5be263d8)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x7424a5ae) - (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ ((0xaa8f9672) ^ (0x6df9d45b));
    rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x39ab654e)) {
        pthread_mutex_lock(&(rb_state13.lock_3));
        rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_2);
        pthread_mutex_unlock(&(rb_state13.lock_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x858a657e) ^ (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (0xf1831301);
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x83d247d0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (1038300 < rb_input_size ? (uint32_t)rb_input[1038300] : 0x4ae382e8);
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
    if ((rb_state17.var_0) == (0xa9fcc317)) {
        rb_state17.var_2 = rb_state17.var_1;
    }
    if ((rb_state17.var_0) == (0xa9fcc317)) {
        pthread_mutex_lock(&(rb_state17.lock_6));
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
        pthread_mutex_unlock(&(rb_state17.lock_6));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x44cade54));
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x4955d169)) ? (rb_state19.var_1) : (0x425d0705));
    rb_state19.var_1 = (rb_state19.var_1) - (723081 < rb_input_size ? (uint32_t)rb_input[723081] : 0xcb644c71);
    if ((rb_state19.var_0) == (0xf63cdef7)) {
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_21))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    ProcID = ProcID % NumProcs;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x6e58700a)) {
        rb_state0.var_2 = rb_state0.var_1;
    }
    if ((rb_state0.var_0) == (0x6e58700a)) {
        if (!((rb_state0.var_1) == (rb_state0.var_2))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_11) == (0x4e32b818)) {
        rb_state2.var_30 = 0xb1cf7da3;
        rb_state2.var_31 = 0x4f35e99c;
        rb_state2.var_32 = (rb_state2.var_32) - (((((0xd9671b57) + (rb_state2.var_15)) ^ (0xef62008e)) + (rb_state2.var_30)) + (0x963880ca));
        rb_state2.var_33 = (rb_state2.var_33) + (((rb_state2.var_31) == (0x4f35e99c)) ? (rb_state2.var_32) : (0x70d2b520));
        rb_state2.var_19 = (rb_state2.var_19) + (rb_state2.var_33);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x2eb9e486)) {
        pthread_mutex_lock(&(rb_state3.lock_43));
        rb_state3.var_19 = 0x1fa66309;
        rb_state3.var_20 = (rb_state3.var_20) - (0x64462997);
        rb_state3.var_21 = (rb_state3.var_21) - (((rb_state3.var_19) == (0x1fa66309)) ? (rb_state3.var_13) : (0x5598a6b0));
        rb_state3.var_22 = (rb_state3.var_22) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_20) : (0xf20fafd9));
        rb_state3.var_23 = (rb_state3.var_23) - (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_21) : (0x91676f94));
        rb_state3.var_24 = (rb_state3.var_24) ^ (rb_state3.var_22);
        rb_state3.var_25 = (rb_state3.var_25) + (rb_state3.var_23);
        rb_state3.var_26 = (rb_state3.var_26) + (rb_state3.var_24);
        rb_state3.var_27 = (rb_state3.var_27) + (((0x6da4488e) ^ (rb_state3.var_25)) + (0x7f2b2208));
        rb_state3.var_28 = (rb_state3.var_28) + (((0xfd4932ec) - (rb_state3.var_26)) - (0x7dc27c5a));
        rb_state3.var_29 = (rb_state3.var_29) + (((((0x70820406) + (0xa7cd0d5)) ^ (0x13ef7af0)) + (rb_state3.var_27)) - (rb_state3.var_28));
        rb_state3.var_18 = (rb_state3.var_18) + (rb_state3.var_29);
        rb_state3.var_2 = rb_state3.var_18;
        pthread_mutex_unlock(&(rb_state3.lock_43));
    }
    if ((rb_state3.var_0) == (0x2eb9e486)) {
        pthread_mutex_lock(&(rb_state3.lock_43));
        rb_state3.var_31 = 0xe4d41738;
        rb_state3.var_32 = 0xd0e8cb15;
        rb_state3.var_33 = (rb_state3.var_33) + (rb_state3.var_16);
        rb_state3.var_34 = (rb_state3.var_34) ^ (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_31) : (0x339fb4b8));
        rb_state3.var_35 = (rb_state3.var_35) ^ (rb_state3.var_32);
        rb_state3.var_36 = (rb_state3.var_36) - (((0x87948a98) + (rb_state3.var_33)) + (0x16422189));
        rb_state3.var_37 = (rb_state3.var_37) + (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_34) : (0xc111c8a3));
        rb_state3.var_38 = (rb_state3.var_38) - (((0x3599e13f) + (rb_state3.var_18)) + (rb_state3.var_35));
        rb_state3.var_39 = (rb_state3.var_39) - (((rb_state3.var_36) == (0x622953df)) ? (rb_state3.var_37) : (0x247b4118));
        rb_state3.var_40 = (rb_state3.var_40) - (rb_state3.var_38);
        rb_state3.var_41 = (rb_state3.var_41) - (((rb_state3.var_19) == (0x1fa66309)) ? (rb_state3.var_39) : (0x6cd72dc1));
        rb_state3.var_42 = (rb_state3.var_42) - (((((0x783fc143) + (rb_state3.var_40)) + (0xae891f1e)) - (rb_state3.var_20)) ^ (rb_state3.var_41));
        rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_42);
        rb_state3.var_2 = rb_state3.var_30;
        pthread_mutex_unlock(&(rb_state3.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x455b43b1)) {
        rb_state5.var_2 = rb_state5.var_0;
    }
    if ((rb_state5.var_1) == (0x455b43b1)) {
        if (!((rb_state5.var_0) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x7424a5ae)) ? (rb_state8.var_0) : (0xd659d606));
    rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x0)) {
        rb_state9.var_15 = 0x29afae6f;
        rb_state9.var_16 = (rb_state9.var_16) - (3233379 < rb_input_size ? (uint32_t)rb_input[3233379] : 0x78506d79);
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_9) : (0x15bceb7e));
        rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_7) : (0x51fc2950));
        rb_state9.var_19 = (rb_state9.var_19) + (rb_state9.var_8);
        rb_state9.var_20 = (rb_state9.var_20) + (rb_state9.var_16);
        rb_state9.var_21 = (rb_state9.var_21) ^ (((0xbbe22fc4) + (rb_state9.var_17)) - (rb_state9.var_11));
        rb_state9.var_22 = (rb_state9.var_22) - (((0x9a177bf1) ^ (rb_state9.var_18)) - (0x73792cb));
        rb_state9.var_23 = (rb_state9.var_23) + (rb_state9.var_19);
        rb_state9.var_24 = (rb_state9.var_24) + (((0xc8f396c7) - (rb_state9.var_12)) - (rb_state9.var_20));
        rb_state9.var_25 = (rb_state9.var_25) - (((0x36a0fbce) ^ (0xf47182a3)) ^ (rb_state9.var_21));
        rb_state9.var_26 = (rb_state9.var_26) ^ (((0x45475dc6) - (rb_state9.var_22)) ^ (0x39c88de8));
        rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_23) : (0xf23e7671));
        rb_state9.var_28 = (rb_state9.var_28) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_24) : (0x2c7cca7a));
        rb_state9.var_29 = (rb_state9.var_29) - (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_26) : (0x5b6b0d8f));
        rb_state9.var_30 = (rb_state9.var_30) - (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0xf63fc87f));
        rb_state9.var_31 = (rb_state9.var_31) - (((rb_state9.var_29) == (0x0)) ? (rb_state9.var_30) : (0x9c13cbd1));
        rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x28fd336e)) ? (rb_state10.var_0) : (0x6f04f218));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (0x5e6db83f);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0xe7cecff)) ? (rb_state15.var_0) : (0xaa9671b3));
    if ((rb_state15.var_0) == (0xac5acf48)) {
        pthread_mutex_lock(&(rb_state15.lock_6));
        rb_state15.var_2 = rb_state15.var_1;
        pthread_mutex_unlock(&(rb_state15.lock_6));
    }
    if ((rb_state15.var_0) == (0xac5acf48)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (0x6dafb0b);
    rb_state18.var_0 = (rb_state18.var_0) + (3338334 < rb_input_size ? (uint32_t)rb_input[3338334] : 0x2bc9d991);
    if ((rb_state18.var_0) == (0x8afb4631)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0x8afb4631)) {
        rb_state18.var_3 = (rb_state18.var_3) - (0xf3d6d72d);
        rb_state18.var_4 = (rb_state18.var_4) ^ (3307626 < rb_input_size ? (uint32_t)rb_input[3307626] : 0xe5489336);
        rb_state18.var_5 = (rb_state18.var_5) - ((0x3440f9ad) + (rb_state18.var_3));
        rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_2) : (0x5e4fadf));
        rb_state18.var_7 = (rb_state18.var_7) ^ (rb_state18.var_3);
        rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_4);
        rb_state18.var_9 = (rb_state18.var_9) - (((((0xc2956187) + (rb_state18.var_4)) - (rb_state18.var_5)) ^ (rb_state18.var_6)) + (rb_state18.var_5));
        rb_state18.var_10 = (rb_state18.var_10) + (((((0x243415c8) + (0x8a58af3c)) ^ (rb_state18.var_6)) ^ (rb_state18.var_7)) - (rb_state18.var_8));
        rb_state18.var_11 = (rb_state18.var_11) + (((0x3848972b) ^ (0xee840560)) ^ (rb_state18.var_9));
        rb_state18.var_12 = (rb_state18.var_12) + (((rb_state18.var_10) == (0xa2a5ed9f)) ? (rb_state18.var_11) : (0x805c4899));
        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_12);
    }
    if ((rb_state18.var_0) == (0x8afb4631)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID);
}