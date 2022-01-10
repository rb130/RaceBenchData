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
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
        rb_state0.var_2 = (rb_state0.var_2) + ((0x1bd9966f) ^ (rb_state0.var_2));
        rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_1);
        rb_state0.var_2 = (rb_state0.var_2) ^ ((0x810003b3) + (rb_state0.var_0));
        rb_state0.var_2 = (rb_state0.var_2) - (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_3) : (0x8e954deb));
        if ((rb_state0.var_2) == (0xa5afe6d0)) {
            if ((rb_state0.var_4) != (0x0)) {
                if (!((rb_state0.var_4) == (rb_state0.var_30))) {
                    racebench_trigger(0);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) - ((0x6ca0448) ^ (rb_state12.var_4));
        if ((rb_state12.var_1) == (0x43dae000)) {
            rb_state12.var_6 = 0xffdfd24;
            rb_state12.var_7 = 0x2728017a;
            rb_state12.var_8 = 0x7806d710;
            rb_state12.var_9 = (rb_state12.var_9) + (0xb73b4b1b);
            rb_state12.var_10 = (rb_state12.var_10) + (rb_state12.var_6);
            rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_5) == (0x0)) ? (rb_state12.var_8) : (0x39a57a76));
            rb_state12.var_12 = (rb_state12.var_12) + (((((0x4d2a98c7) - (0x9d637188)) ^ (rb_state12.var_9)) ^ (rb_state12.var_7)) - (0xb7ab0923));
            rb_state12.var_13 = (rb_state12.var_13) ^ (rb_state12.var_10);
            rb_state12.var_14 = (rb_state12.var_14) - (rb_state12.var_11);
            rb_state12.var_15 = (rb_state12.var_15) - (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_13) : (0x4709ac87));
            rb_state12.var_16 = (rb_state12.var_16) + (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_15) : (0xbd5d6596));
            rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_16);
        }
        #endif
        float distance = dist(points->p[k], points->p[0], points->dim);
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) ^ ((0xc72d7de7) + (rb_state0.var_1));
        rb_state0.var_3 = (rb_state0.var_3) - (20740 < rb_input_size ? (uint32_t)rb_input[20740] : 0x8cb6fd8e);
        rb_state0.var_3 = (rb_state0.var_3) ^ (8738 < rb_input_size ? (uint32_t)rb_input[8738] : 0x28e11d1b);
        rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_1);
        if ((rb_state0.var_2) == (0xf5b560fc)) {
            rb_state0.var_19 = 0xc79ec07d;
            rb_state0.var_20 = 0x9cd5a1e0;
            rb_state0.var_21 = 0x489a1655;
            rb_state0.var_22 = (rb_state0.var_22) ^ (rb_state0.var_10);
            rb_state0.var_23 = (rb_state0.var_23) - (((0x53279799) - (rb_state0.var_11)) + (rb_state0.var_19));
            rb_state0.var_24 = (rb_state0.var_24) + (((0x67b6741d) ^ (rb_state0.var_12)) - (rb_state0.var_21));
            rb_state0.var_25 = (rb_state0.var_25) + (((0x3fa6f208) + (rb_state0.var_20)) - (rb_state0.var_13));
            rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_14) == (0x0)) ? (rb_state0.var_22) : (0x59846227));
            rb_state0.var_27 = (rb_state0.var_27) + (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_24) : (0x47e848ca));
            rb_state0.var_28 = (rb_state0.var_28) ^ (((((0x155419b3) ^ (rb_state0.var_26)) ^ (0xea6f7b4b)) ^ (rb_state0.var_25)) + (rb_state0.var_15));
            rb_state0.var_29 = (rb_state0.var_29) - (((((0x274209ed) ^ (rb_state0.var_27)) - (rb_state0.var_16)) + (rb_state0.var_17)) ^ (rb_state0.var_28));
            rb_state0.var_18 = (rb_state0.var_18) + (rb_state0.var_29);
            rb_state0.var_4 = rb_state0.var_18;
        }
        if ((rb_state0.var_2) == (0xa5afe6d0)) {
            pthread_mutex_lock(&(rb_state0.lock_39));
            rb_state0.var_31 = 0x19c4a309;
            rb_state0.var_32 = 0xcd7bae54;
            rb_state0.var_33 = (rb_state0.var_33) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_32) : (0x7155450d));
            rb_state0.var_34 = (rb_state0.var_34) ^ (((((0x8367335c) ^ (rb_state0.var_18)) + (0xa1460d0c)) ^ (rb_state0.var_31)) ^ (0xddb7741c));
            rb_state0.var_35 = (rb_state0.var_35) ^ (rb_state0.var_33);
            rb_state0.var_36 = (rb_state0.var_36) + (((0x5abdae23) ^ (0xe61ff202)) ^ (rb_state0.var_34));
            rb_state0.var_37 = (rb_state0.var_37) - (((0xcd6162c7) + (rb_state0.var_35)) ^ (rb_state0.var_20));
            rb_state0.var_38 = (rb_state0.var_38) + (((((0x18341290) - (rb_state0.var_36)) - (rb_state0.var_21)) ^ (0x5c1fdf1d)) + (rb_state0.var_37));
            rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_38);
            rb_state0.var_4 = rb_state0.var_30;
            pthread_mutex_unlock(&(rb_state0.lock_39));
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_3 = (rb_state12.var_3) + (rb_state12.var_4);
        if ((rb_state12.var_1) == (0x43dae000)) {
            rb_state12.var_5 = rb_state12.var_2;
        }
        if ((rb_state12.var_1) == (0x43dae000)) {
            if (!((rb_state12.var_2) == (rb_state12.var_5))) {
                racebench_trigger(12);
            }
        }
        #endif
        points->p[k].cost = distance * points->p[k].weight;
        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - (0x2f0aa9b4);
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
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0x0)) {
        rb_state16.var_15 = 0xd7dd62bc;
        rb_state16.var_16 = 0xf2f33874;
        rb_state16.var_17 = (rb_state16.var_17) ^ (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0xac558620));
        rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_15) : (0x1eb50cd0));
        rb_state16.var_19 = (rb_state16.var_19) ^ (rb_state16.var_17);
        rb_state16.var_20 = (rb_state16.var_20) - (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_18) : (0x94a3a038));
        rb_state16.var_21 = (rb_state16.var_21) + (((rb_state16.var_19) == (0x0)) ? (rb_state16.var_20) : (0xf19a0e46));
        rb_state16.var_14 = (rb_state16.var_14) - (rb_state16.var_21);
        rb_state16.var_4 = rb_state16.var_14;
    }
    if ((rb_state16.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state16.lock_41));
        rb_state16.var_23 = 0x40ba8c6d;
        rb_state16.var_24 = 0xfaa6f3a5;
        rb_state16.var_25 = (rb_state16.var_25) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_11) : (0xec89cfc8));
        rb_state16.var_26 = (rb_state16.var_26) + (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_13) : (0x903500ad));
        rb_state16.var_27 = (rb_state16.var_27) - (((((0xd94730f4) - (rb_state16.var_10)) ^ (0xf3d8e153)) - (rb_state16.var_15)) - (rb_state16.var_23));
        rb_state16.var_28 = (rb_state16.var_28) - (((rb_state16.var_16) == (0x0)) ? (rb_state16.var_24) : (0x254b19c6));
        rb_state16.var_29 = (rb_state16.var_29) ^ (((rb_state16.var_17) == (0x0)) ? (rb_state16.var_25) : (0xd4c14f88));
        rb_state16.var_30 = (rb_state16.var_30) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0x643ff3f6));
        rb_state16.var_31 = (rb_state16.var_31) + (((rb_state16.var_18) == (0x0)) ? (rb_state16.var_28) : (0x2ad11caf));
        rb_state16.var_32 = (rb_state16.var_32) ^ (rb_state16.var_29);
        rb_state16.var_33 = (rb_state16.var_33) ^ (((((0x8dc10832) + (rb_state16.var_31)) - (rb_state16.var_30)) + (rb_state16.var_19)) + (0x28433fa5));
        rb_state16.var_34 = (rb_state16.var_34) + (rb_state16.var_32);
        rb_state16.var_35 = (rb_state16.var_35) + (rb_state16.var_33);
        rb_state16.var_36 = (rb_state16.var_36) - (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_34) : (0xb92585df));
        rb_state16.var_37 = (rb_state16.var_37) + (((rb_state16.var_21) == (0x0)) ? (rb_state16.var_35) : (0x1d463bc3));
        rb_state16.var_38 = (rb_state16.var_38) - (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_36) : (0xdebfb033));
        rb_state16.var_39 = (rb_state16.var_39) ^ (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_37) : (0xbb70fc55));
        rb_state16.var_40 = (rb_state16.var_40) + (((rb_state16.var_38) == (0x0)) ? (rb_state16.var_39) : (0xe3c61d71));
        rb_state16.var_22 = (rb_state16.var_22) ^ (rb_state16.var_40);
        rb_state16.var_4 = rb_state16.var_22;
        pthread_mutex_unlock(&(rb_state16.lock_41));
    }
    #endif
    hiz = loz = 0.0;
    long numberOfPoints = points->num;
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - ((0x4d8a4da) ^ (0x33164995));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_1);
    #endif
    long ptDimension = points->dim;

    long bsize = points->num / nproc;
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_1) == (0x801fb13c)) ? (rb_state16.var_0) : (0x4fab69fb));
    #endif
    long k1 = bsize * pid;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) - ((0xda3cdb68) - (rb_state14.var_0));
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_2) : (0xe00b994e));
    #endif
    long k2 = k1 + bsize;
    if (pid == nproc - 1) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ ((0xde588a2) ^ (rb_state4.var_0));
        #endif
        k2 = points->num;
    }

