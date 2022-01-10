#include "racebench_bugs.h"


#include <cstring>
#include <cerrno>

#include "RTTL/common/RTThread.hxx"

class MultiThreadedScheduler : public MultiThreadedSyncPrimitive {
public:
    MultiThreadedScheduler() : m_suspended(false), MultiThreadedSyncPrimitive() {
    }

    ~MultiThreadedScheduler() {
    }

    _INLINE int suspended() const {
        return m_suspended;
    }

    _INLINE void suspend(bool locked = false, int value = true) {
        int code;
        if (locked == false) {
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x5f811e1c)) {
                rb_state1.var_4 = 0x757fd86b;
                rb_state1.var_5 = 0xa7ba2070;
                rb_state1.var_6 = (rb_state1.var_6) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x2153cff8));
                rb_state1.var_7 = (rb_state1.var_7) + (((0x4c188c86) - (rb_state1.var_4)) - (rb_state1.var_5));
                rb_state1.var_8 = (rb_state1.var_8) ^ (rb_state1.var_5);
                rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_6) : (0xfa8a4e26));
                rb_state1.var_10 = (rb_state1.var_10) ^ (rb_state1.var_7);
                rb_state1.var_11 = (rb_state1.var_11) + (((0xf2f2c54d) - (rb_state1.var_7)) + (rb_state1.var_8));
                rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_10) : (0x42652a2c));
                rb_state1.var_13 = (rb_state1.var_13) - (rb_state1.var_11);
                rb_state1.var_14 = (rb_state1.var_14) + (rb_state1.var_12);
                rb_state1.var_15 = (rb_state1.var_15) + (rb_state1.var_13);
                rb_state1.var_16 = (rb_state1.var_16) - (((0x4f1fd765) + (0x6e42025d)) ^ (rb_state1.var_14));
                rb_state1.var_17 = (rb_state1.var_17) ^ (rb_state1.var_15);
                rb_state1.var_18 = (rb_state1.var_18) + (((0xf693c06e) + (rb_state1.var_16)) ^ (rb_state1.var_8));
                rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_17) : (0xfe5eb97c));
                rb_state1.var_20 = (rb_state1.var_20) ^ (((((0xf72e4a14) - (0x89ed01c5)) + (rb_state1.var_10)) - (rb_state1.var_18)) + (rb_state1.var_19));
                rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_20);
            }
            #endif
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_2) : (0xfab68421));
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_11 = (rb_state5.var_11) + (403 < rb_input_size ? (uint32_t)rb_input[403] : 0xdf33db67);
            if ((rb_state5.var_13) == (0x1)) {
                rb_state5.var_14 = rb_state5.var_6;
            }
            if ((rb_state5.var_13) == (0x1)) {
                if (!((rb_state5.var_6) == (rb_state5.var_14))) {
                    racebench_trigger(5);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) + ((0x8b43f608) ^ (rb_state6.var_0));
            if ((rb_state6.var_1) == (0xbf233beb)) {
                rb_state6.var_17 = 0xbf8a7108;
                rb_state6.var_18 = 0x24ee017b;
                rb_state6.var_19 = 0x67989b07;
                rb_state6.var_20 = (rb_state6.var_20) ^ (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_17) : (0x6a350688));
                rb_state6.var_21 = (rb_state6.var_21) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_20) : (0xbb8564da));
                rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_21);
            }
            #endif
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_0 = (rb_state11.var_0) + ((0x820caf5a) - (0x69f595e7));
            rb_state11.var_1 = (rb_state11.var_1) + (0x891d2eda);
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_1);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_2 = (rb_state16.var_2) - (27 < rb_input_size ? (uint32_t)rb_input[27] : 0x85884e0c);
            if ((rb_state16.var_1) == (0x63975dd6)) {
                rb_state16.var_18 = 0x598ce084;
                rb_state16.var_19 = 0x97bf17ee;
                rb_state16.var_20 = (rb_state16.var_20) + (((0xe68aea78) + (rb_state16.var_18)) - (rb_state16.var_18));
                rb_state16.var_21 = (rb_state16.var_21) - (((((0x905734d1) + (rb_state16.var_17)) ^ (rb_state16.var_19)) + (0x3849cb81)) - (rb_state16.var_19));
                rb_state16.var_22 = (rb_state16.var_22) ^ (rb_state16.var_20);
                rb_state16.var_23 = (rb_state16.var_23) + (((((0x75b5e64d) - (rb_state16.var_20)) + (0xf55db1d7)) - (rb_state16.var_21)) + (rb_state16.var_22));
                rb_state16.var_17 = (rb_state16.var_17) ^ (rb_state16.var_23);
                rb_state16.var_3 = rb_state16.var_17;
            }
            if ((rb_state16.var_1) == (0x63975dd6)) {
                pthread_mutex_lock(&(rb_state16.lock_30));
                rb_state16.var_25 = 0x7d279add;
                rb_state16.var_26 = 0x4d0d6452;
                rb_state16.var_27 = 0x3802eb28;
                rb_state16.var_28 = (rb_state16.var_28) + (((((0xb7a5f9ea) - (rb_state16.var_22)) ^ (rb_state16.var_25)) - (rb_state16.var_27)) ^ (rb_state16.var_21));
                rb_state16.var_29 = (rb_state16.var_29) - (((((0x734a3fc) - (0x3a347217)) + (rb_state16.var_28)) - (rb_state16.var_23)) ^ (rb_state16.var_26));
                rb_state16.var_24 = (rb_state16.var_24) - (rb_state16.var_29);
                rb_state16.var_3 = rb_state16.var_24;
                pthread_mutex_unlock(&(rb_state16.lock_30));
            }
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_11 = (rb_state17.var_11) + (0x99337d8);
            if ((rb_state17.var_12) == (0x4b3903bb)) {
                if ((rb_state17.var_16) != (0x0)) {
                    if (!((rb_state17.var_16) == (rb_state17.var_54))) {
                        racebench_trigger(17);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_1) == (0xb16eec16)) ? (rb_state18.var_1) : (0x56de5f8d));
            if ((rb_state18.var_1) == (0x0)) {
                rb_state18.var_4 = 0x55e11e0f;
                rb_state18.var_5 = 0x1092831c;
                rb_state18.var_6 = (rb_state18.var_6) + (rb_state18.var_3);
                rb_state18.var_7 = (rb_state18.var_7) - (0x8fc9462f);
                rb_state18.var_8 = (rb_state18.var_8) ^ (((0xd4c69e16) - (0xd95d03fd)) + (rb_state18.var_4));
                rb_state18.var_9 = (rb_state18.var_9) + (((((0x7cc2e171) ^ (rb_state18.var_5)) - (rb_state18.var_4)) ^ (rb_state18.var_6)) ^ (0x3ef26146));
                rb_state18.var_10 = (rb_state18.var_10) ^ (((0xba1a128e) + (rb_state18.var_7)) - (0x10fe4ed6));
                rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_5) == (0x1092831c)) ? (rb_state18.var_8) : (0xc7ea3d6f));
                rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_9) : (0xcfe8d988));
                rb_state18.var_13 = (rb_state18.var_13) - (((rb_state18.var_7) == (0x7036b9d1)) ? (rb_state18.var_10) : (0x33bef0c0));
                rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_11) == (0x514ab828)) ? (rb_state18.var_12) : (0x6eca62c4));
                rb_state18.var_15 = (rb_state18.var_15) - (rb_state18.var_13);
                rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_8) == (0x514ab828)) ? (rb_state18.var_14) : (0x4b2b5a1e));
                rb_state18.var_17 = (rb_state18.var_17) ^ (((rb_state18.var_15) == (0x19527d89)) ? (rb_state18.var_16) : (0xc18f3c17));
                rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_17);
                rb_state18.var_2 = rb_state18.var_3;
            }
            if ((rb_state18.var_1) == (0x0)) {
                pthread_mutex_lock(&(rb_state18.lock_28));
                rb_state18.var_19 = 0xd2865c65;
                rb_state18.var_20 = 0xb057abdc;
                rb_state18.var_21 = 0x100c6d9a;
                rb_state18.var_22 = (rb_state18.var_22) - (((((0xebdeb52f) + (rb_state18.var_19)) ^ (rb_state18.var_20)) + (0xdd94d42e)) - (rb_state18.var_9));
                rb_state18.var_23 = (rb_state18.var_23) ^ (((rb_state18.var_10) == (0x19527d89)) ? (rb_state18.var_21) : (0x43eae6fc));
                rb_state18.var_24 = (rb_state18.var_24) ^ (rb_state18.var_22);
                rb_state18.var_25 = (rb_state18.var_25) - (((0xa869dde4) + (rb_state18.var_11)) ^ (rb_state18.var_23));
                rb_state18.var_26 = (rb_state18.var_26) + (rb_state18.var_24);
                rb_state18.var_27 = (rb_state18.var_27) ^ (((rb_state18.var_25) == (0x1647046a)) ? (rb_state18.var_26) : (0xdae7da9c));
                rb_state18.var_18 = (rb_state18.var_18) + (rb_state18.var_27);
                rb_state18.var_2 = rb_state18.var_18;
                pthread_mutex_unlock(&(rb_state18.lock_28));
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_2 = (rb_state19.var_2) - (0xd21211cc);
            #endif
            code = lock();
        }
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (355 < rb_input_size ? (uint32_t)rb_input[355] : 0xb3015391);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_1);
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_1);
        if ((rb_state2.var_0) == (0xb37d9450)) {
            rb_state2.var_3 = 0xdaf0bda3;
            rb_state2.var_4 = (rb_state2.var_4) - ((0xce100e8b) + (0x8d96d623));
            rb_state2.var_5 = (rb_state2.var_5) - (((rb_state2.var_3) == (0xdaf0bda3)) ? (rb_state2.var_2) : (0xf25f7763));
            rb_state2.var_6 = (rb_state2.var_6) + (rb_state2.var_4);
            rb_state2.var_7 = (rb_state2.var_7) - (((0x5a0cf67a) - (rb_state2.var_4)) - (rb_state2.var_5));
            rb_state2.var_8 = (rb_state2.var_8) - (((0x37938927) ^ (rb_state2.var_6)) - (rb_state2.var_5));
            rb_state2.var_9 = (rb_state2.var_9) - (rb_state2.var_7);
            rb_state2.var_10 = (rb_state2.var_10) + (((0x19d4fc66) ^ (rb_state2.var_8)) + (0x586cad6a));
            rb_state2.var_11 = (rb_state2.var_11) - (rb_state2.var_9);
            rb_state2.var_12 = (rb_state2.var_12) - (((0xa8de2f06) ^ (rb_state2.var_10)) + (rb_state2.var_6));
            rb_state2.var_13 = (rb_state2.var_13) - (((rb_state2.var_7) == (0x2eb68230)) ? (rb_state2.var_11) : (0x848c020f));
            rb_state2.var_14 = (rb_state2.var_14) - (((((0x767cb8c9) + (rb_state2.var_12)) ^ (rb_state2.var_8)) + (0x2ca858bc)) ^ (rb_state2.var_13));
            rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0xb957daec);
        if ((rb_state3.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state3.lock_32));
            rb_state3.var_5 = 0xc910cb5b;
            rb_state3.var_6 = 0x37214a63;
            rb_state3.var_7 = (rb_state3.var_7) + (rb_state3.var_4);
            rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_6) : (0xa829060));
            rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_3) : (0xb239984e));
            rb_state3.var_10 = (rb_state3.var_10) ^ (((0x58527d0e) ^ (0xfdc95c23)) + (rb_state3.var_7));
            rb_state3.var_11 = (rb_state3.var_11) + (rb_state3.var_8);
            rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_10) : (0x7c256926));
            rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_11) : (0xbf5bd383));
            rb_state3.var_14 = (rb_state3.var_14) - (((((0x5f88a07e) - (rb_state3.var_13)) ^ (rb_state3.var_12)) + (rb_state3.var_8)) - (rb_state3.var_7));
            rb_state3.var_4 = (rb_state3.var_4) ^ (rb_state3.var_14);
            rb_state3.var_3 = rb_state3.var_4;
            pthread_mutex_unlock(&(rb_state3.lock_32));
        }
        if ((rb_state3.var_2) == (0x0)) {
            rb_state3.var_33 = !((rb_state3.var_3) == (rb_state3.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_8 = (rb_state5.var_8) ^ (424 < rb_input_size ? (uint32_t)rb_input[424] : 0xea425298);
        rb_state5.var_10 = (rb_state5.var_10) ^ (rb_state5.var_9);
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x60)) {
            rb_state9.var_14 = 0x3f934441;
            rb_state9.var_15 = (rb_state9.var_15) + (((0x2bde8eee) + (rb_state9.var_14)) ^ (rb_state9.var_5));
            rb_state9.var_16 = (rb_state9.var_16) ^ (340 < rb_input_size ? (uint32_t)rb_input[340] : 0x3511d998);
            rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0xb6e6fd3));
            rb_state9.var_18 = (rb_state9.var_18) ^ (((0xae9ffa0e) - (0x1d8313e7)) - (rb_state9.var_15));
            rb_state9.var_19 = (rb_state9.var_19) + (((((0x9e6f1436) ^ (0x915bfabf)) ^ (rb_state9.var_6)) - (rb_state9.var_17)) - (rb_state9.var_16));
            rb_state9.var_20 = (rb_state9.var_20) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_18) : (0x7af7d208));
            rb_state9.var_21 = (rb_state9.var_21) - (((0xf37e44c8) - (0x98a178db)) ^ (rb_state9.var_19));
            rb_state9.var_22 = (rb_state9.var_22) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_20) : (0x38026be3));
            rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_22) : (0x8cba5994));
            rb_state9.var_13 = (rb_state9.var_13) ^ (rb_state9.var_23);
            rb_state9.var_2 = rb_state9.var_13;
        }
        if ((rb_state9.var_0) == (0xc0)) {
            pthread_mutex_lock(&(rb_state9.lock_28));
            rb_state9.var_25 = 0xc00c9d1d;
            rb_state9.var_26 = (rb_state9.var_26) + (((((0x712bc85f) ^ (rb_state9.var_25)) + (rb_state9.var_9)) - (rb_state9.var_11)) ^ (0xd41ea293));
            rb_state9.var_27 = (rb_state9.var_27) - (((((0x31dd00c8) ^ (0x94a7e3)) + (rb_state9.var_10)) + (rb_state9.var_12)) ^ (rb_state9.var_26));
            rb_state9.var_24 = (rb_state9.var_24) + (rb_state9.var_27);
            rb_state9.var_2 = rb_state9.var_24;
            pthread_mutex_unlock(&(rb_state9.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0x63975dd6)) {
            if ((rb_state16.var_3) != (0x0)) {
                if (!((rb_state16.var_3) == (rb_state16.var_24))) {
                    racebench_trigger(16);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0xee47bc12)) {
            pthread_mutex_lock(&(rb_state17.lock_14));
            rb_state17.var_4 = 0xd0a1a407;
            rb_state17.var_5 = 0x855c4e0d;
            rb_state17.var_6 = (rb_state17.var_6) + (rb_state17.var_3);
            rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_4) : (0x9de3b9b2));
            rb_state17.var_8 = (rb_state17.var_8) + (((0xd4c64027) + (rb_state17.var_5)) - (0x87f4357));
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_6) : (0x4685c047));
            rb_state17.var_10 = (rb_state17.var_10) + (((0x7ad54a97) ^ (rb_state17.var_7)) - (0xdb9d4f73));
            rb_state17.var_11 = (rb_state17.var_11) ^ (((((0xcae29f9c) + (rb_state17.var_8)) + (rb_state17.var_9)) ^ (0x37722633)) ^ (rb_state17.var_5));
            rb_state17.var_12 = (rb_state17.var_12) - (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_10) : (0xe5ef5cb9));
            rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_11) == (0x13266fb0)) ? (rb_state17.var_12) : (0x532d0a1));
            rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_13);
            rb_state17.var_3 = rb_state17.var_2;
            pthread_mutex_unlock(&(rb_state17.lock_14));
        }
        rb_state17.var_7 = (rb_state17.var_7) + (416 < rb_input_size ? (uint32_t)rb_input[416] : 0x8db2096f);
        rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_9) : (0x67b728e1));
        if ((rb_state17.var_12) == (0x4b3903bb)) {
            rb_state17.var_22 = 0xe07a38b7;
            rb_state17.var_23 = (rb_state17.var_23) + (rb_state17.var_21);
            rb_state17.var_24 = (rb_state17.var_24) - ((0xb7fef8a3) + (0xc42eb891));
            rb_state17.var_25 = (rb_state17.var_25) - (335 < rb_input_size ? (uint32_t)rb_input[335] : 0xf84e46ed);
            rb_state17.var_26 = (rb_state17.var_26) ^ (rb_state17.var_22);
            rb_state17.var_27 = (rb_state17.var_27) + (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_20) : (0x38b2a69a));
            rb_state17.var_28 = (rb_state17.var_28) ^ (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_23) : (0x5d4645af));
            rb_state17.var_29 = (rb_state17.var_29) - (rb_state17.var_24);
            rb_state17.var_30 = (rb_state17.var_30) + (rb_state17.var_25);
            rb_state17.var_31 = (rb_state17.var_31) ^ (rb_state17.var_26);
            rb_state17.var_32 = (rb_state17.var_32) - (rb_state17.var_27);
            rb_state17.var_33 = (rb_state17.var_33) ^ (rb_state17.var_28);
            rb_state17.var_34 = (rb_state17.var_34) + (((0x53fbe997) ^ (rb_state17.var_29)) + (rb_state17.var_24));
            rb_state17.var_35 = (rb_state17.var_35) + (rb_state17.var_30);
            rb_state17.var_36 = (rb_state17.var_36) - (((0xfdce99fb) + (rb_state17.var_31)) + (0xaff890c));
            rb_state17.var_37 = (rb_state17.var_37) ^ (rb_state17.var_32);
            rb_state17.var_38 = (rb_state17.var_38) - (((rb_state17.var_25) == (0x0)) ? (rb_state17.var_33) : (0x799e7f53));
            rb_state17.var_39 = (rb_state17.var_39) ^ (rb_state17.var_34);
            rb_state17.var_40 = (rb_state17.var_40) + (((rb_state17.var_26) == (0x0)) ? (rb_state17.var_35) : (0xd8933688));
            rb_state17.var_41 = (rb_state17.var_41) ^ (((0xd206c7ec) ^ (rb_state17.var_36)) - (0xe296ad2b));
            rb_state17.var_42 = (rb_state17.var_42) ^ (((rb_state17.var_37) == (0x0)) ? (rb_state17.var_38) : (0xdf23245e));
            rb_state17.var_43 = (rb_state17.var_43) - (((rb_state17.var_27) == (0x0)) ? (rb_state17.var_39) : (0xfb310dbe));
            rb_state17.var_44 = (rb_state17.var_44) - (rb_state17.var_40);
            rb_state17.var_45 = (rb_state17.var_45) + (((((0x5454e058) - (rb_state17.var_29)) ^ (rb_state17.var_28)) + (rb_state17.var_42)) ^ (rb_state17.var_41));
            rb_state17.var_46 = (rb_state17.var_46) ^ (((0x37fc5db0) + (rb_state17.var_43)) - (rb_state17.var_30));
            rb_state17.var_47 = (rb_state17.var_47) - (((rb_state17.var_31) == (0x0)) ? (rb_state17.var_44) : (0xa9cde672));
            rb_state17.var_48 = (rb_state17.var_48) - (((((0xc082fa94) ^ (0x258ac)) ^ (rb_state17.var_46)) - (rb_state17.var_45)) - (0xd2d3db2f));
            rb_state17.var_49 = (rb_state17.var_49) ^ (((0xaa81c49f) ^ (rb_state17.var_47)) ^ (0xa14592c1));
            rb_state17.var_50 = (rb_state17.var_50) - (((0xa2705c28) - (rb_state17.var_48)) ^ (0x3590caac));
            rb_state17.var_51 = (rb_state17.var_51) - (((rb_state17.var_32) == (0x0)) ? (rb_state17.var_49) : (0x1f79fbb7));
            rb_state17.var_52 = (rb_state17.var_52) + (rb_state17.var_50);
            rb_state17.var_53 = (rb_state17.var_53) ^ (((((0x4ea30132) + (rb_state17.var_33)) ^ (rb_state17.var_52)) - (rb_state17.var_51)) + (0xdb8ea5a2));
            rb_state17.var_21 = (rb_state17.var_21) ^ (rb_state17.var_53);
            rb_state17.var_16 = rb_state17.var_21;
        }
        if ((rb_state17.var_12) == (0x4b3903bb)) {
            pthread_mutex_lock(&(rb_state17.lock_65));
            rb_state17.var_55 = 0xe22c3307;
            rb_state17.var_56 = 0x1670562d;
            rb_state17.var_57 = (rb_state17.var_57) + (((rb_state17.var_55) == (0x0)) ? (rb_state17.var_34) : (0xeda81929));
            rb_state17.var_58 = (rb_state17.var_58) + (((0x5b9e174d) - (rb_state17.var_35)) + (rb_state17.var_56));
            rb_state17.var_59 = (rb_state17.var_59) ^ (((rb_state17.var_36) == (0x0)) ? (rb_state17.var_57) : (0xc82f2241));
            rb_state17.var_60 = (rb_state17.var_60) + (((rb_state17.var_37) == (0x0)) ? (rb_state17.var_58) : (0x4d65134c));
            rb_state17.var_61 = (rb_state17.var_61) + (((rb_state17.var_38) == (0x0)) ? (rb_state17.var_59) : (0x6289fb5d));
            rb_state17.var_62 = (rb_state17.var_62) ^ (((rb_state17.var_39) == (0x0)) ? (rb_state17.var_60) : (0x8b9c69ae));
            rb_state17.var_63 = (rb_state17.var_63) - (rb_state17.var_61);
            rb_state17.var_64 = (rb_state17.var_64) + (((((0xc848a237) ^ (rb_state17.var_63)) ^ (0x6b6b3b45)) - (0xf0b594e4)) + (rb_state17.var_62));
            rb_state17.var_54 = (rb_state17.var_54) ^ (rb_state17.var_64);
            rb_state17.var_16 = rb_state17.var_54;
            pthread_mutex_unlock(&(rb_state17.lock_65));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) - ((0x88999b49) + (rb_state19.var_1));
        #endif
        m_suspended = value;
        code = MultiThreadedSyncPrimitive::suspend();
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (251 < rb_input_size ? (uint32_t)rb_input[251] : 0xda2781b8);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_2) : (0x3e63e75a));
        #endif
        code = unlock();
    }

    _INLINE void resume() {
        m_suspended = false;
        MultiThreadedSyncPrimitive::resume();
    }

