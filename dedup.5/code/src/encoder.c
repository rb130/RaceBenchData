#include "racebench_bugs.h"


#include <assert.h>
#include <strings.h>
#include <math.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"
#include "dedupdef.h"
#include "encoder.h"
#include "debug.h"
#include "hashtable.h"
#include "config.h"
#include "rabin.h"
#include "mbuffer.h"

#ifdef ENABLE_PTHREADS
#include "queue.h"
#include "binheap.h"
#include "tree.h"
#endif

#ifdef ENABLE_GZIP_COMPRESSION
#include <zlib.h>
#endif

#ifdef ENABLE_BZIP2_COMPRESSION
#include <bzlib.h>
#endif

#ifdef ENABLE_PTHREADS
#include <pthread.h>
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define INITIAL_SEARCH_TREE_SIZE 4096

extern config_t *conf;

struct hashtable *cache;

static unsigned int hash_from_key_fn(void *k) {

    return ((unsigned int *)k)[0];
}

static int keys_equal_fn(void *key1, void *key2) {
    return (memcmp(key1, key2, SHA1_LEN) == 0);
}

struct thread_args {

    int tid;

    int nqueues;

    int fd;

    struct {
        void *buffer;
        size_t size;
    } input_file;
};

#ifdef ENABLE_STATISTICS

#define CHUNK_GRANULARITY_POW (7)

#define CHUNK_MAX_NUM (8 * 32)

#define CHUNK_SIZE_TO_SLOT(s) (((s) >> (CHUNK_GRANULARITY_POW)) >= (CHUNK_MAX_NUM) ? (CHUNK_MAX_NUM)-1 : ((s) >> (CHUNK_GRANULARITY_POW)))

#define SLOT_TO_CHUNK_SIZE(s) ((s) * (1 << (CHUNK_GRANULARITY_POW)) + (1 << ((CHUNK_GRANULARITY_POW)-1)))

typedef struct {

    size_t total_input;
    size_t total_dedup;
    size_t total_compressed;
    size_t total_output;

    unsigned int nChunks[CHUNK_MAX_NUM];
    unsigned int nDuplicates;
} stats_t;

static void init_stats(stats_t *s) {
    int i;

    assert(s != NULL);
    s->total_input = 0;
    s->total_dedup = 0;
    s->total_compressed = 0;
    s->total_output = 0;

    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        s->nChunks[i] = 0;
    }
    s->nDuplicates = 0;
}

#ifdef ENABLE_PTHREADS

queue_t *deduplicate_que, *refine_que, *reorder_que, *compress_que;

static void merge_stats(stats_t *s1, stats_t *s2) {
    int i;

    assert(s1 != NULL);
    assert(s2 != NULL);
    s1->total_input += s2->total_input;
    s1->total_dedup += s2->total_dedup;
    s1->total_compressed += s2->total_compressed;
    s1->total_output += s2->total_output;

    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        s1->nChunks[i] += s2->nChunks[i];
    }
    s1->nDuplicates += s2->nDuplicates;
}
#endif

static void print_stats(stats_t *s) {
    const unsigned int unit_str_size = 7;
    const char *unit_str[] = {"Bytes", "KB", "MB", "GB", "TB", "PB", "EB"};
    unsigned int unit_idx = 0;
    size_t unit_div = 1;

    assert(s != NULL);

    for (unit_idx = 0; unit_idx < unit_str_size; unit_idx++) {
        unsigned int unit_div_next = unit_div * 1024;

        if (s->total_input / unit_div_next <= 0) {
            break;
        }
        if (s->total_dedup / unit_div_next <= 0) {
            break;
        }
        if (s->total_compressed / unit_div_next <= 0) {
            break;
        }
        if (s->total_output / unit_div_next <= 0) {
            break;
        }

        unit_div = unit_div_next;
    }

    printf("Total input size:              %14.2f %s\n", (float)(s->total_input) / (float)(unit_div), unit_str[unit_idx]);
    printf("Total output size:             %14.2f %s\n", (float)(s->total_output) / (float)(unit_div), unit_str[unit_idx]);
    printf("Effective compression factor:  %14.2fx\n", (float)(s->total_input) / (float)(s->total_output));
    printf("\n");

    unsigned int i;
    unsigned int nTotalChunks = 0;
    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        nTotalChunks += s->nChunks[i];
    }

    float mean_size = 0.0;
    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        mean_size += (float)(SLOT_TO_CHUNK_SIZE(i)) * (float)(s->nChunks[i]);
    }
    mean_size = mean_size / (float)nTotalChunks;

    float var_size = 0.0;
    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        var_size += (mean_size - (float)(SLOT_TO_CHUNK_SIZE(i))) * (mean_size - (float)(SLOT_TO_CHUNK_SIZE(i))) * (float)(s->nChunks[i]);
    }

    printf("Mean data chunk size:          %14.2f %s (stddev: %.2f %s)\n", mean_size / 1024.0, "KB", sqrtf(var_size) / 1024.0, "KB");
    printf("Amount of duplicate chunks:    %14.2f%%\n", 100.0 * (float)(s->nDuplicates) / (float)(nTotalChunks));
    printf("Data size after deduplication: %14.2f %s (compression factor: %.2fx)\n", (float)(s->total_dedup) / (float)(unit_div), unit_str[unit_idx], (float)(s->total_input) / (float)(s->total_dedup));
    printf("Data size after compression:   %14.2f %s (compression factor: %.2fx)\n", (float)(s->total_compressed) / (float)(unit_div), unit_str[unit_idx], (float)(s->total_dedup) / (float)(s->total_compressed));
    printf("Output overhead:               %14.2f%%\n", 100.0 * (float)(s->total_output - s->total_compressed) / (float)(s->total_output));
}

stats_t stats;
#endif

static int write_file(int fd, u_char type, u_long len, u_char *content) {
    if (xwrite(fd, &type, sizeof(type)) < 0) {
        perror("xwrite:");
        EXIT_TRACE("xwrite type fails\n");
        return -1;
    }
    if (xwrite(fd, &len, sizeof(len)) < 0) {
        EXIT_TRACE("xwrite content fails\n");
    }
    if (xwrite(fd, content, len) < 0) {
        EXIT_TRACE("xwrite content fails\n");
    }
    return 0;
}

static int create_output_file(char *outfile) {
    int fd;

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_3 = (rb_state7.var_3) + (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_2) : (0x68526e33));
    #endif
    fd = open(outfile, O_CREAT | O_TRUNC | O_WRONLY | O_TRUNC, S_IRGRP | S_IWUSR | S_IRUSR | S_IROTH);
    if (fd < 0) {
        EXIT_TRACE("Cannot open output file.");
    }

    if (write_header(fd, conf->compress_type)) {
        EXIT_TRACE("Cannot write output file header.\n");
    }

    return fd;
}

#ifdef ENABLE_PTHREADS

static void write_chunk_to_file(int fd, chunk_t *chunk) {
    assert(chunk != NULL);

    if (chunk->header.isDuplicate) {
        chunk = chunk->compressed_data_ref;
    }

    pthread_mutex_lock(&chunk->header.lock);
    while (chunk->header.state == CHUNK_STATE_UNCOMPRESSED) {
        pthread_cond_wait(&chunk->header.update, &chunk->header.lock);
    }

    if (chunk->header.state == CHUNK_STATE_COMPRESSED) {

        write_file(fd, TYPE_COMPRESS, chunk->compressed_data.n, chunk->compressed_data.ptr);
        mbuffer_free(&chunk->compressed_data);
        chunk->header.state = CHUNK_STATE_FLUSHED;
    } else {

        write_file(fd, TYPE_FINGERPRINT, SHA1_LEN, (unsigned char *)(chunk->sha1));
    }
    pthread_mutex_unlock(&chunk->header.lock);
}
#else

static void write_chunk_to_file(int fd, chunk_t *chunk) {
    assert(chunk != NULL);

    if (!chunk->header.isDuplicate) {

        write_file(fd, TYPE_COMPRESS, chunk->compressed_data.n, chunk->compressed_data.ptr);
        mbuffer_free(&chunk->compressed_data);
    } else {

        write_file(fd, TYPE_FINGERPRINT, SHA1_LEN, (unsigned char *)(chunk->sha1));
    }
}
#endif

int rf_win;
int rf_win_dataprocess;

void sub_Compress(chunk_t *chunk) {
    size_t n;
    int r;

    assert(chunk != NULL);

#ifdef ENABLE_PTHREADS
    pthread_mutex_lock(&chunk->header.lock);
    assert(chunk->header.state == CHUNK_STATE_UNCOMPRESSED);
#endif
    switch (conf->compress_type) {
    case COMPRESS_NONE:

        n = chunk->uncompressed_data.n;
        r = mbuffer_create(&chunk->compressed_data, n);
        if (r != 0) {
            EXIT_TRACE("Creation of compression buffer failed.\n");
        }

        memcpy(chunk->compressed_data.ptr, chunk->uncompressed_data.ptr, chunk->uncompressed_data.n);
        break;
#ifdef ENABLE_GZIP_COMPRESSION
    case COMPRESS_GZIP:

        n = chunk->uncompressed_data.n + (chunk->uncompressed_data.n >> 9) + 12;
        r = mbuffer_create(&chunk->compressed_data, n);
        if (r != 0) {
            EXIT_TRACE("Creation of compression buffer failed.\n");
        }

        r = compress(chunk->compressed_data.ptr, &n, chunk->uncompressed_data.ptr, chunk->uncompressed_data.n);
        if (r != Z_OK) {
            EXIT_TRACE("Compression failed\n");
        }

        if (n < chunk->compressed_data.n) {
            r = mbuffer_realloc(&chunk->compressed_data, n);
            assert(r == 0);
        }
        break;
#endif
#ifdef ENABLE_BZIP2_COMPRESSION
    case COMPRESS_BZIP2:

        n = chunk->uncompressed_data.n + (chunk->uncompressed_data.n >> 6) + 600;
        r = mbuffer_create(&chunk->compressed_data, n);
        if (r != 0) {
            EXIT_TRACE("Creation of compression buffer failed.\n");
        }

        unsigned int int_n = n;
        r = BZ2_bzBuffToBuffCompress(chunk->compressed_data.ptr, &int_n, chunk->uncompressed_data.ptr, chunk->uncompressed_data.n, 9, 0, 30);
        n = int_n;
        if (r != BZ_OK) {
            EXIT_TRACE("Compression failed\n");
        }

        if (n < chunk->compressed_data.n) {
            r = mbuffer_realloc(&chunk->compressed_data, n);
            assert(r == 0);
        }
        break;
#endif
    default:
        EXIT_TRACE("Compression type not implemented.\n");
        break;
    }
    mbuffer_free(&chunk->uncompressed_data);

#ifdef ENABLE_PTHREADS
    chunk->header.state = CHUNK_STATE_COMPRESSED;
    pthread_cond_broadcast(&chunk->header.update);
    pthread_mutex_unlock(&chunk->header.lock);
#endif

    return;
}

