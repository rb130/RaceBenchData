#include "racebench_bugs.h"


#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#endif
#include <math.h>
#include <pthread.h>
#include <assert.h>
#include <float.h>

#include "fluid.hpp"
#include "cellpool.hpp"
#include "parsec_barrier.hpp"

#ifdef ENABLE_VISUALIZATION
#include "fluidview.hpp"
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

cellpool *pools;

fptype restParticlesPerMeter, h, hSq;
fptype densityCoeff, pressureCoeff, viscosityCoeff;

int nx, ny, nz;
Vec3 delta;
int numParticles = 0;
int numCells = 0;
Cell *cells = 0;
Cell *cells2 = 0;
int *cnumPars = 0;
int *cnumPars2 = 0;
Cell **last_cells = NULL;
#ifdef ENABLE_VISUALIZATION
Vec3 vMax(0.0, 0.0, 0.0);
Vec3 vMin(0.0, 0.0, 0.0);
#endif

int XDIVS = 1;
int ZDIVS = 1;

#define NUM_GRIDS ((XDIVS) * (ZDIVS))
#define MUTEXES_PER_CELL 128
#define CELL_MUTEX_ID 0

struct Grid {
    union {
        struct {
            int sx, sy, sz;
            int ex, ey, ez;
        };
        unsigned char pp[CACHELINE_SIZE];
    };
} * grids;
bool *border;
pthread_attr_t attr;
pthread_t *thread;
pthread_mutex_t **mutex;
pthread_barrier_t barrier;
#ifdef ENABLE_VISUALIZATION
pthread_barrier_t visualization_barrier;
#endif

typedef struct __thread_args {
    int tid;
    int frames;
} thread_args;

unsigned int hmgweight(unsigned int x, int *lsb) {
    unsigned int weight = 0;
    unsigned int mask = 1;
    unsigned int count = 0;

    *lsb = -1;
    while (x > 0) {
        unsigned int temp;
        temp = (x & mask);
        if ((x & mask) == 1) {
            weight++;
            if (*lsb == -1) {
                *lsb = count;
            }
        }
        x >>= 1;
        count++;
    }

    return weight;
}

void InitSim(char const *fileName, unsigned int threadnum) {

    int lsb;
    if (hmgweight(threadnum, &lsb) != 1) {
        std::cerr << "Number of threads must be a power of 2" << std::endl;
        exit(1);
    }
    XDIVS = 1 << (lsb / 2);
    ZDIVS = 1 << (lsb / 2);
    if (XDIVS * ZDIVS != threadnum) {
        XDIVS *= 2;
    }
    assert(XDIVS * ZDIVS == threadnum);

    thread = new pthread_t[NUM_GRIDS];
    grids = new struct Grid[NUM_GRIDS];
    assert(sizeof(Grid) <= CACHELINE_SIZE);

    pools = new cellpool[NUM_GRIDS];

    std::cout << "Loading file \"" << fileName << "\"..." << std::endl;
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file. Aborting." << std::endl;
        exit(1);
    }

    float restParticlesPerMeter_le;
    int numParticles_le;
    file.read((char *)&restParticlesPerMeter_le, FILE_SIZE_FLOAT);
    file.read((char *)&numParticles_le, FILE_SIZE_INT);
    if (!isLittleEndian()) {
        restParticlesPerMeter = bswap_float(restParticlesPerMeter_le);
        numParticles = bswap_int32(numParticles_le);
    } else {
        restParticlesPerMeter = restParticlesPerMeter_le;
        numParticles = numParticles_le;
    }
    for (int i = 0; i < NUM_GRIDS; i++) {
        cellpool_init(&pools[i], numParticles / NUM_GRIDS);
    }

    h = kernelRadiusMultiplier / restParticlesPerMeter;
    hSq = h * h;

#ifndef ENABLE_DOUBLE_PRECISION
    fptype coeff1 = 315.0 / (64.0 * pi * powf(h, 9.0));
    fptype coeff2 = 15.0 / (pi * powf(h, 6.0));
    fptype coeff3 = 45.0 / (pi * powf(h, 6.0));
#else
    fptype coeff1 = 315.0 / (64.0 * pi * pow(h, 9.0));
    fptype coeff2 = 15.0 / (pi * pow(h, 6.0));
    fptype coeff3 = 45.0 / (pi * pow(h, 6.0));
#endif
    fptype particleMass = 0.5 * doubleRestDensity / (restParticlesPerMeter * restParticlesPerMeter * restParticlesPerMeter);
    densityCoeff = particleMass * coeff1;
    pressureCoeff = 3.0 * coeff2 * 0.50 * stiffnessPressure * particleMass;
    viscosityCoeff = viscosity * coeff3 * particleMass;

    Vec3 range = domainMax - domainMin;
    nx = (int)(range.x / h);
    ny = (int)(range.y / h);
    nz = (int)(range.z / h);
    assert(nx >= 1 && ny >= 1 && nz >= 1);
    numCells = nx * ny * nz;
    std::cout << "Number of cells: " << numCells << std::endl;
    delta.x = range.x / nx;
    delta.y = range.y / ny;
    delta.z = range.z / nz;
    assert(delta.x >= h && delta.y >= h && delta.z >= h);

    std::cout << "Grids steps over x, y, z: " << delta.x << " " << delta.y << " " << delta.z << std::endl;

    assert(nx >= XDIVS && nz >= ZDIVS);
    int gi = 0;
    int sx, sz, ex, ez;
    ex = 0;
    for (int i = 0; i < XDIVS; ++i) {
        sx = ex;
        ex = (int)((fptype)(nx) / (fptype)(XDIVS) * (i + 1) + 0.5);
        assert(sx < ex);

        ez = 0;
        for (int j = 0; j < ZDIVS; ++j, ++gi) {
            sz = ez;
            ez = (int)((fptype)(nz) / (fptype)(ZDIVS) * (j + 1) + 0.5);
            assert(sz < ez);

            grids[gi].sx = sx;
            grids[gi].ex = ex;
            grids[gi].sy = 0;
            grids[gi].ey = ny;
            grids[gi].sz = sz;
            grids[gi].ez = ez;
        }
    }
    assert(gi == NUM_GRIDS);

    border = new bool[numCells];
    for (int i = 0; i < NUM_GRIDS; ++i) {
        for (int iz = grids[i].sz; iz < grids[i].ez; ++iz) {
            for (int iy = grids[i].sy; iy < grids[i].ey; ++iy) {
                for (int ix = grids[i].sx; ix < grids[i].ex; ++ix) {
                    int index = (iz * ny + iy) * nx + ix;
                    border[index] = false;
                    for (int dk = -1; dk <= 1; ++dk) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            for (int di = -1; di <= 1; ++di) {
                                int ci = ix + di;
                                int cj = iy + dj;
                                int ck = iz + dk;

                                if (ci < 0) {
                                    ci = 0;
                                } else if (ci > (nx - 1)) {
                                    ci = nx - 1;
                                }
                                if (cj < 0) {
                                    cj = 0;
                                } else if (cj > (ny - 1)) {
                                    cj = ny - 1;
                                }
                                if (ck < 0) {
                                    ck = 0;
                                } else if (ck > (nz - 1)) {
                                    ck = nz - 1;
                                }

                                if (ci < grids[i].sx || ci >= grids[i].ex || cj < grids[i].sy || cj >= grids[i].ey || ck < grids[i].sz || ck >= grids[i].ez) {

                                    border[index] = true;
                                    break;
                                }
                            }
                            if (border[index]) {
                                break;
                            }
                        }
                        if (border[index]) {
                            break;
                        }
                    }
                }
            }
        }
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    mutex = new pthread_mutex_t *[numCells];
    for (int i = 0; i < numCells; ++i) {
        assert(CELL_MUTEX_ID < MUTEXES_PER_CELL);
        int n = (border[i] ? MUTEXES_PER_CELL : CELL_MUTEX_ID + 1);
        mutex[i] = new pthread_mutex_t[n];
        for (int j = 0; j < n; ++j) {
            pthread_mutex_init(&mutex[i][j], NULL);
        }
    }
    pthread_barrier_init(&barrier, NULL, NUM_GRIDS);
#ifdef ENABLE_VISUALIZATION

    pthread_barrier_init(&visualization_barrier, NULL, NUM_GRIDS + 1);
#endif

    assert(sizeof(Cell) % CACHELINE_SIZE == 0);

    assert(offsetof(struct Cell_aux, padding) == offsetof(struct Cell, padding));

#if defined(WIN32)
    cells = (struct Cell *)_aligned_malloc(sizeof(struct Cell) * numCells, CACHELINE_SIZE);
    cells2 = (struct Cell *)_aligned_malloc(sizeof(struct Cell) * numCells, CACHELINE_SIZE);
    cnumPars = (int *)_aligned_malloc(sizeof(int) * numCells, CACHELINE_SIZE);
    cnumPars2 = (int *)_aligned_malloc(sizeof(int) * numCells, CACHELINE_SIZE);
    last_cells = (struct Cell **)_aligned_malloc(sizeof(struct Cell *) * numCells, CACHELINE_SIZE);
    assert((cells != NULL) && (cells2 != NULL) && (cnumPars != NULL) && (cnumPars2 != NULL) && (last_cells != NULL));
#elif defined(SPARC_SOLARIS)
    cells = (Cell *)memalign(CACHELINE_SIZE, sizeof(struct Cell) * numCells);
    cells2 = (Cell *)memalign(CACHELINE_SIZE, sizeof(struct Cell) * numCells);
    cnumPars = (int *)memalign(CACHELINE_SIZE, sizeof(int) * numCells);
    cnumPars2 = (int *)memalign(CACHELINE_SIZE, sizeof(int) * numCells);
    last_cells = (Cell **)memalign(CACHELINE_SIZE, sizeof(struct Cell *) * numCells);
    assert((cells != 0) && (cells2 != 0) && (cnumPars != 0) && (cnumPars2 != 0) && (last_cells != 0));
#else
    int rv0 = posix_memalign((void **)(&cells), CACHELINE_SIZE, sizeof(struct Cell) * numCells);
    int rv1 = posix_memalign((void **)(&cells2), CACHELINE_SIZE, sizeof(struct Cell) * numCells);
    int rv2 = posix_memalign((void **)(&cnumPars), CACHELINE_SIZE, sizeof(int) * numCells);
    int rv3 = posix_memalign((void **)(&cnumPars2), CACHELINE_SIZE, sizeof(int) * numCells);
    int rv4 = posix_memalign((void **)(&last_cells), CACHELINE_SIZE, sizeof(struct Cell *) * numCells);
    assert((rv0 == 0) && (rv1 == 0) && (rv2 == 0) && (rv3 == 0) && (rv4 == 0));
