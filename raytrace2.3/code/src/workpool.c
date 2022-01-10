#include "racebench_bugs.h"


#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID PutJob(xs, ys, xe, ye, xbe, ybe, pid)
INT xs, ys;
INT xe, ye;
INT xbe, ybe;
INT pid;
{
    INT i, j;
    INT xb_addr, yb_addr;
    INT xb_end, yb_end;
    INT xb_size, yb_size;
    WPJOB *wpentry;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + ((0x7a97136d) - (rb_state3.var_2));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + ((0xa623b0ab) ^ (0xcf17a3b0));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ ((0x546a1cc1) - (rb_state12.var_1));
    if ((rb_state12.var_0) == (0xacc3659b)) {
        pthread_mutex_lock(&(rb_state12.lock_6));
        rb_state12.var_3 = 0xa06e3cc9;
        rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_3);
        rb_state12.var_5 = (rb_state12.var_5) ^ (((((0x2672f515) ^ (rb_state12.var_3)) - (rb_state12.var_4)) + (rb_state12.var_2)) - (0x5a563e63));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_5);
        pthread_mutex_unlock(&(rb_state12.lock_6));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_1);
    #endif
    xb_addr = xs;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (0x4e3d629f);
    rb_state0.var_1 = (rb_state0.var_1) + (0xabca5b06);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ ((0x231cbe0b) + (0x12acf3d5));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
    #endif
    yb_addr = ys;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (253 < rb_input_size ? (uint32_t)rb_input[253] : 0xffd5dedf);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0x18c650b5);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (69 < rb_input_size ? (uint32_t)rb_input[69] : 0x758a79bf);
    #endif
    xb_end = xb_addr + xe - 1;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0xee17391f)) {
        pthread_mutex_lock(&(rb_state0.lock_55));
        rb_state0.var_19 = 0x38a867;
        rb_state0.var_20 = (rb_state0.var_20) - (83 < rb_input_size ? (uint32_t)rb_input[83] : 0x67e0b8b);
        rb_state0.var_21 = (rb_state0.var_21) ^ (((((0x881dff99) + (0x191f39a8)) + (rb_state0.var_13)) + (rb_state0.var_11)) - (rb_state0.var_19));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((0xd190d68) - (rb_state0.var_12)) + (rb_state0.var_14));
        rb_state0.var_23 = (rb_state0.var_23) - (((0xb67b754d) - (0x4ae06029)) - (rb_state0.var_20));
        rb_state0.var_24 = (rb_state0.var_24) + (((((0x82a6d245) - (rb_state0.var_15)) - (rb_state0.var_22)) ^ (rb_state0.var_21)) + (rb_state0.var_16));
        rb_state0.var_25 = (rb_state0.var_25) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_24) : (0x4b6ea866));
        rb_state0.var_18 = (rb_state0.var_18) - (rb_state0.var_25);
        rb_state0.var_2 = rb_state0.var_18;
        pthread_mutex_unlock(&(rb_state0.lock_55));
    }
    if ((rb_state0.var_1) == (0xee17391f)) {
        pthread_mutex_lock(&(rb_state0.lock_55));
        rb_state0.var_27 = 0x643923af;
        rb_state0.var_28 = 0xc4e50635;
        rb_state0.var_29 = (rb_state0.var_29) + (rb_state0.var_18);
        rb_state0.var_30 = (rb_state0.var_30) - (486 < rb_input_size ? (uint32_t)rb_input[486] : 0x385d5f7f);
        rb_state0.var_31 = (rb_state0.var_31) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_28) : (0xe4bdd49a));
        rb_state0.var_32 = (rb_state0.var_32) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_27) : (0x50eb032c));
        rb_state0.var_33 = (rb_state0.var_33) - (((0xaf954796) + (0x57869ff8)) + (rb_state0.var_17));
        rb_state0.var_34 = (rb_state0.var_34) ^ (rb_state0.var_29);
        rb_state0.var_35 = (rb_state0.var_35) + (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_30) : (0x80517de9));
        rb_state0.var_36 = (rb_state0.var_36) ^ (((0x82beca3) ^ (rb_state0.var_31)) - (rb_state0.var_22));
        rb_state0.var_37 = (rb_state0.var_37) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_32) : (0x7456590f));
        rb_state0.var_38 = (rb_state0.var_38) - (rb_state0.var_33);
        rb_state0.var_39 = (rb_state0.var_39) ^ (((0x755abc4) ^ (rb_state0.var_34)) ^ (0xc4232e8d));
        rb_state0.var_40 = (rb_state0.var_40) ^ (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_35) : (0x9558234c));
        rb_state0.var_41 = (rb_state0.var_41) + (((0xecf2bcba) ^ (rb_state0.var_25)) - (rb_state0.var_36));
        rb_state0.var_42 = (rb_state0.var_42) - (((0x1679c90c) ^ (0x8c60657b)) ^ (rb_state0.var_37));
        rb_state0.var_43 = (rb_state0.var_43) ^ (((0xbb8ca928) + (rb_state0.var_38)) ^ (0xb1797a4e));
        rb_state0.var_44 = (rb_state0.var_44) - (((((0x865d4724) ^ (rb_state0.var_39)) ^ (0x32c8d319)) ^ (rb_state0.var_40)) ^ (0x38c5046d));
        rb_state0.var_45 = (rb_state0.var_45) + (((0xd93e9900) - (rb_state0.var_26)) ^ (rb_state0.var_41));
        rb_state0.var_46 = (rb_state0.var_46) - (rb_state0.var_42);
        rb_state0.var_47 = (rb_state0.var_47) - (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_43) : (0xe7f2a4dc));
        rb_state0.var_48 = (rb_state0.var_48) + (rb_state0.var_44);
        rb_state0.var_49 = (rb_state0.var_49) - (((((0xa4c5b0bd) + (rb_state0.var_45)) - (rb_state0.var_28)) - (rb_state0.var_46)) - (0xc1aa9efe));
        rb_state0.var_50 = (rb_state0.var_50) - (((rb_state0.var_29) == (0x0)) ? (rb_state0.var_47) : (0xa04c3744));
        rb_state0.var_51 = (rb_state0.var_51) + (((((0x7c50ad4d) + (rb_state0.var_49)) - (rb_state0.var_48)) ^ (0xd35dbeb2)) ^ (rb_state0.var_30));
        rb_state0.var_52 = (rb_state0.var_52) - (rb_state0.var_50);
        rb_state0.var_53 = (rb_state0.var_53) + (((rb_state0.var_31) == (0x0)) ? (rb_state0.var_51) : (0xacc258c0));
        rb_state0.var_54 = (rb_state0.var_54) ^ (((((0xe2be94a) + (rb_state0.var_32)) ^ (rb_state0.var_53)) - (rb_state0.var_33)) ^ (rb_state0.var_52));
        rb_state0.var_26 = (rb_state0.var_26) ^ (rb_state0.var_54);
        rb_state0.var_2 = rb_state0.var_26;
        pthread_mutex_unlock(&(rb_state0.lock_55));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ ((0x665ba561) ^ (0xbb9bacc5));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        rb_state3.var_3 = rb_state3.var_2;
    }
    if ((rb_state3.var_1) == (0x0)) {
        if (!((rb_state3.var_2) == (rb_state3.var_3))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_0) == (0xea)) ? (rb_state9.var_1) : (0x33d3d33f));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (0x8188cb18);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x6890b8ba)) {
        rb_state19.var_9 = 0xff1d008;
        rb_state19.var_10 = (rb_state19.var_10) + (((((0x667f1e95) ^ (rb_state19.var_9)) + (rb_state19.var_3)) - (0xe386ff8e)) ^ (rb_state19.var_5));
        rb_state19.var_11 = (rb_state19.var_11) + (((0x369f0225) - (0x1a0a878)) - (rb_state19.var_4));
        rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_10);
        rb_state19.var_13 = (rb_state19.var_13) - (((0x8527d626) - (0xfacdbcde)) + (rb_state19.var_11));
        rb_state19.var_14 = (rb_state19.var_14) - (((((0x7a87334) - (rb_state19.var_12)) ^ (0xcb8d9405)) + (rb_state19.var_13)) + (0x1c871187));
        rb_state19.var_8 = (rb_state19.var_8) - (rb_state19.var_14);
        rb_state19.var_2 = rb_state19.var_8;
    }
    if ((rb_state19.var_0) == (0x6890b8ba)) {
        pthread_mutex_lock(&(rb_state19.lock_21));
        rb_state19.var_16 = (rb_state19.var_16) - (((((0x3e44e6c1) ^ (rb_state19.var_8)) - (rb_state19.var_9)) ^ (rb_state19.var_7)) - (rb_state19.var_10));
        rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_12) == (0x8607cf0f)) ? (rb_state19.var_11) : (0xc992e621));
        rb_state19.var_18 = (rb_state19.var_18) - (rb_state19.var_6);
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_16) == (0xfcda76a4)) ? (rb_state19.var_17) : (0x439986f4));
        rb_state19.var_20 = (rb_state19.var_20) + (((((0x842c6c2b) + (rb_state19.var_18)) ^ (0xae70c1b0)) - (0x5a2ce678)) + (rb_state19.var_19));
        rb_state19.var_15 = (rb_state19.var_15) ^ (rb_state19.var_20);
        rb_state19.var_2 = rb_state19.var_15;
        pthread_mutex_unlock(&(rb_state19.lock_21));
    }
    #endif
    yb_end = yb_addr + ye - 1;

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_0);
                if ((rb_state1.var_0) == (0xc35cf62c)) {
                    usleep(100);
                }
                if ((rb_state1.var_0) == (0xc35cf62c)) {
                    pthread_mutex_lock(&(rb_state1.lock_14));
                    rb_state1.var_15 = !((rb_state1.var_4) == (rb_state1.var_3));
                    pthread_mutex_unlock(&(rb_state1.lock_14));
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) ^ (427 < rb_input_size ? (uint32_t)rb_input[427] : 0x6da2a433);
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (0x8642da78);
                #endif
                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_2) == (0x27e842b6)) {
                    pthread_mutex_lock(&(rb_state14.lock_26));
                    rb_state14.var_5 = 0xb5cc063f;
                    rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_3);
                    rb_state14.var_7 = (rb_state14.var_7) + (((((0xd56cd2b3) + (rb_state14.var_4)) ^ (0xf15c6d3e)) + (rb_state14.var_5)) ^ (rb_state14.var_5));
                    rb_state14.var_8 = (rb_state14.var_8) - ((0xccbc8ea7) ^ (rb_state14.var_6));
                    rb_state14.var_9 = (rb_state14.var_9) + (((0x82c06b89) - (rb_state14.var_7)) ^ (rb_state14.var_6));
                    rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_7);
                    rb_state14.var_11 = (rb_state14.var_11) + (((0x688f95c) ^ (rb_state14.var_8)) ^ (rb_state14.var_8));
                    rb_state14.var_12 = (rb_state14.var_12) - (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_10) : (0x1886f243));
                    rb_state14.var_13 = (rb_state14.var_13) + (((0x3d24bc) + (rb_state14.var_9)) + (rb_state14.var_11));
                    rb_state14.var_14 = (rb_state14.var_14) ^ (((0x2191b95d) - (rb_state14.var_12)) ^ (0xf29200d4));
                    rb_state14.var_15 = (rb_state14.var_15) - (rb_state14.var_13);
                    rb_state14.var_16 = (rb_state14.var_16) ^ (rb_state14.var_14);
                    rb_state14.var_17 = (rb_state14.var_17) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_15) : (0xe276b255));
                    rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0x31ceb5c9));
                    rb_state14.var_4 = (rb_state14.var_4) - (rb_state14.var_18);
                    rb_state14.var_3 = rb_state14.var_4;
                    pthread_mutex_unlock(&(rb_state14.lock_26));
                }
                if ((rb_state14.var_2) == (0x27e842b6)) {
                    if (!((rb_state14.var_3) == (rb_state14.var_4))) {
                        racebench_trigger(14);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_20 = (rb_state16.var_20) + (rb_state16.var_21);
                rb_state16.var_22 = (rb_state16.var_22) + ((0x9915732e) + (rb_state16.var_23));
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_1 = (rb_state17.var_1) - ((0x292fb660) + (0xf9a52f8a));
                if ((rb_state17.var_0) == (0x7cf9cd14)) {
                    rb_state17.var_12 = 0xd221bec9;
                    rb_state17.var_13 = (rb_state17.var_13) - (0x32df8484);
                    rb_state17.var_14 = (rb_state17.var_14) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_6) : (0x9794eda));
                    rb_state17.var_15 = (rb_state17.var_15) - (293 < rb_input_size ? (uint32_t)rb_input[293] : 0x4193c6b8);
                    rb_state17.var_16 = (rb_state17.var_16) ^ (((((0x336ff15) + (rb_state17.var_10)) + (rb_state17.var_12)) ^ (rb_state17.var_7)) ^ (rb_state17.var_9));
                    rb_state17.var_17 = (rb_state17.var_17) + (((0xc5c713cf) ^ (rb_state17.var_13)) ^ (0x28704634));
                    rb_state17.var_18 = (rb_state17.var_18) + (((((0x8b0977d2) + (0x31963345)) + (rb_state17.var_15)) - (rb_state17.var_14)) ^ (0x45522e5d));
                    rb_state17.var_19 = (rb_state17.var_19) + (rb_state17.var_16);
                    rb_state17.var_20 = (rb_state17.var_20) ^ (rb_state17.var_17);
                    rb_state17.var_21 = (rb_state17.var_21) ^ (rb_state17.var_18);
                    rb_state17.var_22 = (rb_state17.var_22) ^ (rb_state17.var_19);
                    rb_state17.var_23 = (rb_state17.var_23) + (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0x5f03e068));
                    rb_state17.var_24 = (rb_state17.var_24) + (((0x58d4fe0e) + (rb_state17.var_22)) + (rb_state17.var_11));
                    rb_state17.var_25 = (rb_state17.var_25) ^ (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_24) : (0x8366560b));
                    rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_25);
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0x1a659580)) {
                    rb_state18.var_18 = 0x1271e809;
                    rb_state18.var_19 = (rb_state18.var_19) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_11) : (0xdd8bd6d4));
                    rb_state18.var_20 = (rb_state18.var_20) + ((0x64fde0b6) - (rb_state18.var_13));
                    rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_18) : (0xb317b609));
                    rb_state18.var_22 = (rb_state18.var_22) + (rb_state18.var_10);
                    rb_state18.var_23 = (rb_state18.var_23) ^ (rb_state18.var_19);
                    rb_state18.var_24 = (rb_state18.var_24) - (rb_state18.var_20);
                    rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_22) : (0x8b35e734));
                    rb_state18.var_26 = (rb_state18.var_26) ^ (((((0x65d058bd) ^ (rb_state18.var_24)) - (rb_state18.var_15)) - (0x8b1d8d6f)) - (rb_state18.var_23));
                    rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_25) : (0x297f6212));
                    rb_state18.var_28 = (rb_state18.var_28) - (((((0xf90a7b5d) - (rb_state18.var_26)) - (0x6e690aa2)) ^ (rb_state18.var_17)) - (rb_state18.var_27));
                    rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_28);
                }
                #endif
                xb_size = xbe;
            } else {
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_21 = (rb_state1.var_21) ^ ((0xe8ab4f7c) ^ (0x44d596db));
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) - (0x7c2bff31);
                #endif
                #ifdef RACEBENCH_BUG_10
                if ((rb_state10.var_1) == (0x2ac0fdc3)) {
                    pthread_mutex_lock(&(rb_state10.lock_25));
                    rb_state10.var_4 = 0x1ba8e479;
                    rb_state10.var_5 = 0x746da94;
                    rb_state10.var_6 = (rb_state10.var_6) + (rb_state10.var_3);
                    rb_state10.var_7 = (rb_state10.var_7) ^ (((((0x9a56adff) + (0xbf206211)) ^ (rb_state10.var_2)) ^ (rb_state10.var_4)) - (rb_state10.var_4));
                    rb_state10.var_8 = (rb_state10.var_8) ^ (rb_state10.var_5);
                    rb_state10.var_9 = (rb_state10.var_9) - (rb_state10.var_6);
                    rb_state10.var_10 = (rb_state10.var_10) ^ (((0xdce551e) - (0xbb059515)) ^ (rb_state10.var_7));
                    rb_state10.var_11 = (rb_state10.var_11) - (((0xb2b28b32) - (rb_state10.var_8)) ^ (0xb6d15aa1));
                    rb_state10.var_12 = (rb_state10.var_12) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_9) : (0xa1872dff));
                    rb_state10.var_13 = (rb_state10.var_13) ^ (((((0x662c43d7) - (rb_state10.var_11)) ^ (rb_state10.var_10)) ^ (0xa1903435)) - (rb_state10.var_6));
                    rb_state10.var_14 = (rb_state10.var_14) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_12) : (0xe8c79ea4));
                    rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_13);
                    rb_state10.var_16 = (rb_state10.var_16) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_14) : (0xb45a557d));
                    rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_15) : (0xcf826aa2));
                    rb_state10.var_18 = (rb_state10.var_18) - (((0xcfc6b8a) - (rb_state10.var_16)) + (0xfcf64438));
                    rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0x1fc3654f));
                    rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_19);
                    rb_state10.var_2 = rb_state10.var_3;
                    pthread_mutex_unlock(&(rb_state10.lock_25));
                }
                if ((rb_state10.var_1) == (0xdfce89f9)) {
                    if (!((rb_state10.var_2) == (rb_state10.var_3))) {
                        racebench_trigger(10);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_2) == (0x27e842b6)) {
                    rb_state14.var_19 = 0x12162514;
                    rb_state14.var_20 = 0xb074d969;
                    rb_state14.var_21 = (rb_state14.var_21) - (((((0xb11623e7) ^ (0xdb4d6a1e)) - (rb_state14.var_20)) + (0xc8cdfb5d)) - (rb_state14.var_19));
                    rb_state14.var_22 = (rb_state14.var_22) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_12) : (0xd07c0431));
                    rb_state14.var_23 = (rb_state14.var_23) + (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_11) : (0xd2965ebe));
                    rb_state14.var_24 = (rb_state14.var_24) + (((rb_state14.var_21) == (0x0)) ? (rb_state14.var_22) : (0xc8c49900));
                    rb_state14.var_25 = (rb_state14.var_25) + (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_24) : (0x9241fdd0));
                    rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_25);
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                if ((rb_state16.var_24) == (0x0)) {
                    pthread_mutex_lock(&(rb_state16.lock_48));
                    rb_state16.var_42 = 0x8634c7df;
                    rb_state16.var_43 = (rb_state16.var_43) + (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_25) : (0x24444580));
                    rb_state16.var_44 = (rb_state16.var_44) + (395 < rb_input_size ? (uint32_t)rb_input[395] : 0x3d62f3e2);
                    rb_state16.var_45 = (rb_state16.var_45) ^ (((rb_state16.var_42) == (0x0)) ? (rb_state16.var_43) : (0xc80aea5a));
                    rb_state16.var_46 = (rb_state16.var_46) ^ (((0xedb7bcff) ^ (0xad166ae9)) + (rb_state16.var_44));
                    rb_state16.var_47 = (rb_state16.var_47) + (((rb_state16.var_45) == (0x0)) ? (rb_state16.var_46) : (0x31d06483));
                    rb_state16.var_40 = (rb_state16.var_40) + (rb_state16.var_47);
                    rb_state16.var_41 = rb_state16.var_40;
                    pthread_mutex_unlock(&(rb_state16.lock_48));
                }
                if ((rb_state16.var_27) == (0xba05ad54)) {
                    if ((rb_state16.var_50) != (0x0)) {
                        if (!((rb_state16.var_50) == (rb_state16.var_74))) {
                            racebench_trigger(16);
                        }
                    }
                }
                #endif
                yb_size = ybe;
            } else {
                yb_size = yb_end - yb_addr + 1;
            }

            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_22) == (0xd409ae0e)) {
                rb_state1.var_26 = rb_state1.var_23;
            }
            if ((rb_state1.var_22) == (0xd409ae0e)) {
                pthread_mutex_lock(&(rb_state1.lock_47));
                if (!((rb_state1.var_23) == (rb_state1.var_26))) {
                    racebench_trigger(1);
                }
                pthread_mutex_unlock(&(rb_state1.lock_47));
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_2) : (0xcc20ab7d));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x7ff00e13)) {
                rb_state12.var_2 = rb_state12.var_1;
            }
            if ((rb_state12.var_0) == (0xacc3659b)) {
                pthread_mutex_lock(&(rb_state12.lock_6));
                if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                    racebench_trigger(12);
                }
                pthread_mutex_unlock(&(rb_state12.lock_6));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (96 < rb_input_size ? (uint32_t)rb_input[96] : 0x75141e78);
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0xbe98d21f)) {
                rb_state16.var_26 = 0x98b6599a;
                rb_state16.var_27 = 0xba05ad54;
                rb_state16.var_28 = (rb_state16.var_28) ^ (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_15) : (0xa8014d00));
                rb_state16.var_29 = (rb_state16.var_29) + (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0x1dfeb888));
                rb_state16.var_30 = (rb_state16.var_30) - (rb_state16.var_28);
                rb_state16.var_31 = (rb_state16.var_31) + (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_30) : (0x85b0c494));
                rb_state16.var_25 = (rb_state16.var_25) + (rb_state16.var_31);
                rb_state16.var_3 = rb_state16.var_25;
            }
            if ((rb_state16.var_1) == (0xbe98d21f)) {
                pthread_mutex_lock(&(rb_state16.lock_38));
                rb_state16.var_33 = 0x98d9e72d;
                rb_state16.var_34 = 0x641cbfae;
                rb_state16.var_35 = (rb_state16.var_35) ^ (rb_state16.var_33);
                rb_state16.var_36 = (rb_state16.var_36) ^ (((((0xe5072a40) ^ (rb_state16.var_17)) + (rb_state16.var_34)) + (rb_state16.var_18)) ^ (0x9b0d19e));
                rb_state16.var_37 = (rb_state16.var_37) ^ (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_36) : (0x16a8dcea));
                rb_state16.var_32 = (rb_state16.var_32) + (rb_state16.var_37);
                rb_state16.var_3 = rb_state16.var_32;
                pthread_mutex_unlock(&(rb_state16.lock_38));
            }
            #endif
            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c");

            wpentry->xpix = xb_addr;
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_10) == (0xf4792626)) ? (rb_state1.var_9) : (0x8e7a2285));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) - (189 < rb_input_size ? (uint32_t)rb_input[189] : 0x1f0d56d2);
            rb_state8.var_12 = (rb_state8.var_12) + (0x8fd8e775);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_1) : (0x30b3a3c2));
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0xe161f33e)) {
                rb_state13.var_12 = 0xf572d4ac;
                rb_state13.var_13 = (rb_state13.var_13) ^ (((((0x4b9f6704) ^ (rb_state13.var_6)) + (rb_state13.var_9)) - (rb_state13.var_8)) - (rb_state13.var_7));
                rb_state13.var_14 = (rb_state13.var_14) ^ (((0x5a1e5c16) - (rb_state13.var_12)) + (0x3cd1e1a5));
                rb_state13.var_15 = (rb_state13.var_15) ^ (((((0x5939368b) - (rb_state13.var_14)) - (rb_state13.var_10)) - (rb_state13.var_13)) - (0x48564e39));
                rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_15);
            }
            #endif
            wpentry->ypix = yb_addr;
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_15 = (rb_state2.var_15) - (569 < rb_input_size ? (uint32_t)rb_input[569] : 0x8b75f4cc);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_2 = (rb_state14.var_2) ^ ((0xe2a7c192) + (rb_state14.var_2));
            #endif
            wpentry->xdim = xb_size;
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_13) == (0x31c3a20e)) {
                rb_state1.var_25 = !((rb_state1.var_17) == (rb_state1.var_16));
            }
            #endif
            wpentry->ydim = yb_size;

            if (!gm->workpool[pid][0]) {
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) - (541 < rb_input_size ? (uint32_t)rb_input[541] : 0xd7f6db04);
                #endif
                wpentry->next = NULL;
            } else {
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_14) == (0xcf5273f1)) {
                    pthread_mutex_lock(&(rb_state8.lock_56));
                    if ((rb_state8.var_23) != (0x0)) {
                        if (!((rb_state8.var_23) == (rb_state8.var_48))) {
                            racebench_trigger(8);
                        }
                    }
                    pthread_mutex_unlock(&(rb_state8.lock_56));
                }
                #endif
                wpentry->next = gm->workpool[pid][0];
            }

            #ifdef RACEBENCH_BUG_16
            rb_state16.var_19 = (rb_state16.var_19) - ((0xbf4ae237) - (0xdf6f9dd3));
            #endif
            gm->workpool[pid][0] = wpentry;
            xb_addr += xbe;
        }

        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ ((0x254a6bfe) + (rb_state17.var_1));
        #endif
        xb_addr = xs;
        yb_addr += ybe;
    }
}

