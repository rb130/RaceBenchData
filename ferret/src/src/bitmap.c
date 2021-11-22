

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <cass_bitmap.h>

bitmap_t *bitmap_new(uint32_t x) {
    bitmap_t *ret;

    x += 31;
    x /= 32;
    x *= 32;
    x /= 8;

    ret = (bitmap_t *)malloc(sizeof(bitmap_t));
    ret->count = 0;
    ret->size = 8 * x;
    ret->vec = (uchar *)malloc(x * sizeof(ret->vec[0]));
    memset(ret->vec, 0, x * sizeof(ret->vec[0]));

    return ret;
}

int bitmap_init(bitmap_t *map, uint32_t x) {
    x += 31;
    x /= 32;
    x *= 32;
    x /= 8;

    map->count = 0;
    map->size = 8 * x;
    map->vec = (uchar *)malloc(x * sizeof(map->vec[0]));
    memset(map->vec, 0, x * sizeof(map->vec[0]));

    return 0;
}

int bitmap_clear(bitmap_t *map) {
    uint32_t x = map->size / 8;

    memset(map->vec, 0, x * sizeof(map->vec[0]));
    return 0;
}

int bitmap_free_vec(bitmap_t *map) {
    if (map->vec != NULL) {
        free(map->vec);
    }

    return 0;
}

int bitmap_free(bitmap_t **map) {
    if (*map) {
        free((*map)->vec);
        free(*map);
        *map = NULL;
    }

    return 0;
}

int bitmap_print(bitmap_t *map) {
    uint32_t i;

    printf("cnt = %lu, size = %lu\n", (unsigned long)(map->count), (unsigned long)(map->size));

    for (i = 0; i < map->size; i++) {
        if (bitisset(map->vec, i)) {
            printf("%lu ", (unsigned long)i);
        }
    }
    printf("\n");

    return 0;
}

uint32_t bitmap_get_count(bitmap_t *map) {
    return map->count;
}

uint32_t bitmap_get_size(bitmap_t *map) {
    return map->size;
}

int bitmap_contain(bitmap_t *map, uint32_t item) {
    if ((item < map->size) && bitisset(map->vec, item)) {
        return 1;
    }

    return 0;
}

int bitmap_insert(bitmap_t *map, uint32_t item) {
    assert(item < map->size);

    if (bitisset(map->vec, item) == 0) {
        map->count++;
    }

    bitset(map->vec, item);

    return 0;
}

int bitmap_union(bitmap_t *mapA, bitmap_t *mapB) {
    uint32_t i, j, size, setsize;

    assert(mapA->size == mapB->size);
    assert((mapB->size & 0x7) == 0);

    setsize = 0;
    size = bitsize(mapB->size);
    for (i = 0; i < size; i++) {
        mapA->vec[i] |= mapB->vec[i];
        for (j = 0; j < 8; j++) {
            if (mapA->vec[i] & (1 << j)) {
                setsize++;
            }
        }
    }

    mapA->count = setsize;

    return 0;
}

uint32_t bitmap_getNext(bitmap_t *map, uint32_t cur) {

    if (cur < map->size) {
        cur++;
    } else {
        cur = 0;
    }

    while ((cur < map->size) && !bitisset(map->vec, cur)) {
        cur++;
    }

    return cur;
}