#ifdef ENABLE_THREADS
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x4302a63f);
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) + (53815 < rb_input_size ? (uint32_t)rb_input[53815] : 0x759bc838);
    #endif
    pthread_barrier_wait(barrier);
#endif

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (59633 < rb_input_size ? (uint32_t)rb_input[59633] : 0x215fbea2);
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ ((0x24ef47a3) + (0x771a1ac5));
    #endif
    double myhiz = 0;
    for (long kk = k1; kk < k2; kk++) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_2 = (rb_state4.var_2) - ((0xf85ce865) ^ (0xe8a257fa));
        rb_state4.var_3 = (rb_state4.var_3) + ((0x949cae0b) + (0xeeb1baa1));
        rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_3) == (0x834e68ac)) ? (rb_state4.var_2) : (0xd460e8ac));
        rb_state4.var_1 = (rb_state4.var_1) ^ ((0x59c8d298) ^ (rb_state4.var_2));
        rb_state4.var_3 = (rb_state4.var_3) + (rb_state4.var_1);
        rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_3);
        if ((rb_state4.var_1) == (0xd06880d6)) {
            pthread_mutex_lock(&(rb_state4.lock_17));
            rb_state4.var_4 = rb_state4.var_2;
            pthread_mutex_unlock(&(rb_state4.lock_17));
        }
        if ((rb_state4.var_1) == (0xe438e3c0)) {
            rb_state4.var_5 = 0xa2bf747c;
            rb_state4.var_6 = 0x1325b4fd;
            rb_state4.var_7 = 0xbf02c397;
            rb_state4.var_8 = (rb_state4.var_8) - ((0x54e84352) - (rb_state4.var_4));
            rb_state4.var_9 = (rb_state4.var_9) - (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_6) : (0xadc31b44));
            rb_state4.var_10 = (rb_state4.var_10) - (((0x1b7f35af) ^ (rb_state4.var_5)) ^ (0x9b482f82));
            rb_state4.var_11 = (rb_state4.var_11) ^ (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_8) : (0xd1ce14a7));
            rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_9) : (0x8a7dddf));
            rb_state4.var_13 = (rb_state4.var_13) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0x56b98994));
            rb_state4.var_14 = (rb_state4.var_14) + (((0x37dc602) - (0xffde0089)) - (rb_state4.var_12));
            rb_state4.var_15 = (rb_state4.var_15) ^ (((0xa6097513) ^ (rb_state4.var_13)) - (0xce685044));
            rb_state4.var_16 = (rb_state4.var_16) + (((((0xced3b3d2) - (rb_state4.var_14)) ^ (rb_state4.var_7)) - (0xa1b8c177)) - (rb_state4.var_15));
            rb_state4.var_2 = (rb_state4.var_2) ^ (rb_state4.var_16);
        }
        if ((rb_state4.var_1) == (0xe438e3c0)) {
            if (!((rb_state4.var_2) == (rb_state4.var_4))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_1 = (rb_state10.var_1) ^ (0x6e38f2da);
        rb_state10.var_2 = (rb_state10.var_2) ^ (((rb_state10.var_1) == (0x46ec46c0)) ? (rb_state10.var_0) : (0x9cac15a6));
        rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_3);
        rb_state10.var_2 = (rb_state10.var_2) + ((0xc6d2e34a) + (rb_state10.var_3));
        rb_state10.var_0 = (rb_state10.var_0) ^ (37716 < rb_input_size ? (uint32_t)rb_input[37716] : 0x48e4f39d);
        rb_state10.var_1 = (rb_state10.var_1) + ((0x67e0c84b) - (rb_state10.var_2));
        rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_1) == (0xc7239d75)) ? (rb_state10.var_0) : (0x7ef8c4cc));
        if ((rb_state10.var_2) == (0x39555d4a)) {
            rb_state10.var_6 = 0x1;
        }
        if ((rb_state10.var_2) == (0x6edaa29e)) {
            pthread_mutex_lock(&(rb_state10.lock_22));
            if ((rb_state10.var_6) != (0x0)) {
                rb_state10.var_23 = !((rb_state10.var_4) == (rb_state10.var_5));
            }
            pthread_mutex_unlock(&(rb_state10.lock_22));
        }
        if ((rb_state10.var_2) == (0x6edaa29e)) {
            pthread_mutex_lock(&(rb_state10.lock_22));
            rb_state10.var_7 = 0x1f37a536;
            rb_state10.var_8 = 0x7a137bfb;
            rb_state10.var_9 = (rb_state10.var_9) - ((0xd1c28267) + (0x135670ab));
            rb_state10.var_10 = (rb_state10.var_10) - (((((0x54d4a7a0) ^ (0x7302526a)) ^ (rb_state10.var_5)) + (rb_state10.var_4)) + (rb_state10.var_8));
            rb_state10.var_11 = (rb_state10.var_11) ^ (rb_state10.var_7);
            rb_state10.var_12 = (rb_state10.var_12) ^ (((0x9477bb17) - (0x60e676c5)) + (rb_state10.var_9));
            rb_state10.var_13 = (rb_state10.var_13) + (((rb_state10.var_6) == (0x0)) ? (rb_state10.var_10) : (0x508f986b));
            rb_state10.var_14 = (rb_state10.var_14) + (((0x4b011316) ^ (rb_state10.var_11)) ^ (0xfc1caf13));
            rb_state10.var_15 = (rb_state10.var_15) - (rb_state10.var_12);
            rb_state10.var_16 = (rb_state10.var_16) + (rb_state10.var_13);
            rb_state10.var_17 = (rb_state10.var_17) ^ (((((0x11add865) ^ (0xfc65c438)) ^ (rb_state10.var_7)) ^ (rb_state10.var_14)) + (rb_state10.var_15));
            rb_state10.var_18 = (rb_state10.var_18) - (((0x4f030c4c) ^ (rb_state10.var_8)) - (rb_state10.var_16));
            rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_17);
            rb_state10.var_20 = (rb_state10.var_20) + (rb_state10.var_18);
            rb_state10.var_21 = (rb_state10.var_21) - (((((0xff149642) - (rb_state10.var_9)) ^ (rb_state10.var_10)) + (rb_state10.var_20)) - (rb_state10.var_19));
            rb_state10.var_5 = (rb_state10.var_5) ^ (rb_state10.var_21);
            rb_state10.var_4 = rb_state10.var_5;
            pthread_mutex_unlock(&(rb_state10.lock_22));
        }
        rb_state10.var_11 = (rb_state10.var_11) - (rb_state10.var_12);
        rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_14) : (0x6d2f40ab));
        rb_state10.var_16 = (rb_state10.var_16) - (21585 < rb_input_size ? (uint32_t)rb_input[21585] : 0xb3ee1d97);
        rb_state10.var_17 = (rb_state10.var_17) - (1129 < rb_input_size ? (uint32_t)rb_input[1129] : 0x40e2c6ac);
        rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_19);
        if ((rb_state10.var_20) == (0x1b7eee69)) {
            pthread_mutex_lock(&(rb_state10.lock_42));
            rb_state10.var_26 = 0x7de06aa7;
            rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_21) : (0x1cad2b9d));
            rb_state10.var_28 = (rb_state10.var_28) + (((rb_state10.var_24) == (0x0)) ? (rb_state10.var_26) : (0xc0041559));
            rb_state10.var_29 = (rb_state10.var_29) + (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_27) : (0x41803479));
            rb_state10.var_30 = (rb_state10.var_30) - (rb_state10.var_28);
            rb_state10.var_31 = (rb_state10.var_31) ^ (((0xf8ce6f24) - (0xef7e3298)) - (rb_state10.var_29));
            rb_state10.var_32 = (rb_state10.var_32) - (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_30) : (0xd2fb1e7));
            rb_state10.var_33 = (rb_state10.var_33) - (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_31) : (0xfc77697b));
            rb_state10.var_34 = (rb_state10.var_34) - (((((0x4e543bc8) - (rb_state10.var_28)) - (rb_state10.var_33)) ^ (rb_state10.var_32)) - (0x416cd050));
            rb_state10.var_25 = (rb_state10.var_25) + (rb_state10.var_34);
            rb_state10.var_24 = rb_state10.var_25;
            pthread_mutex_unlock(&(rb_state10.lock_42));
        }
        if ((rb_state10.var_20) == (0x1b7eee69)) {
            rb_state10.var_35 = 0xd2847b4;
            rb_state10.var_36 = 0xd7028eb2;
            rb_state10.var_37 = 0x40e3334d;
            rb_state10.var_38 = (rb_state10.var_38) + (((0xdcf2a6c5) ^ (rb_state10.var_35)) + (0x233acc0e));
            rb_state10.var_39 = (rb_state10.var_39) + (((((0xad0fe8b2) ^ (rb_state10.var_29)) - (rb_state10.var_36)) ^ (rb_state10.var_30)) + (rb_state10.var_37));
            rb_state10.var_40 = (rb_state10.var_40) ^ (((0x1386f91e) ^ (0x55957536)) ^ (rb_state10.var_38));
            rb_state10.var_41 = (rb_state10.var_41) - (((rb_state10.var_39) == (0x0)) ? (rb_state10.var_40) : (0x5161a86a));
            rb_state10.var_24 = (rb_state10.var_24) + (rb_state10.var_41);
        }
        if ((rb_state10.var_20) == (0x1b7eee69)) {
            if (!((rb_state10.var_24) == (rb_state10.var_25))) {
                racebench_trigger(10);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_1 = (rb_state12.var_1) + ((0x6ee5a7f6) ^ (rb_state12.var_0));
        rb_state12.var_1 = (rb_state12.var_1) + (47615 < rb_input_size ? (uint32_t)rb_input[47615] : 0x2d5f5c31);
        rb_state12.var_2 = (rb_state12.var_2) ^ (0x22f13c68);
        rb_state12.var_2 = (rb_state12.var_2) - (((rb_state12.var_1) == (0x4e21ed70)) ? (rb_state12.var_0) : (0xdfdeda04));
        rb_state12.var_2 = (rb_state12.var_2) ^ (rb_state12.var_0);
        rb_state12.var_3 = (rb_state12.var_3) - (0x3d9dad75);
        #endif
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) ^ (0xca0b43c9);
        rb_state14.var_1 = (rb_state14.var_1) - ((0xbbf77e96) ^ (rb_state14.var_2));
        rb_state14.var_0 = (rb_state14.var_0) - (30565 < rb_input_size ? (uint32_t)rb_input[30565] : 0x9ba7f857);
        rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_0) == (0x37423482)) ? (rb_state14.var_2) : (0x1c9c8a5f));
        rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_3);
        rb_state14.var_4 = (rb_state14.var_4) + ((0xdb6232ac) - (0xce31b7d7));
        rb_state14.var_4 = (rb_state14.var_4) ^ (((rb_state14.var_4) == (0xd307ad5)) ? (rb_state14.var_3) : (0x2168a10c));
        rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_3) == (0x0)) ? (rb_state14.var_2) : (0xee9aa947));
        if ((rb_state14.var_4) == (0x4bf7cf05)) {
            usleep(100);
        }
        if ((rb_state14.var_4) == (0x7840e8d6)) {
            pthread_mutex_lock(&(rb_state14.lock_16));
            rb_state14.var_7 = 0x9bd55ca7;
            rb_state14.var_8 = 0xe480f497;
            rb_state14.var_9 = (rb_state14.var_9) + (rb_state14.var_6);
            rb_state14.var_10 = (rb_state14.var_10) ^ (((0x9c62c0be) - (rb_state14.var_5)) + (rb_state14.var_7));
            rb_state14.var_11 = (rb_state14.var_11) + (((((0x3920a0be) ^ (rb_state14.var_8)) + (rb_state14.var_7)) + (0x7424f5)) + (rb_state14.var_8));
            rb_state14.var_12 = (rb_state14.var_12) + (((((0x3cea734c) - (0x580de122)) ^ (rb_state14.var_10)) + (rb_state14.var_9)) + (rb_state14.var_9));
            rb_state14.var_13 = (rb_state14.var_13) - (((0xccf0f53c) + (rb_state14.var_10)) ^ (rb_state14.var_11));
            rb_state14.var_14 = (rb_state14.var_14) ^ (((rb_state14.var_11) == (0x0)) ? (rb_state14.var_12) : (0x608d471b));
            rb_state14.var_15 = (rb_state14.var_15) + (((((0x832a5654) ^ (rb_state14.var_14)) ^ (rb_state14.var_13)) ^ (rb_state14.var_13)) ^ (rb_state14.var_12));
            rb_state14.var_5 = (rb_state14.var_5) + (rb_state14.var_15);
            rb_state14.var_6 = rb_state14.var_5;
            pthread_mutex_unlock(&(rb_state14.lock_16));
        }
        if ((rb_state14.var_4) == (0x7840e8d6)) {
            rb_state14.var_17 = !((rb_state14.var_6) == (rb_state14.var_5));
        }
        rb_state14.var_14 = (rb_state14.var_14) - ((0x81a575cc) - (0x522ab75d));
        rb_state14.var_15 = (rb_state14.var_15) ^ (rb_state14.var_17);
        rb_state14.var_11 = (rb_state14.var_11) ^ (0x5af817bf);
        if ((rb_state14.var_6) == (0x7846a733)) {
            rb_state14.var_30 = 0xd54455f0;
            rb_state14.var_31 = 0x3e4bb5b9;
            rb_state14.var_32 = (rb_state14.var_32) ^ (((rb_state14.var_31) == (0x0)) ? (rb_state14.var_22) : (0x71a97c30));
            rb_state14.var_33 = (rb_state14.var_33) ^ ((0xa7ee284e) + (0x7900751d));
            rb_state14.var_34 = (rb_state14.var_34) - (((0x18ae4a75) ^ (0xf753f8af)) ^ (rb_state14.var_30));
            rb_state14.var_35 = (rb_state14.var_35) + (((((0x8df75d1b) ^ (0x5cb986c2)) - (0x69132724)) - (rb_state14.var_32)) ^ (rb_state14.var_33));
            rb_state14.var_36 = (rb_state14.var_36) ^ (rb_state14.var_34);
            rb_state14.var_37 = (rb_state14.var_37) - (((((0x675da810) - (rb_state14.var_36)) ^ (rb_state14.var_23)) ^ (rb_state14.var_35)) ^ (rb_state14.var_24));
            rb_state14.var_18 = (rb_state14.var_18) ^ (rb_state14.var_37);
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_2 = (rb_state19.var_2) - (12116 < rb_input_size ? (uint32_t)rb_input[12116] : 0xac7a2f0f);
        rb_state19.var_0 = (rb_state19.var_0) + (0x5852ecd0);
        rb_state19.var_1 = (rb_state19.var_1) + (0xa09e0e67);
        rb_state19.var_3 = (rb_state19.var_3) ^ ((0x49a1a129) ^ (0xd64c289b));
        rb_state19.var_3 = (rb_state19.var_3) - (0xc807d5f2);
        rb_state19.var_3 = (rb_state19.var_3) ^ (0x755ef6e6);
        rb_state19.var_2 = (rb_state19.var_2) ^ ((0x4b6655f2) - (0xb70929d2));
        if ((rb_state19.var_3) == (0xde56534a)) {
            rb_state19.var_6 = 0x248c2af0;
            rb_state19.var_7 = (rb_state19.var_7) - (((((0x9091c2a5) ^ (rb_state19.var_5)) + (0x55d5059a)) + (rb_state19.var_6)) ^ (0xadde0612));
            rb_state19.var_5 = (rb_state19.var_5) - (rb_state19.var_7);
            rb_state19.var_4 = rb_state19.var_5;
        }
        if ((rb_state19.var_3) == (0xceaf3e0)) {
            if ((rb_state19.var_4) != (0x0)) {
                rb_state19.var_19 = !((rb_state19.var_4) == (rb_state19.var_8));
            }
        }
        if ((rb_state19.var_3) == (0xceaf3e0)) {
            pthread_mutex_lock(&(rb_state19.lock_18));
            rb_state19.var_9 = 0x4fc5672e;
            rb_state19.var_10 = 0x792c1d05;
            rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_8) : (0x76c8f1fd));
            rb_state19.var_12 = (rb_state19.var_12) - (0xeec1b5fc);
            rb_state19.var_13 = (rb_state19.var_13) ^ (((((0xed16131d) + (0x16fe1427)) - (rb_state19.var_10)) ^ (rb_state19.var_7)) + (0x5ae88e63));
            rb_state19.var_14 = (rb_state19.var_14) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_11) : (0x15536647));
            rb_state19.var_15 = (rb_state19.var_15) ^ (((0x42ee6aa4) + (rb_state19.var_12)) - (rb_state19.var_10));
            rb_state19.var_16 = (rb_state19.var_16) - (((((0x343ecfe4) ^ (0xe70846ef)) ^ (rb_state19.var_11)) ^ (rb_state19.var_13)) - (rb_state19.var_14));
            rb_state19.var_17 = (rb_state19.var_17) ^ (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_16) : (0xe1085b1d));
            rb_state19.var_8 = (rb_state19.var_8) - (rb_state19.var_17);
            rb_state19.var_4 = rb_state19.var_8;
            pthread_mutex_unlock(&(rb_state19.lock_18));
        }
        rb_state19.var_12 = (rb_state19.var_12) - (rb_state19.var_13);
        rb_state19.var_14 = (rb_state19.var_14) - (rb_state19.var_15);
        rb_state19.var_16 = (rb_state19.var_16) + (63200 < rb_input_size ? (uint32_t)rb_input[63200] : 0x16f4657a);
        rb_state19.var_17 = (rb_state19.var_17) ^ (59271 < rb_input_size ? (uint32_t)rb_input[59271] : 0x2d9eb6c5);
        rb_state19.var_5 = (rb_state19.var_5) + (35485 < rb_input_size ? (uint32_t)rb_input[35485] : 0xcbc75163);
        if ((rb_state19.var_19) == (0x1)) {
            pthread_mutex_lock(&(rb_state19.lock_42));
            rb_state19.var_22 = 0x54cf3a16;
            rb_state19.var_23 = 0x7b018976;
            rb_state19.var_24 = (rb_state19.var_24) - (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_21) : (0x33e25997));
            rb_state19.var_25 = (rb_state19.var_25) + (((rb_state19.var_23) == (0x0)) ? (rb_state19.var_22) : (0x5090db04));
            rb_state19.var_26 = (rb_state19.var_26) - (((((0x33e79118) + (rb_state19.var_24)) + (rb_state19.var_23)) - (rb_state19.var_25)) + (rb_state19.var_20));
            rb_state19.var_27 = (rb_state19.var_27) ^ (((rb_state19.var_24) == (0x0)) ? (rb_state19.var_25) : (0xd046f63));
            rb_state19.var_28 = (rb_state19.var_28) ^ (rb_state19.var_26);
            rb_state19.var_29 = (rb_state19.var_29) ^ (((rb_state19.var_26) == (0x0)) ? (rb_state19.var_27) : (0x113ad4e6));
            rb_state19.var_30 = (rb_state19.var_30) + (rb_state19.var_28);
            rb_state19.var_31 = (rb_state19.var_31) - (((((0x4b2d4ec5) ^ (rb_state19.var_28)) + (rb_state19.var_29)) + (rb_state19.var_27)) + (rb_state19.var_30));
            rb_state19.var_21 = (rb_state19.var_21) + (rb_state19.var_31);
            rb_state19.var_20 = rb_state19.var_21;
            pthread_mutex_unlock(&(rb_state19.lock_42));
        }
        if ((rb_state19.var_19) == (0x1)) {
            rb_state19.var_32 = 0xc0cfc791;
            rb_state19.var_33 = 0x6ba25a77;
            rb_state19.var_34 = (rb_state19.var_34) - (rb_state19.var_30);
            rb_state19.var_35 = (rb_state19.var_35) - (48833 < rb_input_size ? (uint32_t)rb_input[48833] : 0x85026733);
            rb_state19.var_36 = (rb_state19.var_36) ^ (rb_state19.var_29);
            rb_state19.var_37 = (rb_state19.var_37) - (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_32) : (0xa97ec5bb));
            rb_state19.var_38 = (rb_state19.var_38) ^ (((((0x7ce931e) ^ (rb_state19.var_34)) ^ (0xc7845dfa)) - (rb_state19.var_35)) ^ (rb_state19.var_31));
            rb_state19.var_39 = (rb_state19.var_39) - (((0xec2f7e17) - (rb_state19.var_36)) - (rb_state19.var_32));
            rb_state19.var_40 = (rb_state19.var_40) - (((((0x7ee846db) - (rb_state19.var_37)) - (rb_state19.var_33)) ^ (0xf4c15708)) + (rb_state19.var_38));
            rb_state19.var_41 = (rb_state19.var_41) - (((rb_state19.var_39) == (0x0)) ? (rb_state19.var_40) : (0xbc567509));
            rb_state19.var_20 = (rb_state19.var_20) ^ (rb_state19.var_41);
        }
        if ((rb_state19.var_19) == (0x1)) {
            pthread_mutex_lock(&(rb_state19.lock_42));
            if (!((rb_state19.var_20) == (rb_state19.var_21))) {
                racebench_trigger(19);
            }
            pthread_mutex_unlock(&(rb_state19.lock_42));
        }
        #endif
        myhiz += dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_9 = (rb_state14.var_9) ^ (65125 < rb_input_size ? (uint32_t)rb_input[65125] : 0xd89ed916);
    #endif
    hizs[pid] = myhiz;

