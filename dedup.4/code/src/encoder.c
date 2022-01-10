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

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) + (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_2) : (0x9c6a5c3e));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (0x83f0de05);
    rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_14);
    #endif
    assert(s != NULL);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (764729 < rb_input_size ? (uint32_t)rb_input[764729] : 0xf99a1508);
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x51106374)) {
        rb_state16.var_4 = rb_state16.var_1;
    }
    if ((rb_state16.var_0) == (0x51106374)) {
        rb_state16.var_14 = !((rb_state16.var_1) == (rb_state16.var_4));
    }
    #endif
    s->total_input = 0;
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (450356 < rb_input_size ? (uint32_t)rb_input[450356] : 0xcdd523ab);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xeed698e9)) {
        rb_state12.var_5 = 0x450ce573;
        rb_state12.var_6 = 0x19c56e29;
        rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_5) == (0x450ce573)) ? (rb_state12.var_4) : (0xb2a32979));
        rb_state12.var_8 = (rb_state12.var_8) + (((0x28065671) - (rb_state12.var_6)) ^ (rb_state12.var_5));
        rb_state12.var_9 = (rb_state12.var_9) ^ (((((0x4519a0a8) + (0x6f35d67)) - (rb_state12.var_6)) ^ (rb_state12.var_7)) + (rb_state12.var_7));
        rb_state12.var_10 = (rb_state12.var_10) ^ (rb_state12.var_8);
        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_9) == (0xc5b84ff8)) ? (rb_state12.var_10) : (0x81a4d2fa));
        rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_11);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x51106374)) {
        rb_state16.var_5 = 0xbeca5f8e;
        rb_state16.var_6 = 0xed11659f;
        rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_6) == (0xed11659f)) ? (rb_state16.var_5) : (0xde13db7a));
        rb_state16.var_8 = (rb_state16.var_8) + (((((0x8073031a) + (rb_state16.var_5)) + (rb_state16.var_7)) + (0x4ba9372c)) - (rb_state16.var_6));
        rb_state16.var_9 = (rb_state16.var_9) + (((0xa1217b79) + (rb_state16.var_4)) + (0x57f99ff4));
        rb_state16.var_10 = (rb_state16.var_10) + (((((0xd2b2dfe0) ^ (rb_state16.var_7)) - (rb_state16.var_8)) + (rb_state16.var_8)) ^ (rb_state16.var_9));
        rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_10) == (0x95639b11)) ? (rb_state16.var_9) : (0xdd857eb3));
        rb_state16.var_12 = (rb_state16.var_12) ^ (((((0x39e5cc32) + (rb_state16.var_10)) - (0x5e56b0d4)) - (0x3d598113)) ^ (rb_state16.var_11));
        rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_12);
    }
    #endif
    s->total_dedup = 0;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0xb1c7a958)) {
        pthread_mutex_lock(&(rb_state5.lock_30));
        rb_state5.var_11 = 0x5aa3c60e;
        rb_state5.var_12 = (rb_state5.var_12) + (3916 < rb_input_size ? (uint32_t)rb_input[3916] : 0x619123f5);
        rb_state5.var_13 = (rb_state5.var_13) - (rb_state5.var_11);
        rb_state5.var_14 = (rb_state5.var_14) + (((0x4c504d3f) + (rb_state5.var_8)) ^ (rb_state5.var_7));
        rb_state5.var_15 = (rb_state5.var_15) + (rb_state5.var_6);
        rb_state5.var_16 = (rb_state5.var_16) ^ (((((0xec03b555) ^ (rb_state5.var_13)) - (rb_state5.var_12)) - (rb_state5.var_9)) - (0x727b5351));
        rb_state5.var_17 = (rb_state5.var_17) + (((0xbcc0fc8a) ^ (rb_state5.var_14)) + (0x43fc468c));
        rb_state5.var_18 = (rb_state5.var_18) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_15) : (0x6a6f9125));
        rb_state5.var_19 = (rb_state5.var_19) + (((((0x321eaee) ^ (rb_state5.var_11)) + (rb_state5.var_17)) ^ (rb_state5.var_16)) + (0x9ee0d454));
        rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_18);
        rb_state5.var_21 = (rb_state5.var_21) ^ (((((0x5a1a04fa) - (rb_state5.var_19)) ^ (rb_state5.var_20)) ^ (rb_state5.var_12)) + (0xaf9da81f));
        rb_state5.var_10 = (rb_state5.var_10) - (rb_state5.var_21);
        rb_state5.var_4 = rb_state5.var_10;
        pthread_mutex_unlock(&(rb_state5.lock_30));
    }
    #endif
    s->total_compressed = 0;
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0xb1c7a958)) {
        pthread_mutex_lock(&(rb_state5.lock_30));
        rb_state5.var_23 = 0x4b2ef5bc;
        rb_state5.var_24 = 0xd9560062;
        rb_state5.var_25 = (rb_state5.var_25) ^ (rb_state5.var_13);
        rb_state5.var_26 = (rb_state5.var_26) + (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_24) : (0xc0af04f6));
        rb_state5.var_27 = (rb_state5.var_27) - (((0x195d4e59) + (0x7766bc0a)) - (rb_state5.var_25));
        rb_state5.var_28 = (rb_state5.var_28) - (((0x838c26e5) - (rb_state5.var_26)) ^ (0xce4fd87d));
        rb_state5.var_29 = (rb_state5.var_29) ^ (((rb_state5.var_27) == (0x0)) ? (rb_state5.var_28) : (0xb980935b));
        rb_state5.var_22 = (rb_state5.var_22) - (rb_state5.var_29);
        rb_state5.var_4 = rb_state5.var_22;
        pthread_mutex_unlock(&(rb_state5.lock_30));
    }
    #endif
    s->total_output = 0;

    for (i = 0; i < CHUNK_MAX_NUM; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_3);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_3 = (rb_state16.var_3) + (0x1e864035);
        if ((rb_state16.var_4) == (0x35170ebc)) {
            rb_state16.var_26 = 0x1606cdb1;
            rb_state16.var_27 = 0x6ef11f4d;
            rb_state16.var_28 = (rb_state16.var_28) - (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_26) : (0xabd4240d));
            rb_state16.var_29 = (rb_state16.var_29) + (rb_state16.var_27);
            rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_29) : (0xf5bcc129));
            rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_30);
            rb_state16.var_15 = rb_state16.var_25;
        }
        if ((rb_state16.var_4) == (0x35170ebc)) {
            if ((rb_state16.var_15) != (0x0)) {
                if (!((rb_state16.var_15) == (rb_state16.var_31))) {
                    racebench_trigger(16);
                }
            }
        }
        if ((rb_state16.var_4) == (0x35170ebc)) {
            pthread_mutex_lock(&(rb_state16.lock_45));
            rb_state16.var_32 = 0x37573d;
            rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_24) == (0x0)) ? (rb_state16.var_23) : (0x4746c5b2));
            rb_state16.var_34 = (rb_state16.var_34) ^ (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_32) : (0x3aa9a419));
            rb_state16.var_35 = (rb_state16.var_35) - (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_22) : (0xa3ddeb02));
            rb_state16.var_36 = (rb_state16.var_36) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_33) : (0x1c0d8db8));
            rb_state16.var_37 = (rb_state16.var_37) ^ (((0x9281c989) + (rb_state16.var_27)) - (rb_state16.var_34));
            rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_35);
            rb_state16.var_39 = (rb_state16.var_39) + (((0xec6f15d2) + (rb_state16.var_36)) - (0x8fc016fc));
            rb_state16.var_40 = (rb_state16.var_40) - (((rb_state16.var_37) == (0x0)) ? (rb_state16.var_38) : (0xb0286fd1));
            rb_state16.var_41 = (rb_state16.var_41) + (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_39) : (0x9cee77ba));
            rb_state16.var_42 = (rb_state16.var_42) - (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_40) : (0x8de69278));
            rb_state16.var_43 = (rb_state16.var_43) - (rb_state16.var_41);
            rb_state16.var_44 = (rb_state16.var_44) + (((rb_state16.var_42) == (0x0)) ? (rb_state16.var_43) : (0x38b663d3));
            rb_state16.var_31 = (rb_state16.var_31) + (rb_state16.var_44);
            rb_state16.var_15 = rb_state16.var_31;
            pthread_mutex_unlock(&(rb_state16.lock_45));
        }
        #endif
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
    struct thread_args *args = (struct thread_args *)targs;
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf;

