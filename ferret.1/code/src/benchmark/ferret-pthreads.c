#include "racebench_bugs.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <cass.h>
#include <cass_timer.h>
#include <../image/image.h>
#include "tpool.h"
#include "queue.h"

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#define DEFAULT_DEPTH 25
#define MAXR 100
#define IMAGE_DIM 14

const char *db_dir = NULL;
const char *table_name = NULL;
const char *query_dir = NULL;
const char *output_path = NULL;

FILE *fout;

int NTHREAD_LOAD = 1;
int NTHREAD_SEG = 1;
int NTHREAD_EXTRACT = 1;
int NTHREAD_VEC = 1;
int NTHREAD_RANK = 1;
int NTHREAD_OUT = 1;
int DEPTH = DEFAULT_DEPTH;

int top_K = 10;

char *extra_params = "-L 8 - T 20";

cass_env_t *env;
cass_table_t *table;
cass_table_t *query_table;

int vec_dist_id = 0;
int vecset_dist_id = 0;

struct load_data {
    int width, height;
    char *name;
    unsigned char *HSV, *RGB;
};

struct queue q_load_seg;

struct seg_data {
    int width, height, nrgn;
    char *name;
    unsigned char *mask;
    unsigned char *HSV;
};

struct queue q_seg_extract;

struct extract_data {
    cass_dataset_t ds;
    char *name;
};

struct queue q_extract_vec;

struct vec_query_data {
    char *name;
    cass_dataset_t *ds;
    cass_result_t result;
};

struct queue q_vec_rank;

struct rank_data {
    char *name;
    cass_dataset_t *ds;
    cass_result_t result;
};

struct queue q_rank_out;

int cnt_enqueue;
int cnt_dequeue;
char path[BUFSIZ];

int scan_dir(const char *, char *head);

int dir_helper(char *dir, char *head) {
    DIR *pd = NULL;
    struct dirent *ent = NULL;
    int result = 0;
    pd = opendir(dir);
    if (pd == NULL) {
        goto except;
    }
    for (;;) {
        ent = readdir(pd);
        if (ent == NULL) {
            break;
        }
        if (scan_dir(ent->d_name, head) != 0) {
            return -1;
        }
    }
    goto final;

except:
    result = -1;
    perror("Error:");
final:
    if (pd != NULL) {
        closedir(pd);
    }
    return result;
}

int file_helper(const char *file) {
    int r;
    struct load_data *data;

    data = (struct load_data *)malloc(sizeof(struct load_data));
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0xfffffff8)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0xfffffff8)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    assert(data != NULL);

    data->name = strdup(file);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ ((0x940b970) - (rb_state2.var_0));
    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x940b970)) ? (rb_state2.var_0) : (0xcf824a2a));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_8 = (rb_state15.var_8) + (rb_state15.var_9);
    if ((rb_state15.var_10) == (0x6f9782f6)) {
        pthread_mutex_lock(&(rb_state15.lock_38));
        rb_state15.var_20 = 0xd4e9e46c;
        rb_state15.var_21 = 0x16b68cda;
        rb_state15.var_22 = 0xea94fec3;
        rb_state15.var_23 = (rb_state15.var_23) + (((((0x1bc4b905) - (rb_state15.var_20)) + (rb_state15.var_21)) ^ (rb_state15.var_11)) - (rb_state15.var_12));
        rb_state15.var_24 = (rb_state15.var_24) + (((((0x3f6a73f7) + (rb_state15.var_23)) - (0xbf5e9d2c)) - (0x7fd22862)) ^ (rb_state15.var_22));
        rb_state15.var_19 = (rb_state15.var_19) - (rb_state15.var_24);
        rb_state15.var_18 = rb_state15.var_19;
        pthread_mutex_unlock(&(rb_state15.lock_38));
    }
    if ((rb_state15.var_10) == (0x6f9782f6)) {
        if (!((rb_state15.var_18) == (rb_state15.var_19))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_0);
    #endif
    r = image_read_rgb_hsv(file, &data->width, &data->height, &data->RGB, &data->HSV);
    assert(r == 0);

    cnt_enqueue++;
    enqueue(&q_load_seg, data);

    return 0;
}

