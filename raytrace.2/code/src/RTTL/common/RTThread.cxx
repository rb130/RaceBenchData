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
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_0) == (0x183a826b)) {
                rb_state3.var_5 = 0x99003df2;
                rb_state3.var_6 = 0x44ead17d;
                rb_state3.var_7 = 0xb5ab83de;
                rb_state3.var_8 = (rb_state3.var_8) ^ (((0x5f32ad1f) + (0x57f9150b)) + (rb_state3.var_5));
                rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_7) : (0x20da92f5));
                rb_state3.var_10 = (rb_state3.var_10) + (((0x905c990d) - (0x9200e04d)) ^ (rb_state3.var_6));
                rb_state3.var_11 = (rb_state3.var_11) + (((0x1fc57b71) ^ (rb_state3.var_8)) - (rb_state3.var_5));
                rb_state3.var_12 = (rb_state3.var_12) - (rb_state3.var_9);
                rb_state3.var_13 = (rb_state3.var_13) + (((0x101f2d59) + (rb_state3.var_6)) - (rb_state3.var_10));
                rb_state3.var_14 = (rb_state3.var_14) - (((0x30eca3c8) ^ (rb_state3.var_11)) ^ (0xb7d79feb));
                rb_state3.var_15 = (rb_state3.var_15) ^ (((0xa31da6f8) ^ (rb_state3.var_12)) ^ (0xd47e0a46));
                rb_state3.var_16 = (rb_state3.var_16) - (((0x46c48207) ^ (rb_state3.var_13)) ^ (0xb03782af));
                rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_14);
                rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_16) : (0xcb5759b4));
                rb_state3.var_19 = (rb_state3.var_19) + (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_17) : (0x5111f41c));
                rb_state3.var_20 = (rb_state3.var_20) + (rb_state3.var_18);
                rb_state3.var_21 = (rb_state3.var_21) + (((0x724417e7) ^ (0xd8a0ffa3)) ^ (rb_state3.var_19));
                rb_state3.var_22 = (rb_state3.var_22) - (rb_state3.var_20);
                rb_state3.var_23 = (rb_state3.var_23) - (rb_state3.var_21);
                rb_state3.var_24 = (rb_state3.var_24) + (rb_state3.var_22);
                rb_state3.var_25 = (rb_state3.var_25) + (((rb_state3.var_23) == (0x0)) ? (rb_state3.var_24) : (0xb8ae225c));
                rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_25);
                rb_state3.var_1 = rb_state3.var_4;
            }
            if ((rb_state3.var_0) == (0x183a826b)) {
                pthread_mutex_lock(&(rb_state3.lock_35));
                rb_state3.var_27 = 0x3351344a;
                rb_state3.var_28 = (rb_state3.var_28) ^ (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_27) : (0xf9f4a383));
                rb_state3.var_29 = (rb_state3.var_29) - (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_10) : (0x255183b));
                rb_state3.var_30 = (rb_state3.var_30) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_28) : (0xc64bca8));
                rb_state3.var_31 = (rb_state3.var_31) + (rb_state3.var_29);
                rb_state3.var_32 = (rb_state3.var_32) - (rb_state3.var_30);
                rb_state3.var_33 = (rb_state3.var_33) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_31) : (0xaf5cfe68));
                rb_state3.var_34 = (rb_state3.var_34) + (((((0xf259c53a) + (rb_state3.var_13)) + (rb_state3.var_33)) - (rb_state3.var_32)) ^ (0x686249e9));
                rb_state3.var_26 = (rb_state3.var_26) + (rb_state3.var_34);
                rb_state3.var_1 = rb_state3.var_26;
                pthread_mutex_unlock(&(rb_state3.lock_35));
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_1) == (0xab19c559)) {
                rb_state9.var_2 = rb_state9.var_0;
            }
            if ((rb_state9.var_1) == (0xffff84aa)) {
                pthread_mutex_lock(&(rb_state9.lock_11));
                if (!((rb_state9.var_0) == (rb_state9.var_2))) {
                    racebench_trigger(9);
                }
                pthread_mutex_unlock(&(rb_state9.lock_11));
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0xa28e74c9));
            if ((rb_state10.var_1) == (0x40ada57c)) {
                pthread_mutex_lock(&(rb_state10.lock_15));
                rb_state10.var_4 = 0x9db95126;
                rb_state10.var_5 = (rb_state10.var_5) - (((((0x7598dd11) - (rb_state10.var_4)) ^ (rb_state10.var_3)) - (0xe87109a0)) - (rb_state10.var_2));
                rb_state10.var_6 = (rb_state10.var_6) + (((((0x68b5ea5e) + (rb_state10.var_4)) - (rb_state10.var_5)) - (0xf4959234)) + (rb_state10.var_5));
                rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_6);
                rb_state10.var_2 = rb_state10.var_3;
                pthread_mutex_unlock(&(rb_state10.lock_15));
            }
            if ((rb_state10.var_1) == (0x40ada57c)) {
                rb_state10.var_7 = 0x5e8166a5;
                rb_state10.var_8 = 0xfad3376;
                rb_state10.var_9 = (rb_state10.var_9) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_6) : (0xa3b73da0));
                rb_state10.var_10 = (rb_state10.var_10) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_7) : (0x974f26e1));
                rb_state10.var_11 = (rb_state10.var_11) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_9) : (0x75b7ac1d));
                rb_state10.var_12 = (rb_state10.var_12) + (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_10) : (0x43b775c0));
                rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_11);
                rb_state10.var_14 = (rb_state10.var_14) - (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x8b207b34));
                rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_14);
            }
            if ((rb_state10.var_1) == (0x40ada57c)) {
                pthread_mutex_lock(&(rb_state10.lock_15));
                if (!((rb_state10.var_2) == (rb_state10.var_3))) {
                    racebench_trigger(10);
                }
                pthread_mutex_unlock(&(rb_state10.lock_15));
            }
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x8ed8a774)) {
                pthread_mutex_lock(&(rb_state12.lock_7));
                rb_state12.var_3 = 0x3e55ad7a;
                rb_state12.var_4 = (rb_state12.var_4) ^ (0xabb6f7b);
                rb_state12.var_5 = (rb_state12.var_5) ^ (((rb_state12.var_3) == (0x3e55ad7a)) ? (rb_state12.var_2) : (0x84a2894f));
                rb_state12.var_6 = (rb_state12.var_6) - (((rb_state12.var_4) == (0xabb6f7b)) ? (rb_state12.var_5) : (0xf9ffcaa9));
                rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_6);
                pthread_mutex_unlock(&(rb_state12.lock_7));
            }
            #endif
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_1) == (0x0)) {
                rb_state15.var_2 = rb_state15.var_0;
            }
            if ((rb_state15.var_1) == (0x0)) {
                if (!((rb_state15.var_0) == (rb_state15.var_2))) {
                    racebench_trigger(15);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
            #endif
            code = lock();
        }
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x3bc53ee7)) {
            rb_state0.var_2 = rb_state0.var_1;
        }
        if ((rb_state0.var_0) == (0x3bc53ee7)) {
            rb_state0.var_3 = 0xd251f308;
            rb_state0.var_4 = 0x6d34d63b;
            rb_state0.var_5 = (rb_state0.var_5) + (((((0x52b23f22) - (rb_state0.var_2)) ^ (rb_state0.var_4)) + (rb_state0.var_4)) + (rb_state0.var_3));
            rb_state0.var_6 = (rb_state0.var_6) + (((((0xc361b55b) + (rb_state0.var_6)) - (rb_state0.var_3)) ^ (rb_state0.var_5)) - (rb_state0.var_5));
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_6);
        }
        if ((rb_state0.var_0) == (0x3bc53ee7)) {
            if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_1) == (0x1b0)) {
            rb_state5.var_13 = 0xdd2a7755;
            rb_state5.var_14 = 0x4d565227;
            rb_state5.var_15 = (rb_state5.var_15) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_14) : (0x112be799));
            rb_state5.var_16 = (rb_state5.var_16) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_9) : (0x1274ef07));
            rb_state5.var_17 = (rb_state5.var_17) + (((0x78b30bbc) - (0x8167940e)) ^ (rb_state5.var_7));
            rb_state5.var_18 = (rb_state5.var_18) - (rb_state5.var_13);
            rb_state5.var_19 = (rb_state5.var_19) + (rb_state5.var_15);
            rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_16);
            rb_state5.var_21 = (rb_state5.var_21) ^ (rb_state5.var_17);
            rb_state5.var_22 = (rb_state5.var_22) ^ (((0x857d28c7) ^ (rb_state5.var_18)) ^ (rb_state5.var_11));
            rb_state5.var_23 = (rb_state5.var_23) ^ (rb_state5.var_19);
            rb_state5.var_24 = (rb_state5.var_24) ^ (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_20) : (0xd292ba65));
            rb_state5.var_25 = (rb_state5.var_25) + (((rb_state5.var_13) == (0xdd2a7755)) ? (rb_state5.var_21) : (0xd6b7c9c4));
            rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_14) == (0x4d565227)) ? (rb_state5.var_22) : (0x21e6761));
            rb_state5.var_27 = (rb_state5.var_27) - (rb_state5.var_23);
            rb_state5.var_28 = (rb_state5.var_28) + (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_25) : (0x6ee099bb));
            rb_state5.var_29 = (rb_state5.var_29) - (((rb_state5.var_15) == (0x4d565227)) ? (rb_state5.var_26) : (0x95add582));
            rb_state5.var_30 = (rb_state5.var_30) ^ (rb_state5.var_27);
            rb_state5.var_31 = (rb_state5.var_31) - (((rb_state5.var_28) == (0xf74b77ae)) ? (rb_state5.var_29) : (0xfc49e39c));
            rb_state5.var_32 = (rb_state5.var_32) ^ (rb_state5.var_30);
            rb_state5.var_33 = (rb_state5.var_33) ^ (rb_state5.var_31);
            rb_state5.var_34 = (rb_state5.var_34) - (((((0x98a82ef4) - (rb_state5.var_32)) ^ (rb_state5.var_17)) + (rb_state5.var_16)) + (rb_state5.var_33));
            rb_state5.var_12 = (rb_state5.var_12) + (rb_state5.var_34);
            rb_state5.var_2 = rb_state5.var_12;
        }
        if ((rb_state5.var_1) == (0x1b0)) {
            if ((rb_state5.var_2) != (0x0)) {
                if (!((rb_state5.var_2) == (rb_state5.var_35))) {
                    racebench_trigger(5);
                }
            }
        }
        if ((rb_state5.var_1) == (0x1b0)) {
            pthread_mutex_lock(&(rb_state5.lock_42));
            rb_state5.var_36 = 0xe34f3bcf;
            rb_state5.var_37 = 0x7c50f928;
            rb_state5.var_38 = (rb_state5.var_38) ^ (rb_state5.var_36);
            rb_state5.var_39 = (rb_state5.var_39) + (((0x700a8ebe) + (rb_state5.var_18)) - (rb_state5.var_19));
            rb_state5.var_40 = (rb_state5.var_40) - (((((0xcaff0646) - (rb_state5.var_20)) ^ (rb_state5.var_38)) - (rb_state5.var_37)) + (0x771c00e4));
            rb_state5.var_41 = (rb_state5.var_41) - (((((0xe53095d2) - (rb_state5.var_39)) + (0xdcdd756e)) + (rb_state5.var_21)) - (rb_state5.var_40));
            rb_state5.var_35 = (rb_state5.var_35) ^ (rb_state5.var_41);
            rb_state5.var_2 = rb_state5.var_35;
            pthread_mutex_unlock(&(rb_state5.lock_42));
        }
        #endif
        m_suspended = value;
        code = MultiThreadedSyncPrimitive::suspend();
        code = unlock();
    }

    _INLINE void resume() {
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

        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_0);
        rb_state4.var_1 = (rb_state4.var_1) - (0x6092be8);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_1) == (0xc5a47ff2)) ? (rb_state12.var_1) : (0x425b205c));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x3c1cfb92)) ? (rb_state17.var_1) : (0x7c713bec));
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

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (0x6b3f4410);
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) + (0xb2f65320);
            #endif
            sched_yield();
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0xb3acbec7)) ? (rb_state3.var_0) : (0xf1b1ad51));
            #endif
            #ifdef RACEBENCH_BUG_6
            if ((rb_state6.var_1) == (0xb2f65342)) {
                rb_state6.var_2 = rb_state6.var_0;
            }
            if ((rb_state6.var_1) == (0xb2f65321)) {
                if (!((rb_state6.var_0) == (rb_state6.var_2))) {
                    racebench_trigger(6);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
            rb_state7.var_0 = (rb_state7.var_0) + (201 < rb_input_size ? (uint32_t)rb_input[201] : 0x825828a3);
            if ((rb_state7.var_0) == (0x952abf70)) {
                pthread_mutex_lock(&(rb_state7.lock_16));
                rb_state7.var_10 = 0xb21a8aea;
                rb_state7.var_11 = (rb_state7.var_11) - (((0x954c0ed2) - (rb_state7.var_4)) - (0x280e576a));
                rb_state7.var_12 = (rb_state7.var_12) + (((((0xa8dc4785) + (rb_state7.var_10)) ^ (0x47ec1fe)) + (0x9feb5548)) ^ (rb_state7.var_5));
                rb_state7.var_13 = (rb_state7.var_13) ^ (rb_state7.var_11);
                rb_state7.var_14 = (rb_state7.var_14) + (((0x2787580d) - (rb_state7.var_12)) - (rb_state7.var_6));
                rb_state7.var_15 = (rb_state7.var_15) ^ (((((0xeaf64c29) + (rb_state7.var_14)) ^ (rb_state7.var_13)) - (rb_state7.var_7)) - (rb_state7.var_8));
                rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_15);
                pthread_mutex_unlock(&(rb_state7.lock_16));
            }
            #endif
            ns = 0;
            for (int i = 0; i < m_threads; i++) {
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_0) == (0x183a826b)) {
                    if ((rb_state3.var_1) != (0x0)) {
                        if (!((rb_state3.var_1) == (rb_state3.var_26))) {
                            racebench_trigger(3);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_4
                if ((rb_state4.var_0) == (0x1001e374)) {
                    rb_state4.var_2 = rb_state4.var_1;
                }
                if ((rb_state4.var_0) == (0x1e70c240)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                        racebench_trigger(4);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x54e641f3));
                rb_state9.var_0 = (rb_state9.var_0) - (347 < rb_input_size ? (uint32_t)rb_input[347] : 0xe896a6f4);
                if ((rb_state9.var_1) == (0xffff84aa)) {
                    rb_state9.var_3 = 0xb0203f14;
                    rb_state9.var_4 = 0x77a30dca;
                    rb_state9.var_5 = 0xde645351;
                    rb_state9.var_6 = (rb_state9.var_6) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0x308adc34));
                    rb_state9.var_7 = (rb_state9.var_7) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_5) : (0x273d96b4));
                    rb_state9.var_8 = (rb_state9.var_8) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_6) : (0xbd373155));
                    rb_state9.var_9 = (rb_state9.var_9) - (rb_state9.var_7);
                    rb_state9.var_10 = (rb_state9.var_10) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0x89dea671));
                    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_10);
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x5fb75f90));
                #endif
                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_0) == (0x0)) {
                    pthread_mutex_lock(&(rb_state14.lock_19));
                    rb_state14.var_4 = 0x1efe8489;
                    rb_state14.var_5 = (rb_state14.var_5) - (258 < rb_input_size ? (uint32_t)rb_input[258] : 0x99f48ede);
                    rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_3) : (0x12326ca6));
                    rb_state14.var_7 = (rb_state14.var_7) - (((((0xd4629526) - (rb_state14.var_2)) - (0xa33a5306)) - (rb_state14.var_4)) ^ (rb_state14.var_5));
                    rb_state14.var_8 = (rb_state14.var_8) ^ (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_5) : (0xb94a9d8c));
                    rb_state14.var_9 = (rb_state14.var_9) - (((((0x49a9eae0) - (rb_state14.var_7)) + (rb_state14.var_7)) ^ (rb_state14.var_8)) ^ (rb_state14.var_6));
                    rb_state14.var_10 = (rb_state14.var_10) - (((0xfb0cb238) - (rb_state14.var_8)) - (0xcb064f9d));
                    rb_state14.var_11 = (rb_state14.var_11) - (rb_state14.var_9);
                    rb_state14.var_12 = (rb_state14.var_12) - (((((0x8d12d528) ^ (rb_state14.var_10)) + (rb_state14.var_11)) ^ (rb_state14.var_9)) + (0xf4031ccd));
                    rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_12);
                    rb_state14.var_2 = rb_state14.var_3;
                    pthread_mutex_unlock(&(rb_state14.lock_19));
                }
                if ((rb_state14.var_0) == (0x0)) {
                    pthread_mutex_lock(&(rb_state14.lock_19));
                    if (!((rb_state14.var_2) == (rb_state14.var_3))) {
                        racebench_trigger(14);
                    }
                    pthread_mutex_unlock(&(rb_state14.lock_19));
                }
                #endif
                #ifdef RACEBENCH_BUG_15
                if ((rb_state15.var_1) == (0x0)) {
                    rb_state15.var_3 = 0xa4f1df34;
                    rb_state15.var_4 = 0xd1f37bf0;
                    rb_state15.var_5 = (rb_state15.var_5) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x2cc5218b));
                    rb_state15.var_6 = (rb_state15.var_6) ^ (((0x5145554) - (rb_state15.var_4)) + (0x8afe05ae));
                    rb_state15.var_7 = (rb_state15.var_7) + (rb_state15.var_3);
                    rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_5) : (0x81d1b284));
                    rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_6);
                    rb_state15.var_10 = (rb_state15.var_10) - (rb_state15.var_7);
                    rb_state15.var_11 = (rb_state15.var_11) ^ (((0xd5b10ac9) - (rb_state15.var_5)) - (rb_state15.var_8));
                    rb_state15.var_12 = (rb_state15.var_12) ^ (((0xa694078e) ^ (rb_state15.var_9)) - (0x54de67e2));
                    rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_10) : (0x638b2aa5));
                    rb_state15.var_14 = (rb_state15.var_14) - (((((0xb043d0b4) ^ (rb_state15.var_12)) - (rb_state15.var_7)) + (0xd184dc8b)) ^ (rb_state15.var_11));
                    rb_state15.var_15 = (rb_state15.var_15) ^ (rb_state15.var_13);
                    rb_state15.var_16 = (rb_state15.var_16) - (((0x3eae6b9f) + (0x3300f8c0)) + (rb_state15.var_14));
                    rb_state15.var_17 = (rb_state15.var_17) + (((((0x2b7301e6) + (rb_state15.var_15)) ^ (rb_state15.var_8)) - (rb_state15.var_16)) + (rb_state15.var_9));
                    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_17);
                }
                #endif
                ns += m_scheduler[i].suspended();
            }
        } while (ns != m_threads);
    }

    static void *threadFunc(void *_id) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0xb3acbec7);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0xf92d51c4);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xb3f0ac31));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (140 < rb_input_size ? (uint32_t)rb_input[140] : 0x308188bb);
        if ((rb_state7.var_0) == (0x952abf70)) {
            pthread_mutex_lock(&(rb_state7.lock_16));
            rb_state7.var_3 = 0x72b5255e;
            rb_state7.var_4 = 0xb1526670;
            rb_state7.var_5 = (rb_state7.var_5) - (((0x14fb4ba1) + (0x33558666)) ^ (rb_state7.var_4));
            rb_state7.var_6 = (rb_state7.var_6) ^ (((0x462d9afa) + (rb_state7.var_3)) - (0x538d1108));
            rb_state7.var_7 = (rb_state7.var_7) - (((((0x6ff1062b) ^ (0x807423a1)) ^ (rb_state7.var_1)) + (rb_state7.var_2)) - (rb_state7.var_5));
            rb_state7.var_8 = (rb_state7.var_8) - (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_6) : (0xe5995439));
            rb_state7.var_9 = (rb_state7.var_9) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0xa7d1d9cf));
            rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_9);
            rb_state7.var_1 = rb_state7.var_2;
            pthread_mutex_unlock(&(rb_state7.lock_16));
        }
        if ((rb_state7.var_0) == (0x952abf70)) {
            if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x341acda7));
        rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_1);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x16062d6a));
        rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x84f2e25c));
        rb_state12.var_0 = (rb_state12.var_0) ^ (249 < rb_input_size ? (uint32_t)rb_input[249] : 0xe60c4acd);
        if ((rb_state12.var_0) == (0x8ed8a774)) {
            pthread_mutex_lock(&(rb_state12.lock_7));
            rb_state12.var_2 = rb_state12.var_1;
            pthread_mutex_unlock(&(rb_state12.lock_7));
        }
        if ((rb_state12.var_0) == (0x8ed8a774)) {
            if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ (90 < rb_input_size ? (uint32_t)rb_input[90] : 0x38730);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (0x11dd765b);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (103 < rb_input_size ? (uint32_t)rb_input[103] : 0x4e0438c8);
        rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0xc3e303ce));
        rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_1);
        if ((rb_state17.var_1) == (0x7839f800)) {
            rb_state17.var_13 = 0xe6b5a564;
            rb_state17.var_14 = (rb_state17.var_14) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_8) : (0xe5c2120d));
            rb_state17.var_15 = (rb_state17.var_15) - (((((0x3e8776c7) + (0xb9d997cf)) - (rb_state17.var_7)) + (rb_state17.var_13)) ^ (0x86dc6d5a));
            rb_state17.var_16 = (rb_state17.var_16) ^ (38 < rb_input_size ? (uint32_t)rb_input[38] : 0x8235a265);
            rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_15) : (0x4f9b561c));
            rb_state17.var_18 = (rb_state17.var_18) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_16) : (0x517f24a2));
            rb_state17.var_19 = (rb_state17.var_19) - (rb_state17.var_17);
            rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_18);
            rb_state17.var_21 = (rb_state17.var_21) - (((0x27cae33b) + (0x3fddc9df)) ^ (rb_state17.var_19));
            rb_state17.var_22 = (rb_state17.var_22) + (rb_state17.var_20);
            rb_state17.var_23 = (rb_state17.var_23) ^ (((0x37a45e62) - (0x878141d)) - (rb_state17.var_21));
            rb_state17.var_24 = (rb_state17.var_24) + (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_22) : (0x3c2c8c66));
            rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_24) : (0x3656f78d));
            rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_25);
            rb_state17.var_2 = rb_state17.var_12;
        }
        if ((rb_state17.var_1) == (0x7839f800)) {
            pthread_mutex_lock(&(rb_state17.lock_32));
            rb_state17.var_27 = 0x943561d9;
            rb_state17.var_28 = (rb_state17.var_28) ^ ((0xd158b3eb) - (rb_state17.var_14));
            rb_state17.var_29 = (rb_state17.var_29) + (((((0xc94ccc73) - (rb_state17.var_27)) - (0xc8bf66b8)) - (rb_state17.var_13)) - (0x40702b3e));
            rb_state17.var_30 = (rb_state17.var_30) + (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_28) : (0x9b98223f));
            rb_state17.var_31 = (rb_state17.var_31) ^ (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_30) : (0xde527638));
            rb_state17.var_26 = (rb_state17.var_26) - (rb_state17.var_31);
            rb_state17.var_2 = rb_state17.var_26;
            pthread_mutex_unlock(&(rb_state17.lock_32));
        }
        #endif
        long long int id = (long long int)_id;

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (0x5a00b161);
        rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_0);
        rb_state1.var_1 = (rb_state1.var_1) - (0xcec484d5);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0xceb2fb1c)) ? (rb_state3.var_0) : (0x31e56908));
        rb_state3.var_0 = (rb_state3.var_0) - (0xeac3d8d9);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (202 < rb_input_size ? (uint32_t)rb_input[202] : 0x9f61f052);
        rb_state5.var_1 = (rb_state5.var_1) + (300 < rb_input_size ? (uint32_t)rb_input[300] : 0x1e6ccda0);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (323 < rb_input_size ? (uint32_t)rb_input[323] : 0xa22094a7);
        rb_state6.var_1 = (rb_state6.var_1) ^ (122 < rb_input_size ? (uint32_t)rb_input[122] : 0xa2fe91fe);
        if ((rb_state6.var_1) == (0xb2f65321)) {
            pthread_mutex_lock(&(rb_state6.lock_6));
            rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_1);
            rb_state6.var_4 = (rb_state6.var_4) ^ (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_2) : (0xd22b56ad));
            rb_state6.var_5 = (rb_state6.var_5) ^ (((((0x9f25213e) ^ (rb_state6.var_4)) ^ (rb_state6.var_3)) ^ (rb_state6.var_5)) + (rb_state6.var_4));
            rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_5);
            pthread_mutex_unlock(&(rb_state6.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x65)) ? (rb_state7.var_0) : (0x6ad540b0));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + (413 < rb_input_size ? (uint32_t)rb_input[413] : 0xcaafe28a);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (55 < rb_input_size ? (uint32_t)rb_input[55] : 0x8dfde495);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ ((0xdcab05db) - (rb_state14.var_0));
        rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_1);
        if ((rb_state14.var_0) == (0x0)) {
            rb_state14.var_13 = 0xe99b662f;
            rb_state14.var_14 = 0x5d6b49a1;
            rb_state14.var_15 = 0x55eb0e1c;
            rb_state14.var_16 = (rb_state14.var_16) - (((((0xbafd7a7) - (rb_state14.var_13)) - (rb_state14.var_10)) + (rb_state14.var_11)) + (rb_state14.var_15));
            rb_state14.var_17 = (rb_state14.var_17) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_14) : (0xb25aa9dc));
            rb_state14.var_18 = (rb_state14.var_18) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0xa26659f4));
            rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_18);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - ((0xfa08cc6a) - (0x12275ab0));
        rb_state15.var_1 = (rb_state15.var_1) ^ (0xef89d2f3);
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_1) == (0x7839f800)) {
            pthread_mutex_lock(&(rb_state17.lock_32));
            if ((rb_state17.var_2) != (0x0)) {
                if (!((rb_state17.var_2) == (rb_state17.var_26))) {
                    racebench_trigger(17);
                }
            }
            pthread_mutex_unlock(&(rb_state17.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ ((0x86b00d46) + (rb_state19.var_0));
        #endif
        _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + ((0xaeed243e) + (0x6d1d4aa6));
        rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0xb0cb2ad4)) ? (rb_state0.var_0) : (0xe512c15));
        rb_state0.var_0 = (rb_state0.var_0) - (0x2938f9eb);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_1);
        if ((rb_state1.var_0) == (0xd132d1b6)) {
            pthread_mutex_lock(&(rb_state1.lock_28));
            rb_state1.var_4 = 0x484e8931;
            rb_state1.var_5 = (rb_state1.var_5) - (((0x6a51fb54) ^ (rb_state1.var_2)) ^ (0x7526e712));
            rb_state1.var_6 = (rb_state1.var_6) + (0x7b3ab022);
            rb_state1.var_7 = (rb_state1.var_7) - (54 < rb_input_size ? (uint32_t)rb_input[54] : 0x412d881);
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x83cbef1e));
            rb_state1.var_9 = (rb_state1.var_9) - (((0xf5d27cca) - (0xef087363)) - (rb_state1.var_4));
            rb_state1.var_10 = (rb_state1.var_10) ^ (((0xc8698274) - (rb_state1.var_5)) ^ (0xf1b1c6eb));
            rb_state1.var_11 = (rb_state1.var_11) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0x39dbee34));
            rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_9) : (0xa7c5e89));
            rb_state1.var_13 = (rb_state1.var_13) - (rb_state1.var_10);
            rb_state1.var_14 = (rb_state1.var_14) + (((((0x4074e47) ^ (rb_state1.var_5)) - (rb_state1.var_12)) ^ (rb_state1.var_11)) ^ (0x53973af7));
            rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0x84941587));
            rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_15);
            rb_state1.var_2 = rb_state1.var_3;
            pthread_mutex_unlock(&(rb_state1.lock_28));
        }
        if ((rb_state1.var_0) == (0xd132d1b6)) {
            pthread_mutex_lock(&(rb_state1.lock_28));
            rb_state1.var_16 = 0x8f2241fa;
            rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_6) : (0xc2e3e2a0));
            rb_state1.var_18 = (rb_state1.var_18) ^ (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_7) : (0xe0ace167));
            rb_state1.var_19 = (rb_state1.var_19) + ((0xf2a2049c) + (rb_state1.var_9));
            rb_state1.var_20 = (rb_state1.var_20) + (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0x11df04da));
            rb_state1.var_21 = (rb_state1.var_21) - (rb_state1.var_19);
            rb_state1.var_22 = (rb_state1.var_22) ^ (((0x67e3c23f) + (rb_state1.var_20)) + (0x1a91dc1));
            rb_state1.var_23 = (rb_state1.var_23) ^ (((0xd64c90f5) ^ (0xb0d06fe9)) - (rb_state1.var_21));
            rb_state1.var_24 = (rb_state1.var_24) ^ (((0xe69751ee) + (rb_state1.var_22)) + (rb_state1.var_10));
            rb_state1.var_25 = (rb_state1.var_25) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_23) : (0x7252667));
            rb_state1.var_26 = (rb_state1.var_26) - (rb_state1.var_24);
            rb_state1.var_27 = (rb_state1.var_27) ^ (((((0x12739629) - (rb_state1.var_25)) ^ (rb_state1.var_12)) - (rb_state1.var_26)) ^ (rb_state1.var_13));
            rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_27);
            pthread_mutex_unlock(&(rb_state1.lock_28));
        }
        if ((rb_state1.var_0) == (0xd132d1b6)) {
            if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
        if ((rb_state4.var_0) == (0x1e70c240)) {
            rb_state4.var_3 = 0x5ecb3439;
            rb_state4.var_4 = (rb_state4.var_4) - (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_3) : (0x3bd33853));
            rb_state4.var_5 = (rb_state4.var_5) - (((0x3324116) - (rb_state4.var_5)) ^ (rb_state4.var_3));
            rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_2) : (0xdff90ad8));
            rb_state4.var_7 = (rb_state4.var_7) - (((0xde7c754d) + (rb_state4.var_4)) ^ (rb_state4.var_7));
            rb_state4.var_8 = (rb_state4.var_8) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0x8d66c21b));
            rb_state4.var_9 = (rb_state4.var_9) + (((0x2a9c18e9) - (rb_state4.var_7)) + (rb_state4.var_8));
            rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0x2f18b85f));
            rb_state4.var_11 = (rb_state4.var_11) - (((0x7a36ae8f) ^ (0x8ce45d60)) + (rb_state4.var_9));
            rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0x8629059c));
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (0x11d5195c);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_1) : (0xbcd9dcf8));
        if ((rb_state8.var_1) == (0xc8)) {
            pthread_mutex_lock(&(rb_state8.lock_32));
            rb_state8.var_4 = (rb_state8.var_4) - (rb_state8.var_3);
            rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_4);
            rb_state8.var_6 = (rb_state8.var_6) ^ (rb_state8.var_2);
            rb_state8.var_7 = (rb_state8.var_7) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0xde30769a));
            rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_5) : (0x957e6286));
            rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_6) : (0x17f3667f));
            rb_state8.var_10 = (rb_state8.var_10) + (((0xe49ef213) ^ (rb_state8.var_8)) ^ (rb_state8.var_7));
            rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_8);
            rb_state8.var_12 = (rb_state8.var_12) + (((((0x97cfcf30) + (rb_state8.var_9)) + (rb_state8.var_9)) - (0xcf03c32c)) - (rb_state8.var_10));
            rb_state8.var_13 = (rb_state8.var_13) + (((0xbde2db2f) - (0x7d168305)) - (rb_state8.var_11));
            rb_state8.var_14 = (rb_state8.var_14) ^ (rb_state8.var_12);
            rb_state8.var_15 = (rb_state8.var_15) ^ (rb_state8.var_13);
            rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_14);
            rb_state8.var_17 = (rb_state8.var_17) - (rb_state8.var_15);
            rb_state8.var_18 = (rb_state8.var_18) ^ (((0x8c42e0e) ^ (0x814945da)) ^ (rb_state8.var_16));
            rb_state8.var_19 = (rb_state8.var_19) ^ (((rb_state8.var_10) == (0xe49ef213)) ? (rb_state8.var_17) : (0x9feada28));
            rb_state8.var_20 = (rb_state8.var_20) ^ (((((0x36e19fd7) ^ (rb_state8.var_11)) + (rb_state8.var_18)) ^ (0x40061a96)) + (rb_state8.var_19));
            rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_20);
            rb_state8.var_2 = rb_state8.var_3;
            pthread_mutex_unlock(&(rb_state8.lock_32));
        }
        if ((rb_state8.var_1) == (0xc8)) {
            rb_state8.var_21 = 0xea19c0a8;
            rb_state8.var_22 = 0xd9048063;
            rb_state8.var_23 = (rb_state8.var_23) - (rb_state8.var_13);
            rb_state8.var_24 = (rb_state8.var_24) ^ (rb_state8.var_21);
            rb_state8.var_25 = (rb_state8.var_25) ^ (((0x5d379ef2) ^ (rb_state8.var_14)) + (rb_state8.var_12));
            rb_state8.var_26 = (rb_state8.var_26) ^ (((rb_state8.var_22) == (0xd9048063)) ? (rb_state8.var_23) : (0x743cde98));
            rb_state8.var_27 = (rb_state8.var_27) - (rb_state8.var_24);
            rb_state8.var_28 = (rb_state8.var_28) + (((0x733de18e) + (rb_state8.var_25)) + (rb_state8.var_15));
            rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_16) == (0x908a1d11)) ? (rb_state8.var_26) : (0xd08e3c4));
            rb_state8.var_30 = (rb_state8.var_30) ^ (((((0x1f56e3a6) ^ (rb_state8.var_28)) + (rb_state8.var_27)) - (0x370b6d53)) ^ (rb_state8.var_17));
            rb_state8.var_31 = (rb_state8.var_31) + (((rb_state8.var_29) == (0x13e5b232)) ? (rb_state8.var_30) : (0xceb38612));
            rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_31);
        }
        if ((rb_state8.var_1) == (0xc8)) {
            if (!((rb_state8.var_2) == (rb_state8.var_3))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_0);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (0xefd496a1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - (0x3a5b800e);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x10267338));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0xd112290f) ^ (0x98eeb995));
        #endif
        m_scheduler[id].suspend();

        int seqn;
        long long int tag = -1;
        while (!m_server.finished()) {
            MultiThreadedTaskQueue *client = MultiThreadedTaskQueue::m_client[id];

            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) + (330 < rb_input_size ? (uint32_t)rb_input[330] : 0x9805f17e);
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
                seqn = client->m_assigned_jobs++;
                m_server.unlock();
            }
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_1) == (0x0)) {
                pthread_mutex_lock(&(rb_state19.lock_27));
                rb_state19.var_4 = 0xc6e26414;
                rb_state19.var_5 = (rb_state19.var_5) ^ (((0xaf44a05) ^ (0x994a13ec)) + (rb_state19.var_3));
                rb_state19.var_6 = (rb_state19.var_6) + ((0x3c31d487) + (0xccbb2170));
                rb_state19.var_7 = (rb_state19.var_7) + (((((0x2d26afd5) + (rb_state19.var_4)) + (rb_state19.var_2)) ^ (0xe7cc5315)) - (0xfdae0b98));
                rb_state19.var_8 = (rb_state19.var_8) ^ (((0x469ff967) + (0xc4d8eb6)) + (rb_state19.var_5));
                rb_state19.var_9 = (rb_state19.var_9) + (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_6) : (0x7a7fa404));
                rb_state19.var_10 = (rb_state19.var_10) ^ (((0x854a8780) - (rb_state19.var_7)) ^ (rb_state19.var_5));
                rb_state19.var_11 = (rb_state19.var_11) ^ (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_9) : (0x7fdfbd2));
                rb_state19.var_12 = (rb_state19.var_12) + (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_10) : (0x3fc5c717));
                rb_state19.var_13 = (rb_state19.var_13) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0x5746c2c4));
                rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_13);
                rb_state19.var_2 = rb_state19.var_3;
                pthread_mutex_unlock(&(rb_state19.lock_27));
            }
            if ((rb_state19.var_1) == (0x0)) {
                if (!((rb_state19.var_2) == (rb_state19.var_3))) {
                    racebench_trigger(19);
                }
            }
            #endif
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

        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - (249 < rb_input_size ? (uint32_t)rb_input[249] : 0x332c2ccb);
        #endif
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
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
        #endif
        m_scheduledJobs[j] = 0;

        m_finished_jobs = j + 1;
        if (m_finished_jobs == queuesize) {
            m_finished_jobs = 0;
        }

        int nt = m_server.m_threads;

        for (int i = 0; i < nt; i++) {
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) ^ (255 < rb_input_size ? (uint32_t)rb_input[255] : 0xee5fc938);
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
        m_server.suspend();
    }

    code = m_server.unlock();
}

MultiThreadedTaskQueueServer MultiThreadedTaskQueue::m_server(1);
MultiThreadedTaskQueue **MultiThreadedTaskQueue::m_client = NULL;
MultiThreadedScheduler *MultiThreadedTaskQueue::m_scheduler = NULL;