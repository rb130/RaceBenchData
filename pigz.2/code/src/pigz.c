#include "racebench_bugs.h"


#define VERSION "pigz 2.6"

#define _FILE_OFFSET_BITS 64
#define _LARGE_FILES
#define _XOPEN_SOURCE 700

#include <stdio.h>

#include <stdlib.h>

#include <stdarg.h>
#include <string.h>

#include <errno.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/time.h>
#include <unistd.h>

#include <fcntl.h>

#include <dirent.h>

#include <limits.h>
#if __STDC_VERSION__ - 0 >= 199901L || __GNUC__ - 0 >= 3
#include <inttypes.h>
typedef uintmax_t length_t;
typedef uint32_t crc_t;
#else
typedef unsigned long length_t;
typedef unsigned long crc_t;
#endif

#ifdef PIGZ_DEBUG
#if defined(__APPLE__)
#include <malloc/malloc.h>
#define MALLOC_SIZE(p) malloc_size(p)
#elif defined(__linux)
#include <malloc.h>
#define MALLOC_SIZE(p) malloc_usable_size(p)
#elif defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
#define MALLOC_SIZE(p) _msize(p)
#else
#define MALLOC_SIZE(p) (0)
#endif
#endif

#ifdef __hpux
#include <sys/param.h>
#include <sys/pstat.h>
#endif

#ifndef S_IFLNK
#define S_IFLNK 0
#endif

#ifdef __MINGW32__
#define chown(p, o, g) 0
#define utimes(p, t) 0
#define lstat(p, s) stat(p, s)
#define _exit(s) exit(s)
#endif

#include "zlib.h"

#if !defined(ZLIB_VERNUM) || ZLIB_VERNUM < 0x1230
#error "Need zlib version 1.2.3 or later"
#endif

#ifndef NOTHREAD
#include "yarn.h"

#endif

#ifndef NOZOPFLI
#include "zopfli/src/zopfli/deflate.h"

#endif

#include "try.h"

#define local static

#if defined(MSDOS) || defined(OS2) || defined(_WIN32) || defined(__CYGWIN__)
#include <io.h>
#define SET_BINARY_MODE(fd) setmode(fd, O_BINARY)
#else
#define SET_BINARY_MODE(fd)
#endif

#define RELEASE(ptr)         \
    do {                     \
        if ((ptr) != NULL) { \
            FREE(ptr);       \
            ptr = NULL;      \
        }                    \
    } while (0)

#define DICT 32768U

#define MAXP2 (UINT_MAX - (UINT_MAX >> 1))

#define RSYNCBITS 12
#define RSYNCMASK ((1U << RSYNCBITS) - 1)
#define RSYNCHIT (RSYNCMASK >> 1)

#define INBUFS(p) (((p) << 1) + 3)
#define OUTPOOL(s) ((s) + ((s) >> 4) + DICT)

#define BUF 32768
#define CEN 42
#define EXT (BUF + CEN)

local struct {
    int volatile ret;
    char *prog;
    int ind;
    int outd;
    char *inf;
    size_t inz;
    char *outf;
    int verbosity;
    int headis;
    int pipeout;
    int keep;
    int force;
    int sync;
    int form;
    int magic1;
    int recurse;
    char *sufx;
    char *name;
    char *alias;
    char *comment;
    time_t mtime;
    int list;
    int first;
    int decode;
    int level;
    int strategy;
#ifndef NOZOPFLI
    ZopfliOptions zopts;
#endif
    int rsync;
    int procs;
    int setdict;
    size_t block;
#ifndef NOTHREAD
    crc_t shift;
#endif

    time_t stamp;
    char *hname;
    char *hcomm;
    unsigned long zip_crc;
    length_t zip_clen;
    length_t zip_ulen;
    int zip64;

    unsigned char in_buf[EXT];
    unsigned char *in_next;
    size_t in_left;
    int in_eof;
    int in_short;
    length_t in_tot;
    length_t out_tot;
    unsigned long out_check;

#ifndef NOTHREAD

    unsigned char in_buf2[EXT];
    size_t in_len;
    int in_which;
    lock *load_state;
    thread *load_thread;
#endif
} g;

local int complain(char *fmt, ...) {
    va_list ap;

    if (g.verbosity > 0) {
        fprintf(stderr, "%s: ", g.prog);
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        putc('\n', stderr);
        fflush(stderr);
    }
    g.ret = 1;
    return 0;
}

#ifdef PIGZ_DEBUG

#define MAXMEM 131072

local struct mem_track_s {
    size_t num;
    size_t size;
    size_t tot;
    size_t max;
#ifndef NOTHREAD
    lock *lock;
#endif
    size_t have;
    void *mem[MAXMEM];
} mem_track;

#ifndef NOTHREAD
#define mem_track_grab(m) possess((m)->lock)
#define mem_track_drop(m) release((m)->lock)
#else
#define mem_track_grab(m)
#define mem_track_drop(m)
#endif

local size_t search_track(struct mem_track_s *mem, void *ptr) {
    ptrdiff_t left = 0;
    ptrdiff_t right = mem->have - 1;
    while (left <= right) {
        ptrdiff_t mid = (left + right) >> 1;
        if (mem->mem[mid] < ptr)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return left;
}

local void insert_track(struct mem_track_s *mem, void *ptr) {
    mem_track_grab(mem);
    assert(mem->have < MAXMEM && "increase MAXMEM in source and try again");
    size_t i = search_track(mem, ptr);
    if (i < mem->have && mem->mem[i] == ptr)
        complain("mem_track: duplicate pointer %p\n", ptr);
    memmove(&mem->mem[i + 1], &mem->mem[i], (mem->have - i) * sizeof(void *));
    mem->mem[i] = ptr;
    mem->have++;
    mem->num++;
    mem->size += MALLOC_SIZE(ptr);
    if (mem->num > mem->tot)
        mem->tot = mem->num;
    if (mem->size > mem->max)
        mem->max = mem->size;
    mem_track_drop(mem);
}

local void delete_track(struct mem_track_s *mem, void *ptr) {
    mem_track_grab(mem);
    size_t i = search_track(mem, ptr);
    if (i < mem->num && mem->mem[i] == ptr) {
        memmove(&mem->mem[i], &mem->mem[i + 1], (mem->have - (i + 1)) * sizeof(void *));
        mem->have--;
    } else
        complain("mem_track: missing pointer %p\n", ptr);
    mem->num--;
    mem->size -= MALLOC_SIZE(ptr);
    mem_track_drop(mem);
}

local void *malloc_track(struct mem_track_s *mem, size_t size) {
    void *ptr = malloc(size);
    if (ptr != NULL)
        insert_track(mem, ptr);
    return ptr;
}

local void *realloc_track(struct mem_track_s *mem, void *ptr, size_t size) {
    if (ptr == NULL)
        return malloc_track(mem, size);
    delete_track(mem, ptr);
    void *got = realloc(ptr, size);
    insert_track(mem, got == NULL ? ptr : got);
    return got;
}

local void free_track(struct mem_track_s *mem, void *ptr) {
    if (ptr != NULL) {
        delete_track(mem, ptr);
        free(ptr);
    }
}

#ifndef NOTHREAD
local void *yarn_malloc(size_t size) {
    return malloc_track(&mem_track, size);
}

local void yarn_free(void *ptr) {
    free_track(&mem_track, ptr);
}
#endif

local voidpf zlib_alloc(voidpf opaque, uInt items, uInt size) {
    return malloc_track(opaque, items * (size_t)size);
}

local void zlib_free(voidpf opaque, voidpf address) {
    free_track(opaque, address);
}

#define REALLOC(p, s) realloc_track(&mem_track, p, s)
#define FREE(p) free_track(&mem_track, p)
#define OPAQUE (&mem_track)
#define ZALLOC zlib_alloc
#define ZFREE zlib_free

#else

#define REALLOC realloc
#define FREE free
#define OPAQUE Z_NULL
#define ZALLOC Z_NULL
#define ZFREE Z_NULL

#endif

local void *alloc(void *ptr, size_t size) {
    ptr = REALLOC(ptr, size);
    if (ptr == NULL) {
        throw(ENOMEM, "not enough memory");
    }
    return ptr;
}

#ifdef PIGZ_DEBUG

local struct timeval start;

local struct log {
    struct timeval when;
    char *msg;
    struct log *next;
} * log_head, **log_tail = NULL;
#ifndef NOTHREAD
local lock *log_lock = NULL;
#endif

#define MAXMSG 256

local void log_init(void) {
    if (log_tail == NULL) {
        mem_track.num = 0;
        mem_track.size = 0;
        mem_track.num = 0;
        mem_track.max = 0;
        mem_track.have = 0;
#ifndef NOTHREAD
        mem_track.lock = new_lock(0);
        yarn_mem(yarn_malloc, yarn_free);
        log_lock = new_lock(0);
#endif
        log_head = NULL;
        log_tail = &log_head;
    }
}

local void log_add(char *fmt, ...) {
    struct timeval now;
    struct log *me;
    va_list ap;
    char msg[MAXMSG];

    gettimeofday(&now, NULL);
    me = alloc(NULL, sizeof(struct log));
    me->when = now;
    va_start(ap, fmt);
    vsnprintf(msg, MAXMSG, fmt, ap);
    va_end(ap);
    me->msg = alloc(NULL, strlen(msg) + 1);
    strcpy(me->msg, msg);
    me->next = NULL;
#ifndef NOTHREAD
    assert(log_lock != NULL);
    possess(log_lock);
#endif
    *log_tail = me;
    log_tail = &(me->next);
#ifndef NOTHREAD
    twist(log_lock, BY, +1);
#endif
}

local int log_show(void) {
    struct log *me;
    struct timeval diff;

    if (log_tail == NULL)
        return 0;
#ifndef NOTHREAD
    possess(log_lock);
#endif
    me = log_head;
    if (me == NULL) {
#ifndef NOTHREAD
        release(log_lock);
#endif
        return 0;
    }
    log_head = me->next;
    if (me->next == NULL)
        log_tail = &log_head;
#ifndef NOTHREAD
    twist(log_lock, BY, -1);
#endif
    diff.tv_usec = me->when.tv_usec - start.tv_usec;
    diff.tv_sec = me->when.tv_sec - start.tv_sec;
    if (diff.tv_usec < 0) {
        diff.tv_usec += 1000000L;
        diff.tv_sec--;
    }
    fprintf(stderr, "trace %ld.%06ld %s\n", (long)diff.tv_sec, (long)diff.tv_usec, me->msg);
    fflush(stderr);
    FREE(me->msg);
    FREE(me);
    return 1;
}

local void log_free(void) {
    struct log *me;

    if (log_tail != NULL) {
#ifndef NOTHREAD
        possess(log_lock);
#endif
        while ((me = log_head) != NULL) {
            log_head = me->next;
            FREE(me->msg);
            FREE(me);
        }
#ifndef NOTHREAD
        twist(log_lock, TO, 0);
        free_lock(log_lock);
        log_lock = NULL;
        yarn_mem(malloc, free);
        free_lock(mem_track.lock);
#endif
        log_tail = NULL;
    }
}

local void log_dump(void) {
    if (log_tail == NULL)
        return;
    while (log_show())
        ;
    log_free();
    if (mem_track.num || mem_track.size)
        complain("memory leak: %zu allocs of %zu bytes total", mem_track.num, mem_track.size);
    if (mem_track.max)
        fprintf(stderr, "%zu bytes of memory used in %zu allocs\n", mem_track.max, mem_track.tot);
}

#define Trace(x)               \
    do {                       \
        if (g.verbosity > 2) { \
            log_add x;         \
        }                      \
    } while (0)

#else

#define log_dump()
#define Trace(x)

#endif

local void cut_short(int sig) {
    if (sig == SIGINT) {
        Trace(("termination by user"));
    }
    if (g.outd != -1 && g.outd != 1) {
        unlink(g.outf);
        RELEASE(g.outf);
        g.outd = -1;
    }
    log_dump();
    _exit(sig < 0 ? -sig : EINTR);
}

#define THREADABORT(ball)                      \
    do {                                       \
        if ((ball).code != EPIPE)              \
            complain("abort: %s", (ball).why); \
        drop(ball);                            \
        cut_short(-(ball).code);               \
    } while (0)

local inline size_t grow(size_t size) {
    size_t was, top;
    int shift;

    was = size;
    size += size >> 2;
    top = size;
    for (shift = 0; top > 7; shift++) {
        top >>= 1;
    }
    if (top == 7) {
        size = (size_t)1 << (shift + 3);
    }
    if (size < 16) {
        size = 16;
    }
    if (size <= was) {
        size = (size_t)0 - 1;
    }
    return size;
}

local inline size_t vmemcpy(char **mem, size_t *size, size_t off, void *cpy, size_t len) {
    size_t need;
    const size_t min_len = 1024;

    need = off + len;
    if (need < min_len)
        need = min_len;
    if (need < off) {
        throw(ERANGE, "overflow");
    }
    if (need > *size) {
        need = grow(need);
        if (off == 0) {
            RELEASE(*mem);
            *size = 0;
        }
        *mem = alloc(*mem, need);
        *size = need;
    }
    memcpy(*mem + off, cpy, len);
    return off + len;
}

local inline size_t vstrcpy(char **str, size_t *size, size_t off, void *cpy) {
    return vmemcpy(str, size, off, cpy, strlen(cpy) + 1);
}

local size_t readn(int desc, unsigned char *buf, size_t len) {
    ssize_t ret;
    size_t got;

    got = 0;
    while (len) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0xe4bda29d);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x6db0dac9));
        #endif
        ret = read(desc, buf, len);
        if (ret < 0) {
            throw(errno, "read error on %s (%s)", g.inf, strerror(errno));
        }
        if (ret == 0) {
            break;
        }
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) + ((0x41992c14) - (0x8c53dc80));
        #endif
        buf += ret;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (0x58fe9c33);
        #endif
        len -= (size_t)ret;
        got += (size_t)ret;
    }
    return got;
}

local size_t writen(int desc, void const *buf, size_t len) {
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (326 < rb_input_size ? (uint32_t)rb_input[326] : 0x63a934a1);
    #endif
    char const *next = buf;
    size_t left = len;

    while (left) {
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x9c7eb74a)) {
            rb_state15.var_6 = 0x1cb0024c;
            rb_state15.var_7 = 0x8e861464;
            rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_6) == (0x1cb0024c)) ? (rb_state15.var_5) : (0x772e7c39));
            rb_state15.var_9 = (rb_state15.var_9) ^ (rb_state15.var_7);
            rb_state15.var_10 = (rb_state15.var_10) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_7) : (0x7891c847));
            rb_state15.var_11 = (rb_state15.var_11) - (((((0xb4807966) - (0x68892990)) + (rb_state15.var_9)) + (rb_state15.var_4)) - (rb_state15.var_6));
            rb_state15.var_12 = (rb_state15.var_12) - (((0x5cf80a40) + (rb_state15.var_8)) - (rb_state15.var_10));
            rb_state15.var_13 = (rb_state15.var_13) - (rb_state15.var_9);
            rb_state15.var_14 = (rb_state15.var_14) ^ (((0xc987aa9c) + (rb_state15.var_10)) + (0x797bf745));
            rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_11) == (0x42329e12)) ? (rb_state15.var_11) : (0x3aed0e2f));
            rb_state15.var_16 = (rb_state15.var_16) + (((0xa92cb6cc) ^ (rb_state15.var_12)) ^ (rb_state15.var_12));
            rb_state15.var_17 = (rb_state15.var_17) ^ (((rb_state15.var_13) == (0x7179eb9c)) ? (rb_state15.var_14) : (0x199eca9c));
            rb_state15.var_18 = (rb_state15.var_18) ^ (rb_state15.var_15);
            rb_state15.var_19 = (rb_state15.var_19) ^ (((((0x27c3e8de) ^ (rb_state15.var_13)) + (rb_state15.var_16)) ^ (rb_state15.var_17)) - (0x4317d3ac));
            rb_state15.var_20 = (rb_state15.var_20) + (((rb_state15.var_18) == (0xbdcd61ee)) ? (rb_state15.var_19) : (0x91d75f0d));
            rb_state15.var_5 = (rb_state15.var_5) + (rb_state15.var_20);
            rb_state15.var_2 = rb_state15.var_5;
        }
        if ((rb_state15.var_0) == (0x9c7eb74a)) {
            pthread_mutex_lock(&(rb_state15.lock_27));
            rb_state15.var_22 = 0x1f3c0e69;
            rb_state15.var_23 = 0x7066215a;
            rb_state15.var_24 = (rb_state15.var_24) ^ (((((0x156a0d56) ^ (0xdd864fba)) + (rb_state15.var_15)) + (rb_state15.var_23)) - (rb_state15.var_14));
            rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_22);
            rb_state15.var_26 = (rb_state15.var_26) + (((((0xf9dc4894) ^ (0x335b7141)) ^ (rb_state15.var_24)) + (rb_state15.var_25)) + (0x54f38b0e));
            rb_state15.var_21 = (rb_state15.var_21) - (rb_state15.var_26);
            rb_state15.var_2 = rb_state15.var_21;
            pthread_mutex_unlock(&(rb_state15.lock_27));
        }
        #endif
        size_t const max = SSIZE_MAX;
        ssize_t ret = write(desc, next, left > max ? max : left);
        if (ret < 1) {
            throw(errno, "write error on %s (%s)", g.outf, strerror(errno));
        }
        next += ret;
        left -= (size_t)ret;
    }
    return len;
}

