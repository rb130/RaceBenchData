
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <cass.h>
#include <cass_heap.h>
#include "LSH.h"

#define ptb_vec_ge(a, b) ((a)->key >= (b)->key)

ptb_vec_t *gen_score(int M)

{
    int i;
    ptb_vec_t *score = type_calloc(ptb_vec_t, 2 * M);
    assert(sizeof(score->set) >= 8);
    for (i = 0; i < M; i++) {
        score[i].set = 1 << i;
        score[i].key = (i + 1.0) * (i + 2.0) / (M + 1.0) / (M + 2.0) * 0.25;
        score[i].key1 = (i + 1.0) / (M + 1.0) * 0.5;

        score[2 * M - 1 - i].set = 1 << (2 * M - 1 - i);
        score[2 * M - 1 - i].key = 1.0 - 2.0 * score[i].key1 + score[i].key;
        score[2 * M - 1 - i].key1 = 1.0 - score[i].key1;
    }

    return score;
}

int gen_perturb_set(const ptb_vec_t *score, ptb_vec_t *set, int M, int T)

{
    ptb_vec_t ptb;
    ptb_vec_t ptb2;
    ARRAY_TYPE(ptb_vec_t) heap;
    int i, j, m;
    ARRAY_INIT(heap);
    ptb = score[0];
    ptb.max = 0;
    HEAP_ENQUEUE(heap, ptb, ptb_vec_ge);
    i = 0;
    if (T > 0) {
        for (;;) {
            if (heap.len <= 0) {

                assert(0);
            }
            ptb = HEAP_HEAD(heap);
            for (j = 0; j < M; j++) {
                if ((ptb.set & j) && (ptb.set & (2 * M - 1 - j))) {
                    break;
                }
            }
            if (j >= M) {
                set[i] = ptb;
                i++;
                if (i == T) {
                    break;
                }
            }
            HEAP_DEQUEUE(heap, ptb_vec_ge);

            m = ptb.max;
            ptb.max++;
            if (ptb.max >= M * 2) {
                continue;
            }

            ptb2 = ptb;
            ptb2.set &= ~score[m].set;
            ptb2.key -= score[m].key;

            {
                ptb.set |= score[ptb.max].set;
                ptb.key += score[ptb.max].key;
                HEAP_ENQUEUE(heap, ptb, ptb_vec_ge);
            }

            {
                ptb2.set |= score[ptb2.max].set;
                ptb2.key += score[ptb2.max].key;
                HEAP_ENQUEUE(heap, ptb2, ptb_vec_ge);
            }
        }
    }
    if (i < T) {
        assert(0);
    }
    ARRAY_CLEANUP(heap);
    return 0;
}

int map_perturb_vector(const ptb_vec_t set[], ptb_vec_t vector[], const ptb_vec_t mapping[], int M, int T)

{
    ptb_vec_t vec;
    int i, j;
    for (i = 0; i < T; i++) {
        vec.dir = vec.set = 0;
        vec.key = 0;
        for (j = 0; j < 2 * M; j++) {
            if (set[i].set & (1 << j)) {
                assert((vec.set & mapping[j].set) == 0);
                vec.set |= mapping[j].set;
                vec.dir |= mapping[j].dir;
                vec.key += mapping[j].key;
            }
        }
        vector[i] = vec;
    }
    return 0;
}

static inline int ptb_ge(ptb_vec_t a, ptb_vec_t b) {
    return a.key >= b.key;
}

QUICKSORT_GENERATE(ptb, ptb_vec_t)
