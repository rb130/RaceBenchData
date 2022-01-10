#include "racebench_bugs.h"


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/resource.h>
#include <limits.h>

#ifdef ENABLE_THREADS
#include <pthread.h>
#include "parsec_barrier.hpp"
#endif

#ifdef TBB_VERSION
#define TBB_STEALER (tbb::task_scheduler_init::occ_stealer)
#define NUM_DIVISIONS (nproc)
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/cache_aligned_allocator.h"
using namespace tbb;
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

using namespace std;

#define MAXNAMESIZE 1024
#define SEED 1

#define SP 1

#define ITER 3

#define CACHE_LINE 32

typedef struct {
    float weight;
    float *coord;
    long assign;
    float cost;
} Point;

typedef struct {
    long num;
    int dim;
    Point *p;
} Points;

static bool *switch_membership;
static bool *is_center;
static int *center_table;

static int nproc;

#ifdef TBB_VERSION
tbb::cache_aligned_allocator<float> memoryFloat;
tbb::cache_aligned_allocator<Point> memoryPoint;
tbb::cache_aligned_allocator<long> memoryLong;
tbb::cache_aligned_allocator<int> memoryInt;
tbb::cache_aligned_allocator<bool> memoryBool;
#endif

float dist(Point p1, Point p2, int dim);


long myrand(void)
{
    static long x = 0xdeadbeef;
    const long p = 1000000007;
    x = (x * 13 + 0x223333) % p;
    return x & 0xFFFFFFFF;
}

#ifdef TBB_VERSION
struct HizReduction {
private:
    double hiz;

public:
    Points *points;
    HizReduction(Points *points_) : hiz(0), points(points_) {
    }
    HizReduction(HizReduction &d, tbb::split) {
        hiz = 0;
        points = d.points;
    }

    void operator()(const tbb::blocked_range<int> &range) {
        double myhiz = 0;
        long ptDimension = points->dim;
        int begin = range.begin();
        int end = range.end();

        for (int kk = begin; kk != end; kk++) {
            myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
        }
        hiz += myhiz;
    }

    void join(HizReduction &d) {
        hiz += d.getHiz();
    }
    double getHiz() {
        return hiz;
    }
};

struct CenterCreate {
    Points *points;
    CenterCreate(Points *p) : points(p) {
    }
    void operator()(const tbb::blocked_range<int> &range) const {
        int begin = range.begin();
        int end = range.end();

        for (int k = begin; k != end; k++) {
            float distance = dist(points->p[k], points->p[0], points->dim);
            points->p[k].cost = distance * points->p[k].weight;
            points->p[k].assign = 0;
        }
    }
};

struct CenterOpen {
private:
    double total_cost;

public:
    Points *points;
    int i;
    int type;
    CenterOpen(Points *p) : points(p), total_cost(0), type(0) {
    }
    CenterOpen(CenterOpen &rhs, tbb::split) {
        total_cost = 0;
        points = rhs.points;
        i = rhs.i;
        type = rhs.type;
    }

    void operator()(const tbb::blocked_range<int> &range) {
        int begin = range.begin();
        int end = range.end();

        if (type) {
            double local_total = 0.0;
            for (int k = begin; k != end; k++)
                local_total += points->p[k].cost;
            total_cost += local_total;
        } else {
            for (int k = begin; k != end; k++) {
                float distance = dist(points->p[i], points->p[k], points->dim);
                if (i && distance * points->p[k].weight < points->p[k].cost) {
                    points->p[k].cost = distance * points->p[k].weight;
                    points->p[k].assign = i;
                }
            }
        }
    }

    void join(CenterOpen &lhs) {
        total_cost += lhs.getTotalCost();
    }
    double getTotalCost() {
        return total_cost;
    }
};

class CenterTableCount : public tbb::task {
private:
    Points *points;
    double *work_mem;
    int stride;
    int pid;

public:
    CenterTableCount(int id, int s, Points *p, double *mem) : pid(id), stride(s), points(p), work_mem(mem) {
    }

    task *execute() {
        int count = 0;
        long bsize = points->num / ((NUM_DIVISIONS));
        long k1 = bsize * pid;
        long k2 = k1 + bsize;

        if (pid == (NUM_DIVISIONS)-1)
            k2 = points->num;

        for (int i = k1; i < k2; i++) {
            if (is_center[i]) {
                center_table[i] = count++;
            }
        }

        work_mem[pid * stride] = count;

        return NULL;
    }
};

class CenterTableCountTask : public tbb::task {
    int is_continuation;
    Points *points;
    double *work_mem;
    int stride;

public:
    CenterTableCountTask(int s, Points *p, double *mem) : stride(s), points(p), work_mem(mem), is_continuation(0) {
    }

    task *execute() {
        tbb::task_list list;
        int p;

        if (!is_continuation) {
            recycle_as_continuation();
            set_ref_count(NUM_DIVISIONS);

            for (p = 1; p < (NUM_DIVISIONS); p++)
                list.push_back(*new (allocate_child()) CenterTableCount(p, stride, points, work_mem));
            CenterTableCount &me = *new (allocate_child()) CenterTableCount(0, stride, points, work_mem);
            spawn(list);
            is_continuation = 1;

            return &me;

        } else {

            int accum = 0;
            for (int p = 0; p < (NUM_DIVISIONS); p++) {
                int tmp = (int)work_mem[p * stride];
                work_mem[p * stride] = accum;
                accum += tmp;
            }

            return NULL;
        }
    }
};

class FixCenter : public tbb::task {
    Points *points;
    double *work_mem;
    int pid;
    int stride;

public:
    FixCenter(int id, int s, Points *p, double *mem) : pid(id), stride(s), points(p), work_mem(mem) {
    }
    task *execute() {
#ifdef SERIAL_FIXCENTER
        long k1 = 0;
        long k2 = points->num;
#else
        long bsize = points->num / ((NUM_DIVISIONS));
        long k1 = bsize * pid;
        long k2 = k1 + bsize;
        if (pid == (NUM_DIVISIONS)-1)
            k2 = points->num;
#endif

        for (int i = k1; i < k2; i++) {
            if (is_center[i]) {
                center_table[i] += (int)work_mem[pid * stride];
            }
        }

        return NULL;
    }
};

class FixCenterTask : public tbb::task {
    bool is_continuation;
    Points *points;
    double *work_mem;
    int stride;

public:
    FixCenterTask(int s, Points *p, double *mem) : stride(s), points(p), work_mem(mem), is_continuation(false) {
    }

    task *execute() {
        tbb::task_list list;
        int p;
        if (!is_continuation) {
            recycle_as_continuation();
            set_ref_count(NUM_DIVISIONS);
            for (p = 1; p < (NUM_DIVISIONS); p++)
                list.push_back(*new (allocate_child()) FixCenter(p, stride, points, work_mem));
            spawn(list);
            FixCenter &me = *new (allocate_child()) FixCenter(0, stride, points, work_mem);
            is_continuation = true;
            return &me;
        } else {

            return NULL;
        }
    }
};

class LowerCost : public tbb::task {
    Points *points;
    double *work_mem;
    long x;
    int K;
    int pid;
    int stride;

public:
    LowerCost(int id, int s, Points *p, long x_, double *mem, int k) : pid(id), stride(s), points(p), work_mem(mem), K(k), x(x_) {
    }
    task *execute() {

        double *lower = &work_mem[pid * stride];
        double local_cost_of_opening_x = 0;
        long bsize = points->num / ((NUM_DIVISIONS));
        long k1 = bsize * pid;
        long k2 = k1 + bsize;
        int i;

        if (pid == (NUM_DIVISIONS)-1)
            k2 = points->num;

        double *cost_of_opening_x = &work_mem[pid * stride + K + 1];

        for (i = k1; i < k2; i++) {
            float x_cost = dist(points->p[i], points->p[x], points->dim) * points->p[i].weight;
            float current_cost = points->p[i].cost;

            if (x_cost < current_cost) {

                switch_membership[i] = 1;
                local_cost_of_opening_x += x_cost - current_cost;

            } else {

                int assign = points->p[i].assign;
                lower[center_table[assign]] += current_cost - x_cost;
            }
        }

        *cost_of_opening_x = local_cost_of_opening_x;
        return NULL;
    }
};

class LowerCostTask : public tbb::task {
    bool is_continuation;
    Points *points;
    double *work_mem;
    int K;
    long x;
    int stride;

public:
    LowerCostTask(int s, Points *p, long x_, double *mem, int k) : stride(s), points(p), work_mem(mem), K(k), x(x_), is_continuation(false) {
    }

    task *execute() {
        tbb::task_list list;
        int p;
        if (!is_continuation) {
            recycle_as_continuation();
            set_ref_count(NUM_DIVISIONS);
            for (p = 1; p < (NUM_DIVISIONS); p++)
                list.push_back(*new (allocate_child()) LowerCost(p, stride, points, x, work_mem, K));
            spawn(list);
            LowerCost &me = *new (allocate_child()) LowerCost(0, stride, points, x, work_mem, K);
            is_continuation = true;
            return &me;
        } else {

            return NULL;
        }
    }
};

class CenterClose : public tbb::task {
    Points *points;
    double *work_mem;
    double *number_of_centers_to_close;
    double z;
    int pid, stride;
    int K;

public:
    CenterClose(int id, int s, Points *p, double *mem, int k, double z_) : pid(id), stride(s), points(p), work_mem(mem), K(k), z(z_) {
    }

    task *execute() {
        double *gl_lower = &work_mem[(NUM_DIVISIONS)*stride];
        double *cost_of_opening_x;
        int local_number_of_centers_to_close = 0;
        long bsize = points->num / ((NUM_DIVISIONS));
        long k1 = bsize * pid;
        long k2 = k1 + bsize;

        if (pid == (NUM_DIVISIONS)-1)
            k2 = points->num;

        number_of_centers_to_close = &work_mem[pid * stride + K];
        cost_of_opening_x = &work_mem[pid * stride + K + 1];

        for (int i = k1; i < k2; i++) {
            if (is_center[i]) {
                double low = z;

                for (int p = 0; p < (NUM_DIVISIONS); p++) {
                    low += work_mem[center_table[i] + p * stride];
                }
                gl_lower[center_table[i]] = low;
                if (low > 0) {

                    ++local_number_of_centers_to_close;
                    *cost_of_opening_x -= low;
                }
            }
        }
        *number_of_centers_to_close = (double)local_number_of_centers_to_close;
        return NULL;
    }
};

class CenterCloseTask : public tbb::task {
    bool is_continuation;
    Points *points;
    double *work_mem;
    int stride;
    double z;
    int K;

public:
    CenterCloseTask(int s, Points *p, double *mem, int k, double z_) : stride(s), points(p), work_mem(mem), K(k), z(z_), is_continuation(false) {
    }

    task *execute() {
        tbb::task_list list;
        int p;
        if (!is_continuation) {
            recycle_as_continuation();
            set_ref_count(NUM_DIVISIONS);
            for (p = 1; p < (NUM_DIVISIONS); p++)
                list.push_back(*new (allocate_child()) CenterClose(p, stride, points, work_mem, K, z));
            spawn(list);
            CenterClose &me = *new (allocate_child()) CenterClose(0, stride, points, work_mem, K, z);
            is_continuation = true;
            return &me;
        } else {

            return NULL;
        }
    }
};

class SaveMoney : public tbb::task {
    Points *points;
    double *work_mem;
    long x;
    int pid, stride;

public:
    SaveMoney(int id, int s, Points *p, long x_, double *mem) : pid(id), stride(s), points(p), x(x_), work_mem(mem) {
    }
    task *execute() {
        double *gl_lower = &work_mem[(NUM_DIVISIONS)*stride];
        long bsize = points->num / ((NUM_DIVISIONS));
        long k1 = bsize * pid;
        long k2 = k1 + bsize;
        int i;

        if (pid == (NUM_DIVISIONS)-1)
            k2 = points->num;

        for (int i = k1; i < k2; i++) {
            bool close_center = gl_lower[center_table[points->p[i].assign]] > 0;
            if (switch_membership[i] || close_center) {

                points->p[i].cost = points->p[i].weight * dist(points->p[i], points->p[x], points->dim);
                points->p[i].assign = x;
            }
        }
        for (int i = k1; i < k2; i++) {
            if (is_center[i] && gl_lower[center_table[i]] > 0) {
                is_center[i] = false;
            }
        }
        if (x >= k1 && x < k2) {

            is_center[x] = true;
        }

        return NULL;
    }
};

class SaveMoneyTask : public tbb::task {
    bool is_continuation;
    Points *points;
    long x;
    double *work_mem;
    int stride;

public:
    SaveMoneyTask(int s, Points *p, long x_, double *mem) : stride(s), points(p), x(x_), work_mem(mem), is_continuation(false) {
    }

    task *execute() {
        tbb::task_list list;
        int p;
        if (!is_continuation) {
            recycle_as_continuation();
            set_ref_count(NUM_DIVISIONS);
            for (p = 1; p < (NUM_DIVISIONS); p++)
                list.push_back(*new (allocate_child()) SaveMoney(p, stride, points, x, work_mem));
            spawn(list);
            SaveMoney &me = *new (allocate_child()) SaveMoney(0, stride, points, x, work_mem);
            is_continuation = true;
            return &me;
        } else {

            return NULL;
        }
    }
};

