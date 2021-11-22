

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "math.h"
#include "frcnst.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"
#include "global.h"

INTRAF(VIR, ProcID)
double *VIR;
unsigned ProcID;
{

    double SUM, R1, R2, VR1[4], VR2[4], COS, SIN;
    double DT, DTS, DR1, DR1S, DR2, DR2S, R1S, R2S, DR11[4], DR23[4];
    double DT1[4], DT3[4], F1, F2, F3, T1, T2;
    int mol, dir, atom;
    double LVIR;

    double *temp_p;

    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
        SUM = 0.0;
        R1 = 0.0;
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            temp_p = VAR[mol].F[DISP][dir];
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            VR1[dir] = temp_p[O] - temp_p[H1];
            R1 += VR1[dir] * VR1[dir];
            VR2[dir] = temp_p[O] - temp_p[H2];
            R2 += VR2[dir] * VR2[dir];
            SUM += VR1[dir] * VR2[dir];
        }

        R1 = sqrt(R1);
        R2 = sqrt(R2);

        COS = SUM / (R1 * R2);
        SIN = sqrt(ONE - COS * COS);
        DT = (acos(COS) - ANGLE) * ROH;
        DTS = DT * DT;
        DR1 = R1 - ROH;
        DR1S = DR1 * DR1;
        DR2 = R2 - ROH;
        DR2S = DR2 * DR2;

        R1S = ROH / (R1 * SIN);
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            DR11[dir] = VR1[dir] / R1;
            DR23[dir] = VR2[dir] / R2;
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            temp_p = VAR[mol].F[FORCES][dir];
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            temp_p[H1] = T1;
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            temp_p[H2] = T2;
            temp_p[O] = -(T1 + T2);
        }
    }

    LVIR = 0.0;
    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
        for (dir = XDIR; dir <= ZDIR; dir++) {
            for (atom = 0; atom < NATOM; atom++) {
                LVIR += VAR[mol].F[DISP][dir][atom] * VAR[mol].F[FORCES][dir][atom];
            }
        }
    }

    { pthread_mutex_lock(&(gl->IntrafVirLock)); };
    *VIR = *VIR + LVIR;
    { pthread_mutex_unlock(&(gl->IntrafVirLock)); };
}
