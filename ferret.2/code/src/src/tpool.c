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

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_3 = 0x6ae7b9a9;
        rb_state6.var_4 = (rb_state6.var_4) - ((0xe05ee5a5) + (rb_state6.var_4));
        rb_state6.var_5 = (rb_state6.var_5) + (((rb_state6.var_3) == (0x6ae7b9a9)) ? (rb_state6.var_2) : (0xd0e0aba6));
        rb_state6.var_6 = (rb_state6.var_6) ^ (0x4a125b32);
        rb_state6.var_7 = (rb_state6.var_7) - (rb_state6.var_4);
        rb_state6.var_8 = (rb_state6.var_8) - (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_5) : (0xffabc645));
        rb_state6.var_9 = (rb_state6.var_9) - (((0x89f78aed) ^ (rb_state6.var_6)) - (rb_state6.var_6));
        rb_state6.var_10 = (rb_state6.var_10) ^ (rb_state6.var_7);
        rb_state6.var_11 = (rb_state6.var_11) - (((0xb18d19e7) ^ (rb_state6.var_7)) - (rb_state6.var_8));
        rb_state6.var_12 = (rb_state6.var_12) + (((0xb14a5bb9) ^ (rb_state6.var_8)) ^ (rb_state6.var_9));
        rb_state6.var_13 = (rb_state6.var_13) - (((0xdc527d87) ^ (0xff8de554)) ^ (rb_state6.var_10));
        rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_9) == (0x862c8953)) ? (rb_state6.var_11) : (0xbdcfa90e));
        rb_state6.var_15 = (rb_state6.var_15) - (((((0xf20e70ca) - (rb_state6.var_13)) ^ (rb_state6.var_10)) - (rb_state6.var_11)) ^ (rb_state6.var_12));
        rb_state6.var_16 = (rb_state6.var_16) + (rb_state6.var_14);
        rb_state6.var_17 = (rb_state6.var_17) + (rb_state6.var_15);
        rb_state6.var_18 = (rb_state6.var_18) - (((rb_state6.var_16) == (0xae2c03be)) ? (rb_state6.var_17) : (0xa7063752));
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0xf2617008)) {
        pthread_mutex_lock(&(rb_state19.lock_12));
        rb_state19.var_5 = 0x2e2d18e7;
        rb_state19.var_6 = (rb_state19.var_6) + ((0xd6dccc00) - (0xf0bd2014));
        rb_state19.var_7 = (rb_state19.var_7) ^ (((((0x486aac7e) ^ (rb_state19.var_5)) - (0xfd2f3a7e)) ^ (rb_state19.var_4)) - (rb_state19.var_6));
        rb_state19.var_8 = (rb_state19.var_8) ^ (rb_state19.var_7);
        rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_6) == (0xe61fabec)) ? (rb_state19.var_7) : (0x7dcaa278));
        rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_8) == (0x82f8ce2f)) ? (rb_state19.var_8) : (0xfeda1e02));
        rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_9) == (0x7d0731d1)) ? (rb_state19.var_10) : (0xacb05e83));
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_11);
        pthread_mutex_unlock(&(rb_state19.lock_12));
    }
    #endif
    pool = (tpool_t *)malloc(sizeof(tpool_t));
    if (pool == NULL) {
        return NULL;
    }
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xa9972f43)) {
        pthread_mutex_lock(&(rb_state2.lock_14));
        rb_state2.var_2 = rb_state2.var_1;
        pthread_mutex_unlock(&(rb_state2.lock_14));
    }
    if ((rb_state2.var_0) == (0xa9972f43)) {
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_14 = (rb_state4.var_14) ^ (0xb9d0ba54);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_0);
    #endif
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    for (i = 0; i < nthreads; i++) {
        if (opts[i].attr == NULL) {
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_1) == (0xffffffee)) {
                pthread_mutex_lock(&(rb_state3.lock_15));
                rb_state3.var_4 = (rb_state3.var_4) ^ (((0x6d114fc8) - (0xc055dca0)) ^ (rb_state3.var_3));
                rb_state3.var_5 = (rb_state3.var_5) + (((0x222345be) + (rb_state3.var_4)) + (rb_state3.var_2));
                rb_state3.var_6 = (rb_state3.var_6) ^ (((((0x946979cd) + (rb_state3.var_5)) ^ (rb_state3.var_4)) - (rb_state3.var_5)) ^ (0x175fb255));
                rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_6);
                rb_state3.var_2 = rb_state3.var_3;
                pthread_mutex_unlock(&(rb_state3.lock_15));
            }
            if ((rb_state3.var_1) == (0xffffffee)) {
                pthread_mutex_lock(&(rb_state3.lock_15));
                if (!((rb_state3.var_2) == (rb_state3.var_3))) {
                    racebench_trigger(3);
                }
                pthread_mutex_unlock(&(rb_state3.lock_15));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_1) == (0x0)) {
                if ((rb_state14.var_2) != (0x0)) {
                    if (!((rb_state14.var_2) == (rb_state14.var_15))) {
                        racebench_trigger(14);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x17e06976));
            #endif
            attr = NULL;
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_15) == (0x887808ea)) {
                pthread_mutex_lock(&(rb_state4.lock_48));
                rb_state4.var_28 = 0x16807bf2;
                rb_state4.var_29 = 0x2bc802d0;
                rb_state4.var_30 = (rb_state4.var_30) - ((0xbb2e4b07) + (0x9906a3d8));
                rb_state4.var_31 = (rb_state4.var_31) + (rb_state4.var_17);
                rb_state4.var_32 = (rb_state4.var_32) ^ (rb_state4.var_28);
                rb_state4.var_33 = (rb_state4.var_33) - (((((0x2bc4b2b5) ^ (rb_state4.var_19)) - (rb_state4.var_18)) + (rb_state4.var_29)) - (rb_state4.var_16));
                rb_state4.var_34 = (rb_state4.var_34) + (rb_state4.var_30);
                rb_state4.var_35 = (rb_state4.var_35) + (((0xc58ba9bc) ^ (rb_state4.var_20)) + (rb_state4.var_31));
                rb_state4.var_36 = (rb_state4.var_36) + (((((0xec8c75cc) - (rb_state4.var_33)) + (rb_state4.var_21)) ^ (0x9b39af12)) + (rb_state4.var_32));
                rb_state4.var_37 = (rb_state4.var_37) - (rb_state4.var_34);
                rb_state4.var_38 = (rb_state4.var_38) + (((0xc8b574ab) ^ (0x7610c236)) + (rb_state4.var_35));
                rb_state4.var_39 = (rb_state4.var_39) ^ (((rb_state4.var_36) == (0x0)) ? (rb_state4.var_37) : (0x579f02c5));
                rb_state4.var_40 = (rb_state4.var_40) ^ (((rb_state4.var_22) == (0x0)) ? (rb_state4.var_38) : (0x2b340eb8));
                rb_state4.var_41 = (rb_state4.var_41) + (rb_state4.var_39);
                rb_state4.var_42 = (rb_state4.var_42) - (((0x51b72ab9) ^ (rb_state4.var_23)) ^ (rb_state4.var_40));
                rb_state4.var_43 = (rb_state4.var_43) + (((rb_state4.var_41) == (0x0)) ? (rb_state4.var_42) : (0x9ffa22e9));
                rb_state4.var_27 = (rb_state4.var_27) ^ (rb_state4.var_43);
                rb_state4.var_26 = rb_state4.var_27;
                pthread_mutex_unlock(&(rb_state4.lock_48));
            }
            if ((rb_state4.var_15) == (0x887808ea)) {
                if (!((rb_state4.var_26) == (rb_state4.var_27))) {
                    racebench_trigger(4);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_0);
            if ((rb_state8.var_1) == (0xfaced07b)) {
                rb_state8.var_2 = rb_state8.var_0;
            }
            if ((rb_state8.var_1) == (0xfaced07b)) {
                rb_state8.var_8 = !((rb_state8.var_0) == (rb_state8.var_2));
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (0xb8741c80);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) ^ (0x9c1625b);
            #endif
            arg = NULL;
        } else {
            arg = opts[i].arg;
        }

        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x0)) {
            rb_state0.var_3 = 0x8bf5e489;
            rb_state0.var_4 = 0x8605e840;
            rb_state0.var_5 = (rb_state0.var_5) ^ (((rb_state0.var_3) == (0x8bf5e489)) ? (rb_state0.var_2) : (0x8c2d2c60));
            rb_state0.var_6 = (rb_state0.var_6) ^ (((((0x6fb0f990) + (rb_state0.var_5)) + (0xf63811d2)) + (0x25edf0e)) ^ (rb_state0.var_4));
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_6);
        }
        #endif
        rv = pthread_create(&(pool->threads[i]), attr, opts[i].start_routine, arg);
        if (rv != 0) {
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
    if ((rb_state0.var_3) == (0x8bf5e489)) {
        pthread_mutex_lock(&(rb_state0.lock_29));
        if ((rb_state0.var_9) != (0x0)) {
            if (!((rb_state0.var_9) == (rb_state0.var_23))) {
                racebench_trigger(0);
            }
        }
        pthread_mutex_unlock(&(rb_state0.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x8a54e613)) {
        rb_state4.var_8 = 0x17f8ed07;
        rb_state4.var_9 = (rb_state4.var_9) - (((0x6b7f0735) ^ (rb_state4.var_3)) - (0x2aa5e28d));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((0x884953aa) ^ (rb_state4.var_4)) + (0xecd7a9d3));
        rb_state4.var_11 = (rb_state4.var_11) + (rb_state4.var_8);
        rb_state4.var_12 = (rb_state4.var_12) + (((((0xb83196f6) - (rb_state4.var_9)) + (rb_state4.var_10)) ^ (rb_state4.var_5)) ^ (0xea7d5e35));
        rb_state4.var_13 = (rb_state4.var_13) - (((0x58ebfa70) - (0x2131c7f1)) - (rb_state4.var_11));
        rb_state4.var_14 = (rb_state4.var_14) - (rb_state4.var_12);
        rb_state4.var_15 = (rb_state4.var_15) + (((0x6846b262) - (rb_state4.var_6)) ^ (rb_state4.var_13));
        rb_state4.var_16 = (rb_state4.var_16) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_15) : (0xebeb6481));
        rb_state4.var_7 = (rb_state4.var_7) + (rb_state4.var_16);
        rb_state4.var_2 = rb_state4.var_7;
    }
    if ((rb_state4.var_0) == (0x8a54e613)) {
        pthread_mutex_lock(&(rb_state4.lock_24));
        rb_state4.var_18 = 0xfbb0448c;
        rb_state4.var_19 = 0x3c7bf8ef;
        rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_19) : (0x550e1785));
        rb_state4.var_21 = (rb_state4.var_21) - (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_7) : (0x54e17ba1));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((0x2ea09357) + (rb_state4.var_9)) + (rb_state4.var_20));
        rb_state4.var_23 = (rb_state4.var_23) - (((((0x385a0251) + (0x3d0e2ad9)) - (rb_state4.var_22)) ^ (rb_state4.var_21)) - (rb_state4.var_10));
        rb_state4.var_17 = (rb_state4.var_17) - (rb_state4.var_23);
        rb_state4.var_2 = rb_state4.var_17;
        pthread_mutex_unlock(&(rb_state4.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_1) : (0x2915b8f));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x182d4f6e)) {
        rb_state7.var_16 = 0x1f814a44;
        rb_state7.var_17 = 0x3092e9b7;
        rb_state7.var_18 = (rb_state7.var_18) + (((((0x5c462f7e) ^ (rb_state7.var_16)) ^ (rb_state7.var_17)) - (rb_state7.var_11)) - (0xbf724067));
        rb_state7.var_19 = (rb_state7.var_19) ^ (((0x20604399) + (rb_state7.var_10)) ^ (rb_state7.var_12));
        rb_state7.var_20 = (rb_state7.var_20) + (rb_state7.var_18);
        rb_state7.var_21 = (rb_state7.var_21) + (((0x74899bd1) ^ (0xabac6eef)) ^ (rb_state7.var_19));
        rb_state7.var_22 = (rb_state7.var_22) - (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_20) : (0x624666c));
        rb_state7.var_23 = (rb_state7.var_23) + (rb_state7.var_21);
        rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_22) : (0x3184eb80));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((((0x92b2d41) ^ (rb_state7.var_23)) + (0xe0e1e60b)) ^ (rb_state7.var_24)) - (0x55551db));
        rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_25);
        rb_state7.var_2 = rb_state7.var_15;
    }
    if ((rb_state7.var_1) == (0x182d4f6e)) {
        pthread_mutex_lock(&(rb_state7.lock_37));
        rb_state7.var_27 = 0xbb232ea;
        rb_state7.var_28 = 0xd0868c57;
        rb_state7.var_29 = (rb_state7.var_29) ^ (0x5de0245e);
        rb_state7.var_30 = (rb_state7.var_30) - (((((0x87d36447) - (rb_state7.var_28)) - (rb_state7.var_15)) ^ (rb_state7.var_16)) ^ (rb_state7.var_17));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((0xca368c0c) ^ (0x65e9cdc9)) ^ (rb_state7.var_27));
        rb_state7.var_32 = (rb_state7.var_32) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_29) : (0xc72228a3));
        rb_state7.var_33 = (rb_state7.var_33) ^ (((0x1fa53107) - (0x38385e57)) + (rb_state7.var_30));
        rb_state7.var_34 = (rb_state7.var_34) + (((rb_state7.var_31) == (0x0)) ? (rb_state7.var_32) : (0xf4ad2dd7));
        rb_state7.var_35 = (rb_state7.var_35) - (((rb_state7.var_19) == (0x0)) ? (rb_state7.var_33) : (0xc5daebd4));
        rb_state7.var_36 = (rb_state7.var_36) - (((((0x4f1c541f) + (rb_state7.var_20)) - (rb_state7.var_21)) ^ (rb_state7.var_35)) + (rb_state7.var_34));
        rb_state7.var_26 = (rb_state7.var_26) - (rb_state7.var_36);
        rb_state7.var_2 = rb_state7.var_26;
        pthread_mutex_unlock(&(rb_state7.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0xc9c86677));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xdec5a5c2)) {
        if ((rb_state11.var_4) != (0x0)) {
            rb_state11.var_21 = !((rb_state11.var_2) == (rb_state11.var_3));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (572 < rb_input_size ? (uint32_t)rb_input[572] : 0xd3a2b1bb);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
    rb_state13.var_0 = (rb_state13.var_0) - (0x81d23732);
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x7e2dc8ce)) ? (rb_state13.var_0) : (0xc1e928d9));
    rb_state13.var_0 = (rb_state13.var_0) - (1760 < rb_input_size ? (uint32_t)rb_input[1760] : 0x5b8bde9);
    if ((rb_state13.var_0) == (0xfc5b90b7)) {
        rb_state13.var_7 = 0xfeb059f0;
        rb_state13.var_8 = (rb_state13.var_8) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_7) : (0x22a15b82));
        rb_state13.var_9 = (rb_state13.var_9) ^ (1935 < rb_input_size ? (uint32_t)rb_input[1935] : 0x507277ec);
        rb_state13.var_10 = (rb_state13.var_10) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_3) : (0x1af7941e));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((((0xe3d1e081) ^ (0xa791a4b7)) + (rb_state13.var_8)) - (rb_state13.var_6)) + (rb_state13.var_9));
        rb_state13.var_12 = (rb_state13.var_12) + (((rb_state13.var_7) == (0xfeb059f0)) ? (rb_state13.var_10) : (0xcc0c72fe));
        rb_state13.var_13 = (rb_state13.var_13) - (((rb_state13.var_8) == (0xfeb059f0)) ? (rb_state13.var_11) : (0xc68bb324));
        rb_state13.var_14 = (rb_state13.var_14) + (((((0x81efb11e) + (rb_state13.var_13)) - (rb_state13.var_10)) ^ (rb_state13.var_12)) - (rb_state13.var_9));
        rb_state13.var_6 = (rb_state13.var_6) - (rb_state13.var_14);
        rb_state13.var_1 = rb_state13.var_6;
    }
    if ((rb_state13.var_0) == (0xfc5b90b7)) {
        pthread_mutex_lock(&(rb_state13.lock_20));
        rb_state13.var_16 = 0x54b1e863;
        rb_state13.var_17 = (rb_state13.var_17) - (((0xcd0d1671) + (rb_state13.var_13)) - (rb_state13.var_12));
        rb_state13.var_18 = (rb_state13.var_18) + (((((0x923beedc) - (rb_state13.var_16)) ^ (rb_state13.var_14)) - (rb_state13.var_11)) ^ (0x9e7c2b7c));
        rb_state13.var_19 = (rb_state13.var_19) ^ (((rb_state13.var_17) == (0x75e387d6)) ? (rb_state13.var_18) : (0xc9c3eac6));
        rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_19);
        rb_state13.var_1 = rb_state13.var_15;
        pthread_mutex_unlock(&(rb_state13.lock_20));
    }
    #endif
    pool->nthreads = nthreads;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (0x8ec73016);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + ((0xc6a7c977) ^ (0x3c69186e));
    if ((rb_state8.var_2) == (0x9e)) {
        pthread_mutex_lock(&(rb_state8.lock_50));
        rb_state8.var_23 = 0xb996f5dd;
        rb_state8.var_24 = (rb_state8.var_24) + (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_23) : (0x3ebe43f7));
        rb_state8.var_25 = (rb_state8.var_25) - (((0x6796ce56) - (0x10842127)) + (rb_state8.var_18));
        rb_state8.var_26 = (rb_state8.var_26) + (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_19) : (0x64e8d170));
        rb_state8.var_27 = (rb_state8.var_27) - (rb_state8.var_24);
        rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_22) == (0x0)) ? (rb_state8.var_25) : (0x13a14d5f));
        rb_state8.var_29 = (rb_state8.var_29) + (((0xda70f883) - (rb_state8.var_23)) ^ (rb_state8.var_26));
        rb_state8.var_30 = (rb_state8.var_30) + (rb_state8.var_27);
        rb_state8.var_31 = (rb_state8.var_31) + (((rb_state8.var_24) == (0x0)) ? (rb_state8.var_28) : (0xfebea246));
        rb_state8.var_32 = (rb_state8.var_32) ^ (((rb_state8.var_25) == (0x0)) ? (rb_state8.var_29) : (0xf999c3c2));
        rb_state8.var_33 = (rb_state8.var_33) + (((rb_state8.var_30) == (0x0)) ? (rb_state8.var_31) : (0xa0e532ba));
        rb_state8.var_34 = (rb_state8.var_34) ^ (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_32) : (0xc304c33b));
        rb_state8.var_35 = (rb_state8.var_35) - (((0xaa769189) + (rb_state8.var_33)) + (rb_state8.var_27));
        rb_state8.var_36 = (rb_state8.var_36) + (((0xe358f40a) - (rb_state8.var_34)) + (rb_state8.var_28));
        rb_state8.var_37 = (rb_state8.var_37) ^ (((0xa80f83f4) - (rb_state8.var_29)) - (rb_state8.var_35));
        rb_state8.var_38 = (rb_state8.var_38) - (((rb_state8.var_36) == (0x0)) ? (rb_state8.var_37) : (0xaa12ce2));
        rb_state8.var_22 = (rb_state8.var_22) ^ (rb_state8.var_38);
        rb_state8.var_9 = rb_state8.var_22;
        pthread_mutex_unlock(&(rb_state8.lock_50));
    }
    if ((rb_state8.var_2) == (0x9e)) {
        pthread_mutex_lock(&(rb_state8.lock_50));
        rb_state8.var_40 = 0xdd860090;
        rb_state8.var_41 = 0xf6495291;
        rb_state8.var_42 = 0x2ff433b2;
        rb_state8.var_43 = (rb_state8.var_43) - (0x87c48155);
        rb_state8.var_44 = (rb_state8.var_44) - (((rb_state8.var_42) == (0x0)) ? (rb_state8.var_41) : (0x2b373a40));
        rb_state8.var_45 = (rb_state8.var_45) + (((rb_state8.var_40) == (0x0)) ? (rb_state8.var_43) : (0xc003a0dc));
        rb_state8.var_46 = (rb_state8.var_46) + (((0x795b95e8) ^ (rb_state8.var_44)) ^ (rb_state8.var_30));
        rb_state8.var_47 = (rb_state8.var_47) + (((rb_state8.var_31) == (0x0)) ? (rb_state8.var_45) : (0xc19ab127));
        rb_state8.var_48 = (rb_state8.var_48) + (((0x1fc780c6) - (0x1437e00b)) ^ (rb_state8.var_46));
        rb_state8.var_49 = (rb_state8.var_49) ^ (((rb_state8.var_47) == (0x0)) ? (rb_state8.var_48) : (0xb525d90e));
        rb_state8.var_39 = (rb_state8.var_39) + (rb_state8.var_49);
        rb_state8.var_9 = rb_state8.var_39;
        pthread_mutex_unlock(&(rb_state8.lock_50));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xf944be9d));
    rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_1);
    if ((rb_state10.var_1) == (0x558072ae)) {
        rb_state10.var_2 = rb_state10.var_0;
    }
    if ((rb_state10.var_1) == (0x558072ae)) {
        if (!((rb_state10.var_0) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_14) : (0x53b5be2a));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0x5d8d3297) ^ (0x225ef47e));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0xd213c3ee);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x105b0755)) {
        pthread_mutex_lock(&(rb_state15.lock_20));
        if ((rb_state15.var_3) != (0x0)) {
            rb_state15.var_21 = !((rb_state15.var_1) == (rb_state15.var_2));
        }
        pthread_mutex_unlock(&(rb_state15.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xf63e9dfc)) {
        rb_state16.var_11 = 0x9abffe58;
        rb_state16.var_12 = 0x272166cb;
        rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_7);
        rb_state16.var_14 = (rb_state16.var_14) ^ (((0xf7f64999) ^ (rb_state16.var_12)) ^ (rb_state16.var_8));
        rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_11) : (0x532465fd));
        rb_state16.var_16 = (rb_state16.var_16) + (rb_state16.var_13);
        rb_state16.var_17 = (rb_state16.var_17) - (rb_state16.var_14);
        rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_16) : (0x2381e993));
        rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_17) : (0x71597938));
        rb_state16.var_20 = (rb_state16.var_20) ^ (((0xd23ff6d9) ^ (rb_state16.var_18)) ^ (rb_state16.var_11));
        rb_state16.var_21 = (rb_state16.var_21) + (rb_state16.var_19);
        rb_state16.var_22 = (rb_state16.var_22) + (((((0x3fb44510) + (0x22f8a821)) + (rb_state16.var_20)) - (rb_state16.var_21)) + (rb_state16.var_12));
        rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_22);
        rb_state16.var_2 = rb_state16.var_10;
    }
    if ((rb_state16.var_0) == (0xf63e9dfc)) {
        pthread_mutex_lock(&(rb_state16.lock_32));
        rb_state16.var_24 = 0xbb4596c0;
        rb_state16.var_25 = (rb_state16.var_25) + (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_24) : (0x9117a9f9));
        rb_state16.var_26 = (rb_state16.var_26) - (rb_state16.var_14);
        rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_25) : (0x7105b0e8));
        rb_state16.var_28 = (rb_state16.var_28) - (rb_state16.var_26);
        rb_state16.var_29 = (rb_state16.var_29) ^ (rb_state16.var_27);
        rb_state16.var_30 = (rb_state16.var_30) - (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_28) : (0xf308042d));
        rb_state16.var_31 = (rb_state16.var_31) - (((((0x2b704ce3) + (0xd344e918)) + (rb_state16.var_29)) ^ (rb_state16.var_30)) - (rb_state16.var_17));
        rb_state16.var_23 = (rb_state16.var_23) - (rb_state16.var_31);
        rb_state16.var_2 = rb_state16.var_23;
        pthread_mutex_unlock(&(rb_state16.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0x1531bb3d));
    if ((rb_state18.var_0) == (0x334e0350)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0x334e0350)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    pool->state = POOL_STATE_RUNNING;

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

    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        rb_state17.var_3 = 0xc219af1c;
        rb_state17.var_4 = 0xe601343;
        rb_state17.var_5 = (rb_state17.var_5) - (((rb_state17.var_3) == (0xc219af1c)) ? (rb_state17.var_2) : (0x4c665d5e));
        rb_state17.var_6 = (rb_state17.var_6) + (((((0xb58d1ffa) ^ (0x600897ee)) + (0x66ca6e37)) + (rb_state17.var_5)) ^ (rb_state17.var_4));
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_6);
    }
    #endif
    assert(pool != NULL);
    assert(pool->state == POOL_STATE_RUNNING);

    for (i = 0; i < pool->nthreads; i++) {
        if (value_ptrs != NULL) {
            value_ptr = &(value_ptrs[i]);
        } else {
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_11) == (0x2a947d1e)) {
                rb_state15.var_22 = rb_state15.var_12;
            }
            if ((rb_state15.var_11) == (0x2a947d1e)) {
                if (!((rb_state15.var_12) == (rb_state15.var_22))) {
                    racebench_trigger(15);
                }
            }
            #endif
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