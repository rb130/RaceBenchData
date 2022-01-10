#include "racebench_bugs.h"


#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "tpool.h"

tpool_t *tpool_create(tdesc_t *opts, int nthreads) {
    int i;
    tpool_t *pool;
    const pthread_attr_t *attr;
    void *arg;
    int rv;

    if (opts == NULL || nthreads < 1) {
        return NULL;
    }
    for (i = 0; i < nthreads; i++) {
        if (opts[i].start_routine == NULL) {
            return NULL;
        }
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xae0eba73));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x7c6198c1)) {
        pthread_mutex_lock(&(rb_state5.lock_26));
        rb_state5.var_3 = 0xbfd2419a;
        rb_state5.var_4 = (rb_state5.var_4) ^ ((0x8612aad0) + (rb_state5.var_3));
        rb_state5.var_5 = (rb_state5.var_5) - (((((0xf0e06f9d) + (rb_state5.var_2)) + (rb_state5.var_4)) + (rb_state5.var_1)) - (0x89160e09));
        rb_state5.var_6 = (rb_state5.var_6) ^ (301 < rb_input_size ? (uint32_t)rb_input[301] : 0xe3bd839b);
        rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_5) == (0x5250b202)) ? (rb_state5.var_3) : (0x34fa2256));
        rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_6) == (0x2)) ? (rb_state5.var_4) : (0xbeb5d904));
        rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_7) == (0x402dbe66)) ? (rb_state5.var_5) : (0x3d8d2260));
        rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_8) == (0xba1b1396)) ? (rb_state5.var_6) : (0xcb29ef7e));
        rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_9) == (0x5250b202)) ? (rb_state5.var_7) : (0x853bb871));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((0xeb190c63) - (0x9b01e740)) - (rb_state5.var_8));
        rb_state5.var_13 = (rb_state5.var_13) - (((0x5012394) ^ (rb_state5.var_9)) - (0xb89c7f2));
        rb_state5.var_14 = (rb_state5.var_14) ^ (((((0x8450b9ec) - (rb_state5.var_10)) ^ (0x17347621)) - (rb_state5.var_11)) - (rb_state5.var_10));
        rb_state5.var_15 = (rb_state5.var_15) ^ (((rb_state5.var_11) == (0x402dbe66)) ? (rb_state5.var_12) : (0x16861a40));
        rb_state5.var_16 = (rb_state5.var_16) ^ (((((0x5404b79f) ^ (rb_state5.var_13)) + (rb_state5.var_13)) ^ (rb_state5.var_14)) + (rb_state5.var_12));
        rb_state5.var_17 = (rb_state5.var_17) ^ (((0x3d74dee8) - (rb_state5.var_15)) ^ (rb_state5.var_14));
        rb_state5.var_18 = (rb_state5.var_18) ^ (((((0x2144352d) - (rb_state5.var_17)) - (rb_state5.var_15)) ^ (rb_state5.var_16)) ^ (0x1f326617));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_18);
        rb_state5.var_1 = rb_state5.var_2;
        pthread_mutex_unlock(&(rb_state5.lock_26));
    }
    if ((rb_state5.var_0) == (0x7c6198c1)) {
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - ((0xe303c371) + (rb_state6.var_0));
    #endif
    pool = (tpool_t *)malloc(sizeof(tpool_t));
    if (pool == NULL) {
        return NULL;
    }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0x27487e8e) + (rb_state1.var_0));
    #endif
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    for (i = 0; i < nthreads; i++) {
        if (opts[i].attr == NULL) {
            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_0) == (0x61e1eadc)) {
                rb_state0.var_3 = 0x7783b23a;
                rb_state0.var_4 = (rb_state0.var_4) + (((0x93acee1b) + (rb_state0.var_2)) - (rb_state0.var_3));
                rb_state0.var_5 = (rb_state0.var_5) ^ (((((0xbf6ebc87) + (rb_state0.var_3)) + (rb_state0.var_1)) + (0x6ea49758)) + (0xd9ecbd7f));
                rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_4);
                rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_5) == (0x7f83c398)) ? (rb_state0.var_4) : (0x7bbe09b3));
                rb_state0.var_8 = (rb_state0.var_8) - (rb_state0.var_5);
                rb_state0.var_9 = (rb_state0.var_9) + (rb_state0.var_6);
                rb_state0.var_10 = (rb_state0.var_10) - (((rb_state0.var_7) == (0x1c293be1)) ? (rb_state0.var_8) : (0xab2a7409));
                rb_state0.var_11 = (rb_state0.var_11) ^ (((((0xc909d901) - (0xb5d1d0ea)) + (rb_state0.var_10)) - (0xea027e69)) + (rb_state0.var_9));
                rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_11);
            }
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_0) == (0x0)) {
                pthread_mutex_lock(&(rb_state2.lock_24));
                rb_state2.var_3 = 0xdbc0d759;
                rb_state2.var_4 = 0x7b5ff37;
                rb_state2.var_5 = (rb_state2.var_5) ^ (0x45c16a16);
                rb_state2.var_6 = (rb_state2.var_6) ^ (((0xa9c70f37) - (rb_state2.var_4)) + (rb_state2.var_2));
                rb_state2.var_7 = (rb_state2.var_7) - (rb_state2.var_3);
                rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_1);
                rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_3) == (0xdbc0d759)) ? (rb_state2.var_5) : (0xf00ffa52));
                rb_state2.var_10 = (rb_state2.var_10) - (((rb_state2.var_4) == (0x7b5ff37)) ? (rb_state2.var_6) : (0x119acb14));
                rb_state2.var_11 = (rb_state2.var_11) + (((rb_state2.var_7) == (0x243f28a7)) ? (rb_state2.var_8) : (0x6f84de11));
                rb_state2.var_12 = (rb_state2.var_12) + (((0xc5e68ec) + (rb_state2.var_9)) - (rb_state2.var_5));
                rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_6) == (0xa2111000)) ? (rb_state2.var_10) : (0xfaec28f9));
                rb_state2.var_14 = (rb_state2.var_14) + (((((0x73e48063) + (0x7824ad6e)) ^ (0x5c6b7fdf)) ^ (rb_state2.var_12)) ^ (rb_state2.var_11));
                rb_state2.var_15 = (rb_state2.var_15) - (((rb_state2.var_7) == (0x243f28a7)) ? (rb_state2.var_13) : (0xa86c8892));
                rb_state2.var_16 = (rb_state2.var_16) ^ (((rb_state2.var_14) == (0x30b9c6ce)) ? (rb_state2.var_15) : (0x7696b9dd));
                rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_16);
                rb_state2.var_1 = rb_state2.var_2;
                pthread_mutex_unlock(&(rb_state2.lock_24));
            }
            if ((rb_state2.var_0) == (0x0)) {
                if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                    racebench_trigger(2);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x9b1e5f09)) {
                pthread_mutex_lock(&(rb_state12.lock_26));
                if ((rb_state12.var_1) != (0x0)) {
                    if (!((rb_state12.var_1) == (rb_state12.var_21))) {
                        racebench_trigger(12);
                    }
                }
                pthread_mutex_unlock(&(rb_state12.lock_26));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_0) == (0x0)) {
                if ((rb_state17.var_1) != (0x0)) {
                    if (!((rb_state17.var_1) == (rb_state17.var_5))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            attr = NULL;
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_8) == (0x0)) {
                rb_state18.var_31 = 0x8a35d467;
                rb_state18.var_32 = (rb_state18.var_32) ^ (rb_state18.var_21);
                rb_state18.var_33 = (rb_state18.var_33) + ((0x15ed4108) + (rb_state18.var_22));
                rb_state18.var_34 = (rb_state18.var_34) - (rb_state18.var_19);
                rb_state18.var_35 = (rb_state18.var_35) + (rb_state18.var_31);
                rb_state18.var_36 = (rb_state18.var_36) ^ (((rb_state18.var_23) == (0x0)) ? (rb_state18.var_20) : (0x826d3080));
                rb_state18.var_37 = (rb_state18.var_37) ^ (((((0x4578e5a8) + (rb_state18.var_32)) + (rb_state18.var_25)) - (rb_state18.var_24)) ^ (rb_state18.var_33));
                rb_state18.var_38 = (rb_state18.var_38) + (rb_state18.var_34);
                rb_state18.var_39 = (rb_state18.var_39) ^ (((rb_state18.var_26) == (0x0)) ? (rb_state18.var_35) : (0x74293401));
                rb_state18.var_40 = (rb_state18.var_40) ^ (((rb_state18.var_36) == (0x0)) ? (rb_state18.var_37) : (0x5354034f));
                rb_state18.var_41 = (rb_state18.var_41) ^ (((rb_state18.var_38) == (0x0)) ? (rb_state18.var_39) : (0x1cfa85a));
                rb_state18.var_42 = (rb_state18.var_42) ^ (rb_state18.var_40);
                rb_state18.var_43 = (rb_state18.var_43) - (((rb_state18.var_41) == (0x0)) ? (rb_state18.var_42) : (0x5fdebe9d));
                rb_state18.var_30 = (rb_state18.var_30) - (rb_state18.var_43);
                rb_state18.var_9 = rb_state18.var_30;
            }
            if ((rb_state18.var_8) == (0x0)) {
                pthread_mutex_lock(&(rb_state18.lock_68));
                rb_state18.var_45 = 0x1fe87a01;
                rb_state18.var_46 = (rb_state18.var_46) - ((0x12e3933) - (rb_state18.var_29));
                rb_state18.var_47 = (rb_state18.var_47) - (((rb_state18.var_31) == (0x0)) ? (rb_state18.var_30) : (0x98208a9c));
                rb_state18.var_48 = (rb_state18.var_48) + (((0x2973070e) + (rb_state18.var_32)) + (rb_state18.var_27));
                rb_state18.var_49 = (rb_state18.var_49) + (rb_state18.var_45);
                rb_state18.var_50 = (rb_state18.var_50) + (((0x2e92881a) ^ (rb_state18.var_33)) - (rb_state18.var_28));
                rb_state18.var_51 = (rb_state18.var_51) + (((rb_state18.var_46) == (0x0)) ? (rb_state18.var_47) : (0x28ef0f9b));
                rb_state18.var_52 = (rb_state18.var_52) ^ (((((0x54ffb1e8) - (rb_state18.var_49)) ^ (0xd3b832c4)) + (0xd33ee1d7)) - (rb_state18.var_48));
                rb_state18.var_53 = (rb_state18.var_53) - (rb_state18.var_50);
                rb_state18.var_54 = (rb_state18.var_54) + (((0xafb05b70) - (rb_state18.var_51)) ^ (rb_state18.var_34));
                rb_state18.var_55 = (rb_state18.var_55) + (rb_state18.var_52);
                rb_state18.var_56 = (rb_state18.var_56) - (rb_state18.var_53);
                rb_state18.var_57 = (rb_state18.var_57) + (((0x6b05b054) - (rb_state18.var_35)) ^ (rb_state18.var_54));
                rb_state18.var_58 = (rb_state18.var_58) + (((rb_state18.var_55) == (0x0)) ? (rb_state18.var_56) : (0x914e193e));
                rb_state18.var_59 = (rb_state18.var_59) + (((rb_state18.var_36) == (0x0)) ? (rb_state18.var_57) : (0x5ec2014c));
                rb_state18.var_60 = (rb_state18.var_60) - (rb_state18.var_58);
                rb_state18.var_61 = (rb_state18.var_61) + (rb_state18.var_59);
                rb_state18.var_62 = (rb_state18.var_62) + (((rb_state18.var_37) == (0x0)) ? (rb_state18.var_60) : (0x4a8d6579));
                rb_state18.var_63 = (rb_state18.var_63) ^ (((rb_state18.var_38) == (0x0)) ? (rb_state18.var_61) : (0x7e1e9f40));
                rb_state18.var_64 = (rb_state18.var_64) + (rb_state18.var_62);
                rb_state18.var_65 = (rb_state18.var_65) ^ (((0x1d873616) ^ (rb_state18.var_39)) + (rb_state18.var_63));
                rb_state18.var_66 = (rb_state18.var_66) + (((rb_state18.var_40) == (0x0)) ? (rb_state18.var_64) : (0xab9c391d));
                rb_state18.var_67 = (rb_state18.var_67) ^ (((rb_state18.var_65) == (0x0)) ? (rb_state18.var_66) : (0x59a8f846));
                rb_state18.var_44 = (rb_state18.var_44) ^ (rb_state18.var_67);
                rb_state18.var_9 = rb_state18.var_44;
                pthread_mutex_unlock(&(rb_state18.lock_68));
            }
            #endif
            arg = NULL;
        } else {
            arg = opts[i].arg;
        }

        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x9e0e61e0)) {
            if ((rb_state9.var_1) != (0x0)) {
                if (!((rb_state9.var_1) == (rb_state9.var_27))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        rv = pthread_create(&(pool->threads[i]), attr, opts[i].start_routine, arg);
        if (rv != 0) {
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0xd9ad3415) - (0xf3af8d5f));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (565 < rb_input_size ? (uint32_t)rb_input[565] : 0x8928ed33);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x931e13a9)) {
        rb_state7.var_3 = 0x1b2e8a54;
        rb_state7.var_4 = (rb_state7.var_4) + (0x9598fc58);
        rb_state7.var_5 = (rb_state7.var_5) + (((0x4cf05e5f) ^ (rb_state7.var_3)) ^ (rb_state7.var_4));
        rb_state7.var_6 = (rb_state7.var_6) ^ (0x18c8c3c0);
        rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_5) == (0xc2462853)) ? (rb_state7.var_2) : (0x1d35c91f));
        rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_4) == (0x9598fc58)) ? (rb_state7.var_5) : (0xf4fcd315));
        rb_state7.var_9 = (rb_state7.var_9) - (((((0x32b23412) ^ (rb_state7.var_7)) + (rb_state7.var_7)) ^ (rb_state7.var_6)) - (rb_state7.var_6));
        rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_8);
        rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_9) == (0xee4dcbee)) ? (rb_state7.var_10) : (0x888df172));
        rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ ((0xb4ec4e78) + (0xab124bbd));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x7bb59249));
    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0xcc830c3e);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x74dafcbf)) {
        pthread_mutex_lock(&(rb_state18.lock_7));
        rb_state18.var_3 = 0xe5eaa19a;
        rb_state18.var_4 = (rb_state18.var_4) + (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_3) : (0x3d5244ae));
        rb_state18.var_5 = (rb_state18.var_5) + (((((0xd3aa5b72) + (rb_state18.var_2)) ^ (0xaf2a3504)) + (rb_state18.var_1)) - (rb_state18.var_4));
        rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_5) : (0x51074190));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_6);
        rb_state18.var_2 = rb_state18.var_1;
        pthread_mutex_unlock(&(rb_state18.lock_7));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0xa1)) {
        if ((rb_state19.var_1) != (0x0)) {
            if (!((rb_state19.var_1) == (rb_state19.var_23))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    pool->nthreads = nthreads;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_8) == (0x4496ad41)) {
        pthread_mutex_lock(&(rb_state0.lock_49));
        rb_state0.var_16 = 0x27a87660;
        rb_state0.var_17 = 0x8026977b;
        rb_state0.var_18 = (rb_state0.var_18) + (((((0xac0afa68) - (0x55d0e909)) - (rb_state0.var_10)) ^ (rb_state0.var_17)) - (rb_state0.var_9));
        rb_state0.var_19 = (rb_state0.var_19) ^ (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_11) : (0x2709b4ff));
        rb_state0.var_20 = (rb_state0.var_20) ^ (((0x4cd19271) - (0x2e892b4d)) - (rb_state0.var_16));
        rb_state0.var_21 = (rb_state0.var_21) + (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_18) : (0x3aa4f1a3));
        rb_state0.var_22 = (rb_state0.var_22) - (rb_state0.var_19);
        rb_state0.var_23 = (rb_state0.var_23) - (((0x53fea168) + (rb_state0.var_15)) ^ (rb_state0.var_20));
        rb_state0.var_24 = (rb_state0.var_24) ^ (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_22) : (0xca503ef3));
        rb_state0.var_25 = (rb_state0.var_25) + (rb_state0.var_23);
        rb_state0.var_26 = (rb_state0.var_26) + (((((0x529d9402) + (rb_state0.var_25)) ^ (rb_state0.var_16)) - (rb_state0.var_24)) - (0xd2329153));
        rb_state0.var_15 = (rb_state0.var_15) + (rb_state0.var_26);
        rb_state0.var_14 = rb_state0.var_15;
        pthread_mutex_unlock(&(rb_state0.lock_49));
    }
    if ((rb_state0.var_8) == (0x4496ad41)) {
        rb_state0.var_50 = !((rb_state0.var_14) == (rb_state0.var_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0xd8b78172)) {
        pthread_mutex_lock(&(rb_state1.lock_18));
        rb_state1.var_3 = 0x8c31040f;
        rb_state1.var_4 = (rb_state1.var_4) - (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_3) : (0x65ddfd));
        rb_state1.var_5 = (rb_state1.var_5) + ((0x3cd2115a) ^ (rb_state1.var_4));
        rb_state1.var_6 = (rb_state1.var_6) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_1) : (0x1c78c1a0));
        rb_state1.var_7 = (rb_state1.var_7) + (((0xeee43d9c) - (rb_state1.var_2)) + (rb_state1.var_6));
        rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_4) : (0x30bc3134));
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_5) : (0x7d6ad309));
        rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_6) : (0x9a2c467f));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_7) : (0x6886b63b));
        rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_9) : (0x37d28df6));
        rb_state1.var_13 = (rb_state1.var_13) - (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_10) : (0x357cbffd));
        rb_state1.var_14 = (rb_state1.var_14) - (((((0xb5d90446) - (rb_state1.var_12)) - (rb_state1.var_11)) ^ (0x31264c0d)) + (rb_state1.var_12));
        rb_state1.var_15 = (rb_state1.var_15) + (((((0x84e0ab4d) - (rb_state1.var_13)) - (rb_state1.var_13)) ^ (rb_state1.var_14)) - (rb_state1.var_14));
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_15);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_18));
    }
    if ((rb_state1.var_0) == (0xd8b78172)) {
        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_19 = 0x3780f92b;
        rb_state3.var_20 = 0x19642505;
        rb_state3.var_21 = (rb_state3.var_21) - (((((0x32736150) + (rb_state3.var_20)) - (rb_state3.var_12)) - (rb_state3.var_11)) + (rb_state3.var_13));
        rb_state3.var_22 = (rb_state3.var_22) ^ (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_19) : (0x56090546));
        rb_state3.var_23 = (rb_state3.var_23) ^ (((0xd3b4bc2b) + (rb_state3.var_15)) + (rb_state3.var_21));
        rb_state3.var_24 = (rb_state3.var_24) ^ (rb_state3.var_22);
        rb_state3.var_25 = (rb_state3.var_25) - (((((0xec5ae168) + (0x47e7fc1d)) - (rb_state3.var_16)) ^ (rb_state3.var_23)) - (rb_state3.var_24));
        rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_25);
        rb_state3.var_1 = rb_state3.var_18;
    }
    if ((rb_state3.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_51));
        rb_state3.var_27 = 0x19b51476;
        rb_state3.var_28 = 0x5ae59224;
        rb_state3.var_29 = (rb_state3.var_29) + (0x5d6329f9);
        rb_state3.var_30 = (rb_state3.var_30) ^ (((0x40f1772) + (rb_state3.var_17)) - (rb_state3.var_18));
        rb_state3.var_31 = (rb_state3.var_31) - (rb_state3.var_27);
        rb_state3.var_32 = (rb_state3.var_32) + (((0xecea2eec) - (rb_state3.var_19)) - (rb_state3.var_28));
        rb_state3.var_33 = (rb_state3.var_33) - (((0x531720) - (rb_state3.var_29)) + (0x876aa6f0));
        rb_state3.var_34 = (rb_state3.var_34) - (rb_state3.var_30);
        rb_state3.var_35 = (rb_state3.var_35) - (((rb_state3.var_20) == (0x19642505)) ? (rb_state3.var_31) : (0x57723de9));
        rb_state3.var_36 = (rb_state3.var_36) ^ (((0x3501227b) + (rb_state3.var_21)) + (rb_state3.var_32));
        rb_state3.var_37 = (rb_state3.var_37) - (rb_state3.var_33);
        rb_state3.var_38 = (rb_state3.var_38) ^ (rb_state3.var_34);
        rb_state3.var_39 = (rb_state3.var_39) + (rb_state3.var_35);
        rb_state3.var_40 = (rb_state3.var_40) ^ (((0x4f260593) - (rb_state3.var_36)) + (0x7faadd9d));
        rb_state3.var_41 = (rb_state3.var_41) ^ (rb_state3.var_37);
        rb_state3.var_42 = (rb_state3.var_42) - (rb_state3.var_38);
        rb_state3.var_43 = (rb_state3.var_43) + (((0x31bcf2d0) + (0xdd1dc47)) - (rb_state3.var_39));
        rb_state3.var_44 = (rb_state3.var_44) - (((rb_state3.var_22) == (0x3780f92b)) ? (rb_state3.var_40) : (0xbe4360c5));
        rb_state3.var_45 = (rb_state3.var_45) + (rb_state3.var_41);
        rb_state3.var_46 = (rb_state3.var_46) - (((rb_state3.var_42) == (0x87f0284a)) ? (rb_state3.var_43) : (0xe183e72c));
        rb_state3.var_47 = (rb_state3.var_47) + (((rb_state3.var_44) == (0x74dc5c93)) ? (rb_state3.var_45) : (0x7d659ce0));
        rb_state3.var_48 = (rb_state3.var_48) - (((rb_state3.var_23) == (0x87dd35d6)) ? (rb_state3.var_46) : (0xfa7e010e));
        rb_state3.var_49 = (rb_state3.var_49) ^ (((rb_state3.var_24) == (0x3780f92b)) ? (rb_state3.var_47) : (0xd5ea136));
        rb_state3.var_50 = (rb_state3.var_50) + (((((0x83fc9921) - (rb_state3.var_49)) ^ (rb_state3.var_48)) + (rb_state3.var_25)) + (rb_state3.var_26));
        rb_state3.var_26 = (rb_state3.var_26) ^ (rb_state3.var_50);
        rb_state3.var_1 = rb_state3.var_26;
        pthread_mutex_unlock(&(rb_state3.lock_51));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x16)) {
        rb_state4.var_21 = 0xa3883811;
        rb_state4.var_22 = 0xd5190c5a;
        rb_state4.var_23 = (rb_state4.var_23) + (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0xea425809));
        rb_state4.var_24 = (rb_state4.var_24) + ((0xf753bbb4) + (0x91d6b4e2));
        rb_state4.var_25 = (rb_state4.var_25) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_21) : (0xdaeff304));
        rb_state4.var_26 = (rb_state4.var_26) + (rb_state4.var_22);
        rb_state4.var_27 = (rb_state4.var_27) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_7) : (0x8fe3543d));
        rb_state4.var_28 = (rb_state4.var_28) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_23) : (0xc440d8ac));
        rb_state4.var_29 = (rb_state4.var_29) + (rb_state4.var_24);
        rb_state4.var_30 = (rb_state4.var_30) + (((0x52043170) - (0x66dac794)) - (rb_state4.var_25));
        rb_state4.var_31 = (rb_state4.var_31) - (((0x3305072) - (0x95908207)) ^ (rb_state4.var_26));
        rb_state4.var_32 = (rb_state4.var_32) - (rb_state4.var_27);
        rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_29) : (0xca64eaaf));
        rb_state4.var_34 = (rb_state4.var_34) - (((((0x736551fa) - (rb_state4.var_31)) + (rb_state4.var_13)) - (0xdb2eaf4c)) + (rb_state4.var_30));
        rb_state4.var_35 = (rb_state4.var_35) ^ (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_33) : (0xa1fdf077));
        rb_state4.var_36 = (rb_state4.var_36) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_34) : (0x7616bbdc));
        rb_state4.var_37 = (rb_state4.var_37) ^ (rb_state4.var_35);
        rb_state4.var_38 = (rb_state4.var_38) - (rb_state4.var_36);
        rb_state4.var_39 = (rb_state4.var_39) + (rb_state4.var_37);
        rb_state4.var_40 = (rb_state4.var_40) + (rb_state4.var_38);
        rb_state4.var_41 = (rb_state4.var_41) ^ (((rb_state4.var_39) == (0x892a7096)) ? (rb_state4.var_40) : (0xa639a123));
        rb_state4.var_20 = (rb_state4.var_20) + (rb_state4.var_41);
        rb_state4.var_1 = rb_state4.var_20;
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x1cfc4097)) {
        rb_state6.var_8 = 0x7a3752eb;
        rb_state6.var_9 = 0x42b3688c;
        rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_8);
        rb_state6.var_11 = (rb_state6.var_11) ^ (((((0x426f53b2) ^ (rb_state6.var_9)) - (rb_state6.var_3)) ^ (rb_state6.var_5)) ^ (rb_state6.var_4));
        rb_state6.var_12 = (rb_state6.var_12) - (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0x6e170149));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_14));
        rb_state8.var_3 = 0xd5b325ec;
        rb_state8.var_4 = 0x8eeb05f4;
        rb_state8.var_5 = (rb_state8.var_5) - (((((0x416cc849) - (rb_state8.var_4)) ^ (rb_state8.var_3)) ^ (rb_state8.var_2)) + (0xf929524d));
        rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_5) : (0x40f285));
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_6);
        rb_state8.var_1 = rb_state8.var_2;
        pthread_mutex_unlock(&(rb_state8.lock_14));
    }
    if ((rb_state8.var_0) == (0x0)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_30));
        rb_state11.var_3 = 0xc7d919c0;
        rb_state11.var_4 = (rb_state11.var_4) + (((0x54e4e788) - (0x71fe27f2)) + (rb_state11.var_3));
        rb_state11.var_5 = (rb_state11.var_5) - (rb_state11.var_3);
        rb_state11.var_6 = (rb_state11.var_6) ^ (137 < rb_input_size ? (uint32_t)rb_input[137] : 0x24a71db8);
        rb_state11.var_7 = (rb_state11.var_7) + (((rb_state11.var_4) == (0xaabfd956)) ? (rb_state11.var_1) : (0xeb4274e7));
        rb_state11.var_8 = (rb_state11.var_8) + (((0x7af9289f) ^ (0x7adcde7b)) + (rb_state11.var_2));
        rb_state11.var_9 = (rb_state11.var_9) ^ (((0x1db96876) + (rb_state11.var_4)) ^ (rb_state11.var_5));
        rb_state11.var_10 = (rb_state11.var_10) + (((rb_state11.var_5) == (0x3826e640)) ? (rb_state11.var_6) : (0xefe83992));
        rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x87cac77e));
        rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_6) == (0x2c)) ? (rb_state11.var_9) : (0x7ac03d1c));
        rb_state11.var_13 = (rb_state11.var_13) - (((0x1e94e1cf) ^ (rb_state11.var_7)) + (rb_state11.var_10));
        rb_state11.var_14 = (rb_state11.var_14) - (((rb_state11.var_8) == (0x25f6e4)) ? (rb_state11.var_11) : (0x103c483b));
        rb_state11.var_15 = (rb_state11.var_15) - (((rb_state11.var_9) == (0xf05fa78c)) ? (rb_state11.var_12) : (0xbe7388c2));
        rb_state11.var_16 = (rb_state11.var_16) + (((((0xb6e2a9d1) - (rb_state11.var_13)) ^ (0xae451e46)) - (0x3cc607f2)) - (rb_state11.var_14));
        rb_state11.var_17 = (rb_state11.var_17) + (rb_state11.var_15);
        rb_state11.var_18 = (rb_state11.var_18) + (rb_state11.var_16);
        rb_state11.var_19 = (rb_state11.var_19) + (((0x388e0cdf) + (rb_state11.var_17)) ^ (rb_state11.var_10));
        rb_state11.var_20 = (rb_state11.var_20) + (((0x24fb3122) - (rb_state11.var_11)) + (rb_state11.var_18));
        rb_state11.var_21 = (rb_state11.var_21) + (rb_state11.var_19);
        rb_state11.var_22 = (rb_state11.var_22) ^ (((0x9a08dda0) ^ (rb_state11.var_12)) + (rb_state11.var_20));
        rb_state11.var_23 = (rb_state11.var_23) ^ (rb_state11.var_21);
        rb_state11.var_24 = (rb_state11.var_24) ^ (((rb_state11.var_13) == (0xe16b1e05)) ? (rb_state11.var_22) : (0x7970a350));
        rb_state11.var_25 = (rb_state11.var_25) + (rb_state11.var_23);
        rb_state11.var_26 = (rb_state11.var_26) - (rb_state11.var_24);
        rb_state11.var_27 = (rb_state11.var_27) - (((((0xb4bbb2ef) ^ (rb_state11.var_26)) + (rb_state11.var_14)) ^ (rb_state11.var_15)) + (rb_state11.var_25));
        rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_27);
        rb_state11.var_1 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_30));
    }
    if ((rb_state11.var_0) == (0x0)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x3ccefe32));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        if ((rb_state14.var_1) != (0x0)) {
            if (!((rb_state14.var_1) == (rb_state14.var_26))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x8cdd7c4a));
    #endif
    pool->state = POOL_STATE_RUNNING;

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x16)) {
        pthread_mutex_lock(&(rb_state4.lock_58));
        rb_state4.var_43 = 0x42170af9;
        rb_state4.var_44 = 0xdef6d97d;
        rb_state4.var_45 = (rb_state4.var_45) + (((((0x467be4e7) ^ (rb_state4.var_16)) + (0x48f0d4fd)) - (rb_state4.var_44)) - (rb_state4.var_43));
        rb_state4.var_46 = (rb_state4.var_46) + (rb_state4.var_17);
        rb_state4.var_47 = (rb_state4.var_47) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_15) : (0x4523a83e));
        rb_state4.var_48 = (rb_state4.var_48) - (((((0x44add555) - (rb_state4.var_20)) ^ (rb_state4.var_45)) ^ (rb_state4.var_46)) - (rb_state4.var_19));
        rb_state4.var_49 = (rb_state4.var_49) - (rb_state4.var_47);
        rb_state4.var_50 = (rb_state4.var_50) ^ (((rb_state4.var_21) == (0xa3883811)) ? (rb_state4.var_48) : (0x21bed55c));
        rb_state4.var_51 = (rb_state4.var_51) - (((rb_state4.var_22) == (0xd5190c5a)) ? (rb_state4.var_49) : (0x12149bb6));
        rb_state4.var_52 = (rb_state4.var_52) - (((0x6957b1f0) ^ (rb_state4.var_50)) + (rb_state4.var_23));
        rb_state4.var_53 = (rb_state4.var_53) ^ (((0x8b6d70bb) - (rb_state4.var_51)) - (0x1527ac05));
        rb_state4.var_54 = (rb_state4.var_54) + (rb_state4.var_52);
        rb_state4.var_55 = (rb_state4.var_55) ^ (((0x991ded2a) - (rb_state4.var_24)) ^ (rb_state4.var_53));
        rb_state4.var_56 = (rb_state4.var_56) ^ (((rb_state4.var_25) == (0x5c77c7ef)) ? (rb_state4.var_54) : (0x927f7f8d));
        rb_state4.var_57 = (rb_state4.var_57) ^ (((rb_state4.var_55) == (0x79b6b822)) ? (rb_state4.var_56) : (0x1057ed5b));
        rb_state4.var_42 = (rb_state4.var_42) ^ (rb_state4.var_57);
        rb_state4.var_1 = rb_state4.var_42;
        pthread_mutex_unlock(&(rb_state4.lock_58));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (0x7c6198c1);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xfffffff8)) {
        rb_state10.var_3 = 0xfb89e97f;
        rb_state10.var_4 = (rb_state10.var_4) + (((0xf9e6176c) ^ (0x8be49196)) - (rb_state10.var_3));
        rb_state10.var_5 = (rb_state10.var_5) + (0x14d00772);
        rb_state10.var_6 = (rb_state10.var_6) ^ (rb_state10.var_1);
        rb_state10.var_7 = (rb_state10.var_7) ^ (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_4) : (0x83f249cd));
        rb_state10.var_8 = (rb_state10.var_8) ^ (((rb_state10.var_3) == (0xfb89e97f)) ? (rb_state10.var_5) : (0x3bf0eb7a));
        rb_state10.var_9 = (rb_state10.var_9) - (((0xb59f160a) ^ (rb_state10.var_6)) - (rb_state10.var_4));
        rb_state10.var_10 = (rb_state10.var_10) + (((0x3a7393a7) - (0xa0d98477)) - (rb_state10.var_7));
        rb_state10.var_11 = (rb_state10.var_11) - (((0xb9ace1df) + (rb_state10.var_8)) - (rb_state10.var_5));
        rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_9) : (0x9e7ba7a9));
        rb_state10.var_13 = (rb_state10.var_13) + (((rb_state10.var_7) == (0x76789d7b)) ? (rb_state10.var_10) : (0x7b6afff4));
        rb_state10.var_14 = (rb_state10.var_14) - (((((0x58078c2c) ^ (rb_state10.var_12)) + (rb_state10.var_8)) - (0x39214e76)) ^ (rb_state10.var_11));
        rb_state10.var_15 = (rb_state10.var_15) + (((rb_state10.var_13) == (0x232171b5)) ? (rb_state10.var_14) : (0xed8e16f6));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x2cc9895c);
    if ((rb_state13.var_0) == (0x2cc9895c)) {
        pthread_mutex_lock(&(rb_state13.lock_34));
        rb_state13.var_15 = 0x7d119325;
        rb_state13.var_16 = 0x14b46a28;
        rb_state13.var_17 = (rb_state13.var_17) - (1811 < rb_input_size ? (uint32_t)rb_input[1811] : 0x9ffa557e);
        rb_state13.var_18 = (rb_state13.var_18) - (((0xf1ca6a28) - (rb_state13.var_16)) + (rb_state13.var_9));
        rb_state13.var_19 = (rb_state13.var_19) + (((rb_state13.var_10) == (0x65d3694a)) ? (rb_state13.var_15) : (0xb97d8b52));
        rb_state13.var_20 = (rb_state13.var_20) - (((0x97a38c9a) + (rb_state13.var_8)) ^ (rb_state13.var_11));
        rb_state13.var_21 = (rb_state13.var_21) ^ (rb_state13.var_17);
        rb_state13.var_22 = (rb_state13.var_22) - (((0x1e733187) - (rb_state13.var_18)) ^ (rb_state13.var_12));
        rb_state13.var_23 = (rb_state13.var_23) + (((((0x64c804d4) + (rb_state13.var_13)) ^ (rb_state13.var_19)) + (rb_state13.var_14)) ^ (rb_state13.var_20));
        rb_state13.var_24 = (rb_state13.var_24) + (((rb_state13.var_15) == (0x7d119325)) ? (rb_state13.var_21) : (0xa615e74));
        rb_state13.var_25 = (rb_state13.var_25) ^ (rb_state13.var_22);
        rb_state13.var_26 = (rb_state13.var_26) ^ (((rb_state13.var_16) == (0x14b46a28)) ? (rb_state13.var_23) : (0x82b38b84));
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_17) == (0xffffff39)) ? (rb_state13.var_24) : (0xb8941e14));
        rb_state13.var_28 = (rb_state13.var_28) - (((0xdf2ad779) - (rb_state13.var_25)) + (0x78b8f620));
        rb_state13.var_29 = (rb_state13.var_29) + (((rb_state13.var_26) == (0x2dc87901)) ? (rb_state13.var_27) : (0x21995895));
        rb_state13.var_30 = (rb_state13.var_30) ^ (((rb_state13.var_18) == (0xbc425e7f)) ? (rb_state13.var_28) : (0xb21bab14));
        rb_state13.var_31 = (rb_state13.var_31) + (rb_state13.var_29);
        rb_state13.var_32 = (rb_state13.var_32) - (rb_state13.var_30);
        rb_state13.var_33 = (rb_state13.var_33) ^ (((((0x8cae552) ^ (rb_state13.var_32)) ^ (rb_state13.var_31)) ^ (0x2b384d28)) - (0x551f63ef));
        rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_33);
        pthread_mutex_unlock(&(rb_state13.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x5ffe9ab6)) {
        rb_state15.var_17 = !((rb_state15.var_2) == (rb_state15.var_1));
    }
    #endif
    return pool;
}

