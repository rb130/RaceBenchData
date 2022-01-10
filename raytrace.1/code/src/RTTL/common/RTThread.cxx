#include "racebench_bugs.h"


#include <cstring>
#include <cerrno>

#include "RTTL/common/RTThread.hxx"

class MultiThreadedScheduler : public MultiThreadedSyncPrimitive {
public:
    MultiThreadedScheduler() : m_suspended(false), MultiThreadedSyncPrimitive() {
    }

    ~MultiThreadedScheduler() {
    }

    _INLINE int suspended() const {
        return m_suspended;
    }

    _INLINE void suspend(bool locked = false, int value = true) {
        int code;
        if (locked == false) {
            #ifdef RACEBENCH_BUG_4
            if ((rb_state4.var_0) == (0xd)) {
                rb_state4.var_16 = 0x7f8471f3;
                rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_14) : (0xdfa7934b));
                rb_state4.var_18 = (rb_state4.var_18) - (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_16) : (0xcdd021ec));
                rb_state4.var_19 = (rb_state4.var_19) ^ (((rb_state4.var_18) == (0x0)) ? (rb_state4.var_16) : (0xf91a549d));
                rb_state4.var_20 = (rb_state4.var_20) - (rb_state4.var_17);
                rb_state4.var_21 = (rb_state4.var_21) + (((0xe8d7b076) + (rb_state4.var_18)) + (rb_state4.var_19));
                rb_state4.var_22 = (rb_state4.var_22) - (rb_state4.var_19);
                rb_state4.var_23 = (rb_state4.var_23) - (((((0x30feef0a) ^ (rb_state4.var_20)) + (rb_state4.var_21)) ^ (rb_state4.var_20)) + (rb_state4.var_21));
                rb_state4.var_24 = (rb_state4.var_24) - (rb_state4.var_22);
                rb_state4.var_25 = (rb_state4.var_25) + (rb_state4.var_23);
                rb_state4.var_26 = (rb_state4.var_26) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_25) : (0x816c8114));
                rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_26);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) ^ (77 < rb_input_size ? (uint32_t)rb_input[77] : 0x330b28ce);
            if ((rb_state11.var_0) == (0x162)) {
                pthread_mutex_lock(&(rb_state11.lock_13));
                rb_state11.var_3 = 0x472643c7;
                rb_state11.var_4 = (rb_state11.var_4) ^ (((((0x8c7266d0) - (rb_state11.var_3)) - (0xf275f905)) + (rb_state11.var_1)) - (0x78710f2e));
                rb_state11.var_5 = (rb_state11.var_5) + (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_4) : (0x45a377e9));
                rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_5);
                rb_state11.var_1 = rb_state11.var_2;
                pthread_mutex_unlock(&(rb_state11.lock_13));
            }
            if ((rb_state11.var_0) == (0x162)) {
                if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                    racebench_trigger(11);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0x2d3e12db)) {
                pthread_mutex_lock(&(rb_state19.lock_23));
                rb_state19.var_3 = 0xc5481f;
                rb_state19.var_4 = (rb_state19.var_4) - (rb_state19.var_2);
                rb_state19.var_5 = (rb_state19.var_5) ^ ((0x91efa0ca) - (0x7ee76f13));
                rb_state19.var_6 = (rb_state19.var_6) + (((((0xba8006d2) ^ (rb_state19.var_3)) ^ (rb_state19.var_1)) - (rb_state19.var_3)) + (rb_state19.var_4));
                rb_state19.var_7 = (rb_state19.var_7) + (((rb_state19.var_5) == (0x130831b7)) ? (rb_state19.var_4) : (0x8d74151));
                rb_state19.var_8 = (rb_state19.var_8) - (((0x81ca8dd) ^ (rb_state19.var_6)) + (rb_state19.var_5));
                rb_state19.var_9 = (rb_state19.var_9) ^ (((((0xd0e93af3) ^ (rb_state19.var_6)) + (0xefc75ac2)) ^ (rb_state19.var_7)) - (rb_state19.var_7));
                rb_state19.var_10 = (rb_state19.var_10) - (((((0x51333629) + (rb_state19.var_8)) ^ (rb_state19.var_9)) ^ (0xe5b871f0)) + (0x67c54109));
                rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_10);
                rb_state19.var_1 = rb_state19.var_2;
                pthread_mutex_unlock(&(rb_state19.lock_23));
            }
            if ((rb_state19.var_0) == (0x2d3e12db)) {
                if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                    racebench_trigger(19);
                }
            }
            #endif
            code = lock();
        }
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0xd)) {
            pthread_mutex_lock(&(rb_state4.lock_27));
            rb_state4.var_3 = (rb_state4.var_3) + (131 < rb_input_size ? (uint32_t)rb_input[131] : 0x69953afd);
            rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_3);
            rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_1);
            rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_2) : (0x28bfbb6b));
            rb_state4.var_7 = (rb_state4.var_7) - (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_3) : (0xd7c2aabd));
            rb_state4.var_8 = (rb_state4.var_8) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_4) : (0x21a4e946));
            rb_state4.var_9 = (rb_state4.var_9) + (((((0x8d32ca36) ^ (rb_state4.var_7)) - (rb_state4.var_5)) ^ (rb_state4.var_6)) + (rb_state4.var_8));
            rb_state4.var_10 = (rb_state4.var_10) - (((0xefa58385) + (rb_state4.var_7)) + (0xd8991212));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0xac915da6));
            rb_state4.var_12 = (rb_state4.var_12) ^ (((((0xb93531a0) ^ (rb_state4.var_10)) + (rb_state4.var_11)) ^ (rb_state4.var_10)) - (rb_state4.var_9));
            rb_state4.var_13 = (rb_state4.var_13) - (((0xd0a311d8) ^ (rb_state4.var_11)) ^ (0xfbd85e17));
            rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_12) : (0x534e40f));
            rb_state4.var_15 = (rb_state4.var_15) ^ (((((0xaba8280) - (rb_state4.var_13)) ^ (rb_state4.var_13)) + (rb_state4.var_14)) + (0xa73b6e8e));
            rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_15);
            rb_state4.var_1 = rb_state4.var_2;
            pthread_mutex_unlock(&(rb_state4.lock_27));
        }
        if ((rb_state4.var_0) == (0xd)) {
            if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_28));
            rb_state8.var_7 = 0x93291308;
            rb_state8.var_8 = 0x64334275;
            rb_state8.var_9 = (rb_state8.var_9) ^ (rb_state8.var_5);
            rb_state8.var_10 = (rb_state8.var_10) + (((((0x4a9e3b57) ^ (rb_state8.var_6)) ^ (0xedb113f4)) + (rb_state8.var_4)) + (rb_state8.var_8));
            rb_state8.var_11 = (rb_state8.var_11) ^ (((((0x44ac5107) - (rb_state8.var_9)) - (0xa99b1005)) + (rb_state8.var_7)) - (rb_state8.var_7));
            rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_8) == (0x64334275)) ? (rb_state8.var_10) : (0x5532f887));
            rb_state8.var_13 = (rb_state8.var_13) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_11) : (0x51100f74));
            rb_state8.var_14 = (rb_state8.var_14) ^ (rb_state8.var_12);
            rb_state8.var_15 = (rb_state8.var_15) ^ (((rb_state8.var_13) == (0x9b114102)) ? (rb_state8.var_14) : (0xa3eabfb));
            rb_state8.var_6 = (rb_state8.var_6) + (rb_state8.var_15);
            rb_state8.var_1 = rb_state8.var_6;
            pthread_mutex_unlock(&(rb_state8.lock_28));
        }
        if ((rb_state8.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_28));
            rb_state8.var_17 = 0x5e90f1ae;
            rb_state8.var_18 = 0x255b4232;
            rb_state8.var_19 = 0xecb2654c;
            rb_state8.var_20 = (rb_state8.var_20) - (((rb_state8.var_11) == (0x9b114102)) ? (rb_state8.var_10) : (0xf9dee0b7));
            rb_state8.var_21 = (rb_state8.var_21) ^ (((0xc181f585) - (0xae84d88b)) + (rb_state8.var_19));
            rb_state8.var_22 = (rb_state8.var_22) + (((0xd879d5b) - (rb_state8.var_18)) - (rb_state8.var_12));
            rb_state8.var_23 = (rb_state8.var_23) ^ (((rb_state8.var_17) == (0x5e90f1ae)) ? (rb_state8.var_20) : (0xe35c273a));
            rb_state8.var_24 = (rb_state8.var_24) + (((0x4e5c5bd1) - (rb_state8.var_21)) ^ (rb_state8.var_13));
            rb_state8.var_25 = (rb_state8.var_25) - (rb_state8.var_22);
            rb_state8.var_26 = (rb_state8.var_26) + (((rb_state8.var_23) == (0xf49d94e8)) ? (rb_state8.var_24) : (0xc361e3de));
            rb_state8.var_27 = (rb_state8.var_27) + (((rb_state8.var_25) == (0xc7139bf)) ? (rb_state8.var_26) : (0xd2bdba7b));
            rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_27);
            rb_state8.var_1 = rb_state8.var_16;
            pthread_mutex_unlock(&(rb_state8.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x162)) {
            rb_state11.var_6 = 0xfe628172;
            rb_state11.var_7 = (rb_state11.var_7) ^ (((((0xc0d89b33) + (rb_state11.var_3)) - (0x3fe85d3b)) - (rb_state11.var_5)) ^ (rb_state11.var_4));
            rb_state11.var_8 = (rb_state11.var_8) - ((0xf2cc2869) ^ (rb_state11.var_6));
            rb_state11.var_9 = (rb_state11.var_9) + (0x86079998);
            rb_state11.var_10 = (rb_state11.var_10) - (((((0xaae118e2) ^ (rb_state11.var_7)) + (rb_state11.var_6)) ^ (rb_state11.var_7)) ^ (rb_state11.var_8));
            rb_state11.var_11 = (rb_state11.var_11) - (((((0x61a2e371) - (rb_state11.var_8)) ^ (rb_state11.var_9)) + (rb_state11.var_9)) ^ (0x6bc13be7));
            rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_10) == (0xa0ed336b)) ? (rb_state11.var_11) : (0x4c3dab16));
            rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (100 < rb_input_size ? (uint32_t)rb_input[100] : 0x5eb9c5a);
        if ((rb_state16.var_0) == (0xffffff9d)) {
            pthread_mutex_lock(&(rb_state16.lock_32));
            rb_state16.var_3 = 0xde71e5cf;
            rb_state16.var_4 = 0xa359d634;
            rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_3);
            rb_state16.var_6 = (rb_state16.var_6) ^ (421 < rb_input_size ? (uint32_t)rb_input[421] : 0x9b378c84);
            rb_state16.var_7 = (rb_state16.var_7) + (rb_state16.var_1);
            rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_4) : (0xb8741162));
            rb_state16.var_9 = (rb_state16.var_9) ^ (((((0x308e9438) + (0xe3fba5aa)) + (rb_state16.var_6)) ^ (rb_state16.var_5)) + (0xd50c322e));
            rb_state16.var_10 = (rb_state16.var_10) - (((rb_state16.var_3) == (0xde71e5cf)) ? (rb_state16.var_7) : (0x55160459));
            rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_4) == (0xa359d634)) ? (rb_state16.var_8) : (0xafb11fe));
            rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_5) == (0xde71e5cf)) ? (rb_state16.var_9) : (0xa7d46404));
            rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_6) == (0x65)) ? (rb_state16.var_10) : (0xc1d1d810));
            rb_state16.var_14 = (rb_state16.var_14) + (rb_state16.var_11);
            rb_state16.var_15 = (rb_state16.var_15) ^ (((0xa3410a94) ^ (rb_state16.var_12)) ^ (rb_state16.var_7));
            rb_state16.var_16 = (rb_state16.var_16) ^ (rb_state16.var_13);
            rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_8) == (0xa359d634)) ? (rb_state16.var_14) : (0x2cc21f9f));
            rb_state16.var_18 = (rb_state16.var_18) + (((0x61d52f3a) + (0xe2d48d5)) + (rb_state16.var_15));
            rb_state16.var_19 = (rb_state16.var_19) ^ (((0x10f19961) ^ (rb_state16.var_16)) + (0xbe3e8bf9));
            rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_17) == (0xa359d634)) ? (rb_state16.var_18) : (0x13e57174));
            rb_state16.var_21 = (rb_state16.var_21) ^ (((rb_state16.var_19) == (0xcf30255a)) ? (rb_state16.var_20) : (0xd5c393b9));
            rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_21);
            rb_state16.var_1 = rb_state16.var_2;
            pthread_mutex_unlock(&(rb_state16.lock_32));
        }
        if ((rb_state16.var_0) == (0xffffff9d)) {
            if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                racebench_trigger(16);
            }
        }
        #endif
        m_suspended = value;
        code = MultiThreadedSyncPrimitive::suspend();
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0xa379a05b)) {
            rb_state0.var_3 = 0x79b300f1;
            rb_state0.var_4 = 0x7f82861c;
            rb_state0.var_5 = 0xda45b9c4;
            rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_2);
            rb_state0.var_7 = (rb_state0.var_7) + (((((0xe727eebb) + (rb_state0.var_4)) - (rb_state0.var_5)) - (0x42c84afb)) + (0x2dee1998));
            rb_state0.var_8 = (rb_state0.var_8) + (rb_state0.var_3);
            rb_state0.var_9 = (rb_state0.var_9) + (((((0x8d17cf73) ^ (0xc870132)) ^ (0xf97cff92)) + (rb_state0.var_7)) + (rb_state0.var_6));
            rb_state0.var_10 = (rb_state0.var_10) + (rb_state0.var_8);
            rb_state0.var_11 = (rb_state0.var_11) - (((((0x898c838) ^ (0x9189e2ac)) ^ (rb_state0.var_9)) ^ (rb_state0.var_10)) ^ (rb_state0.var_3));
            rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_11);
        }
        #endif
        code = unlock();
    }

    _INLINE void resume() {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ ((0xa379a05b) ^ (rb_state0.var_0));
        #endif
        m_suspended = false;
        MultiThreadedSyncPrimitive::resume();
    }

