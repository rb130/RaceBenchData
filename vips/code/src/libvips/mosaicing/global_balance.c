
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/transform.h>

#include "merge.h"
#include "global_balance.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MAX_ITEMS (50)

#define TRIVIAL (20 * 20)

static int break_items(char *line, char **out) {
    int i;
    char *p;

    for (i = 0; i < MAX_ITEMS; i++) {

        if (!(p = strchr(line, '<'))) {
            break;
        }

        out[i] = line = p + 1;

        if (!(p = strchr(line, '>'))) {
            im_error("break_files", "%s", _("no matching '>'"));
            return (-1);
        }

        *p = '\0';
        line = p + 1;
    }

    if (i == MAX_ITEMS) {
        im_error("break_files", "%s", _("too many items"));
        return (-1);
    }

    return (i);
}

IMAGE *im__global_open_image(SymbolTable *st, char *name) {
    IMAGE *im;

    if ((im = im_open_local(st->im, name, "r"))) {
        return (im);
    }
    if ((im = im_open_local(st->im, im_skip_dir(name), "r"))) {
        return (im);
    }

    return (NULL);
}

static int junk_node(JoinNode *node) {
    IM_FREEF(g_slist_free, node->overlaps);

    return (0);
}

static int hash(char *n) {
    int i;
    int r = 0;
    int l = strlen(n);

    for (i = 0; i < l; i++) {
        r = ((r + n[i]) * 43) & 0xffffff;
    }

    return (r % SYM_TAB_SIZE);
}

static JoinNode *build_node(SymbolTable *st, char *name) {
    JoinNode *node = IM_NEW(st->im, JoinNode);
    int n = hash(name);

    if (!node || !(node->name = im_strdup(st->im, name))) {
        return (NULL);
    }

    node->type = JOIN_LEAF;
    node->dirty = 0;
    node->mwidth = -2;
    node->st = st;
    im__transform_init(&node->cumtrn);
    node->trnim = NULL;
    node->arg1 = NULL;
    node->arg2 = NULL;
    node->overlaps = NULL;
    node->im = NULL;
    node->index = 0;

    if (im_add_close_callback(st->im, (im_callback_fn)junk_node, node, NULL)) {
        return (NULL);
    }

    if ((node->im = im__global_open_image(st, name))) {

        node->cumtrn.oarea.width = node->im->Xsize;
        node->cumtrn.oarea.height = node->im->Ysize;
    } else {

        im_error_clear();
    }

    st->table[n] = g_slist_prepend(st->table[n], node);

    return (node);
}

static OverlapInfo *build_overlap(JoinNode *node, JoinNode *other, Rect *overlap) {
    OverlapInfo *lap = IM_NEW(node->st->im, OverlapInfo);

    if (!lap) {
        return (NULL);
    }

    lap->node = node;
    lap->other = other;
    lap->overlap = *overlap;
    lap->nstats = NULL;
    lap->ostats = NULL;
    node->overlaps = g_slist_prepend(node->overlaps, lap);
    node->st->novl++;

    return (lap);
}

static void overlap_destroy(OverlapInfo *lap) {
    JoinNode *node = lap->node;

    node->overlaps = g_slist_remove(node->overlaps, lap);
    assert(node->st->novl > 0);
    node->st->novl--;
}

static int junk_table(SymbolTable *st) {
    int i;

    for (i = 0; i < st->sz; i++) {
        IM_FREEF(g_slist_free, st->table[i]);
    }

    return (0);
}

SymbolTable *im__build_symtab(IMAGE *out, int sz) {
    SymbolTable *st = IM_NEW(out, SymbolTable);
    int i;

    if (!st) {
        return (NULL);
    }
    if (!(st->table = IM_ARRAY(out, sz, GSList *))) {
        return (NULL);
    }
    st->sz = sz;
    st->im = out;
    st->novl = 0;
    st->nim = 0;
    st->njoin = 0;
    st->root = NULL;
    st->leaf = NULL;
    st->fac = NULL;

    if (im_add_close_callback(out, (im_callback_fn)junk_table, st, NULL)) {
        return (NULL);
    }

    for (i = 0; i < sz; i++) {
        st->table[i] = NULL;
    }

    return (st);
}

static JoinNode *test_name(JoinNode *node, char *name) {
    if (strcmp(node->name, name) == 0) {
        return (node);
    } else {
        return (NULL);
    }
}

static JoinNode *find_node(SymbolTable *st, char *name) {
    return (im_slist_map2(st->table[hash(name)], (VSListMap2Fn)test_name, name, NULL));
}