#endif

    for (int i = 0; i < numCells; ++i) {
        new (&cells[i]) Cell;
        new (&cells2[i]) Cell;
    }

    memset(cnumPars, 0, numCells * sizeof(int));

    int pool_id = 0;
    float px, py, pz, hvx, hvy, hvz, vx, vy, vz;
    for (int i = 0; i < numParticles; ++i) {
        file.read((char *)&px, FILE_SIZE_FLOAT);
        file.read((char *)&py, FILE_SIZE_FLOAT);
        file.read((char *)&pz, FILE_SIZE_FLOAT);
        file.read((char *)&hvx, FILE_SIZE_FLOAT);
        file.read((char *)&hvy, FILE_SIZE_FLOAT);
        file.read((char *)&hvz, FILE_SIZE_FLOAT);
        file.read((char *)&vx, FILE_SIZE_FLOAT);
        file.read((char *)&vy, FILE_SIZE_FLOAT);
        file.read((char *)&vz, FILE_SIZE_FLOAT);
        if (!isLittleEndian()) {
            px = bswap_float(px);
            py = bswap_float(py);
            pz = bswap_float(pz);
            hvx = bswap_float(hvx);
            hvy = bswap_float(hvy);
            hvz = bswap_float(hvz);
            vx = bswap_float(vx);
            vy = bswap_float(vy);
            vz = bswap_float(vz);
        }

        int ci = (int)((px - domainMin.x) / delta.x);
        int cj = (int)((py - domainMin.y) / delta.y);
        int ck = (int)((pz - domainMin.z) / delta.z);

        if (ci < 0) {
            ci = 0;
        } else if (ci > (nx - 1)) {
            ci = nx - 1;
        }
        if (cj < 0) {
            cj = 0;
        } else if (cj > (ny - 1)) {
            cj = ny - 1;
        }
        if (ck < 0) {
            ck = 0;
        } else if (ck > (nz - 1)) {
            ck = nz - 1;
        }

        int index = (ck * ny + cj) * nx + ci;
        Cell *cell = &cells[index];

        int np = cnumPars[index];
        while (np > PARTICLES_PER_CELL) {
            cell = cell->next;
            np = np - PARTICLES_PER_CELL;
        }

        if ((np % PARTICLES_PER_CELL == 0) && (cnumPars[index] != 0)) {

            cell->next = cellpool_getcell(&pools[pool_id]);
            pool_id = (pool_id + 1) % NUM_GRIDS;
            cell = cell->next;
            np = np - PARTICLES_PER_CELL;
        }

        cell->p[np].x = px;
        cell->p[np].y = py;
        cell->p[np].z = pz;
        cell->hv[np].x = hvx;
        cell->hv[np].y = hvy;
        cell->hv[np].z = hvz;
        cell->v[np].x = vx;
        cell->v[np].y = vy;
        cell->v[np].z = vz;
#ifdef ENABLE_VISUALIZATION
        vMin.x = std::min(vMin.x, cell->v[np].x);
        vMax.x = std::max(vMax.x, cell->v[np].x);
        vMin.y = std::min(vMin.y, cell->v[np].y);
        vMax.y = std::max(vMax.y, cell->v[np].y);
        vMin.z = std::min(vMin.z, cell->v[np].z);
        vMax.z = std::max(vMax.z, cell->v[np].z);
#endif
        ++cnumPars[index];
    }

    std::cout << "Number of particles: " << numParticles << std::endl;
}

void SaveFile(char const *fileName) {
    std::cout << "Saving file \"" << fileName << "\"..." << std::endl;

    std::ofstream file(fileName, std::ios::binary);
    assert(file);

    if (!isLittleEndian()) {
        float restParticlesPerMeter_le;
        int numParticles_le;

        restParticlesPerMeter_le = bswap_float((float)restParticlesPerMeter);
        numParticles_le = bswap_int32(numParticles);
        file.write((char *)&restParticlesPerMeter_le, FILE_SIZE_FLOAT);
        file.write((char *)&numParticles_le, FILE_SIZE_INT);
    } else {
        file.write((char *)&restParticlesPerMeter, FILE_SIZE_FLOAT);
        file.write((char *)&numParticles, FILE_SIZE_INT);
    }

    int count = 0;
    for (int i = 0; i < numCells; ++i) {
        Cell *cell = &cells[i];
        int np = cnumPars[i];
        for (int j = 0; j < np; ++j) {

            float px, py, pz, hvx, hvy, hvz, vx, vy, vz;
            if (!isLittleEndian()) {
                px = bswap_float((float)(cell->p[j % PARTICLES_PER_CELL].x));
                py = bswap_float((float)(cell->p[j % PARTICLES_PER_CELL].y));
                pz = bswap_float((float)(cell->p[j % PARTICLES_PER_CELL].z));
                hvx = bswap_float((float)(cell->hv[j % PARTICLES_PER_CELL].x));
                hvy = bswap_float((float)(cell->hv[j % PARTICLES_PER_CELL].y));
                hvz = bswap_float((float)(cell->hv[j % PARTICLES_PER_CELL].z));
                vx = bswap_float((float)(cell->v[j % PARTICLES_PER_CELL].x));
                vy = bswap_float((float)(cell->v[j % PARTICLES_PER_CELL].y));
                vz = bswap_float((float)(cell->v[j % PARTICLES_PER_CELL].z));
            } else {
                px = (float)(cell->p[j % PARTICLES_PER_CELL].x);
                py = (float)(cell->p[j % PARTICLES_PER_CELL].y);
                pz = (float)(cell->p[j % PARTICLES_PER_CELL].z);
                hvx = (float)(cell->hv[j % PARTICLES_PER_CELL].x);
                hvy = (float)(cell->hv[j % PARTICLES_PER_CELL].y);
                hvz = (float)(cell->hv[j % PARTICLES_PER_CELL].z);
                vx = (float)(cell->v[j % PARTICLES_PER_CELL].x);
                vy = (float)(cell->v[j % PARTICLES_PER_CELL].y);
                vz = (float)(cell->v[j % PARTICLES_PER_CELL].z);
            }
            file.write((char *)&px, FILE_SIZE_FLOAT);
            file.write((char *)&py, FILE_SIZE_FLOAT);
            file.write((char *)&pz, FILE_SIZE_FLOAT);
            file.write((char *)&hvx, FILE_SIZE_FLOAT);
            file.write((char *)&hvy, FILE_SIZE_FLOAT);
            file.write((char *)&hvz, FILE_SIZE_FLOAT);
            file.write((char *)&vx, FILE_SIZE_FLOAT);
            file.write((char *)&vy, FILE_SIZE_FLOAT);
            file.write((char *)&vz, FILE_SIZE_FLOAT);
            ++count;

            if (j % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                cell = cell->next;
            }
        }
    }
    assert(count == numParticles);
}

void CleanUpSim() {

    for (int i = 0; i < numCells; ++i) {
        Cell &cell = cells[i];
        while (cell.next) {
            Cell *temp = cell.next;
            cell.next = temp->next;
            cellpool_returncell(&pools[0], temp);
        }
    }

    for (int i = 0; i < NUM_GRIDS; i++) {
        cellpool_destroy(&pools[i]);
    }
    pthread_attr_destroy(&attr);

    for (int i = 0; i < numCells; ++i) {
        assert(CELL_MUTEX_ID < MUTEXES_PER_CELL);
        int n = (border[i] ? MUTEXES_PER_CELL : CELL_MUTEX_ID + 1);
        for (int j = 0; j < n; ++j) {
            pthread_mutex_destroy(&mutex[i][j]);
        }
        delete[] mutex[i];
    }
    delete[] mutex;
    pthread_barrier_destroy(&barrier);
#ifdef ENABLE_VISUALIZATION
    pthread_barrier_destroy(&visualization_barrier);
#endif

    delete[] border;

#if defined(WIN32)
    _aligned_free(cells);
    _aligned_free(cells2);
    _aligned_free(cnumPars);
    _aligned_free(cnumPars2);
    _aligned_free(last_cells);
#else
    free(cells);
    free(cells2);
    free(cnumPars);
    free(cnumPars2);
    free(last_cells);
#endif
    delete[] thread;
    delete[] grids;
}

void ClearParticlesMT(int tid) {
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) + (0x571371c3);
                if ((rb_state4.var_0) == (0xe43a71c9)) {
                    rb_state4.var_12 = 0x309eded9;
                    rb_state4.var_13 = 0xfc9ff226;
                    rb_state4.var_14 = (rb_state4.var_14) ^ (((0x7eb615ae) + (rb_state4.var_8)) ^ (rb_state4.var_9));
                    rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_12) : (0x2b4c79f1));
                    rb_state4.var_16 = (rb_state4.var_16) - (rb_state4.var_14);
                    rb_state4.var_17 = (rb_state4.var_17) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0x2f949d9f));
                    rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_17);
                }
                #endif
                int index = (iz * ny + iy) * nx + ix;
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) ^ (156146 < rb_input_size ? (uint32_t)rb_input[156146] : 0x85217ad4);
                #endif
                cnumPars[index] = 0;
                #ifdef RACEBENCH_BUG_4
                if ((rb_state4.var_0) == (0x8d270006)) {
                    pthread_mutex_lock(&(rb_state4.lock_18));
                    rb_state4.var_3 = 0xc96e98bf;
                    rb_state4.var_4 = (rb_state4.var_4) - (rb_state4.var_1);
                    rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_3);
                    rb_state4.var_6 = (rb_state4.var_6) - (((((0x83d68a6b) + (rb_state4.var_4)) ^ (rb_state4.var_3)) ^ (rb_state4.var_2)) - (rb_state4.var_4));
                    rb_state4.var_7 = (rb_state4.var_7) + (((0x9cbdf92a) ^ (0x1a40e4f3)) - (rb_state4.var_5));
                    rb_state4.var_8 = (rb_state4.var_8) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0xcb80dc84));
                    rb_state4.var_9 = (rb_state4.var_9) + (rb_state4.var_7);
                    rb_state4.var_10 = (rb_state4.var_10) - (((0xa922efb3) ^ (rb_state4.var_6)) + (rb_state4.var_8));
                    rb_state4.var_11 = (rb_state4.var_11) ^ (((((0x376a9d49) - (rb_state4.var_7)) + (rb_state4.var_10)) - (0x6820daca)) + (rb_state4.var_9));
                    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_11);
                    rb_state4.var_1 = rb_state4.var_2;
                    pthread_mutex_unlock(&(rb_state4.lock_18));
                }
                if ((rb_state4.var_0) == (0xe43a71c9)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                        racebench_trigger(4);
                    }
                }
                #endif
                cells[index].next = NULL;
                last_cells[index] = &cells[index];
            }
        }
    }
}

