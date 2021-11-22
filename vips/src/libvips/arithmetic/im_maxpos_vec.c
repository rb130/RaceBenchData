

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdlib.h>
#include <float.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {

    int *xs;
    int *ys;
    double *vals;
    int *ptrs;
    int start;

} maxpos_list;

static maxpos_list *maxpos_list_alloc(int n);
static void maxpos_list_free(maxpos_list *list);

static void maxpos_list_init(maxpos_list *list, int n);
static void *maxpos_vec_start(IMAGE *unrequired, void *, void *);
static int maxpos_vec_scan(REGION *reg, void *seq, void *, void *);
static void add_to_maxpos_list(maxpos_list *list, int x, int y, double val);
static int maxpos_vec_stop(void *seq, void *, void *);

static void minpos_list_init(maxpos_list *list, int n);
static void *minpos_vec_start(IMAGE *unrequired, void *, void *);
static int minpos_vec_scan(REGION *reg, void *seq, void *, void *);
static void add_to_minpos_list(maxpos_list *list, int x, int y, double val);
static int minpos_vec_stop(void *seq, void *, void *);

int im_maxpos_vec(IMAGE *im, int *xpos, int *ypos, double *maxima, int n) {
#define FUNCTION_NAME "im_maxpos_vec"

    int result;
    int *pointers = im_malloc(NULL, n * sizeof(int *));
    maxpos_list master_list = {xpos, ypos, maxima, pointers, 0};

    if (im_pincheck(im)) {
        return -1;
    }

    if (!pointers) {
        return -1;
    }

    if (!(vips_bandfmt_isint(im->BandFmt) || vips_bandfmt_isfloat(im->BandFmt))) {
        im_error(FUNCTION_NAME, "%s", _("scalar images only"));
        return -1;
    }

    if (1 != im->Bands) {
        im_error(FUNCTION_NAME, "%s", _("single band images only"));
        return -1;
    }

    if (IM_CODING_NONE != im->Coding) {
        im_error(FUNCTION_NAME, "%s", _("uncoded images only"));
        return -1;
    }

    if (!xpos || !ypos || !maxima || n < 1) {
        im_error(FUNCTION_NAME, "%s", _("invalid argument"));
        return -1;
    }

    maxpos_list_init(&master_list, n);

    result = vips_sink(im, maxpos_vec_start, maxpos_vec_scan, maxpos_vec_stop, &n, &master_list);

    im_free(pointers);

    return result;
#undef FUNCTION_NAME
}

int im_minpos_vec(IMAGE *im, int *xpos, int *ypos, double *minima, int n) {
#define FUNCTION_NAME "im_minpos_vec"

    int result;
    int *pointers = im_malloc(NULL, n * sizeof(int *));
    maxpos_list master_list = {xpos, ypos, minima, pointers, 0};

    if (im_pincheck(im)) {
        return -1;
    }

    if (!pointers) {
        return -1;
    }

    if (!(vips_bandfmt_isint(im->BandFmt) || vips_bandfmt_isfloat(im->BandFmt))) {
        im_error(FUNCTION_NAME, "%s", _("scalar images only"));
        return -1;
    }

    if (1 != im->Bands) {
        im_error(FUNCTION_NAME, "%s", _("single band images only"));
        return -1;
    }

    if (IM_CODING_NONE != im->Coding) {
        im_error(FUNCTION_NAME, "%s", _("uncoded images only"));
        return -1;
    }

    if (!xpos || !ypos || !minima || n < 1) {
        im_error(FUNCTION_NAME, "%s", _("invalid argument"));
        return -1;
    }

    minpos_list_init(&master_list, n);

    result = vips_sink(im, minpos_vec_start, minpos_vec_scan, minpos_vec_stop, &n, &master_list);

    im_free(pointers);

    return result;
#undef FUNCTION_NAME
}

static maxpos_list *maxpos_list_alloc(int n) {

    maxpos_list *list = im_malloc(NULL, sizeof(maxpos_list));

    if (!list) {
        return NULL;
    }

    list->xs = im_malloc(NULL, 3 * n * sizeof(int));
    list->vals = im_malloc(NULL, n * sizeof(double));

    if (!list->xs || !list->vals) {
        im_free(list->xs);
        im_free(list->vals);
        im_free(list);
        return NULL;
    }
    list->ys = list->xs + n;
    list->ptrs = list->ys + n;

    return list;
}