int scan_dir(const char *dir, char *head) {
    struct stat st;
    int ret;

    if (dir[0] == '.') {
        if (dir[1] == 0) {
            return 0;
        } else if (dir[1] == '.') {
            if (dir[2] == 0) {
                return 0;
            }
        }
    }

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x16)) {
        if ((rb_state4.var_1) != (0x0)) {
            if (!((rb_state4.var_1) == (rb_state4.var_42))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (443 < rb_input_size ? (uint32_t)rb_input[443] : 0xcd9c2ca5);
    #endif
    strcat(head, dir);
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x931e13a9)) {
        rb_state7.var_2 = rb_state7.var_1;
    }
    if ((rb_state7.var_0) == (0x931e13a9)) {
        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
            racebench_trigger(7);
        }
    }
    #endif
    ret = stat(path, &st);
    if (ret != 0) {
        perror("Error:");
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x7c6198c1)) {
            rb_state5.var_19 = 0x91b6565c;
            rb_state5.var_20 = 0xa66d4003;
            rb_state5.var_21 = (rb_state5.var_21) + (((0x46319b29) + (rb_state5.var_16)) ^ (0x9d0d37ec));
            rb_state5.var_22 = (rb_state5.var_22) + (((rb_state5.var_19) == (0x91b6565c)) ? (rb_state5.var_20) : (0xcc669c9f));
            rb_state5.var_23 = (rb_state5.var_23) - (rb_state5.var_21);
            rb_state5.var_24 = (rb_state5.var_24) + (rb_state5.var_22);
            rb_state5.var_25 = (rb_state5.var_25) ^ (((((0xe60512c3) ^ (0xee7b9c6c)) ^ (rb_state5.var_23)) ^ (rb_state5.var_24)) + (rb_state5.var_17));
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_25);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0xfffffffc)) ? (rb_state10.var_0) : (0x55158f7e));
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x2cc9895c)) {
            pthread_mutex_lock(&(rb_state13.lock_34));
            rb_state13.var_3 = 0x8b9dbe28;
            rb_state13.var_4 = 0x7d569a11;
            rb_state13.var_5 = (rb_state13.var_5) + (((0x693b8601) + (rb_state13.var_1)) + (0x301cd87e));
            rb_state13.var_6 = (rb_state13.var_6) - (((((0xae928e53) ^ (rb_state13.var_2)) ^ (rb_state13.var_3)) + (rb_state13.var_3)) + (rb_state13.var_4));
            rb_state13.var_7 = (rb_state13.var_7) - (rb_state13.var_5);
            rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_4) == (0x7d569a11)) ? (rb_state13.var_6) : (0x8cc9e03d));
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_5) == (0x99585e7f)) ? (rb_state13.var_7) : (0x14d44e7c));
            rb_state13.var_10 = (rb_state13.var_10) + (((0xc8231e18) ^ (rb_state13.var_8)) ^ (0x83f3ffe6));
            rb_state13.var_11 = (rb_state13.var_11) - (((0x9a737135) - (rb_state13.var_9)) - (0x3fbd1c96));
            rb_state13.var_12 = (rb_state13.var_12) - (((0xdf98e2da) ^ (rb_state13.var_10)) + (rb_state13.var_6));
            rb_state13.var_13 = (rb_state13.var_13) + (((0xe8025bcc) ^ (rb_state13.var_7)) ^ (rb_state13.var_11));
            rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_12) == (0x73b7fd24)) ? (rb_state13.var_13) : (0x1d0a50c0));
            rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_14);
            rb_state13.var_1 = rb_state13.var_2;
            pthread_mutex_unlock(&(rb_state13.lock_34));
        }
        if ((rb_state13.var_0) == (0x2cc9895c)) {
            if (!((rb_state13.var_1) == (rb_state13.var_2))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x5ffe9ab6)) {
            pthread_mutex_lock(&(rb_state15.lock_16));
            rb_state15.var_3 = 0xa42787ff;
            rb_state15.var_4 = 0x90687d0a;
            rb_state15.var_5 = (rb_state15.var_5) ^ (((0xdd7b5590) - (rb_state15.var_2)) ^ (rb_state15.var_3));
            rb_state15.var_6 = (rb_state15.var_6) - (((0x87cdd451) + (0x4c074bf6)) - (rb_state15.var_1));
            rb_state15.var_7 = (rb_state15.var_7) - (((rb_state15.var_3) == (0xa42787ff)) ? (rb_state15.var_4) : (0x29128a6));
            rb_state15.var_8 = (rb_state15.var_8) + (((rb_state15.var_4) == (0x90687d0a)) ? (rb_state15.var_5) : (0x75fe1b45));
            rb_state15.var_9 = (rb_state15.var_9) + (((rb_state15.var_5) == (0x795cd26f)) ? (rb_state15.var_6) : (0xaf1315be));
            rb_state15.var_10 = (rb_state15.var_10) + (rb_state15.var_7);
            rb_state15.var_11 = (rb_state15.var_11) - (rb_state15.var_8);
            rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_9) == (0x2c2adfb9)) ? (rb_state15.var_10) : (0x27e00ea8));
            rb_state15.var_13 = (rb_state15.var_13) ^ (((0x32db7f21) - (rb_state15.var_6)) + (rb_state15.var_11));
            rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_7) == (0x6f9782f6)) ? (rb_state15.var_12) : (0x3ce7985));
            rb_state15.var_15 = (rb_state15.var_15) + (((rb_state15.var_13) == (0x8d53ccf9)) ? (rb_state15.var_14) : (0xee2f6041));
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_15);
            rb_state15.var_2 = rb_state15.var_1;
            pthread_mutex_unlock(&(rb_state15.lock_16));
        }
        #endif
        file_helper(path);
    } else if (S_ISDIR(st.st_mode)) {
        strcat(head, "/");
        dir_helper(path, head + strlen(head));
    }

    head[0] = 0;
    return 0;
}

