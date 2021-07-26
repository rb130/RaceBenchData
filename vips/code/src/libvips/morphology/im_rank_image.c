
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct Rank {
    IMAGE **in;
    IMAGE *out;
    int n;
    int index;
} Rank;

static Rank *rank_new(IMAGE **in, IMAGE *out, int n, int index) {
    int i;
    Rank *rank;

    if (!(rank = IM_NEW(out, Rank))) {
        return (NULL);
    }

    rank->n = n;
    rank->index = index;
    rank->out = out;
    if (!(rank->in = IM_ARRAY(out, n + 1, IMAGE *))) {
        return (NULL);
    }
    for (i = 0; i < n; i++) {
        rank->in[i] = in[i];
    }
    rank->in[n] = NULL;

    return (rank);
}

typedef struct {
    Rank *rank;

    REGION **ir;
    PEL **pts;
    PEL *sort;
} RankSequence;

static int rank_stop(void *vseq, void *a, void *b) {
    RankSequence *seq = (RankSequence *)vseq;
    Rank *rank = (Rank *)b;
    int i;

    for (i = 0; i < rank->n; i++) {
        IM_FREEF(im_region_free, seq->ir[i]);
    }

    return (0);
}

static void *rank_start(IMAGE *out, void *a, void *b) {
    IMAGE **in = (IMAGE **)a;
    Rank *rank = (Rank *)b;
    RankSequence *seq;
    int i;

    if (!(seq = IM_NEW(out, RankSequence))) {
        return (NULL);
    }

    seq->rank = rank;
    seq->ir = NULL;
    seq->pts = NULL;

    seq->ir = IM_ARRAY(out, rank->n + 1, REGION *);
    seq->pts = IM_ARRAY(out, rank->n + 1, PEL *);
    seq->sort = IM_ARRAY(out, rank->n * IM_IMAGE_SIZEOF_ELEMENT(in[0]), PEL);
    if (!seq->ir || !seq->pts || !seq->sort) {
        rank_stop(seq, in, rank);
        return (NULL);
    }

    for (i = 0; i < rank->n; i++) {
        if (!(seq->ir[i] = im_region_create(in[i]))) {
            rank_stop(seq, in, rank);
            return (NULL);
        }
    }
    seq->ir[i] = NULL;

    return ((void *)seq);
}

#define FIND_IM_MAX(TYPE)                          \
    {                                              \
        for (x = 0; x < sz; x++) {                 \
            TYPE top = ((TYPE *)seq->pts[0])[x];   \
                                                   \
            for (i = 1; i < rank->n; i++) {        \
                TYPE v = ((TYPE *)seq->pts[i])[x]; \
                                                   \
                if (v > top)                       \
                    top = v;                       \
            }                                      \
                                                   \
            ((TYPE *)q)[x] = top;                  \
        }                                          \
    }

#define FIND_IM_MIN(TYPE)                          \
    {                                              \
        for (x = 0; x < sz; x++) {                 \
            TYPE bot = ((TYPE *)seq->pts[0])[x];   \
                                                   \
            for (i = 1; i < rank->n; i++) {        \
                TYPE v = ((TYPE *)seq->pts[i])[x]; \
                                                   \
                if (v < bot)                       \
                    bot = v;                       \
            }                                      \
                                                   \
            ((TYPE *)q)[x] = bot;                  \
        }                                          \
    }

#define FIND_IM_RANK(TYPE)                         \
    {                                              \
        TYPE *sort = (TYPE *)seq->sort;            \
                                                   \
        for (x = 0; x < sz; x++) {                 \
            for (i = 0; i < rank->n; i++) {        \
                TYPE v = ((TYPE *)seq->pts[i])[x]; \
                                                   \
                for (j = 0; j < i; j++)            \
                    if (sort[j] > v)               \
                        break;                     \
                                                   \
                for (k = i; k > j; k--)            \
                    sort[k] = sort[k - 1];         \
                                                   \
                sort[j] = v;                       \
            }                                      \
                                                   \
            ((TYPE *)q)[x] = sort[rank->index];    \
        }                                          \
    }

