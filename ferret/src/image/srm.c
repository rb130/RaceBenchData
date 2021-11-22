

#include <cass.h>

#define ERROR_RET(err_msg, err_no)                                  \
    do {                                                            \
        fprintf(stderr, "Error in %s: %s\n", func_name, (err_msg)); \
        fflush(stderr);                                             \
                                                                    \
        return (err_no);                                            \
    } while (0)

#define SET_FUNC_NAME(f_name) static const char func_name[] = f_name

#define IS_NULL(x) ((x) == NULL)

#define IS_BYTE(x) ((0 <= (x)) && ((x) < 256))

#ifndef DBL_EPSILON
#define DBL_EPSILON 1e-9L
#endif

#define IS_ZERO(x) (fabs((x)) < DBL_EPSILON)

#define IS_POS(x) ((x) > DBL_EPSILON)

#define IS_NEG(x) ((x) < -DBL_EPSILON)

#define IS_ODD(x) ((x) % 2)
#define IS_EVEN(x) (!IS_ODD((x)))
#define IS_POS_ODD(x) (((x) > 0) && IS_ODD(x))

#define IS_IN_0_1(x) (!(IS_NEG((x)) || IS_POS((x)-1.0)))

#define NUM_GRAY 256

typedef struct {
    int reg1, reg2, delta;
} RegionPair;

static int find_set(const int *parent, int i);
static int union_set(const int i, const int j, int *parent, int *rank);
static RegionPair *bucket_sort(const RegionPair *pair, const int num_elems);

static int find_set(const int *parent, int i) {
    while (parent[i] != i) {
        i = parent[i];
    }

    return i;
}

static int union_set(const int i, const int j, int *parent, int *rank) {
    if (rank[i] > rank[j]) {
        parent[j] = i;
        return i;
    }

    parent[i] = j;
    if (rank[i] == rank[j]) {
        rank[j]++;
    }

    return j;
}

static RegionPair *bucket_sort(const RegionPair *pair, const int num_elems) {
    int ih;
    int *histo;
    int *cum_histo;
    RegionPair *sorted;

    sorted = (RegionPair *)calloc(num_elems, sizeof(RegionPair));
    if (IS_NULL(sorted)) {
        return NULL;
    }

    histo = (int *)calloc(NUM_GRAY, sizeof(int));
    cum_histo = (int *)malloc(NUM_GRAY * sizeof(int));

    for (ih = 0; ih < num_elems; ih++) {
        histo[pair[ih].delta]++;
    }

    cum_histo[0] = 0;
    for (ih = 1; ih < NUM_GRAY; ih++) {
        cum_histo[ih] = cum_histo[ih - 1] + histo[ih - 1];
    }

    for (ih = 0; ih < num_elems; ih++) {
        sorted[cum_histo[pair[ih].delta]++] = pair[ih];
    }

    free(histo);
    free(cum_histo);

    return sorted;
}

#define MIN_2(x, y) (((x) > (y)) ? (y) : (x))

#define MAX_2(x, y) (((x) > (y)) ? (x) : (y))

#define MAX_3(x, y, z) MAX_2((x), MAX_2((y), (z)))

#ifndef byte
typedef uchar byte;
#endif

typedef uchar pixel_t[3];

extern double Q_value;
extern double size_factor;

