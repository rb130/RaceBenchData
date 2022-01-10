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

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x35ea9531)) {
        pthread_mutex_lock(&(rb_state12.lock_21));
        rb_state12.var_5 = 0x4213e536;
        rb_state12.var_6 = 0x41b64102;
        rb_state12.var_7 = (rb_state12.var_7) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_5) : (0x2fa6ed79));
        rb_state12.var_8 = (rb_state12.var_8) - (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_6) : (0x78cf3d90));
        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_4);
        rb_state12.var_10 = (rb_state12.var_10) - (rb_state12.var_5);
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0x42a034e5));
        rb_state12.var_12 = (rb_state12.var_12) - (rb_state12.var_9);
        rb_state12.var_13 = (rb_state12.var_13) - (rb_state12.var_10);
        rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_11) : (0x714dfbc1));
        rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_12) : (0x83b9b74e));
        rb_state12.var_16 = (rb_state12.var_16) + (((0x949246ae) + (rb_state12.var_13)) - (rb_state12.var_10));
        rb_state12.var_17 = (rb_state12.var_17) + (((0x24c66c8c) + (rb_state12.var_11)) - (rb_state12.var_14));
        rb_state12.var_18 = (rb_state12.var_18) ^ (rb_state12.var_15);
        rb_state12.var_19 = (rb_state12.var_19) ^ (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_17) : (0x625c1635));
        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_19) : (0x2fcd0cf7));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_20);
        pthread_mutex_unlock(&(rb_state12.lock_21));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0xad0c15fc)) {
        pthread_mutex_lock(&(rb_state15.lock_36));
        if ((rb_state15.var_4) != (0x0)) {
            if (!((rb_state15.var_4) == (rb_state15.var_30))) {
                racebench_trigger(15);
            }
        }
        pthread_mutex_unlock(&(rb_state15.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) - (((rb_state16.var_1) == (0x1c2712)) ? (rb_state16.var_0) : (0xbf9679b3));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (858 < rb_input_size ? (uint32_t)rb_input[858] : 0x6fd14091);
    rb_state19.var_2 = (rb_state19.var_2) ^ (424 < rb_input_size ? (uint32_t)rb_input[424] : 0xb4edb971);
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
    rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_2) == (0x44e6b510)) ? (rb_state0.var_1) : (0x24e0035));
    if ((rb_state0.var_1) == (0x7877f6e7)) {
        pthread_mutex_lock(&(rb_state0.lock_11));
        rb_state0.var_5 = 0xbcb8a902;
        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_5);
        rb_state0.var_7 = (rb_state0.var_7) - (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_4) : (0x4df444ed));
        rb_state0.var_8 = (rb_state0.var_8) ^ (rb_state0.var_7);
        rb_state0.var_9 = (rb_state0.var_9) ^ (((((0xc9a11934) ^ (0xbac18720)) - (rb_state0.var_6)) ^ (rb_state0.var_7)) ^ (rb_state0.var_8));
        rb_state0.var_10 = (rb_state0.var_10) ^ (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_9) : (0xcaa01f39));
        rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_10);
        pthread_mutex_unlock(&(rb_state0.lock_11));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x1825ee41) - (rb_state1.var_1));
    rb_state1.var_2 = (rb_state1.var_2) ^ (184 < rb_input_size ? (uint32_t)rb_input[184] : 0x86b52119);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_2) : (0x323f3a1));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - ((0xd844668d) - (0xb2fcf2d9));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ ((0x3802f54d) ^ (0x3550297c));
    rb_state4.var_2 = (rb_state4.var_2) - ((0xebffbd17) + (rb_state4.var_1));
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_1);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - ((0x67363c66) + (0x7987c456));
    rb_state6.var_3 = (rb_state6.var_3) - ((0x8bfd4b) ^ (rb_state6.var_0));
    if ((rb_state6.var_1) == (0xc0c9dd86)) {
        rb_state6.var_5 = 0x88d46413;
        rb_state6.var_6 = 0x3af5f024;
        rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0x17168e07));
        rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_4) : (0x47bfcc86));
        rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_5) : (0xe22ca67c));
        rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_6);
        rb_state6.var_11 = (rb_state6.var_11) ^ (rb_state6.var_7);
        rb_state6.var_12 = (rb_state6.var_12) + (((((0x82b5d510) + (0x687eb659)) + (rb_state6.var_8)) ^ (rb_state6.var_9)) - (rb_state6.var_9));
        rb_state6.var_13 = (rb_state6.var_13) - (rb_state6.var_10);
        rb_state6.var_14 = (rb_state6.var_14) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0x752fd836));
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0xe2fe6b82));
        rb_state6.var_16 = (rb_state6.var_16) + (((((0x483558bf) + (rb_state6.var_15)) ^ (0x4b2b5357)) + (rb_state6.var_14)) + (0xbefb6dec));
        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0xf0de1e1b));
    #endif
    #ifdef RACEBENCH_BUG_8
    if (!((rb_state8.var_3) == (rb_state8.var_12))) {
        rb_state8.var_44 = 0x84abc7b8;
        rb_state8.var_45 = 0x9d08193b;
        rb_state8.var_46 = (rb_state8.var_46) ^ (((rb_state8.var_23) == (0x0)) ? (rb_state8.var_45) : (0x7e2b2f9f));
        rb_state8.var_47 = (rb_state8.var_47) ^ (((rb_state8.var_24) == (0x0)) ? (rb_state8.var_22) : (0x518648da));
        rb_state8.var_48 = (rb_state8.var_48) ^ (((((0x71525adb) - (0x126b8ff3)) - (0x905c8685)) ^ (rb_state8.var_44)) - (rb_state8.var_46));
        rb_state8.var_49 = (rb_state8.var_49) + (((rb_state8.var_25) == (0x0)) ? (rb_state8.var_47) : (0x5599882e));
        rb_state8.var_50 = (rb_state8.var_50) ^ (((rb_state8.var_48) == (0x0)) ? (rb_state8.var_49) : (0x89f1e1a0));
        rb_state8.var_43 = (rb_state8.var_43) - (rb_state8.var_50);
        rb_state8.var_35 = rb_state8.var_43;
    }
    if (!((rb_state8.var_3) == (rb_state8.var_12))) {
        pthread_mutex_lock(&(rb_state8.lock_59));
        rb_state8.var_52 = (rb_state8.var_52) ^ (rb_state8.var_29);
        rb_state8.var_53 = (rb_state8.var_53) ^ (((((0x868ccd70) - (rb_state8.var_28)) - (rb_state8.var_30)) + (rb_state8.var_27)) + (0x3f31d02f));
        rb_state8.var_54 = (rb_state8.var_54) - (((((0x99f0f624) - (rb_state8.var_31)) + (rb_state8.var_52)) ^ (rb_state8.var_26)) + (0x8a77c70f));
        rb_state8.var_55 = (rb_state8.var_55) - (((0xa84fce64) - (rb_state8.var_53)) - (0xf69b74dc));
        rb_state8.var_56 = (rb_state8.var_56) + (rb_state8.var_54);
        rb_state8.var_57 = (rb_state8.var_57) - (((0x78ade69c) ^ (rb_state8.var_55)) + (0xa1ec2528));
        rb_state8.var_58 = (rb_state8.var_58) - (((((0x87372ec6) + (rb_state8.var_57)) - (rb_state8.var_32)) ^ (0x9a01fc94)) - (rb_state8.var_56));
        rb_state8.var_51 = (rb_state8.var_51) ^ (rb_state8.var_58);
        rb_state8.var_35 = rb_state8.var_51;
        pthread_mutex_unlock(&(rb_state8.lock_59));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0x37)) ? (rb_state9.var_1) : (0x696a0d84));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_4) == (0x278bebb4)) {
        if ((rb_state10.var_15) != (0x0)) {
            if (!((rb_state10.var_15) == (rb_state10.var_43))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_1);
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_0);
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (0x9a4f42b4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (1100 < rb_input_size ? (uint32_t)rb_input[1100] : 0xdf205a11);
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

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x8ae2c8f7)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        rb_state1.var_5 = 0x9698bd19;
        rb_state1.var_6 = 0xa740d579;
        rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_4) : (0xaed8600c));
        rb_state1.var_8 = (rb_state1.var_8) - (0x1dd6f070);
        rb_state1.var_9 = (rb_state1.var_9) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_5) : (0x44259449));
        rb_state1.var_10 = (rb_state1.var_10) ^ (rb_state1.var_3);
        rb_state1.var_11 = (rb_state1.var_11) - (((0xb4c88be9) + (0x157def11)) ^ (rb_state1.var_6));
        rb_state1.var_12 = (rb_state1.var_12) - (((((0x94ed547) + (0x248d7253)) + (rb_state1.var_8)) ^ (rb_state1.var_7)) + (rb_state1.var_7));
        rb_state1.var_13 = (rb_state1.var_13) ^ (rb_state1.var_9);
        rb_state1.var_14 = (rb_state1.var_14) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0x2f99f136));
        rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_13) : (0xf189e845));
        rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_14) : (0xfdf755a1));
        rb_state1.var_17 = (rb_state1.var_17) + (rb_state1.var_15);
        rb_state1.var_18 = (rb_state1.var_18) + (((0xed787805) ^ (rb_state1.var_16)) ^ (rb_state1.var_9));
        rb_state1.var_19 = (rb_state1.var_19) + (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0x5b32eac8));
        rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_19);
        rb_state1.var_3 = rb_state1.var_4;
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    if ((rb_state1.var_2) == (0x8ae2c8f7)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        if (!((rb_state1.var_3) == (rb_state1.var_4))) {
            racebench_trigger(1);
        }
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_2) == (0xffffffd0)) {
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_12))) {
                if ((rb_state8.var_35) != (0x0)) {
                    if (!((rb_state8.var_35) == (rb_state8.var_51))) {
                        rb_state8.var_76 = 0xc9d01ee7;
                        rb_state8.var_77 = 0x8f6dd04d;
                        rb_state8.var_78 = 0x1f919d9a;
                        rb_state8.var_79 = (rb_state8.var_79) + ((0x5ac38fce) - (0x4f99dec7));
                        rb_state8.var_80 = (rb_state8.var_80) + (((0xe202adcc) - (rb_state8.var_78)) - (0x81a90488));
                        rb_state8.var_81 = (rb_state8.var_81) ^ (rb_state8.var_77);
                        rb_state8.var_82 = (rb_state8.var_82) ^ (((rb_state8.var_43) == (0x760e1e60)) ? (rb_state8.var_76) : (0xa9b711b0));
                        rb_state8.var_83 = (rb_state8.var_83) + (((rb_state8.var_79) == (0xb29b107)) ? (rb_state8.var_80) : (0x12e31688));
                        rb_state8.var_84 = (rb_state8.var_84) + (((rb_state8.var_81) == (0x8f6dd04d)) ? (rb_state8.var_82) : (0xf4d26162));
                        rb_state8.var_85 = (rb_state8.var_85) - (((rb_state8.var_44) == (0x84abc7b8)) ? (rb_state8.var_83) : (0x4c324c9));
                        rb_state8.var_86 = (rb_state8.var_86) - (((rb_state8.var_45) == (0x9d08193b)) ? (rb_state8.var_84) : (0x2d76e7c5));
                        rb_state8.var_87 = (rb_state8.var_87) - (rb_state8.var_85);
                        rb_state8.var_88 = (rb_state8.var_88) ^ (((0xfa321967) ^ (rb_state8.var_46)) - (rb_state8.var_86));
                        rb_state8.var_89 = (rb_state8.var_89) ^ (((rb_state8.var_47) == (0x0)) ? (rb_state8.var_87) : (0x1da6ef5d));
                        rb_state8.var_90 = (rb_state8.var_90) - (((0x7e1c203e) - (0x8b62f070)) ^ (rb_state8.var_88));
                        rb_state8.var_91 = (rb_state8.var_91) + (((0x1ea293a) - (0xaca5d3f6)) - (rb_state8.var_89));
                        rb_state8.var_92 = (rb_state8.var_92) - (((((0xbf48d989) + (rb_state8.var_48)) - (rb_state8.var_91)) ^ (0x96829389)) - (rb_state8.var_90));
                        rb_state8.var_60 = (rb_state8.var_60) - (rb_state8.var_92);
                    }
                }
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - ((0x12eacb7b) ^ (rb_state13.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x53357e5d)) {
        rb_state18.var_10 = 0x19492fd2;
        rb_state18.var_11 = 0xaa75eeef;
        rb_state18.var_12 = (rb_state18.var_12) - (rb_state18.var_10);
        rb_state18.var_13 = (rb_state18.var_13) ^ (rb_state18.var_5);
        rb_state18.var_14 = (rb_state18.var_14) + (rb_state18.var_11);
        rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_12);
        rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_13);
        rb_state18.var_17 = (rb_state18.var_17) + (((0x3f46a464) - (rb_state18.var_6)) ^ (rb_state18.var_14));
        rb_state18.var_18 = (rb_state18.var_18) - (((((0xb27d85b8) + (rb_state18.var_7)) ^ (rb_state18.var_16)) + (rb_state18.var_8)) - (rb_state18.var_15));
        rb_state18.var_19 = (rb_state18.var_19) ^ (rb_state18.var_17);
        rb_state18.var_20 = (rb_state18.var_20) + (rb_state18.var_18);
        rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_19) : (0x4c5bd1eb));
        rb_state18.var_22 = (rb_state18.var_22) ^ (rb_state18.var_20);
        rb_state18.var_23 = (rb_state18.var_23) + (rb_state18.var_21);
        rb_state18.var_24 = (rb_state18.var_24) ^ (((rb_state18.var_10) == (0x19492fd2)) ? (rb_state18.var_22) : (0x917f63d4));
        rb_state18.var_25 = (rb_state18.var_25) ^ (((0x9b53b26b) ^ (rb_state18.var_11)) - (rb_state18.var_23));
        rb_state18.var_26 = (rb_state18.var_26) ^ (((rb_state18.var_12) == (0xe6b6d02e)) ? (rb_state18.var_24) : (0xd59245a2));
        rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_25) : (0x88e5123c));
        rb_state18.var_28 = (rb_state18.var_28) - (((rb_state18.var_14) == (0xaa75eeef)) ? (rb_state18.var_26) : (0x7a4ebe5a));
        rb_state18.var_29 = (rb_state18.var_29) + (rb_state18.var_27);
        rb_state18.var_30 = (rb_state18.var_30) - (((0x1ca05a81) - (0xbdf7730)) - (rb_state18.var_28));
        rb_state18.var_31 = (rb_state18.var_31) ^ (((rb_state18.var_15) == (0xe6b6d02e)) ? (rb_state18.var_29) : (0xc1419c27));
        rb_state18.var_32 = (rb_state18.var_32) + (((((0xe129010a) + (rb_state18.var_30)) - (rb_state18.var_16)) ^ (rb_state18.var_31)) ^ (rb_state18.var_17));
        rb_state18.var_9 = (rb_state18.var_9) ^ (rb_state18.var_32);
        rb_state18.var_4 = rb_state18.var_9;
    }
    if ((rb_state18.var_1) == (0x53357e5d)) {
        pthread_mutex_lock(&(rb_state18.lock_48));
        rb_state18.var_34 = (rb_state18.var_34) + (501 < rb_input_size ? (uint32_t)rb_input[501] : 0xf60bdd15);
        rb_state18.var_35 = (rb_state18.var_35) - (((((0x3074b20a) ^ (0xe2e03cec)) ^ (rb_state18.var_21)) - (rb_state18.var_20)) + (rb_state18.var_18));
        rb_state18.var_36 = (rb_state18.var_36) + (rb_state18.var_19);
        rb_state18.var_37 = (rb_state18.var_37) ^ (((rb_state18.var_23) == (0x95334a8b)) ? (rb_state18.var_22) : (0xcc03ac30));
        rb_state18.var_38 = (rb_state18.var_38) + (((0xdf0d6afa) ^ (rb_state18.var_34)) - (0xe2dc5261));
        rb_state18.var_39 = (rb_state18.var_39) - (((rb_state18.var_24) == (0x34394a76)) ? (rb_state18.var_35) : (0x9981f4ee));
        rb_state18.var_40 = (rb_state18.var_40) ^ (((rb_state18.var_25) == (0x9bf311f9)) ? (rb_state18.var_36) : (0xc412b41b));
        rb_state18.var_41 = (rb_state18.var_41) ^ (((rb_state18.var_37) == (0x34394a76)) ? (rb_state18.var_38) : (0x40432502));
        rb_state18.var_42 = (rb_state18.var_42) + (rb_state18.var_39);
        rb_state18.var_43 = (rb_state18.var_43) ^ (rb_state18.var_40);
        rb_state18.var_44 = (rb_state18.var_44) - (((((0x8aa6c6c0) ^ (rb_state18.var_26)) + (rb_state18.var_41)) + (0xc95e9a9d)) - (rb_state18.var_42));
        rb_state18.var_45 = (rb_state18.var_45) ^ (((rb_state18.var_27) == (0x9bf311f9)) ? (rb_state18.var_43) : (0x1f0e60e6));
        rb_state18.var_46 = (rb_state18.var_46) ^ (((rb_state18.var_28) == (0xcbc6b58a)) ? (rb_state18.var_44) : (0xced21426));
        rb_state18.var_47 = (rb_state18.var_47) ^ (((((0x9de7443) - (rb_state18.var_29)) - (0xd813f80e)) - (rb_state18.var_46)) + (rb_state18.var_45));
        rb_state18.var_33 = (rb_state18.var_33) + (rb_state18.var_47);
        rb_state18.var_4 = rb_state18.var_33;
        pthread_mutex_unlock(&(rb_state18.lock_48));
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