#define SWITCH(OPERATION)          \
    switch (rank->out->BandFmt) {  \
    case IM_BANDFMT_UCHAR:         \
        OPERATION(unsigned char);  \
        break;                     \
    case IM_BANDFMT_CHAR:          \
        OPERATION(signed char);    \
        break;                     \
    case IM_BANDFMT_USHORT:        \
        OPERATION(unsigned short); \
        break;                     \
    case IM_BANDFMT_SHORT:         \
        OPERATION(signed short);   \
        break;                     \
    case IM_BANDFMT_UINT:          \
        OPERATION(unsigned int);   \
        break;                     \
    case IM_BANDFMT_INT:           \
        OPERATION(signed int);     \
        break;                     \
    case IM_BANDFMT_FLOAT:         \
        OPERATION(float);          \
        break;                     \
    case IM_BANDFMT_DOUBLE:        \
        OPERATION(double);         \
        break;                     \
                                   \
    default:                       \
        assert(0);                 \
    }

static int rank_gen(REGION * or, void *vseq, void *a, void *b) {
    RankSequence *seq = (RankSequence *)vseq;
    Rank *rank = (Rank *)b;
    Rect *r = & or->valid;
    int le = r->left;
    int to = r->top;
    int bo = IM_RECT_BOTTOM(r);
    int sz = IM_REGION_N_ELEMENTS(or);

    int x, y, i, j, k;

    for (i = 0; i < rank->n; i++) {
        if (im_prepare(seq->ir[i], r)) {
            return (-1);
        }
    }

    for (y = to; y < bo; y++) {
        PEL *q = (PEL *)IM_REGION_ADDR(or, le, y);

        for (i = 0; i < rank->n; i++) {
            seq->pts[i] = (PEL *)IM_REGION_ADDR(seq->ir[i], le, y);
        }

        if (rank->index == 0) {
            SWITCH(FIND_IM_MIN)
        } else if (rank->index == rank->n - 1) {
            SWITCH(FIND_IM_MAX)
        } else {
            SWITCH(FIND_IM_RANK)
        }
    }

    return (0);
}

int im_rank_image(IMAGE **in, IMAGE *out, int n, int index) {
    int i;
    Rank *rank;

    if (n < 1) {
        im_error("im_rank_image", "%s", _("zero input images!"));
        return (-1);
    }
    if (index < 0 || index > n - 1) {
        im_error("im_rank_image", _("index should be in range 0 - %d"), n - 1);
        return (-1);
    }
    if (im_poutcheck(out)) {
        return (-1);
    }
    for (i = 0; i < n; i++) {
        if (im_pincheck(in[i])) {
            return (-1);
        }

        if (in[i]->Coding != IM_CODING_NONE || vips_bandfmt_iscomplex(in[i]->BandFmt)) {
            im_error("im_rank_image", "%s", _("uncoded non-complex only"));
            return (-1);
        }

        if (in[0]->BandFmt != in[i]->BandFmt) {
            im_error("im_rank_image", "%s", _("input images differ in format"));
            return (-1);
        }
        if (in[0]->Xsize != in[i]->Xsize || in[0]->Ysize != in[i]->Ysize) {
            im_error("im_rank_image", "%s", _("input images differ in size"));
            return (-1);
        }
        if (in[0]->Bands != in[i]->Bands) {
            im_error("im_rank_image", "%s", _("input images differ in number of bands"));
            return (-1);
        }
    }

    if (!(rank = rank_new(in, out, n, index)) || im_cp_desc_array(out, rank->in) || im_demand_hint_array(out, IM_THINSTRIP, rank->in) || im_generate(out, rank_start, rank_gen, rank_stop, rank->in, rank)) {
        return (-1);
    }

    return (0);
}

int im_maxvalue(IMAGE **in, IMAGE *out, int n) {
    return (im_rank_image(in, out, n, n - 1));
}