private:
    int m_suspended;
};

class MultiThreadedTaskQueueServer : public MultiThreadedTaskQueue, public MultiThreadedSyncPrimitive {

protected:
    MultiThreadedTaskQueueServer(int) : MultiThreadedTaskQueue(), MultiThreadedSyncPrimitive(), m_thread(NULL) {
        PRINT(this);
        m_finished_jobs = 0;
        memset(m_waitingClients, 0, sizeof(m_waitingClients));
    }

    ~MultiThreadedTaskQueueServer() {
        clean(false);
    }

    _INLINE void clean(bool force_exit_from_thread_function = true) {
        if (m_threads <= 0) {
            return;
        }

        if (force_exit_from_thread_function) {
            lock();

            int nt = m_threads;
            m_threads = -1;

            for (int i = 0; i < nt; i++) {
                m_scheduler[i].resume();
            }
            unlock();

            int i = 0;
            while (i < nt) {
                for (; i < nt; i++) {
                    if (m_scheduler[i].suspended() != -1) {

                        sched_yield();
                        break;
                    }
                }
            }
        }

        if (m_thread) {
            delete[] m_thread;
        }
        if (m_client) {
            delete[] m_client;
        }
        if (m_scheduler) {
            delete[] m_scheduler;
        }
    }

public:
    friend class MultiThreadedTaskQueue;

