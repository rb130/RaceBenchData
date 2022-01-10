#include "racebench_bugs.h"


#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#define _THREAD_SAFE

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <errno.h>

#include "yarn.h"

#define local static

char *yarn_prefix = "yarn";
void (*yarn_abort)(int) = NULL;

local void fail(int err, char const *file, long line, char const *func) {
    fprintf(stderr, "%s: ", yarn_prefix);
    switch (err) {
    case EPERM:
        fputs("already unlocked", stderr);
        break;
    case ESRCH:
        fputs("no such thread", stderr);
        break;
    case EDEADLK:
        fputs("resource deadlock", stderr);
        break;
    case ENOMEM:
        fputs("out of memory", stderr);
        break;
    case EBUSY:
        fputs("can't destroy locked resource", stderr);
        break;
    case EINVAL:
        fputs("invalid request", stderr);
        break;
    case EAGAIN:
        fputs("resource unavailable", stderr);
        break;
    default:
        fprintf(stderr, "internal error %d", err);
    }
    fprintf(stderr, " (%s:%ld:%s)\n", file, line, func);
    if (yarn_abort != NULL) {
        yarn_abort(err);
    }
    exit(err);
}

typedef void *(*malloc_t)(size_t);
typedef void (*free_t)(void *);
local malloc_t my_malloc_f = malloc;
local free_t my_free = free;

void yarn_mem(malloc_t lease, free_t vacate) {
    my_malloc_f = lease;
    my_free = vacate;
}

local void *my_malloc(size_t size, char const *file, long line) {
    void *block;

    if ((block = my_malloc_f(size)) == NULL) {
        fail(ENOMEM, file, line, "malloc");
    }
    return block;
}

struct lock_s {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    long value;
};

lock *new_lock_(long initial, char const *file, long line) {
    lock *bolt = my_malloc(sizeof(struct lock_s), file, line);
    int ret = pthread_mutex_init(&(bolt->mutex), NULL);
    if (ret) {
        fail(ret, file, line, "mutex_init");
    }
    ret = pthread_cond_init(&(bolt->cond), NULL);
    if (ret) {
        fail(ret, file, line, "cond_init");
    }
    bolt->value = initial;
    return bolt;
}

void possess_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - (2352 < rb_input_size ? (uint32_t)rb_input[2352] : 0xe2ad1db9);
    rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_3);
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0xd0ba97b9)) {
        rb_state1.var_8 = (rb_state1.var_8) + (((((0xe1acc90a) ^ (0xe67bfee6)) - (rb_state1.var_5)) + (rb_state1.var_7)) - (0xb03c9927));
        rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_6) : (0x2147dd73));
        rb_state1.var_10 = (rb_state1.var_10) ^ (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_9) : (0xbaefa690));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((0x375eed72) + (rb_state1.var_8)) ^ (rb_state1.var_11));
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) ^ (((0xa550118b) ^ (rb_state1.var_10)) ^ (rb_state1.var_12));
        rb_state1.var_14 = (rb_state1.var_14) ^ (rb_state1.var_11);
        rb_state1.var_15 = (rb_state1.var_15) - (rb_state1.var_12);
        rb_state1.var_16 = (rb_state1.var_16) - (((((0x3477e12c) - (rb_state1.var_13)) - (0x633752c9)) - (rb_state1.var_14)) ^ (rb_state1.var_13));
        rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_15) : (0x3039cedb));
        rb_state1.var_18 = (rb_state1.var_18) ^ (((((0x16cdc515) ^ (rb_state1.var_16)) + (rb_state1.var_17)) ^ (0xcced00f8)) - (rb_state1.var_15));
        rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x51997841) - (0xea33ea70));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x27fb0158)) {
        pthread_mutex_lock(&(rb_state5.lock_28));
        rb_state5.var_6 = 0xe15a84e9;
        rb_state5.var_7 = 0xab1ce57;
        rb_state5.var_8 = 0x5b4cde7d;
        rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_8) == (0x5b4cde7d)) ? (rb_state5.var_7) : (0x859766f0));
        rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_6) == (0xe15a84e9)) ? (rb_state5.var_9) : (0x640b0a68));
        rb_state5.var_5 = (rb_state5.var_5) - (rb_state5.var_10);
        rb_state5.var_4 = rb_state5.var_5;
        pthread_mutex_unlock(&(rb_state5.lock_28));
    }
    if ((rb_state5.var_0) == (0x27fb0158)) {
        if (!((rb_state5.var_4) == (rb_state5.var_5))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_14) : (0xa0147240));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_3) : (0x98dcb931));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_0) == (0x1b99133f)) ? (rb_state14.var_2) : (0x9dea8303));
    if ((rb_state14.var_0) == (0x1b9913c1)) {
        rb_state14.var_3 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0x1b9913c1)) {
        if (!((rb_state14.var_1) == (rb_state14.var_3))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) - (2560 < rb_input_size ? (uint32_t)rb_input[2560] : 0xd9937d92);
    #endif
    int ret = pthread_mutex_lock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_lock");
    }
}

void release_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ ((0xf79ef095) + (0x1762076b));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (3602 < rb_input_size ? (uint32_t)rb_input[3602] : 0x223bff04);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_2);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + ((0xedc9b73d) ^ (0xc6e2c055));
    if ((rb_state17.var_3) == (0xca662cc2)) {
        rb_state17.var_4 = rb_state17.var_0;
    }
    if ((rb_state17.var_3) == (0xca662cc2)) {
        if (!((rb_state17.var_0) == (rb_state17.var_4))) {
            racebench_trigger(17);
        }
    }
    #endif
    int ret = pthread_mutex_unlock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_unlock");
    }
}

void twist_(lock *bolt, enum twist_op op, long val, char const *file, long line) {
    if (op == TO) {
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) - (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_0) : (0x9adce198));
        if ((rb_state11.var_3) == (0x2a)) {
            rb_state11.var_4 = rb_state11.var_0;
        }
        if ((rb_state11.var_3) == (0x2a)) {
            if (!((rb_state11.var_0) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        bolt->value = val;
    } else if (op == BY) {
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_3 = (rb_state6.var_3) + (823 < rb_input_size ? (uint32_t)rb_input[823] : 0x9ab1dc14);
        #endif
        bolt->value += val;
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0x9fa6f11c) + (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_1) == (0x68a4e6db)) ? (rb_state7.var_0) : (0x8d877cd3));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0xffffff01)) ? (rb_state9.var_1) : (0xeae279e8));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_2) : (0x5d321d3d));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_23 = (rb_state15.var_23) - (rb_state15.var_24);
    #endif
    int ret = pthread_cond_broadcast(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_broadcast");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + ((0x5a07ab77) ^ (0x1f25b587));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (3415 < rb_input_size ? (uint32_t)rb_input[3415] : 0x8f65cf0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (3571 < rb_input_size ? (uint32_t)rb_input[3571] : 0x642d297c);
    rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_0) == (0xd848bf27)) ? (rb_state6.var_3) : (0xefdc23b5));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + ((0xbbb297e4) ^ (rb_state11.var_0));
    #endif
    ret = pthread_mutex_unlock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_unlock");
    }
}

