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

    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_6) == (0x4cd7ca7)) {
        rb_state14.var_26 = 0x82622fd2;
        rb_state14.var_27 = 0x4b2a7827;
        rb_state14.var_28 = (rb_state14.var_28) + ((0xdc55a762) + (rb_state14.var_15));
        rb_state14.var_29 = (rb_state14.var_29) - (((rb_state14.var_26) == (0x82622fd2)) ? (rb_state14.var_14) : (0x4b919e6c));
        rb_state14.var_30 = (rb_state14.var_30) - (((rb_state14.var_27) == (0x4b2a7827)) ? (rb_state14.var_28) : (0xeebaba0));
        rb_state14.var_31 = (rb_state14.var_31) ^ (rb_state14.var_29);
        rb_state14.var_32 = (rb_state14.var_32) ^ (((((0x77e45d0a) ^ (rb_state14.var_30)) - (0xf5df68ba)) ^ (0x136996a3)) - (rb_state14.var_31));
        rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_32);
        rb_state14.var_10 = rb_state14.var_25;
    }
    if ((rb_state14.var_6) == (0x4cd7ca7)) {
        pthread_mutex_lock(&(rb_state14.lock_45));
        rb_state14.var_34 = 0x20c05919;
        rb_state14.var_35 = (rb_state14.var_35) - (707 < rb_input_size ? (uint32_t)rb_input[707] : 0x8c6b14f8);
        rb_state14.var_36 = (rb_state14.var_36) ^ (rb_state14.var_17);
        rb_state14.var_37 = (rb_state14.var_37) ^ (rb_state14.var_34);
        rb_state14.var_38 = (rb_state14.var_38) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_35) : (0x3283c22d));
        rb_state14.var_39 = (rb_state14.var_39) - (rb_state14.var_36);
        rb_state14.var_40 = (rb_state14.var_40) ^ (((0x35246791) - (rb_state14.var_18)) + (rb_state14.var_37));
        rb_state14.var_41 = (rb_state14.var_41) ^ (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_38) : (0xc5308b62));
        rb_state14.var_42 = (rb_state14.var_42) ^ (((0xa2a49636) + (rb_state14.var_39)) - (rb_state14.var_20));
        rb_state14.var_43 = (rb_state14.var_43) ^ (((((0x4353f70) - (0x794e7704)) ^ (rb_state14.var_40)) + (rb_state14.var_21)) + (rb_state14.var_41));
        rb_state14.var_44 = (rb_state14.var_44) - (((((0x1220fb4b) + (rb_state14.var_42)) ^ (rb_state14.var_43)) + (rb_state14.var_22)) ^ (rb_state14.var_23));
        rb_state14.var_33 = (rb_state14.var_33) ^ (rb_state14.var_44);
        rb_state14.var_10 = rb_state14.var_33;
        pthread_mutex_unlock(&(rb_state14.lock_45));
    }
    #endif
    n = _n;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xda576c9f)) {
        pthread_mutex_lock(&(rb_state8.lock_27));
        if ((rb_state8.var_1) != (0x0)) {
            if (!((rb_state8.var_1) == (rb_state8.var_19))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_27));
    }
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