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

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_2);
    #endif
    M = &_M;

#if defined(HAVE_LIBPTHREAD)
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + ((0xd2492f2e) + (rb_state6.var_2));
    #endif
    nWaiting = 0;
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

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_0) == (0x1abf2d78)) ? (rb_state9.var_1) : (0xdef370a2));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x5bce8c0e)) {
        if ((rb_state10.var_4) != (0x0)) {
            if (!((rb_state10.var_4) == (rb_state10.var_28))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x1d0927b2)) {
        pthread_mutex_lock(&(rb_state13.lock_13));
        rb_state13.var_5 = 0x1efca1cb;
        rb_state13.var_6 = (rb_state13.var_6) - ((0x9d441ce9) + (0x1fe585f4));
        rb_state13.var_7 = (rb_state13.var_7) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_5) : (0x502d82a1));
        rb_state13.var_8 = (rb_state13.var_8) - (((((0xbc047c29) ^ (0x49ef3159)) - (rb_state13.var_4)) + (0x9a252700)) - (rb_state13.var_5));
        rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_7) : (0x1ccf4749));
        rb_state13.var_10 = (rb_state13.var_10) - (((0xa926b6b) + (rb_state13.var_7)) ^ (rb_state13.var_8));
        rb_state13.var_11 = (rb_state13.var_11) - (rb_state13.var_9);
        rb_state13.var_12 = (rb_state13.var_12) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_11) : (0xe0d800c5));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_12);
        pthread_mutex_unlock(&(rb_state13.lock_13));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_0);
    #endif
    nWaiting++;

    while (nWakeupTickets == 0) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_3);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_4 = (rb_state6.var_4) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0x78657082));
        if ((rb_state6.var_2) == (0x54b2a3e6)) {
            if ((rb_state6.var_5) != (0x0)) {
                if (!((rb_state6.var_5) == (rb_state6.var_27))) {
                    racebench_trigger(6);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state18.lock_14));
            rb_state18.var_4 = rb_state18.var_1;
            pthread_mutex_unlock(&(rb_state18.lock_14));
        }
        if ((rb_state18.var_0) == (0x0)) {
            if (!((rb_state18.var_1) == (rb_state18.var_4))) {
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