#define until(a) while (!(a))

void wait_for_(lock *bolt, enum wait_op op, long val, char const *file, long line) {
    switch (op) {
    case TO_BE:
        until(bolt->value == val) {
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_21) == (0x1)) {
                rb_state3.var_33 = 0xf1a4b7bb;
                rb_state3.var_34 = 0x60cd7858;
                rb_state3.var_35 = (rb_state3.var_35) - (((((0x2619880a) - (rb_state3.var_27)) ^ (rb_state3.var_34)) ^ (rb_state3.var_28)) + (0xf132d9b7));
                rb_state3.var_36 = (rb_state3.var_36) ^ (((0x55126e59) + (rb_state3.var_29)) ^ (rb_state3.var_33));
                rb_state3.var_37 = (rb_state3.var_37) ^ (((0x283d1d37) ^ (rb_state3.var_30)) + (rb_state3.var_35));
                rb_state3.var_38 = (rb_state3.var_38) - (rb_state3.var_36);
                rb_state3.var_39 = (rb_state3.var_39) ^ (((0xdc4ba839) ^ (rb_state3.var_37)) - (rb_state3.var_31));
                rb_state3.var_40 = (rb_state3.var_40) + (((0xfe11845b) + (rb_state3.var_38)) ^ (rb_state3.var_32));
                rb_state3.var_41 = (rb_state3.var_41) ^ (rb_state3.var_39);
                rb_state3.var_42 = (rb_state3.var_42) ^ (((0xc083d3aa) + (rb_state3.var_40)) + (rb_state3.var_33));
                rb_state3.var_43 = (rb_state3.var_43) + (rb_state3.var_41);
                rb_state3.var_44 = (rb_state3.var_44) + (((rb_state3.var_34) == (0x0)) ? (rb_state3.var_42) : (0xe5084ea6));
                rb_state3.var_45 = (rb_state3.var_45) - (((0x4eee797f) - (rb_state3.var_43)) + (rb_state3.var_35));
                rb_state3.var_46 = (rb_state3.var_46) - (((rb_state3.var_36) == (0x0)) ? (rb_state3.var_44) : (0x6c6250));
                rb_state3.var_47 = (rb_state3.var_47) + (rb_state3.var_45);
                rb_state3.var_48 = (rb_state3.var_48) - (((0x5bb66fe1) ^ (0xada3932e)) - (rb_state3.var_46));
                rb_state3.var_49 = (rb_state3.var_49) - (rb_state3.var_47);
                rb_state3.var_50 = (rb_state3.var_50) - (((rb_state3.var_37) == (0x0)) ? (rb_state3.var_48) : (0xc4ab40e6));
                rb_state3.var_51 = (rb_state3.var_51) + (rb_state3.var_49);
                rb_state3.var_52 = (rb_state3.var_52) - (((rb_state3.var_38) == (0x0)) ? (rb_state3.var_50) : (0x1d3a3b5a));
                rb_state3.var_53 = (rb_state3.var_53) + (((((0x98276be8) - (rb_state3.var_52)) + (0x97dca9b2)) - (rb_state3.var_39)) - (rb_state3.var_51));
                rb_state3.var_32 = (rb_state3.var_32) ^ (rb_state3.var_53);
                rb_state3.var_22 = rb_state3.var_32;
            }
            if ((rb_state3.var_21) == (0x1)) {
                pthread_mutex_lock(&(rb_state3.lock_67));
                rb_state3.var_55 = 0x9c23a598;
                rb_state3.var_56 = 0xa1974156;
                rb_state3.var_57 = 0x97cb4137;
                rb_state3.var_58 = (rb_state3.var_58) ^ (((rb_state3.var_41) == (0x0)) ? (rb_state3.var_40) : (0x5454a857));
                rb_state3.var_59 = (rb_state3.var_59) - (((0xaee4694f) ^ (0x5724a012)) + (rb_state3.var_56));
                rb_state3.var_60 = (rb_state3.var_60) ^ (((0x2b98dfcd) ^ (rb_state3.var_57)) - (0x73513a1b));
                rb_state3.var_61 = (rb_state3.var_61) + (((rb_state3.var_42) == (0x0)) ? (rb_state3.var_55) : (0x31920fbe));
                rb_state3.var_62 = (rb_state3.var_62) - (((((0xdcb710cf) + (rb_state3.var_59)) ^ (rb_state3.var_58)) ^ (rb_state3.var_43)) + (0x4c71af97));
                rb_state3.var_63 = (rb_state3.var_63) + (((rb_state3.var_60) == (0x0)) ? (rb_state3.var_61) : (0x6685fbc9));
                rb_state3.var_64 = (rb_state3.var_64) + (rb_state3.var_62);
                rb_state3.var_65 = (rb_state3.var_65) + (((0xe7a490ac) - (rb_state3.var_44)) ^ (rb_state3.var_63));
                rb_state3.var_66 = (rb_state3.var_66) ^ (((rb_state3.var_64) == (0x0)) ? (rb_state3.var_65) : (0xeb06f4fc));
                rb_state3.var_54 = (rb_state3.var_54) + (rb_state3.var_66);
                rb_state3.var_22 = rb_state3.var_54;
                pthread_mutex_unlock(&(rb_state3.lock_67));
            }
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0x4ea76e05));
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ ((0xf491b541) - (0x6dcf7c93));
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_0) == (0x1b9913c1)) {
                pthread_mutex_lock(&(rb_state14.lock_15));
                rb_state14.var_4 = 0xaea29b16;
                rb_state14.var_5 = 0x5a4215e3;
                rb_state14.var_6 = 0x655d822;
                rb_state14.var_7 = (rb_state14.var_7) - (0x3363082f);
                rb_state14.var_8 = (rb_state14.var_8) - (((0xa5ef1398) ^ (rb_state14.var_6)) - (rb_state14.var_3));
                rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_4) : (0xe64fe05));
                rb_state14.var_10 = (rb_state14.var_10) - (((0x33c3598b) ^ (rb_state14.var_7)) - (0x1f0a945a));
                rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_8) : (0x5841293c));
                rb_state14.var_12 = (rb_state14.var_12) - (((0x3f35051f) - (rb_state14.var_5)) + (rb_state14.var_9));
                rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_11) : (0x49be8623));
                rb_state14.var_14 = (rb_state14.var_14) + (((((0xc62ec1e7) - (0x9503c52f)) - (rb_state14.var_6)) ^ (rb_state14.var_12)) ^ (rb_state14.var_13));
                rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_14);
                pthread_mutex_unlock(&(rb_state14.lock_15));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_29) == (0xe6892387)) {
                pthread_mutex_lock(&(rb_state15.lock_83));
                if ((rb_state15.var_38) != (0x0)) {
                    if (!((rb_state15.var_38) == (rb_state15.var_76))) {
                        racebench_trigger(15);
                    }
                }
                pthread_mutex_unlock(&(rb_state15.lock_83));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_2);
            #endif
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        break;
    case NOT_TO_BE:
        until(bolt->value != val) {
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        break;
    case TO_BE_MORE_THAN:
        until(bolt->value > val) {
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        break;
    case TO_BE_LESS_THAN:
        until(bolt->value < val) {
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
    }
}

long peek_lock(lock *bolt) {
    return bolt->value;
}

void free_lock_(lock *bolt, char const *file, long line) {
    if (bolt == NULL) {
        return;
    }
    int ret = pthread_cond_destroy(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_destroy");
    }
    ret = pthread_mutex_destroy(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_destroy");
    }
    my_free(bolt);
}

struct thread_s {
    pthread_t id;
    int done;
    thread *next;
};

local lock threads_lock = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};
local thread *threads = NULL;

