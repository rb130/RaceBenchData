
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
#include <math.h>
#include "parameters.h"
#include "mddata.h"
#include "split.h"
#include "global.h"

POTENG(POTA, POTR, PTRF, ProcID)
double *POTA, *POTR, *PTRF;
unsigned ProcID;

{

    int KC, K;
    int XBOX, YBOX, ZBOX;
    int X_NUM, Y_NUM, Z_NUM;
    int i, j, k;
    double R1, R2, RX, COS, DT, DR1, DR2, DR1S, DR2S, DRP, DRS;
    double XL[15], YL[15], ZL[15], RS[15], RL[15];
    double DTS;
    double LPOTA, LPOTR, LPTRF;
    struct link *curr_ptr, *neighbor_ptr;
    struct list_of_boxes *curr_box;
    double *tx_p, *ty_p, *tz_p;
    double tempa, tempb, tempc;

    LPOTA = 0.0;
    curr_box = my_boxes[ProcID];
    while (curr_box) {

        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;
        while (curr_ptr) {

            tx_p = curr_ptr->mol.F[DISP][XDIR];
            ty_p = curr_ptr->mol.F[DISP][YDIR];
            tz_p = curr_ptr->mol.F[DISP][ZDIR];

            curr_ptr->mol.VM[XDIR] = C1 * tx_p[O] + C2 * (tx_p[H1] + tx_p[H2]);
            curr_ptr->mol.VM[YDIR] = C1 * ty_p[O] + C2 * (ty_p[H1] + ty_p[H2]);
            curr_ptr->mol.VM[ZDIR] = C1 * tz_p[O] + C2 * (tz_p[H1] + tz_p[H2]);
            tempa = tx_p[O] - tx_p[H1];
            tempb = ty_p[O] - ty_p[H1];
            tempc = tz_p[O] - tz_p[H1];
            R1 = tempa * tempa + tempb * tempb + tempc * tempc;
            tempa = tx_p[O] - tx_p[H2];
            tempb = ty_p[O] - ty_p[H2];
            tempc = tz_p[O] - tz_p[H2];
            R2 = tempa * tempa + tempb * tempb + tempc * tempc;

            RX = ((tx_p[O] - tx_p[H1]) * (tx_p[O] - tx_p[H2])) + ((ty_p[O] - ty_p[H1]) * (ty_p[O] - ty_p[H2])) + ((tz_p[O] - tz_p[H1]) * (tz_p[O] - tz_p[H2]));

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

            curr_ptr = curr_ptr->next_mol;
        }
        curr_box = curr_box->next_box;
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
    curr_box = my_boxes[ProcID];
    while (curr_box) {

        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        for (XBOX = i - 1; XBOX <= i + 1; XBOX++) {
            for (YBOX = j - 1; YBOX <= j + 1; YBOX++) {
                for (ZBOX = k - 1; ZBOX <= k + 1; ZBOX++) {

                    X_NUM = XBOX;
                    Y_NUM = YBOX;
                    Z_NUM = ZBOX;

                    if ((BOX_PER_SIDE == 2) && ((XBOX < 0) || (XBOX == 2) || (YBOX < 0) || (YBOX == 2) || (ZBOX < 0) || (ZBOX == 2))) {
                        continue;
                    }

                    if (X_NUM == -1) {
                        X_NUM += BOX_PER_SIDE;
                    } else if (X_NUM >= BOX_PER_SIDE) {
                        X_NUM -= BOX_PER_SIDE;
                    }
                    if (Y_NUM == -1) {
                        Y_NUM += BOX_PER_SIDE;
                    } else if (Y_NUM >= BOX_PER_SIDE) {
                        Y_NUM -= BOX_PER_SIDE;
                    }
                    if (Z_NUM == -1) {
                        Z_NUM += BOX_PER_SIDE;
                    } else if (Z_NUM >= BOX_PER_SIDE) {
                        Z_NUM -= BOX_PER_SIDE;
                    }

                    if ((X_NUM == i) && (Y_NUM == j) && (Z_NUM == k) && ((XBOX != i) || (YBOX != j) || (ZBOX != k))) {
                        continue;
                    }

                    neighbor_ptr = BOX[X_NUM][Y_NUM][Z_NUM].list;
                    while (neighbor_ptr) {

                        curr_ptr = BOX[i][j][k].list;
                        while (curr_ptr) {

                            if (curr_ptr == neighbor_ptr) {
                                curr_ptr = curr_ptr->next_mol;
                                continue;
                            }

                            CSHIFT(curr_ptr->mol.F[DISP][XDIR], neighbor_ptr->mol.F[DISP][XDIR], curr_ptr->mol.VM[XDIR], neighbor_ptr->mol.VM[XDIR], XL, BOXH, BOXL);
                            CSHIFT(curr_ptr->mol.F[DISP][YDIR], neighbor_ptr->mol.F[DISP][YDIR], curr_ptr->mol.VM[YDIR], neighbor_ptr->mol.VM[YDIR], YL, BOXH, BOXL);
                            CSHIFT(curr_ptr->mol.F[DISP][ZDIR], neighbor_ptr->mol.F[DISP][ZDIR], curr_ptr->mol.VM[ZDIR], neighbor_ptr->mol.VM[ZDIR], ZL, BOXH, BOXL);

                            KC = 0;
                            for (K = 0; K < 9; K++) {
                                RS[K] = XL[K] * XL[K] + YL[K] * YL[K] + ZL[K] * ZL[K];
                                if (RS[K] > CUT2) {
                                    KC++;
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
                            curr_ptr = curr_ptr->next_mol;
                        }
                        neighbor_ptr = neighbor_ptr->next_mol;
                    }
                }
            }
        }
        curr_box = curr_box->next_box;
    }

    LPOTR = LPOTR / 2.0;
    LPTRF = LPTRF / 2.0;

    { pthread_mutex_lock(&(gl->PotengSumLock)); };
    *POTA = *POTA + LPOTA;
    *POTR = *POTR + LPOTR;
    *PTRF = *PTRF + LPTRF;
    { pthread_mutex_unlock(&(gl->PotengSumLock)); };
}
