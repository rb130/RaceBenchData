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
                        std::cerr << "FATAL ERROR: Courant???Friedrichs???Lewy condition not satisfied." << std::endl;
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
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_2);
    if ((rb_state5.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state5.lock_9));
        rb_state5.var_4 = 0xd5a6aebe;
        rb_state5.var_5 = 0xc530e1cc;
        rb_state5.var_6 = (rb_state5.var_6) ^ (((((0x718c150d) + (0xdabcbb0e)) ^ (rb_state5.var_3)) + (0xe3f54463)) ^ (rb_state5.var_5));
        rb_state5.var_7 = (rb_state5.var_7) - (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_4) : (0x57f99e25));
        rb_state5.var_8 = (rb_state5.var_8) + (((((0x233fed96) ^ (0xae7d2baf)) - (rb_state5.var_6)) ^ (rb_state5.var_7)) ^ (0x49300714));
        rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_8);
        pthread_mutex_unlock(&(rb_state5.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (37600 < rb_input_size ? (uint32_t)rb_input[37600] : 0x70258ce2);
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + ((0xdfbd1613) ^ (rb_state7.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_3) == (0xcfa96c47)) {
        rb_state10.var_31 = 0xd1f2708e;
        rb_state10.var_32 = 0x14e3344a;
        rb_state10.var_33 = 0xbc50fd60;
        rb_state10.var_34 = (rb_state10.var_34) + (((rb_state10.var_26) == (0x4f0bf2)) ? (rb_state10.var_25) : (0x5b01c40a));
        rb_state10.var_35 = (rb_state10.var_35) ^ (rb_state10.var_32);
        rb_state10.var_36 = (rb_state10.var_36) + (((0x789fe8fb) + (rb_state10.var_27)) ^ (rb_state10.var_33));
        rb_state10.var_37 = (rb_state10.var_37) + (((rb_state10.var_28) == (0x82b51c9d)) ? (rb_state10.var_31) : (0x8cb83cdc));
        rb_state10.var_38 = (rb_state10.var_38) ^ (((rb_state10.var_34) == (0x23bd5ce2)) ? (rb_state10.var_35) : (0xfe0d94ef));
        rb_state10.var_39 = (rb_state10.var_39) - (((rb_state10.var_36) == (0x570b8b8d)) ? (rb_state10.var_37) : (0x8bae1808));
        rb_state10.var_40 = (rb_state10.var_40) ^ (((rb_state10.var_38) == (0x14e3344a)) ? (rb_state10.var_39) : (0xf87342e7));
        rb_state10.var_20 = (rb_state10.var_20) ^ (rb_state10.var_40);
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) ^ (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_2) : (0x18356f5c));
    if ((rb_state17.var_2) == (0x0)) {
        rb_state17.var_3 = rb_state17.var_1;
    }
    if ((rb_state17.var_2) == (0x0)) {
        if (!((rb_state17.var_1) == (rb_state17.var_3))) {
            racebench_trigger(17);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x884025b6)) {
        pthread_mutex_lock(&(rb_state19.lock_28));
        rb_state19.var_17 = 0xbc7be304;
        rb_state19.var_18 = (rb_state19.var_18) ^ (((rb_state19.var_17) == (0xbc7be304)) ? (rb_state19.var_12) : (0x24d28a40));
        rb_state19.var_19 = (rb_state19.var_19) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_13) : (0xd22f697b));
        rb_state19.var_20 = (rb_state19.var_20) + (rb_state19.var_15);
        rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_18) == (0x0)) ? (rb_state19.var_19) : (0x72b17d96));
        rb_state19.var_22 = (rb_state19.var_22) - (((((0x9227a3d8) ^ (rb_state19.var_21)) - (rb_state19.var_20)) ^ (rb_state19.var_17)) - (rb_state19.var_16));
        rb_state19.var_16 = (rb_state19.var_16) + (rb_state19.var_22);
        rb_state19.var_3 = rb_state19.var_16;
        pthread_mutex_unlock(&(rb_state19.lock_28));
    }
    if ((rb_state19.var_2) == (0x884025b6)) {
        pthread_mutex_lock(&(rb_state19.lock_28));
        rb_state19.var_24 = 0x1afdca12;
        rb_state19.var_25 = (rb_state19.var_25) - (((((0x9df308ac) + (0xc64ac007)) ^ (0xdd0b1628)) - (rb_state19.var_19)) + (rb_state19.var_24));
        rb_state19.var_26 = (rb_state19.var_26) - (((0xe9213d93) ^ (rb_state19.var_20)) - (rb_state19.var_18));
        rb_state19.var_27 = (rb_state19.var_27) ^ (((rb_state19.var_25) == (0x2bcb5753)) ? (rb_state19.var_26) : (0x8289684e));
        rb_state19.var_23 = (rb_state19.var_23) - (rb_state19.var_27);
        rb_state19.var_3 = rb_state19.var_23;
        pthread_mutex_unlock(&(rb_state19.lock_28));
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
    rb_state0.var_1 = (rb_state0.var_1) ^ (rb_state0.var_0);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x8d6f2073) - (0xf3b6f99));
    rb_state1.var_2 = (rb_state1.var_2) - (147384 < rb_input_size ? (uint32_t)rb_input[147384] : 0x73cc1247);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_2 = (rb_state2.var_2) - (0xbf745720);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
    rb_state4.var_1 = (rb_state4.var_1) - (0xac15a1fe);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (118858 < rb_input_size ? (uint32_t)rb_input[118858] : 0xb64d6c5a);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x108479f0)) {
        pthread_mutex_lock(&(rb_state9.lock_29));
        rb_state9.var_4 = 0x688bd9c5;
        rb_state9.var_5 = (rb_state9.var_5) ^ (rb_state9.var_2);
        rb_state9.var_6 = (rb_state9.var_6) - (rb_state9.var_4);
        rb_state9.var_7 = (rb_state9.var_7) ^ (107730 < rb_input_size ? (uint32_t)rb_input[107730] : 0x6d5f868e);
        rb_state9.var_8 = (rb_state9.var_8) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0x6a14c8cd));
        rb_state9.var_9 = (rb_state9.var_9) + (rb_state9.var_5);
        rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_5) == (0x0)) ? (rb_state9.var_6) : (0xc8db7e6e));
        rb_state9.var_11 = (rb_state9.var_11) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_7) : (0xd842cf58));
        rb_state9.var_12 = (rb_state9.var_12) + (rb_state9.var_8);
        rb_state9.var_13 = (rb_state9.var_13) - (((((0x2789df11) + (rb_state9.var_10)) + (0x84b20173)) + (rb_state9.var_9)) + (rb_state9.var_7));
        rb_state9.var_14 = (rb_state9.var_14) + (((((0xcfc1b31b) + (0xdb82cb81)) - (0x956cc512)) - (rb_state9.var_11)) - (rb_state9.var_12));
        rb_state9.var_15 = (rb_state9.var_15) ^ (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_14) : (0x8162ec20));
        rb_state9.var_3 = (rb_state9.var_3) ^ (rb_state9.var_15);
        rb_state9.var_2 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_29));
    }
    if ((rb_state9.var_0) == (0x58ef36aa)) {
        if (!((rb_state9.var_2) == (rb_state9.var_3))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) - (0x3f35cb8a);
    if ((rb_state10.var_2) == (0x819468ec)) {
        usleep(100);
    }
    if ((rb_state10.var_2) == (0x819468ec)) {
        rb_state10.var_19 = !((rb_state10.var_4) == (rb_state10.var_3));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) + (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_2) : (0xdc810bca));
    if ((rb_state16.var_2) == (0x0)) {
        rb_state16.var_7 = 0x3a355acf;
        rb_state16.var_8 = 0x98922c3e;
        rb_state16.var_9 = (rb_state16.var_9) ^ ((0x5d42f89f) - (0x2cde32a));
        rb_state16.var_10 = (rb_state16.var_10) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_4) : (0x808b9668));
        rb_state16.var_11 = (rb_state16.var_11) + (rb_state16.var_8);
        rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_9);
        rb_state16.var_13 = (rb_state16.var_13) ^ (rb_state16.var_10);
        rb_state16.var_14 = (rb_state16.var_14) - (((0x77d0cfa8) - (rb_state16.var_11)) ^ (0x735539c));
        rb_state16.var_15 = (rb_state16.var_15) + (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x1091d45));
        rb_state16.var_16 = (rb_state16.var_16) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_14) : (0x16ede3df));
        rb_state16.var_17 = (rb_state16.var_17) ^ (rb_state16.var_15);
        rb_state16.var_18 = (rb_state16.var_18) + (((((0xbf5e400c) ^ (rb_state16.var_6)) + (rb_state16.var_7)) - (rb_state16.var_17)) ^ (rb_state16.var_16));
        rb_state16.var_6 = (rb_state16.var_6) + (rb_state16.var_18);
        rb_state16.var_3 = rb_state16.var_6;
    }
    if ((rb_state16.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_24));
        rb_state16.var_20 = 0x55a35f6e;
        rb_state16.var_21 = 0xa3bb3ce4;
        rb_state16.var_22 = (rb_state16.var_22) - (((((0x5f95e998) - (rb_state16.var_20)) ^ (rb_state16.var_21)) + (rb_state16.var_10)) - (rb_state16.var_9));
        rb_state16.var_23 = (rb_state16.var_23) ^ (((((0x31ec7da4) ^ (rb_state16.var_22)) ^ (rb_state16.var_11)) - (rb_state16.var_8)) + (0x76139289));
        rb_state16.var_19 = (rb_state16.var_19) + (rb_state16.var_23);
        rb_state16.var_3 = rb_state16.var_19;
        pthread_mutex_unlock(&(rb_state16.lock_24));
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (90396 < rb_input_size ? (uint32_t)rb_input[90396] : 0xcd2a1d6f);
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_1) : (0x933253fd));
    rb_state18.var_2 = (rb_state18.var_2) - (34467 < rb_input_size ? (uint32_t)rb_input[34467] : 0x81c7079c);
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x884025b6)) {
        if ((rb_state19.var_3) != (0x0)) {
            if (!((rb_state19.var_3) == (rb_state19.var_23))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    thread_args *targs = (thread_args *)args;

    for (int i = 0; i < targs->frames; ++i) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0x5b1722fe)) {
            rb_state0.var_3 = rb_state0.var_1;
        }
        if ((rb_state0.var_0) == (0xb1ec0d83)) {
            if (!((rb_state0.var_1) == (rb_state0.var_3))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_0);
        rb_state1.var_1 = (rb_state1.var_1) - (0x130153b1);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + ((0x673bad98) + (0xb394796d));
        if ((rb_state2.var_2) == (0x811751c0)) {
            pthread_mutex_lock(&(rb_state2.lock_7));
            rb_state2.var_3 = rb_state2.var_0;
            pthread_mutex_unlock(&(rb_state2.lock_7));
        }
        if ((rb_state2.var_2) == (0x811751c0)) {
            if (!((rb_state2.var_0) == (rb_state2.var_3))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0x54cabf7c);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) ^ (46876 < rb_input_size ? (uint32_t)rb_input[46876] : 0x18650605);
        if ((rb_state4.var_0) == (0x0)) {
            rb_state4.var_7 = 0xc99a7e42;
            rb_state4.var_8 = (rb_state4.var_8) + (((rb_state4.var_7) == (0xc99a7e42)) ? (rb_state4.var_5) : (0x2fbf0bac));
            rb_state4.var_9 = (rb_state4.var_9) - (0x2ecf30f6);
            rb_state4.var_10 = (rb_state4.var_10) - (((((0x250d4fbe) - (rb_state4.var_8)) + (rb_state4.var_7)) - (0x6c7318a1)) - (rb_state4.var_6));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0x8082f0aa));
            rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_9) == (0xd130cf0a)) ? (rb_state4.var_10) : (0xc5ffaed0));
            rb_state4.var_13 = (rb_state4.var_13) + (((0x8f5229f6) ^ (rb_state4.var_11)) + (0x5906d895));
            rb_state4.var_14 = (rb_state4.var_14) - (rb_state4.var_12);
            rb_state4.var_15 = (rb_state4.var_15) ^ (((rb_state4.var_10) == (0x7dcb4aa1)) ? (rb_state4.var_13) : (0xca845e50));
            rb_state4.var_16 = (rb_state4.var_16) + (((0x751ebed) - (rb_state4.var_14)) + (0x79da1c1b));
            rb_state4.var_17 = (rb_state4.var_17) + (((0x86245120) - (rb_state4.var_15)) - (0xd135a290));
            rb_state4.var_18 = (rb_state4.var_18) - (((0x3ac833d3) + (rb_state4.var_11)) - (rb_state4.var_16));
            rb_state4.var_19 = (rb_state4.var_19) - (((rb_state4.var_17) == (0xfd84eeff)) ? (rb_state4.var_18) : (0x60add74c));
            rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_19);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (62033 < rb_input_size ? (uint32_t)rb_input[62033] : 0x8af0293c);
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0x17a)) {
            pthread_mutex_lock(&(rb_state6.lock_25));
            rb_state6.var_5 = 0xcadcc5ea;
            rb_state6.var_6 = 0xe8973463;
            rb_state6.var_7 = (rb_state6.var_7) - (rb_state6.var_4);
            rb_state6.var_8 = (rb_state6.var_8) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_5) : (0x1abe15fd));
            rb_state6.var_9 = (rb_state6.var_9) - (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_3) : (0x5d331528));
            rb_state6.var_10 = (rb_state6.var_10) - (((((0x172732e6) + (0x4ff53284)) ^ (rb_state6.var_6)) + (rb_state6.var_7)) ^ (0x14f81d0d));
            rb_state6.var_11 = (rb_state6.var_11) ^ (rb_state6.var_8);
            rb_state6.var_12 = (rb_state6.var_12) + (((((0x7cdfcd01) + (rb_state6.var_9)) + (rb_state6.var_10)) + (0x9e4fe463)) + (0x444ef1e));
            rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_11) : (0x4a1c1b23));
            rb_state6.var_14 = (rb_state6.var_14) + (rb_state6.var_12);
            rb_state6.var_15 = (rb_state6.var_15) ^ (rb_state6.var_13);
            rb_state6.var_16 = (rb_state6.var_16) ^ (rb_state6.var_14);
            rb_state6.var_17 = (rb_state6.var_17) + (((((0x605a361d) - (rb_state6.var_8)) ^ (0x1370ceaa)) - (rb_state6.var_15)) + (rb_state6.var_16));
            rb_state6.var_4 = (rb_state6.var_4) + (rb_state6.var_17);
            rb_state6.var_3 = rb_state6.var_4;
            pthread_mutex_unlock(&(rb_state6.lock_25));
        }
        if ((rb_state6.var_2) == (0x17a)) {
            if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_16);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (((rb_state11.var_2) == (0xd991abe9)) ? (rb_state11.var_1) : (0x8996e63b));
        if ((rb_state11.var_0) == (0x8996e63b)) {
            rb_state11.var_9 = (rb_state11.var_9) ^ (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_4) : (0x14d5ebcd));
            rb_state11.var_10 = (rb_state11.var_10) - (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_9) : (0x30e1412));
            rb_state11.var_3 = (rb_state11.var_3) + (rb_state11.var_10);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (637 < rb_input_size ? (uint32_t)rb_input[637] : 0x3f091e7f);
        rb_state13.var_0 = (rb_state13.var_0) - (78780 < rb_input_size ? (uint32_t)rb_input[78780] : 0xed3a90bb);
        if ((rb_state13.var_2) == (0x0)) {
            rb_state13.var_10 = (rb_state13.var_10) - (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_6) : (0x772b59e8));
            rb_state13.var_11 = (rb_state13.var_11) ^ ((0x4f7dba65) ^ (0xcc42691d));
            rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_8);
            rb_state13.var_13 = (rb_state13.var_13) ^ (134103 < rb_input_size ? (uint32_t)rb_input[134103] : 0x17f138cc);
            rb_state13.var_14 = (rb_state13.var_14) + (((((0x840a1dfd) ^ (rb_state13.var_10)) - (rb_state13.var_11)) - (rb_state13.var_10)) ^ (rb_state13.var_9));
            rb_state13.var_15 = (rb_state13.var_15) + (rb_state13.var_12);
            rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_14) : (0x94cc8876));
            rb_state13.var_17 = (rb_state13.var_17) - (rb_state13.var_15);
            rb_state13.var_18 = (rb_state13.var_18) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_16) : (0x83dbb145));
            rb_state13.var_19 = (rb_state13.var_19) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_17) : (0x6b811fb2));
            rb_state13.var_20 = (rb_state13.var_20) - (((0xbc9b360) + (rb_state13.var_18)) + (0xd5637cb9));
            rb_state13.var_21 = (rb_state13.var_21) - (((rb_state13.var_13) == (0x0)) ? (rb_state13.var_19) : (0xfb18788b));
            rb_state13.var_22 = (rb_state13.var_22) - (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_21) : (0x1a5bc69c));
            rb_state13.var_3 = (rb_state13.var_3) + (rb_state13.var_22);
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_2) == (0x189a91c)) {
            pthread_mutex_lock(&(rb_state14.lock_9));
            rb_state14.var_4 = 0x1a35ddd7;
            rb_state14.var_5 = 0xc5999e1;
            rb_state14.var_6 = 0xf7f1eec1;
            rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_5) == (0xc5999e1)) ? (rb_state14.var_4) : (0x863da0ee));
            rb_state14.var_8 = (rb_state14.var_8) - (((((0x6bebf30c) - (rb_state14.var_6)) - (rb_state14.var_3)) ^ (rb_state14.var_7)) ^ (rb_state14.var_4));
            rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_8);
            pthread_mutex_unlock(&(rb_state14.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_1);
        if ((rb_state16.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_24));
            if ((rb_state16.var_3) != (0x0)) {
                if (!((rb_state16.var_3) == (rb_state16.var_19))) {
                    racebench_trigger(16);
                }
            }
            pthread_mutex_unlock(&(rb_state16.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x0)) {
            rb_state18.var_25 = 0x7b8adfab;
            rb_state18.var_26 = 0x4ced9bc2;
            rb_state18.var_27 = (rb_state18.var_27) + (((rb_state18.var_13) == (0x0)) ? (rb_state18.var_12) : (0x20fc603a));
            rb_state18.var_28 = (rb_state18.var_28) ^ (44810 < rb_input_size ? (uint32_t)rb_input[44810] : 0xe0baa0e0);
            rb_state18.var_29 = (rb_state18.var_29) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_25) : (0xe5c1b8ad));
            rb_state18.var_30 = (rb_state18.var_30) + (((rb_state18.var_15) == (0x0)) ? (rb_state18.var_26) : (0xda412f39));
            rb_state18.var_31 = (rb_state18.var_31) + (((0x15c0e272) ^ (rb_state18.var_27)) ^ (rb_state18.var_16));
            rb_state18.var_32 = (rb_state18.var_32) + (rb_state18.var_28);
            rb_state18.var_33 = (rb_state18.var_33) ^ (((0x4ce1cf36) ^ (rb_state18.var_17)) ^ (rb_state18.var_29));
            rb_state18.var_34 = (rb_state18.var_34) - (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_30) : (0x123bda7));
            rb_state18.var_35 = (rb_state18.var_35) ^ (((((0xa200b188) ^ (rb_state18.var_31)) ^ (rb_state18.var_19)) + (rb_state18.var_32)) + (0x7a051509));
            rb_state18.var_36 = (rb_state18.var_36) ^ (rb_state18.var_33);
            rb_state18.var_37 = (rb_state18.var_37) - (rb_state18.var_34);
            rb_state18.var_38 = (rb_state18.var_38) ^ (((rb_state18.var_35) == (0x0)) ? (rb_state18.var_36) : (0xd9978275));
            rb_state18.var_39 = (rb_state18.var_39) - (((rb_state18.var_20) == (0x0)) ? (rb_state18.var_37) : (0x6a1f4096));
            rb_state18.var_40 = (rb_state18.var_40) ^ (((rb_state18.var_38) == (0x0)) ? (rb_state18.var_39) : (0xf4891583));
            rb_state18.var_24 = (rb_state18.var_24) - (rb_state18.var_40);
            rb_state18.var_3 = rb_state18.var_24;
        }
        if ((rb_state18.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state18.lock_55));
            rb_state18.var_42 = 0x9bba8812;
            rb_state18.var_43 = (rb_state18.var_43) + (13668 < rb_input_size ? (uint32_t)rb_input[13668] : 0x142ead6d);
            rb_state18.var_44 = (rb_state18.var_44) + (((((0x2c6559e0) - (rb_state18.var_21)) ^ (0x839c90a8)) ^ (rb_state18.var_23)) + (rb_state18.var_42));
            rb_state18.var_45 = (rb_state18.var_45) - (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_22) : (0x1893a3b));
            rb_state18.var_46 = (rb_state18.var_46) + (((0x585a4e4c) - (rb_state18.var_43)) ^ (0x95429230));
            rb_state18.var_47 = (rb_state18.var_47) + (((rb_state18.var_25) == (0x0)) ? (rb_state18.var_44) : (0x4fa642bc));
            rb_state18.var_48 = (rb_state18.var_48) - (((rb_state18.var_26) == (0x0)) ? (rb_state18.var_45) : (0x18152a44));
            rb_state18.var_49 = (rb_state18.var_49) ^ (((0x6f6f1601) ^ (rb_state18.var_27)) + (rb_state18.var_46));
            rb_state18.var_50 = (rb_state18.var_50) - (((rb_state18.var_47) == (0x0)) ? (rb_state18.var_48) : (0x94e3078c));
            rb_state18.var_51 = (rb_state18.var_51) + (((0x256fa1c) + (rb_state18.var_49)) - (0xe2a70489));
            rb_state18.var_52 = (rb_state18.var_52) + (((rb_state18.var_28) == (0x0)) ? (rb_state18.var_50) : (0x3e8cef0a));
            rb_state18.var_53 = (rb_state18.var_53) ^ (((rb_state18.var_29) == (0x0)) ? (rb_state18.var_51) : (0x79d45c49));
            rb_state18.var_54 = (rb_state18.var_54) - (((((0xdec06cd2) + (rb_state18.var_52)) ^ (rb_state18.var_30)) ^ (rb_state18.var_53)) - (0x2f4b0380));
            rb_state18.var_41 = (rb_state18.var_41) + (rb_state18.var_54);
            rb_state18.var_3 = rb_state18.var_41;
            pthread_mutex_unlock(&(rb_state18.lock_55));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_0);
        rb_state19.var_1 = (rb_state19.var_1) ^ (0x98495f6e);
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
    std::cout << "WARNING: Check for Courant???Friedrichs???Lewy condition enabled. Do not use for performance measurements." << std::endl;
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
        rb_state0.var_0 = (rb_state0.var_0) + ((0xab21e952) ^ (0xfdf503d7));
        if ((rb_state0.var_0) == (0xb1ec0d83)) {
            rb_state0.var_4 = 0x81cb2e3b;
            rb_state0.var_5 = (rb_state0.var_5) + (((0xb8b11d92) - (0x787f86ee)) + (rb_state0.var_3));
            rb_state0.var_6 = (rb_state0.var_6) ^ (((((0x69e29016) - (rb_state0.var_5)) - (0xc0d56f65)) ^ (0x396fd73b)) ^ (rb_state0.var_4));
            rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (0x210a9404);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state4.lock_20));
            rb_state4.var_5 = (rb_state4.var_5) - (rb_state4.var_4);
            rb_state4.var_6 = (rb_state4.var_6) ^ (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_5) : (0x1596e392));
            rb_state4.var_4 = (rb_state4.var_4) - (rb_state4.var_6);
            rb_state4.var_3 = rb_state4.var_4;
            pthread_mutex_unlock(&(rb_state4.lock_20));
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (0xb7954358);
        rb_state9.var_0 = (rb_state9.var_0) ^ (51112 < rb_input_size ? (uint32_t)rb_input[51112] : 0x7a1cf13f);
        if ((rb_state9.var_0) == (0x58ef36aa)) {
            rb_state9.var_16 = 0xfae28232;
            rb_state9.var_17 = 0x8f2901c4;
            rb_state9.var_18 = (rb_state9.var_18) ^ (34830 < rb_input_size ? (uint32_t)rb_input[34830] : 0x858df37f);
            rb_state9.var_19 = (rb_state9.var_19) ^ (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_17) : (0xf4958d9e));
            rb_state9.var_20 = (rb_state9.var_20) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_8) : (0x975a5c54));
            rb_state9.var_21 = (rb_state9.var_21) - (((0x53999d60) ^ (rb_state9.var_16)) - (rb_state9.var_11));
            rb_state9.var_22 = (rb_state9.var_22) - (rb_state9.var_18);
            rb_state9.var_23 = (rb_state9.var_23) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_20) : (0xcebb2cc1));
            rb_state9.var_24 = (rb_state9.var_24) + (((0xbb702e67) - (rb_state9.var_12)) ^ (rb_state9.var_21));
            rb_state9.var_25 = (rb_state9.var_25) - (rb_state9.var_22);
            rb_state9.var_26 = (rb_state9.var_26) - (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_23) : (0xd33d194));
            rb_state9.var_27 = (rb_state9.var_27) - (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0x2388f1b4));
            rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_26) == (0x0)) ? (rb_state9.var_27) : (0x51493c9a));
            rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (6911 < rb_input_size ? (uint32_t)rb_input[6911] : 0x574865e);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) ^ ((0x488f66a7) + (rb_state11.var_2));
        rb_state11.var_0 = (rb_state11.var_0) ^ ((0x519643c6) ^ (0xee8a33f8));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_1);
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_2);
        rb_state13.var_0 = (rb_state13.var_0) - (0xaa3505ec);
        rb_state13.var_1 = (rb_state13.var_1) - (0x6859003c);
        if ((rb_state13.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_23));
            rb_state13.var_5 = 0x714319ec;
            rb_state13.var_6 = 0xcf5a3cb3;
            rb_state13.var_7 = (rb_state13.var_7) - (((((0xd04f1fe5) - (rb_state13.var_4)) + (rb_state13.var_6)) - (rb_state13.var_5)) - (0xf416cb4e));
            rb_state13.var_8 = (rb_state13.var_8) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_3) : (0xc3a56433));
            rb_state13.var_9 = (rb_state13.var_9) - (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_8) : (0xafecb8dc));
            rb_state13.var_4 = (rb_state13.var_4) + (rb_state13.var_9);
            rb_state13.var_3 = rb_state13.var_4;
            pthread_mutex_unlock(&(rb_state13.lock_23));
        }
        if ((rb_state13.var_2) == (0x0)) {
            if (!((rb_state13.var_3) == (rb_state13.var_4))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ ((0x8053a3a7) - (rb_state14.var_0));
        rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_0) == (0x8053a3a7)) ? (rb_state14.var_1) : (0x1eec0047));
        rb_state14.var_1 = (rb_state14.var_1) ^ (116032 < rb_input_size ? (uint32_t)rb_input[116032] : 0x980d7c6);
        rb_state14.var_0 = (rb_state14.var_0) ^ (rb_state14.var_1);
        if ((rb_state14.var_2) == (0x189a91c)) {
            rb_state14.var_3 = rb_state14.var_0;
        }
        if ((rb_state14.var_2) == (0x189a91c)) {
            if (!((rb_state14.var_0) == (rb_state14.var_3))) {
                racebench_trigger(14);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (156274 < rb_input_size ? (uint32_t)rb_input[156274] : 0xd250190);
        rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_0);
        rb_state16.var_1 = (rb_state16.var_1) + (0xb80dd080);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (123 < rb_input_size ? (uint32_t)rb_input[123] : 0x84b6a56);
        rb_state17.var_1 = (rb_state17.var_1) ^ (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_1) : (0x3db955f9));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_2);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_1) : (0xc27684a0));
        rb_state19.var_2 = (rb_state19.var_2) + (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_2) : (0xeff6c648));
        #endif
        targs[i].tid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_2 = (rb_state0.var_2) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0xf1f33b55));
        rb_state0.var_0 = (rb_state0.var_0) + (62178 < rb_input_size ? (uint32_t)rb_input[62178] : 0x3151d8ba);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (0xdf0502f9);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - ((0xb7273ffd) + (rb_state2.var_0));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) + (0x3f5ae282);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_0) == (0x8a)) ? (rb_state5.var_2) : (0x7094caad));
        rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0x8a)) ? (rb_state5.var_2) : (0x94a5bd16));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (159936 < rb_input_size ? (uint32_t)rb_input[159936] : 0x89009d54);
        rb_state6.var_0 = (rb_state6.var_0) + (2372 < rb_input_size ? (uint32_t)rb_input[2372] : 0x4087c27f);
        rb_state6.var_1 = (rb_state6.var_1) ^ (135099 < rb_input_size ? (uint32_t)rb_input[135099] : 0x73632fed);
        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_1);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0x4330eda8);
        rb_state9.var_1 = (rb_state9.var_1) ^ (116716 < rb_input_size ? (uint32_t)rb_input[116716] : 0x512678c7);
        rb_state9.var_0 = (rb_state9.var_0) ^ ((0xba0c1c88) + (0xc1d7c39));
        rb_state9.var_1 = (rb_state9.var_1) - (82986 < rb_input_size ? (uint32_t)rb_input[82986] : 0x347bf7fc);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (0xb968f9dc);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
        rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_1);
        rb_state11.var_1 = (rb_state11.var_1) - ((0x7908fa78) + (rb_state11.var_2));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) ^ ((0x189a83f) ^ (rb_state14.var_2));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0xbd)) ? (rb_state17.var_0) : (0xb6e506c2));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x24c6b6e5));
        rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_1);
        rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xa312c195));
        rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
        #endif
        targs[i].frames = framenum;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_2);
        rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_1) : (0xad6d4c48));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) ^ (142085 < rb_input_size ? (uint32_t)rb_input[142085] : 0x359f4054);
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_2);
        if ((rb_state2.var_2) == (0x811751c0)) {
            rb_state2.var_4 = 0xe153fe6;
            rb_state2.var_5 = (rb_state2.var_5) + (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_3) : (0xe3e7cced));
            rb_state2.var_6 = (rb_state2.var_6) - (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_5) : (0x529ea3a2));
            rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (rb_state4.var_0);
        rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_1);
        if ((rb_state4.var_0) == (0x0)) {
            if (!((rb_state4.var_3) == (rb_state4.var_4))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state5.lock_9));
            rb_state5.var_3 = rb_state5.var_2;
            pthread_mutex_unlock(&(rb_state5.lock_9));
        }
        if ((rb_state5.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state5.lock_9));
            if (!((rb_state5.var_2) == (rb_state5.var_3))) {
                racebench_trigger(5);
            }
            pthread_mutex_unlock(&(rb_state5.lock_9));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) ^ (0x5e6bb55c);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_0);
        rb_state10.var_0 = (rb_state10.var_0) ^ (0x639c29fd);
        rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_1) == (0xb968fa98)) ? (rb_state10.var_0) : (0x2d0d8d16));
        if ((rb_state10.var_2) == (0x819468ec)) {
            pthread_mutex_lock(&(rb_state10.lock_18));
            rb_state10.var_5 = 0x403322ca;
            rb_state10.var_6 = (rb_state10.var_6) + (rb_state10.var_4);
            rb_state10.var_7 = (rb_state10.var_7) ^ (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_5) : (0xdc17c755));
            rb_state10.var_8 = (rb_state10.var_8) - ((0x1ade07ca) - (0x211db3ba));
            rb_state10.var_9 = (rb_state10.var_9) + (((rb_state10.var_5) == (0x0)) ? (rb_state10.var_3) : (0x389a5ae2));
            rb_state10.var_10 = (rb_state10.var_10) + (((0x8bd5c883) ^ (rb_state10.var_7)) - (rb_state10.var_6));
            rb_state10.var_11 = (rb_state10.var_11) + (((((0xc253b40) - (0xde806d3d)) - (rb_state10.var_8)) - (rb_state10.var_8)) + (rb_state10.var_7));
            rb_state10.var_12 = (rb_state10.var_12) + (((0x7e790111) ^ (rb_state10.var_9)) - (0xd2b99b13));
            rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_11) : (0xb2ecc60c));
            rb_state10.var_14 = (rb_state10.var_14) - (rb_state10.var_12);
            rb_state10.var_15 = (rb_state10.var_15) ^ (((0x8878b4ea) - (rb_state10.var_9)) - (rb_state10.var_13));
            rb_state10.var_16 = (rb_state10.var_16) + (((0x7add22f9) - (rb_state10.var_14)) - (rb_state10.var_10));
            rb_state10.var_17 = (rb_state10.var_17) + (((((0xba365e1e) ^ (rb_state10.var_11)) ^ (rb_state10.var_16)) - (rb_state10.var_15)) + (rb_state10.var_12));
            rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_17);
            rb_state10.var_4 = rb_state10.var_3;
            pthread_mutex_unlock(&(rb_state10.lock_18));
        }
        rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_14);
        rb_state10.var_17 = (rb_state10.var_17) - (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_19) : (0xda289dc6));
        if ((rb_state10.var_3) == (0xcfa96c47)) {
            pthread_mutex_lock(&(rb_state10.lock_41));
            rb_state10.var_22 = 0xa2e864f6;
            rb_state10.var_23 = (rb_state10.var_23) + (((0x764de145) ^ (rb_state10.var_20)) + (0x8a012aad));
            rb_state10.var_24 = (rb_state10.var_24) - (0x5050c77);
            rb_state10.var_25 = (rb_state10.var_25) + (((0xddec92f6) + (rb_state10.var_22)) + (rb_state10.var_22));
            rb_state10.var_26 = (rb_state10.var_26) ^ (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_23) : (0x73a2d6c1));
            rb_state10.var_27 = (rb_state10.var_27) ^ (((((0xb6215951) ^ (0x1c807e34)) - (rb_state10.var_24)) - (rb_state10.var_25)) ^ (0xf9535b08));
            rb_state10.var_28 = (rb_state10.var_28) ^ (((0x834b237d) ^ (rb_state10.var_26)) - (rb_state10.var_23));
            rb_state10.var_29 = (rb_state10.var_29) - (rb_state10.var_27);
            rb_state10.var_30 = (rb_state10.var_30) - (((((0xbacab2f0) - (rb_state10.var_24)) + (rb_state10.var_29)) + (0xec652c8)) + (rb_state10.var_28));
            rb_state10.var_21 = (rb_state10.var_21) + (rb_state10.var_30);
            rb_state10.var_20 = rb_state10.var_21;
            pthread_mutex_unlock(&(rb_state10.lock_41));
        }
        if ((rb_state10.var_3) == (0xcfa96c47)) {
            if (!((rb_state10.var_20) == (rb_state10.var_21))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x8996e63b)) {
            pthread_mutex_lock(&(rb_state11.lock_11));
            rb_state11.var_5 = 0xe56c6589;
            rb_state11.var_6 = 0xdedb6c2a;
            rb_state11.var_7 = (rb_state11.var_7) - (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_3) : (0xf2fb55de));
            rb_state11.var_8 = (rb_state11.var_8) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_7) : (0x6d82b98));
            rb_state11.var_4 = (rb_state11.var_4) - (rb_state11.var_8);
            rb_state11.var_3 = rb_state11.var_4;
            pthread_mutex_unlock(&(rb_state11.lock_11));
        }
        if ((rb_state11.var_0) == (0x8996e63b)) {
            if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                racebench_trigger(11);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (60301 < rb_input_size ? (uint32_t)rb_input[60301] : 0x25961086);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) ^ ((0x1864bc7c) + (0x2fc3142d));
        #endif
        pthread_create(&thread[i], &attr, AdvanceFramesMT, &targs[i]);
    }

