#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <vips/vips.h>
#include <vips/thread.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void im_semaphore_init(im_semaphore_t *s, int v, char *name) {
    s->v = v;
    s->name = name;
#ifdef HAVE_THREADS
    s->mutex = g_mutex_new();
    s->cond = g_cond_new();
#endif
}

void im_semaphore_destroy(im_semaphore_t *s) {
#ifdef HAVE_THREADS
    IM_FREEF(g_mutex_free, s->mutex);
    IM_FREEF(g_cond_free, s->cond);
#endif
}

int im_semaphore_upn(im_semaphore_t *s, int n) {
    int value_after_op;

#ifdef HAVE_THREADS
    g_mutex_lock(s->mutex);
#endif
    s->v += n;
    value_after_op = s->v;
#ifdef HAVE_THREADS
    g_mutex_unlock(s->mutex);
    g_cond_signal(s->cond);
#endif

#ifdef DEBUG_IO
    printf("im_semaphore_upn(\"%s\",%d) = %d\n", s->name, n, value_after_op);
    if (value_after_op > 1)
        im_errormsg("up over 1!");
#endif

    return (value_after_op);
}

int im_semaphore_up(im_semaphore_t *s) {
    return (im_semaphore_upn(s, 1));
}

int im_semaphore_downn(im_semaphore_t *s, int n) {
    int value_after_op;

#ifdef HAVE_THREADS
    g_mutex_lock(s->mutex); racebench_bug7(0); racebench_bug7(1);
    while (s->v < n) {
        g_cond_wait(s->cond, s->mutex);
    }
#endif
    s->v -= n;
    value_after_op = s->v;
#ifdef HAVE_THREADS
    g_mutex_unlock(s->mutex);
#endif

#ifdef DEBUG_IO
    printf("im_semaphore_downn(\"%s\",%d): %d\n", s->name, n, value_after_op);
#endif

    return (value_after_op);
}

int im_semaphore_down(im_semaphore_t *s) {
    return (im_semaphore_downn(s, 1));
}