int image_segment(void **output, int *num_ccs, uchar *in_data_1d, int num_cols, int num_rows) {
    SET_FUNC_NAME("srm");
    byte *out_data;
    pixel_t **in_data_3d;
    int red, green, blue;
    int ir, ic, ik;
    int num_pixels;
    int num_pixels_t3;
    int num_rows_m1, num_cols_m1;
    int cnt, idx;
    int num_edges;
    int reg1, reg2;
    int min_reg_size;
    int root, total_size;
    int *size;
    int *parent;
    int *rank;
    double log_delta;
    double threshold;
    double thresh_factor;
    double *red_mean;
    double *green_mean;
    double *blue_mean;
    RegionPair *pair;
    RegionPair *sorted;

    int num_region;

    if (IS_NEG(Q_value)) {
        warn("Q value ( %f ) must be positive !", Q_value);
        return -1;
    }

    if (!IS_IN_0_1(size_factor)) {
        warn("Size factor ( %f ) must be in [0,1] range !", size_factor);
        return -1;
    }

    num_pixels = num_rows * num_cols;
    num_pixels_t3 = 3 * num_pixels;

    num_rows_m1 = num_rows - 1;
    num_cols_m1 = num_cols - 1;

    in_data_3d = type_array2matrix(pixel_t, num_rows, num_cols, (pixel_t *)in_data_1d);

    log_delta = 2.0 * log(6.0 * num_pixels);
    thresh_factor = (NUM_GRAY * NUM_GRAY) / (2.0 * Q_value);
    min_reg_size = size_factor * num_pixels;

    red_mean = (double *)malloc(num_pixels * sizeof(double));
    green_mean = (double *)malloc(num_pixels * sizeof(double));
    blue_mean = (double *)malloc(num_pixels * sizeof(double));
    size = (int *)malloc(num_pixels * sizeof(int));
    parent = (int *)malloc(num_pixels * sizeof(int));
    rank = (int *)calloc(num_pixels, sizeof(int));

    if (IS_NULL(red_mean) || IS_NULL(green_mean) || IS_NULL(blue_mean) || IS_NULL(size) || IS_NULL(parent) || IS_NULL(rank)) {
        ERROR_RET("Insufficient memory !", -1);
    }

    cnt = 0;
    for (ik = 0; ik < num_pixels_t3; ik += 3) {
        red_mean[cnt] = in_data_1d[ik];
        green_mean[cnt] = in_data_1d[ik + 1];
        blue_mean[cnt] = in_data_1d[ik + 2];
        size[cnt] = 1;
        parent[cnt] = cnt;
        cnt++;
    }

    idx = 0;
    num_edges = 2 * num_cols_m1 * num_rows_m1 + num_rows_m1 + num_cols_m1;
    pair = (RegionPair *)calloc(num_edges, sizeof(RegionPair));
    if (IS_NULL(pair)) {
        ERROR_RET("Insufficient memory !", -1);
    }

    cnt = 0;
    for (ir = 0; ir < num_rows_m1; ir++) {
        for (ic = 0; ic < num_cols_m1; ic++) {
            red = in_data_3d[ir][ic][0];
            green = in_data_3d[ir][ic][1];
            blue = in_data_3d[ir][ic][2];

            pair[idx].reg1 = cnt;
            pair[idx].reg2 = cnt + 1;
            pair[idx].delta = MAX_3(abs(in_data_3d[ir][ic + 1][0] - red), abs(in_data_3d[ir][ic + 1][1] - green), abs(in_data_3d[ir][ic + 1][2] - blue));

            idx++;
            pair[idx].reg1 = cnt;
            pair[idx].reg2 = cnt + num_cols;
            pair[idx].delta = MAX_3(abs(in_data_3d[ir + 1][ic][0] - red), abs(in_data_3d[ir + 1][ic][1] - green), abs(in_data_3d[ir + 1][ic][2] - blue));

            idx++;
            cnt++;
        }
        cnt++;
    }

    cnt = num_cols_m1;
    for (ir = 0; ir < num_rows_m1; ir++) {
        pair[idx].reg1 = cnt;
        cnt += num_cols;
        pair[idx].reg2 = cnt;
        pair[idx].delta = MAX_3(abs(in_data_3d[ir + 1][num_cols_m1][0] - in_data_3d[ir][num_cols_m1][0]), abs(in_data_3d[ir + 1][num_cols_m1][1] - in_data_3d[ir][num_cols_m1][1]), abs(in_data_3d[ir + 1][num_cols_m1][2] - in_data_3d[ir][num_cols_m1][2]));
        idx++;
    }

    cnt = num_rows_m1 * num_cols;
    for (ic = 0; ic < num_cols_m1; ic++) {
        pair[idx].reg1 = cnt;
        cnt++;
        pair[idx].reg2 = cnt;
        pair[idx].delta = MAX_3(abs(in_data_3d[num_rows_m1][ic + 1][0] - in_data_3d[num_rows_m1][ic][0]), abs(in_data_3d[num_rows_m1][ic + 1][1] - in_data_3d[num_rows_m1][ic][1]), abs(in_data_3d[num_rows_m1][ic + 1][2] - in_data_3d[num_rows_m1][ic][2]));
        idx++;
    }

    sorted = bucket_sort(pair, num_edges);

    for (ik = 0; ik < num_edges; ik++) {
        reg1 = find_set(parent, sorted[ik].reg1);
        reg2 = find_set(parent, sorted[ik].reg2);

        if (reg1 != reg2) {
            threshold = sqrt(thresh_factor * (((MIN_2(NUM_GRAY, size[reg1]) * log(1.0 + size[reg1]) + log_delta) / size[reg1]) + ((MIN_2(NUM_GRAY, size[reg2]) * log(1.0 + size[reg2]) + log_delta) / size[reg2])));

            if ((fabs(red_mean[reg1] - red_mean[reg2]) < threshold) && (fabs(green_mean[reg1] - green_mean[reg2]) < threshold) && (fabs(blue_mean[reg1] - blue_mean[reg2]) < threshold)) {
                root = union_set(reg1, reg2, parent, rank);
                total_size = size[reg1] + size[reg2];

                red_mean[root] = (size[reg1] * red_mean[reg1] + size[reg2] * red_mean[reg2]) / total_size;
                green_mean[root] = (size[reg1] * green_mean[reg1] + size[reg2] * green_mean[reg2]) / total_size;
                blue_mean[root] = (size[reg1] * blue_mean[reg1] + size[reg2] * blue_mean[reg2]) / total_size;
                size[root] = total_size;
            }
        }
    }

    cnt = 0;
    for (ir = 0; ir < num_rows; ir++) {
        cnt++;
        for (ic = 1; ic < num_cols; ic++) {
            reg1 = find_set(parent, cnt);
            reg2 = find_set(parent, cnt - 1);

            if ((reg1 != reg2) && ((size[reg2] < min_reg_size) || (size[reg1] < min_reg_size))) {
                root = union_set(reg1, reg2, parent, rank);
                total_size = size[reg1] + size[reg2];

                red_mean[root] = (size[reg1] * red_mean[reg1] + size[reg2] * red_mean[reg2]) / total_size;
                green_mean[root] = (size[reg1] * green_mean[reg1] + size[reg2] * green_mean[reg2]) / total_size;
                blue_mean[root] = (size[reg1] * blue_mean[reg1] + size[reg2] * blue_mean[reg2]) / total_size;
                size[root] = total_size;
            }
            cnt++;
        }
    }

    out_data = type_calloc(uchar, num_pixels);

    num_region = 0;
    for (ik = 0; ik < num_pixels; ik++) {
        rank[ik] = -1;
    }

    cnt = 0;
    for (ik = 0; ik < num_pixels_t3; ik += 3) {
        idx = find_set(parent, cnt);
        if (rank[idx] < 0) {
            rank[idx] = num_region;
            num_region++;
        }
        out_data[cnt] = rank[idx];
        cnt++;
    }

    *num_ccs = num_region;

    free(red_mean);
    free(green_mean);
    free(blue_mean);
    free(size);
    free(parent);
    free(rank);
    free(pair);
    free(sorted);

    matrix_free_index(in_data_3d);

    *output = out_data;

    return 0;
}

#undef MIN_2
#undef MAX_2
#undef MAX_3

double Q_value = 128;
double size_factor = 0.005;
