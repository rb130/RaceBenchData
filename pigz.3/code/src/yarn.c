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
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_20) == (0x1)) {
        rb_state15.var_31 = 0x804e76a;
        rb_state15.var_32 = (rb_state15.var_32) ^ (((rb_state15.var_31) == (0x804e76a)) ? (rb_state15.var_28) : (0x99782aa));
        rb_state15.var_33 = (rb_state15.var_33) ^ (((((0xa3b2533a) + (rb_state15.var_29)) ^ (rb_state15.var_32)) ^ (0xb7edd459)) ^ (rb_state15.var_27));
        rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_33);
    }
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_1);
    if ((rb_state0.var_19) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_46));
        rb_state0.var_22 = 0xc1da9a03;
        rb_state0.var_23 = 0x7c24b87a;
        rb_state0.var_24 = (rb_state0.var_24) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_20) : (0x11bfb61c));
        rb_state0.var_25 = (rb_state0.var_25) + (rb_state0.var_22);
        rb_state0.var_26 = (rb_state0.var_26) + (((0x1634e642) - (0x60784eb5)) - (rb_state0.var_24));
        rb_state0.var_27 = (rb_state0.var_27) - (((0xe1a08f4e) - (rb_state0.var_25)) + (rb_state0.var_21));
        rb_state0.var_28 = (rb_state0.var_28) + (rb_state0.var_26);
        rb_state0.var_29 = (rb_state0.var_29) - (((rb_state0.var_22) == (0x0)) ? (rb_state0.var_27) : (0xfe1af0f0));
        rb_state0.var_30 = (rb_state0.var_30) ^ (((0xd21a85e5) - (rb_state0.var_28)) - (0x92d810ce));
        rb_state0.var_31 = (rb_state0.var_31) ^ (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_29) : (0xe062b64d));
        rb_state0.var_32 = (rb_state0.var_32) ^ (rb_state0.var_30);
        rb_state0.var_33 = (rb_state0.var_33) ^ (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_31) : (0xa26fd414));
        rb_state0.var_34 = (rb_state0.var_34) + (rb_state0.var_32);
        rb_state0.var_35 = (rb_state0.var_35) - (((rb_state0.var_33) == (0x0)) ? (rb_state0.var_34) : (0xf869005e));
        rb_state0.var_21 = (rb_state0.var_21) + (rb_state0.var_35);
        rb_state0.var_20 = rb_state0.var_21;
        pthread_mutex_unlock(&(rb_state0.lock_46));
    }
    if ((rb_state0.var_19) == (0x0)) {
        if (!((rb_state0.var_20) == (rb_state0.var_21))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_1) : (0xd770dbdc));
    rb_state5.var_1 = (rb_state5.var_1) ^ (2599 < rb_input_size ? (uint32_t)rb_input[2599] : 0xbf0bb5e2);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0xffffff31)) {
        rb_state7.var_4 = 0x59a19616;
        rb_state7.var_5 = (rb_state7.var_5) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0xa3c075f6));
        rb_state7.var_6 = (rb_state7.var_6) ^ (((((0x365b36b0) + (rb_state7.var_4)) ^ (0x7a3fd5fd)) + (rb_state7.var_3)) + (0xf71ed1e));
        rb_state7.var_7 = (rb_state7.var_7) + ((0x2eea3c04) + (rb_state7.var_6));
        rb_state7.var_8 = (rb_state7.var_8) + (rb_state7.var_5);
        rb_state7.var_9 = (rb_state7.var_9) + (rb_state7.var_6);
        rb_state7.var_10 = (rb_state7.var_10) + (((rb_state7.var_7) == (0x9c01fbca)) ? (rb_state7.var_8) : (0xb164799d));
        rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_9) == (0x6d17bfc6)) ? (rb_state7.var_10) : (0x83c3d11f));
        rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x8ef28f7b)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_32))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x4471eb80)) {
        rb_state12.var_15 = 0xab39c37b;
        rb_state12.var_16 = 0x836efd7b;
        rb_state12.var_17 = (rb_state12.var_17) ^ (((rb_state12.var_16) == (0x836efd7b)) ? (rb_state12.var_10) : (0x6ad6fa89));
        rb_state12.var_18 = (rb_state12.var_18) ^ (((rb_state12.var_11) == (0xe5faa794)) ? (rb_state12.var_15) : (0x4a771616));
        rb_state12.var_19 = (rb_state12.var_19) ^ (rb_state12.var_17);
        rb_state12.var_20 = (rb_state12.var_20) - (rb_state12.var_18);
        rb_state12.var_21 = (rb_state12.var_21) - (((((0xf906adbc) - (0xa8e953c2)) - (0xafbf7763)) + (rb_state12.var_20)) - (rb_state12.var_19));
        rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_1) : (0x48ab042c));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ ((0x4fc375e8) - (0x9908b4b7));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    if ((rb_state18.var_11) == (0x3da29f33)) {
        if ((rb_state18.var_15) != (0x0)) {
            if (!((rb_state18.var_15) == (rb_state18.var_34))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    int ret = pthread_mutex_lock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_lock");
    }
}

void release_(lock *bolt, char const *file, long line) {
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0xa6802e73);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x8ef28f7b)) {
        rb_state8.var_20 = 0x313a8232;
        rb_state8.var_21 = 0x40bb9eda;
        rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_20) : (0x42d4794d));
        rb_state8.var_23 = (rb_state8.var_23) ^ (((0xb3ec4235) + (rb_state8.var_15)) ^ (rb_state8.var_17));
        rb_state8.var_24 = (rb_state8.var_24) ^ (rb_state8.var_21);
        rb_state8.var_25 = (rb_state8.var_25) + (((0x83890a56) - (rb_state8.var_18)) - (rb_state8.var_22));
        rb_state8.var_26 = (rb_state8.var_26) - (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_23) : (0xd50177b0));
        rb_state8.var_27 = (rb_state8.var_27) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_24) : (0xc4e913bc));
        rb_state8.var_28 = (rb_state8.var_28) + (((0x7d1a9560) + (0x17f07430)) ^ (rb_state8.var_25));
        rb_state8.var_29 = (rb_state8.var_29) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0x9bc9e51c));
        rb_state8.var_30 = (rb_state8.var_30) - (((0x1886f499) ^ (0xadb443c1)) - (rb_state8.var_28));
        rb_state8.var_31 = (rb_state8.var_31) + (((((0x45dd19df) + (rb_state8.var_30)) ^ (rb_state8.var_29)) ^ (0x32b05c7a)) + (rb_state8.var_21));
        rb_state8.var_19 = (rb_state8.var_19) - (rb_state8.var_31);
        rb_state8.var_3 = rb_state8.var_19;
    }
    if ((rb_state8.var_0) == (0x8ef28f7b)) {
        pthread_mutex_lock(&(rb_state8.lock_37));
        rb_state8.var_33 = 0x1bbbd46d;
        rb_state8.var_34 = (rb_state8.var_34) + (((((0x635c5357) ^ (rb_state8.var_23)) + (rb_state8.var_22)) + (rb_state8.var_24)) ^ (rb_state8.var_25));
        rb_state8.var_35 = (rb_state8.var_35) ^ (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_33) : (0xc4062bf6));
        rb_state8.var_36 = (rb_state8.var_36) - (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0x2da5bd64));
        rb_state8.var_32 = (rb_state8.var_32) + (rb_state8.var_36);
        rb_state8.var_3 = rb_state8.var_32;
        pthread_mutex_unlock(&(rb_state8.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (0x80aebd12);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_1) : (0xff207453));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0x193ff93b) ^ (rb_state15.var_0));
    #endif
    int ret = pthread_mutex_unlock(&(bolt->mutex));
    if (ret) {
        fail(ret, file, line, "mutex_unlock");
    }
}