private:
    int m_suspended;
};

class MultiThreadedTaskQueueServer : public MultiThreadedTaskQueue, public MultiThreadedSyncPrimitive {

protected:
    MultiThreadedTaskQueueServer(int) : MultiThreadedTaskQueue(), MultiThreadedSyncPrimitive(), m_thread(NULL) {
        PRINT(this);
        m_finished_jobs = 0;
        memset(m_waitingClients, 0, sizeof(m_waitingClients));
    }

    ~MultiThreadedTaskQueueServer() {
        clean(false);
    }

    _INLINE void clean(bool force_exit_from_thread_function = true) {
        if (m_threads <= 0) {
            return;
        }

        if (force_exit_from_thread_function) {
            lock();

            int nt = m_threads;
            m_threads = -1;

            for (int i = 0; i < nt; i++) {
                m_scheduler[i].resume();
            }
            unlock();

            int i = 0;
            while (i < nt) {
                for (; i < nt; i++) {
                    if (m_scheduler[i].suspended() != -1) {

                        sched_yield();
                        break;
                    }
                }
            }
        }

        if (m_thread) {
            delete[] m_thread;
        }
        if (m_client) {
            delete[] m_client;
        }
        if (m_scheduler) {
            delete[] m_scheduler;
        }
    }

public:
    friend class MultiThreadedTaskQueue;