#endif

int isIdentical(float *i, float *j, int D)

{
    int a = 0;
    int equal = 1;

    while (equal && a < D) {
        if (i[a] != j[a]) {
            equal = 0;
        } else {
            a++;
        }
    }
    if (equal) {
        return 1;
    } else {
        return 0;
    }
}

static int floatcomp(const void *i, const void *j) {
    float a, b;
    a = *(float *)(i);
    b = *(float *)(j);
    if (a > b) {
        return (1);
    }
    if (a < b) {
        return (-1);
    }
    return (0);
}

void shuffle(Points *points) {
    long i, j;
    Point temp;
    for (i = 0; i < points->num - 1; i++) {
        j = (myrand() % (points->num - i)) + i;
        temp = points->p[i];
        points->p[i] = points->p[j];
        points->p[j] = temp;
    }
}

void intshuffle(int *intarray, int length) {
    long i, j;
    int temp;
    for (i = 0; i < length; i++) {
        j = (myrand() % (length - i)) + i;
        temp = intarray[i];
        intarray[i] = intarray[j];
        intarray[j] = temp;
    }
}

float dist(Point p1, Point p2, int dim) {
    int i;
    float result = 0.0;
    for (i = 0; i < dim; i++) {
        result += (p1.coord[i] - p2.coord[i]) * (p1.coord[i] - p2.coord[i]);
    }
    return (result);
}

#ifdef TBB_VERSION

float pspeedy(Points *points, float z, long *kcenter) {
    static double totalcost;
    static bool open = false;
    static double *costs;
    static int i;

    {
        int grain_size = points->num / ((NUM_DIVISIONS));
        CenterCreate c(points);
        tbb::parallel_for(tbb::blocked_range<int>(0, points->num, grain_size), c);
    }

    *kcenter = 1;

    {
        int grain_size = points->num / ((NUM_DIVISIONS));
        double acc_cost = 0.0;
        CenterOpen c(points);
        for (i = 1; i < points->num; i++) {
            bool to_open = ((float)myrand() / (float)INT_MAX) < (points->p[i].cost / z);
            if (to_open) {
                (*kcenter)++;
                c.i = i;

                tbb::parallel_reduce(tbb::blocked_range<int>(0, points->num, grain_size), c);
            }
        }

        c.type = 1;
        tbb::parallel_reduce(tbb::blocked_range<int>(0, points->num, grain_size), c);

        totalcost = z * (*kcenter);
        totalcost += c.getTotalCost();
    }
    return (totalcost);
}

#else

float pspeedy(Points *points, float z, long *kcenter, int pid, pthread_barrier_t *barrier) {
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    long bsize = points->num / nproc;
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        k2 = points->num;
    }

    static double totalcost;

    static bool open = false;
    static double *costs;
    static int i;