static JoinNode *add_node(SymbolTable *st, char *name) {
    JoinNode *node;

    if (!(node = find_node(st, name)) && !(node = build_node(st, name))) {
        return (NULL);
    }

    return (node);
}

void *im__map_table(SymbolTable *st, void *(*fn)(), void *a, void *b) {
    int i;
    void *r;

    for (i = 0; i < st->sz; i++) {
        if ((r = im_slist_map2(st->table[i], (VSListMap2Fn)fn, a, b))) {
            return (r);
        }
    }

    return (NULL);
}

static void *set_dirty(JoinNode *node, int state) {
    node->dirty = state;

    return (NULL);
}

static void clean_table(SymbolTable *st) {
    (void)im__map_table(st, set_dirty, (void *)0, NULL);
}

static void calc_geometry(JoinNode *node) {
    Rect um;

    switch (node->type) {
    case JOIN_LR:
    case JOIN_TB:
    case JOIN_LRROTSCALE:
    case JOIN_TBROTSCALE:

        im_rect_unionrect(&node->arg1->cumtrn.oarea, &node->arg2->cumtrn.oarea, &um);
        node->cumtrn.iarea.left = 0;
        node->cumtrn.iarea.top = 0;
        node->cumtrn.iarea.width = um.width;
        node->cumtrn.iarea.height = um.height;
        im__transform_set_area(&node->cumtrn);
        break;

    case JOIN_CP:

        node->cumtrn = node->arg1->cumtrn;
        break;

    case JOIN_LEAF:

        if (node->im) {
            node->cumtrn.iarea.left = 0;
            node->cumtrn.iarea.top = 0;
            node->cumtrn.iarea.width = node->im->Xsize;
            node->cumtrn.iarea.height = node->im->Ysize;
            im__transform_set_area(&node->cumtrn);
        }
        break;

    default:
        error_exit("internal error #98356");
    }
}

static int propogate_transform(JoinNode *node, Transformation *trn) {
    if (!node) {
        return (0);
    }

    if (node->dirty && node->arg1 && node->arg2) {
        im_error("im_global_balance", "%s", _("circularity detected"));
        return (-1);
    }
    node->dirty = 1;

    if (propogate_transform(node->arg1, trn) || propogate_transform(node->arg2, trn)) {
        return (-1);
    }

    im__transform_add(&node->cumtrn, trn, &node->cumtrn);
    calc_geometry(node);

    return (0);
}

static int make_join(SymbolTable *st, JoinType type, JoinNode *arg1, JoinNode *arg2, JoinNode *out, double a, double b, double dx, double dy, int mwidth) {
    Transformation trn;

    if (out->type != JOIN_LEAF) {
        im_error("im_global_balance", _("image \"%s\" used twice as output"), out->name);
        return (-1);
    }

    out->type = type;
    out->mwidth = mwidth;
    out->a = a;
    out->b = b;
    out->dx = dx;
    out->dy = dy;
    out->arg1 = arg1;
    out->arg2 = arg2;
    out->thistrn.a = a;
    out->thistrn.b = -b;
    out->thistrn.c = b;
    out->thistrn.d = a;
    out->thistrn.dx = dx;
    out->thistrn.dy = dy;

    clean_table(st);
    if (propogate_transform(arg2, &out->thistrn)) {
        return (-1);
    }

    calc_geometry(out);

    trn.a = 1.0;
    trn.b = 0.0;
    trn.c = 0.0;
    trn.d = 1.0;
    trn.dx = -out->cumtrn.oarea.left;
    trn.dy = -out->cumtrn.oarea.top;
    clean_table(st);
    if (propogate_transform(out, &trn)) {
        return (-1);
    }

    return (0);
}

static int make_copy(SymbolTable *st, JoinNode *before, JoinNode *after) {

    if (after->type != JOIN_LEAF) {
        im_error("im_global_balance", _("image \"%s\" used twice as output"), after->name);
        return (-1);
    }

    after->type = JOIN_CP;
    after->arg1 = before;
    after->arg2 = NULL;

    calc_geometry(after);

    return (0);
}

