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

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (931272 < rb_input_size ? (uint32_t)rb_input[931272] : 0x73eec8c8);
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
    rb_state4.var_0 = (rb_state4.var_0) + (1002829 < rb_input_size ? (uint32_t)rb_input[1002829] : 0x10d4d281);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (rb_state12.var_0);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (0x297f1ab);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ ((0x2159c830) + (0x4f988058));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_2);
    rb_state0.var_2 = (rb_state0.var_2) - (0x802ffe45);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (422439 < rb_input_size ? (uint32_t)rb_input[422439] : 0x501f0ce8);
    if ((rb_state7.var_1) == (0x8e933b8a)) {
        pthread_mutex_lock(&(rb_state7.lock_41));
        rb_state7.var_5 = 0xf6ffe1a;
        rb_state7.var_6 = 0x5629d8ec;
        rb_state7.var_7 = (rb_state7.var_7) ^ (((0xdcfaee36) ^ (rb_state7.var_4)) + (rb_state7.var_6));
        rb_state7.var_8 = (rb_state7.var_8) + (rb_state7.var_5);
        rb_state7.var_9 = (rb_state7.var_9) - (((0xf6e3e205) + (rb_state7.var_5)) + (rb_state7.var_6));
        rb_state7.var_10 = (rb_state7.var_10) ^ (((0x45d9433b) + (0x5d911b7f)) + (rb_state7.var_3));
        rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_7);
        rb_state7.var_12 = (rb_state7.var_12) + (rb_state7.var_8);
        rb_state7.var_13 = (rb_state7.var_13) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x5c7549f));
        rb_state7.var_14 = (rb_state7.var_14) ^ (rb_state7.var_11);
        rb_state7.var_15 = (rb_state7.var_15) + (((((0x3dddca44) ^ (rb_state7.var_12)) ^ (rb_state7.var_13)) ^ (rb_state7.var_7)) - (0xfeb41878));
        rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_14) : (0x12439f3b));
        rb_state7.var_17 = (rb_state7.var_17) ^ (((((0x9a33f21) + (rb_state7.var_16)) ^ (rb_state7.var_9)) ^ (0x53088c88)) + (rb_state7.var_15));
        rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_17);
        rb_state7.var_3 = rb_state7.var_4;
        pthread_mutex_unlock(&(rb_state7.lock_41));
    }
    if ((rb_state7.var_1) == (0x8e933b8a)) {
        if (!((rb_state7.var_3) == (rb_state7.var_4))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x81cc5d60)) {
        rb_state11.var_20 = 0xa2a3c31c;
        rb_state11.var_21 = (rb_state11.var_21) - (((rb_state11.var_15) == (0x3aa59efa)) ? (rb_state11.var_14) : (0x7d6dbf25));
        rb_state11.var_22 = (rb_state11.var_22) ^ (379197 < rb_input_size ? (uint32_t)rb_input[379197] : 0x4d35e1bf);
        rb_state11.var_23 = (rb_state11.var_23) ^ (((((0xd4f16b2) ^ (rb_state11.var_13)) - (rb_state11.var_17)) ^ (rb_state11.var_16)) - (rb_state11.var_12));
        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_20) == (0xa2a3c31c)) ? (rb_state11.var_21) : (0xe3167006));
        rb_state11.var_25 = (rb_state11.var_25) - (rb_state11.var_22);
        rb_state11.var_26 = (rb_state11.var_26) ^ (((rb_state11.var_23) == (0xcaa3e586)) ? (rb_state11.var_24) : (0xebe394dc));
        rb_state11.var_27 = (rb_state11.var_27) + (((rb_state11.var_18) == (0x3464ac7f)) ? (rb_state11.var_25) : (0x365c8f26));
        rb_state11.var_28 = (rb_state11.var_28) + (rb_state11.var_26);
        rb_state11.var_29 = (rb_state11.var_29) - (((rb_state11.var_27) == (0xffffffd7)) ? (rb_state11.var_28) : (0x227b60d2));
        rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_29);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (0xa799931f);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_0) == (0x5d41db5d)) ? (rb_state16.var_2) : (0x42308ff0));
    #endif
    if (thread_stats == NULL)
        EXIT_TRACE("Memory allocation failed.\n");
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0xb207940d);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) ^ (365306 < rb_input_size ? (uint32_t)rb_input[365306] : 0x89f7dcd7);
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x8e933b8a)) {
        rb_state7.var_18 = 0x5514e00;
        rb_state7.var_19 = 0xb688a495;
        rb_state7.var_20 = (rb_state7.var_20) + (0x5365582d);
        rb_state7.var_21 = (rb_state7.var_21) - (0x89ef6120);
        rb_state7.var_22 = (rb_state7.var_22) - (((rb_state7.var_11) == (0xccdb38de)) ? (rb_state7.var_10) : (0xe12cd8d0));
        rb_state7.var_23 = (rb_state7.var_23) ^ (((0x236856bd) ^ (rb_state7.var_12)) - (rb_state7.var_19));
        rb_state7.var_24 = (rb_state7.var_24) - (((rb_state7.var_18) == (0x5514e00)) ? (rb_state7.var_20) : (0xfb75a20));
        rb_state7.var_25 = (rb_state7.var_25) + (((0xf35ccfaa) - (rb_state7.var_21)) + (rb_state7.var_13));
        rb_state7.var_26 = (rb_state7.var_26) - (((0x71fc3c4e) ^ (rb_state7.var_14)) + (rb_state7.var_22));
        rb_state7.var_27 = (rb_state7.var_27) ^ (((0x2b4e6ce9) - (rb_state7.var_23)) - (0xc89f3aec));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((0x14869b93) ^ (0xc18168c0)) ^ (rb_state7.var_24));
        rb_state7.var_29 = (rb_state7.var_29) ^ (((0x817694fa) + (rb_state7.var_15)) ^ (rb_state7.var_25));
        rb_state7.var_30 = (rb_state7.var_30) + (((0x87daef51) ^ (rb_state7.var_26)) + (0x64d24dd4));
        rb_state7.var_31 = (rb_state7.var_31) ^ (((0x925a40fe) ^ (rb_state7.var_16)) + (rb_state7.var_27));
        rb_state7.var_32 = (rb_state7.var_32) ^ (((rb_state7.var_28) == (0x799d5480)) ? (rb_state7.var_29) : (0x7f9b6210));
        rb_state7.var_33 = (rb_state7.var_33) + (((0x61e2ff53) ^ (rb_state7.var_17)) - (rb_state7.var_30));
        rb_state7.var_34 = (rb_state7.var_34) ^ (((rb_state7.var_18) == (0x5514e00)) ? (rb_state7.var_31) : (0x7776effb));
        rb_state7.var_35 = (rb_state7.var_35) + (((rb_state7.var_19) == (0xb688a495)) ? (rb_state7.var_32) : (0x8f2fe28f));
        rb_state7.var_36 = (rb_state7.var_36) - (((rb_state7.var_20) == (0x5365582d)) ? (rb_state7.var_33) : (0x66da8704));
        rb_state7.var_37 = (rb_state7.var_37) + (((rb_state7.var_34) == (0x6d4a0db0)) ? (rb_state7.var_35) : (0xa2176ddc));
        rb_state7.var_38 = (rb_state7.var_38) ^ (((0x9e8b4dde) - (0xeb1a42bc)) - (rb_state7.var_36));
        rb_state7.var_39 = (rb_state7.var_39) ^ (((rb_state7.var_21) == (0x76109ee0)) ? (rb_state7.var_37) : (0x3abe43fc));
        rb_state7.var_40 = (rb_state7.var_40) + (((rb_state7.var_38) == (0x7df064b2)) ? (rb_state7.var_39) : (0x58127e94));
        rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_40);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (558523 < rb_input_size ? (uint32_t)rb_input[558523] : 0xfed245b6);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (718687 < rb_input_size ? (uint32_t)rb_input[718687] : 0x61f8944);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) + (0xa99f2d23);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (227469 < rb_input_size ? (uint32_t)rb_input[227469] : 0x5612d7fe);
    #endif
    init_stats(thread_stats);
