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
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0xc18d5d4f)) {
        rb_state14.var_15 = 0xb7d362b;
        rb_state14.var_16 = 0x65478dfc;
        rb_state14.var_17 = 0x426960f;
        rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_15) : (0xfde6c46d));
        rb_state14.var_19 = (rb_state14.var_19) ^ (((0xe8fe72bc) - (rb_state14.var_8)) ^ (rb_state14.var_17));
        rb_state14.var_20 = (rb_state14.var_20) - (((0x75e31170) - (rb_state14.var_9)) + (rb_state14.var_16));
        rb_state14.var_21 = (rb_state14.var_21) + (((rb_state14.var_18) == (0xb7d362b)) ? (rb_state14.var_19) : (0xcda46c7e));
        rb_state14.var_22 = (rb_state14.var_22) ^ (((rb_state14.var_20) == (0x24d56094)) ? (rb_state14.var_21) : (0x1f3dec41));
        rb_state14.var_14 = (rb_state14.var_14) + (rb_state14.var_22);
        rb_state14.var_2 = rb_state14.var_14;
    }
    if ((rb_state14.var_1) == (0xc18d5d4f)) {
        pthread_mutex_lock(&(rb_state14.lock_28));
        rb_state14.var_24 = 0x628da39e;
        rb_state14.var_25 = (rb_state14.var_25) + (((0x8abd474b) + (rb_state14.var_11)) + (rb_state14.var_12));
        rb_state14.var_26 = (rb_state14.var_26) + (((((0x1650f9c7) + (rb_state14.var_13)) - (rb_state14.var_24)) - (rb_state14.var_10)) - (0x51d09211));
        rb_state14.var_27 = (rb_state14.var_27) - (((rb_state14.var_25) == (0x8abd474b)) ? (rb_state14.var_26) : (0xd736f06f));
        rb_state14.var_23 = (rb_state14.var_23) + (rb_state14.var_27);
        rb_state14.var_2 = rb_state14.var_23;
        pthread_mutex_unlock(&(rb_state14.lock_28));
    }
    rb_state14.var_18 = (rb_state14.var_18) ^ (((rb_state14.var_17) == (0x426960f)) ? (rb_state14.var_16) : (0xd0f413c3));
    #endif
    int ret = pthread_mutex_lock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_lock");
    }
}

