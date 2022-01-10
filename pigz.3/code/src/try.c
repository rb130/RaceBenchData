#include "racebench_bugs.h"


#include "try.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef PTHREAD_ONCE_INIT
pthread_key_t try_key_;
static pthread_once_t try_once_ = PTHREAD_ONCE_INIT;
static void try_create_(void) {
    int ret = pthread_key_create(&try_key_, NULL);
    assert(ret == 0 && "try: pthread_key_create() failed");
}
void try_setup_(void) {
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (0x393d5738);
    if ((rb_state2.var_12) == (0x78c904bb)) {
        pthread_mutex_lock(&(rb_state2.lock_54));
        if ((rb_state2.var_16) != (0x0)) {
            if (!((rb_state2.var_16) == (rb_state2.var_50))) {
                racebench_trigger(2);
            }
        }
        pthread_mutex_unlock(&(rb_state2.lock_54));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_0);
    if ((rb_state3.var_1) == (0xc7a75ad6)) {
        pthread_mutex_lock(&(rb_state3.lock_8));
        rb_state3.var_3 = 0x60e97485;
        rb_state3.var_4 = 0xeb2e09f9;
        rb_state3.var_5 = 0xcbfae809;
        rb_state3.var_6 = (rb_state3.var_6) + (((((0x7c408f9) + (rb_state3.var_2)) - (rb_state3.var_3)) ^ (rb_state3.var_3)) + (rb_state3.var_4));
        rb_state3.var_7 = (rb_state3.var_7) ^ (((((0x24f4c27f) ^ (rb_state3.var_4)) ^ (rb_state3.var_6)) ^ (rb_state3.var_5)) ^ (rb_state3.var_5));
        rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_7);
        pthread_mutex_unlock(&(rb_state3.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_3) == (0xead78dda)) {
        rb_state16.var_22 = 0x92462059;
        rb_state16.var_23 = (rb_state16.var_23) - (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_18) : (0x7ea4f9d));
        rb_state16.var_24 = (rb_state16.var_24) + (0x8a5a41ab);
        rb_state16.var_25 = (rb_state16.var_25) - (rb_state16.var_19);
        rb_state16.var_26 = (rb_state16.var_26) - (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_22) : (0xcba4a1d9));
        rb_state16.var_27 = (rb_state16.var_27) + (((0x4e4b1dc1) - (rb_state16.var_23)) + (rb_state16.var_21));
        rb_state16.var_28 = (rb_state16.var_28) + (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_25) : (0xaf779834));
        rb_state16.var_29 = (rb_state16.var_29) ^ (rb_state16.var_26);
        rb_state16.var_30 = (rb_state16.var_30) - (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_28) : (0x40dd653e));
        rb_state16.var_31 = (rb_state16.var_31) - (rb_state16.var_29);
        rb_state16.var_32 = (rb_state16.var_32) ^ (((rb_state16.var_30) == (0x0)) ? (rb_state16.var_31) : (0x2a33f837));
        rb_state16.var_21 = (rb_state16.var_21) + (rb_state16.var_32);
        rb_state16.var_13 = rb_state16.var_21;
    }
    if ((rb_state16.var_3) == (0xead78dda)) {
        pthread_mutex_lock(&(rb_state16.lock_43));
        rb_state16.var_34 = 0x99249fbf;
        rb_state16.var_35 = 0x22d499dc;
        rb_state16.var_36 = (rb_state16.var_36) ^ (rb_state16.var_34);
        rb_state16.var_37 = (rb_state16.var_37) + (rb_state16.var_35);
        rb_state16.var_38 = (rb_state16.var_38) - (((0xe7f336e3) + (rb_state16.var_22)) ^ (rb_state16.var_23));
        rb_state16.var_39 = (rb_state16.var_39) + (((rb_state16.var_36) == (0x0)) ? (rb_state16.var_37) : (0x5974d338));
        rb_state16.var_40 = (rb_state16.var_40) ^ (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_38) : (0x8fc52061));
        rb_state16.var_41 = (rb_state16.var_41) + (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_39) : (0xd2762ad8));
        rb_state16.var_42 = (rb_state16.var_42) ^ (((rb_state16.var_40) == (0x0)) ? (rb_state16.var_41) : (0x597f2b94));
        rb_state16.var_33 = (rb_state16.var_33) - (rb_state16.var_42);
        rb_state16.var_13 = rb_state16.var_33;
        pthread_mutex_unlock(&(rb_state16.lock_43));
    }
    #endif
    int ret = pthread_once(&try_once_, try_create_);
    assert(ret == 0 && "try: pthread_once() failed");
}
#else
try_t_ *try_stack_ = NULL;
#endif

void try_throw_(int code, char *fmt, ...) {

    try_setup_();
    assert(try_stack_ != NULL && "try: naked throw");
    try_stack_->ball.code = code;
    try_stack_->ball.free = 0;
    try_stack_->ball.why = fmt;

    if (fmt != NULL && strchr(fmt, '%') != NULL) {
        char *why, nul[1];
        size_t len;
        va_list ap1, ap2;

        va_start(ap1, fmt);
        va_copy(ap2, ap1);
        len = vsnprintf(nul, 1, fmt, ap1);
        va_end(ap1);
        why = malloc(len + 1);
        if (why == NULL) {
            try_stack_->ball.why = "try: out of memory";
        } else {
            vsnprintf(why, len + 1, fmt, ap2);
            va_end(ap2);
            try_stack_->ball.free = 1;
            try_stack_->ball.why = why;
        }
    }

    longjmp(try_stack_->env, 2);
}