void RebuildGridMT(int tid) {

    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                #ifdef RACEBENCH_BUG_19
                if ((rb_state19.var_0) == (0x471a2b5a)) {
                    pthread_mutex_lock(&(rb_state19.lock_32));
                    rb_state19.var_3 = 0xbc003880;
                    rb_state19.var_4 = (rb_state19.var_4) ^ ((0xf2e319e7) ^ (0x18662b0e));
                    rb_state19.var_5 = (rb_state19.var_5) + (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0x6effe9bd));
                    rb_state19.var_6 = (rb_state19.var_6) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_3) : (0x9365f280));
                    rb_state19.var_7 = (rb_state19.var_7) - (rb_state19.var_4);
                    rb_state19.var_8 = (rb_state19.var_8) ^ (rb_state19.var_5);
                    rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_6) : (0xe09f1a00));
                    rb_state19.var_10 = (rb_state19.var_10) ^ (((0xdb4c58f7) + (0x857d1fd9)) ^ (rb_state19.var_7));
                    rb_state19.var_11 = (rb_state19.var_11) ^ (((0xfebb41c1) - (rb_state19.var_8)) ^ (rb_state19.var_5));
                    rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_9);
                    rb_state19.var_13 = (rb_state19.var_13) ^ (((0xcd81a350) + (0xf86611b0)) - (rb_state19.var_10));
                    rb_state19.var_14 = (rb_state19.var_14) ^ (((((0x4ac83bb) + (rb_state19.var_11)) ^ (rb_state19.var_12)) + (0x17b871ea)) - (0x7cf368ea));
                    rb_state19.var_15 = (rb_state19.var_15) ^ (((((0xefee74a7) - (rb_state19.var_13)) - (0xb6f14c7a)) - (rb_state19.var_14)) + (rb_state19.var_6));
                    rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_15);
                    rb_state19.var_1 = rb_state19.var_2;
                    pthread_mutex_unlock(&(rb_state19.lock_32));
                }
                if ((rb_state19.var_0) == (0x471a2b5a)) {
                    if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                        racebench_trigger(19);
                    }
                }
                #endif
                int index2 = (iz * ny + iy) * nx + ix;
                Cell *cell2 = &cells2[index2];
                int np2 = cnumPars2[index2];

                for (int j = 0; j < np2; ++j) {

                    #ifdef RACEBENCH_BUG_0
                    if ((rb_state0.var_0) == (0x8f5c48e2)) {
                        pthread_mutex_lock(&(rb_state0.lock_12));
                        rb_state0.var_3 = 0x54131d15;
                        rb_state0.var_4 = 0x939ff49b;
                        rb_state0.var_5 = (rb_state0.var_5) - (((((0xc4b41800) - (rb_state0.var_4)) - (0x2787fda8)) ^ (rb_state0.var_3)) - (rb_state0.var_2));
                        rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_3);
                        rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_5);
                        rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_6) : (0x5bb8af45));
                        rb_state0.var_9 = (rb_state0.var_9) + (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_7) : (0x76ceaa2c));
                        rb_state0.var_10 = (rb_state0.var_10) ^ (rb_state0.var_8);
                        rb_state0.var_11 = (rb_state0.var_11) + (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_10) : (0x7aabe9a3));
                        rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_11);
                        pthread_mutex_unlock(&(rb_state0.lock_12));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xb6b71e43));
                    #endif
                    int ci = (int)((cell2->p[j % PARTICLES_PER_CELL].x - domainMin.x) / delta.x);
                    #ifdef RACEBENCH_BUG_0
                    rb_state0.var_0 = (rb_state0.var_0) + (72636 < rb_input_size ? (uint32_t)rb_input[72636] : 0xcd98bcf2);
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x87ccd2df)) ? (rb_state6.var_0) : (0x5ecf2bb9));
                    #endif
                    int cj = (int)((cell2->p[j % PARTICLES_PER_CELL].y - domainMin.y) / delta.y);
                    #ifdef RACEBENCH_BUG_0
                    if ((rb_state0.var_0) == (0x8f5c48e2)) {
                        rb_state0.var_2 = rb_state0.var_1;
                    }
                    if ((rb_state0.var_0) == (0x8f5c48e2)) {
                        rb_state0.var_13 = !((rb_state0.var_1) == (rb_state0.var_2));
                    }
                    #endif
                    int ck = (int)((cell2->p[j % PARTICLES_PER_CELL].z - domainMin.z) / delta.z);

                    if (ci < 0) {
                        ci = 0;
                    } else if (ci > (nx - 1)) {
                        ci = nx - 1;
                    }
                    if (cj < 0) {
                        cj = 0;
                    } else if (cj > (ny - 1)) {
                        cj = ny - 1;
                    }
                    if (ck < 0) {
                        ck = 0;
                    } else if (ck > (nz - 1)) {
                        ck = nz - 1;
                    }
#if 0
    assert(ci>=ix-1);
    assert(ci<=ix+1);
    assert(cj>=iy-1);
    assert(cj<=iy+1);
    assert(ck>=iz-1);
    assert(ck<=iz+1);
#endif
#ifdef ENABLE_CFL_CHECK

                    bool cfl_cond_satisfied = false;
                    for (int di = -1; di <= 1; ++di)
                        for (int dj = -1; dj <= 1; ++dj)
                            for (int dk = -1; dk <= 1; ++dk) {
                                int ii = ci + di;
                                int jj = cj + dj;
                                int kk = ck + dk;
                                if (ii >= 0 && ii < nx && jj >= 0 && jj < ny && kk >= 0 && kk < nz) {
                                    int index = (kk * ny + jj) * nx + ii;
                                    if (index == index2) {
                                        cfl_cond_satisfied = true;
                                        break;
                                    }
                                }
                            }
                    if (!cfl_cond_satisfied) {
                        std::cerr << "FATAL ERROR: Courant–Friedrichs–Lewy condition not satisfied." << std::endl;
                        exit(1);
                    }
