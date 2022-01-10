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

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) - ((0x7bef09d7) ^ (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_9 = (rb_state15.var_9) ^ (1998 < rb_input_size ? (uint32_t)rb_input[1998] : 0xfa32a65a);
    rb_state15.var_10 = (rb_state15.var_10) + (452 < rb_input_size ? (uint32_t)rb_input[452] : 0x1ec7a4fb);
    if ((rb_state15.var_11) == (0xa4c1be04)) {
        rb_state15.var_36 = 0xd9c2ef7d;
        rb_state15.var_37 = 0x71fceb1c;
        rb_state15.var_38 = (rb_state15.var_38) + (((((0x3a216cd5) ^ (rb_state15.var_24)) + (rb_state15.var_36)) ^ (0xd8e1a549)) + (rb_state15.var_23));
        rb_state15.var_39 = (rb_state15.var_39) - (0xc6535f81);
        rb_state15.var_40 = (rb_state15.var_40) ^ (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_37) : (0xd906e958));
        rb_state15.var_41 = (rb_state15.var_41) - (((rb_state15.var_38) == (0xcb05f91b)) ? (rb_state15.var_39) : (0x3df2e720));
        rb_state15.var_42 = (rb_state15.var_42) ^ (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_40) : (0xd1fabc8a));
        rb_state15.var_43 = (rb_state15.var_43) ^ (((0xe532ed65) + (0x7968e179)) - (rb_state15.var_41));
        rb_state15.var_44 = (rb_state15.var_44) + (((rb_state15.var_42) == (0x71fceb1c)) ? (rb_state15.var_43) : (0xf7eb2680));
        rb_state15.var_35 = (rb_state15.var_35) ^ (rb_state15.var_44);
        rb_state15.var_18 = rb_state15.var_35;
    }
    if ((rb_state15.var_11) == (0xa4c1be04)) {
        pthread_mutex_lock(&(rb_state15.lock_53));
        rb_state15.var_46 = 0xa21c827a;
        rb_state15.var_47 = 0x10e609a7;
        rb_state15.var_48 = (rb_state15.var_48) - (1209 < rb_input_size ? (uint32_t)rb_input[1209] : 0x8de979dd);
        rb_state15.var_49 = (rb_state15.var_49) + (((((0xc3fe0c4d) ^ (rb_state15.var_47)) ^ (0x34c36fd2)) - (0x81d6b7ed)) - (rb_state15.var_46));
        rb_state15.var_50 = (rb_state15.var_50) + (((rb_state15.var_28) == (0x0)) ? (rb_state15.var_27) : (0x13b1a9dc));
        rb_state15.var_51 = (rb_state15.var_51) + (((rb_state15.var_48) == (0xfffffff0)) ? (rb_state15.var_49) : (0xebe85809));
        rb_state15.var_52 = (rb_state15.var_52) ^ (((rb_state15.var_50) == (0x0)) ? (rb_state15.var_51) : (0xf547f627));
        rb_state15.var_45 = (rb_state15.var_45) ^ (rb_state15.var_52);
        rb_state15.var_18 = rb_state15.var_45;
        pthread_mutex_unlock(&(rb_state15.lock_53));
    }
    #endif
    data = (struct load_data *)malloc(sizeof(struct load_data));
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (1534 < rb_input_size ? (uint32_t)rb_input[1534] : 0x1200d9da);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xfc16284e)) {
        pthread_mutex_lock(&(rb_state11.lock_36));
        if ((rb_state11.var_2) != (0x0)) {
            if (!((rb_state11.var_2) == (rb_state11.var_27))) {
                racebench_trigger(11);
            }
        }
        pthread_mutex_unlock(&(rb_state11.lock_36));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - ((0xe79cd036) + (0xb6bbe253));
    #endif
    assert(data != NULL);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x9caf20f4));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (1978 < rb_input_size ? (uint32_t)rb_input[1978] : 0x790c3ea);
    #endif
    data->name = strdup(file);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_2);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_1) == (0x2d)) {
        rb_state5.var_18 = 0x3861beec;
        rb_state5.var_19 = 0x6edda115;
        rb_state5.var_20 = (rb_state5.var_20) + (rb_state5.var_18);
        rb_state5.var_21 = (rb_state5.var_21) + (((rb_state5.var_14) == (0xe3ad1de0)) ? (rb_state5.var_19) : (0x1c61114));
        rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_20) : (0x7c641e6e));
        rb_state5.var_23 = (rb_state5.var_23) ^ (((rb_state5.var_21) == (0x6edda115)) ? (rb_state5.var_22) : (0xb811d49a));
        rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_23);
    }
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

    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x701022dd)) ? (rb_state14.var_1) : (0x28f66d21));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x48ebff19)) {
        pthread_mutex_lock(&(rb_state15.lock_16));
        rb_state15.var_4 = 0xee8cecb7;
        rb_state15.var_5 = (rb_state15.var_5) + (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_2) : (0x2d8a332b));
        rb_state15.var_6 = (rb_state15.var_6) - (rb_state15.var_4);
        rb_state15.var_7 = (rb_state15.var_7) - (((0x37856558) + (0x23b8dca4)) ^ (rb_state15.var_5));
        rb_state15.var_8 = (rb_state15.var_8) ^ (((0xef8b2cbc) ^ (rb_state15.var_6)) + (0x55dbb525));
        rb_state15.var_9 = (rb_state15.var_9) ^ (((rb_state15.var_4) == (0xee8cecb7)) ? (rb_state15.var_7) : (0xb2d23f56));
        rb_state15.var_10 = (rb_state15.var_10) - (((0x709cb220) - (0xbf7bdccb)) + (rb_state15.var_8));
        rb_state15.var_11 = (rb_state15.var_11) ^ (rb_state15.var_9);
        rb_state15.var_12 = (rb_state15.var_12) ^ (((0xd2e45119) - (rb_state15.var_5)) ^ (rb_state15.var_10));
        rb_state15.var_13 = (rb_state15.var_13) - (((0xa6e02861) - (rb_state15.var_11)) + (rb_state15.var_6));
        rb_state15.var_14 = (rb_state15.var_14) ^ (((0x4adbec28) + (rb_state15.var_7)) ^ (rb_state15.var_12));
        rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_13) == (0xec6e825a)) ? (rb_state15.var_14) : (0xd438bcbc));
        rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_15);
        rb_state15.var_3 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_1) == (0xffffff9e)) ? (rb_state19.var_0) : (0x7ad3c2a9));
    if ((rb_state19.var_2) == (0xbe)) {
        pthread_mutex_lock(&(rb_state19.lock_15));
        rb_state19.var_3 = rb_state19.var_0;
        pthread_mutex_unlock(&(rb_state19.lock_15));
    }
    #endif
    strcat(head, dir);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (50 < rb_input_size ? (uint32_t)rb_input[50] : 0xb2d629b0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_1) == (0x438607ad)) ? (rb_state14.var_0) : (0xb01aebf6));
    if ((rb_state14.var_0) == (0xb3962a8a)) {
        rb_state14.var_8 = 0xfc723a4c;
        rb_state14.var_9 = 0x2424af56;
        rb_state14.var_10 = (rb_state14.var_10) - (((rb_state14.var_6) == (0xbfb97ad2)) ? (rb_state14.var_9) : (0x5872eb4b));
        rb_state14.var_11 = (rb_state14.var_11) + (((((0x51f161d1) ^ (rb_state14.var_8)) - (rb_state14.var_5)) - (0xa722fd7b)) ^ (rb_state14.var_7));
        rb_state14.var_12 = (rb_state14.var_12) - (((rb_state14.var_8) == (0xfc723a4c)) ? (rb_state14.var_10) : (0xe49407aa));
        rb_state14.var_13 = (rb_state14.var_13) - (((rb_state14.var_9) == (0x2424af56)) ? (rb_state14.var_11) : (0xdd8c5b2c));
        rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_12) == (0x2424af56)) ? (rb_state14.var_13) : (0x9b24251a));
        rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_8 = (rb_state15.var_8) + ((0x99f1390e) - (0xfd4a4877));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0xbe)) {
        if (!((rb_state19.var_0) == (rb_state19.var_3))) {
            racebench_trigger(19);
        }
    }
    #endif
    ret = stat(path, &st);
    if (ret != 0) {
        perror("Error:");
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_0) : (0x41466b6d));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_2) == (0x79217ee9)) ? (rb_state5.var_1) : (0x60ac215f));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_1) : (0xd5255918));
        rb_state7.var_2 = (rb_state7.var_2) ^ (0x550b3f05);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (1088 < rb_input_size ? (uint32_t)rb_input[1088] : 0x5fe8837a);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (2157 < rb_input_size ? (uint32_t)rb_input[2157] : 0xe82ae225);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) + (201 < rb_input_size ? (uint32_t)rb_input[201] : 0x411d6be9);
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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (1761 < rb_input_size ? (uint32_t)rb_input[1761] : 0xb44ab2e8);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ ((0x455bb85) + (0x65650e0d));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0x57e25a1e);
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_0);
    rb_state19.var_1 = (rb_state19.var_1) - (516 < rb_input_size ? (uint32_t)rb_input[516] : 0xfa190832);
    #endif
    const char *dir = (const char *)dummy;

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (0x944bcc21);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x4e68d145));
    rb_state15.var_0 = (rb_state15.var_0) ^ (0x48ebff76);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) ^ ((0xfcaa2a0c) + (rb_state17.var_1));
    #endif
    path[0] = 0;

    if (strcmp(dir, ".") == 0) {
        dir_helper(".", path);
    } else {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (2011 < rb_input_size ? (uint32_t)rb_input[2011] : 0x6863c615);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0x88972b4e) ^ (0xd76ebe20));
        rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0xffffff41)) ? (rb_state9.var_0) : (0x8337188e));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_1) : (0x13df31da));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (0xb18b230);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (329 < rb_input_size ? (uint32_t)rb_input[329] : 0xac5425da);
        rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_1) == (0xffffff9e)) ? (rb_state19.var_2) : (0x9adb5205));
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
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xb3962a8a)) {
        pthread_mutex_lock(&(rb_state14.lock_15));
        rb_state14.var_4 = 0x4046852e;
        rb_state14.var_5 = (rb_state14.var_5) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_3) : (0xee4df505));
        rb_state14.var_6 = (rb_state14.var_6) - (((rb_state14.var_4) == (0x4046852e)) ? (rb_state14.var_4) : (0x17539e6a));
        rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_6) : (0xa6383a6e));
        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_7);
        rb_state14.var_2 = rb_state14.var_3;
        pthread_mutex_unlock(&(rb_state14.lock_15));
    }
    if ((rb_state14.var_0) == (0xb3962a8a)) {
        if (!((rb_state14.var_2) == (rb_state14.var_3))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (((rb_state15.var_0) == (0x48ebff19)) ? (rb_state15.var_1) : (0xc46c93cf));
    #endif
    p_seg = tpool_create(t_seg_desc, NTHREAD_SEG);
    p_extract = tpool_create(t_extract_desc, NTHREAD_EXTRACT);
    p_vec = tpool_create(t_vec_desc, NTHREAD_VEC);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0x2a694811)) {
        rb_state0.var_19 = 0x16778e17;
        rb_state0.var_20 = 0x1ae3f046;
        rb_state0.var_21 = (rb_state0.var_21) ^ (((0xb1060c41) - (rb_state0.var_19)) - (rb_state0.var_16));
        rb_state0.var_22 = (rb_state0.var_22) + (((((0xfeba53c5) ^ (rb_state0.var_20)) ^ (0x81a619fb)) - (rb_state0.var_17)) + (rb_state0.var_15));
        rb_state0.var_23 = (rb_state0.var_23) + (rb_state0.var_21);
        rb_state0.var_24 = (rb_state0.var_24) - (((((0x78c41e65) + (rb_state0.var_18)) + (0x1f277b1f)) - (rb_state0.var_22)) ^ (rb_state0.var_23));
        rb_state0.var_3 = (rb_state0.var_3) ^ (rb_state0.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (158 < rb_input_size ? (uint32_t)rb_input[158] : 0x6998af3e);
    #endif
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