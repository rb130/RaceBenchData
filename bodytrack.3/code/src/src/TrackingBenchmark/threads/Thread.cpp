#include "racebench_bugs.h"


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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (1391 < rb_input_size ? (uint32_t)rb_input[1391] : 0x47ab5646);
    rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_2) : (0x7ab30d63));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (1006 < rb_input_size ? (uint32_t)rb_input[1006] : 0x1616a673);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (0x6dd3d696);
    rb_state5.var_0 = (rb_state5.var_0) ^ (1403 < rb_input_size ? (uint32_t)rb_input[1403] : 0x780c49ec);
    rb_state5.var_0 = (rb_state5.var_0) - (885 < rb_input_size ? (uint32_t)rb_input[885] : 0xc9ff36c1);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0x0)) {
        rb_state6.var_3 = rb_state6.var_2;
    }
    if ((rb_state6.var_1) == (0x0)) {
        if (!((rb_state6.var_2) == (rb_state6.var_3))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) - (0xec22a05d);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0xffffffd3)) ? (rb_state14.var_0) : (0xda698a15));
    rb_state14.var_1 = (rb_state14.var_1) + (522 < rb_input_size ? (uint32_t)rb_input[522] : 0x751ea486);
    rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (0x19f6a5d2);
    #endif
    Runnable *tobj = static_cast<Runnable *>(arg);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x3512ccda));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (0x285b8add);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0xc28a9296));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x75b94001));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + (1023 < rb_input_size ? (uint32_t)rb_input[1023] : 0x5ae5c9fe);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        rb_state11.var_2 = rb_state11.var_0;
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    if ((rb_state11.var_1) == (0x0)) {
        if (!((rb_state11.var_0) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_2) == (0xa5)) ? (rb_state14.var_1) : (0x38a08a07));
    rb_state14.var_1 = (rb_state14.var_1) ^ ((0x3d6f759b) ^ (0xb2702895));
    if ((rb_state14.var_2) == (0x38a08b1a)) {
        pthread_mutex_lock(&(rb_state14.lock_15));
        rb_state14.var_5 = 0x6dae9ae3;
        rb_state14.var_6 = 0xb5a88f01;
        rb_state14.var_7 = (rb_state14.var_7) - (((((0x1835ec31) + (rb_state14.var_3)) ^ (0x5b0040a6)) ^ (rb_state14.var_5)) - (rb_state14.var_4));
        rb_state14.var_8 = (rb_state14.var_8) ^ (rb_state14.var_6);
        rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0xbcddf1d0));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_9);
        rb_state14.var_3 = rb_state14.var_4;
        pthread_mutex_unlock(&(rb_state14.lock_15));
    }
    if ((rb_state14.var_2) == (0x38a08b1a)) {
        rb_state14.var_16 = !((rb_state14.var_3) == (rb_state14.var_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x197d268a)) {
        rb_state16.var_5 = 0x84fb1446;
        rb_state16.var_6 = 0xe1039b8d;
        rb_state16.var_7 = (rb_state16.var_7) ^ (((0xc9b1fd3e) ^ (rb_state16.var_5)) ^ (0xccc5a661));
        rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_6) : (0x342900f));
        rb_state16.var_9 = (rb_state16.var_9) - (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_7) : (0x403d1223));
        rb_state16.var_10 = (rb_state16.var_10) + (((0x9e0f1c46) - (rb_state16.var_8)) ^ (0x77a003d1));
        rb_state16.var_11 = (rb_state16.var_11) + (((((0x905e5e5c) - (rb_state16.var_6)) - (rb_state16.var_10)) ^ (rb_state16.var_7)) + (rb_state16.var_9));
        rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_11);
    }
    #endif
    tobj->Run();

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