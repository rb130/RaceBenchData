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
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_4) == (0x33e4ad8)) {
        rb_state11.var_19 = 0xe8a86f9a;
        rb_state11.var_20 = 0x6707cbde;
        rb_state11.var_21 = 0x7b83fde3;
        rb_state11.var_22 = (rb_state11.var_22) ^ (rb_state11.var_9);
        rb_state11.var_23 = (rb_state11.var_23) - (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_20) : (0x8fd58e98));
        rb_state11.var_24 = (rb_state11.var_24) - (((((0xe5d08190) + (rb_state11.var_19)) - (rb_state11.var_11)) ^ (rb_state11.var_21)) - (0xc59c2bac));
        rb_state11.var_25 = (rb_state11.var_25) ^ (rb_state11.var_22);
        rb_state11.var_26 = (rb_state11.var_26) ^ (((0x5e44ac12) + (0xcb2d050c)) + (rb_state11.var_23));
        rb_state11.var_27 = (rb_state11.var_27) - (((rb_state11.var_24) == (0x0)) ? (rb_state11.var_25) : (0x7351c3cf));
        rb_state11.var_28 = (rb_state11.var_28) + (((((0x22745a4) + (0xb4ee19ee)) + (rb_state11.var_27)) - (0x415bc395)) - (rb_state11.var_26));
        rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x67b24fcc)) {
        pthread_mutex_lock(&(rb_state15.lock_20));
        rb_state15.var_3 = 0xceecf545;
        rb_state15.var_4 = (rb_state15.var_4) - (((rb_state15.var_3) == (0xceecf545)) ? (rb_state15.var_1) : (0x307f0a6d));
        rb_state15.var_5 = (rb_state15.var_5) ^ (0x649a27d0);
        rb_state15.var_6 = (rb_state15.var_6) + (((0x6e0d3050) ^ (rb_state15.var_2)) - (rb_state15.var_3));
        rb_state15.var_7 = (rb_state15.var_7) - (((((0xf354d30c) - (0x35f2d798)) ^ (rb_state15.var_4)) - (rb_state15.var_4)) ^ (rb_state15.var_5));
        rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xf699726e) ^ (rb_state15.var_7)) + (rb_state15.var_5)) + (rb_state15.var_6)) + (rb_state15.var_6));
        rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_8);
        rb_state15.var_1 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_20));
    }
    if ((rb_state15.var_0) == (0x67b24fcc)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
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