void release_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x607190da));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x83f48d7f)) {
        if ((rb_state12.var_2) != (0x0)) {
            if (!((rb_state12.var_2) == (rb_state12.var_15))) {
                racebench_trigger(12);
            }
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
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) - (0x4d303e05);
        #endif
        bolt->value = val;
    } else if (op == BY) {
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x62)) ? (rb_state19.var_0) : (0x10587294));
        #endif
        bolt->value += val;
    }
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xadf2485f)) {
        rb_state6.var_3 = 0x31c9b495;
        rb_state6.var_4 = 0x93d363c;
        rb_state6.var_5 = (rb_state6.var_5) ^ (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_2) : (0x4733910e));
        rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_4);
        rb_state6.var_7 = (rb_state6.var_7) + (((0xe0e6a478) ^ (rb_state6.var_4)) ^ (0x896937a7));
        rb_state6.var_8 = (rb_state6.var_8) - (rb_state6.var_3);
        rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_5);
        rb_state6.var_10 = (rb_state6.var_10) - (((0xa8d35dae) - (0x6da09c7d)) + (rb_state6.var_6));
        rb_state6.var_11 = (rb_state6.var_11) - (((0x48d193a5) - (rb_state6.var_7)) + (0x494880e3));
        rb_state6.var_12 = (rb_state6.var_12) - (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_8) : (0xa72be2e5));
        rb_state6.var_13 = (rb_state6.var_13) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_9) : (0xe86fc7de));
        rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_10) : (0x5d075f03));
        rb_state6.var_15 = (rb_state6.var_15) - (((0xcb106b02) + (0x96fdb9db)) + (rb_state6.var_11));
        rb_state6.var_16 = (rb_state6.var_16) - (rb_state6.var_12);
        rb_state6.var_17 = (rb_state6.var_17) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_13) : (0x5f3f7de3));
        rb_state6.var_18 = (rb_state6.var_18) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_14) : (0xdc62958e));
        rb_state6.var_19 = (rb_state6.var_19) - (((0xe367c51f) + (rb_state6.var_10)) ^ (rb_state6.var_15));
        rb_state6.var_20 = (rb_state6.var_20) + (rb_state6.var_16);
        rb_state6.var_21 = (rb_state6.var_21) - (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_17) : (0x3fd9f4f1));
        rb_state6.var_22 = (rb_state6.var_22) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_18) : (0xee280358));
        rb_state6.var_23 = (rb_state6.var_23) + (((0x1d2ddda9) + (0x7f6dd6ed)) - (rb_state6.var_19));
        rb_state6.var_24 = (rb_state6.var_24) - (((((0x7e367919) + (rb_state6.var_21)) ^ (rb_state6.var_13)) + (0x264f220d)) + (rb_state6.var_20));
        rb_state6.var_25 = (rb_state6.var_25) ^ (((rb_state6.var_22) == (0x0)) ? (rb_state6.var_23) : (0xa8d51128));
        rb_state6.var_26 = (rb_state6.var_26) ^ (((0x8077434d) + (0xfefb1a0f)) - (rb_state6.var_24));
        rb_state6.var_27 = (rb_state6.var_27) - (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_25) : (0x2914fb9b));
        rb_state6.var_28 = (rb_state6.var_28) + (((rb_state6.var_26) == (0x0)) ? (rb_state6.var_27) : (0xa1537fce));
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_19) == (0xecd8e4b3)) {
        pthread_mutex_lock(&(rb_state14.lock_44));
        rb_state14.var_32 = 0x9eb7cd0f;
        rb_state14.var_33 = 0xe85069b9;
        rb_state14.var_34 = 0xec2b36dd;
        rb_state14.var_35 = (rb_state14.var_35) ^ (351 < rb_input_size ? (uint32_t)rb_input[351] : 0x14a6b93c);
        rb_state14.var_36 = (rb_state14.var_36) + (((((0x6187445c) + (rb_state14.var_34)) + (rb_state14.var_20)) ^ (rb_state14.var_32)) ^ (0x6d023a9b));
        rb_state14.var_37 = (rb_state14.var_37) ^ (((0x6338d6fb) - (rb_state14.var_33)) - (rb_state14.var_21));
        rb_state14.var_38 = (rb_state14.var_38) - (((((0x55f5c55d) ^ (0xacd8bc60)) + (rb_state14.var_35)) ^ (0x40ccfd34)) ^ (rb_state14.var_36));
        rb_state14.var_39 = (rb_state14.var_39) - (((rb_state14.var_22) == (0xf3e508f2)) ? (rb_state14.var_37) : (0xa9c03a32));
        rb_state14.var_40 = (rb_state14.var_40) + (((0xab0684ac) + (rb_state14.var_38)) - (0xb3c39803));
        rb_state14.var_41 = (rb_state14.var_41) - (rb_state14.var_39);
        rb_state14.var_42 = (rb_state14.var_42) ^ (rb_state14.var_40);
        rb_state14.var_43 = (rb_state14.var_43) + (((((0x46459173) + (rb_state14.var_23)) ^ (rb_state14.var_24)) + (rb_state14.var_42)) - (rb_state14.var_41));
        rb_state14.var_30 = (rb_state14.var_30) - (rb_state14.var_43);
        rb_state14.var_31 = rb_state14.var_30;
        pthread_mutex_unlock(&(rb_state14.lock_44));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_1);
    #endif
    int ret = pthread_cond_broadcast(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_broadcast");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x99eb92b4)) ? (rb_state0.var_0) : (0xc97f1cfa));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0x90428def);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x9d229083)) ? (rb_state8.var_0) : (0x25baed83));
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
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_20) == (0x1)) {
                rb_state13.var_45 = 0x20bddbee;
                rb_state13.var_46 = 0x7aec5961;
                rb_state13.var_47 = (rb_state13.var_47) + (((rb_state13.var_27) == (0x0)) ? (rb_state13.var_46) : (0xa24d0185));
                rb_state13.var_48 = (rb_state13.var_48) ^ (((rb_state13.var_45) == (0x0)) ? (rb_state13.var_47) : (0xc9d98cf5));
                rb_state13.var_35 = (rb_state13.var_35) + (rb_state13.var_48);
            }
            #endif
            #ifdef RACEBENCH_BUG_16
            if ((rb_state16.var_1) == (0x0)) {
                rb_state16.var_17 = 0xb404972e;
                rb_state16.var_18 = 0x829d0ddd;
                rb_state16.var_19 = (rb_state16.var_19) - (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_17) : (0xab7ac9fe));
                rb_state16.var_20 = (rb_state16.var_20) + (0xfabd0af2);
                rb_state16.var_21 = (rb_state16.var_21) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_11) : (0xbbf7426f));
                rb_state16.var_22 = (rb_state16.var_22) - (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_19) : (0xa4832940));
                rb_state16.var_23 = (rb_state16.var_23) + (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_21) : (0x42d9d29d));
                rb_state16.var_24 = (rb_state16.var_24) + (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_22) : (0x9bdd97ec));
                rb_state16.var_25 = (rb_state16.var_25) + (((((0xdb55fcb4) - (rb_state16.var_16)) + (rb_state16.var_23)) + (rb_state16.var_24)) - (rb_state16.var_15));
                rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_25);
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0x126)) {
                pthread_mutex_lock(&(rb_state19.lock_48));
                rb_state19.var_32 = 0x7f9222fb;
                rb_state19.var_33 = 0x640be2ab;
                rb_state19.var_34 = (rb_state19.var_34) + (3793 < rb_input_size ? (uint32_t)rb_input[3793] : 0x26077fe6);
                rb_state19.var_35 = (rb_state19.var_35) - (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_32) : (0x3553bb1d));
                rb_state19.var_36 = (rb_state19.var_36) ^ (rb_state19.var_17);
                rb_state19.var_37 = (rb_state19.var_37) - (rb_state19.var_33);
                rb_state19.var_38 = (rb_state19.var_38) ^ (((0xed64d468) ^ (rb_state19.var_19)) ^ (rb_state19.var_34));
                rb_state19.var_39 = (rb_state19.var_39) + (((0xa0e8ec75) ^ (rb_state19.var_35)) ^ (0x23ce2a7a));
                rb_state19.var_40 = (rb_state19.var_40) + (rb_state19.var_36);
                rb_state19.var_41 = (rb_state19.var_41) - (((((0x621c2ac4) - (0xa7e4294f)) - (0xa0ea076f)) + (rb_state19.var_37)) - (rb_state19.var_38));
                rb_state19.var_42 = (rb_state19.var_42) + (rb_state19.var_39);
                rb_state19.var_43 = (rb_state19.var_43) ^ (((0x3538cd59) ^ (rb_state19.var_20)) ^ (rb_state19.var_40));
                rb_state19.var_44 = (rb_state19.var_44) ^ (((rb_state19.var_41) == (0x0)) ? (rb_state19.var_42) : (0x5ae8a744));
                rb_state19.var_45 = (rb_state19.var_45) + (rb_state19.var_43);
                rb_state19.var_46 = (rb_state19.var_46) - (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_44) : (0x766b6f84));
                rb_state19.var_47 = (rb_state19.var_47) ^ (((((0x91aef8e3) - (rb_state19.var_45)) - (rb_state19.var_46)) ^ (rb_state19.var_22)) + (rb_state19.var_23));
                rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_47);
                pthread_mutex_unlock(&(rb_state19.lock_48));
            }
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
    struct capsule *capsule = arg;

    pthread_t me = pthread_self();
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xadf2485f)) {
        rb_state6.var_2 = rb_state6.var_0;
    }
    if ((rb_state6.var_1) == (0xadf2485f)) {
        rb_state6.var_30 = !((rb_state6.var_0) == (rb_state6.var_2));
    }
    rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_16) : (0x125dae10));
    rb_state6.var_18 = (rb_state6.var_18) - (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_19) : (0x507e9e6a));
    if ((rb_state6.var_21) == (0xc0260b0f)) {
        rb_state6.var_31 = rb_state6.var_22;
    }
    if ((rb_state6.var_21) == (0xc0260b0f)) {
        rb_state6.var_40 = !((rb_state6.var_22) == (rb_state6.var_31));
    }
    #endif
    possess_(&(threads_lock), capsule->file, capsule->line);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x58fe9c33)) ? (rb_state5.var_0) : (0xb7c4da2e));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (3074 < rb_input_size ? (uint32_t)rb_input[3074] : 0x9c3ea716);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_0);
    #endif
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

    match->done = 1;
    if (threads != match) {
        *prior = match->next;
        match->next = threads;
        threads = match;
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0xff46a7e0);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x0)) {
        if ((rb_state9.var_1) != (0x0)) {
            if (!((rb_state9.var_1) == (rb_state9.var_21))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    twist_(&(threads_lock), BY, +1, capsule->file, capsule->line);

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_1) : (0x7591f165));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_21) == (0xc0260b0f)) {
        rb_state6.var_32 = 0x1ba3dc09;
        rb_state6.var_33 = 0x542c2ca;
        rb_state6.var_34 = (rb_state6.var_34) - (rb_state6.var_33);
        rb_state6.var_35 = (rb_state6.var_35) + (rb_state6.var_24);
        rb_state6.var_36 = (rb_state6.var_36) ^ (((((0x96a596ff) + (0x34c470e7)) ^ (0xffdbbb9)) + (rb_state6.var_23)) ^ (rb_state6.var_32));
        rb_state6.var_37 = (rb_state6.var_37) + (((((0x8d04edc5) + (rb_state6.var_25)) ^ (0xf22afcd9)) ^ (rb_state6.var_34)) - (rb_state6.var_35));
        rb_state6.var_38 = (rb_state6.var_38) - (((((0xa10c3576) - (rb_state6.var_26)) + (rb_state6.var_36)) + (rb_state6.var_27)) ^ (rb_state6.var_37));
        rb_state6.var_22 = (rb_state6.var_22) + (rb_state6.var_38);
    }
    rb_state6.var_28 = (rb_state6.var_28) ^ ((0x6271a7e6) - (rb_state6.var_30));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (311 < rb_input_size ? (uint32_t)rb_input[311] : 0x10ee9b76);
    #endif
    my_free(capsule);
}

