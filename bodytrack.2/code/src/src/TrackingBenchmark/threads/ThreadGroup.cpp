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
        rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
        if ((rb_state8.var_0) == (0x0)) {
            rb_state8.var_16 = 0xd8e8435b;
            rb_state8.var_17 = (rb_state8.var_17) - (rb_state8.var_8);
            rb_state8.var_18 = (rb_state8.var_18) - (((((0xdba7e436) ^ (rb_state8.var_7)) ^ (rb_state8.var_9)) - (rb_state8.var_16)) ^ (rb_state8.var_10));
            rb_state8.var_19 = (rb_state8.var_19) + (((0x6013914a) - (rb_state8.var_17)) ^ (0x6f66a84b));
            rb_state8.var_20 = (rb_state8.var_20) - (((0xb4699b35) - (0x7249f92)) - (rb_state8.var_18));
            rb_state8.var_21 = (rb_state8.var_21) ^ (rb_state8.var_19);
            rb_state8.var_22 = (rb_state8.var_22) - (rb_state8.var_20);
            rb_state8.var_23 = (rb_state8.var_23) - (((rb_state8.var_21) == (0xf753901)) ? (rb_state8.var_22) : (0xe08eada2));
            rb_state8.var_15 = (rb_state8.var_15) + (rb_state8.var_23);
            rb_state8.var_1 = rb_state8.var_15;
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ ((0xbd6fa4c0) ^ (rb_state18.var_0));
        #endif
        temp = new Thread(tobj);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (1532 < rb_input_size ? (uint32_t)rb_input[1532] : 0xfb5ec030);
        if ((rb_state0.var_0) == (0x0)) {
            rb_state0.var_10 = (rb_state0.var_10) ^ ((0xe78b288f) + (0x58a37cfa));
            rb_state0.var_11 = (rb_state0.var_11) ^ (((0x9a4bed7c) + (rb_state0.var_3)) + (rb_state0.var_6));
            rb_state0.var_12 = (rb_state0.var_12) ^ (rb_state0.var_5);
            rb_state0.var_13 = (rb_state0.var_13) - (((rb_state0.var_8) == (0xa4bcab7d)) ? (rb_state0.var_7) : (0x4279667f));
            rb_state0.var_14 = (rb_state0.var_14) - (((((0xaa08f22c) - (0x222a2ec6)) ^ (rb_state0.var_9)) + (rb_state0.var_4)) ^ (rb_state0.var_10));
            rb_state0.var_15 = (rb_state0.var_15) + (((0x58cbca57) - (rb_state0.var_11)) - (rb_state0.var_10));
            rb_state0.var_16 = (rb_state0.var_16) - (rb_state0.var_12);
            rb_state0.var_17 = (rb_state0.var_17) + (((0x1691b154) + (rb_state0.var_11)) + (rb_state0.var_13));
            rb_state0.var_18 = (rb_state0.var_18) ^ (((0xb8884749) ^ (rb_state0.var_14)) - (0xb6ba6541));
            rb_state0.var_19 = (rb_state0.var_19) ^ (((rb_state0.var_15) == (0xb61e67c5)) ? (rb_state0.var_16) : (0x9a142fca));
            rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_17) == (0x79106e67)) ? (rb_state0.var_18) : (0x5dc3842c));
            rb_state0.var_21 = (rb_state0.var_21) - (((((0x6332ed1b) - (rb_state0.var_12)) - (rb_state0.var_13)) - (rb_state0.var_20)) ^ (rb_state0.var_19));
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (1183 < rb_input_size ? (uint32_t)rb_input[1183] : 0xa8af1adc);
        if ((rb_state8.var_0) == (0x34)) {
            pthread_mutex_lock(&(rb_state8.lock_38));
            rb_state8.var_25 = 0xe6f47ce5;
            rb_state8.var_26 = 0x64f7d475;
            rb_state8.var_27 = 0x4179551;
            rb_state8.var_28 = (rb_state8.var_28) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_26) : (0xdb217cff));
            rb_state8.var_29 = (rb_state8.var_29) ^ (((0xab65cdce) - (rb_state8.var_12)) ^ (rb_state8.var_27));
            rb_state8.var_30 = (rb_state8.var_30) - (rb_state8.var_25);
            rb_state8.var_31 = (rb_state8.var_31) - (((0xa6a5fd9) - (rb_state8.var_28)) ^ (0xfe178188));
            rb_state8.var_32 = (rb_state8.var_32) - (((0x283180aa) - (0xfeddd604)) - (rb_state8.var_29));
            rb_state8.var_33 = (rb_state8.var_33) ^ (((rb_state8.var_30) == (0x0)) ? (rb_state8.var_31) : (0xa673307b));
            rb_state8.var_34 = (rb_state8.var_34) + (rb_state8.var_32);
            rb_state8.var_35 = (rb_state8.var_35) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_33) : (0x304b547a));
            rb_state8.var_36 = (rb_state8.var_36) + (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_34) : (0x59e00d9f));
            rb_state8.var_37 = (rb_state8.var_37) ^ (((rb_state8.var_35) == (0x0)) ? (rb_state8.var_36) : (0x61969c86));
            rb_state8.var_24 = (rb_state8.var_24) - (rb_state8.var_37);
            rb_state8.var_1 = rb_state8.var_24;
            pthread_mutex_unlock(&(rb_state8.lock_38));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_0);
        if ((rb_state9.var_0) == (0xffffff6d)) {
            if ((rb_state9.var_4) != (0x0)) {
                rb_state9.var_23 = !((rb_state9.var_2) == (rb_state9.var_3));
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (0x147110);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (0x270b53fa);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x34)) ? (rb_state12.var_0) : (0x715921));
        if ((rb_state12.var_1) == (0xffffffcc)) {
            pthread_mutex_lock(&(rb_state12.lock_16));
            rb_state12.var_4 = 0x8551065c;
            rb_state12.var_5 = (rb_state12.var_5) + (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_2) : (0x45d50ddc));
            rb_state12.var_6 = (rb_state12.var_6) + (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_4) : (0xdcd4c791));
            rb_state12.var_7 = (rb_state12.var_7) - (((((0x5f25d4eb) + (rb_state12.var_6)) + (0xdb49418c)) - (rb_state12.var_5)) + (rb_state12.var_5));
            rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_7);
            rb_state12.var_2 = rb_state12.var_3;
            pthread_mutex_unlock(&(rb_state12.lock_16));
        }
        if ((rb_state12.var_1) == (0xffffffcc)) {
            if (!((rb_state12.var_2) == (rb_state12.var_3))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
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