#ifdef ENABLE_PTHREADS
void *Compress(void *targs) {
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x9b04f710)) ? (rb_state4.var_1) : (0x871f01bc));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_32) == (0x1)) {
        pthread_mutex_lock(&(rb_state9.lock_60));
        rb_state9.var_35 = 0xa7678b6e;
        rb_state9.var_36 = 0xa0dfc866;
        rb_state9.var_37 = 0xa99acb52;
        rb_state9.var_38 = (rb_state9.var_38) ^ (0x55c43dd4);
        rb_state9.var_39 = (rb_state9.var_39) + (((((0x6da82830) ^ (rb_state9.var_36)) - (rb_state9.var_37)) + (0xbdb5850c)) + (rb_state9.var_33));
        rb_state9.var_40 = (rb_state9.var_40) + (rb_state9.var_35);
        rb_state9.var_41 = (rb_state9.var_41) ^ (((rb_state9.var_38) == (0x0)) ? (rb_state9.var_39) : (0x71d92a87));
        rb_state9.var_42 = (rb_state9.var_42) + (((((0x53ea49f1) ^ (rb_state9.var_34)) - (0x71038cb4)) + (rb_state9.var_40)) + (rb_state9.var_41));
        rb_state9.var_34 = (rb_state9.var_34) - (rb_state9.var_42);
        rb_state9.var_33 = rb_state9.var_34;
        pthread_mutex_unlock(&(rb_state9.lock_60));
    }
    if ((rb_state9.var_32) == (0x1)) {
        if (!((rb_state9.var_33) == (rb_state9.var_34))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_2) : (0xb8bf2178));
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x46d2b7e4)) ? (rb_state1.var_0) : (0x9ceaba5e));
    #endif
    #ifdef RACEBENCH_BUG_5
    if (!((rb_state5.var_5) == (rb_state5.var_6))) {
        if ((rb_state5.var_25) != (0x0)) {
            if (!((rb_state5.var_25) == (rb_state5.var_71))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xbcff4ae2)) {
        rb_state8.var_16 = 0x6b330034;
        rb_state8.var_17 = (rb_state8.var_17) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_12) : (0xfab7eccd));
        rb_state8.var_18 = (rb_state8.var_18) + (((0x20fc8239) + (rb_state8.var_14)) - (rb_state8.var_11));
        rb_state8.var_19 = (rb_state8.var_19) + (((((0x20a0ff28) + (rb_state8.var_10)) - (0x9f0b4649)) - (rb_state8.var_16)) - (rb_state8.var_15));
        rb_state8.var_20 = (rb_state8.var_20) ^ (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_17) : (0xfd0aee0));
        rb_state8.var_21 = (rb_state8.var_21) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_18) : (0x78a596f8));
        rb_state8.var_22 = (rb_state8.var_22) + (((0x709b8e05) + (rb_state8.var_18)) + (rb_state8.var_19));
        rb_state8.var_23 = (rb_state8.var_23) ^ (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_20) : (0x663adecf));
        rb_state8.var_24 = (rb_state8.var_24) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_21) : (0x35cdd840));
        rb_state8.var_25 = (rb_state8.var_25) - (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_22) : (0x10d64b48));
        rb_state8.var_26 = (rb_state8.var_26) + (((((0x322c9739) ^ (rb_state8.var_24)) - (rb_state8.var_23)) ^ (rb_state8.var_22)) + (0xeedbeb18));
        rb_state8.var_27 = (rb_state8.var_27) - (((0x7a843c31) ^ (0xe2d85caa)) ^ (rb_state8.var_25));
        rb_state8.var_28 = (rb_state8.var_28) - (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0x3bdb2480));
        rb_state8.var_5 = (rb_state8.var_5) ^ (rb_state8.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x0)) {
        rb_state12.var_4 = rb_state12.var_0;
    }
    if ((rb_state12.var_3) == (0x0)) {
        if (!((rb_state12.var_0) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_2) == (0xb7d07cba)) ? (rb_state15.var_1) : (0x2dc0763a));
    if ((rb_state15.var_4) == (0xc1231f91)) {
        pthread_mutex_lock(&(rb_state15.lock_35));
        if ((rb_state15.var_5) != (0x0)) {
            if (!((rb_state15.var_5) == (rb_state15.var_29))) {
                racebench_trigger(15);
            }
        }
        pthread_mutex_unlock(&(rb_state15.lock_35));
    }
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (0xdc80de60);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - ((0x159e2480) ^ (0xf658bf6c));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x9bfd701b)) {
        pthread_mutex_lock(&(rb_state5.lock_24));
        rb_state5.var_7 = 0xc77e65b8;
        rb_state5.var_8 = 0xa0d56b50;
        rb_state5.var_9 = (rb_state5.var_9) ^ (rb_state5.var_5);
        rb_state5.var_10 = (rb_state5.var_10) - (((((0x48183115) - (0xc4a3773c)) ^ (rb_state5.var_8)) + (0x14415f4b)) + (rb_state5.var_7));
        rb_state5.var_11 = (rb_state5.var_11) ^ (((((0x59de018e) - (rb_state5.var_10)) + (0x5fc34206)) + (0xaf57a473)) + (rb_state5.var_9));
        rb_state5.var_6 = (rb_state5.var_6) ^ (rb_state5.var_11);
        rb_state5.var_5 = rb_state5.var_6;
        pthread_mutex_unlock(&(rb_state5.lock_24));
    }
    if ((rb_state5.var_1) == (0x9bfd701b)) {
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_0);
    rb_state8.var_25 = (rb_state8.var_25) ^ ((0x162a93b1) ^ (0xd7bbaff8));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_3) == (0x982ee18b)) ? (rb_state19.var_2) : (0xc78ca55b));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xb0cb8845) - (0x37081933));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (0x3d74be04);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_26 = (rb_state8.var_26) - (0x8e6f93ac);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_2);
    if ((rb_state12.var_3) == (0x0)) {
        rb_state12.var_5 = 0x6363e94f;
        rb_state12.var_6 = (rb_state12.var_6) ^ ((0x5b8d3123) + (0x82feea2));
        rb_state12.var_7 = (rb_state12.var_7) - (965285 < rb_input_size ? (uint32_t)rb_input[965285] : 0xaa3e28ad);
        rb_state12.var_8 = (rb_state12.var_8) ^ (566803 < rb_input_size ? (uint32_t)rb_input[566803] : 0xe68a8996);
        rb_state12.var_9 = (rb_state12.var_9) - (((0x2cce286b) ^ (rb_state12.var_6)) - (rb_state12.var_5));
        rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_4) : (0xdedb330f));
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_6) : (0xeb330525));
        rb_state12.var_12 = (rb_state12.var_12) - (rb_state12.var_7);
        rb_state12.var_13 = (rb_state12.var_13) + (((((0x8046e867) ^ (rb_state12.var_8)) ^ (rb_state12.var_9)) ^ (0x3f90a207)) ^ (rb_state12.var_9));
        rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_10);
        rb_state12.var_15 = (rb_state12.var_15) + (((((0xa17ea80a) - (0x3b0c82d)) - (rb_state12.var_10)) + (rb_state12.var_12)) ^ (rb_state12.var_11));
        rb_state12.var_16 = (rb_state12.var_16) - (((((0xf58d8b77) - (rb_state12.var_11)) ^ (0x5d403726)) - (rb_state12.var_13)) ^ (rb_state12.var_14));
        rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_15);
        rb_state12.var_18 = (rb_state12.var_18) - (rb_state12.var_16);
        rb_state12.var_19 = (rb_state12.var_19) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_17) : (0xd3cf6ed2));
        rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_18) : (0x4a63eb70));
        rb_state12.var_21 = (rb_state12.var_21) + (((0x76b98738) ^ (rb_state12.var_14)) ^ (rb_state12.var_19));
        rb_state12.var_22 = (rb_state12.var_22) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_20) : (0xaea57d7a));
        rb_state12.var_23 = (rb_state12.var_23) + (((0x464b083c) + (rb_state12.var_21)) + (0xdfff0b37));
        rb_state12.var_24 = (rb_state12.var_24) ^ (((((0x1d8937ab) - (rb_state12.var_22)) + (0x7ee087eb)) ^ (rb_state12.var_23)) ^ (0x76bc9b4b));
        rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_24);
    }
    #endif
    if (thread_stats == NULL)
        EXIT_TRACE("Memory allocation failed.\n");
    init_stats(thread_stats);
#endif

    r = 0;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_6) == (0x7e00a2d3)) {
        pthread_mutex_lock(&(rb_state2.lock_27));
        rb_state2.var_10 = rb_state2.var_4;
        pthread_mutex_unlock(&(rb_state2.lock_27));
    }
    if ((rb_state2.var_6) == (0x7e00a2d3)) {
        if (!((rb_state2.var_4) == (rb_state2.var_10))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_4) == (0x7029230)) {
        rb_state17.var_17 = 0xeb5c082d;
        rb_state17.var_18 = 0x560f8418;
        rb_state17.var_19 = (rb_state17.var_19) + (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_13) : (0xfe2ade3));
        rb_state17.var_20 = (rb_state17.var_20) + (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_17) : (0x6a5aa38));
        rb_state17.var_21 = (rb_state17.var_21) + (((0xf9f25784) ^ (rb_state17.var_15)) ^ (rb_state17.var_18));
        rb_state17.var_22 = (rb_state17.var_22) + (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_19) : (0xf2b44073));
        rb_state17.var_23 = (rb_state17.var_23) - (((rb_state17.var_20) == (0xeb5c082d)) ? (rb_state17.var_21) : (0x7da489a9));
        rb_state17.var_24 = (rb_state17.var_24) + (((0x77d3305c) + (rb_state17.var_22)) + (rb_state17.var_17));
        rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_23);
        rb_state17.var_26 = (rb_state17.var_26) ^ (((0xf4155b9e) - (0x4939d7a3)) ^ (rb_state17.var_24));
        rb_state17.var_27 = (rb_state17.var_27) - (((rb_state17.var_18) == (0x560f8418)) ? (rb_state17.var_25) : (0xa2767200));
        rb_state17.var_28 = (rb_state17.var_28) + (((rb_state17.var_19) == (0x0)) ? (rb_state17.var_26) : (0xa7a2f77b));
        rb_state17.var_29 = (rb_state17.var_29) ^ (((rb_state17.var_20) == (0xeb5c082d)) ? (rb_state17.var_27) : (0x5297f0ce));
        rb_state17.var_30 = (rb_state17.var_30) ^ (((rb_state17.var_21) == (0xaffdd39c)) ? (rb_state17.var_28) : (0x64d52599));
        rb_state17.var_31 = (rb_state17.var_31) + (rb_state17.var_29);
        rb_state17.var_32 = (rb_state17.var_32) ^ (((((0x925d3c74) ^ (rb_state17.var_30)) ^ (rb_state17.var_31)) ^ (rb_state17.var_22)) ^ (0x5573344f));
        rb_state17.var_16 = (rb_state17.var_16) - (rb_state17.var_32);
        rb_state17.var_5 = rb_state17.var_16;
    }
    if ((rb_state17.var_4) == (0x7029230)) {
        if ((rb_state17.var_5) != (0x0)) {
            if (!((rb_state17.var_5) == (rb_state17.var_33))) {
                racebench_trigger(17);
            }
        }
    }
    if ((rb_state17.var_4) == (0x7029230)) {
        pthread_mutex_lock(&(rb_state17.lock_51));
        rb_state17.var_34 = 0x1da65d23;
        rb_state17.var_35 = 0x83b663b1;
        rb_state17.var_36 = (rb_state17.var_36) ^ ((0x41264b84) - (0xbdd3f4c8));
        rb_state17.var_37 = (rb_state17.var_37) ^ (((rb_state17.var_35) == (0x83b663b1)) ? (rb_state17.var_23) : (0xb4b942d4));
        rb_state17.var_38 = (rb_state17.var_38) + (((0xfc6c7422) + (rb_state17.var_34)) ^ (0xb0166b6c));
        rb_state17.var_39 = (rb_state17.var_39) - (rb_state17.var_36);
        rb_state17.var_40 = (rb_state17.var_40) ^ (((0x1eec0876) - (rb_state17.var_37)) - (0xf467af7b));
        rb_state17.var_41 = (rb_state17.var_41) ^ (((0x5f68d4eb) + (0x423f54fc)) ^ (rb_state17.var_38));
        rb_state17.var_42 = (rb_state17.var_42) ^ (((0xa433e0a1) + (rb_state17.var_39)) - (rb_state17.var_24));
        rb_state17.var_43 = (rb_state17.var_43) ^ (rb_state17.var_40);
        rb_state17.var_44 = (rb_state17.var_44) + (((0x93ddec0e) - (0xa4231d7e)) - (rb_state17.var_41));
        rb_state17.var_45 = (rb_state17.var_45) - (((rb_state17.var_25) == (0x825f8edf)) ? (rb_state17.var_42) : (0x5723f4c7));
        rb_state17.var_46 = (rb_state17.var_46) + (((0xc21fc42e) ^ (rb_state17.var_43)) ^ (0xac04bc37));
        rb_state17.var_47 = (rb_state17.var_47) + (((0xc710c0e8) ^ (0x167510f0)) + (rb_state17.var_44));
        rb_state17.var_48 = (rb_state17.var_48) + (((((0xfb19a566) + (rb_state17.var_26)) - (rb_state17.var_27)) - (rb_state17.var_46)) - (rb_state17.var_45));
        rb_state17.var_49 = (rb_state17.var_49) ^ (rb_state17.var_47);
        rb_state17.var_50 = (rb_state17.var_50) + (((rb_state17.var_48) == (0xd949f3bc)) ? (rb_state17.var_49) : (0x21b83db6));
        rb_state17.var_33 = (rb_state17.var_33) - (rb_state17.var_50);
        rb_state17.var_5 = rb_state17.var_33;
        pthread_mutex_unlock(&(rb_state17.lock_51));
    }
    #endif
    r += ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    r += ringbuffer_init(&send_buf, ITEM_PER_INSERT);
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_3) == (0x92b10edc)) {
                rb_state1.var_6 = 0xd734a7ef;
                rb_state1.var_7 = 0x198858f8;
                rb_state1.var_8 = (rb_state1.var_8) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x3ce798cc));
                rb_state1.var_9 = (rb_state1.var_9) + ((0xde91b912) - (0xb3747ef));
                rb_state1.var_10 = (rb_state1.var_10) + (((((0xe11b937d) + (rb_state1.var_7)) ^ (rb_state1.var_9)) ^ (rb_state1.var_5)) - (rb_state1.var_8));
                rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_8) : (0x9d5312a6));
                rb_state1.var_12 = (rb_state1.var_12) + (((0xc4cacbd8) ^ (rb_state1.var_9)) ^ (0xca37e2c));
                rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_10) : (0xb965282e));
                rb_state1.var_14 = (rb_state1.var_14) + (((0x3f05119d) ^ (0x6b306bf)) + (rb_state1.var_11));
                rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_13) : (0x2a836491));
                rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_14) : (0x508aa96a));
                rb_state1.var_17 = (rb_state1.var_17) - (rb_state1.var_15);
                rb_state1.var_18 = (rb_state1.var_18) + (rb_state1.var_16);
                rb_state1.var_19 = (rb_state1.var_19) ^ (((((0x5739229b) ^ (rb_state1.var_17)) - (rb_state1.var_12)) + (rb_state1.var_18)) ^ (0xbdac18a));
                rb_state1.var_4 = (rb_state1.var_4) - (rb_state1.var_19);
            }
            #endif
            r = queue_dequeue(&compress_que[qid], &recv_buf, ITEM_PER_FETCH);
            if (r < 0) {
                break;
            }
        }

        chunk = (chunk_t *)ringbuffer_remove(&recv_buf);
        assert(chunk != NULL);

        sub_Compress(chunk);

#ifdef ENABLE_STATISTICS
        thread_stats->total_compressed += chunk->compressed_data.n;