struct capsule {
    void (*probe)(void *);
    void *payload;
    char const *file;
    long line;
};

local void reenter(void *arg) {
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_3);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_25 = (rb_state15.var_25) - (((rb_state15.var_27) == (0xe7c8edcf)) ? (rb_state15.var_26) : (0x81340690));
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_1) == (0x62c6fcf9)) ? (rb_state1.var_0) : (0xf59d2057));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (2712 < rb_input_size ? (uint32_t)rb_input[2712] : 0xab8cd619);
    #endif
    pthread_t me = pthread_self();
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) ^ ((0xd2b88bbe) + (0x85bb1b04));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_2);
    #endif
    possess_(&(threads_lock), capsule->file, capsule->line);
    thread **prior = &(threads);
    thread *match;
    while ((match = *prior) != NULL) {
        if (pthread_equal(match->id, me)) {
            break;
        }
        prior = &(match->next);
    }
    if (match == NULL) {
        fail(ESRCH, capsule->file, capsule->line, "reenter lost");
    }

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_1);
    #endif
    match->done = 1;
    if (threads != match) {
        *prior = match->next;
        match->next = threads;
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_3) == (0x2a)) {
            rb_state11.var_5 = 0x69851bd4;
            rb_state11.var_6 = (rb_state11.var_6) + (((rb_state11.var_4) == (0xbc4297e4)) ? (rb_state11.var_5) : (0xb6bb5052));
            rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_6);
        }
        #endif
        threads = match;
    }

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (3838 < rb_input_size ? (uint32_t)rb_input[3838] : 0xffb98763);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_3);
    #endif
    twist_(&(threads_lock), BY, +1, capsule->file, capsule->line);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (3075 < rb_input_size ? (uint32_t)rb_input[3075] : 0xa47addb6);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + ((0xf1a5c3ff) ^ (0x9e883366));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (2866 < rb_input_size ? (uint32_t)rb_input[2866] : 0xc5920857);
    #endif
    my_free(capsule);
}