static int process_line(SymbolTable *st, const char *text) {
    char line[1024];

#ifdef DEBUG
    printf("read: %s\n", text);
#endif

    im_strncpy(line, text, 1024);

    if (im_isprefix("#LRJOIN ", line) || im_isprefix("#TBJOIN ", line)) {

        char *item[MAX_ITEMS];
        int nitems;
        JoinType type;
        JoinNode *arg1, *arg2, *join;
        int dx, dy, mwidth;

        if ((nitems = break_items(line, item)) < 0) {
            return (-1);
        }
        if (nitems != 5 && nitems != 6) {
            im_error("global_balance", "%s", _("bad number of args in join line"));
            return (-1);
        }

        if (!(arg1 = add_node(st, item[0])) || !(arg2 = add_node(st, item[1])) || !(join = add_node(st, item[2]))) {
            return (-1);
        }
        dx = atoi(item[3]);
        dy = atoi(item[4]);
        if (nitems == 6) {
            mwidth = atoi(item[5]);
        } else {
            mwidth = -1;
        }
        if (im_isprefix("#LRJOIN ", line)) {
            type = JOIN_LR;
        } else {
            type = JOIN_TB;
        }

        if (make_join(st, type, arg1, arg2, join, 1.0, 0.0, dx, dy, mwidth)) {
            return (-1);
        }
    } else if (im_isprefix("#LRROTSCALE ", line) || im_isprefix("#TBROTSCALE ", line)) {

        char *item[MAX_ITEMS];
        int nitems;
        JoinType type;
        JoinNode *arg1, *arg2, *join;
        double a, b, dx, dy;
        int mwidth;

        if ((nitems = break_items(line, item)) < 0) {
            return (-1);
        }
        if (nitems != 7 && nitems != 8) {
            im_error("global_balance", "%s", _("bad number of args in join1 line"));
            return (-1);
        }

        if (!(arg1 = add_node(st, item[0])) || !(arg2 = add_node(st, item[1])) || !(join = add_node(st, item[2]))) {
            return (-1);
        }
        a = g_ascii_strtod(item[3], NULL);
        b = g_ascii_strtod(item[4], NULL);
        dx = g_ascii_strtod(item[5], NULL);
        dy = g_ascii_strtod(item[6], NULL);
        if (nitems == 8) {
            mwidth = atoi(item[7]);
        } else {
            mwidth = -1;
        }
        if (im_isprefix("#LRROTSCALE ", line)) {
            type = JOIN_LRROTSCALE;
        } else {
            type = JOIN_TBROTSCALE;
        }

        if (make_join(st, type, arg1, arg2, join, a, b, dx, dy, mwidth)) {
            return (-1);
        }
    } else if (im_isprefix("copy ", line)) {

        char *item[MAX_ITEMS];
        int nitems;
        JoinNode *before, *after;

        if ((nitems = break_items(line, item)) < 0) {
            return (-1);
        }
        if (nitems != 2) {
            im_error("global_balance", "%s", _("bad number of args in copy line"));
            return (-1);
        }

        if (!(before = add_node(st, item[0])) || !(after = add_node(st, item[1])) || make_copy(st, before, after)) {
            return (-1);
        }
    }

    return (0);
}

static void *set_referenced(JoinNode *node) {
    if (node->arg1) {
        node->arg1->dirty = 1;
    }
    if (node->arg2) {
        node->arg2->dirty = 1;
    }

    return (NULL);
}

static void *is_root(JoinNode *node) {
    if (!node->dirty) {
        return ((void *)node);
    } else {
        return (NULL);
    }
}

static JoinNode *find_root(SymbolTable *st) {
    JoinNode *root;
    JoinNode *notroot;

    clean_table(st);
    im__map_table(st, set_referenced, NULL, NULL);

    root = (JoinNode *)im__map_table(st, is_root, NULL, NULL);

    if (!root) {
        im_error("im_global_balance", "%s",
            _("mosaic root not found in desc file\n"
              "is this really a mosaiced image?"));
        return (NULL);
    }

    root->dirty = 1;
    if ((notroot = im__map_table(st, is_root, NULL, NULL))) {
        im_error("im_global_balance", "%s", _("more than one root"));
        return (NULL);
    }

    return (root);
}

int im__parse_desc(SymbolTable *st, IMAGE *in) {
    GSList *p;

    for (p = in->history_list; p; p = p->next) {
        GValue *value = (GValue *)p->data;

        assert(G_VALUE_TYPE(value) == IM_TYPE_REF_STRING);

        if (process_line(st, im_ref_string_get(value))) {
            return (-1);
        }
    }

    if (!(st->root = find_root(st))) {
        return (-1);
    }

    return (0);
}

static void *count_leaves(JoinNode *node) {
    if (node->type == JOIN_LEAF) {
        node->index = node->st->nim;
        node->st->nim++;
    }

    return (NULL);
}

#ifdef DEBUG

static void print_node(JoinNode *node) {
    printf("%s, position %dx%d, size %dx%d, index %d\n", im_skip_dir(node->name), node->cumtrn.oarea.left, node->cumtrn.oarea.top, node->cumtrn.oarea.width, node->cumtrn.oarea.height, node->index);
}
#endif