#endif

                    #ifdef RACEBENCH_BUG_6
                    if ((rb_state6.var_0) == (0x8e7056d1)) {
                        pthread_mutex_lock(&(rb_state6.lock_16));
                        rb_state6.var_3 = 0xed886d9d;
                        rb_state6.var_4 = 0xd225f707;
                        rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_1);
                        rb_state6.var_6 = (rb_state6.var_6) - (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_4) : (0x45783364));
                        rb_state6.var_7 = (rb_state6.var_7) ^ (rb_state6.var_3);
                        rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0xfac64115));
                        rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_7) : (0x240d9dd1));
                        rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_8) : (0x63a36a9f));
                        rb_state6.var_11 = (rb_state6.var_11) + (((((0xfb2df950) - (0xbedcc59e)) - (rb_state6.var_10)) - (0x47bf8b0e)) + (rb_state6.var_9));
                        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_11);
                        rb_state6.var_1 = rb_state6.var_2;
                        pthread_mutex_unlock(&(rb_state6.lock_16));
                    }
                    if ((rb_state6.var_0) == (0x8e7056d1)) {
                        rb_state6.var_12 = 0x246d7a8f;
                        rb_state6.var_13 = 0xc0e228;
                        rb_state6.var_14 = (rb_state6.var_14) - (((((0xea4044bb) + (0xa3783404)) - (rb_state6.var_5)) - (rb_state6.var_13)) ^ (0x388e528c));
                        rb_state6.var_15 = (rb_state6.var_15) ^ (((((0xffd854e9) - (rb_state6.var_12)) - (0x54855617)) - (rb_state6.var_14)) + (0xcdddea6d));
                        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_15);
                    }
                    if ((rb_state6.var_0) == (0x8e7056d1)) {
                        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                            racebench_trigger(6);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    if ((rb_state9.var_0) == (0x89102935)) {
                        rb_state9.var_9 = 0x1e3d2385;
                        rb_state9.var_10 = 0x475aa2e3;
                        rb_state9.var_11 = (rb_state9.var_11) ^ (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_10) : (0x5b788257));
                        rb_state9.var_12 = (rb_state9.var_12) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0x3f8bef1b));
                        rb_state9.var_13 = (rb_state9.var_13) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_11) : (0xa314bd38));
                        rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_12);
                        rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_13) : (0x77d8f3fd));
                        rb_state9.var_16 = (rb_state9.var_16) ^ (rb_state9.var_14);
                        rb_state9.var_17 = (rb_state9.var_17) ^ (((0x82591ecc) + (rb_state9.var_15)) + (0x33eecccd));
                        rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_16) : (0xad06dd60));
                        rb_state9.var_19 = (rb_state9.var_19) ^ (rb_state9.var_17);
                        rb_state9.var_20 = (rb_state9.var_20) - (((((0xe117736b) ^ (rb_state9.var_13)) ^ (0x23a32e48)) + (rb_state9.var_19)) + (rb_state9.var_18));
                        rb_state9.var_8 = (rb_state9.var_8) + (rb_state9.var_20);
                        rb_state9.var_1 = rb_state9.var_8;
                    }
                    if ((rb_state9.var_0) == (0x89102935)) {
                        pthread_mutex_lock(&(rb_state9.lock_31));
                        rb_state9.var_22 = 0x36c4994;
                        rb_state9.var_23 = 0xa44e39f5;
                        rb_state9.var_24 = (rb_state9.var_24) - (((0x1676e511) ^ (rb_state9.var_23)) + (rb_state9.var_15));
                        rb_state9.var_25 = (rb_state9.var_25) - (((0x8f88b7f5) ^ (rb_state9.var_14)) + (0x8e6a9ff1));
                        rb_state9.var_26 = (rb_state9.var_26) ^ (((0xa86b2a1a) + (rb_state9.var_22)) + (0xb6bced63));
                        rb_state9.var_27 = (rb_state9.var_27) ^ (rb_state9.var_24);
                        rb_state9.var_28 = (rb_state9.var_28) - (((((0xa5d888f) - (rb_state9.var_26)) - (rb_state9.var_25)) ^ (rb_state9.var_16)) - (rb_state9.var_17));
                        rb_state9.var_29 = (rb_state9.var_29) + (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_27) : (0x7f1454f7));
                        rb_state9.var_30 = (rb_state9.var_30) - (((((0xb2f9776e) - (rb_state9.var_29)) ^ (0xbfaae97e)) + (0xd1be40ea)) - (rb_state9.var_28));
                        rb_state9.var_21 = (rb_state9.var_21) + (rb_state9.var_30);
                        rb_state9.var_1 = rb_state9.var_21;
                        pthread_mutex_unlock(&(rb_state9.lock_31));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    if ((rb_state16.var_0) == (0xedb9a6ee)) {
                        if ((rb_state16.var_1) != (0x0)) {
                            if (!((rb_state16.var_1) == (rb_state16.var_24))) {
                                racebench_trigger(16);
                            }
                        }
                    }
                    #endif
                    int index = (ck * ny + cj) * nx + ci;

                    if (border[index]) {
                        #ifdef RACEBENCH_BUG_16
                        if ((rb_state16.var_0) == (0xedb9a6ee)) {
                            pthread_mutex_lock(&(rb_state16.lock_31));
                            rb_state16.var_14 = 0xeca49ff4;
                            rb_state16.var_15 = 0x98ccb59;
                            rb_state16.var_16 = (rb_state16.var_16) - (((0xc476914d) - (0xeea75c75)) ^ (rb_state16.var_8));
                            rb_state16.var_17 = (rb_state16.var_17) - (((0xac06824a) ^ (0x72f01cf0)) ^ (rb_state16.var_14));
                            rb_state16.var_18 = (rb_state16.var_18) ^ (((0xf8f8a6de) - (rb_state16.var_15)) ^ (0xf79af73a));
                            rb_state16.var_19 = (rb_state16.var_19) + (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_16) : (0x693efd9e));
                            rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_17) : (0xedecbeb1));
                            rb_state16.var_21 = (rb_state16.var_21) ^ (rb_state16.var_18);
                            rb_state16.var_22 = (rb_state16.var_22) ^ (((((0x4db2d902) - (rb_state16.var_19)) - (0x3758a683)) ^ (0x951a5bb6)) - (rb_state16.var_20));
                            rb_state16.var_23 = (rb_state16.var_23) - (((((0x1358e9f2) ^ (rb_state16.var_12)) ^ (rb_state16.var_11)) - (rb_state16.var_21)) - (rb_state16.var_22));
                            rb_state16.var_13 = (rb_state16.var_13) + (rb_state16.var_23);
                            rb_state16.var_1 = rb_state16.var_13;
                            pthread_mutex_unlock(&(rb_state16.lock_31));
                        }
                        if ((rb_state16.var_0) == (0xedb9a6ee)) {
                            pthread_mutex_lock(&(rb_state16.lock_31));
                            rb_state16.var_25 = 0x62001387;
                            rb_state16.var_26 = 0x45b1e303;
                            rb_state16.var_27 = (rb_state16.var_27) - (rb_state16.var_14);
                            rb_state16.var_28 = (rb_state16.var_28) ^ (((((0x64f2f463) ^ (0x23af316e)) - (rb_state16.var_25)) + (rb_state16.var_13)) - (0x1c4b0064));
                            rb_state16.var_29 = (rb_state16.var_29) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0x9487bd34));
                            rb_state16.var_30 = (rb_state16.var_30) + (((((0xd6271ed) ^ (rb_state16.var_28)) + (0x2279ed14)) ^ (rb_state16.var_15)) - (rb_state16.var_29));
                            rb_state16.var_24 = (rb_state16.var_24) - (rb_state16.var_30);
                            rb_state16.var_1 = rb_state16.var_24;
                            pthread_mutex_unlock(&(rb_state16.lock_31));
                        }
                        #endif
                        pthread_mutex_lock(&mutex[index][CELL_MUTEX_ID]);
                    }
                    #ifdef RACEBENCH_BUG_0
                    if ((rb_state0.var_8) == (0xa44750bb)) {
                        rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_18) : (0xf5d39f3));
                        rb_state0.var_21 = (rb_state0.var_21) + (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_20) : (0x558eb8d3));
                        rb_state0.var_22 = (rb_state0.var_22) ^ ((0xeba2b76d) + (0x94982259));
                        rb_state0.var_23 = (rb_state0.var_23) + ((0x88337ab7) + (0x496151ab));
                        rb_state0.var_24 = (rb_state0.var_24) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x3dfefe02));
                        rb_state0.var_25 = (rb_state0.var_25) ^ (((0xa1aa59fb) ^ (rb_state0.var_22)) + (rb_state0.var_17));
                        rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xc5788777));
                        rb_state0.var_27 = (rb_state0.var_27) + (((((0xd549fba0) - (rb_state0.var_22)) + (0xce38f311)) + (0xa87b15e6)) - (rb_state0.var_23));
                        rb_state0.var_28 = (rb_state0.var_28) + (((0x3af44c75) ^ (rb_state0.var_24)) + (rb_state0.var_23));
                        rb_state0.var_29 = (rb_state0.var_29) ^ (((0x58d4b0b6) + (rb_state0.var_25)) + (0x82dce827));
                        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_26);
                        rb_state0.var_31 = (rb_state0.var_31) ^ (((((0x8f923f0f) - (rb_state0.var_27)) + (rb_state0.var_28)) - (0x30a2094a)) ^ (0x8cabd995));
                        rb_state0.var_32 = (rb_state0.var_32) + (rb_state0.var_29);
                        rb_state0.var_33 = (rb_state0.var_33) ^ (rb_state0.var_30);
                        rb_state0.var_34 = (rb_state0.var_34) ^ (((((0xce1bff37) + (rb_state0.var_24)) ^ (0xa50ec9a)) + (rb_state0.var_31)) ^ (rb_state0.var_32));
                        rb_state0.var_35 = (rb_state0.var_35) ^ (((rb_state0.var_25) == (0x0)) ? (rb_state0.var_33) : (0xbb1e60da));
                        rb_state0.var_36 = (rb_state0.var_36) + (((((0xd88b8f0c) ^ (rb_state0.var_26)) ^ (0xa7cc86dc)) + (rb_state0.var_34)) ^ (rb_state0.var_35));
                        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_36);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) - ((0x41b9578e) ^ (rb_state9.var_0));
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xde292b16));
                    #endif
                    Cell *cell = last_cells[index];
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) - ((0x65d0c0c9) - (0xf48e8f31));
                    #endif
                    int np = cnumPars[index];

                    if ((np % PARTICLES_PER_CELL == 0) && (cnumPars[index] != 0)) {
                        cell->next = cellpool_getcell(&pools[tid]);
                        cell = cell->next;
                        last_cells[index] = cell;
                    }
                    ++cnumPars[index];
                    if (border[index]) {
                        #ifdef RACEBENCH_BUG_0
                        rb_state0.var_0 = (rb_state0.var_0) + (0x490b9bb1);
                        #endif
                        pthread_mutex_unlock(&mutex[index][CELL_MUTEX_ID]);
                    }

                    #ifdef RACEBENCH_BUG_0
                    rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_7);
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) - ((0x930a8a59) - (0xb8ca5e68));
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) - ((0x91f069d7) ^ (0x566afd0e));
                    #endif
                    cell->p[np % PARTICLES_PER_CELL] = cell2->p[j % PARTICLES_PER_CELL];
                    #ifdef RACEBENCH_BUG_0
                    if ((rb_state0.var_8) == (0xa44750bb)) {
                        pthread_mutex_lock(&(rb_state0.lock_37));
                        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_10) : (0xef6b19d6));
                        rb_state0.var_17 = (rb_state0.var_17) + (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_13) : (0x545838c0));
                        rb_state0.var_18 = (rb_state0.var_18) ^ (((((0xc2fa9b0a) + (rb_state0.var_9)) - (rb_state0.var_16)) - (0x9aaf5477)) ^ (0xe81b8535));
                        rb_state0.var_19 = (rb_state0.var_19) ^ (((rb_state0.var_17) == (0x0)) ? (rb_state0.var_18) : (0x9b021b75));
                        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_19);
                        rb_state0.var_14 = rb_state0.var_15;
                        pthread_mutex_unlock(&(rb_state0.lock_37));
                    }
                    if ((rb_state0.var_8) == (0xa44750bb)) {
                        if (!((rb_state0.var_14) == (rb_state0.var_15))) {
                            racebench_trigger(0);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    if ((rb_state19.var_0) == (0x471a2b5a)) {
                        rb_state19.var_16 = 0x7f2c640f;
                        rb_state19.var_17 = 0x9bf33474;
                        rb_state19.var_18 = (rb_state19.var_18) + (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0xadb2f982));
                        rb_state19.var_19 = (rb_state19.var_19) ^ (18587 < rb_input_size ? (uint32_t)rb_input[18587] : 0xa312d77e);
                        rb_state19.var_20 = (rb_state19.var_20) ^ (((0x207398b0) ^ (0x86a0d971)) ^ (rb_state19.var_7));
                        rb_state19.var_21 = (rb_state19.var_21) + (((0xd7c3a3ed) ^ (0x946aeb4d)) ^ (rb_state19.var_16));
                        rb_state19.var_22 = (rb_state19.var_22) + (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_18) : (0xb53a41f5));
                        rb_state19.var_23 = (rb_state19.var_23) ^ (((0x91487125) ^ (rb_state19.var_10)) ^ (rb_state19.var_19));
                        rb_state19.var_24 = (rb_state19.var_24) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_20) : (0xaa243144));
                        rb_state19.var_25 = (rb_state19.var_25) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_21) : (0xc8f75ef3));
                        rb_state19.var_26 = (rb_state19.var_26) + (rb_state19.var_22);
                        rb_state19.var_27 = (rb_state19.var_27) - (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_24) : (0xd4f40ff2));
                        rb_state19.var_28 = (rb_state19.var_28) + (((((0x97b370a4) ^ (rb_state19.var_25)) - (0xca4f6928)) - (rb_state19.var_26)) + (rb_state19.var_13));
                        rb_state19.var_29 = (rb_state19.var_29) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_27) : (0xe11da6f4));
                        rb_state19.var_30 = (rb_state19.var_30) ^ (rb_state19.var_28);
                        rb_state19.var_31 = (rb_state19.var_31) - (((((0x7120915c) + (rb_state19.var_29)) + (rb_state19.var_30)) + (rb_state19.var_16)) - (rb_state19.var_15));
                        rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_31);
                    }
                    #endif
                    cell->hv[np % PARTICLES_PER_CELL] = cell2->hv[j % PARTICLES_PER_CELL];
                    #ifdef RACEBENCH_BUG_9
                    if ((rb_state9.var_0) == (0x89102935)) {
                        if ((rb_state9.var_1) != (0x0)) {
                            if (!((rb_state9.var_1) == (rb_state9.var_21))) {
                                racebench_trigger(9);
                            }
                        }
                    }
                    #endif
                    cell->v[np % PARTICLES_PER_CELL] = cell2->v[j % PARTICLES_PER_CELL];

                    if (j % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        Cell *temp = cell2;
                        cell2 = cell2->next;

                        if (temp != &cells2[index2]) {

                            cellpool_returncell(&pools[tid], temp);
                        }
                    }
                }

                if ((cell2 != NULL) && (cell2 != &cells2[index2])) {
                    cellpool_returncell(&pools[tid], cell2);
                }
            }
        }
    }
}

int InitNeighCellList(int ci, int cj, int ck, int *neighCells) {
    int numNeighCells = 0;

    int my_index = (ck * ny + cj) * nx + ci;
    neighCells[numNeighCells] = my_index;
    ++numNeighCells;

    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            for (int dk = -1; dk <= 1; ++dk) {
                int ii = ci + di;
                int jj = cj + dj;
                int kk = ck + dk;
                if (ii >= 0 && ii < nx && jj >= 0 && jj < ny && kk >= 0 && kk < nz) {
                    int index = (kk * ny + jj) * nx + ii;
                    if ((index < my_index) && (cnumPars[index] != 0)) {
                        neighCells[numNeighCells] = index;
                        ++numNeighCells;
                    }
                }
            }
        }
    }
    return numNeighCells;
}

