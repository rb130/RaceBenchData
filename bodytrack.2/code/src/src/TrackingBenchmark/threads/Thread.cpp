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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xea0ba8d0) - (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (1114 < rb_input_size ? (uint32_t)rb_input[1114] : 0xf0bf8952);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (0x3e62329);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (487 < rb_input_size ? (uint32_t)rb_input[487] : 0x48a6ffe0);
    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0xeb1d98c2));
    if ((rb_state10.var_0) == (0xffd71e20)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0xffd71e20)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (1502 < rb_input_size ? (uint32_t)rb_input[1502] : 0xa06d5371);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (0x3903c91d);
    #endif
    Runnable *tobj = static_cast<Runnable *>(arg);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (0x64ac670d);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (1069 < rb_input_size ? (uint32_t)rb_input[1069] : 0x68d75135);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x0)) {
        if ((rb_state7.var_2) != (0x0)) {
            if (!((rb_state7.var_2) == (rb_state7.var_21))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (12 < rb_input_size ? (uint32_t)rb_input[12] : 0xb10b9390);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (1504 < rb_input_size ? (uint32_t)rb_input[1504] : 0xbd53f050);
    if ((rb_state10.var_0) == (0xffd71e20)) {
        rb_state10.var_3 = 0x27b4e527;
        rb_state10.var_4 = (rb_state10.var_4) ^ ((0x9e5393f9) - (0x7040d5ff));
        rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_4) == (0x2e12bdfa)) ? (rb_state10.var_2) : (0x1b53ebe0));
        rb_state10.var_6 = (rb_state10.var_6) + (0xd3a13cf6);
        rb_state10.var_7 = (rb_state10.var_7) ^ (((0xa5846468) ^ (rb_state10.var_3)) + (0x1d5d9a6e));
        rb_state10.var_8 = (rb_state10.var_8) ^ (rb_state10.var_4);
        rb_state10.var_9 = (rb_state10.var_9) ^ (((rb_state10.var_5) == (0xeb0927f0)) ? (rb_state10.var_6) : (0x69aafe34));
        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_7) == (0x9f8e1bbd)) ? (rb_state10.var_8) : (0x6f81663e));
        rb_state10.var_11 = (rb_state10.var_11) + (((0xf5511da5) - (rb_state10.var_9)) + (rb_state10.var_5));
        rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_10);
        rb_state10.var_13 = (rb_state10.var_13) - (((0xf9b9f7fd) ^ (0xc7608ba5)) - (rb_state10.var_11));
        rb_state10.var_14 = (rb_state10.var_14) ^ (((0x9c25ba41) ^ (rb_state10.var_12)) ^ (rb_state10.var_6));
        rb_state10.var_15 = (rb_state10.var_15) ^ (rb_state10.var_13);
        rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_7) == (0x9f8e1bbd)) ? (rb_state10.var_14) : (0x3b10ff2));
        rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_8) == (0x2e12bdfa)) ? (rb_state10.var_15) : (0x9a3bcad9));
        rb_state10.var_18 = (rb_state10.var_18) + (((0x78913812) - (0x83f6e700)) ^ (rb_state10.var_16));
        rb_state10.var_19 = (rb_state10.var_19) - (((rb_state10.var_9) == (0xd3a13cf6)) ? (rb_state10.var_17) : (0x471e3476));
        rb_state10.var_20 = (rb_state10.var_20) - (((0x2141ff4f) + (rb_state10.var_18)) ^ (rb_state10.var_10));
        rb_state10.var_21 = (rb_state10.var_21) + (((((0x9371a9ce) ^ (rb_state10.var_11)) ^ (rb_state10.var_20)) ^ (0xced0ced3)) + (rb_state10.var_19));
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (0x80231274);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (1098 < rb_input_size ? (uint32_t)rb_input[1098] : 0xf20e2b46);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x53713cac));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ ((0x39440a9c) - (0xe25ea834));
    if ((rb_state18.var_1) == (0x56e56268)) {
        if ((rb_state18.var_4) != (0x0)) {
            rb_state18.var_13 = !((rb_state18.var_2) == (rb_state18.var_3));
        }
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