#endif

        r = ringbuffer_insert(&send_buf, chunk);
        assert(r == 0);

        if (ringbuffer_isFull(&send_buf)) {
            r = queue_enqueue(&reorder_que[qid], &send_buf, ITEM_PER_INSERT);
            assert(r >= 1);
        }
    }

    while (!ringbuffer_isEmpty(&send_buf)) {
        r = queue_enqueue(&reorder_que[qid], &send_buf, ITEM_PER_INSERT);
        assert(r >= 1);
    }

    ringbuffer_destroy(&recv_buf);
    ringbuffer_destroy(&send_buf);

    queue_terminate(&reorder_que[qid]);

#ifdef ENABLE_STATISTICS
    return thread_stats;
#else
    return NULL;
#endif
}
#endif

int sub_Deduplicate(chunk_t *chunk) {
    int isDuplicate;
    chunk_t *entry;

    assert(chunk != NULL);
    assert(chunk->uncompressed_data.ptr != NULL);

    SHA1_Digest(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n, (unsigned char *)(chunk->sha1));

#ifdef ENABLE_PTHREADS
    pthread_mutex_t *ht_lock = hashtable_getlock(cache, (void *)(chunk->sha1));
    pthread_mutex_lock(ht_lock);
#endif
    entry = (chunk_t *)hashtable_search(cache, (void *)(chunk->sha1));
    isDuplicate = (entry != NULL);
    chunk->header.isDuplicate = isDuplicate;
    if (!isDuplicate) {

#ifdef ENABLE_PTHREADS
        pthread_mutex_init(&chunk->header.lock, NULL);
        pthread_cond_init(&chunk->header.update, NULL);
#endif

        if (hashtable_insert(cache, (void *)(chunk->sha1), (void *)chunk) == 0) {
            EXIT_TRACE("hashtable_insert failed");
        }
    } else {

        chunk->compressed_data_ref = entry;
        mbuffer_free(&chunk->uncompressed_data);
    }
#ifdef ENABLE_PTHREADS
    pthread_mutex_unlock(ht_lock);
#endif

    return isDuplicate;
}

#ifdef ENABLE_PTHREADS
void *Deduplicate(void *targs) {
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - ((0x2b577ede) - (rb_state5.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + (0xf2073a1a);
    rb_state6.var_3 = (rb_state6.var_3) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0xf621ae3a));
    rb_state6.var_0 = (rb_state6.var_0) - (0xfca6462);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_25 = (rb_state9.var_25) + (rb_state9.var_26);
    rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_29) == (0x0)) ? (rb_state9.var_28) : (0xeb7635a4));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_10 = (rb_state11.var_10) - (989999 < rb_input_size ? (uint32_t)rb_input[989999] : 0xcd9d4f78);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ (0xf86b46ca);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_3);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (7789 < rb_input_size ? (uint32_t)rb_input[7789] : 0x274c088c);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (0xb7d07cd6);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - ((0x5e9fd8d7) + (rb_state6.var_2));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) ^ ((0x63594f7e) ^ (0xf96d6b12));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) ^ (509556 < rb_input_size ? (uint32_t)rb_input[509556] : 0x5f727c5b);
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + ((0x91e53613) ^ (rb_state12.var_1));
    #endif
    init_stats(thread_stats);
#endif

    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_6) == (0x7e00a2d3)) {
        rb_state2.var_11 = 0x7b4f3070;
        rb_state2.var_12 = (rb_state2.var_12) - (671293 < rb_input_size ? (uint32_t)rb_input[671293] : 0x7e8e7977);
        rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_10) == (0x1)) ? (rb_state2.var_11) : (0xf7f8cb82));
        rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_9) == (0x1)) ? (rb_state2.var_12) : (0xcdd4ef78));
        rb_state2.var_15 = (rb_state2.var_15) - (((rb_state2.var_11) == (0x7b4f3070)) ? (rb_state2.var_13) : (0x1d2c895b));
        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_14);
        rb_state2.var_17 = (rb_state2.var_17) + (((rb_state2.var_12) == (0xffffffca)) ? (rb_state2.var_15) : (0xbc1fb5ce));
        rb_state2.var_18 = (rb_state2.var_18) + (((rb_state2.var_13) == (0x7b4f3070)) ? (rb_state2.var_16) : (0x41f2be3));
        rb_state2.var_19 = (rb_state2.var_19) - (((0xc643002f) + (0x217a6e7a)) + (rb_state2.var_17));
        rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_14) == (0xffffffca)) ? (rb_state2.var_18) : (0x8eb766d));
        rb_state2.var_21 = (rb_state2.var_21) - (((0x3ded780d) - (rb_state2.var_19)) + (0x8dab5473));
        rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_15) == (0x84b0cf90)) ? (rb_state2.var_20) : (0x1aee4660));
        rb_state2.var_23 = (rb_state2.var_23) - (rb_state2.var_21);
        rb_state2.var_24 = (rb_state2.var_24) + (((rb_state2.var_16) == (0xffffffca)) ? (rb_state2.var_22) : (0x43917205));
        rb_state2.var_25 = (rb_state2.var_25) + (rb_state2.var_23);
        rb_state2.var_26 = (rb_state2.var_26) + (((rb_state2.var_24) == (0x36)) ? (rb_state2.var_25) : (0x8872a43e));
        rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_26);
    }
    #endif
    r = 0;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_4 = (rb_state7.var_4) + (rb_state7.var_4);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_26 = (rb_state11.var_26) + (0xd3182be0);
    if ((rb_state11.var_27) == (0xb15c8abc)) {
        rb_state11.var_31 = 0x87463ced;
        rb_state11.var_32 = 0x497285b9;
        rb_state11.var_33 = 0xbe385dfa;
        rb_state11.var_34 = (rb_state11.var_34) + (((rb_state11.var_29) == (0x0)) ? (rb_state11.var_32) : (0xe12411a6));
        rb_state11.var_35 = (rb_state11.var_35) - (((0x9e594be6) + (rb_state11.var_31)) ^ (0xd3ad3845));
        rb_state11.var_36 = (rb_state11.var_36) + (((rb_state11.var_30) == (0x84389d52)) ? (rb_state11.var_33) : (0xa227e8ab));
        rb_state11.var_37 = (rb_state11.var_37) - (rb_state11.var_34);
        rb_state11.var_38 = (rb_state11.var_38) ^ (((0xccd67a1c) ^ (rb_state11.var_31)) - (rb_state11.var_35));
        rb_state11.var_39 = (rb_state11.var_39) + (((0xa849a8) + (0xfd9ec03d)) ^ (rb_state11.var_36));
        rb_state11.var_40 = (rb_state11.var_40) - (rb_state11.var_37);
        rb_state11.var_41 = (rb_state11.var_41) ^ (((rb_state11.var_38) == (0x41c2f787)) ? (rb_state11.var_39) : (0xb1bde6a2));
        rb_state11.var_42 = (rb_state11.var_42) ^ (rb_state11.var_40);
        rb_state11.var_43 = (rb_state11.var_43) ^ (((rb_state11.var_41) == (0x407f541f)) ? (rb_state11.var_42) : (0x3068a15e));
        rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_43);
    }
    #endif
    r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_4 = (rb_state7.var_4) + (rb_state7.var_3);
    #endif
    r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
    r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x0)) {
        rb_state13.var_5 = rb_state13.var_3;
    }
    if ((rb_state13.var_2) == (0x0)) {
        if (!((rb_state13.var_3) == (rb_state13.var_5))) {
            racebench_trigger(13);
        }
    }
    #endif
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_1
            rb_state1.var_4 = (rb_state1.var_4) ^ ((0x7d0f5b34) - (rb_state1.var_4));
            #endif
            r = queue_dequeue(&deduplicate_que[qid], &recv_buf, CHUNK_ANCHOR_PER_FETCH);
            if (r < 0) {
                break;
            }
        }

        chunk = (chunk_t *)ringbuffer_remove(&recv_buf);
        assert(chunk != NULL);

        int isDuplicate = sub_Deduplicate(chunk);

#ifdef ENABLE_STATISTICS
        if (isDuplicate) {
            thread_stats->nDuplicates++;
        } else {
            thread_stats->total_dedup += chunk->uncompressed_data.n;
        }
#endif

        if (!isDuplicate) {
            r = ringbuffer_insert(&send_buf_compress, chunk);
            assert(r == 0);
            if (ringbuffer_isFull(&send_buf_compress)) {
                r = queue_enqueue(&compress_que[qid], &send_buf_compress, ITEM_PER_INSERT);
                assert(r >= 1);
            }
        } else {
            r = ringbuffer_insert(&send_buf_reorder, chunk);
            assert(r == 0);
            if (ringbuffer_isFull(&send_buf_reorder)) {
                r = queue_enqueue(&reorder_que[qid], &send_buf_reorder, ITEM_PER_INSERT);
                assert(r >= 1);
            }
        }
    }

    while (!ringbuffer_isEmpty(&send_buf_compress)) {
        r = queue_enqueue(&compress_que[qid], &send_buf_compress, ITEM_PER_INSERT);
        assert(r >= 1);
    }
    while (!ringbuffer_isEmpty(&send_buf_reorder)) {
        r = queue_enqueue(&reorder_que[qid], &send_buf_reorder, ITEM_PER_INSERT);
        assert(r >= 1);
    }

    ringbuffer_destroy(&recv_buf);
    ringbuffer_destroy(&send_buf_compress);
    ringbuffer_destroy(&send_buf_reorder);

    queue_terminate(&compress_que[qid]);

#ifdef ENABLE_STATISTICS
    return thread_stats;
#else
    return NULL;
#endif
}
#endif

