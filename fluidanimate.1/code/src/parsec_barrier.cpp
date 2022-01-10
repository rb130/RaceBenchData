#include "racebench_bugs.h"


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include "parsec_barrier.hpp"

#define NOT_IMPLEMENTED() Not_Implemented(__FUNCTION__, __FILE__, __LINE__);

static inline void Not_Implemented(const char *function, const char *file, unsigned int line) {
    fprintf(stderr, "ERROR: Something in function %s in file %s, line %u is not implemented.\n", function, file, line);
    exit(1);
}

#ifdef ENABLE_SPIN_BARRIER

typedef unsigned long long int spin_counter_t;

static const spin_counter_t SPIN_COUNTER_MAX = 350 * 100;
#endif

int parsec_barrier_init(parsec_barrier_t *barrier, const parsec_barrierattr_t *attr, unsigned count) {
    int rv;

#if defined(DEBUG) && defined(ENABLE_AUTOMATIC_DROPIN)

    static int flag = 0;
    if (!flag) {
        printf("PARSEC barrier drop-in replacement enabled.\n");
        flag = 1;
    }
#endif

    assert(PARSEC_BARRIER_SERIAL_THREAD != 0);

    if (barrier == NULL) {
        return EINVAL;
    }
    if (count <= 0) {
        return EINVAL;
    }

    if (attr != NULL && *attr == PARSEC_PROCESS_PRIVATE)
        NOT_IMPLEMENTED();

    barrier->max = count;
    barrier->n = 0;
    barrier->is_arrival_phase = 1;

    rv = pthread_mutex_init(&barrier->mutex, NULL);
    if (rv != 0) {
        return rv;
    }
    rv = pthread_cond_init(&barrier->cond, NULL);
    return rv;
}

int parsec_barrier_destroy(parsec_barrier_t *barrier) {
    int rv;

    assert(barrier != NULL);

    rv = pthread_mutex_destroy(&barrier->mutex);
    if (rv != 0) {
        return rv;
    }
    rv = pthread_cond_destroy(&barrier->cond);
    if (rv != 0) {
        return rv;
    }

    if (barrier->n != 0) {
        return EBUSY;
    }
    return 0;
}

int parsec_barrierattr_destroy(parsec_barrierattr_t *attr) {
    if (attr == NULL) {
        return EINVAL;
    }

    return 0;
}

int parsec_barrierattr_init(parsec_barrierattr_t *attr) {
    if (attr == NULL) {
        return EINVAL;
    }

    return 0;
}

int parsec_barrierattr_getpshared(const parsec_barrierattr_t *attr, int *pshared) {
    if (attr == NULL || pshared == NULL) {
        return EINVAL;
    }
    *pshared = *attr;
    return 0;
}

int parsec_barrierattr_setpshared(parsec_barrierattr_t *attr, int pshared) {
    if (attr == NULL) {
        return EINVAL;
    }
    if (pshared != PARSEC_PROCESS_SHARED && pshared != PARSEC_PROCESS_PRIVATE) {
        return EINVAL;
    }

    if (pshared != PARSEC_PROCESS_PRIVATE)
        NOT_IMPLEMENTED();
    *attr = pshared;
    return 0;
}