#ifdef ENABLE_THREADS
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_5);
    if ((rb_state14.var_6) == (0x7846a733)) {
        pthread_mutex_lock(&(rb_state14.lock_38));
        rb_state14.var_20 = 0x163e2279;
        rb_state14.var_21 = (rb_state14.var_21) ^ (0x6f0e39b3);
        rb_state14.var_22 = (rb_state14.var_22) ^ (((0x2d798100) ^ (rb_state14.var_20)) - (0x2bccd19d));
        rb_state14.var_23 = (rb_state14.var_23) - (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_19) : (0x8dfc7284));
        rb_state14.var_24 = (rb_state14.var_24) ^ (rb_state14.var_21);
        rb_state14.var_25 = (rb_state14.var_25) + (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_22) : (0x919c7fbe));
        rb_state14.var_26 = (rb_state14.var_26) + (((0x8a72e556) - (rb_state14.var_23)) - (0xcadd50f9));
        rb_state14.var_27 = (rb_state14.var_27) - (((rb_state14.var_24) == (0x0)) ? (rb_state14.var_25) : (0x14e0985b));
        rb_state14.var_28 = (rb_state14.var_28) - (rb_state14.var_26);
        rb_state14.var_29 = (rb_state14.var_29) + (((((0xc661175b) - (0xb409ac0a)) ^ (rb_state14.var_27)) ^ (rb_state14.var_28)) + (rb_state14.var_21));
        rb_state14.var_19 = (rb_state14.var_19) + (rb_state14.var_29);
        rb_state14.var_18 = rb_state14.var_19;
        pthread_mutex_unlock(&(rb_state14.lock_38));
    }
    if ((rb_state14.var_6) == (0x7846a733)) {
        if (!((rb_state14.var_18) == (rb_state14.var_19))) {
            racebench_trigger(14);
        }
    }
    #endif
    pthread_barrier_wait(barrier);
