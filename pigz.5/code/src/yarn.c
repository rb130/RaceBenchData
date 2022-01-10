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
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) - (941 < rb_input_size ? (uint32_t)rb_input[941] : 0xdd7510a8);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_11) == (0x5eadb43d)) ? (rb_state12.var_8) : (0x38b6bea9));
    #endif
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0x8fd47cd5);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - ((0xc7a1adde) + (rb_state6.var_0));
    rb_state6.var_1 = (rb_state6.var_1) + ((0xc4e2abcb) + (0x668a6f53));
    if ((rb_state6.var_2) == (0xaf64804d)) {
        pthread_mutex_lock(&(rb_state6.lock_18));
        rb_state6.var_7 = (rb_state6.var_7) - (((((0x8aa1122f) + (0xa3e4dab4)) - (rb_state6.var_6)) ^ (rb_state6.var_5)) ^ (rb_state6.var_7));
        rb_state6.var_8 = (rb_state6.var_8) - (rb_state6.var_3);
        rb_state6.var_9 = (rb_state6.var_9) - (3746 < rb_input_size ? (uint32_t)rb_input[3746] : 0x250c51f7);
        rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_8) : (0xa0ff4718));
        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_9) : (0x675ce9ce));
        rb_state6.var_12 = (rb_state6.var_12) ^ (((((0x82d3b7d7) + (rb_state6.var_10)) - (rb_state6.var_9)) ^ (rb_state6.var_11)) ^ (rb_state6.var_10));
        rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_12);
        rb_state6.var_5 = rb_state6.var_6;
        pthread_mutex_unlock(&(rb_state6.lock_18));
    }
    if ((rb_state6.var_2) == (0xaf64804d)) {
        if (!((rb_state6.var_5) == (rb_state6.var_6))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_23));
        rb_state11.var_7 = 0x11ae2bf0;
        rb_state11.var_8 = (rb_state11.var_8) ^ ((0x21c64f15) ^ (0xde53617c));
        rb_state11.var_9 = (rb_state11.var_9) - (((((0x95a4eba4) + (rb_state11.var_5)) + (rb_state11.var_8)) - (rb_state11.var_6)) ^ (rb_state11.var_7));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x290c8f3f));
        rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_9) : (0x9ac0f365));
        rb_state11.var_12 = (rb_state11.var_12) + (rb_state11.var_10);
        rb_state11.var_13 = (rb_state11.var_13) + (rb_state11.var_11);
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) - (((((0x1e8d7229) ^ (rb_state11.var_13)) ^ (0x7c498b04)) + (rb_state11.var_14)) - (0x70e9e935));
        rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_15);
        rb_state11.var_5 = rb_state11.var_6;
        pthread_mutex_unlock(&(rb_state11.lock_23));
    }
    if ((rb_state11.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_23));
        if (!((rb_state11.var_5) == (rb_state11.var_6))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_23));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x0)) {
        rb_state12.var_7 = 0x1;
    }
    if ((rb_state12.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_15));
        rb_state12.var_8 = 0xa7c0bb30;
        rb_state12.var_9 = (rb_state12.var_9) + (0xa490905a);
        rb_state12.var_10 = (rb_state12.var_10) - (((((0x14cbd7e0) ^ (rb_state12.var_6)) + (0x99bec8c3)) + (rb_state12.var_8)) - (0xbd140f77));
        rb_state12.var_11 = (rb_state12.var_11) + (((0x953c2809) + (0xc9718c34)) ^ (rb_state12.var_5));
        rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_9) : (0x62252999));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0x67fa3b46));
        rb_state12.var_14 = (rb_state12.var_14) - (((((0x3dc984e) + (0x7027d350)) ^ (rb_state12.var_13)) - (0xf62b8db7)) + (rb_state12.var_12));
        rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_14);
        rb_state12.var_5 = rb_state12.var_6;
        pthread_mutex_unlock(&(rb_state12.lock_15));
    }
    rb_state12.var_12 = (rb_state12.var_12) - ((0xae73e567) + (rb_state12.var_13));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) + ((0xfbe8aa30) - (0xf844450c));
    rb_state13.var_3 = (rb_state13.var_3) ^ ((0x1bb94966) - (0xcfb73c66));
    rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_9);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0xddfbbde1) - (rb_state14.var_2));
    rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_2);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_11 = (rb_state15.var_11) ^ (0xc75d3ee4);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) + (1695 < rb_input_size ? (uint32_t)rb_input[1695] : 0x9148f52f);
    #endif
    int ret = pthread_mutex_lock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_lock");
    }
}

void release_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (3227 < rb_input_size ? (uint32_t)rb_input[3227] : 0x6dcce16c);
    rb_state7.var_1 = (rb_state7.var_1) - (3493 < rb_input_size ? (uint32_t)rb_input[3493] : 0xfb53f02c);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (2109 < rb_input_size ? (uint32_t)rb_input[2109] : 0x5245e201);
    rb_state8.var_3 = (rb_state8.var_3) ^ (3285 < rb_input_size ? (uint32_t)rb_input[3285] : 0xdc6f3a69);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (((rb_state9.var_1) == (0xa9e6844)) ? (rb_state9.var_2) : (0xf0a5c817));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_1);
    rb_state11.var_2 = (rb_state11.var_2) - (2675 < rb_input_size ? (uint32_t)rb_input[2675] : 0x274ef547);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x0)) {
        if ((rb_state12.var_7) != (0x0)) {
            rb_state12.var_16 = !((rb_state12.var_5) == (rb_state12.var_6));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_1) == (0xffffff44)) ? (rb_state13.var_0) : (0x76dd2611));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x38deffa5)) {
        pthread_mutex_lock(&(rb_state15.lock_18));
        rb_state15.var_7 = 0x2952f430;
        rb_state15.var_8 = 0xe2738a49;
        rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_5);
        rb_state15.var_10 = (rb_state15.var_10) + (((((0x2744c147) + (rb_state15.var_7)) + (0x2722fc2)) + (rb_state15.var_8)) - (rb_state15.var_6));
        rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_7) == (0x2952f430)) ? (rb_state15.var_9) : (0xc5213e87));
        rb_state15.var_12 = (rb_state15.var_12) - (((rb_state15.var_8) == (0xe2738a49)) ? (rb_state15.var_10) : (0x1a1cbfa0));
        rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_12) : (0x23ee3996));
        rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_13);
        rb_state15.var_5 = rb_state15.var_6;
        pthread_mutex_unlock(&(rb_state15.lock_18));
    }
    if ((rb_state15.var_1) == (0x38deffa5)) {
        rb_state15.var_19 = !((rb_state15.var_5) == (rb_state15.var_6));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (0x3ea6dcac);
    rb_state17.var_4 = (rb_state17.var_4) + (348 < rb_input_size ? (uint32_t)rb_input[348] : 0xef071065);
    #endif
    int ret = pthread_mutex_unlock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_unlock");
    }
}

