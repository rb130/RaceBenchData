#include "racebench_bugs.h"


#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "util.h"

int xread(int sd, void *buf, size_t len) {
    char *p = (char *)buf;
    size_t nrecv = 0;
    size_t rv;

    while (nrecv < len) {
        rv = read(sd, p, len - nrecv);
        if (0 > rv && errno == EINTR) {
            continue;
        }
        if (0 > rv) {
            return -1;
        }
        if (0 == rv) {
            return 0;
        }
        nrecv += rv;
        p += rv;
    }
    return nrecv;
}

int xwrite(int sd, const void *buf, size_t len) {
    char *p = (char *)buf;
    size_t nsent = 0;
    ssize_t rv;

    while (nsent < len) {
        rv = write(sd, p, len - nsent);
        if (0 > rv && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }
        if (0 > rv) {
            return -1;
        }
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0x45c4d582)) {
            rb_state14.var_2 = rb_state14.var_1;
        }
        if ((rb_state14.var_0) == (0x45c4d582)) {
            if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
        }
        #endif
        nsent += rv;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - ((0x41e20f5) + (0xcb71b6de));
        #endif
        p += rv;
    }
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_14) == (0x4260cb6f)) {
        pthread_mutex_lock(&(rb_state13.lock_90));
        if ((rb_state13.var_29) != (0x0)) {
            if (!((rb_state13.var_29) == (rb_state13.var_61))) {
                racebench_trigger(13);
            }
        }
        pthread_mutex_unlock(&(rb_state13.lock_90));
    }
    #endif
    return nsent;
}

int read_header(int fd, byte *compress_type) {
    int checkbit;

    assert(compress_type != NULL);

    if (xread(fd, &checkbit, sizeof(int)) < 0) {
        return -1;
    }
    if (checkbit != CHECKBIT) {
        printf("format error!\n");
        return -1;
    }

    if (xread(fd, compress_type, sizeof(byte)) < 0) {
        return -1;
    }

    return 0;
}

int write_header(int fd, byte compress_type) {
    int checkbit = CHECKBIT;
    if (xwrite(fd, &checkbit, sizeof(int)) < 0) {
        return -1;
    }

    if (xwrite(fd, &compress_type, sizeof(byte)) < 0) {
        return -1;
    }
    return 0;
}