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

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_8 = (rb_state3.var_8) + ((0x7e177d35) + (rb_state3.var_6));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x35a45b97)) {
        pthread_mutex_lock(&(rb_state7.lock_25));
        rb_state7.var_10 = 0xfdb02b59;
        rb_state7.var_11 = 0xadc001e7;
        rb_state7.var_12 = (rb_state7.var_12) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_7) : (0x5fbece6f));
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_11) : (0xaf6ba50));
        rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_6) : (0x514203c));
        rb_state7.var_15 = (rb_state7.var_15) ^ (rb_state7.var_10);
        rb_state7.var_16 = (rb_state7.var_16) + (((0xdad488dc) ^ (rb_state7.var_12)) ^ (rb_state7.var_11));
        rb_state7.var_17 = (rb_state7.var_17) - (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_14) : (0xb7116e90));
        rb_state7.var_18 = (rb_state7.var_18) - (((0x21b30f8c) - (rb_state7.var_12)) ^ (rb_state7.var_15));
        rb_state7.var_19 = (rb_state7.var_19) ^ (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_16) : (0x740e087));
        rb_state7.var_20 = (rb_state7.var_20) ^ (((((0xf544f28a) ^ (rb_state7.var_17)) + (0x20aaa7)) ^ (rb_state7.var_14)) + (rb_state7.var_18));
        rb_state7.var_21 = (rb_state7.var_21) ^ (rb_state7.var_19);
        rb_state7.var_22 = (rb_state7.var_22) ^ (rb_state7.var_20);
        rb_state7.var_23 = (rb_state7.var_23) ^ (((0x4306f22) ^ (rb_state7.var_21)) + (0xf8e2eba9));
        rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_22) == (0x0)) ? (rb_state7.var_23) : (0x3f33f2b2));
        rb_state7.var_3 = (rb_state7.var_3) ^ (rb_state7.var_24);
        pthread_mutex_unlock(&(rb_state7.lock_25));
    }
    #endif
    M = &_M;

