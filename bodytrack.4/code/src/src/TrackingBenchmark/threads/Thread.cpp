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
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x9db12eea));
    rb_state0.var_1 = (rb_state0.var_1) - ((0x8788091f) ^ (rb_state0.var_1));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + ((0xa0671dfc) - (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x720ab6ce) - (0x4dfc65ed));
    rb_state2.var_1 = (rb_state2.var_1) + (0x18b02826);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (0x41a31897);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0x1e583b0f) - (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (1523 < rb_input_size ? (uint32_t)rb_input[1523] : 0x8424fbfa);
    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_0) == (0x35)) ? (rb_state6.var_2) : (0xce7e10df));
    #endif
    #ifdef RACEBENCH_BUG_8
    if (!((rb_state8.var_35) == (rb_state8.var_51))) {
        pthread_mutex_lock(&(rb_state8.lock_93));
        rb_state8.var_62 = 0x6f154785;
        rb_state8.var_63 = (rb_state8.var_63) + (rb_state8.var_33);
        rb_state8.var_64 = (rb_state8.var_64) + (((0xb31c46ca) - (rb_state8.var_62)) ^ (0x5e7935a3));
        rb_state8.var_65 = (rb_state8.var_65) + ((0xca8a2ca7) + (rb_state8.var_36));
        rb_state8.var_66 = (rb_state8.var_66) - (((rb_state8.var_37) == (0x0)) ? (rb_state8.var_35) : (0x8a29c8b9));
        rb_state8.var_67 = (rb_state8.var_67) - (((rb_state8.var_38) == (0x0)) ? (rb_state8.var_63) : (0xfcbc38ac));
        rb_state8.var_68 = (rb_state8.var_68) ^ (((0x780d5f) + (0x6f2f3d47)) ^ (rb_state8.var_64));
        rb_state8.var_69 = (rb_state8.var_69) ^ (rb_state8.var_65);
        rb_state8.var_70 = (rb_state8.var_70) + (((0x3795ee45) + (rb_state8.var_66)) ^ (rb_state8.var_39));
        rb_state8.var_71 = (rb_state8.var_71) - (((rb_state8.var_67) == (0x0)) ? (rb_state8.var_68) : (0x8fd251f1));
        rb_state8.var_72 = (rb_state8.var_72) - (((0xd766335d) + (rb_state8.var_69)) + (rb_state8.var_40));
        rb_state8.var_73 = (rb_state8.var_73) ^ (((rb_state8.var_70) == (0x0)) ? (rb_state8.var_71) : (0xaa7cd59a));
        rb_state8.var_74 = (rb_state8.var_74) ^ (((rb_state8.var_41) == (0x0)) ? (rb_state8.var_72) : (0x8a45fe03));
        rb_state8.var_75 = (rb_state8.var_75) + (((((0xc479881a) + (0x4c158921)) + (rb_state8.var_74)) - (rb_state8.var_42)) - (rb_state8.var_73));
        rb_state8.var_61 = (rb_state8.var_61) + (rb_state8.var_75);
        rb_state8.var_60 = rb_state8.var_61;
        pthread_mutex_unlock(&(rb_state8.lock_93));
    }
    if (!((rb_state8.var_35) == (rb_state8.var_51))) {
        if (!((rb_state8.var_60) == (rb_state8.var_61))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (1203 < rb_input_size ? (uint32_t)rb_input[1203] : 0x10879c8a);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xac441c0b) - (rb_state10.var_1));
    rb_state10.var_0 = (rb_state10.var_0) + (693 < rb_input_size ? (uint32_t)rb_input[693] : 0x86c18ecf);
    rb_state10.var_2 = (rb_state10.var_2) - (1071 < rb_input_size ? (uint32_t)rb_input[1071] : 0xc57eca87);
    rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_2);
    rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_0) == (0x3cf93b78)) ? (rb_state10.var_2) : (0x1bcfbe84));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + (412 < rb_input_size ? (uint32_t)rb_input[412] : 0x85b83441);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0xe212798f));
    #endif
    Runnable *tobj = static_cast<Runnable *>(arg);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_1) == (0xb3ec3cdc)) ? (rb_state3.var_0) : (0x4613aa29));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (887 < rb_input_size ? (uint32_t)rb_input[887] : 0xe62c02a8);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (513 < rb_input_size ? (uint32_t)rb_input[513] : 0x82845821);
    rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0xc5a29319));
    rb_state5.var_0 = (rb_state5.var_0) + (0x16c66ef5);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_3);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (0xc3fd8efb);
    rb_state7.var_1 = (rb_state7.var_1) ^ (740 < rb_input_size ? (uint32_t)rb_input[740] : 0xba66156d);
    rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (1053 < rb_input_size ? (uint32_t)rb_input[1053] : 0x4bd7d7c1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ ((0x79bd93b4) + (0x2f0a3d25));
    rb_state12.var_2 = (rb_state12.var_2) ^ (853 < rb_input_size ? (uint32_t)rb_input[853] : 0x9989643);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_1) : (0x6d01a0ea));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (24 < rb_input_size ? (uint32_t)rb_input[24] : 0x52e7962f);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_2) : (0x2ce8f8ed));
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