local void *ignition(void *arg) {
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) + (1840 < rb_input_size ? (uint32_t)rb_input[1840] : 0x4baf126d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0x23dda6f6);
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0xdc22590a)) ? (rb_state3.var_0) : (0x86904a44));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x9512b5a7) + (rb_state5.var_0));
    rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_2);
    if ((rb_state10.var_3) == (0x18)) {
        rb_state10.var_5 = (rb_state10.var_5) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_5) : (0x677f7c8a));
        rb_state10.var_6 = (rb_state10.var_6) ^ (4040 < rb_input_size ? (uint32_t)rb_input[4040] : 0xcb0325b);
        rb_state10.var_7 = (rb_state10.var_7) - (0x4c411e97);
        rb_state10.var_8 = (rb_state10.var_8) ^ ((0x47ae197f) - (0xe9c1bdd2));
        rb_state10.var_9 = (rb_state10.var_9) + (((((0xf85a4ecb) + (0xfd5437c7)) ^ (rb_state10.var_1)) - (rb_state10.var_4)) + (rb_state10.var_6));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((0xc5625fa6) + (rb_state10.var_5)) - (0x7e901c8f));
        rb_state10.var_11 = (rb_state10.var_11) + (rb_state10.var_6);
        rb_state10.var_12 = (rb_state10.var_12) + (((((0x2879d4f8) - (rb_state10.var_8)) ^ (rb_state10.var_8)) - (rb_state10.var_7)) + (rb_state10.var_7));
        rb_state10.var_13 = (rb_state10.var_13) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_9) : (0x7627a898));
        rb_state10.var_14 = (rb_state10.var_14) ^ (((0x8f208c2f) - (rb_state10.var_10)) ^ (0x20b5b8f9));
        rb_state10.var_15 = (rb_state10.var_15) ^ (((0x76f8288c) - (0x104884e3)) - (rb_state10.var_11));
        rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0xea6bd1bd));
        rb_state10.var_17 = (rb_state10.var_17) - (rb_state10.var_14);
        rb_state10.var_18 = (rb_state10.var_18) + (((((0x3b530722) ^ (rb_state10.var_16)) + (0xb572b940)) ^ (rb_state10.var_10)) + (rb_state10.var_15));
        rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_17) : (0xb423fa40));
        rb_state10.var_20 = (rb_state10.var_20) - (rb_state10.var_18);
        rb_state10.var_21 = (rb_state10.var_21) + (rb_state10.var_19);
        rb_state10.var_22 = (rb_state10.var_22) + (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_20) : (0xa6543f17));
        rb_state10.var_23 = (rb_state10.var_23) + (((0xa6671e7) - (0xb05cca52)) ^ (rb_state10.var_21));
        rb_state10.var_24 = (rb_state10.var_24) - (((0x539e335b) + (0xb1c7b612)) + (rb_state10.var_22));
        rb_state10.var_25 = (rb_state10.var_25) + (((0x9009ef68) + (0xcdca0f42)) - (rb_state10.var_23));
        rb_state10.var_26 = (rb_state10.var_26) - (((((0x1c0a91a0) ^ (rb_state10.var_25)) ^ (0x9ec7448a)) ^ (rb_state10.var_24)) + (rb_state10.var_13));
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x0)) {
        rb_state12.var_5 = 0x3a131d40;
        rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_4) : (0x8cd90fc4));
        rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_6) : (0x33fbf8ae));
        rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x7a49b3fd));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0xe466ecc1) ^ (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0xb204b338)) ? (rb_state15.var_0) : (0x1218559f));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0x33846ca2);
    rb_state17.var_1 = (rb_state17.var_1) ^ ((0x8d2fcfb) - (rb_state17.var_0));
    rb_state17.var_2 = (rb_state17.var_2) + (0x8cf3ff4f);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (4036 < rb_input_size ? (uint32_t)rb_input[4036] : 0x249297eb);
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0x3f2c931));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_19 = (rb_state3.var_19) ^ (2319 < rb_input_size ? (uint32_t)rb_input[2319] : 0x5ed68a7);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (0x190b7728);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (456 < rb_input_size ? (uint32_t)rb_input[456] : 0x633b8240);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (3866 < rb_input_size ? (uint32_t)rb_input[3866] : 0xf689647);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - ((0xe0d7436b) ^ (rb_state17.var_1));
    #endif
    pthread_cleanup_push(reenter, arg);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0x7a39c0e9));
    if ((rb_state0.var_3) == (0x0)) {
        rb_state0.var_20 = 0x5e3aa263;
        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_14) : (0xeadef105));
        rb_state0.var_22 = (rb_state0.var_22) - (112 < rb_input_size ? (uint32_t)rb_input[112] : 0x830dd586);
        rb_state0.var_23 = (rb_state0.var_23) ^ (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_15) : (0x4810253));
        rb_state0.var_24 = (rb_state0.var_24) - (rb_state0.var_21);
        rb_state0.var_25 = (rb_state0.var_25) - (((0x8c441dd2) - (rb_state0.var_17)) + (rb_state0.var_22));
        rb_state0.var_26 = (rb_state0.var_26) + (((((0x79c1d3f4) ^ (rb_state0.var_23)) + (rb_state0.var_18)) - (rb_state0.var_24)) + (rb_state0.var_19));
        rb_state0.var_27 = (rb_state0.var_27) - (((0x88eef396) ^ (rb_state0.var_25)) + (0x2ddab95d));
        rb_state0.var_28 = (rb_state0.var_28) ^ (((((0x8f36ea0a) + (rb_state0.var_27)) ^ (rb_state0.var_26)) ^ (rb_state0.var_20)) - (0x4a7be63e));
        rb_state0.var_4 = (rb_state0.var_4) - (rb_state0.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_3);
    if ((rb_state4.var_1) == (0x11e619b)) {
        pthread_mutex_lock(&(rb_state4.lock_22));
        rb_state4.var_12 = 0xb6d30018;
        rb_state4.var_13 = 0xaff6c712;
        rb_state4.var_14 = (rb_state4.var_14) + (((0xb6ce5d40) - (rb_state4.var_9)) ^ (rb_state4.var_13));
        rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_10) : (0x5bb4bea1));
        rb_state4.var_16 = (rb_state4.var_16) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_12) : (0x5058c987));
        rb_state4.var_17 = (rb_state4.var_17) - (rb_state4.var_14);
        rb_state4.var_18 = (rb_state4.var_18) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0xf1ec19ef));
        rb_state4.var_19 = (rb_state4.var_19) + (((0x39577c6a) - (rb_state4.var_17)) + (0x6152aa0f));
        rb_state4.var_20 = (rb_state4.var_20) ^ (((0x41fb6ac) - (rb_state4.var_18)) + (rb_state4.var_12));
        rb_state4.var_21 = (rb_state4.var_21) - (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_20) : (0xfba72df7));
        rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_21);
        pthread_mutex_unlock(&(rb_state4.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xd848c01e)) {
        rb_state6.var_5 = 0xc415dcc6;
        rb_state6.var_6 = 0xcea1b46b;
        rb_state6.var_7 = (rb_state6.var_7) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0xbca4d1a3));
        rb_state6.var_8 = (rb_state6.var_8) + (((0x50b86927) - (0x56770568)) + (rb_state6.var_4));
        rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0xc3f1bcb3));
        rb_state6.var_10 = (rb_state6.var_10) - (((((0x131c1f0b) ^ (0x4ec75e62)) + (rb_state6.var_8)) - (0xf3a533b4)) - (rb_state6.var_7));
        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_9) : (0xccdc5bc0));
        rb_state6.var_12 = (rb_state6.var_12) ^ (((0x16bdd8cb) ^ (rb_state6.var_10)) ^ (rb_state6.var_8));
        rb_state6.var_13 = (rb_state6.var_13) - (((0xb5b54f00) + (rb_state6.var_9)) ^ (rb_state6.var_11));
        rb_state6.var_14 = (rb_state6.var_14) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0x1579dc5b));
        rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (2822 < rb_input_size ? (uint32_t)rb_input[2822] : 0x7ab17cd4);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (0x77909f68);
    rb_state13.var_2 = (rb_state13.var_2) ^ (3916 < rb_input_size ? (uint32_t)rb_input[3916] : 0x6ca019a9);
    rb_state13.var_3 = (rb_state13.var_3) + (972 < rb_input_size ? (uint32_t)rb_input[972] : 0xdf343417);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (58 < rb_input_size ? (uint32_t)rb_input[58] : 0x7d6aaf1b);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + ((0x9d31479d) ^ (0xa575a868));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0x866d1c7f));
    rb_state18.var_2 = (rb_state18.var_2) - ((0xf5a31636) - (rb_state18.var_2));
    rb_state18.var_0 = (rb_state18.var_0) - (0x9651a482);
    #endif
    capsule->probe(capsule->payload);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (33 < rb_input_size ? (uint32_t)rb_input[33] : 0x3233aca5);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) - (1434 < rb_input_size ? (uint32_t)rb_input[1434] : 0x6b0aae8b);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (3037 < rb_input_size ? (uint32_t)rb_input[3037] : 0x4f260486);
    #endif
    pthread_cleanup_pop(1);

    return NULL;
}