#ifdef ENABLE_THREADS
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#endif

    for (int k = k1; k < k2; k++) {
        float distance = dist(points->p[k], points->p[0], points->dim);
        points->p[k].cost = distance * points->p[k].weight;
        points->p[k].assign = 0;
    }

    if (pid == 0) {
        *kcenter = 1;
        costs = (double *)malloc(sizeof(double) * nproc);
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    if (pid != 0) {
        while (1) {
#ifdef ENABLE_THREADS
            pthread_mutex_lock(&mutex);
            while (!open) {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
#endif
            if (i >= points->num) {
                break;
            }
            for (int k = k1; k < k2; k++) {
                float distance = dist(points->p[i], points->p[k], points->dim);
                if (distance * points->p[k].weight < points->p[k].cost) {
                    points->p[k].cost = distance * points->p[k].weight;
                    points->p[k].assign = i;
                }
            }
#ifdef ENABLE_THREADS
            pthread_barrier_wait(barrier);
            pthread_barrier_wait(barrier);
#endif
        }
    } else {
        for (i = 1; i < points->num; i++) {
            bool to_open = ((float)myrand() / (float)INT_MAX) < (points->p[i].cost / z);
            if (to_open) {
                (*kcenter)++;
#ifdef ENABLE_THREADS
                pthread_mutex_lock(&mutex);
#endif
                open = true;
#ifdef ENABLE_THREADS
                pthread_mutex_unlock(&mutex);
                pthread_cond_broadcast(&cond);
#endif
                for (int k = k1; k < k2; k++) {
                    float distance = dist(points->p[i], points->p[k], points->dim);
                    if (distance * points->p[k].weight < points->p[k].cost) {
                        points->p[k].cost = distance * points->p[k].weight;
                        points->p[k].assign = i;
                    }
                }
#ifdef ENABLE_THREADS
                pthread_barrier_wait(barrier);
#endif
                open = false;
#ifdef ENABLE_THREADS
                pthread_barrier_wait(barrier);
#endif
            }
        }
#ifdef ENABLE_THREADS
        pthread_mutex_lock(&mutex);
#endif
        open = true;
#ifdef ENABLE_THREADS
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
#endif
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
    open = false;
    double mytotal = 0;
    for (int k = k1; k < k2; k++) {
        mytotal += points->p[k].cost;
    }
    costs[pid] = mytotal;
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    if (pid == 0) {
        totalcost = z * (*kcenter);
        for (int i = 0; i < nproc; i++) {
            totalcost += costs[i];
        }
        free(costs);
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    return (totalcost);
}

#endif

#ifdef TBB_VERSION
double pgain(long x, Points *points, double z, long int *numcenters) {
    int i;
    int number_of_centers_to_close = 0;

    static double *work_mem;
    static double gl_cost_of_opening_x;
    static int gl_number_of_centers_to_close;

    int stride = *numcenters + 2;

    int cl = CACHE_LINE / sizeof(double);
    if (stride % cl != 0) {
        stride = cl * (stride / cl + 1);
    }
    int K = stride - 2;

    double cost_of_opening_x = 0;

    work_mem = (double *)calloc(stride * ((NUM_DIVISIONS) + 1), sizeof(double));

    gl_cost_of_opening_x = 0;
    gl_number_of_centers_to_close = 0;

    {
        CenterTableCountTask &t = *new (tbb::task::allocate_root()) CenterTableCountTask(stride, points, work_mem);
        tbb::task::spawn_root_and_wait(t);
    }

    {
        FixCenterTask &t = *new (tbb::task::allocate_root()) FixCenterTask(stride, points, work_mem);
        tbb::task::spawn_root_and_wait(t);
    }

    memset(switch_membership, 0, points->num * sizeof(bool));
    memset(work_mem, 0, (NUM_DIVISIONS + 1) * stride * sizeof(double));

    {
        LowerCostTask &t = *new (tbb::task::allocate_root()) LowerCostTask(stride, points, x, work_mem, K);
        tbb::task::spawn_root_and_wait(t);
    }

    {
        CenterCloseTask &t = *new (tbb::task::allocate_root()) CenterCloseTask(stride, points, work_mem, K, z);
        tbb::task::spawn_root_and_wait(t);
    }

    gl_cost_of_opening_x = z;

    for (int p = 0; p < (NUM_DIVISIONS); p++) {
        gl_number_of_centers_to_close += (int)work_mem[p * stride + K];
        gl_cost_of_opening_x += work_mem[p * stride + K + 1];
    }

    if (gl_cost_of_opening_x < 0) {

        SaveMoneyTask &t = *new (tbb::task::allocate_root()) SaveMoneyTask(stride, points, x, work_mem);
        tbb::task::spawn_root_and_wait(t);

        *numcenters = *numcenters + 1 - gl_number_of_centers_to_close;
    } else {
        gl_cost_of_opening_x = 0;
    }

    free(work_mem);

    return -gl_cost_of_opening_x;
}

#else

double pgain(long x, Points *points, double z, long int *numcenters, int pid, pthread_barrier_t *barrier) {

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    long bsize = points->num / nproc;
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        k2 = points->num;
    }

    int i;
    int number_of_centers_to_close = 0;

    static double *work_mem;
    static double gl_cost_of_opening_x;
    static int gl_number_of_centers_to_close;

    int stride = *numcenters + 2;

    int cl = CACHE_LINE / sizeof(double);
    if (stride % cl != 0) {
        stride = cl * (stride / cl + 1);
    }
    int K = stride - 2;

    double cost_of_opening_x = 0;

    if (pid == 0) {
        work_mem = (double *)malloc(stride * (nproc + 1) * sizeof(double));
        gl_cost_of_opening_x = 0;
        gl_number_of_centers_to_close = 0;
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    int count = 0;
    for (int i = k1; i < k2; i++) {
        if (is_center[i]) {
            center_table[i] = count++;
        }
    }
    work_mem[pid * stride] = count;

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    if (pid == 0) {
        int accum = 0;
        for (int p = 0; p < nproc; p++) {
            int tmp = (int)work_mem[p * stride];
            work_mem[p * stride] = accum;
            accum += tmp;
        }
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    for (int i = k1; i < k2; i++) {
        if (is_center[i]) {
            center_table[i] += (int)work_mem[pid * stride];
        }
    }

    memset(switch_membership + k1, 0, (k2 - k1) * sizeof(bool));
    memset(work_mem + pid * stride, 0, stride * sizeof(double));
    if (pid == 0) {
        memset(work_mem + nproc * stride, 0, stride * sizeof(double));
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    double *lower = &work_mem[pid * stride];

    double *gl_lower = &work_mem[nproc * stride];

    for (i = k1; i < k2; i++) {
        float x_cost = dist(points->p[i], points->p[x], points->dim) * points->p[i].weight;
        float current_cost = points->p[i].cost;

        if (x_cost < current_cost) {

            switch_membership[i] = 1;
            cost_of_opening_x += x_cost - current_cost;

        } else {

            int assign = points->p[i].assign;
            lower[center_table[assign]] += current_cost - x_cost;
        }
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    for (int i = k1; i < k2; i++) {
        if (is_center[i]) {
            double low = z;

            for (int p = 0; p < nproc; p++) {
                low += work_mem[center_table[i] + p * stride];
            }
            gl_lower[center_table[i]] = low;
            if (low > 0) {

                ++number_of_centers_to_close;
                cost_of_opening_x -= low;
            }
        }
    }

    work_mem[pid * stride + K] = number_of_centers_to_close;
    work_mem[pid * stride + K + 1] = cost_of_opening_x;

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    if (pid == 0) {
        gl_cost_of_opening_x = z;

        for (int p = 0; p < nproc; p++) {
            gl_number_of_centers_to_close += (int)work_mem[p * stride + K];
            gl_cost_of_opening_x += work_mem[p * stride + K + 1];
        }
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    if (gl_cost_of_opening_x < 0) {

        for (int i = k1; i < k2; i++) {
            bool close_center = gl_lower[center_table[points->p[i].assign]] > 0;
            if (switch_membership[i] || close_center) {

                points->p[i].cost = points->p[i].weight * dist(points->p[i], points->p[x], points->dim);
                points->p[i].assign = x;
            }
        }
        for (int i = k1; i < k2; i++) {
            if (is_center[i] && gl_lower[center_table[i]] > 0) {
                is_center[i] = false;
            }
        }
        if (x >= k1 && x < k2) {
            is_center[x] = true;
        }

        if (pid == 0) {
            *numcenters = *numcenters + 1 - gl_number_of_centers_to_close;
        }
    } else {
        if (pid == 0) {
            gl_cost_of_opening_x = 0;
        }
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
    if (pid == 0) {
        free(work_mem);
    }

    return -gl_cost_of_opening_x;
}

#endif

#ifdef TBB_VERSION
float pFL(Points *points, int *feasible, int numfeasible, double z, long *k, double cost, long iter, double e) {

    long i;
    long x;
    double change;
    long numberOfPoints;

    change = cost;

    while (change / cost > 1.0 * e) {
        change = 0.0;
        numberOfPoints = points->num;

        intshuffle(feasible, numfeasible);

        for (i = 0; i < iter; i++) {
            x = i % numfeasible;

            change += pgain(feasible[x], points, z, k);
        }
        cost -= change;
    }

    return (cost);
}

#else
float pFL(Points *points, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e, int pid, pthread_barrier_t *barrier) {
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
    long i;
    long x;
    double change;
    long numberOfPoints;

    change = cost;

    while (change / cost > 1.0 * e) {
        change = 0.0;
        numberOfPoints = points->num;

        if (pid == 0) {
            intshuffle(feasible, numfeasible);
        }
#ifdef ENABLE_THREADS
        pthread_barrier_wait(barrier);
#endif
        for (i = 0; i < iter; i++) {
            x = i % numfeasible;
            change += pgain(feasible[x], points, z, k, pid, barrier);
        }
        cost -= change;
#ifdef ENABLE_THREADS
        pthread_barrier_wait(barrier);
#endif
    }
    return (cost);
}

#endif

#ifdef TBB_VERSION
int selectfeasible_fast(Points *points, int **feasible, int kmin)
#else
int selectfeasible_fast(Points *points, int **feasible, int kmin, int pid, pthread_barrier_t *barrier)
#endif
{
    int numfeasible = points->num;
    if (numfeasible > (ITER * kmin * log((double)kmin))) {
        numfeasible = (int)(ITER * kmin * log((double)kmin));
    }
    *feasible = (int *)malloc(numfeasible * sizeof(int));

    float *accumweight;
    float totalweight;

    long k1 = 0;
    long k2 = numfeasible;

    float w;
    int l, r, k;

    if (numfeasible == points->num) {
        for (int i = k1; i < k2; i++) {
            (*feasible)[i] = i;
        }
        return numfeasible;
    }
#ifdef TBB_VERSION
    accumweight = (float *)memoryFloat.allocate(sizeof(float) * points->num);
#else
    accumweight = (float *)malloc(sizeof(float) * points->num);
#endif

    accumweight[0] = points->p[0].weight;
    totalweight = 0;
    for (int i = 1; i < points->num; i++) {
        accumweight[i] = accumweight[i - 1] + points->p[i].weight;
    }
    totalweight = accumweight[points->num - 1];

    for (int i = k1; i < k2; i++) {
        w = (myrand() / (float)INT_MAX) * totalweight;

        l = 0;
        r = points->num - 1;
        if (accumweight[0] > w) {
            (*feasible)[i] = 0;
            continue;
        }
        while (l + 1 < r) {
            k = (l + r) / 2;
            if (accumweight[k] > w) {
                r = k;
            } else {
                l = k;
            }
        }
        (*feasible)[i] = r;
    }

#ifdef TBB_VERSION
    memoryFloat.deallocate(accumweight, sizeof(float));
#else
    free(accumweight);
#endif

    return numfeasible;
}

#ifdef TBB_VERSION

float pkmedian(Points *points, long kmin, long kmax, long *kfinal, int pid, pthread_barrier_t *barrier) {
    int i;
    double cost;
    double lastcost;
    double hiz, loz, z;

    static long k;
    static int *feasible;
    static int numfeasible;
    static double *hizs;

    hiz = loz = 0.0;
    long numberOfPoints = points->num;
    long ptDimension = points->dim;

    long bsize = points->num / nproc;
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    if (pid == nproc - 1)
        k2 = points->num;

    int grain_size = points->num / ((NUM_DIVISIONS));
    if (grain_size == 0) {

        for (long kk = 0; kk < points->num; kk++) {
            hiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
        }

    } else {
        HizReduction h(points);
        tbb::parallel_reduce(tbb::blocked_range<int>(0, points->num, grain_size), h);
        hiz = h.getHiz();
    }

    loz = 0.0;
    z = (hiz + loz) / 2.0;

    if (points->num <= kmax) {

        for (long kk = 0; kk < points->num; kk++) {
            points->p[kk].assign = kk;
            points->p[kk].cost = 0;
        }

        cost = 0;
        *kfinal = k;

        return cost;
    }

    shuffle(points);
    cost = pspeedy(points, z, &k);

    i = 0;

    while ((k < kmin) && (i < SP)) {
        cost = pspeedy(points, z, &k);
        i++;
    }

    while (k < kmin) {
        if (i >= SP) {
            hiz = z;
            z = (hiz + loz) / 2.0;
            i = 0;
        }

        shuffle(points);
        cost = pspeedy(points, z, &k);
        i++;
    }

    numfeasible = selectfeasible_fast(points, &feasible, kmin);
    for (int i = 0; i < points->num; i++) {

        is_center[points->p[i].assign] = true;
    }

    while (1) {

        lastcost = cost;
        cost = pFL(points, feasible, numfeasible, z, &k, cost, (long)(ITER * kmax * log((double)kmax)), 0.1);

        if (((k <= (1.1) * kmax) && (k >= (0.9) * kmin)) || ((k <= kmax + 2) && (k >= kmin - 2))) {

            cost = pFL(points, feasible, numfeasible, z, &k, cost, (long)(ITER * kmax * log((double)kmax)), 0.001);
        }

        if (k > kmax) {

            loz = z;
            z = (hiz + loz) / 2.0;
            cost += (z - loz) * k;
        }
        if (k < kmin) {

            hiz = z;
            z = (hiz + loz) / 2.0;
            cost += (z - hiz) * k;
        }

        if (((k <= kmax) && (k >= kmin)) || ((loz >= (0.999) * hiz))) {
            break;
        }
    }

    free(feasible);
    *kfinal = k;

    return cost;
}

#else

float pkmedian(Points *points, long kmin, long kmax, long *kfinal, int pid, pthread_barrier_t *barrier) {
    int i;
    double cost;
    double lastcost;
    double hiz, loz, z;

    static long k;
    static int *feasible;
    static int numfeasible;
    static double *hizs;

    if (pid == 0) {
        hizs = (double *)calloc(nproc, sizeof(double));
    }
    hiz = loz = 0.0;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + ((0x6e6f80d8) - (0xcc707d93));
    #endif
    long numberOfPoints = points->num;
    long ptDimension = points->dim;

    long bsize = points->num / nproc;
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        k2 = points->num;
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (rb_state12.var_1);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ (0x6e2de4a8);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) + (0xe8ed1660);
    #endif
    double myhiz = 0;
    for (long kk = k1; kk < k2; kk++) {
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_0);
        rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_1) : (0x91d600ec));
        rb_state5.var_1 = (rb_state5.var_1) + (0x1451d470);
        rb_state5.var_0 = (rb_state5.var_0) + (0xfd703c68);
        rb_state5.var_1 = (rb_state5.var_1) ^ ((0x517f8ea4) + (rb_state5.var_0));
        rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_1) == (0x5abe1f7c)) ? (rb_state5.var_0) : (0x980f2084));
        rb_state5.var_0 = (rb_state5.var_0) - (rb_state5.var_1);
        rb_state5.var_0 = (rb_state5.var_0) + ((0x954a55ed) ^ (rb_state5.var_1));
        if ((rb_state5.var_0) == (0x54bbcb5b)) {
            rb_state5.var_2 = rb_state5.var_1;
        }
        if ((rb_state5.var_0) == (0x3ba2274b)) {
            pthread_mutex_lock(&(rb_state5.lock_11));
            rb_state5.var_3 = 0x4654c4d2;
            rb_state5.var_4 = 0xe57cf29f;
            rb_state5.var_5 = 0xf5187bbe;
            rb_state5.var_6 = (rb_state5.var_6) + (((0x7b9ee81d) ^ (rb_state5.var_3)) ^ (0x93bf9632));
            rb_state5.var_7 = (rb_state5.var_7) - (((0x4ed68a1e) ^ (rb_state5.var_5)) + (0x61bd995a));
            rb_state5.var_8 = (rb_state5.var_8) ^ (rb_state5.var_4);
            rb_state5.var_9 = (rb_state5.var_9) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0x2d569abe));
            rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x2330993c));
            rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_10);
            pthread_mutex_unlock(&(rb_state5.lock_11));
        }
        if ((rb_state5.var_0) == (0x3ba2274b)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x1170848a));
        rb_state12.var_1 = (rb_state12.var_1) ^ ((0xc0dcb722) ^ (0xa8d5ff40));
        rb_state12.var_0 = (rb_state12.var_0) - (0xcb9d9067);
        rb_state12.var_2 = (rb_state12.var_2) ^ (13311 < rb_input_size ? (uint32_t)rb_input[13311] : 0x1d67fe8e);
        rb_state12.var_2 = (rb_state12.var_2) ^ (((rb_state12.var_1) == (0x68094862)) ? (rb_state12.var_2) : (0x9d8b5dad));
        rb_state12.var_2 = (rb_state12.var_2) - ((0x8824744d) - (rb_state12.var_0));
        rb_state12.var_1 = (rb_state12.var_1) - ((0xec11074f) ^ (0x67b05b8e));
        if ((rb_state12.var_2) == (0xc5d75b61)) {
            pthread_mutex_lock(&(rb_state12.lock_6));
            rb_state12.var_3 = rb_state12.var_0;
            pthread_mutex_unlock(&(rb_state12.lock_6));
        }
        if ((rb_state12.var_2) == (0xd4a366c)) {
            rb_state12.var_4 = 0xf31726be;
            rb_state12.var_5 = (rb_state12.var_5) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_3) : (0x5981ddd0));
            rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_5);
        }
        if ((rb_state12.var_2) == (0xd4a366c)) {
            pthread_mutex_lock(&(rb_state12.lock_6));
            if (!((rb_state12.var_0) == (rb_state12.var_3))) {
                racebench_trigger(12);
            }
            pthread_mutex_unlock(&(rb_state12.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (0x24561e06);
        rb_state14.var_1 = (rb_state14.var_1) + ((0xfe418802) + (0x85d25619));
        rb_state14.var_2 = (rb_state14.var_2) - (52275 < rb_input_size ? (uint32_t)rb_input[52275] : 0xc5f0c853);
        rb_state14.var_1 = (rb_state14.var_1) + (19495 < rb_input_size ? (uint32_t)rb_input[19495] : 0x582d8773);
        rb_state14.var_2 = (rb_state14.var_2) - (51073 < rb_input_size ? (uint32_t)rb_input[51073] : 0xc49e0d54);
        if ((rb_state14.var_1) == (0xdf9d7730)) {
            pthread_mutex_lock(&(rb_state14.lock_12));
            rb_state14.var_5 = 0x1;
            pthread_mutex_unlock(&(rb_state14.lock_12));
        }
        if ((rb_state14.var_1) == (0x63b15622)) {
            pthread_mutex_lock(&(rb_state14.lock_12));
            if ((rb_state14.var_5) != (0x0)) {
                rb_state14.var_13 = !((rb_state14.var_3) == (rb_state14.var_4));
            }
            pthread_mutex_unlock(&(rb_state14.lock_12));
        }
        if ((rb_state14.var_1) == (0x63b15622)) {
            pthread_mutex_lock(&(rb_state14.lock_12));
            rb_state14.var_6 = 0x7514258a;
            rb_state14.var_7 = (rb_state14.var_7) - (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_6) : (0x5e6c9734));
            rb_state14.var_8 = (rb_state14.var_8) - (rb_state14.var_3);
            rb_state14.var_9 = (rb_state14.var_9) - (((((0x8b99063a) ^ (rb_state14.var_6)) + (0x50b15d43)) - (rb_state14.var_7)) + (rb_state14.var_4));
            rb_state14.var_10 = (rb_state14.var_10) ^ (rb_state14.var_8);
            rb_state14.var_11 = (rb_state14.var_11) + (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_10) : (0x55f86dee));
            rb_state14.var_4 = (rb_state14.var_4) + (rb_state14.var_11);
            rb_state14.var_3 = rb_state14.var_4;
            pthread_mutex_unlock(&(rb_state14.lock_12));
        }
        rb_state14.var_7 = (rb_state14.var_7) ^ (40436 < rb_input_size ? (uint32_t)rb_input[40436] : 0x888893eb);
        rb_state14.var_8 = (rb_state14.var_8) + ((0x16e482db) - (0x242bc223));
        rb_state14.var_9 = (rb_state14.var_9) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_10) : (0x5a175241));
        rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_13);
        if ((rb_state14.var_3) == (0x55f86dee)) {
            rb_state14.var_15 = 0x29ccbd40;
            rb_state14.var_16 = 0xd91b218e;
            rb_state14.var_17 = 0xff445c00;
            rb_state14.var_18 = (rb_state14.var_18) ^ (0xb099e9f);
            rb_state14.var_19 = (rb_state14.var_19) ^ (((0x3a46bfcd) ^ (rb_state14.var_15)) - (0x8fe4218f));
            rb_state14.var_20 = (rb_state14.var_20) ^ (rb_state14.var_17);
            rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_16) : (0xbf37dcd7));
            rb_state14.var_22 = (rb_state14.var_22) - (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_18) : (0x1ef9fba9));
            rb_state14.var_23 = (rb_state14.var_23) ^ (rb_state14.var_19);
            rb_state14.var_24 = (rb_state14.var_24) ^ (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_21) : (0x7eb995c8));
            rb_state14.var_25 = (rb_state14.var_25) + (((((0xb8ae9bca) ^ (rb_state14.var_23)) - (rb_state14.var_22)) + (0xa68546b5)) ^ (rb_state14.var_16));
            rb_state14.var_26 = (rb_state14.var_26) + (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_24) : (0xba1c1e91));
            rb_state14.var_27 = (rb_state14.var_27) - (((0xc75970fb) ^ (rb_state14.var_18)) + (rb_state14.var_25));
            rb_state14.var_28 = (rb_state14.var_28) ^ (((rb_state14.var_26) == (0x0)) ? (rb_state14.var_27) : (0xdf48c3c0));
            rb_state14.var_8 = (rb_state14.var_8) + (rb_state14.var_28);
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) ^ (0xc09a0383);
        rb_state16.var_3 = (rb_state16.var_3) + (12019 < rb_input_size ? (uint32_t)rb_input[12019] : 0x5315dbbe);
        rb_state16.var_0 = (rb_state16.var_0) ^ ((0x891cd48a) - (rb_state16.var_1));
        rb_state16.var_2 = (rb_state16.var_2) + (53477 < rb_input_size ? (uint32_t)rb_input[53477] : 0xf0a2e0ca);
        rb_state16.var_3 = (rb_state16.var_3) - (0xf773a67e);
        rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_1);
        if ((rb_state16.var_2) == (0xc09b00c1)) {
            rb_state16.var_14 = !((rb_state16.var_5) == (rb_state16.var_4));
        }
        if ((rb_state16.var_2) == (0x103dc)) {
            pthread_mutex_lock(&(rb_state16.lock_13));
            rb_state16.var_6 = 0x2551e30e;
            rb_state16.var_7 = (rb_state16.var_7) - (((rb_state16.var_4) == (0x0)) ? (rb_state16.var_5) : (0x44466e9));
            rb_state16.var_8 = (rb_state16.var_8) - (((rb_state16.var_6) == (0x0)) ? (rb_state16.var_6) : (0xa2f21e50));
            rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_7);
            rb_state16.var_10 = (rb_state16.var_10) - (rb_state16.var_7);
            rb_state16.var_11 = (rb_state16.var_11) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0xec26d030));
            rb_state16.var_12 = (rb_state16.var_12) + (((((0x93ca9ab7) ^ (rb_state16.var_11)) ^ (rb_state16.var_10)) + (0xe896f9d9)) - (0xb4f1d0a));
            rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_12);
            rb_state16.var_5 = rb_state16.var_4;
            pthread_mutex_unlock(&(rb_state16.lock_13));
        }
        rb_state16.var_8 = (rb_state16.var_8) - ((0xd803e472) - (0x13ddc274));
        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_10) : (0x8a8d6d1e));
        rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_14);
        rb_state16.var_3 = (rb_state16.var_3) - (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_4) : (0x3e72aa35));
        if ((rb_state16.var_6) == (0x2551e30e)) {
            rb_state16.var_16 = 0xf297f4b0;
            rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_15) : (0x8e854db1));
            rb_state16.var_7 = (rb_state16.var_7) ^ (rb_state16.var_17);
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0x58c4fb4e));
        rb_state17.var_1 = (rb_state17.var_1) + ((0x17ea51ff) + (rb_state17.var_1));
        rb_state17.var_1 = (rb_state17.var_1) + (0xdbecde21);
        rb_state17.var_2 = (rb_state17.var_2) - ((0x380e345) - (0xca195df4));
        rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_2);
        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_2);
        rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_1) == (0xf3d73020)) ? (rb_state17.var_0) : (0x62bd1e));
        rb_state17.var_2 = (rb_state17.var_2) - (0xd53422b4);
        if ((rb_state17.var_3) == (0x66f98d58)) {
            pthread_mutex_lock(&(rb_state17.lock_18));
            rb_state17.var_4 = rb_state17.var_0;
            pthread_mutex_unlock(&(rb_state17.lock_18));
        }
        if ((rb_state17.var_3) == (0x6696d03a)) {
            rb_state17.var_5 = 0xb448ae89;
            rb_state17.var_6 = (rb_state17.var_6) ^ (0xf5488fdc);
            rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_4) : (0xbb1d7a07));
            rb_state17.var_8 = (rb_state17.var_8) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_7) : (0xf0f4ed3a));
            rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_5);
            rb_state17.var_10 = (rb_state17.var_10) + (rb_state17.var_6);
            rb_state17.var_11 = (rb_state17.var_11) + (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_7) : (0x89a3398f));
            rb_state17.var_12 = (rb_state17.var_12) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_9) : (0x854dc0c6));
            rb_state17.var_13 = (rb_state17.var_13) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0xd54af1a3));
            rb_state17.var_14 = (rb_state17.var_14) - (((0x60045844) ^ (rb_state17.var_12)) ^ (rb_state17.var_10));
            rb_state17.var_15 = (rb_state17.var_15) ^ (rb_state17.var_13);
            rb_state17.var_16 = (rb_state17.var_16) ^ (rb_state17.var_14);
            rb_state17.var_17 = (rb_state17.var_17) ^ (((((0xa2444fba) ^ (0xaf610831)) ^ (rb_state17.var_15)) - (rb_state17.var_11)) ^ (rb_state17.var_16));
            rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_17);
        }
        if ((rb_state17.var_3) == (0x6696d03a)) {
            pthread_mutex_lock(&(rb_state17.lock_18));
            if (!((rb_state17.var_0) == (rb_state17.var_4))) {
                racebench_trigger(17);
            }
            pthread_mutex_unlock(&(rb_state17.lock_18));
        }
        #endif
        myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    hizs[pid] = myhiz;

