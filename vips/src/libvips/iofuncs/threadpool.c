

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/thread.h>
#include <vips/debug.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_THREADS (1024)

#define IM_CONCURRENCY "IM_CONCURRENCY"

int im__tile_width = IM__TILE_WIDTH;
int im__tile_height = IM__TILE_HEIGHT;
int im__fatstrip_height = IM__FATSTRIP_HEIGHT;
int im__thinstrip_height = IM__THINSTRIP_HEIGHT;

int im__concurrency = 0;

#ifndef HAVE_THREADS

void im__g_thread_init(GThreadFunctions *vtable) {
}
gpointer im__g_thread_join(GThread *dummy) {
    return (NULL);
}
gpointer im__g_thread_self(void) {
    return (NULL);
}
GThread *im__g_thread_create_full(GThreadFunc d1, gpointer d2, gulong d3, gboolean d4, gboolean d5, GThreadPriority d6, GError **d7) {
    return (NULL);
}

GMutex *im__g_mutex_new(void) {
    return (NULL);
}
void im__g_mutex_free(GMutex *d) {
}
void im__g_mutex_lock(GMutex *d) {
}
void im__g_mutex_unlock(GMutex *d) {
}
#endif

void im_concurrency_set(int concurrency) {
    im__concurrency = concurrency;
}

static int get_num_processors(void) {
    int nproc;

    nproc = 1;

#ifdef G_OS_UNIX

#if defined(HAVE_UNISTD_H) && defined(_SC_NPROCESSORS_ONLN)
    {

        int x;

        x = sysconf(_SC_NPROCESSORS_ONLN);
        if (x > 0) {
            nproc = x;
        }
    }
#elif defined HW_NCPU
    {

        int x;
        size_t len = sizeof(x);

        sysctl((int[2]){CTL_HW, HW_NCPU}, 2, &x, &len, NULL, 0);
        if (x > 0)
            nproc = x;
    }
#endif

#endif

#ifdef OS_WIN32
    {

        DWORD_PTR process_cpus;
        DWORD_PTR system_cpus;

        if (GetProcessAffinityMask(GetCurrentProcess(), &process_cpus, &system_cpus)) {
            unsigned int count;

            for (count = 0; process_cpus != 0; process_cpus >>= 1)
                if (process_cpus & 1)
                    count++;

            if (count > 0)
                nproc = count;
        }
    }
#endif

    return (nproc);
}

int im_concurrency_get(void) {
    const char *str;
    int nthr;
    int x;

    if (im__concurrency > 0) {
        nthr = im__concurrency;
    } else if ((str = g_getenv(IM_CONCURRENCY)) && (x = atoi(str)) > 0) {
        nthr = x;
    } else {
        nthr = get_num_processors();
    }

    if (nthr < 1 || nthr > IM_MAX_THREADS) {
        nthr = IM_CLIP(1, nthr, IM_MAX_THREADS);

        im_warn("im_concurrency_get", _("threads clipped to %d"), nthr);
    }

    im_concurrency_set(nthr);

    return (nthr);
}

G_DEFINE_TYPE(VipsThreadState, vips_thread_state, VIPS_TYPE_OBJECT);

static void vips_thread_state_dispose(GObject *gobject) {
    VipsThreadState *state = (VipsThreadState *)gobject;

    VIPS_DEBUG_MSG("vips_thread_state_dispose:\n");

    IM_FREEF(im_region_free, state->reg);

    G_OBJECT_CLASS(vips_thread_state_parent_class)->dispose(gobject);
}

static int vips_thread_state_build(VipsObject *object) {
    VipsThreadState *state = (VipsThreadState *)object;

    if (!(state->reg = im_region_create(state->im))) {
        return (-1);
    }

    return (VIPS_OBJECT_CLASS(vips_thread_state_parent_class)->build(object));
}

static void vips_thread_state_class_init(VipsThreadStateClass *class) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(class);
    VipsObjectClass *object_class = VIPS_OBJECT_CLASS(class);

    gobject_class->dispose = vips_thread_state_dispose;

    object_class->build = vips_thread_state_build;
    object_class->nickname = "threadstate";
    object_class->description = _("per-thread state for vipsthreadpool");
}

static void vips_thread_state_init(VipsThreadState *state) {
    VIPS_DEBUG_MSG("vips_thread_state_init:\n");

    state->reg = NULL;
}

void *vips_thread_state_set(VipsObject *object, void *a, void *b) {
    VipsThreadState *state = (VipsThreadState *)object;
    VipsImage *im = (VipsImage *)a;

    VIPS_DEBUG_MSG("vips_thread_state_set:\n");

    state->im = im;
    state->a = b;

    return (NULL);
}

VipsThreadState *vips_thread_state_new(VipsImage *im, void *a) {
    VIPS_DEBUG_MSG("vips_thread_state_new:\n");

    return (VIPS_THREAD_STATE(vips_object_new(VIPS_TYPE_THREAD_STATE, vips_thread_state_set, im, a)));
}