#endif

    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xffffffc2)) {
        rb_state17.var_12 = 0xadce68d;
        rb_state17.var_13 = (rb_state17.var_13) - ((0x128a201f) + (0x26be94ff));
        rb_state17.var_14 = (rb_state17.var_14) ^ (((rb_state17.var_12) == (0xadce68d)) ? (rb_state17.var_9) : (0xd85348a4));
        rb_state17.var_15 = (rb_state17.var_15) ^ ((0x31f97a14) ^ (rb_state17.var_10));
        rb_state17.var_16 = (rb_state17.var_16) ^ (((0x72603919) - (rb_state17.var_8)) + (rb_state17.var_11));
        rb_state17.var_17 = (rb_state17.var_17) ^ (((0x30415836) - (0xcec8959e)) - (rb_state17.var_13));
        rb_state17.var_18 = (rb_state17.var_18) - (rb_state17.var_14);
        rb_state17.var_19 = (rb_state17.var_19) - (rb_state17.var_15);
        rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_16);
        rb_state17.var_21 = (rb_state17.var_21) ^ (((((0xb736c155) - (rb_state17.var_12)) - (rb_state17.var_17)) - (rb_state17.var_18)) - (0x882d1394));
        rb_state17.var_22 = (rb_state17.var_22) + (((rb_state17.var_13) == (0xc6b74ae2)) ? (rb_state17.var_19) : (0xb6bee84e));
        rb_state17.var_23 = (rb_state17.var_23) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_20) : (0xc967d719));
        rb_state17.var_24 = (rb_state17.var_24) ^ (((rb_state17.var_15) == (0x31f97a14)) ? (rb_state17.var_21) : (0xf6477074));
        rb_state17.var_25 = (rb_state17.var_25) + (((rb_state17.var_22) == (0xce0685ec)) ? (rb_state17.var_23) : (0x6f7f2a1d));
        rb_state17.var_26 = (rb_state17.var_26) - (((rb_state17.var_24) == (0x896b4f7e)) ? (rb_state17.var_25) : (0x1f5447ba));
        rb_state17.var_11 = (rb_state17.var_11) - (rb_state17.var_26);
        rb_state17.var_3 = rb_state17.var_11;
    }
    if ((rb_state17.var_0) == (0xffffffc2)) {
        pthread_mutex_lock(&(rb_state17.lock_38));
        rb_state17.var_28 = 0x4c0f6142;
        rb_state17.var_29 = 0x79d0d04d;
        rb_state17.var_30 = 0x7731cb8f;
        rb_state17.var_31 = (rb_state17.var_31) - (rb_state17.var_30);
        rb_state17.var_32 = (rb_state17.var_32) - (((rb_state17.var_29) == (0x79d0d04d)) ? (rb_state17.var_28) : (0x99c2dcad));
        rb_state17.var_33 = (rb_state17.var_33) + (rb_state17.var_31);
        rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_16) == (0x72603919)) ? (rb_state17.var_32) : (0x6e15b4a3));
        rb_state17.var_35 = (rb_state17.var_35) - (rb_state17.var_33);
        rb_state17.var_36 = (rb_state17.var_36) - (rb_state17.var_34);
        rb_state17.var_37 = (rb_state17.var_37) - (((rb_state17.var_35) == (0x7731cb8f)) ? (rb_state17.var_36) : (0xa83be3f8));
        rb_state17.var_27 = (rb_state17.var_27) - (rb_state17.var_37);
        rb_state17.var_3 = rb_state17.var_27;
        pthread_mutex_unlock(&(rb_state17.lock_38));
    }
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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x6c98e464));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ ((0xbca768b2) - (rb_state17.var_2));
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0xf0364d9);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (248984 < rb_input_size ? (uint32_t)rb_input[248984] : 0x152ccc83);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (0x9bd933ce);
    #endif
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    chunk_t *chunk;
    int r;

    ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_0) == (0x77fc146a)) ? (rb_state2.var_2) : (0xf5e79a19));
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (472527 < rb_input_size ? (uint32_t)rb_input[472527] : 0x18e7f8b6);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (1083099 < rb_input_size ? (uint32_t)rb_input[1083099] : 0x2de74199);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_1);
    rb_state16.var_2 = (rb_state16.var_2) ^ ((0x4cd8d3be) - (0x8aeffc67));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xb17d77e3));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) ^ (194686 < rb_input_size ? (uint32_t)rb_input[194686] : 0x264a78fe);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0xea89112f)) {
        rb_state18.var_7 = 0xf4349bf5;
        rb_state18.var_8 = 0x3d5ab358;
        rb_state18.var_9 = 0x2f5465d3;
        rb_state18.var_10 = (rb_state18.var_10) ^ (rb_state18.var_6);
        rb_state18.var_11 = (rb_state18.var_11) ^ (rb_state18.var_9);
        rb_state18.var_12 = (rb_state18.var_12) + (rb_state18.var_7);
        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_10) : (0x7f0220d));
        rb_state18.var_14 = (rb_state18.var_14) - (((((0xb1b77070) - (rb_state18.var_12)) - (rb_state18.var_7)) ^ (rb_state18.var_8)) ^ (rb_state18.var_11));
        rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_13);
        rb_state18.var_16 = (rb_state18.var_16) + (rb_state18.var_14);
        rb_state18.var_17 = (rb_state18.var_17) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_15) : (0xb5384e2f));
        rb_state18.var_18 = (rb_state18.var_18) ^ (((0x2d4e3d78) - (rb_state18.var_10)) + (rb_state18.var_16));
        rb_state18.var_19 = (rb_state18.var_19) - (((0x4cd987d8) ^ (rb_state18.var_17)) ^ (rb_state18.var_11));
        rb_state18.var_20 = (rb_state18.var_20) + (((((0x899581ca) ^ (rb_state18.var_19)) + (rb_state18.var_18)) ^ (0x4fb74acc)) - (rb_state18.var_12));
        rb_state18.var_6 = (rb_state18.var_6) - (rb_state18.var_20);
        rb_state18.var_3 = rb_state18.var_6;
    }
    if ((rb_state18.var_1) == (0xea89112f)) {
        pthread_mutex_lock(&(rb_state18.lock_27));
        rb_state18.var_22 = 0x11a06146;
        rb_state18.var_23 = 0xb5bc5d1b;
        rb_state18.var_24 = 0x960ed5b8;
        rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_22) == (0x0)) ? (rb_state18.var_24) : (0x81e8f12e));
        rb_state18.var_26 = (rb_state18.var_26) + (((((0x2ce056d3) - (0xfcf6a2f1)) ^ (rb_state18.var_25)) + (rb_state18.var_23)) ^ (rb_state18.var_13));
        rb_state18.var_21 = (rb_state18.var_21) + (rb_state18.var_26);
        rb_state18.var_3 = rb_state18.var_21;
        pthread_mutex_unlock(&(rb_state18.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_0);
    #endif
    init_stats(thread_stats);
#endif

    r = 0;
    r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xb448fb25)) {
        pthread_mutex_lock(&(rb_state4.lock_6));
        rb_state4.var_3 = rb_state4.var_2;
        pthread_mutex_unlock(&(rb_state4.lock_6));
    }
    if ((rb_state4.var_1) == (0x8628496e)) {
        if (!((rb_state4.var_2) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) + (0x2a62ba8b);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xffffffc2)) {
        if ((rb_state17.var_3) != (0x0)) {
            if (!((rb_state17.var_3) == (rb_state17.var_27))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
    r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x81)) {
        pthread_mutex_lock(&(rb_state3.lock_16));
        rb_state3.var_4 = 0x450e2b4d;
        rb_state3.var_5 = 0xfc5d8031;
        rb_state3.var_6 = (rb_state3.var_6) - (rb_state3.var_4);
        rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_3) : (0xd8035ef8));
        rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_4) : (0xde42afe2));
        rb_state3.var_9 = (rb_state3.var_9) ^ (((0xcbaa1afa) - (rb_state3.var_6)) + (rb_state3.var_6));
        rb_state3.var_10 = (rb_state3.var_10) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_7) : (0x37ca57b9));
        rb_state3.var_11 = (rb_state3.var_11) - (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_8) : (0x63fd1d22));
        rb_state3.var_12 = (rb_state3.var_12) ^ (((0x5709b73a) ^ (rb_state3.var_9)) + (rb_state3.var_9));
        rb_state3.var_13 = (rb_state3.var_13) + (((rb_state3.var_10) == (0x0)) ? (rb_state3.var_11) : (0x1d0fcd57));
        rb_state3.var_14 = (rb_state3.var_14) ^ (((0x4e639471) ^ (0xda0cf4a1)) ^ (rb_state3.var_12));
        rb_state3.var_15 = (rb_state3.var_15) - (((((0x8447229a) - (0x139cc0b9)) - (rb_state3.var_14)) + (0x2b118fc1)) + (rb_state3.var_13));
        rb_state3.var_2 = (rb_state3.var_2) + (rb_state3.var_15);
        pthread_mutex_unlock(&(rb_state3.lock_16));
    }
    #endif
    assert(r == 0);

    while (1) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_12
            if ((rb_state12.var_2) == (0x7f282fa1)) {
                rb_state12.var_10 = 0x68c8a48e;
                rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_6) : (0x1447d93b));
                rb_state12.var_12 = (rb_state12.var_12) ^ (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0x5d0d3907));
                rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_12);
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
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xa4ffdbf7));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (190062 < rb_input_size ? (uint32_t)rb_input[190062] : 0x8dbf974f);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    const int qid = args->tid / MAX_THREADS_PER_QUEUE;
    ringbuffer_t recv_buf, send_buf;
    int r;

    chunk_t *temp;
    chunk_t *chunk;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ ((0xcadfb545) + (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ ((0xb60cd8c6) ^ (rb_state14.var_1));
    #endif
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    r = 0;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) - (749026 < rb_input_size ? (uint32_t)rb_input[749026] : 0x925b6d51);
    #endif
    r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
    r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (744506 < rb_input_size ? (uint32_t)rb_input[744506] : 0x2c20f8d0);
    rb_state15.var_2 = (rb_state15.var_2) - ((0x667df124) - (0x192f4587));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - ((0x746b4961) ^ (rb_state16.var_1));
    #endif
    assert(r == 0);

#ifdef ENABLE_STATISTICS
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0xffffff14)) ? (rb_state6.var_1) : (0x224bec5b));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (772589 < rb_input_size ? (uint32_t)rb_input[772589] : 0xa03a6cfb);
    rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_2);
    if ((rb_state18.var_1) == (0xea89112f)) {
        if ((rb_state18.var_3) != (0x0)) {
            if (!((rb_state18.var_3) == (rb_state18.var_21))) {
                rb_state18.var_28 = rb_state18.var_14;
            }
            if (!((rb_state18.var_3) == (rb_state18.var_21))) {
                if (!((rb_state18.var_14) == (rb_state18.var_28))) {
                    racebench_trigger(18);
                }
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x167b1acb));
    #endif
    stats_t *thread_stats = malloc(sizeof(stats_t));
    if (thread_stats == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (0xc50a4687);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x77fc146a)) {
        pthread_mutex_lock(&(rb_state2.lock_30));
        rb_state2.var_4 = 0x4545f7de;
        rb_state2.var_5 = 0x25945980;
        rb_state2.var_6 = (rb_state2.var_6) + (431913 < rb_input_size ? (uint32_t)rb_input[431913] : 0xd77b465a);
        rb_state2.var_7 = (rb_state2.var_7) + (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_4) : (0xae7be2cd));
        rb_state2.var_8 = (rb_state2.var_8) + (rb_state2.var_3);
        rb_state2.var_9 = (rb_state2.var_9) - (((0xd998f11) ^ (rb_state2.var_5)) ^ (0x62920142));
        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0x4108071c));
        rb_state2.var_11 = (rb_state2.var_11) - (((((0x741c4160) - (rb_state2.var_8)) ^ (rb_state2.var_6)) ^ (rb_state2.var_7)) ^ (rb_state2.var_7));
        rb_state2.var_12 = (rb_state2.var_12) - (((0x464e7062) ^ (rb_state2.var_9)) + (0x1dcee823));
        rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_10) : (0xaab6834c));
        rb_state2.var_14 = (rb_state2.var_14) - (((0x6a7a4cd5) + (rb_state2.var_9)) + (rb_state2.var_11));
        rb_state2.var_15 = (rb_state2.var_15) ^ (((0xb99d23a9) ^ (rb_state2.var_12)) - (rb_state2.var_10));
        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_13);
        rb_state2.var_17 = (rb_state2.var_17) ^ (((0xdc337237) - (rb_state2.var_14)) + (0x9c1133f5));
        rb_state2.var_18 = (rb_state2.var_18) - (((0xb9ee6b4e) ^ (rb_state2.var_11)) + (rb_state2.var_15));
        rb_state2.var_19 = (rb_state2.var_19) ^ (rb_state2.var_16);
        rb_state2.var_20 = (rb_state2.var_20) - (rb_state2.var_17);
        rb_state2.var_21 = (rb_state2.var_21) + (((0x72ca382a) ^ (rb_state2.var_18)) ^ (0x3cc50aa1));
        rb_state2.var_22 = (rb_state2.var_22) ^ (((0x20f22ab) - (rb_state2.var_19)) + (rb_state2.var_12));
        rb_state2.var_23 = (rb_state2.var_23) - (((0xb156cd87) - (rb_state2.var_20)) + (rb_state2.var_13));
        rb_state2.var_24 = (rb_state2.var_24) ^ (((((0x772fce79) ^ (rb_state2.var_15)) - (rb_state2.var_14)) + (rb_state2.var_22)) - (rb_state2.var_21));
        rb_state2.var_25 = (rb_state2.var_25) ^ (((rb_state2.var_16) == (0x0)) ? (rb_state2.var_23) : (0x3ceb6274));
        rb_state2.var_26 = (rb_state2.var_26) ^ (((0x2774a56b) - (0xe492b5e5)) - (rb_state2.var_24));
        rb_state2.var_27 = (rb_state2.var_27) ^ (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_25) : (0xc03e90c8));
        rb_state2.var_28 = (rb_state2.var_28) ^ (rb_state2.var_26);
        rb_state2.var_29 = (rb_state2.var_29) ^ (((rb_state2.var_27) == (0x0)) ? (rb_state2.var_28) : (0x3941983d));
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_29);
        pthread_mutex_unlock(&(rb_state2.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_0) : (0x1e2314));
    #endif
    init_stats(thread_stats);