void *t_load(void *dummy) {
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (303 < rb_input_size ? (uint32_t)rb_input[303] : 0xa1be97c4);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - ((0x6ce1eb7f) ^ (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
    if ((rb_state11.var_0) == (0x0)) {
        rb_state11.var_28 = (rb_state11.var_28) ^ (((((0xc8fd9c1c) ^ (rb_state11.var_18)) ^ (rb_state11.var_16)) + (rb_state11.var_19)) ^ (rb_state11.var_20));
        rb_state11.var_29 = (rb_state11.var_29) - (((((0xaa423677) - (rb_state11.var_28)) + (rb_state11.var_22)) - (rb_state11.var_21)) ^ (rb_state11.var_17));
        rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_29);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xbff8c15b));
    if ((rb_state14.var_0) == (0x0)) {
        rb_state14.var_18 = (rb_state14.var_18) - (rb_state14.var_7);
        rb_state14.var_19 = (rb_state14.var_19) + (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_9) : (0x7defd167));
        rb_state14.var_20 = (rb_state14.var_20) - (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_8) : (0xa4375174));
        rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_18) : (0xcd99ce3f));
        rb_state14.var_22 = (rb_state14.var_22) + (((((0xfb711370) ^ (rb_state14.var_13)) - (rb_state14.var_19)) ^ (rb_state14.var_20)) + (0xbfe69ec9));
        rb_state14.var_23 = (rb_state14.var_23) ^ (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_21) : (0x27838e7b));
        rb_state14.var_24 = (rb_state14.var_24) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_22) : (0xeb825f81));
        rb_state14.var_25 = (rb_state14.var_25) ^ (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_24) : (0xd2f23e67));
        rb_state14.var_17 = (rb_state14.var_17) - (rb_state14.var_25);
        rb_state14.var_1 = rb_state14.var_17;
    }
    #endif
    const char *dir = (const char *)dummy;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        if ((rb_state3.var_1) != (0x0)) {
            if (!((rb_state3.var_1) == (rb_state3.var_26))) {
                racebench_trigger(3);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (815 < rb_input_size ? (uint32_t)rb_input[815] : 0x2e74d97e);
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state14.lock_32));
        rb_state14.var_27 = 0x8bc7f631;
        rb_state14.var_28 = (rb_state14.var_28) - (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_17) : (0x3663a8ff));
        rb_state14.var_29 = (rb_state14.var_29) + (rb_state14.var_27);
        rb_state14.var_30 = (rb_state14.var_30) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_28) : (0xcc608ced));
        rb_state14.var_31 = (rb_state14.var_31) - (((((0x70bc7405) ^ (0x264cc767)) ^ (rb_state14.var_30)) + (rb_state14.var_29)) + (rb_state14.var_19));
        rb_state14.var_26 = (rb_state14.var_26) ^ (rb_state14.var_31);
        rb_state14.var_1 = rb_state14.var_26;
        pthread_mutex_unlock(&(rb_state14.lock_32));
    }
    #endif
    path[0] = 0;

    if (strcmp(dir, ".") == 0) {
        dir_helper(".", path);
    } else {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (0x61e1eadc);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (566 < rb_input_size ? (uint32_t)rb_input[566] : 0xdba29ff9);
        #endif
        scan_dir(dir, path);
    }

    queue_signal_terminate(&q_load_seg);
    return NULL;
}