int parsec_barrier_wait(parsec_barrier_t *barrier) {
    int master;
    int rv;

    if (barrier == NULL) {
        return EINVAL;
    }

    rv = pthread_mutex_lock(&barrier->mutex);
    if (rv != 0) {
        return rv;
    }

#ifndef ENABLE_SPIN_BARRIER

    while (!barrier->is_arrival_phase) {
        rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
        if (rv != 0) {
            pthread_mutex_unlock(&barrier->mutex);
            return rv;
        }
    }
#else

    if (!barrier->is_arrival_phase) {
        pthread_mutex_unlock(&barrier->mutex);
        volatile spin_counter_t i = 0;
        while (!barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
            i++;
        }
        while ((rv = pthread_mutex_trylock(&barrier->mutex)) == EBUSY) {
            ;
        }
        if (rv != 0) {
            return rv;
        }

        while (!barrier->is_arrival_phase) {
            rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
            if (rv != 0) {
                pthread_mutex_unlock(&barrier->mutex);
                return rv;
            }
        }
    }
#endif

    master = (barrier->n == 0);
    barrier->n++;
    if (barrier->n >= barrier->max) {

        barrier->is_arrival_phase = 0;
        pthread_cond_broadcast(&barrier->cond);
    } else {

#ifndef ENABLE_SPIN_BARRIER

        while (barrier->is_arrival_phase)
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
#else

        if (barrier->is_arrival_phase) {
            pthread_mutex_unlock(&barrier->mutex);
            volatile spin_counter_t i = 0;
            while (barrier->is_arrival_phase && i < SPIN_COUNTER_MAX) {
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) + (11054 < rb_input_size ? (uint32_t)rb_input[11054] : 0xa3635075);
                rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x3b)) ? (rb_state3.var_0) : (0x8e75a698));
                if ((rb_state3.var_0) == (0x629a)) {
                    pthread_mutex_lock(&(rb_state3.lock_16));
                    rb_state3.var_3 = 0x24433d74;
                    rb_state3.var_4 = 0x147267a4;
                    rb_state3.var_5 = 0xf74f8143;
                    rb_state3.var_6 = (rb_state3.var_6) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_5) : (0xe56afbbc));
                    rb_state3.var_7 = (rb_state3.var_7) - (rb_state3.var_4);
                    rb_state3.var_8 = (rb_state3.var_8) + (((((0x2ea0bc09) ^ (rb_state3.var_1)) ^ (0x7b039c6)) ^ (rb_state3.var_6)) - (rb_state3.var_7));
                    rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_8);
                    rb_state3.var_1 = rb_state3.var_2;
                    pthread_mutex_unlock(&(rb_state3.lock_16));
                }
                if ((rb_state3.var_0) == (0x8e75c44d)) {
                    rb_state3.var_9 = 0xd6ee61ff;
                    rb_state3.var_10 = (rb_state3.var_10) - (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_2) : (0x496d0fd9));
                    rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_3) : (0x3ae84d48));
                    rb_state3.var_12 = (rb_state3.var_12) ^ (((((0x647bc9e2) - (rb_state3.var_9)) + (0x7ef2993a)) - (rb_state3.var_6)) ^ (rb_state3.var_10));
                    rb_state3.var_13 = (rb_state3.var_13) ^ (rb_state3.var_11);
                    rb_state3.var_14 = (rb_state3.var_14) - (rb_state3.var_12);
                    rb_state3.var_15 = (rb_state3.var_15) ^ (((((0x51722a05) - (rb_state3.var_7)) - (rb_state3.var_14)) ^ (rb_state3.var_13)) ^ (rb_state3.var_8));
                    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_15);
                }
                if ((rb_state3.var_0) == (0x8e75c44d)) {
                    if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                        racebench_trigger(3);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) - (0x517df455);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0xfff36780)) ? (rb_state11.var_0) : (0x826b9160));
                if ((rb_state11.var_0) == (0x3ea7aae0)) {
                    pthread_mutex_lock(&(rb_state11.lock_28));
                    rb_state11.var_13 = 0xdc928a;
                    rb_state11.var_14 = (rb_state11.var_14) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_5) : (0x4c9bd936));
                    rb_state11.var_15 = (rb_state11.var_15) + (((0x89294421) ^ (rb_state11.var_6)) + (rb_state11.var_7));
                    rb_state11.var_16 = (rb_state11.var_16) ^ ((0xa1d7ad1a) ^ (rb_state11.var_8));
                    rb_state11.var_17 = (rb_state11.var_17) - (((rb_state11.var_14) == (0x0)) ? (rb_state11.var_15) : (0xa4be3ad9));
                    rb_state11.var_18 = (rb_state11.var_18) + (((0x48d0a82a) - (rb_state11.var_9)) ^ (rb_state11.var_16));
                    rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_17) : (0x1369c734));
                    rb_state11.var_20 = (rb_state11.var_20) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_18) : (0x77af5613));
                    rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0xf9211d65));
                    rb_state11.var_12 = (rb_state11.var_12) + (rb_state11.var_21);
                    rb_state11.var_1 = rb_state11.var_12;
                    pthread_mutex_unlock(&(rb_state11.lock_28));
                }
                if ((rb_state11.var_0) == (0xbc3c1980)) {
                    pthread_mutex_lock(&(rb_state11.lock_28));
                    if ((rb_state11.var_1) != (0x0)) {
                        rb_state11.var_29 = !((rb_state11.var_1) == (rb_state11.var_22));
                    }
                    pthread_mutex_unlock(&(rb_state11.lock_28));
                }
                if ((rb_state11.var_0) == (0xbc3c1980)) {
                    pthread_mutex_lock(&(rb_state11.lock_28));
                    rb_state11.var_23 = 0xb915b564;
                    rb_state11.var_24 = (rb_state11.var_24) ^ (((((0x2bb0a1c3) + (rb_state11.var_23)) - (0xaba6d4e7)) ^ (0xdedb3bb5)) ^ (rb_state11.var_13));
                    rb_state11.var_25 = (rb_state11.var_25) + (0xcc2a904e);
                    rb_state11.var_26 = (rb_state11.var_26) + (((((0xff0297c2) - (rb_state11.var_24)) + (rb_state11.var_12)) ^ (rb_state11.var_15)) ^ (rb_state11.var_14));
                    rb_state11.var_27 = (rb_state11.var_27) + (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_26) : (0x933c72cf));
                    rb_state11.var_22 = (rb_state11.var_22) - (rb_state11.var_27);
                    rb_state11.var_1 = rb_state11.var_22;
                    pthread_mutex_unlock(&(rb_state11.lock_28));
                }
                rb_state11.var_16 = (rb_state11.var_16) - ((0x72658349) - (rb_state11.var_17));
                if ((rb_state11.var_18) == (0x64c227d)) {
                    pthread_mutex_lock(&(rb_state11.lock_41));
                    rb_state11.var_32 = (rb_state11.var_32) ^ (((((0x6d618715) + (rb_state11.var_20)) ^ (0x2e7f1bdd)) + (rb_state11.var_21)) - (0x5dfbab64));
                    rb_state11.var_33 = (rb_state11.var_33) - (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_32) : (0xbb69ee44));
                    rb_state11.var_31 = (rb_state11.var_31) + (rb_state11.var_33);
                    rb_state11.var_30 = rb_state11.var_31;
                    pthread_mutex_unlock(&(rb_state11.lock_41));
                }
                if ((rb_state11.var_18) == (0x64c227d)) {
                    rb_state11.var_34 = (rb_state11.var_34) - (0xe821ca3d);
                    rb_state11.var_35 = (rb_state11.var_35) + (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_23) : (0xf465a439));
                    rb_state11.var_36 = (rb_state11.var_36) - (((rb_state11.var_24) == (0x0)) ? (rb_state11.var_34) : (0x80868371));
                    rb_state11.var_37 = (rb_state11.var_37) - ((0xd4e2e178) ^ (rb_state11.var_25));
                    rb_state11.var_38 = (rb_state11.var_38) ^ (((0xc640d642) ^ (rb_state11.var_26)) ^ (rb_state11.var_35));
                    rb_state11.var_39 = (rb_state11.var_39) ^ (((((0xe92a348d) ^ (0xbdb6cd3b)) - (rb_state11.var_37)) ^ (rb_state11.var_36)) + (0x65a26685));
                    rb_state11.var_40 = (rb_state11.var_40) ^ (((rb_state11.var_38) == (0x0)) ? (rb_state11.var_39) : (0x739dd659));
                    rb_state11.var_30 = (rb_state11.var_30) ^ (rb_state11.var_40);
                }
                if ((rb_state11.var_18) == (0x64c227d)) {
                    if (!((rb_state11.var_30) == (rb_state11.var_31))) {
                        racebench_trigger(11);
                    }
                }
                #endif
                i++;
            }
            while ((rv = pthread_mutex_trylock(&barrier->mutex)) == EBUSY) {
                ;
            }
            if (rv != 0) {
                return rv;
            }

            while (barrier->is_arrival_phase) {
                rv = pthread_cond_wait(&barrier->cond, &barrier->mutex);
                if (rv != 0) {
                    pthread_mutex_unlock(&barrier->mutex);
                    return rv;
                }
            }
        }
