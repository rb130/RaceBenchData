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
                #ifdef RACEBENCH_BUG_0
                if ((rb_state0.var_0) == (0x1c3f91e0)) {
                    if ((rb_state0.var_1) != (0x0)) {
                        if (!((rb_state0.var_1) == (rb_state0.var_11))) {
                            racebench_trigger(0);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_3
                if ((rb_state3.var_0) == (0x2585a)) {
                    pthread_mutex_lock(&(rb_state3.lock_37));
                    rb_state3.var_21 = 0xde41e7d;
                    rb_state3.var_22 = (rb_state3.var_22) ^ ((0xb3db05da) + (rb_state3.var_11));
                    rb_state3.var_23 = (rb_state3.var_23) + ((0xc1c33cb0) - (rb_state3.var_12));
                    rb_state3.var_24 = (rb_state3.var_24) - (0xb82223a3);
                    rb_state3.var_25 = (rb_state3.var_25) + (((0xc3c5e497) + (0xb3b7124e)) - (rb_state3.var_21));
                    rb_state3.var_26 = (rb_state3.var_26) + (rb_state3.var_10);
                    rb_state3.var_27 = (rb_state3.var_27) + (((((0x3d7aac19) - (rb_state3.var_13)) + (0xea75e458)) ^ (rb_state3.var_9)) + (rb_state3.var_22));
                    rb_state3.var_28 = (rb_state3.var_28) ^ (((rb_state3.var_23) == (0x0)) ? (rb_state3.var_24) : (0x7945868c));
                    rb_state3.var_29 = (rb_state3.var_29) - (((0xe0f10b6c) + (rb_state3.var_14)) ^ (rb_state3.var_25));
                    rb_state3.var_30 = (rb_state3.var_30) + (((rb_state3.var_26) == (0x0)) ? (rb_state3.var_27) : (0xef62d9d3));
                    rb_state3.var_31 = (rb_state3.var_31) + (((rb_state3.var_28) == (0x0)) ? (rb_state3.var_29) : (0x6db64ab2));
                    rb_state3.var_32 = (rb_state3.var_32) - (rb_state3.var_30);
                    rb_state3.var_33 = (rb_state3.var_33) ^ (((rb_state3.var_31) == (0x0)) ? (rb_state3.var_32) : (0x351f1f5));
                    rb_state3.var_20 = (rb_state3.var_20) - (rb_state3.var_33);
                    rb_state3.var_1 = rb_state3.var_20;
                    pthread_mutex_unlock(&(rb_state3.lock_37));
                }
                if ((rb_state3.var_0) == (0x2585a)) {
                    pthread_mutex_lock(&(rb_state3.lock_37));
                    if ((rb_state3.var_1) != (0x0)) {
                        if (!((rb_state3.var_1) == (rb_state3.var_34))) {
                            racebench_trigger(3);
                        }
                    }
                    pthread_mutex_unlock(&(rb_state3.lock_37));
                }
                if ((rb_state3.var_0) == (0x2585a)) {
                    pthread_mutex_lock(&(rb_state3.lock_37));
                    rb_state3.var_35 = (rb_state3.var_35) - (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_17) : (0xf30c8cfd));
                    rb_state3.var_36 = (rb_state3.var_36) ^ (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_35) : (0x6d2e599d));
                    rb_state3.var_34 = (rb_state3.var_34) - (rb_state3.var_36);
                    rb_state3.var_1 = rb_state3.var_34;
                    pthread_mutex_unlock(&(rb_state3.lock_37));
                }
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0x1bad668a));
                if ((rb_state4.var_0) == (0x3a47eb12)) {
                    rb_state4.var_3 = (rb_state4.var_3) - (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_2) : (0x9885591));
                    rb_state4.var_4 = (rb_state4.var_4) + (((((0xd471d62b) + (rb_state4.var_1)) ^ (0x784a779d)) - (rb_state4.var_3)) ^ (0x9d6f9b6c));
                    rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_4);
                }
                #endif
                #ifdef RACEBENCH_BUG_5
                if ((rb_state5.var_0) == (0x66c8c)) {
                    pthread_mutex_lock(&(rb_state5.lock_16));
                    rb_state5.var_7 = 0x8d6be6a6;
                    rb_state5.var_8 = (rb_state5.var_8) ^ (((0xb44db8ba) ^ (rb_state5.var_4)) ^ (0x43b4666d));
                    rb_state5.var_9 = (rb_state5.var_9) ^ (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_7) : (0x5a647e4b));
                    rb_state5.var_10 = (rb_state5.var_10) - ((0x88cdcfe3) + (0xea39624f));
                    rb_state5.var_11 = (rb_state5.var_11) - (rb_state5.var_8);
                    rb_state5.var_12 = (rb_state5.var_12) ^ (((0x90a0817f) ^ (rb_state5.var_6)) + (rb_state5.var_9));
                    rb_state5.var_13 = (rb_state5.var_13) - (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_11) : (0x28ed9b73));
                    rb_state5.var_14 = (rb_state5.var_14) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_12) : (0xedcbe11c));
                    rb_state5.var_15 = (rb_state5.var_15) - (((((0xc3897113) ^ (rb_state5.var_14)) + (0xe2d3e0e)) + (0xc845b378)) - (rb_state5.var_13));
                    rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_15);
                    pthread_mutex_unlock(&(rb_state5.lock_16));
                }
                #endif
                #ifdef RACEBENCH_BUG_6
                if ((rb_state6.var_0) == (0x83f6d221)) {
                    pthread_mutex_lock(&(rb_state6.lock_30));
                    rb_state6.var_16 = 0x1845551b;
                    rb_state6.var_17 = 0x7913face;
                    rb_state6.var_18 = (rb_state6.var_18) - (97303 < rb_input_size ? (uint32_t)rb_input[97303] : 0x5f0a11b7);
                    rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_9) : (0xa6b03225));
                    rb_state6.var_20 = (rb_state6.var_20) + (((0xb3460514) ^ (rb_state6.var_8)) ^ (rb_state6.var_11));
                    rb_state6.var_21 = (rb_state6.var_21) - (rb_state6.var_17);
                    rb_state6.var_22 = (rb_state6.var_22) - (rb_state6.var_16);
                    rb_state6.var_23 = (rb_state6.var_23) + (rb_state6.var_18);
                    rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_19) == (0x0)) ? (rb_state6.var_20) : (0x12616bef));
                    rb_state6.var_25 = (rb_state6.var_25) - (((0xbec0a25b) + (rb_state6.var_12)) ^ (rb_state6.var_21));
                    rb_state6.var_26 = (rb_state6.var_26) + (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_22) : (0xc0c852a));
                    rb_state6.var_27 = (rb_state6.var_27) + (((rb_state6.var_23) == (0x0)) ? (rb_state6.var_24) : (0xbbb0d10e));
                    rb_state6.var_28 = (rb_state6.var_28) - (((rb_state6.var_25) == (0x0)) ? (rb_state6.var_26) : (0xc74bd149));
                    rb_state6.var_29 = (rb_state6.var_29) ^ (((rb_state6.var_27) == (0x0)) ? (rb_state6.var_28) : (0xb208b94c));
                    rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_29);
                    pthread_mutex_unlock(&(rb_state6.lock_30));
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) + (0xfef33dd3);
                if ((rb_state8.var_0) == (0x2360822c)) {
                    pthread_mutex_lock(&(rb_state8.lock_36));
                    rb_state8.var_3 = 0x2abe9ba3;
                    rb_state8.var_4 = (rb_state8.var_4) + (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0x627a2293));
                    rb_state8.var_5 = (rb_state8.var_5) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_4) : (0x251a66a5));
                    rb_state8.var_6 = (rb_state8.var_6) - (((((0xa155620d) - (rb_state8.var_6)) - (rb_state8.var_3)) - (0x790b9af8)) ^ (rb_state8.var_1));
                    rb_state8.var_7 = (rb_state8.var_7) + (((0x3405c75b) ^ (0xbd59f225)) - (rb_state8.var_4));
                    rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0xc535bfa4));
                    rb_state8.var_9 = (rb_state8.var_9) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_7) : (0x7876c56f));
                    rb_state8.var_10 = (rb_state8.var_10) + (rb_state8.var_8);
                    rb_state8.var_11 = (rb_state8.var_11) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_9) : (0xfce0866b));
                    rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x2731e2ea));
                    rb_state8.var_13 = (rb_state8.var_13) + (rb_state8.var_11);
                    rb_state8.var_14 = (rb_state8.var_14) + (rb_state8.var_12);
                    rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_13) : (0xfb9e695d));
                    rb_state8.var_16 = (rb_state8.var_16) ^ (rb_state8.var_14);
                    rb_state8.var_17 = (rb_state8.var_17) + (rb_state8.var_15);
                    rb_state8.var_18 = (rb_state8.var_18) ^ (((0x91d83df7) - (rb_state8.var_16)) - (rb_state8.var_11));
                    rb_state8.var_19 = (rb_state8.var_19) + (((0x3d9f72ef) ^ (0x4af29908)) ^ (rb_state8.var_17));
                    rb_state8.var_20 = (rb_state8.var_20) ^ (((0x4c68d340) - (rb_state8.var_18)) ^ (rb_state8.var_12));
                    rb_state8.var_21 = (rb_state8.var_21) + (((((0x23a9b21f) ^ (0xa41c6fa6)) - (rb_state8.var_20)) - (rb_state8.var_19)) + (rb_state8.var_13));
                    rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_21);
                    rb_state8.var_1 = rb_state8.var_2;
                    pthread_mutex_unlock(&(rb_state8.lock_36));
                }
                if ((rb_state8.var_0) == (0x2360822c)) {
                    pthread_mutex_lock(&(rb_state8.lock_36));
                    if (!((rb_state8.var_1) == (rb_state8.var_2))) {
                        racebench_trigger(8);
                    }
                    pthread_mutex_unlock(&(rb_state8.lock_36));
                }
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) - (0x70d75d26);
                if ((rb_state10.var_0) == (0xa5de5e2a)) {
                    if ((rb_state10.var_1) != (0x0)) {
                        if (!((rb_state10.var_1) == (rb_state10.var_21))) {
                            racebench_trigger(10);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_0) == (0xafef7562)) {
                    rb_state12.var_22 = (rb_state12.var_22) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0x45f19f10));
                    rb_state12.var_23 = (rb_state12.var_23) + (((((0xfcadf2b7) ^ (rb_state12.var_22)) - (0xd6a07d9d)) - (0x7373a051)) + (rb_state12.var_13));
                    rb_state12.var_21 = (rb_state12.var_21) - (rb_state12.var_23);
                    rb_state12.var_1 = rb_state12.var_21;
                }
                if ((rb_state12.var_0) == (0xafef7562)) {
                    pthread_mutex_lock(&(rb_state12.lock_29));
                    rb_state12.var_25 = 0x2c0abaa5;
                    rb_state12.var_26 = (rb_state12.var_26) ^ (((0xa5f894f0) - (rb_state12.var_18)) ^ (rb_state12.var_25));
                    rb_state12.var_27 = (rb_state12.var_27) ^ (((((0xd959f8fe) + (rb_state12.var_19)) - (rb_state12.var_16)) + (rb_state12.var_20)) ^ (rb_state12.var_17));
                    rb_state12.var_28 = (rb_state12.var_28) - (((((0xc76e4413) - (0xfd366f64)) ^ (rb_state12.var_21)) ^ (rb_state12.var_27)) + (rb_state12.var_26));
                    rb_state12.var_24 = (rb_state12.var_24) + (rb_state12.var_28);
                    rb_state12.var_1 = rb_state12.var_24;
                    pthread_mutex_unlock(&(rb_state12.lock_29));
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) + (66998 < rb_input_size ? (uint32_t)rb_input[66998] : 0xfe9506ce);
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) - (103614 < rb_input_size ? (uint32_t)rb_input[103614] : 0x61710461);
                #endif
                #ifdef RACEBENCH_BUG_16
                if ((rb_state16.var_0) == (0x859fbc18)) {
                    pthread_mutex_lock(&(rb_state16.lock_27));
                    rb_state16.var_3 = 0x7a6d093;
                    rb_state16.var_4 = 0x8c2e0a04;
                    rb_state16.var_5 = (rb_state16.var_5) + (rb_state16.var_4);
                    rb_state16.var_6 = (rb_state16.var_6) ^ (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_3) : (0xdafbfebf));
                    rb_state16.var_7 = (rb_state16.var_7) ^ (rb_state16.var_3);
                    rb_state16.var_8 = (rb_state16.var_8) ^ (rb_state16.var_2);
                    rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_5);
                    rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_6) : (0x78bfd05));
                    rb_state16.var_11 = (rb_state16.var_11) ^ (((0x6783ee4a) ^ (rb_state16.var_7)) ^ (0x9eda38dd));
                    rb_state16.var_12 = (rb_state16.var_12) + (rb_state16.var_8);
                    rb_state16.var_13 = (rb_state16.var_13) - (((0x547e6d28) + (rb_state16.var_9)) ^ (rb_state16.var_6));
                    rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_10) : (0xc78dbb91));
                    rb_state16.var_15 = (rb_state16.var_15) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_11) : (0x85cfbf80));
                    rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_12) : (0x7b215edd));
                    rb_state16.var_17 = (rb_state16.var_17) + (((0x49211a41) - (rb_state16.var_13)) - (rb_state16.var_10));
                    rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_15) : (0x773d0039));
                    rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_16) : (0x69bf5413));
                    rb_state16.var_20 = (rb_state16.var_20) - (((0x79b697fd) + (rb_state16.var_12)) ^ (rb_state16.var_17));
                    rb_state16.var_21 = (rb_state16.var_21) - (((0xf73c8e24) - (rb_state16.var_18)) - (0x78325903));
                    rb_state16.var_22 = (rb_state16.var_22) ^ (rb_state16.var_19);
                    rb_state16.var_23 = (rb_state16.var_23) - (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_20) : (0xb23bf642));
                    rb_state16.var_24 = (rb_state16.var_24) ^ (rb_state16.var_21);
                    rb_state16.var_25 = (rb_state16.var_25) ^ (((((0xd12bd6b8) + (rb_state16.var_23)) + (0xd094f06a)) ^ (rb_state16.var_14)) + (rb_state16.var_22));
                    rb_state16.var_26 = (rb_state16.var_26) - (((((0xcc286c88) - (rb_state16.var_15)) + (0x75a41c8d)) - (rb_state16.var_25)) + (rb_state16.var_24));
                    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_26);
                    pthread_mutex_unlock(&(rb_state16.lock_27));
                }
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) - (0xa4c4e416);
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_0 = (rb_state18.var_0) ^ (9538 < rb_input_size ? (uint32_t)rb_input[9538] : 0x7e0b3593);
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) ^ (0x9fffeb16);
                if ((rb_state19.var_0) == (0x84099c32)) {
                    pthread_mutex_lock(&(rb_state19.lock_17));
                    rb_state19.var_3 = 0xfa6855a6;
                    rb_state19.var_4 = 0xa3dc15d6;
                    rb_state19.var_5 = (rb_state19.var_5) ^ (((0x5325786a) + (rb_state19.var_3)) + (0x52a7ca80));
                    rb_state19.var_6 = (rb_state19.var_6) - (rb_state19.var_4);
                    rb_state19.var_7 = (rb_state19.var_7) - (rb_state19.var_1);
                    rb_state19.var_8 = (rb_state19.var_8) - (((((0x58a3c51f) - (rb_state19.var_6)) ^ (rb_state19.var_5)) ^ (0x4a43e446)) - (0xf2f4b96c));
                    rb_state19.var_9 = (rb_state19.var_9) - (((((0xd525207f) - (rb_state19.var_8)) ^ (0x3e1f40e7)) + (rb_state19.var_2)) + (rb_state19.var_7));
                    rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_9);
                    rb_state19.var_1 = rb_state19.var_2;
                    pthread_mutex_unlock(&(rb_state19.lock_17));
                }
                if ((rb_state19.var_0) == (0x1bf67724)) {
                    pthread_mutex_lock(&(rb_state19.lock_17));
                    rb_state19.var_10 = 0x38e8f187;
                    rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_10) : (0x2175cc5c));
                    rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_3);
                    rb_state19.var_13 = (rb_state19.var_13) + (((((0x69b7a8d9) + (rb_state19.var_4)) ^ (rb_state19.var_11)) + (0x8d5f18a9)) ^ (rb_state19.var_6));
                    rb_state19.var_14 = (rb_state19.var_14) + (rb_state19.var_12);
                    rb_state19.var_15 = (rb_state19.var_15) + (rb_state19.var_13);
                    rb_state19.var_16 = (rb_state19.var_16) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0xc4702340));
                    rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_16);
                    pthread_mutex_unlock(&(rb_state19.lock_17));
                }
                if ((rb_state19.var_0) == (0x1bf67724)) {
                    if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                        racebench_trigger(19);
                    }
                }
                #endif
                int index = (iz * ny + iy) * nx + ix;
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xd7c66285));
                if ((rb_state1.var_0) == (0x359e5413)) {
                    rb_state1.var_14 = 0xbb8ba8ad;
                    rb_state1.var_15 = 0x43a0d779;
                    rb_state1.var_16 = (rb_state1.var_16) + (rb_state1.var_9);
                    rb_state1.var_17 = (rb_state1.var_17) - (((((0xebacc730) ^ (rb_state1.var_11)) + (rb_state1.var_10)) - (rb_state1.var_15)) + (rb_state1.var_8));
                    rb_state1.var_18 = (rb_state1.var_18) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_14) : (0xdb938ba1));
                    rb_state1.var_19 = (rb_state1.var_19) ^ (rb_state1.var_16);
                    rb_state1.var_20 = (rb_state1.var_20) ^ (((0xb350d54d) - (rb_state1.var_17)) - (0x85c27e64));
                    rb_state1.var_21 = (rb_state1.var_21) - (((0x51c84216) ^ (rb_state1.var_13)) ^ (rb_state1.var_18));
                    rb_state1.var_22 = (rb_state1.var_22) ^ (((rb_state1.var_14) == (0x0)) ? (rb_state1.var_19) : (0x5aff08e4));
                    rb_state1.var_23 = (rb_state1.var_23) - (((0x52d18908) ^ (rb_state1.var_15)) - (rb_state1.var_20));
                    rb_state1.var_24 = (rb_state1.var_24) + (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_22) : (0xecc855b6));
                    rb_state1.var_25 = (rb_state1.var_25) ^ (rb_state1.var_23);
                    rb_state1.var_26 = (rb_state1.var_26) - (((0x378dae7e) + (rb_state1.var_16)) - (rb_state1.var_24));
                    rb_state1.var_27 = (rb_state1.var_27) + (((0xca583c43) ^ (0x119ab86a)) ^ (rb_state1.var_25));
                    rb_state1.var_28 = (rb_state1.var_28) + (((0x14d536aa) ^ (rb_state1.var_26)) - (rb_state1.var_17));
                    rb_state1.var_29 = (rb_state1.var_29) + (((0xecb7b139) + (rb_state1.var_18)) - (rb_state1.var_27));
                    rb_state1.var_30 = (rb_state1.var_30) - (((0xd12f2a24) - (0xaf23cd87)) + (rb_state1.var_28));
                    rb_state1.var_31 = (rb_state1.var_31) ^ (((((0x710e6ddf) + (rb_state1.var_29)) - (0xed4e4eaf)) + (rb_state1.var_30)) + (rb_state1.var_19));
                    rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_31);
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                rb_state2.var_0 = (rb_state2.var_0) + (((rb_state2.var_0) == (0xe1b183c0)) ? (rb_state2.var_0) : (0xed22a568));
                if ((rb_state2.var_0) == (0x1136a708)) {
                    rb_state2.var_16 = !((rb_state2.var_2) == (rb_state2.var_1));
                }
                rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_11) : (0x8a418f8d));
                #endif
                #ifdef RACEBENCH_BUG_3
                rb_state3.var_0 = (rb_state3.var_0) + (97868 < rb_input_size ? (uint32_t)rb_input[97868] : 0xc449afa0);
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x7ca4c83b));
                if ((rb_state5.var_0) == (0x66c8c)) {
                    pthread_mutex_lock(&(rb_state5.lock_16));
                    rb_state5.var_3 = 0x20b5ef17;
                    rb_state5.var_4 = 0xa1491996;
                    rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_1) : (0xca381be8));
                    rb_state5.var_6 = (rb_state5.var_6) - (((((0x37d56354) + (rb_state5.var_3)) ^ (rb_state5.var_3)) ^ (rb_state5.var_5)) + (rb_state5.var_2));
                    rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_6);
                    rb_state5.var_1 = rb_state5.var_2;
                    pthread_mutex_unlock(&(rb_state5.lock_16));
                }
                if ((rb_state5.var_0) == (0x66c8c)) {
                    if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                        racebench_trigger(5);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_8
                if ((rb_state8.var_0) == (0x2360822c)) {
                    rb_state8.var_22 = 0x5eb2f307;
                    rb_state8.var_23 = 0x769a3753;
                    rb_state8.var_24 = (rb_state8.var_24) ^ (((0x72d3ac9a) + (rb_state8.var_14)) - (0xa216af45));
                    rb_state8.var_25 = (rb_state8.var_25) + (((((0x93791c01) ^ (rb_state8.var_23)) - (rb_state8.var_22)) - (rb_state8.var_16)) + (rb_state8.var_15));
                    rb_state8.var_26 = (rb_state8.var_26) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_24) : (0xbdad9bba));
                    rb_state8.var_27 = (rb_state8.var_27) ^ (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_25) : (0x60dc9c44));
                    rb_state8.var_28 = (rb_state8.var_28) + (rb_state8.var_26);
                    rb_state8.var_29 = (rb_state8.var_29) - (rb_state8.var_27);
                    rb_state8.var_30 = (rb_state8.var_30) - (rb_state8.var_28);
                    rb_state8.var_31 = (rb_state8.var_31) + (((0xa12d897) ^ (rb_state8.var_19)) ^ (rb_state8.var_29));
                    rb_state8.var_32 = (rb_state8.var_32) ^ (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_30) : (0x125366cb));
                    rb_state8.var_33 = (rb_state8.var_33) + (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_31) : (0x75bee2c3));
                    rb_state8.var_34 = (rb_state8.var_34) - (rb_state8.var_32);
                    rb_state8.var_35 = (rb_state8.var_35) - (((rb_state8.var_33) == (0x0)) ? (rb_state8.var_34) : (0x8405fed5));
                    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_35);
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                rb_state12.var_0 = (rb_state12.var_0) + (0x9ec4e66a);
                #endif
                #ifdef RACEBENCH_BUG_13
                rb_state13.var_0 = (rb_state13.var_0) - ((0xfe08bb16) - (rb_state13.var_0));
                #endif
                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_0) == (0xfffbdfff)) {
                    pthread_mutex_lock(&(rb_state14.lock_13));
                    rb_state14.var_3 = 0x2a70189e;
                    rb_state14.var_4 = 0xc7058407;
                    rb_state14.var_5 = (rb_state14.var_5) ^ (((((0x7557ef3) - (rb_state14.var_2)) + (rb_state14.var_3)) - (rb_state14.var_4)) - (rb_state14.var_1));
                    rb_state14.var_6 = (rb_state14.var_6) ^ (((0xf46c9d35) + (rb_state14.var_3)) - (0x8ed12d99));
                    rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_6) : (0x671867b5));
                    rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_7);
                    rb_state14.var_1 = rb_state14.var_2;
                    pthread_mutex_unlock(&(rb_state14.lock_13));
                }
                if ((rb_state14.var_0) == (0xfffbdfff)) {
                    rb_state14.var_8 = 0x1e334d4f;
                    rb_state14.var_9 = (rb_state14.var_9) + (65626 < rb_input_size ? (uint32_t)rb_input[65626] : 0xa6d028ff);
                    rb_state14.var_10 = (rb_state14.var_10) - (((((0x2eee3577) + (rb_state14.var_8)) - (0xfaf6b7b0)) - (rb_state14.var_5)) ^ (0xb0569d5f));
                    rb_state14.var_11 = (rb_state14.var_11) + (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_9) : (0x76a0b35d));
                    rb_state14.var_12 = (rb_state14.var_12) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_11) : (0x6b7af201));
                    rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_12);
                }
                if ((rb_state14.var_0) == (0xfffbdfff)) {
                    if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                        racebench_trigger(14);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                if ((rb_state16.var_0) == (0x859fbc18)) {
                    pthread_mutex_lock(&(rb_state16.lock_27));
                    rb_state16.var_2 = rb_state16.var_1;
                    pthread_mutex_unlock(&(rb_state16.lock_27));
                }
                if ((rb_state16.var_0) == (0x859fbc18)) {
                    if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                        racebench_trigger(16);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0xb4)) ? (rb_state18.var_0) : (0xdef7dd29));
                #endif
                cnumPars[index] = 0;
                #ifdef RACEBENCH_BUG_1
                rb_state1.var_0 = (rb_state1.var_0) + ((0x29a9f02) + (rb_state1.var_0));
                if ((rb_state1.var_0) == (0x5dd7f18e)) {
                    pthread_mutex_lock(&(rb_state1.lock_32));
                    rb_state1.var_3 = 0xa60da645;
                    rb_state1.var_4 = (rb_state1.var_4) ^ (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_3) : (0x22f82a7a));
                    rb_state1.var_5 = (rb_state1.var_5) ^ (rb_state1.var_1);
                    rb_state1.var_6 = (rb_state1.var_6) ^ (116020 < rb_input_size ? (uint32_t)rb_input[116020] : 0xdba0ca04);
                    rb_state1.var_7 = (rb_state1.var_7) + (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_2) : (0xd7dff6f6));
                    rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_5) : (0x7b04d085));
                    rb_state1.var_9 = (rb_state1.var_9) ^ (rb_state1.var_6);
                    rb_state1.var_10 = (rb_state1.var_10) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0xb9a7dcaf));
                    rb_state1.var_11 = (rb_state1.var_11) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_9) : (0x2645bc26));
                    rb_state1.var_12 = (rb_state1.var_12) + (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_10) : (0x81d84fe3));
                    rb_state1.var_13 = (rb_state1.var_13) + (((((0xb21eb0f0) + (0x18467bc9)) + (rb_state1.var_11)) - (rb_state1.var_7)) - (rb_state1.var_12));
                    rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_13);
                    rb_state1.var_1 = rb_state1.var_2;
                    pthread_mutex_unlock(&(rb_state1.lock_32));
                }
                if ((rb_state1.var_0) == (0x359e5413)) {
                    if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                        racebench_trigger(1);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_0) == (0x1136a708)) {
                    pthread_mutex_lock(&(rb_state2.lock_15));
                    rb_state2.var_3 = 0x88c30809;
                    rb_state2.var_4 = 0xa31b4f89;
                    rb_state2.var_5 = (rb_state2.var_5) - (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_2) : (0x2e0eb1fe));
                    rb_state2.var_6 = (rb_state2.var_6) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_4) : (0xdc4ee0b7));
                    rb_state2.var_7 = (rb_state2.var_7) + (rb_state2.var_1);
                    rb_state2.var_8 = (rb_state2.var_8) ^ (((((0x950b8958) - (rb_state2.var_7)) ^ (rb_state2.var_4)) + (rb_state2.var_6)) ^ (rb_state2.var_3));
                    rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_5) : (0xcbabb18));
                    rb_state2.var_10 = (rb_state2.var_10) + (((((0xa125c88f) - (rb_state2.var_7)) ^ (rb_state2.var_6)) + (0x3af69b7f)) + (0xbfddc111));
                    rb_state2.var_11 = (rb_state2.var_11) - (((0x4cfeb944) ^ (rb_state2.var_8)) + (rb_state2.var_9));
                    rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0x8ab88fa));
                    rb_state2.var_13 = (rb_state2.var_13) + (((0x9c7a6dc0) ^ (rb_state2.var_11)) - (0x43bd287e));
                    rb_state2.var_14 = (rb_state2.var_14) - (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_13) : (0x392648ba));
                    rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_14);
                    rb_state2.var_2 = rb_state2.var_1;
                    pthread_mutex_unlock(&(rb_state2.lock_15));
                }
                #endif
                #ifdef RACEBENCH_BUG_4
                rb_state4.var_0 = (rb_state4.var_0) - (0xea6ad1c8);
                if ((rb_state4.var_0) == (0x55f5519c)) {
                    rb_state4.var_2 = rb_state4.var_1;
                }
                if ((rb_state4.var_0) == (0x3a47eb12)) {
                    if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                        racebench_trigger(4);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) + (14325 < rb_input_size ? (uint32_t)rb_input[14325] : 0x575524be);
                #endif
                #ifdef RACEBENCH_BUG_8
                rb_state8.var_0 = (rb_state8.var_0) + (133991 < rb_input_size ? (uint32_t)rb_input[133991] : 0x181ce2e2);
                #endif
                #ifdef RACEBENCH_BUG_10
                rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
                if ((rb_state10.var_0) == (0x16b5bb50)) {
                    pthread_mutex_lock(&(rb_state10.lock_41));
                    rb_state10.var_10 = 0xb3612cbe;
                    rb_state10.var_11 = (rb_state10.var_11) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0xb5d0005d));
                    rb_state10.var_12 = (rb_state10.var_12) ^ (83653 < rb_input_size ? (uint32_t)rb_input[83653] : 0xdafb6e4b);
                    rb_state10.var_13 = (rb_state10.var_13) + (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0xc1851cf4));
                    rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_12);
                    rb_state10.var_15 = (rb_state10.var_15) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_13) : (0x3ef477c));
                    rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_14) : (0x2c3ad392));
                    rb_state10.var_17 = (rb_state10.var_17) ^ (rb_state10.var_15);
                    rb_state10.var_18 = (rb_state10.var_18) + (((0xa5d9faf3) + (rb_state10.var_7)) ^ (rb_state10.var_16));
                    rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_17) : (0x4c995930));
                    rb_state10.var_20 = (rb_state10.var_20) - (((((0x6cf2e8ea) ^ (rb_state10.var_18)) - (rb_state10.var_19)) ^ (0xeb7bff22)) - (0x5ad45c87));
                    rb_state10.var_9 = (rb_state10.var_9) - (rb_state10.var_20);
                    rb_state10.var_1 = rb_state10.var_9;
                    pthread_mutex_unlock(&(rb_state10.lock_41));
                }
                if ((rb_state10.var_0) == (0xa5de5e2a)) {
                    pthread_mutex_lock(&(rb_state10.lock_41));
                    rb_state10.var_22 = (rb_state10.var_22) - (rb_state10.var_9);
                    rb_state10.var_23 = (rb_state10.var_23) ^ (((0x7a4cadd2) - (rb_state10.var_10)) ^ (rb_state10.var_12));
                    rb_state10.var_24 = (rb_state10.var_24) ^ (((((0xa5b3dfc8) - (rb_state10.var_22)) - (rb_state10.var_13)) - (0x5a2a3f6a)) - (rb_state10.var_11));
                    rb_state10.var_25 = (rb_state10.var_25) + ((0x3a7b33e2) ^ (0x69f46139));
                    rb_state10.var_26 = (rb_state10.var_26) + (((0xd10d830) ^ (rb_state10.var_14)) + (rb_state10.var_23));
                    rb_state10.var_27 = (rb_state10.var_27) + (((0x2afe965b) + (0xee28dddb)) ^ (rb_state10.var_24));
                    rb_state10.var_28 = (rb_state10.var_28) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_25) : (0xbeb510b3));
                    rb_state10.var_29 = (rb_state10.var_29) - (((((0x2cf65cc6) - (rb_state10.var_26)) ^ (0x485843d0)) ^ (rb_state10.var_16)) ^ (rb_state10.var_27));
                    rb_state10.var_30 = (rb_state10.var_30) - (rb_state10.var_28);
                    rb_state10.var_31 = (rb_state10.var_31) ^ (((0xf0032950) - (rb_state10.var_17)) - (rb_state10.var_29));
                    rb_state10.var_32 = (rb_state10.var_32) + (((rb_state10.var_18) == (0x0)) ? (rb_state10.var_30) : (0x5dd289e5));
                    rb_state10.var_33 = (rb_state10.var_33) - (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_31) : (0xa39d8cd4));
                    rb_state10.var_34 = (rb_state10.var_34) + (((0xfe8c131) ^ (0x49d38f97)) + (rb_state10.var_32));
                    rb_state10.var_35 = (rb_state10.var_35) + (rb_state10.var_33);
                    rb_state10.var_36 = (rb_state10.var_36) - (rb_state10.var_34);
                    rb_state10.var_37 = (rb_state10.var_37) - (((0xe93979e7) ^ (rb_state10.var_20)) - (rb_state10.var_35));
                    rb_state10.var_38 = (rb_state10.var_38) + (rb_state10.var_36);
                    rb_state10.var_39 = (rb_state10.var_39) ^ (rb_state10.var_37);
                    rb_state10.var_40 = (rb_state10.var_40) + (((((0x2a61c259) ^ (rb_state10.var_39)) - (0x86c6c114)) - (rb_state10.var_21)) ^ (rb_state10.var_38));
                    rb_state10.var_21 = (rb_state10.var_21) + (rb_state10.var_40);
                    rb_state10.var_1 = rb_state10.var_21;
                    pthread_mutex_unlock(&(rb_state10.lock_41));
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_0) == (0x6abd8737)) {
                    rb_state11.var_11 = 0xef10011e;
                    rb_state11.var_12 = 0x3330fa93;
                    rb_state11.var_13 = (rb_state11.var_13) - (((((0xbcc3f525) + (0x6bb17ae0)) - (rb_state11.var_3)) - (rb_state11.var_12)) + (rb_state11.var_2));
                    rb_state11.var_14 = (rb_state11.var_14) + ((0x59a7b6f6) - (0x8d208a0c));
                    rb_state11.var_15 = (rb_state11.var_15) + (((((0x373c7b2b) + (rb_state11.var_11)) ^ (0x2595cedc)) - (0x30869caf)) ^ (rb_state11.var_13));
                    rb_state11.var_16 = (rb_state11.var_16) - (((0x66e78fc4) - (rb_state11.var_14)) ^ (rb_state11.var_4));
                    rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_15) : (0x8840ea1e));
                    rb_state11.var_18 = (rb_state11.var_18) + (((((0xa5af6f81) - (rb_state11.var_16)) - (rb_state11.var_17)) + (rb_state11.var_6)) - (rb_state11.var_7));
                    rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_18);
                }
                #endif
                #ifdef RACEBENCH_BUG_12
                if ((rb_state12.var_0) == (0xafef7562)) {
                    if ((rb_state12.var_1) != (0x0)) {
                        if (!((rb_state12.var_1) == (rb_state12.var_24))) {
                            racebench_trigger(12);
                        }
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_13
                if ((rb_state13.var_0) == (0x80e0578a)) {
                    rb_state13.var_2 = rb_state13.var_1;
                }
                if ((rb_state13.var_0) == (0x80e0578a)) {
                    rb_state13.var_3 = 0xf939c19e;
                    rb_state13.var_4 = (rb_state13.var_4) - (rb_state13.var_2);
                    rb_state13.var_5 = (rb_state13.var_5) + (45088 < rb_input_size ? (uint32_t)rb_input[45088] : 0x56dc5195);
                    rb_state13.var_6 = (rb_state13.var_6) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_3) : (0xea37b46e));
                    rb_state13.var_7 = (rb_state13.var_7) + (rb_state13.var_4);
                    rb_state13.var_8 = (rb_state13.var_8) ^ (((0x300f4226) + (rb_state13.var_5)) - (0xd680740a));
                    rb_state13.var_9 = (rb_state13.var_9) ^ (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_6) : (0xa2959a98));
                    rb_state13.var_10 = (rb_state13.var_10) ^ (((0x71b89662) - (rb_state13.var_7)) ^ (0xedffab34));
                    rb_state13.var_11 = (rb_state13.var_11) ^ (((((0xe63c47eb) ^ (0x8d4693f6)) + (0x1ed332d0)) ^ (rb_state13.var_9)) ^ (rb_state13.var_8));
                    rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_10);
                    rb_state13.var_13 = (rb_state13.var_13) - (((((0xe57bba5c) - (rb_state13.var_12)) - (0x52d70e5c)) + (0xa9840ace)) ^ (rb_state13.var_11));
                    rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_13);
                }
                if ((rb_state13.var_0) == (0x80e0578a)) {
                    if (!((rb_state13.var_1) == (rb_state13.var_2))) {
                        racebench_trigger(13);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_14
                rb_state14.var_0 = (rb_state14.var_0) ^ (131672 < rb_input_size ? (uint32_t)rb_input[131672] : 0xb252612e);
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_0 = (rb_state16.var_0) ^ (0x66fa6a4);
                #endif
                #ifdef RACEBENCH_BUG_17
                if ((rb_state17.var_0) == (0x1df5a55a)) {
                    pthread_mutex_lock(&(rb_state17.lock_23));
                    rb_state17.var_3 = 0xe6b8079f;
                    rb_state17.var_4 = (rb_state17.var_4) + ((0xe9435d2f) - (0xc4832c70));
                    rb_state17.var_5 = (rb_state17.var_5) - (((((0x2ed83eaa) ^ (rb_state17.var_2)) - (0x42e452d3)) - (rb_state17.var_3)) + (0xa6ab26b8));
                    rb_state17.var_6 = (rb_state17.var_6) ^ (((((0xcdf18330) - (rb_state17.var_4)) ^ (rb_state17.var_3)) - (rb_state17.var_1)) - (rb_state17.var_4));
                    rb_state17.var_7 = (rb_state17.var_7) + (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_5) : (0x257e185f));
                    rb_state17.var_8 = (rb_state17.var_8) - (((0x1e8956fc) - (rb_state17.var_6)) ^ (0xaa5b4f76));
                    rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_7);
                    rb_state17.var_10 = (rb_state17.var_10) ^ (((0xad5ce934) + (0xd4c3f39a)) + (rb_state17.var_8));
                    rb_state17.var_11 = (rb_state17.var_11) ^ (rb_state17.var_9);
                    rb_state17.var_12 = (rb_state17.var_12) ^ (((0xafd49a3e) + (rb_state17.var_6)) - (rb_state17.var_10));
                    rb_state17.var_13 = (rb_state17.var_13) - (((0x956446c) + (rb_state17.var_11)) - (rb_state17.var_7));
                    rb_state17.var_14 = (rb_state17.var_14) - (((0x35b3ac0) - (rb_state17.var_8)) + (rb_state17.var_12));
                    rb_state17.var_15 = (rb_state17.var_15) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_13) : (0xef453fe7));
                    rb_state17.var_16 = (rb_state17.var_16) - (((((0x4247f3e2) - (0x120e6bca)) ^ (rb_state17.var_14)) + (rb_state17.var_10)) - (rb_state17.var_15));
                    rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_16);
                    rb_state17.var_1 = rb_state17.var_2;
                    pthread_mutex_unlock(&(rb_state17.lock_23));
                }
                if ((rb_state17.var_0) == (0x4fb84bca)) {
                    if (!((rb_state17.var_1) == (rb_state17.var_2))) {
                        racebench_trigger(17);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0xf049fc82)) {
                    rb_state18.var_3 = 0xc020513a;
                    rb_state18.var_4 = (rb_state18.var_4) ^ (((((0x21cd4717) + (0xa8bc2888)) - (rb_state18.var_3)) ^ (rb_state18.var_2)) - (rb_state18.var_1));
                    rb_state18.var_5 = (rb_state18.var_5) + (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_3) : (0x5c4ecc74));
                    rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_5);
                    rb_state18.var_7 = (rb_state18.var_7) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0x5d08dfe3));
                    rb_state18.var_8 = (rb_state18.var_8) ^ (((0xbab9be56) + (rb_state18.var_5)) - (rb_state18.var_7));
                    rb_state18.var_9 = (rb_state18.var_9) + (rb_state18.var_6);
                    rb_state18.var_10 = (rb_state18.var_10) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_7) : (0x638045c9));
                    rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_8) : (0x5684c03b));
                    rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_9);
                    rb_state18.var_13 = (rb_state18.var_13) ^ (((0xab4bdae3) + (rb_state18.var_10)) + (rb_state18.var_10));
                    rb_state18.var_14 = (rb_state18.var_14) - (((0xe652ac95) ^ (rb_state18.var_11)) - (rb_state18.var_11));
                    rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_12) : (0x7cda4333));
                    rb_state18.var_16 = (rb_state18.var_16) ^ (rb_state18.var_13);
                    rb_state18.var_17 = (rb_state18.var_17) ^ (rb_state18.var_14);
                    rb_state18.var_18 = (rb_state18.var_18) - (rb_state18.var_15);
                    rb_state18.var_19 = (rb_state18.var_19) - (((0x9bb970a8) - (rb_state18.var_13)) + (rb_state18.var_16));
                    rb_state18.var_20 = (rb_state18.var_20) - (((0xdf7034d) - (rb_state18.var_17)) + (rb_state18.var_14));
                    rb_state18.var_21 = (rb_state18.var_21) + (rb_state18.var_18);
                    rb_state18.var_22 = (rb_state18.var_22) - (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_20) : (0xb40c21bb));
                    rb_state18.var_23 = (rb_state18.var_23) ^ (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_21) : (0x80566c));
                    rb_state18.var_24 = (rb_state18.var_24) - (rb_state18.var_22);
                    rb_state18.var_25 = (rb_state18.var_25) ^ (((0x8b933ed0) + (rb_state18.var_23)) ^ (0xbccef608));
                    rb_state18.var_26 = (rb_state18.var_26) + (((0xa90503ca) ^ (rb_state18.var_24)) ^ (rb_state18.var_16));
                    rb_state18.var_27 = (rb_state18.var_27) - (((0xc41ea12e) - (rb_state18.var_17)) - (rb_state18.var_25));
                    rb_state18.var_28 = (rb_state18.var_28) ^ (rb_state18.var_26);
                    rb_state18.var_29 = (rb_state18.var_29) + (rb_state18.var_27);
                    rb_state18.var_30 = (rb_state18.var_30) + (((rb_state18.var_28) == (0x0)) ? (rb_state18.var_29) : (0xdf1725d4));
                    rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_30);
                }
                rb_state18.var_18 = (rb_state18.var_18) - ((0xc96515fb) ^ (0x30205507));
                if ((rb_state18.var_19) == (0x64468f58)) {
                    pthread_mutex_lock(&(rb_state18.lock_57));
                    rb_state18.var_35 = 0x75e17c54;
                    rb_state18.var_36 = (rb_state18.var_36) - (((0x6bfc7b02) ^ (0x74896fa8)) - (rb_state18.var_35));
                    rb_state18.var_37 = (rb_state18.var_37) - ((0x692a201c) + (0xbcd3f5a6));
                    rb_state18.var_38 = (rb_state18.var_38) ^ (((rb_state18.var_23) == (0x0)) ? (rb_state18.var_22) : (0x90566c61));
                    rb_state18.var_39 = (rb_state18.var_39) + (((0x72274981) + (0xd7d1bfcb)) + (rb_state18.var_20));
                    rb_state18.var_40 = (rb_state18.var_40) + (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_36) : (0xe0e26733));
                    rb_state18.var_41 = (rb_state18.var_41) - (((0x478d8a7d) - (rb_state18.var_37)) - (rb_state18.var_24));
                    rb_state18.var_42 = (rb_state18.var_42) - (((rb_state18.var_25) == (0x0)) ? (rb_state18.var_38) : (0x4bd0d126));
                    rb_state18.var_43 = (rb_state18.var_43) + (((rb_state18.var_26) == (0x0)) ? (rb_state18.var_39) : (0x2c8a3115));
                    rb_state18.var_44 = (rb_state18.var_44) + (((0x1a25dd08) - (0x5021ca1b)) ^ (rb_state18.var_40));
                    rb_state18.var_45 = (rb_state18.var_45) ^ (((rb_state18.var_41) == (0x0)) ? (rb_state18.var_42) : (0x205fd989));
                    rb_state18.var_46 = (rb_state18.var_46) - (rb_state18.var_43);
                    rb_state18.var_47 = (rb_state18.var_47) - (rb_state18.var_44);
                    rb_state18.var_48 = (rb_state18.var_48) - (rb_state18.var_45);
                    rb_state18.var_49 = (rb_state18.var_49) - (((rb_state18.var_27) == (0x0)) ? (rb_state18.var_46) : (0xc4365abc));
                    rb_state18.var_50 = (rb_state18.var_50) + (((0x55fe7b8c) + (rb_state18.var_47)) + (rb_state18.var_28));
                    rb_state18.var_51 = (rb_state18.var_51) ^ (((rb_state18.var_48) == (0x0)) ? (rb_state18.var_49) : (0x78b987e0));
                    rb_state18.var_52 = (rb_state18.var_52) - (((((0x79a8f6f9) - (rb_state18.var_29)) + (rb_state18.var_50)) - (0x82fd212a)) + (rb_state18.var_51));
                    rb_state18.var_34 = (rb_state18.var_34) + (rb_state18.var_52);
                    rb_state18.var_33 = rb_state18.var_34;
                    pthread_mutex_unlock(&(rb_state18.lock_57));
                }
                if ((rb_state18.var_19) == (0x64468f58)) {
                    rb_state18.var_53 = 0x4a025310;
                    rb_state18.var_54 = 0xb4da64d1;
                    rb_state18.var_55 = (rb_state18.var_55) - (((rb_state18.var_30) == (0x0)) ? (rb_state18.var_53) : (0xbd2cb552));
                    rb_state18.var_56 = (rb_state18.var_56) ^ (((rb_state18.var_54) == (0x0)) ? (rb_state18.var_55) : (0x96468fea));
                    rb_state18.var_33 = (rb_state18.var_33) ^ (rb_state18.var_56);
                }
                if ((rb_state18.var_19) == (0x64468f58)) {
                    if (!((rb_state18.var_33) == (rb_state18.var_34))) {
                        racebench_trigger(18);
                    }
                }
                #endif
                cells[index].next = NULL;
                #ifdef RACEBENCH_BUG_0
                if ((rb_state0.var_0) == (0x1c3f91e0)) {
                    pthread_mutex_lock(&(rb_state0.lock_26));
                    rb_state0.var_7 = 0x7404e7ce;
                    rb_state0.var_8 = (rb_state0.var_8) + (((((0x9312cdba) + (0x1356fb17)) ^ (rb_state0.var_7)) + (rb_state0.var_7)) ^ (rb_state0.var_5));
                    rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_6) : (0x7e090d7));
                    rb_state0.var_10 = (rb_state0.var_10) - (((((0x4bfe829e) - (rb_state0.var_9)) ^ (0x80c28cf7)) - (rb_state0.var_8)) + (0xc10cc656));
                    rb_state0.var_6 = (rb_state0.var_6) ^ (rb_state0.var_10);
                    rb_state0.var_1 = rb_state0.var_6;
                    pthread_mutex_unlock(&(rb_state0.lock_26));
                }
                if ((rb_state0.var_0) == (0x1c3f91e0)) {
                    pthread_mutex_lock(&(rb_state0.lock_26));
                    rb_state0.var_12 = 0xd51890c5;
                    rb_state0.var_13 = 0xa4cc75ab;
                    rb_state0.var_14 = 0x3faa5bdf;
                    rb_state0.var_15 = (rb_state0.var_15) + (rb_state0.var_9);
                    rb_state0.var_16 = (rb_state0.var_16) + (rb_state0.var_12);
                    rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_14);
                    rb_state0.var_18 = (rb_state0.var_18) + (((0x886d6e48) ^ (0xaae0e0bf)) + (rb_state0.var_13));
                    rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_16) : (0x92f447fb));
                    rb_state0.var_20 = (rb_state0.var_20) ^ (((0xcffa31e3) - (0xeaeab92a)) + (rb_state0.var_17));
                    rb_state0.var_21 = (rb_state0.var_21) - (rb_state0.var_18);
                    rb_state0.var_22 = (rb_state0.var_22) + (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_19) : (0x4b1ae6e3));
                    rb_state0.var_23 = (rb_state0.var_23) + (((((0x5f22a2ca) + (0xb014f69f)) ^ (rb_state0.var_21)) + (rb_state0.var_20)) - (0x424d8ba6));
                    rb_state0.var_24 = (rb_state0.var_24) ^ (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_22) : (0x9624df4e));
                    rb_state0.var_25 = (rb_state0.var_25) + (((((0xe1ebddb3) - (0x5e633535)) + (0x1671ccd)) + (rb_state0.var_23)) - (rb_state0.var_24));
                    rb_state0.var_11 = (rb_state0.var_11) + (rb_state0.var_25);
                    rb_state0.var_1 = rb_state0.var_11;
                    pthread_mutex_unlock(&(rb_state0.lock_26));
                }
                #endif
                #ifdef RACEBENCH_BUG_2
                if ((rb_state2.var_13) == (0xe643f8c3)) {
                    rb_state2.var_17 = rb_state2.var_14;
                }
                if ((rb_state2.var_13) == (0xe643f8c3)) {
                    rb_state2.var_18 = 0x47806d45;
                    rb_state2.var_19 = 0x7ddc32c4;
                    rb_state2.var_20 = 0x69753669;
                    rb_state2.var_21 = (rb_state2.var_21) - (rb_state2.var_19);
                    rb_state2.var_22 = (rb_state2.var_22) + (((((0xf643517b) - (rb_state2.var_18)) + (rb_state2.var_20)) + (0x45eeee73)) - (0x917b4ac9));
                    rb_state2.var_23 = (rb_state2.var_23) + (((0xd9a5dd30) + (0x72c9027e)) - (rb_state2.var_21));
                    rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_23) : (0x534133be));
                    rb_state2.var_14 = (rb_state2.var_14) - (rb_state2.var_24);
                }
                if ((rb_state2.var_13) == (0xe643f8c3)) {
                    if (!((rb_state2.var_14) == (rb_state2.var_17))) {
                        racebench_trigger(2);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_5
                rb_state5.var_0 = (rb_state5.var_0) + (56370 < rb_input_size ? (uint32_t)rb_input[56370] : 0x5e4f18ef);
                #endif
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0xc3)) ? (rb_state6.var_0) : (0x22b9ab25));
                if ((rb_state6.var_0) == (0x83f6d221)) {
                    pthread_mutex_lock(&(rb_state6.lock_30));
                    rb_state6.var_3 = 0x17e25f5a;
                    rb_state6.var_4 = (rb_state6.var_4) ^ (((0x1c9af7fd) ^ (rb_state6.var_3)) - (rb_state6.var_1));
                    rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_2);
                    rb_state6.var_6 = (rb_state6.var_6) - (rb_state6.var_3);
                    rb_state6.var_7 = (rb_state6.var_7) - (rb_state6.var_4);
                    rb_state6.var_8 = (rb_state6.var_8) - (((0x87a235a9) - (rb_state6.var_5)) - (rb_state6.var_4));
                    rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0x8bede0c3));
                    rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_7) : (0x4737d856));
                    rb_state6.var_11 = (rb_state6.var_11) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_8) : (0x521fa1d1));
                    rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_9);
                    rb_state6.var_13 = (rb_state6.var_13) + (((((0x7003c88d) - (rb_state6.var_11)) ^ (rb_state6.var_10)) ^ (0x44b4d6e)) + (0x7b5f285b));
                    rb_state6.var_14 = (rb_state6.var_14) ^ (rb_state6.var_12);
                    rb_state6.var_15 = (rb_state6.var_15) + (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_14) : (0x442f9d5c));
                    rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_15);
                    rb_state6.var_1 = rb_state6.var_2;
                    pthread_mutex_unlock(&(rb_state6.lock_30));
                }
                if ((rb_state6.var_0) == (0x83f6d221)) {
                    if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                        racebench_trigger(6);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) - (0x2e9fdfb9);
                if ((rb_state9.var_0) == (0xef7493d7)) {
                    rb_state9.var_9 = 0x81cafdb8;
                    rb_state9.var_10 = 0xbf2e986b;
                    rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_4) : (0xb37dd9c7));
                    rb_state9.var_12 = (rb_state9.var_12) - ((0x6bfeb674) ^ (rb_state9.var_5));
                    rb_state9.var_13 = (rb_state9.var_13) ^ (((0xad7bf44a) - (rb_state9.var_9)) - (0x92aec12b));
                    rb_state9.var_14 = (rb_state9.var_14) + (((0xdf8a2e15) ^ (0xf3cb0c4c)) + (rb_state9.var_11));
                    rb_state9.var_15 = (rb_state9.var_15) + (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_12) : (0xd213cc31));
                    rb_state9.var_16 = (rb_state9.var_16) - (((((0xa4c20c5e) ^ (rb_state9.var_8)) - (rb_state9.var_14)) ^ (rb_state9.var_7)) + (rb_state9.var_13));
                    rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_15) : (0x341ca6f4));
                    rb_state9.var_18 = (rb_state9.var_18) - (rb_state9.var_16);
                    rb_state9.var_19 = (rb_state9.var_19) - (rb_state9.var_17);
                    rb_state9.var_20 = (rb_state9.var_20) ^ (rb_state9.var_18);
                    rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_20) : (0x9efde0ca));
                    rb_state9.var_8 = (rb_state9.var_8) + (rb_state9.var_21);
                    rb_state9.var_1 = rb_state9.var_8;
                }
                if ((rb_state9.var_0) == (0xc0d4b41e)) {
                    if ((rb_state9.var_1) != (0x0)) {
                        if (!((rb_state9.var_1) == (rb_state9.var_22))) {
                            racebench_trigger(9);
                        }
                    }
                }
                if ((rb_state9.var_0) == (0xc0d4b41e)) {
                    pthread_mutex_lock(&(rb_state9.lock_42));
                    rb_state9.var_23 = 0xa4bc70e4;
                    rb_state9.var_24 = (rb_state9.var_24) ^ (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_12) : (0xcbb64c07));
                    rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_14) : (0x729b8545));
                    rb_state9.var_26 = (rb_state9.var_26) + (rb_state9.var_10);
                    rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_23) : (0x94513118));
                    rb_state9.var_28 = (rb_state9.var_28) + (rb_state9.var_11);
                    rb_state9.var_29 = (rb_state9.var_29) - (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0x5d10c522));
                    rb_state9.var_30 = (rb_state9.var_30) - (((((0x50fd1348) - (rb_state9.var_27)) ^ (rb_state9.var_17)) - (rb_state9.var_26)) - (rb_state9.var_18));
                    rb_state9.var_31 = (rb_state9.var_31) ^ (((0xe59b46ea) - (rb_state9.var_19)) - (rb_state9.var_28));
                    rb_state9.var_32 = (rb_state9.var_32) ^ (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_29) : (0x9655eb3e));
                    rb_state9.var_33 = (rb_state9.var_33) - (((rb_state9.var_30) == (0x0)) ? (rb_state9.var_31) : (0xb471b26e));
                    rb_state9.var_34 = (rb_state9.var_34) ^ (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_32) : (0x9e040065));
                    rb_state9.var_35 = (rb_state9.var_35) - (rb_state9.var_33);
                    rb_state9.var_36 = (rb_state9.var_36) - (((rb_state9.var_22) == (0x0)) ? (rb_state9.var_34) : (0xfb744f52));
                    rb_state9.var_37 = (rb_state9.var_37) + (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_35) : (0xac6e92a8));
                    rb_state9.var_38 = (rb_state9.var_38) - (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_36) : (0x16e80336));
                    rb_state9.var_39 = (rb_state9.var_39) - (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_37) : (0x5f53dc9b));
                    rb_state9.var_40 = (rb_state9.var_40) - (rb_state9.var_38);
                    rb_state9.var_41 = (rb_state9.var_41) - (((((0x7b109214) - (rb_state9.var_40)) - (rb_state9.var_26)) - (rb_state9.var_39)) + (0xd93def62));
                    rb_state9.var_22 = (rb_state9.var_22) - (rb_state9.var_41);
                    rb_state9.var_1 = rb_state9.var_22;
                    pthread_mutex_unlock(&(rb_state9.lock_42));
                }
                #endif
                #ifdef RACEBENCH_BUG_11
                if ((rb_state11.var_0) == (0x6abd8737)) {
                    pthread_mutex_lock(&(rb_state11.lock_19));
                    rb_state11.var_3 = 0xaa7da590;
                    rb_state11.var_4 = 0x71bd0e5c;
                    rb_state11.var_5 = 0x4d22dc3a;
                    rb_state11.var_6 = (rb_state11.var_6) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_3) : (0x3b716925));
                    rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_4) : (0xd767138f));
                    rb_state11.var_8 = (rb_state11.var_8) - (((0xc1f888a8) - (0x20b3a57f)) - (rb_state11.var_6));
                    rb_state11.var_9 = (rb_state11.var_9) - (rb_state11.var_7);
                    rb_state11.var_10 = (rb_state11.var_10) - (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_9) : (0xe1d0c6e6));
                    rb_state11.var_2 = (rb_state11.var_2) ^ (rb_state11.var_10);
                    rb_state11.var_1 = rb_state11.var_2;
                    pthread_mutex_unlock(&(rb_state11.lock_19));
                }
                if ((rb_state11.var_0) == (0x6abd8737)) {
                    if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                        racebench_trigger(11);
                    }
                }
                #endif
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
                #endif
                #ifdef RACEBENCH_BUG_17
                rb_state17.var_0 = (rb_state17.var_0) + (0x31c2a670);
                if ((rb_state17.var_0) == (0x4fb84bca)) {
                    pthread_mutex_lock(&(rb_state17.lock_23));
                    rb_state17.var_17 = 0x15265c82;
                    rb_state17.var_18 = 0xedd8febe;
                    rb_state17.var_19 = (rb_state17.var_19) ^ (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_11) : (0xe2a2711d));
                    rb_state17.var_20 = (rb_state17.var_20) - (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_18) : (0x308aa1d5));
                    rb_state17.var_21 = (rb_state17.var_21) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_19) : (0x2a923e3a));
                    rb_state17.var_22 = (rb_state17.var_22) - (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0x14998262));
                    rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_22);
                    pthread_mutex_unlock(&(rb_state17.lock_23));
                }
                #endif
                #ifdef RACEBENCH_BUG_18
                if ((rb_state18.var_0) == (0xf049fc82)) {
                    pthread_mutex_lock(&(rb_state18.lock_31));
                    rb_state18.var_2 = rb_state18.var_1;
                    pthread_mutex_unlock(&(rb_state18.lock_31));
                }
                if ((rb_state18.var_0) == (0xf049fc82)) {
                    rb_state18.var_32 = !((rb_state18.var_1) == (rb_state18.var_2));
                }
                #endif
                #ifdef RACEBENCH_BUG_19
                rb_state19.var_0 = (rb_state19.var_0) - ((0xe5a4c801) ^ (0xf4b2967b));
                #endif
                last_cells[index] = &cells[index];
            }
        }
    }
}