    using MultiThreadedSyncPrimitive::suspend;
    _INLINE static void suspend(int i) {
        assert(m_client[i] == NULL);
        m_scheduler[i].suspend();
    }

    using MultiThreadedSyncPrimitive::resume;
    using MultiThreadedSyncPrimitive::resumeAll;
    _INLINE static void resume(int i) {
        assert(m_client[i]);
        m_scheduler[i].resume();
    }

    void createThreads(int threads) {
        assert(threads >= 1);

        clean();

        typedef MultiThreadedTaskQueue *pMultiThreadedTaskQueue;
        m_threads = threads;
        m_thread = new pthread_t[threads];
        m_client = new pMultiThreadedTaskQueue[threads];
        m_scheduler = new MultiThreadedScheduler[threads];
        memset(m_client, 0, sizeof(pMultiThreadedTaskQueue) * threads);

        int res;

        for (int i = 0; i < m_threads; i++) {
            if ((res = pthread_create(&m_thread[i], NULL, &threadFunc, (void *)i)) != 0) {
                cerr << "can't create thread " << i;
                if (res == EAGAIN) {
                    cerr << " (the system does not have enough resources)" << endl;
                } else {
                    cerr << ". reason : " << res << endl;
                }
                exit(EXIT_FAILURE);
            }
        }

        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0xffffff3a)) {
            rb_state6.var_16 = 0x31897736;
            rb_state6.var_17 = 0x71edc29;
            rb_state6.var_18 = (rb_state6.var_18) - ((0x71da9af9) ^ (0x43f87c5a));
            rb_state6.var_19 = (rb_state6.var_19) - (((((0xdf5764bb) + (rb_state6.var_9)) + (rb_state6.var_8)) ^ (0x4de6ba0c)) - (rb_state6.var_17));
            rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_16) == (0x31897736)) ? (rb_state6.var_18) : (0x8baaa522));
            rb_state6.var_21 = (rb_state6.var_21) - (((((0xb2f01152) - (rb_state6.var_20)) + (0xae8e7819)) - (rb_state6.var_19)) + (rb_state6.var_10));
            rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x0)) {
            if ((rb_state8.var_1) != (0x0)) {
                if (!((rb_state8.var_1) == (rb_state8.var_16))) {
                    racebench_trigger(8);
                }
            }
        }
        #endif
        waitUntillAllThreadsAreSuspended();
    }

    _INLINE bool finished() const {
        return m_threads == -1;
    }