    using MultiThreadedSyncPrimitive::suspend;
    _INLINE static void suspend(int i) {
        assert(m_client[i] == NULL);
        m_scheduler[i].suspend();
    }

    using MultiThreadedSyncPrimitive::resume;
    using MultiThreadedSyncPrimitive::resumeAll;
    _INLINE static void resume(int i) {
        assert(m_client[i]);
        m_scheduler[i].resume();
    }

    void createThreads(int threads) {
        assert(threads >= 1);

        clean();

        typedef MultiThreadedTaskQueue *pMultiThreadedTaskQueue;
        m_threads = threads;
        m_thread = new pthread_t[threads];
        m_client = new pMultiThreadedTaskQueue[threads];
        m_scheduler = new MultiThreadedScheduler[threads];
        memset(m_client, 0, sizeof(pMultiThreadedTaskQueue) * threads);

        int res;

        for (int i = 0; i < m_threads; i++) {
            if ((res = pthread_create(&m_thread[i], NULL, &threadFunc, (void *)i)) != 0) {
                cerr << "can't create thread " << i;
                if (res == EAGAIN) {
                    cerr << " (the system does not have enough resources)" << endl;
                } else {
                    cerr << ". reason : " << res << endl;
                }
                exit(EXIT_FAILURE);
            }
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0xfffff918)) ? (rb_state1.var_2) : (0x6726ea9d));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (264 < rb_input_size ? (uint32_t)rb_input[264] : 0xb1454081);
        rb_state2.var_1 = (rb_state2.var_1) + (0x1b95a2a8);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (329 < rb_input_size ? (uint32_t)rb_input[329] : 0x6da0be7a);
        rb_state5.var_1 = (rb_state5.var_1) - (0x85ae190a);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0xaa233c16) + (0x60df673));
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0xb0313289)) ? (rb_state6.var_0) : (0xe1f7ea42));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (117 < rb_input_size ? (uint32_t)rb_input[117] : 0x1e4e94aa);
        #endif
        waitUntillAllThreadsAreSuspended();
    }

    _INLINE bool finished() const {
        return m_threads == -1;
    }