#ifdef DEBUG

static void *print_leaf(JoinNode *node) {
    if (node->type == JOIN_LEAF)
        print_node(node);

    return (NULL);
}
#endif

static void *count_joins(JoinNode *node) {
    if (node->type == JOIN_TB || node->type == JOIN_LR || node->type == JOIN_LRROTSCALE || node->type == JOIN_TBROTSCALE) {
        node->st->njoin++;
    }

    return (NULL);
}

#ifdef DEBUG

static void spc(int n) {
    int i;

    for (i = 0; i < n; i++)
        printf(" ");
}
#endif

#ifdef DEBUG
static char *JoinType2char(JoinType type) {
    switch (type) {
    case JOIN_LR:
        return ("JOIN_LR");
    case JOIN_TB:
        return ("JOIN_TB");
    case JOIN_LRROTSCALE:
        return ("JOIN_LRROTSCALE");
    case JOIN_TBROTSCALE:
        return ("JOIN_TBROTSCALE");
    case JOIN_CP:
        return ("JOIN_CP");
    case JOIN_LEAF:
        return ("JOIN_LEAF");

    default:
        error_exit("internal error #9275");

        return (NULL);
    }
}
#endif

#ifdef DEBUG

static void *print_joins(JoinNode *node, int indent) {
    switch (node->type) {
    case JOIN_TB:
    case JOIN_LR:
    case JOIN_TBROTSCALE:
    case JOIN_LRROTSCALE:
        spc(indent);
        printf("%s to make %s, size %dx%d, pos. %dx%d, of:\n", JoinType2char(node->type), im_skip_dir(node->name), node->cumtrn.oarea.width, node->cumtrn.oarea.height, node->cumtrn.oarea.left, node->cumtrn.oarea.top);
        spc(indent);
        printf("reference:\n");
        print_joins(node->arg1, indent + 2);
        spc(indent);
        printf("secondary:\n");
        print_joins(node->arg2, indent + 2);
        break;

    case JOIN_CP:
        spc(indent);
        printf("copy to make %s of:\n", im_skip_dir(node->name));
        print_joins(node->arg1, indent + 2);
        break;

    case JOIN_LEAF:
        spc(indent);
        printf("input image %s\n", im_skip_dir(node->name));
        break;
    }

    return (NULL);
}
#endif

#ifdef DEBUG

static void *print_overlap(OverlapInfo *lap) {
    printf("-> %s overlaps with %s; (this, other) = (%.4G, %.4G)\n", im_skip_dir(lap->node->name), im_skip_dir(lap->other->name), lap->nstats->coeff[4], lap->ostats->coeff[4]);

    return (NULL);
}
#endif

#ifdef DEBUG

static void *print_overlaps(JoinNode *node) {
    if (node->type == JOIN_LEAF && g_slist_length(node->overlaps) > 0) {
        printf("overlap of %s with:\n", im_skip_dir(node->name));
        im_slist_map2(node->overlaps, (VSListMap2Fn)print_overlap, NULL, NULL);
    }

    return (NULL);
}
#endif

#ifdef DEBUG

static void *print_overlap_error(OverlapInfo *lap, double *fac, double *total) {
    double na = lap->nstats->coeff[4];
    double oa = lap->ostats->coeff[4];
    double err;

    if (fac) {
        na *= fac[lap->node->index];
        oa *= fac[lap->other->index];
    }

    err = na - oa;

    printf("-> file %s, error = %g\n", im_skip_dir(lap->other->name), err);
    *total += err * err;

    return (NULL);
}
#endif

#ifdef DEBUG

static void *print_overlap_errors(JoinNode *node, double *fac, double *total) {
    if (node->type == JOIN_LEAF && g_slist_length(node->overlaps) > 0) {
        printf("overlap of %s (index %d) with:\n", im_skip_dir(node->name), node->index);
        im_slist_map2(node->overlaps, (VSListMap2Fn)print_overlap_error, fac, total);
    }

    return (NULL);
}
#endif

static int extract_rect(IMAGE *in, IMAGE *out, Rect *r) {
    return (im_extract_area(in, out, r->left, r->top, r->width, r->height));
}

static int make_overlap_mask(IMAGE *ref, IMAGE *sec, IMAGE *mask, Rect *rarea, Rect *sarea) {
    IMAGE *t[6];

    if (im_open_local_array(mask, t, 6, "mytemps", "p") || extract_rect(ref, t[0], rarea) || extract_rect(sec, t[1], sarea) || im_extract_band(t[0], t[2], 0) || im_extract_band(t[1], t[3], 0) || im_notequalconst(t[2], t[4], 0.0) || im_notequalconst(t[3], t[5], 0.0) || im_andimage(t[4], t[5], mask)) {
        return (-1);
    }

    return (0);
}