void *t_seg(void *dummy) {
    struct seg_data *seg;
    struct load_data *load;

    while (1) {
        if (dequeue(&q_load_seg, &load) < 0) {
            break;
        }

        assert(load != NULL);
        seg = (struct seg_data *)calloc(1, sizeof(struct seg_data));

        seg->name = load->name;

        seg->width = load->width;
        seg->height = load->height;
        seg->HSV = load->HSV;
        image_segment(&seg->mask, &seg->nrgn, load->RGB, load->width, load->height);

        free(load->RGB);
        free(load);

        enqueue(&q_seg_extract, seg);
    }

    queue_signal_terminate(&q_seg_extract);
    return NULL;
}

void *t_extract(void *dummy) {
    struct seg_data *seg;
    struct extract_data *extract;

    while (1) {
        if (dequeue(&q_seg_extract, &seg) < 0) {
            break;
        }

        assert(seg != NULL);
        extract = (struct extract_data *)calloc(1, sizeof(struct extract_data));

        extract->name = seg->name;

        image_extract_helper(seg->HSV, seg->mask, seg->width, seg->height, seg->nrgn, &extract->ds);

        free(seg->mask);
        free(seg->HSV);
        free(seg);

        enqueue(&q_extract_vec, extract);
    }

    queue_signal_terminate(&q_extract_vec);
    return NULL;
}

void *t_vec(void *dummy) {
    struct extract_data *extract;
    struct vec_query_data *vec;
    cass_query_t query;
    while (1) {
        if (dequeue(&q_extract_vec, &extract) < 0) {
            break;
        }

        assert(extract != NULL);
        vec = (struct vec_query_data *)calloc(1, sizeof(struct vec_query_data));
        vec->name = extract->name;

        memset(&query, 0, sizeof query);
        query.flags = CASS_RESULT_LISTS | CASS_RESULT_USERMEM;

        vec->ds = query.dataset = &extract->ds;
        query.vecset_id = 0;

        query.vec_dist_id = vec_dist_id;

        query.vecset_dist_id = vecset_dist_id;

        query.topk = 2 * top_K;

        query.extra_params = extra_params;

        cass_result_alloc_list(&vec->result, vec->ds->vecset[0].num_regions, query.topk);

        cass_table_query(table, &query, &vec->result);

        enqueue(&q_vec_rank, vec);
    }

    queue_signal_terminate(&q_vec_rank);
    return NULL;
}

