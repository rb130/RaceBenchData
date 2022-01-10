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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
    #endif
    char *p = (char *)buf;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
    #endif
    size_t nsent = 0;
    ssize_t rv;

    while (nsent < len) {
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + ((0x8fa66337) - (0x480c5234));
        #endif
        rv = write(sd, p, len - nsent);
        if (0 > rv && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }
        if (0 > rv) {
            return -1;
        }
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + (966358 < rb_input_size ? (uint32_t)rb_input[966358] : 0x8f56efe8);
        if ((rb_state1.var_0) == (0x0)) {
            rb_state1.var_3 = 0xe7d8a18;
            rb_state1.var_4 = 0xf6e05bc4;
            rb_state1.var_5 = (rb_state1.var_5) - (940846 < rb_input_size ? (uint32_t)rb_input[940846] : 0xf33a0ebf);
            rb_state1.var_6 = (rb_state1.var_6) - (((0xc8d060a2) ^ (rb_state1.var_3)) + (rb_state1.var_3));
            rb_state1.var_7 = (rb_state1.var_7) ^ (((0x37498625) ^ (rb_state1.var_2)) + (rb_state1.var_4));
            rb_state1.var_8 = (rb_state1.var_8) - (rb_state1.var_4);
            rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_5) == (0xffffffe0)) ? (rb_state1.var_5) : (0xa0cde7));
            rb_state1.var_10 = (rb_state1.var_10) ^ (rb_state1.var_6);
            rb_state1.var_11 = (rb_state1.var_11) ^ (((0x57b8b86d) ^ (rb_state1.var_7)) + (0x37e2f396));
            rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_6) == (0x2ad48b2e)) ? (rb_state1.var_8) : (0xa9a97ba7));
            rb_state1.var_13 = (rb_state1.var_13) + (((((0x27fa1571) + (rb_state1.var_10)) + (0xd03c39a8)) - (0x86291dd2)) + (rb_state1.var_9));
            rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_7) == (0x23777b07)) ? (rb_state1.var_11) : (0xf2938c2));
            rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_8) == (0x91fa43c)) ? (rb_state1.var_12) : (0x46b4e0bb));
            rb_state1.var_16 = (rb_state1.var_16) ^ (((0x7110f0d5) ^ (rb_state1.var_9)) ^ (rb_state1.var_13));
            rb_state1.var_17 = (rb_state1.var_17) - (((0xab68f3fc) + (rb_state1.var_14)) + (rb_state1.var_10));
            rb_state1.var_18 = (rb_state1.var_18) + (((rb_state1.var_15) == (0xf6e05bc4)) ? (rb_state1.var_16) : (0xa0e5e072));
            rb_state1.var_19 = (rb_state1.var_19) - (rb_state1.var_17);
            rb_state1.var_20 = (rb_state1.var_20) + (((rb_state1.var_11) == (0xacb2b700)) ? (rb_state1.var_18) : (0xda64fda1));
            rb_state1.var_21 = (rb_state1.var_21) ^ (((rb_state1.var_19) == (0x82f0362a)) ? (rb_state1.var_20) : (0x1e05c8a0));
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x40091f9b)) {
            pthread_mutex_lock(&(rb_state5.lock_12));
            rb_state5.var_8 = 0x87e3b4af;
            rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_4);
            rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_8) : (0x2b3c9228));
            rb_state5.var_11 = (rb_state5.var_11) + (((((0x1797a433) ^ (rb_state5.var_9)) + (rb_state5.var_10)) ^ (rb_state5.var_5)) ^ (rb_state5.var_6));
            rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_11);
            pthread_mutex_unlock(&(rb_state5.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_2) == (0x905d1902)) {
            pthread_mutex_lock(&(rb_state9.lock_16));
            rb_state9.var_7 = 0x8149595d;
            rb_state9.var_8 = (rb_state9.var_8) + (((0x20ca61cd) + (rb_state9.var_4)) - (rb_state9.var_7));
            rb_state9.var_9 = (rb_state9.var_9) + (((0xec01248e) + (rb_state9.var_8)) + (rb_state9.var_7));
            rb_state9.var_10 = (rb_state9.var_10) ^ ((0xb1fcf717) - (rb_state9.var_9));
            rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_6) == (0x30640a6)) ? (rb_state9.var_8) : (0xa9ca3c34));
            rb_state9.var_12 = (rb_state9.var_12) ^ (((0x7988a771) - (rb_state9.var_9)) ^ (rb_state9.var_10));
            rb_state9.var_13 = (rb_state9.var_13) + (((rb_state9.var_10) == (0xa837b162)) ? (rb_state9.var_11) : (0x7254b509));
            rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_12);
            rb_state9.var_15 = (rb_state9.var_15) ^ (((rb_state9.var_13) == (0x63853836)) ? (rb_state9.var_14) : (0x3f3ab041));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_15);
            pthread_mutex_unlock(&(rb_state9.lock_16));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + ((0x6046426a) ^ (0xf7e58c01));
        #endif
        nsent += rv;
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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (384619 < rb_input_size ? (uint32_t)rb_input[384619] : 0xaff7c125);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x9e898ce8)) {
        pthread_mutex_lock(&(rb_state19.lock_13));
        rb_state19.var_4 = 0x3a942f4;
        rb_state19.var_5 = 0x2ee7cd67;
        rb_state19.var_6 = (rb_state19.var_6) ^ (((((0x9fee348) + (rb_state19.var_3)) - (0x87114279)) ^ (0xce9f9858)) - (rb_state19.var_5));
        rb_state19.var_7 = (rb_state19.var_7) ^ (0x5fb2f113);
        rb_state19.var_8 = (rb_state19.var_8) + (rb_state19.var_4);
        rb_state19.var_9 = (rb_state19.var_9) ^ (((((0x7c0c3aa4) - (rb_state19.var_4)) ^ (rb_state19.var_7)) - (0x25be20d)) ^ (rb_state19.var_6));
        rb_state19.var_10 = (rb_state19.var_10) + (((rb_state19.var_5) == (0x2ee7cd67)) ? (rb_state19.var_8) : (0xb429c26a));
        rb_state19.var_11 = (rb_state19.var_11) ^ (rb_state19.var_9);
        rb_state19.var_12 = (rb_state19.var_12) + (((rb_state19.var_10) == (0x3a942f4)) ? (rb_state19.var_11) : (0x8bb5ac4b));
        rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_12);
        pthread_mutex_unlock(&(rb_state19.lock_13));
    }
    #endif
    int checkbit = CHECKBIT;
    if (xwrite(fd, &checkbit, sizeof(int)) < 0) {
        return -1;
    }

    if (xwrite(fd, &compress_type, sizeof(byte)) < 0) {
        return -1;
    }
    return 0;
}