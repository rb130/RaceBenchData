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
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (0x3a82a308);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (0x6a504cbb);
        if ((rb_state9.var_1) == (0x9dd8a675)) {
            if ((rb_state9.var_3) != (0x0)) {
                if (!((rb_state9.var_3) == (rb_state9.var_30))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - ((0xdeaff42b) + (rb_state12.var_1));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_6) == (0x1)) {
            rb_state15.var_8 = 0x3e07989b;
            rb_state15.var_9 = 0x8aa9fa16;
            rb_state15.var_10 = 0x65cd75ba;
            rb_state15.var_11 = (rb_state15.var_11) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_10) : (0xd2b798fc));
            rb_state15.var_12 = (rb_state15.var_12) ^ (((0x3c0b032d) + (0x9ef26c94)) - (rb_state15.var_8));
            rb_state15.var_13 = (rb_state15.var_13) + (rb_state15.var_9);
            rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_11);
            rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_12) : (0x76245b69));
            rb_state15.var_16 = (rb_state15.var_16) ^ (rb_state15.var_13);
            rb_state15.var_17 = (rb_state15.var_17) + (((((0x8b0990e0) ^ (0x9badefda)) - (0xdc9273eb)) - (rb_state15.var_15)) ^ (rb_state15.var_14));
            rb_state15.var_18 = (rb_state15.var_18) ^ (rb_state15.var_16);
            rb_state15.var_19 = (rb_state15.var_19) + (((((0xf77b6aa7) - (rb_state15.var_17)) + (0x9c72d68b)) ^ (rb_state15.var_9)) ^ (rb_state15.var_18));
            rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_19);
        }
        #endif
        SUM = 0.0;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) + (1477377 < rb_input_size ? (uint32_t)rb_input[1477377] : 0x4705997f);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (0xd1439926);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) - ((0xfd6618c9) - (rb_state13.var_0));
        #endif
        R1 = 0.0;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - (99809 < rb_input_size ? (uint32_t)rb_input[99809] : 0x3faa999d);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_2) : (0x8f0c99f0));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_1);
        if ((rb_state17.var_2) == (0xa986af3)) {
            pthread_mutex_lock(&(rb_state17.lock_13));
            rb_state17.var_5 = 0x20022663;
            rb_state17.var_6 = 0xd8e5932d;
            rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0xfaa86770));
            rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_3) : (0xe174a714));
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_8) : (0xa3f755f0));
            rb_state17.var_4 = (rb_state17.var_4) + (rb_state17.var_9);
            rb_state17.var_3 = rb_state17.var_4;
            pthread_mutex_unlock(&(rb_state17.lock_13));
        }
        if ((rb_state17.var_2) == (0xa986af3)) {
            pthread_mutex_lock(&(rb_state17.lock_13));
            rb_state17.var_14 = !((rb_state17.var_3) == (rb_state17.var_4));
            pthread_mutex_unlock(&(rb_state17.lock_13));
        }
        #endif
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_1) == (0xdf542ac6)) {
                rb_state0.var_3 = rb_state0.var_2;
            }
            if ((rb_state0.var_1) == (0xdf542ac6)) {
                if (!((rb_state0.var_2) == (rb_state0.var_3))) {
                    racebench_trigger(0);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) ^ (1015479 < rb_input_size ? (uint32_t)rb_input[1015479] : 0xc441a67f);
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_2) == (0x648d34f7)) {
                rb_state6.var_4 = rb_state6.var_3;
            }
            if ((rb_state6.var_2) == (0x648d34f7)) {
                if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) + (0x59cb9d47);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_0);
            rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_0) == (0xc57d5cf8)) ? (rb_state8.var_2) : (0xbdf533cb));
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_0) == (0x56390513)) {
                rb_state11.var_5 = 0x4ce43879;
                rb_state11.var_6 = (rb_state11.var_6) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_4) : (0x2d2e55ef));
                rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_6);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_0);
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_2) == (0xbbaa1f0d)) {
                rb_state15.var_4 = rb_state15.var_3;
            }
            if ((rb_state15.var_2) == (0xbbaa1f0d)) {
                rb_state15.var_6 = !((rb_state15.var_3) == (rb_state15.var_4));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0x9462d9de)) {
                rb_state16.var_5 = 0xf98c1e2f;
                rb_state16.var_6 = (rb_state16.var_6) + (((0x9ec5bd54) ^ (rb_state16.var_4)) + (0x67c32c8e));
                rb_state16.var_7 = (rb_state16.var_7) - (3305959 < rb_input_size ? (uint32_t)rb_input[3305959] : 0x6e04c139);
                rb_state16.var_8 = (rb_state16.var_8) + (((0xd25368d3) - (rb_state16.var_6)) + (rb_state16.var_5));
                rb_state16.var_9 = (rb_state16.var_9) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_6) : (0xa88c4d53));
                rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0x484bb4cb));
                rb_state16.var_11 = (rb_state16.var_11) + (((0xab272a5d) ^ (rb_state16.var_8)) - (0x58259b0a));
                rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_9);
                rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_10);
                rb_state16.var_14 = (rb_state16.var_14) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_11) : (0x58ccea25));
                rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x3dd08ba5));
                rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_15) : (0x4b500645));
                rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_16);
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_7 = (rb_state17.var_7) + ((0xdde91409) ^ (0x28ff6939));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (1529593 < rb_input_size ? (uint32_t)rb_input[1529593] : 0xc6627fc5);
            #endif
            temp_p = VAR[mol].F[DISP][dir];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) + (0x1ff4a258);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (3431994 < rb_input_size ? (uint32_t)rb_input[3431994] : 0x6ecc620);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_3 = (rb_state7.var_3) + (0x909985ac);
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_1) == (0x9dd8a675)) {
                rb_state9.var_12 = 0xe587d9e5;
                rb_state9.var_13 = 0xe6b3c557;
                rb_state9.var_14 = (rb_state9.var_14) ^ ((0xc27aafea) ^ (0xc61e2a57));
                rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_12) : (0x9171338c));
                rb_state9.var_16 = (rb_state9.var_16) ^ (((0x9c20db42) ^ (rb_state9.var_5)) - (rb_state9.var_3));
                rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_13) : (0x90e97192));
                rb_state9.var_18 = (rb_state9.var_18) ^ (((0xd40c5e52) - (rb_state9.var_7)) - (rb_state9.var_14));
                rb_state9.var_19 = (rb_state9.var_19) - (((0xc4110c49) - (0x18089da1)) - (rb_state9.var_15));
                rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_17) : (0x6cd6249b));
                rb_state9.var_21 = (rb_state9.var_21) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_18) : (0xf860aace));
                rb_state9.var_22 = (rb_state9.var_22) + (rb_state9.var_19);
                rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_20) : (0x627db840));
                rb_state9.var_24 = (rb_state9.var_24) ^ (rb_state9.var_21);
                rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_22) : (0x33052632));
                rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0xc05ceff0));
                rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_25) : (0xf9ec57a0));
                rb_state9.var_28 = (rb_state9.var_28) - (((0xc4af4a9e) ^ (0x9f17672f)) - (rb_state9.var_26));
                rb_state9.var_29 = (rb_state9.var_29) - (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0x9b4d2fe));
                rb_state9.var_11 = (rb_state9.var_11) - (rb_state9.var_29);
                rb_state9.var_3 = rb_state9.var_11;
            }
            if ((rb_state9.var_1) == (0x9dd8a675)) {
                pthread_mutex_lock(&(rb_state9.lock_41));
                rb_state9.var_31 = 0xb30535f3;
                rb_state9.var_32 = 0x85eb6581;
                rb_state9.var_33 = (rb_state9.var_33) ^ (((0xa4671a97) - (0x192bf137)) - (rb_state9.var_12));
                rb_state9.var_34 = (rb_state9.var_34) ^ (0x49f3712f);
                rb_state9.var_35 = (rb_state9.var_35) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_31) : (0xdc46ab94));
                rb_state9.var_36 = (rb_state9.var_36) - (rb_state9.var_32);
                rb_state9.var_37 = (rb_state9.var_37) - (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0xf9b2df5c));
                rb_state9.var_38 = (rb_state9.var_38) + (((0xf1cd5838) - (rb_state9.var_14)) + (rb_state9.var_35));
                rb_state9.var_39 = (rb_state9.var_39) - (((((0x2a5976fe) - (rb_state9.var_37)) + (rb_state9.var_15)) ^ (rb_state9.var_16)) ^ (rb_state9.var_36));
                rb_state9.var_40 = (rb_state9.var_40) ^ (((rb_state9.var_38) == (0x0)) ? (rb_state9.var_39) : (0x15c701c6));
                rb_state9.var_30 = (rb_state9.var_30) + (rb_state9.var_40);
                rb_state9.var_3 = rb_state9.var_30;
                pthread_mutex_unlock(&(rb_state9.lock_41));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (2845421 < rb_input_size ? (uint32_t)rb_input[2845421] : 0xccb17a66);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0xe247b61));
            rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_2 = (rb_state13.var_2) + (0xfadfba88);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_2 = (rb_state14.var_2) + (0x7972b384);
            #endif
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (0xc46bd9bb);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) - (1062724 < rb_input_size ? (uint32_t)rb_input[1062724] : 0xfeb830ad);
            if ((rb_state3.var_0) == (0x488e4525)) {
                rb_state3.var_10 = 0x4fd5d4f5;
                rb_state3.var_11 = (rb_state3.var_11) + (rb_state3.var_5);
                rb_state3.var_12 = (rb_state3.var_12) + (1392958 < rb_input_size ? (uint32_t)rb_input[1392958] : 0x47297676);
                rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_10) : (0xc059eea3));
                rb_state3.var_14 = (rb_state3.var_14) + (rb_state3.var_11);
                rb_state3.var_15 = (rb_state3.var_15) - (((0xd739d415) + (rb_state3.var_12)) ^ (0xf1b0d1c1));
                rb_state3.var_16 = (rb_state3.var_16) ^ (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_13) : (0x407912c5));
                rb_state3.var_17 = (rb_state3.var_17) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0x4fb0d85d));
                rb_state3.var_18 = (rb_state3.var_18) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_16) : (0xb078850));
                rb_state3.var_19 = (rb_state3.var_19) - (((0x49137f9b) - (rb_state3.var_17)) ^ (0xc12e3eb2));
                rb_state3.var_20 = (rb_state3.var_20) - (((((0xc596844b) - (rb_state3.var_18)) - (0x21d1c87f)) - (rb_state3.var_19)) - (rb_state3.var_9));
                rb_state3.var_4 = (rb_state3.var_4) ^ (rb_state3.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_0) : (0xa2e3b88f));
            rb_state4.var_17 = (rb_state4.var_17) + (3253762 < rb_input_size ? (uint32_t)rb_input[3253762] : 0x85ff33ea);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) - (0x3ebfb463);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) + (((rb_state8.var_1) == (0x3a82a308)) ? (rb_state8.var_0) : (0xd7fc4cd3));
            rb_state8.var_3 = (rb_state8.var_3) + (1727913 < rb_input_size ? (uint32_t)rb_input[1727913] : 0x6db72c80);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) + ((0x453be5f7) + (rb_state10.var_0));
            rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_0);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (0x27326af);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (2564360 < rb_input_size ? (uint32_t)rb_input[2564360] : 0x2d1a3b1f);
            #endif
            VR1[dir] = temp_p[O] - temp_p[H1];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) + (310873 < rb_input_size ? (uint32_t)rb_input[310873] : 0xf068ac2c);
            rb_state0.var_0 = (rb_state0.var_0) + (2615768 < rb_input_size ? (uint32_t)rb_input[2615768] : 0x4b7ce882);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) ^ (1348534 < rb_input_size ? (uint32_t)rb_input[1348534] : 0x58b0185d);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_3 = (rb_state5.var_3) + (1847566 < rb_input_size ? (uint32_t)rb_input[1847566] : 0xe70209e8);
            rb_state5.var_0 = (rb_state5.var_0) - ((0xf407b6a1) + (rb_state5.var_1));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_0);
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0x362943aa)) {
                rb_state12.var_3 = rb_state12.var_2;
            }
            if ((rb_state12.var_1) == (0x362943aa)) {
                pthread_mutex_lock(&(rb_state12.lock_18));
                if (!((rb_state12.var_2) == (rb_state12.var_3))) {
                    racebench_trigger(12);
                }
                pthread_mutex_unlock(&(rb_state12.lock_18));
            }
            #endif
            R1 += VR1[dir] * VR1[dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_0);
            #endif
            #ifdef RACEBENCH_BUG_2
            if (!((rb_state2.var_3) == (rb_state2.var_19))) {
                rb_state2.var_49 = (rb_state2.var_49) ^ (((0x8bcf585c) - (rb_state2.var_24)) + (rb_state2.var_22));
                rb_state2.var_50 = (rb_state2.var_50) - (112267 < rb_input_size ? (uint32_t)rb_input[112267] : 0xac00d881);
                rb_state2.var_51 = (rb_state2.var_51) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_25) : (0x6b90f2c8));
                rb_state2.var_52 = (rb_state2.var_52) + (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_23) : (0x630f9a0c));
                rb_state2.var_53 = (rb_state2.var_53) - (rb_state2.var_49);
                rb_state2.var_54 = (rb_state2.var_54) ^ (rb_state2.var_50);
                rb_state2.var_55 = (rb_state2.var_55) ^ (((0x34428590) + (0xe3550b84)) ^ (rb_state2.var_51));
                rb_state2.var_56 = (rb_state2.var_56) ^ (((rb_state2.var_52) == (0x0)) ? (rb_state2.var_53) : (0x6d22cd0e));
                rb_state2.var_57 = (rb_state2.var_57) + (rb_state2.var_54);
                rb_state2.var_58 = (rb_state2.var_58) - (((0x6c67db1d) + (rb_state2.var_27)) + (rb_state2.var_55));
                rb_state2.var_59 = (rb_state2.var_59) ^ (((0x1170547d) - (rb_state2.var_56)) - (0x428e194e));
                rb_state2.var_60 = (rb_state2.var_60) - (rb_state2.var_57);
                rb_state2.var_61 = (rb_state2.var_61) - (rb_state2.var_58);
                rb_state2.var_62 = (rb_state2.var_62) - (((0x150b9e2b) - (rb_state2.var_59)) + (0xacfa6c45));
                rb_state2.var_63 = (rb_state2.var_63) + (((rb_state2.var_28) == (0x0)) ? (rb_state2.var_60) : (0xc3973e07));
                rb_state2.var_64 = (rb_state2.var_64) + (rb_state2.var_61);
                rb_state2.var_65 = (rb_state2.var_65) + (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_62) : (0x1fd3621d));
                rb_state2.var_66 = (rb_state2.var_66) - (((rb_state2.var_30) == (0x0)) ? (rb_state2.var_63) : (0xb10de9ce));
                rb_state2.var_67 = (rb_state2.var_67) - (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_64) : (0xd1755738));
                rb_state2.var_68 = (rb_state2.var_68) ^ (((rb_state2.var_33) == (0x0)) ? (rb_state2.var_65) : (0x6c62044c));
                rb_state2.var_69 = (rb_state2.var_69) ^ (rb_state2.var_66);
                rb_state2.var_70 = (rb_state2.var_70) - (rb_state2.var_67);
                rb_state2.var_71 = (rb_state2.var_71) + (((0xbb277e30) - (rb_state2.var_34)) - (rb_state2.var_68));
                rb_state2.var_72 = (rb_state2.var_72) + (((((0x50c3f320) - (rb_state2.var_70)) - (0xc746368c)) + (rb_state2.var_69)) - (0xb1179e67));
                rb_state2.var_73 = (rb_state2.var_73) + (((rb_state2.var_71) == (0x0)) ? (rb_state2.var_72) : (0x60805c99));
                rb_state2.var_33 = (rb_state2.var_33) - (rb_state2.var_73);
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) - (0x5058b71e);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (2870622 < rb_input_size ? (uint32_t)rb_input[2870622] : 0x7a46ff8e);
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_2) == (0xa363e64b)) {
                rb_state7.var_4 = rb_state7.var_3;
            }
            if ((rb_state7.var_2) == (0xa363e64b)) {
                if (!((rb_state7.var_3) == (rb_state7.var_4))) {
                    racebench_trigger(7);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_2);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_0);
            rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_10);
            #endif
            VR2[dir] = temp_p[O] - temp_p[H2];
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_2) == (0xffffff4a)) ? (rb_state2.var_1) : (0x79932c1));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (1490031 < rb_input_size ? (uint32_t)rb_input[1490031] : 0x2952bd9b);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_3 = (rb_state11.var_3) - (0xdaf1bd6c);
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_2) == (0xfec16ba9)) {
                pthread_mutex_lock(&(rb_state14.lock_16));
                rb_state14.var_12 = 0xbe96695c;
                rb_state14.var_13 = (rb_state14.var_13) + (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_4) : (0x46c24d95));
                rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_12) : (0xf62f7dd5));
                rb_state14.var_15 = (rb_state14.var_15) - (((((0x233caa57) + (0x17a6a722)) + (rb_state14.var_13)) + (0xf39010da)) - (rb_state14.var_14));
                rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_15);
                pthread_mutex_unlock(&(rb_state14.lock_16));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_0);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_1) : (0xe70225c));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_1);
            #endif
            R2 += VR2[dir] * VR2[dir];
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_0) == (0xa6866dcf)) {
                if ((rb_state2.var_3) != (0x0)) {
                    if (!((rb_state2.var_3) == (rb_state2.var_19))) {
                        pthread_mutex_lock(&(rb_state2.lock_74));
                        rb_state2.var_35 = 0xc39ec836;
                        rb_state2.var_36 = 0x33e0dc17;
                        rb_state2.var_37 = 0xca415ac3;
                        rb_state2.var_38 = (rb_state2.var_38) + (rb_state2.var_15);
                        rb_state2.var_39 = (rb_state2.var_39) ^ (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_37) : (0xa356a16));
                        rb_state2.var_40 = (rb_state2.var_40) - (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_36) : (0x8d13f233));
                        rb_state2.var_41 = (rb_state2.var_41) ^ (((rb_state2.var_35) == (0x0)) ? (rb_state2.var_38) : (0x1a90c65c));
                        rb_state2.var_42 = (rb_state2.var_42) ^ (((((0x7bd98a81) + (rb_state2.var_39)) - (rb_state2.var_40)) + (rb_state2.var_18)) ^ (0x71b3532d));
                        rb_state2.var_43 = (rb_state2.var_43) ^ (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_41) : (0xbc5149b7));
                        rb_state2.var_44 = (rb_state2.var_44) - (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_42) : (0xc7cbfac));
                        rb_state2.var_45 = (rb_state2.var_45) + (rb_state2.var_43);
                        rb_state2.var_46 = (rb_state2.var_46) - (((0xd9e9326d) ^ (rb_state2.var_44)) ^ (0x8d78fba9));
                        rb_state2.var_47 = (rb_state2.var_47) ^ (rb_state2.var_45);
                        rb_state2.var_48 = (rb_state2.var_48) + (((rb_state2.var_46) == (0x0)) ? (rb_state2.var_47) : (0xdc799ce0));
                        rb_state2.var_34 = (rb_state2.var_34) + (rb_state2.var_48);
                        rb_state2.var_33 = rb_state2.var_34;
                        pthread_mutex_unlock(&(rb_state2.lock_74));
                    }
                    if (!((rb_state2.var_3) == (rb_state2.var_19))) {
                        if (!((rb_state2.var_33) == (rb_state2.var_34))) {
                            racebench_trigger(2);
                        }
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_1);
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_1) == (0xb693fc4e)) {
                rb_state10.var_15 = 0xb41cae25;
                rb_state10.var_16 = 0x657c50ff;
                rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_16) : (0x631e5e5f));
                rb_state10.var_18 = (rb_state10.var_18) + (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_7) : (0xd805c4e1));
                rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_17);
                rb_state10.var_20 = (rb_state10.var_20) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_18) : (0xb3d33d32));
                rb_state10.var_21 = (rb_state10.var_21) + (rb_state10.var_19);
                rb_state10.var_22 = (rb_state10.var_22) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_20) : (0xb3a4a45e));
                rb_state10.var_23 = (rb_state10.var_23) + (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_21) : (0x70cca96d));
                rb_state10.var_24 = (rb_state10.var_24) + (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_23) : (0xae2f8638));
                rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_24);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ ((0xdf5adc3) ^ (0x662e85ad));
            rb_state11.var_3 = (rb_state11.var_3) ^ ((0x690bef90) - (0xb5ce8433));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) ^ (0x96da821);
            rb_state18.var_1 = (rb_state18.var_1) - ((0x17b92cf4) - (0x283a26f8));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_3 = (rb_state19.var_3) - (2178404 < rb_input_size ? (uint32_t)rb_input[2178404] : 0x1bf83d66);
            #endif
            SUM += VR1[dir] * VR2[dir];
        }

        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_1);
        #endif
        R1 = sqrt(R1);
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0x488e4525)) {
            pthread_mutex_lock(&(rb_state3.lock_21));
            rb_state3.var_6 = 0x73a8cf8e;
            rb_state3.var_7 = 0x674fc1aa;
            rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_6) : (0x8e7036c5));
            rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_8) : (0x18d60e26));
            rb_state3.var_5 = (rb_state3.var_5) ^ (rb_state3.var_9);
            rb_state3.var_4 = rb_state3.var_5;
            pthread_mutex_unlock(&(rb_state3.lock_21));
        }
        if ((rb_state3.var_0) == (0x488e4525)) {
            if (!((rb_state3.var_4) == (rb_state3.var_5))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (1971231 < rb_input_size ? (uint32_t)rb_input[1971231] : 0x914a0ae9);
        #endif
        R2 = sqrt(R2);

        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_1) == (0x362943aa)) {
            rb_state12.var_4 = 0xa685ed76;
            rb_state12.var_5 = 0xedf8ba23;
            rb_state12.var_6 = 0x32134cd2;
            rb_state12.var_7 = (rb_state12.var_7) ^ (rb_state12.var_3);
            rb_state12.var_8 = (rb_state12.var_8) ^ (rb_state12.var_5);
            rb_state12.var_9 = (rb_state12.var_9) + (((0xe7fc0d0) + (rb_state12.var_4)) - (0x2a66f3df));
            rb_state12.var_10 = (rb_state12.var_10) ^ (((0xc2dc3349) + (rb_state12.var_6)) - (rb_state12.var_4));
            rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0x2d916a31));
            rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0x47919718));
            rb_state12.var_13 = (rb_state12.var_13) ^ (rb_state12.var_11);
            rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_12) : (0xc267c6da));
            rb_state12.var_15 = (rb_state12.var_15) ^ (((0xdc9c8663) + (rb_state12.var_13)) - (rb_state12.var_6));
            rb_state12.var_16 = (rb_state12.var_16) + (((0x305ca26) ^ (0x4f86ad8d)) + (rb_state12.var_14));
            rb_state12.var_17 = (rb_state12.var_17) ^ (((((0x97ddc991) ^ (rb_state12.var_16)) ^ (rb_state12.var_15)) - (rb_state12.var_7)) ^ (0xee80121f));
            rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_2) == (0xfec16ba9)) {
            pthread_mutex_lock(&(rb_state14.lock_16));
            rb_state14.var_5 = 0xfca2fb8f;
            rb_state14.var_6 = 0xe7ac0402;
            rb_state14.var_7 = 0x28db2eee;
            rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_5) : (0xf3f02b9f));
            rb_state14.var_9 = (rb_state14.var_9) ^ (((0x81028f7c) ^ (rb_state14.var_6)) + (rb_state14.var_3));
            rb_state14.var_10 = (rb_state14.var_10) + (((0xb99744bd) + (rb_state14.var_8)) ^ (0x7f9ec546));
            rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_10) : (0xa6e5f060));
            rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_11);
            rb_state14.var_3 = rb_state14.var_4;
            pthread_mutex_unlock(&(rb_state14.lock_16));
        }
        if ((rb_state14.var_2) == (0xfec16ba9)) {
            if (!((rb_state14.var_3) == (rb_state14.var_4))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0x9462d9de)) {
            rb_state16.var_4 = rb_state16.var_2;
        }
        if ((rb_state16.var_1) == (0x9462d9de)) {
            pthread_mutex_lock(&(rb_state16.lock_17));
            if (!((rb_state16.var_2) == (rb_state16.var_4))) {
                racebench_trigger(16);
            }
            pthread_mutex_unlock(&(rb_state16.lock_17));
        }
        #endif
        COS = SUM / (R1 * R2);
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ ((0x441c0ebc) ^ (0xbadf6f37));
        #endif
        SIN = sqrt(ONE - COS * COS);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - (0x145efcaf);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0x17d5424);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) - ((0xb6533284) + (0xd3de07eb));
        #endif
        DT = (acos(COS) - ANGLE) * ROH;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + ((0xb62336fb) ^ (rb_state0.var_1));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (0x37899968);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ ((0xdc3956ab) + (rb_state14.var_0));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_3) : (0x472f58e8));
        #endif
        DTS = DT * DT;
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_1) : (0xd5951df3));
        #endif
        DR1 = R1 - ROH;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - (1657819 < rb_input_size ? (uint32_t)rb_input[1657819] : 0x6b7f7fd6);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + ((0x5b3bf5f6) ^ (rb_state6.var_1));
        #endif
        DR1S = DR1 * DR1;
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_8 = (rb_state17.var_8) + (rb_state17.var_9);
        #endif
        DR2 = R2 - ROH;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) - (0xb2f2eb0c);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_24) == (0x1)) {
            rb_state4.var_25 = rb_state4.var_6;
        }
        if ((rb_state4.var_24) == (0x1)) {
            if (!((rb_state4.var_6) == (rb_state4.var_25))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
        #endif
        DR2S = DR2 * DR2;

        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_1) == (0xd1944897)) {
            rb_state13.var_5 = 0x46cac6b;
            rb_state13.var_6 = 0x89dae7b0;
            rb_state13.var_7 = (rb_state13.var_7) ^ (233083 < rb_input_size ? (uint32_t)rb_input[233083] : 0x9b0ffb60);
            rb_state13.var_8 = (rb_state13.var_8) + (0x405524dc);
            rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_4) : (0x167698a0));
            rb_state13.var_10 = (rb_state13.var_10) ^ (rb_state13.var_6);
            rb_state13.var_11 = (rb_state13.var_11) - (((0x48d5bcdc) + (rb_state13.var_5)) + (rb_state13.var_6));
            rb_state13.var_12 = (rb_state13.var_12) ^ (((0x8ee1378f) ^ (0xf5e3ffd6)) ^ (rb_state13.var_7));
            rb_state13.var_13 = (rb_state13.var_13) ^ (((0x20e4711b) + (rb_state13.var_7)) ^ (rb_state13.var_8));
            rb_state13.var_14 = (rb_state13.var_14) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_10) : (0xdc0dae32));
            rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_11);
            rb_state13.var_16 = (rb_state13.var_16) - (((((0x222dbec0) ^ (rb_state13.var_13)) - (rb_state13.var_12)) - (0x59e16b06)) ^ (0xd1a5dc93));
            rb_state13.var_17 = (rb_state13.var_17) - (((0x485e5390) - (rb_state13.var_14)) ^ (0x48b05f86));
            rb_state13.var_18 = (rb_state13.var_18) + (((0xb0b217ab) + (0xcde8e9d9)) + (rb_state13.var_15));
            rb_state13.var_19 = (rb_state13.var_19) + (((((0x482a374a) ^ (rb_state13.var_17)) + (rb_state13.var_16)) ^ (rb_state13.var_8)) ^ (0xf0573116));
            rb_state13.var_20 = (rb_state13.var_20) - (((0x5423a57e) - (rb_state13.var_18)) ^ (0xdfa3bfb6));
            rb_state13.var_21 = (rb_state13.var_21) - (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_20) : (0xf004bdce));
            rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_21);
        }
        #endif
        R1S = ROH / (R1 * SIN);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + (3017696 < rb_input_size ? (uint32_t)rb_input[3017696] : 0x7df743ae);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + ((0xa069ba05) ^ (rb_state3.var_0));
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_24) == (0x1)) {
            rb_state4.var_26 = 0x680fad9d;
            rb_state4.var_27 = 0x16c87499;
            rb_state4.var_28 = 0x5bd7bf4e;
            rb_state4.var_29 = (rb_state4.var_29) - (rb_state4.var_25);
            rb_state4.var_30 = (rb_state4.var_30) + (((rb_state4.var_26) == (0x0)) ? (rb_state4.var_26) : (0xe28cf1ef));
            rb_state4.var_31 = (rb_state4.var_31) ^ (rb_state4.var_27);
            rb_state4.var_32 = (rb_state4.var_32) - (rb_state4.var_28);
            rb_state4.var_33 = (rb_state4.var_33) ^ (rb_state4.var_29);
            rb_state4.var_34 = (rb_state4.var_34) ^ (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_30) : (0xa92ac078));
            rb_state4.var_35 = (rb_state4.var_35) ^ (((0x81715a13) - (rb_state4.var_31)) + (rb_state4.var_28));
            rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_32);
            rb_state4.var_37 = (rb_state4.var_37) ^ (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_33) : (0xd2cadc9));
            rb_state4.var_38 = (rb_state4.var_38) - (((0xe114eb72) - (rb_state4.var_34)) - (0xcb73242));
            rb_state4.var_39 = (rb_state4.var_39) + (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_35) : (0x77849061));
            rb_state4.var_40 = (rb_state4.var_40) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_36) : (0x241ad132));
            rb_state4.var_41 = (rb_state4.var_41) + (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_37) : (0x809adfd3));
            rb_state4.var_42 = (rb_state4.var_42) - (((rb_state4.var_38) == (0x0)) ? (rb_state4.var_39) : (0x42f773ba));
            rb_state4.var_43 = (rb_state4.var_43) + (((rb_state4.var_33) == (0x0)) ? (rb_state4.var_40) : (0x4c93eb46));
            rb_state4.var_44 = (rb_state4.var_44) + (((0x35cf1ccb) ^ (rb_state4.var_41)) ^ (rb_state4.var_34));
            rb_state4.var_45 = (rb_state4.var_45) ^ (rb_state4.var_42);
            rb_state4.var_46 = (rb_state4.var_46) + (((0x874a2f4f) ^ (rb_state4.var_35)) ^ (rb_state4.var_43));
            rb_state4.var_47 = (rb_state4.var_47) ^ (((rb_state4.var_36) == (0x0)) ? (rb_state4.var_44) : (0xf9d08483));
            rb_state4.var_48 = (rb_state4.var_48) + (((rb_state4.var_45) == (0x0)) ? (rb_state4.var_46) : (0xe31d2344));
            rb_state4.var_49 = (rb_state4.var_49) ^ (((rb_state4.var_37) == (0x0)) ? (rb_state4.var_47) : (0xbdfba3f1));
            rb_state4.var_50 = (rb_state4.var_50) - (((0x38133f32) ^ (0x143f7590)) - (rb_state4.var_48));
            rb_state4.var_51 = (rb_state4.var_51) + (((0xadfa571d) ^ (0xd2a202eb)) + (rb_state4.var_49));
            rb_state4.var_52 = (rb_state4.var_52) + (rb_state4.var_50);
            rb_state4.var_53 = (rb_state4.var_53) + (((rb_state4.var_51) == (0x0)) ? (rb_state4.var_52) : (0xba526834));
            rb_state4.var_6 = (rb_state4.var_6) - (rb_state4.var_53);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (1893942 < rb_input_size ? (uint32_t)rb_input[1893942] : 0x97bac72e);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ ((0x9d7cbd47) ^ (0x28d4f013));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xef112bb6));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_3 = (rb_state16.var_3) - (0x6d9af5c6);
        #endif
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_0) == (0x2d26b8a0)) {
                rb_state1.var_19 = 0x498e35e0;
                rb_state1.var_20 = 0x736a27a8;
                rb_state1.var_21 = 0x41773344;
                rb_state1.var_22 = (rb_state1.var_22) + (rb_state1.var_21);
                rb_state1.var_23 = (rb_state1.var_23) ^ (((((0xfd0c8ad8) - (rb_state1.var_19)) - (rb_state1.var_20)) - (0x6a24b140)) ^ (0xec2a2e4f));
                rb_state1.var_24 = (rb_state1.var_24) - (rb_state1.var_22);
                rb_state1.var_25 = (rb_state1.var_25) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_23) : (0xb9edca4b));
                rb_state1.var_26 = (rb_state1.var_26) - (((0x729a6d93) + (rb_state1.var_24)) + (rb_state1.var_13));
                rb_state1.var_27 = (rb_state1.var_27) - (rb_state1.var_25);
                rb_state1.var_28 = (rb_state1.var_28) ^ (((0x545d37f6) - (0xf91e8c0d)) ^ (rb_state1.var_26));
                rb_state1.var_29 = (rb_state1.var_29) - (((0x63220931) ^ (rb_state1.var_27)) - (0x6a02af80));
                rb_state1.var_30 = (rb_state1.var_30) ^ (((rb_state1.var_28) == (0x0)) ? (rb_state1.var_29) : (0x238f8592));
                rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_30);
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_3 = (rb_state8.var_3) ^ (2307746 < rb_input_size ? (uint32_t)rb_input[2307746] : 0x587a69d9);
            if ((rb_state8.var_2) == (0x1c335002)) {
                rb_state8.var_4 = rb_state8.var_3;
            }
            if ((rb_state8.var_2) == (0x1c335002)) {
                if (!((rb_state8.var_3) == (rb_state8.var_4))) {
                    racebench_trigger(8);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) ^ ((0x8cf49789) + (rb_state10.var_2));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_2 = (rb_state11.var_2) ^ ((0x79e10d6b) + (0xbde600ec));
            rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_3) : (0xca21c26));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) - (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x41f4a51));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) - ((0x2dac6776) + (0x71432ec5));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) + ((0x15352dd5) + (0x5a12072));
            if ((rb_state17.var_2) == (0xa986af3)) {
                rb_state17.var_10 = 0xca005344;
                rb_state17.var_11 = (rb_state17.var_11) ^ (((((0xbab0f7ce) ^ (rb_state17.var_5)) + (0xa364a764)) + (rb_state17.var_7)) - (rb_state17.var_6));
                rb_state17.var_12 = (rb_state17.var_12) - (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x8e8d7051));
                rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_12);
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_1) == (0x7386d61c)) ? (rb_state18.var_0) : (0x4b6b7029));
            #endif
            DR11[dir] = VR1[dir] / R1;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_21 = (rb_state4.var_21) - ((0x109ca363) + (rb_state4.var_22));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_3 = (rb_state6.var_3) + (1231865 < rb_input_size ? (uint32_t)rb_input[1231865] : 0x5ece4232);
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_2) == (0x1c335002)) {
                rb_state8.var_5 = 0x54d00e2a;
                rb_state8.var_6 = (rb_state8.var_6) - (((((0x25f94da6) - (rb_state8.var_6)) + (0xfb44c4fd)) ^ (rb_state8.var_5)) + (rb_state8.var_4));
                rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_6);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) + (3556378 < rb_input_size ? (uint32_t)rb_input[3556378] : 0x48186e31);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_0) == (0x56390513)) {
                pthread_mutex_lock(&(rb_state11.lock_7));
                rb_state11.var_4 = rb_state11.var_1;
                pthread_mutex_unlock(&(rb_state11.lock_7));
            }
            if ((rb_state11.var_0) == (0x56390513)) {
                if (!((rb_state11.var_1) == (rb_state11.var_4))) {
                    racebench_trigger(11);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) ^ ((0x7bf8d923) ^ (rb_state12.var_2));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) ^ (804560 < rb_input_size ? (uint32_t)rb_input[804560] : 0x5076e27a);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) + ((0x72f56e0f) ^ (rb_state14.var_1));
            #endif
            DR23[dir] = VR2[dir] / R2;
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) ^ (1924551 < rb_input_size ? (uint32_t)rb_input[1924551] : 0x48d64924);
            rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_1);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_2);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x374b2b51));
            if ((rb_state6.var_2) == (0x648d34f7)) {
                rb_state6.var_5 = 0x35e45b4b;
                rb_state6.var_6 = 0x734f0328;
                rb_state6.var_7 = (rb_state6.var_7) ^ (((((0x91f770a7) - (rb_state6.var_4)) - (0xef1ad7e4)) + (rb_state6.var_5)) ^ (rb_state6.var_5));
                rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_7) : (0x86b25f4d));
                rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_8);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_0);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - ((0xac9da3be) - (0xe577b16a));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_2) : (0x18181231));
            rb_state15.var_2 = (rb_state15.var_2) + (400119 < rb_input_size ? (uint32_t)rb_input[400119] : 0x8a8337ee);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_2 = (rb_state18.var_2) + (0x483dfbee);
            if ((rb_state18.var_2) == (0xd70e25c2)) {
                rb_state18.var_10 = 0x51f046c2;
                rb_state18.var_11 = (rb_state18.var_11) ^ (((((0x1ca4ca43) + (0xc54b32c9)) ^ (rb_state18.var_5)) - (rb_state18.var_6)) ^ (rb_state18.var_4));
                rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_10) : (0xf0d6c9e4));
                rb_state18.var_13 = (rb_state18.var_13) + (((((0xe4f4713b) ^ (rb_state18.var_8)) ^ (rb_state18.var_12)) - (rb_state18.var_9)) - (rb_state18.var_11));
                rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_13);
            }
            #endif
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_2);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - (462325 < rb_input_size ? (uint32_t)rb_input[462325] : 0x11fd4c14);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) + ((0x2d3cee0e) ^ (rb_state4.var_0));
            if ((rb_state4.var_2) == (0x25fa0a2c)) {
                rb_state4.var_11 = 0x742a4cdb;
                rb_state4.var_12 = 0x2795fedc;
                rb_state4.var_13 = (rb_state4.var_13) + (((((0xc2292a46) - (0xe7376279)) ^ (rb_state4.var_11)) - (rb_state4.var_9)) ^ (rb_state4.var_12));
                rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_10) : (0x93b35698));
                rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_13) : (0x4df54994));
                rb_state4.var_16 = (rb_state4.var_16) ^ (((0x54491d72) - (rb_state4.var_14)) + (rb_state4.var_12));
                rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_15) : (0x65599577));
                rb_state4.var_18 = (rb_state4.var_18) + (((0xb9b9a766) ^ (rb_state4.var_16)) + (rb_state4.var_14));
                rb_state4.var_19 = (rb_state4.var_19) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_17) : (0xcb157b96));
                rb_state4.var_20 = (rb_state4.var_20) + (((0x546db1dc) + (rb_state4.var_16)) - (rb_state4.var_18));
                rb_state4.var_21 = (rb_state4.var_21) ^ (((0xfd6cfa58) + (0xd6f2660f)) ^ (rb_state4.var_19));
                rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_21) : (0x7585c4cc));
                rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_22);
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x369bb67d));
            rb_state15.var_0 = (rb_state15.var_0) - ((0x505fbb5a) - (rb_state15.var_6));
            if ((rb_state15.var_6) == (0x1)) {
                rb_state15.var_7 = rb_state15.var_0;
            }
            if ((rb_state15.var_6) == (0x1)) {
                if (!((rb_state15.var_0) == (rb_state15.var_7))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (459465 < rb_input_size ? (uint32_t)rb_input[459465] : 0x40b0a942);
            rb_state16.var_2 = (rb_state16.var_2) + (1138577 < rb_input_size ? (uint32_t)rb_input[1138577] : 0x75328d0f);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_2);
            if ((rb_state17.var_14) == (0x1)) {
                pthread_mutex_lock(&(rb_state17.lock_33));
                rb_state17.var_17 = 0x49158e7a;
                rb_state17.var_18 = 0x9eee0028;
                rb_state17.var_19 = 0xbcee2eaf;
                rb_state17.var_20 = (rb_state17.var_20) - (((0xf2b4dfee) - (rb_state17.var_19)) - (rb_state17.var_15));
                rb_state17.var_21 = (rb_state17.var_21) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_18) : (0x5b5a4f2f));
                rb_state17.var_22 = (rb_state17.var_22) + (((0x7719a673) - (rb_state17.var_17)) ^ (rb_state17.var_17));
                rb_state17.var_23 = (rb_state17.var_23) ^ (rb_state17.var_20);
                rb_state17.var_24 = (rb_state17.var_24) - (((rb_state17.var_21) == (0x0)) ? (rb_state17.var_22) : (0x7e7d1f21));
                rb_state17.var_25 = (rb_state17.var_25) + (rb_state17.var_23);
                rb_state17.var_26 = (rb_state17.var_26) - (rb_state17.var_24);
                rb_state17.var_27 = (rb_state17.var_27) - (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_26) : (0x186b0cad));
                rb_state17.var_16 = (rb_state17.var_16) ^ (rb_state17.var_27);
                rb_state17.var_15 = rb_state17.var_16;
                pthread_mutex_unlock(&(rb_state17.lock_33));
            }
            if ((rb_state17.var_14) == (0x1)) {
                if (!((rb_state17.var_15) == (rb_state17.var_16))) {
                    racebench_trigger(17);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_2) == (0x26b33938)) {
                rb_state19.var_5 = 0x1e61f1fb;
                rb_state19.var_6 = (rb_state19.var_6) ^ (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_5) : (0x66d97331));
                rb_state19.var_7 = (rb_state19.var_7) + (rb_state19.var_6);
                rb_state19.var_8 = (rb_state19.var_8) + (0x25c4c23e);
                rb_state19.var_9 = (rb_state19.var_9) - (((((0x96ac86fd) ^ (0x1fc3403e)) ^ (rb_state19.var_4)) - (0x45109f22)) + (rb_state19.var_6));
                rb_state19.var_10 = (rb_state19.var_10) - (((((0x23dbdccb) + (rb_state19.var_7)) ^ (rb_state19.var_7)) ^ (rb_state19.var_8)) - (0x4c47051));
                rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_9);
                rb_state19.var_12 = (rb_state19.var_12) + (((0xce6deac8) + (rb_state19.var_10)) - (rb_state19.var_8));
                rb_state19.var_13 = (rb_state19.var_13) ^ (((0xc20f4b02) ^ (rb_state19.var_11)) + (0xcd49627b));
                rb_state19.var_14 = (rb_state19.var_14) + (((((0x43ee720b) + (rb_state19.var_12)) ^ (rb_state19.var_9)) ^ (rb_state19.var_10)) ^ (rb_state19.var_13));
                rb_state19.var_3 = (rb_state19.var_3) - (rb_state19.var_14);
            }
            #endif
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0x9466629c) + (rb_state15.var_0));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_11 = (rb_state17.var_11) + (1744309 < rb_input_size ? (uint32_t)rb_input[1744309] : 0xe34b0d3f);
        #endif
        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x2d26b8a0)) {
            pthread_mutex_lock(&(rb_state1.lock_31));
            rb_state1.var_5 = 0xe764fbe6;
            rb_state1.var_6 = 0xfad523a4;
            rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x6c93fcc5));
            rb_state1.var_8 = (rb_state1.var_8) - (((0xe15b17d1) - (rb_state1.var_6)) + (rb_state1.var_5));
            rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_5);
            rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0x95d42568));
            rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_8);
            rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_9) : (0xfc068254));
            rb_state1.var_13 = (rb_state1.var_13) + (((0x102ffcf1) ^ (0x8bc035be)) - (rb_state1.var_10));
            rb_state1.var_14 = (rb_state1.var_14) ^ (((((0x1d417cab) + (0x58fc8d0e)) - (rb_state1.var_8)) - (rb_state1.var_11)) - (rb_state1.var_12));
            rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_13) : (0xe1ce2001));
            rb_state1.var_16 = (rb_state1.var_16) + (((0x2a4bd059) - (rb_state1.var_10)) - (rb_state1.var_14));
            rb_state1.var_17 = (rb_state1.var_17) + (((0xefbe8926) ^ (rb_state1.var_11)) ^ (rb_state1.var_15));
            rb_state1.var_18 = (rb_state1.var_18) ^ (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_17) : (0x855f9b64));
            rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_18);
            rb_state1.var_3 = rb_state1.var_4;
            pthread_mutex_unlock(&(rb_state1.lock_31));
        }
        if ((rb_state1.var_0) == (0x2d26b8a0)) {
            if (!((rb_state1.var_3) == (rb_state1.var_4))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (0x6de80cda);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_1) : (0xca5a1726));
        #endif
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (3589572 < rb_input_size ? (uint32_t)rb_input[3589572] : 0x2575ae0);
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_1) == (0xb693fc4e)) {
            pthread_mutex_lock(&(rb_state10.lock_25));
            rb_state10.var_5 = 0x8c613d0c;
            rb_state10.var_6 = 0x5cfcf961;
            rb_state10.var_7 = (rb_state10.var_7) ^ (((((0x424e022f) ^ (rb_state10.var_4)) ^ (rb_state10.var_6)) + (0x9e38cb89)) - (rb_state10.var_3));
            rb_state10.var_8 = (rb_state10.var_8) ^ (0x3440f135);
            rb_state10.var_9 = (rb_state10.var_9) - (rb_state10.var_5);
            rb_state10.var_10 = (rb_state10.var_10) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0x43b6d16));
            rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_9) : (0xe4cc83c2));
            rb_state10.var_12 = (rb_state10.var_12) + (rb_state10.var_10);
            rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_11) : (0xb66a34b));
            rb_state10.var_14 = (rb_state10.var_14) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0xb0dfb201));
            rb_state10.var_4 = (rb_state10.var_4) + (rb_state10.var_14);
            rb_state10.var_3 = rb_state10.var_4;
            pthread_mutex_unlock(&(rb_state10.lock_25));
        }
        if ((rb_state10.var_1) == (0xb693fc4e)) {
            if (!((rb_state10.var_3) == (rb_state10.var_4))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + (0x15d3dee7);
        #endif
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_3);
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_2) == (0x8ed029d4)) {
            pthread_mutex_lock(&(rb_state18.lock_14));
            rb_state18.var_5 = 0x7450938b;
            rb_state18.var_6 = 0x1f07eef7;
            rb_state18.var_7 = 0x85bdd05d;
            rb_state18.var_8 = (rb_state18.var_8) ^ (((((0x8a1a431a) + (0x10234eeb)) ^ (rb_state18.var_3)) + (rb_state18.var_5)) ^ (rb_state18.var_7));
            rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_8) : (0x6c5f5f88));
            rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_9);
            rb_state18.var_3 = rb_state18.var_4;
            pthread_mutex_unlock(&(rb_state18.lock_14));
        }
        if ((rb_state18.var_2) == (0xd70e25c2)) {
            if (!((rb_state18.var_3) == (rb_state18.var_4))) {
                racebench_trigger(18);
            }
        }
        #endif
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) + (1180146 < rb_input_size ? (uint32_t)rb_input[1180146] : 0xacecbdda);
        rb_state5.var_2 = (rb_state5.var_2) - ((0x688f1396) + (0x30455a));
        if ((rb_state5.var_3) == (0x458d0c79)) {
            pthread_mutex_lock(&(rb_state5.lock_14));
            rb_state5.var_4 = rb_state5.var_0;
            pthread_mutex_unlock(&(rb_state5.lock_14));
        }
        if ((rb_state5.var_3) == (0x458d0c79)) {
            if (!((rb_state5.var_0) == (rb_state5.var_4))) {
                racebench_trigger(5);
            }
        }
        #endif
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0xa6866dcf)) {
            rb_state2.var_14 = 0x32d11d5c;
            rb_state2.var_15 = 0xed58170c;
            rb_state2.var_16 = (rb_state2.var_16) ^ (rb_state2.var_15);
            rb_state2.var_17 = (rb_state2.var_17) ^ (((((0x758f12c2) + (rb_state2.var_10)) - (rb_state2.var_9)) - (rb_state2.var_14)) + (0xc0dc2811));
            rb_state2.var_18 = (rb_state2.var_18) - (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_17) : (0x4b64251e));
            rb_state2.var_13 = (rb_state2.var_13) ^ (rb_state2.var_18);
            rb_state2.var_3 = rb_state2.var_13;
        }
        if ((rb_state2.var_0) == (0xa6866dcf)) {
            pthread_mutex_lock(&(rb_state2.lock_32));
            rb_state2.var_20 = 0xcfa33f2d;
            rb_state2.var_21 = 0xff3f32f3;
            rb_state2.var_22 = (rb_state2.var_22) ^ (0x36b9e701);
            rb_state2.var_23 = (rb_state2.var_23) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_20) : (0x470bce41));
            rb_state2.var_24 = (rb_state2.var_24) - (rb_state2.var_21);
            rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0x2403fb3));
            rb_state2.var_26 = (rb_state2.var_26) + (((0x61fbe0d5) + (0x3e83b5fd)) ^ (rb_state2.var_24));
            rb_state2.var_27 = (rb_state2.var_27) ^ (((0xc6f24879) ^ (0x4b64cffe)) - (rb_state2.var_25));
            rb_state2.var_28 = (rb_state2.var_28) + (rb_state2.var_26);
            rb_state2.var_29 = (rb_state2.var_29) - (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_27) : (0x2ce465fc));
            rb_state2.var_30 = (rb_state2.var_30) - (((0x74080e88) ^ (rb_state2.var_28)) + (0xcc0bd5ef));
            rb_state2.var_31 = (rb_state2.var_31) - (((((0x19fd5d8d) ^ (rb_state2.var_13)) - (rb_state2.var_14)) ^ (rb_state2.var_30)) ^ (rb_state2.var_29));
            rb_state2.var_19 = (rb_state2.var_19) + (rb_state2.var_31);
            rb_state2.var_3 = rb_state2.var_19;
            pthread_mutex_unlock(&(rb_state2.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_2) == (0xbbaa1f0d)) {
            rb_state15.var_3 = (rb_state15.var_3) - (rb_state15.var_4);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_0);
        #endif
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) - (622924 < rb_input_size ? (uint32_t)rb_input[622924] : 0x2a30d3e4);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_2) == (0xfffffc74)) ? (rb_state3.var_1) : (0x5a314501));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_3 = (rb_state5.var_3) + (0xb31cb72);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_2);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_6);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + ((0xcb2ffe) ^ (0x8fb0f942));
            #endif
            temp_p = VAR[mol].F[FORCES][dir];
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (0x8fe8d0d6);
            rb_state2.var_2 = (rb_state2.var_2) + (1157953 < rb_input_size ? (uint32_t)rb_input[1157953] : 0x12de8b53);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) + ((0xb3b70eae) + (rb_state4.var_0));
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) ^ (0x230aabcc);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) ^ (1706054 < rb_input_size ? (uint32_t)rb_input[1706054] : 0x4e69a7bc);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_3);
            rb_state15.var_1 = (rb_state15.var_1) + (0x378a8672);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
            #endif
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_1) == (0xdf542ac6)) {
                rb_state0.var_4 = 0x8e4844c8;
                rb_state0.var_5 = 0x45557518;
                rb_state0.var_6 = 0x8c91b761;
                rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_3) : (0x4906241f));
                rb_state0.var_8 = (rb_state0.var_8) - (rb_state0.var_5);
                rb_state0.var_9 = (rb_state0.var_9) - (((0xfe5c15fc) + (0xdbf0e16a)) ^ (rb_state0.var_6));
                rb_state0.var_10 = (rb_state0.var_10) + (rb_state0.var_4);
                rb_state0.var_11 = (rb_state0.var_11) ^ (rb_state0.var_7);
                rb_state0.var_12 = (rb_state0.var_12) + (((0x24663b18) + (rb_state0.var_5)) - (rb_state0.var_8));
                rb_state0.var_13 = (rb_state0.var_13) + (((((0xa538b7bf) - (0xf48cf707)) - (rb_state0.var_6)) ^ (rb_state0.var_10)) ^ (rb_state0.var_9));
                rb_state0.var_14 = (rb_state0.var_14) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_11) : (0xe8fae7ad));
                rb_state0.var_15 = (rb_state0.var_15) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_12) : (0x19bb6cc));
                rb_state0.var_16 = (rb_state0.var_16) - (rb_state0.var_13);
                rb_state0.var_17 = (rb_state0.var_17) - (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_15) : (0x63620dce));
                rb_state0.var_18 = (rb_state0.var_18) - (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_17) : (0x9dcb3d4));
                rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_18);
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) - ((0xba6a9095) + (0xcc48667a));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (1705289 < rb_input_size ? (uint32_t)rb_input[1705289] : 0x598b302a);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) + (1099027 < rb_input_size ? (uint32_t)rb_input[1099027] : 0xd88e2538);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) ^ (0xbccf51f2);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (627703 < rb_input_size ? (uint32_t)rb_input[627703] : 0xb423f3ea);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) - (0x25afc5c1);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) + ((0x8a04de17) + (0x228d3fa0));
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_2) == (0x26b33938)) {
                rb_state19.var_4 = rb_state19.var_3;
            }
            if ((rb_state19.var_2) == (0x26b33938)) {
                if (!((rb_state19.var_3) == (rb_state19.var_4))) {
                    racebench_trigger(19);
                }
            }
            #endif
            temp_p[H1] = T1;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) ^ (0x7a45ef87);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (0x1d29b6ed);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) + (0xb974f8c8);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) ^ (3535883 < rb_input_size ? (uint32_t)rb_input[3535883] : 0x787fbcf2);
            if ((rb_state17.var_14) == (0x1)) {
                pthread_mutex_lock(&(rb_state17.lock_33));
                rb_state17.var_28 = 0x9297396a;
                rb_state17.var_29 = (rb_state17.var_29) - (((((0x4b8e8ba6) ^ (rb_state17.var_20)) - (rb_state17.var_21)) ^ (rb_state17.var_19)) ^ (rb_state17.var_28));
                rb_state17.var_30 = (rb_state17.var_30) ^ (1894588 < rb_input_size ? (uint32_t)rb_input[1894588] : 0x6fbe785e);
                rb_state17.var_31 = (rb_state17.var_31) ^ (((((0x445646d8) - (rb_state17.var_29)) - (rb_state17.var_22)) + (rb_state17.var_18)) - (rb_state17.var_23));
                rb_state17.var_32 = (rb_state17.var_32) + (((((0x76d4bf6d) + (0x2ebfc3ad)) + (rb_state17.var_31)) ^ (rb_state17.var_30)) + (0x5dcace86));
                rb_state17.var_15 = (rb_state17.var_15) + (rb_state17.var_32);
                pthread_mutex_unlock(&(rb_state17.lock_33));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (0xd893ad54);
            rb_state18.var_2 = (rb_state18.var_2) ^ (((rb_state18.var_2) == (0xd26bd74c)) ? (rb_state18.var_2) : (0x4c26508e));
            #endif
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_2) == (0x43695a9a)) ? (rb_state0.var_1) : (0x49e40fc));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x5c981efc)) ? (rb_state2.var_0) : (0x73627b28));
            #endif
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_2) == (0x2ef9af06)) {
                pthread_mutex_lock(&(rb_state4.lock_23));
                rb_state4.var_5 = 0xbaaf5a34;
                rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x294eec26));
                rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_3) : (0xe53039bd));
                rb_state4.var_8 = (rb_state4.var_8) ^ (((0xad78f379) ^ (rb_state4.var_6)) ^ (rb_state4.var_6));
                rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_7) : (0x89a240fd));
                rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0xc8984314));
                rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_10);
                rb_state4.var_3 = rb_state4.var_4;
                pthread_mutex_unlock(&(rb_state4.lock_23));
            }
            if ((rb_state4.var_2) == (0x25fa0a2c)) {
                rb_state4.var_24 = !((rb_state4.var_3) == (rb_state4.var_4));
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_3) == (0x458d0c79)) {
                rb_state5.var_5 = 0xb7ab1b48;
                rb_state5.var_6 = 0x9aad5666;
                rb_state5.var_7 = (rb_state5.var_7) ^ (((0x8d168d1c) ^ (rb_state5.var_5)) ^ (rb_state5.var_5));
                rb_state5.var_8 = (rb_state5.var_8) - (940301 < rb_input_size ? (uint32_t)rb_input[940301] : 0x9fe0015a);
                rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_6) : (0x5a0cdb54));
                rb_state5.var_10 = (rb_state5.var_10) + (((((0xc1532a11) + (rb_state5.var_4)) - (0x2210a8cd)) ^ (0x7d00c0ff)) - (rb_state5.var_7));
                rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0xf1f79b77));
                rb_state5.var_12 = (rb_state5.var_12) ^ (((0x5a6edafc) ^ (rb_state5.var_10)) ^ (0x3ffa53be));
                rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_12) : (0x2fbf20f6));
                rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_13);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_3 = (rb_state6.var_3) ^ (1533334 < rb_input_size ? (uint32_t)rb_input[1533334] : 0xb09e04e5);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (0x65652734);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_3);
            if ((rb_state13.var_1) == (0xd1944897)) {
                pthread_mutex_lock(&(rb_state13.lock_22));
                rb_state13.var_4 = rb_state13.var_2;
                pthread_mutex_unlock(&(rb_state13.lock_22));
            }
            if ((rb_state13.var_1) == (0xd1944897)) {
                if (!((rb_state13.var_2) == (rb_state13.var_4))) {
                    racebench_trigger(13);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) - (2226594 < rb_input_size ? (uint32_t)rb_input[2226594] : 0xda0ac0ea);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_2);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) + ((0xfc9dea8f) ^ (0x8f1e48fa));
            #endif
            temp_p[H2] = T2;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_2) == (0xf29a1262)) ? (rb_state1.var_1) : (0xdf7b5abb));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_18 = (rb_state4.var_18) - ((0x2e67d18) - (rb_state4.var_19));
            rb_state4.var_20 = (rb_state4.var_20) + (609954 < rb_input_size ? (uint32_t)rb_input[609954] : 0x25d97b5b);
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_2) == (0xa363e64b)) {
                rb_state7.var_5 = 0xad20baff;
                rb_state7.var_6 = (rb_state7.var_6) ^ (1637221 < rb_input_size ? (uint32_t)rb_input[1637221] : 0x7a02867d);
                rb_state7.var_7 = (rb_state7.var_7) + (((((0xbf27311f) ^ (rb_state7.var_5)) ^ (rb_state7.var_5)) + (rb_state7.var_4)) ^ (rb_state7.var_6));
                rb_state7.var_8 = (rb_state7.var_8) ^ (rb_state7.var_6);
                rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_7) : (0xfe5b8c35));
                rb_state7.var_10 = (rb_state7.var_10) - (rb_state7.var_8);
                rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_9) : (0xee1fe389));
                rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x2d095d88));
                rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_11);
                rb_state7.var_14 = (rb_state7.var_14) ^ (((((0x3b9bde93) - (rb_state7.var_12)) ^ (rb_state7.var_13)) + (rb_state7.var_10)) ^ (0x7fbdbded));
                rb_state7.var_3 = (rb_state7.var_3) ^ (rb_state7.var_14);
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (1094379 < rb_input_size ? (uint32_t)rb_input[1094379] : 0x8d9e7f7a);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_2) : (0xa42b334f));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) ^ (0xf3b4ae26);
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