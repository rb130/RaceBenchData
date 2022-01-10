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
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (0x197d2628);
    #endif
    RegisterCmd((thread_cmd_t)_cmd, obj);
}

void WorkerGroup::RegisterCmd(thread_cmd_t _cmd, Threadable &obj) {
    if (_cmd >= cmds.size()) {
        cmds.resize(_cmd + 1, NULL);
    }
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

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_2) == (0xcd97d893)) {
        if ((rb_state3.var_5) != (0x0)) {
            rb_state3.var_16 = !((rb_state3.var_3) == (rb_state3.var_4));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_7 = (rb_state14.var_7) - (((rb_state14.var_5) == (0x6dae9ae3)) ? (rb_state14.var_4) : (0x875178bf));
    #endif
    workDispatch.Lock();

    while (cmd == THREADS_IDLE) {
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ ((0x52b3a110) ^ (0xd5d6ef4b));
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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x4382884a));
    rb_state2.var_1 = (rb_state2.var_1) + ((0xe1fae4ca) - (rb_state2.var_1));
    rb_state2.var_1 = (rb_state2.var_1) ^ (908 < rb_input_size ? (uint32_t)rb_input[908] : 0x367629df);
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x68045dc2);
    if ((rb_state2.var_2) == (0x854cf2b0)) {
        rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_3);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0xcc3d3477);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_0) : (0xf5fba0c5));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0x76dc2d7b)) {
        rb_state5.var_22 = 0x2cad610b;
        rb_state5.var_23 = 0xdc99f599;
        rb_state5.var_24 = 0x4b29fdb9;
        rb_state5.var_25 = (rb_state5.var_25) - (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_23) : (0xf5e60a8c));
        rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_22) : (0xebc69431));
        rb_state5.var_27 = (rb_state5.var_27) + (((rb_state5.var_25) == (0x0)) ? (rb_state5.var_26) : (0x720d46a1));
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_27);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ ((0x922c38df) - (0xb572d9a4));
    rb_state6.var_0 = (rb_state6.var_0) - (0x508e29aa);
    if ((rb_state6.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_6));
        rb_state6.var_4 = 0xb32e9e50;
        rb_state6.var_5 = (rb_state6.var_5) ^ (((((0xe7c4ae16) + (rb_state6.var_4)) ^ (rb_state6.var_3)) ^ (0x341c045f)) ^ (rb_state6.var_4));
        rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_5);
        pthread_mutex_unlock(&(rb_state6.lock_6));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) + (0x490d16ae);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ ((0x1534fe34) - (0xd4862170));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (1460 < rb_input_size ? (uint32_t)rb_input[1460] : 0xc42444ab);
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0x83584bdc));
    if ((rb_state11.var_1) == (0x0)) {
        rb_state11.var_3 = 0x3ee8be8a;
        rb_state11.var_4 = (rb_state11.var_4) ^ (((rb_state11.var_2) == (0x38fb28e0)) ? (rb_state11.var_3) : (0xbf8ebe09));
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_4);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (1380 < rb_input_size ? (uint32_t)rb_input[1380] : 0x61b8b4ec);
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_1) : (0xa2ba903f));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_1) : (0xc9e2e005));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x38a08b1a)) {
        rb_state14.var_10 = 0xaaa2416f;
        rb_state14.var_11 = (rb_state14.var_11) + (((0xcc490d35) - (rb_state14.var_7)) ^ (rb_state14.var_6));
        rb_state14.var_12 = (rb_state14.var_12) + (((((0x4b36701b) ^ (rb_state14.var_8)) - (rb_state14.var_5)) + (0xffd23127)) - (rb_state14.var_10));
        rb_state14.var_13 = (rb_state14.var_13) + (((0xd349850e) + (rb_state14.var_11)) ^ (0x76a6f771));
        rb_state14.var_14 = (rb_state14.var_14) + (((((0x41ac21d4) ^ (rb_state14.var_10)) + (rb_state14.var_13)) - (rb_state14.var_9)) ^ (rb_state14.var_12));
        rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (262 < rb_input_size ? (uint32_t)rb_input[262] : 0xe07ddeec);
    rb_state15.var_1 = (rb_state15.var_1) + ((0xfcd37a29) ^ (0x78cf2b18));
    rb_state15.var_2 = (rb_state15.var_2) ^ ((0x32f0c45f) - (rb_state15.var_0));
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x750eddf0)) ? (rb_state18.var_0) : (0x6c64a47f));
    rb_state18.var_1 = (rb_state18.var_1) - ((0xa03f9816) + (0x4760e7d6));
    #endif
    bool doExit = false;
    static thread_rank_t counter = 0;
    thread_rank_t rank;
    thread_internal_cmd_t cmd;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ ((0x24501d66) ^ (0x7577fdfd));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x987acb05));
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_2);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
    rb_state4.var_0 = (rb_state4.var_0) - (1038 < rb_input_size ? (uint32_t)rb_input[1038] : 0xc7d3d5a7);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) + (1096 < rb_input_size ? (uint32_t)rb_input[1096] : 0x40405e8d);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x677859ef));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
    rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_2);
    rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0x3f55bfe1));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_4 = 0x62ad257;
        rb_state15.var_5 = 0x16fbee93;
        rb_state15.var_6 = (rb_state15.var_6) + (rb_state15.var_4);
        rb_state15.var_7 = (rb_state15.var_7) ^ (((0x12191379) - (rb_state15.var_5)) + (rb_state15.var_4));
        rb_state15.var_8 = (rb_state15.var_8) ^ (((0x291e2246) - (0xb7e53490)) - (rb_state15.var_3));
        rb_state15.var_9 = (rb_state15.var_9) - (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_6) : (0xd2d3072a));
        rb_state15.var_10 = (rb_state15.var_10) + (((0x30306f50) - (rb_state15.var_7)) + (0xc4aa74bd));
        rb_state15.var_11 = (rb_state15.var_11) - (((0x5ffcae59) - (rb_state15.var_8)) ^ (rb_state15.var_6));
        rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_9) : (0xa5f16375));
        rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_10) : (0xc88a9661));
        rb_state15.var_14 = (rb_state15.var_14) + (rb_state15.var_11);
        rb_state15.var_15 = (rb_state15.var_15) + (((((0x8e8a2a49) + (rb_state15.var_13)) - (0x41721c9b)) + (rb_state15.var_12)) - (0xc17485f5));
        rb_state15.var_16 = (rb_state15.var_16) + (rb_state15.var_14);
        rb_state15.var_17 = (rb_state15.var_17) + (rb_state15.var_15);
        rb_state15.var_18 = (rb_state15.var_18) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_16) : (0x30a2ee99));
        rb_state15.var_19 = (rb_state15.var_19) + (((((0x890037ec) ^ (rb_state15.var_10)) - (rb_state15.var_17)) + (rb_state15.var_18)) ^ (0x9a7e7113));
        rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0x8d88da0);
    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0xf7277260)) ? (rb_state17.var_0) : (0xf0539aff));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_1);
    #endif
    workDispatch.Lock();
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x7da331b3)) {
        if ((rb_state1.var_3) != (0x0)) {
            if (!((rb_state1.var_3) == (rb_state1.var_24))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_2) == (0xcd97d893)) {
        rb_state3.var_5 = 0x1;
    }
    if ((rb_state3.var_2) == (0xcd97d893)) {
        pthread_mutex_lock(&(rb_state3.lock_15));
        rb_state3.var_6 = 0x6712876c;
        rb_state3.var_7 = 0x3d4f9bfd;
        rb_state3.var_8 = (rb_state3.var_8) + (rb_state3.var_4);
        rb_state3.var_9 = (rb_state3.var_9) + (((0x34a198c1) + (rb_state3.var_3)) ^ (0x367e213c));
        rb_state3.var_10 = (rb_state3.var_10) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0x99e1a2a3));
        rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_8) : (0xb6e3ff81));
        rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_10) : (0xfc9bc3e5));
        rb_state3.var_13 = (rb_state3.var_13) ^ (rb_state3.var_11);
        rb_state3.var_14 = (rb_state3.var_14) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_13) : (0xde23a22b));
        rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_14);
        rb_state3.var_3 = rb_state3.var_4;
        pthread_mutex_unlock(&(rb_state3.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ ((0xf636497f) ^ (rb_state4.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + ((0x1622ee5c) ^ (0x60fec327));
    if ((rb_state5.var_2) == (0x76dc2d7b)) {
        pthread_mutex_lock(&(rb_state5.lock_28));
        rb_state5.var_5 = 0x6882179f;
        rb_state5.var_6 = 0xf36486e0;
        rb_state5.var_7 = (rb_state5.var_7) - (0xb4845117);
        rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_5) : (0x944da4ae));
        rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_6) == (0xf36486e0)) ? (rb_state5.var_3) : (0x43a3c8c5));
        rb_state5.var_10 = (rb_state5.var_10) + (rb_state5.var_7);
        rb_state5.var_11 = (rb_state5.var_11) - (((0x5b4c5e8a) ^ (rb_state5.var_8)) ^ (0xed45a0ff));
        rb_state5.var_12 = (rb_state5.var_12) ^ (((0x72c4a064) + (rb_state5.var_5)) ^ (rb_state5.var_9));
        rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_6) == (0xf36486e0)) ? (rb_state5.var_10) : (0x25d57887));
        rb_state5.var_14 = (rb_state5.var_14) ^ (rb_state5.var_11);
        rb_state5.var_15 = (rb_state5.var_15) - (((rb_state5.var_7) == (0x4b7baee9)) ? (rb_state5.var_12) : (0xa4fb8495));
        rb_state5.var_16 = (rb_state5.var_16) ^ (rb_state5.var_13);
        rb_state5.var_17 = (rb_state5.var_17) + (rb_state5.var_14);
        rb_state5.var_18 = (rb_state5.var_18) - (rb_state5.var_15);
        rb_state5.var_19 = (rb_state5.var_19) - (rb_state5.var_16);
        rb_state5.var_20 = (rb_state5.var_20) ^ (((((0xfdeca8d) - (rb_state5.var_17)) + (0x7e6f1c)) - (0x4e3f6d18)) + (rb_state5.var_18));
        rb_state5.var_21 = (rb_state5.var_21) + (((rb_state5.var_19) == (0x4b7baee9)) ? (rb_state5.var_20) : (0x3a6c1d9f));
        rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_21);
        rb_state5.var_3 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_28));
    }
    if ((rb_state5.var_2) == (0x76dc2d7b)) {
        if (!((rb_state5.var_3) == (rb_state5.var_4))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_2) : (0xb10c31fc));
    if ((rb_state9.var_2) == (0x3b981f21)) {
        rb_state9.var_14 = 0x84939acd;
        rb_state9.var_15 = (rb_state9.var_15) ^ (((((0xefe62b1) ^ (rb_state9.var_10)) ^ (rb_state9.var_9)) - (0x13f18727)) ^ (rb_state9.var_8));
        rb_state9.var_16 = (rb_state9.var_16) - (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_14) : (0x6ff08c5a));
        rb_state9.var_17 = (rb_state9.var_17) ^ (0xd4f7dac5);
        rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_15);
        rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_16) : (0x364dd7c6));
        rb_state9.var_20 = (rb_state9.var_20) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_17) : (0xe00b667c));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_14) == (0x84939acd)) ? (rb_state9.var_18) : (0xb26e7da0));
        rb_state9.var_22 = (rb_state9.var_22) ^ (((rb_state9.var_15) == (0xfb0cdb8a)) ? (rb_state9.var_19) : (0x900df3e2));
        rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_20);
        rb_state9.var_24 = (rb_state9.var_24) - (((rb_state9.var_21) == (0xfb0cdb8a)) ? (rb_state9.var_22) : (0x8763b518));
        rb_state9.var_25 = (rb_state9.var_25) ^ (rb_state9.var_23);
        rb_state9.var_26 = (rb_state9.var_26) - (((0xaec0a03b) - (rb_state9.var_24)) ^ (rb_state9.var_16));
        rb_state9.var_27 = (rb_state9.var_27) + (rb_state9.var_25);
        rb_state9.var_28 = (rb_state9.var_28) ^ (((((0xde740069) - (0x78f9a1e0)) ^ (0x14918010)) ^ (rb_state9.var_27)) + (rb_state9.var_26));
        rb_state9.var_13 = (rb_state9.var_13) + (rb_state9.var_28);
        rb_state9.var_3 = rb_state9.var_13;
    }
    if ((rb_state9.var_2) == (0x3b981f21)) {
        pthread_mutex_lock(&(rb_state9.lock_39));
        rb_state9.var_30 = 0xb025047b;
        rb_state9.var_31 = (rb_state9.var_31) + (0xc2640be7);
        rb_state9.var_32 = (rb_state9.var_32) ^ (((rb_state9.var_19) == (0x7b6c6533)) ? (rb_state9.var_30) : (0x8eacfccc));
        rb_state9.var_33 = (rb_state9.var_33) - (((rb_state9.var_20) == (0xd4f7dac5)) ? (rb_state9.var_18) : (0x6baa5963));
        rb_state9.var_34 = (rb_state9.var_34) + (((rb_state9.var_21) == (0xfb0cdb8a)) ? (rb_state9.var_17) : (0xd8e73552));
        rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_31) == (0xc2640be7)) ? (rb_state9.var_32) : (0xa54d535c));
        rb_state9.var_36 = (rb_state9.var_36) - (((((0xa2d157d8) - (rb_state9.var_23)) ^ (rb_state9.var_33)) + (rb_state9.var_34)) - (rb_state9.var_22));
        rb_state9.var_37 = (rb_state9.var_37) - (((0xb5a3a102) ^ (rb_state9.var_35)) - (rb_state9.var_24));
        rb_state9.var_38 = (rb_state9.var_38) ^ (((((0x59110564) + (0x8e87e54c)) - (rb_state9.var_37)) + (rb_state9.var_36)) + (rb_state9.var_25));
        rb_state9.var_29 = (rb_state9.var_29) - (rb_state9.var_38);
        rb_state9.var_3 = rb_state9.var_29;
        pthread_mutex_unlock(&(rb_state9.lock_39));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x40aededa)) {
        rb_state10.var_3 = rb_state10.var_1;
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_16 = (rb_state12.var_16) ^ (743 < rb_input_size ? (uint32_t)rb_input[743] : 0xf92d5a96);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0xc3573f9d));
    rb_state19.var_2 = (rb_state19.var_2) ^ ((0xd5b56dcc) ^ (0x7860de99));
    #endif
    rank = counter;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0x804f836d) + (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x6c)) ? (rb_state3.var_0) : (0x6041df9));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xf1919551)) {
        rb_state4.var_10 = 0xeac34fa;
        rb_state4.var_11 = (rb_state4.var_11) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_10) : (0x85a7e993));
        rb_state4.var_12 = (rb_state4.var_12) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_5) : (0xbfaeaee2));
        rb_state4.var_13 = (rb_state4.var_13) - (((((0xcd2e84ca) - (0x18bf01b7)) + (rb_state4.var_11)) - (0x59b25940)) ^ (rb_state4.var_12));
        rb_state4.var_9 = (rb_state4.var_9) ^ (rb_state4.var_13);
        rb_state4.var_2 = rb_state4.var_9;
    }
    if ((rb_state4.var_1) == (0xf1919551)) {
        pthread_mutex_lock(&(rb_state4.lock_38));
        rb_state4.var_15 = 0x6a91e8d3;
        rb_state4.var_16 = 0xdd2e395b;
        rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_16) : (0x5b6c7302));
        rb_state4.var_18 = (rb_state4.var_18) - (rb_state4.var_9);
        rb_state4.var_19 = (rb_state4.var_19) + (((0x63c4d4af) + (0xef69b730)) + (rb_state4.var_15));
        rb_state4.var_20 = (rb_state4.var_20) - (((rb_state4.var_10) == (0xeac34fa)) ? (rb_state4.var_7) : (0x749716ad));
        rb_state4.var_21 = (rb_state4.var_21) + (((rb_state4.var_11) == (0xeac34fa)) ? (rb_state4.var_17) : (0x2d13f565));
        rb_state4.var_22 = (rb_state4.var_22) ^ (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_18) : (0xfc0ec989));
        rb_state4.var_23 = (rb_state4.var_23) ^ (((((0xfdae38cc) - (rb_state4.var_19)) - (rb_state4.var_20)) + (0x55496c89)) ^ (0x492c8b75));
        rb_state4.var_24 = (rb_state4.var_24) ^ (rb_state4.var_21);
        rb_state4.var_25 = (rb_state4.var_25) ^ (((0x3141e93f) - (rb_state4.var_22)) - (0x6efdbfff));
        rb_state4.var_26 = (rb_state4.var_26) - (((0x9449b329) ^ (rb_state4.var_13)) + (rb_state4.var_23));
        rb_state4.var_27 = (rb_state4.var_27) - (rb_state4.var_24);
        rb_state4.var_28 = (rb_state4.var_28) - (((0x16689f31) + (rb_state4.var_25)) + (0x3471bd4d));
        rb_state4.var_29 = (rb_state4.var_29) ^ (rb_state4.var_26);
        rb_state4.var_30 = (rb_state4.var_30) + (rb_state4.var_27);
        rb_state4.var_31 = (rb_state4.var_31) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_28) : (0xbc0361f5));
        rb_state4.var_32 = (rb_state4.var_32) + (((rb_state4.var_29) == (0x21053210)) ? (rb_state4.var_30) : (0x349ff17));
        rb_state4.var_33 = (rb_state4.var_33) ^ (((rb_state4.var_15) == (0x6a91e8d3)) ? (rb_state4.var_31) : (0x68bc5b2f));
        rb_state4.var_34 = (rb_state4.var_34) - (rb_state4.var_32);
        rb_state4.var_35 = (rb_state4.var_35) + (((0x9f4ca9c8) ^ (rb_state4.var_33)) - (rb_state4.var_16));
        rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_34);
        rb_state4.var_37 = (rb_state4.var_37) + (((rb_state4.var_35) == (0x5fcb55de)) ? (rb_state4.var_36) : (0xfd8d30c7));
        rb_state4.var_14 = (rb_state4.var_14) + (rb_state4.var_37);
        rb_state4.var_2 = rb_state4.var_14;
        pthread_mutex_unlock(&(rb_state4.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (0x283afa33);
    if ((rb_state7.var_0) == (0x35a45b97)) {
        pthread_mutex_lock(&(rb_state7.lock_25));
        rb_state7.var_5 = 0xbb2b4abe;
        rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_5) : (0xdc81d6cc));
        rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_3) : (0x5789dfbc));
        rb_state7.var_8 = (rb_state7.var_8) ^ (((0xc164edd2) ^ (rb_state7.var_6)) + (0xce7e8371));
        rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0x4fbb75d2));
        rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_9);
        rb_state7.var_3 = rb_state7.var_4;
        pthread_mutex_unlock(&(rb_state7.lock_25));
    }
    if ((rb_state7.var_0) == (0x35a45b97)) {
        if (!((rb_state7.var_3) == (rb_state7.var_4))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x40aededa)) {
        if (!((rb_state10.var_1) == (rb_state10.var_3))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x8989b479)) {
        pthread_mutex_lock(&(rb_state13.lock_32));
        rb_state13.var_13 = 0x9d012690;
        rb_state13.var_14 = 0x94ccb677;
        rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_14);
        rb_state13.var_16 = (rb_state13.var_16) ^ (((((0xd1bae49c) + (rb_state13.var_5)) + (0x7029e7ea)) ^ (0x801e9ba)) - (rb_state13.var_13));
        rb_state13.var_17 = (rb_state13.var_17) + (((0xe32d8a78) + (0xf64de0b1)) + (rb_state13.var_15));
        rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_16);
        rb_state13.var_19 = (rb_state13.var_19) ^ (rb_state13.var_17);
        rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_6) == (0x75f5f2e1)) ? (rb_state13.var_18) : (0x8560a3b1));
        rb_state13.var_21 = (rb_state13.var_21) - (rb_state13.var_19);
        rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_20);
        rb_state13.var_23 = (rb_state13.var_23) + (((rb_state13.var_7) == (0x54559213)) ? (rb_state13.var_21) : (0xd8f68f3d));
        rb_state13.var_24 = (rb_state13.var_24) + (((0x9988dcec) + (rb_state13.var_22)) - (rb_state13.var_8));
        rb_state13.var_25 = (rb_state13.var_25) ^ (((0xb0feb4d8) + (rb_state13.var_9)) ^ (rb_state13.var_23));
        rb_state13.var_26 = (rb_state13.var_26) ^ (rb_state13.var_24);
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_25) : (0x23b15d96));
        rb_state13.var_28 = (rb_state13.var_28) - (rb_state13.var_26);
        rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_11) == (0x75f5f2e1)) ? (rb_state13.var_27) : (0x546f7b8c));
        rb_state13.var_30 = (rb_state13.var_30) - (rb_state13.var_28);
        rb_state13.var_31 = (rb_state13.var_31) ^ (((((0x9179fa6e) ^ (0xda1fd2d3)) - (rb_state13.var_29)) - (0x96e334b0)) + (rb_state13.var_30));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_31);
        pthread_mutex_unlock(&(rb_state13.lock_32));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x9679bfb9)) {
        rb_state17.var_4 = 0x42a9051f;
        rb_state17.var_5 = 0x66f70960;
        rb_state17.var_6 = 0x60f20009;
        rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_4) : (0x4eb989c9));
        rb_state17.var_8 = (rb_state17.var_8) - (((((0x82b37bb7) ^ (0x4c84f5a6)) ^ (rb_state17.var_6)) + (rb_state17.var_4)) + (rb_state17.var_5));
        rb_state17.var_9 = (rb_state17.var_9) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_7) : (0x64c5e00b));
        rb_state17.var_10 = (rb_state17.var_10) ^ (((0x7ee869bc) - (rb_state17.var_8)) + (rb_state17.var_6));
        rb_state17.var_11 = (rb_state17.var_11) ^ (((((0xf9bc40d1) + (0x9d1bc04c)) ^ (rb_state17.var_9)) ^ (rb_state17.var_7)) + (rb_state17.var_10));
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_11);
    }
    #endif
    counter++;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0xb932a128) + (rb_state10.var_1));
    #endif
    workDispatch.Unlock();

    while (!doExit) {

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0xc01b0acf));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) + (0xeeea9bd2);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_3) == (0x211f382d)) ? (rb_state14.var_16) : (0x9fa94b07));
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