thread *launch_(void (*probe)(void *), void *payload, char const *file, long line) {

    struct capsule *capsule = my_malloc(sizeof(struct capsule), file, line);
    capsule->probe = probe;
    capsule->payload = payload;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (882 < rb_input_size ? (uint32_t)rb_input[882] : 0x87999c63);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_29) == (0xe6892387)) {
        rb_state15.var_56 = 0x151574cc;
        rb_state15.var_57 = 0xd1b1f36a;
        rb_state15.var_58 = 0xc54dbe65;
        rb_state15.var_59 = (rb_state15.var_59) ^ (2603 < rb_input_size ? (uint32_t)rb_input[2603] : 0x132c8995);
        rb_state15.var_60 = (rb_state15.var_60) ^ (rb_state15.var_57);
        rb_state15.var_61 = (rb_state15.var_61) ^ (((0x327fd02c) - (0x63b04c8e)) + (rb_state15.var_56));
        rb_state15.var_62 = (rb_state15.var_62) + (rb_state15.var_58);
        rb_state15.var_63 = (rb_state15.var_63) - (((rb_state15.var_39) == (0x0)) ? (rb_state15.var_59) : (0xc861d1c4));
        rb_state15.var_64 = (rb_state15.var_64) ^ (rb_state15.var_60);
        rb_state15.var_65 = (rb_state15.var_65) ^ (((rb_state15.var_40) == (0x0)) ? (rb_state15.var_61) : (0xa93e4add));
        rb_state15.var_66 = (rb_state15.var_66) - (rb_state15.var_62);
        rb_state15.var_67 = (rb_state15.var_67) ^ (((0xe2876823) + (0x86642040)) - (rb_state15.var_63));
        rb_state15.var_68 = (rb_state15.var_68) ^ (((0xeaa049bc) + (rb_state15.var_64)) + (rb_state15.var_41));
        rb_state15.var_69 = (rb_state15.var_69) + (((0x46d955ed) - (rb_state15.var_65)) ^ (0xe14a2d4));
        rb_state15.var_70 = (rb_state15.var_70) - (rb_state15.var_66);
        rb_state15.var_71 = (rb_state15.var_71) + (((((0x2aebbc1d) ^ (rb_state15.var_67)) ^ (rb_state15.var_68)) + (0x8f43acb7)) ^ (rb_state15.var_42));
        rb_state15.var_72 = (rb_state15.var_72) - (rb_state15.var_69);
        rb_state15.var_73 = (rb_state15.var_73) - (((((0xef093eec) ^ (0xbcadb34f)) - (0xfd58e1da)) ^ (rb_state15.var_70)) ^ (rb_state15.var_71));
        rb_state15.var_74 = (rb_state15.var_74) ^ (((0x42f1c9ee) ^ (0xddb402e0)) - (rb_state15.var_72));
        rb_state15.var_75 = (rb_state15.var_75) + (((((0xae6faf20) ^ (rb_state15.var_73)) + (rb_state15.var_74)) + (0x8ba5d800)) ^ (rb_state15.var_43));
        rb_state15.var_55 = (rb_state15.var_55) ^ (rb_state15.var_75);
        rb_state15.var_38 = rb_state15.var_55;
    }
    #endif
    capsule->file = file;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (1575 < rb_input_size ? (uint32_t)rb_input[1575] : 0xf5d1358);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_29) == (0xe6892387)) {
        pthread_mutex_lock(&(rb_state15.lock_83));
        rb_state15.var_77 = 0xc08e0e7a;
        rb_state15.var_78 = 0xfd7efe0a;
        rb_state15.var_79 = (rb_state15.var_79) ^ (((rb_state15.var_78) == (0xfd7efe0a)) ? (rb_state15.var_77) : (0x8d2e6896));
        rb_state15.var_80 = (rb_state15.var_80) ^ (rb_state15.var_44);
        rb_state15.var_81 = (rb_state15.var_81) ^ (((0x1c612fc1) + (0x179b7ff9)) ^ (rb_state15.var_79));
        rb_state15.var_82 = (rb_state15.var_82) - (((((0x9fc1fb43) - (rb_state15.var_80)) - (rb_state15.var_45)) - (rb_state15.var_81)) + (rb_state15.var_46));
        rb_state15.var_76 = (rb_state15.var_76) - (rb_state15.var_82);
        rb_state15.var_38 = rb_state15.var_76;
        pthread_mutex_unlock(&(rb_state15.lock_83));
    }
    #endif
    capsule->line = line;

    possess_(&(threads_lock), file, line);

    thread *th = my_malloc(sizeof(struct thread_s), file, line);
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret) {
        fail(ret, file, line, "attr_init");
    }
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (ret) {
        fail(ret, file, line, "attr_setdetachstate");
    }
    ret = pthread_create(&(th->id), &attr, ignition, capsule);
    if (ret) {
        fail(ret, file, line, "create");
    }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0xffd78256) - (0x9147e5f6));
    rb_state1.var_1 = (rb_state1.var_1) + (0x90444a9f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_7 = (rb_state8.var_7) + (127 < rb_input_size ? (uint32_t)rb_input[127] : 0xf7b1b4a5);
    rb_state8.var_8 = (rb_state8.var_8) - (2272 < rb_input_size ? (uint32_t)rb_input[2272] : 0xf94419aa);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_3) == (0x18)) {
        rb_state10.var_4 = rb_state10.var_0;
    }
    if ((rb_state10.var_3) == (0x18)) {
        if (!((rb_state10.var_0) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x0)) {
        rb_state12.var_4 = rb_state12.var_1;
    }
    if ((rb_state12.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_8));
        if (!((rb_state12.var_1) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
        pthread_mutex_unlock(&(rb_state12.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (0xb204b338);
    rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_0);
    rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_1);
    rb_state15.var_2 = (rb_state15.var_2) ^ (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_2) : (0x3cad066));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (691 < rb_input_size ? (uint32_t)rb_input[691] : 0xe0928b64);
    rb_state16.var_3 = (rb_state16.var_3) + (3334 < rb_input_size ? (uint32_t)rb_input[3334] : 0x8a0b15de);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) + (1846 < rb_input_size ? (uint32_t)rb_input[1846] : 0x8cfb55ee);
    #endif
    ret = pthread_attr_destroy(&attr);
    if (ret) {
        fail(ret, file, line, "attr_destroy");
    }

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_14 = (rb_state3.var_14) - ((0xf98b33e3) ^ (rb_state3.var_15));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state9.lock_12));
        rb_state9.var_4 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_12));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (0x22683236);
    rb_state13.var_3 = (rb_state13.var_3) + (247 < rb_input_size ? (uint32_t)rb_input[247] : 0xfe264422);
    rb_state13.var_2 = (rb_state13.var_2) ^ (rb_state13.var_3);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - (0xad785a16);
    if ((rb_state15.var_1) == (0x0)) {
        rb_state15.var_21 = (rb_state15.var_21) ^ (((((0xfcf10ed5) + (rb_state15.var_12)) + (rb_state15.var_14)) - (rb_state15.var_13)) + (rb_state15.var_10));
        rb_state15.var_22 = (rb_state15.var_22) - (0x944696bb);
        rb_state15.var_23 = (rb_state15.var_23) ^ ((0x53d0e5ee) ^ (rb_state15.var_15));
        rb_state15.var_24 = (rb_state15.var_24) + (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_21) : (0xf3841100));
        rb_state15.var_25 = (rb_state15.var_25) ^ (((rb_state15.var_22) == (0x6bb96945)) ? (rb_state15.var_23) : (0x6b7fcac4));
        rb_state15.var_26 = (rb_state15.var_26) - (((0x1b46035c) + (rb_state15.var_24)) - (rb_state15.var_16));
        rb_state15.var_27 = (rb_state15.var_27) ^ (((rb_state15.var_25) == (0x53d0e5ee)) ? (rb_state15.var_26) : (0x36503efb));
        rb_state15.var_20 = (rb_state15.var_20) + (rb_state15.var_27);
        rb_state15.var_4 = rb_state15.var_20;
    }
    if ((rb_state15.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_36));
        rb_state15.var_29 = 0xe6892387;
        rb_state15.var_30 = (rb_state15.var_30) ^ (((rb_state15.var_20) == (0xe7c8edcf)) ? (rb_state15.var_19) : (0xe5fce4ec));
        rb_state15.var_31 = (rb_state15.var_31) - ((0x6dba12e7) - (0x36e0d5e6));
        rb_state15.var_32 = (rb_state15.var_32) - (((rb_state15.var_18) == (0x0)) ? (rb_state15.var_17) : (0x2850cb0c));
        rb_state15.var_33 = (rb_state15.var_33) + (((((0x2ad2777e) - (rb_state15.var_21)) ^ (rb_state15.var_30)) ^ (0x1e5066cb)) ^ (rb_state15.var_29));
        rb_state15.var_34 = (rb_state15.var_34) + (((rb_state15.var_31) == (0xc926c2ff)) ? (rb_state15.var_32) : (0x21271ee4));
        rb_state15.var_35 = (rb_state15.var_35) - (((((0x7c005e56) + (rb_state15.var_33)) + (rb_state15.var_34)) + (0xe7b525d6)) ^ (0x582e58e4));
        rb_state15.var_28 = (rb_state15.var_28) + (rb_state15.var_35);
        rb_state15.var_4 = rb_state15.var_28;
        pthread_mutex_unlock(&(rb_state15.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (3207 < rb_input_size ? (uint32_t)rb_input[3207] : 0xcbc19214);
    rb_state16.var_1 = (rb_state16.var_1) - ((0x6ae6910e) + (0x90cceae2));
    if ((rb_state16.var_2) == (0x3a087373)) {
        if ((rb_state16.var_4) != (0x0)) {
            if (!((rb_state16.var_4) == (rb_state16.var_15))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    th->done = 0;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_29));
        rb_state0.var_6 = 0x4c41c34;
        rb_state0.var_7 = (rb_state0.var_7) ^ (2257 < rb_input_size ? (uint32_t)rb_input[2257] : 0xd7954cd4);
        rb_state0.var_8 = (rb_state0.var_8) ^ (((((0xbb0a49c7) - (0x8c03169)) ^ (0x72a0cd11)) ^ (rb_state0.var_5)) + (rb_state0.var_4));
        rb_state0.var_9 = (rb_state0.var_9) - (1148 < rb_input_size ? (uint32_t)rb_input[1148] : 0x15632283);
        rb_state0.var_10 = (rb_state0.var_10) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_6) : (0xbd14ce57));
        rb_state0.var_11 = (rb_state0.var_11) ^ (((0x1482d9ea) - (rb_state0.var_7)) + (rb_state0.var_7));
        rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_8) : (0xf85f6389));
        rb_state0.var_13 = (rb_state0.var_13) + (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_9) : (0x545ab472));
        rb_state0.var_14 = (rb_state0.var_14) ^ (((0x5d88a2b2) - (rb_state0.var_10)) - (rb_state0.var_10));
        rb_state0.var_15 = (rb_state0.var_15) - (((0x89cdcc35) ^ (rb_state0.var_11)) ^ (0x14deb54));
        rb_state0.var_16 = (rb_state0.var_16) ^ (((0xd0827dd9) + (rb_state0.var_12)) + (0x23e21cc8));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_14) : (0x5f201a9f));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((((0x2f071be7) ^ (0x74360305)) + (rb_state0.var_11)) - (rb_state0.var_15)) + (rb_state0.var_16));
        rb_state0.var_19 = (rb_state0.var_19) + (((((0x1ea74c6) - (rb_state0.var_12)) - (rb_state0.var_13)) ^ (rb_state0.var_18)) + (rb_state0.var_17));
        rb_state0.var_5 = (rb_state0.var_5) - (rb_state0.var_19);
        rb_state0.var_4 = rb_state0.var_5;
        pthread_mutex_unlock(&(rb_state0.lock_29));
    }
    if ((rb_state0.var_3) == (0x0)) {
        if (!((rb_state0.var_4) == (rb_state0.var_5))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x64872bd2));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (3056 < rb_input_size ? (uint32_t)rb_input[3056] : 0x45c62187);
    if ((rb_state4.var_1) == (0x11e619b)) {
        pthread_mutex_lock(&(rb_state4.lock_22));
        rb_state4.var_6 = 0xae2999fd;
        rb_state4.var_7 = 0x5e7e70f1;
        rb_state4.var_8 = (rb_state4.var_8) + (((((0x229abb03) - (rb_state4.var_5)) + (rb_state4.var_4)) + (0xf93e4179)) ^ (rb_state4.var_6));
        rb_state4.var_9 = (rb_state4.var_9) - (((0x1c47ff76) - (rb_state4.var_7)) - (0x40510990));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_8) : (0x95cb8c43));
        rb_state4.var_11 = (rb_state4.var_11) - (((((0xa4779e1b) + (rb_state4.var_10)) ^ (rb_state4.var_9)) - (0x189ca2ad)) - (rb_state4.var_7));
        rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_11);
        rb_state4.var_4 = rb_state4.var_5;
        pthread_mutex_unlock(&(rb_state4.lock_22));
    }
    if ((rb_state4.var_1) == (0x11e619b)) {
        if (!((rb_state4.var_4) == (rb_state4.var_5))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (0x6d17b44f);
    rb_state5.var_2 = (rb_state5.var_2) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_2) : (0xcfd730e4));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xd848c01e)) {
        rb_state6.var_4 = rb_state6.var_1;
    }
    if ((rb_state6.var_0) == (0xd848c01e)) {
        if (!((rb_state6.var_1) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_2);
    if ((rb_state7.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_34));
        rb_state7.var_20 = 0x9237dbe9;
        rb_state7.var_21 = 0xc58d73dc;
        rb_state7.var_22 = (rb_state7.var_22) + (((((0xa3a691cd) ^ (0xe8122a47)) ^ (0xecd02fab)) + (rb_state7.var_21)) - (rb_state7.var_20));
        rb_state7.var_23 = (rb_state7.var_23) - ((0x90c61bd1) ^ (rb_state7.var_11));
        rb_state7.var_24 = (rb_state7.var_24) + (((0x1759dd57) - (rb_state7.var_10)) ^ (0x650d9fcb));
        rb_state7.var_25 = (rb_state7.var_25) - (rb_state7.var_22);
        rb_state7.var_26 = (rb_state7.var_26) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_23) : (0x269cb908));
        rb_state7.var_27 = (rb_state7.var_27) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_24) : (0x8c7c50eb));
        rb_state7.var_28 = (rb_state7.var_28) + (rb_state7.var_25);
        rb_state7.var_29 = (rb_state7.var_29) ^ (((0xda1cd288) ^ (0x5776c98e)) ^ (rb_state7.var_26));
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_27) : (0x6fdb295));
        rb_state7.var_31 = (rb_state7.var_31) ^ (rb_state7.var_28);
        rb_state7.var_32 = (rb_state7.var_32) - (((rb_state7.var_29) == (0x0)) ? (rb_state7.var_30) : (0x79444e17));
        rb_state7.var_33 = (rb_state7.var_33) + (((((0x5d7ebc83) ^ (rb_state7.var_15)) ^ (rb_state7.var_32)) + (rb_state7.var_31)) ^ (0xa8cc9b09));
        rb_state7.var_3 = (rb_state7.var_3) ^ (rb_state7.var_33);
        pthread_mutex_unlock(&(rb_state7.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (0x7882a9e4);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x0)) {
        if (!((rb_state9.var_3) == (rb_state9.var_4))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - ((0xb558ff02) + (0x626c68da));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        rb_state13.var_22 = 0x2646552;
        rb_state13.var_23 = (rb_state13.var_23) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_10) : (0x8d9f9b71));
        rb_state13.var_24 = (rb_state13.var_24) - (rb_state13.var_22);
        rb_state13.var_25 = (rb_state13.var_25) ^ (((((0x16bdea40) - (rb_state13.var_23)) - (rb_state13.var_13)) + (0x4726acda)) ^ (rb_state13.var_11));
        rb_state13.var_26 = (rb_state13.var_26) + (((rb_state13.var_24) == (0xfd9b9aae)) ? (rb_state13.var_25) : (0x6b59f409));
        rb_state13.var_21 = (rb_state13.var_21) - (rb_state13.var_26);
        rb_state13.var_4 = rb_state13.var_21;
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_30 = (rb_state15.var_30) ^ (rb_state15.var_28);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (((rb_state16.var_1) == (0x44c8410)) ? (rb_state16.var_0) : (0xe77ad92e));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (0x18081eec);
    #endif
    th->next = threads;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0xd0ba97b9)) {
        pthread_mutex_lock(&(rb_state1.lock_19));
        rb_state1.var_5 = (rb_state1.var_5) - (rb_state1.var_3);
        rb_state1.var_6 = (rb_state1.var_6) - (((0xe3806f8f) - (0x76bb2e6f)) + (rb_state1.var_4));
        rb_state1.var_7 = (rb_state1.var_7) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_6) : (0x95758af));
        rb_state1.var_4 = (rb_state1.var_4) ^ (rb_state1.var_7);
        rb_state1.var_3 = rb_state1.var_4;
        pthread_mutex_unlock(&(rb_state1.lock_19));
    }
    if ((rb_state1.var_2) == (0xd0ba97b9)) {
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ ((0xd36025da) - (0x19137216));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (0xb0b35ec3);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) ^ (1825 < rb_input_size ? (uint32_t)rb_input[1825] : 0x9c5d9562);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (1118 < rb_input_size ? (uint32_t)rb_input[1118] : 0x40aa4c97);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0xb76e51dc)) {
        pthread_mutex_lock(&(rb_state19.lock_21));
        rb_state19.var_6 = 0xba13cc09;
        rb_state19.var_7 = (rb_state19.var_7) - (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_6) : (0x4b49d16e));
        rb_state19.var_8 = (rb_state19.var_8) ^ ((0x43172684) - (rb_state19.var_7));
        rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_5) : (0xc47563a));
        rb_state19.var_10 = (rb_state19.var_10) ^ (((((0xe131d62a) + (rb_state19.var_8)) ^ (rb_state19.var_9)) ^ (rb_state19.var_8)) + (rb_state19.var_7));
        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0x762f3da0));
        rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_11);
        rb_state19.var_4 = rb_state19.var_5;
        pthread_mutex_unlock(&(rb_state19.lock_21));
    }
    if ((rb_state19.var_0) == (0xb76e51dc)) {
        if (!((rb_state19.var_4) == (rb_state19.var_5))) {
            racebench_trigger(19);
        }
    }
    #endif
    threads = th;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (((rb_state2.var_1) == (0x20de3c52)) ? (rb_state2.var_2) : (0x5b044b13));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + ((0x161d632d) + (0xab9d527c));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) + (0xbd40d256);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0x190b7728)) {
        pthread_mutex_lock(&(rb_state8.lock_15));
        rb_state8.var_6 = 0x53d86801;
        rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0xefb35dda));
        rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_7) : (0xe4ae9d94));
        rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_8);
        rb_state8.var_4 = rb_state8.var_5;
        pthread_mutex_unlock(&(rb_state8.lock_15));
    }
    if ((rb_state8.var_2) == (0x190b7728)) {
        rb_state8.var_16 = !((rb_state8.var_4) == (rb_state8.var_5));
    }
    if ((rb_state8.var_4) == (0x9912bba6)) {
        rb_state8.var_26 = 0xddfebc73;
        rb_state8.var_27 = 0xc9095cb5;
        rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_24) == (0x0)) ? (rb_state8.var_26) : (0xcad9d34c));
        rb_state8.var_29 = (rb_state8.var_29) ^ (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_23) : (0xf98c7ec));
        rb_state8.var_30 = (rb_state8.var_30) - (((rb_state8.var_25) == (0x0)) ? (rb_state8.var_28) : (0x979cbf68));
        rb_state8.var_31 = (rb_state8.var_31) + (rb_state8.var_29);
        rb_state8.var_32 = (rb_state8.var_32) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_30) : (0x573b3bd9));
        rb_state8.var_33 = (rb_state8.var_33) ^ (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_31) : (0x5ad6182c));
        rb_state8.var_34 = (rb_state8.var_34) + (((rb_state8.var_28) == (0x0)) ? (rb_state8.var_32) : (0x77fc5c7d));
        rb_state8.var_35 = (rb_state8.var_35) + (((((0x3443de45) ^ (rb_state8.var_34)) - (0x41094335)) - (rb_state8.var_33)) - (rb_state8.var_29));
        rb_state8.var_17 = (rb_state8.var_17) ^ (rb_state8.var_35);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (1045 < rb_input_size ? (uint32_t)rb_input[1045] : 0x603f250c);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_38));
        rb_state13.var_28 = 0xb5ade7e4;
        rb_state13.var_29 = (rb_state13.var_29) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_28) : (0xed7677c4));
        rb_state13.var_30 = (rb_state13.var_30) ^ ((0xbccd16) + (0xabbb653c));
        rb_state13.var_31 = (rb_state13.var_31) ^ (((((0x409c5052) + (rb_state13.var_15)) + (rb_state13.var_29)) + (0x591659cc)) - (0xcd41956b));
        rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_30) : (0xf76bb4a5));
        rb_state13.var_33 = (rb_state13.var_33) - (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_31) : (0x3046c8fc));
        rb_state13.var_34 = (rb_state13.var_34) ^ (rb_state13.var_32);
        rb_state13.var_35 = (rb_state13.var_35) - (rb_state13.var_33);
        rb_state13.var_36 = (rb_state13.var_36) - (rb_state13.var_34);
        rb_state13.var_37 = (rb_state13.var_37) - (((((0x7c0fb073) ^ (rb_state13.var_18)) + (rb_state13.var_35)) + (0x38a769fb)) + (rb_state13.var_36));
        rb_state13.var_27 = (rb_state13.var_27) ^ (rb_state13.var_37);
        rb_state13.var_4 = rb_state13.var_27;
        pthread_mutex_unlock(&(rb_state13.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - (0xd96cfd55);
    rb_state18.var_3 = (rb_state18.var_3) + (((rb_state18.var_1) == (0xffffffdd)) ? (rb_state18.var_0) : (0xe077bf67));
    if ((rb_state18.var_2) == (0x30efec75)) {
        rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_8);
        rb_state18.var_9 = (rb_state18.var_9) + (((((0xd6d72d06) + (rb_state18.var_9)) ^ (rb_state18.var_5)) + (0x1fdaf66d)) - (rb_state18.var_4));
        rb_state18.var_10 = (rb_state18.var_10) + (((((0xb19c699b) ^ (rb_state18.var_10)) ^ (rb_state18.var_8)) ^ (rb_state18.var_7)) ^ (0xcf4012ea));
        rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_11) : (0x93c21cd9));
        rb_state18.var_12 = (rb_state18.var_12) + (((rb_state18.var_9) == (0xf6b22373)) ? (rb_state18.var_10) : (0x1b76b47a));
        rb_state18.var_13 = (rb_state18.var_13) - (rb_state18.var_11);
        rb_state18.var_14 = (rb_state18.var_14) + (((0x127d752e) + (rb_state18.var_12)) ^ (rb_state18.var_12));
        rb_state18.var_15 = (rb_state18.var_15) - (rb_state18.var_13);
        rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_14) : (0x2ee088c5));
        rb_state18.var_17 = (rb_state18.var_17) ^ (((0x80a58aa2) - (rb_state18.var_15)) + (0x7fc81aa7));
        rb_state18.var_18 = (rb_state18.var_18) ^ (rb_state18.var_16);
        rb_state18.var_19 = (rb_state18.var_19) ^ (rb_state18.var_17);
        rb_state18.var_20 = (rb_state18.var_20) + (rb_state18.var_18);
        rb_state18.var_21 = (rb_state18.var_21) + (rb_state18.var_19);
        rb_state18.var_22 = (rb_state18.var_22) ^ (((((0x55f4ba03) ^ (0xb8a68667)) ^ (rb_state18.var_21)) + (rb_state18.var_20)) ^ (0x5cdbf505));
        rb_state18.var_7 = (rb_state18.var_7) + (rb_state18.var_22);
        rb_state18.var_4 = rb_state18.var_7;
    }
    if ((rb_state18.var_2) == (0x30efec75)) {
        pthread_mutex_lock(&(rb_state18.lock_27));
        rb_state18.var_24 = 0x9aad4423;
        rb_state18.var_25 = (rb_state18.var_25) - (((((0x240782b8) - (rb_state18.var_16)) ^ (rb_state18.var_15)) + (0x94dd6a5f)) ^ (rb_state18.var_14));
        rb_state18.var_26 = (rb_state18.var_26) + (((rb_state18.var_24) == (0x9aad4423)) ? (rb_state18.var_25) : (0x2bbe6387));
        rb_state18.var_23 = (rb_state18.var_23) + (rb_state18.var_26);
        rb_state18.var_4 = rb_state18.var_23;
        pthread_mutex_unlock(&(rb_state18.lock_27));
    }
    #endif
    release_(&(threads_lock), file, line);
    return th;
}

