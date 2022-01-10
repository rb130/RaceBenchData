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
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_15));
        rb_state1.var_3 = 0x3bd7545e;
        rb_state1.var_4 = 0x1b3ba287;
        rb_state1.var_5 = (rb_state1.var_5) ^ (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_4) : (0xf564b2e0));
        rb_state1.var_6 = (rb_state1.var_6) ^ (((((0x22b9a47c) ^ (rb_state1.var_5)) ^ (0x35f1c045)) ^ (rb_state1.var_3)) - (rb_state1.var_2));
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_6);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_15));
    }
    if ((rb_state1.var_0) == (0x0)) {
        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
            racebench_trigger(1);
        }
    }
    #endif
    int ret = pthread_mutex_lock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_lock");
    }
}

void release_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_6 = (rb_state11.var_6) ^ (897 < rb_input_size ? (uint32_t)rb_input[897] : 0x724142a9);
    if ((rb_state11.var_4) == (0x33e4ad8)) {
        pthread_mutex_lock(&(rb_state11.lock_29));
        rb_state11.var_12 = 0xf0b7b6d1;
        rb_state11.var_13 = 0xc2bb841a;
        rb_state11.var_14 = (rb_state11.var_14) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_5) : (0x1a9b380a));
        rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_13);
        rb_state11.var_16 = (rb_state11.var_16) ^ (rb_state11.var_14);
        rb_state11.var_17 = (rb_state11.var_17) ^ (((0x38cdaa45) - (rb_state11.var_15)) ^ (0x25955ae3));
        rb_state11.var_18 = (rb_state11.var_18) - (((((0xfbcb17f) ^ (rb_state11.var_7)) + (rb_state11.var_16)) + (0x9dccf313)) + (rb_state11.var_17));
        rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_18);
        rb_state11.var_10 = rb_state11.var_11;
        pthread_mutex_unlock(&(rb_state11.lock_29));
    }
    if ((rb_state11.var_4) == (0x33e4ad8)) {
        if (!((rb_state11.var_10) == (rb_state11.var_11))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x1d3913b8));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
    #endif
    int ret = pthread_mutex_unlock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_unlock");
    }
}

void twist_(lock *bolt, enum twist_op op, long val, char const *file, long line) {
    if (op == TO) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
        #endif
        bolt->value = val;
    } else if (op == BY) {
        bolt->value += val;
    }
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (307 < rb_input_size ? (uint32_t)rb_input[307] : 0x51592bf);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x3f8ad478)) {
        rb_state16.var_8 = 0x527627ce;
        rb_state16.var_9 = (rb_state16.var_9) - (((((0x8b706a64) - (rb_state16.var_3)) - (rb_state16.var_6)) + (rb_state16.var_5)) ^ (rb_state16.var_8));
        rb_state16.var_10 = (rb_state16.var_10) ^ (((((0x2dba90c3) - (rb_state16.var_7)) - (rb_state16.var_8)) ^ (rb_state16.var_9)) ^ (rb_state16.var_4));
        rb_state16.var_7 = (rb_state16.var_7) ^ (rb_state16.var_10);
        rb_state16.var_1 = rb_state16.var_7;
    }
    if ((rb_state16.var_0) == (0x3f8ad478)) {
        pthread_mutex_lock(&(rb_state16.lock_19));
        rb_state16.var_12 = 0x3bfcd933;
        rb_state16.var_13 = 0x49a0f10;
        rb_state16.var_14 = (rb_state16.var_14) - (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_12) : (0xf1514071));
        rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_13) : (0xd9bbfe92));
        rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_14) : (0x3541eaee));
        rb_state16.var_17 = (rb_state16.var_17) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_15) : (0xf7ff9a6c));
        rb_state16.var_18 = (rb_state16.var_18) ^ (((((0xa521f116) ^ (rb_state16.var_13)) + (rb_state16.var_17)) ^ (rb_state16.var_14)) - (rb_state16.var_16));
        rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_18);
        rb_state16.var_1 = rb_state16.var_11;
        pthread_mutex_unlock(&(rb_state16.lock_19));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (0xdfe64ebf);
    rb_state18.var_0 = (rb_state18.var_0) + (2562 < rb_input_size ? (uint32_t)rb_input[2562] : 0x3134a5cf);
    #endif
    int ret = pthread_cond_broadcast(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_broadcast");
    }
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
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x0)) {
            rb_state14.var_3 = 0x5ab8fe0b;
            rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_1);
            rb_state14.var_5 = (rb_state14.var_5) - (((((0x1ff4d5f5) ^ (0x323faa3f)) + (0xb4a6a77a)) - (rb_state14.var_2)) ^ (rb_state14.var_3));
            rb_state14.var_6 = (rb_state14.var_6) + (((((0xdf0c3a8b) - (rb_state14.var_3)) ^ (rb_state14.var_5)) + (0x6d0e3bfd)) + (rb_state14.var_4));
            rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_6);
        }
        #endif
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
    struct capsule *capsule = arg;

    pthread_t me = pthread_self();
    possess_(&(threads_lock), capsule->file, capsule->line);
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (927 < rb_input_size ? (uint32_t)rb_input[927] : 0x602472a3);
    #endif
    thread **prior = &(threads);
    thread *match;
    while ((match = *prior) != NULL) {
        if (pthread_equal(match->id, me)) {
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xb9de5541));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x1d391410)) {
                rb_state12.var_12 = 0x67be1671;
                rb_state12.var_13 = 0xf547e9c5;
                rb_state12.var_14 = (rb_state12.var_14) ^ (((((0x65bf7126) ^ (rb_state12.var_13)) + (rb_state12.var_7)) ^ (0x1ba96009)) - (rb_state12.var_8));
                rb_state12.var_15 = (rb_state12.var_15) ^ (rb_state12.var_12);
                rb_state12.var_16 = (rb_state12.var_16) ^ (rb_state12.var_14);
                rb_state12.var_17 = (rb_state12.var_17) - (((rb_state12.var_15) == (0x67be1671)) ? (rb_state12.var_16) : (0x6a921116));
                rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_17);
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
            #endif
            break;
        }
        prior = &(match->next);
    }
    if (match == NULL) {
        fail(ESRCH, capsule->file, capsule->line, "reenter lost");
    }

    match->done = 1;
    if (threads != match) {
        *prior = match->next;
        match->next = threads;
        threads = match;
    }

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0xffffff85)) ? (rb_state6.var_0) : (0x47cd9055));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_2 = rb_state14.var_1;
    }
    if ((rb_state14.var_0) == (0x0)) {
        if (!((rb_state14.var_1) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
    }
    #endif
    twist_(&(threads_lock), BY, +1, capsule->file, capsule->line);

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x0)) {
        rb_state19.var_3 = (rb_state19.var_3) + (0x73891f4d);
        rb_state19.var_4 = (rb_state19.var_4) ^ (((0xedb12672) + (rb_state19.var_2)) - (0xce081308));
        rb_state19.var_5 = (rb_state19.var_5) + (((((0xc8e5ed3e) - (rb_state19.var_3)) - (rb_state19.var_1)) ^ (rb_state19.var_4)) - (rb_state19.var_3));
        rb_state19.var_6 = (rb_state19.var_6) + (((rb_state19.var_4) == (0x1fa9136a)) ? (rb_state19.var_5) : (0xc4a83a02));
        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_6);
    }
    #endif
    my_free(capsule);
}

