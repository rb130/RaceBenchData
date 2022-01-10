#include "racebench_bugs.h"


#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef ENABLE_PTHREADS
#include <pthread.h>
#endif

#ifdef ENABLE_DMALLOC
#include <dmalloc.h>
#endif

#include "mbuffer.h"

#ifdef ENABLE_PTHREADS

#define ENABLE_SPIN_LOCKS

#ifdef ENABLE_SPIN_LOCKS
typedef pthread_spinlock_t pthread_lock_t;
#define PTHREAD_LOCK_INIT(l) pthread_spin_init(l, PTHREAD_PROCESS_PRIVATE)
#define PTHREAD_LOCK_DESTROY(l) pthread_spin_destroy(l)
#define PTHREAD_LOCK(l) pthread_spin_lock(l)
#define PTHREAD_UNLOCK(l) pthread_spin_unlock(l)
#else
typedef pthread_mutex_t pthread_lock_t;
#define PTHREAD_LOCK_INIT(l) pthread_mutex_init(l, NULL)
#define PTHREAD_LOCK_DESTROY(l) pthread_mutex_destroy(l)
#define PTHREAD_LOCK(l) pthread_mutex_lock(l)
#define PTHREAD_UNLOCK(l) pthread_mutex_unlock(l)
#endif

pthread_lock_t *locks = NULL;

#define NUMBER_OF_LOCKS 1021

static inline int lock_hash(void *p) {
    return (int)(((unsigned long long int)p) % NUMBER_OF_LOCKS);
}
#endif

int mbuffer_system_init() {
#ifdef ENABLE_PTHREADS
    int i;

    assert(locks == NULL);
    locks = malloc(NUMBER_OF_LOCKS * sizeof(pthread_lock_t));
    if (locks == NULL) {
        return -1;
    }
    for (i = 0; i < NUMBER_OF_LOCKS; i++) {
        if (PTHREAD_LOCK_INIT(&locks[i]) != 0) {
            int j;
            for (j = 0; j < i; j++) {
                PTHREAD_LOCK_DESTROY(&locks[i]);
            }
            free((void *)locks);
            locks = NULL;
            return -1;
        }
    }
#endif
    return 0;
}

int mbuffer_system_destroy() {
#ifdef ENABLE_PTHREADS
    int i, rv;
    rv = 0;
    for (i = 0; i < NUMBER_OF_LOCKS; i++) {
        rv += PTHREAD_LOCK_DESTROY(&locks[i]);
    }
    free((void *)locks);
    locks = NULL;
    return rv ? -1 : 0;
#else
    return 0;
#endif
}

int mbuffer_create(mbuffer_t *m, size_t size) {
    void *ptr;

    assert(m != NULL);
    assert(size > 0);

    ptr = malloc(size);
    if (ptr == NULL) {
        return -1;
    }
    m->mcb = (mcb_t *)malloc(sizeof(mcb_t));
    if (m->mcb == NULL) {
        free(ptr);
        ptr = NULL;
        return -1;
    }

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x65c64189));
    #endif
    m->ptr = ptr;
    m->n = size;
    m->mcb->i = 1;
    m->mcb->ptr = ptr;
#ifdef ENABLE_MBUFFER_CHECK
    m->check_flag = MBUFFER_CHECK_MAGIC;
#endif

    return 0;
}

mbuffer_t *mbuffer_clone(mbuffer_t *m) {
    mbuffer_t *temp;

    assert(m != NULL);
#ifdef ENABLE_MBUFFER_CHECK
    assert(m->check_flag == MBUFFER_CHECK_MAGIC);
#endif

    temp = malloc(sizeof(mbuffer_t));
    if (temp == NULL) {
        return NULL;
    }

#ifdef ENABLE_PTHREADS
    PTHREAD_LOCK(&locks[lock_hash(m->mcb)]);
    assert(m->mcb->i >= 1);
    m->mcb->i++;
    PTHREAD_UNLOCK(&locks[lock_hash(m->mcb)]);
#else
    assert(m->mcb->i >= 1);
    m->mcb->i++;
#endif

    temp->ptr = m->ptr;
    temp->n = m->n;
    temp->mcb = m->mcb;
#ifdef ENABLE_MBUFFER_CHECK
    temp->check_flag = MBUFFER_CHECK_MAGIC;
#endif

    return temp;
}

