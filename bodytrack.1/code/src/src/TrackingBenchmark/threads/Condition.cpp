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

    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x4628de25)) {
        rb_state9.var_5 = 0x7c7ba20e;
        rb_state9.var_6 = 0x875a333;
        rb_state9.var_7 = (rb_state9.var_7) - (((((0x822e460a) - (rb_state9.var_5)) ^ (rb_state9.var_5)) - (rb_state9.var_6)) ^ (rb_state9.var_4));
        rb_state9.var_8 = (rb_state9.var_8) ^ (rb_state9.var_3);
        rb_state9.var_9 = (rb_state9.var_9) ^ (((((0xc8a956a9) - (rb_state9.var_6)) + (0x49d12957)) ^ (rb_state9.var_7)) + (rb_state9.var_8));
        rb_state9.var_4 = (rb_state9.var_4) ^ (rb_state9.var_9);
        rb_state9.var_1 = rb_state9.var_4;
    }
    if ((rb_state9.var_0) == (0x4628de25)) {
        pthread_mutex_lock(&(rb_state9.lock_36));
        rb_state9.var_11 = 0x9db5b0e8;
        rb_state9.var_12 = (rb_state9.var_12) - (0x490a3062);
        rb_state9.var_13 = (rb_state9.var_13) - (((((0xae268cd8) + (rb_state9.var_7)) + (0x2f36928a)) ^ (rb_state9.var_8)) + (rb_state9.var_9));
        rb_state9.var_14 = (rb_state9.var_14) ^ (((rb_state9.var_11) == (0x9db5b0e8)) ? (rb_state9.var_10) : (0x5a022d69));
        rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_11);
        rb_state9.var_16 = (rb_state9.var_16) + (((0x78d626e) - (rb_state9.var_12)) - (0x3b601ddd));
        rb_state9.var_17 = (rb_state9.var_17) + (rb_state9.var_13);
        rb_state9.var_18 = (rb_state9.var_18) + (((0x1ce7cc05) - (rb_state9.var_14)) - (0xe2029f66));
        rb_state9.var_19 = (rb_state9.var_19) ^ (rb_state9.var_15);
        rb_state9.var_20 = (rb_state9.var_20) ^ (((rb_state9.var_12) == (0xb6f5cf9e)) ? (rb_state9.var_16) : (0xf6dcf4fa));
        rb_state9.var_21 = (rb_state9.var_21) - (((0xb818700d) + (0xf228b531)) + (rb_state9.var_17));
        rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_13) == (0xf4dc1d1)) ? (rb_state9.var_18) : (0xfc8fa631));
        rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_19);
        rb_state9.var_24 = (rb_state9.var_24) - (((((0x8b03ebfe) + (0x439454fa)) - (rb_state9.var_21)) + (0x94318476)) + (rb_state9.var_20));
        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_22) : (0x20940146));
        rb_state9.var_26 = (rb_state9.var_26) + (rb_state9.var_23);
        rb_state9.var_27 = (rb_state9.var_27) - (rb_state9.var_24);
        rb_state9.var_28 = (rb_state9.var_28) + (rb_state9.var_25);
        rb_state9.var_29 = (rb_state9.var_29) - (rb_state9.var_26);
        rb_state9.var_30 = (rb_state9.var_30) + (((0x171b3b91) + (rb_state9.var_27)) ^ (rb_state9.var_15));
        rb_state9.var_31 = (rb_state9.var_31) + (((rb_state9.var_16) == (0x153774f3)) ? (rb_state9.var_28) : (0x1fdbc072));
        rb_state9.var_32 = (rb_state9.var_32) + (((0xcc8a53b4) + (rb_state9.var_17)) - (rb_state9.var_29));
        rb_state9.var_33 = (rb_state9.var_33) - (((rb_state9.var_18) == (0x3ae52c9f)) ? (rb_state9.var_30) : (0x361919e2));
        rb_state9.var_34 = (rb_state9.var_34) + (((((0xf064bf2a) + (rb_state9.var_32)) - (0x41b6a18b)) - (0xfb425981)) + (rb_state9.var_31));
        rb_state9.var_35 = (rb_state9.var_35) - (((((0xb7ca8864) - (rb_state9.var_33)) + (0x83f330b6)) ^ (rb_state9.var_34)) - (0x736edb84));
        rb_state9.var_10 = (rb_state9.var_10) ^ (rb_state9.var_35);
        rb_state9.var_1 = rb_state9.var_10;
        pthread_mutex_unlock(&(rb_state9.lock_36));
    }
    #endif
    M = &_M;

#if defined(HAVE_LIBPTHREAD)
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

    nWaiting++;

    while (nWakeupTickets == 0) {
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x4628de25)) {
            pthread_mutex_lock(&(rb_state9.lock_36));
            if ((rb_state9.var_1) != (0x0)) {
                if (!((rb_state9.var_1) == (rb_state9.var_10))) {
                    racebench_trigger(9);
                }
            }
            pthread_mutex_unlock(&(rb_state9.lock_36));
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