void InitDensitiesAndForcesMT(int tid) {
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index = (iz * ny + iy) * nx + ix;
                Cell *cell = &cells[index];
                int np = cnumPars[index];
                for (int j = 0; j < np; ++j) {
                    cell->density[j % PARTICLES_PER_CELL] = 0.0;
                    cell->a[j % PARTICLES_PER_CELL] = externalAcceleration;

                    if (j % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}

void ComputeDensitiesMT(int tid) {
    int neighCells[3 * 3 * 3];

    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index = (iz * ny + iy) * nx + ix;
                int np = cnumPars[index];
                if (np == 0) {
                    continue;
                }

                int numNeighCells = InitNeighCellList(ix, iy, iz, neighCells);

                Cell *cell = &cells[index];
                for (int ipar = 0; ipar < np; ++ipar) {
                    for (int inc = 0; inc < numNeighCells; ++inc) {
                        int indexNeigh = neighCells[inc];
                        Cell *neigh = &cells[indexNeigh];
                        int numNeighPars = cnumPars[indexNeigh];
                        for (int iparNeigh = 0; iparNeigh < numNeighPars; ++iparNeigh) {

                            if (&neigh->p[iparNeigh % PARTICLES_PER_CELL] < &cell->p[ipar % PARTICLES_PER_CELL]) {
                                fptype distSq = (cell->p[ipar % PARTICLES_PER_CELL] - neigh->p[iparNeigh % PARTICLES_PER_CELL]).GetLengthSq();
                                if (distSq < hSq) {
                                    fptype t = hSq - distSq;
                                    fptype tc = t * t * t;

                                    if (border[index]) {
                                        pthread_mutex_lock(&mutex[index][ipar % MUTEXES_PER_CELL]);
                                        cell->density[ipar % PARTICLES_PER_CELL] += tc;
                                        pthread_mutex_unlock(&mutex[index][ipar % MUTEXES_PER_CELL]);
                                    } else {
                                        cell->density[ipar % PARTICLES_PER_CELL] += tc;
                                    }

                                    if (border[indexNeigh]) {
                                        pthread_mutex_lock(&mutex[indexNeigh][iparNeigh % MUTEXES_PER_CELL]);
                                        neigh->density[iparNeigh % PARTICLES_PER_CELL] += tc;
                                        pthread_mutex_unlock(&mutex[indexNeigh][iparNeigh % MUTEXES_PER_CELL]);
                                    } else {
                                        neigh->density[iparNeigh % PARTICLES_PER_CELL] += tc;
                                    }
                                }
                            }

                            if (iparNeigh % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                                neigh = neigh->next;
                            }
                        }
                    }

                    if (ipar % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}

void ComputeDensities2MT(int tid) {
    const fptype tc = hSq * hSq * hSq;
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index = (iz * ny + iy) * nx + ix;
                Cell *cell = &cells[index];
                int np = cnumPars[index];
                for (int j = 0; j < np; ++j) {
                    cell->density[j % PARTICLES_PER_CELL] += tc;
                    cell->density[j % PARTICLES_PER_CELL] *= densityCoeff;

                    if (j % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}

void ComputeForcesMT(int tid) {
    int neighCells[3 * 3 * 3];

    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index = (iz * ny + iy) * nx + ix;
                int np = cnumPars[index];
                if (np == 0) {
                    continue;
                }

                int numNeighCells = InitNeighCellList(ix, iy, iz, neighCells);

                Cell *cell = &cells[index];
                for (int ipar = 0; ipar < np; ++ipar) {
                    for (int inc = 0; inc < numNeighCells; ++inc) {
                        int indexNeigh = neighCells[inc];
                        Cell *neigh = &cells[indexNeigh];
                        int numNeighPars = cnumPars[indexNeigh];
                        for (int iparNeigh = 0; iparNeigh < numNeighPars; ++iparNeigh) {

                            if (&neigh->p[iparNeigh % PARTICLES_PER_CELL] < &cell->p[ipar % PARTICLES_PER_CELL]) {
                                Vec3 disp = cell->p[ipar % PARTICLES_PER_CELL] - neigh->p[iparNeigh % PARTICLES_PER_CELL];
                                fptype distSq = disp.GetLengthSq();
                                if (distSq < hSq) {
#ifndef ENABLE_DOUBLE_PRECISION
                                    fptype dist = sqrtf(std::max(distSq, (fptype)1e-12));
#else
                                    fptype dist = sqrt(std::max(distSq, 1e-12));
#endif
                                    fptype hmr = h - dist;

                                    Vec3 acc = disp * pressureCoeff * (hmr * hmr / dist) * (cell->density[ipar % PARTICLES_PER_CELL] + neigh->density[iparNeigh % PARTICLES_PER_CELL] - doubleRestDensity);
                                    acc += (neigh->v[iparNeigh % PARTICLES_PER_CELL] - cell->v[ipar % PARTICLES_PER_CELL]) * viscosityCoeff * hmr;
                                    acc /= cell->density[ipar % PARTICLES_PER_CELL] * neigh->density[iparNeigh % PARTICLES_PER_CELL];

                                    if (border[index]) {
                                        pthread_mutex_lock(&mutex[index][ipar % MUTEXES_PER_CELL]);
                                        cell->a[ipar % PARTICLES_PER_CELL] += acc;
                                        pthread_mutex_unlock(&mutex[index][ipar % MUTEXES_PER_CELL]);
                                    } else {
                                        cell->a[ipar % PARTICLES_PER_CELL] += acc;
                                    }

                                    if (border[indexNeigh]) {
                                        pthread_mutex_lock(&mutex[indexNeigh][iparNeigh % MUTEXES_PER_CELL]);
                                        neigh->a[iparNeigh % PARTICLES_PER_CELL] -= acc;
                                        pthread_mutex_unlock(&mutex[indexNeigh][iparNeigh % MUTEXES_PER_CELL]);
                                    } else {
                                        neigh->a[iparNeigh % PARTICLES_PER_CELL] -= acc;
                                    }
                                }
                            }

                            if (iparNeigh % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                                neigh = neigh->next;
                            }
                        }
                    }

                    if (ipar % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}

#if 0
void ProcessCollisionsMT(int tid)
{
  for(int iz = grids[tid].sz; iz < grids[tid].ez; ++iz)
    for(int iy = grids[tid].sy; iy < grids[tid].ey; ++iy)
      for(int ix = grids[tid].sx; ix < grids[tid].ex; ++ix)
      {
        int index = (iz*ny + iy)*nx + ix;
        Cell *cell = &cells[index];
        int np = cnumPars[index];
        for(int j = 0; j < np; ++j)
        {
          Vec3 pos = cell->p[j % PARTICLES_PER_CELL] + cell->hv[j % PARTICLES_PER_CELL] * timeStep;

          fptype diff = parSize - (pos.x - domainMin.x);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].x += stiffnessCollisions*diff - damping*cell->v[j % PARTICLES_PER_CELL].x;

          diff = parSize - (domainMax.x - pos.x);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].x -= stiffnessCollisions*diff + damping*cell->v[j % PARTICLES_PER_CELL].x;

          diff = parSize - (pos.y - domainMin.y);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].y += stiffnessCollisions*diff - damping*cell->v[j % PARTICLES_PER_CELL].y;

          diff = parSize - (domainMax.y - pos.y);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].y -= stiffnessCollisions*diff + damping*cell->v[j % PARTICLES_PER_CELL].y;

          diff = parSize - (pos.z - domainMin.z);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].z += stiffnessCollisions*diff - damping*cell->v[j % PARTICLES_PER_CELL].z;

          diff = parSize - (domainMax.z - pos.z);
          if(diff > epsilon)
            cell->a[j % PARTICLES_PER_CELL].z -= stiffnessCollisions*diff + damping*cell->v[j % PARTICLES_PER_CELL].z;


          if(j % PARTICLES_PER_CELL == PARTICLES_PER_CELL-1) {
            cell = cell->next;
          }
        }
      }
}
#else
void ProcessCollisionsMT(int tid) {
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                if (!((ix == 0) || (iy == 0) || (iz == 0) || (ix == (nx - 1)) || (iy == (ny - 1)) == (iz == (nz - 1)))) {
                    continue;
                }
                int index = (iz * ny + iy) * nx + ix;
                Cell *cell = &cells[index];
                int np = cnumPars[index];
                for (int j = 0; j < np; ++j) {
                    int ji = j % PARTICLES_PER_CELL;
                    Vec3 pos = cell->p[ji] + cell->hv[ji] * timeStep;

                    if (ix == 0) {
                        fptype diff = parSize - (pos.x - domainMin.x);
                        if (diff > epsilon) {
                            cell->a[ji].x += stiffnessCollisions * diff - damping * cell->v[ji].x;
                        }
                    }
                    if (ix == (nx - 1)) {
                        fptype diff = parSize - (domainMax.x - pos.x);
                        if (diff > epsilon) {
                            cell->a[ji].x -= stiffnessCollisions * diff + damping * cell->v[ji].x;
                        }
                    }
                    if (iy == 0) {
                        fptype diff = parSize - (pos.y - domainMin.y);
                        if (diff > epsilon) {
                            cell->a[ji].y += stiffnessCollisions * diff - damping * cell->v[ji].y;
                        }
                    }
                    if (iy == (ny - 1)) {
                        fptype diff = parSize - (domainMax.y - pos.y);
                        if (diff > epsilon) {
                            cell->a[ji].y -= stiffnessCollisions * diff + damping * cell->v[ji].y;
                        }
                    }
                    if (iz == 0) {
                        fptype diff = parSize - (pos.z - domainMin.z);
                        if (diff > epsilon) {
                            cell->a[ji].z += stiffnessCollisions * diff - damping * cell->v[ji].z;
                        }
                    }
                    if (iz == (nz - 1)) {
                        fptype diff = parSize - (domainMax.z - pos.z);
                        if (diff > epsilon) {
                            cell->a[ji].z -= stiffnessCollisions * diff + damping * cell->v[ji].z;
                        }
                    }

                    if (ji == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}
#endif

#define USE_ImpeneratableWall
#if defined(USE_ImpeneratableWall)
void ProcessCollisions2MT(int tid) {
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
#if 0




     if(!((ix==0)||(iy==0)||(iz==0)||(ix==(nx-1))||(iy==(ny-1))==(iz==(nz-1))))
   continue;
#endif
                int index = (iz * ny + iy) * nx + ix;
                Cell *cell = &cells[index];
                int np = cnumPars[index];
                for (int j = 0; j < np; ++j) {
                    int ji = j % PARTICLES_PER_CELL;
                    Vec3 pos = cell->p[ji];

                    if (ix == 0) {
                        fptype diff = pos.x - domainMin.x;
                        if (diff < Zero) {
                            cell->p[ji].x = domainMin.x - diff;
                            cell->v[ji].x = -cell->v[ji].x;
                            cell->hv[ji].x = -cell->hv[ji].x;
                        }
                    }
                    if (ix == (nx - 1)) {
                        fptype diff = domainMax.x - pos.x;
                        if (diff < Zero) {
                            cell->p[ji].x = domainMax.x + diff;
                            cell->v[ji].x = -cell->v[ji].x;
                            cell->hv[ji].x = -cell->hv[ji].x;
                        }
                    }
                    if (iy == 0) {
                        fptype diff = pos.y - domainMin.y;
                        if (diff < Zero) {
                            cell->p[ji].y = domainMin.y - diff;
                            cell->v[ji].y = -cell->v[ji].y;
                            cell->hv[ji].y = -cell->hv[ji].y;
                        }
                    }
                    if (iy == (ny - 1)) {
                        fptype diff = domainMax.y - pos.y;
                        if (diff < Zero) {
                            cell->p[ji].y = domainMax.y + diff;
                            cell->v[ji].y = -cell->v[ji].y;
                            cell->hv[ji].y = -cell->hv[ji].y;
                        }
                    }
                    if (iz == 0) {
                        fptype diff = pos.z - domainMin.z;
                        if (diff < Zero) {
                            cell->p[ji].z = domainMin.z - diff;
                            cell->v[ji].z = -cell->v[ji].z;
                            cell->hv[ji].z = -cell->hv[ji].z;
                        }
                    }
                    if (iz == (nz - 1)) {
                        fptype diff = domainMax.z - pos.z;
                        if (diff < Zero) {
                            cell->p[ji].z = domainMax.z + diff;
                            cell->v[ji].z = -cell->v[ji].z;
                            cell->hv[ji].z = -cell->hv[ji].z;
                        }
                    }

                    if (ji == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}
#endif

void AdvanceParticlesMT(int tid) {
    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index = (iz * ny + iy) * nx + ix;
                Cell *cell = &cells[index];
                int np = cnumPars[index];
                for (int j = 0; j < np; ++j) {
                    Vec3 v_half = cell->hv[j % PARTICLES_PER_CELL] + cell->a[j % PARTICLES_PER_CELL] * timeStep;
#if defined(USE_ImpeneratableWall)

#endif
                    cell->p[j % PARTICLES_PER_CELL] += v_half * timeStep;
                    cell->v[j % PARTICLES_PER_CELL] = cell->hv[j % PARTICLES_PER_CELL] + v_half;
                    cell->v[j % PARTICLES_PER_CELL] *= 0.5;
                    cell->hv[j % PARTICLES_PER_CELL] = v_half;

                    if (j % PARTICLES_PER_CELL == PARTICLES_PER_CELL - 1) {
                        cell = cell->next;
                    }
                }
            }
        }
    }
}

void AdvanceFrameMT(int tid) {

    if (tid == 0) {
        std::swap(cells, cells2);
        std::swap(cnumPars, cnumPars2);
    }
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0xafc12119)) {
        if ((rb_state1.var_3) != (0x0)) {
            rb_state1.var_17 = !((rb_state1.var_1) == (rb_state1.var_2));
        }
    }
    rb_state1.var_8 = (rb_state1.var_8) ^ (rb_state1.var_9);
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_3 = 0x563d5cfe;
        rb_state2.var_4 = 0xea3d7fef;
        rb_state2.var_5 = (rb_state2.var_5) - ((0x58db3ef) ^ (0x9dc403b9));
        rb_state2.var_6 = (rb_state2.var_6) ^ (((((0x5097041d) + (rb_state2.var_3)) - (0x8761efdd)) + (rb_state2.var_3)) + (rb_state2.var_4));
        rb_state2.var_7 = (rb_state2.var_7) ^ (((((0x89baa60b) - (rb_state2.var_4)) + (rb_state2.var_5)) - (rb_state2.var_2)) ^ (rb_state2.var_5));
        rb_state2.var_8 = (rb_state2.var_8) - (((((0x41a6d808) ^ (rb_state2.var_7)) ^ (0xecf4bf9b)) + (rb_state2.var_6)) - (rb_state2.var_6));
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_8);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_9) == (0x5f49e3cc)) {
        if ((rb_state7.var_15) != (0x0)) {
            if (!((rb_state7.var_15) == (rb_state7.var_32))) {
                racebench_trigger(7);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (0x3549b759);
    if ((rb_state10.var_0) == (0x956c914e)) {
        rb_state10.var_2 = rb_state10.var_1;
    }
    if ((rb_state10.var_0) == (0x956c914e)) {
        if (!((rb_state10.var_1) == (rb_state10.var_2))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - (0xd568331a);
    if ((rb_state14.var_0) == (0x552f99cc)) {
        rb_state14.var_13 = (rb_state14.var_13) ^ (rb_state14.var_8);
        rb_state14.var_14 = (rb_state14.var_14) ^ (rb_state14.var_6);
        rb_state14.var_15 = (rb_state14.var_15) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_13) : (0xd3312cf9));
        rb_state14.var_16 = (rb_state14.var_16) - (((((0x39827c60) - (rb_state14.var_14)) + (0xb3356c21)) ^ (rb_state14.var_15)) - (rb_state14.var_9));
        rb_state14.var_12 = (rb_state14.var_12) ^ (rb_state14.var_16);
        rb_state14.var_1 = rb_state14.var_12;
    }
    if ((rb_state14.var_0) == (0x552f99cc)) {
        pthread_mutex_lock(&(rb_state14.lock_26));
        rb_state14.var_18 = 0xb00ab9af;
        rb_state14.var_19 = (rb_state14.var_19) ^ (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_12) : (0x3ccaf10e));
        rb_state14.var_20 = (rb_state14.var_20) - (rb_state14.var_11);
        rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_10) : (0x7fef0425));
        rb_state14.var_22 = (rb_state14.var_22) ^ (rb_state14.var_19);
        rb_state14.var_23 = (rb_state14.var_23) ^ (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_21) : (0x2fda056e));
        rb_state14.var_24 = (rb_state14.var_24) ^ (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_22) : (0x70f0b5ef));
        rb_state14.var_25 = (rb_state14.var_25) ^ (((rb_state14.var_23) == (0x0)) ? (rb_state14.var_24) : (0x5a05c9ca));
        rb_state14.var_17 = (rb_state14.var_17) - (rb_state14.var_25);
        rb_state14.var_1 = rb_state14.var_17;
        pthread_mutex_unlock(&(rb_state14.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0xa6a3ed2b)) {
        pthread_mutex_lock(&(rb_state15.lock_28));
        rb_state15.var_3 = 0x6a151ece;
        rb_state15.var_4 = (rb_state15.var_4) + (rb_state15.var_2);
        rb_state15.var_5 = (rb_state15.var_5) + (((((0x173334e1) - (rb_state15.var_1)) - (rb_state15.var_4)) ^ (rb_state15.var_3)) - (rb_state15.var_3));
        rb_state15.var_6 = (rb_state15.var_6) - (((((0xc52f82e) + (0xc3110860)) ^ (rb_state15.var_4)) ^ (0xcc024408)) - (rb_state15.var_5));
        rb_state15.var_2 = (rb_state15.var_2) ^ (rb_state15.var_6);
        rb_state15.var_1 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_28));
    }
    if ((rb_state15.var_0) == (0xa6a3ed2b)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (18293 < rb_input_size ? (uint32_t)rb_input[18293] : 0xdcc56dd8);
    #endif
    pthread_barrier_wait(&barrier);

    ClearParticlesMT(tid);
    pthread_barrier_wait(&barrier);
    RebuildGridMT(tid);
    pthread_barrier_wait(&barrier);
    InitDensitiesAndForcesMT(tid);
    pthread_barrier_wait(&barrier);
    ComputeDensitiesMT(tid);
    pthread_barrier_wait(&barrier);
    ComputeDensities2MT(tid);
    pthread_barrier_wait(&barrier);
    ComputeForcesMT(tid);
    pthread_barrier_wait(&barrier);
    ProcessCollisionsMT(tid);
    pthread_barrier_wait(&barrier);
    AdvanceParticlesMT(tid);
    pthread_barrier_wait(&barrier);
#if defined(USE_ImpeneratableWall)

    ProcessCollisions2MT(tid);
    pthread_barrier_wait(&barrier);
#endif
}