void twist_(lock *bolt, enum twist_op op, long val, char const *file, long line) {
    if (op == TO) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + ((0x41f79b) + (0x383c8f4f));
        #endif
        bolt->value = val;
    } else if (op == BY) {
        bolt->value += val;
    }
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0x40de80ec));
    #endif
    int ret = pthread_cond_broadcast(&(bolt->cond));
    if (ret) {
        fail(ret, file, line, "cond_broadcast");
    }
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - ((0x2410a6ad) + (0x427ca931));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0xa478c0b4);
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
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_6 = (rb_state15.var_6) ^ (0xf6aaa416);
            if ((rb_state15.var_20) == (0x1)) {
                pthread_mutex_lock(&(rb_state15.lock_34));
                rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_7) == (0xb267c6b0)) ? (rb_state15.var_21) : (0x93a72937));
                rb_state15.var_24 = (rb_state15.var_24) ^ (rb_state15.var_23);
                rb_state15.var_25 = (rb_state15.var_25) - (0xed258248);
                rb_state15.var_26 = (rb_state15.var_26) ^ (((rb_state15.var_22) == (0x0)) ? (rb_state15.var_23) : (0x60b64731));
                rb_state15.var_27 = (rb_state15.var_27) ^ (rb_state15.var_24);
                rb_state15.var_28 = (rb_state15.var_28) ^ (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_25) : (0x49069bfe));
                rb_state15.var_29 = (rb_state15.var_29) - (((((0xff0c1806) + (rb_state15.var_26)) - (rb_state15.var_25)) ^ (rb_state15.var_26)) - (rb_state15.var_27));
                rb_state15.var_30 = (rb_state15.var_30) + (((rb_state15.var_28) == (0x12da7db8)) ? (rb_state15.var_29) : (0x363f5849));
                rb_state15.var_22 = (rb_state15.var_22) - (rb_state15.var_30);
                rb_state15.var_21 = rb_state15.var_22;
                pthread_mutex_unlock(&(rb_state15.lock_34));
            }
            if ((rb_state15.var_20) == (0x1)) {
                if (!((rb_state15.var_21) == (rb_state15.var_22))) {
                    racebench_trigger(15);
                }
            }
            #endif
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) + (0x6a3a04a4);
        #endif
        break;
    case NOT_TO_BE:
        until(bolt->value != val) {
            int ret = pthread_cond_wait(&(bolt->cond), &(bolt->mutex));
            if (ret) {
                fail(ret, file, line, "cond_wait");
            }
        }
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state14.lock_33));
            rb_state14.var_5 = 0xacac9f10;
            rb_state14.var_6 = 0x7b98442a;
            rb_state14.var_7 = (rb_state14.var_7) ^ ((0x1afd4f04) ^ (rb_state14.var_4));
            rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_5) == (0xacac9f10)) ? (rb_state14.var_5) : (0xf98f88fb));
            rb_state14.var_9 = (rb_state14.var_9) - (((((0x62ff175f) - (rb_state14.var_6)) ^ (rb_state14.var_3)) - (rb_state14.var_6)) - (0xfdae3806));
            rb_state14.var_10 = (rb_state14.var_10) + (rb_state14.var_7);
            rb_state14.var_11 = (rb_state14.var_11) - (((rb_state14.var_7) == (0x1afd4f04)) ? (rb_state14.var_8) : (0xe32c1951));
            rb_state14.var_12 = (rb_state14.var_12) - (rb_state14.var_9);
            rb_state14.var_13 = (rb_state14.var_13) ^ (((0xaa4675ad) + (rb_state14.var_8)) ^ (rb_state14.var_10));
            rb_state14.var_14 = (rb_state14.var_14) - (((rb_state14.var_9) == (0x91dfa8fb)) ? (rb_state14.var_11) : (0xb0dc3e6d));
            rb_state14.var_15 = (rb_state14.var_15) ^ (((0xb8211e52) ^ (rb_state14.var_10)) ^ (rb_state14.var_12));
            rb_state14.var_16 = (rb_state14.var_16) - (((((0x2ed74f7) ^ (rb_state14.var_11)) ^ (0x9f0abc24)) + (rb_state14.var_13)) - (rb_state14.var_14));
            rb_state14.var_17 = (rb_state14.var_17) + (((0xde7c2290) - (rb_state14.var_12)) - (rb_state14.var_15));
            rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_13) == (0x4c0e5bb9)) ? (rb_state14.var_16) : (0xb0f095d5));
            rb_state14.var_19 = (rb_state14.var_19) + (((0x3117f8d0) ^ (0xdf5e76f3)) - (rb_state14.var_17));
            rb_state14.var_20 = (rb_state14.var_20) ^ (((((0xb8eef1c2) ^ (rb_state14.var_19)) + (rb_state14.var_14)) - (0xd268e462)) ^ (rb_state14.var_18));
            rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_20);
            rb_state14.var_3 = rb_state14.var_4;
            pthread_mutex_unlock(&(rb_state14.lock_33));
        }
        if ((rb_state14.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state14.lock_33));
            if (!((rb_state14.var_3) == (rb_state14.var_4))) {
                racebench_trigger(14);
            }
            pthread_mutex_unlock(&(rb_state14.lock_33));
        }
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
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (0x98853e1f);
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (808 < rb_input_size ? (uint32_t)rb_input[808] : 0xc4cd916b);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_9 = (rb_state2.var_9) + (rb_state2.var_10);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x96314dd2));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_2);
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0xfc0f80da)) ? (rb_state6.var_0) : (0x2cb5a930));
    #endif
    pthread_t me = pthread_self();
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_11 = (rb_state2.var_11) ^ (0xf7103884);
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

    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - ((0xc99828f3) + (0xc50b4d55));
    #endif
    match->done = 1;
    if (threads != match) {
        *prior = match->next;
        match->next = threads;
        threads = match;
    }

    twist_(&(threads_lock), BY, +1, capsule->file, capsule->line);

    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_2) == (0x66)) ? (rb_state19.var_1) : (0x16112390));
    #endif
    my_free(capsule);
}

