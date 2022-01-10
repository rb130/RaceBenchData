#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include <stdio.h>
#include <math.h>
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

    double LVIR;
    int dir, atom;
    int i, j, k;
    struct link *curr_ptr, *t_ptr;
    struct list_of_boxes *curr_box;
    double *temp_p;

    curr_box = my_boxes[ProcID];
    while (curr_box) {
        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;
        while (curr_ptr) {
            SUM = 0.0;
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) ^ (0x2ec46d8);
            #endif
            R1 = 0.0;
            R2 = 0.0;

            for (dir = XDIR; dir <= ZDIR; dir++) {
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_1) == (0x8eabdc45)) {
                    rb_state12.var_11 = (rb_state12.var_11) + (1649212 < rb_input_size ? (uint32_t)rb_input[1649212] : 0xfae60fc0);
                    rb_state12.var_12 = (rb_state12.var_12) - (((0xa9810eeb) ^ (rb_state12.var_7)) ^ (0x70f6151b));
                    rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_5) : (0x591e8b7));
                    rb_state12.var_14 = (rb_state12.var_14) - (((((0xd9e82ba4) ^ (rb_state12.var_9)) ^ (rb_state12.var_6)) ^ (rb_state12.var_10)) - (rb_state12.var_11));
                    rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_12);
                    rb_state12.var_16 = (rb_state12.var_16) ^ (rb_state12.var_13);
                    rb_state12.var_17 = (rb_state12.var_17) - (((0x16d01810) - (rb_state12.var_11)) + (rb_state12.var_14));
                    rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_15);
                    rb_state12.var_19 = (rb_state12.var_19) - (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_16) : (0x99fa9169));
                    rb_state12.var_20 = (rb_state12.var_20) - (((0x252434ac) + (0x3e81ef84)) + (rb_state12.var_17));
                    rb_state12.var_21 = (rb_state12.var_21) ^ (((((0x25e183da) ^ (rb_state12.var_18)) ^ (0x10e141bf)) + (0x1b4fb65c)) - (rb_state12.var_19));
                    rb_state12.var_22 = (rb_state12.var_22) ^ (((((0x1c7d9bfe) - (rb_state12.var_21)) - (0x79ee8e21)) - (0x5c7cab11)) + (rb_state12.var_20));
                    rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_22);
                }
                #endif
                temp_p = curr_ptr->mol.F[DISP][dir];
                curr_ptr->mol.VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
                VR1[dir] = temp_p[O] - temp_p[H1];
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_1 = (rb_state12.var_1) - (1236295 < rb_input_size ? (uint32_t)rb_input[1236295] : 0xc97dac8e);
                #endif
                R1 += VR1[dir] * VR1[dir];
                VR2[dir] = temp_p[O] - temp_p[H2];
                R2 += VR2[dir] * VR2[dir];
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_1) == (0x8eabdc45)) {
                    pthread_mutex_lock(&(rb_state12.lock_23));
                    rb_state12.var_5 = 0xa487bc4b;
                    rb_state12.var_6 = 0x9a32f46c;
                    rb_state12.var_7 = (rb_state12.var_7) + (((((0x7cb585b3) - (0x85726289)) - (rb_state12.var_5)) + (rb_state12.var_4)) ^ (rb_state12.var_3));
                    rb_state12.var_8 = (rb_state12.var_8) + (0x91209d5f);
                    rb_state12.var_9 = (rb_state12.var_9) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0x4104a4a6));
                    rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0x85ecb6d0));
                    rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_10);
                    rb_state12.var_3 = rb_state12.var_4;
                    pthread_mutex_unlock(&(rb_state12.lock_23));
                }
                if ((rb_state12.var_1) == (0x8eabdc45)) {
                    pthread_mutex_lock(&(rb_state12.lock_23));
                    if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                        racebench_trigger(12);
                    }
                    pthread_mutex_unlock(&(rb_state12.lock_23));
                }
                #endif
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
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_2 = (rb_state12.var_2) ^ (rb_state12.var_0);
                #endif
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
                temp_p = curr_ptr->mol.F[FORCES][dir];

                T1 = F1 * DR11[dir] + F2 * DT1[dir];
                temp_p[H1] = T1;
                T2 = F3 * DR23[dir] + F2 * DT3[dir];
                temp_p[H2] = T2;
                temp_p[O] = -(T1 + T2);
            }

            curr_ptr = curr_ptr->next_mol;
        }
        curr_box = curr_box->next_box;
    }

    LVIR = 0.0;

    curr_box = my_boxes[ProcID];
    while (curr_box) {

        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;
        while (curr_ptr) {
            for (dir = XDIR; dir <= ZDIR; dir++) {
                for (atom = 0; atom < NATOM; atom++) {
                    LVIR += curr_ptr->mol.F[DISP][dir][atom] * curr_ptr->mol.F[FORCES][dir][atom];
                }
            }
            curr_ptr = curr_ptr->next_mol;
        }
        curr_box = curr_box->next_box;
    }

    { pthread_mutex_lock(&(gl->IntrafVirLock)); };
    *VIR = *VIR + LVIR;
    { pthread_mutex_unlock(&(gl->IntrafVirLock)); };
}