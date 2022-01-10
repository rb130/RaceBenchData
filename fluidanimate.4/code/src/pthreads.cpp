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
                int index = (iz * ny + iy) * nx + ix;
                cnumPars[index] = 0;
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
                int index2 = (iz * ny + iy) * nx + ix;
                Cell *cell2 = &cells2[index2];
                int np2 = cnumPars2[index2];

                for (int j = 0; j < np2; ++j) {

                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_2 = (rb_state6.var_2) - (64501 < rb_input_size ? (uint32_t)rb_input[64501] : 0xfa8db9d8);
                    #endif
                    int ci = (int)((cell2->p[j % PARTICLES_PER_CELL].x - domainMin.x) / delta.x);
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_3 = (rb_state6.var_3) ^ ((0x2f301a01) ^ (0x76967b14));
                    rb_state6.var_2 = (rb_state6.var_2) - (103751 < rb_input_size ? (uint32_t)rb_input[103751] : 0xab94fcfb);
                    #endif
                    int cj = (int)((cell2->p[j % PARTICLES_PER_CELL].y - domainMin.y) / delta.y);
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_2);
                    rb_state6.var_3 = (rb_state6.var_3) - (134858 < rb_input_size ? (uint32_t)rb_input[134858] : 0x530bc715);
                    if ((rb_state6.var_3) == (0x59a63acf)) {
                        rb_state6.var_27 = 0x73a6e838;
                        rb_state6.var_28 = 0x1a054c8a;
                        rb_state6.var_29 = 0x125f7ec8;
                        rb_state6.var_30 = (rb_state6.var_30) + (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_12) : (0xee825a8b));
                        rb_state6.var_31 = (rb_state6.var_31) ^ (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_27) : (0xd9f9ed0f));
                        rb_state6.var_32 = (rb_state6.var_32) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_29) : (0xb818bc74));
                        rb_state6.var_33 = (rb_state6.var_33) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_28) : (0x8946159a));
                        rb_state6.var_34 = (rb_state6.var_34) ^ (((0x95fa776c) - (rb_state6.var_17)) - (rb_state6.var_30));
                        rb_state6.var_35 = (rb_state6.var_35) + (((((0x7cc54fc) ^ (rb_state6.var_32)) + (rb_state6.var_18)) ^ (rb_state6.var_19)) + (rb_state6.var_31));
                        rb_state6.var_36 = (rb_state6.var_36) ^ (((rb_state6.var_20) == (0x0)) ? (rb_state6.var_33) : (0x71445364));
                        rb_state6.var_37 = (rb_state6.var_37) - (((rb_state6.var_34) == (0x0)) ? (rb_state6.var_35) : (0x104f0d5a));
                        rb_state6.var_38 = (rb_state6.var_38) ^ (((0xe008dfd4) + (rb_state6.var_36)) ^ (rb_state6.var_21));
                        rb_state6.var_39 = (rb_state6.var_39) ^ (((0xf99283ed) ^ (rb_state6.var_22)) + (rb_state6.var_37));
                        rb_state6.var_40 = (rb_state6.var_40) ^ (((rb_state6.var_23) == (0x0)) ? (rb_state6.var_38) : (0xcb0a55dc));
                        rb_state6.var_41 = (rb_state6.var_41) + (((0x1b87a762) + (rb_state6.var_24)) + (rb_state6.var_39));
                        rb_state6.var_42 = (rb_state6.var_42) - (((((0x92d0d35d) ^ (0xe22decd0)) - (rb_state6.var_40)) + (0x64ccd7ef)) ^ (rb_state6.var_41));
                        rb_state6.var_4 = (rb_state6.var_4) ^ (rb_state6.var_42);
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
                    rb_state6.var_1 = (rb_state6.var_1) - (145693 < rb_input_size ? (uint32_t)rb_input[145693] : 0x93447d05);
                    #endif
                    int index = (ck * ny + cj) * nx + ci;

                    if (border[index]) {
                        #ifdef RACEBENCH_BUG_6
                        rb_state6.var_1 = (rb_state6.var_1) + ((0xf9d58ae4) + (0x8504f3a8));
                        #endif
                        pthread_mutex_lock(&mutex[index][CELL_MUTEX_ID]);
                    }
                    Cell *cell = last_cells[index];
                    #ifdef RACEBENCH_BUG_6
                    if ((rb_state6.var_3) == (0x59a63b65)) {
                        pthread_mutex_lock(&(rb_state6.lock_43));
                        rb_state6.var_6 = 0x68a26190;
                        rb_state6.var_7 = (rb_state6.var_7) ^ (120207 < rb_input_size ? (uint32_t)rb_input[120207] : 0x20283277);
                        rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_6) : (0xaa7fe2ec));
                        rb_state6.var_9 = (rb_state6.var_9) + (0x1589a55f);
                        rb_state6.var_10 = (rb_state6.var_10) ^ (((0x850030fa) ^ (rb_state6.var_5)) ^ (0xda9874f3));
                        rb_state6.var_11 = (rb_state6.var_11) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_8) : (0xcc94c88e));
                        rb_state6.var_12 = (rb_state6.var_12) ^ (rb_state6.var_9);
                        rb_state6.var_13 = (rb_state6.var_13) + (((((0x611d59c2) ^ (rb_state6.var_11)) - (rb_state6.var_6)) + (0x4d26359d)) + (rb_state6.var_10));
                        rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_12);
                        rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_13);
                        rb_state6.var_16 = (rb_state6.var_16) ^ (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_14) : (0x1c815c70));
                        rb_state6.var_17 = (rb_state6.var_17) + (rb_state6.var_15);
                        rb_state6.var_18 = (rb_state6.var_18) - (rb_state6.var_16);
                        rb_state6.var_19 = (rb_state6.var_19) - (rb_state6.var_17);
                        rb_state6.var_20 = (rb_state6.var_20) - (((rb_state6.var_8) == (0x0)) ? (rb_state6.var_18) : (0xf7bbe57b));
                        rb_state6.var_21 = (rb_state6.var_21) ^ (((0xe142ef68) - (rb_state6.var_19)) - (0xb73712ba));
                        rb_state6.var_22 = (rb_state6.var_22) - (rb_state6.var_20);
                        rb_state6.var_23 = (rb_state6.var_23) - (rb_state6.var_21);
                        rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_22) : (0x62c266cc));
                        rb_state6.var_25 = (rb_state6.var_25) + (rb_state6.var_23);
                        rb_state6.var_26 = (rb_state6.var_26) ^ (((((0x4a77eb7e) ^ (rb_state6.var_24)) ^ (rb_state6.var_10)) ^ (rb_state6.var_11)) - (rb_state6.var_25));
                        rb_state6.var_5 = (rb_state6.var_5) ^ (rb_state6.var_26);
                        rb_state6.var_4 = rb_state6.var_5;
                        pthread_mutex_unlock(&(rb_state6.lock_43));
                    }
                    if ((rb_state6.var_3) == (0x59a63acf)) {
                        if (!((rb_state6.var_4) == (rb_state6.var_5))) {
                            racebench_trigger(6);
                        }
                    }
                    #endif
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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) ^ (141602 < rb_input_size ? (uint32_t)rb_input[141602] : 0x27c2b37f);
    if ((rb_state1.var_0) == (0x0)) {
        rb_state1.var_4 = rb_state1.var_1;
    }
    if ((rb_state1.var_0) == (0x0)) {
        if (!((rb_state1.var_1) == (rb_state1.var_4))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_15 = (rb_state2.var_15) ^ (1859 < rb_input_size ? (uint32_t)rb_input[1859] : 0x41e05c8e);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (20821 < rb_input_size ? (uint32_t)rb_input[20821] : 0xad908fa);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_2);
    rb_state4.var_3 = (rb_state4.var_3) + (((rb_state4.var_1) == (0x225d1ec)) ? (rb_state4.var_3) : (0x44bddb0a));
    rb_state4.var_2 = (rb_state4.var_2) ^ (94811 < rb_input_size ? (uint32_t)rb_input[94811] : 0x8201be32);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (((rb_state5.var_1) == (0xfbe48cfa)) ? (rb_state5.var_2) : (0x2fc727f1));
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_2) == (0x205ac21)) {
        rb_state7.var_23 = 0xf3b5a600;
        rb_state7.var_24 = 0xa2cd1259;
        rb_state7.var_25 = 0x236b8ad6;
        rb_state7.var_26 = (rb_state7.var_26) + (rb_state7.var_16);
        rb_state7.var_27 = (rb_state7.var_27) + (rb_state7.var_24);
        rb_state7.var_28 = (rb_state7.var_28) ^ (((((0xef167a81) ^ (0x3083f86d)) ^ (rb_state7.var_25)) + (rb_state7.var_23)) + (rb_state7.var_17));
        rb_state7.var_29 = (rb_state7.var_29) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_26) : (0xc055be07));
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_28) : (0x4c5d2372));
        rb_state7.var_31 = (rb_state7.var_31) ^ (rb_state7.var_29);
        rb_state7.var_32 = (rb_state7.var_32) ^ (((((0xf90a97df) - (rb_state7.var_30)) + (0x51d2ade1)) - (rb_state7.var_31)) ^ (0xce25e7d7));
        rb_state7.var_3 = (rb_state7.var_3) + (rb_state7.var_32);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_46));
        if ((rb_state8.var_4) != (0x0)) {
            if (!((rb_state8.var_4) == (rb_state8.var_35))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_46));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) - (0xd42c7c8d);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_3) == (0x0)) {
        rb_state11.var_4 = rb_state11.var_2;
    }
    if ((rb_state11.var_3) == (0x0)) {
        if (!((rb_state11.var_2) == (rb_state11.var_4))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_3) == (0x6a15bee8)) {
        pthread_mutex_lock(&(rb_state13.lock_11));
        rb_state13.var_5 = 0x3201523;
        rb_state13.var_6 = (rb_state13.var_6) - (rb_state13.var_6);
        rb_state13.var_7 = (rb_state13.var_7) + (((((0xa6fea2c3) + (rb_state13.var_5)) + (rb_state13.var_3)) - (rb_state13.var_4)) - (rb_state13.var_7));
        rb_state13.var_8 = (rb_state13.var_8) + (127886 < rb_input_size ? (uint32_t)rb_input[127886] : 0x2efe27c5);
        rb_state13.var_9 = (rb_state13.var_9) + (((((0xaa84481b) - (rb_state13.var_6)) - (rb_state13.var_7)) + (rb_state13.var_9)) + (rb_state13.var_8));
        rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_9) : (0x68527a62));
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_10);
        pthread_mutex_unlock(&(rb_state13.lock_11));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) ^ ((0x3d767498) + (rb_state15.var_1));
    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_2);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_3);
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x3abb3fa6)) {
        pthread_mutex_lock(&(rb_state18.lock_11));
        rb_state18.var_4 = rb_state18.var_1;
        pthread_mutex_unlock(&(rb_state18.lock_11));
    }
    if ((rb_state18.var_0) == (0x3abb3fa6)) {
        if (!((rb_state18.var_1) == (rb_state18.var_4))) {
            racebench_trigger(18);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (101218 < rb_input_size ? (uint32_t)rb_input[101218] : 0x418716d6);
    if ((rb_state19.var_3) == (0x0)) {
        rb_state19.var_5 = 0x8ee8c8c7;
        rb_state19.var_6 = 0x3239e448;
        rb_state19.var_7 = (rb_state19.var_7) - ((0x3ae103df) - (0xa927f370));
        rb_state19.var_8 = (rb_state19.var_8) + (99345 < rb_input_size ? (uint32_t)rb_input[99345] : 0x1784dde2);
        rb_state19.var_9 = (rb_state19.var_9) ^ (((rb_state19.var_6) == (0x3239e448)) ? (rb_state19.var_5) : (0xcecd206d));
        rb_state19.var_10 = (rb_state19.var_10) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_7) : (0x3fb90d3b));
        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_8) == (0x3b)) ? (rb_state19.var_9) : (0xb6f81c48));
        rb_state19.var_12 = (rb_state19.var_12) + (((0x63d08a6a) + (0xcfd9cd4e)) - (rb_state19.var_10));
        rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_11) == (0x8ee8c8c7)) ? (rb_state19.var_12) : (0x222c9a38));
        rb_state19.var_0 = (rb_state19.var_0) ^ (rb_state19.var_13);
    }
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - (0x3ed6382b);
    if ((rb_state0.var_2) == (0xffffffc4)) {
        rb_state0.var_3 = rb_state0.var_0;
    }
    if ((rb_state0.var_2) == (0xffffffc4)) {
        if (!((rb_state0.var_0) == (rb_state0.var_3))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_3 = (rb_state1.var_3) - ((0xd1aca0ab) + (rb_state1.var_3));
    rb_state1.var_1 = (rb_state1.var_1) - (((rb_state1.var_3) == (0x2e535f55)) ? (rb_state1.var_2) : (0x505a2e85));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + ((0x44d8f12c) + (0xb965a584));
    rb_state2.var_1 = (rb_state2.var_1) - ((0x1485a04c) - (0x78acba25));
    rb_state2.var_14 = (rb_state2.var_14) ^ (28746 < rb_input_size ? (uint32_t)rb_input[28746] : 0x1a64f26f);
    if ((rb_state2.var_17) == (0x1)) {
        pthread_mutex_lock(&(rb_state2.lock_40));
        rb_state2.var_20 = 0xe1eea4d9;
        rb_state2.var_21 = (rb_state2.var_21) ^ (((rb_state2.var_18) == (0x0)) ? (rb_state2.var_20) : (0x5c7e2066));
        rb_state2.var_22 = (rb_state2.var_22) - (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_21) : (0xa87a48b7));
        rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_22);
        rb_state2.var_18 = rb_state2.var_19;
        pthread_mutex_unlock(&(rb_state2.lock_40));
    }
    if ((rb_state2.var_17) == (0x1)) {
        rb_state2.var_23 = 0x693bcb1e;
        rb_state2.var_24 = 0x4904812b;
        rb_state2.var_25 = (rb_state2.var_25) - (rb_state2.var_21);
        rb_state2.var_26 = (rb_state2.var_26) - (rb_state2.var_22);
        rb_state2.var_27 = (rb_state2.var_27) + (((0xe27f79aa) ^ (0xc18145da)) + (rb_state2.var_24));
        rb_state2.var_28 = (rb_state2.var_28) - (((((0x9569f258) ^ (0xd4c1b0b7)) ^ (rb_state2.var_23)) - (rb_state2.var_20)) ^ (0xc6e4bbc5));
        rb_state2.var_29 = (rb_state2.var_29) - (((((0xeb61edf1) - (rb_state2.var_26)) + (rb_state2.var_24)) ^ (rb_state2.var_25)) - (rb_state2.var_23));
        rb_state2.var_30 = (rb_state2.var_30) - (((0xc78cf70) + (rb_state2.var_27)) ^ (rb_state2.var_25));
        rb_state2.var_31 = (rb_state2.var_31) + (rb_state2.var_28);
        rb_state2.var_32 = (rb_state2.var_32) ^ (rb_state2.var_29);
        rb_state2.var_33 = (rb_state2.var_33) ^ (rb_state2.var_30);
        rb_state2.var_34 = (rb_state2.var_34) ^ (((0x32568fda) + (rb_state2.var_31)) ^ (0x15778361));
        rb_state2.var_35 = (rb_state2.var_35) - (((rb_state2.var_26) == (0x0)) ? (rb_state2.var_32) : (0xddf1b94c));
        rb_state2.var_36 = (rb_state2.var_36) ^ (((((0x750904d2) ^ (rb_state2.var_34)) + (rb_state2.var_33)) ^ (rb_state2.var_27)) + (0xa6753bad));
        rb_state2.var_37 = (rb_state2.var_37) ^ (((rb_state2.var_28) == (0x0)) ? (rb_state2.var_35) : (0xedca50a2));
        rb_state2.var_38 = (rb_state2.var_38) ^ (((rb_state2.var_29) == (0x0)) ? (rb_state2.var_36) : (0x3122a845));
        rb_state2.var_39 = (rb_state2.var_39) + (((((0xef640d8b) ^ (rb_state2.var_30)) ^ (rb_state2.var_37)) - (0x2bb739a7)) + (rb_state2.var_38));
        rb_state2.var_18 = (rb_state2.var_18) + (rb_state2.var_39);
    }
    if ((rb_state2.var_17) == (0x1)) {
        if (!((rb_state2.var_18) == (rb_state2.var_19))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + ((0xccc8c4c0) - (rb_state4.var_0));
    if ((rb_state4.var_0) == (0xccc8c4c0)) {
        if ((rb_state4.var_4) != (0x0)) {
            if (!((rb_state4.var_4) == (rb_state4.var_31))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) + (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_1) : (0xbfb9576d));
    rb_state5.var_0 = (rb_state5.var_0) ^ ((0xdfc8bd89) - (0xbd34394d));
    if ((rb_state5.var_0) == (0x1b049784)) {
        rb_state5.var_20 = 0x34bd67a7;
        rb_state5.var_21 = (rb_state5.var_21) ^ ((0x222b30b3) ^ (rb_state5.var_12));
        rb_state5.var_22 = (rb_state5.var_22) ^ (((0xccb7228a) ^ (rb_state5.var_20)) ^ (0xa957fc6c));
        rb_state5.var_23 = (rb_state5.var_23) + ((0xa2f569e3) + (rb_state5.var_13));
        rb_state5.var_24 = (rb_state5.var_24) + (((((0xdf87fbb2) + (0x149b1db2)) - (0x8a5f40a5)) ^ (rb_state5.var_11)) ^ (rb_state5.var_10));
        rb_state5.var_25 = (rb_state5.var_25) - (rb_state5.var_21);
        rb_state5.var_26 = (rb_state5.var_26) - (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_23) : (0x7b541227));
        rb_state5.var_27 = (rb_state5.var_27) - (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_25) : (0x9c0883ee));
        rb_state5.var_28 = (rb_state5.var_28) - (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_26) : (0xb50f213b));
        rb_state5.var_29 = (rb_state5.var_29) + (((0xcf7c965b) + (rb_state5.var_27)) + (rb_state5.var_15));
        rb_state5.var_30 = (rb_state5.var_30) + (((0xa42aa2c0) ^ (rb_state5.var_16)) ^ (rb_state5.var_28));
        rb_state5.var_31 = (rb_state5.var_31) - (((rb_state5.var_29) == (0x0)) ? (rb_state5.var_30) : (0x36c05b0));
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_1) : (0x74b2b6e0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (0xe3bac365);
    rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_0) : (0x56e95fd4));
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_1) == (0x0)) ? (rb_state10.var_0) : (0x559d9cd2));
    rb_state10.var_2 = (rb_state10.var_2) ^ ((0x5cca89fa) - (0x1ca0533a));
    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_2) : (0x50fb4a3b));
    if ((rb_state10.var_2) == (0xc16cb5c1)) {
        pthread_mutex_lock(&(rb_state10.lock_15));
        rb_state10.var_5 = 0x68241bf4;
        rb_state10.var_6 = 0xdcac7c1f;
        rb_state10.var_7 = (rb_state10.var_7) - (((((0xb2195280) ^ (rb_state10.var_4)) - (rb_state10.var_3)) - (rb_state10.var_6)) - (0xdb7a78fc));
        rb_state10.var_8 = (rb_state10.var_8) ^ (((((0xdabdee4d) - (rb_state10.var_5)) ^ (0x1eb23a66)) - (rb_state10.var_7)) ^ (rb_state10.var_5));
        rb_state10.var_4 = (rb_state10.var_4) + (rb_state10.var_8);
        rb_state10.var_3 = rb_state10.var_4;
        pthread_mutex_unlock(&(rb_state10.lock_15));
    }
    if ((rb_state10.var_2) == (0xc16cb5c1)) {
        if (!((rb_state10.var_3) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - ((0x4e37c92) - (0xd06cd594));
    rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_2) : (0x2a3d9121));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (0xc086d89e);
    rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_2);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x380182ff));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0xcd813477) + (0x84e6745c));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) + (0x159478cc);
    rb_state15.var_3 = (rb_state15.var_3) - (124227 < rb_input_size ? (uint32_t)rb_input[124227] : 0xf722b16f);
    if ((rb_state15.var_3) == (0xffffff0c)) {
        pthread_mutex_lock(&(rb_state15.lock_37));
        rb_state15.var_6 = 0x658c1627;
        rb_state15.var_7 = 0xc16b67b7;
        rb_state15.var_8 = (rb_state15.var_8) + (((((0x7797f1d2) ^ (rb_state15.var_4)) - (rb_state15.var_5)) + (rb_state15.var_6)) ^ (0xc5227cff));
        rb_state15.var_9 = (rb_state15.var_9) ^ (23124 < rb_input_size ? (uint32_t)rb_input[23124] : 0xe8a43bc3);
        rb_state15.var_10 = (rb_state15.var_10) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_7) : (0x465a812c));
        rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0xb6de5fd9));
        rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_9) : (0x4275b707));
        rb_state15.var_13 = (rb_state15.var_13) + (((((0x6c785a33) + (0xf44e0301)) + (rb_state15.var_10)) ^ (rb_state15.var_9)) - (rb_state15.var_11));
        rb_state15.var_14 = (rb_state15.var_14) ^ (((0xfc2b65f9) + (0x41350d03)) - (rb_state15.var_12));
        rb_state15.var_15 = (rb_state15.var_15) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_13) : (0xbf623d4c));
        rb_state15.var_16 = (rb_state15.var_16) - (rb_state15.var_14);
        rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_15) : (0xfd05c2ff));
        rb_state15.var_18 = (rb_state15.var_18) ^ (((((0x606c2796) + (0x48762cbc)) - (rb_state15.var_16)) - (rb_state15.var_17)) + (0x85d6fa65));
        rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_18);
        rb_state15.var_4 = rb_state15.var_5;
        pthread_mutex_unlock(&(rb_state15.lock_37));
    }
    if ((rb_state15.var_3) == (0xffffff0c)) {
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (128088 < rb_input_size ? (uint32_t)rb_input[128088] : 0xff26a0cf);
    rb_state17.var_2 = (rb_state17.var_2) ^ (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0xe9ad48de));
    if ((rb_state17.var_3) == (0x0)) {
        rb_state17.var_4 = rb_state17.var_0;
    }
    if ((rb_state17.var_3) == (0x0)) {
        if (!((rb_state17.var_0) == (rb_state17.var_4))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_1) : (0xa475d584));
    rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_2);
    rb_state19.var_3 = (rb_state19.var_3) ^ (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_3) : (0x5b3e3dcd));
    rb_state19.var_3 = (rb_state19.var_3) + (rb_state19.var_0);
    #endif
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xa99f8789));
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_0) == (0x30c80171)) {
            pthread_mutex_lock(&(rb_state2.lock_16));
            rb_state2.var_5 = 0x4ccd96c2;
            rb_state2.var_6 = 0x7f0928ec;
            rb_state2.var_7 = (rb_state2.var_7) - (((((0x640c2690) ^ (rb_state2.var_5)) - (0x42b0eedc)) - (rb_state2.var_3)) ^ (0xc12fb2eb));
            rb_state2.var_8 = (rb_state2.var_8) ^ (((((0xaae6b1cb) - (rb_state2.var_4)) ^ (rb_state2.var_6)) + (rb_state2.var_7)) - (0xab932a26));
            rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_8);
            rb_state2.var_3 = rb_state2.var_4;
            pthread_mutex_unlock(&(rb_state2.lock_16));
        }
        if ((rb_state2.var_0) == (0x30c80171)) {
            rb_state2.var_17 = !((rb_state2.var_3) == (rb_state2.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) - (135898 < rb_input_size ? (uint32_t)rb_input[135898] : 0x6248363b);
        if ((rb_state4.var_0) == (0xccc8c4c0)) {
            pthread_mutex_lock(&(rb_state4.lock_41));
            rb_state4.var_16 = 0x794caff1;
            rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_16) : (0xa8f6e38e));
            rb_state4.var_18 = (rb_state4.var_18) + (((0xa5d9cbfa) - (rb_state4.var_10)) + (rb_state4.var_9));
            rb_state4.var_19 = (rb_state4.var_19) + (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_11) : (0xc33c7c36));
            rb_state4.var_20 = (rb_state4.var_20) - (((0x5bdbed6d) ^ (rb_state4.var_17)) ^ (0x4d033709));
            rb_state4.var_21 = (rb_state4.var_21) - (((0x20457b4c) - (0x7e29abe7)) ^ (rb_state4.var_18));
            rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_19);
            rb_state4.var_23 = (rb_state4.var_23) - (((0xcadb906) ^ (rb_state4.var_20)) + (rb_state4.var_13));
            rb_state4.var_24 = (rb_state4.var_24) - (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_21) : (0x8adaa128));
            rb_state4.var_25 = (rb_state4.var_25) - (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_22) : (0x40f25e9b));
            rb_state4.var_26 = (rb_state4.var_26) ^ (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_23) : (0xa67c2535));
            rb_state4.var_27 = (rb_state4.var_27) + (((((0xef738e0b) - (rb_state4.var_25)) - (rb_state4.var_24)) - (0x312dcef6)) - (0x75995bcb));
            rb_state4.var_28 = (rb_state4.var_28) ^ (rb_state4.var_26);
            rb_state4.var_29 = (rb_state4.var_29) ^ (((rb_state4.var_17) == (0x0)) ? (rb_state4.var_27) : (0x8080c466));
            rb_state4.var_30 = (rb_state4.var_30) ^ (((((0x2249850) - (rb_state4.var_29)) ^ (rb_state4.var_18)) ^ (0x23c6cbda)) ^ (rb_state4.var_28));
            rb_state4.var_15 = (rb_state4.var_15) + (rb_state4.var_30);
            rb_state4.var_4 = rb_state4.var_15;
            pthread_mutex_unlock(&(rb_state4.lock_41));
        }
        if ((rb_state4.var_0) == (0xccc8c4c0)) {
            pthread_mutex_lock(&(rb_state4.lock_41));
            rb_state4.var_32 = 0x5fcf8182;
            rb_state4.var_33 = 0xb73fd633;
            rb_state4.var_34 = (rb_state4.var_34) + (((((0x5b57681e) + (0x9434a9f9)) + (0xfefe3050)) + (rb_state4.var_32)) + (rb_state4.var_19));
            rb_state4.var_35 = (rb_state4.var_35) + (rb_state4.var_33);
            rb_state4.var_36 = (rb_state4.var_36) ^ (rb_state4.var_34);
            rb_state4.var_37 = (rb_state4.var_37) + (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_35) : (0xabb51d27));
            rb_state4.var_38 = (rb_state4.var_38) - (rb_state4.var_36);
            rb_state4.var_39 = (rb_state4.var_39) ^ (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_37) : (0xdcf6b74a));
            rb_state4.var_40 = (rb_state4.var_40) ^ (((((0x75976e9a) ^ (0x1d095cf7)) + (rb_state4.var_38)) - (rb_state4.var_39)) + (rb_state4.var_22));
            rb_state4.var_31 = (rb_state4.var_31) + (rb_state4.var_40);
            rb_state4.var_4 = rb_state4.var_31;
            pthread_mutex_unlock(&(rb_state4.lock_41));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + ((0x8523be55) + (rb_state5.var_2));
        if ((rb_state5.var_0) == (0x1b049784)) {
            pthread_mutex_lock(&(rb_state5.lock_32));
            rb_state5.var_5 = 0xb23e3c25;
            rb_state5.var_6 = (rb_state5.var_6) ^ (0xb01992a6);
            rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_5) : (0x7a715b21));
            rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_4);
            rb_state5.var_9 = (rb_state5.var_9) ^ (rb_state5.var_3);
            rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_5) : (0x6c5ce725));
            rb_state5.var_11 = (rb_state5.var_11) + (((0x4f5a135a) + (0x3760ad01)) + (rb_state5.var_6));
            rb_state5.var_12 = (rb_state5.var_12) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_8) : (0x1a2760a3));
            rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x5a8dce2c));
            rb_state5.var_14 = (rb_state5.var_14) ^ (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_10) : (0x7cea0d53));
            rb_state5.var_15 = (rb_state5.var_15) - (rb_state5.var_11);
            rb_state5.var_16 = (rb_state5.var_16) - (((((0x64b65764) - (rb_state5.var_13)) - (0xc427955a)) - (rb_state5.var_12)) - (0x91149f5));
            rb_state5.var_17 = (rb_state5.var_17) + (((((0xb7915e98) + (rb_state5.var_14)) - (0x338f9e66)) + (rb_state5.var_15)) ^ (0xed132745));
            rb_state5.var_18 = (rb_state5.var_18) - (((0xdd3571fd) - (rb_state5.var_16)) + (0x7a846f31));
            rb_state5.var_19 = (rb_state5.var_19) - (((((0xa8fa44cc) ^ (rb_state5.var_18)) - (rb_state5.var_17)) - (0xcc1ef54a)) ^ (0x5d6c2a98));
            rb_state5.var_4 = (rb_state5.var_4) - (rb_state5.var_19);
            rb_state5.var_3 = rb_state5.var_4;
            pthread_mutex_unlock(&(rb_state5.lock_32));
        }
        if ((rb_state5.var_0) == (0x1b049784)) {
            pthread_mutex_lock(&(rb_state5.lock_32));
            if (!((rb_state5.var_3) == (rb_state5.var_4))) {
                racebench_trigger(5);
            }
            pthread_mutex_unlock(&(rb_state5.lock_32));
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_3);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + ((0x81427f01) ^ (rb_state10.var_0));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_1);
        rb_state11.var_3 = (rb_state11.var_3) ^ (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_3) : (0x8b4d3e3e));
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_2) == (0x7ef24ec4)) {
            rb_state12.var_5 = 0x9d3fcfb1;
            rb_state12.var_6 = 0x21df72af;
            rb_state12.var_7 = 0xa726bc76;
            rb_state12.var_8 = (rb_state12.var_8) - (((((0x42a02e54) ^ (0x8c979c77)) + (rb_state12.var_7)) - (rb_state12.var_6)) + (0x391475d7));
            rb_state12.var_9 = (rb_state12.var_9) ^ (((0xefdd8217) - (0x157e3f41)) - (rb_state12.var_5));
            rb_state12.var_10 = (rb_state12.var_10) + (rb_state12.var_8);
            rb_state12.var_11 = (rb_state12.var_11) + (((((0xcaa6b67) - (rb_state12.var_9)) ^ (rb_state12.var_5)) ^ (rb_state12.var_10)) - (rb_state12.var_4));
            rb_state12.var_3 = (rb_state12.var_3) ^ (rb_state12.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) + ((0x45ff5172) ^ (0xf0f58e06));
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state14.lock_14));
            rb_state14.var_4 = 0x61e60256;
            rb_state14.var_5 = 0x594e03c6;
            rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_4);
            rb_state14.var_7 = (rb_state14.var_7) + (((((0x76313af0) ^ (rb_state14.var_4)) ^ (rb_state14.var_3)) ^ (0xa591df23)) ^ (0x37c061fd));
            rb_state14.var_8 = (rb_state14.var_8) ^ (rb_state14.var_5);
            rb_state14.var_9 = (rb_state14.var_9) + (((((0xde130475) + (rb_state14.var_6)) + (rb_state14.var_7)) + (0xa13e976)) + (rb_state14.var_5));
            rb_state14.var_10 = (rb_state14.var_10) + (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_8) : (0xc665c296));
            rb_state14.var_11 = (rb_state14.var_11) ^ (rb_state14.var_9);
            rb_state14.var_12 = (rb_state14.var_12) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_10) : (0x17d8ebb2));
            rb_state14.var_13 = (rb_state14.var_13) + (((((0x1bc1a1ab) ^ (0x5a653391)) + (0x2d2d5f9a)) - (rb_state14.var_11)) + (rb_state14.var_12));
            rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_13);
            pthread_mutex_unlock(&(rb_state14.lock_14));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_3) == (0xffffff0c)) {
            rb_state15.var_19 = (rb_state15.var_19) + (0x6f4fd686);
            rb_state15.var_20 = (rb_state15.var_20) ^ (19615 < rb_input_size ? (uint32_t)rb_input[19615] : 0xf76f2717);
            rb_state15.var_21 = (rb_state15.var_21) - (rb_state15.var_12);
            rb_state15.var_22 = (rb_state15.var_22) ^ (((0xb4c966f) ^ (rb_state15.var_14)) + (0xb55c6ca8));
            rb_state15.var_23 = (rb_state15.var_23) - (rb_state15.var_13);
            rb_state15.var_24 = (rb_state15.var_24) + (((((0xf96a9ed4) ^ (rb_state15.var_20)) + (rb_state15.var_19)) + (0x947c6553)) + (0x66a1dbb0));
            rb_state15.var_25 = (rb_state15.var_25) ^ (((0xf18de8ab) ^ (rb_state15.var_21)) - (rb_state15.var_15));
            rb_state15.var_26 = (rb_state15.var_26) + (((rb_state15.var_16) == (0x0)) ? (rb_state15.var_22) : (0x7b69e841));
            rb_state15.var_27 = (rb_state15.var_27) ^ (rb_state15.var_23);
            rb_state15.var_28 = (rb_state15.var_28) + (((0xc07b5b97) ^ (rb_state15.var_24)) - (rb_state15.var_17));
            rb_state15.var_29 = (rb_state15.var_29) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0x762c566e));
            rb_state15.var_30 = (rb_state15.var_30) ^ (((0xd5807882) + (rb_state15.var_27)) - (rb_state15.var_18));
            rb_state15.var_31 = (rb_state15.var_31) - (rb_state15.var_28);
            rb_state15.var_32 = (rb_state15.var_32) + (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_29) : (0x299301c7));
            rb_state15.var_33 = (rb_state15.var_33) - (((((0x1ac6ff27) - (rb_state15.var_21)) + (rb_state15.var_30)) - (rb_state15.var_31)) - (rb_state15.var_20));
            rb_state15.var_34 = (rb_state15.var_34) ^ (((rb_state15.var_22) == (0x0)) ? (rb_state15.var_32) : (0x7397071a));
            rb_state15.var_35 = (rb_state15.var_35) + (((0x9d50827b) + (rb_state15.var_33)) + (rb_state15.var_23));
            rb_state15.var_36 = (rb_state15.var_36) - (((rb_state15.var_34) == (0x0)) ? (rb_state15.var_35) : (0x4c0c8ede));
            rb_state15.var_4 = (rb_state15.var_4) + (rb_state15.var_36);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (((rb_state17.var_0) == (0x9)) ? (rb_state17.var_1) : (0xd6b87ef8));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (162410 < rb_input_size ? (uint32_t)rb_input[162410] : 0xecf0e534);
        rb_state18.var_1 = (rb_state18.var_1) - ((0xbe196699) - (0x6fb03e9f));
        #endif
        #ifdef RACEBENCH_BUG_19
        if ((rb_state19.var_3) == (0x0)) {
            rb_state19.var_4 = rb_state19.var_0;
        }
        if ((rb_state19.var_3) == (0x0)) {
            if (!((rb_state19.var_0) == (rb_state19.var_4))) {
                racebench_trigger(19);
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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x49ca4e96));
        rb_state0.var_2 = (rb_state0.var_2) - (101250 < rb_input_size ? (uint32_t)rb_input[101250] : 0x76b4135f);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + (0x379c3496);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xccb7fc71));
        rb_state2.var_1 = (rb_state2.var_1) + (0xcbc3471b);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - ((0x3a974179) - (0xe5f932cb));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xa23c724f));
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_1);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_0);
        rb_state7.var_2 = (rb_state7.var_2) - (rb_state7.var_2);
        rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_1) == (0x0)) ? (rb_state7.var_0) : (0x205ac5f));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xdb5beb15));
        rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0xe299c072));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_2);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
        rb_state11.var_2 = (rb_state11.var_2) - (0x27261e6d);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (54250 < rb_input_size ? (uint32_t)rb_input[54250] : 0x41a4f6e5);
        rb_state12.var_3 = (rb_state12.var_3) + (((rb_state12.var_0) == (0x3d)) ? (rb_state12.var_3) : (0xc71b5113));
        rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (0x4b286417);
        rb_state13.var_2 = (rb_state13.var_2) - (1429 < rb_input_size ? (uint32_t)rb_input[1429] : 0x6b62bce);
        rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_1) == (0x4b286417)) ? (rb_state13.var_0) : (0x4cf8e7dc));
        rb_state13.var_2 = (rb_state13.var_2) ^ (((rb_state13.var_1) == (0x4b286417)) ? (rb_state13.var_0) : (0xb2bf01ca));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (0xebcbedbb);
        rb_state14.var_1 = (rb_state14.var_1) ^ ((0xf810f504) ^ (0x958f4bd5));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (160486 < rb_input_size ? (uint32_t)rb_input[160486] : 0x762605ba);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0x74e0db35));
        if ((rb_state17.var_3) == (0x0)) {
            rb_state17.var_5 = 0x3e87db8f;
            rb_state17.var_6 = (rb_state17.var_6) ^ (((((0xca86ffc2) - (rb_state17.var_6)) ^ (rb_state17.var_5)) - (0x17b46c8e)) - (rb_state17.var_4));
            rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (165699 < rb_input_size ? (uint32_t)rb_input[165699] : 0xcac947d7);
        rb_state18.var_0 = (rb_state18.var_0) + ((0x3abb3fa6) - (rb_state18.var_0));
        rb_state18.var_1 = (rb_state18.var_1) ^ (0x478bde71);
        rb_state18.var_3 = (rb_state18.var_3) ^ (rb_state18.var_2);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (rb_state19.var_0);
        #endif
        targs[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_1);
        rb_state0.var_1 = (rb_state0.var_1) - ((0x35cc743b) - (0xd882df41));
        if ((rb_state0.var_2) == (0xffffffc4)) {
            rb_state0.var_4 = 0x395a46c2;
            rb_state0.var_5 = 0x9d13ce4f;
            rb_state0.var_6 = 0x30bb0658;
            rb_state0.var_7 = (rb_state0.var_7) ^ (((((0x4540cf8) + (rb_state0.var_6)) - (rb_state0.var_3)) + (rb_state0.var_5)) - (rb_state0.var_4));
            rb_state0.var_8 = (rb_state0.var_8) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0xff2165e6));
            rb_state0.var_9 = (rb_state0.var_9) + (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_7) : (0x74aae32f));
            rb_state0.var_10 = (rb_state0.var_10) + (((((0x8b23049b) - (rb_state0.var_8)) - (rb_state0.var_8)) ^ (rb_state0.var_9)) ^ (rb_state0.var_7));
            rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_10);
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_1) : (0xd7ac36a9));
        rb_state1.var_1 = (rb_state1.var_1) ^ (56842 < rb_input_size ? (uint32_t)rb_input[56842] : 0xb1b02203);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_1);
        rb_state2.var_2 = (rb_state2.var_2) - (0x6330cd47);
        rb_state2.var_11 = (rb_state2.var_11) ^ (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_9) : (0xa2b6ce7));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (129981 < rb_input_size ? (uint32_t)rb_input[129981] : 0x9c8e9c92);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (0x722833e);
        rb_state7.var_2 = (rb_state7.var_2) - (130897 < rb_input_size ? (uint32_t)rb_input[130897] : 0x1595b8ea);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ (0x6ba7561a);
        rb_state8.var_3 = (rb_state8.var_3) - (0xfbfbdd18);
        rb_state8.var_2 = (rb_state8.var_2) - (0x60db9b7a);
        rb_state8.var_3 = (rb_state8.var_3) ^ (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_0) : (0x3f476a39));
        rb_state8.var_2 = (rb_state8.var_2) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_3) : (0x6a78a4e9));
        if ((rb_state8.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_46));
            rb_state8.var_25 = 0xe62e727;
            rb_state8.var_26 = (rb_state8.var_26) ^ (((rb_state8.var_25) == (0xe62e727)) ? (rb_state8.var_18) : (0xab28b5a9));
            rb_state8.var_27 = (rb_state8.var_27) ^ (126461 < rb_input_size ? (uint32_t)rb_input[126461] : 0x18b7b5cd);
            rb_state8.var_28 = (rb_state8.var_28) - (((((0x13dfa9fe) ^ (rb_state8.var_26)) ^ (0xd727896c)) + (rb_state8.var_17)) ^ (0x35f7deb4));
            rb_state8.var_29 = (rb_state8.var_29) ^ (rb_state8.var_27);
            rb_state8.var_30 = (rb_state8.var_30) ^ (((rb_state8.var_19) == (0x0)) ? (rb_state8.var_28) : (0x8ee71bde));
            rb_state8.var_31 = (rb_state8.var_31) - (((0x94141fc9) + (0x4eca09d3)) ^ (rb_state8.var_29));
            rb_state8.var_32 = (rb_state8.var_32) + (((0x83ddc1fd) + (rb_state8.var_20)) - (rb_state8.var_30));
            rb_state8.var_33 = (rb_state8.var_33) - (rb_state8.var_31);
            rb_state8.var_34 = (rb_state8.var_34) ^ (((rb_state8.var_32) == (0x74edc023)) ? (rb_state8.var_33) : (0xbbcad515));
            rb_state8.var_24 = (rb_state8.var_24) ^ (rb_state8.var_34);
            rb_state8.var_4 = rb_state8.var_24;
            pthread_mutex_unlock(&(rb_state8.lock_46));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x90fabca4));
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state11.lock_17));
            rb_state11.var_5 = 0xf0a8ce19;
            rb_state11.var_6 = 0x8041160f;
            rb_state11.var_7 = (rb_state11.var_7) ^ (3820 < rb_input_size ? (uint32_t)rb_input[3820] : 0x16cccd1);
            rb_state11.var_8 = (rb_state11.var_8) - (((rb_state11.var_4) == (0x0)) ? (rb_state11.var_6) : (0xcc788a7e));
            rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_5) : (0xccb02684));
            rb_state11.var_10 = (rb_state11.var_10) - (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xdbcf1c85));
            rb_state11.var_11 = (rb_state11.var_11) + (rb_state11.var_8);
            rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_9) : (0x364d349));
            rb_state11.var_13 = (rb_state11.var_13) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_11) : (0x2aba70af));
            rb_state11.var_14 = (rb_state11.var_14) + (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_12) : (0xe90869d2));
            rb_state11.var_15 = (rb_state11.var_15) + (((0xde2bc1c9) + (rb_state11.var_13)) - (0x6a690081));
            rb_state11.var_16 = (rb_state11.var_16) - (((((0xf1a8ff48) + (0xef02096d)) + (rb_state11.var_15)) + (rb_state11.var_14)) ^ (0xd15d6f5c));
            rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_16);
            pthread_mutex_unlock(&(rb_state11.lock_17));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) - ((0x26bf7150) ^ (rb_state12.var_0));
        rb_state12.var_1 = (rb_state12.var_1) ^ (162057 < rb_input_size ? (uint32_t)rb_input[162057] : 0xd22d0fc2);
        if ((rb_state12.var_2) == (0x7ef24ec4)) {
            pthread_mutex_lock(&(rb_state12.lock_12));
            rb_state12.var_4 = rb_state12.var_3;
            pthread_mutex_unlock(&(rb_state12.lock_12));
        }
        if ((rb_state12.var_2) == (0x7ef24ec4)) {
            if (!((rb_state12.var_3) == (rb_state12.var_4))) {
                racebench_trigger(12);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) - ((0xd55b7d72) ^ (rb_state13.var_1));
        if ((rb_state13.var_3) == (0x6a15bee8)) {
            rb_state13.var_4 = rb_state13.var_2;
        }
        if ((rb_state13.var_3) == (0x6a15bee8)) {
            pthread_mutex_lock(&(rb_state13.lock_11));
            if (!((rb_state13.var_2) == (rb_state13.var_4))) {
                racebench_trigger(13);
            }
            pthread_mutex_unlock(&(rb_state13.lock_11));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (0x62105586);
        rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0x996444e8)) ? (rb_state14.var_1) : (0xff444c80));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_0);
        rb_state14.var_1 = (rb_state14.var_1) + (171082 < rb_input_size ? (uint32_t)rb_input[171082] : 0x65adea05);
        rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_2) : (0xced495bd));
        if ((rb_state14.var_2) == (0x0)) {
            rb_state14.var_3 = rb_state14.var_0;
        }
        if ((rb_state14.var_2) == (0x0)) {
            if (!((rb_state14.var_0) == (rb_state14.var_3))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_0) : (0xf7083998));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) + ((0xcafc1bcf) + (0xd8b69e56));
        rb_state17.var_2 = (rb_state17.var_2) - (((rb_state17.var_2) == (0x9)) ? (rb_state17.var_2) : (0x3db105fc));
        rb_state17.var_2 = (rb_state17.var_2) - (171639 < rb_input_size ? (uint32_t)rb_input[171639] : 0xc463242b);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) + (0x4856f2d5);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) - ((0x88cec0e1) - (0xeecf4a7a));
        rb_state19.var_1 = (rb_state19.var_1) ^ (0xe54b5d2b);
        #endif
        targs[i].frames = framenum;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (0xc36dbc03);
        rb_state0.var_1 = (rb_state0.var_1) + ((0x9e3671ec) + (rb_state0.var_0));
        rb_state0.var_2 = (rb_state0.var_2) ^ (((rb_state0.var_2) == (0xffffffc4)) ? (rb_state0.var_2) : (0xb44f53d7));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_0);
        if ((rb_state1.var_0) == (0x0)) {
            rb_state1.var_5 = 0xe52f146f;
            rb_state1.var_6 = 0xf7075f9a;
            rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_5) : (0x79e64035));
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0x62ce4095));
            rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_4);
            rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_7) : (0x348bdf6d));
            rb_state1.var_11 = (rb_state1.var_11) + (((((0x996e3ace) + (rb_state1.var_9)) + (rb_state1.var_8)) + (rb_state1.var_8)) - (rb_state1.var_9));
            rb_state1.var_12 = (rb_state1.var_12) - (((((0x233ca0e3) - (rb_state1.var_10)) + (rb_state1.var_11)) - (rb_state1.var_10)) + (0xa55eeb85));
            rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) - (0xaf6c30c7);
        rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_2);
        if ((rb_state2.var_0) == (0x30c80171)) {
            rb_state2.var_9 = 0x4b84c6bf;
            rb_state2.var_10 = 0x58b52c8;
            rb_state2.var_11 = (rb_state2.var_11) ^ (rb_state2.var_5);
            rb_state2.var_12 = (rb_state2.var_12) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0xc56e64d9));
            rb_state2.var_13 = (rb_state2.var_13) ^ (((0xeb950104) - (rb_state2.var_11)) + (rb_state2.var_6));
            rb_state2.var_14 = (rb_state2.var_14) - (((0x40f18bca) - (0x7958332d)) + (rb_state2.var_12));
            rb_state2.var_15 = (rb_state2.var_15) - (((((0xc106c033) - (rb_state2.var_7)) - (rb_state2.var_8)) - (rb_state2.var_14)) ^ (rb_state2.var_13));
            rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_15);
        }
        rb_state2.var_12 = (rb_state2.var_12) ^ (rb_state2.var_13);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (99209 < rb_input_size ? (uint32_t)rb_input[99209] : 0x73030949);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + ((0x1b049784) - (rb_state5.var_0));
        rb_state5.var_1 = (rb_state5.var_1) ^ ((0x3eaf104) - (rb_state5.var_1));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x31a344d9));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (0x5130b3d);
        rb_state7.var_1 = (rb_state7.var_1) ^ (113309 < rb_input_size ? (uint32_t)rb_input[113309] : 0xc21f274b);
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_46));
            rb_state8.var_36 = 0xe3ea85d0;
            rb_state8.var_37 = (rb_state8.var_37) - (rb_state8.var_22);
            rb_state8.var_38 = (rb_state8.var_38) - (((((0xda9330ce) ^ (0x5706fb91)) + (0xdcb5d96e)) ^ (rb_state8.var_36)) ^ (rb_state8.var_21));
            rb_state8.var_39 = (rb_state8.var_39) + (rb_state8.var_23);
            rb_state8.var_40 = (rb_state8.var_40) + (((rb_state8.var_24) == (0xe2de29dc)) ? (rb_state8.var_37) : (0x3eaed737));
            rb_state8.var_41 = (rb_state8.var_41) + (((rb_state8.var_38) == (0x765edee3)) ? (rb_state8.var_39) : (0x8fa38ef));
            rb_state8.var_42 = (rb_state8.var_42) ^ (((0x860b092e) - (rb_state8.var_40)) - (rb_state8.var_25));
            rb_state8.var_43 = (rb_state8.var_43) ^ (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_41) : (0xfe98565a));
            rb_state8.var_44 = (rb_state8.var_44) ^ (rb_state8.var_42);
            rb_state8.var_45 = (rb_state8.var_45) ^ (((((0xea7c77a4) ^ (0xda562401)) + (rb_state8.var_44)) - (0x1e728c)) - (rb_state8.var_43));
            rb_state8.var_35 = (rb_state8.var_35) - (rb_state8.var_45);
            rb_state8.var_4 = rb_state8.var_35;
            pthread_mutex_unlock(&(rb_state8.lock_46));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - ((0xac667bc8) + (0x239c038d));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_1);
        rb_state10.var_1 = (rb_state10.var_1) ^ ((0x2a27788a) - (0xea38af14));
        if ((rb_state10.var_2) == (0xc16cb5c1)) {
            rb_state10.var_9 = 0x5c0f02ba;
            rb_state10.var_10 = 0x6619354c;
            rb_state10.var_11 = (rb_state10.var_11) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_9) : (0xd099cfee));
            rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_7) : (0x8919acd7));
            rb_state10.var_13 = (rb_state10.var_13) ^ (((((0x1baf7935) + (rb_state10.var_10)) - (rb_state10.var_9)) + (rb_state10.var_10)) + (rb_state10.var_11));
            rb_state10.var_14 = (rb_state10.var_14) - (((((0xb0d35ad5) - (rb_state10.var_12)) - (0xd7ffa05e)) - (rb_state10.var_13)) ^ (0xdcbc7ba3));
            rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_1);
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_1) : (0xda17aba6));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_2);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) + ((0x9844839c) + (0x6e8289b5));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (46028 < rb_input_size ? (uint32_t)rb_input[46028] : 0x2303f864);
        rb_state15.var_2 = (rb_state15.var_2) ^ (104874 < rb_input_size ? (uint32_t)rb_input[104874] : 0x8a317b1d);
        #endif
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_2) == (0x205ac21)) {
            pthread_mutex_lock(&(rb_state7.lock_33));
            rb_state7.var_5 = 0x44918cdf;
            rb_state7.var_6 = 0x3917bddc;
            rb_state7.var_7 = (rb_state7.var_7) ^ (((0xc6263e42) ^ (0xd043ddbc)) - (rb_state7.var_6));
            rb_state7.var_8 = (rb_state7.var_8) + (((rb_state7.var_5) == (0x44918cdf)) ? (rb_state7.var_4) : (0x730355bf));
            rb_state7.var_9 = (rb_state7.var_9) ^ (((((0xc6589dc7) + (0xff5ecd27)) ^ (rb_state7.var_3)) ^ (rb_state7.var_6)) - (rb_state7.var_5));
            rb_state7.var_10 = (rb_state7.var_10) + (((((0x5158ce91) ^ (0x4b390574)) + (rb_state7.var_7)) + (rb_state7.var_8)) - (0x698006b2));
            rb_state7.var_11 = (rb_state7.var_11) ^ (((0xfe4fb6c1) + (0xd022ab8e)) ^ (rb_state7.var_9));
            rb_state7.var_12 = (rb_state7.var_12) + (((0xbb54f934) - (rb_state7.var_10)) - (0xfa71a1a8));
            rb_state7.var_13 = (rb_state7.var_13) ^ (((0xc1c9950c) ^ (rb_state7.var_7)) ^ (rb_state7.var_11));
            rb_state7.var_14 = (rb_state7.var_14) + (((0x77291ed5) + (rb_state7.var_12)) - (rb_state7.var_8));
            rb_state7.var_15 = (rb_state7.var_15) - (((0xa938e8f) - (rb_state7.var_9)) ^ (rb_state7.var_13));
            rb_state7.var_16 = (rb_state7.var_16) ^ (((0x87bdb9e4) + (rb_state7.var_10)) + (rb_state7.var_14));
            rb_state7.var_17 = (rb_state7.var_17) ^ (rb_state7.var_15);
            rb_state7.var_18 = (rb_state7.var_18) + (((0xbaf6c4d4) ^ (rb_state7.var_16)) + (0x2198458e));
            rb_state7.var_19 = (rb_state7.var_19) ^ (((0xcbcc8bfe) ^ (rb_state7.var_17)) ^ (rb_state7.var_11));
            rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_12) == (0x32b36c37)) ? (rb_state7.var_18) : (0xa1186859));
            rb_state7.var_21 = (rb_state7.var_21) ^ (((rb_state7.var_13) == (0x6afa9b32)) ? (rb_state7.var_19) : (0x1025f7b4));
            rb_state7.var_22 = (rb_state7.var_22) - (((((0xa1a61bdc) ^ (rb_state7.var_15)) + (rb_state7.var_21)) + (rb_state7.var_14)) ^ (rb_state7.var_20));
            rb_state7.var_4 = (rb_state7.var_4) - (rb_state7.var_22);
            rb_state7.var_3 = rb_state7.var_4;
            pthread_mutex_unlock(&(rb_state7.lock_33));
        }
        if ((rb_state7.var_2) == (0x205ac21)) {
            if (!((rb_state7.var_3) == (rb_state7.var_4))) {
                racebench_trigger(7);
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