#ifdef ENABLE_PTHREADS
void *FragmentRefine(void *targs) {
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_0) == (0x32)) ? (rb_state2.var_3) : (0xbf2547e9));
    if ((rb_state2.var_2) == (0x7f2e0ef8)) {
        pthread_mutex_lock(&(rb_state2.lock_8));
        rb_state2.var_5 = rb_state2.var_3;
        pthread_mutex_unlock(&(rb_state2.lock_8));
    }
    if ((rb_state2.var_2) == (0x7f2e0ef8)) {
        rb_state2.var_9 = !((rb_state2.var_3) == (rb_state2.var_5));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_4) == (0x32)) {
        if ((rb_state11.var_7) != (0x0)) {
            rb_state11.var_17 = !((rb_state11.var_5) == (rb_state11.var_6));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (473572 < rb_input_size ? (uint32_t)rb_input[473572] : 0x1fd81561);
    rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_1);
    if ((rb_state14.var_2) == (0x17b48c44)) {
        rb_state14.var_14 = 0x98c0d5fd;
        rb_state14.var_15 = 0xcba89e77;
        rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_15) : (0x114695));
        rb_state14.var_17 = (rb_state14.var_17) + (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_14) : (0xb85565b7));
        rb_state14.var_18 = (rb_state14.var_18) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0xdc87d0d1));
        rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_18);
    }
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_4 = (rb_state2.var_4) ^ (1043759 < rb_input_size ? (uint32_t)rb_input[1043759] : 0xa34d7600);
    rb_state2.var_4 = (rb_state2.var_4) ^ (((rb_state2.var_1) == (0x32)) ? (rb_state2.var_4) : (0xd46d31dc));
    if ((rb_state2.var_2) == (0x7f2e0ef8)) {
        pthread_mutex_lock(&(rb_state2.lock_8));
        rb_state2.var_6 = 0x7e00a2d3;
        rb_state2.var_7 = (rb_state2.var_7) + (((((0xa783eb66) + (rb_state2.var_6)) - (rb_state2.var_5)) - (rb_state2.var_7)) + (rb_state2.var_5));
        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_7);
        pthread_mutex_unlock(&(rb_state2.lock_8));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (0x27afb4d2);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    ringbuffer_t recv_buf, send_buf;
    int r;

    chunk_t *temp;
    chunk_t *chunk;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0x1dd42a3c) + (rb_state3.var_0));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_0);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (0x3fd7b863);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0xcdfeb7ec));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (0x1b135b69);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (138414 < rb_input_size ? (uint32_t)rb_input[138414] : 0x55287a01);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (13833 < rb_input_size ? (uint32_t)rb_input[13833] : 0x608f3e2d);
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (35049 < rb_input_size ? (uint32_t)rb_input[35049] : 0xbe472faf);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_3);
    rb_state9.var_4 = (rb_state9.var_4) + (770534 < rb_input_size ? (uint32_t)rb_input[770534] : 0x65dc1906);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_0);
    #endif
    r = 0;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (0xd5e7055);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + (((rb_state9.var_0) == (0xab6bb975)) ? (rb_state9.var_3) : (0xd7f0f4d2));
    rb_state9.var_1 = (rb_state9.var_1) + (359046 < rb_input_size ? (uint32_t)rb_input[359046] : 0xcf78fb77);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (774124 < rb_input_size ? (uint32_t)rb_input[774124] : 0x81c06987);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0xde8f30e2));
    #endif
    r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ ((0xaa907f05) ^ (0xf269c4fa));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_4 = (rb_state9.var_4) - (rb_state9.var_4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - (((rb_state18.var_2) == (0xffffff00)) ? (rb_state18.var_1) : (0x91398c56));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_3 = (rb_state19.var_3) - ((0x67d11e1a) ^ (rb_state19.var_3));
    #endif
    r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_3);
    if ((rb_state5.var_1) == (0x9bfd701b)) {
        rb_state5.var_12 = 0x6ec47823;
        rb_state5.var_13 = 0xf1915a0a;
        rb_state5.var_14 = (rb_state5.var_14) - (((((0x24044c) + (rb_state5.var_8)) - (rb_state5.var_12)) + (rb_state5.var_7)) + (rb_state5.var_13));
        rb_state5.var_15 = (rb_state5.var_15) + (414158 < rb_input_size ? (uint32_t)rb_input[414158] : 0x5aa5b57d);
        rb_state5.var_16 = (rb_state5.var_16) - (((((0x1e0657fd) - (0x3ec4af4c)) ^ (rb_state5.var_6)) ^ (rb_state5.var_14)) + (rb_state5.var_9));
        rb_state5.var_17 = (rb_state5.var_17) ^ (rb_state5.var_15);
        rb_state5.var_18 = (rb_state5.var_18) + (((0x50b8c378) + (rb_state5.var_16)) ^ (0x36aecfe9));
        rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_17) : (0x107d839c));
        rb_state5.var_20 = (rb_state5.var_20) - (((0xd16a5611) - (0xb3132627)) ^ (rb_state5.var_18));
        rb_state5.var_21 = (rb_state5.var_21) - (rb_state5.var_19);
        rb_state5.var_22 = (rb_state5.var_22) ^ (((0x1599218f) + (rb_state5.var_20)) ^ (0xd31b4f3e));
        rb_state5.var_23 = (rb_state5.var_23) - (((rb_state5.var_21) == (0x0)) ? (rb_state5.var_22) : (0x47c351b5));
        rb_state5.var_5 = (rb_state5.var_5) - (rb_state5.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) - (1041676 < rb_input_size ? (uint32_t)rb_input[1041676] : 0x7faeda31);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ (0xc75b31a4);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_3);
    rb_state16.var_3 = (rb_state16.var_3) ^ (0xe4a69db6);
    #endif
    assert(r == 0);

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) + (0x9cc3fefd);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x16a55451)) ? (rb_state13.var_0) : (0x18697966));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ (1095232 < rb_input_size ? (uint32_t)rb_input[1095232] : 0x39f965f6);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_3) == (0x37f7b884)) {
        rb_state18.var_30 = 0xa8b90368;
        rb_state18.var_31 = 0x61afe51d;
        rb_state18.var_32 = (rb_state18.var_32) + (0xf23e3a30);
        rb_state18.var_33 = (rb_state18.var_33) + (rb_state18.var_31);
        rb_state18.var_34 = (rb_state18.var_34) ^ (((0x4c0fb3f2) + (rb_state18.var_18)) + (rb_state18.var_17));
        rb_state18.var_35 = (rb_state18.var_35) - (((((0x8eef9c08) ^ (rb_state18.var_32)) + (rb_state18.var_19)) - (0xa5ad8dea)) - (rb_state18.var_30));
        rb_state18.var_36 = (rb_state18.var_36) + (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_33) : (0x1210c45b));
        rb_state18.var_37 = (rb_state18.var_37) ^ (((0xcd7d7a87) - (rb_state18.var_34)) - (0xf9aac642));
        rb_state18.var_38 = (rb_state18.var_38) + (((((0x4e4fc157) + (rb_state18.var_21)) - (0x4dbb5972)) ^ (rb_state18.var_35)) - (rb_state18.var_36));
        rb_state18.var_39 = (rb_state18.var_39) ^ (((0xbf1ecc7d) - (rb_state18.var_37)) ^ (rb_state18.var_22));
        rb_state18.var_40 = (rb_state18.var_40) - (((((0xae3b6832) + (0x7c9f301d)) - (rb_state18.var_38)) ^ (rb_state18.var_39)) ^ (rb_state18.var_23));
        rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_40);
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_1) == (0xc67f1523)) ? (rb_state9.var_2) : (0x21b98a8b));
    if ((rb_state9.var_0) == (0x90585e0c)) {
        rb_state9.var_16 = 0x44cf99eb;
        rb_state9.var_17 = (rb_state9.var_17) - (((((0x2bd703e1) + (rb_state9.var_9)) ^ (rb_state9.var_11)) ^ (0x134d0ab9)) + (rb_state9.var_16));
        rb_state9.var_18 = (rb_state9.var_18) ^ (rb_state9.var_12);
        rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_10) : (0xbb8828be));
        rb_state9.var_20 = (rb_state9.var_20) + (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_18) : (0x25bd9837));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_20) : (0x389c4f5a));
        rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_21);
        rb_state9.var_5 = rb_state9.var_15;
    }
    if ((rb_state9.var_0) == (0x90585e0c)) {
        pthread_mutex_lock(&(rb_state9.lock_31));
        rb_state9.var_23 = (rb_state9.var_23) - (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_14) : (0xed8ba64f));
        rb_state9.var_24 = (rb_state9.var_24) ^ (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_17) : (0x90163845));
        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_15) : (0x8a150489));
        rb_state9.var_26 = (rb_state9.var_26) ^ (((((0xfa12cb66) + (rb_state9.var_24)) - (rb_state9.var_21)) ^ (rb_state9.var_23)) + (rb_state9.var_20));
        rb_state9.var_27 = (rb_state9.var_27) - (((0x666e5d20) + (rb_state9.var_25)) ^ (0x9f13da31));
        rb_state9.var_28 = (rb_state9.var_28) ^ (((0xb33f0693) - (rb_state9.var_26)) + (rb_state9.var_22));
        rb_state9.var_29 = (rb_state9.var_29) + (rb_state9.var_27);
        rb_state9.var_30 = (rb_state9.var_30) - (((((0x2326e119) ^ (0x33858632)) + (rb_state9.var_23)) - (rb_state9.var_29)) ^ (rb_state9.var_28));
        rb_state9.var_22 = (rb_state9.var_22) + (rb_state9.var_30);
        rb_state9.var_5 = rb_state9.var_22;
        pthread_mutex_unlock(&(rb_state9.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x8964276d)) {
        if ((rb_state16.var_4) != (0x0)) {
            if (!((rb_state16.var_4) == (rb_state16.var_18))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (0x3e1f1203);
    #endif
    init_stats(thread_stats);
#endif

    while (TRUE) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            r = queue_dequeue(&refine_que[qid], &recv_buf, MAX_PER_FETCH);
            if (r < 0) {
                break;
            }
        }

        chunk = (chunk_t *)ringbuffer_remove(&recv_buf);
        assert(chunk != NULL);

        rabininit(rf_win, rabintab, rabinwintab);

        int split;
        sequence_number_t chcount = 0;
        do {

            int offset = rabinseg(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n, rf_win, rabintab, rabinwintab);

            if (offset < chunk->uncompressed_data.n) {

                temp = (chunk_t *)malloc(sizeof(chunk_t));
                if (temp == NULL)
                    EXIT_TRACE("Memory allocation failed.\n");
                temp->header.state = chunk->header.state;
                temp->sequence.l1num = chunk->sequence.l1num;

                r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data, offset);
                if (r != 0)
                    EXIT_TRACE("Unable to split memory buffer.\n");

                chunk->sequence.l2num = chcount;
                chunk->isLastL2Chunk = FALSE;
                chcount++;

#ifdef ENABLE_STATISTICS

                thread_stats->nChunks[CHUNK_SIZE_TO_SLOT(chunk->uncompressed_data.n)]++;
#endif

                r = ringbuffer_insert(&send_buf, chunk);
                assert(r == 0);
                if (ringbuffer_isFull(&send_buf)) {
                    r = queue_enqueue(&deduplicate_que[qid], &send_buf, CHUNK_ANCHOR_PER_INSERT);
                    assert(r >= 1);
                }

                chunk = temp;
                split = 1;
            } else {

                chunk->sequence.l2num = chcount;
                chunk->isLastL2Chunk = TRUE;
                chcount++;

#ifdef ENABLE_STATISTICS

                thread_stats->nChunks[CHUNK_SIZE_TO_SLOT(chunk->uncompressed_data.n)]++;
#endif

                r = ringbuffer_insert(&send_buf, chunk);
                assert(r == 0);
                if (ringbuffer_isFull(&send_buf)) {
                    r = queue_enqueue(&deduplicate_que[qid], &send_buf, CHUNK_ANCHOR_PER_INSERT);
                    assert(r >= 1);
                }

                chunk = NULL;
                split = 0;
            }
        } while (split);
    }

    while (!ringbuffer_isEmpty(&send_buf)) {
        r = queue_enqueue(&deduplicate_que[qid], &send_buf, CHUNK_ANCHOR_PER_INSERT);
        assert(r >= 1);
    }

    free(rabintab);
    free(rabinwintab);
    ringbuffer_destroy(&recv_buf);
    ringbuffer_destroy(&send_buf);

    queue_terminate(&deduplicate_que[qid]);
#ifdef ENABLE_STATISTICS
    return thread_stats;
#else
    return NULL;
#endif
}
#endif

void *SerialIntegratedPipeline(void *targs) {
    struct thread_args *args = (struct thread_args *)targs;
    size_t preloading_buffer_seek = 0;
    int fd = args->fd;
    int fd_out = create_output_file(conf->outfile);
    int r;

    chunk_t *temp = NULL;
    chunk_t *chunk = NULL;
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    rf_win_dataprocess = 0;
    rabininit(rf_win_dataprocess, rabintab, rabinwintab);

    if (MAXBUF < 8 * ANCHOR_JUMP) {
        printf("WARNING: I/O buffer size is very small. Performance degraded.\n");
        fflush(NULL);
    }

    while (1) {
        size_t bytes_left;

        if (temp != NULL) {
            bytes_left = temp->uncompressed_data.n;
        } else {
            bytes_left = 0;
        }

        if (MAXBUF + bytes_left > SSIZE_MAX) {
            EXIT_TRACE("Input buffer size exceeds system maximum.\n");
        }

        chunk = (chunk_t *)malloc(sizeof(chunk_t));
        if (chunk == NULL)
            EXIT_TRACE("Memory allocation failed.\n");
        r = mbuffer_create(&chunk->uncompressed_data, MAXBUF + bytes_left);
        if (r != 0) {
            EXIT_TRACE("Unable to initialize memory buffer.\n");
        }
        chunk->header.state = CHUNK_STATE_UNCOMPRESSED;
        if (bytes_left > 0) {

            memcpy(chunk->uncompressed_data.ptr, temp->uncompressed_data.ptr, temp->uncompressed_data.n);
            mbuffer_free(&temp->uncompressed_data);
            free(temp);
            temp = NULL;
        }

        size_t bytes_read = 0;
        if (conf->preloading) {
            size_t max_read = MIN(MAXBUF, args->input_file.size - preloading_buffer_seek);
            memcpy(chunk->uncompressed_data.ptr + bytes_left, args->input_file.buffer + preloading_buffer_seek, max_read);
            bytes_read = max_read;
            preloading_buffer_seek += max_read;
        } else {
            while (bytes_read < MAXBUF) {
                r = read(fd, chunk->uncompressed_data.ptr + bytes_left + bytes_read, MAXBUF - bytes_read);
                if (r < 0) {
                    switch (errno) {
                    case EAGAIN:
                        EXIT_TRACE("I/O error: No data available\n");
                        break;
                    case EBADF:
                        EXIT_TRACE("I/O error: Invalid file descriptor\n");
                        break;
                    case EFAULT:
                        EXIT_TRACE("I/O error: Buffer out of range\n");
                        break;
                    case EINTR:
                        EXIT_TRACE("I/O error: Interruption\n");
                        break;
                    case EINVAL:
                        EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
                        break;
                    case EIO:
                        EXIT_TRACE("I/O error: Generic I/O error\n");
                        break;
                    case EISDIR:
                        EXIT_TRACE("I/O error: Cannot read from a directory\n");
                        break;
                    default:
                        EXIT_TRACE("I/O error: Unrecognized error\n");
                        break;
                    }
                }
                if (r == 0) {
                    break;
                }
                bytes_read += r;
            }
        }

        if (bytes_left + bytes_read == 0) {
            mbuffer_free(&chunk->uncompressed_data);
            free(chunk);
            chunk = NULL;
            break;
        }

        if (bytes_left + bytes_read < chunk->uncompressed_data.n) {
            r = mbuffer_realloc(&chunk->uncompressed_data, bytes_left + bytes_read);
            assert(r == 0);
        }

        if (bytes_read == 0) {
#ifdef ENABLE_STATISTICS

            stats.nChunks[CHUNK_SIZE_TO_SLOT(chunk->uncompressed_data.n)]++;
#endif

            int isDuplicate = sub_Deduplicate(chunk);
#ifdef ENABLE_STATISTICS
            if (isDuplicate) {
                stats.nDuplicates++;
            } else {
                stats.total_dedup += chunk->uncompressed_data.n;
            }
#endif

            if (!isDuplicate) {
                sub_Compress(chunk);
#ifdef ENABLE_STATISTICS
                stats.total_compressed += chunk->compressed_data.n;
#endif
            }

            write_chunk_to_file(fd_out, chunk);
            if (chunk->header.isDuplicate) {
                free(chunk);
                chunk = NULL;
            }

            break;
        }

        int split;
        do {
            split = 0;

            int offset = rabinseg(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n, rf_win_dataprocess, rabintab, rabinwintab);

            if (offset == 0) {

                assert(0);
                split = 0;
            } else if (offset < chunk->uncompressed_data.n) {

                temp = (chunk_t *)malloc(sizeof(chunk_t));
                if (temp == NULL)
                    EXIT_TRACE("Memory allocation failed.\n");

                r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data, offset);
                if (r != 0)
                    EXIT_TRACE("Unable to split memory buffer.\n");
                temp->header.state = CHUNK_STATE_UNCOMPRESSED;

#ifdef ENABLE_STATISTICS

                stats.nChunks[CHUNK_SIZE_TO_SLOT(chunk->uncompressed_data.n)]++;
#endif

                int isDuplicate = sub_Deduplicate(chunk);
#ifdef ENABLE_STATISTICS
                if (isDuplicate) {
                    stats.nDuplicates++;
                } else {
                    stats.total_dedup += chunk->uncompressed_data.n;
                }
#endif

                if (!isDuplicate) {
                    sub_Compress(chunk);
#ifdef ENABLE_STATISTICS
                    stats.total_compressed += chunk->compressed_data.n;
#endif
                }

                write_chunk_to_file(fd_out, chunk);
                if (chunk->header.isDuplicate) {
                    free(chunk);
                    chunk = NULL;
                }

                chunk = temp;
                temp = NULL;
                split = 1;
            } else {

                temp = chunk;
                chunk = NULL;
                split = 0;
            }
        } while (split);
    }

    free(rabintab);
    free(rabinwintab);

    close(fd_out);

    return NULL;
}

