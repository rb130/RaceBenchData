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
    if ((rb_state2.var_0) == (0xd968c108)) {
        pthread_mutex_lock(&(rb_state2.lock_10));
        rb_state2.var_6 = 0x48f66b9a;
        rb_state2.var_7 = 0xbf7495bc;
        rb_state2.var_8 = (rb_state2.var_8) + (((((0x69f105fc) - (rb_state2.var_6)) - (rb_state2.var_7)) + (rb_state2.var_7)) ^ (rb_state2.var_6));
        rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_8) : (0x3881ad7e));
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_9);
        pthread_mutex_unlock(&(rb_state2.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_2) == (0xffffffc7)) ? (rb_state3.var_1) : (0xadaa97a0));
    rb_state3.var_3 = (rb_state3.var_3) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_3) : (0x17f2d86));
    rb_state3.var_0 = (rb_state3.var_0) + (0x1db98dd7);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_3) == (0x91)) {
        pthread_mutex_lock(&(rb_state7.lock_27));
        rb_state7.var_6 = 0x8448995d;
        rb_state7.var_7 = (rb_state7.var_7) - (((rb_state7.var_6) == (0x8448995d)) ? (rb_state7.var_5) : (0x93a5ebab));
        rb_state7.var_8 = (rb_state7.var_8) + (rb_state7.var_4);
        rb_state7.var_9 = (rb_state7.var_9) ^ (((0xb56a4e92) ^ (rb_state7.var_7)) - (rb_state7.var_6));
        rb_state7.var_10 = (rb_state7.var_10) - (((((0xe802f788) ^ (rb_state7.var_9)) ^ (0x5e466d2c)) ^ (0x7fb9f975)) + (rb_state7.var_8));
        rb_state7.var_5 = (rb_state7.var_5) ^ (rb_state7.var_10);
        rb_state7.var_4 = rb_state7.var_5;
        pthread_mutex_unlock(&(rb_state7.lock_27));
    }
    if ((rb_state7.var_3) == (0x91)) {
        pthread_mutex_lock(&(rb_state7.lock_27));
        if (!((rb_state7.var_4) == (rb_state7.var_5))) {
            racebench_trigger(7);
        }
        pthread_mutex_unlock(&(rb_state7.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - ((0x282e26df) - (0x68530cbf));
    if ((rb_state8.var_4) == (0x0)) {
        if ((rb_state8.var_5) != (0x0)) {
            if (!((rb_state8.var_5) == (rb_state8.var_20))) {
                racebench_trigger(8);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_1) == (0x38deffa5)) {
        rb_state15.var_14 = 0xd5d42273;
        rb_state15.var_15 = 0x69de0567;
        rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_15) : (0x28218d87));
        rb_state15.var_17 = (rb_state15.var_17) - (((((0xdb4beaa5) ^ (rb_state15.var_16)) ^ (0x6753f1b)) - (rb_state15.var_14)) + (0x8f7c15f8));
        rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (0xf08dc4eb);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_2) : (0x78350afb));
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