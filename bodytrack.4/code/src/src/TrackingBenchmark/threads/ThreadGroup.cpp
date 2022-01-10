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
        rb_state0.var_2 = (rb_state0.var_2) - (0xd8fcb407);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + (915 < rb_input_size ? (uint32_t)rb_input[915] : 0x4953c344);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ ((0xb3ec3d13) + (rb_state3.var_0));
        if ((rb_state3.var_3) == (0xd2675243)) {
            rb_state3.var_17 = 0xc9d81fe9;
            rb_state3.var_18 = 0x3e5cf1b5;
            rb_state3.var_19 = (rb_state3.var_19) - (rb_state3.var_7);
            rb_state3.var_20 = (rb_state3.var_20) - (rb_state3.var_8);
            rb_state3.var_21 = (rb_state3.var_21) ^ (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_18) : (0x943886bc));
            rb_state3.var_22 = (rb_state3.var_22) + (rb_state3.var_17);
            rb_state3.var_23 = (rb_state3.var_23) + (((((0x7fc66912) - (rb_state3.var_19)) + (rb_state3.var_20)) + (0x3ded13f4)) ^ (rb_state3.var_10));
            rb_state3.var_24 = (rb_state3.var_24) ^ (((0xb290c4ec) ^ (rb_state3.var_11)) ^ (rb_state3.var_21));
            rb_state3.var_25 = (rb_state3.var_25) ^ (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_23) : (0x73057d4d));
            rb_state3.var_26 = (rb_state3.var_26) - (rb_state3.var_24);
            rb_state3.var_27 = (rb_state3.var_27) ^ (((0xdf96ea2a) + (rb_state3.var_25)) - (rb_state3.var_12));
            rb_state3.var_28 = (rb_state3.var_28) ^ (((0x557842a1) + (rb_state3.var_26)) ^ (0x5b7a2f15));
            rb_state3.var_29 = (rb_state3.var_29) ^ (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_27) : (0x625e555d));
            rb_state3.var_30 = (rb_state3.var_30) - (((((0x97f48977) ^ (rb_state3.var_29)) - (rb_state3.var_15)) ^ (rb_state3.var_28)) + (rb_state3.var_14));
            rb_state3.var_4 = (rb_state3.var_4) - (rb_state3.var_30);
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + ((0x53b695b0) + (0x3f559586));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (0xe6a7f5bf);
        #endif
        temp = new Thread(tobj);
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (0xbaf6b6f8);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (347 < rb_input_size ? (uint32_t)rb_input[347] : 0x9bbce1b7);
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_0) == (0xffffffc9)) ? (rb_state3.var_3) : (0x131078a));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_2);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (906 < rb_input_size ? (uint32_t)rb_input[906] : 0x57f6aef8);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (176 < rb_input_size ? (uint32_t)rb_input[176] : 0x40b1eb5a);
        rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_0);
        if ((rb_state8.var_2) == (0xffffffd0)) {
            rb_state8.var_5 = 0xbb790f9b;
            rb_state8.var_6 = (rb_state8.var_6) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_6) : (0xa0a5f0ce));
            rb_state8.var_7 = (rb_state8.var_7) ^ (707 < rb_input_size ? (uint32_t)rb_input[707] : 0x2147fa88);
            rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0xf2ab12fa));
            rb_state8.var_9 = (rb_state8.var_9) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_6) : (0x3c5ddb32));
            rb_state8.var_10 = (rb_state8.var_10) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0xa493c8a0));
            rb_state8.var_11 = (rb_state8.var_11) + (((((0x171556ae) ^ (0xd8f7dea4)) + (rb_state8.var_10)) + (rb_state8.var_9)) - (0x49b3aab7));
            rb_state8.var_4 = (rb_state8.var_4) ^ (rb_state8.var_11);
            rb_state8.var_3 = rb_state8.var_4;
        }
        if ((rb_state8.var_2) == (0xffffffd0)) {
            pthread_mutex_lock(&(rb_state8.lock_34));
            rb_state8.var_13 = 0xf33877f0;
            rb_state8.var_14 = (rb_state8.var_14) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_9) : (0xe4acf15f));
            rb_state8.var_15 = (rb_state8.var_15) + (rb_state8.var_11);
            rb_state8.var_16 = (rb_state8.var_16) ^ ((0xc8624e28) + (0x69f5ef6b));
            rb_state8.var_17 = (rb_state8.var_17) ^ (rb_state8.var_8);
            rb_state8.var_18 = (rb_state8.var_18) - (((0x33d6542f) + (rb_state8.var_13)) ^ (0x4823ff3c));
            rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_14) : (0xdab348ce));
            rb_state8.var_20 = (rb_state8.var_20) + (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_15) : (0x57747a9));
            rb_state8.var_21 = (rb_state8.var_21) ^ (((0x3abeab15) + (rb_state8.var_16)) + (rb_state8.var_14));
            rb_state8.var_22 = (rb_state8.var_22) - (rb_state8.var_17);
            rb_state8.var_23 = (rb_state8.var_23) - (((0xd83ad072) ^ (rb_state8.var_18)) - (0xc9a99f61));
            rb_state8.var_24 = (rb_state8.var_24) - (rb_state8.var_19);
            rb_state8.var_25 = (rb_state8.var_25) + (((((0xaa421af8) + (rb_state8.var_15)) - (0x807ea74c)) - (rb_state8.var_21)) + (rb_state8.var_20));
            rb_state8.var_26 = (rb_state8.var_26) + (((0x92f45142) - (0x9e0df643)) + (rb_state8.var_22));
            rb_state8.var_27 = (rb_state8.var_27) + (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_23) : (0xf0df628a));
            rb_state8.var_28 = (rb_state8.var_28) ^ (((0x10ee3162) + (rb_state8.var_17)) ^ (rb_state8.var_24));
            rb_state8.var_29 = (rb_state8.var_29) + (rb_state8.var_25);
            rb_state8.var_30 = (rb_state8.var_30) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0xef5e1f62));
            rb_state8.var_31 = (rb_state8.var_31) - (rb_state8.var_28);
            rb_state8.var_32 = (rb_state8.var_32) + (((((0x8aa3a768) + (rb_state8.var_19)) ^ (rb_state8.var_30)) ^ (rb_state8.var_18)) - (rb_state8.var_29));
            rb_state8.var_33 = (rb_state8.var_33) + (((((0x4b2cd053) + (rb_state8.var_20)) ^ (rb_state8.var_32)) + (rb_state8.var_31)) + (0x4cc6935e));
            rb_state8.var_12 = (rb_state8.var_12) ^ (rb_state8.var_33);
            rb_state8.var_3 = rb_state8.var_12;
            pthread_mutex_unlock(&(rb_state8.lock_34));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x39)) ? (rb_state9.var_0) : (0xcfb7390a));
        rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0xfffffffe)) ? (rb_state9.var_1) : (0x39e0c237));
        rb_state9.var_1 = (rb_state9.var_1) + (1108 < rb_input_size ? (uint32_t)rb_input[1108] : 0xc02ce30a);
        if ((rb_state9.var_0) == (0xffffffc3)) {
            if ((rb_state9.var_2) != (0x0)) {
                if (!((rb_state9.var_2) == (rb_state9.var_23))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
        rb_state10.var_1 = (rb_state10.var_1) - ((0x72388180) - (rb_state10.var_1));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x424ad302));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0xa8c7d0d9)) ? (rb_state12.var_0) : (0x894d3233));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (0xa123d42e);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
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