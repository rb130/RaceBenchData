#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(HAVE_LIBPTHREAD)
#include <pthread.h>
#include <errno.h>
#else

#endif

#include <exception>

#include "Mutex.h"

namespace threads {

Mutex::Mutex() throw(MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_mutex_init(&m, NULL);

    switch (rv) {
    case 0:
        break;
    case EAGAIN:
    case ENOMEM: {
        MutexResourceException e;
        throw e;
        break;
    }
    case EPERM:
    case EBUSY:
    case EINVAL: {
        MutexInitException e;
        throw e;
        break;
    }
    default: {
        MutexUnknownException e;
        throw e;
        break;
    }
    }
#else
    InitializeCriticalSection(&m);
#endif
}

Mutex::~Mutex() throw(MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_mutex_destroy(&m);

    switch (rv) {
    case 0:
        break;
    case EBUSY:
    case EINVAL: {
        MutexDestroyException e;
        throw e;
        break;
    }
    default: {
        MutexUnknownException e;
        throw e;
        break;
    }
    }
#else
    DeleteCriticalSection(&m);
#endif
}

void Mutex::Lock() throw(MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state1.lock_30));
        rb_state1.var_3 = 0x525df56d;
        rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_3);
        rb_state1.var_5 = (rb_state1.var_5) ^ (805 < rb_input_size ? (uint32_t)rb_input[805] : 0x851f09ba);
        rb_state1.var_6 = (rb_state1.var_6) ^ (((((0x86748e3b) - (rb_state1.var_2)) + (0x48e50bf2)) - (rb_state1.var_4)) + (rb_state1.var_1));
        rb_state1.var_7 = (rb_state1.var_7) - (((rb_state1.var_3) == (0x525df56d)) ? (rb_state1.var_4) : (0xe7417943));
        rb_state1.var_8 = (rb_state1.var_8) + (((0xabb024bb) - (rb_state1.var_5)) - (rb_state1.var_5));
        rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_6) == (0x7cfba4c0)) ? (rb_state1.var_6) : (0x44e8420d));
        rb_state1.var_10 = (rb_state1.var_10) ^ (rb_state1.var_7);
        rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_7) == (0xada20a93)) ? (rb_state1.var_8) : (0xe54c2950));
        rb_state1.var_12 = (rb_state1.var_12) - (((0x6697b1ad) - (rb_state1.var_9)) ^ (0xa384dbdc));
        rb_state1.var_13 = (rb_state1.var_13) - (((0xb6b7e6f4) + (0x8515d2a4)) + (rb_state1.var_10));
        rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_8) == (0xabb0245b)) ? (rb_state1.var_11) : (0xc29d1a8d));
        rb_state1.var_15 = (rb_state1.var_15) ^ (rb_state1.var_12);
        rb_state1.var_16 = (rb_state1.var_16) ^ (rb_state1.var_13);
        rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_9) == (0x7cfba4c0)) ? (rb_state1.var_14) : (0x762ead5d));
        rb_state1.var_18 = (rb_state1.var_18) + (((rb_state1.var_10) == (0xada20a93)) ? (rb_state1.var_15) : (0xb6d21213));
        rb_state1.var_19 = (rb_state1.var_19) - (((rb_state1.var_11) == (0x544fdba5)) ? (rb_state1.var_16) : (0x742c74b5));
        rb_state1.var_20 = (rb_state1.var_20) + (rb_state1.var_17);
        rb_state1.var_21 = (rb_state1.var_21) - (((rb_state1.var_18) == (0xb5e728cf)) ? (rb_state1.var_19) : (0x25c316d4));
        rb_state1.var_22 = (rb_state1.var_22) + (((0xf08dfad1) ^ (rb_state1.var_12)) ^ (rb_state1.var_20));
        rb_state1.var_23 = (rb_state1.var_23) + (((0x1801a2d2) - (0xd283b43)) - (rb_state1.var_21));
        rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_22);
        rb_state1.var_25 = (rb_state1.var_25) - (((0xebfe9f6d) + (rb_state1.var_23)) - (rb_state1.var_13));
        rb_state1.var_26 = (rb_state1.var_26) ^ (rb_state1.var_24);
        rb_state1.var_27 = (rb_state1.var_27) + (rb_state1.var_25);
        rb_state1.var_28 = (rb_state1.var_28) + (((0xf2044167) - (0x2d5e1b54)) + (rb_state1.var_26));
        rb_state1.var_29 = (rb_state1.var_29) + (((rb_state1.var_27) == (0x364870ae)) ? (rb_state1.var_28) : (0x2c12118e));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_29);
        rb_state1.var_2 = rb_state1.var_1;
        pthread_mutex_unlock(&(rb_state1.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (573 < rb_input_size ? (uint32_t)rb_input[573] : 0xa69ed704);
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x8c)) {
        rb_state10.var_9 = 0x7f6df841;
        rb_state10.var_10 = 0x7ab83c0f;
        rb_state10.var_11 = (rb_state10.var_11) - (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_10) : (0x3df9f187));
        rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_11) : (0x7a6e38e8));
        rb_state10.var_8 = (rb_state10.var_8) + (rb_state10.var_12);
        rb_state10.var_1 = rb_state10.var_8;
    }
    if ((rb_state10.var_0) == (0x8c)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_14 = 0x6c19388e;
        rb_state10.var_15 = 0xd8cd7f0c;
        rb_state10.var_16 = (rb_state10.var_16) ^ (((0xe21b3e4c) - (0xb7f772bf)) ^ (rb_state10.var_15));
        rb_state10.var_17 = (rb_state10.var_17) - (0x4571b30);
        rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_14);
        rb_state10.var_19 = (rb_state10.var_19) + (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0x63e800ac));
        rb_state10.var_20 = (rb_state10.var_20) ^ (rb_state10.var_16);
        rb_state10.var_21 = (rb_state10.var_21) + (((0x5e96beb8) - (rb_state10.var_7)) - (rb_state10.var_17));
        rb_state10.var_22 = (rb_state10.var_22) + (((0x56e02693) ^ (rb_state10.var_18)) - (rb_state10.var_8));
        rb_state10.var_23 = (rb_state10.var_23) ^ (((((0x5583c9b4) - (rb_state10.var_9)) - (rb_state10.var_20)) ^ (0x6554120f)) - (rb_state10.var_19));
        rb_state10.var_24 = (rb_state10.var_24) - (rb_state10.var_21);
        rb_state10.var_25 = (rb_state10.var_25) - (((((0xda18f10d) + (rb_state10.var_23)) - (0x65f13ed4)) + (rb_state10.var_10)) ^ (rb_state10.var_22));
        rb_state10.var_26 = (rb_state10.var_26) + (rb_state10.var_24);
        rb_state10.var_27 = (rb_state10.var_27) + (((0x4218371b) - (rb_state10.var_25)) ^ (rb_state10.var_11));
        rb_state10.var_28 = (rb_state10.var_28) + (((((0x6bd172ba) + (rb_state10.var_27)) - (rb_state10.var_13)) ^ (rb_state10.var_26)) ^ (rb_state10.var_12));
        rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_28);
        rb_state10.var_1 = rb_state10.var_13;
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    #endif
    rv = pthread_mutex_lock(&m);

    switch (rv) {
    case 0:

        break;
    case EINVAL:
    case EAGAIN: {
        MutexLockingException e;
        throw e;
        break;
    }
    case EDEADLK: {
        MutexDeadlockException e;
        throw e;
        break;
    }
    default: {
        MutexUnknownException e;
        throw e;
        break;
    }
    }