private:
    _INLINE void waitUntillAllThreadsAreSuspended() {
        int ns;
        do {

            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) - (0x5768c6a6);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + ((0x3d8782fd) + (0x280437de));
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + (0x3a6f094b);
            #endif
            sched_yield();
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
            #endif
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0x2d3e12db)) {
                rb_state19.var_11 = (rb_state19.var_11) + (rb_state19.var_10);
                rb_state19.var_12 = (rb_state19.var_12) ^ (((((0xc34edac6) + (rb_state19.var_11)) + (0x34e5b215)) - (rb_state19.var_9)) - (rb_state19.var_8));
                rb_state19.var_13 = (rb_state19.var_13) ^ (rb_state19.var_12);
                rb_state19.var_14 = (rb_state19.var_14) + (((((0xb5ba6539) + (rb_state19.var_13)) ^ (rb_state19.var_12)) - (rb_state19.var_11)) - (0xc38846eb));
                rb_state19.var_15 = (rb_state19.var_15) ^ (rb_state19.var_13);
                rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_14);
                rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_14) == (0x1e8f53f9)) ? (rb_state19.var_15) : (0xd25954fa));
                rb_state19.var_18 = (rb_state19.var_18) - (rb_state19.var_16);
                rb_state19.var_19 = (rb_state19.var_19) - (rb_state19.var_17);
                rb_state19.var_20 = (rb_state19.var_20) - (((0x3e01d5a9) + (0xa4b5c7b)) ^ (rb_state19.var_18));
                rb_state19.var_21 = (rb_state19.var_21) - (rb_state19.var_19);
                rb_state19.var_22 = (rb_state19.var_22) ^ (((((0x65c513e9) - (rb_state19.var_16)) - (rb_state19.var_15)) ^ (rb_state19.var_21)) + (rb_state19.var_20));
                rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_22);
            }
            #endif
            ns = 0;
            for (int i = 0; i < m_threads; i++) {
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) ^ (359 < rb_input_size ? (uint32_t)rb_input[359] : 0x26d3b9bb);
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xc1d335aa));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) + (239 < rb_input_size ? (uint32_t)rb_input[239] : 0x8b4e9e3d);
                #endif
                ns += m_scheduler[i].suspended();
            }
        } while (ns != m_threads);
    }

    static void *threadFunc(void *_id) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xb97ebf8d));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (122 < rb_input_size ? (uint32_t)rb_input[122] : 0xd84dead9);
        rb_state6.var_0 = (rb_state6.var_0) ^ ((0x209fa92) + (0x7adec244));
        if ((rb_state6.var_0) == (0xffffff3a)) {
            pthread_mutex_lock(&(rb_state6.lock_22));
            rb_state6.var_3 = 0x128c6017;
            rb_state6.var_4 = (rb_state6.var_4) + (272 < rb_input_size ? (uint32_t)rb_input[272] : 0xe75684df);
            rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_1);
            rb_state6.var_6 = (rb_state6.var_6) ^ (((0x481aa258) + (rb_state6.var_3)) ^ (rb_state6.var_3));
            rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_4) : (0xd06291c));
            rb_state6.var_8 = (rb_state6.var_8) ^ (rb_state6.var_5);
            rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_4) == (0x40)) ? (rb_state6.var_6) : (0xbb2d0365));
            rb_state6.var_10 = (rb_state6.var_10) ^ (((0xb81beab0) - (0xed5f13dd)) + (rb_state6.var_7));
            rb_state6.var_11 = (rb_state6.var_11) + (rb_state6.var_8);
            rb_state6.var_12 = (rb_state6.var_12) ^ (((0xecaa40ac) ^ (rb_state6.var_9)) + (0xc4819d00));
            rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_10) : (0x2dfc6e46));
            rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_12) : (0xe8b27ed1));
            rb_state6.var_15 = (rb_state6.var_15) ^ (((((0x36649796) + (rb_state6.var_13)) - (rb_state6.var_14)) ^ (rb_state6.var_6)) ^ (rb_state6.var_7));
            rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_15);
            rb_state6.var_1 = rb_state6.var_2;
            pthread_mutex_unlock(&(rb_state6.lock_22));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + ((0xe0a1c75f) ^ (0x46e4c46f));
        #endif
        long long int id = (long long int)_id;

        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
        if ((rb_state5.var_0) == (0x962eeb6c)) {
            rb_state5.var_7 = 0x230b5c9a;
            rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_7) : (0xbced2f43));
            rb_state5.var_9 = (rb_state5.var_9) + (((((0xf869d452) ^ (rb_state5.var_8)) + (rb_state5.var_4)) - (0x67d24c36)) + (rb_state5.var_5));
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x83174201)) {
            if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (0x830a01f0);
        rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
        #endif
        _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));

        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x6215f8e7)) {
            rb_state2.var_3 = 0x1;
        }
        if ((rb_state2.var_0) == (0x6215f8e7)) {
            pthread_mutex_lock(&(rb_state2.lock_15));
            if ((rb_state2.var_3) != (0x0)) {
                rb_state2.var_16 = !((rb_state2.var_1) == (rb_state2.var_2));
            }
            pthread_mutex_unlock(&(rb_state2.lock_15));
        }
        if ((rb_state2.var_0) == (0x6215f8e7)) {
            pthread_mutex_lock(&(rb_state2.lock_15));
            rb_state2.var_4 = 0x79277bef;
            rb_state2.var_5 = (rb_state2.var_5) + (((rb_state2.var_4) == (0x79277bef)) ? (rb_state2.var_3) : (0x91ac1672));
            rb_state2.var_6 = (rb_state2.var_6) ^ (((((0xe58da9a7) - (rb_state2.var_4)) + (rb_state2.var_2)) ^ (0x79de8daf)) + (rb_state2.var_5));
            rb_state2.var_7 = (rb_state2.var_7) - (164 < rb_input_size ? (uint32_t)rb_input[164] : 0x7f02b366);
            rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_1);
            rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_5) == (0x1)) ? (rb_state2.var_6) : (0x879f5c4a));
            rb_state2.var_10 = (rb_state2.var_10) + (((0xe3997ebd) + (rb_state2.var_7)) ^ (rb_state2.var_6));
            rb_state2.var_11 = (rb_state2.var_11) ^ (((0x409348cd) ^ (rb_state2.var_7)) - (rb_state2.var_8));
            rb_state2.var_12 = (rb_state2.var_12) ^ (((0x9bab6281) - (0x2c060a4f)) + (rb_state2.var_9));
            rb_state2.var_13 = (rb_state2.var_13) + (((((0xbf2664ab) - (0xeeff6d55)) - (rb_state2.var_10)) + (rb_state2.var_11)) - (0x99c97ebd));
            rb_state2.var_14 = (rb_state2.var_14) + (((rb_state2.var_12) == (0x59ecb81a)) ? (rb_state2.var_13) : (0xafed316d));
            rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_14);
            rb_state2.var_1 = rb_state2.var_2;
            pthread_mutex_unlock(&(rb_state2.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x962eeb6c)) {
            pthread_mutex_lock(&(rb_state5.lock_10));
            rb_state5.var_3 = 0x3f15b13;
            rb_state5.var_4 = 0xa81f92ae;
            rb_state5.var_5 = (rb_state5.var_5) - (((rb_state5.var_3) == (0x3f15b13)) ? (rb_state5.var_1) : (0xcc56883c));
            rb_state5.var_6 = (rb_state5.var_6) - (((((0xe0cc81e2) - (rb_state5.var_4)) - (rb_state5.var_2)) ^ (0x19451e5f)) + (rb_state5.var_5));
            rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_6);
            rb_state5.var_1 = rb_state5.var_2;
            pthread_mutex_unlock(&(rb_state5.lock_10));
        }
        if ((rb_state5.var_0) == (0x962eeb6c)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0xffffff9d)) {
            rb_state16.var_22 = 0x2ac3dd45;
            rb_state16.var_23 = 0xf2990b0;
            rb_state16.var_24 = (rb_state16.var_24) ^ (((((0x6ce09a2a) ^ (rb_state16.var_10)) ^ (rb_state16.var_9)) - (rb_state16.var_22)) + (0xc6e618ba));
            rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_11);
            rb_state16.var_26 = (rb_state16.var_26) + (((0x6ce4eadc) - (rb_state16.var_12)) + (rb_state16.var_23));
            rb_state16.var_27 = (rb_state16.var_27) + (((0xd178aa4c) + (rb_state16.var_24)) - (0x873f90e1));
            rb_state16.var_28 = (rb_state16.var_28) + (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_26) : (0x7837f481));
            rb_state16.var_29 = (rb_state16.var_29) - (((0x3d86f2cc) ^ (rb_state16.var_13)) - (rb_state16.var_27));
            rb_state16.var_30 = (rb_state16.var_30) - (((0x83646120) + (0x24cd5ee5)) ^ (rb_state16.var_28));
            rb_state16.var_31 = (rb_state16.var_31) - (((((0xb3c942b9) ^ (rb_state16.var_14)) ^ (rb_state16.var_29)) - (0x5be405db)) ^ (rb_state16.var_30));
            rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_31);
        }
        #endif
        m_scheduler[id].suspend();

        int seqn;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ ((0x205948f0) - (0xe2a7ee9f));
        #endif
        long long int tag = -1;
        while (!m_server.finished()) {
            MultiThreadedTaskQueue *client = MultiThreadedTaskQueue::m_client[id];

            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0x0)) {
                rb_state7.var_3 = 0x3884381f;
                rb_state7.var_4 = 0x61f91ad4;
                rb_state7.var_5 = (rb_state7.var_5) ^ (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_3) : (0x137bd7cd));
                rb_state7.var_6 = (rb_state7.var_6) - (((((0xc9335a39) + (0xaad502a9)) ^ (rb_state7.var_4)) + (rb_state7.var_2)) + (0xf55ba11f));
                rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_5) : (0x3f01a55b));
                rb_state7.var_8 = (rb_state7.var_8) - (((((0x3f8e8311) - (rb_state7.var_7)) ^ (rb_state7.var_6)) ^ (0xbece550)) - (rb_state7.var_5));
                rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_8);
            }
            #endif
            int action = 0;

            assert(client);
            if (tag != client->tag()) {
                tag = client->tag();
                m_server.lock();
                if (client->m_assigned_jobs >= client->m_threads) {
                    m_server.unlock();
                    goto getnext;
                }
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
                #endif
                seqn = client->m_assigned_jobs++;
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_11) == (0x736f4ce5)) {
                    rb_state2.var_27 = 0x257084e1;
                    rb_state2.var_28 = 0x280ecae5;
                    rb_state2.var_29 = (rb_state2.var_29) - ((0x78c7b964) + (rb_state2.var_16));
                    rb_state2.var_30 = (rb_state2.var_30) + (rb_state2.var_27);
                    rb_state2.var_31 = (rb_state2.var_31) ^ (((0x848966c5) + (rb_state2.var_28)) ^ (rb_state2.var_17));
                    rb_state2.var_32 = (rb_state2.var_32) ^ (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_14) : (0x6edd50dc));
                    rb_state2.var_33 = (rb_state2.var_33) ^ (((0x55d44cb6) + (rb_state2.var_29)) - (0xb2eaa7f2));
                    rb_state2.var_34 = (rb_state2.var_34) - (((((0x6bd91e1c) + (rb_state2.var_31)) + (0xb37b0856)) + (rb_state2.var_30)) + (rb_state2.var_19));
                    rb_state2.var_35 = (rb_state2.var_35) ^ (rb_state2.var_32);
                    rb_state2.var_36 = (rb_state2.var_36) ^ (((0xf8df9a31) ^ (rb_state2.var_33)) + (rb_state2.var_20));
                    rb_state2.var_37 = (rb_state2.var_37) - (((rb_state2.var_21) == (0x0)) ? (rb_state2.var_34) : (0x4090677a));
                    rb_state2.var_38 = (rb_state2.var_38) ^ (((rb_state2.var_35) == (0x0)) ? (rb_state2.var_36) : (0xde30f7f5));
                    rb_state2.var_39 = (rb_state2.var_39) ^ (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_37) : (0x3faef71c));
                    rb_state2.var_40 = (rb_state2.var_40) ^ (((((0x95bcda53) ^ (rb_state2.var_38)) ^ (rb_state2.var_23)) ^ (rb_state2.var_24)) - (rb_state2.var_39));
                    rb_state2.var_17 = (rb_state2.var_17) ^ (rb_state2.var_40);
                }
                #endif
                m_server.unlock();
            }
            action = client->task(seqn, (int)id);
            if (action == THREAD_EXIT) {
                break;
            }

        getnext:

            seqn = deactivateThread((int)id);
        }

        m_scheduler[id].suspend(false, -1);
        return NULL;
    }

    _INLINE static int deactivateThread(const int threadID) {

        MultiThreadedTaskQueue *client = m_client[threadID];
        assert(m_client[threadID]);
        m_client[threadID] = NULL;

        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_11) == (0x736f4ce5)) {
            pthread_mutex_lock(&(rb_state2.lock_41));
            rb_state2.var_19 = 0x54bc0541;
            rb_state2.var_20 = 0x665acfde;
            rb_state2.var_21 = 0x8c1eb80b;
            rb_state2.var_22 = (rb_state2.var_22) + (rb_state2.var_20);
            rb_state2.var_23 = (rb_state2.var_23) - (((0x74ed105f) + (rb_state2.var_21)) + (0x1319533d));
            rb_state2.var_24 = (rb_state2.var_24) ^ (((rb_state2.var_12) == (0x202bf70)) ? (rb_state2.var_19) : (0xa899640f));
            rb_state2.var_25 = (rb_state2.var_25) - (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0x33b19ede));
            rb_state2.var_26 = (rb_state2.var_26) + (((((0xace55d5) ^ (0xda834a42)) - (rb_state2.var_25)) - (rb_state2.var_24)) ^ (rb_state2.var_13));
            rb_state2.var_18 = (rb_state2.var_18) ^ (rb_state2.var_26);
            rb_state2.var_17 = rb_state2.var_18;
            pthread_mutex_unlock(&(rb_state2.lock_41));
        }
        if ((rb_state2.var_11) == (0x736f4ce5)) {
            if (!((rb_state2.var_17) == (rb_state2.var_18))) {
                racebench_trigger(2);
            }
        }
        #endif
        int code = m_server.lock();
        int seqn = -1;

        bool done = ++client->m_finished_jobs == client->m_threads;

        if (m_server.schedule(threadID)) {

            if (done) {
                m_server.resume();
            } else {
                seqn = client->m_assigned_jobs++;
            }
            code = m_server.unlock();
        } else {

            if (done) {
                m_server.resume();
            }
            m_scheduler[threadID].lock();
            code = m_server.unlock();
            m_scheduler[threadID].suspend(true);
        }
        return seqn;
    }

    _INLINE int getNextClientIndex() {

        int j = m_finished_jobs;
        for (int i = 0; i < queuesize; i++) {
            if (m_waitingClients[j]) {
                return j;
            }
            if (++j == queuesize) {
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xb0517a9f));
                #endif
                j = 0;
            }
        }
        return -1;
    }

    void addClient(MultiThreadedTaskQueue *client) {

    recheck:
        m_server.lock();
        int j = m_finished_jobs;
        for (int i = 0; i < queuesize; i++) {
            if (m_waitingClients[j] == NULL) {
                goto found;
            }
            if (++j == queuesize) {
                j = 0;
            }
        }

        m_server.unlock();

        sched_yield();
        goto recheck;

    found:

        m_waitingClients[j] = client;
        m_scheduledJobs[j] = 0;

        m_finished_jobs = j + 1;
        if (m_finished_jobs == queuesize) {
            m_finished_jobs = 0;
        }

        int nt = m_server.m_threads;

        for (int i = 0; i < nt; i++) {
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) - (0xfa419672);
            #endif
            m_scheduler[i].lock();
            if (m_scheduler[i].suspended()) {
                if (!schedule(i)) {
                    m_scheduler[i].unlock();
                    break;
                }
                m_scheduler[i].resume();
            }
            m_scheduler[i].unlock();
        }

        m_server.unlock();
    }

    bool schedule(int i) {

        if (m_client[i] == NULL) {

            #ifdef RACEBENCH_BUG_2
            rb_state2.var_8 = (rb_state2.var_8) ^ (((rb_state2.var_10) == (0x4e833e1)) ? (rb_state2.var_9) : (0xb5bc3ca9));
            #endif
            int ji = getNextClientIndex();
            if (ji == -1) {

                return false;
            }
            m_client[i] = m_waitingClients[ji];

            if (++m_scheduledJobs[ji] == m_waitingClients[ji]->m_threads) {
                m_waitingClients[ji] = NULL;
            }
        }
        return true;
    }

    pthread_t *m_thread;
    static const int queuesize = 4;
    MultiThreadedTaskQueue *m_waitingClients[queuesize];
    int m_scheduledJobs[queuesize];
};