static int count_nonzero(IMAGE *in, gint64 *count) {
    double avg;

    if (im_avg(in, &avg)) {
        return (-1);
    }
    *count = (avg * in->Xsize * in->Ysize) / 255.0;

    return (0);
}

static DOUBLEMASK *find_image_stats(IMAGE *in, IMAGE *mask, Rect *area) {
    DOUBLEMASK *stats;
    IMAGE *t[4];
    gint64 count;

    if (im_open_local_array(in, t, 4, "find_image_stats", "p") || extract_rect(in, t[0], area) || im_black(t[1], t[0]->Xsize, t[0]->Ysize, t[0]->Bands) || im_clip2fmt(t[1], t[2], t[0]->BandFmt) || im_ifthenelse(mask, t[0], t[2], t[3])) {
        return (NULL);
    }

    if (!(stats = im_local_dmask(in, im_stats(t[3])))) {
        return (NULL);
    }

    if (count_nonzero(mask, &count)) {
        return (NULL);
    }

    stats->coeff[4] *= (double)count / ((double)mask->Xsize * mask->Ysize);

    stats->coeff[5] = count;

#ifdef DEBUG
    if (count == 0)
        im_warn("global_balance", "%s", _("empty overlap!"));
#endif

    return (stats);
}

static int find_overlap_stats(OverlapInfo *lap) {
    IMAGE *t1 = im_open_local(lap->node->im, "find_overlap_stats:1", "p");
    Rect rarea, sarea;

    rarea = lap->overlap;
    rarea.left -= lap->node->cumtrn.oarea.left;
    rarea.top -= lap->node->cumtrn.oarea.top;

    sarea = lap->overlap;
    sarea.left -= lap->other->cumtrn.oarea.left;
    sarea.top -= lap->other->cumtrn.oarea.top;

    if (make_overlap_mask(lap->node->trnim, lap->other->trnim, t1, &rarea, &sarea)) {
        return (-1);
    }

    if (!(lap->nstats = find_image_stats(lap->node->trnim, t1, &rarea))) {
        return (-1);
    }
    if (!(lap->ostats = find_image_stats(lap->other->trnim, t1, &sarea))) {
        return (-1);
    }

    return (0);
}

static void *overlap_eq(OverlapInfo *this, JoinNode *node) {
    if (this->other == node) {
        return (this);
    } else {
        return (NULL);
    }
}

static void *test_overlap(JoinNode *other, JoinNode *node) {
    Rect overlap;
    OverlapInfo *lap;

    if (other->type != JOIN_LEAF || node == other) {
        return (NULL);
    }

    im_rect_intersectrect(&node->cumtrn.oarea, &other->cumtrn.oarea, &overlap);
    if (im_rect_isempty(&overlap)) {
        return (NULL);
    }

    if (overlap.width * overlap.height < TRIVIAL) {

        return (NULL);
    }

    if (im_slist_map2(other->overlaps, (VSListMap2Fn)overlap_eq, node, NULL)) {
        return (NULL);
    }

    if (!(lap = build_overlap(node, other, &overlap))) {
        return (node);
    }

    if (find_overlap_stats(lap)) {
        return (node);
    }

    if (lap->nstats->coeff[5] < TRIVIAL || lap->ostats->coeff[5] < TRIVIAL) {
#ifdef DEBUG
        printf("trivial overlap ... junking\n");
        printf("nstats count = %g, ostats count = %g\n", lap->nstats->coeff[5], lap->ostats->coeff[5]);
        print_overlap(lap);
#endif
        overlap_destroy(lap);
    }

    return (NULL);
}

static void *find_overlaps(JoinNode *node, SymbolTable *st) {
    if (node->type == JOIN_LEAF) {

        if (!node->im) {
            im_error("im_global_balance", _("unable to open \"%s\""), node->name);
            return (node);
        }
        if (!node->trnim) {
            error_exit("global_balance: sanity failure #9834");
        }

        return (im__map_table(st, test_overlap, node, NULL));
    }

    return (NULL);
}

typedef struct {
    SymbolTable *st;
    JoinNode *leaf;
    DOUBLEMASK *K;
    DOUBLEMASK *M;
    int row;
} MatrixBundle;