#ifdef ENABLE_THREADS
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_3) == (0x55f86dee)) {
        pthread_mutex_lock(&(rb_state14.lock_29));
        rb_state14.var_14 = rb_state14.var_8;
        pthread_mutex_unlock(&(rb_state14.lock_29));
    }
    if ((rb_state14.var_3) == (0x55f86dee)) {
        if (!((rb_state14.var_8) == (rb_state14.var_14))) {
            racebench_trigger(14);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_6) == (0x2551e30e)) {
        rb_state16.var_15 = rb_state16.var_7;
    }
    if ((rb_state16.var_6) == (0x2551e30e)) {
        if (!((rb_state16.var_7) == (rb_state16.var_15))) {
            racebench_trigger(16);
        }
    }
    #endif
    pthread_barrier_wait(barrier);
#endif

    for (int i = 0; i < nproc; i++) {
        hiz += hizs[i];
    }

    loz = 0.0;
    z = (hiz + loz) / 2.0;

    if (points->num <= kmax) {

        for (long kk = k1; kk < k2; kk++) {
            points->p[kk].assign = kk;
            points->p[kk].cost = 0;
        }
        cost = 0;
        if (pid == 0) {
            free(hizs);
            *kfinal = k;
        }
        return cost;
    }

    if (pid == 0) {
        shuffle(points);
    }
    cost = pspeedy(points, z, &k, pid, barrier);

    i = 0;

    while ((k < kmin) && (i < SP)) {
        cost = pspeedy(points, z, &k, pid, barrier);
        i++;
    }

    while (k < kmin) {
        if (i >= SP) {
            hiz = z;
            z = (hiz + loz) / 2.0;
            i = 0;
        }
        if (pid == 0) {
            shuffle(points);
        }
        cost = pspeedy(points, z, &k, pid, barrier);
        i++;
    }

    if (pid == 0) {
        numfeasible = selectfeasible_fast(points, &feasible, kmin, pid, barrier);
        for (int i = 0; i < points->num; i++) {
            is_center[points->p[i].assign] = true;
        }
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    while (1) {

        lastcost = cost;
        cost = pFL(points, feasible, numfeasible, z, &k, cost, (long)(ITER * kmax * log((double)kmax)), 0.1, pid, barrier);

        if (((k <= (1.1) * kmax) && (k >= (0.9) * kmin)) || ((k <= kmax + 2) && (k >= kmin - 2))) {

            cost = pFL(points, feasible, numfeasible, z, &k, cost, (long)(ITER * kmax * log((double)kmax)), 0.001, pid, barrier);
        }

        if (k > kmax) {

            loz = z;
            z = (hiz + loz) / 2.0;
            cost += (z - loz) * k;
        }
        if (k < kmin) {

            hiz = z;
            z = (hiz + loz) / 2.0;
            cost += (z - hiz) * k;
        }

        if (((k <= kmax) && (k >= kmin)) || ((loz >= (0.999) * hiz))) {
            break;
        }
#ifdef ENABLE_THREADS
        pthread_barrier_wait(barrier);
#endif
    }

    if (pid == 0) {
        free(feasible);
        free(hizs);
        *kfinal = k;
    }

    return cost;
}

#endif

int contcenters(Points *points) {
    long i, ii;
    float relweight;

    for (i = 0; i < points->num; i++) {

        if (points->p[i].assign != i) {
            relweight = points->p[points->p[i].assign].weight + points->p[i].weight;
            relweight = points->p[i].weight / relweight;
            for (ii = 0; ii < points->dim; ii++) {
                points->p[points->p[i].assign].coord[ii] *= 1.0 - relweight;
                points->p[points->p[i].assign].coord[ii] += points->p[i].coord[ii] * relweight;
            }
            points->p[points->p[i].assign].weight += points->p[i].weight;
        }
    }

    return 0;
}

void copycenters(Points *points, Points *centers, long *centerIDs, long offset) {
    long i;
    long k;

    bool *is_a_median = (bool *)calloc(points->num, sizeof(bool));

    for (i = 0; i < points->num; i++) {
        is_a_median[points->p[i].assign] = 1;
    }

    k = centers->num;

    for (i = 0; i < points->num; i++) {
        if (is_a_median[i]) {
            memcpy(centers->p[k].coord, points->p[i].coord, points->dim * sizeof(float));
            centers->p[k].weight = points->p[i].weight;
            centerIDs[k] = i + offset;
            k++;
        }
    }

    centers->num = k;

    free(is_a_median);
}

struct pkmedian_arg_t {
    Points *points;
    long kmin;
    long kmax;
    long *kfinal;
    int pid;
    pthread_barrier_t *barrier;
};

