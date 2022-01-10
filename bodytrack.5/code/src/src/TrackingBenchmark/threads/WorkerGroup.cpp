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
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_0);
    #endif
    RegisterCmd((thread_cmd_t)_cmd, obj);
}

void WorkerGroup::RegisterCmd(thread_cmd_t _cmd, Threadable &obj) {
    if (_cmd >= cmds.size()) {
        cmds.resize(_cmd + 1, NULL);
    }
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_3) == (0x0)) {
        rb_state1.var_9 = 0x364102f5;
        rb_state1.var_10 = (rb_state1.var_10) ^ (0xbd172d27);
        rb_state1.var_11 = (rb_state1.var_11) ^ (((rb_state1.var_6) == (0x68a3fdb0)) ? (rb_state1.var_7) : (0x3ac2ec77));
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) ^ (rb_state1.var_10);
        rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_8) == (0x7c7b2b2a)) ? (rb_state1.var_11) : (0x39e880b2));
        rb_state1.var_15 = (rb_state1.var_15) - (((0x67c2595b) + (rb_state1.var_9)) + (rb_state1.var_12));
        rb_state1.var_16 = (rb_state1.var_16) + (((rb_state1.var_10) == (0xbd172d27)) ? (rb_state1.var_13) : (0x8fcf88aa));
        rb_state1.var_17 = (rb_state1.var_17) - (rb_state1.var_14);
        rb_state1.var_18 = (rb_state1.var_18) ^ (((0x1293f0f4) + (rb_state1.var_15)) - (rb_state1.var_11));
        rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_16);
        rb_state1.var_20 = (rb_state1.var_20) + (((rb_state1.var_12) == (0xc9befd0b)) ? (rb_state1.var_17) : (0xa542c574));
        rb_state1.var_21 = (rb_state1.var_21) - (((rb_state1.var_13) == (0xbd172d27)) ? (rb_state1.var_18) : (0x1c74bd7c));
        rb_state1.var_22 = (rb_state1.var_22) + (rb_state1.var_19);
        rb_state1.var_23 = (rb_state1.var_23) ^ (((0xfc267623) + (rb_state1.var_14)) ^ (rb_state1.var_20));
        rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_21) == (0xd8b33d3d)) ? (rb_state1.var_22) : (0x93078c7e));
        rb_state1.var_25 = (rb_state1.var_25) - (((rb_state1.var_15) == (0x983da6a5)) ? (rb_state1.var_23) : (0xf0b41964));
        rb_state1.var_26 = (rb_state1.var_26) - (((((0x349bffaf) ^ (rb_state1.var_25)) ^ (rb_state1.var_24)) + (rb_state1.var_16)) ^ (0xe8ba4d42));
        rb_state1.var_4 = (rb_state1.var_4) ^ (rb_state1.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (93 < rb_input_size ? (uint32_t)rb_input[93] : 0x11027e0d);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (0xa04446cb);
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

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x31b)) {
        pthread_mutex_lock(&(rb_state0.lock_19));
        rb_state0.var_6 = 0x39aaa78a;
        rb_state0.var_7 = 0x421653b2;
        rb_state0.var_8 = (rb_state0.var_8) - (((((0xe1b34f07) - (rb_state0.var_4)) - (rb_state0.var_5)) - (0x1d488161)) + (rb_state0.var_6));
        rb_state0.var_9 = (rb_state0.var_9) + (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_7) : (0x1ca0072d));
        rb_state0.var_10 = (rb_state0.var_10) ^ (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_8) : (0xe138c15d));
        rb_state0.var_11 = (rb_state0.var_11) + (((((0x65592adf) ^ (rb_state0.var_8)) - (rb_state0.var_9)) + (rb_state0.var_9)) + (rb_state0.var_10));
        rb_state0.var_5 = (rb_state0.var_5) - (rb_state0.var_11);
        rb_state0.var_4 = rb_state0.var_5;
        pthread_mutex_unlock(&(rb_state0.lock_19));
    }
    if ((rb_state0.var_0) == (0x31b)) {
        if (!((rb_state0.var_4) == (rb_state0.var_5))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (0x94de87d6);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0x43a9d1a4)) {
        if ((rb_state5.var_4) != (0x0)) {
            if (!((rb_state5.var_4) == (rb_state5.var_15))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_2) : (0x2e20da5b));
    rb_state6.var_4 = (rb_state6.var_4) - ((0xa0b79a8d) + (0xc5f3534b));
    #endif
    workDispatch.Lock();

    while (cmd == THREADS_IDLE) {
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_2) == (0x43a9d1a4)) {
            pthread_mutex_lock(&(rb_state5.lock_31));
            rb_state5.var_8 = 0xb73fc538;
            rb_state5.var_9 = (rb_state5.var_9) + (((0xea30d5ce) - (rb_state5.var_8)) + (rb_state5.var_7));
            rb_state5.var_10 = (rb_state5.var_10) ^ (1409 < rb_input_size ? (uint32_t)rb_input[1409] : 0x99883498);
            rb_state5.var_11 = (rb_state5.var_11) + (((((0x1b038a69) ^ (0x22c29c29)) - (rb_state5.var_8)) - (rb_state5.var_6)) - (0x82c683da));
            rb_state5.var_12 = (rb_state5.var_12) - (((rb_state5.var_9) == (0x32f11096)) ? (rb_state5.var_9) : (0x4b5e3e42));
            rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_10) == (0x33)) ? (rb_state5.var_11) : (0x424597c2));
            rb_state5.var_14 = (rb_state5.var_14) - (((((0x8c8cdbfc) - (rb_state5.var_10)) ^ (rb_state5.var_12)) ^ (rb_state5.var_13)) + (0xeaad1522));
            rb_state5.var_7 = (rb_state5.var_7) - (rb_state5.var_14);
            rb_state5.var_4 = rb_state5.var_7;
            pthread_mutex_unlock(&(rb_state5.lock_31));
        }
        if ((rb_state5.var_2) == (0x43a9d1a4)) {
            pthread_mutex_lock(&(rb_state5.lock_31));
            rb_state5.var_16 = 0x4869519e;
            rb_state5.var_17 = 0xdd35896f;
            rb_state5.var_18 = (rb_state5.var_18) - (rb_state5.var_12);
            rb_state5.var_19 = (rb_state5.var_19) ^ (rb_state5.var_11);
            rb_state5.var_20 = (rb_state5.var_20) + (((rb_state5.var_13) == (0x4532d2)) ? (rb_state5.var_17) : (0x57a602f9));
            rb_state5.var_21 = (rb_state5.var_21) - (((0xb2d2cb34) + (rb_state5.var_16)) + (0x6f12735c));
            rb_state5.var_22 = (rb_state5.var_22) + (((0x53a03b63) - (rb_state5.var_18)) ^ (0x7805f649));
            rb_state5.var_23 = (rb_state5.var_23) + (rb_state5.var_19);
            rb_state5.var_24 = (rb_state5.var_24) ^ (rb_state5.var_20);
            rb_state5.var_25 = (rb_state5.var_25) + (((0x75e275a2) + (rb_state5.var_14)) + (rb_state5.var_21));
            rb_state5.var_26 = (rb_state5.var_26) ^ (rb_state5.var_22);
            rb_state5.var_27 = (rb_state5.var_27) + (((0x2b2c0b46) + (0x3eedec37)) - (rb_state5.var_23));
            rb_state5.var_28 = (rb_state5.var_28) + (((rb_state5.var_24) == (0xdd35896f)) ? (rb_state5.var_25) : (0xc4d0df31));
            rb_state5.var_29 = (rb_state5.var_29) + (((((0x74978b98) + (0x322ee86e)) - (rb_state5.var_26)) - (rb_state5.var_27)) - (rb_state5.var_15));
            rb_state5.var_30 = (rb_state5.var_30) - (((((0xe8a88c37) + (rb_state5.var_28)) - (rb_state5.var_17)) ^ (rb_state5.var_29)) ^ (rb_state5.var_16));
            rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_30);
            rb_state5.var_4 = rb_state5.var_15;
            pthread_mutex_unlock(&(rb_state5.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - ((0xab4d5c1a) + (rb_state6.var_0));
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0xe7ef7f27));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0x51e5512f) ^ (0xb4dd5dbc));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0x62c57227);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x4b18670e)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_32))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (567 < rb_input_size ? (uint32_t)rb_input[567] : 0x4bcdba77);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x8b88637f)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_7))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) + (((rb_state11.var_1) == (0x30)) ? (rb_state11.var_0) : (0xbbfff192));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x41eb7922)) ? (rb_state12.var_0) : (0x7526c6ea));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_1) == (0xf27ea32)) ? (rb_state14.var_0) : (0x19d30cb9));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - ((0x4b872e97) + (rb_state15.var_3));
    rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_3) == (0xb478d169)) ? (rb_state15.var_2) : (0x8aa1eefa));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + ((0xf1bc24ea) - (0xedc4a461));
    rb_state16.var_2 = (rb_state16.var_2) ^ (709 < rb_input_size ? (uint32_t)rb_input[709] : 0xf40a8ae2);
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + ((0xd77dac29) + (0x84a5a353));
    rb_state19.var_3 = (rb_state19.var_3) - (502 < rb_input_size ? (uint32_t)rb_input[502] : 0xebc10615);
    #endif
    bool doExit = false;
    static thread_rank_t counter = 0;
    thread_rank_t rank;
    thread_internal_cmd_t cmd;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (1396 < rb_input_size ? (uint32_t)rb_input[1396] : 0x7531716e);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + ((0x7787210c) ^ (0x152a8abf));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0x902108a7);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (0x48902a4c);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + ((0xdff6eee4) + (rb_state4.var_1));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0x7d8e04c7) - (0x1e5cb16a));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0x57264478));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (0x9f0bb1ce);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_1);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - (0xc865c271);
    #endif
    workDispatch.Lock();
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) + ((0x686f6e0e) - (0x4fa010e8));
    rb_state0.var_2 = (rb_state0.var_2) - (1543 < rb_input_size ? (uint32_t)rb_input[1543] : 0x4e6b719b);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_2);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + ((0x4777cb0a) - (0xa5a2e307));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (0xcf67c6a4);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - ((0xada5f76f) + (0x4d559058));
    if ((rb_state11.var_3) == (0x38)) {
        rb_state11.var_18 = 0x9ded984a;
        rb_state11.var_19 = 0x89d15007;
        rb_state11.var_20 = (rb_state11.var_20) ^ (rb_state11.var_12);
        rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_19) == (0x89d15007)) ? (rb_state11.var_18) : (0xa97475f2));
        rb_state11.var_22 = (rb_state11.var_22) - (((rb_state11.var_20) == (0x1c2c9bd0)) ? (rb_state11.var_21) : (0x4846e28b));
        rb_state11.var_5 = (rb_state11.var_5) + (rb_state11.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ ((0xca586d65) + (0x35633ed7));
    #endif
    rank = counter;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_3) == (0xcfb38b80)) {
        rb_state3.var_5 = 0xa5ac7a85;
        rb_state3.var_6 = (rb_state3.var_6) + (((rb_state3.var_5) == (0xa5ac7a85)) ? (rb_state3.var_4) : (0x5d421732));
        rb_state3.var_7 = (rb_state3.var_7) ^ (((0xa0d02fb9) - (rb_state3.var_5)) - (rb_state3.var_6));
        rb_state3.var_8 = (rb_state3.var_8) + (0x348acabb);
        rb_state3.var_9 = (rb_state3.var_9) - (((((0x9385abf7) ^ (rb_state3.var_7)) - (0x9a8f95b9)) - (rb_state3.var_7)) + (rb_state3.var_6));
        rb_state3.var_10 = (rb_state3.var_10) + (((rb_state3.var_8) == (0x348acabb)) ? (rb_state3.var_9) : (0x2a8aa1cb));
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_10);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_2);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x937b2870)) {
        pthread_mutex_lock(&(rb_state15.lock_50));
        rb_state15.var_24 = 0xee56cb73;
        rb_state15.var_25 = 0x38bf59db;
        rb_state15.var_26 = (rb_state15.var_26) ^ (106 < rb_input_size ? (uint32_t)rb_input[106] : 0x39ddbc48);
        rb_state15.var_27 = (rb_state15.var_27) ^ ((0x7e360e8) ^ (0x7a7a62fa));
        rb_state15.var_28 = (rb_state15.var_28) ^ (((((0xfd0c37a2) ^ (0xd3b7ef74)) - (rb_state15.var_14)) ^ (rb_state15.var_25)) + (rb_state15.var_13));
        rb_state15.var_29 = (rb_state15.var_29) ^ (rb_state15.var_24);
        rb_state15.var_30 = (rb_state15.var_30) - (rb_state15.var_26);
        rb_state15.var_31 = (rb_state15.var_31) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_27) : (0xa6d53966));
        rb_state15.var_32 = (rb_state15.var_32) ^ (((rb_state15.var_28) == (0x1604810d)) ? (rb_state15.var_29) : (0xdaf99b5e));
        rb_state15.var_33 = (rb_state15.var_33) ^ (rb_state15.var_30);
        rb_state15.var_34 = (rb_state15.var_34) ^ (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_31) : (0x6c774a84));
        rb_state15.var_35 = (rb_state15.var_35) + (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_32) : (0xe00a5db1));
        rb_state15.var_36 = (rb_state15.var_36) ^ (rb_state15.var_33);
        rb_state15.var_37 = (rb_state15.var_37) ^ (((((0x4f92f1e0) - (rb_state15.var_35)) ^ (0x95e1d6a2)) + (0x62d48ba7)) - (rb_state15.var_34));
        rb_state15.var_38 = (rb_state15.var_38) ^ (rb_state15.var_36);
        rb_state15.var_39 = (rb_state15.var_39) + (((((0x99da0ca5) ^ (rb_state15.var_38)) + (0x2573bdad)) - (rb_state15.var_18)) + (rb_state15.var_37));
        rb_state15.var_23 = (rb_state15.var_23) ^ (rb_state15.var_39);
        rb_state15.var_4 = rb_state15.var_23;
        pthread_mutex_unlock(&(rb_state15.lock_50));
    }
    if ((rb_state15.var_0) == (0x937b2870)) {
        pthread_mutex_lock(&(rb_state15.lock_50));
        rb_state15.var_41 = (rb_state15.var_41) + (0xe0ac1733);
        rb_state15.var_42 = (rb_state15.var_42) ^ (((((0xe922ae8f) - (rb_state15.var_20)) ^ (rb_state15.var_21)) ^ (rb_state15.var_22)) ^ (rb_state15.var_23));
        rb_state15.var_43 = (rb_state15.var_43) - (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_41) : (0x2f37441b));
        rb_state15.var_44 = (rb_state15.var_44) + (rb_state15.var_42);
        rb_state15.var_45 = (rb_state15.var_45) - (rb_state15.var_43);
        rb_state15.var_46 = (rb_state15.var_46) + (((rb_state15.var_24) == (0xee56cb73)) ? (rb_state15.var_44) : (0x7424fd92));
        rb_state15.var_47 = (rb_state15.var_47) - (rb_state15.var_45);
        rb_state15.var_48 = (rb_state15.var_48) + (((rb_state15.var_25) == (0x38bf59db)) ? (rb_state15.var_46) : (0x9e2b2bc1));
        rb_state15.var_49 = (rb_state15.var_49) - (((rb_state15.var_47) == (0x1f53e8cd)) ? (rb_state15.var_48) : (0xb7d187bb));
        rb_state15.var_40 = (rb_state15.var_40) - (rb_state15.var_49);
        rb_state15.var_4 = rb_state15.var_40;
        pthread_mutex_unlock(&(rb_state15.lock_50));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x89bfb548);
    rb_state16.var_0 = (rb_state16.var_0) + (643 < rb_input_size ? (uint32_t)rb_input[643] : 0xad692332);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_4) == (0x90cb8542)) {
        if ((rb_state19.var_5) != (0x0)) {
            if (!((rb_state19.var_5) == (rb_state19.var_23))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    counter++;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - (0x8421c2e4);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_4 = (rb_state16.var_4) ^ ((0x7f4cf83) - (0x439badb7));
    #endif
    workDispatch.Unlock();

    while (!doExit) {

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_3) == (0x61bfbbfe)) ? (rb_state0.var_2) : (0x5697219f));
        rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_2) == (0xfffffef7)) ? (rb_state0.var_1) : (0xffed2ef));
        rb_state0.var_3 = (rb_state0.var_3) ^ (0xcd42a1d0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (0x9adb7b6);
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