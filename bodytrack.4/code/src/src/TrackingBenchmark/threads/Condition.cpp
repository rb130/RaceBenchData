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
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_3) == (0xcb617a98)) {
        pthread_mutex_lock(&(rb_state17.lock_21));
        rb_state17.var_6 = (rb_state17.var_6) + (((((0x1114f9e7) + (rb_state17.var_6)) + (rb_state17.var_4)) - (rb_state17.var_5)) + (0x3381acf4));
        rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_6);
        rb_state17.var_4 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_21));
    }
    if ((rb_state17.var_3) == (0xcb617a98)) {
        if (!((rb_state17.var_4) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
    }
    #endif
    nWaiting = 0;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (0x7916d73);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0xffffff40)) {
        if ((rb_state19.var_3) != (0x0)) {
            if (!((rb_state19.var_3) == (rb_state19.var_29))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    nWakeupTickets = 0;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0x7877f6e7)) {
        rb_state0.var_4 = rb_state0.var_2;
    }
    if ((rb_state0.var_1) == (0x7877f6e7)) {
        if (!((rb_state0.var_2) == (rb_state0.var_4))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x4f1585bf)) {
        rb_state16.var_10 = (rb_state16.var_10) + (((((0xdc5915ea) ^ (rb_state16.var_10)) ^ (rb_state16.var_6)) - (0x716b5ea1)) + (rb_state16.var_8));
        rb_state16.var_11 = (rb_state16.var_11) - (((((0x6b5fa153) - (0xfdc4aa6b)) - (rb_state16.var_11)) ^ (rb_state16.var_9)) + (rb_state16.var_10));
        rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_11);
        rb_state16.var_4 = rb_state16.var_9;
    }
    if ((rb_state16.var_2) == (0x4f1585bf)) {
        pthread_mutex_lock(&(rb_state16.lock_15));
        rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_12) : (0x2c8f8174));
        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_13) : (0x2e15a71f));
        rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_14);
        rb_state16.var_4 = rb_state16.var_12;
        pthread_mutex_unlock(&(rb_state16.lock_15));
    }
    #endif
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

    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x38)) {
        rb_state11.var_15 = 0x84bf0464;
        rb_state11.var_16 = 0xe9e902de;
        rb_state11.var_17 = (rb_state11.var_17) - (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_11) : (0x7231e308));
        rb_state11.var_18 = (rb_state11.var_18) + (0xde18af95);
        rb_state11.var_19 = (rb_state11.var_19) - (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_15) : (0x567d8a02));
        rb_state11.var_20 = (rb_state11.var_20) ^ (rb_state11.var_10);
        rb_state11.var_21 = (rb_state11.var_21) + (((((0x2d1127b) ^ (0x580003)) + (rb_state11.var_17)) + (rb_state11.var_18)) + (0x8e005e22));
        rb_state11.var_22 = (rb_state11.var_22) + (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0x82775bce));
        rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_21) == (0x0)) ? (rb_state11.var_22) : (0xd8b1459));
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_23);
        rb_state11.var_3 = rb_state11.var_14;
    }
    if ((rb_state11.var_2) == (0x38)) {
        pthread_mutex_lock(&(rb_state11.lock_28));
        rb_state11.var_25 = 0xb3d10b5b;
        rb_state11.var_26 = (rb_state11.var_26) - (((((0xb9d4be73) ^ (rb_state11.var_14)) ^ (rb_state11.var_25)) ^ (0xf5f382ef)) ^ (0xbd355966));
        rb_state11.var_27 = (rb_state11.var_27) ^ (((((0x49ba5fe7) + (rb_state11.var_13)) - (rb_state11.var_16)) ^ (rb_state11.var_15)) ^ (rb_state11.var_26));
        rb_state11.var_24 = (rb_state11.var_24) - (rb_state11.var_27);
        rb_state11.var_3 = rb_state11.var_24;
        pthread_mutex_unlock(&(rb_state11.lock_28));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) ^ ((0x79010b08) + (0xbce989fb));
    #endif
    nWaiting++;

    while (nWakeupTickets == 0) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_3 = (rb_state16.var_3) + (((rb_state16.var_2) == (0x4f158588)) ? (rb_state16.var_3) : (0xd3acc60a));
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_3) == (0xcb617a98)) {
            rb_state17.var_7 = 0xd720118e;
            rb_state17.var_8 = 0xa0d66319;
            rb_state17.var_9 = 0x23ae1783;
            rb_state17.var_10 = (rb_state17.var_10) - (rb_state17.var_8);
            rb_state17.var_11 = (rb_state17.var_11) ^ (rb_state17.var_7);
            rb_state17.var_12 = (rb_state17.var_12) + (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_9) : (0x4664cce3));
            rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_10);
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_11) : (0xe7ee9276));
            rb_state17.var_15 = (rb_state17.var_15) - (rb_state17.var_12);
            rb_state17.var_16 = (rb_state17.var_16) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_13) : (0xf8939eee));
            rb_state17.var_17 = (rb_state17.var_17) - (((0x1d6049f5) - (rb_state17.var_14)) - (0xbf2ebe30));
            rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_16) : (0xff011405));
            rb_state17.var_19 = (rb_state17.var_19) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_17) : (0xdacf06c3));
            rb_state17.var_20 = (rb_state17.var_20) - (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_19) : (0x46667e2b));
            rb_state17.var_4 = (rb_state17.var_4) + (rb_state17.var_20);
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