void *localSearchSub(void *arg_) {

    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_1) == (0x0)) {
        rb_state0.var_10 = 0xc042699;
        rb_state0.var_11 = 0xe36d825c;
        rb_state0.var_12 = (rb_state0.var_12) - (rb_state0.var_8);
        rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_9) : (0x5804c714));
        rb_state0.var_14 = (rb_state0.var_14) - (rb_state0.var_11);
        rb_state0.var_15 = (rb_state0.var_15) + (((0xa572f4cb) - (rb_state0.var_10)) ^ (0xfa9c4e9b));
        rb_state0.var_16 = (rb_state0.var_16) - (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_12) : (0xf3367057));
        rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_13);
        rb_state0.var_18 = (rb_state0.var_18) + (rb_state0.var_14);
        rb_state0.var_19 = (rb_state0.var_19) - (((rb_state0.var_12) == (0x0)) ? (rb_state0.var_15) : (0x7eda47c8));
        rb_state0.var_20 = (rb_state0.var_20) - (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_16) : (0x35a23b94));
        rb_state0.var_21 = (rb_state0.var_21) + (((((0x9f2057e0) ^ (rb_state0.var_17)) - (rb_state0.var_15)) + (rb_state0.var_14)) + (rb_state0.var_18));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_20) : (0x1be429d6));
        rb_state0.var_23 = (rb_state0.var_23) ^ (((rb_state0.var_16) == (0x0)) ? (rb_state0.var_21) : (0x68a128ae));
        rb_state0.var_24 = (rb_state0.var_24) ^ (((((0x5d69161d) + (rb_state0.var_22)) - (0xed82ce24)) ^ (0xea6b37c4)) + (rb_state0.var_23));
        rb_state0.var_4 = (rb_state0.var_4) ^ (rb_state0.var_24);
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - (0xb9db91a6);
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_3 = (rb_state2.var_3) - (20375 < rb_input_size ? (uint32_t)rb_input[20375] : 0xa6a2497b);
    if ((rb_state2.var_2) == (0x0)) {
        rb_state2.var_16 = 0x4277f65;
        rb_state2.var_17 = (rb_state2.var_17) ^ (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_16) : (0xb7ce76f4));
        rb_state2.var_18 = (rb_state2.var_18) + (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_17) : (0x5c328a95));
        rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_3 = (rb_state3.var_3) ^ (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ (52463 < rb_input_size ? (uint32_t)rb_input[52463] : 0x9f0fe314);
    if ((rb_state4.var_2) == (0x0)) {
        if ((rb_state4.var_6) != (0x0)) {
            rb_state4.var_19 = !((rb_state4.var_4) == (rb_state4.var_5));
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0x2824774b)) {
        rb_state6.var_15 = 0xfc8b1bd2;
        rb_state6.var_16 = 0x88b603d9;
        rb_state6.var_17 = (rb_state6.var_17) ^ (0x733697c9);
        rb_state6.var_18 = (rb_state6.var_18) + (((((0x5d4874ca) + (rb_state6.var_11)) ^ (rb_state6.var_16)) - (0x27f191e2)) ^ (rb_state6.var_10));
        rb_state6.var_19 = (rb_state6.var_19) + (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_15) : (0xc25c29da));
        rb_state6.var_20 = (rb_state6.var_20) + (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_18) : (0x827fa45b));
        rb_state6.var_21 = (rb_state6.var_21) - (((((0xa3c365b5) + (rb_state6.var_13)) ^ (0xcb28742b)) ^ (rb_state6.var_20)) ^ (rb_state6.var_19));
        rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_21);
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (0x996f4e66);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + ((0xf3a35a33) + (rb_state10.var_0));
    if ((rb_state10.var_3) == (0x4d51703d)) {
        rb_state10.var_4 = rb_state10.var_1;
    }
    if ((rb_state10.var_3) == (0x4d51703d)) {
        if (!((rb_state10.var_1) == (rb_state10.var_4))) {
            racebench_trigger(10);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_35));
        rb_state11.var_13 = 0x3baf6d69;
        rb_state11.var_14 = 0xe386bc53;
        rb_state11.var_15 = (rb_state11.var_15) + (((((0xe64d1c77) - (rb_state11.var_13)) + (rb_state11.var_9)) ^ (rb_state11.var_14)) ^ (0x3ae53cc5));
        rb_state11.var_16 = (rb_state11.var_16) ^ (rb_state11.var_10);
        rb_state11.var_17 = (rb_state11.var_17) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_8) : (0xce39b7a8));
        rb_state11.var_18 = (rb_state11.var_18) ^ (((0x9de02d78) - (rb_state11.var_15)) - (rb_state11.var_12));
        rb_state11.var_19 = (rb_state11.var_19) + (((0xa26884c) + (0x5abb667d)) + (rb_state11.var_16));
        rb_state11.var_20 = (rb_state11.var_20) + (rb_state11.var_17);
        rb_state11.var_21 = (rb_state11.var_21) - (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x4c026a87));
        rb_state11.var_22 = (rb_state11.var_22) + (((0x74bc94d6) ^ (0x9ae3ca5a)) + (rb_state11.var_20));
        rb_state11.var_23 = (rb_state11.var_23) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_21) : (0x87eac369));
        rb_state11.var_24 = (rb_state11.var_24) ^ (((((0x1b2b0b50) + (rb_state11.var_14)) + (rb_state11.var_22)) - (rb_state11.var_15)) + (rb_state11.var_23));
        rb_state11.var_12 = (rb_state11.var_12) ^ (rb_state11.var_24);
        rb_state11.var_4 = rb_state11.var_12;
        pthread_mutex_unlock(&(rb_state11.lock_35));
    }
    if ((rb_state11.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state11.lock_35));
        rb_state11.var_26 = 0xae72bf53;
        rb_state11.var_27 = (rb_state11.var_27) ^ (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_26) : (0xcdd17592));
        rb_state11.var_28 = (rb_state11.var_28) ^ ((0x9a4eb4ea) + (rb_state11.var_18));
        rb_state11.var_29 = (rb_state11.var_29) + (rb_state11.var_19);
        rb_state11.var_30 = (rb_state11.var_30) + (((0xf7465493) - (rb_state11.var_17)) ^ (rb_state11.var_20));
        rb_state11.var_31 = (rb_state11.var_31) + (((((0xa2258e4e) + (rb_state11.var_28)) ^ (rb_state11.var_21)) ^ (0xa42d8fbd)) + (rb_state11.var_27));
        rb_state11.var_32 = (rb_state11.var_32) ^ (((rb_state11.var_22) == (0x0)) ? (rb_state11.var_29) : (0x10025806));
        rb_state11.var_33 = (rb_state11.var_33) + (((((0x5fcfd4d9) - (0x73cfdc2f)) - (rb_state11.var_30)) - (rb_state11.var_31)) - (0x13132b99));
        rb_state11.var_34 = (rb_state11.var_34) - (((((0x772892e8) + (rb_state11.var_33)) + (rb_state11.var_32)) + (rb_state11.var_24)) + (rb_state11.var_23));
        rb_state11.var_25 = (rb_state11.var_25) ^ (rb_state11.var_34);
        rb_state11.var_4 = rb_state11.var_25;
        pthread_mutex_unlock(&(rb_state11.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x0)) {
        rb_state13.var_3 = rb_state13.var_0;
    }
    if ((rb_state13.var_2) == (0x0)) {
        if (!((rb_state13.var_0) == (rb_state13.var_3))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_3 = (rb_state18.var_3) + (0x5d7f4b71);
    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_2) == (0xffffff84)) ? (rb_state18.var_1) : (0xf729d710));
    rb_state18.var_3 = (rb_state18.var_3) ^ (0xba2c554f);
    rb_state18.var_0 = (rb_state18.var_0) + ((0x25f8d2f5) + (rb_state18.var_1));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (rb_state19.var_2);
    rb_state19.var_3 = (rb_state19.var_3) ^ (rb_state19.var_3);
    #endif
    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_2) : (0xbe302b15));
    rb_state0.var_2 = (rb_state0.var_2) + (0xf4244a57);
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_1);
    if ((rb_state1.var_0) == (0x8c48dcb4)) {
        rb_state1.var_9 = 0x4137701b;
        rb_state1.var_10 = 0x10bad38c;
        rb_state1.var_11 = (rb_state1.var_11) ^ (rb_state1.var_9);
        rb_state1.var_12 = (rb_state1.var_12) - (40753 < rb_input_size ? (uint32_t)rb_input[40753] : 0x18f2624b);
        rb_state1.var_13 = (rb_state1.var_13) ^ (((((0x2a42dfd9) + (rb_state1.var_6)) ^ (rb_state1.var_10)) - (rb_state1.var_5)) + (0xf0907846));
        rb_state1.var_14 = (rb_state1.var_14) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_12) : (0x6847976f));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((((0x28ed2cec) - (rb_state1.var_14)) + (rb_state1.var_13)) - (rb_state1.var_7)) + (0x73517e6b));
        rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_15);
    }
    if ((rb_state1.var_9) == (0x4137701b)) {
        rb_state1.var_37 = 0x16e17396;
        rb_state1.var_38 = (rb_state1.var_38) + (((0x5849c5db) ^ (rb_state1.var_30)) + (rb_state1.var_32));
        rb_state1.var_39 = (rb_state1.var_39) - (((0xa27f7dde) ^ (rb_state1.var_37)) - (rb_state1.var_33));
        rb_state1.var_40 = (rb_state1.var_40) ^ (((rb_state1.var_31) == (0x0)) ? (rb_state1.var_38) : (0x3779f5cb));
        rb_state1.var_41 = (rb_state1.var_41) - (((rb_state1.var_34) == (0x0)) ? (rb_state1.var_39) : (0xf619c074));
        rb_state1.var_42 = (rb_state1.var_42) + (((rb_state1.var_35) == (0x0)) ? (rb_state1.var_40) : (0x3a14ef3));
        rb_state1.var_43 = (rb_state1.var_43) + (((rb_state1.var_36) == (0x0)) ? (rb_state1.var_41) : (0x4b3397c2));
        rb_state1.var_44 = (rb_state1.var_44) + (((rb_state1.var_42) == (0x0)) ? (rb_state1.var_43) : (0x4fde5023));
        rb_state1.var_18 = (rb_state1.var_18) + (rb_state1.var_44);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_37));
        rb_state3.var_11 = 0x7394c553;
        rb_state3.var_12 = (rb_state3.var_12) - (40907 < rb_input_size ? (uint32_t)rb_input[40907] : 0x3815f90f);
        rb_state3.var_13 = (rb_state3.var_13) + ((0xf14f8747) ^ (rb_state3.var_10));
        rb_state3.var_14 = (rb_state3.var_14) ^ (rb_state3.var_8);
        rb_state3.var_15 = (rb_state3.var_15) + (((0x8d33211) - (rb_state3.var_11)) ^ (rb_state3.var_11));
        rb_state3.var_16 = (rb_state3.var_16) - (((((0x5fa84ac1) ^ (rb_state3.var_12)) + (rb_state3.var_9)) - (rb_state3.var_12)) ^ (rb_state3.var_13));
        rb_state3.var_17 = (rb_state3.var_17) + (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_14) : (0x7e0db5aa));
        rb_state3.var_18 = (rb_state3.var_18) ^ (((((0x808b8eb) + (rb_state3.var_14)) - (rb_state3.var_16)) ^ (0x2f7fb015)) ^ (rb_state3.var_15));
        rb_state3.var_19 = (rb_state3.var_19) - (((0x1b97ffbd) + (rb_state3.var_17)) - (0xaed7031c));
        rb_state3.var_20 = (rb_state3.var_20) + (((0x8f822534) - (0x78a1a0d)) ^ (rb_state3.var_18));
        rb_state3.var_21 = (rb_state3.var_21) ^ (rb_state3.var_19);
        rb_state3.var_22 = (rb_state3.var_22) - (((rb_state3.var_15) == (0x0)) ? (rb_state3.var_20) : (0xeca70014));
        rb_state3.var_23 = (rb_state3.var_23) - (rb_state3.var_21);
        rb_state3.var_24 = (rb_state3.var_24) + (((0xd2879895) - (rb_state3.var_22)) - (0x921d2323));
        rb_state3.var_25 = (rb_state3.var_25) - (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_23) : (0xad0f004a));
        rb_state3.var_26 = (rb_state3.var_26) ^ (((0x7b346352) ^ (rb_state3.var_24)) ^ (0x4f634c86));
        rb_state3.var_27 = (rb_state3.var_27) ^ (((rb_state3.var_25) == (0x0)) ? (rb_state3.var_26) : (0x5f34a551));
        rb_state3.var_10 = (rb_state3.var_10) ^ (rb_state3.var_27);
        rb_state3.var_4 = rb_state3.var_10;
        pthread_mutex_unlock(&(rb_state3.lock_37));
    }
    if ((rb_state3.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state3.lock_37));
        rb_state3.var_29 = 0xfc16f904;
        rb_state3.var_30 = 0x3e6c5294;
        rb_state3.var_31 = (rb_state3.var_31) - ((0x5c9b5096) - (0x2bfcbc52));
        rb_state3.var_32 = (rb_state3.var_32) + (((((0xd5877940) + (rb_state3.var_17)) - (rb_state3.var_19)) + (rb_state3.var_18)) + (rb_state3.var_30));
        rb_state3.var_33 = (rb_state3.var_33) - (rb_state3.var_29);
        rb_state3.var_34 = (rb_state3.var_34) - (((0xb6f8d15d) + (rb_state3.var_31)) + (0xd8a71493));
        rb_state3.var_35 = (rb_state3.var_35) + (((((0x146800) ^ (rb_state3.var_20)) + (rb_state3.var_32)) - (rb_state3.var_33)) ^ (0xa94ad570));
        rb_state3.var_36 = (rb_state3.var_36) - (((rb_state3.var_34) == (0x0)) ? (rb_state3.var_35) : (0xb4e08dac));
        rb_state3.var_28 = (rb_state3.var_28) - (rb_state3.var_36);
        rb_state3.var_4 = rb_state3.var_28;
        pthread_mutex_unlock(&(rb_state3.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_2 = (rb_state4.var_2) + (rb_state4.var_2);
    rb_state4.var_3 = (rb_state4.var_3) + (0xc0dfa3cb);
    rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_11);
    rb_state4.var_12 = (rb_state4.var_12) + (((rb_state4.var_14) == (0x0)) ? (rb_state4.var_13) : (0x499aee7e));
    rb_state4.var_17 = (rb_state4.var_17) - ((0x6584f086) - (0xf7c645c));
    if ((rb_state4.var_19) == (0x0)) {
        rb_state4.var_27 = 0xab432e54;
        rb_state4.var_28 = 0x2e143b5e;
        rb_state4.var_29 = (rb_state4.var_29) + (0x75e67580);
        rb_state4.var_30 = (rb_state4.var_30) ^ (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_21) : (0xfdeef1fc));
        rb_state4.var_31 = (rb_state4.var_31) + (((0xe9b40ea9) - (rb_state4.var_27)) - (rb_state4.var_22));
        rb_state4.var_32 = (rb_state4.var_32) ^ (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_29) : (0x444105ad));
        rb_state4.var_33 = (rb_state4.var_33) - (rb_state4.var_30);
        rb_state4.var_34 = (rb_state4.var_34) ^ (((0x16c2f6b1) - (rb_state4.var_31)) - (0x4e204ad6));
        rb_state4.var_35 = (rb_state4.var_35) ^ (rb_state4.var_32);
        rb_state4.var_36 = (rb_state4.var_36) ^ (rb_state4.var_33);
        rb_state4.var_37 = (rb_state4.var_37) - (((0x30d5da0c) + (rb_state4.var_34)) - (rb_state4.var_24));
        rb_state4.var_38 = (rb_state4.var_38) ^ (rb_state4.var_35);
        rb_state4.var_39 = (rb_state4.var_39) - (rb_state4.var_36);
        rb_state4.var_40 = (rb_state4.var_40) ^ (((0x772e1ecd) - (rb_state4.var_25)) - (rb_state4.var_37));
        rb_state4.var_41 = (rb_state4.var_41) - (((0x855f97b6) + (0x4b06f774)) ^ (rb_state4.var_38));
        rb_state4.var_42 = (rb_state4.var_42) - (((((0xc25d3832) ^ (rb_state4.var_40)) - (0x9bd7a5fd)) + (rb_state4.var_39)) - (rb_state4.var_26));
        rb_state4.var_43 = (rb_state4.var_43) - (((rb_state4.var_41) == (0x0)) ? (rb_state4.var_42) : (0x5ee62538));
        rb_state4.var_26 = (rb_state4.var_26) ^ (rb_state4.var_43);
        rb_state4.var_20 = rb_state4.var_26;
    }
    if ((rb_state4.var_19) == (0x0)) {
        pthread_mutex_lock(&(rb_state4.lock_56));
        rb_state4.var_45 = 0x78d6e49b;
        rb_state4.var_46 = (rb_state4.var_46) - (rb_state4.var_29);
        rb_state4.var_47 = (rb_state4.var_47) ^ (rb_state4.var_28);
        rb_state4.var_48 = (rb_state4.var_48) ^ (((rb_state4.var_30) == (0x0)) ? (rb_state4.var_45) : (0x102b146d));
        rb_state4.var_49 = (rb_state4.var_49) ^ (rb_state4.var_27);
        rb_state4.var_50 = (rb_state4.var_50) + (rb_state4.var_46);
        rb_state4.var_51 = (rb_state4.var_51) ^ (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_47) : (0x89524676));
        rb_state4.var_52 = (rb_state4.var_52) - (((rb_state4.var_48) == (0x0)) ? (rb_state4.var_49) : (0x698023a3));
        rb_state4.var_53 = (rb_state4.var_53) ^ (((((0x92504b3) + (rb_state4.var_50)) + (rb_state4.var_51)) + (0xbfa5e950)) ^ (0x736a90ff));
        rb_state4.var_54 = (rb_state4.var_54) + (((0x4408d60f) - (rb_state4.var_32)) + (rb_state4.var_52));
        rb_state4.var_55 = (rb_state4.var_55) + (((((0xda0346ed) + (rb_state4.var_33)) + (0xd9652e1f)) + (rb_state4.var_54)) ^ (rb_state4.var_53));
        rb_state4.var_44 = (rb_state4.var_44) - (rb_state4.var_55);
        rb_state4.var_20 = rb_state4.var_44;
        pthread_mutex_unlock(&(rb_state4.lock_56));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (64359 < rb_input_size ? (uint32_t)rb_input[64359] : 0xe5b8411a);
    rb_state8.var_1 = (rb_state8.var_1) - (13757 < rb_input_size ? (uint32_t)rb_input[13757] : 0xf6f8e0de);
    if ((rb_state8.var_2) == (0x2bddf626)) {
        rb_state8.var_16 = (rb_state8.var_16) ^ (rb_state8.var_12);
        rb_state8.var_17 = (rb_state8.var_17) + (0x7fc743ac);
        rb_state8.var_18 = (rb_state8.var_18) ^ (((((0xd9b13f0c) ^ (0x5c60b69a)) ^ (rb_state8.var_11)) + (rb_state8.var_10)) - (rb_state8.var_13));
        rb_state8.var_19 = (rb_state8.var_19) + (((((0xcd5bf2ca) ^ (rb_state8.var_16)) + (rb_state8.var_17)) - (rb_state8.var_14)) ^ (rb_state8.var_15));
        rb_state8.var_20 = (rb_state8.var_20) ^ (((((0x267f42c4) ^ (rb_state8.var_18)) ^ (rb_state8.var_19)) + (0x6a201a98)) ^ (rb_state8.var_16));
        rb_state8.var_4 = (rb_state8.var_4) ^ (rb_state8.var_20);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_1) : (0x130bd79a));
    if ((rb_state9.var_3) == (0xb2)) {
        rb_state9.var_5 = 0x29b49046;
        rb_state9.var_6 = 0x8eb78140;
        rb_state9.var_7 = 0xbc829fab;
        rb_state9.var_8 = (rb_state9.var_8) ^ (((((0xe4d95b6) + (rb_state9.var_5)) ^ (rb_state9.var_7)) + (rb_state9.var_4)) - (0x766db1e6));
        rb_state9.var_9 = (rb_state9.var_9) + (((((0x4f17fe92) ^ (rb_state9.var_8)) ^ (0xa91f2de4)) + (rb_state9.var_6)) - (rb_state9.var_5));
        rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_9);
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_1) : (0x4e4f552d));
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x62b10200));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0x0)) {
        rb_state15.var_5 = 0xb0e52480;
        rb_state15.var_6 = 0xa936fce9;
        rb_state15.var_7 = 0x2a126412;
        rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_6) : (0x4d2a239d));
        rb_state15.var_9 = (rb_state15.var_9) ^ (((((0x98eda65c) + (rb_state15.var_6)) + (rb_state15.var_5)) + (rb_state15.var_7)) + (rb_state15.var_5));
        rb_state15.var_10 = (rb_state15.var_10) - (((0xa3c0a9b7) - (0xfce1d685)) - (rb_state15.var_8));
        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_9) : (0xf01e4d73));
        rb_state15.var_12 = (rb_state15.var_12) - (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0x787c84e6));
        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_12);
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x3c99abba)) {
        rb_state18.var_22 = 0xaef4d67a;
        rb_state18.var_23 = 0xfb889a8e;
        rb_state18.var_24 = (rb_state18.var_24) + (((rb_state18.var_18) == (0x8a2ec57c)) ? (rb_state18.var_17) : (0xf3db256c));
        rb_state18.var_25 = (rb_state18.var_25) + (rb_state18.var_19);
        rb_state18.var_26 = (rb_state18.var_26) ^ (((rb_state18.var_23) == (0xfb889a8e)) ? (rb_state18.var_16) : (0x5c6bac89));
        rb_state18.var_27 = (rb_state18.var_27) - (((((0xc67c533e) - (0x196ddbe1)) + (rb_state18.var_24)) + (0x9657e0e9)) - (rb_state18.var_22));
        rb_state18.var_28 = (rb_state18.var_28) ^ (rb_state18.var_25);
        rb_state18.var_29 = (rb_state18.var_29) ^ (((0x7789bc8) + (rb_state18.var_26)) - (rb_state18.var_20));
        rb_state18.var_30 = (rb_state18.var_30) + (((((0xbf3855f4) + (0xfe3c6cc9)) ^ (rb_state18.var_27)) + (rb_state18.var_21)) ^ (rb_state18.var_28));
        rb_state18.var_31 = (rb_state18.var_31) + (((((0xc34fbfe2) - (rb_state18.var_29)) - (rb_state18.var_22)) ^ (0x693d94e4)) - (rb_state18.var_30));
        rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_31);
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state19.lock_9));
        rb_state19.var_5 = 0x14803370;
        rb_state19.var_6 = 0x7264a483;
        rb_state19.var_7 = (rb_state19.var_7) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_5) : (0x6aad9606));
        rb_state19.var_8 = (rb_state19.var_8) - (((((0x11e20438) ^ (rb_state19.var_6)) + (rb_state19.var_7)) + (rb_state19.var_6)) ^ (rb_state19.var_5));
        rb_state19.var_2 = (rb_state19.var_2) + (rb_state19.var_8);
        pthread_mutex_unlock(&(rb_state19.lock_9));
    }
    #endif
    pkmedian(arg->points, arg->kmin, arg->kmax, arg->kfinal, arg->pid, arg->barrier);

    return NULL;
}