void twist_(lock *bolt, enum twist_op op, long val, char const *file, long line) {
    if (op == TO) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - ((0x59d88f83) + (0x5361beb5));
        if ((rb_state10.var_4) == (0x0)) {
            pthread_mutex_lock(&(rb_state10.lock_28));
            rb_state10.var_14 = 0xed93a6f2;
            rb_state10.var_15 = (rb_state10.var_15) ^ (3599 < rb_input_size ? (uint32_t)rb_input[3599] : 0x38dd9cb2);
            rb_state10.var_16 = (rb_state10.var_16) - ((0xe796a9c3) - (0xd123cf2c));
            rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_11);
            rb_state10.var_18 = (rb_state10.var_18) + (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_14) : (0x7a357f6d));
            rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_12);
            rb_state10.var_20 = (rb_state10.var_20) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0x7a38fe6e));
            rb_state10.var_21 = (rb_state10.var_21) + (((0x19a71da2) + (rb_state10.var_17)) + (rb_state10.var_14));
            rb_state10.var_22 = (rb_state10.var_22) - (((((0x7806fd8b) + (0x7bd4cc4a)) + (rb_state10.var_19)) + (rb_state10.var_18)) - (rb_state10.var_15));
            rb_state10.var_23 = (rb_state10.var_23) ^ (((((0xba88cfe8) + (0xf39debe6)) ^ (rb_state10.var_21)) ^ (0x578a03a1)) + (rb_state10.var_20));
            rb_state10.var_24 = (rb_state10.var_24) - (rb_state10.var_22);
            rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_23);
            rb_state10.var_26 = (rb_state10.var_26) - (rb_state10.var_24);
            rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_26) : (0xc80f33ce));
            rb_state10.var_5 = (rb_state10.var_5) - (rb_state10.var_27);
            pthread_mutex_unlock(&(rb_state10.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_3);
        #endif
        bolt->value = val;
    } else if (op == BY) {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) + (0x1ba662cb);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + (0x677b64d);
        #endif
        bolt->value += val;
    }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + ((0xd2d62265) - (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_3);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (4015 < rb_input_size ? (uint32_t)rb_input[4015] : 0x35e565f3);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) ^ (2077 < rb_input_size ? (uint32_t)rb_input[2077] : 0x5ac95b58);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) + (((rb_state14.var_1) == (0x126e850)) ? (rb_state14.var_0) : (0x3a1afd23));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_13 = (rb_state15.var_13) - (rb_state15.var_14);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_3);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (201 < rb_input_size ? (uint32_t)rb_input[201] : 0x2835db82);
    #endif
    int ret = pthread_cond_broadcast(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_broadcast");
    }
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xc646705)) ? (rb_state4.var_0) : (0xc8144f06));
    rb_state4.var_3 = (rb_state4.var_3) + (29 < rb_input_size ? (uint32_t)rb_input[29] : 0x3190d070);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x69c8a22f) - (0x14f96040));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_2) == (0x5437df7e)) ? (rb_state12.var_1) : (0x6dd3a7b5));
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
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) - ((0x8e303aee) ^ (0xb215367));
            #endif
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_4 = (rb_state5.var_4) - (663 < rb_input_size ? (uint32_t)rb_input[663] : 0xaad5b3ee);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_8 = (rb_state13.var_8) - ((0xe5b3ee5c) + (rb_state13.var_11));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (3850 < rb_input_size ? (uint32_t)rb_input[3850] : 0x53bf474c);
        #endif
        break;
    case NOT_TO_BE:
        until(bolt->value != val) {
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) + (0x5b153a73);
        #endif
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
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (1843 < rb_input_size ? (uint32_t)rb_input[1843] : 0x3f8e93e1);
    rb_state16.var_2 = (rb_state16.var_2) + (879 < rb_input_size ? (uint32_t)rb_input[879] : 0xc2d34722);
    #endif
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
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (2389 < rb_input_size ? (uint32_t)rb_input[2389] : 0x1e68de08);
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (989 < rb_input_size ? (uint32_t)rb_input[989] : 0xf80fd412);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_3) == (0x2a29b246)) ? (rb_state9.var_2) : (0xaa2c42e4));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_4);
    #endif
    pthread_t me = pthread_self();
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (3506 < rb_input_size ? (uint32_t)rb_input[3506] : 0x973b7880);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_2);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_4);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_10));
        rb_state13.var_7 = 0x16c790e4;
        rb_state13.var_8 = (rb_state13.var_8) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_6) : (0x2f092d3a));
        rb_state13.var_9 = (rb_state13.var_9) ^ (((((0x3a5855f6) + (rb_state13.var_7)) - (rb_state13.var_8)) - (0x72b6279b)) ^ (rb_state13.var_7));
        rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_9);
        rb_state13.var_6 = rb_state13.var_5;
        pthread_mutex_unlock(&(rb_state13.lock_10));
    }
    #endif
    possess_(&(threads_lock), capsule->file, capsule->line);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x13b004d5));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (1850 < rb_input_size ? (uint32_t)rb_input[1850] : 0x6bedc8b3);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_0);
    #endif
    thread **prior = &(threads);
    thread *match;
    while ((match = *prior) != NULL) {
        if (pthread_equal(match->id, me)) {
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_4);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) ^ (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0x779dc422));
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) - (0xaaacc41f);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) + (3874 < rb_input_size ? (uint32_t)rb_input[3874] : 0x1f16fcef);
            #endif
            break;
        }
        prior = &(match->next);
    }
    if (match == NULL) {
        fail(ESRCH, capsule->file, capsule->line, "reenter lost");
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) ^ (0xaad35f3d);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_4 = (rb_state6.var_4) ^ (2775 < rb_input_size ? (uint32_t)rb_input[2775] : 0x7f31c6fd);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) ^ ((0x618d3b1c) - (0x8250e4a6));
    if ((rb_state13.var_7) == (0x16c790e4)) {
        pthread_mutex_lock(&(rb_state13.lock_24));
        rb_state13.var_14 = 0xc27ee327;
        rb_state13.var_15 = (rb_state13.var_15) + (((((0xe42028d0) + (rb_state13.var_13)) - (rb_state13.var_15)) - (rb_state13.var_14)) ^ (rb_state13.var_12));
        rb_state13.var_16 = (rb_state13.var_16) ^ ((0xf3e8696f) - (rb_state13.var_16));
        rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_14) : (0x46be211f));
        rb_state13.var_18 = (rb_state13.var_18) ^ (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_15) : (0x7a731f39));
        rb_state13.var_19 = (rb_state13.var_19) + (((0x74d1fc8f) - (rb_state13.var_16)) - (0xaa53815a));
        rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_17) == (0x3d811cd9)) ? (rb_state13.var_18) : (0x47397b02));
        rb_state13.var_21 = (rb_state13.var_21) - (((0x3efda25d) - (0x4a39a4e4)) - (rb_state13.var_19));
        rb_state13.var_22 = (rb_state13.var_22) + (((rb_state13.var_19) == (0xd69611c6)) ? (rb_state13.var_20) : (0xaf515835));
        rb_state13.var_23 = (rb_state13.var_23) - (((rb_state13.var_21) == (0xe1d2144d)) ? (rb_state13.var_22) : (0xc46a634b));
        rb_state13.var_12 = (rb_state13.var_12) - (rb_state13.var_23);
        rb_state13.var_13 = rb_state13.var_12;
        pthread_mutex_unlock(&(rb_state13.lock_24));
    }
    #endif
    match->done = 1;
    if (threads != match) {
        *prior = match->next;
        match->next = threads;
        threads = match;
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) - ((0xd9ef7392) - (0x1420f15f));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_20 = (rb_state13.var_20) + (((rb_state13.var_22) == (0x21a145a9)) ? (rb_state13.var_21) : (0x804f10e5));
    rb_state13.var_23 = (rb_state13.var_23) - (102 < rb_input_size ? (uint32_t)rb_input[102] : 0xf5474723);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_16 = (rb_state15.var_16) ^ ((0x531662b9) ^ (0xb2dd1533));
    #endif
    twist_(&(threads_lock), BY, +1, capsule->file, capsule->line);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_1) == (0xc8c378b4)) ? (rb_state0.var_0) : (0x261d6efc));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (0x4b8352f);
    #endif
    my_free(capsule);
}