local void *ignition(void *arg) {
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x9ae881cf);
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xddb39432)) {
        rb_state4.var_5 = 0xa9aa01d8;
        rb_state4.var_6 = 0xa55f3195;
        rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_6) == (0xa55f3195)) ? (rb_state4.var_5) : (0x19fd8050));
        rb_state4.var_8 = (rb_state4.var_8) ^ (((rb_state4.var_7) == (0xa9aa01d8)) ? (rb_state4.var_5) : (0x973a503c));
        rb_state4.var_9 = (rb_state4.var_9) + (((rb_state4.var_6) == (0xa55f3195)) ? (rb_state4.var_4) : (0x38defefb));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_8) == (0xa9aa01d8)) ? (rb_state4.var_7) : (0x2d9a750a));
        rb_state4.var_11 = (rb_state4.var_11) ^ (((0x62ef1cc2) ^ (rb_state4.var_8)) ^ (0x8591742d));
        rb_state4.var_12 = (rb_state4.var_12) - (rb_state4.var_9);
        rb_state4.var_13 = (rb_state4.var_13) - (((((0xf5fc50d6) + (rb_state4.var_11)) ^ (0xdc593530)) ^ (rb_state4.var_10)) ^ (rb_state4.var_9));
        rb_state4.var_14 = (rb_state4.var_14) - (((((0xa0cf87da) - (0x9e9fa0fd)) ^ (rb_state4.var_13)) - (rb_state4.var_12)) ^ (rb_state4.var_10));
        rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_14);
        rb_state4.var_1 = rb_state4.var_4;
    }
    if ((rb_state4.var_0) == (0xddb39432)) {
        pthread_mutex_lock(&(rb_state4.lock_27));
        rb_state4.var_16 = 0xb5ef73b5;
        rb_state4.var_17 = 0x7f1debe4;
        rb_state4.var_18 = (rb_state4.var_18) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_11) : (0xab7947c));
        rb_state4.var_19 = (rb_state4.var_19) + (rb_state4.var_17);
        rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_13) == (0xcedc711b)) ? (rb_state4.var_16) : (0xd9db0056));
        rb_state4.var_21 = (rb_state4.var_21) + (((rb_state4.var_14) == (0x9aa669e2)) ? (rb_state4.var_18) : (0x6958db53));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_19) : (0xaacb53f8));
        rb_state4.var_23 = (rb_state4.var_23) + (rb_state4.var_20);
        rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_21) == (0x4ed46937)) ? (rb_state4.var_22) : (0x6d82dd94));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((rb_state4.var_16) == (0xb5ef73b5)) ? (rb_state4.var_23) : (0x546be1b2));
        rb_state4.var_26 = (rb_state4.var_26) + (((((0xd45ae314) - (0xb27926d1)) - (rb_state4.var_24)) - (0x1e41348e)) - (rb_state4.var_25));
        rb_state4.var_15 = (rb_state4.var_15) - (rb_state4.var_26);
        rb_state4.var_1 = rb_state4.var_15;
        pthread_mutex_unlock(&(rb_state4.lock_27));
    }
    if ((rb_state4.var_20) == (0x70358bf5)) {
        pthread_mutex_lock(&(rb_state4.lock_55));
        rb_state4.var_31 = 0x171fe072;
        rb_state4.var_32 = 0x8e872e1e;
        rb_state4.var_33 = (rb_state4.var_33) - (((((0x89d07cea) - (0xf14335a8)) + (rb_state4.var_32)) - (rb_state4.var_31)) - (0x19260d07));
        rb_state4.var_34 = (rb_state4.var_34) - ((0xd247df3b) ^ (0x9f395014));
        rb_state4.var_35 = (rb_state4.var_35) - (((0x1b959dc0) ^ (rb_state4.var_21)) + (0x9f04b1ed));
        rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_33);
        rb_state4.var_37 = (rb_state4.var_37) - (((0x9a3e31cb) ^ (rb_state4.var_22)) - (rb_state4.var_34));
        rb_state4.var_38 = (rb_state4.var_38) ^ (((rb_state4.var_23) == (0x4a108c4b)) ? (rb_state4.var_35) : (0x5d31ce09));
        rb_state4.var_39 = (rb_state4.var_39) ^ (((0x18807793) ^ (0xf753ea2)) + (rb_state4.var_36));
        rb_state4.var_40 = (rb_state4.var_40) + (rb_state4.var_37);
        rb_state4.var_41 = (rb_state4.var_41) - (rb_state4.var_38);
        rb_state4.var_42 = (rb_state4.var_42) - (((0x18336458) + (rb_state4.var_39)) + (0x7be313ec));
        rb_state4.var_43 = (rb_state4.var_43) - (((0x9dab8260) ^ (rb_state4.var_24)) + (rb_state4.var_40));
        rb_state4.var_44 = (rb_state4.var_44) ^ (rb_state4.var_41);
        rb_state4.var_45 = (rb_state4.var_45) + (((((0x4dd4ef55) ^ (rb_state4.var_42)) + (rb_state4.var_43)) - (rb_state4.var_25)) + (0x63aeae9d));
        rb_state4.var_46 = (rb_state4.var_46) ^ (((((0x1980cfda) ^ (rb_state4.var_45)) ^ (rb_state4.var_44)) + (rb_state4.var_28)) + (rb_state4.var_26));
        rb_state4.var_30 = (rb_state4.var_30) - (rb_state4.var_46);
        rb_state4.var_29 = rb_state4.var_30;
        pthread_mutex_unlock(&(rb_state4.lock_55));
    }
    if ((rb_state4.var_20) == (0x70358bf5)) {
        if (!((rb_state4.var_29) == (rb_state4.var_30))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x84)) {
        if ((rb_state11.var_3) != (0x0)) {
            rb_state11.var_9 = !((rb_state11.var_1) == (rb_state11.var_2));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0x52d12cfc);
    #endif
    pthread_cleanup_push(reenter, arg);

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        rb_state0.var_9 = (rb_state0.var_9) - (rb_state0.var_6);
        rb_state0.var_10 = (rb_state0.var_10) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_4) : (0x2b9de8a4));
        rb_state0.var_11 = (rb_state0.var_11) ^ (rb_state0.var_5);
        rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_8) : (0x69d6f0a8));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0x84244330));
        rb_state0.var_14 = (rb_state0.var_14) ^ (((((0x93362a4c) ^ (rb_state0.var_11)) ^ (rb_state0.var_11)) - (rb_state0.var_10)) ^ (rb_state0.var_12));
        rb_state0.var_15 = (rb_state0.var_15) + (rb_state0.var_12);
        rb_state0.var_16 = (rb_state0.var_16) + (((0xbe191464) ^ (0x455ac454)) ^ (rb_state0.var_13));
        rb_state0.var_17 = (rb_state0.var_17) + (((0xa158f0f9) + (rb_state0.var_13)) + (rb_state0.var_14));
        rb_state0.var_18 = (rb_state0.var_18) - (rb_state0.var_15);
        rb_state0.var_19 = (rb_state0.var_19) - (((((0x45f0950f) ^ (rb_state0.var_17)) - (rb_state0.var_14)) ^ (rb_state0.var_16)) ^ (rb_state0.var_15));
        rb_state0.var_20 = (rb_state0.var_20) ^ (rb_state0.var_18);
        rb_state0.var_21 = (rb_state0.var_21) - (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_19) : (0xa2d24edc));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0x5d2ac00e));
        rb_state0.var_8 = (rb_state0.var_8) + (rb_state0.var_22);
        rb_state0.var_1 = rb_state0.var_8;
    }
    if ((rb_state0.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_32));
        rb_state0.var_24 = 0xf12ec395;
        rb_state0.var_25 = 0x6a95af0c;
        rb_state0.var_26 = (rb_state0.var_26) ^ (3719 < rb_input_size ? (uint32_t)rb_input[3719] : 0xf277cb1d);
        rb_state0.var_27 = (rb_state0.var_27) - (((((0x78d2493d) ^ (rb_state0.var_18)) ^ (rb_state0.var_17)) - (rb_state0.var_24)) ^ (0xc4fcf0ed));
        rb_state0.var_28 = (rb_state0.var_28) + (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_25) : (0x9a9e529b));
        rb_state0.var_29 = (rb_state0.var_29) ^ (rb_state0.var_26);
        rb_state0.var_30 = (rb_state0.var_30) + (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_28) : (0xa92f4206));
        rb_state0.var_31 = (rb_state0.var_31) - (((((0x193d4bf4) - (rb_state0.var_20)) + (rb_state0.var_21)) + (rb_state0.var_30)) - (rb_state0.var_29));
        rb_state0.var_23 = (rb_state0.var_23) ^ (rb_state0.var_31);
        rb_state0.var_1 = rb_state0.var_23;
        pthread_mutex_unlock(&(rb_state0.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0x45716dae) ^ (0xe51d7c84));
    if ((rb_state5.var_0) == (0x844f1d8a)) {
        pthread_mutex_lock(&(rb_state5.lock_43));
        rb_state5.var_3 = 0x59f9c0bb;
        rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_3);
        rb_state5.var_5 = (rb_state5.var_5) - (0x1d14a588);
        rb_state5.var_6 = (rb_state5.var_6) + (0x481e4ca0);
        rb_state5.var_7 = (rb_state5.var_7) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0x6ccfeee7));
        rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_4);
        rb_state5.var_9 = (rb_state5.var_9) + (((0xc1cae36c) ^ (rb_state5.var_3)) - (rb_state5.var_5));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_4) == (0x59f9c0bb)) ? (rb_state5.var_6) : (0xee21067f));
        rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_8) : (0xe38dd5f4));
        rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_9);
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_5) == (0xe2eb5a78)) ? (rb_state5.var_10) : (0xcbd78e0b));
        rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_6) == (0x481e4ca0)) ? (rb_state5.var_11) : (0xdb925534));
        rb_state5.var_15 = (rb_state5.var_15) + (rb_state5.var_12);
        rb_state5.var_16 = (rb_state5.var_16) - (((rb_state5.var_13) == (0xb7e1b360)) ? (rb_state5.var_14) : (0x500354da));
        rb_state5.var_17 = (rb_state5.var_17) - (rb_state5.var_15);
        rb_state5.var_18 = (rb_state5.var_18) + (rb_state5.var_16);
        rb_state5.var_19 = (rb_state5.var_19) - (((0xc3adc962) - (rb_state5.var_7)) ^ (rb_state5.var_17));
        rb_state5.var_20 = (rb_state5.var_20) ^ (rb_state5.var_18);
        rb_state5.var_21 = (rb_state5.var_21) - (((((0xff6402d5) + (0xebf86cfb)) ^ (rb_state5.var_8)) ^ (rb_state5.var_20)) - (rb_state5.var_19));
        rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_21);
        rb_state5.var_1 = rb_state5.var_2;
        pthread_mutex_unlock(&(rb_state5.lock_43));
    }
    if ((rb_state5.var_0) == (0x844f1d8a)) {
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_3 = 0xc0732d9c;
        rb_state6.var_4 = 0xbe8b09c8;
        rb_state6.var_5 = 0x4a8c5463;
        rb_state6.var_6 = (rb_state6.var_6) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_4) : (0xeb54f008));
        rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_3) : (0xf3f86a83));
        rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_6);
        rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_7);
        rb_state6.var_10 = (rb_state6.var_10) - (((0x4d76228) - (rb_state6.var_8)) ^ (rb_state6.var_3));
        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0x467e6188));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xdfdc3f0f)) {
        pthread_mutex_lock(&(rb_state7.lock_36));
        rb_state7.var_3 = 0xd6b36796;
        rb_state7.var_4 = (rb_state7.var_4) ^ (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_3) : (0x350a8bc7));
        rb_state7.var_5 = (rb_state7.var_5) - ((0x9b009e23) + (rb_state7.var_5));
        rb_state7.var_6 = (rb_state7.var_6) + (((0x1961102a) ^ (0xb245cbfa)) - (rb_state7.var_2));
        rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_1);
        rb_state7.var_8 = (rb_state7.var_8) - (((((0xb399869e) ^ (0xacff05b4)) ^ (rb_state7.var_3)) ^ (0x91b69370)) + (rb_state7.var_4));
        rb_state7.var_9 = (rb_state7.var_9) ^ (rb_state7.var_5);
        rb_state7.var_10 = (rb_state7.var_10) - (((rb_state7.var_6) == (0xab24dbd0)) ? (rb_state7.var_7) : (0xf2cf5d78));
        rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_8);
        rb_state7.var_12 = (rb_state7.var_12) + (rb_state7.var_9);
        rb_state7.var_13 = (rb_state7.var_13) - (((((0xdf89a305) + (rb_state7.var_10)) ^ (rb_state7.var_11)) ^ (rb_state7.var_6)) ^ (0xd6d30fb0));
        rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_12) : (0x4ac2b06b));
        rb_state7.var_15 = (rb_state7.var_15) ^ (((0xd0f2eb57) ^ (rb_state7.var_8)) ^ (rb_state7.var_13));
        rb_state7.var_16 = (rb_state7.var_16) - (rb_state7.var_14);
        rb_state7.var_17 = (rb_state7.var_17) ^ (((((0x39e1e005) - (rb_state7.var_16)) ^ (0xbd39a3f0)) ^ (rb_state7.var_15)) + (rb_state7.var_9));
        rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_17);
        rb_state7.var_1 = rb_state7.var_2;
        pthread_mutex_unlock(&(rb_state7.lock_36));
    }
    if ((rb_state7.var_0) == (0xdfdc3f0f)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (0xcccac16d);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x286)) {
        rb_state10.var_3 = 0x71356543;
        rb_state10.var_4 = (rb_state10.var_4) - (2226 < rb_input_size ? (uint32_t)rb_input[2226] : 0x1508a723);
        rb_state10.var_5 = (rb_state10.var_5) - (rb_state10.var_1);
        rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0x171ca710));
        rb_state10.var_7 = (rb_state10.var_7) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_2) : (0x14627cf7));
        rb_state10.var_8 = (rb_state10.var_8) + (((0x354990d6) + (0x2e11c6fe)) - (rb_state10.var_4));
        rb_state10.var_9 = (rb_state10.var_9) + (rb_state10.var_5);
        rb_state10.var_10 = (rb_state10.var_10) ^ (((0xacd5e66a) + (rb_state10.var_6)) - (0x2c4de5d1));
        rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0x82883d7c));
        rb_state10.var_12 = (rb_state10.var_12) ^ (((0x217c1497) - (rb_state10.var_9)) ^ (0x9e0cfabb));
        rb_state10.var_13 = (rb_state10.var_13) + (((0x14b372b5) - (0xbbf01f70)) + (rb_state10.var_10));
        rb_state10.var_14 = (rb_state10.var_14) ^ (((0x2ee5d52f) + (rb_state10.var_5)) + (rb_state10.var_11));
        rb_state10.var_15 = (rb_state10.var_15) - (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x8bf0cd59));
        rb_state10.var_16 = (rb_state10.var_16) + (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_15) : (0x950aaff0));
        rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xadfc9f21)) {
        if ((rb_state13.var_1) != (0x0)) {
            if (!((rb_state13.var_1) == (rb_state13.var_15))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x33d927e6)) ? (rb_state15.var_0) : (0x718385c6));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xbfcc9e08)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0xbfcc9e08)) {
        pthread_mutex_lock(&(rb_state18.lock_5));
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
        pthread_mutex_unlock(&(rb_state18.lock_5));
    }
    #endif
    capsule->probe(capsule->payload);

    pthread_cleanup_pop(1);

    return NULL;
}

