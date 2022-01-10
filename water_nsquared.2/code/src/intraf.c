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
        SUM = 0.0;
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x4972da2f));
        #endif
        R1 = 0.0;
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_1) == (0xa65d7650)) {
                rb_state4.var_2 = rb_state4.var_0;
            }
            if ((rb_state4.var_1) == (0xa65d7650)) {
                if (!((rb_state4.var_0) == (rb_state4.var_2))) {
                    racebench_trigger(4);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_1);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + ((0x22ffb489) - (0xb99c4e99));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - ((0xefb1b8cc) - (rb_state17.var_0));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) + (2613285 < rb_input_size ? (uint32_t)rb_input[2613285] : 0x44d0ad34);
            #endif
            temp_p = VAR[mol].F[DISP][dir];
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_6) == (0xb8f14ff0)) {
                rb_state0.var_14 = rb_state0.var_9;
            }
            if ((rb_state0.var_6) == (0xb8f14ff0)) {
                if (!((rb_state0.var_9) == (rb_state0.var_14))) {
                    racebench_trigger(0);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0xd9590a1d)) {
                rb_state1.var_3 = 0x7c20b9f2;
                rb_state1.var_4 = 0x3aafe528;
                rb_state1.var_5 = (rb_state1.var_5) - (((0x1c1b9c7e) ^ (rb_state1.var_3)) - (rb_state1.var_3));
                rb_state1.var_6 = (rb_state1.var_6) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_2) : (0x8a3a3bc0));
                rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_5) : (0x227491d));
                rb_state1.var_8 = (rb_state1.var_8) + (rb_state1.var_6);
                rb_state1.var_9 = (rb_state1.var_9) + (((0x6f8b9bcb) + (rb_state1.var_7)) + (0x67ad063));
                rb_state1.var_10 = (rb_state1.var_10) - (((0xa93b37a6) - (0x7ae353d5)) + (rb_state1.var_8));
                rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0xa9e373af));
                rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_11);
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_1) == (0x1acba227)) {
                rb_state2.var_15 = 0x7715de57;
                rb_state2.var_16 = 0x1b9b9be5;
                rb_state2.var_17 = (rb_state2.var_17) - (((((0x337d6eb8) ^ (0xbb947ce1)) ^ (rb_state2.var_9)) ^ (rb_state2.var_16)) + (rb_state2.var_8));
                rb_state2.var_18 = (rb_state2.var_18) - (((((0xe4491c5c) + (0xa9b9b9f4)) + (0xd2ff0cbc)) + (rb_state2.var_17)) + (rb_state2.var_15));
                rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_18);
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xd2d6cfe0)) {
                rb_state9.var_3 = 0x54d3dc8d;
                rb_state9.var_4 = (rb_state9.var_4) ^ (((((0x74933b51) + (rb_state9.var_2)) + (0xb13b6ee6)) ^ (rb_state9.var_3)) ^ (rb_state9.var_4));
                rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_4);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_1) == (0xa8170d87)) {
                rb_state11.var_6 = 0xe3c0907d;
                rb_state11.var_7 = 0x4fed8e5d;
                rb_state11.var_8 = (rb_state11.var_8) - (((0xd7a6e2f) + (rb_state11.var_3)) ^ (0xdb0af82b));
                rb_state11.var_9 = (rb_state11.var_9) + (rb_state11.var_6);
                rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_7);
                rb_state11.var_11 = (rb_state11.var_11) ^ (rb_state11.var_8);
                rb_state11.var_12 = (rb_state11.var_12) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_9) : (0xaa72187a));
                rb_state11.var_13 = (rb_state11.var_13) - (((0x63da3460) ^ (0x1bbae7eb)) ^ (rb_state11.var_10));
                rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_11) : (0xd85a3997));
                rb_state11.var_15 = (rb_state11.var_15) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_12) : (0x136b789e));
                rb_state11.var_16 = (rb_state11.var_16) - (((((0x7326d493) + (rb_state11.var_14)) - (0xdb5ed36d)) + (rb_state11.var_13)) + (0x48053d0d));
                rb_state11.var_17 = (rb_state11.var_17) - (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x4ff08f37));
                rb_state11.var_5 = (rb_state11.var_5) - (rb_state11.var_17);
                rb_state11.var_2 = rb_state11.var_5;
            }
            if ((rb_state11.var_1) == (0xa8170d87)) {
                pthread_mutex_lock(&(rb_state11.lock_26));
                rb_state11.var_19 = 0x9fa5d8b9;
                rb_state11.var_20 = 0xe2c69ec1;
                rb_state11.var_21 = (rb_state11.var_21) + (((((0xaa51e2e1) - (rb_state11.var_20)) - (0x168a7441)) - (rb_state11.var_8)) + (rb_state11.var_7));
                rb_state11.var_22 = (rb_state11.var_22) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_9) : (0x30e834d));
                rb_state11.var_23 = (rb_state11.var_23) + (((((0xe3b106aa) ^ (rb_state11.var_21)) - (0xe8233319)) ^ (rb_state11.var_19)) + (0xa1d01959));
                rb_state11.var_24 = (rb_state11.var_24) + (rb_state11.var_22);
                rb_state11.var_25 = (rb_state11.var_25) ^ (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_24) : (0x458dd9cc));
                rb_state11.var_18 = (rb_state11.var_18) ^ (rb_state11.var_25);
                rb_state11.var_2 = rb_state11.var_18;
                pthread_mutex_unlock(&(rb_state11.lock_26));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_0) == (0x1ac50d00)) {
                rb_state15.var_18 = 0x602ec8fa;
                rb_state15.var_19 = 0x8253f89b;
                rb_state15.var_20 = (rb_state15.var_20) - (rb_state15.var_19);
                rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_9);
                rb_state15.var_22 = (rb_state15.var_22) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_8) : (0x13930f4d));
                rb_state15.var_23 = (rb_state15.var_23) - (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_18) : (0x72cc3db9));
                rb_state15.var_24 = (rb_state15.var_24) + (((((0xdaa48846) ^ (rb_state15.var_20)) ^ (rb_state15.var_12)) + (0x86faad1c)) ^ (rb_state15.var_21));
                rb_state15.var_25 = (rb_state15.var_25) + (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_22) : (0xedb3925a));
                rb_state15.var_26 = (rb_state15.var_26) + (((0x300ab790) - (0xf823432f)) ^ (rb_state15.var_23));
                rb_state15.var_27 = (rb_state15.var_27) ^ (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_24) : (0xc766579a));
                rb_state15.var_28 = (rb_state15.var_28) ^ (rb_state15.var_25);
                rb_state15.var_29 = (rb_state15.var_29) + (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_27) : (0xfc90d051));
                rb_state15.var_30 = (rb_state15.var_30) + (((rb_state15.var_28) == (0x0)) ? (rb_state15.var_29) : (0xfa9138ce));
                rb_state15.var_17 = (rb_state15.var_17) + (rb_state15.var_30);
                rb_state15.var_2 = rb_state15.var_17;
            }
            if ((rb_state15.var_0) == (0x1ac50d00)) {
                pthread_mutex_lock(&(rb_state15.lock_42));
                rb_state15.var_32 = 0xb5ba6cfe;
                rb_state15.var_33 = 0x9d6272e3;
                rb_state15.var_34 = 0x211c59a5;
                rb_state15.var_35 = (rb_state15.var_35) + ((0x3b4c1480) + (rb_state15.var_15));
                rb_state15.var_36 = (rb_state15.var_36) + (rb_state15.var_32);
                rb_state15.var_37 = (rb_state15.var_37) - (((0xbc864f19) - (rb_state15.var_33)) - (0x6052fcb));
                rb_state15.var_38 = (rb_state15.var_38) - (((((0x4e461cb7) + (rb_state15.var_34)) ^ (0x5ff9709d)) - (rb_state15.var_35)) ^ (rb_state15.var_16));
                rb_state15.var_39 = (rb_state15.var_39) - (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_36) : (0xfab709c3));
                rb_state15.var_40 = (rb_state15.var_40) + (((((0xfc51fdb2) - (rb_state15.var_37)) ^ (rb_state15.var_38)) + (0xdfd58a32)) - (rb_state15.var_18));
                rb_state15.var_41 = (rb_state15.var_41) - (((rb_state15.var_39) == (0x0)) ? (rb_state15.var_40) : (0xc7f03289));
                rb_state15.var_31 = (rb_state15.var_31) + (rb_state15.var_41);
                rb_state15.var_2 = rb_state15.var_31;
                pthread_mutex_unlock(&(rb_state15.lock_42));
            }
            #endif
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - ((0x7575fce8) + (0x42ce9a98));
            #endif
            VR1[dir] = temp_p[O] - temp_p[H1];
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_12) == (0xa4d9bb79)) {
                rb_state2.var_28 = 0x579ca52c;
                rb_state2.var_29 = 0xa6422cc4;
                rb_state2.var_30 = (rb_state2.var_30) - ((0x4ab15784) ^ (0x466409ad));
                rb_state2.var_31 = (rb_state2.var_31) + (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_20) : (0xf5caeac9));
                rb_state2.var_32 = (rb_state2.var_32) - (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_29) : (0x7a2805d8));
                rb_state2.var_33 = (rb_state2.var_33) + (((rb_state2.var_23) == (0x0)) ? (rb_state2.var_28) : (0x85f9b671));
                rb_state2.var_34 = (rb_state2.var_34) + (rb_state2.var_18);
                rb_state2.var_35 = (rb_state2.var_35) ^ (((0x18a2e3ed) ^ (rb_state2.var_30)) ^ (0x12126490));
                rb_state2.var_36 = (rb_state2.var_36) + (rb_state2.var_31);
                rb_state2.var_37 = (rb_state2.var_37) - (((rb_state2.var_32) == (0x0)) ? (rb_state2.var_33) : (0x89cd4267));
                rb_state2.var_38 = (rb_state2.var_38) - (((rb_state2.var_24) == (0x0)) ? (rb_state2.var_34) : (0x1a83f8aa));
                rb_state2.var_39 = (rb_state2.var_39) ^ (((((0x592e0473) + (rb_state2.var_25)) + (0xaefc127d)) - (rb_state2.var_35)) + (rb_state2.var_36));
                rb_state2.var_40 = (rb_state2.var_40) + (((0xdfdcad1f) + (rb_state2.var_37)) - (rb_state2.var_26));
                rb_state2.var_41 = (rb_state2.var_41) + (((0xe110ccfa) - (0xcdb266ec)) + (rb_state2.var_38));
                rb_state2.var_42 = (rb_state2.var_42) + (((0x2fbf706a) + (rb_state2.var_39)) + (rb_state2.var_27));
                rb_state2.var_43 = (rb_state2.var_43) - (((0x1cbeef6e) + (0x8f4bd9fb)) - (rb_state2.var_40));
                rb_state2.var_44 = (rb_state2.var_44) ^ (rb_state2.var_41);
                rb_state2.var_45 = (rb_state2.var_45) - (((((0x63a6dad9) + (rb_state2.var_28)) ^ (rb_state2.var_43)) - (rb_state2.var_42)) ^ (0x77fdd2be));
                rb_state2.var_46 = (rb_state2.var_46) - (((0xe5dba952) - (rb_state2.var_44)) ^ (0xbfc9dcfe));
                rb_state2.var_47 = (rb_state2.var_47) - (rb_state2.var_45);
                rb_state2.var_48 = (rb_state2.var_48) - (((((0xab2de14f) + (rb_state2.var_29)) + (rb_state2.var_30)) + (rb_state2.var_46)) - (rb_state2.var_47));
                rb_state2.var_21 = (rb_state2.var_21) ^ (rb_state2.var_48);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_1) == (0x5d51e6f6)) {
                pthread_mutex_lock(&(rb_state6.lock_40));
                rb_state6.var_4 = 0xb64c3051;
                rb_state6.var_5 = 0x3a692bdc;
                rb_state6.var_6 = (rb_state6.var_6) ^ (rb_state6.var_5);
                rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_4) : (0xf2b010df));
                rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_2) : (0x1b2b714a));
                rb_state6.var_9 = (rb_state6.var_9) ^ (((0x8e36117f) - (0xd64461d5)) + (rb_state6.var_6));
                rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_7);
                rb_state6.var_11 = (rb_state6.var_11) + (((0xcc5df447) ^ (0x7ee3eb7d)) ^ (rb_state6.var_8));
                rb_state6.var_12 = (rb_state6.var_12) ^ (((0x99c4a584) ^ (rb_state6.var_9)) - (rb_state6.var_5));
                rb_state6.var_13 = (rb_state6.var_13) + (((((0x59214098) + (rb_state6.var_10)) ^ (rb_state6.var_11)) ^ (rb_state6.var_6)) + (0xbeef9181));
                rb_state6.var_14 = (rb_state6.var_14) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_12) : (0xea1bf7db));
                rb_state6.var_15 = (rb_state6.var_15) ^ (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_13) : (0x7ada07b));
                rb_state6.var_16 = (rb_state6.var_16) ^ (((0xa1cdb544) - (rb_state6.var_14)) + (rb_state6.var_9));
                rb_state6.var_17 = (rb_state6.var_17) + (rb_state6.var_15);
                rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_17) : (0x5bb2a06d));
                rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_18);
                rb_state6.var_2 = rb_state6.var_3;
                pthread_mutex_unlock(&(rb_state6.lock_40));
            }
            if ((rb_state6.var_1) == (0x5d51e6f6)) {
                if (!((rb_state6.var_2) == (rb_state6.var_3))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0x3bb2bee1));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x696365f0)) ? (rb_state9.var_0) : (0x37e5dd13));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_1);
            #endif
            R1 += VR1[dir] * VR1[dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_5);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (0xc91fd113);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ ((0xd2e4e6) + (rb_state16.var_0));
            #endif
            VR2[dir] = temp_p[O] - temp_p[H2];
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0xc2700c8)) {
                rb_state13.var_2 = rb_state13.var_0;
            }
            if ((rb_state13.var_1) == (0xc2700c8)) {
                pthread_mutex_lock(&(rb_state13.lock_4));
                rb_state13.var_5 = !((rb_state13.var_0) == (rb_state13.var_2));
                pthread_mutex_unlock(&(rb_state13.lock_4));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_0) == (0x54456179)) {
                rb_state14.var_16 = 0x4c149a76;
                rb_state14.var_17 = 0xe05ec683;
                rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_11) : (0xd0de93fd));
                rb_state14.var_19 = (rb_state14.var_19) - (0xbc026c17);
                rb_state14.var_20 = (rb_state14.var_20) + (((0x1f69e0f5) - (rb_state14.var_16)) ^ (0xd9d0259c));
                rb_state14.var_21 = (rb_state14.var_21) + (((0xb0b22892) + (rb_state14.var_18)) ^ (rb_state14.var_12));
                rb_state14.var_22 = (rb_state14.var_22) - (rb_state14.var_19);
                rb_state14.var_23 = (rb_state14.var_23) ^ (((((0x48fcb0f8) + (0xac623c2e)) - (rb_state14.var_21)) - (rb_state14.var_20)) + (0x2a6bd726));
                rb_state14.var_24 = (rb_state14.var_24) ^ (((0x48f89e33) - (rb_state14.var_13)) + (rb_state14.var_22));
                rb_state14.var_25 = (rb_state14.var_25) + (((0xec177c64) ^ (rb_state14.var_23)) + (rb_state14.var_14));
                rb_state14.var_26 = (rb_state14.var_26) - (rb_state14.var_24);
                rb_state14.var_27 = (rb_state14.var_27) ^ (((0xe969f3b7) ^ (rb_state14.var_25)) + (rb_state14.var_15));
                rb_state14.var_28 = (rb_state14.var_28) ^ (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_27) : (0xb3a2543a));
                rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_28);
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - ((0xfce769d9) + (0xf062c999));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_0);
            #endif
            R2 += VR2[dir] * VR2[dir];
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_0) == (0x81eb0ee8)) {
                pthread_mutex_lock(&(rb_state10.lock_7));
                rb_state10.var_4 = (rb_state10.var_4) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_3) : (0x89987c17));
                rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_4);
                rb_state10.var_2 = rb_state10.var_3;
                pthread_mutex_unlock(&(rb_state10.lock_7));
            }
            if ((rb_state10.var_0) == (0x3d61dd0)) {
                if (!((rb_state10.var_2) == (rb_state10.var_3))) {
                    racebench_trigger(10);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0xab7a1fee)) {
                if ((rb_state12.var_2) != (0x0)) {
                    if (!((rb_state12.var_2) == (rb_state12.var_30))) {
                        racebench_trigger(12);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_1) == (0xb40ffc81)) {
                rb_state18.var_14 = (rb_state18.var_14) - (rb_state18.var_13);
                rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_12) : (0xd93bf5d4));
                rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_10) : (0xa92c8410));
                rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_14) : (0x5ec627f9));
                rb_state18.var_18 = (rb_state18.var_18) ^ (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_16) : (0x41da3455));
                rb_state18.var_19 = (rb_state18.var_19) + (rb_state18.var_17);
                rb_state18.var_20 = (rb_state18.var_20) + (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_18) : (0x6fa03e01));
                rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_20) : (0xa8e42a30));
                rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_21);
            }
            #endif
            SUM += VR1[dir] * VR2[dir];
        }

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_11 = (rb_state2.var_11) - (219916 < rb_input_size ? (uint32_t)rb_input[219916] : 0xba8a5d42);
        #endif
        R1 = sqrt(R1);
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_1) == (0xa65d7650)) {
            pthread_mutex_lock(&(rb_state4.lock_9));
            rb_state4.var_3 = 0xc344d98b;
            rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_3);
            rb_state4.var_5 = (rb_state4.var_5) - (0x4b8b6be6);
            rb_state4.var_6 = (rb_state4.var_6) ^ (((0x49e00bbc) ^ (rb_state4.var_4)) ^ (rb_state4.var_2));
            rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x83d5819e));
            rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_7) : (0x96838867));
            rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_8);
            pthread_mutex_unlock(&(rb_state4.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (3675394 < rb_input_size ? (uint32_t)rb_input[3675394] : 0x61d30c36);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_0);
        if ((rb_state12.var_1) == (0xab7a1fee)) {
            rb_state12.var_8 = 0x3289d484;
            rb_state12.var_9 = 0x32eb70e2;
            rb_state12.var_10 = (rb_state12.var_10) ^ (rb_state12.var_5);
            rb_state12.var_11 = (rb_state12.var_11) - (557350 < rb_input_size ? (uint32_t)rb_input[557350] : 0x4cb1f06c);
            rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_8) : (0x9cfa1d34));
            rb_state12.var_13 = (rb_state12.var_13) - (rb_state12.var_9);
            rb_state12.var_14 = (rb_state12.var_14) ^ (rb_state12.var_4);
            rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_10) : (0x9f1afc9e));
            rb_state12.var_16 = (rb_state12.var_16) - (((((0x557a39fa) - (0x9f481e11)) ^ (rb_state12.var_12)) - (rb_state12.var_8)) - (rb_state12.var_11));
            rb_state12.var_17 = (rb_state12.var_17) - (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_14) : (0xbfaa671a));
            rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_15);
            rb_state12.var_19 = (rb_state12.var_19) + (((0x3e3b522a) + (0xae1a40f7)) ^ (rb_state12.var_16));
            rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_17) : (0x439320d6));
            rb_state12.var_21 = (rb_state12.var_21) ^ (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_18) : (0x2c6af361));
            rb_state12.var_22 = (rb_state12.var_22) - (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_19) : (0x42f82f07));
            rb_state12.var_23 = (rb_state12.var_23) - (rb_state12.var_20);
            rb_state12.var_24 = (rb_state12.var_24) + (((((0xf020b919) - (rb_state12.var_21)) - (rb_state12.var_22)) ^ (rb_state12.var_12)) ^ (0x4abaf12b));
            rb_state12.var_25 = (rb_state12.var_25) ^ (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_23) : (0x341742b8));
            rb_state12.var_26 = (rb_state12.var_26) - (rb_state12.var_24);
            rb_state12.var_27 = (rb_state12.var_27) + (rb_state12.var_25);
            rb_state12.var_28 = (rb_state12.var_28) - (rb_state12.var_26);
            rb_state12.var_29 = (rb_state12.var_29) ^ (((rb_state12.var_27) == (0x0)) ? (rb_state12.var_28) : (0xfe733e72));
            rb_state12.var_7 = (rb_state12.var_7) - (rb_state12.var_29);
            rb_state12.var_2 = rb_state12.var_7;
        }
        if ((rb_state12.var_1) == (0xab7a1fee)) {
            pthread_mutex_lock(&(rb_state12.lock_39));
            rb_state12.var_31 = 0x5f4862a4;
            rb_state12.var_32 = (rb_state12.var_32) + ((0xbb5b2faf) ^ (0xd3b1fddb));
            rb_state12.var_33 = (rb_state12.var_33) + (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_31) : (0xe01c2a10));
            rb_state12.var_34 = (rb_state12.var_34) ^ (((0xf421262f) ^ (rb_state12.var_16)) ^ (rb_state12.var_15));
            rb_state12.var_35 = (rb_state12.var_35) ^ (rb_state12.var_32);
            rb_state12.var_36 = (rb_state12.var_36) - (((((0xb1c5e369) ^ (0x3d28f358)) + (rb_state12.var_34)) ^ (0xd6abdd66)) + (rb_state12.var_33));
            rb_state12.var_37 = (rb_state12.var_37) - (((0x52ce69c0) ^ (rb_state12.var_17)) ^ (rb_state12.var_35));
            rb_state12.var_38 = (rb_state12.var_38) + (((rb_state12.var_36) == (0x0)) ? (rb_state12.var_37) : (0x6e924969));
            rb_state12.var_30 = (rb_state12.var_30) - (rb_state12.var_38);
            rb_state12.var_2 = rb_state12.var_30;
            pthread_mutex_unlock(&(rb_state12.lock_39));
        }
        #endif
        R2 = sqrt(R2);

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - ((0xf312bbb) + (rb_state2.var_0));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0xb7017768)) ? (rb_state14.var_1) : (0x48b83d0d));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + ((0xc664fe3c) - (0x80aff73f));
        #endif
        COS = SUM / (R1 * R2);
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (906703 < rb_input_size ? (uint32_t)rb_input[906703] : 0x983e874e);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0xd87852cb)) {
            pthread_mutex_lock(&(rb_state19.lock_10));
            rb_state19.var_3 = 0x42bd5c66;
            rb_state19.var_4 = 0x9cf7e27d;
            rb_state19.var_5 = (rb_state19.var_5) - (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_4) : (0xf88151af));
            rb_state19.var_6 = (rb_state19.var_6) + (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_3) : (0x7b957ba4));
            rb_state19.var_7 = (rb_state19.var_7) + (rb_state19.var_5);
            rb_state19.var_8 = (rb_state19.var_8) - (rb_state19.var_6);
            rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0x4df07fee));
            rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_9);
            pthread_mutex_unlock(&(rb_state19.lock_10));
        }
        #endif
        SIN = sqrt(ONE - COS * COS);
        DT = (acos(COS) - ANGLE) * ROH;
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_1) == (0xca1be046)) {
            rb_state8.var_3 = 0x84f9ffeb;
            rb_state8.var_4 = 0xbe9835f8;
            rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_3);
            rb_state8.var_6 = (rb_state8.var_6) - (rb_state8.var_2);
            rb_state8.var_7 = (rb_state8.var_7) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_5) : (0xa859525d));
            rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0xe0aa31b7));
            rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) ^ ((0x5e6d7f24) ^ (0x2252964f));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ (2094836 < rb_input_size ? (uint32_t)rb_input[2094836] : 0x19a6224);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x84e51c03)) {
            rb_state16.var_2 = rb_state16.var_1;
        }
        if ((rb_state16.var_0) == (0x84e51c03)) {
            if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                racebench_trigger(16);
            }
        }
        #endif
        DTS = DT * DT;
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_1) == (0x9833e77)) {
            rb_state5.var_2 = rb_state5.var_0;
        }
        if ((rb_state5.var_1) == (0x9833e77)) {
            if (!((rb_state5.var_0) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0x58fbf805));
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_5) == (0x1)) {
            rb_state13.var_9 = 0xb02d1e0f;
            rb_state13.var_10 = (rb_state13.var_10) - (((((0x15572104) - (rb_state13.var_7)) ^ (rb_state13.var_10)) + (rb_state13.var_9)) + (rb_state13.var_9));
            rb_state13.var_11 = (rb_state13.var_11) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_8) : (0xd36e075a));
            rb_state13.var_12 = (rb_state13.var_12) ^ (rb_state13.var_10);
            rb_state13.var_13 = (rb_state13.var_13) - (((((0xf998871b) ^ (rb_state13.var_11)) ^ (rb_state13.var_12)) - (rb_state13.var_13)) + (rb_state13.var_12));
            rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_13);
            rb_state13.var_6 = rb_state13.var_8;
        }
        if ((rb_state13.var_5) == (0x1)) {
            pthread_mutex_lock(&(rb_state13.lock_18));
            rb_state13.var_15 = 0xf107aff4;
            rb_state13.var_16 = (rb_state13.var_16) - (((0x5d9351f) ^ (rb_state13.var_16)) ^ (rb_state13.var_15));
            rb_state13.var_17 = (rb_state13.var_17) ^ (((((0x330361b3) ^ (0x52b5a511)) - (rb_state13.var_17)) - (rb_state13.var_16)) ^ (rb_state13.var_14));
            rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_17);
            rb_state13.var_6 = rb_state13.var_14;
            pthread_mutex_unlock(&(rb_state13.lock_18));
        }
        #endif
        DR1 = R1 - ROH;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0x826a1fa3));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (((rb_state16.var_0) == (0x1488f7ac)) ? (rb_state16.var_0) : (0x211d9aa2));
        #endif
        DR1S = DR1 * DR1;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xfbbf42f8));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (2930452 < rb_input_size ? (uint32_t)rb_input[2930452] : 0xbe6f442c);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_1) == (0xaa22c374)) {
            rb_state17.var_2 = rb_state17.var_0;
        }
        if ((rb_state17.var_1) == (0xaa22c374)) {
            if (!((rb_state17.var_0) == (rb_state17.var_2))) {
                racebench_trigger(17);
            }
        }
        #endif
        DR2 = R2 - ROH;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (0x594ce865);
        #endif
        DR2S = DR2 * DR2;

        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_1);
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_1) == (0x5e759f42)) {
            rb_state7.var_10 = 0x45070696;
            rb_state7.var_11 = 0x4e308baf;
            rb_state7.var_12 = (rb_state7.var_12) ^ (((0xfdc34d9c) - (rb_state7.var_4)) ^ (rb_state7.var_10));
            rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_5);
            rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_11) : (0x6417a881));
            rb_state7.var_15 = (rb_state7.var_15) ^ (rb_state7.var_12);
            rb_state7.var_16 = (rb_state7.var_16) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_13) : (0x6f80b829));
            rb_state7.var_17 = (rb_state7.var_17) - (rb_state7.var_14);
            rb_state7.var_18 = (rb_state7.var_18) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_16) : (0xee3edbf3));
            rb_state7.var_19 = (rb_state7.var_19) + (rb_state7.var_17);
            rb_state7.var_20 = (rb_state7.var_20) + (rb_state7.var_18);
            rb_state7.var_21 = (rb_state7.var_21) ^ (rb_state7.var_19);
            rb_state7.var_22 = (rb_state7.var_22) - (rb_state7.var_20);
            rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_21) : (0xc718668c));
            rb_state7.var_24 = (rb_state7.var_24) + (((((0x76afe1ff) - (rb_state7.var_8)) ^ (0xf2b76816)) + (rb_state7.var_23)) + (rb_state7.var_22));
            rb_state7.var_9 = (rb_state7.var_9) ^ (rb_state7.var_24);
            rb_state7.var_2 = rb_state7.var_9;
        }
        if ((rb_state7.var_1) == (0x5e759f42)) {
            pthread_mutex_lock(&(rb_state7.lock_40));
            rb_state7.var_26 = 0x76559ae4;
            rb_state7.var_27 = (rb_state7.var_27) - (0x68697a37);
            rb_state7.var_28 = (rb_state7.var_28) + (rb_state7.var_11);
            rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_26) : (0xbff3250f));
            rb_state7.var_30 = (rb_state7.var_30) - (((((0xfb4060d9) - (0x4c531034)) - (rb_state7.var_9)) + (rb_state7.var_10)) - (0x25db91c1));
            rb_state7.var_31 = (rb_state7.var_31) ^ (((0x2e3a0b73) - (rb_state7.var_27)) - (0x93ac9aec));
            rb_state7.var_32 = (rb_state7.var_32) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_28) : (0x72b328c9));
            rb_state7.var_33 = (rb_state7.var_33) ^ (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_29) : (0x8402cc8c));
            rb_state7.var_34 = (rb_state7.var_34) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_30) : (0x3a165e6a));
            rb_state7.var_35 = (rb_state7.var_35) + (((rb_state7.var_31) == (0x0)) ? (rb_state7.var_32) : (0xcf8ff06a));
            rb_state7.var_36 = (rb_state7.var_36) ^ (((((0x552a23cc) + (rb_state7.var_34)) - (0x65765509)) + (0x24169b47)) - (rb_state7.var_33));
            rb_state7.var_37 = (rb_state7.var_37) ^ (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_35) : (0xdb817bca));
            rb_state7.var_38 = (rb_state7.var_38) ^ (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_36) : (0x292066d5));
            rb_state7.var_39 = (rb_state7.var_39) + (((rb_state7.var_37) == (0x0)) ? (rb_state7.var_38) : (0xdae97b06));
            rb_state7.var_25 = (rb_state7.var_25) + (rb_state7.var_39);
            rb_state7.var_2 = rb_state7.var_25;
            pthread_mutex_unlock(&(rb_state7.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x84e51c03)) {
            rb_state16.var_3 = 0x5e2a417e;
            rb_state16.var_4 = 0x180a1988;
            rb_state16.var_5 = (rb_state16.var_5) - (((0x82e73ce5) ^ (0x4b499265)) - (rb_state16.var_3));
            rb_state16.var_6 = (rb_state16.var_6) + (((((0xa5ba7786) ^ (rb_state16.var_4)) - (rb_state16.var_2)) + (rb_state16.var_4)) + (rb_state16.var_3));
            rb_state16.var_7 = (rb_state16.var_7) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_5) : (0x71c47249));
            rb_state16.var_8 = (rb_state16.var_8) ^ (((0x81cfb313) + (rb_state16.var_6)) + (rb_state16.var_6));
            rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0xdbb1ba94));
            rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_9);
        }
        #endif
        R1S = ROH / (R1 * SIN);
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (0x33684a1f);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - ((0x4ccdccb2) - (rb_state18.var_0));
        #endif
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) - (3548106 < rb_input_size ? (uint32_t)rb_input[3548106] : 0xe6a24768);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) + (3307290 < rb_input_size ? (uint32_t)rb_input[3307290] : 0xde174e04);
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_0) == (0x54456179)) {
                pthread_mutex_lock(&(rb_state14.lock_29));
                rb_state14.var_4 = 0x7afb3dd2;
                rb_state14.var_5 = 0xf222311f;
                rb_state14.var_6 = (rb_state14.var_6) + (((0x9cf89cb2) + (rb_state14.var_5)) - (rb_state14.var_3));
                rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_4);
                rb_state14.var_8 = (rb_state14.var_8) - (((((0x59d19cdd) ^ (rb_state14.var_2)) + (rb_state14.var_4)) + (rb_state14.var_5)) + (rb_state14.var_6));
                rb_state14.var_9 = (rb_state14.var_9) - (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_6) : (0x2961bdbd));
                rb_state14.var_10 = (rb_state14.var_10) + (((0x69d9f111) + (rb_state14.var_7)) + (0x7dead280));
                rb_state14.var_11 = (rb_state14.var_11) ^ (rb_state14.var_8);
                rb_state14.var_12 = (rb_state14.var_12) ^ (((((0x569aa434) + (0x37c66bed)) ^ (rb_state14.var_8)) + (rb_state14.var_9)) ^ (rb_state14.var_10));
                rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_11) : (0x101cd35c));
                rb_state14.var_14 = (rb_state14.var_14) + (((0xbe44b6c2) - (rb_state14.var_10)) + (rb_state14.var_12));
                rb_state14.var_15 = (rb_state14.var_15) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_14) : (0x9d4bc508));
                rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_15);
                rb_state14.var_2 = rb_state14.var_3;
                pthread_mutex_unlock(&(rb_state14.lock_29));
            }
            if ((rb_state14.var_0) == (0x54456179)) {
                if (!((rb_state14.var_2) == (rb_state14.var_3))) {
                    racebench_trigger(14);
                }
            }
            #endif
            DR11[dir] = VR1[dir] / R1;
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_0);
            if ((rb_state0.var_1) == (0xf7d3a861)) {
                pthread_mutex_lock(&(rb_state0.lock_12));
                rb_state0.var_4 = 0xfb669a63;
                rb_state0.var_5 = 0x8ef21b67;
                rb_state0.var_6 = 0xb8f14ff0;
                rb_state0.var_7 = (rb_state0.var_7) + (((0x86f859fd) - (rb_state0.var_5)) - (0xb5a1037d));
                rb_state0.var_8 = (rb_state0.var_8) ^ (((((0xc5629ecb) - (rb_state0.var_4)) - (rb_state0.var_2)) + (rb_state0.var_6)) - (rb_state0.var_3));
                rb_state0.var_9 = (rb_state0.var_9) ^ (rb_state0.var_7);
                rb_state0.var_10 = (rb_state0.var_10) - (((0xe68acc7) - (rb_state0.var_4)) + (rb_state0.var_8));
                rb_state0.var_11 = (rb_state0.var_11) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_10) : (0xa93169e0));
                rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_11);
                rb_state0.var_3 = rb_state0.var_2;
                pthread_mutex_unlock(&(rb_state0.lock_12));
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_0) : (0x55d06821));
            if ((rb_state3.var_1) == (0xdbad3ca9)) {
                pthread_mutex_lock(&(rb_state3.lock_13));
                rb_state3.var_4 = 0xaa8dc259;
                rb_state3.var_5 = 0x10afc153;
                rb_state3.var_6 = 0xa9412da2;
                rb_state3.var_7 = (rb_state3.var_7) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_5) : (0xba1b140c));
                rb_state3.var_8 = (rb_state3.var_8) ^ (rb_state3.var_4);
                rb_state3.var_9 = (rb_state3.var_9) + (((((0x8e0c7d55) ^ (rb_state3.var_2)) - (rb_state3.var_7)) + (rb_state3.var_8)) ^ (rb_state3.var_3));
                rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_9);
                rb_state3.var_2 = rb_state3.var_3;
                pthread_mutex_unlock(&(rb_state3.lock_13));
            }
            if ((rb_state3.var_1) == (0xdbad3ca9)) {
                if (!((rb_state3.var_2) == (rb_state3.var_3))) {
                    racebench_trigger(3);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (0x280744f1);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) - (0x77a6e67e);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0x632fc41a));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + ((0xb5133da1) - (rb_state9.var_0));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) - (3396093 < rb_input_size ? (uint32_t)rb_input[3396093] : 0xb1b4a850);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (3524207 < rb_input_size ? (uint32_t)rb_input[3524207] : 0x1a42e0be);
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_1) == (0xb40ffc81)) {
                pthread_mutex_lock(&(rb_state18.lock_22));
                rb_state18.var_4 = 0xeda9fe48;
                rb_state18.var_5 = 0x169370b0;
                rb_state18.var_6 = (rb_state18.var_6) + (((0x48319b9b) - (rb_state18.var_3)) + (rb_state18.var_2));
                rb_state18.var_7 = (rb_state18.var_7) + (((0x10eb4161) ^ (rb_state18.var_4)) ^ (rb_state18.var_5));
                rb_state18.var_8 = (rb_state18.var_8) - (((((0x9ea3429b) - (rb_state18.var_6)) ^ (rb_state18.var_6)) + (rb_state18.var_5)) ^ (rb_state18.var_4));
                rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_7) : (0x8975a70c));
                rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_8) : (0x2999640d));
                rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_9) : (0xd4e52873));
                rb_state18.var_12 = (rb_state18.var_12) ^ (((0xdc161fca) - (rb_state18.var_10)) - (0xc531c9bb));
                rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0x5a84ac43));
                rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_13);
                rb_state18.var_2 = rb_state18.var_3;
                pthread_mutex_unlock(&(rb_state18.lock_22));
            }
            if ((rb_state18.var_1) == (0xb40ffc81)) {
                if (!((rb_state18.var_2) == (rb_state18.var_3))) {
                    racebench_trigger(18);
                }
            }
            #endif
            DR23[dir] = VR2[dir] / R2;
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0xd9590a1d)) {
                rb_state1.var_2 = rb_state1.var_0;
            }
            if ((rb_state1.var_1) == (0xd9590a1d)) {
                if (!((rb_state1.var_0) == (rb_state1.var_2))) {
                    racebench_trigger(1);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_1) == (0x1acba227)) {
                pthread_mutex_lock(&(rb_state2.lock_19));
                rb_state2.var_4 = 0x2fe8aba2;
                rb_state2.var_5 = 0xb3fabd1a;
                rb_state2.var_6 = (rb_state2.var_6) + (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_3) : (0xcf677e4d));
                rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0xea131c80));
                rb_state2.var_8 = (rb_state2.var_8) - (rb_state2.var_2);
                rb_state2.var_9 = (rb_state2.var_9) + (rb_state2.var_6);
                rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_7) : (0x1a5f6ad8));
                rb_state2.var_11 = (rb_state2.var_11) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_8) : (0xa7bcbef0));
                rb_state2.var_12 = (rb_state2.var_12) - (rb_state2.var_9);
                rb_state2.var_13 = (rb_state2.var_13) - (((((0xfe32add5) + (rb_state2.var_11)) + (rb_state2.var_10)) - (0x801e8855)) - (0x96be425e));
                rb_state2.var_14 = (rb_state2.var_14) ^ (((((0xc796e69b) - (rb_state2.var_12)) ^ (rb_state2.var_13)) ^ (0x3d70b820)) ^ (rb_state2.var_7));
                rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_14);
                rb_state2.var_2 = rb_state2.var_3;
                pthread_mutex_unlock(&(rb_state2.lock_19));
            }
            if ((rb_state2.var_1) == (0x1acba227)) {
                rb_state2.var_20 = !((rb_state2.var_2) == (rb_state2.var_3));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) + (0x2f890cd8);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - ((0x42a6d98) + (0x7cfaad20));
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_1) == (0xaa22c374)) {
                pthread_mutex_lock(&(rb_state17.lock_19));
                rb_state17.var_3 = 0xb0500cd9;
                rb_state17.var_4 = 0xbd22ddec;
                rb_state17.var_5 = (rb_state17.var_5) ^ ((0x9ec8eaff) + (0xd07dd148));
                rb_state17.var_6 = (rb_state17.var_6) - (0x11a86b9);
                rb_state17.var_7 = (rb_state17.var_7) + (rb_state17.var_2);
                rb_state17.var_8 = (rb_state17.var_8) - (rb_state17.var_4);
                rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_3);
                rb_state17.var_10 = (rb_state17.var_10) ^ (((0x2a89768) - (0x398b558c)) ^ (rb_state17.var_5));
                rb_state17.var_11 = (rb_state17.var_11) + (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_6) : (0x706f3c82));
                rb_state17.var_12 = (rb_state17.var_12) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0x3823443));
                rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_10) : (0x823c7229));
                rb_state17.var_14 = (rb_state17.var_14) ^ (((0xcf54f51) ^ (rb_state17.var_11)) - (0x2f1f4b70));
                rb_state17.var_15 = (rb_state17.var_15) - (((0x65c8e55c) - (rb_state17.var_4)) + (rb_state17.var_12));
                rb_state17.var_16 = (rb_state17.var_16) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_13) : (0x273c14bb));
                rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_15) : (0xd168e5ab));
                rb_state17.var_18 = (rb_state17.var_18) + (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_17) : (0xcbc734c4));
                rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_18);
                pthread_mutex_unlock(&(rb_state17.lock_19));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) ^ (0xf9363101);
            #endif
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_8 = (rb_state0.var_8) ^ (1602399 < rb_input_size ? (uint32_t)rb_input[1602399] : 0x17e3b8a0);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) + (0x3402a39c);
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_0) == (0x1ac50d00)) {
                if ((rb_state15.var_2) != (0x0)) {
                    if (!((rb_state15.var_2) == (rb_state15.var_31))) {
                        racebench_trigger(15);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (0x6e30b313);
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0xd87852cb)) {
                pthread_mutex_lock(&(rb_state19.lock_10));
                rb_state19.var_2 = rb_state19.var_1;
                pthread_mutex_unlock(&(rb_state19.lock_10));
            }
            if ((rb_state19.var_0) == (0xd87852cb)) {
                pthread_mutex_lock(&(rb_state19.lock_10));
                if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                    racebench_trigger(19);
                }
                pthread_mutex_unlock(&(rb_state19.lock_10));
            }
            #endif
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_12) == (0xa4d9bb79)) {
            pthread_mutex_lock(&(rb_state2.lock_49));
            rb_state2.var_23 = (rb_state2.var_23) + (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_13) : (0x1587788d));
            rb_state2.var_24 = (rb_state2.var_24) - (((0xf66bf70a) ^ (rb_state2.var_17)) - (rb_state2.var_15));
            rb_state2.var_25 = (rb_state2.var_25) - (rb_state2.var_14);
            rb_state2.var_26 = (rb_state2.var_26) - (((rb_state2.var_23) == (0x0)) ? (rb_state2.var_24) : (0xc9d40d19));
            rb_state2.var_27 = (rb_state2.var_27) + (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_26) : (0x14755245));
            rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_27);
            rb_state2.var_21 = rb_state2.var_22;
            pthread_mutex_unlock(&(rb_state2.lock_49));
        }
        if ((rb_state2.var_12) == (0xa4d9bb79)) {
            if (!((rb_state2.var_21) == (rb_state2.var_22))) {
                racebench_trigger(2);
            }
        }
        #endif
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_6) == (0xb8f14ff0)) {
            rb_state0.var_15 = 0x6ddc556e;
            rb_state0.var_16 = (rb_state0.var_16) + (((((0x52088959) + (rb_state0.var_15)) - (rb_state0.var_11)) + (0x18177e1)) + (rb_state0.var_13));
            rb_state0.var_17 = (rb_state0.var_17) - (((0xddda85cf) ^ (rb_state0.var_10)) + (rb_state0.var_14));
            rb_state0.var_18 = (rb_state0.var_18) ^ (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_17) : (0xfd2d96c4));
            rb_state0.var_9 = (rb_state0.var_9) + (rb_state0.var_18);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_1) : (0x247ef574));
        #endif
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x17812130));
        #endif
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (3629177 < rb_input_size ? (uint32_t)rb_input[3629177] : 0x5c360140);
        #endif
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_0) : (0x2ea60df5));
        #endif
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (66848 < rb_input_size ? (uint32_t)rb_input[66848] : 0x8b796fbb);
        #endif
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (0xd1dcf6f2);
        #endif
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (2486204 < rb_input_size ? (uint32_t)rb_input[2486204] : 0x88016de5);
        #endif
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_1) == (0x57b6cb17)) {
                usleep(100);
            }
            if ((rb_state0.var_1) == (0xf7d3a861)) {
                rb_state0.var_13 = !((rb_state0.var_3) == (rb_state0.var_2));
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0xe19da88a)) ? (rb_state2.var_1) : (0xdc6fd887));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (0x57c1d28a);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x3c9f5c79));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) + (1519828 < rb_input_size ? (uint32_t)rb_input[1519828] : 0x34008594);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) - ((0x6ce17370) - (rb_state18.var_1));
            #endif
            temp_p = VAR[mol].F[FORCES][dir];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + ((0xda6cbf2) + (rb_state1.var_1));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) - (0xb50ac932);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_1) : (0x1eb36906));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x990b4c86)) ? (rb_state6.var_0) : (0xbdc8b285));
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xd2d6cfe0)) {
                pthread_mutex_lock(&(rb_state9.lock_5));
                rb_state9.var_2 = rb_state9.var_1;
                pthread_mutex_unlock(&(rb_state9.lock_5));
            }
            if ((rb_state9.var_0) == (0xd2d6cfe0)) {
                pthread_mutex_lock(&(rb_state9.lock_5));
                if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                    racebench_trigger(9);
                }
                pthread_mutex_unlock(&(rb_state9.lock_5));
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) + (0x8d01cc4c);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) + (0x7c947c3);
            #endif
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) + ((0x9cd7eb6a) - (0xb79c9f5b));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (1053771 < rb_input_size ? (uint32_t)rb_input[1053771] : 0x23d65cba);
            #endif
            temp_p[H1] = T1;
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) ^ (0xfd7d592b);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (0xd11615ff);
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0xc2700c8)) {
                rb_state13.var_3 = (rb_state13.var_3) - (((rb_state13.var_1) == (0x120)) ? (rb_state13.var_2) : (0x81601f89));
                rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_3);
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - ((0xa4b661d7) ^ (0x13c68cf8));
            #endif
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - ((0xabcb7392) - (0x10b8c83f));
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + ((0x16391a7e) + (0xe0cd8db5));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_10 = (rb_state2.var_10) - (0x614380b0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) - ((0xb9a022b4) ^ (rb_state5.var_1));
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_1) == (0xca1be046)) {
                rb_state8.var_2 = rb_state8.var_0;
            }
            if ((rb_state8.var_1) == (0xca1be046)) {
                pthread_mutex_lock(&(rb_state8.lock_9));
                if (!((rb_state8.var_0) == (rb_state8.var_2))) {
                    racebench_trigger(8);
                }
                pthread_mutex_unlock(&(rb_state8.lock_9));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
            if ((rb_state10.var_0) == (0x3d61dd0)) {
                rb_state10.var_5 = 0x7d47e080;
                rb_state10.var_6 = (rb_state10.var_6) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0x215b9693));
                rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_6);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_1) == (0xa8170d87)) {
                if ((rb_state11.var_2) != (0x0)) {
                    if (!((rb_state11.var_2) == (rb_state11.var_18))) {
                        racebench_trigger(11);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_0);
            if ((rb_state13.var_5) == (0x1)) {
                if ((rb_state13.var_6) != (0x0)) {
                    if (!((rb_state13.var_6) == (rb_state13.var_14))) {
                        racebench_trigger(13);
                    }
                }
            }
            #endif
            temp_p[H2] = T2;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x91db0300));
            #endif
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_1) == (0xdbad3ca9)) {
                pthread_mutex_lock(&(rb_state3.lock_13));
                rb_state3.var_10 = 0xb1acfd3c;
                rb_state3.var_11 = (rb_state3.var_11) ^ (((((0x6711b0d1) - (rb_state3.var_4)) ^ (rb_state3.var_7)) ^ (rb_state3.var_5)) - (rb_state3.var_6));
                rb_state3.var_12 = (rb_state3.var_12) ^ (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_11) : (0x11a04174));
                rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_12);
                pthread_mutex_unlock(&(rb_state3.lock_13));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_1) == (0x9833e77)) {
                rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_2);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_1) == (0x5d51e6f6)) {
                rb_state6.var_19 = 0x341775f5;
                rb_state6.var_20 = (rb_state6.var_20) + (0x9d6ecca4);
                rb_state6.var_21 = (rb_state6.var_21) ^ (rb_state6.var_12);
                rb_state6.var_22 = (rb_state6.var_22) - (rb_state6.var_13);
                rb_state6.var_23 = (rb_state6.var_23) + (((0x9a574443) ^ (rb_state6.var_10)) ^ (0xe94d0952));
                rb_state6.var_24 = (rb_state6.var_24) - (((((0x57fe031e) + (rb_state6.var_14)) ^ (rb_state6.var_11)) + (rb_state6.var_19)) ^ (0xa3d76b6a));
                rb_state6.var_25 = (rb_state6.var_25) + (rb_state6.var_20);
                rb_state6.var_26 = (rb_state6.var_26) + (((0x3581203e) - (rb_state6.var_15)) + (rb_state6.var_21));
                rb_state6.var_27 = (rb_state6.var_27) + (((0x122d9e6b) - (rb_state6.var_22)) ^ (rb_state6.var_16));
                rb_state6.var_28 = (rb_state6.var_28) + (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_23) : (0x9e91c49e));
                rb_state6.var_29 = (rb_state6.var_29) - (((0x3e134498) - (rb_state6.var_24)) ^ (rb_state6.var_18));
                rb_state6.var_30 = (rb_state6.var_30) - (((((0xe683f6f0) - (0xc6742c66)) ^ (rb_state6.var_19)) - (rb_state6.var_26)) - (rb_state6.var_25));
                rb_state6.var_31 = (rb_state6.var_31) + (((0x4c8050df) + (rb_state6.var_27)) + (rb_state6.var_20));
                rb_state6.var_32 = (rb_state6.var_32) - (((0xfcdf3d7d) ^ (rb_state6.var_21)) - (rb_state6.var_28));
                rb_state6.var_33 = (rb_state6.var_33) ^ (rb_state6.var_29);
                rb_state6.var_34 = (rb_state6.var_34) - (((0x76630ea9) - (rb_state6.var_22)) - (rb_state6.var_30));
                rb_state6.var_35 = (rb_state6.var_35) ^ (rb_state6.var_31);
                rb_state6.var_36 = (rb_state6.var_36) ^ (rb_state6.var_32);
                rb_state6.var_37 = (rb_state6.var_37) - (((rb_state6.var_33) == (0x0)) ? (rb_state6.var_34) : (0xd408a046));
                rb_state6.var_38 = (rb_state6.var_38) - (((rb_state6.var_35) == (0x0)) ? (rb_state6.var_36) : (0xe56e5d4a));
                rb_state6.var_39 = (rb_state6.var_39) ^ (((((0xa2be1a46) - (rb_state6.var_37)) + (rb_state6.var_38)) + (0x1a0b56d7)) + (0xf8a7f42));
                rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_39);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_1) == (0x5e759f42)) {
                if ((rb_state7.var_2) != (0x0)) {
                    if (!((rb_state7.var_2) == (rb_state7.var_25))) {
                        racebench_trigger(7);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) - (0x26cc5545);
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