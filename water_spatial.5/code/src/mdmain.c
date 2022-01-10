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
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) - (0x65228e4c);
                if ((rb_state10.var_0) == (0xfcc92fce)) {
                    rb_state10.var_4 = rb_state10.var_1;
                }
                if ((rb_state10.var_0) == (0x54bed963)) {
                    if (!((rb_state10.var_1) == (rb_state10.var_4))) {
                        racebench_trigger(10);
                    }
                }
                #endif
                new_box = (box_list *)malloc(sizeof(box_list));
                ;
                new_box->coord[XDIR] = i;
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_1 = (rb_state10.var_1) - (3346888 < rb_input_size ? (uint32_t)rb_input[3346888] : 0x7f302b45);
                rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_2) : (0xcd657e6e));
                #endif
                new_box->coord[YDIR] = j;
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_3 = (rb_state10.var_3) ^ ((0xca86d1b5) + (rb_state10.var_1));
                #endif
                new_box->coord[ZDIR] = k;
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - ((0xb3a8fbd8) + (0x995b1166));
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_0);
                #endif
                new_box->next_box = NULL;
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_2 = (rb_state10.var_2) - (((rb_state10.var_0) == (0x35bae368)) ? (rb_state10.var_3) : (0xd36ac3b));
                #endif
                curr_box = my_boxes[ProcID];
                if (curr_box == NULL) {
                    my_boxes[ProcID] = new_box;
                } else {
                    while (curr_box->next_box != NULL) {
                        #ifdef RACEBENCH_BUG_10
                        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_3);
                        rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_1);
                        #endif
                        curr_box = curr_box->next_box;
                    }
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) - ((0xfe560611) + (rb_state10.var_1));
                    rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_3);
                    if ((rb_state10.var_0) == (0x54bed963)) {
                        rb_state10.var_5 = 0xd9803ae3;
                        rb_state10.var_6 = (rb_state10.var_6) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_4) : (0x50d2b894));
                        rb_state10.var_7 = (rb_state10.var_7) - (((0x1792b4a3) - (rb_state10.var_6)) ^ (rb_state10.var_5));
                        rb_state10.var_8 = (rb_state10.var_8) ^ (((((0xc7480722) + (0xb94320e8)) ^ (rb_state10.var_7)) + (rb_state10.var_7)) - (rb_state10.var_6));
                        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_8);
                    }
                    #endif
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