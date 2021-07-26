#ifdef __cplusplus
extern "C"
#endif
void racebench_bug1(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug1(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug4(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug5(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug5(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug10(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug10(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug13(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug13(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug16(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug16(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug18(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug19(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug19(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug21(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug27(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug30(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug32(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug33(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug35(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug39(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug64(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug64(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug71(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug71(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug73(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug74(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug78(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug79(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug79(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug83(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug83(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug85(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug85(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug90(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug93(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug99(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug99(int);

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

double MDMAIN(NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1, ProcID) int NFSV, NFRST, NSTEP, NRST, NPRINT, NSAVE, LKT, NORD1;
unsigned ProcID;

{

    double TVIR = 0.0; racebench_bug7(1); racebench_bug59(1);
    double TTMV = 0.0; racebench_bug59(0); racebench_bug73(0);
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
                new_box = (box_list *)malloc(sizeof(box_list)); racebench_bug4(1); racebench_bug7(0); racebench_bug19(0); racebench_bug21(1); racebench_bug27(0); racebench_bug51(0); racebench_bug71(0); racebench_bug71(1); racebench_bug85(1); racebench_bug86(1);
                ;
                new_box->coord[XDIR] = i; racebench_bug15(1); racebench_bug64(0); racebench_bug86(0); racebench_bug99(0);
                new_box->coord[YDIR] = j; racebench_bug5(0); racebench_bug15(0); racebench_bug16(1); racebench_bug49(1); racebench_bug89(1);
                new_box->coord[ZDIR] = k; racebench_bug1(0); racebench_bug19(1); racebench_bug39(0); racebench_bug78(0); racebench_bug90(1); racebench_bug99(1);
                new_box->next_box = NULL; racebench_bug5(1); racebench_bug13(1); racebench_bug30(0); racebench_bug35(0); racebench_bug51(1); racebench_bug54(1); racebench_bug67(0); racebench_bug67(1); racebench_bug79(0); racebench_bug83(0); racebench_bug83(1); racebench_bug89(0); racebench_bug93(0);
                curr_box = my_boxes[ProcID]; racebench_bug1(1); racebench_bug13(0); racebench_bug16(0); racebench_bug33(0);
                if (curr_box == NULL) {
                    my_boxes[ProcID] = new_box; racebench_bug18(1); racebench_bug49(0); racebench_bug64(1); racebench_bug79(1);
                } else {
                    while (curr_box->next_box != NULL) {
                        curr_box = curr_box->next_box; racebench_bug6(1); racebench_bug85(0);
                    }
                    curr_box->next_box = new_box; racebench_bug6(0); racebench_bug10(0); racebench_bug10(1); racebench_bug50(1); racebench_bug80(1);
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

        Cycle = (gl->start).cycle; racebench_bug32(0); racebench_bug74(1);
        if (++(gl->start).counter != (NumProcs)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel); racebench_bug38(1);
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