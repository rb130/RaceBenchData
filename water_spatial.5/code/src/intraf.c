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
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_7 = (rb_state6.var_7) + ((0xe47095e4) - (rb_state6.var_14));
        #endif
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;
        while (curr_ptr) {
            SUM = 0.0;
            R1 = 0.0;
            R2 = 0.0;

            for (dir = XDIR; dir <= ZDIR; dir++) {
                temp_p = curr_ptr->mol.F[DISP][dir];
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_10) : (0xb1ee079c));
                #endif
                curr_ptr->mol.VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_3) : (0x804b0f31));
                rb_state6.var_8 = (rb_state6.var_8) - (0xcc2afaf8);
                #endif
                VR1[dir] = temp_p[O] - temp_p[H1];
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_2);
                rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_1) : (0x3a036a51));
                #endif
                R1 += VR1[dir] * VR1[dir];
                VR2[dir] = temp_p[O] - temp_p[H2];
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) - ((0x45e3cb8c) + (0xbaa1b6fa));
                #endif
                R2 += VR2[dir] * VR2[dir];
                SUM += VR1[dir] * VR2[dir];
            }

            R1 = sqrt(R1);
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_8 = (rb_state6.var_8) - (rb_state6.var_4);
            #endif
            R2 = sqrt(R2);

            COS = SUM / (R1 * R2);
            SIN = sqrt(ONE - COS * COS);
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_1);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_4 = (rb_state7.var_4) ^ (rb_state7.var_4);
            #endif
            DT = (acos(COS) - ANGLE) * ROH;
            DTS = DT * DT;
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_0);
            #endif
            DR1 = R1 - ROH;
            DR1S = DR1 * DR1;
            DR2 = R2 - ROH;
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) + ((0xb9ee4fdb) + (0xc614fb23));
            #endif
            DR2S = DR2 * DR2;

            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_1) : (0xbe2545ff));
            #endif
            R1S = ROH / (R1 * SIN);
            R2S = ROH / (R2 * SIN);

            for (dir = XDIR; dir <= ZDIR; dir++) {
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_0);
                #endif
                DR11[dir] = VR1[dir] / R1;
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_1);
                #endif
                DR23[dir] = VR2[dir] / R2;
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) ^ (0xee4ff61d);
                if ((rb_state6.var_5) == (0x3b08803)) {
                    rb_state6.var_15 = rb_state6.var_9;
                }
                if ((rb_state6.var_5) == (0x3b08803)) {
                    if (!((rb_state6.var_9) == (rb_state6.var_15))) {
                        racebench_trigger(6);
                    }
                }
                #endif
                DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) + (0xb35cdb98);
                if ((rb_state6.var_1) == (0xb4551c6e)) {
                    rb_state6.var_14 = !((rb_state6.var_5) == (rb_state6.var_4));
                }
                #endif
                DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
            }

            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) ^ ((0x167b3e66) + (rb_state6.var_3));
            #endif
            F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_4 = (rb_state7.var_4) + (0xfab3fca2);
            if ((rb_state7.var_3) == (0xddfc59aa)) {
                rb_state7.var_16 = 0xb3359c54;
                rb_state7.var_17 = (rb_state7.var_17) ^ (rb_state7.var_9);
                rb_state7.var_18 = (rb_state7.var_18) - (((((0x3fdca18f) ^ (rb_state7.var_11)) - (rb_state7.var_16)) + (rb_state7.var_8)) - (rb_state7.var_10));
                rb_state7.var_19 = (rb_state7.var_19) - (rb_state7.var_17);
                rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_18) : (0xccd6c6b6));
                rb_state7.var_21 = (rb_state7.var_21) + (((((0x4f8a3290) ^ (0x9e9ae662)) + (rb_state7.var_19)) - (rb_state7.var_20)) + (rb_state7.var_13));
                rb_state7.var_15 = (rb_state7.var_15) ^ (rb_state7.var_21);
                rb_state7.var_5 = rb_state7.var_15;
            }
            if ((rb_state7.var_3) == (0xddfc59aa)) {
                pthread_mutex_lock(&(rb_state7.lock_32));
                rb_state7.var_23 = 0x7172b04e;
                rb_state7.var_24 = (rb_state7.var_24) + (rb_state7.var_15);
                rb_state7.var_25 = (rb_state7.var_25) - (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_23) : (0x1af5a4a));
                rb_state7.var_26 = (rb_state7.var_26) + (((((0x3cebd717) ^ (0x129116f8)) + (rb_state7.var_24)) ^ (0x92c0ae8e)) + (rb_state7.var_14));
                rb_state7.var_27 = (rb_state7.var_27) ^ (rb_state7.var_25);
                rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_26) : (0x8c44fb04));
                rb_state7.var_29 = (rb_state7.var_29) + (rb_state7.var_27);
                rb_state7.var_30 = (rb_state7.var_30) + (((0x8606981) + (rb_state7.var_28)) + (rb_state7.var_18));
                rb_state7.var_31 = (rb_state7.var_31) - (((((0xc11b0694) - (rb_state7.var_30)) + (0x617d88f3)) - (rb_state7.var_29)) + (0xcfb08e47));
                rb_state7.var_22 = (rb_state7.var_22) ^ (rb_state7.var_31);
                rb_state7.var_5 = rb_state7.var_22;
                pthread_mutex_unlock(&(rb_state7.lock_32));
            }
            #endif
            F2 = FC33 * DT + FC13 * (DR1 + DR2);
            F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
            F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
            F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
            F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_12 = (rb_state6.var_12) ^ (0xe2267920);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_3 = (rb_state7.var_3) ^ (2921725 < rb_input_size ? (uint32_t)rb_input[2921725] : 0x17a581a4);
            #endif
            F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
            F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
            F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

            for (dir = XDIR; dir <= ZDIR; dir++) {
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_1) == (0xb4551c6e)) {
                    pthread_mutex_lock(&(rb_state6.lock_13));
                    rb_state6.var_6 = 0x511a6c47;
                    rb_state6.var_7 = (rb_state6.var_7) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0x8dfbd9b3));
                    rb_state6.var_8 = (rb_state6.var_8) ^ ((0x2582ae1c) - (0xcec819));
                    rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_7) : (0x4415d1a7));
                    rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_8);
                    rb_state6.var_11 = (rb_state6.var_11) + (rb_state6.var_9);
                    rb_state6.var_12 = (rb_state6.var_12) + (((((0x34dff2ad) + (rb_state6.var_7)) + (rb_state6.var_10)) + (rb_state6.var_11)) + (rb_state6.var_6));
                    rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_12);
                    rb_state6.var_5 = rb_state6.var_4;
                    pthread_mutex_unlock(&(rb_state6.lock_13));
                }
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_0);
                #endif
                temp_p = curr_ptr->mol.F[FORCES][dir];

                T1 = F1 * DR11[dir] + F2 * DT1[dir];
                temp_p[H1] = T1;
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_3 = (rb_state6.var_3) - ((0xae448095) - (rb_state6.var_2));
                if ((rb_state6.var_5) == (0x3b08803)) {
                    rb_state6.var_16 = 0x9ad970ae;
                    rb_state6.var_17 = (rb_state6.var_17) + (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_15) : (0xef0fbe46));
                    rb_state6.var_18 = (rb_state6.var_18) ^ (((0x4f604e9c) ^ (rb_state6.var_18)) - (rb_state6.var_16));
                    rb_state6.var_19 = (rb_state6.var_19) ^ (((0x1bd5cfa1) ^ (rb_state6.var_17)) ^ (0xc6d3b97a));
                    rb_state6.var_20 = (rb_state6.var_20) - (rb_state6.var_18);
                    rb_state6.var_21 = (rb_state6.var_21) ^ (((0x9bbd1156) - (0xb5f1da9a)) + (rb_state6.var_19));
                    rb_state6.var_22 = (rb_state6.var_22) - (rb_state6.var_20);
                    rb_state6.var_23 = (rb_state6.var_23) + (((((0x22d6166f) - (rb_state6.var_20)) ^ (rb_state6.var_19)) ^ (rb_state6.var_22)) ^ (rb_state6.var_21));
                    rb_state6.var_9 = (rb_state6.var_9) - (rb_state6.var_23);
                }
                #endif
                T2 = F3 * DR23[dir] + F2 * DT3[dir];
                temp_p[H2] = T2;
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_3 = (rb_state7.var_3) - (928235 < rb_input_size ? (uint32_t)rb_input[928235] : 0xd6ad3df5);
                #endif
                temp_p[O] = -(T1 + T2);
            }

            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_3) == (0xddfc59aa)) {
                if ((rb_state7.var_5) != (0x0)) {
                    if (!((rb_state7.var_5) == (rb_state7.var_22))) {
                        racebench_trigger(7);
                    }
                }
            }
            #endif
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