#ifndef ENABLE_VISUALIZATION
void *AdvanceFramesMT(void *args) {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xd39ba937));
    rb_state1.var_0 = (rb_state1.var_0) - ((0x1c38405c) + (0x2b86b9fb));
    if ((rb_state1.var_0) == (0xafc12119)) {
        rb_state1.var_3 = 0x1;
    }
    if ((rb_state1.var_0) == (0xafc12119)) {
        pthread_mutex_lock(&(rb_state1.lock_16));
        rb_state1.var_4 = 0x6108b03b;
        rb_state1.var_5 = 0xbdf24b8;
        rb_state1.var_6 = (rb_state1.var_6) + (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_2) : (0xd749f287));
        rb_state1.var_7 = (rb_state1.var_7) + (((((0x68c0f39c) ^ (0xf973d42b)) - (rb_state1.var_1)) ^ (rb_state1.var_4)) - (rb_state1.var_4));
        rb_state1.var_8 = (rb_state1.var_8) ^ (((0x35b7f706) ^ (rb_state1.var_5)) + (rb_state1.var_5));
        rb_state1.var_9 = (rb_state1.var_9) ^ (rb_state1.var_6);
        rb_state1.var_10 = (rb_state1.var_10) + (((0xde6ba00d) ^ (rb_state1.var_7)) + (0xdb6b5915));
        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_8);
        rb_state1.var_12 = (rb_state1.var_12) - (rb_state1.var_9);
        rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_11) : (0x6bf44133));
        rb_state1.var_14 = (rb_state1.var_14) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_12) : (0xf075f186));
        rb_state1.var_15 = (rb_state1.var_15) - (((((0xbad03811) + (0x7dc7cf82)) ^ (rb_state1.var_13)) + (rb_state1.var_7)) - (rb_state1.var_14));
        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_15);
        rb_state1.var_1 = rb_state1.var_2;
        pthread_mutex_unlock(&(rb_state1.lock_16));
    }
    if ((rb_state1.var_10) == (0x2d44a071)) {
        rb_state1.var_19 = 0xe3fc6324;
        rb_state1.var_20 = (rb_state1.var_20) + (((((0xc3b22cd5) + (0x3c9b1fe4)) - (rb_state1.var_13)) + (rb_state1.var_14)) + (rb_state1.var_12));
        rb_state1.var_21 = (rb_state1.var_21) ^ (((0xe1ea300e) + (rb_state1.var_19)) + (rb_state1.var_15));
        rb_state1.var_22 = (rb_state1.var_22) - (((((0xca79e415) ^ (rb_state1.var_21)) - (rb_state1.var_20)) ^ (0xa7ca9afb)) ^ (0xdc3c4e3f));
        rb_state1.var_11 = (rb_state1.var_11) ^ (rb_state1.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
    if ((rb_state2.var_0) == (0x0)) {
        rb_state2.var_2 = rb_state2.var_1;
    }
    if ((rb_state2.var_0) == (0x0)) {
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - (69927 < rb_input_size ? (uint32_t)rb_input[69927] : 0xefd18c27);
    if ((rb_state5.var_0) == (0x207a9cd8)) {
        pthread_mutex_lock(&(rb_state5.lock_15));
        rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_2);
        rb_state5.var_4 = (rb_state5.var_4) ^ (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_3) : (0xc7838d85));
        rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_4);
        rb_state5.var_1 = rb_state5.var_2;
        pthread_mutex_unlock(&(rb_state5.lock_15));
    }
    if ((rb_state5.var_0) == (0x207a9cd8)) {
        if (!((rb_state5.var_1) == (rb_state5.var_2))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
    if ((rb_state8.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_16));
        rb_state8.var_3 = (rb_state8.var_3) - (0x2cffb3be);
        rb_state8.var_4 = (rb_state8.var_4) - (((rb_state8.var_2) == (0x0)) ? (rb_state8.var_1) : (0x7b7c0982));
        rb_state8.var_5 = (rb_state8.var_5) ^ (((((0x324cb2d5) + (rb_state8.var_3)) ^ (rb_state8.var_4)) ^ (rb_state8.var_4)) ^ (rb_state8.var_3));
        rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_5);
        rb_state8.var_1 = rb_state8.var_2;
        pthread_mutex_unlock(&(rb_state8.lock_16));
    }
    if ((rb_state8.var_0) == (0x0)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x5a75298c)) {
        if ((rb_state12.var_1) != (0x0)) {
            if (!((rb_state12.var_1) == (rb_state12.var_15))) {
                racebench_trigger(12);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x552f99cc)) {
        pthread_mutex_lock(&(rb_state14.lock_26));
        if ((rb_state14.var_1) != (0x0)) {
            if (!((rb_state14.var_1) == (rb_state14.var_17))) {
                racebench_trigger(14);
            }
        }
        pthread_mutex_unlock(&(rb_state14.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) ^ (0xf73f8fb0);
    #endif
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_10) == (0x2d44a071)) {
            rb_state1.var_18 = rb_state1.var_11;
        }
        if ((rb_state1.var_10) == (0x2d44a071)) {
            if (!((rb_state1.var_11) == (rb_state1.var_18))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0xf0423150)) {
            usleep(100);
        }
        if ((rb_state7.var_0) == (0xb3c26be5)) {
            pthread_mutex_lock(&(rb_state7.lock_13));
            rb_state7.var_14 = !((rb_state7.var_2) == (rb_state7.var_1));
            pthread_mutex_unlock(&(rb_state7.lock_13));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (0x955b3f6e);
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0x956c914e)) {
            pthread_mutex_lock(&(rb_state10.lock_12));
            rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_3) == (0x0)) ? (rb_state10.var_3) : (0xed89d2cf));
            rb_state10.var_4 = (rb_state10.var_4) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_4) : (0x93337138));
            rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_2) : (0xe2e5ee6d));
            rb_state10.var_6 = (rb_state10.var_6) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_3) : (0xb5fb3ba0));
            rb_state10.var_7 = (rb_state10.var_7) + (((0x83656bab) - (rb_state10.var_4)) - (0x7e86adc5));
            rb_state10.var_8 = (rb_state10.var_8) - (((0xb8e74798) + (rb_state10.var_5)) ^ (0xfb278641));
            rb_state10.var_9 = (rb_state10.var_9) - (((0x3d3dcef1) ^ (rb_state10.var_6)) - (rb_state10.var_6));
            rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_8) : (0x3ab34fe1));
            rb_state10.var_11 = (rb_state10.var_11) ^ (((((0xd316996) + (0xdb8cbdb2)) + (rb_state10.var_9)) - (rb_state10.var_7)) ^ (rb_state10.var_10));
            rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_11);
            pthread_mutex_unlock(&(rb_state10.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + ((0xf36513ee) - (0xb3683a0e));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xa7f1c6e9));
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x81cb38ee)) {
            pthread_mutex_lock(&(rb_state13.lock_12));
            rb_state13.var_3 = 0xc6101c7a;
            rb_state13.var_4 = (rb_state13.var_4) - (rb_state13.var_2);
            rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_3);
            rb_state13.var_6 = (rb_state13.var_6) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_4) : (0xf198551f));
            rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_5) : (0x8136de99));
            rb_state13.var_8 = (rb_state13.var_8) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_6) : (0xebc41c5));
            rb_state13.var_9 = (rb_state13.var_9) - (rb_state13.var_7);
            rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0xa1af03cd));
            rb_state13.var_11 = (rb_state13.var_11) ^ (((((0x897d1e59) ^ (rb_state13.var_10)) + (0xbcf05f88)) - (0xfae3ac2c)) + (rb_state13.var_9));
            rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_11);
            pthread_mutex_unlock(&(rb_state13.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) ^ (93896 < rb_input_size ? (uint32_t)rb_input[93896] : 0x5e49e490);
        if ((rb_state17.var_0) == (0xffffffba)) {
            rb_state17.var_7 = (rb_state17.var_7) - (rb_state17.var_5);
            rb_state17.var_8 = (rb_state17.var_8) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_3) : (0x5114626d));
            rb_state17.var_9 = (rb_state17.var_9) ^ (rb_state17.var_4);
            rb_state17.var_10 = (rb_state17.var_10) + (rb_state17.var_7);
            rb_state17.var_11 = (rb_state17.var_11) - (((0x1a57d710) - (rb_state17.var_8)) + (0xab934048));
            rb_state17.var_12 = (rb_state17.var_12) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_10) : (0xe5f73e2c));
            rb_state17.var_13 = (rb_state17.var_13) + (((((0x6aa8d00e) - (0x7cc0fb9)) + (0x8e3db2f2)) ^ (rb_state17.var_12)) ^ (rb_state17.var_11));
            rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0xd13a21ae)) {
            pthread_mutex_lock(&(rb_state18.lock_16));
            rb_state18.var_3 = 0xc18728af;
            rb_state18.var_4 = (rb_state18.var_4) + (((((0xf20c34ef) - (rb_state18.var_2)) ^ (0xf900cf4a)) + (0xd56b226)) - (rb_state18.var_3));
            rb_state18.var_5 = (rb_state18.var_5) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_4) : (0x41e929b1));
            rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_5);
            rb_state18.var_1 = rb_state18.var_2;
            pthread_mutex_unlock(&(rb_state18.lock_16));
        }
        if ((rb_state18.var_0) == (0xd13a21ae)) {
            if (!((rb_state18.var_1) == (rb_state18.var_2))) {
                racebench_trigger(18);
            }
        }
        #endif
        AdvanceFrameMT(targs->tid);
    }

    return NULL;
}
#else