#ifdef TBB_VERSION
void localSearch(Points *points, long kmin, long kmax, long *kfinal) {
    pkmedian_arg_t arg;
    arg.points = points;
    arg.kmin = kmin;
    arg.kmax = kmax;
    arg.pid = 0;
    arg.kfinal = kfinal;
    localSearchSub(&arg);
}
#else

void localSearch(Points *points, long kmin, long kmax, long *kfinal) {
    pthread_barrier_t barrier;
    pthread_t *threads = new pthread_t[nproc];
    pkmedian_arg_t *arg = new pkmedian_arg_t[nproc];

#ifdef ENABLE_THREADS
    pthread_barrier_init(&barrier, NULL, nproc);
#endif
    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) + (0x4f70457b);
        if ((rb_state0.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state0.lock_25));
            rb_state0.var_6 = (rb_state0.var_6) ^ (((0xae035250) + (rb_state0.var_5)) - (rb_state0.var_6));
            rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_4);
            rb_state0.var_8 = (rb_state0.var_8) ^ (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_6) : (0x1f742348));
            rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_7) == (0x0)) ? (rb_state0.var_8) : (0x17ac5efb));
            rb_state0.var_5 = (rb_state0.var_5) - (rb_state0.var_9);
            rb_state0.var_4 = rb_state0.var_5;
            pthread_mutex_unlock(&(rb_state0.lock_25));
        }
        if ((rb_state0.var_1) == (0x0)) {
            if (!((rb_state0.var_4) == (rb_state0.var_5))) {
                racebench_trigger(0);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) ^ (0x8d57bf18);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_3 = (rb_state2.var_3) - (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_3) : (0xc9bcc621));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
        rb_state3.var_3 = (rb_state3.var_3) - (0xffec670a);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ ((0xba27dee0) + (0x53e4dce1));
        if ((rb_state4.var_2) == (0x0)) {
            rb_state4.var_6 = 0x1;
        }
        if ((rb_state4.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state4.lock_18));
            rb_state4.var_7 = 0x7deb6476;
            rb_state4.var_8 = 0x38896096;
            rb_state4.var_9 = (rb_state4.var_9) - (((0x2e273665) ^ (rb_state4.var_8)) ^ (rb_state4.var_5));
            rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_6);
            rb_state4.var_11 = (rb_state4.var_11) - (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_4) : (0x6cff6cd6));
            rb_state4.var_12 = (rb_state4.var_12) ^ (((0x26fc6893) ^ (0x1c47d664)) - (rb_state4.var_9));
            rb_state4.var_13 = (rb_state4.var_13) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_10) : (0xda535270));
            rb_state4.var_14 = (rb_state4.var_14) ^ (((0x36f7cdfd) - (0xe78e3d41)) ^ (rb_state4.var_11));
            rb_state4.var_15 = (rb_state4.var_15) + (rb_state4.var_12);
            rb_state4.var_16 = (rb_state4.var_16) + (((((0xc1c1ae9c) + (0xf7136dc8)) + (rb_state4.var_14)) - (rb_state4.var_8)) + (rb_state4.var_13));
            rb_state4.var_17 = (rb_state4.var_17) ^ (((((0x48f182c) - (rb_state4.var_16)) - (rb_state4.var_9)) + (0x925876f4)) - (rb_state4.var_15));
            rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_17);
            rb_state4.var_4 = rb_state4.var_5;
            pthread_mutex_unlock(&(rb_state4.lock_18));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (33225 < rb_input_size ? (uint32_t)rb_input[33225] : 0xe65c3c30);
        if ((rb_state6.var_2) == (0x2824774b)) {
            pthread_mutex_lock(&(rb_state6.lock_22));
            rb_state6.var_5 = (rb_state6.var_5) + ((0x7746a114) - (0x280cd244));
            rb_state6.var_6 = (rb_state6.var_6) + (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_5) : (0x9c7e3e4f));
            rb_state6.var_7 = (rb_state6.var_7) - (((0xaa4c556a) - (rb_state6.var_3)) - (0x26a86af0));
            rb_state6.var_8 = (rb_state6.var_8) + (rb_state6.var_4);
            rb_state6.var_9 = (rb_state6.var_9) + (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0xe6e5189));
            rb_state6.var_10 = (rb_state6.var_10) + (((rb_state6.var_7) == (0x0)) ? (rb_state6.var_7) : (0x761a6738));
            rb_state6.var_11 = (rb_state6.var_11) ^ (((0x93cdf334) - (rb_state6.var_8)) - (rb_state6.var_8));
            rb_state6.var_12 = (rb_state6.var_12) - (((0x18f9b3f3) - (rb_state6.var_9)) - (0xf085d9de));
            rb_state6.var_13 = (rb_state6.var_13) ^ (((((0x35589699) ^ (rb_state6.var_11)) ^ (0xe82d932c)) ^ (rb_state6.var_10)) ^ (rb_state6.var_9));
            rb_state6.var_14 = (rb_state6.var_14) ^ (((((0xf31eaded) + (rb_state6.var_13)) + (rb_state6.var_12)) + (0xc915a0ec)) - (0x1236b24));
            rb_state6.var_4 = (rb_state6.var_4) + (rb_state6.var_14);
            rb_state6.var_3 = rb_state6.var_4;
            pthread_mutex_unlock(&(rb_state6.lock_22));
        }
        if ((rb_state6.var_2) == (0x2824774b)) {
            if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (0x95eefb13);
        rb_state8.var_3 = (rb_state8.var_3) - (22794 < rb_input_size ? (uint32_t)rb_input[22794] : 0xa4eb3d82);
        rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_3);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x68f23ff));
        if ((rb_state9.var_3) == (0xb2)) {
            rb_state9.var_4 = rb_state9.var_0;
        }
        if ((rb_state9.var_3) == (0xb2)) {
            if (!((rb_state9.var_0) == (rb_state9.var_4))) {
                racebench_trigger(9);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x0)) ? (rb_state11.var_0) : (0xa349db24));
        if ((rb_state11.var_2) == (0x0)) {
            if ((rb_state11.var_4) != (0x0)) {
                if (!((rb_state11.var_4) == (rb_state11.var_25))) {
                    racebench_trigger(11);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0x9bcd69fe) ^ (0x10e5b1e));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (0xc75b3e00);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ ((0xc4a62741) - (rb_state19.var_0));
        #endif
        arg[i].points = points;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_0);
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_1);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_2) : (0xcc30a7a8));
        rb_state1.var_1 = (rb_state1.var_1) - (rb_state1.var_2);
        if ((rb_state1.var_0) == (0x8c48dcb4)) {
            pthread_mutex_lock(&(rb_state1.lock_16));
            rb_state1.var_5 = 0x1f44956f;
            rb_state1.var_6 = 0xb8caee72;
            rb_state1.var_7 = (rb_state1.var_7) ^ (((((0xd5bbc1ef) + (0x4df8d599)) - (rb_state1.var_4)) ^ (rb_state1.var_3)) + (rb_state1.var_5));
            rb_state1.var_8 = (rb_state1.var_8) ^ (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_7) : (0x887a5049));
            rb_state1.var_4 = (rb_state1.var_4) + (rb_state1.var_8);
            rb_state1.var_3 = rb_state1.var_4;
            pthread_mutex_unlock(&(rb_state1.lock_16));
        }
        if ((rb_state1.var_0) == (0x8c48dcb4)) {
            pthread_mutex_lock(&(rb_state1.lock_16));
            rb_state1.var_17 = !((rb_state1.var_3) == (rb_state1.var_4));
            pthread_mutex_unlock(&(rb_state1.lock_16));
        }
        rb_state1.var_13 = (rb_state1.var_13) - (28746 < rb_input_size ? (uint32_t)rb_input[28746] : 0x53f97b3);
        if ((rb_state1.var_9) == (0x4137701b)) {
            pthread_mutex_lock(&(rb_state1.lock_45));
            rb_state1.var_20 = (rb_state1.var_20) - (rb_state1.var_17);
            rb_state1.var_21 = (rb_state1.var_21) ^ (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_10) : (0x80e190e4));
            rb_state1.var_22 = (rb_state1.var_22) + (((0x31aaef9a) + (rb_state1.var_19)) - (rb_state1.var_14));
            rb_state1.var_23 = (rb_state1.var_23) ^ ((0x9e93ae57) - (rb_state1.var_20));
            rb_state1.var_24 = (rb_state1.var_24) - (((0x7739b0ef) + (rb_state1.var_15)) ^ (0xcd6daf64));
            rb_state1.var_25 = (rb_state1.var_25) - (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_20) : (0x554c3639));
            rb_state1.var_26 = (rb_state1.var_26) + (rb_state1.var_21);
            rb_state1.var_27 = (rb_state1.var_27) ^ (rb_state1.var_22);
            rb_state1.var_28 = (rb_state1.var_28) - (rb_state1.var_23);
            rb_state1.var_29 = (rb_state1.var_29) - (rb_state1.var_24);
            rb_state1.var_30 = (rb_state1.var_30) ^ (((0x34d97e70) ^ (rb_state1.var_25)) + (rb_state1.var_22));
            rb_state1.var_31 = (rb_state1.var_31) + (((0xd73dfc8f) + (0xcf7ddea8)) - (rb_state1.var_26));
            rb_state1.var_32 = (rb_state1.var_32) ^ (((rb_state1.var_23) == (0x0)) ? (rb_state1.var_27) : (0x612976e));
            rb_state1.var_33 = (rb_state1.var_33) - (((rb_state1.var_28) == (0x0)) ? (rb_state1.var_29) : (0xe4af1f46));
            rb_state1.var_34 = (rb_state1.var_34) ^ (((((0xa3e5142d) ^ (rb_state1.var_24)) ^ (rb_state1.var_31)) ^ (rb_state1.var_30)) ^ (rb_state1.var_25));
            rb_state1.var_35 = (rb_state1.var_35) ^ (((((0x82adb850) + (rb_state1.var_32)) + (rb_state1.var_27)) ^ (rb_state1.var_33)) ^ (rb_state1.var_26));
            rb_state1.var_36 = (rb_state1.var_36) ^ (((((0x4381382a) - (rb_state1.var_29)) - (rb_state1.var_34)) + (rb_state1.var_35)) - (rb_state1.var_28));
            rb_state1.var_19 = (rb_state1.var_19) + (rb_state1.var_36);
            rb_state1.var_18 = rb_state1.var_19;
            pthread_mutex_unlock(&(rb_state1.lock_45));
        }
        if ((rb_state1.var_9) == (0x4137701b)) {
            if (!((rb_state1.var_18) == (rb_state1.var_19))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state2.lock_19));
            rb_state2.var_6 = 0xa6db2b76;
            rb_state2.var_7 = (rb_state2.var_7) - (((((0xf2890cb0) - (rb_state2.var_6)) ^ (rb_state2.var_6)) ^ (rb_state2.var_4)) - (rb_state2.var_7));
            rb_state2.var_8 = (rb_state2.var_8) ^ (0x7a69d597);
            rb_state2.var_9 = (rb_state2.var_9) + (((0x68455bc5) ^ (rb_state2.var_8)) - (rb_state2.var_5));
            rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_7) : (0x8ef5d482));
            rb_state2.var_11 = (rb_state2.var_11) + (((0xe10df041) + (rb_state2.var_10)) ^ (rb_state2.var_8));
            rb_state2.var_12 = (rb_state2.var_12) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0xe52ebf24));
            rb_state2.var_13 = (rb_state2.var_13) + (((0x7bd7c152) + (rb_state2.var_11)) - (0xed6c41b));
            rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_12) : (0x8201f515));
            rb_state2.var_15 = (rb_state2.var_15) ^ (((((0x49f619d8) + (rb_state2.var_13)) ^ (rb_state2.var_14)) - (0x88fbeaff)) ^ (0xa0d149fe));
            rb_state2.var_5 = (rb_state2.var_5) ^ (rb_state2.var_15);
            rb_state2.var_4 = rb_state2.var_5;
            pthread_mutex_unlock(&(rb_state2.lock_19));
        }
        if ((rb_state2.var_2) == (0x0)) {
            if (!((rb_state2.var_4) == (rb_state2.var_5))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
        rb_state3.var_2 = (rb_state3.var_2) ^ ((0x67fc39e9) - (rb_state3.var_2));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_15 = (rb_state4.var_15) ^ (rb_state4.var_16);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_2);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0x2b33e120) - (rb_state9.var_1));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (10207 < rb_input_size ? (uint32_t)rb_input[10207] : 0xc90b6dd9);
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_0);
        rb_state10.var_2 = (rb_state10.var_2) - ((0xc6e405e1) ^ (rb_state10.var_2));
        rb_state10.var_3 = (rb_state10.var_3) ^ (0x4d51703d);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (0x8fea072f);
        rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_0);
        rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x8fea072f)) ? (rb_state13.var_0) : (0xca9a2947));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (36479 < rb_input_size ? (uint32_t)rb_input[36479] : 0xa37b9fbf);
        rb_state15.var_2 = (rb_state15.var_2) + ((0xfc66e9b8) ^ (0x35aaa5c0));
        rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_1);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (0x3c99abba);
        rb_state18.var_2 = (rb_state18.var_2) - (33265 < rb_input_size ? (uint32_t)rb_input[33265] : 0xc82f4362);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_1);
        #endif
        arg[i].kmin = kmin;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ (0xcae45f7);
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0xf24ca057));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (0xdae5c6ef);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_1) == (0x3e)) ? (rb_state4.var_0) : (0x339f72e8));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0xfffffff4)) ? (rb_state6.var_0) : (0xa6560ce7));
        rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) - (13230 < rb_input_size ? (uint32_t)rb_input[13230] : 0x1440c146);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (((rb_state10.var_0) == (0x3d)) ? (rb_state10.var_2) : (0xfd71e245));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (0x3be2ab69);
        rb_state11.var_1 = (rb_state11.var_1) ^ ((0x1f934490) ^ (rb_state11.var_0));
        rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_2) == (0x0)) ? (rb_state11.var_2) : (0xae017006));
        rb_state11.var_3 = (rb_state11.var_3) ^ (64080 < rb_input_size ? (uint32_t)rb_input[64080] : 0x992db2d7);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (0xa5dea424);
        if ((rb_state13.var_2) == (0x0)) {
            rb_state13.var_4 = 0x5b7b57f7;
            rb_state13.var_5 = (rb_state13.var_5) ^ (((0xe1795095) - (rb_state13.var_4)) - (rb_state13.var_5));
            rb_state13.var_6 = (rb_state13.var_6) + (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_5) : (0x70e71e9f));
            rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_6);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (0x928c9e66);
        rb_state15.var_0 = (rb_state15.var_0) - (0x297db661);
        rb_state15.var_1 = (rb_state15.var_1) ^ (((rb_state15.var_0) == (0x71457cbc)) ? (rb_state15.var_2) : (0x82119902));
        rb_state15.var_3 = (rb_state15.var_3) ^ (rb_state15.var_3);
        if ((rb_state15.var_3) == (0x0)) {
            rb_state15.var_4 = rb_state15.var_1;
        }
        if ((rb_state15.var_3) == (0x0)) {
            if (!((rb_state15.var_1) == (rb_state15.var_4))) {
                racebench_trigger(15);
            }
        }
        #endif
        arg[i].kmax = kmax;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_8 = (rb_state1.var_8) ^ (6590 < rb_input_size ? (uint32_t)rb_input[6590] : 0x9b602c);
        rb_state1.var_11 = (rb_state1.var_11) ^ (0xb9275a84);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x251a3911)) ? (rb_state2.var_0) : (0xc842deb9));
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
        rb_state2.var_0 = (rb_state2.var_0) - (0xd880ff38);
        #endif
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_19) == (0x0)) {
            if ((rb_state4.var_20) != (0x0)) {
                if (!((rb_state4.var_20) == (rb_state4.var_44))) {
                    racebench_trigger(4);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (0x1515ba58);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_0) == (0x95eefb13)) ? (rb_state8.var_2) : (0xd5074125));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) - (39794 < rb_input_size ? (uint32_t)rb_input[39794] : 0x9a0ca768);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_2);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (0x72a5eecd);
        #endif
        arg[i].pid = i;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) ^ ((0xe6ff10d7) - (0x4210f189));
        #endif
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_12 = (rb_state1.var_12) - (62501 < rb_input_size ? (uint32_t)rb_input[62501] : 0x14f6b31d);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_3 = (rb_state3.var_3) - (0x89c7b76f);
        if ((rb_state3.var_1) == (0x0)) {
            if ((rb_state3.var_4) != (0x0)) {
                if (!((rb_state3.var_4) == (rb_state3.var_28))) {
                    racebench_trigger(3);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + ((0xc822a506) ^ (rb_state6.var_1));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_3 = (rb_state7.var_3) ^ ((0x84728907) ^ (0x14a0cf81));
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_2) == (0x2bddf626)) {
            pthread_mutex_lock(&(rb_state8.lock_21));
            rb_state8.var_6 = 0x1cc63675;
            rb_state8.var_7 = 0xfa524fcb;
            rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_5);
            rb_state8.var_9 = (rb_state8.var_9) - (0x37e87941);
            rb_state8.var_10 = (rb_state8.var_10) + (((((0xbcee0a9c) + (rb_state8.var_6)) - (rb_state8.var_4)) - (rb_state8.var_7)) - (rb_state8.var_7));
            rb_state8.var_11 = (rb_state8.var_11) + (rb_state8.var_6);
            rb_state8.var_12 = (rb_state8.var_12) ^ (((((0x3c72e9af) ^ (rb_state8.var_9)) - (0xd139330c)) - (rb_state8.var_8)) + (rb_state8.var_8));
            rb_state8.var_13 = (rb_state8.var_13) - (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_11) : (0x7dc2dfd3));
            rb_state8.var_14 = (rb_state8.var_14) + (((rb_state8.var_9) == (0x0)) ? (rb_state8.var_12) : (0x3ff6141a));
            rb_state8.var_15 = (rb_state8.var_15) ^ (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_14) : (0xec344e66));
            rb_state8.var_5 = (rb_state8.var_5) - (rb_state8.var_15);
            rb_state8.var_4 = rb_state8.var_5;
            pthread_mutex_unlock(&(rb_state8.lock_21));
        }
        if ((rb_state8.var_2) == (0x2bddf626)) {
            if (!((rb_state8.var_4) == (rb_state8.var_5))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_2);
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) - (((rb_state10.var_0) == (0x3d)) ? (rb_state10.var_2) : (0x3d786377));
        if ((rb_state10.var_3) == (0x4d51703d)) {
            rb_state10.var_5 = 0x844ff546;
            rb_state10.var_6 = 0x8504fea;
            rb_state10.var_7 = (rb_state10.var_7) + (((((0xe037a488) - (0x9db3644e)) + (rb_state10.var_5)) - (rb_state10.var_5)) ^ (rb_state10.var_6));
            rb_state10.var_8 = (rb_state10.var_8) ^ (((((0x7e266945) + (rb_state10.var_7)) + (rb_state10.var_7)) + (rb_state10.var_4)) - (rb_state10.var_6));
            rb_state10.var_1 = (rb_state10.var_1) - (rb_state10.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_2);
        #endif
        arg[i].kfinal = kfinal;

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_2 = (rb_state1.var_2) - (0x925ba);
        rb_state1.var_2 = (rb_state1.var_2) ^ ((0x276a795d) + (rb_state1.var_1));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_2);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ ((0xf31ef312) + (rb_state3.var_1));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_3 = (rb_state4.var_3) + (29825 < rb_input_size ? (uint32_t)rb_input[29825] : 0x6dca45ec);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (44032 < rb_input_size ? (uint32_t)rb_input[44032] : 0x9d739b34);
        rb_state6.var_2 = (rb_state6.var_2) ^ (31964 < rb_input_size ? (uint32_t)rb_input[31964] : 0x22053370);
        rb_state6.var_2 = (rb_state6.var_2) ^ ((0x2824774b) ^ (rb_state6.var_2));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) ^ (0x515364db);
        rb_state7.var_2 = (rb_state7.var_2) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_3) : (0x61c197c1));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_0) == (0x95eefb13)) ? (rb_state8.var_2) : (0x91eefe35));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) + ((0x770c5824) ^ (0x8e99884f));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_3);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) + (5837 < rb_input_size ? (uint32_t)rb_input[5837] : 0x349b56e0);
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (0xfb44a67e);
        if ((rb_state19.var_3) == (0x0)) {
            rb_state19.var_4 = rb_state19.var_2;
        }
        if ((rb_state19.var_3) == (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_4))) {
                racebench_trigger(19);
            }
        }
        #endif
        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_3 = (rb_state0.var_3) + ((0x246f4432) - (rb_state0.var_3));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) + (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - ((0xb3bac4cd) ^ (0xe4523151));
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (rb_state4.var_0);
        rb_state4.var_1 = (rb_state4.var_1) + (21501 < rb_input_size ? (uint32_t)rb_input[21501] : 0x13c66ec7);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ ((0xdbfd5efd) + (rb_state7.var_0));
        if ((rb_state7.var_0) == (0x2402a103)) {
            pthread_mutex_lock(&(rb_state7.lock_16));
            rb_state7.var_6 = 0xa74c01d1;
            rb_state7.var_7 = (rb_state7.var_7) ^ (((rb_state7.var_5) == (0x0)) ? (rb_state7.var_4) : (0x642872e3));
            rb_state7.var_8 = (rb_state7.var_8) ^ (((((0x3f58ec1d) - (rb_state7.var_6)) + (rb_state7.var_6)) - (rb_state7.var_7)) - (0x2abdc84a));
            rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_8);
            rb_state7.var_4 = rb_state7.var_5;
            pthread_mutex_unlock(&(rb_state7.lock_16));
        }
        if ((rb_state7.var_0) == (0x2402a103)) {
            pthread_mutex_lock(&(rb_state7.lock_16));
            if (!((rb_state7.var_4) == (rb_state7.var_5))) {
                racebench_trigger(7);
            }
            pthread_mutex_unlock(&(rb_state7.lock_16));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_3);
        rb_state9.var_3 = (rb_state9.var_3) ^ (38236 < rb_input_size ? (uint32_t)rb_input[38236] : 0x202e140c);
        rb_state9.var_2 = (rb_state9.var_2) - (0x22c62299);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
        rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_2);
        rb_state13.var_0 = (rb_state13.var_0) ^ (0xd09a1cd0);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) - (0x63aa947c);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) + (((rb_state18.var_0) == (0x66cfe869)) ? (rb_state18.var_3) : (0xa35ea51d));
        if ((rb_state18.var_1) == (0x3c99abba)) {
            pthread_mutex_lock(&(rb_state18.lock_32));
            rb_state18.var_6 = (rb_state18.var_6) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_4) : (0xeb35be99));
            rb_state18.var_7 = (rb_state18.var_7) - ((0xbaa0d8bc) - (0xd9dec11a));
            rb_state18.var_8 = (rb_state18.var_8) - (0xad7283f6);
            rb_state18.var_9 = (rb_state18.var_9) ^ (rb_state18.var_5);
            rb_state18.var_10 = (rb_state18.var_10) ^ (((0xa58e608a) - (rb_state18.var_7)) + (rb_state18.var_6));
            rb_state18.var_11 = (rb_state18.var_11) ^ (((rb_state18.var_8) == (0x528d7c0a)) ? (rb_state18.var_7) : (0x8d07e70d));
            rb_state18.var_12 = (rb_state18.var_12) - (((0x83a29b7f) - (rb_state18.var_9)) - (rb_state18.var_8));
            rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0xd155a5da));
            rb_state18.var_14 = (rb_state18.var_14) ^ (((0x93f20982) - (rb_state18.var_11)) ^ (0x5fd7c517));
            rb_state18.var_15 = (rb_state18.var_15) ^ (((0xc2945ddb) ^ (rb_state18.var_12)) ^ (rb_state18.var_10));
            rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_11) == (0x1f3de85e)) ? (rb_state18.var_13) : (0xac773317));
            rb_state18.var_17 = (rb_state18.var_17) - (((0x680b3692) + (0x3cb8125)) - (rb_state18.var_14));
            rb_state18.var_18 = (rb_state18.var_18) ^ (((rb_state18.var_12) == (0xceeae08b)) ? (rb_state18.var_15) : (0x3a499e40));
            rb_state18.var_19 = (rb_state18.var_19) + (((((0x75c71011) + (rb_state18.var_17)) ^ (rb_state18.var_14)) - (rb_state18.var_13)) - (rb_state18.var_16));
            rb_state18.var_20 = (rb_state18.var_20) ^ (((rb_state18.var_15) == (0x8a2ec57c)) ? (rb_state18.var_18) : (0x580e8fe5));
            rb_state18.var_21 = (rb_state18.var_21) - (((rb_state18.var_19) == (0x1196e866)) ? (rb_state18.var_20) : (0xbeeb083e));
            rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_21);
            rb_state18.var_4 = rb_state18.var_5;
            pthread_mutex_unlock(&(rb_state18.lock_32));
        }
        if ((rb_state18.var_1) == (0x3c99abba)) {
            if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                racebench_trigger(18);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) + (46318 < rb_input_size ? (uint32_t)rb_input[46318] : 0xa5e0c50a);
        #endif
        pthread_create(threads + i, NULL, localSearchSub, (void *)&arg[i]);
