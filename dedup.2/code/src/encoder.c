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
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xd0164ee6)) {
        rb_state5.var_9 = 0x1443e51a;
        rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_3) : (0xc0af4ea5));
        rb_state5.var_11 = (rb_state5.var_11) ^ (((((0x51729612) ^ (rb_state5.var_9)) + (rb_state5.var_6)) - (rb_state5.var_4)) ^ (0x2644be50));
        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_10) : (0x6dbc2661));
        rb_state5.var_13 = (rb_state5.var_13) - (((0x2827370) ^ (rb_state5.var_8)) + (rb_state5.var_11));
        rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_12) : (0x29326e52));
        rb_state5.var_15 = (rb_state5.var_15) - (rb_state5.var_13);
        rb_state5.var_16 = (rb_state5.var_16) - (((0x9fa3fb7a) ^ (rb_state5.var_10)) + (rb_state5.var_14));
        rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_15) : (0x762fb4fd));
        rb_state5.var_18 = (rb_state5.var_18) ^ (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_17) : (0x98959ca2));
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_18);
    }
    #endif
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

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (171059 < rb_input_size ? (uint32_t)rb_input[171059] : 0x42264958);
    rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x8c3b5874));
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
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x635d4f30)) {
        pthread_mutex_lock(&(rb_state12.lock_31));
        rb_state12.var_4 = 0x7a9bcc90;
        rb_state12.var_5 = 0x526283fb;
        rb_state12.var_6 = 0x53478b3f;
        rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_4) : (0x8086d810));
        rb_state12.var_8 = (rb_state12.var_8) ^ (rb_state12.var_6);
        rb_state12.var_9 = (rb_state12.var_9) - (rb_state12.var_5);
        rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_7) == (0x85643370)) ? (rb_state12.var_8) : (0x76fc9c2e));
        rb_state12.var_11 = (rb_state12.var_11) + (((0x582582af) + (rb_state12.var_9)) + (0x6beb502c));
        rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_10) : (0xc6b26edc));
        rb_state12.var_13 = (rb_state12.var_13) + (((rb_state12.var_4) == (0x7a9bcc90)) ? (rb_state12.var_11) : (0xa765ad57));
        rb_state12.var_14 = (rb_state12.var_14) - (((rb_state12.var_5) == (0x526283fb)) ? (rb_state12.var_12) : (0xeb9a3dbb));
        rb_state12.var_15 = (rb_state12.var_15) - (((((0x2fefc229) + (rb_state12.var_13)) ^ (0xe412ac7c)) ^ (rb_state12.var_14)) + (rb_state12.var_6));
        rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_15);
        rb_state12.var_2 = rb_state12.var_3;
        pthread_mutex_unlock(&(rb_state12.lock_31));
    }
    if ((rb_state12.var_0) == (0x635d4f30)) {
        if (!((rb_state12.var_2) == (rb_state12.var_3))) {
            racebench_trigger(12);
        }
    }
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0xf40f324e);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ (0xf8682459);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_12 = 0xd54502e3;
        rb_state14.var_13 = 0x5f412e03;
        rb_state14.var_14 = 0x76c66913;
        rb_state14.var_15 = (rb_state14.var_15) + (rb_state14.var_5);
        rb_state14.var_16 = (rb_state14.var_16) - (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_14) : (0xc5dcd97d));
        rb_state14.var_17 = (rb_state14.var_17) ^ (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_12) : (0x7fce2b7a));
        rb_state14.var_18 = (rb_state14.var_18) + (((0xe6216d0d) ^ (rb_state14.var_7)) - (rb_state14.var_15));
        rb_state14.var_19 = (rb_state14.var_19) + (((0xe4ca698b) ^ (rb_state14.var_8)) + (rb_state14.var_16));
        rb_state14.var_20 = (rb_state14.var_20) + (rb_state14.var_17);
        rb_state14.var_21 = (rb_state14.var_21) ^ (((((0xedfecc9c) ^ (rb_state14.var_18)) ^ (rb_state14.var_19)) - (rb_state14.var_9)) - (0x3b209101));
        rb_state14.var_22 = (rb_state14.var_22) ^ (rb_state14.var_20);
        rb_state14.var_23 = (rb_state14.var_23) - (rb_state14.var_21);
        rb_state14.var_24 = (rb_state14.var_24) ^ (((rb_state14.var_22) == (0x0)) ? (rb_state14.var_23) : (0x512c93fd));
        rb_state14.var_11 = (rb_state14.var_11) - (rb_state14.var_24);
        rb_state14.var_1 = rb_state14.var_11;
    }
    if ((rb_state14.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_26 = 0xfa75e5b9;
        rb_state14.var_27 = (rb_state14.var_27) - (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_10) : (0x96d13872));
        rb_state14.var_28 = (rb_state14.var_28) - (((((0x41ca66ab) - (0x27a083b6)) ^ (rb_state14.var_11)) - (rb_state14.var_27)) - (0x1148a001));
        rb_state14.var_25 = (rb_state14.var_25) ^ (rb_state14.var_28);
        rb_state14.var_1 = rb_state14.var_25;
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0xffffffa0)) {
        rb_state16.var_2 = rb_state16.var_0;
    }
    if ((rb_state16.var_1) == (0xffffffa0)) {
        if (!((rb_state16.var_0) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (0x669fe450);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x2486bec8)) {
        if ((rb_state4.var_2) != (0x0)) {
            if (!((rb_state4.var_2) == (rb_state4.var_27))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_1);
    #endif
    if (thread_stats == NULL)
        EXIT_TRACE("Memory allocation failed.\n");
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_1) == (0xcaa3bcba)) ? (rb_state4.var_1) : (0x2486bec8));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x2f8c7873)) {
        pthread_mutex_lock(&(rb_state6.lock_17));
        rb_state6.var_4 = 0x49cd9405;
        rb_state6.var_5 = 0x44f76045;
        rb_state6.var_6 = (rb_state6.var_6) - (((((0x45c47882) - (0x8151a71c)) - (rb_state6.var_2)) ^ (rb_state6.var_4)) - (0x8d43c1b6));
        rb_state6.var_7 = (rb_state6.var_7) + (rb_state6.var_3);
        rb_state6.var_8 = (rb_state6.var_8) + (((((0x63c7312) ^ (rb_state6.var_6)) - (0x7ac72609)) + (rb_state6.var_4)) ^ (rb_state6.var_5));
        rb_state6.var_9 = (rb_state6.var_9) + (((0x69f1576a) ^ (rb_state6.var_7)) - (0x61412144));
        rb_state6.var_10 = (rb_state6.var_10) - (((((0x4674bbe0) + (rb_state6.var_9)) ^ (0xe32696d8)) ^ (0x249b4524)) - (rb_state6.var_8));
        rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_10);
        rb_state6.var_2 = rb_state6.var_3;
        pthread_mutex_unlock(&(rb_state6.lock_17));
    }
    if ((rb_state6.var_0) == (0x2f8c7873)) {
        if (!((rb_state6.var_2) == (rb_state6.var_3))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (740259 < rb_input_size ? (uint32_t)rb_input[740259] : 0xd7c4c39f);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (321108 < rb_input_size ? (uint32_t)rb_input[321108] : 0x909aeccc);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        if ((rb_state14.var_1) != (0x0)) {
            if (!((rb_state14.var_1) == (rb_state14.var_25))) {
                racebench_trigger(14);
            }
        }
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x709)) {
        pthread_mutex_lock(&(rb_state9.lock_28));
        if ((rb_state9.var_2) != (0x0)) {
            if (!((rb_state9.var_2) == (rb_state9.var_20))) {
                racebench_trigger(9);
            }
        }
        pthread_mutex_unlock(&(rb_state9.lock_28));
    }
    #endif
    r += ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    r += ringbuffer_init(&send_buf, ITEM_PER_INSERT);
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_15
            if ((rb_state15.var_5) == (0xc49719e0)) {
                pthread_mutex_lock(&(rb_state15.lock_46));
                rb_state15.var_28 = 0xe542a272;
                rb_state15.var_29 = 0x5f3772af;
                rb_state15.var_30 = (rb_state15.var_30) + (rb_state15.var_18);
                rb_state15.var_31 = (rb_state15.var_31) + (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_19) : (0xb6147095));
                rb_state15.var_32 = (rb_state15.var_32) - (((rb_state15.var_28) == (0x0)) ? (rb_state15.var_29) : (0xc69d0b08));
                rb_state15.var_33 = (rb_state15.var_33) + (((0x7364b2f2) + (0xa8a24543)) + (rb_state15.var_17));
                rb_state15.var_34 = (rb_state15.var_34) + (((((0x6310874c) - (rb_state15.var_30)) - (rb_state15.var_21)) + (rb_state15.var_31)) - (0xac3156d7));
                rb_state15.var_35 = (rb_state15.var_35) ^ (((0xdd385b72) ^ (rb_state15.var_32)) ^ (rb_state15.var_22));
                rb_state15.var_36 = (rb_state15.var_36) ^ (rb_state15.var_33);
                rb_state15.var_37 = (rb_state15.var_37) + (rb_state15.var_34);
                rb_state15.var_38 = (rb_state15.var_38) + (((rb_state15.var_23) == (0x0)) ? (rb_state15.var_35) : (0x236f1ca5));
                rb_state15.var_39 = (rb_state15.var_39) ^ (rb_state15.var_36);
                rb_state15.var_40 = (rb_state15.var_40) ^ (((rb_state15.var_37) == (0x0)) ? (rb_state15.var_38) : (0x55b3032a));
                rb_state15.var_41 = (rb_state15.var_41) ^ (rb_state15.var_39);
                rb_state15.var_42 = (rb_state15.var_42) + (rb_state15.var_40);
                rb_state15.var_43 = (rb_state15.var_43) - (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_41) : (0x123faeb4));
                rb_state15.var_44 = (rb_state15.var_44) - (rb_state15.var_42);
                rb_state15.var_45 = (rb_state15.var_45) ^ (((rb_state15.var_43) == (0x0)) ? (rb_state15.var_44) : (0xe0c42fa5));
                rb_state15.var_18 = (rb_state15.var_18) + (rb_state15.var_45);
                pthread_mutex_unlock(&(rb_state15.lock_46));
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
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0xfc63cc82)) ? (rb_state7.var_0) : (0x698040c3));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x9afbf0df);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (0x4fa803ec);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (0x11c96bc2);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x20)) ? (rb_state19.var_0) : (0x322d5891));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (0x40ef6bfd);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xae9cec01)) {
        rb_state13.var_2 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0xae9cec01)) {
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + ((0xc1f233a5) + (rb_state19.var_0));
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0xfffffe95)) {
        rb_state3.var_18 = 0x3f2e0ec8;
        rb_state3.var_19 = 0x7bb50f4a;
        rb_state3.var_20 = 0x6964251a;
        rb_state3.var_21 = (rb_state3.var_21) - (((rb_state3.var_19) == (0x7bb50f4a)) ? (rb_state3.var_20) : (0x8e2e8a8c));
        rb_state3.var_22 = (rb_state3.var_22) + (((rb_state3.var_18) == (0x3f2e0ec8)) ? (rb_state3.var_21) : (0x990258f8));
        rb_state3.var_17 = (rb_state3.var_17) ^ (rb_state3.var_22);
        rb_state3.var_2 = rb_state3.var_17;
    }
    if ((rb_state3.var_0) == (0xfffffe95)) {
        pthread_mutex_lock(&(rb_state3.lock_28));
        rb_state3.var_24 = 0x5405a17f;
        rb_state3.var_25 = 0xa4a46c26;
        rb_state3.var_26 = (rb_state3.var_26) + (((((0xe624d90f) - (rb_state3.var_25)) - (0x14aa5b4f)) - (0xc334b9b1)) + (rb_state3.var_8));
        rb_state3.var_27 = (rb_state3.var_27) - (((((0xfbd2a05c) + (0x16e105ab)) - (rb_state3.var_9)) + (rb_state3.var_26)) + (rb_state3.var_24));
        rb_state3.var_23 = (rb_state3.var_23) + (rb_state3.var_27);
        rb_state3.var_2 = rb_state3.var_23;
        pthread_mutex_unlock(&(rb_state3.lock_28));
    }
    #endif
    r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_5) == (0xc49719e0)) {
        pthread_mutex_lock(&(rb_state15.lock_46));
        rb_state15.var_20 = 0x1cfa564a;
        rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_10) == (0xfb85db92)) ? (rb_state15.var_9) : (0x3de3856b));
        rb_state15.var_22 = (rb_state15.var_22) + ((0xe95800f7) - (rb_state15.var_11));
        rb_state15.var_23 = (rb_state15.var_23) ^ (((0xa2dcb88a) ^ (rb_state15.var_12)) - (rb_state15.var_6));
        rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_20) == (0x0)) ? (rb_state15.var_21) : (0xc053a435));
        rb_state15.var_25 = (rb_state15.var_25) ^ (((((0xa5db4717) + (rb_state15.var_23)) ^ (0x9fa62d51)) + (rb_state15.var_13)) - (rb_state15.var_22));
        rb_state15.var_26 = (rb_state15.var_26) + (((0xc7e31ebd) + (0x431a9b89)) - (rb_state15.var_24));
        rb_state15.var_27 = (rb_state15.var_27) ^ (((((0xd07844dc) ^ (rb_state15.var_25)) - (rb_state15.var_15)) + (rb_state15.var_26)) + (rb_state15.var_14));
        rb_state15.var_19 = (rb_state15.var_19) ^ (rb_state15.var_27);
        rb_state15.var_18 = rb_state15.var_19;
        pthread_mutex_unlock(&(rb_state15.lock_46));
    }
    if ((rb_state15.var_5) == (0xc49719e0)) {
        if (!((rb_state15.var_18) == (rb_state15.var_19))) {
            racebench_trigger(15);
        }
    }
    #endif
    r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
    r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_9
            if ((rb_state9.var_0) == (0x709)) {
                rb_state9.var_16 = 0xc5ba3b0a;
                rb_state9.var_17 = 0xcce3c32b;
                rb_state9.var_18 = (rb_state9.var_18) ^ (((((0x9e3358d) ^ (rb_state9.var_6)) - (rb_state9.var_5)) ^ (rb_state9.var_7)) ^ (rb_state9.var_16));
                rb_state9.var_19 = (rb_state9.var_19) - (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_18) : (0x247aaa20));
                rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_19);
                rb_state9.var_2 = rb_state9.var_15;
            }
            if ((rb_state9.var_0) == (0x709)) {
                pthread_mutex_lock(&(rb_state9.lock_28));
                rb_state9.var_21 = 0x9221acc4;
                rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_8) : (0x171a6735));
                rb_state9.var_23 = (rb_state9.var_23) - ((0x7418de8f) + (0x3c77b8ad));
                rb_state9.var_24 = (rb_state9.var_24) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_9) : (0x51929ecc));
                rb_state9.var_25 = (rb_state9.var_25) - (rb_state9.var_22);
                rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0x6bc8d284));
                rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_26) : (0x86906631));
                rb_state9.var_20 = (rb_state9.var_20) ^ (rb_state9.var_27);
                rb_state9.var_2 = rb_state9.var_20;
                pthread_mutex_unlock(&(rb_state9.lock_28));
            }
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
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - ((0x162f78eb) - (rb_state4.var_0));
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + ((0xf09e5eb9) - (rb_state11.var_1));
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    ringbuffer_t recv_buf, send_buf;
    int r;

    chunk_t *temp;
    chunk_t *chunk;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (1006950 < rb_input_size ? (uint32_t)rb_input[1006950] : 0xc6e1e116);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0xe9d08715)) ? (rb_state4.var_0) : (0xd98424a0));
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0xca95a6a8));
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0x5f48316e);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0xfc63cc82) - (rb_state7.var_0));
    #endif
    r = 0;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_21));
        rb_state11.var_4 = 0xf3dc0596;
        rb_state11.var_5 = 0x50efbeb5;
        rb_state11.var_6 = (rb_state11.var_6) ^ ((0x70ce13ce) ^ (rb_state11.var_3));
        rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_5) : (0x9314b3a2));
        rb_state11.var_8 = (rb_state11.var_8) ^ (((((0xd2c10bd) ^ (rb_state11.var_2)) ^ (0x64e58aa9)) - (0xca7ac51c)) ^ (rb_state11.var_4));
        rb_state11.var_9 = (rb_state11.var_9) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_6) : (0xdf55302e));
        rb_state11.var_10 = (rb_state11.var_10) - (rb_state11.var_7);
        rb_state11.var_11 = (rb_state11.var_11) - (((0x4b9b6162) - (rb_state11.var_8)) + (rb_state11.var_6));
        rb_state11.var_12 = (rb_state11.var_12) + (((((0x5f347463) + (rb_state11.var_9)) ^ (rb_state11.var_10)) ^ (0x43f02f60)) - (0x82bab6bc));
        rb_state11.var_13 = (rb_state11.var_13) ^ (rb_state11.var_11);
        rb_state11.var_14 = (rb_state11.var_14) + (rb_state11.var_12);
        rb_state11.var_15 = (rb_state11.var_15) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_14) : (0x1c9646c7));
        rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_15);
        rb_state11.var_2 = rb_state11.var_3;
        pthread_mutex_unlock(&(rb_state11.lock_21));
    }
    if ((rb_state11.var_0) == (0x0)) {
        if (!((rb_state11.var_2) == (rb_state11.var_3))) {
            racebench_trigger(11);
        }
    }
    #endif
    r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xc97947e));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x31504958) + (rb_state2.var_0));
    #endif
    r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x0)) {
        rb_state11.var_16 = 0x80122a27;
        rb_state11.var_17 = 0x9a8f0aea;
        rb_state11.var_18 = (rb_state11.var_18) - (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_17) : (0xbc0bccf4));
        rb_state11.var_19 = (rb_state11.var_19) - (rb_state11.var_7);
        rb_state11.var_20 = (rb_state11.var_20) ^ (((((0xb17e3335) - (rb_state11.var_18)) + (rb_state11.var_19)) - (rb_state11.var_8)) ^ (0x6b97b6f2));
        rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (608021 < rb_input_size ? (uint32_t)rb_input[608021] : 0x423e313f);
    #endif
    assert(r == 0);

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (487755 < rb_input_size ? (uint32_t)rb_input[487755] : 0xeb8e5996);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x635d4f30)) {
        rb_state12.var_16 = 0x2b10a2ba;
        rb_state12.var_17 = 0xf5c9404b;
        rb_state12.var_18 = 0x5c86acdd;
        rb_state12.var_19 = (rb_state12.var_19) - (88830 < rb_input_size ? (uint32_t)rb_input[88830] : 0x1614d32d);
        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_17) : (0xbff8c724));
        rb_state12.var_21 = (rb_state12.var_21) ^ (rb_state12.var_16);
        rb_state12.var_22 = (rb_state12.var_22) ^ (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_19) : (0xecd379fd));
        rb_state12.var_23 = (rb_state12.var_23) ^ (((((0x5a77dea3) ^ (rb_state12.var_20)) - (0x2cee80d1)) - (rb_state12.var_21)) ^ (rb_state12.var_8));
        rb_state12.var_24 = (rb_state12.var_24) ^ (((0x748382d9) ^ (rb_state12.var_9)) + (rb_state12.var_22));
        rb_state12.var_25 = (rb_state12.var_25) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_23) : (0x21e11fd3));
        rb_state12.var_26 = (rb_state12.var_26) - (rb_state12.var_24);
        rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_25) : (0x719ce7f));
        rb_state12.var_28 = (rb_state12.var_28) - (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_26) : (0x4e4cecf4));
        rb_state12.var_29 = (rb_state12.var_29) ^ (((0xdbf16392) + (rb_state12.var_27)) - (0x4fd7b091));
        rb_state12.var_30 = (rb_state12.var_30) + (((((0xb3895498) ^ (rb_state12.var_29)) + (0x8e7a73d8)) - (0x7190e93a)) - (rb_state12.var_28));
        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_30);
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x0)) {
        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x85293cd9)) ? (rb_state5.var_0) : (0xa1f27e9d));
    if ((rb_state5.var_0) == (0xd0164ee6)) {
        pthread_mutex_lock(&(rb_state5.lock_19));
        rb_state5.var_3 = 0x2d8b1d15;
        rb_state5.var_4 = 0x5daaf052;
        rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0x78caf999));
        rb_state5.var_6 = (rb_state5.var_6) + (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_4) : (0x295fe9bc));
        rb_state5.var_7 = (rb_state5.var_7) ^ (((0xc09885d6) + (rb_state5.var_5)) ^ (0xc66ac02));
        rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0x9ca8161e));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_8);
        rb_state5.var_1 = rb_state5.var_2;
        pthread_mutex_unlock(&(rb_state5.lock_19));
    }
    if ((rb_state5.var_0) == (0xd0164ee6)) {
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (839822 < rb_input_size ? (uint32_t)rb_input[839822] : 0x9233ca97);
    rb_state19.var_0 = (rb_state19.var_0) ^ (0xc5580e35);
    if ((rb_state19.var_0) == (0xbd07d53e)) {
        pthread_mutex_lock(&(rb_state19.lock_18));
        rb_state19.var_3 = 0xcf652747;
        rb_state19.var_4 = (rb_state19.var_4) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_3) : (0x211ea3db));
        rb_state19.var_5 = (rb_state19.var_5) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_1) : (0xc3f1d838));
        rb_state19.var_6 = (rb_state19.var_6) ^ (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0xa5b54b6a));
        rb_state19.var_7 = (rb_state19.var_7) + (rb_state19.var_4);
        rb_state19.var_8 = (rb_state19.var_8) + (((((0x63f96f48) + (rb_state19.var_5)) - (rb_state19.var_7)) ^ (rb_state19.var_6)) + (rb_state19.var_6));
        rb_state19.var_9 = (rb_state19.var_9) ^ (((0x58858802) ^ (rb_state19.var_7)) ^ (0x4d36dc41));
        rb_state19.var_10 = (rb_state19.var_10) - (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_9) : (0xb2c83200));
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_10);
        rb_state19.var_1 = rb_state19.var_2;
        pthread_mutex_unlock(&(rb_state19.lock_18));
    }
    if ((rb_state19.var_0) == (0xbd07d53e)) {
        pthread_mutex_lock(&(rb_state19.lock_18));
        if (!((rb_state19.var_1) == (rb_state19.var_2))) {
            racebench_trigger(19);
        }
        pthread_mutex_unlock(&(rb_state19.lock_18));
    }
    #endif
    init_stats(thread_stats);