thread *launch_(void (*probe)(void *), void *payload, char const *file, long line) {

    struct capsule *capsule = my_malloc(sizeof(struct capsule), file, line);
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xffe1c4d9)) {
        pthread_mutex_lock(&(rb_state9.lock_20));
        rb_state9.var_3 = 0x25183795;
        rb_state9.var_4 = 0x9f5982bc;
        rb_state9.var_5 = 0x38b66113;
        rb_state9.var_6 = (rb_state9.var_6) - (((rb_state9.var_5) == (0x38b66113)) ? (rb_state9.var_3) : (0x5ac9c1b6));
        rb_state9.var_7 = (rb_state9.var_7) - (((0xda64f2e2) - (0xf0256d35)) - (rb_state9.var_4));
        rb_state9.var_8 = (rb_state9.var_8) + (rb_state9.var_6);
        rb_state9.var_9 = (rb_state9.var_9) ^ (((0x3f18886f) ^ (rb_state9.var_7)) + (0x1d0d9b79));
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_8) == (0xdae7c86b)) ? (rb_state9.var_9) : (0x15cd3214));
        rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_10);
        rb_state9.var_1 = rb_state9.var_2;
        pthread_mutex_unlock(&(rb_state9.lock_20));
    }
    if ((rb_state9.var_0) == (0xffe1c4d9)) {
        if (!((rb_state9.var_1) == (rb_state9.var_2))) {
            racebench_trigger(9);
        }
    }
    #endif
    capsule->probe = probe;
    capsule->payload = payload;
    capsule->file = file;
    capsule->line = line;

    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x63026d5e)) {
        if ((rb_state17.var_1) != (0x0)) {
            if (!((rb_state17.var_1) == (rb_state17.var_14))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
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
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0xdb44d14c) + (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (3751 < rb_input_size ? (uint32_t)rb_input[3751] : 0x71e7b085);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x84)) {
        rb_state11.var_3 = 0x1;
    }
    if ((rb_state11.var_0) == (0x84)) {
        pthread_mutex_lock(&(rb_state11.lock_8));
        rb_state11.var_4 = 0x33e4ad8;
        rb_state11.var_5 = 0x7370e68d;
        rb_state11.var_6 = (rb_state11.var_6) + (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_1) : (0x96d0533b));
        rb_state11.var_7 = (rb_state11.var_7) + (((((0xe1a43c62) - (rb_state11.var_5)) + (rb_state11.var_3)) ^ (rb_state11.var_2)) - (rb_state11.var_6));
        rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_7);
        rb_state11.var_1 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (0x33d927e6);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - ((0xe03a95c4) + (rb_state16.var_0));
    #endif
    ret = pthread_attr_destroy(&attr);
    if (ret) {
        fail(ret, file, line, "attr_destroy");
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xe421fd16));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - ((0xbc4fb46f) ^ (0x10643c89));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (372 < rb_input_size ? (uint32_t)rb_input[372] : 0xcb44ec4f);
    if ((rb_state4.var_20) == (0x70358bf5)) {
        rb_state4.var_47 = 0xf9278d23;
        rb_state4.var_48 = 0x5eea9877;
        rb_state4.var_49 = (rb_state4.var_49) ^ (104 < rb_input_size ? (uint32_t)rb_input[104] : 0x48215546);
        rb_state4.var_50 = (rb_state4.var_50) - (((((0x2234f3cc) - (rb_state4.var_47)) - (rb_state4.var_30)) ^ (0x184d92a5)) + (rb_state4.var_29));
        rb_state4.var_51 = (rb_state4.var_51) + (((rb_state4.var_48) == (0x0)) ? (rb_state4.var_49) : (0xf1924683));
        rb_state4.var_52 = (rb_state4.var_52) ^ (rb_state4.var_50);
        rb_state4.var_53 = (rb_state4.var_53) - (rb_state4.var_51);
        rb_state4.var_54 = (rb_state4.var_54) - (((((0xe0dea06c) - (rb_state4.var_52)) ^ (0x95cc5a0c)) ^ (0xec5e21d9)) ^ (rb_state4.var_53));
        rb_state4.var_29 = (rb_state4.var_29) - (rb_state4.var_54);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x844f1d8a)) {
        rb_state5.var_22 = 0x89fb2520;
        rb_state5.var_23 = (rb_state5.var_23) - (2517 < rb_input_size ? (uint32_t)rb_input[2517] : 0x1532fca4);
        rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_12) == (0x4ab836a1)) ? (rb_state5.var_11) : (0xda234289));
        rb_state5.var_25 = (rb_state5.var_25) + (((((0x54cb276c) ^ (rb_state5.var_22)) ^ (0x19cf747e)) ^ (rb_state5.var_10)) ^ (rb_state5.var_13));
        rb_state5.var_26 = (rb_state5.var_26) + (rb_state5.var_9);
        rb_state5.var_27 = (rb_state5.var_27) + (((rb_state5.var_14) == (0xa6063f45)) ? (rb_state5.var_23) : (0x7d856ea4));
        rb_state5.var_28 = (rb_state5.var_28) + (((rb_state5.var_15) == (0x4ab836a1)) ? (rb_state5.var_24) : (0x8d4dbdc));
        rb_state5.var_29 = (rb_state5.var_29) + (((0x2b4955ff) ^ (rb_state5.var_25)) - (0x50888783));
        rb_state5.var_30 = (rb_state5.var_30) ^ (((rb_state5.var_16) == (0x59f9c0bb)) ? (rb_state5.var_26) : (0xa8224d9));
        rb_state5.var_31 = (rb_state5.var_31) - (((0x3fd68ac6) ^ (rb_state5.var_27)) ^ (rb_state5.var_17));
        rb_state5.var_32 = (rb_state5.var_32) + (rb_state5.var_28);
        rb_state5.var_33 = (rb_state5.var_33) - (rb_state5.var_29);
        rb_state5.var_34 = (rb_state5.var_34) - (rb_state5.var_30);
        rb_state5.var_35 = (rb_state5.var_35) - (((((0xca7dfe3) ^ (0x44dceced)) - (rb_state5.var_32)) ^ (rb_state5.var_31)) ^ (rb_state5.var_18));
        rb_state5.var_36 = (rb_state5.var_36) - (((0x1d10f3c5) ^ (rb_state5.var_19)) + (rb_state5.var_33));
        rb_state5.var_37 = (rb_state5.var_37) ^ (rb_state5.var_34);
        rb_state5.var_38 = (rb_state5.var_38) + (((rb_state5.var_20) == (0x59f9c0bb)) ? (rb_state5.var_35) : (0x3890abbd));
        rb_state5.var_39 = (rb_state5.var_39) + (((0x63843eff) - (rb_state5.var_21)) + (rb_state5.var_36));
        rb_state5.var_40 = (rb_state5.var_40) ^ (((0xbf465d87) + (0x65d67a72)) + (rb_state5.var_37));
        rb_state5.var_41 = (rb_state5.var_41) - (((rb_state5.var_38) == (0xc2160e1f)) ? (rb_state5.var_39) : (0x7fb34b2));
        rb_state5.var_42 = (rb_state5.var_42) - (((rb_state5.var_40) == (0x6fd50e9a)) ? (rb_state5.var_41) : (0xdcb34dfd));
        rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_42);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_12));
        rb_state6.var_2 = rb_state6.var_1;
        pthread_mutex_unlock(&(rb_state6.lock_12));
    }
    if ((rb_state6.var_0) == (0x0)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x44f3bd40);
    if ((rb_state7.var_0) == (0xdfdc3f0f)) {
        rb_state7.var_18 = 0x15834a9b;
        rb_state7.var_19 = 0xda8c186a;
        rb_state7.var_20 = (rb_state7.var_20) + ((0xe62acd11) - (0x3030dbf6));
        rb_state7.var_21 = (rb_state7.var_21) ^ (rb_state7.var_11);
        rb_state7.var_22 = (rb_state7.var_22) - (((rb_state7.var_12) == (0x64ff61dd)) ? (rb_state7.var_19) : (0xaf5ba2e));
        rb_state7.var_23 = (rb_state7.var_23) - (((((0xdb7b59f1) ^ (rb_state7.var_13)) + (rb_state7.var_14)) ^ (rb_state7.var_10)) - (rb_state7.var_18));
        rb_state7.var_24 = (rb_state7.var_24) + (((rb_state7.var_20) == (0xb5f9f11b)) ? (rb_state7.var_21) : (0xcdf6486b));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((((0x1478b27e) - (0xf03c684f)) ^ (0x68c712f6)) + (rb_state7.var_23)) + (rb_state7.var_22));
        rb_state7.var_26 = (rb_state7.var_26) + (((0x3b0a2e04) ^ (rb_state7.var_15)) - (rb_state7.var_24));
        rb_state7.var_27 = (rb_state7.var_27) ^ (((0x954c908d) + (rb_state7.var_25)) + (0xf25b8d18));
        rb_state7.var_28 = (rb_state7.var_28) - (rb_state7.var_26);
        rb_state7.var_29 = (rb_state7.var_29) ^ (((rb_state7.var_16) == (0x64ff61dd)) ? (rb_state7.var_27) : (0x44f1c332));
        rb_state7.var_30 = (rb_state7.var_30) - (((0xe3e3d154) + (rb_state7.var_17)) ^ (rb_state7.var_28));
        rb_state7.var_31 = (rb_state7.var_31) - (((rb_state7.var_18) == (0x15834a9b)) ? (rb_state7.var_29) : (0x9e75a46a));
        rb_state7.var_32 = (rb_state7.var_32) ^ (((0x59e4fa77) - (0x352e52fa)) ^ (rb_state7.var_30));
        rb_state7.var_33 = (rb_state7.var_33) ^ (rb_state7.var_31);
        rb_state7.var_34 = (rb_state7.var_34) - (((0xf7d78090) - (rb_state7.var_32)) - (0x5b32ed73));
        rb_state7.var_35 = (rb_state7.var_35) ^ (((((0x4df19f15) - (rb_state7.var_34)) + (rb_state7.var_33)) - (rb_state7.var_19)) + (rb_state7.var_20));
        rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_35);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (1011 < rb_input_size ? (uint32_t)rb_input[1011] : 0x94e54c7a);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x67b24fcc)) {
        rb_state15.var_9 = 0x8d1af232;
        rb_state15.var_10 = (rb_state15.var_10) + (((((0x9cdada35) - (rb_state15.var_9)) + (rb_state15.var_9)) + (0x8066cfeb)) + (rb_state15.var_7));
        rb_state15.var_11 = (rb_state15.var_11) + (0xda3b0c96);
        rb_state15.var_12 = (rb_state15.var_12) - (((rb_state15.var_10) == (0x4345cd7c)) ? (rb_state15.var_8) : (0x4fa077b1));
        rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_11) == (0xda3b0c96)) ? (rb_state15.var_10) : (0xf23de7ab));
        rb_state15.var_14 = (rb_state15.var_14) - (((rb_state15.var_12) == (0x8c8810e8)) ? (rb_state15.var_11) : (0xe19b422));
        rb_state15.var_15 = (rb_state15.var_15) - (((0x4c17960c) ^ (rb_state15.var_12)) - (rb_state15.var_13));
        rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_13);
        rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_14) == (0x25c4f36a)) ? (rb_state15.var_14) : (0x472a4c2b));
        rb_state15.var_18 = (rb_state15.var_18) + (((rb_state15.var_15) == (0xfc1aaba0)) ? (rb_state15.var_16) : (0xf1d66e9d));
        rb_state15.var_19 = (rb_state15.var_19) - (((rb_state15.var_17) == (0xda3b0c96)) ? (rb_state15.var_18) : (0x18742437));
        rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
    #endif
    th->done = 0;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (1335 < rb_input_size ? (uint32_t)rb_input[1335] : 0x1c5327e1);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ ((0x1035b19d) ^ (0xcd8625b4));
    if ((rb_state4.var_0) == (0xddb39432)) {
        pthread_mutex_lock(&(rb_state4.lock_27));
        if ((rb_state4.var_1) != (0x0)) {
            rb_state4.var_28 = !((rb_state4.var_1) == (rb_state4.var_15));
        }
        pthread_mutex_unlock(&(rb_state4.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (2374 < rb_input_size ? (uint32_t)rb_input[2374] : 0x11c128f9);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xadfc9f21)) {
        rb_state13.var_11 = 0x78b14d38;
        rb_state13.var_12 = 0xf3103941;
        rb_state13.var_13 = (rb_state13.var_13) + (((((0x1fac74b7) ^ (0x1caf548a)) - (0x660f500a)) ^ (rb_state13.var_5)) ^ (rb_state13.var_12));
        rb_state13.var_14 = (rb_state13.var_14) - (((((0x1bdb0634) - (rb_state13.var_6)) - (rb_state13.var_13)) + (0x65ce7949)) - (rb_state13.var_11));
        rb_state13.var_10 = (rb_state13.var_10) - (rb_state13.var_14);
        rb_state13.var_1 = rb_state13.var_10;
    }
    if ((rb_state13.var_0) == (0xadfc9f21)) {
        pthread_mutex_lock(&(rb_state13.lock_20));
        rb_state13.var_16 = 0x28c13e16;
        rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_7) : (0xbd1f444d));
        rb_state13.var_18 = (rb_state13.var_18) + (((((0x5dd01341) + (rb_state13.var_10)) + (rb_state13.var_11)) + (rb_state13.var_8)) + (rb_state13.var_16));
        rb_state13.var_19 = (rb_state13.var_19) ^ (((((0x3b139a01) - (rb_state13.var_18)) + (rb_state13.var_17)) ^ (rb_state13.var_12)) - (rb_state13.var_13));
        rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_19);
        rb_state13.var_1 = rb_state13.var_15;
        pthread_mutex_unlock(&(rb_state13.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + ((0x6b45f6a4) ^ (rb_state14.var_0));
    #endif
    th->next = threads;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_32));
        if ((rb_state0.var_1) != (0x0)) {
            if (!((rb_state0.var_1) == (rb_state0.var_23))) {
                racebench_trigger(0);
            }
        }
        pthread_mutex_unlock(&(rb_state0.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_15));
        rb_state1.var_7 = 0x72d7a0ed;
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_7) : (0x2fd98cb1));
        rb_state1.var_9 = (rb_state1.var_9) ^ (3460 < rb_input_size ? (uint32_t)rb_input[3460] : 0x7fcd62fe);
        rb_state1.var_10 = (rb_state1.var_10) ^ (((0x14164fbc) + (rb_state1.var_3)) ^ (rb_state1.var_5));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_8);
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0xc3d04120));
        rb_state1.var_14 = (rb_state1.var_14) + (((((0x3baf8f94) ^ (rb_state1.var_12)) - (rb_state1.var_13)) + (0x14cf0d1f)) ^ (0x9c8477f3));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_14);
        pthread_mutex_unlock(&(rb_state1.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x53d4771a)) ? (rb_state3.var_0) : (0x92ed1733));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x286)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0x286)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (0xbaed8a8a);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xbfcc9e08)) {
        rb_state18.var_3 = 0x4667f737;
        rb_state18.var_4 = (rb_state18.var_4) + (((((0x302260d7) - (0x4f35630d)) + (rb_state18.var_3)) - (rb_state18.var_2)) + (rb_state18.var_4));
        rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_4);
    }
    #endif
    threads = th;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x63990496));
    #endif
    release_(&(threads_lock), file, line);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_0);
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_12 = 0xb9e63219;
        rb_state2.var_13 = (rb_state2.var_13) ^ (rb_state2.var_12);
        rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_9) : (0xaf389db2));
        rb_state2.var_15 = (rb_state2.var_15) ^ (((((0x2e2ea4e7) + (0x30e0e5b5)) + (rb_state2.var_11)) ^ (rb_state2.var_8)) - (rb_state2.var_13));
        rb_state2.var_16 = (rb_state2.var_16) ^ (((0x665339fa) + (rb_state2.var_14)) + (0x76c7e19c));
        rb_state2.var_17 = (rb_state2.var_17) + (rb_state2.var_15);
        rb_state2.var_18 = (rb_state2.var_18) - (((((0x5dfbdb5f) + (rb_state2.var_17)) - (rb_state2.var_16)) ^ (rb_state2.var_13)) ^ (rb_state2.var_12));
        rb_state2.var_11 = (rb_state2.var_11) + (rb_state2.var_18);
        rb_state2.var_1 = rb_state2.var_11;
    }
    if ((rb_state2.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_44));
        rb_state2.var_20 = 0xd1e0bb7;
        rb_state2.var_21 = (rb_state2.var_21) - (0x9e6c8eca);
        rb_state2.var_22 = (rb_state2.var_22) - (((rb_state2.var_15) == (0xa5295883)) ? (rb_state2.var_20) : (0xbd39fb1f));
        rb_state2.var_23 = (rb_state2.var_23) + (rb_state2.var_16);
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_17) == (0xa5295883)) ? (rb_state2.var_14) : (0x4e5e0137));
        rb_state2.var_25 = (rb_state2.var_25) + (((0xa89396a8) - (rb_state2.var_18)) + (rb_state2.var_21));
        rb_state2.var_26 = (rb_state2.var_26) + (((0x2ca49125) - (0x75510605)) - (rb_state2.var_22));
        rb_state2.var_27 = (rb_state2.var_27) ^ (rb_state2.var_23);
        rb_state2.var_28 = (rb_state2.var_28) + (((0xd77036da) + (rb_state2.var_24)) - (0x79fc780));
        rb_state2.var_29 = (rb_state2.var_29) ^ (((((0x10c42d41) ^ (rb_state2.var_25)) + (rb_state2.var_19)) ^ (rb_state2.var_26)) ^ (0xe34a2bf3));
        rb_state2.var_30 = (rb_state2.var_30) + (rb_state2.var_27);
        rb_state2.var_31 = (rb_state2.var_31) + (((0xfac2b7dc) - (rb_state2.var_20)) ^ (rb_state2.var_28));
        rb_state2.var_32 = (rb_state2.var_32) + (((0x3b467582) + (rb_state2.var_21)) + (rb_state2.var_29));
        rb_state2.var_33 = (rb_state2.var_33) + (((0x71f3b79) - (0x229c4e2d)) ^ (rb_state2.var_30));
        rb_state2.var_34 = (rb_state2.var_34) + (rb_state2.var_31);
        rb_state2.var_35 = (rb_state2.var_35) + (rb_state2.var_32);
        rb_state2.var_36 = (rb_state2.var_36) ^ (((((0x7a1a32ab) ^ (rb_state2.var_33)) - (rb_state2.var_34)) + (rb_state2.var_23)) - (rb_state2.var_22));
        rb_state2.var_37 = (rb_state2.var_37) - (((rb_state2.var_24) == (0x0)) ? (rb_state2.var_35) : (0x7aba6eda));
        rb_state2.var_38 = (rb_state2.var_38) - (rb_state2.var_36);
        rb_state2.var_39 = (rb_state2.var_39) - (((rb_state2.var_25) == (0x3031202a)) ? (rb_state2.var_37) : (0x232aa6d1));
        rb_state2.var_40 = (rb_state2.var_40) ^ (((rb_state2.var_26) == (0xc47196d7)) ? (rb_state2.var_38) : (0x60bffe6a));
        rb_state2.var_41 = (rb_state2.var_41) + (rb_state2.var_39);
        rb_state2.var_42 = (rb_state2.var_42) ^ (((rb_state2.var_27) == (0xdd1b1b96)) ? (rb_state2.var_40) : (0x1adb87f8));
        rb_state2.var_43 = (rb_state2.var_43) + (((((0x68eb6b8a) + (rb_state2.var_28)) ^ (rb_state2.var_41)) ^ (0x5fc9ef5c)) + (rb_state2.var_42));
        rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_43);
        rb_state2.var_1 = rb_state2.var_19;
        pthread_mutex_unlock(&(rb_state2.lock_44));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0xbaed8a8a)) ? (rb_state13.var_0) : (0x7d33ff83));
    #endif
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