void *AdvanceFramesMT(void *args) {
    thread_args *targs = (thread_args *)args;

#if 1
    while (1)
#else
    for (int i = 0; i < targs->frames; ++i)
#endif
    {
        pthread_barrier_wait(&visualization_barrier);

        AdvanceFrameMT(targs->tid);
        pthread_barrier_wait(&visualization_barrier);
    }

    return NULL;
}

void AdvanceFrameVisualization() {

    pthread_barrier_wait(&visualization_barrier);

    pthread_barrier_wait(&visualization_barrier);
}
#endif

int main(int argc, char *argv[]) {
#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    std::cout << "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) << std::endl << std::flush;
#else
    std::cout << "PARSEC Benchmark Suite" << std::endl << std::flush;
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_fluidanimate);
#endif

    if (argc < 4 || argc >= 6) {
        std::cout << "Usage: " << argv[0] << " <threadnum> <framenum> <.fluid input file> [.fluid output file]" << std::endl;
        return -1;
    }

    int threadnum = atoi(argv[1]);
    int framenum = atoi(argv[2]);

    if (threadnum < 1) {
        std::cerr << "<threadnum> must at least be 1" << std::endl;
        return -1;
    }
    if (framenum < 1) {
        std::cerr << "<framenum> must at least be 1" << std::endl;
        return -1;
    }

#ifdef ENABLE_CFL_CHECK
    std::cout << "WARNING: Check for Courant–Friedrichs–Lewy condition enabled. Do not use for performance measurements." << std::endl;
#endif

    InitSim(argv[3], threadnum);
#ifdef ENABLE_VISUALIZATION
    InitVisualizationMode(&argc, argv, &AdvanceFrameVisualization, &numCells, &cells, &cnumPars);
#endif

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif
#if defined(WIN32)
    thread_args *targs = (thread_args *)alloca(sizeof(thread_args) * threadnum);
#else
    thread_args targs[threadnum];