mbuffer_t *mbuffer_copy(mbuffer_t *m) {
    mbuffer_t *temp;

    assert(m != NULL);
    assert(m->n >= 1);
#ifdef ENABLE_MBUFFER_CHECK
    assert(m->check_flag == MBUFFER_CHECK_MAGIC);
#endif

    temp = malloc(sizeof(mbuffer_t));
    if (temp == NULL) {
        return NULL;
    }
    if (mbuffer_create(temp, m->n) != 0) {
        free(temp);
        temp = NULL;
        return NULL;
    }
    memcpy(temp->ptr, m->ptr, m->n);
#ifdef ENABLE_MBUFFER_CHECK
    temp->check_flag = MBUFFER_CHECK_MAGIC;
#endif

    return temp;
}

void mbuffer_free(mbuffer_t *m) {
    unsigned int ref;

    assert(m != NULL);
#ifdef ENABLE_MBUFFER_CHECK
    assert(m->check_flag == MBUFFER_CHECK_MAGIC);
#endif

#ifdef ENABLE_PTHREADS
    PTHREAD_LOCK(&locks[lock_hash(m->mcb)]);
    m->mcb->i--;
    ref = m->mcb->i;
    PTHREAD_UNLOCK(&locks[lock_hash(m->mcb)]);
#else
    m->mcb->i--;
    ref = m->mcb->i;
#endif

    if (ref == 0) {
        free(m->mcb->ptr);
        m->mcb->ptr = NULL;
        free(m->mcb);
        m->mcb = NULL;
    }
#ifdef ENABLE_MBUFFER_CHECK
    m->check_flag = 0;
#endif
}

int mbuffer_realloc(mbuffer_t *m, size_t size) {
    void *r;

    assert(m != NULL);
    assert(size > 0);
#ifdef ENABLE_MBUFFER_CHECK
    assert(m->check_flag == MBUFFER_CHECK_MAGIC);
#endif

#ifdef ENABLE_PTHREADS
    PTHREAD_LOCK(&locks[lock_hash(m->mcb)]);
#endif

    if (m->mcb->i > 1) {
#ifdef ENABLE_PTHREADS
        PTHREAD_UNLOCK(&locks[lock_hash(m->mcb)]);
#endif
        return -1;
    }

    if (m->ptr != m->mcb->ptr) {
#ifdef ENABLE_PTHREADS
        PTHREAD_UNLOCK(&locks[lock_hash(m->mcb)]);
#endif
        return -1;
    }

    r = realloc(m->ptr, size);
    if (r != NULL) {
        m->ptr = r;
        m->n = size;
        m->mcb->ptr = r;
    }

#ifdef ENABLE_PTHREADS
    PTHREAD_UNLOCK(&locks[lock_hash(m->mcb)]);
#endif

    if (r == NULL) {
        return -1;
    } else {
        return 0;
    }
}

int mbuffer_split(mbuffer_t *m1, mbuffer_t *m2, size_t split) {
    assert(m1 != NULL);
    assert(m2 != NULL);
    assert(split > 0);
    assert(split < m1->n);
#ifdef ENABLE_MBUFFER_CHECK
    assert(m1->check_flag == MBUFFER_CHECK_MAGIC);
    assert(m2->check_flag != MBUFFER_CHECK_MAGIC);
#endif

#ifdef ENABLE_PTHREADS
    PTHREAD_LOCK(&locks[lock_hash(m1->mcb)]);
    assert(m1->mcb->i >= 1);
    m1->mcb->i++;
    PTHREAD_UNLOCK(&locks[lock_hash(m1->mcb)]);
#else
    assert(m1->mcb->i >= 1);
    m1->mcb->i++;
#endif

    m2->ptr = m1->ptr + split;
    m2->n = m1->n - split;
    m2->mcb = m1->mcb;
    m1->n = split;
#ifdef ENABLE_MBUFFER_CHECK
    m2->check_flag = MBUFFER_CHECK_MAGIC;
#endif

    return 0;
}