#else
        localSearchSub(&arg[0]);
#endif
    }

#ifdef ENABLE_THREADS
    for (int i = 0; i < nproc; i++) {
        pthread_join(threads[i], NULL);
    }
#endif

    delete[] threads;
    delete[] arg;
#ifdef ENABLE_THREADS
    pthread_barrier_destroy(&barrier);
#endif
}
#endif

class PStream {
public:
    virtual size_t read(float *dest, int dim, int num) = 0;
    virtual int ferror() = 0;
    virtual int feof() = 0;
    virtual ~PStream() {
    }
};

class SimStream : public PStream {
public:
    SimStream(long n_) {
        n = n_;
    }
    size_t read(float *dest, int dim, int num) {
        size_t count = 0;
        for (int i = 0; i < num && n > 0; i++) {
            for (int k = 0; k < dim; k++) {
                dest[i * dim + k] = myrand() / (float)INT_MAX;
            }
            n--;
            count++;
        }
        return count;
    }
    int ferror() {
        return 0;
    }
    int feof() {
        return n <= 0;
    }
    ~SimStream() {
    }

private:
    long n;
};

class FileStream : public PStream {
public:
    FileStream(char *filename) {
        fp = fopen(filename, "rb");
        if (fp == NULL) {
            fprintf(stderr, "error opening file %s\n.", filename);
            exit(1);
        }
    }
    size_t read(float *dest, int dim, int num) {
        size_t count = std::fread(dest, sizeof(float) * dim, num, fp);
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                float *val = &dest[i * dim + j];
                if (isnan(*val) || *val < 0)
                    *val = 0.0;
                else if (*val > 1)
                    *val = 1.0;
            }
        }
        return count;
    }
    int ferror() {
        return std::ferror(fp);
    }
    int feof() {
        return std::feof(fp);
    }
    ~FileStream() {
        fprintf(stderr, "closing file stream\n");
        fclose(fp);
    }

