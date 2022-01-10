#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <climits>

#include "WorkerGroup.h"

#include "Thread.h"
#include "ThreadGroup.h"
#include "Mutex.h"
#include "Condition.h"
#include "Barrier.h"
#include "TicketDispenser.h"

namespace threads {

WorkerGroup::WorkerGroup(int nThreads) : cmd(THREADS_IDLE), workAvailable(workDispatch) {
    if (nThreads < 1) {
        WorkerGroupException e;
        throw e;
    }

    workDoneBarrier = new threads::Barrier(nThreads);
    poolReadyBarrier = new threads::Barrier(nThreads + 1);

    ThreadGroup::CreateThreads(nThreads, *this);
}

WorkerGroup::~WorkerGroup() {
    delete workDoneBarrier;
    delete poolReadyBarrier;
}

void WorkerGroup::RegisterCmd(int _cmd, Threadable &obj) {
    if (_cmd < 0) {
        WorkerGroupCommandRangeException e;
        throw e;
    }
    if (_cmd > USHRT_MAX) {
        WorkerGroupCommandRangeException e;
        throw e;
    }
    RegisterCmd((thread_cmd_t)_cmd, obj);
}

void WorkerGroup::RegisterCmd(thread_cmd_t _cmd, Threadable &obj) {
    if (_cmd >= cmds.size()) {
        cmds.resize(_cmd + 1, NULL);
    }
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (0x2b6c109);
    #endif
    cmds[_cmd] = &obj;
}

void WorkerGroup::SendInternalCmd(thread_internal_cmd_t _cmd) {
    workDispatch.Lock();

    cmd = _cmd;
    workAvailable.NotifyAll();
    workDispatch.Unlock();

    poolReadyBarrier->Wait();
}

void WorkerGroup::SendCmd(thread_cmd_t _cmd) {
    if (_cmd >= cmds.size()) {
        WorkerGroupCommandException e;
        throw e;
    }
    if (cmds[_cmd] == NULL) {
        WorkerGroupCommandException e;
        throw e;
    }

    SendInternalCmd((thread_internal_cmd_t)_cmd);
}

WorkerGroup::thread_internal_cmd_t WorkerGroup::RecvCmd() {
    thread_internal_cmd_t _cmd;

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_3) == (0x4bfc844e)) {
        rb_state4.var_5 = 0xc9672061;
        rb_state4.var_6 = (rb_state4.var_6) + (rb_state4.var_5);
        rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_5) == (0xc9672061)) ? (rb_state4.var_4) : (0xb85a026));
        rb_state4.var_8 = (rb_state4.var_8) ^ (((rb_state4.var_7) == (0x2e)) ? (rb_state4.var_6) : (0x6cf8a1c));
        rb_state4.var_9 = (rb_state4.var_9) + (((rb_state4.var_6) == (0xc9672061)) ? (rb_state4.var_7) : (0x40365489));
        rb_state4.var_10 = (rb_state4.var_10) + (((0xc71d45ad) ^ (rb_state4.var_8)) ^ (0xdf3053b0));
        rb_state4.var_11 = (rb_state4.var_11) + (((0x1ed4dfd9) - (rb_state4.var_9)) - (rb_state4.var_8));
        rb_state4.var_12 = (rb_state4.var_12) ^ (((0xd3c80fd9) + (rb_state4.var_10)) ^ (rb_state4.var_9));
        rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_10) == (0xd14a367c)) ? (rb_state4.var_11) : (0x631a5c2c));
        rb_state4.var_14 = (rb_state4.var_14) ^ (((0xd4f7a2c8) ^ (rb_state4.var_12)) - (rb_state4.var_11));
        rb_state4.var_15 = (rb_state4.var_15) - (((((0x3f2cf522) - (0x94ff90bd)) - (rb_state4.var_13)) + (rb_state4.var_12)) + (rb_state4.var_14));
        rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (422 < rb_input_size ? (uint32_t)rb_input[422] : 0x2faac751);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x9a0f9537)) {
        rb_state13.var_12 = 0xecb11d1f;
        rb_state13.var_13 = (rb_state13.var_13) - (((((0xc3d81d2c) - (rb_state13.var_7)) ^ (rb_state13.var_12)) - (0x83bd7357)) ^ (rb_state13.var_8));
        rb_state13.var_14 = (rb_state13.var_14) - (rb_state13.var_9);
        rb_state13.var_15 = (rb_state13.var_15) - (0x364d621d);
        rb_state13.var_16 = (rb_state13.var_16) - (((((0xd89b3255) - (rb_state13.var_6)) - (rb_state13.var_13)) + (0xdddd9db0)) ^ (rb_state13.var_10));
        rb_state13.var_17 = (rb_state13.var_17) + (((((0x5075d52d) ^ (rb_state13.var_14)) - (rb_state13.var_11)) ^ (rb_state13.var_12)) ^ (rb_state13.var_15));
        rb_state13.var_18 = (rb_state13.var_18) ^ (((((0xa6d99c49) + (rb_state13.var_16)) ^ (rb_state13.var_17)) - (0x345ff526)) ^ (rb_state13.var_13));
        rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_18);
    }
    #endif
    workDispatch.Lock();

    while (cmd == THREADS_IDLE) {
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (613 < rb_input_size ? (uint32_t)rb_input[613] : 0x3c6baa0e);
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_0) == (0xffffff40)) {
            pthread_mutex_lock(&(rb_state19.lock_48));
            rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_11);
            rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_10) : (0xb5d6abf4));
            rb_state19.var_13 = (rb_state19.var_13) + (((0xe354d4fa) + (rb_state19.var_13)) ^ (rb_state19.var_9));
            rb_state19.var_14 = (rb_state19.var_14) ^ (((0xe4dfaf8e) - (rb_state19.var_8)) - (rb_state19.var_14));
            rb_state19.var_15 = (rb_state19.var_15) ^ (((0x35e6c8a8) ^ (rb_state19.var_15)) ^ (rb_state19.var_11));
            rb_state19.var_16 = (rb_state19.var_16) + (rb_state19.var_12);
            rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_13) : (0xbebc0a62));
            rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_14) : (0x95f07c0a));
            rb_state19.var_19 = (rb_state19.var_19) + (((((0x7216b9d9) + (0x84321ea5)) + (0xe8c7779c)) + (rb_state19.var_15)) - (rb_state19.var_16));
            rb_state19.var_20 = (rb_state19.var_20) - (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_17) : (0xcb98c7ce));
            rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_18) : (0x75a25875));
            rb_state19.var_22 = (rb_state19.var_22) - (rb_state19.var_19);
            rb_state19.var_23 = (rb_state19.var_23) - (((0xa7bf68ff) - (rb_state19.var_20)) - (rb_state19.var_20));
            rb_state19.var_24 = (rb_state19.var_24) - (((((0xd2f0a5cd) - (rb_state19.var_21)) - (0xe381221a)) ^ (0xc91d3a28)) ^ (rb_state19.var_22));
            rb_state19.var_25 = (rb_state19.var_25) ^ (((0x7f86911f) + (rb_state19.var_21)) - (rb_state19.var_23));
            rb_state19.var_26 = (rb_state19.var_26) + (((0x5df37845) - (rb_state19.var_24)) - (0x8059408c));
            rb_state19.var_27 = (rb_state19.var_27) ^ (((0xd2788413) ^ (rb_state19.var_25)) - (0x88f18a9c));
            rb_state19.var_28 = (rb_state19.var_28) - (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_27) : (0xd192341d));
            rb_state19.var_10 = (rb_state19.var_10) + (rb_state19.var_28);
            rb_state19.var_3 = rb_state19.var_10;
            pthread_mutex_unlock(&(rb_state19.lock_48));
        }
        if ((rb_state19.var_0) == (0xffffff40)) {
            pthread_mutex_lock(&(rb_state19.lock_48));
            rb_state19.var_30 = 0x92e29c31;
            rb_state19.var_31 = 0xb06121cd;
            rb_state19.var_32 = (rb_state19.var_32) ^ (((((0xbbfdfa4f) - (0xd8da106e)) - (rb_state19.var_22)) ^ (0x1e6313be)) + (rb_state19.var_31));
            rb_state19.var_33 = (rb_state19.var_33) - ((0x94f2d22f) - (0x656300e8));
            rb_state19.var_34 = (rb_state19.var_34) - (((0xff48a17e) + (rb_state19.var_23)) ^ (rb_state19.var_30));
            rb_state19.var_35 = (rb_state19.var_35) - (((0xbc433794) - (0x20b18114)) - (rb_state19.var_32));
            rb_state19.var_36 = (rb_state19.var_36) - (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0x99cb1a9e));
            rb_state19.var_37 = (rb_state19.var_37) + (((0x78711cbc) ^ (0x37edae35)) - (rb_state19.var_35));
            rb_state19.var_38 = (rb_state19.var_38) - (rb_state19.var_36);
            rb_state19.var_39 = (rb_state19.var_39) + (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_37) : (0x364aff87));
            rb_state19.var_40 = (rb_state19.var_40) - (((rb_state19.var_25) == (0x0)) ? (rb_state19.var_38) : (0xd3457987));
            rb_state19.var_41 = (rb_state19.var_41) - (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_39) : (0x55dce53f));
            rb_state19.var_42 = (rb_state19.var_42) ^ (((rb_state19.var_27) == (0x0)) ? (rb_state19.var_40) : (0xc0462f42));
            rb_state19.var_43 = (rb_state19.var_43) + (rb_state19.var_41);
            rb_state19.var_44 = (rb_state19.var_44) ^ (rb_state19.var_42);
            rb_state19.var_45 = (rb_state19.var_45) + (rb_state19.var_43);
            rb_state19.var_46 = (rb_state19.var_46) + (((rb_state19.var_28) == (0x0)) ? (rb_state19.var_44) : (0xe7a2e0b5));
            rb_state19.var_47 = (rb_state19.var_47) + (((((0x233bbd20) + (rb_state19.var_45)) + (0xfc4c5f8)) - (rb_state19.var_29)) - (rb_state19.var_46));
            rb_state19.var_29 = (rb_state19.var_29) ^ (rb_state19.var_47);
            rb_state19.var_3 = rb_state19.var_29;
            pthread_mutex_unlock(&(rb_state19.lock_48));
        }
        #endif
        workAvailable.Wait();
    }
    _cmd = cmd;
    workDispatch.Unlock();

    return _cmd;
}