typedef struct {

    struct _VipsThreadpool *pool;

    VipsThreadState *state;

    GThread *thread;

    gboolean exit;

    gboolean error;

#ifdef TIME_THREAD
    double *btime, *etime;
    int tpos;
#endif
} VipsThread;

typedef struct _VipsThreadpool {

    VipsImage *im;

    VipsThreadStart start;
    VipsThreadpoolAllocate allocate;
    VipsThreadpoolWork work;
    GMutex *allocate_lock;
    void *a;

    int nthr;
    VipsThread **thr;

    im_semaphore_t finish;

    im_semaphore_t tick;

    gboolean error;

    gboolean stop;
} VipsThreadpool;

#ifdef TIME_THREAD

#define IM_TBUF_SIZE (20000)
static GTimer *thread_timer = NULL;
#endif

#ifdef TIME_THREAD

static int vips_thread_save_time_buffers(VipsThread *thr) {
    int i;
    static int rn = 1;
    FILE *fp;
    char name[256];

    im_snprintf(name, 256, "time%d", rn++);
    printf("vips_thread_save_time_buffers: "
           "saving buffer to \"%s\"\n",
        name);
    if (!(fp = fopen(name, "w")))
        error_exit("unable to write to \"%s\"", name);
    for (i = 0; i < thr->tpos; i++)
        fprintf(fp, "%g, %g\n", thr->btime[i], thr->etime[i]);
    fclose(fp);

    return (0);
}
#endif

static void vips_thread_free(VipsThread *thr) {

    if (thr->thread) {
        thr->exit = 1;

        (void)g_thread_join(thr->thread);
        VIPS_DEBUG_MSG_RED("thread_free: g_thread_join()\n");
        thr->thread = NULL;
    }

    IM_FREEF(g_object_unref, thr->state);
    thr->pool = NULL;

#ifdef TIME_THREAD
    if (thr->btime)
        (void)vips_thread_save_time_buffers(thr);
#endif
}

static int vips_thread_allocate(VipsThread *thr) {
    VipsThreadpool *pool = thr->pool;

    g_assert(!pool->stop);

    if (!thr->state) {
        if (!(thr->state = pool->start(pool->im, pool->a))) {
            return (-1);
        }
    }

    if (pool->allocate(thr->state, pool->a, &pool->stop)) {
        return (-1);
    }

    return (0);
}

static int vips_thread_work(VipsThread *thr) {
    VipsThreadpool *pool = thr->pool;
    int result;

    result = 0;

#ifdef TIME_THREAD

    if (thr->btime && thr->tpos < IM_TBUF_SIZE)
        thr->btime[thr->tpos] = g_timer_elapsed(thread_timer, NULL);
#endif

    if (pool->work(thr->state, pool->a)) {
        result = -1;
    }

#ifdef TIME_THREAD

    if (thr->etime && thr->tpos < IM_TBUF_SIZE) {
        thr->etime[thr->tpos] = g_timer_elapsed(thread_timer, NULL);
        thr->tpos += 1;
    }
#endif

    return (result);
}

static void vips_thread_work_unit(VipsThread *thr) {
    VipsThreadpool *pool = thr->pool;

    if (thr->error) {
        return;
    }

    g_mutex_lock(pool->allocate_lock);

    if (pool->stop) {
        g_mutex_unlock(pool->allocate_lock);
        return;
    }

    if (vips_thread_allocate(thr)) {
        thr->error = TRUE;
        g_mutex_unlock(pool->allocate_lock);
        return;
    }

    if (pool->stop) {
        g_mutex_unlock(pool->allocate_lock);
        return;
    }

    g_mutex_unlock(pool->allocate_lock);

    if (vips_thread_work(thr)) {
        thr->error = TRUE;
    }
}

#ifdef HAVE_THREADS

static void *vips_thread_main_loop(void *a) {
    VipsThread *thr = (VipsThread *)a;
    VipsThreadpool *pool = thr->pool;

    g_assert(pool == thr->pool);

    for (;;) {
        vips_thread_work_unit(thr);
        im_semaphore_up(&pool->tick);

        if (pool->stop || pool->error) {
            break;
        }
    }

    im_semaphore_up(&pool->finish);

    return (NULL);
}
#endif

static VipsThread *vips_thread_new(VipsThreadpool *pool) {
    VipsThread *thr;

    if (!(thr = IM_NEW(pool->im, VipsThread))) {
        return (NULL);
    }
    thr->pool = pool;
    thr->state = NULL;
    thr->thread = NULL;
    thr->exit = 0;
    thr->error = 0;
#ifdef TIME_THREAD
    thr->btime = NULL;
    thr->etime = NULL;
    thr->tpos = 0;
#endif

#ifdef TIME_THREAD
    thr->btime = IM_ARRAY(pool->im, IM_TBUF_SIZE, double);
    thr->etime = IM_ARRAY(pool->im, IM_TBUF_SIZE, double);
    if (!thr->btime || !thr->etime) {
        thread_free(thr);
        return (NULL);
    }
#endif

#ifdef HAVE_THREADS

    if (!(thr->thread = g_thread_create_full(vips_thread_main_loop, thr, IM__DEFAULT_STACK_SIZE, TRUE, FALSE, G_THREAD_PRIORITY_NORMAL, NULL))) {
        im_error("vips_thread_new", "%s", _("unable to create thread"));
        vips_thread_free(thr);
        return (NULL);
    }

    VIPS_DEBUG_MSG_RED("vips_thread_new: g_thread_create_full()\n");
#endif

    return (thr);
}