#endif
    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xdf8561ae));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ ((0x2fc1f0d0) + (rb_state7.var_0));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - ((0x82bb1ec5) + (0xc7553b4e));
        #endif
        targs[i].tid = i;
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_8 = (rb_state7.var_8) + (0x6a783987);
        if ((rb_state7.var_9) == (0x5f49e3cc)) {
            rb_state7.var_26 = (rb_state7.var_26) ^ (((((0x454a164a) ^ (rb_state7.var_19)) ^ (0x563c0e7c)) + (rb_state7.var_16)) + (rb_state7.var_18));
            rb_state7.var_27 = (rb_state7.var_27) - (77259 < rb_input_size ? (uint32_t)rb_input[77259] : 0xf398ba6a);
            rb_state7.var_28 = (rb_state7.var_28) - (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_17) : (0xa292c691));
            rb_state7.var_29 = (rb_state7.var_29) + (((((0x53172934) ^ (rb_state7.var_27)) ^ (rb_state7.var_26)) - (rb_state7.var_21)) ^ (0xf70e7ddc));
            rb_state7.var_30 = (rb_state7.var_30) - (rb_state7.var_28);
            rb_state7.var_31 = (rb_state7.var_31) - (((((0x59b38ac9) - (0x623ffa8e)) ^ (rb_state7.var_29)) ^ (rb_state7.var_30)) ^ (rb_state7.var_22));
            rb_state7.var_25 = (rb_state7.var_25) + (rb_state7.var_31);
            rb_state7.var_15 = rb_state7.var_25;
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + ((0x5a75298c) - (rb_state12.var_0));
        if ((rb_state12.var_0) == (0x5a75298c)) {
            rb_state12.var_11 = (rb_state12.var_11) ^ (((0x9c397837) - (rb_state12.var_4)) ^ (rb_state12.var_7));
            rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_6) : (0xebba1669));
            rb_state12.var_13 = (rb_state12.var_13) - (rb_state12.var_11);
            rb_state12.var_14 = (rb_state12.var_14) ^ (((((0xd265e1d) - (rb_state12.var_8)) - (0xe35ba5ce)) - (rb_state12.var_13)) ^ (rb_state12.var_12));
            rb_state12.var_10 = (rb_state12.var_10) - (rb_state12.var_14);
            rb_state12.var_1 = rb_state12.var_10;
        }
        if ((rb_state12.var_0) == (0x5a75298c)) {
            pthread_mutex_lock(&(rb_state12.lock_34));
            rb_state12.var_16 = (rb_state12.var_16) + (rb_state12.var_12);
            rb_state12.var_17 = (rb_state12.var_17) ^ (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_9) : (0xb9300f32));
            rb_state12.var_18 = (rb_state12.var_18) - (88946 < rb_input_size ? (uint32_t)rb_input[88946] : 0x7626f686);
            rb_state12.var_19 = (rb_state12.var_19) ^ (rb_state12.var_14);
            rb_state12.var_20 = (rb_state12.var_20) + (((0xc0d40767) + (0x7a10b4d2)) + (rb_state12.var_10));
            rb_state12.var_21 = (rb_state12.var_21) - (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_16) : (0xce7d2c04));
            rb_state12.var_22 = (rb_state12.var_22) + (((((0x25e76bff) - (rb_state12.var_16)) + (rb_state12.var_15)) ^ (rb_state12.var_18)) - (rb_state12.var_17));
            rb_state12.var_23 = (rb_state12.var_23) + (((0x7c4aa68e) + (rb_state12.var_17)) - (rb_state12.var_19));
            rb_state12.var_24 = (rb_state12.var_24) + (((0x1e867eaa) + (rb_state12.var_20)) ^ (rb_state12.var_18));
            rb_state12.var_25 = (rb_state12.var_25) + (rb_state12.var_21);
            rb_state12.var_26 = (rb_state12.var_26) - (((rb_state12.var_19) == (0x0)) ? (rb_state12.var_22) : (0xc207cb3e));
            rb_state12.var_27 = (rb_state12.var_27) - (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_23) : (0xad7015c9));
            rb_state12.var_28 = (rb_state12.var_28) + (((rb_state12.var_21) == (0x0)) ? (rb_state12.var_24) : (0x7bb037e8));
            rb_state12.var_29 = (rb_state12.var_29) - (((rb_state12.var_25) == (0x0)) ? (rb_state12.var_26) : (0x1e05bcae));
            rb_state12.var_30 = (rb_state12.var_30) - (((0x6076defd) + (0x39b5ce51)) - (rb_state12.var_27));
            rb_state12.var_31 = (rb_state12.var_31) ^ (rb_state12.var_28);
            rb_state12.var_32 = (rb_state12.var_32) - (((((0x8110d98e) + (rb_state12.var_30)) + (rb_state12.var_29)) + (rb_state12.var_22)) - (0x23247395));
            rb_state12.var_33 = (rb_state12.var_33) + (((((0xd28c1a0a) ^ (rb_state12.var_32)) ^ (rb_state12.var_23)) + (rb_state12.var_24)) ^ (rb_state12.var_31));
            rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_33);
            rb_state12.var_1 = rb_state12.var_15;
            pthread_mutex_unlock(&(rb_state12.lock_34));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ ((0x81cb38ee) ^ (rb_state13.var_0));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0x4b2d822) - (rb_state15.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0xd13a21ae)) {
            rb_state18.var_6 = (rb_state18.var_6) - (((0x31810978) + (rb_state18.var_4)) + (rb_state18.var_6));
            rb_state18.var_7 = (rb_state18.var_7) + ((0x9c8b1392) ^ (rb_state18.var_7));
            rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_8) : (0x9e01c18b));
            rb_state18.var_9 = (rb_state18.var_9) - (0x28b64871);
            rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_3) == (0x0)) ? (rb_state18.var_5) : (0x508e55f7));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_7) : (0x7df8d503));
            rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_8);
            rb_state18.var_13 = (rb_state18.var_13) + (((((0xdd6cf1f8) - (0xff983836)) - (rb_state18.var_10)) ^ (0x2a0805ae)) - (rb_state18.var_9));
            rb_state18.var_14 = (rb_state18.var_14) - (((((0x1120023c) - (rb_state18.var_11)) + (0x41fddb53)) + (rb_state18.var_12)) + (rb_state18.var_9));
            rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_14) : (0xebdc52d8));
            rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_15);
        }
        #endif
        targs[i].frames = framenum;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + ((0x793ac6e6) - (0x859d0907));
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x207a9cd8)) {
            rb_state5.var_5 = 0xf818e1d7;
            rb_state5.var_6 = 0xa1c11a50;
            rb_state5.var_7 = (rb_state5.var_7) ^ (rb_state5.var_5);
            rb_state5.var_8 = (rb_state5.var_8) - (((0x34c3a0a3) + (0xa92fce06)) ^ (rb_state5.var_6));
            rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0xd4694acb));
            rb_state5.var_10 = (rb_state5.var_10) - (((((0x270dd2e8) + (rb_state5.var_7)) - (rb_state5.var_8)) + (rb_state5.var_5)) ^ (rb_state5.var_6));
            rb_state5.var_11 = (rb_state5.var_11) ^ (rb_state5.var_9);
            rb_state5.var_12 = (rb_state5.var_12) + (rb_state5.var_10);
            rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_11) : (0x23fd5e8c));
            rb_state5.var_14 = (rb_state5.var_14) - (((((0x5c893a4e) ^ (rb_state5.var_13)) + (rb_state5.var_12)) - (rb_state5.var_8)) + (0x89d23de5));
            rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (0x43805ab5);
        if ((rb_state7.var_0) == (0xb3c26be5)) {
            pthread_mutex_lock(&(rb_state7.lock_13));
            rb_state7.var_3 = 0x33b0d783;
            rb_state7.var_4 = (rb_state7.var_4) ^ (((0x1000f610) ^ (rb_state7.var_3)) ^ (rb_state7.var_3));
            rb_state7.var_5 = (rb_state7.var_5) - (134657 < rb_input_size ? (uint32_t)rb_input[134657] : 0x492a684b);
            rb_state7.var_6 = (rb_state7.var_6) + (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_1) : (0xde80da3c));
            rb_state7.var_7 = (rb_state7.var_7) + (((0xac0ed4dd) - (0xfe296e13)) - (rb_state7.var_2));
            rb_state7.var_8 = (rb_state7.var_8) ^ (((0x99c5392) - (rb_state7.var_4)) + (0xaafaf7da));
            rb_state7.var_9 = (rb_state7.var_9) - (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_5) : (0xa0b61c34));
            rb_state7.var_10 = (rb_state7.var_10) ^ (((((0x48f617ba) - (rb_state7.var_6)) - (rb_state7.var_6)) ^ (rb_state7.var_7)) ^ (0xd07236f4));
            rb_state7.var_11 = (rb_state7.var_11) ^ (((((0x6c10e54f) - (0x5a2188d3)) - (rb_state7.var_7)) - (rb_state7.var_8)) - (rb_state7.var_9));
            rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0x2f593dc9));
            rb_state7.var_1 = (rb_state7.var_1) - (rb_state7.var_12);
            rb_state7.var_2 = rb_state7.var_1;
            pthread_mutex_unlock(&(rb_state7.lock_13));
        }
        if ((rb_state7.var_9) == (0x5f49e3cc)) {
            pthread_mutex_lock(&(rb_state7.lock_41));
            rb_state7.var_33 = 0x41a8f9a;
            rb_state7.var_34 = (rb_state7.var_34) - (((0x7509271b) + (0x42df8a0f)) + (rb_state7.var_33));
            rb_state7.var_35 = (rb_state7.var_35) ^ (((0x1d5496d0) + (rb_state7.var_23)) ^ (0xacfaf561));
            rb_state7.var_36 = (rb_state7.var_36) + (((rb_state7.var_25) == (0x0)) ? (rb_state7.var_24) : (0xe9b43c3e));
            rb_state7.var_37 = (rb_state7.var_37) - (((((0x82a61cd1) ^ (rb_state7.var_35)) ^ (rb_state7.var_34)) - (0x86423701)) - (0x9c0a0e94));
            rb_state7.var_38 = (rb_state7.var_38) ^ (rb_state7.var_36);
            rb_state7.var_39 = (rb_state7.var_39) + (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_37) : (0xec0d32f0));
            rb_state7.var_40 = (rb_state7.var_40) - (((rb_state7.var_38) == (0x0)) ? (rb_state7.var_39) : (0x2f1dc004));
            rb_state7.var_32 = (rb_state7.var_32) - (rb_state7.var_40);
            rb_state7.var_15 = rb_state7.var_32;
            pthread_mutex_unlock(&(rb_state7.lock_41));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0x0)) {
            rb_state8.var_6 = 0x2ee403d8;
            rb_state8.var_7 = 0xd13d1728;
            rb_state8.var_8 = (rb_state8.var_8) - ((0x787fb061) ^ (rb_state8.var_6));
            rb_state8.var_9 = (rb_state8.var_9) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_6) : (0xbd472608));
            rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_8) : (0x4de0b51b));
            rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_9);
            rb_state8.var_12 = (rb_state8.var_12) ^ (rb_state8.var_10);
            rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_11) : (0xa5bd831c));
            rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_12) : (0xb2d08f58));
            rb_state8.var_15 = (rb_state8.var_15) - (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_14) : (0xfff1eb7d));
            rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xf6fefa09));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (0x5e0eeaf7);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + ((0xd1818dde) ^ (rb_state18.var_0));
        #endif
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0x81cb38ee)) {
            rb_state13.var_2 = rb_state13.var_1;
        }
        if ((rb_state13.var_0) == (0x81cb38ee)) {
            if (!((rb_state13.var_1) == (rb_state13.var_2))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0xa6a3ed2b)) {
            rb_state15.var_7 = 0x2e02cd2f;
            rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_7) == (0x2e02cd2f)) ? (rb_state15.var_6) : (0x9f428ba7));
            rb_state15.var_9 = (rb_state15.var_9) ^ (0x55a42207);
            rb_state15.var_10 = (rb_state15.var_10) - (0xa82b4a50);
            rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_7) == (0x2e02cd2f)) ? (rb_state15.var_5) : (0xa0049fd9));
            rb_state15.var_12 = (rb_state15.var_12) + (((0x8603a747) + (rb_state15.var_8)) - (rb_state15.var_8));
            rb_state15.var_13 = (rb_state15.var_13) - (((0xca504dd9) - (rb_state15.var_9)) + (rb_state15.var_9));
            rb_state15.var_14 = (rb_state15.var_14) - (((0x53388aa) + (0x6b93587d)) + (rb_state15.var_10));
            rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_11) == (0xeceef49f)) ? (rb_state15.var_12) : (0x6a6a2c49));
            rb_state15.var_16 = (rb_state15.var_16) + (rb_state15.var_13);
            rb_state15.var_17 = (rb_state15.var_17) - (((0xeddd1eef) + (0x80842cc0)) + (rb_state15.var_14));
            rb_state15.var_18 = (rb_state15.var_18) ^ (((rb_state15.var_10) == (0x57d4b5b0)) ? (rb_state15.var_15) : (0xd1cc5ac8));
            rb_state15.var_19 = (rb_state15.var_19) - (((0x566e350d) - (rb_state15.var_16)) ^ (0xb3c3d36f));
            rb_state15.var_20 = (rb_state15.var_20) ^ (((((0x1284236e) - (0xd49025f1)) - (rb_state15.var_18)) - (rb_state15.var_17)) + (rb_state15.var_11));
            rb_state15.var_21 = (rb_state15.var_21) + (rb_state15.var_19);
            rb_state15.var_22 = (rb_state15.var_22) + (rb_state15.var_20);
            rb_state15.var_23 = (rb_state15.var_23) - (rb_state15.var_21);
            rb_state15.var_24 = (rb_state15.var_24) + (((0x8e73eeb2) ^ (0x71c61a27)) + (rb_state15.var_22));
            rb_state15.var_25 = (rb_state15.var_25) ^ (((0xd4aa8e4) + (rb_state15.var_23)) ^ (0x3dd8939d));
            rb_state15.var_26 = (rb_state15.var_26) + (rb_state15.var_24);
            rb_state15.var_27 = (rb_state15.var_27) + (((rb_state15.var_25) == (0x9d1f69f0)) ? (rb_state15.var_26) : (0x947a6fe6));
            rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_27);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0xffffffb8)) {
            pthread_mutex_lock(&(rb_state17.lock_14));
            rb_state17.var_3 = 0xf16fd450;
            rb_state17.var_4 = 0xf7c61e61;
            rb_state17.var_5 = (rb_state17.var_5) ^ (((((0x8b98740f) ^ (0xcd9e0f47)) - (rb_state17.var_2)) ^ (rb_state17.var_1)) + (rb_state17.var_3));
            rb_state17.var_6 = (rb_state17.var_6) + (((rb_state17.var_4) == (0xf7c61e61)) ? (rb_state17.var_5) : (0x2557c692));
            rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_6);
            rb_state17.var_1 = rb_state17.var_2;
            pthread_mutex_unlock(&(rb_state17.lock_14));
        }
        if ((rb_state17.var_0) == (0xffffffba)) {
            if (!((rb_state17.var_1) == (rb_state17.var_2))) {
                racebench_trigger(17);
            }
        }
        #endif
        pthread_join(thread[i], NULL);
    }
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    if (argc > 4) {
        SaveFile(argv[4]);
    }
    CleanUpSim();

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif

    return 0;
}