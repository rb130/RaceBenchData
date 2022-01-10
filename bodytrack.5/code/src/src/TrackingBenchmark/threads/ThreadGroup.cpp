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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x7f018c6f));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_0);
        rb_state2.var_1 = (rb_state2.var_1) - (0xb0a4d5cb);
        rb_state2.var_0 = (rb_state2.var_0) ^ (751 < rb_input_size ? (uint32_t)rb_input[751] : 0xf5730500);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_3) : (0x52a6e919));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) - (0x7475c273);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_0) == (0xd92d766f)) ? (rb_state8.var_1) : (0x79ee6e41));
        rb_state8.var_1 = (rb_state8.var_1) ^ (0x6b00f942);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_3) == (0x38)) {
            pthread_mutex_lock(&(rb_state11.lock_23));
            rb_state11.var_7 = 0x72b68182;
            rb_state11.var_8 = 0xf179ccbe;
            rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_5) : (0xfd102374));
            rb_state11.var_10 = (rb_state11.var_10) + (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0xeaccc893));
            rb_state11.var_11 = (rb_state11.var_11) - (((0x8ba9bec1) - (rb_state11.var_7)) + (rb_state11.var_8));
            rb_state11.var_12 = (rb_state11.var_12) - (((((0x170d567b) + (rb_state11.var_9)) ^ (rb_state11.var_10)) ^ (rb_state11.var_9)) ^ (0x1e12fad8));
            rb_state11.var_13 = (rb_state11.var_13) - (rb_state11.var_11);
            rb_state11.var_14 = (rb_state11.var_14) - (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_12) : (0xabcf2620));
            rb_state11.var_15 = (rb_state11.var_15) + (rb_state11.var_13);
            rb_state11.var_16 = (rb_state11.var_16) ^ (((0x6250d284) + (rb_state11.var_14)) + (rb_state11.var_11));
            rb_state11.var_17 = (rb_state11.var_17) ^ (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x499e26ca));
            rb_state11.var_6 = (rb_state11.var_6) ^ (rb_state11.var_17);
            rb_state11.var_5 = rb_state11.var_6;
            pthread_mutex_unlock(&(rb_state11.lock_23));
        }
        if ((rb_state11.var_3) == (0x38)) {
            if (!((rb_state11.var_5) == (rb_state11.var_6))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (0x8ee74c1a);
        rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_2) == (0x2d)) ? (rb_state12.var_1) : (0xda042cc4));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (0xa365c966);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_4) == (0x90cb8542)) {
            rb_state19.var_17 = 0xc3b0f3a3;
            rb_state19.var_18 = 0x1e3cbdc4;
            rb_state19.var_19 = (rb_state19.var_19) - (((0x5c1cf982) - (0x10106027)) + (rb_state19.var_18));
            rb_state19.var_20 = (rb_state19.var_20) + (((((0x47ee66b8) - (0xb536f95a)) + (0x9fe765f0)) + (rb_state19.var_17)) ^ (rb_state19.var_9));
            rb_state19.var_21 = (rb_state19.var_21) + (rb_state19.var_19);
            rb_state19.var_22 = (rb_state19.var_22) + (((((0x615fe53) ^ (rb_state19.var_20)) + (rb_state19.var_10)) ^ (rb_state19.var_11)) ^ (rb_state19.var_21));
            rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_22);
            rb_state19.var_5 = rb_state19.var_16;
        }
        if ((rb_state19.var_4) == (0x90cb8542)) {
            pthread_mutex_lock(&(rb_state19.lock_27));
            rb_state19.var_24 = (rb_state19.var_24) + (((((0x2b1ee3c7) + (rb_state19.var_12)) - (0x72a7f0d1)) + (rb_state19.var_15)) - (rb_state19.var_14));
            rb_state19.var_25 = (rb_state19.var_25) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_13) : (0x95467f57));
            rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_25) : (0xfa7fea1e));
            rb_state19.var_23 = (rb_state19.var_23) + (rb_state19.var_26);
            rb_state19.var_5 = rb_state19.var_23;
            pthread_mutex_unlock(&(rb_state19.lock_27));
        }
        #endif
        temp = new Thread(tobj);
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_2) == (0x1a330a90)) {
            pthread_mutex_lock(&(rb_state2.lock_6));
            rb_state2.var_3 = rb_state2.var_1;
            pthread_mutex_unlock(&(rb_state2.lock_6));
        }
        if ((rb_state2.var_2) == (0x1a330a90)) {
            if (!((rb_state2.var_1) == (rb_state2.var_3))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_3 = (rb_state3.var_3) ^ (0xcfb38b80);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x75db13f4));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (333 < rb_input_size ? (uint32_t)rb_input[333] : 0x5f24529d);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - ((0xe540d2b8) ^ (rb_state9.var_0));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_4);
        rb_state11.var_4 = (rb_state11.var_4) + (0x26caba0d);
        rb_state11.var_1 = (rb_state11.var_1) ^ (0x5a73b0e5);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_4 = (rb_state12.var_4) + (rb_state12.var_4);
        if ((rb_state12.var_4) == (0x0)) {
            rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_6) : (0x58ed326d));
            rb_state12.var_7 = (rb_state12.var_7) ^ (((((0xc21eeb71) ^ (0xb72a0de9)) ^ (rb_state12.var_5)) - (rb_state12.var_2)) - (0x560959c));
            rb_state12.var_8 = (rb_state12.var_8) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0x9ead1fa7));
            rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) ^ (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_2) : (0xc5f7793a));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (0x25a34f71);
        rb_state19.var_2 = (rb_state19.var_2) - (240 < rb_input_size ? (uint32_t)rb_input[240] : 0x35c98194);
        rb_state19.var_2 = (rb_state19.var_2) - (0x1010ba88);
        rb_state19.var_4 = (rb_state19.var_4) ^ (0x58230f98);
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
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x31b)) {
        rb_state0.var_12 = 0x795432ff;
        rb_state0.var_13 = 0xdef77b37;
        rb_state0.var_14 = (rb_state0.var_14) - (((((0xfbb233c) - (rb_state0.var_13)) + (rb_state0.var_10)) ^ (rb_state0.var_12)) - (rb_state0.var_11));
        rb_state0.var_15 = (rb_state0.var_15) + (((0x6bbbe60e) ^ (rb_state0.var_12)) - (rb_state0.var_13));
        rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_14);
        rb_state0.var_17 = (rb_state0.var_17) ^ (rb_state0.var_15);
        rb_state0.var_18 = (rb_state0.var_18) ^ (((((0x9a35ad15) ^ (rb_state0.var_14)) - (0x4a0c6c7c)) ^ (rb_state0.var_17)) - (rb_state0.var_16));
        rb_state0.var_4 = (rb_state0.var_4) - (rb_state0.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_1);
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_3);
    #endif
    return (int)threads.size();
}

} // namespace threads