local unsigned long time2dos(time_t t) {
    struct tm *tm;
    unsigned long dos;

    if (t == 0) {
        t = time(NULL);
    }
    tm = localtime(&t);
    if (tm->tm_year < 80 || tm->tm_year > 207) {
        return 0;
    }
    dos = (unsigned long)(tm->tm_year - 80) << 25;
    dos += (unsigned long)(tm->tm_mon + 1) << 21;
    dos += (unsigned long)tm->tm_mday << 16;
    dos += (unsigned long)tm->tm_hour << 11;
    dos += (unsigned long)tm->tm_min << 5;
    dos += (unsigned long)(tm->tm_sec + 1) >> 1;
    return dos;
}

typedef length_t val_t;

local unsigned put(int out, ...) {

    unsigned count = 0;
    int n;
    va_list ap;
    va_start(ap, out);
    while ((n = va_arg(ap, int)) != 0) {
        va_arg(ap, val_t);
        count += (unsigned)abs(n);
    }
    va_end(ap);

    unsigned char *wrap = alloc(NULL, count);
    unsigned char *next = wrap;

    va_start(ap, out);
    while ((n = va_arg(ap, int)) != 0) {
        val_t val = va_arg(ap, val_t);
        if (n < 0) {
            n = -n << 3;
            do {
                n -= 8;
                *next++ = (unsigned char)(val >> n);
            } while (n);
        } else {
            do {
                *next++ = (unsigned char)val;
                val >>= 8;
            } while (--n);
        }
    }
    va_end(ap);

    writen(out, wrap, count);
    FREE(wrap);
    return count;
}

#define LOW32 0xffffffff

local length_t put_header(void) {
    length_t len;

    if (g.form > 1) {

        len = put(g.outd, 4, (val_t)0x04034b50, 2, (val_t)45, 2, (val_t)8, 2, (val_t)8, 4, (val_t)time2dos(g.mtime), 4, (val_t)0, 4, (val_t)LOW32, 4, (val_t)LOW32, 2, (val_t)(strlen(g.name == NULL ? g.alias : g.name)), 2, (val_t)29, 0);

        len += writen(g.outd, g.name == NULL ? g.alias : g.name, strlen(g.name == NULL ? g.alias : g.name));

        len += put(g.outd, 2, (val_t)0x0001, 2, (val_t)16, 8, (val_t)0, 8, (val_t)0, 2, (val_t)0x5455, 2, (val_t)5, 1, (val_t)1, 4, (val_t)g.mtime, 0);
    } else if (g.form) {
        if (g.comment != NULL) {
            complain("can't store comment in zlib format -- ignoring");
        }
        unsigned head;
        head = (0x78 << 8) + (g.level >= 9 ? 3 << 6 : g.level == 1 ? 0 << 6 : g.level >= 6 || g.level == Z_DEFAULT_COMPRESSION ? 1 << 6 : 2 << 6);
        head += 31 - (head % 31);
        len = put(g.outd, -2, (val_t)head, 0);
    } else {
        len = put(g.outd, 1, (val_t)31, 1, (val_t)139, 1, (val_t)8, 1, (val_t)((g.name != NULL ? 8 : 0) + (g.comment != NULL ? 16 : 0)), 4, (val_t)g.mtime, 1, (val_t)(g.level >= 9 ? 2 : g.level == 1 ? 4 : 0), 1, (val_t)3, 0);
        if (g.name != NULL) {
            len += writen(g.outd, g.name, strlen(g.name) + 1);
        }
        if (g.comment != NULL) {
            len += writen(g.outd, g.comment, strlen(g.comment) + 1);
        }
    }
    return len;
}

local void put_trailer(length_t ulen, length_t clen, unsigned long check, length_t head) {
    if (g.form > 1) {

        length_t desc = put(g.outd, 4, (val_t)0x08074b50, 4, (val_t)check, 8, (val_t)clen, 8, (val_t)ulen, 0);

        int zip64 = ulen >= LOW32 || clen >= LOW32;

        length_t cent = put(g.outd, 4, (val_t)0x02014b50, 1, (val_t)45, 1, (val_t)255, 2, (val_t)45, 2, (val_t)8, 2, (val_t)8, 4, (val_t)time2dos(g.mtime), 4, (val_t)check, 4, (val_t)(zip64 ? LOW32 : clen), 4, (val_t)(zip64 ? LOW32 : ulen), 2, (val_t)(strlen(g.name == NULL ? g.alias : g.name)), 2, (val_t)(zip64 ? 29 : 9), 2, (val_t)(g.comment == NULL ? 0 : strlen(g.comment)), 2, (val_t)0, 2, (val_t)0, 4, (val_t)0, 4, (val_t)0, 0);

        cent += writen(g.outd, g.name == NULL ? g.alias : g.name, strlen(g.name == NULL ? g.alias : g.name));

        if (zip64) {
            cent += put(g.outd, 2, (val_t)0x0001, 2, (val_t)16, 8, (val_t)ulen, 8, (val_t)clen, 0);
        }

        cent += put(g.outd, 2, (val_t)0x5455, 2, (val_t)5, 1, (val_t)1, 4, (val_t)g.mtime, 0);

        if (g.comment != NULL) {
            cent += writen(g.outd, g.comment, strlen(g.comment));
        }

        zip64 = head + clen + desc >= LOW32;
        if (zip64) {

            put(g.outd, 4, (val_t)0x06064b50, 8, (val_t)44, 2, (val_t)45, 2, (val_t)45, 4, (val_t)0, 4, (val_t)0, 8, (val_t)1, 8, (val_t)1, 8, (val_t)cent, 8, (val_t)(head + clen + desc), 4, (val_t)0x07064b50, 4, (val_t)0, 8, (val_t)(head + clen + desc + cent), 4, (val_t)1, 0);
        }

        put(g.outd, 4, (val_t)0x06054b50, 2, (val_t)0, 2, (val_t)0, 2, (val_t)(zip64 ? 0xffff : 1), 2, (val_t)(zip64 ? 0xffff : 1), 4, (val_t)(zip64 ? LOW32 : cent), 4, (val_t)(zip64 ? LOW32 : head + clen + desc), 2, (val_t)0, 0);
    } else if (g.form) {
        put(g.outd, -4, (val_t)check, 0);
    } else {
        put(g.outd, 4, (val_t)check, 4, (val_t)ulen, 0);
    }
}

local unsigned long adler32z(unsigned long adler, unsigned char const *buf, size_t len) {
    while (len > UINT_MAX && buf != NULL) {
        adler = adler32(adler, buf, UINT_MAX);
        buf += UINT_MAX;
        len -= UINT_MAX;
    }
    return adler32(adler, buf, (unsigned)len);
}

local unsigned long crc32z(unsigned long crc, unsigned char const *buf, size_t len) {
    while (len > UINT_MAX && buf != NULL) {
        crc = crc32(crc, buf, UINT_MAX);
        buf += UINT_MAX;
        len -= UINT_MAX;
    }
    return crc32(crc, buf, (unsigned)len);
}

#define CHECK(a, b, c) (g.form == 1 ? adler32z(a, b, c) : crc32z(a, b, c))

local long zlib_vernum(void) {
    char const *ver = zlibVersion();
    long num = 0;
    int left = 4;
    int comp = 0;
    do {
        if (*ver >= '0' && *ver <= '9') {
            comp = 10 * comp + *ver - '0';
        } else {
            num = (num << 4) + (comp > 0xf ? 0xf : comp);
            left--;
            if (*ver != '.') {
                break;
            }
            comp = 0;
        }
        ver++;
    } while (left);
    return left < 2 ? num << (left << 2) : -1;
}

#ifndef NOTHREAD

#define COMB(a, b, c) (g.form == 1 ? adler32_comb(a, b, c) : crc32_comb(a, b, c))

#define POLY 0xedb88320

local crc_t multmodp(crc_t a, crc_t b) {
    crc_t m = (crc_t)1 << 31;
    crc_t p = 0;
    for (;;) {
        if (a & m) {
            p ^= b;
            if ((a & (m - 1)) == 0) {
                break;
            }
        }
        m >>= 1;
        b = b & 1 ? (b >> 1) ^ POLY : b >> 1;
    }
    return p;
}

local const crc_t x2n_table[] = {0x40000000, 0x20000000, 0x08000000, 0x00800000, 0x00008000, 0xedb88320, 0xb1e6b092, 0xa06a2517, 0xed627dae, 0x88d14467, 0xd7bbfe6a, 0xec447f11, 0x8e7ea170, 0x6427800e, 0x4d47bae0, 0x09fe548f, 0x83852d0f, 0x30362f1a, 0x7b5a9cc3, 0x31fec169, 0x9fec022a, 0x6c8dedc4, 0x15d6874d, 0x5fde7a4e, 0xbad90e37, 0x2e4e5eef, 0x4eaba214, 0xa8a472c0, 0x429a969e, 0x148d302a, 0xc40ba6d0, 0xc4e22c3c};

local crc_t x2nmodp(size_t n, unsigned k) {
    crc_t p = (crc_t)1 << 31;
    while (n) {
        if (n & 1) {
            p = multmodp(x2n_table[k & 31], p);
        }
        n >>= 1;
        k++;
    }
    return p;
}

local unsigned long crc32_comb(unsigned long crc1, unsigned long crc2, size_t len2) {
    return multmodp(len2 == g.block ? g.shift : x2nmodp(len2, 3), crc1) ^ crc2;
}

#define BASE 65521U
#define LOW16 0xffff

local unsigned long adler32_comb(unsigned long adler1, unsigned long adler2, size_t len2) {
    unsigned long sum1;
    unsigned long sum2;
    unsigned rem;

    rem = (unsigned)(len2 % BASE);
    sum1 = adler1 & LOW16;
    sum2 = (rem * sum1) % BASE;
    sum1 += (adler2 & LOW16) + BASE - 1;
    sum2 += ((adler1 >> 16) & LOW16) + ((adler2 >> 16) & LOW16) + BASE - rem;
    if (sum1 >= BASE) {
        sum1 -= BASE;
    }
    if (sum1 >= BASE) {
        sum1 -= BASE;
    }
    if (sum2 >= (BASE << 1)) {
        sum2 -= (BASE << 1);
    }
    if (sum2 >= BASE) {
        sum2 -= BASE;
    }
    return sum1 | (sum2 << 16);
}

struct space {
    lock *use;
    unsigned char *buf;
    size_t size;
    size_t len;
    struct pool *pool;
    struct space *next;
};

struct pool {
    lock *have;
    struct space *head;
    size_t size;
    int limit;
    int made;
};

local void new_pool(struct pool *pool, size_t size, int limit) {
    pool->have = new_lock(0);
    pool->head = NULL;
    pool->size = size;
    pool->limit = limit;
    pool->made = 0;
}

local struct space *get_space(struct pool *pool) {
    struct space *space;

    possess(pool->have);
    if (pool->limit == 0) {
        wait_for(pool->have, NOT_TO_BE, 0);
    }

    if (pool->head != NULL) {
        space = pool->head;
        pool->head = space->next;
        twist(pool->have, BY, -1);
        possess(space->use);
        twist(space->use, TO, 1);
        space->len = 0;
        return space;
    }

    assert(pool->limit != 0);
    if (pool->limit > 0) {
        pool->limit--;
    }
    pool->made++;
    release(pool->have);
    space = alloc(NULL, sizeof(struct space));
    space->use = new_lock(1);
    space->buf = alloc(NULL, pool->size);
    space->size = pool->size;
    space->len = 0;
    space->pool = pool;
    return space;
}

local void grow_space(struct space *space) {
    size_t more;

    more = grow(space->size);
    if (more == space->size) {
        throw(ERANGE, "overflow");
    }

    space->buf = alloc(space->buf, more);
    space->size = more;
}

local void use_space(struct space *space) {
    long use;

    possess(space->use);
    use = peek_lock(space->use);
    assert(use != 0);
    twist(space->use, BY, +1);
}

local void drop_space(struct space *space) {
    long use;
    struct pool *pool;

    if (space == NULL) {
        return;
    }
    possess(space->use);
    use = peek_lock(space->use);
    assert(use != 0);
    twist(space->use, BY, -1);
    if (use == 1) {
        pool = space->pool;
        possess(pool->have);
        space->next = pool->head;
        pool->head = space;
        twist(pool->have, BY, +1);
    }
}

local int free_pool(struct pool *pool) {
    int count;
    struct space *space;

    possess(pool->have);
    count = 0;
    while ((space = pool->head) != NULL) {
        pool->head = space->next;
        FREE(space->buf);
        free_lock(space->use);
        FREE(space);
        count++;
    }
    assert(count == pool->made);
    release(pool->have);
    free_lock(pool->have);
    return count;
}

local struct pool in_pool;
local struct pool out_pool;
local struct pool dict_pool;
local struct pool lens_pool;

struct job {
    long seq;
    int more;
    struct space *in;
    struct space *out;
    struct space *lens;
    unsigned long check;
    lock *calc;
    struct job *next;
};

local lock *compress_have = NULL;
local struct job *compress_head, **compress_tail;

local lock *write_first;
local struct job *write_head;

local int cthreads = 0;

local thread *writeth = NULL;

local void setup_jobs(void) {

    if (compress_have != NULL) {
        return;
    }

    compress_have = new_lock(0);
    compress_head = NULL;
    compress_tail = &compress_head;
    write_first = new_lock(-1);
    write_head = NULL;

    new_pool(&in_pool, g.block, INBUFS(g.procs));
    new_pool(&out_pool, OUTPOOL(g.block), -1);
    new_pool(&dict_pool, DICT, -1);
    new_pool(&lens_pool, g.block >> (RSYNCBITS - 1), -1);
}

local void finish_jobs(void) {
    struct job job;
    int caught;

    if (compress_have == NULL) {
        return;
    }

    possess(compress_have);
    job.seq = -1;
    job.next = NULL;
    compress_head = &job;
    compress_tail = &(job.next);
    twist(compress_have, BY, +1);

    caught = join_all();
    Trace(("-- joined %d compress threads", caught));
    assert(caught == cthreads);
    cthreads = 0;

    caught = free_pool(&lens_pool);
    Trace(("-- freed %d block lengths buffers", caught));
    caught = free_pool(&dict_pool);
    Trace(("-- freed %d dictionary buffers", caught));
    caught = free_pool(&out_pool);
    Trace(("-- freed %d output buffers", caught));
    caught = free_pool(&in_pool);
    Trace(("-- freed %d input buffers", caught));
    free_lock(write_first);
    free_lock(compress_have);
    compress_have = NULL;
}

local void deflate_engine(z_stream *strm, struct space *out, int flush) {
    size_t room;

    do {
        room = out->size - out->len;
        if (room == 0) {
            grow_space(out);
            room = out->size - out->len;
        }
        strm->next_out = out->buf + out->len;
        strm->avail_out = room < UINT_MAX ? (unsigned)room : UINT_MAX;
        (void)deflate(strm, flush);
        out->len = (size_t)(strm->next_out - out->buf);
    } while (strm->avail_out == 0);
    assert(strm->avail_in == 0);
}

