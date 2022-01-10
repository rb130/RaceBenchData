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
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x4f1baa6a));
        if ((rb_state14.var_0) == (0xd6511a0)) {
            if ((rb_state14.var_1) != (0x0)) {
                if (!((rb_state14.var_1) == (rb_state14.var_23))) {
                    racebench_trigger(14);
                }
            }
        }
        #endif
        float distance = dist(points->p[k], points->p[0], points->dim);
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_0) == (0xbe496736)) {
            pthread_mutex_lock(&(rb_state14.lock_35));
            rb_state14.var_13 = 0xcc27394d;
            rb_state14.var_14 = 0xa3fb9ad6;
            rb_state14.var_15 = 0x5ddf77de;
            rb_state14.var_16 = (rb_state14.var_16) ^ (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_13) : (0x9900ebe7));
            rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_14) : (0x87abf433));
            rb_state14.var_18 = (rb_state14.var_18) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_16) : (0xeeaaa5ac));
            rb_state14.var_19 = (rb_state14.var_19) - (((0xc6a77f06) ^ (rb_state14.var_17)) - (0x3b8473d0));
            rb_state14.var_20 = (rb_state14.var_20) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_18) : (0xee741763));
            rb_state14.var_21 = (rb_state14.var_21) + (((0x8f94e7e3) + (rb_state14.var_19)) ^ (0x134d0fe9));
            rb_state14.var_22 = (rb_state14.var_22) + (((((0x83b7dcbe) - (rb_state14.var_21)) - (0x60cb46f0)) - (rb_state14.var_9)) ^ (rb_state14.var_20));
            rb_state14.var_12 = (rb_state14.var_12) - (rb_state14.var_22);
            rb_state14.var_1 = rb_state14.var_12;
            pthread_mutex_unlock(&(rb_state14.lock_35));
        }
        if ((rb_state14.var_0) == (0xd6511a0)) {
            pthread_mutex_lock(&(rb_state14.lock_35));
            rb_state14.var_24 = 0x2113930c;
            rb_state14.var_25 = 0xfdb92e78;
            rb_state14.var_26 = (rb_state14.var_26) - (((0xc8e71a83) ^ (rb_state14.var_11)) ^ (rb_state14.var_24));
            rb_state14.var_27 = (rb_state14.var_27) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_25) : (0xdf65bca6));
            rb_state14.var_28 = (rb_state14.var_28) - (rb_state14.var_10);
            rb_state14.var_29 = (rb_state14.var_29) ^ (rb_state14.var_26);
            rb_state14.var_30 = (rb_state14.var_30) + (rb_state14.var_27);
            rb_state14.var_31 = (rb_state14.var_31) - (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_28) : (0x7617f58));
            rb_state14.var_32 = (rb_state14.var_32) ^ (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_29) : (0xbdf52231));
            rb_state14.var_33 = (rb_state14.var_33) + (((((0x7ddbc16e) + (rb_state14.var_16)) - (rb_state14.var_30)) - (rb_state14.var_31)) + (rb_state14.var_15));
            rb_state14.var_34 = (rb_state14.var_34) ^ (((((0xb39fbd04) - (rb_state14.var_32)) ^ (0x1b22e799)) - (rb_state14.var_17)) - (rb_state14.var_33));
            rb_state14.var_23 = (rb_state14.var_23) ^ (rb_state14.var_34);
            rb_state14.var_1 = rb_state14.var_23;
            pthread_mutex_unlock(&(rb_state14.lock_35));
        }
        #endif
        points->p[k].cost = distance * points->p[k].weight;
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (47262 < rb_input_size ? (uint32_t)rb_input[47262] : 0xa79d102c);
        #endif
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
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xebc7c70f));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x32ec7ea7));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x84)) ? (rb_state17.var_0) : (0x69617367));
    #endif
    hiz = loz = 0.0;
    long numberOfPoints = points->num;
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_19));
        rb_state12.var_5 = 0x62c969b;
        rb_state12.var_6 = (rb_state12.var_6) + (rb_state12.var_4);
        rb_state12.var_7 = (rb_state12.var_7) ^ ((0x7aa76f1e) + (rb_state12.var_5));
        rb_state12.var_8 = (rb_state12.var_8) - (((((0x76755437) ^ (rb_state12.var_5)) ^ (rb_state12.var_3)) - (0x3e666d96)) ^ (rb_state12.var_6));
        rb_state12.var_9 = (rb_state12.var_9) + (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0x36da7d23));
        rb_state12.var_10 = (rb_state12.var_10) - (((0x78b3a255) - (0xb5fbc67a)) ^ (rb_state12.var_8));
        rb_state12.var_11 = (rb_state12.var_11) ^ (((((0xed1dfe60) ^ (0x5e8dd69f)) + (rb_state12.var_10)) + (rb_state12.var_9)) + (0x50475405));
        rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_11);
        rb_state12.var_1 = rb_state12.var_4;
        pthread_mutex_unlock(&(rb_state12.lock_19));
    }
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_19));
        rb_state12.var_13 = 0xd3bd6568;
        rb_state12.var_14 = 0xbfdeef5e;
        rb_state12.var_15 = (rb_state12.var_15) - (((0x6e544233) - (rb_state12.var_14)) ^ (rb_state12.var_8));
        rb_state12.var_16 = (rb_state12.var_16) - (((((0xc198d460) ^ (rb_state12.var_7)) + (0x2f5d8bf2)) + (rb_state12.var_9)) ^ (rb_state12.var_13));
        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_15) : (0xc5a8a2c6));
        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_17) : (0x96e2100));
        rb_state12.var_12 = (rb_state12.var_12) ^ (rb_state12.var_18);
        rb_state12.var_1 = rb_state12.var_12;
        pthread_mutex_unlock(&(rb_state12.lock_19));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x0)) {
        rb_state15.var_22 = 0x64ae912c;
        rb_state15.var_23 = (rb_state15.var_23) - (((((0x708d7352) ^ (rb_state15.var_10)) ^ (0x2130ab70)) ^ (rb_state15.var_11)) + (rb_state15.var_22));
        rb_state15.var_24 = (rb_state15.var_24) - (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_23) : (0x4d7f5319));
        rb_state15.var_21 = (rb_state15.var_21) ^ (rb_state15.var_24);
        rb_state15.var_1 = rb_state15.var_21;
    }
    if ((rb_state15.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state15.lock_30));
        rb_state15.var_26 = 0xe38444bc;
        rb_state15.var_27 = (rb_state15.var_27) + (rb_state15.var_12);
        rb_state15.var_28 = (rb_state15.var_28) - (((rb_state15.var_26) == (0xe38444bc)) ? (rb_state15.var_13) : (0xdd4bb9de));
        rb_state15.var_29 = (rb_state15.var_29) - (((((0xf9d9f2b2) + (rb_state15.var_28)) ^ (rb_state15.var_15)) ^ (rb_state15.var_14)) - (rb_state15.var_27));
        rb_state15.var_25 = (rb_state15.var_25) ^ (rb_state15.var_29);
        rb_state15.var_1 = rb_state15.var_25;
        pthread_mutex_unlock(&(rb_state15.lock_30));
    }
    #endif
    long ptDimension = points->dim;

    long bsize = points->num / nproc;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0xe7093b08));
    #endif
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0xd2c2e7d6)) {
        rb_state17.var_2 = rb_state17.var_1;
    }
    if ((rb_state17.var_0) == (0xd2c2e7d6)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    long k1 = bsize * pid;
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0x0)) {
        if ((rb_state13.var_1) != (0x0)) {
            if (!((rb_state13.var_1) == (rb_state13.var_19))) {
                racebench_trigger(13);
            }
        }
    }
    #endif
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        k2 = points->num;
    }