private:
    _INLINE void waitUntillAllThreadsAreSuspended() {
        int ns;
        do {

            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_2) == (0x0)) {
                rb_state3.var_15 = 0xf09a1619;
                rb_state3.var_16 = 0x44b14815;
                rb_state3.var_17 = 0xc54456d8;
                rb_state3.var_18 = (rb_state3.var_18) ^ ((0x491225a7) - (rb_state3.var_9));
                rb_state3.var_19 = (rb_state3.var_19) ^ (rb_state3.var_15);
                rb_state3.var_20 = (rb_state3.var_20) ^ (((0x1a4b1ade) - (rb_state3.var_10)) ^ (rb_state3.var_16));
                rb_state3.var_21 = (rb_state3.var_21) - (((0xb55d83af) - (rb_state3.var_17)) + (0x8e792327));
                rb_state3.var_22 = (rb_state3.var_22) + (((0x5c0079f6) + (rb_state3.var_11)) ^ (rb_state3.var_18));
                rb_state3.var_23 = (rb_state3.var_23) - (rb_state3.var_19);
                rb_state3.var_24 = (rb_state3.var_24) + (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_20) : (0xf5010054));
                rb_state3.var_25 = (rb_state3.var_25) + (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_21) : (0x5f38c4fa));
                rb_state3.var_26 = (rb_state3.var_26) + (((((0x2df41) ^ (0xbd0d58a8)) + (rb_state3.var_22)) ^ (rb_state3.var_23)) - (rb_state3.var_14));
                rb_state3.var_27 = (rb_state3.var_27) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_24) : (0xfc221e42));
                rb_state3.var_28 = (rb_state3.var_28) + (((((0xa0507cdd) - (rb_state3.var_26)) + (rb_state3.var_25)) - (0x4eae1c8c)) ^ (0x1427304));
                rb_state3.var_29 = (rb_state3.var_29) + (rb_state3.var_27);
                rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_28);
                rb_state3.var_31 = (rb_state3.var_31) ^ (((((0xbfa7dc3b) - (rb_state3.var_29)) - (0x91fad774)) ^ (rb_state3.var_30)) + (0x74891faf));
                rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_31);
            }
            #endif
            #ifdef RACEBENCH_BUG_5
            rb_state5.var_2 = (rb_state5.var_2) - ((0x96202e9b) - (0x90f09b9c));
            #endif
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0xd371b622)) {
                pthread_mutex_lock(&(rb_state8.lock_26));
                rb_state8.var_5 = 0x371c161f;
                rb_state8.var_6 = 0xa1c9cb2e;
                rb_state8.var_7 = (rb_state8.var_7) - (rb_state8.var_4);
                rb_state8.var_8 = (rb_state8.var_8) - (rb_state8.var_5);
                rb_state8.var_9 = (rb_state8.var_9) - (rb_state8.var_6);
                rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_5) == (0x371c161f)) ? (rb_state8.var_3) : (0xf6517e4c));
                rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0x9e77899));
                rb_state8.var_12 = (rb_state8.var_12) - (((0xf6b85949) + (rb_state8.var_9)) ^ (rb_state8.var_6));
                rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x3108eafe));
                rb_state8.var_14 = (rb_state8.var_14) - (((0x9cd73372) - (rb_state8.var_12)) - (0xd8c5bd35));
                rb_state8.var_15 = (rb_state8.var_15) + (((((0x83e9e280) ^ (rb_state8.var_7)) - (0x3607fc5d)) + (rb_state8.var_14)) ^ (rb_state8.var_13));
                rb_state8.var_4 = (rb_state8.var_4) - (rb_state8.var_15);
                rb_state8.var_3 = rb_state8.var_4;
                pthread_mutex_unlock(&(rb_state8.lock_26));
            }
            if ((rb_state8.var_0) == (0xd371b622)) {
                rb_state8.var_27 = !((rb_state8.var_3) == (rb_state8.var_4));
            }
            #endif
            #ifdef RACEBENCH_BUG_9
            rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
            rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_1);
            if ((rb_state9.var_0) == (0xc0)) {
                if ((rb_state9.var_2) != (0x0)) {
                    if (!((rb_state9.var_2) == (rb_state9.var_24))) {
                        racebench_trigger(9);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_10
            rb_state10.var_0 = (rb_state10.var_0) - (0x3435ec31);
            rb_state10.var_0 = (rb_state10.var_0) ^ (0x2761c40d);
            #endif
            #ifdef RACEBENCH_BUG_16
            rb_state16.var_1 = (rb_state16.var_1) + ((0x6dc5b46f) + (0x9d7d50e3));
            #endif
            sched_yield();
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_1) == (0x5f811e1c)) {
                rb_state1.var_3 = rb_state1.var_2;
            }
            if ((rb_state1.var_1) == (0x5f811e1c)) {
                if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                    racebench_trigger(1);
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_0);
            #endif
            ns = 0;
            for (int i = 0; i < m_threads; i++) {
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_1);
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x1f847062));
                #endif
                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_13) == (0x1)) {
                    pthread_mutex_lock(&(rb_state5.lock_21));
                    rb_state5.var_15 = (rb_state5.var_15) + (((((0xe741c6ef) - (rb_state5.var_3)) + (0x7d612a7f)) - (rb_state5.var_15)) - (rb_state5.var_2));
                    rb_state5.var_16 = (rb_state5.var_16) - ((0xcab49044) - (0xa20d1335));
                    rb_state5.var_17 = (rb_state5.var_17) ^ (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_14) : (0x97e15d8a));
                    rb_state5.var_18 = (rb_state5.var_18) ^ (rb_state5.var_15);
                    rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_17) : (0x95e834a2));
                    rb_state5.var_20 = (rb_state5.var_20) + (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_19) : (0xf66cc4dd));
                    rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_20);
                    pthread_mutex_unlock(&(rb_state5.lock_21));
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_1);
                rb_state9.var_1 = (rb_state9.var_1) - (334 < rb_input_size ? (uint32_t)rb_input[334] : 0x86f3f08d);
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xcbcc094f));
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_1 = (rb_state17.var_1) - (0x55ece140);
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_1) == (0x0)) {
                    if ((rb_state18.var_2) != (0x0)) {
                        if (!((rb_state18.var_2) == (rb_state18.var_18))) {
                            racebench_trigger(18);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_2) == (0x4da6bf14)) {
                    rb_state19.var_4 = 0x9629b55c;
                    rb_state19.var_5 = 0x5ab49d0e;
                    rb_state19.var_6 = (rb_state19.var_6) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_4) : (0x7b840c55));
                    rb_state19.var_7 = (rb_state19.var_7) + (((((0xd872d3ee) + (rb_state19.var_6)) + (rb_state19.var_5)) - (rb_state19.var_7)) - (rb_state19.var_3));
                    rb_state19.var_8 = (rb_state19.var_8) - (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_4) : (0x4a4ea29a));
                    rb_state19.var_9 = (rb_state19.var_9) + (((0x6e65d5dd) - (0x967f542)) ^ (rb_state19.var_6));
                    rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_7) : (0x49370578));
                    rb_state19.var_11 = (rb_state19.var_11) - (((((0xc1daa9f6) - (rb_state19.var_10)) ^ (rb_state19.var_8)) ^ (0x9c537aaa)) ^ (rb_state19.var_9));
                    rb_state19.var_12 = (rb_state19.var_12) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_10) : (0x784cec8));
                    rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_11) : (0x35ae8f69));
                    rb_state19.var_14 = (rb_state19.var_14) - (((0xd5b5290d) ^ (0xa760edb0)) ^ (rb_state19.var_12));
                    rb_state19.var_15 = (rb_state19.var_15) ^ (rb_state19.var_13);
                    rb_state19.var_16 = (rb_state19.var_16) ^ (((((0xfd1f64b) + (rb_state19.var_13)) - (0xdd27fd7a)) + (rb_state19.var_14)) ^ (rb_state19.var_15));
                    rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_16);
                }
                #endif
                ns += m_scheduler[i].suspended();
            }
        } while (ns != m_threads);
    }

    static void *threadFunc(void *_id) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (82 < rb_input_size ? (uint32_t)rb_input[82] : 0x46f4c340);
        rb_state1.var_1 = (rb_state1.var_1) + (132 < rb_input_size ? (uint32_t)rb_input[132] : 0x3649d71);
        rb_state1.var_1 = (rb_state1.var_1) - (0x281fb8ed);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0xd9914a5b);
        rb_state4.var_2 = (rb_state4.var_2) - (257 < rb_input_size ? (uint32_t)rb_input[257] : 0x3dbf0768);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x5bb8e051));
        if ((rb_state5.var_2) == (0xfad06d01)) {
            pthread_mutex_lock(&(rb_state5.lock_12));
            rb_state5.var_5 = 0x5581d93;
            rb_state5.var_6 = (rb_state5.var_6) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_4) : (0x1ce61236));
            rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_5) : (0xbd34ce91));
            rb_state5.var_8 = (rb_state5.var_8) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0x423b170c));
            rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_8);
            rb_state5.var_3 = rb_state5.var_4;
            pthread_mutex_unlock(&(rb_state5.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xbabdd409));
        rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0xbb88f946));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_1) : (0x5674d748));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (0x7d75a97c);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (217 < rb_input_size ? (uint32_t)rb_input[217] : 0xad524559);
        rb_state16.var_0 = (rb_state16.var_0) ^ (((rb_state16.var_0) == (0x20)) ? (rb_state16.var_0) : (0xe2bf722d));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (0x5fc37e);
        rb_state19.var_1 = (rb_state19.var_1) ^ (0xcb5c7103);
        #endif
        long long int id = (long long int)_id;

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - ((0xd13148de) - (rb_state2.var_1));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_0);
        rb_state4.var_1 = (rb_state4.var_1) - (((rb_state4.var_0) == (0xd9914a5b)) ? (rb_state4.var_2) : (0x7f71a911));
        rb_state4.var_1 = (rb_state4.var_1) ^ (108 < rb_input_size ? (uint32_t)rb_input[108] : 0xe9a6f80);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ ((0x37618e41) + (0x24f6d56f));
        if ((rb_state5.var_2) == (0xfad06d01)) {
            rb_state5.var_13 = !((rb_state5.var_3) == (rb_state5.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - ((0x35e2fed) - (rb_state6.var_1));
        rb_state6.var_1 = (rb_state6.var_1) + ((0x2f2d3db5) + (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (0x977d8384);
        rb_state8.var_2 = (rb_state8.var_2) + (385 < rb_input_size ? (uint32_t)rb_input[385] : 0x5c6ba359);
        rb_state8.var_2 = (rb_state8.var_2) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_2) : (0xcd31070b));
        rb_state8.var_2 = (rb_state8.var_2) ^ (0x4847c0b1);
        if ((rb_state8.var_0) == (0xd371b622)) {
            rb_state8.var_16 = 0xba6d8c5a;
            rb_state8.var_17 = 0x621432c5;
            rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_16) : (0x85b9d66a));
            rb_state8.var_19 = (rb_state8.var_19) ^ (rb_state8.var_9);
            rb_state8.var_20 = (rb_state8.var_20) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_17) : (0x8ee6b25a));
            rb_state8.var_21 = (rb_state8.var_21) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_18) : (0x31e00603));
            rb_state8.var_22 = (rb_state8.var_22) + (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_20) : (0xaa29c0cc));
            rb_state8.var_23 = (rb_state8.var_23) - (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_21) : (0x420dc57));
            rb_state8.var_24 = (rb_state8.var_24) - (((0x9973fed1) + (rb_state8.var_13)) ^ (rb_state8.var_22));
            rb_state8.var_25 = (rb_state8.var_25) - (((rb_state8.var_23) == (0x0)) ? (rb_state8.var_24) : (0x82daa8ff));
            rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_25);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
        rb_state13.var_1 = (rb_state13.var_1) ^ (0x44bbb21a);
        rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x514ad08)) ? (rb_state13.var_1) : (0xa5e093cc));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (50 < rb_input_size ? (uint32_t)rb_input[50] : 0xd40a5a17);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (0x7672eede);
        rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x7672eede)) ? (rb_state17.var_0) : (0x11114f92));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + ((0x40a0f4f0) + (0x98eb21e4));
        rb_state18.var_0 = (rb_state18.var_0) ^ (0x8f9256fd);
        rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (282 < rb_input_size ? (uint32_t)rb_input[282] : 0x481b3917);
        #endif
        _mm_setcsr(_mm_getcsr() | (1 << 15) | (1 << 6));

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + ((0x4b3ead3a) - (0x87d5112a));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_2);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0xb37d9450)) {
            rb_state2.var_2 = rb_state2.var_1;
        }
        if ((rb_state2.var_0) == (0xb37d9450)) {
            if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_1) : (0x8516f29a));
        rb_state3.var_1 = (rb_state3.var_1) ^ (0xeb1ea7b);
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_1);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) - (((rb_state4.var_1) == (0x286cd31a)) ? (rb_state4.var_0) : (0x1eb29e9));
        if ((rb_state4.var_2) == (0x3375ad00)) {
            pthread_mutex_lock(&(rb_state4.lock_19));
            rb_state4.var_3 = rb_state4.var_0;
            pthread_mutex_unlock(&(rb_state4.lock_19));
        }
        if ((rb_state4.var_2) == (0xcd308394)) {
            rb_state4.var_4 = 0xeee73ec;
            rb_state4.var_5 = 0xae94fb33;
            rb_state4.var_6 = (rb_state4.var_6) ^ (0xf7504a09);
            rb_state4.var_7 = (rb_state4.var_7) - (((rb_state4.var_4) == (0xeee73ec)) ? (rb_state4.var_5) : (0x44d2a24));
            rb_state4.var_8 = (rb_state4.var_8) ^ (((rb_state4.var_5) == (0xae94fb33)) ? (rb_state4.var_4) : (0xbc1eb922));
            rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_3) == (0x6645296c)) ? (rb_state4.var_6) : (0xf8cadb7d));
            rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_7);
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_8) == (0xeee73ec)) ? (rb_state4.var_9) : (0xe27391f4));
            rb_state4.var_12 = (rb_state4.var_12) + (rb_state4.var_10);
            rb_state4.var_13 = (rb_state4.var_13) - (((0x3bedf7ed) + (0xbe86ce5a)) - (rb_state4.var_11));
            rb_state4.var_14 = (rb_state4.var_14) ^ (rb_state4.var_12);
            rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_13);
            rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_14);
            rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_6) == (0xf7504a09)) ? (rb_state4.var_15) : (0x478ce296));
            rb_state4.var_18 = (rb_state4.var_18) + (((rb_state4.var_16) == (0x516b04cd)) ? (rb_state4.var_17) : (0x94f2c13b));
            rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_18);
        }
        if ((rb_state4.var_2) == (0xcd308394)) {
            pthread_mutex_lock(&(rb_state4.lock_19));
            if (!((rb_state4.var_0) == (rb_state4.var_3))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_19));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_1);
        if ((rb_state5.var_2) == (0xfad06d01)) {
            pthread_mutex_lock(&(rb_state5.lock_12));
            rb_state5.var_9 = 0x90b81383;
            rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_6) == (0x1ce61236)) ? (rb_state5.var_9) : (0x7ac469ce));
            rb_state5.var_11 = (rb_state5.var_11) + (((((0x2ec54ae4) ^ (0x16f654f9)) - (0x6deb93b5)) ^ (rb_state5.var_7)) + (rb_state5.var_10));
            rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_11);
            pthread_mutex_unlock(&(rb_state5.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (0x9fd397cc);
        if ((rb_state6.var_1) == (0xbf233beb)) {
            pthread_mutex_lock(&(rb_state6.lock_22));
            rb_state6.var_4 = 0x98af05bd;
            rb_state6.var_5 = (rb_state6.var_5) + (rb_state6.var_2);
            rb_state6.var_6 = (rb_state6.var_6) ^ (0xc73a182b);
            rb_state6.var_7 = (rb_state6.var_7) + (((((0x25844861) - (0xec3dc4ea)) + (rb_state6.var_4)) ^ (rb_state6.var_3)) ^ (rb_state6.var_4));
            rb_state6.var_8 = (rb_state6.var_8) + (((0x585fc470) ^ (rb_state6.var_5)) ^ (rb_state6.var_5));
            rb_state6.var_9 = (rb_state6.var_9) - (((0x35ffa873) - (rb_state6.var_6)) + (0xdeb1d8c7));
            rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_7) : (0x7ddcca9a));
            rb_state6.var_11 = (rb_state6.var_11) + (rb_state6.var_8);
            rb_state6.var_12 = (rb_state6.var_12) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0xbd5eae01));
            rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_11) : (0x90b70308));
            rb_state6.var_14 = (rb_state6.var_14) + (((0xd8e932fb) - (rb_state6.var_12)) - (rb_state6.var_8));
            rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_13) : (0xf8b1120b));
            rb_state6.var_16 = (rb_state6.var_16) - (((((0x68b09ecc) ^ (rb_state6.var_14)) - (rb_state6.var_10)) - (rb_state6.var_15)) - (rb_state6.var_11));
            rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_16);
            rb_state6.var_2 = rb_state6.var_3;
            pthread_mutex_unlock(&(rb_state6.lock_22));
        }
        if ((rb_state6.var_1) == (0xbf233beb)) {
            if (!((rb_state6.var_2) == (rb_state6.var_3))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_0);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x44bbb23a)) ? (rb_state13.var_0) : (0x25f4cb1e));
        if ((rb_state13.var_1) == (0x30898a5c)) {
            pthread_mutex_lock(&(rb_state13.lock_30));
            rb_state13.var_9 = 0x3e896b33;
            rb_state13.var_10 = (rb_state13.var_10) ^ (((((0x6e53109e) - (0x5bcd6351)) + (rb_state13.var_9)) + (rb_state13.var_5)) ^ (0xc88effea));
            rb_state13.var_11 = (rb_state13.var_11) + (rb_state13.var_4);
            rb_state13.var_12 = (rb_state13.var_12) - (((rb_state13.var_10) == (0x9981e76a)) ? (rb_state13.var_11) : (0x24f4a28f));
            rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_12);
            rb_state13.var_2 = rb_state13.var_8;
            pthread_mutex_unlock(&(rb_state13.lock_30));
        }
        if ((rb_state13.var_1) == (0x30898a5c)) {
            if ((rb_state13.var_2) != (0x0)) {
                if (!((rb_state13.var_2) == (rb_state13.var_13))) {
                    racebench_trigger(13);
                }
            }
        }
        if ((rb_state13.var_1) == (0x30898a5c)) {
            pthread_mutex_lock(&(rb_state13.lock_30));
            rb_state13.var_14 = 0x21795043;
            rb_state13.var_15 = 0x3d444f11;
            rb_state13.var_16 = (rb_state13.var_16) - (((0x8d8c2044) - (0xfb53de28)) ^ (rb_state13.var_15));
            rb_state13.var_17 = (rb_state13.var_17) ^ ((0xd1dabd27) - (rb_state13.var_7));
            rb_state13.var_18 = (rb_state13.var_18) ^ (((((0x85640bce) - (0x21dacbd5)) ^ (rb_state13.var_14)) + (0xda9f3262)) - (rb_state13.var_6));
            rb_state13.var_19 = (rb_state13.var_19) - (((0x500e4fa5) ^ (rb_state13.var_16)) + (rb_state13.var_8));
            rb_state13.var_20 = (rb_state13.var_20) - (((0x76963309) - (rb_state13.var_9)) - (rb_state13.var_17));
            rb_state13.var_21 = (rb_state13.var_21) ^ (((((0x34ad6a28) - (rb_state13.var_19)) - (0x9d9eb294)) + (rb_state13.var_18)) ^ (rb_state13.var_10));
            rb_state13.var_22 = (rb_state13.var_22) ^ (rb_state13.var_20);
            rb_state13.var_23 = (rb_state13.var_23) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_21) : (0xe14182a0));
            rb_state13.var_24 = (rb_state13.var_24) - (((rb_state13.var_12) == (0xdb0b5d71)) ? (rb_state13.var_22) : (0x829b6f15));
            rb_state13.var_25 = (rb_state13.var_25) + (((0x4f81ebe9) + (rb_state13.var_13)) - (rb_state13.var_23));
            rb_state13.var_26 = (rb_state13.var_26) - (((0xaaa0960c) ^ (0x6b4e07c)) + (rb_state13.var_24));
            rb_state13.var_27 = (rb_state13.var_27) + (((0x76d7e61f) - (0xc2a358ce)) ^ (rb_state13.var_25));
            rb_state13.var_28 = (rb_state13.var_28) ^ (((0x9f097c3f) - (rb_state13.var_26)) ^ (0xd758a2e6));
            rb_state13.var_29 = (rb_state13.var_29) ^ (((rb_state13.var_27) == (0x6b8ded31)) ? (rb_state13.var_28) : (0xab39c5a7));
            rb_state13.var_13 = (rb_state13.var_13) ^ (rb_state13.var_29);
            rb_state13.var_2 = rb_state13.var_13;
            pthread_mutex_unlock(&(rb_state13.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (58 < rb_input_size ? (uint32_t)rb_input[58] : 0x3885e5fe);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (0x3a2ff2ef);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + ((0xd21aba8e) ^ (0xc40facaf));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + ((0xa5c2c6e5) ^ (rb_state17.var_1));
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x2232b979)) ? (rb_state17.var_1) : (0x29a02846));
        if ((rb_state17.var_0) == (0xee47bc12)) {
            rb_state17.var_15 = !((rb_state17.var_3) == (rb_state17.var_2));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0xd336d6d8)) ? (rb_state18.var_0) : (0x19915287));
        rb_state18.var_1 = (rb_state18.var_1) + ((0xe9907eb7) + (0x6f26f754));
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_2) == (0x4da6bf14)) {
            rb_state19.var_3 = rb_state19.var_0;
        }
        if ((rb_state19.var_2) == (0x4da6bf14)) {
            rb_state19.var_18 = !((rb_state19.var_0) == (rb_state19.var_3));
        }
        #endif
        m_scheduler[id].suspend();

        int seqn;
        long long int tag = -1;
        while (!m_server.finished()) {
            MultiThreadedTaskQueue *client = MultiThreadedTaskQueue::m_client[id];

            #ifdef RACEBENCH_BUG_3
            rb_state3.var_20 = (rb_state3.var_20) ^ (((rb_state3.var_22) == (0xf05b870f)) ? (rb_state3.var_21) : (0x724a12a9));
            #endif
            int action = 0;

            assert(client);
            if (tag != client->tag()) {
                tag = client->tag();
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_1 = (rb_state12.var_1) - (233 < rb_input_size ? (uint32_t)rb_input[233] : 0xd38d1c38);
                #endif
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_18) == (0x1)) {
                    rb_state19.var_35 = 0x1212ae3;
                    rb_state19.var_36 = (rb_state19.var_36) + ((0x7669a053) + (rb_state19.var_26));
                    rb_state19.var_37 = (rb_state19.var_37) ^ (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_35) : (0x5127cf1f));
                    rb_state19.var_38 = (rb_state19.var_38) ^ (((rb_state19.var_25) == (0x0)) ? (rb_state19.var_36) : (0xca00a619));
                    rb_state19.var_39 = (rb_state19.var_39) - (((0x83125273) - (0x3169f7b2)) - (rb_state19.var_37));
                    rb_state19.var_40 = (rb_state19.var_40) - (((0xb7d89146) ^ (rb_state19.var_38)) - (rb_state19.var_27));
                    rb_state19.var_41 = (rb_state19.var_41) ^ (rb_state19.var_39);
                    rb_state19.var_42 = (rb_state19.var_42) + (((rb_state19.var_28) == (0x0)) ? (rb_state19.var_40) : (0xa86a7aae));
                    rb_state19.var_43 = (rb_state19.var_43) ^ (((rb_state19.var_29) == (0x0)) ? (rb_state19.var_41) : (0xe1999f60));
                    rb_state19.var_44 = (rb_state19.var_44) ^ (rb_state19.var_42);
                    rb_state19.var_45 = (rb_state19.var_45) + (((rb_state19.var_43) == (0x0)) ? (rb_state19.var_44) : (0x8694e74c));
                    rb_state19.var_34 = (rb_state19.var_34) - (rb_state19.var_45);
                    rb_state19.var_19 = rb_state19.var_34;
                }
                if ((rb_state19.var_18) == (0x1)) {
                    pthread_mutex_lock(&(rb_state19.lock_61));
                    rb_state19.var_47 = 0xfc778423;
                    rb_state19.var_48 = 0xb311571;
                    rb_state19.var_49 = (rb_state19.var_49) ^ (((rb_state19.var_31) == (0x0)) ? (rb_state19.var_48) : (0xe30c315b));
                    rb_state19.var_50 = (rb_state19.var_50) - (((0xdab536ab) - (rb_state19.var_32)) - (rb_state19.var_30));
                    rb_state19.var_51 = (rb_state19.var_51) ^ (((0xcdf14d4c) - (rb_state19.var_47)) - (rb_state19.var_33));
                    rb_state19.var_52 = (rb_state19.var_52) - (((rb_state19.var_49) == (0x0)) ? (rb_state19.var_50) : (0x1f3de74e));
                    rb_state19.var_53 = (rb_state19.var_53) + (((0xd4f8be0d) + (rb_state19.var_51)) ^ (0xc3e59ed7));
                    rb_state19.var_54 = (rb_state19.var_54) + (rb_state19.var_52);
                    rb_state19.var_55 = (rb_state19.var_55) + (((rb_state19.var_34) == (0x0)) ? (rb_state19.var_53) : (0xd8f7d8b4));
                    rb_state19.var_56 = (rb_state19.var_56) + (((0xd959d55e) - (rb_state19.var_54)) + (0xde5e8173));
                    rb_state19.var_57 = (rb_state19.var_57) ^ (((0x670f0788) + (0x76f877d3)) + (rb_state19.var_55));
                    rb_state19.var_58 = (rb_state19.var_58) ^ (((rb_state19.var_35) == (0x0)) ? (rb_state19.var_56) : (0xa6b39034));
                    rb_state19.var_59 = (rb_state19.var_59) - (((0x1fe324c1) - (rb_state19.var_57)) + (rb_state19.var_36));
                    rb_state19.var_60 = (rb_state19.var_60) - (((((0xac6cc6e5) + (rb_state19.var_58)) + (0x30a2177f)) + (rb_state19.var_59)) ^ (0xe596c66a));
                    rb_state19.var_46 = (rb_state19.var_46) - (rb_state19.var_60);
                    rb_state19.var_19 = rb_state19.var_46;
                    pthread_mutex_unlock(&(rb_state19.lock_61));
                }
                #endif
                m_server.lock();
                if (client->m_assigned_jobs >= client->m_threads) {
                    m_server.unlock();
                    goto getnext;
                }
                seqn = client->m_assigned_jobs++;
                #ifdef RACEBENCH_BUG_0
                rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0xda66f08)) ? (rb_state0.var_1) : (0xa94685fe));
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_2) == (0xf73f450b)) {
                    rb_state12.var_24 = 0xd07a1d08;
                    rb_state12.var_25 = 0x6effb6e;
                    rb_state12.var_26 = 0x290551fe;
                    rb_state12.var_27 = (rb_state12.var_27) - (rb_state12.var_18);
                    rb_state12.var_28 = (rb_state12.var_28) ^ (((rb_state12.var_24) == (0x0)) ? (rb_state12.var_25) : (0x8b1edbf8));
                    rb_state12.var_29 = (rb_state12.var_29) - (rb_state12.var_26);
                    rb_state12.var_30 = (rb_state12.var_30) + (((0x3c36c856) ^ (rb_state12.var_19)) - (rb_state12.var_27));
                    rb_state12.var_31 = (rb_state12.var_31) - (rb_state12.var_28);
                    rb_state12.var_32 = (rb_state12.var_32) - (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_29) : (0xcad8b21b));
                    rb_state12.var_33 = (rb_state12.var_33) ^ (rb_state12.var_30);
                    rb_state12.var_34 = (rb_state12.var_34) + (rb_state12.var_31);
                    rb_state12.var_35 = (rb_state12.var_35) - (((0x9d887eb4) + (rb_state12.var_32)) - (0xb246bb0e));
                    rb_state12.var_36 = (rb_state12.var_36) ^ (rb_state12.var_33);
                    rb_state12.var_37 = (rb_state12.var_37) + (rb_state12.var_34);
                    rb_state12.var_38 = (rb_state12.var_38) + (((0x41e366aa) + (rb_state12.var_21)) + (rb_state12.var_35));
                    rb_state12.var_39 = (rb_state12.var_39) + (rb_state12.var_36);
                    rb_state12.var_40 = (rb_state12.var_40) ^ (((rb_state12.var_22) == (0x0)) ? (rb_state12.var_37) : (0xd63915ed));
                    rb_state12.var_41 = (rb_state12.var_41) + (((0xdc9a0ea2) - (rb_state12.var_38)) ^ (rb_state12.var_23));
                    rb_state12.var_42 = (rb_state12.var_42) - (((((0x201f3c6d) + (rb_state12.var_40)) + (0xf0eea44d)) - (rb_state12.var_39)) - (0x69e97837));
                    rb_state12.var_43 = (rb_state12.var_43) - (((0x7e4a5de7) + (rb_state12.var_41)) - (rb_state12.var_24));
                    rb_state12.var_44 = (rb_state12.var_44) + (((rb_state12.var_42) == (0x0)) ? (rb_state12.var_43) : (0xfcc8bf0a));
                    rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_44);
                }
                #endif
                m_server.unlock();
            }
            action = client->task(seqn, (int)id);
            if (action == THREAD_EXIT) {
                break;
            }

        getnext:

            seqn = deactivateThread((int)id);
        }

        m_scheduler[id].suspend(false, -1);
        return NULL;
    }

    _INLINE static int deactivateThread(const int threadID) {

        MultiThreadedTaskQueue *client = m_client[threadID];
        assert(m_client[threadID]);
        m_client[threadID] = NULL;

        int code = m_server.lock();
        int seqn = -1;

        bool done = ++client->m_finished_jobs == client->m_threads;

        if (m_server.schedule(threadID)) {

            if (done) {
                m_server.resume();
            } else {
                seqn = client->m_assigned_jobs++;
            }
            code = m_server.unlock();
        } else {

            if (done) {
                m_server.resume();
            }
            m_scheduler[threadID].lock();
            code = m_server.unlock();
            m_scheduler[threadID].suspend(true);
        }
        return seqn;
    }

    _INLINE int getNextClientIndex() {

        int j = m_finished_jobs;
        for (int i = 0; i < queuesize; i++) {
            if (m_waitingClients[j]) {
                return j;
            }
            if (++j == queuesize) {
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_14 = (rb_state19.var_14) + (0x29d84167);
                #endif
                j = 0;
            }
        }
        return -1;
    }

    void addClient(MultiThreadedTaskQueue *client) {

    recheck:
        m_server.lock();
        int j = m_finished_jobs;
        for (int i = 0; i < queuesize; i++) {
            if (m_waitingClients[j] == NULL) {
                goto found;
            }
            if (++j == queuesize) {
                j = 0;
            }
        }

        m_server.unlock();

        sched_yield();
        goto recheck;

    found:

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_1);
        #endif
        m_waitingClients[j] = client;
        m_scheduledJobs[j] = 0;

        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ ((0xe4baf620) ^ (0x9dbfefbd));
        #endif
        m_finished_jobs = j + 1;
        if (m_finished_jobs == queuesize) {
            m_finished_jobs = 0;
        }

        int nt = m_server.m_threads;

        for (int i = 0; i < nt; i++) {
            m_scheduler[i].lock();
            if (m_scheduler[i].suspended()) {
                if (!schedule(i)) {
                    m_scheduler[i].unlock();
                    break;
                }
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_15 = (rb_state19.var_15) - (22 < rb_input_size ? (uint32_t)rb_input[22] : 0xd0379576);
                #endif
                m_scheduler[i].resume();
            }
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) - (412 < rb_input_size ? (uint32_t)rb_input[412] : 0xe6a3babe);
            #endif
            m_scheduler[i].unlock();
        }

        m_server.unlock();
    }

    bool schedule(int i) {

        if (m_client[i] == NULL) {

            #ifdef RACEBENCH_BUG_3
            rb_state3.var_19 = (rb_state3.var_19) + (rb_state3.var_17);
            #endif
            int ji = getNextClientIndex();
            if (ji == -1) {

                return false;
            }
            m_client[i] = m_waitingClients[ji];

            if (++m_scheduledJobs[ji] == m_waitingClients[ji]->m_threads) {
                m_waitingClients[ji] = NULL;
            }
        }
        return true;
    }

    pthread_t *m_thread;
    static const int queuesize = 4;
    MultiThreadedTaskQueue *m_waitingClients[queuesize];
    int m_scheduledJobs[queuesize];
};

