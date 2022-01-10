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

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0x82a51855)) {
        rb_state19.var_20 = 0x2e2e9d63;
        rb_state19.var_21 = 0xedf9d053;
        rb_state19.var_22 = 0x6a154b2e;
        rb_state19.var_23 = (rb_state19.var_23) + (((0x97685063) ^ (0x40cd6138)) ^ (rb_state19.var_21));
        rb_state19.var_24 = (rb_state19.var_24) + (((0x8ca60ff9) ^ (0x2e4d72ca)) ^ (rb_state19.var_22));
        rb_state19.var_25 = (rb_state19.var_25) ^ (((0xe0baf328) - (rb_state19.var_11)) - (rb_state19.var_20));
        rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_23);
        rb_state19.var_27 = (rb_state19.var_27) + (rb_state19.var_24);
        rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_25) : (0xbf778348));
        rb_state19.var_29 = (rb_state19.var_29) ^ (((((0xa25c0f84) - (0x527c6691)) ^ (rb_state19.var_13)) - (rb_state19.var_27)) + (rb_state19.var_26));
        rb_state19.var_30 = (rb_state19.var_30) ^ (((((0x5db32fcd) - (rb_state19.var_29)) ^ (0xffee5561)) - (rb_state19.var_28)) + (rb_state19.var_14));
        rb_state19.var_19 = (rb_state19.var_19) + (rb_state19.var_30);
        rb_state19.var_2 = rb_state19.var_19;
    }
    if ((rb_state19.var_1) == (0xb68d183)) {
        pthread_mutex_lock(&(rb_state19.lock_40));
        rb_state19.var_32 = (rb_state19.var_32) ^ (((0xa093d1cb) + (rb_state19.var_17)) + (rb_state19.var_18));
        rb_state19.var_33 = (rb_state19.var_33) - (((((0xee60e2ca) ^ (rb_state19.var_19)) ^ (rb_state19.var_16)) + (rb_state19.var_15)) + (rb_state19.var_20));
        rb_state19.var_34 = (rb_state19.var_34) + (0x854821ae);
        rb_state19.var_35 = (rb_state19.var_35) ^ (((((0xcefcf38a) - (0x4bdb2028)) - (rb_state19.var_21)) - (rb_state19.var_32)) ^ (rb_state19.var_33));
        rb_state19.var_36 = (rb_state19.var_36) - (((0x932cb12e) - (rb_state19.var_22)) + (rb_state19.var_34));
        rb_state19.var_37 = (rb_state19.var_37) ^ (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_35) : (0x7064d354));
        rb_state19.var_38 = (rb_state19.var_38) + (rb_state19.var_36);
        rb_state19.var_39 = (rb_state19.var_39) + (((((0x14a2fcda) ^ (0x23485ccb)) - (rb_state19.var_24)) ^ (rb_state19.var_38)) ^ (rb_state19.var_37));
        rb_state19.var_31 = (rb_state19.var_31) ^ (rb_state19.var_39);
        rb_state19.var_2 = rb_state19.var_31;
        pthread_mutex_unlock(&(rb_state19.lock_40));
    }
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
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_1 = (rb_state19.var_1) ^ (((rb_state19.var_1) == (0x4110341b)) ? (rb_state19.var_0) : (0x89cdc92a));
        rb_state19.var_0 = (rb_state19.var_0) ^ (0xffb1a75d);
        rb_state19.var_1 = (rb_state19.var_1) + (31638 < rb_input_size ? (uint32_t)rb_input[31638] : 0x19a3831e);
        if ((rb_state19.var_1) == (0xb68d183)) {
            pthread_mutex_lock(&(rb_state19.lock_40));
            if ((rb_state19.var_2) != (0x0)) {
                if (!((rb_state19.var_2) == (rb_state19.var_31))) {
                    racebench_trigger(19);
                }
            }
            pthread_mutex_unlock(&(rb_state19.lock_40));
        }
        #endif
        points->p[k].cost = distance * points->p[k].weight;
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) + (13799 < rb_input_size ? (uint32_t)rb_input[13799] : 0x5fa99dba);
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
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_1 = (rb_state17.var_1) + (rb_state17.var_0);
    #endif
    hiz = loz = 0.0;
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_1);
    #endif
    long numberOfPoints = points->num;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - (0xc139394a);
    rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_1);
    #endif
    long ptDimension = points->dim;

    long bsize = points->num / nproc;
    long k1 = bsize * pid;
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_1) : (0xfff1caef));
    #endif
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        k2 = points->num;
    }

#ifdef ENABLE_THREADS
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x333d9347));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - ((0xa4ddcf78) - (0xddea49a2));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) - (rb_state9.var_0);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_1 = (rb_state10.var_1) + ((0xb4572da2) - (rb_state10.var_0));
    #endif
    pthread_barrier_wait(barrier);