#endif

    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_4 = (rb_state12.var_4) ^ (0x6549c81c);
        #endif
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
    if ((rb_state1.var_2) == (0x0)) {
        rb_state1.var_6 = 0xeb925df1;
        rb_state1.var_7 = (rb_state1.var_7) ^ (((0xf4e5860a) + (0xa12cd6df)) ^ (rb_state1.var_5));
        rb_state1.var_8 = (rb_state1.var_8) + ((0x825557b3) - (0xdae772b0));
        rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0xc274343d));
        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_8) == (0x0)) ? (rb_state1.var_7) : (0xba9e6740));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((((0xd6bf3245) + (rb_state1.var_9)) ^ (rb_state1.var_8)) + (0xe3314130)) - (rb_state1.var_9));
        rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_10) == (0x0)) ? (rb_state1.var_10) : (0xbeeeab14));
        rb_state1.var_13 = (rb_state1.var_13) ^ (((0xda346e1b) - (rb_state1.var_11)) ^ (rb_state1.var_11));
        rb_state1.var_14 = (rb_state1.var_14) - (((0x7595449f) - (rb_state1.var_12)) - (rb_state1.var_12));
        rb_state1.var_15 = (rb_state1.var_15) - (((rb_state1.var_13) == (0x0)) ? (rb_state1.var_14) : (0xacf3c3f0));
        rb_state1.var_3 = (rb_state1.var_3) ^ (rb_state1.var_15);
    }
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) + (32496 < rb_input_size ? (uint32_t)rb_input[32496] : 0x2862163c);
    if ((rb_state2.var_3) == (0x0)) {
        pthread_mutex_lock(&(rb_state2.lock_28));
        rb_state2.var_18 = 0xca9c958a;
        rb_state2.var_19 = (rb_state2.var_19) ^ (((((0x8d728efd) - (rb_state2.var_18)) - (rb_state2.var_9)) ^ (rb_state2.var_8)) + (0x341b69c7));
        rb_state2.var_20 = (rb_state2.var_20) + (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_7) : (0x7a8c2ff8));
        rb_state2.var_21 = (rb_state2.var_21) ^ ((0x4bb0e199) ^ (rb_state2.var_11));
        rb_state2.var_22 = (rb_state2.var_22) - (rb_state2.var_19);
        rb_state2.var_23 = (rb_state2.var_23) + (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_20) : (0x8acc4e88));
        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_21) : (0xd6f9e8a4));
        rb_state2.var_25 = (rb_state2.var_25) ^ (rb_state2.var_22);
        rb_state2.var_26 = (rb_state2.var_26) - (((((0x455da93f) ^ (rb_state2.var_24)) + (0xf02ca147)) + (0x35b7e740)) ^ (rb_state2.var_23));
        rb_state2.var_27 = (rb_state2.var_27) + (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_26) : (0x88478531));
        rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_27);
        pthread_mutex_unlock(&(rb_state2.lock_28));
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
    if ((rb_state5.var_1) == (0xffffffc3)) {
        rb_state5.var_4 = rb_state5.var_2;
    }
    if ((rb_state5.var_1) == (0xffffffc3)) {
        if (!((rb_state5.var_2) == (rb_state5.var_4))) {
            racebench_trigger(5);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) + (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xd0d975ad));
    rb_state7.var_1 = (rb_state7.var_1) - (0xd25ca130);
    rb_state7.var_3 = (rb_state7.var_3) ^ (24864 < rb_input_size ? (uint32_t)rb_input[24864] : 0xbb4e4373);
    rb_state7.var_3 = (rb_state7.var_3) + ((0x4711652d) ^ (rb_state7.var_4));
    if ((rb_state7.var_0) == (0x0)) {
        rb_state7.var_18 = 0x3bea456a;
        rb_state7.var_19 = (rb_state7.var_19) ^ (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_18) : (0xfb2d5d2c));
        rb_state7.var_20 = (rb_state7.var_20) - (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_11) : (0x6c23e999));
        rb_state7.var_21 = (rb_state7.var_21) ^ (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_9) : (0x71e9f5a7));
        rb_state7.var_22 = (rb_state7.var_22) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_19) : (0x899526c7));
        rb_state7.var_23 = (rb_state7.var_23) ^ (rb_state7.var_20);
        rb_state7.var_24 = (rb_state7.var_24) ^ (((((0x4da59533) ^ (rb_state7.var_22)) ^ (rb_state7.var_21)) ^ (0xf2cd7deb)) ^ (rb_state7.var_15));
        rb_state7.var_25 = (rb_state7.var_25) ^ (((((0xc7a2a6b1) ^ (0xafef5f04)) - (rb_state7.var_23)) + (rb_state7.var_16)) - (rb_state7.var_24));
        rb_state7.var_5 = (rb_state7.var_5) + (rb_state7.var_25);
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_0) == (0xc4007246)) ? (rb_state8.var_1) : (0x18e6f6c0));
    if ((rb_state8.var_2) == (0x1d6b87e4)) {
        rb_state8.var_6 = 0x492b794e;
        rb_state8.var_7 = (rb_state8.var_7) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_7) : (0xaa30314a));
        rb_state8.var_8 = (rb_state8.var_8) ^ (rb_state8.var_8);
        rb_state8.var_9 = (rb_state8.var_9) + (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_9) : (0x476d72fc));
        rb_state8.var_10 = (rb_state8.var_10) ^ (rb_state8.var_6);
        rb_state8.var_11 = (rb_state8.var_11) ^ (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_7) : (0x918e49a6));
        rb_state8.var_12 = (rb_state8.var_12) - (((((0x3d8c1a7b) + (0x28c36597)) ^ (rb_state8.var_8)) + (rb_state8.var_9)) - (0xc4fc4a0d));
        rb_state8.var_13 = (rb_state8.var_13) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_10) : (0x7002c7f0));
        rb_state8.var_14 = (rb_state8.var_14) - (((rb_state8.var_11) == (0x0)) ? (rb_state8.var_11) : (0xa62dec61));
        rb_state8.var_15 = (rb_state8.var_15) ^ (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_12) : (0xb1b6bcb2));
        rb_state8.var_16 = (rb_state8.var_16) + (rb_state8.var_13);
        rb_state8.var_17 = (rb_state8.var_17) - (((((0x70781220) + (rb_state8.var_14)) - (rb_state8.var_13)) ^ (rb_state8.var_14)) - (rb_state8.var_15));
        rb_state8.var_18 = (rb_state8.var_18) + (((rb_state8.var_15) == (0x0)) ? (rb_state8.var_16) : (0x58c1f6ff));
        rb_state8.var_19 = (rb_state8.var_19) ^ (((((0x6fde57d4) + (rb_state8.var_17)) - (rb_state8.var_18)) - (rb_state8.var_16)) + (rb_state8.var_17));
        rb_state8.var_3 = (rb_state8.var_3) + (rb_state8.var_19);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) - ((0x8f46a483) + (0x41816074));
    rb_state10.var_1 = (rb_state10.var_1) + ((0x4041209d) - (rb_state10.var_0));
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (62910 < rb_input_size ? (uint32_t)rb_input[62910] : 0x352e1f1e);
    rb_state11.var_2 = (rb_state11.var_2) + (rb_state11.var_2);
    #endif
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_2) == (0xae5ccf8c)) {
        rb_state13.var_26 = 0x4fd35c18;
        rb_state13.var_27 = 0x8f78edcd;
        rb_state13.var_28 = (rb_state13.var_28) - (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_15) : (0x4d20f3a2));
        rb_state13.var_29 = (rb_state13.var_29) - (rb_state13.var_14);
        rb_state13.var_30 = (rb_state13.var_30) ^ (((((0x7a7f1a3f) ^ (rb_state13.var_26)) ^ (rb_state13.var_27)) ^ (rb_state13.var_17)) - (rb_state13.var_18));
        rb_state13.var_31 = (rb_state13.var_31) - (rb_state13.var_28);
        rb_state13.var_32 = (rb_state13.var_32) - (rb_state13.var_29);
        rb_state13.var_33 = (rb_state13.var_33) + (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_30) : (0x85c45e4));
        rb_state13.var_34 = (rb_state13.var_34) + (((0x7c98486d) ^ (rb_state13.var_31)) ^ (rb_state13.var_20));
        rb_state13.var_35 = (rb_state13.var_35) - (((((0xb014a5ec) ^ (rb_state13.var_32)) ^ (0xd2f1cf08)) - (rb_state13.var_21)) + (rb_state13.var_33));
        rb_state13.var_36 = (rb_state13.var_36) - (((0x9cfad39d) ^ (0xa4db30c8)) - (rb_state13.var_34));
        rb_state13.var_37 = (rb_state13.var_37) ^ (rb_state13.var_35);
        rb_state13.var_38 = (rb_state13.var_38) + (((((0x2608e7b2) + (0x8a138825)) ^ (rb_state13.var_22)) ^ (rb_state13.var_37)) - (rb_state13.var_36));
        rb_state13.var_5 = (rb_state13.var_5) ^ (rb_state13.var_38);
    }
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) - ((0xa6f229e) + (0x27c05003));
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_3) == (0x4079cf9c)) {
        rb_state15.var_5 = rb_state15.var_4;
    }
    if ((rb_state15.var_3) == (0x4079cf9c)) {
        pthread_mutex_lock(&(rb_state15.lock_9));
        if (!((rb_state15.var_4) == (rb_state15.var_5))) {
            racebench_trigger(15);
        }
        pthread_mutex_unlock(&(rb_state15.lock_9));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_4 = (rb_state17.var_4) + (19247 < rb_input_size ? (uint32_t)rb_input[19247] : 0x8d10da7d);
    if ((rb_state17.var_4) == (0xba)) {
        rb_state17.var_14 = 0x8102cb2;
        rb_state17.var_15 = 0xf826ef37;
        rb_state17.var_16 = (rb_state17.var_16) ^ (rb_state17.var_14);
        rb_state17.var_17 = (rb_state17.var_17) + (((((0x13e42761) - (rb_state17.var_9)) - (rb_state17.var_8)) + (rb_state17.var_15)) ^ (0x1dfaf862));
        rb_state17.var_18 = (rb_state17.var_18) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_16) : (0xbc43483c));
        rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_17) : (0xee385f2c));
        rb_state17.var_20 = (rb_state17.var_20) + (((((0x38e8212a) - (0x25a8601f)) + (0x89d4aeb)) + (rb_state17.var_18)) + (rb_state17.var_19));
        rb_state17.var_13 = (rb_state17.var_13) + (rb_state17.var_20);
        rb_state17.var_5 = rb_state17.var_13;
    }
    if ((rb_state17.var_4) == (0xba)) {
        pthread_mutex_lock(&(rb_state17.lock_35));
        rb_state17.var_22 = 0x3fcbd3a1;
        rb_state17.var_23 = 0xa9164540;
        rb_state17.var_24 = (rb_state17.var_24) ^ (39546 < rb_input_size ? (uint32_t)rb_input[39546] : 0xdd5f48e7);
        rb_state17.var_25 = (rb_state17.var_25) + (((0x163f9725) ^ (0xd7eb1064)) + (rb_state17.var_22));
        rb_state17.var_26 = (rb_state17.var_26) + (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_12) : (0xc4a8142c));
        rb_state17.var_27 = (rb_state17.var_27) + (((((0x4baec71e) + (rb_state17.var_15)) - (rb_state17.var_24)) + (rb_state17.var_14)) + (rb_state17.var_23));
        rb_state17.var_28 = (rb_state17.var_28) ^ (((rb_state17.var_16) == (0x0)) ? (rb_state17.var_25) : (0x82a7d79d));
        rb_state17.var_29 = (rb_state17.var_29) + (rb_state17.var_26);
        rb_state17.var_30 = (rb_state17.var_30) - (((rb_state17.var_27) == (0x0)) ? (rb_state17.var_28) : (0x9ad548b8));
        rb_state17.var_31 = (rb_state17.var_31) ^ (((0x47cfda6f) ^ (0x442cb17)) + (rb_state17.var_29));
        rb_state17.var_32 = (rb_state17.var_32) + (((rb_state17.var_17) == (0x0)) ? (rb_state17.var_30) : (0xf425df3f));
        rb_state17.var_33 = (rb_state17.var_33) ^ (((rb_state17.var_18) == (0x0)) ? (rb_state17.var_31) : (0x24b21feb));
        rb_state17.var_34 = (rb_state17.var_34) ^ (((rb_state17.var_32) == (0x0)) ? (rb_state17.var_33) : (0xd36f0315));
        rb_state17.var_21 = (rb_state17.var_21) + (rb_state17.var_34);
        rb_state17.var_5 = rb_state17.var_21;
        pthread_mutex_unlock(&(rb_state17.lock_35));
    }
    #endif
    pkmedian_arg_t *arg = (pkmedian_arg_t *)arg_;
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0xa8db8350) + (rb_state1.var_1));
    #endif
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_2) == (0x0)) {
        rb_state3.var_4 = 0x6d751b3f;
        rb_state3.var_5 = (rb_state3.var_5) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_4) : (0x41b6a361));
        rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_5);
    }
    #endif
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_2) : (0xe803b178));
    #endif
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (0x9f0fff6d);
    if ((rb_state6.var_0) == (0x60f00093)) {
        rb_state6.var_11 = 0xeb258ccb;
        rb_state6.var_12 = (rb_state6.var_12) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_8) : (0xd11e1f50));
        rb_state6.var_13 = (rb_state6.var_13) - (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_7) : (0x69996dc9));
        rb_state6.var_14 = (rb_state6.var_14) - (((((0x7a7ce8a5) + (rb_state6.var_12)) + (rb_state6.var_10)) ^ (0xae6e532e)) ^ (rb_state6.var_6));
        rb_state6.var_15 = (rb_state6.var_15) + (rb_state6.var_13);
        rb_state6.var_16 = (rb_state6.var_16) - (((0x53220a2e) + (rb_state6.var_14)) + (0x37744637));
        rb_state6.var_17 = (rb_state6.var_17) + (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_15) : (0xb719bac3));
        rb_state6.var_18 = (rb_state6.var_18) ^ (((((0x73cdc574) + (rb_state6.var_16)) + (rb_state6.var_12)) ^ (rb_state6.var_17)) + (0x3d86699c));
        rb_state6.var_4 = (rb_state6.var_4) + (rb_state6.var_18);
    }
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state9.lock_10));
        rb_state9.var_4 = rb_state9.var_3;
        pthread_mutex_unlock(&(rb_state9.lock_10));
    }
    if ((rb_state9.var_2) == (0x0)) {
        if (!((rb_state9.var_3) == (rb_state9.var_4))) {
            racebench_trigger(9);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_3) == (0x0)) {
        rb_state11.var_5 = rb_state11.var_4;
    }
    if ((rb_state11.var_3) == (0x0)) {
        if (!((rb_state11.var_4) == (rb_state11.var_5))) {
            racebench_trigger(11);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_2 = (rb_state13.var_2) + ((0x93919995) + (0x513800ef));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_3 = (rb_state15.var_3) - ((0x8f41ba30) + (rb_state15.var_3));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) ^ (0xbe202342);
    rb_state16.var_1 = (rb_state16.var_1) - ((0x96433c65) + (0xdbcd0e48));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + ((0xa4d09f76) - (0x83ffd7bc));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) - (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_1) : (0x94c2672c));
    rb_state18.var_4 = (rb_state18.var_4) - (rb_state18.var_4);
    rb_state18.var_4 = (rb_state18.var_4) + (56136 < rb_input_size ? (uint32_t)rb_input[56136] : 0x2e80b4f0);
    if ((rb_state18.var_2) == (0x56aaf3c0)) {
        rb_state18.var_6 = 0x3567c781;
        rb_state18.var_7 = (rb_state18.var_7) ^ (13346 < rb_input_size ? (uint32_t)rb_input[13346] : 0xb7ea81fa);
        rb_state18.var_8 = (rb_state18.var_8) - (((0x4e1eaeb9) - (0x7a360999)) + (rb_state18.var_6));
        rb_state18.var_9 = (rb_state18.var_9) ^ (((((0xb9bfab5f) ^ (rb_state18.var_5)) ^ (rb_state18.var_8)) ^ (rb_state18.var_7)) ^ (rb_state18.var_7));
        rb_state18.var_10 = (rb_state18.var_10) - (rb_state18.var_8);
        rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0x1b9839bf));
        rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_11);
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
        #ifdef RACEBENCH_BUG_1
        if ((rb_state1.var_2) == (0x0)) {
            rb_state1.var_5 = rb_state1.var_3;
        }
        if ((rb_state1.var_2) == (0x0)) {
            if (!((rb_state1.var_3) == (rb_state1.var_5))) {
                racebench_trigger(1);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) + (rb_state2.var_0);
        rb_state2.var_2 = (rb_state2.var_2) ^ (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_2) : (0x14032548));
        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_3);
        if ((rb_state2.var_3) == (0x0)) {
            pthread_mutex_lock(&(rb_state2.lock_28));
            rb_state2.var_6 = 0x4c9e7935;
            rb_state2.var_7 = 0x2d3088ae;
            rb_state2.var_8 = (rb_state2.var_8) - (0xaa4e0929);
            rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_6) : (0x40b3295a));
            rb_state2.var_10 = (rb_state2.var_10) - (rb_state2.var_4);
            rb_state2.var_11 = (rb_state2.var_11) - (((0xe1982d06) + (rb_state2.var_8)) ^ (0x96828c22));
            rb_state2.var_12 = (rb_state2.var_12) - (((((0xf2542cbe) + (rb_state2.var_9)) ^ (rb_state2.var_5)) ^ (rb_state2.var_10)) - (0xf229b92c));
            rb_state2.var_13 = (rb_state2.var_13) - (((0xd7fe919) ^ (rb_state2.var_11)) ^ (0x2268eb34));
            rb_state2.var_14 = (rb_state2.var_14) ^ (rb_state2.var_12);
            rb_state2.var_15 = (rb_state2.var_15) + (((0xe2bf0b60) + (rb_state2.var_6)) - (rb_state2.var_13));
            rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_14);
            rb_state2.var_17 = (rb_state2.var_17) + (((((0x9f75a006) - (0xe1db40bb)) - (rb_state2.var_16)) + (rb_state2.var_15)) ^ (0x87759aa1));
            rb_state2.var_5 = (rb_state2.var_5) + (rb_state2.var_17);
            rb_state2.var_4 = rb_state2.var_5;
            pthread_mutex_unlock(&(rb_state2.lock_28));
        }
        if ((rb_state2.var_3) == (0x0)) {
            if (!((rb_state2.var_4) == (rb_state2.var_5))) {
                racebench_trigger(2);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) ^ (39730 < rb_input_size ? (uint32_t)rb_input[39730] : 0xdefea618);
        rb_state3.var_1 = (rb_state3.var_1) + (64128 < rb_input_size ? (uint32_t)rb_input[64128] : 0x9104b2d0);
        rb_state3.var_0 = (rb_state3.var_0) ^ ((0x6c0d8ff1) ^ (rb_state3.var_1));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_1) : (0x4a320f70));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - ((0xa0a7983a) ^ (0x1b32dce2));
        rb_state6.var_2 = (rb_state6.var_2) + ((0xeab0bbf6) ^ (rb_state6.var_3));
        rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_2);
        rb_state6.var_3 = (rb_state6.var_3) - (((rb_state6.var_1) == (0x154f440a)) ? (rb_state6.var_0) : (0x2dac968b));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_4 = (rb_state8.var_4) ^ ((0x92f76d0d) - (rb_state8.var_1));
        if ((rb_state8.var_2) == (0x1d6b87e4)) {
            rb_state8.var_5 = rb_state8.var_3;
        }
        if ((rb_state8.var_2) == (0x1d6b87e4)) {
            if (!((rb_state8.var_3) == (rb_state8.var_5))) {
                racebench_trigger(8);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - ((0x1b023ab5) - (rb_state11.var_0));
        rb_state11.var_1 = (rb_state11.var_1) ^ (0x19e85804);
        rb_state11.var_2 = (rb_state11.var_2) - ((0x9c7dc000) + (rb_state11.var_0));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + ((0xc0b5d8af) ^ (rb_state13.var_0));
        rb_state13.var_4 = (rb_state13.var_4) ^ (4106 < rb_input_size ? (uint32_t)rb_input[4106] : 0xdb44b989);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_1) : (0x3df381bb));
        rb_state15.var_0 = (rb_state15.var_0) ^ (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_1) : (0x561db613));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_3 = (rb_state16.var_3) + ((0x748bad3) + (rb_state16.var_3));
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - ((0xea3a1531) - (0xcfd7f4c1));
        rb_state17.var_0 = (rb_state17.var_0) ^ (0x9a2696ae);
        #endif
        arg[i].points = points;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_0);
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (3448 < rb_input_size ? (uint32_t)rb_input[3448] : 0x7d06c8a3);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0x71ad1b07);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_3);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) ^ ((0xc4007246) - (rb_state8.var_0));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_2);
        rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_2);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_3 = (rb_state11.var_3) ^ (rb_state11.var_3);
        rb_state11.var_4 = (rb_state11.var_4) - (18373 < rb_input_size ? (uint32_t)rb_input[18373] : 0x45db3cb);
        rb_state11.var_4 = (rb_state11.var_4) + (0x1bbb3249);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) ^ (rb_state13.var_0);
        rb_state13.var_1 = (rb_state13.var_1) + (13947 < rb_input_size ? (uint32_t)rb_input[13947] : 0xb26d11b9);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ ((0xaa6f5230) - (0x6a163f84));
        rb_state15.var_4 = (rb_state15.var_4) - (((rb_state15.var_1) == (0x0)) ? (rb_state15.var_4) : (0x29d453a5));
        rb_state15.var_2 = (rb_state15.var_2) + (20793 < rb_input_size ? (uint32_t)rb_input[20793] : 0xcaa2750e);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) + (1845 < rb_input_size ? (uint32_t)rb_input[1845] : 0x69aaaaf);
        rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_2) == (0x20d0c7ba)) ? (rb_state17.var_1) : (0x2a85eb2));
        rb_state17.var_1 = (rb_state17.var_1) ^ (6416 < rb_input_size ? (uint32_t)rb_input[6416] : 0xa888070c);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) + (21428 < rb_input_size ? (uint32_t)rb_input[21428] : 0x74e7fc4a);
        rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0xc1)) ? (rb_state18.var_0) : (0x2b4aabc9));
        #endif
        arg[i].kmin = kmin;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (15810 < rb_input_size ? (uint32_t)rb_input[15810] : 0xa08c8b2b);
        rb_state1.var_2 = (rb_state1.var_2) ^ (rb_state1.var_2);
        rb_state1.var_3 = (rb_state1.var_3) + (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_3) : (0x4feef82e));
        rb_state1.var_4 = (rb_state1.var_4) - (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_4) : (0x1c6f7eaa));
        #endif
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) - (37731 < rb_input_size ? (uint32_t)rb_input[37731] : 0xb75db3e3);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) - (0x32daf788);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_2 = (rb_state6.var_2) ^ (rb_state6.var_3);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_3 = (rb_state7.var_3) - (0xb6d8189c);
        if ((rb_state7.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state7.lock_26));
            rb_state7.var_7 = 0xe3a24fdd;
            rb_state7.var_8 = 0xd615b63a;
            rb_state7.var_9 = (rb_state7.var_9) ^ ((0x7c7e0054) - (rb_state7.var_6));
            rb_state7.var_10 = (rb_state7.var_10) - (((0x2967ea90) + (0x1a5514e)) - (rb_state7.var_7));
            rb_state7.var_11 = (rb_state7.var_11) - (rb_state7.var_8);
            rb_state7.var_12 = (rb_state7.var_12) - (rb_state7.var_5);
            rb_state7.var_13 = (rb_state7.var_13) - (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_10) : (0xa3473fcc));
            rb_state7.var_14 = (rb_state7.var_14) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_11) : (0xcf612cde));
            rb_state7.var_15 = (rb_state7.var_15) ^ (((0xa4c2c286) + (0x689963e7)) ^ (rb_state7.var_12));
            rb_state7.var_16 = (rb_state7.var_16) - (((((0xba498c67) + (rb_state7.var_14)) - (rb_state7.var_13)) - (rb_state7.var_8)) ^ (0x934fcfee));
            rb_state7.var_17 = (rb_state7.var_17) ^ (((((0x8dc52ea7) + (0x2509d48f)) + (rb_state7.var_15)) + (0x3a7737cd)) + (rb_state7.var_16));
            rb_state7.var_6 = (rb_state7.var_6) - (rb_state7.var_17);
            rb_state7.var_5 = rb_state7.var_6;
            pthread_mutex_unlock(&(rb_state7.lock_26));
        }
        if ((rb_state7.var_0) == (0x0)) {
            if (!((rb_state7.var_5) == (rb_state7.var_6))) {
                racebench_trigger(7);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_2 = (rb_state8.var_2) ^ ((0x1d6b87e4) ^ (rb_state8.var_2));
        rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_2);
        rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
        #endif
        #ifdef RACEBENCH_BUG_11
        if ((rb_state11.var_3) == (0x0)) {
            rb_state11.var_6 = (rb_state11.var_6) ^ ((0xc91964a3) ^ (0xd010dbcb));
            rb_state11.var_7 = (rb_state11.var_7) + (((((0x526fbb99) ^ (0xbac0692c)) + (0xcd775da3)) ^ (rb_state11.var_5)) - (rb_state11.var_3));
            rb_state11.var_8 = (rb_state11.var_8) + (rb_state11.var_6);
            rb_state11.var_9 = (rb_state11.var_9) + (((0x417725cb) ^ (0x2bcda523)) ^ (rb_state11.var_7));
            rb_state11.var_10 = (rb_state11.var_10) ^ (((0x9e76b6af) ^ (0xa21b247f)) + (rb_state11.var_8));
            rb_state11.var_11 = (rb_state11.var_11) + (((((0x5d0b834b) + (0x5dd4eb7d)) + (rb_state11.var_6)) - (rb_state11.var_9)) - (rb_state11.var_10));
            rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_11);
        }
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) ^ ((0xfa6ec6ef) ^ (rb_state13.var_2));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_3 = (rb_state15.var_3) + (0xe7ddc4e6);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_0);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) - (((rb_state17.var_0) == (0x7fc47679)) ? (rb_state17.var_3) : (0x33b44a13));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) ^ (43449 < rb_input_size ? (uint32_t)rb_input[43449] : 0x5041f6c8);
        if ((rb_state18.var_2) == (0x56aaf3c0)) {
            rb_state18.var_5 = rb_state18.var_3;
        }
        if ((rb_state18.var_2) == (0x56aaf3c0)) {
            if (!((rb_state18.var_3) == (rb_state18.var_5))) {
                racebench_trigger(18);
            }
        }
        #endif
        arg[i].kmax = kmax;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_1) == (0x74)) ? (rb_state2.var_0) : (0xe52830e));
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) - (rb_state3.var_1);
        rb_state3.var_0 = (rb_state3.var_0) - (11440 < rb_input_size ? (uint32_t)rb_input[11440] : 0x1c4375cc);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) + (0x912b29ed);
        rb_state5.var_2 = (rb_state5.var_2) - (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_3) : (0x34361e2c));
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
        if ((rb_state6.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state6.lock_19));
            rb_state6.var_6 = 0x57c29ec5;
            rb_state6.var_7 = 0x1078e6fa;
            rb_state6.var_8 = 0xc9a4011f;
            rb_state6.var_9 = (rb_state6.var_9) - (((((0x7fecfeb1) ^ (rb_state6.var_4)) + (rb_state6.var_7)) ^ (0x801bfa1a)) + (rb_state6.var_8));
            rb_state6.var_10 = (rb_state6.var_10) - (((((0xd1700bb9) ^ (0x51900d6d)) ^ (rb_state6.var_9)) + (rb_state6.var_6)) + (rb_state6.var_5));
            rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_10);
            rb_state6.var_4 = rb_state6.var_5;
            pthread_mutex_unlock(&(rb_state6.lock_19));
        }
        if ((rb_state6.var_0) == (0x60f00093)) {
            if (!((rb_state6.var_4) == (rb_state6.var_5))) {
                racebench_trigger(6);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) - (0x4d435d9b);
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_3 = (rb_state8.var_3) ^ (50769 < rb_input_size ? (uint32_t)rb_input[50769] : 0xb1cadd0f);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - ((0x8a7ea870) - (rb_state9.var_1));
        rb_state9.var_3 = (rb_state9.var_3) ^ ((0x2c301201) - (0x183d7c26));
        rb_state9.var_3 = (rb_state9.var_3) + (65424 < rb_input_size ? (uint32_t)rb_input[65424] : 0xbd02a8ac);
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_4);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_3);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_1 = (rb_state17.var_1) - ((0x8c49c50b) - (0x3bd8f214));
        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_3);
        if ((rb_state17.var_4) == (0xba)) {
            if ((rb_state17.var_5) != (0x0)) {
                if (!((rb_state17.var_5) == (rb_state17.var_21))) {
                    racebench_trigger(17);
                }
            }
        }
        #endif
        arg[i].pid = i;
        #ifdef RACEBENCH_BUG_2
        rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
        rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_1);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_1 = (rb_state3.var_1) ^ ((0x2891942a) + (0x1f5b627a));
        rb_state3.var_2 = (rb_state3.var_2) ^ (0xa4472814);
        #endif
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ (rb_state4.var_0);
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_2 = (rb_state5.var_2) - ((0x9fe6d1a) + (rb_state5.var_0));
        rb_state5.var_1 = (rb_state5.var_1) - (7310 < rb_input_size ? (uint32_t)rb_input[7310] : 0xae77b4b6);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) ^ (13755 < rb_input_size ? (uint32_t)rb_input[13755] : 0x3d3641b3);
        rb_state7.var_4 = (rb_state7.var_4) + (24128 < rb_input_size ? (uint32_t)rb_input[24128] : 0x22892a5d);
        rb_state7.var_4 = (rb_state7.var_4) - ((0x1489ffa9) - (rb_state7.var_2));
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_0 = (rb_state8.var_0) + (38495 < rb_input_size ? (uint32_t)rb_input[38495] : 0x9f575987);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) - (0x64ae663d);
        if ((rb_state9.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state9.lock_10));
            rb_state9.var_5 = 0x198de67e;
            rb_state9.var_6 = (rb_state9.var_6) - (0x19900c4f);
            rb_state9.var_7 = (rb_state9.var_7) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_4) : (0x7dfc6615));
            rb_state9.var_8 = (rb_state9.var_8) ^ (((((0x82f6f57d) - (0x99bc9ac0)) - (rb_state9.var_6)) + (rb_state9.var_5)) ^ (0xa2930b84));
            rb_state9.var_9 = (rb_state9.var_9) - (((((0xd049d4a9) - (0xaa22b6ee)) - (rb_state9.var_7)) ^ (rb_state9.var_8)) + (0xad051881));
            rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_9);
            pthread_mutex_unlock(&(rb_state9.lock_10));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) ^ (rb_state11.var_1);
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) + ((0x96a7f32d) + (rb_state15.var_0));
        if ((rb_state15.var_3) == (0x4079cf9c)) {
            rb_state15.var_6 = 0xb80c43f8;
            rb_state15.var_7 = (rb_state15.var_7) ^ (((0x8ea3ac32) + (rb_state15.var_6)) - (rb_state15.var_7));
            rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xade54a40) - (rb_state15.var_7)) - (0xa7470b8d)) - (rb_state15.var_5)) ^ (rb_state15.var_8));
            rb_state15.var_4 = (rb_state15.var_4) + (rb_state15.var_8);
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) - (rb_state18.var_2);
        #endif
        arg[i].kfinal = kfinal;

        #ifdef RACEBENCH_BUG_1
        rb_state1.var_1 = (rb_state1.var_1) + (0x71be9b4d);
        rb_state1.var_4 = (rb_state1.var_4) ^ (0x40dd19bd);
        rb_state1.var_0 = (rb_state1.var_0) ^ (15114 < rb_input_size ? (uint32_t)rb_input[15114] : 0x5072ee01);
        rb_state1.var_1 = (rb_state1.var_1) - (0xcd78da47);
        #endif
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) ^ (((rb_state3.var_2) == (0xa4472814)) ? (rb_state3.var_2) : (0x1392e480));
        #endif
        #ifdef RACEBENCH_BUG_5
        rb_state5.var_3 = (rb_state5.var_3) ^ (((rb_state5.var_1) == (0xffffffc3)) ? (rb_state5.var_0) : (0xee0e25f1));
        if ((rb_state5.var_1) == (0xffffffc3)) {
            rb_state5.var_5 = 0x77ccbcb6;
            rb_state5.var_6 = 0x778eef56;
            rb_state5.var_7 = 0x2440d4b2;
            rb_state5.var_8 = (rb_state5.var_8) + (rb_state5.var_4);
            rb_state5.var_9 = (rb_state5.var_9) - (((((0x548ef335) - (rb_state5.var_5)) ^ (rb_state5.var_6)) ^ (rb_state5.var_5)) - (rb_state5.var_7));
            rb_state5.var_10 = (rb_state5.var_10) ^ (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0xd163e32d));
            rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_8) : (0xe6df65aa));
            rb_state5.var_12 = (rb_state5.var_12) - (((0x892fce7b) + (rb_state5.var_9)) ^ (rb_state5.var_9));
            rb_state5.var_13 = (rb_state5.var_13) - (((((0xcdc042bd) ^ (0x5e7bcf1)) + (rb_state5.var_10)) ^ (rb_state5.var_10)) + (rb_state5.var_11));
            rb_state5.var_14 = (rb_state5.var_14) + (rb_state5.var_12);
            rb_state5.var_15 = (rb_state5.var_15) ^ (((((0x219f4929) + (0xb68eb3e)) - (rb_state5.var_13)) ^ (rb_state5.var_11)) + (rb_state5.var_14));
            rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_15);
        }
        #endif
        #ifdef RACEBENCH_BUG_8
        rb_state8.var_4 = (rb_state8.var_4) + ((0x3e1114c7) - (0x5d7ff461));
        rb_state8.var_4 = (rb_state8.var_4) ^ ((0xbdf2e85e) ^ (0x8ae9ebd5));
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_1 = (rb_state9.var_1) ^ (0xa723c6b1);
        rb_state9.var_0 = (rb_state9.var_0) + (rb_state9.var_1);
        rb_state9.var_0 = (rb_state9.var_0) ^ (5143 < rb_input_size ? (uint32_t)rb_input[5143] : 0x2377257c);
        rb_state9.var_3 = (rb_state9.var_3) ^ ((0x1c7ab306) + (0xe6a1f4e2));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_3) == (0x1ea75c60)) ? (rb_state13.var_2) : (0x9fb15663));
        rb_state13.var_4 = (rb_state13.var_4) + ((0xb5a61989) - (0x4285d719));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_4 = (rb_state15.var_4) - ((0x7f72b46e) ^ (0x6c37fb18));
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_1 = (rb_state16.var_1) ^ (56367 < rb_input_size ? (uint32_t)rb_input[56367] : 0xb6787455);
        if ((rb_state16.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_41));
            if ((rb_state16.var_4) != (0x0)) {
                if (!((rb_state16.var_4) == (rb_state16.var_22))) {
                    racebench_trigger(16);
                }
            }
            pthread_mutex_unlock(&(rb_state16.lock_41));
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_2 = (rb_state18.var_2) ^ (0x56aaf3c0);
        #endif
        arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_0 = (rb_state3.var_0) + (0x838ba7aa);
        if ((rb_state3.var_2) == (0x0)) {
            rb_state3.var_3 = rb_state3.var_0;
        }
        if ((rb_state3.var_2) == (0x0)) {
            pthread_mutex_lock(&(rb_state3.lock_6));
            if (!((rb_state3.var_0) == (rb_state3.var_3))) {
                racebench_trigger(3);
            }
            pthread_mutex_unlock(&(rb_state3.lock_6));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) + ((0x68cc899f) - (rb_state6.var_1));
        rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_1) == (0x68cc899f)) ? (rb_state6.var_2) : (0x152ae6ae));
        rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_3);
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) + (rb_state7.var_1);
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_2 = (rb_state11.var_2) ^ (((rb_state11.var_1) == (0x19e8580b)) ? (rb_state11.var_0) : (0x2181e616));
        #endif
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_1 = (rb_state15.var_1) - (0xca35195e);
        #endif
        #ifdef RACEBENCH_BUG_16
        rb_state16.var_2 = (rb_state16.var_2) - (63282 < rb_input_size ? (uint32_t)rb_input[63282] : 0x2d7c7e87);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) ^ (58170 < rb_input_size ? (uint32_t)rb_input[58170] : 0x4aaf6c64);
        rb_state18.var_3 = (rb_state18.var_3) ^ ((0x85f666f0) - (rb_state18.var_3));
        #endif
        pthread_create(threads + i, NULL, localSearchSub, (void *)&arg[i]);
