

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

    double myhiz = 0;
    for (long kk = k1; kk < k2; kk++) {
        myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    hizs[pid] = myhiz;

#ifdef ENABLE_THREADS
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

    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
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
        arg[i].points = points;
        arg[i].kmin = kmin;
        arg[i].kmax = kmax;
        arg[i].pid = i;
        arg[i].kfinal = kfinal;

        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
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
