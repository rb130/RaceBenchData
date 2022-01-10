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

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (1963 < rb_input_size ? (uint32_t)rb_input[1963] : 0x28b65ba);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ (2078 < rb_input_size ? (uint32_t)rb_input[2078] : 0xb982dbc0);
    if ((rb_state11.var_1) == (0x817296b7)) {
        rb_state11.var_5 = 0x72476c99;
        rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_4);
        rb_state11.var_7 = (rb_state11.var_7) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_5) : (0xb0ea1a3d));
        rb_state11.var_8 = (rb_state11.var_8) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0xff3dc7ba));
        rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_7) : (0xb50d0626));
        rb_state11.var_10 = (rb_state11.var_10) ^ (rb_state11.var_8);
        rb_state11.var_11 = (rb_state11.var_11) - (rb_state11.var_9);
        rb_state11.var_12 = (rb_state11.var_12) + (((0x6c306953) ^ (rb_state11.var_10)) ^ (0xe0f8864f));
        rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_11) : (0x15b87dc5));
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_13);
        rb_state11.var_16 = (rb_state11.var_16) - (((0xd6531b57) + (rb_state11.var_14)) ^ (0x45aee78a));
        rb_state11.var_17 = (rb_state11.var_17) - (((0x963b8c76) ^ (rb_state11.var_15)) + (0xbbff17ad));
        rb_state11.var_18 = (rb_state11.var_18) ^ (rb_state11.var_16);
        rb_state11.var_19 = (rb_state11.var_19) ^ (((((0xa6496c83) - (rb_state11.var_17)) + (rb_state11.var_10)) - (rb_state11.var_9)) - (rb_state11.var_18));
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (0x1ab4d3ae);
    #endif
    pool = (tpool_t *)malloc(sizeof(tpool_t));
    if (pool == NULL) {
        return NULL;
    }
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x5ad01449)) {
        pthread_mutex_lock(&(rb_state3.lock_20));
        rb_state3.var_6 = 0xf7adff77;
        rb_state3.var_7 = 0x5e87b57a;
        rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x15c35e20));
        rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_7);
        rb_state3.var_10 = (rb_state3.var_10) - (rb_state3.var_6);
        rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_9) : (0xaff8ed6e));
        rb_state3.var_12 = (rb_state3.var_12) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_10) : (0x906b8e24));
        rb_state3.var_13 = (rb_state3.var_13) + (((0x12412831) - (0x1fbd2ec2)) ^ (rb_state3.var_11));
        rb_state3.var_14 = (rb_state3.var_14) ^ (((0xaf6bbad9) - (rb_state3.var_12)) ^ (rb_state3.var_7));
        rb_state3.var_15 = (rb_state3.var_15) ^ (((((0xb168bde) ^ (0x4dc38084)) + (rb_state3.var_8)) ^ (rb_state3.var_14)) + (rb_state3.var_13));
        rb_state3.var_5 = (rb_state3.var_5) + (rb_state3.var_15);
        rb_state3.var_4 = rb_state3.var_5;
        pthread_mutex_unlock(&(rb_state3.lock_20));
    }
    if ((rb_state3.var_1) == (0x5ad01449)) {
        if (!((rb_state3.var_4) == (rb_state3.var_5))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ (798 < rb_input_size ? (uint32_t)rb_input[798] : 0xa003d31b);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (0x2d16fd1f);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0x76e8b64e)) {
        pthread_mutex_lock(&(rb_state9.lock_13));
        rb_state9.var_6 = (rb_state9.var_6) - (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_4) : (0x6b10758b));
        rb_state9.var_5 = (rb_state9.var_5) - (rb_state9.var_6);
        rb_state9.var_4 = rb_state9.var_5;
        pthread_mutex_unlock(&(rb_state9.lock_13));
    }
    if ((rb_state9.var_1) == (0x76e8b64e)) {
        pthread_mutex_lock(&(rb_state9.lock_13));
        if (!((rb_state9.var_4) == (rb_state9.var_5))) {
            racebench_trigger(9);
        }
        pthread_mutex_unlock(&(rb_state9.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (0xdb0fff61);
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x6371ef85);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_2);
    #endif
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    for (i = 0; i < nthreads; i++) {
        if (opts[i].attr == NULL) {
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_0) == (0xbe2f8426)) {
                rb_state5.var_5 = 0x9c0a79f8;
                rb_state5.var_6 = 0x97f10b18;
                rb_state5.var_7 = (rb_state5.var_7) - (454 < rb_input_size ? (uint32_t)rb_input[454] : 0xf96a51f7);
                rb_state5.var_8 = (rb_state5.var_8) + (0x679fa38c);
                rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_5) == (0x9c0a79f8)) ? (rb_state5.var_6) : (0x5bd75245));
                rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_5) == (0x9c0a79f8)) ? (rb_state5.var_4) : (0x7bfe776c));
                rb_state5.var_11 = (rb_state5.var_11) + (((0x5a4f6ef1) - (rb_state5.var_7)) ^ (0xc8b37c82));
                rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_6) == (0x97f10b18)) ? (rb_state5.var_8) : (0xc95499));
                rb_state5.var_13 = (rb_state5.var_13) ^ (((0xfaf01c4d) ^ (rb_state5.var_7)) + (rb_state5.var_9));
                rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_8) == (0x679fa38c)) ? (rb_state5.var_10) : (0x45aab00e));
                rb_state5.var_15 = (rb_state5.var_15) ^ (((rb_state5.var_9) == (0x97f10b18)) ? (rb_state5.var_11) : (0x8a073533));
                rb_state5.var_16 = (rb_state5.var_16) + (rb_state5.var_12);
                rb_state5.var_17 = (rb_state5.var_17) ^ (((((0x54813328) + (rb_state5.var_11)) - (rb_state5.var_13)) ^ (rb_state5.var_10)) ^ (rb_state5.var_14));
                rb_state5.var_18 = (rb_state5.var_18) ^ (((0x20a16924) ^ (0xc0254d97)) ^ (rb_state5.var_15));
                rb_state5.var_19 = (rb_state5.var_19) ^ (rb_state5.var_16);
                rb_state5.var_20 = (rb_state5.var_20) + (((((0x9f0eea4c) - (0xe6e26e3)) ^ (rb_state5.var_17)) ^ (rb_state5.var_18)) - (0x855a6125));
                rb_state5.var_21 = (rb_state5.var_21) - (((((0x39d26270) ^ (rb_state5.var_19)) ^ (rb_state5.var_20)) - (rb_state5.var_12)) - (0x2911cffe));
                rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_21);
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            if ((rb_state10.var_2) == (0xce233328)) {
                if ((rb_state10.var_3) != (0x0)) {
                    if (!((rb_state10.var_3) == (rb_state10.var_15))) {
                        racebench_trigger(10);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_1 = (rb_state13.var_1) ^ (2095 < rb_input_size ? (uint32_t)rb_input[2095] : 0x38552d75);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_3);
            #endif
            attr = NULL;
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x7aaa9b20)) ? (rb_state4.var_1) : (0xec8e822f));
            #endif
            arg = NULL;
        } else {
            arg = opts[i].arg;
        }

        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (2145 < rb_input_size ? (uint32_t)rb_input[2145] : 0x66fb271a);
        rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_1) : (0xc832ad5f));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x1ed4fdfe)) {
            pthread_mutex_lock(&(rb_state14.lock_10));
            rb_state14.var_5 = (rb_state14.var_5) - (((((0x3d369bae) + (rb_state14.var_3)) ^ (0x7d908fbb)) - (rb_state14.var_4)) ^ (rb_state14.var_5));
            rb_state14.var_4 = (rb_state14.var_4) - (rb_state14.var_5);
            rb_state14.var_3 = rb_state14.var_4;
            pthread_mutex_unlock(&(rb_state14.lock_10));
        }
        if ((rb_state14.var_1) == (0x3ad85ee)) {
            if (!((rb_state14.var_3) == (rb_state14.var_4))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_1) == (0x9242d421)) {
            rb_state17.var_15 = 0x2058ff8e;
            rb_state17.var_16 = 0x4cac4d00;
            rb_state17.var_17 = 0x75e15211;
            rb_state17.var_18 = (rb_state17.var_18) + (((0xdce127fa) - (rb_state17.var_17)) - (0xbb95a8bb));
            rb_state17.var_19 = (rb_state17.var_19) ^ (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_16) : (0xda9ada89));
            rb_state17.var_20 = (rb_state17.var_20) ^ (rb_state17.var_15);
            rb_state17.var_21 = (rb_state17.var_21) ^ (rb_state17.var_18);
            rb_state17.var_22 = (rb_state17.var_22) + (((0x1604041b) - (rb_state17.var_12)) - (rb_state17.var_19));
            rb_state17.var_23 = (rb_state17.var_23) + (((((0x8153b354) - (rb_state17.var_13)) - (rb_state17.var_21)) ^ (rb_state17.var_20)) ^ (rb_state17.var_14));
            rb_state17.var_24 = (rb_state17.var_24) ^ (rb_state17.var_22);
            rb_state17.var_25 = (rb_state17.var_25) + (((0xc82b1834) + (rb_state17.var_15)) - (rb_state17.var_23));
            rb_state17.var_26 = (rb_state17.var_26) - (((rb_state17.var_16) == (0x4cac4d00)) ? (rb_state17.var_24) : (0x8258775));
            rb_state17.var_27 = (rb_state17.var_27) - (((0xc1380258) ^ (rb_state17.var_17)) - (rb_state17.var_25));
            rb_state17.var_28 = (rb_state17.var_28) + (((((0xa56be131) - (rb_state17.var_27)) - (rb_state17.var_26)) - (0x9b0ec6d8)) ^ (0xac5d98b3));
            rb_state17.var_14 = (rb_state17.var_14) + (rb_state17.var_28);
            rb_state17.var_3 = rb_state17.var_14;
        }
        if ((rb_state17.var_1) == (0x9242d421)) {
            pthread_mutex_lock(&(rb_state17.lock_38));
            rb_state17.var_30 = 0xca424048;
            rb_state17.var_31 = (rb_state17.var_31) - (((rb_state17.var_20) == (0x2058ff8e)) ? (rb_state17.var_19) : (0xbccd50cd));
            rb_state17.var_32 = (rb_state17.var_32) + (((((0x5599de58) + (rb_state17.var_18)) ^ (rb_state17.var_30)) ^ (rb_state17.var_21)) + (rb_state17.var_22));
            rb_state17.var_33 = (rb_state17.var_33) + (rb_state17.var_23);
            rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_24) == (0xc957b71b)) ? (rb_state17.var_31) : (0x3350e40d));
            rb_state17.var_35 = (rb_state17.var_35) + (((0x6e015cf1) - (rb_state17.var_32)) + (rb_state17.var_25));
            rb_state17.var_36 = (rb_state17.var_36) - (((rb_state17.var_33) == (0xf5b179a8)) ? (rb_state17.var_34) : (0x9c1093de));
            rb_state17.var_37 = (rb_state17.var_37) ^ (((rb_state17.var_35) == (0x374fdd10)) ? (rb_state17.var_36) : (0xb298fa2e));
            rb_state17.var_29 = (rb_state17.var_29) - (rb_state17.var_37);
            rb_state17.var_3 = rb_state17.var_29;
            pthread_mutex_unlock(&(rb_state17.lock_38));
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
    rb_state0.var_0 = (rb_state0.var_0) ^ (0xb1d276a5);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x97f059f2) + (0xdda2966b));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
    rb_state4.var_2 = (rb_state4.var_2) + (0xe09bd170);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ ((0xbe2f8452) - (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x30d06685)) ? (rb_state9.var_1) : (0x1a0042ca));
    rb_state9.var_1 = (rb_state9.var_1) - ((0x41c25609) + (0xcb7f08bf));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + ((0x8692a704) ^ (rb_state13.var_0));
    rb_state13.var_2 = (rb_state13.var_2) + (0xaa6b30af);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_0);
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_2);
    #endif
    pool->nthreads = nthreads;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - ((0xee8cecce) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (2026 < rb_input_size ? (uint32_t)rb_input[2026] : 0xdd11be8);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_1);
    rb_state6.var_2 = (rb_state6.var_2) - (1095 < rb_input_size ? (uint32_t)rb_input[1095] : 0x3b2b03d5);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (193 < rb_input_size ? (uint32_t)rb_input[193] : 0x323aadec);
    if ((rb_state7.var_2) == (0xc054f499)) {
        rb_state7.var_3 = rb_state7.var_0;
    }
    if ((rb_state7.var_2) == (0xc054f499)) {
        if (!((rb_state7.var_0) == (rb_state7.var_3))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (0xe2264d4c);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x1963b902) + (rb_state17.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (932 < rb_input_size ? (uint32_t)rb_input[932] : 0xc02df9ec);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) + (119 < rb_input_size ? (uint32_t)rb_input[119] : 0xdd4c1a72);
    if ((rb_state19.var_3) == (0xb0)) {
        rb_state19.var_15 = 0x68782ac3;
        rb_state19.var_16 = 0x9f382f81;
        rb_state19.var_17 = (rb_state19.var_17) - (rb_state19.var_8);
        rb_state19.var_18 = (rb_state19.var_18) + (((((0x4159e137) + (0x473816d7)) - (rb_state19.var_9)) + (rb_state19.var_15)) + (rb_state19.var_16));
        rb_state19.var_19 = (rb_state19.var_19) - (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_17) : (0x6a3179df));
        rb_state19.var_20 = (rb_state19.var_20) - (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_18) : (0x1c4422e1));
        rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_19) : (0x10d2b23));
        rb_state19.var_22 = (rb_state19.var_22) - (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_20) : (0x20d9ecfb));
        rb_state19.var_23 = (rb_state19.var_23) - (((0xa02e1302) + (rb_state19.var_21)) + (0x900bfca0));
        rb_state19.var_24 = (rb_state19.var_24) + (rb_state19.var_22);
        rb_state19.var_25 = (rb_state19.var_25) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_23) : (0xab34351));
        rb_state19.var_26 = (rb_state19.var_26) - (rb_state19.var_24);
        rb_state19.var_27 = (rb_state19.var_27) - (rb_state19.var_25);
        rb_state19.var_28 = (rb_state19.var_28) - (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_27) : (0x7988043e));
        rb_state19.var_4 = (rb_state19.var_4) - (rb_state19.var_28);
    }
    #endif
    pool->state = POOL_STATE_RUNNING;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0xfffffffb)) {
        if ((rb_state1.var_4) != (0x0)) {
            if (!((rb_state1.var_4) == (rb_state1.var_21))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (1427 < rb_input_size ? (uint32_t)rb_input[1427] : 0x6cbc799c);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_2) == (0x48952007)) {
        pthread_mutex_lock(&(rb_state4.lock_54));
        rb_state4.var_5 = 0x743031b;
        rb_state4.var_6 = 0x7762b59c;
        rb_state4.var_7 = (rb_state4.var_7) - (0x7a4ea7f9);
        rb_state4.var_8 = (rb_state4.var_8) ^ (rb_state4.var_3);
        rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_6);
        rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x2ac191ca));
        rb_state4.var_11 = (rb_state4.var_11) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_7) : (0x1c1b75cf));
        rb_state4.var_12 = (rb_state4.var_12) + (((((0xb6e7beb0) + (rb_state4.var_8)) - (0x4de8d5)) ^ (rb_state4.var_6)) + (rb_state4.var_9));
        rb_state4.var_13 = (rb_state4.var_13) - (((0xc28bb5a4) + (rb_state4.var_7)) ^ (rb_state4.var_10));
        rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_11) : (0x2d6d074d));
        rb_state4.var_15 = (rb_state4.var_15) + (rb_state4.var_12);
        rb_state4.var_16 = (rb_state4.var_16) + (((0x9face5c5) + (rb_state4.var_13)) ^ (rb_state4.var_9));
        rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_14) : (0xaf8ba58a));
        rb_state4.var_18 = (rb_state4.var_18) - (((0xd7efdfdb) + (0x9d9639c6)) + (rb_state4.var_15));
        rb_state4.var_19 = (rb_state4.var_19) - (((0x484eae02) - (rb_state4.var_16)) - (0x4a388ced));
        rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_17) : (0x7c647688));
        rb_state4.var_21 = (rb_state4.var_21) + (rb_state4.var_18);
        rb_state4.var_22 = (rb_state4.var_22) + (((((0xe0602ae9) ^ (0xfed034a9)) - (0x7c8185fb)) + (rb_state4.var_20)) - (rb_state4.var_19));
        rb_state4.var_23 = (rb_state4.var_23) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_21) : (0xb72a3561));
        rb_state4.var_24 = (rb_state4.var_24) + (rb_state4.var_22);
        rb_state4.var_25 = (rb_state4.var_25) + (((0xad41369f) - (rb_state4.var_23)) ^ (0x83f47390));
        rb_state4.var_26 = (rb_state4.var_26) + (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_25) : (0x33c358bd));
        rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_26);
        rb_state4.var_3 = rb_state4.var_4;
        pthread_mutex_unlock(&(rb_state4.lock_54));
    }
    if ((rb_state4.var_2) == (0x48952007)) {
        pthread_mutex_lock(&(rb_state4.lock_54));
        if (!((rb_state4.var_3) == (rb_state4.var_4))) {
            racebench_trigger(4);
        }
        pthread_mutex_unlock(&(rb_state4.lock_54));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (0x10671828);
    rb_state5.var_2 = (rb_state5.var_2) + (1904 < rb_input_size ? (uint32_t)rb_input[1904] : 0x77ef9594);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_3);
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_2) : (0x7d75c064));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (0x31af37e4);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - ((0xe1dcb1c5) + (0xcc0f7862));
    rb_state12.var_1 = (rb_state12.var_1) - (0x918a8771);
    rb_state12.var_2 = (rb_state12.var_2) ^ ((0xb978cf61) ^ (rb_state12.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_0);
    if ((rb_state13.var_2) == (0xff41920d)) {
        rb_state13.var_10 = (rb_state13.var_10) + (((((0x7cea8648) + (rb_state13.var_8)) + (rb_state13.var_10)) - (0x84e2027c)) - (rb_state13.var_9));
        rb_state13.var_11 = (rb_state13.var_11) + (0xd7267402);
        rb_state13.var_12 = (rb_state13.var_12) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_7) : (0x3b42d1e3));
        rb_state13.var_13 = (rb_state13.var_13) ^ (((((0x42e76969) + (rb_state13.var_11)) + (rb_state13.var_13)) - (rb_state13.var_10)) - (rb_state13.var_12));
        rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_12) : (0xfde7f3ab));
        rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_13);
        rb_state13.var_16 = (rb_state13.var_16) ^ (rb_state13.var_14);
        rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0x8e719e82));
        rb_state13.var_9 = (rb_state13.var_9) + (rb_state13.var_17);
        rb_state13.var_3 = rb_state13.var_9;
    }
    if ((rb_state13.var_2) == (0xff41920d)) {
        pthread_mutex_lock(&(rb_state13.lock_26));
        rb_state13.var_19 = 0xbc6d3153;
        rb_state13.var_20 = 0x35333dc7;
        rb_state13.var_21 = 0x5c5e21bf;
        rb_state13.var_22 = (rb_state13.var_22) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_20) : (0x79890eec));
        rb_state13.var_23 = (rb_state13.var_23) - (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_21) : (0xb5f0519f));
        rb_state13.var_24 = (rb_state13.var_24) - (rb_state13.var_22);
        rb_state13.var_25 = (rb_state13.var_25) ^ (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_24) : (0x74e6951c));
        rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_25);
        rb_state13.var_3 = rb_state13.var_18;
        pthread_mutex_unlock(&(rb_state13.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (0xaff53c17);
    rb_state15.var_3 = (rb_state15.var_3) + ((0x1b145571) + (rb_state15.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_21));
        rb_state16.var_6 = 0x7927feb9;
        rb_state16.var_7 = 0xa249fcae;
        rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_4) : (0x8ef81504));
        rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_5);
        rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0x7d9fa7bb));
        rb_state16.var_11 = (rb_state16.var_11) ^ (((((0x45b25634) + (rb_state16.var_7)) ^ (0x9ef6e577)) + (rb_state16.var_9)) ^ (rb_state16.var_8));
        rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_10) : (0x98f9fd9b));
        rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_11);
        rb_state16.var_14 = (rb_state16.var_14) - (rb_state16.var_12);
        rb_state16.var_15 = (rb_state16.var_15) + (rb_state16.var_13);
        rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_15) : (0x81b9f9cf));
        rb_state16.var_5 = (rb_state16.var_5) - (rb_state16.var_16);
        rb_state16.var_4 = rb_state16.var_5;
        pthread_mutex_unlock(&(rb_state16.lock_21));
    }
    if ((rb_state16.var_0) == (0x0)) {
        if (!((rb_state16.var_4) == (rb_state16.var_5))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ ((0xbe730cf9) + (0xb30756cd));
    rb_state17.var_1 = (rb_state17.var_1) + (0x57cd9c41);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (((rb_state18.var_0) == (0x944c4146)) ? (rb_state18.var_2) : (0x2a15ba36));
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

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x9fada121)) {
        rb_state12.var_5 = (rb_state12.var_5) - (((0xb6f785c4) + (0x2533e5f9)) ^ (rb_state12.var_3));
        rb_state12.var_6 = (rb_state12.var_6) + (2155 < rb_input_size ? (uint32_t)rb_input[2155] : 0xbb6c4094);
        rb_state12.var_7 = (rb_state12.var_7) - ((0xefff6dd3) - (rb_state12.var_5));
        rb_state12.var_8 = (rb_state12.var_8) ^ (((((0x1921150d) - (rb_state12.var_5)) - (rb_state12.var_4)) ^ (0xe0273a4b)) - (0x23852826));
        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_6);
        rb_state12.var_10 = (rb_state12.var_10) - (rb_state12.var_7);
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_8) == (0x31f488b8)) ? (rb_state12.var_9) : (0xc64a134b));
        rb_state12.var_12 = (rb_state12.var_12) + (rb_state12.var_10);
        rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_11);
        rb_state12.var_14 = (rb_state12.var_14) + (((0xfc7ee19a) ^ (0xa348369a)) - (rb_state12.var_12));
        rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_6) == (0xb6)) ? (rb_state12.var_13) : (0x58fdf24a));
        rb_state12.var_16 = (rb_state12.var_16) - (((((0x2e3741fc) - (rb_state12.var_7)) ^ (rb_state12.var_14)) ^ (rb_state12.var_15)) ^ (rb_state12.var_8));
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_16);
    }
    #endif
    assert(pool != NULL);
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