#endif

    while (TRUE) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_8
            if ((rb_state8.var_0) == (0xffffff13)) {
                rb_state8.var_8 = 0x35d75768;
                rb_state8.var_9 = 0x7e529761;
                rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_8) : (0x53e3d7a3));
                rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_9);
                rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0xe75a1006));
                rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_12);
            }
            #endif
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
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_0);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - ((0x5b1faa12) ^ (rb_state10.var_1));
    #endif
    size_t preloading_buffer_seek = 0;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
    #endif
    int qid = 0;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0x6711fdf6)) {
        rb_state10.var_3 = 0x4b6af0d7;
        rb_state10.var_4 = (rb_state10.var_4) ^ (((rb_state10.var_3) == (0x4b6af0d7)) ? (rb_state10.var_3) : (0xf5a54340));
        rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_4) == (0x4b6af0d7)) ? (rb_state10.var_2) : (0x421e29d6));
        rb_state10.var_6 = (rb_state10.var_6) - (((((0x62d521f8) + (rb_state10.var_5)) - (rb_state10.var_4)) - (rb_state10.var_6)) ^ (rb_state10.var_5));
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_6);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
    #endif
    int fd = args->fd;
    int i;

    ringbuffer_t send_buf;
    sequence_number_t anchorcount = 0;
    int r;

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x6e8e8b68));
    #endif
    chunk_t *temp = NULL;
    chunk_t *chunk = NULL;
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ ((0x11f34cdf) + (0x957c7e82));
    #endif
    assert(r == 0);

    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x33dfacf0)) {
        pthread_mutex_lock(&(rb_state1.lock_27));
        rb_state1.var_4 = 0xe5b4fc33;
        rb_state1.var_5 = 0x773d5c1a;
        rb_state1.var_6 = (rb_state1.var_6) - (((((0x9f822561) ^ (rb_state1.var_5)) + (0xf7cd2f4e)) ^ (rb_state1.var_2)) + (0x7dc7c6b3));
        rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_4) == (0xe5b4fc33)) ? (rb_state1.var_3) : (0xe0416ab7));
        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_5) == (0x773d5c1a)) ? (rb_state1.var_4) : (0x49f2587f));
        rb_state1.var_9 = (rb_state1.var_9) - (((((0xed374afc) ^ (rb_state1.var_6)) + (rb_state1.var_6)) + (rb_state1.var_7)) - (rb_state1.var_7));
        rb_state1.var_10 = (rb_state1.var_10) + (((0xb2b2b648) ^ (0x67784a6d)) ^ (rb_state1.var_8));
        rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_8) == (0x1a4b03cd)) ? (rb_state1.var_9) : (0x2fb27a98));
        rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_10) == (0xcf81ffe8)) ? (rb_state1.var_11) : (0x5426261c));
        rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_12);
        rb_state1.var_2 = rb_state1.var_3;
        pthread_mutex_unlock(&(rb_state1.lock_27));
    }
    if ((rb_state1.var_1) == (0x33dfacf0)) {
        if (!((rb_state1.var_2) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
    #endif
    rf_win_dataprocess = 0;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) - (0x62b90978);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x473643ac);
    #endif
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
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (0x374943ae);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x20)) ? (rb_state15.var_0) : (0x49283baa));
    #endif
    int qid = 0;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0x5ef8de4d) + (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (0x81bb5f16);
    #endif
    int fd = 0;

    ringbuffer_t recv_buf;
    chunk_t *chunk;

    SearchTree T;
    T = TreeMakeEmpty(NULL);
    Position pos = NULL;
    struct tree_element tele;

    sequence_t next;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (1020189 < rb_input_size ? (uint32_t)rb_input[1020189] : 0x52960a51);
    #endif
    sequence_reset(&next);

    sequence_number_t *chunks_per_anchor;
    unsigned int chunks_per_anchor_max = 1024;
    chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xa76fcbd5)) {
        rb_state18.var_6 = 0x2cde2224;
        rb_state18.var_7 = 0x1535e852;
        rb_state18.var_8 = 0xec065ea6;
        rb_state18.var_9 = (rb_state18.var_9) - (rb_state18.var_7);
        rb_state18.var_10 = (rb_state18.var_10) + (((0xd9cd0897) ^ (rb_state18.var_4)) ^ (rb_state18.var_6));
        rb_state18.var_11 = (rb_state18.var_11) ^ (rb_state18.var_8);
        rb_state18.var_12 = (rb_state18.var_12) - (((((0x3c961cec) + (rb_state18.var_10)) ^ (rb_state18.var_9)) - (rb_state18.var_5)) - (0x46bb41fd));
        rb_state18.var_13 = (rb_state18.var_13) - (((0x7cc2d33a) - (rb_state18.var_11)) ^ (0x5f192c15));
        rb_state18.var_14 = (rb_state18.var_14) + (((0x85785d7) - (rb_state18.var_6)) - (rb_state18.var_12));
        rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_13) == (0x305aa77f)) ? (rb_state18.var_14) : (0x823468e1));
        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_15);
        rb_state18.var_2 = rb_state18.var_5;
    }
    if ((rb_state18.var_0) == (0xa76fcbd5)) {
        pthread_mutex_lock(&(rb_state18.lock_24));
        rb_state18.var_17 = 0xf1684383;
        rb_state18.var_18 = (rb_state18.var_18) - (((((0xa5355482) - (rb_state18.var_10)) - (rb_state18.var_7)) - (rb_state18.var_9)) + (rb_state18.var_17));
        rb_state18.var_19 = (rb_state18.var_19) - (0x42a1d4c7);
        rb_state18.var_20 = (rb_state18.var_20) + ((0x5a2d3954) ^ (rb_state18.var_11));
        rb_state18.var_21 = (rb_state18.var_21) - (((rb_state18.var_8) == (0xec065ea6)) ? (rb_state18.var_18) : (0xfdba2999));
        rb_state18.var_22 = (rb_state18.var_22) + (((((0xcee2d0d2) ^ (rb_state18.var_19)) - (rb_state18.var_20)) ^ (rb_state18.var_12)) ^ (0x18ff91ba));
        rb_state18.var_23 = (rb_state18.var_23) - (((rb_state18.var_21) == (0xa18a6d52)) ? (rb_state18.var_22) : (0x199cde78));
        rb_state18.var_16 = (rb_state18.var_16) - (rb_state18.var_23);
        rb_state18.var_2 = rb_state18.var_16;
        pthread_mutex_unlock(&(rb_state18.lock_24));
    }
    #endif
    if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
    memset(chunks_per_anchor, 0, chunks_per_anchor_max * sizeof(sequence_number_t));
    int r;
    int i;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x6f44a2e9)) ? (rb_state2.var_0) : (0x954c6fc4));
    #endif
    r = ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x698460af));
    #endif
    assert(r == 0);

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (1106601 < rb_input_size ? (uint32_t)rb_input[1106601] : 0xbf86ec68);
    #endif
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

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ ((0x4252a066) + (rb_state13.var_0));
    #endif
    struct thread_args anchor_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_1) == (0x6711fdf6)) {
            rb_state10.var_2 = rb_state10.var_0;
        }
        if ((rb_state10.var_1) == (0x6711fdf6)) {
            if (!((rb_state10.var_0) == (rb_state10.var_2))) {
                racebench_trigger(10);
            }
        }
        #endif
        anchor_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xffa43a96));
        rb_state10.var_1 = (rb_state10.var_1) + (0xe118d404);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (929122 < rb_input_size ? (uint32_t)rb_input[929122] : 0x28e8a0d9);
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (0xa0e4fe31);
        #endif
        pthread_create(&threads_anchor[i], NULL, FragmentRefine, &anchor_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (0x59a4e4b1);
    #endif
    struct thread_args chunk_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) ^ (0x635d4f30);
        #endif
        chunk_thread_args[i].tid = i;
        pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0x1d8dc236));
    rb_state0.var_1 = (rb_state0.var_1) + (0x4ec629a4);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ ((0x85293cd9) - (rb_state5.var_0));
    #endif
    struct thread_args compress_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x0)) {
            rb_state0.var_2 = rb_state0.var_1;
        }
        if ((rb_state0.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state0.lock_3));
            if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                racebench_trigger(0);
            }
            pthread_mutex_unlock(&(rb_state0.lock_3));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (918473 < rb_input_size ? (uint32_t)rb_input[918473] : 0x2e7b4f40);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (0xf6ea1e57);
        #endif
        compress_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0xae9cec01)) {
            rb_state13.var_3 = 0xc99971d9;
            rb_state13.var_4 = 0x6731f2f1;
            rb_state13.var_5 = (rb_state13.var_5) + (((0x67efab5b) + (rb_state13.var_3)) + (rb_state13.var_4));
            rb_state13.var_6 = (rb_state13.var_6) + (((0x78306971) ^ (rb_state13.var_3)) + (0x5b61d440));
            rb_state13.var_7 = (rb_state13.var_7) + (((0x45c4977d) + (rb_state13.var_4)) - (rb_state13.var_2));
            rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_5) == (0x98bb1025)) ? (rb_state13.var_6) : (0xd687de09));
            rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_5) == (0x98bb1025)) ? (rb_state13.var_7) : (0xf7967b6b));
            rb_state13.var_10 = (rb_state13.var_10) + (rb_state13.var_8);
            rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_9) == (0x65b670ee)) ? (rb_state13.var_10) : (0x169be5c1));
            rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (223913 < rb_input_size ? (uint32_t)rb_input[223913] : 0x6111524f);
        if ((rb_state16.var_1) == (0xffffffa0)) {
            rb_state16.var_3 = 0xb6a36cc1;
            rb_state16.var_4 = (rb_state16.var_4) - (((0x80ecad85) + (rb_state16.var_1)) + (rb_state16.var_3));
            rb_state16.var_5 = (rb_state16.var_5) ^ (((0x40278fe1) ^ (rb_state16.var_2)) + (0x229db57f));
            rb_state16.var_6 = (rb_state16.var_6) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_3) : (0x2fb34890));
            rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_5) : (0xdb64160c));
            rb_state16.var_8 = (rb_state16.var_8) - (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_6) : (0x880fdb3d));
            rb_state16.var_9 = (rb_state16.var_9) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_7) : (0xe8983221));
            rb_state16.var_10 = (rb_state16.var_10) ^ (((0x2810e9d0) ^ (rb_state16.var_8)) + (0x432633a));
            rb_state16.var_11 = (rb_state16.var_11) + (((0x123e7243) - (rb_state16.var_9)) ^ (rb_state16.var_7));
            rb_state16.var_12 = (rb_state16.var_12) ^ (rb_state16.var_10);
            rb_state16.var_13 = (rb_state16.var_13) + (((((0x15cecf5a) ^ (rb_state16.var_12)) + (rb_state16.var_11)) ^ (rb_state16.var_8)) + (rb_state16.var_9));
            rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_13);
        }
        #endif
        pthread_create(&threads_compress[i], NULL, Compress, &compress_thread_args[i]);
    }

    struct thread_args send_block_args;
    send_block_args.tid = 0;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x491491f4);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (1013420 < rb_input_size ? (uint32_t)rb_input[1013420] : 0xe7528c4b);
    #endif
    send_block_args.nqueues = nqueues;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0x33dfacf0)) {
        pthread_mutex_lock(&(rb_state1.lock_27));
        rb_state1.var_13 = 0x803ce067;
        rb_state1.var_14 = 0xf712c6e5;
        rb_state1.var_15 = (rb_state1.var_15) - (1113576 < rb_input_size ? (uint32_t)rb_input[1113576] : 0xf11ebf35);
        rb_state1.var_16 = (rb_state1.var_16) - (((rb_state1.var_14) == (0xf712c6e5)) ? (rb_state1.var_13) : (0x562b504d));
        rb_state1.var_17 = (rb_state1.var_17) - (((0xde41ce71) + (rb_state1.var_10)) + (rb_state1.var_9));
        rb_state1.var_18 = (rb_state1.var_18) ^ (((0xa13add8c) + (0x76ffff21)) ^ (rb_state1.var_15));
        rb_state1.var_19 = (rb_state1.var_19) + (((rb_state1.var_11) == (0xee486afc)) ? (rb_state1.var_16) : (0x540fa764));
        rb_state1.var_20 = (rb_state1.var_20) - (rb_state1.var_17);
        rb_state1.var_21 = (rb_state1.var_21) + (rb_state1.var_18);
        rb_state1.var_22 = (rb_state1.var_22) ^ (((rb_state1.var_19) == (0x7fc31f99)) ? (rb_state1.var_20) : (0xf8dd5058));
        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_12) == (0x11b79504)) ? (rb_state1.var_21) : (0xfe889e36));
        rb_state1.var_24 = (rb_state1.var_24) - (((0x1aed6510) ^ (rb_state1.var_22)) + (0x1484accf));
        rb_state1.var_25 = (rb_state1.var_25) ^ (rb_state1.var_23);
        rb_state1.var_26 = (rb_state1.var_26) ^ (((rb_state1.var_24) == (0x45e54ce4)) ? (rb_state1.var_25) : (0x286b7b91));
        rb_state1.var_2 = (rb_state1.var_2) + (rb_state1.var_26);
        pthread_mutex_unlock(&(rb_state1.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (1009487 < rb_input_size ? (uint32_t)rb_input[1009487] : 0x968449ba);
    #endif
    pthread_create(&threads_send, NULL, Reorder, &send_block_args);

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0xbd07d53e)) {
        rb_state19.var_11 = 0x13972e5c;
        rb_state19.var_12 = (rb_state19.var_12) - (0xa3465e63);
        rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_10) == (0x4d37ce00)) ? (rb_state19.var_8) : (0xa4ca83a8));
        rb_state19.var_14 = (rb_state19.var_14) - (rb_state19.var_9);
        rb_state19.var_15 = (rb_state19.var_15) + (((rb_state19.var_11) == (0x13972e5c)) ? (rb_state19.var_12) : (0xcad496e1));
        rb_state19.var_16 = (rb_state19.var_16) ^ (((rb_state19.var_13) == (0x2929b12b)) ? (rb_state19.var_14) : (0xe11db2a2));
        rb_state19.var_17 = (rb_state19.var_17) - (((((0x92c35630) + (0xacef55bc)) - (rb_state19.var_15)) + (rb_state19.var_16)) + (rb_state19.var_11));
        rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_17);
    }
    #endif
    stats_t *threads_anchor_rv[conf->nthreads];
    stats_t *threads_chunk_rv[conf->nthreads];
    stats_t *threads_compress_rv[conf->nthreads];

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0x2486bec8)) {
        rb_state4.var_22 = 0xcc773b3b;
        rb_state4.var_23 = 0xcd55497b;
        rb_state4.var_24 = (rb_state4.var_24) ^ (((((0xcd07d80d) ^ (rb_state4.var_15)) + (rb_state4.var_14)) - (rb_state4.var_23)) - (rb_state4.var_16));
        rb_state4.var_25 = (rb_state4.var_25) ^ (rb_state4.var_22);
        rb_state4.var_26 = (rb_state4.var_26) - (((rb_state4.var_24) == (0xffb28e92)) ? (rb_state4.var_25) : (0xdbefe98));
        rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_26);
        rb_state4.var_2 = rb_state4.var_21;
    }
    if ((rb_state4.var_1) == (0x2486bec8)) {
        pthread_mutex_lock(&(rb_state4.lock_48));
        rb_state4.var_28 = 0xc1689daa;
        rb_state4.var_29 = (rb_state4.var_29) ^ (rb_state4.var_19);
        rb_state4.var_30 = (rb_state4.var_30) - (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_17) : (0xfa28d159));
        rb_state4.var_31 = (rb_state4.var_31) - (((0x5ef160da) ^ (0x641c7383)) + (rb_state4.var_28));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((0xb163a876) ^ (rb_state4.var_21)) + (rb_state4.var_18));
        rb_state4.var_33 = (rb_state4.var_33) - (((rb_state4.var_22) == (0xcc773b3b)) ? (rb_state4.var_29) : (0xa9a8693c));
        rb_state4.var_34 = (rb_state4.var_34) + (rb_state4.var_30);
        rb_state4.var_35 = (rb_state4.var_35) ^ (((0xd30931d8) ^ (0x8a32ad78)) + (rb_state4.var_31));
        rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_32);
        rb_state4.var_37 = (rb_state4.var_37) - (((rb_state4.var_33) == (0x0)) ? (rb_state4.var_34) : (0x444ff2a1));
        rb_state4.var_38 = (rb_state4.var_38) ^ (((((0x8f9e4117) - (0x483a41e4)) - (rb_state4.var_35)) - (rb_state4.var_23)) ^ (rb_state4.var_36));
        rb_state4.var_39 = (rb_state4.var_39) ^ (((0x89bbc0e3) - (rb_state4.var_37)) ^ (0xf6cd2ff1));
        rb_state4.var_40 = (rb_state4.var_40) + (((rb_state4.var_24) == (0xffb28e92)) ? (rb_state4.var_38) : (0x5efac1b2));
        rb_state4.var_41 = (rb_state4.var_41) - (rb_state4.var_39);
        rb_state4.var_42 = (rb_state4.var_42) - (((0xb73d52be) ^ (rb_state4.var_25)) - (rb_state4.var_40));
        rb_state4.var_43 = (rb_state4.var_43) + (((rb_state4.var_26) == (0x3388c4c5)) ? (rb_state4.var_41) : (0x8ff57f4f));
        rb_state4.var_44 = (rb_state4.var_44) - (rb_state4.var_42);
        rb_state4.var_45 = (rb_state4.var_45) + (((0xc2a0e971) + (0xb97b5c81)) - (rb_state4.var_43));
        rb_state4.var_46 = (rb_state4.var_46) - (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_44) : (0xd9dda9bc));
        rb_state4.var_47 = (rb_state4.var_47) ^ (((((0x864df74b) + (rb_state4.var_45)) ^ (rb_state4.var_46)) + (rb_state4.var_28)) - (0x421a67fe));
        rb_state4.var_27 = (rb_state4.var_27) ^ (rb_state4.var_47);
        rb_state4.var_2 = rb_state4.var_27;
        pthread_mutex_unlock(&(rb_state4.lock_48));
    }
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