#endif

    double myhiz = 0;
    for (long kk = k1; kk < k2; kk++) {
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - (24841 < rb_input_size ? (uint32_t)rb_input[24841] : 0x42639ee9);
        rb_state0.var_1 = (rb_state0.var_1) + (0x1fd673d6);
        rb_state0.var_1 = (rb_state0.var_1) ^ (0xe928a762);
        rb_state0.var_0 = (rb_state0.var_0) - (38072 < rb_input_size ? (uint32_t)rb_input[38072] : 0x858fc56a);
        rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_0);
        if ((rb_state0.var_1) == (0xedc3cd64)) {
            pthread_mutex_lock(&(rb_state0.lock_30));
            rb_state0.var_8 = 0xb31c0ded;
            rb_state0.var_9 = (rb_state0.var_9) ^ (((((0x82745ff2) ^ (rb_state0.var_7)) + (rb_state0.var_5)) - (rb_state0.var_6)) ^ (0x7b8f0d79));
            rb_state0.var_10 = (rb_state0.var_10) + (rb_state0.var_8);
            rb_state0.var_11 = (rb_state0.var_11) ^ (((((0xc6b5667e) - (0x427de830)) ^ (rb_state0.var_10)) ^ (rb_state0.var_9)) ^ (rb_state0.var_8));
            rb_state0.var_7 = (rb_state0.var_7) ^ (rb_state0.var_11);
            rb_state0.var_2 = rb_state0.var_7;
            pthread_mutex_unlock(&(rb_state0.lock_30));
        }
        if ((rb_state0.var_1) == (0xe4b1fa05)) {
            pthread_mutex_lock(&(rb_state0.lock_30));
            if ((rb_state0.var_2) != (0x0)) {
                if (!((rb_state0.var_2) == (rb_state0.var_12))) {
                    racebench_trigger(0);
                }
            }
            pthread_mutex_unlock(&(rb_state0.lock_30));
        }
        if ((rb_state0.var_1) == (0xe4b1fa05)) {
            pthread_mutex_lock(&(rb_state0.lock_30));
            rb_state0.var_13 = 0x8a158a9c;
            rb_state0.var_14 = (rb_state0.var_14) - (((((0xcfd70488) - (0x9e920b2e)) + (rb_state0.var_11)) ^ (rb_state0.var_10)) + (rb_state0.var_13));
            rb_state0.var_15 = (rb_state0.var_15) ^ (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_12) : (0x7e079abe));
            rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_14) : (0xdf76638));
            rb_state0.var_17 = (rb_state0.var_17) + (((((0xcbf18d8c) - (rb_state0.var_16)) + (rb_state0.var_9)) ^ (rb_state0.var_14)) + (0xe779d873));
            rb_state0.var_18 = (rb_state0.var_18) + (((0x639d1c2) - (0x4d00e64)) ^ (rb_state0.var_15));
            rb_state0.var_19 = (rb_state0.var_19) + (rb_state0.var_16);
            rb_state0.var_20 = (rb_state0.var_20) ^ (((0x941378e6) + (rb_state0.var_17)) ^ (rb_state0.var_17));
            rb_state0.var_21 = (rb_state0.var_21) - (rb_state0.var_18);
            rb_state0.var_22 = (rb_state0.var_22) ^ (rb_state0.var_19);
            rb_state0.var_23 = (rb_state0.var_23) + (((((0x24916cee) - (rb_state0.var_18)) - (rb_state0.var_21)) ^ (rb_state0.var_20)) ^ (rb_state0.var_19));
            rb_state0.var_24 = (rb_state0.var_24) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_22) : (0xc85fb3f));
            rb_state0.var_25 = (rb_state0.var_25) - (rb_state0.var_23);
            rb_state0.var_26 = (rb_state0.var_26) + (rb_state0.var_24);
            rb_state0.var_27 = (rb_state0.var_27) + (rb_state0.var_25);
            rb_state0.var_28 = (rb_state0.var_28) ^ (((rb_state0.var_21) == (0x0)) ? (rb_state0.var_26) : (0x845c03a9));
            rb_state0.var_29 = (rb_state0.var_29) ^ (((((0x997f2d49) - (0x49c31f93)) ^ (0xe54bdbad)) ^ (rb_state0.var_28)) - (rb_state0.var_27));
            rb_state0.var_12 = (rb_state0.var_12) ^ (rb_state0.var_29);
            rb_state0.var_2 = rb_state0.var_12;
            pthread_mutex_unlock(&(rb_state0.lock_30));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_0);
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_0);
        rb_state6.var_1 = (rb_state6.var_1) ^ ((0x6a226e10) ^ (0xbf618f86));
        rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_2) : (0x6c93b787));
        rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_1) : (0x46747244));
        if ((rb_state6.var_0) == (0xec3f0c1a)) {
            pthread_mutex_lock(&(rb_state6.lock_21));
            rb_state6.var_5 = 0xd5406667;
            rb_state6.var_6 = (rb_state6.var_6) + (0x5f40b9c5);
            rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_3) : (0x8aa077d4));
            rb_state6.var_8 = (rb_state6.var_8) ^ (((((0x29f48c2a) ^ (0x64121bbc)) + (rb_state6.var_6)) + (rb_state6.var_5)) - (rb_state6.var_4));
            rb_state6.var_9 = (rb_state6.var_9) ^ (((((0xf32f3bd1) + (rb_state6.var_8)) ^ (rb_state6.var_7)) - (rb_state6.var_6)) - (0x7a74893c));
            rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_9);
            rb_state6.var_3 = rb_state6.var_4;
            pthread_mutex_unlock(&(rb_state6.lock_21));
        }
        if ((rb_state6.var_0) == (0x3c7eb28)) {
            pthread_mutex_lock(&(rb_state6.lock_21));
            rb_state6.var_10 = 0x4c218a19;
            rb_state6.var_11 = 0x88149f40;
            rb_state6.var_12 = (rb_state6.var_12) ^ (2240 < rb_input_size ? (uint32_t)rb_input[2240] : 0x8b02d5dc);
            rb_state6.var_13 = (rb_state6.var_13) ^ (((rb_state6.var_10) == (0x0)) ? (rb_state6.var_11) : (0xc23c9dcd));
            rb_state6.var_14 = (rb_state6.var_14) ^ (((0x4f9dc6d0) + (rb_state6.var_8)) + (rb_state6.var_7));
            rb_state6.var_15 = (rb_state6.var_15) - (((rb_state6.var_12) == (0x0)) ? (rb_state6.var_13) : (0xc98e322e));
            rb_state6.var_16 = (rb_state6.var_16) ^ (rb_state6.var_14);
            rb_state6.var_17 = (rb_state6.var_17) + (((0xcf2660d1) + (rb_state6.var_15)) + (rb_state6.var_9));
            rb_state6.var_18 = (rb_state6.var_18) - (((0x324ad587) ^ (0x1d2ea4ec)) - (rb_state6.var_16));
            rb_state6.var_19 = (rb_state6.var_19) - (((0x84b47d9e) + (rb_state6.var_17)) - (rb_state6.var_10));
            rb_state6.var_20 = (rb_state6.var_20) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_19) : (0xc9ffab5a));
            rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_20);
            pthread_mutex_unlock(&(rb_state6.lock_21));
        }
        if ((rb_state6.var_0) == (0x3c7eb28)) {
            if (!((rb_state6.var_3) == (rb_state6.var_4))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_0);
        rb_state9.var_1 = (rb_state9.var_1) - (0x853c9616);
        rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_1);
        rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_2) == (0x7ac369ea)) ? (rb_state9.var_1) : (0xdd2ffeff));
        rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_2) : (0xc0843b3c));
        if ((rb_state9.var_1) == (0x8d3cc9ee)) {
            usleep(100);
        }
        if ((rb_state9.var_1) == (0x8dd09506)) {
            pthread_mutex_lock(&(rb_state9.lock_22));
            rb_state9.var_5 = 0x87f166cd;
            rb_state9.var_6 = (rb_state9.var_6) + (rb_state9.var_5);
            rb_state9.var_7 = (rb_state9.var_7) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_6) : (0x75e7fd10));
            rb_state9.var_8 = (rb_state9.var_8) - ((0x52e3034b) + (0x2e43d9b2));
            rb_state9.var_9 = (rb_state9.var_9) - (((0xd593fed5) ^ (rb_state9.var_8)) - (rb_state9.var_5));
            rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_4) : (0x9a37d8fc));
            rb_state9.var_11 = (rb_state9.var_11) + (rb_state9.var_6);
            rb_state9.var_12 = (rb_state9.var_12) - (rb_state9.var_7);
            rb_state9.var_13 = (rb_state9.var_13) ^ (((((0x5f447c81) - (rb_state9.var_8)) + (rb_state9.var_9)) ^ (rb_state9.var_9)) + (0x9151fea7));
            rb_state9.var_14 = (rb_state9.var_14) ^ (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_10) : (0x4d9ccc03));
            rb_state9.var_15 = (rb_state9.var_15) ^ (((0xa07f0123) + (rb_state9.var_11)) - (0x3ea7031e));
            rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_13) : (0xf32f71e8));
            rb_state9.var_17 = (rb_state9.var_17) + (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_14) : (0x8b64adb7));
            rb_state9.var_18 = (rb_state9.var_18) ^ (((((0x3a2b3dec) ^ (rb_state9.var_15)) ^ (rb_state9.var_16)) + (rb_state9.var_12)) + (0xc260852f));
            rb_state9.var_19 = (rb_state9.var_19) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_17) : (0x3bd26539));
            rb_state9.var_20 = (rb_state9.var_20) - (((0xa2d4c915) + (rb_state9.var_18)) + (rb_state9.var_14));
            rb_state9.var_21 = (rb_state9.var_21) ^ (((rb_state9.var_19) == (0x0)) ? (rb_state9.var_20) : (0x7c69309));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_21);
            rb_state9.var_4 = rb_state9.var_3;
            pthread_mutex_unlock(&(rb_state9.lock_22));
        }
        if ((rb_state9.var_1) == (0x8dd09506)) {
            pthread_mutex_lock(&(rb_state9.lock_22));
            rb_state9.var_23 = !((rb_state9.var_4) == (rb_state9.var_3));
            pthread_mutex_unlock(&(rb_state9.lock_22));
        }
        rb_state9.var_15 = (rb_state9.var_15) - (0x67237e0a);
        rb_state9.var_16 = (rb_state9.var_16) - (rb_state9.var_17);
        rb_state9.var_18 = (rb_state9.var_18) + (0x69d77632);
        if ((rb_state9.var_19) == (0x77a4ca72)) {
            rb_state9.var_35 = !((rb_state9.var_25) == (rb_state9.var_24));
        }
        if ((rb_state9.var_19) == (0x77a4ca72)) {
            pthread_mutex_lock(&(rb_state9.lock_34));
            rb_state9.var_26 = 0x9bff545e;
            rb_state9.var_27 = 0xb457d0ab;
            rb_state9.var_28 = (rb_state9.var_28) + (rb_state9.var_26);
            rb_state9.var_29 = (rb_state9.var_29) - (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_27) : (0x1f44760c));
            rb_state9.var_30 = (rb_state9.var_30) + (((rb_state9.var_21) == (0x0)) ? (rb_state9.var_28) : (0xa01ec3a4));
            rb_state9.var_31 = (rb_state9.var_31) + (((0x32522773) + (0x1fafd990)) + (rb_state9.var_29));
            rb_state9.var_32 = (rb_state9.var_32) - (((0xcdc05df4) ^ (0xbd6b60ec)) - (rb_state9.var_30));
            rb_state9.var_33 = (rb_state9.var_33) ^ (((((0x52d1c3cc) + (rb_state9.var_31)) - (rb_state9.var_23)) ^ (0x7b6a6cbc)) + (rb_state9.var_32));
            rb_state9.var_24 = (rb_state9.var_24) + (rb_state9.var_33);
            rb_state9.var_25 = rb_state9.var_24;
            pthread_mutex_unlock(&(rb_state9.lock_34));
        }
        rb_state9.var_28 = (rb_state9.var_28) + ((0x95fa2f51) - (0x702b809d));
        rb_state9.var_29 = (rb_state9.var_29) ^ (((rb_state9.var_25) == (0x0)) ? (rb_state9.var_24) : (0xe7ae0f52));
        if ((rb_state9.var_26) == (0x9bff545e)) {
            if ((rb_state9.var_36) != (0x0)) {
                if (!((rb_state9.var_36) == (rb_state9.var_47))) {
                    racebench_trigger(9);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_0);
        rb_state10.var_1 = (rb_state10.var_1) - (0xa2525944);
        rb_state10.var_2 = (rb_state10.var_2) ^ (((rb_state10.var_1) == (0x3db90cbc)) ? (rb_state10.var_0) : (0x11cdeeef));
        if ((rb_state10.var_2) == (0xbe40379b)) {
            rb_state10.var_13 = (rb_state10.var_13) + (rb_state10.var_7);
            rb_state10.var_14 = (rb_state10.var_14) + (rb_state10.var_6);
            rb_state10.var_15 = (rb_state10.var_15) + (((0x1e454c29) ^ (rb_state10.var_8)) - (rb_state10.var_5));
            rb_state10.var_16 = (rb_state10.var_16) - (23036 < rb_input_size ? (uint32_t)rb_input[23036] : 0x4187a4c3);
            rb_state10.var_17 = (rb_state10.var_17) - (((((0xa34af7d7) + (0x983f8c99)) ^ (rb_state10.var_13)) + (rb_state10.var_9)) ^ (rb_state10.var_14));
            rb_state10.var_18 = (rb_state10.var_18) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0x913158b4));
            rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0x7ec498c7));
            rb_state10.var_12 = (rb_state10.var_12) - (rb_state10.var_19);
            rb_state10.var_3 = rb_state10.var_12;
        }
        if ((rb_state10.var_2) == (0xa896bc9c)) {
            if ((rb_state10.var_3) != (0x0)) {
                if (!((rb_state10.var_3) == (rb_state10.var_20))) {
                    racebench_trigger(10);
                }
            }
        }
        if ((rb_state10.var_2) == (0xa896bc9c)) {
            pthread_mutex_lock(&(rb_state10.lock_24));
            rb_state10.var_21 = 0x55a7252;
            rb_state10.var_22 = (rb_state10.var_22) - (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_21) : (0x66507408));
            rb_state10.var_23 = (rb_state10.var_23) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_22) : (0x736152ff));
            rb_state10.var_20 = (rb_state10.var_20) + (rb_state10.var_23);
            rb_state10.var_3 = rb_state10.var_20;
            pthread_mutex_unlock(&(rb_state10.lock_24));
        }
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) - (0x8311147d);
        rb_state16.var_1 = (rb_state16.var_1) ^ (26863 < rb_input_size ? (uint32_t)rb_input[26863] : 0xa894b517);
        if ((rb_state16.var_1) == (0x8b76f5de)) {
            rb_state16.var_13 = 0xee216fd8;
            rb_state16.var_14 = 0xb67b1d60;
            rb_state16.var_15 = 0x581991b5;
            rb_state16.var_16 = (rb_state16.var_16) - (64387 < rb_input_size ? (uint32_t)rb_input[64387] : 0x128cd54c);
            rb_state16.var_17 = (rb_state16.var_17) ^ (((0xb0eb45a7) + (0x7eb7760e)) ^ (rb_state16.var_14));
            rb_state16.var_18 = (rb_state16.var_18) - (rb_state16.var_13);
            rb_state16.var_19 = (rb_state16.var_19) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_15) : (0xd80882ea));
            rb_state16.var_20 = (rb_state16.var_20) ^ (((0x3fab5e23) ^ (rb_state16.var_16)) ^ (0xbb9f5dca));
            rb_state16.var_21 = (rb_state16.var_21) - (((0x95d06f7d) - (rb_state16.var_17)) ^ (0xa2aac5f2));
            rb_state16.var_22 = (rb_state16.var_22) + (((0x28f6d4f3) + (rb_state16.var_6)) - (rb_state16.var_18));
            rb_state16.var_23 = (rb_state16.var_23) ^ (rb_state16.var_19);
            rb_state16.var_24 = (rb_state16.var_24) + (((0x4947529f) + (0x7b517ea5)) + (rb_state16.var_20));
            rb_state16.var_25 = (rb_state16.var_25) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_21) : (0x9f22d6fe));
            rb_state16.var_26 = (rb_state16.var_26) + (((((0x463f95a6) - (rb_state16.var_8)) + (rb_state16.var_23)) + (0xfe455271)) ^ (rb_state16.var_22));
            rb_state16.var_27 = (rb_state16.var_27) ^ (((0xcb18bab8) ^ (rb_state16.var_24)) - (rb_state16.var_9));
            rb_state16.var_28 = (rb_state16.var_28) + (rb_state16.var_25);
            rb_state16.var_29 = (rb_state16.var_29) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_26) : (0xfecb07b5));
            rb_state16.var_30 = (rb_state16.var_30) ^ (rb_state16.var_27);
            rb_state16.var_31 = (rb_state16.var_31) + (((0xecdaa12a) + (rb_state16.var_28)) - (0x62e0eb6));
            rb_state16.var_32 = (rb_state16.var_32) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_29) : (0xd3357a76));
            rb_state16.var_33 = (rb_state16.var_33) + (((rb_state16.var_30) == (0x0)) ? (rb_state16.var_31) : (0x5ddb34da));
            rb_state16.var_34 = (rb_state16.var_34) + (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_33) : (0xdeca1b8e));
            rb_state16.var_12 = (rb_state16.var_12) - (rb_state16.var_34);
            rb_state16.var_2 = rb_state16.var_12;
        }
        if ((rb_state16.var_1) == (0x865e15f)) {
            if ((rb_state16.var_2) != (0x0)) {
                if (!((rb_state16.var_2) == (rb_state16.var_35))) {
                    racebench_trigger(16);
                }
            }
        }
        if ((rb_state16.var_1) == (0x865e15f)) {
            pthread_mutex_lock(&(rb_state16.lock_46));
            rb_state16.var_36 = 0xf4394556;
            rb_state16.var_37 = 0xca86d518;
            rb_state16.var_38 = (rb_state16.var_38) + (rb_state16.var_12);
            rb_state16.var_39 = (rb_state16.var_39) + (48925 < rb_input_size ? (uint32_t)rb_input[48925] : 0xcf315859);
            rb_state16.var_40 = (rb_state16.var_40) + (((0xf8f3b009) + (rb_state16.var_13)) + (rb_state16.var_36));
            rb_state16.var_41 = (rb_state16.var_41) ^ (((((0x3141c44d) ^ (rb_state16.var_37)) - (rb_state16.var_14)) ^ (rb_state16.var_38)) + (0x9e4d2f3));
            rb_state16.var_42 = (rb_state16.var_42) - (((((0x2d32fe94) + (0x97144c45)) + (rb_state16.var_15)) + (rb_state16.var_40)) - (rb_state16.var_39));
            rb_state16.var_43 = (rb_state16.var_43) ^ (rb_state16.var_41);
            rb_state16.var_44 = (rb_state16.var_44) + (((0xa6cda712) - (0x7afcf9e)) + (rb_state16.var_42));
            rb_state16.var_45 = (rb_state16.var_45) ^ (((rb_state16.var_43) == (0x0)) ? (rb_state16.var_44) : (0x66e50511));
            rb_state16.var_35 = (rb_state16.var_35) ^ (rb_state16.var_45);
            rb_state16.var_2 = rb_state16.var_35;
            pthread_mutex_unlock(&(rb_state16.lock_46));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_0);
        rb_state17.var_1 = (rb_state17.var_1) + ((0x27a0ae3d) ^ (rb_state17.var_2));
        rb_state17.var_0 = (rb_state17.var_0) ^ (39968 < rb_input_size ? (uint32_t)rb_input[39968] : 0x4ff56618);
        if ((rb_state17.var_1) == (0xd7a7ef89)) {
            rb_state17.var_11 = 0x1a201667;
            rb_state17.var_12 = 0x8f598115;
            rb_state17.var_13 = (rb_state17.var_13) - (rb_state17.var_7);
            rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_8) : (0x1b304826));
            rb_state17.var_15 = (rb_state17.var_15) ^ (rb_state17.var_6);
            rb_state17.var_16 = (rb_state17.var_16) + (((((0x8f2d287f) - (rb_state17.var_12)) ^ (rb_state17.var_10)) + (rb_state17.var_11)) + (0xb3e4112f));
            rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_13) : (0xb989006c));
            rb_state17.var_18 = (rb_state17.var_18) + (((((0x7d64feda) + (rb_state17.var_15)) ^ (rb_state17.var_12)) - (rb_state17.var_13)) + (rb_state17.var_14));
            rb_state17.var_19 = (rb_state17.var_19) ^ (((0x86836738) - (rb_state17.var_16)) - (rb_state17.var_14));
            rb_state17.var_20 = (rb_state17.var_20) ^ (((0xc33d40f6) - (rb_state17.var_15)) ^ (rb_state17.var_17));
            rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_18) : (0xe6cd5efd));
            rb_state17.var_22 = (rb_state17.var_22) - (((((0xa414ad70) + (0x4e02790f)) + (rb_state17.var_20)) - (rb_state17.var_19)) - (rb_state17.var_17));
            rb_state17.var_23 = (rb_state17.var_23) - (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_21) : (0xadf2735b));
            rb_state17.var_24 = (rb_state17.var_24) ^ (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_23) : (0xcfc3dcd9));
            rb_state17.var_10 = (rb_state17.var_10) ^ (rb_state17.var_24);
            rb_state17.var_3 = rb_state17.var_10;
        }
        if ((rb_state17.var_1) == (0xb00745ee)) {
            if ((rb_state17.var_3) != (0x0)) {
                if (!((rb_state17.var_3) == (rb_state17.var_25))) {
                    racebench_trigger(17);
                }
            }
        }
        if ((rb_state17.var_1) == (0xb00745ee)) {
            pthread_mutex_lock(&(rb_state17.lock_54));
            rb_state17.var_26 = 0xf0fad8d7;
            rb_state17.var_27 = (rb_state17.var_27) - (rb_state17.var_20);
            rb_state17.var_28 = (rb_state17.var_28) ^ ((0xb15a4d00) + (0xdcbe406));
            rb_state17.var_29 = (rb_state17.var_29) + (rb_state17.var_21);
            rb_state17.var_30 = (rb_state17.var_30) + (((0x138b198) + (rb_state17.var_26)) - (rb_state17.var_22));
            rb_state17.var_31 = (rb_state17.var_31) - (((rb_state17.var_23) == (0x0)) ? (rb_state17.var_19) : (0x76fcaad1));
            rb_state17.var_32 = (rb_state17.var_32) + (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_27) : (0x255ffecb));
            rb_state17.var_33 = (rb_state17.var_33) + (((0x29fd6225) ^ (rb_state17.var_28)) + (rb_state17.var_25));
            rb_state17.var_34 = (rb_state17.var_34) + (((0x5ee68de1) ^ (rb_state17.var_29)) ^ (rb_state17.var_26));
            rb_state17.var_35 = (rb_state17.var_35) - (((rb_state17.var_27) == (0x0)) ? (rb_state17.var_30) : (0xb63a0edc));
            rb_state17.var_36 = (rb_state17.var_36) - (((rb_state17.var_28) == (0x0)) ? (rb_state17.var_31) : (0x52d6bef5));
            rb_state17.var_37 = (rb_state17.var_37) + (rb_state17.var_32);
            rb_state17.var_38 = (rb_state17.var_38) + (rb_state17.var_33);
            rb_state17.var_39 = (rb_state17.var_39) + (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_34) : (0x8d422289));
            rb_state17.var_40 = (rb_state17.var_40) - (((((0xe2103ada) ^ (rb_state17.var_35)) ^ (rb_state17.var_30)) + (0x9bd281f3)) + (rb_state17.var_36));
            rb_state17.var_41 = (rb_state17.var_41) - (rb_state17.var_37);
            rb_state17.var_42 = (rb_state17.var_42) + (((rb_state17.var_38) == (0x0)) ? (rb_state17.var_39) : (0xeef8914c));
            rb_state17.var_43 = (rb_state17.var_43) + (((0x73244c5d) ^ (rb_state17.var_40)) + (rb_state17.var_31));
            rb_state17.var_44 = (rb_state17.var_44) - (((rb_state17.var_32) == (0x0)) ? (rb_state17.var_41) : (0xc7c52d07));
            rb_state17.var_45 = (rb_state17.var_45) + (rb_state17.var_42);
            rb_state17.var_46 = (rb_state17.var_46) + (((rb_state17.var_33) == (0x0)) ? (rb_state17.var_43) : (0x96132ff2));
            rb_state17.var_47 = (rb_state17.var_47) + (rb_state17.var_44);
            rb_state17.var_48 = (rb_state17.var_48) - (((((0xb5d11d51) ^ (rb_state17.var_34)) - (rb_state17.var_45)) ^ (rb_state17.var_46)) + (0x5a3fd9e7));
            rb_state17.var_49 = (rb_state17.var_49) - (((rb_state17.var_35) == (0x0)) ? (rb_state17.var_47) : (0xdb02cf31));
            rb_state17.var_50 = (rb_state17.var_50) + (((0xc72df28d) - (rb_state17.var_48)) ^ (0xbc5b384b));
            rb_state17.var_51 = (rb_state17.var_51) - (((rb_state17.var_36) == (0x0)) ? (rb_state17.var_49) : (0x62195e05));
            rb_state17.var_52 = (rb_state17.var_52) - (((0xb408c270) ^ (0x7acbe42b)) - (rb_state17.var_50));
            rb_state17.var_53 = (rb_state17.var_53) - (((rb_state17.var_51) == (0x0)) ? (rb_state17.var_52) : (0x352e42c6));
            rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_53);
            rb_state17.var_3 = rb_state17.var_25;
            pthread_mutex_unlock(&(rb_state17.lock_54));
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) ^ (0xc3b532d2);
        rb_state19.var_1 = (rb_state19.var_1) - ((0xb9e62ac9) ^ (rb_state19.var_0));
        #endif
        myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    hizs[pid] = myhiz;