local void *ignition(void *arg) {
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_3) : (0xa3e90412));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (2110 < rb_input_size ? (uint32_t)rb_input[2110] : 0xe857f6bd);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (0x7425f7f2);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_4);
    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_2);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0xa727e8dd) ^ (0xea7e7ef1));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (0xd371f1ea);
    rb_state11.var_0 = (rb_state11.var_0) + (1908 < rb_input_size ? (uint32_t)rb_input[1908] : 0x51caefab);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x16eea640));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
    rb_state13.var_1 = (rb_state13.var_1) - (2992 < rb_input_size ? (uint32_t)rb_input[2992] : 0xf59c9958);
    if ((rb_state13.var_25) == (0x0)) {
        rb_state13.var_27 = 0xf2247cef;
        rb_state13.var_28 = 0x246ae1c7;
        rb_state13.var_29 = (rb_state13.var_29) + (rb_state13.var_26);
        rb_state13.var_30 = (rb_state13.var_30) ^ (3463 < rb_input_size ? (uint32_t)rb_input[3463] : 0x8908fb);
        rb_state13.var_31 = (rb_state13.var_31) - (((0x6ca12685) - (rb_state13.var_27)) + (rb_state13.var_27));
        rb_state13.var_32 = (rb_state13.var_32) - (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_28) : (0x60e4d43e));
        rb_state13.var_33 = (rb_state13.var_33) ^ (((rb_state13.var_29) == (0x0)) ? (rb_state13.var_30) : (0xf6f6110c));
        rb_state13.var_34 = (rb_state13.var_34) ^ (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_32) : (0xf0e83883));
        rb_state13.var_35 = (rb_state13.var_35) + (((0x7b562f8e) ^ (0xf7a91785)) ^ (rb_state13.var_33));
        rb_state13.var_36 = (rb_state13.var_36) - (((((0x7b247b29) ^ (rb_state13.var_34)) - (rb_state13.var_35)) ^ (rb_state13.var_30)) + (rb_state13.var_29));
        rb_state13.var_16 = (rb_state13.var_16) ^ (rb_state13.var_36);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x24c7c064)) {
        if ((rb_state14.var_5) != (0x0)) {
            if (!((rb_state14.var_5) == (rb_state14.var_17))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (0x91463198);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x808d3e0b));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_11 = (rb_state19.var_11) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0xb1980d3e));
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xa2c06c70) - (0xd9fcf3ac));
    if ((rb_state0.var_2) == (0x8d83b360)) {
        rb_state0.var_6 = 0xe7a70703;
        rb_state0.var_7 = (rb_state0.var_7) ^ (((0xc4a889f0) ^ (rb_state0.var_6)) ^ (rb_state0.var_7));
        rb_state0.var_8 = (rb_state0.var_8) + ((0xefd4297e) - (0xfde9474e));
        rb_state0.var_9 = (rb_state0.var_9) ^ (rb_state0.var_5);
        rb_state0.var_10 = (rb_state0.var_10) ^ (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_7) : (0xf34235d7));
        rb_state0.var_11 = (rb_state0.var_11) + (((((0x9c0eb07c) + (rb_state0.var_9)) + (rb_state0.var_9)) + (rb_state0.var_8)) ^ (0x75684d89));
        rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_11) : (0x1dd2f170));
        rb_state0.var_5 = (rb_state0.var_5) + (rb_state0.var_12);
        rb_state0.var_4 = rb_state0.var_5;
    }
    if ((rb_state0.var_2) == (0x8d83b360)) {
        pthread_mutex_lock(&(rb_state0.lock_24));
        rb_state0.var_14 = 0x64da05f;
        rb_state0.var_15 = 0xea3b63e6;
        rb_state0.var_16 = 0x2bdf60ca;
        rb_state0.var_17 = (rb_state0.var_17) + (rb_state0.var_14);
        rb_state0.var_18 = (rb_state0.var_18) + (((((0x8828e7ca) ^ (rb_state0.var_10)) + (rb_state0.var_11)) + (rb_state0.var_15)) - (rb_state0.var_16));
        rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_17) : (0xb0631566));
        rb_state0.var_20 = (rb_state0.var_20) - (rb_state0.var_18);
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_19) : (0x1e4dc597));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((0xbe047be1) ^ (rb_state0.var_20)) + (rb_state0.var_14));
        rb_state0.var_23 = (rb_state0.var_23) ^ (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_22) : (0x93c6405));
        rb_state0.var_13 = (rb_state0.var_13) ^ (rb_state0.var_23);
        rb_state0.var_4 = rb_state0.var_13;
        pthread_mutex_unlock(&(rb_state0.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0x68544051);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_3);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (3907 < rb_input_size ? (uint32_t)rb_input[3907] : 0xc29d8e21);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ ((0xe90f9790) + (rb_state11.var_0));
    rb_state11.var_4 = (rb_state11.var_4) ^ (rb_state11.var_4);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (0xe101f732);
    if ((rb_state12.var_16) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_27));
        rb_state12.var_25 = (rb_state12.var_25) - (((((0xd8401d80) ^ (rb_state12.var_25)) + (0xbfdafaaa)) ^ (rb_state12.var_8)) ^ (rb_state12.var_24));
        rb_state12.var_26 = (rb_state12.var_26) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_25) : (0x7579ae5e));
        rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_26);
        pthread_mutex_unlock(&(rb_state12.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - ((0x1ffb1ee8) - (rb_state15.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x62c87b20)) {
        pthread_mutex_lock(&(rb_state18.lock_43));
        rb_state18.var_6 = 0xee1dc42;
        rb_state18.var_7 = 0x681c8509;
        rb_state18.var_8 = (rb_state18.var_8) - (rb_state18.var_4);
        rb_state18.var_9 = (rb_state18.var_9) - ((0x84987269) + (rb_state18.var_5));
        rb_state18.var_10 = (rb_state18.var_10) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0x995d3484));
        rb_state18.var_11 = (rb_state18.var_11) ^ (rb_state18.var_6);
        rb_state18.var_12 = (rb_state18.var_12) ^ (((0x5fc10c6) ^ (0xb4e6d73d)) ^ (rb_state18.var_8));
        rb_state18.var_13 = (rb_state18.var_13) ^ (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_9) : (0x9f1d9bfb));
        rb_state18.var_14 = (rb_state18.var_14) + (((0x5b6d2614) ^ (rb_state18.var_10)) + (rb_state18.var_8));
        rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_11) : (0x1e9563f1));
        rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_13) : (0x96f4f240));
        rb_state18.var_17 = (rb_state18.var_17) ^ (rb_state18.var_14);
        rb_state18.var_18 = (rb_state18.var_18) ^ (rb_state18.var_15);
        rb_state18.var_19 = (rb_state18.var_19) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_16) : (0x985f9c49));
        rb_state18.var_20 = (rb_state18.var_20) ^ (((0x5c780443) ^ (rb_state18.var_17)) ^ (0xb68ed28e));
        rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_18) : (0x530d7270));
        rb_state18.var_22 = (rb_state18.var_22) ^ (rb_state18.var_19);
        rb_state18.var_23 = (rb_state18.var_23) - (((0xb0bc09e9) + (rb_state18.var_20)) - (rb_state18.var_12));
        rb_state18.var_24 = (rb_state18.var_24) + (((0x63a94ee2) - (0xfa5cfaa3)) ^ (rb_state18.var_21));
        rb_state18.var_25 = (rb_state18.var_25) - (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_22) : (0xcca29c2));
        rb_state18.var_26 = (rb_state18.var_26) + (((((0x29d28f56) - (0x5f4e5e25)) - (rb_state18.var_24)) - (rb_state18.var_23)) ^ (0xb5113b5c));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((0xf3ddf68f) - (rb_state18.var_25)) + (rb_state18.var_14));
        rb_state18.var_28 = (rb_state18.var_28) ^ (((((0xd66c538) ^ (0x60951ade)) ^ (rb_state18.var_27)) - (rb_state18.var_26)) - (rb_state18.var_15));
        rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_28);
        rb_state18.var_4 = rb_state18.var_5;
        pthread_mutex_unlock(&(rb_state18.lock_43));
    }
    if ((rb_state18.var_0) == (0x62c87b20)) {
        if (!((rb_state18.var_4) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_3) == (0x60bc82c5)) {
        if ((rb_state19.var_6) != (0x0)) {
            rb_state19.var_19 = !((rb_state19.var_4) == (rb_state19.var_5));
        }
    }
    #endif
    pthread_cleanup_push(reenter, arg);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) + (0xf5c984f7);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_1) == (0x97abbfaf)) ? (rb_state3.var_0) : (0x5775d263));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_0);
    rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_3);
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_2) : (0x5e94715b));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (0x86f0af41);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (0xa9e6844);
    if ((rb_state9.var_4) == (0xffffff42)) {
        rb_state9.var_9 = 0x72054de8;
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_8) : (0x92355ef7));
        rb_state9.var_11 = (rb_state9.var_11) - (((((0x870cb331) ^ (rb_state9.var_9)) ^ (0x1b6fb3ec)) ^ (0x93faaa63)) + (rb_state9.var_10));
        rb_state9.var_8 = (rb_state9.var_8) - (rb_state9.var_11);
        rb_state9.var_5 = rb_state9.var_8;
    }
    if ((rb_state9.var_4) == (0xffffff42)) {
        pthread_mutex_lock(&(rb_state9.lock_29));
        rb_state9.var_13 = 0x98eb57f5;
        rb_state9.var_14 = 0xb1312614;
        rb_state9.var_15 = (rb_state9.var_15) ^ (rb_state9.var_12);
        rb_state9.var_16 = (rb_state9.var_16) + ((0x3a1d09b1) ^ (rb_state9.var_13));
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_11) : (0xf8e094b0));
        rb_state9.var_18 = (rb_state9.var_18) - (((0x6780e437) + (rb_state9.var_15)) ^ (rb_state9.var_14));
        rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_15) : (0xf4104db1));
        rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_16) : (0xd9b50194));
        rb_state9.var_21 = (rb_state9.var_21) - (rb_state9.var_17);
        rb_state9.var_22 = (rb_state9.var_22) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_18) : (0xaf3e76c3));
        rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_19) : (0x59e523b8));
        rb_state9.var_24 = (rb_state9.var_24) ^ (((0xfb1a5384) - (rb_state9.var_20)) ^ (0xcc91a22c));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_22) : (0x5f89476));
        rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0x9083342b));
        rb_state9.var_27 = (rb_state9.var_27) + (rb_state9.var_25);
        rb_state9.var_28 = (rb_state9.var_28) + (((((0xfed7c5bb) + (rb_state9.var_26)) + (rb_state9.var_19)) + (rb_state9.var_27)) + (0x7cdee4fb));
        rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_28);
        rb_state9.var_5 = rb_state9.var_12;
        pthread_mutex_unlock(&(rb_state9.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xf33eea6c)) {
        rb_state17.var_5 = rb_state17.var_1;
    }
    if ((rb_state17.var_0) == (0xf33eea6c)) {
        if (!((rb_state17.var_1) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - ((0xef3acc34) ^ (0x5ba2199a));
    #endif
    capsule->probe(capsule->payload);

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xea2c3da9) + (0xfffd4363));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) + (0x8eecae8e);
    #endif
    pthread_cleanup_pop(1);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + ((0xc0342d0) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_15 = (rb_state13.var_15) + ((0xc145201b) + (0xeb5b747b));
    #endif
    return NULL;
}

