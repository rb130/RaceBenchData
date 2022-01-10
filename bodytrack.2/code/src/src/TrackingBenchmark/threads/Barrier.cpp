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

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_1) == (0x1c491f0b)) {
        if ((rb_state2.var_2) != (0x0)) {
            if (!((rb_state2.var_2) == (rb_state2.var_16))) {
                racebench_trigger(2);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_12) == (0xc5bd47e4)) {
        rb_state11.var_28 = 0xd30c92d;
        rb_state11.var_29 = 0x77f202b6;
        rb_state11.var_30 = (rb_state11.var_30) + (rb_state11.var_28);
        rb_state11.var_31 = (rb_state11.var_31) - (((rb_state11.var_29) == (0x77f202b6)) ? (rb_state11.var_20) : (0x2f98647e));
        rb_state11.var_32 = (rb_state11.var_32) + (((((0xf4569861) + (rb_state11.var_30)) + (rb_state11.var_31)) - (0x28564d9f)) ^ (0x1a791968));
        rb_state11.var_20 = (rb_state11.var_20) - (rb_state11.var_32);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_0) == (0x6c3a9e54)) ? (rb_state14.var_1) : (0x392577e0));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xb44b76a4)) {
        rb_state16.var_9 = 0x12f85069;
        rb_state16.var_10 = (rb_state16.var_10) ^ (((((0xcf24e3c1) + (0x7d46155e)) ^ (0xaf4467a2)) + (rb_state16.var_7)) ^ (rb_state16.var_9));
        rb_state16.var_11 = (rb_state16.var_11) - (((((0x1adf0818) - (rb_state16.var_6)) - (0xf9407633)) ^ (rb_state16.var_10)) - (rb_state16.var_8));
        rb_state16.var_8 = (rb_state16.var_8) - (rb_state16.var_11);
        rb_state16.var_2 = rb_state16.var_8;
    }
    if ((rb_state16.var_0) == (0xb44b76a4)) {
        pthread_mutex_lock(&(rb_state16.lock_52));
        rb_state16.var_13 = (rb_state16.var_13) + (0xba0cecef);
        rb_state16.var_14 = (rb_state16.var_14) - (((rb_state16.var_13) == (0xba0cecef)) ? (rb_state16.var_12) : (0x981ccaff));
        rb_state16.var_15 = (rb_state16.var_15) - (0x9195246c);
        rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_9) : (0x76248769));
        rb_state16.var_17 = (rb_state16.var_17) ^ (((0x52456ffd) ^ (rb_state16.var_11)) ^ (rb_state16.var_15));
        rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_16) == (0x12f85069)) ? (rb_state16.var_10) : (0xa0331054));
        rb_state16.var_19 = (rb_state16.var_19) + (((rb_state16.var_17) == (0x139814a6)) ? (rb_state16.var_13) : (0x299763e7));
        rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_18) == (0xf1d6ced4)) ? (rb_state16.var_14) : (0x666b64b9));
        rb_state16.var_21 = (rb_state16.var_21) + (((rb_state16.var_19) == (0xba0cecef)) ? (rb_state16.var_15) : (0x3eb04f55));
        rb_state16.var_22 = (rb_state16.var_22) + (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_16) : (0x3fcd3539));
        rb_state16.var_23 = (rb_state16.var_23) - (((rb_state16.var_21) == (0x6e6adb94)) ? (rb_state16.var_17) : (0x4b833bb6));
        rb_state16.var_24 = (rb_state16.var_24) ^ (((rb_state16.var_18) == (0xf1d6ced4)) ? (rb_state16.var_19) : (0x1e15ec0f));
        rb_state16.var_25 = (rb_state16.var_25) ^ (((0x324e46a3) - (rb_state16.var_20)) + (rb_state16.var_22));
        rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_21);
        rb_state16.var_27 = (rb_state16.var_27) + (((rb_state16.var_23) == (0xec67eb5a)) ? (rb_state16.var_22) : (0xb5b73c1a));
        rb_state16.var_28 = (rb_state16.var_28) + (((0xb3081ca3) + (rb_state16.var_23)) + (0xedabd2e3));
        rb_state16.var_29 = (rb_state16.var_29) ^ (((rb_state16.var_24) == (0xba0cecef)) ? (rb_state16.var_24) : (0x66d603e7));
        rb_state16.var_30 = (rb_state16.var_30) + (rb_state16.var_25);
        rb_state16.var_31 = (rb_state16.var_31) + (((0x7131bc3b) + (rb_state16.var_26)) + (0xb344f06b));
        rb_state16.var_32 = (rb_state16.var_32) - (((rb_state16.var_25) == (0x4546970c)) ? (rb_state16.var_27) : (0xabfb85d3));
        rb_state16.var_33 = (rb_state16.var_33) - (((0x4cefed25) ^ (0xa9014422)) + (rb_state16.var_28));
        rb_state16.var_34 = (rb_state16.var_34) - (((0x31e93864) - (rb_state16.var_29)) + (rb_state16.var_26));
        rb_state16.var_35 = (rb_state16.var_35) + (((0x4942c28c) + (rb_state16.var_30)) - (rb_state16.var_27));
        rb_state16.var_36 = (rb_state16.var_36) + (((0xf8866a28) + (0x76ae26d1)) + (rb_state16.var_31));
        rb_state16.var_37 = (rb_state16.var_37) - (rb_state16.var_32);
        rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_33);
        rb_state16.var_39 = (rb_state16.var_39) - (((rb_state16.var_34) == (0x19b8d8f7)) ? (rb_state16.var_35) : (0xa7db0dbe));
        rb_state16.var_40 = (rb_state16.var_40) ^ (((0x76bc48ea) + (rb_state16.var_36)) + (rb_state16.var_28));
        rb_state16.var_41 = (rb_state16.var_41) - (((0x6efa2073) + (rb_state16.var_37)) - (0xe5f0aec8));
        rb_state16.var_42 = (rb_state16.var_42) + (rb_state16.var_38);
        rb_state16.var_43 = (rb_state16.var_43) + (((rb_state16.var_29) == (0xba0cecef)) ? (rb_state16.var_39) : (0xecccfab3));
        rb_state16.var_44 = (rb_state16.var_44) ^ (((rb_state16.var_40) == (0x5ee3cfd)) ? (rb_state16.var_41) : (0x94b969f3));
        rb_state16.var_45 = (rb_state16.var_45) - (((((0xf64cac55) ^ (rb_state16.var_31)) - (rb_state16.var_43)) - (rb_state16.var_30)) ^ (rb_state16.var_42));
        rb_state16.var_46 = (rb_state16.var_46) - (((0xd13a1ede) ^ (rb_state16.var_44)) + (0xd79578b9));
        rb_state16.var_47 = (rb_state16.var_47) + (((rb_state16.var_32) == (0xed07af97)) ? (rb_state16.var_45) : (0x73427df2));
        rb_state16.var_48 = (rb_state16.var_48) ^ (rb_state16.var_46);
        rb_state16.var_49 = (rb_state16.var_49) - (((rb_state16.var_33) == (0x8cf57c19)) ? (rb_state16.var_47) : (0x5e0632bb));
        rb_state16.var_50 = (rb_state16.var_50) + (((0xe898c7f4) ^ (rb_state16.var_48)) + (rb_state16.var_34));
        rb_state16.var_51 = (rb_state16.var_51) - (((rb_state16.var_49) == (0x1602ea8b)) ? (rb_state16.var_50) : (0x7bf0ec0c));
        rb_state16.var_12 = (rb_state16.var_12) + (rb_state16.var_51);
        rb_state16.var_2 = rb_state16.var_12;
        pthread_mutex_unlock(&(rb_state16.lock_52));
    }
    #endif
    n = _n;
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
    rb_state19.var_1 = (rb_state19.var_1) ^ (0xacfb98c0);
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