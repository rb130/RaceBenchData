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
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x6f5fa0fe)) {
        if ((rb_state17.var_2) != (0x0)) {
            if (!((rb_state17.var_2) == (rb_state17.var_41))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    char *p = (char *)buf;
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_1) : (0x8f38bb01));
    #endif
    size_t nsent = 0;
    ssize_t rv;

    while (nsent < len) {
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xc366d877)) {
            rb_state7.var_3 = 0x1aef99ad;
            rb_state7.var_4 = (rb_state7.var_4) - (0xad8e6ced);
            rb_state7.var_5 = (rb_state7.var_5) ^ ((0x70c25f12) + (rb_state7.var_4));
            rb_state7.var_6 = (rb_state7.var_6) + (rb_state7.var_3);
            rb_state7.var_7 = (rb_state7.var_7) + (((0x65cf581b) + (0x5dd2e03f)) - (rb_state7.var_2));
            rb_state7.var_8 = (rb_state7.var_8) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0x58b75167));
            rb_state7.var_9 = (rb_state7.var_9) ^ (((0x62a24859) + (0x5535d730)) ^ (rb_state7.var_5));
            rb_state7.var_10 = (rb_state7.var_10) - (((((0x769970e7) - (rb_state7.var_6)) + (0xa27080ca)) + (rb_state7.var_6)) + (rb_state7.var_7));
            rb_state7.var_11 = (rb_state7.var_11) - (((((0x4f90743d) - (rb_state7.var_9)) - (rb_state7.var_7)) - (rb_state7.var_8)) + (rb_state7.var_8));
            rb_state7.var_12 = (rb_state7.var_12) ^ (((((0xecfdbcca) - (rb_state7.var_10)) ^ (rb_state7.var_11)) + (rb_state7.var_9)) ^ (0x1df2c318));
            rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_12);
        }
        #endif
        rv = write(sd, p, len - nsent);
        if (0 > rv && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }
        if (0 > rv) {
            return -1;
        }
        nsent += rv;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x775e3c00)) {
            rb_state2.var_10 = 0x845ab969;
            rb_state2.var_11 = 0xb636864a;
            rb_state2.var_12 = (rb_state2.var_12) ^ (((((0xa0c1fd7a) - (rb_state2.var_7)) + (rb_state2.var_11)) + (rb_state2.var_10)) + (rb_state2.var_6));
            rb_state2.var_13 = (rb_state2.var_13) - (((rb_state2.var_8) == (0xd34de411)) ? (rb_state2.var_5) : (0xd1f5d000));
            rb_state2.var_14 = (rb_state2.var_14) ^ (rb_state2.var_12);
            rb_state2.var_15 = (rb_state2.var_15) + (((0x86b931bf) - (rb_state2.var_9)) + (rb_state2.var_13));
            rb_state2.var_16 = (rb_state2.var_16) - (rb_state2.var_14);
            rb_state2.var_17 = (rb_state2.var_17) + (((((0x79bfee0) ^ (rb_state2.var_10)) ^ (0xeede377d)) - (rb_state2.var_16)) - (rb_state2.var_15));
            rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_17);
        }
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