#ifdef ENABLE_PTHREADS
void *Fragment(void *targs) {
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (0x29952b42);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + ((0x66ee8b58) ^ (0x877cfb1f));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (0xd4c1c1d0);
    rb_state11.var_3 = (rb_state11.var_3) + (0x2e1ef875);
    #endif
    size_t preloading_buffer_seek = 0;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (38250 < rb_input_size ? (uint32_t)rb_input[38250] : 0xe04c3f67);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (356769 < rb_input_size ? (uint32_t)rb_input[356769] : 0x4dec8119);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (rb_state18.var_0);
    rb_state18.var_1 = (rb_state18.var_1) - ((0x5a6ad12e) + (rb_state18.var_1));
    #endif
    int qid = 0;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (946749 < rb_input_size ? (uint32_t)rb_input[946749] : 0xd3561fc);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_3) == (0x2e1ef914)) ? (rb_state11.var_2) : (0xe6da6a6));
    rb_state11.var_0 = (rb_state11.var_0) + (0x62f7eb43);
    rb_state11.var_4 = (rb_state11.var_4) + (345541 < rb_input_size ? (uint32_t)rb_input[345541] : 0xb5660fb4);
    if ((rb_state11.var_4) == (0x32)) {
        rb_state11.var_7 = 0x1;
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (0xb1313770);
    #endif
    int fd = args->fd;
    int i;

    ringbuffer_t send_buf;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_4) == (0x32)) {
        pthread_mutex_lock(&(rb_state11.lock_16));
        rb_state11.var_8 = 0xb5a06756;
        rb_state11.var_9 = 0x84389dca;
        rb_state11.var_10 = (rb_state11.var_10) ^ (((((0x1871d11a) + (rb_state11.var_6)) - (0x81a1cc50)) ^ (rb_state11.var_5)) - (rb_state11.var_8));
        rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_9);
        rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_7) == (0x1)) ? (rb_state11.var_10) : (0x66676f8e));
        rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_8) == (0xb5a06756)) ? (rb_state11.var_11) : (0x4a9bfd5d));
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) + (((rb_state11.var_13) == (0x84389dca)) ? (rb_state11.var_14) : (0x2a387108));
        rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_15);
        rb_state11.var_5 = rb_state11.var_6;
        pthread_mutex_unlock(&(rb_state11.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (0xf0dcb53e);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_0);
    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_0) == (0x88f0e18a)) ? (rb_state16.var_2) : (0x13dd1fa1));
    #endif
    sequence_number_t anchorcount = 0;
    int r;

    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_2) == (0x17b48c44)) {
        pthread_mutex_lock(&(rb_state14.lock_19));
        rb_state14.var_6 = 0x9db72afb;
        rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0x4f9c6c37));
        rb_state14.var_8 = (rb_state14.var_8) + (0x1184138b);
        rb_state14.var_9 = (rb_state14.var_9) ^ (((0xcdbb9e56) ^ (rb_state14.var_5)) - (rb_state14.var_6));
        rb_state14.var_10 = (rb_state14.var_10) + (rb_state14.var_7);
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_8) == (0x1184138b)) ? (rb_state14.var_9) : (0xcf9d0e65));
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_7) == (0x9db72afb)) ? (rb_state14.var_10) : (0xfa2b1599));
        rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_11) == (0x3004735b)) ? (rb_state14.var_12) : (0xd760f3cb));
        rb_state14.var_5 = (rb_state14.var_5) ^ (rb_state14.var_13);
        rb_state14.var_4 = rb_state14.var_5;
        pthread_mutex_unlock(&(rb_state14.lock_19));
    }
    if ((rb_state14.var_2) == (0x17b48c44)) {
        if (!((rb_state14.var_4) == (rb_state14.var_5))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_0) == (0x88f0e18a)) ? (rb_state16.var_2) : (0xd1de00ea));
    #endif
    chunk_t *temp = NULL;
    chunk_t *chunk = NULL;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) + (86541 < rb_input_size ? (uint32_t)rb_input[86541] : 0xc1ca9e);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0x1d871123));
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_3 = (rb_state8.var_3) + (0x588f07b9);
    #endif
    r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) ^ ((0x86f6ccb3) ^ (0x15dfe266));
    #endif
    assert(r == 0);

    rf_win_dataprocess = 0;
    rabininit(rf_win_dataprocess, rabintab, rabinwintab);

    if (MAXBUF < 8 * ANCHOR_JUMP) {
        printf("WARNING: I/O buffer size is very small. Performance degraded.\n");
        fflush(NULL);
    }

    while (1) {
        size_t bytes_left;

        if (temp != NULL) {
            bytes_left = temp->uncompressed_data.n;
        } else {
            bytes_left = 0;
        }

        if (MAXBUF + bytes_left > SSIZE_MAX) {
            EXIT_TRACE("Input buffer size exceeds system maximum.\n");
        }

        chunk = (chunk_t *)malloc(sizeof(chunk_t));
        if (chunk == NULL)
            EXIT_TRACE("Memory allocation failed.\n");
        r = mbuffer_create(&chunk->uncompressed_data, MAXBUF + bytes_left);
        if (r != 0) {
            EXIT_TRACE("Unable to initialize memory buffer.\n");
        }
        if (bytes_left > 0) {

            chunk->header.state = CHUNK_STATE_UNCOMPRESSED;
            chunk->sequence.l1num = temp->sequence.l1num;

            memcpy(chunk->uncompressed_data.ptr, temp->uncompressed_data.ptr, temp->uncompressed_data.n);
            mbuffer_free(&temp->uncompressed_data);
            free(temp);
            temp = NULL;
        } else {

            chunk->header.state = CHUNK_STATE_UNCOMPRESSED;
            chunk->sequence.l1num = anchorcount;
            anchorcount++;
        }

        size_t bytes_read = 0;
        if (conf->preloading) {
            size_t max_read = MIN(MAXBUF, args->input_file.size - preloading_buffer_seek);
            memcpy(chunk->uncompressed_data.ptr + bytes_left, args->input_file.buffer + preloading_buffer_seek, max_read);
            bytes_read = max_read;
            preloading_buffer_seek += max_read;
        } else {
            while (bytes_read < MAXBUF) {
                r = read(fd, chunk->uncompressed_data.ptr + bytes_left + bytes_read, MAXBUF - bytes_read);
                if (r < 0) {
                    switch (errno) {
                    case EAGAIN:
                        EXIT_TRACE("I/O error: No data available\n");
                        break;
                    case EBADF:
                        EXIT_TRACE("I/O error: Invalid file descriptor\n");
                        break;
                    case EFAULT:
                        EXIT_TRACE("I/O error: Buffer out of range\n");
                        break;
                    case EINTR:
                        EXIT_TRACE("I/O error: Interruption\n");
                        break;
                    case EINVAL:
                        EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
                        break;
                    case EIO:
                        EXIT_TRACE("I/O error: Generic I/O error\n");
                        break;
                    case EISDIR:
                        EXIT_TRACE("I/O error: Cannot read from a directory\n");
                        break;
                    default:
                        EXIT_TRACE("I/O error: Unrecognized error\n");
                        break;
                    }
                }
                if (r == 0) {
                    break;
                }
                bytes_read += r;
            }
        }

        if (bytes_left + bytes_read == 0) {
            mbuffer_free(&chunk->uncompressed_data);
            free(chunk);
            chunk = NULL;
            break;
        }

        if (bytes_left + bytes_read < chunk->uncompressed_data.n) {
            r = mbuffer_realloc(&chunk->uncompressed_data, bytes_left + bytes_read);
            assert(r == 0);
        }

        if (bytes_read == 0) {

            r = ringbuffer_insert(&send_buf, chunk);
            assert(r == 0);

            break;
        }

        int split;
        do {
            split = 0;

            if (ANCHOR_JUMP < chunk->uncompressed_data.n) {
                int offset = rabinseg(chunk->uncompressed_data.ptr + ANCHOR_JUMP, chunk->uncompressed_data.n - ANCHOR_JUMP, rf_win_dataprocess, rabintab, rabinwintab);

                if (offset == 0) {

                    assert(0);
                    split = 0;
                } else if (offset + ANCHOR_JUMP < chunk->uncompressed_data.n) {

                    temp = (chunk_t *)malloc(sizeof(chunk_t));
                    if (temp == NULL)
                        EXIT_TRACE("Memory allocation failed.\n");

                    r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data, offset + ANCHOR_JUMP);
                    if (r != 0)
                        EXIT_TRACE("Unable to split memory buffer.\n");
                    temp->header.state = CHUNK_STATE_UNCOMPRESSED;
                    temp->sequence.l1num = anchorcount;
                    anchorcount++;

                    r = ringbuffer_insert(&send_buf, chunk);
                    assert(r == 0);

                    if (ringbuffer_isFull(&send_buf)) {
                        r = queue_enqueue(&refine_que[qid], &send_buf, ANCHOR_DATA_PER_INSERT);
                        assert(r >= 1);
                        qid = (qid + 1) % args->nqueues;
                    }

                    chunk = temp;
                    temp = NULL;
                    split = 1;
                } else {

                    temp = chunk;
                    chunk = NULL;
                    split = 0;
                }
            } else {

                temp = chunk;
                chunk = NULL;
                split = 0;
            }
        } while (split);
    }

    while (!ringbuffer_isEmpty(&send_buf)) {
        r = queue_enqueue(&refine_que[qid], &send_buf, ANCHOR_DATA_PER_INSERT);
        assert(r >= 1);
        qid = (qid + 1) % args->nqueues;
    }

    free(rabintab);
    free(rabinwintab);
    ringbuffer_destroy(&send_buf);

    for (i = 0; i < args->nqueues; i++) {
        queue_terminate(&refine_que[i]);
    }

    return NULL;
}
#endif

