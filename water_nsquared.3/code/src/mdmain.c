#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "stdio.h"
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
    double XTT;
    int i;
    double POTA, POTR, POTRF;
    double XVIR, AVGT, TEN;
    double TTMV = 0.0, TKIN = 0.0, TVIR = 0.0;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x2c38f453));
    #endif
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

        if ((ProcID == 0) && (i >= 2)) {
            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (gl->intrastart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
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
            gl->intratime += gl->intraend - gl->intrastart;
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
                fprintf(six, "     %5d %14.5lf %12.5lf %12.5lf  \
                %12.5lf\n %16.3lf %16.5lf %16.5lf\n",
                    i, TEN, POTA, POTR, POTRF, XTT, AVGT, XVIR);
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