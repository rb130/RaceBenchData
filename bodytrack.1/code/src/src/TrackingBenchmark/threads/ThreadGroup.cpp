#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <list>

#include "Thread.h"
#include "ThreadGroup.h"

namespace threads {

ThreadGroup::ThreadGroup() {
}

ThreadGroup::~ThreadGroup() {
    for (std::list<Thread *>::iterator it = threads.begin(); it != threads.end(); it++) {
        delete (*it);
    }
}

void ThreadGroup::CreateThread(Runnable &tobj) {
    Thread *temp = new Thread(tobj);

    threads.push_front(temp);
}

void ThreadGroup::CreateThreads(int n, Runnable &tobj) {
    int i;
    Thread *temp;

    for (i = 0; i < n; i++) {
        temp = new Thread(tobj);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ ((0x37073256) - (rb_state0.var_0));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (0xf3e8a069);
        #endif
        #ifdef RACEBENCH_BUG_11
        if (!((rb_state11.var_1) == (rb_state11.var_22))) {
            rb_state11.var_48 = 0x710f3b9;
            rb_state11.var_49 = (rb_state11.var_49) + (((0x4165bfd6) + (rb_state11.var_18)) + (0x2435b83d));
            rb_state11.var_50 = (rb_state11.var_50) - (((((0x48be3af0) + (rb_state11.var_17)) ^ (rb_state11.var_48)) ^ (rb_state11.var_19)) - (0xe5c6332a));
            rb_state11.var_51 = (rb_state11.var_51) ^ (((((0x36f3863e) + (0xb9ca8204)) - (0xe3f52d1)) ^ (rb_state11.var_49)) ^ (rb_state11.var_50));
            rb_state11.var_38 = (rb_state11.var_38) - (rb_state11.var_51);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0xa0c5dfcf) - (0x8cd7a57a));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xa4568812));
        if ((rb_state19.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state19.lock_21));
            rb_state19.var_7 = (rb_state19.var_7) + (((0x5597fc1c) - (rb_state19.var_7)) - (rb_state19.var_5));
            rb_state19.var_8 = (rb_state19.var_8) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_8) : (0x5401972a));
            rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_1) : (0x311d2268));
            rb_state19.var_10 = (rb_state19.var_10) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0xc7d85ee1));
            rb_state19.var_11 = (rb_state19.var_11) - (rb_state19.var_9);
            rb_state19.var_12 = (rb_state19.var_12) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0xbf4a7f74));
            rb_state19.var_13 = (rb_state19.var_13) ^ (((0x630a6c4d) ^ (rb_state19.var_10)) + (rb_state19.var_11));
            rb_state19.var_14 = (rb_state19.var_14) ^ (((0xe1b1b6f4) ^ (0xac78ff3a)) - (rb_state19.var_12));
            rb_state19.var_15 = (rb_state19.var_15) - (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_14) : (0x583196f8));
            rb_state19.var_6 = (rb_state19.var_6) ^ (rb_state19.var_15);
            rb_state19.var_1 = rb_state19.var_6;
            pthread_mutex_unlock(&(rb_state19.lock_21));
        }
        if ((rb_state19.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state19.lock_21));
            rb_state19.var_17 = 0xf19d40d2;
            rb_state19.var_18 = 0xc7761bf0;
            rb_state19.var_19 = (rb_state19.var_19) + (((((0xbb343709) ^ (rb_state19.var_17)) - (rb_state19.var_11)) ^ (0x6c23d6ba)) + (rb_state19.var_12));
            rb_state19.var_20 = (rb_state19.var_20) ^ (((((0x43341900) + (rb_state19.var_19)) - (rb_state19.var_13)) + (0x745e31ed)) - (rb_state19.var_18));
            rb_state19.var_16 = (rb_state19.var_16) - (rb_state19.var_20);
            rb_state19.var_1 = rb_state19.var_16;
            pthread_mutex_unlock(&(rb_state19.lock_21));
        }
        #endif
        threads.push_front(temp);
    }
}

void ThreadGroup::AddThread(Thread *t) {
    threads.push_front(t);
}

void ThreadGroup::RemoveThread(Thread *t) {
    threads.remove(t);
}

void ThreadGroup::JoinAll() {
    for (std::list<Thread *>::iterator it = threads.begin(); it != threads.end(); it++) {
        (*it)->Join();
    }
}

int ThreadGroup::Size() const {
    return (int)threads.size();
}

} // namespace threads