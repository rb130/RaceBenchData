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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
    if ((rb_state1.var_0) == (0x0)) {
        rb_state1.var_31 = !((rb_state1.var_2) == (rb_state1.var_1));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xa)) {
        pthread_mutex_lock(&(rb_state3.lock_37));
        rb_state3.var_13 = 0x17d60bbd;
        rb_state3.var_14 = (rb_state3.var_14) - (rb_state3.var_9);
        rb_state3.var_15 = (rb_state3.var_15) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_7) : (0x2ba7db89));
        rb_state3.var_16 = (rb_state3.var_16) - (((0x7ee9920e) + (rb_state3.var_10)) - (rb_state3.var_13));
        rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_14);
        rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_16) : (0x6c127dec));
        rb_state3.var_19 = (rb_state3.var_19) + (rb_state3.var_17);
        rb_state3.var_20 = (rb_state3.var_20) ^ (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_18) : (0x9c2f692c));
        rb_state3.var_21 = (rb_state3.var_21) - (((((0x53e7fdd5) + (0x1e6cb598)) + (rb_state3.var_12)) ^ (rb_state3.var_19)) + (rb_state3.var_20));
        rb_state3.var_12 = (rb_state3.var_12) ^ (rb_state3.var_21);
        rb_state3.var_1 = rb_state3.var_12;
        pthread_mutex_unlock(&(rb_state3.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + ((0x1286557a) + (rb_state12.var_0));
    #endif
    Runnable *tobj = static_cast<Runnable *>(arg);
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xa)) {
        pthread_mutex_lock(&(rb_state3.lock_37));
        rb_state3.var_23 = 0xd0cee25f;
        rb_state3.var_24 = 0xc67c781f;
        rb_state3.var_25 = (rb_state3.var_25) ^ (rb_state3.var_14);
        rb_state3.var_26 = (rb_state3.var_26) - (rb_state3.var_23);
        rb_state3.var_27 = (rb_state3.var_27) - (rb_state3.var_24);
        rb_state3.var_28 = (rb_state3.var_28) - (((((0x32503f47) ^ (rb_state3.var_25)) + (rb_state3.var_13)) + (0xc7609f8)) - (rb_state3.var_15));
        rb_state3.var_29 = (rb_state3.var_29) - (((0xbe98b2ea) ^ (rb_state3.var_26)) - (rb_state3.var_16));
        rb_state3.var_30 = (rb_state3.var_30) ^ (((((0x9d998289) - (rb_state3.var_28)) + (0xf2155d10)) + (0x3e4b064b)) + (rb_state3.var_27));
        rb_state3.var_31 = (rb_state3.var_31) - (((0x839308c1) ^ (rb_state3.var_29)) ^ (0xfe891154));
        rb_state3.var_32 = (rb_state3.var_32) ^ (((0x49e739a6) + (rb_state3.var_17)) ^ (rb_state3.var_30));
        rb_state3.var_33 = (rb_state3.var_33) ^ (((rb_state3.var_18) == (0x0)) ? (rb_state3.var_31) : (0x6e31c4be));
        rb_state3.var_34 = (rb_state3.var_34) ^ (((rb_state3.var_19) == (0x0)) ? (rb_state3.var_32) : (0x771d3f69));
        rb_state3.var_35 = (rb_state3.var_35) + (((rb_state3.var_20) == (0x0)) ? (rb_state3.var_33) : (0xe354b5e7));
        rb_state3.var_36 = (rb_state3.var_36) + (((rb_state3.var_34) == (0x0)) ? (rb_state3.var_35) : (0xc13098c));
        rb_state3.var_22 = (rb_state3.var_22) + (rb_state3.var_36);
        rb_state3.var_1 = rb_state3.var_22;
        pthread_mutex_unlock(&(rb_state3.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        rb_state4.var_3 = (rb_state4.var_3) + ((0xcd388a5d) ^ (0x884b016));
        rb_state4.var_4 = (rb_state4.var_4) - (rb_state4.var_3);
        rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_4);
        rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0x116f8e40));
        rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_3) : (0x8cd945ce));
        rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_4) : (0x53387d10));
        rb_state4.var_9 = (rb_state4.var_9) ^ (((0x8e0ce967) + (rb_state4.var_7)) ^ (rb_state4.var_5));
        rb_state4.var_10 = (rb_state4.var_10) + (((0x4981d0e1) - (rb_state4.var_8)) - (rb_state4.var_6));
        rb_state4.var_11 = (rb_state4.var_11) - (((((0xeb4ec028) ^ (0xd6c60a4)) + (rb_state4.var_8)) + (rb_state4.var_7)) - (rb_state4.var_9));
        rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0x8b085621));
        rb_state4.var_13 = (rb_state4.var_13) ^ (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0x7e3e95db));
        rb_state4.var_14 = (rb_state4.var_14) - (rb_state4.var_12);
        rb_state4.var_15 = (rb_state4.var_15) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_14) : (0xa12a0176));
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x1c8969c7));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (25 < rb_input_size ? (uint32_t)rb_input[25] : 0x4a5fe89c);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x34e72c0b)) {
        pthread_mutex_lock(&(rb_state11.lock_37));
        rb_state11.var_10 = 0xe57c16e1;
        rb_state11.var_11 = 0xddde9f;
        rb_state11.var_12 = (rb_state11.var_12) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_11) : (0x92071af5));
        rb_state11.var_13 = (rb_state11.var_13) + (952 < rb_input_size ? (uint32_t)rb_input[952] : 0xc6fd0594);
        rb_state11.var_14 = (rb_state11.var_14) ^ (((0x8ebc660a) - (rb_state11.var_10)) - (0x1bf26ce9));
        rb_state11.var_15 = (rb_state11.var_15) ^ (((0xe99b721a) + (rb_state11.var_3)) ^ (rb_state11.var_5));
        rb_state11.var_16 = (rb_state11.var_16) - (((((0x23e0befa) - (rb_state11.var_12)) + (0x24b7ca03)) + (rb_state11.var_13)) + (0x98622f0b));
        rb_state11.var_17 = (rb_state11.var_17) - (rb_state11.var_14);
        rb_state11.var_18 = (rb_state11.var_18) - (((0x45898cde) - (0x6c657781)) - (rb_state11.var_15));
        rb_state11.var_19 = (rb_state11.var_19) ^ (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_17) : (0x604165be));
        rb_state11.var_20 = (rb_state11.var_20) ^ (((0x1198545e) ^ (rb_state11.var_18)) + (rb_state11.var_6));
        rb_state11.var_21 = (rb_state11.var_21) - (((((0x8b7b5c24) - (0x983f3047)) - (rb_state11.var_19)) - (0x558c35f2)) + (rb_state11.var_20));
        rb_state11.var_9 = (rb_state11.var_9) ^ (rb_state11.var_21);
        rb_state11.var_1 = rb_state11.var_9;
        pthread_mutex_unlock(&(rb_state11.lock_37));
    }
    if ((rb_state11.var_0) == (0x246805cd)) {
        pthread_mutex_lock(&(rb_state11.lock_37));
        rb_state11.var_23 = 0x47d45f72;
        rb_state11.var_24 = (rb_state11.var_24) ^ (1473 < rb_input_size ? (uint32_t)rb_input[1473] : 0xd1ce0135);
        rb_state11.var_25 = (rb_state11.var_25) ^ (((0xdb99c30b) - (rb_state11.var_23)) - (rb_state11.var_9));
        rb_state11.var_26 = (rb_state11.var_26) ^ (0xfba2a4cc);
        rb_state11.var_27 = (rb_state11.var_27) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_7) : (0xe4f3dce));
        rb_state11.var_28 = (rb_state11.var_28) + (rb_state11.var_24);
        rb_state11.var_29 = (rb_state11.var_29) ^ (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_26) : (0xbb42f35));
        rb_state11.var_30 = (rb_state11.var_30) ^ (((0x7669a139) ^ (rb_state11.var_10)) - (rb_state11.var_27));
        rb_state11.var_31 = (rb_state11.var_31) ^ (((((0xa085cb94) - (rb_state11.var_11)) - (0xbf65e415)) - (rb_state11.var_29)) + (rb_state11.var_28));
        rb_state11.var_32 = (rb_state11.var_32) ^ (rb_state11.var_30);
        rb_state11.var_33 = (rb_state11.var_33) ^ (rb_state11.var_31);
        rb_state11.var_34 = (rb_state11.var_34) + (((0x4cd0d863) - (rb_state11.var_32)) ^ (rb_state11.var_12));
        rb_state11.var_35 = (rb_state11.var_35) + (((0x762eb4f6) - (rb_state11.var_13)) ^ (rb_state11.var_33));
        rb_state11.var_36 = (rb_state11.var_36) - (((((0x93a420a7) ^ (0x663ccf88)) - (rb_state11.var_35)) + (rb_state11.var_34)) + (0x80da2ef0));
        rb_state11.var_22 = (rb_state11.var_22) - (rb_state11.var_36);
        rb_state11.var_1 = rb_state11.var_22;
        pthread_mutex_unlock(&(rb_state11.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xcf1e58ea)) {
        pthread_mutex_lock(&(rb_state12.lock_45));
        rb_state12.var_3 = 0x3c373652;
        rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_3);
        rb_state12.var_5 = (rb_state12.var_5) + (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_3) : (0xbe3c3cc3));
        rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_4);
        rb_state12.var_7 = (rb_state12.var_7) - (((((0x4e951a45) + (rb_state12.var_5)) + (rb_state12.var_2)) ^ (rb_state12.var_4)) - (0x28482ec2));
        rb_state12.var_8 = (rb_state12.var_8) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_5) : (0xdb295a81));
        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_6);
        rb_state12.var_10 = (rb_state12.var_10) + (((0x1f2853dd) ^ (rb_state12.var_7)) - (0x62c79a28));
        rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_8);
        rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_9) : (0xa02f3e37));
        rb_state12.var_13 = (rb_state12.var_13) - (((0x607ad520) ^ (rb_state12.var_8)) + (rb_state12.var_10));
        rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_11);
        rb_state12.var_15 = (rb_state12.var_15) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_12) : (0x45d75c2a));
        rb_state12.var_16 = (rb_state12.var_16) ^ (rb_state12.var_13);
        rb_state12.var_17 = (rb_state12.var_17) ^ (((0xccc19ab0) - (rb_state12.var_10)) + (rb_state12.var_14));
        rb_state12.var_18 = (rb_state12.var_18) + (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_15) : (0x56827164));
        rb_state12.var_19 = (rb_state12.var_19) ^ (((((0x4e958319) ^ (rb_state12.var_16)) - (rb_state12.var_12)) ^ (rb_state12.var_17)) ^ (rb_state12.var_13));
        rb_state12.var_20 = (rb_state12.var_20) - (((0xba509112) ^ (0x53b95d9d)) - (rb_state12.var_18));
        rb_state12.var_21 = (rb_state12.var_21) - (rb_state12.var_19);
        rb_state12.var_22 = (rb_state12.var_22) ^ (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_21) : (0x95fab794));
        rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_22);
        rb_state12.var_1 = rb_state12.var_2;
        pthread_mutex_unlock(&(rb_state12.lock_45));
    }
    if ((rb_state12.var_0) == (0xcf1e58ea)) {
        if (!((rb_state12.var_1) == (rb_state12.var_2))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x5bcf61e)) {
        usleep(100);
    }
    if ((rb_state14.var_0) == (0x5bcf61e)) {
        rb_state14.var_9 = !((rb_state14.var_2) == (rb_state14.var_1));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - ((0xc94a4e6a) + (rb_state15.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (0xa02373a1);
    if ((rb_state18.var_0) == (0xa02373a1)) {
        pthread_mutex_lock(&(rb_state18.lock_15));
        rb_state18.var_3 = 0x9d8b3b83;
        rb_state18.var_4 = (rb_state18.var_4) ^ (0x1ce5f6a4);
        rb_state18.var_5 = (rb_state18.var_5) - (((((0x4fc76c5b) - (rb_state18.var_3)) - (0x9198157a)) ^ (rb_state18.var_2)) + (rb_state18.var_3));
        rb_state18.var_6 = (rb_state18.var_6) ^ (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_1) : (0x6e4bd4cb));
        rb_state18.var_7 = (rb_state18.var_7) + (((0x47ac0a1d) - (0x62b5cc55)) ^ (rb_state18.var_4));
        rb_state18.var_8 = (rb_state18.var_8) - (((0xafe695ec) - (rb_state18.var_5)) ^ (rb_state18.var_5));
        rb_state18.var_9 = (rb_state18.var_9) - (((((0xf4fde3ac) - (0x57cdf46b)) ^ (rb_state18.var_7)) + (rb_state18.var_6)) ^ (rb_state18.var_6));
        rb_state18.var_10 = (rb_state18.var_10) - (rb_state18.var_8);
        rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0xfca65da));
        rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_11);
        rb_state18.var_1 = rb_state18.var_2;
        pthread_mutex_unlock(&(rb_state18.lock_15));
    }
    if ((rb_state18.var_0) == (0xa02373a1)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_0);
    if ((rb_state19.var_0) == (0x0)) {
        if ((rb_state19.var_1) != (0x0)) {
            if (!((rb_state19.var_1) == (rb_state19.var_16))) {
                racebench_trigger(19);
            }
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