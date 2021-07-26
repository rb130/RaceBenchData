
#include <cass.h>

static inline int itopk_ge(itopk_t a, itopk_t b) {
    return a.key >= b.key;
}

static inline int itopk_rev_ge(itopk_t a, itopk_t b) {
    return b.key >= a.key;
}

static inline int ftopk_ge(ftopk_t a, ftopk_t b) {
    return a.key >= b.key;
}

static inline int ftopk_rev_ge(ftopk_t a, ftopk_t b) {
    return b.key >= a.key;
}

QUICKSORT_GENERATE(ftopk, ftopk_t)

QUICKSORT_GENERATE(ftopk_rev, ftopk_t)

QUICKSORT_GENERATE(itopk, itopk_t)

QUICKSORT_GENERATE(itopk_rev, itopk_t)