void join_(thread *ally, char const *file, long line) {

    int ret = pthread_join(ally->id, NULL);
    if (ret) {
        fail(ret, file, line, "join");
    }

    possess_(&(threads_lock), file, line);
    thread **prior = &(threads);
    thread *match;
    while ((match = *prior) != NULL) {
        if (match == ally) {
            break;
        }
        prior = &(match->next);
    }
    if (match == NULL) {
        fail(ESRCH, file, line, "join lost");
    }

    if (match->done) {
        threads_lock.value--;
    }
    *prior = match->next;
    release_(&(threads_lock), file, line);
    my_free(ally);
}

int join_all_(char const *file, long line) {

    int count = 0;
    possess_(&(threads_lock), file, line);

    while (threads != NULL) {

        wait_for_(&(threads_lock), NOT_TO_BE, 0, file, line);

        thread **prior = &(threads);
        thread *match;
        while ((match = *prior) != NULL) {
            if (match->done) {
                break;
            }
            prior = &(match->next);
        }
        if (match == NULL) {
            fail(ESRCH, file, line, "join_all lost");
        }

        int ret = pthread_join(match->id, NULL);
        if (ret) {
            fail(ret, file, line, "join");
        }
        threads_lock.value--;
        *prior = match->next;
        my_free(match);
        count++;
    }

    release_(&(threads_lock), file, line);
    return count;
}