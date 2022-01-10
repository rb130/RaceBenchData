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

                    int ci = (int)((cell2->p[j % PARTICLES_PER_CELL].x - domainMin.x) / delta.x);
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_1 = (rb_state8.var_1) + ((0x711a808c) + (rb_state8.var_0));
                    #endif
                    int cj = (int)((cell2->p[j % PARTICLES_PER_CELL].y - domainMin.y) / delta.y);
                    #ifdef RACEBENCH_BUG_8
                    if ((rb_state8.var_16) == (0x830cd265)) {
                        rb_state8.var_25 = 0x85acb46e;
                        rb_state8.var_26 = 0x4bfa09ac;
                        rb_state8.var_27 = (rb_state8.var_27) + (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_20) : (0xbdcc1a85));
                        rb_state8.var_28 = (rb_state8.var_28) + (136369 < rb_input_size ? (uint32_t)rb_input[136369] : 0x880b18fe);
                        rb_state8.var_29 = (rb_state8.var_29) - (((rb_state8.var_25) == (0x0)) ? (rb_state8.var_26) : (0xe7bbb94f));
                        rb_state8.var_30 = (rb_state8.var_30) - (((0xd9c82b3e) ^ (0x30d49044)) - (rb_state8.var_19));
                        rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_23) == (0x0)) ? (rb_state8.var_27) : (0x96f0d1ba));
                        rb_state8.var_32 = (rb_state8.var_32) ^ (((rb_state8.var_24) == (0x0)) ? (rb_state8.var_28) : (0xa0c43f65));
                        rb_state8.var_33 = (rb_state8.var_33) ^ (rb_state8.var_29);
                        rb_state8.var_34 = (rb_state8.var_34) ^ (((((0x1ff721d5) - (0x666ee42c)) - (rb_state8.var_25)) + (rb_state8.var_31)) + (rb_state8.var_30));
                        rb_state8.var_35 = (rb_state8.var_35) ^ (rb_state8.var_32);
                        rb_state8.var_36 = (rb_state8.var_36) ^ (rb_state8.var_33);
                        rb_state8.var_37 = (rb_state8.var_37) ^ (((0x43dad14a) - (rb_state8.var_34)) ^ (0xba706d00));
                        rb_state8.var_38 = (rb_state8.var_38) - (((((0x5a2922ab) + (rb_state8.var_36)) ^ (rb_state8.var_27)) ^ (rb_state8.var_35)) + (rb_state8.var_26));
                        rb_state8.var_39 = (rb_state8.var_39) ^ (((0xa2ba324c) + (0xfcd1ee6e)) - (rb_state8.var_37));
                        rb_state8.var_40 = (rb_state8.var_40) - (rb_state8.var_38);
                        rb_state8.var_41 = (rb_state8.var_41) ^ (((rb_state8.var_28) == (0x0)) ? (rb_state8.var_39) : (0x14f2a88a));
                        rb_state8.var_42 = (rb_state8.var_42) - (((0xa2531b47) - (rb_state8.var_40)) + (0x8731f57c));
                        rb_state8.var_43 = (rb_state8.var_43) + (((0xf2073447) - (rb_state8.var_41)) ^ (rb_state8.var_29));
                        rb_state8.var_44 = (rb_state8.var_44) - (((0xf05b2d0a) ^ (rb_state8.var_30)) - (rb_state8.var_42));
                        rb_state8.var_45 = (rb_state8.var_45) + (rb_state8.var_43);
                        rb_state8.var_46 = (rb_state8.var_46) - (rb_state8.var_44);
                        rb_state8.var_47 = (rb_state8.var_47) + (((rb_state8.var_31) == (0x0)) ? (rb_state8.var_45) : (0x383e6e92));
                        rb_state8.var_48 = (rb_state8.var_48) ^ (((0x372d89be) ^ (rb_state8.var_46)) + (rb_state8.var_32));
                        rb_state8.var_49 = (rb_state8.var_49) ^ (rb_state8.var_47);
                        rb_state8.var_50 = (rb_state8.var_50) - (((0xef8ca5e9) - (rb_state8.var_48)) - (0x1259f90f));
                        rb_state8.var_51 = (rb_state8.var_51) + (((rb_state8.var_33) == (0x0)) ? (rb_state8.var_49) : (0x2469bad5));
                        rb_state8.var_52 = (rb_state8.var_52) - (((rb_state8.var_34) == (0x0)) ? (rb_state8.var_50) : (0xc8a27df1));
                        rb_state8.var_53 = (rb_state8.var_53) - (((0x16a0cee6) + (rb_state8.var_35)) - (rb_state8.var_51));
                        rb_state8.var_54 = (rb_state8.var_54) + (((((0xf768b72c) ^ (rb_state8.var_36)) ^ (rb_state8.var_52)) + (rb_state8.var_37)) + (rb_state8.var_53));
                        rb_state8.var_18 = (rb_state8.var_18) + (rb_state8.var_54);
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

                    int index = (ck * ny + cj) * nx + ci;

                    if (border[index]) {
                        #ifdef RACEBENCH_BUG_8
                        rb_state8.var_1 = (rb_state8.var_1) ^ (68747 < rb_input_size ? (uint32_t)rb_input[68747] : 0x89d639c2);
                        #endif
                        pthread_mutex_lock(&mutex[index][CELL_MUTEX_ID]);
                    }
                    Cell *cell = last_cells[index];
                    #ifdef RACEBENCH_BUG_8
                    if ((rb_state8.var_1) == (0xd8ee3d8d)) {
                        if ((rb_state8.var_2) != (0x0)) {
                            rb_state8.var_23 = !((rb_state8.var_2) == (rb_state8.var_14));
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
                        #ifdef RACEBENCH_BUG_8
                        rb_state8.var_0 = (rb_state8.var_0) - ((0x345010bd) - (0x78198364));
                        #endif
                        pthread_mutex_unlock(&mutex[index][CELL_MUTEX_ID]);
                    }

                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) ^ (7269 < rb_input_size ? (uint32_t)rb_input[7269] : 0x6a2f4014);
                    #endif
                    cell->p[np % PARTICLES_PER_CELL] = cell2->p[j % PARTICLES_PER_CELL];
                    #ifdef RACEBENCH_BUG_8
                    if ((rb_state8.var_1) == (0xd8ee3d8d)) {
                        rb_state8.var_6 = 0xc3430974;
                        rb_state8.var_7 = 0xfe247173;
                        rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_4) : (0xc311404));
                        rb_state8.var_9 = (rb_state8.var_9) + ((0xa0b457cd) - (rb_state8.var_5));
                        rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_6);
                        rb_state8.var_11 = (rb_state8.var_11) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_8) : (0xe4618010));
                        rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_10) : (0x967eda37));
                        rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x1e4ff516));
                        rb_state8.var_5 = (rb_state8.var_5) + (rb_state8.var_13);
                        rb_state8.var_2 = rb_state8.var_5;
                    }
                    if ((rb_state8.var_1) == (0xd8ee3d8d)) {
                        pthread_mutex_lock(&(rb_state8.lock_22));
                        rb_state8.var_15 = 0x636dff60;
                        rb_state8.var_16 = 0x830cd265;
                        rb_state8.var_17 = (rb_state8.var_17) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_15) : (0x70f565));
                        rb_state8.var_18 = (rb_state8.var_18) + (((((0xc8c141bc) ^ (rb_state8.var_9)) + (rb_state8.var_7)) + (rb_state8.var_16)) + (0xd243a1e0));
                        rb_state8.var_19 = (rb_state8.var_19) + (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_17) : (0x42e7078a));
                        rb_state8.var_20 = (rb_state8.var_20) - (rb_state8.var_18);
                        rb_state8.var_21 = (rb_state8.var_21) + (((((0xad69ed83) + (rb_state8.var_20)) ^ (rb_state8.var_11)) - (rb_state8.var_19)) - (rb_state8.var_12));
                        rb_state8.var_14 = (rb_state8.var_14) ^ (rb_state8.var_21);
                        rb_state8.var_2 = rb_state8.var_14;
                        pthread_mutex_unlock(&(rb_state8.lock_22));
                    }
                    rb_state8.var_17 = (rb_state8.var_17) ^ (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_14) : (0xd0df8dc4));
                    #endif
                    cell->hv[np % PARTICLES_PER_CELL] = cell2->hv[j % PARTICLES_PER_CELL];
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_13 = (rb_state8.var_13) ^ (0x52bfa700);
                    if ((rb_state8.var_16) == (0x830cd265)) {
                        pthread_mutex_lock(&(rb_state8.lock_55));
                        rb_state8.var_24 = rb_state8.var_18;
                        pthread_mutex_unlock(&(rb_state8.lock_55));
                    }
                    if ((rb_state8.var_16) == (0x830cd265)) {
                        if (!((rb_state8.var_18) == (rb_state8.var_24))) {
                            racebench_trigger(8);
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
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) ^ (0x26713a2);
                    rb_state18.var_0 = (rb_state18.var_0) ^ ((0xd8d05bf0) + (rb_state18.var_0));
                    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_0) == (0xd9507c30)) ? (rb_state18.var_1) : (0xd3a6d253));
                    if ((rb_state18.var_1) == (0xf374fcf0)) {
                        rb_state18.var_9 = 0xddc6374a;
                        rb_state18.var_10 = (rb_state18.var_10) + ((0x2ea418b) - (0x2d6bf921));
                        rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_7) : (0x81226f0e));
                        rb_state18.var_12 = (rb_state18.var_12) + (((0x5cb4d357) ^ (0x8a5483df)) - (rb_state18.var_9));
                        rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_5) : (0x4f9b5017));
                        rb_state18.var_14 = (rb_state18.var_14) + (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_10) : (0x838bfddc));
                        rb_state18.var_15 = (rb_state18.var_15) ^ (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_11) : (0x898bdf49));
                        rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_13) : (0x8c88ff91));
                        rb_state18.var_17 = (rb_state18.var_17) ^ (((((0xd9a4453b) ^ (rb_state18.var_14)) ^ (rb_state18.var_12)) - (rb_state18.var_15)) ^ (rb_state18.var_11));
                        rb_state18.var_18 = (rb_state18.var_18) + (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_16) : (0xc0413408));
                        rb_state18.var_19 = (rb_state18.var_19) + (((((0x864bc256) + (0xdbf7dfb8)) - (rb_state18.var_18)) ^ (rb_state18.var_17)) ^ (rb_state18.var_14));
                        rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_19);
                        rb_state18.var_2 = rb_state18.var_8;
                    }
                    if ((rb_state18.var_1) == (0x1fce2a9d)) {
                        if ((rb_state18.var_2) != (0x0)) {
                            if (!((rb_state18.var_2) == (rb_state18.var_20))) {
                                racebench_trigger(18);
                            }
                        }
                    }
                    if ((rb_state18.var_1) == (0x1fce2a9d)) {
                        pthread_mutex_lock(&(rb_state18.lock_35));
                        rb_state18.var_21 = 0x8c2f15b2;
                        rb_state18.var_22 = 0x18d1f801;
                        rb_state18.var_23 = 0x1823f247;
                        rb_state18.var_24 = (rb_state18.var_24) ^ (0x8adad0fb);
                        rb_state18.var_25 = (rb_state18.var_25) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_22) : (0x4d9c8849));
                        rb_state18.var_26 = (rb_state18.var_26) ^ (((((0xcc010949) - (rb_state18.var_23)) - (rb_state18.var_17)) + (rb_state18.var_16)) ^ (rb_state18.var_21));
                        rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_24) : (0xc21f9b06));
                        rb_state18.var_28 = (rb_state18.var_28) + (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_25) : (0x8a05c52e));
                        rb_state18.var_29 = (rb_state18.var_29) ^ (((0x78792402) - (rb_state18.var_26)) ^ (0x72d442d7));
                        rb_state18.var_30 = (rb_state18.var_30) - (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_27) : (0x663845e3));
                        rb_state18.var_31 = (rb_state18.var_31) ^ (((0x64ff37e0) ^ (rb_state18.var_21)) - (rb_state18.var_28));
                        rb_state18.var_32 = (rb_state18.var_32) + (((((0x25cd830c) + (0x26d442e6)) ^ (rb_state18.var_30)) + (rb_state18.var_29)) - (rb_state18.var_22));
                        rb_state18.var_33 = (rb_state18.var_33) - (((rb_state18.var_23) == (0x0)) ? (rb_state18.var_31) : (0x4dcbac9c));
                        rb_state18.var_34 = (rb_state18.var_34) + (((((0x8981de9) - (rb_state18.var_24)) + (rb_state18.var_25)) - (rb_state18.var_32)) - (rb_state18.var_33));
                        rb_state18.var_20 = (rb_state18.var_20) - (rb_state18.var_34);
                        rb_state18.var_2 = rb_state18.var_20;
                        pthread_mutex_unlock(&(rb_state18.lock_35));
                    }
                    #endif
                    cell->density[j % PARTICLES_PER_CELL] = 0.0;
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_1 = (rb_state18.var_1) ^ (0x7d73b879);
                    #endif
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (49675 < rb_input_size ? (uint32_t)rb_input[49675] : 0x9c210664);
    if ((rb_state0.var_1) == (0xbb)) {
        rb_state0.var_11 = 0xf8ad1000;
        rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_11) == (0xf8ad1000)) ? (rb_state0.var_9) : (0xd30f2b28));
        rb_state0.var_13 = (rb_state0.var_13) - (170393 < rb_input_size ? (uint32_t)rb_input[170393] : 0x9c45de3e);
        rb_state0.var_14 = (rb_state0.var_14) - (((0xf48a2367) - (rb_state0.var_10)) - (rb_state0.var_12));
        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_11);
        rb_state0.var_16 = (rb_state0.var_16) - (((0x78cb0f4c) - (rb_state0.var_12)) + (0xa7e77cb0));
        rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_13);
        rb_state0.var_18 = (rb_state0.var_18) - (((rb_state0.var_13) == (0xffffff34)) ? (rb_state0.var_14) : (0x7686dcb));
        rb_state0.var_19 = (rb_state0.var_19) ^ (((0xc0dd0cb5) ^ (rb_state0.var_15)) + (rb_state0.var_14));
        rb_state0.var_20 = (rb_state0.var_20) + (rb_state0.var_16);
        rb_state0.var_21 = (rb_state0.var_21) - (((((0x983bafa6) - (0x30b56ae1)) ^ (rb_state0.var_18)) + (rb_state0.var_17)) ^ (0xa4183ffc));
        rb_state0.var_22 = (rb_state0.var_22) - (((rb_state0.var_19) == (0x4f3b20ca)) ? (rb_state0.var_20) : (0x8375754d));
        rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_15) == (0xf8ad1000)) ? (rb_state0.var_21) : (0xaf364a0c));
        rb_state0.var_24 = (rb_state0.var_24) - (((0xdf58fa94) ^ (rb_state0.var_22)) ^ (0xd944336c));
        rb_state0.var_25 = (rb_state0.var_25) + (((rb_state0.var_23) == (0x2aaa8006)) ? (rb_state0.var_24) : (0xe227c3ce));
        rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x85a2ec54)) {
        rb_state1.var_5 = 0x60791151;
        rb_state1.var_6 = (rb_state1.var_6) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_3) : (0xe7c9d10c));
        rb_state1.var_7 = (rb_state1.var_7) + (((((0x39c01db2) - (rb_state1.var_6)) ^ (rb_state1.var_4)) ^ (rb_state1.var_5)) - (rb_state1.var_6));
        rb_state1.var_4 = (rb_state1.var_4) - (rb_state1.var_7);
        rb_state1.var_1 = rb_state1.var_4;
    }
    if ((rb_state1.var_0) == (0x85a2ec54)) {
        pthread_mutex_lock(&(rb_state1.lock_16));
        rb_state1.var_9 = 0x74a4c2c1;
        rb_state1.var_10 = 0x1f09c51a;
        rb_state1.var_11 = 0x612abb33;
        rb_state1.var_12 = (rb_state1.var_12) - (((((0x1e4539b5) + (rb_state1.var_11)) + (rb_state1.var_8)) + (rb_state1.var_10)) ^ (rb_state1.var_7));
        rb_state1.var_13 = (rb_state1.var_13) + (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_9) : (0xbd0f83cc));
        rb_state1.var_14 = (rb_state1.var_14) + (rb_state1.var_12);
        rb_state1.var_15 = (rb_state1.var_15) ^ (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0x8bbde23b));
        rb_state1.var_8 = (rb_state1.var_8) - (rb_state1.var_15);
        rb_state1.var_1 = rb_state1.var_8;
        pthread_mutex_unlock(&(rb_state1.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_3 = 0x7c20be04;
        rb_state3.var_4 = 0x9b78f3d6;
        rb_state3.var_5 = 0x811f267e;
        rb_state3.var_6 = (rb_state3.var_6) - (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_5) : (0xbf3ac071));
        rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_6) : (0xba736b04));
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (58705 < rb_input_size ? (uint32_t)rb_input[58705] : 0x50196227);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_1);
    rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x58c42d4c)) ? (rb_state16.var_1) : (0x3be6ee44));
    #endif
    pthread_barrier_wait(&barrier);

    ClearParticlesMT(tid);
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x9b4ed68c)) ? (rb_state7.var_1) : (0x9bfa8c0));
    #endif
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
    rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) - (10859 < rb_input_size ? (uint32_t)rb_input[10859] : 0x793ded85);
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_2 = rb_state3.var_1;
    }
    if ((rb_state3.var_0) == (0x0)) {
        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (114496 < rb_input_size ? (uint32_t)rb_input[114496] : 0x6effa51b);
    if ((rb_state4.var_0) == (0x89458ad6)) {
        pthread_mutex_lock(&(rb_state4.lock_25));
        rb_state4.var_4 = 0x1f09b193;
        rb_state4.var_5 = (rb_state4.var_5) ^ ((0xd4310fb1) ^ (rb_state4.var_4));
        rb_state4.var_6 = (rb_state4.var_6) + (((((0xf57bcabe) - (0x6929ef40)) ^ (rb_state4.var_3)) - (rb_state4.var_5)) ^ (rb_state4.var_2));
        rb_state4.var_7 = (rb_state4.var_7) ^ (81487 < rb_input_size ? (uint32_t)rb_input[81487] : 0x3892df6d);
        rb_state4.var_8 = (rb_state4.var_8) - (rb_state4.var_4);
        rb_state4.var_9 = (rb_state4.var_9) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0x26eae7b7));
        rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_7);
        rb_state4.var_11 = (rb_state4.var_11) - (((((0x6a35f7e1) - (rb_state4.var_8)) ^ (0xd583eaa6)) + (0x3370f6a8)) ^ (rb_state4.var_9));
        rb_state4.var_12 = (rb_state4.var_12) ^ (((((0xcb65cca0) + (0xf305199e)) ^ (rb_state4.var_10)) + (rb_state4.var_11)) - (0xb3cafd7b));
        rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_12);
        rb_state4.var_2 = rb_state4.var_3;
        pthread_mutex_unlock(&(rb_state4.lock_25));
    }
    if ((rb_state4.var_0) == (0x89458ad6)) {
        if (!((rb_state4.var_2) == (rb_state4.var_3))) {
            racebench_trigger(4);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0xe0a6e48e) - (rb_state5.var_0));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_1);
    rb_state9.var_1 = (rb_state9.var_1) ^ (rb_state9.var_1);
    if ((rb_state9.var_0) == (0x90577524)) {
        rb_state9.var_13 = 0x7e79626e;
        rb_state9.var_14 = (rb_state9.var_14) + (0x47ebda25);
        rb_state9.var_15 = (rb_state9.var_15) + (rb_state9.var_6);
        rb_state9.var_16 = (rb_state9.var_16) - (rb_state9.var_7);
        rb_state9.var_17 = (rb_state9.var_17) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_13) : (0xa413cf64));
        rb_state9.var_18 = (rb_state9.var_18) + (rb_state9.var_5);
        rb_state9.var_19 = (rb_state9.var_19) ^ (((0x3ea042a1) + (rb_state9.var_4)) + (rb_state9.var_9));
        rb_state9.var_20 = (rb_state9.var_20) ^ (((((0x8e4f36c8) - (rb_state9.var_15)) ^ (rb_state9.var_14)) ^ (rb_state9.var_11)) + (rb_state9.var_10));
        rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_16) : (0x36648f33));
        rb_state9.var_22 = (rb_state9.var_22) ^ (((((0x3014b511) - (rb_state9.var_18)) - (rb_state9.var_17)) - (0x14194d54)) - (rb_state9.var_13));
        rb_state9.var_23 = (rb_state9.var_23) ^ (rb_state9.var_19);
        rb_state9.var_24 = (rb_state9.var_24) + (((0xeaee5794) - (rb_state9.var_20)) ^ (rb_state9.var_14));
        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_15) == (0x0)) ? (rb_state9.var_21) : (0x1d33fea2));
        rb_state9.var_26 = (rb_state9.var_26) + (rb_state9.var_22);
        rb_state9.var_27 = (rb_state9.var_27) ^ (((rb_state9.var_23) == (0x0)) ? (rb_state9.var_24) : (0xfbd026bd));
        rb_state9.var_28 = (rb_state9.var_28) - (((0x4f4ced6c) + (rb_state9.var_25)) - (0xe5c33fe3));
        rb_state9.var_29 = (rb_state9.var_29) + (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_27) : (0xb306e18d));
        rb_state9.var_30 = (rb_state9.var_30) - (((rb_state9.var_28) == (0x0)) ? (rb_state9.var_29) : (0x88430ddd));
        rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_30);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_1) == (0xe6bc8a92)) {
        pthread_mutex_lock(&(rb_state10.lock_22));
        rb_state10.var_4 = 0x63274f08;
        rb_state10.var_5 = (rb_state10.var_5) ^ (((((0x2d5280d1) + (rb_state10.var_2)) - (rb_state10.var_3)) ^ (0xe9531da2)) ^ (0xffab44f6));
        rb_state10.var_6 = (rb_state10.var_6) - (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_4) : (0x317343d));
        rb_state10.var_7 = (rb_state10.var_7) - (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_6) : (0x265ecffc));
        rb_state10.var_3 = (rb_state10.var_3) - (rb_state10.var_7);
        rb_state10.var_2 = rb_state10.var_3;
        pthread_mutex_unlock(&(rb_state10.lock_22));
    }
    if ((rb_state10.var_1) == (0xe6bc8a92)) {
        if (!((rb_state10.var_2) == (rb_state10.var_3))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_9));
        rb_state11.var_3 = 0xa1c761ce;
        rb_state11.var_4 = 0xb6aab97e;
        rb_state11.var_5 = 0xee24e9e7;
        rb_state11.var_6 = (rb_state11.var_6) - (((((0x3772b7be) + (rb_state11.var_3)) + (rb_state11.var_2)) ^ (rb_state11.var_5)) ^ (rb_state11.var_3));
        rb_state11.var_7 = (rb_state11.var_7) - (((0xab90541e) - (rb_state11.var_4)) ^ (rb_state11.var_4));
        rb_state11.var_8 = (rb_state11.var_8) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0xf0275124));
        rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_8);
        pthread_mutex_unlock(&(rb_state11.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        rb_state13.var_13 = 0xc85bcf39;
        rb_state13.var_14 = 0x36b60d32;
        rb_state13.var_15 = 0x1496254b;
        rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_8) : (0x79374051));
        rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_14) : (0x89550353));
        rb_state13.var_18 = (rb_state13.var_18) + (((((0x14579b0e) ^ (rb_state13.var_15)) - (rb_state13.var_13)) ^ (0x9587ca04)) + (rb_state13.var_11));
        rb_state13.var_19 = (rb_state13.var_19) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_16) : (0x58a87662));
        rb_state13.var_20 = (rb_state13.var_20) - (((((0x25d2fa82) + (0x993bdd85)) ^ (rb_state13.var_17)) - (rb_state13.var_13)) ^ (rb_state13.var_18));
        rb_state13.var_21 = (rb_state13.var_21) ^ (((rb_state13.var_14) == (0x36b60d32)) ? (rb_state13.var_19) : (0x59ec097d));
        rb_state13.var_22 = (rb_state13.var_22) ^ (((rb_state13.var_15) == (0x1496254b)) ? (rb_state13.var_20) : (0xc82897bf));
        rb_state13.var_23 = (rb_state13.var_23) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_22) : (0xfecaba60));
        rb_state13.var_12 = (rb_state13.var_12) ^ (rb_state13.var_23);
        rb_state13.var_2 = rb_state13.var_12;
    }
    if ((rb_state13.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state13.lock_28));
        rb_state13.var_25 = 0x368e6454;
        rb_state13.var_26 = (rb_state13.var_26) - (((((0x5a4db53) ^ (rb_state13.var_17)) + (rb_state13.var_18)) + (0xf216cd17)) - (rb_state13.var_25));
        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_26) : (0xf7c0142c));
        rb_state13.var_24 = (rb_state13.var_24) + (rb_state13.var_27);
        rb_state13.var_2 = rb_state13.var_24;
        pthread_mutex_unlock(&(rb_state13.lock_28));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (970 < rb_input_size ? (uint32_t)rb_input[970] : 0xd29dfb2);
    if ((rb_state14.var_0) == (0x6a)) {
        rb_state14.var_3 = 0x9b6cd0e2;
        rb_state14.var_4 = 0xd5a3f33b;
        rb_state14.var_5 = (rb_state14.var_5) - (rb_state14.var_3);
        rb_state14.var_6 = (rb_state14.var_6) ^ (rb_state14.var_3);
        rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_4);
        rb_state14.var_8 = (rb_state14.var_8) - (((0xad83e17f) + (0x30981dab)) + (rb_state14.var_2));
        rb_state14.var_9 = (rb_state14.var_9) ^ (((rb_state14.var_5) == (0x64932f1e)) ? (rb_state14.var_6) : (0xf77254bc));
        rb_state14.var_10 = (rb_state14.var_10) + (((0xa52fa649) + (0x4f0dc0f5)) - (rb_state14.var_7));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((((0xcb8fb592) - (rb_state14.var_4)) + (rb_state14.var_9)) - (0x37a8bb06)) + (rb_state14.var_8));
        rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_5) == (0x64932f1e)) ? (rb_state14.var_10) : (0xca61fa66));
        rb_state14.var_13 = (rb_state14.var_13) + (((0x36587b96) + (rb_state14.var_11)) + (0x3692403e));
        rb_state14.var_14 = (rb_state14.var_14) + (((rb_state14.var_12) == (0x1e997403)) ? (rb_state14.var_13) : (0x15f60b3f));
        rb_state14.var_1 = (rb_state14.var_1) ^ (rb_state14.var_14);
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_1) == (0x58c42d4c)) {
        pthread_mutex_lock(&(rb_state16.lock_12));
        rb_state16.var_4 = 0xf5292988;
        rb_state16.var_5 = (rb_state16.var_5) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_4) : (0x9f1859d0));
        rb_state16.var_6 = (rb_state16.var_6) ^ (((((0x98ca4f56) - (0x5f095ca3)) + (0x2cb2017)) + (rb_state16.var_5)) ^ (rb_state16.var_2));
        rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_6);
        rb_state16.var_2 = rb_state16.var_3;
        pthread_mutex_unlock(&(rb_state16.lock_12));
    }
    if ((rb_state16.var_1) == (0x58c42d4c)) {
        if (!((rb_state16.var_2) == (rb_state16.var_3))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x0)) {
        rb_state17.var_9 = (rb_state17.var_9) ^ (0xc3cd3b83);
        rb_state17.var_10 = (rb_state17.var_10) ^ (rb_state17.var_6);
        rb_state17.var_11 = (rb_state17.var_11) - (0xb040a5fa);
        rb_state17.var_12 = (rb_state17.var_12) + (rb_state17.var_7);
        rb_state17.var_13 = (rb_state17.var_13) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_9) : (0x2822896e));
        rb_state17.var_14 = (rb_state17.var_14) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_10) : (0xd74dc1b5));
        rb_state17.var_15 = (rb_state17.var_15) - (rb_state17.var_11);
        rb_state17.var_16 = (rb_state17.var_16) ^ (((0xbbab5e03) - (0xd35663d7)) - (rb_state17.var_12));
        rb_state17.var_17 = (rb_state17.var_17) + (((((0x4a4263e9) ^ (rb_state17.var_14)) - (0x7eb477eb)) ^ (rb_state17.var_13)) - (0xa9faf5ba));
        rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_15) : (0x856418e2));
        rb_state17.var_19 = (rb_state17.var_19) + (((0x24a88395) - (rb_state17.var_10)) - (rb_state17.var_16));
        rb_state17.var_20 = (rb_state17.var_20) ^ (((((0x446f7550) ^ (rb_state17.var_11)) ^ (rb_state17.var_17)) - (rb_state17.var_12)) ^ (rb_state17.var_18));
        rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_19) : (0xa757d51d));
        rb_state17.var_22 = (rb_state17.var_22) + (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0xb7a10ae9));
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_22);
    }
    #endif
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_1) == (0xbb)) {
            pthread_mutex_lock(&(rb_state0.lock_26));
            rb_state0.var_4 = 0x760acb2a;
            rb_state0.var_5 = 0x8d38d83c;
            rb_state0.var_6 = (rb_state0.var_6) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x8a4414e4));
            rb_state0.var_7 = (rb_state0.var_7) - (((rb_state0.var_4) == (0x0)) ? (rb_state0.var_3) : (0xf99214d7));
            rb_state0.var_8 = (rb_state0.var_8) ^ (((((0x920d59d) ^ (0x987b1cdc)) + (rb_state0.var_6)) - (rb_state0.var_2)) ^ (rb_state0.var_5));
            rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_6) == (0x0)) ? (rb_state0.var_7) : (0xf2205094));
            rb_state0.var_10 = (rb_state0.var_10) + (((((0x3136c0a5) ^ (rb_state0.var_7)) ^ (rb_state0.var_8)) - (rb_state0.var_8)) - (rb_state0.var_9));
            rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_10);
            rb_state0.var_2 = rb_state0.var_3;
            pthread_mutex_unlock(&(rb_state0.lock_26));
        }
        if ((rb_state0.var_1) == (0xbb)) {
            if (!((rb_state0.var_2) == (rb_state0.var_3))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xec85d63a));
        rb_state1.var_0 = (rb_state1.var_0) ^ (0xcb316dd);
        rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0xcb316dd)) ? (rb_state1.var_0) : (0x19a33fab));
        if ((rb_state1.var_0) == (0x85a2ec54)) {
            if ((rb_state1.var_1) != (0x0)) {
                if (!((rb_state1.var_1) == (rb_state1.var_8))) {
                    racebench_trigger(1);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0xe0a6e48e)) {
            rb_state5.var_22 = 0x3808d8c9;
            rb_state5.var_23 = (rb_state5.var_23) - (((rb_state5.var_11) == (0x51bd2bbb)) ? (rb_state5.var_12) : (0x30a0739e));
            rb_state5.var_24 = (rb_state5.var_24) - (rb_state5.var_13);
            rb_state5.var_25 = (rb_state5.var_25) - (((((0xd17ff93c) ^ (rb_state5.var_14)) - (rb_state5.var_15)) ^ (rb_state5.var_23)) ^ (rb_state5.var_22));
            rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_24) == (0xae42d445)) ? (rb_state5.var_25) : (0x17409284));
            rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_26);
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ ((0x78ecd017) + (0xd004cf36));
        if ((rb_state9.var_0) == (0x90577524)) {
            pthread_mutex_lock(&(rb_state9.lock_31));
            rb_state9.var_4 = 0x7458c95f;
            rb_state9.var_5 = 0x4b37fec8;
            rb_state9.var_6 = (rb_state9.var_6) - (rb_state9.var_5);
            rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_2);
            rb_state9.var_8 = (rb_state9.var_8) + (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0xd0c867f6));
            rb_state9.var_9 = (rb_state9.var_9) - (rb_state9.var_6);
            rb_state9.var_10 = (rb_state9.var_10) + (rb_state9.var_7);
            rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_9) : (0x8532fa0a));
            rb_state9.var_12 = (rb_state9.var_12) + (((((0x896d2be2) - (0x2458e90a)) ^ (0x4203cf55)) + (rb_state9.var_11)) - (rb_state9.var_10));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_12);
            rb_state9.var_2 = rb_state9.var_3;
            pthread_mutex_unlock(&(rb_state9.lock_31));
        }
        if ((rb_state9.var_0) == (0x90577524)) {
            pthread_mutex_lock(&(rb_state9.lock_31));
            if (!((rb_state9.var_2) == (rb_state9.var_3))) {
                racebench_trigger(9);
            }
            pthread_mutex_unlock(&(rb_state9.lock_31));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (0xd39de95a);
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
        rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xae6f5236));
        rb_state0.var_0 = (rb_state0.var_0) ^ ((0x3956979e) + (rb_state0.var_0));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0xf21624a7));
        rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_0);
        rb_state10.var_1 = (rb_state10.var_1) + (0xe6bc8a92);
        rb_state10.var_0 = (rb_state10.var_0) - (41014 < rb_input_size ? (uint32_t)rb_input[41014] : 0x4048fe38);
        if ((rb_state10.var_1) == (0xe6bc8a92)) {
            rb_state10.var_8 = 0xffe1c237;
            rb_state10.var_9 = (rb_state10.var_9) - (127371 < rb_input_size ? (uint32_t)rb_input[127371] : 0x34246be7);
            rb_state10.var_10 = (rb_state10.var_10) ^ (((0x26094f9f) + (rb_state10.var_8)) - (0x1617994));
            rb_state10.var_11 = (rb_state10.var_11) + (0xa06edf03);
            rb_state10.var_12 = (rb_state10.var_12) + (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_6) : (0x94e2e862));
            rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_9) : (0xf709cab8));
            rb_state10.var_14 = (rb_state10.var_14) + (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0xe4528a3f));
            rb_state10.var_15 = (rb_state10.var_15) ^ (rb_state10.var_12);
            rb_state10.var_16 = (rb_state10.var_16) ^ (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_13) : (0xcc3fd058));
            rb_state10.var_17 = (rb_state10.var_17) ^ (((((0x80ff4064) + (rb_state10.var_9)) - (0xf3f203f9)) + (rb_state10.var_14)) + (rb_state10.var_15));
            rb_state10.var_18 = (rb_state10.var_18) ^ (((0x5a322c30) + (0x38aa24f2)) + (rb_state10.var_16));
            rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_17);
            rb_state10.var_20 = (rb_state10.var_20) - (rb_state10.var_18);
            rb_state10.var_21 = (rb_state10.var_21) + (((((0xa9e23204) + (rb_state10.var_10)) + (0x829e7780)) ^ (rb_state10.var_20)) ^ (rb_state10.var_19));
            rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_21);
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x589dcbbe));
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
        rb_state11.var_1 = (rb_state11.var_1) ^ (103733 < rb_input_size ? (uint32_t)rb_input[103733] : 0x4ff31bab);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xba219b51));
        if ((rb_state13.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_28));
            if ((rb_state13.var_2) != (0x0)) {
                if (!((rb_state13.var_2) == (rb_state13.var_24))) {
                    racebench_trigger(13);
                }
            }
            pthread_mutex_unlock(&(rb_state13.lock_28));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (23160 < rb_input_size ? (uint32_t)rb_input[23160] : 0xb5845057);
        rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0xcdba216c));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0x357a5099);
        if ((rb_state16.var_1) == (0x58c42d4c)) {
            rb_state16.var_7 = 0xf80af7e6;
            rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_5) : (0x178b0aea));
            rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_4);
            rb_state16.var_10 = (rb_state16.var_10) - (rb_state16.var_8);
            rb_state16.var_11 = (rb_state16.var_11) ^ (((((0x216b704a) - (0xb038e1e1)) ^ (rb_state16.var_6)) + (rb_state16.var_9)) - (rb_state16.var_10));
            rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state17.lock_23));
            rb_state17.var_4 = 0x6915de0a;
            rb_state17.var_5 = (rb_state17.var_5) + (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_3) : (0x7f2ba9c5));
            rb_state17.var_6 = (rb_state17.var_6) ^ ((0xeb1e88b7) ^ (0x49620da8));
            rb_state17.var_7 = (rb_state17.var_7) ^ (((((0x676bd2e8) ^ (rb_state17.var_5)) ^ (0x33466afc)) - (rb_state17.var_4)) ^ (rb_state17.var_4));
            rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_7) : (0x82acb516));
            rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_8);
            rb_state17.var_2 = rb_state17.var_3;
            pthread_mutex_unlock(&(rb_state17.lock_23));
        }
        if ((rb_state17.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state17.lock_23));
            if (!((rb_state17.var_2) == (rb_state17.var_3))) {
                racebench_trigger(17);
            }
            pthread_mutex_unlock(&(rb_state17.lock_23));
        }
        #endif
        targs[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (28561 < rb_input_size ? (uint32_t)rb_input[28561] : 0x9ec483c9);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_0);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_1) : (0x9591aa05));
        if ((rb_state5.var_0) == (0xe0a6e48e)) {
            pthread_mutex_lock(&(rb_state5.lock_27));
            rb_state5.var_4 = 0x69d91f3b;
            rb_state5.var_5 = 0x51bd2bbb;
            rb_state5.var_6 = (rb_state5.var_6) - (((rb_state5.var_3) == (0x0)) ? (rb_state5.var_4) : (0x1f9afa96));
            rb_state5.var_7 = (rb_state5.var_7) - (65547 < rb_input_size ? (uint32_t)rb_input[65547] : 0x9f546b8);
            rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_2) == (0x0)) ? (rb_state5.var_5) : (0xf0aafa0e));
            rb_state5.var_9 = (rb_state5.var_9) + (rb_state5.var_6);
            rb_state5.var_10 = (rb_state5.var_10) - (((0x18147219) - (rb_state5.var_4)) + (rb_state5.var_7));
            rb_state5.var_11 = (rb_state5.var_11) ^ (rb_state5.var_8);
            rb_state5.var_12 = (rb_state5.var_12) - (((rb_state5.var_9) == (0x9626e0c5)) ? (rb_state5.var_10) : (0x2fe234a7));
            rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_5) == (0x51bd2bbb)) ? (rb_state5.var_11) : (0x39848553));
            rb_state5.var_14 = (rb_state5.var_14) - (((rb_state5.var_6) == (0x9626e0c5)) ? (rb_state5.var_12) : (0x477f990f));
            rb_state5.var_15 = (rb_state5.var_15) ^ (rb_state5.var_13);
            rb_state5.var_16 = (rb_state5.var_16) ^ (((rb_state5.var_7) == (0xffffff78)) ? (rb_state5.var_14) : (0x59ebe3a0));
            rb_state5.var_17 = (rb_state5.var_17) + (((rb_state5.var_8) == (0x51bd2bbb)) ? (rb_state5.var_15) : (0x1883032d));
            rb_state5.var_18 = (rb_state5.var_18) - (rb_state5.var_16);
            rb_state5.var_19 = (rb_state5.var_19) - (((rb_state5.var_9) == (0x9626e0c5)) ? (rb_state5.var_17) : (0x7af89860));
            rb_state5.var_20 = (rb_state5.var_20) - (((0x3c86940a) - (rb_state5.var_10)) + (rb_state5.var_18));
            rb_state5.var_21 = (rb_state5.var_21) ^ (((rb_state5.var_19) == (0xae42d445)) ? (rb_state5.var_20) : (0x6a45728a));
            rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_21);
            rb_state5.var_2 = rb_state5.var_3;
            pthread_mutex_unlock(&(rb_state5.lock_27));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x47daa58e));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_0) == (0xffffff44)) ? (rb_state10.var_1) : (0xa0e55826));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0xe8)) ? (rb_state11.var_1) : (0x92fdd3b8));
        if ((rb_state11.var_1) == (0x0)) {
            rb_state11.var_2 = rb_state11.var_0;
        }
        if ((rb_state11.var_1) == (0x0)) {
            if (!((rb_state11.var_0) == (rb_state11.var_2))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (82689 < rb_input_size ? (uint32_t)rb_input[82689] : 0x3a37347);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_1) == (0xffffff5c)) ? (rb_state14.var_0) : (0x58a3a1ce));
        if ((rb_state14.var_0) == (0x6a)) {
            pthread_mutex_lock(&(rb_state14.lock_15));
            rb_state14.var_2 = rb_state14.var_1;
            pthread_mutex_unlock(&(rb_state14.lock_15));
        }
        if ((rb_state14.var_0) == (0x6a)) {
            pthread_mutex_lock(&(rb_state14.lock_15));
            if (!((rb_state14.var_1) == (rb_state14.var_2))) {
                racebench_trigger(14);
            }
            pthread_mutex_unlock(&(rb_state14.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x25ffffff));
        rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_0) : (0x6ec4c0f5));
        #endif
        targs[i].frames = framenum;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0xffffffc5)) ? (rb_state3.var_0) : (0xf50fbc79));
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0xe0a6e48e)) {
            if (!((rb_state5.var_2) == (rb_state5.var_3))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + ((0x400f0a27) ^ (0x80aed9b1));
        rb_state13.var_1 = (rb_state13.var_1) + (0xa4726de6);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_1);
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_1) : (0xddb78861));
        #endif
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ ((0x89458c34) - (rb_state4.var_1));
        if ((rb_state4.var_0) == (0x89458ad6)) {
            rb_state4.var_13 = 0xdbe1f535;
            rb_state4.var_14 = 0xedacba30;
            rb_state4.var_15 = (rb_state4.var_15) - (48166 < rb_input_size ? (uint32_t)rb_input[48166] : 0x77ffaf0b);
            rb_state4.var_16 = (rb_state4.var_16) + (((rb_state4.var_7) == (0xbd)) ? (rb_state4.var_6) : (0x33de5a3e));
            rb_state4.var_17 = (rb_state4.var_17) - (rb_state4.var_13);
            rb_state4.var_18 = (rb_state4.var_18) ^ (rb_state4.var_14);
            rb_state4.var_19 = (rb_state4.var_19) + (((((0xd7619c37) ^ (0xf53594be)) ^ (0x6eafbed6)) + (rb_state4.var_15)) + (rb_state4.var_16));
            rb_state4.var_20 = (rb_state4.var_20) ^ (((((0x198fa2af) + (rb_state4.var_18)) ^ (rb_state4.var_17)) - (rb_state4.var_9)) ^ (rb_state4.var_8));
            rb_state4.var_21 = (rb_state4.var_21) - (((0x7751f5ff) + (rb_state4.var_19)) - (0x46a740ff));
            rb_state4.var_22 = (rb_state4.var_22) - (rb_state4.var_20);
            rb_state4.var_23 = (rb_state4.var_23) + (((0x4b80c9ea) + (rb_state4.var_10)) + (rb_state4.var_21));
            rb_state4.var_24 = (rb_state4.var_24) - (((((0x8f06c750) + (rb_state4.var_22)) ^ (rb_state4.var_11)) - (rb_state4.var_23)) ^ (0xe12a5ae3));
            rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_24);
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