static void *add_nominated(OverlapInfo *ovl, MatrixBundle *bun, double *gamma) {
    double *Kp = bun->K->coeff + bun->row;
    double *Mp = bun->M->coeff + bun->row * bun->M->xsize;
    double ns = pow(ovl->nstats->coeff[4], 1 / (*gamma));
    double os = pow(ovl->ostats->coeff[4], 1 / (*gamma));

    Kp[0] = ns;
    Mp[ovl->other->index - 1] = os;

    bun->row++;

    return (NULL);
}

static void *add_other(OverlapInfo *ovl, MatrixBundle *bun, double *gamma) {
    double *Mp = bun->M->coeff + bun->row * bun->M->xsize;
    double ns = -pow(ovl->nstats->coeff[4], 1 / (*gamma));
    double os = pow(ovl->ostats->coeff[4], 1 / (*gamma));

    Mp[ovl->node->index - 1] = ns;
    Mp[ovl->other->index - 1] = os;

    bun->row++;

    return (NULL);
}

static void *add_row(JoinNode *node, MatrixBundle *bun, double *gamma) {
    if (node == bun->leaf) {
        im_slist_map2(node->overlaps, (VSListMap2Fn)add_nominated, bun, gamma);
    } else {
        im_slist_map2(node->overlaps, (VSListMap2Fn)add_other, bun, gamma);
    }

    return (NULL);
}

static void fill_matricies(SymbolTable *st, double gamma, DOUBLEMASK *K, DOUBLEMASK *M) {
    MatrixBundle bun;

    bun.st = st;
    bun.leaf = st->leaf;
    bun.K = K;
    bun.M = M;
    bun.row = 0;

    im__map_table(st, add_row, &bun, &gamma);
}

static void *choose_leaf(JoinNode *node) {
    if (node->type == JOIN_LEAF) {
        return (node);
    }

    return (NULL);
}

static IMAGE *make_mos_image(SymbolTable *st, JoinNode *node, transform_fn tfn, void *a) {
    IMAGE *im1, *im2, *out;

    switch (node->type) {
    case JOIN_LR:
    case JOIN_TB:
        if (!(im1 = make_mos_image(st, node->arg1, tfn, a)) || !(im2 = make_mos_image(st, node->arg2, tfn, a)) || !(out = im_open_local(st->im, node->name, "p"))) {
            return (NULL);
        }

        if (node->type == JOIN_LR) {
            if (im_lrmerge(im1, im2, out, -node->dx, -node->dy, node->mwidth)) {
                return (NULL);
            }
        } else {
            if (im_tbmerge(im1, im2, out, -node->dx, -node->dy, node->mwidth)) {
                return (NULL);
            }
        }

        break;

    case JOIN_LRROTSCALE:
    case JOIN_TBROTSCALE:
        if (!(im1 = make_mos_image(st, node->arg1, tfn, a)) || !(im2 = make_mos_image(st, node->arg2, tfn, a)) || !(out = im_open_local(st->im, node->name, "p"))) {
            return (NULL);
        }

        if (node->type == JOIN_LRROTSCALE) {
            if (im__lrmerge1(im1, im2, out, node->a, node->b, node->dx, node->dy, node->mwidth)) {
                return (NULL);
            }
        } else {
            if (im__tbmerge1(im1, im2, out, node->a, node->b, node->dx, node->dy, node->mwidth)) {
                return (NULL);
            }
        }

        break;

    case JOIN_LEAF:

        if (!(out = tfn(node, a))) {
            return (NULL);
        }

        break;

    case JOIN_CP:

        out = make_mos_image(st, node->arg1, tfn, a);

        break;

    default:
        error_exit("internal error #982369824375987");

        return (NULL);
    }

    return (out);
}

int im__build_mosaic(SymbolTable *st, IMAGE *out, transform_fn tfn, void *a) {
    JoinNode *root = st->root;
    IMAGE *im1, *im2;

    switch (root->type) {
    case JOIN_LR:
    case JOIN_TB:
        if (!(im1 = make_mos_image(st, root->arg1, tfn, a)) || !(im2 = make_mos_image(st, root->arg2, tfn, a))) {
            return (-1);
        }

        if (root->type == JOIN_LR) {
            if (im_lrmerge(im1, im2, out, -root->dx, -root->dy, root->mwidth)) {
                return (-1);
            }
        } else {
            if (im_tbmerge(im1, im2, out, -root->dx, -root->dy, root->mwidth)) {
                return (-1);
            }
        }

        break;

    case JOIN_LRROTSCALE:
    case JOIN_TBROTSCALE:
        if (!(im1 = make_mos_image(st, root->arg1, tfn, a)) || !(im2 = make_mos_image(st, root->arg2, tfn, a))) {
            return (-1);
        }

        if (root->type == JOIN_LRROTSCALE) {
            if (im__lrmerge1(im1, im2, out, root->a, root->b, root->dx, root->dy, root->mwidth)) {
                return (-1);
            }
        } else {
            if (im__tbmerge1(im1, im2, out, root->a, root->b, root->dx, root->dy, root->mwidth)) {
                return (-1);
            }
        }

        break;

    case JOIN_LEAF:

        if (!(im1 = tfn(root, a)) || im_copy(im1, out)) {
            return (-1);
        }

        break;

    case JOIN_CP:

        if (!(im1 = make_mos_image(st, root->arg1, tfn, a))) {
            return (-1);
        }
        if (im_copy(im1, out)) {
            return (-1);
        }

        break;

    default:
        error_exit("internal error #982369824375987");
    }

    return (0);
}