#ifdef ENABLE_PTHREADS
void *Reorder(void *targs) {
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (706729 < rb_input_size ? (uint32_t)rb_input[706729] : 0xb88c8e88);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - ((0x3859221) - (rb_state13.var_3));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x2bc9f58a)) {
        rb_state19.var_10 = 0x3339928e;
        rb_state19.var_11 = 0x49416b4d;
        rb_state19.var_12 = (rb_state19.var_12) + (369466 < rb_input_size ? (uint32_t)rb_input[369466] : 0xf34caf63);
        rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_10) == (0x3339928e)) ? (rb_state19.var_11) : (0x6485a6b9));
        rb_state19.var_14 = (rb_state19.var_14) ^ (((rb_state19.var_7) == (0xc450e6bf)) ? (rb_state19.var_6) : (0x4ef822ae));
        rb_state19.var_15 = (rb_state19.var_15) + (((rb_state19.var_12) == (0x78)) ? (rb_state19.var_13) : (0x7e828ab1));
        rb_state19.var_16 = (rb_state19.var_16) + (rb_state19.var_14);
        rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_15) == (0xb6be94b3)) ? (rb_state19.var_16) : (0x9414b7e1));
        rb_state19.var_4 = (rb_state19.var_4) ^ (rb_state19.var_17);
    }
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (461967 < rb_input_size ? (uint32_t)rb_input[461967] : 0xa631a36e);
    #endif
    int qid = 0;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_30) == (0x1)) {
        pthread_mutex_lock(&(rb_state8.lock_57));
        rb_state8.var_33 = 0xe0fda4d7;
        rb_state8.var_34 = (rb_state8.var_34) + (((rb_state8.var_32) == (0x0)) ? (rb_state8.var_31) : (0xd50f1b20));
        rb_state8.var_35 = (rb_state8.var_35) - (0xd26fab17);
        rb_state8.var_36 = (rb_state8.var_36) ^ (((0x12bc34e5) + (rb_state8.var_33)) + (rb_state8.var_33));
        rb_state8.var_37 = (rb_state8.var_37) - (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_35) : (0xfda14158));
        rb_state8.var_38 = (rb_state8.var_38) - (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_36) : (0x868e31b0));
        rb_state8.var_39 = (rb_state8.var_39) - (rb_state8.var_37);
        rb_state8.var_40 = (rb_state8.var_40) - (((rb_state8.var_35) == (0x2d9054e9)) ? (rb_state8.var_38) : (0x8e1575a0));
        rb_state8.var_41 = (rb_state8.var_41) + (((rb_state8.var_36) == (0xd4b77e93)) ? (rb_state8.var_39) : (0xa5a8898e));
        rb_state8.var_42 = (rb_state8.var_42) - (((rb_state8.var_40) == (0xd4b77e93)) ? (rb_state8.var_41) : (0x83ac8a62));
        rb_state8.var_32 = (rb_state8.var_32) - (rb_state8.var_42);
        rb_state8.var_31 = rb_state8.var_32;
        pthread_mutex_unlock(&(rb_state8.lock_57));
    }
    if ((rb_state8.var_30) == (0x1)) {
        pthread_mutex_lock(&(rb_state8.lock_57));
        if (!((rb_state8.var_31) == (rb_state8.var_32))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_57));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_4 = (rb_state13.var_4) - (271634 < rb_input_size ? (uint32_t)rb_input[271634] : 0x3da41678);
    #endif
    int fd = 0;

    ringbuffer_t recv_buf;
    chunk_t *chunk;

    SearchTree T;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_4) == (0x32)) {
        pthread_mutex_lock(&(rb_state11.lock_28));
        rb_state11.var_20 = 0xdd9f3dd8;
        rb_state11.var_21 = 0x6712e573;
        rb_state11.var_22 = (rb_state11.var_22) + (((0xcb7bad73) + (rb_state11.var_21)) ^ (0x9bf65916));
        rb_state11.var_23 = (rb_state11.var_23) ^ (((((0xcdf1ef84) - (rb_state11.var_20)) - (0x408133d5)) - (rb_state11.var_19)) ^ (rb_state11.var_18));
        rb_state11.var_24 = (rb_state11.var_24) ^ (((rb_state11.var_20) == (0xdd9f3dd8)) ? (rb_state11.var_22) : (0x53ccaa8a));
        rb_state11.var_25 = (rb_state11.var_25) + (((0x9803b4ba) - (0xe43b4f32)) - (rb_state11.var_23));
        rb_state11.var_26 = (rb_state11.var_26) + (((0x60f15bbf) ^ (rb_state11.var_21)) + (rb_state11.var_24));
        rb_state11.var_27 = (rb_state11.var_27) ^ (((rb_state11.var_25) == (0x3f6e7b1)) ? (rb_state11.var_26) : (0x8b9c6022));
        rb_state11.var_18 = (rb_state11.var_18) + (rb_state11.var_27);
        rb_state11.var_19 = rb_state11.var_18;
        pthread_mutex_unlock(&(rb_state11.lock_28));
    }
    #endif
    T = TreeMakeEmpty(NULL);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_4) == (0xa499410d)) {
        rb_state0.var_17 = 0x713b2c37;
        rb_state0.var_18 = 0x63d5e59c;
        rb_state0.var_19 = 0xe0193a28;
        rb_state0.var_20 = (rb_state0.var_20) + (((rb_state0.var_13) == (0xc15f85cf)) ? (rb_state0.var_17) : (0xa5866eb1));
        rb_state0.var_21 = (rb_state0.var_21) - (((((0xef1e5502) ^ (rb_state0.var_14)) - (0x849fc78f)) ^ (rb_state0.var_18)) ^ (rb_state0.var_19));
        rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_20) == (0x713b2c37)) ? (rb_state0.var_21) : (0x4e40123));
        rb_state0.var_5 = (rb_state0.var_5) + (rb_state0.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_3);
    if ((rb_state4.var_4) == (0xa383ff10)) {
        pthread_mutex_lock(&(rb_state4.lock_27));
        rb_state4.var_7 = 0x855d4c54;
        rb_state4.var_8 = 0x4140ab06;
        rb_state4.var_9 = 0x85fcc98f;
        rb_state4.var_10 = (rb_state4.var_10) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_5) : (0xc0a97c62));
        rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_7) == (0x855d4c54)) ? (rb_state4.var_9) : (0x72565eb5));
        rb_state4.var_12 = (rb_state4.var_12) ^ (((((0xe7ecca8e) ^ (0xd2c69eae)) + (rb_state4.var_8)) - (rb_state4.var_7)) ^ (rb_state4.var_8));
        rb_state4.var_13 = (rb_state4.var_13) ^ (rb_state4.var_10);
        rb_state4.var_14 = (rb_state4.var_14) ^ (((rb_state4.var_9) == (0x85fcc98f)) ? (rb_state4.var_11) : (0xb4e91895));
        rb_state4.var_15 = (rb_state4.var_15) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_12) : (0x521d72d4));
        rb_state4.var_16 = (rb_state4.var_16) ^ (((0x5a2fbf67) + (0xb082f4dd)) + (rb_state4.var_13));
        rb_state4.var_17 = (rb_state4.var_17) ^ (((((0x8e8c69ca) ^ (0x54133d98)) ^ (rb_state4.var_11)) + (rb_state4.var_14)) ^ (rb_state4.var_15));
        rb_state4.var_18 = (rb_state4.var_18) + (rb_state4.var_16);
        rb_state4.var_19 = (rb_state4.var_19) + (((0xd58f577c) - (rb_state4.var_12)) ^ (rb_state4.var_17));
        rb_state4.var_20 = (rb_state4.var_20) - (((((0x7d970726) ^ (0x57ff12b5)) ^ (rb_state4.var_18)) - (rb_state4.var_19)) ^ (rb_state4.var_13));
        rb_state4.var_6 = (rb_state4.var_6) + (rb_state4.var_20);
        rb_state4.var_5 = rb_state4.var_6;
        pthread_mutex_unlock(&(rb_state4.lock_27));
    }
    if ((rb_state4.var_4) == (0xa383ff4a)) {
        if (!((rb_state4.var_5) == (rb_state4.var_6))) {
            racebench_trigger(4);
        }
    }
    #endif
    Position pos = NULL;
    struct tree_element tele;

    sequence_t next;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x17d987c4)) {
        if ((rb_state3.var_3) != (0x0)) {
            if (!((rb_state3.var_3) == (rb_state3.var_36))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    sequence_reset(&next);

    sequence_number_t *chunks_per_anchor;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_2);
    rb_state1.var_3 = (rb_state1.var_3) - (742931 < rb_input_size ? (uint32_t)rb_input[742931] : 0x71c478e3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_1);
    #endif
    unsigned int chunks_per_anchor_max = 1024;
    chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
    if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - ((0x89e1bb3a) ^ (rb_state10.var_0));
    #endif
    memset(chunks_per_anchor, 0, chunks_per_anchor_max * sizeof(sequence_number_t));
    int r;
    int i;

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) ^ (((rb_state13.var_0) == (0x88de5b7b)) ? (rb_state13.var_4) : (0xa9121ceb));
    #endif
    r = ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    assert(r == 0);

    fd = create_output_file(conf->outfile);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {

            for (i = 0, r = 0; r <= 0 && i < args->nqueues; i++) {
                r = queue_dequeue(&reorder_que[qid], &recv_buf, ITEM_PER_FETCH);
                qid = (qid + 1) % args->nqueues;
            }
            if (r < 0) {
                break;
            }
        }
        chunk = (chunk_t *)ringbuffer_remove(&recv_buf);
        if (chunk == NULL) {
            break;
        }

        if (chunk->sequence.l1num >= chunks_per_anchor_max) {
            chunks_per_anchor = realloc(chunks_per_anchor, 2 * chunks_per_anchor_max * sizeof(sequence_number_t));
            if (chunks_per_anchor == NULL)
                EXIT_TRACE("Error allocating memory\n");
            memset(&chunks_per_anchor[chunks_per_anchor_max], 0, chunks_per_anchor_max * sizeof(sequence_number_t));
            chunks_per_anchor_max *= 2;
        }

        if (chunk->isLastL2Chunk) {
            assert(chunks_per_anchor[chunk->sequence.l1num] == 0);
            chunks_per_anchor[chunk->sequence.l1num] = chunk->sequence.l2num + 1;
        }

        if (!sequence_eq(chunk->sequence, next)) {
            pos = TreeFind(chunk->sequence.l1num, T);
            if (pos == NULL) {

                tele.l1num = chunk->sequence.l1num;
                tele.queue = Initialize(INITIAL_SEARCH_TREE_SIZE);
                Insert(chunk, tele.queue);
                T = TreeInsert(tele, T);
            } else {
                Insert(chunk, pos->Element.queue);
            }
            continue;
        }

        pos = TreeFindMin(T);
        do {
            write_chunk_to_file(fd, chunk);
            if (chunk->header.isDuplicate) {
                free(chunk);
                chunk = NULL;
            }
            sequence_inc_l2(&next);
            if (chunks_per_anchor[next.l1num] != 0 && next.l2num == chunks_per_anchor[next.l1num]) {
                sequence_inc_l1(&next);
            }

            if (pos != NULL && (pos->Element.l1num == next.l1num)) {
                chunk = FindMin(pos->Element.queue);
                if (sequence_eq(chunk->sequence, next)) {

                    DeleteMin(pos->Element.queue);
                    if (IsEmpty(pos->Element.queue)) {
                        Destroy(pos->Element.queue);
                        T = TreeDelete(pos->Element, T);
                        pos = TreeFindMin(T);
                    }
                } else {

                    chunk = NULL;
                }
            } else {

                chunk = NULL;
            }
        } while (chunk != NULL);
    }

    pos = TreeFindMin(T);
    while (pos != NULL) {
        if (pos->Element.l1num == next.l1num) {
            chunk = FindMin(pos->Element.queue);
            if (sequence_eq(chunk->sequence, next)) {

                DeleteMin(pos->Element.queue);
                if (IsEmpty(pos->Element.queue)) {
                    Destroy(pos->Element.queue);
                    T = TreeDelete(pos->Element, T);
                    pos = TreeFindMin(T);
                }
            } else {

                EXIT_TRACE("L2 sequence number mismatch.\n");
            }
        } else {

            EXIT_TRACE("L1 sequence number mismatch.\n");
        }
        write_chunk_to_file(fd, chunk);
        if (chunk->header.isDuplicate) {
            free(chunk);
            chunk = NULL;
        }
        sequence_inc_l2(&next);
        if (chunks_per_anchor[next.l1num] != 0 && next.l2num == chunks_per_anchor[next.l1num]) {
            sequence_inc_l1(&next);
        }
    }

    close(fd);

    ringbuffer_destroy(&recv_buf);
    free(chunks_per_anchor);

    return NULL;
}
#endif