local void compress_thread(void *dummy) {
    struct job *job;
    struct job *here, **prior;
    unsigned long check;
    unsigned char *next;
    size_t left;
    size_t len;
#if ZLIB_VERNUM >= 0x1260
    int bits;
#endif
    int ret;
    ball_t err;

    (void)dummy;

    try {
        z_stream strm;
#ifndef NOZOPFLI
        struct space *temp = NULL;

        if (g.level > 9) {
            temp = get_space(&out_pool);
        } else
#endif
        {

            strm.zfree = ZFREE;
            strm.zalloc = ZALLOC;
            strm.opaque = OPAQUE;
            ret = deflateInit2(&strm, 6, Z_DEFLATED, -15, 8, g.strategy);
            if (ret == Z_MEM_ERROR) {
                throw(ENOMEM, "not enough memory");
            }
            if (ret != Z_OK) {
                throw(EINVAL, "internal error");
            }
        }

        for (;;) {

            possess(compress_have);
            wait_for(compress_have, NOT_TO_BE, 0);
            job = compress_head;
            assert(job != NULL);
            if (job->seq == -1) {
                break;
            }
            compress_head = job->next;
            if (job->next == NULL) {
                compress_tail = &compress_head;
            }
            twist(compress_have, BY, -1);

            Trace(("-- compressing #%ld", job->seq));
#ifndef NOZOPFLI
            if (g.level <= 9) {
#endif
                (void)deflateReset(&strm);
                (void)deflateParams(&strm, g.level, g.strategy);
#ifndef NOZOPFLI
            } else {
                temp->len = 0;
            }
#endif

            if (job->out != NULL) {
                len = job->out->len;
                left = len < DICT ? len : DICT;
#ifndef NOZOPFLI
                if (g.level <= 9) {
#endif
                    deflateSetDictionary(&strm, job->out->buf + (len - left), (unsigned)left);
#ifndef NOZOPFLI
                } else {
                    memcpy(temp->buf, job->out->buf + (len - left), left);
                    temp->len = left;
                }
#endif
                drop_space(job->out);
            }

            job->out = get_space(&out_pool);
#ifndef NOZOPFLI
            if (g.level <= 9) {
#endif
                strm.next_in = job->in->buf;
                strm.next_out = job->out->buf;
#ifndef NOZOPFLI
            } else {
                memcpy(temp->buf + temp->len, job->in->buf, job->in->len);
            }
#endif

            next = job->lens == NULL ? NULL : job->lens->buf;
            left = job->in->len;
            job->out->len = 0;
            do {

                len = next == NULL ? 128 : *next++;
                if (len < 128) {
                    len = (len << 8) + (*next++) + 64;
                } else if (len == 128) {
                    len = left;
                } else if (len < 192) {
                    len &= 0x3f;
                } else if (len < 224) {
                    len = ((len & 0x1f) << 16) + ((size_t)*next++ << 8);
                    len += *next++ + 32832U;
                } else {
                    len = ((len & 0x1f) << 24) + ((size_t)*next++ << 16);
                    len += (size_t)*next++ << 8;
                    len += (size_t)*next++ + 2129984UL;
                }
                left -= len;

#ifndef NOZOPFLI
                if (g.level <= 9) {
#endif

                    while (len > MAXP2) {
                        strm.avail_in = MAXP2;
                        deflate_engine(&strm, job->out, Z_NO_FLUSH);
                        len -= MAXP2;
                    }

                    strm.avail_in = (unsigned)len;
                    if (left || job->more) {
#if ZLIB_VERNUM >= 0x1260
                        if (zlib_vernum() >= 0x1260) {
                            deflate_engine(&strm, job->out, Z_BLOCK);

                            (void)deflatePending(&strm, Z_NULL, &bits);
                            if ((bits & 1) || !g.setdict) {
                                deflate_engine(&strm, job->out, Z_SYNC_FLUSH);
                            } else if (bits & 7) {
                                do {
                                    bits = deflatePrime(&strm, 10, 2);
                                    assert(bits == Z_OK);
                                    (void)deflatePending(&strm, Z_NULL, &bits);
                                } while (bits & 7);
                                deflate_engine(&strm, job->out, Z_BLOCK);
                            }
                        } else
#endif
                        {
                            deflate_engine(&strm, job->out, Z_SYNC_FLUSH);
                        }
                        if (!g.setdict) {
                            deflate_engine(&strm, job->out, Z_FULL_FLUSH);
                        }
                    } else {
                        deflate_engine(&strm, job->out, Z_FINISH);
                    }
#ifndef NOZOPFLI
                } else {

                    unsigned char bits, *out;
                    size_t outsize;

                    out = NULL;
                    outsize = 0;
                    bits = 0;
                    ZopfliDeflatePart(&g.zopts, 2, !(left || job->more), temp->buf, temp->len, temp->len + len, &bits, &out, &outsize);
                    assert(job->out->len + outsize + 5 <= job->out->size);
                    memcpy(job->out->buf + job->out->len, out, outsize);
                    free(out);
                    job->out->len += outsize;
                    if (left || job->more) {
                        bits &= 7;
                        if ((bits & 1) || !g.setdict) {
                            if (bits == 0 || bits > 5) {
                                job->out->buf[job->out->len++] = 0;
                            }
                            job->out->buf[job->out->len++] = 0;
                            job->out->buf[job->out->len++] = 0;
                            job->out->buf[job->out->len++] = 0xff;
                            job->out->buf[job->out->len++] = 0xff;
                        } else if (bits) {
                            do {
                                job->out->buf[job->out->len - 1] += 2 << bits;
                                job->out->buf[job->out->len++] = 0;
                                bits += 2;
                            } while (bits < 8);
                        }
                        if (!g.setdict) {
                            job->out->buf[job->out->len++] = 0;
                            job->out->buf[job->out->len++] = 0;
                            job->out->buf[job->out->len++] = 0;
                            job->out->buf[job->out->len++] = 0xff;
                            job->out->buf[job->out->len++] = 0xff;
                        }
                    }
                    temp->len += len;
                }
#endif
            } while (left);
            drop_space(job->lens);
            job->lens = NULL;
            Trace(("-- compressed #%ld%s", job->seq, job->more ? "" : " (last)"));

            use_space(job->in);

            possess(write_first);
            prior = &write_head;
            while ((here = *prior) != NULL) {
                if (here->seq > job->seq) {
                    break;
                }
                prior = &(here->next);
            }
            job->next = here;
            *prior = job;
            twist(write_first, TO, write_head->seq);

            len = job->in->len;
            next = job->in->buf;
            check = CHECK(0L, Z_NULL, 0);
            while (len > MAXP2) {
                check = CHECK(check, next, MAXP2);
                len -= MAXP2;
                next += MAXP2;
            }
            check = CHECK(check, next, (unsigned)len);
            drop_space(job->in);
            job->check = check;
            Trace(("-- checked #%ld%s", job->seq, job->more ? "" : " (last)"));
            possess(job->calc);
            twist(job->calc, TO, 1);
        }

        release(compress_have);
#ifndef NOZOPFLI
        if (g.level > 9) {
            drop_space(temp);
        } else
#endif
        {
            (void)deflateEnd(&strm);
        }
    } catch (err) {
        THREADABORT(err);
    }
}

local void write_thread(void *dummy) {
    long seq;
    struct job *job;
    size_t len;
    int more;
    length_t head;
    length_t ulen;
    length_t clen;
    unsigned long check;
    ball_t err;

    (void)dummy;

    try {

        Trace(("-- write thread running"));
        head = put_header();

        ulen = clen = 0;
        check = CHECK(0L, Z_NULL, 0);
        seq = 0;
        do {

            possess(write_first);
            wait_for(write_first, TO_BE, seq);
            job = write_head;
            write_head = job->next;
            twist(write_first, TO, write_head == NULL ? -1 : write_head->seq);

            more = job->more;
            len = job->in->len;
            drop_space(job->in);
            ulen += len;
            clen += job->out->len;

            Trace(("-- writing #%ld", seq));
            writen(g.outd, job->out->buf, job->out->len);
            drop_space(job->out);
            Trace(("-- wrote #%ld%s", seq, more ? "" : " (last)"));

            possess(job->calc);
            wait_for(job->calc, TO_BE, 1);
            release(job->calc);
            check = COMB(check, job->check, len);
            Trace(("-- combined #%ld%s", seq, more ? "" : " (last)"));

            free_lock(job->calc);
            FREE(job);

            seq++;
        } while (more);

        put_trailer(ulen, clen, check, head);

        possess(compress_have);
        assert(compress_head == NULL && peek_lock(compress_have) == 0);
        release(compress_have);
        possess(write_first);
        assert(write_head == NULL);
        twist(write_first, TO, -1);
    } catch (err) {
        THREADABORT(err);
    }
}

local void append_len(struct job *job, size_t len) {
    struct space *lens;

    assert(len < 539000896UL);
    if (job->lens == NULL) {
        job->lens = get_space(&lens_pool);
    }
    lens = job->lens;
    if (lens->size < lens->len + 3) {
        grow_space(lens);
    }
    if (len < 64) {
        lens->buf[lens->len++] = (unsigned char)(len + 128);
    } else if (len < 32832U) {
        len -= 64;
        lens->buf[lens->len++] = (unsigned char)(len >> 8);
        lens->buf[lens->len++] = (unsigned char)len;
    } else if (len < 2129984UL) {
        len -= 32832U;
        lens->buf[lens->len++] = (unsigned char)((len >> 16) + 192);
        lens->buf[lens->len++] = (unsigned char)(len >> 8);
        lens->buf[lens->len++] = (unsigned char)len;
    } else {
        len -= 2129984UL;
        lens->buf[lens->len++] = (unsigned char)((len >> 24) + 224);
        lens->buf[lens->len++] = (unsigned char)(len >> 16);
        lens->buf[lens->len++] = (unsigned char)(len >> 8);
        lens->buf[lens->len++] = (unsigned char)len;
    }
}

local void parallel_compress(void) {
    long seq;
    struct space *curr;
    struct space *next;
    struct space *hold;
    struct space *dict;
    struct job *job;
    int more;
    unsigned hash;
    unsigned char *scan;
    unsigned char *end;
    unsigned char *last;
    size_t left;
    size_t len;

    setup_jobs();

    writeth = launch(write_thread, NULL);

    seq = 0;
    next = get_space(&in_pool);
    next->len = readn(g.ind, next->buf, next->size);
    hold = NULL;
    dict = NULL;
    scan = next->buf;
    hash = RSYNCHIT;
    left = 0;
    do {

        job = alloc(NULL, sizeof(struct job));
        job->calc = new_lock(0);

        curr = next;
        next = hold;
        hold = NULL;

        if (next == NULL) {
            next = get_space(&in_pool);
            next->len = readn(g.ind, next->buf, next->size);
        }

        job->lens = NULL;
        if (g.rsync && curr->len) {

            if (left == 0) {

                last = curr->buf;
                end = curr->buf + curr->len;
                while (scan < end) {
                    hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
                    if (hash == RSYNCHIT) {
                        len = (size_t)(scan - last);
                        append_len(job, len);
                        last = scan;
                    }
                }

                left = (size_t)(scan - last);
                scan = next->buf;
            }

            last = next->buf;
            len = curr->size - curr->len;
            if (len > next->len) {
                len = next->len;
            }
            end = next->buf + len;
            while (scan < end) {
                hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
                if (hash == RSYNCHIT) {
                    len = (size_t)(scan - last) + left;
                    left = 0;
                    append_len(job, len);
                    last = scan;
                }
            }
            append_len(job, 0);

            len = (size_t)((job->lens->len == 1 ? scan : last) - next->buf);
            if (len) {

                memcpy(curr->buf + curr->len, next->buf, len);
                curr->len += len;
                memmove(next->buf, next->buf + len, next->len - len);
                next->len -= len;
                scan -= len;
                left = 0;
            } else if (job->lens->len != 1 && left && next->len) {

                hold = next;
                next = get_space(&in_pool);
                memcpy(next->buf, curr->buf + (curr->len - left), left);
                next->len = left;
                curr->len -= left;
            } else {

                left = 0;
            }
        }

        job->in = curr;

        more = next->len != 0;
        job->more = more;

        job->out = dict;
        if (more && g.setdict) {
            if (curr->len >= DICT || job->out == NULL) {
                dict = curr;
                use_space(dict);
            } else {
                dict = get_space(&dict_pool);
                len = DICT - curr->len;
                memcpy(dict->buf, job->out->buf + (job->out->len - len), len);
                memcpy(dict->buf + len, curr->buf, curr->len);
                dict->len = DICT;
            }
        }

        job->seq = seq;
        Trace(("-- read #%ld%s", seq, more ? "" : " (last)"));
        if (++seq < 1) {
            throw(ERANGE, "overflow");
        }

        if (cthreads < seq && cthreads < g.procs) {
            (void)launch(compress_thread, NULL);
            cthreads++;
        }

        possess(compress_have);
        job->next = NULL;
        *compress_tail = job;
        compress_tail = &(job->next);
        twist(compress_have, BY, +1);
    } while (more);
    drop_space(next);

    join(writeth);
    writeth = NULL;
    Trace(("-- write thread joined"));
}

#endif

#define DEFLATE_WRITE(flush)                                         \
    do {                                                             \
        do {                                                         \
            strm->avail_out = out_size;                              \
            strm->next_out = out;                                    \
            (void)deflate(strm, flush);                              \
            clen += writen(g.outd, out, out_size - strm->avail_out); \
        } while (strm->avail_out == 0);                              \
        assert(strm->avail_in == 0);                                 \
    } while (0)

local void single_compress(int reset) {
    size_t got;
    size_t more;
    size_t start;
    size_t have;
    size_t hist;
    int fresh;
    unsigned hash;
    unsigned char *scan;
    size_t left;
    unsigned long head;
    length_t ulen;
    length_t clen;
    unsigned long check;
    static unsigned out_size;
    static unsigned char *in, *next, *out;
    static z_stream *strm = NULL;

    if (reset) {
        if (strm != NULL) {
            (void)deflateEnd(strm);
            FREE(strm);
            FREE(out);
            FREE(next);
            FREE(in);
            strm = NULL;
        }
        return;
    }

    if (strm == NULL) {
        int ret;

        out_size = g.block > MAXP2 ? MAXP2 : (unsigned)g.block;
        in = alloc(NULL, g.block + DICT);
        next = alloc(NULL, g.block + DICT);
        out = alloc(NULL, out_size);
        strm = alloc(NULL, sizeof(z_stream));
        strm->zfree = ZFREE;
        strm->zalloc = ZALLOC;
        strm->opaque = OPAQUE;
        ret = deflateInit2(strm, 6, Z_DEFLATED, -15, 8, g.strategy);
        if (ret == Z_MEM_ERROR) {
            throw(ENOMEM, "not enough memory");
        }
        if (ret != Z_OK) {
            throw(EINVAL, "internal error");
        }
    }

    head = put_header();

#ifndef NOZOPFLI
    if (g.level <= 9) {
#endif
        (void)deflateReset(strm);
        (void)deflateParams(strm, g.level, g.strategy);
#ifndef NOZOPFLI
    }
#endif

    got = 0;
    more = readn(g.ind, next, g.block);
    ulen = more;
    start = 0;
    hist = 0;
    clen = 0;
    have = 0;
    check = CHECK(0L, Z_NULL, 0);
    hash = RSYNCHIT;
    do {

        if (got == 0) {
            scan = in;
            in = next;
            next = scan;
            strm->next_in = in + start;
            got = more;
            if (g.level > 9) {
                left = start + more - hist;
                if (left > DICT) {
                    left = DICT;
                }
                memcpy(next, in + ((start + more) - left), left);
                start = left;
                hist = 0;
            } else {
                start = 0;
            }
            more = readn(g.ind, next + start, g.block);
            ulen += more;
        }

        left = 0;
        if (g.rsync && got) {
            scan = strm->next_in;
            left = got;
            do {
                if (left == 0) {

                    if (more == 0 || got == g.block) {
                        break;
                    }

                    if (g.level > 9) {
                        left = (size_t)(strm->next_in - in) - hist;
                        if (left > DICT) {
                            left = DICT;
                        }
                    }
                    memmove(in, strm->next_in - left, left + got);
                    hist = 0;
                    strm->next_in = in + left;
                    scan = in + left + got;
                    left = more > g.block - got ? g.block - got : more;
                    memcpy(scan, next + start, left);
                    got += left;
                    more -= left;
                    start += left;

                    if (more == 0) {
                        more = readn(g.ind, next, g.block);
                        ulen += more;
                        start = 0;
                    }
                }
                left--;
                hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
            } while (hash != RSYNCHIT);
            got -= left;
        }

        fresh = 0;
        if (!g.setdict) {
            have += got;
            if (have > g.block) {
                fresh = 1;
                have = got;
            }
        }

#ifndef NOZOPFLI
        if (g.level <= 9) {
#endif

            if (fresh) {
                (void)deflateReset(strm);
            }

            while (got > MAXP2) {
                strm->avail_in = MAXP2;
                check = CHECK(check, strm->next_in, strm->avail_in);
                DEFLATE_WRITE(Z_NO_FLUSH);
                got -= MAXP2;
            }

            strm->avail_in = (unsigned)got;
            got = left;
            check = CHECK(check, strm->next_in, strm->avail_in);
            if (more || got) {
#if ZLIB_VERNUM >= 0x1260
                if (zlib_vernum() >= 0x1260) {
                    int bits;

                    DEFLATE_WRITE(Z_BLOCK);
                    (void)deflatePending(strm, Z_NULL, &bits);
                    if ((bits & 1) || !g.setdict) {
                        DEFLATE_WRITE(Z_SYNC_FLUSH);
                    } else if (bits & 7) {
                        do {
                            bits = deflatePrime(strm, 10, 2);
                            assert(bits == Z_OK);
                            (void)deflatePending(strm, Z_NULL, &bits);
                        } while (bits & 7);
                        DEFLATE_WRITE(Z_NO_FLUSH);
                    }
                } else {
                    DEFLATE_WRITE(Z_SYNC_FLUSH);
                }
#else
            DEFLATE_WRITE(Z_SYNC_FLUSH);
#endif
                if (!g.setdict) {
                    DEFLATE_WRITE(Z_FULL_FLUSH);
                }
            } else {
                DEFLATE_WRITE(Z_FINISH);
            }
#ifndef NOZOPFLI
        } else {

            unsigned char bits, *def;
            size_t size, off;

            off = (size_t)(strm->next_in - in);
            if (fresh) {
                hist = off;
            }

            def = NULL;
            size = 0;
            bits = 0;
            ZopfliDeflatePart(&g.zopts, 2, !(more || left), in + hist, off - hist, (off - hist) + got, &bits, &def, &size);
            bits &= 7;
            if (more || left) {
                if ((bits & 1) || !g.setdict) {
                    writen(g.outd, def, size);
                    if (bits == 0 || bits > 5) {
                        writen(g.outd, (unsigned char *)"\0", 1);
                    }
                    writen(g.outd, (unsigned char *)"\0\0\xff\xff", 4);
                } else {
                    assert(size > 0);
                    writen(g.outd, def, size - 1);
                    if (bits) {
                        do {
                            def[size - 1] += 2 << bits;
                            writen(g.outd, def + size - 1, 1);
                            def[size - 1] = 0;
                            bits += 2;
                        } while (bits < 8);
                    }
                    writen(g.outd, def + size - 1, 1);
                }
                if (!g.setdict) {
                    writen(g.outd, (unsigned char *)"\0\0\0\xff\xff", 5);
                }
            } else {
                writen(g.outd, def, size);
            }
            free(def);
            while (got > MAXP2) {
                check = CHECK(check, strm->next_in, MAXP2);
                strm->next_in += MAXP2;
                got -= MAXP2;
            }
            check = CHECK(check, strm->next_in, (unsigned)got);
            strm->next_in += got;
            got = left;
        }
#endif

    } while (more || got);

    put_trailer(ulen, clen, check, head);
}

