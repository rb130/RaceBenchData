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
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x46e73e55));
                rb_state0.var_0 = (rb_state0.var_0) + (0x92e4de0c);
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) - (0x81417790);
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x78026829));
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - (0x68b40c6f);
                rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x974bf391)) ? (rb_state7.var_0) : (0x753c8996));
                if ((rb_state7.var_0) == (0x88451d5f)) {
                    rb_state7.var_2 = rb_state7.var_1;
                }
                if ((rb_state7.var_0) == (0xaa54875a)) {
                    rb_state7.var_3 = 0xc5fd5d25;
                    rb_state7.var_4 = (rb_state7.var_4) ^ (((0xa7df8ecb) + (rb_state7.var_4)) - (rb_state7.var_3));
                    rb_state7.var_5 = (rb_state7.var_5) - (0x416586d);
                    rb_state7.var_6 = (rb_state7.var_6) + (rb_state7.var_5);
                    rb_state7.var_7 = (rb_state7.var_7) + (((0x73b16d34) ^ (rb_state7.var_6)) - (rb_state7.var_2));
                    rb_state7.var_8 = (rb_state7.var_8) - (((((0xb269103b) - (rb_state7.var_5)) - (0x69976331)) + (rb_state7.var_7)) ^ (rb_state7.var_4));
                    rb_state7.var_9 = (rb_state7.var_9) ^ (rb_state7.var_6);
                    rb_state7.var_10 = (rb_state7.var_10) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_7) : (0x88dae16a));
                    rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_9) : (0xe229a59e));
                    rb_state7.var_12 = (rb_state7.var_12) + (rb_state7.var_10);
                    rb_state7.var_13 = (rb_state7.var_13) + (((((0xe31941ba) ^ (rb_state7.var_11)) + (rb_state7.var_12)) ^ (rb_state7.var_9)) + (0x9b9deee));
                    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_13);
                }
                if ((rb_state7.var_0) == (0xaa54875a)) {
                    if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                        racebench_trigger(7);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (0xc5b833e5);
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) + (0xc5b459b5);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) + (32770 < rb_input_size ? (uint32_t)rb_input[32770] : 0x4a6beccd);
                #endif
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x95d60636));
                rb_state15.var_0 = (rb_state15.var_0) + (0xcb1b570e);
                if ((rb_state15.var_0) == (0xae7b7ebe)) {
                    pthread_mutex_lock(&(rb_state15.lock_30));
                    rb_state15.var_3 = 0x4dffe252;
                    rb_state15.var_4 = 0xbc6e00d;
                    rb_state15.var_5 = (rb_state15.var_5) + (rb_state15.var_2);
                    rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_4);
                    rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_1) : (0x5c67eee));
                    rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_5) : (0xbd02a83e));
                    rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_6) : (0xf7cbeabb));
                    rb_state15.var_10 = (rb_state15.var_10) + (rb_state15.var_7);
                    rb_state15.var_11 = (rb_state15.var_11) + (((0x67526bf2) ^ (rb_state15.var_8)) - (rb_state15.var_5));
                    rb_state15.var_12 = (rb_state15.var_12) + (((0xa456347) - (rb_state15.var_9)) ^ (rb_state15.var_6));
                    rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_10) : (0x222ad11d));
                    rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_11);
                    rb_state15.var_15 = (rb_state15.var_15) + (((0xaf9e76ed) ^ (rb_state15.var_8)) + (rb_state15.var_12));
                    rb_state15.var_16 = (rb_state15.var_16) ^ (((0x669480d7) - (rb_state15.var_13)) - (0x47f9d86));
                    rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_15) : (0xebddead8));
                    rb_state15.var_18 = (rb_state15.var_18) - (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_17) : (0x6e2987ff));
                    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_18);
                    rb_state15.var_1 = rb_state15.var_2;
                    pthread_mutex_unlock(&(rb_state15.lock_30));
                }
                if ((rb_state15.var_0) == (0x6c8cf96)) {
                    rb_state15.var_19 = 0x62504a48;
                    rb_state15.var_20 = (rb_state15.var_20) + (rb_state15.var_19);
                    rb_state15.var_21 = (rb_state15.var_21) - (rb_state15.var_9);
                    rb_state15.var_22 = (rb_state15.var_22) + (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_11) : (0x733ed1e3));
                    rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_10) : (0x435468cb));
                    rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_21) : (0x4345632f));
                    rb_state15.var_25 = (rb_state15.var_25) ^ (((((0xeb47c669) ^ (rb_state15.var_22)) ^ (rb_state15.var_14)) - (rb_state15.var_23)) + (0xe8856190));
                    rb_state15.var_26 = (rb_state15.var_26) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_24) : (0xdab9f531));
                    rb_state15.var_27 = (rb_state15.var_27) - (rb_state15.var_25);
                    rb_state15.var_28 = (rb_state15.var_28) + (((0x8377e13f) - (0xac384fbc)) ^ (rb_state15.var_26));
                    rb_state15.var_29 = (rb_state15.var_29) + (((((0xd9c57911) - (rb_state15.var_28)) - (0x133cc079)) ^ (0x341618af)) - (rb_state15.var_27));
                    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_29);
                }
                if ((rb_state15.var_0) == (0x6c8cf96)) {
                    if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                        racebench_trigger(15);
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