private:
    FILE *fp;
};

void outcenterIDs(Points *centers, long *centerIDs, char *outfile) {
    FILE *fp = fopen(outfile, "w");
    if (fp == NULL) {
        fprintf(stderr, "error opening %s\n", outfile);
        exit(1);
    }
    int *is_a_median = (int *)calloc(sizeof(int), centers->num);
    for (int i = 0; i < centers->num; i++) {
        is_a_median[centers->p[i].assign] = 1;
    }

    for (int i = 0; i < centers->num; i++) {
        if (is_a_median[i]) {
            fprintf(fp, "%u\n", centerIDs[i]);
            fprintf(fp, "%lf\n", centers->p[i].weight);
            for (int k = 0; k < centers->dim; k++) {
                fprintf(fp, "%lf ", centers->p[i].coord[k]);
            }
            fprintf(fp, "\n\n");
        }
    }
    fclose(fp);
}

void streamCluster(PStream *stream, long kmin, long kmax, int dim, long chunksize, long centersize, char *outfile) {

#ifdef TBB_VERSION
    float *block = (float *)memoryFloat.allocate(chunksize * dim * sizeof(float));
    float *centerBlock = (float *)memoryFloat.allocate(centersize * dim * sizeof(float));
    long *centerIDs = (long *)memoryLong.allocate(centersize * dim * sizeof(long));
#else
    float *block = (float *)malloc(chunksize * dim * sizeof(float));
    float *centerBlock = (float *)malloc(centersize * dim * sizeof(float));
    long *centerIDs = (long *)malloc(centersize * dim * sizeof(long));
#endif

    if (block == NULL) {
        fprintf(stderr, "not enough memory for a chunk!\n");
        exit(1);
    }

    Points points;
    points.dim = dim;
    points.num = chunksize;
    points.p =
#ifdef TBB_VERSION
        (Point *)memoryPoint.allocate(chunksize * sizeof(Point), NULL);
#else
        (Point *)malloc(chunksize * sizeof(Point));
#endif

    for (int i = 0; i < chunksize; i++) {
        points.p[i].coord = &block[i * dim];
    }

    Points centers;
    centers.dim = dim;
    centers.p =
#ifdef TBB_VERSION
        (Point *)memoryPoint.allocate(centersize * sizeof(Point), NULL);
#else
        (Point *)malloc(centersize * sizeof(Point));
#endif
    centers.num = 0;

    for (int i = 0; i < centersize; i++) {
        centers.p[i].coord = &centerBlock[i * dim];
        centers.p[i].weight = 1.0;
    }

    long IDoffset = 0;
    long kfinal;
    while (1) {

        size_t numRead = stream->read(block, dim, chunksize);
        fprintf(stderr, "read %d points\n", numRead);

        if (stream->ferror() || numRead < (unsigned int)chunksize && !stream->feof()) {
            fprintf(stderr, "error reading data!\n");
            exit(1);
        }

        points.num = numRead;
        for (int i = 0; i < points.num; i++) {
            points.p[i].weight = 1.0;
        }

#ifdef TBB_VERSION
        switch_membership = (bool *)memoryBool.allocate(points.num * sizeof(bool), NULL);
        is_center = (bool *)calloc(points.num, sizeof(bool));
        center_table = (int *)memoryInt.allocate(points.num * sizeof(int));
#else
        switch_membership = (bool *)malloc(points.num * sizeof(bool));
        is_center = (bool *)calloc(points.num, sizeof(bool));
        center_table = (int *)malloc(points.num * sizeof(int));
#endif

        localSearch(&points, kmin, kmax, &kfinal);

        contcenters(&points);
        if (kfinal + centers.num > centersize) {

            fprintf(stderr, "oops! no more space for centers\n");
            exit(1);
        }

        copycenters(&points, &centers, centerIDs, IDoffset);
        IDoffset += numRead;

#ifdef TBB_VERSION
        memoryBool.deallocate(switch_membership, sizeof(bool));
        free(is_center);
        memoryInt.deallocate(center_table, sizeof(int));
#else
        free(is_center);
        free(switch_membership);
        free(center_table);
#endif

        if (stream->feof()) {
            break;
        }
    }

#ifdef TBB_VERSION
    switch_membership = (bool *)memoryBool.allocate(centers.num * sizeof(bool));
    is_center = (bool *)calloc(centers.num, sizeof(bool));
    center_table = (int *)memoryInt.allocate(centers.num * sizeof(int));
#else
    switch_membership = (bool *)malloc(centers.num * sizeof(bool));
    is_center = (bool *)calloc(centers.num, sizeof(bool));
    center_table = (int *)malloc(centers.num * sizeof(int));
#endif

    localSearch(&centers, kmin, kmax, &kfinal);
    contcenters(&centers);
    outcenterIDs(&centers, centerIDs, outfile);
}

int main(int argc, char **argv) {
    char *outfilename = new char[MAXNAMESIZE];
    char *infilename = new char[MAXNAMESIZE];
    long kmin, kmax, n, chunksize, clustersize;
    int dim;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    fprintf(stderr, "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) "\n");
    fflush(NULL);
#else
    fprintf(stderr, "PARSEC Benchmark Suite\n");
    fflush(NULL);
#endif
#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_begin(__parsec_streamcluster);
#endif

    if (argc < 10) {
        fprintf(stderr, "usage: %s k1 k2 d n chunksize clustersize infile outfile nproc\n", argv[0]);
        fprintf(stderr, "  k1:          Min. number of centers allowed\n");
        fprintf(stderr, "  k2:          Max. number of centers allowed\n");
        fprintf(stderr, "  d:           Dimension of each data point\n");
        fprintf(stderr, "  n:           Number of data points\n");
        fprintf(stderr, "  chunksize:   Number of data points to handle per step\n");
        fprintf(stderr, "  clustersize: Maximum number of intermediate centers\n");
        fprintf(stderr, "  infile:      Input file (if n<=0)\n");
        fprintf(stderr, "  outfile:     Output file\n");
        fprintf(stderr, "  nproc:       Number of threads to use\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "if n > 0, points will be randomly generated instead of reading from infile.\n");
        exit(1);
    }

    kmin = atoi(argv[1]);
    kmax = atoi(argv[2]);
    dim = atoi(argv[3]);
    n = atoi(argv[4]);
    chunksize = atoi(argv[5]);
    clustersize = atoi(argv[6]);
    strcpy(infilename, argv[7]);
    strcpy(outfilename, argv[8]);
    nproc = atoi(argv[9]);

#ifdef TBB_VERSION
    fprintf(stderr, "TBB version. Number of divisions: %d\n", NUM_DIVISIONS);
    tbb::task_scheduler_init init(nproc);
#endif

    srand48(SEED);
    PStream *stream;
    if (n > 0) {
        stream = new SimStream(n);
    } else {
        stream = new FileStream(infilename);
    }

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif

    streamCluster(stream, kmin, kmax, dim, chunksize, clustersize, outfilename);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    delete stream;

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_bench_end();
#endif

    return 0;
}