static int find_factors(SymbolTable *st, double gamma) {
    DOUBLEMASK *K;
    DOUBLEMASK *M;
    DOUBLEMASK *m1, *m2, *m3, *m4, *m5;
    double total;
    double avg;
    int i;

    if (!(K = im_local_dmask(st->im, im_create_dmask("K", 1, st->novl))) || !(M = im_local_dmask(st->im, im_create_dmask("M", st->nim - 1, st->novl)))) {
        return (-1);
    }
    fill_matricies(st, gamma, K, M);
#ifdef DEBUG
    im_write_dmask(K);
    im_write_dmask(M);
#endif

    if (!(m1 = im_local_dmask(st->im, im_mattrn(M, "lms:1")))) {
        return (-1);
    }
    if (!(m2 = im_local_dmask(st->im, im_matmul(m1, M, "lms:2")))) {
        return (-1);
    }
    if (!(m3 = im_local_dmask(st->im, im_matinv(m2, "lms:3")))) {
        return (-1);
    }
    if (!(m4 = im_local_dmask(st->im, im_matmul(m3, m1, "lms:4")))) {
        return (-1);
    }
    if (!(m5 = im_local_dmask(st->im, im_matmul(m4, K, "lms:5")))) {
        return (-1);
    }

    if (!(st->fac = IM_ARRAY(st->im, st->nim, double))) {
        return (-1);
    }
    for (i = 0; i < m5->ysize; i++) {
        st->fac[i + 1] = m5->coeff[i];
    }
    st->fac[0] = 1.0;

    total = 0.0;
    for (i = 0; i < st->nim; i++) {
        total += st->fac[i];
    }
    avg = total / st->nim;
    for (i = 0; i < st->nim; i++) {
        st->fac[i] /= avg;
    }

#ifdef DEBUG

    printf("debugging output for im_global_balance():\n");
    for (i = 0; i < st->nim; i++)
        printf("balance factor %d = %g\n", i, st->fac[i]);
    total = 0.0;
    printf("Overlap errors:\n");
    im__map_table(st, print_overlap_errors, NULL, &total);
    printf("RMS error = %g\n", sqrt(total / st->novl));

    total = 0.0;
    printf("Overlap errors after adjustment:\n");
    im__map_table(st, print_overlap_errors, st->fac, &total);
    printf("RMS error = %g\n", sqrt(total / st->novl));
#endif

    return (0);
}

static void *generate_trn_leaves(JoinNode *node, SymbolTable *st) {
    if (node->type == JOIN_LEAF) {

        if (!node->im) {
            im_error("im_global_balance", _("unable to open \"%s\""), node->name);
            return (node);
        }
        if (node->trnim) {
            error_exit("global_balance: sanity failure #765");
        }

        if (im__transform_isidentity(&node->cumtrn)) {
            node->trnim = node->im;
        } else if (!(node->trnim = im_open_local(node->im, "trnleaf:1", "p")) || im__affine(node->im, node->trnim, &node->cumtrn)) {
            return (node);
        }
    }

    return (NULL);
}

static int analyse_mosaic(SymbolTable *st, IMAGE *in) {

    if (im__parse_desc(st, in)) {
        return (-1);
    }

#ifdef DEBUG
    printf("Input files:\n");
    im__map_table(st, print_leaf, NULL, NULL);
    printf("\nOutput file:\n");
    print_node(st->root);
    printf("\nJoin commands:\n");
    print_joins(st->root, 0);
#endif

    if (im__map_table(st, generate_trn_leaves, st, NULL)) {
        return (-1);
    }

    if (im__map_table(st, find_overlaps, st, NULL)) {
        return (-1);
    }

    im__map_table(st, count_leaves, NULL, NULL);
    im__map_table(st, count_joins, NULL, NULL);

    st->leaf = im__map_table(st, choose_leaf, NULL, NULL);

#ifdef DEBUG
    printf("\nLeaf to be 1.000:\n");
    print_node(st->leaf);
    printf("\nOverlaps:\n");
    im__map_table(st, print_overlaps, NULL, NULL);
    printf("\n%d input files, %d unique overlaps, %d joins\n", st->nim, st->novl, st->njoin);
#endif

    return (0);
}

