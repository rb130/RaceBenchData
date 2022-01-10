#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <list>

#include "Thread.h"
#include "ThreadGroup.h"

namespace threads {

ThreadGroup::ThreadGroup() {
}

ThreadGroup::~ThreadGroup() {
    for (std::list<Thread *>::iterator it = threads.begin(); it != threads.end(); it++) {
        delete (*it);
    }
}

void ThreadGroup::CreateThread(Runnable &tobj) {
    Thread *temp = new Thread(tobj);

    threads.push_front(temp);
}

void ThreadGroup::CreateThreads(int n, Runnable &tobj) {
    int i;
    Thread *temp;

    for (i = 0; i < n; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_1);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_2) : (0xebe5bb08));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x15d5db77));
        rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (199 < rb_input_size ? (uint32_t)rb_input[199] : 0xa76ec4b6);
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_1);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + ((0xe523ba54) - (0x6b61b3aa));
        if ((rb_state13.var_1) == (0x8989b479)) {
            pthread_mutex_lock(&(rb_state13.lock_32));
            rb_state13.var_4 = 0xabaa6ded;
            rb_state13.var_5 = (rb_state13.var_5) ^ (0x86ccf9dc);
            rb_state13.var_6 = (rb_state13.var_6) - (0x8a0a0d1f);
            rb_state13.var_7 = (rb_state13.var_7) - (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_4) : (0xde0d78c));
            rb_state13.var_8 = (rb_state13.var_8) - (rb_state13.var_3);
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_5) == (0x86ccf9dc)) ? (rb_state13.var_6) : (0x77aaa8b7));
            rb_state13.var_10 = (rb_state13.var_10) + (((rb_state13.var_7) == (0x54559213)) ? (rb_state13.var_8) : (0x327551ba));
            rb_state13.var_11 = (rb_state13.var_11) + (((rb_state13.var_4) == (0xabaa6ded)) ? (rb_state13.var_9) : (0x1b0ef935));
            rb_state13.var_12 = (rb_state13.var_12) + (((((0x84527b28) - (rb_state13.var_11)) + (0x1814f54)) ^ (rb_state13.var_10)) + (0xc9907c8f));
            rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_12);
            rb_state13.var_2 = rb_state13.var_3;
            pthread_mutex_unlock(&(rb_state13.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (632 < rb_input_size ? (uint32_t)rb_input[632] : 0x99d0807);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (0xf8d1e3b1);
        #endif
        temp = new Thread(tobj);
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ ((0x71fdf4dc) + (rb_state1.var_1));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - (0x6622b7cf);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ ((0x6039e5c6) ^ (rb_state5.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
        rb_state6.var_1 = (rb_state6.var_1) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_1) : (0x13ac3485));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - ((0xa7ef86a8) ^ (0xce074a7a));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (0xbd7a5b08);
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x4285a4f8)) ? (rb_state11.var_0) : (0x7a7e8c28));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_0);
        rb_state13.var_1 = (rb_state13.var_1) - (0x76764b87);
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_1);
        if ((rb_state13.var_1) == (0x33b9ac1)) {
            if (!((rb_state13.var_2) == (rb_state13.var_3))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ ((0xc47fd323) ^ (0x20c99e03));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (0xba876ef8);
        if ((rb_state18.var_1) == (0x185f8047)) {
            if ((rb_state18.var_2) != (0x0)) {
                if (!((rb_state18.var_2) == (rb_state18.var_18))) {
                    racebench_trigger(18);
                }
            }
        }
        #endif
        threads.push_front(temp);
    }
}

void ThreadGroup::AddThread(Thread *t) {
    threads.push_front(t);
}

void ThreadGroup::RemoveThread(Thread *t) {
    threads.remove(t);
}

void ThreadGroup::JoinAll() {
    for (std::list<Thread *>::iterator it = threads.begin(); it != threads.end(); it++) {
        (*it)->Join();
    }
}

int ThreadGroup::Size() const {
    return (int)threads.size();
}

} // namespace threads