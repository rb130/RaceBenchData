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

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_4 = (rb_state2.var_4) ^ (159 < rb_input_size ? (uint32_t)rb_input[159] : 0x191945b0);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) ^ (0xe663f549);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_2) : (0xc21c5568));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) + (1083 < rb_input_size ? (uint32_t)rb_input[1083] : 0xe07490c5);
    rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_2);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_2) : (0x85f24ba1));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) - (0xe424816d);
    #endif
    data = (struct load_data *)malloc(sizeof(struct load_data));
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (0x89f54b9c);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_2);
    rb_state5.var_3 = (rb_state5.var_3) ^ (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_3) : (0xf3d8c54));
    rb_state5.var_3 = (rb_state5.var_3) + ((0xa072c183) - (rb_state5.var_2));
    rb_state5.var_3 = (rb_state5.var_3) ^ (0xb1f246ec);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + ((0x2395f235) + (0xa72fdfb3));
    if ((rb_state11.var_1) == (0xa814f211)) {
        if ((rb_state11.var_4) != (0x0)) {
            if (!((rb_state11.var_4) == (rb_state11.var_16))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_3 = (rb_state12.var_3) ^ ((0x4be869b7) - (rb_state12.var_1));
    rb_state12.var_2 = (rb_state12.var_2) + (((rb_state12.var_0) == (0xd893227d)) ? (rb_state12.var_3) : (0x9d158edf));
    rb_state12.var_1 = (rb_state12.var_1) ^ (((rb_state12.var_3) == (0x58f888e2)) ? (rb_state12.var_2) : (0xb344bfad));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - ((0xdbdf09ad) - (0x93bd77d6));
    if ((rb_state13.var_1) == (0xe869561b)) {
        rb_state13.var_5 = 0x70ccc45;
        rb_state13.var_6 = 0xd4fa8104;
        rb_state13.var_7 = 0xec38ef65;
        rb_state13.var_8 = (rb_state13.var_8) - (((0xae234e7c) - (rb_state13.var_7)) ^ (rb_state13.var_4));
        rb_state13.var_9 = (rb_state13.var_9) ^ (rb_state13.var_6);
        rb_state13.var_10 = (rb_state13.var_10) ^ (((0x315ad486) + (rb_state13.var_5)) ^ (rb_state13.var_5));
        rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_6) == (0xd4fa8104)) ? (rb_state13.var_8) : (0x932db40e));
        rb_state13.var_12 = (rb_state13.var_12) - (((rb_state13.var_9) == (0xd4fa8104)) ? (rb_state13.var_10) : (0xd0bc7c8a));
        rb_state13.var_13 = (rb_state13.var_13) + (((0xe829ed76) + (rb_state13.var_11)) - (rb_state13.var_7));
        rb_state13.var_14 = (rb_state13.var_14) + (((0xf2d3eb08) - (0x1591e2a8)) - (rb_state13.var_12));
        rb_state13.var_15 = (rb_state13.var_15) + (((rb_state13.var_8) == (0xd67cf7e4)) ? (rb_state13.var_13) : (0x6c06fb81));
        rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_9) == (0xd4fa8104)) ? (rb_state13.var_14) : (0x593b4b59));
        rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_15) == (0xd26df5f5)) ? (rb_state13.var_16) : (0x5e07cf13));
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (199 < rb_input_size ? (uint32_t)rb_input[199] : 0xf6ed2bed);
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_0) == (0xfffffffc)) ? (rb_state18.var_2) : (0xb6c197a5));
    #endif
    assert(data != NULL);

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0xdb17a2db)) ? (rb_state1.var_1) : (0xadfc1dc));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0x0)) {
        rb_state2.var_6 = 0x3d1552ef;
        rb_state2.var_7 = 0x5ed09f90;
        rb_state2.var_8 = (rb_state2.var_8) - (((rb_state2.var_5) == (0xcbf686c)) ? (rb_state2.var_7) : (0x74eb1138));
        rb_state2.var_9 = (rb_state2.var_9) + (rb_state2.var_6);
        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_8) == (0xa12f6070)) ? (rb_state2.var_9) : (0x9ec11d96));
        rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_10);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_2);
    rb_state9.var_3 = (rb_state9.var_3) - (0x1998a5bf);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
    rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_1) : (0x1eb86650));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_2);
    if ((rb_state18.var_0) == (0xfffffffc)) {
        pthread_mutex_lock(&(rb_state18.lock_7));
        rb_state18.var_3 = rb_state18.var_1;
        pthread_mutex_unlock(&(rb_state18.lock_7));
    }
    if ((rb_state18.var_0) == (0xfffffffc)) {
        if (!((rb_state18.var_1) == (rb_state18.var_3))) {
            racebench_trigger(18);
        }
    }
    #endif
    data->name = strdup(file);

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_3) == (0xe05af443)) {
        rb_state4.var_5 = rb_state4.var_4;
    }
    if ((rb_state4.var_3) == (0xe05af443)) {
        if (!((rb_state4.var_4) == (rb_state4.var_5))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_4 = (rb_state5.var_4) + (439 < rb_input_size ? (uint32_t)rb_input[439] : 0x95ad82ac);
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xd893227d)) {
        rb_state12.var_4 = rb_state12.var_1;
    }
    if ((rb_state12.var_0) == (0xd893227d)) {
        if (!((rb_state12.var_1) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + (0x6ba6238c);
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

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (1137 < rb_input_size ? (uint32_t)rb_input[1137] : 0xf6a645a8);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_3 = (rb_state4.var_3) ^ (0xdb180ccd);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_4 = (rb_state6.var_4) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_4) : (0x8f35ed75));
    rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_4);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (1857 < rb_input_size ? (uint32_t)rb_input[1857] : 0x52325929);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_3 = (rb_state11.var_3) ^ ((0x354a83b7) ^ (0x6ceb1162));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_2);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (140 < rb_input_size ? (uint32_t)rb_input[140] : 0xb94236f8);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_2);
    #endif
    strcat(head, dir);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - ((0x8695464d) ^ (0xa27d1b32));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - ((0x7734ff9a) + (rb_state2.var_0));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xe213c0bd));
    rb_state5.var_1 = (rb_state5.var_1) - (945 < rb_input_size ? (uint32_t)rb_input[945] : 0xc7ca1448);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x9f5fb3c7)) {
        rb_state6.var_7 = 0xf5608062;
        rb_state6.var_8 = (rb_state6.var_8) ^ (rb_state6.var_8);
        rb_state6.var_9 = (rb_state6.var_9) - (rb_state6.var_6);
        rb_state6.var_10 = (rb_state6.var_10) - ((0xb762273b) - (0x4ea14d2f));
        rb_state6.var_11 = (rb_state6.var_11) ^ (((rb_state6.var_7) == (0xf5608062)) ? (rb_state6.var_8) : (0xfb64826e));
        rb_state6.var_12 = (rb_state6.var_12) + (rb_state6.var_9);
        rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0x7561fa35));
        rb_state6.var_14 = (rb_state6.var_14) ^ (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_12) : (0xc51dc8b8));
        rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_10) == (0x973f25f4)) ? (rb_state6.var_13) : (0xb414df3e));
        rb_state6.var_16 = (rb_state6.var_16) ^ (((((0x1566a80b) ^ (rb_state6.var_14)) ^ (rb_state6.var_15)) + (rb_state6.var_11)) ^ (rb_state6.var_12));
        rb_state6.var_6 = (rb_state6.var_6) - (rb_state6.var_16);
        rb_state6.var_5 = rb_state6.var_6;
    }
    if ((rb_state6.var_0) == (0x9f5fb3c7)) {
        pthread_mutex_lock(&(rb_state6.lock_25));
        rb_state6.var_18 = 0x80d5de0;
        rb_state6.var_19 = 0x58966c49;
        rb_state6.var_20 = 0x7830852;
        rb_state6.var_21 = (rb_state6.var_21) - (((0xbb50e2bc) + (rb_state6.var_13)) ^ (rb_state6.var_19));
        rb_state6.var_22 = (rb_state6.var_22) - (((((0xc8a0699b) ^ (0xdcb07e7c)) ^ (rb_state6.var_18)) - (0xa258efd7)) ^ (rb_state6.var_20));
        rb_state6.var_23 = (rb_state6.var_23) ^ (((0x45a0d504) ^ (rb_state6.var_21)) ^ (0x503c098c));
        rb_state6.var_24 = (rb_state6.var_24) + (((((0x45400753) - (rb_state6.var_15)) + (rb_state6.var_14)) ^ (rb_state6.var_23)) - (rb_state6.var_22));
        rb_state6.var_17 = (rb_state6.var_17) - (rb_state6.var_24);
        rb_state6.var_5 = rb_state6.var_17;
        pthread_mutex_unlock(&(rb_state6.lock_25));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0xfffffffe)) ? (rb_state9.var_0) : (0xf7eb8fb2));
    rb_state9.var_1 = (rb_state9.var_1) - (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_1) : (0x78e59708));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (1238 < rb_input_size ? (uint32_t)rb_input[1238] : 0xa19619f9);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) ^ (122 < rb_input_size ? (uint32_t)rb_input[122] : 0x99c01ef5);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (1115 < rb_input_size ? (uint32_t)rb_input[1115] : 0xa50c0bb7);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (((rb_state14.var_1) == (0x3)) ? (rb_state14.var_0) : (0x7a60e289));
    rb_state14.var_2 = (rb_state14.var_2) + ((0x3726f644) - (0x54697ba2));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ (1186 < rb_input_size ? (uint32_t)rb_input[1186] : 0x6347fe29);
    #endif
    ret = stat(path, &st);
    if (ret != 0) {
        perror("Error:");
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_2);
        rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_3);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_2) == (0xffffff54)) ? (rb_state4.var_1) : (0x8bbd9e8a));
        rb_state4.var_4 = (rb_state4.var_4) - (((rb_state4.var_3) == (0xe05af443)) ? (rb_state4.var_4) : (0xe0b90a50));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (1134 < rb_input_size ? (uint32_t)rb_input[1134] : 0xa8ee7d1f);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) + (0xda1c9a12);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_3);
        rb_state13.var_0 = (rb_state13.var_0) - (1601 < rb_input_size ? (uint32_t)rb_input[1601] : 0x30a1b597);
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x3)) {
            pthread_mutex_lock(&(rb_state14.lock_26));
            rb_state14.var_17 = 0x9f84abdc;
            rb_state14.var_18 = (rb_state14.var_18) ^ (((((0x5c635d07) + (rb_state14.var_12)) - (rb_state14.var_14)) ^ (rb_state14.var_11)) + (rb_state14.var_13));
            rb_state14.var_19 = (rb_state14.var_19) ^ (((((0x678dcab6) ^ (rb_state14.var_18)) - (rb_state14.var_15)) + (0xc11114dd)) - (rb_state14.var_17));
            rb_state14.var_16 = (rb_state14.var_16) ^ (rb_state14.var_19);
            rb_state14.var_4 = rb_state14.var_16;
            pthread_mutex_unlock(&(rb_state14.lock_26));
        }
        if ((rb_state14.var_1) == (0x3)) {
            pthread_mutex_lock(&(rb_state14.lock_26));
            rb_state14.var_21 = (rb_state14.var_21) ^ (((rb_state14.var_19) == (0x5d7b00b2)) ? (rb_state14.var_16) : (0xafa6d781));
            rb_state14.var_22 = (rb_state14.var_22) - ((0xe92411c6) ^ (rb_state14.var_20));
            rb_state14.var_23 = (rb_state14.var_23) - (((rb_state14.var_18) == (0x5c635d07)) ? (rb_state14.var_17) : (0xbc0bac4f));
            rb_state14.var_24 = (rb_state14.var_24) - (((rb_state14.var_21) == (0x5d7b00b2)) ? (rb_state14.var_22) : (0xc98d5f53));
            rb_state14.var_25 = (rb_state14.var_25) ^ (((((0x26cb1cd) - (rb_state14.var_23)) - (rb_state14.var_24)) + (0x72f83781)) + (0x7a707319));
            rb_state14.var_20 = (rb_state14.var_20) ^ (rb_state14.var_25);
            rb_state14.var_4 = rb_state14.var_20;
            pthread_mutex_unlock(&(rb_state14.lock_26));
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
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (441 < rb_input_size ? (uint32_t)rb_input[441] : 0x9b39b2ef);
    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_1) == (0xffffffd3)) ? (rb_state4.var_0) : (0x5c6d3fb8));
    rb_state4.var_3 = (rb_state4.var_3) - (0xc4bd0772);
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0x2337e0eb));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x1db70075));
    #endif
    const char *dir = (const char *)dummy;

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (0x6df347bc);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_2 = (rb_state6.var_2) ^ (1520 < rb_input_size ? (uint32_t)rb_input[1520] : 0xc4e94b1e);
    rb_state6.var_2 = (rb_state6.var_2) + (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_2) : (0xd82c8947));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) ^ (59 < rb_input_size ? (uint32_t)rb_input[59] : 0x7e4dad43);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_1) : (0x7d9f63c2));
    #endif
    path[0] = 0;

    if (strcmp(dir, ".") == 0) {
        dir_helper(".", path);
    } else {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (1735 < rb_input_size ? (uint32_t)rb_input[1735] : 0x7d4b7909);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_3 = (rb_state6.var_3) ^ ((0x999718b3) - (0xaada7301));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) + (1491 < rb_input_size ? (uint32_t)rb_input[1491] : 0xe87a5370);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_2) == (0xe869570b)) ? (rb_state13.var_1) : (0x506e699a));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (0x28832799);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - (908 < rb_input_size ? (uint32_t)rb_input[908] : 0x5ca4c898);
        rb_state18.var_2 = (rb_state18.var_2) + (469 < rb_input_size ? (uint32_t)rb_input[469] : 0x471d6d64);
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_0) == (0x10bf183)) ? (rb_state0.var_1) : (0x13e36e2a));
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (742 < rb_input_size ? (uint32_t)rb_input[742] : 0x561a8b72);
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x9f5fb3c7)) {
        if ((rb_state6.var_5) != (0x0)) {
            if (!((rb_state6.var_5) == (rb_state6.var_17))) {
                racebench_trigger(6);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (763 < rb_input_size ? (uint32_t)rb_input[763] : 0xeb1dd6c4);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ ((0x718d8d96) - (0xffe89c59));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x5bf0390d));
    rb_state12.var_0 = (rb_state12.var_0) ^ (0xd893227d);
    rb_state12.var_1 = (rb_state12.var_1) - ((0xd101e5c) ^ (rb_state12.var_1));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) - (0x1796a911);
    #endif
    p_seg = tpool_create(t_seg_desc, NTHREAD_SEG);
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_3) == (0x0)) {
        rb_state10.var_4 = rb_state10.var_2;
    }
    if ((rb_state10.var_3) == (0x0)) {
        if (!((rb_state10.var_2) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    p_extract = tpool_create(t_extract_desc, NTHREAD_EXTRACT);
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_2);
    #endif
    p_vec = tpool_create(t_vec_desc, NTHREAD_VEC);
    p_rank = tpool_create(t_rank_desc, NTHREAD_RANK);
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_11 = (rb_state8.var_11) - (698 < rb_input_size ? (uint32_t)rb_input[698] : 0x2f2ba047);
    #endif
    p_out = tpool_create(t_out_desc, NTHREAD_OUT);

    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_3) == (0x4368dbc8)) {
        pthread_mutex_lock(&(rb_state16.lock_21));
        rb_state16.var_15 = 0x40769184;
        rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_14) == (0x73732a2a)) ? (rb_state16.var_15) : (0xd76eba53));
        rb_state16.var_17 = (rb_state16.var_17) + (((((0x4c423446) - (0xa571d5b9)) + (rb_state16.var_12)) + (0xa3d0debe)) - (rb_state16.var_13));
        rb_state16.var_18 = (rb_state16.var_18) - (((rb_state16.var_15) == (0x40769184)) ? (rb_state16.var_16) : (0x76ff8085));
        rb_state16.var_19 = (rb_state16.var_19) - (((rb_state16.var_16) == (0x40769184)) ? (rb_state16.var_17) : (0xb0a1da65));
        rb_state16.var_20 = (rb_state16.var_20) ^ (((((0x4a3f53d9) + (rb_state16.var_17)) - (rb_state16.var_19)) + (0xc6e38ada)) - (rb_state16.var_18));
        rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_20);
        pthread_mutex_unlock(&(rb_state16.lock_21));
    }
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