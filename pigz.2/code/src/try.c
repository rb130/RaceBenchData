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
    if ((rb_state2.var_1) == (0x0)) {
        rb_state2.var_8 = 0x56868cc;
        rb_state2.var_9 = 0x20a30b10;
        rb_state2.var_10 = 0xadba7b6e;
        rb_state2.var_11 = (rb_state2.var_11) + (817 < rb_input_size ? (uint32_t)rb_input[817] : 0x1092ed22);
        rb_state2.var_12 = (rb_state2.var_12) + (((((0x4d83a2a5) ^ (0x7a74c504)) - (0xecdc2f06)) - (rb_state2.var_10)) + (rb_state2.var_9));
        rb_state2.var_13 = (rb_state2.var_13) + (((0x3153f10) + (rb_state2.var_8)) ^ (0x2bcb163c));
        rb_state2.var_14 = (rb_state2.var_14) ^ (rb_state2.var_11);
        rb_state2.var_15 = (rb_state2.var_15) ^ (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_13) : (0xcc269697));
        rb_state2.var_16 = (rb_state2.var_16) ^ (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_14) : (0x855e47a5));
        rb_state2.var_17 = (rb_state2.var_17) ^ (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_16) : (0x778b2f56));
        rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0xda45127d)) {
        rb_state8.var_2 = rb_state8.var_0;
    }
    if ((rb_state8.var_1) == (0xda45127d)) {
        rb_state8.var_4 = !((rb_state8.var_0) == (rb_state8.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + ((0x2dbd3e63) - (0x83501177));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (0xd18a33);
    if ((rb_state11.var_0) == (0x8f)) {
        pthread_mutex_lock(&(rb_state11.lock_5));
        rb_state11.var_3 = 0xd85ba57f;
        rb_state11.var_4 = (rb_state11.var_4) + (((((0x53ff0848) - (0x9d11aee7)) ^ (rb_state11.var_4)) + (rb_state11.var_2)) ^ (rb_state11.var_3));
        rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_4);
        pthread_mutex_unlock(&(rb_state11.lock_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0x7c0b7281) - (rb_state12.var_1));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0xfcb71675) - (0x2f3ffcc7));
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