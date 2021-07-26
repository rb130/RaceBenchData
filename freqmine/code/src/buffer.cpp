#ifdef __cplusplus
extern "C"
#endif
void racebench_bug23(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug23(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug53(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug53(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug78(int);

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "common.h"
#include <malloc.h>

memory::memory() {
    BUFPOS = 20;

    BUFS_BIG = 1024576L;
    BUFS_SMALL = 4096L;
    BUFSBSWITCH = 2;
    init();
}

memory::memory(int bufpos, long bufs_small, long bufs_big, int bufsbswitch) {
    BUFPOS = bufpos;
    BUFS_BIG = bufs_big;
    BUFS_SMALL = bufs_small;
    BUFSBSWITCH = bufsbswitch;
    init();
}

void memory::init() {
    buffer = new char *[BUFPOS];
    start = new char *[BUFPOS];
    rest = new unsigned int[BUFPOS];
    restsize = new unsigned int[BUFPOS];

    buffer[0] = new char[BUFS_SMALL];

    if (buffer[0] == NULL) {
#ifdef DEBUG
        perror("initbuf 2");
#endif
        printf("INIT: not enough memory to run this program\n");
        exit(0);
    }
    start[0] = buffer[0];
    markbuf = buffer[0];
    markcount = 0;

    markrest = BUFS_SMALL;
    rest[0] = BUFS_SMALL;
    restsize[0] = BUFS_SMALL;
    bufcount = 0;
}

memory::~memory() {
    int i;
    for (i = 0; i <= bufcount; i++) {
        delete[] buffer[i];
    }

    delete[] buffer;
    delete[] start;
    delete[] rest;
    delete[] restsize;
}

char *memory::newbuf(unsigned int num, unsigned int size)

{
    register unsigned int i;
    register int pos;
    char *hlp;

    size += (i = (size & L2BITS)) ? MULTOF - i : 0; racebench_bug23(0); racebench_bug53(0); racebench_bug67(1); racebench_bug78(0);
    i = num * size; racebench_bug23(1);
    for (pos = markcount; pos < bufcount; pos++) {
        if (rest[pos] >= i) {
            break;
        }
    }

    if (rest[pos] < i) {
        pos = switchbuf(i);
    }
    hlp = start[pos];
    start[pos] += i; racebench_bug53(1);
    rest[pos] -= i;

    return hlp;
}

int memory::switchbuf(unsigned int i) {
    bufcount++;
    if (bufcount == BUFPOS) {
        buffree();
        bufcount--;
        printf("The blocks are used up.\n");
        delete this;
        exit(0);
    }
    if (bufcount < BUFSBSWITCH) {
        rest[bufcount] = BUFS_SMALL;
    } else {
        int j = BUFPOS / (BUFPOS - bufcount);
        if (j < 12) {
            rest[bufcount] = power2[j - 1] * BUFS_BIG;
        } else {
            rest[bufcount] = power2[11] * BUFS_BIG;
        }
    }

    if (rest[bufcount] < i) {
        rest[bufcount] = i;
    }
    restsize[bufcount] = rest[bufcount];

    buffer[bufcount] = new char[rest[bufcount]];
    if (buffer[bufcount] == NULL) {
#ifdef DEBUG
        perror("switchbuf 1");
#endif
        buffree();
        printf("Not enough memory!\n");
        delete this;
    }
    start[bufcount] = buffer[bufcount];
    return bufcount;
}

char *memory::bufmark(unsigned int *MR, int *MC) {
    markbuf = start[bufcount];
    markcount = bufcount;

    markrest = rest[bufcount];
    *MR = markrest;
    *MC = markcount;
    return markbuf;
}

void memory::freebuf(unsigned int MR, int MC, char *MB) {
    int i;

    int freesize = 0;
    for (i = MC + 1; i <= bufcount; i++) {

        delete[] buffer[i];
        freesize += start[i] - buffer[i];
        rest[i] = 0;
        restsize[i] = 0;
        start[i] = NULL;
        buffer[i] = NULL;
    }
    bufcount = MC;

    freesize += MR - rest[bufcount];
    start[bufcount] = MB;
    rest[bufcount] = MR;
}

void memory::buffree() {
    markbuf = buffer[0];
    markcount = 0;
    markrest = BUFS_SMALL;
}