static void maxpos_list_free(maxpos_list *list) {
    im_free(list->xs);
    im_free(list->vals);
    im_free(list);
}

static void maxpos_list_init(maxpos_list *list, int n) {
    int i;

    for (i = 0; i < n; ++i) {
        list->xs[i] = 0;
        list->ys[i] = 0;
        list->vals[i] = 0;
        list->ptrs[i] = i + 1;
    }

    list->ptrs[n - 1] = -1;
    list->start = 0;
}

static void *maxpos_vec_start(IMAGE *unrequired, void *a, void *b) {

    int *n = (int *)a;
    maxpos_list *list = maxpos_list_alloc(*n);

    if (!list) {
        return NULL;
    }

    maxpos_list_init(list, *n);

    return list;
}

static int maxpos_vec_scan(REGION *reg, void *seq, void *a, void *b) {

    maxpos_list *list = (maxpos_list *)seq;

#define MAXPOS_VEC_SCAN(type)                                                          \
    {                                                                                  \
                                                                                       \
        int y = reg->valid.top;                                                        \
        int x;                                                                         \
        int ymax = y + reg->valid.height;                                              \
        int xmax = reg->valid.left + reg->valid.width;                                 \
                                                                                       \
        type *row = (type *)IM_REGION_ADDR(reg, reg->valid.left, y) - reg->valid.left; \
        size_t skip = IM_REGION_LSKIP(reg) / sizeof(type);                             \
                                                                                       \
        for (; y < ymax; ++y, row += skip)                                             \
            for (x = reg->valid.left; x < xmax; ++x)                                   \
                if (row[x] > list->vals[list->start])                                  \
                    add_to_maxpos_list(list, x, y, row[x]);                            \
    }

    switch (reg->im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        MAXPOS_VEC_SCAN(guint8) break;
    case IM_BANDFMT_CHAR:
        MAXPOS_VEC_SCAN(gint8) break;
    case IM_BANDFMT_USHORT:
        MAXPOS_VEC_SCAN(guint16) break;
    case IM_BANDFMT_SHORT:
        MAXPOS_VEC_SCAN(gint16) break;
    case IM_BANDFMT_UINT:
        MAXPOS_VEC_SCAN(guint32) break;
    case IM_BANDFMT_INT:
        MAXPOS_VEC_SCAN(gint32) break;
    case IM_BANDFMT_FLOAT:
        MAXPOS_VEC_SCAN(float) break;
    case IM_BANDFMT_DOUBLE:
        MAXPOS_VEC_SCAN(double) break;
    default:
        g_assert(0);
    }

#undef MAXPOS_VEC_SCAN

    return 0;
}

static void add_to_maxpos_list(maxpos_list *list, int x, int y, double val) {

    int pointer = list->start;

    while (-1 != list->ptrs[pointer] && val > list->vals[list->ptrs[pointer]]) {
        pointer = list->ptrs[pointer];
    }

    list->xs[list->start] = x;
    list->ys[list->start] = y;
    list->vals[list->start] = val;

    if (list->start != pointer) {

        int second = list->ptrs[list->start];

        list->ptrs[list->start] = list->ptrs[pointer];
        list->ptrs[pointer] = list->start;
        list->start = second;
    }
}

static int maxpos_vec_stop(void *seq, void *a, void *b) {

    maxpos_list *list = (maxpos_list *)seq;
    maxpos_list *master_list = (maxpos_list *)b;

    int prev = -1;
    int pointer = list->start;

    while (-1 != list->ptrs[pointer]) {

        int next = list->ptrs[pointer];

        list->ptrs[pointer] = prev;
        prev = pointer;
        pointer = next;
    }
    list->ptrs[pointer] = prev;
    list->start = pointer;

    for (; - 1 != pointer; pointer = list->ptrs[pointer]) {

        if (list->vals[pointer] > master_list->vals[master_list->start]) {
            add_to_maxpos_list(master_list, list->xs[pointer], list->ys[pointer], list->vals[pointer]);
        } else {
            break;
        }
    }

    maxpos_list_free(list);

    return 0;
}

