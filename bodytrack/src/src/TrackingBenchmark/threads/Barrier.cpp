

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

    n = _n;
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
