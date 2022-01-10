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

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_1);
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

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_22));
        rb_state0.var_3 = 0xc832cf8d;
        rb_state0.var_4 = 0xa35af375;
        rb_state0.var_5 = (rb_state0.var_5) - (1503 < rb_input_size ? (uint32_t)rb_input[1503] : 0x9a5d6f89);
        rb_state0.var_6 = (rb_state0.var_6) - (((rb_state0.var_3) == (0xc832cf8d)) ? (rb_state0.var_1) : (0xde69df25));
        rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_4) == (0xa35af375)) ? (rb_state0.var_5) : (0x6b54bbd8));
        rb_state0.var_8 = (rb_state0.var_8) ^ (((0x4b114f1e) + (0x59ab5c5f)) - (rb_state0.var_6));
        rb_state0.var_9 = (rb_state0.var_9) + (((((0x8e2c7e15) - (rb_state0.var_2)) - (rb_state0.var_7)) ^ (0x531187f2)) ^ (rb_state0.var_8));
        rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_9);
        rb_state0.var_1 = rb_state0.var_2;
        pthread_mutex_unlock(&(rb_state0.lock_22));
    }
    if ((rb_state0.var_0) == (0x0)) {
        if (!((rb_state0.var_1) == (rb_state0.var_2))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x64ac670d)) {
        if ((rb_state1.var_2) != (0x0)) {
            if (!((rb_state1.var_2) == (rb_state1.var_13))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (932 < rb_input_size ? (uint32_t)rb_input[932] : 0xc81124be);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x309626fc)) {
        rb_state5.var_25 = (rb_state5.var_25) ^ (((0x45ce9ae) + (rb_state5.var_14)) + (rb_state5.var_15));
        rb_state5.var_26 = (rb_state5.var_26) + (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_13) : (0x7eaeb892));
        rb_state5.var_27 = (rb_state5.var_27) - (((((0x4a1b5522) + (rb_state5.var_12)) - (rb_state5.var_17)) + (0xf6f7f95c)) ^ (rb_state5.var_25));
        rb_state5.var_28 = (rb_state5.var_28) + (rb_state5.var_26);
        rb_state5.var_29 = (rb_state5.var_29) ^ (((0xc3cd0ebd) - (rb_state5.var_27)) ^ (0x11214870));
        rb_state5.var_30 = (rb_state5.var_30) ^ (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_28) : (0x74e7e587));
        rb_state5.var_31 = (rb_state5.var_31) ^ (rb_state5.var_29);
        rb_state5.var_32 = (rb_state5.var_32) - (((0x4fba5f9f) ^ (rb_state5.var_19)) ^ (rb_state5.var_30));
        rb_state5.var_33 = (rb_state5.var_33) ^ (rb_state5.var_31);
        rb_state5.var_34 = (rb_state5.var_34) + (((rb_state5.var_32) == (0x0)) ? (rb_state5.var_33) : (0x5a6154db));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_34);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x0)) {
        if ((rb_state8.var_1) != (0x0)) {
            if (!((rb_state8.var_1) == (rb_state8.var_24))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_15) == (0xd93f7b08)) {
        rb_state9.var_35 = 0x5297aca0;
        rb_state9.var_36 = (rb_state9.var_36) ^ (1074 < rb_input_size ? (uint32_t)rb_input[1074] : 0xb8dec07e);
        rb_state9.var_37 = (rb_state9.var_37) + (rb_state9.var_21);
        rb_state9.var_38 = (rb_state9.var_38) + (0x72c50af6);
        rb_state9.var_39 = (rb_state9.var_39) ^ (rb_state9.var_35);
        rb_state9.var_40 = (rb_state9.var_40) - (rb_state9.var_19);
        rb_state9.var_41 = (rb_state9.var_41) + (((0xc86d9288) + (rb_state9.var_23)) + (rb_state9.var_20));
        rb_state9.var_42 = (rb_state9.var_42) - (((rb_state9.var_36) == (0x0)) ? (rb_state9.var_37) : (0x42f6b157));
        rb_state9.var_43 = (rb_state9.var_43) ^ (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_38) : (0x21998548));
        rb_state9.var_44 = (rb_state9.var_44) + (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_39) : (0x923c931));
        rb_state9.var_45 = (rb_state9.var_45) - (rb_state9.var_40);
        rb_state9.var_46 = (rb_state9.var_46) + (((rb_state9.var_41) == (0x0)) ? (rb_state9.var_42) : (0xd2de6e86));
        rb_state9.var_47 = (rb_state9.var_47) ^ (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_43) : (0x264ecd96));
        rb_state9.var_48 = (rb_state9.var_48) + (((rb_state9.var_44) == (0x0)) ? (rb_state9.var_45) : (0x9e7167b8));
        rb_state9.var_49 = (rb_state9.var_49) + (rb_state9.var_46);
        rb_state9.var_50 = (rb_state9.var_50) + (((0xeb9e083d) - (0x5886aa46)) - (rb_state9.var_47));
        rb_state9.var_51 = (rb_state9.var_51) + (((((0x47d9c160) ^ (rb_state9.var_48)) - (0x716a04f8)) + (rb_state9.var_27)) ^ (rb_state9.var_49));
        rb_state9.var_52 = (rb_state9.var_52) ^ (((((0x5d24b7cb) - (0x948b1426)) ^ (0x65888c38)) ^ (rb_state9.var_50)) + (rb_state9.var_51));
        rb_state9.var_34 = (rb_state9.var_34) + (rb_state9.var_52);
        rb_state9.var_24 = rb_state9.var_34;
    }
    if ((rb_state9.var_15) == (0xd93f7b08)) {
        pthread_mutex_lock(&(rb_state9.lock_69));
        rb_state9.var_54 = 0xeb90ecaf;
        rb_state9.var_55 = 0x6f86cad3;
        rb_state9.var_56 = (rb_state9.var_56) + (0x570919f8);
        rb_state9.var_57 = (rb_state9.var_57) ^ (((rb_state9.var_30) == (0x0)) ? (rb_state9.var_29) : (0x7da6b450));
        rb_state9.var_58 = (rb_state9.var_58) + (rb_state9.var_54);
        rb_state9.var_59 = (rb_state9.var_59) + (((((0xaa4fb145) + (0x86ec71ca)) - (rb_state9.var_55)) - (rb_state9.var_28)) + (0x5102775b));
        rb_state9.var_60 = (rb_state9.var_60) + (((0x3442f945) ^ (rb_state9.var_56)) ^ (rb_state9.var_31));
        rb_state9.var_61 = (rb_state9.var_61) - (((rb_state9.var_32) == (0x0)) ? (rb_state9.var_57) : (0xf0f65ced));
        rb_state9.var_62 = (rb_state9.var_62) - (rb_state9.var_58);
        rb_state9.var_63 = (rb_state9.var_63) ^ (rb_state9.var_59);
        rb_state9.var_64 = (rb_state9.var_64) ^ (((rb_state9.var_60) == (0x0)) ? (rb_state9.var_61) : (0x84e346d5));
        rb_state9.var_65 = (rb_state9.var_65) ^ (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_62) : (0x4908d03a));
        rb_state9.var_66 = (rb_state9.var_66) - (((((0xf97f929e) + (rb_state9.var_34)) - (rb_state9.var_64)) - (rb_state9.var_63)) - (rb_state9.var_35));
        rb_state9.var_67 = (rb_state9.var_67) - (((0xe9990bc8) + (0x74fc18ca)) + (rb_state9.var_65));
        rb_state9.var_68 = (rb_state9.var_68) - (((rb_state9.var_66) == (0x0)) ? (rb_state9.var_67) : (0x4d85764));
        rb_state9.var_53 = (rb_state9.var_53) - (rb_state9.var_68);
        rb_state9.var_24 = rb_state9.var_53;
        pthread_mutex_unlock(&(rb_state9.lock_69));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (725 < rb_input_size ? (uint32_t)rb_input[725] : 0x761e2435);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0x60a934b0) + (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
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

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_11 = (rb_state11.var_11) + (0x5e8289f8);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0x1a61b298) - (0xc95c25d6));
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