void *t_rank(void *dummy) {
    struct vec_query_data *vec;
    struct rank_data *rank;
    cass_result_t *candidate;
    cass_query_t query;
    while (1) {
        if (dequeue(&q_vec_rank, &vec) < 0) {
            break;
        }

        assert(vec != NULL);

        rank = (struct rank_data *)calloc(1, sizeof(struct rank_data));
        rank->name = vec->name;

        query.flags = CASS_RESULT_LIST | CASS_RESULT_USERMEM | CASS_RESULT_SORT;
        query.dataset = vec->ds;
        query.vecset_id = 0;

        query.vec_dist_id = vec_dist_id;

        query.vecset_dist_id = vecset_dist_id;

        query.topk = top_K;

        query.extra_params = NULL;

        candidate = cass_result_merge_lists(&vec->result, (cass_dataset_t *)query_table->__private, 0);
        query.candidate = candidate;

        cass_result_alloc_list(&rank->result, 0, top_K);
        cass_table_query(query_table, &query, &rank->result);

        cass_result_free(&vec->result);
        cass_result_free(candidate);
        free(candidate);
        cass_dataset_release(vec->ds);
        free(vec->ds);
        free(vec);
        enqueue(&q_rank_out, rank);
    }

    queue_signal_terminate(&q_rank_out);
    return NULL;
}

void *t_out(void *dummy) {
    struct rank_data *rank;
    while (1) {
        if (dequeue(&q_rank_out, &rank) < 0) {
            break;
        }

        assert(rank != NULL);

        fprintf(fout, "%s", rank->name);

        ARRAY_BEGIN_FOREACH(rank->result.u.list, cass_list_entry_t p) {
            char *obj = NULL;
            if (p.dist >= CASS_DIST_MAX) {
                continue;
            }
            cass_map_id_to_dataobj(query_table->map, p.id, &obj);
            assert(obj != NULL);
            fprintf(fout, "\t%s:%g", obj, p.dist);
        }
        ARRAY_END_FOREACH;

        fprintf(fout, "\n");

        cass_result_free(&rank->result);
        free(rank->name);
        free(rank);

        cnt_dequeue++;

        fprintf(stderr, "(%d,%d)\n", cnt_enqueue, cnt_dequeue);
    }

    assert(cnt_enqueue == cnt_dequeue);
    return NULL;
}

