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
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xe1429964)) {
            rb_state7.var_3 = rb_state7.var_1;
        }
        if ((rb_state7.var_0) == (0xe1429964)) {
            if (!((rb_state7.var_1) == (rb_state7.var_3))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (0xe6455aa6);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - ((0xf13952db) + (rb_state16.var_0));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) - (0x11969a23);
        #endif
        SUM = 0.0;
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (0xddf05775);
        #endif
        R1 = 0.0;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + (0xbce2dc6a);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - ((0x1c925c0c) - (0x25262bc9));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
        #endif
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (((rb_state1.var_1) == (0xbce2dc6a)) ? (rb_state1.var_1) : (0xd13101ae));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) + (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_1) : (0xa673edf9));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x3a4473b0));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_2 = (rb_state11.var_2) + ((0xcd2e7a23) ^ (rb_state11.var_0));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) ^ (0xeb7377e8);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - ((0x7dfac75c) ^ (rb_state15.var_0));
            #endif
            temp_p = VAR[mol].F[DISP][dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) ^ (0xf3d801c5);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + (3109487 < rb_input_size ? (uint32_t)rb_input[3109487] : 0x2e2cf07);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_2);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_1) == (0x99c9aa44)) {
                pthread_mutex_lock(&(rb_state11.lock_28));
                rb_state11.var_5 = 0x67a6e7ce;
                rb_state11.var_6 = (rb_state11.var_6) - (((((0x854ed520) - (rb_state11.var_6)) + (rb_state11.var_5)) - (rb_state11.var_3)) - (rb_state11.var_4));
                rb_state11.var_7 = (rb_state11.var_7) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_7) : (0x21d20595));
                rb_state11.var_8 = (rb_state11.var_8) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_5) : (0x53525d73));
                rb_state11.var_9 = (rb_state11.var_9) - (((0x3cfdabd1) + (0xc4bed6d0)) + (rb_state11.var_6));
                rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_7);
                rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_8) : (0x24acc7c3));
                rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_9) : (0x32bce0fa));
                rb_state11.var_13 = (rb_state11.var_13) ^ (((0x230f770) - (0x2f5360b3)) - (rb_state11.var_10));
                rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_11);
                rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_12);
                rb_state11.var_16 = (rb_state11.var_16) + (((0xca7b7353) + (0x4e08dadb)) + (rb_state11.var_13));
                rb_state11.var_17 = (rb_state11.var_17) - (((0xff312382) - (rb_state11.var_14)) ^ (rb_state11.var_11));
                rb_state11.var_18 = (rb_state11.var_18) - (((((0x420de89e) - (rb_state11.var_16)) - (0x6c88caed)) + (rb_state11.var_15)) ^ (rb_state11.var_12));
                rb_state11.var_19 = (rb_state11.var_19) - (rb_state11.var_17);
                rb_state11.var_20 = (rb_state11.var_20) + (((((0xcba0662e) - (rb_state11.var_14)) ^ (rb_state11.var_18)) + (rb_state11.var_13)) + (rb_state11.var_19));
                rb_state11.var_4 = (rb_state11.var_4) - (rb_state11.var_20);
                rb_state11.var_3 = rb_state11.var_4;
                pthread_mutex_unlock(&(rb_state11.lock_28));
            }
            if ((rb_state11.var_1) == (0x99c9aa44)) {
                pthread_mutex_lock(&(rb_state11.lock_28));
                if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                    racebench_trigger(11);
                }
                pthread_mutex_unlock(&(rb_state11.lock_28));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_1);
            #endif
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (3338647 < rb_input_size ? (uint32_t)rb_input[3338647] : 0xce7b6e00);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_0);
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xb802f5ce)) {
                rb_state9.var_3 = 0xfa34612b;
                rb_state9.var_4 = 0xc75a19f;
                rb_state9.var_5 = (rb_state9.var_5) ^ (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0xa183a8ad));
                rb_state9.var_6 = (rb_state9.var_6) - (((0xe8bd1456) + (rb_state9.var_3)) ^ (0xd3e8e9c3));
                rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_4);
                rb_state9.var_8 = (rb_state9.var_8) ^ (((((0x707221fa) - (rb_state9.var_5)) ^ (rb_state9.var_2)) + (0xfddd8755)) ^ (rb_state9.var_5));
                rb_state9.var_9 = (rb_state9.var_9) ^ (((0x718d8afc) + (rb_state9.var_6)) ^ (rb_state9.var_6));
                rb_state9.var_10 = (rb_state9.var_10) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0x30dc257a));
                rb_state9.var_11 = (rb_state9.var_11) ^ (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_10) : (0xc6409f74));
                rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_11);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_2) : (0x3cd3f623));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_1);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_2 = (rb_state18.var_2) - (0xad1fb726);
            if ((rb_state18.var_2) == (0x179ea3c7)) {
                pthread_mutex_lock(&(rb_state18.lock_24));
                rb_state18.var_21 = 0x96b8bee8;
                rb_state18.var_22 = (rb_state18.var_22) - (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_15) : (0x291febf0));
                rb_state18.var_23 = (rb_state18.var_23) ^ (((((0xf25debeb) - (0x360b2eff)) - (rb_state18.var_16)) - (rb_state18.var_22)) + (rb_state18.var_14));
                rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_23);
                pthread_mutex_unlock(&(rb_state18.lock_24));
            }
            #endif
            VR1[dir] = temp_p[O] - temp_p[H1];
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (0x9adcb4e);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - ((0x2e6b9ee7) + (0x8c82ccf));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0xb8a9e141));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (0x9d5c7fcb);
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_1) == (0x8f857590)) {
                pthread_mutex_lock(&(rb_state15.lock_15));
                rb_state15.var_7 = 0xf5b0cd4e;
                rb_state15.var_8 = 0xfbdc6df6;
                rb_state15.var_9 = (rb_state15.var_9) - (0xdb7daa27);
                rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0xd5330d8d));
                rb_state15.var_11 = (rb_state15.var_11) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_6) : (0x15487ac8));
                rb_state15.var_12 = (rb_state15.var_12) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_9) : (0x9e044c08));
                rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0xfe0fe711));
                rb_state15.var_14 = (rb_state15.var_14) + (((((0x4fd7d248) - (0x1e36e2ca)) + (rb_state15.var_12)) + (rb_state15.var_9)) ^ (rb_state15.var_13));
                rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_14);
                pthread_mutex_unlock(&(rb_state15.lock_15));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_1);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
            #endif
            R1 += VR1[dir] * VR1[dir];
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) ^ (3679809 < rb_input_size ? (uint32_t)rb_input[3679809] : 0xebb5d874);
            if ((rb_state3.var_1) == (0x3b3a)) {
                rb_state3.var_3 = rb_state3.var_2;
            }
            if ((rb_state3.var_1) == (0x3b3a)) {
                if (!((rb_state3.var_2) == (rb_state3.var_3))) {
                    racebench_trigger(3);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ (0x318e60ed);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) + (0x33735447);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (606389 < rb_input_size ? (uint32_t)rb_input[606389] : 0xa8758530);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) + (0xa6a8a41e);
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0xffff6429)) {
                pthread_mutex_lock(&(rb_state12.lock_25));
                rb_state12.var_8 = 0x121b7f1a;
                rb_state12.var_9 = 0xbe446e1b;
                rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_9) : (0x49dade9a));
                rb_state12.var_11 = (rb_state12.var_11) ^ (rb_state12.var_4);
                rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_8) : (0x6988847e));
                rb_state12.var_13 = (rb_state12.var_13) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_10) : (0x1c13481b));
                rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_11);
                rb_state12.var_15 = (rb_state12.var_15) - (((0x3cda6768) + (0x9ea65dcd)) + (rb_state12.var_12));
                rb_state12.var_16 = (rb_state12.var_16) - (((((0x60bf8e11) - (0x9bbb43c1)) ^ (rb_state12.var_13)) ^ (rb_state12.var_14)) ^ (rb_state12.var_7));
                rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0x329622e6));
                rb_state12.var_7 = (rb_state12.var_7) + (rb_state12.var_17);
                rb_state12.var_2 = rb_state12.var_7;
                pthread_mutex_unlock(&(rb_state12.lock_25));
            }
            if ((rb_state12.var_0) == (0xffff6456)) {
                pthread_mutex_lock(&(rb_state12.lock_25));
                rb_state12.var_19 = 0xc262703d;
                rb_state12.var_20 = 0x52ff0a35;
                rb_state12.var_21 = (rb_state12.var_21) ^ (rb_state12.var_9);
                rb_state12.var_22 = (rb_state12.var_22) + (((rb_state12.var_19) == (0x0)) ? (rb_state12.var_20) : (0xd957be3a));
                rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_21) : (0xc3c0cb6c));
                rb_state12.var_24 = (rb_state12.var_24) + (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_23) : (0xd36e0ce0));
                rb_state12.var_18 = (rb_state12.var_18) ^ (rb_state12.var_24);
                rb_state12.var_2 = rb_state12.var_18;
                pthread_mutex_unlock(&(rb_state12.lock_25));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_2) == (0x996c7f4d)) {
                rb_state19.var_28 = 0x3625aac9;
                rb_state19.var_29 = (rb_state19.var_29) + (((((0xe29fb0c3) ^ (0x25968ab5)) ^ (rb_state19.var_18)) ^ (0x4076710b)) ^ (rb_state19.var_19));
                rb_state19.var_30 = (rb_state19.var_30) + (((rb_state19.var_20) == (0x0)) ? (rb_state19.var_28) : (0xb2e6ccf4));
                rb_state19.var_31 = (rb_state19.var_31) - (((((0x4ed2c3d7) - (0x922e8be2)) - (rb_state19.var_30)) + (rb_state19.var_21)) ^ (rb_state19.var_29));
                rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_31);
            }
            #endif
            VR2[dir] = temp_p[O] - temp_p[H2];
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_6) == (0x4d7435e9)) {
                rb_state1.var_20 = 0x3cfd348c;
                rb_state1.var_21 = (rb_state1.var_21) - (rb_state1.var_16);
                rb_state1.var_22 = (rb_state1.var_22) + (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_15) : (0x83bb50a6));
                rb_state1.var_23 = (rb_state1.var_23) ^ (3501669 < rb_input_size ? (uint32_t)rb_input[3501669] : 0xb695aae2);
                rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_14);
                rb_state1.var_25 = (rb_state1.var_25) + (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_20) : (0x2583bef0));
                rb_state1.var_26 = (rb_state1.var_26) ^ (((((0xcc95aac2) + (0x9918ae82)) + (rb_state1.var_19)) ^ (rb_state1.var_21)) ^ (rb_state1.var_22));
                rb_state1.var_27 = (rb_state1.var_27) ^ (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_23) : (0x97960f09));
                rb_state1.var_28 = (rb_state1.var_28) - (rb_state1.var_24);
                rb_state1.var_29 = (rb_state1.var_29) - (((rb_state1.var_25) == (0x0)) ? (rb_state1.var_26) : (0x2e95abc6));
                rb_state1.var_30 = (rb_state1.var_30) + (rb_state1.var_27);
                rb_state1.var_31 = (rb_state1.var_31) ^ (((0xf7052495) + (0x87bd3313)) + (rb_state1.var_28));
                rb_state1.var_32 = (rb_state1.var_32) - (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_30) : (0x8ee623d2));
                rb_state1.var_33 = (rb_state1.var_33) - (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_31) : (0x1208761b));
                rb_state1.var_34 = (rb_state1.var_34) + (((rb_state1.var_22) == (0x0)) ? (rb_state1.var_32) : (0xe0b5c926));
                rb_state1.var_35 = (rb_state1.var_35) - (((0x166db7fc) + (rb_state1.var_33)) ^ (0xce591f3c));
                rb_state1.var_36 = (rb_state1.var_36) + (rb_state1.var_34);
                rb_state1.var_37 = (rb_state1.var_37) + (((0x55642d90) + (rb_state1.var_23)) - (rb_state1.var_35));
                rb_state1.var_38 = (rb_state1.var_38) - (((0x369992ab) - (rb_state1.var_36)) + (0x56632794));
                rb_state1.var_39 = (rb_state1.var_39) ^ (((rb_state1.var_37) == (0x0)) ? (rb_state1.var_38) : (0xc10a3765));
                rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_39);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_1);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (1513708 < rb_input_size ? (uint32_t)rb_input[1513708] : 0xb941a15b);
            if ((rb_state12.var_0) == (0xffff6456)) {
                pthread_mutex_lock(&(rb_state12.lock_25));
                if ((rb_state12.var_2) != (0x0)) {
                    if (!((rb_state12.var_2) == (rb_state12.var_18))) {
                        racebench_trigger(12);
                    }
                }
                pthread_mutex_unlock(&(rb_state12.lock_25));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) - ((0x7cb28d34) ^ (rb_state13.var_0));
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0x33c9db68)) {
                rb_state16.var_3 = rb_state16.var_2;
            }
            if ((rb_state16.var_1) == (0x33c9db68)) {
                if (!((rb_state16.var_2) == (rb_state16.var_3))) {
                    racebench_trigger(16);
                }
            }
            #endif
            R2 += VR2[dir] * VR2[dir];
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0xf37ab6b7));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0xa25a0bb0)) ? (rb_state12.var_1) : (0x5029cd8a));
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0x50bcfc3c)) {
                pthread_mutex_lock(&(rb_state13.lock_40));
                rb_state13.var_5 = 0xe7f0e6f2;
                rb_state13.var_6 = (rb_state13.var_6) ^ (502905 < rb_input_size ? (uint32_t)rb_input[502905] : 0xe6363afd);
                rb_state13.var_7 = (rb_state13.var_7) ^ (((0x118247f1) + (rb_state13.var_3)) + (0x1ec320f1));
                rb_state13.var_8 = (rb_state13.var_8) - (rb_state13.var_5);
                rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_5) : (0x10ab59a6));
                rb_state13.var_10 = (rb_state13.var_10) - (((((0xeda701f3) + (rb_state13.var_6)) + (0xd249e705)) - (0x54bebd1)) ^ (rb_state13.var_4));
                rb_state13.var_11 = (rb_state13.var_11) ^ (rb_state13.var_7);
                rb_state13.var_12 = (rb_state13.var_12) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0x3a601b16));
                rb_state13.var_13 = (rb_state13.var_13) + (((((0x6f04118c) - (rb_state13.var_10)) ^ (rb_state13.var_9)) + (rb_state13.var_8)) + (0x84387789));
                rb_state13.var_14 = (rb_state13.var_14) ^ (((0x46362ee6) + (rb_state13.var_11)) + (rb_state13.var_9));
                rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_12);
                rb_state13.var_16 = (rb_state13.var_16) ^ (rb_state13.var_13);
                rb_state13.var_17 = (rb_state13.var_17) + (((0xe9783a2) - (rb_state13.var_14)) ^ (rb_state13.var_10));
                rb_state13.var_18 = (rb_state13.var_18) ^ (((0x30405c72) + (0x79099cdf)) ^ (rb_state13.var_15));
                rb_state13.var_19 = (rb_state13.var_19) ^ (rb_state13.var_16);
                rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_17);
                rb_state13.var_21 = (rb_state13.var_21) - (((0x1248efa4) ^ (0xc0624285)) ^ (rb_state13.var_18));
                rb_state13.var_22 = (rb_state13.var_22) + (rb_state13.var_19);
                rb_state13.var_23 = (rb_state13.var_23) - (((0x494e7af1) + (rb_state13.var_11)) + (rb_state13.var_20));
                rb_state13.var_24 = (rb_state13.var_24) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_22) : (0x9fb46217));
                rb_state13.var_25 = (rb_state13.var_25) - (((0xa995512d) ^ (rb_state13.var_23)) - (0x75306132));
                rb_state13.var_26 = (rb_state13.var_26) ^ (((0x4971065a) + (rb_state13.var_24)) - (rb_state13.var_12));
                rb_state13.var_27 = (rb_state13.var_27) ^ (((0x6ff0a899) + (rb_state13.var_25)) ^ (0xee094c1c));
                rb_state13.var_28 = (rb_state13.var_28) ^ (((((0xcedaa299) - (rb_state13.var_13)) + (0x47d3c63c)) + (rb_state13.var_27)) + (rb_state13.var_26));
                rb_state13.var_4 = (rb_state13.var_4) - (rb_state13.var_28);
                rb_state13.var_3 = rb_state13.var_4;
                pthread_mutex_unlock(&(rb_state13.lock_40));
            }
            if ((rb_state13.var_1) == (0x50bcfc3c)) {
                pthread_mutex_lock(&(rb_state13.lock_40));
                if (!((rb_state13.var_3) == (rb_state13.var_4))) {
                    racebench_trigger(13);
                }
                pthread_mutex_unlock(&(rb_state13.lock_40));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (3553513 < rb_input_size ? (uint32_t)rb_input[3553513] : 0x83710922);
            if ((rb_state14.var_1) == (0xcaaa782a)) {
                pthread_mutex_lock(&(rb_state14.lock_9));
                rb_state14.var_3 = 0xa8a7d143;
                rb_state14.var_4 = 0xcef2cceb;
                rb_state14.var_5 = 0x376ca140;
                rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_5) : (0x2355bd3a));
                rb_state14.var_7 = (rb_state14.var_7) + (((((0x6d524f37) + (rb_state14.var_4)) - (rb_state14.var_3)) + (rb_state14.var_3)) ^ (rb_state14.var_4));
                rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_7) : (0x4e2b3fcd));
                rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_8);
                pthread_mutex_unlock(&(rb_state14.lock_9));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) + (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0x9add60ce));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (2427676 < rb_input_size ? (uint32_t)rb_input[2427676] : 0x2da0fd22);
            #endif
            SUM += VR1[dir] * VR2[dir];
        }

        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (0x1d68fbdc);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ (0x4d557bc9);
        #endif
        R1 = sqrt(R1);
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_2) == (0x481daa99)) {
            pthread_mutex_lock(&(rb_state0.lock_23));
            rb_state0.var_5 = 0x987a5353;
            rb_state0.var_6 = (rb_state0.var_6) + (rb_state0.var_3);
            rb_state0.var_7 = (rb_state0.var_7) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x89d08422));
            rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_6) : (0xac3dce3f));
            rb_state0.var_9 = (rb_state0.var_9) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_6) : (0xdd71c70e));
            rb_state0.var_10 = (rb_state0.var_10) ^ (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_7) : (0xe3b7916b));
            rb_state0.var_11 = (rb_state0.var_11) ^ (((((0x829f3466) - (0x719e74f1)) ^ (rb_state0.var_9)) + (rb_state0.var_9)) + (rb_state0.var_8));
            rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_10) : (0xe154797f));
            rb_state0.var_13 = (rb_state0.var_13) ^ (rb_state0.var_11);
            rb_state0.var_14 = (rb_state0.var_14) ^ (((0xdd952a01) + (rb_state0.var_11)) + (rb_state0.var_12));
            rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_13);
            rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_14);
            rb_state0.var_17 = (rb_state0.var_17) - (((((0x8933a789) + (rb_state0.var_15)) ^ (rb_state0.var_16)) - (rb_state0.var_12)) + (rb_state0.var_13));
            rb_state0.var_4 = (rb_state0.var_4) + (rb_state0.var_17);
            rb_state0.var_3 = rb_state0.var_4;
            pthread_mutex_unlock(&(rb_state0.lock_23));
        }
        if ((rb_state0.var_2) == (0x481daa99)) {
            pthread_mutex_lock(&(rb_state0.lock_23));
            if (!((rb_state0.var_3) == (rb_state0.var_4))) {
                racebench_trigger(0);
            }
            pthread_mutex_unlock(&(rb_state0.lock_23));
        }
        #endif
        R2 = sqrt(R2);

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_9) : (0x303d4f40));
        rb_state1.var_5 = (rb_state1.var_5) + (2624507 < rb_input_size ? (uint32_t)rb_input[2624507] : 0xecc79505);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (2628575 < rb_input_size ? (uint32_t)rb_input[2628575] : 0xf7fa0185);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (3127997 < rb_input_size ? (uint32_t)rb_input[3127997] : 0x84712c73);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_2) == (0x996c7f4d)) {
            pthread_mutex_lock(&(rb_state19.lock_32));
            rb_state19.var_17 = 0x22bbeb48;
            rb_state19.var_18 = 0x6eceb97a;
            rb_state19.var_19 = 0x7b01c14d;
            rb_state19.var_20 = (rb_state19.var_20) + (((0x4a3e6400) ^ (rb_state19.var_17)) - (rb_state19.var_15));
            rb_state19.var_21 = (rb_state19.var_21) ^ (rb_state19.var_19);
            rb_state19.var_22 = (rb_state19.var_22) - (((0x829db0ec) + (rb_state19.var_18)) - (0x47e55dd8));
            rb_state19.var_23 = (rb_state19.var_23) - (((0xaaf67e66) ^ (rb_state19.var_20)) ^ (rb_state19.var_16));
            rb_state19.var_24 = (rb_state19.var_24) + (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_22) : (0x32b475b1));
            rb_state19.var_25 = (rb_state19.var_25) ^ (rb_state19.var_23);
            rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_24) : (0xfb6123fb));
            rb_state19.var_27 = (rb_state19.var_27) ^ (((rb_state19.var_25) == (0x0)) ? (rb_state19.var_26) : (0xed0535f7));
            rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_27);
            rb_state19.var_15 = rb_state19.var_16;
            pthread_mutex_unlock(&(rb_state19.lock_32));
        }
        if ((rb_state19.var_2) == (0x996c7f4d)) {
            if (!((rb_state19.var_15) == (rb_state19.var_16))) {
                racebench_trigger(19);
            }
        }
        #endif
        COS = SUM / (R1 * R2);
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_1) == (0xec3ce5ad)) {
            if ((rb_state4.var_2) != (0x0)) {
                if (!((rb_state4.var_2) == (rb_state4.var_15))) {
                    racebench_trigger(4);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (0xe72aa035);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_1) : (0xefd128b0));
        #endif
        SIN = sqrt(ONE - COS * COS);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (2238377 < rb_input_size ? (uint32_t)rb_input[2238377] : 0x761f621b);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (0xc7bdb8b3);
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_2) == (0xde41abbe)) {
            rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_8);
            rb_state10.var_14 = (rb_state10.var_14) ^ (((0x34f81211) - (rb_state10.var_6)) + (rb_state10.var_9));
            rb_state10.var_15 = (rb_state10.var_15) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_5) : (0xbdd1aed9));
            rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_10) : (0xdd772c3f));
            rb_state10.var_17 = (rb_state10.var_17) ^ (((0xa2298454) - (rb_state10.var_12)) ^ (rb_state10.var_13));
            rb_state10.var_18 = (rb_state10.var_18) ^ (((((0x649f2f50) + (rb_state10.var_15)) - (0x964dd5ea)) - (rb_state10.var_14)) - (rb_state10.var_13));
            rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_16) : (0xb59cd956));
            rb_state10.var_20 = (rb_state10.var_20) - (rb_state10.var_17);
            rb_state10.var_21 = (rb_state10.var_21) - (rb_state10.var_18);
            rb_state10.var_22 = (rb_state10.var_22) + (((0x9f5f8db4) ^ (rb_state10.var_15)) ^ (rb_state10.var_19));
            rb_state10.var_23 = (rb_state10.var_23) - (((((0x6e31d345) ^ (rb_state10.var_16)) - (rb_state10.var_20)) ^ (rb_state10.var_21)) + (0xafe8fb9e));
            rb_state10.var_24 = (rb_state10.var_24) + (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_23) : (0x7cba88c7));
            rb_state10.var_12 = (rb_state10.var_12) ^ (rb_state10.var_24);
            rb_state10.var_3 = rb_state10.var_12;
        }
        if ((rb_state10.var_2) == (0xde41abbe)) {
            pthread_mutex_lock(&(rb_state10.lock_46));
            rb_state10.var_26 = 0xb85b50c5;
            rb_state10.var_27 = (rb_state10.var_27) ^ (rb_state10.var_19);
            rb_state10.var_28 = (rb_state10.var_28) ^ (rb_state10.var_20);
            rb_state10.var_29 = (rb_state10.var_29) - (0xf7a750cc);
            rb_state10.var_30 = (rb_state10.var_30) - (rb_state10.var_26);
            rb_state10.var_31 = (rb_state10.var_31) - (rb_state10.var_18);
            rb_state10.var_32 = (rb_state10.var_32) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_27) : (0xdb4a669b));
            rb_state10.var_33 = (rb_state10.var_33) + (((((0xfbc89b80) - (0xb5768595)) - (rb_state10.var_28)) + (rb_state10.var_29)) - (0xbcea39da));
            rb_state10.var_34 = (rb_state10.var_34) - (((0x169d9250) - (rb_state10.var_21)) + (rb_state10.var_30));
            rb_state10.var_35 = (rb_state10.var_35) + (rb_state10.var_31);
            rb_state10.var_36 = (rb_state10.var_36) - (((0x3b20cccf) - (rb_state10.var_22)) + (rb_state10.var_32));
            rb_state10.var_37 = (rb_state10.var_37) + (((0xbefd80f5) ^ (rb_state10.var_33)) - (rb_state10.var_23));
            rb_state10.var_38 = (rb_state10.var_38) - (((0x5486a579) + (0x5ecd20cf)) + (rb_state10.var_34));
            rb_state10.var_39 = (rb_state10.var_39) + (((rb_state10.var_35) == (0x0)) ? (rb_state10.var_36) : (0x8f23f972));
            rb_state10.var_40 = (rb_state10.var_40) ^ (((rb_state10.var_24) == (0x0)) ? (rb_state10.var_37) : (0x630976a));
            rb_state10.var_41 = (rb_state10.var_41) ^ (rb_state10.var_38);
            rb_state10.var_42 = (rb_state10.var_42) ^ (((((0x14994a53) ^ (rb_state10.var_39)) ^ (rb_state10.var_40)) + (rb_state10.var_25)) - (0x67dc63c2));
            rb_state10.var_43 = (rb_state10.var_43) - (rb_state10.var_41);
            rb_state10.var_44 = (rb_state10.var_44) + (((0x7dc406e5) ^ (0x6ca25070)) - (rb_state10.var_42));
            rb_state10.var_45 = (rb_state10.var_45) + (((rb_state10.var_43) == (0x0)) ? (rb_state10.var_44) : (0x56ec47b5));
            rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_45);
            rb_state10.var_3 = rb_state10.var_25;
            pthread_mutex_unlock(&(rb_state10.lock_46));
        }
        #endif
        DT = (acos(COS) - ANGLE) * ROH;
        DTS = DT * DT;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_1) == (0x63455662)) {
            pthread_mutex_lock(&(rb_state1.lock_10));
            rb_state1.var_4 = (rb_state1.var_4) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_2) : (0x1a187cb2));
            rb_state1.var_5 = (rb_state1.var_5) + ((0xcdb1c833) ^ (0x78a23b6a));
            rb_state1.var_6 = (rb_state1.var_6) ^ ((0xd6db1f80) - (rb_state1.var_5));
            rb_state1.var_7 = (rb_state1.var_7) + (((((0x2b2f0509) ^ (rb_state1.var_3)) - (rb_state1.var_4)) ^ (rb_state1.var_6)) + (rb_state1.var_7));
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_5) == (0x20)) ? (rb_state1.var_6) : (0x6cac2a18));
            rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0xb30e9873));
            rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_9);
            rb_state1.var_3 = rb_state1.var_2;
            pthread_mutex_unlock(&(rb_state1.lock_10));
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (22647 < rb_input_size ? (uint32_t)rb_input[22647] : 0xefb9b560);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x8206c75c)) ? (rb_state15.var_0) : (0x4dfd66e8));
        #endif
        DR1 = R1 - ROH;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (1191442 < rb_input_size ? (uint32_t)rb_input[1191442] : 0x81e6bb2b);
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xe1429964)) {
            rb_state7.var_4 = 0xf397c8e0;
            rb_state7.var_5 = 0x3a241167;
            rb_state7.var_6 = 0x7ea11027;
            rb_state7.var_7 = (rb_state7.var_7) + (((((0x1eb1dfce) ^ (rb_state7.var_6)) - (0xa471651b)) + (rb_state7.var_4)) - (0x4f42218b));
            rb_state7.var_8 = (rb_state7.var_8) + (((((0xc57338ac) - (rb_state7.var_5)) + (0xdd398433)) ^ (rb_state7.var_7)) - (0x42f3fcc5));
            rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_1);
        #endif
        DR1S = DR1 * DR1;
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_1) == (0x8f857590)) {
            pthread_mutex_lock(&(rb_state15.lock_15));
            rb_state15.var_4 = 0xec145db8;
            rb_state15.var_5 = (rb_state15.var_5) + (((((0x5e9bad15) ^ (rb_state15.var_5)) ^ (rb_state15.var_3)) + (rb_state15.var_4)) + (rb_state15.var_4));
            rb_state15.var_6 = (rb_state15.var_6) + (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_5) : (0x6888edd3));
            rb_state15.var_3 = (rb_state15.var_3) - (rb_state15.var_6);
            rb_state15.var_2 = rb_state15.var_3;
            pthread_mutex_unlock(&(rb_state15.lock_15));
        }
        if ((rb_state15.var_1) == (0x8f857590)) {
            pthread_mutex_lock(&(rb_state15.lock_15));
            if (!((rb_state15.var_2) == (rb_state15.var_3))) {
                racebench_trigger(15);
            }
            pthread_mutex_unlock(&(rb_state15.lock_15));
        }
        #endif
        DR2 = R2 - ROH;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + (2552556 < rb_input_size ? (uint32_t)rb_input[2552556] : 0x67341462);
        #endif
        DR2S = DR2 * DR2;

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (3329813 < rb_input_size ? (uint32_t)rb_input[3329813] : 0x6e129de9);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (0x7654bb85);
        #endif
        R1S = ROH / (R1 * SIN);
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0x3b3a)) {
            rb_state3.var_4 = (rb_state3.var_4) ^ ((0xea4cf2fb) + (0xfdde10c9));
            rb_state3.var_5 = (rb_state3.var_5) + (rb_state3.var_3);
            rb_state3.var_6 = (rb_state3.var_6) + (((((0x28e13606) + (0xdb9fca84)) ^ (rb_state3.var_4)) + (rb_state3.var_1)) ^ (0x5939bf1f));
            rb_state3.var_7 = (rb_state3.var_7) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0xcfab7c24));
            rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_7);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (0x685cbcf);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x1e2d)) {
            rb_state17.var_12 = 0x1315c9a0;
            rb_state17.var_13 = 0xa5c7bf9c;
            rb_state17.var_14 = (rb_state17.var_14) + (587772 < rb_input_size ? (uint32_t)rb_input[587772] : 0xf7b813e3);
            rb_state17.var_15 = (rb_state17.var_15) - (rb_state17.var_6);
            rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_13) : (0x857c57b3));
            rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_14) : (0xa4971ab6));
            rb_state17.var_18 = (rb_state17.var_18) - (((0xa5c939b3) - (rb_state17.var_7)) ^ (rb_state17.var_15));
            rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_16) : (0x19858d58));
            rb_state17.var_20 = (rb_state17.var_20) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_18) : (0xc58fb3f2));
            rb_state17.var_21 = (rb_state17.var_21) ^ (((((0x313690e5) ^ (0xcaecc33a)) - (rb_state17.var_20)) + (rb_state17.var_19)) - (rb_state17.var_9));
            rb_state17.var_11 = (rb_state17.var_11) ^ (rb_state17.var_21);
            rb_state17.var_3 = rb_state17.var_11;
        }
        if ((rb_state17.var_0) == (0x1e2d)) {
            pthread_mutex_lock(&(rb_state17.lock_31));
            rb_state17.var_23 = (rb_state17.var_23) + ((0x93ae2be7) ^ (0x54dc24e9));
            rb_state17.var_24 = (rb_state17.var_24) + (832271 < rb_input_size ? (uint32_t)rb_input[832271] : 0xa574d7e5);
            rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_12) : (0x5635ff11));
            rb_state17.var_26 = (rb_state17.var_26) + (((((0x5c15ad13) ^ (rb_state17.var_23)) - (rb_state17.var_10)) - (rb_state17.var_14)) + (rb_state17.var_13));
            rb_state17.var_27 = (rb_state17.var_27) + (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_24) : (0xa351e71f));
            rb_state17.var_28 = (rb_state17.var_28) - (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_26) : (0x9510f577));
            rb_state17.var_29 = (rb_state17.var_29) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_27) : (0x75a9c393));
            rb_state17.var_30 = (rb_state17.var_30) + (((rb_state17.var_28) == (0x0)) ? (rb_state17.var_29) : (0x89bc7a84));
            rb_state17.var_22 = (rb_state17.var_22) + (rb_state17.var_30);
            rb_state17.var_3 = rb_state17.var_22;
            pthread_mutex_unlock(&(rb_state17.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (0x30e8097b);
        if ((rb_state19.var_1) == (0x753a66fa)) {
            pthread_mutex_lock(&(rb_state19.lock_13));
            rb_state19.var_5 = 0x751dc65e;
            rb_state19.var_6 = 0xdaa4b61f;
            rb_state19.var_7 = 0xa2b5a02d;
            rb_state19.var_8 = (rb_state19.var_8) + (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_7) : (0xcda41c59));
            rb_state19.var_9 = (rb_state19.var_9) - (rb_state19.var_5);
            rb_state19.var_10 = (rb_state19.var_10) ^ (((((0x4d4dae3b) ^ (rb_state19.var_8)) ^ (rb_state19.var_6)) - (0x708b4d50)) ^ (rb_state19.var_4));
            rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_9) : (0xf1860f95));
            rb_state19.var_12 = (rb_state19.var_12) ^ (((((0xfd75e862) + (rb_state19.var_11)) + (rb_state19.var_6)) + (0xb985fd6f)) ^ (rb_state19.var_10));
            rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_12);
            rb_state19.var_4 = rb_state19.var_3;
            pthread_mutex_unlock(&(rb_state19.lock_13));
        }
        #endif
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x65)) ? (rb_state4.var_0) : (0x4ee3273c));
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0xfe93be78)) {
                rb_state6.var_3 = 0x464e6e9d;
                rb_state6.var_4 = 0x7bccfdfa;
                rb_state6.var_5 = (rb_state6.var_5) + (((((0x56cae1f3) + (rb_state6.var_2)) ^ (rb_state6.var_4)) + (0x70d89aaf)) + (rb_state6.var_3));
                rb_state6.var_6 = (rb_state6.var_6) + (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_5) : (0xce2e5a03));
                rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_6);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_2) == (0xde41abbe)) {
                if ((rb_state10.var_3) != (0x0)) {
                    if (!((rb_state10.var_3) == (rb_state10.var_25))) {
                        racebench_trigger(10);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) - ((0xd9f7a978) ^ (0x64236d81));
            #endif
            DR11[dir] = VR1[dir] / R1;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) - (103776 < rb_input_size ? (uint32_t)rb_input[103776] : 0x974f94f3);
            if ((rb_state5.var_2) == (0xeb24e0b4)) {
                rb_state5.var_21 = 0x8351b5ac;
                rb_state5.var_22 = 0x9ff63e3b;
                rb_state5.var_23 = (rb_state5.var_23) ^ (880756 < rb_input_size ? (uint32_t)rb_input[880756] : 0xedd3cda3);
                rb_state5.var_24 = (rb_state5.var_24) - (1668495 < rb_input_size ? (uint32_t)rb_input[1668495] : 0x17bbb135);
                rb_state5.var_25 = (rb_state5.var_25) - (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_8) : (0x110461e7));
                rb_state5.var_26 = (rb_state5.var_26) - (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_21) : (0x734b430e));
                rb_state5.var_27 = (rb_state5.var_27) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_22) : (0xcc43c099));
                rb_state5.var_28 = (rb_state5.var_28) - (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_23) : (0x6b7f3a0f));
                rb_state5.var_29 = (rb_state5.var_29) - (rb_state5.var_24);
                rb_state5.var_30 = (rb_state5.var_30) + (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_25) : (0x67b4f1ae));
                rb_state5.var_31 = (rb_state5.var_31) ^ (((((0x6cb9ef58) + (rb_state5.var_14)) + (0x800cf3ab)) + (rb_state5.var_26)) ^ (rb_state5.var_27));
                rb_state5.var_32 = (rb_state5.var_32) ^ (rb_state5.var_28);
                rb_state5.var_33 = (rb_state5.var_33) + (((((0x4a8f0332) + (rb_state5.var_29)) + (0xc99d4f71)) - (rb_state5.var_15)) - (rb_state5.var_30));
                rb_state5.var_34 = (rb_state5.var_34) + (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_31) : (0xa35982d));
                rb_state5.var_35 = (rb_state5.var_35) + (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_32) : (0x190358cf));
                rb_state5.var_36 = (rb_state5.var_36) ^ (((((0x2144d300) - (rb_state5.var_18)) ^ (rb_state5.var_33)) + (0x1e67e1c0)) ^ (rb_state5.var_34));
                rb_state5.var_37 = (rb_state5.var_37) + (((0x58446f11) + (0xcc06133)) - (rb_state5.var_35));
                rb_state5.var_38 = (rb_state5.var_38) - (rb_state5.var_36);
                rb_state5.var_39 = (rb_state5.var_39) - (((0xa2313fa1) ^ (rb_state5.var_37)) + (0xe6fd7847));
                rb_state5.var_40 = (rb_state5.var_40) ^ (((rb_state5.var_19) == (0x0)) ? (rb_state5.var_38) : (0xe3fcde32));
                rb_state5.var_41 = (rb_state5.var_41) + (((((0xd8b4df8f) + (rb_state5.var_40)) ^ (rb_state5.var_39)) + (0x96b12bfa)) - (0xec2734f4));
                rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_41);
                rb_state5.var_3 = rb_state5.var_20;
            }
            if ((rb_state5.var_2) == (0xc15d6cb4)) {
                pthread_mutex_lock(&(rb_state5.lock_47));
                rb_state5.var_43 = 0x5dcb2880;
                rb_state5.var_44 = (rb_state5.var_44) + (((((0xce0aed9d) - (rb_state5.var_23)) - (rb_state5.var_22)) ^ (rb_state5.var_43)) + (rb_state5.var_21));
                rb_state5.var_45 = (rb_state5.var_45) - (rb_state5.var_20);
                rb_state5.var_46 = (rb_state5.var_46) - (((((0xf97163a1) - (rb_state5.var_45)) + (rb_state5.var_44)) - (0xdb5ea92)) + (0x28581e9d));
                rb_state5.var_42 = (rb_state5.var_42) ^ (rb_state5.var_46);
                rb_state5.var_3 = rb_state5.var_42;
                pthread_mutex_unlock(&(rb_state5.lock_47));
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (0x9fef4a5);
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0x50bcfc3c)) {
                rb_state13.var_29 = 0xf8d3a2cb;
                rb_state13.var_30 = (rb_state13.var_30) + (((((0xf863ad72) ^ (rb_state13.var_16)) + (rb_state13.var_14)) ^ (0x6e648fd6)) + (rb_state13.var_29));
                rb_state13.var_31 = (rb_state13.var_31) - (0xe8f04c15);
                rb_state13.var_32 = (rb_state13.var_32) ^ (((0x658f592a) + (0xa509c7d9)) ^ (rb_state13.var_15));
                rb_state13.var_33 = (rb_state13.var_33) ^ (((0xcfe5125f) - (0x47f067d9)) + (rb_state13.var_30));
                rb_state13.var_34 = (rb_state13.var_34) ^ (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_31) : (0xe4434abe));
                rb_state13.var_35 = (rb_state13.var_35) + (((rb_state13.var_32) == (0x0)) ? (rb_state13.var_33) : (0x85745f2f));
                rb_state13.var_36 = (rb_state13.var_36) ^ (rb_state13.var_34);
                rb_state13.var_37 = (rb_state13.var_37) ^ (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_35) : (0xe913bb14));
                rb_state13.var_38 = (rb_state13.var_38) ^ (((0xceef7414) ^ (rb_state13.var_36)) - (0x6f7fd24b));
                rb_state13.var_39 = (rb_state13.var_39) ^ (((((0x62e53669) ^ (rb_state13.var_37)) + (0x443bdd3d)) ^ (rb_state13.var_38)) - (rb_state13.var_19));
                rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_39);
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (994641 < rb_input_size ? (uint32_t)rb_input[994641] : 0x3309d933);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
            #endif
            DR23[dir] = VR2[dir] / R2;
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_2) == (0x481daa99)) {
                rb_state0.var_18 = 0xb11ef4ef;
                rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_18) : (0xcef30966));
                rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_16) : (0xf194b5e0));
                rb_state0.var_21 = (rb_state0.var_21) - (((((0x4eaa8e28) - (rb_state0.var_18)) ^ (rb_state0.var_15)) - (rb_state0.var_19)) ^ (rb_state0.var_19));
                rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xe69bf2f));
                rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_22);
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_1) == (0x5f9de500)) {
                rb_state2.var_17 = (rb_state2.var_17) - (rb_state2.var_7);
                rb_state2.var_18 = (rb_state2.var_18) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0x9a90fffe));
                rb_state2.var_19 = (rb_state2.var_19) ^ (((0x5b3a5c9a) + (rb_state2.var_4)) - (rb_state2.var_8));
                rb_state2.var_20 = (rb_state2.var_20) ^ (296352 < rb_input_size ? (uint32_t)rb_input[296352] : 0xfc861910);
                rb_state2.var_21 = (rb_state2.var_21) + (((0xcebed0b) - (rb_state2.var_17)) - (0x8400d8c5));
                rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_18);
                rb_state2.var_23 = (rb_state2.var_23) ^ (((((0x36edc5df) + (rb_state2.var_9)) - (rb_state2.var_19)) + (0x9fc612f2)) - (rb_state2.var_20));
                rb_state2.var_24 = (rb_state2.var_24) - (((((0x7b3ecc52) + (0x66c1ec02)) ^ (rb_state2.var_22)) + (rb_state2.var_21)) + (rb_state2.var_10));
                rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_23) : (0xf25ba665));
                rb_state2.var_26 = (rb_state2.var_26) - (rb_state2.var_24);
                rb_state2.var_27 = (rb_state2.var_27) + (((0xec9df831) + (0x8f12358)) - (rb_state2.var_25));
                rb_state2.var_28 = (rb_state2.var_28) ^ (((((0x670f8f77) - (0x49449e32)) + (0x4b95c1d0)) ^ (rb_state2.var_26)) - (rb_state2.var_27));
                rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_28);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_2);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (1068905 < rb_input_size ? (uint32_t)rb_input[1068905] : 0xd1f76838);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_2 = (rb_state16.var_2) ^ (550711 < rb_input_size ? (uint32_t)rb_input[550711] : 0x8406a1f4);
            #endif
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_6) == (0x4d7435e9)) {
                pthread_mutex_lock(&(rb_state1.lock_40));
                rb_state1.var_14 = 0xb526427a;
                rb_state1.var_15 = 0xb815f74c;
                rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_14) : (0x3d04887b));
                rb_state1.var_17 = (rb_state1.var_17) + (0x3a9d007d);
                rb_state1.var_18 = (rb_state1.var_18) ^ (((((0xb757239f) ^ (rb_state1.var_12)) ^ (0x542a93c7)) - (rb_state1.var_16)) ^ (rb_state1.var_13));
                rb_state1.var_19 = (rb_state1.var_19) + (((((0xbd194ac6) ^ (0x182bb844)) + (0xd30cc5b4)) ^ (rb_state1.var_18)) ^ (rb_state1.var_17));
                rb_state1.var_13 = (rb_state1.var_13) - (rb_state1.var_19);
                rb_state1.var_12 = rb_state1.var_13;
                pthread_mutex_unlock(&(rb_state1.lock_40));
            }
            if ((rb_state1.var_6) == (0x4d7435e9)) {
                if (!((rb_state1.var_12) == (rb_state1.var_13))) {
                    racebench_trigger(1);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_1);
            #endif
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ ((0x8a98d6ce) - (rb_state1.var_0));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0xcaaa782a)) {
            rb_state14.var_2 = rb_state14.var_0;
        }
        if ((rb_state14.var_1) == (0xcaaa782a)) {
            if (!((rb_state14.var_0) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (3578707 < rb_input_size ? (uint32_t)rb_input[3578707] : 0xd887e9b6);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_2) == (0xc4be5aed)) {
            pthread_mutex_lock(&(rb_state18.lock_24));
            rb_state18.var_5 = 0x1098c4af;
            rb_state18.var_6 = (rb_state18.var_6) ^ (1651927 < rb_input_size ? (uint32_t)rb_input[1651927] : 0x8b673358);
            rb_state18.var_7 = (rb_state18.var_7) - (((0xa8f1f355) + (rb_state18.var_3)) + (rb_state18.var_5));
            rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0x59167b09));
            rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_5) : (0xdcb64772));
            rb_state18.var_10 = (rb_state18.var_10) - (((0xe95635c) ^ (rb_state18.var_8)) - (rb_state18.var_6));
            rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_7) : (0x9eb11add));
            rb_state18.var_12 = (rb_state18.var_12) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_8) : (0xcc4f9321));
            rb_state18.var_13 = (rb_state18.var_13) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_9) : (0x5e0c6e80));
            rb_state18.var_14 = (rb_state18.var_14) + (rb_state18.var_10);
            rb_state18.var_15 = (rb_state18.var_15) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0xd242135d));
            rb_state18.var_16 = (rb_state18.var_16) ^ (rb_state18.var_13);
            rb_state18.var_17 = (rb_state18.var_17) - (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0xdc867718));
            rb_state18.var_18 = (rb_state18.var_18) + (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_16) : (0x7baf3074));
            rb_state18.var_19 = (rb_state18.var_19) - (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_17) : (0x1f2f1b1b));
            rb_state18.var_20 = (rb_state18.var_20) - (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_19) : (0x9e635e10));
            rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_20);
            rb_state18.var_3 = rb_state18.var_4;
            pthread_mutex_unlock(&(rb_state18.lock_24));
        }
        if ((rb_state18.var_2) == (0x179ea3c7)) {
            if (!((rb_state18.var_3) == (rb_state18.var_4))) {
                racebench_trigger(18);
            }
        }
        #endif
        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0xfffffe62)) ? (rb_state1.var_0) : (0x9c19af04));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0xffffff3c)) ? (rb_state6.var_0) : (0xcd821164));
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_2) == (0xf23fe490)) {
            rb_state8.var_4 = 0xb5fe4ffa;
            rb_state8.var_5 = 0xa657170a;
            rb_state8.var_6 = (rb_state8.var_6) ^ (rb_state8.var_5);
            rb_state8.var_7 = (rb_state8.var_7) + (((((0x5882696d) ^ (rb_state8.var_3)) - (0xe6a63ae8)) + (rb_state8.var_4)) + (rb_state8.var_4));
            rb_state8.var_8 = (rb_state8.var_8) + (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0xa27e3aca));
            rb_state8.var_9 = (rb_state8.var_9) ^ (((((0x69a6d50f) - (rb_state8.var_7)) - (rb_state8.var_8)) ^ (rb_state8.var_6)) ^ (0xf3139ab8));
            rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_9);
        }
        #endif
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - (0xff5684cd);
        #endif
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_2) : (0x423d910a));
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_1) == (0x5f9de500)) {
            pthread_mutex_lock(&(rb_state2.lock_29));
            rb_state2.var_4 = 0x1214c92a;
            rb_state2.var_5 = 0x982a1070;
            rb_state2.var_6 = 0xe5603377;
            rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0xaa241872));
            rb_state2.var_8 = (rb_state2.var_8) - (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_4) : (0x4978b80b));
            rb_state2.var_9 = (rb_state2.var_9) ^ (rb_state2.var_7);
            rb_state2.var_10 = (rb_state2.var_10) ^ (rb_state2.var_8);
            rb_state2.var_11 = (rb_state2.var_11) + (rb_state2.var_9);
            rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_10);
            rb_state2.var_13 = (rb_state2.var_13) - (((0xe1b16bdf) ^ (rb_state2.var_3)) - (rb_state2.var_11));
            rb_state2.var_14 = (rb_state2.var_14) - (((0x3317b4eb) + (rb_state2.var_12)) - (0x624fb013));
            rb_state2.var_15 = (rb_state2.var_15) + (rb_state2.var_13);
            rb_state2.var_16 = (rb_state2.var_16) - (((((0x2e95b18a) + (0xc8f82319)) - (rb_state2.var_15)) - (rb_state2.var_14)) ^ (0xc312759f));
            rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_16);
            rb_state2.var_2 = rb_state2.var_3;
            pthread_mutex_unlock(&(rb_state2.lock_29));
        }
        if ((rb_state2.var_1) == (0x5f9de500)) {
            if (!((rb_state2.var_2) == (rb_state2.var_3))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ ((0x5fe2d082) - (0x72aebfdc));
        #endif
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - ((0x971e58b8) + (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (1292053 < rb_input_size ? (uint32_t)rb_input[1292053] : 0x603458b4);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + ((0x8cde924c) + (0x240a6b04));
        rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x224f3ef4)) ? (rb_state11.var_1) : (0xb1f8bc14));
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x1e2d)) {
            if ((rb_state17.var_3) != (0x0)) {
                if (!((rb_state17.var_3) == (rb_state17.var_22))) {
                    racebench_trigger(17);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ ((0xff7bc1f3) + (0x75bea507));
        #endif
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_2) == (0xf397eed0)) ? (rb_state8.var_2) : (0x488919b0));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_1) == (0x99c9aa44)) {
            pthread_mutex_lock(&(rb_state11.lock_28));
            rb_state11.var_21 = 0x742b6cfb;
            rb_state11.var_22 = 0x2df7b199;
            rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_15) : (0xf0a82dd));
            rb_state11.var_24 = (rb_state11.var_24) ^ (((((0x5b091963) - (0x87a1ed21)) - (0x7606e0d5)) + (rb_state11.var_22)) ^ (rb_state11.var_21));
            rb_state11.var_25 = (rb_state11.var_25) ^ (rb_state11.var_23);
            rb_state11.var_26 = (rb_state11.var_26) ^ (rb_state11.var_24);
            rb_state11.var_27 = (rb_state11.var_27) + (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_26) : (0x53c977c2));
            rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_27);
            pthread_mutex_unlock(&(rb_state11.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_12) : (0xaff5326e));
        #endif
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_4 = (rb_state1.var_4) ^ (rb_state1.var_3);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) ^ ((0xa6f9a750) - (rb_state11.var_2));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_9 = (rb_state19.var_9) + ((0x1cfb80e) - (rb_state19.var_10));
        #endif
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_1);
            #endif
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_1) == (0xec3ce5ad)) {
                rb_state4.var_10 = 0x2d219c5e;
                rb_state4.var_11 = (rb_state4.var_11) + (0xe3256393);
                rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_7) : (0xb9e26380));
                rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_11) : (0x3ac03d7f));
                rb_state4.var_14 = (rb_state4.var_14) - (((((0xec04519b) ^ (rb_state4.var_13)) - (0xb7279d59)) - (0xd00067d3)) - (rb_state4.var_12));
                rb_state4.var_9 = (rb_state4.var_9) - (rb_state4.var_14);
                rb_state4.var_2 = rb_state4.var_9;
            }
            if ((rb_state4.var_1) == (0xec3ce5ad)) {
                pthread_mutex_lock(&(rb_state4.lock_27));
                rb_state4.var_16 = 0xd0093843;
                rb_state4.var_17 = 0x794c9dd9;
                rb_state4.var_18 = 0x209e8bb1;
                rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_18) : (0x47793709));
                rb_state4.var_20 = (rb_state4.var_20) + (rb_state4.var_16);
                rb_state4.var_21 = (rb_state4.var_21) - (rb_state4.var_19);
                rb_state4.var_22 = (rb_state4.var_22) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_20) : (0x2b5b92a4));
                rb_state4.var_23 = (rb_state4.var_23) ^ (((0x83342518) ^ (rb_state4.var_9)) + (rb_state4.var_21));
                rb_state4.var_24 = (rb_state4.var_24) - (((0xf86ce92a) - (0x742bf101)) - (rb_state4.var_22));
                rb_state4.var_25 = (rb_state4.var_25) ^ (rb_state4.var_23);
                rb_state4.var_26 = (rb_state4.var_26) ^ (((((0xd63bfdd) + (rb_state4.var_25)) + (0xcefa64d5)) ^ (rb_state4.var_24)) + (rb_state4.var_10));
                rb_state4.var_15 = (rb_state4.var_15) + (rb_state4.var_26);
                rb_state4.var_2 = rb_state4.var_15;
                pthread_mutex_unlock(&(rb_state4.lock_27));
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_1);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x99)) ? (rb_state10.var_1) : (0xd4551d7f));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) + (2973296 < rb_input_size ? (uint32_t)rb_input[2973296] : 0xb5fc87ca);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + (2555630 < rb_input_size ? (uint32_t)rb_input[2555630] : 0xb7697049);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) ^ ((0xb766f301) + (0xe90635a));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) - (0x42caf6c);
            rb_state19.var_8 = (rb_state19.var_8) ^ (rb_state19.var_7);
            #endif
            temp_p = VAR[mol].F[FORCES][dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - ((0x7524e77c) + (rb_state0.var_0));
            rb_state0.var_2 = (rb_state0.var_2) + ((0x44efe8d8) - (0x5ce615a5));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x4efd9fa9)) ? (rb_state9.var_0) : (0x35f03d74));
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0x33c9db68)) {
                pthread_mutex_lock(&(rb_state16.lock_13));
                rb_state16.var_4 = 0x98c50b6;
                rb_state16.var_5 = 0x82f717;
                rb_state16.var_6 = (rb_state16.var_6) + (rb_state16.var_4);
                rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_5) : (0x1d1cb35a));
                rb_state16.var_8 = (rb_state16.var_8) ^ (rb_state16.var_6);
                rb_state16.var_9 = (rb_state16.var_9) - (((0xd7f09117) + (rb_state16.var_7)) ^ (rb_state16.var_4));
                rb_state16.var_10 = (rb_state16.var_10) - (rb_state16.var_8);
                rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_9);
                rb_state16.var_12 = (rb_state16.var_12) - (((((0xd277130f) ^ (rb_state16.var_11)) - (rb_state16.var_10)) - (0xf3859bc9)) ^ (rb_state16.var_5));
                rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_12);
                pthread_mutex_unlock(&(rb_state16.lock_13));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_1) == (0x45d26f81)) {
                rb_state19.var_14 = !((rb_state19.var_4) == (rb_state19.var_3));
            }
            #endif
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (0x8a13dbf8);
            if ((rb_state1.var_1) == (0x63455662)) {
                pthread_mutex_lock(&(rb_state1.lock_10));
                rb_state1.var_11 = !((rb_state1.var_3) == (rb_state1.var_2));
                pthread_mutex_unlock(&(rb_state1.lock_10));
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + ((0x4d690ab0) - (rb_state2.var_1));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x39)) ? (rb_state3.var_0) : (0xf8b22e10));
            rb_state3.var_1 = (rb_state3.var_1) - (128471 < rb_input_size ? (uint32_t)rb_input[128471] : 0xb3fd431f);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) - (600392 < rb_input_size ? (uint32_t)rb_input[600392] : 0x956be3d5);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) - ((0xb6b0deba) + (rb_state13.var_0));
            #endif
            temp_p[H1] = T1;
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) - (773131 < rb_input_size ? (uint32_t)rb_input[773131] : 0xf6df263e);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x5af9b9ea));
            #endif
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) - ((0xfca6140) + (0xe670aea8));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_0);
            if ((rb_state5.var_2) == (0xc15d6cb4)) {
                if ((rb_state5.var_3) != (0x0)) {
                    if (!((rb_state5.var_3) == (rb_state5.var_42))) {
                        racebench_trigger(5);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_0) == (0xfe93be78)) {
                rb_state6.var_2 = rb_state6.var_1;
            }
            if ((rb_state6.var_0) == (0xfe93be78)) {
                if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_1);
            if ((rb_state8.var_2) == (0xf23fe490)) {
                pthread_mutex_lock(&(rb_state8.lock_10));
                rb_state8.var_3 = rb_state8.var_0;
                pthread_mutex_unlock(&(rb_state8.lock_10));
            }
            if ((rb_state8.var_2) == (0xf23fe490)) {
                if (!((rb_state8.var_0) == (rb_state8.var_3))) {
                    racebench_trigger(8);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0xb802f5ce)) {
                pthread_mutex_lock(&(rb_state9.lock_12));
                rb_state9.var_2 = rb_state9.var_1;
                pthread_mutex_unlock(&(rb_state9.lock_12));
            }
            if ((rb_state9.var_0) == (0xb802f5ce)) {
                if (!((rb_state9.var_1) == (rb_state9.var_2))) {
                    racebench_trigger(9);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) + (0xc8c20bed);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x135e4750));
            rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_0);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (2146723 < rb_input_size ? (uint32_t)rb_input[2146723] : 0x171f4975);
            #endif
            temp_p[H2] = T2;
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (0xfe18f0ef);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (0x64d05ef7);
            rb_state9.var_0 = (rb_state9.var_0) + (0x980774d0);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_0);
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