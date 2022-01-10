#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include <stdio.h>

#include "parameters.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "cnst.h"
#include "mddata.h"
#include "fileio.h"
#include "split.h"
#include "global.h"

double MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID)
int NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1;
unsigned ProcID;

{

    double TVIR = 0.0;
    double TTMV = 0.0;
    double TKIN = 0.0;
    double XTT;
    int i, j, k;
    double POTA, POTR, POTRF;
    double XVIR, AVGT, TEN;
    struct link *curr_ptr;
    struct list_of_boxes *new_box, *curr_box;

    for (i = start_end[ProcID]->box[XDIR][FIRST]; i <= start_end[ProcID]->box[XDIR][LAST]; i++) {
        for (j = start_end[ProcID]->box[YDIR][FIRST]; j <= start_end[ProcID]->box[YDIR][LAST]; j++) {
            for (k = start_end[ProcID]->box[ZDIR][FIRST]; k <= start_end[ProcID]->box[ZDIR][LAST]; k++) {
                new_box = (box_list *)malloc(sizeof(box_list));
                ;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_2) : (0xe8f83b47));
                #endif
                new_box->coord[XDIR] = i;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_1 = (rb_state3.var_1) + (2203834 < rb_input_size ? (uint32_t)rb_input[2203834] : 0x6d2c9bb8);
                rb_state3.var_2 = (rb_state3.var_2) ^ (0x7a0c6d34);
                #endif
                new_box->coord[YDIR] = j;
                new_box->coord[ZDIR] = k;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_2);
                #endif
                new_box->next_box = NULL;
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x73ba20a4));
                rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xe02e5ecd));
                rb_state3.var_1 = (rb_state3.var_1) ^ (0x466d6cc2);
                #endif
                curr_box = my_boxes[ProcID];
                if (curr_box == NULL) {
                    my_boxes[ProcID] = new_box;
                } else {
                    while (curr_box->next_box != NULL) {
                        #ifdef RACEBENCH_BUG_3
                        rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_1);
                        if ((rb_state3.var_1) == (0x7f829600)) {
                            pthread_mutex_lock(&(rb_state3.lock_25));
                            rb_state3.var_5 = 0x8b9442bf;
                            rb_state3.var_6 = 0x99266d9a;
                            rb_state3.var_7 = (rb_state3.var_7) ^ (3161072 < rb_input_size ? (uint32_t)rb_input[3161072] : 0x96de7098);
                            rb_state3.var_8 = (rb_state3.var_8) - ((0xbadf3909) ^ (rb_state3.var_4));
                            rb_state3.var_9 = (rb_state3.var_9) - (((0x9d083fa3) + (rb_state3.var_5)) + (0x90b9ce0c));
                            rb_state3.var_10 = (rb_state3.var_10) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_3) : (0xc6ff74f6));
                            rb_state3.var_11 = (rb_state3.var_11) - (((((0xe0d89afe) + (rb_state3.var_7)) ^ (rb_state3.var_6)) ^ (rb_state3.var_8)) - (rb_state3.var_5));
                            rb_state3.var_12 = (rb_state3.var_12) ^ (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_9) : (0xddba9fd3));
                            rb_state3.var_13 = (rb_state3.var_13) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_10) : (0x711f640a));
                            rb_state3.var_14 = (rb_state3.var_14) ^ (rb_state3.var_11);
                            rb_state3.var_15 = (rb_state3.var_15) - (((((0xcc1c282) ^ (rb_state3.var_13)) - (0xa7173afb)) - (rb_state3.var_12)) - (0xd6bc9a67));
                            rb_state3.var_16 = (rb_state3.var_16) ^ (rb_state3.var_14);
                            rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_15);
                            rb_state3.var_18 = (rb_state3.var_18) + (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_17) : (0x44c0b835));
                            rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_18);
                            rb_state3.var_3 = rb_state3.var_4;
                            pthread_mutex_unlock(&(rb_state3.lock_25));
                        }
                        if ((rb_state3.var_1) == (0xff052c00)) {
                            pthread_mutex_lock(&(rb_state3.lock_25));
                            rb_state3.var_19 = 0x80b3d885;
                            rb_state3.var_20 = 0x595a8dd5;
                            rb_state3.var_21 = (rb_state3.var_21) ^ (((((0x9c31e9a4) - (rb_state3.var_11)) ^ (rb_state3.var_9)) ^ (rb_state3.var_10)) ^ (rb_state3.var_19));
                            rb_state3.var_22 = (rb_state3.var_22) + (rb_state3.var_20);
                            rb_state3.var_23 = (rb_state3.var_23) - (rb_state3.var_21);
                            rb_state3.var_24 = (rb_state3.var_24) + (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_23) : (0x3d94a2f2));
                            rb_state3.var_3 = (rb_state3.var_3) + (rb_state3.var_24);
                            pthread_mutex_unlock(&(rb_state3.lock_25));
                        }
                        if ((rb_state3.var_1) == (0xff052c00)) {
                            if (!((rb_state3.var_3) == (rb_state3.var_4))) {
                                racebench_trigger(3);
                            }
                        }
                        #endif
                        curr_box = curr_box->next_box;
                    }
                    curr_box->next_box = new_box;
                }
            }
        }
    }

    INTRAF(&gl->VIR, ProcID);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(gl->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (gl->start).cycle;
        if (++(gl->start).counter != (NumProcs)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (gl->start).cycle) {
                Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (gl->start).cycle = !(gl->start).cycle;
            (gl->start).counter = 0;
            Error = pthread_cond_broadcast(&(gl->start).cv);
        }
        pthread_mutex_unlock(&(gl->start).mutex);
    };

    INTERF(ACC, &gl->VIR, ProcID);

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(gl->start).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (gl->start).cycle;
        if (++(gl->start).counter != (NumProcs)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (gl->start).cycle) {
                Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (gl->start).cycle = !(gl->start).cycle;
            (gl->start).counter = 0;
            Error = pthread_cond_broadcast(&(gl->start).cv);
        }
        pthread_mutex_unlock(&(gl->start).mutex);
    };

    for (i = 1; i <= NSTEP; i++) {
        TTMV = TTMV + 1.00;

        if (ProcID == 0) {
            int dir;
            if (i >= 2) {
                {
                    struct timeval FullTime;

                    gettimeofday(&FullTime, NULL);
                    (gl->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
                };
            }
            gl->VIR = 0.0;
            gl->POTA = 0.0;
            gl->POTR = 0.0;
            gl->POTRF = 0.0;
            for (dir = XDIR; dir <= ZDIR; dir++) {
                gl->SUM[dir] = 0.0;
            }
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->intrastart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
        }

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(gl->start).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (gl->start).cycle;
            if (++(gl->start).counter != (NumProcs)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (gl->start).cycle) {
                    Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (gl->start).cycle = !(gl->start).cycle;
                (gl->start).counter = 0;
                Error = pthread_cond_broadcast(&(gl->start).cv);
            }
            pthread_mutex_unlock(&(gl->start).mutex);
        };

        PREDIC(TLC, NORD1, ProcID);
        INTRAF(&gl->VIR, ProcID);

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(gl->start).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (gl->start).cycle;
            if (++(gl->start).counter != (NumProcs)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (gl->start).cycle) {
                    Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (gl->start).cycle = !(gl->start).cycle;
                (gl->start).counter = 0;
                Error = pthread_cond_broadcast(&(gl->start).cv);
            }
            pthread_mutex_unlock(&(gl->start).mutex);
        };

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
            gl->intratime += gl->intraend - gl->intrastart;
        }

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
        }

        INTERF(FORCES, &gl->VIR, ProcID);

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
            gl->intertime += gl->interend - gl->interstart;
        }

        CORREC(PCC, NORD1, ProcID);

        BNDRY(ProcID);

        KINETI(NMOL, gl->SUM, HMAS, OMAS, ProcID);

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(gl->start).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (gl->start).cycle;
            if (++(gl->start).counter != (NumProcs)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (gl->start).cycle) {
                    Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (gl->start).cycle = !(gl->start).cycle;
                (gl->start).counter = 0;
                Error = pthread_cond_broadcast(&(gl->start).cv);
            }
            pthread_mutex_unlock(&(gl->start).mutex);
        };

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->intraend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
            gl->intratime += gl->intraend - gl->interend;
        }

        TKIN = TKIN + gl->SUM[0] + gl->SUM[1] + gl->SUM[2];
        TVIR = TVIR - gl->VIR;

        if (((i % NPRINT) == 0) || ((NSAVE > 0) && ((i % NSAVE) == 0))) {

            if ((ProcID == 0) && (i >= 2)) {
                {
                    struct timeval FullTime;

                    gettimeofday(&FullTime, NULL);
                    (gl->interstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
                };
            }

            POTENG(&gl->POTA, &gl->POTR, &gl->POTRF, ProcID);

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                Error = pthread_mutex_lock(&(gl->start).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (gl->start).cycle;
                if (++(gl->start).counter != (NumProcs)) {
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (gl->start).cycle) {
                        Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (gl->start).cycle = !(gl->start).cycle;
                    (gl->start).counter = 0;
                    Error = pthread_cond_broadcast(&(gl->start).cv);
                }
                pthread_mutex_unlock(&(gl->start).mutex);
            };

            if ((ProcID == 0) && (i >= 2)) {
                {
                    struct timeval FullTime;

                    gettimeofday(&FullTime, NULL);
                    (gl->interend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
                };
                gl->intertime += gl->interend - gl->interstart;
            }

            POTA = gl->POTA * FPOT;
            POTR = gl->POTR * FPOT;
            POTRF = gl->POTRF * FPOT;
            XVIR = TVIR * FPOT * 0.50 / TTMV;
            AVGT = TKIN * FKIN * TEMP * 2.00 / (3.00 * TTMV);
            TEN = (gl->SUM[0] + gl->SUM[1] + gl->SUM[2]) * FKIN;
            XTT = POTA + POTR + POTRF + TEN;

            if ((i % NPRINT) == 0 && ProcID == 0) {
                { pthread_mutex_lock(&(gl->IOLock)); };
                fprintf(six, "     %5d %14.5lf %12.5lf %12.5lf %12.5lf \n", i, TEN, POTA, POTR, POTRF);
                fprintf(six, " %16.3lf %16.5lf %16.5lf\n", XTT, AVGT, XVIR);
                fflush(six);
                { pthread_mutex_unlock(&(gl->IOLock)); };
            }
        }

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(gl->start).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (gl->start).cycle;
            if (++(gl->start).counter != (NumProcs)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (gl->start).cycle) {
                    Error = pthread_cond_wait(&(gl->start).cv, &(gl->start).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (gl->start).cycle = !(gl->start).cycle;
                (gl->start).counter = 0;
                Error = pthread_cond_broadcast(&(gl->start).cv);
            }
            pthread_mutex_unlock(&(gl->start).mutex);
        };

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->trackend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
            gl->tracktime += gl->trackend - gl->trackstart;
        }
    }

    return (XTT);
}