void MultiThreadedTaskQueue::setMaxNumberOfThreads(int threads) {
    PRINT(threads);
    m_server.createThreads(threads);
}

int MultiThreadedTaskQueue::maxNumberOfThreads() {
    return m_server.numberOfThreads();
}

void MultiThreadedTaskQueue::createThreads(int threads) {
    assert(threads >= 1);
    m_threads = threads;
    m_server.lock();
    if (m_server.m_threads == 0) {

        m_server.createThreads(threads);
    }
    m_server.unlock();
}

void MultiThreadedTaskQueue::startThreads() {
    m_finished_jobs = m_assigned_jobs = 0;
    m_server.addClient(this);
}

void MultiThreadedTaskQueue::waitForAllThreads() {
    if (m_finished_jobs == m_threads) {
        return;
    }
    int code = m_server.lock();

    while (m_finished_jobs < m_threads) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0xa379a05b)) {
            rb_state0.var_2 = rb_state0.var_1;
        }
        if ((rb_state0.var_0) == (0xa379a05b)) {
            if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                racebench_trigger(0);
            }
        }
        #endif
        m_server.suspend();
    }

    code = m_server.unlock();
}

MultiThreadedTaskQueueServer MultiThreadedTaskQueue::m_server(1);
MultiThreadedTaskQueue **MultiThreadedTaskQueue::m_client = NULL;
MultiThreadedScheduler *MultiThreadedTaskQueue::m_scheduler = NULL;