#ifdef ENABLE_THREADS
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_26) == (0x9bff545e)) {
        rb_state9.var_43 = 0xe8118217;
        rb_state9.var_44 = 0x64f51889;
        rb_state9.var_45 = (rb_state9.var_45) + (((((0x597ae61b) - (rb_state9.var_32)) ^ (rb_state9.var_43)) - (rb_state9.var_31)) + (0xede36a21));
        rb_state9.var_46 = (rb_state9.var_46) ^ (((((0x1970ca18) ^ (0xe9dd97e)) ^ (rb_state9.var_44)) ^ (rb_state9.var_45)) - (0x91fd2eb6));
        rb_state9.var_42 = (rb_state9.var_42) - (rb_state9.var_46);
        rb_state9.var_36 = rb_state9.var_42;
    }
    if ((rb_state9.var_26) == (0x9bff545e)) {
        pthread_mutex_lock(&(rb_state9.lock_64));
        rb_state9.var_48 = (rb_state9.var_48) - (44833 < rb_input_size ? (uint32_t)rb_input[44833] : 0x29d7e8a0);
        rb_state9.var_49 = (rb_state9.var_49) - ((0xfe769552) + (rb_state9.var_37));
        rb_state9.var_50 = (rb_state9.var_50) - (((((0xd644ac40) ^ (0x482f7244)) - (rb_state9.var_33)) - (rb_state9.var_35)) + (rb_state9.var_38));
        rb_state9.var_51 = (rb_state9.var_51) + (((rb_state9.var_36) == (0x0)) ? (rb_state9.var_48) : (0xb8cd15d5));
        rb_state9.var_52 = (rb_state9.var_52) ^ (rb_state9.var_49);
        rb_state9.var_53 = (rb_state9.var_53) - (((rb_state9.var_39) == (0x0)) ? (rb_state9.var_50) : (0xeba0d77e));
        rb_state9.var_54 = (rb_state9.var_54) + (((rb_state9.var_40) == (0x0)) ? (rb_state9.var_51) : (0x5d0d0bfd));
        rb_state9.var_55 = (rb_state9.var_55) ^ (rb_state9.var_52);
        rb_state9.var_56 = (rb_state9.var_56) ^ (rb_state9.var_53);
        rb_state9.var_57 = (rb_state9.var_57) ^ (((0xe778a342) ^ (0x89b98b7e)) - (rb_state9.var_54));
        rb_state9.var_58 = (rb_state9.var_58) + (((rb_state9.var_41) == (0x0)) ? (rb_state9.var_55) : (0x35ec22b1));
        rb_state9.var_59 = (rb_state9.var_59) ^ (((rb_state9.var_42) == (0x0)) ? (rb_state9.var_56) : (0x3a5ec74a));
        rb_state9.var_60 = (rb_state9.var_60) ^ (((rb_state9.var_43) == (0x0)) ? (rb_state9.var_57) : (0x6433a251));
        rb_state9.var_61 = (rb_state9.var_61) + (rb_state9.var_58);
        rb_state9.var_62 = (rb_state9.var_62) ^ (((rb_state9.var_59) == (0x0)) ? (rb_state9.var_60) : (0xf282d660));
        rb_state9.var_63 = (rb_state9.var_63) + (((((0xd24ef15a) - (rb_state9.var_44)) - (0xfbed9ed2)) + (rb_state9.var_62)) ^ (rb_state9.var_61));
        rb_state9.var_47 = (rb_state9.var_47) - (rb_state9.var_63);
        rb_state9.var_36 = rb_state9.var_47;
        pthread_mutex_unlock(&(rb_state9.lock_64));
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

    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + ((0xb1acfd68) + (rb_state1.var_2));
    if ((rb_state1.var_2) == (0xdbbaedd8)) {
        rb_state1.var_3 = rb_state1.var_0;
    }
    if ((rb_state1.var_2) == (0xdbbaedd8)) {
        if (!((rb_state1.var_0) == (rb_state1.var_3))) {
            racebench_trigger(1);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_2) == (0x0)) {
        rb_state2.var_3 = rb_state2.var_0;
    }
    if ((rb_state2.var_2) == (0x0)) {
        if (!((rb_state2.var_0) == (rb_state2.var_3))) {
            racebench_trigger(2);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_0);
    #endif
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_2) == (0x0)) {
        if ((rb_state5.var_3) != (0x0)) {
            if (!((rb_state5.var_3) == (rb_state5.var_37))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0x1ca92fef));
    rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_1);
    rb_state8.var_2 = (rb_state8.var_2) - ((0xd31f0521) - (0x4dfe0898));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (0x8ffad43f);
    if ((rb_state11.var_1) == (0x0)) {
        if ((rb_state11.var_3) != (0x0)) {
            if (!((rb_state11.var_3) == (rb_state11.var_15))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + ((0x162924ec) + (rb_state15.var_1));
    if ((rb_state15.var_1) == (0x427b6ec4)) {
        rb_state15.var_3 = rb_state15.var_2;
    }
    if ((rb_state15.var_1) == (0x427b6ec4)) {
        rb_state15.var_7 = !((rb_state15.var_2) == (rb_state15.var_3));
    }
    if ((rb_state15.var_4) == (0x60b8caa9)) {
        if ((rb_state15.var_8) != (0x0)) {
            if (!((rb_state15.var_8) == (rb_state15.var_48))) {
                racebench_trigger(15);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_2) == (0x0)) {
        rb_state18.var_4 = 0x7054c120;
        rb_state18.var_5 = 0x6d4c6250;
        rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_4) == (0x7054c120)) ? (rb_state18.var_4) : (0xd3364820));
        rb_state18.var_7 = (rb_state18.var_7) + (((((0xba069bf0) ^ (rb_state18.var_6)) ^ (rb_state18.var_5)) - (rb_state18.var_3)) ^ (rb_state18.var_5));
        rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_7) == (0x35ada510)) ? (rb_state18.var_6) : (0x753af61b));
        rb_state18.var_9 = (rb_state18.var_9) - (((((0xe706886c) - (rb_state18.var_8)) ^ (rb_state18.var_8)) ^ (rb_state18.var_7)) ^ (0xe21efe8));
        rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_9);
    }
    #endif
    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_0) : (0x9b90450e));
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
    rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0xffffffbf)) ? (rb_state2.var_0) : (0x7a0c39ab));
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_1) == (0x0)) {
        rb_state3.var_2 = rb_state3.var_0;
    }
    if ((rb_state3.var_1) == (0x0)) {
        if (!((rb_state3.var_0) == (rb_state3.var_2))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_0) : (0xd28c707c));
    if ((rb_state4.var_2) == (0x0)) {
        rb_state4.var_4 = 0x40ee3dbb;
        rb_state4.var_5 = 0x25cc65c8;
        rb_state4.var_6 = (rb_state4.var_6) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_3) : (0x1314478c));
        rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0x4cdef70f));
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_7);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (((rb_state5.var_0) == (0xc73ab772)) ? (rb_state5.var_0) : (0x29a7d3e1));
    rb_state5.var_1 = (rb_state5.var_1) ^ ((0x12b10d88) + (rb_state5.var_1));
    rb_state5.var_1 = (rb_state5.var_1) - (36066 < rb_input_size ? (uint32_t)rb_input[36066] : 0x9a0a0474);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) ^ (rb_state8.var_2);
    if ((rb_state8.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state8.lock_43));
        if ((rb_state8.var_3) != (0x0)) {
            if (!((rb_state8.var_3) == (rb_state8.var_31))) {
                racebench_trigger(8);
            }
        }
        pthread_mutex_unlock(&(rb_state8.lock_43));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) + (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_1) : (0xdfd0c25));
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0x6e46d060)) {
        rb_state13.var_3 = rb_state13.var_1;
    }
    if ((rb_state13.var_2) == (0x6e46d060)) {
        if (!((rb_state13.var_1) == (rb_state13.var_3))) {
            racebench_trigger(13);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) ^ ((0xb77a916b) ^ (rb_state14.var_0));
    if ((rb_state14.var_1) == (0xd9)) {
        if ((rb_state14.var_3) != (0x0)) {
            if (!((rb_state14.var_3) == (rb_state14.var_13))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_7);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) + (22583 < rb_input_size ? (uint32_t)rb_input[22583] : 0x1dce64a6);
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
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ (rb_state1.var_1);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_2) : (0x55d53bfb));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) - (0x293c3268);
        if ((rb_state3.var_1) == (0x0)) {
            rb_state3.var_3 = 0xfb3e6719;
            rb_state3.var_4 = 0xad77b7e7;
            rb_state3.var_5 = (rb_state3.var_5) + (((0x59ec27c2) + (0xfaf48e68)) ^ (rb_state3.var_4));
            rb_state3.var_6 = (rb_state3.var_6) ^ (((((0x835e0828) + (rb_state3.var_2)) - (0x8fb8d902)) ^ (0xc78edd90)) + (rb_state3.var_3));
            rb_state3.var_7 = (rb_state3.var_7) - (rb_state3.var_5);
            rb_state3.var_8 = (rb_state3.var_8) ^ (((((0x6fc5d06f) - (rb_state3.var_6)) + (rb_state3.var_7)) ^ (0xb55b0888)) - (0x705340c2));
            rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) - (rb_state4.var_2);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_2);
        rb_state5.var_0 = (rb_state5.var_0) - ((0x38c5488e) - (rb_state5.var_1));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (53382 < rb_input_size ? (uint32_t)rb_input[53382] : 0xb03044c8);
        rb_state11.var_2 = (rb_state11.var_2) ^ (24238 < rb_input_size ? (uint32_t)rb_input[24238] : 0x75cf1d74);
        if ((rb_state11.var_1) == (0x0)) {
            rb_state11.var_11 = 0x9f7d557a;
            rb_state11.var_12 = 0x97dc6fba;
            rb_state11.var_13 = (rb_state11.var_13) ^ (((((0x5a6d54ea) + (rb_state11.var_9)) - (0xc8e29f44)) - (rb_state11.var_12)) ^ (rb_state11.var_11));
            rb_state11.var_14 = (rb_state11.var_14) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_13) : (0x1a42932));
            rb_state11.var_10 = (rb_state11.var_10) + (rb_state11.var_14);
            rb_state11.var_3 = rb_state11.var_10;
        }
        if ((rb_state11.var_1) == (0x0)) {
            pthread_mutex_lock(&(rb_state11.lock_20));
            rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_11) : (0x89670655));
            rb_state11.var_17 = (rb_state11.var_17) ^ (0x918d556);
            rb_state11.var_18 = (rb_state11.var_18) + (((((0x23a40e4b) - (rb_state11.var_16)) ^ (rb_state11.var_13)) + (rb_state11.var_10)) ^ (0xc08ac169));
            rb_state11.var_19 = (rb_state11.var_19) + (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_18) : (0xbc575e83));
            rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_19);
            rb_state11.var_3 = rb_state11.var_15;
            pthread_mutex_unlock(&(rb_state11.lock_20));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) ^ (rb_state12.var_2);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) - (38049 < rb_input_size ? (uint32_t)rb_input[38049] : 0xb3740b2f);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (55471 < rb_input_size ? (uint32_t)rb_input[55471] : 0xd17de34b);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_0) == (0x0)) ? (rb_state15.var_0) : (0x407f009b));
        rb_state15.var_0 = (rb_state15.var_0) + (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_1) : (0x417b81d5));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) + (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x7c36a343));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (0xdbf78dab);
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_2);
        if ((rb_state18.var_2) == (0x0)) {
            rb_state18.var_3 = rb_state18.var_1;
        }
        #endif
        arg[i].points = points;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (43384 < rb_input_size ? (uint32_t)rb_input[43384] : 0x8417982);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (50830 < rb_input_size ? (uint32_t)rb_input[50830] : 0xfad41400);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) + (0x66cefc99);
        if ((rb_state4.var_2) == (0x0)) {
            rb_state4.var_3 = rb_state4.var_1;
        }
        if ((rb_state4.var_2) == (0x0)) {
            if (!((rb_state4.var_1) == (rb_state4.var_3))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - ((0xd88208cc) - (rb_state8.var_0));
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) - (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_1) : (0xc0223a75));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + ((0x81721c7b) ^ (rb_state13.var_0));
        rb_state13.var_1 = (rb_state13.var_1) - (0x25a4114f);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) + (((rb_state14.var_2) == (0x0)) ? (rb_state14.var_2) : (0x5e233a49));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) - (0xacd388fc);
        rb_state15.var_5 = (rb_state15.var_5) + (39996 < rb_input_size ? (uint32_t)rb_input[39996] : 0x22c7f586);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (0xf8b67435);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (((rb_state18.var_0) == (0xdbf78dab)) ? (rb_state18.var_0) : (0xf7fa90d4));
        if ((rb_state18.var_2) == (0x0)) {
            if (!((rb_state18.var_1) == (rb_state18.var_3))) {
                racebench_trigger(18);
            }
        }
        #endif
        arg[i].kmin = kmin;
        #ifdef RACEBENCH_BUG_2
        if ((rb_state2.var_2) == (0x0)) {
            rb_state2.var_4 = 0x5ccabf61;
            rb_state2.var_5 = (rb_state2.var_5) ^ (((((0x81081d44) + (rb_state2.var_5)) + (rb_state2.var_3)) ^ (rb_state2.var_4)) ^ (0xd89d8506));
            rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_5);
        }
        #endif
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_2) == (0x0)) {
            rb_state5.var_28 = 0x3eb363cb;
            rb_state5.var_29 = 0xe42047a1;
            rb_state5.var_30 = (rb_state5.var_30) + (rb_state5.var_17);
            rb_state5.var_31 = (rb_state5.var_31) - (((((0xd1a699e7) ^ (rb_state5.var_18)) - (rb_state5.var_19)) + (rb_state5.var_16)) - (rb_state5.var_29));
            rb_state5.var_32 = (rb_state5.var_32) + (((((0x572f17a1) + (rb_state5.var_20)) + (rb_state5.var_28)) ^ (rb_state5.var_30)) + (rb_state5.var_21));
            rb_state5.var_33 = (rb_state5.var_33) - (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_31) : (0x2f8376a7));
            rb_state5.var_34 = (rb_state5.var_34) - (rb_state5.var_32);
            rb_state5.var_35 = (rb_state5.var_35) - (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_33) : (0x3a3d5ee0));
            rb_state5.var_36 = (rb_state5.var_36) + (((rb_state5.var_34) == (0x0)) ? (rb_state5.var_35) : (0xb9e02e79));
            rb_state5.var_27 = (rb_state5.var_27) ^ (rb_state5.var_36);
            rb_state5.var_3 = rb_state5.var_27;
        }
        if ((rb_state5.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state5.lock_65));
            rb_state5.var_38 = 0x7a96ae52;
            rb_state5.var_39 = 0x239defe4;
            rb_state5.var_40 = 0x167b48e8;
            rb_state5.var_41 = (rb_state5.var_41) + (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_39) : (0x8b24fd18));
            rb_state5.var_42 = (rb_state5.var_42) + (((0xc8c856c2) + (rb_state5.var_40)) - (0xef02f4be));
            rb_state5.var_43 = (rb_state5.var_43) ^ (((rb_state5.var_25) == (0x0)) ? (rb_state5.var_38) : (0xa2e30a5b));
            rb_state5.var_44 = (rb_state5.var_44) - (((0x7b82ae84) ^ (rb_state5.var_26)) ^ (rb_state5.var_41));
            rb_state5.var_45 = (rb_state5.var_45) ^ (rb_state5.var_42);
            rb_state5.var_46 = (rb_state5.var_46) + (((rb_state5.var_27) == (0x0)) ? (rb_state5.var_43) : (0xb8f39624));
            rb_state5.var_47 = (rb_state5.var_47) - (((rb_state5.var_28) == (0x0)) ? (rb_state5.var_44) : (0x5d9e18cb));
            rb_state5.var_48 = (rb_state5.var_48) - (rb_state5.var_45);
            rb_state5.var_49 = (rb_state5.var_49) + (rb_state5.var_46);
            rb_state5.var_50 = (rb_state5.var_50) - (rb_state5.var_47);
            rb_state5.var_51 = (rb_state5.var_51) ^ (rb_state5.var_48);
            rb_state5.var_52 = (rb_state5.var_52) ^ (((0x8baad75f) ^ (rb_state5.var_29)) ^ (rb_state5.var_49));
            rb_state5.var_53 = (rb_state5.var_53) ^ (((0x8735bde6) ^ (rb_state5.var_50)) - (rb_state5.var_30));
            rb_state5.var_54 = (rb_state5.var_54) ^ (((rb_state5.var_31) == (0x0)) ? (rb_state5.var_51) : (0x8c4567ca));
            rb_state5.var_55 = (rb_state5.var_55) ^ (((rb_state5.var_32) == (0x0)) ? (rb_state5.var_52) : (0x4a23e64b));
            rb_state5.var_56 = (rb_state5.var_56) - (((0x5de80c5f) - (0xb49afe8b)) ^ (rb_state5.var_53));
            rb_state5.var_57 = (rb_state5.var_57) - (((0x5f237b3a) ^ (rb_state5.var_54)) ^ (rb_state5.var_33));
            rb_state5.var_58 = (rb_state5.var_58) - (rb_state5.var_55);
            rb_state5.var_59 = (rb_state5.var_59) - (((0x7a3024fb) + (rb_state5.var_56)) - (0xf0e40000));
            rb_state5.var_60 = (rb_state5.var_60) - (((((0x8580588c) + (rb_state5.var_57)) - (rb_state5.var_35)) ^ (rb_state5.var_34)) ^ (rb_state5.var_58));
            rb_state5.var_61 = (rb_state5.var_61) - (((0xe9c61e70) - (0x5b05a14c)) + (rb_state5.var_59));
            rb_state5.var_62 = (rb_state5.var_62) ^ (rb_state5.var_60);
            rb_state5.var_63 = (rb_state5.var_63) + (rb_state5.var_61);
            rb_state5.var_64 = (rb_state5.var_64) + (((((0xf5cf7039) + (rb_state5.var_62)) - (0xb121eac7)) - (rb_state5.var_36)) - (rb_state5.var_63));
            rb_state5.var_37 = (rb_state5.var_37) - (rb_state5.var_64);
            rb_state5.var_3 = rb_state5.var_37;
            pthread_mutex_unlock(&(rb_state5.lock_65));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_0);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_0);
        if ((rb_state14.var_1) == (0xb77a918d)) {
            rb_state14.var_7 = 0x658bddeb;
            rb_state14.var_8 = 0x93c71451;
            rb_state14.var_9 = 0x58c023d6;
            rb_state14.var_10 = (rb_state14.var_10) - (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_8) : (0xe3ebe663));
            rb_state14.var_11 = (rb_state14.var_11) + (rb_state14.var_7);
            rb_state14.var_12 = (rb_state14.var_12) ^ (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_11) : (0xec67a926));
            rb_state14.var_6 = (rb_state14.var_6) + (rb_state14.var_12);
            rb_state14.var_3 = rb_state14.var_6;
        }
        if ((rb_state14.var_1) == (0xd9)) {
            pthread_mutex_lock(&(rb_state14.lock_23));
            rb_state14.var_14 = 0x1c26bbb3;
            rb_state14.var_15 = (rb_state14.var_15) + (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_8) : (0x7c978aa));
            rb_state14.var_16 = (rb_state14.var_16) ^ (((0xd158e58b) ^ (rb_state14.var_10)) - (rb_state14.var_14));
            rb_state14.var_17 = (rb_state14.var_17) - (rb_state14.var_6);
            rb_state14.var_18 = (rb_state14.var_18) ^ (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_15) : (0x85f270ea));
            rb_state14.var_19 = (rb_state14.var_19) + (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_16) : (0x63a790cb));
            rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_17) : (0xce8fbc03));
            rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_19) : (0x436d6060));
            rb_state14.var_22 = (rb_state14.var_22) + (((((0x674b70f0) ^ (rb_state14.var_21)) - (0xa10fc324)) + (0x3bd2a9cd)) + (rb_state14.var_20));
            rb_state14.var_13 = (rb_state14.var_13) + (rb_state14.var_22);
            rb_state14.var_3 = rb_state14.var_13;
            pthread_mutex_unlock(&(rb_state14.lock_23));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_1) : (0x38434a62));
        #endif
        arg[i].kmax = kmax;
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_2) == (0xdbbaedd8)) {
            pthread_mutex_lock(&(rb_state1.lock_8));
            rb_state1.var_4 = 0xacfab16a;
            rb_state1.var_5 = 0x3e0e3005;
            rb_state1.var_6 = (rb_state1.var_6) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0xa7eeb612));
            rb_state1.var_7 = (rb_state1.var_7) + (((((0x693e144a) ^ (rb_state1.var_5)) ^ (rb_state1.var_6)) ^ (rb_state1.var_5)) + (rb_state1.var_4));
            rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_7);
            pthread_mutex_unlock(&(rb_state1.lock_8));
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (2891 < rb_input_size ? (uint32_t)rb_input[2891] : 0x5b47828d);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (0x638458f6);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - (12690 < rb_input_size ? (uint32_t)rb_input[12690] : 0xd0329730);
        rb_state4.var_0 = (rb_state4.var_0) - (30218 < rb_input_size ? (uint32_t)rb_input[30218] : 0x113cccb2);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_1 = (rb_state14.var_1) + (62553 < rb_input_size ? (uint32_t)rb_input[62553] : 0xcecdfd);
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_1) == (0x427b6ec4)) {
            rb_state15.var_4 = 0x60b8caa9;
            rb_state15.var_5 = (rb_state15.var_5) - (((((0x58a5d80e) - (rb_state15.var_3)) ^ (rb_state15.var_4)) ^ (0x19046b23)) - (0x17cefe));
            rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_5);
        }
        rb_state15.var_5 = (rb_state15.var_5) ^ (rb_state15.var_3);
        #endif
        arg[i].pid = i;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0xffffff87)) ? (rb_state1.var_0) : (0x35486284));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (7292 < rb_input_size ? (uint32_t)rb_input[7292] : 0xa7698647);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_0);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (0xfc013e89);
        rb_state13.var_2 = (rb_state13.var_2) + ((0x6e46d060) ^ (rb_state13.var_2));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_0) : (0xafe95922));
        #endif
        arg[i].kfinal = kfinal;

        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - ((0xefaaa538) ^ (0x7dcf4ef1));
        rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x6d9a1437)) ? (rb_state3.var_1) : (0xb917420a));
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) ^ (11175 < rb_input_size ? (uint32_t)rb_input[11175] : 0x418e5b7f);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_1) == (0x162924ec)) ? (rb_state15.var_2) : (0x3359a620));
        #endif
        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) ^ ((0xd6660988) ^ (0xff0a722c));
        rb_state1.var_1 = (rb_state1.var_1) + (57710 < rb_input_size ? (uint32_t)rb_input[57710] : 0xb01c07b1);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) - (45654 < rb_input_size ? (uint32_t)rb_input[45654] : 0x1e50ff5);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) - (35596 < rb_input_size ? (uint32_t)rb_input[35596] : 0xb4218e19);
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x80b3d56d));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) - (0x9a9f46c2);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_1) : (0x3eaa4e21));
        #endif
        pthread_create(threads + i, NULL, localSearchSub, (void *)&arg[i]);
