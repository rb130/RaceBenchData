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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (413514 < rb_input_size ? (uint32_t)rb_input[413514] : 0x769a9474);
    #endif
    char *p = (char *)buf;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_22 = (rb_state11.var_22) ^ ((0x9a84bd06) ^ (rb_state11.var_23));
    #endif
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
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) ^ (0xe681251f);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) - (234951 < rb_input_size ? (uint32_t)rb_input[234951] : 0x9c5b2d15);
        #endif
        nsent += rv;
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_1);
        #endif
        p += rv;
    }
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