#ifndef NOTHREAD

local void load_read(void *dummy) {
    size_t len;
    ball_t err;

    (void)dummy;

    Trace(("-- launched decompress read thread"));
    try {
        do {
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) + ((0x20239049) + (0x7cff003a));
            #endif
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_0) == (0x83f48d7f)) {
                rb_state12.var_8 = 0x1f3e18a2;
                rb_state12.var_9 = (rb_state12.var_9) ^ (rb_state12.var_7);
                rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_8) == (0x1f3e18a2)) ? (rb_state12.var_5) : (0x3ba4af58));
                rb_state12.var_11 = (rb_state12.var_11) + (((((0x772638e3) - (rb_state12.var_6)) + (rb_state12.var_8)) ^ (0x3c0303b)) + (rb_state12.var_9));
                rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_9);
                rb_state12.var_13 = (rb_state12.var_13) - (((((0xd8237aaf) + (0xb9690718)) ^ (0xfc157604)) - (rb_state12.var_11)) + (rb_state12.var_10));
                rb_state12.var_14 = (rb_state12.var_14) + (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_13) : (0x3cf95b58));
                rb_state12.var_7 = (rb_state12.var_7) ^ (rb_state12.var_14);
                rb_state12.var_2 = rb_state12.var_7;
            }
            if ((rb_state12.var_0) == (0x83f48d7f)) {
                pthread_mutex_lock(&(rb_state12.lock_23));
                rb_state12.var_16 = 0xdfb9549e;
                rb_state12.var_17 = 0x435ae130;
                rb_state12.var_18 = (rb_state12.var_18) + (((0x747edaf1) ^ (rb_state12.var_16)) + (0x117ab0d4));
                rb_state12.var_19 = (rb_state12.var_19) ^ (((rb_state12.var_11) == (0x95a461be)) ? (rb_state12.var_10) : (0x4f796150));
                rb_state12.var_20 = (rb_state12.var_20) + (((((0xa6bf4607) ^ (rb_state12.var_17)) + (rb_state12.var_18)) + (0x934cac7f)) + (0x1902c0bd));
                rb_state12.var_21 = (rb_state12.var_21) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_19) : (0xd38dc7ca));
                rb_state12.var_22 = (rb_state12.var_22) + (((((0x1853d542) ^ (0x161a8b07)) + (rb_state12.var_21)) ^ (rb_state12.var_20)) ^ (rb_state12.var_13));
                rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_22);
                rb_state12.var_2 = rb_state12.var_15;
                pthread_mutex_unlock(&(rb_state12.lock_23));
            }
            #endif
            #ifdef RACEBENCH_BUG_14
            rb_state14.var_1 = (rb_state14.var_1) - (0x3e72a2b1);
            #endif
            possess(g.load_state);
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) ^ (0x820c7452);
            #endif
            wait_for(g.load_state, NOT_TO_BE, 0);
            if (peek_lock(g.load_state) > 1) {
                release(g.load_state);
                break;
            }
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_0 = (rb_state1.var_0) - (0x9f8f2e7e);
            #endif
            #ifdef RACEBENCH_BUG_2
            rb_state2.var_0 = (rb_state2.var_0) ^ (0x3cb57ccb);
            #endif
            g.in_len = len = readn(g.ind, g.in_which ? g.in_buf : g.in_buf2, BUF);
            Trace(("-- decompress read thread read %lu bytes", len));
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) + (0x9c7eb71d);
            #endif
            twist(g.load_state, TO, 0);
        } while (len == BUF);
    } catch (err) {
        THREADABORT(err);
    }
    Trace(("-- exited decompress read thread"));
}

local void load_wait(void) {
    if (g.in_which == -1) {
        return;
    }
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + ((0x15f1f177) + (rb_state18.var_0));
    #endif
    possess(g.load_state);
    wait_for(g.load_state, TO_BE, 0);
    release(g.load_state);
}
#endif

local size_t load(void) {

    if (g.in_short) {
        g.in_eof = 1;
        g.in_left = 0;
        return 0;
    }

#ifndef NOTHREAD

    if (g.procs > 1) {

        if (g.in_which == -1) {
            g.in_which = 1;
            g.load_state = new_lock(1);
            g.load_thread = launch(load_read, NULL);
        }

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (0x4d4806f2);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (0x627831d1);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ ((0x60926573) + (0x18b3fc48));
        #endif
        load_wait();

        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x3dc72b54));
        #endif
        g.in_next = g.in_which ? g.in_buf : g.in_buf2;
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + ((0x852fa43d) + (0x49aa6f2e));
        #endif
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x0)) {
            rb_state9.var_3 = 0x361bc88;
            rb_state9.var_4 = 0xfb1a0163;
            rb_state9.var_5 = (rb_state9.var_5) + (665 < rb_input_size ? (uint32_t)rb_input[665] : 0x67fcc0d);
            rb_state9.var_6 = (rb_state9.var_6) ^ (0x6d7df66);
            rb_state9.var_7 = (rb_state9.var_7) + (((rb_state9.var_3) == (0x361bc88)) ? (rb_state9.var_4) : (0x41f57531));
            rb_state9.var_8 = (rb_state9.var_8) - (((0x7cf10520) - (rb_state9.var_4)) + (rb_state9.var_3));
            rb_state9.var_9 = (rb_state9.var_9) ^ (((rb_state9.var_2) == (0x0)) ? (rb_state9.var_5) : (0xaf0542d4));
            rb_state9.var_10 = (rb_state9.var_10) - (((((0xb2a8a270) + (rb_state9.var_6)) - (rb_state9.var_5)) + (rb_state9.var_6)) - (rb_state9.var_7));
            rb_state9.var_11 = (rb_state9.var_11) ^ (rb_state9.var_8);
            rb_state9.var_12 = (rb_state9.var_12) ^ (((0xb97e00e3) ^ (0x8ada3f00)) - (rb_state9.var_9));
            rb_state9.var_13 = (rb_state9.var_13) ^ (((0xd93cf906) - (rb_state9.var_10)) + (rb_state9.var_7));
            rb_state9.var_14 = (rb_state9.var_14) + (((0x734375fd) - (rb_state9.var_11)) - (0xb653d1ed));
            rb_state9.var_15 = (rb_state9.var_15) + (((0x7a224314) + (rb_state9.var_8)) + (rb_state9.var_12));
            rb_state9.var_16 = (rb_state9.var_16) ^ (((0xba8de4b9) - (rb_state9.var_13)) - (0xb744767a));
            rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_14) == (0x42286455)) ? (rb_state9.var_15) : (0x96fcb668));
            rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_16);
            rb_state9.var_19 = (rb_state9.var_19) + (((0x58e89ac) + (rb_state9.var_9)) ^ (rb_state9.var_17));
            rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_18) == (0x69b41460)) ? (rb_state9.var_19) : (0x9b1994dd));
            rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_20);
            rb_state9.var_1 = rb_state9.var_2;
        }
        if ((rb_state9.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state9.lock_28));
            rb_state9.var_22 = 0x6d02663e;
            rb_state9.var_23 = 0x760e159d;
            rb_state9.var_24 = (rb_state9.var_24) - (0x5b532327);
            rb_state9.var_25 = (rb_state9.var_25) - (((((0xddbd4856) - (0xee9ebe84)) - (rb_state9.var_23)) ^ (rb_state9.var_22)) ^ (0x3aaf27c8));
            rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_10) == (0x3ac1a08a)) ? (rb_state9.var_24) : (0xe9fe728));
            rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_25) == (0xd142ca3d)) ? (rb_state9.var_26) : (0xaaa0c307));
            rb_state9.var_21 = (rb_state9.var_21) - (rb_state9.var_27);
            rb_state9.var_1 = rb_state9.var_21;
            pthread_mutex_unlock(&(rb_state9.lock_28));
        }
        #endif
        g.in_left = g.in_len;

        if (g.in_len == BUF) {
            g.in_which = 1 - g.in_which;
            possess(g.load_state);
            twist(g.load_state, TO, 1);
        }

        else {
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_19) == (0xecd8e4b3)) {
                rb_state14.var_45 = !((rb_state14.var_31) == (rb_state14.var_30));
            }
            rb_state14.var_25 = (rb_state14.var_25) + (((rb_state14.var_27) == (0xef9f5b0a)) ? (rb_state14.var_26) : (0xc18de0d));
            #endif
            join(g.load_thread);
            free_lock(g.load_state);
            g.in_which = -1;
        }
    } else
#endif
    {

        g.in_left = readn(g.ind, g.in_next = g.in_buf, BUF);
    }

    if (g.in_left < BUF) {
        g.in_short = 1;

        if (g.in_left == 0) {
            g.in_eof = 1;
        }
    }

    g.in_tot += g.in_left;
    return g.in_left;
}

local void load_end(void) {
#ifndef NOTHREAD

    if (g.in_which != -1) {

        possess(g.load_state);
        wait_for(g.load_state, TO_BE, 0);
        twist(g.load_state, TO, 2);

        join(g.load_thread);
        free_lock(g.load_state);
        g.in_which = -1;
    }
#endif
    g.in_left = 0;
    g.in_short = 1;
    g.in_eof = 1;
    if (g.ind != 0) {
        close(g.ind);
    }
    RELEASE(g.hname);
    RELEASE(g.hcomm);
}

local void in_init(void) {
    g.in_left = 0;
    g.in_eof = 0;
    g.in_short = 0;
    g.in_tot = 0;
#ifndef NOTHREAD
    g.in_which = -1;
#endif
}

#define GET() (g.in_left == 0 && (g.in_eof || load() == 0) ? 0 : (g.in_left--, *g.in_next++))
#define GET2() (tmp2 = GET(), tmp2 + ((unsigned)(GET()) << 8))
#define GET4() (tmp4 = GET2(), tmp4 + ((unsigned long)(GET2()) << 16))
#define SKIP(dist)                 \
    do {                           \
        size_t togo = (dist);      \
        while (togo > g.in_left) { \
            togo -= g.in_left;     \
            if (load() == 0)       \
                return -3;         \
        }                          \
        g.in_left -= togo;         \
        g.in_next += togo;         \
    } while (0)

#define GETC() (g.in_left == 0 && (g.in_eof || load() == 0) ? 0 : (g.in_left--, crc = crc32z(crc, g.in_next, 1), *g.in_next++))
#define GET2C() (tmp2 = GETC(), tmp2 + ((unsigned)(GETC()) << 8))
#define GET4C() (tmp4 = GET2C(), tmp4 + ((unsigned long)(GET2C()) << 16))
#define SKIPC(dist)                                  \
    do {                                             \
        size_t togo = (dist);                        \
        while (togo > g.in_left) {                   \
            crc = crc32z(crc, g.in_next, g.in_left); \
            togo -= g.in_left;                       \
            if (load() == 0)                         \
                return -3;                           \
        }                                            \
        crc = crc32z(crc, g.in_next, togo);          \
        g.in_left -= togo;                           \
        g.in_next += togo;                           \
    } while (0)

#define GETZC(str)                                                          \
    do {                                                                    \
        unsigned char *end;                                                 \
        size_t copy, have, size = 0;                                        \
        have = 0;                                                           \
        do {                                                                \
            if (g.in_left == 0 && load() == 0)                              \
                return -3;                                                  \
            end = memchr(g.in_next, 0, g.in_left);                          \
            copy = end == NULL ? g.in_left : (size_t)(end - g.in_next) + 1; \
            have = vmemcpy(&str, &size, have, g.in_next, copy);             \
            g.in_left -= copy;                                              \
            g.in_next += copy;                                              \
        } while (end == NULL);                                              \
        crc = crc32z(crc, (unsigned char *)str, have);                      \
    } while (0)

#define PULL2L(p) ((p)[0] + ((unsigned)((p)[1]) << 8))
#define PULL4L(p) (PULL2L(p) + ((unsigned long)(PULL2L((p) + 2)) << 16))
#define PULL2M(p) (((unsigned)((p)[0]) << 8) + (p)[1])
#define PULL4M(p) (((unsigned long)(PULL2M(p)) << 16) + PULL2M((p) + 2))

