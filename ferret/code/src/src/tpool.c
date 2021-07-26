#ifdef __cplusplus
extern "C"
#endif
void racebench_bug1(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug4(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug5(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug8(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug9(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug10(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug11(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug13(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug14(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug16(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug18(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug20(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug21(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug22(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug23(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug26(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug27(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug28(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug29(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug30(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug32(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug34(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug35(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug36(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug37(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug40(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug41(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug45(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug47(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug48(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug53(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug57(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug58(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug60(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug65(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug66(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug68(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug69(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug71(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug73(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug75(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug76(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug77(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug78(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug81(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug85(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug87(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug88(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug90(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug91(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug94(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug97(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug98(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug99(int);

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

    pool = (tpool_t *)malloc(sizeof(tpool_t)); racebench_bug37(1); racebench_bug53(1); racebench_bug59(0); racebench_bug63(0); racebench_bug68(0);
    if (pool == NULL) {
        return NULL;
    }
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * nthreads); racebench_bug9(0); racebench_bug14(0); racebench_bug29(1); racebench_bug34(0); racebench_bug35(1); racebench_bug45(0); racebench_bug65(1); racebench_bug87(0); racebench_bug91(0); racebench_bug99(1);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    for (i = 0; i < nthreads; i++) {
        if (opts[i].attr == NULL) {
            attr = NULL; racebench_bug5(0); racebench_bug15(1); racebench_bug16(0); racebench_bug26(1); racebench_bug27(1); racebench_bug69(0); racebench_bug78(1); racebench_bug89(1); racebench_bug97(1);
        } else {
            attr = opts[i].attr;
        }
        if (opts[i].arg == NULL) {
            arg = NULL; racebench_bug4(0); racebench_bug17(1); racebench_bug36(1); racebench_bug60(1); racebench_bug76(0); racebench_bug80(0);
        } else {
            arg = opts[i].arg;
        }

        rv = pthread_create(&(pool->threads[i]), attr, opts[i].start_routine, arg); racebench_bug7(0); racebench_bug20(1); racebench_bug58(0); racebench_bug67(0); racebench_bug73(0); racebench_bug85(1); racebench_bug94(1);
        if (rv != 0) {
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    pool->nthreads = nthreads; racebench_bug11(0); racebench_bug13(1); racebench_bug18(0); racebench_bug21(0); racebench_bug41(0); racebench_bug49(0); racebench_bug75(1); racebench_bug77(0); racebench_bug82(0); racebench_bug86(1); racebench_bug88(1);
    pool->state = POOL_STATE_RUNNING; racebench_bug1(1); racebench_bug6(0); racebench_bug8(1); racebench_bug22(0); racebench_bug23(0); racebench_bug28(1); racebench_bug30(0); racebench_bug32(0); racebench_bug40(1); racebench_bug48(1); racebench_bug57(0); racebench_bug66(0); racebench_bug81(0);

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

    assert(pool != NULL); racebench_bug10(0); racebench_bug38(0); racebench_bug54(1); racebench_bug98(1);
    assert(pool->state == POOL_STATE_RUNNING); racebench_bug47(1);

    for (i = 0; i < pool->nthreads; i++) {
        if (value_ptrs != NULL) {
            value_ptr = &(value_ptrs[i]);
        } else {
            value_ptr = NULL; racebench_bug50(1); racebench_bug71(0); racebench_bug90(0);
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