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
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0xdbb9e0ff)) {
        pthread_mutex_lock(&(rb_state6.lock_37));
        rb_state6.var_3 = 0x46128549;
        rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_2);
        rb_state6.var_5 = (rb_state6.var_5) + (rb_state6.var_3);
        rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_4);
        rb_state6.var_7 = (rb_state6.var_7) ^ (((0x11dfbfeb) ^ (rb_state6.var_3)) - (0xc5436aa4));
        rb_state6.var_8 = (rb_state6.var_8) - (rb_state6.var_1);
        rb_state6.var_9 = (rb_state6.var_9) ^ (((0x996cc2d9) + (rb_state6.var_5)) ^ (rb_state6.var_4));
        rb_state6.var_10 = (rb_state6.var_10) ^ (((0xb8285e98) - (rb_state6.var_5)) - (0x80d692d3));
        rb_state6.var_11 = (rb_state6.var_11) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_6) : (0x7428d2dd));
        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_7);
        rb_state6.var_13 = (rb_state6.var_13) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_9) : (0xd334859d));
        rb_state6.var_14 = (rb_state6.var_14) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0x2f874fbd));
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0xcd1b873c));
        rb_state6.var_16 = (rb_state6.var_16) + (((0xb3f17ffc) - (0xbd455bab)) - (rb_state6.var_14));
        rb_state6.var_17 = (rb_state6.var_17) ^ (rb_state6.var_15);
        rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_16) : (0x9a8bef87));
        rb_state6.var_19 = (rb_state6.var_19) - (rb_state6.var_17);
        rb_state6.var_20 = (rb_state6.var_20) + (((((0x17aec2ce) + (0x80cc322b)) + (0x564103e6)) ^ (rb_state6.var_18)) - (rb_state6.var_19));
        rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_20);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_37));
    }
    if ((rb_state6.var_0) == (0xdbb9e0ff)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
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

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x1731965b)) {
        pthread_mutex_lock(&(rb_state2.lock_23));
        rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_2);
        rb_state2.var_4 = (rb_state2.var_4) + (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_3) : (0xd2d36043));
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_4);
        rb_state2.var_1 = rb_state2.var_2;
        pthread_mutex_unlock(&(rb_state2.lock_23));
    }
    if ((rb_state2.var_0) == (0x1731965b)) {
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xda576c9f)) {
        rb_state8.var_9 = 0x6816b2a3;
        rb_state8.var_10 = 0x4e962617;
        rb_state8.var_11 = 0xb7fafd32;
        rb_state8.var_12 = (rb_state8.var_12) + (rb_state8.var_4);
        rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_9) == (0x6816b2a3)) ? (rb_state8.var_11) : (0x2da2957c));
        rb_state8.var_14 = (rb_state8.var_14) ^ (((((0x93478910) + (rb_state8.var_10)) + (rb_state8.var_5)) ^ (rb_state8.var_12)) ^ (0x58cbe60f));
        rb_state8.var_15 = (rb_state8.var_15) + (((0x735816ad) - (rb_state8.var_13)) ^ (rb_state8.var_6));
        rb_state8.var_16 = (rb_state8.var_16) ^ (((0xbdd13d59) ^ (rb_state8.var_7)) ^ (rb_state8.var_14));
        rb_state8.var_17 = (rb_state8.var_17) ^ (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_15) : (0xa941ff45));
        rb_state8.var_18 = (rb_state8.var_18) + (((((0xe25844d4) ^ (rb_state8.var_16)) + (0xaea2c419)) + (0x9cbc8264)) - (rb_state8.var_17));
        rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_18);
        rb_state8.var_1 = rb_state8.var_8;
    }
    if ((rb_state8.var_0) == (0xda576c9f)) {
        pthread_mutex_lock(&(rb_state8.lock_27));
        rb_state8.var_20 = 0xfaedb7dd;
        rb_state8.var_21 = 0xa7bfbc2e;
        rb_state8.var_22 = (rb_state8.var_22) ^ (((rb_state8.var_20) == (0xfaedb7dd)) ? (rb_state8.var_9) : (0xa47b27fe));
        rb_state8.var_23 = (rb_state8.var_23) + (((rb_state8.var_11) == (0xb7fafd32)) ? (rb_state8.var_10) : (0xf2219276));
        rb_state8.var_24 = (rb_state8.var_24) - (rb_state8.var_21);
        rb_state8.var_25 = (rb_state8.var_25) ^ (((rb_state8.var_22) == (0x6816b2a3)) ? (rb_state8.var_23) : (0xa707c08c));
        rb_state8.var_26 = (rb_state8.var_26) - (((((0x20967ec9) + (rb_state8.var_24)) ^ (0xe026cfbe)) - (rb_state8.var_25)) ^ (0xaf200d1));
        rb_state8.var_19 = (rb_state8.var_19) + (rb_state8.var_26);
        rb_state8.var_1 = rb_state8.var_19;
        pthread_mutex_unlock(&(rb_state8.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if (!((rb_state14.var_10) == (rb_state14.var_33))) {
        rb_state14.var_46 = rb_state14.var_24;
    }
    if (!((rb_state14.var_10) == (rb_state14.var_33))) {
        if (!((rb_state14.var_24) == (rb_state14.var_46))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x4aa3ebeb)) {
        if ((rb_state15.var_1) != (0x0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_13))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    workDispatch.Lock();

    while (cmd == THREADS_IDLE) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (270 < rb_input_size ? (uint32_t)rb_input[270] : 0x28ffa6cd);
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0xdbb9e0ff)) {
            rb_state6.var_21 = 0xaa4cca48;
            rb_state6.var_22 = 0xc746bc7c;
            rb_state6.var_23 = (rb_state6.var_23) + (((((0xe5b35c84) + (0x36f3449)) ^ (rb_state6.var_21)) ^ (rb_state6.var_8)) - (rb_state6.var_9));
            rb_state6.var_24 = (rb_state6.var_24) - (0xd2416aec);
            rb_state6.var_25 = (rb_state6.var_25) - (((0xe71585a1) ^ (rb_state6.var_22)) + (rb_state6.var_10));
            rb_state6.var_26 = (rb_state6.var_26) + (rb_state6.var_23);
            rb_state6.var_27 = (rb_state6.var_27) ^ (((0xdb44771a) ^ (0x746e7812)) - (rb_state6.var_24));
            rb_state6.var_28 = (rb_state6.var_28) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_25) : (0xe324bc26));
            rb_state6.var_29 = (rb_state6.var_29) + (((0x38dbac78) + (rb_state6.var_26)) ^ (0x4b6af5e8));
            rb_state6.var_30 = (rb_state6.var_30) ^ (((0x2fa074a3) ^ (rb_state6.var_27)) - (rb_state6.var_12));
            rb_state6.var_31 = (rb_state6.var_31) - (((0x7b7bbc25) - (rb_state6.var_28)) + (0x7714cb49));
            rb_state6.var_32 = (rb_state6.var_32) ^ (((0x9f34cb95) ^ (rb_state6.var_29)) + (rb_state6.var_13));
            rb_state6.var_33 = (rb_state6.var_33) - (((rb_state6.var_30) == (0x0)) ? (rb_state6.var_31) : (0x581452d3));
            rb_state6.var_34 = (rb_state6.var_34) + (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_32) : (0x3384d2ec));
            rb_state6.var_35 = (rb_state6.var_35) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_33) : (0x84fab7d8));
            rb_state6.var_36 = (rb_state6.var_36) ^ (((rb_state6.var_34) == (0x0)) ? (rb_state6.var_35) : (0xdf5e8200));
            rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_36);
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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x4277080f));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + ((0x71f7282d) + (rb_state11.var_0));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x76b11463));
    if ((rb_state13.var_0) == (0x0)) {
        rb_state13.var_2 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0x0)) {
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + ((0xe4e8942d) - (rb_state16.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0xa02373a1)) ? (rb_state18.var_0) : (0x4c77e505));
    #endif
    bool doExit = false;
    static thread_rank_t counter = 0;
    thread_rank_t rank;
    thread_internal_cmd_t cmd;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x4b0536c));
    rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_15) : (0x8ed36e57));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x5454420f);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (0x107f263e);
    if ((rb_state11.var_0) == (0x246805cd)) {
        if ((rb_state11.var_1) != (0x0)) {
            if (!((rb_state11.var_1) == (rb_state11.var_22))) {
                pthread_mutex_lock(&(rb_state11.lock_52));
                rb_state11.var_40 = 0x9e77f87f;
                rb_state11.var_41 = 0x7ef564a;
                rb_state11.var_42 = (rb_state11.var_42) + (((0x8edc4cf8) ^ (rb_state11.var_15)) - (rb_state11.var_14));
                rb_state11.var_43 = (rb_state11.var_43) ^ (((rb_state11.var_41) == (0x0)) ? (rb_state11.var_40) : (0x634494db));
                rb_state11.var_44 = (rb_state11.var_44) - (((0xfe0b28e8) + (rb_state11.var_42)) ^ (rb_state11.var_16));
                rb_state11.var_45 = (rb_state11.var_45) - (((0xdecf558e) - (rb_state11.var_43)) - (0xde015363));
                rb_state11.var_46 = (rb_state11.var_46) + (((0x548d0630) ^ (rb_state11.var_44)) ^ (0xc935ebf7));
                rb_state11.var_47 = (rb_state11.var_47) + (((rb_state11.var_45) == (0x0)) ? (rb_state11.var_46) : (0x6bc6e81b));
                rb_state11.var_39 = (rb_state11.var_39) ^ (rb_state11.var_47);
                rb_state11.var_38 = rb_state11.var_39;
                pthread_mutex_unlock(&(rb_state11.lock_52));
            }
            if (!((rb_state11.var_1) == (rb_state11.var_22))) {
                pthread_mutex_lock(&(rb_state11.lock_52));
                if (!((rb_state11.var_38) == (rb_state11.var_39))) {
                    racebench_trigger(11);
                }
                pthread_mutex_unlock(&(rb_state11.lock_52));
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_5 = (rb_state14.var_5) - ((0xdfb753e7) + (0xb59fa8a9));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
    if ((rb_state16.var_0) == (0xc9d1285a)) {
        rb_state16.var_2 = rb_state16.var_1;
    }
    if ((rb_state16.var_0) == (0xc9d1285a)) {
        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    workDispatch.Lock();
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0x831bb82b) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (289 < rb_input_size ? (uint32_t)rb_input[289] : 0xaa099d0f);
    if ((rb_state3.var_0) == (0xa)) {
        if ((rb_state3.var_1) != (0x0)) {
            if (!((rb_state3.var_1) == (rb_state3.var_22))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x656a41cc));
    rb_state14.var_0 = (rb_state14.var_0) ^ ((0x5bcf61e) + (rb_state14.var_0));
    if ((rb_state14.var_6) == (0x4cd7ca7)) {
        if ((rb_state14.var_10) != (0x0)) {
            if (!((rb_state14.var_10) == (rb_state14.var_33))) {
                rb_state14.var_47 = 0xf8f86757;
                rb_state14.var_48 = (rb_state14.var_48) ^ (((rb_state14.var_27) == (0x0)) ? (rb_state14.var_25) : (0x876a48fe));
                rb_state14.var_49 = (rb_state14.var_49) + (((rb_state14.var_28) == (0x0)) ? (rb_state14.var_26) : (0x83d1cf7f));
                rb_state14.var_50 = (rb_state14.var_50) - (((((0x8fed8dd5) ^ (0x5ea6231c)) - (rb_state14.var_47)) + (rb_state14.var_29)) + (rb_state14.var_48));
                rb_state14.var_51 = (rb_state14.var_51) + (rb_state14.var_49);
                rb_state14.var_52 = (rb_state14.var_52) - (((0xff6009e0) - (rb_state14.var_50)) + (0x6daa5875));
                rb_state14.var_53 = (rb_state14.var_53) + (((rb_state14.var_51) == (0x0)) ? (rb_state14.var_52) : (0x761be3ec));
                rb_state14.var_24 = (rb_state14.var_24) ^ (rb_state14.var_53);
            }
        }
    }
    #endif
    rank = counter;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_17) == (0x40ad365f)) {
        pthread_mutex_lock(&(rb_state1.lock_41));
        if ((rb_state1.var_34) != (0x0)) {
            rb_state1.var_42 = !((rb_state1.var_32) == (rb_state1.var_33));
        }
        pthread_mutex_unlock(&(rb_state1.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x1731965b) ^ (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        rb_state13.var_3 = 0x5918e145;
        rb_state13.var_4 = 0xbfe0e63a;
        rb_state13.var_5 = 0x7231887e;
        rb_state13.var_6 = (rb_state13.var_6) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_2) : (0xf230f407));
        rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_3) : (0xb6f5a3ad));
        rb_state13.var_8 = (rb_state13.var_8) - (((0x4f830951) - (rb_state13.var_5)) ^ (rb_state13.var_5));
        rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_4) : (0x87d13e2d));
        rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0xc4806ffb));
        rb_state13.var_11 = (rb_state13.var_11) - (rb_state13.var_8);
        rb_state13.var_12 = (rb_state13.var_12) - (((0x502be927) + (rb_state13.var_7)) ^ (rb_state13.var_9));
        rb_state13.var_13 = (rb_state13.var_13) + (rb_state13.var_10);
        rb_state13.var_14 = (rb_state13.var_14) + (((((0xaa034df5) + (rb_state13.var_8)) - (0xd3522372)) + (rb_state13.var_11)) - (rb_state13.var_12));
        rb_state13.var_15 = (rb_state13.var_15) ^ (((0xc7c408b2) ^ (rb_state13.var_13)) - (0x665e4137));
        rb_state13.var_16 = (rb_state13.var_16) - (((((0x24678f90) - (rb_state13.var_15)) - (0xc0570992)) ^ (rb_state13.var_14)) ^ (0x9d268df6));
        rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x5bcf61e)) {
        pthread_mutex_lock(&(rb_state14.lock_8));
        rb_state14.var_3 = 0x4cd7ca7;
        rb_state14.var_4 = 0xad94e474;
        rb_state14.var_5 = (rb_state14.var_5) ^ (((((0xa5e483b0) ^ (rb_state14.var_1)) + (0x5b5bc258)) - (rb_state14.var_2)) - (rb_state14.var_4));
        rb_state14.var_6 = (rb_state14.var_6) ^ (((rb_state14.var_3) == (0x4cd7ca7)) ? (rb_state14.var_3) : (0xae5b50ec));
        rb_state14.var_7 = (rb_state14.var_7) ^ (((((0x4b970b9d) ^ (rb_state14.var_5)) ^ (rb_state14.var_4)) ^ (0x7d7a2775)) - (rb_state14.var_6));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_7);
        rb_state14.var_2 = rb_state14.var_1;
        pthread_mutex_unlock(&(rb_state14.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xa02373a1)) {
        rb_state18.var_12 = 0x2e957196;
        rb_state18.var_13 = (rb_state18.var_13) - (((rb_state18.var_8) == (0xd03aba2e)) ? (rb_state18.var_12) : (0xfd4ea62));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_7) == (0xf813cb6c)) ? (rb_state18.var_13) : (0xd33d7129));
        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_14);
    }
    #endif
    counter++;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_21) == (0xcb0a0e2d)) {
        rb_state1.var_43 = rb_state1.var_22;
    }
    if ((rb_state1.var_21) == (0xcb0a0e2d)) {
        if (!((rb_state1.var_22) == (rb_state1.var_43))) {
            racebench_trigger(1);
        }
    }
    #endif
    workDispatch.Unlock();

    while (!doExit) {

        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_21) == (0xcb0a0e2d)) {
            rb_state1.var_44 = 0x84ec8657;
            rb_state1.var_45 = (rb_state1.var_45) ^ (((rb_state1.var_25) == (0x7c77f218)) ? (rb_state1.var_44) : (0xeb49b818));
            rb_state1.var_46 = (rb_state1.var_46) - (((rb_state1.var_26) == (0x7e8a3148)) ? (rb_state1.var_23) : (0x9db5c135));
            rb_state1.var_47 = (rb_state1.var_47) + (((rb_state1.var_28) == (0x2fba40dc)) ? (rb_state1.var_27) : (0xd306d2d8));
            rb_state1.var_48 = (rb_state1.var_48) - (rb_state1.var_24);
            rb_state1.var_49 = (rb_state1.var_49) - (((rb_state1.var_45) == (0x84ec8657)) ? (rb_state1.var_46) : (0xde2362ea));
            rb_state1.var_50 = (rb_state1.var_50) ^ (rb_state1.var_47);
            rb_state1.var_51 = (rb_state1.var_51) ^ (((rb_state1.var_48) == (0xd36d5d18)) ? (rb_state1.var_49) : (0xccd8c6f6));
            rb_state1.var_52 = (rb_state1.var_52) ^ (rb_state1.var_50);
            rb_state1.var_53 = (rb_state1.var_53) + (rb_state1.var_51);
            rb_state1.var_54 = (rb_state1.var_54) + (((0xf91e4b9) + (0x8883c1ce)) - (rb_state1.var_52));
            rb_state1.var_55 = (rb_state1.var_55) - (((rb_state1.var_29) == (0x2def52ea)) ? (rb_state1.var_53) : (0x46e3fa4e));
            rb_state1.var_56 = (rb_state1.var_56) - (((0xf504351c) ^ (rb_state1.var_54)) ^ (rb_state1.var_31));
            rb_state1.var_57 = (rb_state1.var_57) - (rb_state1.var_55);
            rb_state1.var_58 = (rb_state1.var_58) + (((rb_state1.var_56) == (0x7919b957)) ? (rb_state1.var_57) : (0xf149f54d));
            rb_state1.var_22 = (rb_state1.var_22) - (rb_state1.var_58);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ ((0x4628de25) - (rb_state9.var_0));
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