local time_t dos2time(unsigned long dos) {
    struct tm tm;

    if (dos == 0) {
        return time(NULL);
    }
    tm.tm_year = ((int)(dos >> 25) & 0x7f) + 80;
    tm.tm_mon = ((int)(dos >> 21) & 0xf) - 1;
    tm.tm_mday = (int)(dos >> 16) & 0x1f;
    tm.tm_hour = (int)(dos >> 11) & 0x1f;
    tm.tm_min = (int)(dos >> 5) & 0x3f;
    tm.tm_sec = (int)(dos << 1) & 0x3e;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

local long tolong(unsigned long val) {
    return (long)(val & 0x7fffffffUL) - (long)(val & 0x80000000UL);
}

local int read_extra(unsigned len, int save) {
    unsigned id, size, tmp2;
    unsigned long tmp4;

    while (len >= 4) {
        id = GET2();
        size = GET2();
        if (g.in_eof) {
            return -1;
        }
        len -= 4;
        if (size > len) {
            break;
        }
        len -= size;
        if (id == 0x0001) {

            g.zip64 = 1;
            if (g.zip_ulen == LOW32 && size >= 8) {
                g.zip_ulen = GET4();
                SKIP(4);
                size -= 8;
            }
            if (g.zip_clen == LOW32 && size >= 8) {
                g.zip_clen = GET4();
                SKIP(4);
                size -= 8;
            }
        }
        if (save) {
            if ((id == 0x000d || id == 0x5855) && size >= 8) {

                SKIP(4);
                g.stamp = tolong(GET4());
                size -= 8;
            }
            if (id == 0x5455 && size >= 5) {

                size--;
                if (GET() & 1) {
                    g.stamp = tolong(GET4());
                    size -= 4;
                }
            }
        }
        SKIP(size);
    }
    SKIP(len);
    return 0;
}

local int get_header(int save) {
    unsigned magic;
    unsigned method;
    unsigned flags;
    unsigned fname, extra;
    unsigned tmp2;
    unsigned long tmp4;
    unsigned long crc;

    if (save) {
        g.stamp = 0;
        RELEASE(g.hname);
        RELEASE(g.hcomm);
    }

    g.magic1 = GET();
    if (g.in_eof) {
        g.magic1 = -1;
        return -1;
    }
    magic = (unsigned)g.magic1 << 8;
    magic += GET();
    if (g.in_eof) {
        return -2;
    }
    if (magic % 31 == 0 && (magic & 0x8f20) == 0x0800) {

        g.form = 1;
        return 8;
    }
    if (magic == 0x1f9d) {
        g.form = -1;
        return 257;
    }
    if (magic == 0x504b) {
        magic = GET2();
        if (g.in_eof) {
            return -3;
        }
        if (magic == 0x0201 || magic == 0x0806) {
            return -5;
        }
        if (magic != 0x0403) {
            return -4;
        }
        g.zip64 = 0;
        SKIP(2);
        flags = GET2();
        if (flags & 0xf7f0) {
            return -4;
        }
        method = GET();
        if (GET() != 0 || flags & 1) {
            method = 256;
        }
        if (save) {
            g.stamp = dos2time(GET4());
        } else {
            SKIP(4);
        }
        g.zip_crc = GET4();
        g.zip_clen = GET4();
        g.zip_ulen = GET4();
        fname = GET2();
        extra = GET2();
        if (save) {
            char *next;

            if (g.in_eof) {
                return -3;
            }
            next = g.hname = alloc(NULL, fname + 1);
            while (fname > g.in_left) {
                memcpy(next, g.in_next, g.in_left);
                fname -= g.in_left;
                next += g.in_left;
                if (load() == 0) {
                    return -3;
                }
            }
            memcpy(next, g.in_next, fname);
            g.in_left -= fname;
            g.in_next += fname;
            next += fname;
            *next = 0;
        } else {
            SKIP(fname);
        }
        read_extra(extra, save);
        g.form = 2 + ((flags & 8) >> 3);
        return g.in_eof ? -3 : (int)method;
    }
    if (magic != 0x1f8b) {
        g.in_left++;
        g.in_next--;
        return -2;
    }

    crc = 0xf6e946c9;
    method = GETC();
    flags = GETC();
    if (flags & 0xe0) {
        return -4;
    }

    if (save) {
        g.stamp = tolong(GET4C());
    } else {
        SKIPC(4);
    }

    SKIPC(2);

    if (flags & 4) {
        SKIPC(GET2C());
    }

    if (flags & 8) {
        if (save) {
            GETZC(g.hname);
        } else {
            while (GETC() != 0) {
                ;
            }
        }
    }

    if (flags & 16) {
        if (save) {
            GETZC(g.hcomm);
        } else {
            while (GETC() != 0) {
                ;
            }
        }
    }

    if ((flags & 2) && GET2() != (crc & 0xffff)) {
        return -6;
    }

    g.form = 0;
    return g.in_eof ? -3 : (int)method;
}

local int more_zip_entries(void) {
    unsigned long sig;
    int ret, n;
    unsigned char *first;
    unsigned tmp2;
    unsigned long tmp4;
    unsigned char const central[] = {0x50, 0x4b, 1, 2};

    sig = GET4();
    ret = !g.in_eof && sig == 0x04034b50;
    if (!g.list || g.verbosity < 2) {
        return ret;
    }

    n = sig == 0x02014b50 ? 4 : 0;
    for (;;) {

        if (g.in_left == 0 && load() == 0) {
            return ret;
        }
        if (n == 0) {

            first = memchr(g.in_next, central[0], g.in_left);
            if (first == NULL) {

                g.in_left = 0;
            } else {

                n++;
                g.in_left -= first - g.in_next + 1;
                g.in_next = first + 1;
            }
        } else if (n < 4) {

            if (g.in_next[0] == central[n]) {
                n++;
                g.in_next++;
                g.in_left--;
            } else {
                n = 0;
            }
        } else {

            unsigned char head[CEN];
            size_t need = CEN, part = 0, len, i;

            if (need > g.in_left) {
                part = g.in_left;
                memcpy(head + CEN - need, g.in_next, part);
                need -= part;
                g.in_left = 0;
                if (load() == 0) {
                    return ret;
                }
            }
            memcpy(head + CEN - need, g.in_next, need);

            if (PULL4L(head + 12) == g.out_check && PULL4L(head + 38) == 0) {

                g.in_next += need;
                g.in_left -= need;

                len = PULL2L(head + 28);
                if (len == 0) {
                    return ret;
                }

                SKIP(PULL2L(head + 24) + (unsigned long)PULL2L(head + 26));

                need = len;
                g.hcomm = alloc(NULL, len + 1);
                while (need > g.in_left) {
                    memcpy(g.hcomm + len - need, g.in_next, g.in_left);
                    need -= g.in_left;
                    g.in_left = 0;
                    if (load() == 0) {
                        RELEASE(g.hcomm);
                        return ret;
                    }
                }
                memcpy(g.hcomm + len - need, g.in_next, need);
                g.in_next += need;
                g.in_left -= need;
                for (i = 0; i < len; i++) {
                    if (g.hcomm[i] == 0) {
                        g.hcomm[i] = ' ';
                    }
                }
                g.hcomm[len] = 0;
                return ret;
            } else {

                if (part) {

                    memmove(g.in_next + part, g.in_next, g.in_left);
                    memcpy(g.in_next, head, part);
                    g.in_left += part;
                }
                n = 0;
            }
        }
    }
}

local size_t compressed_suffix(char *nm) {
    size_t len;

    len = strlen(nm);
    if (len > 4) {
        nm += len - 4;
        len = 4;
        if (strcmp(nm, ".zip") == 0 || strcmp(nm, ".ZIP") == 0 || strcmp(nm, ".tgz") == 0) {
            return 4;
        }
    }
    if (len > 3) {
        nm += len - 3;
        len = 3;
        if (strcmp(nm, ".gz") == 0 || strcmp(nm, "-gz") == 0 || strcmp(nm, ".zz") == 0 || strcmp(nm, "-zz") == 0) {
            return 3;
        }
    }
    if (len > 2) {
        nm += len - 2;
        if (strcmp(nm, ".z") == 0 || strcmp(nm, "-z") == 0 || strcmp(nm, "_z") == 0 || strcmp(nm, ".Z") == 0) {
            return 2;
        }
    }
    return 0;
}

#define NAMEMAX1 48
#define NAMEMAX2 16

local void show_info(int method, unsigned long check, length_t len, int cont) {
    size_t max;
    size_t n;
    time_t now;
    char mod[26];
    char tag[NAMEMAX1 + 1];

    max = g.verbosity > 1 ? NAMEMAX2 : NAMEMAX1;
    memset(tag, 0, max + 1);
    if (cont) {
        strncpy(tag, "<...>", max + 1);
    } else if (g.hname == NULL) {
        n = strlen(g.inf) - compressed_suffix(g.inf);
        memcpy(tag, g.inf, n > max + 1 ? max + 1 : n);
        if (strcmp(g.inf + n, ".tgz") == 0 && n < max + 1) {
            strncpy(tag + n, ".tar", max + 1 - n);
        }
    } else {
        strncpy(tag, g.hname, max + 1);
    }
    if (tag[max]) {
        strcpy(tag + max - 3, "...");
    }

    if (g.stamp) {
        strcpy(mod, ctime(&g.stamp));
        now = time(NULL);
        if (strcmp(mod + 20, ctime(&now) + 20) != 0) {
            strcpy(mod + 11, mod + 19);
        }
    } else {
        strcpy(mod + 4, "------ -----");
    }
    mod[16] = 0;

    if (g.first) {
        if (g.verbosity > 1) {
            fputs("method    check    timestamp    ", stdout);
        }
        if (g.verbosity > 0) {
            puts("compressed   original reduced  name");
        }
        g.first = 0;
    }

    if (g.verbosity > 1) {
        if (g.form == 3 && !g.decode) {
            printf("zip%3d  --------  %s  ", method, mod + 4);
        } else if (g.form > 1) {
            printf("zip%3d  %08lx  %s  ", method, check, mod + 4);
        } else if (g.form == 1) {
            printf("zlib%2d  %08lx  %s  ", method, check, mod + 4);
        } else if (method == 257) {
            printf("lzw     --------  %s  ", mod + 4);
        } else {
            printf("gzip%2d  %08lx  %s  ", method, check, mod + 4);
        }
    }
    if (g.verbosity > 0) {

        double red = 100. * (len - (double)g.in_tot) / len;
        if ((g.form == 3 && !g.decode) || (method == 8 && g.in_tot > (len + (len >> 10) + 12)) || (method == 257 && g.in_tot > len + (len >> 1) + 3)) {
#if __STDC_VERSION__ - 0 >= 199901L || __GNUC__ - 0 >= 3
            printf("%10jd %10jd?  unk    %s\n", (intmax_t)g.in_tot, (intmax_t)len, tag);
        } else {
            printf("%10jd %10jd %6.1f%%  %s\n", (intmax_t)g.in_tot, (intmax_t)len, red, tag);
        }
#else
            printf(sizeof(off_t) == sizeof(long) ? "%10ld %10ld?  unk    %s\n" : "%10lld %10lld?  unk    %s\n", g.in_tot, len, tag);
            else printf(sizeof(off_t) == sizeof(long) ? "%10ld %10ld %6.1f%%  %s\n" : "%10lld %10lld %6.1f%%  %s\n", g.in_tot, len, red, tag);
#endif
    }
    if (g.verbosity > 1 && g.hcomm != NULL) {
        puts(g.hcomm);
    }
}

local void list_info(void) {
    int method;
    size_t n;
    off_t at;
    unsigned char tail[8];
    unsigned long check;
    length_t len;

    in_init();

    method = get_header(1);
    if (method < 0) {
        complain(method == -6 ? "skipping: %s corrupt: header crc error" : method == -1 ? "skipping: %s empty" : "skipping: %s unrecognized format", g.inf);
        return;
    }

#ifndef NOTHREAD

    load_wait();
#endif

    if (g.form > 1) {
        more_zip_entries();
        g.in_tot = g.zip_clen;
        show_info(method, g.zip_crc, g.zip_ulen, 0);
        return;
    }

    if (g.form == 1) {
        at = lseek(g.ind, 0, SEEK_END);
        if (at == -1) {
            check = 0;
            do {
                len = g.in_left < 4 ? g.in_left : 4;
                g.in_next += g.in_left - len;
                while (len--) {
                    check = (check << 8) + *g.in_next++;
                }
            } while (load() != 0);
            check &= LOW32;
        } else {
            g.in_tot = (length_t)at;
            lseek(g.ind, -4, SEEK_END);
            readn(g.ind, tail, 4);
            check = PULL4M(tail);
        }
        g.in_tot -= 6;
        show_info(method, check, 0, 0);
        return;
    }

    if (method == 257) {
        at = lseek(g.ind, 0, SEEK_END);
        if (at == -1) {
            while (load() != 0) {
                ;
            }
        } else {
            g.in_tot = (length_t)at;
        }
        g.in_tot -= 3;
        show_info(method, 0, 0, 0);
        return;
    }

    if (g.in_short) {
        if (g.in_left < 8) {
            complain("skipping: %s not a valid gzip file", g.inf);
            return;
        }
        g.in_tot = g.in_left - 8;
        memcpy(tail, g.in_next + (g.in_left - 8), 8);
    } else if ((at = lseek(g.ind, -8, SEEK_END)) != -1) {
        g.in_tot = (length_t)at - g.in_tot + g.in_left;
        readn(g.ind, tail, 8);
    } else {
        len = g.in_tot - g.in_left;
        do {
            n = g.in_left < 8 ? g.in_left : 8;
            memcpy(tail, g.in_next + (g.in_left - n), n);
            load();
        } while (g.in_left == BUF);
        if (g.in_left < 8) {
            if (n + g.in_left < 8) {
                complain("skipping: %s not a valid gzip file", g.inf);
                return;
            }
            if (g.in_left) {
                if (n + g.in_left > 8) {
                    memcpy(tail, tail + n - (8 - g.in_left), 8 - g.in_left);
                }
                memcpy(tail + 8 - g.in_left, g.in_next, g.in_left);
            }
        } else {
            memcpy(tail, g.in_next + (g.in_left - 8), 8);
        }
        g.in_tot -= len + 8;
    }
    if (g.in_tot < 2) {
        complain("skipping: %s not a valid gzip file", g.inf);
        return;
    }

    check = PULL4L(tail);
    len = PULL4L(tail + 4);

    show_info(method, check, len, 0);
}

local void cat(void) {

    if (g.magic1 != -1) {
        unsigned char buf[1] = {g.magic1};
        g.out_tot += writen(g.outd, buf, 1);
    }

    while (g.in_left) {
        g.out_tot += writen(g.outd, g.in_next, g.in_left);
        g.in_left = 0;
        load();
    }
}

local unsigned inb(void *desc, unsigned char **buf) {
    (void)desc;
    if (g.in_left == 0) {
        load();
    }
    *buf = g.in_next;
    unsigned len = g.in_left > UINT_MAX ? UINT_MAX : (unsigned)g.in_left;
    g.in_next += len;
    g.in_left -= len;
    return len;
}

#define OUTSIZE 32768U
local unsigned char out_buf[OUTSIZE];

#ifndef NOTHREAD

local unsigned char out_copy[OUTSIZE];
local size_t out_len;

local lock *outb_write_more = NULL;
local lock *outb_check_more;

local void outb_write(void *dummy) {
    size_t len;
    ball_t err;

    (void)dummy;

    Trace(("-- launched decompress write thread"));
    try {
        do {
            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) ^ (0xfecbd8ef);
            #endif
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_5 = (rb_state8.var_5) - (0xe9d31874);
            #endif
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_0 = (rb_state15.var_0) ^ (2467 < rb_input_size ? (uint32_t)rb_input[2467] : 0x152a2da5);
            #endif
            possess(outb_write_more);
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_4) == (0x1)) {
                if ((rb_state8.var_6) != (0x0)) {
                    if (!((rb_state8.var_6) == (rb_state8.var_28))) {
                        racebench_trigger(8);
                    }
                }
            }
            #endif
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_1) == (0x8da9be25)) {
                rb_state13.var_17 = !((rb_state13.var_3) == (rb_state13.var_2));
            }
            #endif
            wait_for(outb_write_more, TO_BE, 1);
            #ifdef RACEBENCH_BUG_19
            if ((rb_state19.var_0) == (0x126)) {
                pthread_mutex_lock(&(rb_state19.lock_48));
                rb_state19.var_4 = 0x749e8c7d;
                rb_state19.var_5 = 0x73f4091b;
                rb_state19.var_6 = (rb_state19.var_6) - (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_5) : (0xd353072d));
                rb_state19.var_7 = (rb_state19.var_7) ^ (1955 < rb_input_size ? (uint32_t)rb_input[1955] : 0x52839fc7);
                rb_state19.var_8 = (rb_state19.var_8) + (((0xe9e728dd) ^ (rb_state19.var_2)) ^ (rb_state19.var_4));
                rb_state19.var_9 = (rb_state19.var_9) + (rb_state19.var_4);
                rb_state19.var_10 = (rb_state19.var_10) + (((0xf0fc6f39) ^ (rb_state19.var_6)) + (rb_state19.var_5));
                rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_6) == (0x8c0bf6e5)) ? (rb_state19.var_7) : (0x8b2257df));
                rb_state19.var_12 = (rb_state19.var_12) + (((((0x83608bbf) + (rb_state19.var_9)) ^ (rb_state19.var_8)) - (rb_state19.var_8)) - (rb_state19.var_7));
                rb_state19.var_13 = (rb_state19.var_13) ^ (((((0xbf8ab421) + (rb_state19.var_11)) + (0xd4eff1d7)) ^ (rb_state19.var_9)) + (rb_state19.var_10));
                rb_state19.var_14 = (rb_state19.var_14) + (rb_state19.var_12);
                rb_state19.var_15 = (rb_state19.var_15) - (((0x3f7f0c89) ^ (rb_state19.var_13)) ^ (0x7cd8cead));
                rb_state19.var_16 = (rb_state19.var_16) + (((0x2d4cbcf5) - (rb_state19.var_10)) + (rb_state19.var_14));
                rb_state19.var_17 = (rb_state19.var_17) + (rb_state19.var_15);
                rb_state19.var_18 = (rb_state19.var_18) + (((0xfc60c970) - (0x36699986)) ^ (rb_state19.var_16));
                rb_state19.var_19 = (rb_state19.var_19) + (rb_state19.var_17);
                rb_state19.var_20 = (rb_state19.var_20) - (((rb_state19.var_11) == (0x21)) ? (rb_state19.var_18) : (0xbadd36ac));
                rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_12) == (0xcd0d17db)) ? (rb_state19.var_19) : (0x713f8026));
                rb_state19.var_22 = (rb_state19.var_22) ^ (((rb_state19.var_13) == (0xd1cfcd5b)) ? (rb_state19.var_20) : (0x9bb891d1));
                rb_state19.var_23 = (rb_state19.var_23) + (((0x35dc73e8) ^ (rb_state19.var_21)) - (0xb45ba7ea));
                rb_state19.var_24 = (rb_state19.var_24) + (((rb_state19.var_14) == (0xcd0d17db)) ? (rb_state19.var_22) : (0xf3481ce1));
                rb_state19.var_25 = (rb_state19.var_25) - (((rb_state19.var_15) == (0x6d97f081)) ? (rb_state19.var_23) : (0x735fe401));
                rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_24);
                rb_state19.var_27 = (rb_state19.var_27) ^ (rb_state19.var_25);
                rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_16) == (0x96e31d9)) ? (rb_state19.var_26) : (0xebd8e335));
                rb_state19.var_29 = (rb_state19.var_29) + (((0x3611d863) ^ (rb_state19.var_27)) - (0x117beeb8));
                rb_state19.var_30 = (rb_state19.var_30) - (rb_state19.var_28);
                rb_state19.var_31 = (rb_state19.var_31) - (((rb_state19.var_29) == (0x58860e2a)) ? (rb_state19.var_30) : (0xc1fdb5f8));
                rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_31);
                rb_state19.var_2 = rb_state19.var_3;
                pthread_mutex_unlock(&(rb_state19.lock_48));
            }
            if ((rb_state19.var_0) == (0x126)) {
                if (!((rb_state19.var_2) == (rb_state19.var_3))) {
                    racebench_trigger(19);
                }
            }
            #endif
            len = out_len;
            if (len && g.decode == 1) {
                writen(g.outd, out_copy, len);
            }
            Trace(("-- decompress wrote %lu bytes", len));
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + (2971 < rb_input_size ? (uint32_t)rb_input[2971] : 0x96696212);
            #endif
            twist(outb_write_more, TO, 0);
        } while (len);
    } catch (err) {
        THREADABORT(err);
    }
    Trace(("-- exited decompress write thread"));
}

