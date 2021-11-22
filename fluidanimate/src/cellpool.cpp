

#include <iostream>

#include <stdlib.h>
#include <assert.h>

#include "fluid.hpp"
#include "cellpool.hpp"

#ifndef ENABLE_MALLOC_FALLBACK

static struct datablockhdr *cellpool_allocblock(int cells) {
    struct datablockhdr *block = NULL;
    struct Cell *temp1, *temp2;
    int i;

    assert(cells > 0);
#if defined(WIN32)
    block = (struct datablockhdr *)_aligned_malloc(sizeof(struct datablockhdr) + cells * sizeof(struct Cell), CACHELINE_SIZE);
    assert(block);
#elif defined(SPARC_SOLARIS)
    block = (struct datablockhdr *)memalign(CACHELINE_SIZE, sizeof(struct datablockhdr) + cells * sizeof(struct Cell));
#else
    int rv = posix_memalign((void **)(&block), CACHELINE_SIZE, sizeof(struct datablockhdr) + cells * sizeof(struct Cell));
    assert(!rv);
#endif

    block->next = NULL;
    temp1 = (struct Cell *)(block + 1);
    for (i = 0; i < cells; i++) {

        assert((uint64_t)(temp1) % sizeof(void *) == 0);
        if (i != cells - 1) {
            temp2 = temp1 + 1;
            temp1->next = temp2;
            temp1 = temp2;
        } else {

            temp1->next = NULL;
        }
    }

    return block;
}

void cellpool_init(cellpool *pool, int particles) {
    int ALLOC_MIN_CELLS = 1024;
    assert(sizeof(struct datablockhdr) % CACHELINE_SIZE == 0);
    assert(pool != NULL);
    assert(particles > 0);

    pool->alloc = 4 * (particles / PARTICLES_PER_CELL);
    pool->alloc = pool->alloc < ALLOC_MIN_CELLS ? ALLOC_MIN_CELLS : pool->alloc;
    pool->datablocks = cellpool_allocblock(pool->alloc);
    pool->cells = (struct Cell *)(pool->datablocks + 1);
}

Cell *cellpool_getcell(cellpool *pool) {
    struct Cell *temp;

    assert(pool != NULL);

    if (pool->cells == NULL) {

        struct datablockhdr *block = cellpool_allocblock(pool->alloc);
        pool->alloc = 2 * pool->alloc;
        block->next = pool->datablocks;
        pool->datablocks = block;
        pool->cells = (struct Cell *)(pool->datablocks + 1);
    }

    temp = pool->cells;
    pool->cells = temp->next;
    temp->next = NULL;
    return temp;
}

void cellpool_returncell(cellpool *pool, Cell *cell) {
    assert(pool != NULL);
    assert(cell != NULL);
    cell->next = pool->cells;
    pool->cells = cell;
}

void cellpool_destroy(cellpool *pool) {
    assert(pool != NULL);

    struct datablockhdr *ptr = pool->datablocks;
    struct datablockhdr *temp;
    while (ptr != NULL) {
        temp = ptr;
        ptr = ptr->next;
#if defined(WIN32)
        _aligned_free(temp);
#else
        free(temp);
#endif
    }
}

#else

void cellpool_init(cellpool *pool, int particles) {
    std::cout << "WARNING: Malloc fallback enabled for cell pool." << std::endl;
}

Cell *cellpool_getcell(cellpool *pool) {
    Cell *cell;

    cell = (struct Cell *)malloc(sizeof(struct Cell));
    assert(cell != NULL);
    return cell;
}

void cellpool_returncell(cellpool *pool, Cell *cell) {
    free(cell);
}

void cellpool_destroy(cellpool *pool) {
    return;
}

#endif

#ifdef ENABLE_TESTER

#include <stdio.h>

const float p_vec_const = 0.0;
const float hv_vec_const = 1.0;
const float v_vec_const = 2.0;
const float a_vec_const = 3.0;
const float density_vec_const = 0.1;
const Vec3 coord_const(-1.1, -2.2, -3.3);

void write_cell(struct Cell *cell) {
    int i;

    for (i = 0; i < PARTICLES_PER_CELL; i++) {
        void *addr;

        addr = (void *)&(cell->p[i]);
        cell->p[i] = coord_const + p_vec_const + (uint64_t)addr;
        addr = (void *)&(cell->hv[i]);
        cell->hv[i] = coord_const + hv_vec_const + (uint64_t)addr;
        addr = (void *)&(cell->v[i]);
        cell->v[i] = coord_const + v_vec_const + (uint64_t)addr;
        addr = (void *)&(cell->a[i]);
        cell->a[i] = coord_const + a_vec_const + (uint64_t)addr;
        addr = (void *)&(cell->density[i]);
        cell->density[i] = density_vec_const + (uint64_t)addr;
    }
}

bool check_cell(struct Cell *cell) {
    Vec3 dummy;
    bool rv = true;
    int i;

    for (i = 0; i < PARTICLES_PER_CELL; i++) {
        void *addr;

        addr = (void *)&(cell->p[i]);
        dummy = coord_const + p_vec_const + (uint64_t)addr;
        rv = rv && (dummy == cell->p[i]);
        addr = (void *)&(cell->hv[i]);
        dummy = coord_const + hv_vec_const + (uint64_t)addr;
        rv = rv && (dummy == cell->hv[i]);
        addr = (void *)&(cell->v[i]);
        dummy = coord_const + v_vec_const + (uint64_t)addr;
        rv = rv && (dummy == cell->v[i]);
        addr = (void *)&(cell->a[i]);
        dummy = coord_const + a_vec_const + (uint64_t)addr;
        rv = rv && (dummy == cell->a[i]);
        addr = (void *)&(cell->density[i]);
        float fdummy = density_vec_const + (uint64_t)addr;
        rv = rv && (fdummy == cell->density[i]);
    }

    return rv;
}

int main() {
    int nCells = 2 * 1000 * 1000;
    const int size_array = 389;
    struct Cell cells[size_array];
    cellpool *pool;
    int i;

    printf("Initializing...\n");
    fflush(NULL);

    cellpool_init(&pool, 1);

    for (i = 0; i < size_array; i++) {
        write_cell(&(cells[i]));
        cells[i].next = NULL;
    }

    printf("Testing (1st pass): ");
    fflush(NULL);

    for (i = 0; i < nCells; i++) {
        struct Cell *ptr, *temp;

        temp = cellpool_getcell(pool);
        write_cell(temp);
        ptr = &(cells[i % size_array]);
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = temp;

        if ((i + 1) % (nCells / 10) == 0) {
            printf("...%i%%", (i + 1) / (nCells / 100));
            fflush(NULL);
        }
    }
    printf("\n");
    fflush(NULL);

    printf("Testing (2nd pass): ");
    fflush(NULL);
    int count = 0;
    for (i = 0; i < size_array; i++) {
        struct Cell *ptr;

        ptr = &(cells[i]);
        do {
            if (!check_cell(ptr)) {
                printf("ERROR: Cell contents altered!\n");
                exit(1);
            }

            count++;

            if ((count + 1) % (nCells / 10) == 0) {
                printf("...%i%%", (count + 1) / (nCells / 100));
                fflush(NULL);
            }

            ptr = ptr->next;
        } while (ptr != NULL);
    }
    printf("\n");
    fflush(NULL);

    printf("Cleanup...\n");
    fflush(NULL);
    cellpool_destroy(&pool);
    printf("Terminating...\n");
    fflush(NULL);
    return 0;
}
#endif
