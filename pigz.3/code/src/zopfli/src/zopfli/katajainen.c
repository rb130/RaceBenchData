

#include "katajainen.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

typedef struct Node Node;

struct Node {
    size_t weight;
    Node *tail;
    int count;
};

typedef struct NodePool {
    Node *next;
} NodePool;

static void InitNode(size_t weight, int count, Node *tail, Node *node) {
    node->weight = weight;
    node->count = count;
    node->tail = tail;
}

static void BoundaryPM(Node *(*lists)[2], Node *leaves, int numsymbols, NodePool *pool, int index) {
    Node *newchain;
    Node *oldchain;
    int lastcount = lists[index][1]->count;

    if (index == 0 && lastcount >= numsymbols) {
        return;
    }

    newchain = pool->next++;
    oldchain = lists[index][1];

    lists[index][0] = oldchain;
    lists[index][1] = newchain;

    if (index == 0) {

        InitNode(leaves[lastcount].weight, lastcount + 1, 0, newchain);
    } else {
        size_t sum = lists[index - 1][0]->weight + lists[index - 1][1]->weight;
        if (lastcount < numsymbols && sum > leaves[lastcount].weight) {

            InitNode(leaves[lastcount].weight, lastcount + 1, oldchain->tail, newchain);
        } else {
            InitNode(sum, lastcount, lists[index - 1][1], newchain);

            BoundaryPM(lists, leaves, numsymbols, pool, index - 1);
            BoundaryPM(lists, leaves, numsymbols, pool, index - 1);
        }
    }
}

static void BoundaryPMFinal(Node *(*lists)[2], Node *leaves, int numsymbols, NodePool *pool, int index) {
    int lastcount = lists[index][1]->count;

    size_t sum = lists[index - 1][0]->weight + lists[index - 1][1]->weight;

    if (lastcount < numsymbols && sum > leaves[lastcount].weight) {
        Node *newchain = pool->next;
        Node *oldchain = lists[index][1]->tail;

        lists[index][1] = newchain;
        newchain->count = lastcount + 1;
        newchain->tail = oldchain;
    } else {
        lists[index][1]->tail = lists[index - 1][1];
    }
}

static void InitLists(NodePool *pool, const Node *leaves, int maxbits, Node *(*lists)[2]) {
    int i;
    Node *node0 = pool->next++;
    Node *node1 = pool->next++;
    InitNode(leaves[0].weight, 1, 0, node0);
    InitNode(leaves[1].weight, 2, 0, node1);
    for (i = 0; i < maxbits; i++) {
        lists[i][0] = node0;
        lists[i][1] = node1;
    }
}

static void ExtractBitLengths(Node *chain, Node *leaves, unsigned *bitlengths) {
    int counts[16] = {0};
    unsigned end = 16;
    unsigned ptr = 15;
    unsigned value = 1;
    Node *node;
    int val;

    for (node = chain; node; node = node->tail) {
        counts[--end] = node->count;
    }

    val = counts[15];
    while (ptr >= end) {
        for (; val > counts[ptr - 1]; val--) {
            bitlengths[leaves[val - 1].count] = value;
        }
        ptr--;
        value++;
    }
}

static int LeafComparator(const void *a, const void *b) {
    return ((const Node *)a)->weight - ((const Node *)b)->weight;
}

int ZopfliLengthLimitedCodeLengths(const size_t *frequencies, int n, int maxbits, unsigned *bitlengths) {
    NodePool pool;
    int i;
    int numsymbols = 0;
    int numBoundaryPMRuns;
    Node *nodes;

    Node *(*lists)[2];

    Node *leaves = (Node *)malloc(n * sizeof(*leaves));

    for (i = 0; i < n; i++) {
        bitlengths[i] = 0;
    }

    for (i = 0; i < n; i++) {
        if (frequencies[i]) {
            leaves[numsymbols].weight = frequencies[i];
            leaves[numsymbols].count = i;
            numsymbols++;
        }
    }

    if ((1 << maxbits) < numsymbols) {
        free(leaves);
        return 1;
    }
    if (numsymbols == 0) {
        free(leaves);
        return 0;
    }
    if (numsymbols == 1) {
        bitlengths[leaves[0].count] = 1;
        free(leaves);
        return 0;
    }
    if (numsymbols == 2) {
        bitlengths[leaves[0].count]++;
        bitlengths[leaves[1].count]++;
        free(leaves);
        return 0;
    }

    for (i = 0; i < numsymbols; i++) {
        if (leaves[i].weight >= ((size_t)1 << (sizeof(leaves[0].weight) * CHAR_BIT - 9))) {
            free(leaves);
            return 1;
        }
        leaves[i].weight = (leaves[i].weight << 9) | leaves[i].count;
    }
    qsort(leaves, numsymbols, sizeof(Node), LeafComparator);
    for (i = 0; i < numsymbols; i++) {
        leaves[i].weight >>= 9;
    }

    if (numsymbols - 1 < maxbits) {
        maxbits = numsymbols - 1;
    }

    nodes = (Node *)malloc(maxbits * 2 * numsymbols * sizeof(Node));
    pool.next = nodes;

    lists = (Node * (*)[2]) malloc(maxbits * sizeof(*lists));
    InitLists(&pool, leaves, maxbits, lists);

    numBoundaryPMRuns = 2 * numsymbols - 4;
    for (i = 0; i < numBoundaryPMRuns - 1; i++) {
        BoundaryPM(lists, leaves, numsymbols, &pool, maxbits - 1);
    }
    BoundaryPMFinal(lists, leaves, numsymbols, &pool, maxbits - 1);

    ExtractBitLengths(lists[maxbits - 1][1], leaves, bitlengths);

    free(lists);
    free(leaves);
    free(nodes);
    return 0;
}