INT GetJob(job, pid)
RAYJOB *job;
INT pid;
{
    WPJOB *wpentry;

    { pthread_mutex_lock(&gm->wplock[pid]); }
    wpentry = gm->workpool[pid][0];

    if (!wpentry) {
        gm->wpstat[pid][0] = WPS_EMPTY;
        { pthread_mutex_unlock(&gm->wplock[pid]); }
        return (WPS_EMPTY);
    }

    gm->workpool[pid][0] = wpentry->next;
    { pthread_mutex_unlock(&gm->wplock[pid]); }

    job->x = wpentry->xpix;
    job->y = wpentry->ypix;
    job->xcurr = wpentry->xpix;
    job->ycurr = wpentry->ypix;
    job->xlen = wpentry->xdim;
    job->ylen = wpentry->ydim;

    GlobalFree(wpentry);
    return (WPS_VALID);
}

INT GetJobs(job, pid)
RAYJOB *job;
INT pid;
{
    INT i;

    i = pid;

    if (gm->wpstat[i][0] == WPS_VALID) {
        if (GetJob(job, i) == WPS_VALID) {
            return (WPS_VALID);
        }
    }

    i = (pid + 1) % gm->nprocs;

    while (i != pid) {
        if (gm->wpstat[i][0] == WPS_VALID) {
            if (GetJob(job, i) == WPS_VALID) {
                return (WPS_VALID);
            }
        }

        i = (i + 1) % gm->nprocs;
    }

    return (WPS_EMPTY);
}