int main(int argc, char *argv[]) {
    stimer_t tmr;

    tdesc_t *t_load_desc;
    tdesc_t *t_seg_desc;
    tdesc_t *t_extract_desc;
    tdesc_t *t_vec_desc;
    tdesc_t *t_rank_desc;
    tdesc_t *t_out_desc;

    tpool_t *p_load;
    tpool_t *p_seg;
    tpool_t *p_extract;
    tpool_t *p_vec;
    tpool_t *p_rank;
    tpool_t *p_out;

    int ret, i;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    printf("PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) "\n");
    fflush(NULL);
#else
    printf("PARSEC Benchmark Suite\n");
    fflush(NULL);
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_ferret);
#endif

    if (argc < 8) {
        printf("%s <database> <table> <query dir> <top K> <depth> <n> <out>\n", argv[0]);
        return 0;
    }

    db_dir = argv[1];
    table_name = argv[2];
    query_dir = argv[3];
    top_K = atoi(argv[4]);

    DEPTH = atoi(argv[5]);
    NTHREAD_SEG = atoi(argv[6]);
    NTHREAD_EXTRACT = atoi(argv[6]);
    NTHREAD_VEC = atoi(argv[6]);
    NTHREAD_RANK = atoi(argv[6]);

    output_path = argv[7];

    fout = fopen(output_path, "w");
    assert(fout != NULL);

    cass_init();

    ret = cass_env_open(&env, db_dir, 0);
    if (ret != 0) {
        printf("ERROR: %s\n", cass_strerror(ret));
        return 0;
    }

    vec_dist_id = cass_reg_lookup(&env->vec_dist, "L2_float");
    assert(vec_dist_id >= 0);

    vecset_dist_id = cass_reg_lookup(&env->vecset_dist, "emd");
    assert(vecset_dist_id >= 0);

    i = cass_reg_lookup(&env->table, table_name);

    table = query_table = cass_reg_get(&env->table, i);

    i = table->parent_id;

    if (i >= 0) {
        query_table = cass_reg_get(&env->table, i);
    }

    if (query_table != table) {
        cass_table_load(query_table);
    }

    cass_map_load(query_table->map);

    cass_table_load(table);

    image_init(argv[0]);

    stimer_tick(&tmr);
    queue_init(&q_load_seg, DEPTH, NTHREAD_LOAD);
    queue_init(&q_seg_extract, DEPTH, NTHREAD_SEG);
    queue_init(&q_extract_vec, DEPTH, NTHREAD_EXTRACT);
    queue_init(&q_vec_rank, DEPTH, NTHREAD_VEC);
    queue_init(&q_rank_out, DEPTH, NTHREAD_RANK);

    t_load_desc = (tdesc_t *)calloc(NTHREAD_LOAD, sizeof(tdesc_t));
    t_seg_desc = (tdesc_t *)calloc(NTHREAD_SEG, sizeof(tdesc_t));
    t_extract_desc = (tdesc_t *)calloc(NTHREAD_EXTRACT, sizeof(tdesc_t));
    t_vec_desc = (tdesc_t *)calloc(NTHREAD_VEC, sizeof(tdesc_t));
    t_rank_desc = (tdesc_t *)calloc(NTHREAD_RANK, sizeof(tdesc_t));
    t_out_desc = (tdesc_t *)calloc(NTHREAD_OUT, sizeof(tdesc_t));

    t_load_desc[0].attr = NULL;
    t_load_desc[0].start_routine = t_load;
    t_load_desc[0].arg = query_dir;

    for (i = 1; i < NTHREAD_LOAD; i++) {
        t_load_desc[i] = t_load_desc[0];
    }

    t_seg_desc[0].attr = NULL;
    t_seg_desc[0].start_routine = t_seg;
    t_seg_desc[0].arg = NULL;

    for (i = 1; i < NTHREAD_SEG; i++) {
        t_seg_desc[i] = t_seg_desc[0];
    }

    t_extract_desc[0].attr = NULL;
    t_extract_desc[0].start_routine = t_extract;
    t_extract_desc[0].arg = NULL;

    for (i = 1; i < NTHREAD_EXTRACT; i++) {
        t_extract_desc[i] = t_extract_desc[0];
    }

    t_vec_desc[0].attr = NULL;
    t_vec_desc[0].start_routine = t_vec;
    t_vec_desc[0].arg = NULL;
    for (i = 1; i < NTHREAD_VEC; i++) {
        t_vec_desc[i] = t_vec_desc[0];
    }

    t_rank_desc[0].attr = NULL;
    t_rank_desc[0].start_routine = t_rank;
    t_rank_desc[0].arg = NULL;
    for (i = 1; i < NTHREAD_RANK; i++) {
        t_rank_desc[i] = t_rank_desc[0];
    }

    t_out_desc[0].attr = NULL;
    t_out_desc[0].start_routine = t_out;
    t_out_desc[0].arg = NULL;
    for (i = 1; i < NTHREAD_OUT; i++) {
        t_out_desc[i] = t_out_desc[0];
    }

    cnt_enqueue = cnt_dequeue = 0;

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif
    p_load = tpool_create(t_load_desc, NTHREAD_LOAD);
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_10) == (0x6f9782f6)) {
        rb_state15.var_25 = 0x9d06d352;
        rb_state15.var_26 = (rb_state15.var_26) - (0xf4427fab);
        rb_state15.var_27 = (rb_state15.var_27) ^ (((rb_state15.var_13) == (0x8d53ccf9)) ? (rb_state15.var_14) : (0xb328a2b));
        rb_state15.var_28 = (rb_state15.var_28) + (0x6757ca34);
        rb_state15.var_29 = (rb_state15.var_29) + (((rb_state15.var_15) == (0x6f9782f6)) ? (rb_state15.var_25) : (0x48cc4219));
        rb_state15.var_30 = (rb_state15.var_30) + (((rb_state15.var_26) == (0xbbd8055)) ? (rb_state15.var_27) : (0xdd2437d9));
        rb_state15.var_31 = (rb_state15.var_31) - (((rb_state15.var_17) == (0x0)) ? (rb_state15.var_28) : (0x7bae33df));
        rb_state15.var_32 = (rb_state15.var_32) ^ (((rb_state15.var_18) == (0x7ebf796a)) ? (rb_state15.var_29) : (0xf37dd3fb));
        rb_state15.var_33 = (rb_state15.var_33) - (((0x4f03d9da) - (rb_state15.var_19)) ^ (rb_state15.var_30));
        rb_state15.var_34 = (rb_state15.var_34) - (((rb_state15.var_31) == (0x98a835cc)) ? (rb_state15.var_32) : (0xce839131));
        rb_state15.var_35 = (rb_state15.var_35) - (((rb_state15.var_20) == (0xd4e9e46c)) ? (rb_state15.var_33) : (0xad9eebd5));
        rb_state15.var_36 = (rb_state15.var_36) - (((0x84790862) - (rb_state15.var_34)) + (rb_state15.var_21));
        rb_state15.var_37 = (rb_state15.var_37) ^ (((((0x2794ecc5) ^ (rb_state15.var_36)) - (rb_state15.var_35)) ^ (0xe2bd0e05)) - (rb_state15.var_22));
        rb_state15.var_18 = (rb_state15.var_18) + (rb_state15.var_37);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (329 < rb_input_size ? (uint32_t)rb_input[329] : 0x2489701d);
    #endif
    p_seg = tpool_create(t_seg_desc, NTHREAD_SEG);
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_26 = (rb_state16.var_26) + (2010 < rb_input_size ? (uint32_t)rb_input[2010] : 0x24594192);
        rb_state16.var_27 = (rb_state16.var_27) - (((0x5fcecda) - (rb_state16.var_13)) ^ (rb_state16.var_15));
        rb_state16.var_28 = (rb_state16.var_28) ^ (rb_state16.var_14);
        rb_state16.var_29 = (rb_state16.var_29) - (((rb_state16.var_12) == (0xb8a1f18c)) ? (rb_state16.var_26) : (0x1de57cb6));
        rb_state16.var_30 = (rb_state16.var_30) - (((rb_state16.var_16) == (0x6731981)) ? (rb_state16.var_27) : (0x51d4e3b3));
        rb_state16.var_31 = (rb_state16.var_31) + (((((0x6d20d20a) - (rb_state16.var_29)) ^ (rb_state16.var_28)) - (rb_state16.var_17)) + (rb_state16.var_18));
        rb_state16.var_32 = (rb_state16.var_32) - (((0x55d1d3a7) - (rb_state16.var_19)) ^ (rb_state16.var_30));
        rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_20) == (0xa102edec)) ? (rb_state16.var_31) : (0x44731d2a));
        rb_state16.var_34 = (rb_state16.var_34) + (rb_state16.var_32);
        rb_state16.var_35 = (rb_state16.var_35) - (((((0xcf48d1fc) ^ (rb_state16.var_34)) + (rb_state16.var_21)) ^ (0x355fa759)) ^ (rb_state16.var_33));
        rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_35);
    }
    #endif
    p_extract = tpool_create(t_extract_desc, NTHREAD_EXTRACT);
    p_vec = tpool_create(t_vec_desc, NTHREAD_VEC);
    p_rank = tpool_create(t_rank_desc, NTHREAD_RANK);
    p_out = tpool_create(t_out_desc, NTHREAD_OUT);

    tpool_join(p_out, NULL);
    tpool_join(p_rank, NULL);
    tpool_join(p_vec, NULL);
    tpool_join(p_extract, NULL);
    tpool_join(p_seg, NULL);
    tpool_join(p_load, NULL);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    tpool_destroy(p_load);
    tpool_destroy(p_seg);
    tpool_destroy(p_extract);
    tpool_destroy(p_vec);
    tpool_destroy(p_rank);
    tpool_destroy(p_out);

    free(t_load_desc);
    free(t_seg_desc);
    free(t_extract_desc);
    free(t_vec_desc);
    free(t_rank_desc);
    free(t_out_desc);

    queue_destroy(&q_load_seg);
    queue_destroy(&q_seg_extract);
    queue_destroy(&q_extract_vec);
    queue_destroy(&q_vec_rank);
    queue_destroy(&q_rank_out);

    stimer_tuck(&tmr, "QUERY TIME");

    ret = cass_env_close(env, 0);
    if (ret != 0) {
        printf("ERROR: %s\n", cass_strerror(ret));
        return 0;
    }

    cass_cleanup();

    image_cleanup();

    fclose(fout);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif
    return 0;
}