#ifdef ENABLE_STATISTICS
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL)
        EXIT_TRACE("Memory allocation failed.\n");
    init_stats(thread_stats);
#endif

    r = 0;
    r += ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    r += ringbuffer_init(&send_buf, ITEM_PER_INSERT);
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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + ((0x512dcdf4) ^ (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_3);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ ((0x1d3cf001) + (rb_state10.var_2));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0xb70f6efd)) {
        rb_state15.var_5 = (rb_state15.var_5) ^ (0xdf442f1c);
        rb_state15.var_6 = (rb_state15.var_6) ^ (rb_state15.var_5);
        rb_state15.var_7 = (rb_state15.var_7) + (((rb_state15.var_6) == (0xdf442f1c)) ? (rb_state15.var_4) : (0x19f9da0b));
        rb_state15.var_8 = (rb_state15.var_8) ^ (0x7341637c);
        rb_state15.var_9 = (rb_state15.var_9) ^ (((0x60f1b216) - (rb_state15.var_2)) - (rb_state15.var_7));
        rb_state15.var_10 = (rb_state15.var_10) ^ (((rb_state15.var_8) == (0x7341637c)) ? (rb_state15.var_5) : (0xfa1dd369));
        rb_state15.var_11 = (rb_state15.var_11) - (rb_state15.var_6);
        rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_7) == (0x3e6e4b78)) ? (rb_state15.var_8) : (0x3181651c));
        rb_state15.var_13 = (rb_state15.var_13) ^ (((rb_state15.var_9) == (0x4ff62a54)) ? (rb_state15.var_9) : (0x288d5355));
        rb_state15.var_14 = (rb_state15.var_14) - (((rb_state15.var_10) == (0xdf442f1c)) ? (rb_state15.var_10) : (0x770c3296));
        rb_state15.var_15 = (rb_state15.var_15) + (rb_state15.var_11);
        rb_state15.var_16 = (rb_state15.var_16) ^ (((0xa23fc156) + (rb_state15.var_12)) ^ (0xd53e51b6));
        rb_state15.var_17 = (rb_state15.var_17) ^ (((0x5f5a97a8) ^ (0xf3ba343a)) ^ (rb_state15.var_13));
        rb_state15.var_18 = (rb_state15.var_18) ^ (((((0xe1c68f74) - (rb_state15.var_14)) ^ (rb_state15.var_15)) + (rb_state15.var_12)) + (rb_state15.var_11));
        rb_state15.var_19 = (rb_state15.var_19) + (((0xf60c2940) ^ (0xf376a907)) - (rb_state15.var_16));
        rb_state15.var_20 = (rb_state15.var_20) + (rb_state15.var_17);
        rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_18) == (0x75aea2d4)) ? (rb_state15.var_19) : (0x85b7d9d4));
        rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_13) == (0x4ff62a54)) ? (rb_state15.var_20) : (0x165f7429));
        rb_state15.var_23 = (rb_state15.var_23) ^ (((0x34adbf01) - (rb_state15.var_21)) ^ (rb_state15.var_14));
        rb_state15.var_24 = (rb_state15.var_24) + (((((0x809bbe1c) ^ (rb_state15.var_22)) ^ (0xeda603ff)) ^ (rb_state15.var_23)) ^ (0x17c78a29));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - ((0xd37da6c4) - (0xbf4a017));
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ (17719 < rb_input_size ? (uint32_t)rb_input[17719] : 0xb8a9d147);
    rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ (836538 < rb_input_size ? (uint32_t)rb_input[836538] : 0xa757e61e);
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + ((0x28e389c6) ^ (0x792f20a5));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_11 = (rb_state16.var_11) ^ (0x2514c4e0);
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
    r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
    r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_2) == (0xc14261ee)) ? (rb_state0.var_1) : (0xbcc87953));
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) ^ (841885 < rb_input_size ? (uint32_t)rb_input[841885] : 0x31dc8637);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (894127 < rb_input_size ? (uint32_t)rb_input[894127] : 0x589f82fd);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) + (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_2) : (0xc0956e88));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) - (840882 < rb_input_size ? (uint32_t)rb_input[840882] : 0xcc25b2b9);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ ((0x1dd5d36f) - (0x977e3304));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_0) == (0xfffffea4)) ? (rb_state17.var_1) : (0xfaae9a1));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_1);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0xe7664210));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) + ((0x797acb50) - (rb_state3.var_3));
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (531816 < rb_input_size ? (uint32_t)rb_input[531816] : 0x9823bcd4);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) ^ ((0x57cdae49) + (rb_state6.var_2));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + ((0xb6781e98) - (rb_state9.var_1));
    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_1) : (0xe41ec30b));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0xf4dc70d7)) ? (rb_state11.var_0) : (0xf070f57e));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + ((0xd28d3c4a) - (rb_state15.var_2));
    rb_state15.var_0 = (rb_state15.var_0) - (919965 < rb_input_size ? (uint32_t)rb_input[919965] : 0x8cb15ede);
    rb_state15.var_3 = (rb_state15.var_3) ^ ((0x67e291e2) ^ (0x6e2ff943));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (0x7e28609c);
    rb_state17.var_1 = (rb_state17.var_1) - ((0xcd0df5c) - (rb_state17.var_0));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - ((0xc6c17296) ^ (0xae5a453e));
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    ringbuffer_t recv_buf, send_buf;
    int r;

    chunk_t *temp;
    chunk_t *chunk;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_36));
        rb_state0.var_18 = 0x166f97e9;
        rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0xa1f6b389));
        rb_state0.var_20 = (rb_state0.var_20) - (rb_state0.var_11);
        rb_state0.var_21 = (rb_state0.var_21) - (rb_state0.var_7);
        rb_state0.var_22 = (rb_state0.var_22) - (((((0x3538b302) - (rb_state0.var_12)) ^ (rb_state0.var_13)) ^ (rb_state0.var_8)) + (rb_state0.var_18));
        rb_state0.var_23 = (rb_state0.var_23) ^ (((0x88d593f7) + (rb_state0.var_14)) - (rb_state0.var_19));
        rb_state0.var_24 = (rb_state0.var_24) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_20) : (0x9d7d612b));
        rb_state0.var_25 = (rb_state0.var_25) + (rb_state0.var_21);
        rb_state0.var_26 = (rb_state0.var_26) + (((0x473aad22) ^ (rb_state0.var_22)) + (rb_state0.var_16));
        rb_state0.var_27 = (rb_state0.var_27) + (rb_state0.var_23);
        rb_state0.var_28 = (rb_state0.var_28) - (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_24) : (0xb5b44768));
        rb_state0.var_29 = (rb_state0.var_29) + (((rb_state0.var_18) == (0x0)) ? (rb_state0.var_25) : (0x44028899));
        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_26);
        rb_state0.var_31 = (rb_state0.var_31) + (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_27) : (0xb54eec91));
        rb_state0.var_32 = (rb_state0.var_32) - (((((0x7d47d26e) ^ (rb_state0.var_20)) ^ (rb_state0.var_29)) ^ (rb_state0.var_28)) ^ (0xeb027a0));
        rb_state0.var_33 = (rb_state0.var_33) ^ (((((0xa5c3c0f7) ^ (rb_state0.var_30)) + (rb_state0.var_21)) + (rb_state0.var_22)) + (rb_state0.var_31));
        rb_state0.var_34 = (rb_state0.var_34) + (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_32) : (0xa43eb097));
        rb_state0.var_35 = (rb_state0.var_35) + (((rb_state0.var_33) == (0x0)) ? (rb_state0.var_34) : (0x8dfac201));
        rb_state0.var_4 = (rb_state0.var_4) - (rb_state0.var_35);
        pthread_mutex_unlock(&(rb_state0.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0x9985b7a5));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (0x9958e49a);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xe4406da5));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0x79b8bfc3));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (40649 < rb_input_size ? (uint32_t)rb_input[40649] : 0xc8e6d745);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) - ((0x790c9089) - (0xb5434bca));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (0xdb211fa4);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (490509 < rb_input_size ? (uint32_t)rb_input[490509] : 0x8098942a);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_3) == (0x0)) {
        rb_state9.var_9 = (rb_state9.var_9) - (0x302ec796);
        rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_7) : (0xe4bc8865));
        rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_10) : (0x7e84b251));
        rb_state9.var_12 = (rb_state9.var_12) + (390259 < rb_input_size ? (uint32_t)rb_input[390259] : 0x835fb505);
        rb_state9.var_13 = (rb_state9.var_13) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_4) : (0xf1f77c9c));
        rb_state9.var_14 = (rb_state9.var_14) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_9) : (0x870ed15f));
        rb_state9.var_15 = (rb_state9.var_15) ^ (rb_state9.var_10);
        rb_state9.var_16 = (rb_state9.var_16) ^ (((0x71a32b76) ^ (rb_state9.var_11)) ^ (0x880b27f));
        rb_state9.var_17 = (rb_state9.var_17) ^ (((((0xf5130d32) + (rb_state9.var_13)) ^ (0xf82b9d6f)) ^ (rb_state9.var_12)) + (0x190f329c));
        rb_state9.var_18 = (rb_state9.var_18) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_14) : (0xadb0a4ff));
        rb_state9.var_19 = (rb_state9.var_19) + (((0xc5113a8) - (rb_state9.var_14)) - (rb_state9.var_15));
        rb_state9.var_20 = (rb_state9.var_20) - (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_17) : (0x68553e97));
        rb_state9.var_21 = (rb_state9.var_21) + (((((0x688485b8) - (rb_state9.var_19)) ^ (rb_state9.var_18)) + (0xce84c934)) ^ (0x1885f3f4));
        rb_state9.var_22 = (rb_state9.var_22) + (((((0x1c197cfb) - (0x893efb5e)) - (rb_state9.var_21)) ^ (0x712c7b73)) - (rb_state9.var_20));
        rb_state9.var_8 = (rb_state9.var_8) + (rb_state9.var_22);
        rb_state9.var_4 = rb_state9.var_8;
    }
    if ((rb_state9.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state9.lock_36));
        rb_state9.var_24 = (rb_state9.var_24) - (rb_state9.var_16);
        rb_state9.var_25 = (rb_state9.var_25) + (1112415 < rb_input_size ? (uint32_t)rb_input[1112415] : 0xdd1bb8d9);
        rb_state9.var_26 = (rb_state9.var_26) ^ (((rb_state9.var_17) == (0x0)) ? (rb_state9.var_15) : (0xb275d4ca));
        rb_state9.var_27 = (rb_state9.var_27) ^ (0xd378a1fe);
        rb_state9.var_28 = (rb_state9.var_28) ^ (((0x99eeea98) - (rb_state9.var_24)) ^ (rb_state9.var_18));
        rb_state9.var_29 = (rb_state9.var_29) + (rb_state9.var_25);
        rb_state9.var_30 = (rb_state9.var_30) - (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_26) : (0xee146e72));
        rb_state9.var_31 = (rb_state9.var_31) ^ (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0x4ac12bc7));
        rb_state9.var_32 = (rb_state9.var_32) - (((rb_state9.var_29) == (0x0)) ? (rb_state9.var_30) : (0x21faa106));
        rb_state9.var_33 = (rb_state9.var_33) - (rb_state9.var_31);
        rb_state9.var_34 = (rb_state9.var_34) + (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_32) : (0x2645ba5b));
        rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0x5c204ab));
        rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_35);
        rb_state9.var_4 = rb_state9.var_23;
        pthread_mutex_unlock(&(rb_state9.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) - (0xfd287796);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_0) == (0xda866136)) ? (rb_state18.var_3) : (0xb196e172));
    if ((rb_state18.var_1) == (0x23c17a68)) {
        rb_state18.var_4 = rb_state18.var_2;
    }
    if ((rb_state18.var_1) == (0x23c17a68)) {
        pthread_mutex_lock(&(rb_state18.lock_10));
        if (!((rb_state18.var_2) == (rb_state18.var_4))) {
            racebench_trigger(18);
        }
        pthread_mutex_unlock(&(rb_state18.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ ((0xbd532396) + (rb_state19.var_0));
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_1);
    if ((rb_state2.var_2) == (0xc9)) {
        pthread_mutex_lock(&(rb_state2.lock_33));
        rb_state2.var_17 = 0x647028f4;
        rb_state2.var_18 = (rb_state2.var_18) - (rb_state2.var_12);
        rb_state2.var_19 = (rb_state2.var_19) ^ (((0xb568902d) + (rb_state2.var_14)) ^ (rb_state2.var_13));
        rb_state2.var_20 = (rb_state2.var_20) - (((0x90e33c65) - (rb_state2.var_17)) + (0xb361d7e8));
        rb_state2.var_21 = (rb_state2.var_21) + (((0x3d2e6a08) ^ (0x2f8523df)) + (rb_state2.var_18));
        rb_state2.var_22 = (rb_state2.var_22) + (rb_state2.var_19);
        rb_state2.var_23 = (rb_state2.var_23) + (rb_state2.var_20);
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_15) == (0x5830d97f)) ? (rb_state2.var_21) : (0x47d35d7c));
        rb_state2.var_25 = (rb_state2.var_25) + (rb_state2.var_22);
        rb_state2.var_26 = (rb_state2.var_26) - (((0xc3afd99a) - (rb_state2.var_16)) - (rb_state2.var_23));
        rb_state2.var_27 = (rb_state2.var_27) + (((((0xb26427e6) - (rb_state2.var_17)) + (rb_state2.var_24)) - (rb_state2.var_18)) + (rb_state2.var_25));
        rb_state2.var_28 = (rb_state2.var_28) ^ (((0xad79af42) - (rb_state2.var_19)) + (rb_state2.var_26));
        rb_state2.var_29 = (rb_state2.var_29) + (((rb_state2.var_20) == (0x202b14a7)) ? (rb_state2.var_27) : (0x19d0c379));
        rb_state2.var_30 = (rb_state2.var_30) + (((rb_state2.var_21) == (0xf0509c52)) ? (rb_state2.var_28) : (0x91bcf1eb));
        rb_state2.var_31 = (rb_state2.var_31) ^ (((0x3be75378) + (rb_state2.var_29)) - (rb_state2.var_22));
        rb_state2.var_32 = (rb_state2.var_32) ^ (((rb_state2.var_30) == (0xe86879be)) ? (rb_state2.var_31) : (0xe886c61b));
        rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_32);
        pthread_mutex_unlock(&(rb_state2.lock_33));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_0);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_1) : (0xb9d5dea2));
    rb_state5.var_2 = (rb_state5.var_2) - ((0xb965da7d) + (0x5d2440fd));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) ^ (0xf250327e);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_2);
    rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (616245 < rb_input_size ? (uint32_t)rb_input[616245] : 0x43120254);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_1) : (0x95fda8ad));
    if ((rb_state11.var_2) == (0xb46e6085)) {
        rb_state11.var_17 = 0x71b8bdd5;
        rb_state11.var_18 = 0x5047159c;
        rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_13) == (0xf32ab050)) ? (rb_state11.var_17) : (0xce82d2b2));
        rb_state11.var_20 = (rb_state11.var_20) - (((rb_state11.var_14) == (0xf5230f9f)) ? (rb_state11.var_18) : (0x22d688a6));
        rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_15) == (0xcd54fb0)) ? (rb_state11.var_12) : (0x6083f66f));
        rb_state11.var_22 = (rb_state11.var_22) - (((((0x2de0b218) + (0x398e7efd)) - (rb_state11.var_20)) + (rb_state11.var_16)) + (rb_state11.var_19));
        rb_state11.var_23 = (rb_state11.var_23) - (((rb_state11.var_17) == (0x71b8bdd5)) ? (rb_state11.var_21) : (0x8c22bb66));
        rb_state11.var_24 = (rb_state11.var_24) + (rb_state11.var_22);
        rb_state11.var_25 = (rb_state11.var_25) - (((rb_state11.var_23) == (0xadcf061)) ? (rb_state11.var_24) : (0x41a8f7f5));
        rb_state11.var_3 = (rb_state11.var_3) ^ (rb_state11.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_2) : (0xf85ce282));
    if ((rb_state17.var_2) == (0x6f)) {
        rb_state17.var_14 = 0xa228877b;
        rb_state17.var_15 = 0x476e8145;
        rb_state17.var_16 = (rb_state17.var_16) - (((0xe22baf41) - (rb_state17.var_10)) - (rb_state17.var_15));
        rb_state17.var_17 = (rb_state17.var_17) ^ ((0xb763a092) + (rb_state17.var_11));
        rb_state17.var_18 = (rb_state17.var_18) + (((((0x25847be) + (rb_state17.var_14)) ^ (rb_state17.var_9)) + (0x770924ba)) ^ (0x2d07a329));
        rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_17) : (0xd416a853));
        rb_state17.var_20 = (rb_state17.var_20) - (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_18) : (0x2586be83));
        rb_state17.var_21 = (rb_state17.var_21) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_19) : (0x6cb74853));
        rb_state17.var_22 = (rb_state17.var_22) - (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0xc7b77dfa));
        rb_state17.var_4 = (rb_state17.var_4) ^ (rb_state17.var_22);
    }
    #endif
    r = 0;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xffffff93)) {
        if ((rb_state10.var_3) != (0x0)) {
            if (!((rb_state10.var_3) == (rb_state10.var_19))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (346378 < rb_input_size ? (uint32_t)rb_input[346378] : 0x3b4a0a07);
    #endif
    r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_29));
        rb_state3.var_6 = 0xfba0b7ac;
        rb_state3.var_7 = 0xf248c09c;
        rb_state3.var_8 = 0x5b034dd1;
        rb_state3.var_9 = (rb_state3.var_9) ^ (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0x815dc9ff));
        rb_state3.var_10 = (rb_state3.var_10) - (((rb_state3.var_7) == (0xf248c09c)) ? (rb_state3.var_6) : (0xe6f8093a));
        rb_state3.var_11 = (rb_state3.var_11) + (((rb_state3.var_6) == (0xfba0b7ac)) ? (rb_state3.var_8) : (0xe58ab546));
        rb_state3.var_12 = (rb_state3.var_12) + (rb_state3.var_9);
        rb_state3.var_13 = (rb_state3.var_13) ^ (((rb_state3.var_10) == (0x45f4854)) ? (rb_state3.var_11) : (0xfebcd574));
        rb_state3.var_14 = (rb_state3.var_14) - (rb_state3.var_12);
        rb_state3.var_15 = (rb_state3.var_15) - (((rb_state3.var_7) == (0xf248c09c)) ? (rb_state3.var_13) : (0x52b180ef));
        rb_state3.var_16 = (rb_state3.var_16) ^ (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_15) : (0x3ddba482));
        rb_state3.var_5 = (rb_state3.var_5) + (rb_state3.var_16);
        rb_state3.var_4 = rb_state3.var_5;
        pthread_mutex_unlock(&(rb_state3.lock_29));
    }
    if ((rb_state3.var_1) == (0x0)) {
        if (!((rb_state3.var_4) == (rb_state3.var_5))) {
            racebench_trigger(3);
        }
    }
    #endif
    r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0x57cdafa8)) {
        rb_state6.var_7 = 0xafaeca8e;
        rb_state6.var_8 = 0x97439e2d;
        rb_state6.var_9 = (rb_state6.var_9) + (((0xef70d156) - (0x80ea9fbf)) - (rb_state6.var_7));
        rb_state6.var_10 = (rb_state6.var_10) ^ (rb_state6.var_8);
        rb_state6.var_11 = (rb_state6.var_11) ^ (((0xd997afdc) + (rb_state6.var_9)) + (0x6c570efe));
        rb_state6.var_12 = (rb_state6.var_12) + (((rb_state6.var_10) == (0x97439e2d)) ? (rb_state6.var_11) : (0xee54718e));
        rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (510385 < rb_input_size ? (uint32_t)rb_input[510385] : 0x67f05878);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_3) == (0xb2)) {
        pthread_mutex_lock(&(rb_state14.lock_24));
        rb_state14.var_6 = 0xf770f1de;
        rb_state14.var_7 = 0xe8580119;
        rb_state14.var_8 = (rb_state14.var_8) - ((0x7242cd6c) - (0x20435aee));
        rb_state14.var_9 = (rb_state14.var_9) - (rb_state14.var_7);
        rb_state14.var_10 = (rb_state14.var_10) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0x66ebe2eb));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_8) : (0x2c9dc566));
        rb_state14.var_12 = (rb_state14.var_12) - (rb_state14.var_9);
        rb_state14.var_13 = (rb_state14.var_13) ^ (((rb_state14.var_10) == (0x88f0e22)) ? (rb_state14.var_11) : (0x52d78a70));
        rb_state14.var_14 = (rb_state14.var_14) - (((((0x688359fc) + (rb_state14.var_6)) ^ (rb_state14.var_12)) + (0x4ebc9792)) ^ (rb_state14.var_13));
        rb_state14.var_5 = (rb_state14.var_5) + (rb_state14.var_14);
        rb_state14.var_4 = rb_state14.var_5;
        pthread_mutex_unlock(&(rb_state14.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) - (0x8a1f15d1);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_2) == (0x6f)) {
        pthread_mutex_lock(&(rb_state17.lock_23));
        rb_state17.var_6 = 0x2f6469af;
        rb_state17.var_7 = 0x8ce7d250;
        rb_state17.var_8 = (rb_state17.var_8) ^ (((((0xdceb56af) ^ (0x41ce48d5)) + (rb_state17.var_4)) + (rb_state17.var_6)) - (0x440f4cf));
        rb_state17.var_9 = (rb_state17.var_9) ^ (716781 < rb_input_size ? (uint32_t)rb_input[716781] : 0x45f6b257);
        rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_7) : (0x41353b89));
        rb_state17.var_11 = (rb_state17.var_11) ^ (((((0x60097ec7) - (rb_state17.var_8)) - (0x507a7e1d)) + (0xdabb28f5)) - (rb_state17.var_9));
        rb_state17.var_12 = (rb_state17.var_12) ^ (((rb_state17.var_6) == (0x2f6469af)) ? (rb_state17.var_10) : (0xf49744c6));
        rb_state17.var_13 = (rb_state17.var_13) - (((((0xa2e1f41c) - (rb_state17.var_8)) + (rb_state17.var_12)) - (rb_state17.var_11)) - (rb_state17.var_7));
        rb_state17.var_5 = (rb_state17.var_5) + (rb_state17.var_13);
        rb_state17.var_4 = rb_state17.var_5;
        pthread_mutex_unlock(&(rb_state17.lock_23));
    }
    #endif
    assert(r == 0);

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (0x917b40ee);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_3);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_1);
    if ((rb_state12.var_0) == (0xeed698e9)) {
        rb_state12.var_4 = rb_state12.var_1;
    }
    if ((rb_state12.var_0) == (0xeed698e9)) {
        if (!((rb_state12.var_1) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_2);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_3) == (0xb2)) {
        if (!((rb_state14.var_4) == (rb_state14.var_5))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (39750 < rb_input_size ? (uint32_t)rb_input[39750] : 0x345c2030);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_2) == (0x6f)) {
        if (!((rb_state17.var_4) == (rb_state17.var_5))) {
            racebench_trigger(17);
        }
    }
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x62)) {
        rb_state1.var_4 = 0xf7daf5b8;
        rb_state1.var_5 = (rb_state1.var_5) - (rb_state1.var_5);
        rb_state1.var_6 = (rb_state1.var_6) - (rb_state1.var_3);
        rb_state1.var_7 = (rb_state1.var_7) ^ (rb_state1.var_6);
        rb_state1.var_8 = (rb_state1.var_8) + (((0x7f42f62a) + (0xa14f26c9)) - (rb_state1.var_4));
        rb_state1.var_9 = (rb_state1.var_9) ^ (((((0x511822c2) ^ (0x2d7f20d1)) - (rb_state1.var_5)) + (rb_state1.var_6)) - (0x62ed553d));
        rb_state1.var_10 = (rb_state1.var_10) + (((0x1c92b2a) + (rb_state1.var_7)) - (0x55064e27));
        rb_state1.var_11 = (rb_state1.var_11) - (((((0xb066653c) - (rb_state1.var_8)) + (0x7842618e)) - (rb_state1.var_9)) - (0x71608fae));
        rb_state1.var_12 = (rb_state1.var_12) + (((((0xd13aa549) + (0x98033ed0)) + (rb_state1.var_11)) ^ (rb_state1.var_7)) - (rb_state1.var_10));
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xa462bab8)) {
        rb_state4.var_4 = rb_state4.var_1;
    }
    if ((rb_state4.var_0) == (0xa462bab8)) {
        pthread_mutex_lock(&(rb_state4.lock_7));
        if (!((rb_state4.var_1) == (rb_state4.var_4))) {
            racebench_trigger(4);
        }
        pthread_mutex_unlock(&(rb_state4.lock_7));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0xc27a047)) {
        rb_state7.var_13 = (rb_state7.var_13) - (rb_state7.var_8);
        rb_state7.var_14 = (rb_state7.var_14) + (0x90c362f8);
        rb_state7.var_15 = (rb_state7.var_15) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_7) : (0xb1316c37));
        rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_13) : (0xc73edbf2));
        rb_state7.var_17 = (rb_state7.var_17) ^ (rb_state7.var_14);
        rb_state7.var_18 = (rb_state7.var_18) ^ (((0xe85df12e) ^ (rb_state7.var_15)) - (0x40a726a5));
        rb_state7.var_19 = (rb_state7.var_19) + (((0x99b97706) ^ (rb_state7.var_16)) - (rb_state7.var_11));
        rb_state7.var_20 = (rb_state7.var_20) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_17) : (0x7f1d853));
        rb_state7.var_21 = (rb_state7.var_21) + (((0x9a07e616) - (rb_state7.var_13)) ^ (rb_state7.var_18));
        rb_state7.var_22 = (rb_state7.var_22) - (((0xefbfd592) ^ (0xef21ace9)) ^ (rb_state7.var_19));
        rb_state7.var_23 = (rb_state7.var_23) - (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_21) : (0x99d276ba));
        rb_state7.var_24 = (rb_state7.var_24) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_22) : (0x9384ee29));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((((0xc2374cc2) - (rb_state7.var_16)) ^ (rb_state7.var_23)) - (rb_state7.var_15)) + (rb_state7.var_24));
        rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_25);
        rb_state7.var_3 = rb_state7.var_12;
    }
    if ((rb_state7.var_0) == (0xc27a047)) {
        pthread_mutex_lock(&(rb_state7.lock_38));
        rb_state7.var_27 = 0x5ede43e2;
        rb_state7.var_28 = 0xdc37a4d4;
        rb_state7.var_29 = (rb_state7.var_29) - ((0x44fba6bd) ^ (rb_state7.var_18));
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_19) == (0x0)) ? (rb_state7.var_28) : (0xb22b1f96));
        rb_state7.var_31 = (rb_state7.var_31) ^ (rb_state7.var_27);
        rb_state7.var_32 = (rb_state7.var_32) ^ (rb_state7.var_17);
        rb_state7.var_33 = (rb_state7.var_33) ^ (rb_state7.var_29);
        rb_state7.var_34 = (rb_state7.var_34) + (((((0x5369b871) - (0xe4c7b200)) + (rb_state7.var_20)) - (rb_state7.var_31)) - (rb_state7.var_30));
        rb_state7.var_35 = (rb_state7.var_35) ^ (((rb_state7.var_32) == (0x0)) ? (rb_state7.var_33) : (0x7faa3fe5));
        rb_state7.var_36 = (rb_state7.var_36) ^ (((0x56119593) ^ (rb_state7.var_34)) + (0xe30b03a7));
        rb_state7.var_37 = (rb_state7.var_37) ^ (((rb_state7.var_35) == (0x0)) ? (rb_state7.var_36) : (0xe5cfdb55));
        rb_state7.var_26 = (rb_state7.var_26) + (rb_state7.var_37);
        rb_state7.var_3 = rb_state7.var_26;
        pthread_mutex_unlock(&(rb_state7.lock_38));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_2) == (0x819712f4)) ? (rb_state13.var_1) : (0x9e1c0c0f));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_1) : (0x8aa72f29));
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (0x4ff23472);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
    rb_state13.var_1 = (rb_state13.var_1) + (0x6bd7c1c7);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (0x921d9dfa);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ ((0xb70f7032) - (rb_state15.var_0));
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + ((0x4b783ebe) ^ (rb_state0.var_0));
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (0xe05186f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + (0x291d9957);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (0xd8e289d5);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
    #endif
    size_t preloading_buffer_seek = 0;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (630641 < rb_input_size ? (uint32_t)rb_input[630641] : 0x199f3fbc);
    #endif
    int qid = 0;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) + ((0x4e771106) ^ (rb_state6.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (511206 < rb_input_size ? (uint32_t)rb_input[511206] : 0x26193360);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (46548 < rb_input_size ? (uint32_t)rb_input[46548] : 0x122edbf5);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_2);
    #endif
    int fd = args->fd;
    int i;

    ringbuffer_t send_buf;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (0xf405dc28);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0x26258bfa));
    #endif
    sequence_number_t anchorcount = 0;
    int r;

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (214590 < rb_input_size ? (uint32_t)rb_input[214590] : 0x95b500a3);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + (0x819712f4);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ ((0xece37732) ^ (rb_state15.var_2));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (742677 < rb_input_size ? (uint32_t)rb_input[742677] : 0xdec3334b);
    #endif
    chunk_t *temp = NULL;
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - ((0xc378084a) + (rb_state8.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0x83e54416);
    rb_state10.var_1 = (rb_state10.var_1) + (0x75869757);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (43207 < rb_input_size ? (uint32_t)rb_input[43207] : 0x5167a838);
    rb_state11.var_2 = (rb_state11.var_2) - (0x4b919f7b);
    #endif
    chunk_t *chunk = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (276131 < rb_input_size ? (uint32_t)rb_input[276131] : 0x29f88f24);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0x57cdafa8)) {
        pthread_mutex_lock(&(rb_state6.lock_13));
        rb_state6.var_6 = (rb_state6.var_6) - (((((0x660f2ec5) + (rb_state6.var_4)) + (rb_state6.var_6)) + (rb_state6.var_5)) - (rb_state6.var_5));
        rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_6);
        rb_state6.var_4 = rb_state6.var_5;
        pthread_mutex_unlock(&(rb_state6.lock_13));
    }
    if ((rb_state6.var_1) == (0x57cdafa8)) {
        if (!((rb_state6.var_4) == (rb_state6.var_5))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (39908 < rb_input_size ? (uint32_t)rb_input[39908] : 0xa0d69660);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) + (661910 < rb_input_size ? (uint32_t)rb_input[661910] : 0x20845ad5);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_3);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_0);
    rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_3);
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state0.lock_36));
        rb_state0.var_6 = 0xf818ee88;
        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_6);
        rb_state0.var_8 = (rb_state0.var_8) - (rb_state0.var_4);
        rb_state0.var_9 = (rb_state0.var_9) + (914914 < rb_input_size ? (uint32_t)rb_input[914914] : 0x853b3f76);
        rb_state0.var_10 = (rb_state0.var_10) ^ (((0xb40b00d8) + (0x198914bc)) ^ (rb_state0.var_5));
        rb_state0.var_11 = (rb_state0.var_11) - (((rb_state0.var_7) == (0xf818ee88)) ? (rb_state0.var_8) : (0x3adc8162));
        rb_state0.var_12 = (rb_state0.var_12) + (((rb_state0.var_6) == (0xf818ee88)) ? (rb_state0.var_9) : (0x61ec5c0f));
        rb_state0.var_13 = (rb_state0.var_13) + (((((0x2078ce2e) ^ (rb_state0.var_11)) + (rb_state0.var_10)) - (0x8fdaed7e)) - (0x62ea926b));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_12);
        rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_13);
        rb_state0.var_16 = (rb_state0.var_16) - (rb_state0.var_14);
        rb_state0.var_17 = (rb_state0.var_17) ^ (((((0xf962b0f6) + (rb_state0.var_15)) ^ (rb_state0.var_16)) + (0x56c46ca6)) ^ (0x9906e711));
        rb_state0.var_5 = (rb_state0.var_5) ^ (rb_state0.var_17);
        rb_state0.var_4 = rb_state0.var_5;
        pthread_mutex_unlock(&(rb_state0.lock_36));
    }
    if ((rb_state0.var_3) == (0x0)) {
        if (!((rb_state0.var_4) == (rb_state0.var_5))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) ^ (0xe779fdf6);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + ((0x6949c43d) + (rb_state10.var_0));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_3) == (0x0)) {
        rb_state13.var_15 = 0xc9b205cd;
        rb_state13.var_16 = 0x9483a9fc;
        rb_state13.var_17 = (rb_state13.var_17) + (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_10) : (0x1f9e2589));
        rb_state13.var_18 = (rb_state13.var_18) ^ (0x78f95ad0);
        rb_state13.var_19 = (rb_state13.var_19) ^ (((((0x120f8346) - (0xb186e808)) ^ (rb_state13.var_9)) - (rb_state13.var_16)) - (0x540ec180));
        rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_15);
        rb_state13.var_21 = (rb_state13.var_21) - (rb_state13.var_17);
        rb_state13.var_22 = (rb_state13.var_22) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_18) : (0x47ca3517));
        rb_state13.var_23 = (rb_state13.var_23) ^ (rb_state13.var_19);
        rb_state13.var_24 = (rb_state13.var_24) + (((rb_state13.var_20) == (0x364dfa33)) ? (rb_state13.var_21) : (0x53a65bd4));
        rb_state13.var_25 = (rb_state13.var_25) ^ (((0xdb9d6912) - (rb_state13.var_13)) ^ (rb_state13.var_22));
        rb_state13.var_26 = (rb_state13.var_26) - (rb_state13.var_23);
        rb_state13.var_27 = (rb_state13.var_27) ^ (((0xddb70ef2) ^ (0x9923ae7d)) ^ (rb_state13.var_24));
        rb_state13.var_28 = (rb_state13.var_28) ^ (((rb_state13.var_25) == (0xa36433c2)) ? (rb_state13.var_26) : (0xf3935348));
        rb_state13.var_29 = (rb_state13.var_29) ^ (((0x3594eb02) - (rb_state13.var_27)) ^ (rb_state13.var_14));
        rb_state13.var_30 = (rb_state13.var_30) - (((rb_state13.var_28) == (0x8809d03e)) ? (rb_state13.var_29) : (0xc6482a9b));
        rb_state13.var_14 = (rb_state13.var_14) ^ (rb_state13.var_30);
        rb_state13.var_4 = rb_state13.var_14;
    }
    if ((rb_state13.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_43));
        rb_state13.var_32 = (rb_state13.var_32) ^ (0x7a4703f8);
        rb_state13.var_33 = (rb_state13.var_33) - (rb_state13.var_16);
        rb_state13.var_34 = (rb_state13.var_34) + (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_15) : (0xe04841f));
        rb_state13.var_35 = (rb_state13.var_35) - (667826 < rb_input_size ? (uint32_t)rb_input[667826] : 0xae99ffef);
        rb_state13.var_36 = (rb_state13.var_36) + (((rb_state13.var_18) == (0x78f95ad0)) ? (rb_state13.var_32) : (0x2c28aee7));
        rb_state13.var_37 = (rb_state13.var_37) - (((((0x27e6e82b) ^ (rb_state13.var_33)) + (rb_state13.var_20)) - (rb_state13.var_34)) + (rb_state13.var_19));
        rb_state13.var_38 = (rb_state13.var_38) + (rb_state13.var_35);
        rb_state13.var_39 = (rb_state13.var_39) - (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_36) : (0xd62201a));
        rb_state13.var_40 = (rb_state13.var_40) + (((((0x93ead572) ^ (rb_state13.var_22)) ^ (rb_state13.var_23)) - (rb_state13.var_37)) + (rb_state13.var_38));
        rb_state13.var_41 = (rb_state13.var_41) + (((0xdf8587a5) + (rb_state13.var_39)) ^ (0x55dd5da0));
        rb_state13.var_42 = (rb_state13.var_42) ^ (((((0xd4f242a2) + (rb_state13.var_41)) + (0xba080f49)) + (rb_state13.var_24)) + (rb_state13.var_40));
        rb_state13.var_31 = (rb_state13.var_31) + (rb_state13.var_42);
        rb_state13.var_4 = rb_state13.var_31;
        pthread_mutex_unlock(&(rb_state13.lock_43));
    }
    #endif
    r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (0x94314a7f);
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xec217a79)) {
        rb_state8.var_5 = 0xf207f5d0;
        rb_state8.var_6 = (rb_state8.var_6) ^ (((rb_state8.var_5) == (0xf207f5d0)) ? (rb_state8.var_4) : (0x8db92a36));
        rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_6);
    }
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
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_2);
            #endif
            bytes_read = max_read;
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_3 = (rb_state4.var_3) ^ ((0xb929f22b) + (rb_state4.var_1));
            #endif
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_3 = (rb_state12.var_3) + (113926 < rb_input_size ? (uint32_t)rb_input[113926] : 0xa7d14e3a);
            #endif
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
            #ifdef RACEBENCH_BUG_4
            rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_2);
            #endif
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
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) + (0xa71e3a64);
    #endif
    T = TreeMakeEmpty(NULL);
    Position pos = NULL;
    struct tree_element tele;

    sequence_t next;
    sequence_reset(&next);

    sequence_number_t *chunks_per_anchor;
    unsigned int chunks_per_anchor_max = 1024;
    chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
    if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
    memset(chunks_per_anchor, 0, chunks_per_anchor_max * sizeof(sequence_number_t));
    int r;
    int i;

    r = ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0xdc0e496b);
    if ((rb_state19.var_3) == (0xfba1d6bf)) {
        rb_state19.var_15 = 0xe01382fb;
        rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_9) == (0x25874068)) ? (rb_state19.var_8) : (0x2906568e));
        rb_state19.var_17 = (rb_state19.var_17) ^ (((0xde71fd8b) + (rb_state19.var_15)) ^ (0x5a250f46));
        rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_16) == (0xea83137a)) ? (rb_state19.var_17) : (0x69c7bd91));
        rb_state19.var_4 = (rb_state19.var_4) + (rb_state19.var_18);
    }
    #endif
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

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (991892 < rb_input_size ? (uint32_t)rb_input[991892] : 0x29df8d4f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x2975c6c) + (0xe98a1e0d));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (984263 < rb_input_size ? (uint32_t)rb_input[984263] : 0x76db3ef3);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + ((0xe9942584) ^ (0x3ed3e69b));
    #endif
    struct thread_args anchor_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (0xc14261ee);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + ((0xef07efd1) ^ (rb_state2.var_0));
        rb_state2.var_1 = (rb_state2.var_1) ^ (0xc0dc3e33);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
        rb_state3.var_0 = (rb_state3.var_0) - (514598 < rb_input_size ? (uint32_t)rb_input[514598] : 0xbcbf48c9);
        rb_state3.var_2 = (rb_state3.var_2) ^ (135875 < rb_input_size ? (uint32_t)rb_input[135875] : 0x1326e9e4);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0xffffff96)) ? (rb_state6.var_0) : (0x20e10c9f));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_2) : (0x4a82ca7f));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_3);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - ((0x62de0cfa) + (rb_state9.var_2));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (66215 < rb_input_size ? (uint32_t)rb_input[66215] : 0xd253bfa3);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (0x61d334ee);
        #endif
        anchor_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + ((0xed4a6b5b) + (0xc28f5c48));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (0xf9d4515a);
        if ((rb_state3.var_1) == (0x0)) {
            rb_state3.var_17 = (rb_state3.var_17) + ((0x5029a5e7) - (rb_state3.var_11));
            rb_state3.var_18 = (rb_state3.var_18) + (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_12) : (0xaf6aa135));
            rb_state3.var_19 = (rb_state3.var_19) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_9) : (0xf7fd4c92));
            rb_state3.var_20 = (rb_state3.var_20) + (((((0x4f8e5bb7) + (0x7d33b16b)) - (rb_state3.var_10)) - (0x7f79ab39)) - (rb_state3.var_8));
            rb_state3.var_21 = (rb_state3.var_21) + (((((0xc7d8b298) - (rb_state3.var_15)) ^ (rb_state3.var_18)) + (0x4a417f76)) + (rb_state3.var_17));
            rb_state3.var_22 = (rb_state3.var_22) ^ (((0xb904bcd1) + (rb_state3.var_19)) - (rb_state3.var_16));
            rb_state3.var_23 = (rb_state3.var_23) ^ (((0xe952d8e2) - (0x12e55888)) - (rb_state3.var_20));
            rb_state3.var_24 = (rb_state3.var_24) - (((0x7ce97781) - (rb_state3.var_17)) + (rb_state3.var_21));
            rb_state3.var_25 = (rb_state3.var_25) + (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_23) : (0x5416f1c7));
            rb_state3.var_26 = (rb_state3.var_26) ^ (((0x936adf4e) + (rb_state3.var_24)) ^ (rb_state3.var_18));
            rb_state3.var_27 = (rb_state3.var_27) ^ (rb_state3.var_25);
            rb_state3.var_28 = (rb_state3.var_28) - (((rb_state3.var_26) == (0x0)) ? (rb_state3.var_27) : (0x16e49a9d));
            rb_state3.var_4 = (rb_state3.var_4) - (rb_state3.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (0x90f85f91);
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0xec217a79)) {
            rb_state8.var_4 = rb_state8.var_1;
        }
        if ((rb_state8.var_0) == (0xec217a79)) {
            pthread_mutex_lock(&(rb_state8.lock_7));
            if (!((rb_state8.var_1) == (rb_state8.var_4))) {
                racebench_trigger(8);
            }
            pthread_mutex_unlock(&(rb_state8.lock_7));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_2) == (0x9d21f306)) ? (rb_state9.var_1) : (0x13e6ce2c));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - ((0x14340182) - (0xbb28271f));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_0);
        rb_state13.var_3 = (rb_state13.var_3) ^ (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_3) : (0xf0c475e5));
        if ((rb_state13.var_3) == (0x0)) {
            if ((rb_state13.var_4) != (0x0)) {
                if (!((rb_state13.var_4) == (rb_state13.var_31))) {
                    racebench_trigger(13);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) - ((0x76ef86a5) ^ (0xe10e9d98));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ ((0xa262845) + (0x72de1f04));
        #endif
        pthread_create(&threads_anchor[i], NULL, FragmentRefine, &anchor_thread_args[i]);
    }

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_3 = (rb_state0.var_3) ^ (((rb_state0.var_0) == (0x547c3f3e)) ? (rb_state0.var_3) : (0xb9bcbd6a));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_2);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0xda29d72c)) ? (rb_state11.var_0) : (0x64bfdecd));
    #endif
    struct thread_args chunk_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_3) == (0x0)) {
            if ((rb_state9.var_4) != (0x0)) {
                if (!((rb_state9.var_4) == (rb_state9.var_23))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_2) == (0xb46e6085)) {
            pthread_mutex_lock(&(rb_state11.lock_26));
            rb_state11.var_5 = 0x52025fb1;
            rb_state11.var_6 = (rb_state11.var_6) ^ (((((0x7caedb91) ^ (rb_state11.var_5)) - (rb_state11.var_4)) - (rb_state11.var_6)) + (rb_state11.var_3));
            rb_state11.var_7 = (rb_state11.var_7) + (rb_state11.var_7);
            rb_state11.var_8 = (rb_state11.var_8) - (((rb_state11.var_2) == (0xb46e6085)) ? (rb_state11.var_8) : (0x5e59b7a5));
            rb_state11.var_9 = (rb_state11.var_9) + (((0x9b2148f4) - (0xf8009906)) + (rb_state11.var_5));
            rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_6);
            rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x743a315c));
            rb_state11.var_12 = (rb_state11.var_12) + (rb_state11.var_9);
            rb_state11.var_13 = (rb_state11.var_13) ^ (((((0x170fa8f1) - (rb_state11.var_10)) + (rb_state11.var_9)) - (rb_state11.var_11)) ^ (rb_state11.var_10));
            rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_12) : (0x10a80821));
            rb_state11.var_15 = (rb_state11.var_15) - (rb_state11.var_13);
            rb_state11.var_16 = (rb_state11.var_16) - (((rb_state11.var_14) == (0xf5230f9f)) ? (rb_state11.var_15) : (0x4817cec2));
            rb_state11.var_4 = (rb_state11.var_4) ^ (rb_state11.var_16);
            rb_state11.var_3 = rb_state11.var_4;
            pthread_mutex_unlock(&(rb_state11.lock_26));
        }
        if ((rb_state11.var_2) == (0xb46e6085)) {
            if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x23c17a68)) {
            rb_state18.var_5 = 0xaddf06ca;
            rb_state18.var_6 = (rb_state18.var_6) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0x4661f291));
            rb_state18.var_7 = (rb_state18.var_7) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_5) : (0x43172b3c));
            rb_state18.var_8 = (rb_state18.var_8) + (((0xf9fdeb03) - (rb_state18.var_8)) - (rb_state18.var_6));
            rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_7) == (0x5220f936)) ? (rb_state18.var_8) : (0x4d87305b));
            rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_9);
        }
        #endif
        chunk_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) + (405408 < rb_input_size ? (uint32_t)rb_input[405408] : 0x6feaca5);
        if ((rb_state2.var_2) == (0xc9)) {
            pthread_mutex_lock(&(rb_state2.lock_33));
            rb_state2.var_6 = 0x8633f54a;
            rb_state2.var_7 = 0x341b0b2d;
            rb_state2.var_8 = (rb_state2.var_8) ^ (rb_state2.var_5);
            rb_state2.var_9 = (rb_state2.var_9) ^ (((((0xef07563f) - (rb_state2.var_6)) ^ (rb_state2.var_6)) + (rb_state2.var_7)) ^ (rb_state2.var_7));
            rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_4) : (0xc27cd960));
            rb_state2.var_11 = (rb_state2.var_11) ^ (((rb_state2.var_9) == (0x16e0abc1)) ? (rb_state2.var_8) : (0x4e3b3827));
            rb_state2.var_12 = (rb_state2.var_12) + (((0x2332bf05) + (rb_state2.var_9)) - (0x17b8bd41));
            rb_state2.var_13 = (rb_state2.var_13) - (((0x57e7bb16) - (rb_state2.var_10)) ^ (0xf0289d97));
            rb_state2.var_14 = (rb_state2.var_14) + (((((0x379c7f01) ^ (rb_state2.var_12)) + (rb_state2.var_11)) ^ (rb_state2.var_10)) - (0x39c41444));
            rb_state2.var_15 = (rb_state2.var_15) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_13) : (0x40ba0813));
            rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_14) == (0xdc02be40)) ? (rb_state2.var_15) : (0xbd120d2d));
            rb_state2.var_5 = (rb_state2.var_5) + (rb_state2.var_16);
            rb_state2.var_4 = rb_state2.var_5;
            pthread_mutex_unlock(&(rb_state2.lock_33));
        }
        if ((rb_state2.var_2) == (0xc9)) {
            if (!((rb_state2.var_4) == (rb_state2.var_5))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (0x49f5aa46);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) - ((0xae948d76) ^ (0xb5177281));
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0xffffff93)) {
            rb_state10.var_9 = 0xb067f5c6;
            rb_state10.var_10 = 0xfc2b45ec;
            rb_state10.var_11 = (rb_state10.var_11) + (rb_state10.var_8);
            rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_9);
            rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_10) : (0x2b4208a8));
            rb_state10.var_14 = (rb_state10.var_14) + (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_11) : (0xd203ec1d));
            rb_state10.var_15 = (rb_state10.var_15) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_12) : (0x270e6a0f));
            rb_state10.var_16 = (rb_state10.var_16) ^ (((0xfc2bd728) - (rb_state10.var_13)) ^ (rb_state10.var_11));
            rb_state10.var_17 = (rb_state10.var_17) ^ (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_15) : (0x237532ec));
            rb_state10.var_18 = (rb_state10.var_18) - (((((0x43dc439a) + (rb_state10.var_17)) - (rb_state10.var_16)) ^ (rb_state10.var_12)) + (0xb127fddf));
            rb_state10.var_8 = (rb_state10.var_8) ^ (rb_state10.var_18);
            rb_state10.var_3 = rb_state10.var_8;
        }
        if ((rb_state10.var_0) == (0xffffff93)) {
            pthread_mutex_lock(&(rb_state10.lock_30));
            rb_state10.var_20 = 0xc5ff6b7e;
            rb_state10.var_21 = 0xc9a06c4d;
            rb_state10.var_22 = 0x5ac05dc1;
            rb_state10.var_23 = (rb_state10.var_23) ^ (((((0xcbd76915) ^ (rb_state10.var_13)) + (0x881ad369)) ^ (rb_state10.var_21)) ^ (rb_state10.var_20));
            rb_state10.var_24 = (rb_state10.var_24) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_22) : (0x46eb6612));
            rb_state10.var_25 = (rb_state10.var_25) + (((0xb048c07b) - (rb_state10.var_23)) ^ (rb_state10.var_15));
            rb_state10.var_26 = (rb_state10.var_26) + (rb_state10.var_24);
            rb_state10.var_27 = (rb_state10.var_27) + (((rb_state10.var_16) == (0x0)) ? (rb_state10.var_25) : (0x427eea31));
            rb_state10.var_28 = (rb_state10.var_28) ^ (rb_state10.var_26);
            rb_state10.var_29 = (rb_state10.var_29) + (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_28) : (0x770a02da));
            rb_state10.var_19 = (rb_state10.var_19) - (rb_state10.var_29);
            rb_state10.var_3 = rb_state10.var_19;
            pthread_mutex_unlock(&(rb_state10.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) - (0x1dff4f98);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_3) == (0xb2)) {
            rb_state14.var_15 = 0xf0060fb5;
            rb_state14.var_16 = 0xd71b9a16;
            rb_state14.var_17 = (rb_state14.var_17) ^ (((0xb1f86df0) + (rb_state14.var_8)) - (rb_state14.var_15));
            rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_9) == (0x17a7fee7)) ? (rb_state14.var_7) : (0xde98852));
            rb_state14.var_19 = (rb_state14.var_19) - (((rb_state14.var_10) == (0x88f0e22)) ? (rb_state14.var_16) : (0xfaa789d5));
            rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_11) == (0xae008d82)) ? (rb_state14.var_17) : (0xd3c4f224));
            rb_state14.var_21 = (rb_state14.var_21) ^ (((((0x7487d3fe) + (rb_state14.var_18)) - (rb_state14.var_12)) + (0x6fcaabdb)) + (rb_state14.var_19));
            rb_state14.var_22 = (rb_state14.var_22) ^ (((0x58e5c272) + (rb_state14.var_13)) + (rb_state14.var_20));
            rb_state14.var_23 = (rb_state14.var_23) + (((((0xb832cffb) - (0xe59ac51f)) + (rb_state14.var_21)) - (rb_state14.var_22)) + (0xa9da1a8c));
            rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_23);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) ^ (637449 < rb_input_size ? (uint32_t)rb_input[637449] : 0x45169169);
        if ((rb_state15.var_0) == (0xb70f6efd)) {
            pthread_mutex_lock(&(rb_state15.lock_25));
            rb_state15.var_4 = rb_state15.var_1;
            pthread_mutex_unlock(&(rb_state15.lock_25));
        }
        if ((rb_state15.var_0) == (0xb70f6efd)) {
            if (!((rb_state15.var_1) == (rb_state15.var_4))) {
                racebench_trigger(15);
            }
        }
        #endif
        pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
    }

    struct thread_args compress_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0xd2315e0a);
        #endif
        compress_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) - ((0x8f1fadde) + (0xdf736462));
        if ((rb_state4.var_0) == (0xa462bab8)) {
            pthread_mutex_lock(&(rb_state4.lock_7));
            rb_state4.var_5 = 0xc2016b3b;
            rb_state4.var_6 = (rb_state4.var_6) - (((((0x9e5b7945) - (rb_state4.var_4)) + (rb_state4.var_6)) ^ (rb_state4.var_5)) + (0x53b62e65));
            rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_6);
            pthread_mutex_unlock(&(rb_state4.lock_7));
        }
        #endif
        pthread_create(&threads_compress[i], NULL, Compress, &compress_thread_args[i]);
    }

    struct thread_args send_block_args;
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x62)) {
        pthread_mutex_lock(&(rb_state1.lock_13));
        rb_state1.var_3 = rb_state1.var_0;
        pthread_mutex_unlock(&(rb_state1.lock_13));
    }
    if ((rb_state1.var_2) == (0x62)) {
        if (!((rb_state1.var_0) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
    }
    #endif
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