#endif

    while (TRUE) {

        if (ringbuffer_isEmpty(&recv_buf)) {
            #ifdef RACEBENCH_BUG_13
            if ((rb_state13.var_0) == (0xdcf192e8)) {
                rb_state13.var_4 = 0xaff4211c;
                rb_state13.var_5 = (rb_state13.var_5) ^ (((((0x2e363038) ^ (rb_state13.var_3)) ^ (rb_state13.var_5)) + (0xb4b7009d)) - (rb_state13.var_4));
                rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_5);
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
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (0x77fc146a);
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (0x455ebc77);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
    #endif
    size_t preloading_buffer_seek = 0;
    int qid = 0;
    int fd = args->fd;
    int i;

    ringbuffer_t send_buf;
    sequence_number_t anchorcount = 0;
    int r;

    chunk_t *temp = NULL;
    chunk_t *chunk = NULL;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
    #endif
    u32int *rabintab = malloc(256 * sizeof rabintab[0]);
    u32int *rabinwintab = malloc(256 * sizeof rabintab[0]);
    if (rabintab == NULL || rabinwintab == NULL) {
        EXIT_TRACE("Memory allocation failed.\n");
    }

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (0x22e6805e);
    #endif
    r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (0x7e33a39f);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0xce0f06f0));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
    #endif
    assert(r == 0);

    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (0x9adb55c);
    #endif
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
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (437322 < rb_input_size ? (uint32_t)rb_input[437322] : 0xbabe8c28);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_9 = 0xa6fafe9c;
        rb_state14.var_10 = 0xf28c0757;
        rb_state14.var_11 = (rb_state14.var_11) ^ (((0xd52a819b) + (rb_state14.var_7)) + (rb_state14.var_9));
        rb_state14.var_12 = (rb_state14.var_12) ^ (984094 < rb_input_size ? (uint32_t)rb_input[984094] : 0xae22b466);
        rb_state14.var_13 = (rb_state14.var_13) - (rb_state14.var_10);
        rb_state14.var_14 = (rb_state14.var_14) + (((0xe4f7e03b) - (0xfb03a838)) ^ (rb_state14.var_6));
        rb_state14.var_15 = (rb_state14.var_15) - (((0x87234527) ^ (0xf7a14e1c)) - (rb_state14.var_11));
        rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_12) == (0x48)) ? (rb_state14.var_13) : (0xcf9a7d1));
        rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_14) == (0xe9f43803)) ? (rb_state14.var_15) : (0x55ee0e04));
        rb_state14.var_18 = (rb_state14.var_18) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_16) : (0x9ef62ee9));
        rb_state14.var_19 = (rb_state14.var_19) ^ (((rb_state14.var_9) == (0xa6fafe9c)) ? (rb_state14.var_17) : (0xfb1c84f1));
        rb_state14.var_20 = (rb_state14.var_20) - (((rb_state14.var_10) == (0xf28c0757)) ? (rb_state14.var_18) : (0x8b87d1bc));
        rb_state14.var_21 = (rb_state14.var_21) ^ (((((0x595ea79) - (rb_state14.var_19)) + (rb_state14.var_20)) + (0x639cf59e)) ^ (rb_state14.var_11));
        rb_state14.var_8 = (rb_state14.var_8) - (rb_state14.var_21);
        rb_state14.var_3 = rb_state14.var_8;
    }
    #endif
    struct thread_args *args = (struct thread_args *)targs;
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_23 = 0x394fa504;
        rb_state14.var_24 = 0x37ccb1f8;
        rb_state14.var_25 = (rb_state14.var_25) ^ (((rb_state14.var_14) == (0xe9f43803)) ? (rb_state14.var_13) : (0x85937001));
        rb_state14.var_26 = (rb_state14.var_26) + (((rb_state14.var_24) == (0x37ccb1f8)) ? (rb_state14.var_23) : (0x9f5e7f7f));
        rb_state14.var_27 = (rb_state14.var_27) ^ (((rb_state14.var_12) == (0x48)) ? (rb_state14.var_25) : (0x6d9ee767));
        rb_state14.var_28 = (rb_state14.var_28) ^ (((((0x5876ce9e) + (rb_state14.var_27)) - (0x179feffd)) + (rb_state14.var_26)) + (0x51a1cbe3));
        rb_state14.var_22 = (rb_state14.var_22) - (rb_state14.var_28);
        rb_state14.var_3 = rb_state14.var_22;
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x0)) {
        if ((rb_state16.var_3) != (0x0)) {
            if (!((rb_state16.var_3) == (rb_state16.var_31))) {
                racebench_trigger(16);
            }
        }
    }
    #endif
    int qid = 0;
    int fd = 0;

    ringbuffer_t recv_buf;
    chunk_t *chunk;

    SearchTree T;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0xccf3c4c7) + (0x7700e58a));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xc8280816)) {
        if ((rb_state6.var_3) != (0x0)) {
            if (!((rb_state6.var_3) == (rb_state6.var_22))) {
                pthread_mutex_lock(&(rb_state6.lock_50));
                rb_state6.var_34 = 0xae2bcccf;
                rb_state6.var_35 = 0xaf4dbc52;
                rb_state6.var_36 = (rb_state6.var_36) + (((rb_state6.var_16) == (0xcfffa9c1)) ? (rb_state6.var_35) : (0x59fbcfdb));
                rb_state6.var_37 = (rb_state6.var_37) + (341905 < rb_input_size ? (uint32_t)rb_input[341905] : 0x3b248f2a);
                rb_state6.var_38 = (rb_state6.var_38) - (rb_state6.var_34);
                rb_state6.var_39 = (rb_state6.var_39) - (((rb_state6.var_36) == (0xaf4dbc52)) ? (rb_state6.var_37) : (0x98c730a7));
                rb_state6.var_40 = (rb_state6.var_40) ^ (((((0x6f713524) + (rb_state6.var_17)) ^ (rb_state6.var_39)) ^ (rb_state6.var_18)) + (rb_state6.var_38));
                rb_state6.var_33 = (rb_state6.var_33) + (rb_state6.var_40);
                rb_state6.var_32 = rb_state6.var_33;
                pthread_mutex_unlock(&(rb_state6.lock_50));
            }
            if (!((rb_state6.var_3) == (rb_state6.var_22))) {
                pthread_mutex_lock(&(rb_state6.lock_50));
                if (!((rb_state6.var_32) == (rb_state6.var_33))) {
                    racebench_trigger(6);
                }
                pthread_mutex_unlock(&(rb_state6.lock_50));
            }
        }
    }
    #endif
    T = TreeMakeEmpty(NULL);
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_1) == (0x3cdff29f)) ? (rb_state12.var_0) : (0x6c88be11));
    #endif
    Position pos = NULL;
    struct tree_element tele;

    sequence_t next;
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0xb0e12dbb)) {
        rb_state0.var_3 = rb_state0.var_2;
    }
    if ((rb_state0.var_1) == (0xb0e12dbb)) {
        if (!((rb_state0.var_2) == (rb_state0.var_3))) {
            racebench_trigger(0);
        }
    }
    #endif
    sequence_reset(&next);

    sequence_number_t *chunks_per_anchor;
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_4 = 0x6b1515f6;
        rb_state15.var_5 = 0x485b253e;
        rb_state15.var_6 = (rb_state15.var_6) + ((0x8e7caa34) ^ (rb_state15.var_4));
        rb_state15.var_7 = (rb_state15.var_7) ^ (((((0x3e3ca4be) + (0x11e348b3)) + (0x80fc4c6e)) ^ (rb_state15.var_4)) ^ (rb_state15.var_3));
        rb_state15.var_8 = (rb_state15.var_8) ^ (rb_state15.var_5);
        rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_6);
        rb_state15.var_10 = (rb_state15.var_10) ^ (((((0x52d62d4f) + (rb_state15.var_7)) ^ (rb_state15.var_5)) + (rb_state15.var_6)) + (rb_state15.var_8));
        rb_state15.var_11 = (rb_state15.var_11) ^ (((rb_state15.var_9) == (0x1a96403e)) ? (rb_state15.var_10) : (0x904999b4));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_11);
    }
    #endif
    unsigned int chunks_per_anchor_max = 1024;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) - ((0x34d5579f) - (0x9bd3890f));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (251598 < rb_input_size ? (uint32_t)rb_input[251598] : 0xa98a81f5);
    #endif
    chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
    if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0xe262ca91)) ? (rb_state10.var_0) : (0x6a94cf16));
    #endif
    memset(chunks_per_anchor, 0, chunks_per_anchor_max * sizeof(sequence_number_t));
    int r;
    int i;

    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (0x48e7a3fc);
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_0) == (0x48e7a2c1)) ? (rb_state9.var_2) : (0xf02bdb02));
    #endif
    r = ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (210373 < rb_input_size ? (uint32_t)rb_input[210373] : 0xdc194e3a);
    rb_state5.var_1 = (rb_state5.var_1) - ((0x2e29845b) ^ (0xe2d1470));
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

    struct thread_args anchor_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (318918 < rb_input_size ? (uint32_t)rb_input[318918] : 0x794544b7);
        #endif
        anchor_thread_args[i].tid = i;
        pthread_create(&threads_anchor[i], NULL, FragmentRefine, &anchor_thread_args[i]);
    }

    struct thread_args chunk_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (0x428fdb2f);
        #endif
        chunk_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (510986 < rb_input_size ? (uint32_t)rb_input[510986] : 0x51a13263);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_1);
        #endif
        pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
    }

    struct thread_args compress_thread_args[conf->nthreads];
    for (i = 0; i < conf->nthreads; i++) {
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xc1a33071));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_1);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + (rb_state19.var_1);
        #endif
        compress_thread_args[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (611991 < rb_input_size ? (uint32_t)rb_input[611991] : 0xbbf9c98e);
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x77fc146a)) {
            rb_state2.var_3 = rb_state2.var_1;
        }
        if ((rb_state2.var_0) == (0x77fc146a)) {
            pthread_mutex_lock(&(rb_state2.lock_30));
            if (!((rb_state2.var_1) == (rb_state2.var_3))) {
                racebench_trigger(2);
            }
            pthread_mutex_unlock(&(rb_state2.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (652261 < rb_input_size ? (uint32_t)rb_input[652261] : 0x3b629b42);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x81cc5d60)) {
            pthread_mutex_lock(&(rb_state11.lock_30));
            rb_state11.var_4 = 0x41476508;
            rb_state11.var_5 = 0x27d4b1a9;
            rb_state11.var_6 = (rb_state11.var_6) - (rb_state11.var_3);
            rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_4) : (0x65adceeb));
            rb_state11.var_8 = (rb_state11.var_8) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_5) : (0xadc46b48));
            rb_state11.var_9 = (rb_state11.var_9) + (((0xe7a93115) ^ (rb_state11.var_6)) - (rb_state11.var_2));
            rb_state11.var_10 = (rb_state11.var_10) ^ (((0xa0c84a12) + (rb_state11.var_6)) ^ (rb_state11.var_7));
            rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0x6e02dd8f));
            rb_state11.var_12 = (rb_state11.var_12) - (((0x53ee85cc) + (rb_state11.var_8)) ^ (rb_state11.var_9));
            rb_state11.var_13 = (rb_state11.var_13) ^ (((0x70d8a014) - (rb_state11.var_9)) ^ (rb_state11.var_10));
            rb_state11.var_14 = (rb_state11.var_14) + (((0xa2658b3b) ^ (rb_state11.var_10)) + (rb_state11.var_11));
            rb_state11.var_15 = (rb_state11.var_15) - (((0xaa8aece7) + (rb_state11.var_12)) ^ (0xac5da050));
            rb_state11.var_16 = (rb_state11.var_16) - (rb_state11.var_13);
            rb_state11.var_17 = (rb_state11.var_17) + (((0x6c904015) - (rb_state11.var_11)) ^ (rb_state11.var_14));
            rb_state11.var_18 = (rb_state11.var_18) ^ (((((0xcde468a2) + (rb_state11.var_15)) ^ (0x228afe26)) + (rb_state11.var_16)) ^ (0x422f4fbd));
            rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0x50270636));
            rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_19);
            rb_state11.var_2 = rb_state11.var_3;
            pthread_mutex_unlock(&(rb_state11.lock_30));
        }
        if ((rb_state11.var_0) == (0x81cc5d60)) {
            if (!((rb_state11.var_2) == (rb_state11.var_3))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_2);
        #endif
        pthread_create(&threads_compress[i], NULL, Compress, &compress_thread_args[i]);
    }

    struct thread_args send_block_args;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0xc0069958));
    #endif
    send_block_args.tid = 0;
    send_block_args.nqueues = nqueues;
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (0xc8280816);
    #endif
    pthread_create(&threads_send, NULL, Reorder, &send_block_args);

    stats_t *threads_anchor_rv[conf->nthreads];
    stats_t *threads_chunk_rv[conf->nthreads];
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xc8280816)) {
        rb_state6.var_16 = 0xcfffa9c1;
        rb_state6.var_17 = 0x403b6a0b;
        rb_state6.var_18 = (rb_state6.var_18) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_17) : (0x60bd56df));
        rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0xa493191d));
        rb_state6.var_20 = (rb_state6.var_20) + (((((0x7373516b) - (0x8004f1a)) ^ (rb_state6.var_16)) - (rb_state6.var_18)) - (rb_state6.var_10));
        rb_state6.var_21 = (rb_state6.var_21) - (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0x1104a4b2));
        rb_state6.var_15 = (rb_state6.var_15) ^ (rb_state6.var_21);
        rb_state6.var_3 = rb_state6.var_15;
    }
    if ((rb_state6.var_2) == (0xc8280816)) {
        pthread_mutex_lock(&(rb_state6.lock_31));
        rb_state6.var_23 = 0x368fde3d;
        rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_23) : (0x93057045));
        rb_state6.var_25 = (rb_state6.var_25) - (495462 < rb_input_size ? (uint32_t)rb_input[495462] : 0xb65344a2);
        rb_state6.var_26 = (rb_state6.var_26) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_11) : (0xf97d16eb));
        rb_state6.var_27 = (rb_state6.var_27) ^ (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_24) : (0x10d5b850));
        rb_state6.var_28 = (rb_state6.var_28) - (((rb_state6.var_25) == (0xffffffcd)) ? (rb_state6.var_26) : (0xb0970063));
        rb_state6.var_29 = (rb_state6.var_29) ^ (((rb_state6.var_15) == (0x9baebe7b)) ? (rb_state6.var_27) : (0x8479df78));
        rb_state6.var_30 = (rb_state6.var_30) ^ (((rb_state6.var_28) == (0x0)) ? (rb_state6.var_29) : (0x2bc2db15));
        rb_state6.var_22 = (rb_state6.var_22) + (rb_state6.var_30);
        rb_state6.var_3 = rb_state6.var_22;
        pthread_mutex_unlock(&(rb_state6.lock_31));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_2) == (0x0)) {
        rb_state16.var_18 = 0x3e5ea05e;
        rb_state16.var_19 = 0x8e5eb21c;
        rb_state16.var_20 = (rb_state16.var_20) ^ (((0xeb2e88df) ^ (0x1eb37089)) + (rb_state16.var_7));
        rb_state16.var_21 = (rb_state16.var_21) + (((((0xaac6a36b) - (rb_state16.var_9)) - (rb_state16.var_18)) - (rb_state16.var_19)) + (rb_state16.var_8));
        rb_state16.var_22 = (rb_state16.var_22) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_20) : (0x764f034f));
        rb_state16.var_23 = (rb_state16.var_23) - (rb_state16.var_21);
        rb_state16.var_24 = (rb_state16.var_24) - (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_22) : (0x21396260));
        rb_state16.var_25 = (rb_state16.var_25) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_23) : (0x385515c2));
        rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_24);
        rb_state16.var_27 = (rb_state16.var_27) ^ (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_25) : (0x9ff05507));
        rb_state16.var_28 = (rb_state16.var_28) - (rb_state16.var_26);
        rb_state16.var_29 = (rb_state16.var_29) + (rb_state16.var_27);
        rb_state16.var_30 = (rb_state16.var_30) ^ (((rb_state16.var_28) == (0xf59df856)) ? (rb_state16.var_29) : (0x5650dab8));
        rb_state16.var_17 = (rb_state16.var_17) - (rb_state16.var_30);
        rb_state16.var_3 = rb_state16.var_17;
    }
    if ((rb_state16.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_36));
        rb_state16.var_32 = 0x8b1e46ff;
        rb_state16.var_33 = (rb_state16.var_33) ^ (((rb_state16.var_32) == (0x8b1e46ff)) ? (rb_state16.var_15) : (0x6a26bed2));
        rb_state16.var_34 = (rb_state16.var_34) - (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_14) : (0x7d1ab66c));
        rb_state16.var_35 = (rb_state16.var_35) + (((((0x7d687e04) - (0x38c305d1)) ^ (0x1751b203)) ^ (rb_state16.var_34)) + (rb_state16.var_33));
        rb_state16.var_31 = (rb_state16.var_31) + (rb_state16.var_35);
        rb_state16.var_3 = rb_state16.var_31;
        pthread_mutex_unlock(&(rb_state16.lock_36));
    }
    #endif
    stats_t *threads_compress_rv[conf->nthreads];

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_1);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) ^ ((0x51d6895c) + (0x3e868fa6));
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