static IMAGE *transform(JoinNode *node, double *gamma) {
    SymbolTable *st = node->st;
    IMAGE *in = node->im;
    double fac = st->fac[node->index];

    IMAGE *out = im_open_local(st->im, node->name, "p");

    IMAGE *t1 = im_open_local(out, "transform:1", "p");
    IMAGE *t2 = im_open_local(out, "transform:2", "p");
    IMAGE *t3 = im_open_local(out, "transform:3", "p");
    IMAGE *t4 = im_open_local(out, "transform:4", "p");
    IMAGE *t5 = im_open_local(out, "transform:5", "p");

    if (!out || !t1 || !t2 || !t3 || !t4 || !t5) {
        return (NULL);
    }

    if (fac == 1.0) {

        out = in;
    } else if (in->BandFmt == IM_BANDFMT_UCHAR) {
        if (im_identity(t1, 1) || im_powtra(t1, t2, 1.0 / (*gamma)) || im_lintra(fac, t2, 0.0, t3) || im_powtra(t3, t4, *gamma) || im_clip2fmt(t4, t5, IM_BANDFMT_UCHAR) || im_maplut(in, out, t5)) {
            return (NULL);
        }
    } else if (in->BandFmt == IM_BANDFMT_USHORT) {
        if (im_identity_ushort(t1, 1, 65535) || im_powtra(t1, t2, 1.0 / (*gamma)) || im_lintra(fac, t2, 0.0, t3) || im_powtra(t3, t4, *gamma) || im_clip2fmt(t4, t5, IM_BANDFMT_USHORT) || im_maplut(in, out, t5)) {
            return (NULL);
        }
    } else {

        if (im_lintra(fac, in, 0.0, t1) || im_clip2fmt(t1, out, in->BandFmt)) {
            return (NULL);
        }
    }

    return (out);
}

static IMAGE *transformf(JoinNode *node, double *gamma) {
    SymbolTable *st = node->st;
    IMAGE *in = node->im;
    double fac = node->st->fac[node->index];

    IMAGE *out = im_open_local(st->im, node->name, "p");
    IMAGE *t1 = im_open_local(out, "transform:1", "p");
    IMAGE *t2 = im_open_local(out, "transform:2", "p");
    IMAGE *t3 = im_open_local(out, "transform:3", "p");
    IMAGE *t4 = im_open_local(out, "transform:4", "p");

    if (!out || !t1 || !t2 || !t3 || !t4) {
        return (NULL);
    }

    if (fac == 1.0) {

        out = in;
    } else if (in->BandFmt == IM_BANDFMT_UCHAR) {
        if (im_identity(t1, 1) || im_powtra(t1, t2, 1 / (*gamma)) || im_lintra(fac, t2, 0.0, t3) || im_powtra(t3, t4, *gamma) || im_maplut(in, out, t4)) {
            return (NULL);
        }
    } else if (in->BandFmt == IM_BANDFMT_USHORT) {
        if (im_identity_ushort(t1, 1, 65535) || im_powtra(t1, t2, 1 / (*gamma)) || im_lintra(fac, t2, 0.0, t3) || im_powtra(t3, t4, *gamma) || im_maplut(in, out, t4)) {
            return (NULL);
        }
    } else {

        if (im_lintra(fac, in, 0.0, out)) {
            return (NULL);
        }
    }

    return (out);
}

int im_global_balance(IMAGE *in, IMAGE *out, double gamma) {
    SymbolTable *st;

    if (!(st = im__build_symtab(out, SYM_TAB_SIZE)) || analyse_mosaic(st, in) || find_factors(st, gamma) || im__build_mosaic(st, out, (transform_fn)transform, &gamma)) {
        return (-1);
    }

    return (0);
}

int im_global_balancef(IMAGE *in, IMAGE *out, double gamma) {
    SymbolTable *st;

    if (!(st = im__build_symtab(out, SYM_TAB_SIZE)) || analyse_mosaic(st, in) || find_factors(st, gamma) || im__build_mosaic(st, out, (transform_fn)transformf, &gamma)) {
        return (-1);
    }

    return (0);
}