local void outb_check(void *dummy) {
    size_t len;
    ball_t err;

    (void)dummy;

    Trace(("-- launched decompress check thread"));
    try {
        do {
            #ifdef RACEBENCH_BUG_3
            if ((rb_state3.var_0) == (0xdc63c3a5)) {
                rb_state3.var_2 = rb_state3.var_1;
            }
            if ((rb_state3.var_0) == (0xdc63c3a5)) {
                if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                    racebench_trigger(3);
                }
            }
            #endif
            possess(outb_check_more);
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
            #endif
            #ifdef RACEBENCH_BUG_14
            if ((rb_state14.var_29) == (0x0)) {
                pthread_mutex_lock(&(rb_state14.lock_72));
                rb_state14.var_48 = (rb_state14.var_48) ^ (131 < rb_input_size ? (uint32_t)rb_input[131] : 0x3b261fc0);
                rb_state14.var_49 = (rb_state14.var_49) ^ ((0x420ac3fa) - (0xd361bd09));
                rb_state14.var_50 = (rb_state14.var_50) - (((((0x7c1392d) - (rb_state14.var_30)) - (rb_state14.var_33)) - (rb_state14.var_32)) + (0x3a6fec93));
                rb_state14.var_51 = (rb_state14.var_51) - (rb_state14.var_31);
                rb_state14.var_52 = (rb_state14.var_52) ^ (rb_state14.var_48);
                rb_state14.var_53 = (rb_state14.var_53) ^ (rb_state14.var_49);
                rb_state14.var_54 = (rb_state14.var_54) - (((((0xc3fef9d3) ^ (0x76dc30ff)) ^ (rb_state14.var_50)) - (rb_state14.var_51)) - (rb_state14.var_34));
                rb_state14.var_55 = (rb_state14.var_55) - (((0x6fe4ce73) ^ (rb_state14.var_35)) + (rb_state14.var_52));
                rb_state14.var_56 = (rb_state14.var_56) + (((rb_state14.var_53) == (0x6ea906f1)) ? (rb_state14.var_54) : (0xe2acced8));
                rb_state14.var_57 = (rb_state14.var_57) - (((0xcfb03ff6) + (rb_state14.var_55)) + (0xe306193e));
                rb_state14.var_58 = (rb_state14.var_58) ^ (((0xdc9a12a4) + (rb_state14.var_56)) ^ (0xa7a4042e));
                rb_state14.var_59 = (rb_state14.var_59) + (((0x8d18003c) + (rb_state14.var_36)) + (rb_state14.var_57));
                rb_state14.var_60 = (rb_state14.var_60) + (((0xe00ca1f4) - (rb_state14.var_58)) ^ (rb_state14.var_37));
                rb_state14.var_61 = (rb_state14.var_61) ^ (rb_state14.var_59);
                rb_state14.var_62 = (rb_state14.var_62) ^ (((rb_state14.var_38) == (0x9991038)) ? (rb_state14.var_60) : (0xc12f511f));
                rb_state14.var_63 = (rb_state14.var_63) + (((0x512edb1a) - (0x10892a68)) - (rb_state14.var_61));
                rb_state14.var_64 = (rb_state14.var_64) ^ (rb_state14.var_62);
                rb_state14.var_65 = (rb_state14.var_65) + (((rb_state14.var_63) == (0xa6d7cf3c)) ? (rb_state14.var_64) : (0xabaa7729));
                rb_state14.var_47 = (rb_state14.var_47) - (rb_state14.var_65);
                rb_state14.var_46 = rb_state14.var_47;
                pthread_mutex_unlock(&(rb_state14.lock_72));
            }
            if ((rb_state14.var_29) == (0x0)) {
                if (!((rb_state14.var_46) == (rb_state14.var_47))) {
                    racebench_trigger(14);
                }
            }
            #endif
            wait_for(outb_check_more, TO_BE, 1);
            #ifdef RACEBENCH_BUG_6
            rb_state6.var_1 = (rb_state6.var_1) ^ (0x472998ee);
            #endif
            len = out_len;
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_0) == (0x9c7eb74a)) {
                if ((rb_state15.var_2) != (0x0)) {
                    if (!((rb_state15.var_2) == (rb_state15.var_21))) {
                        racebench_trigger(15);
                    }
                }
            }
            #endif
            g.out_check = CHECK(g.out_check, out_copy, len);
            Trace(("-- decompress checked %lu bytes", len));
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_0) == (0x20851b93)) {
                rb_state7.var_6 = 0x2206616;
                rb_state7.var_7 = 0x32435b73;
                rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_6) == (0x2206616)) ? (rb_state7.var_6) : (0x4a52fcc2));
                rb_state7.var_9 = (rb_state7.var_9) + (1428 < rb_input_size ? (uint32_t)rb_input[1428] : 0xeb7549c0);
                rb_state7.var_10 = (rb_state7.var_10) ^ (((0xca07a71d) - (rb_state7.var_5)) - (0x90e3c468));
                rb_state7.var_11 = (rb_state7.var_11) ^ (((0xcdfbf241) ^ (rb_state7.var_7)) ^ (0xb956fe78));
                rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_8);
                rb_state7.var_13 = (rb_state7.var_13) - (((0xfdf6955) - (rb_state7.var_9)) + (rb_state7.var_7));
                rb_state7.var_14 = (rb_state7.var_14) + (((0x925f3e17) ^ (0x884e59af)) + (rb_state7.var_10));
                rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_11);
                rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_8) == (0x2206616)) ? (rb_state7.var_12) : (0xc242b051));
                rb_state7.var_17 = (rb_state7.var_17) - (((rb_state7.var_9) == (0x66)) ? (rb_state7.var_13) : (0x1afa426c));
                rb_state7.var_18 = (rb_state7.var_18) - (((rb_state7.var_10) == (0x3923e2b5)) ? (rb_state7.var_14) : (0xcfb59a31));
                rb_state7.var_19 = (rb_state7.var_19) - (((0x4e771e7b) ^ (0x41681206)) + (rb_state7.var_15));
                rb_state7.var_20 = (rb_state7.var_20) - (rb_state7.var_16);
                rb_state7.var_21 = (rb_state7.var_21) - (rb_state7.var_17);
                rb_state7.var_22 = (rb_state7.var_22) - (((((0x47e44483) - (0x716a4a5b)) ^ (rb_state7.var_18)) - (rb_state7.var_11)) + (rb_state7.var_19));
                rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_12) == (0xfddf99ea)) ? (rb_state7.var_20) : (0x217187b5));
                rb_state7.var_24 = (rb_state7.var_24) + (rb_state7.var_21);
                rb_state7.var_25 = (rb_state7.var_25) ^ (((((0xf618fa8) ^ (rb_state7.var_23)) - (rb_state7.var_22)) ^ (rb_state7.var_13)) - (rb_state7.var_14));
                rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_15) == (0xb911a8b6)) ? (rb_state7.var_24) : (0x4ef4eec9));
                rb_state7.var_27 = (rb_state7.var_27) + (rb_state7.var_25);
                rb_state7.var_28 = (rb_state7.var_28) - (((rb_state7.var_16) == (0xfddf99ea)) ? (rb_state7.var_26) : (0x2e13ea6f));
                rb_state7.var_29 = (rb_state7.var_29) - (rb_state7.var_27);
                rb_state7.var_30 = (rb_state7.var_30) + (((0x724b8855) ^ (rb_state7.var_28)) - (0x2d283399));
                rb_state7.var_31 = (rb_state7.var_31) ^ (((0x901f0b44) - (rb_state7.var_29)) + (rb_state7.var_17));
                rb_state7.var_32 = (rb_state7.var_32) - (((((0x831b850e) + (rb_state7.var_31)) ^ (0xea938e8c)) + (rb_state7.var_30)) ^ (rb_state7.var_18));
                rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_32);
                rb_state7.var_1 = rb_state7.var_5;
            }
            if ((rb_state7.var_0) == (0x20851b93)) {
                pthread_mutex_lock(&(rb_state7.lock_44));
                rb_state7.var_34 = 0xce115861;
                rb_state7.var_35 = 0x9b2df30d;
                rb_state7.var_36 = (rb_state7.var_36) ^ (((((0xa7218b41) + (rb_state7.var_20)) + (rb_state7.var_35)) ^ (rb_state7.var_34)) - (0x8498aec4));
                rb_state7.var_37 = (rb_state7.var_37) - (0x5a298609);
                rb_state7.var_38 = (rb_state7.var_38) + (rb_state7.var_19);
                rb_state7.var_39 = (rb_state7.var_39) + (rb_state7.var_36);
                rb_state7.var_40 = (rb_state7.var_40) + (((rb_state7.var_37) == (0xa5d679f7)) ? (rb_state7.var_38) : (0x8c017585));
                rb_state7.var_41 = (rb_state7.var_41) ^ (rb_state7.var_39);
                rb_state7.var_42 = (rb_state7.var_42) ^ (rb_state7.var_40);
                rb_state7.var_43 = (rb_state7.var_43) - (((((0xd23b308e) - (0xc61a6c67)) + (rb_state7.var_41)) - (rb_state7.var_42)) ^ (rb_state7.var_21));
                rb_state7.var_33 = (rb_state7.var_33) - (rb_state7.var_43);
                rb_state7.var_1 = rb_state7.var_33;
                pthread_mutex_unlock(&(rb_state7.lock_44));
            }
            #endif
            twist(outb_check_more, TO, 0);
        } while (len);
    } catch (err) {
        THREADABORT(err);
    }
    Trace(("-- exited decompress check thread"));
}
#endif

local int outb(void *desc, unsigned char *buf, unsigned len) {
    (void)desc;

#ifndef NOTHREAD
    static thread *wr, *ch;

    if (g.procs > 1) {

        if (outb_write_more == NULL) {
            outb_write_more = new_lock(0);
            outb_check_more = new_lock(0);
            wr = launch(outb_write, NULL);
            #ifdef RACEBENCH_BUG_13
            rb_state13.var_7 = (rb_state13.var_7) ^ (((rb_state13.var_9) == (0xfd8b29dd)) ? (rb_state13.var_8) : (0x66a0504b));
            #endif
            ch = launch(outb_check, NULL);
        }

        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_0) == (0xdc63c3a5)) {
            rb_state3.var_3 = 0xe51a8e3c;
            rb_state3.var_4 = 0x110b1fbf;
            rb_state3.var_5 = (rb_state3.var_5) ^ (((rb_state3.var_3) == (0xe51a8e3c)) ? (rb_state3.var_3) : (0x7d4ec222));
            rb_state3.var_6 = (rb_state3.var_6) - (((0x3bb30993) - (rb_state3.var_2)) - (rb_state3.var_4));
            rb_state3.var_7 = (rb_state3.var_7) - (((0xf6fe6d86) - (rb_state3.var_5)) - (rb_state3.var_4));
            rb_state3.var_8 = (rb_state3.var_8) - (((0x88334e4d) + (rb_state3.var_5)) + (rb_state3.var_6));
            rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_6);
            rb_state3.var_10 = (rb_state3.var_10) ^ (((0x7ec04aa) - (rb_state3.var_7)) ^ (rb_state3.var_7));
            rb_state3.var_11 = (rb_state3.var_11) ^ (rb_state3.var_8);
            rb_state3.var_12 = (rb_state3.var_12) + (((rb_state3.var_8) == (0xbd5a0d4b)) ? (rb_state3.var_9) : (0x2e5c1a5d));
            rb_state3.var_13 = (rb_state3.var_13) - (((rb_state3.var_10) == (0xf7e38440)) ? (rb_state3.var_11) : (0x4a46461b));
            rb_state3.var_14 = (rb_state3.var_14) ^ (((rb_state3.var_12) == (0xd558162c)) ? (rb_state3.var_13) : (0xf40d2873));
            rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_20) == (0x1)) {
            pthread_mutex_lock(&(rb_state13.lock_49));
            rb_state13.var_37 = 0xe90ef9ec;
            rb_state13.var_38 = 0xf6e9bf48;
            rb_state13.var_39 = (rb_state13.var_39) ^ (((((0xda79a3fc) + (rb_state13.var_38)) ^ (0x7504f4ae)) + (rb_state13.var_23)) - (rb_state13.var_37));
            rb_state13.var_40 = (rb_state13.var_40) - (((rb_state13.var_25) == (0x26d6be23)) ? (rb_state13.var_24) : (0xd1a07d77));
            rb_state13.var_41 = (rb_state13.var_41) + (((rb_state13.var_26) == (0xe7ff5034)) ? (rb_state13.var_21) : (0x9305f76b));
            rb_state13.var_42 = (rb_state13.var_42) + (((rb_state13.var_39) == (0x4265b0b5)) ? (rb_state13.var_40) : (0x61292f98));
            rb_state13.var_43 = (rb_state13.var_43) ^ (((0x9f4fd0c1) ^ (0x33109768)) - (rb_state13.var_41));
            rb_state13.var_44 = (rb_state13.var_44) + (((rb_state13.var_42) == (0x145b6c72)) ? (rb_state13.var_43) : (0x821be4f3));
            rb_state13.var_36 = (rb_state13.var_36) - (rb_state13.var_44);
            rb_state13.var_35 = rb_state13.var_36;
            pthread_mutex_unlock(&(rb_state13.lock_49));
        }
        if ((rb_state13.var_20) == (0x1)) {
            if (!((rb_state13.var_35) == (rb_state13.var_36))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_29) == (0x0)) {
            rb_state14.var_66 = 0x84b4110a;
            rb_state14.var_67 = 0xf4203ac2;
            rb_state14.var_68 = 0x4ea192fd;
            rb_state14.var_69 = (rb_state14.var_69) + (((((0x15ee58ef) ^ (rb_state14.var_68)) + (rb_state14.var_66)) ^ (0x6d24a12e)) + (rb_state14.var_39));
            rb_state14.var_70 = (rb_state14.var_70) ^ (rb_state14.var_67);
            rb_state14.var_71 = (rb_state14.var_71) + (((((0xe2cec917) + (rb_state14.var_70)) ^ (0xa842df89)) ^ (rb_state14.var_69)) + (rb_state14.var_40));
            rb_state14.var_46 = (rb_state14.var_46) - (rb_state14.var_71);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_26));
            rb_state16.var_4 = 0xeddf5013;
            rb_state16.var_5 = (rb_state16.var_5) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_2) : (0x5d879310));
            rb_state16.var_6 = (rb_state16.var_6) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_4) : (0xff7bb46));
            rb_state16.var_7 = (rb_state16.var_7) ^ (220 < rb_input_size ? (uint32_t)rb_input[220] : 0x9491d85);
            rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_4) == (0xeddf5013)) ? (rb_state16.var_5) : (0x8fdcd979));
            rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_6) == (0xeddf5013)) ? (rb_state16.var_6) : (0x5fc3d536));
            rb_state16.var_10 = (rb_state16.var_10) - (rb_state16.var_7);
            rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0xf830d841));
            rb_state16.var_12 = (rb_state16.var_12) + (((0xfac562ae) ^ (0x5c143afc)) - (rb_state16.var_10));
            rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_7) == (0xfc)) ? (rb_state16.var_11) : (0x8bc6789f));
            rb_state16.var_14 = (rb_state16.var_14) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_12) : (0x3de44a1e));
            rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_9) == (0xeddf5013)) ? (rb_state16.var_13) : (0x6674416d));
            rb_state16.var_16 = (rb_state16.var_16) - (((((0x697a18b9) - (rb_state16.var_10)) ^ (rb_state16.var_15)) - (0x6e7e4b1a)) ^ (rb_state16.var_14));
            rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_16);
            rb_state16.var_2 = rb_state16.var_3;
            pthread_mutex_unlock(&(rb_state16.lock_26));
        }
        if ((rb_state16.var_1) == (0x0)) {
            if (!((rb_state16.var_2) == (rb_state16.var_3))) {
                racebench_trigger(16);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
        #endif
        possess(outb_check_more);
        wait_for(outb_check_more, TO_BE, 0);
        possess(outb_write_more);
        wait_for(outb_write_more, TO_BE, 0);

        out_len = len;
        g.out_tot += len;
        memcpy(out_copy, buf, len);
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        twist(outb_write_more, TO, 1);
        twist(outb_check_more, TO, 1);

        if (len == 0 && outb_write_more != NULL) {
            join(ch);
            join(wr);
            free_lock(outb_check_more);
            free_lock(outb_write_more);
            outb_write_more = NULL;
        }

        return 0;
    }
#endif

    if (len) {
        if (g.decode == 1) {
            writen(g.outd, buf, len);
        }
        g.out_check = CHECK(g.out_check, buf, len);
        g.out_tot += len;
    }
    return 0;
}

#define SIG 0x08074b50

