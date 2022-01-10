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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) + (362 < rb_input_size ? (uint32_t)rb_input[362] : 0x7cf6cf9);
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_1) : (0x5a3bf4a3));
            rb_state3.var_3 = (rb_state3.var_3) - ((0x4add5436) ^ (rb_state3.var_4));
            if ((rb_state3.var_0) == (0xffffff00)) {
                rb_state3.var_6 = 0x3e326bd;
                rb_state3.var_7 = 0x961ff19f;
                rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_5) : (0xd0f355e2));
                rb_state3.var_9 = (rb_state3.var_9) - (0x43f7d620);
                rb_state3.var_10 = (rb_state3.var_10) - (((((0x54bf2664) + (rb_state3.var_7)) - (rb_state3.var_7)) + (rb_state3.var_8)) + (rb_state3.var_6));
                rb_state3.var_11 = (rb_state3.var_11) - (rb_state3.var_8);
                rb_state3.var_12 = (rb_state3.var_12) - (((0x24aa9be2) + (rb_state3.var_9)) ^ (rb_state3.var_9));
                rb_state3.var_13 = (rb_state3.var_13) + (((0x9ea1a5fb) - (rb_state3.var_10)) ^ (rb_state3.var_10));
                rb_state3.var_14 = (rb_state3.var_14) ^ (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_11) : (0xd3bc74aa));
                rb_state3.var_15 = (rb_state3.var_15) ^ (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_13) : (0xee8da021));
                rb_state3.var_16 = (rb_state3.var_16) - (((0x1b5e72a1) ^ (rb_state3.var_14)) ^ (0x4c96063b));
                rb_state3.var_17 = (rb_state3.var_17) - (((0x5950ea22) + (rb_state3.var_15)) - (0x77783026));
                rb_state3.var_18 = (rb_state3.var_18) - (((((0x502b5b59) - (0x2355d86f)) ^ (rb_state3.var_17)) - (0x70d456b0)) ^ (rb_state3.var_16));
                rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_18);
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_0 = (rb_state5.var_0) + (0xbaf0cc75);
            rb_state5.var_2 = (rb_state5.var_2) + (0xa1fbefd2);
            rb_state5.var_2 = (rb_state5.var_2) ^ ((0xb3d173be) ^ (rb_state5.var_3));
            rb_state5.var_4 = (rb_state5.var_4) + (249 < rb_input_size ? (uint32_t)rb_input[249] : 0xe5e8de6f);
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_4);
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_4);
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) ^ (115 < rb_input_size ? (uint32_t)rb_input[115] : 0x69871a80);
            #endif
            code = lock();
        }
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0xffffff00)) {
            pthread_mutex_lock(&(rb_state3.lock_19));
            rb_state3.var_5 = rb_state3.var_1;
            pthread_mutex_unlock(&(rb_state3.lock_19));
        }
        if ((rb_state3.var_0) == (0xffffff00)) {
            if (!((rb_state3.var_1) == (rb_state3.var_5))) {
                racebench_trigger(3);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_3) : (0x7e7368b2));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ ((0x2ccdcaac) - (rb_state10.var_0));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_3 = (rb_state14.var_3) ^ (0xebb73321);
        if ((rb_state14.var_0) == (0xb2500917)) {
            rb_state14.var_19 = 0x5b18693d;
            rb_state14.var_20 = 0xd1bc6fa0;
            rb_state14.var_21 = (rb_state14.var_21) ^ ((0xeee673f2) + (rb_state14.var_11));
            rb_state14.var_22 = (rb_state14.var_22) + (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_12) : (0xa182a80b));
            rb_state14.var_23 = (rb_state14.var_23) - (rb_state14.var_19);
            rb_state14.var_24 = (rb_state14.var_24) - (rb_state14.var_10);
            rb_state14.var_25 = (rb_state14.var_25) - (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_20) : (0xda9529cf));
            rb_state14.var_26 = (rb_state14.var_26) - (rb_state14.var_21);
            rb_state14.var_27 = (rb_state14.var_27) - (rb_state14.var_22);
            rb_state14.var_28 = (rb_state14.var_28) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_23) : (0xfee85cdc));
            rb_state14.var_29 = (rb_state14.var_29) - (rb_state14.var_24);
            rb_state14.var_30 = (rb_state14.var_30) ^ (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_25) : (0xa0c9a228));
            rb_state14.var_31 = (rb_state14.var_31) - (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_27) : (0x41c1f953));
            rb_state14.var_32 = (rb_state14.var_32) - (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_28) : (0xff771c3));
            rb_state14.var_33 = (rb_state14.var_33) + (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_29) : (0x27779c1));
            rb_state14.var_34 = (rb_state14.var_34) ^ (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_30) : (0x154547ab));
            rb_state14.var_35 = (rb_state14.var_35) - (((((0x5f4c5981) - (rb_state14.var_20)) - (rb_state14.var_31)) - (rb_state14.var_32)) + (0x8988cc56));
            rb_state14.var_36 = (rb_state14.var_36) - (((rb_state14.var_21) == (0x0)) ? (rb_state14.var_33) : (0x5115340f));
            rb_state14.var_37 = (rb_state14.var_37) ^ (((rb_state14.var_22) == (0x0)) ? (rb_state14.var_34) : (0x5bf21e78));
            rb_state14.var_38 = (rb_state14.var_38) + (rb_state14.var_35);
            rb_state14.var_39 = (rb_state14.var_39) + (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_36) : (0xf3ed1b71));
            rb_state14.var_40 = (rb_state14.var_40) ^ (((0x27dc98cf) ^ (rb_state14.var_24)) - (rb_state14.var_37));
            rb_state14.var_41 = (rb_state14.var_41) - (rb_state14.var_38);
            rb_state14.var_42 = (rb_state14.var_42) + (((((0x257e91fa) ^ (rb_state14.var_39)) - (0xe943d82c)) + (0x6c90bd)) - (rb_state14.var_40));
            rb_state14.var_43 = (rb_state14.var_43) ^ (((rb_state14.var_25) == (0x0)) ? (rb_state14.var_41) : (0x50224dfd));
            rb_state14.var_44 = (rb_state14.var_44) ^ (rb_state14.var_42);
            rb_state14.var_45 = (rb_state14.var_45) ^ (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_43) : (0x679ff61c));
            rb_state14.var_46 = (rb_state14.var_46) ^ (((rb_state14.var_27) == (0x0)) ? (rb_state14.var_44) : (0xcf69e8a3));
            rb_state14.var_47 = (rb_state14.var_47) - (((((0x789a1514) + (rb_state14.var_46)) ^ (rb_state14.var_28)) + (0xb0505051)) - (rb_state14.var_45));
            rb_state14.var_4 = (rb_state14.var_4) - (rb_state14.var_47);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_0);
        #endif
        m_suspended = value;
        code = MultiThreadedSyncPrimitive::suspend();
        code = unlock();
    }

    _INLINE void resume() {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_3 = (rb_state1.var_3) - (((rb_state1.var_0) == (0x746c12c)) ? (rb_state1.var_4) : (0xfc5be1e3));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xffffffe0)) ? (rb_state4.var_0) : (0x6c34d886));
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
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) ^ (0xe12410bd);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (208 < rb_input_size ? (uint32_t)rb_input[208] : 0x2ed2a914);
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
            rb_state0.var_0 = (rb_state0.var_0) - (0x69bd43be);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ ((0xe7f1ae15) + (0x18ecd8ce));
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_4 = (rb_state3.var_4) - (rb_state3.var_3);
            rb_state3.var_4 = (rb_state3.var_4) + (0x8ee50dd9);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_0);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_1);
            rb_state17.var_3 = (rb_state17.var_3) ^ (0xfb6ffacc);
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + ((0xb66042f3) + (0xce16b2c6));
            #endif
            sched_yield();
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x6c)) ? (rb_state5.var_0) : (0x3c229079));
            #endif
            ns = 0;
            for (int i = 0; i < m_threads; i++) {
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_3 = (rb_state1.var_3) + ((0x5e16970d) + (0xa51e26b0));
                rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x2f99ccf1));
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) - (0x593d2fa3);
                #endif
                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_1) == (0xd8)) {
                    rb_state5.var_5 = rb_state5.var_2;
                }
                if ((rb_state5.var_1) == (0xd8)) {
                    if (!((rb_state5.var_2) == (rb_state5.var_5))) {
                        racebench_trigger(5);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) + ((0x46602dad) - (rb_state8.var_1));
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - ((0x91fbaf4f) - (0xb8727fea));
                if ((rb_state9.var_1) == (0x6462ca16)) {
                    rb_state9.var_5 = rb_state9.var_2;
                }
                if ((rb_state9.var_1) == (0x6462ca16)) {
                    if (!((rb_state9.var_2) == (rb_state9.var_5))) {
                        racebench_trigger(9);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                rb_state11.var_1 = (rb_state11.var_1) - (0xc5b4ee24);
                rb_state11.var_2 = (rb_state11.var_2) - ((0x72618e0b) + (0x92ff1b40));
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_3 = (rb_state14.var_3) - (0x36be29b2);
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_1);
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0x176c5511));
                #endif
                ns += m_scheduler[i].suspended();
            }
        } while (ns != m_threads);
    }

    static void *threadFunc(void *_id) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (221 < rb_input_size ? (uint32_t)rb_input[221] : 0xb6cf537e);
        rb_state0.var_1 = (rb_state0.var_1) + (64 < rb_input_size ? (uint32_t)rb_input[64] : 0x83651aaf);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) + (284 < rb_input_size ? (uint32_t)rb_input[284] : 0x629b3a8c);
        rb_state3.var_3 = (rb_state3.var_3) ^ (0xd3f0b87c);
        rb_state3.var_1 = (rb_state3.var_1) - (60 < rb_input_size ? (uint32_t)rb_input[60] : 0x7d5583d1);
        rb_state3.var_2 = (rb_state3.var_2) ^ ((0xcb141bf3) + (0x86791d02));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x375a4fdb));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x535e810b));
        rb_state9.var_1 = (rb_state9.var_1) + ((0xc6531a86) + (rb_state9.var_1));
        rb_state9.var_2 = (rb_state9.var_2) + (0x3e957c73);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x5ab06e9e));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_1) : (0xc9d8e17));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0xe9648977));
        rb_state17.var_2 = (rb_state17.var_2) ^ (244 < rb_input_size ? (uint32_t)rb_input[244] : 0x5629a04b);
        rb_state17.var_1 = (rb_state17.var_1) ^ (0xda11a083);
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_2);
        rb_state17.var_4 = (rb_state17.var_4) - (0x48b7186b);
        if ((rb_state17.var_3) == (0xfb6ffacc)) {
            rb_state17.var_5 = rb_state17.var_4;
        }
        if ((rb_state17.var_3) == (0xfb6ffacc)) {
            if (!((rb_state17.var_4) == (rb_state17.var_5))) {
                racebench_trigger(17);
            }
        }
        #endif
        long long int id = (long long int)_id;

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
        rb_state3.var_1 = (rb_state3.var_1) ^ (0xab31cd45);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (301 < rb_input_size ? (uint32_t)rb_input[301] : 0x5ee7aa47);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0xc6531a86)) ? (rb_state9.var_0) : (0x8e9ef0f1));
        rb_state9.var_2 = (rb_state9.var_2) - (((rb_state9.var_2) == (0x7d2af8e6)) ? (rb_state9.var_3) : (0xfb9ee06));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (4 < rb_input_size ? (uint32_t)rb_input[4] : 0x42bfeb80);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + ((0x93b29e5e) ^ (0x25b8b289));
        rb_state14.var_1 = (rb_state14.var_1) ^ (126 < rb_input_size ? (uint32_t)rb_input[126] : 0xfe33218d);
        rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_2);
        rb_state14.var_3 = (rb_state14.var_3) ^ (322 < rb_input_size ? (uint32_t)rb_input[322] : 0xf581c24);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0x4c2b07f4);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_1);
        if ((rb_state17.var_3) == (0xfb6ffacc)) {
            rb_state17.var_6 = 0x1d4dfb66;
            rb_state17.var_7 = 0xd2194ed2;
            rb_state17.var_8 = (rb_state17.var_8) + (rb_state17.var_7);
            rb_state17.var_9 = (rb_state17.var_9) - (0x58aad42d);
            rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_6) : (0x19f18515));
            rb_state17.var_11 = (rb_state17.var_11) ^ (((0xf242964f) ^ (rb_state17.var_5)) ^ (0x59bfacba));
            rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_8);
            rb_state17.var_13 = (rb_state17.var_13) + (((0x84adb480) ^ (0xd2c0d6f5)) + (rb_state17.var_9));
            rb_state17.var_14 = (rb_state17.var_14) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_10) : (0x96a9e8f7));
            rb_state17.var_15 = (rb_state17.var_15) + (((((0xb3c9ac80) + (0x355c1b5d)) - (rb_state17.var_8)) + (rb_state17.var_12)) - (rb_state17.var_11));
            rb_state17.var_16 = (rb_state17.var_16) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_13) : (0x8afd6338));
            rb_state17.var_17 = (rb_state17.var_17) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_14) : (0x63304906));
            rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_15) : (0xb4c64b61));
            rb_state17.var_19 = (rb_state17.var_19) + (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_17) : (0x58ad2e2a));
            rb_state17.var_20 = (rb_state17.var_20) + (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_19) : (0x3d608f91));
            rb_state17.var_4 = (rb_state17.var_4) - (rb_state17.var_20);
        }
        #endif
        _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (264 < rb_input_size ? (uint32_t)rb_input[264] : 0x20222e1);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + ((0x118d4014) + (rb_state1.var_1));
        rb_state1.var_2 = (rb_state1.var_2) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0x1ce86b8));
        rb_state1.var_3 = (rb_state1.var_3) + (0x3ca1053b);
        rb_state1.var_4 = (rb_state1.var_4) ^ (((rb_state1.var_1) == (0x118d4014)) ? (rb_state1.var_4) : (0xcabed0f3));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) - (0x55fb17b1);
        rb_state5.var_4 = (rb_state5.var_4) + (rb_state5.var_2);
        rb_state5.var_3 = (rb_state5.var_3) ^ ((0x93742a5d) - (rb_state5.var_4));
        if ((rb_state5.var_1) == (0xd8)) {
            rb_state5.var_6 = 0x995d0a;
            rb_state5.var_7 = 0xe1db0694;
            rb_state5.var_8 = 0xf3c08ce6;
            rb_state5.var_9 = (rb_state5.var_9) + (rb_state5.var_7);
            rb_state5.var_10 = (rb_state5.var_10) - (((((0xca10938f) + (0xfa0bc166)) ^ (0x8960743e)) ^ (rb_state5.var_6)) - (rb_state5.var_8));
            rb_state5.var_11 = (rb_state5.var_11) ^ (((((0xd807f762) + (rb_state5.var_9)) + (0x9614d5d0)) ^ (0x45a080a0)) ^ (rb_state5.var_10));
            rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_1) == (0x6462ca16)) {
            rb_state9.var_6 = 0xcd8a5ff6;
            rb_state9.var_7 = 0x3cc16857;
            rb_state9.var_8 = (rb_state9.var_8) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_6) : (0xdd22a284));
            rb_state9.var_9 = (rb_state9.var_9) - (148 < rb_input_size ? (uint32_t)rb_input[148] : 0xe920855f);
            rb_state9.var_10 = (rb_state9.var_10) ^ (((0x47a2b99a) - (rb_state9.var_6)) ^ (0xbfc8b021));
            rb_state9.var_11 = (rb_state9.var_11) - (((((0x66bd402e) - (rb_state9.var_7)) ^ (rb_state9.var_5)) ^ (0xa010499)) - (0x48ef3e25));
            rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0xe2f13bb8));
            rb_state9.var_13 = (rb_state9.var_13) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_10) : (0x4a0cf53d));
            rb_state9.var_14 = (rb_state9.var_14) - (((0xe7f018dd) ^ (0x5d6da916)) - (rb_state9.var_11));
            rb_state9.var_15 = (rb_state9.var_15) ^ (((((0x5506ddc9) - (rb_state9.var_13)) - (rb_state9.var_12)) ^ (0xcd24f08e)) ^ (rb_state9.var_9));
            rb_state9.var_16 = (rb_state9.var_16) - (((0x1fab95e5) ^ (rb_state9.var_10)) - (rb_state9.var_14));
            rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_16) : (0x25beef99));
            rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (125 < rb_input_size ? (uint32_t)rb_input[125] : 0xef272120);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_3) == (0x9283ad00)) ? (rb_state14.var_2) : (0xb17de87b));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_2);
        if ((rb_state14.var_0) == (0xb2500917)) {
            pthread_mutex_lock(&(rb_state14.lock_48));
            rb_state14.var_6 = 0x203ff4ce;
            rb_state14.var_7 = (rb_state14.var_7) - ((0x4ffcbd07) ^ (0x25a12a3f));
            rb_state14.var_8 = (rb_state14.var_8) + (((((0x66feeaa) ^ (rb_state14.var_5)) ^ (0xffbf857)) + (rb_state14.var_6)) ^ (rb_state14.var_6));
            rb_state14.var_9 = (rb_state14.var_9) - (((0x35ea5b8) - (rb_state14.var_7)) ^ (rb_state14.var_4));
            rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_7);
            rb_state14.var_11 = (rb_state14.var_11) - (((0xde75d0e6) ^ (rb_state14.var_8)) + (rb_state14.var_8));
            rb_state14.var_12 = (rb_state14.var_12) ^ (rb_state14.var_9);
            rb_state14.var_13 = (rb_state14.var_13) - (rb_state14.var_10);
            rb_state14.var_14 = (rb_state14.var_14) - (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_12) : (0x6031cb9b));
            rb_state14.var_15 = (rb_state14.var_15) + (rb_state14.var_13);
            rb_state14.var_16 = (rb_state14.var_16) + (rb_state14.var_14);
            rb_state14.var_17 = (rb_state14.var_17) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_15) : (0x675a9f68));
            rb_state14.var_18 = (rb_state14.var_18) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0xfccf90a));
            rb_state14.var_5 = (rb_state14.var_5) ^ (rb_state14.var_18);
            rb_state14.var_4 = rb_state14.var_5;
            pthread_mutex_unlock(&(rb_state14.lock_48));
        }
        if ((rb_state14.var_0) == (0xb2500917)) {
            if (!((rb_state14.var_4) == (rb_state14.var_5))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) + ((0x1e83efa) ^ (rb_state17.var_0));
        #endif
        m_scheduler[id].suspend();

        int seqn;
        long long int tag = -1;
        while (!m_server.finished()) {
            MultiThreadedTaskQueue *client = MultiThreadedTaskQueue::m_client[id];

            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x746c12c)) {
                rb_state1.var_5 = rb_state1.var_2;
            }
            if ((rb_state1.var_1) == (0x746c12c)) {
                if (!((rb_state1.var_2) == (rb_state1.var_5))) {
                    racebench_trigger(1);
                }
            }
            #endif
            int action = 0;

            assert(client);
            if (tag != client->tag()) {
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_2 = (rb_state8.var_2) - (227 < rb_input_size ? (uint32_t)rb_input[227] : 0x5fd2a7d4);
                #endif
                tag = client->tag();
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_4 = (rb_state2.var_4) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_4) : (0x7a704f35));
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_2 = (rb_state8.var_2) - ((0x20fb360b) ^ (rb_state8.var_0));
                #endif
                m_server.lock();
                if (client->m_assigned_jobs >= client->m_threads) {
                    m_server.unlock();
                    goto getnext;
                }
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_1 = (rb_state13.var_1) - (51 < rb_input_size ? (uint32_t)rb_input[51] : 0xa3dfe333);
                #endif
                seqn = client->m_assigned_jobs++;
                #ifdef RACEBENCH_BUG_0
                if ((rb_state0.var_1) == (0x1c35732a)) {
                    pthread_mutex_lock(&(rb_state0.lock_11));
                    rb_state0.var_3 = rb_state0.var_2;
                    pthread_mutex_unlock(&(rb_state0.lock_11));
                }
                if ((rb_state0.var_1) == (0x1c35732a)) {
                    if (!((rb_state0.var_2) == (rb_state0.var_3))) {
                        racebench_trigger(0);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_0);
                #endif
                m_server.unlock();
            }
            action = client->task(seqn, (int)id);
            if (action == THREAD_EXIT) {
                break;
            }

        getnext:

            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_3) == (0xc68b9bbf)) {
                rb_state15.var_18 = 0x1cb316f2;
                rb_state15.var_19 = (rb_state15.var_19) - (rb_state15.var_10);
                rb_state15.var_20 = (rb_state15.var_20) ^ ((0xe24a7721) + (rb_state15.var_11));
                rb_state15.var_21 = (rb_state15.var_21) - (rb_state15.var_8);
                rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_12) == (0xa)) ? (rb_state15.var_9) : (0xff1d8e4f));
                rb_state15.var_23 = (rb_state15.var_23) ^ (rb_state15.var_18);
                rb_state15.var_24 = (rb_state15.var_24) ^ (((((0x796cd4b7) + (rb_state15.var_20)) - (rb_state15.var_13)) + (rb_state15.var_14)) ^ (rb_state15.var_19));
                rb_state15.var_25 = (rb_state15.var_25) - (((rb_state15.var_21) == (0xfffffff6)) ? (rb_state15.var_22) : (0x9976091));
                rb_state15.var_26 = (rb_state15.var_26) + (((0x47a6cff4) - (rb_state15.var_23)) - (rb_state15.var_15));
                rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_24) == (0xc66e78f3)) ? (rb_state15.var_25) : (0x4b66f462));
                rb_state15.var_28 = (rb_state15.var_28) - (((0x2a079b8c) + (rb_state15.var_26)) ^ (0xc87afef4));
                rb_state15.var_29 = (rb_state15.var_29) + (((rb_state15.var_16) == (0x516bc842)) ? (rb_state15.var_27) : (0x8bcbe393));
                rb_state15.var_30 = (rb_state15.var_30) + (((0xafc6a6f7) + (rb_state15.var_28)) ^ (rb_state15.var_17));
                rb_state15.var_31 = (rb_state15.var_31) + (rb_state15.var_29);
                rb_state15.var_32 = (rb_state15.var_32) + (rb_state15.var_30);
                rb_state15.var_33 = (rb_state15.var_33) ^ (((((0x6ae6ac13) ^ (0x4ae1596e)) - (rb_state15.var_32)) ^ (rb_state15.var_31)) + (rb_state15.var_18));
                rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_33);
            }
            #endif
            seqn = deactivateThread((int)id);
        }

        m_scheduler[id].suspend(false, -1);
        return NULL;
    }

    _INLINE static int deactivateThread(const int threadID) {

        #ifdef RACEBENCH_BUG_2
        rb_state2.var_4 = (rb_state2.var_4) ^ (0x2e5cb424);
        #endif
        MultiThreadedTaskQueue *client = m_client[threadID];
        assert(m_client[threadID]);
        m_client[threadID] = NULL;

        int code = m_server.lock();
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_2) == (0xb7be3f76)) {
            rb_state13.var_4 = rb_state13.var_3;
        }
        if ((rb_state13.var_2) == (0xb7be3f76)) {
            if (!((rb_state13.var_3) == (rb_state13.var_4))) {
                racebench_trigger(13);
            }
        }
        #endif
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
        rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_1);
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

        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0x46c2c8b0);
        #endif
        m_waitingClients[j] = client;
        m_scheduledJobs[j] = 0;

        m_finished_jobs = j + 1;
        if (m_finished_jobs == queuesize) {
            m_finished_jobs = 0;
        }

        int nt = m_server.m_threads;

        for (int i = 0; i < nt; i++) {
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (20 < rb_input_size ? (uint32_t)rb_input[20] : 0xf9908b3c);
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_2) == (0xd4fab5a8)) ? (rb_state11.var_1) : (0xeed2e7b4));
            #endif
            m_scheduler[i].lock();
            if (m_scheduler[i].suspended()) {
                if (!schedule(i)) {
                    m_scheduler[i].unlock();
                    break;
                }
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x46602dad)) ? (rb_state8.var_0) : (0x8daa8e6d));
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_2);
                #endif
                m_scheduler[i].resume();
            }
            m_scheduler[i].unlock();
        }

        m_server.unlock();
    }

    bool schedule(int i) {

        if (m_client[i] == NULL) {

            #ifdef RACEBENCH_BUG_0
            rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_1) == (0x69bd44ca)) ? (rb_state0.var_0) : (0x8c75761a));
            rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_0);
            #endif
            #ifdef RACEBENCH_BUG_2
            if ((rb_state2.var_3) == (0xb97226dc)) {
                rb_state2.var_6 = 0x4b45f875;
                rb_state2.var_7 = 0x40f7a9e7;
                rb_state2.var_8 = 0xffbf415a;
                rb_state2.var_9 = (rb_state2.var_9) ^ (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_5) : (0x813b7c83));
                rb_state2.var_10 = (rb_state2.var_10) - (((0xf4e0c7bb) ^ (0x8e4284da)) + (rb_state2.var_7));
                rb_state2.var_11 = (rb_state2.var_11) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_8) : (0x2ec7231c));
                rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_9);
                rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0xe6893445));
                rb_state2.var_14 = (rb_state2.var_14) ^ (((((0xa8fe3655) - (rb_state2.var_13)) ^ (rb_state2.var_7)) + (0x57a33f90)) + (rb_state2.var_12));
                rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_14);
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_1);
            #endif
            #ifdef RACEBENCH_BUG_7
            rb_state7.var_0 = (rb_state7.var_0) + ((0xafd42386) + (rb_state7.var_0));
            #endif
            int ji = getNextClientIndex();
            if (ji == -1) {

                return false;
            }
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) - (25 < rb_input_size ? (uint32_t)rb_input[25] : 0xfe1df9df);
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_1 = (rb_state10.var_1) ^ (0xf33efbab);
            #endif
            m_client[i] = m_waitingClients[ji];

            if (++m_scheduledJobs[ji] == m_waitingClients[ji]->m_threads) {
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_2 = (rb_state6.var_2) + (80 < rb_input_size ? (uint32_t)rb_input[80] : 0xb1c72a41);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_2 = (rb_state10.var_2) - (0xe70584fb);
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
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x746c12c)) {
        rb_state1.var_6 = 0x14b7c27f;
        rb_state1.var_7 = 0x2756f3af;
        rb_state1.var_8 = 0xefbb36e0;
        rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_5) == (0x7132995d)) ? (rb_state1.var_6) : (0x1ddc73bb));
        rb_state1.var_10 = (rb_state1.var_10) ^ (((rb_state1.var_7) == (0x2756f3af)) ? (rb_state1.var_8) : (0x8b229017));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((((0xd67e5b42) ^ (rb_state1.var_10)) ^ (rb_state1.var_7)) ^ (rb_state1.var_6)) ^ (rb_state1.var_9));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_11);
    }
    #endif
    int code = m_server.lock();

    while (m_finished_jobs < m_threads) {
        m_server.suspend();
    }

    code = m_server.unlock();
}

MultiThreadedTaskQueueServer MultiThreadedTaskQueue::m_server(1);
MultiThreadedTaskQueue **MultiThreadedTaskQueue::m_client = NULL;
MultiThreadedScheduler *MultiThreadedTaskQueue::m_scheduler = NULL;