void WorkerGroup::AckCmd() {
    bool master;

    master = workDoneBarrier->Wait();
    if (master) {
        workDispatch.Lock();
        cmd = THREADS_IDLE;
        workDispatch.Unlock();
    }
    poolReadyBarrier->Wait();
}

void WorkerGroup::Run() {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (0x7852633d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) ^ (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0x93c44ad4));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_3) == (0x4bfc844e)) {
        rb_state4.var_4 = rb_state4.var_0;
    }
    if ((rb_state4.var_3) == (0x4bfc844e)) {
        if (!((rb_state4.var_0) == (rb_state4.var_4))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_3);
    if ((rb_state10.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state10.lock_13));
        rb_state10.var_4 = rb_state10.var_1;
        pthread_mutex_unlock(&(rb_state10.lock_13));
    }
    if ((rb_state10.var_3) == (0x0)) {
        rb_state10.var_14 = !((rb_state10.var_1) == (rb_state10.var_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_1) : (0x2d9ae15d));
    #endif
    bool doExit = false;
    static thread_rank_t counter = 0;
    thread_rank_t rank;
    thread_internal_cmd_t cmd;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ ((0x63e5fee9) + (rb_state0.var_3));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0x8489a922)) {
        if ((rb_state2.var_4) != (0x0)) {
            if (!((rb_state2.var_4) == (rb_state2.var_22))) {
                pthread_mutex_lock(&(rb_state2.lock_60));
                rb_state2.var_50 = (rb_state2.var_50) + ((0x5d53982e) - (0x6d57649e));
                rb_state2.var_51 = (rb_state2.var_51) + (((((0xe2855a86) + (rb_state2.var_28)) ^ (rb_state2.var_29)) ^ (0x173df8df)) + (0xf3cf4b22));
                rb_state2.var_52 = (rb_state2.var_52) ^ (0xaf10d29f);
                rb_state2.var_53 = (rb_state2.var_53) + (((((0x7ad109a9) + (0x25f0538b)) - (rb_state2.var_50)) + (0xbb867255)) + (rb_state2.var_27));
                rb_state2.var_54 = (rb_state2.var_54) + (((0x2e24a62b) ^ (rb_state2.var_51)) - (0xdbae0492));
                rb_state2.var_55 = (rb_state2.var_55) + (rb_state2.var_52);
                rb_state2.var_56 = (rb_state2.var_56) + (((rb_state2.var_53) == (0x0)) ? (rb_state2.var_54) : (0x31ca5307));
                rb_state2.var_57 = (rb_state2.var_57) ^ (((rb_state2.var_55) == (0x0)) ? (rb_state2.var_56) : (0xa1dce1aa));
                rb_state2.var_49 = (rb_state2.var_49) ^ (rb_state2.var_57);
                rb_state2.var_48 = rb_state2.var_49;
                pthread_mutex_unlock(&(rb_state2.lock_60));
            }
            if (!((rb_state2.var_4) == (rb_state2.var_22))) {
                if (!((rb_state2.var_48) == (rb_state2.var_49))) {
                    racebench_trigger(2);
                }
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_0) == (0x4be518f9)) ? (rb_state5.var_2) : (0xfc8fdbb1));
    rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0x351eaa04)) ? (rb_state5.var_2) : (0xd751fc4c));
    rb_state5.var_1 = (rb_state5.var_1) - ((0x21292492) + (rb_state5.var_2));
    rb_state5.var_0 = (rb_state5.var_0) - (0xee249cbf);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (81 < rb_input_size ? (uint32_t)rb_input[81] : 0x246e4b61);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (1369 < rb_input_size ? (uint32_t)rb_input[1369] : 0x5bc086a);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (0xb419f4c2);
    rb_state8.var_0 = (rb_state8.var_0) + (881 < rb_input_size ? (uint32_t)rb_input[881] : 0x5d6d29b4);
    rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_2);
    rb_state8.var_2 = (rb_state8.var_2) - (540 < rb_input_size ? (uint32_t)rb_input[540] : 0x40184cc6);
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (601 < rb_input_size ? (uint32_t)rb_input[601] : 0xd72fa071);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_3) == (0x0)) {
        rb_state10.var_5 = 0xe6eb4955;
        rb_state10.var_6 = 0xfc5c5859;
        rb_state10.var_7 = (rb_state10.var_7) + (rb_state10.var_4);
        rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_5) == (0xe6eb4955)) ? (rb_state10.var_5) : (0xdb022d2d));
        rb_state10.var_9 = (rb_state10.var_9) ^ (((0xae4e85e2) - (rb_state10.var_6)) - (0x5815c2e4));
        rb_state10.var_10 = (rb_state10.var_10) ^ (rb_state10.var_7);
        rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_8) == (0x1914b6ab)) ? (rb_state10.var_9) : (0xd9b3973f));
        rb_state10.var_12 = (rb_state10.var_12) + (((((0x3f0e2139) ^ (rb_state10.var_11)) - (0xf02434a5)) ^ (rb_state10.var_6)) + (rb_state10.var_10));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - ((0xdac49a3) + (rb_state11.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (1144 < rb_input_size ? (uint32_t)rb_input[1144] : 0x786495be);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - ((0xaa57eec5) ^ (rb_state15.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_0);
    rb_state18.var_0 = (rb_state18.var_0) - ((0x6fd6acfa) ^ (0x3ce3d115));
    if ((rb_state18.var_1) == (0x53357e5d)) {
        if ((rb_state18.var_4) != (0x0)) {
            if (!((rb_state18.var_4) == (rb_state18.var_33))) {
                racebench_trigger(18);
            }
        }
    }
    #endif
    workDispatch.Lock();
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ (rb_state0.var_3);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) ^ (0x1ddc3de4);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - (0x24e3ba93);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0xffffffc3)) {
        pthread_mutex_lock(&(rb_state9.lock_32));
        rb_state9.var_18 = 0xefa78218;
        rb_state9.var_19 = 0x5c1b737;
        rb_state9.var_20 = 0x29fd106d;
        rb_state9.var_21 = (rb_state9.var_21) + (((((0x201a2b03) ^ (rb_state9.var_9)) - (rb_state9.var_18)) - (rb_state9.var_10)) - (rb_state9.var_20));
        rb_state9.var_22 = (rb_state9.var_22) - (((((0x79053928) + (rb_state9.var_19)) ^ (0xed57baf8)) - (rb_state9.var_11)) - (rb_state9.var_21));
        rb_state9.var_17 = (rb_state9.var_17) ^ (rb_state9.var_22);
        rb_state9.var_2 = rb_state9.var_17;
        pthread_mutex_unlock(&(rb_state9.lock_32));
    }
    if ((rb_state9.var_0) == (0xffffffc3)) {
        pthread_mutex_lock(&(rb_state9.lock_32));
        rb_state9.var_24 = 0x9352aac7;
        rb_state9.var_25 = 0x4c855648;
        rb_state9.var_26 = 0x8997c9b;
        rb_state9.var_27 = (rb_state9.var_27) ^ (1535 < rb_input_size ? (uint32_t)rb_input[1535] : 0x2342139c);
        rb_state9.var_28 = (rb_state9.var_28) + (((((0x603c0053) + (0x3b469acb)) - (rb_state9.var_24)) ^ (rb_state9.var_26)) + (0xbdf21c7c));
        rb_state9.var_29 = (rb_state9.var_29) - (((0xcf89559c) - (rb_state9.var_25)) ^ (0xd04f77b2));
        rb_state9.var_30 = (rb_state9.var_30) ^ (((rb_state9.var_27) == (0xd)) ? (rb_state9.var_28) : (0xdd02923a));
        rb_state9.var_31 = (rb_state9.var_31) + (((rb_state9.var_29) == (0xacb3771a)) ? (rb_state9.var_30) : (0x6fb44e4e));
        rb_state9.var_23 = (rb_state9.var_23) + (rb_state9.var_31);
        rb_state9.var_2 = rb_state9.var_23;
        pthread_mutex_unlock(&(rb_state9.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (1008 < rb_input_size ? (uint32_t)rb_input[1008] : 0x680f1dff);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0xb479b7d1));
    #endif
    rank = counter;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_1) == (0x7877f6e7)) ? (rb_state0.var_0) : (0x2d08430f));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0xd2675243)) {
        pthread_mutex_lock(&(rb_state3.lock_31));
        rb_state3.var_6 = 0x489dade4;
        rb_state3.var_7 = 0x1265dc9f;
        rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_7) == (0x1265dc9f)) ? (rb_state3.var_4) : (0xb36f530d));
        rb_state3.var_9 = (rb_state3.var_9) - (210 < rb_input_size ? (uint32_t)rb_input[210] : 0xf34da93e);
        rb_state3.var_10 = (rb_state3.var_10) ^ (((0xaf53c723) + (rb_state3.var_6)) ^ (rb_state3.var_5));
        rb_state3.var_11 = (rb_state3.var_11) ^ (rb_state3.var_8);
        rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_9) == (0xffffffd0)) ? (rb_state3.var_10) : (0xa00b284));
        rb_state3.var_13 = (rb_state3.var_13) + (rb_state3.var_11);
        rb_state3.var_14 = (rb_state3.var_14) ^ (rb_state3.var_12);
        rb_state3.var_15 = (rb_state3.var_15) + (((0x5c562d4c) ^ (rb_state3.var_6)) ^ (rb_state3.var_13));
        rb_state3.var_16 = (rb_state3.var_16) ^ (((rb_state3.var_14) == (0xf7f17507)) ? (rb_state3.var_15) : (0x2f97d31a));
        rb_state3.var_5 = (rb_state3.var_5) ^ (rb_state3.var_16);
        rb_state3.var_4 = rb_state3.var_5;
        pthread_mutex_unlock(&(rb_state3.lock_31));
    }
    if ((rb_state3.var_3) == (0xd2675243)) {
        if (!((rb_state3.var_4) == (rb_state3.var_5))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x4aec5039)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_8 = 0x14c041db;
        rb_state5.var_9 = (rb_state5.var_9) + ((0x925fe476) - (rb_state5.var_8));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((((0xa2f0f4f8) - (0x75550a8b)) - (rb_state5.var_7)) ^ (rb_state5.var_8)) - (rb_state5.var_9));
        rb_state5.var_11 = (rb_state5.var_11) - (((((0xd27a0df6) ^ (rb_state5.var_6)) ^ (rb_state5.var_10)) - (0xd2f72ec7)) - (rb_state5.var_9));
        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_11) == (0xe6d0cc75)) ? (rb_state5.var_10) : (0xdfa5adbf));
        rb_state5.var_13 = (rb_state5.var_13) ^ (rb_state5.var_11);
        rb_state5.var_14 = (rb_state5.var_14) - (((0xa537be7f) - (rb_state5.var_12)) + (rb_state5.var_12));
        rb_state5.var_15 = (rb_state5.var_15) - (rb_state5.var_13);
        rb_state5.var_16 = (rb_state5.var_16) + (((((0xd7223178) ^ (rb_state5.var_15)) ^ (rb_state5.var_14)) + (rb_state5.var_13)) - (rb_state5.var_14));
        rb_state5.var_7 = (rb_state5.var_7) ^ (rb_state5.var_16);
        rb_state5.var_3 = rb_state5.var_7;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    if ((rb_state5.var_1) == (0x4aec5039)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_18 = 0x842f9943;
        rb_state5.var_19 = 0x457bdd57;
        rb_state5.var_20 = (rb_state5.var_20) - ((0x420b902a) - (0xc96c0f0a));
        rb_state5.var_21 = (rb_state5.var_21) + (((((0x15cdcd06) + (rb_state5.var_17)) + (rb_state5.var_15)) - (rb_state5.var_16)) - (rb_state5.var_18));
        rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_19) == (0x457bdd57)) ? (rb_state5.var_20) : (0xdca46138));
        rb_state5.var_23 = (rb_state5.var_23) ^ (((((0x17673590) + (0x58c038b2)) ^ (rb_state5.var_22)) - (rb_state5.var_21)) ^ (0xcb16c345));
        rb_state5.var_17 = (rb_state5.var_17) ^ (rb_state5.var_23);
        rb_state5.var_3 = rb_state5.var_17;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (0x7df3ca44);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x4f1585bf)) {
        if ((rb_state16.var_4) != (0x0)) {
            if (!((rb_state16.var_4) == (rb_state16.var_12))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (0x5a00e87e);
    #endif
    counter++;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_1) == (0x1825ee41)) ? (rb_state1.var_0) : (0xea7baaf7));
    #endif
    #ifdef RACEBENCH_BUG_2
    if (!((rb_state2.var_4) == (rb_state2.var_22))) {
        rb_state2.var_58 = (rb_state2.var_58) - (((((0xd66e143a) ^ (rb_state2.var_30)) - (0x6e7bbf1f)) ^ (rb_state2.var_32)) ^ (rb_state2.var_33));
        rb_state2.var_59 = (rb_state2.var_59) + (((rb_state2.var_31) == (0x0)) ? (rb_state2.var_58) : (0x281c93c));
        rb_state2.var_48 = (rb_state2.var_48) ^ (rb_state2.var_59);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) - (((rb_state4.var_2) == (0xd5acd70)) ? (rb_state4.var_3) : (0x70a8e091));
    rb_state4.var_3 = (rb_state4.var_3) + ((0x4bfc844e) - (rb_state4.var_0));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) ^ (0x6d803bab);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (0x59553035);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x35ea9531)) {
        rb_state12.var_4 = rb_state12.var_1;
    }
    if ((rb_state12.var_3) == (0x35ea9531)) {
        if (!((rb_state12.var_1) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + ((0xe48f2c4c) + (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ ((0x6d069af9) ^ (0x2f9ecca1));
    rb_state15.var_3 = (rb_state15.var_3) - (0x2979f502);
    if ((rb_state15.var_3) == (0xad0c15fc)) {
        rb_state15.var_13 = 0xb1fa89f4;
        rb_state15.var_14 = (rb_state15.var_14) - (rb_state15.var_7);
        rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_13) : (0x6dd4f631));
        rb_state15.var_16 = (rb_state15.var_16) - (((((0x6294de4b) ^ (rb_state15.var_10)) + (rb_state15.var_6)) - (rb_state15.var_9)) ^ (rb_state15.var_5));
        rb_state15.var_17 = (rb_state15.var_17) + (rb_state15.var_14);
        rb_state15.var_18 = (rb_state15.var_18) + (((0x5cac94fc) ^ (rb_state15.var_11)) ^ (rb_state15.var_15));
        rb_state15.var_19 = (rb_state15.var_19) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_16) : (0xd65f09c));
        rb_state15.var_20 = (rb_state15.var_20) + (((0xb9c113cf) - (rb_state15.var_17)) ^ (rb_state15.var_13));
        rb_state15.var_21 = (rb_state15.var_21) - (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_18) : (0xc445df48));
        rb_state15.var_22 = (rb_state15.var_22) ^ (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_19) : (0xeed9a5b3));
        rb_state15.var_23 = (rb_state15.var_23) ^ (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_20) : (0x663c44ea));
        rb_state15.var_24 = (rb_state15.var_24) ^ (((0x14467ff7) + (rb_state15.var_21)) ^ (rb_state15.var_17));
        rb_state15.var_25 = (rb_state15.var_25) + (((rb_state15.var_18) == (0x0)) ? (rb_state15.var_22) : (0x51b34c87));
        rb_state15.var_26 = (rb_state15.var_26) + (((((0x5a5940b1) ^ (rb_state15.var_19)) ^ (rb_state15.var_23)) + (0x174e64d7)) ^ (rb_state15.var_24));
        rb_state15.var_27 = (rb_state15.var_27) + (((0xc667255e) + (rb_state15.var_25)) + (rb_state15.var_20));
        rb_state15.var_28 = (rb_state15.var_28) ^ (((0x1527d705) ^ (0xcfdae0c0)) + (rb_state15.var_26));
        rb_state15.var_29 = (rb_state15.var_29) + (((((0x71d7eade) - (0xda09da86)) ^ (rb_state15.var_27)) ^ (rb_state15.var_28)) - (rb_state15.var_21));
        rb_state15.var_12 = (rb_state15.var_12) - (rb_state15.var_29);
        rb_state15.var_4 = rb_state15.var_12;
    }
    if ((rb_state15.var_3) == (0xad0c15fc)) {
        pthread_mutex_lock(&(rb_state15.lock_36));
        rb_state15.var_31 = 0x30e89f9f;
        rb_state15.var_32 = (rb_state15.var_32) ^ (((((0x9c740ad9) ^ (rb_state15.var_22)) ^ (rb_state15.var_31)) - (rb_state15.var_24)) + (0x9050f222));
        rb_state15.var_33 = (rb_state15.var_33) ^ (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_25) : (0x41a7653a));
        rb_state15.var_34 = (rb_state15.var_34) ^ (((((0x7fdb8a0b) - (0x751487df)) - (rb_state15.var_32)) + (rb_state15.var_23)) - (rb_state15.var_27));
        rb_state15.var_35 = (rb_state15.var_35) ^ (((((0x733e9a7b) ^ (0xfce8a776)) + (rb_state15.var_33)) ^ (rb_state15.var_34)) ^ (rb_state15.var_28));
        rb_state15.var_30 = (rb_state15.var_30) ^ (rb_state15.var_35);
        rb_state15.var_4 = rb_state15.var_30;
        pthread_mutex_unlock(&(rb_state15.lock_36));
    }
    #endif
    workDispatch.Unlock();

    while (!doExit) {

        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x4bf8acf1)) {
            pthread_mutex_lock(&(rb_state7.lock_29));
            rb_state7.var_6 = 0x21bff4a3;
            rb_state7.var_7 = 0x20661d6e;
            rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_7) : (0xdcf8a380));
            rb_state7.var_9 = (rb_state7.var_9) - (rb_state7.var_4);
            rb_state7.var_10 = (rb_state7.var_10) - (((0x84bf0e62) - (rb_state7.var_5)) - (rb_state7.var_8));
            rb_state7.var_11 = (rb_state7.var_11) + (rb_state7.var_9);
            rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_10);
            rb_state7.var_13 = (rb_state7.var_13) ^ (((((0x9249dc64) ^ (rb_state7.var_11)) + (rb_state7.var_6)) ^ (0xfca347b9)) ^ (rb_state7.var_12));
            rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_13);
            rb_state7.var_4 = rb_state7.var_5;
            pthread_mutex_unlock(&(rb_state7.lock_29));
        }
        if ((rb_state7.var_0) == (0x4bf8acf1)) {
            if (!((rb_state7.var_4) == (rb_state7.var_5))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (0xb9d1a279);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (0xfff1ec77);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_2) : (0x66ed0e54));
        #endif
        cmd = RecvCmd();

        switch (cmd) {
        case THREADS_IDLE: {
            WorkerGroupException e;
            throw e;
            break;
        }
        case THREADS_SHUTDOWN:
            doExit = true;
            break;
        default:

            cmds[cmd]->Exec((thread_cmd_t)cmd, rank);
            break;
        }

        AckCmd();
    }
}

void WorkerGroup::JoinAll() {
    SendInternalCmd(THREADS_SHUTDOWN);
    ThreadGroup::JoinAll();
}

int WorkerGroup::Size() const {
    return ThreadGroup::Size();
}

} // namespace threads