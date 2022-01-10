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
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0xfffffca4)) ? (rb_state7.var_0) : (0x8b0b1bb6));
                rb_state7.var_0 = (rb_state7.var_0) + (38143 < rb_input_size ? (uint32_t)rb_input[38143] : 0xccd34f38);
                rb_state7.var_0 = (rb_state7.var_0) - (32507 < rb_input_size ? (uint32_t)rb_input[32507] : 0xb1de904b);
                if ((rb_state7.var_0) == (0xd35fdcbc)) {
                    rb_state7.var_3 = 0xfb2e7f9e;
                    rb_state7.var_4 = (rb_state7.var_4) ^ (((((0x247fc189) + (rb_state7.var_4)) - (rb_state7.var_2)) - (rb_state7.var_3)) ^ (rb_state7.var_3));
                    rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_4);
                    rb_state7.var_1 = rb_state7.var_2;
                }
                if ((rb_state7.var_0) == (0x5e6af871)) {
                    pthread_mutex_lock(&(rb_state7.lock_13));
                    if ((rb_state7.var_1) != (0x0)) {
                        if (!((rb_state7.var_1) == (rb_state7.var_5))) {
                            racebench_trigger(7);
                        }
                    }
                    pthread_mutex_unlock(&(rb_state7.lock_13));
                }
                if ((rb_state7.var_0) == (0x5e6af871)) {
                    pthread_mutex_lock(&(rb_state7.lock_13));
                    rb_state7.var_6 = 0x89540979;
                    rb_state7.var_7 = 0xe8377f02;
                    rb_state7.var_8 = 0x67bc55bc;
                    rb_state7.var_9 = (rb_state7.var_9) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0x7af610d5));
                    rb_state7.var_10 = (rb_state7.var_10) - (((0x5bbe5f02) - (rb_state7.var_5)) - (rb_state7.var_6));
                    rb_state7.var_11 = (rb_state7.var_11) ^ (rb_state7.var_9);
                    rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0x1efb8db1));
                    rb_state7.var_5 = (rb_state7.var_5) ^ (rb_state7.var_12);
                    rb_state7.var_1 = rb_state7.var_5;
                    pthread_mutex_unlock(&(rb_state7.lock_13));
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