local void *ignition(void *arg) {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + ((0xf5e247bb) + (0xd81fcc1a));
    rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x4cdc990a));
    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_0) == (0xce0213d5)) ? (rb_state1.var_2) : (0x96df7983));
    if ((rb_state1.var_5) == (0x1)) {
        rb_state1.var_26 = 0x5f60a90e;
        rb_state1.var_27 = 0x6343dcdd;
        rb_state1.var_28 = (rb_state1.var_28) - (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_26) : (0x7b5ab0b7));
        rb_state1.var_29 = (rb_state1.var_29) - (((((0x2dfdd302) - (0xd1c75009)) + (rb_state1.var_18)) ^ (rb_state1.var_27)) - (0x9cfbb585));
        rb_state1.var_30 = (rb_state1.var_30) + (((rb_state1.var_20) == (0x0)) ? (rb_state1.var_28) : (0xac11b306));
        rb_state1.var_31 = (rb_state1.var_31) - (((rb_state1.var_29) == (0x0)) ? (rb_state1.var_30) : (0x9827d18a));
        rb_state1.var_7 = (rb_state1.var_7) ^ (rb_state1.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x8d90064d) - (0xc10863e1));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x3e3cae47) + (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (2977 < rb_input_size ? (uint32_t)rb_input[2977] : 0x8d4f680b);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (3155 < rb_input_size ? (uint32_t)rb_input[3155] : 0x33650550);
    if ((rb_state10.var_1) == (0x269addf8)) {
        if ((rb_state10.var_3) != (0x0)) {
            if (!((rb_state10.var_3) == (rb_state10.var_30))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x7f9b63b3) ^ (rb_state12.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x0)) {
        rb_state13.var_18 = 0x2d3189ec;
        rb_state13.var_19 = (rb_state13.var_19) + (rb_state13.var_9);
        rb_state13.var_20 = (rb_state13.var_20) ^ (((((0xfee3e7fa) ^ (0xa3948214)) - (rb_state13.var_10)) - (rb_state13.var_18)) - (0x61f1b892));
        rb_state13.var_21 = (rb_state13.var_21) ^ (rb_state13.var_19);
        rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_20);
        rb_state13.var_23 = (rb_state13.var_23) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_22) : (0x896284dd));
        rb_state13.var_17 = (rb_state13.var_17) ^ (rb_state13.var_23);
        rb_state13.var_3 = rb_state13.var_17;
    }
    if ((rb_state13.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_37));
        rb_state13.var_25 = 0x416bf357;
        rb_state13.var_26 = 0x2938a1fb;
        rb_state13.var_27 = (rb_state13.var_27) - (rb_state13.var_12);
        rb_state13.var_28 = (rb_state13.var_28) + (rb_state13.var_26);
        rb_state13.var_29 = (rb_state13.var_29) - (rb_state13.var_25);
        rb_state13.var_30 = (rb_state13.var_30) ^ (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_11) : (0x1cab525));
        rb_state13.var_31 = (rb_state13.var_31) + (((0xd2cdf303) - (rb_state13.var_14)) ^ (rb_state13.var_27));
        rb_state13.var_32 = (rb_state13.var_32) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_29) : (0x6aef2f92));
        rb_state13.var_33 = (rb_state13.var_33) + (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0x293858fd));
        rb_state13.var_34 = (rb_state13.var_34) + (((0x1a7afa41) - (rb_state13.var_32)) - (rb_state13.var_15));
        rb_state13.var_35 = (rb_state13.var_35) ^ (((0xc276bf63) - (0x9fc7ac3d)) - (rb_state13.var_33));
        rb_state13.var_36 = (rb_state13.var_36) + (((rb_state13.var_34) == (0x0)) ? (rb_state13.var_35) : (0x1e4d0901));
        rb_state13.var_24 = (rb_state13.var_24) + (rb_state13.var_36);
        rb_state13.var_3 = rb_state13.var_24;
        pthread_mutex_unlock(&(rb_state13.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_33));
        if ((rb_state17.var_3) != (0x0)) {
            if (!((rb_state17.var_3) == (rb_state17.var_26))) {
                racebench_trigger(17);
            }
        }
        pthread_mutex_unlock(&(rb_state17.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - ((0xb65530af) ^ (0x9e10b10d));
    #endif
    struct capsule *capsule = arg;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0x26eeda25)) {
        pthread_mutex_lock(&(rb_state0.lock_18));
        rb_state0.var_5 = 0x6f0fdd0d;
        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_5);
        rb_state0.var_7 = (rb_state0.var_7) - (rb_state0.var_6);
        rb_state0.var_8 = (rb_state0.var_8) ^ (2080 < rb_input_size ? (uint32_t)rb_input[2080] : 0x2b58ff6);
        rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_3) : (0x9e26ec4d));
        rb_state0.var_10 = (rb_state0.var_10) ^ (((((0x382f1aa8) - (rb_state0.var_8)) + (rb_state0.var_5)) + (rb_state0.var_4)) ^ (0xbae8349f));
        rb_state0.var_11 = (rb_state0.var_11) - (rb_state0.var_6);
        rb_state0.var_12 = (rb_state0.var_12) + (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_8) : (0xe79c4ac1));
        rb_state0.var_13 = (rb_state0.var_13) + (((0x48a9cb9c) - (rb_state0.var_9)) - (rb_state0.var_9));
        rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_10);
        rb_state0.var_15 = (rb_state0.var_15) ^ (((((0xbae93e62) ^ (rb_state0.var_11)) - (rb_state0.var_11)) ^ (rb_state0.var_12)) + (rb_state0.var_10));
        rb_state0.var_16 = (rb_state0.var_16) ^ (((rb_state0.var_13) == (0xa6ef4ff0)) ? (rb_state0.var_14) : (0xd8978ca6));
        rb_state0.var_17 = (rb_state0.var_17) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x1e34133d));
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_17);
        rb_state0.var_4 = rb_state0.var_3;
        pthread_mutex_unlock(&(rb_state0.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (0x7e44d2de);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (2768 < rb_input_size ? (uint32_t)rb_input[2768] : 0xf00402c0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (0xe6cf4ea8);
    if ((rb_state6.var_1) == (0x0)) {
        rb_state6.var_2 = rb_state6.var_0;
    }
    if ((rb_state6.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_9));
        if (!((rb_state6.var_0) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_1) : (0x1a282970));
    if ((rb_state9.var_1) == (0x720c973b)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        if ((rb_state9.var_2) != (0x0)) {
            if (!((rb_state9.var_2) == (rb_state9.var_23))) {
                racebench_trigger(9);
            }
        }
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (0x40b4a5d2);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + ((0x5aef5567) - (rb_state12.var_1));
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (2913 < rb_input_size ? (uint32_t)rb_input[2913] : 0x76c4b51e);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0x451412dc));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x66)) {
        rb_state19.var_4 = 0x9586b90e;
        rb_state19.var_5 = 0x124446ee;
        rb_state19.var_6 = (rb_state19.var_6) ^ (((((0x7eb71aad) + (rb_state19.var_4)) ^ (0xac0a22d9)) - (rb_state19.var_5)) + (rb_state19.var_3));
        rb_state19.var_7 = (rb_state19.var_7) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_4) : (0xa0ec4068));
        rb_state19.var_8 = (rb_state19.var_8) + (((0xd79e3f3d) - (rb_state19.var_6)) - (rb_state19.var_6));
        rb_state19.var_9 = (rb_state19.var_9) - (rb_state19.var_7);
        rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_9) : (0xe73d6c25));
        rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_10);
    }
    #endif
    pthread_cleanup_push(reenter, arg);

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x25092501)) {
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_3);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xc1b3eb31));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (2619 < rb_input_size ? (uint32_t)rb_input[2619] : 0x956ede66);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (913 < rb_input_size ? (uint32_t)rb_input[913] : 0xaf37766e);
    #endif
    capsule->probe(capsule->payload);

    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (1541 < rb_input_size ? (uint32_t)rb_input[1541] : 0xc3eb4fb);
    #endif
    pthread_cleanup_pop(1);

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    #endif
    return NULL;
}

