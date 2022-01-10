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
    rb_state2.var_0 = (rb_state2.var_0) ^ (1371 < rb_input_size ? (uint32_t)rb_input[1371] : 0xe1a0b7c6);
    if ((rb_state2.var_2) == (0x1a330a90)) {
        rb_state2.var_4 = 0xec991f3e;
        rb_state2.var_5 = (rb_state2.var_5) ^ (((((0xe87a619f) + (rb_state2.var_3)) ^ (rb_state2.var_5)) - (rb_state2.var_4)) ^ (0xf02629e3));
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_3) : (0xd782e345));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - ((0x5171c413) + (0x81dbf4c3));
    if ((rb_state4.var_1) == (0x4b18670e)) {
        rb_state4.var_8 = 0x4b9954d0;
        rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0x6c85b1af));
        rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_10);
        rb_state4.var_11 = (rb_state4.var_11) + (((0x175b4aa2) ^ (0xa5370c8a)) + (rb_state4.var_7));
        rb_state4.var_12 = (rb_state4.var_12) ^ (rb_state4.var_8);
        rb_state4.var_13 = (rb_state4.var_13) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_9) : (0xc6b011c0));
        rb_state4.var_14 = (rb_state4.var_14) ^ (((0x87233bc4) - (0xcd1b020)) + (rb_state4.var_10));
        rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_11);
        rb_state4.var_16 = (rb_state4.var_16) ^ (rb_state4.var_12);
        rb_state4.var_17 = (rb_state4.var_17) ^ (rb_state4.var_13);
        rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_14) : (0x4f39c34c));
        rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0xd1b25589));
        rb_state4.var_20 = (rb_state4.var_20) + (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_17) : (0xa4d1339c));
        rb_state4.var_21 = (rb_state4.var_21) + (rb_state4.var_18);
        rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_19) : (0x852f7f7c));
        rb_state4.var_23 = (rb_state4.var_23) - (((0xfd2b9509) + (rb_state4.var_14)) ^ (rb_state4.var_20));
        rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_21) : (0x16f23fa1));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_22) : (0xd5f355fe));
        rb_state4.var_26 = (rb_state4.var_26) + (((0xec1ed49c) - (rb_state4.var_17)) ^ (rb_state4.var_23));
        rb_state4.var_27 = (rb_state4.var_27) - (rb_state4.var_24);
        rb_state4.var_28 = (rb_state4.var_28) + (((((0x34f6a7c6) + (rb_state4.var_26)) - (rb_state4.var_25)) - (0xf3115cc2)) + (0x303647a3));
        rb_state4.var_29 = (rb_state4.var_29) - (rb_state4.var_27);
        rb_state4.var_30 = (rb_state4.var_30) + (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_28) : (0x15dfbf1d));
        rb_state4.var_31 = (rb_state4.var_31) ^ (((rb_state4.var_29) == (0x0)) ? (rb_state4.var_30) : (0xa58562fc));
        rb_state4.var_7 = (rb_state4.var_7) - (rb_state4.var_31);
        rb_state4.var_4 = rb_state4.var_7;
    }
    if ((rb_state4.var_1) == (0x4b18670e)) {
        pthread_mutex_lock(&(rb_state4.lock_48));
        rb_state4.var_33 = (rb_state4.var_33) + (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_22) : (0xc891b448));
        rb_state4.var_34 = (rb_state4.var_34) ^ (rb_state4.var_21);
        rb_state4.var_35 = (rb_state4.var_35) - (rb_state4.var_24);
        rb_state4.var_36 = (rb_state4.var_36) ^ (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_19) : (0x43889e03));
        rb_state4.var_37 = (rb_state4.var_37) - (((rb_state4.var_26) == (0x0)) ? (rb_state4.var_20) : (0x5381a7db));
        rb_state4.var_38 = (rb_state4.var_38) - (((((0x2bf10c33) + (rb_state4.var_34)) ^ (0x17449b1)) - (rb_state4.var_33)) + (rb_state4.var_27));
        rb_state4.var_39 = (rb_state4.var_39) - (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_35) : (0x5f3ccfc1));
        rb_state4.var_40 = (rb_state4.var_40) - (((((0xb7b2c35d) ^ (rb_state4.var_37)) - (rb_state4.var_29)) + (0x14755bcb)) ^ (rb_state4.var_36));
        rb_state4.var_41 = (rb_state4.var_41) ^ (rb_state4.var_38);
        rb_state4.var_42 = (rb_state4.var_42) ^ (((((0xd484a71f) + (rb_state4.var_39)) ^ (0x1fed7c28)) + (rb_state4.var_40)) + (rb_state4.var_30));
        rb_state4.var_43 = (rb_state4.var_43) ^ (rb_state4.var_41);
        rb_state4.var_44 = (rb_state4.var_44) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_42) : (0x5d6c607e));
        rb_state4.var_45 = (rb_state4.var_45) + (((0xfae82ef8) ^ (rb_state4.var_32)) ^ (rb_state4.var_43));
        rb_state4.var_46 = (rb_state4.var_46) ^ (rb_state4.var_44);
        rb_state4.var_47 = (rb_state4.var_47) - (((rb_state4.var_45) == (0x0)) ? (rb_state4.var_46) : (0xdbf60a7b));
        rb_state4.var_32 = (rb_state4.var_32) ^ (rb_state4.var_47);
        rb_state4.var_4 = rb_state4.var_32;
        pthread_mutex_unlock(&(rb_state4.lock_48));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - ((0x1443c543) + (rb_state8.var_0));
    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_2);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (994 < rb_input_size ? (uint32_t)rb_input[994] : 0xde300a42);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (0xcf0c3538);
    if (!((rb_state12.var_1) == (rb_state12.var_5))) {
        rb_state12.var_11 = 0x1b2f8a76;
        rb_state12.var_12 = 0xbdee56e8;
        rb_state12.var_13 = 0xba6dd413;
        rb_state12.var_14 = (rb_state12.var_14) ^ ((0x13a0a027) ^ (0x9ffa5478));
        rb_state12.var_15 = (rb_state12.var_15) - (rb_state12.var_11);
        rb_state12.var_16 = (rb_state12.var_16) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_13) : (0x8d3f955d));
        rb_state12.var_17 = (rb_state12.var_17) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_14) : (0x312b7772));
        rb_state12.var_18 = (rb_state12.var_18) ^ (rb_state12.var_15);
        rb_state12.var_19 = (rb_state12.var_19) ^ (((0xbbd7577b) ^ (rb_state12.var_16)) + (0x466c5070));
        rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_17) : (0x71c03526));
        rb_state12.var_21 = (rb_state12.var_21) - (((0x3e300c0c) + (0xbf346e31)) ^ (rb_state12.var_18));
        rb_state12.var_22 = (rb_state12.var_22) ^ (((rb_state12.var_19) == (0x0)) ? (rb_state12.var_20) : (0x210b4f19));
        rb_state12.var_23 = (rb_state12.var_23) ^ (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_22) : (0x5f4f6761));
        rb_state12.var_7 = (rb_state12.var_7) - (rb_state12.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
    rb_state15.var_1 = (rb_state15.var_1) + (0x9a47a672);
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_1);
    #endif
    Runnable *tobj = static_cast<Runnable *>(arg);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x596e0dd5));
    rb_state2.var_2 = (rb_state2.var_2) ^ (857 < rb_input_size ? (uint32_t)rb_input[857] : 0xffa47d3e);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
    rb_state3.var_1 = (rb_state3.var_1) + (1442 < rb_input_size ? (uint32_t)rb_input[1442] : 0x9ec8ba11);
    rb_state3.var_1 = (rb_state3.var_1) + (394 < rb_input_size ? (uint32_t)rb_input[394] : 0x733fca5e);
    rb_state3.var_2 = (rb_state3.var_2) - (0xd600f247);
    if ((rb_state3.var_3) == (0xcfb38b80)) {
        pthread_mutex_lock(&(rb_state3.lock_11));
        rb_state3.var_4 = rb_state3.var_0;
        pthread_mutex_unlock(&(rb_state3.lock_11));
    }
    if ((rb_state3.var_3) == (0xcfb38b80)) {
        if (!((rb_state3.var_0) == (rb_state3.var_4))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (227 < rb_input_size ? (uint32_t)rb_input[227] : 0x87f70ff6);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x41a8b143) + (0x9784c55e));
    rb_state8.var_2 = (rb_state8.var_2) + ((0x6062e7eb) + (rb_state8.var_2));
    if ((rb_state8.var_0) == (0x8b88637f)) {
        rb_state8.var_5 = 0x389076cd;
        rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0x99284ed0));
        rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_6);
        rb_state8.var_3 = rb_state8.var_4;
    }
    if ((rb_state8.var_0) == (0x8b88637f)) {
        pthread_mutex_lock(&(rb_state8.lock_13));
        rb_state8.var_8 = 0x3ede2aae;
        rb_state8.var_9 = 0x5dac58c7;
        rb_state8.var_10 = 0x81d0e83a;
        rb_state8.var_11 = (rb_state8.var_11) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x9730f673));
        rb_state8.var_12 = (rb_state8.var_12) + (((((0x7182af5c) - (rb_state8.var_6)) - (rb_state8.var_8)) ^ (rb_state8.var_11)) ^ (rb_state8.var_7));
        rb_state8.var_7 = (rb_state8.var_7) - (rb_state8.var_12);
        rb_state8.var_3 = rb_state8.var_7;
        pthread_mutex_unlock(&(rb_state8.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (1521 < rb_input_size ? (uint32_t)rb_input[1521] : 0x238a4604);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (0x506fb9d9);
    rb_state12.var_2 = (rb_state12.var_2) + (718 < rb_input_size ? (uint32_t)rb_input[718] : 0xce8b0622);
    rb_state12.var_3 = (rb_state12.var_3) + (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_3) : (0x26b81dac));
    if ((rb_state12.var_4) == (0x0)) {
        rb_state12.var_5 = rb_state12.var_1;
    }
    if ((rb_state12.var_4) == (0x0)) {
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x45fd3868);
    rb_state14.var_1 = (rb_state14.var_1) + (0x8793f519);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - (249 < rb_input_size ? (uint32_t)rb_input[249] : 0xa63a86ad);
    rb_state15.var_0 = (rb_state15.var_0) ^ (939 < rb_input_size ? (uint32_t)rb_input[939] : 0x1300e751);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0x76f76a8e));
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