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

    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_6) == (0xb5a88f01)) {
        rb_state14.var_26 = 0xce0f7abe;
        rb_state14.var_27 = 0xbee8c28;
        rb_state14.var_28 = (rb_state14.var_28) + (1336 < rb_input_size ? (uint32_t)rb_input[1336] : 0xf46f3cca);
        rb_state14.var_29 = (rb_state14.var_29) - (rb_state14.var_19);
        rb_state14.var_30 = (rb_state14.var_30) + (((rb_state14.var_26) == (0xce0f7abe)) ? (rb_state14.var_27) : (0xa3badf3a));
        rb_state14.var_31 = (rb_state14.var_31) + (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_28) : (0x3fa91cf));
        rb_state14.var_32 = (rb_state14.var_32) + (((rb_state14.var_21) == (0x0)) ? (rb_state14.var_29) : (0xab97887b));
        rb_state14.var_33 = (rb_state14.var_33) + (((((0xcbf6363b) + (rb_state14.var_22)) ^ (0xcb42f473)) - (rb_state14.var_31)) ^ (rb_state14.var_30));
        rb_state14.var_34 = (rb_state14.var_34) ^ (((rb_state14.var_32) == (0x0)) ? (rb_state14.var_33) : (0x85bac8d5));
        rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_34);
        rb_state14.var_17 = rb_state14.var_25;
    }
    if ((rb_state14.var_6) == (0xb5a88f01)) {
        pthread_mutex_lock(&(rb_state14.lock_40));
        rb_state14.var_36 = 0xf6aab562;
        rb_state14.var_37 = (rb_state14.var_37) - (((0xa8215153) - (rb_state14.var_24)) - (rb_state14.var_25));
        rb_state14.var_38 = (rb_state14.var_38) ^ (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_36) : (0xca7c44e6));
        rb_state14.var_39 = (rb_state14.var_39) + (((((0xb2db69d7) + (rb_state14.var_38)) + (rb_state14.var_27)) - (rb_state14.var_26)) ^ (rb_state14.var_37));
        rb_state14.var_35 = (rb_state14.var_35) - (rb_state14.var_39);
        rb_state14.var_17 = rb_state14.var_35;
        pthread_mutex_unlock(&(rb_state14.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ (0x76fab29c);
    rb_state17.var_2 = (rb_state17.var_2) - (((rb_state17.var_2) == (0x76fab29c)) ? (rb_state17.var_1) : (0x31fee1c5));
    #endif
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
    if ((rb_state1.var_2) == (0x7da331b3)) {
        rb_state1.var_20 = 0xbe50eed5;
        rb_state1.var_21 = 0xf757ebb;
        rb_state1.var_22 = (rb_state1.var_22) + (((((0x811502ad) - (0x4ca91e56)) + (rb_state1.var_20)) ^ (rb_state1.var_21)) - (0xbbf9a929));
        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_22) : (0x608643b));
        rb_state1.var_19 = (rb_state1.var_19) ^ (rb_state1.var_23);
        rb_state1.var_3 = rb_state1.var_19;
    }
    if ((rb_state1.var_2) == (0x7da331b3)) {
        pthread_mutex_lock(&(rb_state1.lock_34));
        rb_state1.var_25 = 0x49ac1984;
        rb_state1.var_26 = 0xa0717925;
        rb_state1.var_27 = (rb_state1.var_27) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_25) : (0x8ddf3dc));
        rb_state1.var_28 = (rb_state1.var_28) + ((0xcdd8ee69) ^ (rb_state1.var_11));
        rb_state1.var_29 = (rb_state1.var_29) + (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_26) : (0x772bde5b));
        rb_state1.var_30 = (rb_state1.var_30) + (((0x510e49da) + (rb_state1.var_13)) - (rb_state1.var_27));
        rb_state1.var_31 = (rb_state1.var_31) ^ (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_28) : (0x3f99cb7a));
        rb_state1.var_32 = (rb_state1.var_32) - (((((0xac2ca17f) - (rb_state1.var_15)) - (rb_state1.var_29)) + (rb_state1.var_16)) - (rb_state1.var_30));
        rb_state1.var_33 = (rb_state1.var_33) ^ (((rb_state1.var_31) == (0x0)) ? (rb_state1.var_32) : (0x88223ad8));
        rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_33);
        rb_state1.var_3 = rb_state1.var_24;
        pthread_mutex_unlock(&(rb_state1.lock_34));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x854cf2b0)) {
        rb_state2.var_3 = rb_state2.var_0;
    }
    if ((rb_state2.var_2) == (0x854cf2b0)) {
        if (!((rb_state2.var_0) == (rb_state2.var_3))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0x6e04fa0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (947 < rb_input_size ? (uint32_t)rb_input[947] : 0x1373d61);
    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (0x9720a62c);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (0x683365df);
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_33));
        if ((rb_state12.var_3) != (0x0)) {
            rb_state12.var_34 = !((rb_state12.var_3) == (rb_state12.var_12));
        }
        pthread_mutex_unlock(&(rb_state12.lock_33));
    }
    if ((rb_state12.var_21) == (0xd6be4402)) {
        pthread_mutex_lock(&(rb_state12.lock_51));
        rb_state12.var_37 = 0x20dea81a;
        rb_state12.var_38 = 0x766238f5;
        rb_state12.var_39 = 0xd30af524;
        rb_state12.var_40 = (rb_state12.var_40) - (((((0x69ad3310) - (rb_state12.var_22)) ^ (rb_state12.var_37)) - (rb_state12.var_38)) - (0x9413c1d3));
        rb_state12.var_41 = (rb_state12.var_41) - (((rb_state12.var_39) == (0x0)) ? (rb_state12.var_40) : (0x9ce9a86d));
        rb_state12.var_36 = (rb_state12.var_36) + (rb_state12.var_41);
        rb_state12.var_35 = rb_state12.var_36;
        pthread_mutex_unlock(&(rb_state12.lock_51));
    }
    if ((rb_state12.var_21) == (0xd6be4402)) {
        if (!((rb_state12.var_35) == (rb_state12.var_36))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_11 = (rb_state14.var_11) - (((rb_state14.var_13) == (0x5430a6c7)) ? (rb_state14.var_12) : (0xd4356a63));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ ((0xd965f2e3) ^ (0x4f1c4d5a));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (233 < rb_input_size ? (uint32_t)rb_input[233] : 0x731136f0);
    if ((rb_state18.var_1) == (0x185f8047)) {
        rb_state18.var_11 = 0x53cb3c22;
        rb_state18.var_12 = 0x47815764;
        rb_state18.var_13 = (rb_state18.var_13) - (((0xe9cf7a5c) + (rb_state18.var_5)) - (0x5307bcf6));
        rb_state18.var_14 = (rb_state18.var_14) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_11) : (0x9d8c90a2));
        rb_state18.var_15 = (rb_state18.var_15) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_13) : (0xfcfd10eb));
        rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_14) : (0x3650165f));
        rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_16) : (0x494f7b84));
        rb_state18.var_10 = (rb_state18.var_10) ^ (rb_state18.var_17);
        rb_state18.var_2 = rb_state18.var_10;
    }
    if ((rb_state18.var_1) == (0x185f8047)) {
        pthread_mutex_lock(&(rb_state18.lock_25));
        rb_state18.var_19 = 0xb1dbcfed;
        rb_state18.var_20 = 0x44807eb6;
        rb_state18.var_21 = (rb_state18.var_21) ^ (((0xb9ce80cc) - (rb_state18.var_20)) ^ (rb_state18.var_9));
        rb_state18.var_22 = (rb_state18.var_22) ^ (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_19) : (0x83b0d62d));
        rb_state18.var_23 = (rb_state18.var_23) ^ (((0x7117c235) ^ (rb_state18.var_21)) - (0x81f2e279));
        rb_state18.var_24 = (rb_state18.var_24) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_23) : (0x8f38d65c));
        rb_state18.var_18 = (rb_state18.var_18) - (rb_state18.var_24);
        rb_state18.var_2 = rb_state18.var_18;
        pthread_mutex_unlock(&(rb_state18.lock_25));
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

    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_6) == (0xb5a88f01)) {
        if ((rb_state14.var_17) != (0x0)) {
            if (!((rb_state14.var_17) == (rb_state14.var_35))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (0xa405025f);
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