thread *launch_(void (*probe)(void *), void *payload, char const *file, long line) {

    #ifdef RACEBENCH_BUG_15
    rb_state15.var_17 = (rb_state15.var_17) - ((0x239449c2) + (rb_state15.var_18));
    rb_state15.var_5 = (rb_state15.var_5) + (3205 < rb_input_size ? (uint32_t)rb_input[3205] : 0xe32274b4);
    #endif
    struct capsule *capsule = my_malloc(sizeof(struct capsule), file, line);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x77477e42)) ? (rb_state5.var_1) : (0x296da9c2));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0xbd4443c6)) {
        rb_state16.var_2 = rb_state16.var_0;
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_1) : (0xc795ddee));
    #endif
    capsule->probe = probe;
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0xbd4443c6)) {
        rb_state16.var_12 = !((rb_state16.var_0) == (rb_state16.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ ((0x3be68cee) - (0x16957954));
    rb_state17.var_2 = (rb_state17.var_2) - (1894 < rb_input_size ? (uint32_t)rb_input[1894] : 0x4bf136c0);
    #endif
    capsule->payload = payload;
    capsule->file = file;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (0x803dad7c);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_10) == (0xd3d50a35)) ? (rb_state16.var_9) : (0xe53cb230));
    #endif
    capsule->line = line;

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_1) : (0x35df9132));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_12);
    #endif
    possess_(&(threads_lock), file, line);

    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (0xbb64f2db);
    #endif
    thread *th = my_malloc(sizeof(struct thread_s), file, line);
    pthread_attr_t attr;
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_6 = (rb_state16.var_6) ^ (rb_state16.var_2);
    #endif
    int ret = pthread_attr_init(&attr);
    if (ret) {
        fail(ret, file, line, "attr_init");
    }
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (ret) {
        fail(ret, file, line, "attr_setdetachstate");
    }
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0xe27e919a) - (0x6b371358));
    #endif
    ret = pthread_create(&(th->id), &attr, ignition, capsule);
    if (ret) {
        fail(ret, file, line, "create");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_2) : (0x26eeda25));
    if ((rb_state0.var_1) == (0x26eeda25)) {
        rb_state0.var_19 = !((rb_state0.var_4) == (rb_state0.var_3));
    }
    rb_state0.var_12 = (rb_state0.var_12) + (0xe1372df9);
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_5) == (0x1)) {
        pthread_mutex_lock(&(rb_state1.lock_32));
        rb_state1.var_9 = 0x7d5f83a5;
        rb_state1.var_10 = (rb_state1.var_10) - (3030 < rb_input_size ? (uint32_t)rb_input[3030] : 0x49b3e3f);
        rb_state1.var_11 = (rb_state1.var_11) + (((0xa441a26) + (rb_state1.var_8)) ^ (rb_state1.var_9));
        rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_7) : (0x150c4a9f));
        rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_9) : (0x7d99f070));
        rb_state1.var_14 = (rb_state1.var_14) ^ (rb_state1.var_10);
        rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_11) : (0x3e47d4f7));
        rb_state1.var_16 = (rb_state1.var_16) + (((0x29e0cd82) - (rb_state1.var_12)) - (0x3b304df7));
        rb_state1.var_17 = (rb_state1.var_17) + (((0xa3f9bfb4) + (rb_state1.var_13)) - (rb_state1.var_13));
        rb_state1.var_18 = (rb_state1.var_18) + (((0x35c81eea) ^ (rb_state1.var_14)) - (rb_state1.var_14));
        rb_state1.var_19 = (rb_state1.var_19) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_16) : (0xf46a86df));
        rb_state1.var_20 = (rb_state1.var_20) + (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0xce10320b));
        rb_state1.var_21 = (rb_state1.var_21) - (((0x49454d4e) ^ (rb_state1.var_19)) - (0x308a5721));
        rb_state1.var_22 = (rb_state1.var_22) ^ (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_20) : (0xd982858d));
        rb_state1.var_23 = (rb_state1.var_23) + (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_21) : (0xdd58dcfa));
        rb_state1.var_24 = (rb_state1.var_24) - (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_22) : (0x2522389f));
        rb_state1.var_25 = (rb_state1.var_25) + (((((0xc2cb5af3) + (0x64da17ed)) ^ (rb_state1.var_23)) + (0x9f4424be)) ^ (rb_state1.var_24));
        rb_state1.var_8 = (rb_state1.var_8) ^ (rb_state1.var_25);
        rb_state1.var_7 = rb_state1.var_8;
        pthread_mutex_unlock(&(rb_state1.lock_32));
    }
    if ((rb_state1.var_5) == (0x1)) {
        if (!((rb_state1.var_7) == (rb_state1.var_8))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0x8663a905) - (0xdebbcb79));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_9));
        rb_state6.var_3 = 0xcc3e4375;
        rb_state6.var_4 = (rb_state6.var_4) ^ ((0x3fd23e92) - (0x9d5d7a06));
        rb_state6.var_5 = (rb_state6.var_5) + (((0xd4884f1) - (rb_state6.var_2)) ^ (0x5c13b675));
        rb_state6.var_6 = (rb_state6.var_6) - (rb_state6.var_3);
        rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_5) : (0x65c596d1));
        rb_state6.var_8 = (rb_state6.var_8) + (((((0xddba970f) - (rb_state6.var_3)) - (rb_state6.var_7)) + (rb_state6.var_4)) + (rb_state6.var_6));
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_8);
        pthread_mutex_unlock(&(rb_state6.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (125 < rb_input_size ? (uint32_t)rb_input[125] : 0x5c48fde0);
    rb_state7.var_1 = (rb_state7.var_1) - (0x67e2b9a4);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + ((0x5a5bd494) - (0xf7c5a6dc));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_1) == (0x720c973b)) {
        rb_state9.var_7 = 0xbd6deeb7;
        rb_state9.var_8 = 0x423a75d2;
        rb_state9.var_9 = (rb_state9.var_9) + (rb_state9.var_7);
        rb_state9.var_10 = (rb_state9.var_10) - (rb_state9.var_7);
        rb_state9.var_11 = (rb_state9.var_11) ^ (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_6) : (0xc864df24));
        rb_state9.var_12 = (rb_state9.var_12) ^ (((0x26022b2d) ^ (rb_state9.var_9)) - (rb_state9.var_8));
        rb_state9.var_13 = (rb_state9.var_13) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_9) : (0x33a14e63));
        rb_state9.var_14 = (rb_state9.var_14) ^ (((0x477d2d78) + (rb_state9.var_11)) + (rb_state9.var_10));
        rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_11);
        rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_13) : (0xceb87f39));
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_14) : (0xd62ed058));
        rb_state9.var_18 = (rb_state9.var_18) + (((0x7c367579) - (rb_state9.var_13)) ^ (rb_state9.var_15));
        rb_state9.var_19 = (rb_state9.var_19) + (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_17) : (0xc1dd4e1d));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_18) : (0x84e1760c));
        rb_state9.var_21 = (rb_state9.var_21) + (rb_state9.var_19);
        rb_state9.var_22 = (rb_state9.var_22) + (((((0x91f10204) - (0x5695aec6)) - (0xeae9c2a5)) ^ (rb_state9.var_21)) ^ (rb_state9.var_20));
        rb_state9.var_6 = (rb_state9.var_6) - (rb_state9.var_22);
        rb_state9.var_2 = rb_state9.var_6;
    }
    if ((rb_state9.var_1) == (0x720c973b)) {
        pthread_mutex_lock(&(rb_state9.lock_30));
        rb_state9.var_24 = (rb_state9.var_24) - (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_18) : (0x3869db0a));
        rb_state9.var_25 = (rb_state9.var_25) ^ (((0xe8a48bc3) + (rb_state9.var_20)) - (rb_state9.var_15));
        rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_17) : (0x6d6058a0));
        rb_state9.var_27 = (rb_state9.var_27) - (((((0x17f8e79a) - (rb_state9.var_16)) - (rb_state9.var_24)) ^ (rb_state9.var_22)) ^ (0x482e6009));
        rb_state9.var_28 = (rb_state9.var_28) + (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_26) : (0xee3855a8));
        rb_state9.var_29 = (rb_state9.var_29) ^ (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0x605e6ea4));
        rb_state9.var_23 = (rb_state9.var_23) + (rb_state9.var_29);
        rb_state9.var_2 = rb_state9.var_23;
        pthread_mutex_unlock(&(rb_state9.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0xe339339b) - (0xcaa3c115));
    #endif
    ret = pthread_attr_destroy(&attr);
    if (ret) {
        fail(ret, file, line, "attr_destroy");
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_13 = (rb_state0.var_13) + ((0xa6ef4ff0) ^ (rb_state0.var_14));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x990502ff) + (rb_state1.var_2));
    rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (3461 < rb_input_size ? (uint32_t)rb_input[3461] : 0x8ee116e0);
    rb_state3.var_1 = (rb_state3.var_1) - (0x677585a8);
    rb_state3.var_0 = (rb_state3.var_0) + ((0x1bf1a80a) ^ (0xac74ea6b));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (236 < rb_input_size ? (uint32_t)rb_input[236] : 0x2078388d);
    rb_state7.var_1 = (rb_state7.var_1) ^ (((rb_state7.var_0) == (0x3e3caf03)) ? (rb_state7.var_2) : (0x7ef68278));
    if ((rb_state7.var_1) == (0xffffff31)) {
        rb_state7.var_3 = rb_state7.var_2;
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0xf8bff17c);
    rb_state12.var_2 = (rb_state12.var_2) ^ ((0x4471eb80) + (rb_state12.var_2));
    #endif
    th->done = 0;
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0xffffff31)) {
        if (!((rb_state7.var_2) == (rb_state7.var_3))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (3415 < rb_input_size ? (uint32_t)rb_input[3415] : 0x185efd1b);
    if ((rb_state10.var_1) == (0x269addf8)) {
        rb_state10.var_15 = 0x9a0e628e;
        rb_state10.var_16 = 0xd4a42f0;
        rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_15) : (0x57a67df0));
        rb_state10.var_18 = (rb_state10.var_18) - ((0x171de6cc) + (0xf01e8ba7));
        rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_8);
        rb_state10.var_20 = (rb_state10.var_20) - (((0xb1c69cc5) ^ (0x3ccd96bb)) - (rb_state10.var_17));
        rb_state10.var_21 = (rb_state10.var_21) + (((0x93be5c14) + (rb_state10.var_9)) - (rb_state10.var_18));
        rb_state10.var_22 = (rb_state10.var_22) + (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_20) : (0x98a5debd));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((0x3f03919d) - (rb_state10.var_10)) ^ (rb_state10.var_21));
        rb_state10.var_24 = (rb_state10.var_24) + (rb_state10.var_22);
        rb_state10.var_25 = (rb_state10.var_25) - (((0xc3d17805) ^ (rb_state10.var_11)) - (rb_state10.var_23));
        rb_state10.var_26 = (rb_state10.var_26) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_24) : (0xd3cfb3d9));
        rb_state10.var_27 = (rb_state10.var_27) - (rb_state10.var_25);
        rb_state10.var_28 = (rb_state10.var_28) + (((0xbcdca5d6) + (rb_state10.var_26)) - (rb_state10.var_13));
        rb_state10.var_29 = (rb_state10.var_29) - (((((0x6c0f2e69) - (rb_state10.var_27)) ^ (rb_state10.var_28)) - (rb_state10.var_14)) - (rb_state10.var_15));
        rb_state10.var_14 = (rb_state10.var_14) ^ (rb_state10.var_29);
        rb_state10.var_3 = rb_state10.var_14;
    }
    if ((rb_state10.var_1) == (0x269addf8)) {
        pthread_mutex_lock(&(rb_state10.lock_43));
        rb_state10.var_31 = 0xf76d7fdd;
        rb_state10.var_32 = (rb_state10.var_32) + ((0x1a1ee865) + (rb_state10.var_18));
        rb_state10.var_33 = (rb_state10.var_33) ^ (((0x16427493) ^ (rb_state10.var_17)) ^ (rb_state10.var_19));
        rb_state10.var_34 = (rb_state10.var_34) + (((((0x107959c0) ^ (rb_state10.var_16)) + (rb_state10.var_31)) ^ (rb_state10.var_20)) ^ (0xf65ef53a));
        rb_state10.var_35 = (rb_state10.var_35) + (((0xa0ee27c7) - (rb_state10.var_32)) ^ (rb_state10.var_21));
        rb_state10.var_36 = (rb_state10.var_36) - (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_33) : (0xaeafda3b));
        rb_state10.var_37 = (rb_state10.var_37) + (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_34) : (0xa0707500));
        rb_state10.var_38 = (rb_state10.var_38) - (((rb_state10.var_35) == (0x0)) ? (rb_state10.var_36) : (0xede5b6ca));
        rb_state10.var_39 = (rb_state10.var_39) + (rb_state10.var_37);
        rb_state10.var_40 = (rb_state10.var_40) + (((0x5e6cb18a) + (rb_state10.var_24)) ^ (rb_state10.var_38));
        rb_state10.var_41 = (rb_state10.var_41) ^ (rb_state10.var_39);
        rb_state10.var_42 = (rb_state10.var_42) - (((((0xac00665) + (rb_state10.var_25)) - (rb_state10.var_40)) + (rb_state10.var_26)) + (rb_state10.var_41));
        rb_state10.var_30 = (rb_state10.var_30) + (rb_state10.var_42);
        rb_state10.var_3 = rb_state10.var_30;
        pthread_mutex_unlock(&(rb_state10.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_2) == (0x4471eb80)) {
        pthread_mutex_lock(&(rb_state12.lock_22));
        rb_state12.var_5 = 0xabc6f6bc;
        rb_state12.var_6 = 0xb206af59;
        rb_state12.var_7 = (rb_state12.var_7) - (0x9aba0843);
        rb_state12.var_8 = (rb_state12.var_8) + (((((0x77cae071) + (rb_state12.var_3)) - (rb_state12.var_4)) ^ (rb_state12.var_6)) - (rb_state12.var_5));
        rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_6) == (0xb206af59)) ? (rb_state12.var_5) : (0x2157c272));
        rb_state12.var_10 = (rb_state12.var_10) ^ (rb_state12.var_7);
        rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_7) == (0x6545f7bd)) ? (rb_state12.var_8) : (0x8a8bfd6b));
        rb_state12.var_12 = (rb_state12.var_12) + (((((0x48ad79c6) ^ (rb_state12.var_8)) ^ (rb_state12.var_10)) ^ (rb_state12.var_9)) + (rb_state12.var_9));
        rb_state12.var_13 = (rb_state12.var_13) + (rb_state12.var_11);
        rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_12) == (0x47f21767)) ? (rb_state12.var_13) : (0xf3762423));
        rb_state12.var_4 = (rb_state12.var_4) ^ (rb_state12.var_14);
        rb_state12.var_3 = rb_state12.var_4;
        pthread_mutex_unlock(&(rb_state12.lock_22));
    }
    if ((rb_state12.var_2) == (0x4471eb80)) {
        if (!((rb_state12.var_3) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x0)) {
        if ((rb_state13.var_3) != (0x0)) {
            if (!((rb_state13.var_3) == (rb_state13.var_24))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_3) == (0xead78dda)) {
        pthread_mutex_lock(&(rb_state16.lock_43));
        if ((rb_state16.var_13) != (0x0)) {
            rb_state16.var_44 = !((rb_state16.var_13) == (rb_state16.var_33));
        }
        pthread_mutex_unlock(&(rb_state16.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (1451 < rb_input_size ? (uint32_t)rb_input[1451] : 0xd4e478b7);
    #endif
    th->next = threads;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_19) == (0x0)) {
        rb_state0.var_36 = 0x84ccc677;
        rb_state0.var_37 = 0xc385b54c;
        rb_state0.var_38 = 0xb315798;
        rb_state0.var_39 = (rb_state0.var_39) - (((((0x6596243a) + (rb_state0.var_38)) + (rb_state0.var_25)) + (rb_state0.var_26)) - (rb_state0.var_36));
        rb_state0.var_40 = (rb_state0.var_40) ^ (rb_state0.var_37);
        rb_state0.var_41 = (rb_state0.var_41) ^ (rb_state0.var_39);
        rb_state0.var_42 = (rb_state0.var_42) ^ (rb_state0.var_40);
        rb_state0.var_43 = (rb_state0.var_43) ^ (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_41) : (0x2109bfd0));
        rb_state0.var_44 = (rb_state0.var_44) - (rb_state0.var_42);
        rb_state0.var_45 = (rb_state0.var_45) ^ (((((0x50f6e73) - (rb_state0.var_44)) ^ (0x8be5bcbf)) ^ (0x9c59193f)) ^ (rb_state0.var_43));
        rb_state0.var_20 = (rb_state0.var_20) - (rb_state0.var_45);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x25092501)) {
        rb_state1.var_3 = rb_state1.var_1;
    }
    if ((rb_state1.var_0) == (0x25092501)) {
        rb_state1.var_5 = !((rb_state1.var_1) == (rb_state1.var_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0xc7a75ad6)) {
        rb_state3.var_2 = rb_state3.var_0;
    }
    if ((rb_state3.var_1) == (0xc7a75ad6)) {
        if (!((rb_state3.var_0) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ ((0x794d70a5) + (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_26 = (rb_state16.var_26) ^ (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_27) : (0x4f587095));
    #endif
    threads = th;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_12) == (0x78c904bb)) {
        rb_state2.var_29 = 0x46b23014;
        rb_state2.var_30 = (rb_state2.var_30) ^ ((0xcec0686b) - (rb_state2.var_25));
        rb_state2.var_31 = (rb_state2.var_31) + (0x27ee4200);
        rb_state2.var_32 = (rb_state2.var_32) + (1282 < rb_input_size ? (uint32_t)rb_input[1282] : 0xe0853d7e);
        rb_state2.var_33 = (rb_state2.var_33) ^ (((0x14a06085) - (rb_state2.var_26)) ^ (rb_state2.var_29));
        rb_state2.var_34 = (rb_state2.var_34) + (rb_state2.var_24);
        rb_state2.var_35 = (rb_state2.var_35) ^ (rb_state2.var_23);
        rb_state2.var_36 = (rb_state2.var_36) ^ (((((0x32389205) ^ (rb_state2.var_31)) + (rb_state2.var_27)) ^ (rb_state2.var_28)) - (rb_state2.var_30));
        rb_state2.var_37 = (rb_state2.var_37) - (((((0xde644862) - (0x618f5e86)) + (rb_state2.var_32)) ^ (rb_state2.var_33)) - (rb_state2.var_29));
        rb_state2.var_38 = (rb_state2.var_38) + (((0x3feac55b) + (rb_state2.var_34)) + (0x5b653ba6));
        rb_state2.var_39 = (rb_state2.var_39) ^ (((((0x97baaf59) - (rb_state2.var_36)) - (rb_state2.var_35)) ^ (rb_state2.var_30)) - (0x250bcfa));
        rb_state2.var_40 = (rb_state2.var_40) - (((((0x436f3279) - (rb_state2.var_38)) + (0x6e829721)) - (rb_state2.var_37)) + (0xaaab850e));
        rb_state2.var_41 = (rb_state2.var_41) + (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_39) : (0xe99225cd));
        rb_state2.var_42 = (rb_state2.var_42) + (((0x8285f772) + (rb_state2.var_32)) ^ (rb_state2.var_40));
        rb_state2.var_43 = (rb_state2.var_43) + (rb_state2.var_41);
        rb_state2.var_44 = (rb_state2.var_44) ^ (((rb_state2.var_33) == (0x0)) ? (rb_state2.var_42) : (0xc8a2de12));
        rb_state2.var_45 = (rb_state2.var_45) - (((rb_state2.var_34) == (0x0)) ? (rb_state2.var_43) : (0xb4146382));
        rb_state2.var_46 = (rb_state2.var_46) ^ (((0x82fde942) - (rb_state2.var_44)) + (0x965abaae));
        rb_state2.var_47 = (rb_state2.var_47) ^ (((0xed66d3f9) ^ (0x5e6cec70)) ^ (rb_state2.var_45));
        rb_state2.var_48 = (rb_state2.var_48) ^ (((rb_state2.var_35) == (0x0)) ? (rb_state2.var_46) : (0x52a795a));
        rb_state2.var_49 = (rb_state2.var_49) - (((((0x892daf12) - (rb_state2.var_48)) ^ (0x6193fbdf)) ^ (0x93a6957)) - (rb_state2.var_47));
        rb_state2.var_28 = (rb_state2.var_28) + (rb_state2.var_49);
        rb_state2.var_16 = rb_state2.var_28;
    }
    if ((rb_state2.var_12) == (0x78c904bb)) {
        pthread_mutex_lock(&(rb_state2.lock_54));
        rb_state2.var_51 = 0xf494a9d;
        rb_state2.var_52 = (rb_state2.var_52) - (((rb_state2.var_51) == (0x0)) ? (rb_state2.var_36) : (0x173a78ff));
        rb_state2.var_53 = (rb_state2.var_53) - (((rb_state2.var_37) == (0x0)) ? (rb_state2.var_52) : (0x22b068c7));
        rb_state2.var_50 = (rb_state2.var_50) - (rb_state2.var_53);
        rb_state2.var_16 = rb_state2.var_50;
        pthread_mutex_unlock(&(rb_state2.lock_54));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_1);
    rb_state4.var_2 = (rb_state4.var_2) - (3548 < rb_input_size ? (uint32_t)rb_input[3548] : 0xbf8fb7fe);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + ((0xf33c6cfc) + (0x9528b626));
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x32e560cc)) {
        rb_state11.var_18 = 0x66b3440e;
        rb_state11.var_19 = (rb_state11.var_19) ^ (rb_state11.var_14);
        rb_state11.var_20 = (rb_state11.var_20) + (0x8b3d56fc);
        rb_state11.var_21 = (rb_state11.var_21) - (((0x976f6ceb) - (rb_state11.var_15)) - (rb_state11.var_18));
        rb_state11.var_22 = (rb_state11.var_22) + (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_13) : (0xa7112c30));
        rb_state11.var_23 = (rb_state11.var_23) ^ (((((0x8aa9f415) ^ (rb_state11.var_17)) - (rb_state11.var_20)) - (rb_state11.var_19)) - (rb_state11.var_18));
        rb_state11.var_24 = (rb_state11.var_24) ^ (rb_state11.var_21);
        rb_state11.var_25 = (rb_state11.var_25) + (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_22) : (0xd04740b0));
        rb_state11.var_26 = (rb_state11.var_26) + (((0x7958d58c) - (rb_state11.var_23)) - (0xe1c686a3));
        rb_state11.var_27 = (rb_state11.var_27) + (((rb_state11.var_20) == (0x0)) ? (rb_state11.var_24) : (0x55ec2b6d));
        rb_state11.var_28 = (rb_state11.var_28) + (((0xd2c10808) + (rb_state11.var_25)) - (rb_state11.var_21));
        rb_state11.var_29 = (rb_state11.var_29) + (((rb_state11.var_26) == (0x0)) ? (rb_state11.var_27) : (0x423f4191));
        rb_state11.var_30 = (rb_state11.var_30) - (((rb_state11.var_28) == (0x0)) ? (rb_state11.var_29) : (0xc319736d));
        rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_30);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0x640814d7) ^ (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0xc04e5600));
    rb_state16.var_29 = (rb_state16.var_29) ^ ((0xc6dbbc56) ^ (0xd54e87d6));
    if ((rb_state16.var_30) == (0xbf229ac2)) {
        rb_state16.var_60 = (rb_state16.var_60) ^ (((rb_state16.var_38) == (0x0)) ? (rb_state16.var_40) : (0x10f01b2f));
        rb_state16.var_61 = (rb_state16.var_61) - (((((0x5383d94a) - (0x7fc93813)) - (rb_state16.var_60)) ^ (rb_state16.var_41)) + (rb_state16.var_39));
        rb_state16.var_45 = (rb_state16.var_45) - (rb_state16.var_61);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_33));
        rb_state17.var_21 = 0x98eb669;
        rb_state17.var_22 = 0xab63c1ba;
        rb_state17.var_23 = (rb_state17.var_23) - (((0x181aa592) ^ (rb_state17.var_11)) ^ (rb_state17.var_21));
        rb_state17.var_24 = (rb_state17.var_24) ^ (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_10) : (0xed21f929));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_24) : (0x4e87381c));
        rb_state17.var_20 = (rb_state17.var_20) ^ (rb_state17.var_25);
        rb_state17.var_3 = rb_state17.var_20;
        pthread_mutex_unlock(&(rb_state17.lock_33));
    }
    if ((rb_state17.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_33));
        rb_state17.var_27 = 0x8ab3d7f6;
        rb_state17.var_28 = 0xb6b805af;
        rb_state17.var_29 = (rb_state17.var_29) ^ (((((0xd2e58722) + (rb_state17.var_12)) - (rb_state17.var_13)) + (0x9fab514f)) + (rb_state17.var_27));
        rb_state17.var_30 = (rb_state17.var_30) + (rb_state17.var_28);
        rb_state17.var_31 = (rb_state17.var_31) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_29) : (0x76c36bd4));
        rb_state17.var_32 = (rb_state17.var_32) + (((rb_state17.var_30) == (0x0)) ? (rb_state17.var_31) : (0x985367da));
        rb_state17.var_26 = (rb_state17.var_26) + (rb_state17.var_32);
        rb_state17.var_3 = rb_state17.var_26;
        pthread_mutex_unlock(&(rb_state17.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x66)) {
        rb_state19.var_3 = rb_state19.var_0;
    }
    if ((rb_state19.var_2) == (0x66)) {
        if (!((rb_state19.var_0) == (rb_state19.var_3))) {
            racebench_trigger(19);
        }
    }
    #endif
    release_(&(threads_lock), file, line);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (3489 < rb_input_size ? (uint32_t)rb_input[3489] : 0x8e06439e);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_2) == (0xfffffe4d)) {
        pthread_mutex_lock(&(rb_state4.lock_32));
        rb_state4.var_27 = 0x8aed7cc;
        rb_state4.var_28 = 0xd88aa354;
        rb_state4.var_29 = (rb_state4.var_29) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_28) : (0xcaa058a5));
        rb_state4.var_30 = (rb_state4.var_30) - (((((0xbb558acf) ^ (rb_state4.var_14)) ^ (0x9bd1ad7e)) - (0x5f104637)) + (rb_state4.var_27));
        rb_state4.var_31 = (rb_state4.var_31) ^ (((((0xabaaa4c7) - (rb_state4.var_16)) ^ (rb_state4.var_30)) - (0xf72932a7)) + (rb_state4.var_29));
        rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_31);
        pthread_mutex_unlock(&(rb_state4.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
    rb_state11.var_2 = (rb_state11.var_2) ^ (3623 < rb_input_size ? (uint32_t)rb_input[3623] : 0x48df8d8b);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_2);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - ((0xd035312a) ^ (rb_state15.var_1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (3489 < rb_input_size ? (uint32_t)rb_input[3489] : 0x46579c3);
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