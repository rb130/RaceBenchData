#ifdef __cplusplus
extern "C"
#endif
void racebench_bug2(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug2(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug3(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug3(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug21(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug43(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug43(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(HAVE_LIBPTHREAD)
#include <pthread.h>
#else
#include <windows.h>
#include <process.h>
#endif

#include <typeinfo>
#include "Thread.h"

namespace threads {

#if defined(HAVE_LIBPTHREAD)

extern "C" {
static void *thread_entry(void *arg) {
    Runnable *tobj = static_cast<Runnable *>(arg); racebench_bug2(0); racebench_bug2(1); racebench_bug3(0); racebench_bug3(1); racebench_bug6(1); racebench_bug43(0); racebench_bug43(1); racebench_bug54(0); racebench_bug54(1); racebench_bug80(0); racebench_bug80(1);
    tobj->Run(); racebench_bug21(0); racebench_bug52(0); racebench_bug52(1);

    return NULL;
}
}
#else
extern "C" {
unsigned __stdcall thread_entry(void *arg) {
    Runnable *tobj = static_cast<Runnable *>(arg);
    tobj->Run();
    return NULL;
}
}
#endif

Thread::Thread(Runnable &_tobj) throw(ThreadCreationException) : tobj(_tobj) {
#if defined(HAVE_LIBPTHREAD)
    if (pthread_create(&t, NULL, &thread_entry, (void *)&tobj)) {
        ThreadCreationException e;
        throw e;
    }
#else
    t = (void *)_beginthreadex(NULL, 0, &thread_entry, (void *)&tobj, 0, &t_id);
    if (!t) {
        ThreadCreationException e;
        throw e;
    }
#endif
}

void Thread::Join() {
    Stoppable *_tobj;
    bool isStoppable = true;

    try {
        _tobj = &dynamic_cast<Stoppable &>(tobj);
    } catch (std::bad_cast e) {
        isStoppable = false;
    }
    if (isStoppable) {
        _tobj->Stop();
    }

#if defined(HAVE_LIBPTHREAD)
    pthread_join(t, NULL);
#else
    WaitForSingleObject(t, INFINITE);
    CloseHandle(t);
#endif
}

} // namespace threads