local void *ignition(void *arg) {
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x13e222a6)) {
        rb_state1.var_9 = 0xb9e8d24e;
        rb_state1.var_10 = (rb_state1.var_10) - (0x29eec8d7);
        rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x7daa0250));
        rb_state1.var_12 = (rb_state1.var_12) + (rb_state1.var_5);
        rb_state1.var_13 = (rb_state1.var_13) - (((((0x6d916434) - (rb_state1.var_1)) + (rb_state1.var_2)) - (0x3ac54748)) + (0xc4964c24));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0xb1b8c89c));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_12) : (0xe9a44b6a));
        rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0xdbedfdde));
        rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_15) : (0x67903a47));
        rb_state1.var_18 = (rb_state1.var_18) ^ (((((0x7bd331fd) + (rb_state1.var_7)) ^ (rb_state1.var_17)) ^ (0x1e976a61)) - (rb_state1.var_16));
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_19));
        rb_state4.var_4 = 0xacb1097f;
        rb_state4.var_5 = 0x4b3fc92e;
        rb_state4.var_6 = (rb_state4.var_6) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_3) : (0x9589046b));
        rb_state4.var_7 = (rb_state4.var_7) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_2) : (0xda44c4a6));
        rb_state4.var_8 = (rb_state4.var_8) - (((0xac1967a1) - (rb_state4.var_5)) - (rb_state4.var_6));
        rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_6) : (0x9c195fa5));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((((0xce681491) - (rb_state4.var_7)) + (rb_state4.var_8)) ^ (rb_state4.var_7)) - (0x9ac658c3));
        rb_state4.var_11 = (rb_state4.var_11) - (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0xdb9e3b61));
        rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_11);
        rb_state4.var_2 = rb_state4.var_3;
        pthread_mutex_unlock(&(rb_state4.lock_19));
    }
    if ((rb_state4.var_0) == (0x0)) {
        if (!((rb_state4.var_2) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0xceda136b)) {
        rb_state5.var_10 = 0x217681f;
        rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_5) : (0xa069caee));
        rb_state5.var_12 = (rb_state5.var_12) + (0xbc4efe9c);
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_10) : (0x40781404));
        rb_state5.var_14 = (rb_state5.var_14) + (rb_state5.var_6);
        rb_state5.var_15 = (rb_state5.var_15) + (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_11) : (0xd384bfd4));
        rb_state5.var_16 = (rb_state5.var_16) - (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_12) : (0xc9b917a7));
        rb_state5.var_17 = (rb_state5.var_17) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_14) : (0xf24e296));
        rb_state5.var_18 = (rb_state5.var_18) - (((((0x3305f9f9) + (rb_state5.var_15)) ^ (0xe1f78bbe)) + (rb_state5.var_16)) - (rb_state5.var_11));
        rb_state5.var_19 = (rb_state5.var_19) ^ (((0xb4633651) + (rb_state5.var_17)) - (0xdd60b698));
        rb_state5.var_20 = (rb_state5.var_20) + (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_18) : (0xcef67156));
        rb_state5.var_21 = (rb_state5.var_21) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_19) : (0x87bda764));
        rb_state5.var_22 = (rb_state5.var_22) ^ (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_20) : (0xfdc6512));
        rb_state5.var_23 = (rb_state5.var_23) - (((rb_state5.var_21) == (0x0)) ? (rb_state5.var_22) : (0xf2835afd));
        rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (1763 < rb_input_size ? (uint32_t)rb_input[1763] : 0x64d44808);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0xa7a7ed4)) ? (rb_state17.var_0) : (0x64c38e1a));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (1568 < rb_input_size ? (uint32_t)rb_input[1568] : 0x2a5ce65a);
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x2ec945e5)) {
        rb_state0.var_7 = (rb_state0.var_7) - (((0xe5219935) + (rb_state0.var_4)) + (rb_state0.var_7));
        rb_state0.var_8 = (rb_state0.var_8) + (((0xda60b9e7) - (0xda767817)) + (rb_state0.var_5));
        rb_state0.var_9 = (rb_state0.var_9) - ((0x84d8c7d0) ^ (0x650eadb5));
        rb_state0.var_10 = (rb_state0.var_10) - (0xd95f9cbb);
        rb_state0.var_11 = (rb_state0.var_11) - (rb_state0.var_6);
        rb_state0.var_12 = (rb_state0.var_12) + (((0xed9e6b0f) + (rb_state0.var_7)) - (0x59f3966c));
        rb_state0.var_13 = (rb_state0.var_13) + (((0x5b06476d) ^ (0xafdc4e75)) ^ (rb_state0.var_8));
        rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_9);
        rb_state0.var_15 = (rb_state0.var_15) + (rb_state0.var_10);
        rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_11);
        rb_state0.var_17 = (rb_state0.var_17) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_12) : (0x1fdf800d));
        rb_state0.var_18 = (rb_state0.var_18) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_13) : (0x3e7465d9));
        rb_state0.var_19 = (rb_state0.var_19) + (((((0x72c80f0f) + (rb_state0.var_15)) ^ (rb_state0.var_14)) - (0xd0ba2b2c)) - (0x92141168));
        rb_state0.var_20 = (rb_state0.var_20) + (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_17) : (0x25a47040));
        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_18) : (0xb1cc9479));
        rb_state0.var_22 = (rb_state0.var_22) - (rb_state0.var_19);
        rb_state0.var_23 = (rb_state0.var_23) + (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_20) : (0x83087cbc));
        rb_state0.var_24 = (rb_state0.var_24) + (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_21) : (0x4c3626a));
        rb_state0.var_25 = (rb_state0.var_25) - (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_23) : (0x31f3389a));
        rb_state0.var_26 = (rb_state0.var_26) - (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_25) : (0x6218ac80));
        rb_state0.var_6 = (rb_state0.var_6) + (rb_state0.var_26);
        rb_state0.var_1 = rb_state0.var_6;
    }
    if ((rb_state0.var_0) == (0x3526102e)) {
        pthread_mutex_lock(&(rb_state0.lock_55));
        rb_state0.var_28 = 0x8a6a1db2;
        rb_state0.var_29 = (rb_state0.var_29) ^ (156 < rb_input_size ? (uint32_t)rb_input[156] : 0x3bb11911);
        rb_state0.var_30 = (rb_state0.var_30) + (rb_state0.var_15);
        rb_state0.var_31 = (rb_state0.var_31) ^ (((((0x9a821eb1) + (rb_state0.var_14)) ^ (rb_state0.var_13)) + (rb_state0.var_16)) - (0x2d9c399d));
        rb_state0.var_32 = (rb_state0.var_32) - (rb_state0.var_28);
        rb_state0.var_33 = (rb_state0.var_33) ^ (rb_state0.var_29);
        rb_state0.var_34 = (rb_state0.var_34) + (((0xbbeac709) ^ (0xb5810818)) ^ (rb_state0.var_30));
        rb_state0.var_35 = (rb_state0.var_35) - (rb_state0.var_31);
        rb_state0.var_36 = (rb_state0.var_36) - (rb_state0.var_32);
        rb_state0.var_37 = (rb_state0.var_37) + (rb_state0.var_33);
        rb_state0.var_38 = (rb_state0.var_38) - (((0x7893d178) + (rb_state0.var_17)) - (rb_state0.var_34));
        rb_state0.var_39 = (rb_state0.var_39) + (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_35) : (0xc5741924));
        rb_state0.var_40 = (rb_state0.var_40) ^ (rb_state0.var_36);
        rb_state0.var_41 = (rb_state0.var_41) ^ (rb_state0.var_37);
        rb_state0.var_42 = (rb_state0.var_42) - (((0x60569be3) + (rb_state0.var_38)) - (rb_state0.var_19));
        rb_state0.var_43 = (rb_state0.var_43) ^ (((0x5050b87) ^ (0x36ec51f5)) + (rb_state0.var_39));
        rb_state0.var_44 = (rb_state0.var_44) + (rb_state0.var_40);
        rb_state0.var_45 = (rb_state0.var_45) + (((0x44ce54cd) + (rb_state0.var_20)) + (rb_state0.var_41));
        rb_state0.var_46 = (rb_state0.var_46) ^ (((rb_state0.var_42) == (0x0)) ? (rb_state0.var_43) : (0xd8cfa87a));
        rb_state0.var_47 = (rb_state0.var_47) ^ (((((0x7d87d8e2) + (rb_state0.var_44)) ^ (rb_state0.var_21)) - (0x17e3289f)) ^ (rb_state0.var_45));
        rb_state0.var_48 = (rb_state0.var_48) ^ (((0x6af64927) + (0xec89178a)) + (rb_state0.var_46));
        rb_state0.var_49 = (rb_state0.var_49) - (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_47) : (0x8c0a618d));
        rb_state0.var_50 = (rb_state0.var_50) ^ (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_48) : (0x325a33b2));
        rb_state0.var_51 = (rb_state0.var_51) + (((0x1d202261) - (0xf7208a91)) + (rb_state0.var_49));
        rb_state0.var_52 = (rb_state0.var_52) - (((0xc1e4f0c7) - (0xd24880eb)) + (rb_state0.var_50));
        rb_state0.var_53 = (rb_state0.var_53) ^ (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_51) : (0xb2bea743));
        rb_state0.var_54 = (rb_state0.var_54) + (((rb_state0.var_52) == (0x0)) ? (rb_state0.var_53) : (0xcb7ef940));
        rb_state0.var_27 = (rb_state0.var_27) + (rb_state0.var_54);
        rb_state0.var_1 = rb_state0.var_27;
        pthread_mutex_unlock(&(rb_state0.lock_55));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_31) == (0xee280358)) {
        pthread_mutex_lock(&(rb_state6.lock_66));
        rb_state6.var_43 = 0xc906cfbf;
        rb_state6.var_44 = (rb_state6.var_44) ^ (rb_state6.var_34);
        rb_state6.var_45 = (rb_state6.var_45) ^ (((0x2b1d0971) - (rb_state6.var_33)) - (0xcbb44c7d));
        rb_state6.var_46 = (rb_state6.var_46) - (((((0xc6fda60b) + (rb_state6.var_35)) + (rb_state6.var_43)) + (rb_state6.var_32)) - (0xf494ca3b));
        rb_state6.var_47 = (rb_state6.var_47) - (((0x49656c79) + (rb_state6.var_36)) ^ (rb_state6.var_44));
        rb_state6.var_48 = (rb_state6.var_48) ^ (((0x6ac9affc) ^ (rb_state6.var_45)) ^ (0x8ac7824b));
        rb_state6.var_49 = (rb_state6.var_49) + (rb_state6.var_46);
        rb_state6.var_50 = (rb_state6.var_50) ^ (((rb_state6.var_37) == (0x0)) ? (rb_state6.var_47) : (0x7a8d734b));
        rb_state6.var_51 = (rb_state6.var_51) ^ (((rb_state6.var_38) == (0x0)) ? (rb_state6.var_48) : (0x5e141f13));
        rb_state6.var_52 = (rb_state6.var_52) + (((rb_state6.var_40) == (0x0)) ? (rb_state6.var_49) : (0xf3ba6be5));
        rb_state6.var_53 = (rb_state6.var_53) - (((rb_state6.var_50) == (0x0)) ? (rb_state6.var_51) : (0xe97d1acb));
        rb_state6.var_54 = (rb_state6.var_54) ^ (rb_state6.var_52);
        rb_state6.var_55 = (rb_state6.var_55) - (rb_state6.var_53);
        rb_state6.var_56 = (rb_state6.var_56) ^ (((((0xee470934) + (0x5f986ab)) + (rb_state6.var_54)) + (rb_state6.var_55)) + (0x3df31723));
        rb_state6.var_42 = (rb_state6.var_42) - (rb_state6.var_56);
        rb_state6.var_41 = rb_state6.var_42;
        pthread_mutex_unlock(&(rb_state6.lock_66));
    }
    if ((rb_state6.var_31) == (0xee280358)) {
        if (!((rb_state6.var_41) == (rb_state6.var_42))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + ((0x36db644e) - (rb_state11.var_0));
    rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_11) == (0x26d6be49)) {
        rb_state13.var_20 = 0x1;
    }
    if ((rb_state13.var_11) == (0x26d6be49)) {
        pthread_mutex_lock(&(rb_state13.lock_33));
        rb_state13.var_21 = 0xf56f7547;
        rb_state13.var_22 = (rb_state13.var_22) - (rb_state13.var_13);
        rb_state13.var_23 = (rb_state13.var_23) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_21) : (0x870d12b7));
        rb_state13.var_24 = (rb_state13.var_24) + (0xeba4938e);
        rb_state13.var_25 = (rb_state13.var_25) - (rb_state13.var_22);
        rb_state13.var_26 = (rb_state13.var_26) ^ (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_23) : (0xe7ff5034));
        rb_state13.var_27 = (rb_state13.var_27) - (((0xe58e5698) ^ (rb_state13.var_15)) ^ (rb_state13.var_24));
        rb_state13.var_28 = (rb_state13.var_28) - (rb_state13.var_25);
        rb_state13.var_29 = (rb_state13.var_29) - (rb_state13.var_26);
        rb_state13.var_30 = (rb_state13.var_30) - (((0x2ceb4ec0) ^ (rb_state13.var_17)) ^ (rb_state13.var_27));
        rb_state13.var_31 = (rb_state13.var_31) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_29) : (0x81fd9a72));
        rb_state13.var_32 = (rb_state13.var_32) + (((((0x871a1c4) + (rb_state13.var_18)) + (rb_state13.var_19)) + (rb_state13.var_30)) - (rb_state13.var_31));
        rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_32);
        rb_state13.var_18 = rb_state13.var_19;
        pthread_mutex_unlock(&(rb_state13.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (922 < rb_input_size ? (uint32_t)rb_input[922] : 0x5497e6e4);
    rb_state14.var_0 = (rb_state14.var_0) - (3488 < rb_input_size ? (uint32_t)rb_input[3488] : 0x9da65271);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
    #endif
    pthread_cleanup_push(reenter, arg);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0xdc63c3a5) ^ (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        if ((rb_state10.var_2) != (0x0)) {
            if (!((rb_state10.var_2) == (rb_state10.var_17))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + ((0xbb98ce93) ^ (0xd6f43086));
    rb_state13.var_22 = (rb_state13.var_22) + (2817 < rb_input_size ? (uint32_t)rb_input[2817] : 0xc1874faf);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_1);
    if ((rb_state17.var_1) == (0x0)) {
        rb_state17.var_16 = 0x6932c232;
        rb_state17.var_17 = (rb_state17.var_17) - ((0xb69e21b2) - (rb_state17.var_9));
        rb_state17.var_18 = (rb_state17.var_18) - (rb_state17.var_10);
        rb_state17.var_19 = (rb_state17.var_19) - ((0xd1f4d14) ^ (0xd8cb898));
        rb_state17.var_20 = (rb_state17.var_20) + (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_8) : (0xd34d870d));
        rb_state17.var_21 = (rb_state17.var_21) - (((0x462e2778) ^ (0xdfce5de8)) - (rb_state17.var_16));
        rb_state17.var_22 = (rb_state17.var_22) - (((0xe6148038) ^ (rb_state17.var_12)) ^ (rb_state17.var_7));
        rb_state17.var_23 = (rb_state17.var_23) ^ (((rb_state17.var_17) == (0x4961de4e)) ? (rb_state17.var_18) : (0x6e85d6e6));
        rb_state17.var_24 = (rb_state17.var_24) + (((0x9482d23f) ^ (rb_state17.var_19)) + (0x8c305a13));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_20) : (0x35b1cf40));
        rb_state17.var_26 = (rb_state17.var_26) ^ (((rb_state17.var_21) == (0xcf5247a2)) ? (rb_state17.var_22) : (0x664bfd0b));
        rb_state17.var_27 = (rb_state17.var_27) + (((rb_state17.var_23) == (0xbff9409c)) ? (rb_state17.var_24) : (0x47a8d9be));
        rb_state17.var_28 = (rb_state17.var_28) ^ (((((0xbf1bf8a) ^ (0x609bb38e)) - (rb_state17.var_25)) - (rb_state17.var_26)) - (0x785260ab));
        rb_state17.var_29 = (rb_state17.var_29) - (((rb_state17.var_14) == (0x4006bf64)) ? (rb_state17.var_27) : (0xe3bbdf36));
        rb_state17.var_30 = (rb_state17.var_30) - (((rb_state17.var_28) == (0xd930aa19)) ? (rb_state17.var_29) : (0x4aa5b5e1));
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_30);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x0)) {
        if ((rb_state18.var_2) != (0x0)) {
            if (!((rb_state18.var_2) == (rb_state18.var_18))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    capsule->probe(capsule->payload);

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (0x39fb6d75);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_0);
    #endif
    pthread_cleanup_pop(1);

    return NULL;
}

thread *launch_(void (*probe)(void *), void *payload, char const *file, long line) {

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (0xe70e0b03);
    #endif
    struct capsule *capsule = my_malloc(sizeof(struct capsule), file, line);
    capsule->probe = probe;
    capsule->payload = payload;
    capsule->file = file;
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_1);
    #endif
    capsule->line = line;

    possess_(&(threads_lock), file, line);

    thread *th = my_malloc(sizeof(struct thread_s), file, line);
    pthread_attr_t attr;
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_10 = (rb_state13.var_10) ^ (2593 < rb_input_size ? (uint32_t)rb_input[2593] : 0x156ec7b1);
    #endif
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
    rb_state1.var_0 = (rb_state1.var_0) - (670 < rb_input_size ? (uint32_t)rb_input[670] : 0x2bb180cb);
    if ((rb_state1.var_0) == (0x13e222a6)) {
        pthread_mutex_lock(&(rb_state1.lock_19));
        rb_state1.var_3 = 0x9ecf3326;
        rb_state1.var_4 = 0xf8f93bf;
        rb_state1.var_5 = 0x4699898e;
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x5175d09a));
        rb_state1.var_7 = (rb_state1.var_7) - (rb_state1.var_5);
        rb_state1.var_8 = (rb_state1.var_8) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0x4ce3e1ed));
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_8);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_19));
    }
    if ((rb_state1.var_0) == (0x13e222a6)) {
        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        rb_state4.var_12 = 0xf955c86d;
        rb_state4.var_13 = 0xa00cc9f1;
        rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_12) : (0x854187ec));
        rb_state4.var_15 = (rb_state4.var_15) - (rb_state4.var_9);
        rb_state4.var_16 = (rb_state4.var_16) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_14) : (0x5049f8fb));
        rb_state4.var_17 = (rb_state4.var_17) - (rb_state4.var_15);
        rb_state4.var_18 = (rb_state4.var_18) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_17) : (0x3d28bc1a));
        rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0xceda136b)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_4 = 0xb6f98013;
        rb_state5.var_5 = (rb_state5.var_5) + (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_2) : (0x2b3ba288));
        rb_state5.var_6 = (rb_state5.var_6) + (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_4) : (0x15e4e7b2));
        rb_state5.var_7 = (rb_state5.var_7) - (((0x51c4e82f) - (0x68c4279e)) - (rb_state5.var_5));
        rb_state5.var_8 = (rb_state5.var_8) - (rb_state5.var_6);
        rb_state5.var_9 = (rb_state5.var_9) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_8) : (0xc01637bb));
        rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_9);
        rb_state5.var_2 = rb_state5.var_3;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    if ((rb_state5.var_1) == (0xceda136b)) {
        if (!((rb_state5.var_2) == (rb_state5.var_3))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0xa809241) + (0x424e95c2));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (3835 < rb_input_size ? (uint32_t)rb_input[3835] : 0xd8ec485b);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (3593 < rb_input_size ? (uint32_t)rb_input[3593] : 0xce9b9547);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (3926 < rb_input_size ? (uint32_t)rb_input[3926] : 0xc7bbd39a);
    rb_state17.var_0 = (rb_state17.var_0) - (0xf58581e5);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x0)) {
        rb_state18.var_12 = 0x7971b193;
        rb_state18.var_13 = 0x92621976;
        rb_state18.var_14 = (rb_state18.var_14) + (((((0xa63ba93a) ^ (0xb0f9c3d8)) + (rb_state18.var_7)) + (rb_state18.var_13)) + (rb_state18.var_8));
        rb_state18.var_15 = (rb_state18.var_15) - (rb_state18.var_12);
        rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_14);
        rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_16) : (0x48c3398));
        rb_state18.var_11 = (rb_state18.var_11) - (rb_state18.var_17);
        rb_state18.var_2 = rb_state18.var_11;
    }
    if ((rb_state18.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_28));
        rb_state18.var_19 = 0x35204600;
        rb_state18.var_20 = 0xec83843e;
        rb_state18.var_21 = (rb_state18.var_21) + (((((0x9129e6ef) ^ (0xbef7731a)) - (0x431e83d2)) + (rb_state18.var_19)) + (rb_state18.var_20));
        rb_state18.var_22 = (rb_state18.var_22) + (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_9) : (0x302824));
        rb_state18.var_23 = (rb_state18.var_23) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_21) : (0x23a0aa55));
        rb_state18.var_24 = (rb_state18.var_24) - (((0x2ade191) - (rb_state18.var_22)) ^ (rb_state18.var_12));
        rb_state18.var_25 = (rb_state18.var_25) ^ (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_23) : (0x88130f75));
        rb_state18.var_26 = (rb_state18.var_26) + (((0xb83d9ff4) - (rb_state18.var_14)) + (rb_state18.var_24));
        rb_state18.var_27 = (rb_state18.var_27) ^ (((rb_state18.var_25) == (0x0)) ? (rb_state18.var_26) : (0xd6cabcba));
        rb_state18.var_18 = (rb_state18.var_18) ^ (rb_state18.var_27);
        rb_state18.var_2 = rb_state18.var_18;
        pthread_mutex_unlock(&(rb_state18.lock_28));
    }
    #endif
    ret = pthread_attr_destroy(&attr);
    if (ret) {
        fail(ret, file, line, "attr_destroy");
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0x3fa9865f) + (0xd5e1963e));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_18));
        rb_state2.var_4 = (rb_state2.var_4) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_4) : (0x108dd89b));
        rb_state2.var_5 = (rb_state2.var_5) - (((((0x9ee9230c) + (rb_state2.var_2)) - (rb_state2.var_5)) ^ (rb_state2.var_4)) ^ (rb_state2.var_3));
        rb_state2.var_6 = (rb_state2.var_6) - (rb_state2.var_4);
        rb_state2.var_7 = (rb_state2.var_7) ^ (((((0xceedd6cf) - (rb_state2.var_6)) - (0xeec53c84)) - (0x30fdf91a)) ^ (rb_state2.var_5));
        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_7);
        rb_state2.var_2 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_18));
    }
    if ((rb_state2.var_1) == (0x0)) {
        if (!((rb_state2.var_2) == (rb_state2.var_3))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0xda45127d)) {
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_2);
    }
    rb_state8.var_5 = (rb_state8.var_5) + ((0x77346e54) - (rb_state8.var_4));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x4b47a4f9));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        rb_state10.var_11 = 0x7ec428d0;
        rb_state10.var_12 = 0xa7346400;
        rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0xc8af7748));
        rb_state10.var_14 = (rb_state10.var_14) + (((((0x52528a1d) - (rb_state10.var_7)) + (rb_state10.var_4)) + (rb_state10.var_11)) + (0x69ad22f4));
        rb_state10.var_15 = (rb_state10.var_15) - (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x329e7edc));
        rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_15) : (0x4b6a8766));
        rb_state10.var_10 = (rb_state10.var_10) - (rb_state10.var_16);
        rb_state10.var_2 = rb_state10.var_10;
    }
    if ((rb_state10.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state10.lock_35));
        rb_state10.var_18 = 0xd8c4077f;
        rb_state10.var_19 = 0x3a4e50c9;
        rb_state10.var_20 = (rb_state10.var_20) + (((rb_state10.var_18) == (0x0)) ? (rb_state10.var_8) : (0xf66eb599));
        rb_state10.var_21 = (rb_state10.var_21) - ((0x8a5f5e8d) + (0x5f6df39e));
        rb_state10.var_22 = (rb_state10.var_22) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_19) : (0xe5406030));
        rb_state10.var_23 = (rb_state10.var_23) + (rb_state10.var_20);
        rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_22) : (0x22c0086d));
        rb_state10.var_25 = (rb_state10.var_25) + (rb_state10.var_23);
        rb_state10.var_26 = (rb_state10.var_26) + (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_24) : (0x232c2ae2));
        rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_25) : (0x250894c8));
        rb_state10.var_28 = (rb_state10.var_28) + (rb_state10.var_26);
        rb_state10.var_29 = (rb_state10.var_29) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_27) : (0xf02f089f));
        rb_state10.var_30 = (rb_state10.var_30) ^ (((0x4308ba18) + (rb_state10.var_28)) - (0x50141bd5));
        rb_state10.var_31 = (rb_state10.var_31) - (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_29) : (0x5c636996));
        rb_state10.var_32 = (rb_state10.var_32) + (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_30) : (0x4f167bd3));
        rb_state10.var_33 = (rb_state10.var_33) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_31) : (0xa072d9ac));
        rb_state10.var_34 = (rb_state10.var_34) ^ (((((0x47f8cdb2) - (rb_state10.var_33)) - (rb_state10.var_32)) ^ (rb_state10.var_16)) ^ (0x3842132d));
        rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_34);
        rb_state10.var_2 = rb_state10.var_17;
        pthread_mutex_unlock(&(rb_state10.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x8f)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        rb_state11.var_2 = rb_state11.var_1;
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0x2f0049d1));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_31));
        rb_state17.var_4 = 0x98441ba1;
        rb_state17.var_5 = (rb_state17.var_5) - (rb_state17.var_2);
        rb_state17.var_6 = (rb_state17.var_6) ^ ((0x839ca624) + (0x1e237d94));
        rb_state17.var_7 = (rb_state17.var_7) + (((rb_state17.var_4) == (0x98441ba1)) ? (rb_state17.var_3) : (0x1546aaae));
        rb_state17.var_8 = (rb_state17.var_8) + (((0x27bd5b3d) ^ (rb_state17.var_4)) + (rb_state17.var_5));
        rb_state17.var_9 = (rb_state17.var_9) + (((rb_state17.var_6) == (0xa1c023b8)) ? (rb_state17.var_7) : (0xfc6c3b43));
        rb_state17.var_10 = (rb_state17.var_10) - (rb_state17.var_8);
        rb_state17.var_11 = (rb_state17.var_11) + (rb_state17.var_9);
        rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_10);
        rb_state17.var_13 = (rb_state17.var_13) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_11) : (0x6500608));
        rb_state17.var_14 = (rb_state17.var_14) ^ (((rb_state17.var_6) == (0xa1c023b8)) ? (rb_state17.var_12) : (0xf14fe716));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0x17b13000));
        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_15);
        rb_state17.var_2 = rb_state17.var_3;
        pthread_mutex_unlock(&(rb_state17.lock_31));
    }
    if ((rb_state17.var_1) == (0x0)) {
        if (!((rb_state17.var_2) == (rb_state17.var_3))) {
            racebench_trigger(17);
        }
    }
    #endif
    th->done = 0;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - ((0x5d44efa2) + (0x9c5e4615));
    if ((rb_state0.var_0) == (0x3526102e)) {
        if ((rb_state0.var_1) != (0x0)) {
            if (!((rb_state0.var_1) == (rb_state0.var_27))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (2281 < rb_input_size ? (uint32_t)rb_input[2281] : 0x59acf4e9);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x8f)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0xc18d5d4f)) {
        pthread_mutex_lock(&(rb_state14.lock_28));
        if ((rb_state14.var_2) != (0x0)) {
            rb_state14.var_29 = !((rb_state14.var_2) == (rb_state14.var_23));
        }
        pthread_mutex_unlock(&(rb_state14.lock_28));
    }
    #endif
    th->next = threads;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_31) == (0xee280358)) {
        rb_state6.var_57 = 0x33da209;
        rb_state6.var_58 = 0xecfbfb20;
        rb_state6.var_59 = (rb_state6.var_59) + (((rb_state6.var_42) == (0x0)) ? (rb_state6.var_41) : (0xaa108e7f));
        rb_state6.var_60 = (rb_state6.var_60) ^ (rb_state6.var_58);
        rb_state6.var_61 = (rb_state6.var_61) - (rb_state6.var_57);
        rb_state6.var_62 = (rb_state6.var_62) ^ (((rb_state6.var_43) == (0x0)) ? (rb_state6.var_59) : (0x48daeba));
        rb_state6.var_63 = (rb_state6.var_63) + (((rb_state6.var_44) == (0x0)) ? (rb_state6.var_60) : (0x43b0ea14));
        rb_state6.var_64 = (rb_state6.var_64) - (((rb_state6.var_61) == (0x0)) ? (rb_state6.var_62) : (0x760db9ca));
        rb_state6.var_65 = (rb_state6.var_65) ^ (((((0x50955be6) - (0x58dafbcc)) - (rb_state6.var_63)) + (rb_state6.var_64)) + (0xeae6ea9c));
        rb_state6.var_41 = (rb_state6.var_41) + (rb_state6.var_65);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_4) == (0x1)) {
        rb_state8.var_20 = 0xbadbdea7;
        rb_state8.var_21 = (rb_state8.var_21) ^ (((0x4e6c6b80) ^ (rb_state8.var_14)) ^ (0x3021462));
        rb_state8.var_22 = (rb_state8.var_22) - (((0x6a02dcad) ^ (rb_state8.var_20)) - (rb_state8.var_16));
        rb_state8.var_23 = (rb_state8.var_23) + (rb_state8.var_17);
        rb_state8.var_24 = (rb_state8.var_24) + (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_21) : (0xc80500c6));
        rb_state8.var_25 = (rb_state8.var_25) - (rb_state8.var_22);
        rb_state8.var_26 = (rb_state8.var_26) + (((((0x5d0221d0) + (0x73547d55)) ^ (rb_state8.var_18)) - (rb_state8.var_23)) - (rb_state8.var_24));
        rb_state8.var_27 = (rb_state8.var_27) + (((((0x150bb8dc) + (rb_state8.var_25)) + (rb_state8.var_26)) ^ (0x34014816)) - (0x92cac881));
        rb_state8.var_19 = (rb_state8.var_19) ^ (rb_state8.var_27);
        rb_state8.var_6 = rb_state8.var_19;
    }
    if ((rb_state8.var_4) == (0x1)) {
        pthread_mutex_lock(&(rb_state8.lock_35));
        rb_state8.var_29 = 0xeff1b318;
        rb_state8.var_30 = 0x4f5341d1;
        rb_state8.var_31 = (rb_state8.var_31) - (((0x3f5661fb) ^ (rb_state8.var_19)) ^ (rb_state8.var_20));
        rb_state8.var_32 = (rb_state8.var_32) - (((rb_state8.var_30) == (0x0)) ? (rb_state8.var_29) : (0x19cd3ed1));
        rb_state8.var_33 = (rb_state8.var_33) ^ (rb_state8.var_31);
        rb_state8.var_34 = (rb_state8.var_34) + (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_33) : (0x4894e703));
        rb_state8.var_28 = (rb_state8.var_28) ^ (rb_state8.var_34);
        rb_state8.var_6 = rb_state8.var_28;
        pthread_mutex_unlock(&(rb_state8.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
    if ((rb_state13.var_11) == (0x26d6be49)) {
        pthread_mutex_lock(&(rb_state13.lock_33));
        if ((rb_state13.var_20) != (0x0)) {
            rb_state13.var_34 = !((rb_state13.var_18) == (rb_state13.var_19));
        }
        pthread_mutex_unlock(&(rb_state13.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_14 = (rb_state14.var_14) + (rb_state14.var_15);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x90ae6bd));
    #endif
    threads = th;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x8da9be25)) {
        pthread_mutex_lock(&(rb_state13.lock_16));
        rb_state13.var_4 = 0x99e532;
        rb_state13.var_5 = 0x6d7b4db7;
        rb_state13.var_6 = (rb_state13.var_6) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_2) : (0x57175cc8));
        rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_4) : (0x5fc33815));
        rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_5);
        rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_6) : (0x274d623));
        rb_state13.var_10 = (rb_state13.var_10) - (rb_state13.var_7);
        rb_state13.var_11 = (rb_state13.var_11) - (((((0x8e1c60e6) ^ (rb_state13.var_9)) + (0x6fe7f574)) ^ (0x88d2caaf)) + (rb_state13.var_8));
        rb_state13.var_12 = (rb_state13.var_12) ^ (rb_state13.var_10);
        rb_state13.var_13 = (rb_state13.var_13) ^ (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_11) : (0xcdd688fb));
        rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_12);
        rb_state13.var_15 = (rb_state13.var_15) - (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_14) : (0x394ac286));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_15);
        rb_state13.var_3 = rb_state13.var_2;
        pthread_mutex_unlock(&(rb_state13.lock_16));
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