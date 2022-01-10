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

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_3) == (0xd2)) {
        pthread_mutex_lock(&(rb_state0.lock_9));
        rb_state0.var_6 = 0x39a1a2a2;
        rb_state0.var_7 = (rb_state0.var_7) ^ (((((0xeb36b326) + (rb_state0.var_5)) + (rb_state0.var_4)) - (0xce722e39)) + (rb_state0.var_6));
        rb_state0.var_8 = (rb_state0.var_8) + (((((0x8400f44f) ^ (rb_state0.var_7)) ^ (rb_state0.var_8)) + (rb_state0.var_7)) + (rb_state0.var_6));
        rb_state0.var_4 = (rb_state0.var_4) ^ (rb_state0.var_8);
        rb_state0.var_5 = rb_state0.var_4;
        pthread_mutex_unlock(&(rb_state0.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - ((0xcd2d62de) + (0xfb9f0930));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (112 < rb_input_size ? (uint32_t)rb_input[112] : 0x6f8b7b1a);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_2) : (0x84f2cd8b));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (0xc0360c1b);
    rb_state17.var_2 = (rb_state17.var_2) - ((0x10df8b32) + (rb_state17.var_0));
    #endif
    data = (struct load_data *)malloc(sizeof(struct load_data));
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (124 < rb_input_size ? (uint32_t)rb_input[124] : 0x815b9680);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) ^ ((0x50567868) + (rb_state9.var_2));
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_17 = 0xa8c64a7c;
        rb_state16.var_18 = 0x1b6bfdc3;
        rb_state16.var_19 = (rb_state16.var_19) ^ (((((0x264de909) - (rb_state16.var_18)) ^ (0xc8692063)) + (rb_state16.var_9)) + (0x7725f7c2));
        rb_state16.var_20 = (rb_state16.var_20) - (((rb_state16.var_17) == (0xa8c64a7c)) ? (rb_state16.var_19) : (0xdd93618));
        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_20);
    }
    #endif
    assert(data != NULL);

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) - (0x5f6c0ab);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) + (((rb_state5.var_3) == (0x10671891)) ? (rb_state5.var_2) : (0xf3bf5f6a));
    #endif
    data->name = strdup(file);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_8 = (rb_state0.var_8) - ((0xeea73c31) ^ (rb_state0.var_10));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xbe2f8426)) {
        rb_state5.var_4 = rb_state5.var_1;
    }
    if ((rb_state5.var_0) == (0xbe2f8426)) {
        if (!((rb_state5.var_1) == (rb_state5.var_4))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) ^ (2142 < rb_input_size ? (uint32_t)rb_input[2142] : 0xe8156b75);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x9242d421)) {
        if ((rb_state17.var_3) != (0x0)) {
            if (!((rb_state17.var_3) == (rb_state17.var_29))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    r = image_read_rgb_hsv(file, &data->width, &data->height, &data->RGB, &data->HSV);
    assert(r == 0);

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x9fada121)) {
        pthread_mutex_lock(&(rb_state12.lock_17));
        rb_state12.var_4 = rb_state12.var_1;
        pthread_mutex_unlock(&(rb_state12.lock_17));
    }
    #endif
    cnt_enqueue++;
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_3) == (0x9fada121)) {
        if (!((rb_state12.var_1) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
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

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (0xaa0d75db);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) - (0x8555656b);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_1);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) + (((rb_state6.var_1) == (0x8a)) ? (rb_state6.var_0) : (0xc716c78f));
    if ((rb_state6.var_2) == (0xfffffff1)) {
        rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_4);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0xcf054879) ^ (0xf51bce0));
    rb_state7.var_2 = (rb_state7.var_2) + ((0x12196720) - (0x7372d6e2));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_0) == (0x8692a704)) ? (rb_state13.var_1) : (0xec7bced8));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) ^ (0xf922bc50);
    if ((rb_state15.var_0) == (0x85f2a0bc)) {
        rb_state15.var_28 = 0x8c41d3ec;
        rb_state15.var_29 = (rb_state15.var_29) ^ (0x6c522205);
        rb_state15.var_30 = (rb_state15.var_30) ^ (1223 < rb_input_size ? (uint32_t)rb_input[1223] : 0xc31ed6ea);
        rb_state15.var_31 = (rb_state15.var_31) - (rb_state15.var_17);
        rb_state15.var_32 = (rb_state15.var_32) - (((rb_state15.var_16) == (0xbd46e736)) ? (rb_state15.var_28) : (0x6b0a18f8));
        rb_state15.var_33 = (rb_state15.var_33) - (((0x5750b57b) + (rb_state15.var_18)) + (rb_state15.var_29));
        rb_state15.var_34 = (rb_state15.var_34) + (((rb_state15.var_30) == (0x73)) ? (rb_state15.var_31) : (0x974b22ba));
        rb_state15.var_35 = (rb_state15.var_35) - (((rb_state15.var_32) == (0x73be2c14)) ? (rb_state15.var_33) : (0x56de8997));
        rb_state15.var_36 = (rb_state15.var_36) - (((((0xe9c73702) + (rb_state15.var_35)) + (rb_state15.var_34)) ^ (rb_state15.var_19)) + (0xc6c9090d));
        rb_state15.var_4 = (rb_state15.var_4) - (rb_state15.var_36);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (0x8753ec16);
    rb_state16.var_3 = (rb_state16.var_3) - (((rb_state16.var_2) == (0x4c1a8b01)) ? (rb_state16.var_3) : (0x82229603));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ ((0xa0f32093) - (0x728c8daf));
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_3) == (0xffffff7a)) {
        pthread_mutex_lock(&(rb_state18.lock_15));
        rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_5) == (0x26e1ef57)) ? (rb_state18.var_7) : (0x91eb088e));
        rb_state18.var_13 = (rb_state18.var_13) ^ (((rb_state18.var_8) == (0x8801c02)) ? (rb_state18.var_6) : (0x70d8875e));
        rb_state18.var_14 = (rb_state18.var_14) ^ (((rb_state18.var_12) == (0x4a7d20a2)) ? (rb_state18.var_13) : (0x1c7d951));
        rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_14);
        pthread_mutex_unlock(&(rb_state18.lock_15));
    }
    #endif
    strcat(head, dir);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_1);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - (1304 < rb_input_size ? (uint32_t)rb_input[1304] : 0x28b07f9d);
    if ((rb_state1.var_3) == (0xfffffffb)) {
        rb_state1.var_8 = 0x318d6b05;
        rb_state1.var_9 = 0x3a476457;
        rb_state1.var_10 = (rb_state1.var_10) - (0xc221d94c);
        rb_state1.var_11 = (rb_state1.var_11) + (((0x78b00055) ^ (rb_state1.var_8)) ^ (rb_state1.var_8));
        rb_state1.var_12 = (rb_state1.var_12) ^ (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) - (((((0xf9191e42) + (rb_state1.var_10)) - (0xf0717b5e)) - (rb_state1.var_7)) + (0x2787575e));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_11) == (0x78b00055)) ? (rb_state1.var_12) : (0x841042ea));
        rb_state1.var_15 = (rb_state1.var_15) - (rb_state1.var_13);
        rb_state1.var_16 = (rb_state1.var_16) ^ (((0xa652d393) - (rb_state1.var_9)) ^ (rb_state1.var_14));
        rb_state1.var_17 = (rb_state1.var_17) - (((rb_state1.var_10) == (0x3dde26b4)) ? (rb_state1.var_15) : (0x51f4b718));
        rb_state1.var_18 = (rb_state1.var_18) - (rb_state1.var_16);
        rb_state1.var_19 = (rb_state1.var_19) ^ (((rb_state1.var_11) == (0x78b00055)) ? (rb_state1.var_17) : (0xd16454a3));
        rb_state1.var_20 = (rb_state1.var_20) - (((((0x793c19bd) + (rb_state1.var_19)) + (0xbddaf4b3)) ^ (0x350c2c09)) ^ (rb_state1.var_18));
        rb_state1.var_7 = (rb_state1.var_7) + (rb_state1.var_20);
        rb_state1.var_4 = rb_state1.var_7;
    }
    if ((rb_state1.var_3) == (0xfffffffb)) {
        pthread_mutex_lock(&(rb_state1.lock_24));
        rb_state1.var_22 = (rb_state1.var_22) ^ (((rb_state1.var_14) == (0x3a476457)) ? (rb_state1.var_12) : (0x642bd6f0));
        rb_state1.var_23 = (rb_state1.var_23) + (((rb_state1.var_13) == (0x91f2df0a)) ? (rb_state1.var_22) : (0x66279a39));
        rb_state1.var_21 = (rb_state1.var_21) + (rb_state1.var_23);
        rb_state1.var_4 = rb_state1.var_21;
        pthread_mutex_unlock(&(rb_state1.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (1721 < rb_input_size ? (uint32_t)rb_input[1721] : 0xe4329a9e);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (0xf48c583b);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0xc9480007)) ? (rb_state17.var_0) : (0xecd6f02e));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xec219f3));
    #endif
    ret = stat(path, &st);
    if (ret != 0) {
        perror("Error:");
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) + (0x7db9a6c);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (0x57ebe9bc);
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
    rb_state1.var_0 = (rb_state1.var_0) + (1144 < rb_input_size ? (uint32_t)rb_input[1144] : 0x49a3a845);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0xdd4b25c3));
    rb_state6.var_1 = (rb_state6.var_1) + (1795 < rb_input_size ? (uint32_t)rb_input[1795] : 0x9262502a);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (rb_state7.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (0x85f2a0bc);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_1);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (0x4a2620a3);
    rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0x32c89138));
    #endif
    const char *dir = (const char *)dummy;

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (0x64e8bee4);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (0x30d06685);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (0xb49f31e6);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ (1629 < rb_input_size ? (uint32_t)rb_input[1629] : 0xcc60caa1);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0x601e1b0f));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
    #endif
    path[0] = 0;

    if (strcmp(dir, ".") == 0) {
        dir_helper(".", path);
    } else {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) + ((0x60283940) + (rb_state0.var_2));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (0xb1a693e8);
        rb_state1.var_2 = (rb_state1.var_2) - (1210 < rb_input_size ? (uint32_t)rb_input[1210] : 0x6556aa0);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + ((0xf5c7ff5a) + (0x211b8111));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (797 < rb_input_size ? (uint32_t)rb_input[797] : 0x9b5ead35);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_2) == (0xfffffff1)) ? (rb_state6.var_1) : (0xd0991673));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0x842a1516);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (462 < rb_input_size ? (uint32_t)rb_input[462] : 0xa1c4f96b);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) ^ ((0x6c9b3cf1) + (0x4758d672));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (((rb_state18.var_1) == (0x67190ee4)) ? (rb_state18.var_0) : (0x5d8a0e00));
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) ^ (1782 < rb_input_size ? (uint32_t)rb_input[1782] : 0x1ff67fff);
    rb_state0.var_3 = (rb_state0.var_3) ^ (1684 < rb_input_size ? (uint32_t)rb_input[1684] : 0x95109407);
    if ((rb_state0.var_3) == (0xd2)) {
        rb_state0.var_10 = !((rb_state0.var_5) == (rb_state0.var_4));
    }
    rb_state0.var_7 = (rb_state0.var_7) ^ ((0x6dcb75e0) - (0x237a13fe));
    rb_state0.var_7 = (rb_state0.var_7) ^ ((0xbd26f3a3) + (rb_state0.var_3));
    if ((rb_state0.var_4) == (0x626e9df1)) {
        if ((rb_state0.var_11) != (0x0)) {
            if (!((rb_state0.var_11) == (rb_state0.var_42))) {
                racebench_trigger(0);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) ^ (1589 < rb_input_size ? (uint32_t)rb_input[1589] : 0x9ea5333);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0xfffffff1)) {
        rb_state6.var_4 = rb_state6.var_3;
    }
    if ((rb_state6.var_2) == (0xfffffff1)) {
        if (!((rb_state6.var_3) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + ((0xd524ad6b) ^ (0x9c134a6e));
    rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x85f2a0bc)) {
        pthread_mutex_lock(&(rb_state15.lock_37));
        rb_state15.var_6 = 0x367f5df8;
        rb_state15.var_7 = (rb_state15.var_7) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_6) : (0x5cbdcbd7));
        rb_state15.var_8 = (rb_state15.var_8) - (((0x1eb6837b) + (rb_state15.var_6)) ^ (rb_state15.var_5));
        rb_state15.var_9 = (rb_state15.var_9) + (746 < rb_input_size ? (uint32_t)rb_input[746] : 0x9053a782);
        rb_state15.var_10 = (rb_state15.var_10) ^ (((0x5a2242f7) + (rb_state15.var_7)) + (0xc852bdd7));
        rb_state15.var_11 = (rb_state15.var_11) + (((0xe185c5cc) - (rb_state15.var_8)) + (rb_state15.var_7));
        rb_state15.var_12 = (rb_state15.var_12) ^ (((rb_state15.var_8) == (0xaaca1e8d)) ? (rb_state15.var_9) : (0x75e11db8));
        rb_state15.var_13 = (rb_state15.var_13) - (((0x86f870de) + (rb_state15.var_10)) - (rb_state15.var_9));
        rb_state15.var_14 = (rb_state15.var_14) + (rb_state15.var_11);
        rb_state15.var_15 = (rb_state15.var_15) + (((((0x665487d3) ^ (rb_state15.var_12)) + (rb_state15.var_10)) + (0xeb90cd2e)) ^ (rb_state15.var_13));
        rb_state15.var_16 = (rb_state15.var_16) ^ (((0x56c2be54) + (rb_state15.var_14)) - (0x6b6dc55));
        rb_state15.var_17 = (rb_state15.var_17) ^ (((0x8a6596f8) + (rb_state15.var_15)) ^ (rb_state15.var_11));
        rb_state15.var_18 = (rb_state15.var_18) ^ (((0x892dd021) ^ (rb_state15.var_16)) + (rb_state15.var_12));
        rb_state15.var_19 = (rb_state15.var_19) - (((0x18340c55) + (rb_state15.var_17)) ^ (0x8d6c251c));
        rb_state15.var_20 = (rb_state15.var_20) - (rb_state15.var_18);
        rb_state15.var_21 = (rb_state15.var_21) + (((rb_state15.var_13) == (0x20133078)) ? (rb_state15.var_19) : (0xfb5a846d));
        rb_state15.var_22 = (rb_state15.var_22) - (((0xa84e65da) - (0xde747967)) - (rb_state15.var_20));
        rb_state15.var_23 = (rb_state15.var_23) + (((rb_state15.var_14) == (0x6d3b0537)) ? (rb_state15.var_21) : (0x22ba3fdf));
        rb_state15.var_24 = (rb_state15.var_24) ^ (rb_state15.var_22);
        rb_state15.var_25 = (rb_state15.var_25) + (rb_state15.var_23);
        rb_state15.var_26 = (rb_state15.var_26) + (((0xbe26194f) + (0x6b1c0c73)) ^ (rb_state15.var_24));
        rb_state15.var_27 = (rb_state15.var_27) + (((((0x9465ca5c) + (rb_state15.var_15)) - (rb_state15.var_25)) - (0xc9356789)) + (rb_state15.var_26));
        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_27);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_37));
    }
    if ((rb_state15.var_0) == (0x85f2a0bc)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (0xc4c69eeb);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + ((0xb409c928) ^ (0xa8fb2769));
    rb_state18.var_3 = (rb_state18.var_3) - (647 < rb_input_size ? (uint32_t)rb_input[647] : 0x34284ee2);
    if ((rb_state18.var_3) == (0xffffff7a)) {
        pthread_mutex_lock(&(rb_state18.lock_15));
        rb_state18.var_6 = 0xb4cfa543;
        rb_state18.var_7 = 0x4a7d20a2;
        rb_state18.var_8 = 0x8801c02;
        rb_state18.var_9 = (rb_state18.var_9) - (((((0xaa99c47) + (0xc4a254da)) ^ (rb_state18.var_8)) ^ (rb_state18.var_4)) ^ (rb_state18.var_7));
        rb_state18.var_10 = (rb_state18.var_10) ^ (((0x4ed3e6a1) - (0xbca59c8d)) ^ (rb_state18.var_6));
        rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_9) == (0x7249327f)) ? (rb_state18.var_10) : (0xdf5f673a));
        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_11);
        rb_state18.var_4 = rb_state18.var_5;
        pthread_mutex_unlock(&(rb_state18.lock_15));
    }
    if ((rb_state18.var_3) == (0xffffff7a)) {
        pthread_mutex_lock(&(rb_state18.lock_15));
        if (!((rb_state18.var_4) == (rb_state18.var_5))) {
            racebench_trigger(18);
        }
        pthread_mutex_unlock(&(rb_state18.lock_15));
    }
    #endif
    p_seg = tpool_create(t_seg_desc, NTHREAD_SEG);
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0xf4127eaa)) {
        rb_state2.var_13 = (rb_state2.var_13) + ((0xf3cbaabe) + (rb_state2.var_11));
        rb_state2.var_14 = (rb_state2.var_14) ^ (0x30cbee73);
        rb_state2.var_15 = (rb_state2.var_15) + (((rb_state2.var_10) == (0xb5ba19a0)) ? (rb_state2.var_8) : (0x57e2b4cc));
        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_9);
        rb_state2.var_17 = (rb_state2.var_17) + (((((0xb8e86814) ^ (rb_state2.var_13)) - (rb_state2.var_14)) - (rb_state2.var_12)) - (0xe71c8e9b));
        rb_state2.var_18 = (rb_state2.var_18) ^ (((((0x4243c7da) - (rb_state2.var_14)) + (rb_state2.var_13)) - (rb_state2.var_15)) ^ (rb_state2.var_16));
        rb_state2.var_19 = (rb_state2.var_19) + (((((0xd8c087cd) + (rb_state2.var_18)) - (0xf39e431e)) ^ (rb_state2.var_17)) - (0xf8261fbb));
        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + ((0x89f575cd) ^ (rb_state3.var_2));
    #endif
    p_extract = tpool_create(t_extract_desc, NTHREAD_EXTRACT);
    p_vec = tpool_create(t_vec_desc, NTHREAD_VEC);
    p_rank = tpool_create(t_rank_desc, NTHREAD_RANK);
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_3) == (0xa028b032)) {
        rb_state8.var_5 = 0xa619c8fa;
        rb_state8.var_6 = 0x46781365;
        rb_state8.var_7 = (rb_state8.var_7) + (((((0x37147d44) + (rb_state8.var_4)) + (rb_state8.var_6)) + (rb_state8.var_5)) + (rb_state8.var_5));
        rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_7) == (0xc82fc01c)) ? (rb_state8.var_6) : (0xd173c5ef));
        rb_state8.var_9 = (rb_state8.var_9) - (((0x88846783) + (rb_state8.var_7)) + (0x4d600fd8));
        rb_state8.var_10 = (rb_state8.var_10) ^ (((((0x5bb356d5) - (rb_state8.var_9)) ^ (rb_state8.var_8)) - (0x674b0e92)) + (0x1d6a976c));
        rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_10);
    }
    #endif
    p_out = tpool_create(t_out_desc, NTHREAD_OUT);

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) - ((0x5dba85d5) ^ (0x3de8db0a));
    #endif
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