void MultiThreadedTaskQueue::setMaxNumberOfThreads(int threads) {
    PRINT(threads);
    m_server.createThreads(threads);
}

int MultiThreadedTaskQueue::maxNumberOfThreads() {
    return m_server.numberOfThreads();
}

void MultiThreadedTaskQueue::createThreads(int threads) {
    assert(threads >= 1);
    m_threads = threads;
    m_server.lock();
    if (m_server.m_threads == 0) {

        m_server.createThreads(threads);
    }
    m_server.unlock();
}

void MultiThreadedTaskQueue::startThreads() {
    m_finished_jobs = m_assigned_jobs = 0;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (0xb53f980);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_14 = (rb_state8.var_14) + (0xbeb2a025);
    #endif
    m_server.addClient(this);
}

void MultiThreadedTaskQueue::waitForAllThreads() {
    if (m_finished_jobs == m_threads) {
        return;
    }
    int code = m_server.lock();

    while (m_finished_jobs < m_threads) {
        m_server.suspend();
    }

    code = m_server.unlock();
}

MultiThreadedTaskQueueServer MultiThreadedTaskQueue::m_server(1);
MultiThreadedTaskQueue **MultiThreadedTaskQueue::m_client = NULL;
MultiThreadedScheduler *MultiThreadedTaskQueue::m_scheduler = NULL;