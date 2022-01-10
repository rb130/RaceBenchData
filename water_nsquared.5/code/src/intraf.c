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
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (3004011 < rb_input_size ? (uint32_t)rb_input[3004011] : 0x9bf2f88c);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_3);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_2) == (0x176670f5)) {
            if ((rb_state14.var_5) != (0x0)) {
                if (!((rb_state14.var_5) == (rb_state14.var_24))) {
                    racebench_trigger(14);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) - (2657814 < rb_input_size ? (uint32_t)rb_input[2657814] : 0xf3e23f90);
        #endif
        SUM = 0.0;
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0xc8ff5ba1)) {
            if ((rb_state6.var_5) != (0x0)) {
                if (!((rb_state6.var_5) == (rb_state6.var_25))) {
                    racebench_trigger(6);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - ((0xd0de79b3) + (rb_state12.var_2));
        #endif
        R1 = 0.0;
        R2 = 0.0;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xa82e1425));
            rb_state0.var_3 = (rb_state0.var_3) ^ (68362 < rb_input_size ? (uint32_t)rb_input[68362] : 0x59efdb23);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) + (0x94425d69);
            rb_state2.var_3 = (rb_state2.var_3) + (0xfe815412);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_3 = (rb_state7.var_3) - (0xb5f6d612);
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_1) == (0x43832bd2)) {
                rb_state9.var_15 = 0x94773465;
                rb_state9.var_16 = 0x234cb6a5;
                rb_state9.var_17 = 0xd7b1f19b;
                rb_state9.var_18 = (rb_state9.var_18) + (0x244fa7e5);
                rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_17) : (0xb547c19e));
                rb_state9.var_20 = (rb_state9.var_20) - (((0xa05f0a04) - (rb_state9.var_12)) + (rb_state9.var_16));
                rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0x8ad21376));
                rb_state9.var_22 = (rb_state9.var_22) ^ (((((0xda3cc357) - (rb_state9.var_21)) - (rb_state9.var_20)) + (rb_state9.var_13)) - (0x6c1fe9be));
                rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_22);
                rb_state9.var_5 = rb_state9.var_14;
            }
            if ((rb_state9.var_1) == (0x43832bd2)) {
                pthread_mutex_lock(&(rb_state9.lock_34));
                rb_state9.var_24 = 0x2832244c;
                rb_state9.var_25 = (rb_state9.var_25) + (((0xb83e1e37) ^ (0x23c6eca0)) - (rb_state9.var_15));
                rb_state9.var_26 = (rb_state9.var_26) + (((0x62fedf3b) + (rb_state9.var_16)) ^ (rb_state9.var_14));
                rb_state9.var_27 = (rb_state9.var_27) ^ (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_17) : (0xc0a1aca4));
                rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_24) : (0x96c13352));
                rb_state9.var_29 = (rb_state9.var_29) - (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_26) : (0x4d0bd6f7));
                rb_state9.var_30 = (rb_state9.var_30) ^ (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0x83278cbe));
                rb_state9.var_31 = (rb_state9.var_31) + (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_29) : (0x53ad4d6f));
                rb_state9.var_32 = (rb_state9.var_32) - (rb_state9.var_30);
                rb_state9.var_33 = (rb_state9.var_33) - (((rb_state9.var_31) == (0x0)) ? (rb_state9.var_32) : (0xd0668e92));
                rb_state9.var_23 = (rb_state9.var_23) - (rb_state9.var_33);
                rb_state9.var_5 = rb_state9.var_23;
                pthread_mutex_unlock(&(rb_state9.lock_34));
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xd70c8084));
            rb_state12.var_0 = (rb_state12.var_0) + (0x81116810);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) - (835455 < rb_input_size ? (uint32_t)rb_input[835455] : 0xbe214393);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (0x3441f1bf);
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0xf666dba4)) {
                rb_state19.var_11 = 0x93850ab2;
                rb_state19.var_12 = 0x846d2bb5;
                rb_state19.var_13 = (rb_state19.var_13) ^ ((0x573c37db) - (0xf85d8d7e));
                rb_state19.var_14 = (rb_state19.var_14) - (((((0x909d4177) - (rb_state19.var_8)) - (rb_state19.var_12)) - (rb_state19.var_11)) - (0x75a4603d));
                rb_state19.var_15 = (rb_state19.var_15) + (((0x8320fe8b) + (rb_state19.var_9)) - (rb_state19.var_7));
                rb_state19.var_16 = (rb_state19.var_16) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_14) : (0x204855a6));
                rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_15) : (0x81b92f59));
                rb_state19.var_18 = (rb_state19.var_18) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_17) : (0xed9bc94f));
                rb_state19.var_4 = (rb_state19.var_4) + (rb_state19.var_18);
            }
            #endif
            temp_p = VAR[mol].F[DISP][dir];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_2 = (rb_state2.var_2) ^ (3361702 < rb_input_size ? (uint32_t)rb_input[3361702] : 0x1c86b245);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x479ebf5b));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) ^ ((0x84b8eb3c) - (0x96feb726));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0x48063c51));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_4 = (rb_state13.var_4) + (0xe97a00c);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) - (0x9864de5b);
            #endif
            VAR[mol].VM[dir] = C1 * temp_p[O] + C2 * (temp_p[H1] + temp_p[H2]);
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) - (0xa68dce7e);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) ^ (1770668 < rb_input_size ? (uint32_t)rb_input[1770668] : 0xa3cf52a4);
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0xbad3d14d));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_2 = (rb_state14.var_2) - ((0xea5adbc0) - (0xb41d03fb));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) ^ ((0x551b4a55) + (rb_state17.var_1));
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x75aa0b1a)) {
                rb_state18.var_14 = 0x1f95b5a2;
                rb_state18.var_15 = 0xa0e0652;
                rb_state18.var_16 = (rb_state18.var_16) + (2612129 < rb_input_size ? (uint32_t)rb_input[2612129] : 0x8194c054);
                rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0x6cbe6955));
                rb_state18.var_18 = (rb_state18.var_18) - (((0xdd5c7803) + (0x54dbbba6)) - (rb_state18.var_12));
                rb_state18.var_19 = (rb_state18.var_19) + (((0xd2a2a739) ^ (rb_state18.var_16)) - (rb_state18.var_13));
                rb_state18.var_20 = (rb_state18.var_20) ^ (((0xe5da4916) - (rb_state18.var_17)) - (0xd2c2f7de));
                rb_state18.var_21 = (rb_state18.var_21) ^ (((0xe09317fd) ^ (rb_state18.var_14)) ^ (rb_state18.var_18));
                rb_state18.var_22 = (rb_state18.var_22) - (rb_state18.var_19);
                rb_state18.var_23 = (rb_state18.var_23) ^ (rb_state18.var_20);
                rb_state18.var_24 = (rb_state18.var_24) + (rb_state18.var_21);
                rb_state18.var_25 = (rb_state18.var_25) - (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_23) : (0x2b75b310));
                rb_state18.var_26 = (rb_state18.var_26) - (((((0x1108ed40) ^ (rb_state18.var_15)) + (rb_state18.var_25)) - (0x15a48933)) ^ (rb_state18.var_24));
                rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_26);
            }
            #endif
            VR1[dir] = temp_p[O] - temp_p[H1];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) ^ (3623817 < rb_input_size ? (uint32_t)rb_input[3623817] : 0xf6ae03a6);
            rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_2) == (0x34)) ? (rb_state0.var_1) : (0x926e67a1));
            rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_1);
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0x2d7f1f8a));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_2) : (0x7f0b119f));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_2 = (rb_state4.var_2) ^ (0x61d88df1);
            #endif
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0x9a90b148)) {
                if ((rb_state5.var_5) != (0x0)) {
                    if (!((rb_state5.var_5) == (rb_state5.var_15))) {
                        racebench_trigger(5);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_3);
            #endif
            #ifdef RACEBENCH_BUG_11
            if ((rb_state11.var_4) == (0x75cfdb20)) {
                usleep(100);
            }
            if ((rb_state11.var_4) == (0x75cfdb20)) {
                rb_state11.var_19 = !((rb_state11.var_6) == (rb_state11.var_5));
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (1861220 < rb_input_size ? (uint32_t)rb_input[1861220] : 0x4edc7928);
            #endif
            R1 += VR1[dir] * VR1[dir];
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_2) == (0x4d75f691)) {
                rb_state0.var_15 = 0x7e621088;
                rb_state0.var_16 = 0x38a2d33;
                rb_state0.var_17 = (rb_state0.var_17) ^ (((((0xd06cebef) ^ (rb_state0.var_5)) ^ (rb_state0.var_16)) + (0x7e24860a)) - (rb_state0.var_6));
                rb_state0.var_18 = (rb_state0.var_18) + (rb_state0.var_15);
                rb_state0.var_19 = (rb_state0.var_19) ^ (rb_state0.var_17);
                rb_state0.var_20 = (rb_state0.var_20) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_18) : (0x7acdce51));
                rb_state0.var_21 = (rb_state0.var_21) ^ (((0xf06a5d15) + (rb_state0.var_19)) ^ (0xaf71bc56));
                rb_state0.var_22 = (rb_state0.var_22) + (((((0xe41482ec) - (0x39d43bc2)) - (rb_state0.var_21)) ^ (rb_state0.var_20)) ^ (0x6fadbedc));
                rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_22);
                rb_state0.var_4 = rb_state0.var_14;
            }
            if ((rb_state0.var_2) == (0x4d75f691)) {
                if ((rb_state0.var_4) != (0x0)) {
                    if (!((rb_state0.var_4) == (rb_state0.var_23))) {
                        racebench_trigger(0);
                    }
                }
            }
            if ((rb_state0.var_2) == (0x4d75f691)) {
                pthread_mutex_lock(&(rb_state0.lock_29));
                rb_state0.var_24 = 0x84ecbb2a;
                rb_state0.var_25 = 0x8d5969ae;
                rb_state0.var_26 = (rb_state0.var_26) - (rb_state0.var_24);
                rb_state0.var_27 = (rb_state0.var_27) - (((((0xb2263a98) - (rb_state0.var_25)) + (0xd79dd75a)) + (0xf21f1fbf)) - (rb_state0.var_8));
                rb_state0.var_28 = (rb_state0.var_28) ^ (((rb_state0.var_26) == (0x0)) ? (rb_state0.var_27) : (0x61417aa8));
                rb_state0.var_23 = (rb_state0.var_23) - (rb_state0.var_28);
                rb_state0.var_4 = rb_state0.var_23;
                pthread_mutex_unlock(&(rb_state0.lock_29));
            }
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_1) : (0xa05b0120));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) - (rb_state3.var_2);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_1) : (0xfb868039));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) - ((0xb4551bf8) - (rb_state8.var_1));
            if ((rb_state8.var_0) == (0x4acb5fe)) {
                rb_state8.var_14 = 0xfd7ebc26;
                rb_state8.var_15 = (rb_state8.var_15) + (rb_state8.var_12);
                rb_state8.var_16 = (rb_state8.var_16) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x489c9217));
                rb_state8.var_17 = (rb_state8.var_17) - (0xc32755c2);
                rb_state8.var_18 = (rb_state8.var_18) + (((((0x8b196275) - (rb_state8.var_13)) ^ (rb_state8.var_14)) ^ (rb_state8.var_15)) + (0xbe5ba17a));
                rb_state8.var_19 = (rb_state8.var_19) - (((((0x216f337) - (rb_state8.var_14)) ^ (rb_state8.var_17)) ^ (rb_state8.var_16)) - (0x359871e0));
                rb_state8.var_20 = (rb_state8.var_20) + (((0x1582b95f) ^ (rb_state8.var_18)) + (rb_state8.var_15));
                rb_state8.var_21 = (rb_state8.var_21) ^ (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_19) : (0xe0724641));
                rb_state8.var_22 = (rb_state8.var_22) + (((0xfac15a2f) - (0x7705e4a2)) + (rb_state8.var_20));
                rb_state8.var_23 = (rb_state8.var_23) - (((0x6633d56d) - (0xdaac483e)) + (rb_state8.var_21));
                rb_state8.var_24 = (rb_state8.var_24) ^ (((rb_state8.var_22) == (0x0)) ? (rb_state8.var_23) : (0x423ee156));
                rb_state8.var_13 = (rb_state8.var_13) + (rb_state8.var_24);
                rb_state8.var_5 = rb_state8.var_13;
            }
            if ((rb_state8.var_0) == (0x4acb5fe)) {
                pthread_mutex_lock(&(rb_state8.lock_34));
                rb_state8.var_26 = 0x6531d430;
                rb_state8.var_27 = 0xa275e2fa;
                rb_state8.var_28 = (rb_state8.var_28) + (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_18) : (0x607e4aee));
                rb_state8.var_29 = (rb_state8.var_29) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_17) : (0xdf8de663));
                rb_state8.var_30 = (rb_state8.var_30) ^ (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_28) : (0x2ca7513f));
                rb_state8.var_31 = (rb_state8.var_31) ^ (rb_state8.var_29);
                rb_state8.var_32 = (rb_state8.var_32) ^ (rb_state8.var_30);
                rb_state8.var_33 = (rb_state8.var_33) ^ (((((0xbc62700f) + (rb_state8.var_32)) + (rb_state8.var_31)) - (rb_state8.var_20)) + (0x76ee7a1f));
                rb_state8.var_25 = (rb_state8.var_25) ^ (rb_state8.var_33);
                rb_state8.var_5 = rb_state8.var_25;
                pthread_mutex_unlock(&(rb_state8.lock_34));
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) ^ (2647393 < rb_input_size ? (uint32_t)rb_input[2647393] : 0x3dd68a4f);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_0);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_1);
            #endif
            VR2[dir] = temp_p[O] - temp_p[H2];
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_3) == (0x1df0547c)) {
                if ((rb_state2.var_5) != (0x0)) {
                    if (!((rb_state2.var_5) == (rb_state2.var_22))) {
                        racebench_trigger(2);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0x796db04f));
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_1);
            if ((rb_state4.var_2) == (0xfe983393)) {
                rb_state4.var_5 = 0xd49c662c;
                rb_state4.var_6 = 0x3d8c2f98;
                rb_state4.var_7 = 0x4b42edc5;
                rb_state4.var_8 = (rb_state4.var_8) + ((0x3ca972a6) + (0x7fdafa6f));
                rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_6) : (0xf3932a7));
                rb_state4.var_10 = (rb_state4.var_10) + (rb_state4.var_5);
                rb_state4.var_11 = (rb_state4.var_11) + (rb_state4.var_7);
                rb_state4.var_12 = (rb_state4.var_12) - (((((0x2099264e) ^ (rb_state4.var_5)) - (rb_state4.var_9)) - (rb_state4.var_6)) - (rb_state4.var_8));
                rb_state4.var_13 = (rb_state4.var_13) ^ (((((0xd146197d) + (rb_state4.var_10)) ^ (rb_state4.var_11)) + (0xfb81b925)) ^ (rb_state4.var_7));
                rb_state4.var_14 = (rb_state4.var_14) ^ (((0xb4c771e1) - (rb_state4.var_12)) ^ (rb_state4.var_8));
                rb_state4.var_15 = (rb_state4.var_15) ^ (((0x9f6637f3) ^ (rb_state4.var_13)) ^ (0x807f14c2));
                rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_15) : (0x1c39dd6a));
                rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_16);
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) + (3269358 < rb_input_size ? (uint32_t)rb_input[3269358] : 0x4d1c2160);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_4);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - ((0xd8931022) + (rb_state7.var_1));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_2) : (0x567835df));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (0xa7c36af2);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_2 = (rb_state14.var_2) + (0x45401270);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) + (2732719 < rb_input_size ? (uint32_t)rb_input[2732719] : 0x6dd10ad4);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) - ((0x842754f8) - (0xa694edfc));
            rb_state16.var_3 = (rb_state16.var_3) + ((0x8bf1a0dc) - (0x1cae24fa));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) + (0x34292272);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x5741d94));
            #endif
            R2 += VR2[dir] * VR2[dir];
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_3 = (rb_state4.var_3) ^ (2927309 < rb_input_size ? (uint32_t)rb_input[2927309] : 0x965e4b3c);
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_2) == (0xc8ff5ba1)) {
                rb_state6.var_17 = 0xe409c078;
                rb_state6.var_18 = 0xce2211e4;
                rb_state6.var_19 = (rb_state6.var_19) - (((((0x8fc734d0) - (rb_state6.var_7)) ^ (0x3d688896)) + (rb_state6.var_18)) + (0xfe07639d));
                rb_state6.var_20 = (rb_state6.var_20) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0x96615d75));
                rb_state6.var_21 = (rb_state6.var_21) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_17) : (0xd1d6b9e5));
                rb_state6.var_22 = (rb_state6.var_22) - (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0xe397b240));
                rb_state6.var_23 = (rb_state6.var_23) - (((0x54bff7df) - (0x3d71cd26)) ^ (rb_state6.var_21));
                rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_22) == (0x0)) ? (rb_state6.var_23) : (0x1446d29b));
                rb_state6.var_16 = (rb_state6.var_16) ^ (rb_state6.var_24);
                rb_state6.var_5 = rb_state6.var_16;
            }
            if ((rb_state6.var_2) == (0xc8ff5ba1)) {
                pthread_mutex_lock(&(rb_state6.lock_37));
                rb_state6.var_26 = 0x7c9c262a;
                rb_state6.var_27 = 0x553b0cff;
                rb_state6.var_28 = 0xbbf4c345;
                rb_state6.var_29 = (rb_state6.var_29) ^ (((0xb912d572) + (rb_state6.var_27)) ^ (rb_state6.var_11));
                rb_state6.var_30 = (rb_state6.var_30) - (rb_state6.var_26);
                rb_state6.var_31 = (rb_state6.var_31) ^ (rb_state6.var_28);
                rb_state6.var_32 = (rb_state6.var_32) - (((0xaaa52188) ^ (rb_state6.var_12)) + (rb_state6.var_29));
                rb_state6.var_33 = (rb_state6.var_33) - (rb_state6.var_30);
                rb_state6.var_34 = (rb_state6.var_34) + (((rb_state6.var_31) == (0x0)) ? (rb_state6.var_32) : (0x44f6e263));
                rb_state6.var_35 = (rb_state6.var_35) ^ (rb_state6.var_33);
                rb_state6.var_36 = (rb_state6.var_36) - (((((0x251b984c) ^ (rb_state6.var_34)) ^ (0xca34ffd3)) - (rb_state6.var_35)) - (rb_state6.var_13));
                rb_state6.var_25 = (rb_state6.var_25) - (rb_state6.var_36);
                rb_state6.var_5 = rb_state6.var_25;
                pthread_mutex_unlock(&(rb_state6.lock_37));
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_0);
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0x4acb5fe)) {
                if ((rb_state8.var_5) != (0x0)) {
                    if (!((rb_state8.var_5) == (rb_state8.var_25))) {
                        racebench_trigger(8);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_3 = (rb_state9.var_3) - (0x61d67220);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0xa0899322));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0x3b131af0)) {
                pthread_mutex_lock(&(rb_state12.lock_32));
                rb_state12.var_5 = 0x762b2ccf;
                rb_state12.var_6 = 0x4fd7dd7a;
                rb_state12.var_7 = (rb_state12.var_7) + (rb_state12.var_4);
                rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_5) : (0x31361857));
                rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_5);
                rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_6) : (0xe4a59830));
                rb_state12.var_11 = (rb_state12.var_11) + (((0x1198bf0d) ^ (rb_state12.var_7)) + (rb_state12.var_7));
                rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_8) : (0x3047ac05));
                rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_9) : (0x1650b362));
                rb_state12.var_14 = (rb_state12.var_14) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_10) : (0xdc1ca940));
                rb_state12.var_15 = (rb_state12.var_15) + (((((0x4170ffee) - (rb_state12.var_11)) ^ (rb_state12.var_12)) + (0x25a362cd)) - (rb_state12.var_11));
                rb_state12.var_16 = (rb_state12.var_16) - (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_14) : (0xb971878f));
                rb_state12.var_17 = (rb_state12.var_17) ^ (rb_state12.var_15);
                rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_16);
                rb_state12.var_19 = (rb_state12.var_19) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_17) : (0xed5d5c5c));
                rb_state12.var_20 = (rb_state12.var_20) ^ (rb_state12.var_18);
                rb_state12.var_21 = (rb_state12.var_21) - (((0xc1f9ba30) ^ (rb_state12.var_19)) ^ (0xe44835a0));
                rb_state12.var_22 = (rb_state12.var_22) + (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_21) : (0xe48395de));
                rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_22);
                rb_state12.var_3 = rb_state12.var_4;
                pthread_mutex_unlock(&(rb_state12.lock_32));
            }
            if ((rb_state12.var_1) == (0x3b131af0)) {
                if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                    racebench_trigger(12);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_0) == (0x96a55d4a)) {
                rb_state13.var_6 = 0x3e16bd06;
                rb_state13.var_7 = (rb_state13.var_7) ^ (0x7e56b44d);
                rb_state13.var_8 = (rb_state13.var_8) ^ (((0xa9191713) - (rb_state13.var_6)) + (rb_state13.var_6));
                rb_state13.var_9 = (rb_state13.var_9) ^ ((0xbc8bc194) ^ (0x5308e993));
                rb_state13.var_10 = (rb_state13.var_10) ^ (((0x257ebc69) + (0x5383674a)) ^ (rb_state13.var_5));
                rb_state13.var_11 = (rb_state13.var_11) - (((0x870c97a0) ^ (rb_state13.var_7)) - (rb_state13.var_7));
                rb_state13.var_12 = (rb_state13.var_12) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0xfd1615c5));
                rb_state13.var_13 = (rb_state13.var_13) - (rb_state13.var_10);
                rb_state13.var_14 = (rb_state13.var_14) ^ (rb_state13.var_11);
                rb_state13.var_15 = (rb_state13.var_15) ^ (((0x8f419258) + (0xd6160c2c)) - (rb_state13.var_12));
                rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_13) : (0xeb5b8811));
                rb_state13.var_17 = (rb_state13.var_17) + (((0xc4006e82) - (rb_state13.var_14)) - (rb_state13.var_9));
                rb_state13.var_18 = (rb_state13.var_18) + (((((0x1e58859a) ^ (rb_state13.var_15)) - (0x338289df)) ^ (rb_state13.var_16)) ^ (rb_state13.var_10));
                rb_state13.var_19 = (rb_state13.var_19) + (rb_state13.var_17);
                rb_state13.var_20 = (rb_state13.var_20) ^ (rb_state13.var_18);
                rb_state13.var_21 = (rb_state13.var_21) + (((0x833f5d90) + (0x8ee12922)) ^ (rb_state13.var_19));
                rb_state13.var_22 = (rb_state13.var_22) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_20) : (0x35af1d3e));
                rb_state13.var_23 = (rb_state13.var_23) + (((((0x1b059aa2) + (rb_state13.var_22)) ^ (rb_state13.var_21)) - (0x3cd8ec89)) + (0x1e653e7e));
                rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_23);
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_4 = (rb_state14.var_4) - (2663157 < rb_input_size ? (uint32_t)rb_input[2663157] : 0x12996e19);
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_2) == (0xffff6cf6)) {
                pthread_mutex_lock(&(rb_state15.lock_21));
                rb_state15.var_6 = 0x3db288de;
                rb_state15.var_7 = (rb_state15.var_7) ^ (0xdee7e04d);
                rb_state15.var_8 = (rb_state15.var_8) ^ (((((0x41855cd) + (rb_state15.var_6)) + (0x11a3b924)) - (rb_state15.var_4)) - (0x6bc24281));
                rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_7) : (0xda9c242d));
                rb_state15.var_10 = (rb_state15.var_10) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_9) : (0x440a9c94));
                rb_state15.var_5 = (rb_state15.var_5) + (rb_state15.var_10);
                rb_state15.var_4 = rb_state15.var_5;
                pthread_mutex_unlock(&(rb_state15.lock_21));
            }
            if ((rb_state15.var_2) == (0xffff6cf6)) {
                if (!((rb_state15.var_4) == (rb_state15.var_5))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_3 = (rb_state17.var_3) + (1768437 < rb_input_size ? (uint32_t)rb_input[1768437] : 0x23acdb4e);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) + (1375264 < rb_input_size ? (uint32_t)rb_input[1375264] : 0x2e2505c2);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + (0xf0ab1148);
            rb_state19.var_2 = (rb_state19.var_2) + (0x6219813b);
            rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_3);
            #endif
            SUM += VR1[dir] * VR2[dir];
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x41dbfd1));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - ((0x83914830) - (0x4c438ee0));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_3);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) ^ ((0x51cb5097) + (0xbdee876e));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_1);
        rb_state13.var_3 = (rb_state13.var_3) ^ ((0xd1c7fd1c) ^ (0x603e708e));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x848beb9b));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) + (0x1a115223);
        #endif
        R1 = sqrt(R1);
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_2) == (0x61d88df1)) ? (rb_state4.var_2) : (0x6ba613f8));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (544747 < rb_input_size ? (uint32_t)rb_input[544747] : 0x489c39b0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) + (0x20fd6f24);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (((rb_state17.var_1) == (0xaae4b5ab)) ? (rb_state17.var_0) : (0xdb81c6d2));
        #endif
        R2 = sqrt(R2);

        #ifdef RACEBENCH_BUG_8
        rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_16 = (rb_state11.var_16) - (rb_state11.var_17);
        #endif
        COS = SUM / (R1 * R2);
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_3 = (rb_state2.var_3) ^ ((0xab012b99) - (0xbe00b589));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0x5f5ff734);
        rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (0xc0485577);
        rb_state8.var_4 = (rb_state8.var_4) + ((0x3643d50b) + (rb_state8.var_4));
        #endif
        SIN = sqrt(ONE - COS * COS);
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + ((0x17c32400) - (0x4db1c807));
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_3) == (0x13fe2e03)) {
            rb_state7.var_6 = 0x1;
        }
        if ((rb_state7.var_3) == (0x13fe2e03)) {
            pthread_mutex_lock(&(rb_state7.lock_15));
            rb_state7.var_7 = 0x42830227;
            rb_state7.var_8 = 0x7338315b;
            rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x8c9559f7));
            rb_state7.var_10 = (rb_state7.var_10) ^ (((((0x3c938cbc) + (0xb7bbef5c)) + (0xb71153ed)) ^ (rb_state7.var_4)) - (rb_state7.var_8));
            rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_7);
            rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x98a55c78));
            rb_state7.var_13 = (rb_state7.var_13) + (((0x774c6c5b) - (rb_state7.var_7)) - (rb_state7.var_11));
            rb_state7.var_14 = (rb_state7.var_14) + (((((0x4ac3468e) - (rb_state7.var_12)) + (rb_state7.var_13)) + (rb_state7.var_9)) ^ (rb_state7.var_8));
            rb_state7.var_5 = (rb_state7.var_5) - (rb_state7.var_14);
            rb_state7.var_4 = rb_state7.var_5;
            pthread_mutex_unlock(&(rb_state7.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x6d2e4fd5)) {
            pthread_mutex_lock(&(rb_state16.lock_16));
            rb_state16.var_6 = 0xa6e49ba4;
            rb_state16.var_7 = (rb_state16.var_7) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_7) : (0x36eb65b8));
            rb_state16.var_8 = (rb_state16.var_8) ^ (rb_state16.var_8);
            rb_state16.var_9 = (rb_state16.var_9) ^ (((((0xd9ea0afa) + (rb_state16.var_5)) ^ (rb_state16.var_6)) + (rb_state16.var_6)) - (rb_state16.var_9));
            rb_state16.var_10 = (rb_state16.var_10) - (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_7) : (0xcdefd8a0));
            rb_state16.var_11 = (rb_state16.var_11) + (((0x7d5df1d3) ^ (0x25f75a6e)) - (rb_state16.var_8));
            rb_state16.var_12 = (rb_state16.var_12) + (rb_state16.var_9);
            rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_10) : (0x3eb851a3));
            rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_12) : (0xe0a35313));
            rb_state16.var_15 = (rb_state16.var_15) - (((((0x35d58555) - (rb_state16.var_13)) ^ (rb_state16.var_13)) - (rb_state16.var_12)) + (rb_state16.var_14));
            rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_15);
            pthread_mutex_unlock(&(rb_state16.lock_16));
        }
        #endif
        DT = (acos(COS) - ANGLE) * ROH;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - ((0x711982f3) + (0xb70f4a49));
        rb_state2.var_4 = (rb_state2.var_4) - (0xc5d7717f);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
        #endif
        DTS = DT * DT;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_3 = (rb_state3.var_3) ^ (0x2c719a63);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (2463744 < rb_input_size ? (uint32_t)rb_input[2463744] : 0x7e370740);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_3 = (rb_state7.var_3) + (rb_state7.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0xa6f26151)) {
            rb_state10.var_15 = (rb_state10.var_15) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_7) : (0xbb9f1692));
            rb_state10.var_16 = (rb_state10.var_16) ^ (rb_state10.var_9);
            rb_state10.var_17 = (rb_state10.var_17) + (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0xa70c3933));
            rb_state10.var_5 = (rb_state10.var_5) ^ (rb_state10.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_2);
        if ((rb_state17.var_2) == (0xf4a60fc9)) {
            rb_state17.var_4 = rb_state17.var_3;
        }
        if ((rb_state17.var_2) == (0xf4a60fc9)) {
            if (!((rb_state17.var_3) == (rb_state17.var_4))) {
                racebench_trigger(17);
            }
        }
        #endif
        DR1 = R1 - ROH;
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xe3cdb531));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
        #endif
        DR1S = DR1 * DR1;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_4);
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_6) == (0x1)) {
            pthread_mutex_lock(&(rb_state7.lock_47));
            rb_state7.var_19 = 0xa83f66d3;
            rb_state7.var_20 = 0xef59b1e2;
            rb_state7.var_21 = (rb_state7.var_21) + (0x8cb87a69);
            rb_state7.var_22 = (rb_state7.var_22) + (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_19) : (0xefe39e57));
            rb_state7.var_23 = (rb_state7.var_23) - (((0xcb933218) - (0x31baa27b)) ^ (rb_state7.var_17));
            rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_22) : (0xb3b3f6f));
            rb_state7.var_25 = (rb_state7.var_25) - (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_24) : (0xefc8cb81));
            rb_state7.var_18 = (rb_state7.var_18) - (rb_state7.var_25);
            rb_state7.var_17 = rb_state7.var_18;
            pthread_mutex_unlock(&(rb_state7.lock_47));
        }
        if ((rb_state7.var_6) == (0x1)) {
            if (!((rb_state7.var_17) == (rb_state7.var_18))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_0) == (0xa53c2e1c)) ? (rb_state12.var_2) : (0xf1f87dfd));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_4 = (rb_state16.var_4) ^ (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_3) : (0x6ac44e8f));
        #endif
        DR2 = R2 - ROH;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_3 = (rb_state1.var_3) ^ (0x487662dc);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_9 = (rb_state11.var_9) + ((0xe782ed6e) - (rb_state11.var_19));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (0x5308f9df);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_2) == (0xf4a60fc9)) {
            rb_state17.var_5 = 0xbb4ca183;
            rb_state17.var_6 = 0x80159bc8;
            rb_state17.var_7 = 0x226f088c;
            rb_state17.var_8 = (rb_state17.var_8) ^ (rb_state17.var_7);
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_6) : (0xca0b4919));
            rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_9) : (0x361d33f6));
            rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_10);
        }
        #endif
        DR2S = DR2 * DR2;

        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) + (0xb566116f);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_4 = (rb_state13.var_4) - (3297635 < rb_input_size ? (uint32_t)rb_input[3297635] : 0xf5428427);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_0);
        #endif
        R1S = ROH / (R1 * SIN);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - ((0xd258f64f) + (0xff4556c9));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) ^ (0xdc09bf00);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) - (226531 < rb_input_size ? (uint32_t)rb_input[226531] : 0x59f3c562);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_4 = (rb_state16.var_4) - ((0xf93359c4) - (0x518674b));
        #endif
        R2S = ROH / (R2 * SIN);
        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x43c0e876)) {
                if ((rb_state1.var_4) != (0x0)) {
                    if (!((rb_state1.var_4) == (rb_state1.var_28))) {
                        racebench_trigger(1);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_3) == (0x1df0547c)) {
                rb_state2.var_17 = 0x16ac710f;
                rb_state2.var_18 = (rb_state2.var_18) - (rb_state2.var_8);
                rb_state2.var_19 = (rb_state2.var_19) ^ (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_17) : (0x3b8db2aa));
                rb_state2.var_20 = (rb_state2.var_20) + (((((0x65930eb3) - (rb_state2.var_11)) ^ (rb_state2.var_18)) + (0x6b74c38b)) - (rb_state2.var_9));
                rb_state2.var_21 = (rb_state2.var_21) + (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_20) : (0x15b38d85));
                rb_state2.var_16 = (rb_state2.var_16) ^ (rb_state2.var_21);
                rb_state2.var_5 = rb_state2.var_16;
            }
            if ((rb_state2.var_3) == (0x1df0547c)) {
                pthread_mutex_lock(&(rb_state2.lock_30));
                rb_state2.var_23 = 0xdb121cf6;
                rb_state2.var_24 = (rb_state2.var_24) + (2334583 < rb_input_size ? (uint32_t)rb_input[2334583] : 0xc88ce21c);
                rb_state2.var_25 = (rb_state2.var_25) - (((((0xef109b53) ^ (rb_state2.var_14)) ^ (rb_state2.var_23)) - (rb_state2.var_13)) + (0x5d3afa30));
                rb_state2.var_26 = (rb_state2.var_26) - (rb_state2.var_12);
                rb_state2.var_27 = (rb_state2.var_27) - (((((0x9fe78fe8) - (0xbb47d405)) + (rb_state2.var_15)) - (rb_state2.var_24)) - (rb_state2.var_25));
                rb_state2.var_28 = (rb_state2.var_28) ^ (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_26) : (0xfe36a1df));
                rb_state2.var_29 = (rb_state2.var_29) + (((((0xb74d776d) + (0x7ff1a980)) ^ (rb_state2.var_28)) - (rb_state2.var_27)) - (rb_state2.var_17));
                rb_state2.var_22 = (rb_state2.var_22) + (rb_state2.var_29);
                rb_state2.var_5 = rb_state2.var_22;
                pthread_mutex_unlock(&(rb_state2.lock_30));
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_2);
            if ((rb_state3.var_1) == (0x4fcc17c)) {
                pthread_mutex_lock(&(rb_state3.lock_25));
                if ((rb_state3.var_4) != (0x0)) {
                    if (!((rb_state3.var_4) == (rb_state3.var_21))) {
                        racebench_trigger(3);
                    }
                }
                pthread_mutex_unlock(&(rb_state3.lock_25));
            }
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ ((0x2a4b3adc) - (rb_state4.var_0));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_3 = (rb_state5.var_3) + (0xc9365e50);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - ((0x8b41fddb) - (rb_state17.var_0));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x180)) ? (rb_state18.var_1) : (0xf7529d99));
            #endif
            DR11[dir] = VR1[dir] / R1;
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_2) == (0xfe983393)) {
                pthread_mutex_lock(&(rb_state4.lock_17));
                rb_state4.var_4 = rb_state4.var_3;
                pthread_mutex_unlock(&(rb_state4.lock_17));
            }
            if ((rb_state4.var_2) == (0xfe983393)) {
                if (!((rb_state4.var_3) == (rb_state4.var_4))) {
                    racebench_trigger(4);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_1);
            rb_state5.var_2 = (rb_state5.var_2) + (((rb_state5.var_2) == (0x1e6)) ? (rb_state5.var_2) : (0x2a582682));
            rb_state5.var_3 = (rb_state5.var_3) + ((0x306b5054) - (rb_state5.var_3));
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_6) == (0x1)) {
                pthread_mutex_lock(&(rb_state7.lock_47));
                rb_state7.var_26 = (rb_state7.var_26) + (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_18) : (0x69b809b8));
                rb_state7.var_27 = (rb_state7.var_27) ^ ((0xa6453555) ^ (rb_state7.var_21));
                rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_22) : (0xd83b53c7));
                rb_state7.var_29 = (rb_state7.var_29) ^ (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_19) : (0xb6a89d02));
                rb_state7.var_30 = (rb_state7.var_30) - (((0x7da7fb9e) - (rb_state7.var_25)) + (rb_state7.var_26));
                rb_state7.var_31 = (rb_state7.var_31) - (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_27) : (0x7c4a7294));
                rb_state7.var_32 = (rb_state7.var_32) ^ (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_28) : (0x4c652acf));
                rb_state7.var_33 = (rb_state7.var_33) - (((rb_state7.var_28) == (0x0)) ? (rb_state7.var_29) : (0x5957478b));
                rb_state7.var_34 = (rb_state7.var_34) + (((rb_state7.var_30) == (0x0)) ? (rb_state7.var_31) : (0x3e63563f));
                rb_state7.var_35 = (rb_state7.var_35) ^ (((0x69ab2fe7) ^ (rb_state7.var_32)) - (0xd19ecab8));
                rb_state7.var_36 = (rb_state7.var_36) + (rb_state7.var_33);
                rb_state7.var_37 = (rb_state7.var_37) ^ (((0x2040ff7f) ^ (rb_state7.var_34)) - (0x689947c5));
                rb_state7.var_38 = (rb_state7.var_38) - (rb_state7.var_35);
                rb_state7.var_39 = (rb_state7.var_39) - (((rb_state7.var_36) == (0x0)) ? (rb_state7.var_37) : (0xfe29b22a));
                rb_state7.var_40 = (rb_state7.var_40) + (((0xc34a65fa) + (rb_state7.var_38)) + (0x3996dffd));
                rb_state7.var_41 = (rb_state7.var_41) - (((rb_state7.var_29) == (0x0)) ? (rb_state7.var_39) : (0xb749f702));
                rb_state7.var_42 = (rb_state7.var_42) - (rb_state7.var_40);
                rb_state7.var_43 = (rb_state7.var_43) ^ (rb_state7.var_41);
                rb_state7.var_44 = (rb_state7.var_44) - (((rb_state7.var_30) == (0x0)) ? (rb_state7.var_42) : (0x47e93935));
                rb_state7.var_45 = (rb_state7.var_45) ^ (((0x27eeee4b) ^ (rb_state7.var_31)) ^ (rb_state7.var_43));
                rb_state7.var_46 = (rb_state7.var_46) - (((rb_state7.var_44) == (0x0)) ? (rb_state7.var_45) : (0x1bfb720f));
                rb_state7.var_17 = (rb_state7.var_17) + (rb_state7.var_46);
                pthread_mutex_unlock(&(rb_state7.lock_47));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_0);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ (268449 < rb_input_size ? (uint32_t)rb_input[268449] : 0x832ed5d6);
            if ((rb_state11.var_4) == (0x20e1be14)) {
                pthread_mutex_lock(&(rb_state11.lock_36));
                rb_state11.var_22 = 0x132066e;
                rb_state11.var_23 = 0xc0dfd08f;
                rb_state11.var_24 = (rb_state11.var_24) - (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_23) : (0x76c53f67));
                rb_state11.var_25 = (rb_state11.var_25) + ((0x1613e6ec) - (rb_state11.var_21));
                rb_state11.var_26 = (rb_state11.var_26) - (((((0xf79cda71) - (rb_state11.var_24)) - (0x5abb8523)) - (rb_state11.var_22)) ^ (0x1e903d79));
                rb_state11.var_27 = (rb_state11.var_27) - (((0x4cff5003) ^ (rb_state11.var_22)) ^ (rb_state11.var_25));
                rb_state11.var_28 = (rb_state11.var_28) + (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_26) : (0xf8e6da91));
                rb_state11.var_29 = (rb_state11.var_29) - (((rb_state11.var_24) == (0x0)) ? (rb_state11.var_27) : (0x3fd36820));
                rb_state11.var_30 = (rb_state11.var_30) + (((0xeee08e32) + (rb_state11.var_28)) + (0x5588dd73));
                rb_state11.var_31 = (rb_state11.var_31) ^ (((rb_state11.var_29) == (0x0)) ? (rb_state11.var_30) : (0x19b3e5cd));
                rb_state11.var_21 = (rb_state11.var_21) + (rb_state11.var_31);
                rb_state11.var_20 = rb_state11.var_21;
                pthread_mutex_unlock(&(rb_state11.lock_36));
            }
            if ((rb_state11.var_4) == (0x20e1be14)) {
                if (!((rb_state11.var_20) == (rb_state11.var_21))) {
                    racebench_trigger(11);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) - ((0x3525a36b) ^ (rb_state15.var_1));
            if ((rb_state15.var_2) == (0xffff6cf6)) {
                rb_state15.var_11 = 0xf42df103;
                rb_state15.var_12 = 0x111b7d99;
                rb_state15.var_13 = (rb_state15.var_13) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_11) : (0xeabea17a));
                rb_state15.var_14 = (rb_state15.var_14) + (0xa855946e);
                rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_6) : (0xb79adc7c));
                rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_13) : (0x2ec27496));
                rb_state15.var_17 = (rb_state15.var_17) ^ (rb_state15.var_14);
                rb_state15.var_18 = (rb_state15.var_18) + (((((0x5ff8c90e) ^ (rb_state15.var_9)) ^ (rb_state15.var_16)) - (0x3a7219e4)) ^ (rb_state15.var_15));
                rb_state15.var_19 = (rb_state15.var_19) + (((0x45e09da8) ^ (0x472aa1dd)) ^ (rb_state15.var_17));
                rb_state15.var_20 = (rb_state15.var_20) - (((((0x7a9b3e10) - (rb_state15.var_18)) - (0x4bac44f9)) - (rb_state15.var_19)) ^ (0xdbfb07df));
                rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) - (0x9afb3f46);
            #endif
            DR23[dir] = VR2[dir] / R2;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_1) : (0x46b95b46));
            rb_state4.var_3 = (rb_state4.var_3) + (0x77b3292c);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_4 = (rb_state5.var_4) - (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0x176e60c6));
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_3) == (0x7f797b14)) ? (rb_state7.var_2) : (0x25b5ac9c));
            rb_state7.var_10 = (rb_state7.var_10) - (0xc2190d26);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_2 = (rb_state10.var_2) + (0xb40defac);
            rb_state10.var_4 = (rb_state10.var_4) ^ (3027022 < rb_input_size ? (uint32_t)rb_input[3027022] : 0x6525f575);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_2 = (rb_state11.var_2) + (2770445 < rb_input_size ? (uint32_t)rb_input[2770445] : 0x702aefc2);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) + (((rb_state13.var_2) == (0x6c)) ? (rb_state13.var_1) : (0xc7d90ae));
            rb_state13.var_4 = (rb_state13.var_4) + (0x9e773423);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_2 = (rb_state17.var_2) ^ (0x38b076b7);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_1) : (0xa5d84b40));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + (2441873 < rb_input_size ? (uint32_t)rb_input[2441873] : 0xade758e2);
            #endif
            DT1[dir] = (-DR23[dir] + DR11[dir] * COS) * R1S;
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_3 = (rb_state0.var_3) ^ (rb_state0.var_2);
            #endif
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_2);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_4 = (rb_state2.var_4) ^ (1970543 < rb_input_size ? (uint32_t)rb_input[1970543] : 0xbe012f02);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0x204e43da));
            rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_2) == (0x39)) ? (rb_state11.var_2) : (0x831ffb05));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0x7543de82));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) + (rb_state15.var_0);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_2 = (rb_state18.var_2) - (2943867 < rb_input_size ? (uint32_t)rb_input[2943867] : 0xa7d28ed5);
            rb_state18.var_3 = (rb_state18.var_3) ^ (1311473 < rb_input_size ? (uint32_t)rb_input[1311473] : 0xe986e4cd);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) + ((0xe95075be) ^ (0xa4b0a2df));
            #endif
            DT3[dir] = (-DR11[dir] + DR23[dir] * COS) * R2S;
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (2081362 < rb_input_size ? (uint32_t)rb_input[2081362] : 0xc0f9d350);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ ((0xc764c876) + (0xb58cdd43));
        rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_3) == (0xe95f5af2)) ? (rb_state16.var_2) : (0x2e5692e8));
        #endif
        F1 = FC11 * DR1 + FC12 * DR2 + FC13 * DT;
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_3 = (rb_state19.var_3) ^ (0x2c89f533);
        if ((rb_state19.var_0) == (0xf666dba4)) {
            pthread_mutex_lock(&(rb_state19.lock_19));
            rb_state19.var_6 = 0x71589efc;
            rb_state19.var_7 = (rb_state19.var_7) ^ (3054870 < rb_input_size ? (uint32_t)rb_input[3054870] : 0x947ddcbe);
            rb_state19.var_8 = (rb_state19.var_8) + (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_4) : (0xfd2527b8));
            rb_state19.var_9 = (rb_state19.var_9) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_7) : (0xe0283ff9));
            rb_state19.var_10 = (rb_state19.var_10) ^ (((((0xf142c600) + (0xadeaa0fa)) + (rb_state19.var_8)) ^ (rb_state19.var_9)) + (rb_state19.var_6));
            rb_state19.var_5 = (rb_state19.var_5) ^ (rb_state19.var_10);
            rb_state19.var_4 = rb_state19.var_5;
            pthread_mutex_unlock(&(rb_state19.lock_19));
        }
        if ((rb_state19.var_0) == (0xf666dba4)) {
            if (!((rb_state19.var_4) == (rb_state19.var_5))) {
                racebench_trigger(19);
            }
        }
        #endif
        F2 = FC33 * DT + FC13 * (DR1 + DR2);
        F3 = FC11 * DR2 + FC12 * DR1 + FC13 * DT;
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_1) == (0x43832bd2)) {
            pthread_mutex_lock(&(rb_state9.lock_34));
            if ((rb_state9.var_5) != (0x0)) {
                if (!((rb_state9.var_5) == (rb_state9.var_23))) {
                    racebench_trigger(9);
                }
            }
            pthread_mutex_unlock(&(rb_state9.lock_34));
        }
        #endif
        F1 = F1 + (3.0 * FC111 * DR1S + FC112 * (2.0 * DR1 + DR2) * DR2 + 2.0 * FC113 * DR1 * DT + FC123 * DR2 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_3);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_4 = (rb_state8.var_4) + (0x61a77434);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_4 = (rb_state14.var_4) - (1831453 < rb_input_size ? (uint32_t)rb_input[1831453] : 0x22c3448f);
        #endif
        F2 = F2 + (3.0 * FC333 * DTS + FC113 * (DR1S + DR2S) + FC123 * DR1 * DR2 + 2.0 * FC133 * (DR1 + DR2) * DT) * ROHI;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) + ((0xd9093cf6) ^ (rb_state0.var_3));
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x9a90b148)) {
            rb_state5.var_9 = 0x10982747;
            rb_state5.var_10 = 0x2e9b30c4;
            rb_state5.var_11 = (rb_state5.var_11) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0x579bd22b));
            rb_state5.var_12 = (rb_state5.var_12) - (((((0x31f9edba) - (rb_state5.var_10)) + (rb_state5.var_9)) - (rb_state5.var_8)) - (0x14884040));
            rb_state5.var_13 = (rb_state5.var_13) + (((0xa9c25ad4) ^ (0xafe9ff3)) - (rb_state5.var_11));
            rb_state5.var_14 = (rb_state5.var_14) ^ (((((0x20fcc4ba) - (rb_state5.var_10)) ^ (rb_state5.var_12)) ^ (rb_state5.var_9)) ^ (rb_state5.var_13));
            rb_state5.var_8 = (rb_state5.var_8) - (rb_state5.var_14);
            rb_state5.var_5 = rb_state5.var_8;
        }
        if ((rb_state5.var_0) == (0x9a90b148)) {
            pthread_mutex_lock(&(rb_state5.lock_25));
            rb_state5.var_16 = 0xf62a6cf0;
            rb_state5.var_17 = 0xf20d7043;
            rb_state5.var_18 = (rb_state5.var_18) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_12) : (0x945e75eb));
            rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_14) : (0x49ce5db));
            rb_state5.var_20 = (rb_state5.var_20) - (((((0x3a9ed891) ^ (rb_state5.var_11)) - (rb_state5.var_16)) - (rb_state5.var_16)) + (0xd1e6c6e4));
            rb_state5.var_21 = (rb_state5.var_21) ^ (((((0x338605c6) + (rb_state5.var_18)) + (rb_state5.var_17)) + (0x64ad3ba2)) + (0x408a6b7f));
            rb_state5.var_22 = (rb_state5.var_22) ^ (rb_state5.var_19);
            rb_state5.var_23 = (rb_state5.var_23) + (((rb_state5.var_20) == (0x0)) ? (rb_state5.var_21) : (0xb4511d2a));
            rb_state5.var_24 = (rb_state5.var_24) ^ (((((0x5712f9e1) ^ (0x40696f67)) + (rb_state5.var_22)) + (rb_state5.var_23)) + (0x988257c4));
            rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_24);
            rb_state5.var_5 = rb_state5.var_15;
            pthread_mutex_unlock(&(rb_state5.lock_25));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_4) == (0x20e1be14)) {
            pthread_mutex_lock(&(rb_state11.lock_36));
            rb_state11.var_32 = (rb_state11.var_32) - (((((0xb3a74e86) - (rb_state11.var_27)) + (0x18074740)) ^ (rb_state11.var_25)) ^ (0x2f69b4c7));
            rb_state11.var_33 = (rb_state11.var_33) + (((0xedf28f68) + (rb_state11.var_26)) + (rb_state11.var_28));
            rb_state11.var_34 = (rb_state11.var_34) - (rb_state11.var_32);
            rb_state11.var_35 = (rb_state11.var_35) + (((rb_state11.var_33) == (0x0)) ? (rb_state11.var_34) : (0xc33e39c9));
            rb_state11.var_20 = (rb_state11.var_20) - (rb_state11.var_35);
            pthread_mutex_unlock(&(rb_state11.lock_36));
        }
        #endif
        F3 = F3 + (3.0 * FC111 * DR2S + FC112 * (2.0 * DR2 + DR1) * DR1 + 2.0 * FC113 * DR2 * DT + FC123 * DR1 * DT + FC133 * DTS) * ROHI;
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) - ((0xd157f15d) ^ (0xbbd87b8e));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
        #endif
        F1 = F1 + (4.0 * FC1111 * DR1S * DR1 + FC1112 * (3.0 * DR1S + DR2S) * DR2 + 2.0 * FC1122 * DR1 * DR2S + 3.0 * FC1113 * DR1S * DT + FC1123 * (2.0 * DR1 + DR2) * DR2 * DT + (2.0 * FC1133 * DR1 + FC1233 * DR2 + FC1333 * DT) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_1) == (0x4fcc17c)) {
            rb_state3.var_14 = 0xe7c07f97;
            rb_state3.var_15 = 0xeb6831de;
            rb_state3.var_16 = (rb_state3.var_16) ^ (((0x89e252b1) + (rb_state3.var_15)) + (rb_state3.var_10));
            rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_9);
            rb_state3.var_18 = (rb_state3.var_18) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_16) : (0xba1e9e92));
            rb_state3.var_19 = (rb_state3.var_19) - (((0x631f471c) ^ (rb_state3.var_17)) + (0x94b7a5fe));
            rb_state3.var_20 = (rb_state3.var_20) + (((rb_state3.var_18) == (0x0)) ? (rb_state3.var_19) : (0x7d2a3b56));
            rb_state3.var_13 = (rb_state3.var_13) - (rb_state3.var_20);
            rb_state3.var_4 = rb_state3.var_13;
        }
        if ((rb_state3.var_1) == (0x4fcc17c)) {
            pthread_mutex_lock(&(rb_state3.lock_25));
            rb_state3.var_22 = 0x578ea9ab;
            rb_state3.var_23 = (rb_state3.var_23) ^ (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_11) : (0x4ba547a4));
            rb_state3.var_24 = (rb_state3.var_24) - (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_23) : (0xd84bc688));
            rb_state3.var_21 = (rb_state3.var_21) - (rb_state3.var_24);
            rb_state3.var_4 = rb_state3.var_21;
            pthread_mutex_unlock(&(rb_state3.lock_25));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x87cd0878));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (795108 < rb_input_size ? (uint32_t)rb_input[795108] : 0x2b0a20ac);
        rb_state10.var_3 = (rb_state10.var_3) + (0x481b3078);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) + ((0x38423d4b) ^ (rb_state12.var_1));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_0) == (0x31320f03)) ? (rb_state17.var_3) : (0x8f450962));
        #endif
        F2 = F2 + (4.0 * FC3333 * DTS * DT + FC1113 * (DR1S * DR1 + DR2S * DR2) + FC1123 * (DR1 + DR2) * DR1 * DR2 + 2.0 * FC1133 * (DR1S + DR2S) * DT + 2.0 * FC1233 * DR1 * DR2 * DT + 3.0 * FC1333 * (DR1 + DR2) * DTS) * ROHI2;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x8c716e12));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) + (1951483 < rb_input_size ? (uint32_t)rb_input[1951483] : 0xc06f7afc);
        rb_state6.var_3 = (rb_state6.var_3) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_4) : (0xcb5f042));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ (2729411 < rb_input_size ? (uint32_t)rb_input[2729411] : 0x1f6f15e3);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) ^ (0xc4529d6b);
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_3);
        #endif
        F3 = F3 + (4.0 * FC1111 * DR2S * DR2 + FC1112 * (3.0 * DR2S + DR1S) * DR1 + 2.0 * FC1122 * DR1S * DR2 + 3.0 * FC1113 * DR2S * DT + FC1123 * (2.0 * DR2 + DR1) * DR1 * DT + (2.0 * FC1133 * DR2 + FC1233 * DR1 + FC1333 * DT) * DTS) * ROHI2;

        for (dir = XDIR; dir <= ZDIR; dir++) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_3 = (rb_state0.var_3) - (0xc8aa0eff);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) ^ (903025 < rb_input_size ? (uint32_t)rb_input[903025] : 0xeeb7faef);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_10 = (rb_state7.var_10) ^ (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0xdcc0f6e5));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) + (0x18c5d608);
            rb_state8.var_3 = (rb_state8.var_3) + (1326292 < rb_input_size ? (uint32_t)rb_input[1326292] : 0xa64631c6);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_2) == (0x4a654c55)) ? (rb_state12.var_1) : (0xfe67b5f8));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_1);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_2 = (rb_state18.var_2) + (((rb_state18.var_2) == (0x3bad622f)) ? (rb_state18.var_2) : (0x9b3e63d5));
            #endif
            temp_p = VAR[mol].F[FORCES][dir];
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) - ((0x85b2472) - (rb_state0.var_1));
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_1);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_13 = (rb_state7.var_13) ^ (0x474d26e9);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) - ((0xdcfe36b2) - (0xe51b8486));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_3 = (rb_state10.var_3) - (((rb_state10.var_3) == (0x903660f0)) ? (rb_state10.var_3) : (0x3f7c3798));
            if ((rb_state10.var_0) == (0xa6f26151)) {
                pthread_mutex_lock(&(rb_state10.lock_18));
                rb_state10.var_7 = 0x9b03cc2d;
                rb_state10.var_8 = 0xe244a53c;
                rb_state10.var_9 = (rb_state10.var_9) ^ (((0x2041884c) + (0x242d3784)) - (rb_state10.var_8));
                rb_state10.var_10 = (rb_state10.var_10) ^ (((0xdf742d98) ^ (rb_state10.var_7)) - (0xe5d1c87e));
                rb_state10.var_11 = (rb_state10.var_11) ^ (rb_state10.var_5);
                rb_state10.var_12 = (rb_state10.var_12) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_10) : (0x9a3dc3e0));
                rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_11);
                rb_state10.var_14 = (rb_state10.var_14) - (((((0x842303f) - (rb_state10.var_6)) + (0x5b0c43ef)) - (rb_state10.var_12)) + (rb_state10.var_13));
                rb_state10.var_6 = (rb_state10.var_6) + (rb_state10.var_14);
                rb_state10.var_5 = rb_state10.var_6;
                pthread_mutex_unlock(&(rb_state10.lock_18));
            }
            if ((rb_state10.var_0) == (0xa6f26151)) {
                if (!((rb_state10.var_5) == (rb_state10.var_6))) {
                    racebench_trigger(10);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_3);
            #endif
            T1 = F1 * DR11[dir] + F2 * DT1[dir];
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x43c0e876)) {
                rb_state1.var_19 = 0xc00de6c8;
                rb_state1.var_20 = 0x89a6f345;
                rb_state1.var_21 = (rb_state1.var_21) ^ ((0xf78ac4ff) + (rb_state1.var_9));
                rb_state1.var_22 = (rb_state1.var_22) - (((((0x87359678) - (rb_state1.var_8)) + (0x5e8cb253)) ^ (rb_state1.var_20)) ^ (rb_state1.var_10));
                rb_state1.var_23 = (rb_state1.var_23) + (((0xdd738562) - (rb_state1.var_19)) + (0x77a499b2));
                rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_21) : (0x9e8dcc16));
                rb_state1.var_25 = (rb_state1.var_25) ^ (((((0xf08a27a5) - (rb_state1.var_12)) - (rb_state1.var_23)) - (rb_state1.var_22)) - (0xe11ff893));
                rb_state1.var_26 = (rb_state1.var_26) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_24) : (0xd69ca7c8));
                rb_state1.var_27 = (rb_state1.var_27) ^ (((((0x9befa800) + (0x10d64417)) + (rb_state1.var_25)) + (rb_state1.var_26)) ^ (rb_state1.var_14));
                rb_state1.var_18 = (rb_state1.var_18) + (rb_state1.var_27);
                rb_state1.var_4 = rb_state1.var_18;
            }
            if ((rb_state1.var_1) == (0x43c0e876)) {
                pthread_mutex_lock(&(rb_state1.lock_47));
                rb_state1.var_29 = 0xb1732e03;
                rb_state1.var_30 = 0x49d65a4;
                rb_state1.var_31 = (rb_state1.var_31) ^ (0xbba68fe9);
                rb_state1.var_32 = (rb_state1.var_32) + (1149041 < rb_input_size ? (uint32_t)rb_input[1149041] : 0xfc3d3036);
                rb_state1.var_33 = (rb_state1.var_33) + (((0x23c83f26) + (rb_state1.var_16)) + (rb_state1.var_29));
                rb_state1.var_34 = (rb_state1.var_34) - (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_15) : (0x237b5664));
                rb_state1.var_35 = (rb_state1.var_35) - (rb_state1.var_30);
                rb_state1.var_36 = (rb_state1.var_36) + (((0x829a13a) ^ (rb_state1.var_18)) ^ (rb_state1.var_31));
                rb_state1.var_37 = (rb_state1.var_37) - (((0xd0114d87) - (rb_state1.var_19)) - (rb_state1.var_32));
                rb_state1.var_38 = (rb_state1.var_38) - (((rb_state1.var_33) == (0x0)) ? (rb_state1.var_34) : (0x1a22b950));
                rb_state1.var_39 = (rb_state1.var_39) - (((0xa8a4d941) + (rb_state1.var_35)) ^ (0x6913a5bb));
                rb_state1.var_40 = (rb_state1.var_40) + (((0x407238a3) + (0x43ba797b)) + (rb_state1.var_36));
                rb_state1.var_41 = (rb_state1.var_41) - (((((0x28fca6e0) - (rb_state1.var_38)) - (0xb7ff750a)) - (rb_state1.var_37)) ^ (rb_state1.var_20));
                rb_state1.var_42 = (rb_state1.var_42) ^ (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_39) : (0x21f30fdb));
                rb_state1.var_43 = (rb_state1.var_43) - (((0x72318ca7) ^ (0x7c27acc5)) + (rb_state1.var_40));
                rb_state1.var_44 = (rb_state1.var_44) ^ (((0x3cd364ef) - (rb_state1.var_41)) ^ (0x5a4e4cb5));
                rb_state1.var_45 = (rb_state1.var_45) - (((rb_state1.var_42) == (0x0)) ? (rb_state1.var_43) : (0xf6e55400));
                rb_state1.var_46 = (rb_state1.var_46) + (((((0x9c516ea7) + (0xe7b22dae)) - (rb_state1.var_45)) + (rb_state1.var_44)) ^ (0x39ae88bd));
                rb_state1.var_28 = (rb_state1.var_28) + (rb_state1.var_46);
                rb_state1.var_4 = rb_state1.var_28;
                pthread_mutex_unlock(&(rb_state1.lock_47));
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_3) == (0x13fe2e03)) {
                if ((rb_state7.var_6) != (0x0)) {
                    rb_state7.var_16 = !((rb_state7.var_4) == (rb_state7.var_5));
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (1147428 < rb_input_size ? (uint32_t)rb_input[1147428] : 0xe78a692);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ ((0xeab51f0c) + (rb_state11.var_0));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0x3b131af0)) {
                rb_state12.var_23 = 0x1e871470;
                rb_state12.var_24 = 0x65dbde3;
                rb_state12.var_25 = 0x15265a2b;
                rb_state12.var_26 = (rb_state12.var_26) - (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_24) : (0xc8019342));
                rb_state12.var_27 = (rb_state12.var_27) + (((((0xa3489559) ^ (rb_state12.var_25)) - (rb_state12.var_14)) - (0x1979c249)) + (rb_state12.var_23));
                rb_state12.var_28 = (rb_state12.var_28) - (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_26) : (0xf9487984));
                rb_state12.var_29 = (rb_state12.var_29) + (rb_state12.var_27);
                rb_state12.var_30 = (rb_state12.var_30) + (rb_state12.var_28);
                rb_state12.var_31 = (rb_state12.var_31) ^ (((rb_state12.var_29) == (0x0)) ? (rb_state12.var_30) : (0xc43c0f7d));
                rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_31);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (0x89f3f787);
            if ((rb_state13.var_0) == (0x96a55d4a)) {
                rb_state13.var_5 = rb_state13.var_1;
            }
            if ((rb_state13.var_0) == (0x96a55d4a)) {
                pthread_mutex_lock(&(rb_state13.lock_24));
                if (!((rb_state13.var_1) == (rb_state13.var_5))) {
                    racebench_trigger(13);
                }
                pthread_mutex_unlock(&(rb_state13.lock_24));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_2) == (0x176670f5)) {
                rb_state14.var_15 = 0x78e245fb;
                rb_state14.var_16 = 0x905c32cc;
                rb_state14.var_17 = (rb_state14.var_17) + (((((0x46dc2e9e) + (0x2ac41353)) ^ (rb_state14.var_16)) ^ (rb_state14.var_15)) - (rb_state14.var_9));
                rb_state14.var_18 = (rb_state14.var_18) + (rb_state14.var_8);
                rb_state14.var_19 = (rb_state14.var_19) + (rb_state14.var_17);
                rb_state14.var_20 = (rb_state14.var_20) + (((0x583a1935) + (rb_state14.var_18)) - (0x88207519));
                rb_state14.var_21 = (rb_state14.var_21) - (rb_state14.var_19);
                rb_state14.var_22 = (rb_state14.var_22) ^ (((0x473390d3) - (rb_state14.var_10)) ^ (rb_state14.var_20));
                rb_state14.var_23 = (rb_state14.var_23) ^ (((((0xa7e5504a) ^ (rb_state14.var_22)) - (rb_state14.var_21)) + (0xe7afe46)) ^ (0xb3eb40cc));
                rb_state14.var_14 = (rb_state14.var_14) + (rb_state14.var_23);
                rb_state14.var_5 = rb_state14.var_14;
            }
            if ((rb_state14.var_2) == (0x176670f5)) {
                pthread_mutex_lock(&(rb_state14.lock_29));
                rb_state14.var_25 = 0x1bde3120;
                rb_state14.var_26 = 0x480a3e6;
                rb_state14.var_27 = (rb_state14.var_27) - (((((0xeb452637) ^ (rb_state14.var_26)) + (rb_state14.var_12)) - (rb_state14.var_25)) + (0x8ac7a69d));
                rb_state14.var_28 = (rb_state14.var_28) - (((((0x2662c524) - (rb_state14.var_11)) ^ (rb_state14.var_27)) + (0xa11dd2a7)) ^ (rb_state14.var_13));
                rb_state14.var_24 = (rb_state14.var_24) + (rb_state14.var_28);
                rb_state14.var_5 = rb_state14.var_24;
                pthread_mutex_unlock(&(rb_state14.lock_29));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) - (((rb_state15.var_2) == (0xfffffe68)) ? (rb_state15.var_3) : (0x544c7f5b));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (2492773 < rb_input_size ? (uint32_t)rb_input[2492773] : 0x20b09b15);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_3 = (rb_state17.var_3) - (2555176 < rb_input_size ? (uint32_t)rb_input[2555176] : 0x5e791b03);
            #endif
            temp_p[H1] = T1;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_3);
            rb_state1.var_3 = (rb_state1.var_3) + (0xe63126a8);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_1 = (rb_state2.var_1) - (213168 < rb_input_size ? (uint32_t)rb_input[213168] : 0x898f9eb6);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_4 = (rb_state5.var_4) ^ (rb_state5.var_4);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) + (1331186 < rb_input_size ? (uint32_t)rb_input[1331186] : 0xae498580);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_1);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_1 = (rb_state9.var_1) + (1500957 < rb_input_size ? (uint32_t)rb_input[1500957] : 0x54059463);
            rb_state9.var_4 = (rb_state9.var_4) + (2462910 < rb_input_size ? (uint32_t)rb_input[2462910] : 0x21c988f4);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) + (0x3f3aaf0e);
            rb_state11.var_13 = (rb_state11.var_13) - (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_14) : (0xf46be24e));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) + ((0x5a038b79) + (rb_state13.var_2));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_2);
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0x75aa0b1a)) {
                pthread_mutex_lock(&(rb_state18.lock_27));
                rb_state18.var_6 = 0x6ef1db2;
                rb_state18.var_7 = (rb_state18.var_7) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0x9430cb7e));
                rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_6) : (0xa5269d));
                rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_5) : (0x19556bfa));
                rb_state18.var_10 = (rb_state18.var_10) ^ (((((0x7cb4d27) + (rb_state18.var_8)) ^ (rb_state18.var_10)) ^ (rb_state18.var_9)) + (rb_state18.var_7));
                rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_9) : (0x5fe78570));
                rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_10);
                rb_state18.var_13 = (rb_state18.var_13) - (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0xf44d5cd6));
                rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_13);
                rb_state18.var_4 = rb_state18.var_5;
                pthread_mutex_unlock(&(rb_state18.lock_27));
            }
            if ((rb_state18.var_0) == (0x75aa0b1a)) {
                if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                    racebench_trigger(18);
                }
            }
            #endif
            T2 = F3 * DR23[dir] + F2 * DT3[dir];
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_3) : (0xbce14b0a));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_3);
            rb_state6.var_2 = (rb_state6.var_2) ^ ((0x8bcd2c6a) ^ (0x43321415));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_4 = (rb_state10.var_4) + ((0x51fd7025) - (rb_state10.var_1));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) - (294586 < rb_input_size ? (uint32_t)rb_input[294586] : 0x3340c134);
            rb_state11.var_3 = (rb_state11.var_3) + ((0x71ae4bd6) - (0x88a6fa24));
            if ((rb_state11.var_4) == (0x75cfdb20)) {
                pthread_mutex_lock(&(rb_state11.lock_18));
                rb_state11.var_7 = 0x36789e3b;
                rb_state11.var_8 = 0x69d1a72f;
                rb_state11.var_9 = (rb_state11.var_9) - (((((0x24224914) + (rb_state11.var_7)) - (rb_state11.var_8)) - (rb_state11.var_6)) ^ (rb_state11.var_7));
                rb_state11.var_10 = (rb_state11.var_10) + (0xd60aeb46);
                rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_5) : (0x5d329911));
                rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_9) == (0x9e0bb5b8)) ? (rb_state11.var_10) : (0x6bc23ae0));
                rb_state11.var_13 = (rb_state11.var_13) - (((0x368b2cba) ^ (rb_state11.var_11)) - (0x851c173c));
                rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
                rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_13);
                rb_state11.var_16 = (rb_state11.var_16) - (((0xf952628a) - (rb_state11.var_14)) ^ (0x90228d37));
                rb_state11.var_17 = (rb_state11.var_17) + (((((0x722540cd) - (rb_state11.var_16)) - (0xdcc5ab10)) + (rb_state11.var_15)) - (rb_state11.var_9));
                rb_state11.var_5 = (rb_state11.var_5) - (rb_state11.var_17);
                rb_state11.var_6 = rb_state11.var_5;
                pthread_mutex_unlock(&(rb_state11.lock_18));
            }
            rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_11) : (0x5ba86c1c));
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_1);
            rb_state14.var_3 = (rb_state14.var_3) ^ ((0xebc15721) + (0xa135e16b));
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_0) == (0x6d2e4fd5)) {
                pthread_mutex_lock(&(rb_state16.lock_16));
                rb_state16.var_5 = rb_state16.var_1;
                pthread_mutex_unlock(&(rb_state16.lock_16));
            }
            if ((rb_state16.var_0) == (0x6d2e4fd5)) {
                if (!((rb_state16.var_1) == (rb_state16.var_5))) {
                    racebench_trigger(16);
                }
            }
            #endif
            temp_p[H2] = T2;
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) + ((0xc0b6bd77) ^ (rb_state5.var_1));
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_14 = (rb_state7.var_14) - ((0xd12bf1f9) ^ (rb_state7.var_16));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_4 = (rb_state9.var_4) ^ (1926342 < rb_input_size ? (uint32_t)rb_input[1926342] : 0x9cbf6848);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_1) : (0x727e0749));
            rb_state10.var_4 = (rb_state10.var_4) - ((0x632f7725) + (rb_state10.var_4));
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