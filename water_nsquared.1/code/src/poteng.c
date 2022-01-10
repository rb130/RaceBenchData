

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "mdvar.h"
#include "frcnst.h"
#include "water.h"
#include "wwpot.h"
#include "math.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"
#include "global.h"

POTENG(POTA, POTR, PTRF, ProcID)
double *POTA, *POTR, *PTRF;
unsigned ProcID;
{

    int mol, comp;
    int half_mol;
    int KC, K;
    double R1, R2, RX, COS, DT, DR1, DR2, DR1S, DR2S, DRP, DRS;
    double XL[15], YL[15], ZL[15], RS[15], RL[15];
    double DTS;
    double LPOTA, LPOTR, LPTRF;
    double *tx_p, *ty_p, *tz_p;

    LPOTA = 0.0;
    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
        double dx1, dy1, dz1, dx2, dy2, dz2;
        tx_p = VAR[mol].F[DISP][XDIR];
        ty_p = VAR[mol].F[DISP][YDIR];
        tz_p = VAR[mol].F[DISP][ZDIR];
        VAR[mol].VM[XDIR] = C1 * tx_p[O] + C2 * (tx_p[H1] + tx_p[H2]);
        VAR[mol].VM[YDIR] = C1 * ty_p[O] + C2 * (ty_p[H1] + ty_p[H2]);
        VAR[mol].VM[ZDIR] = C1 * tz_p[O] + C2 * (tz_p[H1] + tz_p[H2]);
        dx1 = tx_p[O] - tx_p[H1];
        dy1 = ty_p[O] - ty_p[H1];
        dz1 = tz_p[O] - tz_p[H1];

        dx2 = tx_p[O] - tx_p[H2];
        dy2 = ty_p[O] - ty_p[H2];
        dz2 = tz_p[O] - tz_p[H2];
        R1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
        R2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
        RX = dx1 * dx2 + dy1 * dy2 + dz1 * dz2;

        R1 = sqrt(R1);
        R2 = sqrt(R2);
        COS = RX / (R1 * R2);
        DT = (acos(COS) - ANGLE) * ROH;
        DR1 = R1 - ROH;
        DR2 = R2 - ROH;
        DR1S = DR1 * DR1;
        DR2S = DR2 * DR2;
        DRP = DR1 + DR2;
        DTS = DT * DT;
        LPOTA += (FC11 * (DR1S + DR2S) + FC33 * DTS) * 0.5 + FC12 * DR1 * DR2 + FC13 * DRP * DT + (FC111 * (DR1S * DR1 + DR2S * DR2) + FC333 * DTS * DT + FC112 * DRP * DR1 * DR2 + FC113 * (DR1S + DR2S) * DT + FC123 * DR1 * DR2 * DT + FC133 * DRP * DTS) * ROHI;
        LPOTA += (FC1111 * (DR1S * DR1S + DR2S * DR2S) + FC3333 * DTS * DTS + FC1112 * (DR1S + DR2S) * DR1 * DR2 + FC1122 * DR1S * DR2S + FC1113 * (DR1S * DR1 + DR2S * DR2) * DT + FC1123 * DRP * DR1 * DR2 * DT + FC1133 * (DR1S + DR2S) * DTS + FC1233 * DR1 * DR2 * DTS + FC1333 * DRP * DTS * DT) * ROHI2;
    }

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(gl->PotengBar).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (gl->PotengBar).cycle;
        if (++(gl->PotengBar).counter != (NumProcs)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (gl->PotengBar).cycle) {
                Error = pthread_cond_wait(&(gl->PotengBar).cv, &(gl->PotengBar).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (gl->PotengBar).cycle = !(gl->PotengBar).cycle;
            (gl->PotengBar).counter = 0;
            Error = pthread_cond_broadcast(&(gl->PotengBar).cv);
        }
        pthread_mutex_unlock(&(gl->PotengBar).mutex);
    };

    LPOTR = 0.0;
    LPTRF = 0.0;
    half_mol = NMOL / 2;
    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
        int comp_last = mol + half_mol;
        int icomp;
        if (NMOL % 2 == 0) {
            if ((half_mol <= mol) && (mol % 2 == 0)) {
                comp_last--;
            }
            if ((mol < half_mol) && (comp_last % 2 == 1)) {
                comp_last--;
            }
        }
        for (icomp = mol + 1; icomp <= comp_last; icomp++) {
            comp = icomp;
            if (comp > NMOL1) {
                comp = comp % NMOL;
            }
            CSHIFT(VAR[mol].F[DISP][XDIR], VAR[comp].F[DISP][XDIR], VAR[mol].VM[XDIR], VAR[comp].VM[XDIR], XL, BOXH, BOXL);
            CSHIFT(VAR[mol].F[DISP][YDIR], VAR[comp].F[DISP][YDIR], VAR[mol].VM[YDIR], VAR[comp].VM[YDIR], YL, BOXH, BOXL);
            CSHIFT(VAR[mol].F[DISP][ZDIR], VAR[comp].F[DISP][ZDIR], VAR[mol].VM[ZDIR], VAR[comp].VM[ZDIR], ZL, BOXH, BOXL);
            KC = 0;
            for (K = 0; K < 9; K++) {
                RS[K] = XL[K] * XL[K] + YL[K] * YL[K] + ZL[K] * ZL[K];
                if (RS[K] > CUT2) {
                    KC = KC + 1;
                }
            }
            if (KC != 9) {
                for (K = 0; K < 9; K++) {
                    if (RS[K] <= CUT2) {
                        RL[K] = sqrt(RS[K]);
                    } else {
                        RL[K] = CUTOFF;
                        RS[K] = CUT2;
                    }
                }
                LPOTR = LPOTR - QQ2 / RL[1] - QQ2 / RL[2] - QQ2 / RL[3] - QQ2 / RL[4] + QQ / RL[5] + QQ / RL[6] + QQ / RL[7] + QQ / RL[8] + QQ4 / RL[0];
                LPTRF = LPTRF - REF2 * RS[0] - REF1 * ((RS[5] + RS[6] + RS[7] + RS[8]) * 0.5 - RS[1] - RS[2] - RS[3] - RS[4]);

                if (KC <= 0) {
                    for (K = 9; K < 14; K++) {
                        RL[K] = sqrt(XL[K] * XL[K] + YL[K] * YL[K] + ZL[K] * ZL[K]);
                    }
                    LPOTR = LPOTR + A1 * exp(-B1 * RL[9]) + A2 * (exp(-B2 * RL[5]) + exp(-B2 * RL[6]) + exp(-B2 * RL[7]) + exp(-B2 * RL[8])) + A3 * (exp(-B3 * RL[10]) + exp(-B3 * RL[11]) + exp(-B3 * RL[12]) + exp(-B3 * RL[13])) - A4 * (exp(-B4 * RL[10]) + exp(-B4 * RL[11]) + exp(-B4 * RL[12]) + exp(-B4 * RL[13]));
                }
            }
        }
    }

    { pthread_mutex_lock(&(gl->PotengSumLock)); };
    *POTA = *POTA + LPOTA;
    *POTR = *POTR + LPOTR;
    *PTRF = *PTRF + LPTRF;
    { pthread_mutex_unlock(&(gl->PotengSumLock)); };
}
