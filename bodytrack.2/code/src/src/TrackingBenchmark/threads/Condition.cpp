#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(HAVE_LIBPTHREAD)
#include <pthread.h>
#include <errno.h>
#else
#include <windows.h>
#endif

#include <exception>

#include "Mutex.h"
#include "Condition.h"

namespace threads {

Condition::Condition(Mutex &_M) throw(CondException) {
    int rv;

    M = &_M;

#if defined(HAVE_LIBPTHREAD)
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xfffffff6)) {
        pthread_mutex_lock(&(rb_state3.lock_9));
        rb_state3.var_10 = !((rb_state3.var_3) == (rb_state3.var_2));
        pthread_mutex_unlock(&(rb_state3.lock_9));
    }
    #endif
    nWaiting = 0;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_9 = (rb_state18.var_9) ^ (((rb_state18.var_4) == (0x1)) ? (rb_state18.var_3) : (0x680a716f));
    #endif
    nWakeupTickets = 0;

    rv = pthread_cond_init(&c, NULL);

    switch (rv) {
    case 0:

        break;
    case EAGAIN:
    case ENOMEM: {
        CondResourceException e;
        throw e;
        break;
    }
    case EBUSY:
    case EINVAL: {
        CondInitException e;
        throw e;
        break;
    }
    default: {
        CondUnknownException e;
        throw e;
        break;
    }
    }
#else
    nWaiting = 0;
    nWakeupTickets = 0;
    genCounter = 0;

    c = CreateEvent(NULL, true, false, NULL);
    if (c == 0) {
        CondInitException e;
        throw e;
    }
#endif
}

Condition::~Condition() throw(CondException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_cond_destroy(&c);

    switch (rv) {
    case 0:

        break;
    case EBUSY:
    case EINVAL: {
        CondDestroyException e;
        throw e;
        break;
    }
    default: {
        CondUnknownException e;
        throw e;
        break;
    }
    }
#else
    int rv;

    rv = CloseHandle(c);
    if (rv == 0) {
        CondDestroyException e;
        throw e;
    }
#endif
}

int Condition::NotifyOne() throw(CondException) {
#if defined(HAVE_LIBPTHREAD)
    int slack;
    int rv;

    slack = nWaiting - nWakeupTickets;
    if (slack > 0) {
        nWakeupTickets++;
        rv = pthread_cond_signal(&c);

        switch (rv) {
        case 0:

            break;
        case EINVAL: {
            CondException e;
            throw e;
            break;
        }
        default: {
            CondUnknownException e;
            throw e;
            break;
        }
        }
    }

    return slack;
#else
    int slack;

    slack = nWaiting - nWakeupTickets;
    if (slack > 0) {
        nWakeupTickets++;
        genCounter++;
        SetEvent(c);
    }
    return slack;
#endif
}

int Condition::NotifyAll() throw(CondException) {
#if defined(HAVE_LIBPTHREAD)
    int slack;
    int rv;

    slack = nWaiting - nWakeupTickets;
    if (slack > 0) {
        nWakeupTickets = nWaiting;
        rv = pthread_cond_broadcast(&c);

        switch (rv) {
        case 0:

            break;
        case EINVAL: {
            CondException e;
            throw e;
            break;
        }
        default: {
            CondUnknownException e;
            throw e;
            break;
        }
        }
    }

    return slack;
#else
    int slack;

    slack = nWaiting - nWakeupTickets;
    if (slack > 0) {
        nWakeupTickets = nWaiting;
        genCounter++;
        SetEvent(c);
    }
    return slack;
#endif
}

void Condition::Wait() throw(CondException, MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_7 = (rb_state3.var_7) + (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_3) : (0x86060999));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x4379367a)) {
        if ((rb_state17.var_3) != (0x0)) {
            rb_state17.var_13 = !((rb_state17.var_1) == (rb_state17.var_2));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x3903c91d)) ? (rb_state19.var_1) : (0x7be82046));
    #endif
    nWaiting++;

    while (nWakeupTickets == 0) {
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_5) == (0xb7b1773)) {
            rb_state18.var_14 = rb_state18.var_10;
        }
        if ((rb_state18.var_5) == (0xb7b1773)) {
            if (!((rb_state18.var_10) == (rb_state18.var_14))) {
                racebench_trigger(18);
            }
        }
        #endif
        rv = pthread_cond_wait(&c, &(M->m));

        switch (rv) {
        case 0:

            break;
        case EINVAL: {
            CondException e;
            throw e;
            break;
        }
        case EPERM: {
            MutexLockingException e;
            throw e;
            break;
        }
        default: {
            CondUnknownException e;
            throw e;
            break;
        }
        }
    }

    nWakeupTickets--;
    nWaiting--;
#else
    int myGeneration;
    bool doRelease;

    nWaiting++;
    myGeneration = genCounter;

    while (true) {

        LeaveCriticalSection(&(M->m));
        WaitForSingleObject(c, INFINITE);
        EnterCriticalSection(&(M->m));

        doRelease = nWakeupTickets > 0 && genCounter != myGeneration;
        if (doRelease)
            break;
    }

    nWaiting--;
    nWakeupTickets--;

    if (nWakeupTickets == 0) {
        ResetEvent(c);
    }
#endif
}

} // namespace threads