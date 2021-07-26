
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include <exception>

#include "RWLock.h"

namespace threads {

RWLock::RWLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_init(&l, NULL);

    switch (rv) {
    case 0:
        break;
    case EAGAIN:
    case ENOMEM: {
        RWLockResourceException e;
        throw e;
        break;
    }
    case EPERM:
    case EBUSY:
    case EINVAL: {
        RWLockInitException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }
}

RWLock::~RWLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_destroy(&l);

    switch (rv) {
    case 0:
        break;
    case EBUSY:
    case EINVAL: {
        RWLockDestroyException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }
}

void RWLock::ReadLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_rdlock(&l);

    switch (rv) {
    case 0:

        break;
    case EINVAL:
    case EAGAIN: {
        RWLockLockingException e;
        throw e;
        break;
    }
    case EDEADLK: {
        RWLockDeadlockException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }
}

bool RWLock::TryReadLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_tryrdlock(&l);

    switch (rv) {
    case 0:

        break;
    case EBUSY:

        return false;
        break;
    case EINVAL:
    case EAGAIN: {
        RWLockLockingException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }

    return true;
}

void RWLock::WriteLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_wrlock(&l);

    switch (rv) {
    case 0:

        break;
    case EINVAL: {
        RWLockLockingException e;
        throw e;
        break;
    }
    case EDEADLK: {
        RWLockDeadlockException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }
}

bool RWLock::TryWriteLock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_trywrlock(&l);

    switch (rv) {
    case 0:

        break;
    case EBUSY:

        return false;
        break;
    case EINVAL: {
        RWLockLockingException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }

    return true;
}

void RWLock::Unlock() throw(RWLockException) {
    int rv;

    rv = pthread_rwlock_unlock(&l);

    switch (rv) {
    case 0:

        break;
    case EINVAL:
    case EPERM: {
        RWLockLockingException e;
        throw e;
        break;
    }
    default: {
        RWLockUnknownException e;
        throw e;
        break;
    }
    }
}

} // namespace threads
