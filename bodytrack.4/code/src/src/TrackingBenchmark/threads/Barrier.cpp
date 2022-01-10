#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(HAVE_LIBPTHREAD)
#include <pthread.h>
#include <errno.h>
#else
#include "Mutex.h"
#include "Condition.h"
#endif

#include <exception>

#include "Barrier.h"

namespace threads {

Barrier::Barrier(int _n) throw(BarrierException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x4aec5039)) {
        if ((rb_state5.var_3) != (0x0)) {
            if (!((rb_state5.var_3) == (rb_state5.var_17))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x4bf8acf1)) {
        rb_state7.var_14 = 0x8a645d03;
        rb_state7.var_15 = (rb_state7.var_15) - (((rb_state7.var_8) == (0xdcf8a380)) ? (rb_state7.var_14) : (0x3506a0ec));
        rb_state7.var_16 = (rb_state7.var_16) ^ (50 < rb_input_size ? (uint32_t)rb_input[50] : 0x6c7741e8);
        rb_state7.var_17 = (rb_state7.var_17) - (rb_state7.var_7);
        rb_state7.var_18 = (rb_state7.var_18) - (((0x4370e116) ^ (rb_state7.var_15)) + (rb_state7.var_9));
        rb_state7.var_19 = (rb_state7.var_19) + (((0x39ba5322) - (rb_state7.var_16)) - (rb_state7.var_10));
        rb_state7.var_20 = (rb_state7.var_20) - (((rb_state7.var_11) == (0x0)) ? (rb_state7.var_17) : (0x6727d311));
        rb_state7.var_21 = (rb_state7.var_21) ^ (((0x3199f4c7) - (rb_state7.var_18)) + (rb_state7.var_12));
        rb_state7.var_22 = (rb_state7.var_22) ^ (((rb_state7.var_19) == (0xe180bdd6)) ? (rb_state7.var_20) : (0xa4fa38b9));
        rb_state7.var_23 = (rb_state7.var_23) ^ (((rb_state7.var_13) == (0xef6cfc5c)) ? (rb_state7.var_21) : (0x3f369a3f));
        rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_14) == (0x8a645d03)) ? (rb_state7.var_22) : (0x6dc3d94a));
        rb_state7.var_25 = (rb_state7.var_25) - (((rb_state7.var_15) == (0x759ba2fd)) ? (rb_state7.var_23) : (0x1b368930));
        rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_16) == (0x2e)) ? (rb_state7.var_24) : (0x3f57be33));
        rb_state7.var_27 = (rb_state7.var_27) - (((0x8f7dfa57) ^ (rb_state7.var_17)) ^ (rb_state7.var_25));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_26) == (0xdf99e292)) ? (rb_state7.var_27) : (0x7ff738b4));
        rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (((rb_state19.var_0) == (0xffffff70)) ? (rb_state19.var_2) : (0xc8218d4d));
    #endif
    n = _n;
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x9a0f9537)) {
        pthread_mutex_lock(&(rb_state13.lock_19));
        rb_state13.var_6 = 0xcef3e1ba;
        rb_state13.var_7 = 0x124105b6;
        rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_7) : (0x178111f3));
        rb_state13.var_9 = (rb_state13.var_9) - (rb_state13.var_6);
        rb_state13.var_10 = (rb_state13.var_10) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_8) : (0xac46b23e));
        rb_state13.var_11 = (rb_state13.var_11) - (((rb_state13.var_9) == (0x310c1e46)) ? (rb_state13.var_10) : (0xcc5ca92e));
        rb_state13.var_5 = (rb_state13.var_5) ^ (rb_state13.var_11);
        rb_state13.var_4 = rb_state13.var_5;
        pthread_mutex_unlock(&(rb_state13.lock_19));
    }
    if ((rb_state13.var_1) == (0x9a0f9537)) {
        if (!((rb_state13.var_4) == (rb_state13.var_5))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (0x68d1c2ec);
    #endif
    rv = pthread_barrier_init(&b, NULL, n);

    switch (rv) {
    case 0:
        break;
    case EINVAL:
    case EBUSY: {
        BarrierInitException e;
        throw e;
        break;
    }
    case EAGAIN:
    case ENOMEM: {
        BarrierResourceException e;
        throw e;
        break;
    }
    default: {
        BarrierUnknownException e;
        throw e;
        break;
    }
    }
#else
    n = _n;
    countSleep = 0;
    countReset = 0;
    M = new Mutex;
    CSleep = new Condition(*M);
    CReset = new Condition(*M);
#endif
}

Barrier::~Barrier() throw(BarrierException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_barrier_destroy(&b);

    switch (rv) {
    case 0:
        break;
    case EINVAL:
    case EBUSY: {
        BarrierDestroyException e;
        throw e;
        break;
    }
    default: {
        BarrierUnknownException e;
        throw e;
        break;
    }
    }
#else
    delete CReset;
    delete CSleep;
    delete M;
#endif
}

bool Barrier::Wait() throw(BarrierException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_barrier_wait(&b);

    switch (rv) {
    case 0:
        break;
    case PTHREAD_BARRIER_SERIAL_THREAD:
        return true;
        break;
    case EINVAL: {
        BarrierException e;
        throw e;
        break;
    }
    default: {
        BarrierUnknownException e;
        throw e;
        break;
    }
    }

    return false;
#else
    bool master;

    M->Lock();

    while (countSleep >= n)
        CReset->Wait();

    master = (countSleep == 0);
    countSleep++;

    if (countSleep < n) {

        CSleep->Wait();
    } else {
        countReset = 0;
        CSleep->NotifyAll();
    }

    countReset++;

    if (countReset < n) {

        CReset->Wait();
    } else {
        countSleep = 0;
        CReset->NotifyAll();
    }

    M->Unlock();

    return master;
#endif
}

const int Barrier::nThreads() const {
    return n;
}

}; // namespace threads