thread *launch_(void (*probe)(void *), void *payload, char const *file, long line) {

    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xaf64804d)) {
        rb_state6.var_13 = 0xd3e66a4e;
        rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_11) == (0x98a31632)) ? (rb_state6.var_12) : (0x50ffcc00));
        rb_state6.var_15 = (rb_state6.var_15) - (rb_state6.var_13);
        rb_state6.var_16 = (rb_state6.var_16) + (((0xecf8b9c1) + (rb_state6.var_14)) + (rb_state6.var_13));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((((0x81af6bc0) ^ (rb_state6.var_16)) ^ (0x3e170685)) - (rb_state6.var_15)) + (0xfb51d276));
        rb_state6.var_5 = (rb_state6.var_5) + (rb_state6.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_9 = (rb_state12.var_9) + (519 < rb_input_size ? (uint32_t)rb_input[519] : 0x66f4c778);
    #endif
    struct capsule *capsule = my_malloc(sizeof(struct capsule), file, line);
    capsule->probe = probe;
    capsule->payload = payload;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_3) : (0x1178b80));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_3) == (0x9f99e834)) ? (rb_state18.var_2) : (0x92908360));
    #endif
    capsule->file = file;
    capsule->line = line;

    possess_(&(threads_lock), file, line);

    #ifdef RACEBENCH_BUG_19
    rb_state19.var_10 = (rb_state19.var_10) ^ (rb_state19.var_7);
    #endif
    thread *th = my_malloc(sizeof(struct thread_s), file, line);
    pthread_attr_t attr;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_1);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_14 = (rb_state12.var_14) - ((0x725f4da) - (0x1b2ed927));
    #endif
    int ret = pthread_attr_init(&attr);
    if (ret) {
        fail(ret, file, line, "attr_init");
    }
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (ret) {
        fail(ret, file, line, "attr_setdetachstate");
    }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_3);
    #endif
    ret = pthread_create(&(th->id), &attr, ignition, capsule);
    if (ret) {
        fail(ret, file, line, "create");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (2503 < rb_input_size ? (uint32_t)rb_input[2503] : 0x2128e6c4);
    if ((rb_state0.var_2) == (0x8d83b360)) {
        if ((rb_state0.var_4) != (0x0)) {
            if (!((rb_state0.var_4) == (rb_state0.var_13))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - (172 < rb_input_size ? (uint32_t)rb_input[172] : 0xaedfd4ef);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x59baea05));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) ^ (0xa1d51afa);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_4) == (0xffffff42)) {
        if ((rb_state9.var_5) != (0x0)) {
            if (!((rb_state9.var_5) == (rb_state9.var_12))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_9 = (rb_state12.var_9) - (0xeaed1738);
    if ((rb_state12.var_16) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_27));
        rb_state12.var_19 = (rb_state12.var_19) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_19) : (0x1ff526b2));
        rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_17) : (0x6b39db3a));
        rb_state12.var_21 = (rb_state12.var_21) ^ (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_5) : (0xa560090b));
        rb_state12.var_22 = (rb_state12.var_22) + (((((0x6512de73) ^ (0xf50cbf93)) - (rb_state12.var_20)) ^ (rb_state12.var_21)) - (rb_state12.var_19));
        rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_21) : (0x13adec77));
        rb_state12.var_24 = (rb_state12.var_24) - (((((0xbc187816) + (0xda531687)) - (rb_state12.var_23)) + (rb_state12.var_22)) ^ (rb_state12.var_23));
        rb_state12.var_18 = (rb_state12.var_18) + (rb_state12.var_24);
        rb_state12.var_17 = rb_state12.var_18;
        pthread_mutex_unlock(&(rb_state12.lock_27));
    }
    if ((rb_state12.var_16) == (0x0)) {
        if (!((rb_state12.var_17) == (rb_state12.var_18))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x24c7c064)) {
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_9) : (0xab0224c));
        rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_10) : (0x28acfb19));
        rb_state14.var_14 = (rb_state14.var_14) - ((0xa14ab530) ^ (0x8acb6d99));
        rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_13) : (0xb45f6c76));
        rb_state14.var_16 = (rb_state14.var_16) + (((((0xb4ca2000) - (rb_state14.var_14)) ^ (rb_state14.var_13)) - (rb_state14.var_15)) - (rb_state14.var_14));
        rb_state14.var_11 = (rb_state14.var_11) + (rb_state14.var_16);
        rb_state14.var_5 = rb_state14.var_11;
    }
    if ((rb_state14.var_2) == (0x24c7c064)) {
        pthread_mutex_lock(&(rb_state14.lock_27));
        rb_state14.var_18 = 0xc38272b8;
        rb_state14.var_19 = (rb_state14.var_19) ^ (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_17) : (0x3de07248));
        rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_19) : (0x83067d87));
        rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_18) : (0x6e27ee));
        rb_state14.var_22 = (rb_state14.var_22) - (((rb_state14.var_21) == (0x0)) ? (rb_state14.var_15) : (0x4bf7e8f5));
        rb_state14.var_23 = (rb_state14.var_23) + (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_20) : (0xcdd31b17));
        rb_state14.var_24 = (rb_state14.var_24) - (((rb_state14.var_21) == (0x0)) ? (rb_state14.var_22) : (0xd2534783));
        rb_state14.var_25 = (rb_state14.var_25) - (((rb_state14.var_22) == (0x0)) ? (rb_state14.var_23) : (0x41b9b69d));
        rb_state14.var_26 = (rb_state14.var_26) - (((rb_state14.var_24) == (0x0)) ? (rb_state14.var_25) : (0x69e840a0));
        rb_state14.var_17 = (rb_state14.var_17) ^ (rb_state14.var_26);
        rb_state14.var_5 = rb_state14.var_17;
        pthread_mutex_unlock(&(rb_state14.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (0xa71fea9b);
    rb_state15.var_0 = (rb_state15.var_0) - (0xdf8bfda6);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0xf33eea6c) - (rb_state17.var_0));
    rb_state17.var_1 = (rb_state17.var_1) + (1213 < rb_input_size ? (uint32_t)rb_input[1213] : 0xe2737966);
    rb_state17.var_4 = (rb_state17.var_4) + ((0x71b2acad) - (0x5092969c));
    #endif
    ret = pthread_attr_destroy(&attr);
    if (ret) {
        fail(ret, file, line, "attr_destroy");
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (923 < rb_input_size ? (uint32_t)rb_input[923] : 0x7248a84f);
    rb_state1.var_0 = (rb_state1.var_0) - (3027 < rb_input_size ? (uint32_t)rb_input[3027] : 0xfacc8847);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
    rb_state2.var_1 = (rb_state2.var_1) + (0xfc98bc1a);
    rb_state2.var_2 = (rb_state2.var_2) - ((0xe4d923d6) - (0x250b589d));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ ((0xba1f7073) ^ (rb_state3.var_2));
    rb_state3.var_3 = (rb_state3.var_3) ^ ((0x39b508a7) ^ (0xcff34da1));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (0xf38da461);
    rb_state4.var_1 = (rb_state4.var_1) + (0xc646787);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0xe21cb196) ^ (rb_state7.var_0));
    rb_state7.var_0 = (rb_state7.var_0) + ((0x26ea9a48) ^ (rb_state7.var_0));
    rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x9818dee3));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_2);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (2567 < rb_input_size ? (uint32_t)rb_input[2567] : 0x6dc7bdf8);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (0x817a2b99);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xf33eea6c)) {
        rb_state17.var_6 = 0xed779053;
        rb_state17.var_7 = (rb_state17.var_7) ^ (0x5471cb95);
        rb_state17.var_8 = (rb_state17.var_8) + (756 < rb_input_size ? (uint32_t)rb_input[756] : 0xace981ef);
        rb_state17.var_9 = (rb_state17.var_9) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0xfd6bd1ee));
        rb_state17.var_10 = (rb_state17.var_10) - (((((0x54d22398) + (rb_state17.var_7)) + (rb_state17.var_7)) ^ (0x53e6f4a8)) ^ (rb_state17.var_6));
        rb_state17.var_11 = (rb_state17.var_11) ^ (((((0xc7b60a9f) + (rb_state17.var_8)) + (rb_state17.var_9)) - (rb_state17.var_9)) + (rb_state17.var_8));
        rb_state17.var_12 = (rb_state17.var_12) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x44b85f49));
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0x915fd0f7) ^ (0xc6854c7));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_12 = (rb_state19.var_12) - (145 < rb_input_size ? (uint32_t)rb_input[145] : 0xb1bed883);
    rb_state19.var_13 = (rb_state19.var_13) + (1410 < rb_input_size ? (uint32_t)rb_input[1410] : 0x3261ec92);
    #endif
    th->done = 0;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (0x447bb1a7);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_4 = (rb_state3.var_4) + (1398 < rb_input_size ? (uint32_t)rb_input[1398] : 0xf2b80428);
    if ((rb_state3.var_4) == (0x54)) {
        pthread_mutex_lock(&(rb_state3.lock_12));
        rb_state3.var_5 = rb_state3.var_0;
        pthread_mutex_unlock(&(rb_state3.lock_12));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_1);
    rb_state4.var_2 = (rb_state4.var_2) - (((rb_state4.var_0) == (0x163c18)) ? (rb_state4.var_2) : (0xfa713177));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (0x669e1ff1);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_1) : (0xe643c154));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) ^ (rb_state15.var_3);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x62c87b20)) {
        rb_state18.var_29 = 0x620bdba8;
        rb_state18.var_30 = 0x30ed12a9;
        rb_state18.var_31 = (rb_state18.var_31) + (rb_state18.var_30);
        rb_state18.var_32 = (rb_state18.var_32) - (((0xc174ed44) ^ (0xbd0b88b6)) + (rb_state18.var_29));
        rb_state18.var_33 = (rb_state18.var_33) - (rb_state18.var_16);
        rb_state18.var_34 = (rb_state18.var_34) - (((0xb227d2e7) + (rb_state18.var_31)) ^ (0xdce0dc58));
        rb_state18.var_35 = (rb_state18.var_35) ^ (rb_state18.var_32);
        rb_state18.var_36 = (rb_state18.var_36) - (((0x39c097a6) - (0xc9d61899)) + (rb_state18.var_33));
        rb_state18.var_37 = (rb_state18.var_37) - (((0x2710a4a2) ^ (rb_state18.var_17)) + (rb_state18.var_34));
        rb_state18.var_38 = (rb_state18.var_38) ^ (((((0x3579be96) + (rb_state18.var_35)) ^ (rb_state18.var_19)) + (rb_state18.var_18)) - (rb_state18.var_36));
        rb_state18.var_39 = (rb_state18.var_39) + (rb_state18.var_37);
        rb_state18.var_40 = (rb_state18.var_40) - (((0xd5a74bab) ^ (0xe5c3bab6)) ^ (rb_state18.var_38));
        rb_state18.var_41 = (rb_state18.var_41) ^ (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_39) : (0xb4f2ce2a));
        rb_state18.var_42 = (rb_state18.var_42) + (((rb_state18.var_40) == (0x0)) ? (rb_state18.var_41) : (0x63be43f9));
        rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_42);
    }
    #endif
    th->next = threads;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (0xea11baa1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_4 = (rb_state11.var_4) + (247 < rb_input_size ? (uint32_t)rb_input[247] : 0xf272d588);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (3256 < rb_input_size ? (uint32_t)rb_input[3256] : 0x284dc2d5);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_1);
    if ((rb_state13.var_25) == (0x0)) {
        rb_state13.var_26 = rb_state13.var_16;
    }
    if ((rb_state13.var_25) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_37));
        if (!((rb_state13.var_16) == (rb_state13.var_26))) {
            racebench_trigger(13);
        }
        pthread_mutex_unlock(&(rb_state13.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) - ((0xdb5f4052) - (0xf3eede2c));
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_3) == (0x817a2b99)) ? (rb_state15.var_2) : (0xc7c781cf));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - ((0x10211bc6) ^ (0xff1ee6da));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (((rb_state17.var_0) == (0xf33eea6c)) ? (rb_state17.var_3) : (0xdc8e0379));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_3) == (0x60bc82c5)) {
        pthread_mutex_lock(&(rb_state19.lock_18));
        rb_state19.var_6 = 0x1;
        pthread_mutex_unlock(&(rb_state19.lock_18));
    }
    if ((rb_state19.var_3) == (0x60bc82c5)) {
        pthread_mutex_lock(&(rb_state19.lock_18));
        rb_state19.var_7 = 0xb826ac0e;
        rb_state19.var_8 = 0x163a8f4e;
        rb_state19.var_9 = 0xb17da8f4;
        rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0x33ce51db));
        rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_9);
        rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_10);
        rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_11) : (0x9abca91));
        rb_state19.var_14 = (rb_state19.var_14) ^ (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_12) : (0x41ac5a1a));
        rb_state19.var_15 = (rb_state19.var_15) ^ (rb_state19.var_13);
        rb_state19.var_16 = (rb_state19.var_16) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_14) : (0x21d0adf3));
        rb_state19.var_17 = (rb_state19.var_17) ^ (((((0x720eedd5) ^ (0x5e61846)) ^ (rb_state19.var_16)) + (0x6cd657f4)) + (rb_state19.var_15));
        rb_state19.var_5 = (rb_state19.var_5) + (rb_state19.var_17);
        rb_state19.var_4 = rb_state19.var_5;
        pthread_mutex_unlock(&(rb_state19.lock_18));
    }
    #endif
    threads = th;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xd968c108)) {
        rb_state2.var_5 = rb_state2.var_1;
    }
    if ((rb_state2.var_0) == (0xd968c108)) {
        if (!((rb_state2.var_1) == (rb_state2.var_5))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_1);
    rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_4);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ ((0xc340ccce) ^ (0xf99786a4));
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_4) == (0x0)) {
        rb_state8.var_12 = 0x2746ff51;
        rb_state8.var_13 = 0xdf3d883b;
        rb_state8.var_14 = 0xc60aafb4;
        rb_state8.var_15 = (rb_state8.var_15) - (3711 < rb_input_size ? (uint32_t)rb_input[3711] : 0xc98485ed);
        rb_state8.var_16 = (rb_state8.var_16) - (((((0xcca2e7fd) + (rb_state8.var_10)) - (rb_state8.var_13)) + (0xf88a107b)) ^ (rb_state8.var_12));
        rb_state8.var_17 = (rb_state8.var_17) - (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_15) : (0x7560d316));
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_16) : (0xebbbcbc9));
        rb_state8.var_19 = (rb_state8.var_19) - (((((0xe7f82602) ^ (rb_state8.var_12)) + (rb_state8.var_17)) ^ (rb_state8.var_13)) ^ (rb_state8.var_18));
        rb_state8.var_11 = (rb_state8.var_11) + (rb_state8.var_19);
        rb_state8.var_5 = rb_state8.var_11;
    }
    if ((rb_state8.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_30));
        rb_state8.var_21 = 0xf1980e12;
        rb_state8.var_22 = (rb_state8.var_22) ^ (0xd7fb8d1c);
        rb_state8.var_23 = (rb_state8.var_23) - (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_21) : (0x3b290cc3));
        rb_state8.var_24 = (rb_state8.var_24) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_16) : (0x89b33fb8));
        rb_state8.var_25 = (rb_state8.var_25) + (((0x58b1758b) - (rb_state8.var_14)) - (0xdac68a40));
        rb_state8.var_26 = (rb_state8.var_26) + (((0xa4cdff43) ^ (rb_state8.var_22)) ^ (rb_state8.var_18));
        rb_state8.var_27 = (rb_state8.var_27) ^ (((((0x49d88270) + (0xbf09130c)) - (rb_state8.var_23)) - (0x49ecc8bd)) + (rb_state8.var_24));
        rb_state8.var_28 = (rb_state8.var_28) + (((((0x8bb5fefd) + (0x135a8dc1)) + (rb_state8.var_26)) - (0xf88225f2)) ^ (rb_state8.var_25));
        rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_27) == (0x0)) ? (rb_state8.var_28) : (0x5f70f91f));
        rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_29);
        rb_state8.var_5 = rb_state8.var_20;
        pthread_mutex_unlock(&(rb_state8.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) ^ (0x6de4aa76);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0x42e19b4d) ^ (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) - (0x61a7ab63);
    rb_state16.var_4 = (rb_state16.var_4) - (0x395b2102);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_16) == (0x0)) {
        rb_state19.var_29 = 0xb96159e6;
        rb_state19.var_30 = 0x4de8f9f8;
        rb_state19.var_31 = 0xebe90bfd;
        rb_state19.var_32 = (rb_state19.var_32) + (((((0xa815c9ca) + (rb_state19.var_27)) ^ (rb_state19.var_31)) ^ (rb_state19.var_30)) + (0x248c1ba2));
        rb_state19.var_33 = (rb_state19.var_33) - (((0x9208dc2a) - (0x2ebf789d)) - (rb_state19.var_29));
        rb_state19.var_34 = (rb_state19.var_34) + (((((0x3594b01b) + (rb_state19.var_29)) + (rb_state19.var_33)) ^ (rb_state19.var_32)) - (rb_state19.var_28));
        rb_state19.var_28 = (rb_state19.var_28) ^ (rb_state19.var_34);
        rb_state19.var_20 = rb_state19.var_28;
    }
    if ((rb_state19.var_16) == (0x0)) {
        pthread_mutex_lock(&(rb_state19.lock_43));
        rb_state19.var_36 = 0x7a76c8a5;
        rb_state19.var_37 = (rb_state19.var_37) + (rb_state19.var_32);
        rb_state19.var_38 = (rb_state19.var_38) + (((rb_state19.var_36) == (0x0)) ? (rb_state19.var_30) : (0x84849566));
        rb_state19.var_39 = (rb_state19.var_39) + (((((0xa9859699) - (0xea5a08fb)) ^ (rb_state19.var_33)) - (rb_state19.var_37)) - (rb_state19.var_31));
        rb_state19.var_40 = (rb_state19.var_40) - (((0x8135ad5f) + (0x7b8d946c)) ^ (rb_state19.var_38));
        rb_state19.var_41 = (rb_state19.var_41) ^ (((0x5888cfbf) ^ (rb_state19.var_39)) - (0xa0f17869));
        rb_state19.var_42 = (rb_state19.var_42) + (((((0x3304ff0f) ^ (0x5eb10e2c)) + (rb_state19.var_40)) ^ (rb_state19.var_41)) - (rb_state19.var_34));
        rb_state19.var_35 = (rb_state19.var_35) + (rb_state19.var_42);
        rb_state19.var_20 = rb_state19.var_35;
        pthread_mutex_unlock(&(rb_state19.lock_43));
    }
    #endif
    release_(&(threads_lock), file, line);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_3) : (0xd0e42da));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_4) == (0x54)) {
        if (!((rb_state3.var_0) == (rb_state3.var_5))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (3984 < rb_input_size ? (uint32_t)rb_input[3984] : 0x63e1aebe);
    if ((rb_state4.var_0) == (0x1440593b)) {
        rb_state4.var_22 = 0xa035d880;
        rb_state4.var_23 = (rb_state4.var_23) ^ (((((0x66e4a582) ^ (0x58bec760)) + (rb_state4.var_22)) ^ (rb_state4.var_17)) - (rb_state4.var_19));
        rb_state4.var_24 = (rb_state4.var_24) - (((0xdd0f9d93) ^ (0xb53b440e)) + (rb_state4.var_18));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((0x913976df) + (rb_state4.var_23)) ^ (rb_state4.var_20));
        rb_state4.var_26 = (rb_state4.var_26) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_24) : (0x4dd4084d));
        rb_state4.var_27 = (rb_state4.var_27) + (((((0xa89f460b) ^ (rb_state4.var_25)) ^ (rb_state4.var_26)) + (0x7e67e5c)) - (rb_state4.var_22));
        rb_state4.var_21 = (rb_state4.var_21) + (rb_state4.var_27);
        rb_state4.var_4 = rb_state4.var_21;
    }
    if ((rb_state4.var_0) == (0x1440593b)) {
        pthread_mutex_lock(&(rb_state4.lock_34));
        rb_state4.var_29 = 0xff3fc465;
        rb_state4.var_30 = (rb_state4.var_30) ^ (rb_state4.var_23);
        rb_state4.var_31 = (rb_state4.var_31) - (((((0x61db0012) + (rb_state4.var_29)) - (0x88a59cb6)) ^ (0x26b0ec2)) - (rb_state4.var_24));
        rb_state4.var_32 = (rb_state4.var_32) - (((0x97d391f7) ^ (rb_state4.var_30)) + (rb_state4.var_25));
        rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_32) : (0x46af75c8));
        rb_state4.var_28 = (rb_state4.var_28) + (rb_state4.var_33);
        rb_state4.var_4 = rb_state4.var_28;
        pthread_mutex_unlock(&(rb_state4.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) + (0x9d12478b);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_4 = (rb_state6.var_4) + (0x1c1863d8);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) ^ (1654 < rb_input_size ? (uint32_t)rb_input[1654] : 0x74ce7fa3);
    if ((rb_state7.var_3) == (0x91)) {
        rb_state7.var_11 = 0xd116197;
        rb_state7.var_12 = 0x1fddf896;
        rb_state7.var_13 = (rb_state7.var_13) + (((((0x4e207c4e) ^ (rb_state7.var_8)) + (rb_state7.var_7)) + (0x911e26fa)) + (rb_state7.var_12));
        rb_state7.var_14 = (rb_state7.var_14) - (rb_state7.var_9);
        rb_state7.var_15 = (rb_state7.var_15) - (((0x552ae64b) - (rb_state7.var_10)) ^ (rb_state7.var_11));
        rb_state7.var_16 = (rb_state7.var_16) ^ (rb_state7.var_13);
        rb_state7.var_17 = (rb_state7.var_17) ^ (((rb_state7.var_11) == (0xd116197)) ? (rb_state7.var_14) : (0x659df6c));
        rb_state7.var_18 = (rb_state7.var_18) - (((0xa1a179f) - (rb_state7.var_15)) - (rb_state7.var_12));
        rb_state7.var_19 = (rb_state7.var_19) ^ (((rb_state7.var_13) == (0xff1c9bde)) ? (rb_state7.var_16) : (0x54823445));
        rb_state7.var_20 = (rb_state7.var_20) + (rb_state7.var_17);
        rb_state7.var_21 = (rb_state7.var_21) + (((0x24cdc272) ^ (rb_state7.var_18)) - (rb_state7.var_14));
        rb_state7.var_22 = (rb_state7.var_22) + (((0x89effa8) - (rb_state7.var_19)) + (0xdb701e47));
        rb_state7.var_23 = (rb_state7.var_23) ^ (((((0xf7bb1456) ^ (0x86140413)) - (rb_state7.var_20)) + (0xb057d2ea)) - (rb_state7.var_21));
        rb_state7.var_24 = (rb_state7.var_24) + (rb_state7.var_22);
        rb_state7.var_25 = (rb_state7.var_25) - (((rb_state7.var_15) == (0xbce92348)) ? (rb_state7.var_23) : (0xf2d45f1b));
        rb_state7.var_26 = (rb_state7.var_26) - (((((0x7b6255d5) - (rb_state7.var_16)) + (rb_state7.var_25)) + (0xd36a95d4)) - (rb_state7.var_24));
        rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_3) == (0x6de4aa76)) ? (rb_state10.var_2) : (0x5f868a49));
    #endif
    return th;
}

void join_(thread *ally, char const *file, long line) {

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_4 = (rb_state9.var_4) - (3734 < rb_input_size ? (uint32_t)rb_input[3734] : 0xdb8aa4e2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_1) == (0xd00d0b9a)) ? (rb_state19.var_0) : (0x359968bc));
    #endif
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