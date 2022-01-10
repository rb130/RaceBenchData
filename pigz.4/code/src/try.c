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
    rb_state2.var_1 = (rb_state2.var_1) + ((0x20de3c52) ^ (rb_state2.var_1));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + ((0xfb967248) + (0xfea8d067));
    rb_state3.var_16 = (rb_state3.var_16) + (rb_state3.var_17);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_2);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ ((0x817f6a28) ^ (0xe9db8fdb));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) + (0xaecfc192);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - ((0x6b484871) + (0x6e50face));
    rb_state15.var_3 = (rb_state15.var_3) - (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_3);
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