void RebuildGridMT(int tid) {

    for (int iz = grids[tid].sz; iz < grids[tid].ez; ++iz) {
        for (int iy = grids[tid].sy; iy < grids[tid].ey; ++iy) {
            for (int ix = grids[tid].sx; ix < grids[tid].ex; ++ix) {
                int index2 = (iz * ny + iy) * nx + ix;
                Cell *cell2 = &cells2[index2];
                int np2 = cnumPars2[index2];

                for (int j = 0; j < np2; ++j) {

                    int ci = (int)((cell2->p[j % PARTICLES_PER_CELL].x - domainMin.x) / delta.x);
                    int cj = (int)((cell2->p[j % PARTICLES_PER_CELL].y - domainMin.y) / delta.y);
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

                    int index = (ck * ny + cj) * nx + ci;

                    if (border[index]) {
                        pthread_mutex_lock(&mutex[index][CELL_MUTEX_ID]);
                    }
                    Cell *cell = last_cells[index];
                    int np = cnumPars[index];

                    if ((np % PARTICLES_PER_CELL == 0) && (cnumPars[index] != 0)) {
                        cell->next = cellpool_getcell(&pools[tid]);
                        cell = cell->next;
                        last_cells[index] = cell;
                    }
                    ++cnumPars[index];
                    if (border[index]) {
                        pthread_mutex_unlock(&mutex[index][CELL_MUTEX_ID]);
                    }

                    cell->p[np % PARTICLES_PER_CELL] = cell2->p[j % PARTICLES_PER_CELL];
                    cell->hv[np % PARTICLES_PER_CELL] = cell2->hv[j % PARTICLES_PER_CELL];
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
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_0);
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
        targs[i].tid = i;
        targs[i].frames = framenum;
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
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