#else
        localSearchSub(&arg[0]);
#endif
    }

#ifdef ENABLE_THREADS
    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_43));
            rb_state8.var_21 = 0x7189a593;
            rb_state8.var_22 = 0x8fe8cf29;
            rb_state8.var_23 = (rb_state8.var_23) - (rb_state8.var_6);
            rb_state8.var_24 = (rb_state8.var_24) + (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_7) : (0xe3023add));
            rb_state8.var_25 = (rb_state8.var_25) - (((((0x4c35398a) + (rb_state8.var_22)) - (rb_state8.var_21)) + (rb_state8.var_9)) ^ (0xbcce004b));
            rb_state8.var_26 = (rb_state8.var_26) - (rb_state8.var_5);
            rb_state8.var_27 = (rb_state8.var_27) ^ (((0x30954786) + (rb_state8.var_23)) ^ (0x4a10961e));
            rb_state8.var_28 = (rb_state8.var_28) ^ (((((0xe26a4227) - (rb_state8.var_25)) + (rb_state8.var_24)) ^ (rb_state8.var_10)) ^ (0xfe31bb91));
            rb_state8.var_29 = (rb_state8.var_29) + (((rb_state8.var_26) == (0x0)) ? (rb_state8.var_27) : (0x1df3e7c4));
            rb_state8.var_30 = (rb_state8.var_30) ^ (((rb_state8.var_28) == (0x46f51e03)) ? (rb_state8.var_29) : (0x58d8eedb));
            rb_state8.var_20 = (rb_state8.var_20) + (rb_state8.var_30);
            rb_state8.var_3 = rb_state8.var_20;
            pthread_mutex_unlock(&(rb_state8.lock_43));
        }
        if ((rb_state8.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state8.lock_43));
            rb_state8.var_32 = 0x9cafd63a;
            rb_state8.var_33 = (rb_state8.var_33) - (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_12) : (0x7476922c));
            rb_state8.var_34 = (rb_state8.var_34) - ((0xee009092) - (rb_state8.var_13));
            rb_state8.var_35 = (rb_state8.var_35) ^ (((((0x1aa1871) + (rb_state8.var_14)) - (0xf8dbb1db)) + (rb_state8.var_33)) + (rb_state8.var_32));
            rb_state8.var_36 = (rb_state8.var_36) ^ (((0x57b38122) ^ (rb_state8.var_34)) + (0x6f7e3799));
            rb_state8.var_37 = (rb_state8.var_37) - (((0xe686ca4e) - (rb_state8.var_35)) ^ (0xc69326c7));
            rb_state8.var_38 = (rb_state8.var_38) ^ (((0x78025964) ^ (rb_state8.var_36)) + (0xeb0869f2));
            rb_state8.var_39 = (rb_state8.var_39) - (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_37) : (0x5b071828));
            rb_state8.var_40 = (rb_state8.var_40) + (rb_state8.var_38);
            rb_state8.var_41 = (rb_state8.var_41) - (rb_state8.var_39);
            rb_state8.var_42 = (rb_state8.var_42) + (((((0x8572a360) - (rb_state8.var_40)) - (0x563b4ade)) + (rb_state8.var_41)) + (rb_state8.var_16));
            rb_state8.var_31 = (rb_state8.var_31) - (rb_state8.var_42);
            rb_state8.var_3 = rb_state8.var_31;
            pthread_mutex_unlock(&(rb_state8.lock_43));
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_2) == (0x6e46d060)) {
            rb_state13.var_4 = 0x3e37810c;
            rb_state13.var_5 = (rb_state13.var_5) - (((((0x5cdd1cd1) - (rb_state13.var_3)) ^ (rb_state13.var_4)) ^ (rb_state13.var_4)) - (rb_state13.var_5));
            rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_5);
        }
        #endif
        #ifdef RACEBENCH_BUG_15
        if ((rb_state15.var_4) == (0x60b8caa9)) {
            pthread_mutex_lock(&(rb_state15.lock_56));
            rb_state15.var_32 = 0xcbcb6c92;
            rb_state15.var_33 = 0x25392534;
            rb_state15.var_34 = (rb_state15.var_34) - (rb_state15.var_23);
            rb_state15.var_35 = (rb_state15.var_35) + (rb_state15.var_32);
            rb_state15.var_36 = (rb_state15.var_36) + (((0xf4c7fc39) + (0x3ff2d4df)) ^ (rb_state15.var_22));
            rb_state15.var_37 = (rb_state15.var_37) - (((rb_state15.var_33) == (0x25392534)) ? (rb_state15.var_34) : (0xa89a6456));
            rb_state15.var_38 = (rb_state15.var_38) + (rb_state15.var_35);
            rb_state15.var_39 = (rb_state15.var_39) + (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_36) : (0x373c9847));
            rb_state15.var_40 = (rb_state15.var_40) ^ (((rb_state15.var_37) == (0x0)) ? (rb_state15.var_38) : (0x84c837b8));
            rb_state15.var_41 = (rb_state15.var_41) ^ (rb_state15.var_39);
            rb_state15.var_42 = (rb_state15.var_42) - (rb_state15.var_40);
            rb_state15.var_43 = (rb_state15.var_43) + (rb_state15.var_41);
            rb_state15.var_44 = (rb_state15.var_44) - (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_42) : (0xe21720a9));
            rb_state15.var_45 = (rb_state15.var_45) - (((0x102da4ae) - (rb_state15.var_43)) ^ (0x6ab2972d));
            rb_state15.var_46 = (rb_state15.var_46) ^ (((rb_state15.var_26) == (0x0)) ? (rb_state15.var_44) : (0x7ac7c137));
            rb_state15.var_47 = (rb_state15.var_47) + (((((0x65ca8b45) ^ (rb_state15.var_46)) - (rb_state15.var_45)) + (0x7a13a368)) + (rb_state15.var_27));
            rb_state15.var_31 = (rb_state15.var_31) ^ (rb_state15.var_47);
            rb_state15.var_8 = rb_state15.var_31;
            pthread_mutex_unlock(&(rb_state15.lock_56));
        }
        if ((rb_state15.var_4) == (0x60b8caa9)) {
            pthread_mutex_lock(&(rb_state15.lock_56));
            rb_state15.var_49 = 0xaeb81638;
            rb_state15.var_50 = 0xfd138c10;
            rb_state15.var_51 = (rb_state15.var_51) - (((((0x4496c99e) + (rb_state15.var_29)) - (rb_state15.var_30)) ^ (rb_state15.var_49)) - (rb_state15.var_50));
            rb_state15.var_52 = (rb_state15.var_52) + (((rb_state15.var_31) == (0xd9d5cffa)) ? (rb_state15.var_28) : (0x686e6052));
            rb_state15.var_53 = (rb_state15.var_53) ^ (((rb_state15.var_32) == (0xcbcb6c92)) ? (rb_state15.var_51) : (0x9b34061e));
            rb_state15.var_54 = (rb_state15.var_54) - (((rb_state15.var_33) == (0x25392534)) ? (rb_state15.var_52) : (0xbc72a704));
            rb_state15.var_55 = (rb_state15.var_55) - (((rb_state15.var_53) == (0x12e4ac6a)) ? (rb_state15.var_54) : (0x3b67776d));
            rb_state15.var_48 = (rb_state15.var_48) ^ (rb_state15.var_55);
            rb_state15.var_8 = rb_state15.var_48;
            pthread_mutex_unlock(&(rb_state15.lock_56));
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