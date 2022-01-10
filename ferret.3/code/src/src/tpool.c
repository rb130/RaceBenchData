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
    rb_state1.var_0 = (rb_state1.var_0) + (0x422b3262);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x2d)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_5 = 0x76d183b;
        rb_state5.var_6 = (rb_state5.var_6) ^ (((((0x98080f34) - (0x438e655b)) ^ (rb_state5.var_5)) + (rb_state5.var_5)) ^ (rb_state5.var_3));
        rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0x4d80ca78));
        rb_state5.var_8 = (rb_state5.var_8) ^ (rb_state5.var_8);
        rb_state5.var_9 = (rb_state5.var_9) ^ (((0x46b6dfde) + (rb_state5.var_9)) - (rb_state5.var_4));
        rb_state5.var_10 = (rb_state5.var_10) + (((((0xcf3a3baf) - (rb_state5.var_7)) + (0xcf7fad6d)) ^ (rb_state5.var_6)) - (0xc3b69789));
        rb_state5.var_11 = (rb_state5.var_11) ^ (rb_state5.var_8);
        rb_state5.var_12 = (rb_state5.var_12) ^ (((((0xce6ccc99) - (rb_state5.var_10)) + (0xdef6f531)) - (rb_state5.var_9)) - (rb_state5.var_10));
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_11) : (0x7a3bc168));
        rb_state5.var_14 = (rb_state5.var_14) ^ (((0x83a0d19c) ^ (rb_state5.var_12)) + (rb_state5.var_12));
        rb_state5.var_15 = (rb_state5.var_15) + (rb_state5.var_13);
        rb_state5.var_16 = (rb_state5.var_16) - (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_14) : (0xa947e0f4));
        rb_state5.var_17 = (rb_state5.var_17) ^ (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_16) : (0x2695a87d));
        rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_17);
        rb_state5.var_3 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    if ((rb_state5.var_1) == (0x2d)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        if (!((rb_state5.var_3) == (rb_state5.var_4))) {
            racebench_trigger(5);
        }
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_2) : (0x672df094));
    if ((rb_state7.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_38));
        if ((rb_state7.var_3) != (0x0)) {
            if (!((rb_state7.var_3) == (rb_state7.var_26))) {
                racebench_trigger(7);
            }
        }
        pthread_mutex_unlock(&(rb_state7.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xfc16284e)) {
        rb_state11.var_6 = (rb_state11.var_6) ^ (0xe4c07b6);
        rb_state11.var_7 = (rb_state11.var_7) ^ (rb_state11.var_6);
        rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_7);
        rb_state11.var_9 = (rb_state11.var_9) ^ (((0x5b053eac) + (rb_state11.var_5)) ^ (0x2c3b4d6c));
        rb_state11.var_10 = (rb_state11.var_10) + (((0x6fa5136e) - (rb_state11.var_3)) ^ (0x691cda31));
        rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_4);
        rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_8) == (0xf1b3f84a)) ? (rb_state11.var_6) : (0xc370d998));
        rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_7) == (0xe4c07b6)) ? (rb_state11.var_8) : (0x7a6ee697));
        rb_state11.var_14 = (rb_state11.var_14) - (((0x23b89c91) + (rb_state11.var_9)) - (rb_state11.var_9));
        rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_10);
        rb_state11.var_16 = (rb_state11.var_16) + (((rb_state11.var_10) == (0x6b9c95f)) ? (rb_state11.var_11) : (0xdf614999));
        rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_12) : (0x2cdcdedd));
        rb_state11.var_18 = (rb_state11.var_18) - (((0x1ffebede) + (rb_state11.var_13)) ^ (rb_state11.var_12));
        rb_state11.var_19 = (rb_state11.var_19) - (((((0xb0e00f5c) - (0xa4e43bc3)) ^ (rb_state11.var_13)) - (rb_state11.var_14)) ^ (rb_state11.var_15));
        rb_state11.var_20 = (rb_state11.var_20) + (((0x2d57f09b) ^ (rb_state11.var_14)) - (rb_state11.var_16));
        rb_state11.var_21 = (rb_state11.var_21) ^ (rb_state11.var_17);
        rb_state11.var_22 = (rb_state11.var_22) ^ (((rb_state11.var_15) == (0x6b9c95f)) ? (rb_state11.var_18) : (0x80cef4fa));
        rb_state11.var_23 = (rb_state11.var_23) - (((rb_state11.var_19) == (0xe746fec5)) ? (rb_state11.var_20) : (0x6f32a2a7));
        rb_state11.var_24 = (rb_state11.var_24) ^ (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_21) : (0x8b6aec87));
        rb_state11.var_25 = (rb_state11.var_25) - (((((0xf248bda3) - (0xd7c9261b)) ^ (rb_state11.var_22)) + (0x8646beb4)) - (rb_state11.var_23));
        rb_state11.var_26 = (rb_state11.var_26) - (((rb_state11.var_24) == (0xe4c07b6)) ? (rb_state11.var_25) : (0x138a4642));
        rb_state11.var_5 = (rb_state11.var_5) + (rb_state11.var_26);
        rb_state11.var_2 = rb_state11.var_5;
    }
    if ((rb_state11.var_1) == (0xfc16284e)) {
        pthread_mutex_lock(&(rb_state11.lock_36));
        rb_state11.var_28 = (rb_state11.var_28) + (((((0xdf9d5e3f) + (rb_state11.var_17)) + (rb_state11.var_20)) - (rb_state11.var_21)) + (rb_state11.var_19));
        rb_state11.var_29 = (rb_state11.var_29) - (0xd84d9855);
        rb_state11.var_30 = (rb_state11.var_30) + (0xecda1de6);
        rb_state11.var_31 = (rb_state11.var_31) + (((0xe1c19137) ^ (0x693af59f)) + (rb_state11.var_18));
        rb_state11.var_32 = (rb_state11.var_32) ^ (((rb_state11.var_28) == (0xb7f4f0f8)) ? (rb_state11.var_29) : (0x29a86858));
        rb_state11.var_33 = (rb_state11.var_33) - (((rb_state11.var_30) == (0xecda1de6)) ? (rb_state11.var_31) : (0x55d9b77a));
        rb_state11.var_34 = (rb_state11.var_34) + (rb_state11.var_32);
        rb_state11.var_35 = (rb_state11.var_35) + (((rb_state11.var_33) == (0x97034bf6)) ? (rb_state11.var_34) : (0x5fe1b171));
        rb_state11.var_27 = (rb_state11.var_27) ^ (rb_state11.var_35);
        rb_state11.var_2 = rb_state11.var_27;
        pthread_mutex_unlock(&(rb_state11.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (812 < rb_input_size ? (uint32_t)rb_input[812] : 0x8dc0b54c);
    if ((rb_state15.var_0) == (0x48ebff19)) {
        usleep(100);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0xbe)) {
        rb_state19.var_4 = 0xfbdd0d3a;
        rb_state19.var_5 = 0x231c6f64;
        rb_state19.var_6 = (rb_state19.var_6) + (((rb_state19.var_4) == (0xfbdd0d3a)) ? (rb_state19.var_5) : (0xc12a3de5));
        rb_state19.var_7 = (rb_state19.var_7) - (0x154c734d);
        rb_state19.var_8 = (rb_state19.var_8) ^ (((rb_state19.var_3) == (0xbe)) ? (rb_state19.var_6) : (0xad34f40d));
        rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_4) == (0xfbdd0d3a)) ? (rb_state19.var_7) : (0x56a792f3));
        rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_5) == (0x231c6f64)) ? (rb_state19.var_8) : (0x90fb8c4c));
        rb_state19.var_11 = (rb_state19.var_11) - (rb_state19.var_9);
        rb_state19.var_12 = (rb_state19.var_12) ^ (((0x5a55546f) + (0xbffb4a3b)) ^ (rb_state19.var_10));
        rb_state19.var_13 = (rb_state19.var_13) + (((rb_state19.var_6) == (0x231c6f64)) ? (rb_state19.var_11) : (0x1054508b));
        rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_12) == (0xc6b30e36)) ? (rb_state19.var_13) : (0x4ea4f9ae));
        rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_14);
    }
    #endif
    pool = (tpool_t *)malloc(sizeof(tpool_t));
    if (pool == NULL) {
        return NULL;
    }
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (0x36ada835);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xde)) {
        rb_state12.var_6 = 0xd21908cd;
        rb_state12.var_7 = 0x37657cbf;
        rb_state12.var_8 = (rb_state12.var_8) - (rb_state12.var_6);
        rb_state12.var_9 = (rb_state12.var_9) - (((rb_state12.var_8) == (0x2de6f733)) ? (rb_state12.var_7) : (0x392e921c));
        rb_state12.var_10 = (rb_state12.var_10) ^ (((0x988f87fc) + (rb_state12.var_6)) ^ (0x52eec80c));
        rb_state12.var_11 = (rb_state12.var_11) - (((0x56e280b0) - (rb_state12.var_5)) ^ (rb_state12.var_9));
        rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_7) == (0x37657cbf)) ? (rb_state12.var_8) : (0x21f3e941));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((((0x40650b19) + (rb_state12.var_9)) ^ (rb_state12.var_10)) - (rb_state12.var_10)) - (0xa0bf77af));
        rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_11) == (0x6187fc0f)) ? (rb_state12.var_11) : (0xe911bc4c));
        rb_state12.var_15 = (rb_state12.var_15) ^ (rb_state12.var_12);
        rb_state12.var_16 = (rb_state12.var_16) - (((rb_state12.var_13) == (0x57b4062b)) ? (rb_state12.var_14) : (0x39440f7c));
        rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_15);
        rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_16) == (0x9e7803f1)) ? (rb_state12.var_17) : (0xa188660c));
        rb_state12.var_5 = (rb_state12.var_5) - (rb_state12.var_18);
        rb_state12.var_2 = rb_state12.var_5;
    }
    if ((rb_state12.var_0) == (0xde)) {
        pthread_mutex_lock(&(rb_state12.lock_36));
        rb_state12.var_20 = 0x60ca251c;
        rb_state12.var_21 = (rb_state12.var_21) ^ (((rb_state12.var_14) == (0x6187fc0f)) ? (rb_state12.var_20) : (0xb45f0755));
        rb_state12.var_22 = (rb_state12.var_22) + (0x85207d77);
        rb_state12.var_23 = (rb_state12.var_23) - (((0xeb632d2) - (0x24ae9942)) + (rb_state12.var_12));
        rb_state12.var_24 = (rb_state12.var_24) - (((rb_state12.var_15) == (0x2de6f733)) ? (rb_state12.var_13) : (0xab749455));
        rb_state12.var_25 = (rb_state12.var_25) + (rb_state12.var_21);
        rb_state12.var_26 = (rb_state12.var_26) ^ (rb_state12.var_22);
        rb_state12.var_27 = (rb_state12.var_27) + (((((0xbe0dd2) ^ (rb_state12.var_17)) ^ (rb_state12.var_16)) ^ (rb_state12.var_23)) ^ (rb_state12.var_24));
        rb_state12.var_28 = (rb_state12.var_28) + (((0x1a97c7fd) - (rb_state12.var_25)) - (0x4b859459));
        rb_state12.var_29 = (rb_state12.var_29) ^ (((((0x8f4a39d6) ^ (rb_state12.var_26)) + (rb_state12.var_18)) ^ (rb_state12.var_27)) ^ (0x381c2f62));
        rb_state12.var_30 = (rb_state12.var_30) - (((0xe5013271) - (rb_state12.var_28)) + (rb_state12.var_19));
        rb_state12.var_31 = (rb_state12.var_31) ^ (rb_state12.var_29);
        rb_state12.var_32 = (rb_state12.var_32) ^ (((rb_state12.var_20) == (0x60ca251c)) ? (rb_state12.var_30) : (0x236ffe57));
        rb_state12.var_33 = (rb_state12.var_33) + (((0x1f1b368) - (rb_state12.var_31)) ^ (rb_state12.var_21));
        rb_state12.var_34 = (rb_state12.var_34) ^ (((rb_state12.var_22) == (0x85207d77)) ? (rb_state12.var_32) : (0x7f2c9c5f));
        rb_state12.var_35 = (rb_state12.var_35) + (((((0xf06f58b2) ^ (rb_state12.var_33)) - (0xa4dc1d0a)) - (rb_state12.var_34)) ^ (0xb9eac37d));
        rb_state12.var_19 = (rb_state12.var_19) - (rb_state12.var_35);
        rb_state12.var_2 = rb_state12.var_19;
        pthread_mutex_unlock(&(rb_state12.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x48ebff19)) {
        rb_state15.var_17 = !((rb_state15.var_3) == (rb_state15.var_2));
    }
    #endif
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    for (i = 0; i < nthreads; i++) {
        if (opts[i].attr == NULL) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_1) : (0xaa8ff34e));
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_1) == (0x8d51c75f)) {
                rb_state8.var_15 = 0x7f155066;
                rb_state8.var_16 = 0xe5a799f8;
                rb_state8.var_17 = (rb_state8.var_17) - (((0xbcf84f27) - (0xd1bcbe2)) - (rb_state8.var_16));
                rb_state8.var_18 = (rb_state8.var_18) + (rb_state8.var_10);
                rb_state8.var_19 = (rb_state8.var_19) - (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_15) : (0x6856135b));
                rb_state8.var_20 = (rb_state8.var_20) + (((((0xa7d4ef10) + (rb_state8.var_9)) + (0xf970e95a)) + (0x81753bd9)) - (rb_state8.var_17));
                rb_state8.var_21 = (rb_state8.var_21) - (rb_state8.var_18);
                rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_20) : (0x47ce6a80));
                rb_state8.var_23 = (rb_state8.var_23) + (((0xaf539e72) ^ (rb_state8.var_21)) ^ (rb_state8.var_12));
                rb_state8.var_24 = (rb_state8.var_24) + (((((0xfeef306c) - (rb_state8.var_22)) - (rb_state8.var_13)) ^ (0xa7ff0f56)) + (rb_state8.var_23));
                rb_state8.var_14 = (rb_state8.var_14) - (rb_state8.var_24);
                rb_state8.var_3 = rb_state8.var_14;
            }
            if ((rb_state8.var_1) == (0x8ec5f105)) {
                pthread_mutex_lock(&(rb_state8.lock_41));
                rb_state8.var_26 = 0x287356f9;
                rb_state8.var_27 = (rb_state8.var_27) - (((0x7207a145) + (rb_state8.var_15)) - (0xaf27848d));
                rb_state8.var_28 = (rb_state8.var_28) ^ (0xb48baef8);
                rb_state8.var_29 = (rb_state8.var_29) + (((0xc7391a8b) ^ (rb_state8.var_26)) - (0x20300d39));
                rb_state8.var_30 = (rb_state8.var_30) + (((0xbce71350) - (rb_state8.var_16)) - (rb_state8.var_14));
                rb_state8.var_31 = (rb_state8.var_31) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_27) : (0xf63d715));
                rb_state8.var_32 = (rb_state8.var_32) + (rb_state8.var_28);
                rb_state8.var_33 = (rb_state8.var_33) - (((((0x23fa3dc7) ^ (rb_state8.var_18)) ^ (rb_state8.var_30)) + (rb_state8.var_29)) ^ (0x28c8c267));
                rb_state8.var_34 = (rb_state8.var_34) - (rb_state8.var_31);
                rb_state8.var_35 = (rb_state8.var_35) + (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_32) : (0x86e45fb4));
                rb_state8.var_36 = (rb_state8.var_36) - (rb_state8.var_33);
                rb_state8.var_37 = (rb_state8.var_37) ^ (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0x44fdec46));
                rb_state8.var_38 = (rb_state8.var_38) ^ (((0x3dd2e11f) - (rb_state8.var_36)) + (rb_state8.var_20));
                rb_state8.var_39 = (rb_state8.var_39) - (((0x6dbe8a98) ^ (rb_state8.var_37)) - (rb_state8.var_21));
                rb_state8.var_40 = (rb_state8.var_40) ^ (((((0x6a7327de) - (rb_state8.var_38)) ^ (rb_state8.var_22)) - (rb_state8.var_39)) + (0xc3dbbab));
                rb_state8.var_25 = (rb_state8.var_25) ^ (rb_state8.var_40);
                rb_state8.var_3 = rb_state8.var_25;
                pthread_mutex_unlock(&(rb_state8.lock_41));
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0x4529e19f)) {
                rb_state13.var_15 = 0xe2b20d9;
                rb_state13.var_16 = (rb_state13.var_16) ^ (((((0x9874718b) + (rb_state13.var_14)) - (rb_state13.var_11)) - (rb_state13.var_13)) + (rb_state13.var_12));
                rb_state13.var_17 = (rb_state13.var_17) - (((((0x175a0c5f) ^ (rb_state13.var_16)) - (rb_state13.var_15)) + (0xef9f0ea6)) - (0x33238ff3));
                rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_17);
                rb_state13.var_2 = rb_state13.var_14;
            }
            if ((rb_state13.var_1) == (0x4529e19f)) {
                pthread_mutex_lock(&(rb_state13.lock_23));
                rb_state13.var_19 = 0x517b0900;
                rb_state13.var_20 = 0x4017dfd9;
                rb_state13.var_21 = (rb_state13.var_21) + (((((0xd7640d61) + (0xf8f42d3b)) - (rb_state13.var_15)) + (rb_state13.var_19)) - (0x6696d1f0));
                rb_state13.var_22 = (rb_state13.var_22) - (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_21) : (0x3c789a56));
                rb_state13.var_18 = (rb_state13.var_18) ^ (rb_state13.var_22);
                rb_state13.var_2 = rb_state13.var_18;
                pthread_mutex_unlock(&(rb_state13.lock_23));
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0x6dbf84af)) {
                rb_state16.var_12 = 0x860ba7b6;
                rb_state16.var_13 = 0x48e4d6c4;
                rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_13) : (0x47ae71d7));
                rb_state16.var_15 = (rb_state16.var_15) ^ (rb_state16.var_12);
                rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_12) : (0x172e773a));
                rb_state16.var_17 = (rb_state16.var_17) - (rb_state16.var_14);
                rb_state16.var_18 = (rb_state16.var_18) ^ (((((0x8b408fb6) - (0x78701de6)) ^ (rb_state16.var_15)) ^ (0x630cbf45)) ^ (rb_state16.var_16));
                rb_state16.var_19 = (rb_state16.var_19) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_17) : (0x5c6a4bc4));
                rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_18) : (0xdb77d58d));
                rb_state16.var_21 = (rb_state16.var_21) - (((((0x4c6dcfde) + (rb_state16.var_20)) ^ (rb_state16.var_16)) - (0xcce41101)) - (rb_state16.var_19));
                rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_21);
                rb_state16.var_3 = rb_state16.var_11;
            }
            if ((rb_state16.var_1) == (0xb84d7799)) {
                pthread_mutex_lock(&(rb_state16.lock_33));
                rb_state16.var_23 = 0x5295d27b;
                rb_state16.var_24 = (rb_state16.var_24) - ((0x191ccb48) + (rb_state16.var_19));
                rb_state16.var_25 = (rb_state16.var_25) ^ (0xe572569);
                rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_17);
                rb_state16.var_27 = (rb_state16.var_27) ^ (rb_state16.var_18);
                rb_state16.var_28 = (rb_state16.var_28) - (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_24) : (0xc103420f));
                rb_state16.var_29 = (rb_state16.var_29) - (((0xf295c065) + (rb_state16.var_25)) - (0x2f04e956));
                rb_state16.var_30 = (rb_state16.var_30) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0xcf56f375));
                rb_state16.var_31 = (rb_state16.var_31) - (((((0xf5843c9e) ^ (0x258ccff4)) + (rb_state16.var_20)) + (rb_state16.var_28)) + (rb_state16.var_29));
                rb_state16.var_32 = (rb_state16.var_32) ^ (((rb_state16.var_30) == (0x0)) ? (rb_state16.var_31) : (0x81d3eba6));
                rb_state16.var_22 = (rb_state16.var_22) + (rb_state16.var_32);
                rb_state16.var_3 = rb_state16.var_22;
                pthread_mutex_unlock(&(rb_state16.lock_33));
            }
            #endif
            attr = NULL;
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_2);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + (0xa9e5edba);
            rb_state8.var_24 = (rb_state8.var_24) - (0x74e9355d);
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0x5ff9962d)) {
                rb_state9.var_15 = 0x4397941c;
                rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_15) : (0xc27c88f7));
                rb_state9.var_17 = (rb_state9.var_17) - (1436 < rb_input_size ? (uint32_t)rb_input[1436] : 0x69ddd1ab);
                rb_state9.var_18 = (rb_state9.var_18) ^ (0xa3dc09c6);
                rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_8) : (0x9aef6eeb));
                rb_state9.var_20 = (rb_state9.var_20) ^ (((((0xa4cd837d) - (0x863473f2)) + (rb_state9.var_11)) ^ (rb_state9.var_16)) ^ (rb_state9.var_17));
                rb_state9.var_21 = (rb_state9.var_21) + (((0x1371a976) ^ (rb_state9.var_18)) ^ (rb_state9.var_12));
                rb_state9.var_22 = (rb_state9.var_22) + (((((0x258da5ec) ^ (rb_state9.var_13)) - (rb_state9.var_20)) + (0xbe882c3)) + (rb_state9.var_19));
                rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_21) == (0xb0ada0b0)) ? (rb_state9.var_22) : (0xebf0331f));
                rb_state9.var_14 = (rb_state9.var_14) - (rb_state9.var_23);
                rb_state9.var_3 = rb_state9.var_14;
            }
            if ((rb_state9.var_0) == (0x5ff9962d)) {
                pthread_mutex_lock(&(rb_state9.lock_29));
                rb_state9.var_25 = 0x11bde36d;
                rb_state9.var_26 = (rb_state9.var_26) - (rb_state9.var_15);
                rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_25) == (0x11bde36d)) ? (rb_state9.var_14) : (0xd42a2e9a));
                rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_26) == (0xbc686be4)) ? (rb_state9.var_27) : (0x7383fea8));
                rb_state9.var_24 = (rb_state9.var_24) + (rb_state9.var_28);
                rb_state9.var_3 = rb_state9.var_24;
                pthread_mutex_unlock(&(rb_state9.lock_29));
            }
            #endif
            arg = NULL;
        } else {
            arg = opts[i].arg;
        }

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) - ((0xdc22a405) ^ (0x92500557));
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_26) == (0x287356f9)) {
            rb_state8.var_44 = 0xf92984b3;
            rb_state8.var_45 = (rb_state8.var_45) + (((rb_state8.var_30) == (0x0)) ? (rb_state8.var_29) : (0xb30da3e9));
            rb_state8.var_46 = (rb_state8.var_46) ^ (((rb_state8.var_31) == (0x0)) ? (rb_state8.var_44) : (0x65aca534));
            rb_state8.var_47 = (rb_state8.var_47) - (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_45) : (0x709df01e));
            rb_state8.var_48 = (rb_state8.var_48) - (rb_state8.var_46);
            rb_state8.var_49 = (rb_state8.var_49) + (rb_state8.var_47);
            rb_state8.var_50 = (rb_state8.var_50) ^ (((rb_state8.var_33) == (0x0)) ? (rb_state8.var_48) : (0x22b0e71));
            rb_state8.var_51 = (rb_state8.var_51) - (((((0x2ba4317b) ^ (rb_state8.var_50)) + (0xd159d83)) ^ (rb_state8.var_49)) ^ (rb_state8.var_34));
            rb_state8.var_28 = (rb_state8.var_28) + (rb_state8.var_51);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_11) == (0xa4c1be04)) {
            if ((rb_state15.var_18) != (0x0)) {
                if (!((rb_state15.var_18) == (rb_state15.var_45))) {
                    racebench_trigger(15);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_1);
        #endif
        rv = pthread_create(&(pool->threads[i]), attr, opts[i].start_routine, arg);
        if (rv != 0) {
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x8410f627)) {
        rb_state1.var_4 = 0x64022322;
        rb_state1.var_5 = (rb_state1.var_5) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0xf89bae4b));
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (0xdc9688a5);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x7b405e41)) {
        pthread_mutex_lock(&(rb_state10.lock_31));
        rb_state10.var_15 = (rb_state10.var_15) - (1542 < rb_input_size ? (uint32_t)rb_input[1542] : 0xa5bb96fe);
        rb_state10.var_16 = (rb_state10.var_16) - (rb_state10.var_11);
        rb_state10.var_17 = (rb_state10.var_17) + (((((0xd598d954) + (rb_state10.var_12)) - (rb_state10.var_10)) ^ (0x518e5774)) ^ (rb_state10.var_9));
        rb_state10.var_18 = (rb_state10.var_18) + (rb_state10.var_15);
        rb_state10.var_19 = (rb_state10.var_19) - (((0xd32d088) ^ (rb_state10.var_13)) + (rb_state10.var_16));
        rb_state10.var_20 = (rb_state10.var_20) ^ (((0xf71d87fc) + (rb_state10.var_17)) + (0x6778b7da));
        rb_state10.var_21 = (rb_state10.var_21) + (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_18) : (0xa64c3fae));
        rb_state10.var_22 = (rb_state10.var_22) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_19) : (0x2a0458));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((((0xef2d6f39) + (rb_state10.var_16)) ^ (rb_state10.var_20)) - (rb_state10.var_21)) - (0x72dc0eef));
        rb_state10.var_24 = (rb_state10.var_24) + (rb_state10.var_22);
        rb_state10.var_25 = (rb_state10.var_25) + (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_23) : (0x3443aaa8));
        rb_state10.var_26 = (rb_state10.var_26) ^ (((0x6061b9a7) ^ (rb_state10.var_24)) - (rb_state10.var_18));
        rb_state10.var_27 = (rb_state10.var_27) + (rb_state10.var_25);
        rb_state10.var_28 = (rb_state10.var_28) + (rb_state10.var_26);
        rb_state10.var_29 = (rb_state10.var_29) + (rb_state10.var_27);
        rb_state10.var_30 = (rb_state10.var_30) ^ (((((0x5826eca0) - (rb_state10.var_29)) - (rb_state10.var_19)) - (0x31e25e3a)) ^ (rb_state10.var_28));
        rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_30);
        pthread_mutex_unlock(&(rb_state10.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x50773917));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (0xc7f27cfb);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (378 < rb_input_size ? (uint32_t)rb_input[378] : 0x905c0148);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x5242fb02) ^ (rb_state17.var_0));
    if ((rb_state17.var_2) == (0xb18b232)) {
        pthread_mutex_lock(&(rb_state17.lock_18));
        rb_state17.var_5 = 0x23434c75;
        rb_state17.var_6 = (rb_state17.var_6) ^ (rb_state17.var_4);
        rb_state17.var_7 = (rb_state17.var_7) ^ (271 < rb_input_size ? (uint32_t)rb_input[271] : 0x47e4f85a);
        rb_state17.var_8 = (rb_state17.var_8) + (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_5) : (0xdfef46d6));
        rb_state17.var_9 = (rb_state17.var_9) + (((((0xa1eb96f8) ^ (rb_state17.var_7)) ^ (0x15d1dea8)) + (rb_state17.var_6)) + (rb_state17.var_5));
        rb_state17.var_10 = (rb_state17.var_10) - (((((0xb746715a) - (rb_state17.var_8)) - (rb_state17.var_6)) ^ (rb_state17.var_9)) + (0xa5610609));
        rb_state17.var_4 = (rb_state17.var_4) - (rb_state17.var_10);
        rb_state17.var_3 = rb_state17.var_4;
        pthread_mutex_unlock(&(rb_state17.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (1271 < rb_input_size ? (uint32_t)rb_input[1271] : 0x95023eb2);
    #endif
    pool->nthreads = nthreads;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xd005243a)) {
        rb_state2.var_11 = 0x2f915999;
        rb_state2.var_12 = 0x665fffb4;
        rb_state2.var_13 = (rb_state2.var_13) ^ ((0xe46183fd) + (0xf1c1f550));
        rb_state2.var_14 = (rb_state2.var_14) + (rb_state2.var_12);
        rb_state2.var_15 = (rb_state2.var_15) - (((0x5fb9b9b9) + (rb_state2.var_11)) ^ (rb_state2.var_4));
        rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_13) : (0x7e33a6f3));
        rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_14);
        rb_state2.var_18 = (rb_state2.var_18) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_15) : (0x5c2d0161));
        rb_state2.var_19 = (rb_state2.var_19) ^ (rb_state2.var_16);
        rb_state2.var_20 = (rb_state2.var_20) + (((0xa4e03c62) + (rb_state2.var_6)) - (rb_state2.var_17));
        rb_state2.var_21 = (rb_state2.var_21) + (rb_state2.var_18);
        rb_state2.var_22 = (rb_state2.var_22) - (((0x389b3ca1) ^ (0x18de182)) - (rb_state2.var_19));
        rb_state2.var_23 = (rb_state2.var_23) ^ (((rb_state2.var_20) == (0x0)) ? (rb_state2.var_21) : (0x172c3fd8));
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_22) : (0x39f63062));
        rb_state2.var_25 = (rb_state2.var_25) - (((0x1e53dd59) ^ (0xe0ef1b75)) ^ (rb_state2.var_23));
        rb_state2.var_26 = (rb_state2.var_26) + (((0x556314c) - (rb_state2.var_24)) ^ (rb_state2.var_8));
        rb_state2.var_27 = (rb_state2.var_27) + (rb_state2.var_25);
        rb_state2.var_28 = (rb_state2.var_28) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_26) : (0xf41c4591));
        rb_state2.var_29 = (rb_state2.var_29) - (rb_state2.var_27);
        rb_state2.var_30 = (rb_state2.var_30) ^ (((0x6871fd2a) ^ (rb_state2.var_28)) - (0xde95289f));
        rb_state2.var_31 = (rb_state2.var_31) + (((((0x6047c5b8) ^ (0x360f640f)) ^ (rb_state2.var_30)) + (0x59c0c922)) - (rb_state2.var_29));
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0x9e047c12) ^ (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0x3615e208)) {
        pthread_mutex_lock(&(rb_state6.lock_30));
        rb_state6.var_5 = 0x879b9dcd;
        rb_state6.var_6 = 0x998b7f60;
        rb_state6.var_7 = (rb_state6.var_7) + (0x3ca5c00f);
        rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_5);
        rb_state6.var_9 = (rb_state6.var_9) - (((((0xd3243f80) ^ (rb_state6.var_4)) - (0x4ac593e1)) + (rb_state6.var_6)) - (rb_state6.var_3));
        rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_7) : (0xc620b54e));
        rb_state6.var_11 = (rb_state6.var_11) + (rb_state6.var_8);
        rb_state6.var_12 = (rb_state6.var_12) + (rb_state6.var_9);
        rb_state6.var_13 = (rb_state6.var_13) - (((((0xa1ddb269) + (rb_state6.var_6)) ^ (rb_state6.var_11)) ^ (rb_state6.var_7)) ^ (rb_state6.var_10));
        rb_state6.var_14 = (rb_state6.var_14) ^ (((0xe7947a3) + (rb_state6.var_12)) ^ (rb_state6.var_8));
        rb_state6.var_15 = (rb_state6.var_15) ^ (((((0x7088bba1) + (rb_state6.var_13)) - (0x88ca79dc)) + (rb_state6.var_9)) + (rb_state6.var_14));
        rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_15);
        rb_state6.var_3 = rb_state6.var_4;
        pthread_mutex_unlock(&(rb_state6.lock_30));
    }
    if ((rb_state6.var_2) == (0xf167574)) {
        if (!((rb_state6.var_3) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xc4339e47);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (1835 < rb_input_size ? (uint32_t)rb_input[1835] : 0xfdcd9a24);
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x35e7b93d));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x151f8207) ^ (0x45da4673));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x23849a);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0xbd4780e3) ^ (0xbc8c366a));
    #endif
    pool->state = POOL_STATE_RUNNING;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x8187bf91));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0xef)) {
        if ((rb_state3.var_3) != (0x0)) {
            if (!((rb_state3.var_3) == (rb_state3.var_23))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xd0c8b8e7)) {
        rb_state4.var_3 = rb_state4.var_1;
    }
    if ((rb_state4.var_0) == (0xd0c8b8e7)) {
        if (!((rb_state4.var_1) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (584 < rb_input_size ? (uint32_t)rb_input[584] : 0x415f4f4c);
    rb_state5.var_2 = (rb_state5.var_2) + ((0x79217ed4) - (rb_state5.var_1));
    rb_state5.var_2 = (rb_state5.var_2) ^ (1085 < rb_input_size ? (uint32_t)rb_input[1085] : 0xb721fed7);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x5885446));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_36 = (rb_state8.var_36) - (0x52978bff);
    if ((rb_state8.var_37) == (0x44fdec46)) {
        pthread_mutex_lock(&(rb_state8.lock_68));
        if ((rb_state8.var_56) != (0x0)) {
            rb_state8.var_69 = !((rb_state8.var_54) == (rb_state8.var_55));
        }
        pthread_mutex_unlock(&(rb_state8.lock_68));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (0x7c9d7e92);
    rb_state11.var_0 = (rb_state11.var_0) - (0xf8d64c94);
    rb_state11.var_1 = (rb_state11.var_1) - ((0xf16bd354) - (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (0x82105b5);
    rb_state12.var_1 = (rb_state12.var_1) + (0x23f0156d);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_2) == (0xb18b232)) {
        if (!((rb_state17.var_3) == (rb_state17.var_4))) {
            racebench_trigger(17);
        }
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

    assert(pool != NULL);
    assert(pool->state == POOL_STATE_RUNNING);

    for (i = 0; i < pool->nthreads; i++) {
        if (value_ptrs != NULL) {
            value_ptr = &(value_ptrs[i]);
        } else {
            value_ptr = NULL;
        }

        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_46) == (0x65aca534)) {
            rb_state8.var_82 = 0xcac45075;
            rb_state8.var_83 = 0xea34347e;
            rb_state8.var_84 = (rb_state8.var_84) ^ (((((0xcb164639) ^ (rb_state8.var_54)) ^ (rb_state8.var_53)) - (rb_state8.var_55)) ^ (rb_state8.var_83));
            rb_state8.var_85 = (rb_state8.var_85) + (((0xa0d53360) ^ (0xacfb3eab)) ^ (rb_state8.var_82));
            rb_state8.var_86 = (rb_state8.var_86) - (((rb_state8.var_56) == (0x1)) ? (rb_state8.var_84) : (0xafb2fd83));
            rb_state8.var_87 = (rb_state8.var_87) + (((0x62c73970) ^ (0xffcd54f6)) ^ (rb_state8.var_85));
            rb_state8.var_88 = (rb_state8.var_88) - (((((0xa8387a64) - (0x523470ad)) + (rb_state8.var_86)) + (rb_state8.var_87)) + (0xf4252e87));
            rb_state8.var_81 = (rb_state8.var_81) + (rb_state8.var_88);
            rb_state8.var_70 = rb_state8.var_81;
        }
        if ((rb_state8.var_46) == (0x65aca534)) {
            pthread_mutex_lock(&(rb_state8.lock_103));
            rb_state8.var_90 = 0xb45570bf;
            rb_state8.var_91 = (rb_state8.var_91) + (((rb_state8.var_59) == (0x0)) ? (rb_state8.var_58) : (0x653f9f0));
            rb_state8.var_92 = (rb_state8.var_92) ^ (((0xded5a27f) + (rb_state8.var_57)) - (0xff645f54));
            rb_state8.var_93 = (rb_state8.var_93) - (0x4391139f);
            rb_state8.var_94 = (rb_state8.var_94) ^ (((0xdff9d1f4) ^ (0x3505018d)) ^ (rb_state8.var_90));
            rb_state8.var_95 = (rb_state8.var_95) + (((((0x6b1cce70) - (rb_state8.var_60)) + (rb_state8.var_92)) - (rb_state8.var_91)) ^ (0xc3ddd662));
            rb_state8.var_96 = (rb_state8.var_96) + (((((0xcb5bf603) - (0xcb7d1d71)) ^ (rb_state8.var_61)) + (rb_state8.var_94)) + (rb_state8.var_93));
            rb_state8.var_97 = (rb_state8.var_97) + (((0xc4426d90) ^ (rb_state8.var_95)) - (0x893178a9));
            rb_state8.var_98 = (rb_state8.var_98) ^ (rb_state8.var_96);
            rb_state8.var_99 = (rb_state8.var_99) ^ (((rb_state8.var_62) == (0xe860a2c4)) ? (rb_state8.var_97) : (0x3f5f876f));
            rb_state8.var_100 = (rb_state8.var_100) - (((0xe8e70bca) ^ (rb_state8.var_98)) ^ (0x7f54defb));
            rb_state8.var_101 = (rb_state8.var_101) - (((0x49edcdcd) + (rb_state8.var_99)) ^ (rb_state8.var_63));
            rb_state8.var_102 = (rb_state8.var_102) ^ (((rb_state8.var_100) == (0x72bb4f78)) ? (rb_state8.var_101) : (0x73cee0a6));
            rb_state8.var_89 = (rb_state8.var_89) - (rb_state8.var_102);
            rb_state8.var_70 = rb_state8.var_89;
            pthread_mutex_unlock(&(rb_state8.lock_103));
        }
        #endif
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