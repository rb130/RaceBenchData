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
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_4 = (rb_state2.var_4) ^ (0xcbf68ac);
    if ((rb_state2.var_3) == (0x0)) {
        rb_state2.var_5 = rb_state2.var_4;
    }
    if ((rb_state2.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_11));
        if (!((rb_state2.var_4) == (rb_state2.var_5))) {
            racebench_trigger(2);
        }
        pthread_mutex_unlock(&(rb_state2.lock_11));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (543 < rb_input_size ? (uint32_t)rb_input[543] : 0x1aa315b1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0x117129c9));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xa814f211)) {
        rb_state11.var_8 = 0xd5051fee;
        rb_state11.var_9 = 0x433e596c;
        rb_state11.var_10 = (rb_state11.var_10) + (((rb_state11.var_8) == (0xd5051fee)) ? (rb_state11.var_9) : (0x2ee63aee));
        rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_9) == (0x433e596c)) ? (rb_state11.var_8) : (0xba9e42ee));
        rb_state11.var_12 = (rb_state11.var_12) ^ (((0xdbd63be4) - (rb_state11.var_7)) ^ (rb_state11.var_10));
        rb_state11.var_13 = (rb_state11.var_13) + (((((0xbe2c5edb) - (rb_state11.var_11)) - (0xaa84c3c5)) + (rb_state11.var_11)) ^ (rb_state11.var_10));
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) - (((((0xa529cf0f) - (rb_state11.var_14)) ^ (0xf6ab241f)) - (0x76e1f160)) ^ (rb_state11.var_13));
        rb_state11.var_7 = (rb_state11.var_7) - (rb_state11.var_15);
        rb_state11.var_4 = rb_state11.var_7;
    }
    if ((rb_state11.var_1) == (0xa814f211)) {
        pthread_mutex_lock(&(rb_state11.lock_24));
        rb_state11.var_17 = (rb_state11.var_17) + (((0x4c0b3921) ^ (rb_state11.var_14)) - (rb_state11.var_15));
        rb_state11.var_18 = (rb_state11.var_18) + (((0x54994e6f) + (rb_state11.var_16)) ^ (rb_state11.var_13));
        rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_12) == (0x98e86288)) ? (rb_state11.var_17) : (0xc0aecea8));
        rb_state11.var_20 = (rb_state11.var_20) ^ (1205 < rb_input_size ? (uint32_t)rb_input[1205] : 0x756d473e);
        rb_state11.var_21 = (rb_state11.var_21) - (((0xdb74d197) - (rb_state11.var_17)) - (rb_state11.var_18));
        rb_state11.var_22 = (rb_state11.var_22) ^ (((((0x1e427799) + (rb_state11.var_20)) + (rb_state11.var_18)) + (rb_state11.var_19)) + (0xca695932));
        rb_state11.var_23 = (rb_state11.var_23) - (((rb_state11.var_21) == (0xd200ab69)) ? (rb_state11.var_22) : (0x8b00467a));
        rb_state11.var_16 = (rb_state11.var_16) + (rb_state11.var_23);
        rb_state11.var_4 = rb_state11.var_16;
        pthread_mutex_unlock(&(rb_state11.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + ((0x59acf735) ^ (rb_state14.var_0));
    if ((rb_state14.var_1) == (0x3)) {
        if ((rb_state14.var_4) != (0x0)) {
            if (!((rb_state14.var_4) == (rb_state14.var_20))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (132 < rb_input_size ? (uint32_t)rb_input[132] : 0xc63f151a);
    rb_state17.var_3 = (rb_state17.var_3) + (1281 < rb_input_size ? (uint32_t)rb_input[1281] : 0x555f564f);
    if ((rb_state17.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_18));
        rb_state17.var_6 = (rb_state17.var_6) ^ (1112 < rb_input_size ? (uint32_t)rb_input[1112] : 0xfab1c365);
        rb_state17.var_7 = (rb_state17.var_7) + (rb_state17.var_4);
        rb_state17.var_8 = (rb_state17.var_8) ^ (((0x9bd1d368) ^ (rb_state17.var_5)) + (rb_state17.var_6));
        rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0x4c9ccf81));
        rb_state17.var_10 = (rb_state17.var_10) + (((((0x3184e1cb) ^ (rb_state17.var_7)) ^ (rb_state17.var_9)) ^ (rb_state17.var_8)) ^ (0x5f610b84));
        rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_10);
        rb_state17.var_4 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_18));
    }
    if ((rb_state17.var_2) == (0x0)) {
        if (!((rb_state17.var_4) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xfffffffc)) {
        rb_state18.var_4 = 0xe8c140da;
        rb_state18.var_5 = (rb_state18.var_5) ^ (((0xdc95d4cb) + (rb_state18.var_3)) + (0x5e204c19));
        rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_4) == (0xe8c140da)) ? (rb_state18.var_5) : (0xfa76b9d9));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_6);
    }
    #endif
    pool = (tpool_t *)malloc(sizeof(tpool_t));
    if (pool == NULL) {
        return NULL;
    }
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - (((rb_state3.var_2) == (0xa9)) ? (rb_state3.var_2) : (0xb49ae9e3));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_4) == (0xf5)) {
        pthread_mutex_lock(&(rb_state5.lock_41));
        rb_state5.var_7 = 0x5100894e;
        rb_state5.var_8 = 0x9c466681;
        rb_state5.var_9 = 0x14c230fa;
        rb_state5.var_10 = (rb_state5.var_10) ^ (rb_state5.var_5);
        rb_state5.var_11 = (rb_state5.var_11) + (((((0x221e9a39) - (rb_state5.var_9)) - (rb_state5.var_6)) ^ (0xca881f5c)) - (rb_state5.var_7));
        rb_state5.var_12 = (rb_state5.var_12) + (((0xf46db468) + (rb_state5.var_7)) + (rb_state5.var_8));
        rb_state5.var_13 = (rb_state5.var_13) - (rb_state5.var_10);
        rb_state5.var_14 = (rb_state5.var_14) + (((rb_state5.var_8) == (0x9c466681)) ? (rb_state5.var_11) : (0x6b25ab1c));
        rb_state5.var_15 = (rb_state5.var_15) + (((rb_state5.var_12) == (0xe1b4a437)) ? (rb_state5.var_13) : (0xb47646b4));
        rb_state5.var_16 = (rb_state5.var_16) - (((0xdb3d2dbd) ^ (rb_state5.var_9)) ^ (rb_state5.var_14));
        rb_state5.var_17 = (rb_state5.var_17) ^ (((0x1b3abd06) + (rb_state5.var_15)) - (rb_state5.var_10));
        rb_state5.var_18 = (rb_state5.var_18) + (((rb_state5.var_11) == (0x76d3ed15)) ? (rb_state5.var_16) : (0xd62dbfb0));
        rb_state5.var_19 = (rb_state5.var_19) + (((0x6809d9bc) + (rb_state5.var_12)) - (rb_state5.var_17));
        rb_state5.var_20 = (rb_state5.var_20) - (((0xed2a18d0) ^ (rb_state5.var_13)) ^ (rb_state5.var_18));
        rb_state5.var_21 = (rb_state5.var_21) ^ (rb_state5.var_19);
        rb_state5.var_22 = (rb_state5.var_22) + (((0x65a9401a) + (rb_state5.var_14)) - (rb_state5.var_20));
        rb_state5.var_23 = (rb_state5.var_23) ^ (((rb_state5.var_21) == (0x2e83c0ed)) ? (rb_state5.var_22) : (0xb29d06e6));
        rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_23);
        rb_state5.var_5 = rb_state5.var_6;
        pthread_mutex_unlock(&(rb_state5.lock_41));
    }
    if ((rb_state5.var_4) == (0xf5)) {
        pthread_mutex_lock(&(rb_state5.lock_41));
        if (!((rb_state5.var_5) == (rb_state5.var_6))) {
            racebench_trigger(5);
        }
        pthread_mutex_unlock(&(rb_state5.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - ((0xa5465bb0) - (0x7c305bc4));
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_1);
    rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_2) : (0xbed531db));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_2) : (0x637b9400));
    rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_3);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (((rb_state15.var_0) == (0x5fe3ad72)) ? (rb_state15.var_2) : (0x152407ff));
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
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_8);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_2 = (rb_state7.var_2) - ((0x30e5d587) - (0xedcfc556));
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_1 = (rb_state8.var_1) - ((0xd31b1005) ^ (0x2eba6756));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0xd893227d)) {
                rb_state12.var_5 = 0x45340cbf;
                rb_state12.var_6 = 0x5b43745d;
                rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_5) == (0x45340cbf)) ? (rb_state12.var_5) : (0xa7f42774));
                rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_6) == (0x5b43745d)) ? (rb_state12.var_4) : (0x55cf592e));
                rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_6);
                rb_state12.var_10 = (rb_state12.var_10) + (((0xa68c720e) ^ (rb_state12.var_7)) ^ (rb_state12.var_7));
                rb_state12.var_11 = (rb_state12.var_11) ^ (((0x82e2e3fd) - (rb_state12.var_8)) - (0xc9a73890));
                rb_state12.var_12 = (rb_state12.var_12) - (rb_state12.var_9);
                rb_state12.var_13 = (rb_state12.var_13) + (((rb_state12.var_8) == (0xaa17696a)) ? (rb_state12.var_10) : (0x7c699c15));
                rb_state12.var_14 = (rb_state12.var_14) - (((0x10900ae) + (rb_state12.var_11)) + (0x4623bfa5));
                rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_12);
                rb_state12.var_16 = (rb_state12.var_16) - (rb_state12.var_13);
                rb_state12.var_17 = (rb_state12.var_17) - (((rb_state12.var_14) == (0xa9aefdaa)) ? (rb_state12.var_15) : (0x40ca0311));
                rb_state12.var_18 = (rb_state12.var_18) - (((0x128210af) + (0xd04238e7)) + (rb_state12.var_16));
                rb_state12.var_19 = (rb_state12.var_19) - (rb_state12.var_17);
                rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_18) == (0xc3c82878)) ? (rb_state12.var_19) : (0x987a2298));
                rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0xe869561b)) {
                pthread_mutex_lock(&(rb_state13.lock_18));
                rb_state13.var_4 = rb_state13.var_2;
                pthread_mutex_unlock(&(rb_state13.lock_18));
            }
            if ((rb_state13.var_1) == (0xe869561b)) {
                pthread_mutex_lock(&(rb_state13.lock_18));
                if (!((rb_state13.var_2) == (rb_state13.var_4))) {
                    racebench_trigger(13);
                }
                pthread_mutex_unlock(&(rb_state13.lock_18));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_3);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_3) == (0x84)) ? (rb_state17.var_3) : (0xacbb765e));
            #endif
            attr = NULL;
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_0) == (0xb62f455c)) ? (rb_state1.var_2) : (0x7cc4f45e));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) + (0xb25d4cd0);
            #endif
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_4 = (rb_state4.var_4) + (((rb_state4.var_1) == (0xffffffd3)) ? (rb_state4.var_0) : (0xf3b9bb64));
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_3 = (rb_state15.var_3) + (((rb_state15.var_1) == (0xf358569b)) ? (rb_state15.var_0) : (0x3747944c));
            rb_state15.var_2 = (rb_state15.var_2) - (0x77d43565);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) ^ (564 < rb_input_size ? (uint32_t)rb_input[564] : 0xaf88b9f7);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) ^ (1885 < rb_input_size ? (uint32_t)rb_input[1885] : 0x5e04900d);
            #endif
            arg = NULL;
        } else {
            arg = opts[i].arg;
        }

        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x10bf18d)) {
            rb_state0.var_5 = 0xeafb12e5;
            rb_state0.var_6 = (rb_state0.var_6) - (273 < rb_input_size ? (uint32_t)rb_input[273] : 0x5161d233);
            rb_state0.var_7 = (rb_state0.var_7) - (rb_state0.var_4);
            rb_state0.var_8 = (rb_state0.var_8) ^ (rb_state0.var_5);
            rb_state0.var_9 = (rb_state0.var_9) - (((((0x27b59863) + (rb_state0.var_7)) ^ (rb_state0.var_5)) ^ (rb_state0.var_6)) ^ (0xc886f34f));
            rb_state0.var_10 = (rb_state0.var_10) ^ (rb_state0.var_8);
            rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_9) : (0x46b2ab23));
            rb_state0.var_12 = (rb_state0.var_12) + (rb_state0.var_10);
            rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_12) : (0x99edc386));
            rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_1);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (886 < rb_input_size ? (uint32_t)rb_input[886] : 0x2668fb7f);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_2);
        if ((rb_state9.var_3) == (0xe6675a41)) {
            rb_state9.var_17 = 0x48d0a05b;
            rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_11) : (0x4a363ca2));
            rb_state9.var_19 = (rb_state9.var_19) + (((0x97795549) + (rb_state9.var_10)) ^ (0xcbe5b425));
            rb_state9.var_20 = (rb_state9.var_20) ^ (((0xb4cc3e6e) - (rb_state9.var_13)) + (rb_state9.var_17));
            rb_state9.var_21 = (rb_state9.var_21) ^ (rb_state9.var_18);
            rb_state9.var_22 = (rb_state9.var_22) + (rb_state9.var_19);
            rb_state9.var_23 = (rb_state9.var_23) ^ (((0x3c66baac) ^ (rb_state9.var_20)) - (0x6c37afa));
            rb_state9.var_24 = (rb_state9.var_24) - (rb_state9.var_21);
            rb_state9.var_25 = (rb_state9.var_25) ^ (((((0xfce10dc) ^ (rb_state9.var_22)) + (rb_state9.var_14)) + (rb_state9.var_15)) - (rb_state9.var_23));
            rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0xb14fad75));
            rb_state9.var_16 = (rb_state9.var_16) + (rb_state9.var_26);
            rb_state9.var_4 = rb_state9.var_16;
        }
        if ((rb_state9.var_3) == (0xe6675a41)) {
            pthread_mutex_lock(&(rb_state9.lock_37));
            rb_state9.var_28 = 0x2122d51f;
            rb_state9.var_29 = 0x8c22fe7f;
            rb_state9.var_30 = 0x168157d;
            rb_state9.var_31 = (rb_state9.var_31) ^ (rb_state9.var_28);
            rb_state9.var_32 = (rb_state9.var_32) - (((((0x7e5a8f78) + (rb_state9.var_16)) - (rb_state9.var_30)) - (0x7b83ebf6)) ^ (rb_state9.var_29));
            rb_state9.var_33 = (rb_state9.var_33) ^ (((rb_state9.var_17) == (0x48d0a05b)) ? (rb_state9.var_31) : (0xc5274854));
            rb_state9.var_34 = (rb_state9.var_34) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_32) : (0xc9cfd296));
            rb_state9.var_35 = (rb_state9.var_35) - (((rb_state9.var_19) == (0x5c9ce16c)) ? (rb_state9.var_33) : (0x4129e2d8));
            rb_state9.var_36 = (rb_state9.var_36) - (((((0x737bba8) - (rb_state9.var_35)) ^ (rb_state9.var_34)) + (rb_state9.var_21)) ^ (rb_state9.var_20));
            rb_state9.var_27 = (rb_state9.var_27) + (rb_state9.var_36);
            rb_state9.var_4 = rb_state9.var_27;
            pthread_mutex_unlock(&(rb_state9.lock_37));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_3) == (0x5fe3ad72)) {
            if ((rb_state15.var_4) != (0x0)) {
                if (!((rb_state15.var_4) == (rb_state15.var_34))) {
                    racebench_trigger(15);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (249 < rb_input_size ? (uint32_t)rb_input[249] : 0x7f45680a);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_0);
        #endif
        rv = pthread_create(&(pool->threads[i]), attr, opts[i].start_routine, arg);
        if (rv != 0) {
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xfeed04c8));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) - (1443 < rb_input_size ? (uint32_t)rb_input[1443] : 0x655389b0);
    rb_state3.var_3 = (rb_state3.var_3) + (0xa84a2db3);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_3) == (0xe05af443)) {
        rb_state4.var_6 = 0x6ff3b1a;
        rb_state4.var_7 = (rb_state4.var_7) - (rb_state4.var_5);
        rb_state4.var_8 = (rb_state4.var_8) - (rb_state4.var_6);
        rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_7);
        rb_state4.var_10 = (rb_state4.var_10) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_6) : (0xd71efd19));
        rb_state4.var_11 = (rb_state4.var_11) ^ (((0x6e7fd2ff) - (rb_state4.var_7)) - (0x2c831a03));
        rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0x36032d68));
        rb_state4.var_13 = (rb_state4.var_13) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0x7f15c42f));
        rb_state4.var_14 = (rb_state4.var_14) ^ (((((0x8dc699a9) - (rb_state4.var_12)) ^ (0xed2dbb98)) ^ (rb_state4.var_10)) - (rb_state4.var_11));
        rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_13) : (0xa6120a23));
        rb_state4.var_16 = (rb_state4.var_16) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_14) : (0xc93cf0b4));
        rb_state4.var_17 = (rb_state4.var_17) ^ (((((0xaa1a71db) - (0x16379e68)) + (rb_state4.var_16)) - (0x5055789b)) ^ (rb_state4.var_15));
        rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x8c036063));
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_1) : (0xfba67fbe));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x1b5c796d);
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (0x57eb0def);
    rb_state11.var_2 = (rb_state11.var_2) ^ ((0xe56b4569) - (rb_state11.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (1799 < rb_input_size ? (uint32_t)rb_input[1799] : 0x7d367149);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (229 < rb_input_size ? (uint32_t)rb_input[229] : 0xb8d44db8);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_1);
    #endif
    pool->nthreads = nthreads;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0x6bb642bc);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (977 < rb_input_size ? (uint32_t)rb_input[977] : 0xa28af9d0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (525 < rb_input_size ? (uint32_t)rb_input[525] : 0x2feee769);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (0x9f5fb3c7);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (1853 < rb_input_size ? (uint32_t)rb_input[1853] : 0x856c9618);
    if ((rb_state7.var_3) == (0x0)) {
        rb_state7.var_12 = 0x41e73a9a;
        rb_state7.var_13 = 0x4a492627;
        rb_state7.var_14 = (rb_state7.var_14) ^ (0x68e95287);
        rb_state7.var_15 = (rb_state7.var_15) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_8) : (0xa19a0da1));
        rb_state7.var_16 = (rb_state7.var_16) + (rb_state7.var_12);
        rb_state7.var_17 = (rb_state7.var_17) ^ (((0x2864eaff) + (0x3b95fcfc)) ^ (rb_state7.var_13));
        rb_state7.var_18 = (rb_state7.var_18) ^ (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_14) : (0x583d2b49));
        rb_state7.var_19 = (rb_state7.var_19) + (((0x9751785f) ^ (rb_state7.var_10)) ^ (rb_state7.var_15));
        rb_state7.var_20 = (rb_state7.var_20) - (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_16) : (0x2dd545b7));
        rb_state7.var_21 = (rb_state7.var_21) ^ (((0x5d39db93) ^ (rb_state7.var_17)) + (0x6c116c7e));
        rb_state7.var_22 = (rb_state7.var_22) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_19) : (0xed6d0551));
        rb_state7.var_23 = (rb_state7.var_23) ^ (rb_state7.var_20);
        rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_21) : (0xc0cb7efe));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((0xd74caee0) - (rb_state7.var_22)) + (0xa41073f2));
        rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_23) : (0x13a1a2d6));
        rb_state7.var_27 = (rb_state7.var_27) + (((0xf34d40fb) + (rb_state7.var_24)) - (0x35f2cebf));
        rb_state7.var_28 = (rb_state7.var_28) - (((rb_state7.var_25) == (0x0)) ? (rb_state7.var_26) : (0xd1a67d20));
        rb_state7.var_29 = (rb_state7.var_29) ^ (((((0x933017b8) ^ (0x62600cdd)) + (rb_state7.var_27)) + (0x6f2a56ca)) + (rb_state7.var_28));
        rb_state7.var_11 = (rb_state7.var_11) + (rb_state7.var_29);
        rb_state7.var_4 = rb_state7.var_11;
    }
    if ((rb_state7.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_35));
        rb_state7.var_31 = 0x69c2b2d9;
        rb_state7.var_32 = (rb_state7.var_32) ^ (rb_state7.var_31);
        rb_state7.var_33 = (rb_state7.var_33) + (((((0x99e419b4) + (rb_state7.var_16)) ^ (rb_state7.var_14)) ^ (0xa3397962)) - (rb_state7.var_15));
        rb_state7.var_34 = (rb_state7.var_34) ^ (((rb_state7.var_32) == (0x0)) ? (rb_state7.var_33) : (0x6fba55cc));
        rb_state7.var_30 = (rb_state7.var_30) ^ (rb_state7.var_34);
        rb_state7.var_4 = rb_state7.var_30;
        pthread_mutex_unlock(&(rb_state7.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_0);
    rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0x9377fec6) - (0x3694af98));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (0xeca800a2);
    rb_state18.var_2 = (rb_state18.var_2) - (912 < rb_input_size ? (uint32_t)rb_input[912] : 0x9144d789);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_4 = (rb_state19.var_4) ^ (2024 < rb_input_size ? (uint32_t)rb_input[2024] : 0x84263509);
    #endif
    pool->state = POOL_STATE_RUNNING;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0xfef40e7d) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (0x432aebf9);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0xfd0e3c17)) {
        rb_state3.var_5 = 0x2914142c;
        rb_state3.var_6 = (rb_state3.var_6) ^ (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_6) : (0x9d4271c8));
        rb_state3.var_7 = (rb_state3.var_7) ^ (((((0x5a52ffb7) - (rb_state3.var_5)) ^ (0xcd53a350)) + (rb_state3.var_4)) + (0x33650e5c));
        rb_state3.var_8 = (rb_state3.var_8) - (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xdb9d01d7));
        rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_3);
    rb_state6.var_2 = (rb_state6.var_2) - (1767 < rb_input_size ? (uint32_t)rb_input[1767] : 0x8e8c87cb);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x1925cd70) - (0x7393c2f3));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
    rb_state8.var_12 = (rb_state8.var_12) ^ ((0xc8f93750) + (rb_state8.var_13));
    if ((rb_state8.var_15) == (0x0)) {
        rb_state8.var_16 = rb_state8.var_9;
    }
    if ((rb_state8.var_15) == (0x0)) {
        if (!((rb_state8.var_9) == (rb_state8.var_16))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) + (1815 < rb_input_size ? (uint32_t)rb_input[1815] : 0xe1d63041);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (379 < rb_input_size ? (uint32_t)rb_input[379] : 0xce86643);
    rb_state14.var_3 = (rb_state14.var_3) - (261 < rb_input_size ? (uint32_t)rb_input[261] : 0x46a9895b);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + ((0x165f8100) ^ (0xaa997da0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_2) : (0x9fb88a2e));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_3);
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
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x153922c4)) {
        if ((rb_state1.var_9) != (0x0)) {
            if (!((rb_state1.var_9) == (rb_state1.var_16))) {
                racebench_trigger(1);
            }
        }
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