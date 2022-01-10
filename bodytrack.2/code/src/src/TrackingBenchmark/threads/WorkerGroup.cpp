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
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_8 = (rb_state3.var_8) - ((0x45f0120d) - (0x6370cf8b));
        #endif
        cmds.resize(_cmd + 1, NULL);
    }
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x44b61dc4)) {
        pthread_mutex_lock(&(rb_state19.lock_59));
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_24))) {
                racebench_trigger(19);
            }
        }
        pthread_mutex_unlock(&(rb_state19.lock_59));
    }
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

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_5) == (0x9d54ab72)) {
        rb_state3.var_13 = 0x1;
    }
    if ((rb_state3.var_5) == (0x9d54ab72)) {
        pthread_mutex_lock(&(rb_state3.lock_34));
        rb_state3.var_14 = (rb_state3.var_14) ^ (((((0xc452c862) + (rb_state3.var_6)) ^ (0xd032ab6d)) - (0x4fddaf5c)) - (rb_state3.var_10));
        rb_state3.var_15 = (rb_state3.var_15) - (0x914ee8f9);
        rb_state3.var_16 = (rb_state3.var_16) - ((0xd65ea756) ^ (rb_state3.var_12));
        rb_state3.var_17 = (rb_state3.var_17) + (((0x3082c92) - (rb_state3.var_13)) - (rb_state3.var_11));
        rb_state3.var_18 = (rb_state3.var_18) + (((0xaf205d66) + (rb_state3.var_14)) + (rb_state3.var_14));
        rb_state3.var_19 = (rb_state3.var_19) ^ (rb_state3.var_15);
        rb_state3.var_20 = (rb_state3.var_20) - (rb_state3.var_16);
        rb_state3.var_21 = (rb_state3.var_21) + (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_17) : (0xb126fed));
        rb_state3.var_22 = (rb_state3.var_22) ^ (rb_state3.var_18);
        rb_state3.var_23 = (rb_state3.var_23) + (((rb_state3.var_19) == (0x0)) ? (rb_state3.var_20) : (0x23688b07));
        rb_state3.var_24 = (rb_state3.var_24) + (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_21) : (0x6e5fcb80));
        rb_state3.var_25 = (rb_state3.var_25) ^ (((0x6cd74fb9) + (rb_state3.var_22)) + (0xd97f41bf));
        rb_state3.var_26 = (rb_state3.var_26) ^ (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_23) : (0xda1bf354));
        rb_state3.var_27 = (rb_state3.var_27) + (((0x79cf8e34) + (rb_state3.var_24)) - (0x5dd4df));
        rb_state3.var_28 = (rb_state3.var_28) ^ (((rb_state3.var_18) == (0x0)) ? (rb_state3.var_25) : (0x7e052756));
        rb_state3.var_29 = (rb_state3.var_29) + (((0x8032c57) ^ (rb_state3.var_26)) + (0xf4f5b372));
        rb_state3.var_30 = (rb_state3.var_30) ^ (((0x958260) + (rb_state3.var_19)) - (rb_state3.var_27));
        rb_state3.var_31 = (rb_state3.var_31) ^ (((0x2102188a) ^ (rb_state3.var_20)) ^ (rb_state3.var_28));
        rb_state3.var_32 = (rb_state3.var_32) ^ (((((0x793cd8d7) - (rb_state3.var_29)) ^ (rb_state3.var_22)) ^ (rb_state3.var_30)) ^ (rb_state3.var_21));
        rb_state3.var_33 = (rb_state3.var_33) - (((rb_state3.var_31) == (0x0)) ? (rb_state3.var_32) : (0xebcee83c));
        rb_state3.var_12 = (rb_state3.var_12) - (rb_state3.var_33);
        rb_state3.var_11 = rb_state3.var_12;
        pthread_mutex_unlock(&(rb_state3.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xbe70abf6));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (1510 < rb_input_size ? (uint32_t)rb_input[1510] : 0xec169388);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_8 = (rb_state18.var_8) ^ ((0xa4364e79) - (0x42026bf1));
    #endif
    workDispatch.Lock();

    while (cmd == THREADS_IDLE) {
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0xfffffff6)) {
            pthread_mutex_lock(&(rb_state3.lock_9));
            rb_state3.var_4 = 0x9d172eb1;
            rb_state3.var_5 = 0x9d54ab72;
            rb_state3.var_6 = 0x535eba3b;
            rb_state3.var_7 = (rb_state3.var_7) + (((((0x77917444) + (rb_state3.var_2)) - (0x619ea606)) - (rb_state3.var_4)) - (rb_state3.var_5));
            rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_7) : (0xd340f0ae));
            rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_8);
            rb_state3.var_3 = rb_state3.var_2;
            pthread_mutex_unlock(&(rb_state3.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_22) == (0x44729d1a)) {
            rb_state4.var_39 = 0x6d8a3837;
            rb_state4.var_40 = (rb_state4.var_40) - ((0xb0c5e832) ^ (rb_state4.var_26));
            rb_state4.var_41 = (rb_state4.var_41) + (((rb_state4.var_39) == (0x0)) ? (rb_state4.var_24) : (0x73c4a26f));
            rb_state4.var_42 = (rb_state4.var_42) ^ (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_25) : (0xa8fad7a5));
            rb_state4.var_43 = (rb_state4.var_43) + (((((0xb9a08d8c) ^ (rb_state4.var_29)) + (rb_state4.var_40)) + (rb_state4.var_41)) - (rb_state4.var_28));
            rb_state4.var_44 = (rb_state4.var_44) + (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_42) : (0xc35e7db5));
            rb_state4.var_45 = (rb_state4.var_45) - (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_43) : (0x4e75987f));
            rb_state4.var_46 = (rb_state4.var_46) + (((rb_state4.var_32) == (0x0)) ? (rb_state4.var_44) : (0x85bcc8b));
            rb_state4.var_47 = (rb_state4.var_47) - (((0xac8923f9) ^ (rb_state4.var_45)) - (0xcf2e5d5));
            rb_state4.var_48 = (rb_state4.var_48) ^ (((rb_state4.var_33) == (0x0)) ? (rb_state4.var_46) : (0xd6587c41));
            rb_state4.var_49 = (rb_state4.var_49) + (((rb_state4.var_34) == (0x0)) ? (rb_state4.var_47) : (0x486c7910));
            rb_state4.var_50 = (rb_state4.var_50) ^ (((rb_state4.var_35) == (0x0)) ? (rb_state4.var_48) : (0x5845c3be));
            rb_state4.var_51 = (rb_state4.var_51) + (((rb_state4.var_49) == (0x0)) ? (rb_state4.var_50) : (0xc8b86a0e));
            rb_state4.var_23 = (rb_state4.var_23) - (rb_state4.var_51);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_1) == (0x269b95d4)) {
            rb_state6.var_10 = 0x8fdb139f;
            rb_state6.var_11 = (rb_state6.var_11) ^ (((0x25ffebca) - (rb_state6.var_6)) + (rb_state6.var_7));
            rb_state6.var_12 = (rb_state6.var_12) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0x35f92900));
            rb_state6.var_13 = (rb_state6.var_13) ^ (451 < rb_input_size ? (uint32_t)rb_input[451] : 0x58b840e5);
            rb_state6.var_14 = (rb_state6.var_14) - (rb_state6.var_5);
            rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_10) : (0x511b795));
            rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_12) : (0xe10c6a69));
            rb_state6.var_17 = (rb_state6.var_17) + (rb_state6.var_13);
            rb_state6.var_18 = (rb_state6.var_18) + (((((0x9f236720) - (0x2403b423)) - (rb_state6.var_14)) ^ (rb_state6.var_11)) - (rb_state6.var_15));
            rb_state6.var_19 = (rb_state6.var_19) - (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_16) : (0x769e4f9c));
            rb_state6.var_20 = (rb_state6.var_20) - (((0xaacc81f2) - (rb_state6.var_17)) + (0xa1fa633f));
            rb_state6.var_21 = (rb_state6.var_21) + (((0x875bfa82) ^ (rb_state6.var_13)) - (rb_state6.var_18));
            rb_state6.var_22 = (rb_state6.var_22) - (((0x701d1bbd) + (rb_state6.var_19)) ^ (0x884fe44));
            rb_state6.var_23 = (rb_state6.var_23) + (((0x3489f511) + (rb_state6.var_20)) + (0x286757d1));
            rb_state6.var_24 = (rb_state6.var_24) + (((0x6350e3a9) + (rb_state6.var_21)) + (rb_state6.var_14));
            rb_state6.var_25 = (rb_state6.var_25) ^ (rb_state6.var_22);
            rb_state6.var_26 = (rb_state6.var_26) ^ (rb_state6.var_23);
            rb_state6.var_27 = (rb_state6.var_27) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_24) : (0x215c9065));
            rb_state6.var_28 = (rb_state6.var_28) - (((0x3bdf75ba) + (rb_state6.var_16)) + (rb_state6.var_25));
            rb_state6.var_29 = (rb_state6.var_29) ^ (((0x71762fd6) + (rb_state6.var_26)) + (rb_state6.var_17));
            rb_state6.var_30 = (rb_state6.var_30) - (((rb_state6.var_27) == (0x0)) ? (rb_state6.var_28) : (0xc385fc0c));
            rb_state6.var_31 = (rb_state6.var_31) ^ (rb_state6.var_29);
            rb_state6.var_32 = (rb_state6.var_32) ^ (((((0x8924f926) ^ (0x57b5860c)) - (rb_state6.var_31)) - (0xb47616fd)) + (rb_state6.var_30));
            rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_32);
            rb_state6.var_2 = rb_state6.var_9;
        }
        if ((rb_state6.var_1) == (0x269b95d4)) {
            pthread_mutex_lock(&(rb_state6.lock_44));
            rb_state6.var_34 = 0xdafa37;
            rb_state6.var_35 = 0x9df49a5a;
            rb_state6.var_36 = (rb_state6.var_36) ^ (((((0x4fefe3ac) ^ (0x3e051238)) ^ (rb_state6.var_19)) - (rb_state6.var_35)) - (rb_state6.var_34));
            rb_state6.var_37 = (rb_state6.var_37) - (rb_state6.var_18);
            rb_state6.var_38 = (rb_state6.var_38) - (((0x29d78a59) ^ (rb_state6.var_20)) ^ (rb_state6.var_36));
            rb_state6.var_39 = (rb_state6.var_39) + (rb_state6.var_37);
            rb_state6.var_40 = (rb_state6.var_40) ^ (((0x7a9e646) ^ (rb_state6.var_21)) + (rb_state6.var_38));
            rb_state6.var_41 = (rb_state6.var_41) - (rb_state6.var_39);
            rb_state6.var_42 = (rb_state6.var_42) + (((0x1cbc011f) ^ (0x4d0564b6)) + (rb_state6.var_40));
            rb_state6.var_43 = (rb_state6.var_43) ^ (((((0x7ad15163) + (0x477a3f06)) + (rb_state6.var_22)) + (rb_state6.var_42)) + (rb_state6.var_41));
            rb_state6.var_33 = (rb_state6.var_33) + (rb_state6.var_43);
            rb_state6.var_2 = rb_state6.var_33;
            pthread_mutex_unlock(&(rb_state6.lock_44));
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (((rb_state0.var_0) == (0xea0ba903)) ? (rb_state0.var_0) : (0x60ebd837));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (0x926785ae);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (0xbdb31bfb);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xc0bfb0b6));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (1107 < rb_input_size ? (uint32_t)rb_input[1107] : 0x7e38011f);
    if ((rb_state9.var_0) == (0xffffff6d)) {
        rb_state9.var_4 = 0x1;
    }
    if ((rb_state9.var_0) == (0xffffff6d)) {
        pthread_mutex_lock(&(rb_state9.lock_22));
        rb_state9.var_5 = 0xa47426fa;
        rb_state9.var_6 = 0xfa9596fb;
        rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0xdf0465f4));
        rb_state9.var_8 = (rb_state9.var_8) ^ (917 < rb_input_size ? (uint32_t)rb_input[917] : 0x2eaecda9);
        rb_state9.var_9 = (rb_state9.var_9) ^ (rb_state9.var_6);
        rb_state9.var_10 = (rb_state9.var_10) - (rb_state9.var_5);
        rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_2) : (0x474b233d));
        rb_state9.var_12 = (rb_state9.var_12) + (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_7) : (0xc56c20d6));
        rb_state9.var_13 = (rb_state9.var_13) ^ (((0x611bf063) ^ (rb_state9.var_8)) - (rb_state9.var_7));
        rb_state9.var_14 = (rb_state9.var_14) ^ (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0xb9f50637));
        rb_state9.var_15 = (rb_state9.var_15) ^ (((0xc85ad2fd) - (0x45a630ef)) ^ (rb_state9.var_10));
        rb_state9.var_16 = (rb_state9.var_16) - (((((0xe16bf279) ^ (rb_state9.var_11)) ^ (rb_state9.var_9)) - (rb_state9.var_10)) - (rb_state9.var_12));
        rb_state9.var_17 = (rb_state9.var_17) - (((((0x11e0ea0d) + (0x4d070622)) - (rb_state9.var_14)) - (rb_state9.var_13)) + (rb_state9.var_11));
        rb_state9.var_18 = (rb_state9.var_18) - (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_15) : (0xcd58c4a6));
        rb_state9.var_19 = (rb_state9.var_19) + (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_17) : (0x9f16323));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((0xe90ab0d2) - (rb_state9.var_18)) ^ (0xb3753464));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_20) : (0xde3ea09d));
        rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_21);
        rb_state9.var_2 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (0x4821e259);
    rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x48e00488));
    #endif
    bool doExit = false;
    static thread_rank_t counter = 0;
    thread_rank_t rank;
    thread_internal_cmd_t cmd;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (224 < rb_input_size ? (uint32_t)rb_input[224] : 0xec5b0633);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x309626fc)) {
        pthread_mutex_lock(&(rb_state5.lock_35));
        rb_state5.var_4 = 0x5914fb5b;
        rb_state5.var_5 = 0x901bb096;
        rb_state5.var_6 = 0x7b7fb859;
        rb_state5.var_7 = (rb_state5.var_7) - (((0x5e7f5339) - (rb_state5.var_5)) - (0xea77d23));
        rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_6) : (0x436b0527));
        rb_state5.var_9 = (rb_state5.var_9) - (((0xf28b4ba8) - (rb_state5.var_4)) ^ (0x95cc5293));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_7) : (0x7b61f13b));
        rb_state5.var_11 = (rb_state5.var_11) + (rb_state5.var_8);
        rb_state5.var_12 = (rb_state5.var_12) - (rb_state5.var_9);
        rb_state5.var_13 = (rb_state5.var_13) - (((0xe360ddd7) ^ (rb_state5.var_4)) - (rb_state5.var_10));
        rb_state5.var_14 = (rb_state5.var_14) + (((0x7859b22e) - (rb_state5.var_11)) - (0x103fc37f));
        rb_state5.var_15 = (rb_state5.var_15) + (((0xead04d2f) + (rb_state5.var_12)) - (rb_state5.var_5));
        rb_state5.var_16 = (rb_state5.var_16) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_13) : (0x63c5a732));
        rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_14) : (0x56e074aa));
        rb_state5.var_18 = (rb_state5.var_18) - (((0xdaae01b8) + (rb_state5.var_15)) ^ (0x863a0d9a));
        rb_state5.var_19 = (rb_state5.var_19) + (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_17) : (0x49e1b68a));
        rb_state5.var_20 = (rb_state5.var_20) + (((0x122722fa) ^ (rb_state5.var_8)) - (rb_state5.var_18));
        rb_state5.var_21 = (rb_state5.var_21) + (rb_state5.var_19);
        rb_state5.var_22 = (rb_state5.var_22) + (rb_state5.var_20);
        rb_state5.var_23 = (rb_state5.var_23) - (((0x12cab2e8) + (rb_state5.var_21)) - (rb_state5.var_9));
        rb_state5.var_24 = (rb_state5.var_24) - (((((0xfcff0d73) + (rb_state5.var_23)) + (rb_state5.var_22)) + (rb_state5.var_10)) ^ (rb_state5.var_11));
        rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_24);
        rb_state5.var_2 = rb_state5.var_3;
        pthread_mutex_unlock(&(rb_state5.lock_35));
    }
    if ((rb_state5.var_1) == (0x309626fc)) {
        if (!((rb_state5.var_2) == (rb_state5.var_3))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (1083 < rb_input_size ? (uint32_t)rb_input[1083] : 0xae74bd2e);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xf58b33aa)) {
        pthread_mutex_lock(&(rb_state11.lock_18));
        rb_state11.var_4 = 0xcad98f1f;
        rb_state11.var_5 = 0x83de3c0f;
        rb_state11.var_6 = (rb_state11.var_6) ^ (rb_state11.var_3);
        rb_state11.var_7 = (rb_state11.var_7) ^ (rb_state11.var_4);
        rb_state11.var_8 = (rb_state11.var_8) + (rb_state11.var_2);
        rb_state11.var_9 = (rb_state11.var_9) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_5) : (0x3a42b81c));
        rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xeacb29c6));
        rb_state11.var_11 = (rb_state11.var_11) ^ (((0xf0ec58ad) - (rb_state11.var_8)) - (rb_state11.var_5));
        rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_9);
        rb_state11.var_13 = (rb_state11.var_13) - (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_10) : (0x84ddf4ba));
        rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_11) : (0x56d597da));
        rb_state11.var_15 = (rb_state11.var_15) + (((0x93d7ffdc) - (rb_state11.var_12)) - (0x26c77815));
        rb_state11.var_16 = (rb_state11.var_16) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_14) : (0x8d0bfa8));
        rb_state11.var_17 = (rb_state11.var_17) + (((((0xefa6a46c) - (rb_state11.var_16)) + (0xcfc97ff1)) ^ (rb_state11.var_15)) ^ (0xdbd7eb6d));
        rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_17);
        rb_state11.var_3 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (0x2b8e787f);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0x0)) {
        rb_state14.var_2 = rb_state14.var_0;
    }
    if ((rb_state14.var_1) == (0x0)) {
        if (!((rb_state14.var_0) == (rb_state14.var_2))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x461c437f));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ ((0xaf49bbe7) + (rb_state19.var_0));
    #endif
    workDispatch.Lock();
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x64ac670d)) {
        pthread_mutex_lock(&(rb_state1.lock_25));
        rb_state1.var_7 = 0xc0a0c462;
        rb_state1.var_8 = 0xfd064af8;
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_7) : (0x9453c54c));
        rb_state1.var_10 = (rb_state1.var_10) + (0x84ff40b9);
        rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_9) : (0x39d2a76e));
        rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0x73a3ad24));
        rb_state1.var_6 = (rb_state1.var_6) + (rb_state1.var_12);
        rb_state1.var_2 = rb_state1.var_6;
        pthread_mutex_unlock(&(rb_state1.lock_25));
    }
    if ((rb_state1.var_1) == (0x64ac670d)) {
        pthread_mutex_lock(&(rb_state1.lock_25));
        rb_state1.var_14 = 0x2d61f6be;
        rb_state1.var_15 = 0xdceffe6b;
        rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_7) : (0xebecbdde));
        rb_state1.var_17 = (rb_state1.var_17) + (rb_state1.var_6);
        rb_state1.var_18 = (rb_state1.var_18) - (rb_state1.var_14);
        rb_state1.var_19 = (rb_state1.var_19) ^ (((((0xffe7dbdb) + (rb_state1.var_9)) ^ (0x9a480ac8)) + (rb_state1.var_16)) + (rb_state1.var_15));
        rb_state1.var_20 = (rb_state1.var_20) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_17) : (0xd1234548));
        rb_state1.var_21 = (rb_state1.var_21) + (rb_state1.var_18);
        rb_state1.var_22 = (rb_state1.var_22) - (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_20) : (0x2366980));
        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_21) : (0x4d9412));
        rb_state1.var_24 = (rb_state1.var_24) ^ (((((0x8041bea7) + (rb_state1.var_23)) - (rb_state1.var_22)) + (0x8a147bfb)) ^ (rb_state1.var_12));
        rb_state1.var_13 = (rb_state1.var_13) + (rb_state1.var_24);
        rb_state1.var_2 = rb_state1.var_13;
        pthread_mutex_unlock(&(rb_state1.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + ((0xe170208b) + (rb_state4.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + ((0xd5e1a704) - (0xc9bc1d45));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (1267 < rb_input_size ? (uint32_t)rb_input[1267] : 0x118dc2cf);
    if ((rb_state12.var_1) == (0xffffffcc)) {
        rb_state12.var_8 = (rb_state12.var_8) - (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_6) : (0x6acd47c2));
        rb_state12.var_9 = (rb_state12.var_9) ^ (0x653ad4a7);
        rb_state12.var_10 = (rb_state12.var_10) - (((((0x79c563d) + (rb_state12.var_1)) + (rb_state12.var_9)) ^ (0x1344c775)) ^ (rb_state12.var_7));
        rb_state12.var_11 = (rb_state12.var_11) + (((0x74d2a20f) - (0xcdc1acf9)) ^ (rb_state12.var_8));
        rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_10) == (0xc053f118)) ? (rb_state12.var_9) : (0x4f37404b));
        rb_state12.var_13 = (rb_state12.var_13) ^ (((0x940a574b) ^ (rb_state12.var_11)) + (rb_state12.var_10));
        rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_11) == (0xddbe0cb2)) ? (rb_state12.var_12) : (0x277a104f));
        rb_state12.var_15 = (rb_state12.var_15) - (((((0xc1226582) ^ (rb_state12.var_13)) ^ (0x15e59e93)) ^ (rb_state12.var_12)) ^ (rb_state12.var_14));
        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x331c2cfc);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_0);
    if ((rb_state15.var_1) == (0x0)) {
        rb_state15.var_3 = 0x117beba2;
        rb_state15.var_4 = (rb_state15.var_4) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_2) : (0x34c5edb7));
        rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_4);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_1);
    if ((rb_state16.var_0) == (0xb44b76a4)) {
        pthread_mutex_lock(&(rb_state16.lock_52));
        if ((rb_state16.var_2) != (0x0)) {
            if (!((rb_state16.var_2) == (rb_state16.var_12))) {
                racebench_trigger(16);
            }
        }
        pthread_mutex_unlock(&(rb_state16.lock_52));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (0xcb448765);
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    rank = counter;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + ((0xe52c6ec1) + (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0xffffffec)) ? (rb_state3.var_0) : (0xce5299c0));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x6272fb4d)) {
        pthread_mutex_lock(&(rb_state4.lock_14));
        rb_state4.var_3 = 0x2aa64934;
        rb_state4.var_4 = (rb_state4.var_4) - (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0xc2927bcc));
        rb_state4.var_5 = (rb_state4.var_5) ^ ((0x3e41d067) + (0x677810e1));
        rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_3) : (0x2968a8e5));
        rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_4) : (0xd4c2a323));
        rb_state4.var_8 = (rb_state4.var_8) ^ (rb_state4.var_5);
        rb_state4.var_9 = (rb_state4.var_9) ^ (((((0x151a7a00) + (rb_state4.var_6)) ^ (rb_state4.var_7)) - (0x618c39eb)) ^ (0x14c2f0af));
        rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_8) : (0x10fa7ccf));
        rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_9) : (0x10cbd87a));
        rb_state4.var_12 = (rb_state4.var_12) ^ (((0x6a932b89) + (rb_state4.var_7)) ^ (rb_state4.var_10));
        rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_12) : (0x8de6a073));
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_13);
        rb_state4.var_2 = rb_state4.var_1;
        pthread_mutex_unlock(&(rb_state4.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x4821e259)) ? (rb_state13.var_1) : (0x51e7a528));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0x4bb4898d);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x4379367a));
    #endif
    counter++;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + ((0x564bde3d) - (rb_state3.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + ((0xb04c7d04) - (0xa1c03b95));
    if ((rb_state13.var_0) == (0x6e3ff6f0)) {
        pthread_mutex_lock(&(rb_state13.lock_22));
        rb_state13.var_4 = (rb_state13.var_4) ^ (rb_state13.var_2);
        rb_state13.var_5 = (rb_state13.var_5) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_4) : (0xbb6f894b));
        rb_state13.var_3 = (rb_state13.var_3) ^ (rb_state13.var_5);
        rb_state13.var_2 = rb_state13.var_3;
        pthread_mutex_unlock(&(rb_state13.lock_22));
    }
    if ((rb_state13.var_0) == (0x6e3ff6f0)) {
        if (!((rb_state13.var_2) == (rb_state13.var_3))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0x0)) {
        rb_state14.var_3 = 0x19dc1b25;
        rb_state14.var_4 = 0x33057d1;
        rb_state14.var_5 = (rb_state14.var_5) - (((((0x5d40cba8) ^ (rb_state14.var_3)) - (rb_state14.var_4)) + (rb_state14.var_4)) + (rb_state14.var_2));
        rb_state14.var_6 = (rb_state14.var_6) - (((((0xe92e859e) ^ (0x7816cc2a)) ^ (rb_state14.var_3)) - (rb_state14.var_5)) - (rb_state14.var_5));
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x44b61dc4)) {
        rb_state19.var_13 = 0x273378c5;
        rb_state19.var_14 = 0x534936be;
        rb_state19.var_15 = 0x622a7d9d;
        rb_state19.var_16 = (rb_state19.var_16) - (0xfc3501ef);
        rb_state19.var_17 = (rb_state19.var_17) + (((((0x3c704c8d) + (0xd0d2959b)) - (rb_state19.var_14)) ^ (rb_state19.var_3)) + (rb_state19.var_15));
        rb_state19.var_18 = (rb_state19.var_18) ^ (((0x4bc7d197) ^ (rb_state19.var_13)) - (0x7acc75fc));
        rb_state19.var_19 = (rb_state19.var_19) - (((0x17842207) + (rb_state19.var_4)) + (rb_state19.var_16));
        rb_state19.var_20 = (rb_state19.var_20) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_18) : (0x135e9007));
        rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_19) : (0x82950d0f));
        rb_state19.var_22 = (rb_state19.var_22) ^ (rb_state19.var_20);
        rb_state19.var_23 = (rb_state19.var_23) ^ (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_22) : (0xc277df92));
        rb_state19.var_12 = (rb_state19.var_12) - (rb_state19.var_23);
        rb_state19.var_2 = rb_state19.var_12;
    }
    if ((rb_state19.var_1) == (0x44b61dc4)) {
        pthread_mutex_lock(&(rb_state19.lock_59));
        rb_state19.var_25 = 0x56e137e9;
        rb_state19.var_26 = 0x70c7d2f2;
        rb_state19.var_27 = (rb_state19.var_27) + ((0x9025146) - (rb_state19.var_7));
        rb_state19.var_28 = (rb_state19.var_28) - ((0xb9d9f3ee) + (rb_state19.var_8));
        rb_state19.var_29 = (rb_state19.var_29) - (rb_state19.var_6);
        rb_state19.var_30 = (rb_state19.var_30) ^ (((((0xe383c52c) + (0x1089d6a3)) ^ (rb_state19.var_25)) + (rb_state19.var_26)) ^ (rb_state19.var_9));
        rb_state19.var_31 = (rb_state19.var_31) ^ (rb_state19.var_27);
        rb_state19.var_32 = (rb_state19.var_32) + (rb_state19.var_28);
        rb_state19.var_33 = (rb_state19.var_33) ^ (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_29) : (0xa9422cc0));
        rb_state19.var_34 = (rb_state19.var_34) ^ (((0x410a49c8) - (rb_state19.var_30)) - (rb_state19.var_11));
        rb_state19.var_35 = (rb_state19.var_35) - (rb_state19.var_31);
        rb_state19.var_36 = (rb_state19.var_36) ^ (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_32) : (0xdf33d1f6));
        rb_state19.var_37 = (rb_state19.var_37) - (((0x6d5cb22a) ^ (rb_state19.var_13)) + (rb_state19.var_33));
        rb_state19.var_38 = (rb_state19.var_38) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_34) : (0xb9003b72));
        rb_state19.var_39 = (rb_state19.var_39) ^ (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_35) : (0x6a37c525));
        rb_state19.var_40 = (rb_state19.var_40) + (rb_state19.var_36);
        rb_state19.var_41 = (rb_state19.var_41) - (rb_state19.var_37);
        rb_state19.var_42 = (rb_state19.var_42) + (((0xf43ea35f) + (0xd3cf4248)) - (rb_state19.var_38));
        rb_state19.var_43 = (rb_state19.var_43) - (((rb_state19.var_39) == (0x0)) ? (rb_state19.var_40) : (0xee31c70a));
        rb_state19.var_44 = (rb_state19.var_44) - (rb_state19.var_41);
        rb_state19.var_45 = (rb_state19.var_45) + (((((0xe2d91b00) + (rb_state19.var_42)) ^ (rb_state19.var_16)) + (rb_state19.var_43)) - (0xe2f4750));
        rb_state19.var_46 = (rb_state19.var_46) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_44) : (0x38ffc41));
        rb_state19.var_47 = (rb_state19.var_47) + (((0x8a3f7f59) ^ (rb_state19.var_45)) ^ (0x8ed16c35));
        rb_state19.var_48 = (rb_state19.var_48) + (((0x2df16e90) - (0x72af656d)) - (rb_state19.var_46));
        rb_state19.var_49 = (rb_state19.var_49) ^ (rb_state19.var_47);
        rb_state19.var_50 = (rb_state19.var_50) - (rb_state19.var_48);
        rb_state19.var_51 = (rb_state19.var_51) ^ (((0x7c85c66c) - (rb_state19.var_18)) - (rb_state19.var_49));
        rb_state19.var_52 = (rb_state19.var_52) - (rb_state19.var_50);
        rb_state19.var_53 = (rb_state19.var_53) ^ (rb_state19.var_51);
        rb_state19.var_54 = (rb_state19.var_54) + (((0xb0c37fb7) + (rb_state19.var_19)) ^ (rb_state19.var_52));
        rb_state19.var_55 = (rb_state19.var_55) ^ (rb_state19.var_53);
        rb_state19.var_56 = (rb_state19.var_56) + (((rb_state19.var_20) == (0x0)) ? (rb_state19.var_54) : (0xd36dddfb));
        rb_state19.var_57 = (rb_state19.var_57) ^ (((0xf02fb834) - (rb_state19.var_55)) ^ (0x55998a6a));
        rb_state19.var_58 = (rb_state19.var_58) - (((rb_state19.var_56) == (0x0)) ? (rb_state19.var_57) : (0xe36bad1f));
        rb_state19.var_24 = (rb_state19.var_24) + (rb_state19.var_58);
        rb_state19.var_2 = rb_state19.var_24;
        pthread_mutex_unlock(&(rb_state19.lock_59));
    }
    #endif
    workDispatch.Unlock();

    while (!doExit) {

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (0xc835e36f);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_1) == (0x1c491f0b)) {
            rb_state2.var_13 = 0x307715f4;
            rb_state2.var_14 = (rb_state2.var_14) ^ (((((0x2c3674e) + (rb_state2.var_13)) ^ (rb_state2.var_9)) - (rb_state2.var_6)) ^ (rb_state2.var_8));
            rb_state2.var_15 = (rb_state2.var_15) ^ (((((0x9afb3763) - (rb_state2.var_14)) ^ (0x60227708)) + (rb_state2.var_7)) + (0xa0d4d9e3));
            rb_state2.var_12 = (rb_state2.var_12) ^ (rb_state2.var_15);
            rb_state2.var_2 = rb_state2.var_12;
        }
        if ((rb_state2.var_1) == (0x1c491f0b)) {
            pthread_mutex_lock(&(rb_state2.lock_22));
            rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_12);
            rb_state2.var_18 = (rb_state2.var_18) - (0x9a699d6a);
            rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0x80b74328));
            rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_18) : (0x53a94a7d));
            rb_state2.var_21 = (rb_state2.var_21) + (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_20) : (0x344f779));
            rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_21);
            rb_state2.var_2 = rb_state2.var_16;
            pthread_mutex_unlock(&(rb_state2.lock_22));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_12) == (0x7a695746)) {
            rb_state4.var_26 = (rb_state4.var_26) - (1051 < rb_input_size ? (uint32_t)rb_input[1051] : 0x7c1bab4a);
            rb_state4.var_27 = (rb_state4.var_27) + (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_16) : (0xca353f2d));
            rb_state4.var_28 = (rb_state4.var_28) ^ (((0x1a42d7c0) ^ (rb_state4.var_19)) ^ (rb_state4.var_17));
            rb_state4.var_29 = (rb_state4.var_29) + (0xcf389bd5);
            rb_state4.var_30 = (rb_state4.var_30) + (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_26) : (0x1be463f6));
            rb_state4.var_31 = (rb_state4.var_31) ^ (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_28) : (0x8718a127));
            rb_state4.var_32 = (rb_state4.var_32) + (rb_state4.var_29);
            rb_state4.var_33 = (rb_state4.var_33) ^ (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_31) : (0xe3201ae8));
            rb_state4.var_34 = (rb_state4.var_34) - (rb_state4.var_32);
            rb_state4.var_35 = (rb_state4.var_35) + (((rb_state4.var_33) == (0x0)) ? (rb_state4.var_34) : (0x45bb2e5d));
            rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_35);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_1) : (0x9a6c6140));
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_12) == (0xc5bd47e4)) {
            pthread_mutex_lock(&(rb_state11.lock_33));
            rb_state11.var_22 = 0x95930d08;
            rb_state11.var_23 = (rb_state11.var_23) ^ (((rb_state11.var_16) == (0x8d0bfa8)) ? (rb_state11.var_15) : (0x4c2a9eb4));
            rb_state11.var_24 = (rb_state11.var_24) ^ (((((0x2f3a9e58) + (rb_state11.var_17)) - (rb_state11.var_13)) ^ (rb_state11.var_14)) ^ (0xa52d30));
            rb_state11.var_25 = (rb_state11.var_25) - (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_23) : (0xb2785444));
            rb_state11.var_26 = (rb_state11.var_26) ^ (rb_state11.var_24);
            rb_state11.var_27 = (rb_state11.var_27) + (((((0xac425619) ^ (rb_state11.var_26)) ^ (rb_state11.var_19)) - (rb_state11.var_25)) - (0xcc52c52e));
            rb_state11.var_21 = (rb_state11.var_21) ^ (rb_state11.var_27);
            rb_state11.var_20 = rb_state11.var_21;
            pthread_mutex_unlock(&(rb_state11.lock_33));
        }
        if ((rb_state11.var_12) == (0xc5bd47e4)) {
            if (!((rb_state11.var_20) == (rb_state11.var_21))) {
                racebench_trigger(11);
            }
        }
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