void Encode(config_t *_conf) {
    struct stat filestat;
    int32 fd;

    conf = _conf;

#ifdef ENABLE_STATISTICS
    init_stats(&stats);
#endif

    cache = hashtable_create(65536, hash_from_key_fn, keys_equal_fn, FALSE);
    if (cache == NULL) {
        printf("ERROR: Out of memory\n");
        exit(1);
    }

#ifdef ENABLE_PTHREADS
    struct thread_args data_process_args;
    int i;

    const int nqueues = (conf->nthreads / MAX_THREADS_PER_QUEUE) + ((conf->nthreads % MAX_THREADS_PER_QUEUE != 0) ? 1 : 0);
    deduplicate_que = malloc(sizeof(queue_t) * nqueues);
    refine_que = malloc(sizeof(queue_t) * nqueues);
    reorder_que = malloc(sizeof(queue_t) * nqueues);
    compress_que = malloc(sizeof(queue_t) * nqueues);
    if ((deduplicate_que == NULL) || (refine_que == NULL) || (reorder_que == NULL) || (compress_que == NULL)) {
        printf("Out of memory\n");
        exit(1);
    }
    int threads_per_queue;
    for (i = 0; i < nqueues; i++) {
        if (i < nqueues - 1 || conf->nthreads % MAX_THREADS_PER_QUEUE == 0) {

            threads_per_queue = MAX_THREADS_PER_QUEUE;
        } else {

            threads_per_queue = conf->nthreads % MAX_THREADS_PER_QUEUE;
        }

        queue_init(&deduplicate_que[i], QUEUE_SIZE, threads_per_queue);
        queue_init(&refine_que[i], QUEUE_SIZE, 1);
        queue_init(&reorder_que[i], QUEUE_SIZE, threads_per_queue);
        queue_init(&compress_que[i], QUEUE_SIZE, threads_per_queue);
    }
#else
    struct thread_args generic_args;
#endif

    assert(!mbuffer_system_init());

    if (stat(conf->infile, &filestat) < 0)
        EXIT_TRACE("stat() %s failed: %s\n", conf->infile, strerror(errno));

    if (!S_ISREG(filestat.st_mode))
        EXIT_TRACE("not a normal file: %s\n", conf->infile);
#ifdef ENABLE_STATISTICS
    stats.total_input = filestat.st_size;
#endif

    if ((fd = open(conf->infile, O_RDONLY | O_LARGEFILE)) < 0)
        EXIT_TRACE("%s file open error %s\n", conf->infile, strerror(errno));

    void *preloading_buffer = NULL;
    if (conf->preloading) {
        size_t bytes_read = 0;
        int r;

        preloading_buffer = malloc(filestat.st_size);
        if (preloading_buffer == NULL)
            EXIT_TRACE("Error allocating memory for input buffer.\n");

        while (bytes_read < filestat.st_size) {
            r = read(fd, preloading_buffer + bytes_read, filestat.st_size - bytes_read);
            if (r < 0) {
                switch (errno) {
                case EAGAIN:
                    EXIT_TRACE("I/O error: No data available\n");
                    break;
                case EBADF:
                    EXIT_TRACE("I/O error: Invalid file descriptor\n");
                    break;
                case EFAULT:
                    EXIT_TRACE("I/O error: Buffer out of range\n");
                    break;
                case EINTR:
                    EXIT_TRACE("I/O error: Interruption\n");
                    break;
                case EINVAL:
                    EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
                    break;
                case EIO:
                    EXIT_TRACE("I/O error: Generic I/O error\n");
                    break;
                case EISDIR:
                    EXIT_TRACE("I/O error: Cannot read from a directory\n");
                    break;
                default:
                    EXIT_TRACE("I/O error: Unrecognized error\n");
                    break;
                }
            }
            if (r == 0) {
                break;
            }
            bytes_read += r;
        }
#ifdef ENABLE_PTHREADS
        data_process_args.input_file.size = filestat.st_size;
        data_process_args.input_file.buffer = preloading_buffer;
#else
        generic_args.input_file.size = filestat.st_size;
        generic_args.input_file.buffer = preloading_buffer;
#endif
    }

#ifdef ENABLE_PTHREADS

    pthread_t threads_anchor[MAX_THREADS], threads_chunk[MAX_THREADS], threads_compress[MAX_THREADS], threads_send, threads_process;

    data_process_args.tid = 0;
    data_process_args.nqueues = nqueues;
    data_process_args.fd = fd;

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif

    pthread_create(&threads_process, NULL, Fragment, &data_process_args);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (421416 < rb_input_size ? (uint32_t)rb_input[421416] : 0xe6d11f92);
    rb_state2.var_1 = (rb_state2.var_1) + (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x78b22b3e));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (0x22534693);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (0x50cd442f);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xeecee337));
    #endif
    struct thread_args anchor_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - ((0x2c55c18d) + (0x29d75e40));
        rb_state2.var_3 = (rb_state2.var_3) + (833474 < rb_input_size ? (uint32_t)rb_input[833474] : 0xc827c51f);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (335629 < rb_input_size ? (uint32_t)rb_input[335629] : 0x4e528646);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (0x945518f6);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ ((0x615f8278) ^ (0x51906acb));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0xbf974b19));
        rb_state14.var_3 = (rb_state14.var_3) ^ (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_3) : (0xa95da03d));
        rb_state14.var_0 = (rb_state14.var_0) + ((0x3f69558f) + (rb_state14.var_1));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ ((0xbeb7418) ^ (0x92afdfdc));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) - (76411 < rb_input_size ? (uint32_t)rb_input[76411] : 0x2b6ffb8d);
        #endif
        anchor_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_0);
        rb_state11.var_2 = (rb_state11.var_2) - (708769 < rb_input_size ? (uint32_t)rb_input[708769] : 0x55d96f80);
        rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_2);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x7391f034));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (rb_state18.var_0);
        #endif
        pthread_create(&threads_anchor[i], NULL, FragmentRefine, &anchor_thread_args[i]);
    }

    struct thread_args chunk_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) - (950846 < rb_input_size ? (uint32_t)rb_input[950846] : 0xd085d31a);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (0x46d2b7e4);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_1);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (0x220ec3e8);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x2d617e86));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (169171 < rb_input_size ? (uint32_t)rb_input[169171] : 0x2e29712f);
        #endif
        chunk_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - ((0x57790a28) ^ (0xdfdf1655));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (1052408 < rb_input_size ? (uint32_t)rb_input[1052408] : 0xfbb80d51);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + ((0xe36daadc) ^ (rb_state8.var_2));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_4) == (0x47)) ? (rb_state9.var_3) : (0xf1ecba11));
        if ((rb_state9.var_0) == (0x90585e0c)) {
            pthread_mutex_lock(&(rb_state9.lock_31));
            if ((rb_state9.var_5) != (0x0)) {
                rb_state9.var_32 = !((rb_state9.var_5) == (rb_state9.var_22));
            }
            pthread_mutex_unlock(&(rb_state9.lock_31));
        }
        rb_state9.var_24 = (rb_state9.var_24) ^ (0x67524260);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) + ((0xb5c0bce7) ^ (rb_state16.var_2));
        rb_state16.var_0 = (rb_state16.var_0) + (0x47db843f);
        if ((rb_state16.var_1) == (0x8964276d)) {
            pthread_mutex_lock(&(rb_state16.lock_30));
            rb_state16.var_12 = 0x4fd46a9b;
            rb_state16.var_13 = (rb_state16.var_13) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0x2e4fb0c7));
            rb_state16.var_14 = (rb_state16.var_14) + (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_12) : (0x21283fcc));
            rb_state16.var_15 = (rb_state16.var_15) + (0x3fe7824f);
            rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_14) : (0x64bafdbe));
            rb_state16.var_17 = (rb_state16.var_17) + (((((0xb7b7be67) ^ (rb_state16.var_15)) ^ (rb_state16.var_16)) ^ (rb_state16.var_10)) ^ (0x246a0ca2));
            rb_state16.var_11 = (rb_state16.var_11) - (rb_state16.var_17);
            rb_state16.var_4 = rb_state16.var_11;
            pthread_mutex_unlock(&(rb_state16.lock_30));
        }
        if ((rb_state16.var_1) == (0x8964276d)) {
            pthread_mutex_lock(&(rb_state16.lock_30));
            rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_14) : (0x24a6fc46));
            rb_state16.var_20 = (rb_state16.var_20) - (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_13) : (0x26365f49));
            rb_state16.var_21 = (rb_state16.var_21) + (rb_state16.var_16);
            rb_state16.var_22 = (rb_state16.var_22) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_19) : (0x62ea8033));
            rb_state16.var_23 = (rb_state16.var_23) ^ (rb_state16.var_20);
            rb_state16.var_24 = (rb_state16.var_24) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_21) : (0xaca8a825));
            rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_22);
            rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_23);
            rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_25) : (0xd8bb465a));
            rb_state16.var_28 = (rb_state16.var_28) - (rb_state16.var_26);
            rb_state16.var_29 = (rb_state16.var_29) + (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_28) : (0xb9bbf4a));
            rb_state16.var_18 = (rb_state16.var_18) - (rb_state16.var_29);
            rb_state16.var_4 = rb_state16.var_18;
            pthread_mutex_unlock(&(rb_state16.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) ^ (487478 < rb_input_size ? (uint32_t)rb_input[487478] : 0x8c8ac3d3);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ ((0x900adda0) ^ (rb_state19.var_2));
        rb_state19.var_1 = (rb_state19.var_1) ^ ((0xe8333a6a) ^ (rb_state19.var_2));
        rb_state19.var_3 = (rb_state19.var_3) - (758311 < rb_input_size ? (uint32_t)rb_input[758311] : 0x2eb23175);
        #endif
        pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (979668 < rb_input_size ? (uint32_t)rb_input[979668] : 0x86d41133);
    #endif
    struct thread_args compress_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_4 = (rb_state5.var_4) ^ (rb_state5.var_4);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_30 = (rb_state9.var_30) - (0x82e80f2f);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - ((0xd0842637) ^ (0xb97a330b));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_1) == (0x55009e1d)) ? (rb_state13.var_1) : (0xdddec566));
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + ((0x7871a8a5) + (0xd6501cf6));
        #endif
        compress_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0xf456b383);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (((rb_state6.var_0) == (0x2be7dcf)) ? (rb_state6.var_3) : (0x816be063));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_9 = (rb_state9.var_9) - ((0x60488512) + (0xece4529b));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_11 = (rb_state11.var_11) - (599370 < rb_input_size ? (uint32_t)rb_input[599370] : 0xc94aa245);
        rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_13) == (0x84389dca)) ? (rb_state11.var_9) : (0xbff27784));
        rb_state11.var_14 = (rb_state11.var_14) - ((0xc0ff19d2) - (rb_state11.var_15));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) + (520867 < rb_input_size ? (uint32_t)rb_input[520867] : 0x81dd7598);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_2) : (0xbc27a7c7));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_3) == (0x37f7b884)) {
            pthread_mutex_lock(&(rb_state18.lock_41));
            rb_state18.var_6 = 0x47a4e66;
            rb_state18.var_7 = 0x3fc5f01b;
            rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_5) : (0x2c63dfb1));
            rb_state18.var_9 = (rb_state18.var_9) - (((0xf1dd64c5) - (0xa15bfad6)) ^ (rb_state18.var_6));
            rb_state18.var_10 = (rb_state18.var_10) ^ (((0x83d48e61) ^ (rb_state18.var_7)) ^ (0x252ced5a));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((0x14a9af24) ^ (rb_state18.var_4)) - (rb_state18.var_7));
            rb_state18.var_12 = (rb_state18.var_12) - (rb_state18.var_8);
            rb_state18.var_13 = (rb_state18.var_13) ^ (((0xec56df5b) ^ (rb_state18.var_9)) ^ (rb_state18.var_8));
            rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0xeb103664));
            rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0xd644f171));
            rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_13) : (0x6935981a));
            rb_state18.var_17 = (rb_state18.var_17) + (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0xb7a01fc6));
            rb_state18.var_18 = (rb_state18.var_18) + (rb_state18.var_16);
            rb_state18.var_19 = (rb_state18.var_19) - (rb_state18.var_17);
            rb_state18.var_20 = (rb_state18.var_20) - (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_18) : (0x2e67d06c));
            rb_state18.var_21 = (rb_state18.var_21) + (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_19) : (0x16968297));
            rb_state18.var_22 = (rb_state18.var_22) - (rb_state18.var_20);
            rb_state18.var_23 = (rb_state18.var_23) ^ (rb_state18.var_21);
            rb_state18.var_24 = (rb_state18.var_24) ^ (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_22) : (0x2d2ffbf0));
            rb_state18.var_25 = (rb_state18.var_25) + (((0x603b9352) + (rb_state18.var_14)) ^ (rb_state18.var_23));
            rb_state18.var_26 = (rb_state18.var_26) - (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_24) : (0xc28347f9));
            rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_25) : (0xc7fb7ee6));
            rb_state18.var_28 = (rb_state18.var_28) + (rb_state18.var_26);
            rb_state18.var_29 = (rb_state18.var_29) ^ (((rb_state18.var_27) == (0x0)) ? (rb_state18.var_28) : (0x107b025a));
            rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_29);
            rb_state18.var_4 = rb_state18.var_5;
            pthread_mutex_unlock(&(rb_state18.lock_41));
        }
        if ((rb_state18.var_3) == (0x37f7b884)) {
            if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                racebench_trigger(18);
            }
        }
        #endif
        pthread_create(&threads_compress[i], NULL, Compress, &compress_thread_args[i]);
    }

    struct thread_args send_block_args;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_2);
    #endif
    send_block_args.tid = 0;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (0xf1b2e777);
    if ((rb_state6.var_2) == (0xe2d0ab52)) {
        pthread_mutex_lock(&(rb_state6.lock_22));
        rb_state6.var_6 = 0xc6682b77;
        rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_6) : (0xb82d2df3));
        rb_state6.var_8 = (rb_state6.var_8) - (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_5) : (0xd870b456));
        rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_6) == (0xc6682b77)) ? (rb_state6.var_4) : (0x3f10bd7e));
        rb_state6.var_10 = (rb_state6.var_10) - (((0x1c35bd31) - (rb_state6.var_9)) ^ (rb_state6.var_7));
        rb_state6.var_11 = (rb_state6.var_11) + (((0x5de8a53c) ^ (0xfec87b7c)) ^ (rb_state6.var_8));
        rb_state6.var_12 = (rb_state6.var_12) + (((0xae1c59ae) + (rb_state6.var_9)) ^ (0xa10709e5));
        rb_state6.var_13 = (rb_state6.var_13) + (((0x1398866c) ^ (rb_state6.var_10)) + (rb_state6.var_10));
        rb_state6.var_14 = (rb_state6.var_14) - (((rb_state6.var_11) == (0xa320de40)) ? (rb_state6.var_12) : (0x90f3b401));
        rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_11) == (0xa320de40)) ? (rb_state6.var_13) : (0xc93c592c));
        rb_state6.var_16 = (rb_state6.var_16) - (((rb_state6.var_12) == (0xf1b504b)) ? (rb_state6.var_14) : (0x79f22462));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((((0xa9e59651) ^ (rb_state6.var_16)) - (0xc895bcbc)) - (0x61a2376b)) - (rb_state6.var_15));
        rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_17);
        rb_state6.var_4 = rb_state6.var_5;
        pthread_mutex_unlock(&(rb_state6.lock_22));
    }
    if ((rb_state6.var_2) == (0xe2d0ab52)) {
        pthread_mutex_lock(&(rb_state6.lock_22));
        if (!((rb_state6.var_4) == (rb_state6.var_5))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_22));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_23 = (rb_state8.var_23) - ((0x9931fff4) - (rb_state8.var_24));
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_32) == (0x1)) {
        rb_state9.var_43 = 0x1bdc6353;
        rb_state9.var_44 = (rb_state9.var_44) ^ (rb_state9.var_37);
        rb_state9.var_45 = (rb_state9.var_45) - (((0x446aa514) - (rb_state9.var_36)) ^ (0x6aaad3b2));
        rb_state9.var_46 = (rb_state9.var_46) - (((rb_state9.var_39) == (0xe1929a10)) ? (rb_state9.var_38) : (0x795aa18));
        rb_state9.var_47 = (rb_state9.var_47) + (rb_state9.var_35);
        rb_state9.var_48 = (rb_state9.var_48) - (rb_state9.var_43);
        rb_state9.var_49 = (rb_state9.var_49) - (rb_state9.var_44);
        rb_state9.var_50 = (rb_state9.var_50) ^ (((rb_state9.var_40) == (0xa7678b6e)) ? (rb_state9.var_45) : (0xe3d0c8ea));
        rb_state9.var_51 = (rb_state9.var_51) ^ (((((0x16d6052e) ^ (rb_state9.var_47)) ^ (rb_state9.var_46)) ^ (rb_state9.var_41)) ^ (0x75383129));
        rb_state9.var_52 = (rb_state9.var_52) ^ (((rb_state9.var_48) == (0xe4239cad)) ? (rb_state9.var_49) : (0xd01fa24d));
        rb_state9.var_53 = (rb_state9.var_53) - (((rb_state9.var_42) == (0xfc277332)) ? (rb_state9.var_50) : (0x3e8a268c));
        rb_state9.var_54 = (rb_state9.var_54) + (((((0xe82f7507) + (rb_state9.var_44)) ^ (rb_state9.var_52)) + (rb_state9.var_43)) - (rb_state9.var_51));
        rb_state9.var_55 = (rb_state9.var_55) ^ (((rb_state9.var_45) == (0x36dff0e4)) ? (rb_state9.var_53) : (0xda3b28f9));
        rb_state9.var_56 = (rb_state9.var_56) - (((rb_state9.var_46) == (0xaa3bc22c)) ? (rb_state9.var_54) : (0x389e313d));
        rb_state9.var_57 = (rb_state9.var_57) - (((rb_state9.var_47) == (0xa7678b6e)) ? (rb_state9.var_55) : (0x20b7253c));
        rb_state9.var_58 = (rb_state9.var_58) ^ (((0x1713eb9a) - (rb_state9.var_56)) ^ (0x10907fcc));
        rb_state9.var_59 = (rb_state9.var_59) + (((((0xf1cdb106) ^ (rb_state9.var_48)) + (rb_state9.var_58)) ^ (rb_state9.var_49)) - (rb_state9.var_57));
        rb_state9.var_33 = (rb_state9.var_33) - (rb_state9.var_59);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (0x5392d1a4);
    #endif
    send_block_args.nqueues = nqueues;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_27 = (rb_state8.var_27) ^ (0x4113a2c);
    #endif
    pthread_create(&threads_send, NULL, Reorder, &send_block_args);

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) + (((rb_state4.var_2) == (0xc5b6f6f7)) ? (rb_state4.var_3) : (0x8c86eba0));
    #endif
    #ifdef RACEBENCH_BUG_5
    if (!((rb_state5.var_5) == (rb_state5.var_6))) {
        rb_state5.var_59 = 0x15d7f43;
        rb_state5.var_60 = (rb_state5.var_60) + (rb_state5.var_28);
        rb_state5.var_61 = (rb_state5.var_61) - (((0x19e9a9ca) ^ (0x4f20c6b0)) - (rb_state5.var_27));
        rb_state5.var_62 = (rb_state5.var_62) - (((rb_state5.var_29) == (0x0)) ? (rb_state5.var_59) : (0xb6b164c));
        rb_state5.var_63 = (rb_state5.var_63) - (rb_state5.var_60);
        rb_state5.var_64 = (rb_state5.var_64) + (((rb_state5.var_30) == (0x0)) ? (rb_state5.var_61) : (0xe8da4c2e));
        rb_state5.var_65 = (rb_state5.var_65) + (((rb_state5.var_31) == (0x0)) ? (rb_state5.var_62) : (0xa99c9a49));
        rb_state5.var_66 = (rb_state5.var_66) + (rb_state5.var_63);
        rb_state5.var_67 = (rb_state5.var_67) ^ (rb_state5.var_64);
        rb_state5.var_68 = (rb_state5.var_68) - (((((0xa3d8d7d0) + (0x4b12579d)) ^ (rb_state5.var_66)) + (rb_state5.var_65)) ^ (rb_state5.var_32));
        rb_state5.var_69 = (rb_state5.var_69) + (((0xed157741) - (rb_state5.var_67)) + (rb_state5.var_33));
        rb_state5.var_70 = (rb_state5.var_70) - (((rb_state5.var_68) == (0x12724fd6)) ? (rb_state5.var_69) : (0x78990b18));
        rb_state5.var_58 = (rb_state5.var_58) - (rb_state5.var_70);
        rb_state5.var_25 = rb_state5.var_58;
    }
    if (!((rb_state5.var_5) == (rb_state5.var_6))) {
        pthread_mutex_lock(&(rb_state5.lock_80));
        rb_state5.var_72 = 0xd16a26e2;
        rb_state5.var_73 = 0x8d9eb7df;
        rb_state5.var_74 = 0xe2bd09f0;
        rb_state5.var_75 = (rb_state5.var_75) + (0xd68828ea);
        rb_state5.var_76 = (rb_state5.var_76) ^ (((((0xa9e0fb3b) ^ (rb_state5.var_74)) + (rb_state5.var_73)) + (rb_state5.var_35)) + (rb_state5.var_34));
        rb_state5.var_77 = (rb_state5.var_77) + (((rb_state5.var_72) == (0xd16a26e2)) ? (rb_state5.var_75) : (0xf45d5997));
        rb_state5.var_78 = (rb_state5.var_78) ^ (rb_state5.var_76);
        rb_state5.var_79 = (rb_state5.var_79) ^ (((((0x2a54572) ^ (rb_state5.var_77)) + (rb_state5.var_78)) + (rb_state5.var_36)) - (0xf455c1b0));
        rb_state5.var_71 = (rb_state5.var_71) + (rb_state5.var_79);
        rb_state5.var_25 = rb_state5.var_71;
        pthread_mutex_unlock(&(rb_state5.lock_80));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_4 = (rb_state15.var_4) - ((0x3edce06f) ^ (rb_state15.var_4));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_2) == (0x900adda0)) ? (rb_state19.var_1) : (0x9f5e8eea));
    #endif
    stats_t *threads_anchor_rv[conf->nthreads];
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_1) == (0x99f2542e)) ? (rb_state0.var_0) : (0x58bfd6af));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0xe4)) ? (rb_state7.var_0) : (0x270f3a51));
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_30) == (0x1)) {
        pthread_mutex_lock(&(rb_state8.lock_57));
        rb_state8.var_43 = 0xba9b1574;
        rb_state8.var_44 = 0x42ac9f40;
        rb_state8.var_45 = (rb_state8.var_45) ^ (rb_state8.var_37);
        rb_state8.var_46 = (rb_state8.var_46) - (rb_state8.var_44);
        rb_state8.var_47 = (rb_state8.var_47) - (((rb_state8.var_43) == (0xba9b1574)) ? (rb_state8.var_45) : (0x1ac7d2dc));
        rb_state8.var_48 = (rb_state8.var_48) - (((0x704a71aa) ^ (rb_state8.var_46)) ^ (rb_state8.var_38));
        rb_state8.var_49 = (rb_state8.var_49) + (((0xe23b24f7) + (0xf3be1528)) - (rb_state8.var_47));
        rb_state8.var_50 = (rb_state8.var_50) ^ (((rb_state8.var_39) == (0x2d9054e9)) ? (rb_state8.var_48) : (0xe19c23a0));
        rb_state8.var_51 = (rb_state8.var_51) - (((rb_state8.var_40) == (0xd4b77e93)) ? (rb_state8.var_49) : (0x59b0e851));
        rb_state8.var_52 = (rb_state8.var_52) - (((rb_state8.var_41) == (0x2d9054e9)) ? (rb_state8.var_50) : (0x33942194));
        rb_state8.var_53 = (rb_state8.var_53) - (((rb_state8.var_42) == (0xd26fab17)) ? (rb_state8.var_51) : (0x63857a15));
        rb_state8.var_54 = (rb_state8.var_54) + (rb_state8.var_52);
        rb_state8.var_55 = (rb_state8.var_55) - (((0xde3d5ff0) - (rb_state8.var_53)) - (0xa05eaa7d));
        rb_state8.var_56 = (rb_state8.var_56) ^ (((rb_state8.var_54) == (0xe6519007)) ? (rb_state8.var_55) : (0xdbdc4e2d));
        rb_state8.var_31 = (rb_state8.var_31) + (rb_state8.var_56);
        pthread_mutex_unlock(&(rb_state8.lock_57));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) - (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_3) : (0x6b70ab55));
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_2) : (0x6676b713));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_4) == (0xc1231f91)) {
        rb_state15.var_16 = 0x2bf03a08;
        rb_state15.var_17 = (rb_state15.var_17) ^ (((0x154c778b) - (0x808bdb18)) ^ (rb_state15.var_8));
        rb_state15.var_18 = (rb_state15.var_18) + (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_9) : (0xe17df7b));
        rb_state15.var_19 = (rb_state15.var_19) - (rb_state15.var_7);
        rb_state15.var_20 = (rb_state15.var_20) + (rb_state15.var_16);
        rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_17);
        rb_state15.var_22 = (rb_state15.var_22) ^ (((((0xfc1fe81a) ^ (rb_state15.var_11)) - (rb_state15.var_12)) ^ (rb_state15.var_19)) ^ (rb_state15.var_18));
        rb_state15.var_23 = (rb_state15.var_23) - (rb_state15.var_20);
        rb_state15.var_24 = (rb_state15.var_24) - (((0x6da96759) ^ (0xd3368bd3)) + (rb_state15.var_21));
        rb_state15.var_25 = (rb_state15.var_25) ^ (((((0x90dfb5d4) + (rb_state15.var_23)) ^ (rb_state15.var_22)) + (0xb27dcbb7)) + (rb_state15.var_13));
        rb_state15.var_26 = (rb_state15.var_26) + (((0xae142019) - (rb_state15.var_24)) ^ (0x5af149d7));
        rb_state15.var_27 = (rb_state15.var_27) - (rb_state15.var_25);
        rb_state15.var_28 = (rb_state15.var_28) - (((((0xf72cadcf) ^ (rb_state15.var_15)) ^ (rb_state15.var_27)) + (rb_state15.var_14)) - (rb_state15.var_26));
        rb_state15.var_15 = (rb_state15.var_15) ^ (rb_state15.var_28);
        rb_state15.var_5 = rb_state15.var_15;
    }
    if ((rb_state15.var_4) == (0xc1231f91)) {
        pthread_mutex_lock(&(rb_state15.lock_35));
        rb_state15.var_30 = 0xbae49b9a;
        rb_state15.var_31 = (rb_state15.var_31) + (((rb_state15.var_16) == (0x2bf03a08)) ? (rb_state15.var_17) : (0x907e756e));
        rb_state15.var_32 = (rb_state15.var_32) ^ (rb_state15.var_30);
        rb_state15.var_33 = (rb_state15.var_33) - (((0x3f61dfa3) ^ (rb_state15.var_31)) ^ (rb_state15.var_18));
        rb_state15.var_34 = (rb_state15.var_34) ^ (((rb_state15.var_32) == (0xbae49b9a)) ? (rb_state15.var_33) : (0xd7b9dd47));
        rb_state15.var_29 = (rb_state15.var_29) - (rb_state15.var_34);
        rb_state15.var_5 = rb_state15.var_29;
        pthread_mutex_unlock(&(rb_state15.lock_35));
    }
    #endif
    stats_t *threads_chunk_rv[conf->nthreads];
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) ^ (0x76400466);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_10 = (rb_state11.var_10) - (rb_state11.var_17);
    if ((rb_state11.var_4) == (0x32)) {
        usleep(100);
    }
    if ((rb_state11.var_4) == (0x32)) {
        rb_state11.var_29 = !((rb_state11.var_19) == (rb_state11.var_18));
    }
    #endif
    stats_t *threads_compress_rv[conf->nthreads];

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ ((0x2a27a50e) + (rb_state0.var_2));
    rb_state0.var_4 = (rb_state0.var_4) + ((0xf98856fd) - (0x54ef15f0));
    rb_state0.var_3 = (rb_state0.var_3) ^ (0x7a757c98);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_4 = (rb_state4.var_4) - ((0x5c7c06a4) ^ (rb_state4.var_4));
    #endif
    pthread_join(threads_process, NULL);
    for (i = 0; i < conf->nthreads; i++) {
        pthread_join(threads_anchor[i], (void **)&threads_anchor_rv[i]);
    }
    for (i = 0; i < conf->nthreads; i++) {
        pthread_join(threads_chunk[i], (void **)&threads_chunk_rv[i]);
    }
    for (i = 0; i < conf->nthreads; i++) {
        pthread_join(threads_compress[i], (void **)&threads_compress_rv[i]);
    }
    pthread_join(threads_send, NULL);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    for (i = 0; i < nqueues; i++) {
        queue_destroy(&deduplicate_que[i]);
        queue_destroy(&refine_que[i]);
        queue_destroy(&reorder_que[i]);
        queue_destroy(&compress_que[i]);
    }
    free(deduplicate_que);
    free(refine_que);
    free(reorder_que);
    free(compress_que);

