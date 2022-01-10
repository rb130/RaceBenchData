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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) + (0x8f70a72f);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_4 = (rb_state1.var_4) - (106141 < rb_input_size ? (uint32_t)rb_input[106141] : 0x716bd750);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) + (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_3) : (0xf6da102c));
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_11));
        rb_state5.var_6 = 0xb33f2b35;
        rb_state5.var_7 = 0x98ebf4c;
        rb_state5.var_8 = 0x6969886e;
        rb_state5.var_9 = (rb_state5.var_9) + (((((0x5fd6d9a0) ^ (0x5a833232)) ^ (rb_state5.var_6)) - (rb_state5.var_8)) - (0x4b32505b));
        rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_7) == (0x98ebf4c)) ? (rb_state5.var_9) : (0x7312848e));
        rb_state5.var_4 = (rb_state5.var_4) ^ (rb_state5.var_10);
        rb_state5.var_5 = rb_state5.var_4;
        pthread_mutex_unlock(&(rb_state5.lock_11));
    }
    rb_state5.var_9 = (rb_state5.var_9) ^ (0x1bcdb6e);
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (79363 < rb_input_size ? (uint32_t)rb_input[79363] : 0xf1b9f2e5);
    rb_state6.var_1 = (rb_state6.var_1) ^ (0x71e94f6d);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_4 = (rb_state8.var_4) - (rb_state8.var_4);
    if ((rb_state8.var_22) == (0x49d37747)) {
        rb_state8.var_44 = rb_state8.var_28;
    }
    if ((rb_state8.var_22) == (0x49d37747)) {
        pthread_mutex_lock(&(rb_state8.lock_70));
        if (!((rb_state8.var_28) == (rb_state8.var_44))) {
            racebench_trigger(8);
        }
        pthread_mutex_unlock(&(rb_state8.lock_70));
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_3 = (rb_state9.var_3) + ((0x30efec47) - (rb_state9.var_3));
    rb_state9.var_4 = (rb_state9.var_4) - (107864 < rb_input_size ? (uint32_t)rb_input[107864] : 0xc9f154d2);
    rb_state9.var_4 = (rb_state9.var_4) - (0x2dda760b);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_2);
    if ((rb_state10.var_4) == (0xbf165a46)) {
        rb_state10.var_17 = 0x6aa3107f;
        rb_state10.var_18 = 0xe9a61e04;
        rb_state10.var_19 = (rb_state10.var_19) ^ (((((0x1437c73e) ^ (rb_state10.var_18)) + (0xafa1d70d)) ^ (rb_state10.var_14)) ^ (rb_state10.var_17));
        rb_state10.var_20 = (rb_state10.var_20) - (((((0xf074abc2) + (0x1a423bab)) - (rb_state10.var_13)) + (rb_state10.var_19)) ^ (0xcac8aa2c));
        rb_state10.var_16 = (rb_state10.var_16) ^ (rb_state10.var_20);
        rb_state10.var_5 = rb_state10.var_16;
    }
    if ((rb_state10.var_4) == (0xbf165a46)) {
        pthread_mutex_lock(&(rb_state10.lock_27));
        rb_state10.var_22 = 0xf7e2bb52;
        rb_state10.var_23 = 0x82c321a9;
        rb_state10.var_24 = 0x96d70cd1;
        rb_state10.var_25 = (rb_state10.var_25) + (((rb_state10.var_24) == (0x0)) ? (rb_state10.var_23) : (0xf25c53bd));
        rb_state10.var_26 = (rb_state10.var_26) + (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_25) : (0x2cdfccaf));
        rb_state10.var_21 = (rb_state10.var_21) - (rb_state10.var_26);
        rb_state10.var_5 = rb_state10.var_21;
        pthread_mutex_unlock(&(rb_state10.lock_27));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_2 = (rb_state12.var_2) - (127124 < rb_input_size ? (uint32_t)rb_input[127124] : 0x7ac8d516);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (0x2140d113);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_2);
    rb_state16.var_3 = (rb_state16.var_3) - (24730 < rb_input_size ? (uint32_t)rb_input[24730] : 0x8ae317e8);
    rb_state16.var_3 = (rb_state16.var_3) ^ (18577 < rb_input_size ? (uint32_t)rb_input[18577] : 0xfa8b5880);
    rb_state16.var_1 = (rb_state16.var_1) - (((rb_state16.var_3) == (0xffffff8e)) ? (rb_state16.var_2) : (0xe1a67e6c));
    if ((rb_state16.var_3) == (0xffffff18)) {
        rb_state16.var_17 = 0x565b0361;
        rb_state16.var_18 = 0x66abb9a2;
        rb_state16.var_19 = (rb_state16.var_19) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_18) : (0x68eea805));
        rb_state16.var_20 = (rb_state16.var_20) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_17) : (0x6b158af2));
        rb_state16.var_21 = (rb_state16.var_21) ^ (((0x11961d89) + (rb_state16.var_7)) + (rb_state16.var_10));
        rb_state16.var_22 = (rb_state16.var_22) + (rb_state16.var_19);
        rb_state16.var_23 = (rb_state16.var_23) - (((((0xb3252679) - (rb_state16.var_20)) + (rb_state16.var_12)) + (rb_state16.var_21)) + (rb_state16.var_11));
        rb_state16.var_24 = (rb_state16.var_24) ^ (((0xee7c96dc) - (rb_state16.var_22)) + (rb_state16.var_13));
        rb_state16.var_25 = (rb_state16.var_25) ^ (((0xfef477ac) ^ (rb_state16.var_23)) ^ (rb_state16.var_14));
        rb_state16.var_26 = (rb_state16.var_26) - (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_24) : (0xbe29ad8d));
        rb_state16.var_27 = (rb_state16.var_27) ^ (rb_state16.var_25);
        rb_state16.var_28 = (rb_state16.var_28) ^ (((((0x8f335ce1) + (rb_state16.var_16)) - (rb_state16.var_26)) + (rb_state16.var_27)) - (0xdd389457));
        rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_4) == (0x25d)) {
        rb_state17.var_13 = 0x8863f1fa;
        rb_state17.var_14 = 0x8cea163b;
        rb_state17.var_15 = (rb_state17.var_15) - (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0x19fa4f95));
        rb_state17.var_16 = (rb_state17.var_16) ^ (0x260b0337);
        rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_15) : (0x397b08dc));
        rb_state17.var_18 = (rb_state17.var_18) - (((0x27c75a) ^ (rb_state17.var_11)) ^ (rb_state17.var_16));
        rb_state17.var_19 = (rb_state17.var_19) ^ (rb_state17.var_17);
        rb_state17.var_20 = (rb_state17.var_20) ^ (rb_state17.var_18);
        rb_state17.var_21 = (rb_state17.var_21) + (rb_state17.var_19);
        rb_state17.var_22 = (rb_state17.var_22) ^ (((rb_state17.var_12) == (0x0)) ? (rb_state17.var_20) : (0xeda58b73));
        rb_state17.var_23 = (rb_state17.var_23) - (((0x1c8b6cd8) - (rb_state17.var_13)) + (rb_state17.var_21));
        rb_state17.var_24 = (rb_state17.var_24) ^ (((0x40ab25cb) ^ (rb_state17.var_14)) + (rb_state17.var_22));
        rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_23) : (0x5257b1b9));
        rb_state17.var_26 = (rb_state17.var_26) - (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_25) : (0x310abc61));
        rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_26);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) ^ (0xaa83c436);
    rb_state18.var_3 = (rb_state18.var_3) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_4) : (0xafea1e09));
    rb_state18.var_1 = (rb_state18.var_1) + (((rb_state18.var_3) == (0xaa83c436)) ? (rb_state18.var_2) : (0xe21a91c7));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - ((0x54c450dd) ^ (0x2f4a4053));
    #endif
    pthread_barrier_wait(&barrier);

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_8 = (rb_state0.var_8) ^ ((0xfe58f5d4) ^ (0x9c583437));
    rb_state0.var_6 = (rb_state0.var_6) - (((rb_state0.var_5) == (0x7c5d0777)) ? (rb_state0.var_10) : (0xf5d3d219));
    rb_state0.var_7 = (rb_state0.var_7) ^ ((0x617d2083) + (0xb1b637aa));
    rb_state0.var_8 = (rb_state0.var_8) ^ (35522 < rb_input_size ? (uint32_t)rb_input[35522] : 0x72ffd2bc);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (0x1b604488);
    rb_state14.var_2 = (rb_state14.var_2) + (0xb8e33698);
    #endif
    ClearParticlesMT(tid);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_6 = (rb_state0.var_6) + ((0x9f34cdd0) - (rb_state0.var_10));
    if ((rb_state0.var_4) == (0x6a9ba0e1)) {
        pthread_mutex_lock(&(rb_state0.lock_16));
        rb_state0.var_11 = rb_state0.var_7;
        pthread_mutex_unlock(&(rb_state0.lock_16));
    }
    if ((rb_state0.var_4) == (0x6a9ba0e1)) {
        rb_state0.var_12 = 0xbed91dbc;
        rb_state0.var_13 = 0x62c9f225;
        rb_state0.var_14 = (rb_state0.var_14) + (((((0x7fd3d922) + (rb_state0.var_12)) + (rb_state0.var_12)) - (rb_state0.var_11)) ^ (0x8f84eea));
        rb_state0.var_15 = (rb_state0.var_15) + (((((0x4a3d5ad6) - (rb_state0.var_14)) + (rb_state0.var_13)) ^ (0x1cdf59ee)) + (rb_state0.var_13));
        rb_state0.var_7 = (rb_state0.var_7) - (rb_state0.var_15);
    }
    if ((rb_state0.var_4) == (0x6a9ba0e1)) {
        if (!((rb_state0.var_7) == (rb_state0.var_11))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_3) == (0xe8c5b785)) {
        rb_state14.var_4 = rb_state14.var_0;
    }
    if ((rb_state14.var_3) == (0xe8c5b785)) {
        rb_state14.var_5 = 0x9fb73833;
        rb_state14.var_6 = 0x29761199;
        rb_state14.var_7 = (rb_state14.var_7) ^ (0x24dff063);
        rb_state14.var_8 = (rb_state14.var_8) ^ (rb_state14.var_4);
        rb_state14.var_9 = (rb_state14.var_9) + (rb_state14.var_6);
        rb_state14.var_10 = (rb_state14.var_10) + (((0xf197cc6c) ^ (0xa9184a1f)) - (rb_state14.var_5));
        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0xe849a88f));
        rb_state14.var_12 = (rb_state14.var_12) ^ (rb_state14.var_9);
        rb_state14.var_13 = (rb_state14.var_13) - (((0x8e0325ab) ^ (rb_state14.var_5)) + (rb_state14.var_10));
        rb_state14.var_14 = (rb_state14.var_14) - (((0xf7b25f05) ^ (rb_state14.var_11)) ^ (0x93508b6c));
        rb_state14.var_15 = (rb_state14.var_15) - (((((0x88e07a12) - (rb_state14.var_13)) - (rb_state14.var_12)) ^ (rb_state14.var_6)) ^ (0x1d7910));
        rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_14) : (0xc83a12a1));
        rb_state14.var_17 = (rb_state14.var_17) + (rb_state14.var_15);
        rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_17) : (0x62a029a));
        rb_state14.var_0 = (rb_state14.var_0) + (rb_state14.var_18);
    }
    if ((rb_state14.var_3) == (0xe8c5b785)) {
        pthread_mutex_lock(&(rb_state14.lock_19));
        if (!((rb_state14.var_0) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
        pthread_mutex_unlock(&(rb_state14.lock_19));
    }
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) - ((0x44ce238a) ^ (rb_state0.var_1));
    rb_state0.var_2 = (rb_state0.var_2) ^ (0x5e3fc007);
    if ((rb_state0.var_2) == (0x0)) {
        rb_state0.var_4 = rb_state0.var_3;
    }
    if ((rb_state0.var_2) == (0x0)) {
        rb_state0.var_10 = !((rb_state0.var_3) == (rb_state0.var_4));
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
    rb_state1.var_3 = (rb_state1.var_3) ^ ((0xa8a34b7d) - (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_20));
        rb_state2.var_7 = 0x2c19c743;
        rb_state2.var_8 = 0x12911ca6;
        rb_state2.var_9 = (rb_state2.var_9) + (rb_state2.var_8);
        rb_state2.var_10 = (rb_state2.var_10) + (rb_state2.var_5);
        rb_state2.var_11 = (rb_state2.var_11) - (((0x54445b7a) + (rb_state2.var_7)) - (0xc468022f));
        rb_state2.var_12 = (rb_state2.var_12) ^ (((((0x5e18135f) ^ (0xa68d58c)) ^ (rb_state2.var_9)) ^ (0x6a7321df)) + (rb_state2.var_10));
        rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_11) : (0x2ebe4c6e));
        rb_state2.var_14 = (rb_state2.var_14) ^ (((((0xdfa5ee7c) - (0x4580431a)) ^ (rb_state2.var_12)) + (rb_state2.var_7)) ^ (rb_state2.var_13));
        rb_state2.var_6 = (rb_state2.var_6) - (rb_state2.var_14);
        rb_state2.var_5 = rb_state2.var_6;
        pthread_mutex_unlock(&(rb_state2.lock_20));
    }
    if ((rb_state2.var_4) == (0x0)) {
        if (!((rb_state2.var_5) == (rb_state2.var_6))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) ^ (22325 < rb_input_size ? (uint32_t)rb_input[22325] : 0x72fd8b81);
    rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_2);
    rb_state3.var_4 = (rb_state3.var_4) - (154483 < rb_input_size ? (uint32_t)rb_input[154483] : 0xd31ef806);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) - ((0x22194945) + (0x33f32dc9));
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x45f06144);
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) - ((0x6eca5f6e) ^ (0x40fad47c));
    rb_state5.var_2 = (rb_state5.var_2) + ((0x977d9bb) ^ (0x9db77f69));
    rb_state5.var_9 = (rb_state5.var_9) + (rb_state5.var_4);
    rb_state5.var_10 = (rb_state5.var_10) ^ (rb_state5.var_5);
    rb_state5.var_9 = (rb_state5.var_9) ^ (0xca26901c);
    if ((rb_state5.var_6) == (0xb33f2b35)) {
        rb_state5.var_14 = 0x3203b67a;
        rb_state5.var_15 = 0x6e6219d1;
        rb_state5.var_16 = (rb_state5.var_16) ^ (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_8) : (0xa1bc24cd));
        rb_state5.var_17 = (rb_state5.var_17) ^ (((((0xb1a3055d) + (0x5ff2443e)) - (rb_state5.var_13)) - (rb_state5.var_15)) - (rb_state5.var_14));
        rb_state5.var_18 = (rb_state5.var_18) - (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_16) : (0xdb881943));
        rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_14) == (0x0)) ? (rb_state5.var_17) : (0x4d5cdf1f));
        rb_state5.var_20 = (rb_state5.var_20) - (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_18) : (0xfa2ee302));
        rb_state5.var_21 = (rb_state5.var_21) - (((0x192de22b) ^ (rb_state5.var_19)) - (rb_state5.var_16));
        rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_17) == (0x0)) ? (rb_state5.var_20) : (0x13ac2e30));
        rb_state5.var_23 = (rb_state5.var_23) + (rb_state5.var_21);
        rb_state5.var_24 = (rb_state5.var_24) + (((rb_state5.var_18) == (0x0)) ? (rb_state5.var_22) : (0xe3a98816));
        rb_state5.var_25 = (rb_state5.var_25) ^ (rb_state5.var_23);
        rb_state5.var_26 = (rb_state5.var_26) ^ (rb_state5.var_24);
        rb_state5.var_27 = (rb_state5.var_27) ^ (((0xa52e5367) + (rb_state5.var_19)) - (rb_state5.var_25));
        rb_state5.var_28 = (rb_state5.var_28) - (((((0x64a576ab) ^ (rb_state5.var_20)) ^ (0x352f1f89)) + (rb_state5.var_27)) ^ (rb_state5.var_26));
        rb_state5.var_10 = (rb_state5.var_10) + (rb_state5.var_28);
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0x0)) {
        rb_state6.var_13 = 0x2c06cc15;
        rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_13);
        rb_state6.var_15 = (rb_state6.var_15) - (rb_state6.var_9);
        rb_state6.var_16 = (rb_state6.var_16) - (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_8) : (0xb6e4897b));
        rb_state6.var_17 = (rb_state6.var_17) ^ (((rb_state6.var_14) == (0x0)) ? (rb_state6.var_15) : (0xbaebbf70));
        rb_state6.var_18 = (rb_state6.var_18) ^ (rb_state6.var_16);
        rb_state6.var_19 = (rb_state6.var_19) - (((((0xead96d5d) ^ (rb_state6.var_17)) - (rb_state6.var_18)) - (0xc82c8358)) ^ (0xaefa3aed));
        rb_state6.var_12 = (rb_state6.var_12) + (rb_state6.var_19);
        rb_state6.var_4 = rb_state6.var_12;
    }
    if ((rb_state6.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state6.lock_30));
        rb_state6.var_21 = 0xe78ef86b;
        rb_state6.var_22 = (rb_state6.var_22) + (rb_state6.var_21);
        rb_state6.var_23 = (rb_state6.var_23) ^ (101203 < rb_input_size ? (uint32_t)rb_input[101203] : 0x955720b6);
        rb_state6.var_24 = (rb_state6.var_24) - (((((0x75bac011) + (rb_state6.var_11)) ^ (rb_state6.var_13)) ^ (rb_state6.var_12)) - (rb_state6.var_14));
        rb_state6.var_25 = (rb_state6.var_25) - (((((0xa08650a) ^ (0x15218b8)) ^ (rb_state6.var_22)) ^ (rb_state6.var_23)) - (0x268a80a4));
        rb_state6.var_26 = (rb_state6.var_26) - (((0xa6ee6659) ^ (rb_state6.var_24)) ^ (0xb17c6afd));
        rb_state6.var_27 = (rb_state6.var_27) - (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_25) : (0xf189a022));
        rb_state6.var_28 = (rb_state6.var_28) + (((rb_state6.var_16) == (0x0)) ? (rb_state6.var_26) : (0x3aa7ea9));
        rb_state6.var_29 = (rb_state6.var_29) - (((((0x77aba415) ^ (rb_state6.var_17)) ^ (0x46377d5)) ^ (rb_state6.var_27)) ^ (rb_state6.var_28));
        rb_state6.var_20 = (rb_state6.var_20) ^ (rb_state6.var_29);
        rb_state6.var_4 = rb_state6.var_20;
        pthread_mutex_unlock(&(rb_state6.lock_30));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_1) == (0x33627305)) ? (rb_state7.var_0) : (0xcdac2389));
    rb_state7.var_3 = (rb_state7.var_3) - (rb_state7.var_3);
    rb_state7.var_4 = (rb_state7.var_4) - (8196 < rb_input_size ? (uint32_t)rb_input[8196] : 0x2e962647);
    rb_state7.var_4 = (rb_state7.var_4) + ((0x8236ef81) ^ (0x3fa6843c));
    if ((rb_state7.var_2) == (0x0)) {
        rb_state7.var_5 = rb_state7.var_3;
    }
    if ((rb_state7.var_2) == (0x0)) {
        if (!((rb_state7.var_3) == (rb_state7.var_5))) {
            racebench_trigger(7);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (((rb_state8.var_1) == (0xffffff68)) ? (rb_state8.var_0) : (0x455f11fe));
    if ((rb_state8.var_4) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_42));
        if ((rb_state8.var_5) != (0x0)) {
            rb_state8.var_43 = !((rb_state8.var_5) == (rb_state8.var_32));
        }
        pthread_mutex_unlock(&(rb_state8.lock_42));
    }
    rb_state8.var_27 = (rb_state8.var_27) - (76084 < rb_input_size ? (uint32_t)rb_input[76084] : 0x4ab8b54);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_4) == (0xd2258914)) {
        pthread_mutex_lock(&(rb_state9.lock_16));
        rb_state9.var_5 = rb_state9.var_2;
        pthread_mutex_unlock(&(rb_state9.lock_16));
    }
    if ((rb_state9.var_4) == (0xd2258914)) {
        pthread_mutex_lock(&(rb_state9.lock_16));
        if (!((rb_state9.var_2) == (rb_state9.var_5))) {
            racebench_trigger(9);
        }
        pthread_mutex_unlock(&(rb_state9.lock_16));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_1) : (0xf89445));
    rb_state10.var_2 = (rb_state10.var_2) - (rb_state10.var_0);
    rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_3);
    rb_state10.var_3 = (rb_state10.var_3) + (170236 < rb_input_size ? (uint32_t)rb_input[170236] : 0x62fa9b51);
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (0x3109ded9);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (63294 < rb_input_size ? (uint32_t)rb_input[63294] : 0xcd18a2a7);
    rb_state12.var_3 = (rb_state12.var_3) + (((rb_state12.var_3) == (0xb569bd55)) ? (rb_state12.var_3) : (0x80648aa4));
    if ((rb_state12.var_1) == (0xadc15568)) {
        rb_state12.var_4 = rb_state12.var_2;
    }
    if ((rb_state12.var_1) == (0xadc15568)) {
        if (!((rb_state12.var_2) == (rb_state12.var_4))) {
            racebench_trigger(12);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_3) == (0x0)) {
        rb_state13.var_4 = rb_state13.var_0;
    }
    if ((rb_state13.var_3) == (0x7)) {
        pthread_mutex_lock(&(rb_state13.lock_10));
        if (!((rb_state13.var_0) == (rb_state13.var_4))) {
            racebench_trigger(13);
        }
        pthread_mutex_unlock(&(rb_state13.lock_10));
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0xeefe8881));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_2 = (rb_state15.var_2) - (0xef1c9d58);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (78198 < rb_input_size ? (uint32_t)rb_input[78198] : 0x184f0967);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_2 = (rb_state18.var_2) ^ (131024 < rb_input_size ? (uint32_t)rb_input[131024] : 0xb03500c);
    #endif
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_2);
        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_0);
        rb_state0.var_0 = (rb_state0.var_0) + (0xdd97d246);
        #endif
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_1) == (0x0)) {
            rb_state1.var_6 = 0xb9aab5eb;
            rb_state1.var_7 = 0x29e45191;
            rb_state1.var_8 = 0x61e1eb86;
            rb_state1.var_9 = (rb_state1.var_9) + (0x56e7d1e4);
            rb_state1.var_10 = (rb_state1.var_10) + (((((0x462b2d04) + (rb_state1.var_7)) ^ (0x176144b)) - (rb_state1.var_6)) + (0x9d54c41c));
            rb_state1.var_11 = (rb_state1.var_11) ^ (((((0xcc2de7c) ^ (rb_state1.var_8)) + (0x2736ab27)) - (rb_state1.var_9)) + (0xdb70a3cb));
            rb_state1.var_12 = (rb_state1.var_12) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_10) : (0x34541c5b));
            rb_state1.var_13 = (rb_state1.var_13) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_11) : (0x545e3f53));
            rb_state1.var_14 = (rb_state1.var_14) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_13) : (0xc77666e1));
            rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_14);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_4 = (rb_state3.var_4) ^ ((0x1c45e285) + (rb_state3.var_4));
        rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_1) : (0x76cf5d15));
        if ((rb_state3.var_3) == (0x20edf517)) {
            if ((rb_state3.var_5) != (0x0)) {
                if (!((rb_state3.var_5) == (rb_state3.var_13))) {
                    racebench_trigger(3);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_4 = (rb_state4.var_4) - (146524 < rb_input_size ? (uint32_t)rb_input[146524] : 0x3c0dd77a);
        if ((rb_state4.var_4) == (0xfffffe20)) {
            pthread_mutex_lock(&(rb_state4.lock_24));
            if ((rb_state4.var_5) != (0x0)) {
                if (!((rb_state4.var_5) == (rb_state4.var_15))) {
                    racebench_trigger(4);
                }
            }
            pthread_mutex_unlock(&(rb_state4.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (150542 < rb_input_size ? (uint32_t)rb_input[150542] : 0x6a3425af);
        rb_state5.var_3 = (rb_state5.var_3) ^ (0xfae3145f);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (rb_state6.var_0);
        rb_state6.var_1 = (rb_state6.var_1) + (54913 < rb_input_size ? (uint32_t)rb_input[54913] : 0x34b75ec8);
        rb_state6.var_3 = (rb_state6.var_3) + ((0xd7bddf2f) - (rb_state6.var_2));
        rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_25 = (rb_state8.var_25) + (92033 < rb_input_size ? (uint32_t)rb_input[92033] : 0xc3901022);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) + ((0x63a1e523) ^ (rb_state10.var_0));
        rb_state10.var_4 = (rb_state10.var_4) ^ ((0x48e9a5ba) + (rb_state10.var_4));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_1) : (0x2aa403bf));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (0x846ab12f);
        rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_3);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0xc726c57d)) ? (rb_state15.var_1) : (0x7a4ea3c4));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_2);
        if ((rb_state16.var_3) == (0xffffff18)) {
            pthread_mutex_lock(&(rb_state16.lock_29));
            rb_state16.var_6 = 0xc2eb581f;
            rb_state16.var_7 = 0x3a020f78;
            rb_state16.var_8 = 0x33b8823e;
            rb_state16.var_9 = (rb_state16.var_9) ^ (((0x527d0311) - (rb_state16.var_8)) - (rb_state16.var_4));
            rb_state16.var_10 = (rb_state16.var_10) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_7) : (0x4769fd1a));
            rb_state16.var_11 = (rb_state16.var_11) - (((0x9f78f310) + (0x7c0469cb)) - (rb_state16.var_6));
            rb_state16.var_12 = (rb_state16.var_12) - (((0xf4b70572) - (rb_state16.var_9)) + (rb_state16.var_6));
            rb_state16.var_13 = (rb_state16.var_13) ^ (((0xa303ea13) - (0x36ce1360)) ^ (rb_state16.var_10));
            rb_state16.var_14 = (rb_state16.var_14) - (((0x4f73e1b) ^ (0xb82f573d)) - (rb_state16.var_11));
            rb_state16.var_15 = (rb_state16.var_15) ^ (((((0xb2ae4dd7) + (0xd41b4e44)) ^ (rb_state16.var_12)) + (rb_state16.var_13)) + (0x4ec6d9ea));
            rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_15) : (0xb86e07f4));
            rb_state16.var_5 = (rb_state16.var_5) ^ (rb_state16.var_16);
            rb_state16.var_4 = rb_state16.var_5;
            pthread_mutex_unlock(&(rb_state16.lock_29));
        }
        if ((rb_state16.var_3) == (0xffffff18)) {
            pthread_mutex_lock(&(rb_state16.lock_29));
            if (!((rb_state16.var_4) == (rb_state16.var_5))) {
                racebench_trigger(16);
            }
            pthread_mutex_unlock(&(rb_state16.lock_29));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_0);
        rb_state17.var_3 = (rb_state17.var_3) - (0x91b7522);
        rb_state17.var_4 = (rb_state17.var_4) + (128715 < rb_input_size ? (uint32_t)rb_input[128715] : 0x805cb181);
        if ((rb_state17.var_4) == (0x25d)) {
            pthread_mutex_lock(&(rb_state17.lock_27));
            rb_state17.var_7 = 0xb2994a1c;
            rb_state17.var_8 = (rb_state17.var_8) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_6) : (0x70161cc8));
            rb_state17.var_9 = (rb_state17.var_9) ^ (((0xb119928) + (rb_state17.var_7)) + (rb_state17.var_8));
            rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_8) : (0xefd849aa));
            rb_state17.var_11 = (rb_state17.var_11) ^ (((0x90c69cdc) - (rb_state17.var_9)) - (0xd11d73a7));
            rb_state17.var_12 = (rb_state17.var_12) + (((((0xf21f9b02) - (rb_state17.var_10)) - (0x9d7d1d71)) + (rb_state17.var_9)) ^ (rb_state17.var_11));
            rb_state17.var_6 = (rb_state17.var_6) - (rb_state17.var_12);
            rb_state17.var_5 = rb_state17.var_6;
            pthread_mutex_unlock(&(rb_state17.lock_27));
        }
        if ((rb_state17.var_4) == (0x25d)) {
            if (!((rb_state17.var_5) == (rb_state17.var_6))) {
                racebench_trigger(17);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_4) == (0x0)) {
            rb_state18.var_28 = 0xfcd4bf28;
            rb_state18.var_29 = 0xca10d97c;
            rb_state18.var_30 = 0xce0b1685;
            rb_state18.var_31 = (rb_state18.var_31) - (rb_state18.var_29);
            rb_state18.var_32 = (rb_state18.var_32) ^ (((rb_state18.var_30) == (0x0)) ? (rb_state18.var_28) : (0x25403e0c));
            rb_state18.var_33 = (rb_state18.var_33) - (rb_state18.var_31);
            rb_state18.var_34 = (rb_state18.var_34) ^ (((rb_state18.var_32) == (0x0)) ? (rb_state18.var_33) : (0x113286b4));
            rb_state18.var_5 = (rb_state18.var_5) - (rb_state18.var_34);
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
        rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0x548af86a));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) - (102579 < rb_input_size ? (uint32_t)rb_input[102579] : 0x361d70d7);
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_1);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
        rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_1);
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_2);
        rb_state2.var_1 = (rb_state2.var_1) ^ ((0xdd7774ec) ^ (rb_state2.var_2));
        rb_state2.var_4 = (rb_state2.var_4) + (rb_state2.var_4);
        if ((rb_state2.var_4) == (0x0)) {
            rb_state2.var_15 = 0xca5225c7;
            rb_state2.var_16 = (rb_state2.var_16) ^ (((0xa148df2) ^ (rb_state2.var_8)) ^ (rb_state2.var_10));
            rb_state2.var_17 = (rb_state2.var_17) - (((((0x33695f3e) + (rb_state2.var_11)) ^ (rb_state2.var_12)) ^ (rb_state2.var_15)) ^ (rb_state2.var_9));
            rb_state2.var_18 = (rb_state2.var_18) ^ (rb_state2.var_16);
            rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_17) == (0x0)) ? (rb_state2.var_18) : (0xdbc60e7f));
            rb_state2.var_5 = (rb_state2.var_5) - (rb_state2.var_19);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (161135 < rb_input_size ? (uint32_t)rb_input[161135] : 0x36883377);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_3);
        rb_state4.var_3 = (rb_state4.var_3) - (42143 < rb_input_size ? (uint32_t)rb_input[42143] : 0xbb496738);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (48544 < rb_input_size ? (uint32_t)rb_input[48544] : 0xd1b6e3ce);
        rb_state5.var_0 = (rb_state5.var_0) - (((rb_state5.var_0) == (0xd1cf751f)) ? (rb_state5.var_1) : (0x689600a1));
        rb_state5.var_2 = (rb_state5.var_2) + ((0x9eb668ca) ^ (rb_state5.var_2));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_2) : (0x6de56b97));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ (0x33627305);
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_1);
        if ((rb_state8.var_4) == (0x0)) {
            rb_state8.var_20 = 0xcb25e045;
            rb_state8.var_21 = 0xf3c04568;
            rb_state8.var_22 = 0x49d37747;
            rb_state8.var_23 = (rb_state8.var_23) + (((((0xd469b020) + (rb_state8.var_8)) - (rb_state8.var_20)) ^ (rb_state8.var_9)) - (rb_state8.var_22));
            rb_state8.var_24 = (rb_state8.var_24) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_21) : (0x55eab014));
            rb_state8.var_25 = (rb_state8.var_25) ^ (rb_state8.var_23);
            rb_state8.var_26 = (rb_state8.var_26) + (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_24) : (0xc0ed1fab));
            rb_state8.var_27 = (rb_state8.var_27) - (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_25) : (0x5a8c7745));
            rb_state8.var_28 = (rb_state8.var_28) + (((0x1b1e0ca2) - (rb_state8.var_26)) - (rb_state8.var_13));
            rb_state8.var_29 = (rb_state8.var_29) + (((0x5ef3749a) ^ (rb_state8.var_14)) - (rb_state8.var_27));
            rb_state8.var_30 = (rb_state8.var_30) + (((0x65e00402) - (0xe20f8882)) ^ (rb_state8.var_28));
            rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_29) == (0x0)) ? (rb_state8.var_30) : (0xe0b8853a));
            rb_state8.var_19 = (rb_state8.var_19) + (rb_state8.var_31);
            rb_state8.var_5 = rb_state8.var_19;
        }
        if ((rb_state8.var_4) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_42));
            rb_state8.var_33 = 0xdd155ea8;
            rb_state8.var_34 = 0xd85eb331;
            rb_state8.var_35 = (rb_state8.var_35) ^ (((rb_state8.var_16) == (0x0)) ? (rb_state8.var_34) : (0x56b44ea3));
            rb_state8.var_36 = (rb_state8.var_36) - (((((0x1404b3fd) ^ (rb_state8.var_17)) ^ (0x7ae4f130)) - (rb_state8.var_15)) - (rb_state8.var_33));
            rb_state8.var_37 = (rb_state8.var_37) ^ (rb_state8.var_35);
            rb_state8.var_38 = (rb_state8.var_38) - (((rb_state8.var_18) == (0x0)) ? (rb_state8.var_36) : (0x7f22edf4));
            rb_state8.var_39 = (rb_state8.var_39) ^ (((0x701e162b) + (rb_state8.var_19)) - (rb_state8.var_37));
            rb_state8.var_40 = (rb_state8.var_40) + (((rb_state8.var_20) == (0x0)) ? (rb_state8.var_38) : (0x2a97204b));
            rb_state8.var_41 = (rb_state8.var_41) - (((((0x67b46aba) ^ (rb_state8.var_40)) + (0x74ca4ad4)) - (rb_state8.var_39)) ^ (0x939f053));
            rb_state8.var_32 = (rb_state8.var_32) + (rb_state8.var_41);
            rb_state8.var_5 = rb_state8.var_32;
            pthread_mutex_unlock(&(rb_state8.lock_42));
        }
        rb_state8.var_23 = (rb_state8.var_23) - (0x50370799);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - ((0xada1c8b5) + (rb_state9.var_0));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_0);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + (rb_state12.var_0);
        rb_state12.var_2 = (rb_state12.var_2) - (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_2) : (0x13dc6c27));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ ((0x4e55315e) ^ (rb_state13.var_0));
        rb_state13.var_0 = (rb_state13.var_0) ^ (0xcb781892);
        rb_state13.var_1 = (rb_state13.var_1) ^ (0xe93a580d);
        rb_state13.var_3 = (rb_state13.var_3) + (147488 < rb_input_size ? (uint32_t)rb_input[147488] : 0x4e003dcf);
        if ((rb_state13.var_3) == (0x7)) {
            rb_state13.var_5 = 0x16d76a23;
            rb_state13.var_6 = (rb_state13.var_6) ^ (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_6) : (0x9062da2));
            rb_state13.var_7 = (rb_state13.var_7) ^ (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_5) : (0x498c9344));
            rb_state13.var_8 = (rb_state13.var_8) - (((0x7f27259a) - (rb_state13.var_6)) - (0xef323884));
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0x8813074b));
            rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_9);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) - (169108 < rb_input_size ? (uint32_t)rb_input[169108] : 0x5e845e6b);
        rb_state14.var_2 = (rb_state14.var_2) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0xb305f2d1));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0xc726c57d) - (rb_state15.var_0));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
        rb_state16.var_0 = (rb_state16.var_0) + (0x7b90d330);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - ((0xb1ce869b) ^ (0x930a4b73));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0xd447337d));
        rb_state18.var_1 = (rb_state18.var_1) - (0x9b921c72);
        #endif
        targs[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_2) : (0x6904230e));
        rb_state0.var_0 = (rb_state0.var_0) ^ ((0x6131a6fa) + (0xcd92679a));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - ((0xc5d6b896) - (0xda68fbf3));
        rb_state1.var_4 = (rb_state1.var_4) ^ ((0xe0667bf7) - (rb_state1.var_3));
        rb_state1.var_4 = (rb_state1.var_4) - (((rb_state1.var_3) == (0xa8a34b7d)) ? (rb_state1.var_2) : (0xf6424611));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
        rb_state2.var_2 = (rb_state2.var_2) - (rb_state2.var_0);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_0) == (0xa)) ? (rb_state3.var_1) : (0x6a7227f2));
        rb_state3.var_3 = (rb_state3.var_3) ^ (0x9f4193e3);
        rb_state3.var_3 = (rb_state3.var_3) ^ ((0x20edf517) ^ (rb_state3.var_3));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (0x846f2182);
        rb_state4.var_1 = (rb_state4.var_1) - (116087 < rb_input_size ? (uint32_t)rb_input[116087] : 0x9bec2eab);
        rb_state4.var_2 = (rb_state4.var_2) ^ ((0x6b6d809c) - (rb_state4.var_0));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) + ((0x33e10f9a) - (0x6569ff4f));
        rb_state5.var_0 = (rb_state5.var_0) + ((0x10a25ccc) ^ (rb_state5.var_1));
        rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_1);
        if ((rb_state5.var_3) == (0x0)) {
            rb_state5.var_12 = !((rb_state5.var_5) == (rb_state5.var_4));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0x71e94f9d)) ? (rb_state6.var_0) : (0x34651b46));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_3 = (rb_state7.var_3) ^ ((0x4d24e827) - (rb_state7.var_0));
        rb_state7.var_1 = (rb_state7.var_1) ^ (12982 < rb_input_size ? (uint32_t)rb_input[12982] : 0xa7622a7d);
        if ((rb_state7.var_2) == (0x0)) {
            rb_state7.var_6 = (rb_state7.var_6) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x6fde4353));
            rb_state7.var_7 = (rb_state7.var_7) ^ (rb_state7.var_4);
            rb_state7.var_8 = (rb_state7.var_8) + ((0xb4d61a81) - (0xecf338ce));
            rb_state7.var_9 = (rb_state7.var_9) ^ (((((0xe898101f) ^ (rb_state7.var_6)) - (rb_state7.var_8)) ^ (rb_state7.var_7)) - (rb_state7.var_7));
            rb_state7.var_10 = (rb_state7.var_10) ^ (rb_state7.var_8);
            rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_9);
            rb_state7.var_12 = (rb_state7.var_12) - (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_11) : (0xc0a1d2e8));
            rb_state7.var_3 = (rb_state7.var_3) + (rb_state7.var_12);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (5484 < rb_input_size ? (uint32_t)rb_input[5484] : 0x4964feb4);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_1);
        rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_1);
        rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_1);
        rb_state9.var_3 = (rb_state9.var_3) + ((0xca4f2144) ^ (0xe14afb99));
        if ((rb_state9.var_4) == (0xd2258914)) {
            rb_state9.var_6 = 0xd5a12ef8;
            rb_state9.var_7 = 0x480495aa;
            rb_state9.var_8 = (rb_state9.var_8) - (rb_state9.var_7);
            rb_state9.var_9 = (rb_state9.var_9) + (((0x1a8d90f9) ^ (0x1904afd8)) ^ (rb_state9.var_6));
            rb_state9.var_10 = (rb_state9.var_10) - (((0x2a6ce37c) ^ (rb_state9.var_5)) ^ (0x334c6b03));
            rb_state9.var_11 = (rb_state9.var_11) + (((((0x6fdd4285) ^ (rb_state9.var_9)) ^ (0x3152e803)) ^ (rb_state9.var_8)) - (rb_state9.var_6));
            rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_10);
            rb_state9.var_13 = (rb_state9.var_13) ^ (((0x90cdebb9) + (0xd692adc1)) + (rb_state9.var_11));
            rb_state9.var_14 = (rb_state9.var_14) - (((0xd43133aa) + (rb_state9.var_12)) - (rb_state9.var_7));
            rb_state9.var_15 = (rb_state9.var_15) - (((((0xb47f5d01) - (rb_state9.var_13)) - (0x6c71c726)) ^ (rb_state9.var_14)) ^ (0xc619b73d));
            rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_1);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) ^ (43024 < rb_input_size ? (uint32_t)rb_input[43024] : 0x610f67eb);
        rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
        rb_state12.var_0 = (rb_state12.var_0) + ((0x3f0f3822) - (0xe82e8d41));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) ^ (92736 < rb_input_size ? (uint32_t)rb_input[92736] : 0x3ac3c242);
        rb_state13.var_1 = (rb_state13.var_1) - (((rb_state13.var_1) == (0xe93a586b)) ? (rb_state13.var_0) : (0x379fb749));
        rb_state13.var_2 = (rb_state13.var_2) + (((rb_state13.var_2) == (0x0)) ? (rb_state13.var_2) : (0x9b3cd595));
        rb_state13.var_3 = (rb_state13.var_3) - ((0x74cc9b6) ^ (rb_state13.var_2));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) ^ (73284 < rb_input_size ? (uint32_t)rb_input[73284] : 0xfbafbeca);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (0x7af0c608);
        rb_state16.var_1 = (rb_state16.var_1) ^ (16509 < rb_input_size ? (uint32_t)rb_input[16509] : 0x3573fd3d);
        rb_state16.var_2 = (rb_state16.var_2) + ((0x3fa53d0b) - (0xbfbb7764));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ (102343 < rb_input_size ? (uint32_t)rb_input[102343] : 0x13c53f86);
        rb_state17.var_3 = (rb_state17.var_3) + (158365 < rb_input_size ? (uint32_t)rb_input[158365] : 0x7e41ec0c);
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_3);
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_2) == (0xffffffae)) ? (rb_state17.var_1) : (0x2aad1ccd));
        #endif
        targs[i].frames = framenum;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_0);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (0x896bceb);
        rb_state1.var_2 = (rb_state1.var_2) - (0x25bf6e05);
        if ((rb_state1.var_1) == (0x0)) {
            rb_state1.var_5 = rb_state1.var_2;
        }
        if ((rb_state1.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_15));
            if (!((rb_state1.var_2) == (rb_state1.var_5))) {
                racebench_trigger(1);
            }
            pthread_mutex_unlock(&(rb_state1.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_3);
        rb_state2.var_3 = (rb_state2.var_3) + (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (rb_state3.var_2);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_0) == (0xca5f82c6)) ? (rb_state4.var_2) : (0x59b7b543));
        rb_state4.var_1 = (rb_state4.var_1) + ((0x36225875) + (0x8396be05));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_10 = (rb_state5.var_10) ^ (139023 < rb_input_size ? (uint32_t)rb_input[139023] : 0x47953bcc);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (146441 < rb_input_size ? (uint32_t)rb_input[146441] : 0x31e28bc9);
        rb_state6.var_3 = (rb_state6.var_3) ^ ((0x8eab79a0) ^ (rb_state6.var_3));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_2) : (0xcd2b33da));
        rb_state7.var_4 = (rb_state7.var_4) + (76706 < rb_input_size ? (uint32_t)rb_input[76706] : 0xf10feba2);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_0);
        rb_state8.var_2 = (rb_state8.var_2) ^ ((0x511eb98a) + (0x70aa8bf5));
        rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_1);
        rb_state8.var_3 = (rb_state8.var_3) + ((0x4ab73675) - (0x96f2a801));
        rb_state8.var_3 = (rb_state8.var_3) ^ (172420 < rb_input_size ? (uint32_t)rb_input[172420] : 0x32a38a5a);
        rb_state8.var_3 = (rb_state8.var_3) ^ ((0x18abbec1) - (0x30d39ebe));
        rb_state8.var_24 = (rb_state8.var_24) - (0x8879d1b6);
        rb_state8.var_26 = (rb_state8.var_26) ^ (rb_state8.var_21);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ ((0x7fe9f2b1) ^ (rb_state9.var_2));
        rb_state9.var_2 = (rb_state9.var_2) ^ (133760 < rb_input_size ? (uint32_t)rb_input[133760] : 0xfaf3badb);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) ^ (0x204a1035);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0x1ed949ac));
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) ^ (0xb569bd55);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) - (((rb_state13.var_1) == (0x640d2e9f)) ? (rb_state13.var_0) : (0x55710fe9));
        rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_3);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (0x11815de2);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_1);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - (0x10dae707);
        rb_state18.var_2 = (rb_state18.var_2) - (((rb_state18.var_1) == (0x5392fc87)) ? (rb_state18.var_3) : (0xb8c237a0));
        rb_state18.var_2 = (rb_state18.var_2) + ((0x859fa6b3) + (0x881e65e6));
        #endif
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_2) == (0x0)) {
            rb_state0.var_5 = 0x7c5d0777;
            rb_state0.var_6 = (rb_state0.var_6) - (((0x1fb4894d) + (0xf35e546e)) - (rb_state0.var_4));
            rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_5) == (0x7c5d0777)) ? (rb_state0.var_5) : (0x4e336f70));
            rb_state0.var_8 = (rb_state0.var_8) ^ (((((0xb7f1d2d9) + (rb_state0.var_7)) + (rb_state0.var_6)) - (rb_state0.var_6)) ^ (rb_state0.var_7));
            rb_state0.var_3 = (rb_state0.var_3) ^ (rb_state0.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_3) == (0x20edf517)) {
            rb_state3.var_10 = 0x675a7faf;
            rb_state3.var_11 = (rb_state3.var_11) - (((((0x30fc4892) ^ (rb_state3.var_10)) - (rb_state3.var_9)) - (rb_state3.var_8)) + (0x5d98fc79));
            rb_state3.var_12 = (rb_state3.var_12) - (((((0x5230b9fc) + (rb_state3.var_11)) - (0x54b54d7e)) + (0xd3bc4395)) + (rb_state3.var_7));
            rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_12);
            rb_state3.var_5 = rb_state3.var_9;
        }
        if ((rb_state3.var_3) == (0x20edf517)) {
            pthread_mutex_lock(&(rb_state3.lock_24));
            rb_state3.var_14 = 0x4da6dc20;
            rb_state3.var_15 = 0x91d1ef2f;
            rb_state3.var_16 = (rb_state3.var_16) + (((((0x5ff8751) - (rb_state3.var_11)) + (rb_state3.var_15)) ^ (rb_state3.var_10)) - (0xae078871));
            rb_state3.var_17 = (rb_state3.var_17) + ((0x403aac6e) + (0x5b6c771f));
            rb_state3.var_18 = (rb_state3.var_18) - (rb_state3.var_14);
            rb_state3.var_19 = (rb_state3.var_19) - (rb_state3.var_16);
            rb_state3.var_20 = (rb_state3.var_20) ^ (((rb_state3.var_12) == (0xe40783a3)) ? (rb_state3.var_17) : (0x101376d6));
            rb_state3.var_21 = (rb_state3.var_21) ^ (((((0xff1ea17c) - (0x5fb7cbed)) ^ (rb_state3.var_19)) - (rb_state3.var_18)) + (0xf191500e));
            rb_state3.var_22 = (rb_state3.var_22) + (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_20) : (0x3db3e696));
            rb_state3.var_23 = (rb_state3.var_23) - (((rb_state3.var_21) == (0x5c129385)) ? (rb_state3.var_22) : (0xb854c33c));
            rb_state3.var_13 = (rb_state3.var_13) ^ (rb_state3.var_23);
            rb_state3.var_5 = rb_state3.var_13;
            pthread_mutex_unlock(&(rb_state3.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_4) == (0xfffffe20)) {
            rb_state4.var_8 = 0xef7f3e2d;
            rb_state4.var_9 = (rb_state4.var_9) + (rb_state4.var_7);
            rb_state4.var_10 = (rb_state4.var_10) + (((0x463651d5) - (rb_state4.var_8)) - (rb_state4.var_8));
            rb_state4.var_11 = (rb_state4.var_11) + (rb_state4.var_6);
            rb_state4.var_12 = (rb_state4.var_12) + (((0x16f83ef7) ^ (rb_state4.var_9)) ^ (rb_state4.var_9));
            rb_state4.var_13 = (rb_state4.var_13) ^ (((((0x581c85bd) - (rb_state4.var_10)) + (0x2f5a8341)) + (rb_state4.var_11)) ^ (0x689957ab));
            rb_state4.var_14 = (rb_state4.var_14) ^ (((((0x19e11f95) + (rb_state4.var_12)) - (0xe1563a0e)) + (rb_state4.var_13)) ^ (rb_state4.var_10));
            rb_state4.var_7 = (rb_state4.var_7) ^ (rb_state4.var_14);
            rb_state4.var_5 = rb_state4.var_7;
        }
        if ((rb_state4.var_4) == (0xfffffe20)) {
            pthread_mutex_lock(&(rb_state4.lock_24));
            rb_state4.var_16 = 0x9f7de3c1;
            rb_state4.var_17 = (rb_state4.var_17) + (((0x970a5dc0) ^ (rb_state4.var_13)) ^ (rb_state4.var_11));
            rb_state4.var_18 = (rb_state4.var_18) + (5845 < rb_input_size ? (uint32_t)rb_input[5845] : 0x1fa102cf);
            rb_state4.var_19 = (rb_state4.var_19) ^ (((rb_state4.var_12) == (0x16f83ef7)) ? (rb_state4.var_16) : (0x575014d3));
            rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_14) == (0xff1e5ddd)) ? (rb_state4.var_17) : (0xaeb3ce87));
            rb_state4.var_21 = (rb_state4.var_21) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_18) : (0xb37b6c9d));
            rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_19) == (0x9f7de3c1)) ? (rb_state4.var_20) : (0xb8874dbe));
            rb_state4.var_23 = (rb_state4.var_23) - (((rb_state4.var_21) == (0x4a)) ? (rb_state4.var_22) : (0x642b8c3f));
            rb_state4.var_15 = (rb_state4.var_15) + (rb_state4.var_23);
            rb_state4.var_5 = rb_state4.var_15;
            pthread_mutex_unlock(&(rb_state4.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_6) == (0xb33f2b35)) {
            rb_state5.var_13 = rb_state5.var_10;
        }
        if ((rb_state5.var_6) == (0xb33f2b35)) {
            if (!((rb_state5.var_10) == (rb_state5.var_13))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0x0)) {
            if ((rb_state6.var_4) != (0x0)) {
                if (!((rb_state6.var_4) == (rb_state6.var_20))) {
                    racebench_trigger(6);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_22) == (0x49d37747)) {
            pthread_mutex_lock(&(rb_state8.lock_70));
            rb_state8.var_45 = (rb_state8.var_45) ^ (((rb_state8.var_32) == (0xbfb3a23e)) ? (rb_state8.var_31) : (0x4a080128));
            rb_state8.var_46 = (rb_state8.var_46) ^ (0xf2f48032);
            rb_state8.var_47 = (rb_state8.var_47) + (((rb_state8.var_33) == (0xdd155ea8)) ? (rb_state8.var_30) : (0x50cb540b));
            rb_state8.var_48 = (rb_state8.var_48) + (0x592d7db2);
            rb_state8.var_49 = (rb_state8.var_49) + (((rb_state8.var_34) == (0xd85eb331)) ? (rb_state8.var_29) : (0x1c5edcdd));
            rb_state8.var_50 = (rb_state8.var_50) + (((rb_state8.var_35) == (0xd85eb331)) ? (rb_state8.var_45) : (0x4e8c5f83));
            rb_state8.var_51 = (rb_state8.var_51) - (((0xbe0c800e) + (0x62793ce1)) ^ (rb_state8.var_46));
            rb_state8.var_52 = (rb_state8.var_52) ^ (((rb_state8.var_36) == (0x6e351bdb)) ? (rb_state8.var_47) : (0x784e2810));
            rb_state8.var_53 = (rb_state8.var_53) ^ (((0x307e4133) + (rb_state8.var_37)) + (rb_state8.var_48));
            rb_state8.var_54 = (rb_state8.var_54) + (((((0xbb160a3b) ^ (rb_state8.var_49)) + (rb_state8.var_38)) ^ (rb_state8.var_50)) ^ (rb_state8.var_39));
            rb_state8.var_55 = (rb_state8.var_55) - (((rb_state8.var_40) == (0x2a97204b)) ? (rb_state8.var_51) : (0xa9217147));
            rb_state8.var_56 = (rb_state8.var_56) - (((0x5cd7ffad) - (rb_state8.var_52)) - (0x64805c66));
            rb_state8.var_57 = (rb_state8.var_57) + (rb_state8.var_53);
            rb_state8.var_58 = (rb_state8.var_58) ^ (rb_state8.var_54);
            rb_state8.var_59 = (rb_state8.var_59) - (((rb_state8.var_41) == (0xbfb3a23e)) ? (rb_state8.var_55) : (0xc49bcaf7));
            rb_state8.var_60 = (rb_state8.var_60) + (((0x8e418da4) - (0x6060885e)) + (rb_state8.var_56));
            rb_state8.var_61 = (rb_state8.var_61) - (((rb_state8.var_43) == (0x0)) ? (rb_state8.var_57) : (0x93c0ab79));
            rb_state8.var_62 = (rb_state8.var_62) ^ (rb_state8.var_58);
            rb_state8.var_63 = (rb_state8.var_63) - (rb_state8.var_59);
            rb_state8.var_64 = (rb_state8.var_64) + (((rb_state8.var_44) == (0xfe51f2a6)) ? (rb_state8.var_60) : (0x85bdac8b));
            rb_state8.var_65 = (rb_state8.var_65) - (((0xdd96908e) - (rb_state8.var_61)) - (0xddbb88e));
            rb_state8.var_66 = (rb_state8.var_66) ^ (((rb_state8.var_62) == (0xc061f58e)) ? (rb_state8.var_63) : (0x6c33962));
            rb_state8.var_67 = (rb_state8.var_67) - (((rb_state8.var_45) == (0xe0b8853a)) ? (rb_state8.var_64) : (0x67a45f3a));
            rb_state8.var_68 = (rb_state8.var_68) - (((rb_state8.var_65) == (0xce3ab5ea)) ? (rb_state8.var_66) : (0x6cb5c1ce));
            rb_state8.var_69 = (rb_state8.var_69) + (((rb_state8.var_67) == (0x4cf514db)) ? (rb_state8.var_68) : (0x24d5583d));
            rb_state8.var_28 = (rb_state8.var_28) ^ (rb_state8.var_69);
            pthread_mutex_unlock(&(rb_state8.lock_70));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_4) == (0xbf165a46)) {
            pthread_mutex_lock(&(rb_state10.lock_27));
            if ((rb_state10.var_5) != (0x0)) {
                if (!((rb_state10.var_5) == (rb_state10.var_21))) {
                    racebench_trigger(10);
                }
            }
            pthread_mutex_unlock(&(rb_state10.lock_27));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_1) == (0xadc15568)) {
            rb_state12.var_5 = 0xa5a433ef;
            rb_state12.var_6 = 0xee88bc8b;
            rb_state12.var_7 = (rb_state12.var_7) + (0x906d6bf3);
            rb_state12.var_8 = (rb_state12.var_8) + (rb_state12.var_4);
            rb_state12.var_9 = (rb_state12.var_9) + (((((0xca0213aa) + (rb_state12.var_5)) + (rb_state12.var_6)) ^ (rb_state12.var_6)) + (rb_state12.var_5));
            rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_7) == (0x906d6bf3)) ? (rb_state12.var_7) : (0x57139def));
            rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_8);
            rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_9);
            rb_state12.var_13 = (rb_state12.var_13) ^ (((((0x22440164) ^ (rb_state12.var_10)) - (rb_state12.var_11)) + (0xce890cd8)) ^ (0xa94d185f));
            rb_state12.var_14 = (rb_state12.var_14) - (((rb_state12.var_8) == (0xd84725d8)) ? (rb_state12.var_12) : (0xcefe31a5));
            rb_state12.var_15 = (rb_state12.var_15) + (((((0x1e6931ce) - (rb_state12.var_9)) ^ (rb_state12.var_14)) - (rb_state12.var_10)) - (rb_state12.var_13));
            rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_4 = (rb_state17.var_4) ^ (83318 < rb_input_size ? (uint32_t)rb_input[83318] : 0xcb85453f);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_4);
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