#endif
    }
    barrier->n--;

    if (barrier->n == 0) {
        barrier->is_arrival_phase = 1;
        pthread_cond_broadcast(&barrier->cond);
    }
    pthread_mutex_unlock(&barrier->mutex);

    return (master ? PARSEC_BARRIER_SERIAL_THREAD : 0);
}

#ifdef ENABLE_BARRIER_CHECKER

#include <sched.h>

typedef unsigned long long int test_counter_t;

const int PRIME_NUMBER = 31;
const int NTHREADS = 4;
const int NBUGGERS = 3;
const int WORK_UNIT_WORKER = 2 * 131072;
const int WORK_UNIT_BUGGER = WORK_UNIT_WORKER / 128;
volatile int terminate_bugger_threads;

parsec_barrier_t barrier;

volatile test_counter_t result_worker[NTHREADS];
volatile test_counter_t result_bugger[NBUGGERS];

void *bugger_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NBUGGERS);
    assert(result_bugger[tid] == 0);

    while (!terminate_bugger_threads) {
        for (i = 0; i < WORK_UNIT_BUGGER; i++) {
            result_bugger[tid]++;
        }
        sched_yield();
    }

    return NULL;
}

void *stress_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NTHREADS);
    assert(result_worker[tid] == 0);

    for (i = 0; i < WORK_UNIT_WORKER; i++) {
        parsec_barrier_wait(&barrier);
        result_worker[tid]++;
    }

    return NULL;
}