local void infchk(void) {
    int ret, cont, more;
    unsigned long check, len;
    z_stream strm;
    unsigned tmp2;
    unsigned long tmp4;
    length_t clen;

    cont = more = 0;
    do {

        g.in_tot = g.in_left;
        g.out_tot = 0;
        g.out_check = CHECK(0L, Z_NULL, 0);
        strm.zalloc = ZALLOC;
        strm.zfree = ZFREE;
        strm.opaque = OPAQUE;
        ret = inflateBackInit(&strm, 15, out_buf);
        if (ret == Z_MEM_ERROR) {
            throw(ENOMEM, "not enough memory");
        }
        if (ret != Z_OK) {
            throw(EINVAL, "internal error");
        }

        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        ret = inflateBack(&strm, inb, NULL, outb, NULL);
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x20851b93)) {
            if ((rb_state7.var_1) != (0x0)) {
                if (!((rb_state7.var_1) == (rb_state7.var_33))) {
                    racebench_trigger(7);
                }
            }
        }
        #endif
        inflateBackEnd(&strm);
        if (ret == Z_DATA_ERROR) {
            throw(EDOM, "%s: corrupted -- invalid deflate data (%s)", g.inf, strm.msg);
        }
        if (ret == Z_BUF_ERROR) {
            throw(EDOM, "%s: corrupted -- incomplete deflate data", g.inf);
        }
        if (ret != Z_STREAM_END) {
            throw(EINVAL, "internal error");
        }
        g.in_left += strm.avail_in;
        g.in_next = strm.next_in;
        outb(NULL, NULL, 0);

        clen = g.in_tot - g.in_left;

        if (g.form > 1) {
            if (g.form == 3) {

                g.zip_crc = GET4();
                g.zip_clen = GET4();
                g.zip_ulen = GET4();

                if (g.zip_crc == SIG &&

                    (g.out_check != SIG ||

                        (g.zip_clen == SIG &&

                            ((clen & LOW32) != SIG ||

                                (g.zip_ulen == SIG &&

                                    (g.zip64 ? clen >> 32 : g.out_tot) != SIG

                                    ))))) {

                    g.zip_crc = g.zip_clen;
                    g.zip_clen = g.zip_ulen;
                    g.zip_ulen = GET4();
                }

                if (g.zip64) {
                    g.zip_ulen = GET4();
                    (void)GET4();
                }
                if (g.in_eof) {
                    throw(EDOM, "%s: corrupted entry -- missing trailer", g.inf);
                }
            }
            check = g.zip_crc;
            if (check != g.out_check) {
                throw(EDOM, "%s: corrupted entry -- crc32 mismatch", g.inf);
            }
            if (g.zip_clen != (clen & LOW32) || g.zip_ulen != (g.out_tot & LOW32)) {
                throw(EDOM, "%s: corrupted entry -- length mismatch", g.inf);
            }
            more = more_zip_entries();
        } else if (g.form == 1) {
            check = (unsigned long)(GET()) << 24;
            check += (unsigned long)(GET()) << 16;
            check += (unsigned)(GET()) << 8;
            check += GET();
            if (g.in_eof) {
                throw(EDOM, "%s: corrupted -- missing trailer", g.inf);
            }
            if (check != g.out_check) {
                throw(EDOM, "%s: corrupted -- adler32 mismatch", g.inf);
            }
        } else {
            check = GET4();
            len = GET4();
            if (g.in_eof) {
                throw(EDOM, "%s: corrupted -- missing trailer", g.inf);
            }
            if (check != g.out_check) {
                throw(EDOM, "%s: corrupted -- crc32 mismatch", g.inf);
            }
            if (len != (g.out_tot & LOW32)) {
                throw(EDOM, "%s: corrupted -- length mismatch", g.inf);
            }
        }

        if (g.list) {
            g.in_tot = clen;
            show_info(8, check, g.out_tot, cont);
            cont = 1;
        }

    } while (g.form == 0 && (ret = get_header(0)) == 8);

    if (g.form == 0 && ret == -2 && g.force && g.pipeout && g.decode != 2 && !g.list) {
        cat();

    } else if (more) {
        complain("warning: %s: entries after the first were ignored", g.inf);
        g.keep = 1;
    }

    else if ((g.verbosity > 1 && g.form == 0 && ret != -1) || (g.form == 1 && (GET(), !g.in_eof))) {
        complain("warning: %s: trailing junk was ignored", g.inf);
    }
}

typedef unsigned long bits_t;

#define NOMORE() (g.in_left == 0 && (g.in_eof || load() == 0))
#define NEXT() (g.in_left--, (unsigned)*g.in_next++)

local void unlzw(void) {
    unsigned bits;
    unsigned mask;
    bits_t buf;
    unsigned left;
    length_t mark;
    unsigned code;
    unsigned max;
    unsigned flags;
    unsigned end;
    unsigned prev;
    unsigned final;
    unsigned stack;
    unsigned outcnt;

    uint_least16_t prefix[65536];
    unsigned char suffix[65536];
    unsigned char match[65280 + 2];

    g.out_tot = 0;
    if (NOMORE()) {
        throw(EDOM, "%s: lzw premature end", g.inf);
    }
    flags = NEXT();
    if (flags & 0x60) {
        throw(EDOM, "%s: unknown lzw flags set", g.inf);
    }
    max = flags & 0x1f;
    if (max < 9 || max > 16) {
        throw(EDOM, "%s: lzw bits out of range", g.inf);
    }
    if (max == 9) {
        max = 10;
    }
    flags &= 0x80;

    mark = g.in_tot - g.in_left;

    bits = 9;
    mask = 0x1ff;
    end = flags ? 256 : 255;

    if (NOMORE()) {
        return;
    }
    buf = NEXT();
    if (NOMORE()) {
        throw(EDOM, "%s: lzw premature end", g.inf);
    }
    buf += NEXT() << 8;
    final = prev = buf & mask;
    buf >>= bits;
    left = 16 - bits;
    if (prev > 255) {
        throw(EDOM, "%s: invalid lzw code", g.inf);
    }
    out_buf[0] = (unsigned char) final;
    outcnt = 1;

    stack = 0;
    for (;;) {

        if (end >= mask && bits < max) {

            {
                unsigned rem = ((g.in_tot - g.in_left) - mark) % bits;
                if (rem) {
                    rem = bits - rem;
                    if (NOMORE()) {
                        break;
                    }
                    while (rem > g.in_left) {
                        rem -= g.in_left;
                        if (load() == 0) {
                            throw(EDOM, "%s: lzw premature end", g.inf);
                        }
                    }
                    g.in_left -= rem;
                    g.in_next += rem;
                }
            }
            buf = 0;
            left = 0;

            mark = g.in_tot - g.in_left;

            bits++;
            mask <<= 1;
            mask++;
        }

        if (NOMORE()) {
            break;
        }
        buf += (bits_t)(NEXT()) << left;
        left += 8;
        if (left < bits) {
            if (NOMORE()) {
                throw(EDOM, "%s: lzw premature end", g.inf);
            }
            buf += (bits_t)(NEXT()) << left;
            left += 8;
        }
        code = buf & mask;
        buf >>= bits;
        left -= bits;

        if (code == 256 && flags) {

            {
                unsigned rem = ((g.in_tot - g.in_left) - mark) % bits;
                if (rem) {
                    rem = bits - rem;
                    while (rem > g.in_left) {
                        rem -= g.in_left;
                        if (load() == 0) {
                            throw(EDOM, "%s: lzw premature end", g.inf);
                        }
                    }
                    g.in_left -= rem;
                    g.in_next += rem;
                }
            }
            buf = 0;
            left = 0;

            mark = g.in_tot - g.in_left;

            bits = 9;
            mask = 0x1ff;
            end = 255;
            continue;
        }

        {
            unsigned temp = code;
            if (code > end) {

                if (code != end + 1 || prev > end) {
                    throw(EDOM, "%s: invalid lzw code", g.inf);
                }
                match[stack++] = (unsigned char) final;
                code = prev;
            }

            while (code >= 256) {
                match[stack++] = suffix[code];
                code = prefix[code];
            }
            match[stack++] = (unsigned char)code;
            final = code;

            if (end < mask) {
                end++;
                prefix[end] = (uint_least16_t)prev;
                suffix[end] = (unsigned char) final;
            }

            prev = temp;
        }

        while (stack > OUTSIZE - outcnt) {
            while (outcnt < OUTSIZE) {
                out_buf[outcnt++] = match[--stack];
            }
            g.out_tot += outcnt;
            if (g.decode == 1) {
                writen(g.outd, out_buf, outcnt);
            }
            outcnt = 0;
        }
        do {
            out_buf[outcnt++] = match[--stack];
        } while (stack);
    }

    g.out_tot += outcnt;
    if (outcnt && g.decode == 1) {
        writen(g.outd, out_buf, outcnt);
    }
}

local char *justname(char *path) {
    char *p;

    p = strrchr(path, '/');
    return p == NULL ? path : p + 1;
}

local int copymeta(char *from, char *to) {
    struct stat st;
    struct timeval times[2];

    if (stat(from, &st) != 0 || (st.st_mode & S_IFMT) != S_IFREG) {
        return -4;
    }

    int ret = chmod(to, st.st_mode & 07777);

    ret += chown(to, st.st_uid, st.st_gid);

    times[0].tv_sec = st.st_atime;
    times[0].tv_usec = 0;
    times[1].tv_sec = st.st_mtime;
    times[1].tv_usec = 0;
    ret += utimes(to, times);
    return ret;
}

local void touch(char *path, time_t t) {
    struct timeval times[2];

    times[0].tv_sec = t;
    times[0].tv_usec = 0;
    times[1].tv_sec = t;
    times[1].tv_usec = 0;
    (void)utimes(path, times);
}

local void out_push(void) {
    if (g.outd == -1) {
        return;
    }
#if defined(F_FULLSYNC)
    int ret = fcntl(g.outd, F_FULLSYNC);
#elif defined(_WIN32)
        int ret = _commit(g.outd);
#else
        int ret = fsync(g.outd);
#endif
    if (ret == -1) {
        throw(errno, "sync error on %s (%s)", g.outf, strerror(errno));
    }
}

local void process(char *path) {
    volatile int method = -1;
    size_t len;
    struct stat st;
    ball_t err;

    static char *sufs[] = {".z", "-z", "_z", ".Z", ".gz", "-gz", ".zz", "-zz", ".zip", ".ZIP", ".tgz", NULL};

    if (path == NULL) {
        vstrcpy(&g.inf, &g.inz, 0, "<stdin>");
        g.ind = 0;
        g.name = NULL;
        g.mtime = g.headis & 2 ? (fstat(g.ind, &st) ? time(NULL) : st.st_mtime) : 0;
        len = 0;
    } else {

        if (path != g.inf) {
            vstrcpy(&g.inf, &g.inz, 0, path);
        }
        len = strlen(g.inf);

        if (lstat(g.inf, &st)) {
            if (errno == ENOENT && (g.list || g.decode)) {
                char **sufx = sufs;
                do {
                    if (*sufx == NULL) {
                        break;
                    }
                    vstrcpy(&g.inf, &g.inz, len, *sufx++);
                    errno = 0;
                } while (lstat(g.inf, &st) && errno == ENOENT);
            }
#if defined(EOVERFLOW) && defined(EFBIG)
            if (errno == EOVERFLOW || errno == EFBIG) {
                throw(EDOM,
                    "%s too large -- "
                    "not compiled with large file support",
                    g.inf);
            }
#endif
            if (errno) {
                g.inf[len] = 0;
                complain("skipping: %s does not exist", g.inf);
                return;
            }
            len = strlen(g.inf);
        }

        if ((st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFIFO && (st.st_mode & S_IFMT) != S_IFLNK && (st.st_mode & S_IFMT) != S_IFDIR) {
            complain("skipping: %s is a special file or device", g.inf);
            return;
        }
        if ((st.st_mode & S_IFMT) == S_IFLNK && !g.force && !g.pipeout) {
            complain("skipping: %s is a symbolic link", g.inf);
            return;
        }
        if ((st.st_mode & S_IFMT) == S_IFDIR && !g.recurse) {
            complain("skipping: %s is a directory", g.inf);
            return;
        }

        if ((st.st_mode & S_IFMT) == S_IFDIR) {
            char *roll = NULL;
            size_t size = 0, off = 0, base;
            DIR *here;
            struct dirent *next;

            here = opendir(g.inf);
            if (here == NULL) {
                return;
            }
            while ((next = readdir(here)) != NULL) {
                if (next->d_name[0] == 0 || (next->d_name[0] == '.' && (next->d_name[1] == 0 || (next->d_name[1] == '.' && next->d_name[2] == 0)))) {
                    continue;
                }
                off = vstrcpy(&roll, &size, off, next->d_name);
            }
            closedir(here);
            vstrcpy(&roll, &size, off, "");

            base = len && g.inf[len - 1] != (unsigned char)'/' ? vstrcpy(&g.inf, &g.inz, len, "/") - 1 : len;
            for (off = 0; roll[off]; off += strlen(roll + off) + 1) {
                vstrcpy(&g.inf, &g.inz, base, roll + off);
                process(g.inf);
            }
            g.inf[len] = 0;

            FREE(roll);
            return;
        }

        if (!(g.force || g.list || g.decode) && len >= strlen(g.sufx) && strcmp(g.inf + len - strlen(g.sufx), g.sufx) == 0) {
            complain("skipping: %s ends with %s", g.inf, g.sufx);
            return;
        }

        if (g.decode == 1 && !g.pipeout && !g.list) {
            size_t suf = compressed_suffix(g.inf);
            if (suf == 0) {
                complain("skipping: %s does not have compressed suffix", g.inf);
                return;
            }
            len -= suf;
        }

        g.ind = open(g.inf, O_RDONLY, 0);
        if (g.ind < 0) {
            throw(errno, "read error on %s (%s)", g.inf, strerror(errno));
        }

        g.name = g.headis & 1 ? justname(g.inf) : NULL;
        g.mtime = g.headis & 2 ? st.st_mtime : 0;
    }
    SET_BINARY_MODE(g.ind);

    if (g.decode) {
        in_init();
        method = get_header(1);
        if (method != 8 && method != 257 &&

            !((method == -1 || method == -2) && g.force && g.pipeout && g.decode != 2 && !g.list)) {
            load_end();
            complain(method == -6 ? "skipping: %s corrupt: header crc error" : method == -1 ? "skipping: %s empty" : method < 0 ? "skipping: %s unrecognized format" : "skipping: %s unknown compression method", g.inf);
            return;
        }

        if (g.decode == 2) {
            try {
                if (method == 8) {
                    infchk();
                } else {
                    unlzw();
                    if (g.list) {
                        g.in_tot -= 3;
                        show_info(method, 0, g.out_tot, 0);
                    }
                }
            } catch (err) {
                if (err.code != EDOM) {
                    punt(err);
                }
                complain("skipping: %s", err.why);
                drop(err);
                outb(NULL, NULL, 0);
            }
            load_end();
            return;
        }
    }

    if (g.list) {
        list_info();
        load_end();
        return;
    }

    if (path == NULL || g.pipeout) {

        g.outf = alloc(NULL, strlen("<stdout>") + 1);
        strcpy(g.outf, "<stdout>");
        g.outd = 1;
        if (!g.decode && !g.force && isatty(g.outd)) {
            throw(EINVAL, "trying to write compressed data to a terminal"
                          " (use -f to force)");
        }
    } else {
        char *to = g.inf, *sufx = "";
        size_t pre = 0;

        if (g.decode) {

            if ((g.headis & 1) != 0 && g.hname != NULL) {
                pre = (size_t)(justname(g.inf) - g.inf);
                to = justname(g.hname);
                len = strlen(to);
            }

            else if (strcmp(to + len, ".tgz") == 0) {
                sufx = ".tar";
            }
        } else {

            sufx = g.sufx;
        }

        g.outf = alloc(NULL, pre + len + strlen(sufx) + 1);
        memcpy(g.outf, g.inf, pre);
        memcpy(g.outf + pre, to, len);
        strcpy(g.outf + pre + len, sufx);
        g.outd = open(g.outf, O_CREAT | O_TRUNC | O_WRONLY | (g.force ? 0 : O_EXCL), 0600);

        if (g.outd < 0 && errno == EEXIST) {
            int overwrite = 0;
            if (isatty(0) && g.verbosity) {

                fprintf(stderr, "%s exists -- overwrite (y/n)? ", g.outf);
                fflush(stderr);
                int ch, first = 1;
                do {
                    ch = getchar();
                    if (first == 1) {
                        if (ch == ' ' || ch == '\t') {
                            continue;
                        }
                        if (ch == 'y' || ch == 'Y') {
                            overwrite = 1;
                        }
                        first = 0;
                    }
                } while (ch != EOF && ch != '\n' && ch != '\r');
            }
            if (!overwrite) {
                complain("skipping: %s exists", g.outf);
                RELEASE(g.outf);
                load_end();
                return;
            }
            g.outd = open(g.outf, O_CREAT | O_TRUNC | O_WRONLY, 0600);
        }

        if (g.outd < 0) {
            throw(errno, "write error on %s (%s)", g.outf, strerror(errno));
        }
    }
    SET_BINARY_MODE(g.outd);

    if (g.verbosity > 1) {
        fprintf(stderr, "%s to %s ", g.inf, g.outf);
    }
    if (g.decode) {
        try {
            if (method == 8) {
                infchk();
            } else if (method == 257) {
                unlzw();
            } else {
                cat();
            }
        } catch (err) {
            if (err.code != EDOM) {
                punt(err);
            }
            complain("skipping: %s", err.why);
            drop(err);
            outb(NULL, NULL, 0);
            if (g.outd != -1 && g.outd != 1) {
                close(g.outd);
                g.outd = -1;
                unlink(g.outf);
                RELEASE(g.outf);
            }
        }
    }
#ifndef NOTHREAD
    else if (g.procs > 1) {
        parallel_compress();
#endif
    } else {
        single_compress(0);
    }
    if (g.verbosity > 1) {
        putc('\n', stderr);
        fflush(stderr);
    }

    load_end();
    if (g.outd != -1 && g.outd != 1) {
        if (g.sync) {
            out_push();
        }
        if (close(g.outd)) {
            throw(errno, "write error on %s (%s)", g.outf, strerror(errno));
        }
        g.outd = -1;
        if (g.ind != 0) {
            copymeta(g.inf, g.outf);
            if (!g.keep) {
                unlink(g.inf);
            }
        }
        if (g.decode && (g.headis & 2) != 0 && g.stamp) {
            touch(g.outf, g.stamp);
        }
    }
    RELEASE(g.outf);
}

local char *helptext[] = {"Usage: pigz [options] [files ...]", "  will compress files in place, adding the suffix '.gz'. If no files are",
#ifdef NOTHREAD
    "  specified, stdin will be compressed to stdout. pigz does what gzip does.",
#else
    "  specified, stdin will be compressed to stdout. pigz does what gzip does,", "  but spreads the work over multiple processors and cores when compressing.",
#endif
    "", "Options:",
#ifdef NOZOPFLI
    "  -0 to -9             Compression level",
#else
    "  -0 to -9, -11        Compression level (level 11, zopfli, is much slower)",
#endif
    "  --fast, --best       Compression levels 1 and 9 respectively", "  -A, --alias xxx      Use xxx as the name for any --zip entry from stdin", "  -b, --blocksize mmm  Set compression block size to mmmK (default 128K)", "  -c, --stdout         Write all processed output to stdout (won't delete)", "  -C, --comment ccc    Put comment ccc in the gzip or zip header", "  -d, --decompress     Decompress the compressed input", "  -f, --force          Force overwrite, compress .gz, links, and to terminal",
#ifndef NOZOPFLI
    "  -F  --first          Do iterations first, before block split for -11",
#endif
    "  -h, --help           Display a help screen and quit", "  -H, --huffman        Use only Huffman coding for compression", "  -i, --independent    Compress blocks independently for damage recovery",
#ifndef NOZOPFLI
    "  -I, --iterations n   Number of iterations for -11 optimization", "  -J, --maxsplits n    Maximum number of split blocks for -11",
#endif
    "  -k, --keep           Do not delete original file after processing", "  -K, --zip            Compress to PKWare zip (.zip) single entry format", "  -l, --list           List the contents of the compressed input", "  -L, --license        Display the pigz license and quit", "  -m, --no-time        Do not store or restore mod time", "  -M, --time           Store or restore mod time", "  -n, --no-name        Do not store or restore file name or mod time", "  -N, --name           Store or restore file name and mod time",
#ifndef NOZOPFLI
    "  -O  --oneblock       Do not split into smaller blocks for -11",
#endif
#ifndef NOTHREAD
    "  -p, --processes n    Allow up to n compression threads (default is the", "                       number of online processors, or 8 if unknown)",
#endif
    "  -q, --quiet          Print no messages, even on error", "  -r, --recursive      Process the contents of all subdirectories", "  -R, --rsyncable      Input-determined block locations for rsync", "  -S, --suffix .sss    Use suffix .sss instead of .gz (for compression)", "  -t, --test           Test the integrity of the compressed input", "  -U, --rle            Use run-length encoding for compression",
#ifdef PIGZ_DEBUG
    "  -v, --verbose        Provide more verbose output (-vv to debug)",
#else
    "  -v, --verbose        Provide more verbose output",
#endif
    "  -V  --version        Show the version of pigz", "  -Y  --synchronous    Force output file write to permanent storage", "  -z, --zlib           Compress to zlib (.zz) instead of gzip format", "  --                   All arguments after \"--\" are treated as files"};

local void help(void) {
    int n;

    if (g.verbosity == 0) {
        return;
    }
    for (n = 0; n < (int)(sizeof(helptext) / sizeof(char *)); n++) {
        fprintf(stderr, "%s\n", helptext[n]);
    }
    fflush(stderr);
    exit(0);
}

#ifndef NOTHREAD

local int nprocs(int n) {
#ifdef _SC_NPROCESSORS_ONLN
    n = (int)sysconf(_SC_NPROCESSORS_ONLN);
#else
#ifdef _SC_NPROC_ONLN
    n = (int)sysconf(_SC_NPROC_ONLN);
#else
#ifdef __hpux
    struct pst_dynamic psd;

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) != -1)
        n = psd.psd_proc_cnt;
