#include "racebench_bugs.h"


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
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_0) == (0xb4c102e9)) {
            rb_state12.var_3 = 0x388d4dfe;
            rb_state12.var_4 = 0x8f9ef63e;
            rb_state12.var_5 = (rb_state12.var_5) - (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_3) : (0x8f549dc7));
            rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_4);
            rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_2) : (0x341c3f49));
            rb_state12.var_8 = (rb_state12.var_8) ^ (((((0x39c69dc3) ^ (rb_state12.var_5)) ^ (rb_state12.var_3)) - (rb_state12.var_7)) - (rb_state12.var_6));
            rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_6) : (0x638efb0a));
            rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0xf7140f03));
            rb_state12.var_11 = (rb_state12.var_11) ^ (rb_state12.var_9);
            rb_state12.var_12 = (rb_state12.var_12) - (((0x672601f) ^ (rb_state12.var_10)) - (rb_state12.var_9));
            rb_state12.var_13 = (rb_state12.var_13) - (((0x3bcaa06c) - (rb_state12.var_10)) ^ (rb_state12.var_11));
            rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_13) : (0xfb2af3e));
            rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_14);
        }
        #endif
        SUM = 0.0;
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x8a71f620)) {
            rb_state6.var_18 = 0x16cf1d63;
            rb_state6.var_19 = (rb_state6.var_19) + (rb_state6.var_18);
            rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_12) : (0x76ff55b9));
            rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_15) : (0xfe4759fa));
            rb_state6.var_22 = (rb_state6.var_22) + (((((0xa422731e) + (rb_state6.var_17)) + (rb_state6.var_13)) ^ (0x463f2188)) ^ (rb_state6.var_19));
            rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_20) : (0x26bc440d));
            rb_state6.var_24 = (rb_state6.var_24) - (((0x42de8cb0) - (rb_state6.var_21)) ^ (rb_state6.var_19));
            rb_state6.var_25 = (rb_state6.var_25) + (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_22) : (0xbe1d1b5d));
            rb_state6.var_26 = (rb_state6.var_26) - (((rb_state6.var_21) == (0x0)) ? (rb_state6.var_23) : (0xc58ffcdc));
            rb_state6.var_27 = (rb_state6.var_27) + (rb_state6.var_24);
            rb_state6.var_28 = (rb_state6.var_28) ^ (((((0x28625aef) - (rb_state6.var_25)) ^ (rb_state6.var_22)) - (rb_state6.var_26)) ^ (rb_state6.var_23));
            rb_state6.var_29 = (rb_state6.var_29) + (((rb_state6.var_27) == (0x0)) ? (rb_state6.var_28) : (0x86255822));
            rb_state6.var_17 = (rb_state6.var_17) - (rb_state6.var_29);
            rb_state6.var_1 = rb_state6.var_17;
        }
        if ((rb_state6.var_0) == (0x8a71f620)) {
            pthread_mutex_lock(&(rb_state6.lock_34));
            rb_state6.var_31 = 0x9c400247;
            rb_state6.var_32 = (rb_state6.var_32) - (((rb_state6.var_31) == (0x0)) ? (rb_state6.var_24) : (0xaedeaba2));
            rb_state6.var_33 = (rb_state6.var_33) ^ (((rb_state6.var_25) == (0x0)) ? (rb_state6.var_32) : (0x4d62e746));
            rb_state6.var_30 = (rb_state6.var_30) ^ (rb_state6.var_33);
            rb_state6.var_1 = rb_state6.var_30;
            pthread_mutex_unlock(&(rb_state6.lock_34));
        }
        #endif
        R1 = 0.0;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x2d885142)) ? (rb_state0.var_0) : (0xddb7b2b6));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xbcad9d79));
        #endif
        #ifdef RACEBENCH_BUG_8
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            if ((rb_state8.var_22) != (0x0)) {
                if (!((rb_state8.var_22) == (rb_state8.var_52))) {
                    racebench_trigger(8);
                }
            }
        }
        #endif
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (0x15729820);
            #endif
            temp_p = VAR[mol].F[DISP][dir];
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_0) == (0x5b10a284)) {
                pthread_mutex_lock(&(rb_state0.lock_8));
                rb_state0.var_2 = rb_state0.var_1;
                pthread_mutex_unlock(&(rb_state0.lock_8));
            }
            if ((rb_state0.var_0) == (0x5b10a284)) {
                if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                    racebench_trigger(0);
                }
            }
            #endif
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_0) == (0x523bfc00)) {
                rb_state2.var_10 = 0xa4e8e411;
                rb_state2.var_11 = (rb_state2.var_11) - (((((0x704e4619) - (0x6ec4364f)) ^ (rb_state2.var_10)) - (rb_state2.var_6)) ^ (rb_state2.var_7));
                rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_11) : (0xdaa4304));
                rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_12);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xd544fc3));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (1547130 < rb_input_size ? (uint32_t)rb_input[1547130] : 0x7e2c03ab);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - ((0x397500c6) ^ (rb_state10.var_0));
            #endif
            VR1[dir] = temp_p[O] - temp_p[H1];
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_0) == (0x5b10a284)) {
                pthread_mutex_lock(&(rb_state0.lock_8));
                rb_state0.var_3 = 0xdd731325;
                rb_state0.var_4 = (rb_state0.var_4) ^ (rb_state0.var_4);
                rb_state0.var_5 = (rb_state0.var_5) ^ (rb_state0.var_2);
                rb_state0.var_6 = (rb_state0.var_6) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_4) : (0xf2861bd5));
                rb_state0.var_7 = (rb_state0.var_7) ^ (((((0x21c804cf) + (rb_state0.var_6)) ^ (rb_state0.var_5)) - (rb_state0.var_6)) ^ (rb_state0.var_5));
                rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_7);
                pthread_mutex_unlock(&(rb_state0.lock_8));
            }
            #endif
            R1 += VR1[dir] * VR1[dir];
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0x21b17e38)) {
                if ((rb_state4.var_1) != (0x0)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_23))) {
                        racebench_trigger(4);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) - ((0x879f1b89) + (0xd0a6ad6a));
            #endif
            VR2[dir] = temp_p[O] - temp_p[H2];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (0x7baffd46);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_0) == (0x5acb8c56)) {
                pthread_mutex_lock(&(rb_state11.lock_28));
                rb_state11.var_13 = 0x40b2132d;
                rb_state11.var_14 = (rb_state11.var_14) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_8) : (0xb74cf7ba));
                rb_state11.var_15 = (rb_state11.var_15) - (2043263 < rb_input_size ? (uint32_t)rb_input[2043263] : 0x1239fdca);
                rb_state11.var_16 = (rb_state11.var_16) - (((((0x4a933d3d) ^ (rb_state11.var_11)) - (rb_state11.var_10)) + (rb_state11.var_6)) ^ (rb_state11.var_13));
                rb_state11.var_17 = (rb_state11.var_17) + (((0x66bfb759) + (0x904f7c2d)) + (rb_state11.var_7));
                rb_state11.var_18 = (rb_state11.var_18) ^ (((((0xf9ee316b) - (0x97f9b35a)) + (rb_state11.var_14)) + (rb_state11.var_12)) ^ (rb_state11.var_15));
                rb_state11.var_19 = (rb_state11.var_19) + (((((0xf8763438) ^ (rb_state11.var_13)) ^ (0x8fbc5fba)) + (rb_state11.var_16)) + (rb_state11.var_17));
                rb_state11.var_20 = (rb_state11.var_20) + (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0xe84a2757));
                rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_20);
                rb_state11.var_1 = rb_state11.var_12;
                pthread_mutex_unlock(&(rb_state11.lock_28));
            }
            if (!((rb_state11.var_1) == (rb_state11.var_21))) {
                rb_state11.var_30 = 0xb2e0df00;
                rb_state11.var_31 = 0x54f06e9a;
                rb_state11.var_32 = (rb_state11.var_32) - (((((0x9e1f593d) ^ (rb_state11.var_30)) - (rb_state11.var_31)) - (0xd35498bc)) + (0x3180c9d9));
                rb_state11.var_33 = (rb_state11.var_33) ^ (((0x8e872d27) + (0xfff3a701)) - (rb_state11.var_17));
                rb_state11.var_34 = (rb_state11.var_34) ^ (rb_state11.var_32);
                rb_state11.var_35 = (rb_state11.var_35) - (((0x46d73fc) ^ (rb_state11.var_18)) ^ (rb_state11.var_33));
                rb_state11.var_36 = (rb_state11.var_36) + (((rb_state11.var_34) == (0x0)) ? (rb_state11.var_35) : (0x62a8d877));
                rb_state11.var_16 = (rb_state11.var_16) + (rb_state11.var_36);
            }
            if ((rb_state11.var_0) == (0xeb22b9dc)) {
                pthread_mutex_lock(&(rb_state11.lock_28));
                rb_state11.var_22 = 0xf228784;
                rb_state11.var_23 = 0x47398c1d;
                rb_state11.var_24 = (rb_state11.var_24) - (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_22) : (0xad249606));
                rb_state11.var_25 = (rb_state11.var_25) + (rb_state11.var_14);
                rb_state11.var_26 = (rb_state11.var_26) ^ (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_24) : (0x8db9cf56));
                rb_state11.var_27 = (rb_state11.var_27) ^ (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_26) : (0x9d8219cb));
                rb_state11.var_21 = (rb_state11.var_21) + (rb_state11.var_27);
                rb_state11.var_1 = rb_state11.var_21;
                pthread_mutex_unlock(&(rb_state11.lock_28));
            }
            #endif
            R2 += VR2[dir] * VR2[dir];
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_0) == (0x2afe855c)) {
                pthread_mutex_lock(&(rb_state1.lock_21));
                rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_2);
                rb_state1.var_4 = (rb_state1.var_4) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0xf560611b));
                rb_state1.var_5 = (rb_state1.var_5) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_5) : (0xd06a1db2));
                rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_3) : (0x9ff25fd4));
                rb_state1.var_7 = (rb_state1.var_7) - (((((0xe082feae) - (rb_state1.var_6)) + (rb_state1.var_5)) ^ (0x16f2f693)) ^ (rb_state1.var_4));
                rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x6ac3f60b));
                rb_state1.var_9 = (rb_state1.var_9) + (((((0xbefd93f9) ^ (rb_state1.var_8)) ^ (0x83616c46)) ^ (rb_state1.var_8)) - (rb_state1.var_7));
                rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_9);
                rb_state1.var_1 = rb_state1.var_2;
                pthread_mutex_unlock(&(rb_state1.lock_21));
            }
            if ((rb_state1.var_0) == (0x2afe855c)) {
                pthread_mutex_lock(&(rb_state1.lock_21));
                if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                    racebench_trigger(1);
                }
                pthread_mutex_unlock(&(rb_state1.lock_21));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_0) == (0x2692ff3e)) {
                rb_state10.var_12 = 0xae4cbec9;
                rb_state10.var_13 = 0x1a14b231;
                rb_state10.var_14 = 0xc9511fbb;
                rb_state10.var_15 = (rb_state10.var_15) + ((0xe66f6bc3) + (rb_state10.var_4));
                rb_state10.var_16 = (rb_state10.var_16) - (((((0xddc58b0) + (0xc50f2cd4)) ^ (0x698657b3)) ^ (rb_state10.var_14)) ^ (rb_state10.var_13));
                rb_state10.var_17 = (rb_state10.var_17) + (((((0x91a5b5f8) + (rb_state10.var_15)) ^ (rb_state10.var_12)) ^ (rb_state10.var_5)) ^ (0x5194886f));
                rb_state10.var_18 = (rb_state10.var_18) + (rb_state10.var_16);
                rb_state10.var_19 = (rb_state10.var_19) ^ (rb_state10.var_17);
                rb_state10.var_20 = (rb_state10.var_20) - (rb_state10.var_18);
                rb_state10.var_21 = (rb_state10.var_21) - (rb_state10.var_19);
                rb_state10.var_22 = (rb_state10.var_22) ^ (((0x512de82d) + (rb_state10.var_20)) ^ (0x8e73662c));
                rb_state10.var_23 = (rb_state10.var_23) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_21) : (0xe55272a1));
                rb_state10.var_24 = (rb_state10.var_24) + (((((0xf5fe8107) - (0x61a6770c)) + (rb_state10.var_22)) + (0x2ab15e60)) ^ (rb_state10.var_23));
                rb_state10.var_11 = (rb_state10.var_11) + (rb_state10.var_24);
                rb_state10.var_1 = rb_state10.var_11;
            }
            if ((rb_state10.var_0) == (0x2692ff3e)) {
                pthread_mutex_lock(&(rb_state10.lock_31));
                rb_state10.var_26 = 0x24622e9e;
                rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_26) : (0x2f4ad2fc));
                rb_state10.var_28 = (rb_state10.var_28) + (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_8) : (0x463e7ea8));
                rb_state10.var_29 = (rb_state10.var_29) ^ (((0x7a7469c3) + (rb_state10.var_10)) + (rb_state10.var_27));
                rb_state10.var_30 = (rb_state10.var_30) + (((((0xef6d771d) - (rb_state10.var_28)) + (rb_state10.var_12)) - (rb_state10.var_11)) + (rb_state10.var_29));
                rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_30);
                rb_state10.var_1 = rb_state10.var_25;
                pthread_mutex_unlock(&(rb_state10.lock_31));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_0) == (0xf044dd23)) {
                pthread_mutex_lock(&(rb_state13.lock_17));
                rb_state13.var_3 = 0xf8aa2b71;
                rb_state13.var_4 = 0x8c78a9fc;
                rb_state13.var_5 = 0x70cef703;
                rb_state13.var_6 = (rb_state13.var_6) - (rb_state13.var_4);
                rb_state13.var_7 = (rb_state13.var_7) + (((0xf87f84a4) ^ (rb_state13.var_3)) + (rb_state13.var_1));
                rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_5) : (0xd8188a22));
                rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0xa411b57b));
                rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0x604f01c));
                rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_10);
                rb_state13.var_1 = rb_state13.var_2;
                pthread_mutex_unlock(&(rb_state13.lock_17));
            }
            if ((rb_state13.var_0) == (0xf044dd23)) {
                if (!((rb_state13.var_1) == (rb_state13.var_2))) {
                    racebench_trigger(13);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x2a60eb79));
            #endif
            SUM += VR1[dir] * VR2[dir];
        }

        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_6) == (0xb0cb1a30)) {
            pthread_mutex_lock(&(rb_state16.lock_40));
            rb_state16.var_11 = 0x5c71ed32;
            rb_state16.var_12 = 0x527ea740;
            rb_state16.var_13 = 0xa218e2da;
            rb_state16.var_14 = (rb_state16.var_14) - ((0x190faa9b) + (0xce2c7fb8));
            rb_state16.var_15 = (rb_state16.var_15) ^ (rb_state16.var_11);
            rb_state16.var_16 = (rb_state16.var_16) ^ (((((0xbee714ae) ^ (rb_state16.var_13)) ^ (rb_state16.var_8)) ^ (rb_state16.var_12)) ^ (0xef6f851a));
            rb_state16.var_17 = (rb_state16.var_17) ^ (((0x964df283) ^ (rb_state16.var_14)) - (0xa60dc74c));
            rb_state16.var_18 = (rb_state16.var_18) - (rb_state16.var_15);
            rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_16) : (0xc481b35b));
            rb_state16.var_20 = (rb_state16.var_20) + (rb_state16.var_17);
            rb_state16.var_21 = (rb_state16.var_21) - (((0x6a40837b) ^ (rb_state16.var_18)) - (0x7088a534));
            rb_state16.var_22 = (rb_state16.var_22) - (rb_state16.var_19);
            rb_state16.var_23 = (rb_state16.var_23) - (((((0xbfb9362b) + (rb_state16.var_20)) ^ (rb_state16.var_21)) + (0xb8626f78)) + (rb_state16.var_10));
            rb_state16.var_24 = (rb_state16.var_24) - (((0x7e65b895) + (0xc468e9b1)) ^ (rb_state16.var_22));
            rb_state16.var_25 = (rb_state16.var_25) + (((0x5db39e7c) ^ (rb_state16.var_11)) ^ (rb_state16.var_23));
            rb_state16.var_26 = (rb_state16.var_26) + (((((0x6b6b4c4f) - (rb_state16.var_24)) - (0x946f3e50)) + (rb_state16.var_25)) - (rb_state16.var_12));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_26);
            rb_state16.var_9 = rb_state16.var_10;
            pthread_mutex_unlock(&(rb_state16.lock_40));
        }
        if ((rb_state16.var_6) == (0xb0cb1a30)) {
            if (!((rb_state16.var_9) == (rb_state16.var_10))) {
                racebench_trigger(16);
            }
        }
        #endif
        R1 = sqrt(R1);
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x5a2764b1)) {
            rb_state14.var_2 = rb_state14.var_1;
        }
        if ((rb_state14.var_0) == (0x5a2764b1)) {
            pthread_mutex_lock(&(rb_state14.lock_7));
            if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
            pthread_mutex_unlock(&(rb_state14.lock_7));
        }
        #endif
        R2 = sqrt(R2);

        COS = SUM / (R1 * R2);
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0xb43345f5));
        #endif
        SIN = sqrt(ONE - COS * COS);
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x75f396c3)) {
            rb_state7.var_14 = 0x243978f3;
            rb_state7.var_15 = 0x451b60d6;
            rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_14) : (0x31e89b7));
            rb_state7.var_17 = (rb_state7.var_17) - (((0x114233c4) + (rb_state7.var_6)) - (rb_state7.var_4));
            rb_state7.var_18 = (rb_state7.var_18) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_16) : (0x2bdcfaa7));
            rb_state7.var_19 = (rb_state7.var_19) + (((((0xad21c7f9) ^ (rb_state7.var_18)) + (rb_state7.var_17)) ^ (rb_state7.var_7)) - (0x5c1bdf4f));
            rb_state7.var_13 = (rb_state7.var_13) ^ (rb_state7.var_19);
            rb_state7.var_1 = rb_state7.var_13;
        }
        if ((rb_state7.var_0) == (0x75f396c3)) {
            pthread_mutex_lock(&(rb_state7.lock_33));
            rb_state7.var_21 = 0xab4cc360;
            rb_state7.var_22 = 0x8efb7c6a;
            rb_state7.var_23 = (rb_state7.var_23) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_21) : (0x920db1bd));
            rb_state7.var_24 = (rb_state7.var_24) ^ (((0xb292c92f) - (rb_state7.var_22)) ^ (0x96f7783));
            rb_state7.var_25 = (rb_state7.var_25) + (rb_state7.var_23);
            rb_state7.var_26 = (rb_state7.var_26) ^ (rb_state7.var_24);
            rb_state7.var_27 = (rb_state7.var_27) ^ (((0xd397be1d) + (rb_state7.var_9)) + (rb_state7.var_25));
            rb_state7.var_28 = (rb_state7.var_28) - (((0xabedb887) ^ (0x9d2a4b96)) - (rb_state7.var_26));
            rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_27) : (0xa27c0bbb));
            rb_state7.var_30 = (rb_state7.var_30) ^ (((0x85f592af) - (rb_state7.var_28)) ^ (0xfe23212b));
            rb_state7.var_31 = (rb_state7.var_31) ^ (rb_state7.var_29);
            rb_state7.var_32 = (rb_state7.var_32) - (((((0xa5fac1eb) ^ (rb_state7.var_30)) ^ (rb_state7.var_11)) + (0x703d078)) - (rb_state7.var_31));
            rb_state7.var_20 = (rb_state7.var_20) ^ (rb_state7.var_32);
            rb_state7.var_1 = rb_state7.var_20;
            pthread_mutex_unlock(&(rb_state7.lock_33));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0x64ae3a4e)) {
            rb_state19.var_2 = rb_state19.var_1;
        }
        if ((rb_state19.var_0) == (0x64ae3a4e)) {
            pthread_mutex_lock(&(rb_state19.lock_7));
            if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                racebench_trigger(19);
            }
            pthread_mutex_unlock(&(rb_state19.lock_7));
        }
        #endif
        DT = (acos(COS) - ANGLE) * ROH;
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0xbc6c036c)) {
            pthread_mutex_lock(&(rb_state3.lock_23));
            rb_state3.var_3 = 0xc7a54a24;
            rb_state3.var_4 = 0xcf9f0caa;
            rb_state3.var_5 = 0x5fbc97c7;
            rb_state3.var_6 = (rb_state3.var_6) + (rb_state3.var_1);
            rb_state3.var_7 = (rb_state3.var_7) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0xd9c75ebc));
            rb_state3.var_8 = (rb_state3.var_8) ^ (((0x582eee45) + (0x3e175ead)) - (rb_state3.var_3));
            rb_state3.var_9 = (rb_state3.var_9) ^ (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_6) : (0xc21566d3));
            rb_state3.var_10 = (rb_state3.var_10) - (((((0x6be87563) ^ (0x3b27facf)) - (rb_state3.var_7)) ^ (rb_state3.var_3)) + (rb_state3.var_8));
            rb_state3.var_11 = (rb_state3.var_11) - (((0x5bf6fc52) + (0xe28b4135)) ^ (rb_state3.var_9));
            rb_state3.var_12 = (rb_state3.var_12) - (((0x49f4e235) - (0xcb195b20)) ^ (rb_state3.var_10));
            rb_state3.var_13 = (rb_state3.var_13) - (((((0x67a22663) ^ (rb_state3.var_12)) + (0x8a7ae56c)) ^ (0xb1aa6912)) - (rb_state3.var_11));
            rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_13);
            rb_state3.var_1 = rb_state3.var_2;
            pthread_mutex_unlock(&(rb_state3.lock_23));
        }
        if ((rb_state3.var_0) == (0xbc6c036c)) {
            if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (0xb12f76f8);
        #endif
        DTS = DT * DT;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0x6c2a9239);
        #endif
        DR1 = R1 - ROH;
        DR1S = DR1 * DR1;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x523bfc00)) {
            pthread_mutex_lock(&(rb_state2.lock_13));
            rb_state2.var_3 = 0xf961ed1c;
            rb_state2.var_4 = (rb_state2.var_4) ^ (0x2f03b71f);
            rb_state2.var_5 = (rb_state2.var_5) ^ (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_1) : (0x83ad21fa));
            rb_state2.var_6 = (rb_state2.var_6) + (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_4) : (0x7b7976ce));
            rb_state2.var_7 = (rb_state2.var_7) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_5) : (0x73dadc70));
            rb_state2.var_8 = (rb_state2.var_8) + (rb_state2.var_6);
            rb_state2.var_9 = (rb_state2.var_9) ^ (((((0xb65603e3) - (rb_state2.var_8)) - (0x826748b1)) ^ (rb_state2.var_4)) - (rb_state2.var_7));
            rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_9);
            rb_state2.var_1 = rb_state2.var_2;
            pthread_mutex_unlock(&(rb_state2.lock_13));
        }
        if ((rb_state2.var_0) == (0x523bfc00)) {
            if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        DR2 = R2 - ROH;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + ((0x5f1e15c7) ^ (0x543f0d3b));
        #endif
        DR2S = DR2 * DR2;

        R1S = ROH / (R1 * SIN);
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0x21b17e38)) {
                rb_state4.var_11 = 0x7542e490;
                rb_state4.var_12 = (rb_state4.var_12) + (rb_state4.var_5);
                rb_state4.var_13 = (rb_state4.var_13) + (594723 < rb_input_size ? (uint32_t)rb_input[594723] : 0x3a516531);
                rb_state4.var_14 = (rb_state4.var_14) ^ (rb_state4.var_6);
                rb_state4.var_15 = (rb_state4.var_15) - (((0x2a3d85b0) ^ (0xfa8f84df)) - (rb_state4.var_11));
                rb_state4.var_16 = (rb_state4.var_16) ^ (((0x1bd99c44) - (rb_state4.var_3)) ^ (rb_state4.var_7));
                rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_4) : (0x8775a536));
                rb_state4.var_18 = (rb_state4.var_18) + (((((0xa84c614f) - (0xef15b7d)) + (rb_state4.var_12)) ^ (rb_state4.var_9)) + (rb_state4.var_13));
                rb_state4.var_19 = (rb_state4.var_19) + (((((0xddec36ba) ^ (rb_state4.var_10)) - (rb_state4.var_14)) ^ (0x1f37a74b)) + (rb_state4.var_15));
                rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_17) : (0x6b397bc4));
                rb_state4.var_21 = (rb_state4.var_21) - (((((0x1b8bfeb0) + (rb_state4.var_11)) - (0xe559f4de)) ^ (rb_state4.var_19)) ^ (rb_state4.var_18));
                rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_21) : (0x6e33eb43));
                rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_22);
                rb_state4.var_1 = rb_state4.var_10;
            }
            if ((rb_state4.var_0) == (0x21b17e38)) {
                pthread_mutex_lock(&(rb_state4.lock_33));
                rb_state4.var_24 = (rb_state4.var_24) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_13) : (0xea2da777));
                rb_state4.var_25 = (rb_state4.var_25) - (rb_state4.var_14);
                rb_state4.var_26 = (rb_state4.var_26) - (rb_state4.var_15);
                rb_state4.var_27 = (rb_state4.var_27) + (rb_state4.var_24);
                rb_state4.var_28 = (rb_state4.var_28) ^ (((0xd9a69338) - (0x4730c6b7)) - (rb_state4.var_25));
                rb_state4.var_29 = (rb_state4.var_29) ^ (((0x9a5402c8) + (rb_state4.var_16)) - (rb_state4.var_26));
                rb_state4.var_30 = (rb_state4.var_30) - (rb_state4.var_27);
                rb_state4.var_31 = (rb_state4.var_31) ^ (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_29) : (0x26dd84ab));
                rb_state4.var_32 = (rb_state4.var_32) + (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_31) : (0x4665fb8));
                rb_state4.var_23 = (rb_state4.var_23) ^ (rb_state4.var_32);
                rb_state4.var_1 = rb_state4.var_23;
                pthread_mutex_unlock(&(rb_state4.lock_33));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x6cea12cc)) {
                rb_state18.var_2 = rb_state18.var_1;
            }
            if ((rb_state18.var_0) == (0x6cea12cc)) {
                if (!((rb_state18.var_1) == (rb_state18.var_2))) {
                    racebench_trigger(18);
                }
            }
            #endif
            DR11[dir] = VR1[dir] / R1;
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0x2793be8d)) {
                rb_state5.var_2 = rb_state5.var_1;
            }
            if ((rb_state5.var_0) == (0x2793be8d)) {
                if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0x8a71f620)) {
                pthread_mutex_lock(&(rb_state6.lock_34));
                if ((rb_state6.var_1) != (0x0)) {
                    if (!((rb_state6.var_1) == (rb_state6.var_30))) {
                        racebench_trigger(6);
                    }
                }
                pthread_mutex_unlock(&(rb_state6.lock_34));
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0xb1e9358a));
            if ((rb_state11.var_0) == (0xeb22b9dc)) {
                if ((rb_state11.var_1) != (0x0)) {
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ ((0x955a7b40) - (0x4a1bb2fb));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (0x5e815321);
            #endif
            DR23[dir] = VR2[dir] / R2;
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (0x23e5fdae);
            #endif
            #ifdef RACEBENCH_BUG_11
            if (!((rb_state11.var_1) == (rb_state11.var_21))) {
                rb_state11.var_29 = rb_state11.var_16;
            }
            if (!((rb_state11.var_1) == (rb_state11.var_21))) {
                if (!((rb_state11.var_16) == (rb_state11.var_29))) {
                    racebench_trigger(11);
                }
            }
            #endif
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (0x89d9fb7a);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - ((0x5db1b10b) - (0xde97f58d));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (166922 < rb_input_size ? (uint32_t)rb_input[166922] : 0xfff85b8d);
            if ((rb_state16.var_0) == (0xffff1e27)) {
                pthread_mutex_lock(&(rb_state16.lock_7));
                rb_state16.var_3 = 0x7d16daca;
                rb_state16.var_4 = (rb_state16.var_4) + (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_3) : (0x6b837118));
                rb_state16.var_5 = (rb_state16.var_5) + (((0x207de5dc) + (rb_state16.var_2)) - (0x1a330a5c));
                rb_state16.var_6 = (rb_state16.var_6) ^ (((((0x4e76253c) ^ (rb_state16.var_4)) ^ (rb_state16.var_5)) + (rb_state16.var_3)) + (0xfe8a1af0));
                rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_6);
                rb_state16.var_2 = rb_state16.var_1;
                pthread_mutex_unlock(&(rb_state16.lock_7));
            }
            #endif
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x75f396c3)) {
            if ((rb_state7.var_1) != (0x0)) {
                if (!((rb_state7.var_1) == (rb_state7.var_20))) {
                    racebench_trigger(7);
                }
            }
        }
        #endif
        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_5);
        #endif
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x2afe855c)) {
            rb_state1.var_10 = 0xd03b0235;
            rb_state1.var_11 = 0xafdbccb0;
            rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_10);
            rb_state1.var_13 = (rb_state1.var_13) + (((((0x16bff548) - (rb_state1.var_11)) - (rb_state1.var_10)) - (0x5157e9e5)) + (rb_state1.var_9));
            rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_11) : (0xfe5683d4));
            rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_12) : (0xf5346808));
            rb_state1.var_16 = (rb_state1.var_16) ^ (rb_state1.var_13);
            rb_state1.var_17 = (rb_state1.var_17) + (((0x156b188b) + (rb_state1.var_14)) ^ (rb_state1.var_14));
            rb_state1.var_18 = (rb_state1.var_18) - (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_16) : (0x1d894d7f));
            rb_state1.var_19 = (rb_state1.var_19) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_17) : (0xa6979b23));
            rb_state1.var_20 = (rb_state1.var_20) - (((((0x927bf1e3) - (rb_state1.var_19)) ^ (rb_state1.var_18)) - (0x81be35e6)) - (0x10a20178));
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_20);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ ((0x393c563) + (0x11a10ce7));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (0xc3fc5b2a);
        #endif
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0x2692ff3e)) {
            if ((rb_state10.var_1) != (0x0)) {
                if (!((rb_state10.var_1) == (rb_state10.var_25))) {
                    racebench_trigger(10);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (927511 < rb_input_size ? (uint32_t)rb_input[927511] : 0x11655fb1);
        #endif
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ ((0xee771cac) - (rb_state3.var_0));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0xd97500c6)) ? (rb_state10.var_0) : (0x8d9f685c));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x5a2764b1)) {
            rb_state14.var_3 = 0x473f0048;
            rb_state14.var_4 = 0x1b6b6658;
            rb_state14.var_5 = (rb_state14.var_5) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_2) : (0x5914f3c4));
            rb_state14.var_6 = (rb_state14.var_6) + (((((0x73224457) + (rb_state14.var_3)) + (rb_state14.var_3)) - (rb_state14.var_4)) - (rb_state14.var_5));
            rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x7ae8b6d2)) {
            rb_state15.var_3 = 0x32793ec7;
            rb_state15.var_4 = 0x8d73ebe4;
            rb_state15.var_5 = (rb_state15.var_5) ^ (((0xe78cd538) + (rb_state15.var_4)) + (rb_state15.var_3));
            rb_state15.var_6 = (rb_state15.var_6) + (0x2f529a90);
            rb_state15.var_7 = (rb_state15.var_7) ^ (((0x3922f771) + (rb_state15.var_3)) + (rb_state15.var_4));
            rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_2) : (0x6764db85));
            rb_state15.var_9 = (rb_state15.var_9) + (((((0x44a311bf) ^ (rb_state15.var_6)) - (0x3fa230ab)) ^ (rb_state15.var_6)) - (rb_state15.var_5));
            rb_state15.var_10 = (rb_state15.var_10) - (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0xdda8cd71));
            rb_state15.var_11 = (rb_state15.var_11) ^ (((((0x9509a34b) - (rb_state15.var_10)) ^ (rb_state15.var_7)) - (0xf59f08e5)) - (rb_state15.var_9));
            rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x5d2de57d)) {
            rb_state17.var_6 = 0x1f9d1d8e;
            rb_state17.var_7 = 0x4411513c;
            rb_state17.var_8 = (rb_state17.var_8) ^ (rb_state17.var_4);
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_6) : (0xc2caa970));
            rb_state17.var_10 = (rb_state17.var_10) ^ (((0x759f0050) - (rb_state17.var_7)) + (rb_state17.var_5));
            rb_state17.var_11 = (rb_state17.var_11) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_8) : (0x11663fb4));
            rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_9);
            rb_state17.var_13 = (rb_state17.var_13) ^ (((((0x2ae4dbc8) - (rb_state17.var_7)) - (rb_state17.var_11)) + (rb_state17.var_10)) - (rb_state17.var_8));
            rb_state17.var_14 = (rb_state17.var_14) - (((0x6e206698) - (rb_state17.var_12)) ^ (0x1a16805f));
            rb_state17.var_15 = (rb_state17.var_15) + (rb_state17.var_13);
            rb_state17.var_16 = (rb_state17.var_16) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_14) : (0xde0ee627));
            rb_state17.var_17 = (rb_state17.var_17) - (((((0x8e7928d8) - (rb_state17.var_16)) + (rb_state17.var_15)) ^ (rb_state17.var_10)) - (0x2d79da33));
            rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_17);
            rb_state17.var_1 = rb_state17.var_5;
        }
        if ((rb_state17.var_0) == (0x5d2de57d)) {
            pthread_mutex_lock(&(rb_state17.lock_35));
            rb_state17.var_19 = (rb_state17.var_19) ^ (2362895 < rb_input_size ? (uint32_t)rb_input[2362895] : 0x927966ff);
            rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_11);
            rb_state17.var_21 = (rb_state17.var_21) + (711541 < rb_input_size ? (uint32_t)rb_input[711541] : 0xe0955af2);
            rb_state17.var_22 = (rb_state17.var_22) - (rb_state17.var_14);
            rb_state17.var_23 = (rb_state17.var_23) ^ (rb_state17.var_13);
            rb_state17.var_24 = (rb_state17.var_24) + (((((0xffcc0db0) + (rb_state17.var_16)) + (rb_state17.var_19)) + (rb_state17.var_15)) - (rb_state17.var_12));
            rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_20) : (0x9122534c));
            rb_state17.var_26 = (rb_state17.var_26) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_21) : (0x941116dd));
            rb_state17.var_27 = (rb_state17.var_27) - (rb_state17.var_22);
            rb_state17.var_28 = (rb_state17.var_28) + (((0x22620fe6) - (rb_state17.var_19)) + (rb_state17.var_23));
            rb_state17.var_29 = (rb_state17.var_29) ^ (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_24) : (0x75438b19));
            rb_state17.var_30 = (rb_state17.var_30) - (((((0x8f4b6caa) - (rb_state17.var_21)) - (rb_state17.var_25)) + (0xd8646536)) + (rb_state17.var_26));
            rb_state17.var_31 = (rb_state17.var_31) ^ (((rb_state17.var_27) == (0x0)) ? (rb_state17.var_28) : (0xf70fc910));
            rb_state17.var_32 = (rb_state17.var_32) - (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_30) : (0x1a41d845));
            rb_state17.var_33 = (rb_state17.var_33) + (rb_state17.var_31);
            rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_32) == (0x0)) ? (rb_state17.var_33) : (0x2055642));
            rb_state17.var_18 = (rb_state17.var_18) ^ (rb_state17.var_34);
            rb_state17.var_1 = rb_state17.var_18;
            pthread_mutex_unlock(&(rb_state17.lock_35));
        }
        #endif
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x1b666c7c)) {
            pthread_mutex_lock(&(rb_state9.lock_6));
            rb_state9.var_3 = (rb_state9.var_3) + (((((0x70a32109) + (0xd8d73a3a)) - (rb_state9.var_2)) - (rb_state9.var_3)) - (rb_state9.var_1));
            rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_3);
            rb_state9.var_1 = rb_state9.var_2;
            pthread_mutex_unlock(&(rb_state9.lock_6));
        }
        if ((rb_state9.var_0) == (0x1b666c7c)) {
            if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (520126 < rb_input_size ? (uint32_t)rb_input[520126] : 0x4307d4);
        #endif
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_6) == (0xb0cb1a30)) {
            rb_state16.var_27 = 0x5ecba85;
            rb_state16.var_28 = (rb_state16.var_28) ^ (rb_state16.var_13);
            rb_state16.var_29 = (rb_state16.var_29) ^ (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_15) : (0xcbe82f37));
            rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_14) : (0x667e95));
            rb_state16.var_31 = (rb_state16.var_31) ^ (((0xa7ddb126) - (rb_state16.var_28)) ^ (rb_state16.var_17));
            rb_state16.var_32 = (rb_state16.var_32) - (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_29) : (0xc46b7d16));
            rb_state16.var_33 = (rb_state16.var_33) - (rb_state16.var_30);
            rb_state16.var_34 = (rb_state16.var_34) + (((((0xb1cfffc8) ^ (rb_state16.var_32)) + (0x889df07)) ^ (rb_state16.var_31)) - (rb_state16.var_19));
            rb_state16.var_35 = (rb_state16.var_35) ^ (((0xbb86e2e2) - (0x2382366b)) ^ (rb_state16.var_33));
            rb_state16.var_36 = (rb_state16.var_36) ^ (rb_state16.var_34);
            rb_state16.var_37 = (rb_state16.var_37) ^ (rb_state16.var_35);
            rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_36);
            rb_state16.var_39 = (rb_state16.var_39) - (((rb_state16.var_37) == (0x0)) ? (rb_state16.var_38) : (0x56a16155));
            rb_state16.var_9 = (rb_state16.var_9) + (rb_state16.var_39);
        }
        #endif
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - ((0x694a5326) + (0x79bd6b97));
            #endif
            #ifdef RACEBENCH_BUG_8
            if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                pthread_mutex_lock(&(rb_state8.lock_71));
                rb_state8.var_29 = (rb_state8.var_29) - (8387 < rb_input_size ? (uint32_t)rb_input[8387] : 0x537e8a22);
                rb_state8.var_30 = (rb_state8.var_30) - (2132836 < rb_input_size ? (uint32_t)rb_input[2132836] : 0x8480fbec);
                rb_state8.var_31 = (rb_state8.var_31) ^ (rb_state8.var_16);
                rb_state8.var_32 = (rb_state8.var_32) + (rb_state8.var_15);
                rb_state8.var_33 = (rb_state8.var_33) - (rb_state8.var_14);
                rb_state8.var_34 = (rb_state8.var_34) ^ (rb_state8.var_29);
                rb_state8.var_35 = (rb_state8.var_35) - (((0xd937bc3e) - (rb_state8.var_30)) + (0x39a55f));
                rb_state8.var_36 = (rb_state8.var_36) ^ (((0x96bb1128) ^ (rb_state8.var_31)) + (rb_state8.var_17));
                rb_state8.var_37 = (rb_state8.var_37) ^ (rb_state8.var_32);
                rb_state8.var_38 = (rb_state8.var_38) - (((0xfaa1a052) + (0xc944ae8b)) - (rb_state8.var_33));
                rb_state8.var_39 = (rb_state8.var_39) - (rb_state8.var_34);
                rb_state8.var_40 = (rb_state8.var_40) + (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_35) : (0xe9e19a7e));
                rb_state8.var_41 = (rb_state8.var_41) - (rb_state8.var_36);
                rb_state8.var_42 = (rb_state8.var_42) ^ (((rb_state8.var_37) == (0x0)) ? (rb_state8.var_38) : (0x634350ae));
                rb_state8.var_43 = (rb_state8.var_43) ^ (((((0x3452e0c2) + (rb_state8.var_39)) + (0x830016b2)) - (rb_state8.var_19)) - (rb_state8.var_40));
                rb_state8.var_44 = (rb_state8.var_44) - (((rb_state8.var_41) == (0x0)) ? (rb_state8.var_42) : (0x7579063c));
                rb_state8.var_45 = (rb_state8.var_45) + (rb_state8.var_43);
                rb_state8.var_46 = (rb_state8.var_46) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_44) : (0x5109b883));
                rb_state8.var_47 = (rb_state8.var_47) - (((rb_state8.var_22) == (0x0)) ? (rb_state8.var_45) : (0x29fba266));
                rb_state8.var_48 = (rb_state8.var_48) ^ (rb_state8.var_46);
                rb_state8.var_49 = (rb_state8.var_49) - (rb_state8.var_47);
                rb_state8.var_50 = (rb_state8.var_50) + (rb_state8.var_48);
                rb_state8.var_51 = (rb_state8.var_51) + (((((0xfeeda6b3) + (rb_state8.var_23)) - (rb_state8.var_49)) ^ (rb_state8.var_50)) + (rb_state8.var_24));
                rb_state8.var_28 = (rb_state8.var_28) ^ (rb_state8.var_51);
                rb_state8.var_22 = rb_state8.var_28;
                pthread_mutex_unlock(&(rb_state8.lock_71));
            }
            if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                pthread_mutex_lock(&(rb_state8.lock_71));
                rb_state8.var_53 = 0x565b462f;
                rb_state8.var_54 = (rb_state8.var_54) + ((0xd13e6548) - (0xcc40d4e8));
                rb_state8.var_55 = (rb_state8.var_55) + (((0x910cdff6) + (rb_state8.var_27)) + (rb_state8.var_53));
                rb_state8.var_56 = (rb_state8.var_56) ^ ((0xc03f2625) - (rb_state8.var_28));
                rb_state8.var_57 = (rb_state8.var_57) + (((rb_state8.var_29) == (0x0)) ? (rb_state8.var_26) : (0x303b513e));
                rb_state8.var_58 = (rb_state8.var_58) ^ (((0x124d4205) + (rb_state8.var_30)) - (rb_state8.var_25));
                rb_state8.var_59 = (rb_state8.var_59) - (rb_state8.var_54);
                rb_state8.var_60 = (rb_state8.var_60) ^ (rb_state8.var_55);
                rb_state8.var_61 = (rb_state8.var_61) - (((rb_state8.var_31) == (0x0)) ? (rb_state8.var_56) : (0x33cd2616));
                rb_state8.var_62 = (rb_state8.var_62) - (((rb_state8.var_57) == (0x0)) ? (rb_state8.var_58) : (0x896693a6));
                rb_state8.var_63 = (rb_state8.var_63) - (rb_state8.var_59);
                rb_state8.var_64 = (rb_state8.var_64) ^ (((rb_state8.var_60) == (0x0)) ? (rb_state8.var_61) : (0xc32e93cf));
                rb_state8.var_65 = (rb_state8.var_65) ^ (rb_state8.var_62);
                rb_state8.var_66 = (rb_state8.var_66) ^ (((rb_state8.var_63) == (0x0)) ? (rb_state8.var_64) : (0xa9187b54));
                rb_state8.var_67 = (rb_state8.var_67) ^ (((0xcb8cc507) ^ (rb_state8.var_65)) ^ (0x42844bbf));
                rb_state8.var_68 = (rb_state8.var_68) - (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_66) : (0xb68103c7));
                rb_state8.var_69 = (rb_state8.var_69) + (((0xcef6f31b) + (rb_state8.var_67)) ^ (rb_state8.var_33));
                rb_state8.var_70 = (rb_state8.var_70) + (((((0x4639d523) - (0x14f735e5)) + (rb_state8.var_34)) + (rb_state8.var_69)) ^ (rb_state8.var_68));
                rb_state8.var_52 = (rb_state8.var_52) - (rb_state8.var_70);
                rb_state8.var_22 = rb_state8.var_52;
                pthread_mutex_unlock(&(rb_state8.lock_71));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (2714032 < rb_input_size ? (uint32_t)rb_input[2714032] : 0xd351c28d);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - (0xa18e393c);
            #endif
            temp_p = VAR[mol].F[FORCES][dir];
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_0) == (0xbc6c036c)) {
                rb_state3.var_14 = 0x5d304246;
                rb_state3.var_15 = (rb_state3.var_15) - (rb_state3.var_6);
                rb_state3.var_16 = (rb_state3.var_16) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x851ca96));
                rb_state3.var_17 = (rb_state3.var_17) ^ (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_14) : (0x5a1fcb0));
                rb_state3.var_18 = (rb_state3.var_18) + (rb_state3.var_15);
                rb_state3.var_19 = (rb_state3.var_19) - (((0x3c0e6c42) ^ (rb_state3.var_16)) + (0x99e03d7f));
                rb_state3.var_20 = (rb_state3.var_20) + (((0x39444325) + (rb_state3.var_17)) ^ (rb_state3.var_8));
                rb_state3.var_21 = (rb_state3.var_21) + (((rb_state3.var_18) == (0x0)) ? (rb_state3.var_19) : (0xa9f62f97));
                rb_state3.var_22 = (rb_state3.var_22) + (((((0x27e18f66) - (0x1f176a58)) + (rb_state3.var_9)) - (rb_state3.var_21)) - (rb_state3.var_20));
                rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_22);
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0x8296cdf8)) {
                pthread_mutex_lock(&(rb_state8.lock_21));
                rb_state8.var_3 = (rb_state8.var_3) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_2) : (0x7b6a134b));
                rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_3);
                rb_state8.var_1 = rb_state8.var_2;
                pthread_mutex_unlock(&(rb_state8.lock_21));
            }
            if ((rb_state8.var_0) == (0x8296cdf8)) {
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (2252367 < rb_input_size ? (uint32_t)rb_input[2252367] : 0xc3e25f67);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - (0xa36fe1d2);
            #endif
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0x2793be8d)) {
                rb_state5.var_3 = 0x9aaaaa00;
                rb_state5.var_4 = 0xd344b602;
                rb_state5.var_5 = (rb_state5.var_5) + (rb_state5.var_4);
                rb_state5.var_6 = (rb_state5.var_6) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0x131b8648));
                rb_state5.var_7 = (rb_state5.var_7) + (rb_state5.var_3);
                rb_state5.var_8 = (rb_state5.var_8) ^ (rb_state5.var_2);
                rb_state5.var_9 = (rb_state5.var_9) + (((0x33ee9bf9) - (rb_state5.var_5)) + (0x46d839e5));
                rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0xf90c60d6));
                rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_8) : (0xbee46a09));
                rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_9);
                rb_state5.var_13 = (rb_state5.var_13) - (((0xe1ca5688) - (rb_state5.var_6)) + (rb_state5.var_10));
                rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_12) : (0x4dddcf24));
                rb_state5.var_15 = (rb_state5.var_15) + (((((0xaba0c9ff) ^ (rb_state5.var_13)) ^ (rb_state5.var_7)) ^ (0x999ac52c)) - (rb_state5.var_14));
                rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_15);
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0x8296cdf8)) {
                rb_state8.var_4 = 0x6d283fdf;
                rb_state8.var_5 = (rb_state8.var_5) + (2303102 < rb_input_size ? (uint32_t)rb_input[2303102] : 0xf5cda919);
                rb_state8.var_6 = (rb_state8.var_6) ^ ((0xc3e43bdf) + (0xc5f7318f));
                rb_state8.var_7 = (rb_state8.var_7) + (536735 < rb_input_size ? (uint32_t)rb_input[536735] : 0x67171e20);
                rb_state8.var_8 = (rb_state8.var_8) + (rb_state8.var_3);
                rb_state8.var_9 = (rb_state8.var_9) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0x6c49d84b));
                rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0xc98e6cb8));
                rb_state8.var_11 = (rb_state8.var_11) - (((0x1b884ecd) - (rb_state8.var_7)) + (rb_state8.var_6));
                rb_state8.var_12 = (rb_state8.var_12) + (rb_state8.var_8);
                rb_state8.var_13 = (rb_state8.var_13) + (rb_state8.var_9);
                rb_state8.var_14 = (rb_state8.var_14) ^ (((0x314a8698) ^ (0x41799184)) + (rb_state8.var_10));
                rb_state8.var_15 = (rb_state8.var_15) - (((((0x9a84137d) + (rb_state8.var_11)) + (0x7f58d485)) ^ (0x1190f7cb)) + (rb_state8.var_12));
                rb_state8.var_16 = (rb_state8.var_16) - (rb_state8.var_13);
                rb_state8.var_17 = (rb_state8.var_17) ^ (((0xd88d78c0) ^ (0x356e65b8)) - (rb_state8.var_14));
                rb_state8.var_18 = (rb_state8.var_18) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_15) : (0x58b19f2f));
                rb_state8.var_19 = (rb_state8.var_19) ^ (((((0x39ad6120) - (rb_state8.var_17)) ^ (0x51598bba)) - (0xf91521db)) ^ (rb_state8.var_16));
                rb_state8.var_20 = (rb_state8.var_20) ^ (((((0xc0600f0b) - (0x2aec3fb8)) ^ (rb_state8.var_8)) + (rb_state8.var_18)) + (rb_state8.var_19));
                rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0x1b666c7c)) {
                pthread_mutex_lock(&(rb_state9.lock_6));
                rb_state9.var_4 = 0x2329fcd6;
                rb_state9.var_5 = (rb_state9.var_5) + (((((0xae29bb91) ^ (0xe6dd8273)) + (rb_state9.var_2)) - (0x2b399848)) + (rb_state9.var_4));
                rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_5);
                pthread_mutex_unlock(&(rb_state9.lock_6));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0x36)) ? (rb_state14.var_0) : (0xa9f00ea9));
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_0) == (0x7ae8b6d2)) {
                rb_state15.var_2 = rb_state15.var_1;
            }
            if ((rb_state15.var_0) == (0x7ae8b6d2)) {
                if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0xffff1e5a)) {
                rb_state16.var_8 = !((rb_state16.var_2) == (rb_state16.var_1));
            }
            #endif
            temp_p[H1] = T1;
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - (629970 < rb_input_size ? (uint32_t)rb_input[629970] : 0xd5a0bf3f);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (0xaadbd7c1);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (2241334 < rb_input_size ? (uint32_t)rb_input[2241334] : 0x5fa676e5);
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x6cea12cc)) {
                rb_state18.var_3 = 0x3f5fa7aa;
                rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_4);
                rb_state18.var_5 = (rb_state18.var_5) + ((0x8f4a6453) ^ (rb_state18.var_5));
                rb_state18.var_6 = (rb_state18.var_6) - (((0xc292ce7) + (rb_state18.var_6)) ^ (rb_state18.var_3));
                rb_state18.var_7 = (rb_state18.var_7) ^ (rb_state18.var_2);
                rb_state18.var_8 = (rb_state18.var_8) - (rb_state18.var_4);
                rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_5) : (0xedbfb335));
                rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_6) : (0xbb70493d));
                rb_state18.var_11 = (rb_state18.var_11) - (((0xfd44c6bd) - (rb_state18.var_9)) ^ (rb_state18.var_7));
                rb_state18.var_12 = (rb_state18.var_12) - (((((0x44e9d599) + (rb_state18.var_10)) ^ (rb_state18.var_11)) ^ (rb_state18.var_9)) + (rb_state18.var_8));
                rb_state18.var_13 = (rb_state18.var_13) - (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_11) : (0xe2b37d14));
                rb_state18.var_14 = (rb_state18.var_14) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_13) : (0x59cff474));
                rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_14);
            }
            #endif
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - ((0x3e245fe2) - (0x51a88c0a));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0xb4c102e9)) {
                rb_state12.var_2 = rb_state12.var_1;
            }
            if ((rb_state12.var_0) == (0xb4c102e9)) {
                if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                    racebench_trigger(12);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x2c8463f0)) ? (rb_state13.var_0) : (0xa35cb735));
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_0) == (0x5d2de57d)) {
                if ((rb_state17.var_1) != (0x0)) {
                    if (!((rb_state17.var_1) == (rb_state17.var_18))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0x64ae3a4e)) {
                rb_state19.var_3 = 0x14ba650a;
                rb_state19.var_4 = (rb_state19.var_4) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_4) : (0x2ad7ef60));
                rb_state19.var_5 = (rb_state19.var_5) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_3) : (0xc3235e56));
                rb_state19.var_6 = (rb_state19.var_6) - (((((0xf3d27c9d) ^ (rb_state19.var_5)) ^ (0xf0e50fc8)) - (rb_state19.var_5)) + (rb_state19.var_4));
                rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_6);
            }
            #endif
            temp_p[H2] = T2;
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_0) == (0xf044dd23)) {
                rb_state13.var_11 = 0x39441c65;
                rb_state13.var_12 = 0xc73f3fb8;
                rb_state13.var_13 = (rb_state13.var_13) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_12) : (0xf92e2596));
                rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_3) : (0x9c9230cf));
                rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_13);
                rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_15) : (0xef871ebb));
                rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_16);
            }
            #endif
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