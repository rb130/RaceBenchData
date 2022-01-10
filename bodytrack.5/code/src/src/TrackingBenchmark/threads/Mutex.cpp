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

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_1);
    rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_2);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (0xe41f19e7);
    #endif
    rv = pthread_mutex_init(&m, NULL);

    switch (rv) {
    case 0:
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_1) : (0xc51a1d25));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) - (0x380678e0);
        #endif
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
    rb_state1.var_0 = (rb_state1.var_0) - (0xc4d8bcb9);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (337 < rb_input_size ? (uint32_t)rb_input[337] : 0xbb6d7d5d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (0x11463a62);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xdff6eee4)) ? (rb_state4.var_2) : (0x92bdfaac));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xabccf0d0));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x2837c14)) {
        rb_state7.var_3 = rb_state7.var_2;
    }
    if ((rb_state7.var_1) == (0x2837c14)) {
        if (!((rb_state7.var_2) == (rb_state7.var_3))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (0x75235af0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_2) : (0xe8ae7c1b));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_4 = (rb_state14.var_4) ^ (((rb_state14.var_2) == (0x1de2b19)) ? (rb_state14.var_4) : (0xfd3f971e));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_1);
    rb_state15.var_2 = (rb_state15.var_2) ^ (634 < rb_input_size ? (uint32_t)rb_input[634] : 0xb158da03);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_4 = (rb_state16.var_4) - (rb_state16.var_3);
    if ((rb_state16.var_2) == (0xffffffd7)) {
        if ((rb_state16.var_5) != (0x0)) {
            if (!((rb_state16.var_5) == (rb_state16.var_38))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_1) : (0xa4f710f5));
    #endif
    rv = pthread_mutex_lock(&m);

    switch (rv) {
    case 0:

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_1);
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_27));
            rb_state1.var_6 = 0x68a3fdb0;
            rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_4) : (0x8384d4d6));
            rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_7) : (0x3f23b1a3));
            rb_state1.var_5 = (rb_state1.var_5) - (rb_state1.var_8);
            rb_state1.var_4 = rb_state1.var_5;
            pthread_mutex_unlock(&(rb_state1.lock_27));
        }
        if ((rb_state1.var_3) == (0x0)) {
            if (!((rb_state1.var_4) == (rb_state1.var_5))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_0);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) + (0xb682c215);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_3) : (0x4b30fe79));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (1350 < rb_input_size ? (uint32_t)rb_input[1350] : 0x7e161fc6);
        rb_state5.var_2 = (rb_state5.var_2) + (393 < rb_input_size ? (uint32_t)rb_input[393] : 0x851d9f1f);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_0) == (0xd92d766f)) ? (rb_state8.var_2) : (0xed54b241));
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_0);
        rb_state8.var_1 = (rb_state8.var_1) - (1241 < rb_input_size ? (uint32_t)rb_input[1241] : 0x4474da0f);
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x1abf2d78)) {
            rb_state9.var_4 = rb_state9.var_1;
        }
        if ((rb_state9.var_0) == (0x1abf2d78)) {
            pthread_mutex_lock(&(rb_state9.lock_5));
            if (!((rb_state9.var_1) == (rb_state9.var_4))) {
                racebench_trigger(9);
            }
            pthread_mutex_unlock(&(rb_state9.lock_5));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_0) == (0x8)) ? (rb_state11.var_3) : (0xe1ec14c0));
        #endif
        #ifdef RACEBENCH_BUG_12
        if (!((rb_state12.var_1) == (rb_state12.var_5))) {
            rb_state12.var_10 = rb_state12.var_7;
        }
        if (!((rb_state12.var_1) == (rb_state12.var_5))) {
            pthread_mutex_lock(&(rb_state12.lock_24));
            if (!((rb_state12.var_7) == (rb_state12.var_10))) {
                racebench_trigger(12);
            }
            pthread_mutex_unlock(&(rb_state12.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_4 = (rb_state14.var_4) + (633 < rb_input_size ? (uint32_t)rb_input[633] : 0x283b4ea8);
        if ((rb_state14.var_0) == (0xaa40af4f)) {
            rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_5);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (874 < rb_input_size ? (uint32_t)rb_input[874] : 0x774ba806);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_4 = (rb_state19.var_4) - (rb_state19.var_3);
        #endif
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

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) - (0x56d53093);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (178 < rb_input_size ? (uint32_t)rb_input[178] : 0x35da611a);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0xffffffd7)) {
        rb_state16.var_22 = 0xb914d05e;
        rb_state16.var_23 = (rb_state16.var_23) - (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_12) : (0x4233ad83));
        rb_state16.var_24 = (rb_state16.var_24) - (rb_state16.var_14);
        rb_state16.var_25 = (rb_state16.var_25) ^ (((((0x6ab665ae) ^ (rb_state16.var_22)) - (rb_state16.var_11)) ^ (0x27e4a7d3)) - (rb_state16.var_15));
        rb_state16.var_26 = (rb_state16.var_26) + (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_10) : (0xdd71e3f5));
        rb_state16.var_27 = (rb_state16.var_27) + (((0x84deabc1) + (rb_state16.var_23)) ^ (rb_state16.var_17));
        rb_state16.var_28 = (rb_state16.var_28) - (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_24) : (0x1a42e1af));
        rb_state16.var_29 = (rb_state16.var_29) - (rb_state16.var_25);
        rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0x50d337ae));
        rb_state16.var_31 = (rb_state16.var_31) - (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_28) : (0x149e5fc3));
        rb_state16.var_32 = (rb_state16.var_32) + (((((0x4b9527e5) ^ (rb_state16.var_20)) + (0x8fadcf4f)) ^ (rb_state16.var_30)) ^ (rb_state16.var_29));
        rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_31) : (0xb175e49e));
        rb_state16.var_34 = (rb_state16.var_34) ^ (((0x6b714837) ^ (rb_state16.var_32)) - (0xe82327bb));
        rb_state16.var_35 = (rb_state16.var_35) + (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_33) : (0xe3ce3dc6));
        rb_state16.var_36 = (rb_state16.var_36) + (((0xf7d6a850) + (rb_state16.var_34)) ^ (rb_state16.var_23));
        rb_state16.var_37 = (rb_state16.var_37) ^ (((((0xf9c94c87) ^ (rb_state16.var_36)) - (0x4eca32bc)) - (rb_state16.var_35)) + (0x9321fafb));
        rb_state16.var_21 = (rb_state16.var_21) + (rb_state16.var_37);
        rb_state16.var_5 = rb_state16.var_21;
    }
    if ((rb_state16.var_2) == (0xffffffd7)) {
        pthread_mutex_lock(&(rb_state16.lock_50));
        rb_state16.var_39 = 0x44c56977;
        rb_state16.var_40 = (rb_state16.var_40) ^ (((((0x60812eec) ^ (rb_state16.var_26)) - (rb_state16.var_39)) + (0xebe9b7e9)) - (rb_state16.var_24));
        rb_state16.var_41 = (rb_state16.var_41) + (415 < rb_input_size ? (uint32_t)rb_input[415] : 0x89a9d987);
        rb_state16.var_42 = (rb_state16.var_42) + (707 < rb_input_size ? (uint32_t)rb_input[707] : 0x594cbf94);
        rb_state16.var_43 = (rb_state16.var_43) - (((0xa2ceb730) - (rb_state16.var_27)) ^ (rb_state16.var_25));
        rb_state16.var_44 = (rb_state16.var_44) ^ (((0xb07767db) - (rb_state16.var_28)) - (rb_state16.var_40));
        rb_state16.var_45 = (rb_state16.var_45) ^ (((((0x42de5fbd) ^ (rb_state16.var_42)) ^ (0x9387b8a2)) + (rb_state16.var_41)) + (rb_state16.var_29));
        rb_state16.var_46 = (rb_state16.var_46) - (((rb_state16.var_43) == (0x0)) ? (rb_state16.var_44) : (0x1ceaf4e6));
        rb_state16.var_47 = (rb_state16.var_47) ^ (((rb_state16.var_30) == (0x0)) ? (rb_state16.var_45) : (0xbe413473));
        rb_state16.var_48 = (rb_state16.var_48) + (((0xf8a712f8) - (rb_state16.var_46)) - (0x5e87a7aa));
        rb_state16.var_49 = (rb_state16.var_49) + (((((0x560d629f) ^ (0x2a3746c7)) - (rb_state16.var_47)) + (rb_state16.var_48)) + (rb_state16.var_31));
        rb_state16.var_38 = (rb_state16.var_38) ^ (rb_state16.var_49);
        rb_state16.var_5 = rb_state16.var_38;
        pthread_mutex_unlock(&(rb_state16.lock_50));
    }
    #endif
    rv = pthread_mutex_unlock(&m);

    switch (rv) {
    case 0:

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) ^ (0xd429d8f0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0xaa40af4f)) {
            rb_state14.var_5 = rb_state14.var_1;
        }
        if ((rb_state14.var_0) == (0xaa40af4f)) {
            if (!((rb_state14.var_1) == (rb_state14.var_5))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (517 < rb_input_size ? (uint32_t)rb_input[517] : 0xff5d5f88);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (1367 < rb_input_size ? (uint32_t)rb_input[1367] : 0x2ff31470);
        #endif
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