#ifdef ENABLE_STATISTICS

    for (i = 0; i < conf->nthreads; i++) {
        merge_stats(&stats, threads_anchor_rv[i]);
        free(threads_anchor_rv[i]);
    }
    for (i = 0; i < conf->nthreads; i++) {
        merge_stats(&stats, threads_chunk_rv[i]);
        free(threads_chunk_rv[i]);
    }
    for (i = 0; i < conf->nthreads; i++) {
        merge_stats(&stats, threads_compress_rv[i]);
        free(threads_compress_rv[i]);
    }
#endif

#else

    generic_args.tid = 0;
    generic_args.nqueues = -1;
    generic_args.fd = fd;

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif

    SerialIntegratedPipeline(&generic_args);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

#endif

    if (conf->preloading) {
        free(preloading_buffer);
    }

    if (conf->infile != NULL) {
        close(fd);
    }

    assert(!mbuffer_system_destroy());

    hashtable_destroy(cache, TRUE);

#ifdef ENABLE_STATISTICS

    if (stat(conf->outfile, &filestat) < 0)
        EXIT_TRACE("stat() %s failed: %s\n", conf->outfile, strerror(errno));
    stats.total_output = filestat.st_size;

    if (conf->verbose) {
        print_stats(&stats);
    }
#endif
}