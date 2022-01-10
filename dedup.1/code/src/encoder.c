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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x1b3c13a2));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_14 = 0x29be9ae0;
        rb_state15.var_15 = 0x37e8629d;
        rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_8) : (0x1c9d591c));
        rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_14) == (0x0)) ? (rb_state15.var_16) : (0x2e4b5eaa));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_17);
    }
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (0x2bee0d2e);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x5a605ec0)) {
        rb_state5.var_3 = 0x6fee9784;
        rb_state5.var_4 = 0x2d32a457;
        rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_4) : (0xa479456));
        rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_3);
        rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_5) : (0xe4b87d24));
        rb_state5.var_8 = (rb_state5.var_8) + (((((0x6770307b) + (rb_state5.var_7)) - (rb_state5.var_6)) + (0x80d0ccd)) + (rb_state5.var_4));
        rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_18));
        rb_state15.var_3 = 0x14252936;
        rb_state15.var_4 = (rb_state15.var_4) + (112400 < rb_input_size ? (uint32_t)rb_input[112400] : 0xe26c6d65);
        rb_state15.var_5 = (rb_state15.var_5) - (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_2) : (0xfd2cb067));
        rb_state15.var_6 = (rb_state15.var_6) ^ (((rb_state15.var_3) == (0x14252936)) ? (rb_state15.var_4) : (0x79a05045));
        rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_3) == (0x14252936)) ? (rb_state15.var_5) : (0xb2153ddd));
        rb_state15.var_8 = (rb_state15.var_8) ^ (((0x18500872) - (rb_state15.var_4)) + (rb_state15.var_6));
        rb_state15.var_9 = (rb_state15.var_9) - (((0x5c52da35) - (rb_state15.var_7)) + (rb_state15.var_5));
        rb_state15.var_10 = (rb_state15.var_10) ^ (rb_state15.var_8);
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_6) == (0x8e)) ? (rb_state15.var_9) : (0x5c54d672));
        rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_10);
        rb_state15.var_13 = (rb_state15.var_13) - (((((0x1dc5fd01) - (0x509575c8)) + (rb_state15.var_11)) - (rb_state15.var_7)) - (rb_state15.var_12));
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_13);
        rb_state15.var_1 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_18));
    }
    if ((rb_state15.var_0) == (0x0)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
    }
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x5a605ec0)) {
        pthread_mutex_lock(&(rb_state5.lock_9));
        rb_state5.var_2 = rb_state5.var_1;
        pthread_mutex_unlock(&(rb_state5.lock_9));
    }
    if ((rb_state5.var_0) == (0x5a605ec0)) {
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (((rb_state13.var_0) == (0x96)) ? (rb_state13.var_0) : (0x19e6d92));
    #endif
    if (thread_stats == NULL)
        EXIT_TRACE("Memory allocation failed.\n");
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xfe61926e)) {
        pthread_mutex_lock(&(rb_state13.lock_27));
        rb_state13.var_3 = 0x3ca8d0d3;
        rb_state13.var_4 = 0x23081c8c;
        rb_state13.var_5 = (rb_state13.var_5) + (((0xd9310ca5) ^ (rb_state13.var_2)) + (rb_state13.var_1));
        rb_state13.var_6 = (rb_state13.var_6) ^ (rb_state13.var_3);
        rb_state13.var_7 = (rb_state13.var_7) ^ (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_5) : (0xcdff05ef));
        rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_6);
        rb_state13.var_9 = (rb_state13.var_9) + (rb_state13.var_7);
        rb_state13.var_10 = (rb_state13.var_10) - (rb_state13.var_8);
        rb_state13.var_11 = (rb_state13.var_11) ^ (((0x8bd641e7) ^ (0x444300b)) - (rb_state13.var_9));
        rb_state13.var_12 = (rb_state13.var_12) + (((((0x2803c71b) ^ (rb_state13.var_10)) + (rb_state13.var_11)) ^ (rb_state13.var_4)) - (rb_state13.var_3));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_12);
        rb_state13.var_1 = rb_state13.var_2;
        pthread_mutex_unlock(&(rb_state13.lock_27));
    }
    if ((rb_state13.var_0) == (0xfe61926e)) {
        pthread_mutex_lock(&(rb_state13.lock_27));
        rb_state13.var_28 = !((rb_state13.var_1) == (rb_state13.var_2));
        pthread_mutex_unlock(&(rb_state13.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x31c2295e)) {
        rb_state16.var_2 = rb_state16.var_1;
    }
    if ((rb_state16.var_0) == (0x31c2295e)) {
        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
            racebench_trigger(16);
        }
    }
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    r += ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    r += ringbuffer_init(&send_buf, ITEM_PER_INSERT);
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xce51480a)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0xce51480a)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
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
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x4b185432)) ? (rb_state15.var_0) : (0xc8b214be));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xdd1628d0)) {
        rb_state4.var_12 = 0xb3e1fe;
        rb_state4.var_13 = (rb_state4.var_13) + (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_8) : (0xafd8ee8e));
        rb_state4.var_14 = (rb_state4.var_14) + (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_10) : (0x35d28d27));
        rb_state4.var_15 = (rb_state4.var_15) ^ (500771 < rb_input_size ? (uint32_t)rb_input[500771] : 0xc44fcb77);
        rb_state4.var_16 = (rb_state4.var_16) + (rb_state4.var_7);
        rb_state4.var_17 = (rb_state4.var_17) - (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_12) : (0xfb302b79));
        rb_state4.var_18 = (rb_state4.var_18) + (((0x2b47efbc) + (rb_state4.var_13)) - (rb_state4.var_6));
        rb_state4.var_19 = (rb_state4.var_19) - (rb_state4.var_13);
        rb_state4.var_20 = (rb_state4.var_20) ^ (((0x4eaf65ac) - (rb_state4.var_14)) - (0xb3358ae0));
        rb_state4.var_21 = (rb_state4.var_21) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0xd2112e4a));
        rb_state4.var_22 = (rb_state4.var_22) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_17) : (0x24818fbd));
        rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_18) : (0x73db0ffc));
        rb_state4.var_24 = (rb_state4.var_24) - (((((0x369829bc) - (0x1f522737)) + (rb_state4.var_20)) ^ (rb_state4.var_16)) ^ (rb_state4.var_19));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_21) : (0x58f59125));
        rb_state4.var_26 = (rb_state4.var_26) - (((0x31ee7fa) ^ (rb_state4.var_22)) ^ (rb_state4.var_18));
        rb_state4.var_27 = (rb_state4.var_27) + (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_23) : (0x8a3dd6e2));
        rb_state4.var_28 = (rb_state4.var_28) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_25) : (0x73a0916f));
        rb_state4.var_29 = (rb_state4.var_29) + (((((0x3eca9933) ^ (0x4e462375)) ^ (rb_state4.var_20)) + (rb_state4.var_26)) ^ (rb_state4.var_27));
        rb_state4.var_30 = (rb_state4.var_30) ^ (rb_state4.var_28);
        rb_state4.var_31 = (rb_state4.var_31) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_29) : (0x35e1c73f));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((((0xb00bafb7) - (rb_state4.var_31)) + (0x919305b2)) + (0x4158ffc5)) ^ (rb_state4.var_30));
        rb_state4.var_11 = (rb_state4.var_11) ^ (rb_state4.var_32);
        rb_state4.var_1 = rb_state4.var_11;
    }
    if ((rb_state4.var_0) == (0xdd1628d0)) {
        pthread_mutex_lock(&(rb_state4.lock_43));
        rb_state4.var_34 = 0x21dc76c5;
        rb_state4.var_35 = (rb_state4.var_35) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_23) : (0x3c28115a));
        rb_state4.var_36 = (rb_state4.var_36) ^ (rb_state4.var_22);
        rb_state4.var_37 = (rb_state4.var_37) ^ (15769 < rb_input_size ? (uint32_t)rb_input[15769] : 0xac1c1ed9);
        rb_state4.var_38 = (rb_state4.var_38) - (((rb_state4.var_34) == (0x0)) ? (rb_state4.var_35) : (0xfbcda560));
        rb_state4.var_39 = (rb_state4.var_39) + (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_36) : (0xf49ccea0));
        rb_state4.var_40 = (rb_state4.var_40) ^ (((0xff5b12c8) - (rb_state4.var_37)) + (0xd1a38aef));
        rb_state4.var_41 = (rb_state4.var_41) - (((rb_state4.var_38) == (0x0)) ? (rb_state4.var_39) : (0x7ca6954d));
        rb_state4.var_42 = (rb_state4.var_42) + (((((0x2247ae34) - (0x49aa7f57)) ^ (rb_state4.var_40)) ^ (0x64dc6437)) - (rb_state4.var_41));
        rb_state4.var_33 = (rb_state4.var_33) - (rb_state4.var_42);
        rb_state4.var_1 = rb_state4.var_33;
        pthread_mutex_unlock(&(rb_state4.lock_43));
    }
    #endif
    r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x23c8)) {
        pthread_mutex_lock(&(rb_state10.lock_29));
        rb_state10.var_3 = 0xc207ca61;
        rb_state10.var_4 = (rb_state10.var_4) + (((0x4d86191a) - (0x65d55436)) - (rb_state10.var_2));
        rb_state10.var_5 = (rb_state10.var_5) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0x3aab6eb0));
        rb_state10.var_6 = (rb_state10.var_6) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0xf7e87570));
        rb_state10.var_7 = (rb_state10.var_7) - (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_3) : (0xe1ba33f4));
        rb_state10.var_8 = (rb_state10.var_8) - (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_4) : (0x27ebcfd4));
        rb_state10.var_9 = (rb_state10.var_9) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_5) : (0x7c34655c));
        rb_state10.var_10 = (rb_state10.var_10) + (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_7) : (0xee47da19));
        rb_state10.var_11 = (rb_state10.var_11) + (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_9) : (0xd7586610));
        rb_state10.var_12 = (rb_state10.var_12) ^ (((((0xed8c413a) + (rb_state10.var_11)) + (0xa2a9a8ef)) - (rb_state10.var_9)) + (rb_state10.var_10));
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_12);
        rb_state10.var_1 = rb_state10.var_2;
        pthread_mutex_unlock(&(rb_state10.lock_29));
    }
    if ((rb_state10.var_0) == (0x23c8)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0xb40)) {
        rb_state0.var_7 = 0xf5385521;
        rb_state0.var_8 = 0xd63b5e50;
        rb_state0.var_9 = 0xf5da64a;
        rb_state0.var_10 = (rb_state0.var_10) - (((((0xf910f619) + (rb_state0.var_4)) + (0x34de4ebc)) + (rb_state0.var_8)) + (rb_state0.var_9));
        rb_state0.var_11 = (rb_state0.var_11) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_7) : (0x15104187));
        rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_10) : (0xf008c255));
        rb_state0.var_13 = (rb_state0.var_13) - (rb_state0.var_11);
        rb_state0.var_14 = (rb_state0.var_14) ^ (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_13) : (0x770afb59));
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0xce51480a)) {
        rb_state18.var_3 = 0x943f5114;
        rb_state18.var_4 = (rb_state18.var_4) - (657072 < rb_input_size ? (uint32_t)rb_input[657072] : 0xdd636da4);
        rb_state18.var_5 = (rb_state18.var_5) - (((((0x3d94706b) - (rb_state18.var_2)) + (rb_state18.var_4)) + (0x47d2c8c9)) + (rb_state18.var_3));
        rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_4);
        rb_state18.var_7 = (rb_state18.var_7) - (rb_state18.var_5);
        rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_6) : (0xe97e0e31));
        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0xa6364bdd));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_9);
    }
    #endif
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
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
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x60de30eb));
    if ((rb_state2.var_0) == (0xeb17cb7b)) {
        rb_state2.var_4 = 0x1dfc13bf;
        rb_state2.var_5 = 0xbf9d85a5;
        rb_state2.var_6 = (rb_state2.var_6) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_5) : (0xa59ec619));
        rb_state2.var_7 = (rb_state2.var_7) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_4) : (0x38ce5c07));
        rb_state2.var_8 = (rb_state2.var_8) - (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_6) : (0xe1a46e25));
        rb_state2.var_9 = (rb_state2.var_9) ^ (((0x8d53076d) + (0x4b0325d0)) ^ (rb_state2.var_7));
        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_8) : (0x72736d54));
        rb_state2.var_11 = (rb_state2.var_11) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_9) : (0x10dcde09));
        rb_state2.var_12 = (rb_state2.var_12) ^ (((((0x738b344a) + (0xf50190c0)) ^ (rb_state2.var_10)) - (rb_state2.var_10)) - (rb_state2.var_11));
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (622057 < rb_input_size ? (uint32_t)rb_input[622057] : 0x6030b6b2);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        rb_state6.var_18 = 0xb1138498;
        rb_state6.var_19 = 0x7db6a72;
        rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_9) == (0x502c6bcf)) ? (rb_state6.var_19) : (0xe8862adf));
        rb_state6.var_21 = (rb_state6.var_21) + (((((0x534b828c) - (rb_state6.var_18)) + (rb_state6.var_8)) + (0xd6811a3)) - (0x7e65735b));
        rb_state6.var_22 = (rb_state6.var_22) ^ (rb_state6.var_20);
        rb_state6.var_23 = (rb_state6.var_23) - (((rb_state6.var_21) == (0x313a9ba2)) ? (rb_state6.var_22) : (0x376f8b62));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_23);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (11636 < rb_input_size ? (uint32_t)rb_input[11636] : 0xf4d80c5e);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    ringbuffer_t recv_buf, send_buf;
    int r;

    chunk_t *temp;
    chunk_t *chunk;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + ((0x63b3ddb) - (rb_state1.var_0));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x6ea75422) + (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x0)) {
        rb_state7.var_2 = rb_state7.var_1;
    }
    if ((rb_state7.var_0) == (0x0)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x775d0d1f));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x7cef8a2b)) {
        pthread_mutex_lock(&(rb_state19.lock_14));
        rb_state19.var_3 = 0x905d93ce;
        rb_state19.var_4 = 0x34edd63d;
        rb_state19.var_5 = 0x9618718f;
        rb_state19.var_6 = (rb_state19.var_6) - (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_5) : (0x67624858));
        rb_state19.var_7 = (rb_state19.var_7) ^ (((0x59beec1f) ^ (0x5feeb138)) + (rb_state19.var_4));
        rb_state19.var_8 = (rb_state19.var_8) + (((0xdbb1d4fb) - (0x83751824)) ^ (rb_state19.var_6));
        rb_state19.var_9 = (rb_state19.var_9) + (((((0x7cf6fcaa) + (0x3fb4d134)) ^ (rb_state19.var_7)) ^ (rb_state19.var_8)) ^ (0x4f833c22));
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_9);
        rb_state19.var_1 = rb_state19.var_2;
        pthread_mutex_unlock(&(rb_state19.lock_14));
    }
    if ((rb_state19.var_0) == (0x7cef8a2b)) {
        if (!((rb_state19.var_1) == (rb_state19.var_2))) {
            racebench_trigger(19);
        }
    }
    #endif
    r = 0;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_20));
        rb_state11.var_3 = 0x5283e91b;
        rb_state11.var_4 = (rb_state11.var_4) + (((((0xc34d5665) ^ (0x50ce1f02)) ^ (rb_state11.var_2)) ^ (rb_state11.var_3)) ^ (0x538f9033));
        rb_state11.var_5 = (rb_state11.var_5) + (((0x3a8f2054) ^ (rb_state11.var_3)) - (rb_state11.var_1));
        rb_state11.var_6 = (rb_state11.var_6) + (rb_state11.var_4);
        rb_state11.var_7 = (rb_state11.var_7) ^ (((0xd008d18c) + (rb_state11.var_5)) - (rb_state11.var_4));
        rb_state11.var_8 = (rb_state11.var_8) - (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0x565bf363));
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_8);
        rb_state11.var_1 = rb_state11.var_2;
        pthread_mutex_unlock(&(rb_state11.lock_20));
    }
    if ((rb_state11.var_0) == (0x0)) {
        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
            racebench_trigger(11);
        }
    }
    #endif
    r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
    r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x63b3ddb)) {
        if ((rb_state1.var_1) != (0x0)) {
            if (!((rb_state1.var_1) == (rb_state1.var_37))) {
                racebench_trigger(1);
            }
        }
    }
    #endif
    assert(r == 0);

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x21c98d8)) {
        rb_state12.var_3 = 0x208038d0;
        rb_state12.var_4 = (rb_state12.var_4) + (756724 < rb_input_size ? (uint32_t)rb_input[756724] : 0x969619fb);
        rb_state12.var_5 = (rb_state12.var_5) ^ (((0x82c7cb88) - (rb_state12.var_3)) - (rb_state12.var_3));
        rb_state12.var_6 = (rb_state12.var_6) + (((((0x56d8c48f) - (rb_state12.var_4)) - (rb_state12.var_5)) + (rb_state12.var_2)) + (rb_state12.var_1));
        rb_state12.var_7 = (rb_state12.var_7) ^ (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_4) : (0x3ac20630));
        rb_state12.var_8 = (rb_state12.var_8) ^ (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_6) : (0x651631ad));
        rb_state12.var_9 = (rb_state12.var_9) ^ (((0x171143e1) ^ (rb_state12.var_7)) ^ (rb_state12.var_7));
        rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_9) : (0xb598e8ab));
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_10);
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
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
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    size_t preloading_buffer_seek = 0;
    int qid = 0;
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_24));
        rb_state6.var_3 = 0x438b826e;
        rb_state6.var_4 = 0x641036ae;
        rb_state6.var_5 = (rb_state6.var_5) + (((((0x23e4c6fd) - (rb_state6.var_3)) - (rb_state6.var_2)) + (rb_state6.var_4)) ^ (rb_state6.var_1));
        rb_state6.var_6 = (rb_state6.var_6) - (223504 < rb_input_size ? (uint32_t)rb_input[223504] : 0x525b36a9);
        rb_state6.var_7 = (rb_state6.var_7) - (((0x8f4edff1) - (rb_state6.var_3)) + (rb_state6.var_4));
        rb_state6.var_8 = (rb_state6.var_8) ^ (((rb_state6.var_5) == (0x44697b3d)) ? (rb_state6.var_6) : (0xb1da931e));
        rb_state6.var_9 = (rb_state6.var_9) ^ (((rb_state6.var_5) == (0x44697b3d)) ? (rb_state6.var_7) : (0x7f6fe792));
        rb_state6.var_10 = (rb_state6.var_10) + (((0x9d5da7ad) ^ (0xcfe9de4e)) + (rb_state6.var_8));
        rb_state6.var_11 = (rb_state6.var_11) + (rb_state6.var_9);
        rb_state6.var_12 = (rb_state6.var_12) + (((0xaf3e0e2d) ^ (rb_state6.var_6)) - (rb_state6.var_10));
        rb_state6.var_13 = (rb_state6.var_13) - (((0xfaa8d8dc) - (rb_state6.var_11)) + (rb_state6.var_7));
        rb_state6.var_14 = (rb_state6.var_14) - (((0xe7da2aa9) + (rb_state6.var_12)) + (0x9a24052b));
        rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_13);
        rb_state6.var_16 = (rb_state6.var_16) - (((0x4d22570b) + (0x4cdd7e1c)) + (rb_state6.var_14));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((rb_state6.var_15) == (0x5572724)) ? (rb_state6.var_16) : (0x885a9ad9));
        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_17);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_24));
    }
    if ((rb_state6.var_0) == (0x0)) {
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
    }
    #endif
    int fd = args->fd;
    int i;

    ringbuffer_t send_buf;
    sequence_number_t anchorcount = 0;
    int r;

    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0x7cef8a2b) + (rb_state19.var_0));
    #endif
    chunk_t *temp = NULL;
    chunk_t *chunk = NULL;
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (0x21c98d8);
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x4bf5fc66)) {
        pthread_mutex_lock(&(rb_state2.lock_13));
        rb_state2.var_3 = (rb_state2.var_3) ^ (((((0xa2e785e7) - (rb_state2.var_2)) ^ (rb_state2.var_1)) ^ (0xa579e2a0)) + (rb_state2.var_3));
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_3);
        rb_state2.var_1 = rb_state2.var_2;
        pthread_mutex_unlock(&(rb_state2.lock_13));
    }
    if ((rb_state2.var_0) == (0xeb17cb7b)) {
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
    }
    #endif
    assert(r == 0);

    rf_win_dataprocess = 0;
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x21c98d8)) {
        pthread_mutex_lock(&(rb_state12.lock_11));
        rb_state12.var_2 = rb_state12.var_1;
        pthread_mutex_unlock(&(rb_state12.lock_11));
    }
    if ((rb_state12.var_0) == (0x21c98d8)) {
        if (!((rb_state12.var_1) == (rb_state12.var_2))) {
            racebench_trigger(12);
        }
    }
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
    struct thread_args *args = (struct thread_args *)targs;
    int qid = 0;
    int fd = 0;

    ringbuffer_t recv_buf;
    chunk_t *chunk;

    SearchTree T;
    T = TreeMakeEmpty(NULL);
    Position pos = NULL;
    struct tree_element tele;

    sequence_t next;
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_13);
    #endif
    sequence_reset(&next);

    sequence_number_t *chunks_per_anchor;
    unsigned int chunks_per_anchor_max = 1024;
    chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x0)) {
        rb_state8.var_10 = 0xba8a288c;
        rb_state8.var_11 = 0x443bc97b;
        rb_state8.var_12 = (rb_state8.var_12) - (((0xb1dab3cf) ^ (rb_state8.var_7)) - (0x3a8b879a));
        rb_state8.var_13 = (rb_state8.var_13) ^ (((((0xe89a7b28) ^ (rb_state8.var_10)) ^ (0x99db733e)) - (rb_state8.var_11)) - (0x4b64dd42));
        rb_state8.var_14 = (rb_state8.var_14) ^ (((0x198aaa35) - (rb_state8.var_8)) - (rb_state8.var_12));
        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_9) == (0x73f07490)) ? (rb_state8.var_13) : (0xc07251d4));
        rb_state8.var_16 = (rb_state8.var_16) + (((0xfd00f422) - (rb_state8.var_10)) ^ (rb_state8.var_14));
        rb_state8.var_17 = (rb_state8.var_17) ^ (rb_state8.var_15);
        rb_state8.var_18 = (rb_state8.var_18) + (rb_state8.var_16);
        rb_state8.var_19 = (rb_state8.var_19) - (rb_state8.var_17);
        rb_state8.var_20 = (rb_state8.var_20) - (rb_state8.var_18);
        rb_state8.var_21 = (rb_state8.var_21) - (rb_state8.var_19);
        rb_state8.var_22 = (rb_state8.var_22) - (rb_state8.var_20);
        rb_state8.var_23 = (rb_state8.var_23) - (((0x82bc7453) ^ (rb_state8.var_11)) ^ (rb_state8.var_21));
        rb_state8.var_24 = (rb_state8.var_24) - (((0xfef77b5f) - (rb_state8.var_22)) ^ (rb_state8.var_12));
        rb_state8.var_25 = (rb_state8.var_25) + (rb_state8.var_23);
        rb_state8.var_26 = (rb_state8.var_26) + (((rb_state8.var_13) == (0x3c2a79dd)) ? (rb_state8.var_24) : (0x554f61c4));
        rb_state8.var_27 = (rb_state8.var_27) ^ (rb_state8.var_25);
        rb_state8.var_28 = (rb_state8.var_28) ^ (((0x2bb98c7d) + (rb_state8.var_26)) + (0xd8000740));
        rb_state8.var_29 = (rb_state8.var_29) - (((0x28dcedfd) + (rb_state8.var_27)) + (0x124ec7b3));
        rb_state8.var_30 = (rb_state8.var_30) ^ (((0x31241554) + (0xbd3fe8a1)) ^ (rb_state8.var_28));
        rb_state8.var_31 = (rb_state8.var_31) + (((0x9a57da19) + (rb_state8.var_29)) ^ (0x80390f69));
        rb_state8.var_32 = (rb_state8.var_32) + (((0x17c4bedc) ^ (rb_state8.var_14)) + (rb_state8.var_30));
        rb_state8.var_33 = (rb_state8.var_33) + (((rb_state8.var_31) == (0xd9e0e637)) ? (rb_state8.var_32) : (0x7268fca4));
        rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_33);
    }
    #endif
    if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
    memset(chunks_per_anchor, 0, chunks_per_anchor_max * sizeof(sequence_number_t));
    int r;
    int i;

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

    struct thread_args anchor_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - ((0x3a0acc71) + (0xfd2a6945));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xf91bd38a));
        #endif
        anchor_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x0)) {
            rb_state7.var_3 = 0x246a7775;
            rb_state7.var_4 = (rb_state7.var_4) ^ (((0x9df97c0e) - (rb_state7.var_4)) ^ (rb_state7.var_2));
            rb_state7.var_5 = (rb_state7.var_5) ^ (0xdaebe92b);
            rb_state7.var_6 = (rb_state7.var_6) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x1df7bec5));
            rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_3) : (0x7994ba5d));
            rb_state7.var_8 = (rb_state7.var_8) + (((((0x257286db) - (0x51eec689)) + (rb_state7.var_5)) - (rb_state7.var_8)) + (rb_state7.var_4));
            rb_state7.var_9 = (rb_state7.var_9) + (((0x3e7a6669) + (rb_state7.var_6)) + (rb_state7.var_9));
            rb_state7.var_10 = (rb_state7.var_10) - (rb_state7.var_7);
            rb_state7.var_11 = (rb_state7.var_11) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_8) : (0xa9f63138));
            rb_state7.var_12 = (rb_state7.var_12) ^ (((((0x50ab91cb) + (0xf34cde5f)) + (rb_state7.var_9)) ^ (rb_state7.var_11)) - (rb_state7.var_10));
            rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x3a73160f));
            rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_13) : (0xdee884e2));
            rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0xa4774110));
        #endif
        pthread_create(&threads_anchor[i], NULL, FragmentRefine, &anchor_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xf5c9d125));
    #endif
    struct thread_args chunk_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x0)) {
            rb_state11.var_9 = 0xea736439;
            rb_state11.var_10 = 0xe45ad4cf;
            rb_state11.var_11 = (rb_state11.var_11) + (((0x78f8f252) + (0x5e328704)) - (rb_state11.var_5));
            rb_state11.var_12 = (rb_state11.var_12) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_10) : (0x5bb3d9b9));
            rb_state11.var_13 = (rb_state11.var_13) - (((0x9520f5a1) + (rb_state11.var_9)) ^ (0x5ea14486));
            rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_11) : (0x24b4c12a));
            rb_state11.var_15 = (rb_state11.var_15) + (((0xf4369610) - (0xa27780fd)) - (rb_state11.var_12));
            rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_13) : (0xa204c9f4));
            rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_14) == (0x0)) ? (rb_state11.var_15) : (0xe034bb43));
            rb_state11.var_18 = (rb_state11.var_18) ^ (rb_state11.var_16);
            rb_state11.var_19 = (rb_state11.var_19) - (((((0xa15bb6f) ^ (rb_state11.var_18)) ^ (0x902b13dd)) - (rb_state11.var_9)) - (rb_state11.var_17));
            rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_19);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (1115705 < rb_input_size ? (uint32_t)rb_input[1115705] : 0xcbdbce9d);
        #endif
        chunk_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x63b3ddb)) {
            rb_state1.var_15 = 0xd84233c8;
            rb_state1.var_16 = (rb_state1.var_16) - (0xc90a2510);
            rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_14) : (0xb7c00c7e));
            rb_state1.var_18 = (rb_state1.var_18) - (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_12) : (0xb9a1c80f));
            rb_state1.var_19 = (rb_state1.var_19) + (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_13) : (0x82d67e98));
            rb_state1.var_20 = (rb_state1.var_20) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_16) : (0xc3dd9be8));
            rb_state1.var_21 = (rb_state1.var_21) + (rb_state1.var_17);
            rb_state1.var_22 = (rb_state1.var_22) ^ (((0xd1523370) - (rb_state1.var_18)) + (rb_state1.var_18));
            rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_19) : (0xf2cf5c83));
            rb_state1.var_24 = (rb_state1.var_24) + (rb_state1.var_20);
            rb_state1.var_25 = (rb_state1.var_25) ^ (((0x14ea162f) - (rb_state1.var_21)) + (rb_state1.var_20));
            rb_state1.var_26 = (rb_state1.var_26) + (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_22) : (0xf68ffaa0));
            rb_state1.var_27 = (rb_state1.var_27) - (((0xb03d0fbf) - (rb_state1.var_23)) ^ (rb_state1.var_22));
            rb_state1.var_28 = (rb_state1.var_28) + (((rb_state1.var_23) == (0x0)) ? (rb_state1.var_24) : (0x5cdd7e1a));
            rb_state1.var_29 = (rb_state1.var_29) - (rb_state1.var_25);
            rb_state1.var_30 = (rb_state1.var_30) - (((rb_state1.var_24) == (0x0)) ? (rb_state1.var_26) : (0xba2c709e));
            rb_state1.var_31 = (rb_state1.var_31) - (((((0x1393a007) + (rb_state1.var_28)) ^ (rb_state1.var_27)) + (rb_state1.var_25)) - (rb_state1.var_26));
            rb_state1.var_32 = (rb_state1.var_32) + (((0x7be2dc07) - (0xd016a2c7)) + (rb_state1.var_29));
            rb_state1.var_33 = (rb_state1.var_33) - (((((0xa43ae372) - (rb_state1.var_30)) ^ (rb_state1.var_31)) ^ (0x687e81d0)) + (rb_state1.var_27));
            rb_state1.var_34 = (rb_state1.var_34) - (((rb_state1.var_28) == (0x0)) ? (rb_state1.var_32) : (0x3738ced8));
            rb_state1.var_35 = (rb_state1.var_35) + (((0xe5b0716) - (rb_state1.var_29)) + (rb_state1.var_33));
            rb_state1.var_36 = (rb_state1.var_36) + (((((0x7924d131) - (0xf7d2dec2)) - (rb_state1.var_35)) ^ (rb_state1.var_34)) ^ (rb_state1.var_30));
            rb_state1.var_14 = (rb_state1.var_14) ^ (rb_state1.var_36);
            rb_state1.var_1 = rb_state1.var_14;
        }
        if ((rb_state1.var_0) == (0x63b3ddb)) {
            pthread_mutex_lock(&(rb_state1.lock_52));
            rb_state1.var_38 = 0x58572dc9;
            rb_state1.var_39 = 0xdde0a86d;
            rb_state1.var_40 = (rb_state1.var_40) + (((rb_state1.var_31) == (0x0)) ? (rb_state1.var_39) : (0x11a690fb));
            rb_state1.var_41 = (rb_state1.var_41) ^ ((0x1b85668d) - (rb_state1.var_32));
            rb_state1.var_42 = (rb_state1.var_42) - (rb_state1.var_38);
            rb_state1.var_43 = (rb_state1.var_43) - (rb_state1.var_40);
            rb_state1.var_44 = (rb_state1.var_44) - (((0x422711e2) ^ (rb_state1.var_41)) ^ (0x7c410051));
            rb_state1.var_45 = (rb_state1.var_45) - (((rb_state1.var_33) == (0x0)) ? (rb_state1.var_42) : (0xfc6548b4));
            rb_state1.var_46 = (rb_state1.var_46) + (((rb_state1.var_34) == (0x0)) ? (rb_state1.var_43) : (0x7b048877));
            rb_state1.var_47 = (rb_state1.var_47) - (rb_state1.var_44);
            rb_state1.var_48 = (rb_state1.var_48) + (rb_state1.var_45);
            rb_state1.var_49 = (rb_state1.var_49) - (rb_state1.var_46);
            rb_state1.var_50 = (rb_state1.var_50) + (((rb_state1.var_47) == (0x0)) ? (rb_state1.var_48) : (0xd1dc5eda));
            rb_state1.var_51 = (rb_state1.var_51) - (((((0xae23b634) ^ (rb_state1.var_49)) ^ (rb_state1.var_50)) + (rb_state1.var_35)) + (rb_state1.var_36));
            rb_state1.var_37 = (rb_state1.var_37) - (rb_state1.var_51);
            rb_state1.var_1 = rb_state1.var_37;
            pthread_mutex_unlock(&(rb_state1.lock_52));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (162607 < rb_input_size ? (uint32_t)rb_input[162607] : 0x6556bada);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (0x6e5d7166);
        #endif
        pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x7cef8a2b)) {
        pthread_mutex_lock(&(rb_state19.lock_14));
        rb_state19.var_10 = 0x598228db;
        rb_state19.var_11 = 0x95892447;
        rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_1) == (0x8b7c9e7)) ? (rb_state19.var_11) : (0xf1cefa0f));
        rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_10) == (0x598228db)) ? (rb_state19.var_12) : (0x5811c7ed));
        rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_13);
        pthread_mutex_unlock(&(rb_state19.lock_14));
    }
    #endif
    struct thread_args compress_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (523995 < rb_input_size ? (uint32_t)rb_input[523995] : 0x9a1848f6);
        rb_state16.var_0 = (rb_state16.var_0) + ((0xf21e592a) - (0xe1884b73));
        #endif
        compress_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0x29fdb7b7));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (0x41a247ac);
        #endif
        pthread_create(&threads_compress[i], NULL, Compress, &compress_thread_args[i]);
    }

    struct thread_args send_block_args;
    send_block_args.tid = 0;
    send_block_args.nqueues = nqueues;
    pthread_create(&threads_send, NULL, Reorder, &send_block_args);

    stats_t *threads_anchor_rv[conf->nthreads];
    stats_t *threads_chunk_rv[conf->nthreads];
    stats_t *threads_compress_rv[conf->nthreads];

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