#else
        localSearchSub(&arg[0]);
#endif
    }

#ifdef ENABLE_THREADS
    for (int i = 0; i < nproc; i++) {
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_1);
        if ((rb_state13.var_2) == (0xae5ccf8c)) {
            pthread_mutex_lock(&(rb_state13.lock_39));
            rb_state13.var_7 = 0x28a0b699;
            rb_state13.var_8 = (rb_state13.var_8) ^ (rb_state13.var_7);
            rb_state13.var_9 = (rb_state13.var_9) ^ (52689 < rb_input_size ? (uint32_t)rb_input[52689] : 0x95c4bcc9);
            rb_state13.var_10 = (rb_state13.var_10) + (rb_state13.var_7);
            rb_state13.var_11 = (rb_state13.var_11) - (rb_state13.var_6);
            rb_state13.var_12 = (rb_state13.var_12) + (((((0x9740bb4c) + (0xaa1f2b29)) - (0x93708149)) - (rb_state13.var_5)) ^ (rb_state13.var_8));
            rb_state13.var_13 = (rb_state13.var_13) ^ (((rb_state13.var_8) == (0x28a0b699)) ? (rb_state13.var_9) : (0x9482caf));
            rb_state13.var_14 = (rb_state13.var_14) - (((rb_state13.var_9) == (0x2)) ? (rb_state13.var_10) : (0xd0efb639));
            rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_11);
            rb_state13.var_16 = (rb_state13.var_16) - (((0xa8fed5dd) - (rb_state13.var_12)) - (0xf750a664));
            rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_10) == (0x28a0b699)) ? (rb_state13.var_13) : (0xbdb8d5fc));
            rb_state13.var_18 = (rb_state13.var_18) - (((0xb6ff18a2) + (0x14f47cdf)) + (rb_state13.var_14));
            rb_state13.var_19 = (rb_state13.var_19) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0x43a8d166));
            rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_17) : (0x72b09437));
            rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_18);
            rb_state13.var_22 = (rb_state13.var_22) + (((rb_state13.var_19) == (0x2c5e5bc4)) ? (rb_state13.var_20) : (0xe44b3de0));
            rb_state13.var_23 = (rb_state13.var_23) + (rb_state13.var_21);
            rb_state13.var_24 = (rb_state13.var_24) - (((rb_state13.var_12) == (0x854fd3b5)) ? (rb_state13.var_22) : (0x381c2850));
            rb_state13.var_25 = (rb_state13.var_25) ^ (((((0x2b428db2) - (0x369eb3af)) ^ (rb_state13.var_23)) + (rb_state13.var_13)) + (rb_state13.var_24));
            rb_state13.var_6 = (rb_state13.var_6) - (rb_state13.var_25);
            rb_state13.var_5 = rb_state13.var_6;
            pthread_mutex_unlock(&(rb_state13.lock_39));
        }
        if ((rb_state13.var_2) == (0xae5ccf8c)) {
            if (!((rb_state13.var_5) == (rb_state13.var_6))) {
                racebench_trigger(13);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_19
        rb_state19.var_0 = (rb_state19.var_0) - (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0x238db11f));
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