#ifdef ENABLE_VISUALIZATION
    Visualize();
#endif

    for (int i = 0; i < threadnum; ++i) {
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_2) == (0x17a)) {
            rb_state6.var_18 = 0x693675e7;
            rb_state6.var_19 = 0x4ee2bbcd;
            rb_state6.var_20 = (rb_state6.var_20) - (((0xab308557) + (rb_state6.var_10)) + (rb_state6.var_19));
            rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_11) == (0x1abe15fd)) ? (rb_state6.var_9) : (0xf48437cd));
            rb_state6.var_22 = (rb_state6.var_22) + (((rb_state6.var_18) == (0x693675e7)) ? (rb_state6.var_20) : (0x8a0ebd94));
            rb_state6.var_23 = (rb_state6.var_23) - (rb_state6.var_21);
            rb_state6.var_24 = (rb_state6.var_24) + (((rb_state6.var_22) == (0xa1600ae0)) ? (rb_state6.var_23) : (0x3a440e4d));
            rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_24);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_2) == (0x0)) {
            rb_state17.var_4 = 0xb91c85dc;
            rb_state17.var_5 = 0x7f2488;
            rb_state17.var_6 = (rb_state17.var_6) ^ (107878 < rb_input_size ? (uint32_t)rb_input[107878] : 0xf3535125);
            rb_state17.var_7 = (rb_state17.var_7) ^ (119109 < rb_input_size ? (uint32_t)rb_input[119109] : 0xcf92bce3);
            rb_state17.var_8 = (rb_state17.var_8) + (((0xac8d7fae) ^ (0xe97f6e90)) - (rb_state17.var_5));
            rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_4) == (0xb91c85dc)) ? (rb_state17.var_3) : (0x9c05d58c));
            rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_5) == (0x7f2488)) ? (rb_state17.var_4) : (0x306eaa74));
            rb_state17.var_11 = (rb_state17.var_11) + (((rb_state17.var_6) == (0x8e)) ? (rb_state17.var_6) : (0x4c807292));
            rb_state17.var_12 = (rb_state17.var_12) - (((rb_state17.var_7) == (0xbd)) ? (rb_state17.var_8) : (0x9574fe8a));
            rb_state17.var_13 = (rb_state17.var_13) ^ (((0xf2470f91) + (rb_state17.var_7)) + (rb_state17.var_9));
            rb_state17.var_14 = (rb_state17.var_14) + (((0x6ea4af94) - (rb_state17.var_8)) + (rb_state17.var_10));
            rb_state17.var_15 = (rb_state17.var_15) + (((((0xa58f0be) + (rb_state17.var_9)) + (rb_state17.var_10)) - (rb_state17.var_11)) + (rb_state17.var_12));
            rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_13) == (0xaa1f3fa5)) ? (rb_state17.var_14) : (0xbf642d2f));
            rb_state17.var_17 = (rb_state17.var_17) ^ (((rb_state17.var_15) == (0xc3a1acf5)) ? (rb_state17.var_16) : (0xb850db4c));
            rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_1) == (0x0)) {
            if ((rb_state18.var_3) != (0x0)) {
                if (!((rb_state18.var_3) == (rb_state18.var_41))) {
                    racebench_trigger(18);
                }
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