static void vips_threadpool_kill_threads(VipsThreadpool *pool) {
    if (pool->thr) {
        int i;

        for (i = 0; i < pool->nthr; i++) {
            vips_thread_free(pool->thr[i]);
        }
        pool->thr = NULL;

        VIPS_DEBUG_MSG("vips_threadpool_kill_threads: "
                       "killed %d threads\n",
            pool->nthr);
    }
}

static int vips_threadpool_free(VipsThreadpool *pool) {
    VIPS_DEBUG_MSG("vips_threadpool_free: \"%s\" (%p)\n", pool->im->filename, pool);

    vips_threadpool_kill_threads(pool);
    IM_FREEF(g_mutex_free, pool->allocate_lock);
    im_semaphore_destroy(&pool->finish);
    im_semaphore_destroy(&pool->tick);

    return (0);
}

static VipsThreadpool *vips_threadpool_new(VipsImage *im) {
    VipsThreadpool *pool;

    if (!(pool = IM_NEW(im, VipsThreadpool))) {
        return (NULL);
    }
    pool->im = im;
    pool->allocate = NULL;
    pool->work = NULL;
    pool->allocate_lock = g_mutex_new();
    pool->nthr = im_concurrency_get();
    pool->thr = NULL;
    im_semaphore_init(&pool->finish, 0, "finish");
    im_semaphore_init(&pool->tick, 0, "tick");
    pool->stop = FALSE;
    pool->error = FALSE;

    if (im_add_close_callback(im, (im_callback_fn)vips_threadpool_free, pool, NULL)) {
        (void)vips_threadpool_free(pool);
        return (NULL);
    }

    VIPS_DEBUG_MSG("vips_threadpool_new: \"%s\" (%p), with %d threads\n", im->filename, pool, pool->nthr);

    return (pool);
}

static int vips_threadpool_create_threads(VipsThreadpool *pool) {
    int i;

    g_assert(!pool->thr);

    if (!(pool->thr = IM_ARRAY(pool->im, pool->nthr, VipsThread *))) {
        return (-1);
    }
    for (i = 0; i < pool->nthr; i++) {
        pool->thr[i] = NULL;
    }

    for (i = 0; i < pool->nthr; i++) {
        if (!(pool->thr[i] = vips_thread_new(pool))) {
            vips_threadpool_kill_threads(pool);
            return (-1);
        }
    }

    return (0);
}

int vips_threadpool_run(VipsImage *im, VipsThreadStart start, VipsThreadpoolAllocate allocate, VipsThreadpoolWork work, VipsThreadpoolProgress progress, void *a) {
    VipsThreadpool *pool;
    int result;

#ifdef TIME_THREAD
    if (!thread_timer)
        thread_timer = g_timer_new();
#endif

    if (!(pool = vips_threadpool_new(im))) {
        return (-1);
    }

    pool->start = start;
    pool->allocate = allocate;
    pool->work = work;
    pool->a = a;

    if (vips_threadpool_create_threads(pool)) {
        vips_threadpool_free(pool);
        return (-1);
    }

    for (;;) {
#ifdef HAVE_THREADS

        im_semaphore_down(&pool->tick);
#else

        vips_thread_work_unit(pool->thr[0]);
#endif

        VIPS_DEBUG_MSG("vips_threadpool_run: tick\n");

        if (pool->stop || pool->error) {
            break;
        }

        if (progress && progress(pool->a)) {
            pool->error = TRUE;
        }

        if (pool->stop || pool->error) {
            break;
        }
    }

    im_semaphore_downn(&pool->finish, pool->nthr);

    result = pool->error ? -1 : 0;

    vips_threadpool_free(pool);

    return (result);
}

void vips_get_tile_size(VipsImage *im, int *tile_width, int *tile_height, int *nlines) {
    const int nthr = im_concurrency_get();

    switch (im->dhint) {
    case IM_SMALLTILE:
        *tile_width = im__tile_width;
        *tile_height = im__tile_height;

        *nlines = *tile_height * (1 + nthr / IM_MAX(1, im->Xsize / *tile_width)) * 2;
        break;

    case IM_FATSTRIP:
        *tile_width = im->Xsize;
        *tile_height = im__fatstrip_height;
        *nlines = *tile_height * nthr * 2;
        break;

    case IM_ANY:
    case IM_THINSTRIP:
        *tile_width = im->Xsize;
        *tile_height = im__thinstrip_height;
        *nlines = *tile_height * nthr * 2;
        break;

    default:
        g_assert(0);
    }

    g_assert(*nlines % *tile_height == 0);

    VIPS_DEBUG_MSG("vips_get_tile_size: %d by %d patches, "
                   "groups of %d scanlines\n",
        *tile_width, *tile_height, *nlines);
}
