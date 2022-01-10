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
            #ifdef RACEBENCH_BUG_5
            if ((rb_state5.var_3) == (0x0)) {
                rb_state5.var_10 = 0xc3480579;
                rb_state5.var_11 = 0x2ba035a0;
                rb_state5.var_12 = (rb_state5.var_12) ^ (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_8) : (0x3c29f890));
                rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_10) : (0x32869533));
                rb_state5.var_14 = (rb_state5.var_14) - (((((0xd1db8de7) + (rb_state5.var_13)) - (rb_state5.var_12)) + (rb_state5.var_10)) - (rb_state5.var_11));
                rb_state5.var_15 = (rb_state5.var_15) + (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_12) : (0xd22f39f1));
                rb_state5.var_16 = (rb_state5.var_16) ^ (((rb_state5.var_13) == (0x0)) ? (rb_state5.var_14) : (0xdb9807cc));
                rb_state5.var_17 = (rb_state5.var_17) - (rb_state5.var_15);
                rb_state5.var_18 = (rb_state5.var_18) - (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_16) : (0xae9f0022));
                rb_state5.var_19 = (rb_state5.var_19) + (rb_state5.var_17);
                rb_state5.var_20 = (rb_state5.var_20) - (((0xd20282df) ^ (rb_state5.var_16)) - (rb_state5.var_18));
                rb_state5.var_21 = (rb_state5.var_21) - (rb_state5.var_19);
                rb_state5.var_22 = (rb_state5.var_22) ^ (((0xd8806f46) - (rb_state5.var_20)) + (rb_state5.var_17));
                rb_state5.var_23 = (rb_state5.var_23) + (((0x6e0a64e7) ^ (rb_state5.var_21)) + (0xe997c4f9));
                rb_state5.var_24 = (rb_state5.var_24) - (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_22) : (0xeeff4d61));
                rb_state5.var_25 = (rb_state5.var_25) ^ (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_24) : (0x656e21b4));
                rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_25);
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - (25 < rb_input_size ? (uint32_t)rb_input[25] : 0xc8419046);
            if ((rb_state7.var_3) == (0xfffffea7)) {
                rb_state7.var_4 = rb_state7.var_1;
            }
            if ((rb_state7.var_3) == (0xfffffea7)) {
                if (!((rb_state7.var_1) == (rb_state7.var_4))) {
                    racebench_trigger(7);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_1) == (0xe2c57f6f)) {
                rb_state12.var_5 = 0x66c45201;
                rb_state12.var_6 = 0xfc1e867b;
                rb_state12.var_7 = (rb_state12.var_7) - (((((0x3d001c09) - (rb_state12.var_5)) ^ (rb_state12.var_5)) + (rb_state12.var_4)) ^ (rb_state12.var_6));
                rb_state12.var_8 = (rb_state12.var_8) - (rb_state12.var_6);
                rb_state12.var_9 = (rb_state12.var_9) ^ (((((0x76cee45e) - (0xe8b7b322)) + (rb_state12.var_7)) ^ (rb_state12.var_7)) ^ (rb_state12.var_8));
                rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_9);
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_3) == (0xb3a21cc6)) {
                rb_state13.var_7 = 0xff2eb443;
                rb_state13.var_8 = 0xf3b7c4df;
                rb_state13.var_9 = (rb_state13.var_9) ^ (((((0xc968122d) + (rb_state13.var_8)) - (0x67bf54cb)) - (rb_state13.var_5)) - (rb_state13.var_6));
                rb_state13.var_10 = (rb_state13.var_10) - (((((0x83f43136) ^ (rb_state13.var_7)) + (rb_state13.var_7)) ^ (0x18ed148c)) + (rb_state13.var_9));
                rb_state13.var_6 = (rb_state13.var_6) ^ (rb_state13.var_10);
                rb_state13.var_4 = rb_state13.var_6;
            }
            if ((rb_state13.var_3) == (0xb3a21cc6)) {
                if ((rb_state13.var_4) != (0x0)) {
                    if (!((rb_state13.var_4) == (rb_state13.var_11))) {
                        racebench_trigger(13);
                    }
                }
            }
            if ((rb_state13.var_3) == (0xb3a21cc6)) {
                pthread_mutex_lock(&(rb_state13.lock_21));
                rb_state13.var_12 = 0x6687e337;
                rb_state13.var_13 = 0xddc4f6a5;
                rb_state13.var_14 = (rb_state13.var_14) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_12) : (0x33203510));
                rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_13);
                rb_state13.var_16 = (rb_state13.var_16) - (((((0x887def1e) ^ (0xbfbead69)) + (rb_state13.var_14)) - (rb_state13.var_8)) + (0x5551cf94));
                rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_15) : (0xb84f7fbc));
                rb_state13.var_18 = (rb_state13.var_18) + (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_16) : (0xf823902a));
                rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_17);
                rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_19) : (0x7706a068));
                rb_state13.var_11 = (rb_state13.var_11) - (rb_state13.var_20);
                rb_state13.var_4 = rb_state13.var_11;
                pthread_mutex_unlock(&(rb_state13.lock_21));
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_3 = (rb_state18.var_3) + ((0xf5b6bc93) + (rb_state18.var_2));
            #endif
            code = lock();
        }
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_2);
        if ((rb_state4.var_3) == (0x1a8)) {
            pthread_mutex_lock(&(rb_state4.lock_20));
            rb_state4.var_6 = 0xdda1e9b;
            rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_5) : (0x15d56843));
            rb_state4.var_8 = (rb_state4.var_8) + (((((0xa159999e) + (rb_state4.var_4)) - (rb_state4.var_6)) + (0x13059ec3)) + (0xfd05e673));
            rb_state4.var_9 = (rb_state4.var_9) + (((0xa712a11e) - (rb_state4.var_7)) - (rb_state4.var_7));
            rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_8) : (0xa8eb3b48));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((0x7bda27f7) - (rb_state4.var_9)) + (rb_state4.var_9));
            rb_state4.var_12 = (rb_state4.var_12) + (((((0x408c1f59) + (rb_state4.var_10)) - (rb_state4.var_11)) + (rb_state4.var_10)) - (0xb139dc9a));
            rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_12);
            rb_state4.var_4 = rb_state4.var_5;
            pthread_mutex_unlock(&(rb_state4.lock_20));
        }
        if ((rb_state4.var_3) == (0x1a8)) {
            rb_state4.var_13 = 0xd565edd9;
            rb_state4.var_14 = 0xc652b639;
            rb_state4.var_15 = (rb_state4.var_15) + (((((0x5174db5) - (rb_state4.var_13)) + (rb_state4.var_11)) ^ (0xcce00938)) ^ (rb_state4.var_12));
            rb_state4.var_16 = (rb_state4.var_16) - (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_14) : (0x13477d23));
            rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_15) : (0x27dfdca0));
            rb_state4.var_18 = (rb_state4.var_18) - (((0x3b5a93cb) ^ (rb_state4.var_16)) + (rb_state4.var_15));
            rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_18) : (0x4554a7d6));
            rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_19);
        }
        if ((rb_state4.var_3) == (0x1a8)) {
            pthread_mutex_lock(&(rb_state4.lock_20));
            if (!((rb_state4.var_4) == (rb_state4.var_5))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_20));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state5.lock_26));
            rb_state5.var_6 = 0xba46cbf;
            rb_state5.var_7 = (rb_state5.var_7) + (rb_state5.var_4);
            rb_state5.var_8 = (rb_state5.var_8) ^ (((((0xaca3a12f) ^ (rb_state5.var_6)) ^ (rb_state5.var_7)) ^ (rb_state5.var_6)) + (rb_state5.var_5));
            rb_state5.var_9 = (rb_state5.var_9) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_8) : (0x7f4abf9d));
            rb_state5.var_5 = (rb_state5.var_5) - (rb_state5.var_9);
            rb_state5.var_4 = rb_state5.var_5;
            pthread_mutex_unlock(&(rb_state5.lock_26));
        }
        if ((rb_state5.var_3) == (0x0)) {
            if (!((rb_state5.var_4) == (rb_state5.var_5))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_0);
        #endif
        m_suspended = value;
        code = MultiThreadedSyncPrimitive::suspend();
        code = unlock();
    }

    _INLINE void resume() {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) - (0xd1d96a86);
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

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_0);
        if ((rb_state1.var_1) == (0x0)) {
            rb_state1.var_8 = 0x94b8064d;
            rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_8) == (0x94b8064d)) ? (rb_state1.var_7) : (0x8cd06c6b));
            rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_8) : (0x923c6d86));
            rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x4311dc8f));
            rb_state1.var_7 = (rb_state1.var_7) - (rb_state1.var_11);
            rb_state1.var_4 = rb_state1.var_7;
        }
        if ((rb_state1.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_18));
            rb_state1.var_13 = 0xa8416c6;
            rb_state1.var_14 = 0x4841b785;
            rb_state1.var_15 = (rb_state1.var_15) ^ (rb_state1.var_9);
            rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_14) == (0x4841b785)) ? (rb_state1.var_13) : (0x314db38b));
            rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_16) : (0x7424e32b));
            rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_17);
            rb_state1.var_4 = rb_state1.var_12;
            pthread_mutex_unlock(&(rb_state1.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + ((0x85946e30) - (0xc071c626));
        rb_state10.var_3 = (rb_state10.var_3) ^ (4 < rb_input_size ? (uint32_t)rb_input[4] : 0xb45f6e95);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) - (0x3e54ace6);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (0xeb202e6d);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) + (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0xdc3a14a8));
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

            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) + (0xd1b50a80);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) - ((0x3e019e7d) - (rb_state7.var_0));
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_3 = (rb_state9.var_3) - (0x97badb54);
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_1);
            rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_1) == (0xc1ab531a)) ? (rb_state13.var_2) : (0x6b19b5ea));
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_3 = (rb_state16.var_3) + (289 < rb_input_size ? (uint32_t)rb_input[289] : 0x53049dad);
            rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_3);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0xfd637c0b));
            #endif
            sched_yield();
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_2);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + ((0x651dac95) - (0x275e93d7));
            if (!((rb_state9.var_4) == (rb_state9.var_28))) {
                rb_state9.var_51 = 0x50f903df;
                rb_state9.var_52 = 0xbf9d88cc;
                rb_state9.var_53 = 0x7d853c51;
                rb_state9.var_54 = (rb_state9.var_54) - (rb_state9.var_51);
                rb_state9.var_55 = (rb_state9.var_55) ^ (((rb_state9.var_27) == (0x8ade7a4b)) ? (rb_state9.var_53) : (0xcced80b2));
                rb_state9.var_56 = (rb_state9.var_56) - (rb_state9.var_52);
                rb_state9.var_57 = (rb_state9.var_57) + (((rb_state9.var_54) == (0xaf06fc21)) ? (rb_state9.var_55) : (0x6eea4e78));
                rb_state9.var_58 = (rb_state9.var_58) - (((rb_state9.var_28) == (0x0)) ? (rb_state9.var_56) : (0xb0ce03aa));
                rb_state9.var_59 = (rb_state9.var_59) - (rb_state9.var_57);
                rb_state9.var_60 = (rb_state9.var_60) ^ (((((0xa5077aec) + (rb_state9.var_59)) - (0x2b1c58cb)) ^ (0xc4277dc1)) - (rb_state9.var_58));
                rb_state9.var_41 = (rb_state9.var_41) ^ (rb_state9.var_60);
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_2) == (0xe2d3fec)) ? (rb_state12.var_1) : (0xd8ec7702));
            rb_state12.var_3 = (rb_state12.var_3) ^ (119 < rb_input_size ? (uint32_t)rb_input[119] : 0xd6b80a02);
            rb_state12.var_3 = (rb_state12.var_3) ^ ((0x428b1f29) ^ (0xadbdd242));
            #endif
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_3 = (rb_state13.var_3) ^ ((0x7221ffb3) - (0x12de0269));
            #endif
            #ifdef RACEBENCH_BUG_18
            if ((rb_state18.var_0) == (0xd74a9520)) {
                pthread_mutex_lock(&(rb_state18.lock_27));
                rb_state18.var_6 = 0x5ba7b307;
                rb_state18.var_7 = 0x3c3b394f;
                rb_state18.var_8 = (rb_state18.var_8) ^ (((0x55a85e88) ^ (rb_state18.var_4)) ^ (rb_state18.var_5));
                rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_6) == (0x5ba7b307)) ? (rb_state18.var_7) : (0x268a855));
                rb_state18.var_10 = (rb_state18.var_10) + (((rb_state18.var_6) == (0x5ba7b307)) ? (rb_state18.var_8) : (0xe0eb2dd6));
                rb_state18.var_11 = (rb_state18.var_11) - (rb_state18.var_9);
                rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_7) == (0x3c3b394f)) ? (rb_state18.var_10) : (0x62e8f79d));
                rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_8) == (0x55a85e88)) ? (rb_state18.var_11) : (0x20b7b4fb));
                rb_state18.var_14 = (rb_state18.var_14) + (rb_state18.var_12);
                rb_state18.var_15 = (rb_state18.var_15) ^ (((rb_state18.var_13) == (0xc3c4c6b1)) ? (rb_state18.var_14) : (0x460b516f));
                rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_15);
                rb_state18.var_4 = rb_state18.var_5;
                pthread_mutex_unlock(&(rb_state18.lock_27));
            }
            if ((rb_state18.var_0) == (0xd74a9520)) {
                pthread_mutex_lock(&(rb_state18.lock_27));
                if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                    racebench_trigger(18);
                }
                pthread_mutex_unlock(&(rb_state18.lock_27));
            }
            #endif
            ns = 0;
            for (int i = 0; i < m_threads; i++) {
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_0);
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_3);
                rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_2) == (0x4b60629c)) ? (rb_state4.var_1) : (0xfa684114));
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_2 = (rb_state5.var_2) + ((0xe95a0fb9) ^ (0x1383a2db));
                rb_state5.var_2 = (rb_state5.var_2) ^ ((0x838d0332) ^ (rb_state5.var_1));
                rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_1);
                rb_state5.var_3 = (rb_state5.var_3) ^ (229 < rb_input_size ? (uint32_t)rb_input[229] : 0xa59c2a7b);
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) - ((0x751ee46a) ^ (0xfdf1b074));
                #endif
                #ifdef RACEBENCH_BUG_7
                rb_state7.var_2 = (rb_state7.var_2) ^ (rb_state7.var_2);
                rb_state7.var_3 = (rb_state7.var_3) - (281 < rb_input_size ? (uint32_t)rb_input[281] : 0x2fac696);
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) ^ ((0x25a4b257) + (0x96c14ed0));
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_2) == (0xb1b9f894)) ? (rb_state10.var_1) : (0xbc929897));
                #endif
                ns += m_scheduler[i].suspended();
            }
        } while (ns != m_threads);
    }

    static void *threadFunc(void *_id) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + ((0x1dc65db) ^ (rb_state0.var_0));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - ((0x853ac381) - (0x7719bd9c));
        rb_state1.var_0 = (rb_state1.var_0) ^ ((0xefaeaaed) - (rb_state1.var_1));
        rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x5f591b32));
        rb_state1.var_3 = (rb_state1.var_3) ^ ((0x17bdd8f1) ^ (0x28834706));
        rb_state1.var_3 = (rb_state1.var_3) - ((0xf1afdeee) + (rb_state1.var_3));
        if ((rb_state1.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_18));
            if ((rb_state1.var_4) != (0x0)) {
                if (!((rb_state1.var_4) == (rb_state1.var_12))) {
                    racebench_trigger(1);
                }
            }
            pthread_mutex_unlock(&(rb_state1.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (0xcf728676);
        rb_state5.var_0 = (rb_state5.var_0) + (0xa9d3ccc1);
        rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0xda61464b)) ? (rb_state5.var_0) : (0xebd09289));
        rb_state5.var_1 = (rb_state5.var_1) - ((0x10efb471) + (rb_state5.var_1));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (0x9ff7a752);
        rb_state7.var_1 = (rb_state7.var_1) + (0x4e261b21);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xda47ae10));
        rb_state9.var_3 = (rb_state9.var_3) ^ ((0x4b59d891) ^ (rb_state9.var_2));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (23 < rb_input_size ? (uint32_t)rb_input[23] : 0x3c150da2);
        rb_state10.var_1 = (rb_state10.var_1) ^ (299 < rb_input_size ? (uint32_t)rb_input[299] : 0x3d22577);
        rb_state10.var_3 = (rb_state10.var_3) ^ (0xe3d275a5);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (0xe2d3fec);
        rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_0);
        rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_1) : (0x1d3580ad));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (351 < rb_input_size ? (uint32_t)rb_input[351] : 0x53bde416);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
        if ((rb_state16.var_2) == (0xeb202e04)) {
            pthread_mutex_lock(&(rb_state16.lock_30));
            rb_state16.var_6 = 0x6e2eebed;
            rb_state16.var_7 = (rb_state16.var_7) - (((((0xe5136d71) + (0xd0e815e)) - (rb_state16.var_6)) - (rb_state16.var_6)) - (rb_state16.var_5));
            rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_7) : (0x691a6a0c));
            rb_state16.var_5 = (rb_state16.var_5) - (rb_state16.var_8);
            rb_state16.var_4 = rb_state16.var_5;
            pthread_mutex_unlock(&(rb_state16.lock_30));
        }
        if ((rb_state16.var_2) == (0xeb202e04)) {
            if (!((rb_state16.var_4) == (rb_state16.var_5))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (134 < rb_input_size ? (uint32_t)rb_input[134] : 0x65e9b979);
        rb_state18.var_2 = (rb_state18.var_2) ^ ((0x2cabc25f) - (0x7edecc40));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_0);
        rb_state19.var_1 = (rb_state19.var_1) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0x91e6dea9));
        rb_state19.var_2 = (rb_state19.var_2) + (107 < rb_input_size ? (uint32_t)rb_input[107] : 0x3afccf4e);
        rb_state19.var_3 = (rb_state19.var_3) ^ (0xa2e3814e);
        #endif
        long long int id = (long long int)_id;

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (0xf90c1776);
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_1);
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
        rb_state0.var_1 = (rb_state0.var_1) - ((0xbe7fc2b5) + (0x268e86ad));
        rb_state0.var_2 = (rb_state0.var_2) ^ ((0x85111374) ^ (rb_state0.var_2));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (77 < rb_input_size ? (uint32_t)rb_input[77] : 0x8738c0ac);
        rb_state1.var_1 = (rb_state1.var_1) ^ ((0x217cd4dc) ^ (0xb52184ba));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (298 < rb_input_size ? (uint32_t)rb_input[298] : 0xe2bc4d45);
        rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_1) : (0xc3cacb65));
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_3) == (0xfffffea7)) {
            rb_state7.var_5 = 0x262e6f02;
            rb_state7.var_6 = 0x8cfcd324;
            rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0xd87b8a5c));
            rb_state7.var_8 = (rb_state7.var_8) ^ (((((0xee446ef8) ^ (0xed774fa8)) + (rb_state7.var_5)) - (rb_state7.var_6)) - (0x3a85c7d8));
            rb_state7.var_9 = (rb_state7.var_9) ^ (((((0xe8286692) - (rb_state7.var_7)) - (0xc6b7f558)) - (0x86c961b1)) ^ (rb_state7.var_8));
            rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_1) : (0x9d0e51fd));
        rb_state9.var_2 = (rb_state9.var_2) + (0x4a10e344);
        rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_2);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - ((0x1a49eab) - (rb_state10.var_1));
        rb_state10.var_2 = (rb_state10.var_2) - (0x5bcd8fd2);
        if ((rb_state10.var_3) == (0xd)) {
            rb_state10.var_16 = 0xe555bba5;
            rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_16) : (0x42ef12d6));
            rb_state10.var_18 = (rb_state10.var_18) ^ (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_17) : (0xd71c47e1));
            rb_state10.var_4 = (rb_state10.var_4) ^ (rb_state10.var_18);
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (374 < rb_input_size ? (uint32_t)rb_input[374] : 0xebc20e9d);
        rb_state12.var_3 = (rb_state12.var_3) ^ (0xd9e4d26f);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - ((0x163089d2) + (0x139eca76));
        rb_state13.var_3 = (rb_state13.var_3) - ((0xbe1b48e3) - (0xb953c146));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_0);
        rb_state15.var_1 = (rb_state15.var_1) - (190 < rb_input_size ? (uint32_t)rb_input[190] : 0x66182f5c);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (400 < rb_input_size ? (uint32_t)rb_input[400] : 0x740589f4);
        rb_state16.var_1 = (rb_state16.var_1) - ((0x6495385) + (rb_state16.var_1));
        rb_state16.var_1 = (rb_state16.var_1) + ((0x1fa9cbd2) ^ (rb_state16.var_0));
        rb_state16.var_1 = (rb_state16.var_1) ^ ((0xcfd4d7c0) ^ (0x4c4b46b1));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - ((0x4a2d5ae2) ^ (rb_state18.var_0));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - ((0xb88dc7b1) ^ (0xcad56a8e));
        if ((rb_state19.var_3) == (0xa2e3814e)) {
            rb_state19.var_17 = 0x3a38678f;
            rb_state19.var_18 = 0x448dba94;
            rb_state19.var_19 = (rb_state19.var_19) + (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_9) : (0xe066749c));
            rb_state19.var_20 = (rb_state19.var_20) + (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_17) : (0x1b2c4d78));
            rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_19) : (0xab4ef285));
            rb_state19.var_22 = (rb_state19.var_22) + (((0x46f51376) + (0xebacf0b2)) + (rb_state19.var_20));
            rb_state19.var_23 = (rb_state19.var_23) + (((0x610c6b45) + (rb_state19.var_12)) ^ (rb_state19.var_21));
            rb_state19.var_24 = (rb_state19.var_24) ^ (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_23) : (0x464d72d1));
            rb_state19.var_16 = (rb_state19.var_16) ^ (rb_state19.var_24);
            rb_state19.var_4 = rb_state19.var_16;
        }
        if ((rb_state19.var_3) == (0xa2e3814e)) {
            pthread_mutex_lock(&(rb_state19.lock_38));
            rb_state19.var_26 = 0xf3f29244;
            rb_state19.var_27 = 0xb38169b5;
            rb_state19.var_28 = (rb_state19.var_28) ^ (0x10b6fdc7);
            rb_state19.var_29 = (rb_state19.var_29) ^ (rb_state19.var_27);
            rb_state19.var_30 = (rb_state19.var_30) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_26) : (0x719dd005));
            rb_state19.var_31 = (rb_state19.var_31) - (rb_state19.var_13);
            rb_state19.var_32 = (rb_state19.var_32) ^ (((0xf9d0ba55) + (0xb3fc5b30)) - (rb_state19.var_28));
            rb_state19.var_33 = (rb_state19.var_33) ^ (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_29) : (0x227a9a10));
            rb_state19.var_34 = (rb_state19.var_34) ^ (((rb_state19.var_30) == (0x0)) ? (rb_state19.var_31) : (0x4373a0d1));
            rb_state19.var_35 = (rb_state19.var_35) - (((0x3b8dfcd) ^ (rb_state19.var_32)) + (0xd9c6ec1b));
            rb_state19.var_36 = (rb_state19.var_36) ^ (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0x614b63aa));
            rb_state19.var_37 = (rb_state19.var_37) + (((((0x77e5e864) + (rb_state19.var_16)) ^ (rb_state19.var_35)) ^ (rb_state19.var_36)) ^ (0x889d2f84));
            rb_state19.var_25 = (rb_state19.var_25) ^ (rb_state19.var_37);
            rb_state19.var_4 = rb_state19.var_25;
            pthread_mutex_unlock(&(rb_state19.lock_38));
        }
        #endif
        _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
        if ((rb_state0.var_2) == (0x0)) {
            rb_state0.var_3 = rb_state0.var_0;
        }
        if ((rb_state0.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state0.lock_8));
            rb_state0.var_4 = 0x6f765f87;
            rb_state0.var_5 = 0xba9935aa;
            rb_state0.var_6 = (rb_state0.var_6) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_3) : (0x73fba756));
            rb_state0.var_7 = (rb_state0.var_7) - (((((0x52a6378f) - (0x4351d462)) + (0x6d7bf26a)) + (rb_state0.var_4)) ^ (rb_state0.var_6));
            rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_7);
            pthread_mutex_unlock(&(rb_state0.lock_8));
        }
        if ((rb_state0.var_2) == (0x0)) {
            if (!((rb_state0.var_0) == (rb_state0.var_3))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0x9b6e3616);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) ^ (31 < rb_input_size ? (uint32_t)rb_input[31] : 0xe72ed964);
        rb_state4.var_0 = (rb_state4.var_0) - (315 < rb_input_size ? (uint32_t)rb_input[315] : 0x468a6e38);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) - (403 < rb_input_size ? (uint32_t)rb_input[403] : 0x37a3f52f);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_2);
        if ((rb_state9.var_3) == (0x65aff7cc)) {
            pthread_mutex_lock(&(rb_state9.lock_40));
            rb_state9.var_17 = 0x6e9b31b8;
            rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_12) : (0x715d723b));
            rb_state9.var_19 = (rb_state9.var_19) - (((((0x2eaa913b) + (rb_state9.var_17)) - (rb_state9.var_10)) ^ (0x279f649)) + (0xd70cc760));
            rb_state9.var_20 = (rb_state9.var_20) ^ (((0x6da20138) - (rb_state9.var_11)) - (rb_state9.var_14));
            rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_18) : (0xcb34b0de));
            rb_state9.var_22 = (rb_state9.var_22) ^ (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_19) : (0xb123afe3));
            rb_state9.var_23 = (rb_state9.var_23) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_20) : (0x19503387));
            rb_state9.var_24 = (rb_state9.var_24) - (rb_state9.var_21);
            rb_state9.var_25 = (rb_state9.var_25) - (((0xf0898cc5) + (0x61e63379)) ^ (rb_state9.var_22));
            rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0x5e38bb6e));
            rb_state9.var_27 = (rb_state9.var_27) - (((((0x86bd3372) ^ (0x4dbcbe05)) ^ (rb_state9.var_18)) + (rb_state9.var_26)) - (rb_state9.var_25));
            rb_state9.var_16 = (rb_state9.var_16) ^ (rb_state9.var_27);
            rb_state9.var_4 = rb_state9.var_16;
            pthread_mutex_unlock(&(rb_state9.lock_40));
        }
        if ((rb_state9.var_3) == (0x65aff7cc)) {
            if ((rb_state9.var_4) != (0x0)) {
                if (!((rb_state9.var_4) == (rb_state9.var_28))) {
                    pthread_mutex_lock(&(rb_state9.lock_61));
                    rb_state9.var_43 = 0x551de470;
                    rb_state9.var_44 = 0x1a2d9636;
                    rb_state9.var_45 = 0x1e532dd1;
                    rb_state9.var_46 = (rb_state9.var_46) + (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_44) : (0xd9a6534));
                    rb_state9.var_47 = (rb_state9.var_47) + (((0x5f3b87e0) - (0x5e4434da)) - (rb_state9.var_45));
                    rb_state9.var_48 = (rb_state9.var_48) - (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_43) : (0x2c116ff1));
                    rb_state9.var_49 = (rb_state9.var_49) ^ (((rb_state9.var_46) == (0x0)) ? (rb_state9.var_47) : (0x663cb5f1));
                    rb_state9.var_50 = (rb_state9.var_50) ^ (((rb_state9.var_48) == (0x0)) ? (rb_state9.var_49) : (0x39c576f5));
                    rb_state9.var_42 = (rb_state9.var_42) + (rb_state9.var_50);
                    rb_state9.var_41 = rb_state9.var_42;
                    pthread_mutex_unlock(&(rb_state9.lock_61));
                }
                if (!((rb_state9.var_4) == (rb_state9.var_28))) {
                    if (!((rb_state9.var_41) == (rb_state9.var_42))) {
                        racebench_trigger(9);
                    }
                }
            }
        }
        if ((rb_state9.var_3) == (0x65aff7cc)) {
            pthread_mutex_lock(&(rb_state9.lock_40));
            rb_state9.var_29 = 0xdc2f84c3;
            rb_state9.var_30 = 0x6605136;
            rb_state9.var_31 = 0xaa5bcb76;
            rb_state9.var_32 = (rb_state9.var_32) + (((0x496f569f) + (rb_state9.var_31)) - (rb_state9.var_19));
            rb_state9.var_33 = (rb_state9.var_33) + (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_29) : (0x629f4218));
            rb_state9.var_34 = (rb_state9.var_34) + (((0xde2333db) - (rb_state9.var_30)) + (rb_state9.var_21));
            rb_state9.var_35 = (rb_state9.var_35) ^ (((((0xcd8d8aef) + (rb_state9.var_22)) - (rb_state9.var_32)) ^ (rb_state9.var_33)) - (rb_state9.var_23));
            rb_state9.var_36 = (rb_state9.var_36) - (((0x1bfdc6d4) - (0xe979e7e4)) - (rb_state9.var_34));
            rb_state9.var_37 = (rb_state9.var_37) + (((0x5d63da5f) - (rb_state9.var_24)) - (rb_state9.var_35));
            rb_state9.var_38 = (rb_state9.var_38) + (((0x222ff009) - (rb_state9.var_36)) ^ (0x8d63ce01));
            rb_state9.var_39 = (rb_state9.var_39) - (((rb_state9.var_37) == (0x0)) ? (rb_state9.var_38) : (0x6707cf4d));
            rb_state9.var_28 = (rb_state9.var_28) - (rb_state9.var_39);
            rb_state9.var_4 = rb_state9.var_28;
            pthread_mutex_unlock(&(rb_state9.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_3) == (0xd)) {
            pthread_mutex_lock(&(rb_state10.lock_19));
            rb_state10.var_6 = 0xa985a756;
            rb_state10.var_7 = (rb_state10.var_7) - (rb_state10.var_4);
            rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_6) : (0xdd64e24));
            rb_state10.var_9 = (rb_state10.var_9) + (rb_state10.var_5);
            rb_state10.var_10 = (rb_state10.var_10) ^ (rb_state10.var_7);
            rb_state10.var_11 = (rb_state10.var_11) - (((((0x59d42e7f) + (0xc6641c3)) - (rb_state10.var_9)) - (rb_state10.var_8)) - (rb_state10.var_7));
            rb_state10.var_12 = (rb_state10.var_12) + (((0xca8e0607) - (rb_state10.var_10)) - (rb_state10.var_8));
            rb_state10.var_13 = (rb_state10.var_13) - (((0x9ea84c9) + (rb_state10.var_11)) + (rb_state10.var_9));
            rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_12) : (0x62fbff53));
            rb_state10.var_15 = (rb_state10.var_15) + (((((0x97591dd) + (rb_state10.var_14)) ^ (rb_state10.var_11)) ^ (rb_state10.var_13)) - (0x8a03eb82));
            rb_state10.var_5 = (rb_state10.var_5) + (rb_state10.var_15);
            rb_state10.var_4 = rb_state10.var_5;
            pthread_mutex_unlock(&(rb_state10.lock_19));
        }
        if ((rb_state10.var_3) == (0xd)) {
            if (!((rb_state10.var_4) == (rb_state10.var_5))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_1) == (0xe2c57f6f)) {
            rb_state12.var_4 = rb_state12.var_2;
        }
        if ((rb_state12.var_1) == (0xe2c57f6f)) {
            if (!((rb_state12.var_2) == (rb_state12.var_4))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) - (97 < rb_input_size ? (uint32_t)rb_input[97] : 0x612d0a23);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_2) == (0xeb202e04)) {
            rb_state16.var_9 = 0x72dc0830;
            rb_state16.var_10 = 0xdb5e485b;
            rb_state16.var_11 = (rb_state16.var_11) - (rb_state16.var_8);
            rb_state16.var_12 = (rb_state16.var_12) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0xf26b96f3));
            rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_7) : (0x5ce8f25c));
            rb_state16.var_14 = (rb_state16.var_14) - (rb_state16.var_9);
            rb_state16.var_15 = (rb_state16.var_15) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_11) : (0x226dc1d));
            rb_state16.var_16 = (rb_state16.var_16) ^ (((0x94ec2367) ^ (0x65d56e6f)) ^ (rb_state16.var_12));
            rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_14) : (0x94f10e5d));
            rb_state16.var_18 = (rb_state16.var_18) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_15) : (0x7a02b9e2));
            rb_state16.var_19 = (rb_state16.var_19) ^ (((0x98756271) - (rb_state16.var_16)) ^ (0xf1f7803d));
            rb_state16.var_20 = (rb_state16.var_20) + (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_18) : (0xf4a53fb7));
            rb_state16.var_21 = (rb_state16.var_21) - (rb_state16.var_19);
            rb_state16.var_22 = (rb_state16.var_22) + (((0x30b18673) + (rb_state16.var_13)) - (rb_state16.var_20));
            rb_state16.var_23 = (rb_state16.var_23) ^ (((0x7b290576) - (0xe3ead351)) - (rb_state16.var_21));
            rb_state16.var_24 = (rb_state16.var_24) ^ (((0x45096aa7) ^ (rb_state16.var_22)) - (rb_state16.var_14));
            rb_state16.var_25 = (rb_state16.var_25) - (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_23) : (0x8d8d2687));
            rb_state16.var_26 = (rb_state16.var_26) - (rb_state16.var_24);
            rb_state16.var_27 = (rb_state16.var_27) ^ (rb_state16.var_25);
            rb_state16.var_28 = (rb_state16.var_28) ^ (rb_state16.var_26);
            rb_state16.var_29 = (rb_state16.var_29) ^ (((((0x2d6ef03e) + (rb_state16.var_27)) - (rb_state16.var_16)) ^ (rb_state16.var_28)) + (0x77effb8a));
            rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_29);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (346 < rb_input_size ? (uint32_t)rb_input[346] : 0xeb8dd3b);
        rb_state18.var_3 = (rb_state18.var_3) - (378 < rb_input_size ? (uint32_t)rb_input[378] : 0x83bdc256);
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_3);
        if ((rb_state18.var_0) == (0xd74a9520)) {
            rb_state18.var_16 = 0xf966ca63;
            rb_state18.var_17 = 0x70a2a1a7;
            rb_state18.var_18 = 0x57640f9b;
            rb_state18.var_19 = (rb_state18.var_19) ^ (rb_state18.var_9);
            rb_state18.var_20 = (rb_state18.var_20) - (((((0xd625e966) ^ (0xe38e7345)) + (rb_state18.var_17)) ^ (0x56deddc8)) - (rb_state18.var_16));
            rb_state18.var_21 = (rb_state18.var_21) - (((0xa1f52e63) ^ (0x3b8467e)) + (rb_state18.var_18));
            rb_state18.var_22 = (rb_state18.var_22) ^ (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_19) : (0x459cdc60));
            rb_state18.var_23 = (rb_state18.var_23) + (((0x9479a76d) - (rb_state18.var_11)) + (rb_state18.var_20));
            rb_state18.var_24 = (rb_state18.var_24) ^ (rb_state18.var_21);
            rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_23) : (0xba042b0e));
            rb_state18.var_26 = (rb_state18.var_26) - (((((0x6acaa5ce) ^ (rb_state18.var_24)) - (rb_state18.var_12)) + (rb_state18.var_25)) - (0x95bad095));
            rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_26);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) - (373 < rb_input_size ? (uint32_t)rb_input[373] : 0x309d735);
        if ((rb_state19.var_3) == (0xa2e3814e)) {
            if ((rb_state19.var_4) != (0x0)) {
                if (!((rb_state19.var_4) == (rb_state19.var_25))) {
                    racebench_trigger(19);
                }
            }
        }
        #endif
        m_scheduler[id].suspend();

        int seqn;
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + ((0xc1a100ae) ^ (0x17cd39dc));
        rb_state11.var_0 = (rb_state11.var_0) ^ (0x6c8e6f67);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) ^ ((0xc2f340ca) - (0xc6ee95e9));
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_1) == (0xec527da0)) {
            pthread_mutex_lock(&(rb_state15.lock_6));
            rb_state15.var_5 = (rb_state15.var_5) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x90d0e8b3));
            rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_5);
            pthread_mutex_unlock(&(rb_state15.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + (296 < rb_input_size ? (uint32_t)rb_input[296] : 0xb26e6993);
        #endif
        long long int tag = -1;
        while (!m_server.finished()) {
            MultiThreadedTaskQueue *client = MultiThreadedTaskQueue::m_client[id];

            int action = 0;

            assert(client);
            if (tag != client->tag()) {
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x464fdd7e));
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) ^ (0xb1939293);
                #endif
                tag = client->tag();
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_1) == (0x2dc34826)) {
                    rb_state3.var_5 = 0x1100054b;
                    rb_state3.var_6 = 0xd8c7889c;
                    rb_state3.var_7 = (rb_state3.var_7) - (((0x1ff07cf4) ^ (rb_state3.var_4)) - (0x1f7e4e90));
                    rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_5) : (0x6f6e5aba));
                    rb_state3.var_9 = (rb_state3.var_9) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0x9f7f4d5));
                    rb_state3.var_10 = (rb_state3.var_10) + (((0xa17a3d75) - (rb_state3.var_7)) ^ (rb_state3.var_7));
                    rb_state3.var_11 = (rb_state3.var_11) + (((((0xd4b3508e) + (rb_state3.var_8)) - (rb_state3.var_8)) ^ (rb_state3.var_9)) ^ (0x528f0b4e));
                    rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_10) : (0x75fbba04));
                    rb_state3.var_13 = (rb_state3.var_13) + (((0xeb7464f6) ^ (rb_state3.var_10)) ^ (rb_state3.var_11));
                    rb_state3.var_14 = (rb_state3.var_14) - (((((0xf4ae79b4) + (rb_state3.var_11)) + (rb_state3.var_13)) ^ (0xc47ce711)) ^ (rb_state3.var_12));
                    rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_14);
                }
                #endif
                m_server.lock();
                if (client->m_assigned_jobs >= client->m_threads) {
                    m_server.unlock();
                    goto getnext;
                }
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_2 = (rb_state8.var_2) ^ (0xc2cf7e38);
                #endif
                seqn = client->m_assigned_jobs++;
                m_server.unlock();
            }
            action = client->task(seqn, (int)id);
            if (action == THREAD_EXIT) {
                break;
            }

        getnext:

            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_1) == (0x1cf5d72a)) {
                pthread_mutex_lock(&(rb_state8.lock_23));
                rb_state8.var_7 = 0x20bee059;
                rb_state8.var_8 = (rb_state8.var_8) ^ (((0x477db644) ^ (0xf942a242)) - (rb_state8.var_5));
                rb_state8.var_9 = (rb_state8.var_9) - ((0x32adfe9d) + (0xa992e179));
                rb_state8.var_10 = (rb_state8.var_10) - (rb_state8.var_7);
                rb_state8.var_11 = (rb_state8.var_11) + (((((0xca91d18c) + (0x4d261b76)) - (rb_state8.var_6)) ^ (rb_state8.var_7)) ^ (rb_state8.var_8));
                rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x3bfb2855));
                rb_state8.var_13 = (rb_state8.var_13) ^ (rb_state8.var_11);
                rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_13) : (0xb0a9e719));
                rb_state8.var_6 = (rb_state8.var_6) + (rb_state8.var_14);
                rb_state8.var_3 = rb_state8.var_6;
                pthread_mutex_unlock(&(rb_state8.lock_23));
            }
            if ((rb_state8.var_1) == (0x1cf5d72a)) {
                pthread_mutex_lock(&(rb_state8.lock_23));
                rb_state8.var_16 = 0xa9897c78;
                rb_state8.var_17 = (rb_state8.var_17) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_10) : (0xfad3aa56));
                rb_state8.var_18 = (rb_state8.var_18) ^ (301 < rb_input_size ? (uint32_t)rb_input[301] : 0xd60902bc);
                rb_state8.var_19 = (rb_state8.var_19) + (((((0xd8983860) + (rb_state8.var_9)) + (0x737d097a)) - (rb_state8.var_16)) + (0x456d8adf));
                rb_state8.var_20 = (rb_state8.var_20) + (((((0xee624718) + (rb_state8.var_12)) + (0xa7e4b9d3)) ^ (rb_state8.var_8)) ^ (rb_state8.var_17));
                rb_state8.var_21 = (rb_state8.var_21) ^ (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_19) : (0x7e367d36));
                rb_state8.var_22 = (rb_state8.var_22) + (((((0x2c63e283) - (0x75b6ae2)) - (0xb540f532)) + (rb_state8.var_21)) ^ (rb_state8.var_20));
                rb_state8.var_15 = (rb_state8.var_15) ^ (rb_state8.var_22);
                rb_state8.var_3 = rb_state8.var_15;
                pthread_mutex_unlock(&(rb_state8.lock_23));
            }
            #endif
            seqn = deactivateThread((int)id);
        }

        m_scheduler[id].suspend(false, -1);
        return NULL;
    }

    _INLINE static int deactivateThread(const int threadID) {

        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x9e520200)) {
            pthread_mutex_lock(&(rb_state17.lock_43));
            rb_state17.var_14 = 0x3256602b;
            rb_state17.var_15 = 0xa6d46475;
            rb_state17.var_16 = (rb_state17.var_16) ^ ((0xa25de282) - (rb_state17.var_7));
            rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_6) : (0x87da7ddc));
            rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_15) : (0x25eacb1f));
            rb_state17.var_19 = (rb_state17.var_19) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_14) : (0xf7e80de3));
            rb_state17.var_20 = (rb_state17.var_20) ^ (((0x1f5f1f6b) + (0xe11ff5b6)) - (rb_state17.var_16));
            rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_17) : (0x371a6cca));
            rb_state17.var_22 = (rb_state17.var_22) ^ (((0x133f49cd) ^ (rb_state17.var_18)) ^ (rb_state17.var_12));
            rb_state17.var_23 = (rb_state17.var_23) ^ (((0xfcf94473) - (rb_state17.var_19)) ^ (0x6bd5dd9));
            rb_state17.var_24 = (rb_state17.var_24) + (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0x40812044));
            rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_22) : (0x3655c236));
            rb_state17.var_26 = (rb_state17.var_26) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_23) : (0x9038be46));
            rb_state17.var_27 = (rb_state17.var_27) ^ (rb_state17.var_24);
            rb_state17.var_28 = (rb_state17.var_28) ^ (((0x93a86a5e) - (rb_state17.var_25)) ^ (rb_state17.var_15));
            rb_state17.var_29 = (rb_state17.var_29) + (((((0x278f84b) ^ (rb_state17.var_27)) ^ (rb_state17.var_26)) + (rb_state17.var_16)) + (0x774cdb77));
            rb_state17.var_30 = (rb_state17.var_30) - (((rb_state17.var_28) == (0x0)) ? (rb_state17.var_29) : (0x7c78b8ea));
            rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_30);
            rb_state17.var_3 = rb_state17.var_13;
            pthread_mutex_unlock(&(rb_state17.lock_43));
        }
        if ((rb_state17.var_0) == (0x9e520200)) {
            pthread_mutex_lock(&(rb_state17.lock_43));
            rb_state17.var_32 = 0xc81b6d15;
            rb_state17.var_33 = (rb_state17.var_33) + (((0x59011aaf) - (rb_state17.var_17)) ^ (0x2916bd72));
            rb_state17.var_34 = (rb_state17.var_34) + (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_19) : (0xf93c5b68));
            rb_state17.var_35 = (rb_state17.var_35) + (rb_state17.var_21);
            rb_state17.var_36 = (rb_state17.var_36) - (((rb_state17.var_32) == (0x0)) ? (rb_state17.var_18) : (0x1bbff4e));
            rb_state17.var_37 = (rb_state17.var_37) - (((((0x17e816fe) + (rb_state17.var_34)) - (rb_state17.var_23)) + (rb_state17.var_33)) + (rb_state17.var_22));
            rb_state17.var_38 = (rb_state17.var_38) + (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_35) : (0x6155cf01));
            rb_state17.var_39 = (rb_state17.var_39) + (rb_state17.var_36);
            rb_state17.var_40 = (rb_state17.var_40) + (((rb_state17.var_37) == (0x0)) ? (rb_state17.var_38) : (0xb3fb592a));
            rb_state17.var_41 = (rb_state17.var_41) ^ (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_39) : (0x8d92e998));
            rb_state17.var_42 = (rb_state17.var_42) + (((rb_state17.var_40) == (0x0)) ? (rb_state17.var_41) : (0x99669150));
            rb_state17.var_31 = (rb_state17.var_31) ^ (rb_state17.var_42);
            rb_state17.var_3 = rb_state17.var_31;
            pthread_mutex_unlock(&(rb_state17.lock_43));
        }
        #endif
        MultiThreadedTaskQueue *client = m_client[threadID];
        assert(m_client[threadID]);
        m_client[threadID] = NULL;

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
                j = 0;
            }
        }
        return -1;
    }

    void addClient(MultiThreadedTaskQueue *client) {

    recheck:
        m_server.lock();
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ ((0x261aded9) ^ (rb_state17.var_0));
        #endif
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

        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - ((0x7328415e) ^ (rb_state14.var_0));
        #endif
        m_finished_jobs = j + 1;
        if (m_finished_jobs == queuesize) {
            m_finished_jobs = 0;
        }

        int nt = m_server.m_threads;

        for (int i = 0; i < nt; i++) {
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

            #ifdef RACEBENCH_BUG_17
            if ((rb_state17.var_0) == (0x9e520200)) {
                if ((rb_state17.var_3) != (0x0)) {
                    if (!((rb_state17.var_3) == (rb_state17.var_31))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            int ji = getNextClientIndex();
            if (ji == -1) {

                return false;
            }
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_2);
            #endif
            m_client[i] = m_waitingClients[ji];

            if (++m_scheduledJobs[ji] == m_waitingClients[ji]->m_threads) {
                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) - (197 < rb_input_size ? (uint32_t)rb_input[197] : 0x917b1fb1);
                #endif
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
        m_server.suspend();
    }

    code = m_server.unlock();
}

MultiThreadedTaskQueueServer MultiThreadedTaskQueue::m_server(1);
MultiThreadedTaskQueue **MultiThreadedTaskQueue::m_client = NULL;
MultiThreadedScheduler *MultiThreadedTaskQueue::m_scheduler = NULL;