VOID PrintWorkPool(pid)
INT pid;
{
    WPJOB *j;

    j = gm->workpool[pid][0];

    while (j) {
        printf("Workpool entry:    pid=%3ld    xs=%3ld    ys=%3ld    xe=%3ld    ye=%3ld\n", pid, j->xpix, j->ypix, j->xdim, j->ydim);
        j = j->next;
    }
}

VOID InitWorkPool(pid)
INT pid;
{
    INT i;
    INT x, y;
    INT xe, ye;
    INT xsize, ysize;

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
    rb_state6.var_0 = (rb_state6.var_0) ^ ((0x21256b43) + (rb_state6.var_0));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_1) : (0x41eb7a95));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xe36e4c6f)) {
        rb_state9.var_3 = rb_state9.var_1;
    }
    if ((rb_state9.var_0) == (0xe36e4c6f)) {
        if (!((rb_state9.var_1) == (rb_state9.var_3))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0x1d19179c) - (0xe7f70ea0));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_2) == (0x35eae44e)) {
        rb_state15.var_3 = rb_state15.var_0;
    }
    if ((rb_state15.var_2) == (0x35eae44e)) {
        if (!((rb_state15.var_0) == (rb_state15.var_3))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (0x46ecdd03);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - ((0x3b4dc7b1) ^ (0x2519cc54));
    #endif
    gm->wpstat[pid][0] = WPS_VALID;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x7e6232b9);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x3196916);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (0xf4a9aae4);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0xed4de200));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - ((0xc12afb61) ^ (rb_state18.var_1));
    #endif
    gm->workpool[pid][0] = NULL;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_1);
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x6b1eee6a)) {
        pthread_mutex_lock(&(rb_state5.lock_16));
        rb_state5.var_5 = 0xe72f0639;
        rb_state5.var_6 = (rb_state5.var_6) ^ (357 < rb_input_size ? (uint32_t)rb_input[357] : 0x1babfaa3);
        rb_state5.var_7 = (rb_state5.var_7) + (((((0x30c3caa2) + (rb_state5.var_5)) + (rb_state5.var_4)) + (0xddc1732b)) + (rb_state5.var_3));
        rb_state5.var_8 = (rb_state5.var_8) ^ (((((0x6dd4894a) + (0x2e15613f)) ^ (0x84eaeb6c)) + (rb_state5.var_5)) - (rb_state5.var_6));
        rb_state5.var_9 = (rb_state5.var_9) ^ (((((0xd5e6fae9) - (rb_state5.var_7)) ^ (rb_state5.var_6)) ^ (rb_state5.var_8)) ^ (0x3724360c));
        rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_9);
        rb_state5.var_3 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_16));
    }
    if ((rb_state5.var_1) == (0x6b1eee6a)) {
        if (!((rb_state5.var_3) == (rb_state5.var_4))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0x52b072a7) ^ (0xe2364b2d));
    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0xd75c3de1));
    if ((rb_state7.var_2) == (0xe08c369)) {
        rb_state7.var_11 = 0x424da0ba;
        rb_state7.var_12 = (rb_state7.var_12) - ((0x59eb1269) ^ (rb_state7.var_5));
        rb_state7.var_13 = (rb_state7.var_13) + (0xfaa40c34);
        rb_state7.var_14 = (rb_state7.var_14) - (rb_state7.var_11);
        rb_state7.var_15 = (rb_state7.var_15) - (((0xe2fd2e) + (rb_state7.var_4)) - (rb_state7.var_6));
        rb_state7.var_16 = (rb_state7.var_16) + (((0x765051e2) ^ (rb_state7.var_7)) - (rb_state7.var_3));
        rb_state7.var_17 = (rb_state7.var_17) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_12) : (0x294fad7b));
        rb_state7.var_18 = (rb_state7.var_18) - (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_14) : (0xfc4e1450));
        rb_state7.var_19 = (rb_state7.var_19) - (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_16) : (0x627e011c));
        rb_state7.var_20 = (rb_state7.var_20) - (((((0xcd9f6eed) - (rb_state7.var_17)) + (0x67b56a43)) - (rb_state7.var_9)) - (rb_state7.var_18));
        rb_state7.var_21 = (rb_state7.var_21) + (rb_state7.var_19);
        rb_state7.var_22 = (rb_state7.var_22) ^ (((((0x53d73d8d) - (0xe0c5babd)) - (rb_state7.var_20)) + (rb_state7.var_21)) - (0xab5fbc02));
        rb_state7.var_10 = (rb_state7.var_10) - (rb_state7.var_22);
        rb_state7.var_3 = rb_state7.var_10;
    }
    if ((rb_state7.var_2) == (0xe08c369)) {
        pthread_mutex_lock(&(rb_state7.lock_35));
        rb_state7.var_24 = 0xed43cca5;
        rb_state7.var_25 = 0xa3da2256;
        rb_state7.var_26 = (rb_state7.var_26) - (578 < rb_input_size ? (uint32_t)rb_input[578] : 0x216799b0);
        rb_state7.var_27 = (rb_state7.var_27) - (((rb_state7.var_25) == (0x0)) ? (rb_state7.var_24) : (0xa80e9cfb));
        rb_state7.var_28 = (rb_state7.var_28) - (((((0xbef21cb4) + (0x4bc5542e)) ^ (rb_state7.var_10)) ^ (rb_state7.var_26)) - (0x10c9b9a4));
        rb_state7.var_29 = (rb_state7.var_29) - (rb_state7.var_27);
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_28) : (0x9d6e340b));
        rb_state7.var_31 = (rb_state7.var_31) - (((0x8ecb3a2d) - (rb_state7.var_29)) - (rb_state7.var_12));
        rb_state7.var_32 = (rb_state7.var_32) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_30) : (0x75861875));
        rb_state7.var_33 = (rb_state7.var_33) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_31) : (0x797a423a));
        rb_state7.var_34 = (rb_state7.var_34) + (((rb_state7.var_32) == (0x0)) ? (rb_state7.var_33) : (0x3f5a19f9));
        rb_state7.var_23 = (rb_state7.var_23) + (rb_state7.var_34);
        rb_state7.var_3 = rb_state7.var_23;
        pthread_mutex_unlock(&(rb_state7.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_1) : (0x21e3d562));
    rb_state11.var_2 = (rb_state11.var_2) ^ ((0xcbc4cb7) + (rb_state11.var_2));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_1) : (0xb10326b3));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (318 < rb_input_size ? (uint32_t)rb_input[318] : 0x6124fa18);
    #endif
    i = 0;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (213 < rb_input_size ? (uint32_t)rb_input[213] : 0xd8f6b65a);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x2c98c5ec));
    if ((rb_state4.var_2) == (0xb495887e)) {
        pthread_mutex_lock(&(rb_state4.lock_33));
        rb_state4.var_21 = 0x109cd386;
        rb_state4.var_22 = 0x577b545c;
        rb_state4.var_23 = (rb_state4.var_23) + (((((0x9353999c) - (0x6c0b6bb3)) + (rb_state4.var_18)) - (rb_state4.var_17)) - (rb_state4.var_22));
        rb_state4.var_24 = (rb_state4.var_24) - (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_19) : (0x6d413e9));
        rb_state4.var_25 = (rb_state4.var_25) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_21) : (0x46d7e35a));
        rb_state4.var_26 = (rb_state4.var_26) ^ (((0xf5722588) ^ (rb_state4.var_22)) + (rb_state4.var_23));
        rb_state4.var_27 = (rb_state4.var_27) - (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_24) : (0x188679a7));
        rb_state4.var_28 = (rb_state4.var_28) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_25) : (0x70089bf2));
        rb_state4.var_29 = (rb_state4.var_29) + (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_26) : (0x4aca8b0b));
        rb_state4.var_30 = (rb_state4.var_30) ^ (((((0xfc28163) + (rb_state4.var_26)) + (rb_state4.var_27)) ^ (0xebd623d9)) + (rb_state4.var_28));
        rb_state4.var_31 = (rb_state4.var_31) + (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_29) : (0x6db2b7e4));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_31) : (0xc277f0d3));
        rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_32);
        pthread_mutex_unlock(&(rb_state4.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xb0079542)) {
        if ((rb_state6.var_2) != (0x0)) {
            if (!((rb_state6.var_2) == (rb_state6.var_25))) {
                racebench_trigger(6);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_2) == (0x35eae44e)) {
        rb_state15.var_4 = 0xbd7aa9fe;
        rb_state15.var_5 = (rb_state15.var_5) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_4) : (0x3a75d9b7));
        rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_1) == (0x3ed5049f)) ? (rb_state18.var_0) : (0x4cd41db1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (0x48bb3bcb);
    if ((rb_state19.var_0) == (0x6890b8ba)) {
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_15))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    xsize = Display.xres / blockx;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x6b1eee6a)) {
        rb_state5.var_10 = 0x226bbd41;
        rb_state5.var_11 = 0x1a2ecf57;
        rb_state5.var_12 = (rb_state5.var_12) + (((((0x551a6e29) - (rb_state5.var_9)) ^ (rb_state5.var_11)) + (rb_state5.var_7)) ^ (rb_state5.var_8));
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_10) : (0xeb48ffe9));
        rb_state5.var_14 = (rb_state5.var_14) - (rb_state5.var_12);
        rb_state5.var_15 = (rb_state5.var_15) - (((((0xea572592) ^ (rb_state5.var_11)) - (rb_state5.var_14)) + (rb_state5.var_12)) ^ (rb_state5.var_13));
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (499 < rb_input_size ? (uint32_t)rb_input[499] : 0x6b6fc8b3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (241 < rb_input_size ? (uint32_t)rb_input[241] : 0xbc9c2d36);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ ((0x21520c6a) - (0xe1db4a5a));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - ((0xeb454d6f) ^ (0xca58f068));
    rb_state13.var_0 = (rb_state13.var_0) - (0x8b94c3c);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0xcaddf704)) ? (rb_state14.var_1) : (0x558b9d3b));
    #endif
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x7b269810)) ? (rb_state0.var_0) : (0x8bb50510));
            if ((rb_state0.var_1) == (0xee17391f)) {
                if ((rb_state0.var_2) != (0x0)) {
                    if (!((rb_state0.var_2) == (rb_state0.var_26))) {
                        racebench_trigger(0);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_2) == (0xddc009a4)) {
                rb_state2.var_18 = 0xea6a6adf;
                rb_state2.var_19 = 0xa359f94;
                rb_state2.var_20 = (rb_state2.var_20) ^ (0x99209cc7);
                rb_state2.var_21 = (rb_state2.var_21) - (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_9) : (0x92bac658));
                rb_state2.var_22 = (rb_state2.var_22) ^ (((((0x4b11ad9e) + (rb_state2.var_18)) ^ (rb_state2.var_11)) + (rb_state2.var_8)) ^ (0x396c8b39));
                rb_state2.var_23 = (rb_state2.var_23) ^ (((((0x85ad1d8b) - (rb_state2.var_13)) ^ (rb_state2.var_19)) - (rb_state2.var_20)) ^ (rb_state2.var_12));
                rb_state2.var_24 = (rb_state2.var_24) - (((((0xac719e2b) ^ (rb_state2.var_22)) - (0x418c5bb5)) ^ (rb_state2.var_21)) + (0xb0fbca6b));
                rb_state2.var_25 = (rb_state2.var_25) ^ (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_23) : (0x97af371));
                rb_state2.var_26 = (rb_state2.var_26) ^ (((rb_state2.var_24) == (0x0)) ? (rb_state2.var_25) : (0x67819664));
                rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_26);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (0xad43638b);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) - ((0x67f1c4fe) + (0x52a35a4a));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (0xd4dbb39b);
            #endif
            ye = ysize;
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_7 = (rb_state1.var_7) ^ (529 < rb_input_size ? (uint32_t)rb_input[529] : 0x7d62157b);
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_1 = (rb_state2.var_1) ^ (0x7e149a96);
                rb_state2.var_20 = (rb_state2.var_20) - (0xf8a899c6);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) - (77 < rb_input_size ? (uint32_t)rb_input[77] : 0x3bb9e823);
                if ((rb_state11.var_2) == (0x76594979)) {
                    pthread_mutex_lock(&(rb_state11.lock_4));
                    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_3);
                    pthread_mutex_unlock(&(rb_state11.lock_4));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) - (0x9ca967ad);
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0xade9df7e)) ? (rb_state14.var_0) : (0x5ecdc53e));
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_0 = (rb_state18.var_0) - (0xd59344d9);
                #endif
                xe = xsize;
            }

            if (i == pid) {
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_1 = (rb_state0.var_1) ^ (410 < rb_input_size ? (uint32_t)rb_input[410] : 0xe6b37de1);
                #endif
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_2);
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_2 = (rb_state7.var_2) ^ (361 < rb_input_size ? (uint32_t)rb_input[361] : 0xe0a59589);
                #endif
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_0) == (0x16f8044a)) {
                    if ((rb_state8.var_5) != (0x0)) {
                        rb_state8.var_22 = !((rb_state8.var_3) == (rb_state8.var_4));
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0xbf19b980)) ? (rb_state19.var_1) : (0x55e10805));
                #endif
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_1) == (0x0)) {
                pthread_mutex_lock(&(rb_state3.lock_6));
                rb_state3.var_4 = 0xa32b7a82;
                rb_state3.var_5 = (rb_state3.var_5) - (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_3) : (0x6365f1fe));
                rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_5);
                pthread_mutex_unlock(&(rb_state3.lock_6));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) + (137 < rb_input_size ? (uint32_t)rb_input[137] : 0x2e125b04);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + ((0x341c72ce) - (rb_state12.var_1));
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) + (301 < rb_input_size ? (uint32_t)rb_input[301] : 0x138f06bf);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) + (0x9ac01160);
            #endif
            i = (i + 1) % gm->nprocs;
        }
    }
}