#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif

    double myhiz = 0;
    for (long kk = k1; kk < k2; kk++) {
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (0xa3b3fef);
        rb_state19.var_0 = (rb_state19.var_0) ^ (0x9800aec2);
        if ((rb_state19.var_0) == (0x17376897)) {
            pthread_mutex_lock(&(rb_state19.lock_26));
            rb_state19.var_3 = 0x46b7aa77;
            rb_state19.var_4 = 0x4da9e77f;
            rb_state19.var_5 = (rb_state19.var_5) - (((0x3aec0d46) ^ (rb_state19.var_2)) ^ (rb_state19.var_3));
            rb_state19.var_6 = (rb_state19.var_6) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_4) : (0xd341031d));
            rb_state19.var_7 = (rb_state19.var_7) - (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_6) : (0x4744c2cb));
            rb_state19.var_2 = (rb_state19.var_2) - (rb_state19.var_7);
            rb_state19.var_1 = rb_state19.var_2;
            pthread_mutex_unlock(&(rb_state19.lock_26));
        }
        if ((rb_state19.var_0) == (0x94fc866a)) {
            rb_state19.var_8 = 0x54a790cc;
            rb_state19.var_9 = (rb_state19.var_9) + (rb_state19.var_5);
            rb_state19.var_10 = (rb_state19.var_10) - (rb_state19.var_3);
            rb_state19.var_11 = (rb_state19.var_11) + ((0x24cb19f) + (rb_state19.var_6));
            rb_state19.var_12 = (rb_state19.var_12) ^ (((0xba4be7c0) + (rb_state19.var_8)) ^ (0x5fc459df));
            rb_state19.var_13 = (rb_state19.var_13) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_4) : (0x78b2a273));
            rb_state19.var_14 = (rb_state19.var_14) - (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_9) : (0x6d173ad9));
            rb_state19.var_15 = (rb_state19.var_15) ^ (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_10) : (0xd108ec76));
            rb_state19.var_16 = (rb_state19.var_16) + (((0xcb9d45c7) + (rb_state19.var_11)) ^ (rb_state19.var_10));
            rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0x38e7c77));
            rb_state19.var_18 = (rb_state19.var_18) - (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_13) : (0xc3eec79f));
            rb_state19.var_19 = (rb_state19.var_19) - (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_15) : (0x1b735e4));
            rb_state19.var_20 = (rb_state19.var_20) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_16) : (0xa3a88aba));
            rb_state19.var_21 = (rb_state19.var_21) - (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_18) : (0x41af8bfa));
            rb_state19.var_22 = (rb_state19.var_22) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_19) : (0x7f7c0220));
            rb_state19.var_23 = (rb_state19.var_23) + (rb_state19.var_20);
            rb_state19.var_24 = (rb_state19.var_24) - (((rb_state19.var_21) == (0x0)) ? (rb_state19.var_22) : (0xa19763c0));
            rb_state19.var_25 = (rb_state19.var_25) ^ (((((0x7a8e093a) + (rb_state19.var_15)) + (rb_state19.var_23)) ^ (rb_state19.var_24)) ^ (0xac3bb048));
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_25);
        }
        if ((rb_state19.var_0) == (0x94fc866a)) {
            if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                racebench_trigger(19);
            }
        }
        #endif
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

    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + ((0xbfd6310a) + (rb_state0.var_0));
    if ((rb_state0.var_0) == (0xe9d91e8)) {
        rb_state0.var_7 = 0xbba02c4b;
        rb_state0.var_8 = 0x21a4699a;
        rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x889dcacc));
        rb_state0.var_10 = (rb_state0.var_10) + (((((0xff963633) + (rb_state0.var_6)) - (rb_state0.var_7)) ^ (rb_state0.var_8)) ^ (rb_state0.var_3));
        rb_state0.var_11 = (rb_state0.var_11) - (rb_state0.var_7);
        rb_state0.var_12 = (rb_state0.var_12) ^ (rb_state0.var_9);
        rb_state0.var_13 = (rb_state0.var_13) ^ (rb_state0.var_10);
        rb_state0.var_14 = (rb_state0.var_14) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_11) : (0xc85c45bd));
        rb_state0.var_15 = (rb_state0.var_15) ^ (((((0xbd2e783a) - (rb_state0.var_12)) ^ (rb_state0.var_13)) - (rb_state0.var_9)) + (0x1378cb1));
        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_14) : (0x9bcf93ef));
        rb_state0.var_17 = (rb_state0.var_17) - (((((0x4fcba15c) - (rb_state0.var_16)) ^ (rb_state0.var_11)) - (0x5dbd8ee0)) - (rb_state0.var_15));
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x0)) {
        rb_state3.var_2 = rb_state3.var_1;
    }
    if ((rb_state3.var_0) == (0xa01290b4)) {
        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (17136 < rb_input_size ? (uint32_t)rb_input[17136] : 0x512f116b);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0x611390d4)) {
        rb_state5.var_22 = 0x9f2dffbe;
        rb_state5.var_23 = (rb_state5.var_23) + (rb_state5.var_12);
        rb_state5.var_24 = (rb_state5.var_24) + (((0x5e32cfcd) ^ (rb_state5.var_13)) + (0x4dfdb68b));
        rb_state5.var_25 = (rb_state5.var_25) ^ (11861 < rb_input_size ? (uint32_t)rb_input[11861] : 0x49684690);
        rb_state5.var_26 = (rb_state5.var_26) ^ (rb_state5.var_22);
        rb_state5.var_27 = (rb_state5.var_27) ^ (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_24) : (0xe2e74a2e));
        rb_state5.var_28 = (rb_state5.var_28) ^ (((((0xeea796ca) + (rb_state5.var_26)) ^ (0xab82b4f4)) + (rb_state5.var_25)) - (0x992a211f));
        rb_state5.var_29 = (rb_state5.var_29) - (((0xcdee7b52) - (rb_state5.var_14)) ^ (rb_state5.var_27));
        rb_state5.var_30 = (rb_state5.var_30) ^ (((0x57d5da26) + (rb_state5.var_28)) + (0xe09a99b));
        rb_state5.var_31 = (rb_state5.var_31) ^ (((rb_state5.var_15) == (0x0)) ? (rb_state5.var_29) : (0x4882ced6));
        rb_state5.var_32 = (rb_state5.var_32) - (((rb_state5.var_30) == (0x0)) ? (rb_state5.var_31) : (0x20de69ec));
        rb_state5.var_1 = (rb_state5.var_1) + (rb_state5.var_32);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) - (rb_state8.var_0);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_0) == (0x202d0160)) {
        pthread_mutex_lock(&(rb_state9.lock_15));
        if ((rb_state9.var_1) != (0x0)) {
            if (!((rb_state9.var_1) == (rb_state9.var_11))) {
                racebench_trigger(9);
            }
        }
        pthread_mutex_unlock(&(rb_state9.lock_15));
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x0)) {
        if ((rb_state10.var_1) != (0x0)) {
            if (!((rb_state10.var_1) == (rb_state10.var_32))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_0) == (0x93519477)) {
        if ((rb_state11.var_1) != (0x0)) {
            if (!((rb_state11.var_1) == (rb_state11.var_22))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) + (0x2f04aec8);
    if ((rb_state16.var_0) == (0xb609dd10)) {
        pthread_mutex_lock(&(rb_state16.lock_26));
        rb_state16.var_3 = 0x1eb3083d;
        rb_state16.var_4 = (rb_state16.var_4) - (rb_state16.var_1);
        rb_state16.var_5 = (rb_state16.var_5) - (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_3) : (0xb47a614e));
        rb_state16.var_6 = (rb_state16.var_6) ^ (rb_state16.var_4);
        rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_5) : (0xf406b3d8));
        rb_state16.var_8 = (rb_state16.var_8) + (((((0xeba0ad0c) ^ (rb_state16.var_4)) ^ (rb_state16.var_7)) + (rb_state16.var_6)) - (0xfe3da432));
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_8);
        rb_state16.var_1 = rb_state16.var_2;
        pthread_mutex_unlock(&(rb_state16.lock_26));
    }
    if ((rb_state16.var_0) == (0x3d06460a)) {
        pthread_mutex_lock(&(rb_state16.lock_26));
        rb_state16.var_27 = !((rb_state16.var_1) == (rb_state16.var_2));
        pthread_mutex_unlock(&(rb_state16.lock_26));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_0) == (0x0)) {
        rb_state18.var_2 = rb_state18.var_1;
    }
    if ((rb_state18.var_0) == (0x0)) {
        if (!((rb_state18.var_1) == (rb_state18.var_2))) {
            racebench_trigger(18);
        }
    }
    #endif
    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ ((0x50b9b266) + (0x8e2ba989));
    #endif
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_0) == (0x0)) {
        rb_state1.var_11 = 0x933fee16;
        rb_state1.var_12 = (rb_state1.var_12) ^ (0x8a2aae0f);
        rb_state1.var_13 = (rb_state1.var_13) + (((((0x66c58b09) + (rb_state1.var_7)) - (rb_state1.var_11)) ^ (0x835cd41)) + (0x5fa80b5b));
        rb_state1.var_14 = (rb_state1.var_14) - (((((0x65882391) - (rb_state1.var_12)) - (0x188a4918)) - (rb_state1.var_6)) + (rb_state1.var_8));
        rb_state1.var_15 = (rb_state1.var_15) ^ (((0x828c04a0) ^ (rb_state1.var_9)) ^ (rb_state1.var_13));
        rb_state1.var_16 = (rb_state1.var_16) + (((0xc5092cb1) + (0xe338e2c3)) + (rb_state1.var_14));
        rb_state1.var_17 = (rb_state1.var_17) + (((((0xe003d7a6) ^ (rb_state1.var_15)) ^ (rb_state1.var_16)) + (0xa6f85d9d)) + (rb_state1.var_10));
        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_17);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x7d555b6e)) {
        rb_state2.var_3 = 0x43d75199;
        rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_2);
        rb_state2.var_5 = (rb_state2.var_5) ^ (rb_state2.var_4);
        rb_state2.var_6 = (rb_state2.var_6) ^ (58955 < rb_input_size ? (uint32_t)rb_input[58955] : 0x6f26268e);
        rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_3) == (0x0)) ? (rb_state2.var_4) : (0xef496c2c));
        rb_state2.var_8 = (rb_state2.var_8) + (rb_state2.var_5);
        rb_state2.var_9 = (rb_state2.var_9) - (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0xc03f80f));
        rb_state2.var_10 = (rb_state2.var_10) + (((0x422b93c3) + (rb_state2.var_6)) ^ (rb_state2.var_7));
        rb_state2.var_11 = (rb_state2.var_11) - (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0xf65a225e));
        rb_state2.var_12 = (rb_state2.var_12) + (rb_state2.var_9);
        rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0xeef9d20d));
        rb_state2.var_14 = (rb_state2.var_14) - (((0xee7cca1e) ^ (rb_state2.var_12)) - (0x77137b09));
        rb_state2.var_15 = (rb_state2.var_15) + (rb_state2.var_13);
        rb_state2.var_16 = (rb_state2.var_16) - (((rb_state2.var_14) == (0x0)) ? (rb_state2.var_15) : (0x4df6ff44));
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_16);
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0x92)) {
        pthread_mutex_lock(&(rb_state4.lock_20));
        rb_state4.var_11 = 0xd2b8a11f;
        rb_state4.var_12 = 0x286dc916;
        rb_state4.var_13 = (rb_state4.var_13) + (((0xb76859f) - (0xd4e42248)) ^ (rb_state4.var_11));
        rb_state4.var_14 = (rb_state4.var_14) - (0x90be48a);
        rb_state4.var_15 = (rb_state4.var_15) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_12) : (0xd233a79e));
        rb_state4.var_16 = (rb_state4.var_16) + (((rb_state4.var_13) == (0xe42ac248)) ? (rb_state4.var_14) : (0xd684a959));
        rb_state4.var_17 = (rb_state4.var_17) ^ (((rb_state4.var_9) == (0x2e17e4d1)) ? (rb_state4.var_15) : (0xa99f7b5c));
        rb_state4.var_18 = (rb_state4.var_18) + (((0x8122a7ed) + (rb_state4.var_10)) ^ (rb_state4.var_16));
        rb_state4.var_19 = (rb_state4.var_19) - (((((0x4a62a64f) - (rb_state4.var_11)) ^ (rb_state4.var_18)) - (rb_state4.var_17)) - (0xf51b9a2d));
        rb_state4.var_1 = (rb_state4.var_1) ^ (rb_state4.var_19);
        pthread_mutex_unlock(&(rb_state4.lock_20));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_0) == (0x61a43288)) {
        pthread_mutex_lock(&(rb_state6.lock_14));
        rb_state6.var_3 = (rb_state6.var_3) ^ (rb_state6.var_1);
        rb_state6.var_4 = (rb_state6.var_4) + (((((0xc269c3f5) - (rb_state6.var_3)) + (0xe64e064d)) - (rb_state6.var_2)) ^ (rb_state6.var_3));
        rb_state6.var_2 = (rb_state6.var_2) - (rb_state6.var_4);
        rb_state6.var_1 = rb_state6.var_2;
        pthread_mutex_unlock(&(rb_state6.lock_14));
    }
    if ((rb_state6.var_0) == (0x61a43288)) {
        pthread_mutex_lock(&(rb_state6.lock_14));
        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
            racebench_trigger(6);
        }
        pthread_mutex_unlock(&(rb_state6.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_14));
        rb_state7.var_3 = (rb_state7.var_3) ^ (0x598bdde1);
        rb_state7.var_4 = (rb_state7.var_4) + (((((0x3381e369) ^ (0xbc5a1544)) ^ (rb_state7.var_3)) ^ (rb_state7.var_2)) - (rb_state7.var_1));
        rb_state7.var_5 = (rb_state7.var_5) + (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_3) : (0x7239bd9f));
        rb_state7.var_6 = (rb_state7.var_6) - (rb_state7.var_5);
        rb_state7.var_7 = (rb_state7.var_7) + (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_4) : (0x2a6a8071));
        rb_state7.var_8 = (rb_state7.var_8) - (rb_state7.var_5);
        rb_state7.var_9 = (rb_state7.var_9) + (rb_state7.var_6);
        rb_state7.var_10 = (rb_state7.var_10) + (rb_state7.var_7);
        rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_8) : (0xf55b6a61));
        rb_state7.var_12 = (rb_state7.var_12) + (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0x173b8e3));
        rb_state7.var_13 = (rb_state7.var_13) ^ (((((0x7ff69352) - (rb_state7.var_8)) + (rb_state7.var_11)) - (rb_state7.var_12)) ^ (rb_state7.var_9));
        rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_13);
        pthread_mutex_unlock(&(rb_state7.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0xe938567)) {
        rb_state8.var_2 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0xe938567)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0x8b0f9b1a);
    if ((rb_state16.var_0) == (0x3d06460a)) {
        rb_state16.var_9 = (rb_state16.var_9) - (rb_state16.var_8);
        rb_state16.var_10 = (rb_state16.var_10) - (((0xdc86b4f9) - (0xa36382d)) + (rb_state16.var_6));
        rb_state16.var_11 = (rb_state16.var_11) ^ (rb_state16.var_5);
        rb_state16.var_12 = (rb_state16.var_12) - (0xa798bb44);
        rb_state16.var_13 = (rb_state16.var_13) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_7) : (0xa75c4c01));
        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0x92f3c73b));
        rb_state16.var_15 = (rb_state16.var_15) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0xa4777b2d));
        rb_state16.var_16 = (rb_state16.var_16) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_12) : (0x950a5867));
        rb_state16.var_17 = (rb_state16.var_17) - (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x4e1cf591));
        rb_state16.var_18 = (rb_state16.var_18) ^ (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_14) : (0x5ac59fbf));
        rb_state16.var_19 = (rb_state16.var_19) ^ (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_16) : (0xb15a6606));
        rb_state16.var_20 = (rb_state16.var_20) + (((((0x9b9f4a33) - (0x87d9a2a6)) - (rb_state16.var_18)) + (0x155db2c9)) ^ (rb_state16.var_17));
        rb_state16.var_21 = (rb_state16.var_21) - (((0x20b56e2c) - (0xda51f23f)) - (rb_state16.var_19));
        rb_state16.var_22 = (rb_state16.var_22) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_20) : (0x352bac0b));
        rb_state16.var_23 = (rb_state16.var_23) ^ (rb_state16.var_21);
        rb_state16.var_24 = (rb_state16.var_24) + (((rb_state16.var_15) == (0x0)) ? (rb_state16.var_22) : (0x2923fe9c));
        rb_state16.var_25 = (rb_state16.var_25) + (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_24) : (0x95ce830c));
        rb_state16.var_1 = (rb_state16.var_1) + (rb_state16.var_25);
    }
    rb_state16.var_16 = (rb_state16.var_16) - (rb_state16.var_17);
    if ((rb_state16.var_18) == (0x5ac59fbf)) {
        pthread_mutex_lock(&(rb_state16.lock_59));
        rb_state16.var_30 = (rb_state16.var_30) - (((0x6bf43b32) ^ (rb_state16.var_20)) - (0x11dcf2c8));
        rb_state16.var_31 = (rb_state16.var_31) - (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_21) : (0xabbece93));
        rb_state16.var_32 = (rb_state16.var_32) - (rb_state16.var_22);
        rb_state16.var_33 = (rb_state16.var_33) - (rb_state16.var_23);
        rb_state16.var_34 = (rb_state16.var_34) + (((((0x57cbd9cb) - (rb_state16.var_30)) ^ (0xb02f8069)) - (0x12dbdd1)) + (rb_state16.var_31));
        rb_state16.var_35 = (rb_state16.var_35) + (rb_state16.var_32);
        rb_state16.var_36 = (rb_state16.var_36) - (rb_state16.var_33);
        rb_state16.var_37 = (rb_state16.var_37) + (((0xe2f1f729) + (0x6d7c4b62)) - (rb_state16.var_34));
        rb_state16.var_38 = (rb_state16.var_38) ^ (((((0x2f2d7c43) + (rb_state16.var_36)) ^ (rb_state16.var_35)) ^ (0xc0c6c6c6)) + (rb_state16.var_24));
        rb_state16.var_39 = (rb_state16.var_39) + (((0x79d82044) - (rb_state16.var_37)) ^ (0x7f9f9283));
        rb_state16.var_40 = (rb_state16.var_40) ^ (((0x8ceab62d) - (rb_state16.var_38)) + (rb_state16.var_25));
        rb_state16.var_41 = (rb_state16.var_41) - (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_39) : (0xd8c02229));
        rb_state16.var_42 = (rb_state16.var_42) ^ (((((0x1fed992) + (rb_state16.var_41)) - (rb_state16.var_40)) ^ (0x4812a68b)) ^ (0x5b0bd468));
        rb_state16.var_29 = (rb_state16.var_29) ^ (rb_state16.var_42);
        rb_state16.var_28 = rb_state16.var_29;
        pthread_mutex_unlock(&(rb_state16.lock_59));
    }
    if ((rb_state16.var_18) == (0x5ac59fbf)) {
        rb_state16.var_43 = 0xb6fbec1a;
        rb_state16.var_44 = (rb_state16.var_44) ^ (rb_state16.var_30);
        rb_state16.var_45 = (rb_state16.var_45) + (0x36fec7b);
        rb_state16.var_46 = (rb_state16.var_46) + (((0x4aecb600) - (0x10ffccfa)) - (rb_state16.var_29));
        rb_state16.var_47 = (rb_state16.var_47) - (((rb_state16.var_31) == (0x0)) ? (rb_state16.var_28) : (0x380ea02));
        rb_state16.var_48 = (rb_state16.var_48) ^ (rb_state16.var_43);
        rb_state16.var_49 = (rb_state16.var_49) - (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_44) : (0x471937fd));
        rb_state16.var_50 = (rb_state16.var_50) ^ (((rb_state16.var_33) == (0x0)) ? (rb_state16.var_45) : (0x427c9a9a));
        rb_state16.var_51 = (rb_state16.var_51) ^ (((rb_state16.var_46) == (0x0)) ? (rb_state16.var_47) : (0x7f1ccca9));
        rb_state16.var_52 = (rb_state16.var_52) - (rb_state16.var_48);
        rb_state16.var_53 = (rb_state16.var_53) + (((rb_state16.var_49) == (0x0)) ? (rb_state16.var_50) : (0x4dc1cef6));
        rb_state16.var_54 = (rb_state16.var_54) + (((rb_state16.var_34) == (0x0)) ? (rb_state16.var_51) : (0xb440f933));
        rb_state16.var_55 = (rb_state16.var_55) ^ (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_52) : (0x2fe4a7bc));
        rb_state16.var_56 = (rb_state16.var_56) + (((rb_state16.var_53) == (0x0)) ? (rb_state16.var_54) : (0x732f3106));
        rb_state16.var_57 = (rb_state16.var_57) ^ (((0x61092a26) ^ (0x4724408c)) - (rb_state16.var_55));
        rb_state16.var_58 = (rb_state16.var_58) - (((rb_state16.var_56) == (0x0)) ? (rb_state16.var_57) : (0xe29b7939));
        rb_state16.var_28 = (rb_state16.var_28) ^ (rb_state16.var_58);
    }
    if ((rb_state16.var_18) == (0x5ac59fbf)) {
        if (!((rb_state16.var_28) == (rb_state16.var_29))) {
            racebench_trigger(16);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (50617 < rb_input_size ? (uint32_t)rb_input[50617] : 0xb525a3bd);
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
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - ((0x979b5fab) + (0xeb0f44e7));
        if ((rb_state2.var_0) == (0x7d555b6e)) {
            rb_state2.var_2 = rb_state2.var_1;
        }
        if ((rb_state2.var_0) == (0x7d555b6e)) {
            if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_0) == (0x61a43288)) {
            rb_state6.var_5 = 0x48c4b768;
            rb_state6.var_6 = 0xf1ce0aea;
            rb_state6.var_7 = (rb_state6.var_7) - (52184 < rb_input_size ? (uint32_t)rb_input[52184] : 0xaa27555b);
            rb_state6.var_8 = (rb_state6.var_8) - (((((0x1855cd85) - (rb_state6.var_4)) ^ (0x65436131)) ^ (rb_state6.var_5)) ^ (0x8f34cdcf));
            rb_state6.var_9 = (rb_state6.var_9) - (rb_state6.var_6);
            rb_state6.var_10 = (rb_state6.var_10) + (rb_state6.var_7);
            rb_state6.var_11 = (rb_state6.var_11) - (((((0xcc72f50a) ^ (rb_state6.var_9)) + (rb_state6.var_6)) - (rb_state6.var_8)) + (rb_state6.var_5));
            rb_state6.var_12 = (rb_state6.var_12) + (((0x3d342bd7) + (0xf25e6b63)) - (rb_state6.var_10));
            rb_state6.var_13 = (rb_state6.var_13) - (((((0x5884cb36) ^ (rb_state6.var_12)) - (rb_state6.var_11)) - (rb_state6.var_7)) + (rb_state6.var_8));
            rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_13);
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_0);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_0);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
        #endif
        arg[i].points = points;
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0x5fed6f4c);
        if ((rb_state3.var_0) == (0xa01290b4)) {
            rb_state3.var_3 = 0xb4c939ad;
            rb_state3.var_4 = (rb_state3.var_4) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_4) : (0x75e5ade2));
            rb_state3.var_5 = (rb_state3.var_5) - (rb_state3.var_3);
            rb_state3.var_6 = (rb_state3.var_6) ^ (rb_state3.var_5);
            rb_state3.var_7 = (rb_state3.var_7) - (((rb_state3.var_2) == (0x0)) ? (rb_state3.var_4) : (0x5555f163));
            rb_state3.var_8 = (rb_state3.var_8) + (((0x19686f28) + (rb_state3.var_5)) + (0x3ae7a697));
            rb_state3.var_9 = (rb_state3.var_9) + (((0x9b2f141) ^ (0x7a9effe)) - (rb_state3.var_6));
            rb_state3.var_10 = (rb_state3.var_10) - (rb_state3.var_7);
            rb_state3.var_11 = (rb_state3.var_11) + (((((0xe3caf4fd) - (0x5752468e)) + (rb_state3.var_9)) - (rb_state3.var_8)) + (0x2729c1));
            rb_state3.var_12 = (rb_state3.var_12) + (((0xc266f517) + (rb_state3.var_6)) - (rb_state3.var_10));
            rb_state3.var_13 = (rb_state3.var_13) - (((rb_state3.var_7) == (0x0)) ? (rb_state3.var_11) : (0x7c8591cd));
            rb_state3.var_14 = (rb_state3.var_14) + (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_12) : (0x6dfa7fc8));
            rb_state3.var_15 = (rb_state3.var_15) + (rb_state3.var_13);
            rb_state3.var_16 = (rb_state3.var_16) ^ (rb_state3.var_14);
            rb_state3.var_17 = (rb_state3.var_17) + (rb_state3.var_15);
            rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_16) : (0xecca7e74));
            rb_state3.var_19 = (rb_state3.var_19) + (((0x19adb1c) - (0x65ce0dd9)) - (rb_state3.var_17));
            rb_state3.var_20 = (rb_state3.var_20) + (rb_state3.var_18);
            rb_state3.var_21 = (rb_state3.var_21) - (rb_state3.var_19);
            rb_state3.var_22 = (rb_state3.var_22) ^ (((((0x26fb0f23) - (rb_state3.var_21)) + (rb_state3.var_11)) - (rb_state3.var_20)) ^ (rb_state3.var_10));
            rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_22);
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (39300 < rb_input_size ? (uint32_t)rb_input[39300] : 0x169cd21d);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + (0x6671c83f);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (63520 < rb_input_size ? (uint32_t)rb_input[63520] : 0xd5153eae);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
        if ((rb_state13.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_40));
            rb_state13.var_9 = (rb_state13.var_9) + ((0x94663c9e) - (0x5c268951));
            rb_state13.var_10 = (rb_state13.var_10) ^ (0x6315b13b);
            rb_state13.var_11 = (rb_state13.var_11) ^ (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_8) : (0xa83dc0b0));
            rb_state13.var_12 = (rb_state13.var_12) ^ (((0x1f1f0b9e) + (rb_state13.var_7)) + (0xfea6c871));
            rb_state13.var_13 = (rb_state13.var_13) - (((rb_state13.var_6) == (0x0)) ? (rb_state13.var_9) : (0xa2b00bb0));
            rb_state13.var_14 = (rb_state13.var_14) ^ (((0x4bb30f35) ^ (rb_state13.var_10)) - (rb_state13.var_10));
            rb_state13.var_15 = (rb_state13.var_15) ^ (((0xc2701f9a) ^ (rb_state13.var_11)) ^ (rb_state13.var_11));
            rb_state13.var_16 = (rb_state13.var_16) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_13) : (0xa9034855));
            rb_state13.var_17 = (rb_state13.var_17) ^ (((((0x61bac0d1) + (rb_state13.var_15)) - (rb_state13.var_12)) ^ (rb_state13.var_13)) - (rb_state13.var_14));
            rb_state13.var_18 = (rb_state13.var_18) - (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_17) : (0x1787b0a9));
            rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_18);
            rb_state13.var_1 = rb_state13.var_8;
            pthread_mutex_unlock(&(rb_state13.lock_40));
        }
        if ((rb_state13.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state13.lock_40));
            rb_state13.var_20 = 0x4d1834bb;
            rb_state13.var_21 = 0x19b06346;
            rb_state13.var_22 = (rb_state13.var_22) ^ (((0xe9c1df58) + (rb_state13.var_15)) + (rb_state13.var_20));
            rb_state13.var_23 = (rb_state13.var_23) - (10687 < rb_input_size ? (uint32_t)rb_input[10687] : 0xffc0acfb);
            rb_state13.var_24 = (rb_state13.var_24) ^ (rb_state13.var_14);
            rb_state13.var_25 = (rb_state13.var_25) ^ (((0x852f7c97) + (rb_state13.var_21)) ^ (0xaf370f5a));
            rb_state13.var_26 = (rb_state13.var_26) + (rb_state13.var_22);
            rb_state13.var_27 = (rb_state13.var_27) + (rb_state13.var_23);
            rb_state13.var_28 = (rb_state13.var_28) ^ (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_24) : (0xad3a34ba));
            rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_25);
            rb_state13.var_30 = (rb_state13.var_30) + (((0x6fecc27a) - (rb_state13.var_26)) - (rb_state13.var_17));
            rb_state13.var_31 = (rb_state13.var_31) + (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_27) : (0x1060d249));
            rb_state13.var_32 = (rb_state13.var_32) - (((0x29cc92cb) + (0x3165a368)) + (rb_state13.var_28));
            rb_state13.var_33 = (rb_state13.var_33) ^ (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_29) : (0xb866217f));
            rb_state13.var_34 = (rb_state13.var_34) + (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0x780ebbbd));
            rb_state13.var_35 = (rb_state13.var_35) - (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_32) : (0x69b81e50));
            rb_state13.var_36 = (rb_state13.var_36) - (((0xc7a68204) ^ (rb_state13.var_21)) - (rb_state13.var_33));
            rb_state13.var_37 = (rb_state13.var_37) + (((0x41666371) + (0xfa980399)) - (rb_state13.var_34));
            rb_state13.var_38 = (rb_state13.var_38) - (((rb_state13.var_35) == (0x0)) ? (rb_state13.var_36) : (0xc3512d3f));
            rb_state13.var_39 = (rb_state13.var_39) ^ (((rb_state13.var_37) == (0x0)) ? (rb_state13.var_38) : (0xe8e44089));
            rb_state13.var_19 = (rb_state13.var_19) + (rb_state13.var_39);
            rb_state13.var_1 = rb_state13.var_19;
            pthread_mutex_unlock(&(rb_state13.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_0) == (0x0)) {
            if ((rb_state15.var_1) != (0x0)) {
                if (!((rb_state15.var_1) == (rb_state15.var_25))) {
                    racebench_trigger(15);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0xd2c2e7d6)) {
            rb_state17.var_3 = (rb_state17.var_3) ^ (((0x29a337b0) - (rb_state17.var_1)) - (rb_state17.var_3));
            rb_state17.var_4 = (rb_state17.var_4) + (((0x58c9b4bc) - (rb_state17.var_2)) - (rb_state17.var_4));
            rb_state17.var_5 = (rb_state17.var_5) ^ (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_5) : (0xba96a9e5));
            rb_state17.var_6 = (rb_state17.var_6) - (49632 < rb_input_size ? (uint32_t)rb_input[49632] : 0xe0ed7595);
            rb_state17.var_7 = (rb_state17.var_7) - (((0x4a0d19a0) ^ (rb_state17.var_6)) + (rb_state17.var_3));
            rb_state17.var_8 = (rb_state17.var_8) ^ (((((0x5528888a) + (rb_state17.var_5)) + (0xa5f666a5)) + (0xe215beb1)) - (rb_state17.var_4));
            rb_state17.var_9 = (rb_state17.var_9) - (rb_state17.var_6);
            rb_state17.var_10 = (rb_state17.var_10) + (rb_state17.var_7);
            rb_state17.var_11 = (rb_state17.var_11) + (rb_state17.var_8);
            rb_state17.var_12 = (rb_state17.var_12) ^ (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_9) : (0xf59d51a9));
            rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_10);
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_11) : (0x3bb4835e));
            rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_12) : (0x5a02c8c2));
            rb_state17.var_16 = (rb_state17.var_16) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0x8fba178a));
            rb_state17.var_17 = (rb_state17.var_17) - (((0x70318938) + (0xaac11a2f)) - (rb_state17.var_15));
            rb_state17.var_18 = (rb_state17.var_18) ^ (rb_state17.var_16);
            rb_state17.var_19 = (rb_state17.var_19) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_17) : (0x84004abe));
            rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_18);
            rb_state17.var_21 = (rb_state17.var_21) ^ (((0x351a18fd) + (rb_state17.var_19)) + (rb_state17.var_11));
            rb_state17.var_22 = (rb_state17.var_22) ^ (rb_state17.var_20);
            rb_state17.var_23 = (rb_state17.var_23) - (((0xbbe0d25c) + (rb_state17.var_12)) ^ (rb_state17.var_21));
            rb_state17.var_24 = (rb_state17.var_24) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_22) : (0xbfd6c08c));
            rb_state17.var_25 = (rb_state17.var_25) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_23) : (0xea4d4565));
            rb_state17.var_26 = (rb_state17.var_26) ^ (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_25) : (0x4e7448c5));
            rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_26);
        }
        #endif
        arg[i].kmin = kmin;
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0x92)) {
            pthread_mutex_lock(&(rb_state4.lock_20));
            rb_state4.var_3 = 0x86f38096;
            rb_state4.var_4 = (rb_state4.var_4) + (((0x4d2da281) + (rb_state4.var_3)) ^ (rb_state4.var_1));
            rb_state4.var_5 = (rb_state4.var_5) - (((rb_state4.var_4) == (0xd4212317)) ? (rb_state4.var_2) : (0x16773cab));
            rb_state4.var_6 = (rb_state4.var_6) ^ (rb_state4.var_5);
            rb_state4.var_7 = (rb_state4.var_7) + (((((0xb96109aa) ^ (0x45c4102a)) - (rb_state4.var_3)) ^ (rb_state4.var_6)) - (rb_state4.var_4));
            rb_state4.var_8 = (rb_state4.var_8) ^ (rb_state4.var_5);
            rb_state4.var_9 = (rb_state4.var_9) ^ (((((0x52159a6) - (rb_state4.var_7)) - (0xd70974d5)) + (rb_state4.var_7)) + (rb_state4.var_6));
            rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0xd2de07f9));
            rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_10);
            rb_state4.var_1 = rb_state4.var_2;
            pthread_mutex_unlock(&(rb_state4.lock_20));
        }
        if ((rb_state4.var_0) == (0x92)) {
            if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0xdc14b572) - (0xd61b4370));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + ((0x202d0160) - (rb_state9.var_0));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x69621229));
        #endif
        #ifdef RACEBENCH_BUG_12
        if ((rb_state12.var_0) == (0x0)) {
            if ((rb_state12.var_1) != (0x0)) {
                if (!((rb_state12.var_1) == (rb_state12.var_12))) {
                    racebench_trigger(12);
                }
            }
        }
        #endif
        arg[i].kmax = kmax;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state1.lock_18));
            rb_state1.var_3 = 0xff574d5c;
            rb_state1.var_4 = (rb_state1.var_4) - (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_3) : (0x391abf7a));
            rb_state1.var_5 = (rb_state1.var_5) - (((0xa4bb4b5c) + (0xfd182cd9)) + (rb_state1.var_2));
            rb_state1.var_6 = (rb_state1.var_6) ^ (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_1) : (0xa7441125));
            rb_state1.var_7 = (rb_state1.var_7) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_4) : (0x4ea2eee9));
            rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_6) : (0x1e51dfce));
            rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_7);
            rb_state1.var_10 = (rb_state1.var_10) + (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_9) : (0x33480dbc));
            rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_10);
            rb_state1.var_1 = rb_state1.var_2;
            pthread_mutex_unlock(&(rb_state1.lock_18));
        }
        if ((rb_state1.var_0) == (0x0)) {
            if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (((rb_state3.var_0) == (0xa01290b4)) ? (rb_state3.var_0) : (0xe5d19469));
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x611390d4)) {
            pthread_mutex_lock(&(rb_state5.lock_33));
            rb_state5.var_3 = 0x75ae641d;
            rb_state5.var_4 = 0x88d5d85f;
            rb_state5.var_5 = (rb_state5.var_5) - ((0xaafcfa51) - (rb_state5.var_2));
            rb_state5.var_6 = (rb_state5.var_6) ^ (rb_state5.var_3);
            rb_state5.var_7 = (rb_state5.var_7) - (rb_state5.var_3);
            rb_state5.var_8 = (rb_state5.var_8) - (((0xdeb87ed8) - (rb_state5.var_1)) - (rb_state5.var_4));
            rb_state5.var_9 = (rb_state5.var_9) - (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_4) : (0x9e1dda3));
            rb_state5.var_10 = (rb_state5.var_10) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_5) : (0x2c997281));
            rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0xf14d241));
            rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_7) : (0xb56a9634));
            rb_state5.var_13 = (rb_state5.var_13) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x38fac));
            rb_state5.var_14 = (rb_state5.var_14) + (((((0xe15b5226) ^ (0x6d260ee)) ^ (0x3facfa33)) ^ (rb_state5.var_11)) + (rb_state5.var_10));
            rb_state5.var_15 = (rb_state5.var_15) ^ (((rb_state5.var_9) == (0x0)) ? (rb_state5.var_12) : (0x6313a801));
            rb_state5.var_16 = (rb_state5.var_16) - (rb_state5.var_13);
            rb_state5.var_17 = (rb_state5.var_17) ^ (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_14) : (0x165c0ffc));
            rb_state5.var_18 = (rb_state5.var_18) ^ (((0xa078b663) - (rb_state5.var_15)) ^ (0xfe674951));
            rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_16) == (0x0)) ? (rb_state5.var_17) : (0x72d8ed6a));
            rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_18);
            rb_state5.var_21 = (rb_state5.var_21) ^ (((((0x21278ba5) ^ (rb_state5.var_20)) ^ (rb_state5.var_11)) + (rb_state5.var_19)) ^ (0x1a770f94));
            rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_21);
            rb_state5.var_1 = rb_state5.var_2;
            pthread_mutex_unlock(&(rb_state5.lock_33));
        }
        if ((rb_state5.var_0) == (0x611390d4)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x0)) {
            rb_state7.var_2 = rb_state7.var_1;
        }
        if ((rb_state7.var_0) == (0x0)) {
            if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ (0xe938567);
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0x0)) {
            rb_state10.var_14 = 0x8dee3ff5;
            rb_state10.var_15 = (rb_state10.var_15) - (0x2e56ef0e);
            rb_state10.var_16 = (rb_state10.var_16) - (rb_state10.var_4);
            rb_state10.var_17 = (rb_state10.var_17) ^ ((0xfb14fb8) - (rb_state10.var_5));
            rb_state10.var_18 = (rb_state10.var_18) + (((0xa1f20819) ^ (rb_state10.var_14)) - (0xb1846c1c));
            rb_state10.var_19 = (rb_state10.var_19) + (((((0x33e7958f) - (rb_state10.var_15)) + (0x5fb27b74)) + (rb_state10.var_6)) + (rb_state10.var_3));
            rb_state10.var_20 = (rb_state10.var_20) - (rb_state10.var_16);
            rb_state10.var_21 = (rb_state10.var_21) + (((0xeb394b7b) - (rb_state10.var_7)) - (rb_state10.var_17));
            rb_state10.var_22 = (rb_state10.var_22) ^ (rb_state10.var_18);
            rb_state10.var_23 = (rb_state10.var_23) ^ (rb_state10.var_19);
            rb_state10.var_24 = (rb_state10.var_24) + (((0x822ba0b7) + (0x1b4a4675)) - (rb_state10.var_20));
            rb_state10.var_25 = (rb_state10.var_25) - (((rb_state10.var_8) == (0x0)) ? (rb_state10.var_21) : (0xfc2d4530));
            rb_state10.var_26 = (rb_state10.var_26) - (((0x98ed64b9) - (rb_state10.var_9)) ^ (rb_state10.var_22));
            rb_state10.var_27 = (rb_state10.var_27) - (rb_state10.var_23);
            rb_state10.var_28 = (rb_state10.var_28) + (((0xeb4b6c70) - (rb_state10.var_24)) ^ (0xa4339c54));
            rb_state10.var_29 = (rb_state10.var_29) - (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_26) : (0x12ce4360));
            rb_state10.var_30 = (rb_state10.var_30) + (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_28) : (0x1ed1cc72));
            rb_state10.var_31 = (rb_state10.var_31) - (((((0x76ec1fb8) + (rb_state10.var_11)) + (rb_state10.var_10)) - (rb_state10.var_29)) - (rb_state10.var_30));
            rb_state10.var_13 = (rb_state10.var_13) - (rb_state10.var_31);
            rb_state10.var_1 = rb_state10.var_13;
        }
        if ((rb_state10.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state10.lock_40));
            rb_state10.var_33 = 0xb6fdd9ce;
            rb_state10.var_34 = 0x4cf2411b;
            rb_state10.var_35 = 0xa4f29ec5;
            rb_state10.var_36 = (rb_state10.var_36) - (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_34) : (0x1befcaa0));
            rb_state10.var_37 = (rb_state10.var_37) - (((((0xe98570da) + (rb_state10.var_13)) ^ (rb_state10.var_33)) - (rb_state10.var_35)) ^ (rb_state10.var_14));
            rb_state10.var_38 = (rb_state10.var_38) - (rb_state10.var_36);
            rb_state10.var_39 = (rb_state10.var_39) + (((rb_state10.var_37) == (0x0)) ? (rb_state10.var_38) : (0x88a60093));
            rb_state10.var_32 = (rb_state10.var_32) - (rb_state10.var_39);
            rb_state10.var_1 = rb_state10.var_32;
            pthread_mutex_unlock(&(rb_state10.lock_40));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) + (0x93519477);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0xc7caee06));
        #endif
        arg[i].pid = i;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - ((0x6b7a7ba5) + (0xd61fad4f));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) + ((0x61a43288) - (rb_state6.var_0));
        #endif
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_0) == (0xe938567)) {
            pthread_mutex_lock(&(rb_state8.lock_20));
            rb_state8.var_3 = 0xce58c1e1;
            rb_state8.var_4 = 0x1638227e;
            rb_state8.var_5 = (rb_state8.var_5) - ((0x942ba196) ^ (0x6e11d749));
            rb_state8.var_6 = (rb_state8.var_6) + (((rb_state8.var_4) == (0x1638227e)) ? (rb_state8.var_3) : (0xe2871a18));
            rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_5) == (0x5c58921)) ? (rb_state8.var_2) : (0x6245b713));
            rb_state8.var_8 = (rb_state8.var_8) - (((0xf412a0a1) + (rb_state8.var_6)) ^ (rb_state8.var_4));
            rb_state8.var_9 = (rb_state8.var_9) + (rb_state8.var_3);
            rb_state8.var_10 = (rb_state8.var_10) + (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_5) : (0x492dffe2));
            rb_state8.var_11 = (rb_state8.var_11) - (rb_state8.var_6);
            rb_state8.var_12 = (rb_state8.var_12) - (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_8) : (0xf2f19432));
            rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_8) == (0x2bacbf04)) ? (rb_state8.var_9) : (0x1788c17c));
            rb_state8.var_14 = (rb_state8.var_14) - (((((0x3ae6084b) + (0x2bbcc11c)) + (rb_state8.var_10)) - (rb_state8.var_11)) + (rb_state8.var_9));
            rb_state8.var_15 = (rb_state8.var_15) ^ (rb_state8.var_12);
            rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_13);
            rb_state8.var_17 = (rb_state8.var_17) + (((rb_state8.var_10) == (0x5c58921)) ? (rb_state8.var_14) : (0xbb1c0f16));
            rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_15) == (0xd45340fc)) ? (rb_state8.var_16) : (0x7b255c59));
            rb_state8.var_19 = (rb_state8.var_19) + (((((0xdc710dce) + (rb_state8.var_17)) + (rb_state8.var_11)) - (0xb820d035)) - (rb_state8.var_18));
            rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_19);
            pthread_mutex_unlock(&(rb_state8.lock_20));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x2e2da98a));
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_0) == (0x93519477)) {
            pthread_mutex_lock(&(rb_state11.lock_31));
            rb_state11.var_9 = 0x4966664c;
            rb_state11.var_10 = 0xabd762f9;
            rb_state11.var_11 = (rb_state11.var_11) + (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_10) : (0x6debd1b));
            rb_state11.var_12 = (rb_state11.var_12) - (rb_state11.var_10);
            rb_state11.var_13 = (rb_state11.var_13) ^ (rb_state11.var_9);
            rb_state11.var_14 = (rb_state11.var_14) + (((0xc75bdf5a) ^ (rb_state11.var_8)) + (0xf1d48501));
            rb_state11.var_15 = (rb_state11.var_15) ^ (((rb_state11.var_11) == (0x0)) ? (rb_state11.var_11) : (0x64e45c0e));
            rb_state11.var_16 = (rb_state11.var_16) + (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_12) : (0x675d4806));
            rb_state11.var_17 = (rb_state11.var_17) + (((((0xb3ea5ae2) ^ (rb_state11.var_14)) + (0x1a0037a9)) - (rb_state11.var_13)) ^ (rb_state11.var_13));
            rb_state11.var_18 = (rb_state11.var_18) + (rb_state11.var_15);
            rb_state11.var_19 = (rb_state11.var_19) - (((rb_state11.var_16) == (0x0)) ? (rb_state11.var_17) : (0xc7d6cf3));
            rb_state11.var_20 = (rb_state11.var_20) + (rb_state11.var_18);
            rb_state11.var_21 = (rb_state11.var_21) ^ (((rb_state11.var_19) == (0x0)) ? (rb_state11.var_20) : (0x1d13655e));
            rb_state11.var_8 = (rb_state11.var_8) - (rb_state11.var_21);
            rb_state11.var_1 = rb_state11.var_8;
            pthread_mutex_unlock(&(rb_state11.lock_31));
        }
        if ((rb_state11.var_0) == (0x93519477)) {
            pthread_mutex_lock(&(rb_state11.lock_31));
            rb_state11.var_23 = 0xd0185783;
            rb_state11.var_24 = (rb_state11.var_24) ^ (((rb_state11.var_23) == (0x0)) ? (rb_state11.var_15) : (0xbdd1278d));
            rb_state11.var_25 = (rb_state11.var_25) ^ ((0x928c913e) ^ (0x15ea6e42));
            rb_state11.var_26 = (rb_state11.var_26) ^ (rb_state11.var_14);
            rb_state11.var_27 = (rb_state11.var_27) ^ (((((0xa926f6ef) + (0x4497eaae)) ^ (rb_state11.var_24)) + (rb_state11.var_16)) ^ (rb_state11.var_25));
            rb_state11.var_28 = (rb_state11.var_28) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_26) : (0x58d06ddf));
            rb_state11.var_29 = (rb_state11.var_29) ^ (((0x2f39032f) + (rb_state11.var_27)) - (rb_state11.var_18));
            rb_state11.var_30 = (rb_state11.var_30) ^ (((((0xf44c7eb0) ^ (rb_state11.var_29)) ^ (rb_state11.var_20)) - (rb_state11.var_28)) - (rb_state11.var_19));
            rb_state11.var_22 = (rb_state11.var_22) - (rb_state11.var_30);
            rb_state11.var_1 = rb_state11.var_22;
            pthread_mutex_unlock(&(rb_state11.lock_31));
        }
        #endif
        arg[i].kfinal = kfinal;

        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_0) == (0x202d0160)) {
            pthread_mutex_lock(&(rb_state9.lock_15));
            rb_state9.var_7 = 0x2e4d6658;
            rb_state9.var_8 = 0x81108e8a;
            rb_state9.var_9 = (rb_state9.var_9) + (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_5) : (0x37ecc4a9));
            rb_state9.var_10 = (rb_state9.var_10) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_9) : (0x96eac624));
            rb_state9.var_6 = (rb_state9.var_6) + (rb_state9.var_10);
            rb_state9.var_1 = rb_state9.var_6;
            pthread_mutex_unlock(&(rb_state9.lock_15));
        }
        if ((rb_state9.var_0) == (0x202d0160)) {
            pthread_mutex_lock(&(rb_state9.lock_15));
            rb_state9.var_12 = 0xffa2724e;
            rb_state9.var_13 = (rb_state9.var_13) - (((((0x58ce45b2) ^ (0x17173386)) ^ (rb_state9.var_7)) + (rb_state9.var_12)) ^ (rb_state9.var_8));
            rb_state9.var_14 = (rb_state9.var_14) - (((((0xee51f766) - (rb_state9.var_6)) + (0xbc343078)) + (0x285f9dad)) + (rb_state9.var_13));
            rb_state9.var_11 = (rb_state9.var_11) ^ (rb_state9.var_14);
            rb_state9.var_1 = rb_state9.var_11;
            pthread_mutex_unlock(&(rb_state9.lock_15));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (rb_state18.var_0);
        rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0x706b5b49));
        #endif
        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_0 = (rb_state5.var_0) + ((0xfaa1c895) - (rb_state5.var_0));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_0 = (rb_state7.var_0) ^ ((0x6d9b5de4) + (rb_state7.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x0)) {
            rb_state18.var_3 = 0x679ce954;
            rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_4);
            rb_state18.var_5 = (rb_state18.var_5) ^ (rb_state18.var_3);
            rb_state18.var_6 = (rb_state18.var_6) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_4) : (0x745a95af));
            rb_state18.var_7 = (rb_state18.var_7) - (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_5) : (0xcad5d981));
            rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_6) : (0x783bd9ad));
            rb_state18.var_9 = (rb_state18.var_9) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_8) : (0xc2b7664d));
            rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_9);
        }
        #endif
        pthread_create(threads + i, NULL, localSearchSub, (void *)&arg[i]);
#else
        localSearchSub(&arg[0]);
#endif
    }

#ifdef ENABLE_THREADS
    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_0
        if ((rb_state0.var_0) == (0xe9d91e8)) {
            pthread_mutex_lock(&(rb_state0.lock_18));
            rb_state0.var_3 = 0xc8a41e9e;
            rb_state0.var_4 = 0xdafda9af;
            rb_state0.var_5 = (rb_state0.var_5) - (((rb_state0.var_4) == (0xdafda9af)) ? (rb_state0.var_1) : (0x341ae655));
            rb_state0.var_6 = (rb_state0.var_6) - (((((0x742b14bd) + (0x135403f6)) + (rb_state0.var_5)) + (rb_state0.var_3)) + (rb_state0.var_2));
            rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_6);
            rb_state0.var_1 = rb_state0.var_2;
            pthread_mutex_unlock(&(rb_state0.lock_18));
        }
        if ((rb_state0.var_0) == (0xe9d91e8)) {
            if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                racebench_trigger(0);
            }
        }
        #endif
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