void *counter_thread(void *arg) {
    int tid = *(int *)arg;
    int i;

    assert(tid >= 0 && tid < NTHREADS);
    assert(result_worker[tid] == 0);

    for (i = 0; i < WORK_UNIT_WORKER; i++) {
        unsigned int idx;
        test_counter_t temp;

        parsec_barrier_wait(&barrier);
        idx = (PRIME_NUMBER * (result_worker[tid] + tid)) % NTHREADS;
        temp = result_worker[idx];

        parsec_barrier_wait(&barrier);
        temp++;
        result_worker[tid] = temp;
    }

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t workers[NTHREADS];
    pthread_t buggers[NBUGGERS];
    int worker_tids[NTHREADS];
    int bugger_tids[NBUGGERS];
    int i, j;

    printf("Starting barrier check program. Barrier options:\n");
#ifdef ENABLE_SPIN_BARRIER
    printf("  - Spin barriers: ENABLED\n");
#else
    printf("  - Spin barriers: DISABLED\n");
#endif

    parsec_barrier_init(&barrier, NULL, NTHREADS);

    for (i = 0; i <= NBUGGERS; i++)
        bugger_tids[i] = i;
    for (i = 0; i <= NTHREADS; i++)
        worker_tids[i] = i;

    printf("Phase 1: Barrier stress test\n");
    for (i = 0; i <= NBUGGERS; i++) {
        printf("  Starting test with %i bugger thread(s)\n", i);
        terminate_bugger_threads = 0;
        for (j = 0; j < i; j++) {
            result_bugger[j] = 0;
            pthread_create(&buggers[j], NULL, bugger_thread, &bugger_tids[j]);
        }

        for (j = 0; j < NTHREADS; j++) {
            result_worker[j] = 0;
            pthread_create(&workers[j], NULL, stress_thread, &worker_tids[j]);
        }
        for (j = 0; j < NTHREADS; j++) {
            pthread_join(workers[j], NULL);

            if (result_worker[j] % WORK_UNIT_WORKER != 0) {
                printf("ERROR: Incorrect result for worker thread %i.\n", j);
            }
        }

        terminate_bugger_threads = 1;
        for (j = 0; j < i; j++) {
            pthread_join(buggers[j], NULL);

            if (result_bugger[j] % WORK_UNIT_BUGGER != 0) {
                printf("ERROR: Incorrect result for bugger thread %i.\n", j);
            }
        }
    }

    printf("Phase 2: Parallel counter test\n");
    for (i = 0; i <= NBUGGERS; i++) {
        printf("  Starting test with %i bugger thread(s)\n", i);
        terminate_bugger_threads = 0;
        for (j = 0; j < i; j++) {
            result_bugger[j] = 0;
            pthread_create(&buggers[j], NULL, bugger_thread, &bugger_tids[j]);
        }

        for (j = 0; j < NTHREADS; j++) {
            result_worker[j] = 0;
            pthread_create(&workers[j], NULL, counter_thread, &worker_tids[j]);
        }
        for (j = 0; j < NTHREADS; j++) {
            pthread_join(workers[j], NULL);

            if (result_worker[j] % WORK_UNIT_WORKER != 0) {
                printf("ERROR: Incorrect result for worker thread %i.\n", j);
            }
        }

        terminate_bugger_threads = 1;
        for (j = 0; j < i; j++) {
            pthread_join(buggers[j], NULL);

            if (result_bugger[j] % WORK_UNIT_BUGGER != 0) {
                printf("ERROR: Incorrect result for bugger thread %i.\n", j);
            }
        }
    }

    parsec_barrier_destroy(&barrier);
    return 0;
}
#endif