#if defined(HAVE_LIBPTHREAD)
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_19 = (rb_state12.var_19) - (rb_state12.var_20);
    #endif
    nWaiting = 0;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_2) == (0xaf8a0c2a)) ? (rb_state7.var_1) : (0xa9fede7d));
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_21) == (0xd6be4402)) {
        pthread_mutex_lock(&(rb_state12.lock_51));
        rb_state12.var_42 = 0xf70022d8;
        rb_state12.var_43 = (rb_state12.var_43) ^ (rb_state12.var_42);
        rb_state12.var_44 = (rb_state12.var_44) - (((0xca3410c4) ^ (0x179cf6b6)) + (rb_state12.var_24));
        rb_state12.var_45 = (rb_state12.var_45) - (((rb_state12.var_25) == (0x6657feb4)) ? (rb_state12.var_23) : (0xc9152c8f));
        rb_state12.var_46 = (rb_state12.var_46) + (rb_state12.var_43);
        rb_state12.var_47 = (rb_state12.var_47) - (((rb_state12.var_26) == (0xd6be4402)) ? (rb_state12.var_44) : (0x26b1666f));
        rb_state12.var_48 = (rb_state12.var_48) + (rb_state12.var_45);
        rb_state12.var_49 = (rb_state12.var_49) + (((((0xd70f58fe) ^ (rb_state12.var_47)) - (rb_state12.var_46)) ^ (0xb087a912)) ^ (rb_state12.var_27));
        rb_state12.var_50 = (rb_state12.var_50) - (((rb_state12.var_48) == (0x0)) ? (rb_state12.var_49) : (0x736934e2));
        rb_state12.var_35 = (rb_state12.var_35) ^ (rb_state12.var_50);
        pthread_mutex_unlock(&(rb_state12.lock_51));
    }
    #endif
    nWakeupTickets = 0;

    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x9679bfb9)) {
        rb_state17.var_3 = rb_state17.var_2;
    }
    if ((rb_state17.var_1) == (0x9679bfb9)) {
        if (!((rb_state17.var_2) == (rb_state17.var_3))) {
            racebench_trigger(17);
        }
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

    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0xe0e788dc)) {
        pthread_mutex_lock(&(rb_state8.lock_15));
        rb_state8.var_4 = 0x1f75f3ba;
        rb_state8.var_5 = 0xc6754113;
        rb_state8.var_6 = (rb_state8.var_6) ^ ((0xb3663149) ^ (0x328cf308));
        rb_state8.var_7 = (rb_state8.var_7) - (((0x9f6365a) ^ (rb_state8.var_4)) - (0x91b423f1));
        rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_5);
        rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_3) : (0x302db054));
        rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0x1da8871b));
        rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_8) : (0x4c36efd4));
        rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_9) : (0x803c9cab));
        rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x290d03a5));
        rb_state8.var_14 = (rb_state8.var_14) + (((((0x9b983083) - (0x918ea76e)) ^ (rb_state8.var_13)) - (rb_state8.var_12)) + (0xbbf14860));
        rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_14);
        pthread_mutex_unlock(&(rb_state8.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x77721f93)) {
        pthread_mutex_lock(&(rb_state19.lock_38));
        rb_state19.var_12 = 0x4c9856e7;
        rb_state19.var_13 = (rb_state19.var_13) + (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_7) : (0x7654df41));
        rb_state19.var_14 = (rb_state19.var_14) + (rb_state19.var_12);
        rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0x26d8c2b));
        rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_13);
        rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0x64ddf5f5));
        rb_state19.var_18 = (rb_state19.var_18) + (((0x3362f794) - (rb_state19.var_16)) ^ (0x28a86d69));
        rb_state19.var_19 = (rb_state19.var_19) ^ (rb_state19.var_17);
        rb_state19.var_20 = (rb_state19.var_20) ^ (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_18) : (0x40d3e957));
        rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_19) : (0x96de3aa7));
        rb_state19.var_22 = (rb_state19.var_22) ^ (rb_state19.var_20);
        rb_state19.var_23 = (rb_state19.var_23) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_21) : (0xb435a65a));
        rb_state19.var_24 = (rb_state19.var_24) - (((0x666099ae) + (0x5c9e0764)) + (rb_state19.var_22));
        rb_state19.var_25 = (rb_state19.var_25) - (((((0x41bb4ae9) - (0xc1350730)) - (rb_state19.var_13)) + (rb_state19.var_24)) ^ (rb_state19.var_23));
        rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_25);
        rb_state19.var_3 = rb_state19.var_11;
        pthread_mutex_unlock(&(rb_state19.lock_38));
    }
    if ((rb_state19.var_2) == (0x77721f93)) {
        pthread_mutex_lock(&(rb_state19.lock_38));
        rb_state19.var_27 = 0x7bcaeb9;
        rb_state19.var_28 = 0xfba2577e;
        rb_state19.var_29 = 0x27a1309b;
        rb_state19.var_30 = (rb_state19.var_30) + (((0xb883c556) - (rb_state19.var_27)) ^ (0x37b8a497));
        rb_state19.var_31 = (rb_state19.var_31) + (rb_state19.var_29);
        rb_state19.var_32 = (rb_state19.var_32) - (((0x6f62dc6e) + (rb_state19.var_28)) + (rb_state19.var_14));
        rb_state19.var_33 = (rb_state19.var_33) ^ (((((0xd075017b) - (0xcf307319)) ^ (0xa369aa90)) - (rb_state19.var_31)) - (rb_state19.var_30));
        rb_state19.var_34 = (rb_state19.var_34) - (((0xe9322adc) + (rb_state19.var_32)) ^ (0x62280381));
        rb_state19.var_35 = (rb_state19.var_35) - (((0xa40726c3) ^ (rb_state19.var_33)) - (rb_state19.var_15));
        rb_state19.var_36 = (rb_state19.var_36) + (((0x4cbd2d10) - (rb_state19.var_34)) ^ (0x5756ae14));
        rb_state19.var_37 = (rb_state19.var_37) - (((((0x1c05fb67) + (rb_state19.var_36)) ^ (rb_state19.var_17)) - (rb_state19.var_35)) ^ (rb_state19.var_16));
        rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_37);
        rb_state19.var_3 = rb_state19.var_26;
        pthread_mutex_unlock(&(rb_state19.lock_38));
    }
    #endif
    nWaiting++;

    while (nWakeupTickets == 0) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_17 = (rb_state12.var_17) ^ (rb_state12.var_18);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + (656 < rb_input_size ? (uint32_t)rb_input[656] : 0x9e69a48c);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ (0xdaa7acc6);
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