static void minpos_list_init(maxpos_list *list, int n) {
    int i;

    for (i = 0; i < n; ++i) {
        list->xs[i] = 0;
        list->ys[i] = 0;
        list->vals[i] = DBL_MAX;
        list->ptrs[i] = i + 1;
    }

    list->ptrs[n - 1] = -1;
    list->start = 0;
}

static void *minpos_vec_start(IMAGE *unrequired, void *a, void *b) {

    int *n = (int *)a;
    maxpos_list *list = maxpos_list_alloc(*n);

    if (!list) {
        return NULL;
    }

    minpos_list_init(list, *n);

    return list;
}

static int minpos_vec_scan(REGION *reg, void *seq, void *a, void *b) {

    maxpos_list *list = (maxpos_list *)seq;

#define MINPOS_VEC_SCAN(type)                                                          \
    {                                                                                  \
                                                                                       \
        int y = reg->valid.top;                                                        \
        int x;                                                                         \
        int ymax = y + reg->valid.height;                                              \
        int xmax = reg->valid.left + reg->valid.width;                                 \
                                                                                       \
        type *row = (type *)IM_REGION_ADDR(reg, reg->valid.left, y) - reg->valid.left; \
        size_t skip = IM_REGION_LSKIP(reg) / sizeof(type);                             \
                                                                                       \
        for (; y < ymax; ++y, row += skip)                                             \
            for (x = reg->valid.left; x < xmax; ++x)                                   \
                if (row[x] < list->vals[list->start])                                  \
                    add_to_minpos_list(list, x, y, row[x]);                            \
    }

    switch (reg->im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        MINPOS_VEC_SCAN(guint8) break;
    case IM_BANDFMT_CHAR:
        MINPOS_VEC_SCAN(gint8) break;
    case IM_BANDFMT_USHORT:
        MINPOS_VEC_SCAN(guint16) break;
    case IM_BANDFMT_SHORT:
        MINPOS_VEC_SCAN(gint16) break;
    case IM_BANDFMT_UINT:
        MINPOS_VEC_SCAN(guint32) break;
    case IM_BANDFMT_INT:
        MINPOS_VEC_SCAN(gint32) break;
    case IM_BANDFMT_FLOAT:
        MINPOS_VEC_SCAN(float) break;
    case IM_BANDFMT_DOUBLE:
        MINPOS_VEC_SCAN(double) break;
    default:
        g_assert(0);
    }

#undef MINPOS_VEC_SCAN

    return 0;
}

static void add_to_minpos_list(maxpos_list *list, int x, int y, double val) {

    int pointer = list->start;

    while (-1 != list->ptrs[pointer] && val < list->vals[list->ptrs[pointer]]) {
        pointer = list->ptrs[pointer];
    }

    list->xs[list->start] = x;
    list->ys[list->start] = y;
    list->vals[list->start] = val;

    if (list->start != pointer) {

        int second = list->ptrs[list->start];

        list->ptrs[list->start] = list->ptrs[pointer];
        list->ptrs[pointer] = list->start;
        list->start = second;
    }
}

static int minpos_vec_stop(void *seq, void *a, void *b) {

    maxpos_list *list = (maxpos_list *)seq;
    maxpos_list *master_list = (maxpos_list *)b;
    int prev = -1;
    int pointer = list->start;

    while (-1 != list->ptrs[pointer]) {

        int next = list->ptrs[pointer];

        list->ptrs[pointer] = prev;
        prev = pointer;
        pointer = next;
    }
    list->ptrs[pointer] = prev;
    list->start = pointer;

    for (; - 1 != pointer; pointer = list->ptrs[pointer]) {

        if (list->vals[pointer] < master_list->vals[master_list->start]) {
            add_to_minpos_list(master_list, list->xs[pointer], list->ys[pointer], list->vals[pointer]);
        } else {
            break;
        }
    }

    maxpos_list_free(list);

    return 0;
}