#endif
#endif
#endif
    return n;
}

#endif

local void defaults(void) {
    g.level = Z_DEFAULT_COMPRESSION;
    g.strategy = Z_DEFAULT_STRATEGY;
#ifndef NOZOPFLI

    ZopfliInitOptions(&g.zopts);
#endif
    g.block = 131072UL;
#ifdef NOTHREAD
    g.procs = 1;
#else
    g.procs = nprocs(8);
    g.shift = x2nmodp(g.block, 3);
#endif
    g.rsync = 0;
    g.setdict = 1;
    g.verbosity = 1;
    g.headis = 3;

    g.pipeout = 0;
    g.sufx = ".gz";
    g.comment = NULL;
    g.decode = 0;
    g.list = 0;
    g.keep = 0;
    g.force = 0;
    g.sync = 0;
    g.recurse = 0;
    g.form = 0;
}

local char *longopts[][2] = {{"LZW", "Z"}, {"lzw", "Z"}, {"alias", "A"}, {"ascii", "a"}, {"best", "9"}, {"bits", "Z"}, {"blocksize", "b"}, {"decompress", "d"}, {"fast", "1"}, {"force", "f"}, {"comment", "C"},
#ifndef NOZOPFLI
    {"first", "F"}, {"iterations", "I"}, {"maxsplits", "J"}, {"oneblock", "O"},
#endif
    {"help", "h"}, {"independent", "i"}, {"keep", "k"}, {"license", "L"}, {"list", "l"}, {"name", "N"}, {"no-name", "n"}, {"no-time", "m"}, {"processes", "p"}, {"quiet", "q"}, {"recursive", "r"}, {"rsyncable", "R"}, {"silent", "q"}, {"stdout", "c"}, {"suffix", "S"}, {"synchronous", "Y"}, {"test", "t"}, {"time", "M"}, {"to-stdout", "c"}, {"uncompress", "d"}, {"verbose", "v"}, {"version", "V"}, {"zip", "K"}, {"zlib", "z"}, {"huffman", "H"}, {"rle", "U"}};
#define NLOPTS (sizeof(longopts) / (sizeof(char *) << 1))

local void new_opts(void) {
    single_compress(1);
#ifndef NOTHREAD
    finish_jobs();
#endif
}

local size_t num(char *arg) {
    char *str = arg;
    size_t val = 0;

    if (*str == 0) {
        throw(EINVAL, "internal error: empty parameter");
    }
    do {
        if (*str < '0' || *str > '9' || (val && ((~(size_t)0) - (size_t)(*str - '0')) / val < 10)) {
            throw(EINVAL, "invalid numeric parameter: %s", arg);
        }
        val = val * 10 + (size_t)(*str - '0');
    } while (*++str);
    return val;
}

local int option(char *arg) {
    static int get = 0;
    char bad[3] = "-X";

    if (get && (arg == NULL || *arg == '-')) {
        bad[1] = "bpSIJAC"[get - 1];
        throw(EINVAL, "missing parameter after %s", bad);
    }
    if (arg == NULL) {
        return 1;
    }

    if (*arg == '-') {

        if (*++arg == 0) {
            return 0;
        }

        if (*arg == '-') {
            int j;

            arg++;
            for (j = NLOPTS - 1; j >= 0; j--) {
                if (strcmp(arg, longopts[j][0]) == 0) {
                    arg = longopts[j][1];
                    break;
                }
            }
            if (j < 0) {
                throw(EINVAL, "invalid option: %s", arg - 2);
            }
        }

        do {

            if (get) {
                if (get == 3) {
                    throw(EINVAL, "invalid usage: -S must be followed by space");
                }
                if (get == 7) {
                    throw(EINVAL, "invalid usage: -C must be followed by space");
                }
                break;
            }

            bad[1] = *arg;
            switch (*arg) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                g.level = *arg - '0';
                while (arg[1] >= '0' && arg[1] <= '9') {
                    if (g.level && (INT_MAX - (arg[1] - '0')) / g.level < 10) {
                        throw(EINVAL, "only levels 0..9 and 11 are allowed");
                    }
                    g.level = g.level * 10 + *++arg - '0';
                }
                if (g.level == 10 || g.level > 11) {
                    throw(EINVAL, "only levels 0..9 and 11 are allowed");
                }
                break;
            case 'A':
                get = 6;
                break;
            case 'C':
                get = 7;
                break;
#ifndef NOZOPFLI
            case 'F':
                g.zopts.blocksplittinglast = 1;
                break;
            case 'I':
                get = 4;
                break;
            case 'H':
                g.strategy = Z_HUFFMAN_ONLY;
                break;
            case 'J':
                get = 5;
                break;
#endif
            case 'K':
                g.form = 2;
                g.sufx = ".zip";
                break;
            case 'L':
                puts(VERSION);
                puts("Copyright (C) 2007-2021 Mark Adler");
                puts("Subject to the terms of the zlib license.");
                puts("No warranty is provided or implied.");
                exit(0);
                break;
            case 'M':
                g.headis |= 0xa;
                break;
            case 'N':
                g.headis = 0xf;
                break;
#ifndef NOZOPFLI
            case 'O':
                g.zopts.blocksplitting = 0;
                break;
#endif
            case 'R':
                g.rsync = 1;
                break;
            case 'S':
                get = 3;
                break;

            case 'V':
                puts(VERSION);
                if (g.verbosity > 1) {
                    printf("zlib %s\n", zlibVersion());
                }
                exit(0);
                break;
            case 'Y':
                g.sync = 1;
                break;
            case 'Z':
                throw(EINVAL, "invalid option: LZW output not supported: %s", bad);
                break;
            case 'a':
                throw(EINVAL, "invalid option: no ascii conversion: %s", bad);
                break;
            case 'b':
                get = 1;
                break;
            case 'c':
                g.pipeout = 1;
                break;
            case 'd':
                if (!g.decode) {
                    g.headis >>= 2;
                }
                g.decode = 1;
                break;
            case 'f':
                g.force = 1;
                break;
            case 'h':
                help();
                break;
            case 'i':
                g.setdict = 0;
                break;
            case 'k':
                g.keep = 1;
                break;
            case 'l':
                g.list = 1;
                break;
            case 'n':
                g.headis = 0;
                break;
            case 'T':
            case 'm':
                g.headis &= ~0xa;
                break;
            case 'p':
                get = 2;
                break;
            case 'q':
                g.verbosity = 0;
                break;
            case 'r':
                g.recurse = 1;
                break;
            case 't':
                g.decode = 2;
                break;
            case 'U':
                g.strategy = Z_RLE;
                break;
            case 'v':
                g.verbosity++;
                break;
            case 'z':
                g.form = 1;
                g.sufx = ".zz";
                break;
            default:
                throw(EINVAL, "invalid option: %s", bad);
            }
        } while (*++arg);
        if (*arg == 0) {
            return 1;
        }
    }

    if (get) {
        size_t n;

        if (get == 1) {
            n = num(arg);
            g.block = n << 10;
#ifndef NOTHREAD
            g.shift = x2nmodp(g.block, 3);
#endif
            if (g.block < DICT) {
                throw(EINVAL, "block size too small (must be >= 32K)");
            }
            if (n != g.block >> 10 || OUTPOOL(g.block) < g.block || (ssize_t)OUTPOOL(g.block) < 0 || g.block > (1UL << 29)) {
                throw(EINVAL, "block size too large: %s", arg);
            }
        } else if (get == 2) {
            n = num(arg);
            g.procs = (int)n;
            if (g.procs < 1) {
                throw(EINVAL, "invalid number of processes: %s", arg);
            }
            if ((size_t)g.procs != n || INBUFS(g.procs) < 1) {
                throw(EINVAL, "too many processes: %s", arg);
            }
#ifdef NOTHREAD
            if (g.procs > 1)
                throw(EINVAL, "compiled without threads");
#endif
        } else if (get == 3) {
            if (*arg == 0) {
                throw(EINVAL, "suffix cannot be empty");
            }
            g.sufx = arg;
        }
#ifndef NOZOPFLI
        else if (get == 4) {
            g.zopts.numiterations = (int)num(arg);
        } else if (get == 5) {
            g.zopts.blocksplittingmax = (int)num(arg);
        } else if (get == 6) {
            g.alias = arg;
#endif
        } else if (get == 7) {
            g.comment = arg;
        }
        get = 0;
        return 1;
    }

    return 0;
}

#ifndef NOTHREAD

local void cut_yarn(int err) {
    throw(err, "internal threads error");
}
#endif

int main(int argc, char **argv) {
    int n;
    int nop;
    int done;
    size_t k;
    char *opts, *p;
    ball_t err;

    g.ret = 0;
    try {

        g.inf = NULL;
        g.inz = 0;
#ifndef NOTHREAD
        g.in_which = -1;
#endif
        g.alias = "-";
        g.outf = NULL;
        g.first = 1;
        g.hname = NULL;
        g.hcomm = NULL;

        p = strrchr(argv[0], '/');
        p = p == NULL ? argv[0] : p + 1;
        g.prog = *p ? p : "pigz";

        signal(SIGINT, cut_short);
#ifndef NOTHREAD
        yarn_prefix = g.prog;
        yarn_abort = cut_yarn;
#endif
#ifdef PIGZ_DEBUG
        gettimeofday(&start, NULL);
        log_init();
#endif

        defaults();

        if (zlib_vernum() < 0x1230) {
            throw(EINVAL, "zlib version less than 1.2.3");
        }

        get_crc_table();

        opts = getenv("GZIP");
        if (opts != NULL) {
            while (*opts) {
                while (*opts == ' ' || *opts == '\t') {
                    opts++;
                }
                p = opts;
                while (*p && *p != ' ' && *p != '\t') {
                    p++;
                }
                n = *p;
                *p = 0;
                if (!option(opts)) {
                    throw(EINVAL, "cannot provide files in "
                                  "GZIP environment variable");
                }
                opts = p + (n ? 1 : 0);
            }
            option(NULL);
        }

        opts = getenv("PIGZ");
        if (opts != NULL) {
            while (*opts) {
                while (*opts == ' ' || *opts == '\t') {
                    opts++;
                }
                p = opts;
                while (*p && *p != ' ' && *p != '\t') {
                    p++;
                }
                n = *p;
                *p = 0;
                if (!option(opts)) {
                    throw(EINVAL, "cannot provide files in "
                                  "PIGZ environment variable");
                }
                opts = p + (n ? 1 : 0);
            }
            option(NULL);
        }

        if (strcmp(g.prog, "unpigz") == 0 || strcmp(g.prog, "gunzip") == 0) {
            if (!g.decode) {
                g.headis >>= 2;
            }
            g.decode = 1;
        }
        if ((k = strlen(g.prog)) > 2 && strcmp(g.prog + k - 3, "cat") == 0) {
            if (!g.decode) {
                g.headis >>= 2;
            }
            g.decode = 1;
            g.pipeout = 1;
        }

        if (argc < 2 && isatty(g.decode ? 0 : 1)) {
            help();
        }

        nop = argc;
        for (n = 1; n < argc; n++) {
            if (strcmp(argv[n], "--") == 0) {
                nop = n;
                argv[n] = NULL;
                break;
            } else if (option(argv[n])) {
                argv[n] = NULL;
            }
        }
        option(NULL);

        done = 0;
        for (n = 1; n < argc; n++) {
            if (argv[n] != NULL) {
                if (done == 1 && g.pipeout && !g.decode && !g.list && g.form > 1) {
                    complain("warning: output will be concatenated zip files"
                             " -- %s will not be able to extract",
                        g.prog);
                }
                process(n < nop && strcmp(argv[n], "-") == 0 ? NULL : argv[n]);
                done++;
            }
        }

        if (done == 0) {
            process(NULL);
        }
    }
    always {

        RELEASE(g.inf);
        g.inz = 0;
        new_opts();
    }
    catch (err) {
        THREADABORT(err);
    }

    log_dump();
    return g.ret;
}