#else
    EnterCriticalSection(&m);
#endif
}

bool Mutex::TryLock() throw(MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    rv = pthread_mutex_trylock(&m);

    switch (rv) {
    case 0:

        break;
    case EBUSY:

        return false;
        break;
    case EINVAL:
    case EAGAIN: {
        MutexLockingException e;
        throw e;
        break;
    }
    default: {
        MutexUnknownException e;
        throw e;
        break;
    }
    }

    return true;
#else
    return TryEnterCriticalSection(&m);
#endif
}

void Mutex::Unlock() throw(MutexException) {
#if defined(HAVE_LIBPTHREAD)
    int rv;

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0xa0c0c950)) {
        rb_state0.var_25 = 0xd712087c;
        rb_state0.var_26 = 0x127f90d5;
        rb_state0.var_27 = (rb_state0.var_27) - (((rb_state0.var_26) == (0x127f90d5)) ? (rb_state0.var_9) : (0x3a46ff31));
        rb_state0.var_28 = (rb_state0.var_28) + (((((0xfafeb2d0) - (0xab375fba)) + (rb_state0.var_27)) + (rb_state0.var_25)) ^ (0x54163790));
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x0)) {
        rb_state4.var_2 = rb_state4.var_1;
    }
    if ((rb_state4.var_0) == (0x0)) {
        if (!((rb_state4.var_1) == (rb_state4.var_2))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x4aa3ebeb)) {
        rb_state15.var_6 = 0x5b5bb10c;
        rb_state15.var_7 = 0x3b3e7c49;
        rb_state15.var_8 = 0x2b5548ad;
        rb_state15.var_9 = (rb_state15.var_9) + (((0x50d406e5) ^ (0x4b9fa2af)) + (rb_state15.var_8));
        rb_state15.var_10 = (rb_state15.var_10) ^ (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_7) : (0xe2888082));
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_9) : (0x97a32611));
        rb_state15.var_12 = (rb_state15.var_12) - (((((0xd4a79ab5) - (rb_state15.var_10)) ^ (0x7c3e8f2f)) + (rb_state15.var_11)) - (0x3ed04f0c));
        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_12);
        rb_state15.var_1 = rb_state15.var_5;
    }
    if ((rb_state15.var_0) == (0x4aa3ebeb)) {
        pthread_mutex_lock(&(rb_state15.lock_20));
        rb_state15.var_14 = 0xb8812337;
        rb_state15.var_15 = 0x1c3b6937;
        rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_14) : (0x724b7cfa));
        rb_state15.var_17 = (rb_state15.var_17) ^ (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_15) : (0xe410cf52));
        rb_state15.var_18 = (rb_state15.var_18) - (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_16) : (0x5ec8a51b));
        rb_state15.var_19 = (rb_state15.var_19) + (((((0x9f3d420) + (rb_state15.var_18)) - (rb_state15.var_8)) - (rb_state15.var_17)) ^ (rb_state15.var_9));
        rb_state15.var_13 = (rb_state15.var_13) + (rb_state15.var_19);
        rb_state15.var_1 = rb_state15.var_13;
        pthread_mutex_unlock(&(rb_state15.lock_20));
    }
    #endif
    rv = pthread_mutex_unlock(&m);

    switch (rv) {
    case 0:

        break;
    case EINVAL:
    case EAGAIN:
    case EPERM: {
        MutexLockingException e;
        throw e;
        break;
    }
    default: {
        MutexUnknownException e;
        throw e;
        break;
    }
    }
#else
    LeaveCriticalSection(&m);
#endif
}

} // namespace threads