void tpool_destroy(tpool_t *pool) {
    assert(pool != NULL);
    assert(pool->state != POOL_STATE_RUNNING);

    free(pool->threads);
    free(pool);
}

int tpool_join(tpool_t *pool, void **value_ptrs) {
    int i;
    void **value_ptr;
    int rv;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_32) == (0x8208ab78)) {
        usleep(100);
    }
    if ((rb_state0.var_32) == (0x8208ab78)) {
        rb_state0.var_59 = !((rb_state0.var_52) == (rb_state0.var_51));
    }
    rb_state0.var_36 = (rb_state0.var_36) - (1092 < rb_input_size ? (uint32_t)rb_input[1092] : 0xb9296e3e);
    #endif
    assert(pool != NULL);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_37) == (0x822ade0)) {
        rb_state0.var_61 = (rb_state0.var_61) + (((rb_state0.var_39) == (0x8e16ef94)) ? (rb_state0.var_41) : (0x7c5fdf58));
        rb_state0.var_62 = (rb_state0.var_62) ^ (((rb_state0.var_42) == (0x2da94cdd)) ? (rb_state0.var_40) : (0x2223b4d));
        rb_state0.var_63 = (rb_state0.var_63) + (0x11c0a56a);
        rb_state0.var_64 = (rb_state0.var_64) - (((((0x211a1e13) ^ (rb_state0.var_61)) ^ (rb_state0.var_62)) - (0xedd91259)) - (0xf65f4d6c));
        rb_state0.var_65 = (rb_state0.var_65) - (((0xca540049) + (rb_state0.var_43)) + (rb_state0.var_63));
        rb_state0.var_66 = (rb_state0.var_66) - (((rb_state0.var_44) == (0x73c29cc)) ? (rb_state0.var_64) : (0x3662b4b));
        rb_state0.var_67 = (rb_state0.var_67) ^ (((rb_state0.var_45) == (0x11bbb653)) ? (rb_state0.var_65) : (0xbcf16606));
        rb_state0.var_68 = (rb_state0.var_68) - (rb_state0.var_66);
        rb_state0.var_69 = (rb_state0.var_69) + (((rb_state0.var_46) == (0xe420cf6e)) ? (rb_state0.var_67) : (0x5ddbeeeb));
        rb_state0.var_70 = (rb_state0.var_70) - (((((0x3570d5ee) - (0x1aa606d1)) - (rb_state0.var_68)) - (rb_state0.var_47)) + (rb_state0.var_69));
        rb_state0.var_38 = (rb_state0.var_38) + (rb_state0.var_70);
    }
    #endif
    assert(pool->state == POOL_STATE_RUNNING);

    for (i = 0; i < pool->nthreads; i++) {
        if (value_ptrs != NULL) {
            value_ptr = &(value_ptrs[i]);
        } else {
            value_ptr = NULL;
        }

        rv = pthread_join(pool->threads[i], value_ptr);
        if (rv != 0) {
            pool->state = POOL_STATE_ERROR;
            return -1;
        }
    }

    pool->state = POOL_STATE_READY;
    return 0;
}

int tpool_cancel(tpool_t *pool) {
    int i;
    int rv;

    assert(pool != NULL);
    assert(pool->state == POOL_STATE_RUNNING);

    rv = 0;
    for (i = 0; i < pool->nthreads; i++) {
        rv += pthread_cancel(pool->threads[i]);
    }

    if (rv != 0) {
        pool->state = POOL_STATE_ERROR;
        return -1;
    }

    pool->state = POOL_STATE_READY;
    return 0;
}